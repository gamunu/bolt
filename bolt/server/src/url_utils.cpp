#include <url_utils.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <regex>
#include <message_types.hpp>

vector<string_t> UrlUtils::splitUri(const http_request& message)
{
	return uri::split_path(uri::decode(message.relative_uri().path()));
}

/// <summary>
/// Gets the name of the table.
/// Validates the table name using regular expression
/// Returns true if maches otherwise returns false
/// </summary>
/// <param name="paths">vector of paths.</param>
/// <param name="table">referece to table name.</param>
/// <returns></returns>
bool UrlUtils::getTableName(const vector<string_t> paths, string_t &table)
{
	//Expression will check for Table('name')
	//Table name must be 2 to 62 long and  can have alphanumaric characters
	const wregex expression(U("^(?:Tables\\(')([A-Za-z][A-Za-z0-9]{2,62})(?:'\\))$"));
	wsmatch what;
	//Path[1] is the Table('name')
	if (regex_match(paths[0], what, expression))
	{
		table = what[1]; //There are three sets in the expression, 2nd is the table name
		return true;
	}
	return false;
}



bool UrlUtils::getTableNamespace(const vector<string_t> paths, string_t &tablenamespace)
{
	if (paths.size() > 0)
	{
		tablenamespace = paths[0];
		return true;
	}
	return false;
}

map<string_t, string_t> UrlUtils::splitQueryString(const http_request& message)
{
	return uri::split_query(uri::decode(message.relative_uri().query()));
}

vector<string_t> UrlUtils::getColumnNames(string_t str)
{
	vector<string_t> columns;
	boost::split(columns, str, boost::is_any_of(","));
	return columns;
}

bool UrlUtils::hasTables(vector<string_t> const paths)
{
	if (paths.size() >= 0 && paths[0] == U("Tables"))
	{
		return true;
	}
	return false;
}

bool UrlUtils::hasQuery(vector<string_t> const paths)
{
	if (paths.size() >= 0 && paths[0] == U("Query"))
	{
		return true;
	}
	return false;
}

bool UrlUtils::hasAdministration(vector<string_t> const paths)
{
	if (paths.size() >= 0 && paths[0] == U("Administration"))
	{
		return true;
	}
	return false;
}

bool UrlUtils::getTableNameWithKeys(vector<string_t> const paths, string_t& table, string_t& rowkey, string_t& paritionkey)
{
	//Expression will check for tablename(RowKey='<rowkey>',PartitionKey='<partitionkey>')
	//Table name must be 2 to 62 long and  can have alphanumaric characters
	const wregex expression(U("^([A-Za-z][A-Za-z0-9]{2,62})(?:\\()(?:(?:PartitionKey=')(\\w+)(?:',RowKey=')(\\w+)(?:'))?(?:\\))$"));
	wsmatch what;
	//Path[1] is the Table('name')
	if (regex_match(paths[0], what, expression))
	{
		table = what[1]; //There are three sets in the expression, 2nd is the table name
		paritionkey = what[2];
		rowkey = what[3];
		return true;
	}
	return false;
}

bool UrlUtils::getTableNameWithoutKeys(vector<string_t> const paths, string_t& table)
{
	//Expression will check for tablename(RowKey='<rowkey>',PartitionKey='<partitionkey>')
	//Table name must be 2 to 62 long and  can have alphanumaric characters
	const wregex expression(U("^([A-Za-z][A-Za-z0-9]{2,62})(?:\\()?(?:\\))?$"));
	wsmatch what;
	//Path[1] is the Table('name')
	if (regex_match(paths[0], what, expression))
	{
		table = what[1]; //There are three sets in the expression, 2nd is the table name
		return true;
	}
	return false;
}

bool UrlUtils::getAnalyze(vector<string_t> const paths, string_t& tablename)
{
	if (paths.size() >= 2 && paths[1] == U("Analyze") && !paths[2].empty())
	{
		tablename = paths[2];
		return true;
	}
	return false;
}

bool UrlUtils::getCheck(vector<string_t> const paths, string_t& tablename)
{
	if (paths.size() >= 2 && paths[1] == U("Check") && !paths[2].empty())
	{
		tablename = paths[2];
		return true;
	}
	return false;
}

bool UrlUtils::getRepair(vector<string_t> const paths, string_t& tablename)
{
	if (paths.size() >= 2 && paths[1] == U("Repair") && !paths[2].empty())
	{
		tablename = paths[2];
		return true;
	}
	return false;
}

bool UrlUtils::getIndexes(vector<string_t> const paths, string_t& tablename)
{
	if (paths.size() >= 2 && paths[1] == U("Indexes") && !paths[2].empty())
	{
		tablename = paths[2];
		return true;
	}
	return false;
}

bool UrlUtils::getKeys(vector<string_t> const paths, string_t& tablename)
{
	if (paths.size() >= 2 && paths[1] == U("Keys") && !paths[2].empty())
	{
		tablename = paths[2];
		return true;
	}
	return false;
}

bool UrlUtils::getOptimize(vector<string_t> const paths, string_t& tablename)
{
	if (paths.size() >= 2 && paths[1] == U("Optimize") && !paths[2].empty())
	{
		tablename = paths[2];
		return true;
	}
	return false;
}

bool UrlUtils::hasEngines(vector<string_t> const paths)
{
	if (paths.size() >= 2 && paths[1] == U("Engines"))
	{
		return true;
	}
	return false;
}

bool UrlUtils::hasStatus(vector<string_t> const paths)
{
	if (paths.size() >= 2 && paths[1] == U("Status"))
	{
		return true;
	}
	return false;
}

bool UrlUtils::hasPlugins(vector<string_t> const paths)
{
	if (paths.size() >= 2 && paths[1] == U("Plugins"))
	{
		return true;
	}
	return false;
}

bool UrlUtils::hasOpenTables(vector<string_t> const paths)
{
	if (paths.size() >= 2 && paths[1] == U("OpenTables"))
	{
		return true;
	}
	return false;
}

bool UrlUtils::getFilter(map<string_t, string_t> const query, map<string_t, string_t> &filter)
{
	map<string_t, string_t> parsed_query;

	//Expression will check for tablename(RowKey='<rowkey>',PartitionKey='<partitionkey>')
	//Table name must be 2 to 62 long and  can have alphanumaric characters
	string_t expression_string = U("^(?:(?:\\()(\\w+)(?:\\s)(le|lt|ge|gt|ne|eq)(?:\\s)(\\w+)(?:\\)))(?:(?:\\s)(and|or)(?:\\s)(?:(?:\\()(\\w+)(?:\\s)(le|lt|ge|gt|ne|eq)(?:\\s)(\\w+)(?:\\))))?$");
	auto lfilter = query.find(FILTER);
	if (lfilter != query.end())
	{
		const wregex expression(expression_string);
		wsmatch what;
		//Path[1] is the Table('name')
		string_t filter_value = lfilter->second;
		if (regex_match(filter_value, what, expression))
		{
			if (what.size() >= 3)
			{
				parsed_query.insert(make_pair(U("first_attr"), what[1]));
				parsed_query.insert(make_pair(U("first_con"), what[2]));
				parsed_query.insert(make_pair(U("first_val"), what[3]));
				if (what.size() == 8)
				{
					parsed_query.insert(make_pair(U("join"), what[4]));
					parsed_query.insert(make_pair(U("second_attr"), what[5]));
					parsed_query.insert(make_pair(U("second_con"), what[6]));
					parsed_query.insert(make_pair(U("second_val"), what[7]));
				}
				filter = parsed_query;
				return true;
			}
		}
	}
	return false;
}

bool UrlUtils::getSelect(map<string_t, string_t> const query, vector<string_t>& select)
{
	if (query.find(SELECT) != query.cend())
	{
		auto lselect = getColumnNames(query.find(SELECT)->second);
		if (lselect.size() > 0)
		{
			select = lselect;
			return true;
		}
	}

	return false;
}