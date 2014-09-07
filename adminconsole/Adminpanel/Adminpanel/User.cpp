#include "stdafx.h"
#include "User.h"
#include <time.h>
#include "sha256.h"

User::User(void)
{
}


User::~User(void)
{
}

	string User::adduser(string accountname)
		{
			Connection con;
			std::auto_ptr<sql::Connection> connection(con.getConnection());
			std::auto_ptr<sql::PreparedStatement> prep_stmt(
				connection->prepareStatement("SELECT * FROM auth_users WHERE account_name = ?")
				);

			prep_stmt->setString(1,accountname);
			std::auto_ptr<sql::ResultSet> result(prep_stmt->executeQuery());
			
			while(result->next())
				{
					return "username already taken"; 
				}

			std::auto_ptr<sql::PreparedStatement> prep_stmt_insert(
				connection->prepareStatement("INSERT INTO auth_users(account_name,account_key) VALUES(? , ? )")
				);

			prep_stmt_insert->setString(1,accountname);
			prep_stmt_insert->setString(2, generatekeys(accountname));

			if(prep_stmt_insert->execute()==false){
				return "Added successfully";
			}
		

			return "Adding failed";

			
		
	}


		/// <summary>
		/// Generate keys for specified username and store it in database.
		/// </summary>
		/// <param name="username">The username.</param>
		/// <param name="first_name">The first_name.</param>
		/// <returns></returns>
		string User::generatekeys(string accountname){
			string key=accountname;
			string rand=gen_random();
			key.append(rand);

			string key_hash=sha256(key);//sha256(key);

			return key_hash;
		}

		/// <summary>
		/// generate random string with 20 charaters.
		/// </summary>
		/// <returns></returns>
		string User::gen_random() 
		{
			auto randchar = []() -> char
			{
				const char charset[] =
					"0123456789"
					"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
					"abcdefghijklmnopqrstuvwxyz";
				const size_t max_index = (sizeof(charset) - 1);
				return charset[ rand() % max_index ];
			};
			std::string str(20,0);
			std::generate_n( str.begin(), 20, randchar);
			return str;
		}	





		/// <summary>
		/// get current time as timestamp.
		/// </summary>
		/// <returns></returns>
		// Get current date/time, format is YYYY-MM-DD.HH:mm:ss
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




		/// <summary>
		/// Remove users that are specified by username.
		/// </summary>
		/// <param name="username">The username.</param>
		/// <returns></returns>
		bool User::removeuser(string username)
		{
			Connection con;
			std::auto_ptr<sql::Connection> connection(con.getConnection());

			std::auto_ptr<sql::PreparedStatement> prep_stmt(
				connection->prepareStatement("SELECT * FROM auth_users WHERE account_name = ?")
				);

			prep_stmt->setString(1,username);
			std::auto_ptr<sql::ResultSet> result(prep_stmt->executeQuery());

			std::auto_ptr<sql::PreparedStatement> prep_stmt_delete(
				connection->prepareStatement("DELETE FROM auth_users WHERE user_id = ?")
				);


			while(result->next())
			{
				prep_stmt_delete->setString(1,result->getString("user_id"));
				prep_stmt_delete->execute();
				return true;
			}

			return false;
		}


		string User::getkey(string user)
		{
				
			sql::Statement *stmt;
			sql::ResultSet *res;
			Connection con;
			stmt=con.getConnection()->createStatement();

			string qry="SELECT * FROM auth_users WHERE account_name = ";
			qry.append("'");
			qry.append(user);
			qry.append("'");

			res=stmt->executeQuery(qry);

			if(res->next()){

				return res->getString("account_key");

			}
				
			return "Invalid";
		
		}


		map<string,string>  User::list_users(){
		
			sql::Statement *stmnt;
			sql::ResultSet *res;




			map<string,string> list;
			string qry="SELECT * FROM auth_users";
			Connection con;
			stmnt=con.getConnection()->createStatement();
			res=stmnt->executeQuery(qry);

			while(res->next()){
			
			
				string usr=res->getString("account_name");
				string key=res->getString("account_key");
				list.insert(pair<string, string>(usr, key));

			
			
			}


			return list;
		
		}