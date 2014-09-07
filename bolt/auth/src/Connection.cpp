#include <connection.hpp>
#include <configuration.hpp>

namespace bolt {
	namespace auth {
		std::unique_ptr<Connection> Connection::m_instance;
		std::once_flag Connection::m_instance_flag;

		Connection& Connection::GetInstance()
		{
			std::call_once(m_instance_flag,
				[] {
				m_instance.reset(new Connection);
			});
			return *m_instance.get();
		}

		Connection::Connection()
		{
			Config &config = Config::getInstance();
			std::string host = config.getStdConfigOption(Config::mysql_host);
			std::string user = config.getStdConfigOption(Config::mysql_user);
			std::string password = config.getStdConfigOption(Config::mysql_password);
			std::string database = config.getStdConfigOption(Config::mysql_database);

			setConnectionParams(host, user, password, database);
		}

		int Connection::setConnectionParams(std::string host, std::string username, std::string password, std::string db){
			sql::Driver* m_driver = get_driver_instance();
			m_connection = std::shared_ptr<sql::Connection>(m_driver->connect(host, username, password));
			/* Connect to the MySQL test database */
			m_connection->setSchema(db);
			return 0;
		}

		std::shared_ptr<sql::Connection> Connection::getConnection()
		{
			return m_connection;
		}
	}
}