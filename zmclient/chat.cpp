#include "chat.h"
#include "ui_chat.h"
#include "dlog.h"
#include <QTcpSocket>
#include <QMessageBox>

using std::string;
using std::vector;

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

Record fri = {"", "", ""};

// One record format: "<name>+<addr_port>+<isOnline>!"
extern string userList;
extern string name;
extern string port;
extern vector<OfflineMessage>offlinev;

void SplitString(const string& s, vector<string>& v, const string& c);

Chat::Chat(QWidget *parent) : QWidget(parent), ui(new Ui::Chat)
{
    ui->setupUi(this);
    ui->userName->setText(QString::fromStdString(name));
    ShowUserList(userList);
    // double click to get user to chat with
    connect(ui->friendsList, SIGNAL(itemDoubleClicked(QListWidgetItem *)), this, SLOT(chooseItem(QListWidgetItem *)));
    if(!offlinev.empty())
    {
        string frname="";
        for(auto it = offlinev.begin(); it!=offlinev.end(); it++)
        {
            frname = frname + it->fromName+" ";
        }
        QMessageBox::information(this, QString::fromLocal8Bit("好友的离线消息"), QString::fromStdString(frname));
    }
}

Chat::~Chat()
{
    delete ui;
}

void Chat::chooseItem(QListWidgetItem *item)
{
    vector<string>v;
    SplitString(item->text().toStdString(), v, " ");
    fri.name = v[0];
    fri.addr = v[1];
    fri.isOnline = v[2];
    Dlog *dl = new Dlog();
    dl->show();
}

void Chat::closeEvent(QCloseEvent *event)
{
    // close the window
    QTcpSocket *clientsocket = new QTcpSocket();
    clientsocket->connectToHost("127.0.0.1", 5050);
    clientsocket->waitForConnected();

    string s = "offline!" + name;
    char sendBuf[BUFLEN];
    strcpy(sendBuf, s.c_str());
    clientsocket->write(sendBuf, strlen(sendBuf)+1);
    clientsocket->waitForBytesWritten();

    clientsocket->disconnectFromHost();
    event->accept();
    return;
}

void Chat::ShowUserList(string str)
{
    if(str.empty())
        return;
    vector<string>name_addr_isOnline;
    vector<Record>recd; // a vector of user record(name addr isOnline)
    SplitString(str, name_addr_isOnline, "!");
    for(auto it=name_addr_isOnline.begin(); it!=name_addr_isOnline.end(); it++)
    {
        vector<string>v;
        SplitString(*it, v, "+");
        Record tmp = {v[0], v[1], v[2]};
        recd.push_back(tmp);
    }
    // show recd in ui->friendsList
    ui->friendsList->clear();
    for(unsigned i=0; i<recd.size(); i++)
    {
        ui->friendsList->addItem(QString::fromStdString(recd[i].name) + " " + QString::fromStdString(recd[i].addr)
                                 + " " + QString::fromStdString(recd[i].isOnline));
    }
}

void SplitString(const string& s, vector<string>& v, const string& c)
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

void Chat::on_updateList_clicked()
{
    QTcpSocket *clientsocket = new QTcpSocket(this);
    clientsocket->connectToHost("127.0.0.1", 5050);
    clientsocket->waitForConnected();
    char recvBuf[BUFLEN];
    char sendBuf[BUFLEN];
    memset(recvBuf, '\0', BUFLEN);
    string s = "update!" + name + "+" + port;
    strcpy(sendBuf, s.c_str());

    clientsocket->write(sendBuf, strlen(sendBuf)+1);
    clientsocket->waitForBytesWritten();

    // receive userlist
    clientsocket->waitForReadyRead();
    clientsocket->read(recvBuf, BUFLEN);

    userList.clear();
    userList = recvBuf;

    clientsocket->disconnectFromHost();
//    clientsocket->waitForDisconnected();

    ShowUserList(userList);
}
