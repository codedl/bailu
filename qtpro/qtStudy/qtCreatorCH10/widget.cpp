#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget )
{
    ui->setupUi(this);

    setMouseTracking(true);

    QTimer *timer = new QTimer(this);
    //connect(timer, SIGNAL(timeout()),this,SLOT(update()));
    timer->start(1000);
    angle = 0;

}

Widget::~Widget()
{
    delete ui;
}

void Widget::paintEvent(QPaintEvent *event)
{
#if 0
    QPainter painter;
    painter.begin(this);
    painter.drawLine(QPoint(0,0),QPoint(100,100));

    QPen pen(Qt::green,5,Qt::DotLine,Qt::RoundCap,Qt::RoundJoin);
    painter.setPen(pen);
    QRectF rect(70,40,80,60);
    painter.drawArc(rect,30*16,120*16);

    pen.setWidth(1);
    pen.setStyle(Qt::SolidLine);
    painter.setPen(pen);
    painter.drawRect(100,100,100,50);

    QBrush brush(QColor(0,0,255),Qt::Dense3Pattern);
    painter.setBrush(brush);
    painter.drawEllipse(100,150,50,50);

    brush.setTexture(QPixmap("./brush.png"));
    painter.setBrush(brush);
    static const QPoint points[] ={
        QPoint(100,200),
        QPoint(120,240),
        QPoint(170,240),
        QPoint(150,200)
    };
    painter.drawPolygon(points,4);
#if 0
    //线性渐变
    QLinearGradient liner(QPoint(220,100),QPoint(280,100));
    liner.setColorAt(0,Qt::yellow);
    liner.setColorAt(0.5,Qt::red);
    liner.setColorAt(1,Qt::green);
    liner.setSpread(QGradient::RepeatSpread);
    painter.setBrush(liner);
    painter.drawRect(200,100,300,50);
    //辐射渐变
    QRadialGradient radial(QPoint(200,190),50,QPoint(275,200));
    radial.setColorAt(0,Qt::red);
    radial.setColorAt(1,Qt::green);
    painter.setBrush(radial);
    painter.drawEllipse(200,190,50,50);
    //锥形渐变
    QConicalGradient conical(QPoint(350,190),60);
    conical.setColorAt(0.2,Qt::yellow);
    conical.setColorAt(0.9,Qt::green);
    painter.setBrush(conical);
    painter.drawEllipse(QPoint(350,190),25,25);
    painter.drawText(150,280,tr("helloQT"));
#endif
    //painter.fillRect(this->rect(),Qt::white);
    painter.setPen(QPen(Qt::red,11));
    painter.translate(200,150);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawLine(QPoint(0,0),QPoint(100,100));

    painter.save();
    painter.rotate(90);
    painter.setPen(Qt::cyan);
    painter.drawLine(QPoint(0,0),QPoint(100,100));
    painter.restore();

    painter.setBrush(Qt::darkGreen);
    painter.drawRect(-50,-50,100,50);
    painter.save();
    painter.scale(0.5,0.4);
    painter.setBrush(Qt::yellow);
    painter.drawRect(-50,-50,100,50);
    painter.restore();

    painter.save();
    painter.setPen(Qt::blue);
    painter.setBrush(Qt::darkYellow);
    painter.drawEllipse(QRect(60,-100,50,50));
    painter.shear(1.5,-0.7);
    painter.setBrush(Qt::darkGray);
    painter.drawEllipse(QRect(60,-100,50,50));
    painter.end();
#endif
   // QPainter painter(this);
//    painter.setWindow(-50,-50,100,100);
//    painter.setBrush(Qt::green);
//    painter.drawRect(0,0,20,20);

//    int side = qMin(width(),height());
//    int x = (width()/2);
//    int y = (height()/2);
//    painter.setViewport(x,y,side,side);
//    painter.setWindow(0,0,100,100);
//    painter.setBrush(Qt::green);
//    painter.drawRect(0,0,20,20);
#if 0
    angle += 10;
    if(angle == 360)
        angle = 0;
    int side = qMin(width(),height());
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QTransform trans;
    trans.translate(width()/2,height()/2);
    trans.scale(side/300,side/300);
    trans.rotate(angle);
    painter.setWorldTransform(trans);
    painter.drawEllipse(-120,-120,240,240);
    painter.drawLine(0,0,100,0);
#endif
#if 0
    QPainter painter(this);
    painter.setBrush(Qt::red);
    QPainterPath path;
    path.moveTo(50,250);
    path.lineTo(50,230);
    path.cubicTo(QPoint(105,40),QPoint(115,80),QPoint(120,60));
    path.lineTo(130,130);
    path.addEllipse(10,50,100,100);
    path.addRect(50,100,100,100);
    painter.setPen(Qt::yellow);
    painter.drawPath(path);
    path.translate(200,0);
    painter.setPen(Qt::blue);
    path.setFillRule(Qt::WindingFill);
    painter.drawPath(path);
#endif
    QPainter painter(this);
    QImage image(400,300,QImage::Format_ARGB32_Premultiplied);
    painter.begin(&image);
    painter.setBrush(Qt::green);
    painter.drawRect(100,50,200,200);
    painter.setBrush(QColor(0,0,255,150));
    painter.drawRect(50,0,100,100);
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.drawRect(250,0,100,100);
}

void Widget::mouseMoveEvent(QMouseEvent *event)
{
    QString pos = QString("%1,%2").arg(event->pos().x()).arg(event->pos().y());
    QToolTip::showText(event->globalPos(),pos,this);
}
