#include "dialog.h"

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("对话框实例"));
    mainLayout = new QGridLayout(this);

    fileBtn = new QPushButton(tr("文件对话框"));
    fileLineEdit = new QLineEdit;
    mainLayout->addWidget(fileBtn,0,0);
    mainLayout->addWidget(fileLineEdit,0,1);
    connect(fileBtn,SIGNAL(clicked()),this,SLOT(showFile()));

    colorBtn = new QPushButton(tr("颜色对话框"));
    colorFrame = new QFrame(this);
    colorFrame->setFrameShape(QFrame::Box);
    colorFrame->setAutoFillBackground(true);
    mainLayout->addWidget(colorBtn,1,0);
    mainLayout->addWidget(colorFrame,1,1);
    connect(colorBtn,SIGNAL(clicked()),this,SLOT(showColor()));

    fontBtn = new QPushButton(tr("字体对话框"));
    fontLineEdit = new QLineEdit;
    fontLineEdit->setText(tr("字体"));
    mainLayout->addWidget(fontBtn,2,0);
    mainLayout->addWidget(fontLineEdit,2,1);
    connect(fontBtn,SIGNAL(clicked()),this,SLOT(showFont()));

    inputBtn=new QPushButton;                                   //创建控件的对象
    inputBtn->setText(tr("输入对话框"));
    mainLayout->addWidget(inputBtn,3,0);                        //布局设计
    connect(inputBtn,SIGNAL(clicked()),this,SLOT(showInputDlg()));//事件关联

    customBtn = new QPushButton(tr("自定义对话框"));
    customLabel = new QLabel;
    customLabel->setFrameStyle(QFrame::Panel| QFrame::Sunken);
    mainLayout->addWidget(customBtn,4,0);
    mainLayout->addWidget(customLabel,4,1);
    connect(customBtn,SIGNAL(clicked()),this,SLOT(showCustom()));
}

void Dialog::showFile(){
    QString file = QFileDialog::getOpenFileName(this,"文件","/","C++ files(*.cpp) ;; C files"
                                                              "(*.c);; head files(*.h)");
    fileLineEdit->setText(file);
}

void Dialog::showColor(){
    QColor color = QColorDialog::getColor(Qt::blue);
    if(color.isValid()){
        colorFrame->setPalette(QPalette(color));
    }
}

void Dialog::showFont(){
    bool ok;
    QFont font = QFontDialog :: getFont(&ok);
    if(ok){
        fontLineEdit->setFont(font);
    }
}

void Dialog::showInputDlg()
{
    inputDlg =new InputDlg(this);
    inputDlg->show();
}
void Dialog::showCustom(){
    customLabel->setText(tr("自定义对话框"));
    QMessageBox customBox;
    QPushButton *yesBtn = customBox.addButton(tr("yes"),QMessageBox::ActionRole);
    QPushButton *noBtn = customBox.addButton(tr("no"),QMessageBox::ActionRole);
    QPushButton *cancelBtn = customBox.addButton(QMessageBox::Cancel);
    customBox.setText(tr("setText"));
    customBox.exec();
    if(customBox.clickedButton() == yesBtn)
        customLabel->setText(customLabel->text()+tr("yes"));
    if(customBox.clickedButton() == noBtn)
        customLabel->setText(customLabel->text()+tr("no"));
    if(customBox.clickedButton() == cancelBtn)
        customLabel->setText(customLabel->text()+tr("cancel"));
}
Dialog::~Dialog()
{

}
