#include "sendtoserver.h"
#include <QMessageBox>
#include <QDebug>
#include <QAbstractSocket>

const char end[] = "END";

int SendtoServer(string str, string option)
{
    char sendBuf[BUFLEN], recvBuf[BUFLEN];
    string tmp = option;
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
