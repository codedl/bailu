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

#define VER_MAJOR            3          //���汾��(ϵͳ��ܴ���)
#define VER_MINOR            0          //�ΰ汾��(�豸�ͺŴ���)
#define VER_REVISION         5          //�޶��汾��(��ݺ���λ)
#define VER_BUILD            180912     //����汾��(�·�+����)
#define VER_TAG						   0 					//0 beta   1 release
#define __var(var)			std::cout << "##var" << #var << "=" << var << std::endl 
  
#define __DEBUG           0          //Debug����
#define _DEBUG            0          //Debug����
#define DEBUG             0          //Debug����
#define __SWEEPPONTDEBUG  0          //ɨ��������Կ���
#define __CMDDEBUG        0          //cmdָ����Կ���
#define __KEYVALDEBUG     0			 		 //������ֵ��ӡ����
#define __COM__           0          //���ڼ����

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

extern char* idnHead;

#define SERIALBUFFERSIZE    8192        //���ڻ�������С
#define VECTORLABELCOUNT    17          //ʸ��ͼ�ı�����
#define AMPTGRATICULECOUNT  9           //���ȱ������
#define TIMESPECTEXTCOUNT   11          //ʱ��Ƶ���ı�����
#define OBWTEXTCOUNT        9           //ռ�ô����ı�����
#define ACPRTEXTCOUNT       32          //�ڵ������ı�����

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

extern char* usbDirName;                //USB�����ļ�������
extern char serialNo[32];               //�豸���к�

#define LOGOWIDTH            100        //LOGO���
#define TIMEWIDTH            150        //ʱ����
#define TOPHEIGHT            30         //�����߶�
#define BOTTOMHEIGHT         36         //�ײ��߶�
#define BUTTONHEIGHT         71         //��ť�߶�
#define BUTTONINTERVAL       3          //��ť���
#define FRAMELINEWIDTH       2          //�߿򰼿��
//#define CANVASWIDTH          0.8125     //ʸ�������ռLCD�ٷֱ�
#define CANVASWIDTH          0.8250     //ʸ�������ռLCD�ٷֱ�
#define TABLELEFT            50         //�����߾���
#define TABLERIGHT           12          //����ұ߾���
#define TABLETOP             48         //����ϱ����
#define TABLEWIDTH           600        //�����

#define TITLEWIDTH           0.5        //������ռʸ�����ٷֱ�
#define BORDERWIDTH          3          //��Ե���
#define TITLEHEIGHT          30         //�������߶�

#define INPUTHEIGHT          40         //�������߶�
#define HELPHEIGHT           36         //�������߶�
#define SCPIBTNSIZE          24         //SCPI��ť�ߴ�
#define BUTTONCOUNT          7          //������Ŀ
#define TEXTHEIGHT           22         //�ı��߶�
#define TABLESEP             10         //���߾�
#define KBDINPUTHEIGHT		 	 40			//�����������߶�

#define BACKGROUNDCOLOR      QColor(192, 192, 192)       //Ĭ�ϱ���ɫ
#define BUTTONUNSELECTCOLOR  QColor(192, 192, 192)       //��ťδѡ��ʱ��ɫ
//#define BUTTONSELECTCOLOR    QColor(230, 220, 220)       //��ťѡ��ʱ��ɫ
#define BUTTONSELECTCOLOR    QColor(230, 220, 200)       //��ťѡ��ʱ��ɫ

#define GRIDCOLOR            QColor(60, 60, 60)          //������ɫ
#define BUTTONENABLECOLOR    QColor(0, 0, 0)             //��ť����ʱ��ɫ
#define BUTTONDISABLECOLOR   QColor(140, 140, 200)       //��ť������ʱ��ɫ

#define fontEnglishBold      QFont("enb", 16, 75, false)  //Ӣ�Ĵ�����
#define fontEnglishLight     QFont("enl", 17, 50, false)  //Ӣ��ϸ����
#define fontChinese          QFont("ch", 16, 50, false)   //��������

#define MACADDRESS          "00405c260a52"

#define KBD_PORT 								"/dev/ttySAC2"
#define BAUDRATE 								B115200            //���̰崮�ڲ�����

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

extern double log10Table[100000];       //����������Ԥ��������, 0.0000 ~ 9.9999, total 100000 points

extern double sysErrValue;
extern double tempTraceData[TRACECOUNT][MAXDATASIZE];
extern bool sysLocked;
extern int lcdRefreshTimes;             //lcdˢ��ʱ��(ms)

extern QFont fontBold;                  //ϵͳ������
extern QFont fontLight;                 //ϵͳϸ����
extern QFont fontData;                  //ϵͳ��������
extern QFont fontDataBig;
extern QFont fontInput;                 //��������
extern QFont fontUnderline;             //���»�������
extern QFont fontSmall;                 //С����

extern selfCheckDef selfCheck;          //ϵͳ�Լ�
extern dataDef sysData;                 //ϵͳ�첨����

extern int SCREENWIDTH;                 //��Ļ���
extern int SCREENHEIGHT;                //��Ļ�߶�
extern unsigned char FIRMWARE_VER_MAJOR;//�̼����汾��
extern unsigned char FIRMWARE_VER_MINOR;//�̼��ΰ汾��
extern QRect rcCal;                     //У׼����

extern int firVerHandle;                //�̼��汾���
extern int keyHandle;                   //���̾��
extern int ifHandle;                    //��Ƶ���
//extern int ifDev;
extern int rfHandle;                    //��Ƶ���
extern int comHandle;                   //���ھ��
extern int srcHandle;                   //Դ���
extern int tmptHandle;                    //�¶Ⱦ��
extern int fftHandle;                   //FFT���
extern int demodHandle;                 //������
extern int iqHandle;                    //IQ���
extern int feHandle;										//��Ƶǰ�˾��
extern int ramHandle;
extern int tmcHandle;                   //TMC�豸���

extern int usbHandle;                   //USB�ӿھ��
extern int gpibHandle;                  //GPIB���
extern int ramADF4360Handle;            //ADF4360���
extern int gpioHandle;                  //gpio���
extern int iicHandle;                  //I2C���
extern bool isFFtnd;                    //�ж�FFT�Ƿ����

extern bool devControling;
extern bool iqSave;
extern int iqFlag;

extern mainKeyDef* sysKey;              //ϵͳ�˵�
extern mainKeyDef* nowMainKey;          //��ǰ����
extern subKeyDef* nowSubKey;            //��ǰ�Ӽ�

extern struct rfDataDef rfData;

extern char sockBuf[SOCKETBUF_SIZE];
extern int curSocket;
extern int intFlag;
extern unsigned long ifDataBuf[RAMDATASIZE];
#endif
