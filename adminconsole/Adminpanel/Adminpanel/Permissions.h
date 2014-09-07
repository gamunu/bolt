#pragma once
#include <string>
#include <vector>
#include "Connection.h"

using namespace std;

class Permissions
{
	public:
	
		
		bool addtblper(string key,string tbl,int type,char value);
		bool addtblperset(string key,string tbl,char sel,char update,char del,char insert,char alter);
		bool addcolumnper(string key,string tbl,vector<string> columnlist,int type,char value);
		bool addcolumnperset(string key,string tbl,vector<string> columnlist,char sel,char update,char insert);
		string gettype(int type);
		string getcolumntype(int type);
		bool chktbl(string key,string tbl,int type);
		bool chkcolumns(string key,string tbl,map<string,int> columns);
		vector<string> gettblper(string username);
		vector<string> getclmnper(string username);
		
};

