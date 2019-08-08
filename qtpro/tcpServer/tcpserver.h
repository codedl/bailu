#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QDialog>

namespace Ui {
class tcpserver;
}

class tcpserver : public QDialog
{
    Q_OBJECT

public:
    explicit tcpserver(QWidget *parent = 0);
    ~tcpserver();

private:
    Ui::tcpserver *ui;
};

#endif // TCPSERVER_H
