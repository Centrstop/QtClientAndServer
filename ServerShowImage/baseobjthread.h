#ifndef BASEOBJTHREAD_H
#define BASEOBJTHREAD_H

#include <QObject>
#include <QThread>
#include <QDebug>
class BaseObjThread : public QObject
{
    Q_OBJECT
public:
    explicit BaseObjThread(QObject *parent = 0);
    ~BaseObjThread();
signals:
    void finished();
    void ErrorObject(const int &errorCode, const QString &what);
public slots:
    virtual void onStartObject() = 0;
    virtual void onStopObject() = 0;
};

#endif // BASEOBJTHREAD_H

