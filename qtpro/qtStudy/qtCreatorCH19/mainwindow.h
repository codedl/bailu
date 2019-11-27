#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QDebug>
#include <QTextCodec>
#include <QSharedMemory>
#include <QFileDialog>
#include <QBuffer>
#include <QFileDialog>
#include "mythread.h"
#include <QtCore>
#include <stdlib.h>
#include <stdio.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();
    void showResult();
    void showChanged(QProcess::ProcessState state);
    void showError(QProcess::ProcessError error);
    void showFinished(int , QProcess::ExitStatus );
    void loadFromFile();
    void loadFromShared();
    void on_loadBtn_clicked();
    void on_displayBtn_clicked();
    void on_startBtn_clicked();
    void on_stopBtn_clicked();

private:
    Ui::MainWindow *ui;
    QProcess process;
    QSharedMemory sharedMemory;
    MyThread thread;
    void detach();
};

#endif // MAINWINDOW_H
