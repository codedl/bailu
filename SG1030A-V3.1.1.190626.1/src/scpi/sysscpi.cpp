/*
 * sysscpi.cpp
 *
 *  Created on: 2016-3-11
 *      Author: hai
 */

#include "sysscpi.h"

//构造
tSysScpi::tSysScpi()
{
	char tempChar[32] = { };

	versionString = "V" + QString(intToString(VER_MAJOR, tempChar)).trimmed() + "." + QString(intToString(VER_MINOR, tempChar)).trimmed() + "." + QString(intToString(VER_REVISION, tempChar)).trimmed()
			+ "." + QString(intToString(VER_BUILD, tempChar)).trimmed() + "." + QString(intToString(VER_TAG, tempChar)).trimmed();

	usbDiscFailNums = 0;
	returnEofString = "";
	scpiBuffer = "";
	fileSelectName = "";
	filesExport = 0;
	gpibReaded = false;
	;
	gpibList.clear();
	isUpdating = false;
	isRcvingLogoFile = false;
	isRcvingSequence = false;

	for (int i = 0; i < sizeof SCPIRESULTEOF / sizeof SCPIRESULTEOF[0]; i++)
	{
		returnEofString += QChar(SCPIRESULTEOF[i]);
	}

	memset(&sysData.scpiData, 0, sizeof sysData.scpiData);
	//  sysData.scpiData.stateL = true;
	closeScpiPortOfAll();
	serverSocket = NULL;
	clientSocket = NULL;
	openServerSocket();

	memset(&eva, 0, sizeof eva);
	QString fileName = QCoreApplication::applicationDirPath() + "/key";
	FILE* fp = fopen(fileName.toStdString().c_str(), "rb");
	if (fp != NULL && QFile(fileName).size() == sizeof(evaluateDataDef))
	{
		fread(&eva, sizeof eva, 1, fp);
		fclose(fp);
	} else
	{
		eva.isEvaluate = true;
		eva.evaluated = true;
	}

	isNeedCloseDemod = false;

	loadDeviceType();
	loadDeviceCompany();
	loadDiveceLogo();
}

//析构
tSysScpi::~tSysScpi()
{

}

//脉冲旋钮设置数值
//value:设置值
//Location：小数位数
double tSysScpi::setValueByRPG(double value, int nums, int location)
{
	//  printf("value = %f,cursorLocation = %d\n",value, sysData.cursorLocation);
	double temp = sysData.cursorLocation - location;
	temp = pow(10.0, temp);
	temp = temp * (double) nums;
	value += temp;

	return value;

	//  if(sysData.EnterPara.Value > MAXVALUE)		sysData.EnterPara.Value = MAXVALUE;
	//  else if(sysData.EnterPara.Value < MINVALUE)	sysData.EnterPara.Value = MINVALUE;
}

//发送Serial数据
int tSysScpi::sendSerialData(QString cmd)
{
	int writecnt;
	if (cmd.isNull() || cmd.isEmpty())
	{
		return __SCPI_FAILED;
	}

	QString fullCmd = cmd.trimmed();

	for (int i = 0; i < sizeof SCPICMDEOF / sizeof SCPICMDEOF[0]; i++)
	{
		fullCmd += SCPICMDEOF[i]; //由于发送下去的多了一个0x0d，故添加下面的if else 语句

		//	if(i==0)
		//		;
		//	else
		//		fullCmd += SCPICMDEOF[i];

	}

	writecnt = write(comHandle, fullCmd.toStdString().c_str(), fullCmd.length() * sizeof(char));
	if (writecnt < 0)
	{
		return __SCPI_FAILED;
	}

	return __SCPI_SUCCESS;
}

bool tSysScpi::isNeedCloseDemodState(void)
{
	return isNeedCloseDemod && (sysData.analog.am.state || sysData.analog.fm.state || sysData.analog.pm.state ||
	//		                      sysData.analog.pluse.state ||
			sysData.digital.ask.state || sysData.digital.fsk.state || sysData.digital.psk.state);
}

//设置频率
int tSysScpi::setFrequency(double value)
{
	double tempValue = value;
	double freqRefValue = 0;

	double minFreq = 0;
	double maxFreq = 0;

	char temp[128] = { };
	char tempChar[128] = { };
	char tempdown[64] = { };

	QString cmdStr = "";
	if (sysData.sweep.mode == smFreq) //设置频率时，当扫描模式是扫频时，需要将扫描模式关闭
	{
		setSweepMode(0);
	} else if (sysData.sweep.mode == smFreqAmpl) //设置频率时，当扫描模式是扫频和扫幅时，需要将扫描模式设置成扫幅
	{
		setSweepMode(2);
	}

	if (sysData.freq.ref.state)
	{
		freqRefValue = sysData.freq.ref.value;
	} else
	{
		freqRefValue = 0;
	}

	//  minFreq = MINFREQ + sysData.freq.offset - freqRefValue;
	//  maxFreq = MAXFREQ + sysData.freq.offset - freqRefValue;
	minFreq = MINFREQ * sysData.freq.multiplier + sysData.freq.offset - freqRefValue;
	maxFreq = MAXFREQ * sysData.freq.multiplier + sysData.freq.offset - freqRefValue;
	if (tempValue >= maxFreq)
	{
		tempValue = maxFreq;

	} else if (tempValue <= minFreq)
	{
		tempValue = minFreq;

	}

	sysData.freq.freq = tempValue;

	sysData.freq.downLoad = (tempValue - sysData.freq.offset) / sysData.freq.multiplier + freqRefValue;
	sysData.freq.prevValue = sysData.freq.downLoad;

	//关闭调制信号
	if (isNeedCloseDemodState())
	{
		//	closeDemod();
	}

	//get input paremwter
	strcpy(sysData.name, "FREQ:");
	strcpy(tempChar, getFreqString(sysData.freq.freq, temp));
	insertThousands(tempChar);
	strcpy(sysData.value, tempChar);
	strcpy(sysData.unit, freqToQUnit(sysData.freq.freq).trimmed().toStdString().c_str());

	// cmdStr = ":FREQ " + doubleToString(sysData.freq.downLoad).trimmed();
	sprintf(tempdown, "%.0f", sysData.freq.downLoad);
	cmdStr = ":FREQ " + QString(tempdown).trimmed();

	//发送
	if (sendSerialData(cmdStr) == __SCPI_FAILED)
	{
		//  printf("send serial failed\n");
		return __SCPI_FAILED;
	}
	// printf("send serial success\n");


//	if (isNeedCloseDemod)
//	{
//		savePowerOffState();
//	}

	//9K ~ 50K, 			幅度： 0dBm ~ -120dBm
	//50kHz ~ 3GHz		幅度：10dBm ~ -120dBm
	if(sysData.freq.downLoad < 50e3)
	{
		if(getdBmValue(sysData.ampl.setValue) > MAXAMPLFIRST)
		{
			setAmplitude(getdBuVmVValue(MAXAMPLFIRST));
			strcpy(sysData.name, "FREQ:");
			strcpy(tempChar, getFreqString(sysData.freq.freq, temp));
			insertThousands(tempChar);
			strcpy(sysData.value, tempChar);
			strcpy(sysData.unit, freqToQUnit(sysData.freq.freq).trimmed().toStdString().c_str());
		}
	}

	return __SCPI_SUCCESS;
}

//设置频率
int tSysScpi::setFrequency(QString value)
{
	bool isOk = false;
	double tempValue = 0;

	tempValue = getFreqValue(value, &isOk);

	if (isOk)
	{
		return setFrequency(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//脉冲旋钮设置频率值
int tSysScpi::setFrequencyWithRpg(int value)
{
	int tempValue = value;

	return setFrequency(setValueByRPG(sysData.freq.freq, tempValue, 0));
}

//设置频率偏置值
int tSysScpi::setFrequencyOffset(double value)
{
	double tempValue = value;
	double freqRefValue = 0;
	char temp[128] = { };
	char tempChar[128] = { };

	QString cmdStr = "";

	if (sysData.freq.ref.state == true)
	{
		freqRefValue = sysData.freq.ref.value;
	} else
	{
		freqRefValue = 0;
	}

	if (tempValue >= MAXFREQOFFSET)
	{
		tempValue = MAXFREQOFFSET;
	} else if (tempValue <= MINFREQOFFSET)
	{
		tempValue = MINFREQOFFSET;
	}

	sysData.freq.offset = tempValue;

	//get input paremwter
	//  strcpy(sysData.name, "Offset:");
	//  strcpy(tempChar, getFreqString( sysData.freq.offset, temp));
	//  insertThousands(tempChar);
	//  strcpy(sysData.value, tempChar);
	//  strcpy(sysData.unit, freqToQUnit(sysData.freq.offset).trimmed().toStdString().c_str());

	sysData.freq.freq = sysData.freq.multiplier * (sysData.freq.downLoad - freqRefValue) + tempValue;

	strcpy(sysData.name, "FREQ:");
	strcpy(tempChar, getFreqString(sysData.freq.freq, temp));
	insertThousands(tempChar);
	strcpy(sysData.value, tempChar);
	strcpy(sysData.unit, freqToQUnit(sysData.freq.freq).trimmed().toStdString().c_str());

	//关闭调制信号
	if (isNeedCloseDemodState())
	{
		closeDemod();
	}

//	if (isNeedCloseDemod)
//	{
//		savePowerOffState();
//	}

	return __SCPI_SUCCESS;
}

//设置频率偏置值
int tSysScpi::setFrequencyOffset(QString value)
{
	bool isOk = false;
	double tempValue = 0;

	tempValue = getFreqValue(value, &isOk);

	if (isOk)
	{
		return setFrequencyOffset(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

int tSysScpi::setFrequencyOffsetWithRpg(int value)
{
	int tempValue = value;

	return setFrequencyOffset(setValueByRPG(sysData.freq.offset, tempValue, 0));
}

//设置频率倍频
int tSysScpi::setFrequencyMultipliter(double value)
{
	double tempValue = value;
	double freqRefValue = 0;
	char temp[128] = { };
	char tempChar[128] = { };

	QString cmdStr = "";

	if (tempValue <= 1)
	{
		tempValue = 1;
	} else if (tempValue >= 100)
	{
		tempValue = 100;
	}
	sysData.freq.multiplier = tempValue;

	//get paramter
	strcpy(sysData.name, "Freq Mult:");
	strcpy(sysData.value, intToQString(sysData.freq.multiplier, temp));
	strcpy(sysData.unit, "x");
	if (sysData.freq.ref.state == true)
	{
		freqRefValue = sysData.freq.ref.value;
	} else
	{
		freqRefValue = 0;
	}

	sysData.freq.freq = sysData.freq.multiplier * (sysData.freq.downLoad - freqRefValue) + sysData.freq.offset;
	//关闭调制信号
	if (isNeedCloseDemodState())
	{
		closeDemod();
	}

//	if (isNeedCloseDemod)
//	{
//		savePowerOffState();
//	}

	return __SCPI_SUCCESS;
}

//设置频率倍频
int tSysScpi::setFrequencyMultipliter(QString value)
{
	bool isOk = false;
	double tempValue = 0;

	tempValue = getIntValue(value, &isOk);

	if (isOk)
	{
		return setFrequencyMultipliter(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//脉冲旋钮设置频率偏置
int tSysScpi::setFrequencyMultipliterWithRpg(int value)
{
	int tempValue = value;

	//  SG1030SetValueByRPG(tempValue, 0, sysData.FreqMult.MaxValue,sysData.FreqMult.MinValue);
	return setFrequencyMultipliter(setValueByRPG(sysData.freq.multiplier, tempValue, 0));

}

//根据当前的频率获取幅度可设置的最大值,返回值单位dBm
//9K ~ 50K, 			幅度： 0dBm ~ -120dBm
//50K ~ 3GHz		幅度：10dBm ~ -120dBm
double tSysScpi::getMaxAmplitude(double freqValue)
{
	double retValue = 0;
	if(freqValue >= 9e3 && freqValue < 50e3)
	{
		retValue = MAXAMPLFIRST;
	}
	else
	{
		retValue = MAXAMPL;
	}
	return retValue;
}

//设置幅度
int tSysScpi::setAmplitude(double value)
{
	double tempValue;
	double amplRefValue = 0;
	double minAmpl = 0;
	double maxAmpl = 0;
	char temp[128] = { };
	char tempChar[128] = { };

	QString cmdStr = "";

	if (sysData.sweep.mode == smFreqAmpl) //当设置幅度时，当扫描模式是扫频和扫幅时，需要将扫描模式设置成扫频
	{
		setSweepMode(1);
	}

	if (sysData.ampl.ref.state)
	{
		switch (sysData.ampl.amplUnit)
		//根据单位设置上下限
		{
			case audBuV:
				if (value > getdBuVmVValue(getMaxAmplitude(sysData.freq.downLoad)))
					value = getdBuVmVValue(getMaxAmplitude(sysData.freq.downLoad));
				else if (value < -13)
					value = -13;
				break;
			case aumV:
				if (value > getdBuVmVValue(getMaxAmplitude(sysData.freq.downLoad)))
					value = getdBuVmVValue(getMaxAmplitude(sysData.freq.downLoad));
				else if (value < 0.000224)
					value = 0.000224;
				break;
			default:
				if (value > getMaxAmplitude(sysData.freq.downLoad) - sysData.ampl.ref.value)
					value = getMaxAmplitude(sysData.freq.downLoad) - sysData.ampl.ref.value;
				else if (value < MINAMPL - sysData.ampl.ref.value)
					value = MINAMPL - sysData.ampl.ref.value;
				break;
		}
	} else
	{
		switch (sysData.ampl.amplUnit)
		//根据单位设置上下限
		{
			case audBuV:
				if (value > getdBuVmVValue(getMaxAmplitude(sysData.freq.downLoad)))
					value = getdBuVmVValue(getMaxAmplitude(sysData.freq.downLoad));
				else if (value < -13)
					value = -13;
				break;
			case aumV:
				if (value > getdBuVmVValue(getMaxAmplitude(sysData.freq.downLoad)))
					value = getdBuVmVValue(getMaxAmplitude(sysData.freq.downLoad));
				else if (value < 0.000224)
					value = 0.000224;
				break;
			default:
				if (value > getMaxAmplitude(sysData.freq.downLoad) + sysData.ampl.offset - sysData.ampl.error.value)
					value = getMaxAmplitude(sysData.freq.downLoad) + sysData.ampl.offset - sysData.ampl.error.value;
				else if (value <MINAMPL + sysData.ampl.offset - sysData.ampl.error.value)
					value = MINAMPL + sysData.ampl.offset - sysData.ampl.error.value;
				break;
		}
	}

	sysData.ampl.ampl = value;
	tempValue = value;
	sysData.ampl.setValue = value; //用户设置值



	if (sysData.ampl.ref.state)
	{
		amplRefValue = sysData.ampl.ref.value;
	} else
	{
		amplRefValue = 0;
	}

	//  minAmpl = MINAMPL + sysData.ampl.offset - amplRefValue - sysData.ampl.error.value;
	//  maxAmpl = MAXAMPL + sysData.ampl.offset - amplRefValue - sysData.ampl.error.value;
	minAmpl = MINAMPL + sysData.ampl.offset - sysData.ampl.error.value;
	maxAmpl = MAXAMPL + sysData.ampl.offset - sysData.ampl.error.value;

	//  tempValue = getdBmValue(tempValue);  //单位转换

	//  tempValue += sysData.ampl.offset;
	//  if(tempValue >= maxAmpl)
	//  {
	//	tempValue = maxAmpl;
	//  }else if(tempValue <= minAmpl)
	//  {
	//	tempValue = minAmpl;
	//  }

	//  sysData.ampl.ampl = tempValue + sysData.ampl.offset;
	sysData.ampl.ampl = tempValue;

	//
	strcpy(sysData.name, "AMPL:");
	//  strcpy(sysData.value, floatToStringDot2((sysData.ampl.setValue + sysData.ampl.offset), temp));  //显示值等于设置值加偏置值
	strcpy(sysData.value, floatToStringDot2(sysData.ampl.ampl, temp));
	strcpy(sysData.unit, amplToQUnit(sysData.ampl.setValue).trimmed().toStdString().c_str()); //zpc add
	sysData.isNeedCable = false;

	//  sysData.ampl.downLoad = sysData.ampl.ampl - sysData.ampl.offset + amplRefValue;
	//  sysData.ampl.downLoad = getdBmValue(tempValue) - sysData.ampl.offset + amplRefValue;
	//  sysData.ampl.downLoad = getdBmValue(sysData.ampl.setValue) + amplRefValue;
	sysData.ampl.downLoad = getdBmValue(sysData.ampl.setValue) + amplRefValue + sysData.ampl.error.value - sysData.ampl.offset;
	//  cmdStr = ":POW " + doubleToString(sysData.ampl.downLoad + sysData.ampl.error.value).trimmed();
	cmdStr = ":POW " + doubleToString(sysData.ampl.downLoad).trimmed();

	//发送
	if (sendSerialData(cmdStr) == __SCPI_FAILED)
	{
		return __SCPI_FAILED;
	}

	//关闭调制信号
	if (isNeedCloseDemodState())
	{
		//	closeDemod();
	}

//	if (isNeedCloseDemod)
//	{
//		savePowerOffState();
//	}

	return __SCPI_SUCCESS;
}

//设置幅度
int tSysScpi::setAmplitude(QString value)
{
	bool isOk = false;
	double tempValue = 0;

	//  tempValue = getDoubleValue(value, &isOk);
	tempValue = getAmptValue(value, &isOk);

	if (isOk)
	{
		return setAmplitude(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//脉冲旋钮设置频率值
int tSysScpi::setAmplitudeWithRpg(int value)
{
	int tempValue = value;

	return setAmplitude(setValueByRPG(sysData.ampl.ampl, tempValue, 2));
}

//设置幅度偏置值
int tSysScpi::setAmplitudeOffset(double value)
{
	double tempValue = value;
	double amplRefValue = 0;

	QString cmdStr = "";
	char temp[128] = { };
	char tempChar[128] = { };

	if (tempValue >= MAXAMPLOFFSET)
	{
		tempValue = MAXAMPLOFFSET;
	} else if (tempValue <= MINAMPLOFFSET)
	{
		tempValue = MINAMPLOFFSET;
	}

	sysData.ampl.offset = tempValue;
	if (sysData.ampl.ref.state)
	{
		//	  sysData.ampl.ampl = sysData.ampl.ampl + sysData.ampl.offset;
		//	  sysData.ampl.ampl = sysData.ampl.setValue - sysData.ampl.ref.value + sysData.ampl.offset;
//		sysData.ampl.ampl = sysData.ampl.setValue + sysData.ampl.offset;
		//	  sysData.ampl.setValue = sysData.ampl.setValue + sysData.ampl.offset;
			  sysData.ampl.ampl = sysData.ampl.downLoad + sysData.ampl.offset;
	} else
	{
//		sysData.ampl.ampl = sysData.ampl.setValue + sysData.ampl.offset;
		sysData.ampl.ampl = sysData.ampl.downLoad + sysData.ampl.offset;	}

	//
	strcpy(sysData.name, "Ampl:");
	strcpy(sysData.value, floatToStringDot2(sysData.ampl.ampl, temp));
//	strcpy(sysData.value, floatToStringDot2(sysData.ampl.downLoad + sysData.ampl.offset, temp	));//显示值 = 下发值 + 偏移值
	strcpy(sysData.unit, amplToQUnit(sysData.ampl.offset).trimmed().toStdString().c_str());

	if (sysData.ampl.ref.state == true)
	{
		amplRefValue = sysData.ampl.ref.value;
	} else
	{
		amplRefValue = 0;
	}

	//  sysData.ampl.ampl = sysData.ampl.downLoad + sysData.ampl.offset - amplRefValue;
	//  sysData.ampl.ampl = sysData.ampl.ampl + sysData.ampl.offset - amplRefValue;

	//关闭调制信号
	if (isNeedCloseDemodState())
	{
		closeDemod();
	}

//	if (isNeedCloseDemod)
//	{
//		savePowerOffState();
//	}

	return __SCPI_SUCCESS;
}

//设置幅度偏置值
int tSysScpi::setAmplitudeOffset(QString value)
{
	bool isOk = false;
	double tempValue = 0;

	tempValue = getDoubleValue(value, &isOk);

	if (isOk)
	{
		return setAmplitudeOffset(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

int tSysScpi::setAmplitudeOffsetWithRpg(int value)
{
	int tempValue = value;

	return setAmplitudeOffset(setValueByRPG(sysData.ampl.offset, tempValue, 2));
}

//设置幅度差损
int tSysScpi::setAmplitudeError(double value)
{
	double max = MAXAMPL - sysData.ampl.downLoad;
	double min = 0;
	char temp[128] = { };
	char tempChar[128] = { };

	if (value >= max)
	{
		value = max;
	} else if (value <= min)
	{
		value = min;
	}

	sysData.isNeedCable = true;

	sysData.ampl.error.value = value;

	//
	strcpy(sysData.name, "I.L Cable:");
	strcpy(sysData.value, floatToStringDot2(sysData.ampl.error.value, temp));
	strcpy(sysData.unit, "dB");

	//  QString cmdStr = ":POW " + doubleToString(sysData.ampl.downLoad + sysData.ampl.error.value).trimmed();
	QString cmdStr = "";
	if (sysData.ampl.ref.state == true)
	{
		if (sysData.ampl.setValue != 0)
		{
			cmdStr = ":POW " + doubleToString(getdBmValue(sysData.ampl.setValue) + sysData.ampl.ref.value + sysData.ampl.error.value).trimmed();
		} else
		{
			cmdStr = ":POW " + doubleToString(sysData.ampl.ref.value + sysData.ampl.error.value).trimmed();
		}

	} else
	{
		cmdStr = ":POW " + doubleToString(getdBmValue(sysData.ampl.setValue) + sysData.ampl.error.value).trimmed();
	}

	//发送
	if (sendSerialData(cmdStr) == __SCPI_FAILED)
	{
		return __SCPI_FAILED;
	}

	//  setAmplitude(tempAmpl);

	return __SCPI_SUCCESS;
}

//设置幅度差损
int tSysScpi::setAmplitudeError(QString value)
{
	bool isOk = false;
	double tempValue = 0;

	tempValue = getDoubleValue(value, &isOk);

	if (isOk)
	{
		return setAmplitudeError(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//脉冲旋钮设置幅度差损
int tSysScpi::setAmplitudeErrorWithRpg(int value)
{
	int tempValue = value;

	return setAmplitudeError(setValueByRPG(sysData.ampl.error.value, tempValue, 2));
}

//设置频率参考
int tSysScpi::setFrequencyOfReference(void)
{
	QString cmdStr = "";
	char temp[128] = { };
	char tempChar[128] = { };

	sysData.freq.ref.state = true;

	sysData.freq.ref.value = sysData.freq.downLoad;

	//get input paremwter
	//  strcpy(sysData.name, "Ref:");
	//  strcpy(tempChar, getFreqString(sysData.freq.ref.value, temp));
	//  insertThousands(tempChar);
	//  strcpy(sysData.value, tempChar);
	//  strcpy(sysData.unit, freqToQUnit(sysData.freq.ref.value).trimmed().toStdString().c_str());


	sysData.freq.freq = sysData.freq.offset;

	strcpy(sysData.name, "FREQ:");
	strcpy(tempChar, getFreqString(sysData.freq.freq, temp));
	insertThousands(tempChar);
	strcpy(sysData.value, tempChar);
	strcpy(sysData.unit, freqToQUnit(sysData.freq.freq).trimmed().toStdString().c_str());

//	if (isNeedCloseDemod)
//	{
//		savePowerOffState();
//	}

	return __SCPI_SUCCESS;
}

//设置频率参考状态
int tSysScpi::setFrequencyOfReferenceState(int value)
{
	bool tempValue = value == 1;
	double freqRef = 0;
	char temp[128] = { };
	char tempChar[128] = { };
	sysData.freq.ref.state = tempValue;

	if (tempValue)
	{
		sysData.freq.ref.state = true;
		freqRef = sysData.freq.ref.value;
	} else
	{
		sysData.freq.ref.state = false;
		freqRef = 0;
	}

	sysData.freq.freq = sysData.freq.multiplier * (sysData.freq.downLoad - freqRef) + sysData.freq.offset;

	strcpy(sysData.name, "FREQ:");
	strcpy(tempChar, getFreqString(sysData.freq.freq, temp));
	insertThousands(tempChar);
	strcpy(sysData.value, tempChar);
	strcpy(sysData.unit, freqToQUnit(sysData.freq.freq).trimmed().toStdString().c_str());

//	if (isNeedCloseDemod)
//	{
//		savePowerOffState();
//	}

	return __SCPI_SUCCESS;
}

//设置频率参考状态
int tSysScpi::setFrequencyOfReferenceState(QString value)
{
	bool isOk = false;
	bool tempValue = 0;

	tempValue = getBoolValue(value, &isOk);

	if (isOk)
	{
		return setFrequencyOfReferenceState(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置幅度 参考
int tSysScpi::setAmplitudeOfReference(void)
{
	QString cmdStr = "";
	char temp[128] = { };
	char tempChar[128] = { };

	if (sysData.ampl.ref.state == true)
	{
		//	  sysData.ampl.ref.value = getdBmValue(sysData.ampl.setValue) + sysData.ampl.ref.value;		//原本的处理方法
		if (sysData.ampl.setValue != 0)
		{
			sysData.ampl.ref.value = getdBmValue(sysData.ampl.setValue) + sysData.ampl.ref.value;
		}

	} else
	{
		//	  sysData.ampl.ref.value = getdBmValue(sysData.ampl.setValue);
		if (sysData.ampl.setValue != 0)
		{
			sysData.ampl.ref.value = getdBmValue(sysData.ampl.setValue);
		}
	}

	sysData.ampl.ref.state = true;

	//  sysData.ampl.ref.value = sysData.ampl.setValue;
	//  sysData.ampl.ref.value = sysData.ampl.downLoad;
	//  sysData.ampl.ref.value = sysData.ampl.setValue + sysData.ampl.ref.value;
	//  sysData.ampl.ref.value = getdBmValue(sysData.ampl.setValue) + sysData.ampl.ref.value;
	//  sysData.ampl.ampl = 0.00;
	sysData.ampl.ampl = sysData.ampl.offset;

	strcpy(sysData.name, "AMPL:");
	strcpy(tempChar, floatToStringDot2(sysData.ampl.ampl, temp));
	strcpy(sysData.value, tempChar);
	strcpy(sysData.unit, "dB");

	sysData.ampl.setValue = 0.00;
	//  switch (sysData.ampl.amplUnit)		//根据单位，将设置值设为0
	//  {
	//	case audBm: sysData.ampl.setValue = 0.00;    break;
	//	case audBuV: sysData.ampl.setValue = 107.00;    break;
	//	case aumV: sysData.ampl.setValue = 223.6068;    break;
	//	default: sysData.ampl.setValue = 0.00;    break;
	//  }
	//  sysData.ampl.ampl = sysData.ampl.offset;


//	if (isNeedCloseDemod)
//	{
//		savePowerOffState();
//	}

	return __SCPI_SUCCESS;
}

//设置幅度参考状态
int tSysScpi::setAmplitudeOfReferenceState(int value)
{
	bool tempValue = value == 1;
	double amplRef = 0;
	char temp[128] = { };
	char tempChar[128] = { };

	if ((sysData.ampl.ref.state == true && tempValue == true) || (sysData.ampl.ref.state == false && tempValue == false))
	{
		return __SCPI_SUCCESS;
	}
	sysData.ampl.ref.state = tempValue;

	if (sysData.ampl.ref.state)
	{
		sysData.ampl.ref.state = true;
		if (sysData.ampl.amplUnit == audBm)
		{
			sysData.ampl.setValue = sysData.ampl.setValue - sysData.ampl.ref.value;
			sysData.ampl.ampl = sysData.ampl.setValue + sysData.ampl.offset;
		} else
		{
			//    	sysData.ampl.ampl = sysData.ampl.setValue + sysData.ampl.offset;		//原本的处理方法
			if (sysData.ampl.ref.value == 0 && sysData.ampl.setValue != 0)
			{
				sysData.ampl.ampl = getdBmValue(sysData.ampl.setValue) + sysData.ampl.offset;
			} else
			{
				//    		sysData.ampl.ampl = sysData.ampl.setValue - getdBuVmVValue(sysData.ampl.ref.value) + sysData.ampl.offset;
				sysData.ampl.setValue = sysData.ampl.setValue - getdBuVmVValue(sysData.ampl.ref.value);
				sysData.ampl.ampl = sysData.ampl.setValue + sysData.ampl.offset;
			}

		}
		strcpy(sysData.name, "AMPL:");
		strcpy(tempChar, floatToStringDot2(sysData.ampl.ampl, temp));
		strcpy(sysData.value, tempChar);
		strcpy(sysData.unit, amplToQUnit(sysData.ampl.setValue).trimmed().toStdString().c_str());

	} else
	{
		if (sysData.ampl.amplUnit == audBm)
		{
			sysData.ampl.setValue = sysData.ampl.setValue + sysData.ampl.ref.value;
			sysData.ampl.ampl = sysData.ampl.setValue + sysData.ampl.offset;
		} else
		{
			//		sysData.ampl.ampl = sysData.ampl.setValue + getdBuVmVValue(sysData.ampl.ref.value) + sysData.ampl.offset;	//原本的处理方法

			if (sysData.ampl.ref.value == 0 && sysData.ampl.setValue != 0)
			{
				sysData.ampl.ampl = sysData.ampl.setValue + sysData.ampl.offset;
			} else
			{
				//			sysData.ampl.ampl = sysData.ampl.setValue + getdBuVmVValue(sysData.ampl.ref.value) + sysData.ampl.offset;
				sysData.ampl.setValue = sysData.ampl.setValue + getdBuVmVValue(sysData.ampl.ref.value);
				sysData.ampl.ampl = sysData.ampl.setValue + sysData.ampl.offset;
			}

		}
		sysData.ampl.ref.state = false;
		strcpy(sysData.name, "AMPL:");
		strcpy(tempChar, floatToStringDot2(sysData.ampl.ampl, temp));
		strcpy(sysData.value, tempChar);
		strcpy(sysData.unit, amplToQUnit(sysData.ampl.setValue).trimmed().toStdString().c_str());

	}

	//  sysData.ampl.ref.state = tempValue;

	//  sysData.ampl.ampl = (sysData.ampl.ampl - amplRef) + sysData.ampl.offset;
//	savePowerOffState();
	return __SCPI_SUCCESS;
}

//设置幅度参考状态
int tSysScpi::setAmplitudeOfReferenceState(QString value)
{
	bool isOk = false;
	bool tempValue = 0;

	tempValue = getBoolValue(value, &isOk);

	if (isOk)
	{
		return setAmplitudeOfReferenceState(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置参考时基状态
int tSysScpi::setRef10MState(int value)
{
	int tempvalue = value;
	QString cmdStr = "";
	sysData.freq.ref10MHz = ref10MHzDef(tempvalue);
	if (tempvalue == 0)
	{
		cmdStr = QString(":SYS:10M:REF 1");
	} else if (tempvalue == 1)
	{
		cmdStr = QString(":SYS:10M:REF 0");
	}
	if (sendSerialData(cmdStr) == __SCPI_FAILED)
	{
		return __SCPI_FAILED;
	}

//	savePowerOffState();
	return __SCPI_SUCCESS;
}
//设置参考时基状态
int tSysScpi::setRef10MState(QString value)
{
	if (value == "rmInternal")
	{
		return setRef10MState(rmInternal);
	} else if (value == "rmExternal")
	{
		return setRef10MState(rmExternal);
	}

	return __SCPI_FAILED;
}

//设置稳幅状态
int tSysScpi::setALCState(int value)
{
	bool tempvalue = value == 1;
	QString cmdStr = "";
	if (tempvalue)
	{
		sysData.ampl.alc.state = true;
		cmdStr = QString(":ALCFUNCTION 1");
	} else
	{
		sysData.ampl.alc.state = false;
		cmdStr = QString(":ALCFUNCTION 0");
	}
	if (sendSerialData(cmdStr) == __SCPI_FAILED)
	{
		return __SCPI_FAILED;
	}
//	savePowerOffState();
	return __SCPI_SUCCESS;
}
//设置稳幅状态
int tSysScpi::setALCState(QString value)
{
	bool isOK = false;
	bool tempValue;

	tempValue = getBoolValue(value, &isOK);
	if (isOK)
	{
		return setALCState(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置稳幅响应
int tSysScpi::setAmplResponse(int value)
{
	bool tempvalue = value == 1;
	if (tempvalue)
	{
		sysData.ampl.alc.response = true;
	} else
	{
		sysData.ampl.alc.response = false;
	}
//	savePowerOffState();
	return __SCPI_SUCCESS;
}
//设置稳幅响应
int tSysScpi::setAmplResponse(QString value)
{
	bool isOK = false;
	bool tempValue;

	tempValue = getBoolValue(value, &isOK);
	if (isOK)
	{
		return setAmplResponse(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置变频参考频率
int tSysScpi::setExtRefFrequency(double value)
{
	if (sysData.exfRef.type == erftInternal || sysData.freq.freq <= 300e6) //当设置频率小于300MHz或是内部变频源时，不能设置参考频率
	{
		return __SCPI_FAILED;
	}

	double tempValue = value;
	char temp[128] = { };
	char tempChar[128] = { };

	QString cmdStr = "";

	if (tempValue >= MAXEXTREFFREQ)
	{
		tempValue = MAXEXTREFFREQ;
	} else if (tempValue <= MINEXTREFFREQ)
	{
		tempValue = MINEXTREFFREQ;
	}

	sysData.exfRef.freq = tempValue;

	//get input paremwter
	strcpy(sysData.name, "ExtRf FREQ:");
	strcpy(tempChar, getFreqString(sysData.exfRef.freq, temp));
	insertThousands(tempChar);
	strcpy(sysData.value, tempChar);
	strcpy(sysData.unit, freqToQUnit(sysData.exfRef.freq).trimmed().toStdString().c_str());

	sprintf(tempChar, "%.0f", sysData.exfRef.freq);
	cmdStr = ":FREQ:CONV:FREQ " + QString(tempChar).trimmed();

	//发送
	if (sendSerialData(cmdStr) == __SCPI_FAILED)
	{
		return __SCPI_FAILED;
	}

//	if (isNeedCloseDemod)
//	{
//		savePowerOffState();
//	}

	return __SCPI_SUCCESS;

}

//设置变频参考频率
int tSysScpi::setExtRefFrequency(QString value)
{
	bool isOk = false;
	double tempValue = 0;

	tempValue = getFreqValue(value, &isOk);

	if (isOk)
	{
		return setExtRefFrequency(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//旋钮设置变频参考频率
int tSysScpi::setExtRefFrequencyWithRpg(int value)
{
	int tempValue = value;
	return setExtRefFrequency(setValueByRPG(sysData.exfRef.freq, tempValue, 0));
}

//设置变频参考类型
int tSysScpi::setExtRefType(int value)
{
	int tempValue = value;
	QString cmdStr = "";
	QString parameter = "";

	if (value > 1 || value < 0)
	{
		return __SCPI_FAILED;
	}

	sysData.exfRef.type = extRefFrequencyTypeDef(value);
	//变频切换 1内部 0外部
	if (sysData.exfRef.type == erftInternal)
	{
		parameter = "0"; //内部
	} else if (sysData.exfRef.type == erftExternal)
	{
		parameter = "1"; //外部
		//	setExtRefFrequency(sysData.exfRef.freq);
	}
	//命令
	cmdStr = ":FREQ:CONV " + parameter;

	//发送
	if (sendSerialData(cmdStr) == __SCPI_FAILED)
	{
		return __SCPI_FAILED;
	}

//	if (isNeedCloseDemod)
//	{
//		savePowerOffState();
//	}

	return __SCPI_SUCCESS;
}

//设置变频参考类型
int tSysScpi::setExtRefType(QString value)
{
	bool isOk = false;

	QString tempValue = value.toUpper();

	if (tempValue == "INT" || tempValue == "INTERNAL")
	{
		return setExtRefType(erftInternal);
	} else if (tempValue == "EXT" || tempValue == "EXTERNAL")
	{
		return setExtRefType(erftExternal);
	}

	return __SCPI_FAILED;
}

//设置LF状态
int tSysScpi::setLFState(int value)
{
	QString cmdStr = "";
	bool tempValue = value == 1;

	//	//关闭解调状态
	//	closeDemodState();
	//	//关闭扫描显示
	//	closeSweepDisplay();

	sysData.lf.state = tempValue;
	if (tempValue)
	{
		cmdStr = ":LFOUT:SWITCH 1";
	} else
	{
		cmdStr = ":LFOUT:SWITCH 0";
	}
	if (sendSerialData(cmdStr) == __SCPI_FAILED)
	{
		return __SCPI_FAILED;
	}
	return __SCPI_SUCCESS;
}

int tSysScpi::setLFState(QString value)
{
	bool isOk = false;
	bool tempValue = 0;

	tempValue = getBoolValue(value, &isOk);

	if (isOk)
	{
		return setLFState(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置LF类型
int tSysScpi::setLfType(int value)
{
	int tempValue = value;
	QString cmdStr = "";
	QString parameter = "";

	if (value < 0)
	{
		return __SCPI_FAILED;
	} else if (value > 3) //去除了辛格函数波
	{
		return __SCPI_FAILED;
	}

	sysData.lf.type = lfOutputTypeDef(value);

	switch (sysData.lf.type)
	{
		case lotSine:
			parameter = "0";
			break;
		case lotSquare:
			parameter = "1";
			if (sysData.lf.freq > 20000)
			{
				sysData.lf.freq = 20000;
				cmdStr = ":LFOUT:FREQ " + doubleToString(sysData.lf.freq).trimmed();
				sendSerialData(cmdStr);
			}
			break;
		case lotTriangle:
			parameter = "2";
			break;
		case lotSawtooth:
			parameter = "3";
			break;
			//case lotSinc:   	parameter = "4";  break;
		default:
			break;
	}

	//命令
	cmdStr = ":LFOUT:TYPE " + parameter;

	//发送
	if (sendSerialData(cmdStr) == __SCPI_FAILED)
	{
		return __SCPI_FAILED;
	}

//	if (isNeedCloseDemod)
//	{
//		savePowerOffState();
//	}

	return __SCPI_SUCCESS;
}

//设置LF类型
int tSysScpi::setLfType(QString value)
{
	bool isOk = false;
	int tempValue = 0;

	tempValue = getIntValue(value, &isOk);

	if (isOk)
	{
		return setLfType(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置LF频率
int tSysScpi::setLfFreq(double value)
{
	double tempValue = value;
	char temp[128] = { };
	char tempChar[128] = { };

	QString cmdStr = "";

	switch (sysData.lf.type)
	{
		case lotSine: //正弦波
			if (tempValue < MINLFFREQ)
				tempValue = MINLFFREQ;
			else if (tempValue > MAXLFFREQSINE)
				tempValue = MAXLFFREQSINE;
			break;

		case lotSquare:
			if (tempValue < MINLFFREQ)
				tempValue = MINLFFREQ;
			else if (tempValue > MAXLFFREQSQUARE)
				tempValue = MAXLFFREQSQUARE;
			break;

		case lotTriangle:
			if (tempValue < MINLFFREQ)
				tempValue = MINLFFREQ;
			else if (tempValue > MAXLFFREQTRIANGLE)
				tempValue = MAXLFFREQTRIANGLE;
			break;

		case lotSawtooth:
			if (tempValue < MINLFFREQ)
				tempValue = MINLFFREQ;
			else if (tempValue > MAXLFFREQSAWTOOTH)
				tempValue = MAXLFFREQSAWTOOTH;
			break;
			/*
			 case lotSinc:  //辛格函数
			 if(tempValue< MINLFFREQ)
			 tempValue = MINLFFREQ;
			 else if(tempValue > MAXLFFREQSINC)
			 tempValue = MAXLFFREQSINC;
			 */
		default:
			break;
	}

	sysData.lf.freq = tempValue;

	//get input paremwter
	strcpy(sysData.name, "LF Freq:");
	strcpy(tempChar, getFreqStringInLF(sysData.lf.freq, temp));
	insertThousands(tempChar);
	strcpy(sysData.value, tempChar);
	strcpy(sysData.unit, freqToQUnit(sysData.lf.freq).trimmed().toStdString().c_str());

	cmdStr = ":LFOUT:FREQ " + doubleToString(sysData.lf.freq).trimmed();

	sendSerialData(cmdStr);

//	if (isNeedCloseDemod)
//	{
//		savePowerOffState();
//	}

	return __SCPI_SUCCESS;
}

//设置LF
//设置LF频率
int tSysScpi::setLfFreq(QString value)
{
	bool isOk = false;
	double tempValue = 0;

	tempValue = getFreqValue(value, &isOk);

	if (isOk)
	{
		return setLfFreq(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

int tSysScpi::setLfFreqWithRpg(int value)
{
	return setLfFreq(setValueByRPG(sysData.lf.freq, value, 2));
}

//设置LF 幅度
int tSysScpi::setLfAmpl(double value)
{
	double tempValue = value;
	QString cmdStr = "";
	char temp[128] = { };
	char tempChar[128] = { };

	if (tempValue < MINLFAMPL)
	{
		tempValue = MINLFAMPL;
	} else if (tempValue > MAXLFAMPL)
	{
		tempValue = MAXLFAMPL;
	}

	sysData.lf.ampl = tempValue;

	strcpy(sysData.name, "LF Ampl:");
	if (tempValue >= 1000)
	{
		tempValue /= 1000;
		sprintf(sysData.value, "%16.3f", tempValue);
	} else
	{
		sprintf(sysData.value, "%16.0f", tempValue);
	}
	strcpy(sysData.unit, vppToQUnit(sysData.lf.ampl).trimmed().toStdString().c_str());

	//  if(sysData.lf.type == lotSquare)
	//  {
	//	  sprintf(tempChar, "%.0f", sysData.lf.ampl);			//方波的时候是正确的，故不需要做处理
	//  }else
	//  {
	//	  sprintf(tempChar, "%.0f", (sysData.lf.ampl / 2));		//由于使用示波器测试时，测得的结果是下发的两倍，故按田要求，将其除以2再下发
	//  }
	sprintf(tempChar, "%.0f", sysData.lf.ampl);

	cmdStr = ":LFOUT:AMPL " + QString(tempChar).trimmed();
	sendSerialData(cmdStr);

//	if (isNeedCloseDemod)
//	{
//		savePowerOffState();
//	}

	return __SCPI_SUCCESS;
}

//设置LF 幅度
int tSysScpi::setLfAmpl(QString value)
{
	bool isOk = false;
	double tempValue = 0;

	tempValue = getVoltageValue(value, &isOk);

	if (isOk)
	{
		return setLfAmpl(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

int tSysScpi::setLfAmplWithRpg(int value)
{
	return setLfAmpl(setValueByRPG(sysData.lf.ampl, value, 0));
}

//设置DRG状态
int tSysScpi::setDrgState(int value)
{
	QString cmdStr = "";
	bool tempValue = value == 1;

	//关闭解调状态
	closeDemodState();

	//关闭扫描显示
	closeSweepDisplay();

	sysData.drg.state = tempValue;

	if (tempValue)
	{
		cmdStr = ":CHIRP:DRG:STAT 1";
	} else
	{
		cmdStr = ":CHIRP:DRG:STAT 0";
	}

	sendSerialData(cmdStr);

//	if (isNeedCloseDemod)
//	{
//		savePowerOffState();
//	}

	return __SCPI_SUCCESS;
}
int tSysScpi::setDrgState(QString value)
{
	bool isOk = false;
	bool tempValue = 0;

	tempValue = getBoolValue(value, &isOk);

	if (isOk)
	{
		return setDrgState(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}
//设置DRG扫描上限
int tSysScpi::setDrgUplimit(double value)
{
	double tempValue = value;
	char temp[128] = { };
	char tempChar[128] = { };
	char tempdown[64] = { };
	QString cmdStr = "";

	if (tempValue < MINFREQ)
	{
		tempValue = MINFREQ;
		sysData.drg.downlimit = MINFREQ;
	} else if (tempValue > MAXFREQ)
	{
		tempValue = MAXFREQ;
	}
	sysData.drg.uplimit = tempValue;

	if (sysData.drg.uplimit >= sysData.drg.downlimit)
	{
		if ((sysData.drg.uplimit - sysData.drg.downlimit) > MXADRGUPDOWNLIMIT)
		{
			sysData.drg.downlimit = sysData.drg.uplimit - MXADRGUPDOWNLIMIT;
		}
	} else
	{
		if (sysData.drg.uplimit > MXADRGUPDOWNLIMIT)
			sysData.drg.downlimit = sysData.drg.uplimit - MXADRGUPDOWNLIMIT;
		else
			sysData.drg.downlimit = MINFREQ;
	}
	//显示及发送
	strcpy(sysData.name, "Up Limit:");
	strcpy(tempChar, getFreqString(sysData.drg.uplimit, temp));
	insertThousands(tempChar);
	strcpy(sysData.value, tempChar);
	strcpy(sysData.unit, freqToQUnit(sysData.drg.uplimit).trimmed().toStdString().c_str());
	//需要发送上限和下限值
	sprintf(tempdown, "%.0f", sysData.drg.uplimit);
	cmdStr = ":DRG:STOP:FREQ " + QString(tempdown).trimmed();
	if (sendSerialData(cmdStr) == __SCPI_FAILED)
	{
		return __SCPI_FAILED;
	}
	sprintf(tempdown, "%.0f", sysData.drg.downlimit);
	cmdStr = ":DRG:START:FREQ " + QString(tempdown).trimmed();
	if (sendSerialData(cmdStr) == __SCPI_FAILED)
	{
		return __SCPI_FAILED;
	}

//	if (isNeedCloseDemod)
//	{
//		savePowerOffState();
//	}

	return __SCPI_SUCCESS;
}

int tSysScpi::setDrgUplimit(QString value)
{
	bool isOk = false;
	double tempValue = 0;

	tempValue = getFreqValue(value, &isOk);

	if (isOk)
	{
		return setDrgUplimit(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

int tSysScpi::setDrgUplimitWithRpg(int value)
{
	return setDrgUplimit(setValueByRPG(sysData.drg.uplimit, value, 0));
}

//设置DRG扫描下限
int tSysScpi::setDrgDownlimit(double value)
{
	double tempValue = value;
	char temp[128] = { };
	char tempChar[128] = { };
	char tempdown[64] = { };
	QString cmdStr = "";

	if (tempValue < MINFREQ)
	{
		tempValue = MINFREQ;
	} else if (tempValue > MAXFREQ)
	{
		sysData.drg.uplimit = MAXFREQ;
		tempValue = MAXFREQ;
		//		return __SCPI_SUCCESS;
	}
	sysData.drg.downlimit = tempValue;
	if (sysData.drg.uplimit >= sysData.drg.downlimit)
	{
		if ((sysData.drg.uplimit - sysData.drg.downlimit) > MXADRGUPDOWNLIMIT)
		{
			sysData.drg.uplimit = sysData.drg.downlimit + MXADRGUPDOWNLIMIT;
		}
	} else
	{
		if (sysData.drg.downlimit + MXADRGUPDOWNLIMIT > MAXFREQ)
			sysData.drg.uplimit = MAXFREQ;
		else
			sysData.drg.uplimit = sysData.drg.downlimit + MXADRGUPDOWNLIMIT;
	}
	//显示及发送
	strcpy(sysData.name, "Down Limit:");
	strcpy(tempChar, getFreqString(sysData.drg.downlimit, temp));
	insertThousands(tempChar);
	strcpy(sysData.value, tempChar);
	strcpy(sysData.unit, freqToQUnit(sysData.drg.downlimit).trimmed().toStdString().c_str());
	//需要发送上限和下限值
	sprintf(tempdown, "%.0f", sysData.drg.uplimit);
	cmdStr = ":DRG:STOP:FREQ " + QString(tempdown).trimmed();
	if (sendSerialData(cmdStr) == __SCPI_FAILED)
	{
		return __SCPI_FAILED;
	}
	sprintf(tempdown, "%.0f", sysData.drg.downlimit);
	cmdStr = ":DRG:START:FREQ " + QString(tempdown).trimmed();
	if (sendSerialData(cmdStr) == __SCPI_FAILED)
	{
		return __SCPI_FAILED;
	}

//	if (isNeedCloseDemod)
//	{
//		savePowerOffState();
//	}

	return __SCPI_SUCCESS;
}

int tSysScpi::setDrgDownlimit(QString value)
{
	bool isOk = false;
	double tempValue = 0;

	tempValue = getFreqValue(value, &isOk);

	if (isOk)
	{
		return setDrgDownlimit(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

int tSysScpi::setDrgDownlimitWithRpg(int value)
{
	return setDrgDownlimit(setValueByRPG(sysData.drg.downlimit, value, 0));
}

//设置DRG扫描点数
int tSysScpi::setDrgPoints(double value)
{
	double tempValue = value;
	QString cmdStr = "";
	char temp[128] = { };
	char tempChar[128] = { };

	if(sysData.drg.rate * 1e6 > 260000)  //当扫描速率大于260us时
	{
		switch(sysData.drg.workmode)
		{
			case ddnormal:  //正常：最多2048个点
				if (tempValue < 1)
						tempValue = 1;
					else if (tempValue > 2048)
						tempValue = 2048;
				break;
			case ddnegative:  //正斜率和负斜率：最多4096个点
			case ddpositive:
				if (tempValue < 1)
						tempValue = 1;
					else if (tempValue > 4096)
						tempValue = 4096;
				break;
		}

	}else
	{
		if (tempValue < 1)
				tempValue = 1;
			else if (tempValue > 65535)
				tempValue = 65535;
	}


	sysData.drg.points = tempValue;
	sprintf(tempChar, "%d", sysData.drg.points);
	cmdStr = ":DRG:STEP:POINT " + QString(tempChar).trimmed();
	if (sendSerialData(cmdStr) == __SCPI_FAILED)
	{
		return __SCPI_FAILED;
	}

//	if (isNeedCloseDemod)
//	{
//		savePowerOffState();
//	}

	return __SCPI_SUCCESS;
}

int tSysScpi::setDrgPoints(QString value)
{
	bool isOk = false;
	double tempValue = 0;

	tempValue = getDoubleValue(value, &isOk);

	if (isOk)
	{
		return setDrgPoints(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

int tSysScpi::setDrgPointsWithRpg(int value)
{
	int tempValue = value;
	return setStepPoints(setValueByRPG(sysData.drg.points, tempValue, 0));
}

//设置DRG扫描速率
int tSysScpi::setDrgRate(double value)
{
	unsigned long tempValue = ((unsigned long) (value * 1e6));
	QString cmdStr = "";

	char temp[128] = { };
	char tempChar[128] = { };
	if (tempValue < 20)
		tempValue = 20;
	else if (tempValue > 50e6)//最大为50ms
		tempValue = 50e6;

	if ((tempValue % 20) >= 10) //四舍五入
	{
		sysData.drg.rate = (((tempValue / 20) + 1) * 20) * 1e-6;
	} else
	{
		sysData.drg.rate = ((tempValue / 20) * 20) * 1e-6;
	}

	if(tempValue> 260e3) //大于260us时，需要改变扫描点数
	{
		switch(sysData.drg.workmode)
		{
			case ddnormal:
				if(sysData.drg.points > 2048)
				{
					sysData.drg.points = 2048;
				}
				break;
			case ddpositive:
			case ddnegative:
				if(sysData.drg.points > 4096)
				{
					sysData.drg.points = 4096;
				}
				break;
		}
	}

	//get paramter
	strcpy(sysData.name, "Rate:");
	strcpy(temp, doubleToQValue(sysData.drg.rate * 1e6).toStdString().c_str());
	strcpy(sysData.value, temp);
	strcpy(sysData.unit, timeToQUnit(sysData.drg.rate).trimmed().toStdString().c_str());

	sprintf(tempChar, "%.0f", (sysData.drg.rate * 1e6));
	cmdStr = ":DRG:STEP:TIME " + QString(tempChar).trimmed();

	if (sendSerialData(cmdStr) == __SCPI_FAILED)
	{
		return __SCPI_FAILED;
	}

//	if (isNeedCloseDemod)
//	{
//		savePowerOffState();
//	}

	return __SCPI_SUCCESS;
}

int tSysScpi::setDrgRate(QString value)
{
	bool isOk = false;
	double tempValue = 0;

	tempValue = getTimeValue(value, &isOk) * 1e-6;
	if (isOk)
	{
		return setDrgRate(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

int tSysScpi::setDrgRateWithRpg(int value)
{
	int tempValue = value;

	//  return setDrgRate(setValueByRPG(sysData.drg.rate, tempValue, 7));
}

//设置DRG工作模式
int tSysScpi::setDrgWorkmode(int value)
{
	int tempValue = value;
	QString cmdStr = "";

	//  if(tempValue < 0)
	//  {
	//	tempValue = 0;
	//  }else if(tempValue > 2)
	//  {
	//	tempValue = 1;
	//  }

	if (tempValue < 0 || tempValue > 2)
	{
		return __SCPI_FAILED;
	}

	sysData.drg.workmode = drgDireTypeDef(tempValue);

	switch (sysData.drg.workmode)
	{
		case ddnormal:
			cmdStr = ":DRG:MODE:CTL 1";
			break;
		case ddpositive:
			cmdStr = ":DRG:MODE:CTL 2";
			break;
		case ddnegative:
			cmdStr = ":DRG:MODE:CTL 3";
			break;
		default:
			break;
	}

	if (sendSerialData(cmdStr) == __SCPI_FAILED)
	{
		return __SCPI_FAILED;
	}
//	if (isNeedCloseDemod)
//	{
//		savePowerOffState();
//	}

	return __SCPI_SUCCESS;
}

int tSysScpi::setDrgWorkmode(QString value)
{
	if (value == "1")
	{
		return setDrgWorkmode(ddnormal);
	} else if (value == "2")
	{
		return setDrgWorkmode(ddpositive);
	} else if (value == "3")
	{
		return setDrgWorkmode(ddnegative);
	}
	return __SCPI_FAILED;
}

//设置SCPI
int tSysScpi::setScpiState(int value)
{
	sysData.system.isScpiFresh = (value == 1);
//	if (isNeedCloseDemod)
//	{
//		savePowerOffState();
//	}
	return __SCPI_SUCCESS;
}

//设置SCPI
int tSysScpi::setScpiState(QString value)
{
	bool isOk = false;
	bool tempValue = getBoolValue(value, &isOk);

	if (isOk)
	{
		return setScpiState(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}

}

#if 0
//设置扫描状态
int tSysScpi::setSweepstate(int value)
{
	QString cmdStr = "";
	bool tempValue = value == 1;

	sysData.sweep.state = tempValue;

	if(sysData.sweep.state == false)
	{
		sysData.sweep.freqstate = false;
		sysData.sweep.amplstate = false;
	}
	if(sysData.sweep.freqstate && sysData.sweep.amplstate)
	{
		sysData.sweep.state = true;
	}

	if(sysData.sweep.state)
	{
		cmdStr = ":SWEEP:STAT 1";
	} else
	{
		cmdStr = ":SWEEP:STAT 0";
	}
	if(sendSerialData(cmdStr) == __SCPI_FAILED)
	{
		return __SCPI_FAILED;
	}
	if(isNeedCloseDemod)
	{
		savePowerOffState();
	}
	return __SCPI_SUCCESS;
}
#endif

//设置扫描模式
int tSysScpi::setSweepMode(int value)
{
	int tempValue = value;
	QString cmdStr = "";
	static bool freqMod = false;
	static bool amplMod = false;

	if (tempValue < 0)
	{
		tempValue = 0;
	} else if (tempValue > 3)
	{
		tempValue = 3;
	}

	sysData.sweep.mode = sweepModeDef(tempValue);

	switch (sysData.sweep.mode)
	{
		case smOff:
			cmdStr = ":SWEEP:STAT 0";
			if (sendSerialData(cmdStr) == __SCPI_FAILED)
				return __SCPI_FAILED;
			break;
		case smFreq:
			cmdStr = ":SWEEP:MODE 1";
			if (sendSerialData(cmdStr) == __SCPI_FAILED)
				return __SCPI_FAILED;
			if (sysData.sweep.repeat == srConutinuos)
			{
				cmdStr = ":SWEEP:STAT 1";
				if (sendSerialData(cmdStr) == __SCPI_FAILED)
					return __SCPI_FAILED;
			}
			break;

		case smAmpl:
			cmdStr = ":SWEEP:MODE 2";
			if (sendSerialData(cmdStr) == __SCPI_FAILED)
				return __SCPI_FAILED;
			if (sysData.sweep.repeat == srConutinuos)
			{
				cmdStr = ":SWEEP:STAT 1";
				if (sendSerialData(cmdStr) == __SCPI_FAILED)
					return __SCPI_FAILED;
			}
			break;

		case smFreqAmpl:
			cmdStr = ":SWEEP:MODE 3";
			if (sendSerialData(cmdStr) == __SCPI_FAILED)
				return __SCPI_FAILED;
			if (sysData.sweep.repeat == srConutinuos)
			{
				cmdStr = ":SWEEP:STAT 1";
				if (sendSerialData(cmdStr) == __SCPI_FAILED)
					return __SCPI_FAILED;
			}
			break;

		default:
			break;
	}

//	if (isNeedCloseDemod)
//	{
//		savePowerOffState();
//	}

	return __SCPI_SUCCESS;
}

//设置扫描模式
int tSysScpi::setSweepMode(QString value)
{
	bool isOk = false;
	int tempValue = 0;

	//  tempValue = getIntValue(value, &isOk);
	if (value == "0")
	{
		return setSweepMode(smOff);
	} else if (value == "1")
	{
		return setSweepMode(smFreq);
	} else if (value == "2")
	{
		return setSweepMode(smAmpl);
	} else if (value == "3")
	{
		return setSweepMode(smFreqAmpl);
	}

	return __SCPI_FAILED;
}

//设置扫描类型
int tSysScpi::setSweepType(int value)
{
	QString cmdStr = "";
	int tempValue = value;
	if (tempValue < 0)
	{
		tempValue = 0;
	} else if (tempValue > 1)
	{
		tempValue = 1;
	}
	sysData.sweep.type = sweepTypeDef(tempValue);

	if (tempValue == 0)
	{
		cmdStr = ":LISTSWEEP:TYPE 0";
	} else if (tempValue == 1)
	{
		cmdStr = ":LISTSWEEP:TYPE 1";
	}
	if (sendSerialData(cmdStr) == __SCPI_FAILED)
	{
		return __SCPI_FAILED;
	}
//	if (isNeedCloseDemod)
//	{
//		savePowerOffState();
//	}

	return __SCPI_SUCCESS;
}
//设置扫描类型
int tSysScpi::setSweepType(QString value)
{
	QString tempValue = value.trimmed().toUpper();
	if (tempValue == "STSTEP")
	{
		return setSweepType(stStep);
	} else if (tempValue == "STLIST")
	{
		return setSweepType(stList);
	}

	return __SCPI_SUCCESS;
}

//设置重复扫描类型
int tSysScpi::setSweepRepeat(int value)
{
	int tempValue = value;
	QString cmdStr = "";

	if (tempValue < 0)
	{
		tempValue = 0;
	} else if (tempValue > 1)
	{
		tempValue = 1;
	}

	sysData.sweep.repeat = sweepRepeatDef(tempValue);

	switch (sysData.sweep.repeat)
	{
		case srSingle:
			cmdStr = ":SWEEP:CONTIN 0";
			sendSerialData(cmdStr);
			break;
		case srConutinuos:
			cmdStr = ":SWEEP:CONTIN 1";
			sendSerialData(cmdStr);
			cmdStr = ":SWEEP:STAT 1";
			sendSerialData(cmdStr);
			break;
		default:
			break;
	}

//	if (isNeedCloseDemod)
//	{
//		savePowerOffState();
//	}

	return __SCPI_SUCCESS;
}

int tSysScpi::setSweepRepeat(QString value)
{
	bool isOk = false;
	int tempValue = 0;

	if (value == "SING")
	{
		return setSweepRepeat(srSingle);
	} else if (value == "CONT")
	{
		return setSweepRepeat(srConutinuos);
	}
	return __SCPI_FAILED;
}

//设置扫描方向
int tSysScpi::setSweepDirection(int value)
{
	int tempValue = value;
	QString cmdStr = "";

	if (tempValue < 0)
	{
		tempValue = 0;
	} else if (tempValue > 1)
	{
		tempValue = 1;
	}

	sysData.sweep.direction = sweepDirectionDef(tempValue);
	if (sysData.sweep.direction == sdUp) //绘制进度条使用
	{
		sysData.indexPoints = 0;
	} else
	{
		sysData.indexPoints = sysData.sweep.step.point;
	}

	switch (sysData.sweep.direction)
	{
		case sdUp:
			cmdStr = ":SWEEP:DIR 0";
			break;
		case sdDown:
			cmdStr = ":SWEEP:DIR 1";
			break;
		default:
			break;
	}

	if (sendSerialData(cmdStr) == __SCPI_FAILED)
	{
		return __SCPI_FAILED;
	}
//	if (isNeedCloseDemod)
//	{
//		savePowerOffState();
//	}

	return __SCPI_SUCCESS;
}

//设置扫描方向
int tSysScpi::setSweepDirection(QString value)
{
	if (value == "0")
	{
		return setSweepDirection(sdUp);
	} else if (value == "1")
	{
		return setSweepDirection(sdDown);
	}
	return __SCPI_FAILED;

}

//设置扫描Retrace
int tSysScpi::setSweepRetrace(int value)
{
	int tempValue = value;
	QString cmdStr = "";

	if (tempValue < 0)
	{
		tempValue = 0;
	} else if (tempValue > 1)
	{
		tempValue = 1;
	}

	sysData.sweep.retrace = sweepRetraceDef(tempValue);

	switch (sysData.sweep.retrace)
	{
		case srOn:
			cmdStr = ":SWEEP:RETR 1";

			break;
		case srOff:
			cmdStr = ":SWEEP:RETR 0";
			sendSerialData(cmdStr);
			break;
		default:
			break;
	}
	if (sendSerialData(cmdStr) == __SCPI_FAILED)
	{
		return __SCPI_FAILED;
	}

//	if (isNeedCloseDemod)
//	{
//		savePowerOffState();
//	}

	return __SCPI_SUCCESS;
}

//设置扫描Retrace
int tSysScpi::setSweepRetrace(QString value)
{
	if (value == "0")
	{
		return setSweepRetrace(srOff);
	} else if (value == "1")
	{
		return setSweepRetrace(srOn);
	}
	return __SCPI_FAILED;
}

//设置扫描触发
int tSysScpi::setSweepTrig(int value)
{
	int tempValue = value;
	QString cmdStr = "";

	if (tempValue < 0)
	{
		tempValue = 0;
	} else if (tempValue > 1)
	{
		tempValue = 1;
	}

	sysData.sweep.trig = sweepTriggerDef(tempValue);

	switch (sysData.sweep.trig)
	{
		case stAuto:
			//	  cmdStr = ":LIST:RETRace 1";
			cmdStr = ":INIT:TRIG 0";
			//	  sendSerialData(cmdStr);
			break;
		case stExt:
			//	  cmdStr = ":LIST:RETRace 0";
			cmdStr = ":INIT:TRIG 1";
			//	  sendSerialData(cmdStr);
			break;
		default:
			break;
	}

	if (sendSerialData(cmdStr) == __SCPI_FAILED)
	{
		return __SCPI_FAILED;
	}

//	if (isNeedCloseDemod)
//	{
//		savePowerOffState();
//	}

	return __SCPI_SUCCESS;
}

//设置扫描Retrace
int tSysScpi::setSweepTrig(QString value)
{
	if (value == "0")
	{
		return setSweepTrig(stAuto);
	} else if (value == "1")
	{
		return setSweepTrig(stExt);
	}
	return __SCPI_FAILED;
}

//设置扫描触发


//设置起始频率值
int tSysScpi::setStepStartFrequency(double value)
{
	double tempValue = value;
	QString cmdStr = "";
	double freqRefValue = 0;
	char temp[128] = { };
	char tempChar[128] = { };
	char tempdown[64] = { };
	if (sysData.freq.ref.state)
	{
		freqRefValue = sysData.freq.ref.value;
	} else
	{
		freqRefValue = 0;
	}

	double minValue = MINFREQ + sysData.freq.offset - freqRefValue;
	double maxValue = MAXFREQ + sysData.freq.offset - freqRefValue;

	if (tempValue < minValue)
		tempValue = minValue;
	else if (tempValue > maxValue)
		tempValue = maxValue;

	sysData.sweep.step.freqStart = tempValue;

	//get input paramter
	strcpy(sysData.name, "Start Freq:");
	strcpy(tempChar, getFreqString(sysData.sweep.step.freqStart, temp));
	insertThousands(tempChar);
	strcpy(sysData.value, tempChar);
	strcpy(sysData.unit, freqToQUnit(sysData.sweep.step.freqStart).trimmed().toStdString().c_str());

	//  cmdStr = ":FREQ:STAR " + QString(doubleToString(sysData.sweep.step.freqStart)).trimmed();
	sprintf(tempdown, "%.0f", sysData.sweep.step.freqStart);
	cmdStr = ":FREQ:STAR " + QString(tempdown).trimmed();
	sendSerialData(cmdStr);

//	if (isNeedCloseDemod)
//	{
//		savePowerOffState();
//	}

	return __SCPI_SUCCESS;
}

//设置起始频率值
int tSysScpi::setStepStartFrequency(QString value)
{
	bool isOk = false;
	double tempValue = 0;

	//  tempValue = getDoubleValue(value, &isOk);
	tempValue = getFreqValue(value, &isOk);

	if (isOk)
	{
		return setStepStartFrequency(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//脉冲旋钮设置起始频率值
int tSysScpi::setStepStartFrequencyWithRpg(int value)
{
	int tempValue = value;

	return setStepStartFrequency(setValueByRPG(sysData.sweep.step.freqStart, tempValue, 0));
}

//设置终止频率值
int tSysScpi::setStepStopFrequency(double value)
{
	double tempValue = value;
	QString cmdStr = "";
	double freqRefValue = 0;
	char temp[128] = { };
	char tempChar[128] = { };
	char tempdown[64] = { };

	if (sysData.freq.ref.state)
	{
		freqRefValue = sysData.freq.ref.value;
	} else
	{
		freqRefValue = 0;
	}

	double minValue = MINFREQ + sysData.freq.offset - freqRefValue;
	double maxValue = MAXFREQ + sysData.freq.offset - freqRefValue;

	if (tempValue < minValue)
		tempValue = minValue;
	else if (tempValue > maxValue)
		tempValue = maxValue;

	sysData.sweep.step.freqStop = tempValue;
	//get input paremwter
	strcpy(sysData.name, "Stop Freq:");
	strcpy(tempChar, getFreqString(sysData.sweep.step.freqStop, temp));
	insertThousands(tempChar);
	strcpy(sysData.value, tempChar);
	strcpy(sysData.unit, freqToQUnit(sysData.sweep.step.freqStop).trimmed().toStdString().c_str());

	//  cmdStr = ":FREQ:STOP " + QString(doubleToString(sysData.sweep.step.freqStop)).trimmed();
	sprintf(tempdown, "%.0f", sysData.sweep.step.freqStop);
	cmdStr = ":FREQ:STOP " + QString(tempdown).trimmed();
	sendSerialData(cmdStr);

//	if (isNeedCloseDemod)
//	{
//		savePowerOffState();
//	}

	return __SCPI_SUCCESS;
}

//设置终止频率值
int tSysScpi::setStepStopFrequency(QString value)
{
	bool isOk = false;
	double tempValue = 0;

	//  tempValue = getDoubleValue(value, &isOk);
	tempValue = getFreqValue(value, &isOk);

	if (isOk)
	{
		return setStepStopFrequency(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//脉冲旋钮设置终止频率值
int tSysScpi::setStepStopFrequencyWithRpg(int value)
{
	int tempValue = value;

	return setStepStopFrequency(setValueByRPG(sysData.sweep.step.freqStop, tempValue, 0));
}

//设置起始幅度值
int tSysScpi::setStepStartAmplitude(double value)
{
	double tempValue = value;
	QString cmdStr = "";
	double amplRefValue = 0;
	char temp[128] = { };
	char tempChar[128] = { };

	if (sysData.ampl.ref.state)
	{
		amplRefValue = sysData.ampl.ref.value;
	} else
	{
		amplRefValue = 0;
	}

	double minValue = MINAMPL + sysData.ampl.offset - amplRefValue - sysData.ampl.error.value;
	double maxValue = MAXAMPL + sysData.ampl.offset - amplRefValue - sysData.ampl.error.value;

	if (tempValue < minValue)
		tempValue = minValue;
	else if (tempValue > maxValue)
		tempValue = maxValue;

	sysData.sweep.step.amplStart = tempValue;

	//
	strcpy(sysData.name, "Start Ampl:");
	strcpy(sysData.value, floatToStringDot2(sysData.sweep.step.amplStart, temp));
	strcpy(sysData.unit, amplToQUnit(sysData.sweep.step.amplStart).trimmed().toStdString().c_str());

	cmdStr = ":POW:STAR " + QString(doubleToString(sysData.sweep.step.amplStart)).trimmed();

	sendSerialData(cmdStr);

//	if (isNeedCloseDemod)
//	{
//		savePowerOffState();
//	}

	return __SCPI_SUCCESS;
}

//设置起始幅度值
int tSysScpi::setStepStartAmplitude(QString value)
{
	bool isOk = false;
	double tempValue = 0;

	tempValue = getDoubleValue(value, &isOk);

	if (isOk)
	{
		return setStepStartAmplitude(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//脉冲旋钮设置起始幅度值
int tSysScpi::setStepStartAmplitudeWithRpg(int value)
{
	int tempValue = value;

	return setStepStartAmplitude(setValueByRPG(sysData.sweep.step.amplStart, tempValue, 2));
}

//设置终止幅度值
int tSysScpi::setStepStopAmplitude(double value)
{
	double tempValue = value;
	QString cmdStr = "";
	double amplRefValue = 0;
	char temp[128] = { };
	char tempChar[128] = { };

	if (sysData.ampl.ref.state)
	{
		amplRefValue = sysData.ampl.ref.value;
	} else
	{
		amplRefValue = 0;
	}

	double minValue = MINAMPL + sysData.ampl.offset - amplRefValue - sysData.ampl.error.value;
	double maxValue = MAXAMPL + sysData.ampl.offset - amplRefValue - sysData.ampl.error.value;

	if (tempValue < minValue)
		tempValue = minValue;
	else if (tempValue > maxValue)
		tempValue = maxValue;

	sysData.sweep.step.amplStop = tempValue;

	//
	strcpy(sysData.name, "Stop Ampl:");
	strcpy(sysData.value, floatToStringDot2(sysData.sweep.step.amplStop, temp));
	strcpy(sysData.unit, amplToQUnit(sysData.sweep.step.amplStop).trimmed().toStdString().c_str());

	cmdStr = ":POW:STOP " + QString(doubleToString(sysData.sweep.step.amplStop)).trimmed();

	sendSerialData(cmdStr);

//	if (isNeedCloseDemod)
//	{
//		savePowerOffState();
//	}

	return __SCPI_SUCCESS;
}

//设置终止幅度值
int tSysScpi::setStepStopAmplitude(QString value)
{
	bool isOk = false;
	double tempValue = 0;

	tempValue = getDoubleValue(value, &isOk);

	if (isOk)
	{
		return setStepStopAmplitude(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//脉冲旋钮设置终止幅度值
int tSysScpi::setStepStopAmplitudeWithRpg(int value)
{
	int tempValue = value;

	return setStepStopAmplitude(setValueByRPG(sysData.sweep.step.amplStop, tempValue, 2));
}

//设置点数
int tSysScpi::setStepPoints(double value)
{
	double tempValue = value;
	QString cmdStr = "";
	char temp[128] = { };
	char tempChar[128] = { };
	int stepFreqValue = 0;
	double setpAmplValue = 0;

	if (tempValue < 1)
		tempValue = 1;
	else if (tempValue > 65535)
		tempValue = 65535;

	sysData.sweep.step.point = tempValue;

	//
	//  strcpy(sysData.name, "Points:");
	//  strcpy(sysData.value, floatToStringDot2(sysData.sweep.step.point, temp));
	//  strcpy(sysData.unit, "");

	//  intToQString(, tempChar);
	sprintf(tempChar, "%d", sysData.sweep.step.point);
	cmdStr = ":SWEEP:POINT " + QString(tempChar).trimmed();
	if (sendSerialData(cmdStr) == __SCPI_FAILED)
	{
		return __SCPI_FAILED;
	}

	//  stepFreqValue = (((int)sysData.sweep.step.freqStop - (int)sysData.sweep.step.freqStart)) / sysData.sweep.step.point;
	//  setpAmplValue = (sysData.sweep.step.amplStop - sysData.sweep.step.amplStart) / sysData.sweep.step.point;
	//  if(sysData.sweep.mode == smFreq)
	//  {
	//	  sprintf(tempChar, "%d", stepFreqValue);
	//	  cmdStr = ":FREQ:STEP " + QString(tempChar).trimmed();
	//	  sendSerialData(cmdStr);
	//  }else if(sysData.sweep.mode == smAmpl)
	//  {
	//	  sprintf(tempChar, "%.2f", setpAmplValue);
	//	  cmdStr = ":POW:STEP " + QString(tempChar).trimmed();
	//	  sendSerialData(cmdStr);
	//  }else if (sysData.sweep.mode == smFreqAmpl)
	//  {
	//	  sprintf(tempChar, "%d", stepFreqValue);
	//	  cmdStr = ":FREQ:STEP " + QString(tempChar).trimmed();
	//	  sendSerialData(cmdStr);
	//	  sprintf(tempChar, "%.2f", setpAmplValue);
	//	  cmdStr = ":POW:STEP " + QString(tempChar).trimmed();
	//	  sendSerialData(cmdStr);
	//  }


//	if (isNeedCloseDemod)
//	{
//		savePowerOffState();
//	}

	return __SCPI_SUCCESS;
}

//设置点数
int tSysScpi::setStepPoints(QString value)
{
	bool isOk = false;
	double tempValue = 0;

	tempValue = getDoubleValue(value, &isOk);

	if (isOk)
	{
		return setStepPoints(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//脉冲旋钮设置点数
int tSysScpi::setStepPointsWithRpg(int value)
{
	int tempValue = value;

	//  SG1030SetValueByRPG(tempValue, 0, sysData.Sweep.Point.MaxValue,sysData.Sweep.Point.MinValue);
	return setStepPoints(setValueByRPG(sysData.sweep.step.point, tempValue, 0));
}

//设置驻留时间
int tSysScpi::setStepDwell(double value)
{
	double tempValue = value;
	QString cmdStr = "";

	char temp[128] = { };
	char tempChar[128] = { };

	if (tempValue < 20)
		tempValue = 20;
	else if (tempValue > 50e3)
		tempValue = 50e3;

	sysData.sweep.step.dwell = tempValue;

	//get paramter
	strcpy(sysData.name, "Dwell:");
	getMsTimeValue(sysData.sweep.step.dwell, temp);
	strcpy(sysData.value, temp);
	//  strcpy(sysData.value, floatToStringDot3(sysData.sweep.step.dwell, temp));
	strcpy(sysData.unit, timeToQUnit(sysData.sweep.step.dwell).trimmed().toStdString().c_str());

	sprintf(tempChar, "%.0f", sysData.sweep.step.dwell);
	cmdStr = ":SWE:DWEL " + QString(tempChar).trimmed();

	if (sendSerialData(cmdStr) == __SCPI_FAILED)
	{
		return __SCPI_FAILED;
	}

//	if (isNeedCloseDemod)
//	{
//		savePowerOffState();
//	}

	return __SCPI_SUCCESS;
}

//设置驻留时间
int tSysScpi::setStepDwell(QString value)
{
	bool isOk = false;
	double tempValue = 0;

	//  tempValue = getDoubleValue(value, &isOk);
	tempValue = getTimeValue(value, &isOk) * 1e-6;
	if (isOk)
	{
		return setStepDwell(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//脉冲旋钮设置驻留时间
int tSysScpi::setStepDwellWithRpg(int value)
{
	int tempValue = value;

	return setStepDwell(setValueByRPG(sysData.sweep.step.dwell, tempValue, 0));
}

//下发列表扫描中的列表数据
/**********************************************************************************************************
 * 下发的列表数据格式为：
 * '#' '0x3C' '数据长度高8位' '数据长度低8位' 'data>>24' 'data>>16' 'data>>8' 'data>>0' ...... crc 0x0D 0x0A
 * crc：累和校验值，从0x3C开始到crc值前面的说有的数值相加后取低8位的值
 * data包括：频率、幅度、驻留时间
 * 数据长度：列表的总行数 * 3
 * 注意：幅度数据--31位~24位表示正负：1表示负数，0表示正数；23位~8位表示整数部分；7位~0位表示小数部分
 *********************************************************************************************************/
int tSysScpi::setSweepListDownload(void)
{
	QString cmdStr = "";
	int i = 0;
	int j = 0;
	int rowIndex = 0;
	int writeCnt;
	int sendTotalCnt = 0; //发送的总字节数
	int sendCnt = 0; //每次下发256个字节的下发的次数
	int sendRemainder = 0; //下发整数个256后的余数
	int crcValue = 0;
	char sendbuf[6152];

	if (sysData.sweep.type == stStep)
	{
		return __SCPI_SUCCESS;
	} else
	{
		sendTotalCnt = sysData.sweep.list.allRow * 3 * 4 + 7;//计算一共需要发送的字节数

		sendbuf[0] = '#';
		sendbuf[1] = 0x3c;
		sendbuf[2] = ((sysData.sweep.list.allRow * 3) >> 8) & 0xFF;
		sendbuf[3] = ((sysData.sweep.list.allRow * 3)) & 0xFF;
		for (i = 4; i < sendTotalCnt - 3; i++) //填充数值
		{
			sendbuf[i] = (((unsigned int) sysData.sweep.list.data[((i - 4) / 12)].freq) >> 24) & 0xFF;
			i++;
			sendbuf[i] = (((unsigned int) sysData.sweep.list.data[((i - 4) / 12)].freq) >> 16) & 0xFF;
			i++;
			sendbuf[i] = (((unsigned int) sysData.sweep.list.data[((i - 4) / 12)].freq) >> 8) & 0xFF;
			i++;
			sendbuf[i] = (((unsigned int) sysData.sweep.list.data[((i - 4) / 12)].freq)) & 0xFF;
			i++;
			if (sysData.sweep.list.data[((i - 4) / 12)].ampl > 0)
			{
				sendbuf[i] = 0;

			} else
			{
				sendbuf[i] = 1;
			}
			i++;

			sendbuf[i] = (((int) fabs(sysData.sweep.list.data[((i - 4) / 12)].ampl)) >> 8) & 0xFF;
			i++;
			sendbuf[i] = (((int) fabs(sysData.sweep.list.data[((i - 4) / 12)].ampl))) & 0xFF;
			i++;
			sendbuf[i] = ((int) (fabs(sysData.sweep.list.data[((i - 4) / 12)].ampl) * 100) % 100);
			i++;

			sendbuf[i] = (((int) sysData.sweep.list.data[((i - 4) / 12)].dwell) >> 24) & 0xFF;
			i++;
			sendbuf[i] = (((int) sysData.sweep.list.data[((i - 4) / 12)].dwell) >> 16) & 0xFF;
			i++;
			sendbuf[i] = (((int) sysData.sweep.list.data[((i - 4) / 12)].dwell) >> 8) & 0xFF;
			i++;
			sendbuf[i] = (((int) sysData.sweep.list.data[((i - 4) / 12)].dwell)) & 0xFF;
		}
		for (j = 1; j < sendTotalCnt - 3; j++) //计算CRC的值
		{
			crcValue = (crcValue + sendbuf[j]) & 0xFF;
		}

		sendbuf[sendTotalCnt - 3] = crcValue;
		sendbuf[sendTotalCnt - 2] = 0x0D;
		sendbuf[sendTotalCnt - 1] = 0x0A;

		//分包发送
		sendCnt = sendTotalCnt / 256;
		for (i = 0; i < sendCnt; i++)
		{
			writeCnt = write(comHandle, &sendbuf[i * 256], 256); //发完整的一包
			if (writeCnt < 0)
			{
				return __SCPI_FAILED;
			}
		}
		sendRemainder = sendTotalCnt % 256; //计算剩余的待发送的字节数
		writeCnt = write(comHandle, &sendbuf[sendCnt * 256], sendRemainder); //发送剩余字节数
		if (writeCnt < 0)
		{
			return __SCPI_FAILED;
		}
	}

//	if (isNeedCloseDemod)
//	{
//		savePowerOffState();
//	}

	return __SCPI_SUCCESS;
}

//设置模拟调制的调制源
int tSysScpi::setAnalogModulateSource(int value)
{
	QString cmdStr = "";
	QString parameter;
	int tempValue = value;

	//关闭扫描显示
	closeSweepDisplay();
	if (tempValue < 0)
		tempValue = 0;
	else if (tempValue > 1)
		tempValue = 1;

	sysData.analog.source = analogSourceTypeDef(tempValue);
	sysData.digital.source = digitalSourceTypeDef(tempValue);	//由于模拟调制源和数字调制的调制源相同，所以需要关联
	switch (tempValue)
	{
		case astInternal:
			parameter = "0";
			break;
		case astExternal:
			parameter = "1";
			break;
		default:
			break;
	}
	cmdStr = ":ANMODU:SOURCE " + parameter;

	if (sendSerialData(cmdStr) == __SCPI_FAILED)
	{
		return __SCPI_FAILED;
	}

//	if (isNeedCloseDemod)
//	{
//		savePowerOffState();
//	}

	return __SCPI_SUCCESS;
}
int tSysScpi::setAnalogModulateSource(QString value)
{
	bool isOk = false;
	QString tempValue = value.trimmed().toUpper();

	if (tempValue == "0")
	{
		return setAnalogModulateSource(0);
	} else if (tempValue == "1")
	{
		return setAnalogModulateSource(1);
	}

	return __SCPI_FAILED;
}

//AM
int tSysScpi::setAmState(int value)
{
	QString cmdStr = "";
	bool tempValue = value == 1;

	//关闭解调状态
	closeDemodState();

	//关闭扫描显示
	closeSweepDisplay();

	sysData.analog.am.state = tempValue;

	if (tempValue)
	{
		cmdStr = ":AM:STAT 1";
	} else
	{
		cmdStr = ":AM:STAT 0";
	}

	sendSerialData(cmdStr);

//	if (isNeedCloseDemod)
//	{
//		savePowerOffState();
//	}

	return __SCPI_SUCCESS;
}

int tSysScpi::setAmState(QString value)
{
	bool isOk = false;
	bool tempValue = 0;

	tempValue = getBoolValue(value, &isOk);

	if (isOk)
	{
		return setAmState(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

int tSysScpi::setAmDepth(double value)
{
	double tempValue = value;
	QString cmdStr = "";
	char temp[128] = { };
	char tempChar[128] = { };

	//判断
	if (tempValue >= 100)
	{
		tempValue = 100;
	} else if (tempValue <= 0)
	{
		tempValue = 0;
	}
	sysData.analog.am.depth = tempValue;

	strcpy(sysData.name, "AM Depth:");
	strcpy(sysData.value, floatToStringDot2(sysData.analog.am.depth, temp));
	strcpy(sysData.unit, "%");

	//参数下发
	//1、数字、命令 组成 字符串
	sprintf(tempChar, "%0.2f", sysData.analog.am.depth);
	cmdStr = QString(":AM:DEPT ") + QString(tempChar).trimmed();
	//2、下发
	sendSerialData(cmdStr);

	return __SCPI_SUCCESS;
}

int tSysScpi::setAmDepth(QString value)
{
	bool isOk = false;
	double tempValue = 0;

	tempValue = getDoubleValue(value, &isOk);

	if (isOk)
	{
		return setAmDepth(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

int tSysScpi::setAmDepthWithRpg(int value)
{
	return setAmDepth(setValueByRPG(sysData.analog.am.depth, value, 2));
}

//设置
int tSysScpi::setAmRate(double value)
{
	double tempValue = value;
	QString cmdStr = "";
	char temp[128] = { };
	char tempChar[128] = { };

	//判断
	if (tempValue >= 1e6) //SG1030A:1MHz
	{
		tempValue = 1e6;
	} else if (tempValue <= 20)
	{
		tempValue = 20;
	}
	sysData.analog.am.rate = tempValue;

	//get input paremwter
	strcpy(sysData.name, "AM Rate:");
	strcpy(tempChar, getFreqString(sysData.analog.am.rate, temp));
	insertThousands(tempChar);
	strcpy(sysData.value, tempChar);
	strcpy(sysData.unit, freqToQUnit(sysData.analog.am.rate).trimmed().toStdString().c_str());

	//参数下发
	//1、数字、命令 组成 字符串
	sprintf(tempChar, "%0.0f", sysData.analog.am.rate);
	cmdStr = QString(":AM:INT:FUNC:FREQ ") + QString(tempChar).trimmed();

	//2、下发
	sendSerialData(cmdStr);

	return __SCPI_SUCCESS;
}

int tSysScpi::setAmRate(QString value)
{
	bool isOk = false;
	double tempValue = 0;

	tempValue = getFreqValue(value, &isOk);

	if (isOk)
	{
		return setAmRate(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//脉冲旋钮设置
int tSysScpi::setAmRateWithRpg(int value)
{
	return setAmRate(setValueByRPG(sysData.analog.am.rate, value, 0));
}

//FM
int tSysScpi::setFmState(int value)
{
	QString cmdStr = "";
	bool tempValue = value == 1;

	//关闭解调状态
	closeDemodState();

	//关闭扫描显示
	closeSweepDisplay();

	sysData.analog.fm.state = tempValue;

	if (tempValue)
	{
		cmdStr = ":FM:STAT 1";
	} else
	{
		cmdStr = ":FM:STAT 0";
	}

	sendSerialData(cmdStr);

//	if (isNeedCloseDemod)
//	{
//		savePowerOffState();
//	}

	return __SCPI_SUCCESS;
}

int tSysScpi::setFmState(QString value)
{
	bool isOk = false;
	bool tempValue = 0;

	tempValue = getBoolValue(value, &isOk);

	if (isOk)
	{
		return setFmState(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

int tSysScpi::setFmDev(double value)
{
	double tempValue = value;
	QString cmdStr = "";

	char temp[128] = { };
	char tempChar[128] = { };

	//判断
	if (tempValue >= 2e6)
	{
		tempValue = 2e6;
	} else if (tempValue <= 0)
	{
		tempValue = 0;
	}

	sysData.analog.fm.deviation = tempValue;

	//get input paremwter
	strcpy(sysData.name, "FM DEV:");
	strcpy(tempChar, getFreqString(sysData.analog.fm.deviation, temp));
	insertThousands(tempChar);
	strcpy(sysData.value, tempChar);
	strcpy(sysData.unit, freqToQUnit(sysData.analog.fm.deviation).trimmed().toStdString().c_str());

	//参数下发
	//1、数字、命令 组成 字符串
	sprintf(tempChar, "%0.0f", sysData.analog.fm.deviation);
	cmdStr = QString(":FM:DEV ") + QString(tempChar).trimmed();

	//2、下发
	sendSerialData(cmdStr);

	return __SCPI_SUCCESS;
}

int tSysScpi::setFmDev(QString value)
{
	bool isOk = false;
	double tempValue = 0;

	tempValue = getFreqValue(value, &isOk);

	if (isOk)
	{
		return setFmDev(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

int tSysScpi::setFmDevWithRpg(int value)
{
	return setFmDev(setValueByRPG(sysData.analog.fm.deviation, value, 0));
}

//设置
int tSysScpi::setFmRate(double value)
{
	double tempValue = value;
	char temp[128] = { };
	char tempChar[128] = { };
	QString cmdStr = "";

	//判断
	if (tempValue >= 1e6) //SG1030A:1MHz
	{
		tempValue = 1e6;
	} else if (tempValue <= 20)
	{
		tempValue = 20;
	}
	sysData.analog.fm.rate = tempValue;

	//get input paramter
	strcpy(sysData.name, "FM Rate:");
	strcpy(tempChar, getFreqString(sysData.analog.fm.rate, temp));
	insertThousands(tempChar);
	strcpy(sysData.value, tempChar);
	strcpy(sysData.unit, freqToQUnit(sysData.analog.fm.rate).trimmed().toStdString().c_str());

	//参数下发
	//1、数字、命令 组成 字符串
	sprintf(tempChar, "%0.0f", sysData.analog.fm.rate);
	cmdStr = QString(":FM:INT:FUNC:FREQ ") + QString(tempChar).trimmed();

	//2、下发
	sendSerialData(cmdStr);

	return __SCPI_SUCCESS;
}

int tSysScpi::setFmRate(QString value)
{
	bool isOk = false;
	double tempValue = 0;

	tempValue = getFreqValue(value, &isOk);

	if (isOk)
	{
		return setFmRate(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//脉冲旋钮设置
int tSysScpi::setFmRateWithRpg(int value)
{
	return setFmRate(setValueByRPG(sysData.analog.fm.rate, value, 0));
}

//PM
int tSysScpi::setPmState(int value)
{
	QString cmdStr = "";
	bool tempValue = value == 1;

	//关闭解调状态
	closeDemodState();

	//关闭扫描显示
	closeSweepDisplay();

	sysData.analog.pm.state = tempValue;

	if (tempValue)
	{
		cmdStr = ":PM:STAT 1";
	} else
	{
		cmdStr = ":PM:STAT 0";
	}

	sendSerialData(cmdStr);

//	if (isNeedCloseDemod)
//	{
//		savePowerOffState();
//	}

	return __SCPI_SUCCESS;
}
int tSysScpi::setPmState(QString value)
{
	bool isOk = false;
	bool tempValue = 0;

	tempValue = getBoolValue(value, &isOk);

	if (isOk)
	{
		return setPmState(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

int tSysScpi::setPmPhase(double value)
{
	double tempValue = value;
	char temp[128] = { };
	char tempChar[128] = { };
	QString cmdStr = "";

	//判断
	if (tempValue >= 360.00)
	{
		tempValue = 360;
	} else if (tempValue <= 0.00)
	{
		tempValue = 0;
	}
	sysData.analog.pm.phase = tempValue;

	strcpy(sysData.name, "PM Phase:");
	//  strcpy(sysData.value, floatToStringDot2(sysData.analog.pm.phase, temp));
	sprintf(sysData.value, "%16.2f", tempValue);

	strcpy(sysData.unit, "°");

	//参数下发
	//1、数字、命令 组成 字符串
	sprintf(tempChar, "%.2f", sysData.analog.pm.phase);
	cmdStr = QString(":PM:PHAS ") + QString(tempChar).trimmed();

	//2、下发
	sendSerialData(cmdStr);

	return __SCPI_SUCCESS;
}

int tSysScpi::setPmPhase(QString value)
{
	bool isOk = false;
	double tempValue = 0;

	tempValue = getDoubleValue(value, &isOk);

	if (isOk)
	{
		return setPmPhase(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}
int tSysScpi::setPmPhaseWithRpg(int value)
{
	return setPmPhase(setValueByRPG(sysData.analog.pm.phase, value, 2));
}

int tSysScpi::setPmRate(double value)
{
	double tempValue = value;
	char temp[128] = { };
	char tempChar[128] = { };
	QString cmdStr = "";

	//判断
	if (tempValue >= 1e6) //SG1030A:1MHz
	{
		tempValue = 1e6;
	} else if (tempValue <= 20)
	{
		tempValue = 20;
	}
	sysData.analog.pm.rate = tempValue;

	//get input paremwter
	strcpy(sysData.name, "PM Rate:");
	strcpy(tempChar, getFreqString(sysData.analog.pm.rate, temp));
	insertThousands(tempChar);
	strcpy(sysData.value, tempChar);
	strcpy(sysData.unit, freqToQUnit(sysData.analog.pm.rate).trimmed().toStdString().c_str());

	//参数下发
	//1、数字、命令 组成 字符串
	sprintf(tempChar, "%.0f", sysData.analog.pm.rate);
	cmdStr = QString(":PM:INT:FUNC:FREQ ") + QString(tempChar).trimmed();

	//2、下发
	sendSerialData(cmdStr);

	return __SCPI_SUCCESS;
}
//设置
int tSysScpi::setPmRate(QString value)
{
	bool isOk = false;
	double tempValue = 0;

	tempValue = getFreqValue(value, &isOk);

	if (isOk)
	{
		return setPmRate(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//脉冲旋钮设置
int tSysScpi::setPmRateWithRpg(int value)
{
	return setPmRate(setValueByRPG(sysData.analog.pm.rate, value, 0));
}

//设置数字调制状态
int tSysScpi::setDigitalModuateState(int value)
{
	QString cmdStr = "";
	bool tempValue = value == 1;

	//关闭解调状态
	closeDemodState();

	//关闭扫描显示
	closeSweepDisplay();

	sysData.digital.digitalstate = tempValue;
	if (tempValue)
	{
		switch (sysData.digital.digitaltype)
		{
			case dt2ASK:
			case dt4ASK:
			case dt8ASK:
				cmdStr = ":ASK:TYPE 2 ";
				if (sendSerialData(cmdStr) == __SCPI_FAILED)
					return __SCPI_FAILED;
				cmdStr = ":ASK:STAT 1";
				if (sendSerialData(cmdStr) == __SCPI_FAILED)
					return __SCPI_FAILED;
				break;
			case dt2FSK:
				cmdStr = ":FSK:TYPE 2 ";
				if (sendSerialData(cmdStr) == __SCPI_FAILED)
					return __SCPI_FAILED;
				cmdStr = ":FSK:STAT 1 ";
				if (sendSerialData(cmdStr) == __SCPI_FAILED)
					return __SCPI_FAILED;
				break;
			case dt4FSK:
				cmdStr = ":FSK:TYPE 4 ";
				if (sendSerialData(cmdStr) == __SCPI_FAILED)
					return __SCPI_FAILED;
				cmdStr = ":FSK:STAT 1 ";
				if (sendSerialData(cmdStr) == __SCPI_FAILED)
					return __SCPI_FAILED;
				break;
			case dt8FSK:
				cmdStr = ":FSK:TYPE 8 ";
				if (sendSerialData(cmdStr) == __SCPI_FAILED)
					return __SCPI_FAILED;
				cmdStr = ":FSK:STAT 1 ";
				if (sendSerialData(cmdStr) == __SCPI_FAILED)
					return __SCPI_FAILED;
				break;
			case dt2PSK:
				cmdStr = ":PSK:TYPE 2 ";
				if (sendSerialData(cmdStr) == __SCPI_FAILED)
					return __SCPI_FAILED;
				cmdStr = ":PSK:STAT 1 ";
				if (sendSerialData(cmdStr) == __SCPI_FAILED)
					return __SCPI_FAILED;
				break;
			case dt4PSK:
				cmdStr = ":PSK:TYPE 4 ";
				if (sendSerialData(cmdStr) == __SCPI_FAILED)
					return __SCPI_FAILED;
				cmdStr = ":PSK:STAT 1 ";
				if (sendSerialData(cmdStr) == __SCPI_FAILED)
					return __SCPI_FAILED;
				break;
			case dt8PSK:
				cmdStr = ":PSK:TYPE 8 ";
				if (sendSerialData(cmdStr) == __SCPI_FAILED)
					return __SCPI_FAILED;
				cmdStr = ":PSK:STAT 1 ";
				if (sendSerialData(cmdStr) == __SCPI_FAILED)
					return __SCPI_FAILED;
				break;
		}
	} else
	{
		cmdStr = ":ASK:STAT 0";
		if (sendSerialData(cmdStr) == __SCPI_FAILED)
			return __SCPI_FAILED;
		cmdStr = ":FSK:STAT 0";
		if (sendSerialData(cmdStr) == __SCPI_FAILED)
			return __SCPI_FAILED;
		cmdStr = ":PSK:STAT 0";
		if (sendSerialData(cmdStr) == __SCPI_FAILED)
			return __SCPI_FAILED;
	}

	//  switch(sysData.digital.digitaltype)
	//  {
	//    case dt2ASK:
	//    case dt4ASK:
	//    case dt8ASK:
	//	  if(tempValue)
	//	  {
	//	  	cmdStr = ":ASK:STAT 1";
	//	  }else
	//	  {
	//	  	cmdStr = ":ASK:STAT 0";
	//	  }
	//	  break;
	//    case dt2FSK:
	//    case dt4FSK:
	//    case dt8FSK:
	//      if(tempValue)
	//	  {
	//		cmdStr = ":FSK:STAT 1";
	//	  }else
	//	  {
	//		cmdStr = ":FSK:STAT 0";
	//	  }
	//	  break;
	//    case dt2PSK:
	//    case dt4PSK:
	//    case dt8PSK:
	//      if(tempValue)
	//	  {
	//		cmdStr = ":PSK:STAT 1";
	//	  }else
	//	  {
	//		cmdStr = ":PSK:STAT 0";
	//	  }
	//	  break;
	//  }

	//  sendSerialData(cmdStr);

//	if (isNeedCloseDemod)
//	{
//		savePowerOffState();
//	}

	return __SCPI_SUCCESS;
}
//设置数字调制状态
int tSysScpi::setDigitalModuateState(QString value)
{
	bool isOk = false;
	bool tempValue = 0;

	tempValue = getBoolValue(value, &isOk);

	if (isOk)
	{
		return setDigitalModuateState(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置数字调制的调制源
int tSysScpi::setDigitalModulateSource(int value)
{
	QString cmdStr = "";
	QString parameter;
	int tempValue = value;

	//关闭扫描显示
	closeSweepDisplay();
	if (tempValue < 0)
		tempValue = 0;
	else if (tempValue > 1)
		tempValue = 1;

	sysData.digital.source = digitalSourceTypeDef(tempValue);
	sysData.analog.source = analogSourceTypeDef(tempValue);	//由于模拟调制源和数字调制的调制源相同，所以需要关联
	switch (tempValue)
	{
		case dstInternal:
			parameter = "0";
			break;
		case dstExternal:
			parameter = "1";
			break;
		default:
			break;
	}
	cmdStr = ":DGMODU:SOURCE " + parameter;

	if (sendSerialData(cmdStr) == __SCPI_FAILED)
	{
		return __SCPI_FAILED;
	}

//	if (isNeedCloseDemod)
//	{
//		savePowerOffState();
//	}

	return __SCPI_SUCCESS;
}
int tSysScpi::setDigitalModulateSource(QString value)
{
	bool isOk = false;
	QString tempValue = value.trimmed().toUpper();

	if (tempValue == "0")
	{
		return setDigitalModulateSource(0);
	} else if (tempValue == "1")
	{
		return setDigitalModulateSource(1);
	}

	return __SCPI_FAILED;
}

//设置数字调制类型
int tSysScpi::setDigitalModuateType(int value)
{
	int tempValue = value;
	QString cmdStr = "";
	QString parameter = "";

	if (tempValue < 0)
	{
		tempValue = 0;
	} else if (tempValue > 8)
	{
		tempValue = 8;
	}

	sysData.digital.digitaltype = digitalTypeDef(tempValue);

	switch (tempValue)
	{
		case dt2ASK:
			parameter = "2";
			cmdStr = ":ASK:TYPE " + parameter;
			break;
		case dt4ASK:
			parameter = "4";
			cmdStr = ":ASK:TYPE " + parameter;
			break;
		case dt8ASK:
			parameter = "8";
			cmdStr = ":ASK:TYPE " + parameter;
			break;
		case dt2FSK:
			parameter = "2";
			cmdStr = ":FSK:TYPE " + parameter;
			break;
		case dt4FSK:
			parameter = "4";
			cmdStr = ":FSK:TYPE " + parameter;
			break;
		case dt8FSK:
			parameter = "8";
			cmdStr = ":FSK:TYPE " + parameter;
			break;
		case dt2PSK:
			parameter = "2";
			cmdStr = ":PSK:TYPE " + parameter;
			break;
		case dt4PSK:
			parameter = "4";
			cmdStr = ":PSK:TYPE " + parameter;
			break;
		case dt8PSK:
			parameter = "8";
			cmdStr = ":PSK:TYPE " + parameter;
			break;
		default:
			break;
	}

	sendSerialData(cmdStr);
//	if (isNeedCloseDemod)
//	{
//		savePowerOffState();
//	}

	return __SCPI_SUCCESS;
}

int tSysScpi::setDigitalModuateType(QString value)
{
	bool isOk = false;
	QString tempValue = value.trimmed().toUpper();

	if (tempValue == "2ASK")
	{
		return setDigitalModuateType(dt2ASK);
	} else if (tempValue == "4ASK")
	{
		return setDigitalModuateType(dt4ASK);
	} else if (tempValue == "8ASK")
	{
		return setDigitalModuateType(dt8ASK);
	} else if (tempValue == "2FSK")
	{
		return setDigitalModuateType(dt2FSK);
	} else if (tempValue == "4FSK")
	{
		return setDigitalModuateType(dt4FSK);
	} else if (tempValue == "8FSK")
	{
		return setDigitalModuateType(dt8FSK);
	} else if (tempValue == "2PSK")
	{
		return setDigitalModuateType(dt2PSK);
	} else if (tempValue == "4PSK")
	{
		return setDigitalModuateType(dt4PSK);
	} else if (tempValue == "8PSK")
	{
		return setDigitalModuateType(dt8PSK);
	}

	return __SCPI_FAILED;
}

//设置数字调制参数，包括深度，频偏，相偏，不包括速率
int tSysScpi::setDigitalModulatePara(double value)
{
	double tempValue = value;
	QString cmdStr = "";
	char temp[128] = { };
	char tempChar[128] = { };
	switch (sysData.digital.digitaltype)
	{
		case dt2ASK:
		case dt4ASK:
		case dt8ASK:
			if (tempValue < 0)
				tempValue = 0;
			else if (tempValue > 100.00)
				tempValue = 100.00;
			sysData.digital.digitalpara.depth = tempValue;

			strcpy(sysData.name, "Depth");
			floatToStringDot2(tempValue, tempChar);
			strcpy(sysData.value, tempChar);
			strcpy(sysData.unit, "%");
			cmdStr = ":ASK:DEPTH " + QString(tempChar).trimmed().toUpper();
			break;

		case dt2FSK:
		case dt4FSK:
		case dt8FSK:
			if (tempValue < 0)
				tempValue = 0;
			else if (tempValue > 10 * 1e6)
				tempValue = 10 * 1e6;
			sysData.digital.digitalpara.deviation = tempValue;
			sprintf(tempChar, "%.0f", tempValue);
			cmdStr = ":FSK:MIN:FREQ:OFFSET " + QString(tempChar).trimmed().toUpper();

			strcpy(sysData.name, "Devitation");
			strcpy(tempChar, getFreqString(sysData.digital.digitalpara.deviation, temp));
			insertThousands(tempChar);
			strcpy(sysData.value, tempChar);
			strcpy(sysData.unit, freqToQUnit(sysData.digital.digitalpara.deviation).trimmed().toStdString().c_str());
			break;

		case dt2PSK:
		case dt4PSK:
		case dt8PSK:
			if (tempValue < 0)
				tempValue = 0;
			else if (tempValue > 360)
				tempValue = 360;
			sysData.digital.digitalpara.phase = tempValue;

			strcpy(sysData.name, "Phase");
			sprintf(tempChar, "%.2f", sysData.digital.digitalpara.phase);
			strcpy(sysData.value, tempChar);
			strcpy(sysData.unit, "°");
			cmdStr = ":PSK:PHASE:OFFSET " + QString(tempChar).trimmed().toUpper();
			break;
		default:
			break;
	}
	if (sendSerialData(cmdStr) == __SCPI_FAILED)
	{
		return __SCPI_FAILED;
	}
//	if (isNeedCloseDemod)
//	{
//		savePowerOffState();
//	}
	return __SCPI_SUCCESS;
}

//设置数字调制参数，包括深度，频偏，相偏，不包括速率
int tSysScpi::setDigitalModulatePara(QString value)
{
	bool isOk = false;
	double tempValue = 0;

	switch (sysData.digital.digitaltype)
	{
		case dt2ASK:
		case dt4ASK:
		case dt8ASK:
		case dt2PSK:
		case dt4PSK:
		case dt8PSK:
			tempValue = getDoubleValue(value, &isOk);
			break;
		case dt2FSK:
		case dt4FSK:
		case dt8FSK:
			tempValue = getFreqValue(value, &isOk);
			break;
		default:
			break;
	}

	if (isOk)
	{
		return setDigitalModulatePara(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

int tSysScpi::setDigitalModulateParaWithRpg(int value)
{
	float tempValue;
	switch (sysData.digital.digitaltype)
	{
		case dt2ASK:
		case dt4ASK:
		case dt8ASK:
			return setDigitalModulatePara(setValueByRPG(sysData.digital.digitalpara.depth, value, 2));
			break;
		case dt2FSK:
		case dt4FSK:
		case dt8FSK:
			return setDigitalModulatePara(setValueByRPG(sysData.digital.digitalpara.deviation, value, 0));
			break;
		case dt2PSK:
		case dt4PSK:
		case dt8PSK:
			return setDigitalModulatePara(setValueByRPG(sysData.digital.digitalpara.phase, value, 2));
			break;
		default:
			break;
	}
}

//设置数字调制速率
int tSysScpi::setDigitalModuateRate(double value)
{
	double tempValue = value;
	QString cmdStr = "";
	char temp[128] = { };
	char tempChar[128] = { };

	if (tempValue < MINASKRATE)
	{
		tempValue = MINASKRATE;
	} else if (tempValue > MAXASKRATE)
	{
		tempValue = MAXASKRATE;
	}

	sysData.digital.digitalpara.rate = tempValue;

	//get input paremwter
	strcpy(sysData.name, "Rate:");
	strcpy(tempChar, getFreqString(sysData.digital.digitalpara.rate, temp));
	insertThousands(tempChar);
	strcpy(sysData.value, tempChar);
	strcpy(sysData.unit, freqToQUnit(sysData.digital.digitalpara.rate).trimmed().toStdString().c_str());

	sprintf(tempChar, "%.0f", sysData.digital.digitalpara.rate);
	cmdStr = ":AFPSK:INT:FUNC:FREQ " + QString(tempChar);

	sendSerialData(cmdStr);

//	if (isNeedCloseDemod)
//	{
//		savePowerOffState();
//	}

	return __SCPI_SUCCESS;
}

int tSysScpi::setDigitalModuateRate(QString value)
{
	bool isOk = false;
	double tempValue = 0;

	tempValue = getFreqValue(value, &isOk);

	if (isOk)
	{
		return setDigitalModuateRate(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

int tSysScpi::setDigitalModuateRateWithRpg(int value)
{
	return setDigitalModuateRate(setValueByRPG(sysData.digital.digitalpara.rate, value, 0));
}

/**********************************************************************************************************
 * 下发的列表数据格式为：
 * '#' '0x2C' '数据长度高8位' '数据长度低8位' '数据' ...... crc 0x0D 0x0A
 * crc：累和校验值，从0x2C开始到crc值前面的说有的数值相加后取低8位的值
 *********************************************************************************************************/
//下发数字调制外部调制源调制序列
int tSysScpi::setDigtalSequenceDownload(void)
{
	QString cmdStr = "";
	char tempChar[64] = { 0 };
	int i = 0;
	int j = 0;
	int writeCnt;
	int sendTotalCnt = 0; //发送的总字节数
	int sendCnt = 0; //每次下发256个字节的下发的次数
	int sendRemainder = 0; //下发整数个256后的余数
	int crcValue = 0;
	unsigned char sendbuf[3079];
	unsigned int sequenceCnt = sysData.digital.sequenceCnt;
	unsigned int sequenceCntBefore = sysData.digital.sequenceCntBefore;
	sendTotalCnt = sequenceCnt + 7;

	//关闭MOD
	cmdStr = ":SYST:RFO 0";
	if (sendSerialData(cmdStr) == __SCPI_FAILED)
	{
		return __SCPI_FAILED;
	}
	//下发序列长度
	sprintf(tempChar, "%d", sequenceCntBefore);
	cmdStr = ":ACTIVE:UNITS:NUM " + QString(tempChar).trimmed();
	if (sendSerialData(cmdStr) == __SCPI_FAILED)
	{
		return __SCPI_FAILED;
	}
	//下发命令，使单片机进入大数据接收模式
	cmdStr = ":SYSDATA:RCV:MODE 1";
	if (sendSerialData(cmdStr) == __SCPI_FAILED)
	{
		return __SCPI_FAILED;
	}

	//	if(sysData.sweep.type == stStep)
	//	{
	//	  return __SCPI_SUCCESS;
	//	}else
	{
		sendbuf[0] = '#';
		sendbuf[1] = 0x2c;
		sendbuf[2] = (sequenceCnt >> 8) & 0xFF;
		sendbuf[3] = (sequenceCnt) & 0xFF;
		for (i = 0; i < sequenceCnt; i++) //填充数值
		{
			sendbuf[i + 4] = sysData.digital.sequence[i];
		}
		for (j = 1; j < (sequenceCnt + 4); j++) //计算CRC的值
		{
			crcValue = (crcValue + sendbuf[j]) & 0xFF;
		}

		sendbuf[sendTotalCnt - 3] = crcValue;
		sendbuf[sendTotalCnt - 2] = 0x0D;
		sendbuf[sendTotalCnt - 1] = 0x0A;

		//分包发送
		sendCnt = sendTotalCnt / 256;
		for (i = 0; i < sendCnt; i++)
		{
			writeCnt = write(comHandle, &sendbuf[i * 256], 256); //发完整的一包
			if (writeCnt < 0)
			{
				return __SCPI_FAILED;
			}
		}
		sendRemainder = sendTotalCnt % 256; //计算剩余的待发送的字节数
		writeCnt = write(comHandle, &sendbuf[sendCnt * 256], sendRemainder); //发送剩余字节数
		if (writeCnt < 0)
		{
			return __SCPI_FAILED;
		}
	}
	//下发命令，使单片机退出大数据接收模式
	cmdStr = ":SYSDATA:RCV:MODE 0";
	if (sendSerialData(cmdStr) == __SCPI_FAILED)
	{
		return __SCPI_FAILED;
	}
	//打开MOD
	cmdStr = ":SYST:RFO 1";
	if (sendSerialData(cmdStr) == __SCPI_FAILED)
	{
		return __SCPI_FAILED;
	}

//	if (isNeedCloseDemod)
//	{
//		savePowerOffState();
//	}

	return __SCPI_SUCCESS;
}

#if 0
//ASK
int tSysScpi::setAskState(int value)
{
	QString cmdStr = "";
	bool tempValue = value == 1;

	//关闭解调状态
	closeDemodState();

	//关闭扫描显示
	closeSweepDisplay();

	sysData.digital.ask.state = tempValue;

	if(tempValue)
	{
		cmdStr = ":ASK:STATe 1;\r\n";
	} else
	{
		cmdStr = ":ASK:STATe 0;\r\n";
	}

	sendSerialData(cmdStr);

	if(isNeedCloseDemod)
	{
		savePowerOffState();
	}

	return __SCPI_SUCCESS;
}

int tSysScpi::setAskState(QString value)
{
	bool isOk = false;
	bool tempValue = 0;

	tempValue = getBoolValue(value, &isOk);

	if (isOk)
	{
		return setAskState(tempValue);
	}
	else
	{
		return __SCPI_FAILED;
	}
}

int tSysScpi::setAskMode(int value)
{
	int tempValue = value;
	QString cmdStr = "";
	QString parameter = "";

	if(tempValue < 0)
	{
		tempValue = 0;
	} else if(tempValue > 2)
	{
		tempValue = 2;
	}

	sysData.digital.ask.mode = askModeDef(tempValue);

	switch (tempValue)
	{
		case askm2: parameter = "2"; break;
		case askm4: parameter = "4"; break;
		case askm8: parameter = "8"; break;
		default:
		break;
	}

	cmdStr = ":ASK:TYPE " + parameter + ";\r\n";

	sendSerialData(cmdStr);
	if(isNeedCloseDemod)
	{
		savePowerOffState();
	}

	return __SCPI_SUCCESS;
}

int tSysScpi::setAskMode(QString value)
{
	bool isOk = false;
	QString tempValue = value.trimmed().toUpper();

	if(tempValue == "2PSK")
	{
		return setAskMode(askm2);
	} else if(tempValue == "4FSK")
	{
		return setAskMode(askm4);
	} else if(tempValue == "8FSK")
	{
		return setAskMode(askm8);
	}

	return __SCPI_FAILED;
}

int tSysScpi::setAskRate(double value)
{
	double tempValue = value;
	QString cmdStr = "";
	char temp[128] =
	{};
	char tempChar[128] =
	{};

	if(tempValue < MINASKRATE)
	{
		tempValue = MINASKRATE;
	} else if (tempValue > MAXASKRATE)
	{
		tempValue = MAXASKRATE;
	}

	sysData.digital.ask.rate = tempValue;

	//get input paremwter
	strcpy(sysData.name, "ASK Rate:");
	strcpy(tempChar, getFreqString(sysData.digital.ask.rate, temp));
	insertThousands(tempChar);
	strcpy(sysData.value, tempChar);
	strcpy(sysData.unit, freqToQUnit(sysData.digital.ask.rate).trimmed().toStdString().c_str());

	cmdStr = ":ASK:INTernal:FUNCtion:FREQuency  " + doubleToString(sysData.digital.ask.rate) + ";\r\n";

	sendSerialData(cmdStr);

	if(isNeedCloseDemod)
	{
		savePowerOffState();
	}

	return __SCPI_SUCCESS;
}

int tSysScpi::setAskRate(QString value)
{
	bool isOk = false;
	double tempValue = 0;

	tempValue = getFreqValue(value, &isOk);

	if (isOk)
	{
		return setAskRate(tempValue);
	}
	else
	{
		return __SCPI_FAILED;
	}
}

int tSysScpi::setAskRateWithRpg(int value)
{
	int tempValue = value;

	return setAskRate(setValueByRPG(sysData.digital.ask.rate, value, 0));
}

//FSK
int tSysScpi::setFskState(int value)
{
	QString cmdStr = "";
	bool tempValue = value == 1;

	//关闭解调状态
	closeDemodState();

	//关闭扫描显示
	closeSweepDisplay();

	sysData.digital.fsk.state = tempValue;

	if(tempValue)
	{
		cmdStr = ":FSK:STAT ON;\r\n";
	} else
	{
		cmdStr = ":FSK:STAT OFF;\r\n";
	}

	sendSerialData(cmdStr);

	if(isNeedCloseDemod)
	{
		savePowerOffState();
	}

	return __SCPI_SUCCESS;
}

int tSysScpi::setFskState(QString value)
{
	bool isOk = false;
	bool tempValue = 0;

	tempValue = getBoolValue(value, &isOk);

	if (isOk)
	{
		return setFskState(tempValue);
	}
	else
	{
		return __SCPI_FAILED;
	}
}

int tSysScpi::setFskMode(int value)
{
	int tempValue = value;
	QString cmdStr = "";
	QString parameter = "";

	if(tempValue < 0)
	{
		tempValue = 0;
	} else if(tempValue > 2)
	{
		tempValue = 2;
	}

	sysData.digital.fsk.mode = fskModeDef(tempValue);

	switch (tempValue)
	{
		case fskm2: parameter = "2"; break;
		case fskm4: parameter = "4"; break;
		case fskm8: parameter = "8"; break;
		default: break;
	}

	cmdStr = ":FSK:TYPE " + parameter + ";\r\n";

	sendSerialData(cmdStr);
	return __SCPI_SUCCESS;
}

int tSysScpi::setFskMode(QString value)
{
	bool isOk = false;
	QString tempValue = value.trimmed().toUpper();

	if(tempValue == "2PSK")
	{
		return setFskMode(fskm2);
	} else if(tempValue == "4FSK")
	{
		return setFskMode(pskmQpsk);
	} else if(tempValue == "8FSK")
	{
		return setPskMode(fskm8);
	}

	return __SCPI_FAILED;
}

int tSysScpi::setFskRate(double value)
{
	double tempValue = value;
	QString cmdStr = "";

	char temp[128] =
	{};
	char tempChar[128] =
	{};

	if(tempValue < MINFSKRATE)
	{
		tempValue = MINFSKRATE;
	} else if (tempValue > MAXFSKRATE)
	{
		tempValue = MAXFSKRATE;
	}

	sysData.digital.fsk.rate = tempValue;

	//get input paramter
	strcpy(sysData.name, "FSK Rate:");
	strcpy(tempChar, getFreqString(sysData.digital.fsk.rate, temp));
	insertThousands(tempChar);
	strcpy(sysData.value, tempChar);
	strcpy(sysData.unit, freqToQUnit(sysData.digital.fsk.rate).trimmed().toStdString().c_str());

	cmdStr = ":FSK:INTernal:FUNCtion:FREQuency  " + doubleToString(sysData.digital.fsk.rate) + ";\r\n";

	sendSerialData(cmdStr);

	if(isNeedCloseDemod)
	{
		savePowerOffState();
	}
}

int tSysScpi::setFskRate(QString value)
{
	bool isOk = false;
	double tempValue = 0;

	tempValue = getFreqValue(value, &isOk);

	if (isOk)
	{
		return setFskRate(tempValue);
	}
	else
	{
		return __SCPI_FAILED;
	}
}

int tSysScpi::setFskRateWithRpg(int value)
{
	int tempValue = value;

	return setFskRate(setValueByRPG(sysData.digital.fsk.rate, tempValue, 0));
}

int tSysScpi::setFskStep(double value)
{
	double tempValue = value;
	QString cmdStr = "";
	char temp[128] =
	{};
	char tempChar[128] =
	{};

	if(tempValue < MINFSKRATE)
	{
		tempValue = MINFSKRATE;
	} else if (tempValue > MAXFSKRATE)
	{
		tempValue = MAXFSKRATE;
	}

	sysData.digital.fsk.step = tempValue;

	//get input paramter
	strcpy(sysData.name, "FSK Step:");
	strcpy(tempChar, getFreqString(sysData.digital.fsk.step, temp));
	insertThousands(tempChar);
	strcpy(sysData.value, tempChar);
	strcpy(sysData.unit, freqToQUnit(sysData.digital.fsk.step).trimmed().toStdString().c_str());

	cmdStr = ":FSK:STEP  " + doubleToString(sysData.digital.fsk.step) + ";\r\n";

	sendSerialData(cmdStr);

	if(isNeedCloseDemod)
	{
		savePowerOffState();
	}

	return __SCPI_SUCCESS;
}

int tSysScpi::setFskStep(QString value)
{
	bool isOk = false;
	double tempValue = 0;

	tempValue = getFreqValue(value, &isOk);

	if (isOk)
	{
		return setFskStep(tempValue);
	}
	else
	{
		return __SCPI_FAILED;
	}
}

int tSysScpi::setFskStepWithRpg(int value)
{
	int tempValue = value;

	return setFskStep(setValueByRPG(sysData.digital.fsk.step, tempValue, 0));
}

//PSK
int tSysScpi::setPskState(int value)
{
	QString cmdStr = "";
	bool tempValue = value == 1;

	//关闭解调状态
	closeDemodState();

	//关闭扫描显示
	closeSweepDisplay();

	sysData.digital.psk.state = tempValue;

	if(tempValue)
	{
		cmdStr = ":PSK:STATe ON;\r\n";
	} else
	{
		cmdStr = ":PSK:STATe OFF;\r\n";
	}

	sendSerialData(cmdStr);

	if(isNeedCloseDemod)
	{
		savePowerOffState();
	}

	return __SCPI_SUCCESS;
}

int tSysScpi::setPskState(QString value)
{
	bool isOk = false;
	bool tempValue = 0;

	tempValue = getBoolValue(value, &isOk);

	if (isOk)
	{
		return setPskState(tempValue);
	}
	else
	{
		return __SCPI_FAILED;
	}
}

int tSysScpi::setPskMode(int value)
{
	int tempValue = value;
	QString cmdStr = "";
	QString parameter = "";

	if(tempValue < 0)
	{
		tempValue = 0;
	} else if(tempValue > 3)
	{
		tempValue = 3;
	}

	sysData.digital.psk.mode = pskModeDef(tempValue);

	switch (tempValue)
	{
		case pskmBpsk: parameter = "2"; break;
		case pskmQpsk: parameter = "4"; break;
		case pskmPI4Psk: parameter = "5"; break;
		case pskm8: parameter = "8"; break;
		default: break;
	}

	cmdStr = ":PSK:TYPE " + parameter + ";\r\n";

	sendSerialData(cmdStr);

	if(isNeedCloseDemod)
	{
		savePowerOffState();
	}

	return __SCPI_SUCCESS;
}

int tSysScpi::setPskMode(QString value)
{
	bool isOk = false;
	QString tempValue = value.trimmed().toUpper();

	if(tempValue == "BPSK")
	{
		return setPskMode(pskmBpsk);
	} else if(tempValue == "QPSK")
	{
		return setPskMode(pskmQpsk);
	} else if(tempValue == "PI/4")
	{
		return setPskMode(pskmPI4Psk);
	} else if(tempValue == "8BSK")
	{
		return setPskMode(pskm8);
	}

	return __SCPI_FAILED;
}

int tSysScpi::setPskRate(double value)
{
	double tempValue = value;
	QString cmdStr = "";
	char temp[128] =
	{};
	char tempChar[128] =
	{};
	if(tempValue < MINPSKRATE)
	{
		tempValue = MINPSKRATE;
	} else if (tempValue > MAXPSKRATE)
	{
		tempValue = MAXPSKRATE;
	}

	sysData.digital.psk.rate = tempValue;

	//get input paramter
	strcpy(sysData.name, "PSK Rate:");
	strcpy(tempChar, getFreqString(sysData.digital.psk.rate, temp));
	insertThousands(tempChar);
	strcpy(sysData.value, tempChar);
	strcpy(sysData.unit, freqToQUnit(sysData.digital.psk.rate).trimmed().toStdString().c_str());

	cmdStr = ":PSK:INTernal:FUNCtion:FREQuency " + doubleToString( sysData.digital.psk.rate) + ";\r\n";

	sendSerialData(cmdStr);

	setPskState(1);

	if(isNeedCloseDemod)
	{
		savePowerOffState();
	}

	return __SCPI_SUCCESS;
}

int tSysScpi::setPskRate(QString value)
{
	bool isOk = false;
	double tempValue = 0;

	tempValue = getDoubleValue(value, &isOk);

	if (isOk)
	{
		return setPskRate(tempValue);
	}
	else
	{
		return __SCPI_FAILED;
	}
}

int tSysScpi::setPskRateWithRpg(int value)
{
	int tempValue = value;

	return setPskRate(setValueByRPG(sysData.digital.psk.rate, tempValue, 0));
}

#endif

//Pluse
int tSysScpi::setPluseState(int value)
{
	QString cmdStr = "";
	bool tempValue = value == 1;

	//关闭解调状态
	//  closeDemodState();

	//关闭扫描显示
	closeSweepDisplay();

	sysData.pluse.state = tempValue;

	if (tempValue)
	{
		cmdStr = ":PLUSE:STAT 1";
	} else
	{
		cmdStr = ":PLUSE:STAT 0";
	}

	sendSerialData(cmdStr);

//	if (isNeedCloseDemod)
//	{
//		savePowerOffState();
//	}

	return __SCPI_SUCCESS;
}

int tSysScpi::setPluseState(QString value)
{
	bool isOk = false;
	bool tempValue = 0;

	tempValue = getBoolValue(value, &isOk);

	if (isOk)
	{
		return setPluseState(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置脉冲源
int tSysScpi::setPluseSource(int value)
{
	QString cmdStr = "";
	QString parameter;
	int tempValue = value;

	//关闭解调状态
	//  if(sysData.pluse.state)
	//  {
	//	  closeDemodState();
	//	  sysData.pluse.state = true;
	//  }else
	//  {
	//	  closeDemodState();
	//  }

	//关闭扫描显示
	closeSweepDisplay();
	if (tempValue < 0)
		tempValue = 0;
	else if (tempValue > 1)
		tempValue = 1;

	sysData.pluse.plusesource = pluseSourceDef(tempValue);

	switch (tempValue)
	{
		case psInternal:
			parameter = "0";
			break;
		case psExternal:
			parameter = "1";
			break;

		default:
			break;
	}
	cmdStr = ":PLUSE:SOURCE " + parameter;

	sendSerialData(cmdStr);

//	if (isNeedCloseDemod)
//	{
//		savePowerOffState();
//	}

	return __SCPI_SUCCESS;
}

int tSysScpi::setPluseSource(QString value)
{
	bool isOk = false;
	QString tempValue = value.trimmed().toUpper();

	if (tempValue == "0")
	{
		return setPluseSource(0);
	} else if (tempValue == "1")
	{
		return setPluseSource(1);
	}

	return __SCPI_FAILED;
}

//设置脉冲触发方式
int tSysScpi::setPluseMethod(int value)
{
	QString cmdStr = "";
	QString parameter;
	int tempValue = value;

	//关闭解调状态
	//  if(sysData.pluse.state)
	//  {
	//	  closeDemodState();
	//	  sysData.pluse.state = true;
	//  }else
	//  {
	//	  closeDemodState();
	//  }

	//关闭扫描显示
	closeSweepDisplay();
	if (tempValue < 0)
		tempValue = 0;
	else if (tempValue > 1)
		tempValue = 1;

	sysData.pluse.plusemethod = pluseMethodDef(tempValue);

	switch (tempValue)
	{
		case plmAuto:
			parameter = "0";
			break;
		case plmTrigg:
			parameter = "1";
			break;
		default:
			break;
	}
	cmdStr = ":PLUSE:TRIGGER " + parameter;

	sendSerialData(cmdStr);

//	if (isNeedCloseDemod)
//	{
//		savePowerOffState();
//	}

	return __SCPI_SUCCESS;
}

int tSysScpi::setPluseMethod(QString value)
{
	bool isOk = false;
	QString tempValue = value.trimmed().toUpper();

	if (tempValue == "0")
	{
		return setPluseMethod(0);
	} else if (tempValue == "1")
	{
		return setPluseMethod(1);
	}

	return __SCPI_FAILED;
}

int tSysScpi::setPlusePeriod(double value)
{
	double tempValue = value;
	QString cmdStr = "";
	char temp[128] = { };
	char tempChar[128] = { };
	if (tempValue < MINPLUSEPERIOD)
	{
		tempValue = MINPLUSEPERIOD;
	} else if (tempValue > MAXPLUSEPERIOD)
	{
		tempValue = MAXPLUSEPERIOD;
	}

	sysData.pluse.period = tempValue * 1e-6;

	//设置周期时，宽度需要做相应的调整，当修改后的周期周期小于宽度时，需要将宽度设置成（周期-100ns）
	if (sysData.pluse.period <= sysData.pluse.width)
	{
		sysData.pluse.width = (tempValue - 100) * 1e-6;
		//	  cmdStr = ":PLUSE:INT:FUNC:WIDT " + doubleToString(sysData.pluse.width * 1e3);
		//	  sendSerialData(cmdStr);
	}

	//get paramter
	strcpy(sysData.name, "Pluse Period:");
	strcpy(sysData.value, floatToStringDot3(sysData.pluse.period, temp));
	strcpy(sysData.unit, timeToQUnit(sysData.pluse.period).trimmed().toStdString().c_str());

	//  cmdStr = ":PLUSE:INT:FUNC:PERI " + doubleToString(sysData.pluse.period * 1e3);		//原本的处理方法

	//由于田兴龙要求大于1S时，下发时就不考虑小数点后2位
	if (sysData.pluse.period >= 1000)
	{
		char tempchar[64] = { };
		sprintf(tempchar, "%.0f", sysData.pluse.period * 1e3);
		cmdStr = ":PLUSE:INT:FUNC:PERI " + QString(tempchar);
	} else
	{
		cmdStr = ":PLUSE:INT:FUNC:PERI " + doubleToString(sysData.pluse.period * 1e3);
	}
	sendSerialData(cmdStr);

	//  setPluseState(1);

//	if (isNeedCloseDemod)
//	{
//		savePowerOffState();
//	}

	return __SCPI_SUCCESS;
}

int tSysScpi::setPlusePeriod(QString value)
{
	bool isOk = false;
	double tempValue = 0;

	tempValue = getTimeValue(value, &isOk);
	if (isOk)
	{
		return setPlusePeriod(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

int tSysScpi::setPlusePeriodWithRpg(int value)
{
	int tempValue = value;

	return setPlusePeriod(setValueByRPG(sysData.pluse.period, tempValue, 6) * 1e6);
}

int tSysScpi::setPluseWidth(double value)
{
	char temp[128] = { };
	char tempChar[128] = { };

	double tempValue = value;
	QString cmdStr = "";

	if (tempValue < MINPLUSEWIDTH)
	{
		tempValue = MINPLUSEWIDTH;
	} else if (tempValue > MAXPLUSEWIDTH)
	{
		tempValue = MAXPLUSEWIDTH;
	}

	sysData.pluse.width = tempValue * 1e-6;
	//设置宽度时，宽度需要做相应的调整，当修改后的宽度大于周期时，需要将周期设置成（宽度+100ns）
	if (sysData.pluse.width >= sysData.pluse.period)
	{
		sysData.pluse.period = (tempValue + 100) * 1e-6;
		//	  cmdStr = ":PLUSE:INT:FUNC:PERI " + doubleToString(sysData.pluse.period * 1e3);
		//	  sendSerialData(cmdStr);
	}

	//get paramter
	strcpy(sysData.name, "Pluse Width:");
	strcpy(sysData.value, floatToStringDot3((sysData.pluse.width), temp));
	strcpy(sysData.unit, timeToQUnit(sysData.pluse.width).trimmed().toStdString().c_str());

	//  cmdStr = ":PLUSE:INT:FUNC:WIDT " + doubleToString(sysData.pluse.width * 1e3);		//原本的处理方法

	//由于田兴龙要求大于1S时，下发时就不考虑小数点后2位
	if (sysData.pluse.width >= 1000)
	{
		char tempchar[64] = { };
		sprintf(tempchar, "%.0f", sysData.pluse.width * 1e3);
		cmdStr = ":PLUSE:INT:FUNC:WIDT " + QString(tempchar);
	} else
	{
		cmdStr = ":PLUSE:INT:FUNC:WIDT " + doubleToString(sysData.pluse.width * 1e3);
	}

	sendSerialData(cmdStr);

	//  setPluseState(1);

//	if (isNeedCloseDemod)
//	{
//		savePowerOffState();
//	}

	return __SCPI_SUCCESS;
}

int tSysScpi::setPluseWidth(QString value)
{
	bool isOk = false;
	double tempValue = 0;

	tempValue = getTimeValue(value, &isOk);

	if (isOk)
	{
		return setPluseWidth(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

int tSysScpi::setPluseWidthWithRpg(int value)
{
	int tempValue = value;

	return setPluseWidth(setValueByRPG(sysData.pluse.width, tempValue, 6) * 1e6);
}

//载入用户参数
//int tSysScpi::loadUserState(void)
//{
//  QString fileName = QCoreApplication::applicationDirPath() + "/user.dat";
//
//  FILE *fp;
//  fp = fopen(fileName.toStdString().c_str(), "rb");
//  if (fp != NULL)
//  {
//	fread(&sysData, sizeof sysData, 1, fp);
//	fclose(fp);
//  }
//
//  return _RESULT_SUCCESS;
//}

//保存用户参数
//int tSysScpi::saveUserState(void)
//{
//  QString fileName = QCoreApplication::applicationDirPath() + "/user.dat";
//  FILE *fp;
//  fp = fopen(fileName.toStdString().c_str(), "wb");
//
//  if (fp != NULL)
//  {
//	fwrite(&sysData, sizeof sysData, 1, fp);
//	fclose(fp);
//  }
//
//  return _RESULT_SUCCESS;
//}

//载入关机参数
int tSysScpi::loadPowerOffState(void)
{
	QString fileName = QCoreApplication::applicationDirPath() + "/poweroff.dat";

	if (QFile(fileName).size() == sizeof sysPowerOffData) //如果文件一致则正常加载
	{
		FILE *fp;
		fp = fopen(fileName.toStdString().c_str(), "rb");
		memset(&sysPowerOffData, 0, sizeof sysPowerOffData);

		if (fp != NULL)
		{
			fread(&sysPowerOffData, sizeof sysPowerOffData, 1, fp);
			memcpy(&sysData, &sysPowerOffData, sizeof sysPowerOffData); //将关机参数复制给系统参数
			fclose(fp);
		}
		return _RESULT_SUCCESS;
	} else
	{
		return _RESULT_FAILED;
	}
}

//保存关机参数
int tSysScpi::savePowerOffState(void)
{
	// QTime t = QTime::currentTime();
	// t.restart();
	memset(&sysPowerOffData, 0, sizeof sysPowerOffData);

	QString fileName = QCoreApplication::applicationDirPath() + "/poweroff.dat";

	FILE *fp;
	fp = fopen(fileName.toStdString().c_str(), "wb");

	if (fp != NULL)
	{
		memcpy(&sysPowerOffData, &sysData, sizeof sysPowerOffData); //将系统参数复制给关机参数
		fwrite(&sysPowerOffData, sizeof sysPowerOffData, 1, fp);
		fclose(fp);
	}

	//  printf("%d ms\n",t.elapsed());

	return _RESULT_SUCCESS;
}

//设置IP地址
int tSysScpi::setLanIP(QString value)
{
	strcpy(sysData.system.io.lan.ip, value.toStdString().c_str());

	strcpy(sysData.name, "IP:");
	strcpy(sysData.value, QString(sysData.system.io.lan.ip).trimmed().toStdString().c_str());
	strcpy(sysData.unit, "");

	saveDeviceConfig();
	resetSystemIOOfLan();

	return __SCPI_SUCCESS;
}

//设置子网掩码地址
int tSysScpi::setLanMask(QString value)
{
	strcpy(sysData.system.io.lan.mask, value.toStdString().c_str());

	strcpy(sysData.name, "MASK:");
	strcpy(sysData.value, QString(sysData.system.io.lan.mask).trimmed().toStdString().c_str());
	strcpy(sysData.unit, "");

	saveDeviceConfig();
	resetSystemIOOfLan();

	return __SCPI_SUCCESS;
}

//设置网关地址
int tSysScpi::setLanGate(QString value)
{
	strcpy(sysData.system.io.lan.gate, value.toStdString().c_str());

	strcpy(sysData.name, "Gate:");
	strcpy(sysData.value, QString(sysData.system.io.lan.gate).trimmed().toStdString().c_str());
	strcpy(sysData.unit, "");

	saveDeviceConfig();
	resetSystemIOOfLan();

	return __SCPI_SUCCESS;
}

//设置系统语言
int tSysScpi::setSystemLanguage(int value)
{
	sysData.system.isLanguageEn = value == 1;

	//  if (sysData.system.isLanguageEn)
	//  {
	//    saveSystemSetting("system", "language", "english");
	//  } else
	//  {
	//  	saveSystemSetting("system", "language", "chinese");
	//  }

	saveDeviceConfig();
	getSystemFont();

	return __SCPI_SUCCESS;
}

//设置系统语言
int tSysScpi::setSystemLanguage(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	QString tempValue = value.toUpper().trimmed();

	if (tempValue == "ENGL" || tempValue == "ENGLISH")
	{
		return setSystemLanguage(1);
	} else if (tempValue == "CHIN" || tempValue == "CHINESE")
	{
		return setSystemLanguage(0);
	} else
	{
		return __SCPI_FAILED;
	}
}

//获取LAN物理地址
QStringList tSysScpi::getLanRealIPAndAddress(void)
{
	bool exeSuccess = false;
	char buf[BUFSIZ];
	FILE *read_fp;
	int chars_read;
	int ret;
	QStringList result;

	memset(buf, 0, BUFSIZ);
	read_fp = popen("ifconfig eth0", "r");

	if (read_fp != NULL)
	{
		chars_read = fread(buf, sizeof(char), BUFSIZ - 1, read_fp);
		exeSuccess = chars_read > 0;
		pclose(read_fp);
	} else
	{
		exeSuccess = false;
	}

	if (exeSuccess)
	{
		QString lanStr = QString(buf).trimmed();
		QStringList lanStrList = lanStr.split(" ");
		QString lineStr = "";

		for (int i = 0; i < lanStrList.count(); i++)
		{
			if (!lanStrList.at(i).isEmpty())
			{
				lineStr = lanStrList.at(i).toUpper().trimmed();

				if (lineStr.isEmpty())
				{
					continue;
				}

				if (lineStr == "HWADDR" && i < lanStrList.count() - 1)
				{
					result << lanStrList.at(i + 1);
				} else if (lineStr.length() > 10 && lineStr.mid(0, 4) == "ADDR")
				{
					result << lineStr.mid(5, lineStr.length() - 5);
				} else if (lineStr.length() > 10 && lineStr.mid(0, 4) == "MASK")
				{
					result << lineStr.mid(5, lineStr.length() - 5);
				} else if (lineStr.length() == 7 && lineStr.mid(0, 7) == "RUNNING")
				{
					result << "UP";
				}
			}
		}
	}

	return result;
}

//设置LAN
int tSysScpi::resetSystemIOOfLan(void)
{
	QStringList strList;
	char temp[64] = { };

	sprintf(temp, "%x%x:%x%x:%x%x:%x%x:%x%x:%x%x", sysData.system.lanMacAddress[0], sysData.system.lanMacAddress[1], sysData.system.lanMacAddress[4], sysData.system.lanMacAddress[5],
			sysData.system.lanMacAddress[2], sysData.system.lanMacAddress[3], sysData.system.lanMacAddress[6], sysData.system.lanMacAddress[7], sysData.system.lanMacAddress[8],
			sysData.system.lanMacAddress[9], sysData.system.lanMacAddress[10], sysData.system.lanMacAddress[11]);

	strList.clear();
	strList << "ifconfig eth0 " + QString(sysData.system.io.lan.ip).trimmed() + " netmask " + QString(sysData.system.io.lan.mask);
	strList << "ifconfig eth0 hw ether " + QString(temp).trimmed();
	strList << "route add default gw " + QString(sysData.system.io.lan.gate).trimmed();

	for (int i = 0; i < strList.count(); i++)
	{
		system(strList.at(i).toStdString().c_str());
	}

	return __SCPI_SUCCESS;
}

//设置开机参数
int tSysScpi::setSystemPoweronParam(int value)
{
	return __SCPI_SUCCESS;
}

//设置开机参数
int tSysScpi::setSystemPoweronParam(QString value)
{
	return __SCPI_SUCCESS;
}

//设置SCPI屏幕刷新
int tSysScpi::setSystemScpiScreenRefresh(int value)
{
	sysData.system.isScpiScreenRefresh = value == 1;
	saveDeviceConfig();

	return __SCPI_SUCCESS;
}

//设置SCPI屏幕刷新
int tSysScpi::setSystemScpiScreenRefresh(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	int tempValue = getBoolValue(value.toUpper().trimmed(), &isOk);

	if (isOk)
	{
		return setSystemScpiScreenRefresh(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

#if 0
//设置串行口  原来的
int tSysScpi::resetSystemIOOfSerial(void)
{
	if (comHandle <= 0)
	{
		return __SCPI_FAILED;
	}

	struct termios options;

	//get attributes
	tcgetattr(comHandle, &options);

	//baudrate
	cfsetispeed(&options, B115200);
	cfsetospeed(&options, B115200);

	//enable receiver and set local mode
	//   options.c_oflag |= ~ONLCR;    			//zpc 注销
	options.c_cflag |= (CLOCAL | CREAD);
	options.c_lflag &= ~(ICANON | ECHO);

	//databits
	options.c_cflag &= ~CSIZE;
	options.c_cflag |= CS8;

	//check
	options.c_cflag &= ~PARENB;
	options.c_iflag &= ~INPCK;

	//stopbits
	options.c_cflag &= ~CSTOPB;

	//flow control
	options.c_cflag &= ~CRTSCTS;
	//time
	options.c_cc[VMIN] = 0;
	options.c_cc[VTIME] = 0;

	tcflush(comHandle, TCIFLUSH);

	if (tcsetattr(comHandle, TCSANOW, &options) == 0)
	{
		return __SCPI_SUCCESS;
	} else
	{
		return __SCPI_FAILED;
	}
}
#endif

//设置串行口  新的
int tSysScpi::resetSystemIOOfSerial(void)
{
	if (comHandle <= 0)
	{
		return __SCPI_FAILED;
	}

	struct termios options;

	//get attributes
	tcgetattr(comHandle, &options);

	//baudrate
	cfsetispeed(&options, B115200);
	cfsetospeed(&options, B115200);

	//output flag control(prevent 0x0a -> 0x0d 0x0a)
	options.c_oflag &= ~(INLCR | IGNCR | ICRNL);
	options.c_oflag &= ~(ONLCR | OCRNL);

	//enable receiver and set local mode
	options.c_cflag |= (CLOCAL | CREAD);
	options.c_lflag &= ~(ICANON | ECHO);

	//databits
	options.c_cflag &= ~CSIZE;
	options.c_cflag |= CS8;

	//check
	options.c_cflag &= ~PARENB;
	options.c_iflag &= ~INPCK;

	//stopbits
	options.c_cflag &= ~CSTOPB;

	//flow control
	options.c_cflag &= ~CRTSCTS;
	//time
	options.c_cc[VMIN] = 0;
	options.c_cc[VTIME] = 0;

	tcflush(comHandle, TCIFLUSH);

	if (tcsetattr(comHandle, TCSANOW, &options) == 0)
	{
		return __SCPI_SUCCESS;
	} else
	{
		return __SCPI_FAILED;
	}
}
//设置序列号
int tSysScpi::setSystemServiceOfSerialNo(QString value)
{
	for (int i = 0; i < value.length(); i++)
	{
		if (value.at(i) < '0' || value.at(i) > '9')
		{
			return __SCPI_FAILED;
		}
	}

	if (value == "000000000000")
	{
		return __SCPI_FAILED;
	}

	QString tempValue = "";
	QString cmd = ":FACT:IDN:CPN ";
	QString cmdStr = "";

	if (value.isNull() || value.isEmpty() || value.length() != 12)
	{
		return __SCPI_FAILED;
	}

	tempValue = "BL" + value.toUpper().trimmed();

	//  cmdStr = cmd + tempValue + ";\r\n";
	//  printf("%s",cmdStr.toStdString().c_str());
	cmdStr = cmd + tempValue;
	sendSerialData(cmdStr);

	strcpy(sysData.system.serialNo, tempValue.toStdString().c_str());
	saveDeviceConfig();

	return __SCPI_SUCCESS;
}

//设置网卡物理地址
int tSysScpi::setSystemServiceOfLanMacAddress(QString value)
{
	if (value.isNull() || value.isEmpty() || value.length() != sizeof sysData.system.lanMacAddress / sizeof sysData.system.lanMacAddress[0])
	{
		return __SCPI_FAILED;
	}

	char xtemp;

	for (int i = 0; i < sizeof sysData.system.lanMacAddress / sizeof sysData.system.lanMacAddress[0]; i++)
	{
		switch (value.at(i).toAscii())
		{
			case '0':
				sysData.system.lanMacAddress[i] = 0;
				break;
			case '1':
				sysData.system.lanMacAddress[i] = 1;
				break;
			case '2':
				sysData.system.lanMacAddress[i] = 2;
				break;
			case '3':
				sysData.system.lanMacAddress[i] = 3;
				break;
			case '4':
				sysData.system.lanMacAddress[i] = 4;
				break;
			case '5':
				sysData.system.lanMacAddress[i] = 5;
				break;
			case '6':
				sysData.system.lanMacAddress[i] = 6;
				break;
			case '7':
				sysData.system.lanMacAddress[i] = 7;
				break;
			case '8':
				sysData.system.lanMacAddress[i] = 8;
				break;
			case '9':
				sysData.system.lanMacAddress[i] = 9;
				break;
			case 'a':
				sysData.system.lanMacAddress[i] = 10;
				break;
			case 'A':
				sysData.system.lanMacAddress[i] = 10;
				break;
			case 'b':
				sysData.system.lanMacAddress[i] = 11;
				break;
			case 'B':
				sysData.system.lanMacAddress[i] = 11;
				break;
			case 'c':
				sysData.system.lanMacAddress[i] = 12;
				break;
			case 'C':
				sysData.system.lanMacAddress[i] = 12;
				break;
			case 'd':
				sysData.system.lanMacAddress[i] = 13;
				break;
			case 'D':
				sysData.system.lanMacAddress[i] = 13;
				break;
			case 'e':
				sysData.system.lanMacAddress[i] = 14;
				break;
			case 'E':
				sysData.system.lanMacAddress[i] = 14;
				break;
			case 'f':
				sysData.system.lanMacAddress[i] = 15;
				break;
			case 'F':
				sysData.system.lanMacAddress[i] = 15;
				break;
		}
	}

	saveDeviceConfig();
	/*
	 QFile file("/etc/net.conf");
	 QString line;
	 QStringList strList;

	 if (file.open(QIODevice::ReadOnly))
	 {
	 QTextStream text(&file);

	 while (!text.atEnd())
	 {
	 line = text.readLine();

	 if (!line.isNull() && !line.isEmpty())
	 {
	 strList << line;
	 }
	 }

	 text.flush();
	 file.close();
	 file.remove();
	 }
	 */
	QString macString = "";
	char tempChar[8] = { };

	for (int i = 0; i < sizeof sysData.system.lanMacAddress / sizeof sysData.system.lanMacAddress[0]; i++)
	{
		if (i > 0 && (i % 2) == 0)
		{
			macString += ":";
		}

		macString += QString(intToHex(sysData.system.lanMacAddress[i], tempChar)).trimmed();
	}

	QString strCmd = "";

	strCmd = "ifconfig eth0 hw ether " + macString.trimmed();
	system(strCmd.toStdString().c_str());
	/*
	 for (int i = 0; i < strList.count(); i++)
	 {
	 if (strList.at(i).toUpper().indexOf("MAC", 0) >= 0)
	 {
	 strList.replace(i, "MAC=" + macString);
	 break;
	 }
	 }

	 if (file.open(QIODevice::WriteOnly))
	 {
	 QTextStream text(&file);

	 for (int i = 0; i < strList.count(); i++)
	 {
	 text << strList.at(i) << "\n";
	 }

	 text.flush();
	 file.close();
	 }

	 system("chmod 777 /etc/net.conf");
	 */
	return __SCPI_SUCCESS;
}

//方向键设置网卡物理地址
int tSysScpi::setSystemServiceOfLanMacAddressWidthArrow(bool up)
{
	unsigned char value = sysData.system.lanMacAddress[sysData.system.lanMacAddrIndex];
	QString tempValue = "";

	if (up)
	{
		if (value < 0xf)
		{
			value++;
		} else
		{
			value = 0x0;
		}
	} else
	{
		if (value > 0x0)
		{
			value--;
		} else
		{
			value = 0xf;
		}
	}

	sysData.system.lanMacAddress[sysData.system.lanMacAddrIndex] = value;
	saveDeviceConfig();

	QString macString = "";
	char tempChar[8] = { };

	for (int i = 0; i < sizeof sysData.system.lanMacAddress / sizeof sysData.system.lanMacAddress[0]; i++)
	{
		if (i > 0 && (i % 2) == 0)
		{
			macString += ":";
		}

		macString += QString(intToHex(sysData.system.lanMacAddress[i], tempChar)).trimmed();
		tempValue += QString(intToHex(sysData.system.lanMacAddress[i], tempChar)).trimmed(); //给sysData.value赋值，不用加“：”
	}

	QString strCmd = "";

	strCmd = "ifconfig eth0 hw ether " + macString.trimmed();
	system(strCmd.toStdString().c_str());

	/*
	 QFile file("/etc/net.conf");
	 QString line;
	 QStringList strList;

	 if (file.open(QIODevice::ReadOnly))
	 {
	 QTextStream text(&file);

	 while (!text.atEnd())
	 {
	 line = text.readLine();

	 if (!line.isNull() && !line.isEmpty())
	 {
	 strList << line;
	 }
	 }

	 text.flush();
	 file.close();
	 file.remove();
	 }

	 QString macString = "";
	 char tempChar[8] = {};

	 for (int i = 0; i < sizeof sysData.system.lanMacAddress / sizeof sysData.system.lanMacAddress[0]; i++)
	 {
	 if (i > 0 && (i % 2) == 0)
	 {
	 macString += ":";
	 }

	 macString += QString(intToHex(sysData.system.lanMacAddress[i], tempChar)).trimmed();
	 }

	 for (int i = 0; i < strList.count(); i++)
	 {
	 if (strList.at(i).toUpper().indexOf("MAC", 0) >= 0)
	 {
	 strList.replace(i, "MAC=" + macString);
	 break;
	 }
	 }

	 if (file.open(QIODevice::WriteOnly))
	 {
	 QTextStream text(&file);

	 for (int i = 0; i < strList.count(); i++)
	 {
	 text << strList.at(i) << "\n";
	 }

	 text.flush();
	 file.close();
	 }

	 system("chmod 777 /etc/net.conf");
	 */
	strcpy(sysData.name, "Mac Address:");
	strcpy(sysData.value, tempValue.toStdString().c_str());
	strcpy(sysData.unit, "");
	return __SCPI_SUCCESS;
}

//方向键设置网卡物理地址
int tSysScpi::setSystemServiceOfLanMacAddressWidthRpg(int value)
{
	return setSystemServiceOfLanMacAddressWidthArrow(value > 0);
}

//获取系统字体
void tSysScpi::getSystemFont(void)
{
	if (sysData.system.isLanguageEn)
	{
		fontBold = fontEnglishBold;
		fontLight = fontEnglishLight;
		fontData = fontLight;
		fontDataBig = fontParaEnglishBold;
		fontSmall = fontData;
		fontDataSmall = fontEnglishSmall;
		fontMarker = fontMarkerLight;
		fontCalibration = fontCalen;

		fontWin1 = fontWin1En;
		fontWin2 = fontWin2En;
		fontWin3 = fontWin3En2;
		fontWin4 = fontWin4En;
	} else
	{
		fontBold = fontChinese;
		fontLight = fontChinese;
		fontData = fontEnglishLight;
		fontDataBig = fontParaChinese;
		fontSmall = fontData;
		fontDataSmall = fontEnglishSmall;
		fontMarker = fontMarkerBold;
		fontCalibration = fontCalch;

		fontWin1 = fontWin1Ch;
		fontWin2 = fontWin2Ch;
		fontWin3 = fontWin3En2;
		fontWin4 = fontWin4En;
	}

	//fontDataBig.setPointSize(fontBold.pointSize() + 1);
	fontData.setPointSize(fontData.pointSize() - 1);
	fontSmall.setPointSize(fontData.pointSize() - 2);
	fontInput = fontMarker;
	fontInput.setPointSize(fontInput.pointSize() + 3);
	fontInput.setBold(true);
	fontUnderline = fontLight;
	fontUnderline.setUnderline(true);
}

//判断是否开始绘制列表process
bool tSysScpi::isStartToDrawListProcess(void)
{
	return ((sysData.sweep.mode != smOff) && ((sysData.sweep.type) == stList) && (sysData.sweep.list.allRow > 1) && (((sysData.sweep.repeat == srSingle) && sysData.isBeginDrawSingleProcess)
			|| (sysData.sweep.repeat == srConutinuos)));
}

//判断是否开始绘制process
bool tSysScpi::isStartToDrawStepProcess(void)
{
	//  printf("isBeginDrawSingleProcess = %d\n", sysData.isBeginDrawSingleProcess);
	return ((sysData.sweep.mode != smOff) && ((sysData.sweep.type) == stStep) && (((sysData.sweep.repeat == srSingle) && sysData.isBeginDrawSingleProcess) || (sysData.sweep.repeat == srConutinuos)));
}

//复位系统数据--参数初始值
void tSysScpi::presetSystemData(void)
{
	QString SendToMSG1030QS;
	char SendToMSG1030CH[50];

	isRcvingSequence = false;

	sysData.indexPoints = 0;
	sysData.inputLength = 10;
	sysData.cursorDirection = UP;
	sysData.cursorLocation = 0;

	sysData.process = 0;
	sysData.isBeginDrawSingleProcess = false;

	sysData.keyBoardSwitchRF = 0;
	sysData.keyBoardSwitchLF = 0;
	sysData.keyBoardSwitchMod = 0;

	//freq
	sysData.freq.freq = MAXFREQ;
	sysData.freq.prevValue = MAXFREQ;
	sysData.freq.downLoad = sysData.freq.freq;
	sysData.freq.offset = 0;
	sysData.freq.ref.state = false;
	sysData.freq.ref.value = 0;
	sysData.freq.multiplier = 1;
	sysData.freq.ref10MHz = rmInternal;

	//  //system name&unit
	strcpy(sysData.name, "");
	strcpy(sysData.value, "");
	strcpy(sysData.unit, "");

	//ampl
	//  sysData.ampl.amplinit = MINAMPL;
	//  sysData.ampl.amplinitUnit = audBm;
	sysData.ampl.ampl = MINAMPL;
	sysData.ampl.downLoad = MINAMPL;
	sysData.ampl.prevValue = MINAMPL;
	sysData.ampl.setValue = -120.00;
	sysData.ampl.offset = 0;
	sysData.ampl.ref.state = false;
	sysData.ampl.ref.value = 0;
	sysData.ampl.error.value = 0;
	sysData.ampl.error.isNeeded = false;
	sysData.ampl.alc.state = false;
	sysData.ampl.alc.response = true;
	sysData.ampl.amplUnit = audBm;

	//sweep
	sysData.sweep.nowpagenum = 1;
	sysData.sweep.mode = smOff;
	//  sysData.sweep.state = false;
	//  sysData.sweep.freqstate = false;
	//  sysData.sweep.amplstate = false;
	//  sysData.sweep.mode = sweepModeDef(smFreq);
	sysData.sweep.type = sweepTypeDef(stStep);
	sysData.sweep.repeat = sweepRepeatDef(srSingle);
	sysData.sweep.retrace = sweepRetraceDef(srOff);
	sysData.sweep.step.freqStart = sysData.freq.freq;
	sysData.sweep.step.freqStop = sysData.freq.freq;
	sysData.sweep.step.amplStart = sysData.ampl.ampl;
	sysData.sweep.step.amplStop = sysData.ampl.ampl;
	sysData.sweep.direction = sdUp;
	sysData.sweep.trig = stAuto;
	sysData.sweep.step.point = 20;
	sysData.sweep.step.dwell = 20;
	memset(&sysData.sweep.list, 0, sizeof(sysData.sweep.list));

	sysData.sweep.list.allRow = 2;
	sysData.sweep.list.selectRow = 0;
	sysData.sweep.list.selectLine = 0;
	sysData.sweep.list.allIndex = 0;
	sysData.sweep.list.displayPage = 0;
	sysData.sweep.list.allPage = 1;
	sysData.sweep.list.data[0].freq = MAXFREQ;
	sysData.sweep.list.data[0].ampl = MINAMPL;
	sysData.sweep.list.data[0].dwell = MINDWELL;
	sysData.sweep.list.data[1].freq = MAXFREQ;
	sysData.sweep.list.data[1].ampl = MINAMPL;
	sysData.sweep.list.data[1].dwell = MINDWELL;

	//analog demod
	sysData.analog.source = astInternal;
	//AM
	sysData.analog.am.state = false;
	sysData.analog.am.depth = 50.00;
	sysData.analog.am.rate = 25e3;

	//FM
	sysData.analog.fm.state = false;
	sysData.analog.fm.deviation = 1000e3;
	sysData.analog.fm.rate = 25e3;

	//PM
	sysData.analog.pm.state = false;
	sysData.analog.pm.phase = 0;
	sysData.analog.pm.rate = 25e3;

	//Pluse
	//  sysData.analog.pluse.state = false;
	//  sysData.analog.pluse.period = 400;
	//  sysData.analog.pluse.width = 200;

	//digital mode(原来的可以注销)
	//ASK
	sysData.digital.ask.state = false;
	sysData.digital.ask.mode = askm2;
	sysData.digital.ask.rate = 1;

	//FSK
	sysData.digital.fsk.state = false;
	sysData.digital.fsk.mode = fskm2;
	sysData.digital.fsk.rate = 10e3;
	sysData.digital.fsk.step = 10e3;

	//PSK
	sysData.digital.psk.state = false;
	sysData.digital.psk.mode = pskm8;
	sysData.digital.psk.rate = 1;

	//digital mode
	sysData.digital.digitalstate = 0;
	sysData.digital.source = dstInternal;
	sysData.digital.digitalpara.depth = 50.00;
	sysData.digital.digitalpara.deviation = 1e6;
	sysData.digital.digitalpara.phase = 0;
	sysData.digital.digitalpara.rate = 10e3;
	sysData.digital.digitaltype = dt2ASK;

	//Pluse
	sysData.pluse.state = 0;
	sysData.pluse.plusemethod = plmAuto;
	sysData.pluse.period = 20;
	sysData.pluse.width = 10;
	sysData.pluse.plusesource = psInternal;

	//LF
	sysData.lf.state = false;
	sysData.lf.type = lotSine;
	sysData.lf.freq = 100e3;
	sysData.lf.ampl = 400;

	//ext RF
	sysData.exfRef.freq = 300e6;
	sysData.exfRef.type = erftInternal;

	//DRG
	sysData.drg.state = false;
	sysData.drg.uplimit = MAXFREQ;
	sysData.drg.downlimit = MAXFREQ - 20e6;
	sysData.drg.points = 100;
	sysData.drg.rate = 1e-3;
	sysData.drg.workmode = ddnormal;

	//system
	strcpy(sysData.system.io.lan.ip, "192.168.1.6");
	strcpy(sysData.system.io.lan.mask, "255.255.255.0");
	strcpy(sysData.system.io.lan.gate, "192.168.1.1");
	strcpy(sysData.system.io.lan.mac, "e6-6d-8d-a3-53-7b");

	getSystemFont();
	//
	//获取设备配置
	loadDeviceConfig();

}

//复位系统
int tSysScpi::presetSystem(void)
{
	presetSystemData();

	return __SCPI_SUCCESS;
}

//获取频率 显示值
void tSysScpi::getFreqDisplayValue(double value)
{
	double temp = value;

	//  strcpy(sysData.Freq.ValueChar, doubleToString(temp).toStdString().c_str());
}

//获取幅度  显示值
void tSysScpi::getAmplDisplayValue(double value)
{
	double temp = value;

	//  strcpy(sysData.Ampl.ValueChar, doubleToString(temp).toStdString().c_str());
}

void tSysScpi::closeDemodState()
{
	sysData.analog.am.state = false;
	sysData.analog.fm.state = false;
	sysData.analog.pm.state = false;
	sysData.digital.ask.state = false;
	sysData.digital.fsk.state = false;
	sysData.digital.psk.state = false;
	sysData.digital.digitalstate = false;
	//  sysData.pluse.state = false;
	sysData.drg.state = false;
	//  sysData.analog.pluse.state = false;
}

//关闭调制
void tSysScpi::closeDemod(void)
{
	closeDemodState();
	//  sysData.sweep.state = false;
	sysData.sweep.mode = smOff;

	//  setPmState(0);
}

void tSysScpi::closeSweepDisplay()
{
	//  sysData.sweep.state = false;
	sysData.sweep.mode = smOff;
}

//参数下发
void tSysScpi::controlParamterDown(void)
{

	char tempChar[64] = { 0 };
	QString cmdStr = "";
	int tempSweepMode = (int) sysData.sweep.mode;

	//10MHz
	setRef10MState(sysData.freq.ref10MHz);
	usleep(100 * 1000);

	sendSerialData("*RST");
	usleep(100 * 1000);

	//频率下发
	// freq
	setFrequency(sysData.freq.freq);
	//  usleep(100 * 1000);


	//幅度下发
	cmdStr = setAmplitude(sysData.ampl.ampl);
	//  usleep(100 * 1000);
	//ALC下发
	setALCState(sysData.ampl.alc.state);
	//  usleep(50 * 1000);

	//sweep
	//mode
	setSweepMode(tempSweepMode); //由于上面的下发频率和幅度时，可能会修改扫描模式，故需要从新设置扫描模式
	switch (sysData.sweep.mode)
	{
		case smFreq:
			setSweepMode(smFreq);
			break;
		case smAmpl:
			setSweepMode(smAmpl);
			break;
		case smFreqAmpl:
			setSweepMode(smFreqAmpl);
			break;
		default:
			break;
	}
	//  usleep(100 * 1000);
	if (sysData.sweep.mode != smOff)
	{
		//direction
		switch (sysData.sweep.direction)
		{
			case sdUp:
				setSweepDirection(sdUp);
				break;
			case sdDown:
				setSweepDirection(sdDown);
				break;
			default:
				break;
		}
		//    usleep(100 * 1000);

		//retrace
		switch (sysData.sweep.retrace)
		{
			case srOn:
				setSweepRetrace(srOn);
				break;
			case srOff:
				setSweepRetrace(srOff);
				break;
			default:
				break;
		}
		//    usleep(100 * 1000);

		if (sysData.sweep.type == stStep)
		{
			//start& stop  freq
			if (sysData.sweep.mode == smFreq || sysData.sweep.mode == smFreqAmpl)
			{
				//start freq
				setStepStartFrequency(sysData.sweep.step.freqStart);
				//    	usleep(100 * 1000);

				//stop freq
				setStepStopFrequency(sysData.sweep.step.freqStop);
				//    	usleep(100 * 1000);
			}

			if (sysData.sweep.mode == smAmpl || sysData.sweep.mode == smFreqAmpl)
			{
				//start ampl
				setStepStartAmplitude(sysData.sweep.step.amplStart);
				//	    usleep(100 * 1000);

				//stop ampl
				setStepStopAmplitude(sysData.sweep.step.amplStop);
				//	    usleep(100 * 1000);
			}
			//points
			setStepPoints(sysData.sweep.step.point);
			//      usleep(100 * 1000);

			//dwell
			setStepDwell(sysData.sweep.step.dwell);
			//      usleep(100 * 1000);
		}
		//    else if(sysData.sweep.type == stList)
		//    {
		//	  setSweepListDownload();
		//    }
		//repeat
		switch (sysData.sweep.repeat)
		{
			case srSingle:
				setSweepRepeat(srSingle);
				break;
			case srConutinuos:
				setSweepRepeat(srConutinuos);
				break;
			default:
				break;
		}
		//    usleep(100 * 1000);
	}

	if (sysData.sweep.type == stList)
	{
		setSweepListDownload();
	}

	//exf
	setExtRefType(sysData.exfRef.type);
	//  usleep(100 *1000);

	//exf freq
	setExtRefFrequency(sysData.exfRef.freq);
	//  usleep(100 *1000);

	//LF
	//type
	setLfType(sysData.lf.type);
	//  usleep(100 *1000);

	//freq
	setLfFreq(sysData.lf.freq);
	//  usleep(100 *1000);

	//ampl
	setLfAmpl(sysData.lf.ampl);
	//  usleep(100 *1000);
	//开关控制
	//mode output state
	keyBoardLEDSwitch(1, sysData.keyBoardSwitchMod);
	sprintf(tempChar, "%d", sysData.keyBoardSwitchMod);
	cmdStr = ":SYST:MODO " + QString(tempChar).trimmed();
	sendSerialData(cmdStr);
	//  usleep(100 *1000);

	//RF outPut state
	keyBoardLEDSwitch(3, sysData.keyBoardSwitchRF);
	sprintf(tempChar, "%d", sysData.keyBoardSwitchRF);
	cmdStr = ":SYST:RFO " + QString(tempChar).trimmed();
	sendSerialData(cmdStr);
	//  usleep(100 *1000);

	//LF outPut
	keyBoardLEDSwitch(2, sysData.keyBoardSwitchLF);
	sprintf(tempChar, "%d", sysData.keyBoardSwitchLF);
	cmdStr = ":SYST:LFO " + QString(tempChar).trimmed();
	sendSerialData(cmdStr);
	//  usleep(100 *1000);

	sysData.lf.state = sysData.keyBoardSwitchLF;
	//  usleep(1000 * 1000);

	//AM
	//  state
	if (sysData.analog.am.state)
	{
		setAnalogModulateSource(sysData.analog.source);
		setAmState(true);
		//	sysData.analog.am.state = true;
		//
		//	sprintf(tempChar, "%d",sysData.analog.am.state);
		//	cmdStr = ":AM:STAT " + QString(tempChar).trimmed();
		//	sendSerialData(cmdStr);
		//	usleep(300 * 1000);

		//depth
		sprintf(tempChar, "%.2f", sysData.analog.am.depth);
		cmdStr = ":AM:DEPT " + QString(tempChar).trimmed();
		sendSerialData(cmdStr);
		//	usleep(100 * 1000);

		//rate
		sprintf(tempChar, "%.0f", sysData.analog.am.rate);
		cmdStr = ":AM:INT:FUNC:FREQ " + QString(tempChar).trimmed();
		sendSerialData(cmdStr);
		//	usleep(100 * 1000);
	} else
	//FM
	//  state
	if (sysData.analog.fm.state)
	{
		setAnalogModulateSource(sysData.analog.source);
		setFmState(true);
		//	sysData.analog.fm.state = true;
		//
		//	sprintf(tempChar, "%d",sysData.analog.fm.state);
		//	cmdStr = ":FM:STAT " + QString(tempChar).trimmed();
		//	sendSerialData(cmdStr);
		//	usleep(300 * 1000);

		//频偏
		sprintf(tempChar, "%.0f", sysData.analog.fm.deviation);
		cmdStr = ":FM:DEV " + QString(tempChar).trimmed();
		sendSerialData(cmdStr);
		//	usleep(100 * 1000);

		//rate
		sprintf(tempChar, "%.0f", sysData.analog.fm.rate);
		cmdStr = ":FM:INT:FUNC:FREQ " + QString(tempChar).trimmed();
		sendSerialData(cmdStr);
		//	usleep(100 * 1000);
	} else

	//PM
	//  state
	if (sysData.analog.pm.state)
	{
		setAnalogModulateSource(sysData.analog.source);
		setPmState(true);
		//	sysData.analog.pm.state = true;
		//
		//	sprintf(tempChar, "%d",sysData.analog.pm.state);
		//	cmdStr = ":PM:STAT " + QString(tempChar).trimmed();
		//	sendSerialData(cmdStr);
		//	usleep(300 * 1000);

		////PM调制相位
		sprintf(tempChar, "%.0f", sysData.analog.pm.phase);
		cmdStr = ":PM:PHAS " + QString(tempChar).trimmed();
		//	printf("%s",cmdStr.toStdString().c_str());
		sendSerialData(cmdStr);
		//	usleep(100 * 1000);

		//rate
		sprintf(tempChar, "%.0f", sysData.analog.pm.rate);
		cmdStr = ":PM:INT:FUNC:FREQ " + QString(tempChar).trimmed();
		sendSerialData(cmdStr);
		//	usleep(100 * 1000);

	} else if (sysData.digital.digitalstate)
	{
		//状态
		setDigitalModuateState(1);
		//	usleep(300 * 1000);

		//类型
		setDigitalModuateType(sysData.digital.digitaltype);
		//	usleep(100 * 1000);

		switch (sysData.digital.digitaltype)
		{
			case dt2ASK:
			case dt4ASK:
			case dt8ASK:
				setDigitalModulatePara(sysData.digital.digitalpara.depth);
				setDigitalModuateRate(sysData.digital.digitalpara.rate);
				break;
			case dt2FSK:
			case dt4FSK:
			case dt8FSK:
				setDigitalModulatePara(sysData.digital.digitalpara.deviation);
				setDigitalModuateRate(sysData.digital.digitalpara.rate);
				break;
			case dt2PSK:
			case dt4PSK:
			case dt8PSK:
				setDigitalModulatePara(sysData.digital.digitalpara.phase);
				setDigitalModuateRate(sysData.digital.digitalpara.rate);
				break;
			default:
				break;
		}
	}
	else if (sysData.drg.state)
	{
		setDrgState(sysData.drg.state);
		setDrgUplimit(sysData.drg.uplimit);
		setDrgDownlimit(sysData.drg.downlimit);
		setDrgPoints(sysData.drg.points);
		setDrgRate(sysData.drg.rate);
		setDrgWorkmode(sysData.drg.workmode);
	}
	setDrgUplimit(sysData.drg.uplimit);
	setDrgDownlimit(sysData.drg.downlimit);
#if 0
	//ASK
	if(sysData.digital.ask.state)
	{
		//state
		setAskState(1);
		usleep(300 * 1000);

		//mode
		setAskMode(sysData.digital.ask.mode);
		usleep(100 * 1000);

		//rate
		setAskRate(sysData.digital.ask.rate);
		usleep(100 * 1000);
	} else
	//FSK
	if(sysData.digital.fsk.state)
	{
		//state
		setFskState(1);
		usleep(300 * 1000);

		//type
		setFskMode(sysData.digital.fsk.mode);
		usleep(100 * 1000);

		//rate
		setFskRate(sysData.digital.fsk.rate);
		usleep(100 * 1000);

		//step
		setFskStep(sysData.digital.fsk.step);
		usleep(100 * 1000);
	} else
	//PSK
	if(sysData.digital.psk.state)
	{
		//state
		setPskState(1);
		usleep(300 * 1000);

		//type
		setPskMode(sysData.digital.psk.mode);
		usleep(100 * 1000);

		//rate
		setPskRate(sysData.digital.psk.rate);
		usleep(100 * 1000);
	} else
	//Pluse
	if(sysData.analog.pluse.state)
	{
		//state
		setPluseState(1);
		usleep(300 * 1000);

		//type
		setPlusePeriod(sysData.analog.pluse.period);
		usleep(100 * 1000);

		//width
		setPluseWidth(sysData.analog.pluse.width);
		usleep(100 * 1000);
	}

	else //关闭

	{
		closeDemod();
		usleep(300 * 1000);
	}
#endif

	//  closeDemodState();

	switch (sysData.sweep.mode)
	{
		//	case smOff:  setSweepMode(smOff); break;
		case smFreq:
			setSweepMode(smFreq);
			break;
		case smAmpl:
			setSweepMode(smAmpl);
			break;
		case smFreqAmpl:
			setSweepMode(smFreqAmpl);
			break;
		default:
			break;
	}
	usleep(100 * 1000);

	isNeedCloseDemod = true;
}

bool tSysScpi::updateRecordOfClear(void)
{
	QFile file(UpdateDirName + "/update/update");

	QString line;
	QStringList strList;

	if (file.open(QIODevice::ReadOnly))
	{
		QTextStream text(&file);

		while (!text.atEnd())
		{
			line = text.readLine();

			if (!line.isNull() && !line.isEmpty())
			{
				strList << line;
			}
		}

		text.flush();
		file.close();
		file.remove();
	}

	if (file.open(QIODevice::WriteOnly))
	{
		QTextStream text(&file);
		text.flush();
		file.close();

		return true;
	}

	return false;
}

bool tSysScpi::updateRecordOfAdd(QString value)
{
	QString tempValue = value.trimmed();
	QStringList tempList = tempValue.split(",");

	if (tempList.count() >= 1 && tempList.count() <= 2)
	{
		QFile file(UpdateDirName + "/update/update");

		QString line;
		QStringList strList;
		char tempChar[32] = { 0x0 };

		if (file.open(QIODevice::ReadOnly))
		{
			QTextStream text(&file);

			while (!text.atEnd())
			{
				line = text.readLine();

				if (!line.isNull() && !line.isEmpty())
				{
					strList << line;
				}
			}

			text.flush();
			file.close();
			file.remove();
		}

		updatingFileName = UpdateDirName + "/update/temp" + QString(intToString(strList.count(), tempChar)).trimmed();
		line = "temp" + QString(intToString(strList.count(), tempChar)).trimmed() + ", " + tempList.at(0);
		if (tempList.count() >= 2)
		{
			line += ", " + tempList.at(1);
		}
		strList << line;

		if (file.open(QIODevice::WriteOnly))
		{
			QTextStream text(&file);

			for (int i = 0; i < strList.count(); i++)
			{
				text << strList.at(i) << "\n";
			}

			text.flush();
			file.close();

			return true;
		}
	}

	return false;
}


bool tSysScpi::updateSystem(void)
{
	QFile file(UpdateDirName + "/update/update");

	QString line;
	QStringList strList;
	char tempChar[32] = { 0x0 };

	if (file.open(QIODevice::ReadOnly))
	{
		QTextStream text(&file);

		while (!text.atEnd())
		{
			line = text.readLine();

			if (!line.isNull() && !line.isEmpty())
			{
				strList << line;
			}
		}

		text.flush();
		file.close();
	}

	QString cmdString = "";
	QStringList sl;

	if (strList.count() > 0)
	{
		QFile f(UpdateDirName + "/install");
		QString str1 = "";
		QString str2 = "";
		QString str3 = "";
		f.open(QIODevice::WriteOnly);
		QTextStream ts(&f);
		ts << "#!/bin/sh" << endl;

		for (int i = 0; i < strList.count(); i++)
		{
			sl = strList.at(i).split(",");
			if (sl.count() >= 2)
			{
				str1 = "file1=" + UpdateDirName + "/update/" + sl.at(0).trimmed();
				str2 = "file2=" + sl.at(1).trimmed();
				ts << str1 << endl;
				ts << str2 << endl;
			}

			str3 = "if [ -f $file2 ]; then rm -f $file2; fi";
			ts << str3 << endl;

			
			if (sl.count() >= 3)
			{
				str3 = "if [ -f $file1 ]; then cp -f $file1 $file2; chmod " + sl.at(2).trimmed() + " $file2; fi";
				ts << str3 << endl;
			} else
			{
				str3 = "if [ -f $file1 ]; then mv -f $file1 $file2; fi";
				ts << str3 << endl;

			}
		}	
		ts.flush();
		f.close();
	}

	return true;
}

QString tSysScpi::updateFileName(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return "";
	}

	QStringList sl = value.split(",");

	if (sl.count() >= 1)
	{
		QString tempValue = sl.at(0).trimmed();

		if (tempValue.isNull() || tempValue.isEmpty())
		{
			return "";
		}

		QString Result = "";

		for (int i = tempValue.length() - 1; i >= 0; i--)
		{
			if (tempValue.at(i) == '/')
			{
				break;
			} else
			{
				Result = tempValue.at(i) + Result;
			}
		}

		return Result;
	}

	return "";
}

void tSysScpi::loadDeviceCompany(void)
{
	company = "Bailu";

	QString fileName = QCoreApplication::applicationDirPath() + "/setting.ini";

	if (QFile(fileName).exists())
	{
		QSettings setting(fileName, QSettings::IniFormat);
		setting.beginGroup("DeviceRes");
		company = setting.value("company", company).toString();
		setting.endGroup();
	}
}

void tSysScpi::saveDeviceCompany(QString value)
{
	QString fileName = QCoreApplication::applicationDirPath() + "/setting.ini";
	QSettings setting(fileName, QSettings::IniFormat);
	setting.beginGroup("DeviceRes");
	setting.setValue("company", value);
	setting.endGroup();
}

void tSysScpi::loadDeviceType(void)
{
	devType = "SG1030A";

	QString fileName = QCoreApplication::applicationDirPath() + "/setting.ini";

	if (QFile(fileName).exists())
	{
		QSettings setting(fileName, QSettings::IniFormat);
		setting.beginGroup("DeviceRes");
		devType = setting.value("type", devType).toString();
		setting.endGroup();
	}
}

void tSysScpi::saveDeviceType(QString value)
{
	QString fileName = QCoreApplication::applicationDirPath() + "/setting.ini";
	QSettings setting(fileName, QSettings::IniFormat);
	setting.beginGroup("DeviceRes");
	setting.setValue("type", value);
	setting.endGroup();
}

void tSysScpi::loadDiveceLogo(void)
{
	QString fileName = QCoreApplication::applicationDirPath() + "/logo.png";

	if (QFile(fileName).exists())
	{
		logo.load(fileName);
	} else
	{
		logo.loadFromData(logoBits, sizeof logoBits / sizeof logoBits[0]);
	}
}

void tSysScpi::saveDeviceLogo(QString value)
{

}

void tSysScpi::saveSystemSetting(QString section, QString name, QVariant value)
{
	QString fileName = QCoreApplication::applicationDirPath() + "/setting.ini";
	QSettings setting(fileName, QSettings::IniFormat);
	setting.beginGroup(section);
	setting.setValue(name, value);
	setting.endGroup();
}

void tSysScpi::scpiFloatToLen8String(double value, char* result)
{
	char temp[16] = { };
	sprintf(temp, "%f", value);

	for (int i = 0; i < 8; i++)
	{
		result[i] = temp[i];
	}

	result[7] = 0x0;
	result[8] = 0x0;
	result[9] = '\0';
}

//异步延时
void tSysScpi::asyncDelay(unsigned long value)
{
	if (value <= 1000 || value > 1000 * 1000)
	{
		usleep(1000);
		QCoreApplication::processEvents();
		return;
	}

	//1ms ~ 1s
	int nums = value % 1000;

	for (int i = 0; i < nums; i++)
	{
		usleep(1000);
		QCoreApplication::processEvents();
	}
}

QString tSysScpi::getSystemInfoString(void)
{
	char temp[32] = { };
	QStringList list;

	list.clear();
	list << getTheTranslatorString("Serial Number") + " = " + QString(sysData.system.serialNo).trimmed();
	sprintf(temp, "%d.%d.%d.%d.%d", VER_MAJOR, VER_MINOR, VER_REVISION, VER_BUILD, VER_TAG);
	list << getTheTranslatorString("Hardware Version") + " = " + QString(temp).trimmed();

	QString result = "";

	for (int i = 0; i < list.count(); i++)
	{
		if (i == 0)
		{
			result = list.at(i);
		} else
		{
			result += ", " + list.at(i);
		}
	}

	return result;
}

//获取布尔字符串
QString tSysScpi::boolToString(bool value)
{
	if (value)
	{
		return "1";
	} else
	{
		return "0";
	}
}

QString tSysScpi::doubleToString(double value)
{
	char temp[64] = { };

	sprintf(temp, "%.2f", value);
	return QString(temp);
}

//获取布尔值
int tSysScpi::getBoolValue(QString value, bool* ok)
{
	QString tempValue = value.toUpper().trimmed();

	*ok = false;

	int result = 0;

	if (tempValue.isNull() || tempValue.isEmpty())
	{
		return result;
	}

	if (tempValue == "ON" || tempValue == "1")
	{
		*ok = true;
		result = 1;
	} else if (tempValue == "OFF" || tempValue == "0")
	{
		*ok = true;
		result = 0;
	}

	return result;
}

//获取小整数
unsigned char tSysScpi::getCharValue(QString value, bool* ok)
{
	*ok = false;

	if (value.isNull() || value.isEmpty() || value.length() != 1)
	{
		return 0;
	}

	bool isOk = false;
	int tempValue = 0;

	tempValue = value.toInt(&isOk);

	if (isOk && tempValue >= 1 && tempValue <= 9)
	{
		*ok = true;
		return tempValue;
	} else
	{
		return 0;
	}
}

//获取频率值
double tSysScpi::getFreqValue(QString value, bool* ok)
{
	*ok = false;

	if (value.isNull() || value.isEmpty())
	{
		return 0;
	}

	bool isOk = false;
	double tempValue = 0;

	tempValue = value.toDouble(&isOk);

	if (isOk)
	{
		*ok = true;
		return tempValue;
	}

	QString valueUpper = value.toUpper().trimmed();
	QStringList unitList = (QStringList() << "GHZ" << "MHZ" << "KHZ" << "HZ");

	for (int i = 0; i < unitList.count(); i++)
		if (valueUpper.indexOf(unitList.at(i)) != -1)
		{
			valueUpper = valueUpper.remove(unitList.at(i)).trimmed();
			tempValue = valueUpper.toDouble(&isOk);

			if (isOk)
			{
				switch (i)
				{
					case 0:
						tempValue *= 1e9;
						break;
					case 1:
						tempValue *= 1e6;
						break;
					case 2:
						tempValue *= 1e3;
						break;
				}

				*ok = true;

				break;
			}
		}

	return tempValue;
}

//获取幅度值
double tSysScpi::getAmptValue(QString value, bool* ok)
{
	*ok = false;

	if (value.isNull() || value.isEmpty())
	{
		return 0;
	}

	bool isOk = false;
	double tempValue = value.toDouble(&isOk);

	if (isOk)
	{
		*ok = true;
		return tempValue;
	}

	QString valueUpper = value.toUpper().trimmed();
	QStringList unitList = (QStringList() << "DBM" << "DBMV" << "DBUV" << "MW" << "W" << "UW" << "NW" << "MV" << "UV" << "NV" << "V");

	for (int i = 0; i < unitList.count(); i++)
	{
		if (valueUpper.indexOf(unitList.at(i)) != -1)
		{
			valueUpper = valueUpper.remove(unitList.at(i)).trimmed();
			tempValue = valueUpper.toDouble(&isOk);

			if (isOk)
			{
				switch (i)
				//zpc add
				{
					case 0:
						sysData.ampl.amplUnit = audBm;
						break;
					case 2:
						sysData.ampl.amplUnit = audBuV;
						break;
					case 7:
						sysData.ampl.amplUnit = aumV;
						break;
					default:
						break;
				}
				*ok = true;
				break;
			}
		}
	}

	return tempValue;
}

//获取dB值
double tSysScpi::getdBValue(QString value, bool* ok)
{
	*ok = false;

	if (value.isNull() || value.isEmpty())
	{
		return 0;
	}

	bool isOk = false;
	double tempValue = 0;

	tempValue = value.toDouble(&isOk);

	if (isOk)
	{
		*ok = true;
		return tempValue;
	}

	QString valueUpper = value.toUpper().trimmed();
	QStringList unitList = (QStringList() << "DB");

	for (int i = 0; i < unitList.count(); i++)
	{
		if (valueUpper.indexOf(unitList.at(i)) != -1)
		{
			valueUpper = valueUpper.remove(unitList.at(i)).trimmed();
			tempValue = valueUpper.toDouble(&isOk);

			if (isOk)
			{
				*ok = true;
				break;
			}
		}
	}

	return tempValue;
}

//获取时间值
double tSysScpi::getTimeValue(QString value, bool* ok)
{
	*ok = false;

	if (value.isNull() || value.isEmpty())
	{
		return 0;
	}

	bool isOk = false;
	double tempValue = 0;

	tempValue = value.toDouble(&isOk);

	if (isOk)
	{
		*ok = true;
		return tempValue;
	}

	QString valueUpper = value.toUpper().trimmed();
	QStringList unitList = (QStringList() << "NS" << "US" << "MS" << "S");

	for (int i = 0; i < unitList.count(); i++)
		if (valueUpper.indexOf(unitList.at(i)) != -1)
		{
			valueUpper = valueUpper.remove(unitList.at(i)).trimmed();
			tempValue = valueUpper.toDouble(&isOk);

			if (isOk)
			{
				switch (i)
				{
					case 3:
						tempValue *= 1e9;
						break;
					case 2:
						tempValue *= 1e6;
						break;
					case 1:
						tempValue *= 1e3;
						break;
					default:
						break;
				}

				*ok = true;
				break;
			}
		}

	return tempValue;
}

//获取电压值
double tSysScpi::getVoltageValue(QString value, bool* ok)
{
	*ok = false;

	if (value.isNull() || value.isEmpty())
	{
		return 0;
	}

	bool isOk = false;
	double tempValue = 0;

	tempValue = value.toDouble(&isOk);

	if (isOk)
	{
		*ok = true;
		return tempValue;
	}

	QString valueUpper = value.toUpper().trimmed();
	QStringList unitList = (QStringList() << "MV" << "V");

	for (int i = 0; i < unitList.count(); i++)
		if (valueUpper.indexOf(unitList.at(i)) != -1)
		{
			valueUpper = valueUpper.remove(unitList.at(i)).trimmed();
			tempValue = valueUpper.toDouble(&isOk);

			if (isOk)
			{
				switch (i)
				{
					case 1:
						tempValue *= 1e3;
						break;
					default:
						break;
				}

				*ok = true;
				break;
			}
		}

	return tempValue;
}

//获取整数值
int tSysScpi::getIntValue(QString value, bool* ok)
{
	*ok = false;

	if (value.isNull() || value.isEmpty())
	{
		return 0;
	}

	bool isOk = false;
	double tempValue = value.toDouble(&isOk);

	if (isOk)
	{
		*ok = true;
		return (int) tempValue;
	}

	return 0;
}

//获取浮点型值
double tSysScpi::getDoubleValue(QString value, bool *ok)
{
	*ok = false;

	if (value.isNull() || value.isEmpty())
	{
		return 0;
	}

	bool isOk = false;
	double tempValue = value.toDouble(&isOk);

	if (isOk)
	{
		*ok = true;
		return tempValue;
	}

	return 0;
}

//检测本地路径
void tSysScpi::checkLocalDirectory(void)
{
	char* saveDir = "/home/sky/save";
	char* tracePngDir = "/home/sky/save/trace";

	QString cmdStr = "";

	if (!QDir(saveDir).exists())
	{
		cmdStr = "mkdir ";
		cmdStr += saveDir;
		system(cmdStr.toStdString().c_str());

		if (!QDir(saveDir).Writable)
		{
			cmdStr = "chmod 777 ";
			cmdStr += saveDir;
			system(cmdStr.toStdString().c_str());
		}
	}

	if (!QDir(tracePngDir).exists())
	{
		cmdStr = "mkdir ";
		cmdStr += tracePngDir;
		system(cmdStr.toStdString().c_str());

		if (!QDir(tracePngDir).Writable)
		{
			cmdStr = "chmod 777 ";
			cmdStr += tracePngDir;
			system(cmdStr.toStdString().c_str());
		}
	}
}

//检测USB路径
int tSysScpi::checkUsbDirectory(void)
{
	char* saveDir = usbDirName;
	QString cmdStr = "";

	if (!QDir(saveDir).exists())
	{
		cmdStr = "mkdir ";
		cmdStr += saveDir;

		if (system(cmdStr.toStdString().c_str()) != 0)
		{
			return _MSG_USBDISK_NOSPACE;
		}

		if (!QDir(saveDir).Writable)
		{
			cmdStr = "chmod 777 ";
			cmdStr += saveDir;

			if (system(cmdStr.toStdString().c_str()) != 0)
			{
				return _MSG_USBDISK_READONLY;
			}
		}
	}

	return _RESULT_SUCCESS;
}

//连接闪存
int tSysScpi::connectToUsbDisk(void)
{
	char* umountCmd = "umount /mnt/udisk";
	char* detectCmd = "fdisk -l | grep /dev/sd > /tmp/usbDisk.temp";
	char* tempFileName = "/tmp/usbDisk.temp";
	int errorNums = 0;

	//uninstall usb disk
	while (true)
	{
		if (system(umountCmd) != 0)
		{
			break;
		}

		errorNums++;

		if (errorNums > 1024)
		{
			return _MSG_USBDISK_UNMOUNTFAIL;
		}
	}

	//detect usb disk id
	if (system(detectCmd) == 0)
	{
		QFile file(tempFileName);

		if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			return _MSG_SYSTEM_FILEOPENERROR;
		}

		QTextStream in(&file);
		QString txt = in.readAll();

		if (txt.isNull() || txt.isEmpty())
		{
			file.close();
			return _MSG_USBDISK_NOTFOUND;
		}

		QChar chr;
		int charNums = 0;
		QString idStr = "";

		for (int i = 0; i < txt.length(); i++)
		{
			chr = txt.at(i);

			if ((chr == '/') && (charNums < 3))
			{
				charNums++;
			}

			if ((charNums >= 3) && (chr != ' '))
			{
				idStr += chr;
			} else if ((charNums >= 3) && (chr == ' '))
			{
				break;
			}
		}

		if (idStr.isEmpty())
		{
			file.close();
			return _MSG_USBDISK_NOTFOUND;
		}

		QString cmdStr = "mount ";
		cmdStr += idStr;
		cmdStr += " /mnt/udisk";

		if (system(cmdStr.toStdString().c_str()) == 0)
		{
			if (!QDir(usbDirName).exists())
			{
				cmdStr = "mkdir ";
				cmdStr += usbDirName;

				if (system(cmdStr.toStdString().c_str()) != 0)
				{
					file.close();
					return _MSG_SYSTEM_CREATEDIRERROR;
				}
			}

			cmdStr = "chmod 777 ";
			cmdStr += usbDirName;

			if (system(cmdStr.toStdString().c_str()) != 0)
			{
				file.close();
				return _MSG_SYSTEM_FILECHMODERROR;
			}
		} else
		{
			idStr = idStr.mid(0, idStr.length() - 1);
			cmdStr = "mount " + idStr + " /mnt/udisk";

			if (system(cmdStr.toStdString().c_str()) == 0)
			{
				if (!QDir(usbDirName).exists())
				{
					cmdStr = "mkdir ";
					cmdStr += usbDirName;

					if (system(cmdStr.toStdString().c_str()) != 0)
					{
						file.close();
						return _MSG_SYSTEM_CREATEDIRERROR;
					}
				}

				cmdStr = "chmod 777 ";
				cmdStr += usbDirName;

				if (system(cmdStr.toStdString().c_str()) != 0)
				{
					file.close();
					return _MSG_SYSTEM_FILECHMODERROR;
				}
			} else
			{
				file.close();
				return _MSG_USBDISK_MOUNTFAIL;
			}
		}

		file.close();
	} else
	{
		return _MSG_USBDISK_NOTFOUND;
	}

	return _RESULT_SUCCESS;
}

//断开闪存连接
int tSysScpi::disConnectFromUsbDisk(void)
{
	if (system("umount /mnt/udisk") == 0)
	{
		return _RESULT_SUCCESS;
	} else
	{
		return _RESULT_FAILED;
	}
}

//获取文件数目
int tSysScpi::getDirFileCount(QString dirName)
{
	QDir dir(dirName);
	dir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
	dir.setSorting(QDir::Size | QDir::Reversed);

	QFileInfoList list = dir.entryInfoList();

	return list.size();
}

//获取字符串
QString tSysScpi::getTheTranslatorString(QString enStr)
{
	if (enStr.isNull() || enStr.isEmpty())
	{
		return "";
	}

	if (sysData.system.isLanguageEn)
	{
		return enStr;
	}

	QString enStr1 = enStr.toUpper().trimmed();
	QString enStr2 = "";

	for (int i = 0; i < sizeof sysTrans / sizeof sysTrans[0]; i++)
	{
		enStr2 = sysTrans[i].enName;
		enStr2 = enStr2.toUpper().trimmed();

		if (enStr1 == enStr2)
		{
			return sysTrans[i].chName;
			break;
		}
	}

	return enStr;
}

void tSysScpi::outputMessage(QString title)
{
	if (title.isNull() || title.isEmpty())
	{
		return;
	}

	int sep = 20;
	int nums = 1;

	QPainter p;
	QPixmap pix(rcCal.size());
	QRect rcTitle = QRect(sep, sep, pix.width() - 2 * sep, pix.height() - 2 * sep);

	p.begin(&pix);
	p.fillRect(pix.rect(), BACKGROUNDCOLOR);
	p.setPen(Qt::black);
	p.setFont(fontBold);
	p.drawText(rcTitle, Qt::AlignCenter, title);
	p.end();

	QScreen::instance()->blit(pix.toImage(), QPoint(rcCal.left(), rcCal.top()), rcCal);
}

//输出到屏幕
void tSysScpi::outputToScreen(QString title)
{
	if (title.isNull() || title.isEmpty())
	{
		return;
	}

	int sep = 20;
	int nums = 1;

	QPainter p;
	QPixmap pix(rcCal.size());
	QRect rcTitle = QRect(sep, sep, pix.width() - 2 * sep, pix.height() - 2 * sep);

	p.begin(&pix);
	p.fillRect(pix.rect(), BACKGROUNDCOLOR);
	p.setPen(Qt::red);
	p.setFont(fontBold);
	p.drawText(rcTitle, Qt::AlignCenter, title);
	p.end();

	QScreen::instance()->blit(pix.toImage(), QPoint(rcCal.left(), rcCal.top()), rcCal);
}

//输出到屏幕
void tSysScpi::outputToScreen(QString title, QString value)
{

	//rcCal = QRect(50, 140, 316, 20);	//zpc add test
	int sep = 20;
	int nums = 1;
	char tempChar[32] = { };
	QPainter p;
	QPixmap pix(rcCal.size());

	QRect rcTitle;
	QRect rcValue;

	if (value.isNull() || value.isEmpty())
	{
		rcTitle = QRect(sep, sep, pix.width() - 2 * sep, pix.height() - 2 * sep);
		nums = 1;
	} else
	{
		rcTitle = QRect(sep, sep, pix.width() - 2 * sep, (pix.height() - 2 * sep) / 2);
		rcValue = QRect(rcTitle.left(), rcTitle.top() + rcTitle.height(), rcTitle.width(), rcTitle.height());
		nums = 2;
	}

	p.begin(&pix);
	p.fillRect(pix.rect(), BACKGROUNDCOLOR);
	p.setPen(Qt::black);
	p.setFont(fontBold);
	p.drawText(rcTitle, Qt::AlignLeft | Qt::AlignVCenter, title);

	if (nums == 2)
	{
		p.drawText(rcValue, Qt::AlignLeft | Qt::AlignVCenter, value);
	}

	p.end();

	QScreen::instance()->blit(pix.toImage(), QPoint(rcCal.left(), rcCal.top()), rcCal);
}

//输出到屏幕
void tSysScpi::outputToScreen(QString title, int progress, int normal)
{
	//QScreen::instance()->blit(pixBackground.toImage(), QPoint(0, 0), QRect(0, 0, SCREENWIDTH, SCREENHEIGHT));

	int sep = 20;
	char tempChar[32] = { };
	QPainter p;
	QPixmap pix(rcCal.size());

	QRect rcTitle = QRect(sep, sep, pix.width() - 2 * sep, (pix.height() - 2 * sep) / 2);
	QRect rcProgress = QRect(rcTitle.left(), rcTitle.top() + rcTitle.height(), rcTitle.width(), 20);
	QRect rcPosition = QRect(rcProgress.left(), rcProgress.top(), rcProgress.width() * progress / 100, rcProgress.height());

	p.begin(&pix);
	p.fillRect(pix.rect(), BACKGROUNDCOLOR);
	if (normal == 0)
	{
		p.setPen(Qt::black);
	} else
	{
		p.setPen(Qt::red);
	}
	p.setFont(fontData);
	p.drawText(rcTitle, Qt::AlignLeft | Qt::AlignVCenter, title);
	p.fillRect(rcProgress, Qt::white);
	p.fillRect(rcPosition, Qt::green);
	p.setPen(Qt::black);
	p.drawRect(rcProgress);
	p.drawText(rcProgress, Qt::AlignCenter, QString(intToString((int) progress, tempChar)).trimmed() + "%");
	p.end();

	QScreen::instance()->blit(pix.toImage(), QPoint(rcCal.left(), rcCal.top()), rcCal);
}

void tSysScpi::drawWin4(QString title)
{

	QRect rc = QRect(WIN4X + 4, WIN4Y, WIN4WIDTH - 4, WIN4HEIGHT);
	QRect rcHelp = QRect(BORDERWIDTH, 0, rc.width() - 3 * BORDERWIDTH, rc.height() - 2 * BORDERWIDTH);
	QPainter p;
	QPixmap pix(rc.size());

	if (!pix.isNull())
	{
		p.begin(&pix);
		p.fillRect(pix.rect(), BACKCOLORALLWIN);
		p.fillRect(rcHelp, BACKCOLORWIN4);
		p.drawRect(rcHelp);
		//QBrush b(QColor(128,128,64));
		//qDrawShadePanel(&p, rcPoint, palette(), true, FRAMELINEWIDTH, &b);
		p.setPen(QColor(64, 0, 0));
		p.setFont(fontWin4);
		rcHelp = QRect(rcHelp.left() + 5, rcHelp.top(), rcHelp.width() - 10, rcHelp.height());
		p.drawText(rcHelp, Qt::AlignLeft | Qt::AlignVCenter, title);
		p.end();

		QScreen::instance()->blit(pix.toImage(), QPoint(rc.left(), rc.top()), rc);
	}
}

double tSysScpi::dBmTomV(double value)
{
	return qSqrt(50000 * qPow(10, 0.1 * value));
}

//关闭所有通信端口
void tSysScpi::closeScpiPortOfAll(void)
{
	closeScpiPortOfLan();
	closeScpiPortOfUsb();
	closeScpiPortOfSerial();

	sysData.scpiData.stateR = false;
	sysData.scpiData.stateL = true;
	sysData.scpiData.stateT = false;
	sysData.scpiData.stateS = false;
}

//关闭LAN端口通信
void tSysScpi::closeScpiPortOfLan(void)
{
	if (sysData.scpiData.lanOpened)
	{
		sysData.scpiData.lanOpened = false;
	}
}

//关闭USB端口通信
void tSysScpi::closeScpiPortOfUsb(void)
{
	if (sysData.scpiData.usbOpened)
	{
		sysData.scpiData.usbOpened = false;
	}
}

//关闭COM端口通信
void tSysScpi::closeScpiPortOfSerial(void)
{
	if (sysData.scpiData.serialOpened)
	{
		sysData.scpiData.serialOpened = false;
	}
}

//打开服务器SOCKET
bool tSysScpi::openServerSocket(void)
{
	QString hostName = QHostInfo::localHostName();
	QHostInfo hostInfo = QHostInfo::fromName(hostName);
	QList<QHostAddress> listAddress = hostInfo.addresses();
	//qDebug("hostNmae:%s;\n",hostName.toStdString().c_str());
	
	if (serverSocket == NULL)
	{
		serverSocket = new QTcpServer;
		serverSocket->setMaxPendingConnections(1);
		serverSocket->listen(QHostAddress::Any, LISTENPORT);
		connect(serverSocket, SIGNAL(newConnection()), this, SLOT(acceptClientSocketConnect()));
	}

	return true;
}

//关闭服务器SOCKET
bool tSysScpi::closeServerSocket(void)
{
	if (serverSocket != NULL)
	{
		disconnect(serverSocket, SIGNAL(newConnection()), this, SLOT(acceptClientSocketConnect()));
		serverSocket->close();
	}
}

//读串口数据
void tSysScpi::readSerialData()
{
	if (comHandle <= 0)
	{
		return;
	}

	if (!sysData.scpiData.serialOpened)
	{
		sysData.scpiData.serialOpened = true;
	}

	sysData.scpiData.lanOpened = false;
	sysData.scpiData.usbOpened = false;

	unsigned char buf[SCPIBUFSIZE] = { };
	unsigned int length = 0;

	memset(buf, 0, sizeof buf);

	//  while (read(comHandle, &buf[0], SCPIBUFSIZE) > 0)  //常工添加
	//  {
	//    read(comHandle, &buf[0], SCPIBUFSIZE);
	//  }
	//
	//  return;

	//  length = read(comHandle, buf, sizeof buf);
	length = read(comHandle, buf, 2048);

	if (length > 0)
	{
		if (sysData.scpiData.stateT)
		{
			sysData.scpiData.stateT = false;
		}

		sysData.scpiData.stateS = true;
		memcpy(sysData.scpiData.buf, buf, sizeof(char) * length);
		sysData.scpiData.rcvLength = length;

		//    if (!sysData.isFactoryCalibrating && !sysData.isPreamplifierCalibrating)
		{
			if (sysData.scpiData.interface != sitCom)
			{
				sysData.scpiData.interface = sitCom;
			}

			if (length <= 1024)
			{
				handleScpiCommand(scpiBuffer + QString((char *) buf));
			} else
			{
				scpiBuffer = "";
			}
		}
	}
}

//读USB从口数据
void tSysScpi::readUsbSlaveData(void)
{
	if (usbHandle <= 0)
	{
		return;
	}

	if (!sysData.scpiData.usbOpened)
	{
		sysData.scpiData.usbOpened = true;
	}

	sysData.scpiData.serialOpened = false;
	sysData.scpiData.lanOpened = false;

	int length = 0;
	int emptyNums = 0;
	char buf[2048] = { 0x0 };

	memset(buf, 0, sizeof buf);
	//  length = read(usbHandle, buf, 64);
	length = read(usbHandle, buf, 2048);

	if (length > 0)
	{
		if (sysData.scpiData.interface != sitUsb)
		{
			sysData.scpiData.interface = sitUsb;
		}
		if (!sysData.scpiData.stateR)
		{
			sysData.scpiData.stateR = true;
			sysData.scpiData.stateL = false;
			emit refreshScpiState(getTheTranslatorString("switch to remote control mode"));
		}
		if (sysData.scpiData.stateT)
		{
			sysData.scpiData.stateT = false;
		}
		sysData.scpiData.stateS = true;
		memcpy(sysData.scpiData.buf, buf, sizeof(char) * length);
		sysData.scpiData.rcvLength = length;

		if (length <= 2048) //原本为1024
		{
			handleScpiCommand(scpiBuffer + QString((char *) buf));
		} else
		{
			scpiBuffer = "";
		}
	} else
	{
		emit resetUsbSlaveInterface();

		if (sysData.scpiData.usbOpened)
		{
			sysData.scpiData.usbOpened = false;
		}

		scpiBuffer = "";
		sysData.scpiData.stateR = false;
		sysData.scpiData.stateL = true;
		sysData.scpiData.interface = sitNone;
		//    emit refreshScpiState(getTheTranslatorString("switch to local control mode"));
	}
}

//接受客户端建立连接
void tSysScpi::acceptClientSocketConnect(void)
{
	scpiBuffer = "";

	if (serverSocket->hasPendingConnections())
	{
		clientSocketDisConnect();
		clientSocket = serverSocket->nextPendingConnection();
	}

	if (clientSocket != NULL)
	{
		clientSocket->readAll();

		if (clientSocket->readBufferSize() != SOCKETBUFSIZE)
		{
			clientSocket->setReadBufferSize(SOCKETBUFSIZE);
		}

		connect(clientSocket, SIGNAL(disconnected()), this, SLOT(clientSocketDisConnect()));
		connect(clientSocket, SIGNAL(readyRead()), this, SLOT(clientSocketReadyRead()));
		connect(clientSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(clientSocketError(QAbstractSocket::SocketError)));
	}

	sysData.scpiData.stateR = true;
	sysData.scpiData.stateL = false;
	sysData.scpiData.interface = sitLan;
	emit refreshScpiState(getTheTranslatorString("switch to remote control mode"));
}

//客户端断开连接
void tSysScpi::clientSocketDisConnect(void)
{
	scpiBuffer = "";

	if (clientSocket != NULL)
	{
		disconnect(clientSocket, SIGNAL(disconnected()), this, SLOT(clientSocketDisConnect()));
		disconnect(clientSocket, SIGNAL(readyRead()), this, SLOT(clientSocketReadyRead()));
		disconnect(clientSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(clientSocketError(QAbstractSocket::SocketError)));

		clientSocket->close();
		clientSocket->deleteLater();
		clientSocket = NULL;
	}

	sysData.scpiData.stateR = false;
	sysData.scpiData.stateL = true;
	sysData.scpiData.interface = sitNone;
	emit refreshScpiState(getTheTranslatorString("switch to local control mode"));
}

//客户端有数据到达
void tSysScpi::clientSocketReadyRead(void)
{
	qint64 len = 0;

	QTime t = QTime::currentTime();
	t.restart();
	if (clientSocket != NULL)
	{
		len = clientSocket->bytesAvailable();

		if (len < 1)
		{
			return;
		}

		if (sysData.scpiData.interface != sitLan)
		{
			sysData.scpiData.interface = sitLan;
		}

		if (!sysData.scpiData.stateR)
		{
			sysData.scpiData.stateR = true;
			sysData.scpiData.stateL = false;
			emit refreshScpiState(getTheTranslatorString("switch to local control mode"));
		}

		sysData.scpiData.stateS = true;
		readBuf = clientSocket->readAll();
		sysData.scpiData.stateS = false;

		if (isRcvingLogoFile)
		{
			scpiBuffer = "";

			if (QString(readBuf).indexOf(":DEVice", 0) != -1)
			{
				if (readBuf.length() <= 1024)
				{
					handleScpiCommand(scpiBuffer + (QString) readBuf);
				} else
				{
					scpiBuffer = "";
				}
			} else
			{
				FILE* fp = fopen(QString(QCoreApplication::applicationDirPath() + "/logo.png.bak").toStdString().c_str(), "ab");

				if (fp != NULL)
				{
					fwrite(readBuf, readBuf.size(), 1, fp);
					fclose(fp);
				}
			}
		} else if (isUpdating && isRcvingFile)
		{
			scpiBuffer = "";

			if (QString(readBuf).indexOf(":UPDATE", 0) != -1)
			{
				if (readBuf.length() <= 1024)
				{
					handleScpiCommand(scpiBuffer + (QString) readBuf);
				} else
				{
					scpiBuffer = "";
				}
			} else
			{
				FILE* fp = fopen(updatingFileName.toStdString().c_str(), "ab");

				if (fp != NULL)
				{
					fwrite(readBuf, readBuf.size(), 1, fp);
					fclose(fp);
				}
			}
		} else if (isRcvingSequence && sysData.digital.source == dstExternal)
		{
			if (QString(readBuf).indexOf(":SEQUENCE", 0) != -1)
			{
				if (readBuf.length() <= 1024)
				{
					handleScpiCommand((QString) readBuf);
				}

			} else
			{
				if (sequenceStr.length() + readBuf.length() <= 24276)
				{
					sequenceStr += QString(readBuf);
				} else
				{
					sequenceStr = "";
				}
			}
		}

		else
		{
			if (readBuf.length() <= 1024)
			{
				handleScpiCommand(scpiBuffer + (QString) readBuf);
			} else
			{
				scpiBuffer = "";
			}
		}
	}
	 //qDebug("%d ms\n",t.elapsed());
}

//客户端发生错误
void tSysScpi::clientSocketError(QAbstractSocket::SocketError)
{
	//  printf("clientSocketError\n");

	scpiBuffer = "";

	if (clientSocket != NULL)
	{
		clientSocketDisConnect();
		emit refreshScpiState(getTheTranslatorString(QString("switch to local control mode")));
	}
}

void tSysScpi::keyBoardLEDAllOpen(void)
{
	keyBoardLEDSwitch(1, OPEN);
	usleep(50 * 1000);
	keyBoardLEDSwitch(2, OPEN);
	usleep(50 * 1000);
	keyBoardLEDSwitch(3, OPEN);
	usleep(50 * 1000);
}

void tSysScpi::initKeyBoardLED(void)
{
	keyBoardLEDSwitch(1, CLOSE);
	usleep(50 * 1000);
	keyBoardLEDSwitch(2, CLOSE);
	usleep(50 * 1000);
	keyBoardLEDSwitch(3, CLOSE);
	usleep(50 * 1000);
}

void tSysScpi::keyBoardLEDSwitch(char num, char state)
{
	uint8_t sendbuf[9] = { 0x00 };
	memset(sendbuf, 0x00, sizeof(sendbuf));

	sendbuf[0] = 0xAA;
	sendbuf[1] = num;
	sendbuf[2] = state;
	sendbuf[3] = 0xBB;
	sendbuf[4] = 0xBB;
	sendbuf[5] = 0xBB;
	sendbuf[6] = 0xBB;
	sendbuf[7] = 0xBB;
	write(keyHandle, sendbuf, 8);
	usleep(1000);
}

//设置系统设备开启
int tSysScpi::setSystemServiceDeviceOn(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	//  if(value == "LOG")
	//  {
	//	return setSystemServiceOfSelectLog(true);
	//  }else if(value == "GPIB")
	//  {
	//	return setSystemServiceOfGpib(true);
	//  }else
	if (value == "EVA" || value == "EVALUATE")
	{
		return setSystemServiceOfEvalute(true);
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置系统设备关闭
int tSysScpi::setSystemServiceDeviceOff(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	//  if(value == "LOG")
	//  {
	//	return setSystemServiceOfSelectLog(false);
	//  }else if(value == "GPIB")
	//  {
	//	return setSystemServiceOfGpib(false);
	//  }else
	if (value == "EVA" || value == "EVALUATE")
	{
		return setSystemServiceOfEvalute(false);
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置试用版本
int tSysScpi::setSystemServiceOfEvalute(int value)
{
	eva.isEvaluate = value == 1;

	if (eva.isEvaluate)
	{
		eva.year = QDate::currentDate().year();
		eva.month = QDate::currentDate().month();
		eva.day = QDate::currentDate().day();
		eva.timesOut = 60 * 24;
	}

	QString fileName = QCoreApplication::applicationDirPath() + "/key";

	FILE *fp = fopen(fileName.toStdString().c_str(), "wb");

	if (fp != NULL)
	{
		fwrite(&eva, sizeof eva, 1, fp);
		fclose(fp);
	}
	return __SCPI_SUCCESS;
}

int tSysScpi::getSystemServiceOfEvaluteSYDays(void)
{
	int year = QDate::currentDate().year();
	int month = QDate::currentDate().month();
	int day = QDate::currentDate().day();

	if (eva.isEvaluate)
	{
		eva.syDays = eva.timesOut - qAbs(day - eva.day + (month - eva.month) * 31 + (year - eva.year) * 365) * 24;
		eva.evaluated = eva.syDays < 0;
	}
}

//获取动态密码
QString tSysScpi::getSystemServiceOfMD5(QString value, bool isServer)
{
	bool tempValue = isServer;
	char tempChar[32] = { 0x0 };

	QString sn = "WAYEE" + value.trimmed() + "BAILU";
	QString md5;
	QByteArray ba, bb;
	QCryptographicHash md(QCryptographicHash::Md5);

	ba.append(sn);
	md.addData(ba);
	bb = md.result();
	md5.append(bb.toHex());

	if (isServer)
	{
		return md5.mid(8, 6);
	} else
	{
		return md5.mid(0, 8);
	}

}

//解锁系统
int tSysScpi::unLockSystem(void)
{
	setSystemServiceOfEvalute(0);

	return __SCPI_SUCCESS;
}

//根据方向键计算光标的位置
void tSysScpi::dirKeyToCursor(bool up)
{
	if (up)
	{
		sysData.cursorDirection = UP;
		sysData.cursorLocation++;
		//	sysData.EnterPara.CursorStatus++;
		if (sysData.cursorLocation > (sysData.inputLength - 1))
		{
			sysData.cursorLocation = 0;
		}
	} else
	{
		sysData.cursorDirection = DOWN;
		sysData.cursorLocation--;
		//	sysData.cursorStatus--;
		if (sysData.cursorLocation < 0)
		{
			sysData.cursorLocation = (sysData.inputLength - 1);
		}
	}
}

//获取dBm值，dBm,dBuV,mV间转换
double tSysScpi::getdBmValue(double value)
{
	double tempValue = value;

	switch (sysData.ampl.amplUnit)
	{
		//    case audBm: return tempValue + sysData.ampl.offset;
		//    case audBuV: return tempValue - 107.0 + sysData.ampl.offset;
		//    case aumV: return 10 * log10(0.001 * qPow(tempValue * 1e-3, 2) / 50.0) + sysData.ampl.offset;
		//    default: return tempValue + sysData.ampl.offset;
		case audBm:
			return tempValue;
		case audBuV:
			return tempValue - 107.0;
		case aumV:
			return 10 * log10(1000 * qPow(tempValue * 1e-3, 2) / 50.0);
		default:
			return tempValue;
	}
}

//dBuV,mV转换成dBm
double tSysScpi::getdBuVmVValue(double value)
{
	double tempValue = value;

	switch (sysData.ampl.amplUnit)
	{
		//    case audBm: return tempValue + sysData.ampl.offset;
		//    case audBuV: return tempValue - 107.0 + sysData.ampl.offset;
		//    case aumV: return 10 * log10(0.001 * qPow(tempValue * 1e-3, 2) / 50.0) + sysData.ampl.offset;
		//    default: return tempValue + sysData.ampl.offset;
		case audBm:
			return tempValue;
		case audBuV:
			return tempValue + 107.0;
		case aumV:
			return (qSqrt((qPow(10, (tempValue / 10))) / 1000 * 50)) * 1000;
		default:
			return tempValue;
	}
}

//获取从底层单片机传上来的指令中的值（解析在handleScpiCommonder()中进行）
void tSysScpi::saveQvalueFromModule(QString tempValue)
{
	rcvFormModuleBuffer = tempValue;
}
QString tSysScpi::getQvalueFromModule(void)
{
	return rcvFormModuleBuffer;
}

//解析远程下发的数字调制外部调制序列
void tSysScpi::analysisDigiSequencedataFormLan(void)
{
	int len = sequenceStr.length();
	int i = 0;
	int cnt, Rema = 0;
	QString temp;
	sysData.digital.sequenceCntBefore = sequenceStr.length();
	if (len <= 0 || len > 24276)
	{
		return;
	}
	cnt = len / 8;
	Rema = len % 8;
	for (i = 0; i < cnt; i++)
	{
		temp = sequenceStr.mid(i * 8, 8);
		sysData.digital.sequence[i] = bqstringToUchar(temp);
	}

	if (Rema == 0)
	{
		sysData.digital.sequenceCnt = cnt;
	} else
	{
		temp = sequenceStr.mid(cnt * 8);
		sysData.digital.sequence[i] = bqstringToUchar(temp);
		sysData.digital.sequenceCnt = cnt + 1;
	}

}

