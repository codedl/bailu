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
#if 0
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

    QPainter painter(this);
    QPen pen(Qt::green, 5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
    painter.setPen(pen);
    QRectF rectf(20.0, 20.0, 80.0, 60.0);

    int startAngle = 30*16;
    int spanAngle  = 120*16;
    painter.drawArc(rectf,startAngle, spanAngle);

    pen.setWidth(1);
    pen.setStyle(Qt::SolidLine);
    painter.setPen(pen);
    painter.drawRect(160,20,50,40);
    QBrush brush(QColor(0,0,255),Qt::Dense4Pattern);
    painter.setBrush(brush);
//    painter.drawRect(160,20,50,40);
    painter.drawEllipse(220,20,50,50);
    brush.setTexture(QPixmap("pix.png"));
    painter.setBrush(brush);
    static const QPointF pontf[4] = {
        QPointF(270.0, 80.0),
        QPointF(290.0, 10.0),
        QPointF(350.0, 30.0),
        QPointF(390.0, 70.0),
    };
    painter.drawPolygon(pontf,4);
    painter.fillRect(QRect(10,100,150,20),QBrush(Qt::darkYellow));
    QPainter painter(this);
    QRectF rect(10.0, 10.0, 380.0, 280.0);
    painter.setPen(Qt::red);
    painter.drawRect(rect);
    painter.setPen(Qt::blue);
    painter.drawText(rect,Qt::AlignLeft,tr("left"));

    QFont font("宋体",15,QFont::Bold,true);
    font.setOverline(true);
    font.setUnderline(true);
    font.setCapitalization(QFont::SmallCaps);
    font.setLetterSpacing(QFont::AbsoluteSpacing,10);
    painter.setFont(font);
    painter.setPen(Qt::green);
    painter.drawText(120,80,tr("hello12080"));
    painter.translate(100,100);
    painter.rotate(90);
    painter.drawText(0,0,tr("hello00"));

    QImage image(100, 100, QImage::Format_ARGB32);
    painter.begin(&image);
    painter.setPen(QPen(Qt::green,3));
    painter.setBrush(Qt::yellow);
    painter.drawRect(10,10,60,60);
    QPainter painter(this);
    QImage image;
    image.load("image.png");
    qDebug()<<image.size()<<image.depth()<<image.format();
    painter.drawImage(QPoint(10,10),image);

    QImage mirror = image.mirrored();
    QTransform tran;
    tran.shear(0.2,0);
    QImage image2 = mirror.transformed(tran);
    painter.drawImage(QPoint(10,160),image2);
    image2.save("mirrow.png");
#endif
    QPainter painter(this);
    QPixmap pix;
    pix.load("image.png");
    painter.drawPixmap(0,0,pix.width(),pix.height(),pix);
    painter.setBrush(QColor(255,255,255,100));
    painter.drawRect(0,0,pix.width(),pix.height());

    painter.drawPixmap(100,0,pix.width(),pix.height(),pix);
    painter.setBrush(QColor(0,0,255,100));
    painter.drawRect(100,0,pix.width(),pix.height());
}
