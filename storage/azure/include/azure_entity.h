#pragma once

#ifdef BOLTAZURE_DLL
#define BOLTAZURE_API __declspec( dllexport )
#else
#define BOLTAZURE_API __declspec( dllimport )
#endif

#include "was/table.h"
#include <logger.hpp>

namespace bolt  {
	namespace storage {
		namespace boltazure {
			using namespace azure::storage;

			class AzureEntity
			{
			private:
				class AEImpl;
				std::shared_ptr<AEImpl> aimpl;
			public:
				BOLTAZURE_API AzureEntity(cloud_table table);
				BOLTAZURE_API ~AzureEntity();

				BOLTAZURE_API AzureEntity(cloud_table table, utility::string_t partition_key, utility::string_t row_key, size_t size);

				/// <summary>
				/// Insert 32bit Int value to a entity.
				/// Inserts the specified property name.
				/// </summary>
				/// <param name="PropertyName">Name of the property.</param>
				/// <param name="value">The value.</param>
				/// <returns></returns>
				BOLTAZURE_API void insert(utility::string_t property_name, int32_t value);

				/// <summary>
				/// Insert 64bit Int value to a entity	
				/// Inserts the specified property name.
				/// </summary>
				/// <param name="PropertyName">Name of the property.</param>
				/// <param name="value">The value.</param>
				/// <returns></returns>
				BOLTAZURE_API void insert(utility::string_t property_name, int64_t value);

				/// <summary>
				/// Insert bool value to a entity	
				/// Inserts the specified property name.
				/// </summary>
				/// <param name="PropertyName">Name of the property.</param>
				/// <param name="value">if set to <c>true</c> [value].</param>
				/// <returns></returns>
				BOLTAZURE_API void insert(utility::string_t property_name, utility::string_t value);

				/// <summary>
				/// Insert datetime value to a entity	
				/// Inserts the specified property name.
				/// </summary>
				/// <param name="PropertyName">Name of the property.</param>
				/// <param name="value">The value.</param>
				/// <returns></returns>
				BOLTAZURE_API void insert(utility::string_t property_name, double value);

				/// <summary>
				/// Insert GUID value to a entity	
				/// Inserts the specified property name.
				/// </summary>
				/// <param name="PropertyName">Name of the property.</param>
				/// <param name="value">The value.</param>
				/// <returns></returns>
				BOLTAZURE_API void insert(utility::string_t property_name, bool value);

				/// <summary>
				/// Insert String Value to  a entity
				/// Inserts the specified property name.
				/// </summary>
				/// <param name="PropertyName">Name of the property.</param>
				/// <param name="value">The value.</param>
				/// <returns></returns>
				BOLTAZURE_API void insert(utility::string_t property_name, utility::datetime value);

				/// <summary>
				/// Insert double Value to  a entity
				/// Inserts the specified property name.
				/// </summary>
				/// <param name="PropertyName">Name of the property.</param>
				/// <param name="value">The value.</param>
				/// <returns></returns>
				BOLTAZURE_API void insert(utility::string_t property_name, utility::uuid value);

				/// <summary>
				/// Create a Entity in the table.Have to pass PartitonKey,RowKey and entity size as params
				/// Creates the entity.
				/// </summary>
				/// <param name="PartitonKey">The partiton key.</param>
				/// <param name="RowKey">The row key.</param>
				/// <param name="size">The size.</param>
				/// <returns></returns>
				BOLTAZURE_API int createEntity(utility::string_t  partition_key, utility::string_t row_key, size_t size);

				/// <summary>
				/// Add Entity to a Table
				/// Executes the entity.
				/// </summary>
				/// <returns></returns>
				BOLTAZURE_API bool executeEntity();
				BOLTAZURE_API bool replaceEntity();
				BOLTAZURE_API bool patchEntity();
				BOLTAZURE_API bool abort();

			};
		}
	}
}