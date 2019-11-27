#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QtNetwork>
#include <QUrl>
#include <QFile>
#include <QHostInfo>
#include <QUdpSocket>
#include <QTcpServer>
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void startRequest(QUrl url);

private:
    Ui::MainWindow *ui;
    QNetworkAccessManager *manger;
    QNetworkReply *reply;
    QUrl url;
    QFile *file;
    QUdpSocket *sender;
    QTcpServer *server;
private slots:
    void httpFinished();
    void httpReadyRead();
    void updateDataReadProgress(qint64,qint64);
    void on_pushButton_clicked();
    void lookup(const QHostInfo &info);
    void on_pushButton_2_clicked();
    void sendMessage();
};

#endif // MAINWINDOW_H
