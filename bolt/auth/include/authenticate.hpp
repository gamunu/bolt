#ifdef BOLTAUTH_DLL
#define BOLTAUTH_API __declspec( dllexport )
#else
#define BOLTAUTH_API __declspec( dllimport )
#endif

#include <logger.hpp>
#include "cpprest/basic_types.h"

namespace bolt {
	namespace auth {

		class BOLTAUTH_API Authenticate
		{
		public:
			/// <summary>
			/// check whether the token is valid
			/// </summary>
			/// <param name="token">The token.</param>
			/// <returns></returns>
			bool getAuthorization(utility::string_t account_name, utility::string_t shared_key);
		private:
			BoltLog bolt_logger;
		};

	}
}
