// -----------------------------------------------------------------------------------------
// <copyright file="cloud_page_blob.cpp" company="Microsoft">
//    Copyright 2013 Microsoft Corporation
//
//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
//      http://www.apache.org/licenses/LICENSE-2.0
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.
// </copyright>
// -----------------------------------------------------------------------------------------

#include "stdafx.h"
#include "wascore/protocol.h"
#include "wascore/protocol_xml.h"
#include "wascore/blobstreams.h"

namespace azure {
	namespace storage {
		pplx::task<void> cloud_page_blob::clear_pages_async(int64_t start_offset, int64_t length, const access_condition& condition, const blob_request_options& options, operation_context context)
		{
			assert_no_snapshot();
			blob_request_options modified_options(options);
			modified_options.apply_defaults(service_client().default_request_options(), type());

			auto end_offset = start_offset + length - 1;
			page_range range(start_offset, end_offset);

			auto properties = m_properties;

			auto command = std::make_shared<core::storage_command<void>>(uri());
			command->set_build_request(std::bind(protocol::put_page, range, page_write::clear, utility::string_t(), condition, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
			command->set_authentication_handler(service_client().authentication_handler());
			command->set_preprocess_response([properties](const web::http::http_response& response, operation_context context)
			{
				protocol::preprocess_response(response, context);

				auto parsed_properties = protocol::blob_response_parsers::parse_blob_properties(response);
				properties->update_etag_and_last_modified(parsed_properties);
				properties->update_page_blob_sequence_number(parsed_properties);
			});
			return core::executor<void>::execute_async(command, modified_options, context);
		}

		pplx::task<void> cloud_page_blob::upload_pages_async(concurrency::streams::istream page_data, int64_t start_offset, const utility::string_t& content_md5, const access_condition& condition, const blob_request_options& options, operation_context context)
		{
			assert_no_snapshot();
			blob_request_options modified_options(options);
			modified_options.apply_defaults(service_client().default_request_options(), type());

			auto properties = m_properties;
			bool needs_md5 = content_md5.empty() && modified_options.use_transactional_md5();

			auto command = std::make_shared<core::storage_command<void>>(uri());
			command->set_authentication_handler(service_client().authentication_handler());
			command->set_preprocess_response([properties](const web::http::http_response& response, operation_context context)
			{
				protocol::preprocess_response(response, context);

				auto parsed_properties = protocol::blob_response_parsers::parse_blob_properties(response);
				properties->update_etag_and_last_modified(parsed_properties);
				properties->update_page_blob_sequence_number(parsed_properties);
			});
			return core::istream_descriptor::create(page_data, needs_md5).then([command, context, start_offset, content_md5, modified_options, condition](core::istream_descriptor request_body) -> pplx::task < void >
			{
				const utility::string_t& md5 = content_md5.empty() ? request_body.content_md5() : content_md5;
				auto end_offset = start_offset + request_body.length() - 1;
				page_range range(start_offset, end_offset);
				command->set_build_request(std::bind(protocol::put_page, range, page_write::update, md5, condition, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
				command->set_request_body(request_body);
				return core::executor<void>::execute_async(command, modified_options, context);
			});
		}

		pplx::task<concurrency::streams::ostream> cloud_page_blob::open_write_async(const access_condition& condition, const blob_request_options& options, operation_context context)
		{
			assert_no_snapshot();
			blob_request_options modified_options(options);
			modified_options.apply_defaults(service_client().default_request_options(), type(), false);

			if (modified_options.store_blob_content_md5())
			{
				throw std::logic_error(protocol::error_md5_not_possible);
			}

			auto instance = std::make_shared<cloud_page_blob>(*this);
			return instance->download_attributes_async(condition, modified_options, context).then([instance, condition, modified_options, context]() -> concurrency::streams::ostream
			{
				return core::cloud_page_blob_ostreambuf(instance, instance->properties().size(), condition, modified_options, context).create_ostream();
			});
		}

		pplx::task<concurrency::streams::ostream> cloud_page_blob::open_write_async(utility::size64_t size, const access_condition& condition, const blob_request_options& options, operation_context context)
		{
			assert_no_snapshot();
			blob_request_options modified_options(options);
			modified_options.apply_defaults(service_client().default_request_options(), type(), false);

			auto instance = std::make_shared<cloud_page_blob>(*this);
			return instance->create_async(size, condition, modified_options, context).then([instance, size, condition, modified_options, context]() -> concurrency::streams::ostream
			{
				return core::cloud_page_blob_ostreambuf(instance, size, condition, modified_options, context).create_ostream();
			});
		}

		pplx::task<void> cloud_page_blob::upload_from_stream_async(concurrency::streams::istream source, utility::size64_t length, const access_condition& condition, const blob_request_options& options, operation_context context)
		{
			assert_no_snapshot();
			blob_request_options modified_options(options);
			modified_options.apply_defaults(service_client().default_request_options(), type());

			if (length == std::numeric_limits<utility::size64_t>::max())
			{
				length = core::get_remaining_stream_length(source);
				if (length == std::numeric_limits<utility::size64_t>::max())
				{
					throw std::logic_error(protocol::error_page_blob_size_unknown);
				}
			}

			return open_write_async(length, condition, modified_options, context).then([source, length](concurrency::streams::ostream blob_stream) -> pplx::task < void >
			{
				return core::stream_copy_async(source, blob_stream, length).then([blob_stream](utility::size64_t) -> pplx::task < void >
				{
					return blob_stream.close();
				});
			});
		}

		pplx::task<void> cloud_page_blob::upload_from_file_async(const utility::string_t &path, const access_condition& condition, const blob_request_options& options, operation_context context)
		{
			auto instance = std::make_shared<cloud_page_blob>(*this);
			return concurrency::streams::file_stream<uint8_t>::open_istream(path).then([instance, condition, options, context](concurrency::streams::istream stream) -> pplx::task < void >
			{
				return instance->upload_from_stream_async(stream, condition, options, context).then([stream](pplx::task<void> upload_task) -> pplx::task < void >
				{
					return stream.close().then([upload_task]()
					{
						upload_task.wait();
					});
				});
			});
		}

		pplx::task<void> cloud_page_blob::create_async(utility::size64_t size, const access_condition& condition, const blob_request_options& options, operation_context context)
		{
			assert_no_snapshot();
			blob_request_options modified_options(options);
			modified_options.apply_defaults(service_client().default_request_options(), type());

			auto properties = m_properties;

			auto command = std::make_shared<core::storage_command<void>>(uri());
			command->set_build_request(std::bind(protocol::put_page_blob, size, *properties, metadata(), condition, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
			command->set_authentication_handler(service_client().authentication_handler());
			command->set_preprocess_response([properties, size](const web::http::http_response& response, operation_context context)
			{
				protocol::preprocess_response(response, context);
				properties->update_etag_and_last_modified(protocol::blob_response_parsers::parse_blob_properties(response));
				properties->m_size = size;
			});
			return core::executor<void>::execute_async(command, modified_options, context);
		}

		pplx::task<void> cloud_page_blob::resize_async(utility::size64_t size, const access_condition& condition, const blob_request_options& options, operation_context context)
		{
			assert_no_snapshot();
			blob_request_options modified_options(options);
			modified_options.apply_defaults(service_client().default_request_options(), type());

			auto properties = m_properties;

			auto command = std::make_shared<core::storage_command<void>>(uri());
			command->set_build_request(std::bind(protocol::resize_page_blob, size, condition, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
			command->set_authentication_handler(service_client().authentication_handler());
			command->set_preprocess_response([properties, size](const web::http::http_response& response, operation_context context)
			{
				protocol::preprocess_response(response, context);

				auto parsed_properties = protocol::blob_response_parsers::parse_blob_properties(response);
				properties->update_etag_and_last_modified(parsed_properties);
				properties->update_page_blob_sequence_number(parsed_properties);
				properties->m_size = size;
			});
			return core::executor<void>::execute_async(command, modified_options, context);
		}

		pplx::task<void> cloud_page_blob::set_sequence_number_async(const azure::storage::sequence_number& sequence_number, const access_condition& condition, const blob_request_options& options, operation_context context)
		{
			assert_no_snapshot();
			blob_request_options modified_options(options);
			modified_options.apply_defaults(service_client().default_request_options(), type());

			auto properties = m_properties;

			auto command = std::make_shared<core::storage_command<void>>(uri());
			command->set_build_request(std::bind(protocol::set_page_blob_sequence_number, sequence_number, condition, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
			command->set_authentication_handler(service_client().authentication_handler());
			command->set_preprocess_response([properties](const web::http::http_response& response, operation_context context)
			{
				protocol::preprocess_response(response, context);

				auto parsed_properties = protocol::blob_response_parsers::parse_blob_properties(response);
				properties->update_etag_and_last_modified(parsed_properties);
				properties->update_page_blob_sequence_number(parsed_properties);
			});
			return core::executor<void>::execute_async(command, modified_options, context);
		}

		pplx::task<std::vector<page_range>> cloud_page_blob::download_page_ranges_async(utility::size64_t offset, utility::size64_t length, const access_condition& condition, const blob_request_options& options, operation_context context) const
		{
			blob_request_options modified_options(options);
			modified_options.apply_defaults(service_client().default_request_options(), type());

			auto properties = m_properties;

			auto command = std::make_shared<core::storage_command<std::vector<page_range>>>(uri());
			command->set_build_request(std::bind(protocol::get_page_ranges, offset, length, snapshot_time(), condition, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
			command->set_authentication_handler(service_client().authentication_handler());
			command->set_location_mode(core::command_location_mode::primary_or_secondary);
			command->set_preprocess_response([properties](const web::http::http_response& response, operation_context context) -> std::vector < page_range >
			{
				protocol::preprocess_response(response, context);

				auto parsed_properties = protocol::blob_response_parsers::parse_blob_properties(response);
				properties->update_etag_and_last_modified(parsed_properties);
				properties->update_size(parsed_properties);
				return std::vector<page_range>();
			});
			command->set_postprocess_response([](const web::http::http_response& response, const request_result&, const core::ostream_descriptor&, operation_context context) -> pplx::task < std::vector<page_range> >
			{
				protocol::page_list_reader reader(response.body());
				return pplx::task_from_result(reader.extract_result());
			});
			return core::executor<std::vector<page_range>>::execute_async(command, modified_options, context);
		}
	}
} // namespace azure::storage