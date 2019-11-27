#include "contact.h"

Contact::Contact(QWidget *parent) :  QWidget(parent)
{
    EmailLabel = new QLabel(tr("电子邮件:"));
    EmailLineEdit = new QLineEdit;
    AddressLabel = new QLabel(tr("联系地址:"));
    AddressLineEdit = new QLineEdit;
    CodeLabel = new QLabel(tr("邮政编码:"));
    CodeLineEdit = new QLineEdit;
    MobileLabel = new QLabel(tr("移动电话:"));
    MobileLineEdit = new QLineEdit;
    MobileCheckBox = new QCheckBox(tr("接收留言"));
    OfficeLabel = new QLabel(tr("办公电话:"));
    OfficeLineEdit = new QLineEdit;

    mainLayout = new QGridLayout(this);
    mainLayout->setMargin(15);
    mainLayout->setSpacing(10);
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    mainLayout->addWidget(EmailLabel,0,0);
    mainLayout->addWidget(EmailLineEdit,0,1);
    mainLayout->addWidget(AddressLabel,1,0);
    mainLayout->addWidget(AddressLineEdit,1,1);
    mainLayout->addWidget(CodeLabel,2,0);
    mainLayout->addWidget(CodeLineEdit,2,1);
    mainLayout->addWidget(MobileLabel,3,0);
    mainLayout->addWidget(MobileLineEdit,3,1);
    mainLayout->addWidget(MobileCheckBox,3,2);
    mainLayout->addWidget(OfficeLabel,4,0);
    mainLayout->addWidget(OfficeLineEdit,4,1);

}
