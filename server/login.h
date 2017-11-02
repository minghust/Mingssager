#ifndef LOGIN_H
#define LOGIN_H

#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include "md5_encode.h"

const char USER_INFO_FILE[] = "info.txt";
const int LOGIN_SUCCESS = 1;
const int FILE_ERROR = 0;
const int INVALID_USERNAME = -1;
const int PSW_ERROR = -2;

using std::string;
 
typedef struct
{
	string name;
	string ip;
	string port;
	bool isOnline;
}UserList;

typedef struct
{
	string fromName;
	string toName;
	string msg;
}OfflineMessage;

void SplitString(const string& s, std::vector<string>& v, const string& c);
int VerifyUserLogin(string loginName, string loginPasswd);
bool GetbackPasswd(string userEmail, string newp);
bool SaveInfo(string regName, string regEmail, string regPasswd);
void UpdateUserList(std::vector<UserList> &ul);


#endif // LOGIN_H
