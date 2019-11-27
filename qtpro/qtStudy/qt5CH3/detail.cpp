#include "detail.h"

Detail::Detail(QWidget *parent) : QWidget(parent)
{
    NationLabel = new QLabel(tr("国家/地区:"));
    NationComboBox = new QComboBox;
    NationComboBox->insertItem(0,tr("中国"));
    NationComboBox->insertItem(1,tr("美国"));
    NationComboBox->insertItem(2,tr("英国"));
    ProvinceLabel = new QLabel(tr("省份:"));
    ProvinceComboBox = new QComboBox;
    ProvinceComboBox->insertItem(0,tr("安徽省"));
    ProvinceComboBox->insertItem(1,tr("江苏省"));
    ProvinceComboBox->insertItem(2,tr("山东省"));
    CityLabel = new QLabel(tr("城市:"));
    CiteLineEdit = new QLineEdit;
    IntroduceLabel = new QLabel(tr("个人说明:"));
    IntroduceTextEdit = new QTextEdit;
    mainLayout = new QGridLayout(this);
    mainLayout->setMargin(15);
    mainLayout->setSpacing(10);
    mainLayout->addWidget(NationLabel,0,0);
    mainLayout->addWidget(NationComboBox,0,1);
    mainLayout->addWidget(ProvinceLabel,1,0);
    mainLayout->addWidget(ProvinceComboBox,1,1);
    mainLayout->addWidget(CityLabel,2,0);
    mainLayout->addWidget(CiteLineEdit,2,1);
    mainLayout->addWidget(IntroduceLabel,3,0);
    mainLayout->addWidget(IntroduceTextEdit,3,1);
}
