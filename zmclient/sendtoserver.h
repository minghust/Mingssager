#ifndef SENDTOSERVER_H
#define SENDTOSERVER_H

//#pragma comment(lib, "ws2_32.lib")

//#include <stdio.h>
//#include <winsock2.h>
//#include <string.h>
//#include <windows.h>
//#include <process.h>
//#include <winbase.h>
#include <iostream>
//#include <vector>
//#include <string>
//#define BUFLEN 255

//using std::cout;
//using std::cin;
//using std::endl;
//using std::vector;
using std::string;

int SendtoServer(string str, string option);

const int LOGIN_SUCCESS = 1;
const int FILE_ERROR = 0;
const int INVALID_USERNAME = -1;
const int PSW_ERROR = -2;

#include <QTcpSocket>
#include <QHostAddress>

#endif // SENDTOSERVER_H
