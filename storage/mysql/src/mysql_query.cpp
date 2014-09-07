#include <table.h>
#include <mysql_query.h>
#include <mysql_connection.h>
#include <mysql_result.h>

namespace bolt {
	namespace storage {
		namespace mysql {

			class MysqlQuery::MQImpl
			{
			public:
				utility::string_t tblname;
				utility::string_t iTableName;
				std::vector<utility::string_t> sfields;

				std::map<utility::string_t, utility::string_t> m_query;

				BoltLog bolt_logger;

				std::deque<mysql_table_entity> innerQuery(std::unique_ptr<sql::ResultSet> &res)
				{

					std::deque<mysql_table_entity> entites;
					try
					{
						sql::ResultSetMetaData *res_meta = res->getMetaData();

						int column_count = res_meta->getColumnCount();

						//Declare variabels outside loops to improve perfomance
						//Partition key of an entity
						utility::string_t parition_key;
						//Row key of an entity
						utility::string_t row_key;
						//a propery of an entity
						mysql_property property;
						//MySql table column column name
						utility::string_t column_name;
						//A MySql entity
						mysql_table_entity table_entity;

						while (res->next())
						{

							for (int i = 1; i <= column_count; i++)
							{
								column_name = utility::conversions::to_string_t(res_meta->getColumnName(i));

								if (column_name == U("PartitionKey"))
								{
									parition_key = utility::conversions::to_string_t(res->getString(i));
									continue;
								}
								if (column_name == U("RowKey"))
								{
									row_key = utility::conversions::to_string_t(res->getString(i));
									continue;
								}

								switch (res_meta->getColumnType(i))
								{
								case sql::DataType::BIT: //fall thorugh switch
								case sql::DataType::TINYINT:
								case sql::DataType::SMALLINT:
								case sql::DataType::MEDIUMINT:
								case sql::DataType::INTEGER:
									property.set_value(res->getInt(i));
									break;
								case sql::DataType::BIGINT:
									property.set_value(res->getInt64(i));
									break;
								case sql::DataType::DOUBLE: //fall thorugh switch
								case sql::DataType::REAL:
								case sql::DataType::DECIMAL:
								case sql::DataType::NUMERIC:
									property.set_value(res->getDouble(i));
									break;
								default:
									property.set_value(utility::conversions::to_string_t(res->getString(i)));
									break;
								}
								table_entity.add_property(column_name, property);
							}
							//To improve performance try not to declare new objects
							table_entity.set_partition_key(parition_key);
							table_entity.set_row_key(row_key);
							//add entity to vector
							entites.push_back(table_entity);
							//Clear properties of the entity
							table_entity.clear_properties();
						}
					}
					catch (sql::SQLException &e)
					{
						bolt_logger << BoltLog::LOG_ERROR << "SQLException in MysqlQuery (queryAll) #ERR: "
							<< e.what() << " (MySQL error code: "
							<< std::to_string(e.getErrorCode()) << ", SQLState: " << e.getSQLState() << " )";
					}

					return entites;
				}


				utility::string_t buildQuery()
				{
					utility::string_t sql;
					sql = (m_query.find(U("distinct")) != m_query.cend()) ? U("SELECT DISTINCT ") : U("SELECT ");
					sql += (m_query.find(U("select")) != m_query.cend()) ? m_query[U("select")] : U("*");

					if (m_query.find(U("from")) != m_query.cend())
						sql += U("\nFROM ") + m_query[U("from")];
					else
					{
						ucout << U("Query must have a  from portion");
						//Throw and exception
					}

					if (m_query.find(U("join")) != m_query.cend())
						sql += U("\n") + m_query[U("join")];

					if (m_query.find(U("where")) != m_query.cend())
						sql += U("\nWHERE ") + m_query[U("where")];

					if (m_query.find(U("group")) != m_query.cend())
						sql += U("\nGROUP BY ") + m_query[U("group")];

					if (m_query.find(U("having")) != m_query.cend())
						sql += U("\nHAVING ") + m_query[U("having")];

					if (m_query.find(U("union")) != m_query.cend())
						sql += U("\nUNION (") + m_query[U("union")] + U(")");

					if (m_query.find(U("order")) != m_query.cend())
						sql += U("\nORDER BY ") + m_query[U("order")];



					int limit = -1;
					int offset = -1;
					if (m_query.find(U("limit")) != m_query.cend())
					{
						utility::istringstream_t buffer(m_query[U("limit")]);
						buffer >> limit;

						if (buffer.fail() || !buffer.eof())
						{
							throw std::runtime_error(error_parse_int32);
						}
					}

					if (m_query.find(U("offset")) != m_query.cend())
					{
						utility::istringstream_t buffer(m_query[U("offset")]);
						buffer >> offset;

						if (buffer.fail() || !buffer.eof())
						{
							throw std::runtime_error(error_parse_int32);
						}
					}

					if (limit > 0 || offset > 0)
						sql = this->applyLimit(sql, limit, offset);

					return sql;
				}

				utility::string_t applyLimit(utility::string_t sql, int limit, int offset)
				{
					if (limit > 0)
						sql += U(" LIMIT ") + std::to_wstring(limit);
					if (offset > 0)
						sql += U(" OFFSET ") + std::to_wstring(offset);

					return sql;
				}
			};

			MysqlQuery::MysqlQuery() : qimpl{ new MQImpl }
			{

			}

			MysqlQuery::~MysqlQuery() //!!
			{
			}

			MysqlQuery::MysqlQuery(utility::string_t table_name) : qimpl{ new MQImpl }
			{
				qimpl->iTableName = table_name;
			}
			
			MysqlQuery& MysqlQuery::select(utility::string_t columns)
			{
				qimpl->m_query[U("select")] = columns;
				return *this;
			}

			MysqlQuery& MysqlQuery::selectDistinct(utility::string_t columns)
			{
				qimpl->m_query[U("select")] = columns;
				qimpl->m_query[U("distinct")] = U("true");
				return *this;
			}

			MysqlQuery& MysqlQuery::from(utility::string_t tables)
			{
				qimpl->m_query[U("from")] = tables;
				return *this;
			}

			MysqlQuery& MysqlQuery::where(utility::string_t conditions)
			{
				qimpl->m_query[U("where")] = conditions;
				return *this;
			}

			MysqlQuery& MysqlQuery::andWhere(utility::string_t conditions)
			{
				auto iter = qimpl->m_query.find(U("where"));

				if (iter != qimpl->m_query.end())
				{
					iter->second += U(" AND ") + conditions;
				}
				else
				{
					qimpl->m_query[U("where")] = conditions;
				}

				return *this;
			}

			MysqlQuery& MysqlQuery::orWhere(utility::string_t conditions)
			{
				auto iter = qimpl->m_query.find(U("where"));

				if (iter != qimpl->m_query.end())
				{
					iter->second += U(" OR ") + conditions;
				}
				else
				{
					qimpl->m_query[U("where")] = conditions;
				}

				return *this;
			}

			MysqlQuery& MysqlQuery::join(utility::string_t table, utility::string_t conditions)
			{
				auto iter = qimpl->m_query.find(U("join"));

				if (iter != qimpl->m_query.end())
				{
					iter->second += U(" JOIN ") + table + U(" ON ") + conditions;
				}
				else
				{
					qimpl->m_query[U("join")] = U(" JOIN ") + table + U(" ON ") + conditions;
				}

				return *this;
			}

			MysqlQuery& MysqlQuery::leftJoin(utility::string_t table, utility::string_t conditions)
			{
				auto iter = qimpl->m_query.find(U("join"));

				if (iter != qimpl->m_query.end())
				{
					iter->second += U(" LEFT JOIN ") + table + U(" ON ") + conditions;
				}
				else
				{
					qimpl->m_query[U("join")] = U(" LEFT JOIN ") + table + U(" ON ") + conditions;
				}

				return *this;
			}

			MysqlQuery& MysqlQuery::rightJoin(utility::string_t table, utility::string_t conditions)
			{
				auto iter = qimpl->m_query.find(U("join"));

				if (iter != qimpl->m_query.end())
				{
					iter->second += U(" RIGHT JOIN ") + table + U(" ON ") + conditions;
				}
				else
				{
					qimpl->m_query[U("join")] = U(" RIGHT JOIN ") + table + U(" ON ") + conditions;
				}
				return *this;
			}

			MysqlQuery& MysqlQuery::crossJoin(utility::string_t table)
			{
				auto iter = qimpl->m_query.find(U("join"));

				if (iter != qimpl->m_query.end())
				{
					iter->second += U(" CROSS JOIN ") + table;
				}
				else
				{
					qimpl->m_query[U("join")] = U(" CROSS JOIN ") + table;
				}
				return *this;
			}

			MysqlQuery& MysqlQuery::naturalJoin(utility::string_t table)
			{
				auto iter = qimpl->m_query.find(U("join"));

				if (iter != qimpl->m_query.end())
				{
					iter->second += U(" NATURAL JOIN ") + table;
				}
				else
				{
					qimpl->m_query[U("join")] = U(" NATURAL JOIN ") + table;
				}
				return *this;
			}

			MysqlQuery& MysqlQuery::group(utility::string_t columns)
			{
				qimpl->m_query[U("group")] = columns;
				return *this;
			}

			MysqlQuery& MysqlQuery::having(utility::string_t conditions)
			{
				qimpl->m_query[U("having")] = conditions;
				return *this;
			}

			MysqlQuery& MysqlQuery::order(utility::string_t columns)
			{
				qimpl->m_query[U("order")] = columns;
				return *this;
			}

			MysqlQuery& MysqlQuery::limit(utility::string_t limit, utility::string_t offset)
			{
				qimpl->m_query[U("limit")] = limit;
				if (offset != U(""))
					qimpl->m_query[U("offset")] = offset;
				return *this;
			}

			MysqlQuery& MysqlQuery::offset(utility::string_t offset)
			{
				qimpl->m_query[U("offset")] = offset;
				return *this;
			}

			MysqlQuery& MysqlQuery::munion(utility::string_t sql)
			{
				qimpl->m_query[U("union")] = sql;
				return *this;
			}

			std::deque<mysql_table_entity> MysqlQuery::queryAll()
			{
				MysqlConnection &connection = MysqlConnection::get_instance();
				utility::string_t query = qimpl->buildQuery();
				std::unique_ptr<sql::Statement> stmt(connection.getConnection()->createStatement());
				std::unique_ptr<sql::ResultSet> res(stmt->executeQuery(utility::conversions::to_utf8string(query)));

				return qimpl->innerQuery(res);
			}


			std::deque<mysql_table_entity> MysqlQuery::filterByKey(utility::string_t partitionkey, utility::string_t rowkey)
			{
				utility::string_t qery = U("SELECT * FROM ") + qimpl->iTableName + U(" WHERE PartitionKey=? AND RowKey=?");

				MysqlConnection &connection = MysqlConnection::get_instance();
				std::unique_ptr<sql::PreparedStatement> stmt(connection.getConnection()->prepareStatement(utility::conversions::to_utf8string(qery)));

				stmt->setString(1, utility::conversions::to_utf8string(partitionkey));
				stmt->setString(2, utility::conversions::to_utf8string(rowkey));

				std::unique_ptr<sql::ResultSet> res(stmt->executeQuery());


				return qimpl->innerQuery(res);
			}



			/// <summary>
			/// Selects the table.
			/// </summary>
			/// <param name="tblName">Name of the table.</param>
			/// <returns></returns>
			void MysqlQuery::selectTable(utility::string_t tblName)
			{
				qimpl->iTableName = tblName;
			}


			/* New Api Functions */


			/// <summary>
			/// Executes the query.
			/// </summary>
			/// <returns></returns>
			std::deque<mysql_table_entity> MysqlQuery::executeQuery()
			{
				utility::string_t qery = U("SELECT ");

				if (!qimpl->sfields.empty())
				{
					for (std::vector<utility::string_t>::size_type i = 0; i != qimpl->sfields.size(); i++) {
						qery.append(qimpl->sfields[i]);
						qery.append(U(","));
					}

					qery = qery.substr(0, qery.size() - 1);
				}
				else {
					qery.append(U("* "));
				}

				qery.append(U(" FROM "));
				qery.append(qimpl->iTableName);
				/*
				if (!iconditions.empty())
				{
				qery.append(U(" WHERE "));
				qery.append(iconditions);
				}

				if (!orderfield.empty()){
				qery.append(U(" ORDER BY "));
				qery.append(orderfield);
				qery.append(U(" "));
				qery.append(order);
				}
				else if (!ihaving.empty()){
				qery.append(U(" HAVING "));
				qery.append(ihaving);
				}
				else if (!igrpfield.empty()){
				qery.append(U(" GROUP BY "));
				qery.append(igrpfield);
				}*/


				MysqlConnection &connection = MysqlConnection::get_instance();
				std::unique_ptr<sql::Statement> stmt(connection.getConnection()->createStatement());
				std::unique_ptr<sql::ResultSet> res(stmt->executeQuery(utility::conversions::to_utf8string(qery)));


				return qimpl->innerQuery(res);
			}


		}
	}
}