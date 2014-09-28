#include <azure_connection.h>

namespace bolt  {
	namespace storage {
		namespace boltazure {
			/// <summary>
			///Function for connect to Azure database using Development settings
			/// <summary>
			/// Connects this instance.
			/// </summary>
			/// <returns></returns>
			void AzureConnection::set_connect()
			{

			}

			/// <summary>
			///Function for connect to Azure database using Account Settings
			/// <summary>
			/// Connects the specified account name.
			/// </summary>
			/// <param name="Accountname">The accountname.</param>
			/// <param name="Accountkey">The accountkey.</param>
			/// <returns></returns>
			cloud_storage_account AzureConnection::connect(utility::string_t account_name, utility::string_t account_key)
			{
				utility::string_t ConnectionString = U("AccountName=") + account_name + U(";AccountKey=") + account_key;
				return cloud_storage_account::parse(ConnectionString);
			}

			cloud_storage_account AzureConnection::connect()
			{
				utility::string_t connection_string = U("UseDevelopmentStorage=true;");
				return cloud_storage_account::parse(connection_string);
			}
		}
	}
}