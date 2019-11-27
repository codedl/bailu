#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QTextCodec>
#include <QPushButton>
#include <QLineEdit>
#include <QGridLayout>
#include <QFileDialog>
#include <QColorDialog>
#include <QFontDialog>
#include <QMessageBox>
#include "inputdialog.h"
class Dialog : public QDialog
{
    Q_OBJECT
private:
    QGridLayout *mainLayout;

    QPushButton *fileBtn;
    QLineEdit *fileLineEdit;

    QPushButton *colorBtn;
    QFrame *colorFrame;

    QPushButton *fontBtn;
    QLineEdit *fontLineEdit;

    QPushButton *inputBtn;
    InputDlg *inputDlg;

    QPushButton *customBtn;
    QLabel *customLabel;

public:
    Dialog(QWidget *parent = 0);
    ~Dialog();

private slots:
    void showFile();
    void showColor();
    void showFont();
    void showInputDlg();
    void showCustom();
};

#endif // DIALOG_H
