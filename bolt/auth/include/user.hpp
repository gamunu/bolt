#pragma once

#ifdef BOLTAUTH_DLL
#define BOLTAUTH_API __declspec( dllexport )
#else
#define BOLTAUTH_API __declspec( dllimport )
#endif
#include <string>
#include <logger.hpp>

namespace bolt {
	namespace auth {

		class User
		{
		public:
			BOLTAUTH_API User() {}
			BOLTAUTH_API ~User() {}

			/// <summary>
			/// generate random string with 20 charaters.
			/// </summary>
			/// <returns></returns>
			std::string gen_random();

			/// <summary>
			/// get current time as timestamp.
			/// </summary>
			/// <returns></returns>
			// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
			std::string getcurrenttime();

			const std::string currentDateTime();
			BOLTAUTH_API std::string getkey(std::string username);
		private:
			BoltLog bolt_logger;
		};
	}
}
