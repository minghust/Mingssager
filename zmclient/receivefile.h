#ifndef RECEIVEFILE_H
#define RECEIVEFILE_H

#include <QDialog>
#include <QFile>
#include <QFileDialog>

namespace Ui {
class ReceiveFile;
}

class ReceiveFile : public QDialog
{
    Q_OBJECT

public:
    explicit ReceiveFile(QWidget *parent = 0);
    void Initial();
    void ServerReceiveFile();
    QFile file;
    ~ReceiveFile();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

    void on_store_clicked();

private:
    Ui::ReceiveFile *ui;
    QFileDialog *fDialog;
};

#endif // RECEIVEFILE_H
