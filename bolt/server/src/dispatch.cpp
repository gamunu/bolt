#include <dispatch.hpp>
#include <mysql_handler.hpp>
#include <azure_handler.hpp>
#include <signature.hpp>
#include <url_utils.hpp>
#include <header_utils.hpp>
#include <permissions.hpp>
#include <message_types.hpp>
#include <whole_handler.hpp>
#include <permissions.hpp>

using namespace bolt::auth;
using namespace http;

class Dispatch::DispatchImpl
{
public:
	Permissions permissions;
	BoltLog bolt_logger;

	void handle_get(const http_request message)
	{

		auto paths = UrlUtils::splitUri(message);

		http_headers headers = message.headers();

		auto query = UrlUtils::splitQueryString(message);
		auto select = query.find(SELECT);

		if (paths.empty())
		{
			message.reply(status_codes::OK, json::value::string(U("Table not selected")));
			return;
		}

		string_t table_name; //paths 0 is name of the table
		string_t rowkey;
		string_t partitionkey;

		if (UrlUtils::hasTables(paths) || UrlUtils::hasAdministration(paths))
		{
			switchDatabase(methods::GET, message);
			return;
		}
		if (UrlUtils::getTableName(paths, table_name)
			|| UrlUtils::getTableNameWithKeys(paths, table_name, rowkey, partitionkey)
			|| UrlUtils::getTableNameWithoutKeys(paths, table_name))
		{
			Signature signature(HeaderUtils::getAuthorizationString(headers));

			if (!permissions.hasGet(table_name, signature.getUsername()))
			{
				message.reply(status_codes::Unauthorized, json::value::string(U("Table access not permited")));
				return;
			}
			if (select != query.end())
			{
				vector<string_t> clmns = UrlUtils::getColumnNames(select->second);
				if (!permissions.hasGet(table_name, signature.getUsername(), clmns)){
					message.reply(status_codes::Unauthorized, json::value::string(U("Column access not permitted")));
					return;
				}
			}
		}
		switchDatabase(methods::GET, message); //Switch database according to request header x-bolt-database
	}

	void handle_post(http_request message)
	{
		auto paths = UrlUtils::splitUri(message);

		http_headers headers = message.headers();

		auto query = UrlUtils::splitQueryString(message);
		auto field = query.find(FIELD);


		if (paths.empty())
		{
			message.reply(status_codes::OK, json::value::string(U("Table not selected")));
			return;
		}

		string_t table_name; //paths 0 is name of the table

		if (UrlUtils::hasTables(paths))
		{
			switchDatabase(methods::POST, message);
			return;
		}

		if (UrlUtils::getTableNameWithoutKeys(paths, table_name))
		{
			Signature signature(HeaderUtils::getAuthorizationString(headers));
			if (!permissions.hasPost(table_name, signature.getUsername()))
			{
				message.reply(status_codes::Unauthorized, json::value::string(U("table access not permitted 2")));
				return;
			}
			switchDatabase(methods::POST, message);
		}
	}

	void handle_put(http_request message)
	{
		switchDatabase(methods::PUT, message); //Switch database according to request header x-bolt-database
	}

	void handle_delete(http_request message)
	{
		http_headers headers = message.headers();

		auto paths = UrlUtils::splitUri(message);
		string_t table_name;


		if (UrlUtils::getTableName(paths, table_name))
		{
			Signature signature(HeaderUtils::getAuthorizationString(headers));


			if (!permissions.hasDelete(table_name, signature.getUsername()))
			{
				message.reply(status_codes::Unauthorized, json::value::string(U("Table access not permited")));
				return;
			}

			switchDatabase(methods::DEL, message); //Switch database according to request header x-bolt-database
		}

		if (UrlUtils::getTableNameWithoutKeys(paths, table_name))
		{
			Signature signature(HeaderUtils::getAuthorizationString(headers));
			if (!permissions.hasPost(table_name, signature.getUsername()))
			{
				message.reply(status_codes::Unauthorized, json::value::string(U("table access not permitted 2")));
				return;
			}
			switchDatabase(methods::DEL, message);
		}

		message.reply(status_codes::BadRequest);
	}

	void handle_patch(http_request const message)
	{
		auto paths = UrlUtils::splitUri(message);

		if (paths.empty())
		{
			message.reply(status_codes::BadRequest);
			return;
		}
		switchDatabase(methods::PATCH, message); //Switch database according to request header x-bolt-database
	}

	void switchDatabase(method method, const http_request& message)
	{
		http_headers headers = message.headers(); //Get headers from http request

		string_t database_type; //get the database type from the header

		if (headers.match(header_names::content_location, database_type))
		{
			if (database_type == U("mysql"))
			{
				MysqlHandler(message, method).InitializeHandlers();
			}
			else if (database_type == U("azure"))
			{
				AzureHandler(message, method).InitializeHandlers();
			}
			else if (database_type == U("whole"))
			{
				WholeHandler(message, method).InitializeHandlers();
			}
			else
			{
				message.reply(status_codes::BadRequest);
			}
		}
		else
		{
			message.reply(status_codes::Forbidden, json::value::string(U("Welcome to Bolt Rest Server!")));
		}
	}
};

Dispatch::Dispatch() : m_impl{ new DispatchImpl }
{
}

Dispatch::~Dispatch()
{
}

Dispatch::Dispatch(string_t url) : m_impl{ new DispatchImpl }, m_listener(url)
{
	//Bind http functions to http listener
	m_listener.support(methods::GET, bind(&DispatchImpl::handle_get, m_impl, placeholders::_1));
	m_listener.support(methods::PUT, bind(&DispatchImpl::handle_put, m_impl, placeholders::_1));
	m_listener.support(methods::POST, bind(&DispatchImpl::handle_post, m_impl, placeholders::_1));
	m_listener.support(methods::DEL, bind(&DispatchImpl::handle_delete, m_impl, placeholders::_1));
	m_listener.support(methods::PATCH, bind(&DispatchImpl::handle_patch, m_impl, placeholders::_1));
}