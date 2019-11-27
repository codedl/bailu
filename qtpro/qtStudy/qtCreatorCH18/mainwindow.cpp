#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <qDebug>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    manger = new QNetworkAccessManager(this);
    ui->progressBar->hide();

    QString hostName = QHostInfo::localHostName();
    QHostInfo info = QHostInfo::fromName(hostName);
    qDebug()<<hostName;
    qDebug()<<info.addresses();
    foreach (QHostAddress address, info.addresses()) {
        if(address.protocol() == QAbstractSocket::IPv4Protocol)
            qDebug() << address.toString();
    }
    QHostInfo::lookupHost("www.baidu.com",this,SLOT(lookup(QHostInfo)));

    QList<QNetworkInterface> list = QNetworkInterface::allInterfaces();
    foreach (QNetworkInterface interface, list) {
        qDebug() << "name :" << interface.name();
        qDebug() << "mac address:" << interface.hardwareAddress();
        QList<QNetworkAddressEntry> entrylist = interface.addressEntries();
        foreach (QNetworkAddressEntry entry, entrylist) {
            qDebug() << "ip:" << entry.ip().toString();
            qDebug() << "mask:" << entry.netmask().toString();
            qDebug() << "broadcast:" << entry.broadcast().toString();
        }
    }
    sender = new QUdpSocket(this);
    server = new QTcpServer(this);
    if(!server->listen(QHostAddress::LocalHost,6666)){
        qDebug() << server->errorString();
        server->close();
    }
    connect(server,&QTcpServer::newConnection,this,&MainWindow::sendMessage);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::startRequest(QUrl url)
{
    reply = manger->get(QNetworkRequest(url));
    connect(reply,&QNetworkReply::readyRead,this,&MainWindow::httpReadyRead);
    connect(reply,&QNetworkReply::downloadProgress,this,&MainWindow::updateDataReadProgress);
    connect(reply,&QNetworkReply::finished,this,&MainWindow::httpFinished);

}

void MainWindow::httpFinished()
{
    ui->progressBar->hide();
    if(file){
        QFileInfo info(file->fileName());
        qDebug() << info.absolutePath();
        qDebug() << file->fileName();
        file->close();
        delete file;
        file=0;
    }
    reply->deleteLater();
    reply = 0;
}

void MainWindow::httpReadyRead()
{
    if(file)
        file->write(reply->readAll());
}

void MainWindow::updateDataReadProgress(qint64 cur, qint64 max)
{
    ui->progressBar->setMaximum(max);
    ui->progressBar->setValue(cur);
}


void MainWindow::on_pushButton_clicked()
{
    url = ui->lineEdit->text();
//    QUrl url;
//    url.setScheme("ftp");
//    url.setHost("v0.ftp.upyun.com");
//    url.setPath("readme.txt");
//    url.setUserName("qtertest/qtftptest");
//    url.setPassword("pwd123456");
    QFileInfo info(url.path());
    QString fileName(info.fileName());
    if(fileName.isEmpty())
        fileName = "index.html";
    qDebug() << fileName;
    file = new QFile(fileName);
    if(!file->open(QIODevice::WriteOnly)){
        delete file;
        file = 0;
        return;
    }
    qDebug() << "open succeed";
    startRequest(url);
    ui->progressBar->setValue(0);
    ui->progressBar->show();
    qDebug() << "show succeed";
}

void MainWindow::lookup(const QHostInfo &info)
{
    if(info.error() != QHostInfo::NoError){
        qDebug() << info.errorString();
        return;
    }
    foreach (const QHostAddress &address, info.addresses()) {
        qDebug() << "found address:" << address.toString();
    }
}

void MainWindow::on_pushButton_2_clicked()
{
    QByteArray data = "hello";
    sender->writeDatagram(data.data(),data.size(),QHostAddress::Broadcast,1200);
}

void MainWindow::sendMessage()
{
    QByteArray block;
    QDataStream out(&block,QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_2);
    out << (quint16)0;
    out << tr("hellotcp");
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));
    QTcpSocket *client = server->nextPendingConnection();
    connect(client,&QTcpSocket::disconnected,client,&QTcpSocket::deleteLater);
    client->write(block);
    client->disconnectFromHost();
    ui->label_2->setText(tr("data send succeed"));
}
