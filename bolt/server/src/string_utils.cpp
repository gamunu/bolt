#include <string_utils.hpp>

myedm_type StringUtils::getMyqlEdmType(string_t key, web::json::value value)
{
	return myedm_type::datetime;
}