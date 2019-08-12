#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    setGeometry(100,100,200,200);
    ui->setupUi(this);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::paintEvent(QPaintEvent *){
//    rect.setRect(20,20,30,30);
//    QPixmap pix(rect.size());
//    painter.begin(&pix);
//    painter.setPen(QPen(Qt::blue,4,Qt::DashLine));
//    painter.setBrush(QBrush(Qt::red,Qt::SolidPattern));
//    painter.drawRect(20,20,30,30);
//    painter.end();
//    QPainter *paint;
//    paint = new QPainter;
//    paint->begin(this);
//    paint->setPen(QPen(Qt::blue,4,Qt::DashLine));
//    paint->setBrush(QBrush(Qt::red,Qt::SolidPattern));
//    paint->drawRect(20,20,30,30);
//    paint->end();
    QRect rect(20,20,60,30);
    QPixmap pixmap(rect.size());
    QPainter painter(this);
    if(!pixmap.isNull()){
        painter.begin(&pixmap);
//        painter.fillRect(pixmap.rect(),QColor(90,95,225));
        painter.setFont(QFont("ch", 20, 75, false));
        painter.drawRect(rect);
        painter.drawText(rect,Qt::AlignLeft,"qt");
        painter.end();
    }
}
