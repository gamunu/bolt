#include<deque>
#include <cpprest/json.h>
#include <mysql_incognito_entity.h>
#include <mysql_entity.h>

using namespace web;
using namespace utility;

class Metadata
{
public:
	Metadata() {}
	~Metadata() {}
	static json::value getMysqlTables(string_t account_name);
	static json::value getAzureTables(string_t account_name);
	static json::value getAzureTable(string_t table_name, string_t account_name);
	static json::value getMysqlTable(string_t table_name, string_t account_name);
	static json::value getAzureEntity(string_t table_name, string_t rowkey, string_t partitionkey);
	static json::value getMysqlEntity(string_t table_name, string_t rowkey, string_t partitionkey);
	static bool getMysqlQueryResults(const json::object &query_obj, json::value &result);
	static bool getAdministration(const std::vector<utility::string_t> paths, json::value &result);
	
private:
	template <class container>
	static json::value generateEntityMeta(container entity_vector);

};