#pragma once

#ifdef BOLTAZURE_DLL
#define BOLTAZURE_API __declspec( dllexport )
#else
#define BOLTAZURE_API __declspec( dllimport )
#endif

#include <map>
#include <was/table.h>
#include <logger.hpp>

namespace bolt {
	namespace storage {
		namespace boltazure {
			using namespace azure::storage;

			class AzureQuery
			{
			private:

				class AQImpl;
				std::shared_ptr<AQImpl> qimpl;
			public:
				/// <summary>
				/// Initializes a new instance of the <see cref="Query"/> class.
				/// </summary>
				/// <param name="tblName">Name of the table.</param>
				/// <param name="Account">The account.</param>
				/// Bolt.Storage.Azure Namespce codes
				BOLTAZURE_API AzureQuery(utility::string_t table_name);
				/// <summary>
				/// Add a Filter by a key
				/// Filters the by key.
				/// </summary>
				/// <param name="partitionkey">The partitionkey.</param>
				/// <param name="rowkey">The rowkey.</param>
				/// <returns>Query result vector</returns>
				BOLTAZURE_API std::vector<table_entity> filterByKey(utility::string_t partition_key, utility::string_t row_key);

				BOLTAZURE_API void select(const std::vector<utility::string_t> columns);
				BOLTAZURE_API void setFilterCondition(const utility::string_t property_name, const utility::string_t coperator, const utility::string_t value);
				BOLTAZURE_API void setAndFilterCondition(const utility::string_t property_name, const utility::string_t coperator, const utility::string_t value);
				BOLTAZURE_API void setOrFilterCondition(const utility::string_t property_name, const utility::string_t coperator, const utility::string_t value);

				/// <summary>
				/// Filter by Field
				/// Filters the byfield.
				/// </summary>
				/// <param name="field">The field.</param>
				/// <param name="val">The value.</param>
				/// <returns>Query result vector</returns>
				BOLTAZURE_API std::vector<table_entity> filterByProperty(utility::string_t property_name, utility::string_t value);

				/// <summary>
				/// Gets the condition for combine filter conditions
				/// </summary>
				/// <param name="type">The type.</param>
				/// <returns> Query logical Operator </returns>
				BOLTAZURE_API utility::string_t getCondition(int type);

				/// <summary>
				/// Gets the filter condition.
				/// </summary>
				/// <param name="type">The type.</param>
				/// <returns></returns>
				BOLTAZURE_API utility::string_t getFilterCondition(int type);

				/// <summary>
				/// Deletes the entity.
				/// </summary>
				/// <param name="retrieve_result">The retrieve_result.</param>
				/// <returns></returns>
				BOLTAZURE_API bool deleteEntity(table_result retrieve_result);
				BOLTAZURE_API bool deleteEntity(utility::string_t partition_key, utility::string_t row_key);
				BOLTAZURE_API bool executeDelete();

				BOLTAZURE_API std::vector<table_entity> queryAll();
			};
		}
	}
}