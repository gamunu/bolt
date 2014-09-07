#include <mysql_insert.h>
#include <mysql_connection.h>

namespace bolt {
	namespace storage {
		namespace mysql {
			/// <summary>
			/// Selects the table.
			/// </summary>
			/// <param name="tblName">Name of the table.</param>
			/// <returns></returns>
			void MysqlInsert::selectTable(utility::string_t tblName)
			{
				iTableName = tblName;
			}

			/// <summary>
			/// Adds the data.
			/// </summary>
			/// <param name="field">The field.</param>
			/// <param name="val">The value.</param>
			/// <returns></returns>
			void MysqlInsert::addField(MysqlField field)
			{
				ifields.push_back(field);
			}

			void MysqlInsert::addInteger(utility::string_t field_name, int value)
			{
				MysqlField field(field_name, MysqlField::DataType::INTEGER);
				field.setData(value);
				addField(field);
			}

			void MysqlInsert::addDouble(utility::string_t field_name, long double value)
			{
				MysqlField field(field_name, MysqlField::DataType::DOUBLE);
				field.setData(value);
				addField(field);
			}

			void MysqlInsert::addBoolean(utility::string_t field_name, bool value)
			{
				MysqlField field(field_name, MysqlField::DataType::BOOLEAN);
				field.setData(value);
				addField(field);
			}

			void MysqlInsert::addString(utility::string_t field_name, utility::string_t value)
			{
				MysqlField field(field_name, MysqlField::DataType::STRING);
				field.setData(value);
				addField(field);
			}
			/// <summary>
			/// Inserts the data.
			/// </summary>
			/// <returns></returns>
			bool MysqlInsert::insertData()
			{
				std::string qery = "INSERT INTO ";
				qery.append(utility::conversions::to_utf8string(iTableName));
				qery.append("(");

				for (auto iter = ifields.begin(); iter != ifields.end(); ++iter) {
					qery.append(utility::conversions::to_utf8string(iter->name()));
					qery.append(",");
				}

				qery = qery.substr(0, qery.size() - 1);

				qery.append(")");
				qery.append(" VALUES");
				qery.append("(");

				for (auto iter = ifields.begin(); iter != ifields.end(); ++iter)
				{
					qery.append("?");
					qery.append(",");
				}

				qery = qery.substr(0, qery.size() - 1);

				qery.append(")");

				bool res = false;

				try
				{
					MysqlConnection &connection = MysqlConnection::get_instance();
					std::unique_ptr<sql::PreparedStatement> pre_statmt(connection.getConnection()->prepareStatement(qery));

					int index = 1;
					for (auto iter = ifields.begin(); iter != ifields.end(); ++iter, ++index)
					{
						switch (iter->dataType())
						{
						case MysqlField::INTEGER:
							pre_statmt->setInt(index, boost::get<int32_t>(iter->data()));
							break;
						case MysqlField::BIGINT:
							pre_statmt->setInt64(index, boost::get<int64_t>(iter->data()));
							break;
						case MysqlField::DOUBLE:
							pre_statmt->setDouble(index, boost::get<long double>(iter->data()));
							break;
						case MysqlField::BOOLEAN:
							pre_statmt->setBoolean(index, boost::get<bool>(iter->data()));
							break;
						case MysqlField::STRING:
							pre_statmt->setString(index, utility::conversions::to_utf8string(boost::get<utility::string_t>(iter->data())));
							break;
						default:
							pre_statmt->setString(index, utility::conversions::to_utf8string(boost::get<utility::string_t>(iter->data())));
							break;
						}
					}

					res = pre_statmt->execute();
				}
				catch (sql::SQLException &e)
				{
					bolt_logger << BoltLog::LOG_ERROR << "SQLException in MysqlInsert "
						<< "(insertData) #ERR: " << e.what() << " (MySQL error code: "
						<< e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )";
				}

				return res;
			}
		}
	}
}