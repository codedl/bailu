#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    QTimer *timer = new QTimer(this);
    connect(timer,&QTimer::timeout,this,&Widget::updateTime);
    timer->start(1000);
    ui->setupUi(this);
}

void Widget::updateTime(){
    QTime time = QTime::currentTime();
    QString text = time.toString("hh:mm");
    if(time.second()%2)
        text[2] = ' ';
    ui->lcdNumber->display(text);
}

Widget::~Widget()
{
    delete ui;
}
