#ifndef SENDTOSERVER_H
#define SENDTOSERVER_H

#include <string>
#include <QTcpSocket>
#include <QHostAddress>

using std::string;

const int LOGIN_SUCCESS = 1;
const int REGISTER_SUCCESS = 1;
const int GETPASSWDBACK_SUCCESS = 1;
const int FILE_ERROR = 0;
const int REGISTER_FAILURE = 0;
const int GETPASSWDBACK_FAILURE = 0;
const int INVALID_USERNAME = -1;
const int PSW_ERROR = -2;
const int BUFLEN = 255;

int SendtoServer(string str, string option);
#endif // SENDTOSERVER_H
