#include <header_utils.hpp>

utility::string_t HeaderUtils::getAuthorizationString(const http_headers& headers)
{
	utility::string_t authorization_string;
	headers.match(header_names::authorization, authorization_string);
	return authorization_string;
}

utility::string_t HeaderUtils::getDateTimeString(const http_headers& headers)
{
	utility::string_t date_string;
	headers.match(header_names::date, date_string);
	return date_string;
}