#include "getpasswdback.h"
#include "ui_getpasswdback.h"
#include "sendtoserver.h"
#include <QMessageBox>
#include <QString>

GetPasswdBack::GetPasswdBack(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GetPasswdBack)
{
    ui->setupUi(this);
    ui->newpd->setEchoMode(QLineEdit::Password);
}

GetPasswdBack::~GetPasswdBack()
{
    delete ui;
}

void GetPasswdBack::on_buttonBox_accepted()
{
    QString str1 = ui->email->text();
    QString str2 = ui->newpd->text();
    if(str1.isEmpty() || str2.isEmpty())
    {
        QMessageBox::information(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("信息不完整！"));
        return;
    }
    else if(SendtoServer(str1.toStdString()+"+"+str2.toStdString(), "getpasswdback"))
    {
        QMessageBox::information(this, QString::fromLocal8Bit("成功"), QString::fromLocal8Bit("密码重设成功！"));
        return;
    }
    else
    {
        QMessageBox::information(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("密码重设失败！"));
        return;
    }
}

void GetPasswdBack::on_buttonBox_rejected()
{
    ui->email->clear();
    ui->newpd->clear();

    return;
}
