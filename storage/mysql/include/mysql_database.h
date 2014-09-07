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
#include <mysql_incognito_entity.h>

using namespace utility;

namespace bolt {
	namespace storage {
		namespace mysql {
			class MySqlDB
			{
			private:
				BoltLog bolt_logger;
				string_t m_table_name;

				static std::vector<mysql_incognito_entity> innerQuery(string_t sql);
			public:
				MySqlDB();

				BOLTMYSQL_API static std::vector<mysql_incognito_entity> showEngines();
				BOLTMYSQL_API static std::vector<mysql_incognito_entity> showTableStatus();
				BOLTMYSQL_API static std::vector<mysql_incognito_entity> showPlugins();
				BOLTMYSQL_API static std::vector<mysql_incognito_entity> showOpenTables();
			};
		}
	}
}