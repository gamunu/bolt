#pragma once

#ifdef BOLTAUTH_DLL
#define BOLTAUTH_API __declspec( dllexport )
#else
#define BOLTAUTH_API __declspec( dllimport )
#endif
#include "cpprest/asyncrt_utils.h"

namespace bolt {
	namespace auth {

		class Signature
		{
		public:

			/// <summary>
			/// Initializes a new instance of the <see cref="Signature"/> class.
			/// </summary>
			/// <param name="auth_string">The auth_string.</param>
			BOLTAUTH_API Signature(utility::string_t auth_string);

			BOLTAUTH_API Signature();
			BOLTAUTH_API ~Signature();

			/// <summary>
			/// Validates the signature.
			/// </summary>
			/// <param name="auth_string">The auth_string.</param>
			/// <param name="datetime">The datetime.</param>
			/// <param name="resource">The resource.</param>
			/// <param name="http_method">The http_method.</param>
			/// <returns></returns>
			BOLTAUTH_API bool isValied(utility::string_t datetime, utility::string_t resource, utility::string_t http_method);

			/// <summary>
			/// Creates the signature.
			/// </summary>
			/// <returns></returns>
			BOLTAUTH_API utility::string_t createSignature(std::string username);

			BOLTAUTH_API utility::string_t getUsername() const;

			BOLTAUTH_API utility::string_t getSignature() const;

		private:

			utility::string_t  m_auth_string;
			utility::string_t  m_http_method;
			utility::string_t  m_datetime;
			utility::string_t  m_resource;
			utility::string_t m_username;
			utility::string_t m_signature;

			/// <summary>
			/// Splits the user and password.
			/// </summary>
			/// <returns></returns>
			std::pair<utility::string_t, utility::string_t> splitUserAndPassword();

		};

	}
}