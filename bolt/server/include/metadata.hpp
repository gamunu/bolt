#include<deque>
#include <cpprest/json.h>
#include <mysql_incognito_entity.h>
#include <mysql_entity.h>
#include <map>

using namespace web;
using namespace utility;

class Metadata
{
public:
	Metadata() {}
	~Metadata() {}
	static json::value getMysqlTables();
	static json::value getAzureTables();
	static json::value getAzureTable(string_t table_name);
	static json::value getMysqlTable(string_t table_name);
	static json::value getAzureEntity(string_t table_name, string_t rowkey, string_t partitionkey);
	static json::value getMysqlEntity(string_t table_name, string_t rowkey, string_t partitionkey);
	static json::value getMysqlEntities(string_t table_name, const std::map<utility::string_t, utility::string_t> query);
	static bool getMysqlQueryResults(const json::object &query_obj, json::value &result);
	static bool getAdministration(const std::vector<utility::string_t> paths, json::value &result);

	static json::value getAzureEntities(string_t table_name, const std::map<utility::string_t, utility::string_t> query);
	
private:
	template <class container>
	static json::value generateEntityMeta(container entity_vector);
	template <class container>
	static json::value generateAzureEntityMeta(container entity_vector);
	static utility::string_t getCondition(string_t condition);

};