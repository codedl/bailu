#ifndef SYSSCPI_H_
#define SYSSCPI_H_

#include <QtGui/QApplication>
#include <QtGui/QScreen>
#include <QtGui/QDesktopWidget>
#include <QtGui/QPixmap>
#include <QtGui/QBitmap>
#include <QtCore/QBuffer>
#include <QtCore/QByteArray>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QSettings>
#include <QtGui/QStaticText>
#include <QtGui/QTextOption>
#include <QtGui/QPainter>
#include <QtCore/QDateTime>
#include <QtCore/QDate>
#include <QtCore/QTime>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QHostAddress>
#include <QtNetwork/qhostinfo.h>
#include <QtNetwork/qnetworkinterface.h>
#include <QtNetwork/qnetworkconfigmanager.h>
#include <QtCore/QCryptographicHash>
#include <QtCore/QProcess>
#include <QtCore/qmath.h>
#include <QtCore/QThread>
#include <termios.h>
#include <math.h>
#include <qmath.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/vfs.h>
#include "libusb.h"
#include "basescpi.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>

#include  <unistd.h>
#include  <sys/types.h>       /* basic system data types */
#include  <sys/socket.h>      /* basic socket definitions */
#include  <netinet/in.h>      /* sockaddr_in{} and other Internet defns */
#include  <arpa/inet.h>       /* inet(3) functions */
#include <sys/epoll.h> /* epoll function */
#include <fcntl.h>     /* nonblocking */
#include <signal.h>
#include <cstring>
struct evaluateDataDef
{
  bool isEvaluate;
  int year;
  int month;
  int day;
  int timesOut;
  bool evaluated;
  int syDays;
};

#define MAXTCPCLIENTS  12
#define SOCKETTIMEOUT  600

struct clientSocketDef
{
  bool isWriting;
  bool isReading;
  QTcpSocket* socket;
  QDateTime sendTime;
  QDateTime recvTime;
};

//****************************************
//* SCPI类(继承自tBaseScpi)
//****************************************
class tSysScpi : public tBaseScpi
{
  Q_OBJECT
  public:
    tSysScpi();                                                          //构造
    ~tSysScpi();                                                         //析构

    QTcpSocket* cSocket;                                                 //校准建立的客户端

    QString devType;
    QByteArray devLogo;

    QFileInfoList fileList;                                              //文件列表
    QString fileSelectName;                                              //选择文件名称
    QPixmap fileSelectPixmap;                                            //选择文件图片
    bool gpibReaded;                                                     //GPIB只读
    QStringList gpibList;                                                //GPIB指令集
    int filesExport;                                                     //文件导出
    evaluateDataDef eva;                                                 //试用
    bool isUpdating;                                                     //升级
    bool isRcvingFile;                                                   //正在接收文件
    QString updatingFileName;                                            //接收文件名称

    QPixmap logo;
    bool isRcvingLogoFile;

    QString traceDataString[TRACECOUNT];
    bool traceDataOrganized[TRACECOUNT];//trace data stauts of preparing
    
    int setFrequencyOfCenter(double value);                              //设置中心频率
    int setFrequencyOfCenter(QString value);                             //设置中心频率
    int setFrequencyOfCenterWithArrow(bool up);                          //方向键设置中心频率
    int setFrequencyOfCenterWithRpg(int value);                          //脉冲旋钮设置中心频率
    int setFrequencyOfStart(double value);                               //设置起始频率
    int setFrequencyOfStart(QString value);                              //设置起始频率
    int setFrequencyOfStartWithArrow(bool up);                           //方向键设置起始频率
    int setFrequencyOfStartWithRpg(int value);                           //脉冲旋钮设置起始频率
    int setFrequencyOfStop(double value);                                //设置终止频率
    int setFrequencyOfStop(QString value);                               //设置终止频率
    int setFrequencyOfStopWithArrow(bool up);                            //方向键设置终止频率
    int setFrequencyOfStopWithRpg(int value);                            //脉冲旋钮设置终止频率
    int setFrequencyOfCenterStep(double value);                          //设置中心频率步进
    int setFrequencyOfCenterStep(QString value);                         //设置中心频率步进
    int setFrequencyOfCenterStepWithArrow(bool up);                      //方向键设置中心频率步进
    int setFrequencyOfCenterStepWithRpg(int value);                      //脉冲旋钮设置中心频率步进
    int setFrequencyOfCenterStepAuto(int value);                         //设置中心频率步进自动
    int setFrequencyOfCenterStepAuto(QString value);                     //设置中心频率步进自动
    int setFrequencyOffset(double value);                                //设置频率偏置
    int setFrequencyOffset(QString value);                               //设置频率偏置
    int setFrequencyOfOffsetWithArrow(bool up);                          //方向键设置中心频率
    int setFrequencyOfOffsetWithRpg(int value);                          //脉冲旋钮设置中心频率
    int setFrequencyOfReference(int value);                              //设置频率参考
    int setFrequencyOfSpan(double value);                                //设置扫宽
    int setFrequencyOfSpan(QString value);                               //设置扫宽
    int setFrequencyOfSpanWithArrow(bool up);                            //方向键设置扫宽
    int setFrequencyOfSpanWithRpg(int value);                            //脉冲旋钮设置扫宽
    int setFrequencyOfLowChannel(int value);                             //设置低频通道
    int setFrequencyOfScaleType(int value);															 //设置频率刻度类型
    int setFrequencyOfScaleType(QString value);													 //设置频率刻度类型

    int setAmptOfRefLevel(double value);                                 //设置参考电平
    int setAmptOfRefLevel(QString value);                                //设置参考电平
    int setAmptOfRefLevelWithArrow(bool up);                             //方向键设置参考电平
    int setAmptOfRefLevelWithRpg(int value);                             //脉冲旋钮设置参考电平
    int setAmptOfAttAuto(int value);                                     //设置衰减自动
    int setAmptOfAttAuto(QString value);                                 //设置衰减自动
    int setPreamptOfAttAuto(int value);                                  //设置前置放大开启
    int setPreamptOfAttAuto(QString value);                              //设置前置放大开启
    int setAmptOfAtt(double value);                                      //设置衰减
    int setAmptOfAtt(QString value);                                     //设置衰减
    int setAmptOfAttWithArrow(bool up);                                  //方向键设置衰减
    int setAmptOfAttWithRpg(int value);                                  //脉冲旋钮设置衰减
    int setLMP_CValue(double value);                                     //设置阻抗变换
    int setAmptOfLMP_CWithArrow(bool up);                                //方向键设置阻抗变换
    int setAmptOfLMP_CWithRpg(int value);                                //脉冲旋钮设置阻抗变换
    int setAmptOfScaleDiv(double value);                                 //设置幅度刻度
    int setAmptOfScaleDiv(QString value);                                //设置幅度刻度
    int setAmptOfScaleDivWithArrow(bool up);                             //方向键设置幅度刻度
    int setAmptOfScaleDivWithRpg(int value);                             //脉冲旋钮设置幅度刻度
    int setAmptOfRefOffset(double value);                                //设置参考偏置
    int setAmptOfRefOffset(QString value);                               //设置参考偏置
    int setAmptOfRefOffsetWithArrow(bool up);                            //方向键设置参考偏置
    int setAmptOfRefOffsetWithRpg(int value);                            //脉冲旋钮设置参考偏置
    int setAmptOfScaleType(int value);                                   //设置刻度类型
    int setAmptOfScaleType(QString value);                               //设置刻度类型
    int setAmptOfRefUnit(int value);                                     //设置参考单位
    int setAmptOfRefUnit(QString value);                                 //设置参考单位

    int setBwOfRbwAuto(int value);                                       //设置分辨率自动
    int setBwOfRbwAuto(QString value);                                   //设置分辨率自动
    int setBwOfRbw(double value);                                        //设置分辨率带宽
    int setBwOfRbw(QString value);                                       //设置分辨率带宽
    int setBwOfRbwWithArrow(bool up);                                    //方向键设置分辨率带宽
    int setBwOfRbwWithRpg(int value);                                    //脉冲旋钮设置分辨率带宽
    int setBwOfRbwStepMode(int value);                                   //设置分辨步进模式
    int setBwOfRbwStepMode(QString value);                               //设置分辨步进模式
    int setBwOfVbwAuto(int value);                                       //设置视频带宽自动
    int setBwOfVbwAuto(QString value);                                   //设置视频带宽自动
    int setBwOfVbw(double value);                                        //设置视频带宽
    int setBwOfVbw(QString value);                                       //设置视频带宽
    int setBwOfVbwWithArrow(bool up);                                    //方向键设置视频带宽
    int setBwOfVbwWithRpg(int value);                                    //脉冲旋钮设置视频带宽

    unsigned long getVbwFactor(double value);                            //获取VBW的系数

    int setBwOfAverage(double value);                                    //设置迹线平均
    int setBwOfAverage(QString value);                                   //设置迹线平均
    int setBwOfAverageWithArrow(bool up);                                //方向键设置迹线平均
    int setBwOfAverageWithRpg(int value);                                //脉冲旋钮设置迹线平均
    int setBwOfAverageSwitch(int value);                                 //设置迹线平均状态
    int setBwOfAverageSwitch(QString value);                             //设置迹线平均状态
    int resetBwOfAverageState(void);                                     //复位迹线平均状态
    int setBwOfEmcState(int value);                                      //设置EMC带宽测量
    int setBwOfEmcState(QString value);                                  //设置EMC带宽测量
    int resetMarkerFctnOfFreqCount(void);                                //复位频率计数

    int setDisplayOfFullScreen(int value);                               //设置显示全屏
    int setDisplayOfFullScreen(QString value);                           //设置显示全屏
    int setDisplayOfZoomFrequency(int value);                            //设置显示窗口缩放频率
    int setDisplayOfZoomOn(int value);                                   //设置显示窗口缩放
    int setDisplayOfZoomOn(QString value);                               //设置显示窗口缩放
    int setDisplayOfLineValue(QString value);                            //设置显示线
    int setDisplayOfLineValue(double value);                             //设置显示线
    int setDisplayOfLineValueWithArrow(bool up);                         //方向键设置显示线
    int setDisplayOfLineValueWithRpg(int value);                         //脉冲旋钮设置显示线
    int setDisplayOfLine(int value);                                     //设置显示线
    int setDisplayOfLine(QString value);                                 //设置显示线
    int setDisplayOfRefLevelGraticule(int value);                        //设置显示参考标尺
    int setDisplayOfRefLevelGraticule(QString value);                    //设置显示参考标尺
    int setDisplayOfGrid(int value);                                     //设置显示网格
    int setDisplayOfGrid(QString value);                                 //设置显示网格
    int setDisplayOfLabel(int value);                                    //设置显示标签
    int setDisplayOfLabel(QString value);                                //设置显示标签
    int setDisplayOfStyle(int value);                                    //设置显示风格
    int setDisplayOfStyle(QString value);                                //设置显示风格

    int setDisplayOfFreqLineStart(double value);                         //设置频率线起始
    int setDisplayOfFreqLineStart(QString value);                        //设置频率线起始
    int setDisplayOfFreqLineStartWithArrow(bool up);                     //方向键设置频率线起始
    int setDisplayOfFreqLineStartWithRpg(int value);                     //脉冲旋钮设置频率线起始
    int setDisplayOfFreqLineStop(double value);                          //设置频率线终止
    int setDisplayOfFreqLineStop(QString value);                         //设置频率线终止
    int setDisplayOfFreqLineStopWithArrow(bool up);                      //方向键设置频率线终止
    int setDisplayOfFreqLineStopWithRpg(int value);                      //脉冲旋钮设置频率线终止
    int setDisplayOfFreqLine(int value);                                 //设置频率线
    int setDisplayOfFreqLine(QString value);                             //设置频率线

    int clearAllUserSelfState(void);                                     //清除所有用户状态
    int clearUserSelfState(int index);                                   //清除用户状态
    int loadUserSelfState(int index);                                    //加载用户状态
    int saveUserSelfState(int index);                                    //保存用户状态
    int setSweepOfTimeAuto(int value);                                   //设置扫描时间自动
    int setSweepOfTimeAuto(QString value);                               //设置扫描时间自动
    int setSweepOfTime(double value);                                    //设置扫描时间
    int setSweepOfTime(QString value);                                   //设置扫描时间
    int setSweepOfTimeWithArrow(bool up);                                //方向键设置扫描时间
    int setSweepOfTimeWithRpg(int value);                                //脉冲旋钮设置扫描时间
    int setSweepOfMode(int value);                                       //设置扫描方式
    int setSweepOfMode(QString value);                                   //设置扫描方式
    int setSweepOfPoints(double value);                                  //设置扫描点数
    int setSweepOfPoints(QString value);                                 //设置扫描点数
    int setSweepOfPointsWithArrow(bool up);                              //方向键设置扫描点数
    int setSweepOfPointsWithRpg(int value);                              //脉冲旋钮设置扫描点数

    int setAutoTune(void);                                               //自动调谐

    int setTraceMode(int traceIndex, int value);                         //设置迹线模式
    int setTraceMode(int traceIndex, QString value);                     //设置迹线模式
    int setTraceOperationsOfExchange(int trace1Index, int trace2Index);  //设置迹线互换
    int setTraceOperationsOfChange(int trace1Index, int trace2Index);    //设置 迹线转移
    int setTraceOperationsOfDisplayLine(int traceIndex);                 //设置 迹线显示线

    int setDetector(int value);                                          //设置检波
    int setDetector(QString value);                                      //设置检波

    int setTrigMode(int value);                                          //设置触发模式
    int setTrigMode(QString value);                                      //设置触发模式
    int setVideoTrigPower(double value);                                 //设置视频触发功率
    int setVideoTrigPower(QString value);                                //设置视频触发功率
    int setVideoTrigPowerWithArrow(bool up);                             //方向键设置视频触发功率
    int setVideoTrigPowerWithRpg(int value);                             //脉冲旋钮设置视频触发功率
    int calculateVideoTrigPowerDown(double value);                       //计算触发功率下发值

    int isMeasureOn(void);                                               //测量已打开
    int isMeasureAllOff(void);                                           //所有测量已关闭
    int setMeasureAllOff(void);                                          //关闭所有测量
    int setMeasureOfTimeSpectrumOn(int value);                           //设置时间频谱测量
    int setMeasureOfAcprOn(int value);                                   //设置邻道功率测量
    int setMeasureOfChannelPowerOn(int value);                           //设置信道功率测量
    int setMeasureOfObwOn(int value);                                    //设置占用带宽测量
    int setMeasureOfDiscState(int value);                                //设置鉴频状态
    int setMeasureOfDiscInputGain(int value);                            //设置鉴频输入增益
    int setMeasureOfDiscOutputPower(double value);                       //设置鉴频输出功率
    int setMeasureOfDiscOutputPowerWithArrow(bool up);                   //方向键设置鉴频输出功率
    int setMeasureOfDiscOutputPowerWithRpg(int value);                   //脉冲旋钮设置鉴频输出功率
    int setMeasureOfDiscScaleDiv(double value);                          //设置鉴频刻度
    int setMeasureOfDiscScaleDivWithArrow(bool up);                      //方向键设置鉴频刻度
    int setMeasureOfDiscScaleDivWithRpg(int value);                      //脉冲旋钮设置鉴频刻度
    int setMeasureOfDiscSweepTime(double value);                         //设置鉴频扫描时间
    int setMeasureOfDiscSweepTimeWithArrow(bool up);                     //方向键设置鉴频扫描时间
    int setMeasureOfDiscSweepTimeWithRpg(int value);                     //脉冲旋钮设置鉴频扫描时间
    int setMeasureOfDiscDCOffset(double value);                          //设置鉴频直流偏置
    int setMeasureOfDiscDCOffsetWithArrow(bool up);                      //方向键设置直流偏置
    int setMeasureOfDiscDCOffsetWithRpg(int value);                      //脉冲旋钮设置直流偏置
    int setMeasureOfDiscNormalize(void);                                 //鉴频器标定

    int setMeasSetupOfChannelWidth(double value);                        //设置测量参数信道带宽
    int setMeasSetupOfChannelWidth(QString value);                       //设置测量参数信道带宽
    int setMeasSetupOfChannelWidthWithArrow(bool up);                    //方向键设置测量参数信道带宽
    int setMeasSetupOfChannelWidthWithRpg(int value);                    //脉冲旋钮设置测量参数信道带宽
    int setMeasSetupOfChannelSpace(double value);                        //设置测量参数信道间隔
    int setMeasSetupOfChannelSpace(QString value);                       //设置测量参数信道间隔
    int setMeasSetupOfChannelSpaceWithArrow(bool up);                    //方向键设置测量参数信道间隔
    int setMeasSetupOfChannelSpaceWithRpg(int value);                    //脉冲旋钮设置测量参数信道间隔
    int setMeasSetupOfChannelNums(double value);                         //设置测量参数邻道数目
    int setMeasSetupOfChannelNums(QString value);                        //设置测量参数邻道数目
    int setMeasSetupOfChannelNumsWithArrow(bool up);                     //方向键设置测量参数邻道数目
    int setMeasSetupOfChannelNumsWithRpg(int value);                     //脉冲旋钮设置测量参数邻道数目
    int setMeasSetupOfPowerPercent(double value);                        //设置测量参数占用带宽
    int setMeasSetupOfPowerPercent(QString value);                       //设置测量参数占用带宽
    int setMeasSetupOfPowerPercentWithArrow(bool up);                    //方向键设置测量参数占用带宽
    int setMeasSetupOfPowerPercentWithRpg(int value);                    //脉冲旋钮设置测量参数占用带宽
    int setMeasSetupOfChannelPowerOfSpan(QString value);                 //设置信道带宽测量扫宽
    int setMeasSetupOfPowerPercentOfSpan(QString value);                 //设置占用带宽测量扫宽

    int saveScreenPixmapToLocal(QString value);                          //保存屏幕截图至本地
    int saveScreenPixmapToUsb(QString value);                            //保存屏幕截图至闪存
	int saveTraceData(QString dirName,QString absoluteFileName);				//保存迹线数据
    int saveTraceDataToLocal(QString value);                             //保存迹线数据至本地
    int saveTraceDataToUsb(QString value);                               //保存迹线数据至闪存

    int loadUserState(void);                                             //载入用户参数
    int saveUserState(void);                                             //保存用户参数
    int loadPowerOffState(void);                                         //载入关机参数
    int savePowerOffState(void);                                         //保存关机参数

    int setPrintOfColor(int value);                                      //设置打印机类型
    int setPrintOfColor(QString value);                                  //设置打印机类型
    int setPrintOfOrientation(int value);                                //设置打印方向
    int setPrintOfOrientation(QString value);                            //设置打印方向
    int setPrintOfPaperSize(int value);                                  //设置打印纸张大小
    int setPrintOfPaperSize(QString value);                              //设置打印纸张大小
    int setPrintOfNums(double value);                                    //设置扫描点数
    int setPrintOfNums(QString value);                                   //设置扫描点数
    int setPrintOfNumsWithArrow(bool up);                                //方向键设置扫描点数
    int setPrintOfNumsWithRpg(int value);                                //脉冲旋钮设置扫描点数

    int setLanIP(QString value);                                         //设置IP地址
    int setLanMask(QString value);                                       //设置子网掩码地址
    int setLanGate(QString value);                                       //设置网关地址

    int setGpibAddr(int value);                                          //设置GPIB地址
    int setGpibAddr(QString value);                                      //设置GPIB地址
    int setGpibAddrWithArrow(bool up);                                   //方向键设置GPIB地址
    int setGpibAddrWithRpg(int value);                                   //脉冲旋钮设置GPIB地址

    int setSystemSerialOfBaudrate(int value);                            //设置串口波特率
    int setSystemSerialOfDatabits(int value);                            //设置串口数据位
    int setSystemSerialOfStopbits(int value);                            //设置串口停止位
    int setSystemSerialOfCheck(int value);                               //设置串口校验
    int setSystemSerialOfFlowControl(int value);                         //设置串口流控制

    QStringList getLanRealIPAndAddress(void);                            //获取LAN物理地址
    int resetSystemIOOfLan(void);                                        //设置LAN
    int resetSystemIOOfSerial(void);                                     //设置串行口

    int setSystemPoweronParam(int value);                                //设置开机参数
    int setSystemPoweronParam(QString value);                            //设置开机参数
    int setSystemPresetParam(int value);                                 //设置复位参数
    int setSystemPresetParam(QString value);                             //设置复位参数
    int setSystemLcdBacklight(double value);                             //设置LCD背光
    int setSystemLcdBacklight(QString value);                            //设置LCD背光
    int setSystemLcdBacklightWidthArrow(bool up);                        //方向键设置LCD背光
    int setSystemLcdBacklightWithRpg(int value);                         //脉冲旋钮设置LCD背光
    int setSystemLanguage(int value);                                    //设置系统语言
    int setSystemLanguage(QString value);                                //设置系统语言
    int setSystemDateTimeShow(int value);                                //设置系统日期时间显示
    int setSystemDateTimeShow(QString value);                            //设置系统日期时间显示
    int setSystemDateFormat(int value);                                  //设置系统日期格式
    int setSystemDateFormat(QString value);                              //设置系统日期格式
    int setSystemDate(QString value);                                    //设置系统日期
    int setSystemTime(QString value);                                    //设置系统时间
    int setSystemScpiScreenRefresh(int value);                           //设置SCPI屏幕刷新
    int setSystemScpiScreenRefresh(QString value);                       //设置SCPI屏幕刷新
    int setSystemServiceOfSerialNo(QString value);                       //设置序列号
    int setSystemServiceOfLanMacAddress(QString value);                  //设置网卡物理地址
    int setSystemServiceOfLanMacAddressWidthArrow(bool up);              //方向键设置网卡物理地址
    int setSystemServiceOfLanMacAddressWidthRpg(int value);              //方向键设置网卡物理地址
    int setSystemChannelSelect(int value);                               //设置内部信号
    int setSystemChannelSelect(QString value);                           //设置内部信号
    int setSystemIOGpibMainAddr(int value);                              //设置GPIB主地址
    int setSystemServiceOfEvalute(int value);                            //设置试用版本
    int setSystemServiceOfSelectTimeMode(int value);                     //选择时间模式
    int setSystemServiceOfSelectTimeMode(QString value);                 //选择时间模式
    int setSystemServiceOfSelectLog(int value);                          //选择log显示
    int setSystemServiceOfSelectLog(QString value);                      //选择log显示
    int setSystemServiceOfGpib(int value);                               //选择GPIB
    int setSystemServiceOfGpib(QString value);                           //选择GPIB

    int setSystemServiceOfTrackSelect(int value);                        //选择跟踪源模式
    int setSystemServiceOfTrackSelect(QString value);                    //选择跟踪源模式

    int setSystemServiceOptionOn(QString value);                         //设置系统选件开启
    int setSystemServiceOptionOff(QString value);                        //设置系统选件关闭

    int setSystemServiceDeviceOn(QString value);                         //设置系统设备开启
    int setSystemServiceDeviceOff(QString value);                        //设置系统设备关闭

    int setSystemServiceCalbrationMode(int value);

    int setSystemServiceSerialNoMode(int value);                         //选择序列号模式
    int setSystemServiceSerialNoMode(QString value);                     //选择序列号模式

    int setSystemServiceOptionFFT10Hz(int value);                        //设置系统FFT 10Hz选件
    int setSystemServiceOptionFFT10Hz(QString value);                    //设置系统FFT 10Hz选件


    int setSystemServiceOptionDemod(int value);                          //设置系统AM、FM选件
    int setSystemServiceOptionDemod(QString value);   
    int setSystemServiceOptionDirectChannel(int Value);                  //设置直采选件
    int getSystemServiceOfEvaluteSYDays(void);
    QString getSystemServiceOfMD5(QString value, bool isServer);
    int unLockSystem(void);   											//

    bool isUsbDeviceConnect(void);										 //USB设备连接状态
    int getFileOfNowSelectIndex(void);                                   //获取当前文件选择索引
    int setFileOfDeleteTheSelect(void);                                  //删除指定文件
    int setFileOfDeleteThePage(void);                                    //删除本页文件
    int setFileOfDeleteAll(void);                                        //删除所有文件
    int setFileOfExportTheSelect(void);                                  //导出选择文件
    int setFileOfExportThePage(void);                                    //导出本页文件
    int setFileOfExportAll(void);                                        //导出全部文件

    int setSourceTrackGenState(int value);                               //设置跟踪源状态
    int setSourceTrackGenState(QString value);                           //设置跟踪源状态
    int setSourceTrackGenNetworkMeasType(networkMeasType value);         //设置网络测量类型
    int setSourceTrackGenNetworkMeasType(QString value);                 //设置网络测量类型
    int setSourceTrackGenOutputPower(double value);                      //设置跟踪源输出功率
    int setSourceTrackGenOutputPower(QString value);                     //设置跟踪源输出功率
    int setSourceTrackGenOutputPowerWithArrow(bool up);                  //方向键设置跟踪源输出功率
    int setSourceTrackGenOutputPowerWithRpg(int value);                  //脉冲旋钮设置跟踪源输出功率
    int setSourceTrackGenNetworkState(int state);                        //设置跟踪源网络测量状态
    int setSourceTrackGenNetworkState(QString value);                    //设置跟踪源网络测量状态
    int setSourceTrackGenNetworkNormalize(void);                         //跟踪源网络测量数据归一化
    int setSourceTrackGenNetworkReflevel(double value);                  //设置跟踪源网络测量参考电平
    int setSourceTrackGenNetworkReflevel(QString value);                 //设置跟踪源网络测量参考电平
    int setSourceTrackGenNetworkReflevelWithArrow(bool up);              //方向键设置跟踪源网络测量参考电平
    int setSourceTrackGenNetworkReflevelWithRpg(int value);              //脉冲旋钮设置跟踪源网络测量参考电平
    int setSourceTrackGenNetworkOutputPower(double value);               //设置跟踪源网络测量输出功率
    int setSourceTrackGenNetworkOutputPower(QString value);              //设置跟踪源网络测量输出功率
    int setSourceTrackGenNetworkOutputPowerWithArrow(bool up);           //方向键设置跟踪源网络测量输出功率
    int setSourceTrackGenNetworkOutputPowerWithRpg(int value);           //脉冲旋钮设置跟踪源网络测量输出功率
    int setSourceTrackCalibrateParam(void);                              //设置跟踪源校准参数
    int setSourceTrackGenVSWRState(int state);                           //设置电压驻波比状态
    int setSourceTrackGenVSWRState(QString value);                       //设置电压驻波比状态
    int setSourceTrackGenVSWRRefLevel(double value);                     //设置电压驻波比参考
    int setSourceTrackGenVSWRRefLevel(QString value);                    //设置电压驻波比参考
    int setSourceTrackGenVSWRRefLevelWithArrow(bool up);                 //方向键设置电压驻波比参考
    int setSourceTrackGenVSWRRefLevelWithRpg(int value);                 //脉冲旋钮设置电压驻波比参考

    int setSourceSignalCalibrateParam(void);                             //设置信号源校准参数
    int setSourceAttention(double value);                                //设置源开关和衰减器
    int setSourceSignalGenState(int value, bool reset);                  //设置信号源状态
    int setSourceSignalGenState(QString value, bool reset);              //设置信号源状态
    int setSourceSignalGenOutputFreq(double value);                      //设置信号源输出频率
    int setSourceSignalGenOutputFreq(QString value);                     //设置信号源输出频率
    int setSourceSignalGenOutputFreqWithArrow(bool up);                  //方向键设置信号源输出频率
    int setSourceSignalGenOutputFreqWithRpg(int value);                  //脉冲旋钮设置信号源输出频率
    int setSourceSignalGenOutputPower(double value);                     //设置信号源输出频率
    int setSourceSignalGenOutputPower(QString value);                    //设置信号源输出频率
    int setSourceSignalGenOutputPowerWithArrow(bool up);                 //方向键设置信号源输出频率
    int setSourceSignalGenOutputPowerWithRpg(int value);                 //脉冲旋钮设置信号源输出频率
    void setSourceTrackGenPllFreq(int index);                            //设置跟踪源锁相环频率
    void setSourceTrackGenPll(int index);                                //设置跟踪源锁相环
    void setSourceSignalCalibrateParamOfFreq(int index);                 //设置信号源校准频率
    void loadDiscConfig(void);                                           //载入鉴频设置
    void saveDiscConfig(void);                                           //保存鉴频设置
    void loadSourceConfig(void);                                         //载入源配置
    void saveSourceConfig(void);                                         //保存源配置
    void saveSourceConfigOfNetworkMeasure(void);                         //保存网络测量配置

    int setDemodOfSoundState(int value);                                 //设置解调状态
    int setDemodOfSoundState(QString value);                             //设置解调状态
    int setDemodOfMode(int value);                                       //设置解调模式
    int setDemodOfMode(QString value);                                   //设置解调模式
    int setDemodOfSound(double value);                                   //设置音量
    int setDemodOfSound(QString value);                                  //设置音量
    int setDemodOfSoundWithArrow(bool up);                               //方向键设置音量
    int setDemodOfSoundWithRpg(int value);                               //脉冲旋钮设置音量
    int setDemodOfTheFrequency(int index, double freq);                  //设置指定解调频率
    int setDemodOfTheFrequency(int index, QString value);                //设置解调电台频率
    int setDemodOfTheFrequencyWithArrow(int index, bool up);             //方向键设置解调电台频率
    int setDemodOfTheFrequencyWithRpg(int index, int value);             //脉冲旋钮设置解调电台频率
    int setDemodModeOfFreq(int index);                                   //设置解调电台频率

    int setDemodOfAmState(int value);                                    //设置AM解调
    int setDemodOfAmState(QString value);                                //设置AM解调
    int setDemodOfFmState(int value);                                    //设置FM解调
    int setDemodOfFmState(QString value);                                //设置FM解调
    int setDemodOfAmCarrierFreq(double value);                           //设置AM载波频率
    int setDemodOfAmCarrierFreq(QString value);                          //设置AM载波频率
    int setDemodOfAmCarrierFreqWithArrow(bool up);                       //方向键设置AM载波频率
    int setDemodOfAmCarrierFreqWithRpg(int value);                       //脉冲旋钮设置AM载波频率
    int setDemodOfAmIFBWState(int value);                                //设置中频带宽状态
    int setDemodOfAmIFBWState(QString value);                            //设置中频带宽状态
    int setDemodOfAmIFBW(double value);                                  //设置AM中频带宽
    int setDemodOfAmIFBW(QString value);                                 //设置AM中频带宽
    int setDemodOfAmIFBWWithArrow(bool up);                              //方向键设置AM中频带宽
    int setDemodOfAmIFBWWithRpg(int value);                              //脉冲旋钮设置AM中频带宽
    int setDemodOfFmCarrierFreq(double value);                           //设置FM载波频率
    int setDemodOfFmCarrierFreq(QString value);                          //设置FM载波频率
    int setDemodOfFmCarrierFreqWithArrow(bool up);                       //方向键设置FM载波频率
    int setDemodOfFmCarrierFreqWithRpg(int value);                       //脉冲旋钮设置FM载波频率
    int setDemodOfFmIFBWState(int value);                                //设置中频带宽状态
    int setDemodOfFmIFBWState(QString value);                            //设置中频带宽状态
    int setDemodOfFmIFBW(double value);                                  //设置FM中频带宽
    int setDemodOfFmIFBW(QString value);                                 //设置FM中频带宽
    int setDemodOfFmIFBWWithArrow(bool up);                              //方向键设置FM中频带宽
    int setDemodOfFmIFBWWithRpg(int value);                              //脉冲旋钮设置FM中频带宽

    int setImpedanceOfState(int value);                                  //设置阻抗匹配
    int setImpedanceOfState(QString value);                              //设置阻抗匹配

    void getSystemFileList(void);                                        //获取文件列表

    int execPrintTrace(void);                                            //打印迹线
    int execPrintScreen(void);                                           //打印屏幕

    void getSystemDateTime(void);                                        //获取系统日期时间
    void getSystemFont(void);                                            //获取系统字体
    void presetSystemData(void);                                         //复位系统数据
    int presetSystem(void);                                              //复位系统

    void rfDownload(unsigned int addr, unsigned long value);             //射频参数下发
    void ifDownload(unsigned int addr, unsigned long value);             //中频参数下发
    void feDownload(unsigned int addr, unsigned long value)
    {
    	if (feHandle > 0)
    	{
    		ioctl(feHandle, addr, value);
    	}
    };						 //射频前端参数下发
    void ramDownload(unsigned int addr, unsigned long value)
    {
    	if (ramHandle > 0)
    	{
    		ioctl(ramHandle, addr, value);
    	}
    };						 //射频前端参数下发
   // void tqIfDownload(unsigned int addr, unsigned long value);

    int setMarkerAllOff(void);                                           //关闭所有频标
    int setMarkerXValue(int mkrIndex, QString value);                    //设置标记X值
    int setMarkerXValue(int mkrIndex, double value);                     //设置标记X值
    int isMarkerAllOff(void);                                            //标记均关闭
    int isTheMarkerOff(int mkrIndex, int traceIndex);                    //标记关闭
    void setMarkerNormal(int mkrIndex, int traceIndex);                  //设置常态标记
    void setMarkerDelta(int mkrIndex, int traceIndex);                   //设置差值标记
    double getTheMarkerXValue(int mkrIndex, int xIndex);                 //获取指定索引X值
    double getTheMarkerYValue(int mkrIndex, int xIndex);                 //获取指定索引Y值
    double getMarkerXValue(int mkrIndex);                                //获取标记X值
    double getMarkerYValue(int mkrIndex);                                //获取标记Y值
    int setMarkerTable(int value);                                       //设置频标列表
    int setMarkerTable(QString value);                                   //设置频标列表

    int setPeakTable(int value);                                         //设置EMI峰值列表
 //   int setPeakTable(QString value);                                   //设置频标列表

    int setMarkerToCenterFreq(int mkrIndex);                             //标记→中心频率
    int setMarkerToCenterFreqStep(int mkrIndex);                         //标记→中心频率步进
    int setMarkerToStartFreq(int mkrIndex);                              //标记→起始频率
    int setMarkerToStopFreq(int mkrIndex);                               //标记→终止频率
    int setMarkerToRefLevel(int mkrIndex);                               //标记→参考电平
    int setMarkerDeltaToSpan(int mkrIndex);                              //标记△→扫宽
    int setMarkerDeltaToCenterFreq(int mkrIndex);                        //标记△→中心频率

    int setMarkerFunctionAllOff(void);                                   //设置频标功能关闭
    int setMarkerFunctionNdBOn(int value);                               //设置频标功能NdB开启
    int setMarkerFunctionNdBOn(QString value);                           //设置频标功能NdB开启
    int setMarkerFunctionNdBValue(double value);                         //设置频标功能NdB值
    int setMarkerFunctionNdBValue(QString value);                        //设置频标功能NdB值
    int setMarkerFunctionNdBValueWithArrow(bool up);                     //方向键设置频标功能NdB值
    int setMarkerFunctionNdBValueWithRpg(int value);                     //脉冲旋钮设置频标功能NdB值
    int setMarkerFunctionFreqCountOn(int value);                         //设置频标功能频率计数开启
    int setMarkerFunctionFreqCountOn(QString value);                     //设置频标功能频率计数开启
    int setMarkerFunctionFreqCountBw(int value);                         //设置频标功能频率计数分辨率
    int setMarkerFunctionFreqCountBw(QString value);                     //设置频标功能频率计数分辨率
    int setMarkerFunctionFreqCountBwWithArrow(bool up);                  //方向键设置频标功能频率计数分辨率
    int setMarkerFunctionFreqCountBwWithRpg(int value);                  //脉冲旋钮设置频标功能频率计数分辨率
    int getMarkerFunctionNdB(int mkrIndex);                              //获取NdB
    int setMarkerFunctionNoiseOn(int value);                             //设置频标功能噪声开启
    int setMarkerFunctionNoiseOn(QString value);                         //设置频标功能噪声开启
    int getMarkerFunctionNoise(int mkrIndex);                            //获取噪声
    int setMarkerFunctionNoiseStartFreq(QString value);
    int setMarkerFunctionNoiseSpanFreq(QString value);
    QString getMarkerFunctionNoiseStartFreq(void);
    QString getMarkerFunctionNoiseSpanFreq(void);

    double getTraceMaxValue(int traceIndex);                             //获取迹线最大值
    double getTraceMinValue(int traceIndex);                             //获取迹线最小值

    int setMarkerPeakMax(int mkrIndex, int traceIndex);                  //设置最大值
    int setMarkerPeakMin(int mkrIndex, int traceIndex);                  //设置最小值
    int setMarkerPeakNext(int mkrIndex, int traceIndex);                 //设置下一峰值
    int setMarkerPeakLeft(int mkrIndex, int traceIndex);                 //设置左峰值
    int setMarkerPeakRight(int mkrIndex, int traceIndex);                //设置右峰值
    int setMarkerPeakToCenterFreq(int mkrIndex, int traceIndex);         //峰值标记至中心频率
    int setMarkerPeakMaxAuto(int value);                                 //设置峰值最大值自动搜索
    int setMarkerPeakMaxAuto(QString value);                             //设置峰值最大值自动搜索

    int getMarkerPeakMaxIndex(int mkrIndex, int traceIndex);             //峰值最大值
    int getMarkerPeakMinIndex(int mkrIndex, int traceIndex);             //峰值最小值
    int getMarkerPeakNextIndex(int mkrIndex, int traceIndex);            //下一峰值
    int getMarkerPeakLeftIndex(int mkrIndex, int traceIndex);            //左峰值
    int getMarkerPeakRightIndex(int mkrIndex, int traceIndex);           //右峰值

    double getModifiedEmcBw(double value);                               //寻找最接近的EMC带宽
    int getBucDelay(void);                                               //Buc Delay
    double getRecentBw(double value);                                    //获取最近的带宽
    double getRecentVBw(double value);                                   //获取最近的带宽
    int getSweepTimeQuieoty(void);                                       //获取扫描时间系数
    void getVbwOffset(void);                                             //获取视频偏移
    void getModifiedVbw(int arrow);                                      //获取视频带宽
    void bwAutoCouple(void);                                             //带宽自动耦合
    double getRefLevelmVTodBm(double value);                             //mV转化为dBm
    double getRefLevelValue(double value);                               //获取当前幅度值
    double getRefLeveldBmValue(void);                                    //获取幅度dBm值
    double getdBmValue(double value);                                    //获取dBm值
    double getAmptMaxMinLimit(void);                                     //获取幅度上下限
    void getTheAmptMaxMinLimit(double minValue, double maxValue,
    	                       double* min, double* max);                //获取功率上下限
    char* getAttAndUnitString(double value, char* result, int full);     //获取衰减和单位字符串
    char* getAmptAndUnitString(double value, char* result, int full);    //获取幅度和单位字符串
    int getStartLogFreqIndex(double value);                              //获取频率起始索引
    int getStopLogFreqIndex(double value);                               //获取频率终止索引

    int resetFrequencyRespondOf9kHz(double value);                       //恢复频响9kHz补偿数据
    int resetFrequencyRespondOf20kHz(double value);                      //恢复频响20kHz补偿数据
    int modifyFrequencyRespondOf9kHz(double value);                      //修改频响9kHz补偿数据
    int modifyFrequencyRespondOf9kHz(QString value);                     //修改频响9kHz补偿数据
    int modifyFrequencyRespondOf20kHz(double value);                     //修改频响20kHz补偿数据
    int modifyFrequencyRespondOf20kHz(QString value);                    //修改频响20kHz补偿数据
    int modifyFrequencyRespondOf9kHzWithArrow(bool up);                  //方向键修改频响9kHz补偿数据
    int modifyFrequencyRespondOf9kHzWithRpg(int value);                  //脉冲旋钮修改频响9kHz补偿数据
    int modifyFrequencyRespondOf20kHzWithArrow(bool up);                 //方向键修改频响20kHz补偿数据
    int modifyFrequencyRespondOf20kHzWithRpg(int value);                 //脉冲旋钮修改频响20kHz补偿数据

    void controlRf(void);                                                //射频控制
    void controlIf(void);                                                //中频控制
    void controlIfDetector(void);                                        //中频检波方式控制
    void modifyBandIndex(void);										     									 //修改频段索引值
    void setFreqSweepStatus(void);					                             //零扫宽和扫描状态选择
    void calSweepTimeOfAuto(void);
    void sweepTimeDown(void);
    void readData(void);
	void getcalData(void);                                            //获取中频数据
	void getDataFromIF(void);                                            //获取中频数据
    void getDataFromUsbDisc(void);                                       //获取鉴频数据
    void getDataFromDemod(void);                                         //获取解调数据

    bool isCalibrating(void);											 //是否正在校准
    void clearFactoryCalibrateData(void);                                //清除出厂校准
    void clearUserCalibrateData(void);                                   //清除用户校准

    int factoryCalibrate(int comCal);                                    //出厂校准
	int freqCalibrate(double freq);								//单点校准
    int PreamplifierCalibrate(int comCal);                               //前置放大校准
    int userCalibrate(void);                                             //用户校准
    int ZCCalibrate(int comCal);                                         //直采通道校准
    int ZCPreamplifierCalibrate(int comCal);                             //直采通道前置放大校准

    QStringList loadLogOfFactoryCalibrate(void);                         //载入出厂校准日志
    void reconnectSerial(void);                                          //重新连接串口

    void showRunMessage(QString title);
    void showRunAlarm(QString title);

    int getRbwIndex(void);                                               //获取分辨带宽索引
    int getVbwIndex(void);                                               //获取视频带宽索引
    int getVbwModIndex(void);                                            //获取视频带宽修正索引

    //fft
    void firFilter(double* x, double* h, double* y, int nx, int nh, int ny, int err);
    bool checkFFTMeasure(void);                                          //FFT测量条件判断检测
    bool isDirectGatherMode(void);                                       //是否为直采模式
    bool isRequireFFTMeas(void);                                         //是否需要进行FFT测量控制
    bool fftControl(int index);                                          //fft控制
    void fftControl(void);
    bool directGatherControl(void);                                      //直采控制
    bool demodControl(void);                                             //解调控制
    bool demodControlIF(void);                                           //解调中频控制

    bool getAmResult(void);                                              //获取AM结果
    bool getFmResult(void);                                              //获取FM结果
    int getSinadAndThd(double *unmodul_idata,
    		           double *unmodul_qdata,
    				   char lra,
    				   double fs,
    				   int *harm_num,
    				   double *sinad,
    				   double *thd,
    			       double *snr,
    				   double *enob,
    				   double *modul_freq);	                             //获取SINAD和THD

    void firDemod(double xin[], double xout[], int length, int n, double *h);  //FIR滤波
    void getDemodPeakList(void);                                         //获取峰值
    void getDemodDiff(double *data, double *diff, int len);
    bool isDemodOk(unsigned long *iqData, double *mData, int n);         //解调成功与否
    bool isDemodOkOfAm(unsigned long *iqData, double *mData, int n);     //AM解调成功与否
    bool isDemodOkOfFm(unsigned long *iqData, double *mData, int n);     //FM解调成功与否
    void getDemodDataOfAm(unsigned long *iqData, double *depth, double *freq, double fs, int n);
    void getDemodDataOfFm(unsigned long *iqData, double *offset, double *freq, double fs, int n);
    void fft(double *x, double *y, int n, int sign);

    void checkMultiDisplay(multiDispType value);

    void resetPassFailData(void);                                        //复位Pass-Fail数据
    void loadPassFailLineData(void);                                     //加载Pass-Fail迹线定义数据

    bool getMeasureOfPassFailOfWinMeas(int mkrIndex);                    //获取Pass-Fail窗口测量结果
    int setMeasureOfPassFailOfWinMeasState(int value);                   //设置Pass-Fail窗口测量状态
    int setMeasureOfPassFailOfWinMeasState(QString value);               //设置Pass-Fail窗口测量状态
    int setMeasureOfPassFailOfWinLimitLineState(int value);              //设置Pass-Fail幅值线状态
    int setMeasureOfPassFailOfWinLimitLineState(QString value);          //设置Pass-Fail幅值线状态
    int setMeasureOfPassFailOfWinFreqLineState(int value);               //设置Pass-Fail频率线状态
    int setMeasureOfPassFailOfWinFreqLineState(QString value);           //设置Pass-Fail频率线状态
    int setMeasureOfPassFailOfWinLineValueMode(int value);               //设置Pass-Fail限制线索引
    int setMeasureOfPassFailOfWinLineValueMode(QString value);           //设置Pass-Fail限制线索引
    int setMeasureOfPassFailOfWinLineValue(double value);                //设置Pass-Fail限制线幅值
    int setMeasureOfPassFailOfWinLineValue(QString value);               //设置Pass-Fail限制线幅值
    int setMeasureOfPassFailOfWinLineValueWithArrow(bool up);            //方向键设置Pass-Fail限制线幅值
    int setMeasureOfPassFailOfWinLineValueWithRpg(int value);            //脉冲旋钮设置Pass-Fail限制线幅值
    int setMeasureOfPassFailOfWinFreqValueMode(int value);               //设置Pass-Fail频率索引
    int setMeasureOfPassFailOfWinFreqValueMode(QString value);           //设置Pass-Fail频率索引
    int setMeasureOfPassFailOfWinFreqValue(double value);                //设置Pass-Fail频率
    int setMeasureOfPassFailOfWinFreqValue(QString value);               //设置Pass-Fail频率
    int setMeasureOfPassFailOfWinFreqValueWithArrow(bool up);            //方向键设置Pass-Fail频率
    int setMeasureOfPassFailOfWinFreqValueWithRpg(int value);            //脉冲旋钮设置Pass-Fail频率
    int setMeasureOfPassFailOfWinSweepState(int value);                  //设置Pass-Fail窗口扫描状态
    int setMeasureOfPassFailOfWinSweepState(QString value);              //设置Pass-Fail窗口扫描状态

    bool getMeasureOfPassFailOfLineMeas(int mkrIndex, double canvasMinValue, double canvasMaxValue); //获取Pass-Fail迹线测量结果
    void refreshMeasureOfPassFailOLineUpPages(void);                     //刷新Pass-Fail迹线测量上线页
    void refreshMeasureOfPassFailOLineLowPages(void);                    //刷新Pass-Fail迹线测量下线页
    void addMeasureOfPassFailOLineUpPoint(void);                         //增加 Pass-Fail迹线测量点
    void insertMeasureOfPassFailOLineUpPoint(void);                      //插入 Pass-Fail迹线测量点
    void deleteMeasureOfPassFailOLineUpPoint(void);                      //删除 Pass-Fail迹线测量点
    void clearMeasureOfPassFailOLineUpPoint(void);                       //清除 Pass-Fail迹线测量点
    int setMeasureOfPassFailOfLineMeasState(int value);                  //设置Pass-Fail迹线测量状态
    int setMeasureOfPassFailOfLineMeasState(QString value);              //设置Pass-Fail迹线测量状态
    int setMeasureOfPassFailOfLineUpState(int value);                    //设置Pass-Fail迹线测量上线状态
    int setMeasureOfPassFailOfLineUpState(QString value);                //设置Pass-Fail迹线测量上线状态
    int setMeasureOfPassFailOfLineLowState(int value);                   //设置Pass-Fail迹线测量下线状态
    int setMeasureOfPassFailOfLineLowState(QString value);               //设置Pass-Fail迹线测量下线状态
    int setMeasureOfPassFailOfLineShiftMode(int value);                  //设置Pass-Fail迹线测量偏置索引
    int setMeasureOfPassFailOfLineShiftMode(QString value);              //设置Pass-Fail迹线测量偏置索引
    int setMeasureOfPassFailOfLineShiftXYValue(double value);            //设置Pass-Fail迹线测量偏置
    int setMeasureOfPassFailOfLineShiftXYValue(QString value);           //设置Pass-Fail迹线测量偏置
    int setMeasureOfPassFailOfLineShiftXYValueWithArrow(bool up);        //方向键设置Pass-Fail迹线测量偏置
    int setMeasureOfPassFailOfLineShiftXYValueWithRpg(int value);        //脉冲旋钮设置Pass-Fail迹线测量偏置
    int getMeasureOfPassFailOfLineMeasOfLineUpPages(void);               //获取Pass-Fail迹线测量上线页数
    int getMeasureOfPassFailOfLineMeasOfLineLowPages(void);              //获取Pass-Fail迹线测量下线页数
    int setMeasureOfPassFailOfLineFreq(double value);                    //设置Pass-Fail迹线频率
    int setMeasureOfPassFailOfLineFreq(QString value);                   //设置Pass-Fail迹线频率
    int setMeasureOfPassFailOfLineFreqWithArrow(bool up);                //方向键设置Pass-Fail迹线频率
    int setMeasureOfPassFailOfLineFreqWithRpg(int value);                //脉冲旋钮设置Pass-Fail迹线频率
    int setMeasureOfPassFailOfLineAmpt(double value);                    //设置Pass-Fail迹线幅度
    int setMeasureOfPassFailOfLineAmpt(QString value);                   //设置Pass-Fail迹线幅度
    int setMeasureOfPassFailOfLineAmptWithArrow(bool up);                //方向键设置Pass-Fail迹线幅度
    int setMeasureOfPassFailOfLineAmptWithRpg(int value);                //脉冲旋钮设置Pass-Fail迹线幅度
    void gotoFirstPageOfMeasureOfPassFailLineData(void);                 //Pass-Fail首页
    void gotoPrevPageOfMeasureOfPassFailLineData(void);                  //Pass-Fail上一页
    void gotoNextPageOfMeasureOfPassFailLineData(void);                  //Pass-Fail下一页
    void gotoLastPageOfMeasureOfPassFailLineData(void);                  //Pass-Fail尾页
    void gotoRowOfMeasureOfPassFailLineData(int value);                  //指定行

   double getErrorOffftMeasureCalculate(double value,
    		                             unsigned long long index,
    		                             unsigned long long startIndex,
    		                             unsigned long long stopIndex);  //获取FFT 计算误差(拟合)


   int setFrontendPreamptOn(int);
   int setFrontEndFreq(double value);
   int setFrontEndFreq(QString value);
   int setLowbandAtt(double value);
   int setLowbandAtt(QString value);
   int setHighbandAtt(double value);
   int setHighbandAtt(QString value);
   int setFrontEndIfAtt(double value);
   int setFrontEndIfAtt(QString value);
   int getPremptData(int value);
   int rfPreamptDown(int value);
   int rfPreamptDown(QString value);
   int setFrontendRefVolt(double value);
   int setFrontendRefVolt(QString value);
   int setFrontendRefSelect(int value);
   int setFrontendRefSelect(QString value);
   char calIfAttValueOfFrontend(double);
   char calHighbandAttValueOfFrontend(double);
   int calLowbandAttValueOfFrontend(double);

   qint64 getFreeDiskSpace(void);                                        //获取剩余内存

   void execSelfCheck(void);                                             //执行自检
   QString getSelfCheckText(bool value);                                 //获取自检参数文本
   void outputToScreen(QString title);                                  //输出到屏幕
   void outputToScreen(QString title, QString value);                   //输出到屏幕
   void outputToScreen(QString title, int progress, int normal);        //输出到屏幕

   int setUdpIP(QString value);
   int setUdpPort(QString value);
  protected:
    bool isFillData;
    unsigned int sweepPrevPoints;
    int usbDiscFailNums;
    bool usbDeviceInitedSuccess;                                  			 //Usb主口状态
    libusb_device **devs;                                                //Usb总线设备
    libusb_device_handle *dev_handle;                                    //Usb设备句柄
    libusb_context *ctx;                                                 //Usb消息
    unsigned char usbhostbuf[1024];                                      //USB缓冲区

    QTcpServer* serverSocket;                                            //服务端SOCKET
    clientSocketDef clientSockets[MAXTCPCLIENTS];                        //客户端SOCKET
    //QTcpSocket* clientSocket;                                          //客户端SOCKET
    QByteArray readBuf;                                                  //读缓冲区
    QByteArray writeBuf;                                                 //写缓冲区

    bool openServerSocket(void);                                         //打开服务器SOCKET
    bool closeServerSocket(void);                                        //关闭服务器SOCKET

    bool discLibraryInitialize(void);                                    //鉴频库初始化
    bool connectToDiscriminator(void);                                   //连接鉴频器
    void disconectFromUSBDevice(void);                               //断开鉴频器
    bool setDiscriminationDataLen(int len);                              //设置鉴频数据长度
    void setDiscriminatorInputGain(int level);                           //设置鉴频输入增益
    bool usbDiscriminatorSend(unsigned char* data, int len);             //发送数据
    bool usbDiscriminatorRecv(unsigned char* data, int dataLen,
    		                                       int* recvLen);        //接收数据
    bool getDiscriminationCalResult(int dw, int* result);                //获取鉴频校准结果
    void setDiscriminatorFreq(double value);                             //设置鉴频频率

    int setPowerMeterFreq(double value);
    int setPowerMeterFreq(QString value);
    int getDataFromPowerMeter(void);
    bool connectToPowerMeter(void);
    bool usbDeviceSend(unsigned char* data, int len, int endpoint);
    bool usbDeviceRecv(unsigned char* data, int dataLen, int* recvLen, int endpoint);

    void handleScpiCommand(QString cmdStr);                              //处理SCPI指令
    void returnScpiResult(QString value);                                //返回SCPI结果
    int returnScpiResult(QByteArray value);                              //返回SCPI结果

    void closeSourceGenerator();                                         //关闭源发生器
    void closeSourceTrackGenerator();                                    //关闭跟踪源
    void closeSourceSignalGenerator();                                   //关闭信号源
    unsigned long setSourceSignalGenFreq(bool value);                    //设置信号源输出频率

    int sendSerialData(bool isCom, QString cmd);                         //发送Serial数据
    int setupSerial(void);                                               //设置串口

    void loadDeviceConfig(void);                                         //获取设备配置
    void saveDeviceConfig(void);                                         //设置设备属性
    void saveLCDBacklight(void);                                         //保存LCD背光

    void resetFactoryCalibrationData(void);                              //复位出厂校准数据
    void resetPreamplifierCalibrationData(void);                         //复位前置放大校准数据
    void resetUserCalibrationData(void);                                 //复位用户校准数据
    void resetZcCalibrationData(void);                                   //复位直采校准数据
    void resetZcPreamplifierCalibrationData(void);                       //复位直采通道前置放大校准数据
    void getFactoryCalibrationData(void);                                //获取出厂校准数据
    void getPreamplifierCalibrationData(void);                           //获取前置放大校准数据
    void getUserCalibrationData(void);                                   //获取用户校准数据
    void getZcCalibrationData(void);                                     //获取直采校准数据
    void getZcPreamplifierCalibrationData(void);                         //获取直采通道前置放大校准数据
    void saveImpedanceValue(void);                                       //保存75欧姆阻抗转换补偿值
    void saveFactoryCalibrationData(void);                               //保存出厂校准数据
    void savePreamplifierCalibrationData(void);                          //保存前置放大校准数据
    void saveUserCalibrationData(void);                                  //保存用户校准数据
    void saveZcFactoryCalibrationData(void);                             //保存直采校准数据
    void saveZcPreamplifierFactoryCalibrationData(void);                 //保存直采通道前置放大校准数据
    void saveSystemLog(QString log);                                     //保存系统日志
    void saveLogOfFactoryCalibrate(QString datetime,
    		                       QString result,
    		                       QString reason);                      //保存出厂校准日志
    void saveLogOfPreamplifierCalibrate(QString datetime,
    		                            QString result,
    		                            QString reason);                 //保存前置放大校准日志

    int printPixmap(QPixmap pix, unsigned char pageSize,
    		                     unsigned char orientation,
    		                     unsigned char copyNums,
    		                     bool colour);                           //打印图片


    int ulideDataSend(unsigned short channel,unsigned short cmd,unsigned short addr,unsigned short size,double data);
    bool updateRecordOfClear(void);
    bool updateRecordOfAdd(QString value);
    bool updateSystem(void);
    QString updateFileName(QString value);

  public:
    void ddcControl(double value);                                       //DDC控制
    void resetZcParams(void);                                            //复位直采参数
    void reCalculateZcParams(void);                                      //重新计算直采参数
  private:
    QString versionString;
    QString returnEofString;
    QString scpiBuffer;                                                  //SCPI栈

    QPixmap pixBackground;                                               //背景图片
    QVector <int> posPeakList;                                           //峰值列表
    struct calRunDataDef calRunData;                                     //校准参数

    bool loadLogarithmDefineData(void);                                  //载入对数预定义数据

    void loadDeviceType(void);
    void saveDeviceType(QString value);
    void loadDiveceLogo(void);
    void saveDeviceLogo(QString value);

    void saveSystemSetting(QString section, QString name, QVariant value);

    void scpiFloatToLen8String(double value, char* result);              //数值转化
    void asyncDelay(unsigned long value);                                //异步延时
    int exitCalibrate(void);                                             //退出校准

    void getSystemYSValue(void);
    void getIfSoftValue(void);                                            //获取增益补偿
    void setAtt1AndAtt2(void);                                           //设置两级衰减
    void zeroSpanHandle(void);                                           //零扫宽处理

    double zoomInData(double value, int step);                           //数据放大
    double zoomOutData(double value, int step);                          //数据缩小

    int copyTheMemoryScreen(QString fileName);
    int copyTheMemoryTrace(QString fileName);
    int copyAllMemoryScreen(void);
    int copyAllMemoryTrace(void);
    int copyAllMemory(void);

    int deleteTheMemoryScreen(QString fileName);
    int deleteTheMemoryTrace(QString fileName);
    int deleteAllMemoryScreen(void);
    int deleteAllMemoryTrace(void);
    int deleteAllMemory(void);

    int loadTheMemoryScreen(QString fileName);
    int loadTheMemoryTrace(QString fileName);

    QString getSystemFileListString(void);
    QString getSystemDiskInformation(void);
    QString getSystemInfoString(void);
    QString getSystemLogString(void);
    QString getSystemMeasureModeName(void);

    int setMarkerMode(int mkrIndex, QString modeName);
    QString getMarkerModeName(int index);
    QString boolToString(bool value);                                    //获取布尔字符串
    int getBoolValue(QString value, bool* ok);                           //获取布尔值
    unsigned char getCharValue(QString value, bool* ok);                 //获取小整数
    double getFreqValue(QString value, bool* ok);                        //获取频率值
    double getAmptValue(QString value, bool* ok);                        //获取幅度值
    double getdBValue(QString value, bool* ok);                          //获取dB值
    double getTimeValue(QString value, bool* ok);                        //获取时间值
    int getIntValue(QString value, bool* ok);                            //获取整数值
    double getDoubleValue(QString value, bool *ok);                      //获取浮点型值

    void checkLocalDirectory(void);                                      //检测本地路径
    int checkUsbDirectory(void);                                         //检测USB路径
    int connectToUsbDisk(void);                                          //连接闪存
    int disConnectFromUsbDisk(void);                                     //断开闪存连接
    int getDirFileCount(QString dirName);                                //获取文件数目

    bool isAllMarkerOff(void);                                           //所有频标关闭
    bool isTheMarkerOff(int index);                                      //指定频标关闭

    int isValidDate(QString value);                                      //日期合法
    int isValidTime(QString value);                                      //时间合法
    int setNewDate(QString value);                                       //设置新日期
    int setNewTime(QString value);                                       //设置新时间
    QString getNewDateString(QString value);                             //获取新日期字符串
    QString getNewTimeString(QString value);                             //获取新时间字符串

    int getPeakList(int traceIndex);                                     //获取峰值列表
    double* diff(double* output, const double * input, int n);           //数据求导

    void resetFreqCountParam(void);                                      //复位频率计数参数

    double setSpanOfMeasure(double value);                               //设置测量扫宽

    void setTuneFreq(double startFreq, double stopFreq);                 //设置自动搜索频率
    void setTuneBD2Freq(double freq);                                    //设置波段2频率
    int getTunePeakIndex(void);                                          //获取自动搜索峰值索引
    int finishAutoTune(double freq);                                     //结束自动搜索
    int locateAutoTune(double freq);                                     //自动搜索定位

    int setAutoTuneFreq(double startFreq, double stopFreq);              //设置自动调谐频率
    int getAutoTunePeakIndex(void);                                      //获取自动调谐峰值索引
    int setAutoTunePeakToCenter(void);                                   //设置自动调谐峰值至中心频率
    int setAutoTuneRefLevel(void);                                       //设置自动调谐参考电平

    void closeScpiPortOfAll(void);                                       //关闭所有通信端口
    void closeScpiPortOfLan(void);                                       //关闭LAN端口通信
    void closeScpiPortOfUsb(void);                                       //关闭USB端口通信
    void closeScpiPortOfGpib(void);                                      //关闭GPIB端口通信
    void closeScpiPortOfSerial(void);                                    //关闭COM端口通信

    int signalIsOnline(bool isCom);                                      //信号源在线
    int signalOutputFreqIsvalid(bool isCom, double value);               //信号源输出频率正常
    int signalOutputAmptIsvalid(bool isCom, double value);               //信号源输出幅度正常

    void setCalibrateParamOfAbsoluteAmplitude(void);                     //设置绝对幅度定标校准参数
    void setCalibrateParamOfIfAttenuation(double attrf, double attif);                           //设置衰减器校准参数
    void setCalibrateParamOfRfAttenuation(void);
    void setCalibrateDistributeAtt(double ref, double att1, double att2);//设置两级衰减器参数
    void setCalibrateParamOfIF(void);                                    //设置中频增益校准参数
    void setCalibrateDistributeIF(double value);                         //设置中频增益参数
    void setCalibrateParamOfRbw(void);                                   //设置分辨率校准参数
    void setCalibrateDistributeRbw(double value);                        //设置分辨率参数
    void setCalibrateParamOfFrequencyRespond(void);                      //设置频率响应校准参数
    void setCalibrateDistributeFrequencyRespond(int freqIndex, double ref, double rfatt, double ifatt);             //设置频率响应参数
    void setZcCalibrateParamOfIF(void);                                  //设置直采通道中频增益校准参数
    void setZcCalibrateDistributeIF(double value);                       //设置直采中频增益参数
    void setZcCalibrateParamOfFrequencyRespond(int freqIndex,  double rfatt, double ifatt);                    //设置直采通道频率响应校准参数
    void setZcCalibrateParamOfRbw(void);                                 //设置直采通道分辨率校准参数
    void setZcCalibrateParamOfAbsoluteAmplitude(void);                   //设置直采通道幅度定标标准参数
    void setZcCalibrateDistributeFrequencyRespond(int freqIndex, double ref, double att); //设置直采通道频率响应参数
    unsigned char getZcIfdown(double value);							 //获取直采通道中频增益下发值
    int getCalibrateRunParam(void);                                      //获取校准运行参数
    int getCalibrateValueOfAbsoluteAmplitude(void);                      //获取绝对幅度定标值

    int factoryCalibrationOfAmplitude(void);                             //幅度校准
    int factoryCalibrationOfAttenuation(void);                           //衰减器校准
    int factoryCalibrationOfIF(void);                                    //中频增益校准
    int factoryCalibrationOfBw(void);                                    //分辨率校准
    int factoryCalibrationOfFreqResp(void);                              //频响校准
    void setFrontendAtt(double, double);
    int getSystemTemperature(double* value);                             //获取系统温度

    double getErrorOfAttenuation(void);                                  //获取衰减器误差
    double getErrorOfAttenuationofPre(void);                             //获取前置放大衰减器误差
    double getErrorOfAttenuationofLowfreq(void);                         //获取低频直采衰减器误差
    double getErrorOfIF(void);                                           //获取中频增益误差
    double getErrorOfIFofLowfreq(void);                                  //获取直采低频增益误差
    double getErrorOfIFofLowfreqofPre(void);                             //获取直采通道前置放大中频增益误差
    double getErrorOfAttenuationofLowfreqofPre(void);                    //获取低频直采前置放大衰减器误差
    double getErrorOfIFofPre(void);                                      //获取前置放大中频增益误差
    double getErrorOfRbw(void);                                          //获取分辨率误差
    double getErrorOfRbwofPre(void);                                     //获取前置放大分辨率误差
    double getErrorOfRbwofLowfreq(void);                                 //获取直采低频分辨率误差
    double getErrorOfRbwofLowfreqofPre(void);							 //获取低频直采前置放大分辨率误差
    double getErrorOfLowFreqResp(int index);                             //获取直采频响误差
    double getErrorOfLowFreqRespofPre(int index);                        //获取直采通道前置放大频响误差
    double getErrorOfEmcRbw(void);                                       //获取EMC分辨率误差
    double getErrorOfFreqResp(int index);                                //获取频响误差
    double getErrorOfFreqRespofPre(int index);                           //获取前置放大频响误差
    double getErrorOfTemperature(void);                                  //获取温度误差
    double getErrorOfNetworkMeasure(int index);                          //获取网络测量误差

    QString getTheTranslatorString(QString enStr);                       //获取字符串
    void saveScreen(void);                                               //保存屏幕
    void outputMessage(QString title);

    bool connecToServerSocket(void);                          //连接服务器
    bool disConnectFromServerSocket(void);                    //断开服务器

    double dBmTomV(double value);
    void reDrawMenuFrame(void);                                          //刷新菜单面板

    char getZcAttVal(void);
    int getRecentFFTRbwIndex(double value);                             //获取rbw索引值
    int getRecentSweepRbwIndex(double value);												 //获取扫描模式rbw索引值
    void updateScanParam(void);
    void updateBandParam(void);											 //更新射频各波段下发值
    void setSweepBandIndex(void);											 //波段指示下发
    void setHighBandIndex(void);										 //高波段指示下发
    void setScanEnable(bool value);
    void restartScan(void);
    void setFTWOfDDS(void);
    void downLo1(void);									//第一本振下发
    void freqScanStart(void);							//频率扫描
    //void fftWinDown(void);								//fft窗函数下发
    void vbwDown(void);									//vbw 权值下发
    void cicDown(void);									//CIC滤波器下发
    void autoAmptAndReflevel(void);
  public slots:
    virtual void clientSocketDataArrive(int fd, int len);                //客户端数据到达

  private slots:
    virtual void readSerialData();                                       //读串口数据
    virtual void readUsbSlaveData();                                     //读USB从口数据
    virtual void readGpibData();                                         //读GPIB数据

    virtual void acceptClientSocketConnect(void);                        //接受客户端建立连接
    virtual void clientSocketDisConnect(void);                           //客户端断开连接
    virtual void clientSocketReadyRead(void);                            //客户端有数据到达
    virtual void clientSocketError(QAbstractSocket::SocketError);        //客户端发生错误
  signals:
    virtual void showAlarmMessage(QString title, QString msg);
    virtual void refreshScpiState(QString helpStr);                      //刷新SCPI状态
    virtual void refreshWindowState(void);                               //刷新窗口状态
    virtual void refreshMenuFrame(unsigned long mainCode, unsigned long subCode);                     //刷新按键状态
    virtual void resetUsbSlaveInterface(void);                           //复位USB从口
};

//Server Socket线程
class tSocketThread : public QThread
{
  Q_OBJECT
  public:
    virtual void run();                      //执行体
  signals:
    void clientSocketDataArrive(int fd, int len);
};


#endif
