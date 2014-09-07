#include <authenticate.hpp>
#include <connection.hpp>
#include <cpprest/asyncrt_utils.h>

namespace bolt {
	namespace auth {
		bool Authenticate::getAuthorization(utility::string_t account_name, utility::string_t shared_key)
		{
			try {
				Connection &con = Connection::GetInstance();
				std::shared_ptr<sql::Connection> connection(con.getConnection());
				std::unique_ptr<sql::PreparedStatement> prep_stmt(connection->prepareStatement("SELECT user_id FROM auth_users WHERE account_name = ? AND shared_key = ?"));

				prep_stmt->setString(1, utility::conversions::to_utf8string(account_name));
				prep_stmt->setString(2, utility::conversions::to_utf8string(shared_key));

				std::unique_ptr<sql::ResultSet> res(prep_stmt->executeQuery());

				while (res->next())
					return true;
			}
			catch (sql::SQLException &e)
			{
				bolt_logger << BoltLog::LOG_ERROR << "SQLException in Authenticate "
					<< "(getAuthorization) #ERR: " << e.what() << " (MySQL error code: "
					<< std::to_string(e.getErrorCode()).c_str()
					<< ", SQLState: " << e.getSQLState().c_str() << " )";
			}
			return false;
		}
	}
}