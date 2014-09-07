// Adminpanel.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include<iostream>
#include <string>
#include <conio.h>
#include <iomanip>
#include "User.h"
#include "Permissions.h"
#include "Connection.h"

using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{


	string pass;
	char ch;
	const char ENTER = 13;
	cout<<"Enter password:";

	while((ch = _getch()) != ENTER)
	{
		pass += ch;
		std::cout << '*';
	}



	if(pass=="12345"){

		system("cls");


		User *user=new User();

		cout<<"#################################"<<endl;
		cout<<"#				#"<<endl;
		cout<<"#  Welcome to Bolt Admin Panel  #"<<endl;
		cout<<"#				#"<<endl;
		cout<<"#################################"<<endl;

		cout<<user->currentDateTime()<<endl;
		cout<<endl;
		cout<<endl;
		cout<<"Type 'help;' or 'h' for help."<<endl;
		cout<<endl;
		cout<<endl;

		
		string command;
		while(command != "exit"){
			cout<<"Enter your command:";
			cin>>command;

			if(command == "add"){

				string usr;

				cout<<"enter username:";
				cin>>usr;
				string reply=user->adduser(usr);
				cout<<reply<<endl;


			}else if(command == "getkey"){

				string key;
				string username;
				cout<<"Enter username: ";
				cin>>username;

				key=user->getkey(username);
				if(key != "Invalid"){

					cout<<"key:"+key<<endl;


				}else{

					cout<<"Invalid username "<<endl;

				}



			}else if(command == "list"){

				map<string,string> list=user->list_users();
				cout<<endl;
				cout<<"Username       Key"<<endl;
				cout<<endl;


				typedef map<string,string>::iterator it_type;
				for(it_type iterator = list.begin(); iterator != list.end(); iterator++) {

					
					cout<<setw(15)<<setiosflags(ios::left)<<iterator->first+" "<<setw(40)<<iterator->second+" "<<endl;
					
				}

				cout<<endl;

			}else if(command == "help" | command=="h"){

				cout<<endl;

				cout<<"add   \t\tAdd new user"<<endl;
				cout<<"getkey\t\tTo get the key of the given user"<<endl;
				cout<<"remove\t\tTo remove user from the system"<<endl;
				cout<<"list  \t\tTo get details about all the users"<<endl;
				cout<<"addprv\t\tTo add table permission for a user"<<endl;
				cout<<"addfullprv  \tTo add all table permissions for a user"<<endl;
				cout<<"addclmnprv  \tTo add column permissions for a user"<<endl;
				cout<<"addclmnfullprv \tTo add all column permissions for a user"<<endl;
				cout<<"findprev \tTo find given table previleges for specified user"<<endl;
				cout<<"findclmnprev \tTo find given cloumn previleges for specified user"<<endl;
				cout<<"exit \t\tTo exit from the console"<<endl;

				cout<<endl;



			}else if(command == "addprv"){

				string username;
				string tblname;
				int type;
				char val;

				cin>>username;
				string key=user->getkey(username);

				if(key=="Invalid"){

					cout<<"Invalid username"<<endl;
					continue;

				}else{

					cin>>tblname;


					if ( !(cin >> type) )
					{
						cin.clear();
						cin.ignore();
						cout << "Incorrect entry. Try again: ";
						continue;
					}


					if ( !(	cin>>val) )
					{
						cin.clear();
						cin.ignore();
						cout << "Incorrect entry. Try again: ";
						continue;
					}



					Permissions *p=new Permissions();
					bool out=p->addtblper(key,tblname,type,val);

					if(!out){

						cout<<"Sucessfully permission added"<<endl;

					}else{

						cout<<"Permission not added"<<endl;

					}

				}

			}else if(command == "addfullprv"){

				string username;
				string tblname;
				char val1;
				char val2;
				char val3;
				char val4;
				char val5;

				cin>>username;
				string key=user->getkey(username);

				if(key=="Invalid"){

					cout<<"Invalid username"<<endl;
					continue;

				}else{

					cin>>tblname;


					if ( !(	cin>>val1) )
					{
						cin.clear();
						cin.ignore();
						cout << "Incorrect entry. Try again: ";
						continue;
					}

					if ( !(	cin>>val2) )
					{
						cin.clear();
						cin.ignore();
						cout << "Incorrect entry. Try again: ";
						continue;
					}

					if ( !(	cin>>val3) )
					{
						cin.clear();
						cin.ignore();
						cout << "Incorrect entry. Try again: ";
						continue;
					}

					if ( !(	cin>>val4) )
					{
						cin.clear();
						cin.ignore();
						cout << "Incorrect entry. Try again: ";
						continue;
					}

					if ( !(	cin>>val5) )
					{
						cin.clear();
						cin.ignore();
						cout << "Incorrect entry. Try again: ";
						continue;
					}


					Permissions *p=new Permissions();
					bool out=p->addtblperset(key,tblname,val1,val2,val3,val4,val5);

					if(!out){

						cout<<"Sucessfully permission added"<<endl;

					}else{
					
						cout<<"Table do not exists"<<endl;
					
					}


				}

			}else if(command == "addclmnprv"){

				string username;
				string tblname;
				vector<string> clmlist;

				cin>>username;
				string key=user->getkey(username);


				if(key=="Invalid"){

					cout<<"Invalid username"<<endl;
					continue;

				}else{

					cin>>tblname;
					bool chk=true;
					while(chk){

						string val;
						cin>>val;
						if(val=="@"){
							chk=false;
						}else{

							clmlist.push_back(val);

						}

					}

					int type;
					if ( !(	cin>>type) )
					{
						cin.clear();
						cin.ignore();
						cout << "Incorrect entry. Try again: ";
						continue;
					}

					char val;
					if ( !(	cin>>val) )
					{
						cin.clear();
						cin.ignore();
						cout << "Incorrect entry. Try again: ";
						continue;
					}


					Permissions *p=new Permissions();
					bool out=p->addcolumnper(key,tblname,clmlist,type,val);

					if(out){

						cout<<"Sucessfully permission added"<<endl;



					}else{

						cout<<"Permission not added"<<endl;

					}

				}

			}else if(command=="addclmnfullprv"){


				string username;
				string tblname;
				vector<string> clmlist;

				cin>>username;
				string key=user->getkey(username);


				if(key=="Invalid"){

					cout<<"Invalid username"<<endl;
					continue;

				}else{

					cin>>tblname;
					bool chk=true;
					while(chk){

						string val;
						cin>>val;
						if(val=="@"){
							chk=false;
						}else{

							clmlist.push_back(val);

						}

					}

					char val1;
					if ( !(	cin>>val1) )
					{
						cin.clear();
						cin.ignore();
						cout << "Incorrect entry. Try again: ";
						continue;
					}

					char val2;
					if ( !(	cin>>val2) )
					{
						cin.clear();
						cin.ignore();
						cout << "Incorrect entry. Try again: ";
						continue;
					}

					char val3;
					if ( !(	cin>>val3) )
					{
						cin.clear();
						cin.ignore();
						cout << "Incorrect entry. Try again: ";
						continue;
					}

					Permissions *p=new Permissions();
					bool out=p->addcolumnperset(key,tblname,clmlist,val1,val2,val3);

					if(out){

						cout<<"Sucessfully permission added"<<endl;



					}else{

						cout<<"Permission not added"<<endl;

					}

				}


			}else if(command == "findprev"){

				vector<string> list;
				string username;
				cin>>username;

				Permissions *p=new Permissions();
				list=p->gettblper(username);


				if(!list.empty()){

					cout<<"Username   Table_name   select  update  delete  insert  alter"<<endl;
					cout<<endl;
					for(string i :list){ 


						cout<<i<<endl;

					}

					cout<<endl;

				}else{

					cout<<"invalid username or no previlege is given to the user"<<endl;

				}

			
				}else if(command == "findclmnprev"){

				vector<string> list;
				string username;
				cin>>username;

				Permissions *p=new Permissions();
				list=p->getclmnper(username);


				if(!list.empty()){

					cout<<"Username   Table_name   Column_name   select  update  insert"<<endl;
					cout<<endl;
					for(string i :list){ 


						cout<<i<<endl;

					}

					cout<<endl;

				}else{

					cout<<"invalid username or no previlege is given to the user"<<endl;

				}


			
			}else if(command == "remove"){

				string usr;
				cout<<"Enter user you want to remove: ";
				cin>>usr;
				bool out=user->removeuser(usr);
				if(out){

					cout<<"Succesfully removed"<<endl;

				}else{

					cout<<"invalid username"<<endl;

				}


			}else{

				cout<<"Invalid command"<<endl;


			}


		}

	}else{

		cout<<"Incorrect Password"<<endl;
		return 0;

	}


	return 0;
}

