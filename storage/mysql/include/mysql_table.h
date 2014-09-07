#pragma once

#ifdef BOLTMYSQL_DLL
#define BOLTMYSQL_API __declspec( dllexport )
#else
#define BOLTMYSQL_API __declspec( dllimport )
#endif

#include <cpprest/asyncrt_utils.h>
#include <table.h>
#include <string>
#include <map>
#include <logger.hpp>
#include <mysql_table_entity.h>
#include <mysql_incognito_entity.h>

namespace bolt {
	namespace storage {
		namespace mysql {

			using namespace utility;

			class MysqlTable
			{
			private:
				BoltLog bolt_logger;
				string_t m_table_name;

				static std::vector<mysql_incognito_entity> innerMaintananceQuery(string_t sql);
			public:
				BOLTMYSQL_API MysqlTable();
				BOLTMYSQL_API MysqlTable(string_t table_name);
				BOLTMYSQL_API static bool createTable(string_t table_name);
				BOLTMYSQL_API std::vector<string_t> getTableList();
				BOLTMYSQL_API bool deleteTable(string_t table_name);
				BOLTMYSQL_API static std::vector<string_t> getColumns(string_t table_name);
				BOLTMYSQL_API static bool createColumn(string_t table_name, string_t column_name, myedm_type datatype);
				BOLTMYSQL_API static bool hasColumn(string_t table_name, string_t column_name);
				
				
				BOLTMYSQL_API static std::vector<mysql_incognito_entity> analyzeTable(string_t table_name);
				BOLTMYSQL_API static std::vector<mysql_incognito_entity> checkTable(string_t table_name);
				BOLTMYSQL_API static std::vector<mysql_incognito_entity> optimzeTable(string_t table_name);
				BOLTMYSQL_API static std::vector<mysql_incognito_entity> repairTable(string_t table_name);
				BOLTMYSQL_API static std::vector<mysql_incognito_entity> showIndexes(string_t table_name);
				BOLTMYSQL_API static std::vector<mysql_incognito_entity> showKeys(string_t table_name);
			};
		}
	}
}