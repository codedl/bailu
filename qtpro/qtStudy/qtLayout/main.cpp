#include "mainwindow.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setFont(QFont("ZYSong18030",12));
    QSplitter *main = new QSplitter(Qt::Horizontal,0);
    QTextEdit *edit = new QTextEdit(QObject::tr("left widget"),main);
    edit->setAlignment(Qt::AlignCenter);
    QSplitter *right = new QSplitter(Qt::Vertical,main);
    right->setOpaqueResize(false);
    QTextEdit *up = new QTextEdit(QObject::tr("Top widget"),right);
    up->setAlignment(Qt::AlignCenter);
    QTextEdit *bottom = new QTextEdit(QObject::tr("bottom widget"),right);
    bottom->setAlignment(Qt::AlignCenter);
    main->setStretchFactor(1,1);
    main->setWindowTitle("layout");
    main->show();

    MainWindow w;
    w.show();

    return a.exec();
}
