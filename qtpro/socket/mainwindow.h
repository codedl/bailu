#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QGridLayout>
#include <QMessageBox>
#include <QHostInfo>
#include <QNetworkInterface>
#include <QUdpSocket>
#include <QTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void getHostInfo();

private:
    Ui::MainWindow *ui;
    QLabel *hostLabel;
    QLineEdit *hostNamelineEdit;
    QLabel *ipLabel;
    QLineEdit *lineAddress;
    QPushButton *detailBtn;
    QGridLayout *mainLayout;
    bool isStart;
    int port;
    QTimer *timer;
    QUdpSocket *udpsocket;
    QLabel *timerLabel;
    QLineEdit *timerLineEdit;
    QPushButton *StartBtn;

public slots:
    void slotsDetil();
    void BtnClicked();
    void timeout();
};

#endif // MAINWINDOW_H
