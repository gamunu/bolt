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
			class mysql_incognito_entity
			{
			public:

				typedef std::unordered_map<utility::string_t, mysql::mysql_property> properties_type;
				typedef std::unordered_map<utility::string_t, mysql::mysql_property>::value_type property_type;

				/// <summary>
				/// Initializes a new instance of the <see cref="mysql::storage::mysql_table_entity"/> class.
				/// </summary>
				mysql_incognito_entity() {}

				/// <summary>
				/// Initializes a new instance of the <see cref="mysql::storage::mysql_table_entity"/> class with the entity's partition key, row key, ETag (if available/required), and properties.
				/// </summary>
				/// <param name="properties">The entity's properties, indexed by property name.</param>
				mysql_incognito_entity(properties_type properties)
					: m_properties(std::move(properties))
				{
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
				/// Gets the properties in the table entity, indexed by property name.
				/// </summary>
				/// <returns>The entity properties.</returns>
				const properties_type& properties() const
				{
					return m_properties;
				}

			private:
				properties_type m_properties;
			};


		}
	}
}