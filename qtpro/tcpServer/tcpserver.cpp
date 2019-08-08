#include "tcpserver.h"
#include "ui_tcpserver.h"

tcpserver::tcpserver(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::tcpserver)
{
    ui->setupUi(this);
}

tcpserver::~tcpserver()
{
    delete ui;
}
