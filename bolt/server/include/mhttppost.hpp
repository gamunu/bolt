#include <cpprest/json.h>

using namespace  web;

class MHttpPost
{
public:
	MHttpPost() {};
	static json::value createTable(json::value object);
};