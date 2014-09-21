#include <mhttpdelete.hpp>
#include <mysql_delete.h>
#include <url_utils.hpp>
using namespace bolt::storage::mysql;

bool MHttpDelete::deleteEntities(utility::string_t tablename, std::map<utility::string_t, utility::string_t> const query)
{
	MysqlDelete mdelete = MysqlDelete();

	mdelete.from(tablename);

	map<string_t, string_t> filter_conditions;
	if (UrlUtils::getFilter(query, filter_conditions))
	{
		string_t firstwhere;
		string_t secondwhere;


		if (filter_conditions.find(U("first_attr")) != filter_conditions.cend()
			&& filter_conditions.find(U("first_con")) != filter_conditions.cend()
			&& filter_conditions.find(U("first_val")) != filter_conditions.cend())
		{
			firstwhere = filter_conditions.find(U("first_attr"))->second;

			string_t first_con = filter_conditions.find(U("first_con"))->second;

			firstwhere += getCondition(first_con);

			firstwhere += filter_conditions.find(U("first_val"))->second;
		}

		if (filter_conditions.find(U("second_attr")) != filter_conditions.cend()
			&& filter_conditions.find(U("second_con")) != filter_conditions.cend()
			&& filter_conditions.find(U("second_val")) != filter_conditions.cend())
		{
			secondwhere = filter_conditions.find(U("second_attr"))->second;

			string_t second_con = filter_conditions.find(U("second_con"))->second;
			secondwhere += getCondition(second_con);

			secondwhere += filter_conditions.find(U("second_val"))->second;
		}

		mdelete.where(firstwhere);

		if (filter_conditions.find(U("join")) != filter_conditions.cend())
		{
			string_t join = filter_conditions.find(U("join"))->second;

			if (join == U("or"))
			{
				mdelete.orWhere(secondwhere);
			}

			if (join == U("and"))
			{
				mdelete.andWhere(secondwhere);
			}
		}

		if (mdelete.executeDelete())
		{
			return true;
		}
	}

	return false;
}

string_t MHttpDelete::getCondition(string_t condition)
{
	if (condition == U("le"))
		return U(" <= ");
	if (condition == U(" lt "))
		return U(" < ");
	if (condition == U("ge"))
		return U(" >= ");
	if (condition == U("gt"))
		return U(" > ");
	if (condition == U("ne"))
		return U(" != ");
	return U(" = "); //eq
}
