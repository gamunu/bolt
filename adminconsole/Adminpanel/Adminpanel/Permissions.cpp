#include "stdafx.h"
#include "Permissions.h"
#include <iostream>
#include <string>



bool Permissions::addtblper(string key,string tbl,int type,char val){


	string type_st=gettype(type);

	sql::ResultSet *res;
	sql::Statement *stmt0;
	Connection con;
	stmt0=con.getConnection()->createStatement();


	string qry0="SELECT * FROM table_permissions WHERE account_key = '";
	qry0.append(key);
	qry0.append("' AND table_name = '");
	qry0.append(tbl);
	qry0.append("'");


	res=stmt0->executeQuery(qry0);


	while(res->next()){




		sql::Statement *stmt1;
		stmt1=con.getConnection()->createStatement();


		string qry1="UPDATE table_permissions SET ";
		qry1.append(type_st);
		qry1.append(" = '");
		qry1.append(string(1,val));
		qry1.append("' WHERE account_key = '");
		qry1.append(key);
		qry1.append("' AND table_name = '");
		qry1.append(tbl);
		qry1.append("'");
		bool ret=stmt1->execute(qry1);


		return ret;

	}


	sql::Statement *stmt2;
	stmt2=con.getConnection()->createStatement();

	sql::Statement *stmt3;
	sql::ResultSet *res3;
	stmt3=con.getConnection()->createStatement();

	string testqry="SELECT table_name from information_schema.tables where table_schema='auth' AND table_name='"+tbl+"'";

	res3=stmt3->executeQuery(testqry);

	bool ret;

	if(res3->rowsCount()==0){

		ret=true;

	}else{

	string qry2="INSERT INTO table_permissions(table_name,account_key,";
	qry2.append(type_st);
	qry2.append(") VALUES(");			
	qry2.append("'");
	qry2.append(tbl);
	qry2.append("','");
	qry2.append(key);
	qry2.append("','");
	qry2.append(string(1,val));	
	qry2.append("')");

	ret=stmt2->execute(qry2);
	
	}

	return ret;
}



string Permissions::gettype(int type){

	if(type==1){

		return "select_p";

	}else if(type==2){

		return "update_p";

	}else if(type==3){

		return "delete_p";

	}else if(type==4){

		return "insert_p";

	}else if(type==5){

		return "alter_p";

	}

}


string Permissions::getcolumntype(int type){

	if(type==1){

		return "select_p";

	}else if(type==2){

		return "update_p";

	}else if(type==3){

		return "insert_p";

	}

}




bool Permissions::addtblperset(string key,string tbl,char sel,char update,char del,char insert,char alter){



	sql::Statement *stmt0;
	sql::ResultSet *res;

	Connection con;

	stmt0=con.getConnection()->createStatement();
	string qry0="SELECT * FROM table_permissions WHERE account_key = '";
	qry0.append(key);
	qry0.append("' AND table_name = '");
	qry0.append(tbl);
	qry0.append("'");


	res=stmt0->executeQuery(qry0);


	while(res->next()){

		sql::Statement *stmt1;
		stmt1=con.getConnection()->createStatement();;

		string qry1="UPDATE table_permissions SET select_p = '";
		qry1.append(string(1,sel));
		qry1.append("', update_p = '");
		qry1.append(string(1,update));
		qry1.append("', delete_p = '");
		qry1.append(string(1,del));
		qry1.append("', insert_p = '");
		qry1.append(string(1,insert));
		qry1.append("', alter_p = '");
		qry1.append(string(1,alter));
		qry1.append("' WHERE account_key = '");
		qry1.append(key);
		qry1.append("' AND table_name = '");
		qry1.append(tbl);
		qry1.append("'");
		bool ret=stmt1->execute(qry1);


		return ret;

	}


	sql::Statement *stmt3;
	sql::ResultSet *res3;
	stmt3=con.getConnection()->createStatement();

	string testqry="SELECT table_name from information_schema.tables where table_schema='auth' AND table_name='"+tbl+"'";

	res3=stmt3->executeQuery(testqry);

	bool ret;

	if(res3->rowsCount()==0){

		ret=true;

	}else{

	sql::Statement *stmt2;
	stmt2=con.getConnection()->createStatement();
	string qry2="INSERT INTO table_permissions(table_name,account_key,select_p,update_p,delete_p,insert_p,alter_p) VALUES(";
	qry2.append("'");
	qry2.append(tbl);
	qry2.append("','");
	qry2.append(key);
	qry2.append("','");
	qry2.append(string(1,sel));
	qry2.append("','");
	qry2.append(string(1,update));
	qry2.append("','");
	qry2.append(string(1,del));
	qry2.append("','");
	qry2.append(string(1,insert));
	qry2.append("','");
	qry2.append(string(1,alter));
	qry2.append("')");

	ret=stmt2->execute(qry2);

	}

	return ret;


}


bool Permissions::addcolumnper(string key,string tbl,vector<string> columnlist,int type,char val){

	for(std::vector<string>::iterator it = columnlist.begin(); it != columnlist.end(); ++it) {

		string colunm=*it;
		Connection con;


		string type_st=getcolumntype(type);
		sql::Statement *stmt0;
		sql::ResultSet *res;
		stmt0=con.getConnection()->createStatement();
		string qry0="SELECT * FROM column_permissions WHERE account_key = '";
		qry0.append(key);
		qry0.append("' AND table_name = '");
		qry0.append(tbl);
		qry0.append("' AND column_name = '");
		qry0.append(colunm);
		qry0.append("'");


		res=stmt0->executeQuery(qry0);


		if(res->next()){

			sql::Statement *stmt1;
			stmt1=con.getConnection()->createStatement();;

			string qry1="UPDATE column_permissions SET ";
			qry1.append(type_st);
			qry1.append(" = '");
			qry1.append(string(1,val));
			qry1.append("' WHERE account_key = '");
			qry1.append(key);
			qry1.append("' AND table_name = '");
			qry1.append(tbl);
			qry1.append("' AND column_name = '");
			qry1.append(colunm);
			qry1.append("'");
			bool ret=stmt1->execute(qry1);




		}else{


			sql::Statement *stmt2;
			stmt2=con.getConnection()->createStatement();;

			string qry2="INSERT INTO column_permissions(table_name,account_key,column_name,";
			qry2.append(type_st);
			qry2.append(") VALUES(");			
			qry2.append("'");
			qry2.append(tbl);
			qry2.append("','");
			qry2.append(key);
			qry2.append("','");
			qry2.append(colunm);
			qry2.append("','");
			qry2.append(string(1,val));	
			qry2.append("')");

			bool ret=stmt2->execute(qry2);


		}


	}


	return true;


}





bool Permissions::addcolumnperset(string key,string tbl,vector<string> columnlist,char sel,char update,char insert){


	for(std::vector<string>::iterator it = columnlist.begin(); it != columnlist.end(); ++it) {

		string colunm=*it;
		Connection con;

		sql::Statement *stmt0;
		sql::ResultSet *res;
		stmt0=con.getConnection()->createStatement();;
		string qry0="SELECT * FROM column_permissions WHERE account_key = '";
		qry0.append(key);
		qry0.append("' AND table_name = '");
		qry0.append(tbl);
		qry0.append("' AND column_name = '");
		qry0.append(colunm);
		qry0.append("'");


		res=stmt0->executeQuery(qry0);


		if(res->next()){

			sql::Statement *stmt1;
			stmt1=con.getConnection()->createStatement();;

			string qry1="UPDATE column_permissions SET select_p = '";
			qry1.append(string(1,sel));
			qry1.append("', update_p = '");
			qry1.append(string(1,update));
			qry1.append("', insert_p = '");
			qry1.append(string(1,insert));
			qry1.append("' WHERE account_key = '");
			qry1.append(key);
			qry1.append("' AND table_name = '");
			qry1.append(tbl);
			qry1.append("' AND column_name = '");
			qry1.append(colunm);
			qry1.append("'");
			bool ret=stmt1->execute(qry1);




		}else{


			sql::Statement *stmt2;
			stmt2=con.getConnection()->createStatement();;

			string qry2="INSERT INTO column_permissions(table_name,account_key,column_name,select_p,update_p,insert_p) VALUES(";
			qry2.append("'");
			qry2.append(tbl);
			qry2.append("','");
			qry2.append(key);
			qry2.append("','");
			qry2.append(colunm);
			qry2.append("','");
			qry2.append(string(1,sel));
			qry2.append("','");
			qry2.append(string(1,update));
			qry2.append("','");
			qry2.append(string(1,insert));
			qry2.append("')");

			bool ret=stmt2->execute(qry2);



		}

	}


	return true;

}


bool Permissions::chktbl(string key,string tbl,int type){

	string type_st=gettype(type);
	sql::Statement *stmt0;
	sql::ResultSet *res;
	Connection con;
	stmt0=con.getConnection()->createStatement();;
	string qry0="SELECT * FROM table_permissions WHERE account_key = '";
	qry0.append(key);
	qry0.append("' AND table_name = '");
	qry0.append(tbl);
	qry0.append("' AND ");
	qry0.append(type_st);
	qry0.append(" = '");
	qry0.append("Y");
	qry0.append("'");
	res=stmt0->executeQuery(qry0);

	if(res->next()){

		return true;


	}else{


		return false;

	}


}



bool Permissions::chkcolumns(string key,string tbl,map<string,int> columns){


	typedef map<string,int>::iterator it_type;
	for(it_type iterator = columns.begin(); iterator != columns.end(); iterator++) {


		string column=iterator->first;
		int type=iterator->second;
		string type_st=getcolumntype(type);
		Connection con;

		sql::Statement *stmt0;
		sql::ResultSet *res;
		stmt0=con.getConnection()->createStatement();
		string qry0="SELECT * FROM column_permissions WHERE account_key = '";
		qry0.append(key);
		qry0.append("' AND table_name = '");
		qry0.append(tbl);
		qry0.append("' AND column_name = '");
		qry0.append(column);
		qry0.append("' AND colum '");
		qry0.append(column);
		qry0.append("'");


		res=stmt0->executeQuery(qry0);




	}

	return true;


}


vector<string> Permissions::gettblper(string username){

	sql::Statement *stmt;
	sql::ResultSet *res;
	Connection con;
	vector<string> result;
 	stmt=con.getConnection()->createStatement();

	string qry="SELECT * FROM auth_users WHERE account_name = ";
	qry.append("'");
	qry.append(username);
	qry.append("'");

	res=stmt->executeQuery(qry);

	if(res->next()){

		string key=res->getString("account_key");

		sql::Statement *stmt1;
		sql::ResultSet *res1;
		stmt1=con.getConnection()->createStatement();

		string qry1="SELECT * FROM table_permissions WHERE account_key = ";
		qry1.append("'");
		qry1.append(key);
		qry1.append("'");

		res1=stmt1->executeQuery(qry1);


		while(res1->next()){
		
		
			string row="";
			row.append(username);
			row.append(" \t");
			row.append(res1->getString("table_name"));
			row.append(" \t");
			row.append(res1->getString("select_p"));
			row.append(" \t");
			row.append(res1->getString("update_p"));
			row.append(" \t");
			row.append(res1->getString("delete_p"));
			row.append(" \t");
			row.append(res1->getString("insert_p"));
			row.append(" \t");
			row.append(res1->getString("alter_p"));
			
			result.push_back(row);
			
		}

	}
		return result;

}



vector<string> Permissions::getclmnper(string username){

	sql::Statement *stmt;
	sql::ResultSet *res;
	Connection con;
	vector<string> result;
 	stmt=con.getConnection()->createStatement();

	string qry="SELECT * FROM auth_users WHERE account_name = ";
	qry.append("'");
	qry.append(username);
	qry.append("'");

	res=stmt->executeQuery(qry);

	if(res->next()){

		string key=res->getString("account_key");

		sql::Statement *stmt1;
		sql::ResultSet *res1;
		stmt1=con.getConnection()->createStatement();

		string qry1="SELECT * FROM column_permissions WHERE account_key = ";
		qry1.append("'");
		qry1.append(key);
		qry1.append("'");

		res1=stmt1->executeQuery(qry1);


		while(res1->next()){
		
		
			string row="";
			row.append(username);
			row.append(" \t  ");
			row.append(res1->getString("table_name"));
			row.append(" \t");
			row.append(res1->getString("column_name"));
			row.append(" \t\t  ");
			row.append(res1->getString("select_p"));
			row.append(" \t  ");
			row.append(res1->getString("update_p"));
			row.append(" \t  ");
			row.append(res1->getString("insert_p"));
			
			result.push_back(row);
			
		}

	}
		return result;

}
