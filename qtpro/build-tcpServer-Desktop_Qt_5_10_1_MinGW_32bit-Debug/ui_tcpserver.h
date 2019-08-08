/********************************************************************************
** Form generated from reading UI file 'tcpserver.ui'
**
** Created by: Qt User Interface Compiler version 5.10.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TCPSERVER_H
#define UI_TCPSERVER_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>

QT_BEGIN_NAMESPACE

class Ui_tcpserver
{
public:

    void setupUi(QDialog *tcpserver)
    {
        if (tcpserver->objectName().isEmpty())
            tcpserver->setObjectName(QStringLiteral("tcpserver"));
        tcpserver->resize(400, 300);

        retranslateUi(tcpserver);

        QMetaObject::connectSlotsByName(tcpserver);
    } // setupUi

    void retranslateUi(QDialog *tcpserver)
    {
        tcpserver->setWindowTitle(QApplication::translate("tcpserver", "tcpserver", nullptr));
    } // retranslateUi

};

namespace Ui {
    class tcpserver: public Ui_tcpserver {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TCPSERVER_H
