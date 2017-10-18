#include "register.h"
#include "ui_register.h"
#include "sendtoserver.h"
#include <QMessageBox>
#include <QString>

Register::Register(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Register)
{
    ui->setupUi(this);
    ui->regPasswd->setEchoMode(QLineEdit::Password);
}

Register::~Register()
{
    delete ui;
}

void Register::on_buttonBox_accepted()
{
    QString str1 = ui->regUserName->text();
    QString str2 = ui->regEmail->text();
    QString str3 = ui->regPasswd->text();
    if(str1.isEmpty() || str2.isEmpty() || str3.isEmpty())
    {
        QMessageBox::information(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("注册信息不完整！"));
        return;
    }
    else
    {
        int re = Send::SendtoServer(str1.toStdString()+"+"+str2.toStdString()+"+"+str3.toStdString(), "register");
        if(re == REGISTER_SUCCESS)
        {
            QMessageBox::information(this, QString::fromLocal8Bit("成功"), QString::fromLocal8Bit("注册完成！"));
            return;
        }
        else if(re == REGISTER_FAILURE)
        {
            QMessageBox::information(this, QString::fromLocal8Bit("错误"), QString::fromLocal8Bit("注册失败！"));
            return;
        }
    }
}

void Register::on_buttonBox_rejected()
{
    ui->regUserName->text().clear();
    ui->regEmail->text().clear();
    ui->regPasswd->text().clear();

    return;
}
