#include <mysql_connection.h>

namespace bolt {
	namespace storage {
		namespace mysql {
			std::unique_ptr<MysqlConnection> MysqlConnection::m_instance;
			std::once_flag MysqlConnection::m_instance_flag;

			MysqlConnection& MysqlConnection::get_instance()
			{
				call_once(m_instance_flag, [] {m_instance.reset(new MysqlConnection); });
				return *m_instance.get();
			}

			MysqlConnection::MysqlConnection()
			{
				setConnectionParams("localhost", "root", "knight", "boltdata");
			}

			int MysqlConnection::setConnectionParams(std::string host, std::string username, std::string password, std::string db){
				sql::Driver* m_driver = get_driver_instance();
				m_connection = std::shared_ptr<sql::Connection>(m_driver->connect(host, username, password));
				/* Connect to the MySQL test database */
				m_connection->setSchema(db);
				return 0;
			}

			/// <summary>
			/// Returns the connection.
			/// </summary>
			/// <returns></returns>
			std::shared_ptr<sql::Connection> MysqlConnection::getConnection()
			{
				return m_connection;
			}
		}
	}
}