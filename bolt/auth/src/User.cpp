#include <user.hpp>
#include <connection.hpp>
#include <time.h>

namespace bolt {
	namespace auth {
		std::string User::gen_random()
		{
			auto randchar = []() -> char
			{
				const char charset[] =
					"0123456789"
					"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
					"abcdefghijklmnopqrstuvwxyz";
				const size_t max_index = (sizeof(charset) - 1);
				return charset[rand() % max_index];
			};
			std::string str(20, 0);
			std::generate_n(str.begin(), 20, randchar);
			return str;
		}

		const std::string User::currentDateTime() {
			time_t     now = time(0);
			struct tm  tstruct;
			char       buf[80];
			tstruct = *localtime(&now);
			// Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
			// for more information about date/time format
			strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

			return buf;
		}

		std::string User::getkey(std::string user)
		{
			try {
				std::string select_q = "SELECT * FROM auth_users WHERE account_name = ?";

				Connection &con = Connection::GetInstance();
				std::unique_ptr<sql::PreparedStatement> pre_stmt(con.getConnection()->prepareStatement(select_q));
				pre_stmt->setString(1, user);

				std::unique_ptr<sql::ResultSet> res(pre_stmt->executeQuery());

				if (res->next())
				{
					return res->getString("account_key");
				}
			}
			catch (sql::SQLException &e)
			{
				bolt_logger << BoltLog::LOG_ERROR << "SQLException in User "
					<< "(getkey) \n #ERR: " << e.what() << " (MySQL error code: "
					<< std::to_string(e.getErrorCode()).c_str()
					<< ", SQLState: " << e.getSQLState().c_str() << " )";
			}

			return nullptr;
		}
	}
}