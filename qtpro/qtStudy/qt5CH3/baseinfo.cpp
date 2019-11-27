#include "baseinfo.h"

BaseInfo::BaseInfo(QWidget *parent) : QWidget(parent)
{
    UserNameLable = new QLabel(tr("用户名:"));
    UserNameLineEdit = new QLineEdit(this);
    NameLabel = new QLabel(tr("姓名:"));
    NameLineEdit = new QLineEdit(this);
    SexLabel = new QLabel(tr("性别:"));
    SexComboBox = new QComboBox(this);
    SexComboBox->addItem(tr("男"));
    SexComboBox->addItem(tr("女"));
    DepartmentLabel = new QLabel(tr("部门:"));
    DepartmentTextEdit = new QTextEdit(this);
    AgeLabel = new QLabel(tr("年龄:"));
    AgeLineEdit = new QLineEdit(this);
    OtherLabel = new QLabel(tr("备注:"));
    OtherLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    LeftLayout = new QGridLayout(this);
    LeftLayout->addWidget(UserNameLable,0,0);
    LeftLayout->addWidget(UserNameLineEdit,0,1);
    LeftLayout->addWidget(NameLabel,1,0);
    LeftLayout->addWidget(NameLineEdit,1,1);
    LeftLayout->addWidget(SexLabel,2,0);
    LeftLayout->addWidget(SexComboBox,2,1);
    LeftLayout->addWidget(DepartmentLabel,3,0);
    LeftLayout->addWidget(DepartmentTextEdit,3,1);
    LeftLayout->addWidget(AgeLabel,4,0);
    LeftLayout->addWidget(AgeLineEdit,4,1);
    LeftLayout->addWidget(OtherLabel,5,0,1,2);
    LeftLayout->setColumnStretch(0,1);
    LeftLayout->setColumnStretch(1,3);

    HeadLabel = new QLabel(tr("头像:"));
    HeadIconLabel = new QLabel;
    QPixmap pix("123.png");
    HeadIconLabel->setPixmap(pix);
    HeadIconLabel->resize(pix.width(),pix.height());
    UpdateBtn = new QPushButton(tr("更新"));
    TopRightLayout = new QHBoxLayout();
    TopRightLayout->setSpacing(20);
    TopRightLayout->addWidget(HeadLabel);
    TopRightLayout->addWidget(HeadIconLabel);
    TopRightLayout->addWidget(UpdateBtn);
    IntroduceLabel = new QLabel(tr("个人说明:"));
    IntroduceTextEdit = new QTextEdit;
    RightLayout = new QVBoxLayout;
    RightLayout->setMargin(15);
    RightLayout->addLayout(TopRightLayout);
    RightLayout->addWidget(IntroduceLabel);
    RightLayout->addWidget(IntroduceTextEdit);

    QGridLayout *mainLayout = new QGridLayout(this);
    mainLayout->setMargin(15);
    mainLayout->setSpacing(10);
    mainLayout->addLayout(LeftLayout,0,0);
    mainLayout->addLayout(RightLayout,0,1);
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
}
