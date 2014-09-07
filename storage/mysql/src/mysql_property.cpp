#include <mysql_property.h>

namespace bolt {
	namespace storage {
		namespace mysql {

			std::vector<uint8_t> mysql_property::binary_value() const
			{
				if (m_property_type != myedm_type::binary)
				{
					throw std::runtime_error(error_entity_property_not_binary);
				}

				return std::vector<uint8_t>(utility::conversions::from_base64(m_value));
			}

			bool mysql_property::boolean_value() const
			{
				if (m_property_type != myedm_type::boolean)
				{
					throw std::runtime_error(error_entity_property_not_boolean);
				}

				if (m_value.compare(U("false")) == 0)
				{
					return false;
				}
				if (m_value.compare(U("true")) == 0)
				{
					return true;
				}
				throw std::runtime_error(error_parse_boolean);
			}

			utility::datetime mysql_property::datetime_value() const
			{
				if (m_property_type != myedm_type::datetime)
				{
					throw std::runtime_error(error_entity_property_not_datetime);
				}

				utility::datetime result = utility::datetime::from_string(m_value, utility::datetime::ISO_8601);
				if (!result.is_initialized())
				{
					throw std::runtime_error(error_parse_datetime);
				}

				return result;
			}

			double mysql_property::double_value() const
			{
				if (m_property_type != myedm_type::double_floating_point)
				{
					throw std::runtime_error(error_entity_property_not_double);
				}

				if (m_value.compare(double_not_a_number) == 0)
				{
					return std::numeric_limits<double>::quiet_NaN();
				}
				if (m_value.compare(double_infinity) == 0)
				{
					return std::numeric_limits<double>::infinity();
				}
				if (m_value.compare(double_negative_infinity) == 0)
				{
					return -std::numeric_limits<double>::infinity();
				}

				double result;
				utility::istringstream_t buffer(m_value);
				buffer >> result;

				if (buffer.fail() || !buffer.eof())
				{
					throw std::runtime_error(error_parse_double);
				}

				return result;
			}

			/*	utility::uuid entity_property::guid_value() const
				{
				if (m_property_type != edm_type::guid)
				{
				throw std::runtime_error(mysql::error_entity_property_not_guid);
				}

				utility::uuid result = utility::string_to_uuid(m_value);
				return result;
				}
				*/
			int32_t mysql_property::int32_value() const
			{
				if (m_property_type != myedm_type::int32)
				{
					throw std::runtime_error(error_entity_property_not_int32);
				}

				int32_t result;
				utility::istringstream_t buffer(m_value);
				buffer >> result;

				if (buffer.fail() || !buffer.eof())
				{
					throw std::runtime_error(error_parse_int32);
				}

				return result;
			}

			int64_t mysql_property::int64_value() const
			{
				if (m_property_type != myedm_type::int64)
				{
					throw std::runtime_error(error_entity_property_not_int64);
				}

				int64_t result;
				utility::istringstream_t buffer(m_value);
				buffer >> result;
				return result;
			}

			utility::string_t mysql_property::string_value() const
			{
				if (m_property_type != myedm_type::string)
				{
					throw std::runtime_error(error_entity_property_not_string);
				}

				return m_value;
			}

			void mysql_property::set_value_impl(double value)
			{
				utility::ostringstream_t buffer;
				buffer.precision(std::numeric_limits<double>::digits10 + 2);
				buffer << value;
				m_value = buffer.str();
			}

			void mysql_property::set_value_impl(long double value)
			{
				utility::ostringstream_t buffer;
				buffer.precision(std::numeric_limits<long double>::digits10 + 2);
				buffer << value;
				m_value = buffer.str();
			}

			void mysql_property::set_value_impl(int32_t value)
			{
				utility::ostringstream_t buffer;
				buffer << value;
				m_value = buffer.str();
			}

			void mysql_property::set_value_impl(int64_t value)
			{
				utility::ostringstream_t buffer;
				buffer << value;
				m_value = buffer.str();
			}
		}

	}
}