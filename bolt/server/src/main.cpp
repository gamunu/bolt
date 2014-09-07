#include <stdio.h>
#include <windows.h>
#include <winservice_installer.hpp>
#include <winservice_base.hpp>
#include <winbservice.hpp>
#include <logger.hpp>
#include <configuration.hpp>

#pragma region Service Definitions

//
// Settings of the service
//

// Internal name of the service
#define SERVICE_NAME             L"WBoltService"

// Displayed name of the service
#define SERVICE_DISPLAY_NAME     L"Bolt Rest Server Service"

// Service start options.
#define SERVICE_START_TYPE       SERVICE_DEMAND_START

// List of service dependencies - "dep1\0dep2\0\0"
#define SERVICE_DEPENDENCIES     L""

// The name of the account under which the service should run
#define SERVICE_ACCOUNT          L"NT AUTHORITY\\LocalService"

// The password to the service account name
#define SERVICE_PASSWORD         NULL

#pragma endregion

// On Windows, all strings are wide.
// in windows utility::string_t is a typedef of std::wstring
// we are using two signatures to work with windows wide strings
int wmain(int argc, wchar_t *argv[])
{
	BoltLog log_file;
	
	if ((argc > 1) && ((*argv[1] == L'-' || (*argv[1] == L'/'))))
	{
		if (_wcsicmp(L"install", argv[1] + 1) == 0)
		{
			// Install the service when the command is
			// "-install" or "/install".
			InstallService(
				SERVICE_NAME,               // Name of service
				SERVICE_DISPLAY_NAME,       // Name to display
				SERVICE_START_TYPE,         // Service start type
				SERVICE_DEPENDENCIES,       // Dependencies
				SERVICE_ACCOUNT,            // Service running account
				SERVICE_PASSWORD            // Password of the account
				);
		}
		else if (_wcsicmp(L"remove", argv[1] + 1) == 0)
		{
			// Uninstall the service when the command is
			// "-remove" or "/remove".
			UninstallService(SERVICE_NAME);
		}
	}
	else
	{
		wprintf(L"Parameters:\n");
		wprintf(L" -install  to install the service.\n");
		wprintf(L" -remove   to remove the service.\n");

		WBService service(SERVICE_NAME);
		if (!WBService::Run(service))
		{
			wprintf(L"Service failed to run w/err 0x%08lx\n", GetLastError());
			log_file << BoltLog::LOG_INFO << "Service failed to run w / err 0x % 08lx\n" << GetLastError();
		}
	}

	return 0;
}