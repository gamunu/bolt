#pragma once

#include <cpprest/http_msg.h>
#include <cpprest/json.h>
#include <permissions.hpp>
#include <mysql_entity.h>

using namespace std;
using namespace web;
using namespace web::http;
using namespace bolt::storage::mysql;

class MysqlHandler 
{
public:
	MysqlHandler(const http_request &request, method method);
	void InitializeHandlers();
	void HandleGet();
	void HandlePost();
	void HandleDelete();
	void HandlePatch();
	void insetKeyValuePropery(MysqlEntity &entity, utility::string_t key, json::value value);
private:
	const http_request &m_http_request;
	method m_method;

	std::unique_ptr<Permissions> permissions;
};