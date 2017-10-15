#ifndef GETPASSWDBACK_H
#define GETPASSWDBACK_H

#include <QDialog>

namespace Ui {
class GetPasswdBack;
}

class GetPasswdBack : public QDialog
{
    Q_OBJECT

public:
    explicit GetPasswdBack(QWidget *parent = 0);
    ~GetPasswdBack();

private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::GetPasswdBack *ui;
};

#endif // GETPASSWDBACK_H
