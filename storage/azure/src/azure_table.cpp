#include <azure_table.h>
#include <azure_connection.h>

namespace bolt {
	namespace storage {
		namespace boltazure {
			AzureTable::AzureTable(utility::string_t table_name)
			{
				AzureConnection connection;
				cloud_table_client table_client = connection.connect().create_cloud_table_client();
				cloud_table table = table_client.get_table_reference(table_name);
				table.create_if_not_exists();
				m_cloud_table = table;
			}

			cloud_table AzureTable::getTable()
			{
				return m_cloud_table;
			}

			std::vector<utility::string_t> AzureTable::getTableList()
			{
				AzureConnection connection;
				auto table_client = connection.connect().create_cloud_table_client();
				auto tables = table_client.list_tables();

				std::vector<utility::string_t> table_names;
				for (auto table : tables)
				{
					table_names.push_back(table.name());
				}
				return table_names;
			}

			bool AzureTable::createTable(utility::string_t table_name)
			{
				AzureConnection connection;
				cloud_table_client table_client = connection.connect().create_cloud_table_client();
				cloud_table table = table_client.get_table_reference(table_name);
				return table.create_if_not_exists();
			}

			bool AzureTable::deleteTable(utility::string_t table_name)
			{
				AzureConnection connection;
				cloud_table_client table_client = connection.connect().create_cloud_table_client();
				cloud_table table = table_client.get_table_reference(table_name);
				return table.delete_table_if_exists();
			}
		}
	}
}