#ifndef DLOG_H
#define DLOG_H

#include <QWidget>
#include <QWidget>
#include <QTextTableFormat>
#include <QCloseEvent>
#include <QMessageBox>
#include <QString>
#include <QFile>
#include <QFileDialog>
#include <string>
#include <vector>
#include <QTextCursor>
#include <QTextTable>
#include <QTimer>
#include <QScrollBar>
#include <QListWidgetItem>
#include <cstdlib>
#include <QTcpSocket>
#include <QTcpServer>
#include <QUdpSocket>
using std::vector;
using std::string;

namespace Ui {
class Dlog;
}

class Dlog : public QWidget
{
    Q_OBJECT

public:
    explicit Dlog(QWidget *parent = 0);
    void NewConnect(); //创建新的连接
    void NewListen();
    void ClientSendMsg(const string &str);
    void AppendMessage(const string & nm, const string &message);
    ~Dlog();

private slots:
    void on_choosefile_clicked();
    void on_sendfile_clicked();
    void on_sendmsg_clicked();
    void AcceptConnect();
public slots:
    void ServerReceiveMsg();
private:
    Ui::Dlog *ui;
    QTextTableFormat tableFormat;
    QTcpSocket *clientsocket;
    QTcpServer *serversocket;
    QTcpSocket *serverResSocket;
    QUdpSocket *udpsocket;
};

#endif // DLOG_H
