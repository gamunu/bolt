#pragma once

#include <unordered_map>
#include <cpprest/asyncrt_utils.h>

namespace bolt
{
	namespace storage
	{
		namespace mysql
		{
			/// <summary>
			/// Enumeration containing the types of values that can be stored in a table entity property.
			/// </summary>
			enum class myedm_type
			{
				/// <summary>
				/// Represents fixed- or variable-length character data.
				/// </summary>
				string,

				/// <summary>
				/// Represents fixed- or variable-length binary data.
				/// </summary>
				binary,

				/// <summary>
				/// Represents the mathematical concept of binary-valued logic.
				/// </summary>
				boolean,

				/// <summary>
				/// Represents date and time.
				/// </summary>
				datetime,

				/// <summary>
				/// Represents a floating point number with 15 digits precision that can represent values with approximate range of +/- 2.23e -308 through +/- 1.79e +308.
				/// </summary>
				double_floating_point,

				/// <summary>
				/// Represents a 16-byte (128-bit) unique identifier value.
				/// </summary>
				guid,

				/// <summary>
				/// Represents a signed 32-bit integer value.
				/// </summary>
				int32,

				/// <summary>
				/// Represents a signed 64-bit integer value.
				/// </summary>
				int64,
			};

		}
	}
}