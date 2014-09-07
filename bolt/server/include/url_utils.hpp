#pragma once

#include <cpprest/http_msg.h>

using namespace std;
using namespace web::http;
using namespace utility;

class UrlUtils
{
public:
	UrlUtils() {}
	~UrlUtils() {}

	static std::vector<utility::string_t> splitUri(const http_request& message);
	static std::map<utility::string_t, utility::string_t> splitQueryString(const http_request& message);

	static bool getTableName(const std::vector<utility::string_t> paths, utility::string_t &table);
	
	
	static bool getTableNamespace(const std::vector<utility::string_t> paths, utility::string_t &tablenamespace);

	static std::vector<utility::string_t> getColumnNames(utility::string_t str);

	static bool hasTables(const std::vector<utility::string_t> paths);
	static bool hasQuery(const std::vector<utility::string_t> paths);

	static bool hasAdministration(const std::vector<utility::string_t> paths);

	static bool getTableNameWithKeys(const std::vector<utility::string_t> paths, utility::string_t &table, utility::string_t &rowkey, utility::string_t &paritionkey);
	static bool getTableNameWithoutKeys(const std::vector<utility::string_t> paths, utility::string_t &table);

	//Administration
	static bool getAnalyze(const std::vector<string_t> paths, utility::string_t& tablename);
	static bool getCheck(const std::vector<string_t> paths, utility::string_t& tablename);
	static bool getRepair(const std::vector<string_t> paths, utility::string_t& tablename);
	static bool getIndexes(const std::vector<string_t> paths, utility::string_t& tablename);
	static bool getKeys(const std::vector<string_t> paths, utility::string_t& tablename);
	static bool getOptimize(const std::vector<string_t> paths, utility::string_t& tablename);

	static bool hasEngines(const std::vector<string_t> paths);
	static bool hasStatus(const std::vector<string_t> paths);
	static bool hasPlugins(const std::vector<string_t> paths);
	static bool hasOpenTables(const std::vector<string_t> paths);

};