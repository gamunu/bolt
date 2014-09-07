#include <ahttppost.hpp>
#include <metadata.hpp>
#include <azure_table.h>
#include <regex>

using namespace std;
using namespace bolt::storage::boltazure;

json::value AHttpPost::createTable(json::value object, string_t account_name)
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
				
				if (AzureTable::createTable(tableName))
				{
					table_metadata = Metadata::getAzureTable(tableName, account_name);
				}
			}
		}
	}
	return table_metadata;
}