#include<QtWidgets>
#include <QApplication>
#include <QDebug>
int main(int argc,char* argv[])
{
    QApplication a(argc,argv);
//    QWidget *widget = new QWidget(0,Qt::Dialog|Qt::FramelessWindowHint);
//    QLabel *label = new QLabel(widget);
//    label->setText(QObject::tr("a label in widget"));
//    label->resize(200, 20);
//    widget->show();

//    QLabel *Label = new QLabel(0,Qt::SplashScreen|Qt::WindowStaysOnTopHint);
//    Label->setWindowTitle(QObject::tr("label"));
//    Label->setText(QObject::tr("a label no parent"));
//    Label->resize(200,20);
//    Label->show();

    QWidget widget;
    widget.resize(400, 300);
    widget.move(200, 100);
    widget.show();
    int x = widget.x();
    int y = widget.y();
    qDebug("x:%d",x);
    qDebug("y:%d",y);
    QRect geometry = widget.geometry();
    QRect frame = widget.frameGeometry();
    qDebug()<< "geometry:" << geometry << "frame:" <<frame;
    qDebug()<< "pos:" << widget.pos() << "rect:" << widget.rect() << "width:" << widget.width()
            << "height:" << widget.height();
    return a.exec();
}
