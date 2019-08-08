#include "tcpclient.h"
#include "ui_tcpclient.h"

tcpClient::tcpClient(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::tcpClient)
{
    ui->setupUi(this);
}

tcpClient::~tcpClient()
{
    delete ui;
}
