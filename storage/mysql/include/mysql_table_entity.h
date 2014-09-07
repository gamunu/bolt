#pragma once
#include <mysql_property.h>

namespace bolt
{
	namespace storage
	{
		namespace mysql
		{
			/// <summary>
			/// Represents an entity in a table.
			/// </summary>
			class mysql_table_entity
			{
			public:

				typedef std::unordered_map<utility::string_t, mysql::mysql_property> properties_type;
				typedef std::unordered_map<utility::string_t, mysql::mysql_property>::value_type property_type;

				/// <summary>
				/// Initializes a new instance of the <see cref="mysql::storage::mysql_table_entity"/> class.
				/// </summary>
				mysql_table_entity() {}

				/// <summary>
				/// Initializes a new instance of the <see cref="mysql::storage::mysql_table_entity"/> class with the specified partition key and row key.
				/// </summary>
				/// <param name="partition_key">The partition key value for the entity.</param>
				/// <param name="row_key">The row key value for the entity.</param>
				mysql_table_entity(utility::string_t partition_key, utility::string_t row_key)
					: m_partition_key(std::move(partition_key)), m_row_key(std::move(row_key))
				{
				}

				/// <summary>
				/// Initializes a new instance of the <see cref="mysql::storage::mysql_table_entity"/> class with the entity's partition key, row key, ETag (if available/required), and properties.
				/// </summary>
				/// <param name="partition_key">The entity's partition key.</param>
				/// <param name="row_key">The entity's row key.</param>
				/// <param name="etag">The entity's current ETag.</param>
				/// <param name="properties">The entity's properties, indexed by property name.</param>
				mysql_table_entity(utility::string_t partition_key, utility::string_t row_key, utility::string_t etag, properties_type properties)
					: m_partition_key(std::move(partition_key)), m_row_key(std::move(row_key)), m_etag(std::move(etag)), m_properties(std::move(properties))
				{
				}

				/// <summary>
				/// Sets the entity's properties.
				/// </summary>
				/// <param name="properties">The entity partition key.</param>
				void set_properties(properties_type properties)
				{
					m_properties = std::move(properties);
				}

				/// <summary>
				/// Gets the properties in the table entity, indexed by property name.
				/// </summary>
				/// <returns>The entity properties.</returns>
				properties_type& properties()
				{
					return m_properties;
				}


				/// <summary>
				/// Add a entity property.
				/// </summary>
				/// <param name="key">The propery key.</param>
				/// <param name="value">The propery value.</param>
				void add_property(utility::string_t key, mysql_property value)
				{
					m_properties.insert(std::make_pair(key, value));
				}

				/// <summary>
				/// Clear all properties
				/// </summary>
				void clear_properties()
				{
					m_properties.clear();
				}

				/// <summary>
				/// Gets the properties in the table entity, indexed by property name.
				/// </summary>
				/// <returns>The entity properties.</returns>
				const properties_type& properties() const
				{
					return m_properties;
				}

				/// <summary>
				/// Gets the entity's partition key.
				/// </summary>
				/// <returns>The entity partition key.</returns>
				const utility::string_t& partition_key() const
				{
					return m_partition_key;
				}

				/// <summary>
				/// Sets the entity's partition key.
				/// </summary>
				/// <param name="partition_key">The entity partition key.</param>
				void set_partition_key(utility::string_t partition_key)
				{
					m_partition_key = std::move(partition_key);
				}

				/// <summary>
				/// Gets the entity's row key.
				/// </summary>
				/// <returns>The entity row key.</returns>
				const utility::string_t& row_key() const
				{
					return m_row_key;
				}

				/// <summary>
				/// Sets the entity's row key.
				/// </summary>
				/// <param name="row_key">The entity row key.</param>
				void set_row_key(utility::string_t row_key)
				{
					m_row_key = std::move(row_key);
				}

				/// <summary>
				/// Gets the entity's timestamp.
				/// </summary>
				/// <returns>The entity timestamp.</returns>
				utility::datetime timestamp() const
				{
					return m_timestamp;
				}

				/// <summary>
				/// Sets the entity's timestamp.
				/// </summary>
				/// <param name="timestamp">The entity timestamp.</param>
				void set_timestamp(utility::datetime timestamp)
				{
					m_timestamp = timestamp;
				}

				/// <summary>
				/// Gets the entity's current ETag.
				/// </summary>
				/// <returns>The entity's ETag value, as a string.</returns>
				/// <remarks>
				/// Set this value to "*" in order to overwrite an entity as part of an update operation.
				/// </remarks>
				const utility::string_t& etag() const
				{
					return m_etag;
				}

				/// <summary>
				/// Sets the entity's current ETag.
				/// </summary>
				/// <param name="etag">The entity's ETag value, as a string.</param>
				/// <remarks>
				/// Set this value to "*" in order to overwrite an entity as part of an update operation.
				/// </remarks>
				void set_etag(utility::string_t etag)
				{
					m_etag = std::move(etag);
				}

			private:
				properties_type m_properties;
				utility::string_t m_partition_key;
				utility::string_t m_row_key;
				utility::datetime m_timestamp;
				utility::string_t m_etag;
			};


		}
	}
}