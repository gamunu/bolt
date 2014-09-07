#pragma once

#ifdef BOLTAUTH_DLL
#define BOLTAUTH_API __declspec( dllexport )
#else
#define BOLTAUTH_API __declspec( dllimport )
#endif

#include <cpprest/asyncrt_utils.h>
#include <logger.hpp>

class Permissions
{
public:
	enum table_permissions
	{
		select,
		insert,
		update,
		del
	};
	BOLTAUTH_API Permissions() {}
	BOLTAUTH_API ~Permissions() {}


	BOLTAUTH_API bool hasGet(utility::string_t table_name, utility::string_t user);
	BOLTAUTH_API bool hasPost(utility::string_t table_name, utility::string_t user);
	BOLTAUTH_API bool hasDelete(utility::string_t table_name, utility::string_t user);
	BOLTAUTH_API bool hasMerge(utility::string_t table_name, utility::string_t user);

	BOLTAUTH_API bool hasGet(utility::string_t table_name, utility::string_t user, std::vector<utility::string_t> columns);
	BOLTAUTH_API bool hasPost(utility::string_t table_name, utility::string_t user, std::vector<utility::string_t> columns);
	BOLTAUTH_API bool hasDelete(utility::string_t table_name, utility::string_t user, std::vector<utility::string_t> columns);
	BOLTAUTH_API bool hasMerge(utility::string_t table_name, utility::string_t user, std::vector<utility::string_t> columns);
private:
	bool hasTablePermissions(utility::string_t table_name, utility::string_t user, table_permissions type);
	bool hasTablePermissions(utility::string_t table_name, utility::string_t user, table_permissions type, std::vector<utility::string_t> columns);
	BoltLog bolt_logger;
};