#pragma once

#ifdef BOLTMYSQL_DLL
#define BOLTMYSQL_API __declspec( dllexport )
#else
#define BOLTMYSQL_API __declspec( dllimport )
#endif

#include <table.h>
#include <cpprest/streams.h>

namespace bolt
{
	namespace storage
	{
		namespace mysql
		{
			const std::string error_empty_batch_operation("The batch operation cannot be empty.");
			const std::string error_batch_operation_partition_key_mismatch("The batch operation cannot contain entities with different partition keys.");
			const std::string error_batch_operation_retrieve_count("The batch operation cannot contain more than one retrieve operation.");
			const std::string error_batch_operation_retrieve_mix("The batch operation cannot contain any other operations when it contains a retrieve operation.");
			const std::string error_entity_property_not_binary("The type of the entity property is not binary.");
			const std::string error_entity_property_not_boolean("The type of the entity property is not boolean.");
			const std::string error_parse_boolean("An error occurred parsing the boolean.");
			const std::string error_parse_double("An error occurred parsing the double.");
			const std::string error_entity_property_not_datetime("The type of the entity property is not date/time.");
			const std::string error_parse_datetime("An error occurred parsing the date/time.");
			const std::string error_entity_property_not_double("The type of the entity property is not double.");
			const std::string error_entity_property_not_guid("The type of the entity property is not GUID.");
			const std::string error_entity_property_not_int32("The type of the entity property is not 32-bit integer.");
			const std::string error_parse_int32("An error occurred parsing the 32-bit integer.");
			const std::string error_entity_property_not_int64("The type of the entity property is not 64-bit integer.");
			const std::string error_entity_property_not_string("The type of the entity property is not string.");


			const utility::string_t double_not_a_number(U("NaN"));
			const utility::string_t double_infinity(U("Infinity"));
			const utility::string_t double_negative_infinity(U("-Infinity"));
			/// <summary>
			/// Class for storing information about a single property in an entity in a table.
			/// </summary>
			class mysql_property
			{
			public:


				/// <summary>
				/// Initializes a new instance of the <see cref="entity_property"/> class.
				/// </summary>
				mysql_property()
					: m_property_type(myedm_type::string), m_is_null(true)
				{
				}

				/// <summary>
				/// Initializes a new instance of the <see cref="entity_property"/> class with a byte array value.
				/// </summary>
				/// <param name="value">A byte array.</param>
				mysql_property(const std::vector<uint8_t>& value)
					: m_property_type(myedm_type::binary), m_is_null(false)
				{
					set_value_impl(value);
				}

				/// <summary>
				/// Initializes a new instance of the <see cref="entity_property"/> class with a boolean value.
				/// </summary>
				/// <param name="value">A boolean value.</param>
				mysql_property(bool value)
					: m_property_type(myedm_type::boolean), m_is_null(false)
				{
					set_value_impl(value);
				}

				/// <summary>
				/// Initializes a new instance of the <see cref="entity_property"/> class with a date/time value.
				/// </summary>
				/// <param name="value">A datetime value.</param>
				mysql_property(utility::datetime value)
					: m_property_type(myedm_type::datetime), m_is_null(false)
				{
					set_value_impl(value);
				}

				/// <summary>
				/// Initializes a new instance of the <see cref="entity_property"/> class with a double precision floating-point number value.
				/// </summary>
				/// <param name="value">A double value.</param>
				mysql_property(double value)
					: m_property_type(myedm_type::double_floating_point), m_is_null(false)
				{
					set_value_impl(value);
				}

				/// <summary>
				/// Initializes a new instance of the <see cref="entity_property"/> class with a GUID value.
				/// </summary>
				/// <param name="value">A GUID value.</param>
				/*		entity_property(const utility::uuid& value)
							: m_property_type(edm_type::guid), m_is_null(false)
							{
							set_value_impl(value);
							}*/

				/// <summary>
				/// Initializes a new instance of the <see cref="entity_property"/> class with a 32-bit integer value.
				/// </summary>
				/// <param name="value">A 32-bit integer value.</param>
				mysql_property(int32_t value)
					: m_property_type(myedm_type::int32), m_is_null(false)
				{
					set_value_impl(value);
				}

				/// <summary>
				/// Initializes a new instance of the <see cref="entity_property"/> class with a 64-bit integer value.
				/// </summary>
				/// <param name="value">A 64-bit integer value.</param>
				mysql_property(int64_t value)
					: m_property_type(myedm_type::int64), m_is_null(false)
				{
					set_value_impl(value);
				}

				/// <summary>
				/// Initializes a new instance of the <see cref="entity_property"/> class with a string value.
				/// </summary>
				/// <param name="value">A string value.</param>
				mysql_property(utility::string_t value)
					: m_property_type(myedm_type::string), m_is_null(false), m_value(move(value))
				{
				}

				/// <summary>
				/// Initializes a new instance of the <see cref="entity_property"/> class with a string value.
				/// </summary>
				/// <param name="value">A string value.</param>
				mysql_property(const utility::char_t* value)
					: m_property_type(myedm_type::string), m_is_null(false)
				{
					set_value_impl(value);
				}

				/// <summary>
				/// Gets the property type of the <see cref="entity_property" /> object.
				/// </summary>
				/// <returns>An <see cref="edm_type" /> object.</returns>
				myedm_type property_type() const
				{
					return m_property_type;
				}

				/// <summary>
				/// Sets the property type of the <see cref="entity_property" /> object.
				/// </summary>
				/// <param name="property_type">An <see cref="edm_type" /> object indicating the property type.</param>
				void set_property_type(myedm_type property_type)
				{
					m_property_type = property_type;
				}

				/// <summary>
				/// Indicates whether the value is null.
				/// </summary>
				/// <returns><c>true</c> if the value is null.</returns>
				bool is_null() const
				{
					return m_is_null;
				}

				/// <summary>
				/// Sets the value to null.
				/// </summary>
				/// <param name="value"><c>true</c> to set the value to null.</param>
				void set_is_null(bool value)
				{
					m_is_null = value;
				}

				// TODO: Consider renaming the getters to as_binary, as_boolean, etc.

				/// <summary>
				/// Gets the byte array value of the <see cref="entity_property"/> object.
				/// </summary>
				/// <returns>The byte array value of the <see cref="entity_property"/> object.</returns>
				/// <remarks>
				/// An exception is thrown if this property is set to a value other than a byte array.
				/// </remarks>
				BOLTMYSQL_API std::vector<uint8_t> binary_value() const;

				/// <summary>
				/// Gets the boolean value of the <see cref="entity_property"/> object.
				/// </summary>
				/// <returns>The boolean value of the <see cref="entity_property"/> object.</returns>
				/// <remarks>
				/// An exception is thrown if this property is set to a value other than a boolean value.
				/// </remarks>
				BOLTMYSQL_API bool boolean_value() const;

				/// <summary>
				/// Gets the datetime value of the <see cref="entity_property"/> object.
				/// </summary>
				/// <returns>The datetime value of the <see cref="entity_property"/> object.</returns>
				/// <remarks>
				/// An exception is thrown if this property is set to a value other than a datetime value.
				/// </remarks>
				BOLTMYSQL_API utility::datetime datetime_value() const;

				/// <summary>
				/// Gets the double-precision floating point value of the <see cref="entity_property"/> object.
				/// </summary>
				/// <returns>The double-precision floating point value of the <see cref="entity_property"/> object.</returns>
				/// <remarks>
				/// An exception is thrown if this property is set to a value other than a double-precision floating point value.
				/// </remarks>
				BOLTMYSQL_API double double_value() const;

				/// <summary>
				/// Gets the GUID value of the <see cref="entity_property"/> object.
				/// </summary>
				/// <returns>The GUID value of the <see cref="entity_property"/> object.</returns>
				/// <remarks>
				/// An exception is thrown if this property is set to a value other than a GUID value.
				/// </remarks>
				/*	BOLTMYSQL_API utility::uuid guid_value() const;*/

				/// <summary>
				/// Gets the 32-bit integer value of the <see cref="entity_property"/> object.
				/// </summary>
				/// <returns>The 32-bit integer value of the <see cref="entity_property"/> object.</returns>
				/// <remarks>
				/// An exception is thrown if this property is set to a value other than a 32-bit integer value.
				/// </remarks>
				BOLTMYSQL_API int32_t int32_value() const;

				/// <summary>
				/// Gets the 64-bit integer value of the <see cref="entity_property"/> object.
				/// </summary>
				/// <returns>The 64-bit integer value of the <see cref="entity_property"/> object.</returns>
				/// <remarks>
				/// An exception is thrown if this property is set to a value other than a 64-bit integer value.
				/// </remarks>
				BOLTMYSQL_API int64_t int64_value() const;

				/// <summary>
				/// Gets the string value of the <see cref="entity_property"/> object.
				/// </summary>
				/// <returns>The string value of the <see cref="entity_property"/> object.</returns>
				/// <remarks>
				/// An exception is thrown if this property is set to a value other than a string value.
				/// </remarks>
				BOLTMYSQL_API utility::string_t string_value() const;

				// TODO: Use std::vector<uint8_t> for binary data throughout the library

				/// <summary>
				/// Sets the byte array value of the <see cref="entity_property"/> object.
				/// </summary>
				/// <param name="value">The byte array value.</param>
				void set_value(const std::vector<uint8_t>& value)
				{
					set_value_impl(value);
				}

				/// <summary>
				/// Sets the boolean value of the <see cref="entity_property"/> object.
				/// </summary>
				/// <param name="value">The boolean value.</param>
				void set_value(bool value)
				{
					m_property_type = myedm_type::boolean;
					m_is_null = false;
					set_value_impl(value);
				}

				// TODO: Test timezone parsing

				/// <summary>
				/// Sets the datetime value of the <see cref="entity_property"/> object.
				/// </summary>
				/// <param name="value">The datetime value.</param>
				void set_value(utility::datetime value)
				{
					m_property_type = myedm_type::datetime;
					m_is_null = false;
					set_value_impl(value);
				}

				/// <summary>
				/// Sets the double-precision floating point value of the <see cref="entity_property"/> object.
				/// </summary>
				/// <param name="value">The double-precision floating point value.</param>
				void set_value(double value)
				{
					m_property_type = myedm_type::double_floating_point;
					m_is_null = false;
					set_value_impl(value);
				}

				void set_value(long double value)
				{
					m_property_type = myedm_type::double_floating_point;
					m_is_null = false;
					set_value_impl(value);
				}
				/// <summary>
				/// Sets the GUID value of the <see cref="entity_property"/> object.
				/// </summary>
				/// <param name="value">The GUID point value.</param>
				/*	void set_value(const utility::uuid& value)
					{
					m_property_type = edm_type::guid;
					m_is_null = false;
					set_value_impl(value);
					}*/

				/// <summary>
				/// Sets the 32-bit integer value of the <see cref="entity_property"/> object.
				/// </summary>
				/// <param name="value">The 32-bit integer value.</param>
				void set_value(int32_t value)
				{
					m_property_type = myedm_type::int32;
					m_is_null = false;
					set_value_impl(value);
				}

				/// <summary>
				/// Sets the 64-bit integer value of the <see cref="entity_property"/> object.
				/// </summary>
				/// <param name="value">The 64-bit integer value.</param>
				void set_value(int64_t value)
				{
					m_property_type = myedm_type::int64;
					m_is_null = false;
					set_value_impl(value);
				}

				/// <summary>
				/// Sets the string value of the <see cref="entity_property"/> object.
				/// </summary>
				/// <param name="value">The string value.</param>
				void set_value(utility::string_t value)
				{
					m_property_type = myedm_type::string;
					m_is_null = false;
					m_value = move(value);
				}

				/// <summary>
				/// Sets the string value of the <see cref="entity_property"/> object.
				/// </summary>
				/// <param name="value">The string value.</param>
				void set_value(const utility::char_t * value)
				{
					m_property_type = myedm_type::string;
					m_is_null = false;
					set_value_impl(value);
				}

				/// <summary>
				/// Returns the value of the <see cref="entity_property"/> object as a string.
				/// </summary>
				/// <returns>A string containing the property value.</returns>
				const utility::string_t& str() const
				{
					return m_value;
				}

			private:

				void set_value_impl(const std::vector<uint8_t>& value)
				{
					m_value = utility::conversions::to_base64(value);
				}

				void set_value_impl(bool value)
				{
					m_value = value ? U("true") : U("false");
				}

				void set_value_impl(utility::datetime value)
				{
					m_value = value.to_string(utility::datetime::ISO_8601);
				}

				BOLTMYSQL_API void set_value_impl(double value);

				BOLTMYSQL_API void set_value_impl(long double value);
				/*void set_value_impl(const utility::uuid& value)
				{
				m_value = utility::uuid_to_string(value);
				}*/

				BOLTMYSQL_API void set_value_impl(int32_t value);

				BOLTMYSQL_API void set_value_impl(int64_t value);

				void set_value_impl(const utility::char_t * value)
				{
					m_value = value;
				}

				myedm_type m_property_type;
				bool m_is_null;
				utility::string_t m_value;
			};
		}
	}
}