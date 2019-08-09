#include "server.h"

server::server(QObject *parent,int port):QTcpServer(parent)
{
        listen(QHostAddress::Any,port);
}

void server::incomingConnection(int socketDescriptor){
        TcpClientSocket *clientSocket = new TcpClientSocket(this);
        clients.append(clientSocket);
        clientSocket->setSocketDescriptor(socketDescriptor);

        connect(clientSocket,SIGNAL(update(QString,int)),this,SLOT(updateClients(QString,int)));
        connect(clientSocket,SIGNAL(disconnected(int)),this,SLOT(disconectClient(int)));

}

void server::updateClients(QString msg, int length){
    emit updateServer(msg,length);
    for(int i=0; i<clients.count();i++){
        QTcpSocket *item = clients.at(i);
        if(item->write(msg.toLatin1(),length) != length){
            continue;
        }
    }
}

void server::disconectClient(int descriptor){
    for(int i=0; i<clients.count();i++){
        QTcpSocket *item = clients.at(i);
        if(item->socketDescriptor() == descriptor){
            clients.removeAt(i);
            return;
        }
    }
    return;
}
