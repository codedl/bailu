#ifndef TCPCLIENTSOCKET_H
#define TCPCLIENTSOCKET_H
#include <QTcpSocket>
#include <QObject>
class TcpClientSocket:public QTcpSocket{
    Q_OBJECT
public:
    TcpClientSocket(QObject *parent = 0);
signals:
    void update(QString,int);
    void disconnected(int);
protected slots:
    void datarcv();
    void slotdisconnected();

};

#endif // TCPCLIENTSOCKET_H
