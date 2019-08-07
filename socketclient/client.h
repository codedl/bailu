#ifndef CLIENT_H
#define CLIENT_H

#include <QDialog>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QPushButton>

namespace Ui {
class client;
}

class client : public QDialog
{
    Q_OBJECT

public:
    explicit client(QWidget *parent = 0, Qt::WindowFlags f =0 );
    ~client();

private:
    Ui::client *ui;
    QTextEdit *rcvEdit;
    QPushButton *closeBtn;
    QVBoxLayout *mainLayout;
};

#endif // CLIENT_H
