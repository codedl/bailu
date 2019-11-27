#include "tcpserver.h"
#include "ui_tcpserver.h"

tcpserver::tcpserver(QWidget *parent, Qt::WindowFlags f) :
    QDialog(parent, f),
    ui(new Ui::tcpserver)
{
    setWindowTitle(tr("Tcp Server"));
    contentList = new QListWidget(this);
    portEdit = new QLineEdit(this);
    portLabel = new QLabel(tr("端口:"));
    createBtn = new QPushButton(tr("创建聊天室"));
    mainLayout = new QGridLayout(this);

    mainLayout->addWidget(contentList,0,0,1,2);
    mainLayout->addWidget(portLabel,1,0);
    mainLayout->addWidget(portEdit,1,1);
    mainLayout->addWidget(createBtn,2,0,1,2);

    port = 8010;
    portEdit->setText(QString::number(port));
    connect(createBtn,SIGNAL(clicked(bool)),this,SLOT(createServer()));
}

void tcpserver::createServer(){
    Server = new server(this, port);
    createBtn->setEnabled(false);
    //connect(server,SIGNAL(updateServer(QString,int)),this,SLOT(updateServer(QString,int)));

}

void tcpserver::updateServer(QString msg, int length){
    contentList->addItem(msg.left(length));
}

tcpserver::~tcpserver()
{
    delete ui;
}
