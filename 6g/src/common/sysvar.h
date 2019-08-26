#ifndef SYSVAR_H_
#define SYSVAR_H_

#include <QtCore/QDebug>
#include <QtGui/QFont>
#include <QtGui/QColor>
#include <QtCore/QMutex>
#include <QtGui/QStaticText>
#include <QtGui/QLabel>
#include "sysstruct.h"

#define __DEBUG         0          	//Debug开关
#define __DEBUGSCPI		0			//SCPI串口调试开关
#define __DEBUGKEY		0			//按键串口调试开关
#define __DEBUGDISPLAY	0			//显示串口调试开关

#define __SWEEPPONTDEBUG  0         //扫描点数测试开关

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


#define SERIALBUFFERSIZE    8192        //串口缓冲区大小
#define VECTORLABELCOUNT    17          //矢量图文本数量
#define AMPTGRATICULECOUNT  9           //幅度标尺数量
#define TIMESPECTEXTCOUNT   11          //时间频谱文本数量
#define OBWTEXTCOUNT        9           //占用带宽文本数量
#define ACPRTEXTCOUNT       32          //邻道测量文本数量

#define FILEROWCOUNT        15
#define FILECOLCOUNT        5

#define CONTINUATION 		1
#define MANUAL 				2
#define OTOO				3
#define OTCO				4

#define NO 					1
#define YES					2

#define INTERNAL			1
#define EXTERNAL			2

#define COMMEN				29
#define DATAEN				28
#define DATA				27

#define VER_MAJOR            3          	//主版本号（系统框架）
#define VER_MINOR            1          	//次版本号
#define VER_REVISION         0          	//修订版本号
#define VER_BUILD            190816  			//日期
#define VER_TAG				 			 0						//0:代表测试版，1:代表发布版

#define LOGOWIDTH            100        //LOGO宽度
#define TIMEWIDTH            150        //时间宽度
#define TOPHEIGHT            0         	//顶部高度
#define BOTTOMHEIGHT         30         //底部高度
#define FRAMELINEWIDTH       3          //边框凹宽度
#define CANVASWIDTH          0.85     	//矢量区宽度占LCD百分比
#define TABLELEFT            28         //表格左边距离
#define TABLERIGHT           2          //表格右边距离
#define TABLETOP             48         //表格上变距离
#define TABLEWIDTH           500        //表格宽度

#define TITLEWIDTH           0.5        //标题区占矢量区百分比
#define TITLEHEIGHT          0         	//标题区高度

#define INPUTHEIGHT          40         //输入区高度
#define HELPHEIGHT           30         //帮助区高度
#define SCPIBTNSIZE          24         //SCPI按钮尺寸



#define TEXTHEIGHT           22         //文本高度
#define TABLESEP             10         //表格边距

#define BORDERWIDTH          2          //边缘宽度
//#define ALLWIDTH			478
//#define ALLHEIGHT			272
#define LETFWIDTH			397//ALLWIDTH * CANVASWIDTH
#define RIGHTWIDTH			83//ALLWIDTH * ( 1 - CANVASWIDTH )

//================================整个界面===================================
#define WINALLWIDTH 480             //整个界面的宽度
#define WINALLHEIGHT 272            //整个界面的高度

//================================主参数显示区域==============================

#define WIN1X		0
#define WIN1Y		0
#define WIN1WIDTH	(WINALLWIDTH * 0.8)			//480*0.7
#define WIN1HEIGHT	WINALLHEIGHT				//272

#define WIN1_1X			0
#define WIN1_1Y			0
#define WIN1_1WIDTH		WIN1WIDTH					//480*0.7
#define WIN1_1HEIGHT	(WIN1HEIGHT * 0.17)		//272*0.17
#define WIN1_1_1X		WIN1_1X						//0
#define WIN1_1_1Y		WIN1_1Y						//0
#define WIN1_1_1WIDTH	(WIN1_1WIDTH* 0.58)		//480 * 0.7 * 0.6
#define WIN1_1_1HEIGHT	WIN1_1HEIGHT			//272*0.17
#define WIN1_1_2X		(WIN1_1WIDTH * 0.58)			//480 * 0.7 * 0.6
#define WIN1_1_2Y		WIN1_1Y						//0
#define WIN1_1_2WIDTH	(WIN1_1WIDTH * 0.42)		//480 * 0.7 * 0.4
#define WIN1_1_2HEIGHT	WIN1_1HEIGHT			//272*0.17

//================================状态显示区域==============================
#define WIN1_2X			0
#define WIN1_2Y			WIN1_1HEIGHT
#define WIN1_2WIDTH		WIN1WIDTH
#define WIN1_2HEIGHT	(WIN1HEIGHT * 0.08)

//================================输入显示区域==============================
#define WIN1_3X			0
#define WIN1_3Y			(WIN1_1HEIGHT + WIN1_2HEIGHT)
#define WIN1_3WIDTH		WIN1WIDTH
#define WIN1_3HEIGHT	(WIN1HEIGHT * 0.1)

//#define CHANNELPARANUME	5
//#define CHANNELNUME		2

//================================参数栏显示区域==============================
#define WIN1_4X			0
#define WIN1_4Y			(WIN1_1HEIGHT + WIN1_2HEIGHT + WIN1_3HEIGHT)
#define WIN1_4WIDTH		WIN1WIDTH
#define WIN1_4HEIGHT	(WIN1HEIGHT * 0.59 - 1)

//================================参数栏显示区域==============================
#define WIN1_5X			0
#define WIN1_5Y			(WIN1_1HEIGHT + WIN1_2HEIGHT + WIN1_3HEIGHT + WIN1_4HEIGHT)
#define WIN1_5WIDTH		WIN1WIDTH
#define WIN1_5HEIGHT	(WIN1HEIGHT * 0.06 + 1)

//================================按钮显示区域================================
#define WIN2X			(WINALLWIDTH * 0.8)
#define WIN2Y			0
#define WIN2WIDTH		(WINALLWIDTH * 0.2)
#define WIN2HEIGHT		WINALLHEIGHT

#define BUTTONTITLEX		(WIN2X + 1)
#define BUTTONTITLEY		(WIN2Y + 1)
#define BUTTONTITLEHEIGHT	(WIN2HEIGHT /5 / 3)
#define BUTTONTITLEWIDTH	(WIN2WIDTH)
#define BUTTONCOUNT         5          //按键数目
#define BUTTONWIDTH         (WIN2WIDTH - 2)         //按钮宽度
#define BUTTONHEIGHT        ((WIN2HEIGHT - (BUTTIONTITLEHEIGHT * 2) / 5)         //按钮高度
#define BUTTONINTERVAL      1          //按钮间隔


//====================================================================================
//#define BACKCOLORALLWIN		QColor(60,30,100)
//#define BACKCOLORWIN1		QColor(128,128,64)
//#define BACKCOLORWIN2		QColor(192,192,192)
//#define BACKCOLORWIN3		QColor(128,128,64)
//#define BACKCOLORWIN4		QColor(128,128,64)
#define BACKCOLORALLWIN		BACKGROUNDCOLOR
#define BACKCOLORWIN1		QColor(255, 255, 255)
#define BACKCOLORREFOFFSET	QColor(220, 220, 220)
#define BACKCOLORWIN2		QColor(90,95,226)
#define BACKCOLORWIN3		QColor(255, 255, 255)
#define BACKCOLORWIN4		QColor(255, 255, 255)
#define BACKCOLORWIN5		QColor(220, 220, 200)

#define BACKGROUNDCOLOR     QColor(240, 240, 240)       //默认背景色  全屏背景色
//#define BUTTONUNSELECTCOLOR QColor(192, 192, 192)       //按钮未选中时颜色
//#define BUTTONSELECTCOLOR   QColor(230, 220, 200)       //按钮选中时颜色
#define BUTTONUNSELECTCOLOR QColor(180, 170, 230)       	//按钮未选中时颜色
#define BUTTONSELECTCOLOR   QColor(100,100,226)       	//按钮选中时颜色
#define BUTTONTESTCOLOR   	QColor(107, 152, 24)        //测试颜色

#define GRIDCOLOR           QColor(60, 60, 60)          //网格颜色
#define BUTTONENABLECOLOR   QColor(0, 0, 0)             //按钮可用时颜色
#define BUTTONDISABLECOLOR  QColor(140, 140, 200)       //按钮不可用时颜色

#define LEFTROUNDCOLOR		QColor(60,30,100)		//左侧默认背景色


//#define fontEnglishBold      QFont("enb", 13, 75, false)  //英文粗字体
//#define fontEnglishLight     QFont("enl", 13, 50, false)  //英文细字体
//#define fontChinese          QFont("ch", 14, 50, false)   //中文字体

//通用字体
#define fontEnglishBold      QFont("enb", 14, 50, false)  //英文粗字体
#define fontEnglishSmallBold      QFont("enb", 12, 75, false)  //小英文粗字体
#define fontEnglishLight     QFont("enl", 14, 30, false)  //英文细字体		帮助栏中英文显示
#define fontChinese          QFont("ch", 14, 50, false)   //中文字体 包括菜单内的所有中文字体

//#define fontEnglishSmall     QFont("enl", 10, 50, false)   //英文字体
#define fontEnglishSmall     QFont("enl", 6, 50, false)   //英文字体

#define fontEnglish			 QFont("enl", 18, 50, false)

#define fontCalen              QFont("enb", 12, 50, false) //英文校准字体
#define fontCalch              QFont("ch", 12, 50, false)  //中文校准字体

#define fontMarkerBold      QFont("ch", 12, 50, false)
#define fontMarkerLight     QFont("enl", 12, 50, false)

//SG10301字体
#define fontChinesePara		QFont("ch", 14, 50, false)		//参数区中的字体，包括标题区和参数区

//PG100字体
#define fontWin1En			QFont("enb", 24, 50, false)			//主参数显示区域字体
#define fontWin1Ch			QFont("ch", 12, 50, false)
#define fontWin2En			QFont("enb", 20, 75, false)			//输入栏显示区域字体
#define fontWin2Ch			QFont("ch", 18, 50, false)
#define fontWin3En1			QFont("enl", 20, 50, false)			//一览表显示区域字体
#define fontWin3En2			QFont("enl", 20, 50, false)			//一览表显示区域字体
#define fontWin3Ch			QFont("ch", 20, 50, false)
#define fontWin4En			QFont("enl", 15, 50, false)			//帮助栏显示区域字体
#define fontWin4Ch			QFont("ch", 15, 50, false)
#define fontWin5En
#define fontWin5Ch

#define fontParaEnglishBold      QFont("enb", 18, 75, false)  //英文粗字体	主参数英文显示   	输入参数名英文显示
#define fontParaEnglishBold1      QFont("enb", 26, 75, false)  //英文粗字体	输入参数英文显示
#define fontParaChinese          QFont("ch", 18, 50, false)   //中文字体		主参数中文显示 	输入参数名中文显示

#define fontListEn			QFont("enl", 14, 50, false)		//一览表英文显示
#define fontListCn			QFont("ch", 14, 50, false)		//一览表中文显示

//#define fontEnglishLight     QFont("enl", 12, 50, false)  //英文细字体		帮助栏中英文显示



extern QMutex mutexDisc;
extern unsigned char usbRcvBuf[1280];

//extern QStaticText stLabelPeakTable[30];
//extern QStaticText stLabelMarkerTable[30];
//extern QStaticText stLabelMarker[5];
//extern QStaticText stLabelMarkerR[5];
//extern QStaticText stLabel[VECTORLABELCOUNT];
//extern QStaticText stLabelGraticule[AMPTGRATICULECOUNT];
//extern QStaticText stLabelTimeSpec[TIMESPECTEXTCOUNT];
//extern QStaticText stLabelObw[OBWTEXTCOUNT];
//extern QStaticText stLabelAcpr[ACPRTEXTCOUNT];
extern QStaticText stLabelCalibrate;

extern QStaticText stLabelFileDiskSpace;
extern QStaticText stLabelFilePage;
extern QStaticText stLabelFileRecord[FILEROWCOUNT][FILECOLCOUNT];


extern QFont fontBold;                  //系统粗字体
extern QFont fontLight;                 //系统细字体
extern QFont fontData;                  //系统数据字体
extern QFont fontDataSmall;             //系统数据小字体
extern QFont fontDataBig;
extern QFont fontInput;                 //输入字体
extern QFont fontUnderline;             //带下划线字体
extern QFont fontSmall;                 //小字体
extern QFont fontMarker;                //marker 字体大小
extern QFont fontCalibration;           //校准参数 字体大小

//PG100字体
extern QFont fontWin1;
extern QFont fontWin2;
extern QFont fontWin3;
extern QFont fontWin4;
extern QFont fontWin5;

extern int stLabelCalibrateProgress;

extern char* usbDirName;                //USB导出文件夹名称
extern char serialNo[32];               //设备序列号

extern dataDef sysData;                 //系统检波数据
//extern powerOffDataDef sysPowerOffData;  //系统开机用户状态
extern dataDef sysPowerOffData;  //系统开机用户状态（原本使用的是powerOffDataDef结构体，现在使用dataDef结构体）

extern struct rfDataDef rfData;

extern int SCREENWIDTH;                 //屏幕宽度
extern int SCREENHEIGHT;                //屏幕高度
extern QRect rcCal;                     //校准区域

extern char* idnHead;
extern int keyHandle;                   //键盘句柄
extern int comHandle;                   //串口句柄
extern int usbHandle;                   //USB从口句柄

extern bool isNeedCloseDemod;

extern mainKeyDef* sysKey;              //系统菜单
extern mainKeyDef* nowMainKey;          //当前主键
extern subKeyDef* nowSubKey;            //当前子键

extern int lcdRefreshTimes;

extern bool TriggerSwitch, Channel1AnodeSwitch, Channel1CathodeSwitch, Channel2AnodeSwitch, Channel2CathodeSwitch;
#endif
