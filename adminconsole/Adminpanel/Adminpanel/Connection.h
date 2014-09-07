#pragma once

#include <string>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>


class Connection
{

	private:
			sql::Driver *m_driver;
			sql::Connection *m_connection;
	
	public:
			
			int setConnectionParams(char *host,char *username,char *password,char *db);
			sql::Connection *getConnection();
};

