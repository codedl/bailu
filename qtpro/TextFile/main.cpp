#include <QCoreApplication>
#include <QFile>
#include <QtDebug>
#include <QDate>
#include <QDataStream>
#include <QDateTime>
#include <QFileInfo>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    QFile file("file.txt");
    if(file.open(QIODevice::ReadOnly)){
        char buf[1024];
        int length = file.readLine(buf,sizeof buf);
        if(length != -1){
            qDebug()<<buf;
            qDebug()<<length;
        }
    }

    QFile file2("data.txt");
    if(file2.open(QIODevice::WriteOnly | QIODevice::Truncate)){
        QTextStream out(&file2);
        out << QObject::tr("today:")<<qSetFieldWidth(8) <<left << 20190809<<endl;
    }

    QFile file3("binary.dat");
    file3.open(QIODevice::WriteOnly | QIODevice::Truncate);
    QDataStream out (&file3);
    out << QString(QObject::tr("orchid:"));
    out << QDate::fromString("1997/04/04","yyyy/MM/dd");
    out << (qint32)23;

    file3.close();
    file3.setFileName("binary.dat");

    if(file3.open(QIODevice::ReadOnly)){
        QDataStream in(&file3);
        QString name;
        QDate date;
        qint32 age;
        in >> name >> date >> age;
        qDebug() << name << date << age;
    }
    file3.close();

    QFileInfo info("binary.dat");
    qint64 size = info.size();
    QDateTime createTime = info.created();
    QDateTime modifyTime = info.lastModified();
    QDateTime readTime = info.lastRead();
    qDebug()<<size;
    qDebug()<<createTime.toString();
    qDebug()<<modifyTime.toString();
    qDebug()<<readTime.toString();
    return a.exec();
}
