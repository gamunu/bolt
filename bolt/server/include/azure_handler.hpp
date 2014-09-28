#pragma once

#include <azure_entity.h>
#include <cpprest/http_msg.h>
#include <cpprest/json.h>
#include <permissions.hpp>

using namespace web;
using namespace bolt::storage;

class AzureHandler 
{
public:
	AzureHandler(const http::http_request &request, http::method method);
	void InitializeHandlers();
	void HandleGet();
	void HandlePost();
	void HandleDelete();
	void HandlePatch();
private:
	void insetKeyValuePropery(boltazure::AzureEntity &entity, utility::string_t key, json::value value);
	const http::http_request &m_http_request;
	http::method m_method;
};