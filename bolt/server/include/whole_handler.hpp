#pragma once

#include <azure_entity.h>
#include <mysql_entity.h>
#include <cpprest/http_msg.h>
#include <cpprest/json.h>
#include <permissions.hpp>

using namespace web;
using namespace bolt::storage;

class WholeHandler 
{
public:
	WholeHandler(const http::http_request &request, http::method method);
	void InitializeHandlers();
	void HandleGet();
	void HandlePost();
	void HandleDelete();
private:
	void insetKeyValuePropery(boltazure::AzureEntity &entity, utility::string_t key, json::value value);
	void insetKeyValuePropery(mysql::MysqlEntity &entity, utility::string_t key, json::value value);
	const http::http_request &m_http_request;
	http::method m_method;
	std::unique_ptr<Permissions> permissions;
};