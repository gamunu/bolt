#include<metadata.hpp>
#include<mysql_table.h>
#include<mysql_database.h>
#include<boost/scoped_ptr.hpp>
#include <azure_table.h>
#include <azure_query.h>
#include <mysql_query.h>
#include <configuration.hpp>
#include <url_utils.hpp>

using namespace std;
using namespace bolt::storage::boltazure;

json::value Metadata::getMysqlTables(string_t account_name)
{
	auto mysqltable = MysqlTable();
	vector<string_t> tables = mysqltable.getTableList();
	//Convert string vector of values to json vector
	//A json vector of table names
	vector<json::value> table_names(tables.begin(), tables.end());

	json::value metadata = json::value::object();
	metadata[U("value")] = json::value::array(table_names);
	return metadata;
}

json::value Metadata::getAzureTables(string_t account_name)
{
	auto azuretable = AzureTable();
	vector<string_t> tables = azuretable.getTableList();
	//Convert string vector of values to json vector
	//A json vector of table names
	vector<json::value> table_names(tables.begin(), tables.end());

	json::value metadata = json::value::object();
	metadata[U("value")] = json::value::array(table_names);
	return metadata;
}

json::value Metadata::getAzureTable(string_t table_name, string_t account_name)
{
	json::value metadata = json::value::object();
	metadata[U("TableName")] = json::value::string(table_name);
	return metadata;
}

json::value Metadata::getMysqlTable(string_t table_name, string_t account_name)
{
	json::value metadata = json::value::object();
	metadata[U("TableName")] = json::value::string(table_name);
	return metadata;
}

json::value Metadata::getAzureEntity(string_t table_name, string_t rowkey, string_t partitionkey)
{
	unique_ptr<AzureQuery> azurequery(new AzureQuery(table_name)); //AzureQuery Object
	vector<table_entity> result;
	if (!(rowkey.empty() && partitionkey.empty()))
	{
		result = azurequery->filterByKey(partitionkey, rowkey);
	}

	//Entity json array
	json::value entities = json::value::array();
	size_t i = 0; //Entity json array index
	for (auto ait = result.cbegin(); ait != result.cend(); ++ait, ++i)
	{
		json::value entity = json::value::object(); //Entity property set
		for (auto pit = ait->properties().cbegin(); pit != ait->properties().cend(); ++pit)
		{
			entity_property propery = pit->second;
			string_t property_key = pit->first;

			switch (propery.property_type())
			{
			case edm_type::string:
				entity[property_key] = json::value::string(propery.string_value());
				break;
			case edm_type::double_floating_point:
				entity[property_key] = json::value::number(propery.double_value());
				break;
			case edm_type::datetime:
				entity[property_key] = json::value::string(propery.datetime_value().to_string());
				break;
			case edm_type::boolean:
				entity[property_key] = json::value::boolean(propery.boolean_value());
				break;
			case edm_type::int64:
				entity[property_key] = json::value(propery.int64_value());
				break;
			case edm_type::binary:
				entity[property_key] = json::value::string(propery.string_value());
				break;
			case edm_type::guid:
				entity[property_key] = json::value::string(propery.string_value());
				break;
			case edm_type::int32:
				entity[property_key] = json::value::number(propery.int32_value());
				break;
			default:
				entity[property_key] = json::value::string(propery.string_value());

				break;
			}
		}

		entity[U("Timestamp")] = json::value::string(ait->timestamp().to_string(datetime::date_format::ISO_8601));
		entities[i] = entity;
	}
	//Entity enclosing object
	json::value replyObj = json::value::object();
	replyObj[U("value")] = entities;

	return replyObj;
}

json::value Metadata::getMysqlEntity(string_t table_name, string_t rowkey, string_t partitionkey)
{
	auto mysqlquery = MysqlQuery(table_name); //MysqlQuery Object

	deque<mysql_table_entity> result;
	if (!(rowkey.empty() && partitionkey.empty()))
	{
		result = mysqlquery.filterByKey(partitionkey, rowkey);
	}

	return generateEntityMeta(result);
}

bool Metadata::getMysqlQueryResults(const json::object &query_obj, json::value& result)
{
	auto query = MysqlQuery();

	const auto iter_select = query_obj.find(U("Select"));
	const auto iter_select_distinct = query_obj.find(U("Distinct"));
	const auto iter_from = query_obj.find(U("From"));
	const auto iter_join = query_obj.find(U("Join"));
	const auto iter_leftjoin = query_obj.find(U("LeftJoin"));
	const auto iter_rightjoin = query_obj.find(U("RightJoin"));
	const auto iter_naturaljoin = query_obj.find(U("NaturalJoin"));
	const auto iter_crossjoin = query_obj.find(U("CrossJoin"));
	const auto iter_where = query_obj.find(U("Where"));
	const auto iter_andwhere = query_obj.find(U("AndWhere"));
	const auto iter_orwhere = query_obj.find(U("OrWhere"));
	const auto iter_group = query_obj.find(U("Group"));
	const auto iter_having = query_obj.find(U("Having"));
	const auto iter_union = query_obj.find(U("Union"));
	const auto iter_orderby = query_obj.find(U("Order"));
	const auto iter_limit = query_obj.find(U("Limit"));
	const auto iter_offset = query_obj.find(U("Offset"));

	const auto objend = query_obj.cend();

	//If distinct is defined add it to the query
	if (iter_select_distinct != objend)
	{
		query.selectDistinct(iter_select_distinct->second.as_string());
	}
	//If defined select, get the string otherwise select all columns
	if (iter_select != objend)
	{
		query.select(iter_select->second.as_string());
	}
	else
	{
		query.select();
	}
	//if defined from then set the value, otherwise return false,
	//because every query must have a from postion
	if (iter_from != objend)
	{
		query.from(iter_from->second.as_string());
	}
	else
	{
		return false;
	}

	if (iter_join != objend)
	{
		auto join = iter_join->second;
		// check the join is an object
		if (join.is_object())
		{
			//Check join has fiedl TableNmae and Conditions
			if (join.has_field(U("TableName")) && join.has_field(U("Conditions")))
			{
				json::value tablename = join.at(U("TableName"));
				json::value conditions = join.at(U("Conditions"));

				query.join(tablename.as_string(), conditions.as_string());
			}
			else
			{
				return false;
			}
		}
		else if (join.is_array())
		{
			for (auto &portionobj : join.as_array())
			{
				//Check join has fiedl TableNmae and Conditions
				if (portionobj.has_field(U("TableName")) && portionobj.has_field(U("Conditions")))
				{
					json::value tablename = portionobj.at(U("TableName"));
					json::value conditions = portionobj.at(U("Conditions"));

					query.join(tablename.as_string(), conditions.as_string());
				}
				else
				{
					return false;
				}
			}
		}
		else
		{
			return false;
		}
	}

	if (iter_leftjoin != objend)
	{
		auto join = iter_leftjoin->second;
		// check the join is an object
		if (join.is_object())
		{
			//Check join has fiedl TableNmae and Conditions
			if (join.has_field(U("TableName")) && join.has_field(U("Conditions")))
			{
				json::value tablename = join.at(U("TableName"));
				json::value conditions = join.at(U("Conditions"));

				query.leftJoin(tablename.as_string(), conditions.as_string());
			}
			else
			{
				return false;
			}
		}
		else if (join.is_array())
		{
			for (auto &portionobj : join.as_array())
			{
				//Check join has fiedl TableNmae and Conditions
				if (portionobj.has_field(U("TableName")) && portionobj.has_field(U("Conditions")))
				{
					json::value tablename = portionobj.at(U("TableName"));
					json::value conditions = portionobj.at(U("Conditions"));

					query.join(tablename.as_string(), conditions.as_string());
				}
				else
				{
					return false;
				}
			}
		}
		else
		{
			return false;
		}
	}

	if (iter_rightjoin != objend)
	{
		auto join = iter_rightjoin->second;
		// check the join is an object
		if (join.is_object())
		{
			//Check join has fiedl TableNmae and Conditions
			if (join.has_field(U("TableName")) && join.has_field(U("Conditions")))
			{
				json::value tablename = join.at(U("TableName"));
				json::value conditions = join.at(U("Conditions"));

				query.join(tablename.as_string(), conditions.as_string());
			}
			else
			{
				return false;
			}
		}
		else if (join.is_array())
		{
			for (auto &portionobj : join.as_array())
			{
				//Check join has fiedl TableNmae and Conditions
				if (portionobj.has_field(U("TableName")) && portionobj.has_field(U("Conditions")))
				{
					json::value tablename = portionobj.at(U("TableName"));
					json::value conditions = portionobj.at(U("Conditions"));

					query.join(tablename.as_string(), conditions.as_string());
				}
				else
				{
					return false;
				}
			}
		}
		else
		{
			return false;
		}
	}


	if (iter_naturaljoin != objend)
	{
		auto join = iter_naturaljoin->second;

		if (join.is_array())
		{
			for (auto &portion : join.as_array())
			{
				query.naturalJoin(portion.as_string());
			}
		}
		else
		{
			return false;
		}
	}

	if (iter_crossjoin != objend)
	{
		auto join = iter_crossjoin->second;

		if (join.is_array())
		{
			for (auto &portion : join.as_array())
			{
				query.crossJoin(portion.as_string());
			}
		}
		else
		{
			return false;
		}
	}

	if (iter_where != objend)
	{
		query.where(iter_where->second.as_string());
	}

	if (iter_andwhere != objend)
	{
		auto andwhere = iter_andwhere->second;

		if (andwhere.is_array())
		{
			for (auto &whereportion : andwhere.as_array())
			{
				query.andWhere(whereportion.as_string());
			}
		}
		else if (andwhere.is_string())
		{
			query.andWhere(andwhere.as_string());
		}
		else
		{
			return false;
		}
	}

	if (iter_orwhere != objend)
	{
		auto orwhere = iter_orwhere->second;

		if (orwhere.is_array())
		{
			for (auto &whereportion : orwhere.as_array())
			{
				query.andWhere(whereportion.as_string());
			}
		}
		else if (orwhere.is_string())
		{
			query.andWhere(orwhere.as_string());
		}
		else
		{
			return false;
		}
	}

	if (iter_group != objend)
	{
		query.group(iter_group->second.as_string());
	}

	if (iter_having != objend)
	{
		query.having(iter_having->second.as_string());
	}

	if (iter_union != objend)
	{
		query.munion(iter_union->second.as_string());
	}

	if (iter_orderby != objend)
	{
		query.order(iter_orderby->second.as_string());
	}

	if (iter_offset != objend && iter_limit != objend)
	{
		string_t limit;
		string_t offset;

		if (iter_limit->second.is_string())
		{
			limit = iter_limit->second.as_string();
		}
		else if (iter_limit->second.is_integer())
		{
			limit = to_wstring(iter_limit->second.as_integer());
		}
		else
		{
			return false;
		}

		if (iter_offset->second.is_string())
		{
			offset = iter_offset->second.as_string();
		}
		else if (iter_offset->second.is_integer())
		{
			offset = to_wstring(iter_offset->second.as_integer());
		}
		else
		{
			return false;
		}

		query.limit(offset, limit);
	}
	else if (iter_limit != objend)
	{
		string_t limit;
		if (iter_limit->second.is_string())
		{
			limit = iter_limit->second.as_string();
		}
		else if (iter_limit->second.is_integer())
		{
			query.limit(to_wstring(iter_limit->second.as_integer()));
		}
		else
		{
			return false;
		}
	}

	//Query part
	string_t host = Config::getInstance().getServerHostWithPort();
	deque<mysql_table_entity> query_result = query.queryAll();
	result = generateEntityMeta(query_result);

	return true;
}

bool Metadata::getAdministration(const vector<string_t> paths, json::value& result)
{
	string_t table_name;
	if (UrlUtils::getAnalyze(paths, table_name))
	{
		result = generateEntityMeta(MysqlTable::analyzeTable(table_name));
		return true;
	}
	if (UrlUtils::getCheck(paths, table_name))
	{
		result = generateEntityMeta(MysqlTable::checkTable(table_name));
		return true;
	}
	if (UrlUtils::getRepair(paths, table_name))
	{
		result = generateEntityMeta(MysqlTable::repairTable(table_name));
		return true;
	}
	if (UrlUtils::getIndexes(paths, table_name))
	{
		result = generateEntityMeta(MysqlTable::showIndexes(table_name));
		return true;
	}
	if (UrlUtils::getKeys(paths, table_name))
	{
		result = generateEntityMeta(MysqlTable::showKeys(table_name));
		return true;
	}
	if (UrlUtils::getOptimize(paths, table_name))
	{
		result = generateEntityMeta(MysqlTable::optimzeTable(table_name));
		return true;
	}
	if (UrlUtils::hasEngines(paths))
	{
		result = generateEntityMeta(MySqlDB::showEngines());
		return true;
	}
	if (UrlUtils::hasStatus(paths))
	{
		result = generateEntityMeta(MySqlDB::showTableStatus());
		return true;
	}
	if (UrlUtils::hasPlugins(paths))
	{
		result = generateEntityMeta(MySqlDB::showPlugins());
		return true;
	}
	if (UrlUtils::hasOpenTables(paths))
	{
		result = generateEntityMeta(MySqlDB::showOpenTables());
		return true;
	}
	return false;
}

template<typename container>
json::value Metadata::generateEntityMeta(container entity_vector)
{
	//Entity json array
	json::value entities = json::value::array();
	size_t i = 0; //Entity json array index
	for (auto ait = entity_vector.cbegin(); ait != entity_vector.cend(); ++ait, ++i)
	{
		json::value entity = json::value::object(); //Entity property set
		for (auto pit = ait->properties().cbegin(); pit != ait->properties().cend(); ++pit)
		{
			mysql_property propery = pit->second;
			string_t property_key = pit->first;

			switch (propery.property_type())
			{
			case myedm_type::double_floating_point:
				entity[property_key] = json::value(propery.double_value());
				break;
			case myedm_type::int64:
				entity[property_key] = json::value(pit->second.int64_value());
				break;
			case myedm_type::int32:
				entity[property_key] = json::value(pit->second.int64_value());
				break;
			default:
				entity[property_key] = json::value(propery.string_value());
				break;
			}
		}
		entities[i] = entity;
	}
	//Entity enclosing object
	json::value replyObj = json::value::object();
	replyObj[U("value")] = entities;

	return replyObj;
}