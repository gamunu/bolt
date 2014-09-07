#pragma once

#include <cpprest/http_msg.h>

using namespace std;
using namespace web::http;

class HeaderUtils

{
public:
	HeaderUtils() { }
	~HeaderUtils() { }
	
	static utility::string_t getAuthorizationString(const http_headers& headers);
	static utility::string_t getDateTimeString(const http_headers& headers);

};
