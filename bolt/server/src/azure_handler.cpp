#include <azure_handler.hpp>
#include <message_types.hpp>
#include <url_utils.hpp>
#include <header_utils.hpp>
#include "azure_query.h"
#include "azure_table.h"
#include <string_utils.hpp>
#include <signature.hpp>

#include "was/table.h"
#include <metadata.hpp>
#include <ahttppost.hpp>
using namespace azure;
using namespace storage;
using namespace bolt::storage;
using namespace bolt::auth;

AzureHandler::AzureHandler(const http_request &request, const method method) : m_http_request(request)
{
	m_method = method;

	permissions = make_unique<Permissions>();
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
		m_http_request.reply(status_codes::OK, Metadata::getAzureTables(account_name));
		return;
	}



	if (UrlUtils::getTableNameWithKeys(paths, table_name, rowkey, partitionkey))
	{
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
void AzureHandler::HandlePost()
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
		json::value meta = AHttpPost::createTable(obj, account_name);

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
		if (!obj.is_null())
		{
			if (obj.is_object())
			{
				unique_ptr<boltazure::AzureTable> azure_table(new boltazure::AzureTable(table_name));
				unique_ptr<boltazure::AzureEntity> entity(new boltazure::AzureEntity(azure_table->getTable()));
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
					//Create new entity using found
					entity->CreateEntity(
						partitionkey_find->second.as_string(), //Remove double quotes
						rowkey_find->second.as_string(), //Remove double quotes
						obj.as_object().size()
						);

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
		}
		else {
			m_http_request.reply(status_codes::BadRequest);
			return;
		}
	}

	catch (http_exception)
	{
		m_http_request.reply(status_codes::BadRequest);
		return;
	}
	catch (exception)
	{
		m_http_request.reply(status_codes::BadRequest);
		return;
	}
}

void AzureHandler::HandleDelete()
{
	http_headers headers = m_http_request.headers();
	auto paths = UrlUtils::splitUri(m_http_request);

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
		boltazure::AzureTable table = boltazure::AzureTable();
		if (table.deleteTable(table_name))
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

void AzureHandler::HandlePatch()
{
	http_headers headers = m_http_request.headers();
	auto paths = UrlUtils::splitUri(m_http_request);
	//userid of the user account
	string_t account_name;
	//Requested table name
	string_t table_name;
	string_t partition_key;
	string_t row_key;

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

	if (!UrlUtils::getTableNameWithKeys(paths, table_name, row_key, partition_key))
	{
		m_http_request.reply(status_codes::BadRequest);
		return;
	}

	if (!permissions->hasMerge(table_name, account_name))
	{
		m_http_request.reply(status_codes::Unauthorized);
		return;
	}

	// get the JSON value from the task and display content from it
	try
	{
		json::value const & obj = m_http_request.extract_json().get();
		if (!obj.is_null())
		{
			if (obj.is_object())
			{
				boltazure::AzureTable azure_table = boltazure::AzureTable(table_name);
				boltazure::AzureEntity entity = boltazure::AzureEntity(azure_table.getTable());

				//map containing the properties received from the http request
				auto &property_map = obj.as_object();

				//Create new entity using found
				entity.CreateEntity(partition_key, row_key, obj.as_object().size());

				for (auto& pair : property_map)
				{
					insetKeyValuePropery(entity, pair.first, pair.second);
				}

				if (entity.PatchEntity())
				{
					m_http_request.reply(status_codes::NoContent, Metadata::getAzureEntity(table_name, row_key, partition_key));
					return;
				}
				else
				{
					m_http_request.reply(status_codes::NotModified);
					return;
				}
			}
		}
		else {
			m_http_request.reply(status_codes::BadRequest);
			return;
		}
	}

	catch (http_exception)
	{
		m_http_request.reply(status_codes::BadRequest);
		return;
	}
	catch (exception)
	{
		m_http_request.reply(status_codes::BadRequest);
		return;
	}
}

void AzureHandler::insetKeyValuePropery(boltazure::AzureEntity &entity, string_t key, json::value value)
{
	if (value.is_string())
	{
		entity.Insert(key, value.as_string());
	}
	else if (value.is_double())
	{
		entity.Insert(key, value.as_double());
	}
	else if (value.is_boolean())
	{
		entity.Insert(key, value.as_bool());
	}
	else if (value.is_integer())
	{
		entity.Insert(key, value.as_integer());
	}
}


void AzureHandler::onPrePost()
{
	http_headers headers = m_http_request.headers();
	auto paths = UrlUtils::splitUri(m_http_request);

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

	if (UrlUtils::hasTables(paths))
	{
		// continue when the response is available
		// and continue when the JSON value is available
		json::value const & obj = m_http_request.extract_json().get();

		json::value meta = AHttpPost::createTable(obj, account_name);

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