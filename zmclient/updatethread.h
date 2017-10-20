#ifndef UPDATETHREAD_H
#define UPDATETHREAD_H
#include <QThread>

class WorkerThread : public QThread
{
    Q_OBJECT

public:
    explicit WorkerThread(QObject *parent = 0) : QThread(parent){}

protected:
    virtual void run() Q_DECL_OVERRIDE
    {
        while (1)
        {
            msleep(5000);
            emit resultReady();
        }
    }
signals:
    void resultReady();
};
#endif // UPDATETHREAD_H
