#include <mysql_database.h>
#include <mysql_connection.h>
#include <logger.hpp>

namespace bolt
{
	namespace storage
	{
		namespace mysql
		{
			MySqlDB::MySqlDB()
			{
			}

			std::vector<mysql_incognito_entity> MySqlDB::innerQuery(string_t sql)
			{
				std::vector<mysql_incognito_entity> entites;

				try {
					MysqlConnection &connection = MysqlConnection::get_instance();
					std::unique_ptr<sql::PreparedStatement> stmt(connection.getConnection()->prepareStatement(conversions::to_utf8string(sql)));
					std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());

					//vector<map<string,string>> data;
					std::map<std::string, std::string> column; //Meta data and data map<metadata, datastr>

					sql::ResultSetMetaData *res_meta = res->getMetaData();

					size_t column_count = res_meta->getColumnCount();
					while (res->next())
					{
						mysql_incognito_entity::properties_type property_type;
						for (int i = 1; i <= column_count; i++)
						{
							mysql_property property;

							string_t column_name = conversions::to_string_t(res_meta->getColumnName(i));
							property.set_value(conversions::to_string_t(res->getString(i)));
							property_type.insert(make_pair(column_name, property));
						}
						mysql_incognito_entity incoent(property_type);
						entites.push_back(incoent);
					}
				}
				catch (sql::SQLException &e)
				{
					BoltLog logger;
					logger << BoltLog::LOG_ERROR << "SQLException in MysqlTable "
						<< "(optimzeTable) #ERR: " << e.what() << " (MySQL error code: "
						<< e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )";
				}

				return entites;
			}

			std::vector<mysql_incognito_entity> MySqlDB::showEngines()
			{
				string_t sql = U("SHOW ENGINES");
				return innerQuery(sql);
			}

			std::vector<mysql_incognito_entity> MySqlDB::showTableStatus()
			{
				string_t sql = U("SHOW TABLE STATUS");
				return innerQuery(sql);
			}

			std::vector<mysql_incognito_entity> MySqlDB::showPlugins()
			{
				string_t sql = U("SHOW PLUGINS");
				return innerQuery(sql);
			}

			std::vector<mysql_incognito_entity> MySqlDB::showOpenTables()
			{
				string_t sql = U("SHOW OPEN TABLES FROM auth");
				return innerQuery(sql);
			}
		}
	}
}