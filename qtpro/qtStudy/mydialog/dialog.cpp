#include "dialog.h"
#include "ui_dialog.h"
#include "mydialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
//    QDialog *dialog = new QDialog(this);
//    dialog->setModal(true);
//    dialog->show();


}

Dialog::~Dialog()
{
    delete ui;
}

QWizardPage *Dialog::create1()
{
    QWizardPage * w = new QWizardPage;
    w->setTitle(tr("create1"));
    return w;
}

QWizardPage *Dialog::create2()
{
    QWizardPage * w = new QWizardPage;
    w->setTitle(tr("create2"));
    return w;
}

QWizardPage *Dialog::create3()
{
    QWizardPage * w = new QWizardPage;
    w->setTitle(tr("create3"));
    return w;
}

void Dialog::on_pushButton_clicked()
{
    QDialog *a = new QDialog(this);
    a->show();
}

void Dialog::on_pushButton_2_clicked()
{
    close();
    MyDialog m;
    if(m.exec() == QDialog::Accepted)
        show();
}

void Dialog::on_pushButton_4_clicked()
{
    QWizard wizard(this) ;
    wizard.setWindowTitle(tr("wizard dialog"));
    wizard.addPage(create1());
    wizard.addPage(create2());
    wizard.addPage(create3());
    wizard.exec();

}
