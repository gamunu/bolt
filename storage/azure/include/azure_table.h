#pragma once

#ifdef BOLTAZURE_DLL
#define BOLTAZURE_API __declspec( dllexport )
#else
#define BOLTAZURE_API __declspec( dllimport )
#endif

#include "was/table.h"

namespace bolt {
	namespace storage {
		namespace boltazure {

			using namespace azure::storage;

			class AzureTable
			{
			private:
				cloud_table Createdtable;
			public:

				/// <summary>
				/// Constructor of the Table class.Have to pass TableName and ConnectionAccount as params to create a table  
				/// <summary>
				/// Initializes a new instance of the Table class.
				/// </summary>
				/// <param name="tblName">Name of the table</param>
				/// <param name="Account">The account</param>
				BOLTAZURE_API AzureTable(utility::string_t tblName);
				BOLTAZURE_API AzureTable() { }
				BOLTAZURE_API cloud_table getTable();
				BOLTAZURE_API std::vector<utility::string_t> getTableList();
				BOLTAZURE_API static bool createTable(utility::string_t table_name);
				BOLTAZURE_API static bool deleteTable(utility::string_t table_name);
			};
		}
	}
}