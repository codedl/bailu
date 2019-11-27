#ifndef PALETTE_H
#define PALETTE_H

#include <QDialog>
#include <QLabel>
#include <QComboBox>
#include <QTextEdit>
#include <QLayout>
class palette : public QDialog
{
    Q_OBJECT

public:
    palette(QWidget *parent = 0);
    ~palette();
    void createLeft();
    void createRight();
    void fillcolorlist(QComboBox *comboBox);
private slots:
    void ShowWindow();
//    void ShowWindowText();
//    void ShowButton();
//    void ShowButtonText();
//    void ShowBase();
private:
    QFrame *leftFrame;
    QLabel *windowLabel;
    QComboBox *windowComboBox;
    QLabel *windowtextLabel;
    QComboBox *windowtextComboBox;
    QLabel *btnLabel;
    QComboBox *btnComboBox;
    QLabel *btnTextLabel;
    QComboBox *btnTextComboBox;
    QLabel *baseLabel;
    QComboBox *baseComboBox;

    QFrame *rightFrame;
    QLabel *valueLabel;
    QComboBox *valueComboBox;
    QLabel *stringLabel;
    QLineEdit *stringLineEdit;
    QTextEdit *textedit;
    QPushButton *okBtn;
    QPushButton *cancelBtn;

};

#endif // PALETTE_H
