#include <azure_entity.h>

namespace bolt  {
	namespace storage {
		namespace boltazure {

			class AzureEntity::AEImpl
			{
			public:
				cloud_table table;
				table_entity CreatedEntity;
				BoltLog bolt_logger;
			};

			AzureEntity::AzureEntity(cloud_table Passingtable) : aimpl { new AEImpl }
			{
				aimpl->table = Passingtable;
			}

			AzureEntity::~AzureEntity() //!!
			{
			}

			AzureEntity::AzureEntity(cloud_table table, utility::string_t Partitonkey, utility::string_t Rowkey, size_t size) : aimpl { new AEImpl }
			{
				table_entity entity(Partitonkey, Rowkey);
				entity.properties().reserve(size);
				aimpl->CreatedEntity = entity;
			}

			int AzureEntity::CreateEntity(utility::string_t PartitonKey, utility::string_t RowKey, size_t size)
			{
				// Insert a table entity
				table_entity entity(PartitonKey, RowKey);
				entity.properties().reserve(size);
				aimpl->CreatedEntity = entity;
				return 1;
			}

			void AzureEntity::Insert(utility::string_t PropertyName, int32_t value)
			{
				aimpl->CreatedEntity.properties().insert(table_entity::property_type(PropertyName, entity_property(value)));
			}

			void AzureEntity::Insert(utility::string_t PropertyName, int64_t value){
				aimpl->CreatedEntity.properties().insert(table_entity::property_type(PropertyName, entity_property(value)));
			}

			void AzureEntity::Insert(utility::string_t PropertyName, bool value)
			{
				aimpl->CreatedEntity.properties().insert(table_entity::property_type(PropertyName, entity_property(value)));
			}

			void AzureEntity::Insert(utility::string_t  PropertyName, utility::datetime value)
			{
				aimpl->CreatedEntity.properties().insert(table_entity::property_type(PropertyName, entity_property(value)));
			}

			void AzureEntity::Insert(utility::string_t PropertyName, utility::uuid value)
			{
				aimpl->CreatedEntity.properties().insert(table_entity::property_type(PropertyName, entity_property(value)));
			}

			void AzureEntity::Insert(utility::string_t PropertyName, utility::string_t value)
			{
				aimpl->CreatedEntity.properties().insert(table_entity::property_type(PropertyName, entity_property(value)));
			}

			void AzureEntity::Insert(utility::string_t PropertyName, double value)
			{
				aimpl->CreatedEntity.properties().insert(table_entity::property_type(PropertyName, entity_property(value)));
			}

			bool AzureEntity::ExecuteEntity()
			{
				table_operation operation1 = table_operation::insert_or_replace_entity(aimpl->CreatedEntity);
				try
				{
					aimpl->table.execute(operation1);
					return true;
				}
				catch (const storage_exception& e)
				{
					aimpl->bolt_logger << BoltLog::LOG_ERROR << utility::conversions::to_utf8string(e.result().extended_error().message())
						<< e.result().http_status_code();
				}
				return false;
			}

			bool AzureEntity::PatchEntity()
			{
				table_operation operation = table_operation::merge_entity(aimpl->CreatedEntity);

				try
				{
					aimpl->table.execute(operation);
					return true;
				}
				catch (const storage_exception& e)
				{
					aimpl->bolt_logger << BoltLog::LOG_ERROR << utility::conversions::to_utf8string(e.result().extended_error().message())
						<< e.result().http_status_code();
				}
				return false;
			}

			bool AzureEntity::ReplaceEntity()
			{
				table_operation operation = table_operation::replace_entity(aimpl->CreatedEntity);
				try
				{
					aimpl->table.execute(operation);
					return true;
				}
				catch (const storage_exception& e)
				{
					aimpl->bolt_logger << BoltLog::LOG_ERROR << utility::conversions::to_utf8string(e.result().extended_error().message())
						<< e.result().http_status_code();
				}
				return false;
			}
		}
	}
}