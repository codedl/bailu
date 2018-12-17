#include <math.h>
#include "sysfunc.h"

#define PCF8563_ADDR    0x51		     //设备地址
#define I2C_DEV		    "/dev/i2c-1"

//redefine function of log10, speed fast than math.log10
double Log10(double value)
{
  if (value <= 0)
  {
	return 0;
  }

  int zs = 0;
  double xs = 0;
  double tempValue = value;

  while (tempValue >= 10.0)
  {
	zs++;
	tempValue /= 10.0;
  }

  xs = log10Table[(int)(tempValue * 10000)];

  return zs + xs;
};

//获取日期时间文件名称
char* getDateTimeFileName(char* result)
{
  char temp[32];

  time_t t = time(NULL);
  tm* tp = localtime(&t);

  sprintf(temp, "%04d%02d%02d_%02d%02d%02d", tp->tm_year + 1900, tp->tm_mon + 1, tp->tm_mday, tp->tm_hour, tp->tm_min, tp->tm_sec);

  strcpy(result, temp);

  return result;
}

//获取本地日期
void getLocalDate(char* result)
{
  char temp[32];

  time_t t = time(NULL);
  tm* tp = localtime(&t);

  sprintf(temp, "%04d-%02d-%02d", tp->tm_year + 1900, tp->tm_mon + 1, tp->tm_mday);
  strcpy(result, temp);
}

//获取本地时间
void getLocalTime(char* result)
{
  char temp[32];

  time_t t = time(NULL);
  tm* tp = localtime(&t);

  sprintf(temp, "%02d:%02d:%02d", tp->tm_hour, tp->tm_min, tp->tm_sec);
  strcpy(result, temp);
}

//设置本地日期时间
int setLocalDateTime(void)
{
  char temp[32];
  struct eeprom e;

  int year = sysData.system.datetime.year;
  int month = sysData.system.datetime.month;
  int day = sysData.system.datetime.day;
  int hour = sysData.system.datetime.hour;
  int minute = sysData.system.datetime.minute;
  int second = sysData.system.datetime.second;

  if (year >= 2000)
  {
	year -= 2000;
	month &= 0x7F;
  } else
  {
    year -= 1900;
	month |= 0x80;
  }

  iicHandle = eeprom_open(I2C_DEV, PCF8563_ADDR, &e);
  if (iicHandle < 0)
  {
	return _RESULT_FAILED;
  }

  write_to_eeprom(&e, PCF8563_CONTOL_STATUS1_REG_ADDR, 0x00); usleep(1000); //设置控制状态1寄存器
  write_to_eeprom(&e, PCF8563_CONTDOWN_VALUE_REG_ADDR, 0x00); usleep(1000); //设置控制状态2寄存器
  write_to_eeprom(&e, PCF8563_SECONDS_VL_REG_ADDR, second & 0x7F); usleep(1000); //设置秒寄存器
  write_to_eeprom(&e, PCF8563_MINUTE_REG_ADDR, minute & 0x7F); usleep(1000); //设置分钟寄存器
  write_to_eeprom(&e, PCF8563_HOURS_REG_ADDR, hour & 0x3F); usleep(1000); //设置小时寄存器
  write_to_eeprom(&e, PCF8563_DAYS_REG_ADDR, day & 0x3F); usleep(1000); //设置日寄存器
  write_to_eeprom(&e, PCF8563_MONTH_CENTURY_REG_ADDR, month); usleep(1000); //设置月寄存器
  write_to_eeprom(&e, PCF8563_YEAR_REG_ADDR, year); usleep(1000);  //设置年寄存器
  write_to_eeprom(&e, PCF8563_MINUTE_ALARM_REG_ADDR, 0x00); usleep(1000); //设置分钟警告寄存器
  write_to_eeprom(&e, PCF8563_HOUR_ALARM_REG_ADDR, 0x00); usleep(1000); //设置小时警告寄存器
  write_to_eeprom(&e, PCF8563_DAY_ALARM_REG_ADDR, 0x00); usleep(1000); //设置天警告寄存器
  write_to_eeprom(&e, PCF8563_WEEKDAY_ALARM_REG_ADDR, 0x00); usleep(1000); //设置星期警告寄存器
  write_to_eeprom(&e, PCF8563_CLKOUT_REG_ADDR, 0x00); usleep(1000); //设置时钟输出寄存器
  write_to_eeprom(&e, PCF8563_TIMER_CONTROL_REG_ADDR, 0x00); usleep(1000); //设置时间控制寄存器
  write_to_eeprom(&e, PCF8563_CONTDOWN_VALUE_REG_ADDR, 0x00); usleep(1000); //设置时间计数寄存器

  eeprom_close(&e);
  return _RESULT_SUCCESS;
}

//将从PCF8563读到的时间写到内核
int setPCF8563toLocalDateTime(void)
{
  return 0;

  struct eeprom e;
  int year = 0;
  int month = 0;
  int day = 0;
  int hour = 0;
  int minute = 0;
  int second = 0;
  int tempValue = 0;
  char temp[64];

  iicHandle = eeprom_open(I2C_DEV, PCF8563_ADDR, &e);
  if(iicHandle < 0)
  {
    return _I2C_ERROR;
  }

  //读年寄存器
  year = read_from_eeprom(&e, PCF8563_YEAR_REG_ADDR, 0x01);
  if(year ==_I2C_ERROR)
  {
	return _I2C_ERROR;
  }

  if (year > 2037)
  {
	year = 2037;
  }

  //读月寄存器
  month = read_from_eeprom(&e, PCF8563_MONTH_CENTURY_REG_ADDR, 0x01);
  if(month ==_I2C_ERROR)
  {
	return _I2C_ERROR;
  }

  //读日寄存器
  day = read_from_eeprom(&e, PCF8563_DAYS_REG_ADDR, 0x01);
  if(day ==_I2C_ERROR)
  {
	return _I2C_ERROR;
  }

  //读小时寄存器
  hour = read_from_eeprom(&e, PCF8563_HOURS_REG_ADDR, 0x01);
  if(hour ==_I2C_ERROR)
  {
	return _I2C_ERROR;
  }

  //读分钟寄存器
  minute = read_from_eeprom(&e, PCF8563_MINUTE_REG_ADDR, 0x01);
  if(minute ==_I2C_ERROR)
  {
	return _I2C_ERROR;
  }

  //读秒寄存器
  second = read_from_eeprom(&e, PCF8563_SECONDS_VL_REG_ADDR, 0x01);
  if(second ==_I2C_ERROR)
  {
	return _I2C_ERROR;
  }

  eeprom_close(&e);

  sysData.system.datetime.year = year + 2000;
  sysData.system.datetime.month = month;
  sysData.system.datetime.day = day;
  sysData.system.datetime.hour = hour;
  sysData.system.datetime.minute = minute;
  sysData.system.datetime.second = second;

  sprintf(temp, "%04d%02d%02d%02d%02d.%02d", sysData.system.datetime.year, sysData.system.datetime.month,
		                                     sysData.system.datetime.day, sysData.system.datetime.hour,
		                                     sysData.system.datetime.minute, sysData.system.datetime.second);

  QString dateTimeStr1 = "date -s " + QString::fromLocal8Bit(temp) + " &";
  QString dateTimeStr2 = "hwclock -w &";

  if (system(dateTimeStr1.toStdString().c_str()) == 0 && system(dateTimeStr2.toStdString().c_str()) == 0)
  {
	return _RESULT_SUCCESS;
  }else
  {
    return _RESULT_FAILED;
  }

}

//获取本地日期时间
void getLocalDateTime(char* result)
{
  char temp[32];

  time_t t = time(NULL);
  tm* tp = localtime(&t);

  sprintf(temp, "%04d-%02d-%02d %02d:%02d:%02d", tp->tm_year + 1900, tp->tm_mon + 1, tp->tm_mday, tp->tm_hour, tp->tm_min, tp->tm_sec);
  strcpy(result, temp);
}

//获取本地日期时间
void getLocalDateTimeConvert(char* result)
{
  char temp[32];

  time_t t = time(NULL);
  tm* tp = localtime(&t);

  sprintf(temp, "%02d:%02d:%02d %04d-%02d-%02d", tp->tm_hour, tp->tm_min, tp->tm_sec, tp->tm_year + 1900, tp->tm_mon + 1, tp->tm_mday);

  strcpy(result, temp);
}


//获取频率值
double getFreqValue(double value)
{
  double tempValue = value;

  while ((tempValue / 1000) >= 1)
  {
	tempValue /= 1000;
  }

  return tempValue;
}

//获取频率单位索引
int getFreqUnitIndex(double value)
{
  double tempValue = value;
  int index = 3;

  while ((tempValue / 1000) >= 1)
  {
	tempValue /= 1000;
	index--;
  }

  if ((index < 0) && (index > 3))
  {
    index = 3;
  }

  return index;
}

//获取频率计数字符串
char* getFCtFreqAndUnitString(double value, int bw, char* result)
{
  double tempValue = value;
  int index = 3;

  while (abs(tempValue / 1000) >= 1)
  {
	tempValue /= 1000;
	index--;
  }

  if ((index < 0) || (index > 3))
  {
    index = 3;
  }

  char temp[32] = {};

  switch (index)
  {
    case 0:
      switch (bw)
      {
        case 1000: sprintf(temp, "%16.6f GHz", tempValue); break;
        case 100: sprintf(temp, "%16.7f GHz", tempValue); break;
        case 10: sprintf(temp, "%16.8f GHz", tempValue); break;
        default: sprintf(temp, "%16.9f GHz", tempValue); break;
      }

      break;

    case 1:
      switch (bw)
      {
        case 1000: sprintf(temp, "%16.3f MHz", tempValue); break;
        case 100: sprintf(temp, "%16.4f MHz", tempValue); break;
        case 10: sprintf(temp, "%16.5f MHz", tempValue); break;
        default: sprintf(temp, "%16.6f MHz", tempValue); break;
      }

      break;

    case 2:
      switch (bw)
      {
        case 1000: sprintf(temp, "%16.0f kHz", tempValue); break;
        case 100: sprintf(temp, "%16.1f kHz", tempValue); break;
        case 10: sprintf(temp, "%16.2f kHz", tempValue); break;
        default: sprintf(temp, "%16.3f kHz", tempValue); break;
      }

      break;

    case 3:
      sprintf(temp, "%16.0f Hz", tempValue);
      break;
  }

  strcpy(result, temp);

  return result;
}

//获取分辨带宽和单位全字符串
char* getBwAndUnitString(double value, int full, char* result)
{
  double tempValue = value;
  int index = 3;

  while (abs(tempValue / 1000) >= 1)
  {
	tempValue /= 1000;
	index--;
  }

  if ((index < 0) || (index > 3))
  {
    index = 3;
  }

  char temp[32];

  if (full == 1)
  {
    switch (index)
    {
      case 0: sprintf(temp, "%16.9f GHz", tempValue); break;
      case 1: sprintf(temp, "%16.6f MHz", tempValue); break;
      case 2: sprintf(temp, "%16.3f kHz", tempValue); break;
      case 3: sprintf(temp, "%16.0f Hz", tempValue); break;
    }
  } else
  {
	char tempDef[16] = {'\0'};
    int total = 16;
	int zs = 6;
	int xs = 9;
	int i = 0;

	sprintf(tempDef, "%16.9f", tempValue);

	for (i = 0; i < 5; i++)
	{
	  if (tempDef[i] == 0x20)
	  {
	    zs--;
	  } else
	  {
	    break;
	  }
	}

    for (i = total - 1; i >= 7; i--)
    {
	  if (tempDef[i] == 0x30)
	  {
	    xs--;
	  } else
	  {
		switch (index)
		{
		  case 0: if (xs > 9) xs = 9; break;
		  case 1: if (xs > 6) xs = 6; break;
		  case 2: if (xs > 3) xs = 3; break;
		  case 3: if (xs > 0) xs = 0; break;
		}

	    break;
	  }
	}

	total = zs + xs;

	gcvt(tempValue, total, temp);

    switch (index)
    {
      case 0: strcat(temp, " GHz"); break;
      case 1: strcat(temp, " MHz"); break;
      case 2: strcat(temp, " kHz"); break;
      case 3: strcat(temp, " Hz"); break;
    }
  }

  strcpy(result, temp);

  return result;
}

//获取频率和单位全字符串
char* getPMFreqAndUnitString(double value, int full, char* result)
{
  double tempValue = value;
  int index = 3;

  while (abs(tempValue / 1000) >= 1)
  {
	tempValue /= 1000;
	index--;
  }

  if ((index < 0) || (index > 3))
  {
    index = 3;
  }

  char temp[32];

  if (full == 1)
  {
    switch (index)
    {
      case 0: sprintf(temp, "%16.9fGHz", tempValue); break;
      case 1: sprintf(temp, "%16.6fMHz", tempValue); break;
      case 2: sprintf(temp, "%16.3fkHz", tempValue); break;
      case 3: sprintf(temp, "%16.0fHz", tempValue); break;
    }
  } else
  {
	char tempDef[16] = {'\0'};
    int total = 16;
	int zs = 6;
	int xs = 9;
	int i = 0;

	sprintf(tempDef, "%16.9f", tempValue);

	for (i = 0; i < 5; i++)
	{
	  if (tempDef[i] == 0x20)
	  {
	    zs--;
	  } else
	  {
	    break;
	  }
	}

    for (i = total - 1; i >= 7; i--)
    {
	  if (tempDef[i] == 0x30)
	  {
	    xs--;
	  } else
	  {
		switch (index)
		{
		  case 0: if (xs > 9) xs = 9; break;
		  case 1: if (xs > 6) xs = 6; break;
		  case 2: if (xs > 3) xs = 3; break;
		  case 3: if (xs > 0) xs = 0; break;
		}

	    break;
	  }
	}

	total = zs + xs;

	gcvt(tempValue, total, temp);

    switch (index)
    {
      case 0: strcat(temp, "GHz"); break;
      case 1: strcat(temp, "MHz"); break;
      case 2: strcat(temp, "kHz"); break;
      case 3: strcat(temp, "Hz"); break;
    }
  }

  strcpy(result, temp);

  return result;
}

//获取频率和单位全字符串
char* getFreqAndUnitString(double value, int full, char* result)
{
  double tempValue = value;
  int index = 3;

  while (abs(tempValue / 1000) >= 1)
  {
	tempValue /= 1000;
	index--;
  }

  if ((index < 0) || (index > 3))
  {
    index = 3;
  }

  char temp[32];

  if (full == 1)
  {
    switch (index)
    {
      case 0: sprintf(temp, "%16.9f GHz", tempValue); break;
      case 1: sprintf(temp, "%16.6f MHz", tempValue); break;
      case 2: sprintf(temp, "%16.3f kHz", tempValue); break;
      case 3: sprintf(temp, "%16.0f Hz", tempValue); break;
    }
  } else
  {
	char tempDef[16] = {'\0'};
    int total = 16;
	int zs = 6;
	int xs = 9;
	int i = 0;

	sprintf(tempDef, "%16.9f", tempValue);

	for (i = 0; i < 5; i++)
	{
	  if (tempDef[i] == 0x20)
	  {
	    zs--;
	  } else
	  {
	    break;
	  }
	}

    for (i = total - 1; i >= 7; i--)
    {
	  if (tempDef[i] == 0x30)
	  {
	    xs--;
	  } else
	  {
		switch (index)
		{
		  case 0: if (xs > 9) xs = 9; break;
		  case 1: if (xs > 6) xs = 6; break;
		  case 2: if (xs > 3) xs = 3; break;
		  case 3: if (xs > 0) xs = 0; break;
		}

	    break;
	  }
	}

	total = zs + xs;

	gcvt(tempValue, total, temp);

    switch (index)
    {
      case 0: strcat(temp, " GHz"); break;
      case 1: strcat(temp, " MHz"); break;
      case 2: strcat(temp, " kHz"); break;
      case 3: strcat(temp, " Hz"); break;
    }
  }

  strcpy(result, temp);

  return result;
}

//获取时间和单位字符串
char* getTimeAndUnitString(double value, int full, char* result)
{
  double tempValue = value;
  int index = 3;

  while (abs(tempValue / 1000) >= 1)
  {
	if (index <= 0)
	{
	  break;
	}

	tempValue /= 1000;
	index--;
  }

  if ((index < 0) || (index > 3))
  {
    index = 3;
  }

  char temp[32];

  if (full == 1)
  {
    switch (index)
    {
      case 0: sprintf(temp, "%16.3f s", tempValue); break;
      case 1: sprintf(temp, "%16.3f ms", tempValue); break;
      case 2: sprintf(temp, "%16.3f us", tempValue); break;
      case 3: sprintf(temp, "%16.3f ns", tempValue); break;
    }
  } else
  {
	char tempDef[16] = {'\0'};
    int total = 16;
	int zs = 6;
	int xs = 9;
	int i = 0;

	sprintf(tempDef, "%16.9f", tempValue);

	for (i = 0; i < 5; i++)
	{
	  if (tempDef[i] == 0x20)
	  {
	    zs--;
	  } else
	  {
	    break;
	  }
	}

    for (i = total - 1; i >= 7; i--)
    {
	  if (tempDef[i] == 0x30)
	  {
	    xs--;
	  } else
	  {
	    if (xs > 3) xs = 3;
	    break;
	  }
	}

	total = zs + xs;
	gcvt(tempValue, total, temp);

    switch (index)
    {
      case 0: strcat(temp, " s"); break;
      case 1: strcat(temp, " ms"); break;
      case 2: strcat(temp, " us"); break;
      case 3: strcat(temp, " ns"); break;
    }
  }

  strcpy(result, temp);
  return result;
}

//获取毫秒时间和单位字符串
char* getMsTimeAndUnitString(double value, char* result)
{
  double tempValue = value / 1e6;  //ms
  int index = 0;
  char temp[32];

  while (abs(tempValue / 1000) >= 1)
  {
	tempValue /= 1000;
	index++;

	if (index >= 1)
	{
	  break;
	}
  }

  if ((index < 0) || (index > 1))
  {
	index = 0;
  }

  switch (index)
  {
    case 0: sprintf(temp, "%16.3f ms", tempValue); break;
    case 1: sprintf(temp, "%16.3f s", tempValue); break;
  }

  strcpy(result, temp);
  return result;
}

//整型转化为字符串
char* intToString(int value, char* result)
{
  char temp[32];
  sprintf(temp, "%d", value);
  strcpy(result, temp);

  return result;
}

//整型转化为十六进制
char* intToHex(unsigned char value, char* result)
{
  char temp[2];
  sprintf(temp, "%01X", value);
  strcpy(result, temp);

  return result;
}

//整型转化为十六进制
char* intToHex4(unsigned int value, char* result)
{
  char temp[16];
  sprintf(temp, "0x%04x", value);
  strcpy(result, temp);

  return result;
}

//长整型转化为字符串
char* longIntToString(unsigned long value, char* result)
{
  char temp[32];
  sprintf(temp, "%ld", value);
  strcpy(result, temp);

  return result;
}

//长长整型转化为字符串
char* longLongIntToString(long long int value, char* result)
{
  char temp[32];
  sprintf(temp, "%lld", value);
  strcpy(result, temp);

  return result;
}

//浮点数转化为字符串
char* floatToStringDot1(double value, char* result)
{
  char temp[32];
  sprintf(temp, "%16.1f", value);
  strcpy(result, temp);

  return result;
}

//浮点数转化为字符串
char* floatToStringDot2(double value, char* result)
{
  char temp[32];
  sprintf(temp, "%16.2f", value);
  strcpy(result, temp);

  return result;
}

//浮点数转化为字符串
char* floatToStringDot3(double value, char* result)
{
  char temp[32];
  sprintf(temp, "%16.3f", value);
  strcpy(result, temp);

  return result;
}

//浮点数转化为字符串
char* floatToStringDot6(double value, char* result)
{
  char temp[32];
  sprintf(temp, "%16.6f", value);
  strcpy(result, temp);

  return result;
}

//整型转化为十六进制
char* intToQString(int value, char* result)
{
  char temp[32];
  sprintf(temp, "%ld", value);
  strcpy(result, temp);

  return result;
}

//浮点数转化为字符串
char* floatToString(double value, int sumlen, int dots, char* buf)
{
  char tempChar[32] = {'\0'};
  int charSize = sizeof tempChar /sizeof tempChar[0];
  int sumLen = sumlen;
  int dotS = dots;

  sprintf(tempChar, "%16.15f", value);

  if (sumLen < 0)
  {
	sumLen = 0;
  } else if (sumLen > 32)
  {
	sumLen = 32;
  }

  if (dotS < 0)
  {
	dotS = 0;
  } else if (dotS > 15)
  {
	dotS = 15;
  }

  if (sumLen > 0)
  {
	for (int i = sumLen; i < 32; i++)
	{
	  tempChar[i] = '\0';
	}
  } else
  {
    if (dots == 0)
    {
      for (int i = charSize - 1; i >= 0; i--)
      {
    	if (tempChar[i] == 0x0)
    	{
    	  continue;
    	}

    	if (tempChar[i] == 0x30)
    	{
    	  tempChar[i] = '\0';
    	} else
    	{
    	  if (tempChar[i] == 0x2e)
    	  {
   		    tempChar[i] = '\0';
    	  }

    	  break;
    	}
      }
    } else
    {
      for (int i = 0; i < charSize; i++)
      {
        if (tempChar[i] == 0x2e)
        {
          for (int j = i + 1 + dotS; j >= i + 1; j--)
          {
        	if (tempChar[j] == 0x0)
        	{
        	  tempChar[j] = 0x30;
        	} else
        	{
              break;
        	}
          }

          for (int j = i + 1 + dotS; j < charSize; j++)
          {
        	if (tempChar[j] != 0x0)
        	{
        	  tempChar[j] = 0x0;
        	} else
        	{
        	  break;
        	}
          }

          break;
        }
      }
    }
  }

  strcpy(buf, tempChar);

  return buf;
}

int charToLower(int c)
{
  if (c >= 'A' && c <= 'Z')
  {
    return c + 'a' - 'A';
  } else
  {
    return c;
  }
}

int hexToI(char s[])
{
  int i;
  int n = 0;

  if (s[0] == '0' && (s[1]=='x' || s[1]=='X')) //判断是否有前导0x或者0X
  {
    i = 2;
  } else
  {
    i = 0;
  }

  for (; (s[i] >= '0' && s[i] <= '9') || (s[i] >= 'a' && s[i] <= 'z') || (s[i] >='A' && s[i] <= 'Z'); ++i)
  {
    if (charToLower(s[i]) > '9')
    {
      n = 16 * n + (10 + charToLower(s[i]) - 'a');
    } else
    {
      n = 16 * n + (charToLower(s[i]) - '0');
    }
  }

  return n;
}

//二进制数转BCD
int BintoBCD(int value)
{
  int tempValue,shi,ge,temp;

  tempValue = value;

  shi = (int)(tempValue / 10);
  ge = (int)(tempValue % 10);

  temp = shi * 16 + ge;

  return temp;
}

//BCD转二进制数
int BCDtoBin(int value)
{
  int tempValue,shi,ge,temp;

  tempValue = value;

  shi = (int)(tempValue / 16);
  ge = (int)(tempValue % 16);

  temp = shi * 10 + ge;

  return temp;
}

inline __s32 i2c_smbus_access(int file, char read_write, __u8 command, int size, union i2c_smbus_data *data)
{
  struct i2c_smbus_ioctl_data args;

  args.read_write = read_write;
  args.command = command;
  args.size = size;
  args.data = data;

  return ioctl(file, I2C_SMBUS, &args);
}

int eeprom_read_byte(struct eeprom* e, __u16 mem_addr)
{
  int r;
  union i2c_smbus_data data;

  ioctl(e->fd, BLKFLSBUF);

  __u8 buf =  mem_addr & 0x0ff;
  r = i2c_smbus_access(e->fd, I2C_SMBUS_WRITE, buf, I2C_SMBUS_BYTE, NULL);;
  if (r < 0)
  {
	return r;
  }

  if (i2c_smbus_access(e->fd, I2C_SMBUS_READ, 0, I2C_SMBUS_BYTE, &data))
  {
	return -1;
  }	else
  {
	return 0x0FF & data.byte;
  }
}

int eeprom_write_byte(struct eeprom *e, __u16 mem_addr, __u8 data)
{
  int r;
  __u8 command = mem_addr & 0x00ff;
  union i2c_smbus_data i2cdata;
  i2cdata.byte = data;

  r = i2c_smbus_access(e->fd, I2C_SMBUS_WRITE, command, I2C_SMBUS_BYTE_DATA, &i2cdata);

  if(r < 0)
  {
    r = i2c_smbus_access(e->fd, I2C_SMBUS_WRITE, command, I2C_SMBUS_BYTE_DATA, &i2cdata);
  }

  return r;
}

int eeprom_open(char *dev_name, int addr, struct eeprom *e)
{
  int funcs, fd, r;
  e->fd = e->addr = 0;
  e->dev = 0;

  fd = open(dev_name, O_RDWR);
  if(fd <= 0)
  {
	return -1;
  }

  // set working device
  ioctl(fd, I2C_TENBIT, 0);
  ioctl(fd, I2C_SLAVE, addr);

  e->fd = fd;
  e->addr = addr;
  e->dev = dev_name;

  return 0;
}

int eeprom_close(struct eeprom *e)
{
  close(e->fd);
  e->fd = -1;
  e->dev = 0;

  return 1;
}

int read_from_eeprom(struct eeprom *e, int addr, int size)
{
  int temp, i, ret,value;
  for(i = 0; i < size; ++i, ++addr)
  {
	temp = eeprom_read_byte(e, addr);
	if(temp < 0)
	{
	  return _I2C_ERROR;
	}

	if(addr == PCF8563_SECONDS_VL_REG_ADDR ||addr == PCF8563_MINUTE_REG_ADDR)
	{
	  temp &= 0x7F;
	}else if(addr == PCF8563_HOURS_REG_ADDR ||addr == PCF8563_DAYS_REG_ADDR)
	{
	  temp &= 0x3F;
	}else if(addr == PCF8563_WEEKDAYS_REG_ADDR)
	{
	  temp &= 0x07;
	}else if(addr == PCF8563_MONTH_CENTURY_REG_ADDR)
	{
	  temp &= 0x1F;
	}

	if(addr != PCF8563_WEEKDAYS_REG_ADDR)
	{
	  value = BCDtoBin(temp);
	}else
	{
	  value = temp;
	}
  }

  return value;
}

int write_to_eeprom(struct eeprom *e, int addr, int data)
{
  int temp = BintoBCD(data);

  return (eeprom_write_byte(e, addr, temp));
}
