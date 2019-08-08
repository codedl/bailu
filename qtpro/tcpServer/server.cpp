#include "server.h"

server::server(QObject *parent, int port) : QTcpServer(parent)
{
        listen(QHostAddress::Any,port);
}
