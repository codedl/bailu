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
#include <QHostInfo>
#include <QtCore/QCryptographicHash>
#include <termios.h>
#include <math.h>
#include <qmath.h>
#include <sys/ioctl.h>
#include <stdlib.h>

#define NDEBUG
#include <assert.h>  //���Ե�ͷ�ļ�

#include "libusb.h"
#include "basescpi.h"
#include "sysstruct.h"

//QT�������
struct pointDef
{
  int xAxis;
  int yAxis;
};

//���Ʒ������صĽṹ��
struct evaluateDataDef
{
  bool isEvaluate;		//�Ƿ���������
  int year;
  int month;
  int day;
  int timesOut;
  bool evaluated;		//���ÿ�ʼ���Ƿ񳬹�����ʱ��
  int syDays;
};

//****************************************
//* SCPI��(�̳���tBaseScpi)
//****************************************
class tSysScpi : public tBaseScpi
{
  Q_OBJECT
  public:
    tSysScpi( );                                                         //����
    ~tSysScpi();                                                         //����

    QString company;
    QString devType;
    QByteArray devLogo;
    QString moduSerial;													//ģ�����к�
    QString moduDevType;												//ģ���豸����
    QString moduSoftVer;												//ģ������汾


    QFileInfoList fileList;                                              //�ļ��б�
    QString fileSelectName;                                              //ѡ���ļ�����
    QPixmap fileSelectPixmap;                                            //ѡ���ļ�ͼƬ
    bool gpibReaded;                                                     //GPIBֻ��
    QStringList gpibList;                                                //GPIBָ�
    int filesExport;                                                     //�ļ�����
    evaluateDataDef eva;                                                 //����
    bool isUpdating;                                                     //����
    bool isRcvingFile;                                                   //���ڽ����ļ�
    QString updatingFileName;                                            //�����ļ�����
    bool isRcvingSequence;												 //�������ֵ����ⲿ��������
    QString sequenceStr;
    QPixmap logo;
    bool isRcvingLogoFile;
    QString tempqstring;
    int tempint;
    QString qstr;
    char* cstr;
    char ccstr[64];
//    bool keyBoardSwitchMod,keyBoardSwitchLF,keyBoardSwitchRF;


    double setValueByRPG(double value, int nums, int location);

    void dirKeyToCursor(bool up);

    void closeDemod(void);                                                 //�رյ���
    void closeDemodState(void);
    void closeSweepDisplay();

    int sendSerialData(QString cmd);                                       //���ʹ�������
    void getFreqDisplayValue(double value);                                //��ȡƵ�� ��ʾֵ
    void getAmplDisplayValue(double value);                                //��ȡ����  ��ʾֵ

    void controlParamterDown(void);
    //    int setFrequencyOfWithRpg(int value);                                  //������ť����Ƶ��

	void keyBoardLED(void);
	void keyBoardLEDAllOpen(void);
	void initKeyBoardLED(void);
	void keyBoardLEDSwitch(char LED, char state);                          //���Ƽ��̵�

    bool isNeedCloseDemodState(void);
    int setFrequency(double value);                                        //����Ƶ��ֵ
    int setFrequency(QString value);                                       //����Ƶ��ֵ
    int setFrequencyWithRpg(int value);                                  //������ť����Ƶ��ֵ

    int setFrequencyOffset(double value);                                 //����Ƶ��ƫ��
    int setFrequencyOffset(QString value);                                //����Ƶ��ƫ��
    int setFrequencyOffsetWithRpg(int value);                             //������ť����Ƶ��ƫ��

    int setFrequencyMultipliter(double value);                            //����Ƶ�ʱ�Ƶ
    int setFrequencyMultipliter(QString value);                           //����Ƶ�ʱ�Ƶ
    int setFrequencyMultipliterWithRpg(int value);                        //������ť���ñ�Ƶ

    double getMaxAmplitude(double freqValue);										//���ݵ�ǰ��Ƶ�ʻ�ȡ���ȿ����õ����ֵ
    int setAmplitude(double value);                                        //���÷���ֵ
    int setAmplitude(QString value);                                       //���÷���ֵ
    int setAmplitudeWithRpg(int value);                                    //������ť���÷���ֵ

    int setAmplitudeOffset(double value);                                 //���÷���ƫ��
    int setAmplitudeOffset(QString value);                                //���÷���ƫ��
    int setAmplitudeOffsetWithRpg(int value);                             //������ť���÷���ƫ��

    int setAmplitudeError(double value);                                 //���÷��Ȳ���
    int setAmplitudeError(QString value);                                //���÷��Ȳ���
    int setAmplitudeErrorWithRpg(int value);                             //������ť���÷��Ȳ���

    int setFrequencyOfReference(void);                                    //����Ƶ�ʲο�
    int setFrequencyOfReferenceState(int value);                          //����Ƶ�ʲο�״̬
    int setFrequencyOfReferenceState(QString value);                       //����Ƶ�ʲο�״̬

    int setALCState(int value);                                            //�����ȷ�״̬
    int setALCState(QString value);                                        //�����ȷ�״̬

    int setAmplResponse(int value);                                         //�����ȷ���Ӧ
    int setAmplResponse(QString value);                                     //�����ȷ���Ӧ

    int setAmplitudeOfReference(void);                                    //���÷��Ȳο�
    int setAmplitudeOfReferenceState(int value);                          //���÷��Ȳο�״̬
    int setAmplitudeOfReferenceState(QString value);                       //���÷��Ȳο�״̬

    int setRef10MState(QString value);
    int setRef10MState(int value);
    
    int setExtRefFrequency(double value);                                //���ñ�Ƶ�ο�Ƶ��
    int setExtRefFrequency(QString value);                                //���ñ�Ƶ�ο�Ƶ��
    int setExtRefFrequencyWithRpg(int value);                        //��ť���ñ�Ƶ�ο�Ƶ��

    int setExtRefType(int value);                                    //���ñ�Ƶ�ο�����
    int setExtRefType(QString value);                                   //���ñ�Ƶ�ο�����

    int setLFState(int value);
    int setLFState(QString value);
    int setLfType(int value);                                       //����LF����
    int setLfType(QString value);                                   //����LF����
    int setLfFreq(double value);                                    //����LFƵ��
    int setLfFreq(QString value);                                   //����LFƵ��
    int setLfFreqWithRpg(int value);                                    //����LFƵ��

    int setLfAmpl(double value);                                    //����LF ����
    int setLfAmpl(QString value);                                   //����LF ����
    int setLfAmplWithRpg(int value);                                   //����LF ����

    int setDrgState(int value);
    int setDrgState(QString value);
    int setDrgUplimit(double value);								//����DRG����Ƶ��
    int setDrgUplimit(QString value);								//����DRG����Ƶ��
    int setDrgUplimitWithRpg(int value);							//����DRG����Ƶ��

    int setDrgDownlimit(double value);
    int setDrgDownlimit(QString value);
    int setDrgDownlimitWithRpg(int value);

    int setDrgPoints(double value);
    int setDrgPoints(QString value);
    int setDrgPointsWithRpg(int value);

    int setDrgRate(double value);
    int setDrgRate(QString value);
    int setDrgRateWithRpg(int value);

    int setDrgWorkmode(int value);
    int setDrgWorkmode(QString value);

    int setScpiState(int value);                                    //����SCPI
    int setScpiState(QString value);                                //����SCPI


//	int setSweepstate(int value);

    int setSweepMode(int value);                                    //����ɨ��ģʽ
    int setSweepMode(QString value);                                //����ɨ��ģʽ


    int setSweepType(int value);                                    //����ɨ������
    int setSweepType(QString value);                                    //����ɨ������

    int setSweepRepeat(int value);                                  //����ɨ��Repeatģʽ
    int setSweepRepeat(QString value);                              //����ɨ��Repeatģʽ

    int setSweepDirection(int value);                               //����ɨ�跽��
    int setSweepDirection(QString value);                           //����ɨ�跽��

    int setSweepRetrace(int value);                                 //����ɨ��Retrace
    int setSweepRetrace(QString value);                             //����ɨ��Retrace

	int setSweepTrig(int value);                                 //����ɨ�败��
	int setSweepTrig(QString value);                                 //����ɨ�败��

    int setStepStartFrequency(double value);                        //������ʼƵ��ֵ
    int setStepStartFrequency(QString value);                       //������ʼƵ��ֵ
    int setStepStartFrequencyWithRpg(int value);                    //������ť������ʼƵ��ֵ

    int setStepStopFrequency(double value);                         //������ֹƵ��ֵ
    int setStepStopFrequency(QString value);                        //������ֹƵ��ֵ
    int setStepStopFrequencyWithRpg(int value);                     //������ť������ֹƵ��ֵ

    int setStepStartAmplitude(double value);                        //������ʼ����ֵ
    int setStepStartAmplitude(QString value);                       //������ʼ����ֵ
    int setStepStartAmplitudeWithRpg(int value);                    //������ť������ʼ����ֵ

    int setStepStopAmplitude(double value);                         //������ֹ����ֵ
    int setStepStopAmplitude(QString value);                        //������ֹ����ֵ
    int setStepStopAmplitudeWithRpg(int value);                     //������ť������ֹ����ֵ

    int setStepPoints(double value);                                //���õ���
    int setStepPoints(QString value);                               //���õ���
    int setStepPointsWithRpg(int value);                                   //������ť���õ���

    int setStepDwell(double value);                                        //����פ��ʱ��
    int setStepDwell(QString value);                                       //����פ��ʱ��
    int setStepDwellWithRpg(int value);                                    //������ť����פ��ʱ��

    int setSweepListDownload();

    //ģ�����Դ
    int setAnalogModulateSource(int value);
    int setAnalogModulateSource(QString value);
    //AM
    int setAmState(int value);
    int setAmState(QString value);
    int setAmDepth(double value);
    int setAmDepth(QString value);
    int setAmDepthWithRpg(int value);
    int setAmRate(double value);                                //����
    int setAmRate(QString value);                               //����
    int setAmRateWithRpg(int value);                            //������ť����

    //FM
    int setFmState(int value);
    int setFmState(QString value);
    int setFmDev(double value);
    int setFmDev(QString value);
    int setFmDevWithRpg(int value);
    int setFmRate(double value);                                //����
    int setFmRate(QString value);                               //����
    int setFmRateWithRpg(int value);                            //������ť����

    //PM
    int setPmState(int value);
    int setPmState(QString value);
    int setPmPhase(double value);
    int setPmPhase(QString value);
    int setPmPhaseWithRpg(int value);
    int setPmRate(double value);                                //����
    int setPmRate(QString value);                               //����
    int setPmRateWithRpg(int value);                            //������ť����

	//���ֵ���
	int setDigitalModuateState(int value);                    //�������ֵ���״̬
	int setDigitalModuateState(QString value);                //�������ֵ���״̬
	int setDigitalModulateSource(int value);				  //�������ֵ���Դ
	int setDigitalModulateSource(QString value);			  //�������ֵ���Դ
	int setDigitalModuateType(int value);                    //��������
	int setDigitalModuateType(QString value);                //��������
	int setDigitalModulatePara(double value);                    //�������ֵ��Ʋ�������������������
	int setDigitalModulatePara(QString value);                //�������ֵ��Ʋ�������������������
	int setDigitalModulateParaWithRpg(int value);          //ʹ��RPG���õ��Ʋ���
	int setDigitalModuateRate(double value);                  //���õ�������
	int setDigitalModuateRate(QString value);                 //���õ�������
	int setDigitalModuateRateWithRpg(int value);           //ʹ��RPG���õ�������
	int setDigtalSequenceDownload(void);					//�·����ֵ����ⲿ����Դ��������
	
#if 0
    //ASK
    int setAskState(int value);
    int setAskState(QString value);
    int setAskMode(int value);
    int setAskMode(QString value);
    int setAskRate(double value);                                //����
    int setAskRate(QString value);                               //����
    int setAskRateWithRpg(int value);                            //������ť����

    //FSK
    int setFskState(int value);
    int setFskState(QString value);
    int setFskMode(int value);
    int setFskMode(QString value);
    int setFskRate(double value);                                //����
    int setFskRate(QString value);                               //����
    int setFskRateWithRpg(int value);                            //������ť����

    int setFskStep(double value);                                //����
    int setFskStep(QString value);                               //����
    int setFskStepWithRpg(int value);                            //������ť����

    //PSK
    int setPskState(int value);
    int setPskState(QString value);
    int setPskMode(int value);
    int setPskMode(QString value);
    int setPskRate(double value);                                //����
    int setPskRate(QString value);                               //����
    int setPskRateWithRpg(int value);                            //������ť����
#endif
    //Pluse
    int setPluseState(int value);
    int setPluseState(QString value);
    int setPluseSource(int value);
    int setPluseSource(QString value);
	int setPluseMethod(int value);
	int setPluseMethod(QString value);
    int setPlusePeriod(double value);
    int setPlusePeriod(QString value);
    int setPlusePeriodWithRpg(int value);
    int setPluseWidth(double value);                                //����
    int setPluseWidth(QString value);                               //����
    int setPluseWidthWithRpg(int value);                            //������ť����

    int saveScreenPixmapToLocal(QString value);                          //������Ļ��ͼ������
    int saveScreenPixmapToUsb(QString value);                            //������Ļ��ͼ������
    int saveTraceDataToLocal(QString value);                             //���漣������������
    int saveTraceDataToUsb(QString value);                               //���漣������������

//    int loadUserState(void);                                             //�����û�����
//    int saveUserState(void);                                             //�����û�����
    int loadPowerOffState(void);                                         //����ػ�����
    int savePowerOffState(void);                                         //����ػ�����

    int setLanIP(QString value);                                         //����IP��ַ
    int setLanMask(QString value);                                       //�������������ַ
    int setLanGate(QString value);                                       //�������ص�ַ

    QStringList getLanRealIPAndAddress(void);                            //��ȡLAN�����ַ
    int resetSystemIOOfLan(void);                                        //����LAN
    int resetSystemIOOfSerial(void);                                     //���ô��п�


    int setSystemPoweronParam(int value);                                //���ÿ�������
    int setSystemPoweronParam(QString value);                            //���ÿ�������
    int setSystemPresetParam(int value);                                 //���ø�λ����
    int setSystemPresetParam(QString value);                             //���ø�λ����
    int setSystemLanguage(int value);                                    //����ϵͳ����
    int setSystemLanguage(QString value);                                //����ϵͳ����
    int setSystemDateTimeShow(int value);                                //����ϵͳ����ʱ����ʾ
    int setSystemDateTimeShow(QString value);                            //����ϵͳ����ʱ����ʾ
    int setSystemDateFormat(int value);                                  //����ϵͳ���ڸ�ʽ
    int setSystemDateFormat(QString value);                              //����ϵͳ���ڸ�ʽ
    int setSystemDate(QString value);                                    //����ϵͳ����
    int setSystemTime(QString value);                                    //����ϵͳʱ��
    int setSystemScpiScreenRefresh(int value);                           //����SCPI��Ļˢ��
    int setSystemScpiScreenRefresh(QString value);                       //����SCPI��Ļˢ��
    int setSystemServiceOfSerialNo(QString value);                       //�������к�
    int setSystemServiceOfLanMacAddress(QString value);                  //�������������ַ
    int setSystemServiceOfLanMacAddressWidthArrow(bool up);              //������������������ַ
    int setSystemServiceOfLanMacAddressWidthRpg(int value);              //������������������ַ

    int setSystemServiceDeviceOn(QString value);						 //����ϵͳ�豸����
    int setSystemServiceDeviceOff(QString value);						 //����ϵͳ�豸�ر�
    int setSystemServiceOfEvalute(int value);                            //�������ð汾
    int getSystemServiceOfEvaluteSYDays(void);
    QString getSystemServiceOfMD5(QString value, bool isServer);
    int unLockSystem(void);												 //����ϵͳ

    int getFileOfNowSelectIndex(void);                                   //��ȡ��ǰ�ļ�ѡ������
    int setFileOfDeleteTheSelect(void);                                  //ɾ��ָ���ļ�
    int setFileOfDeleteThePage(void);                                    //ɾ����ҳ�ļ�
    int setFileOfDeleteAll(void);                                        //ɾ�������ļ�
    int setFileOfExportTheSelect(void);                                  //����ѡ���ļ�
    int setFileOfExportThePage(void);                                    //������ҳ�ļ�
    int setFileOfExportAll(void);                                        //����ȫ���ļ�

    void getSystemFileList(void);                                        //��ȡ�ļ��б�

    int execPrintTrace(void);                                            //��ӡ����
    int execPrintScreen(void);                                           //��ӡ��Ļ

    void getSystemDateTime(void);                                        //��ȡϵͳ����ʱ��
    void getSystemFont(void);                                            //��ȡϵͳ����
    void presetSystemData(void);                                         //��λϵͳ����
    int presetSystem(void);                                              //��λϵͳ

    QString getMarkerFunctionNoiseStartFreq(void);
    QStringList loadLogOfFactoryCalibrate(void);                         //�������У׼��־
    void reconnectSerial(void);                                          //�������Ӵ���

    void showRunMessage(QString title);
    void showRunAlarm(QString title);
    void drawWin4(QString title);

    void closeScpiPortOfAll(void);                                       //�ر�����ͨ�Ŷ˿�
    void closeScpiPortOfLan(void);                                       //�ر�LAN�˿�ͨ��
    void closeScpiPortOfUsb(void);                                       //�ر�USB�˿�ͨ��
    void closeScpiPortOfGpib(void);                                      //�ر�GPIB�˿�ͨ��
    void closeScpiPortOfSerial(void);                                    //�ر�COM�˿�ͨ��

    bool isStartToDrawStepProcess(void);                                 //�ж��Ƿ�ʼ���Ʋ���process
    bool isStartToDrawListProcess(void);                                 //�ж��Ƿ�ʼ�����б�process


    void saveQvalueFromModule(QString tempValue);
    QString getQvalueFromModule(void);

    void analysisDigiSequencedataFormLan(void);

  protected:
    int usbDiscFailNums;
    bool usbDiscriminatorInitedSuccess;                                  //Usb����״̬
    libusb_device **devs;                                                //Usb�����豸
    libusb_device_handle *dev_handle;                                    //Usb�豸���
    libusb_context *ctx;                                                 //Usb��Ϣ
    unsigned char usbhostbuf[1024];                                      //USB������

    QTcpServer* serverSocket;                                            //�����SOCKET
    QTcpSocket* clientSocket;                                            //�ͻ���SOCKET
    QByteArray readBuf;                                                  //��������
    QByteArray writeBuf;                                                 //д������

    bool openServerSocket(void);                                         //�򿪷�����SOCKET
    bool closeServerSocket(void);                                        //�رշ�����SOCKET

    void handleScpiCommand(QString cmdStr);                              //����SCPIָ��
    void returnScpiResult(QString value);                                //����SCPI���
    int returnScpiResult(QByteArray value);                              //����SCPI���


//    int sendSerialData(QString cmd);                                     //����Serial����
    int setupSerial(void);                                               //���ô���

    void loadDeviceConfig(void);                                         //��ȡ�豸����
    void saveDeviceConfig(void);                                         //�����豸����

    int printPixmap(QPixmap pix, unsigned char pageSize,
    		                     unsigned char orientation,
    		                     unsigned char copyNums,
    		                     bool colour);                           //��ӡͼƬ

    bool updateRecordOfClear(void);
    bool updateRecordOfAdd(QString value);
    bool updateSystem(void);
    QString updateFileName(QString value);
   private:
    QString versionString;
    QString returnEofString;
    QString scpiBuffer;                                                  //SCPIջ

    QString rcvFormModuleBuffer;		//�ײ㵥Ƭ����������ָ��ֵbuffer  zpc add

//    void checkMultiDisplay(multiDispType value);

    void loadDeviceCompany(void);
    void saveDeviceCompany(QString value);

    void loadDeviceType(void);
    void saveDeviceType(QString value);
    void loadDiveceLogo(void);
    void saveDeviceLogo(QString value);

    void saveSystemSetting(QString section, QString name, QVariant value);

    void scpiFloatToLen8String(double value, char* result);              //��ֵת��
    void asyncDelay(unsigned long value);                                //�첽��ʱ
    int exitCalibrate(void);                                             //�˳�У׼


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
    QString boolToString(bool value);                                    //��ȡ�����ַ���
    QString doubleToString(double value);
    int getBoolValue(QString value, bool* ok);                           //��ȡ����ֵ
    unsigned char getCharValue(QString value, bool* ok);                 //��ȡС����
    double getFreqValue(QString value, bool* ok);                        //��ȡƵ��ֵ
    double getAmptValue(QString value, bool* ok);                        //��ȡ����ֵ
    double getdBValue(QString value, bool* ok);                          //��ȡdBֵ
    double getTimeValue(QString value, bool* ok);                        //��ȡʱ��ֵ
    double getVoltageValue(QString value, bool* ok);					 //��ȡ��ѹֵ
    int getIntValue(QString value, bool* ok);                            //��ȡ����ֵ
    double getDoubleValue(QString value, bool *ok);                      //��ȡ������ֵ

    void checkLocalDirectory(void);                                      //��Ȿ��·��
    int checkUsbDirectory(void);                                         //���USB·��
    int connectToUsbDisk(void);                                          //��������
    int disConnectFromUsbDisk(void);                                     //�Ͽ���������
    int getDirFileCount(QString dirName);                                //��ȡ�ļ���Ŀ

    bool isAllMarkerOff(void);                                           //����Ƶ��ر�
    bool isTheMarkerOff(int index);                                      //ָ��Ƶ��ر�

    int isValidDate(QString value);                                      //���ںϷ�
    int isValidTime(QString value);                                      //ʱ��Ϸ�
    int setNewDate(QString value);                                      //����������
    int setNewTime(QString value);                                     //������ʱ��

    QString getTheTranslatorString(QString enStr);                       //��ȡ�ַ���
    void saveScreen(void);                                               //������Ļ
    void outputMessage(QString title);
    void outputToScreen(QString title);                                  //�������Ļ
    void outputToScreen(QString title, QString value);                   //�������Ļ
    void outputToScreen(QString title, int progress, int normal);        //�������Ļ

    double dBmTomV(double value);
	double getdBmValue(double value);                                    //���ȵ�������λת��ΪdBm
	double getdBuVmVValue(double value);								//dBmֵת��������ֵ

  private slots:
    virtual void readSerialData();                                       //����������
    virtual void readUsbSlaveData(void);                                 //��USB�ӿ�����

    virtual void acceptClientSocketConnect(void);                        //���ܿͻ��˽�������
    virtual void clientSocketDisConnect(void);                           //�ͻ��˶Ͽ�����
    virtual void clientSocketReadyRead(void);                            //�ͻ��������ݵ���
    virtual void clientSocketError(QAbstractSocket::SocketError);        //�ͻ��˷�������
  signals:
    virtual void showAlarmMessage(QString title, QString msg);
    virtual void refreshScpiState(QString helpStr);                      //ˢ��SCPI״̬
    virtual void refreshWindowState(void);                               //ˢ�´���״̬
    virtual void resetUsbSlaveInterface(void);                           //��λUSB�ӿ�

    virtual void drawProcess(void);                                           //���� process
    virtual void drawsListData(void);                                         //�����б�ɨ��༭
    virtual void drawProgBar(bool Display, float RatioProgBar);               //���ƽ�����
    virtual void drawDownloadProg(bool Display, int current, int amount);     //���Ƽ��ؽ���
    virtual void drawAnalogModulate(void);                                    //����ģ�����
    virtual void drawDigitalModulate(void);                                   //�������ֵ���
	virtual void drawPluseModulate(void);                                    //�����������
	virtual void drawSystemPara(void);                                        //����ϵͳ����
    virtual void drawArbitraryWave(void);                                     //�������Ⲩ
//    virtual void drawFreqAndAmpl(void);                                       //����Ƶ�ʷ���
	virtual void drawFreqInParamter(void);
	virtual void drawAmplInParamter(void);
    virtual void drawExfFref(void);                                           //���Ʋο�
    virtual void drawDrgFunction();											  //����DRG
    virtual void drawSweep(int type);                                         //����ɨ��

    virtual void drawFreqAndAmplMain(void);		                              //������������ʾ��
    virtual void drawState(void);	   	                                  //���Ʊ���
    virtual void drawInputParamterNoParamter(void);	                              //������������ʾ��
    virtual void drawInputParamter(char* str, int funcode);                   //������������ʾ��
//    virtual void drawHelp(void);                                              //���ư�������ʾ��
    virtual void drawButton(void);                                            //���ư�ť��ʾ��
//    virtual void drawAll(void);                                               //����ȫ��

};

#endif
