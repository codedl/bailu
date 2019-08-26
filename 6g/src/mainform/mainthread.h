#ifndef MAINTHREAD_H_
#define MAINTHREAD_H_

#include <QtGui/QApplication>
#include <QtCore/QObject>
#include <QtCore/QThread>
#include <QtCore/QTime>
#include "../common/global.h"
#include "../scpi/sysscpi.h"

//系统主线程类
class mainThread : public QThread
{
    Q_OBJECT
  public:
	virtual void run();             	//线程函数
  signals:
    void drawState(void);
	void drawSystemInf(void);
};

//系统次线程类
class minorThread : public QThread
{
  Q_OBJECT
  public:
    virtual void run();           //线程函数
    tSysScpi* sysScpi;
  private:

  signals:
//  	  void drawWin1_5(void);        //PG100绘制按钮显示区
	  void drawProcess(void);       //绘制process
	  void drawUSBPower(void);		//绘制USB功率计
};

//系统数据线程定义
/*class dataThread : public QThread
{
  Q_OBJECT
  public:
    virtual void run();             //线程函数

    tSysScpi* sysScpi;
  private:
    void getDataFromIF(void);       //获取中频数据
    void getDataFromDisc(void);     //获取鉴频数据
};
*/
#endif
