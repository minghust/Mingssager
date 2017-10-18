#include "recievefile.h"
#include "ui_recievefile.h"

RecieveFile::RecieveFile(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RecieveFile)
{
    ui->setupUi(this);
}

RecieveFile::~RecieveFile()
{
    delete ui;
}

void RecieveFile::on_buttonBox_accepted()
{

}

void RecieveFile::on_buttonBox_rejected()
{

}
