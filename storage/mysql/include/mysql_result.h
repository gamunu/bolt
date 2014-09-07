#pragma once
#include <unordered_map>
#include <mysql_table_entity.h>

namespace bolt
{
	namespace storage
	{
		namespace mysql
		{
			/// <summary>
			/// Represents the result of a table operation.
			/// </summary>
			/// <remarks>The <see cref="azure::storage::table_result"/> class encapsulates the HTTP response and any query 
			/// results returned for a particular <see cref="azure::storage::table_operation"/>.</remarks>
			class mysql_result
			{
			public:

				/// <summary>
				/// Initializes a new instance of the <see cref="azure::storage::table_result"/> class.
				/// </summary>
				mysql_result()
					: m_http_status_code(0)
				{
				}

				/// <summary>
				/// Gets a <see cref="table_entity" /> object returned as part of a <see cref="azure::storage::table_result" /> object.
				/// </summary>
				/// <returns>A <see cref="table_entity" /> object.</returns>
				const mysql::mysql_table_entity& entity() const
				{
					return m_entity;
				}

				/// <summary>
				/// Sets a <see cref="table_entity" /> object returned as part of a <see cref="azure::storage::table_result" /> object.
				/// </summary>
				/// <param name="value">A <see cref="table_entity" /> object.</param>
				void set_entity(mysql::mysql_table_entity value)
				{
					m_entity = std::move(value);
				}

				/// <summary>
				/// Gets the HTTP status code for a <see cref="azure::storage::table_result" /> object.
				/// </summary>
				/// <returns>The HTTP status code.</returns>
				int http_status_code() const
				{
					return m_http_status_code;
				}

				/// <summary>
				/// Sets the HTTP status code for a <see cref="azure::storage::table_result" /> object.
				/// </summary>
				/// <param name="value">The HTTP status code.</param>
				void set_http_status_code(int value)
				{
					m_http_status_code = value;
				}

				/// <summary>
				/// Gets the ETag for a <see cref="azure::storage::table_result" /> object.
				/// </summary>
				/// <returns>The ETag, as a string.</returns>
				const utility::string_t& etag() const
				{
					return m_etag;
				}

				/// <summary>
				/// Sets the ETag for a <see cref="azure::storage::table_result" /> object.
				/// </summary>
				/// <param name="value">The ETag, as a string.</param>
				void set_etag(utility::string_t value)
				{
					m_etag = std::move(value);
				}

			private:

				mysql::mysql_table_entity m_entity;
				int m_http_status_code;
				utility::string_t m_etag;
			};
		}
	}
}