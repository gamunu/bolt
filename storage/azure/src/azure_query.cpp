#include <azure_query.h>
#include <azure_connection.h>

namespace bolt {
	namespace storage {
		namespace boltazure {

			struct FilterCondition
			{
				utility::string_t property_name;
				utility::string_t value;
				utility::string_t condition;
			};

			class AzureQuery::AQImpl
			{
			public:
				AQImpl()
				{
					m_table_client = m_connection.getConn().create_cloud_table_client();
				}

				AzureConnection m_connection;
				cloud_table_client m_table_client;
				cloud_table m_table;
				table_query m_table_query;

				std::map<utility::string_t, FilterCondition> m_query;
				std::vector<utility::string_t> m_select_coloumns;

				BoltLog bolt_logger;

				table_query buildQuery()
				{
					table_query t_query;

					auto filter = m_query.find(U("filter"));
					auto andjoin = m_query.find(U("andjoin"));
					auto orjoin = m_query.find(U("orjoin"));

					utility::string_t q_filter;
					utility::string_t q_join;

					if (!m_select_coloumns.empty())
					{
						t_query.set_select_columns(m_select_coloumns);
					}

					if (filter != m_query.cend())
					{
						q_filter = table_query::generate_filter_condition(filter->second.property_name, filter->second.condition, filter->second.value);
						t_query.set_filter_string(q_filter);

						if (andjoin != m_query.cend())
						{
							q_join = table_query::generate_filter_condition(andjoin->second.property_name, andjoin->second.condition, andjoin->second.value);
							t_query.set_filter_string(table_query::combine_filter_conditions(q_filter, query_logical_operator::and, q_join));
						}
						else if (orjoin != m_query.cend())
						{
							q_join = table_query::generate_filter_condition(orjoin->second.property_name, orjoin->second.condition, orjoin->second.value);
							t_query.set_filter_string(table_query::combine_filter_conditions(q_filter, query_logical_operator::or, q_join));
						}
					}
					return t_query;
				}
			};


			AzureQuery::AzureQuery(utility::string_t tablename) : qimpl{ new AQImpl }
			{
				qimpl->m_table = qimpl->m_table_client.get_table_reference(tablename);
			}

			std::vector<table_entity> AzureQuery::filterByKey(utility::string_t partitionkey, utility::string_t rowkey)
			{
				table_query query;
				query.set_filter_string(table_query::combine_filter_conditions(
					table_query::generate_filter_condition(U("PartitionKey"), query_comparison_operator::equal, partitionkey),
					query_logical_operator::and,
					table_query::generate_filter_condition(U("RowKey"), query_comparison_operator::equal, rowkey)));
				std::vector<table_entity> entities = qimpl->m_table.execute_query(query);
				return entities;
			}

			void AzureQuery::select(std::vector<utility::string_t> const columns)
			{
				if (!columns.empty())
				{
					qimpl->m_select_coloumns = columns;
				}
			}

			void AzureQuery::setFilterCondition(utility::string_t propertyname, utility::string_t coperator, utility::string_t value)
			{
				FilterCondition condition;
				condition.property_name = propertyname;
				condition.condition = coperator;
				condition.value = value;
				qimpl->m_query[U("filter")] = condition;
			}

			void AzureQuery::setAndFilterCondition(utility::string_t const propertyname, utility::string_t const coperator, utility::string_t const value)
			{
				FilterCondition condition;
				condition.property_name = propertyname;
				condition.condition = coperator;
				condition.value = value;
				qimpl->m_query[U("andjoin")] = condition;
			}

			void AzureQuery::setOrFilterCondition(utility::string_t const propertyname, utility::string_t const coperator, utility::string_t const value)
			{
				FilterCondition condition;
				condition.property_name = propertyname;
				condition.condition = coperator;
				condition.value = value;
				qimpl->m_query[U("orjoin")] = condition;
			}

			std::vector<table_entity> AzureQuery::filterByfield(utility::string_t field, utility::string_t val){
				table_query query;
				query.set_filter_string(
					table_query::generate_filter_condition(field, query_comparison_operator::equal, val));
				std::vector<table_entity> entities = qimpl->m_table.execute_query(query);
				return entities;
			}

			std::vector<table_entity> AzureQuery::AddFilter(utility::string_t field1, utility::string_t value1, int condition1, utility::string_t field2, utility::string_t value2, int condition2, int join)
			{
				table_query query;
				utility::string_t u_condition1 = GetFilterCondition(condition1);
				utility::string_t u_condition2 = GetFilterCondition(condition2);
				utility::string_t joinCondition = GetCondition(join);

				query.set_filter_string(table_query::combine_filter_conditions(
					table_query::generate_filter_condition(field1, u_condition1, value1),
					joinCondition,
					table_query::generate_filter_condition(field2, u_condition2, value2)));
				std::vector<table_entity> entities = qimpl->m_table.execute_query(query);
				return entities;
			}

			utility::string_t AzureQuery::GetCondition(int type)
			{
				switch (type)
				{
				case 2:
					return query_logical_operator::or;
				case 3:
					return query_logical_operator::not;
				default:
					return query_logical_operator::and;
				}
			}

			utility::string_t AzureQuery::GetFilterCondition(int type)
			{
				switch (type)
				{
				case 2:
					return query_comparison_operator::not_equal;
				case 3:
					return query_comparison_operator::greater_than;
				case 4:
					return query_comparison_operator::greater_than_or_equal;
				case 5:
					return query_comparison_operator::less_than;
				case 6:
					return query_comparison_operator::less_than_or_equal;
				default:
					return query_comparison_operator::equal;
				}
			}

			bool AzureQuery::deleteEntity(table_result retrieve_result)
			{
				table_operation operationdel = table_operation::delete_entity(retrieve_result.entity());
				try{
					qimpl->m_table.execute(operationdel);
					return true;
				}
				catch (const storage_exception& e)
				{
					qimpl->bolt_logger << BoltLog::LOG_ERROR << utility::conversions::to_utf8string(e.result().extended_error().message())
						<< e.result().http_status_code();
				}
				return false;
			}

			bool AzureQuery::deleteByKey(utility::string_t partitionkey, utility::string_t rowkey){
				table_operation operation2 = table_operation::retrieve_entity(partitionkey, rowkey);
				table_result retrieve_result = qimpl->m_table.execute(operation2);

				table_operation operation4 = table_operation::delete_entity(retrieve_result.entity());

				try {
					qimpl->m_table.execute(operation4);
					return true;
				}
				catch (const storage_exception& e)
				{
					qimpl->bolt_logger << BoltLog::LOG_ERROR << utility::conversions::to_utf8string(e.result().extended_error().message())
						<< e.result().http_status_code();
				}
				return false;
			}

			bool AzureQuery::deleteByField(utility::string_t fieldname, utility::string_t value){
				/*vector<wa::storage::table_entity> qry;
				qry = filterByfield(fieldname,value);
				wa::storage::table_entity ent;
				ent = qry.pop_back;
				wa::storage::table_operation deloperation = wa::storage::table_operation::delete_entity(ent);
				wa::storage::table_result delete_result = Createdtable.execute(deloperation);*/

				return false;
			}

			std::vector<table_entity> AzureQuery::queryAll()
			{
				return qimpl->m_table.execute_query(qimpl->buildQuery());
			}
		}
	}
}