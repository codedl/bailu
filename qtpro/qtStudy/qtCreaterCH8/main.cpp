#include "mainwindow.h"
#include <QApplication>
#include <QStyleFactory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    a.setStyle(QStyleFactory::create("windows"));
    MainWindow w;
    w.show();

    return a.exec();
}
