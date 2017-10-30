#ifndef DLOG_H
#define DLOG_H

#include <QWidget>
#include <QTextTableFormat>
#include <QCloseEvent>
#include <QMessageBox>
#include <QString>
#include <QTextCursor>
#include <QTextTable>
#include <QTimer>
#include <QScrollBar>
#include <QListWidgetItem>
#include <QTcpSocket>
#include <QTcpServer>
#include <QUdpSocket>
#include <QByteArray>
#include <QDebug>
#include <QProgressBar>
#include <QFileInfo>
#include <QFile>
#include <QFileDialog>
#include <QDataStream>

#include <string>
#include <vector>
#include <cstdlib>
#include "receivefile.h"
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
    void NewListen();
    void ClientSendMsg(const string &str);
    void AppendMessage(const string & nm, const string &message);
    void ClientSendDatagram(const string &targetPort);
    QUdpSocket *ServerUdpSocket;
    ~Dlog();

private slots:
    void on_sendmsg_clicked();
    void AcceptConnect();
    void ServerReceiveMsg();
    void ServerReadDatagram(); // to delete
    void ClientReadDatagram();
    void on_choosefile_clicked();
    void on_sendfile_clicked();
protected:
    void closeEvent(QCloseEvent *event) override;


private:
    Ui::Dlog *ui;
    QTextTableFormat tableFormat;
    QTcpSocket *clientsocket;
    QTcpServer *serversocket;
    QTcpSocket *serverResSocket;
    QUdpSocket *clientUdpSocket;
    bool isHead;
    QFile *file;
    int sendLength;
    ReceiveFile * recv;
    QDataStream *in;
};

#endif // DLOG_H
