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

extern double getFreqValue(double value);                                  //获取频率值
extern int getFreqUnitIndex(double value);                                 //获取频率单位索引
extern char* getFreqString(double value, char* result);                    //获取频率字符串
extern char* getFreqStringInLF(double value, char* result);                //在LF中获取频率字符串

extern char* getFreqAndUnitString(double value, int full, char* result);   //获取频率和单位字符串
extern char* getTimeAndUnitString(double value, int full, char* result);   //获取时间和单位字符串
extern char* getMsTimeAndUnitString(double value, char* result);           //获取毫秒时间和单位字符串
extern char* getMsTimeValue(double value, char* result);                    //获取毫秒时间
extern char* intToString(unsigned int value, char* result);                //整型转化为字符串
extern char* intToHex(unsigned char value, char* result);                  //整型转化为十六进制
extern char* intToHex4(unsigned int value, char* result);                  //整型转化为十六进制
extern char* longIntToString(unsigned long value, char* result);           //长整型转化为字符串
extern char* longLongIntToString(long long int value, char* result);       //长长整型转化为字符串
extern char* floatToStringDot1(double value, char* result);                //浮点数转化为字符串
extern char* floatToStringDot2(double value, char* result);                //浮点数转化为字符串
extern char* floatToStringDot3(double value, char* result);                //浮点数转化为字符串
extern char* intToQString(int value, char* result);                        //整型转化为十六进制
extern char* floatToString(double value, int sumlen, int dots, char* buf); //浮点数转化为字符串
extern int charToLower(int c);
extern int hexToI(char s[]);
extern int BintoBCD(int value);                                            //二进制数转BCD
extern int BCDtoBin(int value);                                            //BCD转二进制数

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
