#include <mysql_delete.h>
#include <mysql_connection.h>

namespace bolt {
	namespace storage {
		namespace mysql {

			class MysqlDelete::MDImpl
			{
			public:
				utility::string_t tblname;
				utility::string_t iTableName;
				std::vector<utility::string_t> sfields;

				std::map<utility::string_t, utility::string_t> m_query;

				BoltLog bolt_logger;

				utility::string_t buildQuery()
				{
					utility::string_t sql = U("DELETE ");

					if (m_query.find(U("from")) != m_query.cend())
						sql += U("\nFROM ") + m_query[U("from")];
					else
					{
						ucout << U("Query must have a  from portion");
						//Throw and exception
					}

					if (m_query.find(U("where")) != m_query.cend())
						sql += U("\nWHERE ") + m_query[U("where")];

					return sql;
				}
			};

			MysqlDelete::MysqlDelete() : dimpl{ new MDImpl }
			{
			}

			MysqlDelete::~MysqlDelete() //!!
			{
			}

			/// <summary>
			/// Inserts the data.
			/// </summary>
			/// <returns></returns>
			bool MysqlDelete::executeDelete()
			{
				bool res = false;

				try
				{
					MysqlConnection &connection = MysqlConnection::get_instance();
					std::unique_ptr<sql::Statement> stmt(connection.getConnection()->createStatement());

					res = stmt->execute(utility::conversions::to_utf8string(dimpl->buildQuery()));
					return true;
				}
				catch (sql::SQLException &e)
				{
					dimpl->bolt_logger << BoltLog::LOG_ERROR << "SQLException in MysqlDelete "
						<< "(executeDelete) #ERR: " << e.what() << " (MySQL error code: "
						<< e.getErrorCode() << ", SQLState: " << e.getSQLState() << " )";
				}
				return res;
			}

			MysqlDelete& MysqlDelete::from(utility::string_t tables)
			{
				dimpl->m_query[U("from")] = tables;
				return *this;
			}

			MysqlDelete& MysqlDelete::where(utility::string_t conditions)
			{
				dimpl->m_query[U("where")] = conditions;
				return *this;
			}

			MysqlDelete& MysqlDelete::andWhere(utility::string_t conditions)
			{
				auto iter = dimpl->m_query.find(U("where"));

				if (iter != dimpl->m_query.end())
				{
					iter->second += U(" AND ") + conditions;
				}
				else
				{
					dimpl->m_query[U("where")] = conditions;
				}

				return *this;
			}

			MysqlDelete& MysqlDelete::orWhere(utility::string_t conditions)
			{
				auto iter = dimpl->m_query.find(U("where"));

				if (iter != dimpl->m_query.end())
				{
					iter->second += U(" OR ") + conditions;
				}
				else
				{
					dimpl->m_query[U("where")] = conditions;
				}

				return *this;
			}
		}
	}
}