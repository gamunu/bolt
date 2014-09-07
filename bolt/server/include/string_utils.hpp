#pragma once

#include <cpprest/asyncrt_utils.h>
#include <cpprest/json.h>
#include <table.h>
using namespace utility;
using namespace bolt::storage::mysql;

class StringUtils
{
public:
	StringUtils() {}
	static myedm_type getMyqlEdmType(string_t key, web::json::value value);
};