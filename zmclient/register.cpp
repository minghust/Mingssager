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
        QMessageBox::information(this, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("ע����Ϣ��������"));
        return;
    }
    else
    {
        int re = Send::SendtoServer(str1.toStdString()+"+"+str2.toStdString()+"+"+str3.toStdString(), "register");
        if(re == REGISTER_SUCCESS)
        {
            QMessageBox::information(this, QString::fromLocal8Bit("�ɹ�"), QString::fromLocal8Bit("ע����ɣ�"));
            return;
        }
        else if(re == REGISTER_FAILURE)
        {
            QMessageBox::information(this, QString::fromLocal8Bit("����"), QString::fromLocal8Bit("ע��ʧ�ܣ�"));
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
