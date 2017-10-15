#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "sendtoserver.h"
#include "getpasswdback.h"
#include "register.h"
#include "chat.h"
#include <QString>
#include <QMessageBox>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->passwdInput->setEchoMode(QLineEdit::Password);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_confirm_clicked()
{

    if(ui->userNameInput->text().isEmpty() || ui->passwdInput->text().isEmpty())
    {
        QMessageBox::information(this, QString::fromLocal8Bit("´íÎó"), QString::fromLocal8Bit("µÇÂ¼ÐÅÏ¢²»ÍêÕû£¡"));
        return;
    }
    else
    {
        QString loginName = ui->userNameInput->text();
        QString loginPasswd = ui->passwdInput->text();
        // send to server
        int re = SendtoServer(loginName.toStdString()+"+"+loginPasswd.toStdString(), "login");
        if(re == LOGIN_SUCCESS) // login success
        {
            // enter in chat
            Chat *ch = new Chat();
            ch->show();
        }
        else if(re == PSW_ERROR)
        {
            QMessageBox::information(this, QString::fromLocal8Bit("´íÎó"), QString::fromLocal8Bit("ÃÜÂë´íÎó£¡"));
            return;
        }
        else if(re == INVALID_USERNAME)
        {
            QMessageBox::information(this, QString::fromLocal8Bit("´íÎó"), QString::fromLocal8Bit("ÎÞ´ËÓÃ»§£¬Çë×¢²á£¡"));
            return;
        }
        else if(re == FILE_ERROR)
        {
            QMessageBox::information(this, QString::fromLocal8Bit("´íÎó"), QString::fromLocal8Bit("Êý¾Ý¶Á´íÎó£¡"));
            return;
        }
    }
}

void MainWindow::on_cancel_clicked()
{
    ui->userNameInput->clear();
    ui->passwdInput->clear();

    return;
}

void MainWindow::on_reg_clicked()
{
    ui->userNameInput->clear();
    ui->passwdInput->clear();
    Register *regwindow = new Register();
    regwindow->show();
}

void MainWindow::on_getPasswdBack_clicked()
{
    ui->userNameInput->clear();
    ui->passwdInput->clear();
    GetPasswdBack *gpbwindow = new GetPasswdBack();
    gpbwindow->show();
}
