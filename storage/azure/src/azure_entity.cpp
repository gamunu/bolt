#include <azure_entity.h>

namespace bolt  {
	namespace storage {
		namespace boltazure {

			class AzureEntity::AEImpl
			{
			public:
				cloud_table table;
				table_entity tab_entity;
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
				aimpl->tab_entity = entity;
			}

			int AzureEntity::createEntity(utility::string_t PartitonKey, utility::string_t RowKey, size_t size)
			{
				// Insert a table entity
				table_entity entity(PartitonKey, RowKey);
				entity.properties().reserve(size);
				aimpl->tab_entity = entity;
				return 1;
			}

			void AzureEntity::insert(utility::string_t PropertyName, int32_t value)
			{
				aimpl->tab_entity.properties().insert(table_entity::property_type(PropertyName, entity_property(value)));
			}

			void AzureEntity::insert(utility::string_t PropertyName, int64_t value){
				aimpl->tab_entity.properties().insert(table_entity::property_type(PropertyName, entity_property(value)));
			}

			void AzureEntity::insert(utility::string_t PropertyName, bool value)
			{
				aimpl->tab_entity.properties().insert(table_entity::property_type(PropertyName, entity_property(value)));
			}

			void AzureEntity::insert(utility::string_t  PropertyName, utility::datetime value)
			{
				aimpl->tab_entity.properties().insert(table_entity::property_type(PropertyName, entity_property(value)));
			}

			void AzureEntity::insert(utility::string_t PropertyName, utility::uuid value)
			{
				aimpl->tab_entity.properties().insert(table_entity::property_type(PropertyName, entity_property(value)));
			}

			void AzureEntity::insert(utility::string_t PropertyName, utility::string_t value)
			{
				aimpl->tab_entity.properties().insert(table_entity::property_type(PropertyName, entity_property(value)));
			}

			void AzureEntity::insert(utility::string_t PropertyName, double value)
			{
				aimpl->tab_entity.properties().insert(table_entity::property_type(PropertyName, entity_property(value)));
			}

			bool AzureEntity::executeEntity()
			{
				table_operation operation1 = table_operation::insert_or_replace_entity(aimpl->tab_entity);
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

			bool AzureEntity::patchEntity()
			{
				table_operation operation = table_operation::merge_entity(aimpl->tab_entity);

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

			bool AzureEntity::abort()
			{
				table_entity table_en = aimpl->tab_entity;
				table_operation operation2 = table_operation::retrieve_entity(table_en.partition_key(), table_en.row_key());
				table_result retrieve_result = aimpl->table.execute(operation2);

				table_operation operation4 = table_operation::delete_entity(retrieve_result.entity());

				try {
					aimpl->table.execute(operation4);
					return true;
				}
				catch (const storage_exception& e)
				{
					aimpl->bolt_logger << BoltLog::LOG_ERROR << utility::conversions::to_utf8string(e.result().extended_error().message())
						<< e.result().http_status_code();
				}
				return false;
			}

			bool AzureEntity::replaceEntity()
			{
				table_operation operation = table_operation::replace_entity(aimpl->tab_entity);
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