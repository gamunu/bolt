#pragma once

#ifdef BOLTMYSQL_DLL
#define BOLTMYSQL_API __declspec( dllexport )
#else
#define BOLTMYSQL_API __declspec( dllimport )
#endif

#include <deque>
#include <vector>
#include <map>
#include <string>
#include <cpprest/asyncrt_utils.h>
#include <logger.hpp>
#include <mysql_result.h>
#include <mysql_table_entity.h>

namespace bolt  {
	namespace storage {
		namespace mysql {

			class MysqlQuery
			{

			public:
				BOLTMYSQL_API MysqlQuery();
				BOLTMYSQL_API ~MysqlQuery();


				BOLTMYSQL_API MysqlQuery& select(utility::string_t columns = U("*"));
				BOLTMYSQL_API MysqlQuery& selectDistinct(utility::string_t columns);
				BOLTMYSQL_API MysqlQuery& from(utility::string_t tables);
				BOLTMYSQL_API MysqlQuery& where(utility::string_t conditions);
				BOLTMYSQL_API MysqlQuery& andWhere(utility::string_t conditions);
				BOLTMYSQL_API MysqlQuery& orWhere(utility::string_t conditions);

				BOLTMYSQL_API MysqlQuery& join(utility::string_t table, utility::string_t conditions);
				BOLTMYSQL_API MysqlQuery& leftJoin(utility::string_t table, utility::string_t conditions);
				BOLTMYSQL_API MysqlQuery& rightJoin(utility::string_t table, utility::string_t conditions);
				BOLTMYSQL_API MysqlQuery& crossJoin(utility::string_t table);
				BOLTMYSQL_API MysqlQuery& naturalJoin(utility::string_t table);

				BOLTMYSQL_API MysqlQuery& group(utility::string_t columns);
				BOLTMYSQL_API MysqlQuery& having(utility::string_t conditions);
				BOLTMYSQL_API MysqlQuery& order(utility::string_t columns);
				BOLTMYSQL_API MysqlQuery& limit(utility::string_t limit, utility::string_t offset = U(""));
				BOLTMYSQL_API MysqlQuery& offset(utility::string_t offset);
				BOLTMYSQL_API MysqlQuery& munion(utility::string_t sql);


				BOLTMYSQL_API std::deque<mysql_table_entity> filterByKey(utility::string_t partition_key, utility::string_t row_key);
				BOLTMYSQL_API std::deque<mysql_table_entity> queryAll();
				
			private:
				class MQImpl;
				std::shared_ptr<MQImpl> qimpl;
			};
		}
	}
}