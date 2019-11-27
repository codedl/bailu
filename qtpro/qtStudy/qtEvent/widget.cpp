#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    setFocus();
    keyLeft = false;
    keyUp = false;
    move = false;

    id1 = startTimer(1000);
//    id2 = startTimer(2000);
//    id3 = startTimer(3000);

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Widget::timerUpdate);
    timer->start(1000);

    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));

//    QTimer::singleShot(10000,this,&Widget::close);

    ui->textEdit->installEventFilter(this);
    ui->spinBox->installEventFilter(this);
}

Widget::~Widget()
{
    delete ui;
}

bool Widget::eventFilter(QObject *obj, QEvent *event)
{
    if(obj == ui->textEdit){
        if(event->type() == QEvent::Wheel){
            QWheelEvent *wheel = static_cast<QWheelEvent*>(event);
            if(wheel->delta() > 0) ui->textEdit->zoomIn();
            else ui->textEdit->zoomOut();
            return true;
        }else return false;
    }else if(obj == ui->spinBox){
        if(event->type() == QEvent::KeyPress){
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
            if(keyEvent->key() == Qt::Key_Space){
                ui->spinBox->setValue(0);
                return true;
            }else return false;
        }
    }else return Widget::eventFilter(obj,event);
}

void Widget::keyPressEvent(QKeyEvent *event)
{
    if(event->modifiers() == Qt::ControlModifier){
        if(event->key() == Qt::Key_M)
            setWindowState(Qt::WindowMaximized);
    }
    if(event->isAutoRepeat()) return;
    if(event->key() == Qt::Key_Up){
        keyUp = true;
    }else if(event->key() == Qt::Key_Left){
        keyLeft = true;
    }
}
void Widget::keyReleaseEvent(QKeyEvent *event)
{
    if(event->isAutoRepeat()) return;
    if(event->key() == Qt::Key_Up){
        keyUp = false;
        if(move){
            move = false;
            return;
        }
        if(keyLeft){
            ui->pushButton->move(30,80);
            move = true;
        }else{
            ui->pushButton->move(120, 80);
        }
    }else if(event->key() == Qt::Key_Left){
        keyLeft = false;
        if(move){
            move = false;
            return;
        }
        if(keyUp){
            ui->pushButton->move(30, 80);
            move = true;
        }else {
            ui->pushButton->move(30, 120);
        }

    }
    else if(event->key() == Qt::Key_Down) {
        ui->pushButton->move(120, 120);
    }
}

void Widget::timerEvent(QTimerEvent *timer)
{
    if(timer->timerId() == id1)
        qDebug() << "timer1";
    else if(timer->timerId() == id2)
        qDebug() << "timer2";
    else if(timer->timerId() == id3)
        qDebug() << "timer3";

    QKeyEvent event(QEvent::KeyPress,Qt::Key_Up,Qt::NoModifier);//在栈上定义
    qApp->sendEvent(ui->spinBox, &event);
}

void Widget::timerUpdate()
{
    QTime time = QTime::currentTime();
    QString text = time.toString("hh:mm");
    if(time.second() % 2) text[2] = ' ';
    ui->lcdNumber->display(text);

    int rand = qrand()%300;
    ui->lcdNumber->move(rand, rand);
}
