#include <whole_handler.hpp>
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

WholeHandler::WholeHandler(const http_request &request, const method method) : m_http_request(request)
{
	m_method = method;

	permissions = make_unique<Permissions>();
}

void WholeHandler::InitializeHandlers()
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
}

/// <summary>
/// Handles the get.
/// </summary>
void WholeHandler::HandleGet()
{
	m_http_request.reply(status_codes::BadRequest);
	return;
}

/// <summary>
/// Handles the post.
/// </summary>
void WholeHandler::HandlePost()
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

					unique_ptr<MysqlEntity> mentity(new MysqlEntity(
						table_name,
						partitionkey_find->second.as_string(), //Remove double quotes
						rowkey_find->second.as_string(), //Remove double quotes
						obj.as_object().size()
						));

					for (auto& pair : property_map)
					{
						if (pair.second == partitionkey_find->second || rowkey_find->second == pair.second)
							continue;
						insetKeyValuePropery(*entity, pair.first, pair.second);
						insetKeyValuePropery(*mentity, pair.first, pair.second);
					}

					if (entity->ExecuteEntity() == 1 && mentity->ExecuteEntity())
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

void WholeHandler::HandleDelete()
{
	m_http_request.reply(status_codes::BadRequest);
	return;
}

void WholeHandler::insetKeyValuePropery(boltazure::AzureEntity &entity, string_t key, json::value value)
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

void WholeHandler::insetKeyValuePropery(MysqlEntity& entity, utility::string_t key, json::value value)
{
	if (value.is_string())
	{
		entity.insertString(key, value.as_string());
	}
	else if (value.is_double())
	{
		entity.insertDouble(key, value.as_double());
	}
	else if (value.is_boolean())
	{
		entity.insertBoolean(key, value.as_bool());
	}
	else if (value.is_integer())
	{
		entity.insertInt64(key, value.as_integer());
	}
}
