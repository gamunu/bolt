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

	static vector<string_t> splitUri(const http_request& message);
	static map<string_t, string_t> splitQueryString(const http_request& message);

	static bool getTableName(const vector<string_t> paths, string_t &table);
	
	
	static bool getTableNamespace(const vector<string_t> paths, string_t &tablenamespace);
	
	static bool hasTables(const vector<string_t> paths);
	static bool hasQuery(const vector<string_t> paths);

	static bool hasAdministration(const vector<string_t> paths);

	static bool getTableNameWithKeys(const vector<string_t> paths, string_t &table, string_t &rowkey, string_t &paritionkey);
	static bool getTableNameWithoutKeys(const vector<string_t> paths, string_t &table);

	//Administration
	static bool getAnalyze(const vector<string_t> paths, string_t& tablename);
	static bool getCheck(const vector<string_t> paths, string_t& tablename);
	static bool getRepair(const vector<string_t> paths, string_t& tablename);
	static bool getIndexes(const vector<string_t> paths, string_t& tablename);
	static bool getKeys(const vector<string_t> paths, string_t& tablename);
	static bool getOptimize(const vector<string_t> paths, string_t& tablename);

	static bool hasEngines(const vector<string_t> paths);
	static bool hasStatus(const vector<string_t> paths);
	static bool hasPlugins(const vector<string_t> paths);
	static bool hasOpenTables(const vector<string_t> paths);

	static bool getFilter(const map<string_t, string_t> query, map<string_t, string_t> &filter);
	static bool getSelect(const map < string_t, string_t> query, vector<string_t> &select);
	static vector<string_t> getColumnNames(string_t str);

};