#ifndef SYSVAR_H_
#define SYSVAR_H_

#include <QtCore/QDebug>
#include <QtGui/QFont>
#include <QtGui/QColor>
#include <QtCore/QMutex>
#include <QtGui/QStaticText>
#include <QtGui/QLabel>
#include "sysstruct.h"

#define __DEBUG         0          	//Debug����
#define __DEBUGSCPI		0			//SCPI���ڵ��Կ���
#define __DEBUGKEY		0			//�������ڵ��Կ���
#define __DEBUGDISPLAY	0			//��ʾ���ڵ��Կ���

#define __SWEEPPONTDEBUG  0         //ɨ��������Կ���

#ifdef __DEBUG                       //Debug������Ϣ
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


#define SERIALBUFFERSIZE    8192        //���ڻ�������С
#define VECTORLABELCOUNT    17          //ʸ��ͼ�ı�����
#define AMPTGRATICULECOUNT  9           //���ȱ������
#define TIMESPECTEXTCOUNT   11          //ʱ��Ƶ���ı�����
#define OBWTEXTCOUNT        9           //ռ�ô����ı�����
#define ACPRTEXTCOUNT       32          //�ڵ������ı�����

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

#define VER_MAJOR            3          	//���汾�ţ�ϵͳ��ܣ�
#define VER_MINOR            1          	//�ΰ汾��
#define VER_REVISION         0          	//�޶��汾��
#define VER_BUILD            190816  			//����
#define VER_TAG				 			 0						//0:������԰棬1:��������

#define LOGOWIDTH            100        //LOGO���
#define TIMEWIDTH            150        //ʱ����
#define TOPHEIGHT            0         	//�����߶�
#define BOTTOMHEIGHT         30         //�ײ��߶�
#define FRAMELINEWIDTH       3          //�߿򰼿��
#define CANVASWIDTH          0.85     	//ʸ�������ռLCD�ٷֱ�
#define TABLELEFT            28         //�����߾���
#define TABLERIGHT           2          //����ұ߾���
#define TABLETOP             48         //����ϱ����
#define TABLEWIDTH           500        //�����

#define TITLEWIDTH           0.5        //������ռʸ�����ٷֱ�
#define TITLEHEIGHT          0         	//�������߶�

#define INPUTHEIGHT          40         //�������߶�
#define HELPHEIGHT           30         //�������߶�
#define SCPIBTNSIZE          24         //SCPI��ť�ߴ�



#define TEXTHEIGHT           22         //�ı��߶�
#define TABLESEP             10         //���߾�

#define BORDERWIDTH          2          //��Ե���
//#define ALLWIDTH			478
//#define ALLHEIGHT			272
#define LETFWIDTH			397//ALLWIDTH * CANVASWIDTH
#define RIGHTWIDTH			83//ALLWIDTH * ( 1 - CANVASWIDTH )

//================================��������===================================
#define WINALLWIDTH 480             //��������Ŀ��
#define WINALLHEIGHT 272            //��������ĸ߶�

//================================��������ʾ����==============================

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

//================================״̬��ʾ����==============================
#define WIN1_2X			0
#define WIN1_2Y			WIN1_1HEIGHT
#define WIN1_2WIDTH		WIN1WIDTH
#define WIN1_2HEIGHT	(WIN1HEIGHT * 0.08)

//================================������ʾ����==============================
#define WIN1_3X			0
#define WIN1_3Y			(WIN1_1HEIGHT + WIN1_2HEIGHT)
#define WIN1_3WIDTH		WIN1WIDTH
#define WIN1_3HEIGHT	(WIN1HEIGHT * 0.1)

//#define CHANNELPARANUME	5
//#define CHANNELNUME		2

//================================��������ʾ����==============================
#define WIN1_4X			0
#define WIN1_4Y			(WIN1_1HEIGHT + WIN1_2HEIGHT + WIN1_3HEIGHT)
#define WIN1_4WIDTH		WIN1WIDTH
#define WIN1_4HEIGHT	(WIN1HEIGHT * 0.59 - 1)

//================================��������ʾ����==============================
#define WIN1_5X			0
#define WIN1_5Y			(WIN1_1HEIGHT + WIN1_2HEIGHT + WIN1_3HEIGHT + WIN1_4HEIGHT)
#define WIN1_5WIDTH		WIN1WIDTH
#define WIN1_5HEIGHT	(WIN1HEIGHT * 0.06 + 1)

//================================��ť��ʾ����================================
#define WIN2X			(WINALLWIDTH * 0.8)
#define WIN2Y			0
#define WIN2WIDTH		(WINALLWIDTH * 0.2)
#define WIN2HEIGHT		WINALLHEIGHT

#define BUTTONTITLEX		(WIN2X + 1)
#define BUTTONTITLEY		(WIN2Y + 1)
#define BUTTONTITLEHEIGHT	(WIN2HEIGHT /5 / 3)
#define BUTTONTITLEWIDTH	(WIN2WIDTH)
#define BUTTONCOUNT         5          //������Ŀ
#define BUTTONWIDTH         (WIN2WIDTH - 2)         //��ť���
#define BUTTONHEIGHT        ((WIN2HEIGHT - (BUTTIONTITLEHEIGHT * 2) / 5)         //��ť�߶�
#define BUTTONINTERVAL      1          //��ť���


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

#define BACKGROUNDCOLOR     QColor(240, 240, 240)       //Ĭ�ϱ���ɫ  ȫ������ɫ
//#define BUTTONUNSELECTCOLOR QColor(192, 192, 192)       //��ťδѡ��ʱ��ɫ
//#define BUTTONSELECTCOLOR   QColor(230, 220, 200)       //��ťѡ��ʱ��ɫ
#define BUTTONUNSELECTCOLOR QColor(180, 170, 230)       	//��ťδѡ��ʱ��ɫ
#define BUTTONSELECTCOLOR   QColor(100,100,226)       	//��ťѡ��ʱ��ɫ
#define BUTTONTESTCOLOR   	QColor(107, 152, 24)        //������ɫ

#define GRIDCOLOR           QColor(60, 60, 60)          //������ɫ
#define BUTTONENABLECOLOR   QColor(0, 0, 0)             //��ť����ʱ��ɫ
#define BUTTONDISABLECOLOR  QColor(140, 140, 200)       //��ť������ʱ��ɫ

#define LEFTROUNDCOLOR		QColor(60,30,100)		//���Ĭ�ϱ���ɫ


//#define fontEnglishBold      QFont("enb", 13, 75, false)  //Ӣ�Ĵ�����
//#define fontEnglishLight     QFont("enl", 13, 50, false)  //Ӣ��ϸ����
//#define fontChinese          QFont("ch", 14, 50, false)   //��������

//ͨ������
#define fontEnglishBold      QFont("enb", 14, 50, false)  //Ӣ�Ĵ�����
#define fontEnglishSmallBold      QFont("enb", 12, 75, false)  //СӢ�Ĵ�����
#define fontEnglishLight     QFont("enl", 14, 30, false)  //Ӣ��ϸ����		��������Ӣ����ʾ
#define fontChinese          QFont("ch", 14, 50, false)   //�������� �����˵��ڵ�������������

//#define fontEnglishSmall     QFont("enl", 10, 50, false)   //Ӣ������
#define fontEnglishSmall     QFont("enl", 6, 50, false)   //Ӣ������

#define fontEnglish			 QFont("enl", 18, 50, false)

#define fontCalen              QFont("enb", 12, 50, false) //Ӣ��У׼����
#define fontCalch              QFont("ch", 12, 50, false)  //����У׼����

#define fontMarkerBold      QFont("ch", 12, 50, false)
#define fontMarkerLight     QFont("enl", 12, 50, false)

//SG10301����
#define fontChinesePara		QFont("ch", 14, 50, false)		//�������е����壬�����������Ͳ�����

//PG100����
#define fontWin1En			QFont("enb", 24, 50, false)			//��������ʾ��������
#define fontWin1Ch			QFont("ch", 12, 50, false)
#define fontWin2En			QFont("enb", 20, 75, false)			//��������ʾ��������
#define fontWin2Ch			QFont("ch", 18, 50, false)
#define fontWin3En1			QFont("enl", 20, 50, false)			//һ������ʾ��������
#define fontWin3En2			QFont("enl", 20, 50, false)			//һ������ʾ��������
#define fontWin3Ch			QFont("ch", 20, 50, false)
#define fontWin4En			QFont("enl", 15, 50, false)			//��������ʾ��������
#define fontWin4Ch			QFont("ch", 15, 50, false)
#define fontWin5En
#define fontWin5Ch

#define fontParaEnglishBold      QFont("enb", 18, 75, false)  //Ӣ�Ĵ�����	������Ӣ����ʾ   	���������Ӣ����ʾ
#define fontParaEnglishBold1      QFont("enb", 26, 75, false)  //Ӣ�Ĵ�����	�������Ӣ����ʾ
#define fontParaChinese          QFont("ch", 18, 50, false)   //��������		������������ʾ 	���������������ʾ

#define fontListEn			QFont("enl", 14, 50, false)		//һ����Ӣ����ʾ
#define fontListCn			QFont("ch", 14, 50, false)		//һ����������ʾ

//#define fontEnglishLight     QFont("enl", 12, 50, false)  //Ӣ��ϸ����		��������Ӣ����ʾ



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


extern QFont fontBold;                  //ϵͳ������
extern QFont fontLight;                 //ϵͳϸ����
extern QFont fontData;                  //ϵͳ��������
extern QFont fontDataSmall;             //ϵͳ����С����
extern QFont fontDataBig;
extern QFont fontInput;                 //��������
extern QFont fontUnderline;             //���»�������
extern QFont fontSmall;                 //С����
extern QFont fontMarker;                //marker �����С
extern QFont fontCalibration;           //У׼���� �����С

//PG100����
extern QFont fontWin1;
extern QFont fontWin2;
extern QFont fontWin3;
extern QFont fontWin4;
extern QFont fontWin5;

extern int stLabelCalibrateProgress;

extern char* usbDirName;                //USB�����ļ�������
extern char serialNo[32];               //�豸���к�

extern dataDef sysData;                 //ϵͳ�첨����
//extern powerOffDataDef sysPowerOffData;  //ϵͳ�����û�״̬
extern dataDef sysPowerOffData;  //ϵͳ�����û�״̬��ԭ��ʹ�õ���powerOffDataDef�ṹ�壬����ʹ��dataDef�ṹ�壩

extern struct rfDataDef rfData;

extern int SCREENWIDTH;                 //��Ļ���
extern int SCREENHEIGHT;                //��Ļ�߶�
extern QRect rcCal;                     //У׼����

extern char* idnHead;
extern int keyHandle;                   //���̾��
extern int comHandle;                   //���ھ��
extern int usbHandle;                   //USB�ӿھ��

extern bool isNeedCloseDemod;

extern mainKeyDef* sysKey;              //ϵͳ�˵�
extern mainKeyDef* nowMainKey;          //��ǰ����
extern subKeyDef* nowSubKey;            //��ǰ�Ӽ�

extern int lcdRefreshTimes;

extern bool TriggerSwitch, Channel1AnodeSwitch, Channel1CathodeSwitch, Channel2AnodeSwitch, Channel2CathodeSwitch;
#endif
