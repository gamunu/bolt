#pragma once

#include <was/storage_account.h>
#include <was/table.h>

namespace bolt {
	namespace storage {
		namespace boltazure {
			using namespace azure::storage;

			class AzureConnection
			{
			public:
				AzureConnection() {}
				~AzureConnection() {}
				cloud_storage_account connect(utility::string_t account_name, utility::string_t account_key);
				cloud_storage_account connect();

			private:
				cloud_storage_account storage_account;
				void set_connect();
			};
		}
	}
}