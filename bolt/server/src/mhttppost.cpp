#include<mhttppost.hpp>
#include<regex>
#include<boost/scoped_ptr.hpp>
#include <mysql_table.h>
#include <metadata.hpp>

using namespace std;
using namespace bolt::storage::mysql;

json::value MHttpPost::createTable(json::value object)
{
	//TODO:Check if we are in windows or unix
	wregex name_regx(U("^[A-Za-z][A-Za-z0-9]{2,62}$"));

	json::value table_metadata;

	if (!object.is_null())
	{
		if (object.has_field(U("TableName")))
		{
			 auto iter = object.as_object().find(U("TableName"));
			
			 string_t tableName = iter->second.as_string();
			if (regex_match(tableName, name_regx))
			{
				MysqlTable table = MysqlTable();
				if (table.createTable(tableName))
				{
					table_metadata = Metadata::getMysqlTable(tableName);
				}
			}
		}
	}
	return table_metadata;
}