#include "myaction.h"

myaction::myaction(QObject *parent):QWidgetAction(parent)
{
    lineEdit = new QLineEdit;
    connect(lineEdit, &QLineEdit::returnPressed,this,&myaction::sendText);
}

QWidget *myaction::createWidget(QWidget *parent)
{
    if(parent->inherits("QMenu")||parent->inherits("QToolBar")){
        qDebug()<<"splitter";
        QSplitter *splitter = new QSplitter(parent);
        QLabel *label = new QLabel;
        label->setText(tr("插入文本"));
        splitter->addWidget(label);
        splitter->addWidget(lineEdit);
        return splitter;
    }
    qDebug()<<"return";
    return 0;
}

void myaction::sendText()
{
    emit getText(lineEdit->text());
    lineEdit->clear();
}

