#pragma once

#ifdef BOLTMYSQL_DLL
#define BOLTMYSQL_API __declspec( dllexport )
#else
#define BOLTMYSQL_API __declspec( dllimport )
#endif
#include <iostream>
#include <cpprest/asyncrt_utils.h>
#include <map>
#include <string>
#include <logger.hpp>

namespace bolt {
	namespace storage {
		namespace mysql {

			class MysqlUpdate
			{
			public:

				BOLTMYSQL_API void selectTable(utility::string_t tblName);
				BOLTMYSQL_API void addInteger(utility::string_t fieldName, int value);
				BOLTMYSQL_API void addDouble(utility::string_t fieldName, long double value);
				BOLTMYSQL_API void addBoolean(utility::string_t fieldName, bool value);
				BOLTMYSQL_API void addString(utility::string_t fieldName, utility::string_t value);
				BOLTMYSQL_API void addCondition(utility::string_t condition);
				BOLTMYSQL_API bool updateTable();
				BOLTMYSQL_API MysqlUpdate() {}

			private:
			//	std::vector<MysqlField> ifields;
				utility::string_t iTableName;
				utility::string_t icondition;
				BoltLog bolt_logger;

			};
		}
	}
}