#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QKeyEvent>
#include <QDebug>
#include <QTime>
#include <QTimer>
#include <QWheelEvent>
namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();
    bool eventFilter(QObject *obj, QEvent *event);

private:
    Ui::Widget *ui;
    bool keyUp;
    bool keyLeft;
    bool move;
    int id1, id2, id3;
protected:
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void timerEvent(QTimerEvent *timer);
private slots:
    void timerUpdate();
};

#endif // WIDGET_H
