#include "sendtoserver.h"
#include <QMessageBox>

const char USER_LOGIN[] = "USER_LOGIN";
const char USER_REGISTER[] = "USER_REGISTER";
const char USER_GETPASSWDBACK[] = "USER_GETPASSWDBACK";
const char end[] = "END";

int SendtoServer(string str, string option)
{
    QTcpSocket *clientsocket = new QTcpSocket();
    clientsocket->connectToHost("127.0.0.1", 5050);
    bool is_conn = clientsocket->waitForConnected();
    if(is_conn == false)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

//int SendtoServer(string str, string option)
//{
//    WSADATA wsaData;
//    int nRC;
//    sockaddr_in srvAddr, clientAddr;
//    SOCKET clientSock;
//    char sendBuf[BUFLEN], recvBuf[BUFLEN];

//    strcpy(sendBuf, str.c_str());

//    //初始化 winsock
//    nRC = WSAStartup(0x0101, &wsaData);
//    //创建 client socket
//    clientSock = socket(AF_INET, SOCK_STREAM, 0);

//    clientAddr.sin_family = AF_INET;
//    clientAddr.sin_port = htons(0);
//    clientAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.2");
//    nRC = bind(clientSock, (LPSOCKADDR)&clientAddr, sizeof(clientAddr));

//    //准备服务器的信息，这里需要指定服务器的地址
//    srvAddr.sin_family = AF_INET;
//    srvAddr.sin_port = htons(5050);
//    srvAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

//    //连接服务器
//    nRC = connect(clientSock,(LPSOCKADDR)&srvAddr, sizeof(srvAddr));

//    //向服务器发送数据和从服务器接受数据
//    memset(recvBuf, '\0', BUFLEN);

//    if(option == "login")
//    {
//        nRC = send(clientSock, USER_LOGIN, strlen(USER_LOGIN), 0);
//        nRC = recv(clientSock, recvBuf, BUFLEN, 0);
//        if(strcmp(recvBuf, "PREPARE_LOGIN") == 0)
//        {
//            nRC = send(clientSock, sendBuf, strlen(sendBuf), 0);
//            nRC = recv(clientSock, recvBuf, BUFLEN, 0);
//            if(strcmp(recvBuf, "LOGIN_SUCCESS") == 0) // login success
//            {
//                return 1;
//            }
//            else if(strcmp(recvBuf, "INVALID_USERNAME") == 0)
//            {

//                return INVALID_USERNAME;
//            }
//            else if(strcmp(recvBuf, "PSW_ERROR") == 0)
//            {

//                return PSW_ERROR;
//            }
//            else if(strcmp(recvBuf, "FILE_ERROR") == 0)
//            {

//                return FILE_ERROR;
//            }
//        }
//        else
//        {

//        }
//    }
//    else if(option == "register")
//    {

//    }
//    else if(option == "getpasswdback")
//    {

//    }


//    closesocket(clientSock);
//    WSACleanup();
//    return 0;
//}
