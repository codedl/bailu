#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    MyDialog *dlg = new MyDialog(this);
    connect(dlg,SIGNAL(dlgReturn(int)),this,SLOT(showValue(int)));
    dlg->show();
}

Widget::~Widget()
{
    delete ui;
}

void Widget::showValue(int value)
{
    ui->label->setText(tr(" value: %1 ").arg(value));
}
