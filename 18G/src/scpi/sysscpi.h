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
//* SCPI��(�̳���tBaseScpi)
//****************************************
class tSysScpi : public tBaseScpi
{
  Q_OBJECT
  public:
    tSysScpi();                                                          //����
    ~tSysScpi();                                                         //����

    QTcpSocket* cSocket;                                                 //У׼�����Ŀͻ���

    QString devType;
    QByteArray devLogo;

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

    QPixmap logo;
    bool isRcvingLogoFile;

    QString traceDataString[TRACECOUNT];
    bool traceDataOrganized[TRACECOUNT];//trace data stauts of preparing
    
    int setFrequencyOfCenter(double value);                              //��������Ƶ��
    int setFrequencyOfCenter(QString value);                             //��������Ƶ��
    int setFrequencyOfCenterWithArrow(bool up);                          //�������������Ƶ��
    int setFrequencyOfCenterWithRpg(int value);                          //������ť��������Ƶ��
    int setFrequencyOfStart(double value);                               //������ʼƵ��
    int setFrequencyOfStart(QString value);                              //������ʼƵ��
    int setFrequencyOfStartWithArrow(bool up);                           //�����������ʼƵ��
    int setFrequencyOfStartWithRpg(int value);                           //������ť������ʼƵ��
    int setFrequencyOfStop(double value);                                //������ֹƵ��
    int setFrequencyOfStop(QString value);                               //������ֹƵ��
    int setFrequencyOfStopWithArrow(bool up);                            //�����������ֹƵ��
    int setFrequencyOfStopWithRpg(int value);                            //������ť������ֹƵ��
    int setFrequencyOfCenterStep(double value);                          //��������Ƶ�ʲ���
    int setFrequencyOfCenterStep(QString value);                         //��������Ƶ�ʲ���
    int setFrequencyOfCenterStepWithArrow(bool up);                      //�������������Ƶ�ʲ���
    int setFrequencyOfCenterStepWithRpg(int value);                      //������ť��������Ƶ�ʲ���
    int setFrequencyOfCenterStepAuto(int value);                         //��������Ƶ�ʲ����Զ�
    int setFrequencyOfCenterStepAuto(QString value);                     //��������Ƶ�ʲ����Զ�
    int setFrequencyOffset(double value);                                //����Ƶ��ƫ��
    int setFrequencyOffset(QString value);                               //����Ƶ��ƫ��
    int setFrequencyOfOffsetWithArrow(bool up);                          //�������������Ƶ��
    int setFrequencyOfOffsetWithRpg(int value);                          //������ť��������Ƶ��
    int setFrequencyOfReference(int value);                              //����Ƶ�ʲο�
    int setFrequencyOfSpan(double value);                                //����ɨ��
    int setFrequencyOfSpan(QString value);                               //����ɨ��
    int setFrequencyOfSpanWithArrow(bool up);                            //���������ɨ��
    int setFrequencyOfSpanWithRpg(int value);                            //������ť����ɨ��
    int setFrequencyOfLowChannel(int value);                             //���õ�Ƶͨ��
    int setFrequencyOfScaleType(int value);															 //����Ƶ�ʿ̶�����
    int setFrequencyOfScaleType(QString value);													 //����Ƶ�ʿ̶�����

    int setAmptOfRefLevel(double value);                                 //���òο���ƽ
    int setAmptOfRefLevel(QString value);                                //���òο���ƽ
    int setAmptOfRefLevelWithArrow(bool up);                             //��������òο���ƽ
    int setAmptOfRefLevelWithRpg(int value);                             //������ť���òο���ƽ
    int setAmptOfAttAuto(int value);                                     //����˥���Զ�
    int setAmptOfAttAuto(QString value);                                 //����˥���Զ�
    int setPreamptOfAttAuto(int value);                                  //����ǰ�÷Ŵ���
    int setPreamptOfAttAuto(QString value);                              //����ǰ�÷Ŵ���
    int setAmptOfAtt(double value);                                      //����˥��
    int setAmptOfAtt(QString value);                                     //����˥��
    int setAmptOfAttWithArrow(bool up);                                  //���������˥��
    int setAmptOfAttWithRpg(int value);                                  //������ť����˥��
    int setLMP_CValue(double value);                                     //�����迹�任
    int setAmptOfLMP_CWithArrow(bool up);                                //����������迹�任
    int setAmptOfLMP_CWithRpg(int value);                                //������ť�����迹�任
    int setAmptOfScaleDiv(double value);                                 //���÷��ȿ̶�
    int setAmptOfScaleDiv(QString value);                                //���÷��ȿ̶�
    int setAmptOfScaleDivWithArrow(bool up);                             //��������÷��ȿ̶�
    int setAmptOfScaleDivWithRpg(int value);                             //������ť���÷��ȿ̶�
    int setAmptOfRefOffset(double value);                                //���òο�ƫ��
    int setAmptOfRefOffset(QString value);                               //���òο�ƫ��
    int setAmptOfRefOffsetWithArrow(bool up);                            //��������òο�ƫ��
    int setAmptOfRefOffsetWithRpg(int value);                            //������ť���òο�ƫ��
    int setAmptOfScaleType(int value);                                   //���ÿ̶�����
    int setAmptOfScaleType(QString value);                               //���ÿ̶�����
    int setAmptOfRefUnit(int value);                                     //���òο���λ
    int setAmptOfRefUnit(QString value);                                 //���òο���λ

    int setBwOfRbwAuto(int value);                                       //���÷ֱ����Զ�
    int setBwOfRbwAuto(QString value);                                   //���÷ֱ����Զ�
    int setBwOfRbw(double value);                                        //���÷ֱ��ʴ���
    int setBwOfRbw(QString value);                                       //���÷ֱ��ʴ���
    int setBwOfRbwWithArrow(bool up);                                    //��������÷ֱ��ʴ���
    int setBwOfRbwWithRpg(int value);                                    //������ť���÷ֱ��ʴ���
    int setBwOfRbwStepMode(int value);                                   //���÷ֱ沽��ģʽ
    int setBwOfRbwStepMode(QString value);                               //���÷ֱ沽��ģʽ
    int setBwOfVbwAuto(int value);                                       //������Ƶ�����Զ�
    int setBwOfVbwAuto(QString value);                                   //������Ƶ�����Զ�
    int setBwOfVbw(double value);                                        //������Ƶ����
    int setBwOfVbw(QString value);                                       //������Ƶ����
    int setBwOfVbwWithArrow(bool up);                                    //�����������Ƶ����
    int setBwOfVbwWithRpg(int value);                                    //������ť������Ƶ����

    unsigned long getVbwFactor(double value);                            //��ȡVBW��ϵ��

    int setBwOfAverage(double value);                                    //���ü���ƽ��
    int setBwOfAverage(QString value);                                   //���ü���ƽ��
    int setBwOfAverageWithArrow(bool up);                                //��������ü���ƽ��
    int setBwOfAverageWithRpg(int value);                                //������ť���ü���ƽ��
    int setBwOfAverageSwitch(int value);                                 //���ü���ƽ��״̬
    int setBwOfAverageSwitch(QString value);                             //���ü���ƽ��״̬
    int resetBwOfAverageState(void);                                     //��λ����ƽ��״̬
    int setBwOfEmcState(int value);                                      //����EMC�������
    int setBwOfEmcState(QString value);                                  //����EMC�������
    int resetMarkerFctnOfFreqCount(void);                                //��λƵ�ʼ���

    int setDisplayOfFullScreen(int value);                               //������ʾȫ��
    int setDisplayOfFullScreen(QString value);                           //������ʾȫ��
    int setDisplayOfZoomFrequency(int value);                            //������ʾ��������Ƶ��
    int setDisplayOfZoomOn(int value);                                   //������ʾ��������
    int setDisplayOfZoomOn(QString value);                               //������ʾ��������
    int setDisplayOfLineValue(QString value);                            //������ʾ��
    int setDisplayOfLineValue(double value);                             //������ʾ��
    int setDisplayOfLineValueWithArrow(bool up);                         //�����������ʾ��
    int setDisplayOfLineValueWithRpg(int value);                         //������ť������ʾ��
    int setDisplayOfLine(int value);                                     //������ʾ��
    int setDisplayOfLine(QString value);                                 //������ʾ��
    int setDisplayOfRefLevelGraticule(int value);                        //������ʾ�ο����
    int setDisplayOfRefLevelGraticule(QString value);                    //������ʾ�ο����
    int setDisplayOfGrid(int value);                                     //������ʾ����
    int setDisplayOfGrid(QString value);                                 //������ʾ����
    int setDisplayOfLabel(int value);                                    //������ʾ��ǩ
    int setDisplayOfLabel(QString value);                                //������ʾ��ǩ
    int setDisplayOfStyle(int value);                                    //������ʾ���
    int setDisplayOfStyle(QString value);                                //������ʾ���

    int setDisplayOfFreqLineStart(double value);                         //����Ƶ������ʼ
    int setDisplayOfFreqLineStart(QString value);                        //����Ƶ������ʼ
    int setDisplayOfFreqLineStartWithArrow(bool up);                     //���������Ƶ������ʼ
    int setDisplayOfFreqLineStartWithRpg(int value);                     //������ť����Ƶ������ʼ
    int setDisplayOfFreqLineStop(double value);                          //����Ƶ������ֹ
    int setDisplayOfFreqLineStop(QString value);                         //����Ƶ������ֹ
    int setDisplayOfFreqLineStopWithArrow(bool up);                      //���������Ƶ������ֹ
    int setDisplayOfFreqLineStopWithRpg(int value);                      //������ť����Ƶ������ֹ
    int setDisplayOfFreqLine(int value);                                 //����Ƶ����
    int setDisplayOfFreqLine(QString value);                             //����Ƶ����

    int clearAllUserSelfState(void);                                     //��������û�״̬
    int clearUserSelfState(int index);                                   //����û�״̬
    int loadUserSelfState(int index);                                    //�����û�״̬
    int saveUserSelfState(int index);                                    //�����û�״̬
    int setSweepOfTimeAuto(int value);                                   //����ɨ��ʱ���Զ�
    int setSweepOfTimeAuto(QString value);                               //����ɨ��ʱ���Զ�
    int setSweepOfTime(double value);                                    //����ɨ��ʱ��
    int setSweepOfTime(QString value);                                   //����ɨ��ʱ��
    int setSweepOfTimeWithArrow(bool up);                                //���������ɨ��ʱ��
    int setSweepOfTimeWithRpg(int value);                                //������ť����ɨ��ʱ��
    int setSweepOfMode(int value);                                       //����ɨ�跽ʽ
    int setSweepOfMode(QString value);                                   //����ɨ�跽ʽ
    int setSweepOfPoints(double value);                                  //����ɨ�����
    int setSweepOfPoints(QString value);                                 //����ɨ�����
    int setSweepOfPointsWithArrow(bool up);                              //���������ɨ�����
    int setSweepOfPointsWithRpg(int value);                              //������ť����ɨ�����

    int setAutoTune(void);                                               //�Զ���г

    int setTraceMode(int traceIndex, int value);                         //���ü���ģʽ
    int setTraceMode(int traceIndex, QString value);                     //���ü���ģʽ
    int setTraceOperationsOfExchange(int trace1Index, int trace2Index);  //���ü��߻���
    int setTraceOperationsOfChange(int trace1Index, int trace2Index);    //���� ����ת��
    int setTraceOperationsOfDisplayLine(int traceIndex);                 //���� ������ʾ��

    int setDetector(int value);                                          //���ü첨
    int setDetector(QString value);                                      //���ü첨

    int setTrigMode(int value);                                          //���ô���ģʽ
    int setTrigMode(QString value);                                      //���ô���ģʽ
    int setVideoTrigPower(double value);                                 //������Ƶ��������
    int setVideoTrigPower(QString value);                                //������Ƶ��������
    int setVideoTrigPowerWithArrow(bool up);                             //�����������Ƶ��������
    int setVideoTrigPowerWithRpg(int value);                             //������ť������Ƶ��������
    int calculateVideoTrigPowerDown(double value);                       //���㴥�������·�ֵ

    int isMeasureOn(void);                                               //�����Ѵ�
    int isMeasureAllOff(void);                                           //���в����ѹر�
    int setMeasureAllOff(void);                                          //�ر����в���
    int setMeasureOfTimeSpectrumOn(int value);                           //����ʱ��Ƶ�ײ���
    int setMeasureOfAcprOn(int value);                                   //�����ڵ����ʲ���
    int setMeasureOfChannelPowerOn(int value);                           //�����ŵ����ʲ���
    int setMeasureOfObwOn(int value);                                    //����ռ�ô������
    int setMeasureOfDiscState(int value);                                //���ü�Ƶ״̬
    int setMeasureOfDiscInputGain(int value);                            //���ü�Ƶ��������
    int setMeasureOfDiscOutputPower(double value);                       //���ü�Ƶ�������
    int setMeasureOfDiscOutputPowerWithArrow(bool up);                   //��������ü�Ƶ�������
    int setMeasureOfDiscOutputPowerWithRpg(int value);                   //������ť���ü�Ƶ�������
    int setMeasureOfDiscScaleDiv(double value);                          //���ü�Ƶ�̶�
    int setMeasureOfDiscScaleDivWithArrow(bool up);                      //��������ü�Ƶ�̶�
    int setMeasureOfDiscScaleDivWithRpg(int value);                      //������ť���ü�Ƶ�̶�
    int setMeasureOfDiscSweepTime(double value);                         //���ü�Ƶɨ��ʱ��
    int setMeasureOfDiscSweepTimeWithArrow(bool up);                     //��������ü�Ƶɨ��ʱ��
    int setMeasureOfDiscSweepTimeWithRpg(int value);                     //������ť���ü�Ƶɨ��ʱ��
    int setMeasureOfDiscDCOffset(double value);                          //���ü�Ƶֱ��ƫ��
    int setMeasureOfDiscDCOffsetWithArrow(bool up);                      //���������ֱ��ƫ��
    int setMeasureOfDiscDCOffsetWithRpg(int value);                      //������ť����ֱ��ƫ��
    int setMeasureOfDiscNormalize(void);                                 //��Ƶ���궨

    int setMeasSetupOfChannelWidth(double value);                        //���ò��������ŵ�����
    int setMeasSetupOfChannelWidth(QString value);                       //���ò��������ŵ�����
    int setMeasSetupOfChannelWidthWithArrow(bool up);                    //��������ò��������ŵ�����
    int setMeasSetupOfChannelWidthWithRpg(int value);                    //������ť���ò��������ŵ�����
    int setMeasSetupOfChannelSpace(double value);                        //���ò��������ŵ����
    int setMeasSetupOfChannelSpace(QString value);                       //���ò��������ŵ����
    int setMeasSetupOfChannelSpaceWithArrow(bool up);                    //��������ò��������ŵ����
    int setMeasSetupOfChannelSpaceWithRpg(int value);                    //������ť���ò��������ŵ����
    int setMeasSetupOfChannelNums(double value);                         //���ò��������ڵ���Ŀ
    int setMeasSetupOfChannelNums(QString value);                        //���ò��������ڵ���Ŀ
    int setMeasSetupOfChannelNumsWithArrow(bool up);                     //��������ò��������ڵ���Ŀ
    int setMeasSetupOfChannelNumsWithRpg(int value);                     //������ť���ò��������ڵ���Ŀ
    int setMeasSetupOfPowerPercent(double value);                        //���ò�������ռ�ô���
    int setMeasSetupOfPowerPercent(QString value);                       //���ò�������ռ�ô���
    int setMeasSetupOfPowerPercentWithArrow(bool up);                    //��������ò�������ռ�ô���
    int setMeasSetupOfPowerPercentWithRpg(int value);                    //������ť���ò�������ռ�ô���
    int setMeasSetupOfChannelPowerOfSpan(QString value);                 //�����ŵ��������ɨ��
    int setMeasSetupOfPowerPercentOfSpan(QString value);                 //����ռ�ô������ɨ��

    int saveScreenPixmapToLocal(QString value);                          //������Ļ��ͼ������
    int saveScreenPixmapToUsb(QString value);                            //������Ļ��ͼ������
	int saveTraceData(QString dirName,QString absoluteFileName);				//���漣������
    int saveTraceDataToLocal(QString value);                             //���漣������������
    int saveTraceDataToUsb(QString value);                               //���漣������������

    int loadUserState(void);                                             //�����û�����
    int saveUserState(void);                                             //�����û�����
    int loadPowerOffState(void);                                         //����ػ�����
    int savePowerOffState(void);                                         //����ػ�����

    int setPrintOfColor(int value);                                      //���ô�ӡ������
    int setPrintOfColor(QString value);                                  //���ô�ӡ������
    int setPrintOfOrientation(int value);                                //���ô�ӡ����
    int setPrintOfOrientation(QString value);                            //���ô�ӡ����
    int setPrintOfPaperSize(int value);                                  //���ô�ӡֽ�Ŵ�С
    int setPrintOfPaperSize(QString value);                              //���ô�ӡֽ�Ŵ�С
    int setPrintOfNums(double value);                                    //����ɨ�����
    int setPrintOfNums(QString value);                                   //����ɨ�����
    int setPrintOfNumsWithArrow(bool up);                                //���������ɨ�����
    int setPrintOfNumsWithRpg(int value);                                //������ť����ɨ�����

    int setLanIP(QString value);                                         //����IP��ַ
    int setLanMask(QString value);                                       //�������������ַ
    int setLanGate(QString value);                                       //�������ص�ַ

    int setGpibAddr(int value);                                          //����GPIB��ַ
    int setGpibAddr(QString value);                                      //����GPIB��ַ
    int setGpibAddrWithArrow(bool up);                                   //���������GPIB��ַ
    int setGpibAddrWithRpg(int value);                                   //������ť����GPIB��ַ

    int setSystemSerialOfBaudrate(int value);                            //���ô��ڲ�����
    int setSystemSerialOfDatabits(int value);                            //���ô�������λ
    int setSystemSerialOfStopbits(int value);                            //���ô���ֹͣλ
    int setSystemSerialOfCheck(int value);                               //���ô���У��
    int setSystemSerialOfFlowControl(int value);                         //���ô���������

    QStringList getLanRealIPAndAddress(void);                            //��ȡLAN�����ַ
    int resetSystemIOOfLan(void);                                        //����LAN
    int resetSystemIOOfSerial(void);                                     //���ô��п�

    int setSystemPoweronParam(int value);                                //���ÿ�������
    int setSystemPoweronParam(QString value);                            //���ÿ�������
    int setSystemPresetParam(int value);                                 //���ø�λ����
    int setSystemPresetParam(QString value);                             //���ø�λ����
    int setSystemLcdBacklight(double value);                             //����LCD����
    int setSystemLcdBacklight(QString value);                            //����LCD����
    int setSystemLcdBacklightWidthArrow(bool up);                        //���������LCD����
    int setSystemLcdBacklightWithRpg(int value);                         //������ť����LCD����
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
    int setSystemChannelSelect(int value);                               //�����ڲ��ź�
    int setSystemChannelSelect(QString value);                           //�����ڲ��ź�
    int setSystemIOGpibMainAddr(int value);                              //����GPIB����ַ
    int setSystemServiceOfEvalute(int value);                            //�������ð汾
    int setSystemServiceOfSelectTimeMode(int value);                     //ѡ��ʱ��ģʽ
    int setSystemServiceOfSelectTimeMode(QString value);                 //ѡ��ʱ��ģʽ
    int setSystemServiceOfSelectLog(int value);                          //ѡ��log��ʾ
    int setSystemServiceOfSelectLog(QString value);                      //ѡ��log��ʾ
    int setSystemServiceOfGpib(int value);                               //ѡ��GPIB
    int setSystemServiceOfGpib(QString value);                           //ѡ��GPIB

    int setSystemServiceOfTrackSelect(int value);                        //ѡ�����Դģʽ
    int setSystemServiceOfTrackSelect(QString value);                    //ѡ�����Դģʽ

    int setSystemServiceOptionOn(QString value);                         //����ϵͳѡ������
    int setSystemServiceOptionOff(QString value);                        //����ϵͳѡ���ر�

    int setSystemServiceDeviceOn(QString value);                         //����ϵͳ�豸����
    int setSystemServiceDeviceOff(QString value);                        //����ϵͳ�豸�ر�

    int setSystemServiceCalbrationMode(int value);

    int setSystemServiceSerialNoMode(int value);                         //ѡ�����к�ģʽ
    int setSystemServiceSerialNoMode(QString value);                     //ѡ�����к�ģʽ

    int setSystemServiceOptionFFT10Hz(int value);                        //����ϵͳFFT 10Hzѡ��
    int setSystemServiceOptionFFT10Hz(QString value);                    //����ϵͳFFT 10Hzѡ��


    int setSystemServiceOptionDemod(int value);                          //����ϵͳAM��FMѡ��
    int setSystemServiceOptionDemod(QString value);   
    int setSystemServiceOptionDirectChannel(int Value);                  //����ֱ��ѡ��
    int getSystemServiceOfEvaluteSYDays(void);
    QString getSystemServiceOfMD5(QString value, bool isServer);
    int unLockSystem(void);   											//

    bool isUsbDeviceConnect(void);										 //USB�豸����״̬
    int getFileOfNowSelectIndex(void);                                   //��ȡ��ǰ�ļ�ѡ������
    int setFileOfDeleteTheSelect(void);                                  //ɾ��ָ���ļ�
    int setFileOfDeleteThePage(void);                                    //ɾ����ҳ�ļ�
    int setFileOfDeleteAll(void);                                        //ɾ�������ļ�
    int setFileOfExportTheSelect(void);                                  //����ѡ���ļ�
    int setFileOfExportThePage(void);                                    //������ҳ�ļ�
    int setFileOfExportAll(void);                                        //����ȫ���ļ�

    int setSourceTrackGenState(int value);                               //���ø���Դ״̬
    int setSourceTrackGenState(QString value);                           //���ø���Դ״̬
    int setSourceTrackGenNetworkMeasType(networkMeasType value);         //���������������
    int setSourceTrackGenNetworkMeasType(QString value);                 //���������������
    int setSourceTrackGenOutputPower(double value);                      //���ø���Դ�������
    int setSourceTrackGenOutputPower(QString value);                     //���ø���Դ�������
    int setSourceTrackGenOutputPowerWithArrow(bool up);                  //��������ø���Դ�������
    int setSourceTrackGenOutputPowerWithRpg(int value);                  //������ť���ø���Դ�������
    int setSourceTrackGenNetworkState(int state);                        //���ø���Դ�������״̬
    int setSourceTrackGenNetworkState(QString value);                    //���ø���Դ�������״̬
    int setSourceTrackGenNetworkNormalize(void);                         //����Դ����������ݹ�һ��
    int setSourceTrackGenNetworkReflevel(double value);                  //���ø���Դ��������ο���ƽ
    int setSourceTrackGenNetworkReflevel(QString value);                 //���ø���Դ��������ο���ƽ
    int setSourceTrackGenNetworkReflevelWithArrow(bool up);              //��������ø���Դ��������ο���ƽ
    int setSourceTrackGenNetworkReflevelWithRpg(int value);              //������ť���ø���Դ��������ο���ƽ
    int setSourceTrackGenNetworkOutputPower(double value);               //���ø���Դ��������������
    int setSourceTrackGenNetworkOutputPower(QString value);              //���ø���Դ��������������
    int setSourceTrackGenNetworkOutputPowerWithArrow(bool up);           //��������ø���Դ��������������
    int setSourceTrackGenNetworkOutputPowerWithRpg(int value);           //������ť���ø���Դ��������������
    int setSourceTrackCalibrateParam(void);                              //���ø���ԴУ׼����
    int setSourceTrackGenVSWRState(int state);                           //���õ�ѹפ����״̬
    int setSourceTrackGenVSWRState(QString value);                       //���õ�ѹפ����״̬
    int setSourceTrackGenVSWRRefLevel(double value);                     //���õ�ѹפ���Ȳο�
    int setSourceTrackGenVSWRRefLevel(QString value);                    //���õ�ѹפ���Ȳο�
    int setSourceTrackGenVSWRRefLevelWithArrow(bool up);                 //��������õ�ѹפ���Ȳο�
    int setSourceTrackGenVSWRRefLevelWithRpg(int value);                 //������ť���õ�ѹפ���Ȳο�

    int setSourceSignalCalibrateParam(void);                             //�����ź�ԴУ׼����
    int setSourceAttention(double value);                                //����Դ���غ�˥����
    int setSourceSignalGenState(int value, bool reset);                  //�����ź�Դ״̬
    int setSourceSignalGenState(QString value, bool reset);              //�����ź�Դ״̬
    int setSourceSignalGenOutputFreq(double value);                      //�����ź�Դ���Ƶ��
    int setSourceSignalGenOutputFreq(QString value);                     //�����ź�Դ���Ƶ��
    int setSourceSignalGenOutputFreqWithArrow(bool up);                  //����������ź�Դ���Ƶ��
    int setSourceSignalGenOutputFreqWithRpg(int value);                  //������ť�����ź�Դ���Ƶ��
    int setSourceSignalGenOutputPower(double value);                     //�����ź�Դ���Ƶ��
    int setSourceSignalGenOutputPower(QString value);                    //�����ź�Դ���Ƶ��
    int setSourceSignalGenOutputPowerWithArrow(bool up);                 //����������ź�Դ���Ƶ��
    int setSourceSignalGenOutputPowerWithRpg(int value);                 //������ť�����ź�Դ���Ƶ��
    void setSourceTrackGenPllFreq(int index);                            //���ø���Դ���໷Ƶ��
    void setSourceTrackGenPll(int index);                                //���ø���Դ���໷
    void setSourceSignalCalibrateParamOfFreq(int index);                 //�����ź�ԴУ׼Ƶ��
    void loadDiscConfig(void);                                           //�����Ƶ����
    void saveDiscConfig(void);                                           //�����Ƶ����
    void loadSourceConfig(void);                                         //����Դ����
    void saveSourceConfig(void);                                         //����Դ����
    void saveSourceConfigOfNetworkMeasure(void);                         //���������������

    int setDemodOfSoundState(int value);                                 //���ý��״̬
    int setDemodOfSoundState(QString value);                             //���ý��״̬
    int setDemodOfMode(int value);                                       //���ý��ģʽ
    int setDemodOfMode(QString value);                                   //���ý��ģʽ
    int setDemodOfSound(double value);                                   //��������
    int setDemodOfSound(QString value);                                  //��������
    int setDemodOfSoundWithArrow(bool up);                               //�������������
    int setDemodOfSoundWithRpg(int value);                               //������ť��������
    int setDemodOfTheFrequency(int index, double freq);                  //����ָ�����Ƶ��
    int setDemodOfTheFrequency(int index, QString value);                //���ý����̨Ƶ��
    int setDemodOfTheFrequencyWithArrow(int index, bool up);             //��������ý����̨Ƶ��
    int setDemodOfTheFrequencyWithRpg(int index, int value);             //������ť���ý����̨Ƶ��
    int setDemodModeOfFreq(int index);                                   //���ý����̨Ƶ��

    int setDemodOfAmState(int value);                                    //����AM���
    int setDemodOfAmState(QString value);                                //����AM���
    int setDemodOfFmState(int value);                                    //����FM���
    int setDemodOfFmState(QString value);                                //����FM���
    int setDemodOfAmCarrierFreq(double value);                           //����AM�ز�Ƶ��
    int setDemodOfAmCarrierFreq(QString value);                          //����AM�ز�Ƶ��
    int setDemodOfAmCarrierFreqWithArrow(bool up);                       //���������AM�ز�Ƶ��
    int setDemodOfAmCarrierFreqWithRpg(int value);                       //������ť����AM�ز�Ƶ��
    int setDemodOfAmIFBWState(int value);                                //������Ƶ����״̬
    int setDemodOfAmIFBWState(QString value);                            //������Ƶ����״̬
    int setDemodOfAmIFBW(double value);                                  //����AM��Ƶ����
    int setDemodOfAmIFBW(QString value);                                 //����AM��Ƶ����
    int setDemodOfAmIFBWWithArrow(bool up);                              //���������AM��Ƶ����
    int setDemodOfAmIFBWWithRpg(int value);                              //������ť����AM��Ƶ����
    int setDemodOfFmCarrierFreq(double value);                           //����FM�ز�Ƶ��
    int setDemodOfFmCarrierFreq(QString value);                          //����FM�ز�Ƶ��
    int setDemodOfFmCarrierFreqWithArrow(bool up);                       //���������FM�ز�Ƶ��
    int setDemodOfFmCarrierFreqWithRpg(int value);                       //������ť����FM�ز�Ƶ��
    int setDemodOfFmIFBWState(int value);                                //������Ƶ����״̬
    int setDemodOfFmIFBWState(QString value);                            //������Ƶ����״̬
    int setDemodOfFmIFBW(double value);                                  //����FM��Ƶ����
    int setDemodOfFmIFBW(QString value);                                 //����FM��Ƶ����
    int setDemodOfFmIFBWWithArrow(bool up);                              //���������FM��Ƶ����
    int setDemodOfFmIFBWWithRpg(int value);                              //������ť����FM��Ƶ����

    int setImpedanceOfState(int value);                                  //�����迹ƥ��
    int setImpedanceOfState(QString value);                              //�����迹ƥ��

    void getSystemFileList(void);                                        //��ȡ�ļ��б�

    int execPrintTrace(void);                                            //��ӡ����
    int execPrintScreen(void);                                           //��ӡ��Ļ

    void getSystemDateTime(void);                                        //��ȡϵͳ����ʱ��
    void getSystemFont(void);                                            //��ȡϵͳ����
    void presetSystemData(void);                                         //��λϵͳ����
    int presetSystem(void);                                              //��λϵͳ

    void rfDownload(unsigned int addr, unsigned long value);             //��Ƶ�����·�
    void ifDownload(unsigned int addr, unsigned long value);             //��Ƶ�����·�
    void feDownload(unsigned int addr, unsigned long value)
    {
    	if (feHandle > 0)
    	{
    		ioctl(feHandle, addr, value);
    	}
    };						 //��Ƶǰ�˲����·�
    void ramDownload(unsigned int addr, unsigned long value)
    {
    	if (ramHandle > 0)
    	{
    		ioctl(ramHandle, addr, value);
    	}
    };						 //��Ƶǰ�˲����·�
   // void tqIfDownload(unsigned int addr, unsigned long value);

    int setMarkerAllOff(void);                                           //�ر�����Ƶ��
    int setMarkerXValue(int mkrIndex, QString value);                    //���ñ��Xֵ
    int setMarkerXValue(int mkrIndex, double value);                     //���ñ��Xֵ
    int isMarkerAllOff(void);                                            //��Ǿ��ر�
    int isTheMarkerOff(int mkrIndex, int traceIndex);                    //��ǹر�
    void setMarkerNormal(int mkrIndex, int traceIndex);                  //���ó�̬���
    void setMarkerDelta(int mkrIndex, int traceIndex);                   //���ò�ֵ���
    double getTheMarkerXValue(int mkrIndex, int xIndex);                 //��ȡָ������Xֵ
    double getTheMarkerYValue(int mkrIndex, int xIndex);                 //��ȡָ������Yֵ
    double getMarkerXValue(int mkrIndex);                                //��ȡ���Xֵ
    double getMarkerYValue(int mkrIndex);                                //��ȡ���Yֵ
    int setMarkerTable(int value);                                       //����Ƶ���б�
    int setMarkerTable(QString value);                                   //����Ƶ���б�

    int setPeakTable(int value);                                         //����EMI��ֵ�б�
 //   int setPeakTable(QString value);                                   //����Ƶ���б�

    int setMarkerToCenterFreq(int mkrIndex);                             //��ǡ�����Ƶ��
    int setMarkerToCenterFreqStep(int mkrIndex);                         //��ǡ�����Ƶ�ʲ���
    int setMarkerToStartFreq(int mkrIndex);                              //��ǡ���ʼƵ��
    int setMarkerToStopFreq(int mkrIndex);                               //��ǡ���ֹƵ��
    int setMarkerToRefLevel(int mkrIndex);                               //��ǡ��ο���ƽ
    int setMarkerDeltaToSpan(int mkrIndex);                              //��ǡ���ɨ��
    int setMarkerDeltaToCenterFreq(int mkrIndex);                        //��ǡ�������Ƶ��

    int setMarkerFunctionAllOff(void);                                   //����Ƶ�깦�ܹر�
    int setMarkerFunctionNdBOn(int value);                               //����Ƶ�깦��NdB����
    int setMarkerFunctionNdBOn(QString value);                           //����Ƶ�깦��NdB����
    int setMarkerFunctionNdBValue(double value);                         //����Ƶ�깦��NdBֵ
    int setMarkerFunctionNdBValue(QString value);                        //����Ƶ�깦��NdBֵ
    int setMarkerFunctionNdBValueWithArrow(bool up);                     //���������Ƶ�깦��NdBֵ
    int setMarkerFunctionNdBValueWithRpg(int value);                     //������ť����Ƶ�깦��NdBֵ
    int setMarkerFunctionFreqCountOn(int value);                         //����Ƶ�깦��Ƶ�ʼ�������
    int setMarkerFunctionFreqCountOn(QString value);                     //����Ƶ�깦��Ƶ�ʼ�������
    int setMarkerFunctionFreqCountBw(int value);                         //����Ƶ�깦��Ƶ�ʼ����ֱ���
    int setMarkerFunctionFreqCountBw(QString value);                     //����Ƶ�깦��Ƶ�ʼ����ֱ���
    int setMarkerFunctionFreqCountBwWithArrow(bool up);                  //���������Ƶ�깦��Ƶ�ʼ����ֱ���
    int setMarkerFunctionFreqCountBwWithRpg(int value);                  //������ť����Ƶ�깦��Ƶ�ʼ����ֱ���
    int getMarkerFunctionNdB(int mkrIndex);                              //��ȡNdB
    int setMarkerFunctionNoiseOn(int value);                             //����Ƶ�깦����������
    int setMarkerFunctionNoiseOn(QString value);                         //����Ƶ�깦����������
    int getMarkerFunctionNoise(int mkrIndex);                            //��ȡ����
    int setMarkerFunctionNoiseStartFreq(QString value);
    int setMarkerFunctionNoiseSpanFreq(QString value);
    QString getMarkerFunctionNoiseStartFreq(void);
    QString getMarkerFunctionNoiseSpanFreq(void);

    double getTraceMaxValue(int traceIndex);                             //��ȡ�������ֵ
    double getTraceMinValue(int traceIndex);                             //��ȡ������Сֵ

    int setMarkerPeakMax(int mkrIndex, int traceIndex);                  //�������ֵ
    int setMarkerPeakMin(int mkrIndex, int traceIndex);                  //������Сֵ
    int setMarkerPeakNext(int mkrIndex, int traceIndex);                 //������һ��ֵ
    int setMarkerPeakLeft(int mkrIndex, int traceIndex);                 //�������ֵ
    int setMarkerPeakRight(int mkrIndex, int traceIndex);                //�����ҷ�ֵ
    int setMarkerPeakToCenterFreq(int mkrIndex, int traceIndex);         //��ֵ���������Ƶ��
    int setMarkerPeakMaxAuto(int value);                                 //���÷�ֵ���ֵ�Զ�����
    int setMarkerPeakMaxAuto(QString value);                             //���÷�ֵ���ֵ�Զ�����

    int getMarkerPeakMaxIndex(int mkrIndex, int traceIndex);             //��ֵ���ֵ
    int getMarkerPeakMinIndex(int mkrIndex, int traceIndex);             //��ֵ��Сֵ
    int getMarkerPeakNextIndex(int mkrIndex, int traceIndex);            //��һ��ֵ
    int getMarkerPeakLeftIndex(int mkrIndex, int traceIndex);            //���ֵ
    int getMarkerPeakRightIndex(int mkrIndex, int traceIndex);           //�ҷ�ֵ

    double getModifiedEmcBw(double value);                               //Ѱ����ӽ���EMC����
    int getBucDelay(void);                                               //Buc Delay
    double getRecentBw(double value);                                    //��ȡ����Ĵ���
    double getRecentVBw(double value);                                   //��ȡ����Ĵ���
    int getSweepTimeQuieoty(void);                                       //��ȡɨ��ʱ��ϵ��
    void getVbwOffset(void);                                             //��ȡ��Ƶƫ��
    void getModifiedVbw(int arrow);                                      //��ȡ��Ƶ����
    void bwAutoCouple(void);                                             //�����Զ����
    double getRefLevelmVTodBm(double value);                             //mVת��ΪdBm
    double getRefLevelValue(double value);                               //��ȡ��ǰ����ֵ
    double getRefLeveldBmValue(void);                                    //��ȡ����dBmֵ
    double getdBmValue(double value);                                    //��ȡdBmֵ
    double getAmptMaxMinLimit(void);                                     //��ȡ����������
    void getTheAmptMaxMinLimit(double minValue, double maxValue,
    	                       double* min, double* max);                //��ȡ����������
    char* getAttAndUnitString(double value, char* result, int full);     //��ȡ˥���͵�λ�ַ���
    char* getAmptAndUnitString(double value, char* result, int full);    //��ȡ���Ⱥ͵�λ�ַ���
    int getStartLogFreqIndex(double value);                              //��ȡƵ����ʼ����
    int getStopLogFreqIndex(double value);                               //��ȡƵ����ֹ����

    int resetFrequencyRespondOf9kHz(double value);                       //�ָ�Ƶ��9kHz��������
    int resetFrequencyRespondOf20kHz(double value);                      //�ָ�Ƶ��20kHz��������
    int modifyFrequencyRespondOf9kHz(double value);                      //�޸�Ƶ��9kHz��������
    int modifyFrequencyRespondOf9kHz(QString value);                     //�޸�Ƶ��9kHz��������
    int modifyFrequencyRespondOf20kHz(double value);                     //�޸�Ƶ��20kHz��������
    int modifyFrequencyRespondOf20kHz(QString value);                    //�޸�Ƶ��20kHz��������
    int modifyFrequencyRespondOf9kHzWithArrow(bool up);                  //������޸�Ƶ��9kHz��������
    int modifyFrequencyRespondOf9kHzWithRpg(int value);                  //������ť�޸�Ƶ��9kHz��������
    int modifyFrequencyRespondOf20kHzWithArrow(bool up);                 //������޸�Ƶ��20kHz��������
    int modifyFrequencyRespondOf20kHzWithRpg(int value);                 //������ť�޸�Ƶ��20kHz��������

    void controlRf(void);                                                //��Ƶ����
    void controlIf(void);                                                //��Ƶ����
    void controlIfDetector(void);                                        //��Ƶ�첨��ʽ����
    void modifyBandIndex(void);										     									 //�޸�Ƶ������ֵ
    void setFreqSweepStatus(void);					                             //��ɨ���ɨ��״̬ѡ��
    void calSweepTimeOfAuto(void);
    void sweepTimeDown(void);
    void readData(void);
	void getcalData(void);                                            //��ȡ��Ƶ����
	void getDataFromIF(void);                                            //��ȡ��Ƶ����
    void getDataFromUsbDisc(void);                                       //��ȡ��Ƶ����
    void getDataFromDemod(void);                                         //��ȡ�������

    bool isCalibrating(void);											 //�Ƿ�����У׼
    void clearFactoryCalibrateData(void);                                //�������У׼
    void clearUserCalibrateData(void);                                   //����û�У׼

    int factoryCalibrate(int comCal);                                    //����У׼
	int freqCalibrate(double freq);								//����У׼
    int PreamplifierCalibrate(int comCal);                               //ǰ�÷Ŵ�У׼
    int userCalibrate(void);                                             //�û�У׼
    int ZCCalibrate(int comCal);                                         //ֱ��ͨ��У׼
    int ZCPreamplifierCalibrate(int comCal);                             //ֱ��ͨ��ǰ�÷Ŵ�У׼

    QStringList loadLogOfFactoryCalibrate(void);                         //�������У׼��־
    void reconnectSerial(void);                                          //�������Ӵ���

    void showRunMessage(QString title);
    void showRunAlarm(QString title);

    int getRbwIndex(void);                                               //��ȡ�ֱ��������
    int getVbwIndex(void);                                               //��ȡ��Ƶ��������
    int getVbwModIndex(void);                                            //��ȡ��Ƶ������������

    //fft
    void firFilter(double* x, double* h, double* y, int nx, int nh, int ny, int err);
    bool checkFFTMeasure(void);                                          //FFT���������жϼ��
    bool isDirectGatherMode(void);                                       //�Ƿ�Ϊֱ��ģʽ
    bool isRequireFFTMeas(void);                                         //�Ƿ���Ҫ����FFT��������
    bool fftControl(int index);                                          //fft����
    void fftControl(void);
    bool directGatherControl(void);                                      //ֱ�ɿ���
    bool demodControl(void);                                             //�������
    bool demodControlIF(void);                                           //�����Ƶ����

    bool getAmResult(void);                                              //��ȡAM���
    bool getFmResult(void);                                              //��ȡFM���
    int getSinadAndThd(double *unmodul_idata,
    		           double *unmodul_qdata,
    				   char lra,
    				   double fs,
    				   int *harm_num,
    				   double *sinad,
    				   double *thd,
    			       double *snr,
    				   double *enob,
    				   double *modul_freq);	                             //��ȡSINAD��THD

    void firDemod(double xin[], double xout[], int length, int n, double *h);  //FIR�˲�
    void getDemodPeakList(void);                                         //��ȡ��ֵ
    void getDemodDiff(double *data, double *diff, int len);
    bool isDemodOk(unsigned long *iqData, double *mData, int n);         //����ɹ����
    bool isDemodOkOfAm(unsigned long *iqData, double *mData, int n);     //AM����ɹ����
    bool isDemodOkOfFm(unsigned long *iqData, double *mData, int n);     //FM����ɹ����
    void getDemodDataOfAm(unsigned long *iqData, double *depth, double *freq, double fs, int n);
    void getDemodDataOfFm(unsigned long *iqData, double *offset, double *freq, double fs, int n);
    void fft(double *x, double *y, int n, int sign);

    void checkMultiDisplay(multiDispType value);

    void resetPassFailData(void);                                        //��λPass-Fail����
    void loadPassFailLineData(void);                                     //����Pass-Fail���߶�������

    bool getMeasureOfPassFailOfWinMeas(int mkrIndex);                    //��ȡPass-Fail���ڲ������
    int setMeasureOfPassFailOfWinMeasState(int value);                   //����Pass-Fail���ڲ���״̬
    int setMeasureOfPassFailOfWinMeasState(QString value);               //����Pass-Fail���ڲ���״̬
    int setMeasureOfPassFailOfWinLimitLineState(int value);              //����Pass-Fail��ֵ��״̬
    int setMeasureOfPassFailOfWinLimitLineState(QString value);          //����Pass-Fail��ֵ��״̬
    int setMeasureOfPassFailOfWinFreqLineState(int value);               //����Pass-FailƵ����״̬
    int setMeasureOfPassFailOfWinFreqLineState(QString value);           //����Pass-FailƵ����״̬
    int setMeasureOfPassFailOfWinLineValueMode(int value);               //����Pass-Fail����������
    int setMeasureOfPassFailOfWinLineValueMode(QString value);           //����Pass-Fail����������
    int setMeasureOfPassFailOfWinLineValue(double value);                //����Pass-Fail�����߷�ֵ
    int setMeasureOfPassFailOfWinLineValue(QString value);               //����Pass-Fail�����߷�ֵ
    int setMeasureOfPassFailOfWinLineValueWithArrow(bool up);            //���������Pass-Fail�����߷�ֵ
    int setMeasureOfPassFailOfWinLineValueWithRpg(int value);            //������ť����Pass-Fail�����߷�ֵ
    int setMeasureOfPassFailOfWinFreqValueMode(int value);               //����Pass-FailƵ������
    int setMeasureOfPassFailOfWinFreqValueMode(QString value);           //����Pass-FailƵ������
    int setMeasureOfPassFailOfWinFreqValue(double value);                //����Pass-FailƵ��
    int setMeasureOfPassFailOfWinFreqValue(QString value);               //����Pass-FailƵ��
    int setMeasureOfPassFailOfWinFreqValueWithArrow(bool up);            //���������Pass-FailƵ��
    int setMeasureOfPassFailOfWinFreqValueWithRpg(int value);            //������ť����Pass-FailƵ��
    int setMeasureOfPassFailOfWinSweepState(int value);                  //����Pass-Fail����ɨ��״̬
    int setMeasureOfPassFailOfWinSweepState(QString value);              //����Pass-Fail����ɨ��״̬

    bool getMeasureOfPassFailOfLineMeas(int mkrIndex, double canvasMinValue, double canvasMaxValue); //��ȡPass-Fail���߲������
    void refreshMeasureOfPassFailOLineUpPages(void);                     //ˢ��Pass-Fail���߲�������ҳ
    void refreshMeasureOfPassFailOLineLowPages(void);                    //ˢ��Pass-Fail���߲�������ҳ
    void addMeasureOfPassFailOLineUpPoint(void);                         //���� Pass-Fail���߲�����
    void insertMeasureOfPassFailOLineUpPoint(void);                      //���� Pass-Fail���߲�����
    void deleteMeasureOfPassFailOLineUpPoint(void);                      //ɾ�� Pass-Fail���߲�����
    void clearMeasureOfPassFailOLineUpPoint(void);                       //��� Pass-Fail���߲�����
    int setMeasureOfPassFailOfLineMeasState(int value);                  //����Pass-Fail���߲���״̬
    int setMeasureOfPassFailOfLineMeasState(QString value);              //����Pass-Fail���߲���״̬
    int setMeasureOfPassFailOfLineUpState(int value);                    //����Pass-Fail���߲�������״̬
    int setMeasureOfPassFailOfLineUpState(QString value);                //����Pass-Fail���߲�������״̬
    int setMeasureOfPassFailOfLineLowState(int value);                   //����Pass-Fail���߲�������״̬
    int setMeasureOfPassFailOfLineLowState(QString value);               //����Pass-Fail���߲�������״̬
    int setMeasureOfPassFailOfLineShiftMode(int value);                  //����Pass-Fail���߲���ƫ������
    int setMeasureOfPassFailOfLineShiftMode(QString value);              //����Pass-Fail���߲���ƫ������
    int setMeasureOfPassFailOfLineShiftXYValue(double value);            //����Pass-Fail���߲���ƫ��
    int setMeasureOfPassFailOfLineShiftXYValue(QString value);           //����Pass-Fail���߲���ƫ��
    int setMeasureOfPassFailOfLineShiftXYValueWithArrow(bool up);        //���������Pass-Fail���߲���ƫ��
    int setMeasureOfPassFailOfLineShiftXYValueWithRpg(int value);        //������ť����Pass-Fail���߲���ƫ��
    int getMeasureOfPassFailOfLineMeasOfLineUpPages(void);               //��ȡPass-Fail���߲�������ҳ��
    int getMeasureOfPassFailOfLineMeasOfLineLowPages(void);              //��ȡPass-Fail���߲�������ҳ��
    int setMeasureOfPassFailOfLineFreq(double value);                    //����Pass-Fail����Ƶ��
    int setMeasureOfPassFailOfLineFreq(QString value);                   //����Pass-Fail����Ƶ��
    int setMeasureOfPassFailOfLineFreqWithArrow(bool up);                //���������Pass-Fail����Ƶ��
    int setMeasureOfPassFailOfLineFreqWithRpg(int value);                //������ť����Pass-Fail����Ƶ��
    int setMeasureOfPassFailOfLineAmpt(double value);                    //����Pass-Fail���߷���
    int setMeasureOfPassFailOfLineAmpt(QString value);                   //����Pass-Fail���߷���
    int setMeasureOfPassFailOfLineAmptWithArrow(bool up);                //���������Pass-Fail���߷���
    int setMeasureOfPassFailOfLineAmptWithRpg(int value);                //������ť����Pass-Fail���߷���
    void gotoFirstPageOfMeasureOfPassFailLineData(void);                 //Pass-Fail��ҳ
    void gotoPrevPageOfMeasureOfPassFailLineData(void);                  //Pass-Fail��һҳ
    void gotoNextPageOfMeasureOfPassFailLineData(void);                  //Pass-Fail��һҳ
    void gotoLastPageOfMeasureOfPassFailLineData(void);                  //Pass-Failβҳ
    void gotoRowOfMeasureOfPassFailLineData(int value);                  //ָ����

   double getErrorOffftMeasureCalculate(double value,
    		                             unsigned long long index,
    		                             unsigned long long startIndex,
    		                             unsigned long long stopIndex);  //��ȡFFT �������(���)


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

   qint64 getFreeDiskSpace(void);                                        //��ȡʣ���ڴ�

   void execSelfCheck(void);                                             //ִ���Լ�
   QString getSelfCheckText(bool value);                                 //��ȡ�Լ�����ı�
   void outputToScreen(QString title);                                  //�������Ļ
   void outputToScreen(QString title, QString value);                   //�������Ļ
   void outputToScreen(QString title, int progress, int normal);        //�������Ļ

   int setUdpIP(QString value);
   int setUdpPort(QString value);
  protected:
    bool isFillData;
    unsigned int sweepPrevPoints;
    int usbDiscFailNums;
    bool usbDeviceInitedSuccess;                                  			 //Usb����״̬
    libusb_device **devs;                                                //Usb�����豸
    libusb_device_handle *dev_handle;                                    //Usb�豸���
    libusb_context *ctx;                                                 //Usb��Ϣ
    unsigned char usbhostbuf[1024];                                      //USB������

    QTcpServer* serverSocket;                                            //�����SOCKET
    clientSocketDef clientSockets[MAXTCPCLIENTS];                        //�ͻ���SOCKET
    //QTcpSocket* clientSocket;                                          //�ͻ���SOCKET
    QByteArray readBuf;                                                  //��������
    QByteArray writeBuf;                                                 //д������

    bool openServerSocket(void);                                         //�򿪷�����SOCKET
    bool closeServerSocket(void);                                        //�رշ�����SOCKET

    bool discLibraryInitialize(void);                                    //��Ƶ���ʼ��
    bool connectToDiscriminator(void);                                   //���Ӽ�Ƶ��
    void disconectFromUSBDevice(void);                               //�Ͽ���Ƶ��
    bool setDiscriminationDataLen(int len);                              //���ü�Ƶ���ݳ���
    void setDiscriminatorInputGain(int level);                           //���ü�Ƶ��������
    bool usbDiscriminatorSend(unsigned char* data, int len);             //��������
    bool usbDiscriminatorRecv(unsigned char* data, int dataLen,
    		                                       int* recvLen);        //��������
    bool getDiscriminationCalResult(int dw, int* result);                //��ȡ��ƵУ׼���
    void setDiscriminatorFreq(double value);                             //���ü�ƵƵ��

    int setPowerMeterFreq(double value);
    int setPowerMeterFreq(QString value);
    int getDataFromPowerMeter(void);
    bool connectToPowerMeter(void);
    bool usbDeviceSend(unsigned char* data, int len, int endpoint);
    bool usbDeviceRecv(unsigned char* data, int dataLen, int* recvLen, int endpoint);

    void handleScpiCommand(QString cmdStr);                              //����SCPIָ��
    void returnScpiResult(QString value);                                //����SCPI���
    int returnScpiResult(QByteArray value);                              //����SCPI���

    void closeSourceGenerator();                                         //�ر�Դ������
    void closeSourceTrackGenerator();                                    //�رո���Դ
    void closeSourceSignalGenerator();                                   //�ر��ź�Դ
    unsigned long setSourceSignalGenFreq(bool value);                    //�����ź�Դ���Ƶ��

    int sendSerialData(bool isCom, QString cmd);                         //����Serial����
    int setupSerial(void);                                               //���ô���

    void loadDeviceConfig(void);                                         //��ȡ�豸����
    void saveDeviceConfig(void);                                         //�����豸����
    void saveLCDBacklight(void);                                         //����LCD����

    void resetFactoryCalibrationData(void);                              //��λ����У׼����
    void resetPreamplifierCalibrationData(void);                         //��λǰ�÷Ŵ�У׼����
    void resetUserCalibrationData(void);                                 //��λ�û�У׼����
    void resetZcCalibrationData(void);                                   //��λֱ��У׼����
    void resetZcPreamplifierCalibrationData(void);                       //��λֱ��ͨ��ǰ�÷Ŵ�У׼����
    void getFactoryCalibrationData(void);                                //��ȡ����У׼����
    void getPreamplifierCalibrationData(void);                           //��ȡǰ�÷Ŵ�У׼����
    void getUserCalibrationData(void);                                   //��ȡ�û�У׼����
    void getZcCalibrationData(void);                                     //��ȡֱ��У׼����
    void getZcPreamplifierCalibrationData(void);                         //��ȡֱ��ͨ��ǰ�÷Ŵ�У׼����
    void saveImpedanceValue(void);                                       //����75ŷķ�迹ת������ֵ
    void saveFactoryCalibrationData(void);                               //�������У׼����
    void savePreamplifierCalibrationData(void);                          //����ǰ�÷Ŵ�У׼����
    void saveUserCalibrationData(void);                                  //�����û�У׼����
    void saveZcFactoryCalibrationData(void);                             //����ֱ��У׼����
    void saveZcPreamplifierFactoryCalibrationData(void);                 //����ֱ��ͨ��ǰ�÷Ŵ�У׼����
    void saveSystemLog(QString log);                                     //����ϵͳ��־
    void saveLogOfFactoryCalibrate(QString datetime,
    		                       QString result,
    		                       QString reason);                      //�������У׼��־
    void saveLogOfPreamplifierCalibrate(QString datetime,
    		                            QString result,
    		                            QString reason);                 //����ǰ�÷Ŵ�У׼��־

    int printPixmap(QPixmap pix, unsigned char pageSize,
    		                     unsigned char orientation,
    		                     unsigned char copyNums,
    		                     bool colour);                           //��ӡͼƬ


    int ulideDataSend(unsigned short channel,unsigned short cmd,unsigned short addr,unsigned short size,double data);
    bool updateRecordOfClear(void);
    bool updateRecordOfAdd(QString value);
    bool updateSystem(void);
    QString updateFileName(QString value);

  public:
    void ddcControl(double value);                                       //DDC����
    void resetZcParams(void);                                            //��λֱ�ɲ���
    void reCalculateZcParams(void);                                      //���¼���ֱ�ɲ���
  private:
    QString versionString;
    QString returnEofString;
    QString scpiBuffer;                                                  //SCPIջ

    QPixmap pixBackground;                                               //����ͼƬ
    QVector <int> posPeakList;                                           //��ֵ�б�
    struct calRunDataDef calRunData;                                     //У׼����

    bool loadLogarithmDefineData(void);                                  //�������Ԥ��������

    void loadDeviceType(void);
    void saveDeviceType(QString value);
    void loadDiveceLogo(void);
    void saveDeviceLogo(QString value);

    void saveSystemSetting(QString section, QString name, QVariant value);

    void scpiFloatToLen8String(double value, char* result);              //��ֵת��
    void asyncDelay(unsigned long value);                                //�첽��ʱ
    int exitCalibrate(void);                                             //�˳�У׼

    void getSystemYSValue(void);
    void getIfSoftValue(void);                                            //��ȡ���油��
    void setAtt1AndAtt2(void);                                           //��������˥��
    void zeroSpanHandle(void);                                           //��ɨ����

    double zoomInData(double value, int step);                           //���ݷŴ�
    double zoomOutData(double value, int step);                          //������С

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
    int getBoolValue(QString value, bool* ok);                           //��ȡ����ֵ
    unsigned char getCharValue(QString value, bool* ok);                 //��ȡС����
    double getFreqValue(QString value, bool* ok);                        //��ȡƵ��ֵ
    double getAmptValue(QString value, bool* ok);                        //��ȡ����ֵ
    double getdBValue(QString value, bool* ok);                          //��ȡdBֵ
    double getTimeValue(QString value, bool* ok);                        //��ȡʱ��ֵ
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
    int setNewDate(QString value);                                       //����������
    int setNewTime(QString value);                                       //������ʱ��
    QString getNewDateString(QString value);                             //��ȡ�������ַ���
    QString getNewTimeString(QString value);                             //��ȡ��ʱ���ַ���

    int getPeakList(int traceIndex);                                     //��ȡ��ֵ�б�
    double* diff(double* output, const double * input, int n);           //������

    void resetFreqCountParam(void);                                      //��λƵ�ʼ�������

    double setSpanOfMeasure(double value);                               //���ò���ɨ��

    void setTuneFreq(double startFreq, double stopFreq);                 //�����Զ�����Ƶ��
    void setTuneBD2Freq(double freq);                                    //���ò���2Ƶ��
    int getTunePeakIndex(void);                                          //��ȡ�Զ�������ֵ����
    int finishAutoTune(double freq);                                     //�����Զ�����
    int locateAutoTune(double freq);                                     //�Զ�������λ

    int setAutoTuneFreq(double startFreq, double stopFreq);              //�����Զ���гƵ��
    int getAutoTunePeakIndex(void);                                      //��ȡ�Զ���г��ֵ����
    int setAutoTunePeakToCenter(void);                                   //�����Զ���г��ֵ������Ƶ��
    int setAutoTuneRefLevel(void);                                       //�����Զ���г�ο���ƽ

    void closeScpiPortOfAll(void);                                       //�ر�����ͨ�Ŷ˿�
    void closeScpiPortOfLan(void);                                       //�ر�LAN�˿�ͨ��
    void closeScpiPortOfUsb(void);                                       //�ر�USB�˿�ͨ��
    void closeScpiPortOfGpib(void);                                      //�ر�GPIB�˿�ͨ��
    void closeScpiPortOfSerial(void);                                    //�ر�COM�˿�ͨ��

    int signalIsOnline(bool isCom);                                      //�ź�Դ����
    int signalOutputFreqIsvalid(bool isCom, double value);               //�ź�Դ���Ƶ������
    int signalOutputAmptIsvalid(bool isCom, double value);               //�ź�Դ�����������

    void setCalibrateParamOfAbsoluteAmplitude(void);                     //���þ��Է��ȶ���У׼����
    void setCalibrateParamOfIfAttenuation(double attrf, double attif);                           //����˥����У׼����
    void setCalibrateParamOfRfAttenuation(void);
    void setCalibrateDistributeAtt(double ref, double att1, double att2);//��������˥��������
    void setCalibrateParamOfIF(void);                                    //������Ƶ����У׼����
    void setCalibrateDistributeIF(double value);                         //������Ƶ�������
    void setCalibrateParamOfRbw(void);                                   //���÷ֱ���У׼����
    void setCalibrateDistributeRbw(double value);                        //���÷ֱ��ʲ���
    void setCalibrateParamOfFrequencyRespond(void);                      //����Ƶ����ӦУ׼����
    void setCalibrateDistributeFrequencyRespond(int freqIndex, double ref, double rfatt, double ifatt);             //����Ƶ����Ӧ����
    void setZcCalibrateParamOfIF(void);                                  //����ֱ��ͨ����Ƶ����У׼����
    void setZcCalibrateDistributeIF(double value);                       //����ֱ����Ƶ�������
    void setZcCalibrateParamOfFrequencyRespond(int freqIndex,  double rfatt, double ifatt);                    //����ֱ��ͨ��Ƶ����ӦУ׼����
    void setZcCalibrateParamOfRbw(void);                                 //����ֱ��ͨ���ֱ���У׼����
    void setZcCalibrateParamOfAbsoluteAmplitude(void);                   //����ֱ��ͨ�����ȶ����׼����
    void setZcCalibrateDistributeFrequencyRespond(int freqIndex, double ref, double att); //����ֱ��ͨ��Ƶ����Ӧ����
    unsigned char getZcIfdown(double value);							 //��ȡֱ��ͨ����Ƶ�����·�ֵ
    int getCalibrateRunParam(void);                                      //��ȡУ׼���в���
    int getCalibrateValueOfAbsoluteAmplitude(void);                      //��ȡ���Է��ȶ���ֵ

    int factoryCalibrationOfAmplitude(void);                             //����У׼
    int factoryCalibrationOfAttenuation(void);                           //˥����У׼
    int factoryCalibrationOfIF(void);                                    //��Ƶ����У׼
    int factoryCalibrationOfBw(void);                                    //�ֱ���У׼
    int factoryCalibrationOfFreqResp(void);                              //Ƶ��У׼
    void setFrontendAtt(double, double);
    int getSystemTemperature(double* value);                             //��ȡϵͳ�¶�

    double getErrorOfAttenuation(void);                                  //��ȡ˥�������
    double getErrorOfAttenuationofPre(void);                             //��ȡǰ�÷Ŵ�˥�������
    double getErrorOfAttenuationofLowfreq(void);                         //��ȡ��Ƶֱ��˥�������
    double getErrorOfIF(void);                                           //��ȡ��Ƶ�������
    double getErrorOfIFofLowfreq(void);                                  //��ȡֱ�ɵ�Ƶ�������
    double getErrorOfIFofLowfreqofPre(void);                             //��ȡֱ��ͨ��ǰ�÷Ŵ���Ƶ�������
    double getErrorOfAttenuationofLowfreqofPre(void);                    //��ȡ��Ƶֱ��ǰ�÷Ŵ�˥�������
    double getErrorOfIFofPre(void);                                      //��ȡǰ�÷Ŵ���Ƶ�������
    double getErrorOfRbw(void);                                          //��ȡ�ֱ������
    double getErrorOfRbwofPre(void);                                     //��ȡǰ�÷Ŵ�ֱ������
    double getErrorOfRbwofLowfreq(void);                                 //��ȡֱ�ɵ�Ƶ�ֱ������
    double getErrorOfRbwofLowfreqofPre(void);							 //��ȡ��Ƶֱ��ǰ�÷Ŵ�ֱ������
    double getErrorOfLowFreqResp(int index);                             //��ȡֱ��Ƶ�����
    double getErrorOfLowFreqRespofPre(int index);                        //��ȡֱ��ͨ��ǰ�÷Ŵ�Ƶ�����
    double getErrorOfEmcRbw(void);                                       //��ȡEMC�ֱ������
    double getErrorOfFreqResp(int index);                                //��ȡƵ�����
    double getErrorOfFreqRespofPre(int index);                           //��ȡǰ�÷Ŵ�Ƶ�����
    double getErrorOfTemperature(void);                                  //��ȡ�¶����
    double getErrorOfNetworkMeasure(int index);                          //��ȡ����������

    QString getTheTranslatorString(QString enStr);                       //��ȡ�ַ���
    void saveScreen(void);                                               //������Ļ
    void outputMessage(QString title);

    bool connecToServerSocket(void);                          //���ӷ�����
    bool disConnectFromServerSocket(void);                    //�Ͽ�������

    double dBmTomV(double value);
    void reDrawMenuFrame(void);                                          //ˢ�²˵����

    char getZcAttVal(void);
    int getRecentFFTRbwIndex(double value);                             //��ȡrbw����ֵ
    int getRecentSweepRbwIndex(double value);												 //��ȡɨ��ģʽrbw����ֵ
    void updateScanParam(void);
    void updateBandParam(void);											 //������Ƶ�������·�ֵ
    void setSweepBandIndex(void);											 //����ָʾ�·�
    void setHighBandIndex(void);										 //�߲���ָʾ�·�
    void setScanEnable(bool value);
    void restartScan(void);
    void setFTWOfDDS(void);
    void downLo1(void);									//��һ�����·�
    void freqScanStart(void);							//Ƶ��ɨ��
    //void fftWinDown(void);								//fft�������·�
    void vbwDown(void);									//vbw Ȩֵ�·�
    void cicDown(void);									//CIC�˲����·�
    void autoAmptAndReflevel(void);
  public slots:
    virtual void clientSocketDataArrive(int fd, int len);                //�ͻ������ݵ���

  private slots:
    virtual void readSerialData();                                       //����������
    virtual void readUsbSlaveData();                                     //��USB�ӿ�����
    virtual void readGpibData();                                         //��GPIB����

    virtual void acceptClientSocketConnect(void);                        //���ܿͻ��˽�������
    virtual void clientSocketDisConnect(void);                           //�ͻ��˶Ͽ�����
    virtual void clientSocketReadyRead(void);                            //�ͻ��������ݵ���
    virtual void clientSocketError(QAbstractSocket::SocketError);        //�ͻ��˷�������
  signals:
    virtual void showAlarmMessage(QString title, QString msg);
    virtual void refreshScpiState(QString helpStr);                      //ˢ��SCPI״̬
    virtual void refreshWindowState(void);                               //ˢ�´���״̬
    virtual void refreshMenuFrame(unsigned long mainCode, unsigned long subCode);                     //ˢ�°���״̬
    virtual void resetUsbSlaveInterface(void);                           //��λUSB�ӿ�
};

//Server Socket�߳�
class tSocketThread : public QThread
{
  Q_OBJECT
  public:
    virtual void run();                      //ִ����
  signals:
    void clientSocketDataArrive(int fd, int len);
};


#endif
