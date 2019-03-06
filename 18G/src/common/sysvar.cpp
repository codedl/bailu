#include "sysvar.h"

char* idnHead = "Company";

QMutex mutexDisc(QMutex::Recursive);
QMutex mutexFFT(QMutex::Recursive);
QMutex mutexDemod(QMutex::Recursive);
QMutex mutexAmpt(QMutex::Recursive);
QMutex mutexPassFail(QMutex::Recursive);
QMutex mutexTrig(QMutex::Recursive);
QMutex mutexZc(QMutex::Recursive);
QMutex mutexIF(QMutex::Recursive);

unsigned char usbRcvBuf[1280];

QStaticText stLabelPeakTable[30];
QStaticText stLabelMarkerTable[30];
QStaticText stLabelMarker[MARKERCOUNT];
QStaticText stLabelMarkerR[MARKERCOUNT];
QStaticText stLabel[VECTORLABELCOUNT];
QStaticText stLabelGraticule[AMPTGRATICULECOUNT];
QStaticText stLabelTimeSpec[TIMESPECTEXTCOUNT];
QStaticText stLabelObw[OBWTEXTCOUNT];
QStaticText stLabelAcpr[ACPRTEXTCOUNT];
QStaticText stLabelCalibrate;

QStaticText stLabelFileDiskSpace;
QStaticText stLabelFilePage;
QStaticText stLabelFileRecord[FILEROWCOUNT][FILECOLCOUNT];

int stLabelCalibrateProgress;

char* usbDirName = "/mnt/udisk/spectrum";
char serialNo[32];

int SCREENWIDTH;
int SCREENHEIGHT;
unsigned char FIRMWARE_VER_MAJOR;
unsigned char FIRMWARE_VER_MINOR;
QRect rcCal;

double log10Table[100000] = {0x0};

double sysErrValue;
double tempTraceData[TRACECOUNT][MAXDATASIZE];
bool sysLocked;
int lcdRefreshTimes;

QFont fontBold;
QFont fontLight;
QFont fontData;
QFont fontDataBig;
QFont fontInput;
QFont fontUnderline;
QFont fontSmall;

struct selfCheckDef selfCheck;
struct dataDef sysData;

struct mainKeyDef* sysKey;
struct mainKeyDef* nowMainKey;
struct subKeyDef* nowSubKey;

struct rfDataDef rfData;

int firVerHandle;
int keyHandle;
int ifHandle;
int rfHandle;
int comHandle;//serial port
int srcHandle;
int tmptHandle;
int fftHandle;
int usbHandle;
int gpibHandle;
int ramADF4360Handle;
int gpioHandle;
int iicHandle;
int demodHandle;
int iqHandle;
int feHandle;
int ifDev;
int ramHandle;
int tmcHandle;//power meter

bool isFFtnd;                    //ÅÐ¶ÏFFTÊÇ·ñ½áÊø

bool devControling = false;
bool iqSave;
int iqFlag;

char sockBuf[SOCKETBUF_SIZE] = {0x0};
int curSocket = 0;
int intFlag = 0;
unsigned long ifDataBuf[RAMDATASIZE];

int valuechanged;

