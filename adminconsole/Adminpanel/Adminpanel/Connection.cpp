#include "stdafx.h"
#include "Connection.h"

#include <iostream>
#include <string>


		/// <summary>
		/// Gets the connection.
		/// </summary>
		/// <param name="host">The host.</param>
		/// <param name="username">The username.</param>
		/// <param name="password">The password.</param>
		/// <param name="db">The database.</param>
		/// <returns></returns>

		int Connection::setConnectionParams(char *host,char *username,char *password,char *db){
			m_driver = get_driver_instance();
			m_connection = m_driver->connect(host,username,password);
			/* Connect to the MySQL test database */
			m_connection->setSchema(db);
			return 0;		
		}


		/// <summary>
		/// Return this instance of database connection.
		/// </summary>
		/// <returns></returns>
		sql::Connection *Connection::getConnection()
		{
			//if(m_connection->isClosed()){

			Connection::setConnectionParams("localhost","root","knight","auth");
			
			//}

			return m_connection;
		}

