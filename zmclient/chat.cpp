#include "chat.h"
#include "ui_chat.h"
#include "updatethread.h"
#include <QTcpSocket>
using std::string;
using std::vector;

const int BUFLEN = 255;

typedef struct
{
    string name;
    string addr;
    string isOnline;
}Record;

// One record format: "<name>+<addr_port>+<isOnline>!"
extern string userList;
extern string name;
extern string port;
void SplitString(const string& s, vector<string>& v, const string& c);

Chat::Chat(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Chat)
{
    ui->setupUi(this);
    ui->userName->setText(QString::fromStdString(name));
    is_open = true;
    ui->msg->setReadOnly(true); // set show message window read-only
    tableFormat.setBorder(0);
//    QTimer *timer = new QTimer(this);
//    timer->start(3000);
    // update userlist
//    connect(timer, SIGNAL(timeout()), this, SLOT(ShowUserList(userList)));
    // start listen
    ShowUserList(userList);
    startThread();
}

Chat::~Chat()
{
    delete ui;
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
    for(int i=0; i<recd.size(); i++)
    {
        ui->friendsList->addItem(QString::fromStdString(recd[i].name));
        ui->isOnlineList->addItem(QString::fromStdString(recd[i].isOnline));
    }
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
    clientsocket->waitForDisconnected();
    event->accept();
    return;
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

// start thread
void startThread()
{
    WorkerThread *workerThread = new WorkerThread(this);
    connect(workerThread, SIGNAL(resultReady()), this, SLOT(handleResults()));
    connect(workerThread, SIGNAL(finished()), workerThread, SLOT(deleteLater()));
    workerThread->start();
}
// update userList
void handleResults()
{
    // close the window
    QTcpSocket *clientsocket = new QTcpSocket();
    clientsocket->connectToHost("127.0.0.1", 5050);
    clientsocket->waitForConnected();
    char recvBuf[BUFLEN];
    char sendBuf[BUFLEN];
    string s = "update!" + name + "+" + port;

    clientsocket->write(sendBuf, strlen(sendBuf)+1);
    clientsocket->waitForBytesWritten();

    // receive userlist
    clientsocket->waitForReadyRead();
    clientsocket->read(recvBuf, BUFLEN);

    userList.clear();
    userList = recvBuf;

    clientsocket->disconnectFromHost();
    clientsocket->waitForDisconnected();

    ShowUserList(userList);
}
