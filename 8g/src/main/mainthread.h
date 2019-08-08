#ifndef MAINTHREAD_H_
#define MAINTHREAD_H_

#include <QtGui/QApplication>
#include <QtCore/QObject>
#include <QtCore/QThread>
#include <QtCore/QTime>
#include "../common/global.h"
#include "../scpi/sysscpi.h"

static float WindowData[FFTSIZE], wr[FFTSIZE * 2], wi[FFTSIZE * 2], wr1[FFTSIZE * 2], wi1[FFTSIZE * 2];

//ϵͳ���̶߳���
class mainThread : public QThread
{
    Q_OBJECT
  public:
	virtual void run();             //�̺߳���
	tSysScpi* sysScpi;
  signals:
    void drawTop(void);             //���ƶ���

    void drawLogo(void);            //����LOGO
    void drawTitle(void);           //���Ʊ������ź�
    void drawTime(void);            //����ʱ�����ź�
    void drawScpi(void);            //����SCPI�ź�
    void drawCanvas(void);          //����ʸ�����ź�
    void drawHelp(void);            //���ư������ź�
    void drawButton(void);          //���ư�ť���ź�
    void drawState(void);           //����״̬���ź�
    void drawAll(void);             //����ȫ��
};

//ϵͳ���̶߳���
class minorThread : public QThread
{
  Q_OBJECT
  public:
    virtual void run();             //�̺߳���
    tSysScpi* sysScpi;
  signals:
    void drawTop(void);             //���ƶ���
    void drawState(void);           //����״̬���ź�
    void drawInput(void);           //�����������ź�
};

//ϵͳ�����̶߳���
class dataThread : public QThread
{
  Q_OBJECT
  public:
    virtual void run();             //�̺߳���// get if data and demod data period

    tSysScpi* sysScpi;
  private:
    void getDataFromIF(void);       //��ȡ��Ƶ����
    void getDataFromDisc(void);     //��ȡ��Ƶ����
};


//FFT�߳�
class fftThread : public QThread
{
  Q_OBJECT
  private:
    void addWindow(float* re, float* im, int n, float quieoty);
    void fft(float *x, float *y, int n, int sign);
    void czt(float *xr, float *xi, int n, int m, double f1, double f2);
  public:
    virtual void run();

    tSysScpi* sysScpi;
};
#endif
