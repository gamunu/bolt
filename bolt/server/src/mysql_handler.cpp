#include <mysql_handler.hpp>
#include <url_utils.hpp>
#include <message_types.hpp>
#include <mysql_query.h>
#include <mysql_delete.h>
#include <signature.hpp>
#include <header_utils.hpp>
#include <mysql_table.h>
#include <metadata.hpp>
#include <mhttppost.hpp>
#include <mhttpdelete.hpp>

using namespace bolt::auth;

MysqlHandler::MysqlHandler(const http_request &request, method method) : m_http_request(request)
{
	m_method = method;
}

void MysqlHandler::InitializeHandlers()
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
void MysqlHandler::HandleGet()
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
		m_http_request.reply(status_codes::OK, Metadata::getMysqlTables());
		return;
	}

	if (UrlUtils::hasAdministration(paths))
	{
		json::value result;
		if (Metadata::getAdministration(paths, result))
		{
			m_http_request.reply(status_codes::OK, result);
			return;
		}
		m_http_request.reply(status_codes::BadRequest);
		return;
	}

	if (UrlUtils::getTableNameWithKeys(paths, table_name, rowkey, partitionkey))
	{
		//if partition key and row key found we are good to go
		if (!(rowkey.empty() && partitionkey.empty()))
		{
			m_http_request.reply(status_codes::OK, Metadata::getMysqlEntity(table_name, rowkey, partitionkey));
			return;
		}
	}

	if (UrlUtils::getTableNameWithoutKeys(paths, table_name))
	{
		m_http_request.reply(status_codes::OK, Metadata::getMysqlEntities(table_name, query));
		return;
	}

	m_http_request.reply(status_codes::BadRequest);
}

/// <summary>
/// Handles the post.
/// </summary>
void MysqlHandler::HandlePost()
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

	if (UrlUtils::hasQuery(paths))
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

		if (obj.is_object())
		{
			auto &query_map = obj.as_object();
			json::value result;
			if (Metadata::getMysqlQueryResults(query_map, result))
			{
				m_http_request.reply(status_codes::OK, result);
				return;
			}
			else
			{
				m_http_request.reply(status_codes::BadRequest);
			}
		}
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
		json::value meta = MHttpPost::createTable(obj);

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
	if (obj.is_object())
	{
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
			MysqlEntity entity = MysqlEntity(table_name, partitionkey_find->second.as_string(), //Remove double quotes
				rowkey_find->second.as_string(), //Remove double quotes
				obj.as_object().size()
				);
			//Create new entity using found

			for (auto& pair : property_map)
			{
				if (pair.first == U("PartitionKey") || pair.first == U("RowKey"))
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

	m_http_request.reply(status_codes::BadRequest);
}

void MysqlHandler::insetKeyValuePropery(MysqlEntity &entity, string_t key, json::value value)
{
	if (value.is_string()) //If value a string
	{
		entity.insertString(key, value.as_string());
	}
	else if (value.is_double()) //If value a double
		entity.insertDouble(key, value.as_double());
	else if (value.is_boolean()) //If value a boolean
		entity.insertBoolean(key, value.as_bool());
	else if (value.is_integer()) //If value an integer
		entity.insertInt64(key, value.as_integer());
}

void MysqlHandler::HandleDelete()
{
	auto paths = UrlUtils::splitUri(m_http_request);
	auto query = UrlUtils::splitQueryString(m_http_request);

	if (paths.empty())
	{
		m_http_request.reply(status_codes::BadRequest);
		return;
	}

	//Requested table name
	string_t table_name;

	//Table name checks for "Table('table_name')" 
	//and assings the table name to the given variable
	if (UrlUtils::getTableName(paths, table_name))
	{
		unique_ptr<MysqlTable> table(new MysqlTable);
		if (table->deleteTable(table_name))
		{
			m_http_request.reply(status_codes::NoContent);
			return;

		}
		m_http_request.reply(status_codes::NoContent);
		return;
	}

	if (UrlUtils::getTableNameWithoutKeys(paths, table_name))
	{
		if (MHttpDelete::deleteEntities(table_name, query))
		{
			m_http_request.reply(status_codes::NoContent);
			return;
		}
		m_http_request.reply(status_codes::NotModified);
		return;
	}
	m_http_request.reply(status_codes::BadRequest);
}

void MysqlHandler::HandlePatch()
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
	string_t paritition_key;
	string_t row_key;

	if (!UrlUtils::getTableNameWithKeys(paths, table_name, row_key, paritition_key))
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
	if (obj.is_object())
	{
		vector<string_t> clmns;

		//map containing the properties received from the http request
		auto &property_map = obj.as_object();

		for (auto& pair : obj.as_object())
		{
			clmns.push_back(pair.first);
		}

		auto entity = MysqlEntity(table_name, paritition_key, row_key, obj.as_object().size());

		for (auto& pair : property_map)
		{
			insetKeyValuePropery(entity, pair.first, pair.second);
		}

		if (entity.patchEntity())
		{
			m_http_request.reply(status_codes::NoContent, Metadata::getMysqlEntity(table_name, paritition_key, row_key));
			return;
		}
		else
		{
			m_http_request.reply(status_codes::NotModified);
			return;
		}
	}
	m_http_request.reply(status_codes::BadRequest);
}