#include "sysvar.h"

char* idnHead = "Company";

QMutex mutexDisc(QMutex::Recursive);
unsigned char usbRcvBuf[1280];

int stLabelCalibrateProgress;
char* usbDirName = "/mnt/udisk/spectrum";
char serialNo[32];

int SCREENWIDTH;
int SCREENHEIGHT;
QRect rcCal;

double sysErrValue;
bool sysLocked;
int lcdRefreshTimes;

QFont fontBold;
QFont fontLight;
QFont fontData;
QFont fontDataSmall;
QFont fontDataBig;
QFont fontInput;
QFont fontUnderline;
QFont fontSmall;
QFont fontMarker;
QFont fontCalibration;

//PG100字体
QFont fontWin1;
QFont fontWin2;
QFont fontWin3;
QFont fontWin4;
QFont fontWin5;

struct dataDef sysData;
//struct powerOffDataDef sysPowerOffData;		//原来的
struct dataDef sysPowerOffData;

struct mainKeyDef* sysKey;
struct mainKeyDef* nowMainKey;
struct subKeyDef* nowSubKey;

bool TriggerSwitch, Channel1AnodeSwitch, Channel1CathodeSwitch, Channel2AnodeSwitch, Channel2CathodeSwitch;


int keyHandle;
int comHandle;
int usbHandle;

bool isNeedCloseDemod;
