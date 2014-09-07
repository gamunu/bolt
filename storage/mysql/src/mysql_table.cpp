#include <mysql_table.h>
#include <mysql_connection.h>
#include <logger.hpp>

namespace bolt
{
	namespace storage
	{
		namespace mysql
		{
			MysqlTable::MysqlTable()
			{
			}

			MysqlTable::MysqlTable(string_t table_name)
			{
				m_table_name = table_name;
			}

			std::vector<mysql_incognito_entity> MysqlTable::innerMaintananceQuery(string_t sql)
			{
				std::vector<mysql_incognito_entity> entites;

				try
				{
					MysqlConnection& connection = MysqlConnection::get_instance();
					std::unique_ptr<sql::PreparedStatement> stmt(connection.getConnection()->prepareStatement(conversions::to_utf8string(sql)));
					std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());

					//vector<map<string,string>> data;
					std::map<std::string, std::string> column; //Meta data and data map<metadata, datastr>

					sql::ResultSetMetaData* res_meta = res->getMetaData();

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
				catch (sql::SQLException& e)
				{
					BoltLog logger;
					logger << BoltLog::LOG_ERROR << "SQLException in MysqlTable "
						<< "(optimzeTable) #ERR: " << e.what() << " (MySQL error code: "
						<< e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )";
				}

				return entites;
			}


			bool MysqlTable::createTable(string_t table_name)
			{
				try
				{
					MysqlConnection& connection = MysqlConnection::get_instance();
					sql::SQLString qery("CREATE TABLE IF NOT EXISTS " + conversions::to_utf8string(table_name)
						+ "(PartitionKey VARCHAR(255) NOT NULL, INDEX(PartitionKey(50)) , RowKey VARCHAR(255) PRIMARY KEY NOT NULL , `Timestamp` TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP)");

					std::unique_ptr<sql::Statement> pre_statmt(connection.getConnection()->createStatement());

					pre_statmt->execute(qery);
					return true;
				}
				catch (sql::SQLException& e)
				{
					BoltLog logger;
					logger << BoltLog::LOG_ERROR << "SQLException in MysqlQuery "
						<< "(executeQuery) #ERR: " << e.what() << " (MySQL error code: "
						<< e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )";
					return false;
				}
			}

			std::vector<string_t> MysqlTable::getTableList()
			{
				std::vector<string_t> tables;

				try
				{
					MysqlConnection& connection = MysqlConnection::get_instance();
					sql::SQLString query2("SHOW TABLES");
					std::unique_ptr<sql::Statement> stmt(connection.getConnection()->createStatement());
					std::unique_ptr<sql::ResultSet> res(stmt->executeQuery(query2));

					while (res->next())
					{
						tables.push_back(conversions::to_string_t(res->getString(1))); // TABLE_NAME
					}
				}
				catch (sql::SQLException& e)
				{
					bolt_logger << BoltLog::LOG_ERROR << "SQLException in MysqlTable "
						<< "(getTableList) #ERR: " << e.what() << " (MySQL error code: "
						<< e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )";
				}
				return tables;
			}


			bool MysqlTable::deleteTable(string_t table_name)
			{
				try
				{
					MysqlConnection& connection = MysqlConnection::get_instance();
					sql::SQLString qery("DROP TABLE IF EXISTS " + conversions::to_utf8string(table_name));

					std::unique_ptr<sql::Statement> pre_statmt(connection.getConnection()->createStatement());

					pre_statmt->execute(qery);
					return true;
				}
				catch (sql::SQLException& e)
				{
					bolt_logger << BoltLog::LOG_ERROR << "SQLException in MysqlQuery "
						<< "(executeQuery) #ERR: " << e.what() << " (MySQL error code: "
						<< e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )";
					return false;
				}
			}

			std::vector<string_t> MysqlTable::getColumns(string_t table_name)
			{
				std::vector<string_t> tables;

				try
				{
					MysqlConnection& connection = MysqlConnection::get_instance();
					sql::SQLString query2("SHOW COLUMNS IN " + conversions::to_utf8string(table_name));
					std::unique_ptr<sql::Statement> stmt(connection.getConnection()->createStatement());
					std::unique_ptr<sql::ResultSet> res(stmt->executeQuery(query2));

					while (res->next())
					{
						tables.push_back(conversions::to_string_t(res->getString(1)));
					}
				}
				catch (sql::SQLException& e)
				{
					BoltLog logger;
					logger << BoltLog::LOG_ERROR << "SQLException in MysqlTable "
						<< "(getTableList) #ERR: " << e.what() << " (MySQL error code: "
						<< e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )";
				}
				return tables;
			}

			bool MysqlTable::createColumn(string_t table_name, string_t column_name, myedm_type datatype)
			{
				try
				{
					MysqlConnection& connection = MysqlConnection::get_instance();

					sql::SQLString query("ALTER TABLE " + conversions::to_utf8string(table_name) +
						" ADD " + conversions::to_utf8string(column_name));

					switch (datatype)
					{
					case myedm_type::string:
						query += " TEXT";
						break;
					case myedm_type::boolean:
						query += " TINYINT(1)";
						break;
					case myedm_type::datetime:
						query += " TIMESTAMP";
						break;
					case myedm_type::double_floating_point:
						query += " DOUBLE";
						break;
					case myedm_type::guid:
						query += " VARCHAR(255)";
						break;
					case myedm_type::int32:
						query += " INT";
						break;
					case myedm_type::int64:
						query += " BIGINT";
						break;
					default:
						//TODO: this will omit the column name
						//Find a column data type for default
						break;
					}

					std::unique_ptr<sql::Statement> pre_statmt(connection.getConnection()->createStatement());

					pre_statmt->execute(query);
					return true;
				}
				catch (sql::SQLException& e)
				{
					BoltLog logger;
					logger << BoltLog::LOG_ERROR << "SQLException in MysqlQuery "
						<< "(executeQuery) #ERR: " << e.what() << " (MySQL error code: "
						<< e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )";
					return false;
				}
			}

			bool MysqlTable::hasColumn(string_t table_name, string_t column_name)
			{
				auto columns_in_table = getColumns(table_name);

				for (auto column : columns_in_table)
				{
					if (column == column_name)
					{
						return true;
					}
				}
				return false;
			}

			std::vector<mysql_incognito_entity> MysqlTable::analyzeTable(string_t table_name)
			{
				string_t sql = U("ANALYZE TABLE ") + table_name;
				return innerMaintananceQuery(sql);
			}

			std::vector<mysql_incognito_entity> MysqlTable::checkTable(string_t table_name)
			{
				string_t sql = U("CHECK TABLE ") + table_name;
				return innerMaintananceQuery(sql);
			}

			std::vector<mysql_incognito_entity> MysqlTable::optimzeTable(string_t table_name)
			{
				string_t sql = U("OPTIMIZE TABLE ") + table_name;

				return innerMaintananceQuery(sql);
			}

			std::vector<mysql_incognito_entity> MysqlTable::repairTable(string_t table_name)
			{
				string_t sql = U("REPAIR TABLE ") + table_name;
				return innerMaintananceQuery(sql);
			}

			std::vector<mysql_incognito_entity> MysqlTable::showIndexes(string_t table_name)
			{
				string_t sql = U("SHOW INDEXES IN ") + table_name;
				return innerMaintananceQuery(sql);
			}

			std::vector<mysql_incognito_entity> MysqlTable::showKeys(string_t table_name)
			{
				string_t sql = U("SHOW KEYS IN ") + table_name;
				return innerMaintananceQuery(sql);
			}
		}
	}
}