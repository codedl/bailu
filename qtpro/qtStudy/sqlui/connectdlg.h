#ifndef CONNECTDLG_H
#define CONNECTDLG_H

#include <QDialog>

namespace Ui {
class ConnDlg;
}

class ConnDlg : public QDialog
{
    Q_OBJECT

public:
    explicit ConnDlg(QWidget *parent = 0);
    ~ConnDlg();

private:
    Ui::ConnDlg *ui;
};

#endif // CONNECTDLG_H
