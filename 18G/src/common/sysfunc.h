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

extern double Log10(double value);                                         //�����
extern char* getDateTimeFileName(char* result);                            //��ȡ����ʱ���ļ�����
extern void getLocalDate(char* result);                                    //��ȡ��������
extern void getLocalTime(char* result);                                    //��ȡ����ʱ��
extern int setPCF8563toLocalDateTime(void);                                //����PCF8563������ʱ��д���ں�
extern void getLocalDateTime(char* result);                                //��ȡ��������ʱ��
extern int setLocalDateTime(void);                                         //���ñ�������ʱ��
extern void getLocalDateTimeConvert(char* result);                         //��ȡ��������ʱ��
extern char* getPMFreqAndUnitString(double value, int full, char* result);
extern double getFreqValue(double value);                                  //��ȡƵ��ֵ
extern int getFreqUnitIndex(double value);                                 //��ȡƵ�ʵ�λ����
extern char* getFCtFreqAndUnitString(double value, int bw, char* result);  //��ȡƵ�ʼ����ַ���
extern char* getFreqAndUnitString(double value, int full, char* result);   //��ȡƵ�ʺ͵�λ�ַ���
extern char* getBwAndUnitString(double value, int full, char* result);	   //��ȡ�ֱ����͵�λ�ַ���
extern char* getTimeAndUnitString(double value, int full, char* result);   //��ȡʱ��͵�λ�ַ���
extern char* getMsTimeAndUnitString(double value, char* result);           //��ȡ����ʱ��͵�λ�ַ���
extern char* intToString(int value, char* result);                         //����ת��Ϊ�ַ���
extern char* intToHex(unsigned char value, char* result);                  //����ת��Ϊʮ������
extern char* intToHex4(unsigned int value, char* result);                  //����ת��Ϊʮ������
extern char* longIntToString(unsigned long value, char* result);           //������ת��Ϊ�ַ���
extern char* longLongIntToString(long long int value, char* result);       //��������ת��Ϊ�ַ���
extern char* floatToStringDot1(double value, char* result);                //������ת��Ϊ�ַ���
extern char* floatToStringDot2(double value, char* result);                //������ת��Ϊ�ַ���
extern char* floatToStringDot3(double value, char* result);                //������ת��Ϊ�ַ���
extern char* floatToStringDot6(double value, char* result);                //������ת��Ϊ�ַ���
extern char* intToQString(int value, char* result);                        //����ת��Ϊʮ������
extern char* floatToString(double value, int sumlen, int dots, char* buf); //������ת��Ϊ�ַ���
extern int charToLower(int c);
extern int hexToI(char s[]);
extern int BintoBCD(int value);                                            //��������תBCD
extern int BCDtoBin(int value);                                            //BCDת��������

inline __s32 i2c_smbus_access(int file, char read_write, __u8 command, int size, union i2c_smbus_data *data);
int eeprom_read_byte(struct eeprom* e, __u16 mem_addr);
int eeprom_write_byte(struct eeprom *e, __u16 mem_addr, __u8 data);
int eeprom_open(char *dev_name, int addr, struct eeprom *e);
int eeprom_close(struct eeprom *e);
int read_from_eeprom(struct eeprom *e, int addr, int size);
int write_to_eeprom(struct eeprom *e, int addr, int data);

#endif
