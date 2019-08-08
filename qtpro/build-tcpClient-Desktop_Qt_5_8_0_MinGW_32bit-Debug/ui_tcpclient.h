/********************************************************************************
** Form generated from reading UI file 'tcpclient.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TCPCLIENT_H
#define UI_TCPCLIENT_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QHeaderView>

QT_BEGIN_NAMESPACE

class Ui_tcpClient
{
public:

    void setupUi(QDialog *tcpClient)
    {
        if (tcpClient->objectName().isEmpty())
            tcpClient->setObjectName(QStringLiteral("tcpClient"));
        tcpClient->resize(400, 300);

        retranslateUi(tcpClient);

        QMetaObject::connectSlotsByName(tcpClient);
    } // setupUi

    void retranslateUi(QDialog *tcpClient)
    {
        tcpClient->setWindowTitle(QApplication::translate("tcpClient", "tcpClient", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class tcpClient: public Ui_tcpClient {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TCPCLIENT_H
