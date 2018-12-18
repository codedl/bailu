#ifndef SYSVAR_H_
#define SYSVAR_H_

#include <QtCore/QDebug>
#include <QtGui/QFont>
#include <QtGui/QColor>
#include <QtCore/QMutex>
#include <QtGui/QStaticText>
#include <QtGui/QLabel>
#include "sysstruct.h"
#include <iostream>

#define VER_MAJOR            3          //主版本号(系统框架代号)
#define VER_MINOR            0          //次版本号(设备型号代码)
#define VER_REVISION         5          //修订版本号(年份后两位)
#define VER_BUILD            180912     //编译版本号(月份+日期)
#define VER_TAG						   0 					//0 beta   1 release
#define __var(var)			std::cout << "##var" << #var << "=" << var << std::endl 
  
#define __DEBUG           0          //Debug开关
#define _DEBUG            0          //Debug开关
#define DEBUG             0          //Debug开关
#define __SWEEPPONTDEBUG  0          //扫描点数测试开关
#define __CMDDEBUG        0          //cmd指令测试开关
#define __KEYVALDEBUG     0			 		 //按键数值打印开关
#define __COM__           0          //串口激活开关

#ifdef __DEBUG                       //Debug错误信息
  #if __DEBUG
    #define PRINTERR(file, function, line, err)\
    {\
      qDebug() << "--> ["<< __TIME__ << "]" << file << ":" << line << ", function:" << function <<", message:" << err;\
    }

    #define PRINTMSG(msg)\
    {\
      qDebug() << msg;\
    }
  #else
    #define PRINTERR(file, function, line, err) {}
    #define PRINTMSG(msg) {}
  #endif
#else
  #define PRINTERR(file, function, line, err) {}
  #define PRINTMSG(msg) {}
#endif

extern char* idnHead;

#define SERIALBUFFERSIZE    8192        //串口缓冲区大小
#define VECTORLABELCOUNT    17          //矢量图文本数量
#define AMPTGRATICULECOUNT  9           //幅度标尺数量
#define TIMESPECTEXTCOUNT   11          //时间频谱文本数量
#define OBWTEXTCOUNT        9           //占用带宽文本数量
#define ACPRTEXTCOUNT       32          //邻道测量文本数量

#define FILEROWCOUNT        15
#define FILECOLCOUNT        5

#define SOCKETBUF_SIZE           (512 * 1024)

extern QMutex mutexDisc;
extern QMutex mutexFFT;
extern QMutex mutexDemod;
extern QMutex mutexAmpt;
extern QMutex mutexPassFail;
extern QMutex mutexTrig;
extern QMutex mutexZc;
extern QMutex mutexIF;

extern unsigned char usbRcvBuf[1280];

extern QStaticText stLabelPeakTable[30];
extern QStaticText stLabelMarkerTable[30];
extern QStaticText stLabelMarker[MARKERCOUNT];
extern QStaticText stLabelMarkerR[MARKERCOUNT];
extern QStaticText stLabel[VECTORLABELCOUNT];
extern QStaticText stLabelGraticule[AMPTGRATICULECOUNT];
extern QStaticText stLabelTimeSpec[TIMESPECTEXTCOUNT];
extern QStaticText stLabelObw[OBWTEXTCOUNT];
extern QStaticText stLabelAcpr[ACPRTEXTCOUNT];
extern QStaticText stLabelCalibrate;

extern QStaticText stLabelFileDiskSpace;
extern QStaticText stLabelFilePage;
extern QStaticText stLabelFileRecord[FILEROWCOUNT][FILECOLCOUNT];

extern int stLabelCalibrateProgress;

extern char* usbDirName;                //USB导出文件夹名称
extern char serialNo[32];               //设备序列号

#define LOGOWIDTH            100        //LOGO宽度
#define TIMEWIDTH            150        //时间宽度
#define TOPHEIGHT            30         //顶部高度
#define BOTTOMHEIGHT         36         //底部高度
#define BUTTONHEIGHT         71         //按钮高度
#define BUTTONINTERVAL       3          //按钮间隔
#define FRAMELINEWIDTH       2          //边框凹宽度
//#define CANVASWIDTH          0.8125     //矢量区宽度占LCD百分比
#define CANVASWIDTH          0.8250     //矢量区宽度占LCD百分比
#define TABLELEFT            50         //表格左边距离
#define TABLERIGHT           12          //表格右边距离
#define TABLETOP             48         //表格上变距离
#define TABLEWIDTH           600        //表格宽度

#define TITLEWIDTH           0.5        //标题区占矢量区百分比
#define BORDERWIDTH          3          //边缘宽度
#define TITLEHEIGHT          30         //标题区高度

#define INPUTHEIGHT          40         //输入区高度
#define HELPHEIGHT           36         //帮助区高度
#define SCPIBTNSIZE          24         //SCPI按钮尺寸
#define BUTTONCOUNT          7          //按键数目
#define TEXTHEIGHT           22         //文本高度
#define TABLESEP             10         //表格边距
#define KBDINPUTHEIGHT		 	 40			//键盘输入区高度

#define BACKGROUNDCOLOR      QColor(192, 192, 192)       //默认背景色
#define BUTTONUNSELECTCOLOR  QColor(192, 192, 192)       //按钮未选中时颜色
//#define BUTTONSELECTCOLOR    QColor(230, 220, 220)       //按钮选中时颜色
#define BUTTONSELECTCOLOR    QColor(230, 220, 200)       //按钮选中时颜色

#define GRIDCOLOR            QColor(60, 60, 60)          //网格颜色
#define BUTTONENABLECOLOR    QColor(0, 0, 0)             //按钮可用时颜色
#define BUTTONDISABLECOLOR   QColor(140, 140, 200)       //按钮不可用时颜色

#define fontEnglishBold      QFont("enb", 16, 75, false)  //英文粗字体
#define fontEnglishLight     QFont("enl", 17, 50, false)  //英文细字体
#define fontChinese          QFont("ch", 16, 50, false)   //中文字体

#define MACADDRESS          "00405c260a52"

#define KBD_PORT 								"/dev/ttySAC2"
#define BAUDRATE 								B115200            //键盘板串口波特率

#define DEVICE_NAME_COM     		"/dev/ttySAC0"
#define DEVICE_NAME_USB     		"/dev/ttyGS0"
#define DEVICE_NAME_GPIB    		"/dev/sax_drv_gpib"

#define DEVICE_NAME_RF					"/dev/BL_TQ3358_RF"
//#define DEVICE_NAME_IF					"/dev/BL_TQ3358_IF"
#define DEVICE_NAME_ADF4360 		"/dev/BL_TQ3358_ADF4360"
#define	DEVICE_NAME_WD					"/dev/BL_TQ3358_FPGA"
#define DEVICE_NAME_IQ					"/dev/BL_TQ3358_IQ"
#define DEVICE_NAME_VERSION			"/dev/BL_TQ3358_VERSION"
//#define	DEVICE_NAME_FFT					"/dev/bl_dsa_fft"
//#define	DEVICE_NAME_DEMOD				"/dev/bl_dsa_demod"
#define DEVICE_NAME_FE          "/dev/BL_TQ3358_FE"
#define DEVICE_NAME_IF  		"BL_TQ3358_IF"
#define DEVICE_NAME_RAM			"/dev/BL_TQ3358_RAM"

extern double log10Table[100000];       //基本对数表预定义数据, 0.0000 ~ 9.9999, total 100000 points

extern double sysErrValue;
extern double tempTraceData[TRACECOUNT][MAXDATASIZE];
extern bool sysLocked;
extern int lcdRefreshTimes;             //lcd刷新时间(ms)

extern QFont fontBold;                  //系统粗字体
extern QFont fontLight;                 //系统细字体
extern QFont fontData;                  //系统数据字体
extern QFont fontDataBig;
extern QFont fontInput;                 //输入字体
extern QFont fontUnderline;             //带下划线字体
extern QFont fontSmall;                 //小字体

extern selfCheckDef selfCheck;          //系统自检
extern dataDef sysData;                 //系统检波数据

extern int SCREENWIDTH;                 //屏幕宽度
extern int SCREENHEIGHT;                //屏幕高度
extern unsigned char FIRMWARE_VER_MAJOR;//固件主版本号
extern unsigned char FIRMWARE_VER_MINOR;//固件次版本号
extern QRect rcCal;                     //校准区域

extern int firVerHandle;                //固件版本句柄
extern int keyHandle;                   //键盘句柄
extern int ifHandle;                    //中频句柄
//extern int ifDev;
extern int rfHandle;                    //射频句柄
extern int comHandle;                   //串口句柄
extern int srcHandle;                   //源句柄
extern int tmptHandle;                    //温度句柄
extern int fftHandle;                   //FFT句柄
extern int demodHandle;                 //解调句柄
extern int iqHandle;                    //IQ句柄
extern int feHandle;										//射频前端句柄
extern int ramHandle;
extern int tmcHandle;                   //TMC设备句柄

extern int usbHandle;                   //USB从口句柄
extern int gpibHandle;                  //GPIB句柄
extern int ramADF4360Handle;            //ADF4360句柄
extern int gpioHandle;                  //gpio句柄
extern int iicHandle;                  //I2C句柄
extern bool isFFtnd;                    //判断FFT是否结束

extern bool devControling;
extern bool iqSave;
extern int iqFlag;

extern mainKeyDef* sysKey;              //系统菜单
extern mainKeyDef* nowMainKey;          //当前主键
extern subKeyDef* nowSubKey;            //当前子键

extern struct rfDataDef rfData;

extern char sockBuf[SOCKETBUF_SIZE];
extern int curSocket;
extern int intFlag;
extern unsigned long ifDataBuf[RAMDATASIZE];
#endif
