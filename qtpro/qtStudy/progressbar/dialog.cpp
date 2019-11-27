#include "dialog.h"

Dialog::Dialog(QWidget *parent)
    : QDialog(parent)
{
    filenumLabel = new QLabel(tr("文件数目:"));
    filenumLineEdit = new QLineEdit;
    filenumLineEdit->setText(tr("100000"));
    displayLabel = new QLabel(tr("显示类型:"));
    displayComboBox = new QComboBox;
    displayComboBox->addItem(tr("progressBar"));
    displayComboBox->addItem(tr("progressDialog"));
    progress = new QProgressBar;
    startBtn = new QPushButton(tr("开始"));

    mainLayout = new QGridLayout(this);
    mainLayout->setMargin(15);
    mainLayout->setSpacing(10);
    mainLayout->addWidget(filenumLabel,0,0);
    mainLayout->addWidget(filenumLineEdit,0,1);
    mainLayout->addWidget(displayLabel,1,0);
    mainLayout->addWidget(displayComboBox,1,1);
    mainLayout->addWidget(progress,2,0,1,2);
    mainLayout->addWidget(startBtn,3,1);

    connect(startBtn,SIGNAL(clicked()),this,SLOT(showProgress()));
}
void Dialog::showProgress(){
    int max = filenumLineEdit->text().toInt();
    if(displayComboBox->currentIndex() == 0){
        progress->setRange(0,max);
        for(int i=0; i<max+1; i++)
            progress->setValue(i);
    }else if(displayComboBox->currentIndex() == 1){
        QProgressDialog *progressDialog = new QProgressDialog(this);
        progressDialog->setMinimumDuration(10);
        progressDialog->setWindowModality(Qt::WindowModal);
        progressDialog->setWindowTitle(tr("进度条对话框"));
        progressDialog->setLabelText(tr("setLebelText"));
        progressDialog->setCancelButtonText(tr("cancels"));
        progressDialog->setRange(0,max);
        for(int i=1; i<max+1; i++){
            progressDialog->setValue(i);
            if(progressDialog->wasCanceled())
                return;
        }
    }
}
Dialog::~Dialog()
{

}
