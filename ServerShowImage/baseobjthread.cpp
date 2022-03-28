#include "baseobjthread.h"

BaseObjThread::BaseObjThread(QObject *parent) : QObject(parent)
{
    this->setObjectName("BaseObjectThread");
}

BaseObjThread::~BaseObjThread()
{
    qDebug()<<"Delete Object: "<<this->objectName()<<"in thread : "<< QThread::currentThreadId();
}

