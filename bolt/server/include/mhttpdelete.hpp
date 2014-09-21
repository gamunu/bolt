#include <cpprest/json.h>
#include <map>
using namespace  web;

class MHttpDelete
{
public:
	MHttpDelete() {};
	static bool deleteEntities(utility::string_t tablename, std::map<utility::string_t, utility::string_t> const query);
private:
	static utility::string_t getCondition(utility::string_t condition);
};