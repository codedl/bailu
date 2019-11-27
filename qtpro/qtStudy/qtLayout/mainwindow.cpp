#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{

    setWindowTitle(tr("layout"));
    QTextEdit *mainEdit = new QTextEdit(this);
    mainEdit->setText(tr("main window"));
    mainEdit->setAlignment(Qt::AlignCenter);
    setCentralWidget(mainEdit);

    QDockWidget *dock = new QDockWidget(tr("dockwidget1"),this);
    dock->setFeatures(QDockWidget::DockWidgetMovable);
    dock->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
    QTextEdit *edit1 = new QTextEdit();
    edit1->setText(tr("window1,this dock widget can be moved"));
    dock->setWidget(edit1);
    addDockWidget(Qt::RightDockWidgetArea, dock);

    dock = new QDockWidget(tr("dockwidget2"),this);
    dock->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetFloatable);
    //    dock->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
    QTextEdit *edit2 = new QTextEdit();
    edit2->setText(tr("window2,this dock widget can be detached from main window,floated as an independent window,and can be closed"));
    dock->setWidget(edit2);
    addDockWidget(Qt::RightDockWidgetArea, dock);

    dock = new QDockWidget(tr("dockwidget3"),this);
    dock->setFeatures(QDockWidget::AllDockWidgetFeatures);
    //    dock->setAllowedAreas(Qt::LeftDockWidgetArea|Qt::RightDockWidgetArea);
    QTextEdit *edit3 = new QTextEdit();
    edit3->setText(tr("window2,this dock widget can be closed,moved,and floated"));
    dock->setWidget(edit3);
    addDockWidget(Qt::RightDockWidgetArea, dock);



}

MainWindow::~MainWindow()
{

}
