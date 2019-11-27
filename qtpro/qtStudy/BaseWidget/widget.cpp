#include "widget.h"
#include "ui_widget.h"


Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    setMinimumSize(500,500);
    setMaximumSize(500,500);
    QPushButton *btn = new QPushButton("quit",this);
    btn->setGeometry(10,10,75,35);
    btn->setFont(QFont("Times",18,QFont::Bold));
    connect(btn,SIGNAL(clicked()),qApp,SLOT(quit()));

    text = new QLabel(this);
    text->setGeometry(90,10,120,35);
    QDateTime *time = new QDateTime(QDateTime::currentDateTime());
    text->setText(time->time().toString());
    QTimer *timer = new QTimer(this);
//    connect(timer,SIGNAL(timeout()),this,timeoutEvent());
    connect(timer,&QTimer::timeout,this,&Widget::timeoutEvent);
    timer->start(1000);

    ui->treeWidget->clear();
    QTreeWidgetItem *group = new QTreeWidgetItem(ui->treeWidget);
    group->setText(0,"group");
    group->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    group->setCheckState(0,Qt::Unchecked);
    QTreeWidgetItem *item1 = new QTreeWidgetItem(group);
    item1->setText(0,"item1");
    item1->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    item1->setCheckState(0,Qt::Unchecked);
    QTreeWidgetItem *item2 = new QTreeWidgetItem(group);
    item2->setText(0,"item2");
    item2->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    item2->setCheckState(0,Qt::Unchecked);
    QTreeWidgetItem *item3 = new QTreeWidgetItem(group);
    item3->setText(0,"item3");
    item3->setFlags(Qt::ItemIsSelectable | Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
    item3->setCheckState(0,Qt::Unchecked);
    connect(ui->treeWidget,SIGNAL(itemChanged(QTreeWidgetItem*,int)),this,SLOT(treeWidgetChanged(QTreeWidgetItem*,int)));

}

Widget::~Widget()
{
    delete ui;
}

void Widget::timeoutEvent(){

    QDateTime *time = new QDateTime(QDateTime::currentDateTime());
    text->setText(time->time().toString());
}

void Widget::treeWidgetChanged(QTreeWidgetItem *widget, int item){

}

