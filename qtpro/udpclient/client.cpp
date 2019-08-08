#include "client.h"
#include "ui_client.h"

client::client(QWidget *parent, Qt::WindowFlags f) :
    QDialog(parent, f),
    ui(new Ui::client)
{
    setWindowTitle(tr("client"));
    rcvEdit = new QTextEdit();
    closeBtn = new QPushButton(tr("close"));
    mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(rcvEdit);
    mainLayout->addWidget(closeBtn);

    udpSocket = new QUdpSocket(this);
    port = 5555;
    bool result = udpSocket->bind(port);
    if(!result){
        QMessageBox::information(this,tr("error"),tr("bind failed"));
        return;
    }

    connect(closeBtn,SIGNAL(clicked()),this,SLOT(BtnClicked()));
    connect(udpSocket,SIGNAL(readyRead()),this,SLOT(dataRcv()));
}

void client::BtnClicked(){
    udpSocket->close();
}

void client::dataRcv(){
    while(udpSocket->hasPendingDatagrams()){
        QByteArray data;
        data.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(data.data(),data.size());
        QString msg = data.data();
        rcvEdit->insertPlainText(msg);
    }
}

client::~client()
{
    delete ui;
}
