#include <mysql_entity.h>
#include <mysql_connection.h>
#include <mysql_table.h>

namespace bolt
{
	namespace storage
	{
		namespace mysql
		{
			class MysqlEntity::MEImpl
			{
			public:
				BoltLog bolt_logger;
				string_t table_name;
				mysql_table_entity table_entity;

				string_t trans_partition_key;
				string_t trans_row_key;
				string_t trans_table_name;
			};

			MysqlEntity::MysqlEntity(string_t table_name) : eimpl{ new MEImpl }
			{
			}

			MysqlEntity::MysqlEntity(string_t table_name, string_t PartitonKey, string_t RowKey, size_t size) : eimpl{ new MEImpl }
			{
				// Insert a table entity
				eimpl->table_name = table_name;
				mysql_table_entity entity(PartitonKey, RowKey);
				entity.properties().reserve(size);
				eimpl->table_entity = entity;
			}

			MysqlEntity::~MysqlEntity() //!!
			{
			}

			void MysqlEntity::insertInt32(string_t PropertyName, int32_t value)
			{
				eimpl->table_entity.properties().insert(mysql_table_entity::property_type(PropertyName, mysql_property(value)));
			}

			void MysqlEntity::insertInt64(string_t PropertyName, int64_t value)
			{
				eimpl->table_entity.properties().insert(mysql_table_entity::property_type(PropertyName, mysql_property(value)));
			}

			void MysqlEntity::insertBoolean(string_t PropertyName, bool value)
			{
				eimpl->table_entity.properties().insert(mysql_table_entity::property_type(PropertyName, mysql_property(value)));
			}

			void MysqlEntity::insertTimestamp(string_t PropertyName, datetime value)
			{
				eimpl->table_entity.properties().insert(mysql_table_entity::property_type(PropertyName, mysql_property(value)));
			}

			void MysqlEntity::insertString(string_t PropertyName, string_t value)
			{
				eimpl->table_entity.properties().insert(mysql_table_entity::property_type(PropertyName, mysql_property(value)));
			}

			void MysqlEntity::insertDouble(string_t PropertyName, double value)
			{
				eimpl->table_entity.properties().insert(mysql_table_entity::property_type(PropertyName, mysql_property(value)));
			}

			bool MysqlEntity::executeEntity()
			{
				if (entityExists(eimpl->table_entity.row_key(), eimpl->table_entity.partition_key()))
				{
					return patchEntity();
				}
				auto properties = eimpl->table_entity.properties();

				for (const auto& property : properties)
				{
					if (!MysqlTable::hasColumn(eimpl->table_name, property.first))
						MysqlTable::createColumn(eimpl->table_name, property.first, property.second.property_type());
				}


				std::string c_values = "";
				std::string c_columns = "";

				for (auto iter = properties.cbegin(); iter != properties.cend(); ++iter)
				{
					c_columns += "`" + conversions::to_utf8string(iter->first) + "`,";
					c_values += "?,";
				}

				//Add postition to column and value 
				c_columns += "PartitionKey,RowKey";
				c_values += "?,?";

				sql::SQLString qery("INSERT INTO " + conversions::to_utf8string(eimpl->table_name) + "(" + c_columns +
					") VALUES(" + c_values + ")");

				try
				{
					MysqlConnection& connection = MysqlConnection::get_instance();
					std::unique_ptr<sql::PreparedStatement> pre_statmt(connection.getConnection()->prepareStatement(qery));

					int index = 1;
					for (auto iter = properties.cbegin(); iter != properties.cend(); ++iter, ++index)
					{
						switch (iter->second.property_type())
						{
						case myedm_type::boolean:
							pre_statmt->setBoolean(index, iter->second.boolean_value());
							break;
						case myedm_type::datetime:
							pre_statmt->setDateTime(index, conversions::to_utf8string(iter->second.datetime_value().to_string()));
							break;
						case myedm_type::double_floating_point:
							pre_statmt->setDouble(index, iter->second.double_value());
							break;
						case myedm_type::int32:
							pre_statmt->setInt(index, iter->second.int32_value());
							break;
						case myedm_type::int64:
							pre_statmt->setInt64(index, iter->second.int64_value());
							break;
						default:
							pre_statmt->setString(index, conversions::to_utf8string(iter->second.string_value()));
						}
					}

					pre_statmt->setString(index, conversions::to_utf8string(eimpl->table_entity.partition_key()));
					pre_statmt->setString(++index, conversions::to_utf8string(eimpl->table_entity.row_key()));

					pre_statmt->execute();
					return true;
				}
				catch (sql::SQLException& e)
				{
					eimpl->bolt_logger << BoltLog::LOG_ERROR << "SQLException in MysqlInsert "
						<< "(insertData) #ERR: " << e.what() << " (MySQL error code: "
						<< e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )";
				}
				return false;
			}


			bool MysqlEntity::patchEntity()
			{
				auto properties = eimpl->table_entity.properties();

				for (const auto& property : properties)
				{
					if (!MysqlTable::hasColumn(eimpl->table_name, property.first))
						MysqlTable::createColumn(eimpl->table_name, property.first, property.second.property_type());
				}


				std::string c_columns = "";

				for (auto iter = properties.cbegin(); iter != properties.cend(); ++iter)
				{
					c_columns += "`" + conversions::to_utf8string(iter->first) + "` = ?";
					if (iter != (--properties.cend()))
					{
						c_columns += ",";
					}
				}

				sql::SQLString qery("UPDATE " + conversions::to_utf8string(eimpl->table_name) + " SET " + c_columns
					+ " WHERE PartitionKey = ? AND RowKey = ?");

				try
				{
					MysqlConnection& connection = MysqlConnection::get_instance();
					std::unique_ptr<sql::PreparedStatement> pre_statmt(connection.getConnection()->prepareStatement(qery));

					int index = 1;
					for (auto iter = properties.cbegin(); iter != properties.cend(); ++iter, ++index)
					{
						switch (iter->second.property_type())
						{
						case myedm_type::boolean:
							pre_statmt->setBoolean(index, iter->second.boolean_value());
							break;
						case myedm_type::datetime:
							pre_statmt->setDateTime(index, conversions::to_utf8string(iter->second.datetime_value().to_string()));
							break;
						case myedm_type::double_floating_point:
							pre_statmt->setDouble(index, iter->second.double_value());
							break;
						case myedm_type::int32:
							pre_statmt->setInt(index, iter->second.int32_value());
							break;
						case myedm_type::int64:
							pre_statmt->setInt64(index, iter->second.int64_value());
							break;
						default:
							pre_statmt->setString(index, conversions::to_utf8string(iter->second.string_value()));
							break;
						}
					}

					pre_statmt->setString(index, conversions::to_utf8string(eimpl->table_entity.partition_key()));
					pre_statmt->setString(++index, conversions::to_utf8string(eimpl->table_entity.row_key()));

					pre_statmt->executeUpdate();
					return true;
				}
				catch (sql::SQLException& e)
				{
					eimpl->bolt_logger << BoltLog::LOG_ERROR << "SQLException in MysqlInsert "
						<< "(insertData) #ERR: " << e.what() << " (MySQL error code: "
						<< e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )";
				}
				return false;
			}

			bool MysqlEntity::entityExists(string_t row_key, string_t partition_key)
			{
				try
				{
					sql::SQLString query("SELECT COUNT(*) AS count FROM " + conversions::to_utf8string(eimpl->table_name) + " WHERE PartitionKey = ? AND RowKey = ?");
					MysqlConnection& connection = MysqlConnection::get_instance();
					std::unique_ptr<sql::PreparedStatement> stmt(connection.getConnection()->prepareStatement(query));

					stmt->setString(1, conversions::to_utf8string(partition_key));
					stmt->setString(2, conversions::to_utf8string(row_key));

					std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());

					res->rowsCount();
					if (res->next() && res->getInt(1) == 1)
					{
						return true;
					}
				}
				catch (sql::SQLException& e)
				{
					BoltLog logger;
					logger << BoltLog::LOG_ERROR << "SQLException in MysqlTable "
						<< "(getTableList) #ERR: " << e.what() << " (MySQL error code: "
						<< e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )";
				}
				return false;
			}

			bool MysqlEntity::abort()
			{
				bool res = false;

				try
				{
					MysqlConnection &connection = MysqlConnection::get_instance();
					sql::SQLString qery("DELETE FROM " + conversions::to_utf8string(eimpl->table_name) + " WHERE PartitionKey = ? AND RowKey = ?");
					std::unique_ptr<sql::PreparedStatement> stmt(connection.getConnection()->prepareStatement(qery));

					stmt->setString(1, conversions::to_utf8string(eimpl->table_entity.partition_key()));
					stmt->setString(1, conversions::to_utf8string(eimpl->table_entity.row_key()));

					res = stmt->execute();
					return true;
				}
				catch (sql::SQLException &e)
				{
					eimpl->bolt_logger << BoltLog::LOG_ERROR << "SQLException in MysqlDelete "
						<< "(executeDelete) #ERR: " << e.what() << " (MySQL error code: "
						<< e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )";
				}
				return res;
			}

			bool MysqlEntity::replaceEntity()
			{
				//TODO: Implement function body
				return false;
			}
		}
	}
}