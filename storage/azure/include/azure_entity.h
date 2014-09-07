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

				BOLTAZURE_API AzureEntity(cloud_table table, utility::string_t Partitonkey, utility::string_t Rowkey, size_t size);

				/// <summary>
				/// Create a Entity in the table.Have to pass PartitonKey,RowKey and entity size as params
				/// Creates the entity.
				/// </summary>
				/// <param name="PartitonKey">The partiton key.</param>
				/// <param name="RowKey">The row key.</param>
				/// <param name="size">The size.</param>
				/// <returns></returns>
				BOLTAZURE_API int CreateEntity(utility::string_t  Partitonkey, utility::string_t Rowkey, size_t size);
				
				/// <summary>
				/// Insert 32bit Int value to a entity.
				/// Inserts the specified property name.
				/// </summary>
				/// <param name="PropertyName">Name of the property.</param>
				/// <param name="value">The value.</param>
				/// <returns></returns>
				BOLTAZURE_API void Insert(utility::string_t propertyName, int32_t value);

				/// <summary>
				/// Insert 64bit Int value to a entity	
				/// Inserts the specified property name.
				/// </summary>
				/// <param name="PropertyName">Name of the property.</param>
				/// <param name="value">The value.</param>
				/// <returns></returns>
				BOLTAZURE_API void Insert(utility::string_t propertyName, int64_t value);

				/// <summary>
				/// Insert bool value to a entity	
				/// Inserts the specified property name.
				/// </summary>
				/// <param name="PropertyName">Name of the property.</param>
				/// <param name="value">if set to <c>true</c> [value].</param>
				/// <returns></returns>
				BOLTAZURE_API void Insert(utility::string_t propertyName, utility::string_t value);

				/// <summary>
				/// Insert datetime value to a entity	
				/// Inserts the specified property name.
				/// </summary>
				/// <param name="PropertyName">Name of the property.</param>
				/// <param name="value">The value.</param>
				/// <returns></returns>
				BOLTAZURE_API void Insert(utility::string_t propertyName, double value);

				/// <summary>
				/// Insert GUID value to a entity	
				/// Inserts the specified property name.
				/// </summary>
				/// <param name="PropertyName">Name of the property.</param>
				/// <param name="value">The value.</param>
				/// <returns></returns>
				BOLTAZURE_API void Insert(utility::string_t propertyName, bool value);

				/// <summary>
				/// Insert String Value to  a entity
				/// Inserts the specified property name.
				/// </summary>
				/// <param name="PropertyName">Name of the property.</param>
				/// <param name="value">The value.</param>
				/// <returns></returns>
				BOLTAZURE_API void Insert(utility::string_t propertyName, utility::datetime value);

				/// <summary>
				/// Insert double Value to  a entity
				/// Inserts the specified property name.
				/// </summary>
				/// <param name="PropertyName">Name of the property.</param>
				/// <param name="value">The value.</param>
				/// <returns></returns>
				BOLTAZURE_API void Insert(utility::string_t propertyName, utility::uuid value);

				/// <summary>
				/// Add Entity to a Table
				/// Executes the entity.
				/// </summary>
				/// <returns></returns>
				BOLTAZURE_API bool ExecuteEntity();
				BOLTAZURE_API bool ReplaceEntity();
				BOLTAZURE_API bool PatchEntity();

			};
		}
	}
}