#include "widget.h"
#include "ui_widget.h"
#include <qdebug.h>
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
//    QHBoxLayout *hlayout = new QHBoxLayout(this);
//    hlayout->addWidget(ui->fontComboBox);
//    hlayout->addWidget(ui->textEdit);
//    hlayout->setSpacing(50);
//    hlayout->setContentsMargins(0,0,50,100);
//    setLayout(hlayout);
//    QGridLayout *glayout = new QGridLayout(this);
//    glayout->addWidget(ui->fontComboBox,0,0,1,2);
//    glayout->addWidget(ui->pushButton,0,2,1,1);
//    glayout->addWidget(ui->textEdit,1,0,1,3);
//    setLayout(glayout);

        ui->textEdit->hide();
        ui->buddyLabel->setText(tr("buddy(&T)"));
        ui->buddyLabel->setBuddy(ui->buddyButton);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_pushButton_toggled(bool checked)
{
    qDebug()<<tr("on_pushButton_toggled");
    ui->textEdit->setVisible(checked);
    if(checked) ui->pushButton->setText(tr("hide widget"));
    else ui->pushButton->setText(tr("display widget"));
}
