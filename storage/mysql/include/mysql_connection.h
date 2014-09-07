#pragma once

#ifdef BOLTMYSQL_DLL
#define BOLTMYSQL_API __declspec( dllexport )
#else
#define BOLTMYSQL_API __declspec( dllimport )
#endif

#include <memory>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/resultset_metadata.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/metadata.h>
#include <memory>
#include <mutex>

namespace bolt {
	namespace storage {
		namespace mysql {

			class MysqlConnection
			{
			public:

				BOLTMYSQL_API virtual ~MysqlConnection() {}
				BOLTMYSQL_API static MysqlConnection& get_instance();


				/// <summary>
				/// Return this instance of database connection.
				/// </summary>
				/// <returns></returns>
				BOLTMYSQL_API std::shared_ptr<sql::Connection> getConnection();
			private:

				std::shared_ptr<sql::Connection> m_connection;

				MysqlConnection(void);

				MysqlConnection(const MysqlConnection& src);
				MysqlConnection &operator=(const MysqlConnection& rhs);
				static std::unique_ptr<MysqlConnection> m_instance;
				static std::once_flag m_instance_flag;


				/// <summary>
				/// Gets the connection.
				/// </summary>
				/// <param name="host">The host.</param>
				/// <param name="username">The username.</param>
				/// <param name="password">The password.</param>
				/// <param name="db">The database.</param>
				/// <returns></returns>
				int setConnectionParams(std::string host, std::string username, std::string password, std::string db);

			};
		}
	}
}