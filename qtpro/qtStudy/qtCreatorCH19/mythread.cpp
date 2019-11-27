#include "mythread.h"
#include <QDebug>
MyThread::MyThread(QObject *parent ) : QThread(parent){
    stopped = false;
}

void MyThread::stop()
{
    stopped = true;
}

void MyThread::run()
{
    qreal i=0;
    while(!stopped){
        qDebug()<< tr("i in thread:%1").arg(i);
        msleep(1000);
        i++;
    }
    stopped = false;
}
