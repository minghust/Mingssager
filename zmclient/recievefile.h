#ifndef RECIEVEFILE_H
#define RECIEVEFILE_H

#include <QDialog>

namespace Ui {
class RecieveFile;
}

class RecieveFile : public QDialog
{
    Q_OBJECT

public:
    explicit RecieveFile(QWidget *parent = 0);
    ~RecieveFile();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::RecieveFile *ui;
};

#endif // RECIEVEFILE_H
