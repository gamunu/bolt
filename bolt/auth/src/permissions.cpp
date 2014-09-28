#include <permissions.hpp>
#include <connection.hpp>
using namespace bolt::auth;

bool Permissions::hasGet(utility::string_t table_name, utility::string_t user)
{
	return hasTablePermissions(table_name, user, select);
}

bool Permissions::hasPost(utility::string_t table_name, utility::string_t user)
{
	return hasTablePermissions(table_name, user, insert);
}

bool Permissions::hasDelete(utility::string_t table_name, utility::string_t user)
{
	return hasTablePermissions(table_name, user, del);
}

bool Permissions::hasMerge(utility::string_t table_name, utility::string_t user)
{
	return hasTablePermissions(table_name, user, update);
}

bool Permissions::hasGet(utility::string_t table_name, utility::string_t user, std::vector<utility::string_t> columns)
{
	return hasTablePermissions(table_name, user, select, columns);
}

bool Permissions::hasPost(utility::string_t table_name, utility::string_t user, std::vector<utility::string_t> columns)
{
	return hasTablePermissions(table_name, user, insert, columns);
}

bool Permissions::hasDelete(utility::string_t table_name, utility::string_t user, std::vector<utility::string_t> columns)
{
	return hasTablePermissions(table_name, user, del, columns);
}

bool Permissions::hasMerge(utility::string_t table_name, utility::string_t user, std::vector<utility::string_t> columns)
{
	return hasTablePermissions(table_name, user, update, columns);
}

bool Permissions::hasTablePermissions(utility::string_t table_name, utility::string_t user, table_permissions type)
{
	bool has_perm = false;
	try {
		std::string query = "SELECT ";

		switch (type)
		{
		case select:
			query += "tp.select_p"; break;
		case insert:
			query += "tp.insert_p"; break;
		case update:
			query += "tp.update_p"; break;
		case del:
			query += "tp.delete_p"; break;
		default: break;
		}

		query += " FROM table_permissions AS tp, account AS au WHERE tp.table_name = ? AND tp.account_id = au.id AND au.account_name = ?";
		Connection &con = Connection::GetInstance();
		std::unique_ptr<sql::PreparedStatement> pre_stmt(con.getConnection()->prepareStatement(query));
		pre_stmt->setString(1, utility::conversions::to_utf8string(table_name));
		pre_stmt->setString(2, utility::conversions::to_utf8string(user));

		std::unique_ptr<sql::ResultSet> res(pre_stmt->executeQuery());

		if (res->next())
		{
			if (res->getString(1) == "Y")
			{
				has_perm = true;
			}

		}

	}
	catch (sql::SQLException &e)
	{
		bolt_logger << BoltLog::LOG_ERROR << "SQLException in User "
			<< "(getkey) #ERR: " << e.what() << " (MySQL error code: "
			<< std::to_string(e.getErrorCode()).c_str()
			<< ", SQLState: " << e.getSQLState().c_str() << " )";
	}

	return has_perm;
}

bool Permissions::hasTablePermissions(utility::string_t table_name, utility::string_t user, table_permissions type, std::vector<utility::string_t> columns)
{
	bool has_perm = false;

	for (int i = 0; i < columns.size(); i++)
	{
		try {
			std::string query = "SELECT ";

			switch (type)
			{
			case select:
				query += "tp.select_p"; break;
			case insert:
				query += "tp.insert_p"; break;
			case update:
				query += "tp.update_p"; break;
			default: break;
			}

			query += " FROM column_permissions AS tp, auth_users AS au WHERE tp.table_name = ? AND tp.column_name = ?  AND tp.account_id = au.user_id AND au.account_name = ?";
			
			Connection &con = Connection::GetInstance();
			std::unique_ptr<sql::PreparedStatement> pre_stmt(con.getConnection()->prepareStatement(query));
			
			pre_stmt->setString(1, utility::conversions::to_utf8string(table_name));
			pre_stmt->setString(2, utility::conversions::to_utf8string(columns[i]));
			pre_stmt->setString(3, utility::conversions::to_utf8string(user));

			std::unique_ptr<sql::ResultSet> res(pre_stmt->executeQuery());

			if (res->next())
			{
				if (res->getString(1) == "N")
				{
					has_perm = false;
					continue;
				}
				has_perm = true;
			}
		}
		catch (sql::SQLException &e)
		{
			bolt_logger << BoltLog::LOG_ERROR << "SQLException in User "
				<< "(getkey) #ERR: " << e.what() << " (MySQL error code: "
				<< std::to_string(e.getErrorCode()).c_str()
				<< ", SQLState: " << e.getSQLState().c_str() << " )";
		}
	}
	return has_perm;
}