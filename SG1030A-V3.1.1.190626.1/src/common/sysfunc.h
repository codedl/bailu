#ifndef SYSFUNC_H_
#define SYSFUNC_H_

#include <QtGui/QApplication>
#include <QtCore/QMutex>
#include <QtCore/QDir>
#include <QtCore/QString>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>


#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <linux/fs.h>
#include <sys/ioctl.h>

#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#include "sysstruct.h"
#include "sysvar.h"
#include "messages.h"

extern double getFreqValue(double value);                                  //��ȡƵ��ֵ
extern int getFreqUnitIndex(double value);                                 //��ȡƵ�ʵ�λ����
extern char* getFreqString(double value, char* result);                    //��ȡƵ���ַ���
extern char* getFreqStringInLF(double value, char* result);                //��LF�л�ȡƵ���ַ���

extern char* getFreqAndUnitString(double value, int full, char* result);   //��ȡƵ�ʺ͵�λ�ַ���
extern char* getTimeAndUnitString(double value, int full, char* result);   //��ȡʱ��͵�λ�ַ���
extern char* getMsTimeAndUnitString(double value, char* result);           //��ȡ����ʱ��͵�λ�ַ���
extern char* getMsTimeValue(double value, char* result);                    //��ȡ����ʱ��
extern char* intToString(unsigned int value, char* result);                //����ת��Ϊ�ַ���
extern char* intToHex(unsigned char value, char* result);                  //����ת��Ϊʮ������
extern char* intToHex4(unsigned int value, char* result);                  //����ת��Ϊʮ������
extern char* longIntToString(unsigned long value, char* result);           //������ת��Ϊ�ַ���
extern char* longLongIntToString(long long int value, char* result);       //��������ת��Ϊ�ַ���
extern char* floatToStringDot1(double value, char* result);                //������ת��Ϊ�ַ���
extern char* floatToStringDot2(double value, char* result);                //������ת��Ϊ�ַ���
extern char* floatToStringDot3(double value, char* result);                //������ת��Ϊ�ַ���
extern char* intToQString(int value, char* result);                        //����ת��Ϊʮ������
extern char* floatToString(double value, int sumlen, int dots, char* buf); //������ת��Ϊ�ַ���
extern int charToLower(int c);
extern int hexToI(char s[]);
extern int BintoBCD(int value);                                            //��������תBCD
extern int BCDtoBin(int value);                                            //BCDת��������

extern void formatStringByChar(QString& org);
extern void insertThousands(char *input);
extern void removeThousandsQString(QString& input);
extern void removeThousandsChar(char *inputChar);


extern QString doubleToQValue(double inputValue);
//extern char* doubleToString(double value, char* result);

extern double valueToDValue(double inputValue);

extern QString freqToQUnit(double freq);
extern QString vppToQUnit(float Vpp);
extern QString amplToQUnit(float ampl);
extern QString timeToQUnit(double time);

extern QString demodTimeToQUnit(double time);
extern QString insertThousandsQS(QString& org);
extern QString insertThousandsCH(char *inputChar);

extern unsigned char bqstringToUchar(QString str);


#endif
