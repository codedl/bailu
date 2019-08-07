#ifndef SYSFUNC_H_
#define SYSFUNC_H_

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

extern char * strdel(char *a, char *b);// delete b from a if b in a

extern double Log10(double value);                                         //求对数
extern char* getDateTimeFileName(char* result);                            //获取日期时间文件名称
extern void getLocalDate(char* result);                                    //获取本地日期
extern void getLocalTime(char* result);                                    //获取本地时间
extern int setPCF8563toLocalDateTime(void);                                //将从PCF8563读到的时间写到内核
extern void getLocalDateTime(char* result);                                //获取本地日期时间
extern int setLocalDateTime(void);                                         //设置本地日期时间
extern void getLocalDateTimeConvert(char* result);                         //获取本地日期时间
extern char* getPMFreqAndUnitString(double value, int full, char* result);
extern double getFreqValue(double value);                                  //获取频率值
extern int getFreqUnitIndex(double value);                                 //获取频率单位索引
extern char* getFCtFreqAndUnitString(double value, int bw, char* result);  //获取频率计数字符串
extern char* getFreqAndUnitString(double value, int full, char* result);   //获取频率和单位字符串
extern char* getBwAndUnitString(double value, int full, char* result);	   //获取分辨带宽和单位字符串
extern char* getTimeAndUnitString(double value, int full, char* result);   //获取时间和单位字符串
extern char* getMsTimeAndUnitString(double value, char* result);           //获取毫秒时间和单位字符串
extern char* intToString(int value, char* result);                         //整型转化为字符串
extern char* intToHex(unsigned char value, char* result);                  //整型转化为十六进制
extern char* intToHex4(unsigned int value, char* result);                  //整型转化为十六进制
extern char* longIntToString(unsigned long value, char* result);           //长整型转化为字符串
extern char* longLongIntToString(long long int value, char* result);       //长长整型转化为字符串
extern char* floatToStringDot1(double value, char* result);                //浮点数转化为字符串
extern char* floatToStringDot2(double value, char* result);                //浮点数转化为字符串
extern char* floatToStringDot3(double value, char* result);                //浮点数转化为字符串
extern char* floatToStringDot6(double value, char* result);                //浮点数转化为字符串
extern char* intToQString(int value, char* result);                        //整型转化为十六进制
extern char* floatToString(double value, int sumlen, int dots, char* buf); //浮点数转化为字符串
extern int charToLower(int c);
extern int hexToI(char s[]);
extern int BintoBCD(int value);                                            //二进制数转BCD
extern int BCDtoBin(int value);                                            //BCD转二进制数

inline __s32 i2c_smbus_access(int file, char read_write, __u8 command, int size, union i2c_smbus_data *data);
int eeprom_read_byte(struct eeprom* e, __u16 mem_addr);
int eeprom_write_byte(struct eeprom *e, __u16 mem_addr, __u8 data);
int eeprom_open(char *dev_name, int addr, struct eeprom *e);
int eeprom_close(struct eeprom *e);
int read_from_eeprom(struct eeprom *e, int addr, int size);
int write_to_eeprom(struct eeprom *e, int addr, int data);

#endif
