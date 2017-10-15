#include "chat.h"
#include "ui_chat.h"
//#include "clientstart.h"
#include <QMessageBox>
#include <QString>
#include <QFile>
#include <QFileDialog>
#include <string>
#include <QTextCursor>
#include <QTextTable>
#include <QScrollBar>

using std::string;

Chat::Chat(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Chat)
{
    ui->setupUi(this);
    ui->msg->setReadOnly(true); // set show message window read-only
    tableFormat.setBorder(0);
//    ClientStart();
}

Chat::~Chat()
{
    delete ui;
}



void Chat::on_sendmsg_clicked()
{
    QString str = ui->input->toPlainText();
    if(str.isEmpty())
    {
        QMessageBox::information(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("输入不能为空！"));
        return;
    }
    else
    {
//        TcpSendMsg(str.toStdString());



        // append message from user
        QTextCursor cursor(ui->msg->textCursor());
        cursor.movePosition(QTextCursor::End);
        QTextTable *table = cursor.insertTable(1, 2, tableFormat);
        table->cellAt(0, 0).firstCursorPosition().insertText("<127.0.0.1>");
        table->cellAt(0, 1).firstCursorPosition().insertText(QString::fromLocal8Bit("我说：") + str + '\n');
        QScrollBar *bar = ui->msg->verticalScrollBar();
        bar->setValue(bar->maximum());
        ui->input->clear();
    }
}

void Chat::on_choosefile_clicked()
{
    ui->showfilename->clear();
    QString filename=QFileDialog::getOpenFileName(this,tr("choose file"),"D:\\");
    ui->showfilename->setText(filename);

    return;
}

void Chat::on_sendfile_clicked()
{
    if(ui->showfilename->text().isEmpty())
    {
        QMessageBox::information(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("请选择文件！"));
        return;
    }
    else
    {
//        UdpSendFile();
    }
}

//void ClientStart(QString )
//{
//    WSADATA wsaData;
//    int nRC;
//    sockaddr_in srvAddr, clientAddr;
//    SOCKET clientSock;
//    char sendBuf[BUFLEN], recvBuf[BUFLEN];
//    FD_SET rfds, wfds;
//    u_long uNonBlock;
//    HANDLE hThread;
//    unsigned dwThreadID;

//    vector<string>cli;

//    InitializeCriticalSection(&gCriticalSection);
//    //初始化 winsock
//    nRC = WSAStartup(0x0101, &wsaData);
//    if (nRC)
//    {
//        printf("Client initialize winsock error!\n");
//        return;
//    }
//    if (wsaData.wVersion != 0x0101)
//    {
//        printf("Client's winsock version error!\n");
//        WSACleanup();
//        return;
//    }
//    printf("Client's winsock initialized !\n");

//    //创建 client socket
//    clientSock = socket(AF_INET, SOCK_STREAM, 0);
//    if (clientSock == INVALID_SOCKET)
//    {
//        printf("Client create socket error!\n");
//        WSACleanup();
//        return;
//    }
//    printf("Client  socket create OK!\n");

//    clientAddr.sin_family = AF_INET;
//    clientAddr.sin_port = htons(0);
//    clientAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.2");
//    nRC = bind(clientSock,(LPSOCKADDR)&clientAddr, sizeof(clientAddr));
//    if (nRC == SOCKET_ERROR)
//    {
//        printf("Client socket bind error!\n");
//        closesocket(clientSock);
//        WSACleanup();
//        return;
//    }
//    printf("Client socket bind OK!\n");


//    //准备服务器的信息，这里需要指定服务器的地址
//    srvAddr.sin_family = AF_INET;
//    srvAddr.sin_port = htons(5050);
//    srvAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

//    //连接服务器
//    nRC = connect(clientSock,
//        (LPSOCKADDR)&srvAddr, sizeof(srvAddr));
//    if (nRC == SOCKET_ERROR)
//    {
//        printf("连接服务器失败!\n");
//        closesocket(clientSock);
//        WSACleanup();
//        return;
//    }
//    int i = 0;
//    //向服务器发送数据和从服务器接受数据
//    while (1)
//    {
//        //清空发送和接收缓冲区
//        memset(sendBuf, '\0', BUFLEN);
//        memset(recvBuf, '\0', BUFLEN);

//        //将SOCKET设为非阻塞模式，
//        //并且等待有数据到来或者可以发送数据
//        FD_ZERO(&rfds);
//        FD_ZERO(&wfds);
//        FD_SET(clientSock, &rfds);
//        FD_SET(clientSock, &wfds);
//        uNonBlock = 1;
//        ioctlsocket(clientSock, FIONBIO, &uNonBlock);
//        select(0, &rfds, &wfds, NULL, NULL);

//        //如果有数据到来
//        if (FD_ISSET(clientSock, &rfds))
//        {

//            //接受服务器发来的数据并且显示
//            nRC = recv(clientSock, recvBuf, BUFLEN, 0);
//            if (nRC == SOCKET_ERROR)
//            {
//                printf("接收数据失败!\n");
//                DeleteCriticalSection(&gCriticalSection);
//                closesocket(clientSock);
//                WSACleanup();
//                return;
//            }
//            else if (nRC > 0)
//            {
//                recvBuf[nRC] = '\0';
//                if (strcmp(recvBuf, argv[1]) != 0)	// don't equal
//                {
//                    string s(&recvBuf[0], &recvBuf[strlen(recvBuf)]);
//                    cli.push_back(s);
//                    cout << "recvbuf:" << recvBuf << endl;
//                }
//                cout << i << "vector: ";
//                for (auto it = cli.begin(); it != cli.end(); it++)
//                    cout << *it << ' ';
//                cout << endl << endl;
//            }
//        }
//        //如果可以发送数据
//        if (FD_ISSET(clientSock, &wfds))
//        {
//            //如果用户在键盘输入了信息，则发送
//            if (strlen(sendBuf) > 0)
//            {
//                nRC = send(clientSock, sendBuf, strlen(sendBuf), 0);
//                if (nRC == SOCKET_ERROR)
//                {
//                    printf("发送数据失败!\n");
//                    DeleteCriticalSection(&gCriticalSection);
//                    closesocket(clientSock);
//                    WSACleanup();
//                    return;
//                }
//                else
//                {
//                    EnterCriticalSection(&gCriticalSection);
//                    sendBuf[0] = '\0';
//                    LeaveCriticalSection(&gCriticalSection);
//                }
//            }
//        }
//        if (strcmp(sendBuf, "exit") == 0)
//        {
//            break;
//        }
//    }
//    DeleteCriticalSection(&gCriticalSection);
//    closesocket(clientSock);
//    WSACleanup();
//}
