#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QPainter>
#include <QRect>
#include <QPixmap>
#include <QColor>
#include <QDebug>


namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

private:
    Ui::Widget *ui;
    void paintEvent(QPaintEvent *);
    QPainter *p;

};

#endif // WIDGET_H
