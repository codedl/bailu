#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(&process,&QProcess::readyRead,this,&MainWindow::showResult);
    connect(&process,&QProcess::stateChanged,this,&MainWindow::showChanged);
    connect(&process,&QProcess::errorOccurred,this,&MainWindow::showError);
    connect(&process,SIGNAL(finished(int, QProcess::ExitStatus)),\
            this,SLOT(showFinished(int, QProcess::ExitStatus)));
    sharedMemory.setKey("sharedMemoryExample");

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QString program = "cmd.exe";
    QStringList param;
    param << "/c dir&pause";
    process.start(program,param);
}

void MainWindow::showResult()
{
    QTextCodec *codec = QTextCodec::codecForLocale();
    QStringList array = codec->toUnicode(process.readAll()).split(" ",QString::SkipEmptyParts);
    qDebug() << "result:";
    for(int i=0; i<array.size(); i++){
        qDebug() << array.at(i);
    }
}

void MainWindow::showChanged(QProcess::ProcessState state)
{
    if(state == QProcess::NotRunning)
        qDebug() << "notruning";
    else if(state == QProcess::Starting)
        qDebug() << "starting";
    else if(state == QProcess::Running)
        qDebug() << "runing";
    else
        qDebug() << "unknown";
}

void MainWindow::showError(QProcess::ProcessError error)
{
    qDebug() << "error:" << error;
}

void MainWindow::showFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    qDebug() << "exitCode:" << exitCode << "exitStatus:" << exitStatus;
}

void MainWindow::loadFromFile()
{
    if(sharedMemory.isAttached())
        detach();
    ui->label->setText(tr("select picture"));
    QString file = QFileDialog::getOpenFileName(0,QString(),QString(),tr("Image(*.png *.jpg)"));
    QImage image;
    if(!image.load(file)){
        ui->label->setText(tr("load failed"));
        return;
    }
    ui->label->resize(QPixmap::fromImage(image).size());
    ui->label->setPixmap(QPixmap::fromImage(image));
    QBuffer buffer;
    buffer.open(QBuffer::ReadWrite);
    QDataStream out(&buffer);
    out << image;
    int size = buffer.size();
    if(!sharedMemory.create(size)){
        ui->label->setText(tr("create failed"));
        return;
    }
    sharedMemory.lock();
    char *to = (char *)sharedMemory.data();
    const char *from = (char *)buffer.data().data();
    memcpy(to,from,qMin(size,sharedMemory.size()));
    sharedMemory.unlock();
}

void MainWindow::loadFromShared()
{
    if(!sharedMemory.attach()){
        ui->label->setText(tr("attach failed"));
        return;
    }
    QBuffer buffer;
    QImage image;
    QDataStream in(&buffer);
    sharedMemory.lock();
    buffer.setData((char*)sharedMemory.constData(),sharedMemory.size());
    buffer.open(QBuffer::ReadOnly);
    in >> image;
    sharedMemory.unlock();
    sharedMemory.detach();
    ui->label->resize(QPixmap::fromImage(image).size());
    ui->label->setPixmap(QPixmap::fromImage(image));


}

void MainWindow::detach()
{
    if(!sharedMemory.detach())
        ui->label->setText(tr("detach failed"));
}

void MainWindow::on_loadBtn_clicked()
{
    loadFromFile();
}

void MainWindow::on_displayBtn_clicked()
{
    loadFromShared();
}

void MainWindow::on_startBtn_clicked()
{
    thread.start();
    ui->startBtn->setEnabled(false);
    ui->stopBtn->setEnabled(true);
}

void MainWindow::on_stopBtn_clicked()
{
    if(thread.isRunning()){
        thread.stop();
        ui->startBtn->setEnabled(true);
        ui->stopBtn->setEnabled(false);
    }
}

