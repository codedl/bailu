#ifndef CONTENT_H
#define CONTENT_H
#include <QStackedWidget>
#include <QPushButton>
#include <QLayout>
#include "baseinfo.h"
#include "contact.h"
#include "detail.h"
class Content:public QFrame
{
    Q_OBJECT
public:
    Content(QWidget *parent = 0);
    QStackedWidget *stack;
    QPushButton *AmendBtn;
    QPushButton *CloseBtn;
    BaseInfo *baseInfo;
    Contact *contact;
    Detail *detail;

};

#endif // CONTENT_H
