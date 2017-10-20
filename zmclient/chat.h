#ifndef CHAT_H
#define CHAT_H

#include <QWidget>
#include <QTextTableFormat>
#include <QCloseEvent>
#include <QMessageBox>
#include <QString>
#include <QFile>
#include <QFileDialog>
#include <string>
#include <vector>
#include <QTextCursor>
#include <QTextTable>
#include <QTimer>
#include <QScrollBar>

namespace Ui {
class Chat;
}

class Chat : public QWidget
{
    Q_OBJECT

public:
    bool is_open;
    explicit Chat(QWidget *parent = 0);
    void ShowUserList(std::string str);
    ~Chat();

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:

    void on_sendfile_clicked();

    void on_sendmsg_clicked();

    void on_choosefile_clicked();

private:
    Ui::Chat *ui;
    QTextTableFormat tableFormat;
};

#endif // CHAT_H
