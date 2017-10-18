#include "sendtoserver.h"
#include <QMessageBox>
#include <QDebug>
#include <QAbstractSocket>
#include <sstream>

const int MINPORTNUM = 20000;
const int MAXPORTNUM = 25000;
// receive user list from server, used for chat and transfer files.
string userList = "";
string port = "";

int Send::SendtoServer(string str, string option)
{
    char sendBuf[BUFLEN], recvBuf[BUFLEN];
    string tmp = option;

    port = GeneratePort();
    option = option + "!" + str;
    strcpy(sendBuf, option.c_str());
    memset(recvBuf, '\0', BUFLEN);

    QTcpSocket *clientsocket = new QTcpSocket();
    clientsocket->connectToHost("127.0.0.1", 5050);
    clientsocket->waitForConnected();

    if(tmp == "login")
    {
        clientsocket->write(sendBuf, strlen(sendBuf)+1);
        clientsocket->waitForBytesWritten();

        clientsocket->waitForReadyRead();
        clientsocket->read(recvBuf, BUFLEN);

        if(strcmp(recvBuf, "LOGIN_SUCCESS") == 0) // login success
        {
            clientsocket->waitForReadyRead();
            clientsocket->read(recvBuf, BUFLEN);
            userList(recvBuf);
        	return LOGIN_SUCCESS;
        }
        else if(strcmp(recvBuf, "INVALID_USERNAME") == 0)
        {
            return INVALID_USERNAME;
        }
        else if(strcmp(recvBuf, "PSW_ERROR") == 0)
        {
            return PSW_ERROR;
        }
        else if(strcmp(recvBuf, "FILE_ERROR") == 0)
        {
            return FILE_ERROR;
        }
    }
    else if(tmp == "register") // test OK
    {
        memset(sendBuf, '\0', BUFLEN);
        option = option + "+" + port;
        strcpy(sendBuf, option.c_str());

        clientsocket->write(sendBuf, strlen(sendBuf)+1);
        clientsocket->waitForBytesWritten();

        clientsocket->waitForReadyRead();
        clientsocket->read(recvBuf, BUFLEN);

        if(strcmp(recvBuf, "REGISTER_SUCCESS") == 0) // login success
        {
            return REGISTER_SUCCESS;
        }
        else if(strcmp(recvBuf, "REGISTER_FAILURE") == 0)
        {
            return REGISTER_FAILURE;
        }
    }
    else if(tmp == "getpasswdback")
    {
        clientsocket->write(sendBuf, strlen(sendBuf)+1);
        clientsocket->waitForBytesWritten();

        clientsocket->waitForReadyRead();
        clientsocket->read(recvBuf, BUFLEN);

        if(strcmp(recvBuf, "GETPASSWDBACK_SUCCESS") == 0) // login success
        {
            return GETPASSWDBACK_SUCCESS;
        }
        else if(strcmp(recvBuf, "GETPASSWDBACK_FAILURE") == 0)
        {
            return GETPASSWDBACK_FAILURE;
        }
    }
    clientsocket->disconnectFromHost();
    if (clientsocket->state() == QAbstractSocket::UnconnectedState || clientsocket->waitForDisconnected(1000))
        qDebug("Disconnected!");
    else
        qDebug("Disconnect faild!");
    return 0;
}

string& GeneratePort()
{
    srand((unsigned)time(0));
    int port = rand() % (MAXPORTNUM - MINPORTNUM + 1) + MINPORTNUM;
    string p;
    stringstream ss;
    ss << port;
    ss >> p;

    return p;
}
