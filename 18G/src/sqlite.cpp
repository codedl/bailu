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
struct dataDef sysData;

char*  lldtoStringUnit(long long int value,char *result);
bool isSqlite();
QSqlDatabase	openSqlite(QString dbname);
void createTable(QString dbname,QString table);
void createTable(QString dbname);
void getFactoryCalibrationData(void);
void saveDatatoTable(QString dbname,QString table);
void getDatafromTable(QString dbname, QString table);
void saveFactoryCalibrationData(void);


QString db = "my.db";
int main(int argc, char ** argv)
{
    QApplication a( argc, argv );
	
	
	bool ok = isSqlite();
    if(ok){
		createTable(db,"factory");//出厂校准数据
		createTable(db,"prefactory");//前置放大开的出厂校准数据
		createTable(db,"zc");//直采校准数据
		createTable(db,"prezc");//前置放大开的直采校准数据
		createTable(db,"userdata");//用户校准数据
		createTable(db);//保存温度等数据的表
		
		getFactoryCalibrationData();
		saveDatatoTable(db,"factory");
		getDatafromTable(db,"factory");

    }
	               

#if 0
    printf("\nnow enter main codes(v%d.%d.%d.%d.%d)\n", VER_MAJOR, VER_MINOR, VER_REVISION, \
           VER_BUILD, VER_TAG);

    //print message
    if (__DEBUG)
    {
        printf("\nnow enter main codes(v%d.%d.%d.%d.%d)\n", VER_MAJOR, VER_MINOR, VER_REVISION, VER_BUILD, VER_TAG);
    }

    //system text code
    QTextCodec *codec = QTextCodec::codecForName("GBK");
    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForTr(codec);

    //lcd size
    SCREENWIDTH = a.desktop()->width();
    SCREENHEIGHT = a.desktop()->height();
    //SCREENWIDTH = 800;
    //SCREENHEIGHT = 600;

    //system main form
    mainForm mf;
    mf.hide();

    //system main thread
    mainThread mt;
    mt.sysScpi = mf.sysScpi;
    mt.setStackSize(1024 * 1024 * 2);
    QObject::connect(&mt, SIGNAL(drawTop()), &mf, SLOT(drawTop()));
    QObject::connect(&mt, SIGNAL(drawCanvas()), &mf, SLOT(drawCanvas()));
    QObject::connect(&mt, SIGNAL(drawHelp()), &mf, SLOT(drawHelp()));
    QObject::connect(&mt, SIGNAL(drawButton()), &mf, SLOT(drawButton()));
    QObject::connect(&mt, SIGNAL(drawState()), &mf, SLOT(drawState()));
    QObject::connect(&mt, SIGNAL(drawAll()), &mf, SLOT(drawAll()));
    mt.start();

    //system minor thread
    minorThread nt;
    nt.sysScpi = mf.sysScpi;
    nt.setStackSize(1024 * 1024 * 2);
    QObject::connect(&nt, SIGNAL(drawTop()), &mf, SLOT(drawTop()));
    QObject::connect(&nt, SIGNAL(drawState()), &mf, SLOT(drawState()));
    QObject::connect(&nt, SIGNAL(drawInput()), &mf, SLOT(drawInput()));
    nt.start();

    //system data thread
    dataThread dt;
    dt.sysScpi = mf.sysScpi;
    dt.setStackSize(1024 * 1024 * 4);
    dt.start();

    //fft thread
    fftThread fourier;
    fourier.sysScpi = mf.sysScpi;
    fourier.setStackSize(1024 * 1024 * 2);
    fourier.start();

    //server socket thread
    tSocketThread net;
    net.setStackSize(1024 * 1024 * 2);
    QObject::connect(&net, SIGNAL(clientSocketDataArrive(int, int)), mf.sysScpi, SLOT(clientSocketDataArrive(int, int)));
    net.start();


    printf("main codes over, program start successed\n");
#endif
    //program loop
    return a.exec();
}

char*  lldtoStringUnit(long long int value,char *result)
{
    char temp[32] ;
    long long int unitM = value;
    double unitG = value;
    if(1e6 < value && value < 1e9)
    {
        unitM = unitM / 1e6;
        sprintf(temp, "%lldMHz", unitM);

    }
    else
    {
        unitG = unitG / 1e9;
        sprintf(temp, "%.2fGHz", unitG);
    }
	strcpy(result, temp);
    return result;
}

bool isSqlite(void){
	QStringList drivers = QSqlDatabase::drivers();//读取系统数据库驱动
    foreach(QString driver, drivers){
        qDebug() << driver;
    }
	if(drivers.contains("QSQLITE"))//是否支持sqlite
		return true;
	else
		return false;
}

/*
@dbname 数据库
@table  表的名字
返回值: 建表成功与否
*/
void createTable(QString dbname,QString table){

	QSqlDatabase db = openSqlite(dbname);
	if(db.tables().contains(table)){
		db.close();
		db = QSqlDatabase::database("", false);
		QSqlDatabase::removeDatabase("qt_sql_default_connection");
		return;//数据库已存在
	}else{
	QSqlTableModel *model = new QSqlTableModel(NULL,db);//创建数据库模型
    QSqlQuery *query = new QSqlQuery(db);//执行SQL语言的接口
    query->exec(QString("create table %1 (id int primary key,freq varchar, \
               rf0 double, rf10 double, rf20 double, rf30 double)").arg(table));
	char freqString[32];
    model->setTable(table);
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    model->select();
	int rows = sizeof FREQRESPHZ / sizeof FREQRESPHZ[0];
    for(int i=0; i< rows; i++)
    {
        model->insertRow(i);
        model->setData(model->index(i,0),i);//设置键值
        model->setData(model->index(i,1),lldtoStringUnit(FREQRESPHZ[i],freqString));
    }
	model->database().transaction();//开始事务
	if(model->submitAll())//是否可以向数据提交
    	model->database().commit();//提交修改
    model->select();
    while(model->canFetchMore())
        model->fetchMore();
	qDebug()<<"insert:" << model->rowCount() << "rows:" << rows;

	delete(model);
	model = NULL;
	delete(query);
	query = NULL;
	db.close();
	db = QSqlDatabase::database("", false);
	QSqlDatabase::removeDatabase("qt_sql_default_connection");
	}

}

/*创建保存温度,绝对幅度,中频的表
@dbname		数据库名
*/
void createTable(QString dbname){
	QSqlDatabase db = openSqlite(dbname);
	if(db.tables().contains("otherdata")){
		db.close();
		db = QSqlDatabase::database("", false);
		QSqlDatabase::removeDatabase("qt_sql_default_connection");
		return;
	}else{
		QSqlTableModel *model = new QSqlTableModel(NULL,db);//创建数据库模型
		QSqlQuery *query = new QSqlQuery(db);//执行SQL语言的接口
		query->exec("create table otherdata (id int primary key,factory double,\
				   prefactory double, zc double, prezc double, userdata double)");
		model->setTable("otherdata");
		model->setEditStrategy(QSqlTableModel::OnManualSubmit);
		model->select();
		int rows =1 + 1 + 1 + 32;//保存校准状态,温度,绝对幅度,中频增益
		for(int i=0; i< rows; i++)
		{
			model->insertRow(i);
			model->setData(model->index(i,0),i);//设置键值，根据键值搜索
			
		}
		model->database().transaction();//开始事务
		if(model->submitAll())//是否可以向数据提交
			model->database().commit();//提交修改
		model->select();
		while(model->canFetchMore())
			model->fetchMore();
		qDebug()<<"insert:" << model->rowCount() << "rows:" << rows;
		delete(model);
		model = NULL;
		delete(query);
		query=NULL;
		db.close();
		db = QSqlDatabase::database("", false);
		QSqlDatabase::removeDatabase("qt_sql_default_connection");
	}

}

/*
保存数据到表
@dbname		数据库名
@table 		保存数据的表
*/
void saveDatatoTable(QString dbname,QString table)
{
	QSqlDatabase db = openSqlite(dbname);
	
	QSqlTableModel *model = new QSqlTableModel(NULL,db);//创建数据库模型
	model->setTable("otherdata");
	model->setEditStrategy(QSqlTableModel::OnManualSubmit);
	model->select();

	int column;
	int rows = sizeof FREQRESPHZ / sizeof FREQRESPHZ[0];
	bool iscaled = false;
	double absoluteAmptValue = 0;
	double temperature = 0;;
	double attIf[32];
	double freqResp[4][FREQRESPCOUNT];
	//判断保存到到哪个表
	if(table == "factory")
	{
		column = 1;
		iscaled = sysData.factoryCalData.isCaled;
		absoluteAmptValue = sysData.factoryCalData.absoluteAmptValue;
		temperature = sysData.factoryCalData.temperature;
		memcpy(attIf,sysData.factoryCalData.attIf,sizeof attIf);
		memcpy(freqResp, sysData.factoryCalData.freqResp, sizeof freqResp);
	}
	else if(table == "prefactory")
	{
		column = 2;
		iscaled = sysData.preamplifierCalData.isCaled;
		absoluteAmptValue = sysData.preamplifierCalData.absoluteAmptValue;
		temperature = sysData.preamplifierCalData.temperature;
		memcpy(attIf,sysData.preamplifierCalData.attIf,sizeof attIf);
		memcpy(freqResp, sysData.preamplifierCalData.freqResp, sizeof freqResp);
	}
	else if(table == "zc")
	{
		column = 3;	
	}
	else if(table == "prezc")
	{
		column = 4;
	}
	else if(table == "userdata")
	{
		column = 5;
	}
	else 
		return;	

	//校准状态
	model->setData(model->index(0,column), iscaled);
	//绝对幅度值
	model->setData(model->index(1,column), absoluteAmptValue);
	//温度
	model->setData(model->index(2,column), temperature);
	for(int i=0; i<32; i++){
		model->setData(model->index(i+3,column),attIf[i]);
	}
	model->database().transaction();//开始事务
	if(model->submitAll())
		if(model->database().commit())//提交改变到数据库
		qDebug() << "data commit";
		
	model->setTable("factory");
	model->select();
	while(model->canFetchMore())
        model->fetchMore();
	
	for(int i=0; i<4; i++){
		for(int j=0; j<rows; j++){
			model->setData(model->index(j,i+2), freqResp[i][j]);
		}
	}
	

	model->database().transaction();//开始事务
	if(model->submitAll())//是否可以向数据提交
    	model->database().commit();//提交修改

		
	system("cp my.db temp.db -r");//数据库备份

	//关闭数据库，释放内存
	delete(model);
	model = NULL;
	db.close();
	db = QSqlDatabase::database("", false);
	QSqlDatabase::removeDatabase("qt_sql_default_connection");

}

void getDatafromTable(QString dbname, QString table)
{
	memset(&sysData.factoryCalData,0,sizeof sysData.factoryCalData);
	QSqlDatabase db = openSqlite(dbname);
	
	QSqlTableModel *model = new QSqlTableModel(NULL,db);//创建数据库模型
	model->setTable("otherdata");
	model->setEditStrategy(QSqlTableModel::OnManualSubmit);
	model->select();
    while(model->canFetchMore())
        model->fetchMore();
	qDebug()<<"rows:" << model->rowCount() ;

	int column;
	int rows = sizeof FREQRESPHZ / sizeof FREQRESPHZ[0];
	//判断查询哪个表
	if(table == "factory")
	{
		column = 1;
		sysData.factoryCalData.isCaled = model->data(model->index(0,1)).toBool();
		sysData.factoryCalData.absoluteAmptValue = model->data(model->index(1,1)).toDouble();
		sysData.factoryCalData.temperature = model->data(model->index(2,1)).toDouble();

		for(int i=0; i<32; i++){
			sysData.factoryCalData.attIf[i] = model->data(model->index(i+3,1)).toDouble();

		}
		model->setTable("factory");
		model->setEditStrategy(QSqlTableModel::OnManualSubmit);
		model->database().transaction();//开始事务
		if(model->submitAll())//是否可以向数据提交
			model->database().commit();//提交修改
		model->select();
		while(model->canFetchMore())
			model->fetchMore();			
		qDebug()<<"rows:" << model->rowCount();
		
		for(int i=0; i<4; i++){
			for(int j=0; j<rows; j++){
			sysData.factoryCalData.freqResp[i][j]= model->data(model->index(j,i+2)).toDouble();
			}
		}

	}
	else if(table == "prefactory")
	{
		column = 2;
	}
	else if(table == "zc")
	{
		column = 3;	
	}
	else if(table == "prezc")
	{
		column = 4;
	}
	else if(table == "userdata")
	{
		column = 5;
	}
	else 
		return;	

	delete(model);
	model = NULL;
	db.close();
	db = QSqlDatabase::database("", false);
	QSqlDatabase::removeDatabase("qt_sql_default_connection");

	saveFactoryCalibrationData();

}

void getFactoryCalibrationData(void)
{
	//resetFactoryCalibrationData();

	QString jindex,iindex,freq;
	char tempChar[32] = { };
	int index = sizeof FREQRESPHZ / sizeof FREQRESPHZ[0];
	QString fileName = QCoreApplication::applicationDirPath() + "/setting.ini";

	if (QFile(fileName).exists())
	{
		QSettings setting(fileName, QSettings::IniFormat);

		setting.beginGroup("factoryData");

		sysData.factoryCalData.isCaled = setting.value("fa_isCaled", false).toBool();
		sysData.factoryCalData.isCaled =true;

		if (sysData.factoryCalData.isCaled)
		{
			sysData.factoryCalData.absoluteAmptValue = setting.value("fa_absoluteAmptValue", 0).toDouble();
			sysData.factoryCalData.temperature = setting.value("fa_temperature", 0).toDouble();

			for(int i = 0; i < 32; i++)
			{
				sysData.factoryCalData.attIf[i] = setting.value("fa_attIf_" + QString(intToString(i, tempChar)).trimmed(), 0).toDouble();
			}
			for (int i = 0; i < 4; i++)
			{
				iindex = QString(intToString(i,tempChar)).trimmed();
				for (int j = 0; j < index; j++)
				{
					freq = QString(lldtoStringUnit(FREQRESPHZ[j], tempChar)).trimmed();
					jindex = QString(intToString(j,tempChar)).trimmed();				
					sysData.factoryCalData.freqResp[i][j] = setting.value("fa_" + iindex + "_" + freq, 0).toDouble();
				}
			}
			
		}

		setting.endGroup();
	}
}


//保存出厂校准数据
void saveFactoryCalibrationData(void)
{
	char tempChar[32] = { };
	int index = sizeof FREQRESPHZ / sizeof FREQRESPHZ[0]; 
	QString jindex,iindex,freq;
	QString fileName = QCoreApplication::applicationDirPath() + "/data.ini";
	QSettings setting(fileName, QSettings::IniFormat);

	setting.beginGroup("factoryData");
	setting.setValue("fa_isCaled", sysData.factoryCalData.isCaled);
	setting.setValue("fa_absoluteAmptValue", sysData.factoryCalData.absoluteAmptValue);
	setting.setValue("fa_temperature", sysData.factoryCalData.temperature);

	for(int i = 0; i < 32; i++)
	{
		setting.setValue("fa_attIf_" + QString(intToString(i, tempChar)).trimmed(), sysData.factoryCalData.attIf[i]);
	}

	for (int i = 0; i < 4; i++)
	{
		iindex = QString(intToString(i,tempChar)).trimmed();
		for (int j = 0; j < index; j++)
		{
			freq = QString(lldtoStringUnit(FREQRESPHZ[j], tempChar)).trimmed();
			jindex = QString(intToString(j,tempChar)).trimmed();
			setting.setValue("fa_" + iindex + "_" + freq+jindex, sysData.factoryCalData.freqResp[i][j]);
		}
	}

	setting.endGroup();
}




//整型转化为字符串
char* intToString(int value, char* result)
{
  char temp[32];
  sprintf(temp, "%d", value);
  strcpy(result, temp);

  return result;
}

/*
打开数据库,没有则创建数据库
@dbname		数据库名
*/
QSqlDatabase	openSqlite(QString dbname){

	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName(dbname);
	if(db.open()){
		return db;
	}else{
		return openSqlite("temp.db");//打开失败使用备份数据库
	}
}




