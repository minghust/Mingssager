#include "dlog.h"
#include "ui_dlog.h"
#include "receivefile.h"

const int BUFLEN = 500;
typedef struct
{
    string name;
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
    string fromPort;
    string fileSize;
    string fileName;
}RFInfo; // receive file info

extern string name;
extern string port;
extern Record fri;
extern vector<OfflineMessage>offlinev;

RFInfo rfInfo = {"", "", "", ""};
void SplitStr(const string& s, vector<string>& v, const string& c);

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
    isHead = true;
    clientsocket = new QTcpSocket(this);
    serversocket = new QTcpServer(this);
    clientUdpSocket = new QUdpSocket(this);
    ServerUdpSocket = new QUdpSocket(this);
    serverResSocket = new QTcpSocket(this);

    if(!offlinev.empty())
    {
        for(auto it = offlinev.begin(); it!=offlinev.end(); it++)
        {
            AppendMessage(it->fromName, it->msg);
        }
    }

    // send by client, recv by server
    connect(serversocket, SIGNAL(newConnection()),this, SLOT(AcceptConnect()));
    // an error occurred
    connect(serversocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(close()));

    NewListen();
    ServerUdpSocket->bind(QHostAddress::LocalHost, (quint16)atoi(port.c_str()));
    connect(ServerUdpSocket, SIGNAL(readyRead()), this, SLOT(ServerReadDatagram()));
}

Dlog::~Dlog()
{
    delete ui;
    delete clientsocket;
    delete serversocket;
    delete clientUdpSocket;
    delete serverResSocket;
    delete ServerUdpSocket;
}


///////////// client send message //////////////
void Dlog::on_sendmsg_clicked()
{
    QString str = ui->input->toPlainText();
    if(str.isEmpty())
    {
        QMessageBox::information(this, QString::fromLocal8Bit("´íÎó"), QString::fromLocal8Bit("ÊäÈë²»ÄÜÎª¿Õ£¡"));
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
        clientsocket->connectToHost("127.0.0.1", atoi(fri.addr.c_str()));
        clientsocket->waitForConnected();
        qDebug("connect to client: ");
        qDebug(fri.name.c_str());
        qDebug(": ");
        qDebug(fri.addr.c_str());
    }
    else if(fri.isOnline == "false")
    {
        // send offline message to server
        clientsocket->connectToHost("127.0.0.1", 5050);
        clientsocket->waitForConnected();
        qDebug("connect to server");
    }

    char sendBuf[BUFLEN];
    strcpy(sendBuf, str.c_str());
    clientsocket->write(sendBuf, strlen(sendBuf)+1);
    clientsocket->waitForBytesWritten();
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
        qDebug("listen");
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

//////////// client send file ///////////////
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
    ui->progressBar->show();
    isHead = true;
    QString filePath = ui->showfilename->text();
    if(filePath.isEmpty())
    {
        QMessageBox::information(this, QString::fromLocal8Bit("´íÎó"), QString::fromLocal8Bit("ÇëÑ¡ÔñÎÄ¼þ£¡"));
        return;
    }
    else
    {
        file = new QFile(filePath);

        bool isOpen = file->open(QIODevice::Readonly);
        if(!isOpen)
        {
            QMessageBox::information(this, QString::fromLocal8Bit("´íÎó"), QString::fromLocal8Bit("ÎÞ´ËÎÄ¼þ£¡"));
            return;
        }
        else
        {
            ui->sendfile->setDisabled(true);
            ServerUdpSocket->close();
            clientUdpSocket->bind("127.0.0.1", port); // client need to listen the server's redpond
            connect(clientUdpSocket, SIGNAL(readyRead()), this, SLOT(ClientReadDatagram()));
            ClientSendDatagram(fri.addr); // just send file head
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

        qDebug() << datagram;
        if(datagram == "OK")
        {
            qDebug() << "send ok";
            ClientSendDatagram(fri.addr);
        }
        else if(datagram == "REJECT")
        {
            qDebug() << "send reject";
            return;
        }
    }
}

void Dlog::ClientSendDatagram(const string &targetPort)
{
    if(isHead) // send file head
    {
        QString filePath = ui->showfilename->text();
        QFileInfo *fileInfo = new QFileInfo(filePath);
        QByteArray fileHeadMsg = name + "?" + port + "?" + fileInfo->fileName() + "?" + QString::number(fileInfo->size());
        clientUdpSocket->writeDatagram(fileHeadMsg.toUtf8(),QHostAddress("127.0.0.1"), targetPort);
        isHead = false;
    }
    else
    {
        if(!file->atEnd())
        {
            QByteArray msg;
            msg = file->read(512);
            int length = clientUdpSocket->writeDatagram( msg, QHostAddress("127.0.0.1"), targetPort);
            sendLength += length;
            ui->progressBar->setValue(sendLength);
            qDebug() << QString::number(length);
        }
        else
        {
            ui->sendfile->setEnabled(true);
            clientUdpSocket->writeDatagram( "OVER", 4, QHostAddress("127.0.0.1"), targetPort);
            QMessageBox::information(this, QString::fromLocal8Bit("Íê³É"), QString::fromLocal8Bit("ÎÄ¼þ´«Êä³É¹¦£¡"));
            sendLength = 0;
            ui->progressBar->hide();
            file->close();
            clientsocket->close();
            ServerUdpSocket->bind("127.0.0.1", port); // reBind self-port for listen other's file transmission
            delete file;
            return;
        }
    }
}

//////////// server receive file ///////////////
void Dlog::ServerReadDatagram()
{
    ReceiveFile *recvFile = new ReceiveFile();
    recvFile->show();

//    QByteArray datagram;
//    QHostAddress nullhost;
//    qunit16 nullport;
//    ServerUdpSocket->readDatagram(datagram.data(), datagram.size(), &nullhost, &nullport);
//    QString s = datagram.data();
//    string str = s.toStdString();
//    vector<string>v;
//    SplitStr(str, v, "?");

}



///////// adding functions //////////

void Dlog::AppendMessage(const string & nm, const string &message)
{
    // append message from user
    QTextCursor cursor(ui->msg->textCursor());
    cursor.movePosition(QTextCursor::End);
    QTextTable *table = cursor.insertTable(1, 2, tableFormat);
    table->cellAt(0, 0).firstCursorPosition().insertText("<127.0.0.1>");
    table->cellAt(0, 1).firstCursorPosition().insertText(QString::fromStdString(nm) + "said: " + QString::fromStdString(message) + '\n');
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
