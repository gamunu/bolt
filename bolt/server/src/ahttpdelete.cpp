#include <ahttpdelete.hpp>
#include <url_utils.hpp>
#include <azure_query.h>
using namespace bolt::storage::boltazure;

struct AzureFilter
{
	string_t property_name;
	string_t condition;
	string_t value;
};

bool AHttpDelete::deleteEntities(string_t tablename, map<string_t, string_t> const query)
{
	AzureQuery adelete = AzureQuery(tablename);

	map<string_t, string_t> filter_conditions;
	if (UrlUtils::getFilter(query, filter_conditions))
	{
		AzureFilter first_filter;
		AzureFilter second_filter;

		if (filter_conditions.find(U("first_attr")) != filter_conditions.cend()
			&& filter_conditions.find(U("first_con")) != filter_conditions.cend()
			&& filter_conditions.find(U("first_val")) != filter_conditions.cend())
		{
			first_filter.property_name = filter_conditions.find(U("first_attr"))->second;
			first_filter.condition = filter_conditions.find(U("first_con"))->second;
			first_filter.value = filter_conditions.find(U("first_val"))->second;
		}

		if (filter_conditions.find(U("second_attr")) != filter_conditions.cend()
			&& filter_conditions.find(U("second_con")) != filter_conditions.cend()
			&& filter_conditions.find(U("second_val")) != filter_conditions.cend())
		{
			second_filter.property_name = filter_conditions.find(U("second_attr"))->second;
			second_filter.condition = filter_conditions.find(U("second_con"))->second;
			second_filter.value = filter_conditions.find(U("second_val"))->second;
		}

		adelete.setFilterCondition(first_filter.property_name, first_filter.condition, first_filter.value);

		if (filter_conditions.find(U("join")) != filter_conditions.cend())
		{
			string_t join = filter_conditions.find(U("join"))->second;

			if (join == U("or"))
			{
				adelete.setOrFilterCondition(second_filter.property_name, second_filter.condition, second_filter.value);
			}

			if (join == U("and"))
			{
				adelete.setAndFilterCondition(second_filter.property_name, second_filter.condition, second_filter.value);
			}
		}
		if (adelete.executeDelete())
		{
			return true;
		}
	}
	return false;
}

string_t AHttpDelete::getCondition(string_t condition)
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