#pragma once

#ifdef BOLTMYSQL_DLL
#define BOLTMYSQL_API __declspec( dllexport )
#else
#define BOLTMYSQL_API __declspec( dllimport )
#endif

#include <cpprest/asyncrt_utils.h>
#include <string>
#include <logger.hpp>

namespace bolt {
	namespace storage {
		namespace mysql {

			class MysqlDelete
			{

			private:
				utility::string_t icondition;
				utility::string_t iTableName;
				BoltLog bolt_logger;

			public:

				BOLTMYSQL_API void selectTable(utility::string_t tblName);
				BOLTMYSQL_API void AddCondition(utility::string_t condition);
				BOLTMYSQL_API bool executeDelete();


			};
		}
	}
}