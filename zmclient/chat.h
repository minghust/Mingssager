#ifndef CHAT_H
#define CHAT_H

#include <QWidget>
#include <QTextTableFormat>
namespace Ui {
class Chat;
}

class Chat : public QWidget
{
    Q_OBJECT

public:
    explicit Chat(QWidget *parent = 0);

    ~Chat();

private slots:

    void on_sendfile_clicked();

    void on_sendmsg_clicked();

    void on_choosefile_clicked();

private:
    Ui::Chat *ui;
    QTextTableFormat tableFormat;
};

#endif // CHAT_H
