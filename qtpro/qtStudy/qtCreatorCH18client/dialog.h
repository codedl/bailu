#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QUdpSocket>
#include <QAbstractSocket>
#include <QTcpSocket>
namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

private:
    Ui::Dialog *ui;
    QUdpSocket *receiver;
    QTcpSocket *tcpSocket;
    QString message;
    quint16 blocksize;
private slots:
    void processPendingData();
    void newConnect();
    void readMessage();
    void displayError(QAbstractSocket::SocketError);
    void on_connectBtn_clicked();
};

#endif // DIALOG_H
