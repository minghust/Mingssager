#include "dlog.h"
#include "ui_dlog.h"

#include <ctime>
#include <sstream>
#include <QCryptographicHash>

const int MINUDPPORTNUM = 30000;
const int MAXUDPPORTNUM = 35000;
const int BUFLEN = 500;
const int DATASIZE = 1500;
bool isError;
QByteArray buf;
typedef struct
{
    string name;
    string ip;
    string addr;
    string isOnline;
}Record;

typedef struct
{
    string fromName;
    string msg;
}OfflineMessage;

typedef struct
{
    string fromName;
    string fromIP;
    string fromPort;
    string fileSize;
    string fileName;
}RFInfo; // receive file info

RFInfo rfInfo = {"", "", "", "", ""};

extern string name; // self name
extern string port; // self tcp & udp port, used to listen for file's head infomation
extern Record fri;
extern string ip;
extern vector<OfflineMessage>offlinev;
extern QString serverIP;

// self-client's port, send to another client to let him know what port he should send "OK" back
string udpPort = "";

// another client's IP and port used to be sent datagram
string clientPort = "";
string clientIP = "";
void SplitStr(const string& s, vector<string>& v, const string& c);
void GenerateUdpPort();
Dlog::Dlog(QWidget *parent) : QWidget(parent), ui(new Ui::Dlog)
{
    ui->setupUi(this);
    ui->msg->setReadOnly(true); // set show message window read-only
    tableFormat.setBorder(0);

    ui->userName->setText("From: " + QString::fromStdString(name));
    ui->sendto->clear();
    ui->sendto->setText("To: "+QString::fromStdString(fri.name));
    ui->progressBar->hide();
    sendLength = 0;

    clientsocket = new QTcpSocket(this);
    serversocket = new QTcpServer(this);
    clientUdpSocket = new QUdpSocket(this);
    ServerUdpSocket = new QUdpSocket(this);
    serverResSocket = new QTcpSocket(this);

    GenerateUdpPort();

    if(!offlinev.empty())
    {
        for(auto it = offlinev.begin(); it!=offlinev.end(); it++)
        {
            AppendMessage(it->fromName, it->msg);
        }
    }
    offlinev.clear(); // clear offline messages after showing them

    // send by client, recv by server
    NewListen();
    connect(serversocket, SIGNAL(newConnection()),this, SLOT(AcceptConnect()));


    // ServerUdpSocket only listen the fileHead
    ServerUdpSocket->bind(QHostAddress(QString::fromStdString(ip)), (quint16)atoi(port.c_str()));
    connect(ServerUdpSocket, SIGNAL(readyRead()), this, SLOT(ServerReadDatagram()));

     // client need to listen the server's redpond
    clientUdpSocket->bind(QHostAddress(QString::fromStdString(ip)), (quint16)atoi(udpPort.c_str()));
    connect(clientUdpSocket, SIGNAL(readyRead()), this, SLOT(ClientReadDatagram()));

}

Dlog::~Dlog()
{
    delete ui;
    delete clientsocket;
    delete serversocket;
    delete clientUdpSocket;
    delete serverResSocket;
    delete ServerUdpSocket;
    delete recv;
    qDebug("ui, clientsocket, serversocket, clientudpsocket, serverRessocket, serverudpsocket, recv boom");
}


///////////// client send message //////////////
void Dlog::on_sendmsg_clicked()
{
    QString str = ui->input->toPlainText();
    if(str.isEmpty())
    {
        QMessageBox::information(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("输入不能为空！"));
        return;
    }
    else
    {
        string str = "message!" + name + "+" + fri.name + "+" + ui->input->toPlainText().toStdString();
        ClientSendMsg(str);
        AppendMessage(name, ui->input->toPlainText().toStdString());
        ui->input->clear();
    }
}

void Dlog::ClientSendMsg(const string &str)
{
    clientsocket->abort();
    if(fri.isOnline == "true")
    {
        clientsocket->connectToHost(QHostAddress(QString::fromStdString(fri.ip)), atoi(fri.addr.c_str()));
        clientsocket->waitForConnected();
        qDebug("connect to client: ");
        qDebug(fri.name.c_str());
        qDebug(": ");
        qDebug(fri.ip.c_str());
        qDebug(": ");
        qDebug(fri.addr.c_str());

        char sendBuf[BUFLEN];
        strcpy(sendBuf, str.c_str());
        clientsocket->write(sendBuf, strlen(sendBuf)+1);
        clientsocket->waitForBytesWritten();
    }
    else if(fri.isOnline == "false")
    {
        // send offline message to server
        clientsocket->connectToHost(QHostAddress(serverIP), 5050);
        clientsocket->waitForConnected();
        qDebug("connect to server");

        char sendBuf[BUFLEN];
        strcpy(sendBuf, str.c_str());
        clientsocket->write(sendBuf, strlen(sendBuf)+1);
        clientsocket->waitForBytesWritten();
    }
}


/////////   server receive message /////////

void Dlog::NewListen()
{
    bool re = serversocket->listen(QHostAddress::Any, atoi(port.c_str()));
    if(!re)
    {
       qDebug()<<serversocket->errorString();
       serversocket->close();
    }
    else
    {
        qDebug("listen...");
    }
}

void Dlog::AcceptConnect()
{
    serverResSocket = serversocket->nextPendingConnection();
    connect(serverResSocket,SIGNAL(readyRead()),this, SLOT(ServerReceiveMsg()));
}

void Dlog::ServerReceiveMsg()
{
    QByteArray data = serverResSocket->readAll();
    QString tmp(data);
    string str = tmp.toStdString();
    vector<string>v;
    vector<string>v2;
    SplitStr(str, v, "!"); // get rid of "message"
    SplitStr(v[1], v2, "+");
    AppendMessage(v2[0], v2[2]); // fromName and context
}

void Dlog::closeEvent(QCloseEvent *event)
{
    clientsocket->close();
    serversocket->close();
    clientUdpSocket->close();
    serverResSocket->close();
    ServerUdpSocket->close();
    delete clientsocket;
    delete serversocket;
    delete clientUdpSocket;
    delete ServerUdpSocket;
    event->accept();
    qDebug("delete all Dialog sockets");
    return;
}

////////////// client send file ///////////////
void Dlog::on_choosefile_clicked()
{
    ui->showfilename->clear();
    QString filename=QFileDialog::getOpenFileName(this,tr("choose file"),"D:\\");
    ui->showfilename->setText(filename);
    ui->sendfile->setEnabled(true);
    return;
}

void Dlog::on_sendfile_clicked()
{
    isHead = true;
    QString filePath = ui->showfilename->text();
    if(filePath.isEmpty())
    {
        QMessageBox::information(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("请选择文件！"));
        return;
    }
    else
    {
//        ui->progressBar->show();
        file = new QFile(filePath);
        bool isOpen = file->open(QIODevice::ReadOnly);
        if(!isOpen)
        {
            QMessageBox::information(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("无此文件！"));
            return;
        }
        else
        {
            ui->sendfile->setDisabled(true);
            ClientSendDatagram(fri.ip, fri.addr); // just send file head
        }
    }
}

void Dlog::ClientReadDatagram()
{
    while(clientUdpSocket->hasPendingDatagrams())
    {
        QByteArray datagram;
        QHostAddress nullhost;
        quint16 nullport;
        datagram.resize(clientUdpSocket->pendingDatagramSize());
        clientUdpSocket->readDatagram(datagram.data(), datagram.size(), &nullhost, &nullport);

        if(datagram == "OK")
        {
            isError = false;
            ClientSendDatagram(clientIP, clientPort);
        }
        else if(datagram == "REJECT")
        {
            return;
        }
        else if(datagram == "ERROR")
        {
            isError = true;
            ClientSendDatagram(clientIP, clientPort);
        }
        else // recv the server's port
        {
            QString re = datagram;
            vector<string>v;
            SplitStr(re.toStdString(), v, "+");
            clientIP = v[0];
            clientPort = v[1];
            ClientSendDatagram(clientIP, clientPort);
        }
    }
}

void Dlog::ClientSendDatagram(const string &targetIP, const string &targetPort)
{
    QString filePath = ui->showfilename->text();
    QFileInfo *fileInfo = new QFileInfo(filePath);
    if(isHead) // send file head
    {
        QString fileHeadMsg = QString::fromStdString(name) + "?" +
                QString::fromStdString(ip) + "?" +
                QString::fromStdString(udpPort) + "?" +
                fileInfo->fileName() + "?" + QString::number(fileInfo->size());
        // the target port is shared with TCP port
        clientUdpSocket->writeDatagram(fileHeadMsg.toStdString().c_str(), QHostAddress(QString::fromStdString(targetIP)), (quint16)atoi(targetPort.c_str()));
        isHead = false;
    }
    else
    {
        if(!file->atEnd())
        {
            if(!isError) // udp error
            {
                QByteArray msg = file->read(DATASIZE);
                buf.clear();
                QByteArray md5code = QCryptographicHash::hash(msg, QCryptographicHash::Md5);
                QByteArray sendqba;
                QDataStream ds(&sendqba, QIODevice::ReadWrite);
                QDataStream ds__(&buf, QIODevice::ReadWrite);
                ds.setVersion(QDataStream::Qt_5_9);
                ds__.setVersion(QDataStream::Qt_5_9);
                ds << msg << md5code;
                ds__ << msg << md5code;
                int length = clientUdpSocket->writeDatagram(sendqba, QHostAddress(QString::fromStdString(targetIP)), (quint16)atoi(targetPort.c_str()));
//             sendLength += length;
//             ui->progressBar->setValue(sendLength);
//             qDebug() << QString::number(length);
            }
            else
            {
                clientUdpSocket->writeDatagram(buf, QHostAddress(QString::fromStdString(targetIP)), (quint16)atoi(targetPort.c_str()));
            }
        }
        else
        {
            ui->sendfile->setEnabled(true);
            clientUdpSocket->writeDatagram( "OVER", 4, QHostAddress(QString::fromStdString(targetIP)), (quint16)atoi(targetPort.c_str()));
            QMessageBox::information(this, QString::fromLocal8Bit("完成"), QString::fromLocal8Bit("文件传输成功！"));
            sendLength = 0;
//            ui->progressBar->hide();
            file->close();
            delete file;
            return;
        }
    }
}

////////////// server receive file ///////////////
void Dlog::ServerReadDatagram()
{
    while(ServerUdpSocket->hasPendingDatagrams())
    {
        QByteArray datagram;
        QHostAddress nullhost;
        quint16 nullport;
        datagram.resize(ServerUdpSocket->pendingDatagramSize());
        ServerUdpSocket->readDatagram(datagram.data(), datagram.size(), &nullhost, &nullport);
        QString s = datagram.data();
        string str = s.toStdString();
        vector<string>v;
        SplitStr(str, v, "?");
        // v[0] fromName, v[1]: fromIP, v[2]: fromPort, v[3] fileName, v[4] = fileSize
        rfInfo.fromName = v[0];
        rfInfo.fromIP = v[1];
        rfInfo.fromPort = v[2];
        rfInfo.fileName = v[3];
        rfInfo.fileSize = v[4];
        recv = new ReceiveFile();
        recv->show();
    }
}
///////// adding functions //////////

void Dlog::AppendMessage(const string & nm, const string &message)
{
    // append message from user
    QTextCursor cursor(ui->msg->textCursor());
    cursor.movePosition(QTextCursor::End);
    QTextTable *table = cursor.insertTable(1, 2, tableFormat);
    table->cellAt(0, 0).firstCursorPosition().insertText("From ");
    table->cellAt(0, 1).firstCursorPosition().insertText(QString::fromStdString(nm) + ": " + QString::fromStdString(message) + '\n');
    QScrollBar *bar = ui->msg->verticalScrollBar();
    bar->setValue(bar->maximum());
}

void SplitStr(const string& s, vector<string>& v, const string& c)
{
    string::size_type pos1, pos2;
    pos2 = s.find(c);
    pos1 = 0;
    while (string::npos != pos2)
    {
        v.push_back(s.substr(pos1, pos2 - pos1));

        pos1 = pos2 + c.size();
        pos2 = s.find(c, pos1);
    }
    if (pos1 != s.length())
        v.push_back(s.substr(pos1));
}

void GenerateUdpPort()
{
    srand((unsigned)time(0));
    int randomport = rand() % (MAXUDPPORTNUM - MINUDPPORTNUM + 1) + MINUDPPORTNUM;
    std::stringstream ss;
    ss << randomport;
    ss >> udpPort;
}
