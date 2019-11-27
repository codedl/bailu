#include "dialog.h"
#include "ui_dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    receiver = new QUdpSocket(this);
    receiver->bind(1200,QUdpSocket::ShareAddress);
    connect(receiver,&QUdpSocket::readyRead,this,&Dialog::processPendingData);
    tcpSocket = new QTcpSocket(this);
    connect(tcpSocket,&QTcpSocket::readyRead,this,&Dialog::readMessage);
    connect(tcpSocket,SIGNAL(error(QAbstractSocket::SocketError)),this,\
            SLOT(displayError(QAbstractSocket::SocketError)));
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::processPendingData()
{
    while(receiver->hasPendingDatagrams()){
        QByteArray data;
        data.resize(receiver->pendingDatagramSize());
        receiver->readDatagram(data.data(),data.size());
        ui->label->setText(data);
    }
}

void Dialog::newConnect()
{
    blocksize = 0;
    tcpSocket->abort();
    tcpSocket->connectToHost(ui->hostLineEdit->text(),ui->portLineEdit->text().toInt());
}

void Dialog::readMessage()
{
    QDataStream in(tcpSocket);
    in.setVersion(QDataStream::Qt_5_2);
    if(blocksize == 0){
        if(tcpSocket->bytesAvailable() < (int)sizeof(quint16))
            return;
        in >> blocksize;
    }
    if(tcpSocket->bytesAvailable() < blocksize)
        return;
    in >> message;
    ui->messageLabel->setText(message);
}

void Dialog::displayError(QAbstractSocket::SocketError)
{
    qDebug() << tcpSocket->errorString();
}

void Dialog::on_connectBtn_clicked()
{
    newConnect();
}
