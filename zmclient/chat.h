#ifndef CHAT_H
#define CHAT_H

#include <QWidget>
#include <QCloseEvent>
#include <QMessageBox>
#include <QString>
#include <string>
#include <vector>
#include <QTimer>
#include <QListWidgetItem>
#include <cstdlib>

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
    void chooseItem(QListWidgetItem *item);
    void on_updateList_clicked();

private:
    Ui::Chat *ui;
};

#endif // CHAT_H
