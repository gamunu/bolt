#pragma once

#include <string>
#include <vector>
#include "Connection.h"
using namespace std;

class User
{



	public:
		User(void);
		~User(void);
		string adduser(string accountname);
		string generatekeys(string accountname);
		string gen_random();
		const std::string currentDateTime();
		bool removeuser(string accountname);
		string User::getkey(string user);
		map<string,string>  list_users();


};

