#include "receivefile.h"
#include "ui_receivefile.h"
#include "dlog.h"

typedef struct
{
    string fromName;
    string fromPort;
    string fileSize;
    string fileName;
}RFInfo; // receive file info

extern RFInfo rfInfo;
extern string port;
ReceiveFile::ReceiveFile(QWidget *parent) : QDialog(parent), ui(new Ui::ReceiveFile)
{
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
    ui->fileSize->setText(QString::fromStdString(rfInfo.fileSize));

    udpsocket = new QUdpSocket(this);
    udpsocket->bind(QHostAddress::LocalHost, quint16(atoi(port.c_str())));
    connect(udpsocket, SIGNAL(readyRead()), this, SLOT(ServerReceiveFile));
}

ReceiveFile::~ReceiveFile()
{
    delete ui;
    delete fDialog;
    delete udpsocket;
}

void ReceiveFile::on_buttonBox_accepted()
{
    if(ui->storePos->text().isEmpty())
    {
        QMessageBox::warning(this, "PATH", "the path is NULL");
        return;
    }
    // send an "OK" to respond has received the fileHeadInfo and agree to receive file.
    udpsocket->writeDatagram("OK", 2, QHostAddress::LocalHost, (quint16)atoi(rfInfo.fromPort.c_str()));
    file.setFileName(ui->storePos->text());
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
        QByteArray datagram;
        QHostAddress host;
        quint16 port;
        datagram.resize(udpsocket->pendingDatagramSize());
        int length = udpsocket->readDatagram(datagram.data(), datagram.size(), &host, &port);

        if(datagram == "OVER")
        {
            recvLength = 0;
            ui->progressBar->hide();
            QMessageBox::information(this, "success", "Recieve File Success");
            emit DialogClose();
            this->close();
            return;
        }
        recvLength += length;
        ui->progressBar->setValue(recvLength);
        file.write(datagram.data(), datagram.size());
        udpsocket->writeDatagram("OK", 2, QHostAddress::LocalHost, (quint16)atoi(rfInfo.fromPort.c_str()));
    }
}

void ReceiveFile::on_buttonBox_rejected()
{
    udpsocket->writeDatagram("REJECT", 6, QHostAddress::LocalHost, (quint16)atoi(rfInfo.fromPort.c_str()));
    emit DialogClose();
    this->close();
    return;
}

void ReceiveFile::on_store_clicked()
{
    fDialog = new QFileDialog(this);
    fDialog->setFileMode(QFileDialog::Directory);
    fDialog->show();
    connect(fDialog,SIGNAL(fileSelected ( const QString & )),this,SLOT(SetFileName(const QString & )));
}

void ReceiveFile::SetFileName(const QString & dpath)
{
    ui->storePos->setText(dpath+"/"+QString::fromStdString(rfInfo.fileName));
}
