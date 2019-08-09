#ifndef SERVER_H
#define SERVER_H

#include<QTcpServer>
#include<QObject>
#include"tcpclientsocket.h"

class server :public QTcpServer
{
    Q_OBJECT
public:
    server(QWidget *parent, int port);
    QList<TcpClientSocket*> clients ;

signals:
    void updateServer(QString,int);
public slots:
    void updateClients(QString,int);
    void disconectClient(int);

protected:
    void incomingConnection(int socketDescriptor);
};

#endif // SERVER_H
