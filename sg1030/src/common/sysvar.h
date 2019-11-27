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


#define VER_MAJOR            3          	//���汾�ţ�ϵͳ��ܣ�
#define VER_MINOR            1          	//�ΰ汾��
#define VER_REVISION         1          	//�޶��汾��
#define VER_BUILD            190718  		//����
#define VER_TAG				 1			    //0:������԰棬1:��������



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


extern  QString skyDirName;
extern QString UpdateDirName;
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

#define BUTTONCOUNT          6          //������Ŀ
#define BUTTONWIDTH          80         //��ť���
#define BUTTONHEIGHT         40         //��ť�߶�
#define BUTTONINTERVAL       2          //��ť���

#define TEXTHEIGHT           22         //�ı��߶�
#define TABLESEP             10         //���߾�

#define BORDERWIDTH          2          //��Ե���
#define ALLWIDTH			478
#define ALLHEIGHT			272
#define LETFWIDTH			397//ALLWIDTH * CANVASWIDTH
#define RIGHTWIDTH			83//ALLWIDTH * ( 1 - CANVASWIDTH )

//================================��������ʾ����==============================
#define WIN1X		0
#define WIN1Y		0
#define WIN1WIDTH	ALLWIDTH
#define WIN1HEIGHT	40

#define WIN1_1WIDTH		0.60
#define WIN1_2WIDTH		0.40

#define WIN1_1_1WIDTH	0.00
#define WIN1_1_2WIDTH	0.80
#define WIN1_1_3WIDTH	0.20

#define WIN1_2_1WIDTH	0.30
#define WIN1_2_2WIDTH	0.40
#define WIN1_2_3WIDTH	0.30

//================================״̬��ʾ����==============================
#define WIN2X		0
#define WIN2Y		WIN1HEIGHT
#define WIN2WIDTH	ALLWIDTH
#define WIN2HEIGHT	20//30

#define WIN2_1WIDTH		0.40
#define WIN2_2WIDTH		0.40
#define WIN2_3WIDTH		0.20

//================================���� ��ʾ����==============================
#define WIN3X		0
#define WIN3Y		WIN1HEIGHT + WIN2HEIGHT
#define WIN3WIDTH	ALLWIDTH
#define WIN3HEIGHT	30

#define WIN3_1WIDTH		0.40
#define WIN3_2WIDTH		0.40
#define WIN3_3WIDTH		0.20

#define CHANNELPARANUME	5
#define CHANNELNUME		2

//================================��������ʾ����==============================
#define WIN4X		0
#define WIN4Y		WIN1HEIGHT + WIN2HEIGHT + WIN3HEIGHT
#define WIN4WIDTH	ALLWIDTH
#define WIN4HEIGHT	ALLHEIGHT - (WIN1HEIGHT + WIN2HEIGHT + WIN3HEIGHT + 40)

//================================��ť��ʾ����================================
#define WIN5X		0
#define WIN5Y		ALLHEIGHT - 40
#define WIN5WIDTH	ALLWIDTH
#define WIN5HEIGHT	40

//====================================================================================
//#define BACKCOLORALLWIN		QColor(60,30,100)
//#define BACKCOLORWIN1		QColor(128,128,64)
//#define BACKCOLORWIN2		QColor(192,192,192)
//#define BACKCOLORWIN3		QColor(128,128,64)
//#define BACKCOLORWIN4		QColor(128,128,64)
#define BACKCOLORALLWIN		BACKGROUNDCOLOR
#define BACKCOLORWIN1		QColor(255, 255, 255)
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
#define fontEnglishLight     QFont("enl", 14, 30, false)  //Ӣ��ϸ����		��������Ӣ����ʾ
#define fontChinese          QFont("ch", 12, 50, false)   //�������� �����˵��ڵ�������������

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
