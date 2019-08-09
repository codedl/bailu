#ifndef MAINTHREAD_H_
#define MAINTHREAD_H_

#include <QtGui/QApplication>
#include <QtCore/QObject>
#include <QtCore/QThread>
#include <QtCore/QTime>
#include "../common/global.h"
#include "../scpi/sysscpi.h"

static float WindowData[FFTSIZE], wr[FFTSIZE * 2], wi[FFTSIZE * 2], wr1[FFTSIZE * 2], wi1[FFTSIZE * 2];

//系统主线程定义
class mainThread : public QThread
{
    Q_OBJECT
  public:
	virtual void run();             //线程函数
	tSysScpi* sysScpi;
  signals:
    void drawTop(void);             //绘制顶部

    void drawLogo(void);            //绘制LOGO
    void drawTitle(void);           //绘制标题区信号
    void drawTime(void);            //绘制时间区信号
    void drawScpi(void);            //绘制SCPI信号
    void drawCanvas(void);          //绘制矢量区信号
    void drawHelp(void);            //绘制帮助区信号
    void drawButton(void);          //绘制按钮区信号
    void drawState(void);           //绘制状态区信号
    void drawAll(void);             //绘制全部
};

//系统次线程定义
class minorThread : public QThread
{
  Q_OBJECT
  public:
    virtual void run();             //线程函数
    tSysScpi* sysScpi;
  signals:
    void drawTop(void);             //绘制顶部
    void drawState(void);           //绘制状态区信号
    void drawInput(void);           //绘制输入区信号
};

//系统数据线程定义
class dataThread : public QThread
{
  Q_OBJECT
  public:
    virtual void run();             //线程函数// get if data and demod data period

    tSysScpi* sysScpi;
  private:
    void getDataFromIF(void);       //获取中频数据
    void getDataFromDisc(void);     //获取鉴频数据
};


//FFT线程
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
