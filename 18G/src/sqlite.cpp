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
		createTable(db,"factory");//����У׼����
		createTable(db,"prefactory");//ǰ�÷Ŵ󿪵ĳ���У׼����
		createTable(db,"zc");//ֱ��У׼����
		createTable(db,"prezc");//ǰ�÷Ŵ󿪵�ֱ��У׼����
		createTable(db,"userdata");//�û�У׼����
		createTable(db);//�����¶ȵ����ݵı�
		
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
	QStringList drivers = QSqlDatabase::drivers();//��ȡϵͳ���ݿ�����
    foreach(QString driver, drivers){
        qDebug() << driver;
    }
	if(drivers.contains("QSQLITE"))//�Ƿ�֧��sqlite
		return true;
	else
		return false;
}

/*
@dbname ���ݿ�
@table  �������
����ֵ: ����ɹ����
*/
void createTable(QString dbname,QString table){

	QSqlDatabase db = openSqlite(dbname);
	if(db.tables().contains(table)){
		db.close();
		db = QSqlDatabase::database("", false);
		QSqlDatabase::removeDatabase("qt_sql_default_connection");
		return;//���ݿ��Ѵ���
	}else{
	QSqlTableModel *model = new QSqlTableModel(NULL,db);//�������ݿ�ģ��
    QSqlQuery *query = new QSqlQuery(db);//ִ��SQL���ԵĽӿ�
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
        model->setData(model->index(i,0),i);//���ü�ֵ
        model->setData(model->index(i,1),lldtoStringUnit(FREQRESPHZ[i],freqString));
    }
	model->database().transaction();//��ʼ����
	if(model->submitAll())//�Ƿ�����������ύ
    	model->database().commit();//�ύ�޸�
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

/*���������¶�,���Է���,��Ƶ�ı�
@dbname		���ݿ���
*/
void createTable(QString dbname){
	QSqlDatabase db = openSqlite(dbname);
	if(db.tables().contains("otherdata")){
		db.close();
		db = QSqlDatabase::database("", false);
		QSqlDatabase::removeDatabase("qt_sql_default_connection");
		return;
	}else{
		QSqlTableModel *model = new QSqlTableModel(NULL,db);//�������ݿ�ģ��
		QSqlQuery *query = new QSqlQuery(db);//ִ��SQL���ԵĽӿ�
		query->exec("create table otherdata (id int primary key,factory double,\
				   prefactory double, zc double, prezc double, userdata double)");
		model->setTable("otherdata");
		model->setEditStrategy(QSqlTableModel::OnManualSubmit);
		model->select();
		int rows =1 + 1 + 1 + 32;//����У׼״̬,�¶�,���Է���,��Ƶ����
		for(int i=0; i< rows; i++)
		{
			model->insertRow(i);
			model->setData(model->index(i,0),i);//���ü�ֵ�����ݼ�ֵ����
			
		}
		model->database().transaction();//��ʼ����
		if(model->submitAll())//�Ƿ�����������ύ
			model->database().commit();//�ύ�޸�
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
�������ݵ���
@dbname		���ݿ���
@table 		�������ݵı�
*/
void saveDatatoTable(QString dbname,QString table)
{
	QSqlDatabase db = openSqlite(dbname);
	
	QSqlTableModel *model = new QSqlTableModel(NULL,db);//�������ݿ�ģ��
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
	//�жϱ��浽���ĸ���
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

	//У׼״̬
	model->setData(model->index(0,column), iscaled);
	//���Է���ֵ
	model->setData(model->index(1,column), absoluteAmptValue);
	//�¶�
	model->setData(model->index(2,column), temperature);
	for(int i=0; i<32; i++){
		model->setData(model->index(i+3,column),attIf[i]);
	}
	model->database().transaction();//��ʼ����
	if(model->submitAll())
		if(model->database().commit())//�ύ�ı䵽���ݿ�
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
	

	model->database().transaction();//��ʼ����
	if(model->submitAll())//�Ƿ�����������ύ
    	model->database().commit();//�ύ�޸�

		
	system("cp my.db temp.db -r");//���ݿⱸ��

	//�ر����ݿ⣬�ͷ��ڴ�
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
	
	QSqlTableModel *model = new QSqlTableModel(NULL,db);//�������ݿ�ģ��
	model->setTable("otherdata");
	model->setEditStrategy(QSqlTableModel::OnManualSubmit);
	model->select();
    while(model->canFetchMore())
        model->fetchMore();
	qDebug()<<"rows:" << model->rowCount() ;

	int column;
	int rows = sizeof FREQRESPHZ / sizeof FREQRESPHZ[0];
	//�жϲ�ѯ�ĸ���
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
		model->database().transaction();//��ʼ����
		if(model->submitAll())//�Ƿ�����������ύ
			model->database().commit();//�ύ�޸�
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


//�������У׼����
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




//����ת��Ϊ�ַ���
char* intToString(int value, char* result)
{
  char temp[32];
  sprintf(temp, "%d", value);
  strcpy(result, temp);

  return result;
}

/*
�����ݿ�,û���򴴽����ݿ�
@dbname		���ݿ���
*/
QSqlDatabase	openSqlite(QString dbname){

	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName(dbname);
	if(db.open()){
		return db;
	}else{
		return openSqlite("temp.db");//��ʧ��ʹ�ñ������ݿ�
	}
}




