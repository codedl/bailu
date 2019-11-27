#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtWinExtras>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->horizontalScrollBar->setStyle(QStyleFactory::create("fusion"));

//    QPalette palette = ui->pushButton->palette();
//    palette.setColor(QPalette::ButtonText,Qt::red);
//    palette.setColor(QPalette::Button, Qt::blue);
//    ui->pushButton->setPalette(palette);

    ui->spinBox->setDisabled(true);
    QPalette  spinPalette =  ui->spinBox->palette();
    spinPalette.setColor(QPalette::Disabled,QPalette::Base,Qt::blue);
    ui->spinBox->setPalette(spinPalette);

    ui->horizontalSlider->setStyleSheet("background:yellow");
//    setStyleSheet("QPushButton{background:blue}QSpinBox{background:red}");
//    QPushButton *btn = new QPushButton("blueBtn",this);
    qssFile = new QFile(":/qss/my.qss",this);
    qssFile->open(QFile::ReadOnly);
    QString style = tr(qssFile->readAll());
    qApp->setStyleSheet(style);
    qssFile->close();

    QPixmap pix(":/image/image/pix.png");
    ui->label->setPixmap(pix);
    ui->label->setMask(pix.mask());

    //setWindowFlags(Qt::FramelessWindowHint);
   // setAttribute(Qt::WA_TranslucentBackground);
//    QPixmap pix;
//    pix.load(":/image/image/pix.png");
//    resize(pix.size());
//    setMask(pix.mask());
    QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect(this);
    effect->setColor(QColor(100,100,100,100));
    effect->setBlurRadius(2);
    effect->setOffset(10);
    ui->label->setGraphicsEffect(effect);

    if(QtWin::isCompositionEnabled()){
        QtWin::extendFrameIntoClientArea(this,-1,-1,-1,-1);
        setAttribute(Qt::WA_TranslucentBackground,true);
        setAttribute(Qt::WA_NoSystemBackground,false);
        setStyleSheet("MainWindow{background:transparent}");
    }else{
        QtWin::resetExtendedFrame(this);
        setAttribute(Qt::WA_TranslucentBackground,false);
        setStyleSheet(QString("MainWindow{background:%1}").arg(QtWin::realColorizationColor().name()));
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    if(qssFile->fileName() == ":/qss/my.qss")
        qssFile->setFileName(":/qss/my1.qss");
    else qssFile->setFileName(":/qss/my.qss");
    qssFile->open(QFile::ReadOnly);
    QString style = (qssFile->readAll());
    qApp->setStyleSheet(style);
    qssFile->close();
}

void MainWindow::paintEvent(QPaintEvent *)
{
//    QPainter painter(this);
//    painter.drawPixmap(0,0,QPixmap(":/image/image/pix.png"));
    QPainter painier(this);
    painier.fillRect(rect(),QColor(255,255,255,100));
}

void MainWindow::mousePressEvent(QMouseEvent *)
{
    close();
}
