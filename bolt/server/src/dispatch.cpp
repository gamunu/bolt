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
	unique_ptr<Permissions> permissions;
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

	/*	if (UrlUtils::getTableName(paths, table_name))
		{
			unique_ptr<Signature> signature(new Signature(HeaderUtils::getAuthorizationString(headers)));

			if (!permissions->hasGet(table_name, signature->splitUserAndPassword().first))
			{
				if (select != query.end())
				{
					vector<string_t> clmns = UrlUtils::getColumnNames(select->second);
					if (!permissions->hasGet(table_name, signature->splitUserAndPassword().first, clmns)){
						message.reply(status_codes::Unauthorized, json::value::string(U("Column access not permitted")));
						return;
					}
				}
				else
				{
					message.reply(status_codes::Unauthorized, json::value::string(U("Table access not permited")));
					return;
				}
			}
			switchDatabase(methods::GET, message); //Switch database according to request header x-bolt-database
		}
		*/
		switchDatabase(methods::GET, message);
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
		/*
		if (UrlUtils::getTableName(paths, table_name))
		{
			unique_ptr<Signature> signature(new Signature(HeaderUtils::getAuthorizationString(headers)));

			if (query.empty())
			{
				if (!permissions->hasGet(table_name, signature->splitUserAndPassword().first))
				{
					message.extract_json().then([=](pplx::task<json::value> extract_json_task)
					{
						vector<string_t> clmns;
						unique_ptr<Signature> signature(new Signature(HeaderUtils::getAuthorizationString(headers)));
						// get the JSON value from the task and display content from it
						try
						{
							json::value const& obj = extract_json_task.get();
							if (!obj.is_null())
							{
								for (auto& iter : obj.as_object())
								{
									clmns.push_back(iter.first);
								}

								if (!permissions->hasPost(table_name, signature->splitUserAndPassword().first, clmns))
								{
									message.reply(status_codes::Unauthorized, json::value::string(U("Column access not permitted 2")));
									return;
								}

								switchDatabase(methods::POST, message);
								return;
							}
						}
						catch (http_exception const& e)
						{
							wcout << e.what() << endl;
							bolt_logger << BoltLog::LOG_ERROR << e.what();
							message.reply(status_codes::BadRequest, json::value::string(U("catch")));
						}
					}
					).wait();
				}
			}
			else if (field != query.cend())
			{
				if (!permissions->hasMerge(table_name, signature->splitUserAndPassword().first))
				{
					message.extract_json().then([=](pplx::task<json::value> extract_json_task)
					{
						vector<string_t> clmns;
						unique_ptr<Signature> signature(new Signature(HeaderUtils::getAuthorizationString(headers)));
						// get the JSON value from the task and display content from it
						try
						{
							json::value const& obj = extract_json_task.get();
							if (!obj.is_null())
							{
								for (auto& iter : obj.as_object())
								{
									clmns.push_back(iter.first);
								}

								if (!permissions->hasMerge(table_name, signature->splitUserAndPassword().first, clmns))
								{
									message.reply(status_codes::Unauthorized, json::value::string(U("Column access not permitted 1")));
									return;
								}

								switchDatabase(methods::POST, message);
								return;
							}
						}
						catch (http_exception const& e)
						{
							wcout << e.what() << endl;
							bolt_logger << BoltLog::LOG_ERROR << e.what();
							message.reply(status_codes::BadRequest, json::value::string(U("catch")));
						}
					}
					).wait();
				}
			}
		}
		*/

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
		http_headers headers = message.headers();

		auto paths = UrlUtils::splitUri(message);
		string_t table_name;

		/*
		if (UrlUtils::getTableName(paths, table_name))
		{
			unique_ptr<Signature> signature(new Signature(HeaderUtils::getAuthorizationString(headers)));


			if (!permissions->hasDelete(table_name, signature->splitUserAndPassword().first))
			{
				message.reply(status_codes::Unauthorized, json::value::string(U("Table access not permited")));
				return;
			}

			switchDatabase(methods::DEL, message); //Switch database according to request header x-bolt-database
		}
		*/
		switchDatabase(methods::DEL, message);
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
	m_impl->permissions = make_unique<Permissions>();
}