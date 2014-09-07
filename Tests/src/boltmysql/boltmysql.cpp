#define BOOST_TEST_MODULE MyTest
#define BOOST_TEST_MAIN
#include <iostream>
#include <mysql_query.h>
#include <cpprest/asyncrt_utils.h>
#include <memory>
#include <boost/test/unit_test.hpp>
using namespace std;
using namespace bolt::storage::mysql;
BOOST_AUTO_TEST_SUITE(ClassMysqlQuery)

BOOST_AUTO_TEST_CASE(SELECTALL)
{
	auto query = MysqlQuery();
	query.select().from(U("employees"));
	utility::string_t q = U("SELECT *\nFROM employees");
	BOOST_CHECK(query.buildQuery() == q);
}

BOOST_AUTO_TEST_CASE(SELECTCOLUMNS)
{
	auto query = MysqlQuery();
	query.select(U("emp_no, first_name, gender, hire_date")).from(U("employees"));
	utility::string_t q = U("SELECT emp_no, first_name, gender, hire_date\nFROM employees");
	BOOST_CHECK(query.buildQuery() == q);
}

BOOST_AUTO_TEST_CASE(SELECTWHEREALL)
{
	auto query = MysqlQuery();
	query.select().from(U("employees")).where(U("emp_no <> 10001"));
	utility::string_t q = U("SELECT *\nFROM employees\nWHERE emp_no <> 10001");
	BOOST_CHECK(query.buildQuery() == q);
}

/*
BOOST_AUTO_TEST_CASE(SELECTDISRINCTWHEREALL)
{
	auto query = MysqlQuery();
	query.selectDistinct().from(U("employees")).where(U("emp_no <> 10001"));
	utility::string_t q = U("SELECT * \nFROM employees\nWHERE emp_no <> 10001");
	BOOST_CHECK(query.buildQuery() == q);
}
*/

BOOST_AUTO_TEST_CASE(SELECTDISTINCTCOLUMNS)
{
	auto query = MysqlQuery();
	query.selectDistinct(U("emp_no, first_name, gender, hire_date")).from(U("employees"));
	utility::string_t q = U("SELECT DISTINCT emp_no, first_name, gender, hire_date\nFROM employees");
	BOOST_CHECK(query.buildQuery() == q);
}


BOOST_AUTO_TEST_CASE(SELECTWHERECOLUMNS)
{
	auto query = MysqlQuery();
	query.select(U("emp_no, first_name")).from(U("employees")).where(U("emp_no <> 10001"));
	utility::string_t q = U("SELECT emp_no, first_name\nFROM employees\nWHERE emp_no <> 10001");
	BOOST_CHECK(query.buildQuery() == q);
}

BOOST_AUTO_TEST_CASE(SELECTANDWHERE)
{
	auto query = MysqlQuery();
	query.select().from(U("employees")).where(U("emp_no <> 10001")).andWhere(U("emp_no >= 10434"));
	utility::string_t q = U("SELECT *\nFROM employees\nWHERE emp_no <> 10001 AND emp_no >= 10434");
	BOOST_CHECK(query.buildQuery() == q);
}

BOOST_AUTO_TEST_CASE(SELECTORWHERE)
{
	auto query = MysqlQuery();
	query.select().from(U("employees")).where(U("emp_no <> 10001")).orWhere(U("emp_no >= 10434"));
	utility::string_t q = U("SELECT *\nFROM employees\nWHERE emp_no <> 10001 OR emp_no >= 10434");
	BOOST_CHECK(query.buildQuery() == q);
}

BOOST_AUTO_TEST_CASE(SELECTORWHEREANDWHERE)
{
	auto query = MysqlQuery();
	query.select().from(U("employees")).where(U("emp_no <> 10001")).orWhere(U("emp_no >= 10434")).andWhere(U("emp_no <> 10434"));
	utility::string_t q = U("SELECT *\nFROM employees\nWHERE emp_no <> 10001 OR emp_no >= 10434 AND emp_no <> 10434");
	BOOST_CHECK(query.buildQuery() == q);
}

BOOST_AUTO_TEST_CASE(SELECTJOIN)
{
	auto query = MysqlQuery();
	query.select().from(U("employees e")).join(U("titles t"), U("e.emp_no = t.emp_no"));
	utility::string_t q = U("SELECT *\nFROM employees e\n JOIN titles t ON e.emp_no = t.emp_no");
	BOOST_CHECK(query.buildQuery() == q);
}

BOOST_AUTO_TEST_CASE(SELECTLEFTJOIN)
{
	auto query = MysqlQuery();
	query.select().from(U("employees e")).leftJoin(U("titles t"), U("e.emp_no = t.emp_no"));
	utility::string_t q = U("SELECT *\nFROM employees e\n LEFT JOIN titles t ON e.emp_no = t.emp_no");
	BOOST_CHECK(query.buildQuery() == q);
}

BOOST_AUTO_TEST_CASE(SELECTRIGHTJOIN)
{
	auto query = MysqlQuery();
	query.select().from(U("employees e")).rightJoin(U("titles t"), U("e.emp_no = t.emp_no"));
	utility::string_t q = U("SELECT *\nFROM employees e\n RIGHT JOIN titles t ON e.emp_no = t.emp_no");
	BOOST_CHECK(query.buildQuery() == q);
}

BOOST_AUTO_TEST_CASE(SELECTCROSSJOIN)
{
	auto query = MysqlQuery();
	query.select().from(U("employees e")).crossJoin(U("titles t"));
	utility::string_t q = U("SELECT *\nFROM employees e\n CROSS JOIN titles t");
	BOOST_CHECK(query.buildQuery() == q);
}

BOOST_AUTO_TEST_CASE(SELECTNATURALJOIN)
{
	auto query = MysqlQuery();
	query.select().from(U("employees e")).naturalJoin(U("titles t"));
	utility::string_t q = U("SELECT *\nFROM employees e\n NATURAL JOIN titles t");
	BOOST_CHECK(query.buildQuery() == q);
}

BOOST_AUTO_TEST_CASE(SELECTJOINWHERE)
{
	auto query = MysqlQuery();
	query.select().from(U("employees e")).join(U("titles t"), U("e.emp_no = t.emp_no")).where(U("e.emp_no < 10434"));
	utility::string_t q = U("SELECT *\nFROM employees e\n JOIN titles t ON e.emp_no = t.emp_no\nWHERE e.emp_no < 10434");
	BOOST_CHECK(query.buildQuery() == q);
}

BOOST_AUTO_TEST_CASE(SELECTGROUPBY)
{
	auto query = MysqlQuery();
	query.select(U("count(emp_no) as empno, first_name")).from(U("employees")).where(U("emp_no <> 10001")).group(U("first_name"));
	utility::string_t q = U("SELECT count(emp_no) as empno, first_name\nFROM employees\nWHERE emp_no <> 10001\nGROUP BY first_name");
	BOOST_CHECK(query.buildQuery() == q);
}

BOOST_AUTO_TEST_CASE(SELECTHAVING)
{
	auto query = MysqlQuery();
	query.select().from(U("salaries")).where(U("emp_no >= 10001")).having(U("sum(salary) > 1000"));
	utility::string_t q = U("SELECT *\nFROM salaries\nWHERE emp_no >= 10001\nHAVING sum(salary) > 1000");

	BOOST_CHECK(query.buildQuery() == q);
}

BOOST_AUTO_TEST_CASE(SELECTUNION)
{
	auto query = MysqlQuery();
	query.select(U("emp_no")).from(U("employees")).munion(U("SELECT emp_no FROM salaries"));
	utility::string_t q = U("SELECT emp_no\nFROM employees\nUNION (SELECT emp_no FROM salaries)");
	BOOST_CHECK(query.buildQuery() == q);
}

BOOST_AUTO_TEST_CASE(SELECTORDERBY)
{

	auto query = MysqlQuery();
	query.select(U("emp_no, first_name")).from(U("employees")).where(U("emp_no <> 10001")).order(U("first_name"));
	utility::string_t q = U("SELECT emp_no, first_name\nFROM employees\nWHERE emp_no <> 10001\nORDER BY first_name");
	BOOST_CHECK(query.buildQuery() == q);
}

BOOST_AUTO_TEST_CASE(SELECTLIMIT)
{

	auto query = MysqlQuery();
	query.select(U("emp_no, first_name")).from(U("employees")).limit(U("10"));
	utility::string_t q = U("SELECT emp_no, first_name\nFROM employees LIMIT 10");
	BOOST_CHECK(query.buildQuery() == q);
}

BOOST_AUTO_TEST_CASE(SELECTLIMITOFFSET)
{

	auto query = MysqlQuery();
	query.select(U("emp_no, first_name")).from(U("employees")).limit(U("10"), U("10"));
	utility::string_t q = U("SELECT emp_no, first_name\nFROM employees LIMIT 10 OFFSET 10");
	BOOST_CHECK(query.buildQuery() == q);
}

BOOST_AUTO_TEST_CASE(SELECTOFFSET)
{

	auto query = MysqlQuery();
	query.select(U("emp_no, first_name")).from(U("employees")).offset(U("10"));
	utility::string_t q = U("SELECT emp_no, first_name\nFROM employees OFFSET 10");
	BOOST_CHECK(query.buildQuery() == q);
}

BOOST_AUTO_TEST_SUITE_END()



BOOST_AUTO_TEST_SUITE(ClassMysqlQueryQueryAll)

BOOST_AUTO_TEST_CASE(SELECTALLQUERY)
{
	auto query = MysqlQuery();
	query.select().from(U("dept_manager"));
	utility::string_t q = U("SELECT *\nFROM dept_manager");

	vector<mysql_table_entity> ss = query.queryAll();
	BOOST_CHECK(!ss.empty());
}

BOOST_AUTO_TEST_CASE(SELECTCOLUMNS)
{
	auto query = MysqlQuery();
	query.select(U("dept_no, dept_name")).from(U("dept_manager"));

	vector<mysql_table_entity> ss = query.queryAll();
	BOOST_CHECK(!ss.empty());
}

BOOST_AUTO_TEST_CASE(SELECTWHEREALL)
{
	auto query = MysqlQuery();
	query.select().from(U("dept_manager")).where(U("dept_no <> d007"));

	vector<mysql_table_entity> ss = query.queryAll();
	BOOST_CHECK(!ss.empty());
}

BOOST_AUTO_TEST_CASE(SELECTDISTINCTCOLUMNS)
{
	auto query = MysqlQuery();
	query.selectDistinct(U("dept_no, dept_name")).from(U("dept_manager"));

	vector<mysql_table_entity> ss = query.queryAll();
	BOOST_CHECK(!ss.empty());
}


BOOST_AUTO_TEST_CASE(SELECTWHERECOLUMNS)
{
	auto query = MysqlQuery();
	query.select(U("dept_no, dept_name")).from(U("dept_manager")).where(U("dept_no <> d007"));

	vector<mysql_table_entity> ss = query.queryAll();
	BOOST_CHECK(!ss.empty());
}

BOOST_AUTO_TEST_CASE(SELECTANDWHERE)
{
	auto query = MysqlQuery();
	query.select().from(U("employees")).where(U("emp_no <> 10001")).andWhere(U("emp_no >= 10434")).limit(U("1000"));
	
	vector<mysql_table_entity> ss = query.queryAll();
	BOOST_CHECK(!ss.empty());
}

BOOST_AUTO_TEST_CASE(SELECTORWHERE)
{
	auto query = MysqlQuery();
	query.select().from(U("employees")).where(U("emp_no <> 10001")).orWhere(U("emp_no >= 10434")).limit(U("1000"));
	
	vector<mysql_table_entity> ss = query.queryAll();
	BOOST_CHECK(!ss.empty());
}

BOOST_AUTO_TEST_CASE(SELECTORWHEREANDWHERE)
{
	auto query = MysqlQuery();
	query.select().from(U("employees")).where(U("emp_no <> 10001")).orWhere(U("emp_no >= 10434")).andWhere(U("emp_no <> 10434")).limit(U("1000"));
	
	vector<mysql_table_entity> ss = query.queryAll();
	BOOST_CHECK(!ss.empty());
}

BOOST_AUTO_TEST_CASE(SELECTJOIN)
{
	auto query = MysqlQuery();
	query.select().from(U("employees e")).join(U("titles t"), U("e.emp_no = t.emp_no")).limit(U("1000"));

	vector<mysql_table_entity> ss = query.queryAll();
	BOOST_CHECK(!ss.empty());
}

BOOST_AUTO_TEST_CASE(SELECTLEFTJOIN)
{
	auto query = MysqlQuery();
	query.select().from(U("employees e")).leftJoin(U("titles t"), U("e.emp_no = t.emp_no")).limit(U("1000"));

	vector<mysql_table_entity> ss = query.queryAll();
	BOOST_CHECK(!ss.empty());
}

BOOST_AUTO_TEST_CASE(SELECTRIGHTJOIN)
{
	auto query = MysqlQuery();
	query.select().from(U("employees e")).rightJoin(U("titles t"), U("e.emp_no = t.emp_no")).limit(U("1000"));

	vector<mysql_table_entity> ss = query.queryAll();
	BOOST_CHECK(!ss.empty());
}

BOOST_AUTO_TEST_CASE(SELECTCROSSJOIN)
{
	auto query = MysqlQuery();
	query.select().from(U("employees e")).crossJoin(U("titles t")).limit(U("1000"));
	
	vector<mysql_table_entity> ss = query.queryAll();
	BOOST_CHECK(!ss.empty());
}

BOOST_AUTO_TEST_CASE(SELECTNATURALJOIN)
{
	auto query = MysqlQuery();
	query.select().from(U("employees e")).naturalJoin(U("titles t")).limit(U("1000"));
	
	vector<mysql_table_entity> ss = query.queryAll();
	BOOST_CHECK(!ss.empty());
}

BOOST_AUTO_TEST_CASE(SELECTJOINWHERE)
{
	auto query = MysqlQuery();
	query.select().from(U("employees e")).join(U("titles t"), U("e.emp_no = t.emp_no")).where(U("e.emp_no < 10434")).limit(U("1000"));
	
	vector<mysql_table_entity> ss = query.queryAll();
	BOOST_CHECK(!ss.empty());
}

BOOST_AUTO_TEST_CASE(SELECTGROUPBY)
{
	auto query = MysqlQuery();
	query.select(U("count(emp_no) as empno, first_name")).from(U("employees")).where(U("emp_no <> 10001")).group(U("first_name")).limit(U("1000"));
	
	vector<mysql_table_entity> ss = query.queryAll();
	BOOST_CHECK(!ss.empty());
}

BOOST_AUTO_TEST_CASE(SELECTHAVING)
{
	auto query = MysqlQuery();
	query.select().from(U("salaries")).where(U("emp_no >= 10001")).having(U("sum(salary) > 1000")).limit(U("1000"));
	
	vector<mysql_table_entity> ss = query.queryAll();
	BOOST_CHECK(!ss.empty());
}

BOOST_AUTO_TEST_CASE(SELECTUNION)
{
	auto query = MysqlQuery();
	query.select(U("emp_no")).from(U("employees")).munion(U("SELECT emp_no FROM salaries")).limit(U("1000"));
	
	vector<mysql_table_entity> ss = query.queryAll();
	BOOST_CHECK(!ss.empty());
}

BOOST_AUTO_TEST_CASE(SELECTORDERBY)
{

	auto query = MysqlQuery();
	query.select(U("emp_no, first_name")).from(U("employees")).where(U("emp_no <> 10001")).order(U("first_name")).limit(U("1000"));
	
	vector<mysql_table_entity> ss = query.queryAll();
	BOOST_CHECK(!ss.empty());
}

BOOST_AUTO_TEST_CASE(SELECTLIMIT)
{

	auto query = MysqlQuery();
	query.select(U("emp_no, first_name")).from(U("employees")).limit(U("10"));
	
	vector<mysql_table_entity> ss = query.queryAll();
	BOOST_CHECK(!ss.empty());
}

BOOST_AUTO_TEST_CASE(SELECTLIMITOFFSET)
{

	auto query = MysqlQuery();
	query.select(U("emp_no, first_name")).from(U("employees")).limit(U("10"), U("10"));
	
	vector<mysql_table_entity> ss = query.queryAll();
	BOOST_CHECK(!ss.empty());
}

BOOST_AUTO_TEST_SUITE_END()