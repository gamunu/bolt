#include <cpprest/json.h>

using namespace web;

class AHttpPost
{
public:
	AHttpPost();
	static json::value createTable(json::value object, utility::string_t account_name);
};