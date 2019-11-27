#ifndef DETAIL_H
#define DETAIL_H
#include <QWidget>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QTextEdit>
#include <QGridLayout>
class Detail : public QWidget
{
public:
    Detail(QWidget *parent = 0);
    QLabel *NationLabel;
    QComboBox *NationComboBox;
    QLabel *ProvinceLabel;
    QComboBox *ProvinceComboBox;
    QLabel *CityLabel;
    QLineEdit *CiteLineEdit;
    QLabel *IntroduceLabel;
    QTextEdit *IntroduceTextEdit;
    QGridLayout *mainLayout;

};

#endif // DETAIL_H
