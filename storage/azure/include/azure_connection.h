#pragma once

#include <was/storage_account.h>
#include <was/table.h>

namespace bolt {
	namespace storage {
		namespace boltazure {
			using namespace azure::storage;

			class AzureConnection
			{
			private:
				cloud_storage_account strAccount;

			public:
				AzureConnection() {}
				~AzureConnection() {}
				void Connect();
				int Connect(utility::string_t Accountname, utility::string_t AccountKey);
				cloud_storage_account getConn();

			};
		}
	}
}