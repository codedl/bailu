#ifndef WIDGET_H
#define WIDGET_H
#include "ui_widget.h"
#include <QWidget>
#include <qlabel.h>
#include <qapplication.h>
#include <qpushbutton.h>
#include <qfont.h>
#include <qdatetime.h>
#include <qtimer.h>
namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    QLabel *text;
    explicit Widget(QWidget *parent = 0);
    ~Widget();
private slots:
    void timeoutEvent();
    void treeWidgetChanged(QTreeWidgetItem* widget,int item);
private:
    Ui::Widget *ui;
};

#endif // WIDGET_H
