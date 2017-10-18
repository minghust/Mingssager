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

int VerifyUserLogin(string loginName, string loginPasswd);
bool GetbackPasswd(string userEmail, string newp);
bool SaveInfo(string regName, string regEmail, string regPasswd, string port);


#endif // LOGIN_H
