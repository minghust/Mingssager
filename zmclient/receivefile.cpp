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

RFInfo rfInfo;

ReceiveFile::ReceiveFile(QWidget *parent) : QDialog(parent), ui(new Ui::ReceiveFile)
{
    ui->setupUi(this);
    ui->fileName->setReadOnly(true);
    ui->fromName->setReadOnly(true);
    ui->fileSize->setReadOnly(true);
    ui->storePos->setReadOnly(true);
    Initial();
}

void ReceiveFile::Initial()
{
    ui->fileName->clear();
    ui->fromName->clear();
    ui->fileSize->clear();
    ui->storePos->clear();
    fromPort = "";
    QByteArray datagram;
    QHostAddress nullhost;
    qunit16 nullport;
    Dlog::ServerUdpSocket->readDatagram(datagram.data(), datagram.size(), &nullhost, &nullport);
    QString s = datagram.data();
    string str = s.toStdString();
    vector<string>v;
    SplitStr(str, v, "?");
    // v[0] fromName, v[1]: fromPort, v[2]: fileName, v[3] fileSize
    ui->filename = "from: "+ QString::fromStdString(v[1]) + '\n' + "fileName:" + QString::fromStdString(v[2]) + "\fileSsize: " + QString::fromStdString(v[1]);
    rfInfo.fromName = v[0];
    rfInfo.fromPort = v[1];
    rfInfo.fileName = v[2];
    rfInfo.fileSize = v[3];

    fDialog = new QFileDialog(this);
    fDialog->setFileMode(QFileDialog::Directory);


}

ReceiveFile::~ReceiveFile()
{
    delete ui;
}

void ReceiveFile::on_buttonBox_accepted()
{
    if(ui->storePos->text().isEmpty())
    {
        QMessageBox::warning(this, "PATH", "the path is NULL");
        return;
    }
    Dlog::ServerUdpSocket->writeDatagram("OK", 2, "127.0.0.1", rfInfo.fromPort);

    connect(ServerUdpSocket, SIGNAL(readyRead()),this, SLOT(ServerReceiveFile()));
    file.setFileName(ui->storePos->text());
    if(!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Unbuffered))
    {
        QMessageBox::warning(this, "create", "create file error");
        return;
    }
}

void ReceiveFile::ServerReceiveFile()
{
    while(udpSocket->hasPendingDatagrams())
    {
        QByteArray datagram;
        QHostAddress host;
        quint16 port;
        datagram.resize(udpSocket->pendingDatagramSize());
        int length = udpSocket->readDatagram(datagram.data(), datagram.size(), &host, &port);

        if(isFileHead){
            QStringList msgs = QString(datagram).split("##");
            qDebug() << msgs;
            if(msgs.count() != 2){
                QMessageBox::warning(this, "Error", "Receive FileHead error");
            }
            QString fileLength = msgs.at(1);
            int fileSize = fileLength.toInt();
            ui->progressBar->setMaximum(fileSize);
            isFileHead = false;
            udpSocket->writeDatagram("OK", 2, host, 7754);
            continue;
        }
        if(datagram == "OVER"){
            QMessageBox::information(this, "success", "Recieve File Success");
            ui->pushButton_recv->setEnabled(true);
            return;
        }
        recvCnt += length;
        ui->progressBar->setValue(recvCnt);
        file.write(datagram.data(),datagram.size());
        udpSocket->writeDatagram("OK", 2, host, 7754);
        ui->listWidget->addItem(QString(datagram));
         ui->listWidget->scrollToBottom();
      //  qDebug() << datagram << " " << host.toString() << QString::number(port);
    }
}

void ReceiveFile::on_buttonBox_rejected()
{
    Dlog::ServerUdpSocket->writeDatagram("REJECT", 6, "127.0.0.1", rfInfo.fromPort);
    return;
}

void ReceiveFile::on_store_clicked()
{
    QString dpath = QFileDialog::getOpenFileName(this);
    QString path = dpath+"\\"+QString::fromStdString(rfInfo.fileName);
    ui->storePos->setText(path);
    QFileInfo fileInfo = QFileInfo(path);

}

