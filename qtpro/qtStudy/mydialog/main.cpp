#include "dialog.h"
#include "mydialog.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Dialog w;
    MyDialog m;
    if(m.exec() == QDialog::Accepted){
        w.show();
        return a.exec();
    }
   else return 0;


}
