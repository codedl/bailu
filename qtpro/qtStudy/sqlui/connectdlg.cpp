#include "connectdlg.h"
#include "ui_connectdlg.h"

ConnDlg::ConnDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConnDlg)
{
    ui->setupUi(this);
}

ConnDlg::~ConnDlg()
{
    delete ui;
}
