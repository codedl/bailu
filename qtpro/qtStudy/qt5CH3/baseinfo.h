#ifndef BASEINFO_H
#define BASEINFO_H
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QTextEdit>
#include <QLayout>
#include <QPushButton>
class BaseInfo : public QWidget
{
public:
    BaseInfo(QWidget *parent=0);
    QLabel *UserNameLable;
    QLineEdit *UserNameLineEdit;
    QLabel *NameLabel;
    QLineEdit *NameLineEdit;
    QLabel *SexLabel;
    QComboBox *SexComboBox;
    QLabel *DepartmentLabel;
    QTextEdit *DepartmentTextEdit;
    QLabel *AgeLabel;
    QLineEdit *AgeLineEdit;
    QLabel *OtherLabel;
    QGridLayout *LeftLayout;

    QLabel *HeadLabel;
    QLabel *HeadIconLabel;
    QPushButton *UpdateBtn;
    QHBoxLayout *TopRightLayout;
    QLabel *IntroduceLabel;
    QTextEdit *IntroduceTextEdit;
    QVBoxLayout *RightLayout;

};

#endif // BASEINFO_H
