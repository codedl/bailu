#ifndef MAINTHREAD_H_
#define MAINTHREAD_H_

#include <QtGui/QApplication>
#include <QtCore/QObject>
#include <QtCore/QThread>
#include <QtCore/QTime>
#include "../common/global.h"
#include "../scpi/sysscpi.h"

//ϵͳ���߳���
class mainThread : public QThread
{
    Q_OBJECT
  public:
	virtual void run();             	//�̺߳���
  signals:
    void drawState(void);
};

//ϵͳ���߳���
class minorThread : public QThread
{
  Q_OBJECT
  public:
    virtual void run();           //�̺߳���
    tSysScpi* sysScpi;
  private:

  signals:
//  	  void drawWin5(void);        //PG100���ư�ť��ʾ��
	  void drawProcess(void);       //����process
};

//ϵͳ�����̶߳���
/*class dataThread : public QThread
{
  Q_OBJECT
  public:
    virtual void run();             //�̺߳���

    tSysScpi* sysScpi;
  private:
    void getDataFromIF(void);       //��ȡ��Ƶ����
    void getDataFromDisc(void);     //��ȡ��Ƶ����
};
*/
#endif
