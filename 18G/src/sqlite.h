#include <QtCore/QTextCodec>
#include <QtGui/QDesktopWidget>
#include "main/mainform.h"
#include "main/mainthread.h"
#include "common/global.h"
#include <QtGui/QApplication>
#include <QSqlDatabase>
#include <QDebug>
#include <QSqlQuery>
#include <QSqlDriver>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>
#include <QStringList>
#include <QSqlTableModel>
#include <stdlib.h>
#include "common/sysstruct.h"

char*  lldtoStringUnit(long long int value,char *result);
bool isSqlite();
QSqlDatabase	openSqlite(QString dbname);
void createTable(QString dbname,QString table);
void createTable(QString dbname);
void saveDatatoTable(QString dbname,QString table);
void getDatafromTable(QString dbname, QString table);
