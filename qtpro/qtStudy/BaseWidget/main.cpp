#include "widget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Widget w;
    w.show();

//    QWidget *window =new QWidget();
//    window->resize(320,240);
//    window->show();
//    QPushButton *btn = new QPushButton("btn",window);
//    btn->move(100,100);
//    btn->show();
    return a.exec();
}
