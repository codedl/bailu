#ifndef CLIENT_H
#define CLIENT_H

#include <QDialog>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QUdpSocket>
#include <QMessageBox>
#include <QHostAddress>

namespace Ui {
class client;
}

class client : public QDialog
{
    Q_OBJECT

public:
    explicit client(QWidget *parent = 0, Qt::WindowFlags f =0 );
    ~client();

private:
    Ui::client *ui;
    QTextEdit *rcvEdit;
    QPushButton *closeBtn;
    QVBoxLayout *mainLayout;
    int port;
    QUdpSocket *udpSocket;

public slots:
    void BtnClicked();
    void dataRcv();
};

#endif // CLIENT_H
