#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWindow)
{
    hostLabel = new QLabel(tr("主机名:"));
    hostNamelineEdit = new QLineEdit();
    ipLabel = new QLabel(tr("ip地址:"));
    lineAddress = new QLineEdit();
    detailBtn = new QPushButton(tr("详细"));
    mainLayout = new QGridLayout(this);
    mainLayout->addWidget(hostLabel,0,0);
    mainLayout->addWidget(hostNamelineEdit,0,1);
    mainLayout->addWidget(ipLabel,1,0);
    mainLayout->addWidget(lineAddress,1,1);
    mainLayout->addWidget(detailBtn,2,0,1,2);

    timerLabel = new QLabel(tr("定时器:"),this);
    timerLineEdit = new QLineEdit(this);
    StartBtn = new QPushButton(tr("开始"),this);
    mainLayout->addWidget(timerLabel,3,0);
    mainLayout->addWidget(timerLineEdit,3,1);
    mainLayout->addWidget(StartBtn,4,0,1,2);

    getHostInfo();
    connect(detailBtn,SIGNAL(clicked(bool)),this,SLOT(slotsDetil()));

    port = 5555;
    isStart = false;
    udpsocket = new QUdpSocket(this);
    timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(timeout()));
    connect(StartBtn,SIGNAL(clicked(bool)),this,SLOT(BtnClicked()));


}


MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow :: getHostInfo(){
    QString hostName = QHostInfo::localHostName();
    hostNamelineEdit->setText(hostName);

    QHostInfo hostInfo = QHostInfo::fromName(hostName);
    QList<QHostAddress> addresses = hostInfo.addresses();
    if(addresses.isEmpty()){
        lineAddress->setText(addresses.first().toString());
    }

}
void MainWindow::timeout(){
    QString msg = timerLineEdit->text();
    int length = 0;
    if(msg == "")
        return;
    if((length = udpsocket->writeDatagram(msg.toLatin1(),msg.length(),
                                          QHostAddress::Broadcast,port)) != msg.length())
        return;
}
void MainWindow::BtnClicked(){
    if(!isStart){
        isStart = true;
        StartBtn->setText(tr("stop"));
        timer->start(1000);

    }else{
        isStart = false;
        StartBtn->setText(tr("start"));
        timer->stop();
    }

}
void MainWindow::slotsDetil(){
    QString detail;
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    for(int i=0; i<interfaces.count(); i++){
        QNetworkInterface interface = interfaces.at(i);
        detail += tr("设备:") + interface.name() + "\n";
        detail += tr("硬件地址:") + interface.hardwareAddress() + "\n";
        QList<QNetworkAddressEntry>entrys = interface.addressEntries();
        for(int j=0; j<entrys.count(); j++){
            QNetworkAddressEntry entry = entrys.at(j);
            detail += tr("ip地址:") + entry.ip().toString() + "\n";
            detail += tr("子网掩码:") + entry.netmask().toString() + "\n";
            detail += tr("广播地址:") + entry.broadcast().toString();
        }

    }
    QMessageBox::information(this, tr("Detail"), detail);

}


