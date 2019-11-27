#include "mainwindow.h"
#include <QApplication>

const int dataSize = 10;
const int bufferSize = 5;
char buffer[bufferSize];
QSemaphore freeBytes(bufferSize);
QSemaphore usedBytes;

class producer:public QThread{
public:
    void run();
};
void producer::run()
{
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    for(int i=0; i<dataSize; i++){
        freeBytes.acquire();
        buffer[i%bufferSize] = "ABCD"[(int)qrand()%4];
        qDebug() << QString("producer:%1").arg(buffer[i%bufferSize]);
        usedBytes.release();
    }
}

class consumer : public QThread{
public:
    void run();
};
void consumer::run()
{
    for(int i=0; i<dataSize; i++){
        usedBytes.acquire();
        qDebug() << QString("consumer:%1").arg(buffer[i%bufferSize]);
        freeBytes.release();
    }
}


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    producer p;
    consumer c;

    //p.start();
    //c.start();
    //c.wait();
    //p.wait();

    return a.exec();
}
