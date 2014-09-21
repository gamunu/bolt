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

			public:
				BOLTMYSQL_API MysqlDelete();
				BOLTMYSQL_API ~MysqlDelete();

				BOLTMYSQL_API void selectTable(utility::string_t tblName);
				BOLTMYSQL_API void AddCondition(utility::string_t condition);
				BOLTMYSQL_API bool executeDelete();

				BOLTMYSQL_API MysqlDelete& from(utility::string_t tables);
				BOLTMYSQL_API MysqlDelete& where(utility::string_t conditions);
				BOLTMYSQL_API MysqlDelete& andWhere(utility::string_t conditions);
				BOLTMYSQL_API MysqlDelete& orWhere(utility::string_t conditions);

			private:
				class MDImpl;
				std::shared_ptr<MDImpl> dimpl;
			};
		}
	}
}