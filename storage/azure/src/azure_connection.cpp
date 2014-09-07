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
			void AzureConnection::Connect()
			{
				utility::char_t *connection_string = L"UseDevelopmentStorage=true;";
				strAccount = cloud_storage_account::parse(connection_string);
			}

			/// <summary>
			///Function for connect to Azure database using Account Settings
			/// <summary>
			/// Connects the specified account name.
			/// </summary>
			/// <param name="Accountname">The accountname.</param>
			/// <param name="Accountkey">The accountkey.</param>
			/// <returns></returns>
			int AzureConnection::Connect(utility::string_t Accountname, utility::string_t Accountkey)
			{
				utility::string_t ConnectionString = U("AccountName=") + Accountname + U(";AccountKey=") + Accountkey;
				strAccount = cloud_storage_account::parse(ConnectionString);
				if (strAccount.is_initialized()){
					return 1;
				}
				else{
					return 0;
				}
			}

			/// <summary>
			///Function for passing connection
			/// <summary>
			/// Gets the connection.
			/// </summary>
			/// <returns></returns>
			cloud_storage_account AzureConnection::getConn()
			{
				if (!strAccount.is_initialized())
					AzureConnection::Connect();
				return strAccount;
			}
		}
	}
}