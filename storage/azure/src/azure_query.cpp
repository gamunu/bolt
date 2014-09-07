#include <azure_query.h>
#include <azure_connection.h>

namespace bolt {
	namespace storage {
		namespace boltazure {
			AzureQuery::AzureQuery(utility::string_t tblName)
			{
				AzureConnection connection;
				cloud_table_client table_client = connection.getConn().create_cloud_table_client();
				cloud_table table = table_client.get_table_reference(tblName);
				Createdtable = table;
			}

			std::vector<table_entity> AzureQuery::filterByKey(utility::string_t partitionkey, utility::string_t rowkey)
			{
				table_query query;
				query.set_filter_string(table_query::combine_filter_conditions(
					table_query::generate_filter_condition(U("PartitionKey"), query_comparison_operator::equal, partitionkey),
					query_logical_operator::and,
					table_query::generate_filter_condition(U("RowKey"), query_comparison_operator::equal, rowkey)));
				std::vector<table_entity> entities = Createdtable.execute_query(query);
				return entities;
			}

			std::vector<table_entity> AzureQuery::filterByfield(utility::string_t field, utility::string_t val){
				table_query query;
				query.set_filter_string(
					table_query::generate_filter_condition(field, query_comparison_operator::equal, val));
				std::vector<table_entity> entities = Createdtable.execute_query(query);
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
				std::vector<table_entity> entities = Createdtable.execute_query(query);
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
					table_result delete_result = Createdtable.execute(operationdel);
					return true;
				}
				catch (const storage_exception& e)
				{
					bolt_logger << BoltLog::LOG_ERROR << utility::conversions::to_utf8string(e.result().extended_error().message())
						<< e.result().http_status_code();
				}
				return false;
			}

			bool AzureQuery::deleteByKey(utility::string_t partitionkey, utility::string_t rowkey){
				table_operation operation2 = table_operation::retrieve_entity(partitionkey, rowkey);
				table_result retrieve_result = Createdtable.execute(operation2);

				table_operation operation4 = table_operation::delete_entity(retrieve_result.entity());

				try {
					table_result delete_result = Createdtable.execute(operation4);
					return true;
				}
				catch (const storage_exception& e)
				{
					bolt_logger << BoltLog::LOG_ERROR << utility::conversions::to_utf8string(e.result().extended_error().message())
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
		}
	}
}