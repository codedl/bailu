#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QLayout>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QProgressBar>
#include <QPushButton>
#include <QProgressDialog>
class Dialog : public QDialog
{
    Q_OBJECT

public:
    Dialog(QWidget *parent = 0);
    ~Dialog();
private:
    QLabel *filenumLabel;
    QLineEdit *filenumLineEdit;
    QLabel *displayLabel;
    QComboBox *displayComboBox;
    QProgressBar *progress;
    QPushButton *startBtn;
    QGridLayout *mainLayout;

private slots:
    void showProgress();

};

#endif // DIALOG_H
