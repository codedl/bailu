#include "content.h"
#include <QApplication>
#include <QSplitter>
#include <QListWidget>
#include <QObject>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setFont(QFont("AR PL KaitiM GB",12));
    QSplitter *mainSplitter = new QSplitter(Qt::Horizontal,0);
    mainSplitter->setOpaqueResize(true);
    QListWidget *list = new QListWidget(mainSplitter);
    list->addItem("基本信息");
    list->addItem("联系方式");
    list->addItem("详细资料");
    Content *content = new Content(mainSplitter);
    QObject::connect(list,SIGNAL(currentRowChanged(int)),content->stack,SLOT(setCurrentIndex(int)));
    mainSplitter->setMinimumSize(mainSplitter->minimumSize());
    mainSplitter->setMaximumSize(mainSplitter->maximumSize());
    mainSplitter->setWindowTitle(QObject::tr("修改用户资料"));
    mainSplitter->show();

    return a.exec();
}
