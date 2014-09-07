#include <mysql_delete.h>
#include <mysql_connection.h>

namespace bolt {
	namespace storage {
		namespace mysql {
			/// <summary>
			/// Selects the table.
			/// </summary>
			/// <param name="tblName">Name of the table.</param>
			/// <returns></returns>
			void MysqlDelete::selectTable(utility::string_t tblName)
			{
				iTableName = tblName;
			}

			void MysqlDelete::AddCondition(utility::string_t condition)
			{
				icondition = condition;
			}

			/// <summary>
			/// Inserts the data.
			/// </summary>
			/// <returns></returns>
			bool MysqlDelete::executeDelete()
			{
				std::string qery = "DELETE FROM ";
				qery.append(utility::conversions::to_utf8string(iTableName));
				qery.append(" WHERE ");
				qery.append(utility::conversions::to_utf8string(icondition));

				bool res = false;

				try
				{
					MysqlConnection &connection = MysqlConnection::get_instance();
					std::unique_ptr<sql::Statement> stmt(connection.getConnection()->createStatement());

					res = stmt->execute(qery);
				}
				catch (sql::SQLException &e)
				{
					bolt_logger << BoltLog::LOG_ERROR << "SQLException in MysqlDelete "
						<< "(executeDelete) #ERR: " << e.what() << " (MySQL error code: "
						<< e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )";
				}
				return res;
			}
		}
	}
}