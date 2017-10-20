#include "dlog.h"
#include "ui_dlog.h"
#include <QTcpSocket>
#include <QTcpServer>
#include <QByteArray>
#include <QDebug>
const int BUFLEN = 255;
typedef struct
{
    string name;
    string addr;
    string isOnline;
}Record;
extern string name;
extern string port;
extern Record fri;

void SplitStr(const string& s, vector<string>& v, const string& c);

Dlog::Dlog(QWidget *parent) : QWidget(parent), ui(new Ui::Dlog)
{
    ui->setupUi(this);
    ui->msg->setReadOnly(true); // set show message window read-only
    tableFormat.setBorder(0);
    ui->userName->setText("From: " + QString::fromStdString(name));
    ui->sendto->clear();
    ui->sendto->setText("To: "+QString::fromStdString(fri.name));
    clientsocket = new QTcpSocket(this);
    serversocket = new QTcpServer(this);
    udpsocket = new QUdpSocket(this);
    serverResSocket = new QTcpSocket(this);

    // send by client, recv by server
    connect(serversocket,SIGNAL(newConnection()),SLOT(AcceptConnect()));
    // an error occurred
    connect(serversocket,SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(close()));
    NewConnect();
    NewListen();
}

Dlog::~Dlog()
{
    delete ui;
    delete clientsocket;
    delete serversocket;
    delete udpsocket;
    delete serverResSocket;
}

void Dlog::on_choosefile_clicked()
{
    ui->showfilename->clear();
    QString filename=QFileDialog::getOpenFileName(this,tr("choose file"),"D:\\");
    ui->showfilename->setText(filename);

    return;
}

void Dlog::on_sendfile_clicked()
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
        string str = "message!" + name + "+" + ui->input->toPlainText().toStdString();
        ClientSendMsg(str);
        AppendMessage(name, ui->input->toPlainText().toStdString());
        ui->input->clear();
    }
}

///////////// client //////////////

void Dlog::NewConnect()
{
    clientsocket->abort();
    if(fri.isOnline == "true")
    {
        clientsocket->connectToHost("127.0.0.1", atoi(fri.addr.c_str()));
        qDebug("connect to client: ");
        qDebug(fri.name.c_str());
        qDebug(": ");
        qDebug(fri.addr.c_str());
    }
    else if(fri.isOnline == "false")
    {
        // send offline message to server
        clientsocket->connectToHost("127.0.0.1", 5050);
        qDebug("connect to server");
    }
    clientsocket->waitForConnected();
}

void Dlog::ClientSendMsg(const string &str)
{
    char sendBuf[BUFLEN];
    strcpy(sendBuf, str.c_str());
    clientsocket->write(sendBuf, strlen(sendBuf)+1);
    clientsocket->waitForBytesWritten();
}


///////   server /////////

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
    connect(serverResSocket,SIGNAL(readyRead()),SLOT(ServerReceiveMsg()));
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
    AppendMessage(v2[0], v2[1]);
}

///////// other //////////

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
