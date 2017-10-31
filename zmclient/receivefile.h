#ifndef RECEIVEFILE_H
#define RECEIVEFILE_H

#include <QDialog>
#include <QFile>
#include <QFileDialog>
#include <QUdpSocket>
#include <QString>
#include <QByteArray>
#include <QProgressBar>
#include <vector>
#include <string>
#include <QMessageBox>
#include <QHostAddress>
#include <QDataStream>

#include <cstdlib> // atoi()
namespace Ui {
class ReceiveFile;
}

class ReceiveFile : public QDialog
{
    Q_OBJECT

public:
    explicit ReceiveFile(QWidget *parent = 0);
    ~ReceiveFile();

private slots:
    void on_buttonBox_accepted();
    void ServerReceiveFile();
    void on_buttonBox_rejected();

    void on_store_clicked();
    void SetFileName(const QString & dpath);
private:
    Ui::ReceiveFile *ui;
    QFileDialog *fDialog;
    int recvLength;
    QUdpSocket *udpsocket;
    QFile file;
};

#endif // RECEIVEFILE_H
