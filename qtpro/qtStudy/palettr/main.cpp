#include "palette.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    palette w;
    w.show();

    return a.exec();
}
