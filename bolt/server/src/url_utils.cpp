#include <url_utils.hpp>
#include <boost/tokenizer.hpp>
#include <regex>

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

	typedef boost::tokenizer < boost::char_separator<wchar_t> >
		tokenizer;
	boost::char_separator<wchar_t> sep(U(","));
	tokenizer tokens(conversions::to_utf8string(str), sep);
	for (tokenizer::iterator tok_iter = tokens.begin();
		tok_iter != tokens.end(); ++tok_iter)
		columns.push_back(conversions::to_string_t(*tok_iter));

	return columns;
}

bool UrlUtils::hasTables(std::vector<utility::string_t> const paths)
{
	if (paths.size() >= 0 && paths[0] == U("Tables"))
	{
		return true;
	}
	return false;
}

bool UrlUtils::hasQuery(std::vector<utility::string_t> const paths)
{
	if (paths.size() >= 0 && paths[0] == U("Query"))
	{
		return true;
	}
	return false;
}

bool UrlUtils::hasAdministration(std::vector<utility::string_t> const paths)
{
	if (paths.size() >= 0 && paths[0] == U("Administration"))
	{
		return true;
	}
	return false;
}

bool UrlUtils::getTableNameWithKeys(std::vector<utility::string_t> const paths, utility::string_t& table, utility::string_t& rowkey, utility::string_t& paritionkey)
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

bool UrlUtils::getTableNameWithoutKeys(std::vector<utility::string_t> const paths, utility::string_t& table)
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

bool UrlUtils::getAnalyze(std::vector<string_t> const paths, utility::string_t& tablename)
{
	if (paths.size() >= 2 && paths[1] == U("Analyze") && !paths[2].empty())
	{
		tablename = paths[2];
		return true;
	}
	return false;
}

bool UrlUtils::getCheck(std::vector<string_t> const paths, utility::string_t& tablename)
{
	if (paths.size() >= 2 && paths[1] == U("Check") && !paths[2].empty())
	{
		tablename = paths[2];
		return true;
	}
	return false;
}

bool UrlUtils::getRepair(std::vector<string_t> const paths, utility::string_t& tablename)
{
	if (paths.size() >= 2 && paths[1] == U("Repair") && !paths[2].empty())
	{
		tablename = paths[2];
		return true;
	}
	return false;
}

bool UrlUtils::getIndexes(std::vector<string_t> const paths, utility::string_t& tablename)
{
	if (paths.size() >= 2 && paths[1] == U("Indexes") && !paths[2].empty())
	{
		tablename = paths[2];
		return true;
	}
	return false;
}

bool UrlUtils::getKeys(std::vector<string_t> const paths, utility::string_t& tablename)
{
	if (paths.size() >= 2 && paths[1] == U("Keys") && !paths[2].empty())
	{
		tablename = paths[2];
		return true;
	}
	return false;
}

bool UrlUtils::getOptimize(std::vector<string_t> const paths, utility::string_t& tablename)
{
	if (paths.size() >= 2 && paths[1] == U("Optimize") && !paths[2].empty())
	{
		tablename = paths[2];
		return true;
	}
	return false;
}

bool UrlUtils::hasEngines(std::vector<string_t> const paths)
{
	if (paths.size() >= 2 && paths[1] == U("Engines"))
	{
		return true;
	}
	return false;
}

bool UrlUtils::hasStatus(std::vector<string_t> const paths)
{
	if (paths.size() >= 2 && paths[1] == U("Status"))
	{
		return true;
	}
	return false;
}

bool UrlUtils::hasPlugins(std::vector<string_t> const paths)
{
	if (paths.size() >= 2 && paths[1] == U("Plugins"))
	{
		return true;
	}
	return false;
}

bool UrlUtils::hasOpenTables(std::vector<string_t> const paths)
{
	if (paths.size() >= 2 && paths[1] == U("OpenTables"))
	{
		return true;
	}
	return false;
}