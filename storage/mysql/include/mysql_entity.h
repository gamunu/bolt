#pragma once

#ifdef BOLTMYSQL_DLL
#define BOLTMYSQL_API __declspec( dllexport )
#else
#define BOLTMYSQL_API __declspec( dllimport )
#endif
#include <memory>
#include <cpprest/asyncrt_utils.h>

namespace bolt  {
	namespace storage {
		namespace mysql {

			class MysqlEntity
			{
			private:
				class MEImpl;
				std::shared_ptr<MEImpl> eimpl;
			public:
				BOLTMYSQL_API MysqlEntity(utility::string_t table_name);

				/// <summary>
				/// Create a Entity in the table.Have to pass PartitonKey,RowKey and entity size as params
				/// Creates the entity.
				/// </summary>
				/// <param name="PartitonKey">The partiton key.</param>
				/// <param name="RowKey">The row key.</param>
				/// <param name="size">The size.</param>
				/// <returns></returns>
				BOLTMYSQL_API MysqlEntity(utility::string_t table_name, utility::string_t  Partitonkey, utility::string_t Rowkey, size_t size);
		

				BOLTMYSQL_API ~MysqlEntity();
				
				/// <summary>
				/// Insert 32bit Int value to a entity.
				/// Inserts the specified property name.
				/// </summary>
				/// <param name="PropertyName">Name of the property.</param>
				/// <param name="value">The value.</param>
				/// <returns></returns>
				BOLTMYSQL_API void insertInt32(utility::string_t propertyName, int32_t value);

				/// <summary>
				/// Insert 64bit Int value to a entity	
				/// Inserts the specified property name.
				/// </summary>
				/// <param name="PropertyName">Name of the property.</param>
				/// <param name="value">The value.</param>
				/// <returns></returns>
				BOLTMYSQL_API void insertInt64(utility::string_t propertyName, int64_t value);

				/// <summary>
				/// Insert bool value to a entity	
				/// Inserts the specified property name.
				/// </summary>
				/// <param name="PropertyName">Name of the property.</param>
				/// <param name="value">if set to <c>true</c> [value].</param>
				/// <returns></returns>
				BOLTMYSQL_API void insertString(utility::string_t propertyName, utility::string_t value);

				/// <summary>
				/// Insert datetime value to a entity	
				/// Inserts the specified property name.
				/// </summary>
				/// <param name="PropertyName">Name of the property.</param>
				/// <param name="value">The value.</param>
				/// <returns></returns>
				BOLTMYSQL_API void insertDouble(utility::string_t propertyName, double value);

				/// <summary>
				/// Insert GUID value to a entity	
				/// Inserts the specified property name.
				/// </summary>
				/// <param name="PropertyName">Name of the property.</param>
				/// <param name="value">The value.</param>
				/// <returns></returns>
				BOLTMYSQL_API void insertBoolean(utility::string_t propertyName, bool value);

				/// <summary>
				/// Insert String Value to  a entity
				/// Inserts the specified property name.
				/// </summary>
				/// <param name="PropertyName">Name of the property.</param>
				/// <param name="value">The value.</param>
				/// <returns></returns>
				BOLTMYSQL_API void insertTimestamp(utility::string_t propertyName, utility::datetime value);
				
				/// <summary>
				/// Add Entity to a Table
				/// Executes the entity.
				/// </summary>
				/// <returns></returns>
				BOLTMYSQL_API bool ExecuteEntity();
				BOLTMYSQL_API bool ReplaceEntity();
				BOLTMYSQL_API bool PatchEntity();
				BOLTMYSQL_API bool entityExists(utility::string_t row_key, utility::string_t partition_key);

			};
		}
	}
}