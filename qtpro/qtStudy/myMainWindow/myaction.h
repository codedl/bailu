#ifndef MYACTION_H
#define MYACTION_H
#include <QWidgetAction>
#include <QLineEdit>
#include <QSplitter>
#include <QLabel>
#include <QDebug>
class  QLineEdit;

class myaction : public QWidgetAction
{
    Q_OBJECT
public:
    explicit myaction(QObject* parent = 0);

protected:
    QWidget * createWidget(QWidget *parent);

signals:
    void getText(const QString &string);
private slots:
    void sendText();

private:
    QLineEdit *lineEdit;
};

#endif // MYACTION_H
