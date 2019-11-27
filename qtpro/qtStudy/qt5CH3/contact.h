#ifndef CONTACT_H
#define CONTACT_H
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QLayout>
class Contact : public QWidget
{
    Q_OBJECT
public:
    Contact(QWidget *parent = 0);
    QLabel *EmailLabel;
    QLineEdit *EmailLineEdit;
    QLabel *AddressLabel;
    QLineEdit *AddressLineEdit;
    QLabel *CodeLabel;
    QLineEdit *CodeLineEdit;
    QLabel *MobileLabel;
    QLineEdit *MobileLineEdit;
    QCheckBox *MobileCheckBox;
    QLabel *OfficeLabel;
    QLineEdit *OfficeLineEdit;
    QGridLayout *mainLayout;
};

#endif // CONTACT_H
