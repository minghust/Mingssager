#ifndef RECEIVEFILE_H
#define RECEIVEFILE_H

#include <QDialog>

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

    void on_buttonBox_rejected();

private:
    Ui::ReceiveFile *ui;
};

#endif // RECEIVEFILE_H
