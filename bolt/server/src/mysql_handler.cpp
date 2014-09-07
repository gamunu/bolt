#include <mysql_handler.hpp>
#include <url_utils.hpp>
#include <message_types.hpp>
#include <mysql_query.h>
#include <string_utils.hpp>
#include <mysql_delete.h>
#include <signature.hpp>
#include <header_utils.hpp>
#include <mysql_table.h>
#include <metadata.hpp>
#include <mhttppost.hpp>

using namespace bolt::auth;

MysqlHandler::MysqlHandler(const http_request &request, method method) : m_http_request(request)
{
	m_method = method;

	permissions = make_unique<Permissions>();
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
	http_headers headers = m_http_request.headers();

	auto paths = UrlUtils::splitUri(m_http_request);
	auto query = UrlUtils::splitQueryString(m_http_request);
	auto select = query.find(SELECT);

	//userid of the user account
	string_t account_name;
	//Requested table name
	string_t table_name;
	string_t rowkey;
	string_t partitionkey;


	if (paths.empty())
	{
		m_http_request.reply(status_codes::BadRequest);
		return;
	}

	unique_ptr<Signature> signature(new Signature(HeaderUtils::getAuthorizationString(headers)));
	account_name = signature->splitUserAndPassword().first;

	if (account_name == U(""))
	{
		m_http_request.reply(status_codes::Unauthorized);
		return;
	}

	if (UrlUtils::hasTables(paths))
	{
		m_http_request.reply(status_codes::OK, Metadata::getMysqlTables(account_name));
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
		unique_ptr<Signature> signature(new Signature(HeaderUtils::getAuthorizationString(headers)));

		if (permissions->hasGet(table_name, account_name))
		{
			if (select != query.end())
			{
				vector<string_t> clmns = UrlUtils::getColumnNames(select->second);
				if (permissions->hasGet(table_name, account_name, clmns))
				{
					m_http_request.reply(status_codes::Unauthorized);
					return;
				}
			}
		}
		else
		{
			m_http_request.reply(status_codes::Unauthorized);
			return;
		}

		//if partition key and row key found we are good to go
		if (!(rowkey.empty() && partitionkey.empty()))
		{
			m_http_request.reply(status_codes::OK, Metadata::getMysqlEntity(table_name, rowkey, partitionkey));
			return;
		}
	}
	m_http_request.reply(status_codes::BadRequest);
	return;
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

	//userid of the user account
	string_t account_name;
	//Requested table name
	string_t table_name;
	string_t table_namespace;

	unique_ptr<Signature> signature(new Signature(HeaderUtils::getAuthorizationString(headers)));
	account_name = signature->splitUserAndPassword().first;

	if (account_name == U(""))
	{
		m_http_request.reply(status_codes::Unauthorized);
		return;
	}
	
	if (UrlUtils::hasQuery(paths))
	{
		json::value const & obj = m_http_request.extract_json().get();

		if (obj.is_object())
		{
			auto &query_map = obj.as_object();
			json::value result;
			if(Metadata::getMysqlQueryResults(query_map, result))
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
		json::value const & obj = m_http_request.extract_json().get();
		json::value meta = MHttpPost::createTable(obj, account_name);

		if (!meta.is_null())
		{
			m_http_request.reply(status_codes::Created, meta);
			return;
		}
		m_http_request.reply(status_codes::NoContent, meta);
		return;
	}

	// get the JSON value from the task and display content from it
	try
	{
		if (!UrlUtils::getTableNameWithoutKeys(paths, table_name))
		{
			m_http_request.reply(status_codes::BadRequest);
			return;
		}

		if (!permissions->hasPost(table_name, account_name))
		{
			m_http_request.reply(status_codes::Unauthorized);
			return;
		}
		json::value const & obj = m_http_request.extract_json().get();
		if (obj.is_object())
		{
			vector<string_t> clmns;

			//map containing the properties received from the http request
			auto &property_map = obj.as_object();

			for (auto& pair : obj.as_object())
			{
				clmns.push_back(pair.first);
			}
			if (!permissions->hasPost(table_name, account_name, clmns))
			{
				m_http_request.reply(status_codes::Unauthorized);
				return;
			}
			//find partition key in the map
			auto partitionkey_find = property_map.find(U("PartitionKey"));
			//find row key in the map
			auto rowkey_find = property_map.find(U("RowKey"));

			//if partition key and row key found we are good to go
			if (partitionkey_find != property_map.end() && rowkey_find != property_map.end())
			{
				unique_ptr<MysqlEntity> entity(new MysqlEntity(table_name, partitionkey_find->second.as_string(), //Remove double quotes
					rowkey_find->second.as_string(), //Remove double quotes
					obj.as_object().size()
					));
				//Create new entity using found

				for (auto& pair : property_map)
				{
					if (pair.second == partitionkey_find->second || rowkey_find->second == pair.second)
						continue;
					insetKeyValuePropery(*entity, pair.first, pair.second);
				}

				if (entity->ExecuteEntity() == 1)
				{
					m_http_request.reply(status_codes::Created, U("Data added to the database"));
					return;
				}
				else
				{
					m_http_request.reply(status_codes::BadRequest);
					return;
				}
			}
		}
		else {
			m_http_request.reply(status_codes::BadRequest);
		}
	}
	catch (http_exception)
	{
		m_http_request.reply(status_codes::BadRequest);
	}
	catch (exception)
	{
		m_http_request.reply(status_codes::BadRequest);
	}
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
	onPreDelete();
	auto paths = UrlUtils::splitUri(m_http_request);
	auto query = UrlUtils::splitQueryString(m_http_request);

	if (paths.empty())
	{
		m_http_request.reply(status_codes::OK, json::value::string(U("Table not selected")));
		return;
	}

	string_t table_name;
	UrlUtils::getTableName(paths, table_name);

	unique_ptr<MysqlDelete> mysql_delete(new MysqlDelete());

	mysql_delete->selectTable(table_name);

	auto filter_condition = query.find(CONDITION);
	auto filter_feild = query.find(FIELD);
	auto filter_value = query.find(VALUE);

	if (filter_condition != query.cend() &&
		filter_feild != query.cend() &&
		filter_value != query.cend())
	{
		mysql_delete->AddCondition(filter_feild->second + getCondition(filter_condition->second) + U("'") + filter_value->second + U("'"));
		if (!mysql_delete->executeDelete())
		{
			m_http_request.reply(status_codes::NoContent);
			return;
		}
		m_http_request.reply(status_codes::BadRequest);
		return;
	}
	m_http_request.reply(status_codes::BadRequest);
	return;
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

	//userid of the user account
	string_t account_name;
	//Requested table name
	string_t table_name;
	string_t paritition_key;
	string_t row_key;
	string_t table_namespace;
	
	unique_ptr<Signature> signature(new Signature(HeaderUtils::getAuthorizationString(headers)));
	account_name = signature->splitUserAndPassword().first;

	if (account_name == U(""))
	{
		m_http_request.reply(status_codes::Unauthorized);
		return;
	}

	if (!UrlUtils::getTableNameWithKeys(paths, table_name, row_key, paritition_key))
	{
		m_http_request.reply(status_codes::BadRequest);
		return;
	}

	// get the JSON value from the task and display content from it
	try
	{
		json::value const & obj = m_http_request.extract_json().get();
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

			if (entity.PatchEntity())
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

	}
	catch (http_exception)
	{
		m_http_request.reply(status_codes::BadRequest);
	}
	catch (exception)
	{
		m_http_request.reply(status_codes::BadRequest);
	}
}

string_t MysqlHandler::getCondition(string_t cond)
{
	if (cond == U("le"))
		return U(" <= ");
	if (cond == U(" lt "))
		return U(" < ");
	if (cond == U("ge"))
		return U(" >= ");
	if (cond == U("gt"))
		return U(" > ");
	if (cond == U("ne"))
		return U(" != ");

	return U(" = "); //eq
}

void MysqlHandler::onPreGet()
{

}

void MysqlHandler::onPrePost()
{
	http_headers headers = m_http_request.headers();

	auto paths = UrlUtils::splitUri(m_http_request);

	if (paths.empty())
	{
		m_http_request.reply(status_codes::BadRequest);
		return;
	}

	//userid of the user account
	string_t account_name;
	//Requested table name
	string_t table_name;
	string_t table_namespace;

	unique_ptr<Signature> signature(new Signature(HeaderUtils::getAuthorizationString(headers)));
	account_name = signature->splitUserAndPassword().first;

	if (account_name == U(""))
	{
		m_http_request.reply(status_codes::Unauthorized);
		return;
	}

	if (!UrlUtils::getTableNamespace(paths, table_namespace))
	{
		m_http_request.reply(status_codes::Unauthorized);
		return;
	}

	// continue when the response is available
	// and continue when the JSON value is available
	if (UrlUtils::hasTables(paths))
	{
		json::value const & obj = m_http_request.extract_json().get();
		json::value meta = MHttpPost::createTable(obj, account_name);

		if (!meta.is_null())
		{
			m_http_request.reply(status_codes::Created, meta);
			return;
		}
		m_http_request.reply(status_codes::NoContent, meta);
		return;
	}
	m_http_request.reply(status_codes::BadRequest);
	return;
}

void MysqlHandler::onPreDelete()
{
	http_headers headers = m_http_request.headers();
	auto paths = UrlUtils::splitUri(m_http_request);

	if (paths.empty())
	{
		m_http_request.reply(status_codes::BadRequest);
		return;
	}

	//userid of the user account
	string_t account_name;
	//Requested table name
	string_t table_name;

	unique_ptr<Signature> signature(new Signature(HeaderUtils::getAuthorizationString(headers)));
	account_name = signature->splitUserAndPassword().first;

	if (account_name == U(""))
	{
		m_http_request.reply(status_codes::Unauthorized);
		return;
	}

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
	m_http_request.reply(status_codes::BadRequest);
	return;
}