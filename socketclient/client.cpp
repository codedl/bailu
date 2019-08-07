#include "client.h"
#include "ui_client.h"

client::client(QWidget *parent, Qt::WindowFlags f) :
    QDialog(parent, f),
    ui(new Ui::client)
{
    setWindowTitle(tr("client"));
    rcvEdit = new QTextEdit();
    closeBtn = new QPushButton(tr("close"));
    mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(rcvEdit);
    mainLayout->addWidget(closeBtn);
}

client::~client()
{
    delete ui;
}
