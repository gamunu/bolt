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
using namespace web::http;

class Dispatch::DispatchImpl
{
public:
	std::unique_ptr<Permissions> permissions;

	void handle_get(const http_request message)
	{
		switchDatabase(methods::GET, message); //Switch database according to request header x-bolt-database
	}

	void handle_post(http_request message)
	{
		auto paths = UrlUtils::splitUri(message);

		if (paths.empty())
		{
			message.reply(status_codes::BadRequest);
			return;
		}
		switchDatabase(methods::POST, message); //Switch database according to request header x-bolt-database
	}

	void handle_put(http_request message)
	{
		http_headers headers = message.headers();

		unique_ptr<Signature> signature(new Signature(HeaderUtils::getAuthorizationString(headers)));

		if (!signature->isValied(HeaderUtils::getDateTimeString(headers), message.relative_uri().path(), U("PUT")))
			message.reply(status_codes::Unauthorized, U("Unauthorized Access"));
		else
			message.reply(status_codes::Accepted, U("Accepted"));
		switchDatabase(methods::PUT, message); //Switch database according to request header x-bolt-database
	}

	void handle_delete(http_request message)
	{
		switchDatabase(methods::DEL, message); //Switch database according to request header x-bolt-database
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

	void switchDatabase(method method, const http_request &message)
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

Dispatch::Dispatch(string_t url) : m_impl { new DispatchImpl }, m_listener(url)
{
	//Bind http functions to http listener
	m_listener.support(methods::GET, bind(&DispatchImpl::handle_get, m_impl, placeholders::_1));
	m_listener.support(methods::PUT, bind(&DispatchImpl::handle_put, m_impl, placeholders::_1));
	m_listener.support(methods::POST, bind(&DispatchImpl::handle_post, m_impl, placeholders::_1));
	m_listener.support(methods::DEL, bind(&DispatchImpl::handle_delete, m_impl, placeholders::_1));
	m_listener.support(methods::PATCH, bind(&DispatchImpl::handle_patch, m_impl, placeholders::_1));
	m_impl->permissions = make_unique<Permissions>();
}