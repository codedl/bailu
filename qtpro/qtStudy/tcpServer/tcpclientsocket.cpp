#include "tcpclientsocket.h"

TcpClientSocket :: TcpClientSocket(QObject *parent){
    connect(this,SIGNAL(readyRead()),this,SLOT(datarcv()));
    connect(this,SIGNAL(disconnected()),this,SLOT(slotdisconnected()));
}

void TcpClientSocket::datarcv(){
    while(bytesAvailable() > 0){
        int length = bytesAvailable();
        char buf[1024];
        read(buf,length);
        QString msg = buf;
        emit update(msg, length);
    }
}

void TcpClientSocket::slotdisconnected(){
    emit disconnected(this->socketDescriptor());
}
