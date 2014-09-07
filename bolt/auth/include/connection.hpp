#pragma once

#ifdef BOLTAUTH_DLL
#define BOLTAUTH_API __declspec( dllexport )
#else
#define BOLTAUTH_API __declspec( dllimport )
#endif

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

#include <memory>
#include <mutex>


namespace bolt {
	namespace auth	{

		class Connection
		{
		public:
			std::shared_ptr<sql::Connection> m_connection;

			virtual ~Connection() {}
			static Connection& GetInstance();
		

			/// <summary>
			/// Return this instance of database connection.
			/// </summary>
			/// <returns></returns>
			std::shared_ptr<sql::Connection> getConnection();
		private:

			Connection(void);

			Connection(const Connection& src);
			Connection &operator=(const Connection& rhs);
			static std::unique_ptr<Connection> m_instance;
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
