#include <math.h>
#include "sysfunc.h"

#define PCF8563_ADDR    0x51		     //设备地址
#define I2C_DEV		    "/dev/i2c-1"

//获取频率字符串
char* getFreqString(double value, char* result)
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
	char tempUnit[32];

	switch (index)
	{
		case 0:
			sprintf(temp, "%16.9f", tempValue);
			break;
		case 1:
			sprintf(temp, "%16.6f", tempValue);
			break;
		case 2:
			sprintf(temp, "%16.3f", tempValue);
			break;
		case 3:
			sprintf(temp, "%16.0f", tempValue);
			break;
	}

	strcpy(result, temp);

	return result;
}

//获取频率字符串在LF中；由于LF中的频率的最小单位是0.01Hz，故作此修改
char* getFreqStringInLF(double value, char* result)
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
	char tempUnit[32];

	switch (index)
	{
		case 0:
			sprintf(temp, "%16.11f", tempValue);
			break;
		case 1:
			sprintf(temp, "%16.8f", tempValue);
			break;
		case 2:
			sprintf(temp, "%16.5f", tempValue);
			break;
		case 3:
			sprintf(temp, "%16.2f", tempValue);
			break;
	}

	strcpy(result, temp);

	return result;
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
			case 0:
				sprintf(temp, "%16.9f GHz", tempValue);
				break;
			case 1:
				sprintf(temp, "%16.6f MHz", tempValue);
				break;
			case 2:
				sprintf(temp, "%16.3f kHz", tempValue);
				break;
			case 3:
				sprintf(temp, "%16.0f Hz", tempValue);
				break;
		}
	} else
	{
		char tempDef[16] = { '\0' };
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
					case 0:
						if (xs > 9)
							xs = 9;
						break;
					case 1:
						if (xs > 6)
							xs = 6;
						break;
					case 2:
						if (xs > 3)
							xs = 3;
						break;
					case 3:
						if (xs > 0)
							xs = 0;
						break;
				}

				break;
			}
		}

		total = zs + xs;

		gcvt(tempValue, total, temp);

		switch (index)
		{
			case 0:
				strcat(temp, " GHz");
				break;
			case 1:
				strcat(temp, " MHz");
				break;
			case 2:
				strcat(temp, " kHz");
				break;
			case 3:
				strcat(temp, " Hz");
				break;
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
			case 0:
				sprintf(temp, "%16.3f s", tempValue);
				break;
			case 1:
				sprintf(temp, "%16.3f ms", tempValue);
				break;
			case 2:
				sprintf(temp, "%16.3f us", tempValue);
				break;
			case 3:
				sprintf(temp, "%16.3f ns", tempValue);
				break;
		}
	} else
	{
		char tempDef[16] = { '\0' };
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
				if (xs > 3)
					xs = 3;
				break;
			}
		}

		total = zs + xs;
		gcvt(tempValue, total, temp);

		switch (index)
		{
			case 0:
				strcat(temp, " s");
				break;
			case 1:
				strcat(temp, " ms");
				break;
			case 2:
				strcat(temp, " us");
				break;
			case 3:
				strcat(temp, " ns");
				break;
		}
	}

	strcpy(result, temp);
	return result;
}

//获取毫秒时间和单位字符串
char* getMsTimeAndUnitString(double value, char* result)
{
	//  double tempValue = value / 1e6;  //ms
	double tempValue = value; //ms
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
		case 0:
			sprintf(temp, "%16.3f ms", tempValue);
			break;
		case 1:
			sprintf(temp, "%16.3f s", tempValue);
			break;
	}

	strcpy(result, temp);
	return result;
}

char* getMsTimeValue(double value, char* result)
{
	//  double tempValue = value / 1e6;  //ms
	double tempValue = value; //ms
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
		case 0:
			sprintf(temp, "%16.0f", tempValue);
			break;
		case 1:
			sprintf(temp, "%16.3f", tempValue);
			break;
	}

	strcpy(result, temp);
	return result;
}

//整型转化为字符串
char* intToString(unsigned int value, char* result)
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
	sprintf(temp, "%4.1f", value);
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
#if 0
//double型转字符串
char* doubleToString(double value, char* result)
{
	char temparry[32];
	double temp = value;
	if((temp >= 0) && (temp < 1000))
	{
		sprintf(temparry,"%.0f",temp);
	}
	else if((temp >= 1000) && (temp < 1000000))
	{
		temp /= 1000;
		sprintf(temparry,"%.3f",temp);
	}
	else if((temp >= 1000000) && (temp < 1000000000))
	{
		temp /= 1000000;
		sprintf(temparry,"%.6f",temp);
	}
	else if(temp >= 1000000000)
	{
		temp /= 1000000000;
		sprintf(temparry,"%.9f",temp);
	}
	else if((temp > -1000) && (temp < 0))
	{
		sprintf(temparry,"%.0f",temp);
	}
	else if((temp > -1000000) && (temp <= -1000))
	{
		temp /= 1000;
		sprintf(temparry,"%.3f",temp);
	}
	else if((temp > -1000000000) && (temp <= -1000000))
	{
		temp /= 1000000;
		sprintf(temparry,"%.6f",temp);
	}
	else if(temp <= -1000000000)
	{
		temp /= 1000000000;
		sprintf(temparry,"%.9f",temp);
	}
	strcpy(result, temparry);
	return result;
}
#endif
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
	char tempChar[32] = { '\0' };
	int charSize = sizeof tempChar / sizeof tempChar[0];
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

	if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) //判断是否有前导0x或者0X
	{
		i = 2;
	} else
	{
		i = 0;
	}

	for (; (s[i] >= '0' && s[i] <= '9') || (s[i] >= 'a' && s[i] <= 'z') || (s[i] >= 'A' && s[i] <= 'Z'); ++i)
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
	int tempValue, shi, ge, temp;

	tempValue = value;

	shi = (int) (tempValue / 10);
	ge = (int) (tempValue % 10);

	temp = shi * 16 + ge;

	return temp;
}

//BCD转二进制数
int BCDtoBin(int value)
{
	int tempValue, shi, ge, temp;

	tempValue = value;

	shi = (int) (tempValue / 16);
	ge = (int) (tempValue % 16);

	temp = shi * 10 + ge;

	return temp;
}

//QString类型插入千分符
void formatStringByChar(QString& org)
{
	int nSpacing = 3;
	const QChar& ch = QChar(' ');
	int index = org.indexOf('.');
	if (index == -1)
		index = 0;
	int length = org.length();

	for (int i = index + nSpacing + 1; i < length; i += nSpacing)
	{
		org.insert(i++, ch);
	}
}

//数组类型插入千分符
void insertThousands(char *inputChar)
{
	QString inputStr;
	const char* temp;

	inputStr = QString(inputChar);
	formatStringByChar(inputStr);

	temp = inputStr.toLatin1().data();
	strcpy(inputChar, temp);
}

void removeThousandsQString(QString& input)
{
	while (input.indexOf(' ') != -1)
	{
		input = input.remove(input.indexOf(' '), 1);
	}
}

void removeThousandsChar(char *inputChar)
{
	QString inputStr;
	const char* temp;

	inputStr = QString(inputChar);

	if (inputStr.indexOf('.') == -1)
	{
		removeThousandsQString(inputStr);
	}

	temp = inputStr.toLatin1().data();
	strcpy(inputChar, temp);
}

/*数据转字符串*/
QString doubleToQValue(double inputValue)
{
	QString value;
	char index;
	double temp = inputValue;

	if ((temp >= 0) && (temp < 1000))
	{
		value.sprintf("%.0f", temp);
	} else if ((temp >= 1000) && (temp < 1000000))
	{
		temp /= 1000;
		value.sprintf("%.3f", temp);
	} else if ((temp >= 1000000) && (temp < 1000000000))
	{
		temp /= 1000000;
		value.sprintf("%.6f", temp);
	} else if (temp >= 1000000000)
	{
		temp /= 1000000000;
		value.sprintf("%.9f", temp);
	} else if ((temp > -1000) && (temp < 0))
	{
		value.sprintf("%.0f", temp);
	} else if ((temp > -1000000) && (temp <= -1000))
	{
		temp /= 1000;
		value.sprintf("%.3f", temp);
	} else if ((temp > -1000000000) && (temp <= -1000000))
	{
		temp /= 1000000;
		value.sprintf("%.6f", temp);
	} else if (temp <= -1000000000)
	{
		temp /= 1000000000;
		value.sprintf("%.9f", temp);
	}
	return value;
}

double valueToDValue(double inputValue)
{
	double value;
	char valuetemp[16];
	if (inputValue < 0)
	{
		value = inputValue;
	} else if ((inputValue >= 0) && (inputValue < 1000))
	{
		value = inputValue;

	} else if ((inputValue >= 1000) && (inputValue < 1000000))
	{
		value = inputValue / 1000;
	} else if ((inputValue >= 1000000) && (inputValue < 1000000000))
	{
		value = inputValue / 1000000;
	} else if (inputValue >= 1000000000)
	{
		value = inputValue / 1000000000;
	}
	return value;
}

/*频率数据转单位*/
QString freqToQUnit(double freq)
{
	char* unit;
	if ((freq >= 0) && (freq < 1000))
	{
		unit = "Hz";
	} else if ((freq >= 1000) && (freq < 1000000))
	{
		unit = "kHz";
	} else if ((freq >= 1000000) && (freq < 1000000000))
	{
		unit = "MHz";
	} else if (freq >= 1000000000)
	{
		unit = "GHz";
	} else if ((freq > -1000) && (freq < 0))
	{
		unit = "Hz";
	} else if ((freq > -1000000) && (freq <= -1000))
	{
		unit = "kHz";
	} else if ((freq > -1000000000) && (freq <= -1000000))
	{
		unit = "MHz";
	} else if (freq <= -1000000000)
	{
		unit = "GHz";
	}
	return unit;
}

/*电压数据转单位*/
QString vppToQUnit(float Vpp)
{
	char* unit;
	if ((Vpp >= 0) && (Vpp < 1000))
	{
		unit = "mVpp";
	} else if ((Vpp >= 1000) && (Vpp < 1000000))
	{
		unit = "Vpp";
	}
	return unit;
}

QString demodTimeToQUnit(double time)
{
	char* unit;

	if (time >= 1e9)
	{
		unit = "s";
	} else if (time >= 1e6)
	{
		unit = "ms";
	} else if (time >= 1e3)
	{
		unit = "us";
	} else
	{
		unit = "ns";
	}
	return unit;

}

/*时间数据转单位*/
QString timeToQUnit(double time)
{
	char* unit;

	if (time >= 0 && time < 1e-3)
	{
		unit = "ns";
	} else if (time >= 1e-3 && time < 1)
	{
		unit = "us";
	}
	if ((time >= 1) && (time < 1000))
	{
		unit = "ms";
	} else if ((time >= 1000) && (time < 1000000))
	{
		unit = "s";
	}
	return unit;
}

/*幅度数据转单位*/
QString amplToQUnit(float ampl)
{
	char* unit;
	//  unit = "dBm";
	/*
	 switch((int)ampl)
	 {
	 case 1:  unit = "dBm";  break;
	 case 2:  unit = "dBuV"; break;
	 case 3:  unit = "mV";   break;
	 default: unit = "dBm";  break;
	 }
	 */
	if (sysData.ampl.ref.state)
	{
		unit = "dB";
	} else
	{
		switch (sysData.ampl.amplUnit)
		{
			case audBm:
				unit = "dBm";
				break;
			case audBuV:
				unit = "dBuV";
				break;
			case aumV:
				unit = "mV";
				break;
			default:
				unit = "dBm";
				break;
		}
	}

	return unit;
}

//QString类型插入千分符
QString insertThousandsQS(QString& org)
{
	int nSpacing = 3;
	QString temp = org;
	const QChar& ch = QChar(' ');
	int index = temp.indexOf('.');
	if (index == -1)
		index = 0;
	int length = temp.length();

	for (int i = index + nSpacing + 1; i < length; i += nSpacing)
	{
		temp.insert(i++, ch);
	}

	return temp;
}

//数组类型插入千分符
QString insertThousandsCH(char *inputChar)
{
	QString inputStr;

	inputStr = QString(inputChar);
	inputStr = insertThousandsQS(inputStr);

	return inputStr;
}

//将用0和1表示的二进制形式的QString转换为unsgined char 型
unsigned char bqstringToUchar(QString str)
{
	unsigned char ret = 0;
	if (str.length() > 8 || str.length() <= 0)
	{
		return 0;
	}
	//	for(int i=0; i<str.length(); i++)
	//	{
	//		if(str.at(i) == '1')
	//		{
	//			ret = ret | (1 << (str.length()-1-i));
	//		}
	//	}
	for (int i = 0; i < 8; i++)
	{
		if (i < str.length())
		{
			if (str.at(i) == '1')
			{
				ret = ret | (1 << (7 - i));
			}
		}

	}
	return ret;
}
