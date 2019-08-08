#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QDialog>
#include <QListWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QGridLayout>
#include "server.h"

namespace Ui {
class tcpserver;
}

class tcpserver : public QDialog
{
    Q_OBJECT

public:
    explicit tcpserver(QWidget *parent = 0, Qt::WindowFlags f = 0);
    ~tcpserver();

private:
    Ui::tcpserver *ui;
    QListWidget *contentList;
    QLabel *portLabel;
    QPushButton *createBtn;
    QLineEdit *portEdit;
    QGridLayout *mainLayout;
    int port;
    server *Server;
public slots:
    void createServer();
};

#endif // TCPSERVER_H
