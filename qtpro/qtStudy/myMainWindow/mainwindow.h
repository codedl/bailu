#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QToolButton>
#include <QSpinBox>
#include <QTextEdit>
#include <QMdiSubWindow>
#include <QLabel>
#include <QTextFrame>
#include <QLineEdit>
#include <QDialog>
#include <QPushButton>
#include <QVBoxLayout>
#include <QDragEnterEvent>
#include <QUrl>
#include <QFile>
#include <QTextStream>
#include <QMimeData>
#include <QMouseEvent>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QPainter>
#include <QDrag>
#include <QPrintDialog>
#include <QPrinter>
#include <QPrintPreviewDialog>
#include <QFileDialog>
#include <QFileInfo>
#include "mysyntaxhighlighter.h"
class QLineEdit;
class QDialog;
class mysyntaxhighlighter;
class QPrinter;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
protected:
//    void dragEnterEvent(QDragEnterEvent *event);
//    void dropEvent(QDropEvent *event);
    void mousePressEvent(QMouseEvent *event);      // 鼠标按下事件
    void dragEnterEvent(QDragEnterEvent *event);   // 拖动进入事件
    void dragMoveEvent(QDragMoveEvent *event);     // 拖动事件
    void dropEvent(QDropEvent *event);             // 放下事件
private slots:
    void on_action_New_triggered();

    void on_action_triggered();
    void setText(const QString &string);
    void showTextFrame();
    void showTextBlock();
    void setTextFont(bool checked);
    void insertList();
    void insertTable();
    void insertImage();
    void textFind();
    void findNext();
    void doPrint();
    void doPrintPreview();
    void PrintPreview(QPrinter *printer);
    void createPdf();
private:
    Ui::MainWindow *ui;
    QLineEdit *lineEdit;
    QDialog *dialog;
    mysyntaxhighlighter *lighter;
};

#endif // MAINWINDOW_H
