#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QDialog>

namespace Ui {
class tcpClient;
}

class tcpClient : public QDialog
{
    Q_OBJECT

public:
    explicit tcpClient(QWidget *parent = 0);
    ~tcpClient();

private:
    Ui::tcpClient *ui;
};

#endif // TCPCLIENT_H
