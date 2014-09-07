#include <iostream>
#include <cpprest/asyncrt_utils.h>
#include <metadata.hpp>
#include <memory>
#include <boost/test/unit_test.hpp>
using namespace std;
using namespace web;

BOOST_AUTO_TEST_SUITE(ClassMetadata)

BOOST_AUTO_TEST_CASE(getMysqlQueryResults_Without_Select_From)
{
	const auto &query = json::value::object();
	json::value result;

	BOOST_CHECK(!Metadata::getMysqlQueryResults(query.as_object(), result));
}
/*
BOOST_AUTO_TEST_CASE(getMysqlQueryResults_Without_Select_Include_From)
{
	auto query = json::value::object();
	query[U("From")] = json::value::string(U("employees"));
	json::value result;
	BOOST_CHECK(Metadata::getMysqlQueryResults(query.as_object(), result));
}
*/
BOOST_AUTO_TEST_SUITE_END()
