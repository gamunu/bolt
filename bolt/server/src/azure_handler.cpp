#include <azure_handler.hpp>
#include <message_types.hpp>
#include <url_utils.hpp>
#include <header_utils.hpp>
#include "azure_query.h"
#include "azure_table.h"
#include <signature.hpp>
#include "was/table.h"
#include <metadata.hpp>
#include <ahttppost.hpp>
#include <ahttpdelete.hpp>
using namespace azure;
using namespace storage;
using namespace bolt::storage;
using namespace bolt::auth;

AzureHandler::AzureHandler(const http_request &request, const method method) : m_http_request(request)
{
	m_method = method;
}

void AzureHandler::InitializeHandlers()
{
	if (m_method == methods::GET)
	{
		HandleGet();
	}
	else if (m_method == methods::POST)
	{
		HandlePost();
	}
	else if (m_method == methods::DEL)
	{
		HandleDelete();
	}
	else if (m_method == methods::PATCH)
	{
		HandlePatch();
	}
}

/// <summary>
/// Handles the get.
/// </summary>
void AzureHandler::HandleGet()
{
	auto paths = UrlUtils::splitUri(m_http_request);
	auto query = UrlUtils::splitQueryString(m_http_request);


	//Requested table name
	string_t table_name;
	string_t rowkey;
	string_t partitionkey;


	if (paths.empty())
	{
		m_http_request.reply(status_codes::BadRequest);
		return;
	}

	if (UrlUtils::hasTables(paths))
	{
		m_http_request.reply(status_codes::OK, Metadata::getAzureTables());
		return;
	}

	if (UrlUtils::getTableNameWithKeys(paths, table_name, rowkey, partitionkey))
	{
		//if partition key and row key found we are good to go
		if (!(rowkey.empty() && partitionkey.empty()))
		{
			m_http_request.reply(status_codes::OK, Metadata::getAzureEntity(table_name, rowkey, partitionkey));
			return;
		}
	}

	if (UrlUtils::getTableNameWithoutKeys(paths, table_name))
	{
		m_http_request.reply(status_codes::OK, Metadata::getAzureEntities(table_name, query));
		return;
	}

	m_http_request.reply(status_codes::BadRequest);
}

/// <summary>
/// Handles the post.
/// </summary>
void AzureHandler::HandlePost()
{
	http_headers headers = m_http_request.headers();
	auto paths = UrlUtils::splitUri(m_http_request);

	if (paths.empty())
	{
		m_http_request.reply(status_codes::BadRequest);
		return;
	}

	//Requested table name
	string_t table_name;
	string_t table_namespace;

	if (!UrlUtils::getTableNamespace(paths, table_namespace))
	{
		m_http_request.reply(status_codes::Unauthorized);
		return;
	}

	// continue when the response is available
	// and continue when the JSON value is available
	if (UrlUtils::hasTables(paths))
	{
		json::value obj;
		try
		{
			obj = m_http_request.extract_json().get();
		}
		catch (const json::json_exception &e) {
			m_http_request.reply(status_codes::BadRequest);
			return;
		}
		json::value meta = AHttpPost::createTable(obj);

		if (!meta.is_null())
		{
			m_http_request.reply(status_codes::Created, meta);
			return;
		}
		m_http_request.reply(status_codes::NoContent, meta);
		return;
	}
	// get the JSON value from the task and display content from it

	if (!UrlUtils::getTableNameWithoutKeys(paths, table_name))
	{
		m_http_request.reply(status_codes::BadRequest);
		return;
	}
	json::value obj;
	try
	{
		obj = m_http_request.extract_json().get();
	}
	catch (const json::json_exception &e) {
		m_http_request.reply(status_codes::BadRequest);
		return;
	}
	if (!obj.is_null())
	{
		if (obj.is_object())
		{
			boltazure::AzureTable azure_table = boltazure::AzureTable(table_name);
			boltazure::AzureEntity entity = boltazure::AzureEntity(azure_table.getTable());
			vector<string_t> clmns;

			//map containing the properties received from the http request
			auto &property_map = obj.as_object();

			for (auto& pair : obj.as_object())
			{
				clmns.push_back(pair.first);
			}

			//find partition key in the map
			auto partitionkey_find = property_map.find(U("PartitionKey"));
			//find row key in the map
			auto rowkey_find = property_map.find(U("RowKey"));

			//if partition key and row key found we are good to go
			if (partitionkey_find != property_map.end() && rowkey_find != property_map.end())
			{
				//Create new entity using found
				entity.createEntity(
					partitionkey_find->second.as_string(), //Remove double quotes
					rowkey_find->second.as_string(), //Remove double quotes
					obj.as_object().size()
					);

				for (auto& pair : property_map)
				{
					if (pair.second == partitionkey_find->second || rowkey_find->second == pair.second)
						continue;
					insetKeyValuePropery(entity, pair.first, pair.second);
				}

				if (entity.executeEntity())
				{
					m_http_request.reply(status_codes::Created);
					return;
				}
				else
				{
					m_http_request.reply(status_codes::NotModified);
					return;
				}
			}
		}
	}
	else {
		m_http_request.reply(status_codes::BadRequest);
		return;
	}

	m_http_request.reply(status_codes::BadRequest);
}

void AzureHandler::HandleDelete()
{
	auto paths = UrlUtils::splitUri(m_http_request);
	auto query = UrlUtils::splitQueryString(m_http_request);

	//Requested table name
	string_t table_name;

	//Table name checks for "Table('table_name')" 
	//and assings the table name to the given variable
	if (UrlUtils::getTableName(paths, table_name))
	{
		boltazure::AzureTable table = boltazure::AzureTable();
		if (table.deleteTable(table_name))
		{
			m_http_request.reply(status_codes::NoContent);
			return;
		}
		m_http_request.reply(status_codes::NoContent);
		return;
	}

	if (UrlUtils::getTableNameWithoutKeys(paths, table_name))
	{
		if (AHttpDelete::deleteEntities(table_name, query))
		{
			m_http_request.reply(status_codes::NoContent);
			return;
		}
		m_http_request.reply(status_codes::NotModified);
		return;
	}

	m_http_request.reply(status_codes::BadRequest);
}

void AzureHandler::HandlePatch()
{
	http_headers headers = m_http_request.headers();
	auto paths = UrlUtils::splitUri(m_http_request);

	//Requested table name
	string_t table_name;
	string_t partition_key;
	string_t row_key;

	if (paths.empty())
	{
		m_http_request.reply(status_codes::BadRequest);
		return;
	}

	if (!UrlUtils::getTableNameWithKeys(paths, table_name, row_key, partition_key))
	{
		m_http_request.reply(status_codes::BadRequest);
		return;
	}

	// get the JSON value from the task and display content from it

	json::value obj;
	try
	{
		obj = m_http_request.extract_json().get();
	}
	catch (const json::json_exception &e) {
		m_http_request.reply(status_codes::BadRequest);
		return;
	}
	if (!obj.is_null())
	{
		if (obj.is_object())
		{
			boltazure::AzureTable azure_table = boltazure::AzureTable(table_name);
			boltazure::AzureEntity entity = boltazure::AzureEntity(azure_table.getTable());

			//map containing the properties received from the http request
			auto &property_map = obj.as_object();

			//Create new entity using found
			entity.createEntity(partition_key, row_key, obj.as_object().size());

			for (auto& pair : property_map)
			{
				insetKeyValuePropery(entity, pair.first, pair.second);
			}

			if (entity.patchEntity())
			{
				m_http_request.reply(status_codes::Created, Metadata::getAzureEntity(table_name, row_key, partition_key));
				return;
			}
			else
			{
				m_http_request.reply(status_codes::NotModified);
				return;
			}
		}
	}

	m_http_request.reply(status_codes::BadRequest);
}

void AzureHandler::insetKeyValuePropery(boltazure::AzureEntity &entity, string_t key, json::value value)
{
	if (value.is_string())
	{
		entity.insert(key, value.as_string());
	}
	else if (value.is_double())
	{
		entity.insert(key, value.as_double());
	}
	else if (value.is_boolean())
	{
		entity.insert(key, value.as_bool());
	}
	else if (value.is_integer())
	{
		entity.insert(key, value.as_integer());
	}
}