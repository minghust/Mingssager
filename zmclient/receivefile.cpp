#include "receivefile.h"
#include "ui_receivefile.h"
#include "dlog.h"
#include <ctime>
#include <sstream>
#include <QDataStream>
#include <QStringList>
#include <QCryptographicHash>
typedef struct
{
    string fromName;
    string fromIP;
    string fromPort;
    string fileSize;
    string fileName;
}RFInfo; // receive file info

extern RFInfo rfInfo;
extern string ip;
string recvPort = "";

void GeneUdpPort();
const int MIN = 58000;
const int MAX = 60000;
const int TIMEOUT = 4000;
ReceiveFile::ReceiveFile(QWidget *parent) : QDialog(parent), ui(new Ui::ReceiveFile)
{
    GeneUdpPort();
    ui->setupUi(this);
    ui->fileName->setReadOnly(true);
    ui->fromName->setReadOnly(true);
    ui->fileSize->setReadOnly(true);
    ui->storePos->setReadOnly(true);

    ui->fileName->clear();
    ui->fromName->clear();
    ui->fileSize->clear();
    ui->storePos->clear();
    ui->progressBar->hide();
    recvLength = 0;

    ui->fromName->setText(QString::fromStdString(rfInfo.fromName));
    ui->fileName->setText(QString::fromStdString(rfInfo.fileName));
    ui->fileSize->setText(QString::fromStdString(rfInfo.fileSize)+" bytes");

    udpsocket = new QUdpSocket(this);
    udpsocket->bind(QHostAddress(QString::fromStdString(ip)), quint16(atoi(recvPort.c_str())));
    connect(udpsocket, SIGNAL(readyRead()), this, SLOT(ServerReceiveFile()));

    time = new QTimer(this);
    connect(time, SIGNAL(timeout()), this, SLOT(ReSend()));
}

ReceiveFile::~ReceiveFile()
{
    delete ui;
    delete fDialog;
    delete udpsocket;
    delete time;
}

void ReceiveFile::on_buttonBox_accepted()
{
    if(ui->storePos->text().isEmpty())
    {
        QMessageBox::warning(this, "PATH", "the path is NULL");
        return;
    }
    // send receive port to respond has received the fileHeadInfo and agree to receive file body.
    string sendmsg = ip + "+" + recvPort;
    udpsocket->writeDatagram(sendmsg.c_str(), strlen(sendmsg.c_str()) + 1, QHostAddress(QString::fromStdString(rfInfo.fromIP)), (quint16)atoi(rfInfo.fromPort.c_str()));
    file.setFileName(ui->storePos->text());
//    ui->progressBar->show();
    if(!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Unbuffered))
    {
        QMessageBox::warning(this, "create", "create file error");
        return;
    }
}

void ReceiveFile::ServerReceiveFile()
{
    while(udpsocket->hasPendingDatagrams())
    {
        time->stop();
        QByteArray datagram;
        QHostAddress host;
        quint16 port;
        datagram.resize(udpsocket->pendingDatagramSize());
        udpsocket->readDatagram(datagram.data(), datagram.size(), &host, &port);
        if(datagram == "OVER")
        {
            recvLength = 0;
//            ui->progressBar->hide();
            QMessageBox::information(this, QString::fromLocal8Bit("成功"), QString::fromLocal8Bit("接收文件成功！"));
            udpsocket->close();
            file.close();
            time->stop();
            this->close();
            return;
        }
        QDataStream ds(&datagram, QIODevice::ReadWrite);
        ds.setVersion(QDataStream::Qt_5_9);
        QByteArray msg;
        QByteArray md5;
        ds >> msg >> md5;
        QByteArray md5code = QCryptographicHash::hash(msg, QCryptographicHash::Md5);
        if(md5code == md5)
        {
            file.write(msg);
            udpsocket->writeDatagram("OK", 2, QHostAddress(QString::fromStdString(rfInfo.fromIP)), (quint16)atoi(rfInfo.fromPort.c_str()));
            time->start(TIMEOUT);
        }
        else
        {
            udpsocket->writeDatagram("ERROR", 5, QHostAddress(QString::fromStdString(rfInfo.fromIP)), (quint16)atoi(rfInfo.fromPort.c_str()));
            time->start(TIMEOUT);
        }
    }
}

void ReceiveFile::ReSend()
{
    udpsocket->writeDatagram("ERROR", 5, QHostAddress(QString::fromStdString(rfInfo.fromIP)), (quint16)atoi(rfInfo.fromPort.c_str()));
    time->start(TIMEOUT);
}

void ReceiveFile::on_buttonBox_rejected()
{
    udpsocket->writeDatagram("REJECT", 6, QHostAddress(QString::fromStdString(rfInfo.fromIP)), (quint16)atoi(rfInfo.fromPort.c_str()));
    udpsocket->close();
    this->close();
    return;
}

void ReceiveFile::on_store_clicked()
{
    fDialog = new QFileDialog(this);
    fDialog->setFileMode(QFileDialog::Directory);
    fDialog->setDirectory("D:\\");
    fDialog->show();
    connect(fDialog,SIGNAL(fileSelected ( const QString & )),this,SLOT(SetFileName(const QString & )));
}

void ReceiveFile::SetFileName(const QString & dpath)
{
    ui->storePos->setText(dpath+"/"+QString::fromStdString(rfInfo.fileName));
}

void GeneUdpPort()
{
    srand((unsigned)time(0));
    int randomport = rand() % (MAX - MIN + 1) + MIN;
    std::stringstream ss;
    ss << randomport;
    ss >> recvPort;
}
