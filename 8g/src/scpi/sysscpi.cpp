#include "sysscpi.h"

//构造
tSysScpi::tSysScpi()
{
	loadLogarithmDefineData();

	char tempChar[32] = { };

	versionString = "V" + QString(intToString(VER_MAJOR, tempChar)).trimmed() + "." + QString(intToString(VER_MINOR, tempChar)).trimmed() + "." + QString(intToString(VER_REVISION, tempChar)).trimmed() + "." + QString(intToString(VER_BUILD, tempChar)).trimmed() + "." + QString(intToString(VER_TAG, tempChar)).trimmed();

	usbDiscFailNums = 0;
	returnEofString = "";
	scpiBuffer = "";
	fileSelectName = "";
	filesExport = 0;
	gpibReaded = false;
	gpibList.clear();
	isUpdating = false;
	isRcvingLogoFile = false;

	for (int i = 0; i < sizeof SCPIRESULTEOF / sizeof SCPIRESULTEOF[0]; i++)
	{
		returnEofString += QChar(SCPIRESULTEOF[i]);
	}

	memset(&sysData.scpiData, 0, sizeof sysData.scpiData);
	sysData.scpiData.stateL = true;
	posPeakList.clear();
	closeScpiPortOfAll();
	discLibraryInitialize();
	serverSocket = NULL;
	//clientSocket = NULL;
	for (int i = 0; i < MAXTCPCLIENTS; i++)
	{
		clientSockets[i].socket = NULL;
		clientSockets[i].isReading = false;
		clientSockets[i].isWriting = false;
		clientSockets[i].sendTime = QDateTime::currentDateTime();
		clientSockets[i].recvTime = QDateTime::currentDateTime();
	}

	cSocket = new QTcpSocket(this);
	// openServerSocket(); (SA2031A QT的Tsocket无法使用 ， 使用linux写的socket)
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

	loadDeviceType();
	loadDiveceLogo();

	//加载用户状态
	memset(tempChar, 0, sizeof tempChar);

	for (int i = 0; i < TRACECOUNT; i++)
	{
		traceDataString[i] = "";
		traceDataOrganized[i] = false;
	}

	for (int i = 0; i < USERCOUNT; i++)
	{
		fileName = QCoreApplication::applicationDirPath() + "/userparam" + QString(intToString(i, tempChar)).trimmed();
		sysData.userSelfState[i].saved = false;
		sysData.userSelfState[i].centerFreq = 0;

		if (QFile(fileName).exists())
		{
			FILE * fp;
			fp = fopen(fileName.toStdString().c_str(), "rb");
			if (fp != NULL)
			{
				fread(&sysData, sizeof sysData, 1, fp);
				fclose(fp);
			}

			sysData.userSelfState[i].saved = true;
			sysData.userSelfState[i].centerFreq = sysData.freq.center;
		}
	}
}

//析构
tSysScpi::~tSysScpi()
{

}

//设置中心频率
int tSysScpi::setFrequencyOfCenter(double value)
{
	double tempValue = value;
	double minFreq = 0;
	double maxFreq = 0;
	
	if(tempValue > ZCMAXFREQ)
		sysData.freq.isLowChannel = false;

	if (sysData.source.isTrackGenOn)
	{
		if (sysData.ampt.isPreamptOn)
			minFreq = MINPRETRACKFREQ + sysData.freq.offset;
		else
			minFreq = MINTRACKFREQ + sysData.freq.offset;
	} else if (sysData.ampt.isPreamptOn && !sysData.freq.isLowChannel)
	{
		minFreq = 100e3 + sysData.freq.offset;
	} else if (sysData.freq.isLowChannel)
	{
		minFreq = ZCMINFREQ + sysData.freq.offset;
	} else
	{
		minFreq = MINFREQ + sysData.freq.offset;
	}

	if (sysData.source.isTrackGenOn && !sysData.options.isTrack36GHzOn)
	{
		if (sysData.ampt.isPreamptOn)
			maxFreq = MAXPRETRACKFREQ + sysData.freq.offset;
		else
			maxFreq = MAXTRACKFREQ + sysData.freq.offset;
	} else if (sysData.freq.isLowChannel)
	{
		maxFreq = ZCMAXFREQ + sysData.freq.offset;
		if (sysData.isZcCalibrating)
			maxFreq = 21e6 + sysData.freq.offset;//ZCMAXFREQ = 21e6 when in cal
	} else
	{
		maxFreq = MAXFREQ + sysData.freq.offset;
	}

	sysData.span.prvSpan = sysData.span.nowSpan;

	if (tempValue < minFreq)
	{
		tempValue = minFreq;
	} else if (tempValue > maxFreq)
	{
		tempValue = maxFreq;
	}

	if (sysData.span.span != 0 && tempValue > maxFreq - MINSPAN / 2)
	{
		tempValue = maxFreq - MINSPAN / 2;
	}
	tempValue = tempValue;
	double span1 = tempValue - minFreq;
	double span2 = maxFreq - tempValue;
	double span = (span1 > span2) ? span2 : span1;
	span *= 2;

	if (sysData.span.span > span)
	{
		sysData.span.span = span;
	}

	sysData.span.span = setSpanOfMeasure(sysData.span.span);
	sysData.freq.center = tempValue * 1.0;

	sysData.freq.start = sysData.freq.center - sysData.span.span / 2;
	sysData.freq.stop = sysData.freq.center + sysData.span.span / 2;

	if (sysData.freq.cfStepAuto)
	{
		sysData.freq.cfStep = sysData.span.span / 10;

		if (sysData.span.span == 0)
		{
			sysData.freq.cfStep = 1e3;
		}
	}

	if(sysData.freq.stop < ZCMAXFREQ)
		sysData.freq.isLowChannel = true;
	else
		sysData.freq.isLowChannel = false;
	
	sysData.span.nowSpan = sysData.span.span;
	zeroSpanHandle();
	controlRf();

	return __SCPI_SUCCESS;
}

//设置中心频率
int tSysScpi::setFrequencyOfCenter(QString value)
{
	bool isOk = false;
	double tempValue = 0;

	tempValue = getFreqValue(value, &isOk);

	if (isOk)
	{
		return setFrequencyOfCenter(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//方向键设置中心频率
int tSysScpi::setFrequencyOfCenterWithArrow(bool up)
{
	if (up)
	{
		return setFrequencyOfCenter(sysData.freq.center + sysData.freq.cfStep);
	} else
	{
		return setFrequencyOfCenter(sysData.freq.center - sysData.freq.cfStep);
	}
}

//脉冲旋钮设置中心频率
int tSysScpi::setFrequencyOfCenterWithRpg(int value)
{
	if (sysData.span.span <= 0)
	{
		if (value > 0)
		{
			//      return setFrequencyOfCenter(sysData.freq.center + 0.01 * sysData.freq.center);
			return setFrequencyOfCenter(sysData.freq.center + 1e3);
		} else
		{
			//	  return setFrequencyOfCenter(sysData.freq.center - 0.01 * sysData.freq.center);
			return setFrequencyOfCenter(sysData.freq.center - 1e3);
		}
	} else
	{
		return setFrequencyOfCenter(sysData.freq.center + 0.01 * value * sysData.span.span);
	}
}


//设置起始频率
int tSysScpi::setFrequencyOfStart(double value)
{
	double tempValue = value;
	double minFreq = 0;
	double maxFreq = 0;
	
	if(tempValue > ZCMAXFREQ)
		sysData.freq.isLowChannel = false;

	//  if (sysData.source.isTrackGenNetworkOn)
	//  {
	//	minFreq = MINTRACKFREQ + sysData.freq.offset;
	//  }else
	if (sysData.source.isTrackGenOn)
	{
		if (sysData.ampt.isPreamptOn)
			minFreq = MINPRETRACKFREQ + sysData.freq.offset;
		else
			minFreq = MINTRACKFREQ + sysData.freq.offset;
	} else if (sysData.ampt.isPreamptOn && !sysData.freq.isLowChannel)
	{
		minFreq = 100e3 + sysData.freq.offset;
	} else if (sysData.freq.isLowChannel)
	{
		minFreq = ZCMINFREQ + sysData.freq.offset;
	} else
	{
		minFreq = MINFREQ + sysData.freq.offset;
	}

	if (sysData.source.isTrackGenOn && !sysData.options.isTrack36GHzOn)
	{
		if (sysData.ampt.isPreamptOn)
			maxFreq = MAXPRETRACKFREQ + sysData.freq.offset;
		else
			maxFreq = MAXTRACKFREQ + sysData.freq.offset;
	} else if (sysData.freq.isLowChannel)
	{
		maxFreq = ZCMAXFREQ + sysData.freq.offset;
		if (sysData.isZcCalibrating)
			maxFreq = 21e6 + sysData.freq.offset;//ZCMAXFREQ = 21e6 when in cal
	} else
	{
		maxFreq = MAXFREQ + sysData.freq.offset;
	}

	sysData.span.prvSpan = sysData.span.nowSpan;

	if (tempValue < minFreq)
	{
		tempValue = minFreq;
	} else if (tempValue > maxFreq)
	{
		tempValue = maxFreq;
	}

	tempValue = tempValue;
	sysData.freq.start = tempValue;

	if (sysData.freq.start >= sysData.freq.stop)
	{
		sysData.freq.stop = sysData.freq.start + 1e3;
	}

	sysData.span.span = sysData.freq.stop - sysData.freq.start;

	sysData.freq.center = sysData.freq.start + sysData.span.span / 2;

	if (sysData.freq.cfStepAuto)
	{
		sysData.freq.cfStep = sysData.span.span / 10;

		if (sysData.span.span == 0)
		{
			sysData.freq.cfStep = 1e3;
		}
	}
	
	if(sysData.freq.stop < ZCMAXFREQ)
		sysData.freq.isLowChannel = true;
	else
		sysData.freq.isLowChannel = false;

	sysData.span.nowSpan = sysData.span.span;
	zeroSpanHandle();
	controlRf();

	return __SCPI_SUCCESS;
}

//设置起始频率
int tSysScpi::setFrequencyOfStart(QString value)
{
	bool isOk = false;
	double tempValue = 0;

	tempValue = getFreqValue(value, &isOk);

	if (isOk)
	{
		return setFrequencyOfStart(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//方向键设置起始频率
int tSysScpi::setFrequencyOfStartWithArrow(bool up)
{
	if (up)
	{
		return setFrequencyOfStart(sysData.freq.start + sysData.span.span * 0.1);
	} else
	{
		return setFrequencyOfStart(sysData.freq.start - sysData.span.span * 0.1);
	}
}

//脉冲旋钮设置起始频率
int tSysScpi::setFrequencyOfStartWithRpg(int value)
{
	if (sysData.span.span <= 0)
	{
		return setFrequencyOfStart(sysData.freq.start + sysData.freq.start * value * 0.01);
	} else
	{
		return setFrequencyOfStart(sysData.freq.start + sysData.span.span * value * 0.01);
	}
}

//设置终止频率
int tSysScpi::setFrequencyOfStop(double value)
{
	double tempValue = value;
	double minFreq = 0;
	double maxFreq = 0;
	
	if(tempValue > ZCMAXFREQ)
		sysData.freq.isLowChannel = false;

	if (sysData.source.isTrackGenOn)
	{
		if (sysData.ampt.isPreamptOn)
			minFreq = MINPRETRACKFREQ + sysData.freq.offset;
		else
			minFreq = MINTRACKFREQ + sysData.freq.offset;
	} else if (sysData.ampt.isPreamptOn && !sysData.freq.isLowChannel)
	{
		minFreq = 100e3 + sysData.freq.offset;
	} else if (sysData.freq.isLowChannel)
	{
		minFreq = ZCMINFREQ + sysData.freq.offset;
	} else
	{
		minFreq = MINFREQ + sysData.freq.offset;
	}

	if (sysData.source.isTrackGenOn && !sysData.options.isTrack36GHzOn)
	{
		if (sysData.ampt.isPreamptOn)
			maxFreq = MAXPRETRACKFREQ + sysData.freq.offset;
		else
			maxFreq = MAXTRACKFREQ + sysData.freq.offset;
	} else if (sysData.freq.isLowChannel)
	{
		maxFreq = ZCMAXFREQ + sysData.freq.offset;
		if (sysData.isZcCalibrating)
			maxFreq = 21e6 + sysData.freq.offset;//ZCMAXFREQ = 21e6 when in cal		
	} else
	{
		maxFreq = MAXFREQ + sysData.freq.offset;
	}

	sysData.span.prvSpan = sysData.span.nowSpan;

	if (tempValue < minFreq)
	{
		tempValue = minFreq;
	} else if (tempValue > maxFreq)
	{
		tempValue = maxFreq;
	}

	tempValue = tempValue;
	sysData.freq.stop = tempValue;

	if (sysData.freq.stop < sysData.freq.start)
	{
		sysData.freq.start = sysData.freq.stop - 1e3;
	}

	sysData.span.span = sysData.freq.stop - sysData.freq.start;

	sysData.freq.center = sysData.freq.start + sysData.span.span / 2;

	if (sysData.freq.cfStepAuto)
	{
		sysData.freq.cfStep = sysData.span.span / 10;

		if (sysData.span.span == 0)
		{
			sysData.freq.cfStep = 1e3;
		}
	}
	
	if(sysData.freq.stop < ZCMAXFREQ)
		sysData.freq.isLowChannel = true;
	else
		sysData.freq.isLowChannel = false;

	sysData.span.nowSpan = sysData.span.span;
	zeroSpanHandle();
	controlRf();

	return __SCPI_SUCCESS;
}

//设置终止频率
int tSysScpi::setFrequencyOfStop(QString value)
{
	bool isOk = false;
	double tempValue = 0;
	int result = 0;

	tempValue = getFreqValue(value, &isOk);

	if (isOk)
	{
		return setFrequencyOfStop(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//方向键设置终止频率
int tSysScpi::setFrequencyOfStopWithArrow(bool up)
{
	if (up)
	{
		return setFrequencyOfStop(sysData.freq.stop + sysData.span.span * 0.1);
	} else
	{
		return setFrequencyOfStop(sysData.freq.stop - sysData.span.span * 0.1);
	}
}

//脉冲旋钮设置终止频率
int tSysScpi::setFrequencyOfStopWithRpg(int value)
{
	if (sysData.span.span <= 0)
	{
		return setFrequencyOfStop(sysData.freq.stop + sysData.freq.stop * value * 0.01);
	} else
	{
		return setFrequencyOfStop(sysData.freq.stop + sysData.span.span * value * 0.01);
	}
}

//设置中心频率步进
int tSysScpi::setFrequencyOfCenterStep(double value)
{
	sysData.freq.cfStep = value;

	if (sysData.freq.cfStep < MINCFSTEP)
	{
		sysData.freq.cfStep = MINCFSTEP;
	} else if (sysData.freq.cfStep > MAXCFSTEP)
	{
		sysData.freq.cfStep = MAXCFSTEP;
	}

	sysData.freq.cfStep = (unsigned long long) sysData.freq.cfStep;

	if (sysData.freq.cfStepAuto)
	{
		sysData.freq.cfStepAuto = false;
	}

	return __SCPI_SUCCESS;
}

//设置中心频率步进
int tSysScpi::setFrequencyOfCenterStep(QString value)
{
	bool isOk = false;
	double tempValue = 0;
	int result = 0;

	tempValue = getFreqValue(value, &isOk);

	if (isOk)
	{
		return setFrequencyOfCenterStep(tempValue);
	} else
	{
		result = __SCPI_FAILED;
	}
}

//方向键设置中心频率步进
int tSysScpi::setFrequencyOfCenterStepWithArrow(bool up)
{
	if (up)
	{
		return setFrequencyOfCenterStep(zoomInData(sysData.freq.cfStep, 2));
	} else
	{
		return setFrequencyOfCenterStep(zoomOutData(sysData.freq.cfStep, 2));
	}
}

//脉冲旋钮设置中心频率步进
int tSysScpi::setFrequencyOfCenterStepWithRpg(int value)
{
	return setFrequencyOfCenterStep(sysData.freq.cfStep + value * 5e6);
}

//设置中心频率步进自动
int tSysScpi::setFrequencyOfCenterStepAuto(int value)
{
	sysData.freq.cfStepAuto = value == 1;

	if (sysData.freq.cfStepAuto)
	{
		sysData.freq.cfStep = sysData.span.span / 10.0;
	}

	return __SCPI_SUCCESS;
}

//设置中心频率步进自动
int tSysScpi::setFrequencyOfCenterStepAuto(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	int tempValue = getBoolValue(value.toUpper().trimmed(), &isOk);

	if (isOk)
	{
		return setFrequencyOfCenterStepAuto(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置频率偏置
int tSysScpi::setFrequencyOffset(double value)
{
	double tempValue = value;
	double centerF = sysData.freq.center - sysData.freq.offset;
	double startF = sysData.freq.start - sysData.freq.offset;
	double stopF = sysData.freq.stop - sysData.freq.offset;

	if (tempValue < MINFREQOFFSET)
	{
		tempValue = MINFREQOFFSET;
	} else if (tempValue > MAXFREQOFFSET)
	{
		tempValue = MAXFREQOFFSET;
	}

	sysData.freq.offset = tempValue;
	sysData.freq.offset = sysData.freq.offset;
	sysData.freq.center = centerF + sysData.freq.offset;
	sysData.freq.start = startF + sysData.freq.offset;
	sysData.freq.stop = stopF + sysData.freq.offset;

	double tempFreq = sysData.options.am_carrierfreq - sysData.freq.offset;
	sysData.options.am_carrierfreq = tempFreq + sysData.freq.offset;

	return __SCPI_SUCCESS;
}

//设置频率偏置
int tSysScpi::setFrequencyOffset(QString value)
{
	bool isOk = false;
	double tempValue = 0;
	int result = 0;

	tempValue = getFreqValue(value, &isOk);

	if (isOk)
	{
		return setFrequencyOffset(tempValue);
	} else
	{
		result = __SCPI_FAILED;
	}
}

//方向键设置中心频率
int tSysScpi::setFrequencyOfOffsetWithArrow(bool up)
{
	if (up)
	{
		return setFrequencyOffset(zoomInData(sysData.freq.offset, 2));
	} else
	{
		return setFrequencyOffset(zoomOutData(sysData.freq.offset, 2));
	}
}

//脉冲旋钮设置中心频率
int tSysScpi::setFrequencyOfOffsetWithRpg(int value)
{
	return setFrequencyOffset(sysData.freq.offset + 5e6 * value);
}

//设置频率参考
int tSysScpi::setFrequencyOfReference(int value)
{
	sysData.freq.refInternal = value == 1;
	controlRf();
	
	return __SCPI_SUCCESS;
}

//设置扫宽
int tSysScpi::setFrequencyOfSpan(double value)
{
	double temp = setSpanOfMeasure(value);
	sysData.span.prvSpan = sysData.span.nowSpan;
	double minFreq = MINFREQ + sysData.freq.offset;
	double maxFreq = MAXFREQ + sysData.freq.offset;
	double minSpan = MINSPAN;
	double maxSpan = MAXSPAN;

	if((sysData.freq.start + temp) > ZCMAXFREQ)
		sysData.freq.isLowChannel = false;
		
	if (sysData.freq.isLowChannel)
	{
		minFreq = ZCMINFREQ;
		maxFreq = ZCMAXFREQ;
		if (sysData.isZcCalibrating)
			maxFreq = 21e6;//ZCMAXFREQ = 21e6 when in cal		
		maxSpan = ZCMAXFREQ - ZCMINFREQ;
	}

	if (temp <= 0)
	{
		temp = 0;
	} else if (temp <= minSpan)
	{
		temp = minSpan;
	} else if (temp >= maxSpan)
	{
		temp = maxSpan;
	}

//	if (sysData.freq.isLowChannel && temp <= 0)
//	{
//		temp = minSpan;
//	}

	//temp = temp;
	sysData.span.span = temp;

	if ((sysData.freq.center - sysData.span.span / 2) <= minFreq)
	{
		sysData.freq.start = minFreq;
		sysData.freq.stop = sysData.freq.start + sysData.span.span;
	} else if ((sysData.freq.center + sysData.span.span / 2) >= maxFreq)
	{
		sysData.freq.stop = maxFreq;
		sysData.freq.start = sysData.freq.stop - sysData.span.span;
	} else
	{
		sysData.freq.start = sysData.freq.center - sysData.span.span / 2;
		sysData.freq.stop = sysData.freq.center + sysData.span.span / 2;
	}

	sysData.freq.center = sysData.freq.start + sysData.span.span / 2;

	/*
	 //--- @直采变频关于频率范围限制 ----
	 double centerF = sysData.freq.center;
	 double spanF = temp;
	 double startF = sysData.freq.start;
	 double stopF = sysData.freq.stop;

	 startF = centerF - spanF / 2.0;
	 stopF = centerF + spanF / 2.0;
	 if (startF < (ZCMINFREQ + sysData.freq.offset) && stopF > (ZCMAXFREQ + sysData.freq.offset))
	 {
	 startF = ZCMINFREQ +  + sysData.freq.offset;
	 }
	 spanF = stopF - startF;
	 sysData.freq.start = startF;
	 sysData.freq.stop = stopF;
	 sysData.span.span = spanF;
	 sysData.freq.center = startF + spanF / 2.0;
	 //--------------------------------
	 */

	if (sysData.freq.cfStepAuto)
	{
		sysData.freq.cfStep = sysData.span.span / 10;

		if (sysData.span.span == 0)
		{
			sysData.freq.cfStep = 1e3;
		}
	}
	if(sysData.freq.stop < ZCMAXFREQ)
		sysData.freq.isLowChannel = true;
	else
		sysData.freq.isLowChannel = false;
	sysData.span.nowSpan = sysData.span.span;
	zeroSpanHandle();
	controlRf();
	
	return __SCPI_SUCCESS;
}

//设置扫宽
int tSysScpi::setFrequencyOfSpan(QString value)
{
	bool isOk = false;
	double tempValue = 0;
	int result = 0;

	tempValue = getFreqValue(value, &isOk);

	if (isOk)
	{
		return setFrequencyOfSpan(tempValue);
	} else
	{
		result = __SCPI_FAILED;
	}
}

//方向键设置扫宽
int tSysScpi::setFrequencyOfSpanWithArrow(bool up)
{
	double center1 = sysData.freq.center;
	double center2 = 0;

	if (up)
	{
		setFrequencyOfSpan(zoomInData(sysData.span.span, 2));
	} else
	{
		setFrequencyOfSpan(zoomOutData(sysData.span.span, 2));
	}

	center2 = sysData.freq.center;

	if ((unsigned long long) center1 == (unsigned long long) center2)
	{
		int index1 = sysData.sweep.sweepPoints * 0.25;
		int index2 = sysData.sweep.sweepPoints * 0.75;

		if ((index1 % 2) != 0)
		{
			index1++;
		}

		if ((index2 % 2) == 0)
		{
			index2++;
		}

		double value = 0;

		for (int i = 0; i < TRACECOUNT; i++)
		{
			if (sysData.trace[i].state == tsClearWrite)
			{
				value = (sysData.trace[i].value[index1] + sysData.trace[i].value[index2]) / 2;

				for (int j = 0; j < index1; j++)
				{
					sysData.trace[i].value[j] = value;
				}

				for (int j = index2; j < sysData.sweep.sweepPoints; j++)
				{
					sysData.trace[i].value[j] = value;
				}
			}
		}

		sysData.freq.lineZero = true;
	}

	return __SCPI_SUCCESS;
}

//脉冲旋钮设置扫宽
int tSysScpi::setFrequencyOfSpanWithRpg(int value)
{
	double tempValue = sysData.span.span;

	if (tempValue > 0)
	{
		tempValue = tempValue + value * tempValue / sysData.sweep.sweepPoints;
	} else
	{
		tempValue = sysData.sweep.sweepPoints;
	}

	return setFrequencyOfSpan(tempValue);
}

//设置低频通道
int tSysScpi::setFrequencyOfLowChannel(int value)
{
	sysData.freq.isLowChannel = value == 1;

	if (value == 1)
	{
		sysData.freq.start = ZCMINFREQ + sysData.freq.offset;
		sysData.freq.stop = ZCMAXFREQ + sysData.freq.offset;
		if (sysData.isZcCalibrating)
			sysData.freq.stop = 21e6 + sysData.freq.offset;//ZCMAXFREQ = 21e6 when in cal
		sysData.span.span = sysData.freq.stop - sysData.freq.start;
		sysData.freq.center = sysData.freq.start + sysData.span.span / 2.0;
		sysData.freq.cfStep = ZCMAXSPAN / 10;
		sysData.detector.detMode = dmPosPeak;

		reCalculateZcParams();
	} else
	{
		sysData.freq.start = MINFREQ + sysData.freq.offset;
		sysData.freq.stop = MAXFREQ + sysData.freq.offset;
		sysData.span.span = sysData.freq.stop - sysData.freq.start;
		sysData.freq.center = sysData.freq.start + sysData.span.span / 2.0;
		sysData.freq.cfStep = MAXSPAN / 10;
		sysData.detector.detMode = dmPosPeak;

		resetZcParams();
	}

	setAmptOfRefLevel(sysData.ampt.refLevel);
	controlRf();
}

//设置频率刻度类型
int tSysScpi::setFrequencyOfScaleType(int value)
{
	sysData.freq.typeLine = value == 1;

	return __SCPI_SUCCESS;
}
//设置频率刻度类型
int tSysScpi::setFrequencyOfScaleType(QString value)
{
  if (value.isNull() || value.isEmpty())
  {
  	return __SCPI_FAILED;
  }

  QString tempValue = value.toUpper().trimmed();

  if (tempValue == "LIN" || tempValue == "LINear")
  {
  	return setFrequencyOfScaleType(1);
  } else if (tempValue == "LOG" || tempValue == "LOGarithmic")
  {
    return setFrequencyOfScaleType(0);
  } else
  {
  	return __SCPI_FAILED;
  }
}

//参考电平和衰减器自动耦合
void tSysScpi::autoAmptAndReflevel(void)
{
	if(!sysData.ampt.isPreamptOn)
	{
		if(sysData.ampt.refLevel <= 0)
		{
			sysData.ampt.attRf_Auto = 10;
			sysData.ampt.attIf = 20 - (int)qAbs(sysData.ampt.refLevel);
			sysData.ampt.ifSoftValue = qAbs(sysData.ampt.refLevel - (int)sysData.ampt.refLevel);
		} else
		{
			sysData.ampt.attIf = 20;
			sysData.ampt.attRf_Auto = (int)sysData.ampt.refLevel + 10;
		}
	} else
	{
		if(sysData.ampt.refLevel <= -20)
		{
			sysData.ampt.attRf_Auto = 10;
			sysData.ampt.attIf = 40 - (int)qAbs(sysData.ampt.refLevel);
			//sysData.ampt.ifSoftValue =
		} else
		{
			sysData.ampt.attIf = 20;
			sysData.ampt.attRf_Auto = 30 - (int)qAbs(sysData.ampt.refLevel);
		}
	}

}

//设置参考电平
int tSysScpi::setAmptOfRefLevel(double value)
{
	getAmptMaxMinLimit();

	double tempValue = value;
	double temp = 0;

	if (tempValue < sysData.ampt.refLevelMin)
	{
		tempValue = sysData.ampt.refLevelMin;
	} else if (tempValue > sysData.ampt.refLevelMax)
	{
		tempValue = sysData.ampt.refLevelMax;
	}

	sysData.ampt.refLevel = tempValue;
	double refLeveldBmValue = getRefLeveldBmValue();

	autoAmptAndReflevel();

	if(sysData.ampt.attAuto)
	{
		sysData.ampt.attRf = sysData.ampt.attRf_Auto;
	} else//rf att and if att not couple according to doc of cal
	{
		if(!sysData.ampt.isPreamptOn)
		{
			double maxValue = sysData.ampt.attRf - 10;

			if(refLeveldBmValue > maxValue)
				refLeveldBmValue = maxValue;

			if(refLeveldBmValue > -10)
			{
				//大于或等于衰减器自动设置且不小于10dB
				if(sysData.ampt.attRf_Auto <= 10 && sysData.ampt.attRf <= 10)
					sysData.ampt.attRf = 10;
				else if(sysData.ampt.attRf_Auto > 10 && sysData.ampt.attRf < sysData.ampt.attRf_Auto)
					sysData.ampt.attRf = sysData.ampt.attRf_Auto;
				//参考电平max=ATR-10
				if(refLeveldBmValue >= (sysData.ampt.attRf - 10))
					refLeveldBmValue = sysData.ampt.attRf - 10;
				
				if(refLeveldBmValue > -10 && refLeveldBmValue <= 0)
					sysData.ampt.attIf = 30 - (int)qAbs(refLeveldBmValue) - sysData.ampt.attRf;
				else if(refLeveldBmValue > 0)
					sysData.ampt.attIf = 30 - sysData.ampt.attRf + (int)qAbs(refLeveldBmValue);

			} else
			{
				if(sysData.ampt.attRf <= 0)
					sysData.ampt.attRf = 0;
				sysData.ampt.attIf = 30 - sysData.ampt.attRf - (int)qAbs(refLeveldBmValue);
				if(sysData.ampt.attIf <= 0)
				{
					sysData.ampt.ifSoftValue = qAbs(sysData.ampt.attIf);
					sysData.ampt.attIf = 0;
				}
				if(sysData.ampt.attIf > MAXATT)
					sysData.ampt.attIf = MAXATT;

			}
		} else
		{
			double maxValue = sysData.ampt.attRf - 30;
			if(refLeveldBmValue > -30)
			{
				//大于或等于衰减器自动设置且不小于10dB
				if(sysData.ampt.attRf_Auto <= 10 && sysData.ampt.attRf <= 10)
					sysData.ampt.attRf = 10;
				else if(sysData.ampt.attRf_Auto > 10 && sysData.ampt.attRf < sysData.ampt.attRf_Auto)
					sysData.ampt.attRf = sysData.ampt.attRf_Auto;

				
					
				if(refLeveldBmValue >= maxValue)
					refLeveldBmValue = maxValue;
				sysData.ampt.attIf = 50 - sysData.ampt.attRf - (int)qAbs(refLeveldBmValue);
			}else
			{
				if(sysData.ampt.attRf < 0)
					sysData.ampt.attRf = 0;
				sysData.ampt.attIf = 50 - sysData.ampt.attRf - (int)qAbs(refLeveldBmValue);
				if(sysData.ampt.attIf <= 0)
				{
					sysData.ampt.attIf = 0;
					sysData.ampt.ifSoftValue = qAbs(sysData.ampt.attIf);
				}
			}
		}
	}

	sysData.ampt.refLevel = refLeveldBmValue;

	if(sysData.ampt.attIf < 0)
		sysData.ampt.attIf = 0;
	if(sysData.ampt.attIf > MAXATT)
		sysData.ampt.attIf = MAXATT;

	if(sysData.ampt.attRf < 0)
		sysData.ampt.attRf = 0;
	if(sysData.ampt.attRf > MAXATT)
		sysData.ampt.attRf = MAXATT;

	sysData.ampt.att = sysData.ampt.attRf;

	controlRf();

	return __SCPI_SUCCESS;
}

//设置参考电平
int tSysScpi::setAmptOfRefLevel(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	double tempValue = getAmptValue(value.toUpper().trimmed(), &isOk);

	if (isOk)
	{
		return setAmptOfRefLevel(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//方向键设置参考电平
int tSysScpi::setAmptOfRefLevelWithArrow(bool up)
{
	double tempValue = sysData.ampt.refLevel;

	if (up)
	{
		if (sysData.ampt.scaleLine)
		{
			tempValue = getRefLevelValue(getRefLeveldBmValue() + sysData.ampt.refOffset + 1.0);
		} else
		{
			tempValue += sysData.ampt.scaleDiv;
		}
	} else
	{
		if (sysData.ampt.scaleLine)
		{
			tempValue = getRefLevelValue(getRefLeveldBmValue() + sysData.ampt.refOffset - 1.0);
		} else
		{
			tempValue -= sysData.ampt.scaleDiv;
		}
	}

	return setAmptOfRefLevel(tempValue);
}

//脉冲旋钮设置参考电平
int tSysScpi::setAmptOfRefLevelWithRpg(int value)
{
	if (sysData.ampt.scaleLine)
	{
		return setAmptOfRefLevel(getRefLevelValue(getRefLeveldBmValue() + sysData.ampt.refOffset + 1.0 * value * 0.01));
	} else
	{
		return setAmptOfRefLevel(sysData.ampt.refLevel + sysData.ampt.scaleDiv * value * 0.01);
	}
}

//设置衰减自动
int tSysScpi::setAmptOfAttAuto(int value)
{
	sysData.ampt.attAuto = value == 1;

	if (sysData.ampt.attAuto)
	{
		setAmptOfRefLevel(sysData.ampt.refLevel);
		controlRf();
	}

	return __SCPI_SUCCESS;
}
//设置前置放大
//int tSysScpi::setPreamptOfAttAuto(int value)
//{
//	sysData.ampt.isPreamptOn = (value == 1);
//
//	if (sysData.ampt.isPreamptOn)
//	{
//		sysData.ampt.preamtOn = 0x01;
//		//sysData.ampt.preamtOn = 0x02;
//		getZcPreamplifierCalibrationData();
//		getPreamplifierCalibrationData();
//	} else
//	{
//		sysData.ampt.preamtOn = 0x02;
//		//sysData.ampt.preamtOn = 0x01;
//		getFactoryCalibrationData();
//		getZcCalibrationData();
//		getUserCalibrationData();
//	}
//
//	setFrequencyOfStart(sysData.freq.start);
//	setAmptOfRefLevel(sysData.ampt.refLevel);
//
//	controlRf();
//
//	return __SCPI_SUCCESS;
//}
int tSysScpi::setPreamptOfAttAuto(int value)
{
	sysData.ampt.isPreamptOn = (value == 1);
//	rfPreamptDown(sysData.ampt.isPreamptOn);
	controlRf();
	return __SCPI_SUCCESS;
}
//设置前置放大
int tSysScpi::setPreamptOfAttAuto(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	int tempValue = getBoolValue(value.toUpper().trimmed(), &isOk);

	if (isOk)
	{
		return setPreamptOfAttAuto(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置衰减自动
int tSysScpi::setAmptOfAttAuto(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	int tempValue = getBoolValue(value.toUpper().trimmed(), &isOk);

	if (isOk)
	{
		return setAmptOfAttAuto(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置衰减
int tSysScpi::setAmptOfAtt(double value)
{
#if 0

	getAmptMaxMinLimit();
	int attValue = (int) value;
	double temp = 0;

	if (attValue < MINATT)
	{
		attValue = MINATT;
	} else if (attValue > MAXATT)
	{
		attValue = MAXATT;
	}

//	sysData.ampt.refLevel = tempValue;
	double refLeveldBmValue = getRefLeveldBmValue();
	__var(refLeveldBmValue);

	autoAmptAndReflevel();

	double minValue = refLeveldBmValue;

	if(attValue < minValue)
		attValue = minValue;

	if(refLeveldBmValue > -10)
	{	
		__var(sysData.ampt.attRf_Auto);
		if(attValue <= sysData.ampt.attRf_Auto)
			attValue = sysData.ampt.attRf_Auto;

		if(refLeveldBmValue > -10 && refLeveldBmValue <= 0)
			sysData.ampt.attIf = 30 - (int)qAbs(refLeveldBmValue) - attValue;

		if(refLeveldBmValue > 0)
			sysData.ampt.attIf = 30 - attValue + (int)qAbs(refLeveldBmValue);

	} else
	{
		sysData.ampt.attIf = 30 - attValue - (int)qAbs(refLeveldBmValue);
		if(sysData.ampt.attIf < 0)
		{
			sysData.ampt.ifSoftValue = qAbs(sysData.ampt.attIf);
			sysData.ampt.attIf = 0;
		}

	}

	sysData.ampt.refLevel = refLeveldBmValue;
	sysData.ampt.attRf = attValue;

	if(sysData.ampt.attIf < MINATT)
		sysData.ampt.attIf = MINATT;

	if(sysData.ampt.attRf < MINATT)
		sysData.ampt.attRf = MINATT;
	if(sysData.ampt.attRf > MAXATT)
		sysData.ampt.attRf = MAXATT;

	sysData.ampt.att = sysData.ampt.attRf;

#endif
	int attValue = (int) value;
	if (attValue < MINATT)
	{
		attValue = MINATT;
	} else if (attValue > MAXATT)
	{
		attValue = MAXATT;
	}
	sysData.ampt.attRf = attValue;
	
	if(!sysData.ampt.isPreamptOn)//前放关
	{
		if(sysData.ampt.attAuto)//自耦和
		{
			if(sysData.ampt.refLevel <= 0)//参考电平
			{
				if(sysData.ampt.attRf <= 10)
					sysData.ampt.attRf = 10;
				sysData.ampt.attIf = 20 - (int)qAbs(sysData.ampt.refLevel);
				sysData.ampt.ifSoftValue = qAbs(sysData.ampt.attIf - (int)sysData.ampt.attIf);
			}
			else
			{
				sysData.ampt.attIf = 20;
				sysData.ampt.attRf =  (int)sysData.ampt.refLevel + 10;
			}

		}
		else//非耦合
		{
			if(sysData.ampt.refLevel > -10)
			{
				if(sysData.ampt.attRf <= 10)
					sysData.ampt.attRf = 10;

				if(sysData.ampt.refLevel > (sysData.ampt.attRf - 10))
					sysData.ampt.refLevel = sysData.ampt.attRf - 10;

				if(sysData.ampt.refLevel > -10 && sysData.ampt.refLevel <= 0)
					sysData.ampt.attIf = 30 - (int)qAbs(sysData.ampt.refLevel) - sysData.ampt.attRf;
				else if(sysData.ampt.refLevel > 0)
					sysData.ampt.attIf = 30 - sysData.ampt.attRf + (int)sysData.ampt.refLevel;
			}
			else
			{
				if(sysData.ampt.attRf <= 0)
					sysData.ampt.attRf = 0;		
				sysData.ampt.attIf = 30 - sysData.ampt.attRf - (int)qAbs(sysData.ampt.refLevel);
				if(sysData.ampt.attIf <= 0)
				{
					sysData.ampt.ifSoftValue = qAbs(sysData.ampt.attIf);
					sysData.ampt.attIf = 0;
				}	
				
			}
				if(sysData.ampt.refLevel > (sysData.ampt.attRf - 10))
					sysData.ampt.refLevel = sysData.ampt.attRf - 10;
				
		}
	}
	else//前放开
		{
			if(sysData.ampt.attAuto)//自耦和
			{
				if(sysData.ampt.refLevel <= -20)
				{
					if(sysData.ampt.attRf <= 10)
						sysData.ampt.attRf = 10;
					sysData.ampt.attIf = 40 - (int)qAbs(sysData.ampt.refLevel);
				}
				else
				{
					sysData.ampt.attIf = 20;
					sysData.ampt.attRf = 30 - (int)qAbs(sysData.ampt.refLevel);
				}
		
			}
			else//非耦合
			{
				if(sysData.ampt.refLevel > -30)
				{
					if(sysData.ampt.attRf < 10)
						sysData.ampt.attRf = 10;

					if(sysData.ampt.refLevel > (sysData.ampt.attRf - 30))
						sysData.ampt.refLevel = sysData.ampt.attRf - 30;

					sysData.ampt.attIf = 50 - (int)qAbs(sysData.ampt.refLevel) - sysData.ampt.attRf;
				}
				else
				{
					if(sysData.ampt.attRf < 0)
						sysData.ampt.attRf = 0;

					sysData.ampt.attIf = 50 - sysData.ampt.attRf - (int)qAbs(sysData.ampt.refLevel);
					if(sysData.ampt.attIf <= 0)
					{
						sysData.ampt.attIf = 0;
						sysData.ampt.ifSoftValue = qAbs(sysData.ampt.attIf);
					}
					if(sysData.ampt.refLevel > (sysData.ampt.attRf - 30))
							sysData.ampt.refLevel = sysData.ampt.attRf - 30;					
				}

			}
		}

	if(sysData.ampt.attIf < MINATT)
		sysData.ampt.attIf = MINATT;

	if(sysData.ampt.attRf < MINATT)
		sysData.ampt.attRf = MINATT;
	if(sysData.ampt.attRf > MAXATT)
		sysData.ampt.attRf = MAXATT;

	sysData.ampt.att = sysData.ampt.attRf;

	return __SCPI_SUCCESS;
}

//设置衰减
int tSysScpi::setAmptOfAtt(QString value)
{
	bool isOk = false;
	double tempValue = getdBValue(value, &isOk);

	if (isOk)
	{
		return setAmptOfAtt(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//方向键设置衰减
int tSysScpi::setAmptOfAttWithArrow(bool up)
{
	double tempValue = sysData.ampt.att;

	if (up)
	{
		tempValue += 10;
	} else
	{
		tempValue -= 10;
	}

	return setAmptOfAtt(tempValue);
}

//脉冲旋钮设置衰减
int tSysScpi::setAmptOfAttWithRpg(int value)
{
	double tempValue = value;

	return setAmptOfAtt(sysData.ampt.att + value);
}

//设置幅度刻度
int tSysScpi::setAmptOfScaleDiv(double value)
{
	double tempValue = value;

	if (tempValue < MINSCALEDIV)
	{
		tempValue = MINSCALEDIV;
	} else if (tempValue > MAXSCALEDIV)
	{
		tempValue = MAXSCALEDIV;
	}

	sysData.ampt.scaleDiv = tempValue;

	return __SCPI_SUCCESS;
}

//设置幅度刻度
int tSysScpi::setAmptOfScaleDiv(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	double tempValue = value.toDouble(&isOk);

	if (isOk)
	{
		return setAmptOfScaleDiv(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//方向键设置幅度刻度
int tSysScpi::setAmptOfScaleDivWithArrow(bool up)
{
	if (up)
	{
		return setAmptOfScaleDiv(zoomInData(sysData.ampt.scaleDiv, 2));
	} else
	{
		return setAmptOfScaleDiv(zoomOutData(sysData.ampt.scaleDiv, 2));
	}
}

//脉冲旋钮设置幅度刻度
int tSysScpi::setAmptOfScaleDivWithRpg(int value)
{
	return setAmptOfScaleDiv(sysData.ampt.scaleDiv + value);
}

//设置参考偏置
int tSysScpi::setAmptOfRefOffset(double value)
{
	double refLevelValue = getRefLeveldBmValue();

	double tempValue = value;

	if (tempValue < MINREFOFFSET)
	{
		tempValue = MINREFOFFSET;
	} else if (tempValue > MAXREFOFFSET)
	{
		tempValue = MAXREFOFFSET;
	}

	sysData.ampt.refOffset = tempValue;
	sysData.ampt.refLevel = getRefLevelValue(refLevelValue + sysData.ampt.refOffset);

	return __SCPI_SUCCESS;
}

//设置参考偏置
int tSysScpi::setAmptOfRefOffset(QString value)
{
	bool isOk = false;
	double tempValue = 0;

	tempValue = getdBValue(value, &isOk);

	if (isOk)
	{
		return setAmptOfRefOffset(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//方向键设置参考偏置
int tSysScpi::setAmptOfRefOffsetWithArrow(bool up)
{
	if (up)
	{
		return setAmptOfRefOffset(zoomInData(sysData.ampt.refOffset, 2));
	} else
	{
		return setAmptOfRefOffset(zoomOutData(sysData.ampt.refOffset, 2));
	}
}

//脉冲旋钮设置参考偏置
int tSysScpi::setAmptOfRefOffsetWithRpg(int value)
{
	return setAmptOfRefOffset(sysData.ampt.refOffset + value);
}

//设置刻度类型
int tSysScpi::setAmptOfScaleType(int value)
{
	double tempValue = 0;
	double tempValue2 = getdBmValue(sysData.display.dispLineValue);
	double tempValue3 = getdBmValue(sysData.trig.power);

	sysData.ampt.scaleLine = value == 1;

	if (sysData.ampt.scaleLine)
	{
		tempValue = getRefLeveldBmValue();
		sysData.ampt.refUnit = ruV;
		sysData.ampt.refLevel = getRefLevelValue(tempValue + sysData.ampt.refOffset);
		sysData.display.dispLineValue = getRefLevelValue(tempValue2);
		sysData.trig.power = getRefLevelValue(tempValue3);
		sysData.ampt.scaleDiv = sysData.ampt.refLevel / 10.0;

		for (int i = 0; i < MARKERCOUNT; i++)
		{
			if (sysData.marker[i].state == msDelta)
			{
				sysData.marker[i].markerDelta.y2Value = getRefLevelValue(sysData.marker[i].markerDelta.y2Value);
			}
		}
	} else
	{
		tempValue = getRefLeveldBmValue();
		sysData.ampt.refUnit = rudBm;
		sysData.ampt.refLevel = getRefLevelValue(tempValue + sysData.ampt.refOffset);
		sysData.display.dispLineValue = getRefLevelValue(tempValue2);
		sysData.trig.power = getRefLevelValue(tempValue3);
		sysData.ampt.scaleDiv = 10.0;

		for (int i = 0; i < MARKERCOUNT; i++)
		{
			if (sysData.marker[i].state == msDelta)
			{
				sysData.marker[i].markerDelta.y2Value = getRefLevelmVTodBm(sysData.marker[i].markerDelta.y2Value);
			}
		}
	}

	tempValue = getdBmValue(sysData.display.dispLineValue);
	sysData.display.dispLineValue = getRefLevelValue(tempValue);

	mutexAmpt.lock();

	for (int i = 0; i < TRACECOUNT; i++)
	{
		sysData.trace[i].flags = 0;
	}

	mutexAmpt.unlock();

	return __SCPI_SUCCESS;
}

//设置刻度类型
int tSysScpi::setAmptOfScaleType(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	QString tempValue = value.toUpper().trimmed();

	if (tempValue == "LIN" || tempValue == "LINear")
	{
		return setAmptOfScaleType(1);
	} else if (tempValue == "LOG" || tempValue == "LOGarithmic")
	{
		return setAmptOfScaleType(0);
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置参考单位
int tSysScpi::setAmptOfRefUnit(int value)
{
	double tempValue = getRefLeveldBmValue();
	double tempValue2 = getdBmValue(sysData.display.dispLineValue);
	double tempValue3 = getdBmValue(sysData.trig.power);

	sysData.ampt.refUnit = refUnits(value);

	sysData.ampt.refLevel = getRefLevelValue(tempValue + sysData.ampt.refOffset);
	sysData.display.dispLineValue = getRefLevelValue(tempValue2);
	sysData.trig.power = getRefLevelValue(tempValue3);

	return __SCPI_SUCCESS;
}

//设置参考单位
int tSysScpi::setAmptOfRefUnit(QString value)
{
	if (value.isNull() || value.isEmpty() || sysData.ampt.scaleLine)
	{
		return __SCPI_FAILED;
	}

	QString tempValue = value.toUpper().trimmed();

	if (tempValue == "DBM")
	{
		return setAmptOfRefUnit(rudBm);
	} else if (tempValue == "DBMV")
	{
		return setAmptOfRefUnit(rudBmV);
	} else if (tempValue == "DBUV")
	{
		return setAmptOfRefUnit(rudBuV);
	} else if (tempValue == "MW")
	{
		return setAmptOfRefUnit(ruW);
	} else if (tempValue == "MV")
	{
		return setAmptOfRefUnit(ruV);
	} else if (tempValue == "DBPW")
	{
		return setAmptOfRefUnit(rudBpW);
	} else if (tempValue == "DBUW")
	{
		return setAmptOfRefUnit(rudBuW);
	}

	return __SCPI_FAILED;
}

//设置分辨率自动
int tSysScpi::setBwOfRbwAuto(int value)
{
	if (!sysData.emc.emcOn)
	{
		sysData.bw.rbwAuto = value == 1;

		if (sysData.bw.rbwAuto)
		{
			controlRf();
		}
	} else
	{
		sysData.bw.rbwAuto = false;
	}

	return __SCPI_SUCCESS;
}

//设置分辨率自动
int tSysScpi::setBwOfRbwAuto(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	int tempValue = getBoolValue(value.toUpper().trimmed(), &isOk);

	if (isOk)
	{
		return setBwOfRbwAuto(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

////设置分辨率带宽
//int tSysScpi::setBwOfRbw(double value)
//{
//	double tempValue = value;
//
//	if (sysData.freq.isLowChannel)
//	{
//		if (tempValue > RAMCIC_ZC_HZ[0])
//		{
//			tempValue = RAMCIC_ZC_HZ[0];
//		} else if (tempValue < RAMCIC_ZC_HZ[sizeof RAMCIC_ZC_HZ / sizeof RAMCIC_ZC_HZ[0] - 1])
//		{
//			tempValue = RAMCIC_ZC_HZ[sizeof RAMCIC_ZC_HZ / sizeof RAMCIC_ZC_HZ[0] - 1];
//		} else
//		{
//			tempValue = getRecentBw(tempValue);
//		}
//
//		if (sysData.bw.rbwAuto)
//		{
//			sysData.bw.rbwAuto = false;
//		}
//
//		sysData.bw.rbw = tempValue;
//
//	} else if (!sysData.emc.emcOn)
//	{
//		if (tempValue > RAMCIC_HZ[0])
//		{
//			tempValue = RAMCIC_HZ[0];
//		} else if (tempValue < RAMCIC_HZ[sizeof RAMCIC_HZ / sizeof RAMCIC_HZ[0] - 1])
//		{
//			tempValue = RAMCIC_HZ[sizeof RAMCIC_HZ / sizeof RAMCIC_HZ[0] - 1];
//		} else
//		{
//			tempValue = getRecentBw(tempValue);
//		}
//
//		if (sysData.bw.rbwAuto)
//		{
//			sysData.bw.rbwAuto = false;
//		}
//
//		sysData.bw.rbw = tempValue;
//
//	} else
//	{
//		sysData.bw.rbw = getModifiedEmcBw(tempValue);
//
//		if (sysData.bw.rbwAuto)
//		{
//			sysData.bw.rbwAuto = false;
//		}
//
//		if (sysData.bw.vbwAuto)
//		{
//			sysData.bw.vbw = sysData.bw.rbw;
//
//			if ((unsigned long) sysData.bw.rbw == 120e3)
//			{
//				sysData.bw.vbw = 100e3;
//			}
//		}
//	}
//
//	controlRf();
//	return __SCPI_SUCCESS;
//}
//
////设置分辨率带宽
//int tSysScpi::setBwOfRbw(QString value)
//{
//	if (value.isNull() || value.isEmpty())
//	{
//		return __SCPI_FAILED;
//	}
//
//	bool isOk = false;
//	double tempValue = getFreqValue(value, &isOk);
//
//	if (isOk)
//	{
//		return setBwOfRbw(tempValue);
//	} else
//	{
//		return __SCPI_FAILED;
//	}
//}

//方向键设置分辨率带宽
int tSysScpi::setBwOfRbwWithArrow(bool up)
{
	if (sysData.emc.emcOn)
	{
		int emcSize = sizeof EMC_BW / sizeof EMC_BW[0];
		int nowIndex = -1;

		for (int i = 0; i < emcSize; i++)
		{
			if ((unsigned long) EMC_BW[i] == (unsigned long) sysData.bw.rbw)
			{
				nowIndex = i;
				break;
			}
		}

		if (nowIndex >= 0 && nowIndex < emcSize)
		{
			if (up && nowIndex > 0)
			{
				sysData.bw.rbw = EMC_BW[nowIndex - 1];
			} else if (!up && nowIndex < emcSize - 1)
			{
				sysData.bw.rbw = EMC_BW[nowIndex + 1];
			}
		} else
		{
			if (sysData.bw.vbw > EMC_BW[0])
			{
				sysData.bw.vbw = EMC_BW[0];
			} else if (sysData.bw.vbw < EMC_BW[emcSize - 1])
			{
				sysData.bw.vbw = EMC_BW[emcSize - 1];
			}
		}

		if (sysData.bw.rbwAuto)
		{
			sysData.bw.rbwAuto = false;
		}

	} else
	{
		int bwIndex = getRbwIndex();
		int bwSize = sizeof RAMCIC_HZ / sizeof RAMCIC_HZ[0];
		double tempValue = 0;

		if (sysData.freq.isLowChannel)
		{
			bwSize = sizeof RAMCIC_ZC_HZ / sizeof RAMCIC_ZC_HZ[0];

			if (up)
			{
				if (sysData.bw.rbwStepCont)
				{
					if (bwIndex > 0)
					{
						tempValue = RAMCIC_ZC_HZ[bwIndex - 1];
					} else
					{
						tempValue = RAMCIC_ZC_HZ[0];
					}
				} else
				{
					tempValue = zoomInData(sysData.bw.rbw, 2);

					if (tempValue > RAMCIC_ZC_HZ[0])
					{
						tempValue = RAMCIC_ZC_HZ[0];
					}
				}
			} else
			{
				if (sysData.bw.rbwStepCont)
				{
					if (bwIndex < (bwSize - 1))
					{
						tempValue = RAMCIC_ZC_HZ[bwIndex + 1];
					} else
					{
						tempValue = RAMCIC_ZC_HZ[bwSize - 1];
					}
				} else
				{
					tempValue = zoomOutData(sysData.bw.rbw, 2);

					if (tempValue < RAMCIC_ZC_HZ[bwSize - 1])
					{
						tempValue = RAMCIC_ZC_HZ[bwSize - 1];
					}
				}
			}

			if (sysData.bw.rbwAuto)
			{
				sysData.bw.rbwAuto = false;
			}

			sysData.bw.rbw = tempValue;

		} else
		{
			if (up)
			{
				if (sysData.bw.rbwStepCont)
				{
					if (bwIndex > 0)
					{
						tempValue = RAMCIC_HZ[bwIndex - 1];
					} else
					{
					    tempValue = RAMCIC_HZ[0];
					}
				} else
				{
					tempValue = zoomInData(sysData.bw.rbw, 3);

					if (tempValue > RAMCIC_HZ[0])
					{
						tempValue = RAMCIC_HZ[0];
					}
				}
			} else
			{
				if (sysData.bw.rbwStepCont)
				{
					if (bwIndex < (bwSize - 1))
					{
						tempValue = RAMCIC_HZ[bwIndex + 1];
					} else
					{
						tempValue = RAMCIC_HZ[bwSize - 1];
					}
				} else
				{
					tempValue = zoomOutData(sysData.bw.rbw, 3);

					if (tempValue < RAMCIC_HZ[bwSize - 1])
					{
						tempValue = RAMCIC_HZ[bwSize - 1];
					}
				}
			}

			if (sysData.bw.rbwAuto)
			{
				sysData.bw.rbwAuto = false;
			}

			sysData.bw.rbw = tempValue;
		}
	}
	if (sysData.bw.vbwAuto)
	{
			sysData.bw.vbw = sysData.bw.rbw;

			if ((unsigned long) sysData.bw.rbw == 120e3)
			{
				sysData.bw.vbw = 100e3;
			}
	}

	controlRf();
	return __SCPI_SUCCESS;
}

//脉冲旋钮设置分辨率带宽
int tSysScpi::setBwOfRbwWithRpg(int value)
{
	if (value != 0)
	{
		setBwOfRbwWithArrow(value > 0);
	}

	return __SCPI_SUCCESS;
}

//设置分辨步进模式
int tSysScpi::setBwOfRbwStepMode(int value)
{
	if (sysData.emc.emcOn)
	{
		return __SCPI_FAILED;
	}

	sysData.bw.rbwStepCont = value == 1;

	return __SCPI_SUCCESS;
}

//设置分辨步进模式
int tSysScpi::setBwOfRbwStepMode(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	QString tempValue = value.toUpper().trimmed();

	if (tempValue == "DEF" || tempValue == "DEFAULT" || tempValue == "0") // tempValue == "0"兼容老框架
	{
		return setBwOfRbwStepMode(0);
	} else if (tempValue == "CONT" || tempValue == "CONTINUOUS" || tempValue == "1")
	{
		return setBwOfRbwStepMode(1);
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置视频带宽自动
int tSysScpi::setBwOfVbwAuto(int value)
{
	sysData.bw.vbwAuto = value == 1;

	if (sysData.bw.vbwAuto)
	{
		sysData.bw.vbw = sysData.bw.rbw;

		if (sysData.emc.emcOn && (unsigned long) sysData.bw.rbw == 120e3)
		{
			sysData.bw.vbw = 100e3;
		}

		controlRf();
	} else
	{
		controlRf();
	}

	return __SCPI_SUCCESS;
}

//设置视频带宽自动
int tSysScpi::setBwOfVbwAuto(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	int tempValue = getBoolValue(value.toUpper().trimmed(), &isOk);

	if (isOk)
	{
		return setBwOfVbwAuto(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置视频带宽
int tSysScpi::setBwOfVbw(double value)
{
	double tempValue = value;

	if (tempValue > RAMCIC_VBWHZ[0])
	{
		tempValue = RAMCIC_VBWHZ[0];
	} else if (tempValue < RAMCIC_VBWHZ[sizeof RAMCIC_VBWHZ / sizeof RAMCIC_VBWHZ[0] - 1])
	{
		tempValue = RAMCIC_HZ[sizeof RAMCIC_HZ / sizeof RAMCIC_HZ[0] - 1];
	} else
	{
		tempValue = getRecentBw(tempValue);
	}

	if (sysData.bw.vbwAuto)
	{
		sysData.bw.vbwAuto = false;
	}

	sysData.bw.vbw = tempValue;
	controlRf();

	return __SCPI_SUCCESS;
}

//设置视频带宽
int tSysScpi::setBwOfVbw(QString value)
{
	bool isOk = false;
	double tempValue = getFreqValue(value, &isOk);

	if (isOk)
	{
		return setBwOfVbw(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//方向键设置视频带宽
int tSysScpi::setBwOfVbwWithArrow(bool up)
{
	if (up)
	{
		getModifiedVbw(1);
	} else
	{
		getModifiedVbw(-1);
	}

	if (sysData.bw.vbwAuto)
	{
		sysData.bw.vbwAuto = false;
	}

	controlRf();

	return __SCPI_SUCCESS;
}

//脉冲旋钮设置视频带宽
int tSysScpi::setBwOfVbwWithRpg(int value)
{
	if (value != 0)
	{
		setBwOfVbwWithArrow(value > 0);
	}

	return __SCPI_SUCCESS;
}

//获取VBW的系数
unsigned long tSysScpi::getVbwFactor(double value)
{
	double temp = value;
	unsigned long result = 0;
	double prevVbw = sysData.bw.vbw;

	int number = sizeof RAMCVC / sizeof RAMCVC[0];

	//获取自动耦合时候VBW，并计算delta
	sysData.bw.vbw = sysData.bw.rbw;
	getVbwOffset();
	int indexOfAutoVbw = getVbwIndex();

	int deltaIndex = number - indexOfAutoVbw - 1;
	unsigned long maxVbwFactor = RAMCVC[0];
	//  unsigned long minVbwFactor = RAMCVC[number - 1] +  indexOfAutoVbw * 0x10; //最小值加 0x10 * n
	unsigned long minVbwFactor = VBWMINCVC[indexOfAutoVbw];
	double deltaFactor = 0;

	//deltaIndex = 0时， 最大VBW与最小VBW间隔为0
	if (deltaIndex > 0)
	{
		deltaFactor = log10(maxVbwFactor * 1.0 / (minVbwFactor * 1.0)) / (deltaIndex);
	} else
	{
		deltaFactor = log10(maxVbwFactor * 1.0 / (minVbwFactor * 1.0));
	}

	//  qDebug() << " deltaIndex = " << deltaIndex << ", maxVbwFactor = " << maxVbwFactor << ", minVbwFactor = " << minVbwFactor << ", deltaFactor = "<< deltaFactor;
	//获取手动VBW
	sysData.bw.vbw = prevVbw;
	int indexOfManVbw = getVbwIndex();

	result = (unsigned long) (qPow(10, deltaFactor * qAbs(number - 1 - indexOfManVbw)) * minVbwFactor * 1.0);

	//VBW 手动的系数不能大于自动耦合时候的系数
	if (result >= maxVbwFactor)
	{
		result = maxVbwFactor;
	}

	return result;
}

//设置迹线平均
int tSysScpi::setBwOfAverage(double value)
{
	int tempValue = (int) value;

	if (tempValue < MINBWAVERAGE)
	{
		tempValue = MINBWAVERAGE;
	} else if (tempValue > MAXBWAVERAGE)
	{
		tempValue = MAXBWAVERAGE;
	}

	sysData.bw.bwAverage = tempValue;
	resetBwOfAverageState();

	return __SCPI_SUCCESS;
}

//设置迹线平均
int tSysScpi::setBwOfAverage(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	int tempValue = getIntValue(value, &isOk);

	if (isOk)
	{
		return setBwOfAverage(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//方向键设置迹线平均
int tSysScpi::setBwOfAverageWithArrow(bool up)
{
	if (up)
	{
		return setBwOfAverage(sysData.bw.bwAverage + 10);
	} else
	{
		return setBwOfAverage(sysData.bw.bwAverage - 10);
	}
}

//脉冲旋钮设置迹线平均
int tSysScpi::setBwOfAverageWithRpg(int value)
{
	return setBwOfAverage(sysData.bw.bwAverage + value);
}

//设置迹线平均状态
int tSysScpi::setBwOfAverageSwitch(int value)
{
	sysData.bw.bwAverageOn = false;
	sysData.bw.bwAverageFlag = 0;
	sysData.bw.bwAverageFulled = false;
	sysData.bw.bwAverageOn = value == 1;

	return __SCPI_SUCCESS;
}

//设置迹线平均状态
int tSysScpi::setBwOfAverageSwitch(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	int tempValue = getBoolValue(value.toUpper().trimmed(), &isOk);

	if (isOk)
	{
		return setBwOfAverageSwitch(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//复位迹线平均状态
int tSysScpi::resetBwOfAverageState(void)
{
	sysData.bw.bwAverageFlag = 0;
	sysData.bw.bwAverageFulled = false;

	return __SCPI_SUCCESS;
}

//设置EMC带宽测量
int tSysScpi::setBwOfEmcState(int value)
{
	sysData.emc.emcOn = value == 1;

	if (sysData.emc.emcOn)
	{
		if (!sysData.emc.emcSaved)
		{
			sysData.emc.emcSaved = true;
		}

		sysData.emc.emcRbwPrv = sysData.bw.rbw;
		sysData.emc.emcRbwPrvAuto = sysData.bw.rbwAuto;
		sysData.emc.emcRbwCont = sysData.bw.rbwStepCont;
		sysData.emc.emcVbwPrv = sysData.bw.vbw;
		sysData.emc.emcVbwPrvAuto = sysData.bw.vbwAuto;
		sysData.emc.detectorPrv = sysData.detector.detMode;

		sysData.bw.rbw = getModifiedEmcBw(sysData.bw.rbw);

		if (sysData.bw.vbwAuto)
		{
			sysData.bw.vbw = sysData.bw.rbw;

			if ((unsigned long) sysData.bw.rbw == 120e3)
			{
				sysData.bw.vbw = 100e3;
			}
		}
	} else if (sysData.emc.emcSaved)
	{
		sysData.bw.rbw = sysData.emc.emcRbwPrv;
		sysData.bw.rbwAuto = sysData.emc.emcRbwPrvAuto;
		sysData.bw.rbwStepCont = sysData.emc.emcRbwCont;
		sysData.bw.vbw = sysData.emc.emcVbwPrv;
		sysData.bw.vbwAuto = sysData.emc.emcVbwPrvAuto;
		sysData.detector.detMode = sysData.emc.detectorPrv;
		sysData.emc.emcSaved = false;
	}

	controlRf();

	return __SCPI_SUCCESS;
}

//设置EMC带宽测量
int tSysScpi::setBwOfEmcState(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	int tempValue = getBoolValue(value.toUpper().trimmed(), &isOk);

	if (isOk)
	{
		return setBwOfEmcState(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//复位频率计数
int tSysScpi::resetMarkerFctnOfFreqCount(void)
{
	if (sysData.markerFctn.isFreqCountOn)
	{
		setMarkerFunctionFreqCountOn(sysData.markerFctn.isFreqCountOn);
	}

	return __SCPI_SUCCESS;
}

//设置显示全屏
int tSysScpi::setDisplayOfFullScreen(int value)
{
	sysData.display.isFullScreen = value == 1;

	return __SCPI_SUCCESS;
}

//设置显示全屏
int tSysScpi::setDisplayOfFullScreen(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	int tempValue = getBoolValue(value.toUpper().trimmed(), &isOk);

	if (isOk)
	{
		return setDisplayOfFullScreen(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置显示窗口缩放频率
int tSysScpi::setDisplayOfZoomFrequency(int value)
{
	if (!sysData.display.isZoomOn)
	{
		return __SCPI_FAILED;
	}

	if (sysData.span.span >= 10)
	{
		sysData.span.span /= 10.0;
	}

	setFrequencyOfSpan(sysData.span.span);
	controlRf();

	return __SCPI_SUCCESS;
}

//设置显示窗口缩放
int tSysScpi::setDisplayOfZoomOn(int value)
{
	sysData.display.isZoomOn = value == 1;

	if (sysData.display.isZoomOn)
	{
		checkMultiDisplay(mdtZoom);
		sysData.mode = smZoom;
		sysData.canvasNums = 2;
	} else
	{
		sysData.mode = smSpectrum;
		sysData.canvasNums = 1;
	}

	sysData.display.zoomed = false;
	sysData.display.zoomSpan = sysData.span.span;
	sysData.display.zoomStartFreq = sysData.freq.start;
	sysData.display.zoomStopFreq = sysData.freq.stop;

	return __SCPI_SUCCESS;
}

//设置显示窗口缩放
int tSysScpi::setDisplayOfZoomOn(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	int tempValue = getBoolValue(value.toUpper().trimmed(), &isOk);

	if (isOk)
	{
		return setDisplayOfZoomOn(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置显示线
int tSysScpi::setDisplayOfLineValue(double value)
{
	getAmptMaxMinLimit();

	double minValue = sysData.ampt.refLevelMin - 10 * sysData.ampt.scaleDiv;
	double maxValue = sysData.ampt.refLevelMax;
	double tempValue = value;

	if (tempValue < minValue)
	{
		tempValue = minValue;
	} else if (tempValue > maxValue)
	{
		tempValue = maxValue;
	}

	sysData.display.dispLineValue = tempValue;

	return __SCPI_SUCCESS;
}

//方向键设置显示线
int tSysScpi::setDisplayOfLineValueWithArrow(bool up)
{
	if (up)
	{
		return setDisplayOfLineValue(sysData.display.dispLineValue + 10.0);
	} else
	{
		return setDisplayOfLineValue(sysData.display.dispLineValue - 10.0);
	}
}

//脉冲旋钮设置显示线
int tSysScpi::setDisplayOfLineValueWithRpg(int value)
{
	return setDisplayOfLineValue(sysData.display.dispLineValue + value * 0.1);
}

//设置显示线
int tSysScpi::setDisplayOfLineValue(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	double tempValue = getAmptValue(value.toUpper().trimmed(), &isOk);

	if (isOk)
	{
		return setDisplayOfLineValue(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置显示线
int tSysScpi::setDisplayOfLine(int value)
{
	sysData.display.isDispLineOn = value == 1;
	return __SCPI_SUCCESS;
}

//设置显示线
int tSysScpi::setDisplayOfLine(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	int tempValue = getBoolValue(value.toUpper().trimmed(), &isOk);

	if (isOk)
	{
		return setDisplayOfLine(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置显示参考标尺
int tSysScpi::setDisplayOfRefLevelGraticule(int value)
{
	sysData.display.isAmptGraticuleOn = value == 1;

	return __SCPI_SUCCESS;
}

//设置显示参考标尺
int tSysScpi::setDisplayOfRefLevelGraticule(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	int tempValue = getBoolValue(value.toUpper().trimmed(), &isOk);

	if (isOk)
	{
		return setDisplayOfRefLevelGraticule(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置显示网格
int tSysScpi::setDisplayOfGrid(int value)
{
	sysData.display.isGridOn = value == 1;

	return __SCPI_SUCCESS;
}

//设置显示网格
int tSysScpi::setDisplayOfGrid(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	int tempValue = getBoolValue(value.toUpper().trimmed(), &isOk);

	if (isOk)
	{
		return setDisplayOfGrid(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置显示标签
int tSysScpi::setDisplayOfLabel(int value)
{
	sysData.display.isLabelOn = value == 1;

	return __SCPI_SUCCESS;
}

//设置显示标签
int tSysScpi::setDisplayOfLabel(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	int tempValue = getBoolValue(value.toUpper().trimmed(), &isOk);

	if (isOk)
	{
		return setDisplayOfLabel(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置显示风格
int tSysScpi::setDisplayOfStyle(int value)
{
	if (value >= 0 && value <= 1)
	{
		sysData.display.winStyle = desktopStyle(value);

		emit refreshWindowState();
	}

	return __SCPI_SUCCESS;
}

//设置显示风格
int tSysScpi::setDisplayOfStyle(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	QString tempValue = value.toUpper().trimmed();

	if (tempValue == "DEF" || tempValue == "DEFAULT")
	{
		return setDisplayOfStyle(dsDefault);
	} else if (tempValue == "WINX" || tempValue == "WINXP")
	{
		return setDisplayOfStyle(dsWinXP);
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置频率线起始
int tSysScpi::setDisplayOfFreqLineStart(double value)
{
	double tempValue1 = value;
	double tempValue2 = sysData.display.freqLineStop;
	double minFreq = sysData.freq.start + sysData.freq.offset;
	double maxFreq = sysData.freq.stop + sysData.freq.offset;

	if (tempValue1 < minFreq)
	{
		tempValue1 = minFreq;
	} else if (tempValue1 > maxFreq)
	{
		tempValue1 = maxFreq;
	}

	if (tempValue2 < minFreq)
	{
		tempValue2 = minFreq;
	} else if (tempValue2 > maxFreq)
	{
		tempValue2 = maxFreq;
	}

	if (tempValue1 > tempValue2)
	{
		tempValue1 = tempValue2;
	}

	sysData.display.freqLineStart = tempValue1;
	sysData.display.freqLineStop = tempValue2;

	return __SCPI_SUCCESS;
}

//设置频率线起始
int tSysScpi::setDisplayOfFreqLineStart(QString value)
{
	bool isOk = false;
	double tempValue = 0;

	tempValue = getFreqValue(value, &isOk);

	if (isOk)
	{
		return setDisplayOfFreqLineStart(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//方向键设置频率线起始
int tSysScpi::setDisplayOfFreqLineStartWithArrow(bool up)
{
	if (up)
	{
		return setDisplayOfFreqLineStart(sysData.display.freqLineStart + sysData.span.span * 0.1);
	} else
	{
		return setDisplayOfFreqLineStart(sysData.display.freqLineStart - sysData.span.span * 0.1);
	}
}

//脉冲旋钮设置频率线起始
int tSysScpi::setDisplayOfFreqLineStartWithRpg(int value)
{
	if (sysData.span.span <= 0)
	{
		return setDisplayOfFreqLineStart(sysData.display.freqLineStart + sysData.display.freqLineStart * value * 0.01);
	} else
	{
		return setDisplayOfFreqLineStart(sysData.display.freqLineStart + sysData.span.span * value * 0.01);
	}
}

//设置频率线终止
int tSysScpi::setDisplayOfFreqLineStop(double value)
{
	double tempValue1 = sysData.display.freqLineStart;
	double tempValue2 = value;
	double minFreq = sysData.freq.start + sysData.freq.offset;
	double maxFreq = sysData.freq.stop + sysData.freq.offset;

	if (tempValue1 < minFreq)
	{
		tempValue1 = minFreq;
	} else if (tempValue1 > maxFreq)
	{
		tempValue1 = maxFreq;
	}

	if (tempValue2 < minFreq)
	{
		tempValue2 = minFreq;
	} else if (tempValue2 > maxFreq)
	{
		tempValue2 = maxFreq;
	}

	if (tempValue2 < tempValue1)
	{
		tempValue2 = tempValue1;
	}

	sysData.display.freqLineStart = tempValue1;
	sysData.display.freqLineStop = tempValue2;

	return __SCPI_SUCCESS;
}

//设置频率线终止
int tSysScpi::setDisplayOfFreqLineStop(QString value)
{
	bool isOk = false;
	double tempValue = 0;

	tempValue = getFreqValue(value, &isOk);

	if (isOk)
	{
		return setDisplayOfFreqLineStop(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//方向键设置频率线终止
int tSysScpi::setDisplayOfFreqLineStopWithArrow(bool up)
{
	if (up)
	{
		return setDisplayOfFreqLineStop(sysData.display.freqLineStop + sysData.span.span * 0.1);
	} else
	{
		return setDisplayOfFreqLineStop(sysData.display.freqLineStop - sysData.span.span * 0.1);
	}
}

//脉冲旋钮设置频率线终止
int tSysScpi::setDisplayOfFreqLineStopWithRpg(int value)
{
	if (sysData.span.span <= 0)
	{
		return setDisplayOfFreqLineStop(sysData.display.freqLineStop + sysData.display.freqLineStop * value * 0.01);
	} else
	{
		return setDisplayOfFreqLineStop(sysData.display.freqLineStop + sysData.span.span * value * 0.01);
	}
}

//设置频率线
int tSysScpi::setDisplayOfFreqLine(int value)
{
	if (!sysData.display.isFreqLineOn && value == 1)
	{
		double minFreq = sysData.freq.start + sysData.freq.offset;
		double maxFreq = sysData.freq.stop + sysData.freq.offset;

		if (sysData.display.freqLineStart < minFreq)
		{
			sysData.display.freqLineStart = minFreq;
		} else if (sysData.display.freqLineStart > maxFreq)
		{
			sysData.display.freqLineStart = maxFreq;
		}

		if (sysData.display.freqLineStop < minFreq)
		{
			sysData.display.freqLineStop = minFreq;
		} else if (sysData.display.freqLineStop > maxFreq)
		{
			sysData.display.freqLineStop = maxFreq;
		}

		if (sysData.display.freqLineStart > sysData.display.freqLineStop)
		{
			sysData.display.freqLineStart = sysData.display.freqLineStop;
		}

		sysData.display.freqLineStart = minFreq + sysData.span.span * 0.4;
		sysData.display.freqLineStop = minFreq + sysData.span.span * 0.6;
	}

	sysData.display.isFreqLineOn = value == 1;

	return __SCPI_SUCCESS;
}

//设置频率线
int tSysScpi::setDisplayOfFreqLine(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	int tempValue = getBoolValue(value.toUpper().trimmed(), &isOk);

	if (isOk)
	{
		return setDisplayOfFreqLine(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//清除所有用户状态
int tSysScpi::clearAllUserSelfState(void)
{
	for (int i = 0; i < USERCOUNT; i++)
	{
		clearUserSelfState(i);
	}

	return __SCPI_SUCCESS;
}

//清除用户状态
int tSysScpi::clearUserSelfState(int index)
{
	if (index < 0 || index >= USERCOUNT)
	{
		return __SCPI_FAILED;
	}

	char tempChar[32] = { };
	QString fileName = QCoreApplication::applicationDirPath() + "/userparam" + QString(intToString(index, tempChar)).trimmed();

	if (QFile(fileName).exists())
	{
		QFile(fileName).remove();
		sysData.userSelfState[index].saved = false;
		sysData.userSelfState[index].centerFreq = 0;

		return __SCPI_SUCCESS;
	} else
	{
		return __SCPI_FAILED;
	}
}

//加载用户状态
int tSysScpi::loadUserSelfState(int index)
{
	if (index < 0 || index >= USERCOUNT)
	{
		return __SCPI_FAILED;
	}

	char tempChar[32] = { };
	QString fileName = QCoreApplication::applicationDirPath() + "/userparam" + QString(intToString(index, tempChar)).trimmed();

	if (QFile(fileName).exists())
	{
		struct userSelfStateDataDef tempState[USERCOUNT];
		memcpy(tempState, sysData.userSelfState, sizeof tempState);

		FILE *fp;
		fp = fopen(fileName.toStdString().c_str(), "rb");

		if (fp != NULL)
		{
			fread(&sysData, sizeof sysData, 1, fp);
			fclose(fp);
			controlRf();
		}

		memcpy(sysData.userSelfState, tempState, sizeof tempState);
		return __SCPI_SUCCESS;
	} else
	{
		return __SCPI_FAILED;
	}
}

//保存用户状态
int tSysScpi::saveUserSelfState(int index)
{
	if (index < 0 || index >= USERCOUNT)
	{
		return __SCPI_FAILED;
	}

	char tempChar[32] = { };
	QString fileName = QCoreApplication::applicationDirPath() + "/userparam" + QString(intToString(index, tempChar)).trimmed();

	FILE *fp;
	fp = fopen(fileName.toStdString().c_str(), "wb");

	if (fp != NULL)
	{
		fwrite(&sysData, sizeof sysData, 1, fp);
		fclose(fp);
		sysData.userSelfState[index].saved = true;
		sysData.userSelfState[index].centerFreq = sysData.freq.center;
	}

	return __SCPI_SUCCESS;
}

//设置扫描时间自动
int tSysScpi::setSweepOfTimeAuto(int value)
{
	sysData.sweep.sweepTimeAuto = value == 1;

	if (sysData.sweep.sweepTimeAuto)
	{
		controlRf();
	}

	return __SCPI_SUCCESS;
}

//设置扫描时间自动
int tSysScpi::setSweepOfTimeAuto(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	int tempValue = getBoolValue(value.toUpper().trimmed(), &isOk);

	if (isOk)
	{
		return setSweepOfTimeAuto(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置扫描时间
int tSysScpi::setSweepOfTime(double value)
{
	double tempValue = value * 1e-3; // to us
	double delSweepTime, delHoldTime;

	calSweepTimeOfAuto();
	
	delSweepTime = tempValue - sysData.sweep.sweepTime;
	delHoldTime = delSweepTime / sysData.fscan.fftFrame;

	if(sysData.fscan.mode == fsFFT)
	{
		sysData.fscan.ffHoldTime += delHoldTime;
		sysData.fscan.ofHoldTime += delHoldTime;
		if(sysData.fscan.ofHoldTime <= 100)
			sysData.fscan.ofHoldTime = 100;
	} else
	{
		sysData.fscan.ofHoldTime +=  delHoldTime;
		if (sysData.fscan.ofHoldTime <= 0.1)
			sysData.fscan.ofHoldTime = 0.1;
	}
	if (sysData.sweep.sweepTimeAuto == true)
		sysData.sweep.sweepTimeAuto = false;
	if (tempValue <= 1e3)//make sysData.sweep.sweepTime min value 1ms
		tempValue = 1e3;

	sysData.sweep.sweepTime = tempValue;//us
	controlRf();
	return __SCPI_SUCCESS;
}

//设置扫描时间
int tSysScpi::setSweepOfTime(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	double tempValue = getTimeValue(value, &isOk);

	if (isOk)
	{
		return setSweepOfTime(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//方向键设置扫描时间
int tSysScpi::setSweepOfTimeWithArrow(bool up)
{
	if (up)
	{
		return setSweepOfTime(zoomInData(sysData.sweep.sweepTime, 2));
	} else
	{
		return setSweepOfTime(zoomOutData(sysData.sweep.sweepTime, 2));
	}
}

//脉冲旋钮设置扫描时间
int tSysScpi::setSweepOfTimeWithRpg(int value)
{
	return setSweepOfTime(sysData.sweep.sweepTime + sysData.sweep.sweepTime * 0.01 * value);
}

//设置扫描方式:单词扫描和连续扫描
int tSysScpi::setSweepOfMode(int value)
{
	sysData.sweep.sweepSingle = value == 1;
	bool val = !sysData.sweep.sweepSingle;
	feDownload(230, val);
	return __SCPI_SUCCESS;
}

//设置扫描方式
int tSysScpi::setSweepOfMode(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	int tempValue = getBoolValue(value.toUpper().trimmed(), &isOk);

	if (isOk)
	{
		return setSweepOfMode(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置阻抗变换值
int tSysScpi::setLMP_CValue(double value)
{
	sysData.ampt.LMPValue = value;
	saveImpedanceValue();

	return __SCPI_SUCCESS;
}

//方向键设置阻抗变换值
int tSysScpi::setAmptOfLMP_CWithArrow(bool up)
{
	double tempValue = sysData.ampt.LMPValue;

	if (up)
	{
		tempValue += 10;
	} else
	{
		tempValue -= 10;
	}

	return setLMP_CValue(tempValue);
}

//脉冲旋钮设置阻抗变换值
int tSysScpi::setAmptOfLMP_CWithRpg(int value)
{
	double tempValue = value;

	return setLMP_CValue(sysData.ampt.LMPValue + value);
}

//设置扫描点数
int tSysScpi::setSweepOfPoints(double value)
{
	int tempValue = (int) value;

	if (tempValue < MINSWEEPPOINTS)
	{
		tempValue = MINSWEEPPOINTS;
	} else if (tempValue > MAXSWEEPPOINTS)
	{
		tempValue = MAXSWEEPPOINTS;
	}

	sweepPrevPoints = sysData.sweep.sweepPoints;

	if (tempValue > sweepPrevPoints)
	{
		isFillData = true;
	}

	sysData.sweep.sweepPoints = tempValue;
	controlRf();

	for (int i = 0; i < MARKERCOUNT; i++)
	{
		if (sysData.marker[i].state == msNormal)
		{
			if (sysData.marker[i].markerNormal.xIndex >= sysData.sweep.sweepPoints)
			{
				sysData.marker[i].markerNormal.xIndex = sysData.sweep.sweepPoints - 1;
			}
		} else if (sysData.marker[i].state == msDelta)
		{
			if (sysData.marker[i].markerDelta.x1Index >= sysData.sweep.sweepPoints)
			{
				sysData.marker[i].markerDelta.x1Index >= sysData.sweep.sweepPoints - 1;
			}

			if (sysData.marker[i].markerDelta.x2Index >= sysData.sweep.sweepPoints)
			{
				sysData.marker[i].markerDelta.x2Index >= sysData.sweep.sweepPoints - 1;
			}
		}
	}

	return __SCPI_SUCCESS;
}

//设置扫描点数
int tSysScpi::setSweepOfPoints(QString value)
{
	bool isOk = false;
	double tempValue = getDoubleValue(value, &isOk);

	if (isOk)
	{
		return setSweepOfPoints(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//方向键设置扫描点数
int tSysScpi::setSweepOfPointsWithArrow(bool up)
{
	int tempValue = sysData.sweep.sweepPoints;

	int modValue = 0;

	if (up)
	{
		modValue = ((tempValue / 100) + 1) * 100 + 1;
	} else
	{
		modValue = ((tempValue / 100) - 1) * 100 + 1;
	}

	return setSweepOfPoints(modValue);
}

//脉冲旋钮设置扫描点数
int tSysScpi::setSweepOfPointsWithRpg(int value)
{
	return setSweepOfPoints(sysData.sweep.sweepPoints + value);
}

//自动调谐
int tSysScpi::setAutoTune(void)
{
	if (!sysData.isAutoSearching)
	{
		int peakIndex = 0;

		if (sysData.freq.isLowChannel)
		{
			presetSystemData();
			sysData.freq.isLowChannel = true;
			setFrequencyOfLowChannel(sysData.freq.isLowChannel);

			setMarkerNormal(0, 0);
			sysData.detector.detMode = dmPosPeak;
			controlRf();
			sysData.isAutoSearching = true;

			//BD1 0dBm
			setAutoTuneFreq(ZCMINFREQ, 100e3);
			peakIndex = getAutoTunePeakIndex();

			if (peakIndex >= 0)
			{
				sysData.marker[0].markerNormal.xIndex = peakIndex;
				setAutoTunePeakToCenter();

				if (setAutoTuneRefLevel() == __SCPI_SUCCESS)
				{
					sysData.isAutoSearching = false;
					return __SCPI_SUCCESS;
				}
			}

			
			setAutoTuneFreq(100e3, ZCMAXFREQ);
			peakIndex = getAutoTunePeakIndex();

			if (peakIndex >= 0)
			{
				sysData.marker[0].markerNormal.xIndex = peakIndex;
				setAutoTunePeakToCenter();

				if (setAutoTuneRefLevel() == __SCPI_SUCCESS)
				{
					sysData.isAutoSearching = false;
					return __SCPI_SUCCESS;
				}
			}

			//Ref -20dBm
			setAmptOfRefLevel(-20);
			setAmptOfAtt(0);
			controlRf();

			//BD1 -20dBm
			setAutoTuneFreq(ZCMINFREQ, 100e3);
			peakIndex = getAutoTunePeakIndex();

			if (peakIndex >= 0)
			{
				sysData.marker[0].markerNormal.xIndex = peakIndex;
				setAutoTunePeakToCenter();

				if (setAutoTuneRefLevel() == __SCPI_SUCCESS)
				{
					sysData.isAutoSearching = false;
					return __SCPI_SUCCESS;
				}
			}


			setAutoTuneFreq(100e3, ZCMAXFREQ);
			peakIndex = getAutoTunePeakIndex();

			if (peakIndex >= 0)
			{
				sysData.marker[0].markerNormal.xIndex = peakIndex;
				setAutoTunePeakToCenter();

				if (setAutoTuneRefLevel() == __SCPI_SUCCESS)
				{
					sysData.isAutoSearching = false;
					return __SCPI_SUCCESS;
				}
			}

			setAmptOfRefLevel(-30);
			setAmptOfAtt(0);
			setPreamptOfAttAuto(1);

			//BD1 -30dBm
			setAutoTuneFreq(ZCMINFREQ, 100e3);
			setBwOfRbwAuto(1);
			setBwOfRbwWithArrow(false);
			peakIndex = getAutoTunePeakIndex();

			if (peakIndex >= 0)
			{
				sysData.marker[0].markerNormal.xIndex = peakIndex;
				setAutoTunePeakToCenter();

				if (setAutoTuneRefLevel() == __SCPI_SUCCESS)
				{
					sysData.isAutoSearching = false;
					return __SCPI_SUCCESS;
				}
			}


			setAutoTuneFreq(100e3, ZCMAXFREQ);
			setBwOfRbwAuto(1);
			setBwOfRbwWithArrow(false);
			peakIndex = getAutoTunePeakIndex();

			if (peakIndex >= 0)
			{
				sysData.marker[0].markerNormal.xIndex = peakIndex;
				setAutoTunePeakToCenter();

				if (setAutoTuneRefLevel() == __SCPI_SUCCESS)
				{
					sysData.isAutoSearching = false;
					return __SCPI_SUCCESS;
				}
			}

		} else
		{
			//Ref 0dBm
			presetSystemData();
			setMarkerNormal(0, 0);
			sysData.detector.detMode = dmPosPeak;
			controlRf();
			sysData.isAutoSearching = true;

			//BD1 0dBm
			setAutoTuneFreq(TUNEBD1START, TUNEBD1STOP);
			peakIndex = getAutoTunePeakIndex();

			if (peakIndex >= 0)
			{
				sysData.marker[0].markerNormal.xIndex = peakIndex;
				setAutoTunePeakToCenter();

				if (setAutoTuneRefLevel() == __SCPI_SUCCESS)
				{
					sysData.isAutoSearching = false;
					return __SCPI_SUCCESS;
				}
			}

			//BD2 0dBm
			setAutoTuneFreq(TUNEBD2START, TUNEBD2STOP);
			peakIndex = getAutoTunePeakIndex();

			if (peakIndex >= 0)
			{
				sysData.marker[0].markerNormal.xIndex = peakIndex;
				setAutoTunePeakToCenter();

				if (setAutoTuneRefLevel() == __SCPI_SUCCESS)
				{
					sysData.isAutoSearching = false;
					return __SCPI_SUCCESS;
				}
			}

			//Ref -20dBm
			setAmptOfRefLevel(-20);
			setAmptOfAtt(0);
			controlRf();

			//BD1 -20dBm
			setAutoTuneFreq(TUNEBD1START, TUNEBD1STOP);
			peakIndex = getAutoTunePeakIndex();

			if (peakIndex >= 0)
			{
				sysData.marker[0].markerNormal.xIndex = peakIndex;
				setAutoTunePeakToCenter();

				if (setAutoTuneRefLevel() == __SCPI_SUCCESS)
				{
					sysData.isAutoSearching = false;
					return __SCPI_SUCCESS;
				}
			}

			//BD2 -20dBm
			setAutoTuneFreq(TUNEBD2START, TUNEBD2STOP);
			peakIndex = getAutoTunePeakIndex();

			if (peakIndex >= 0)
			{
				sysData.marker[0].markerNormal.xIndex = peakIndex;
				setAutoTunePeakToCenter();

				if (setAutoTuneRefLevel() == __SCPI_SUCCESS)
				{
					sysData.isAutoSearching = false;
					return __SCPI_SUCCESS;
				}
			}

			//-----add -80dBm
			//Ref -30dBm
			setAmptOfRefLevel(-30);
			setAmptOfAtt(0);
			setPreamptOfAttAuto(1);

			//BD1 -30dBm
			setAutoTuneFreq(TUNEBD1START, TUNEBD1STOP);
			setBwOfRbwAuto(1);
			setBwOfRbwWithArrow(false);
			peakIndex = getAutoTunePeakIndex();

			if (peakIndex >= 0)
			{
				sysData.marker[0].markerNormal.xIndex = peakIndex;
				setAutoTunePeakToCenter();

				if (setAutoTuneRefLevel() == __SCPI_SUCCESS)
				{
					sysData.isAutoSearching = false;
					return __SCPI_SUCCESS;
				}
			}

			//BD2 -30dBm
			setAutoTuneFreq(TUNEBD2START, TUNEBD2STOP);
			setBwOfRbwAuto(1);
			setBwOfRbwWithArrow(false);
			setBwOfRbwWithArrow(false);
			peakIndex = getAutoTunePeakIndex();

			if (peakIndex >= 0)
			{
				sysData.marker[0].markerNormal.xIndex = peakIndex;
				setAutoTunePeakToCenter();

				if (setAutoTuneRefLevel() == __SCPI_SUCCESS)
				{
					sysData.isAutoSearching = false;
					return __SCPI_SUCCESS;
				}
			}
			//-----
		}
		sysData.isAutoSearching = false;
		return __SCPI_FAILED;
	} else
	{
		sysData.isAutoSearching = false;
		return __SCPI_IGNORE;
	}
}

//设置触发模式
int tSysScpi::setTrigMode(int value)
{
	int temp = value;

	if (temp < 0)
	{
		temp = 0;
	} else if (temp > 3)
	{
		temp = 3;
	}

	switch (temp)
	{
		case tgRun:
			sysData.trig.trigDown = 0x00;
			sysData.sweep.sweepSingle = false;
			sysData.trig.isDisplayVideoLine = false;
			break;
		case tgVideo:
			sysData.trig.trigDown = 0x01;
			sysData.sweep.sweepSingle = true;
			sysData.trig.isDisplayVideoLine = true;
			break;
		case tgRise:
			sysData.trig.trigDown = 0x03;
			sysData.sweep.sweepSingle = true;
			sysData.trig.isDisplayVideoLine = false;
			break;
		case tgFall:
			sysData.trig.trigDown = 0x02;
			sysData.sweep.sweepSingle = true;
			sysData.trig.isDisplayVideoLine = false;
			break;
	}

	sysData.trig.mode = trigMode(temp);

	//set trig mode 
	feDownload(46, sysData.trig.mode&0x03);
	
	controlRf();
	return __SCPI_SUCCESS;
}

//设置触发模式
int tSysScpi::setTrigMode(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	QString tempValue = value.toUpper().trimmed();

	if (tempValue == "RUN")
	{
		setTrigMode(tgRun);
	} else if (tempValue == "VID" || tempValue == "VIDEO")
	{
		setTrigMode(tgVideo);
	} else if (tempValue == "POS" || tempValue == "POSITIVE")
	{
		setTrigMode(tgRise);
	} else if (tempValue == "NEG" || tempValue == "NEGATIVE")
	{
		setTrigMode(tgFall);
	} else
	{
		return __SCPI_FAILED;
	}

	return __SCPI_SUCCESS;
}

//计算触发功率下发值
int tSysScpi::calculateVideoTrigPowerDown(double value)
{
	double tempValue = value;
	double temp = 0;
	double bcValue = 0;
	double tempValueOfDown = 0;

	if (sysData.freq.isLowChannel)
	{
		if (sysData.ampt.isPreamptOn)
		{
			//bc att
			bcValue = getErrorOfAttenuationofLowfreqofPre();

			//bc if
			bcValue += getErrorOfIFofLowfreqofPre();

			//bc rbw
			bcValue += getErrorOfRbwofLowfreqofPre();

			//bc emc rbw
			if (sysData.emc.emcOn)
			{
				bcValue -= getErrorOfEmcRbw();
			}

			//bc temperature
			bcValue += getErrorOfTemperature();

		} else
		{
			//bc att
			bcValue = getErrorOfAttenuationofLowfreq();

			//bc if
			bcValue += getErrorOfIFofLowfreq();

			//bc rbw
			bcValue += getErrorOfRbwofLowfreq();

			//bc emc rbw
			if (sysData.emc.emcOn)
			{
				bcValue -= getErrorOfEmcRbw();
			}

			//bc temperature
			bcValue += getErrorOfTemperature();

		}
	} else
	{
		if (sysData.ampt.isPreamptOn)
		{
			//bc att
			bcValue = getErrorOfAttenuationofPre();

			//bc if
			bcValue += getErrorOfIFofPre();

			//bc rbw
			bcValue += getErrorOfRbwofPre();

			//bc emc rbw
			if (sysData.emc.emcOn)
			{
				bcValue -= getErrorOfEmcRbw();
			}

			//bc temperature
			bcValue += getErrorOfTemperature();
		} else
		{
			//bc att
			bcValue = getErrorOfAttenuation();

			//bc if
			bcValue += getErrorOfIF();

			//bc rbw
			bcValue += getErrorOfRbw();

			//bc emc rbw
			if (sysData.emc.emcOn)
			{
				bcValue += getErrorOfEmcRbw();
			}

			//bc temperature
			bcValue += getErrorOfTemperature();
		}
	}

	if (sysData.freq.isLowChannel)
	{
		if (sysData.ampt.isPreamptOn)
		{
			temp = tempValue - (-40.0 - sysData.zcCalData.absoluteAmptValue + sysData.userCalData.absError + sysData.ampt.ifSoftValue + sysData.ampt.att - sysData.ampt.ifValue + sysData.ampt.refOffset + bcValue);
		} else
		{
			temp = tempValue - (-20.0 - sysData.zcCalData.absoluteAmptValue + sysData.userCalData.absError + sysData.ampt.ifSoftValue + sysData.ampt.att - sysData.ampt.ifValue + sysData.ampt.refOffset + bcValue);
		}
	} else
	{
		if (sysData.ampt.isPreamptOn)
		{
			temp = 40.0 + tempValue + sysData.preamplifierCalData.absoluteAmptValue - sysData.userCalData.absError - sysData.ampt.ifSoftValue - sysData.ampt.att + sysData.ampt.ifValue - sysData.ampt.refOffset - bcValue;
		} else
		{
			temp = tempValue - (-20.0 - sysData.factoryCalData.absoluteAmptValue + sysData.userCalData.absError + sysData.ampt.ifSoftValue + sysData.ampt.att - sysData.ampt.ifValue + sysData.ampt.refOffset + bcValue);
		}
	}

	if (sysData.ampt.isPreamptOn)
	{
		temp -= getErrorOfFreqRespofPre(250);
	} else
	{
		temp -= getErrorOfFreqResp(250);
	}

	tempValueOfDown = qPow(10, temp / 20);
	sysData.trig.powerDown = (unsigned long) tempValueOfDown;
	sysData.trig.powerDownLL = (sysData.trig.powerDown >> 8) & 0xff;
	sysData.trig.powerDownLH = (sysData.trig.powerDown >> 16) & 0xff;
	sysData.trig.powerDownH = (sysData.trig.powerDown >> 24) & 0xff;

	//Video trig power
	if (sysData.trig.mode == tgVideo && sysData.span.span == 0)
	{
		//RF_ADS(20) 视频触发功率
		rfDownload(0x50, sysData.trig.powerDownH);
		rfDownload(0x50, sysData.trig.powerDownLH);
		rfDownload(0x50, sysData.trig.powerDownLL);
	}
	return __SCPI_SUCCESS;
}

//设置触发功率
int tSysScpi::setVideoTrigPower(double value)
{
	getAmptMaxMinLimit();

	double tempValue = value;

	if (tempValue < sysData.ampt.refLevelMin)
	{
		tempValue = sysData.ampt.refLevelMin;
	} else if (tempValue > sysData.ampt.refLevelMax)
	{
		tempValue = sysData.ampt.refLevelMax;
	}

	sysData.trig.power = tempValue;
	double trigPower = getdBmValue(sysData.trig.power);

	//set video trig level 	
	//feDownload(47, (sysData.trig.power << 16)&0xff);
	//feDownload(48, sysData.trig.power * 0xffff);

	calculateVideoTrigPowerDown(trigPower);
	return __SCPI_SUCCESS;
}

//设置触发功率
int tSysScpi::setVideoTrigPower(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	double tempValue = getAmptValue(value.toUpper().trimmed(), &isOk);

	if (isOk)
	{
		return setVideoTrigPower(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}

	return __SCPI_SUCCESS;
}

//方向键设置触发功率
int tSysScpi::setVideoTrigPowerWithArrow(bool up)
{
	if (up)
	{
		return setVideoTrigPower(sysData.trig.power + 10.0);
	} else
	{
		return setVideoTrigPower(sysData.trig.power - 10.0);
	}
}

//脉冲旋钮设置触发功率
int tSysScpi::setVideoTrigPowerWithRpg(int value)
{
	return setVideoTrigPower(sysData.trig.power + value * 0.1);
}

//设置迹线模式
int tSysScpi::setTraceMode(int traceIndex, int value)
{
	if (traceIndex < 0 || traceIndex >= TRACECOUNT)
	{
		return __SCPI_FAILED;
	}

	switch (value)
	{
		case tsClearWrite:
			sysData.trace[traceIndex].flags = 0;
			sysData.trace[traceIndex].state = tsClearWrite;
			break;
		case tsMaxHold:
			//if (sysData.detector.detMode != dmPosPeak)
			//{
			//  setDetector(dmPosPeak);
			//}

			memcpy(sysData.trace[traceIndex].value, sysData.prjValue, sizeof(double) * sysData.sweep.sweepPoints);
			sysData.trace[traceIndex].flags = 0;
			sysData.trace[traceIndex].state = tsMaxHold;
			break;
		case tsMinHold:
			//if (sysData.detector.detMode != dmNegPeak)
			//{
			//  setDetector(dmNegPeak);
			//}

			memcpy(sysData.trace[traceIndex].value, sysData.prjValue, sizeof(double) * sysData.sweep.sweepPoints);
			sysData.trace[traceIndex].flags = 0;
			sysData.trace[traceIndex].state = tsMinHold;
			break;
		case tsView:
			sysData.trace[traceIndex].flags = 0;
			sysData.trace[traceIndex].state = tsView;
			break;
		case tsBlank:
			sysData.trace[traceIndex].flags = 0;
			sysData.trace[traceIndex].state = tsBlank;
			break;
		default:
			return __SCPI_FAILED;
	}

	return __SCPI_SUCCESS;
}

//设置迹线模式
int tSysScpi::setTraceMode(int traceIndex, QString value)
{
	if (traceIndex < 0 || traceIndex >= TRACECOUNT || value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	QString tempValue = value.toUpper().trimmed();

	if (tempValue == "WRIT" || tempValue == "WRITE")
	{
		return setTraceMode(traceIndex, tsClearWrite);
	} else if (tempValue == "MAXH" || tempValue == "MAXHOLD")
	{
		return setTraceMode(traceIndex, tsMaxHold);
	} else if (tempValue == "MINH" || tempValue == "MINHOLD")
	{
		return setTraceMode(traceIndex, tsMinHold);
	} else if (tempValue == "VIEW")
	{
		return setTraceMode(traceIndex, tsView);
	} else if (tempValue == "BLAN" || tempValue == "BLANK")
	{
		return setTraceMode(traceIndex, tsBlank);
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置迹线互换
int tSysScpi::setTraceOperationsOfExchange(int trace1Index, int trace2Index)
{
	if (trace1Index < 0 || trace1Index >= TRACECOUNT || trace2Index < 0 || trace2Index >= TRACECOUNT)
	{
		return __SCPI_FAILED;
	}

	sysData.trace[trace1Index].state = tsView;
	sysData.trace[trace2Index].state = tsView;

	double tempValue[sysData.sweep.sweepPoints];

	memcpy(tempValue, sysData.trace[trace1Index].value, sizeof tempValue);
	memcpy(sysData.trace[trace1Index].value, sysData.trace[trace2Index].value, sizeof tempValue);
	memcpy(sysData.trace[trace2Index].value, tempValue, sizeof tempValue);

	return __SCPI_SUCCESS;
}

//设置 迹线转移
int tSysScpi::setTraceOperationsOfChange(int trace1Index, int trace2Index)
{
	if (trace1Index < 0 || trace1Index >= TRACECOUNT || trace2Index < 0 || trace2Index >= TRACECOUNT)
	{
		return __SCPI_FAILED;
	}

	sysData.trace[trace2Index].state = tsView;
	memcpy(sysData.trace[trace2Index].value, sysData.trace[trace1Index].value, sizeof(double) * sysData.sweep.sweepPoints);

	return __SCPI_SUCCESS;
}

//设置 迹线显示线
int tSysScpi::setTraceOperationsOfDisplayLine(int traceIndex)
{
	if (traceIndex < 0 || traceIndex >= TRACECOUNT)
	{
		return __SCPI_FAILED;
	}

	sysData.trace[traceIndex].state = tsView;

	for (int i = 0; i < sysData.sweep.sweepPoints; i++)
	{
		sysData.trace[traceIndex].value[i] -= sysData.display.dispLineValue;
	}

	if (!sysData.display.isDispLineOn)
	{
		sysData.display.isDispLineOn = true;
	}

	return __SCPI_SUCCESS;
}

//设置检波
int tSysScpi::setDetector(int value)
{
	sysData.detector.detMode = detectorMode(value);
	controlIfDetector();
	resetBwOfAverageState();

	return __SCPI_SUCCESS;
}

//设置检波
int tSysScpi::setDetector(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	QString tempValue = value.toUpper().trimmed();

	if (tempValue == "AUTO")
	{
		return setDetector(dmAuto);
	} else if (tempValue == "NORM" || tempValue == "NORMAL")
	{
		return setDetector(dmNormal);
	} else if (tempValue == "POS" || tempValue == "POSITIVE")
	{
		return setDetector(dmPosPeak);
	} else if (tempValue == "SAMP" || tempValue == "SAMPLE")
	{
		return setDetector(dmSample);
	} else if (tempValue == "NEG" || tempValue == "NEGATIVE")
	{
		return setDetector(dmNegPeak);
	} else if (tempValue == "QP" || tempValue == "QUASIPEAK")
	{
		return setDetector(dmQuasiPeak);
	} else if (tempValue == "AVER" || tempValue == "AVERAGE")
	{
		return setDetector(dmAverage);
	} else if (tempValue == "TRANSIENT" || tempValue == "TRAN")
	{
		return setDetector(dmTransient);
	} else if (tempValue == "RMS")
	{
		return setDetector(dmRMS);
	} else
	{
		return __SCPI_FAILED;
	}
}

//测量已打开
int tSysScpi::isMeasureOn(void)
{
	if ((sysData.measure.isAcprOn || sysData.measure.isChannelPowerOn || sysData.measure.isObwOn) && (sysData.span.span > 0))
	{
		return __SCPI_SUCCESS;
	}

	return __SCPI_FAILED;
}

//所有测量已关闭
int tSysScpi::isMeasureAllOff(void)
{
	if (sysData.measure.isAcprOn || sysData.measure.isChannelPowerOn || sysData.measure.isObwOn || sysData.disc.isDiscOn)
	{
		return __SCPI_FAILED;
	}

	return __SCPI_SUCCESS;
}

//关闭所有测量
int tSysScpi::setMeasureAllOff(void)
{
	sysData.measure.isTimeSpectrumOn = false;
	sysData.measure.isAcprOn = false;
	sysData.measure.isChannelPowerOn = false;
	sysData.measure.isObwOn = false;
	setMeasureOfDiscState(0);
	sysData.canvasNums = 1;
	sysData.mode = smSpectrum;

	//复位测量参数
	sysData.measureSetup.channelWidth = 1e6;
	sysData.measureSetup.channelSpace = 2e6;
	sysData.measureSetup.channelNums = 3;
	sysData.measureSetup.powerPercent = 99;

	return __SCPI_SUCCESS;
}

//设置时间频谱测量
int tSysScpi::setMeasureOfTimeSpectrumOn(int value)
{
	setMeasureAllOff();
	sysData.measure.isTimeSpectrumOn = value == 1;
	sysData.measure.timeSpecFlag = 0;

	if (sysData.measure.isTimeSpectrumOn)
	{
		checkMultiDisplay(mdtTimeSpec);
		sysData.canvasNums = 2;
		sysData.mode = smTimeSpec;
	} else
	{
		sysData.canvasNums = 1;
		sysData.mode = smSpectrum;
	}

	return __SCPI_SUCCESS;
}

//设置邻道功率测量
int tSysScpi::setMeasureOfAcprOn(int value)
{
	setMeasureAllOff();
	sysData.measure.isAcprOn = value == 1;

	if (sysData.measure.isAcprOn)
	{
		checkMultiDisplay(mdtAcpr);

		if (sysData.measureSetup.channelWidth <= 0)
		{
			sysData.measureSetup.channelWidth = 1e6;
		}

		if (sysData.measureSetup.channelSpace <= 0)
		{
			sysData.measureSetup.channelSpace = 2e6;
		}

		setFrequencyOfSpan((sysData.measureSetup.channelNums * 2 + 1) * sysData.measureSetup.channelSpace);

		sysData.bw.rbwAuto = false;
		sysData.bw.rbwStepCont = true;
		setBwOfRbw(0.02 * sysData.measureSetup.channelWidth);
		sysData.detector.detMode = dmSample;
		controlRf();

		sysData.canvasNums = 2;
		sysData.mode = smAcpr;
	} else
	{
		sysData.canvasNums = 1;
		sysData.mode = smSpectrum;
	}

	return __SCPI_SUCCESS;
}
//设置信道功率测量
int tSysScpi::setMeasureOfChannelPowerOn(int value)
{
	setMeasureAllOff();
	sysData.measure.isChannelPowerOn = value == 1;

	if (sysData.measure.isChannelPowerOn)
	{
		checkMultiDisplay(mdtChannelPower);

		if (sysData.measureSetup.channelWidth <= 0)
		{
			sysData.measureSetup.channelWidth = 1e6;
		}

		setFrequencyOfSpan(3 * sysData.measureSetup.channelWidth);
		sysData.bw.rbwAuto = false;
		sysData.bw.rbwStepCont = true;
		setBwOfRbw(0.02 * sysData.measureSetup.channelWidth);
		sysData.detector.detMode = dmSample;
		controlRf();
		sysData.canvasNums = 1;
		sysData.mode = smSpectrum;
	}

	return __SCPI_SUCCESS;
}

//设置占用带宽测量
int tSysScpi::setMeasureOfObwOn(int value)
{
	setMeasureAllOff();
	sysData.measure.isObwOn = value == 1;

	if (sysData.measure.isObwOn)
	{
		checkMultiDisplay(mdtObw);
		setFrequencyOfSpan(3 * sysData.measureSetup.channelWidth);

		sysData.bw.rbwAuto = false;
		sysData.bw.rbwStepCont = true;
		setBwOfRbw(0.02 * sysData.measureSetup.channelWidth);
		sysData.detector.detMode = dmSample;
		controlRf();

		sysData.canvasNums = 2;
		sysData.mode = smObw;
	} else
	{
		sysData.canvasNums = 1;
		sysData.mode = smSpectrum;
	}

	return __SCPI_SUCCESS;
}

//设置鉴频状态
int tSysScpi::setMeasureOfDiscState(int value)
{
	sysData.disc.isDiscOn = value == 1;

	mutexDisc.lock();
	if (sysData.disc.isDiscOn)
	{
		//multidisplay
		checkMultiDisplay(mdtDisc);

		//try connect
		if (!connectToDiscriminator())
		{
			sysData.disc.isDiscOn = false;
			mutexDisc.unlock();
			return __SCPI_FAILED;
		}

		//reset sweep points
		if (sysData.sweep.sweepPoints != 601)
		{
			setSweepOfPoints(601);
		}

		//reset discriminator
		unsigned char buf[64] = { };
		int rcvLen = 0;

		switch (sysData.disc.inputGain)
		{
			case igtx1:
				setDiscriminatorInputGain(1 * 100);
				break;
			case igtx2:
				setDiscriminatorInputGain(2 * 100);
				break;
			case igtx4:
				setDiscriminatorInputGain(4 * 100);
				break;
			case igtx05:
				setDiscriminatorInputGain(0.5 * 100);
				break;
			case igtx025:
				setDiscriminatorInputGain(0.25 * 100);
				break;
		}

		setDiscriminationDataLen(19);
		usbDiscriminatorRecv(buf, 64, &rcvLen);

		//track gen
		sysData.source.trackGenOutputPower = sysData.disc.outputPower;
		sysData.source.isTrackGenNetworkOn = false;
		setSourceTrackGenState(true);

		sysData.canvasNums = 1;
		sysData.mode = smDiscriminator;

		loadDiscConfig();

		memset(sysData.disc.discValue, 0, sizeof sysData.disc.discValue);
		memset(sysData.disc.discValueInit, 0, sizeof sysData.disc.discValueInit);

		for (int i = 0; i < TRACECOUNT; i++)
		{
			memset(sysData.trace[i].value, 0, sizeof sysData.trace[i].value);
		}
	} else
	{
		disconectFromUSBDevice();

		sysData.sweep.sweepSingle = false;
		controlRf();
		setSourceTrackGenState(false);
		sysData.canvasNums = 1;
		sysData.mode = smSpectrum;
	}

	mutexDisc.unlock();

	return __SCPI_SUCCESS;
}

//设置鉴频输入增益
int tSysScpi::setMeasureOfDiscInputGain(int value)
{
	if (value < 0 || value > 4)
	{
		return __SCPI_FAILED;
	}

	sysData.disc.inputGain = inputGainType(value);

	unsigned char data[64] = { };
	int level = 0;

	switch (sysData.disc.inputGain)
	{
		case igtx2:
			level = 200;
			break;
		case igtx4:
			level = 400;
			break;
		case igtx05:
			level = 50;
			break;
		case igtx025:
			level = 25;
			break;
		default:
			level = 100;
			break;
	}

	data[0] = 0x50;
	data[1] = ~data[0];
	data[2] = ((level >> 8) & 0xff);
	data[3] = (level & 0xff);

	usbDiscriminatorSend(data, sizeof data / sizeof data[0]);

	return __SCPI_SUCCESS;
}

//设置鉴频输出功率
int tSysScpi::setMeasureOfDiscOutputPower(double value)
{
	double tempValue = value;

	if (tempValue < MINTRACKGENPOWER)
	{
		tempValue = MINTRACKGENPOWER;
	} else if (tempValue > MAXTRACKGENPOWER)
	{
		tempValue = MAXTRACKGENPOWER;
	}

	int intValue = (int) tempValue;

	sysData.disc.outputPower = intValue + (qAbs(intValue) % 2);
	sysData.source.trackGenOutputPower = intValue + (qAbs(intValue) % 2);
	setSourceTrackGenOutputPower(sysData.source.trackGenOutputPower);

	return __SCPI_SUCCESS;
}

//方向键设置鉴频输出功率
int tSysScpi::setMeasureOfDiscOutputPowerWithArrow(bool up)
{
	if (up)
	{
		return setMeasureOfDiscOutputPower(sysData.disc.outputPower + 10.0);
	} else
	{
		return setMeasureOfDiscOutputPower(sysData.disc.outputPower - 10.0);
	}
}

//脉冲旋钮设置鉴频输出功率
int tSysScpi::setMeasureOfDiscOutputPowerWithRpg(int value)
{
	return setMeasureOfDiscOutputPower(sysData.disc.outputPower + 2 * value);
}

//设置鉴频刻度
int tSysScpi::setMeasureOfDiscScaleDiv(double value)
{
	double tempValue = value;

	if (tempValue < MINDISCSCALEDIV)
	{
		tempValue = MINDISCSCALEDIV;
	} else if (tempValue > MAXDISCSCALEDIV)
	{
		tempValue = MAXDISCSCALEDIV;
	}

	sysData.disc.scaleDiv = tempValue;

	return __SCPI_SUCCESS;
}

//方向键设置鉴频刻度
int tSysScpi::setMeasureOfDiscScaleDivWithArrow(bool up)
{
	if (up)
	{
		return setMeasureOfDiscScaleDiv(sysData.disc.scaleDiv + 0.1);
	} else
	{
		return setMeasureOfDiscScaleDiv(sysData.disc.scaleDiv - 0.1);
	}
}

//脉冲旋钮设置鉴频刻度
int tSysScpi::setMeasureOfDiscScaleDivWithRpg(int value)
{
	return setMeasureOfDiscScaleDiv(sysData.disc.scaleDiv + 0.01 * value);
}

//设置鉴频扫描时间
int tSysScpi::setMeasureOfDiscSweepTime(double value)
{
	double tempValue = value;

	if (tempValue < MINDISCTIME)
	{
		tempValue = MINDISCTIME;
	} else if (tempValue > MAXDISCTIME)
	{
		tempValue = MAXDISCTIME;
	}

	sysData.disc.sweepTime = tempValue;

	return __SCPI_SUCCESS;
}

//方向键设置鉴频扫描时间
int tSysScpi::setMeasureOfDiscSweepTimeWithArrow(bool up)
{
	if (up)
	{
		return setMeasureOfDiscSweepTime(zoomInData(sysData.disc.sweepTime, 2));
	} else
	{
		return setMeasureOfDiscSweepTime(zoomOutData(sysData.disc.sweepTime, 2));
	}
}

//脉冲旋钮设置鉴频扫描时间
int tSysScpi::setMeasureOfDiscSweepTimeWithRpg(int value)
{
	return setMeasureOfDiscSweepTime(sysData.disc.sweepTime + sysData.disc.sweepTime * 0.01 * value);
}

//设置鉴频直流偏置
int tSysScpi::setMeasureOfDiscDCOffset(double value)
{
	double tempValue = value;

	if (tempValue < MINDISCDCOFFSET)
	{
		tempValue = MINDISCDCOFFSET;
	} else if (tempValue > MAXDISCDCOFFSET)
	{
		tempValue = MAXDISCDCOFFSET;
	}

	sysData.disc.dcOffset = tempValue;

	return __SCPI_SUCCESS;
}

//方向键设置直流偏置
int tSysScpi::setMeasureOfDiscDCOffsetWithArrow(bool up)
{
	if (up)
	{
		return setMeasureOfDiscDCOffset(sysData.disc.dcOffset + 1.0);
	} else
	{
		return setMeasureOfDiscDCOffset(sysData.disc.dcOffset - 1.0);
	}
}

//脉冲旋钮设置直流偏置
int tSysScpi::setMeasureOfDiscDCOffsetWithRpg(int value)
{
	return setMeasureOfDiscDCOffset(sysData.disc.dcOffset + 0.01 * value);
}

//鉴频器标定
int tSysScpi::setMeasureOfDiscNormalize(void)
{
	if (sysData.isCalibrating)
	{
		return __SCPI_FAILED;
	}

	sysData.isCalibrating = true;
	QCoreApplication::processEvents();
	saveScreen();
	int progress = 0;
	outputToScreen(getTheTranslatorString("Discriminator Calibrating") + "......", progress, 0);

	setMeasureOfDiscState(false);

	if (setMeasureOfDiscState(true) == __SCPI_FAILED)
	{
		sysData.isCalibrating = false;
		return __SCPI_FAILED;
	}

	progress = 5;
	outputToScreen(getTheTranslatorString("Discriminator Calibrating") + "......", progress, 0);
	setSourceTrackGenState(false);

	int discResult[5];

	progress = 10;
	outputToScreen(getTheTranslatorString("Discriminator Calibrating") + "......", progress, 0);
	if (!getDiscriminationCalResult(100, &discResult[0]))
	{
		outputToScreen(getTheTranslatorString("Discriminator Calibrate Failed"), progress, 1);
		sysData.isCalibrating = false;
		return __SCPI_FAILED;
	}

	progress = 30;
	outputToScreen(getTheTranslatorString("Discriminator Calibrating") + "......", progress, 0);
	if (!getDiscriminationCalResult(200, &discResult[1]))
	{
		outputToScreen(getTheTranslatorString("Discriminator Calibrate Failed"), progress, 1);
		sysData.isCalibrating = false;
		return __SCPI_FAILED;
	}

	progress = 50;
	outputToScreen(getTheTranslatorString("Discriminator Calibrating") + "......", progress, 0);
	if (!getDiscriminationCalResult(400, &discResult[2]))
	{
		outputToScreen(getTheTranslatorString("Discriminator Calibrate Failed"), progress, 1);
		sysData.isCalibrating = false;
		return __SCPI_FAILED;
	}

	progress = 70;
	outputToScreen(getTheTranslatorString("Discriminator Calibrating") + "......", progress, 0);
	if (!getDiscriminationCalResult(50, &discResult[3]))
	{
		outputToScreen(getTheTranslatorString("Discriminator Calibrate Failed"), progress, 1);
		sysData.isCalibrating = false;
		return __SCPI_FAILED;
	}

	progress = 90;
	outputToScreen(getTheTranslatorString("Discriminator Calibrating") + "......", progress, 0);
	if (!getDiscriminationCalResult(25, &discResult[4]))
	{
		outputToScreen(getTheTranslatorString("Discriminator Calibrate Failed"), progress, 1);
		sysData.isCalibrating = false;
		return __SCPI_FAILED;
	}

	memcpy(sysData.disc.discStandard, discResult, sizeof sysData.disc.discStandard);
	saveDiscConfig();
	progress = 100;
	outputToScreen(getTheTranslatorString("Discriminator Calibrate Success") + "......", progress, 0);
	sysData.isCalibrating = false;

	return __SCPI_SUCCESS;
}

//设置测量参数信道带宽
int tSysScpi::setMeasSetupOfChannelWidth(double value)
{
	if (sysData.span.span <= 0)
	{
		return __SCPI_FAILED;
	}

	double tempValue = value;

	if (sysData.measure.isChannelPowerOn || sysData.measure.isObwOn || sysData.measure.isAcprOn)
	{
		if (sysData.measure.isChannelPowerOn || sysData.measure.isObwOn)
		{
			if (tempValue < (sysData.span.span / 10.0))
			{
				tempValue = sysData.span.span / 10.0;
			} else if (tempValue > sysData.span.span)
			{
				if (tempValue > MAXSPAN)
				{
					tempValue = MAXSPAN;
				}

				setFrequencyOfSpan(tempValue * 3);
			}
		} else if (sysData.measure.isAcprOn)
		{
			if (tempValue < (sysData.measureSetup.channelSpace / 100.0))
			{
				tempValue = sysData.measureSetup.channelSpace / 100.0;
			} else if (tempValue > sysData.measureSetup.channelSpace)
			{
				sysData.measureSetup.channelSpace = tempValue;

				if (((sysData.measureSetup.channelNums * 2 + 1) * sysData.measureSetup.channelSpace) > MAXSPAN)
				{
					tempValue = MAXSPAN / (sysData.measureSetup.channelNums * 2 + 1);
					sysData.measureSetup.channelSpace = tempValue;
				}

				setFrequencyOfSpan((sysData.measureSetup.channelNums * 2 + 1) * sysData.measureSetup.channelSpace);
			}
		}

		sysData.measureSetup.channelWidth = tempValue;
		sysData.bw.rbwAuto = false;
		sysData.bw.rbwStepCont = true;
		setBwOfRbw(0.02 * sysData.measureSetup.channelWidth);
	} else
	{
		return __SCPI_FAILED;
	}

	return __SCPI_SUCCESS;
}

//设置测量参数信道带宽
int tSysScpi::setMeasSetupOfChannelWidth(QString value)
{
	bool isOk = false;
	double tempValue = 0;

	tempValue = getFreqValue(value, &isOk);

	if (isOk)
	{
		return setMeasSetupOfChannelWidth(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//方向键设置测量参数信道带宽
int tSysScpi::setMeasSetupOfChannelWidthWithArrow(bool up)
{
	if (up)
	{
		return setMeasSetupOfChannelWidth(zoomInData(sysData.measureSetup.channelWidth, 2));
	} else
	{
		return setMeasSetupOfChannelWidth(zoomOutData(sysData.measureSetup.channelWidth, 2));
	}
}

//脉冲旋钮设置测量参数信道带宽
int tSysScpi::setMeasSetupOfChannelWidthWithRpg(int value)
{
	if (sysData.span.span >= 100)
	{
		return setMeasSetupOfChannelWidth(sysData.measureSetup.channelWidth + sysData.span.span * value * 0.01);
	} else
	{
		return setMeasSetupOfChannelWidth(sysData.measureSetup.channelWidth + sysData.measureSetup.channelWidth * value * 0.01);
	}
}

//设置测量参数信道间隔
int tSysScpi::setMeasSetupOfChannelSpace(double value)
{
	if (!sysData.measure.isAcprOn)
	{
		return __SCPI_FAILED;
	}

	double tempValue = value;

	if (tempValue < sysData.measureSetup.channelWidth)
	{
		tempValue = sysData.measureSetup.channelWidth;
	}

	if (((sysData.measureSetup.channelNums * 2 + 1) * tempValue) > MAXSPAN)
	{
		tempValue = MAXSPAN / (sysData.measureSetup.channelNums * 2 + 1);

		if (sysData.measureSetup.channelWidth > tempValue)
		{
			sysData.measureSetup.channelWidth = tempValue;
		}

		setFrequencyOfSpan(MAXSPAN);
	} else if (sysData.span.span < ((sysData.measureSetup.channelNums * 2 + 1) * tempValue))
	{
		setFrequencyOfSpan((sysData.measureSetup.channelNums * 2 + 1) * tempValue);
	}

	sysData.measureSetup.channelSpace = tempValue;
	sysData.bw.rbwAuto = false;
	sysData.bw.rbwStepCont = true;
	setBwOfRbw(0.02 * sysData.measureSetup.channelWidth);

	return __SCPI_SUCCESS;
}

//设置测量参数信道间隔
int tSysScpi::setMeasSetupOfChannelSpace(QString value)
{
	bool isOk = false;
	double tempValue = 0;

	tempValue = getFreqValue(value, &isOk);

	if (isOk)
	{
		return setMeasSetupOfChannelSpace(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//方向键设置测量参数信道间隔
int tSysScpi::setMeasSetupOfChannelSpaceWithArrow(bool up)
{
	if (up)
	{
		return setMeasSetupOfChannelSpace(zoomInData(sysData.measureSetup.channelSpace, 2));
	} else
	{
		return setMeasSetupOfChannelSpace(zoomOutData(sysData.measureSetup.channelSpace, 2));
	}
}

//脉冲旋钮设置测量参数信道间隔
int tSysScpi::setMeasSetupOfChannelSpaceWithRpg(int value)
{
	if (sysData.span.span >= 100)
	{
		return setMeasSetupOfChannelSpace(sysData.measureSetup.channelSpace + sysData.span.span * value * 0.01);
	} else
	{
		return setMeasSetupOfChannelSpace(sysData.measureSetup.channelSpace + sysData.measureSetup.channelSpace * value * 0.01);
	}
}

//设置测量参数邻道数目
int tSysScpi::setMeasSetupOfChannelNums(double value)
{
	if (!sysData.measure.isAcprOn)
	{
		return __SCPI_FAILED;
	}

	int tempValue = value;

	if (tempValue < MINADJCOUNT)
	{
		tempValue = MINADJCOUNT;
	} else if (tempValue > MAXADJCOUNT)
	{
		tempValue = MAXADJCOUNT;
	}

	if (((tempValue * 2 + 1) * sysData.measureSetup.channelSpace) > MAXSPAN)
	{
		tempValue = (MAXSPAN / sysData.measureSetup.channelSpace - 1) / 2;

		if (tempValue < MINADJCOUNT)
		{
			tempValue = MINADJCOUNT;
		} else if (tempValue > MAXADJCOUNT)
		{
			tempValue = MAXADJCOUNT;
		}

		sysData.measureSetup.channelSpace = MAXSPAN / (tempValue * 2 + 1);

		if (sysData.measureSetup.channelWidth > sysData.measureSetup.channelSpace)
		{
			sysData.measureSetup.channelWidth = sysData.measureSetup.channelSpace;
		}

		setFrequencyOfSpan(MAXSPAN);
	}

	sysData.measureSetup.channelNums = tempValue;
	sysData.bw.rbwAuto = false;
	sysData.bw.rbwStepCont = true;
	setBwOfRbw(0.02 * sysData.measureSetup.channelWidth);

	return __SCPI_SUCCESS;
}

//设置测量参数邻道数目
int tSysScpi::setMeasSetupOfChannelNums(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	int tempValue = getIntValue(value, &isOk);

	if (isOk)
	{
		return setMeasSetupOfChannelNums(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//方向键设置测量参数邻道数目
int tSysScpi::setMeasSetupOfChannelNumsWithArrow(bool up)
{
	if (up)
	{
		return setMeasSetupOfChannelNums(sysData.measureSetup.channelNums + 1);
	} else
	{
		return setMeasSetupOfChannelNums(sysData.measureSetup.channelNums - 1);
	}
}

//脉冲旋钮设置测量参数邻道数目
int tSysScpi::setMeasSetupOfChannelNumsWithRpg(int value)
{
	if (value > 0)
	{
		return setMeasSetupOfChannelNums(sysData.measureSetup.channelNums + 1);
	} else if (value < 0)
	{
		return setMeasSetupOfChannelNums(sysData.measureSetup.channelNums - 1);
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置测量参数占用带宽
int tSysScpi::setMeasSetupOfPowerPercent(double value)
{
	if (!sysData.measure.isObwOn)
	{
		return __SCPI_FAILED;
	}

	double tempValue = value;

	if (tempValue < MINPOWERPERCENT)
	{
		tempValue = MINPOWERPERCENT;
	} else if (tempValue > MAXPOWERPERCENT)
	{
		tempValue = MAXPOWERPERCENT;
	}

	sysData.measureSetup.powerPercent = tempValue;

	return __SCPI_SUCCESS;
}

//设置测量参数占用带宽
int tSysScpi::setMeasSetupOfPowerPercent(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	int tempValue = getDoubleValue(value, &isOk);

	if (isOk)
	{
		return setMeasSetupOfPowerPercent(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//方向键设置测量参数占用带宽
int tSysScpi::setMeasSetupOfPowerPercentWithArrow(bool up)
{
	if (up)
	{
		return setMeasSetupOfPowerPercent(sysData.measureSetup.powerPercent + 10.0);
	} else
	{
		return setMeasSetupOfPowerPercent(sysData.measureSetup.powerPercent - 10.0);
	}
}

//脉冲旋钮设置测量参数占用带宽
int tSysScpi::setMeasSetupOfPowerPercentWithRpg(int value)
{
	return setMeasSetupOfPowerPercent(sysData.measureSetup.powerPercent + value * 0.01);
}

//设置信道带宽测量扫宽
int tSysScpi::setMeasSetupOfChannelPowerOfSpan(QString value)
{
	bool isOk = false;
	double tempValue = getFreqValue(value, &isOk);

	if (isOk)
	{
		return setFrequencyOfSpan(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置占用带宽测量扫宽
int tSysScpi::setMeasSetupOfPowerPercentOfSpan(QString value)
{
	bool isOk = false;
	double tempValue = getFreqValue(value, &isOk);

	if (isOk)
	{
		return setFrequencyOfSpan(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//保存屏幕截图至本地
int tSysScpi::saveScreenPixmapToLocal(QString value)
{
	char temp[32] = { };
	QString dirName = "/home/sky/save";
	QString relativeFileName = "";
	QString absoluteFileName = dirName;
	QString message = "";

	if (value.isNull() || value.isEmpty())
	{
		relativeFileName = getDateTimeFileName(temp);
	} else
	{
		relativeFileName = value;

		int dotIndex = relativeFileName.indexOf(".", 0);

		if (dotIndex != -1)
		{
			relativeFileName = relativeFileName.mid(0, dotIndex);
		}
	}

	relativeFileName += ".png";
	absoluteFileName += "/";
	absoluteFileName += relativeFileName;

	checkLocalDirectory();

	if (getDirFileCount(dirName) <= (MAXFILECOUNT - 1))
	{
		QPixmap::grabWindow(qApp->desktop()->winId(), 0, 0, SCREENWIDTH, SCREENHEIGHT).save(absoluteFileName);
	} else
	{
		return _MSG_LOCAL_FILE_TOMAXLIMIT;
	}

	return _RESULT_SUCCESS;
}

//保存屏幕截图至闪存
int tSysScpi::saveScreenPixmapToUsb(QString value)
{
	long int code = 0;

	QPixmap pix = QPixmap::grabWindow(qApp->desktop()->winId(), 0, 0, SCREENWIDTH, SCREENHEIGHT);

	outputToScreen(getTheTranslatorString("Detecting usb disk") + "......", "");

	if (!isUsbDeviceConnect())
		code = _MSG_USBDISK_NOTFOUND;
	else
		code = _RESULT_SUCCESS;

	if (code != _RESULT_SUCCESS)
	{
		return code;
	}

	outputToScreen(getTheTranslatorString("Saving file to usb disk") + "......", "");
	code = checkUsbDirectory();

	if (code != _RESULT_SUCCESS)
	{
		return code;
	}

	char temp[32] = { };
	QString dirName = usbDirName;
	QString relativeFileName = "";
	QString absoluteFileName = dirName;
	QString message = "";

	if (value.isNull() || value.isEmpty())
	{
		relativeFileName = getDateTimeFileName(temp);
	} else
	{
		relativeFileName = value;
	}

	relativeFileName += ".png";
	absoluteFileName += "/";
	absoluteFileName += relativeFileName;

	pix.save(absoluteFileName);

//	sleep(3);
	// return disConnectFromUsbDisk();
	return _RESULT_SUCCESS;
}

//保存迹线数据
int tSysScpi::saveTraceData(QString dirName, QString absoluteFileName)
{
	char temp[32] = { };
	if (getDirFileCount(dirName) <= (MAXFILECOUNT - 1))
	{
		QFile file(absoluteFileName);

		if (not file.open(QIODevice::WriteOnly | QIODevice::Text))
		{
			return _MSG_SYSTEM_FILECREATEERROR;
		}

		QTextStream out(&file);
		QString amptUnit = "";
		QString rowStr = "";

		switch (sysData.ampt.refUnit)
		{
			case rudBm:
				amptUnit = "dBm";
				break;
			case rudBmV:
				amptUnit = "dBmV";
				break;
			case rudBuV:
				amptUnit = "dBuV";
				break;
			case ruW:
				amptUnit = "W";
				break;
			case ruV:
				amptUnit = "V";
				break;
			default:
				amptUnit = "dBm";
				break;
		}

		out << "No, Freq(Hz)";

		for (int i = 0; i < TRACECOUNT; i++)
		{
			out << ", " << "Ampt_Trace" << QString(intToString(i + 1, temp)).trimmed() << "(" << amptUnit << ")";
		}

		out << endl;

		for (int i = 0; i < sysData.sweep.sweepPoints; i++)
		{
			rowStr = QString(intToString(i + 1, temp)).trimmed();
			rowStr += ", ";
			rowStr += QString(longLongIntToString(sysData.freq.start + i * sysData.span.span / (sysData.sweep.sweepPoints - 1), temp)).trimmed();

			for (int j = 0; j < TRACECOUNT; j++)
			{
				rowStr += ", ";
				rowStr += floatToString(sysData.trace[j].value[i], 0, 2, temp);
			}

			out << rowStr << endl;
		}

		out.flush();
		file.close();

	} else
	{
		return _MSG_LOCAL_FILE_TOMAXLIMIT;
	}
	return _RESULT_SUCCESS;
}

//保存迹线数据至本地
int tSysScpi::saveTraceDataToLocal(QString value)
{
	char temp[32] = { };
	QString dirName = "/home/sky/save";
	QString relativeFileName = "";
	QString absoluteFileName = dirName;
	QString absoluteFileName2 = dirName;
	QString message = "";
	long int code = 0;
	if (value.isNull() || value.isEmpty())
	{
		relativeFileName = getDateTimeFileName(temp);
	} else
	{
		relativeFileName = value;

		int dotIndex = relativeFileName.indexOf(".", 0);

		if (dotIndex != -1)
		{
			relativeFileName = relativeFileName.mid(0, dotIndex);
		}
	}

	absoluteFileName2 += "/trace/" + relativeFileName + ".png";
	relativeFileName += ".csv";
	absoluteFileName += "/";
	absoluteFileName += relativeFileName;

	checkLocalDirectory();

	code = saveTraceData(dirName, absoluteFileName);
	QPixmap::grabWindow(qApp->desktop()->winId(), 0, 0, SCREENWIDTH, SCREENHEIGHT).save(absoluteFileName2);

	return code;
}

//保存迹线数据至闪存
int tSysScpi::saveTraceDataToUsb(QString value)
{
	long int code = 0;

	outputToScreen(getTheTranslatorString("Detecting usb disk") + "......", "");

	if (!isUsbDeviceConnect())
		code = _MSG_USBDISK_NOTFOUND;
	else
		code = _RESULT_SUCCESS;

	if (code != _RESULT_SUCCESS)
	{
		return code;
	}

	outputToScreen(getTheTranslatorString("Saving file to usb disk") + "......", "");
	code = checkUsbDirectory();

	if (code != _RESULT_SUCCESS)
	{
		return code;
	}

	char temp[32] = { };
	QString dirName = usbDirName;
	QString relativeFileName = "";
	QString absoluteFileName = dirName;
	QString message = "";

	if (value.isNull() || value.isEmpty())
	{
		relativeFileName = getDateTimeFileName(temp);
	} else
	{
		relativeFileName = value;
	}

	relativeFileName += ".csv";
	absoluteFileName += "/";
	absoluteFileName += relativeFileName;

	checkUsbDirectory();

	code = saveTraceData(dirName, absoluteFileName);

	return code;
}

//载入用户参数
int tSysScpi::loadUserState(void)
{
	QString fileName = QCoreApplication::applicationDirPath() + "/user.dat";

	FILE *fp;
	fp = fopen(fileName.toStdString().c_str(), "rb");
	if (fp != NULL)
	{
		fread(&sysData, sizeof sysData, 1, fp);
		fclose(fp);
	}

	return _RESULT_SUCCESS;
}

//保存用户参数
int tSysScpi::saveUserState(void)
{
	QString fileName = QCoreApplication::applicationDirPath() + "/user.dat";
	FILE *fp;
	fp = fopen(fileName.toStdString().c_str(), "wb");

	if (fp != NULL)
	{
		fwrite(&sysData, sizeof sysData, 1, fp);
		fclose(fp);
	}

	return _RESULT_SUCCESS;
}

//载入关机参数
int tSysScpi::loadPowerOffState(void)
{
	QString fileName = QCoreApplication::applicationDirPath() + "/poweroff.dat";

	FILE *fp;
	fp = fopen(fileName.toStdString().c_str(), "rb");
	if (fp != NULL)
	{
		fread(&sysData, sizeof sysData, 1, fp);
		fclose(fp);
	}

	return _RESULT_SUCCESS;
}

//保存关机参数
int tSysScpi::savePowerOffState(void)
{
	QString fileName = QCoreApplication::applicationDirPath() + "/poweroff.dat";
	FILE *fp;
	fp = fopen(fileName.toStdString().c_str(), "wb");

	if (fp != NULL)
	{
		fwrite(&sysData, sizeof sysData, 1, fp);
		fclose(fp);
	}

	return _RESULT_SUCCESS;
}

//设置打印机类型
int tSysScpi::setPrintOfColor(int value)
{
	sysData.print.isColorPrinter = value == 1;

	return __SCPI_RETURN;
}

//设置打印机类型
int tSysScpi::setPrintOfColor(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	int tempValue = getBoolValue(value.toUpper().trimmed(), &isOk);

	if (isOk)
	{
		return setPrintOfColor(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置打印方向
int tSysScpi::setPrintOfOrientation(int value)
{
	sysData.print.isOrientationHor = value == 1;

	return __SCPI_SUCCESS;
}

//设置打印方向
int tSysScpi::setPrintOfOrientation(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	QString tempValue = value.toUpper().trimmed();

	if (tempValue == "LAND" || tempValue == "LANDSCAPE")
	{
		return setPrintOfOrientation(1);
	} else if (tempValue == "PORT" || tempValue == "PORTRAIT")
	{
		return setPrintOfOrientation(0);
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置打印纸张大小
int tSysScpi::setPrintOfPaperSize(int value)
{
	if (value >= 0 && value <= 4)
	{
		sysData.print.paper = paperType(value);

		return __SCPI_SUCCESS;
	}

	return __SCPI_FAILED;
}

//设置打印纸张大小
int tSysScpi::setPrintOfPaperSize(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	QString tempValue = value.toUpper().trimmed();

	if (tempValue == "DEF" || tempValue == "DEFAULT" || tempValue == "A4")
	{
		return setPrintOfPaperSize(ptA4);
	} else if (tempValue == "A3")
	{
		return setPrintOfPaperSize(ptA3);
	} else if (tempValue == "B5")
	{
		return setPrintOfPaperSize(ptB5);
	} else if (tempValue == "C5")
	{
		return setPrintOfPaperSize(ptC5);
	} else if (tempValue == "LETT" || tempValue == "LETTER")
	{
		return setPrintOfPaperSize(ptLetter);
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置扫描点数
int tSysScpi::setPrintOfNums(double value)
{
	int tempValue = (int) value;

	if (tempValue < MINPRINTNUMS)
	{
		tempValue = MINPRINTNUMS;
	} else if (tempValue > MAXPRINTNUMS)
	{
		tempValue = MAXPRINTNUMS;
	}

	sysData.print.nums = tempValue;

	return 0;
}

//设置扫描点数
int tSysScpi::setPrintOfNums(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_SUCCESS;
	}

	bool isOk = false;
	int tempValue = getDoubleValue(value, &isOk);

	if (isOk)
	{
		return setPrintOfNums(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//方向键设置扫描点数
int tSysScpi::setPrintOfNumsWithArrow(bool up)
{
	if (up)
	{
		return setPrintOfNums(zoomInData(sysData.print.nums, 2));
	} else
	{
		return setPrintOfNums(zoomOutData(sysData.print.nums, 2));
	}
}

//脉冲旋钮设置扫描点数
int tSysScpi::setPrintOfNumsWithRpg(int value)
{
	return setPrintOfNums(sysData.print.nums + value);
}

//设置IP地址
int tSysScpi::setLanIP(QString value)
{
	strcpy(sysData.system.io.lan.ip, value.toStdString().c_str());
	saveDeviceConfig();
	resetSystemIOOfLan();
	
	QString strCmd = "";

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

	for (int i = 0; i < strList.count(); i++)
	{
		if (strList.at(i).toUpper().indexOf("IPADDR", 0) >= 0)
		{
			strList.replace(i, "IPADDR=" + QString(sysData.system.io.lan.ip).trimmed());
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

	return __SCPI_SUCCESS;
}

//设置子网掩码地址
int tSysScpi::setLanMask(QString value)
{
	strcpy(sysData.system.io.lan.mask, value.toStdString().c_str());
	saveDeviceConfig();
	resetSystemIOOfLan();

	QString strCmd = "";

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

	for (int i = 0; i < strList.count(); i++)
	{
		if (strList.at(i).toUpper().indexOf("NETMASK", 0) >= 0)
		{
			strList.replace(i, "NETMASK=" + QString(sysData.system.io.lan.mask).trimmed());
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
	return __SCPI_SUCCESS;
}

//设置网关地址
int tSysScpi::setLanGate(QString value)
{
	strcpy(sysData.system.io.lan.gate, value.toStdString().c_str());
	saveDeviceConfig();
	resetSystemIOOfLan();

	QString strCmd = "";

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

	for (int i = 0; i < strList.count(); i++)
	{
		if (strList.at(i).toUpper().indexOf("GATEWAY", 0) >= 0)
		{
			strList.replace(i, "GATEWAY=" + QString(sysData.system.io.lan.gate).trimmed());
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
	return __SCPI_SUCCESS;
}

//设置GPIB地址
int tSysScpi::setGpibAddr(int value)
{
	int tempValue = value;

	if (tempValue < MINGPIBADDR)
	{
		tempValue = MINGPIBADDR;
	} else if (tempValue > MAXGPIBADDR)
	{
		tempValue = MAXGPIBADDR;
	}

	sysData.system.io.gpib.addr = tempValue;
	saveDeviceConfig();

	setSystemIOGpibMainAddr(sysData.system.io.gpib.addr);

	return 0;
}

//设置GPIB地址
int tSysScpi::setGpibAddr(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	int tempValue = value.toInt(&isOk);

	if (isOk)
	{
		return setGpibAddr(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//方向键设置GPIB地址
int tSysScpi::setGpibAddrWithArrow(bool up)
{
	if (up)
	{
		return setGpibAddr(sysData.system.io.gpib.addr + 10);
	} else
	{
		return setGpibAddr(sysData.system.io.gpib.addr - 10);
	}
}

//脉冲旋钮设置GPIB地址
int tSysScpi::setGpibAddrWithRpg(int value)
{
	return setGpibAddr(sysData.system.io.gpib.addr + value);
}

//设置串口
int tSysScpi::setupSerial(void)
{
	if (comHandle <= 0)
	{
		return __SCPI_FAILED;
	}

	struct termios options;

	//get attributes
	tcgetattr(comHandle, &options);

	//baudrate
	switch (sysData.system.io.com.baudrate)
	{
		case br1200:
			cfsetispeed(&options, B1200);
			cfsetospeed(&options, B1200);
			break;
		case br2400:
			cfsetispeed(&options, B2400);
			cfsetospeed(&options, B2400);
			break;
		case br4800:
			cfsetispeed(&options, B4800);
			cfsetospeed(&options, B4800);
			break;
		case br9600:
			cfsetispeed(&options, B9600);
			cfsetospeed(&options, B9600);
			break;
		case br19200:
			cfsetispeed(&options, B19200);
			cfsetospeed(&options, B19200);
			break;
		case br38400:
			cfsetispeed(&options, B38400);
			cfsetospeed(&options, B38400);
			break;
		case br57600:
			cfsetispeed(&options, B57600);
			cfsetospeed(&options, B57600);
			break;
		case br115200:
			cfsetispeed(&options, B115200);
			cfsetospeed(&options, B115200);
			break;
	}

	//enable receiver and set local mode
	options.c_cflag |= (CLOCAL | CREAD);
	options.c_lflag &= ~(ICANON | ECHO);

	//databits
	options.c_cflag &= ~CSIZE;
	switch (sysData.system.io.com.databits)
	{
		case db5:
			options.c_cflag |= CS5;
			break;
		case db6:
			options.c_cflag |= CS6;
			break;
		case db7:
			options.c_cflag |= CS7;
			break;
		case db8:
			options.c_cflag |= CS8;
			break;
	}

	//check
	switch (sysData.system.io.com.check)
	{
		case cmEven:
			options.c_cflag |= PARENB;
			options.c_cflag &= ~PARODD;
			options.c_iflag |= (INPCK | ISTRIP);
			break;
		case cmOdd:
			options.c_cflag |= (PARODD | PARENB);
			options.c_iflag |= (INPCK | ISTRIP);
			break;
		case cmNone:
			options.c_cflag &= ~PARENB;
			options.c_iflag &= ~INPCK;
			break;
		case cmSpace:
			options.c_cflag &= ~PARENB;
			options.c_cflag &= ~CSTOPB;
			break;
	}

	//stopbits
	switch (sysData.system.io.com.stopbits)
	{
		case sb1:
			options.c_cflag &= ~CSTOPB;
			break;
		case sb2:
			options.c_cflag |= CSTOPB;
			break;
	}

	//flow control
	switch (sysData.system.io.com.flow)
	{
		case fcXonXoff:
			options.c_cflag |= IXON | IXOFF | IXANY;
			break;
		case fcHardware:
			options.c_cflag |= CRTSCTS;
			break;
		case fcNone:
			options.c_cflag &= ~CRTSCTS;
			break;
	}

	//output flag control(prevent 0x0a -> 0x0d 0x0a)
	options.c_oflag &= ~(INLCR | IGNCR | ICRNL);
	options.c_oflag &= ~(ONLCR | OCRNL);

	//time
	options.c_cc[VMIN] = 0;
	options.c_cc[VTIME] = 0;

	tcflush(comHandle, TCIFLUSH);

	if (tcsetattr(comHandle, TCSAFLUSH, &options) == 0)
	{
		return __SCPI_SUCCESS;
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置串口波特率
int tSysScpi::setSystemSerialOfBaudrate(int value)
{
	if (value < 0 || value > 7)
	{
		return __SCPI_FAILED;
	}

	if (sysData.system.io.com.baudrate != baudRate(value))
	{
		sysData.system.io.com.baudrate = baudRate(value);
		saveDeviceConfig();
		resetSystemIOOfSerial();
	}

	return __SCPI_SUCCESS;
}

//设置串口数据位
int tSysScpi::setSystemSerialOfDatabits(int value)
{
	if (value < 0 || value > 3)
	{
		return __SCPI_FAILED;
	}

	if (sysData.system.io.com.databits != dataBits(value))
	{
		sysData.system.io.com.databits = dataBits(value);
		saveDeviceConfig();
		resetSystemIOOfSerial();
	}

	return __SCPI_SUCCESS;
}

//设置串口停止位
int tSysScpi::setSystemSerialOfStopbits(int value)
{
	if (value < 0 || value > 1)
	{
		return __SCPI_FAILED;
	}

	if (sysData.system.io.com.stopbits != stopBits(value))
	{
		sysData.system.io.com.stopbits = stopBits(value);
		saveDeviceConfig();
		resetSystemIOOfSerial();
	}

	return __SCPI_SUCCESS;
}

//设置串口校验
int tSysScpi::setSystemSerialOfCheck(int value)
{
	if (value < 0 || value > 3)
	{
		return __SCPI_FAILED;
	}

	if (sysData.system.io.com.check != checkMode(value))
	{
		sysData.system.io.com.check = checkMode(value);
		saveDeviceConfig();
		resetSystemIOOfSerial();
	}

	return __SCPI_SUCCESS;
}

//设置串口流控制
int tSysScpi::setSystemSerialOfFlowControl(int value)
{
	if (value < 0 || value > 2)
	{
		return __SCPI_FAILED;
	}

	if (sysData.system.io.com.flow != flowControl(value))
	{
		sysData.system.io.com.flow = flowControl(value);
		saveDeviceConfig();
		resetSystemIOOfSerial();
	}

	return __SCPI_SUCCESS;
}

//设置系统语言
int tSysScpi::setSystemLanguage(int value)
{
	sysData.system.isLanguageEn = value == 1;

	if (sysData.system.isLanguageEn)
	{
		saveSystemSetting("system", "language", "english");
	} else
	{
		saveSystemSetting("system", "language", "chinese");
	}

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

//设置系统日期时间显示
int tSysScpi::setSystemDateTimeShow(int value)
{
	sysData.system.datetime.isVisible = value == 1;
	saveDeviceConfig();

	return __SCPI_SUCCESS;
}

//设置系统日期时间显示
int tSysScpi::setSystemDateTimeShow(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	int tempValue = getBoolValue(value.toUpper().trimmed(), &isOk);

	if (isOk)
	{
		return setSystemDateTimeShow(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置系统日期格式
int tSysScpi::setSystemDateFormat(int value)
{
	if (value >= 0 && value <= 1)
	{
		sysData.system.datetime.style = dateTimeStyle(value);
		saveDeviceConfig();
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置系统日期格式
int tSysScpi::setSystemDateFormat(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	QString tempValue = value.toUpper().trimmed();

	if (tempValue == "YMD" || tempValue == "YMDHMS")
	{
		return setSystemDateFormat(dtsYMDHMS);
	} else if (tempValue == "HMS" || tempValue == "HMSYMD")
	{
		return setSystemDateFormat(dtsHMSYMD);
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置系统日期
int tSysScpi::setSystemDate(QString value)
{
	if (value.isNull() || value.isEmpty() || eva.isEvaluate)
	{
		return _MSG_DATETIME_SETFAILED;
	}

	QString tempValue = "";

	for (int i = 0; i < value.length(); i++)
	{
		if (value.at(i) == ',' || value.at(i) == ' ' || value.at(i) == 0x0)
		{
			continue;
		}

		tempValue += value.at(i);
	}

	long int code = isValidDate(tempValue);

	if (code != _RESULT_SUCCESS)
	{
		return code;
	}

	QString dateTimeStr1 = "date -s " + getNewDateString(tempValue) + " &";
	QString dateTimeStr2 = "hwclock -w &";

	if (system(dateTimeStr1.toStdString().c_str()) == 0 && system(dateTimeStr2.toStdString().c_str()) == 0)
	{
		if (sysData.system.datetime.isPCF8563)
		{
			time_t t = time(NULL);
			tm *tp = localtime(&t);

			sysData.system.datetime.year = tp->tm_year + 1900;
			sysData.system.datetime.month = tp->tm_mon + 1;
			sysData.system.datetime.day = tp->tm_mday;
			sysData.system.datetime.hour = tp->tm_hour;
			sysData.system.datetime.minute = tp->tm_min;
			sysData.system.datetime.second = tp->tm_sec;

			if (setLocalDateTime() == _RESULT_SUCCESS)
			{
				return _RESULT_SUCCESS;
			} else
			{
				return _MSG_DATETIME_SETFAILED;
			}
		}

		return _RESULT_SUCCESS;
	} else
	{
		return _MSG_DATETIME_SETFAILED;
	}
}

//设置系统时间
int tSysScpi::setSystemTime(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return _MSG_DATETIME_SETFAILED;
	}

	QString tempValue = "";

	for (int i = 0; i < value.length(); i++)
	{
		if (value.at(i) == ',' || value.at(i) == ' ' || value.at(i) == 0x0)
		{
			continue;
		}

		tempValue += value.at(i);
	}

	long int code = isValidTime(tempValue);

	if (code != _RESULT_SUCCESS)
	{
		return code;
	}

	QString dateTimeStr1 = "date -s " + getNewTimeString(tempValue) + " &";
	QString dateTimeStr2 = "hwclock -w &";

	if (system(dateTimeStr1.toStdString().c_str()) == 0 && system(dateTimeStr2.toStdString().c_str()) == 0)
	{
		if (sysData.system.datetime.isPCF8563)
		{
			time_t t = time(NULL);
			tm *tp = localtime(&t);

			sysData.system.datetime.year = tp->tm_year + 1900;
			sysData.system.datetime.month = tp->tm_mon + 1;
			sysData.system.datetime.day = tp->tm_mday;
			sysData.system.datetime.hour = tp->tm_hour;
			sysData.system.datetime.minute = tp->tm_min;
			sysData.system.datetime.second = tp->tm_sec;

			if (setLocalDateTime() == _RESULT_SUCCESS)
			{
				return _RESULT_SUCCESS;
			} else
			{
				return _MSG_DATETIME_SETFAILED;
			}
		}

		return _RESULT_SUCCESS;
	} else
	{
		return _MSG_DATETIME_SETFAILED;
	}
}

//获取LAN物理地址
QStringList tSysScpi::getLanRealIPAndAddress(void)
{
	QStringList result;
	QString temp = "";

	//获取所有网络接口的列表
	QList<QNetworkInterface> list = QNetworkInterface::allInterfaces();

	//遍历每一个网络接口
	foreach(QNetworkInterface interface, list)
	{
		//设备名
		//	temp = "Device:" + interface.name();
		//	result_test << temp.trimmed();

		//获取eth0的相关信息
		if(interface.name().trimmed() == "eth0")
		{
			//硬件地址
			result << interface.hardwareAddress().trimmed();

			//获取IP地址条目列表，每个条目中包含一个IP地址，一个子网掩码，一个广播地址
			QList <QNetworkAddressEntry> entryList = interface.addressEntries();
			//遍历每一个IP地址
			foreach(QNetworkAddressEntry entry, entryList)
			{
				//IP地址
				result << entry.ip().toString().trimmed();

				//子网掩码
				result << entry.netmask().toString().trimmed();

			}
		}
	}

	return result;
}

//设置LAN
int tSysScpi::resetSystemIOOfLan(void)
{
	QStringList strList;
	char hwAddr[64] = { };

	sprintf(hwAddr, "%X%X:%X%X:%X%X:%X%X:%X%X:%X%X", sysData.system.lanMacAddress[0], sysData.system.lanMacAddress[1], sysData.system.lanMacAddress[2], sysData.system.lanMacAddress[3], sysData.system.lanMacAddress[4], sysData.system.lanMacAddress[5], sysData.system.lanMacAddress[6], sysData.system.lanMacAddress[7], sysData.system.lanMacAddress[8], sysData.system.lanMacAddress[9], sysData.system.lanMacAddress[10], sysData.system.lanMacAddress[11]);

	strList.clear();
	strList << "ifconfig eth0 " + QString(sysData.system.io.lan.ip).trimmed() + " netmask " + QString(sysData.system.io.lan.mask).trimmed() + " hw ether " + QString(hwAddr).trimmed();
	strList << "route add default gw " + QString(sysData.system.io.lan.gate).trimmed();

	for (int i = 0; i < strList.count(); i++)
	{
		system(strList.at(i).toStdString().c_str());
	}

	return __SCPI_SUCCESS;
}

//设置串行口
int tSysScpi::resetSystemIOOfSerial(void)
{
	return setupSerial();
}

//设置开机参数
int tSysScpi::setSystemPoweronParam(int value)
{
	if (value < 0 || value > 2)
	{
		return __SCPI_FAILED;
	}

	if (sysData.system.powerOnPreset.powerOnParam != systemParam(value))
	{
		sysData.system.powerOnPreset.powerOnParam = systemParam(value);
		saveDeviceConfig();
	}

	return __SCPI_SUCCESS;
}

//设置开机参数
int tSysScpi::setSystemPoweronParam(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	QString tempValue = value.toUpper().trimmed();

	if (tempValue == "FACT" || tempValue == "FACTORY")
	{
		return setSystemPoweronParam(spFactory);
	} else if (tempValue == "USER")
	{
		return setSystemPoweronParam(spUser);
	} else if (tempValue == "LAST")
	{
		return setSystemPoweronParam(spPowerOff);
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置复位参数
int tSysScpi::setSystemPresetParam(int value)
{
	if (value < 0 || value > 2)
	{
		return __SCPI_FAILED;
	}

	if (sysData.system.powerOnPreset.presetParam != systemParam(value))
	{
		sysData.system.powerOnPreset.presetParam = systemParam(value);
		saveDeviceConfig();
	}

	return __SCPI_SUCCESS;
}

//设置复位参数
int tSysScpi::setSystemPresetParam(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	QString tempValue = value.toUpper().trimmed();

	if (tempValue == "FACT" || tempValue == "FACTORY")
	{
		return setSystemPresetParam(spFactory);
	} else if (tempValue == "USER")
	{
		return setSystemPresetParam(spUser);
	} else if (tempValue == "LAST")
	{
		return setSystemPresetParam(spPowerOff);
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置LCD背光
int tSysScpi::setSystemLcdBacklight(double value)
{
	int tempValue = (int) value;

	if (tempValue < MINBACKLIGHT)
	{
		tempValue = 0;
	} else if (tempValue > MAXBACKLIGHT)
	{
		tempValue = 100;
	}

	sysData.system.backLight = tempValue;

	if (ifHandle >= 0)
	{
		ioctl(ifHandle, 0xf5, int(175 - 1.75 * sysData.system.backLight));

		saveLCDBacklight();

		return __SCPI_SUCCESS;
	}

	return __SCPI_FAILED;
}

//设置LCD背光
int tSysScpi::setSystemLcdBacklight(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	int tempValue = getIntValue(value, &isOk);

	if (isOk)
	{
		return setSystemLcdBacklight(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//方向键设置LCD背光
int tSysScpi::setSystemLcdBacklightWidthArrow(bool up)
{
	int tempValue = sysData.system.backLight;

	if (up)
	{
		return setSystemLcdBacklight(tempValue + 1);
	} else
	{
		return setSystemLcdBacklight(tempValue - 1);
	}
}

//脉冲旋钮设置LCD背光
int tSysScpi::setSystemLcdBacklightWithRpg(int value)
{
	return setSystemLcdBacklight(sysData.system.backLight + value);
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

//设置序列号
int tSysScpi::setSystemServiceOfSerialNo(QString value)
{
	if (!sysData.system.isServiceUnlock)
	{
		return __SCPI_SERVICELOCK;
	}

	for (int i = 0; i < value.length(); i++)
	{
		//	if ((value.at(i) < '0' || value.at(i) > '9')||(value.at(11) != '1' && value.at(11) != '2'))
		if (value.at(i) < '0' || value.at(i) > '9')
		{
			showRunAlarm(getTheTranslatorString("SerialNo Input is Invalid!"));
			sleep(2);
			return __SCPI_FAILED;
		}
	}

	if (value == "000000000000")
	{
		return __SCPI_FAILED;
	}

	QString tempValue = "";

	if (sysData.system.isSerialNoOldModeOn)
	{
		if (value.isNull() || value.isEmpty() || value.length() != 9)
		{
			showRunAlarm(getTheTranslatorString("SerialNo Lenght is error!"));
			sleep(2);
			return __SCPI_FAILED;
		}

		tempValue = value.toUpper().trimmed();
	} else
	{
		if (value.isNull() || value.isEmpty() || value.length() != 12)
		{
			showRunAlarm(getTheTranslatorString("SerialNo Lenght is error!"));
			sleep(2);
			return __SCPI_FAILED;
		}

		tempValue = "BL" + value.toUpper().trimmed();
	}

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

	return __SCPI_SUCCESS;
}

//方向键设置网卡物理地址
int tSysScpi::setSystemServiceOfLanMacAddressWidthArrow(bool up)
{
	unsigned char value = sysData.system.lanMacAddress[sysData.system.lanMacAddrIndex];

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
	}

	QString strCmd = "";

	strCmd = "ifconfig eth0 hw ether " + macString.trimmed();
	system(strCmd.toStdString().c_str());

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

	return __SCPI_SUCCESS;
}

//方向键设置网卡物理地址
int tSysScpi::setSystemServiceOfLanMacAddressWidthRpg(int value)
{
	return setSystemServiceOfLanMacAddressWidthArrow(value > 0);
}

//设置系统设备开启
int tSysScpi::setSystemServiceDeviceOn(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	if (value == "LOG")
	{
		return setSystemServiceOfSelectLog(true);
	} else if (value == "GPIB")
	{
		return setSystemServiceOfGpib(true);
	} else if (value == "EVA" || value == "EVALUATE")
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

	if (value == "LOG")
	{
		return setSystemServiceOfSelectLog(false);
	} else if (value == "GPIB")
	{
		return setSystemServiceOfGpib(false);
	} else if (value == "EVA" || value == "EVALUATE")
	{
		return setSystemServiceOfEvalute(false);
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置系统选件开启
int tSysScpi::setSystemServiceOptionOn(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	if (value == "GEN")
	{
		return setSystemServiceOfTrackSelect(true);
	} else if (value == "FFT")
	{
		return setSystemServiceOptionFFT10Hz(true);
	} else if (value == "DEMOD")
	{
		return setSystemServiceOptionDemod(true);
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置系统选件关闭
int tSysScpi::setSystemServiceOptionOff(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	if (value == "GEN")
	{
		return setSystemServiceOfTrackSelect(false);
	} else if (value == "FFT")
	{
		return setSystemServiceOptionFFT10Hz(false);
	} else if (value == "DEMOD")
	{
		return setSystemServiceOptionDemod(false);
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置系统序列号模式
int tSysScpi::setSystemServiceSerialNoMode(int value)
{
	sysData.isComCalbration = value == 1;

	return __SCPI_SUCCESS;
}

//设置系统序列号模式
int tSysScpi::setSystemServiceSerialNoMode(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	int tempValue = getBoolValue(value.toUpper().trimmed(), &isOk);

	if (isOk)
	{
		return setSystemServiceSerialNoMode(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置系统校准模式
int tSysScpi::setSystemServiceCalbrationMode(int value)
{
	sysData.isComCalbration = value == 1;

	return __SCPI_SUCCESS;
}

//设置系统FFT选件
int tSysScpi::setSystemServiceOptionFFT10Hz(int value)
{
	sysData.options.isFft10HzOn = value == 1;
	saveDeviceConfig();

	return __SCPI_SUCCESS;
}

//设置系统FFT选件
int tSysScpi::setSystemServiceOptionFFT10Hz(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	int tempValue = getBoolValue(value.toUpper().trimmed(), &isOk);

	if (isOk)
	{
		return setSystemServiceOptionFFT10Hz(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置系统解调选件
int tSysScpi::setSystemServiceOptionDemod(int value)
{
	sysData.options.isDemodOn = value == 1;//save state of demod
	saveDeviceConfig();

	return __SCPI_SUCCESS;
}

//设置系统解调选件
int tSysScpi::setSystemServiceOptionDemod(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	int tempValue = getBoolValue(value.toUpper().trimmed(), &isOk);

	if (isOk)
	{
		return setSystemServiceOptionDemod(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置直采选件
int tSysScpi::setSystemServiceOptionDirectChannel(int value)
{
	sysData.options.directChannelOptionOn = value == 1;
	saveDeviceConfig();

	return __SCPI_SUCCESS;
}

//设置内部信号
int tSysScpi::setSystemChannelSelect(int value)
{
	sysData.system.isInternalSignalOn = value == 1;
	controlRf();

	return __SCPI_SUCCESS;
}

//设置内部信号
int tSysScpi::setSystemChannelSelect(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	QString tempValue = value.toUpper().trimmed();

	if (tempValue == "INT" || tempValue == "INTERNAL")
	{
		return setSystemChannelSelect(1);
	} else if (tempValue == "EXT" || tempValue == "EXTERNAL")
	{
		return setSystemChannelSelect(0);
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置GPIB主地址
int tSysScpi::setSystemIOGpibMainAddr(int value)
{
	if (gpibHandle >= 0)
	{
		int tempValue = value;

		if (tempValue < MINGPIBADDR)
		{
			tempValue = MINGPIBADDR;
		} else if (tempValue > MAXGPIBADDR)
		{
			tempValue = MAXGPIBADDR;
		}

		ioctl(gpibHandle, 0xff, tempValue);

		return __SCPI_SUCCESS;
	} else
	{
		return __SCPI_FAILED;
	}
}
//选择GPIB
int tSysScpi::setSystemServiceOfGpib(int value)
{
	sysData.system.isGpibOn = value == 1;
	saveDeviceConfig();

	return __SCPI_SUCCESS;
}

//选择GPIB
int tSysScpi::setSystemServiceOfGpib(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	int tempValue = getBoolValue(value.toUpper().trimmed(), &isOk);

	if (isOk)
	{
		return setSystemServiceOfSelectLog(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//选择LOG显示
int tSysScpi::setSystemServiceOfSelectLog(int value)
{
	QString typeName = "";
	sysData.system.isLogOn = value == 1;
	if (sysData.system.isLogOn)
	{
		typeName = DEVICETYPE;
	} else
	{
		typeName = " ";
	}
	saveDeviceType(typeName);

	saveDeviceConfig();
	return __SCPI_SUCCESS;
}

//选择LOG显示
int tSysScpi::setSystemServiceOfSelectLog(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	int tempValue = getBoolValue(value.toUpper().trimmed(), &isOk);

	if (isOk)
	{
		return setSystemServiceOfSelectLog(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//选择跟踪源模式
int tSysScpi::setSystemServiceOfTrackSelect(int value)
{
	sysData.options.isTrack36GHzOn = value == 1;
	saveDeviceConfig();
	controlRf();

	return __SCPI_SUCCESS;
}

//选择跟踪源模式
int tSysScpi::setSystemServiceOfTrackSelect(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	int tempValue = getBoolValue(value.toUpper().trimmed(), &isOk);

	if (isOk)
	{
		return setSystemServiceOfTrackSelect(tempValue);
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

int tSysScpi::unLockSystem(void)
{
	setSystemServiceOfEvalute(0);

	return __SCPI_SUCCESS;
}

//获取当前文件选择索引
int tSysScpi::getFileOfNowSelectIndex(void)
{
	return sysData.file.nowPageIndex * sysData.file.perPage + sysData.file.nowSelIndex;
}

//删除指定文件
int tSysScpi::setFileOfDeleteTheSelect(void)
{
	if (sysData.file.pages > 0 && sysData.file.nowPageIndex >= 0 && sysData.file.nowPageCount > 0 && sysData.file.nowSelIndex >= 0 && sysData.file.nowSelIndex < sysData.file.nowPageCount)
	{
		int pageIndex = sysData.file.nowPageIndex;
		int selIndex = sysData.file.nowSelIndex;
		int absIndex = getFileOfNowSelectIndex();

		QString fileName = fileList.at(absIndex).absoluteFilePath();

		if (fileList.at(absIndex).suffix().toUpper() == "CSV")
		{
			QString csvFileName = fileList.at(absIndex).absolutePath() + "/trace/" + fileList.at(absIndex).baseName() + ".png";

			if (QFile(csvFileName).exists())
			{
				csvFileName = "rm " + csvFileName;
				system(csvFileName.toStdString().c_str());
			}
		}

		if (QFile(fileName).exists())
		{
			fileName = "rm " + fileName;
			system(fileName.toStdString().c_str());
		}

		getSystemFileList();

		if (pageIndex <= sysData.file.pages - 1)
		{
			sysData.file.nowPageIndex = pageIndex;
		} else
		{
			sysData.file.nowPageIndex = 0;
		}

		if (sysData.file.nowPageIndex == sysData.file.pages - 1)
		{
			sysData.file.nowPageCount = sysData.file.total - sysData.file.nowPageIndex * sysData.file.perPage;
		}

		if (selIndex <= sysData.file.nowPageCount - 1)
		{
			sysData.file.nowSelIndex = selIndex;
		} else if (sysData.file.nowPageCount > 0)
		{
			sysData.file.nowSelIndex = sysData.file.nowPageCount - 1;
		} else
		{
			sysData.file.nowSelIndex = 0;
		}

		return __SCPI_SUCCESS;
	} else
	{
		return __SCPI_FAILED;
	}
}

//删除本页文件
int tSysScpi::setFileOfDeleteThePage(void)
{
	if (sysData.file.pages > 0 && sysData.file.nowPageIndex >= 0 && sysData.file.nowPageCount >= 0)
	{
		int pageIndex = sysData.file.nowPageIndex;
		int selIndex = sysData.file.nowSelIndex;
		int absIndex = 0;
		QString fileName = "";

		for (int i = 0; i < sysData.file.nowPageCount; i++)
		{
			absIndex = sysData.file.nowPageIndex * sysData.file.perPage + i;
			fileName = fileList.at(absIndex).absoluteFilePath();

			if (fileList.at(absIndex).suffix().toUpper() == "CSV")
			{
				QString csvFileName = fileList.at(absIndex).absolutePath() + "/trace/" + fileList.at(absIndex).baseName() + ".png";

				if (QFile(csvFileName).exists())
				{
					csvFileName = "rm " + csvFileName;
					system(csvFileName.toStdString().c_str());
				}
			}

			if (QFile(fileName).exists())
			{
				fileName = "rm " + fileName;
				system(fileName.toStdString().c_str());
			}
		}

		getSystemFileList();

		if (pageIndex <= sysData.file.pages - 1)
		{
			sysData.file.nowPageIndex = pageIndex;
		} else
		{
			sysData.file.nowPageIndex = 0;
		}

		if (sysData.file.nowPageIndex == sysData.file.pages - 1)
		{
			sysData.file.nowPageCount = sysData.file.total - sysData.file.nowPageIndex * sysData.file.perPage;
		}

		if (selIndex <= sysData.file.nowPageCount - 1)
		{
			sysData.file.nowSelIndex = selIndex;
		} else if (sysData.file.nowPageCount > 0)
		{
			sysData.file.nowSelIndex = sysData.file.nowPageCount - 1;
		} else
		{
			sysData.file.nowSelIndex = 0;
		}

		return __SCPI_SUCCESS;
	} else
	{
		return __SCPI_FAILED;
	}
}

//删除所有文件
int tSysScpi::setFileOfDeleteAll(void)
{
	if (sysData.file.pages > 0)
	{
		QString dirString = "/home/sky/save";
		QString cmdString = "rm -r " + dirString + "/*";
		int pageIndex = sysData.file.nowPageIndex;
		int selIndex = sysData.file.nowSelIndex;

		if (QDir(dirString).exists() && QDir(dirString).count() > 0)
		{
			system(cmdString.toStdString().c_str());
		}

		//    dirString += "/trace";
		//
		//    if (QDir(dirString).exists() && QDir(dirString).count() > 0)
		//    {
		//      cmdString = "rm -r " + dirString + "/*";
		//      system(cmdString.toStdString().c_str());
		//    }

		getSystemFileList();

		if (pageIndex <= sysData.file.pages - 1)
		{
			sysData.file.nowPageIndex = pageIndex;
		} else
		{
			sysData.file.nowPageIndex = 0;
		}

		if (sysData.file.nowPageIndex == sysData.file.pages - 1)
		{
			sysData.file.nowPageCount = sysData.file.total - sysData.file.nowPageIndex * sysData.file.perPage;
		}

		if (selIndex <= sysData.file.nowPageCount - 1)
		{
			sysData.file.nowSelIndex = selIndex;
		} else if (sysData.file.nowPageCount > 0)
		{
			sysData.file.nowSelIndex = sysData.file.nowPageCount - 1;
		} else
		{
			sysData.file.nowSelIndex = 0;
		}
	}

	return __SCPI_SUCCESS;
}

//USB设备连接
bool tSysScpi::isUsbDeviceConnect(void)
{
	QString dirName = "/sys/bus/usb/devices/1-1:1.0";

	return QDir(dirName).exists();
}

//导出选择文件
int tSysScpi::setFileOfExportTheSelect(void)
{
	if (sysData.file.pages > 0 && sysData.file.nowPageIndex >= 0 && sysData.file.nowPageCount > 0 && sysData.file.nowSelIndex >= 0 && sysData.file.nowSelIndex < sysData.file.nowPageCount)
	{
		outputToScreen(getTheTranslatorString("Saving file to usb disk") + "......", "");

		if (isUsbDeviceConnect() && checkUsbDirectory() == _RESULT_SUCCESS)
		{
			int absIndex = getFileOfNowSelectIndex();
			QString srcFileName = fileList.at(absIndex).absoluteFilePath();
			QString objFileName = QString(usbDirName) + "/" + fileList.at(absIndex).baseName() + "." + fileList.at(absIndex).suffix();
			QString cmdString = "cp " + srcFileName + " " + objFileName;

			if (system(cmdString.toStdString().c_str()) != 0)
			{
				//  disConnectFromUsbDisk();
				return __SCPI_FAILED;
			}
			sleep(3);
			return __SCPI_SUCCESS;
		} else
		{
			outputToScreen(getTheTranslatorString("file export failed"));
			usleep(FORMCLOSETIME);
		}
	}

	return __SCPI_FAILED;
}

//导出本页文件
int tSysScpi::setFileOfExportThePage(void)
{
	if (sysData.file.pages > 0 && sysData.file.nowPageIndex >= 0 && sysData.file.nowPageCount >= 0)
	{
		outputToScreen(getTheTranslatorString("Saving file to usb disk") + "......", "");

		if (isUsbDeviceConnect() && checkUsbDirectory() == _RESULT_SUCCESS)
		{
			int absIndex = 0;
			QString srcFileName = "";
			QString objFileName = "";
			QString cmdString = "";
			for (int i = 0; i < sysData.file.nowPageCount; i++)
			{
				absIndex = sysData.file.nowPageIndex * sysData.file.perPage + i;
				srcFileName = fileList.at(absIndex).absoluteFilePath();
				objFileName = QString(usbDirName) + "/" + fileList.at(absIndex).baseName() + "." + fileList.at(absIndex).suffix();
				cmdString = "cp " + srcFileName + " " + objFileName;

				if (system(cmdString.toStdString().c_str()) != 0)
				{
					//  disConnectFromUsbDisk();
					return __SCPI_FAILED;
				}
			}
			//  disConnectFromUsbDisk();
			sleep(3);
			return __SCPI_SUCCESS;
		} else
		{
			outputToScreen(getTheTranslatorString("file export failed"));
			usleep(FORMCLOSETIME);
		}
	}

	return __SCPI_FAILED;
}

//导出全部文件
int tSysScpi::setFileOfExportAll(void)
{
	if (sysData.file.pages > 0)
	{
		outputToScreen(getTheTranslatorString("Saving file to usb disk") + "......", "");

		if (isUsbDeviceConnect() && checkUsbDirectory() == _RESULT_SUCCESS)
		{
			QString cmdString = "cp -r /home/sky/save/* " + QString(usbDirName) + "/";
			if (system(cmdString.toStdString().c_str()) != 0)
			{
				//  disConnectFromUsbDisk();
				return __SCPI_FAILED;
			}
			sleep(3);
			return __SCPI_SUCCESS;
		} else
		{
			outputToScreen(getTheTranslatorString("file export failed"));
			usleep(FORMCLOSETIME);
		}
	}

	return __SCPI_FAILED;
}

//设置跟踪源状态
int tSysScpi::setSourceTrackGenState(int value)
{
	if (rfHandle < 0)
	{
		return __SCPI_FAILED;
	}
	sysData.source.attentionValue = 0 - sysData.source.trackGenOutputPower;

	unsigned int trackAttentionDown = 31 - (int) (sysData.source.attentionValue);
	unsigned char trackAttentionDown1;
	unsigned char trackAttentionDown2;
	unsigned char trackAttentionDown3;
	unsigned char trackAttentionDown4;
	unsigned char trackAttentionDown5;

	closeSourceSignalGenerator();

	if (rfHandle >= 0)
	{
		sysData.source.isTrackGenOn = (value == 1);
		if (!sysData.source.isTrackGenOn && sysData.source.isTrackGenNetworkOn)
		{
			sysData.source.isTrackGenNetworkOn = false;
			setSourceTrackGenNetworkState(sysData.source.isTrackGenNetworkOn);
		}

		if ((sysData.source.isTrackGenOn && sysData.options.isTrack36GHzOn) || sysData.source.isSignalGenOn)
		{
			sysData.source.isSourceOn = true;
		} else
		{
			sysData.source.isSourceOn = false;
		}

		//	if(sysData.source.isTrackGenOn)
		//	{
		//	  sysData.freq.prvStart = sysData.freq.start;
		//	  sysData.freq.prvStop = sysData.freq.stop;
		//	}else
		//	{
		//	  sysData.freq.start = sysData.freq.prvStart;
		//	  sysData.freq.stop = sysData.freq.prvStop;
		//	}
		setFrequencyOfStart(sysData.freq.start);
		setFrequencyOfStop(sysData.freq.stop);

		if (sysData.source.isSourceOn)
		{
			//总源开关及幅度控制
			setSourceAttention(sysData.source.attentionValue);
			usleep(10 * 1000);

			//模块跟踪源选择
			rfDownload(0xfe, 0 | (sysData.source.isTrackGenOn << 5) | (sysData.freq.refInternal << 6)); //自带跟踪源电源控制
			rfDownload(0x44, 0x00); //跟踪源开关控制

			rfDownload(0xfb, 0x00); //跟踪源控制送数
		} else
		{
			rfDownload(0xfc, 0x00 << 6); //源模块关闭

			//跟踪源衰减、跟踪源开关、内外参考
			if (sysData.source.isTrackGenOn && !sysData.options.isTrack36GHzOn)
			{
				//跟踪源
				trackAttentionDown1 = trackAttentionDown & 0x01;
				trackAttentionDown2 = trackAttentionDown >> 1 & 0x01;
				trackAttentionDown3 = trackAttentionDown >> 2 & 0x01;
				trackAttentionDown4 = trackAttentionDown >> 3 & 0x01;
				trackAttentionDown5 = trackAttentionDown >> 4 & 0x01;

				rfDownload(0xfe, (trackAttentionDown1 << 4 | trackAttentionDown2 << 3 | trackAttentionDown3 << 2 | trackAttentionDown4 << 1 | trackAttentionDown5) | (sysData.source.isTrackGenOn << 5) | (sysData.freq.refInternal << 6));

				rfDownload(0xff, 0x00); //跟踪源送数

			} else
			{
				rfDownload(0xfe, 0 | (sysData.source.isTrackGenOn << 5) | (sysData.freq.refInternal << 6));
			}
		}
		return __SCPI_SUCCESS;
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置跟踪源状态
int tSysScpi::setSourceTrackGenState(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	int tempValue = getBoolValue(value.toUpper().trimmed(), &isOk);

	if (isOk)
	{
		return setSourceTrackGenState(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置网络测量类型
int tSysScpi::setSourceTrackGenNetworkMeasType(networkMeasType value)
{
	if (value != sysData.source.measType)
	{
		sysData.source.measType = networkMeasType(value);
		switch (sysData.source.measType)
		{
			case S21:
				if (sysData.markerTableOn)
				{
					sysData.markerTableOn = false;
					checkMultiDisplay(mdtSpectrum);
					sysData.mode = smSpectrum;
					sysData.canvasNums = 1;
				}
				break;
			case S11:
				sysData.ampt.scaleLine = false;
				sysData.ampt.refUnit = rudBm;
				setFrequencyOfStart(10e6);
				setFrequencyOfStop(100e6);
				setBwOfRbw(30e3);
				setAmptOfRefLevel(0);
				setAmptOfAttAuto(1);
				setSweepOfTimeAuto(1);
				if (sysData.detector.detMode != dmPosPeak)
				{
					sysData.detector.detMode = dmPosPeak;
					controlRf();
				}

				if (!sysData.markerTableOn)
				{
					sysData.markerTableOn = true;
					checkMultiDisplay(mdtMkrTable);
					sysData.mode = smMarkerTable;
					sysData.canvasNums = 2;
				}
				break;
		}
	}
}

//设置网络测量类型
int tSysScpi::setSourceTrackGenNetworkMeasType(QString value)
{
	if (value.isNull() || value.isEmpty() || sysData.ampt.scaleLine)
	{
		return __SCPI_FAILED;
	}

	QString tempValue = value.toUpper().trimmed();

	if (tempValue == "S21")
	{
		return setSourceTrackGenNetworkMeasType(S21);
	} else if (tempValue == "S11")
	{
		return setSourceTrackGenNetworkMeasType(S11);
	}

	return __SCPI_FAILED;
}

//设置跟踪源输出功率
int tSysScpi::setSourceTrackGenOutputPower(double value)
{
	double minValue = 0;
	double maxValue = 0;
	double tempValue = value;
	double temp = 0;

	getTheAmptMaxMinLimit(MINTRACKGENPOWER, MAXTRACKGENPOWER, &minValue, &maxValue);

	if (tempValue < minValue)
	{
		tempValue = minValue;
	} else if (tempValue > maxValue)
	{
		tempValue = maxValue;
	}

	if (sysData.options.isTrack36GHzOn && sysData.source.isTrackGenOn)
	{
		if (tempValue * 2 - (int) (tempValue * 2.0) > 0.5)
		{
			temp = ((int) (tempValue * 2.0) + 1) * 1.0 / 2.0;
		} else
		{
			temp = (int) (tempValue * 2.0) * 1.0 / 2.0;
		}
	} else if (!sysData.options.isTrack36GHzOn && sysData.source.isTrackGenOn)
	{
		if (tempValue - (int) (tempValue) > 0.5)
		{
			temp = (int) (tempValue) + 1;
		} else
		{
			temp = (int) (tempValue);
		}
	}
	sysData.source.trackGenOutputPower = temp;

	sysData.source.attentionValue = 0 - sysData.source.trackGenOutputPower;

	setSourceTrackGenState(sysData.source.isTrackGenOn);

	return __SCPI_SUCCESS;
}

//设置跟踪源输出功率
int tSysScpi::setSourceTrackGenOutputPower(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	double tempValue = getDoubleValue(value, &isOk);

	if (isOk)
	{
		return setSourceTrackGenOutputPower(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//方向键设置跟踪源输出功率
int tSysScpi::setSourceTrackGenOutputPowerWithArrow(bool up)
{
	if (up)
	{
		return setSourceTrackGenOutputPower(sysData.source.trackGenOutputPower + 10.0);
	} else
	{
		return setSourceTrackGenOutputPower(sysData.source.trackGenOutputPower - 10.0);
	}
}

//脉冲旋钮设置跟踪源输出功率
int tSysScpi::setSourceTrackGenOutputPowerWithRpg(int value)
{
	return setSourceTrackGenOutputPower(sysData.source.trackGenOutputPower + value);
}

//设置跟踪源网络测量状态
int tSysScpi::setSourceTrackGenNetworkState(int state)
{
	sysData.source.isTrackGenNetworkOn = state == 1;

	//  sysData.source.isTrackGenOn = sysData.source.isTrackGenNetworkOn;
	//  setSourceTrackGenState(sysData.source.isTrackGenOn);

	if (sysData.source.isTrackGenNetworkOn)
	{
		//	//记忆先前的扫描时间、扫描点数、检波方式
		//	sysData.sweep.prvSweepTime = sysData.sweep.sweepTime;
		//	sysData.sweep.prvSweepTimeAuto = sysData.sweep.sweepTimeAuto;
		//	sysData.sweep.sweepPrevPoints = sysData.sweep.sweepPoints;
		//	sysData.detector.prvDetMode = sysData.detector.detMode;
		//    sysData.bw.prvRbw = sysData.bw.rbw;
		//    sysData.bw.prvRbwAuto = sysData.bw.rbwAuto;

		sysData.source.isTrackGenOn = sysData.source.isTrackGenNetworkOn;
		setSourceTrackGenState(sysData.source.isTrackGenOn);

		sysData.ampt.scaleLine = false;
		sysData.ampt.refUnit = rudBm;

		//频率<100K时 分辨带宽设为1K
		if (sysData.freq.start < 100e3)
		{
			setBwOfRbw(1e3);
		} else
		{
			setBwOfRbw(30e3);
		}

		if (sysData.detector.detMode != dmPosPeak)
		{
			sysData.detector.detMode = dmPosPeak;
			controlRf();
		}
	} else
	{
		//	//恢复先前的扫描时间、扫描点数、检波方式
		//	sysData.sweep.sweepTime = sysData.sweep.prvSweepTime;
		//	sysData.sweep.sweepTimeAuto = sysData.sweep.prvSweepTimeAuto;
		//	sysData.sweep.sweepPoints = sysData.sweep.sweepPrevPoints;
		//	sysData.detector.detMode = sysData.detector.prvDetMode;
		//    sysData.bw.rbw = sysData.bw.prvRbw;
		//    sysData.bw.rbwAuto = sysData.bw.prvRbwAuto;
		//
		//	controlRf();
	}

	return __SCPI_SUCCESS;
}

//设置跟踪源网络测量状态
int tSysScpi::setSourceTrackGenNetworkState(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	int tempValue = getBoolValue(value.toUpper().trimmed(), &isOk);

	if (isOk)
	{
		return setSourceTrackGenNetworkState(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//跟踪源网络测量数据归一化
int tSysScpi::setSourceTrackGenNetworkNormalize(void)
{
	if (sysData.isCalibrating)
	{
		return __SCPI_FAILED;
	}

	int progress = 0;
	char tempChar[32] = { };
	QString datetime = "";
	double startFreq = sysData.freq.start;
	double stopFreq = sysData.freq.stop;
	double rbw = sysData.bw.rbw;
	bool rbwAuto = sysData.bw.rbwAuto;
	double refLevel = sysData.source.trackGenNetworkRefLevel;
	double outputPower = sysData.source.trackGenNetworkOutputPower;
	int sweepPoints = sysData.sweep.sweepPoints;
	networkMeasType mt = sysData.source.measType;
	struct markerDef mkr[MARKERCOUNT];
	for (int i = 0; i < MARKERCOUNT; i++)
		mkr[i] = sysData.marker[i];

	//first
	progress = 0;
	outputToScreen(getTheTranslatorString("Network Measure Normalizing") + "......", progress, 0);

	getLocalDateTime(tempChar);
	datetime = tempChar;

	sysData.isCalibrating = true;
	sysData.mode = smCalibrate;
	sysData.canvasNums = 1;
	QCoreApplication::processEvents();
	saveScreen();
	presetSystemData();
	setSourceTrackGenNetworkMeasType(mt);
	sysData.source.trackGenNetworkRefLevel = refLevel;
	controlRf();
	memset(&sysData.source.trackAmptStandard, 0, sizeof sysData.source.trackNetworkStandard);
	sysData.source.trackNetworkStandard.startFreq = startFreq;
	sysData.source.trackNetworkStandard.stopFreq = stopFreq;
	sysData.source.trackNetworkStandard.sweepPoints = sweepPoints;
	sysData.detector.detMode = dmPosPeak;
	setSourceTrackGenNetworkState(true);
	// 频率100KHz以下分辨带宽设置为1KHz
	if (rbwAuto)
	{
		if (startFreq < 100e3)
		{
			setBwOfRbw(1e3);
		} else
			setBwOfRbwAuto(true);
	} else
	{
		setBwOfRbw(rbw);
	}
	setFrequencyOfStart(sysData.source.trackNetworkStandard.startFreq);
	setFrequencyOfStop(sysData.source.trackNetworkStandard.stopFreq);
	//2015.07.03 频谱仪质量提升中发现的关于扫描点数的问题
	setSweepOfPoints(sweepPoints);
	for (int i = 0; i < MARKERCOUNT; i++)
		sysData.marker[i] = mkr[i];
	controlRf();
	sysData.source.trackGenOutputPower = outputPower;
	sysData.source.trackNetworkStandard.outputPower = outputPower;
	controlRf();
	usleep(sysData.sweep.sweepTime / 1e3 + 100 * 1000);
	getDataFromIF();
	progress = 10;
	outputToScreen(getTheTranslatorString("Network Measure Normalizing") + "......", progress, 0);

	double topValue = sysData.source.trackGenNetworkRefLevel + 5 * sysData.ampt.scaleDiv;

	for (int i = 0; i < sysData.sweep.sweepPoints; i++)
	{
		progress = 10 + 90 * i / sysData.sweep.sweepPoints;
		outputToScreen(getTheTranslatorString("Network Measure Normalizing") + "......", progress, 0);
		sysData.source.trackNetworkStandard.value[i] = 0 - sysData.trace[0].value[i];
	}

	saveSourceConfigOfNetworkMeasure();
	loadSourceConfig();
	sysData.isCalibrating = false;
	sysData.mode = smSpectrum;
	sysData.canvasNums = 1;
	if (sysData.source.isTrackGenNetworkOn && sysData.source.measType == S11)
	{
		setMarkerTable(1);
	}
	//last
	sysData.source.trackGenNetworkOutputPower = outputPower;
	progress = 100;
	outputToScreen(getTheTranslatorString("Network Measure Normalize Successed"), progress, 0);

	return __SCPI_SUCCESS;
}

//设置跟踪源网络测量参考电平
int tSysScpi::setSourceTrackGenNetworkReflevel(double value)
{
	double tempValue = value;

	//新版参考设置
	if (tempValue > MAXNETWORKREF)
	{
		tempValue = MAXNETWORKREF;
	} else if (tempValue < MINNETWORKREF)
	{
		tempValue = MINNETWORKREF;
	}

	/*
	 //旧版参考设置
	 if (tempValue < MINREFLEVELDBM)
	 {
	 tempValue = MINREFLEVELDBM;
	 } else if (tempValue > MAXREFLEVELDBM)
	 {
	 tempValue = MAXREFLEVELDBM;
	 }
	 */

	sysData.source.trackGenNetworkRefLevel = tempValue;
	//setAmptOfRefLevel(sysData.source.trackGenNetworkRefLevel);
	controlRf();

	return __SCPI_SUCCESS;
}

//设置跟踪源网络测量参考电平
int tSysScpi::setSourceTrackGenNetworkReflevel(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	double tempValue = getDoubleValue(value, &isOk);

	if (isOk)
	{
		return setSourceTrackGenNetworkReflevel(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//方向键设置跟踪源网络测量参考电平
int tSysScpi::setSourceTrackGenNetworkReflevelWithArrow(bool up)
{
	if (up)
	{
		return setSourceTrackGenNetworkReflevel(sysData.source.trackGenNetworkRefLevel + 10.0);
	} else
	{
		return setSourceTrackGenNetworkReflevel(sysData.source.trackGenNetworkRefLevel - 10.0);
	}
}

//脉冲旋钮设置跟踪源网络测量参考电平
int tSysScpi::setSourceTrackGenNetworkReflevelWithRpg(int value)
{
	return setSourceTrackGenNetworkReflevel(sysData.source.trackGenNetworkRefLevel + value);
}

//设置跟踪源网络测量输出功率
int tSysScpi::setSourceTrackGenNetworkOutputPower(double value)
{
	double minValue = 0;
	double maxValue = 0;
	double tempValue = value;
	double temp = 0;

	getTheAmptMaxMinLimit(MINTRACKGENPOWER, MAXTRACKGENPOWER, &minValue, &maxValue);

	if (tempValue < minValue)
	{
		tempValue = minValue;
	} else if (tempValue > maxValue)
	{
		tempValue = maxValue;
	}

	if (sysData.options.isTrack36GHzOn && sysData.source.isTrackGenOn)
	{
		if (tempValue * 2 - (int) (tempValue * 2.0) > 0.5)
		{
			temp = ((int) (tempValue * 2.0) + 1) * 1.0 / 2.0;
		} else
		{
			temp = (int) (tempValue * 2.0) * 1.0 / 2.0;
		}
	} else if (!sysData.options.isTrack36GHzOn && sysData.source.isTrackGenOn)
	{
		if (tempValue - (int) (tempValue) > 0.5)
		{
			temp = (int) (tempValue) + 1;
		} else
		{
			temp = (int) (tempValue);
		}
	}

	sysData.source.trackGenNetworkOutputPower = temp;
	setSourceTrackGenOutputPower(sysData.source.trackGenNetworkOutputPower);

	return __SCPI_SUCCESS;
}

//设置跟踪源网络测量输出功率
int tSysScpi::setSourceTrackGenNetworkOutputPower(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	double tempValue = getDoubleValue(value, &isOk);

	if (isOk)
	{
		return setSourceTrackGenNetworkOutputPower(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//方向键设置跟踪源网络测量输出功率
int tSysScpi::setSourceTrackGenNetworkOutputPowerWithArrow(bool up)
{
	if (up)
	{
		return setSourceTrackGenNetworkOutputPower(sysData.source.trackGenNetworkOutputPower + 10.0);
	} else
	{
		return setSourceTrackGenNetworkOutputPower(sysData.source.trackGenNetworkOutputPower - 10.0);
	}
}

//脉冲旋钮设置跟踪源网络测量输出功率
int tSysScpi::setSourceTrackGenNetworkOutputPowerWithRpg(int value)
{
	return setSourceTrackGenNetworkOutputPower(sysData.source.trackGenNetworkOutputPower + value);
}

//设置跟踪源校准参数
int tSysScpi::setSourceTrackCalibrateParam(void)
{
	presetSystemData();
	sysData.detector.detMode = dmPosPeak;
	setAmptOfRefLevel(10);
	setMarkerNormal(0, 0);
	sysData.isAutoPeakMaxing = true;
	controlRf();

	sysData.source.trackAmptStandardIndex = 0;
	setFrequencyOfCenter(sysData.source.trackAmptStandard[sysData.source.trackAmptStandardIndex].centerFreq);
	setFrequencyOfSpan(sysData.source.trackAmptStandard[sysData.source.trackAmptStandardIndex].span);
	sysData.freq.isShowCenter = false;
	controlRf();

	sysData.source.isTrackGenOn = true;
	sysData.source.trackGenOutputPower = 0;
	setSourceTrackGenState(sysData.source.isTrackGenOn);

	for (int i = 0; i < sizeof sysData.source.trackAmptStandard / sizeof sysData.source.trackAmptStandard[0]; i++)
	{
		setSourceTrackGenPll(i);
	}
}

//设置电压驻波比状态
int tSysScpi::setSourceTrackGenVSWRState(int state)
{
	sysData.source.isVswrOn = state == 1;

	if (sysData.source.isVswrOn)
	{
		sysData.source.isTrackGenOn = sysData.source.isVswrOn;
		setSourceTrackGenState(sysData.source.isVswrOn);
		sysData.ampt.scaleLine = false;
		sysData.ampt.refUnit = rudBm;

		setFrequencyOfStart(10e6);
		setFrequencyOfStop(100e6);
		setBwOfRbw(30e3);
		setAmptOfRefLevel(0);
		setAmptOfAttAuto(1);
		setSweepOfTimeAuto(1);
		if (sysData.detector.detMode != dmPosPeak)
		{
			sysData.detector.detMode = dmPosPeak;
			controlRf();
		}
	} else
	{
		closeSourceGenerator();
	}
}

//设置电压驻波比状态
int tSysScpi::setSourceTrackGenVSWRState(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	int tempValue = getBoolValue(value.toUpper().trimmed(), &isOk);

	if (isOk)
	{
		return setSourceTrackGenVSWRState(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置电压驻波比参考
int tSysScpi::setSourceTrackGenVSWRRefLevel(double value)
{

}

//设置电压驻波比参考
int tSysScpi::setSourceTrackGenVSWRRefLevel(QString value)
{

}

//方向键设置电压驻波比参考
int tSysScpi::setSourceTrackGenVSWRRefLevelWithArrow(bool up)
{

}

//脉冲旋钮设置电压驻波比参考
int tSysScpi::setSourceTrackGenVSWRRefLevelWithRpg(int value)
{

}

//设置信号源校准参数
int tSysScpi::setSourceSignalCalibrateParam()
{
	presetSystemData();
	sysData.detector.detMode = dmPosPeak;
	setAmptOfRefLevel(10);
	setMarkerNormal(0, 0);
	sysData.isAutoPeakMaxing = true;
	controlRf();

	sysData.source.signalAmptStandardIndex = 0;
	setFrequencyOfCenter(sysData.source.signalAmptStandard[sysData.source.signalAmptStandardIndex].baseFreq);
	setFrequencyOfSpan(1e6);
	sysData.freq.isShowCenter = false;
	controlRf();

	sysData.source.isSignalGenOn = true;
	sysData.source.signalGenOutputFreq = sysData.source.signalAmptStandard[sysData.source.signalAmptStandardIndex].baseFreq;
	sysData.source.signalGenOutputPower = 0;

	setSourceSignalGenState(sysData.source.isSignalGenOn, false);
}

//设置源开关和衰减器
int tSysScpi::setSourceAttention(double value)
{
	double temp = value * 2.0;
	unsigned int sourceAtt;
	unsigned int sourceAttDown;
	unsigned char sourceAttDown1;
	unsigned char sourceAttDown2;
	unsigned char sourceAttDown3;
	unsigned char sourceAttDown4;
	unsigned char sourceAttDown5;
	unsigned char sourceAttDown6;

	if (temp - (int) temp >= 0.5)
	{
		sourceAtt = (int) temp + 1;
	} else
	{
		sourceAtt = (int) temp;
	}

	sourceAttDown = (unsigned int) (63 - sourceAtt); // 跟踪源衰减器参数计算
	sourceAttDown1 = sourceAttDown & 0x01;
	sourceAttDown2 = (sourceAttDown >> 1) & 0x01;
	sourceAttDown3 = (sourceAttDown >> 2) & 0x01;
	sourceAttDown4 = (sourceAttDown >> 3) & 0x01;
	sourceAttDown5 = (sourceAttDown >> 4) & 0x01;
	sourceAttDown6 = (sourceAttDown >> 5) & 0x01;

	//源衰减器参数下发、源开启控制
	rfDownload(0xfc, 0x03 << 6 | sourceAttDown1 << 5 | sourceAttDown2 << 4 | sourceAttDown3 << 3 | sourceAttDown4 << 2 | sourceAttDown5 << 1 | sourceAttDown6);

	return __SCPI_SUCCESS;
}

//设置信号源状态
int tSysScpi::setSourceSignalGenState(int value, bool reset)
{
	closeSourceTrackGenerator();

	if (sysData.source.isTrackGenNetworkOn)
	{
		sysData.source.isTrackGenNetworkOn = false;
	}

	sysData.source.isSignalGenOn = value == 1;

	if ((sysData.source.isTrackGenOn && sysData.options.isTrack36GHzOn) || sysData.source.isSignalGenOn)
	{
		sysData.source.isSourceOn = true;
	} else
	{
		sysData.source.isSourceOn = false;
	}

	//信号源开关
	if (sysData.source.isSourceOn)
	{
		//信号源功率、源总开关送数
		sysData.source.attentionValue = 0 - sysData.source.signalGenOutputPower;
		setSourceAttention(sysData.source.attentionValue);

		//信号源频率
		setSourceSignalGenFreq(reset);
	} else
	{
		rfDownload(0xfc, 0x00 << 6); //源模块关闭
	}

	return __SCPI_SUCCESS;
}

//设置信号源状态
int tSysScpi::setSourceSignalGenState(QString value, bool reset)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	int tempValue = getBoolValue(value.toUpper().trimmed(), &isOk);

	if (isOk)
	{
		return setSourceSignalGenState(tempValue, reset);
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置信号源输出频率
int tSysScpi::setSourceSignalGenOutputFreq(double value)
{
	double tempValue = value;

	if (tempValue < 150e6)
	{
		tempValue = 150e6;
	} else if (tempValue > MAXFREQ)
	{
		tempValue = MAXFREQ;
	}

	sysData.source.signalGenOutputFreq = tempValue;
	setSourceSignalGenFreq(false);

	return __SCPI_SUCCESS;
}

//设置信号源输出频率
int tSysScpi::setSourceSignalGenOutputFreq(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	double tempValue = getFreqValue(value, &isOk);

	if (isOk)
	{
		return setSourceSignalGenOutputFreq(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//方向键设置信号源输出频率
int tSysScpi::setSourceSignalGenOutputFreqWithArrow(bool up)
{
	if (up)
	{
		return setSourceSignalGenOutputFreq(sysData.source.signalGenOutputFreq + sysData.freq.cfStep);
	} else
	{
		return setSourceSignalGenOutputFreq(sysData.source.signalGenOutputFreq - sysData.freq.cfStep);
	}
}

//脉冲旋钮设置信号源输出频率
int tSysScpi::setSourceSignalGenOutputFreqWithRpg(int value)
{
	return setSourceSignalGenOutputFreq(sysData.source.signalGenOutputFreq + value * 0.01 * sysData.freq.cfStep);
}

//设置信号源输出功率
int tSysScpi::setSourceSignalGenOutputPower(double value)
{
	double minValue = 0;
	double maxValue = 0;
	double tempValue = value;
	double temp = 0;

	getTheAmptMaxMinLimit(MINTRACKGENPOWER, MAXTRACKGENPOWER, &minValue, &maxValue);

	if (tempValue < minValue)
	{
		tempValue = minValue;
	} else if (tempValue > maxValue)
	{
		tempValue = maxValue;
	}

	//  int intValue = (int)tempValue;
	if (tempValue * 2 - (int) (tempValue * 2.0) > 0.5)
	{
		temp = ((int) (tempValue * 2.0) + 1) * 1.0 / 2.0;
	} else
	{
		temp = (int) (tempValue * 2.0) * 1.0 / 2.0;
	}
	sysData.source.signalGenOutputPower = temp;

	setSourceSignalGenState(sysData.source.isSignalGenOn, false);

	return __SCPI_SUCCESS;
}

//设置信号源输出功率
int tSysScpi::setSourceSignalGenOutputPower(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	double tempValue = getDoubleValue(value, &isOk);

	if (isOk)
	{
		return setSourceSignalGenOutputPower(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//方向键设置信号源输出频率
int tSysScpi::setSourceSignalGenOutputPowerWithArrow(bool up)
{
	if (up)
	{
		return setSourceSignalGenOutputPower(sysData.source.signalGenOutputPower + 10.0);
	} else
	{
		return setSourceSignalGenOutputPower(sysData.source.signalGenOutputPower - 10.0);
	}
}

//脉冲旋钮设置信号源输出频率
int tSysScpi::setSourceSignalGenOutputPowerWithRpg(int value)
{
	return setSourceSignalGenOutputPower(sysData.source.signalGenOutputPower + value);
}

//设置解调状态
int tSysScpi::setDemodOfSoundState(int value)
{
	sysData.demod.demodOn = value == 1;
	printf("demod on\n");
	if (sysData.demod.demodOn)
	{
		setDemodOfMode(sysData.demod.mode);

		if (sysData.demod.mode == dmFMW)
		{
			setDemodModeOfFreq(sysData.demod.radioSet);
		}
	} else
	{
		controlRf();
	}
	return __SCPI_SUCCESS;
}

//设置解调状态
int tSysScpi::setDemodOfSoundState(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	int tempValue = getBoolValue(value.toUpper().trimmed(), &isOk);

	if (isOk)
	{
		return setDemodOfSoundState(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}
//设置阻抗匹配状态
int tSysScpi::setImpedanceOfState(int value)
{
	sysData.isImpedanceTransformation = value == 1;

	if (sysData.isImpedanceTransformation == 0)
	{
		//sysData.ampt.LMPValue = 0;
	}
	return __SCPI_SUCCESS;
}

//设置阻抗匹配状态
int tSysScpi::setImpedanceOfState(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	int tempValue = getBoolValue(value.toUpper().trimmed(), &isOk);

	if (isOk)
	{
		return setImpedanceOfState(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置解调模式
int tSysScpi::setDemodOfMode(int value)
{
	int tempValue = value;
	unsigned char numberOfMode = 0;
	double tempFTWOfDdc = 0;
	unsigned long long FTWOfDdc = 0;
	unsigned int FTWOfDdchh = 0;
	unsigned int FTWOfDdchl = 0;
	unsigned int FTWOfDdclh = 0;
	unsigned int FTWOfDdcll = 0;

	if (tempValue <= 0)
	{
		tempValue = 0;
	} else if (tempValue >= 4)
	{
		tempValue = 4;
	}

	if (!sysData.demod.demodOn)
	{
		return __SCPI_FAILED;
	}

	//if (tempValue == 0 || tempValue == 1)
	//{
	//	tempFTWOfDdc = (8.66 / 30.0) * 4294967296.0;
	//} else
	//{
	//	tempFTWOfDdc = (8.6 / 30.0) * 4294967296.0;
	//}

	//DDC NCO 下发控制字
	//FTWOfDdc = (unsigned long long) tempFTWOfDdc;
	//FTWOfDdchh = (FTWOfDdc >> 24) & 0xff;
	//FTWOfDdchl = (FTWOfDdc >> 16) & 0xff;
	//FTWOfDdclh = (FTWOfDdc >> 8) & 0xff;
	//FTWOfDdcll = (FTWOfDdc) & 0xff;

	sysData.ampt.isPreamptOn = true;
	sysData.ampt.preamtOn = 0x01;
	//sysData.ampt.preamtOn = 0x02;
	sysData.ampt.attAuto = true;
	sysData.bw.rbwStepCont = true;
	sysData.bw.rbwAuto = false;
	setAmptOfRefLevel(-20);
	//  sysData.ampt.att = 9;
	//  setAmptOfAtt(sysData.ampt.att);

	if (tempValue == 0)//FMW
	{
		numberOfMode = 0x00;
		sysData.bw.rbw = 200e3;
		sysData.ampt.ifValue = 0;
	} else if (tempValue == 1)//FM
	{
		numberOfMode = 0x00;
		sysData.bw.rbw = 100e3; //CIC
		sysData.ampt.ifValue = 0;
	} else if (tempValue == 2)//AM
	{
		numberOfMode = 0x01;
		sysData.bw.rbw = 40e3; //CIC
		sysData.ampt.ifValue = 10;
	} else if (tempValue == 3)//USB
	{
		numberOfMode = 0x02;
		sysData.bw.rbw = 20e3; //CIC
		sysData.ampt.ifValue = 10;
	} else if (tempValue == 4) //LSB
	{
		numberOfMode = 0x03;
		sysData.bw.rbw = 20e3; //CIC
		sysData.ampt.ifValue = 10;
	}

	//DDC NCO 下发控制字
	//ifDownload(0xfa, FTWOfDdchh);
	//ifDownload(0xfa, FTWOfDdchl);
	//ifDownload(0xfa, FTWOfDdclh);
	//ifDownload(0xfa, FTWOfDdcll);
	//ifDownload(0xf9, 0x00); //空操纵

	//CIC、增益下发
	controlRf();

	//ADDR(29)解调模式下发
	feDownload(50, numberOfMode&0x02);

	//ADD(30)音量下发
	//ifDownload(0x1e, (unsigned char) (sysData.demod.sound * 2.55));
	feDownload(51, sysData.demod.sound);//set vol
	feDownload(52, 1);
	usleep(10);
	feDownload(52, 0);//enable by high to low

	sysData.demod.mode = demodMode(tempValue);

	return __SCPI_SUCCESS;
}

//设置解调模式
int tSysScpi::setDemodOfMode(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}
	printf("demod mode is %s\n",value.toStdString().c_str());
	if (value == "FMW")
	{
		return setDemodOfMode(dmFMW);
	} else if (value == "FM")
	{
		return setDemodOfMode(dmFM);
	} else if (value == "AM")
	{
		return setDemodOfMode(dmAM);
	} else if (value == "USB")
	{
		return setDemodOfMode(dmUSB);
	} else if (value == "LSB")
	{
		return setDemodOfMode(dmLSB);
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置音量
int tSysScpi::setDemodOfSound(double value)
{
	double tempValue = value;

	if (tempValue < MINSOUND)
	{
		tempValue = MINSOUND;
	} else if (tempValue > MAXSOUND)
	{
		tempValue = MAXSOUND;
	}

	sysData.demod.sound = (int) tempValue;

	if (sysData.demod.demodOn)
	{
		feDownload(51, sysData.demod.sound);//set vol
		feDownload(52, 1);
		usleep(10);
		feDownload(52, 0);//enable by high to low
	}
	__var(sysData.demod.sound);

	return __SCPI_SUCCESS;
}

//设置音量
int tSysScpi::setDemodOfSound(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	int tempValue = getDoubleValue(value, &isOk);

	if (isOk)
	{
		return setDemodOfSound(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//方向键设置音量
int tSysScpi::setDemodOfSoundWithArrow(bool up)
{
	if (up)
	{
		return setDemodOfSound(sysData.demod.sound + 10.0);
	} else
	{
		return setDemodOfSound(sysData.demod.sound - 10.0);
	}
}

//脉冲旋钮设置音量
int tSysScpi::setDemodOfSoundWithRpg(int value)
{
	return setDemodOfSound(sysData.demod.sound + value);
}

//设置解调电台频率
int tSysScpi::setDemodModeOfFreq(int index)
{
	int tempValue = index;

	if (tempValue < 0)
	{
		tempValue = 0;
	} else if (tempValue > 5)
	{
		tempValue = 5;
	}

	sysData.demod.radioSet = demodRadio(tempValue);

	switch (sysData.demod.radioSet)
	{
		case radio1:
			setDemodOfTheFrequency(radio1, sysData.demod.radioFreqSet[0]);
			break;
		case radio2:
			setDemodOfTheFrequency(radio2, sysData.demod.radioFreqSet[1]);
			break;
		case radio3:
			setDemodOfTheFrequency(radio3, sysData.demod.radioFreqSet[2]);
			break;
		case radio4:
			setDemodOfTheFrequency(radio4, sysData.demod.radioFreqSet[3]);
			break;
		case radio5:
			setDemodOfTheFrequency(radio5, sysData.demod.radioFreqSet[4]);
			break;
		case radio6:
			setDemodOfTheFrequency(radio6, sysData.demod.radioFreqSet[5]);
			break;
		default:
			break;
	}

	return __SCPI_SUCCESS;
}

//设置解调电台频率
int tSysScpi::setDemodOfTheFrequency(int index, double freq)
{
	sysData.ampt.scaleLine = false;
	sysData.bw.rbwStepCont = true;
	sysData.bw.rbwAuto = false;
	setFrequencyOfCenter(freq);
	setFrequencyOfSpan(0);
	sysData.sweep.sweepSingle = false;
	sysData.sweep.sweepTimeAuto = true;
	sysData.freq.isShowCenter = true;
	controlRf();
	__var(freq);

	if (index == 0)
	{
		sysData.demod.radioFreqSet[0] = freq;
	} else if (index == 1)
	{
		sysData.demod.radioFreqSet[1] = freq;
	} else if (index == 2)
	{
		sysData.demod.radioFreqSet[2] = freq;
	} else if (index == 3)
	{
		sysData.demod.radioFreqSet[3] = freq;
	} else if (index == 4)
	{
		sysData.demod.radioFreqSet[4] = freq;
	} else if (index == 5)
	{
		sysData.demod.radioFreqSet[5] = freq;
	}

	saveDeviceConfig();

	setDemodOfMode(0);
	return __SCPI_SUCCESS;
}

//设置解调电台频率
int tSysScpi::setDemodOfTheFrequency(int index, QString value)
{
	if (value.isNull() || value.isEmpty() || index > 5 || index < 0)
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	int tempValue = getFreqValue(value, &isOk);

	if (isOk)
	{
		return setDemodOfTheFrequency(index, tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//方向键设置解调电台频率
int tSysScpi::setDemodOfTheFrequencyWithArrow(int index, bool up)
{
	if (up)
	{
		if (index == 0)
		{
			return setDemodOfTheFrequency(index, sysData.demod.radioFreqSet[0] + 1e3);
		} else if (index == 1)
		{
			return setDemodOfTheFrequency(index, sysData.demod.radioFreqSet[1] + 1e3);
		} else if (index == 2)
		{
			return setDemodOfTheFrequency(index, sysData.demod.radioFreqSet[2] + 1e3);
		} else if (index == 3)
		{
			return setDemodOfTheFrequency(index, sysData.demod.radioFreqSet[3] + 1e3);
		} else if (index == 4)
		{
			return setDemodOfTheFrequency(index, sysData.demod.radioFreqSet[4] + 1e3);
		} else if (index == 5)
		{
			return setDemodOfTheFrequency(index, sysData.demod.radioFreqSet[5] + 1e3);
		}
	} else
	{
		if (index == 0)
		{
			return setDemodOfTheFrequency(index, sysData.demod.radioFreqSet[0] - 1e3);
		} else if (index == 1)
		{
			return setDemodOfTheFrequency(index, sysData.demod.radioFreqSet[1] - 1e3);
		} else if (index == 2)
		{
			return setDemodOfTheFrequency(index, sysData.demod.radioFreqSet[2] - 1e3);
		} else if (index == 3)
		{
			return setDemodOfTheFrequency(index, sysData.demod.radioFreqSet[3] - 1e3);
		} else if (index == 4)
		{
			return setDemodOfTheFrequency(index, sysData.demod.radioFreqSet[4] - 1e3);
		} else if (index == 5)
		{
			return setDemodOfTheFrequency(index, sysData.demod.radioFreqSet[5] - 1e3);
		}
	}
}

//脉冲旋钮设置解调电台频率
int tSysScpi::setDemodOfTheFrequencyWithRpg(int index, int value)
{
	if (value > 0)
	{
		if (index == 0)
		{
			return setDemodOfTheFrequency(index, sysData.demod.radioFreqSet[0] + 1e3);
		} else if (index == 1)
		{
			return setDemodOfTheFrequency(index, sysData.demod.radioFreqSet[1] + 1e3);
		} else if (index == 2)
		{
			return setDemodOfTheFrequency(index, sysData.demod.radioFreqSet[2] + 1e3);
		} else if (index == 3)
		{
			return setDemodOfTheFrequency(index, sysData.demod.radioFreqSet[3] + 1e3);
		} else if (index == 4)
		{
			return setDemodOfTheFrequency(index, sysData.demod.radioFreqSet[4] + 1e3);
		} else if (index == 5)
		{
			return setDemodOfTheFrequency(index, sysData.demod.radioFreqSet[5] + 1e3);
		}
	} else
	{
		if (index == 0)
		{
			return setDemodOfTheFrequency(index, sysData.demod.radioFreqSet[0] - 1e3);
		} else if (index == 1)
		{
			return setDemodOfTheFrequency(index, sysData.demod.radioFreqSet[1] - 1e3);
		} else if (index == 2)
		{
			return setDemodOfTheFrequency(index, sysData.demod.radioFreqSet[2] - 1e3);
		} else if (index == 3)
		{
			return setDemodOfTheFrequency(index, sysData.demod.radioFreqSet[3] - 1e3);
		} else if (index == 4)
		{
			return setDemodOfTheFrequency(index, sysData.demod.radioFreqSet[4] - 1e3);
		} else if (index == 5)
		{
			return setDemodOfTheFrequency(index, sysData.demod.radioFreqSet[5] - 1e3);
		}
	}
}

//设置AM解调
int tSysScpi::setDemodOfAmState(int value)
{
	if ((sysData.options.amOn && value == 1) || (!sysData.options.amOn && value == 0))
	{
		return 0;//make sure value  differ from cur state
	}

	if (value == 1 && !sysData.options.amOn)
	{
		sysData.ampt.refLevel = -10;//set reflevel
		sysData.ampt.att = 0;
		controlRf();
		checkMultiDisplay(mdtAM);
	}

	sysData.options.amOn = value == 1;

	if (sysData.options.amOn)
	{
		if (sysData.options.isDemodOn && sysData.options.fmOn)
		{
			setDemodOfFmState(0);//close fm as am or fm on once
		}

		setFrequencyOfSpan(0);
		//demodControl();
		sysData.mode = smAM;
		sysData.canvasNums = 2;
	} else
	{
		sysData.sweep.sweepTimeAuto = true;
		sysData.sweep.sweepSingle = false;
		sysData.mode = smSpectrum;
		sysData.canvasNums = 1;
		controlRf();
	}

	return __SCPI_SUCCESS;
}

//设置AM解调
int tSysScpi::setDemodOfAmState(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	int tempValue = getDoubleValue(value, &isOk);

	if (isOk)
	{
		return setDemodOfAmState(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置FM解调
int tSysScpi::setDemodOfFmState(int value)
{
	if ((sysData.options.fmOn && value == 1) || (!sysData.options.fmOn && value == 0))
	{
		return 0;
	}

	if (value == 1 && !sysData.options.fmOn)
	{
		sysData.ampt.refLevel = -10;
		sysData.ampt.att = 0;
		controlRf();
		checkMultiDisplay(mdtFM);
	}

	sysData.options.fmOn = value == 1;

	if (sysData.options.fmOn)
	{
		if (sysData.options.isDemodOn && sysData.options.amOn)
		{
			setDemodOfAmState(0);
		}

		setFrequencyOfSpan(0);
		demodControl();
		sysData.mode = smFM;
		sysData.canvasNums = 2;
	} else
	{
		sysData.sweep.sweepTimeAuto = true;
		sysData.sweep.sweepSingle = false;
		sysData.mode = smSpectrum;
		sysData.canvasNums = 1;
		controlRf();
	}

	return __SCPI_SUCCESS;
}

//设置FM解调
int tSysScpi::setDemodOfFmState(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	int tempValue = getDoubleValue(value, &isOk);

	if (isOk)
	{
		return setDemodOfFmState(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置AM载波频率
int tSysScpi::setDemodOfAmCarrierFreq(double value)
{
	double tempValue = value;
	double minFreq = MINDEMODFREQ + sysData.freq.offset;
	double maxFreq = MAXDEMODFREQ + sysData.freq.offset;

	if (tempValue < minFreq)
	{
		tempValue = minFreq;
	} else if (tempValue > maxFreq)
	{
		tempValue = maxFreq;
	}

	sysData.options.am_carrierfreq = tempValue;

	if (sysData.options.amOn)
	{
		demodControl();
	}

	return __SCPI_SUCCESS;
}

//设置AM载波频率
int tSysScpi::setDemodOfAmCarrierFreq(QString value)
{
	bool isOk = false;
	double tempValue = 0;

	tempValue = getFreqValue(value, &isOk);

	if (isOk)
	{
		return setDemodOfAmCarrierFreq(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//方向键设置AM载波频率
int tSysScpi::setDemodOfAmCarrierFreqWithArrow(bool up)
{
	double downSpanValue = sysData.options.am_carrierfreq - (MINDEMODFREQ + sysData.freq.offset);
	double upSpanValue = (MAXDEMODFREQ + sysData.freq.offset) - sysData.options.am_carrierfreq;

	if (up)
	{
		return setDemodOfAmCarrierFreq(sysData.options.am_carrierfreq + upSpanValue * 0.1);
	} else
	{
		return setDemodOfAmCarrierFreq(sysData.options.am_carrierfreq - downSpanValue * 0.1);
	}
}

//脉冲旋钮设置AM载波频率
int tSysScpi::setDemodOfAmCarrierFreqWithRpg(int value)
{
	double downSpanValue = sysData.options.am_carrierfreq - (MINDEMODFREQ + sysData.freq.offset);
	double upSpanValue = (MAXDEMODFREQ + sysData.freq.offset) - sysData.options.am_carrierfreq;

	if (value >= 0)
	{
		return setDemodOfAmCarrierFreq(sysData.options.am_carrierfreq + upSpanValue * value * 0.01);
	} else
	{
		return setDemodOfAmCarrierFreq(sysData.options.am_carrierfreq + downSpanValue * value * 0.01);
	}
}

//设置中频带宽状态
int tSysScpi::setDemodOfAmIFBWState(int value)
{
	bool prvState = sysData.options.am_ifbw_auto;

	sysData.options.am_ifbw_auto = value == 1;

	if (prvState && !sysData.options.am_ifbw_auto && sysData.options.amOn)
	{
		demodControl();
	}

	return __SCPI_SUCCESS;
}

//设置中频带宽状态
int tSysScpi::setDemodOfAmIFBWState(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	int tempValue = getDoubleValue(value, &isOk);

	if (isOk)
	{
		return setDemodOfAmIFBWState(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置AM中频带宽
int tSysScpi::setDemodOfAmIFBW(double value)
{
	if (sysData.options.am_ifbw_auto)
	{
		sysData.options.am_ifbw_auto = false;
	}

	double minFs = IFFS / MAXDEMODCIC;
	double maxFs = IFFS / MINDEMODCIC;
	double tempValue = value;

	if (tempValue < minFs)
	{
		tempValue = minFs;
	} else if (tempValue > maxFs)
	{
		tempValue = maxFs;
	}

	sysData.options.am_ifbw = tempValue;

	if (sysData.options.amOn)
	{
		demodControl();
	}

	return __SCPI_SUCCESS;
}

//设置AM中频带宽
int tSysScpi::setDemodOfAmIFBW(QString value)
{
	bool isOk = false;
	double tempValue = 0;

	tempValue = getFreqValue(value, &isOk);

	if (isOk)
	{
		return setDemodOfAmIFBW(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//方向键设置AM中频带宽
int tSysScpi::setDemodOfAmIFBWWithArrow(bool up)
{
	if (sysData.options.am_ifbw_auto)
	{
		sysData.options.am_ifbw_auto = false;
	}

	double minFs = IFFS / MAXDEMODCIC;
	double maxFs = IFFS / MINDEMODCIC;
	double tempValue = sysData.options.am_ifbw;

	if (up)
	{
		tempValue = zoomInData(tempValue, 3);
	} else
	{
		tempValue = zoomOutData(tempValue, 3);
	}

	if (tempValue < minFs)
	{
		tempValue = minFs;
	} else if (tempValue > maxFs)
	{
		tempValue = maxFs;
	}

	sysData.options.am_ifbw = tempValue;

	if (sysData.options.amOn)
	{
		demodControl();
	}

	return __SCPI_SUCCESS;
}

//脉冲旋钮设置AM中频带宽
int tSysScpi::setDemodOfAmIFBWWithRpg(int value)
{
	if (sysData.options.am_ifbw_auto)
	{
		sysData.options.am_ifbw_auto = false;
	}

	unsigned int tempValue = (unsigned int) (IFFS / sysData.options.am_ifbw);

	tempValue -= value;

	if (tempValue < MINDEMODCIC)
	{
		tempValue = MINDEMODCIC;
	} else if (tempValue > MAXDEMODCIC)
	{
		tempValue = MAXDEMODCIC;
	}

	sysData.options.am_ifbw = IFFS / tempValue;

	if (sysData.options.amOn)
	{
		demodControl();
	}

	return __SCPI_SUCCESS;
}

//设置FM载波频率
int tSysScpi::setDemodOfFmCarrierFreq(double value)
{
	double tempValue = value;
	double minFreq = MINDEMODFREQ + sysData.freq.offset;
	double maxFreq = MAXDEMODFREQ + sysData.freq.offset;

	if (tempValue < minFreq)
	{
		tempValue = minFreq;
	} else if (tempValue > maxFreq)
	{
		tempValue = maxFreq;
	}

	sysData.options.fm_carrierfreq = tempValue;

	if (sysData.options.fmOn)
	{
		demodControl();
	}

	return __SCPI_SUCCESS;
}

//设置FM载波频率
int tSysScpi::setDemodOfFmCarrierFreq(QString value)
{
	bool isOk = false;
	double tempValue = 0;

	tempValue = getFreqValue(value, &isOk);

	if (isOk)
	{
		return setDemodOfFmCarrierFreq(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//方向键设置FM载波频率
int tSysScpi::setDemodOfFmCarrierFreqWithArrow(bool up)
{
	double downSpanValue = sysData.options.fm_carrierfreq - (MINDEMODFREQ + sysData.freq.offset);
	double upSpanValue = (MAXDEMODFREQ + sysData.freq.offset) - sysData.options.fm_carrierfreq;

	if (up)
	{
		return setDemodOfFmCarrierFreq(sysData.options.fm_carrierfreq + upSpanValue * 0.1);
	} else
	{
		return setDemodOfFmCarrierFreq(sysData.options.fm_carrierfreq - downSpanValue * 0.1);
	}
}

//脉冲旋钮设置FM载波频率
int tSysScpi::setDemodOfFmCarrierFreqWithRpg(int value)
{
	double downSpanValue = sysData.options.fm_carrierfreq - (MINDEMODFREQ + sysData.freq.offset);
	double upSpanValue = (MAXDEMODFREQ + sysData.freq.offset) - sysData.options.fm_carrierfreq;

	if (value >= 0)
	{
		return setDemodOfFmCarrierFreq(sysData.options.fm_carrierfreq + upSpanValue * value * 0.01);
	} else
	{
		return setDemodOfFmCarrierFreq(sysData.options.fm_carrierfreq + downSpanValue * value * 0.01);
	}
}

//设置中频带宽状态
int tSysScpi::setDemodOfFmIFBWState(int value)
{
	bool prvState = sysData.options.fm_ifbw_auto;

	sysData.options.fm_ifbw_auto = value == 1;

	if (prvState && !sysData.options.fm_ifbw_auto && sysData.options.fmOn)
	{
		demodControl();
	}

	return __SCPI_SUCCESS;
}

//设置中频带宽状态
int tSysScpi::setDemodOfFmIFBWState(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	int tempValue = getDoubleValue(value, &isOk);

	if (isOk)
	{
		return setDemodOfFmIFBWState(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置FM中频带宽
int tSysScpi::setDemodOfFmIFBW(double value)
{
	if (sysData.options.fm_ifbw_auto)
	{
		sysData.options.fm_ifbw_auto = false;
	}

	double minFs = IFFS / MAXDEMODCIC;
	double maxFs = IFFS / MINDEMODCIC;
	double tempValue = value;

	if (tempValue < minFs)
	{
		tempValue = minFs;
	} else if (tempValue > maxFs)
	{
		tempValue = maxFs;
	}

	sysData.options.fm_ifbw = tempValue;

	if (sysData.options.fmOn)
	{
		demodControl();
	}

	return __SCPI_SUCCESS;
}

//设置FM中频带宽
int tSysScpi::setDemodOfFmIFBW(QString value)
{
	bool isOk = false;
	double tempValue = 0;

	tempValue = getFreqValue(value, &isOk);

	if (isOk)
	{
		return setDemodOfFmIFBW(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//方向键设置FM中频带宽
int tSysScpi::setDemodOfFmIFBWWithArrow(bool up)
{
	if (sysData.options.fm_ifbw_auto)
	{
		sysData.options.fm_ifbw_auto = false;
	}

	double minFs = IFFS / MAXDEMODCIC;
	double maxFs = IFFS / MINDEMODCIC;
	double tempValue = sysData.options.fm_ifbw;

	if (up)
	{
		tempValue = zoomInData(tempValue, 3);
	} else
	{
		tempValue = zoomOutData(tempValue, 3);
	}

	if (tempValue < minFs)
	{
		tempValue = minFs;
	} else if (tempValue > maxFs)
	{
		tempValue = maxFs;
	}

	sysData.options.fm_ifbw = tempValue;

	if (sysData.options.fmOn)
	{
		demodControl();
	}

	return __SCPI_SUCCESS;
}

//脉冲旋钮设置FM中频带宽
int tSysScpi::setDemodOfFmIFBWWithRpg(int value)
{
	if (sysData.options.fm_ifbw_auto)
	{
		sysData.options.fm_ifbw_auto = false;
	}

	unsigned int tempValue = (unsigned int) (IFFS / sysData.options.fm_ifbw);

	tempValue -= value;

	if (tempValue < MINDEMODCIC)
	{
		tempValue = MINDEMODCIC;
	} else if (tempValue > MAXDEMODCIC)
	{
		tempValue = MAXDEMODCIC;
	}

	sysData.options.fm_ifbw = IFFS / tempValue;

	if (sysData.options.fmOn)
	{
		demodControl();
	}

	return __SCPI_SUCCESS;
}

//获取文件列表
void tSysScpi::getSystemFileList(void)
{
	QString dir = "/home/sky/save";
	QStringList filter;
	QDir fDir(dir);

	sysData.file.total = 0;
	sysData.file.pages = 0;
	sysData.file.nowPageIndex = 0;
	sysData.file.nowPageCount = 0;
	filter.clear();

	switch (sysData.file.show)
	{
		case stScreenPng:
			filter << "*.png";
			break;
		case stTraceData:
			filter << "*.csv";
			break;
		case stAll:
			filter << "*.*";
	}

	if (fDir.exists())
	{
		fDir.setFilter(QDir::Files | QDir::Hidden | QDir::NoSymLinks);
		fDir.setSorting(QDir::NoSort);

		switch (sysData.file.sort)
		{
			case stName:
				fDir.setSorting(QDir::Name);
				break;
			case stTime:
				fDir.setSorting(QDir::Time);
				break;
			case stSize:
				fDir.setSorting(QDir::Size);
				break;
		}

		fileList.clear();
		fileList = fDir.entryInfoList(filter, QDir::Files);

		if (sysData.file.show == stAll)
		{
			sysData.file.freeDiskSpace = 1.0 - fileList.size() / (2.0 * MAXFILECOUNT);
		}

		sysData.file.total = fileList.size();

		if ((sysData.file.total % sysData.file.perPage) == 0)
		{
			sysData.file.pages = (int) (sysData.file.total / sysData.file.perPage);
		} else
		{
			sysData.file.pages = (int) (sysData.file.total / sysData.file.perPage) + 1;
		}

		sysData.file.nowPageIndex = 0;
		sysData.file.nowPageCount = 0;

		if (sysData.file.total > 0)
		{
			if (sysData.file.nowPageIndex < sysData.file.pages - 1)
			{
				sysData.file.nowPageCount = sysData.file.perPage;
			} else
			{
				sysData.file.nowPageCount = sysData.file.total - sysData.file.nowPageIndex * sysData.file.perPage;
			}
		}
	}

	if (sysData.file.nowSelIndex < 0)
	{
		sysData.file.nowSelIndex = 0;
	} else if (sysData.file.nowSelIndex >= sysData.file.nowPageCount)
	{
		sysData.file.nowSelIndex = sysData.file.nowPageCount - 1;
	}
}

//打印迹线
int tSysScpi::execPrintTrace(void)
{
	QRect rc = QRect(BORDERWIDTH, TOPHEIGHT, SCREENWIDTH * CANVASWIDTH - BORDERWIDTH, SCREENHEIGHT - TOPHEIGHT - BOTTOMHEIGHT);
	QPixmap pix(rc.size());
	pix = QPixmap::grabWindow(qApp->desktop()->winId(), rc.left(), rc.top(), rc.width(), rc.height());

	int pageSize = 26;
	int orientation = 1;
	int copyNums = sysData.print.nums;
	bool isColor = sysData.print.isColorPrinter;

	switch (sysData.print.paper)
	{
		case ptA4:
			pageSize = 26;
			break;
		case ptA3:
			pageSize = 27;
			break;
		case ptB5:
			pageSize = 100;
			break;
		case ptC5:
			pageSize = 91;
			break;
		case ptLetter:
			pageSize = 2;
			break;
	}

	if (!sysData.print.isOrientationHor)
	{
		orientation = 0;
	}

	return printPixmap(pix, pageSize, orientation, copyNums, isColor);
}

//打印屏幕
int tSysScpi::execPrintScreen(void)
{
	QPixmap pix(SCREENWIDTH, SCREENHEIGHT);
	pix = QPixmap::grabWindow(qApp->desktop()->winId(), 0, 0, SCREENWIDTH, SCREENHEIGHT);

	int pageSize = 26;
	int orientation = 1;
	int copyNums = sysData.print.nums;
	bool isColor = sysData.print.isColorPrinter;

	switch (sysData.print.paper)
	{
		case ptA4:
			pageSize = 26;
			break;
		case ptA3:
			pageSize = 27;
			break;
		case ptB5:
			pageSize = 100;
			break;
		case ptC5:
			pageSize = 91;
			break;
		case ptLetter:
			pageSize = 2;
			break;
	}

	if (!sysData.print.isOrientationHor)
	{
		orientation = 0;
	}

	return printPixmap(pix, pageSize, orientation, copyNums, isColor);
}

//获取系统日期时间
void tSysScpi::getSystemDateTime(void)
{
	char temp[32] = { };
	getLocalDate(temp);
	strcpy(sysData.system.datetime.date, temp);
	getLocalTime(temp);
	strcpy(sysData.system.datetime.time, temp);
}

//获取系统字体
void tSysScpi::getSystemFont(void)
{
	if (sysData.system.isLanguageEn)
	{
		fontBold = fontEnglishBold;
		fontLight = fontEnglishLight;
		fontData = fontLight;
		fontDataBig = fontBold;
		fontSmall = fontData;
	} else
	{
		fontBold = fontChinese;
		fontLight = fontChinese;
		fontData = fontEnglishLight;
		fontDataBig = fontBold;
		fontSmall = fontData;
	}

	fontDataBig.setPointSize(fontBold.pointSize() + 1);
	fontData.setPointSize(fontData.pointSize() - 1);
	fontSmall.setPointSize(fontData.pointSize() - 2);
	fontInput = fontData;
	fontInput.setPointSize(fontInput.pointSize() + 3);
	fontInput.setBold(true);
	fontUnderline = fontLight;
	fontUnderline.setUnderline(true);
}

//复位系统数据
void tSysScpi::presetSystemData(void)
{
	isRcvingLogoFile = false;

	//直采通道关闭
	resetZcParams();

	//默认关闭刷新
	sysData.system.isScpiScreenRefresh = false;
	sysData.isImpedanceTransformation = false;

	//屏幕及变量初始化
	sysData.mode = smSpectrum;
	sysData.canvasNums = 1;
	sysData.traceSelIndex = 0;
	sysData.markerSelIndex = 0;

	for (int i = 0; i < MARKERCOUNT; i++)
	{
		sysData.marker[i].state = msClose;
		sysData.marker[i].traceIndex = 0;
		memset(&sysData.marker[i].markerNormal, 0, sizeof sysData.marker[i].markerNormal);
		memset(&sysData.marker[i].markerDelta, 0, sizeof sysData.marker[i].markerDelta);
		sysData.marker[i].setted = false;
		sysData.marker[i].settedValue = 0;
	}

	sysData.markerTableOn = false;
	sysData.peakTableOn = false;
	sysData.isAutoPeakMaxing = false;
	sysData.isAutoSearching = false;
	sysData.isPreseting = false;
	sysData.isUserCalibrating = false;
	sysData.isFactoryCalibrating = false;
	sysData.isPreamplifierCalibrating = false;
	sysData.isFftCalibrating = false;
	sysData.isComCalbration = false;
	sysData.isZcCalibrating = false;
	sysData.isZcPreamplifierCalibrating = false;

	//迹线数据初始化
	//memset(sysData.initValue, 0, sizeof sysData.initValue);
	//memset(sysData.prjValue, 0, sizeof sysData.prjValue);
	//memset(sysData.avgValue, 0, sizeof sysData.avgValue);

	for (int i = 0; i < TRACECOUNT; i++)
	{
		sysData.trace[i].state = tsBlank;
		sysData.trace[i].flags = 0;

		if (i == 0)
		{
			//memset(sysData.trace[i].value, 0, sizeof sysData.trace[i].value);
		} else
		{
			//memset(sysData.trace[i].value, -100, sizeof sysData.trace[i].value);
		}
	}

	sysData.trace[0].state = tsClearWrite;
	sysData.trace[0].colorR = 0xff;
	sysData.trace[0].colorG = 0xff;
	sysData.trace[0].colorB = 0x00;
	sysData.trace[1].colorR = 0xff;
	sysData.trace[1].colorG = 0x00;
	sysData.trace[1].colorB = 0xff;
	sysData.trace[2].colorR = 0x00;
	sysData.trace[2].colorG = 0x80;
	sysData.trace[2].colorB = 0x80;
	sysData.trace[3].colorR = 0x00;
	sysData.trace[3].colorG = 0x00;
	sysData.trace[3].colorB = 0xff;
	sysData.trace[4].colorR = 0x00;
	sysData.trace[4].colorG = 0xff;
	sysData.trace[4].colorB = 0x00;

	//检波
	sysData.detector.detMode = dmPosPeak;

	//频标初始化
	for (int i = 0; i < MARKERCOUNT; i++)
	{
		sysData.marker[i].state = msClose;
		sysData.marker[i].markerNormal.xIndex = 0;
		sysData.marker[i].markerNormal.xValue = 0;
		sysData.marker[i].markerNormal.yValue = 0;
		sysData.marker[i].markerDelta.x1Index = 0;
		sysData.marker[i].markerDelta.x1Value = 0;
		sysData.marker[i].markerDelta.x2Index = 0;
		sysData.marker[i].markerDelta.x2Value = 0;
		sysData.marker[i].markerDelta.y1Value = 0;
		sysData.marker[i].markerDelta.y2Value = 0;
	}

	//频标功能初始化
	sysData.markerFctn.isNdBOn = false;
	sysData.markerFctn.isMkrNoiseOn = false;
	sysData.markerFctn.noiseValue = 0;
	sysData.markerFctn.isFreqCountOn = false;
	sysData.markerFctn.freqCountBw = 1;
	sysData.markerFctn.freqCountValue = 0;
	memset(&sysData.markerFctn.freqCountParam, 0, sizeof sysData.markerFctn.freqCountParam);
	sysData.markerFctn.ndBValue = 3.0;
	sysData.markerFctn.ndBIndex1 = 0;
	sysData.markerFctn.ndBIndex2 = 0;
	sysData.markerFctn.ndBFreq = 0;
	sysData.markerFctn.ndBFreq1 = 0;
	sysData.markerFctn.ndBFreq2 = 0;

	//频率
	sysData.freq.center = MAXSPAN / 2 + MINFREQ;
	sysData.freq.start = MINFREQ;
	sysData.freq.stop = MAXFREQ;
	sysData.freq.cfStep = MAXSPAN / 10;
	sysData.freq.offset = 0;
	sysData.freq.cfStepAuto = true;
	sysData.freq.refInternal = true;
	sysData.freq.typeLine = true;
	sysData.freq.isShowCenter = true;
	sysData.freq.lineZero = false;
	sysData.freq.isLowChannel = false;

	//扫宽
	sysData.span.span = MAXSPAN;
	sysData.span.prvSpan = MAXSPAN;
	sysData.span.nowSpan = MAXSPAN;
	sysData.span.isZeroSpan = false;
	ioctl(ramHandle, 0xff, sysData.span.isZeroSpan);

	//幅度
	sysData.ampt.refLevelMin = MINREFLEVELDBM;
	sysData.ampt.refLevelMax = MAXREFLEVELDBM;
	sysData.ampt.isPreamptOn = false;
	sysData.ampt.preamtOn = 0x02;
	//sysData.ampt.preamtOn = 0x01;
	sysData.ampt.refLevel = 0;
	sysData.ampt.att = 9;
	sysData.ampt.att1 = 0;
	sysData.ampt.att2 = 9;
	sysData.ampt.ifValue = 10;
	sysData.ampt.ifSoftValue = 0;
	sysData.ampt.attAuto = true;
	sysData.ampt.refOffset = 0;
	sysData.ampt.refUnit = rudBm;
	sysData.ampt.scaleLine = false;
	sysData.ampt.scaleDiv = 10.0;
	sysData.ampt.LMPValue = 0.0;

	sysData.ampt.refLevel = 0;
	sysData.ampt.attIf = 20;
	sysData.ampt.attRf = 10;
	sysData.ampt.att = 10;

	//分辨率
	sysData.bw.rbw = 5e6;
	sysData.bw.rbwAuto = true;
	sysData.bw.rbwStepCont = false;
	sysData.bw.vbw = sysData.bw.rbw;
	sysData.bw.vbwAuto = true;
	sysData.bw.vbwStepCont = false;
	sysData.bw.bwAverage = 64;
	sysData.bw.bwAverageFlag = 0;
	sysData.bw.bwAverageOn = false;
	sysData.bw.bwAverageFulled = false;
	sysData.emc.emcOn = false;
	sysData.emc.emcSaved = false;
	sysData.emc.emcRbwPrv = 0;
	sysData.emc.emcRbwPrvAuto = false;
	sysData.emc.emcVbwPrv = 0;
	sysData.emc.emcVbwPrvAuto = false;
	sysData.emc.emcRbwCont = false;
	sysData.emc.detectorPrv = dmPosPeak;

	//扫描时间
	sysData.sweep.sweepTimeAuto = true;
	sysData.sweep.sweepTime = 500 * 1e6;
	sysData.sweep.sweepPoints = RAMDATASIZE;
	sysData.sweep.sweepSingle = false;
//	calSweepTimeOfAuto();

	//显示
	sysData.display.isFullScreen = false;
	sysData.display.isZoomOn = false;
	sysData.display.zoomSpan = 0;
	sysData.display.zoomStartFreq = 0;
	sysData.display.zoomStopFreq = 0;
	sysData.display.zoomed = false;
	sysData.display.dispLineValue = -25.0;
	sysData.display.isAmptGraticuleOn = false;
	sysData.display.isDispLineOn = false;
	sysData.display.isGridOn = true;
	sysData.display.isLabelOn = true;
	sysData.display.winStyle = dsDefault;

	//测量设置
	sysData.measureSetup.channelWidth = 1e6;
	sysData.measureSetup.channelSpace = 2e6;
	sysData.measureSetup.channelNums = 3;
	sysData.measureSetup.powerPercent = 99;

	//测量
	sysData.measure.isAllOff = true;
	sysData.measure.isTimeSpectrumOn = false;
	sysData.measure.timeSpecFlag = 0;
	sysData.measure.isAcprOn = false;
	sysData.measure.acprTxChannelBw = 0;
	sysData.measure.acprTxChannelPower = 0;
	sysData.measure.channelPowerStartX = 0;
	sysData.measure.channelPowerStopX = 0;
	memset(sysData.measure.acprAdjacent, 0, sizeof sysData.measure.acprAdjacent);
	memset(sysData.measure.acprRects, 0, sizeof sysData.measure.acprRects);
	sysData.measure.acprTxChannelStartX = 0;
	sysData.measure.acprTxChannelStopX = 0;
	sysData.measure.isChannelPowerOn = false;
	sysData.measure.channelPower = 0;
	sysData.measure.isObwOn = false;
	sysData.measure.obwBandWidth = 0;
	sysData.measure.obwStartFreq = 0;
	sysData.measure.obwStartFreqPower = 0;
	sysData.measure.obwStopFreq = 0;
	sysData.measure.obwStartFreqPower = 0;
	sysData.measure.obwStartX = (sysData.sweep.sweepPoints - 1) / 2;
	sysData.measure.obwStopX = sysData.measure.obwStartX;
	sysData.measure.powerMeter.isPowerMeterOn = false;

	//Pass-Fail
	resetPassFailData();

	//系统
	//  sysData.system.serialNo = SERIALNO;
	sysData.system.isServiceUnlock = false;
	strcpy(sysData.system.serialNo, SERIALNO);
	sysData.system.isSerialNoOldModeOn = false;
	sysData.system.isInternalSignalOn = false;
	sysData.system.backLight = 50;
	sysData.system.io.lan.hostPort = SOCKETSEVERPORT; //5025
	strcpy(sysData.system.io.lan.hostIp, SOCKETSERVERIP);
	sysData.system.isLogOn = true;
	sysData.system.isGpibOn = false;
	strcpy(sysData.system.io.lan.ip, "192.168.1.6");
	strcpy(sysData.system.io.lan.mask, "255.255.255.0");
	strcpy(sysData.system.io.lan.gate, "192.168.1.1");
	//  strcpy(sysData.system.io.lan.mac, "00-40-5c-26-0a-52");
	sysData.system.io.lan.macLo = 0x6789ab;
	sysData.system.io.gpib.addr = 18;
	sysData.system.datetime.isVisible = true;
	sysData.system.datetime.style = dtsYMDHMS;
	sysData.system.powerOnPreset.powerOnParam = spFactory;
	sysData.system.powerOnPreset.presetParam = spFactory;
	getSystemDateTime();
	getSystemFont();
	sysData.system.firmwareVersion[0] = FIRMWARE_VER_MAJOR;
	sysData.system.firmwareVersion[1] = FIRMWARE_VER_MINOR;

	//源
	closeSourceGenerator();
	memset(&sysData.source, 0, sizeof sysData.source);
	sysData.source.trackGenOutputPower = -10;
	sysData.source.signalGenOutputFreq = 500e6;
	sysData.source.signalGenOutputPower = -10;
	sysData.source.isVswrOn = false;
	sysData.source.vswrRef = 0;
	sysData.source.measType = S21;

	//选件初始化
	sysData.options.isTrack36GHzOn = false;
	sysData.options.isFft10HzOn = false;

	//源模块初始化
	sysData.source.isSourceOn = false;
	sysData.source.isSignalGenOn = false;
	sysData.source.isTrackGenOn = false;
	sysData.source.R5 = 0x00000005;
	sysData.source.R4 = 0x00B5003C;
	sysData.source.R3 = 0x000004B3;
	sysData.source.R2 = 0x00004EC2;
	sysData.source.R1 = 0x0800fd01;
	sysData.source.R0 = 0x00C80000;
	sysData.source.BandNumber = 0x00;
	sysData.source.attentionValue = 10;

	//文件
	sysData.file.pages = 0;
	sysData.file.nowPageIndex = 0;
	sysData.file.nowPageCount = 0;
	sysData.file.nowSelIndex = 0;
	sysData.file.total = 0;
	sysData.file.show = stAll;
	sysData.file.sort = stName;
	sysData.file.freeDiskSpace = 1;
	sysData.file.perPage = FILEROWCOUNT - 1;
	sysData.file.isRestoring = false;

	//打印
	sysData.print.paper = ptA4;
	sysData.print.isPclLanguage = true;
	sysData.print.isColorPrinter = false;
	sysData.print.isOrientationHor = true;
	sysData.print.nums = 1;

	//解调
	sysData.demod.demodOn = false;
	sysData.demod.sound = 50;
	sysData.demod.mode = dmFMW;
	sysData.demod.radioSet = radio1;
	sysData.demod.radioFreqSet[0] = 87.6e6;
	sysData.demod.radioFreqSet[1] = 88.6e6;
	sysData.demod.radioFreqSet[2] = 89.5e6;
	sysData.demod.radioFreqSet[3] = 90.8e6;
	sysData.demod.radioFreqSet[4] = 93.5e6;
	sysData.demod.radioFreqSet[5] = 107.4e6;
	sysData.options.isFftOn = true; //FFT

	sysData.options.amOn = false;
	sysData.options.am_carrierfreq = sysData.freq.center;
	sysData.options.am_cicvalue = 30;
	sysData.options.am_ifbw = IFFS / sysData.options.am_cicvalue;
	sysData.options.am_ifbw_auto = true;
	sysData.options.fmOn = false;
	sysData.options.fm_carrierfreq = sysData.freq.center;
	sysData.options.fm_cicvalue = 30;
	sysData.options.fm_ifbw = IFFS / sysData.options.fm_cicvalue;
	sysData.options.fm_ifbw_auto = true;

	//disc
	if (sysData.disc.isDiscOn)
	{
		setMeasureOfDiscState(false);
	}
	memset(&sysData.disc, 0, sizeof sysData.disc);
	sysData.disc.outputPower = -10.0;
	sysData.disc.sweepTime = MINDISCTIME;
	sysData.disc.scaleDiv = 0.5;

	//获取校准数据
	getPreamplifierCalibrationData();
	getFactoryCalibrationData();
	getUserCalibrationData();
	getZcCalibrationData();
	getZcPreamplifierCalibrationData();

	//获取设备配置
	loadDeviceConfig();

	//获取源配置
	loadSourceConfig();

	//trig
	sysData.trig.mode = tgRun;
	sysData.trig.trigDown = 0x00;
	sysData.trig.power = -20.0;
	sysData.trig.isDisplayVideoLine = false;
	calculateVideoTrigPowerDown(sysData.trig.power);


	//射频前端
//	setBwOfRbw(100e3);
	controlRf();

//	feDownload(500, 1);
//	usleep(1);
//	feDownload(500, 0);
}

//复位系统
int tSysScpi::presetSystem(void)
{
	systemParam powerOnParam = sysData.system.powerOnPreset.powerOnParam;
	systemParam presetParam = sysData.system.powerOnPreset.presetParam;
	presetSystemData();
	sysData.system.powerOnPreset.powerOnParam = powerOnParam;
	sysData.system.powerOnPreset.presetParam = presetParam;

	execSelfCheck();

	if (sysData.system.powerOnPreset.presetParam == spUser)
	{
		loadUserState();
		controlRf();
	} else if (sysData.system.powerOnPreset.presetParam == spPowerOff)
	{
		loadPowerOffState();
		controlRf();
	}

	return __SCPI_SUCCESS;
}


////射频参数下发
//inline void tSysScpi::feDownload(unsigned int addr, unsigned long value)
//{
//	if (feHandle > 0)
//	{
//		ioctl(feHandle, addr, value);
//	}
//}

//void tSysScpi::tqIfDownload(unsigned int addr, unsigned long value)
//{
////  if (ifHandleTQ > 0)
////  {
////	ioctl(ifDev, addr, value);
////  }
//}


//射频参数下发
void tSysScpi::rfDownload(unsigned int addr, unsigned long value)
{
	if (rfHandle > 0)
	{
		ioctl(rfHandle, addr, value);
	}
}

//中频参数下发
void tSysScpi::ifDownload(unsigned int addr, unsigned long value)
{
	if (ifHandle > 0)
	{
		ioctl(ifHandle, (unsigned int) addr, (unsigned long) value);
	}
}

//关闭所有频标
int tSysScpi::setMarkerAllOff(void)
{
	for (int i = 0; i < MARKERCOUNT; i++)
	{
		sysData.marker[i].state = msClose;
		sysData.marker[i].setted = false;
		sysData.marker[i].settedValue = 0;
	}

	return __SCPI_SUCCESS;
}

//设置标记X值
int tSysScpi::setMarkerXValue(int mkrIndex, QString value)
{
	if (mkrIndex >= 0 && mkrIndex < MARKERCOUNT && !value.isNull() && !value.isEmpty())
	{
		bool isOk = false;
		double tempValue = 0;

		if (sysData.span.span > 0) //freq, default hz
		{
			tempValue = getFreqValue(value, &isOk);
		} else //time, default ns
		{
			tempValue = getTimeValue(value, &isOk);
		}

		if (!isOk)
		{
			return __SCPI_FAILED;
		}

		setMarkerXValue(mkrIndex, tempValue);

		return __SCPI_SUCCESS;
	}

	return __SCPI_FAILED;
}

//设置标记X值
int tSysScpi::setMarkerXValue(int mkrIndex, double value)
{
	if (isMarkerAllOff() == 0 || mkrIndex < 0 || mkrIndex >= MARKERCOUNT || sysData.marker[mkrIndex].state == msClose)
	{
		return 0;
	}

	double minValue = 0;
	double maxValue = 0;
	double setValue = 0;
	double tempValue = value;
	double mkrValue = 0;

	if (sysData.marker[mkrIndex].state == msNormal)
	{
		if (sysData.span.span > 0)
		{
			minValue = sysData.freq.start;
			maxValue = sysData.freq.stop;
		} else
		{
			minValue = 0;
			maxValue = sysData.sweep.sweepTime;
		}

		setValue = value;

		if (setValue < minValue)
		{
			setValue = minValue;
		} else if (setValue > maxValue)
		{
			setValue = maxValue;
		}

		for (int i = 0; i < sysData.sweep.sweepPoints; i++)
		{
			if (sysData.span.span > 0)
			{
				mkrValue = sysData.freq.start + i * sysData.span.span / (sysData.sweep.sweepPoints - 1);
			} else
			{
				mkrValue = i * sysData.sweep.sweepTime / (sysData.sweep.sweepPoints - 1);
			}

			if (mkrValue >= tempValue)
			{
				sysData.marker[mkrIndex].markerNormal.xIndex = i;
				sysData.marker[mkrIndex].setted = true;
				sysData.marker[mkrIndex].settedValue = setValue;
				return 0;
			}
		}

		sysData.marker[mkrIndex].markerNormal.xIndex = sysData.sweep.sweepPoints - 1;
		sysData.marker[mkrIndex].setted = true;
		sysData.marker[mkrIndex].settedValue = setValue;
	} else if (sysData.marker[mkrIndex].state == msDelta)
	{
		if (sysData.span.span > 0)
		{
			minValue = (0 - sysData.marker[mkrIndex].markerDelta.x2Index) * sysData.span.span / (sysData.sweep.sweepPoints - 1);
			maxValue = (sysData.sweep.sweepPoints - 1 - sysData.marker[mkrIndex].markerDelta.x2Index) * sysData.span.span / (sysData.sweep.sweepPoints - 1);
		} else
		{
			minValue = (0 - sysData.marker[mkrIndex].markerDelta.x2Index) * sysData.sweep.sweepTime / (sysData.sweep.sweepPoints - 1);
			;
			maxValue = (sysData.sweep.sweepPoints - 1 - sysData.marker[mkrIndex].markerDelta.x2Index) * sysData.sweep.sweepTime / (sysData.sweep.sweepPoints - 1);
		}

		setValue = value;

		if (setValue < minValue)
		{
			setValue = minValue;
		} else if (setValue > maxValue)
		{
			setValue = maxValue;
		}

		for (int i = 0; i < sysData.sweep.sweepPoints; i++)
		{
			if (sysData.span.span > 0)
			{
				mkrValue = (i - sysData.marker[mkrIndex].markerDelta.x2Index) * sysData.span.span / (sysData.sweep.sweepPoints - 1);
			} else
			{
				mkrValue = (i - sysData.marker[mkrIndex].markerDelta.x2Index) * sysData.sweep.sweepTime / (sysData.sweep.sweepPoints - 1);
			}

			if (mkrValue >= tempValue)
			{
				sysData.marker[mkrIndex].markerDelta.x1Index = i;
				sysData.marker[mkrIndex].setted = true;
				sysData.marker[mkrIndex].settedValue = setValue;
				return 0;
			}
		}

		sysData.marker[mkrIndex].markerDelta.x1Index = sysData.sweep.sweepPoints - 1;
		sysData.marker[mkrIndex].setted = true;
		sysData.marker[mkrIndex].settedValue = setValue;
	}

	return 0;
}

//标记均关闭
int tSysScpi::isMarkerAllOff(void)
{
	for (int i = 0; i < MARKERCOUNT; i++)
	{
		if (sysData.marker[i].state != msClose)
		{
			return __SCPI_FAILED;
		}
	}

	return __SCPI_SUCCESS;
}

//标记关闭
int tSysScpi::isTheMarkerOff(int mkrIndex, int traceIndex)
{
	if (mkrIndex < 0 || mkrIndex >= MARKERCOUNT || traceIndex < 0 || traceIndex >= TRACECOUNT || sysData.marker[mkrIndex].state != msClose)
	{
		return __SCPI_FAILED;
	} else
	{
		return __SCPI_SUCCESS;
	}
}

//设置常态标记
void tSysScpi::setMarkerNormal(int mkrIndex, int traceIndex)
{
	if (mkrIndex < 0 || mkrIndex >= MARKERCOUNT)
	{
		return;
	}

	if (sysData.marker[mkrIndex].state != msNormal)
	{
		if (sysData.marker[mkrIndex].state == msClose)
		{
			sysData.marker[mkrIndex].markerNormal.xIndex = (sysData.sweep.sweepPoints - 1) / 2;
		} else if (sysData.marker[mkrIndex].state == msDelta)
		{
			sysData.marker[mkrIndex].markerNormal.xIndex = sysData.marker[mkrIndex].markerDelta.x1Index;
		}

		if (sysData.marker[mkrIndex].traceIndex != traceIndex)
		{
			sysData.marker[mkrIndex].traceIndex = traceIndex;
		}

		sysData.marker[mkrIndex].state = msNormal;
	}

	sysData.marker[mkrIndex].setted = false;
	sysData.marker[mkrIndex].settedValue = 0;
}

//设置差值标记
void tSysScpi::setMarkerDelta(int mkrIndex, int traceIndex)
{
	if (mkrIndex < 0 || mkrIndex >= MARKERCOUNT)
	{
		return;
	}

	if (sysData.marker[mkrIndex].state == msClose)
	{
		sysData.marker[mkrIndex].markerDelta.x1Index = (sysData.sweep.sweepPoints - 1) / 2;
		sysData.marker[mkrIndex].markerDelta.x2Index = (sysData.sweep.sweepPoints - 1) / 2;
	} else if (sysData.marker[mkrIndex].state == msNormal)
	{
		sysData.marker[mkrIndex].markerDelta.x1Index = sysData.marker[mkrIndex].markerNormal.xIndex;
		sysData.marker[mkrIndex].markerDelta.x2Index = sysData.marker[mkrIndex].markerDelta.x1Index;
	} else
	{
		sysData.marker[mkrIndex].markerDelta.x2Index = sysData.marker[mkrIndex].markerDelta.x1Index;
	}

	if (sysData.marker[mkrIndex].traceIndex != traceIndex)
	{
		sysData.marker[mkrIndex].traceIndex = traceIndex;
	}

	sysData.marker[mkrIndex].markerDelta.y2Value = getTheMarkerYValue(mkrIndex, sysData.marker[mkrIndex].markerDelta.x2Index);
	sysData.marker[mkrIndex].state = msDelta;
	sysData.marker[mkrIndex].setted = false;
	sysData.marker[mkrIndex].settedValue = 0;
}

//获取指定索引X值
double tSysScpi::getTheMarkerXValue(int mkrIndex, int xIndex)
{
	if (mkrIndex < 0 || mkrIndex >= MARKERCOUNT || xIndex < 0 || xIndex >= sysData.sweep.sweepPoints)
	{
		return 0;
	}

	if (sysData.span.span > 0)
	{
		if (sysData.marker[mkrIndex].setted)
		{
			return sysData.marker[mkrIndex].settedValue * 1.0;
		} else
		{
			return (sysData.freq.start + xIndex * sysData.span.span / (sysData.sweep.sweepPoints - 1)) * 1.0;
		}
	} else
	{
		if (sysData.marker[mkrIndex].setted)
		{
			return sysData.marker[mkrIndex].settedValue * 1.0;
		} else
		{
			return (sysData.sweep.sweepTime * xIndex / (sysData.sweep.sweepPoints - 1)) * 1.0;
		}
	}
}

//获取指定索引Y值
double tSysScpi::getTheMarkerYValue(int mkrIndex, int xIndex)
{
	if (mkrIndex < 0 || mkrIndex >= MARKERCOUNT || xIndex < 0 || xIndex >= sysData.sweep.sweepPoints)
	{
		return 0;
	}

	return sysData.trace[sysData.marker[mkrIndex].traceIndex].value[xIndex];
}

//获取标记X值
double tSysScpi::getMarkerXValue(int mkrIndex)
{
	if (mkrIndex < 0 || mkrIndex >= MARKERCOUNT)
	{
		return 0;
	}

	if (sysData.marker[mkrIndex].state == msNormal)
	{
		return getTheMarkerXValue(mkrIndex, sysData.marker[mkrIndex].markerNormal.xIndex);
	} else if (sysData.marker[mkrIndex].state == msDelta)
	{
		if (sysData.marker[mkrIndex].setted)
		{
			return sysData.marker[mkrIndex].settedValue;
		} else
		{
			return getTheMarkerXValue(mkrIndex, sysData.marker[mkrIndex].markerDelta.x1Index) - getTheMarkerXValue(mkrIndex, sysData.marker[mkrIndex].markerDelta.x2Index);
		}
	}

	return 0;
}

//获取标记Y值
double tSysScpi::getMarkerYValue(int mkrIndex)
{
	if (mkrIndex < 0 || mkrIndex >= MARKERCOUNT)
	{
		return 0;
	}

	if (sysData.marker[mkrIndex].state == msNormal)
	{
		return getTheMarkerYValue(mkrIndex, sysData.marker[mkrIndex].markerNormal.xIndex);
	} else if (sysData.marker[mkrIndex].state == msDelta)
	{
		if (sysData.ampt.scaleLine)
		{
			return getTheMarkerYValue(mkrIndex, sysData.marker[mkrIndex].markerDelta.x1Index) * 100.0 / sysData.marker[mkrIndex].markerDelta.y2Value;
		} else
		{
			return getTheMarkerYValue(mkrIndex, sysData.marker[mkrIndex].markerDelta.x1Index) - sysData.marker[mkrIndex].markerDelta.y2Value;
		}
	}

	return 0;
}
//设置EMI峰值列表
int tSysScpi::setPeakTable(int value)
{
	sysData.peakTableOn = value == 1;

	if (sysData.peakTableOn)
	{
		checkMultiDisplay(mdtPeakTable);
		sysData.mode = smPeakTable;
		sysData.canvasNums = 2;
	} else
	{
		sysData.mode = smSpectrum;
		sysData.canvasNums = 1;
	}

	return __SCPI_SUCCESS;
}

//设置频标列表
int tSysScpi::setMarkerTable(int value)
{
	sysData.markerTableOn = value == 1;

	if (sysData.markerTableOn)
	{
		checkMultiDisplay(mdtMkrTable);
		sysData.mode = smMarkerTable;
		sysData.canvasNums = 2;
	} else
	{
		sysData.mode = smSpectrum;
		sysData.canvasNums = 1;
	}

	return __SCPI_SUCCESS;
}

//设置频标列表
int tSysScpi::setMarkerTable(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	int tempValue = getBoolValue(value.toUpper().trimmed(), &isOk);

	if (isOk)
	{
		return setMarkerTable(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//标记→中心频率
int tSysScpi::setMarkerToCenterFreq(int mkrIndex)
{
	if (mkrIndex < 0 || mkrIndex >= MARKERCOUNT || sysData.span.span <= 0 || sysData.marker[mkrIndex].state == msClose)
	{
		return __SCPI_FAILED;
	}

	double tempValue = 0;

	if (sysData.marker[mkrIndex].state == msNormal)
	{
		tempValue = sysData.freq.start + sysData.marker[mkrIndex].markerNormal.xIndex * sysData.span.span / (sysData.sweep.sweepPoints - 1);
		sysData.marker[mkrIndex].markerNormal.xIndex = (sysData.sweep.sweepPoints - 1) / 2;
	} else
	{
		tempValue = sysData.freq.start + sysData.marker[mkrIndex].markerDelta.x1Index * sysData.span.span / (sysData.sweep.sweepPoints - 1);
		sysData.marker[mkrIndex].markerDelta.x1Index = (sysData.sweep.sweepPoints - 1) / 2;
	}

	setFrequencyOfCenter(tempValue);
	controlRf();

	return __SCPI_SUCCESS;
}

//标记→中心频率步进
int tSysScpi::setMarkerToCenterFreqStep(int mkrIndex)
{
	if (mkrIndex < 0 || mkrIndex >= MARKERCOUNT || sysData.span.span <= 0 || sysData.marker[mkrIndex].state == msClose)
	{
		return __SCPI_FAILED;
	}

	double tempValue = 0;

	if (sysData.marker[mkrIndex].state == msNormal)
	{
		tempValue = sysData.freq.start + sysData.marker[mkrIndex].markerNormal.xIndex * sysData.span.span / (sysData.sweep.sweepPoints - 1);
	} else
	{
		tempValue = sysData.freq.start + sysData.marker[mkrIndex].markerDelta.x1Index * sysData.span.span / (sysData.sweep.sweepPoints - 1);
	}

	return setFrequencyOfCenterStep(tempValue);
}

//标记→起始频率
int tSysScpi::setMarkerToStartFreq(int mkrIndex)
{
	if (mkrIndex < 0 || mkrIndex >= MARKERCOUNT || sysData.span.span <= 0 || sysData.marker[mkrIndex].state == msClose)
	{
		return __SCPI_FAILED;
	}

	double tempValue = 0;

	if (sysData.marker[mkrIndex].state == msNormal)
	{
		tempValue = sysData.freq.start + sysData.marker[mkrIndex].markerNormal.xIndex * sysData.span.span / (sysData.sweep.sweepPoints - 1);
		sysData.marker[mkrIndex].markerNormal.xIndex = 0;
	} else
	{
		tempValue = sysData.freq.start + sysData.marker[mkrIndex].markerDelta.x1Index * sysData.span.span / (sysData.sweep.sweepPoints - 1);
		sysData.marker[mkrIndex].markerDelta.x1Index = 0;
	}

	setFrequencyOfStart(tempValue);
	controlRf();

	return __SCPI_SUCCESS;
}

//标记→终止频率
int tSysScpi::setMarkerToStopFreq(int mkrIndex)
{
	if (mkrIndex < 0 || mkrIndex >= MARKERCOUNT || sysData.span.span <= 0 || sysData.marker[mkrIndex].state == msClose)
	{
		return __SCPI_FAILED;
	}

	double tempValue = 0;

	if (sysData.marker[mkrIndex].state == msNormal)
	{
		tempValue = sysData.freq.start + sysData.marker[mkrIndex].markerNormal.xIndex * sysData.span.span / (sysData.sweep.sweepPoints - 1);
		sysData.marker[mkrIndex].markerNormal.xIndex = sysData.sweep.sweepPoints - 1;
	} else
	{
		tempValue = sysData.freq.start + sysData.marker[mkrIndex].markerDelta.x1Index * sysData.span.span / (sysData.sweep.sweepPoints - 1);
		sysData.marker[mkrIndex].markerDelta.x1Index = sysData.sweep.sweepPoints - 1;
	}

	setFrequencyOfStop(tempValue);
	controlRf();

	return __SCPI_SUCCESS;
}

//标记→参考电平
int tSysScpi::setMarkerToRefLevel(int mkrIndex)
{
	if (mkrIndex < 0 || mkrIndex >= MARKERCOUNT || sysData.marker[mkrIndex].state == msClose)
	{
		return __SCPI_FAILED;
	}

	double tempValue = 0;

	if (sysData.marker[mkrIndex].state == msNormal)
	{
		tempValue = getTheMarkerYValue(mkrIndex, sysData.marker[mkrIndex].markerNormal.xIndex);
	} else
	{
		tempValue = getTheMarkerYValue(mkrIndex, sysData.marker[mkrIndex].markerDelta.x1Index);
	}

	setAmptOfRefLevel(tempValue);
	controlRf();

	return __SCPI_SUCCESS;
}

//标记△→扫宽
int tSysScpi::setMarkerDeltaToSpan(int mkrIndex)
{
	if (mkrIndex < 0 || mkrIndex >= MARKERCOUNT || sysData.span.span <= 0 || sysData.marker[mkrIndex].state != msDelta)
	{
		return __SCPI_FAILED;
	}

	double tempValue = 0;

	tempValue = qAbs(sysData.marker[mkrIndex].markerDelta.x1Index - sysData.marker[mkrIndex].markerDelta.x2Index) * sysData.span.span / (sysData.sweep.sweepPoints - 1);
	setFrequencyOfSpan(tempValue);
	controlRf();

	return __SCPI_SUCCESS;
}

//标记△→中心频率
int tSysScpi::setMarkerDeltaToCenterFreq(int mkrIndex)
{
	if (mkrIndex < 0 || mkrIndex >= MARKERCOUNT || sysData.span.span <= 0 || sysData.marker[mkrIndex].state != msDelta)
	{
		return __SCPI_FAILED;
	}

	double tempValue = 0;

	tempValue = qAbs(sysData.marker[mkrIndex].markerDelta.x1Index - sysData.marker[mkrIndex].markerDelta.x2Index) * sysData.span.span / (sysData.sweep.sweepPoints - 1);
	setFrequencyOfCenter(tempValue);
	controlRf();

	return __SCPI_SUCCESS;
}

//设置频标功能关闭
int tSysScpi::setMarkerFunctionAllOff(void)
{
	if (sysData.markerFctn.isNdBOn)
	{
		sysData.markerFctn.isNdBOn = false;
	}

	if (sysData.markerFctn.isMkrNoiseOn)
	{
		sysData.markerFctn.isMkrNoiseOn = false;
	}

	if (sysData.markerFctn.isFreqCountOn)
	{
		sysData.markerFctn.isFreqCountOn = false;
		resetFreqCountParam();
		controlRf();
	}

	return __SCPI_SUCCESS;
}

//设置频标功能NdB开启
int tSysScpi::setMarkerFunctionNdBOn(int value)
{
	setMarkerFunctionAllOff();
	sysData.markerFctn.isNdBOn = value == 1;

	return __SCPI_SUCCESS;
}

//设置频标功能NdB开启
int tSysScpi::setMarkerFunctionNdBOn(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	int tempValue = getBoolValue(value.toUpper().trimmed(), &isOk);

	if (isOk)
	{
		return setMarkerFunctionNdBOn(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置频标功能NdB值
int tSysScpi::setMarkerFunctionNdBValue(double value)
{
	double tempValue = value;

	if (tempValue < MINNDB)
	{
		tempValue = MINNDB;
	} else if (tempValue > MAXNDB)
	{
		tempValue = MAXNDB;
	}

	sysData.markerFctn.ndBValue = tempValue;

	return __SCPI_SUCCESS;
}

//设置频标功能NdB值
int tSysScpi::setMarkerFunctionNdBValue(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	double tempValue = getDoubleValue(value, &isOk);

	if (isOk)
	{
		return setMarkerFunctionNdBValue(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//方向键设置频标功能NdB值
int tSysScpi::setMarkerFunctionNdBValueWithArrow(bool up)
{
	if (up)
	{
		return setMarkerFunctionNdBValue(sysData.markerFctn.ndBValue + 1.0);
	} else
	{
		return setMarkerFunctionNdBValue(sysData.markerFctn.ndBValue - 1.0);
	}
}

//脉冲旋钮设置频标功能NdB值
int tSysScpi::setMarkerFunctionNdBValueWithRpg(int value)
{
	return setMarkerFunctionNdBValue(sysData.markerFctn.ndBValue + 0.01 * value * sysData.markerFctn.ndBValue);
}

//设置频标功能噪声开启
int tSysScpi::setMarkerFunctionNoiseOn(int value)
{
	setMarkerFunctionAllOff();
	sysData.markerFctn.isMkrNoiseOn = value == 1;

	return __SCPI_SUCCESS;
}

//设置频标功能噪声开启
int tSysScpi::setMarkerFunctionNoiseOn(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	int tempValue = getBoolValue(value.toUpper().trimmed(), &isOk);

	if (isOk)
	{
		return setMarkerFunctionNoiseOn(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置频标功能频率计数开启
int tSysScpi::setMarkerFunctionFreqCountOn(int value)
{
	bool prvState = sysData.markerFctn.isFreqCountOn;
	bool nowState = value == 1;
	unsigned long freqBwCount = 0;

	if (prvState != nowState && nowState)
	{
		for (int i = 0; i < TRACECOUNT; i++)
		{
			memcpy(tempTraceData[i], sysData.trace[i].value, sizeof tempTraceData[i]);
		}

		sysData.markerFctn.freqCountParam.centerFreq = sysData.freq.center;
		sysData.markerFctn.freqCountParam.span = sysData.span.span;
		sysData.markerFctn.freqCountParam.startFreq = sysData.freq.start;
		sysData.markerFctn.freqCountParam.stopFreq = sysData.freq.stop;
		sysData.markerFctn.freqCountParam.rbw = sysData.bw.rbw;
		sysData.markerFctn.freqCountParam.rbwAuto = sysData.bw.rbwAuto;
		sysData.markerFctn.freqCountParam.vbw = sysData.bw.vbw;
		sysData.markerFctn.freqCountParam.vbwAuto = sysData.bw.vbwAuto;
		sysData.markerFctn.freqCountParam.refLevel = sysData.ampt.refLevel;
		sysData.markerFctn.freqCountParam.att = sysData.ampt.att;
		sysData.markerFctn.freqCountParam.att1 = sysData.ampt.att1;
		sysData.markerFctn.freqCountParam.att2 = sysData.ampt.att2;
		sysData.markerFctn.freqCountParam.attAuto = sysData.ampt.attAuto;
		sysData.markerFctn.freqCountParam.sweepTime = sysData.sweep.sweepTime;
		sysData.markerFctn.freqCountParam.sweepTimeAuto = sysData.sweep.sweepTimeAuto;
		sysData.markerFctn.freqCountParam.isSaved = true;

		setFrequencyOfCenter(getTheMarkerXValue(sysData.markerSelIndex, sysData.marker[sysData.markerSelIndex].markerNormal.xIndex));
		setFrequencyOfSpan(0);
		controlRf();
		freqBwCount = 16777216 - 1e7 / sysData.markerFctn.freqCountBw;
		ifDownload(0x1a, (freqBwCount & 0xff0000) >> 16);
		ifDownload(0x1a, (freqBwCount & 0xff00) >> 8);
		ifDownload(0x1a, (freqBwCount & 0xff) >> 0);
		ifDownload(0x1c, 0x01);

	} else if (prvState == nowState && nowState)
	{
		freqBwCount = 16777216 - 1e7 / sysData.markerFctn.freqCountBw;
		ifDownload(0x1a, (freqBwCount & 0xff0000) >> 16);
		ifDownload(0x1a, (freqBwCount & 0xff00) >> 8);
		ifDownload(0x1a, (freqBwCount & 0xff) >> 0);
		ifDownload(0x1c, 0x01);
	} else if (!nowState)
	{
		setMarkerFunctionAllOff();
	}

	sysData.markerFctn.isFreqCountOn = value == 1;
	sysData.markerFctn.freqCountValue = 0;

	if (!sysData.markerFctn.isFreqCountOn && prvState != nowState && prvState)
	{
		resetFreqCountParam();
		controlRf();
	}

	return __SCPI_SUCCESS;
}

//设置频标功能频率计数开启
int tSysScpi::setMarkerFunctionFreqCountOn(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	int tempValue = getBoolValue(value.toUpper().trimmed(), &isOk);

	if (isOk)
	{
		return setMarkerFunctionFreqCountOn(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置频标功能频率计数分辨率
int tSysScpi::setMarkerFunctionFreqCountBw(int value)
{
	int tempValue = value;
	int freqBwSize = sizeof FREQCOUNTBW / sizeof FREQCOUNTBW[0];

	if (tempValue < FREQCOUNTBW[0])
	{
		tempValue = FREQCOUNTBW[0];
	} else if (tempValue > FREQCOUNTBW[freqBwSize - 1])
	{
		tempValue = FREQCOUNTBW[freqBwSize - 1];
	}

	double lValue = 0;
	double rValue = 0;

	for (int i = 0; i < freqBwSize; i++)
	{
		if (FREQCOUNTBW[i] == tempValue)
		{
			sysData.markerFctn.freqCountBw = FREQCOUNTBW[i];

			if (sysData.markerFctn.isFreqCountOn)
			{
				setMarkerFunctionFreqCountOn(sysData.markerFctn.isFreqCountOn);
			}

			return __SCPI_SUCCESS;
		} else if (i > 0 && FREQCOUNTBW[i] > tempValue)
		{
			lValue = qAbs(tempValue - FREQCOUNTBW[i - 1]);
			rValue = qAbs(FREQCOUNTBW[i] - tempValue);

			if (lValue <= rValue)
			{
				sysData.markerFctn.freqCountBw = FREQCOUNTBW[i - 1];
			} else
			{
				sysData.markerFctn.freqCountBw = FREQCOUNTBW[i];
			}

			if (sysData.markerFctn.isFreqCountOn)
			{
				setMarkerFunctionFreqCountOn(sysData.markerFctn.isFreqCountOn);
			}

			return __SCPI_SUCCESS;
		}
	}

	sysData.markerFctn.freqCountBw = FREQCOUNTBW[freqBwSize - 1];

	if (sysData.markerFctn.isFreqCountOn)
	{
		setMarkerFunctionFreqCountOn(sysData.markerFctn.isFreqCountOn);
	}

	return __SCPI_SUCCESS;
}

//设置频标功能频率计数分辨率
int tSysScpi::setMarkerFunctionFreqCountBw(QString value)
{
	bool isOk = false;
	double tempValue = 0;

	tempValue = getFreqValue(value, &isOk);

	if (isOk)
	{
		return setMarkerFunctionFreqCountBw(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//方向键设置频标功能频率计数分辨率
int tSysScpi::setMarkerFunctionFreqCountBwWithArrow(bool up)
{
	int index = -1;
	int freqBwSize = sizeof FREQCOUNTBW / sizeof FREQCOUNTBW[0];

	for (int i = 0; i < freqBwSize; i++)
	{
		if (FREQCOUNTBW[i] == sysData.markerFctn.freqCountBw)
		{
			index = i;
			break;
		}
	}

	if (index == -1)
	{
		sysData.markerFctn.freqCountBw = FREQCOUNTBW[0];
	} else
	{
		if (up && index < freqBwSize - 1)
		{
			index++;
		} else if (!up && index > 0)
		{
			index--;
		}

		sysData.markerFctn.freqCountBw = FREQCOUNTBW[index];
	}

	if (sysData.markerFctn.isFreqCountOn)
	{
		setMarkerFunctionFreqCountOn(sysData.markerFctn.isFreqCountOn);
	}

	return __SCPI_SUCCESS;
}

//脉冲旋钮设置频标功能频率计数分辨率
int tSysScpi::setMarkerFunctionFreqCountBwWithRpg(int value)
{
	if (value == 0)
	{
		return __SCPI_FAILED;
	} else
	{
		return setMarkerFunctionFreqCountBwWithArrow(value > 0);
	}
}

//获取NdB
int tSysScpi::getMarkerFunctionNdB(int mkrIndex)
{
	if (!sysData.markerFctn.isNdBOn || sysData.span.span <= 0 || mkrIndex < 0 || mkrIndex >= MARKERCOUNT || sysData.marker[mkrIndex].state == msClose)
	{
		return __SCPI_FAILED;
	}

	int index1 = 0;
	int index2 = 0;

	if (setMarkerPeakMax(sysData.markerSelIndex, sysData.marker[mkrIndex].traceIndex) == __SCPI_SUCCESS)
	{
		int index = 0;
		double value = 0;
		double value1 = 0;
		double value2 = 0;
		double err1 = 0;
		double err2 = 0;

		if (sysData.marker[mkrIndex].state == msNormal)
		{
			index = sysData.marker[mkrIndex].markerNormal.xIndex;
		} else if (sysData.marker[mkrIndex].state == msDelta)
		{
			index = sysData.marker[mkrIndex].markerDelta.x1Index;
		}

		index1 = index;
		index2 = index;
		value = sysData.trace[sysData.marker[mkrIndex].traceIndex].value[index];

		index1 = 0;

		for (int i = index - 1; i >= 0; i--)
		{
			value1 = qAbs(sysData.trace[sysData.marker[mkrIndex].traceIndex].value[i] - value);

			if (value1 >= sysData.markerFctn.ndBValue)
			{
				index1 = i;

				if ((index1 + 1) <= (index - 1))
				{
					err1 = qAbs(qAbs(sysData.trace[sysData.marker[mkrIndex].traceIndex].value[index1 + 1] - value) - sysData.markerFctn.ndBValue);
					err2 = qAbs(value1 - sysData.markerFctn.ndBValue);

					if (err1 <= err2)
					{
						index1++;
					}
				}

				break;
			}
		}

		index2 = sysData.sweep.sweepPoints - 1;

		for (int i = index + 1; i < sysData.sweep.sweepPoints; i++)
		{
			value2 = qAbs(sysData.trace[sysData.marker[mkrIndex].traceIndex].value[i] - value);

			if (value2 >= sysData.markerFctn.ndBValue)
			{
				index2 = i;

				if ((index2 - 1) >= (index + 1))
				{
					err1 = qAbs(qAbs(sysData.trace[sysData.marker[mkrIndex].traceIndex].value[index2 - 1] - value) - sysData.markerFctn.ndBValue);
					err2 = qAbs(value1 - sysData.markerFctn.ndBValue);

					if (err1 <= err2)
					{
						index2--;
					}
				}

				break;
			}
		}
	}

	sysData.markerFctn.ndBIndex1 = index1;
	sysData.markerFctn.ndBIndex2 = index2;
	sysData.markerFctn.ndBFreq1 = getTheMarkerXValue(mkrIndex, sysData.markerFctn.ndBIndex1);
	sysData.markerFctn.ndBFreq2 = getTheMarkerXValue(mkrIndex, sysData.markerFctn.ndBIndex2);
	sysData.markerFctn.ndBFreq = qAbs(sysData.markerFctn.ndBFreq1 - sysData.markerFctn.ndBFreq2);
	return __SCPI_SUCCESS;
}

//获取噪声
int tSysScpi::getMarkerFunctionNoise(int mkrIndex)
{
	if (!sysData.markerFctn.isMkrNoiseOn || sysData.span.span <= 0 || mkrIndex < 0 || mkrIndex >= MARKERCOUNT || sysData.marker[mkrIndex].state == msClose)
	{
		return __SCPI_FAILED;
	}

	int index = 0;
	int indexR = 0;
	double value = 0;
	double valueR = 0;

	if (sysData.marker[mkrIndex].state == msNormal)
	{
		index = sysData.marker[mkrIndex].markerNormal.xIndex;
		value = getTheMarkerYValue(mkrIndex, index) - 10 * log10(sysData.bw.rbw);
		sysData.markerFctn.noiseValue = value;
	} else if (sysData.marker[mkrIndex].state == msDelta)
	{
		index = sysData.marker[mkrIndex].markerDelta.x1Index;
		value = getTheMarkerYValue(mkrIndex, index);
		indexR = sysData.marker[mkrIndex].markerDelta.x2Index;
		valueR = sysData.marker[mkrIndex].markerDelta.y2Value;

		sysData.markerFctn.noiseValue = (value - valueR) - 10 * log10(sysData.bw.rbw);
	}

	return __SCPI_SUCCESS;
}

int tSysScpi::setMarkerFunctionNoiseStartFreq(QString value)
{
	if (isAllMarkerOff() || isTheMarkerOff(sysData.markerSelIndex) || value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	double tempValue = getFreqValue(value, &isOk);

	if (isOk)
	{
		if (sysData.marker[sysData.markerSelIndex].state == msNormal)
		{
			setMarkerXValue(sysData.markerSelIndex, tempValue);
			return __SCPI_SUCCESS;
		} else if (sysData.marker[sysData.markerSelIndex].state == msDelta)
		{
			double freq = 0;

			if (tempValue < sysData.freq.start)
			{
				sysData.marker[sysData.markerSelIndex].markerDelta.x2Index = 0;
			} else if (tempValue > sysData.freq.stop)
			{
				sysData.marker[sysData.markerSelIndex].markerDelta.x2Index = sysData.sweep.sweepPoints - 1;
			} else
			{
				for (int i = 0; i < sysData.sweep.sweepPoints; i++)
				{
					freq = sysData.freq.start + i * sysData.span.span / (sysData.sweep.sweepPoints - 1);

					if (freq >= tempValue)
					{
						sysData.marker[sysData.markerSelIndex].markerDelta.x2Index = i;
						break;
					}
				}
			}

			sysData.marker[sysData.markerSelIndex].markerDelta.y2Value = getTheMarkerYValue(sysData.markerSelIndex, sysData.marker[sysData.markerSelIndex].markerDelta.x2Index);
			return __SCPI_SUCCESS;
		}
	} else
	{
		return __SCPI_FAILED;
	}
}

int tSysScpi::setMarkerFunctionNoiseSpanFreq(QString value)
{
	if (isAllMarkerOff() || isTheMarkerOff(sysData.markerSelIndex) || value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	double tempValue = getFreqValue(value, &isOk);

	if (isOk && sysData.marker[sysData.markerSelIndex].state == msDelta)
	{
		setMarkerXValue(sysData.markerSelIndex, tempValue);
		return __SCPI_SUCCESS;
	} else
	{
		return __SCPI_FAILED;
	}
}

QString tSysScpi::getMarkerFunctionNoiseStartFreq(void)
{
	if (isAllMarkerOff() || isTheMarkerOff(sysData.markerSelIndex))
	{
		return "0";
	}

	int mkrIndex = sysData.markerSelIndex;
	int xIndex = 0;
	char tempChar[32] = { 0 };

	if (sysData.marker[mkrIndex].state == msNormal)
	{
		xIndex = sysData.marker[mkrIndex].markerNormal.xIndex;
		return QString(floatToString(getTheMarkerXValue(mkrIndex, xIndex), 0, 0, tempChar)).trimmed();
	} else if (sysData.marker[mkrIndex].state == msDelta)
	{
		xIndex = sysData.marker[mkrIndex].markerDelta.x2Index;
		return QString(floatToString(getTheMarkerXValue(mkrIndex, xIndex), 0, 0, tempChar)).trimmed();
	} else
	{
		return "0";
	}
}

QString tSysScpi::getMarkerFunctionNoiseSpanFreq(void)
{
	if (isAllMarkerOff() || isTheMarkerOff(sysData.markerSelIndex))
	{
		return "0";
	}

	if (sysData.marker[sysData.markerSelIndex].state == msDelta)
	{
		char tempChar[32] = { 0 };

		return QString(floatToString(getMarkerXValue(sysData.markerSelIndex), 0, 0, tempChar)).trimmed();
	} else
	{
		return "0";
	}
}

//获取迹线最大值
double tSysScpi::tSysScpi::getTraceMaxValue(int traceIndex)
{
	if (traceIndex < 0 || traceIndex >= TRACECOUNT)
	{
		return 0;
	}

	double result = sysData.trace[traceIndex].value[0];

	for (int i = 1; i < sysData.sweep.sweepPoints; i++)
	{
		if (sysData.trace[traceIndex].value[i] > result)
		{
			result = sysData.trace[0].value[i];
		}
	}

	return result;
}

//获取迹线最小值
double tSysScpi::getTraceMinValue(int traceIndex)
{
	if (traceIndex < 0 || traceIndex >= TRACECOUNT)
	{
		return 0;
	}

	double result = sysData.trace[traceIndex].value[0];

	for (int i = 1; i < sysData.sweep.sweepPoints; i++)
	{
		if (sysData.trace[traceIndex].value[i] < result)
		{
			result = sysData.trace[0].value[i];
		}
	}

	return result;
}

//设置最大值
int tSysScpi::setMarkerPeakMax(int mkrIndex, int traceIndex)
{
	if (mkrIndex < 0 || mkrIndex >= MARKERCOUNT || traceIndex < 0 || traceIndex >= TRACECOUNT || sysData.marker[mkrIndex].state == msClose)
	{
		return __SCPI_FAILED;
	}

	if (sysData.marker[mkrIndex].state == msNormal)
	{
		sysData.marker[mkrIndex].markerNormal.xIndex = getMarkerPeakMaxIndex(mkrIndex, traceIndex);
	} else
	{
		sysData.marker[mkrIndex].markerDelta.x1Index = getMarkerPeakMaxIndex(mkrIndex, traceIndex);
	}

	return __SCPI_SUCCESS;
}

//设置最小值
int tSysScpi::setMarkerPeakMin(int mkrIndex, int traceIndex)
{
	if (mkrIndex < 0 || mkrIndex >= MARKERCOUNT || traceIndex < 0 || traceIndex >= TRACECOUNT || sysData.marker[mkrIndex].state == msClose)
	{
		return __SCPI_FAILED;
	}

	if (sysData.marker[mkrIndex].state == msNormal)
	{
		sysData.marker[mkrIndex].markerNormal.xIndex = getMarkerPeakMinIndex(mkrIndex, traceIndex);
	} else
	{
		sysData.marker[mkrIndex].markerDelta.x1Index = getMarkerPeakMinIndex(mkrIndex, traceIndex);
	}

	return __SCPI_SUCCESS;
}

//设置下一峰值
int tSysScpi::setMarkerPeakNext(int mkrIndex, int traceIndex)
{
	return getMarkerPeakNextIndex(mkrIndex, traceIndex);
}

//设置左峰值
int tSysScpi::setMarkerPeakLeft(int mkrIndex, int traceIndex)
{
	return getMarkerPeakLeftIndex(mkrIndex, traceIndex);
}

//设置右峰值
int tSysScpi::setMarkerPeakRight(int mkrIndex, int traceIndex)
{
	return getMarkerPeakRightIndex(mkrIndex, traceIndex);
}

//峰值标记至中心频率
int tSysScpi::setMarkerPeakToCenterFreq(int mkrIndex, int traceIndex)
{
	return setMarkerToCenterFreq(mkrIndex);
}

//设置峰值最大值自动搜索
int tSysScpi::setMarkerPeakMaxAuto(int value)
{
	sysData.isAutoPeakMaxing = value == 1;

	return __SCPI_SUCCESS;
}

//设置峰值最大值自动搜索
int tSysScpi::setMarkerPeakMaxAuto(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	int tempValue = getBoolValue(value.toUpper().trimmed(), &isOk);

	if (isOk)
	{
		return setMarkerPeakMaxAuto(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//峰值最大值
int tSysScpi::getMarkerPeakMaxIndex(int mkrIndex, int traceIndex)
{
	int index = (sysData.sweep.sweepPoints - 1) / 2;

	if (mkrIndex < 0 || mkrIndex >= MARKERCOUNT || traceIndex < 0 || traceIndex >= TRACECOUNT || sysData.marker[mkrIndex].state == msClose)
	{
		return index;
	}

	int startIndex = 0;
	int stopIndex = sysData.sweep.sweepPoints;
	double startFreq = 0;
	double tempValue = 0;

	if (sysData.span.span > 0)
	{
		for (int i = startIndex; i < stopIndex; i++)
		{
			startFreq = sysData.freq.start - sysData.freq.offset + i * sysData.span.span / (sysData.sweep.sweepPoints - 1);

			if (startFreq >= 5 * sysData.bw.rbw)
			{
				startIndex = i;
				break;
			}
		}
	}

	for (int i = startIndex; i < stopIndex; i++)
	{
		if ((i == startIndex) || (i > startIndex && sysData.trace[traceIndex].value[i] > tempValue))
		{
			tempValue = sysData.trace[traceIndex].value[i];
			index = i;
		}
	}

	return index;
}

//峰值最小值
int tSysScpi::getMarkerPeakMinIndex(int mkrIndex, int traceIndex)
{
	int index = (sysData.sweep.sweepPoints - 1) / 2;

	if (mkrIndex < 0 || mkrIndex >= MARKERCOUNT || traceIndex < 0 || traceIndex >= TRACECOUNT || sysData.marker[mkrIndex].state == msClose)
	{
		return index;
	}

	int startIndex = 0;
	int stopIndex = sysData.sweep.sweepPoints;
	double tempValue = 0;

	for (int i = startIndex; i < stopIndex; i++)
	{
		if ((i == startIndex) || (i > startIndex && sysData.trace[traceIndex].value[i] < tempValue))
		{
			tempValue = sysData.trace[traceIndex].value[i];
			index = i;
		}
	}

	return index;
}

//下一峰值
int tSysScpi::getMarkerPeakNextIndex(int mkrIndex, int traceIndex)
{
	if (mkrIndex < 0 || mkrIndex >= MARKERCOUNT || traceIndex < 0 || traceIndex >= TRACECOUNT || sysData.marker[mkrIndex].state == msClose)
	{
		return __SCPI_FAILED;
	}

	if (getPeakList(traceIndex) == __SCPI_FAILED)
	{
		return __SCPI_FAILED;
	}

	double nowIndex = 0;
	double nowValue = 0;

	if (sysData.marker[mkrIndex].state == msNormal)
	{
		nowIndex = sysData.marker[mkrIndex].markerNormal.xIndex;
		nowValue = getTheMarkerYValue(mkrIndex, nowIndex);
	} else if (sysData.marker[mkrIndex].state == msDelta)
	{
		nowIndex = sysData.marker[mkrIndex].markerDelta.x1Index;
		nowValue = getTheMarkerYValue(mkrIndex, nowIndex);
	}

	int objIndex = -1;
	double objValue = 0;
	double theValue = 0;
	int flag = -1;

	for (int i = 0; i < posPeakList.count(); i++)
	{
		theValue = getTheMarkerYValue(mkrIndex, posPeakList[i]);

		if (posPeakList[i] != nowIndex && theValue <= nowValue)
		{
			if (flag == -1)
			{
				objIndex = posPeakList[i];
				objValue = theValue;
				flag++;

				if (flag > 0)
				{
					flag = 0;
				}
			} else
			{
				if (theValue > objValue)
				{
					objIndex = posPeakList[i];
					objValue = theValue;
				}
			}
		}
	}

	if (objIndex != -1)
	{
		if (sysData.marker[mkrIndex].state == msNormal)
		{
			sysData.marker[mkrIndex].markerNormal.xIndex = objIndex;
		} else if (sysData.marker[mkrIndex].state == msDelta)
		{
			sysData.marker[mkrIndex].markerDelta.x1Index = objIndex;
		}

		return __SCPI_SUCCESS;
	} else
	{
		return __SCPI_FAILED;
	}
}

//左峰值
int tSysScpi::getMarkerPeakLeftIndex(int mkrIndex, int traceIndex)
{
	if (mkrIndex < 0 || mkrIndex >= MARKERCOUNT || traceIndex < 0 || traceIndex >= TRACECOUNT || sysData.marker[mkrIndex].state == msClose)
	{
		return __SCPI_FAILED;
	}

	if (getPeakList(traceIndex) == __SCPI_FAILED)
	{
		return __SCPI_FAILED;
	}

	int nowIndex = 0;
	int objIndex = -1;

	if (sysData.marker[mkrIndex].state == msNormal)
	{
		nowIndex = sysData.marker[mkrIndex].markerNormal.xIndex;
	} else if (sysData.marker[mkrIndex].state == msDelta)
	{
		nowIndex = sysData.marker[mkrIndex].markerDelta.x1Index;
	}

	for (int i = posPeakList.count() - 1; i >= 0; i--)
	{
		if (posPeakList[i] < nowIndex)
		{
			objIndex = posPeakList[i];
			break;
		}
	}

	if (objIndex != -1)
	{
		if (sysData.marker[mkrIndex].state == msNormal)
		{
			sysData.marker[mkrIndex].markerNormal.xIndex = objIndex;
		} else if (sysData.marker[mkrIndex].state == msDelta)
		{
			sysData.marker[mkrIndex].markerDelta.x1Index = objIndex;
		}

		return __SCPI_SUCCESS;
	} else
	{
		return __SCPI_FAILED;
	}
}

//右峰值
int tSysScpi::getMarkerPeakRightIndex(int mkrIndex, int traceIndex)
{
	if (mkrIndex < 0 || mkrIndex >= MARKERCOUNT || traceIndex < 0 || traceIndex >= TRACECOUNT || sysData.marker[mkrIndex].state == msClose)
	{
		return __SCPI_FAILED;
	}

	if (getPeakList(traceIndex) == __SCPI_FAILED)
	{
		return __SCPI_FAILED;
	}

	int nowIndex = 0;
	int objIndex = -1;

	if (sysData.marker[mkrIndex].state == msNormal)
	{
		nowIndex = sysData.marker[mkrIndex].markerNormal.xIndex;
	} else if (sysData.marker[mkrIndex].state == msDelta)
	{
		nowIndex = sysData.marker[mkrIndex].markerDelta.x1Index;
	}

	for (int i = 0; i < posPeakList.count(); i++)
	{
		if (posPeakList[i] > nowIndex)
		{
			objIndex = posPeakList[i];
			break;
		}
	}

	if (objIndex != -1)
	{
		if (sysData.marker[mkrIndex].state == msNormal)
		{
			sysData.marker[mkrIndex].markerNormal.xIndex = objIndex;
		} else if (sysData.marker[mkrIndex].state == msDelta)
		{
			sysData.marker[mkrIndex].markerDelta.x1Index = objIndex;
		}

		return __SCPI_SUCCESS;
	} else
	{
		return __SCPI_FAILED;
	}
}

//寻找最接近的EMC带宽
double tSysScpi::getModifiedEmcBw(double value)
{
	double tempValue = value;
	int emcSize = sizeof EMC_BW / sizeof EMC_BW[0];

	if (tempValue >= EMC_BW[0])
	{
		tempValue = EMC_BW[0];
	} else if (tempValue <= EMC_BW[emcSize - 1])
	{
		tempValue = EMC_BW[emcSize - 1];
	} else
	{
		double tempValue1 = 0;
		double tempValue2 = 0;

		for (int i = 0; i < emcSize - 1; i++)
		{
			if (tempValue <= EMC_BW[i] && tempValue >= EMC_BW[i + 1])
			{
				tempValue1 = qAbs(tempValue - EMC_BW[i]);
				tempValue2 = qAbs(tempValue - EMC_BW[i + 1]);

				tempValue = (tempValue1 < tempValue2) ? EMC_BW[i] : EMC_BW[i + 1];
				break;
			}
		}
	}

	return tempValue;
}

//Buc Delay
int tSysScpi::getBucDelay(void)
{
	int bwIndex = getRbwIndex();
	int sBuc = 0;

	if ((sysData.span.span > 0) && (sysData.bw.rbw < 900))
	{
		if (bwIndex < 41)
		{
			sBuc = (int) (BUCDELAY[bwIndex - 25] / (sysData.sweep.sweepTime / 1e6 / sysData.sweep.sweepPoints));
		} else
		{
			sBuc = (int) (BUCDELAY[bwIndex - 25] * 1.25 / (sysData.sweep.sweepTime / 1e6 / sysData.sweep.sweepPoints)) + 2;
		}
	} else if ((sysData.span.span > 0) && (sysData.bw.rbw >= 900) && (sysData.bw.rbw <= 10e3))
	{
		sBuc = (int) (0.7 * (101 * 1000 * RAMCIC_NOFFT[bwIndex] / 30000000.0) / (sysData.sweep.sweepTime / 1e6 / sysData.sweep.sweepPoints));
	} else if ((sysData.span.span > 0) && (sysData.bw.rbw > 10e3))
	{
		sBuc = (int) (0.8 * (25.0 * 1000 * RAMCIC_NOFFT[bwIndex] / 30000000.0) / (sysData.sweep.sweepTime / 1e6 / sysData.sweep.sweepPoints));
	}

	if (sBuc > 60000)
	{
		sBuc = 60000;
	}

	return sBuc;
}

//获取频率起始索引
int tSysScpi::getStartLogFreqIndex(double value)
{
	unsigned long long tempValue = (unsigned long long) (value);

	for (int i = 0; i < sizeof LOGFREQLIST / sizeof LOGFREQLIST[0]; i++)
	{
		if (LOGFREQLIST[i] >= tempValue)
		{
			return i;
		}
	}

	return -1;
}

//获取频率终止索引
int tSysScpi::getStopLogFreqIndex(double value)
{
	unsigned long long tempValue = (unsigned long long) (value);

	if (tempValue >= LOGFREQLIST[sizeof LOGFREQLIST / sizeof LOGFREQLIST[0] - 1])
	{
		return sizeof LOGFREQLIST / sizeof LOGFREQLIST[0] - 1;
	}

	for (int i = 0; i < sizeof LOGFREQLIST / sizeof LOGFREQLIST[0] - 1; i++)
	{
		if ((tempValue >= LOGFREQLIST[i]) && (tempValue < LOGFREQLIST[i + 1]))
		{
			return i;
		}
	}

	return -1;
}

//恢复频响9kHz补偿数据
int tSysScpi::resetFrequencyRespondOf9kHz(double value)
{
	return modifyFrequencyRespondOf9kHz(RESP9KHZ);
}

//恢复频响20kHz补偿数据
int tSysScpi::resetFrequencyRespondOf20kHz(double value)
{
	return modifyFrequencyRespondOf20kHz(RESP20KHZ);
}

//修改频响9kHz补偿数据
int tSysScpi::modifyFrequencyRespondOf9kHz(QString value)
{
	bool isOk = false;
	double tempValue = 0;

	tempValue = value.toDouble(&isOk);

	if (isOk)
	{
		return modifyFrequencyRespondOf9kHz(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//修改频响9kHz补偿数据
int tSysScpi::modifyFrequencyRespondOf9kHz(double value)
{
	if (!sysData.factoryCalData.isCaled && !sysData.preamplifierCalData.isCaled)
	{
		return __SCPI_FAILED;
	}

	char tempChar[32] = { };
	QString fileName = QCoreApplication::applicationDirPath() + "/setting.ini";
	QSettings setting(fileName, QSettings::IniFormat);

	if (sysData.ampt.isPreamptOn)
	{
		setting.beginGroup("preamplifierCalData");

		for (int i = 0; i < 5; i++)
		{
			for (int j = 0; j < 1; j++)
			{
				sysData.preamplifierCalData.freqResp[i][j] = value;
				setting.setValue("freqResp_" + QString(intToString(i, tempChar)).trimmed() + "_" + QString(intToString(j, tempChar)).trimmed(), sysData.preamplifierCalData.freqResp[i][j]);
			}
		}
		setting.endGroup();
	} else
	{
		setting.beginGroup("factoryData");

		for (int i = 0; i < 5; i++)
		{
			for (int j = 0; j < 1; j++)
			{
				sysData.factoryCalData.freqResp[i][j] = value;
				setting.setValue("freqResp_" + QString(intToString(i, tempChar)).trimmed() + "_" + QString(intToString(j, tempChar)).trimmed(), sysData.factoryCalData.freqResp[i][j]);
			}
		}

		setting.endGroup();
	}

	return __SCPI_SUCCESS;
}

//修改频响20kHz补偿数据
int tSysScpi::modifyFrequencyRespondOf20kHz(QString value)
{
	bool isOk = false;
	double tempValue = 0;

	tempValue = value.toDouble(&isOk);

	if (isOk)
	{
		return modifyFrequencyRespondOf20kHz(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}
//修改频响20kHz补偿数据
int tSysScpi::modifyFrequencyRespondOf20kHz(double value)
{
	if (!sysData.factoryCalData.isCaled && !sysData.preamplifierCalData.isCaled)
	{
		return __SCPI_FAILED;
	}

	char tempChar[32] = { };
	QString fileName = QCoreApplication::applicationDirPath() + "/setting.ini";
	QSettings setting(fileName, QSettings::IniFormat);

	if (sysData.ampt.isPreamptOn)
	{
		setting.beginGroup("preamplifierCalData");

		for (int i = 0; i < 5; i++)
		{
			for (int j = 1; j < 2; j++)
			{
				sysData.preamplifierCalData.freqResp[i][j] = value;
				setting.setValue("freqResp_" + QString(intToString(i, tempChar)).trimmed() + "_" + QString(intToString(j, tempChar)).trimmed(), sysData.preamplifierCalData.freqResp[i][j]);
			}
		}
		setting.endGroup();
	} else
	{
		setting.beginGroup("factoryData");

		for (int i = 0; i < 5; i++)
		{
			for (int j = 1; j < 2; j++)
			{
				sysData.factoryCalData.freqResp[i][j] = value;
				setting.setValue("freqResp_" + QString(intToString(i, tempChar)).trimmed() + "_" + QString(intToString(j, tempChar)).trimmed(), sysData.factoryCalData.freqResp[i][j]);
			}
		}
		setting.endGroup();
	}

	return __SCPI_SUCCESS;
}

//方向键修改频响9kHz补偿数据
int tSysScpi::modifyFrequencyRespondOf9kHzWithArrow(bool up)
{
	double tempValue = 0.0;
	if (sysData.ampt.isPreamptOn)
	{
		tempValue = sysData.preamplifierCalData.freqResp[0][0];
	} else
	{
		tempValue = sysData.factoryCalData.freqResp[0][0];
	}

	if (up)
	{
		return modifyFrequencyRespondOf9kHz(tempValue + 1.0);
	} else
	{
		return modifyFrequencyRespondOf9kHz(tempValue - 1.0);
	}
}

//脉冲旋钮修改频响9kHz补偿数据
int tSysScpi::modifyFrequencyRespondOf9kHzWithRpg(int value)
{
	if (sysData.ampt.isPreamptOn)
	{
		return modifyFrequencyRespondOf9kHz(sysData.preamplifierCalData.freqResp[0][0] + 0.1 * value);
	} else
	{
		return modifyFrequencyRespondOf9kHz(sysData.factoryCalData.freqResp[0][0] + 0.1 * value);
	}
}

//方向键修改频响20kHz补偿数据
int tSysScpi::modifyFrequencyRespondOf20kHzWithArrow(bool up)
{
	double tempValue = 0.0;
	if (sysData.ampt.isPreamptOn)
	{
		tempValue = sysData.preamplifierCalData.freqResp[0][1];
	} else
	{
		tempValue = sysData.factoryCalData.freqResp[0][1];
	}

	if (up)
	{
		return modifyFrequencyRespondOf20kHz(tempValue + 1.0);
	} else
	{
		return modifyFrequencyRespondOf20kHz(tempValue - 1.0);
	}
}

//脉冲旋钮修改频响20kHz补偿数据
int tSysScpi::modifyFrequencyRespondOf20kHzWithRpg(int value)
{
	if (sysData.ampt.isPreamptOn)
	{
		return modifyFrequencyRespondOf20kHz(sysData.preamplifierCalData.freqResp[0][1] + 0.1 * value);
	} else
	{
		return modifyFrequencyRespondOf20kHz(sysData.factoryCalData.freqResp[0][1] + 0.1 * value);
	}
}
//修改频段索引值
void tSysScpi::modifyBandIndex(void)
{
	int i = 0;
	unsigned int band2_start_index = rfData.N_L;
	unsigned int band3_start_index = rfData.N_L + rfData.N_HL;
	unsigned int band4_start_index = rfData.N_L + rfData.N_HL + rfData.N_HH1;
	unsigned int band5_start_index = rfData.N_L + rfData.N_HL + rfData.N_HH1 + rfData.N_HH2;
	unsigned int band6_start_index = rfData.N_L + rfData.N_HL + rfData.N_HH1 + rfData.N_HH2 + rfData.N_HH3;
	unsigned int band7_start_index = band6_start_index + rfData.N_HH4;
	for (i = 0; i < sysData.sweep.sweepPoints; i++)
	{
//		if (i < band2_start_index)
//		{
//			freqIndex[i].tIndex1 = 0;
//			freqIndex[i].tIndex2 = 37;
//		} else
		if ((i >= band2_start_index) && (i < band3_start_index))
		{
			freqIndex[i].tIndex1 = 0;
			freqIndex[i].tIndex2 = 162;
		} else if ((i >= band3_start_index) && (i < band4_start_index))
		{
			freqIndex[i].tIndex1 = 163;
			freqIndex[i].tIndex2 = 215;
		} else if (i >= band4_start_index && i < band5_start_index)
		{
			freqIndex[i].tIndex1 = 216;
			freqIndex[i].tIndex2 = 268;
		} else if(i >= band5_start_index && i < band6_start_index)
		{
			freqIndex[i].tIndex1 = 269;
			freqIndex[i].tIndex2 = 321;
		} else if(i >= band6_start_index)
		{
			freqIndex[i].tIndex1 = 322;
			freqIndex[i].tIndex2 = 373;
		}
	}
}

void tSysScpi::freqScanStart(void)
{
	setScanEnable(true);
	restartScan();
}

//扫描和零扫宽功能选择
void tSysScpi::setFreqSweepStatus(void)
{
	char rbwSel = 0, sweepMode = 0, workMode = 0;

	if(sysData.bw.rbw >= 1e6 && sysData.bw.rbw <= 5e6)
		rbwSel = 0;
	else if(sysData.bw.rbw >= 100e3 && sysData.bw.rbw <= 500e3)
		rbwSel = 1;
	else if(sysData.bw.rbw >= 1e3 && sysData.bw.rbw <= 50e3)
		rbwSel = 2;
	else if (sysData.bw.rbw >= 10 && sysData.bw.rbw <= 500)
		rbwSel = 3;

	if(sysData.fscan.mode == fsFFT)
		sweepMode = 1;
	else
		sweepMode = 0;

	if(sysData.span.isZeroSpan)
		workMode = 1;
	else
		workMode = 0;

	feDownload(40, (rbwSel | (sweepMode << 2) | (workMode << 3)));
}

/*衰减器的控制参数下发*/
char tSysScpi::getZcAttVal(void)
{
	char ifAttDown = 0, rfAttDown = 0, temp = 0;

	if(sysData.ampt.attIf <= 0)
	{
		ifAttDown = 0;
		sysData.ampt.dsattIf = 0;
	}
	else if(sysData.ampt.attIf <= 10)
	{
		ifAttDown = 2;
		sysData.ampt.dsattIf = 10;
	}
	else if(sysData.ampt.attIf <= 20)
	{
		ifAttDown = 3;
		sysData.ampt.dsattIf = 20;
	}
	else if(sysData.ampt.attIf <= 31)
	{
		ifAttDown = 1;
		sysData.ampt.dsattIf = 30;
	}

	if(sysData.ampt.attRf <= 0)
	{
		rfAttDown = 0;
		sysData.ampt.dsattRf = 0;
	}
	else if(sysData.ampt.attRf <= 10)
	{
		rfAttDown = 2;
		sysData.ampt.dsattRf = 10;
	}
	else if(sysData.ampt.attRf <= 20)
	{
		rfAttDown = 3;
		sysData.ampt.dsattRf = 20;
	}
	else if(sysData.ampt.attRf <= 31)
	{
		rfAttDown = 1;
		sysData.ampt.dsattRf = 30;
	}

	temp = (ifAttDown << 2 | rfAttDown) & 0x0f;

	return temp;

}

//射频控制
void tSysScpi::controlRf(void)
{
	bwAutoCouple();

	setScanEnable(false);    //停止扫描

	feDownload(100, 1);
	feDownload(100, 0);		//系统复位,addr_100 1 to 0

	updateScanParam(); //扫描参数计算

	setFreqSweepStatus();     //零扫宽和扫描模式选择

	feDownload(207, 0);         //本振送数开关

	//serial data 
	//setFrontEndIfAtt(sysData.ampt.attIf);  //中频衰减器
	//setLowbandAtt(sysData.ampt.attRf);     //低波段衰减器
	//setHighbandAtt(sysData.ampt.attRf);    //高波段衰减器
	feDownload(0xfff6,(int)(sysData.ampt.attRf * 4));
	rfPreamptDown(sysData.ampt.isPreamptOn); //前置放大选择
	updateBandParam();         //本振参数计算

	fftControl();
	
	sweepTimeDown();            //采样驻留时间

	setFTWOfDDS();             //DDS下发

	feDownload(0xfff8, 0);     //一本初始化
	//feDownload(0xfff9, 0);     //二本初始化
	downLo1();                  //本振下发
	setSweepBandIndex();        //扫描波段指示

	setHighBandIndex();         //高波段指示
	
	feDownload(207, 1);       //本振送数开关

	//data to FPGA
	//feDownload(234, calHighbandAttValueOfFrontend(sysData.ampt.attRf));     //高波段衰减值
	//feDownload(235, calLowbandAttValueOfFrontend(sysData.ampt.attRf));      //低波段衰减值
	feDownload(236, calIfAttValueOfFrontend(sysData.ampt.attIf));           //中频段衰减值
	feDownload(237, getPremptData(sysData.ampt.isPreamptOn));	//前放
	
	if(sysData.span.isZeroSpan)
	{
		feDownload(207, 0);	
		setFrontEndFreq(sysData.freq.center);
	}

	feDownload(248, getZcAttVal());				  //直采衰减器


	cicDown();									//CIC滤波器设置

	vbwDown();

	//检波控制
	controlIfDetector();

	//modifyBandIndex();

	feDownload(100, 1);
	feDownload(100, 0);		//系统复位

	setSweepOfMode(sysData.sweep.sweepSingle);  //扫描模式选择

	freqScanStart();          //扫描开始

}

//获取最近的带宽
double tSysScpi::getRecentBw(double value)
{
	int index = 0,i;

	index = sizeof RAMCIC_HZ / sizeof RAMCIC_HZ[0]-1;
	if(value > RAMCIC_HZ[0])
		return (double)RAMCIC_HZ[0]; //max
	else if (value < RAMCIC_HZ[index])
		return (double)RAMCIC_HZ[index];//min
	else
	{
		for (i=0; i<index; i++)
		{
	  //return a value colse to bigger integer in RAMCIC_HZ or a value equal to element in RAMCIC_HZ
			if ((value < RAMCIC_HZ[i] && value > RAMCIC_HZ[i+1]) || value == RAMCIC_HZ[i]) 
			 return (double)RAMCIC_HZ[i];
		}
		if (value == RAMCIC_HZ[i])
			 return (double)RAMCIC_HZ[i];
	}



}

//获取最近的带宽
double tSysScpi::getRecentVBw(double value)
{
	double error = 0;
	int index = 0;

	//完全匹配
	for (int i = 0; i < sizeof RAMCIC_VBWHZ / sizeof RAMCIC_VBWHZ[0]; i++)
	{
		if (RAMCIC_VBWHZ[i] == (unsigned long) value)
		{
			return RAMCIC_VBWHZ[i];
		}
	}

	//匹配接近
	index = 0;
	error = qAbs(value - RAMCIC_VBWHZ[0]);

	for (int i = 1; i < sizeof RAMCIC_VBWHZ / sizeof RAMCIC_VBWHZ[0]; i++)
	{
		if (qAbs(value - RAMCIC_VBWHZ[i]) < error)
		{
			error = qAbs(value - RAMCIC_VBWHZ[i]);
			index = i;
		}
	}

	return RAMCIC_VBWHZ[index];
}

//获取扫描时间系数
int tSysScpi::getSweepTimeQuieoty(void)
{
	int rIndex = 0;
	int vIndex = 0;
	int ratio = 1;

	for (int i = 0; i < sizeof RAMCIC_VBWHZ / sizeof RAMCIC_VBWHZ[0] - 1; i++)
	{
		if ((sysData.bw.rbw <= RAMCIC_VBWHZ[i]) && (sysData.bw.rbw > RAMCIC_VBWHZ[i + 1]))
		{
			rIndex = i;
			break;
		}
	}

	for (int i = 0; i < sizeof RAMCIC_VBWHZ / sizeof RAMCIC_VBWHZ[0] - 1; i++)
	{
		if ((sysData.bw.vbw <= RAMCIC_VBWHZ[i]) && (sysData.bw.vbw > RAMCIC_VBWHZ[i + 1]))
		{
			vIndex = i;
			break;
		}
	}

	ratio = 1.0;

	if ((rIndex - vIndex) <= 0)
	{
		ratio = vIndex - rIndex;

		if (ratio >= 7)
		{
			ratio = 7;
		}
	}

	return ratio;
}

//获取视频偏移
void tSysScpi::getVbwOffset(void)
{
	double leftValue = 0;
	double rightValue = 0;
	double tempValue = 0;

	tempValue = sysData.bw.vbw;

	for (int i = 0; i < sizeof RAMCIC_VBWHZ / sizeof RAMCIC_VBWHZ[0] - 1; i++)
	{
		if ((tempValue <= RAMCIC_VBWHZ[i]) && (tempValue > RAMCIC_VBWHZ[i + 1]))
		{
			leftValue = qAbs(RAMCIC_VBWHZ[i] - sysData.bw.vbw);
			rightValue = qAbs(RAMCIC_VBWHZ[i + 1] - sysData.bw.vbw);

			if (leftValue <= rightValue)
			{
				tempValue = RAMCIC_VBWHZ[i];
			} else
			{
				tempValue = RAMCIC_VBWHZ[i + 1];
			}

			if (!sysData.options.isFft10HzOn)
			{
				if (tempValue < 10.0)
				{
					tempValue = 10.0;
				}
			}

			sysData.bw.vbw = tempValue;

			break;
		}
	}
}

//获取视频带宽
void tSysScpi::getModifiedVbw(int arrow)
{
	int bwSize = sizeof RAMCIC_VBWHZ / sizeof RAMCIC_VBWHZ[0];
	double tempValue = sysData.bw.vbw;

	for (int i = 0; i < bwSize - 1; i++)
	{
		if ((arrow > 0) && (tempValue < RAMCIC_VBWHZ[i]) && (tempValue >= RAMCIC_VBWHZ[i + 1]))
		{
			sysData.bw.vbw = RAMCIC_VBWHZ[i];
			break;
		}

		if ((arrow < 0) && (tempValue <= RAMCIC_VBWHZ[i]) && (tempValue > RAMCIC_VBWHZ[i + 1]))
		{
			sysData.bw.vbw = RAMCIC_VBWHZ[i + 1];
			//
			//	  if(!sysData.options.isFft10HzOn)
			//	  {
			//	    if (sysData.bw.vbw < 10.0)
			//	    {
			//		  sysData.bw.vbw = 10.0;
			//	    }
			//	  }

			//fft onoff rbw & vbw
			if (sysData.bw.rbw >= 1e3) //fft模式时候 VBW可以达到1Hz
			{
				if (sysData.bw.vbw < 10.0)
				{
					sysData.bw.vbw = 10.0;
				}
			}

			break;
		}
	}
}

//带宽自动耦合
void tSysScpi::bwAutoCouple(void)
{

	double k = 120.0; // factor of span

	if (sysData.bw.rbwAuto && sysData.span.span != 0)
	{
		sysData.bw.rbw = getRecentBw(sysData.span.span / k);
		if (sysData.bw.vbwAuto)
		{
			sysData.bw.vbw = sysData.bw.rbw;

			if ((unsigned long) sysData.bw.rbw == 120e3)
			{
				sysData.bw.vbw = 100e3;
			}
		}
	}

}

//获取幅度上下限
double tSysScpi::getAmptMaxMinLimit(void)
{
	double minValue = MINREFLEVELDBM + sysData.ampt.refOffset;
	double maxValue = 0;

	if (sysData.ampt.isPreamptOn)
	{
		maxValue = MAXREFLEVELDBM + sysData.ampt.refOffset - 20.0;
	} else
	{
		maxValue = MAXREFLEVELDBM + sysData.ampt.refOffset;
	}

	if (sysData.system.isInternalSignalOn)
	{
		maxValue = 0 + sysData.ampt.refOffset;
	}

	if (sysData.trig.mode == tgVideo)
	{
		minValue = VIDEOTRIGPOWERMIN + sysData.ampt.refOffset;
	}

	switch (sysData.ampt.refUnit)
	{
		case rudBm:
			break;
		case rudBpW:
			minValue += 90.0;
			maxValue += 90.0;
			break;
		case rudBuW:
			minValue += 30.0;
			maxValue += 30.0;
			break;
		case rudBmV:
			minValue += 47.0;
			maxValue += 47.0;
			break;
		case rudBuV:
		case rudBuVm:
			minValue += 107.0;
			maxValue += 107.0;
			break;
		case ruW:
			minValue = qPow(10, 0.1 * minValue);
			maxValue = qPow(10, 0.1 * maxValue);
			break;
		case ruV:
			minValue = qSqrt(50000 * qPow(10, 0.1 * minValue));
			maxValue = qSqrt(50000 * qPow(10, 0.1 * maxValue));
			break;
		default:
			break;
	}
	sysData.ampt.refLevelMin = minValue;
	sysData.ampt.refLevelMax = maxValue;
}

//获取功率上下限
void tSysScpi::getTheAmptMaxMinLimit(double minValue, double maxValue, double* min, double* max)
{
	switch (sysData.ampt.refUnit)
	{
		case rudBmV:
			*min = minValue + 47.0;
			*max = maxValue + 47.0;
			break;
		case rudBpW:
			*min = minValue + 90.0;
			*max = maxValue + 90.0;
			break;
		case rudBuW:
			*min = minValue + 30.0;
			*max = maxValue + 30.0;
			break;
		case rudBuV:
		case rudBuVm:
			*min = minValue + 107.0;
			*max = maxValue + 107.0;
			break;
		case ruW:
			*min = qPow(10, 0.1 * minValue);
			*max = qPow(10, 0.1 * maxValue);
			break;
		case ruV:
			*min = qSqrt(50000 * qPow(10, 0.1 * minValue));
			*max = qSqrt(50000 * qPow(10, 0.1 * maxValue));
			break;

		default:
			*min = minValue;
			*max = maxValue;
			break;
	}
}

//获取衰减和单位字符串
char* tSysScpi::getAttAndUnitString(double value, char* result, int full)
{
	char temp[32];
	sprintf(temp, "%.2f dB", value);
	strcpy(result, temp);
	return result;
}

//获取幅度和单位字符串
char* tSysScpi::getAmptAndUnitString(double value, char* result, int full)
{
	double tempValue = value;
	char temp[32];
	int brev = 0;

	if ((full > 0) && ((long) value == value))
	{
		brev = 1;
	}

	if (sysData.disc.isDiscOn)
	{
		if (brev == 1)
		{
			sprintf(temp, "%16.0f V", tempValue);
		} else
		{
			sprintf(temp, "%16.2f V", tempValue);
		}
	} else

		switch (sysData.ampt.refUnit)
		{
			case rudBm:
				if (brev == 1)
				{
					sprintf(temp, "%16.0f dBm", tempValue);
				} else
				{
					sprintf(temp, "%16.2f dBm", tempValue);
				}
				break;
			case rudBpW:
				if (brev == 1)
				{
					sprintf(temp, "%16.0f dBpW", tempValue);
				} else
				{
					sprintf(temp, "%16.2f dBpW", tempValue);
				}
				break;

			case rudBuW:
				if (brev == 1)
				{
					sprintf(temp, "%16.0f dBuW", tempValue);
				} else
				{
					sprintf(temp, "%16.2f dBuW", tempValue);
				}
				break;

			case rudBmV:
				if (brev == 1)
				{
					sprintf(temp, "%16.0f dBmV", tempValue);
				} else
				{
					sprintf(temp, "%16.2f dBmV", tempValue);
				}
				break;
			case rudBuV:
				if (brev == 1)
				{
					sprintf(temp, "%16.0f dBuV", tempValue);
				} else
				{
					sprintf(temp, "%16.2f dBuV", tempValue);
				}
				break;
			case ruW:
				if (tempValue < 1e-3)
				{
					if (brev == 1)
					{
						sprintf(temp, "%16.0f nW", tempValue / 1e-6);
					} else
					{
						sprintf(temp, "%16.2f nW", tempValue / 1e-6);
					}
				} else if (tempValue >= 1e-3 && tempValue < 1)
				{
					if (brev == 1)
					{
						sprintf(temp, "%16.0f uW", tempValue / 1e-3);
					} else
					{
						sprintf(temp, "%16.2f uW", tempValue / 1e-3);
					}
				} else if (tempValue >= 1 && tempValue < 1e3)
				{
					if (brev == 1)
					{
						sprintf(temp, "%16.0f mW", tempValue);
					} else
					{
						sprintf(temp, "%16.2f mW", tempValue);
					}
				} else if (tempValue >= 1e3)
				{
					if (brev == 1)
					{
						sprintf(temp, "%16.0f W", tempValue / 1e3);
					} else
					{
						sprintf(temp, "%16.2f W", tempValue / 1e3);
					}
				}
				break;
			case ruV:
				if (tempValue < 1e-3)
				{
					if (brev == 1)
					{
						sprintf(temp, "%16.0f nV", tempValue / 1e-6);
					} else
					{
						sprintf(temp, "%16.2f nV", tempValue / 1e-6);
					}
				} else if (tempValue >= 1e-3 && tempValue < 1)
				{
					if (brev == 1)
					{
						sprintf(temp, "%16.0f uV", tempValue / 1e-3);
					} else
					{
						sprintf(temp, "%16.2f uV", tempValue / 1e-3);
					}
				} else if (tempValue >= 1 && tempValue < 1e3)
				{
					if (brev == 1)
					{
						sprintf(temp, "%16.0f mV", tempValue);
					} else
					{
						sprintf(temp, "%16.2f mV", tempValue);
					}
				} else if (tempValue >= 1e3)
				{
					if (brev == 1)
					{
						sprintf(temp, "%16.0f V", tempValue / 1e3);
					} else
					{
						sprintf(temp, "%16.2f V", tempValue / 1e3);
					}
				}

				break;
			case ruuV:
				if (brev == 1)
				{
					sprintf(temp, "%16.0f uV", tempValue);
				} else
				{
					sprintf(temp, "%16.2f uV", tempValue);
				}

			case rudBuVm:
				if (brev == 1)
				{
					sprintf(temp, "%16.0f dBuV/m", tempValue);
				} else
				{
					sprintf(temp, "%16.2f dBuV/m", tempValue);
				}
				break;
			default:
				if (brev == 1)
				{
					sprintf(temp, "%16.0f unKnown", tempValue);
				} else
				{
					sprintf(temp, "%16.2f unKnown", tempValue);
				}
				break;
		}

	strcpy(result, temp);
	return result;
}

//中频控制
void tSysScpi::controlIf(void)
{
	int SBUC = 5;
	int SBUC_D = 0;

	//sweep points
	ifDownload(0xfb, sysData.sweep.sweepPoints);

	//sbuc
	if (sysData.span.span > 0)
	{
		SBUC = 30;
	}

	SBUC_D = 65536 - SBUC;
	ifDownload(0x1a, ((SBUC_D >> 8) & 0xff));
	ifDownload(0x1a, (SBUC_D & 0xff));
	//rbw
	int rbwIndex = getRbwIndex();

	if (sysData.emc.emcOn && (unsigned long) sysData.bw.rbw == 120e3)
	{
		rbwIndex = 6;
	}

	//  //vbw
	//  if (sysData.bw.vbwAuto)
	//  {
	//    ifDownload(0x08, RAMCVC[0]);
	//  } else
	//  {
	//    ifDownload(0x08, RAMCVC[getVbwModIndex()]);
	//  }

	//vbw 提速
	if (sysData.bw.vbw > 10e3)
	{
		ifDownload(0x1b, 0x80);
	} else
	{
		ifDownload(0x1b, 0x40);
	}

	unsigned long vbwCvc = 0;

	if (sysData.bw.vbwAuto)
	{
		vbwCvc = RAMCVC[0];
	} else
	{
		vbwCvc = getVbwFactor(sysData.bw.vbw);
	}


	//  unsigned long vbwCvc = RAMCVC[getVbwIndex()];
	//  ifDownload(0x08, RAMCVC[getVbwIndex()]);
	ifDownload(0x08, (vbwCvc >> 8) & 0xff);
	ifDownload(0x08, (vbwCvc) & 0xff);

	//ADRS（21）：视频滤波因子装载地址；（当视频滤波因子改变时，D7位先送“1”再送“0”）
	ifDownload(0x15, 0xff);
	ifDownload(0x15, 0x00);

	long int crcclk;
	int shift = (int) (5.0 * log10(double(RAMCIC_NOFFT[rbwIndex])) / log10(2));
	if (isRequireFFTMeas())
	{
		shift = (int) (5.0 * log10(double(RAMCIC[rbwIndex])) / log10(2)) + 4;
	} else if (isDirectGatherMode())
	{
		shift = (int) (5.0 * log10(double(RAMCIC_ZC[rbwIndex])) / log10(2)) + 4;
	}

	int emcIndex = 0;

	if (sysData.emc.emcOn)
	{
		for (int i = 0; i < sizeof EMC_BW / sizeof EMC_BW[0]; i++)
		{
			if ((unsigned long) sysData.bw.rbw == EMC_BW[i])
			{
				shift = (int) (5.0 * log10(double(EMC_CIC[i])) / log10(2));
				emcIndex = i;
				break;
			}
		}
	}

	if (shift >= 74) //old is 63
	{
		shift = 74;
	}
	ifDownload(0x06, shift);

	// CIC CLK
	crcclk = (~RAMCIC_NOFFT[rbwIndex]) + 1;
	if (sysData.emc.emcOn)
	{
		crcclk = (~EMC_CIC[emcIndex]) + 1;
	}
	if (isRequireFFTMeas())
	{
		crcclk = (~RAMCIC[rbwIndex]) + 1;
	}
	if (isDirectGatherMode())
	{
		crcclk = (~RAMCIC_ZC[rbwIndex]) + 1;
	}

	ifDownload(0x03, (crcclk & 0xff));
	ifDownload(0x04, (crcclk >> 8) & 0xff);
	ifDownload(0x05, (crcclk >> 16) & 0xff);

	//CIC RST & LOAD
	//ADRS（7）：CIC抽取滤波和FIR滤波控制
	unsigned char a7 = 0;
	// a7 = 0x01 & 0xf2;
	ifDownload(0x07, 0x01);
	//a7 = 0x12 & 0xf2;
	ifDownload(0x07, 0x12);

	if (sysData.bw.rbw >= 2e3)
	{
		rfDownload(0x64, 0);
	} else
	{
		rfDownload(0x64, 1);
	}

	if (rbwIndex <= 1) //RBW为3MHz和1MHz；
	{
		a7 = 0x00 & 0xf2;
		ifDownload(0x07, a7);
		sysData.sweep.sweepDelay = 3000;
	} else if (rbwIndex <= 14) //RBW为11kHz~500kHz；
	{
		a7 = 0x10 & 0xf2;
		ifDownload(0x07, a7);
		sysData.sweep.sweepDelay = 3000;
	} else if (rbwIndex <= 25) //RBW为900Hz~10kHz
	{
		a7 = 0x20 & 0xf2;
		ifDownload(0x07, a7);
		sysData.sweep.sweepDelay = 10000;
	} else if (rbwIndex <= 41)
	{
		a7 = 0x30 & 0xf2;
		ifDownload(0x07, a7);//RBW为20Hz~800Hz
		sysData.sweep.sweepDelay = 50000;
	} else
	{
		a7 = 0x30 & 0xf2;
		ifDownload(0x07, a7);//RBW为10Hz
		sysData.sweep.sweepDelay = 100000;
	}

	//VBW 第一次下发延时
	if (sysData.bw.vbw > 10e3)
	{
		sysData.sweep.sweepDelay = 10000;
	} else if (sysData.bw.vbw > 100)
	{
		sysData.sweep.sweepDelay = 50000;
	} else
	{
		sysData.sweep.sweepDelay = 100000;
	}

	//sample id
	controlIfDetector();

	//中频增益
	int ifDown = 0x0;
	int ifZs = 0;
	int ifXs = 0;

	ifZs = (int) sysData.ampt.ifValue / 10;
	ifXs = (int) sysData.ampt.ifValue % 10;

	//四舍五入处理
	if (sysData.ampt.ifValue <= MAXIF)
	{
		if (ifXs >= 5)
		{
			ifZs++;
			ifXs = 0;
		}
	}

	getIfSoftValue();

	switch (ifZs)
	{
		case 0:
			ifDown = 0x08;
			break;
		case 1:
			ifDown = 0x00;
			break;
		case 2:
			ifDown = 0x18;
			break;
		case 3:
			ifDown = 0x10;
			break;
		default:
			ifDown = 0x10;
			break;
	}

	//Buc Delay
	if (isRequireFFTMeas())
	{
		ifDownload(0xfc, 0);
	} else
	{
		ifDownload(0xfc, getBucDelay());
	}

	//freq count
	if (sysData.markerFctn.isFreqCountOn)
	{
		unsigned long freqBwCount = 16777216 - 1e7 / sysData.markerFctn.freqCountBw;

		ifDownload(0x1a, (freqBwCount & 0xff0000) >> 16);
		ifDownload(0x1a, (freqBwCount & 0xff00) >> 8);
		ifDownload(0x1a, (freqBwCount & 0xff) >> 0);
		ifDownload(0x1c, 0x01);
	} else
	{
		ifDownload(0x1c, 0x00);
	}

	//零扫宽控制、中频增益 & 音频解调 & 触发
	bool isZeroSpan;

	if (sysData.span.span == 0)
	{
		isZeroSpan = true;
	} else
	{
		isZeroSpan = false;
	}

	rfDownload(0x54, (sysData.trig.trigDown << 4) | (isZeroSpan << 3) | (sysData.demod.demodOn << 2) | (ifDown >> 3));

	if (!isDirectGatherMode())
	{
		unsigned char trigDown = 0;

		if (sysData.trig.mode == tgRise || sysData.trig.mode == tgFall)
		{
			trigDown = 1;
		}

		rfDownload(0xf9, (trigDown << 4) | (0 << 3)); //外部触发控制
	}
	//  rfDownload(0x54, (0x01 << 4) | (isZeroSpan << 3) |(sysData.demod.demodOn << 2) | (ifDown >> 3));
	//解调音量
	ifDownload(0x1e, (unsigned char) (sysData.demod.sound * 2.55));
}

//中频检波控制
void tSysScpi::controlIfDetector(void)
{
	//fft reset
	//feDownload(15, 1);
	//usleep(1);
	//feDownload(15, 0);

	char detMode = 0;
	int interpRate = 0, shiftRate = 0;
	//if(sysData.fscan.fftRealN < 400)
	//{
	//	detMode = 5;
	//} else 
	if(sysData.fscan.fftRealN <= 1600)
	{
		detMode = 0;
	} else
	{
		switch(sysData.detector.detMode)
		{
			case dmPosPeak:  detMode = 1;break;
			case dmNegPeak:  detMode = 2;break;
			case dmSample:	 detMode = 3;break;
			case dmAverage:  detMode = 4;break;
			case dmAvevalid: detMode = 5;break;
			default: 		 detMode = 0;break;
		}
	}

	//FFT 检波方式
	feDownload(27, detMode&0x07);
	if(sysData.fscan.fftRealN < 400)
	{
		feDownload(49, 1);
		
		//CIC 插值倍数
		if(sysData.fscan.fftRealN == 1)
			interpRate = 1600;
		else
			interpRate = floor(1600.0 / (sysData.fscan.fftRealN - 1));
		shiftRate = ceil(3.0 * log10(interpRate) / log10(2));
		feDownload(43, interpRate & 0xff);
		feDownload(44, shiftRate & 0xff);
		feDownload(45, 1);
		usleep(1);
		feDownload(45, 0);
	}
	else
	{
		feDownload(49, 0);
	}

	//检波实际长度
	if(sysData.fscan.fftRealN < 400)
	{
		if(sysData.fscan.fftRealN == 1)
			sysData.detector.realLength = 1600;
		else
			sysData.detector.realLength = (sysData.fscan.fftRealN - 1) * interpRate;
	} else
	{
		sysData.detector.realLength = sysData.fscan.fftRealN;
	}

	//检波数据长度
//	if(sysData.fscan.mode == fsSweep)
//  {
//		if(rfData.DBUC_FREQ < 250e3)
//			sysData.detector.length = 5;
//		else
//			sysData.detector.length = ceil(rfData.DBUC_FREQ / 50000);
//  } else
//  {
//  	sysData.detector.length = ceil(sysData.detector.realLength / 1600.0);
//  }
//
//	if(sysData.span.isZeroSpan)
//		sysData.detector.length = ceil(sysData.fscan.fftRealN / 1600.0);
	sysData.detector.length = ceil(sysData.detector.realLength / 1600.0);

	if(sysData.detector.length == 0)
		return;

	feDownload(28, (sysData.detector.length >> 16) & 0xff);
	feDownload(29, sysData.detector.length & 0xffff);

	if(sysData.fscan.fftRealN <= 1600)
		feDownload(39, 0);
	else
		feDownload(39, 1);

	//检波后数据长度
//	if(sysData.fscan.mode == fsSweep)
//	{
//		sysData.detector.ldata = RAMDATASIZE;
//	} else
	{
		sysData.detector.ldata = ceil(sysData.detector.realLength * 1.0 / sysData.detector.length);
	}
	feDownload(30, sysData.detector.ldata);

	//均值检波和均方根值检波 常规量化数据
	unsigned int DaverData = round((pow2_32 - 1) * 1.0 / sysData.detector.length);
	feDownload(31, (DaverData >> 16) & 0xffff);
	feDownload(32, DaverData & 0xffff);

	//均值检波和均方根值检波 尾数量化数据
	unsigned int IaverData = 0;
	if((sysData.detector.realLength - floor(sysData.detector.realLength / sysData.detector.length) * sysData.detector.length) == 0)
		IaverData = DaverData;
	else
		IaverData = round((pow2_32 - 1) * 1.0 / (sysData.detector.realLength - floor(sysData.detector.realLength / sysData.detector.length) * sysData.detector.length));

	feDownload(33, (IaverData >> 16) & 0xffff);
	feDownload(34, IaverData & 0xffff);
}

//关闭源发生器
void tSysScpi::closeSourceGenerator()
{
	closeSourceTrackGenerator();
	closeSourceSignalGenerator();
	rfDownload(0xfc, 0x00 << 6); //源模块关闭
}

//关闭跟踪源
void tSysScpi::closeSourceTrackGenerator()
{
	if (sysData.source.isTrackGenOn)
	{
		sysData.source.isTrackGenOn = false;
	}
}

//关闭信号源
void tSysScpi::closeSourceSignalGenerator()
{
	if (sysData.source.isSignalGenOn)
	{
		sysData.source.isSignalGenOn = false;
	}
}

//设置信号源输出频率
unsigned long tSysScpi::setSourceSignalGenFreq(bool value)
{
	unsigned long freq = (unsigned long) sysData.source.signalGenOutputFreq;
	int N = 1;
	unsigned int zs = 0;
	unsigned int xs = 0;
	double Y = 0;

//	if (freq > MAXFREQ)
//		freq = MAXFREQ;
//	if (freq < 150e6)
//		freq = 150e6;

	//计算R5
	sysData.source.R5 = 0x00000005;

	//计算R4
	if (freq >= 150e6 && freq < 275e6)
	{
		N = 16;
		sysData.source.BandNumber = 0x00;
		sysData.source.R4 = 0x00C5003C;
	} else if (freq >= 275e6 && freq < 550e6)
	{
		N = 8;
		sysData.source.BandNumber = 0x00;
		sysData.source.R4 = 0x00B5003C;
	} else if (freq >= 550e6 && freq < 1100e6)
	{
		N = 4;
		sysData.source.BandNumber = 0x00;
		sysData.source.R4 = 0x00A5003C;
	} else if (freq >= 1100e6 && freq <= 1500e6)
	{
		N = 2;
		sysData.source.BandNumber = 0x00;
		sysData.source.R4 = 0x0095003C;
	} else if (freq > 1500e6 && freq < 2200e6)
	{
		N = 2;
		sysData.source.BandNumber = 0x01;
		sysData.source.R4 = 0x0095003C;
	} else if (freq >= 2200e6 && freq <= 2300e6)
	{
		N = 1;
		sysData.source.BandNumber = 0x01;
		sysData.source.R4 = 0x0085003C;
	} else if (freq > 2300e6 && freq <= 3100e6)
	{
		N = 1;
		sysData.source.BandNumber = 0x02;
		sysData.source.R4 = 0x0085003C;
	} else if (freq > 3100e6 && freq <= MAXFREQ)
	{
		N = 1;
		sysData.source.BandNumber = 0x03;
		sysData.source.R4 = 0x0085003C;
	}

	//计算R3
	sysData.source.R3 = 0x000004B3;

	//计算R2
	sysData.source.R2 = 0x00004EC2;

	//计算R1
	sysData.source.R1 = 0x0800fd01;

	//计算R0
	Y = N * (freq * 1e-6) / 10; // RFout = (INT + DOT / MOD)*Fref /N(分频比)
	zs = (unsigned int) Y;
	xs = (unsigned int) ((Y - zs) * 4000); //MOD = 4000
	sysData.source.R0 = (zs << 15) | (xs << 3);

	rfDownload(0x44, sysData.source.isSignalGenOn << 2 | sysData.source.BandNumber); //波段号和信号源控制

	rfDownload(0xfa, sysData.source.R5 & 0xFFFFFFFF); //R5
	rfDownload(0xfa, sysData.source.R4 & 0xFFFFFFFF); //R4
	rfDownload(0xfa, sysData.source.R3 & 0xFFFFFFFF); //R3
	rfDownload(0xfa, sysData.source.R2 & 0xFFFFFFFF); //R2
	rfDownload(0xfa, sysData.source.R1 & 0xFFFFFFFF); //R1
	rfDownload(0xfa, sysData.source.R0 & 0xFFFFFFFF); //R0

	rfDownload(0xfb, 0x01); //关闭三本

	return ((zs << 15) + (xs << 3));
	//    controlRf();
}

//打开服务器SOCKET
bool tSysScpi::openServerSocket(void)
{
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

//鉴频库初始化
bool tSysScpi::discLibraryInitialize(void)
{
	devs = NULL;
	dev_handle = NULL;
	ctx = NULL;
	usbDeviceInitedSuccess = (libusb_init(&ctx) == 0);
	if (usbDeviceInitedSuccess)
		libusb_set_debug(ctx, 3);
}

//连接鉴频器
bool tSysScpi::connectToDiscriminator(void)
{
	if (not usbDeviceInitedSuccess)
	{
		return false;
	}

	if (dev_handle != NULL)
	{
		disconectFromUSBDevice();
		dev_handle = NULL;
	}

	int r;
	ssize_t cnt;
	cnt = libusb_get_device_list(ctx, &devs);
	if (cnt < 0)
	{
		return false;
	}

	dev_handle = libusb_open_device_with_vid_pid(ctx, 0x10c4, 0x0);

	if (dev_handle == NULL)
	{
		return false;
	}

	libusb_free_device_list(devs, 1);

	r = libusb_reset_device(dev_handle);
	if (r < 0)
	{
		return false;
	}

	r = libusb_kernel_driver_active(dev_handle, 0);
	if (r < 0)
	{
		return false;
	}

	r = libusb_detach_kernel_driver(dev_handle, 0);

	r = libusb_claim_interface(dev_handle, 0);
	if (r < 0)
	{
		return false;
	}

	r = libusb_clear_halt(dev_handle, 0x02);
	if (r < 0)
	{
		return false;
	}

	r = libusb_clear_halt(dev_handle, 0x81);
	if (r < 0)
	{
		return false;
	}

	usbDiscFailNums = 0;
	return true;
}

//断开鉴频器
void tSysScpi::disconectFromUSBDevice(void)
{
	if (not usbDeviceInitedSuccess)
	{
		return;
	}

	if (dev_handle != NULL)
	{
		libusb_attach_kernel_driver(dev_handle, 0);
		libusb_release_interface(dev_handle, 0);
		libusb_close(dev_handle);
		dev_handle = NULL;
	}
}

//设置鉴频数据长度
bool tSysScpi::setDiscriminationDataLen(int len)
{
	unsigned char discSTemp[64] = { 0x80, 0x7f, 0x13 };
	usbDiscriminatorSend(discSTemp, sizeof discSTemp / sizeof discSTemp[0]);
}

//设置鉴频输入增益
void tSysScpi::setDiscriminatorInputGain(int level)
{
	unsigned char data[64] = { };
	data[0] = 0x50;
	data[1] = ~data[0];
	data[2] = ((level >> 8) & 0xff);
	data[3] = (level & 0xff);

	usbDiscriminatorSend(data, sizeof data / sizeof data[0]);
}

//发送数据
bool tSysScpi::usbDiscriminatorSend(unsigned char* data, int len)
{
	if ((not usbDeviceInitedSuccess) || (dev_handle == NULL))
	{
		return false;
	}

	int actual = 0;
	int r = libusb_bulk_transfer(dev_handle, 0x02, data, len, &actual, 2000);

	return (r == 0) && (actual == len);
}

//接收数据
bool tSysScpi::usbDiscriminatorRecv(unsigned char* data, int dataLen, int* recvLen)
{
	if ((not usbDeviceInitedSuccess) || (dev_handle == NULL))
	{
		return false;
	}

	int r = 0;
	int actual = 0;

	r = libusb_bulk_transfer(dev_handle, 0x81, data, dataLen, &actual, 3000);

	if ((r == 0) && (actual > 0))
	{
		*recvLen = actual;
		return true;
	} else
	{
		*recvLen = 0;
		return false;
	}
}

//获取鉴频校准结果
bool tSysScpi::getDiscriminationCalResult(int dw, int* result)
{
	unsigned char discSTemp[64] = { 0x80, 0x7f };
	unsigned char discRTemp[64] = { };
	int discValue[3] = { 0 };
	int discLen = 0;

	setDiscriminatorInputGain(dw);
	usleep(50 * 1000);

	for (int i = 0; i < sizeof discValue / sizeof discValue[0]; i++)
	{
		if (not usbDiscriminatorSend(discSTemp, sizeof discSTemp / sizeof discSTemp[0]))
		{
			return false;
		}

		usleep(10000);

		if (usbDiscriminatorRecv(discRTemp, sizeof discRTemp / sizeof discRTemp[0], &discLen))
		{
			discValue[i] = (discRTemp[0] << 8) + discRTemp[1];
		} else
		{
			return false;
		}
	}

	*result = (int) ((discValue[1] + discValue[2]) / 2);
	return true;
}

//设置鉴频频率
void tSysScpi::setDiscriminatorFreq(double value)
{
	//Buc延时
	int BucLen = 0;

	//MHz
	rfData.Ft1 = 1500;
	rfData.Ft2 = 2300;
	rfData.Ft3 = 3100;
	rfData.Ft4 = 3601;
	rfData.BucLen = sysData.sweep.sweepPoints;

	//值计算
	rfData.Db = 40;
	double tempDb1 = 0;
	double tempDb2 = 0;
	double tempDb = 0;
	tempDb1 = 0.2 * sysData.span.span / sysData.bw.rbw;
	tempDb2 = sysData.sweep.sweepTime / (1.0 * 1e6) * 3;
	tempDb = tempDb1 < tempDb2 ? tempDb1 : tempDb2; //取二者最小值

	if (tempDb < 3.0)
	{
		rfData.Db = 3;
	} else if (tempDb > 60000.0)
	{
		rfData.Db = 60000;
	} else
	{
		rfData.Db = (int) tempDb;
	}

	rfData.Delta_f = 0; //MHz
	rfData.Delta_n = 0;//
	rfData.Dnf_f = 0;

	rfData.Dnf_f1 = (rfData.Dnf_f & 0xff);
	rfData.Dnf_f2 = (rfData.Dnf_f >> 8) & 0xff;
	rfData.Dnf_f3 = (rfData.Dnf_f >> 16) & 0xff;
	rfData.Dnf_f4 = (rfData.Dnf_f >> 24) & 0xff;
	rfData.Dnf_f5 = 0x00;

	rfData.FLo1 = (double) (value / 1e6);
	rfData.FLo2 = (double) (value / 1e6);

	double startErr1 = 0;
	double stopErr1 = 0;
	double startErr2 = 0;
	double stopErr2 = 0;

	if (rfData.FLo1 <= rfData.Ft1)
	{
		rfData.Start_Band = 0;
		startErr1 = qAbs(rfData.FLo1);
		startErr2 = qAbs(rfData.Ft1 * 1.0 - rfData.FLo1);
	} else if (rfData.FLo1 <= rfData.Ft2)
	{
		rfData.Start_Band = 1;
		startErr1 = qAbs(rfData.FLo1 - rfData.Ft1 * 1.0);
		startErr2 = qAbs(rfData.Ft2 * 1.0 - rfData.FLo1);
	} else if (rfData.FLo1 <= rfData.Ft3)
	{
		rfData.Start_Band = 2;
		startErr1 = qAbs(rfData.FLo1 - rfData.Ft2 * 1.0);
		startErr2 = qAbs(rfData.Ft3 * 1.0 - rfData.FLo1);
	} else
	{
		rfData.Start_Band = 3;
		startErr1 = qAbs(rfData.FLo1 - rfData.Ft3 * 1.0);
		startErr2 = qAbs(rfData.Ft4 * 1.0 - rfData.FLo1);
	}

	if (rfData.FLo2 <= rfData.Ft1)
	{
		rfData.Stop_Band = 0;
		stopErr1 = qAbs(rfData.FLo2);
		stopErr2 = qAbs(rfData.Ft1 * 1.0 - rfData.FLo2);
	} else if (rfData.FLo2 <= rfData.Ft2)
	{
		rfData.Stop_Band = 1;
		stopErr1 = qAbs(rfData.FLo2 - rfData.Ft1 * 1.0);
		stopErr2 = qAbs(rfData.Ft2 * 1.0 - rfData.FLo2);
	} else if (rfData.FLo2 <= rfData.Ft3)
	{
		rfData.Stop_Band = 2;
		stopErr1 = qAbs(rfData.FLo2 - rfData.Ft2 * 1.0);
		stopErr2 = qAbs(rfData.Ft3 * 1.0 - rfData.FLo2);
	} else if (rfData.FLo2 <= rfData.Ft4)
	{
		rfData.Stop_Band = 3;
		stopErr1 = qAbs(rfData.FLo2 - rfData.Ft3 * 1.0);
		stopErr2 = qAbs(rfData.Ft4 * 1.0 - rfData.FLo2);
	}

	double yxwc = 5;
	int bdc = rfData.Stop_Band - rfData.Start_Band;

	if (((bdc) > 0) && (sysData.span.span > 0) && ((startErr2 <= yxwc) || (stopErr1 <= yxwc)))
	{
		if ((startErr2 <= yxwc) && (stopErr1 <= yxwc) && (startErr2 <= stopErr1) && (bdc == 1))
		{
			rfData.Start_Band++;
		} else if ((startErr2 <= yxwc) && (stopErr1 <= yxwc) && (startErr2 > stopErr1) && (bdc == 1))
		{
			rfData.Stop_Band--;
		} else if ((startErr2 <= yxwc) && (stopErr1 > yxwc) && (bdc == 1))
		{
			rfData.Start_Band++;
		} else if ((startErr2 > yxwc) && (stopErr1 <= yxwc) && (bdc == 1))
		{
			rfData.Stop_Band--;
		} else if ((startErr2 <= yxwc) && (stopErr1 <= yxwc) && (bdc > 1))
		{
			rfData.Start_Band++;
			rfData.Stop_Band--;
		} else if ((startErr2 <= yxwc) && (stopErr1 > yxwc) && (bdc > 1))
		{
			rfData.Start_Band++;
		} else if ((startErr2 > yxwc) && (stopErr1 <= yxwc) && (bdc > 1))
		{
			rfData.Stop_Band--;
		}
	}

	//波段方程
	double freqOffset = RFFREQOFFSET;
	double freqLC1 = 2026.4;
	switch (rfData.Start_Band)
	{
		case 0: //起点在0波段
			rfData.bandNumber = 0x00;

			if (rfData.FLo2 - rfData.FLo1 == 0)
			{
				rfData.N1 = (rfData.FLo1 + freqLC1 + freqOffset) / 80.0;
				rfData.N2 = rfData.N3 = rfData.N4 = 0;
				rfData.BAND1_BUG = sysData.sweep.sweepPoints;
				rfData.BAND2_BUG = rfData.BAND3_BUG = rfData.BAND4_BUG = 0;
			} else
			{
				if ((rfData.Stop_Band - rfData.Start_Band) == 0)
				{
					rfData.bandNumber = 0x00;
					rfData.Start1 = rfData.FLo1;
					rfData.N1 = (rfData.Start1 + freqLC1 + freqOffset) / 80.0;
					rfData.BAND1_BUG = (int) ((rfData.FLo2 - rfData.FLo1) / rfData.Delta_f + 0.5) + BucLen;
					rfData.BAND2_BUG = rfData.BAND3_BUG = rfData.BAND4_BUG = 0x00;
					rfData.Start2 = rfData.Start3 = rfData.Start4 = 0;
					rfData.N2 = 0x0;
					rfData.N3 = 0x0;
					rfData.N4 = 0x0;
				} else
				{
					rfData.Start1 = rfData.FLo1;
					rfData.N1 = (rfData.Start1 + freqLC1 + freqOffset) / 80.0;
					rfData.BAND1_BUG = (int) ((rfData.Ft1 - rfData.Start1) / rfData.Delta_f + 0.5);
					rfData.Start2 = rfData.FLo1 + (rfData.BAND1_BUG + 0) * rfData.Delta_f;
					rfData.FLo3 = rfData.Start2 + 826.4 + freqOffset; //MHZ
					rfData.N2 = rfData.FLo3 / 80.0;

					if (rfData.Stop_Band - rfData.Start_Band == 1)
					{
						rfData.BAND2_BUG = (int) ((rfData.FLo2 - rfData.Start2) / rfData.Delta_f + 0.5) + BucLen;
						rfData.BAND3_BUG = rfData.BAND4_BUG = 0x00;
						rfData.Start3 = rfData.Start4 = 0;
						rfData.N3 = 0x0;
						rfData.N4 = 0x0;
					} else
					{
						rfData.BAND2_BUG = (int) ((rfData.Ft2 - rfData.Start2) / rfData.Delta_f + 0.5);
						rfData.Start3 = rfData.Start2 + (rfData.BAND2_BUG + 0) * rfData.Delta_f;
						rfData.FLo4 = rfData.Start3 + 826.4 + freqOffset; //MHZ
						rfData.N3 = rfData.FLo4 / 80.0;

						if (rfData.Stop_Band - rfData.Start_Band == 2)
						{
							rfData.BAND3_BUG = (int) ((rfData.FLo2 - rfData.Start3) / rfData.Delta_f + 0.5) + BucLen;
							rfData.BAND4_BUG = 0x00;

							rfData.Start4 = 0;
							rfData.N4 = 0x0;
						} else
						{
							rfData.BAND3_BUG = (int) ((rfData.Ft3 - rfData.Start3) / rfData.Delta_f + 0.5);
							rfData.Start4 = rfData.Start3 + (rfData.BAND3_BUG + 0) * rfData.Delta_f;
							rfData.BAND4_BUG = (int) ((rfData.FLo2 - rfData.Start4) / rfData.Delta_f + 0.5) + BucLen;
							rfData.FLo5 = rfData.Start4 - 826.4 - freqOffset;
							rfData.N4 = rfData.FLo5 / 80.0;
						}
					}
				}
			}
			break;
		case 1: //起点在1波段
			rfData.bandNumber = 0x01;

			if (rfData.FLo2 - rfData.FLo1 == 0)
			{
				rfData.N2 = (rfData.FLo1 + 826.4 + freqOffset) / 80.0;
				rfData.N1 = rfData.N3 = rfData.N4 = 0;

				rfData.BAND2_BUG = sysData.sweep.sweepPoints;
				rfData.BAND1_BUG = rfData.BAND3_BUG = rfData.BAND4_BUG = 0;
			} else
			{
				rfData.BAND1_BUG = 0x0;
				rfData.N1 = 0x0;
				rfData.Start1 = 0;
				rfData.Start2 = rfData.FLo1;
				rfData.N2 = (rfData.Start2 + 826.4 + freqOffset) / 80.0;

				if (rfData.Stop_Band - rfData.Start_Band == 0)
				{
					rfData.BAND2_BUG = (int) ((rfData.FLo2 - rfData.Start2) / rfData.Delta_f + 0.5) + BucLen;
					rfData.BAND3_BUG = rfData.BAND4_BUG = 0x00;
					rfData.Start3 = rfData.Start4 = 0;
					rfData.N3 = rfData.N4 = 0x0;
				} else
				{
					rfData.BAND2_BUG = (int) ((rfData.Ft2 - rfData.Start2) / rfData.Delta_f + 0.5);
					rfData.Start3 = rfData.Start2 + (rfData.BAND2_BUG) * rfData.Delta_f;
					rfData.N3 = (rfData.Start3 + 826.4 + freqOffset) / 80.0;

					if (rfData.Stop_Band - rfData.Start_Band == 1)
					{
						rfData.BAND3_BUG = (int) ((rfData.FLo2 - rfData.Start3) / rfData.Delta_f + 0.5) + BucLen;
						rfData.BAND4_BUG = 0x0;
						rfData.Start4 = 0;
						rfData.N4 = 0x0;
					} else
					{
						rfData.BAND3_BUG = (int) ((rfData.Ft3 - rfData.Start3) / rfData.Delta_f + 0.5);
						rfData.Start4 = rfData.Start3 + (rfData.BAND3_BUG + 0) * rfData.Delta_f;
						rfData.N4 = (rfData.Start4 - 826.4 - freqOffset) / 80.0;
						rfData.BAND4_BUG = (int) ((rfData.FLo2 - rfData.Start4) / rfData.Delta_f + 0.5) + BucLen;
					}
				}
			}
			break;
		case 2: //起点在2波段
			rfData.bandNumber = 0x02;

			if (rfData.FLo2 - rfData.FLo1 == 0)
			{
				rfData.N3 = (rfData.FLo1 + 826.4 + freqOffset) / 80.0;
				rfData.N1 = rfData.N2 = rfData.N4 = 0;

				rfData.BAND3_BUG = sysData.sweep.sweepPoints;
				rfData.BAND2_BUG = rfData.BAND1_BUG = rfData.BAND4_BUG = 0;
			} else
			{
				rfData.BAND1_BUG = rfData.BAND2_BUG = 0x0;
				rfData.N1 = rfData.N2 = 0x0;
				rfData.Start1 = rfData.Start2 = 0;
				rfData.Start3 = rfData.FLo1;
				rfData.N3 = (rfData.Start3 + 826.4 + freqOffset) / 80.0;

				if (rfData.Stop_Band - rfData.Start_Band == 0)
				{
					rfData.BAND3_BUG = (int) ((rfData.FLo2 - rfData.Start3) / rfData.Delta_f + 0.5) + BucLen;
					rfData.BAND4_BUG = 0x0;
					rfData.Start4 = 0;
					rfData.N4 = 0x0;
				} else
				{
					rfData.BAND3_BUG = (int) ((rfData.Ft3 - rfData.Start3) / rfData.Delta_f + 0.5);
					rfData.Start4 = rfData.Start3 + (rfData.BAND3_BUG + 0) * rfData.Delta_f;
					rfData.N4 = (rfData.Start4 - 826.4 - freqOffset) / 80.0;
					rfData.BAND4_BUG = (int) ((rfData.FLo2 - rfData.Start4) / rfData.Delta_f + 0.5) + BucLen;
				}
			}
			break;
		case 3: //起点在3波段
			rfData.bandNumber = 0x03;
			if (rfData.FLo2 - rfData.FLo1 == 0)
			{
				rfData.N4 = (rfData.FLo1 - 826.4 - freqOffset) / 80.0;
				rfData.N1 = rfData.N2 = rfData.N3 = 0;

				rfData.BAND4_BUG = sysData.sweep.sweepPoints;
				rfData.BAND2_BUG = rfData.BAND3_BUG = rfData.BAND1_BUG = 0;
			} else
			{
				rfData.BAND1_BUG = 0x00;
				rfData.BAND2_BUG = 0x00;
				rfData.BAND3_BUG = 0x00;
				rfData.N1 = 0x0;
				rfData.N2 = 0x0;
				rfData.N3 = 0x0;
				rfData.Start1 = rfData.Start2 = rfData.Start3 = 0;
				rfData.Start4 = rfData.FLo1;
				rfData.BAND4_BUG = (int) ((rfData.FLo2 - rfData.FLo1) / rfData.Delta_f + 0.5) + BucLen;
				rfData.N4 = (rfData.FLo1 - 826.4 - freqOffset) / 80.0;
			}
			break;
		default:
			break;
	}

	//N1 //高8位整数位，低32位是小数位   起点在第0波段起始频率
	rfData.N1_n = (long int) rfData.N1;
	rfData.N1_f = (unsigned long long) ((rfData.N1 - rfData.N1_n) * 4294967295.0);
	rfData.N1_f1 = (int) (rfData.N1_f & 0xff);
	rfData.N1_f2 = (int) ((rfData.N1_f >> 8) & 0xff);
	rfData.N1_f3 = (int) ((rfData.N1_f >> 16) & 0xff);
	rfData.N1_f4 = (int) ((rfData.N1_f >> 24) & 0xff);
	rfData.N1_n = (int) ((rfData.N1_n - 0) & 0xff);

	//N2 //高8位整数位，低32位是小数位   起点在第1 波段起始频率
	rfData.N2_n = (long int) rfData.N2;
	rfData.N2_f = (unsigned long long) ((rfData.N2 - rfData.N2_n) * 4294967295.0);
	rfData.N2_f1 = (int) (rfData.N2_f & 0xff);
	rfData.N2_f2 = (int) ((rfData.N2_f >> 8) & 0xff);
	rfData.N2_f3 = (int) ((rfData.N2_f >> 16) & 0xff);
	rfData.N2_f4 = (int) ((rfData.N2_f >> 24) & 0xff);
	rfData.N2_n = (int) ((rfData.N2_n - 0) & 0xff);

	//N3  //高8位整数位，低32位是小数位   起点在第2 波段起始频率
	rfData.N3_n = (long int) rfData.N3;
	rfData.N3_f = (unsigned long long) ((rfData.N3 - rfData.N3_n) * 4294967295.0);
	rfData.N3_f1 = (int) (rfData.N3_f & 0xff);
	rfData.N3_f2 = (int) ((rfData.N3_f >> 8) & 0xff);
	rfData.N3_f3 = (int) ((rfData.N3_f >> 16) & 0xff);
	rfData.N3_f4 = (int) ((rfData.N3_f >> 24) & 0xff);
	rfData.N3_n = (int) ((rfData.N3_n - 0) & 0xff);

	//N4  //高8位整数位，低32位是小数位   起点在第3 波段起始频率
	rfData.N4_n = (long int) rfData.N4;
	rfData.N4_f = (unsigned long long) ((rfData.N4 - rfData.N4_n) * 4294967295.0);
	rfData.N4_f1 = (int) (rfData.N4_f & 0xff);
	rfData.N4_f2 = (int) ((rfData.N4_f >> 8) & 0xff);
	rfData.N4_f3 = (int) ((rfData.N4_f >> 16) & 0xff);
	rfData.N4_f4 = (int) ((rfData.N4_f >> 24) & 0xff);
	rfData.N4_n = (int) ((rfData.N4_n - 0) & 0xff);

	rfDownload(0x18, 0x00);//count stop

	//总的BUC的数目  （地址为S4）
	rfDownload(0x10, (((65535 - sysData.sweep.sweepPoints - BucLen) >> 8) & 0xff));
	rfDownload(0x10, ((65535 - sysData.sweep.sweepPoints - BucLen) & 0xff));

	//各个波段BUC的数目
	//0波段 （地址为S13）
	rfDownload(0x34, ((65535 - rfData.BAND1_BUG) >> 8) & 0xff);
	rfDownload(0x34, (65535 - rfData.BAND1_BUG) & 0xff);

	//1波段 （地址为S14）
	rfDownload(0x38, ((65535 - rfData.BAND2_BUG) >> 8) & 0xff);
	rfDownload(0x38, (65535 - rfData.BAND2_BUG) & 0xff);

	//2波段 （地址为S15）
	rfDownload(0x3C, ((65535 - rfData.BAND3_BUG) >> 8) & 0xff);
	rfDownload(0x3C, (65535 - rfData.BAND3_BUG) & 0xff);

	//3波段 （地址为S16）
	rfDownload(0x40, ((65535 - rfData.BAND4_BUG) >> 8) & 0xff);
	rfDownload(0x40, (65535 - rfData.BAND4_BUG) & 0xff);

	//sbuc(地址为S3)
	rfData.Db1 = 65536 - rfData.Db;
	rfDownload(0x0c, (rfData.Db1 >> 8) & 0xff);
	rfDownload(0x0c, (rfData.Db1) & 0xff);

	rfDownload(0x04, rfData.Dnf_f5);
	rfDownload(0x04, rfData.Dnf_f4);
	rfDownload(0x04, rfData.Dnf_f3);
	rfDownload(0x04, rfData.Dnf_f2);
	rfDownload(0x04, rfData.Dnf_f1);

	//start freq
	//0波段 （地址为S0）
	rfDownload(0x00, rfData.N1_n);
	rfDownload(0x00, rfData.N1_f4);
	rfDownload(0x00, rfData.N1_f3);
	rfDownload(0x00, rfData.N1_f2);
	rfDownload(0x00, rfData.N1_f1);

	//1波段 （地址为S10）
	rfDownload(0x28, rfData.N2_n);
	rfDownload(0x28, rfData.N2_f4);
	rfDownload(0x28, rfData.N2_f3);
	rfDownload(0x28, rfData.N2_f2);
	rfDownload(0x28, rfData.N2_f1);

	//2波段 （地址为S11）
	rfDownload(0x2c, rfData.N3_n);
	rfDownload(0x2c, rfData.N3_f4);
	rfDownload(0x2c, rfData.N3_f3);
	rfDownload(0x2c, rfData.N3_f2);
	rfDownload(0x2c, rfData.N3_f1);

	//3波段 （地址为S12）
	rfDownload(0x30, rfData.N4_n);
	rfDownload(0x30, rfData.N4_f4);
	rfDownload(0x30, rfData.N4_f3);
	rfDownload(0x30, rfData.N4_f2);
	rfDownload(0x30, rfData.N4_f1);

	//sweep time
	rfData.Da1 = sysData.sweep.sweepTime / (rfData.Db * sysData.sweep.sweepPoints);
	rfData.Da = (unsigned long) (16777216.0 - ((rfData.Da1 * 15000.0)));

	rfDownload(0x08, (rfData.Da >> 16) & 0xff);
	rfDownload(0x08, (rfData.Da >> 8) & 0xff);
	rfDownload(0x08, (rfData.Da) & 0xff);

	rfDownload(0x18, (rfData.bandNumber << 5) | 0x02);//count load
	rfDownload(0x18, (rfData.bandNumber << 5) | 0x00);//
	//  usleep(50000);
	rfDownload(0x18, (rfData.bandNumber << 5) | 0x04);//trig sweep

	if (rfData.Stop_Band - rfData.Start_Band == 0)
	{
		rfDownload(0x18, (0 << 4) | (rfData.bandNumber << 5) | 0x01);
	} else
	{
		rfDownload(0x18, (1 << 4) | (rfData.bandNumber << 5) | 0x01);
	}
	return;
}

int tSysScpi::ulideDataSend(unsigned short channel, unsigned short cmd, unsigned short addr, unsigned short size, double data)
{
	struct packpage
	{
			unsigned short header;
			unsigned short cmd;
			unsigned short ch;
			unsigned short addr;
			unsigned short size;
			unsigned char data[54];
	};
	struct packpage d;
	memset(&d, 0, sizeof(d));
	d.header = 0x55aa;
	d.cmd = cmd; //4 写  5 读
	d.ch = channel - 1;
	d.addr = addr;
	d.size = size;
	memcpy(&d.data[0], (const char *) &data, size);

	if (cSocket != NULL)
	{
		cSocket->write((const char *) &d, sizeof(d));
		if (!cSocket->waitForBytesWritten(1000))
		{
			return __SCPI_FAILED;
		}
	}
	usleep(300 * 1000);
	return __SCPI_SUCCESS;
}

//发送Serial数据
int tSysScpi::sendSerialData(bool isCom, QString cmd)
{
	if (cmd.isNull() || cmd.isEmpty())
	{
		return __SCPI_FAILED;
	}

	if (isCom && comHandle <= 0)
	{
		return __SCPI_FAILED;
	}

	QString fullCmd = cmd.trimmed();

	for (int i = 0; i < sizeof SCPICMDEOF / sizeof SCPICMDEOF[0]; i++)
	{
		fullCmd += SCPICMDEOF[i];
	}

	int len = fullCmd.length();

	if (isCom)//by serial
	{
		int sendLen = 0;
		QString sendBuf = cmd;
		QTime t;
		t.restart();

		while (sendBuf.length() > 0)
		{
			sendLen = write(comHandle, sendBuf.toStdString().c_str(), sendBuf.length());
			if (sendLen <= 0 && t.elapsed() >= 10000)
			{
				break;
			} else if (sendLen > 0)
			{
				sendBuf.remove(0, sendLen);
			}
		}
	} else//by eth
	{
		if (cSocket != NULL)
		{
			cSocket->write(fullCmd.toStdString().c_str());

			if (cSocket->waitForBytesWritten(1000))
			{

			}
		}
	}
	printf("cmd to signal generator=>%s\n",cmd.toStdString().c_str());
	return __SCPI_SUCCESS;
}

//设置跟踪源锁相环频率
void tSysScpi::setSourceTrackGenPllFreq(int index)
{
	if (index >= 0 && index < sizeof sysData.source.trackAmptStandard / sizeof sysData.source.trackAmptStandard[0])
	{
		setFrequencyOfCenter(sysData.source.trackAmptStandard[index].centerFreq);
		setFrequencyOfSpan(sysData.source.trackAmptStandard[index].span);
		sysData.freq.isShowCenter = false;
		controlRf();
		setSourceTrackGenPll(index);
	}
}

//设置跟踪源锁相环
void tSysScpi::setSourceTrackGenPll(int index)
{
	if (srcHandle >= 0 && index >= 0 && index < sizeof sysData.source.trackAmptStandard / sizeof sysData.source.trackAmptStandard[0])
	{
		ioctl(srcHandle, sysData.source.trackAmptStandard[index].hAddr, sysData.source.trackAmptStandard[index].value & 0xff);
		ioctl(srcHandle, sysData.source.trackAmptStandard[index].lAddr, (sysData.source.trackAmptStandard[index].value >> 8) & 0xff);
	}
}

//设置信号源校准频率
void tSysScpi::setSourceSignalCalibrateParamOfFreq(int index)
{
	if (index >= 0 && index < sizeof sysData.source.signalAmptStandard / sizeof sysData.source.signalAmptStandard[0])
	{
		sysData.source.signalGenOutputFreq = sysData.source.signalAmptStandard[index].baseFreq;
		sysData.freq.isShowCenter = false;
		setFrequencyOfCenter(sysData.source.signalGenOutputFreq);
		controlRf();
		setSourceSignalGenState(sysData.source.isSignalGenOn, false);
	}
}

//载入鉴频设置
void tSysScpi::loadDiscConfig(void)
{
	QString fileName = QCoreApplication::applicationDirPath() + "/setting.ini";

	memset(sysData.disc.discStandard, 0x1ff, sizeof sysData.disc.discStandard);

	if (QFile(fileName).exists())
	{
		QSettings setting(fileName, QSettings::IniFormat);
		char tempChar[32] = { };

		setting.beginGroup("discriminator");

		for (int i = 0; i < sizeof sysData.disc.discStandard / sizeof sysData.disc.discStandard[0]; i++)
		{
			sysData.disc.discStandard[i] = setting.value("discStandard" + QString(intToString(i, tempChar)).trimmed()).toInt();
		}

		setting.endGroup();
	}
}

//保存鉴频设置
void tSysScpi::saveDiscConfig(void)
{
	QString fileName = QCoreApplication::applicationDirPath() + "/setting.ini";
	QSettings setting(fileName, QSettings::IniFormat);
	char tempChar[32] = { };

	setting.beginGroup("discriminator");

	for (int i = 0; i < sizeof sysData.disc.discStandard / sizeof sysData.disc.discStandard[0]; i++)
	{
		setting.setValue("discStandard" + QString(intToString(i, tempChar)).trimmed(), sysData.disc.discStandard[i]);
	}

	setting.endGroup();
}

//载入源配置
void tSysScpi::loadSourceConfig(void)
{
	memcpy(sysData.source.signalAmptStandard, signalAmptRef, sizeof sysData.source.signalAmptStandard);
	memcpy(sysData.source.trackAmptStandard, trackAmptRef, sizeof sysData.source.trackAmptStandard);

	QString fileName = QCoreApplication::applicationDirPath() + "/setting.ini";

	if (QFile(fileName).exists())
	{
		QSettings setting(fileName, QSettings::IniFormat);
		char tempChar[32] = { };

		setting.beginGroup("signal");

		for (int i = 0; i < sizeof sysData.source.signalAmptStandard / sizeof sysData.source.signalAmptStandard[0]; i++)
		{
			sysData.source.signalAmptStandard[i].value = setting.value("signalAmptStandard" + QString(intToString(i, tempChar)).trimmed(), sysData.source.signalAmptStandard[i].value).toUInt();
		}

		for (int i = 0; i < sizeof sysData.source.trackAmptStandard / sizeof sysData.source.trackAmptStandard[0]; i++)
		{
			sysData.source.trackAmptStandard[i].value = setting.value("trackAmptStandard" + QString(intToString(i, tempChar)).trimmed(), sysData.source.trackAmptStandard[i].value).toUInt();
		}

		sysData.source.trackNetworkStandard.startFreq = setting.value("trackNetworkStandard_StartFreq", 0).toDouble();
		sysData.source.trackNetworkStandard.stopFreq = setting.value("trackNetworkStandard_StopFreq", 0).toDouble();
		sysData.source.trackNetworkStandard.sweepPoints = setting.value("trackNetworkStandard_SweepPoints", 0).toDouble();
		sysData.source.trackNetworkStandard.normalized = setting.value("trackNetworkStandard_Normalized", false).toBool();
		for (int i = 0; i < sysData.source.trackNetworkStandard.sweepPoints; i++)
		{
			sysData.source.trackNetworkStandard.value[i] = setting.value("trackNetworkStandard" + QString(intToString(i, tempChar)).trimmed(), 0).toDouble();
			sysData.source.trackNetworkStandard.normalizeData[i] = setting.value("trackNetworkNormalData" + QString(intToString(i, tempChar)).trimmed(), 0).toDouble();
		}

		setting.endGroup();
	}
}

//保存源配置
void tSysScpi::saveSourceConfig(void)
{
	QString fileName = QCoreApplication::applicationDirPath() + "/setting.ini";
	QSettings setting(fileName, QSettings::IniFormat);
	char tempChar[32] = { };

	setting.beginGroup("signal");

	for (int i = 0; i < sizeof sysData.source.signalAmptStandard / sizeof sysData.source.signalAmptStandard[0]; i++)
	{
		setting.setValue("signalAmptStandard" + QString(intToString(i, tempChar)).trimmed(), sysData.source.signalAmptStandard[i].value);
	}

	for (int i = 0; i < sizeof sysData.source.trackAmptStandard / sizeof sysData.source.trackAmptStandard[0]; i++)
	{
		setting.setValue("trackAmptStandard" + QString(intToString(i, tempChar)).trimmed(), sysData.source.trackAmptStandard[i].value);
	}

	setting.setValue("trackNetworkStandard_StartFreq", sysData.source.trackNetworkStandard.startFreq);
	setting.setValue("trackNetworkStandard_StopFreq", sysData.source.trackNetworkStandard.stopFreq);
	setting.setValue("trackNetworkStandard_SweepPoints", sysData.source.trackNetworkStandard.sweepPoints);

	for (int i = 0; i < sysData.source.trackNetworkStandard.sweepPoints; i++)
	{
		setting.setValue("trackNetworkStandard" + QString(intToString(i, tempChar)).trimmed(), sysData.source.trackNetworkStandard.value[i]);
		setting.setValue("trackNetworkStandard_NormalizeData" + QString(intToString(i, tempChar)).trimmed(), sysData.source.trackNetworkStandard.normalizeData[i]);
	}

	setting.endGroup();
}

//保存网络测量配置
void tSysScpi::saveSourceConfigOfNetworkMeasure(void)
{
	QString fileName = QCoreApplication::applicationDirPath() + "/setting.ini";
	QSettings setting(fileName, QSettings::IniFormat);
	char tempChar[32] = { };

	setting.beginGroup("signal");
	setting.setValue("trackNetworkStandard_StartFreq", sysData.source.trackNetworkStandard.startFreq);
	setting.setValue("trackNetworkStandard_StopFreq", sysData.source.trackNetworkStandard.stopFreq);
	setting.setValue("trackNetworkStandard_SweepPoints", sysData.source.trackNetworkStandard.sweepPoints);

	for (int i = 0; i < sysData.source.trackNetworkStandard.sweepPoints; i++)
	{
		setting.setValue("trackNetworkStandard" + QString(intToString(i, tempChar)).trimmed(), sysData.source.trackNetworkStandard.value[i]);
		//usleep(sysData.sweep.sweepTime / 1e3 + 100 * 1000);
		//getDataFromIF();
		//setting.setValue("trackNetworkNormalData" + QString(intToString(i, tempChar)).trimmed(), sysData.prjValue[i]);
	}

	usleep(sysData.sweep.sweepTime / 1e3 + 100 * 1000);
	getDataFromIF();
	sysData.source.trackNetworkStandard.normalized = true;
	setting.setValue("trackNetworkStandard_Normalized", sysData.source.trackNetworkStandard.normalized);

	for (int i = 0; i < sysData.source.trackNetworkStandard.sweepPoints; i++)
	{
		sysData.source.trackNetworkStandard.normalizeData[i] = sysData.trace[0].value[i] + sysData.source.trackNetworkStandard.value[i];
		setting.setValue("trackNetworkStandard_NormalizeData" + QString(intToString(i, tempChar)).trimmed(), sysData.source.trackNetworkStandard.normalizeData[i]);
	}

	setting.endGroup();
}

//复位出厂校准数据
void tSysScpi::resetFactoryCalibrationData(void)
{
	memset(&sysData.factoryCalData, 0, sizeof sysData.factoryCalData);
}

//复位直采出厂校准数据
void tSysScpi::resetZcCalibrationData(void)
{
	memset(&sysData.zcCalData, 0, sizeof sysData.zcCalData);
}

//复位直采通道前置放大校准数据
void tSysScpi::resetZcPreamplifierCalibrationData(void)
{
	memset(&sysData.zcPreamplifierCalData, 0, sizeof sysData.zcPreamplifierCalData);
}

//复位前置放大校准数据
void tSysScpi::resetPreamplifierCalibrationData(void)
{
	memset(&sysData.preamplifierCalData, 0, sizeof sysData.preamplifierCalData);
}

//复位用户校准数据
void tSysScpi::resetUserCalibrationData(void)
{
	memset(&sysData.userCalData, 0, sizeof sysData.userCalData);
}

//重新连接串口
void tSysScpi::reconnectSerial(void)
{

}

void tSysScpi::showRunMessage(QString title)
{
	outputMessage(title);
}

void tSysScpi::showRunAlarm(QString title)
{
	outputToScreen(title);
}

//打印图片
int tSysScpi::printPixmap(QPixmap pix, unsigned char pageSize, unsigned char orientation, unsigned char copyNums, bool colour)
{
	char* usbPrintDrvName = "/dev/lp0";

	if (!QFile(usbPrintDrvName).exists() || pix.isNull())
	{
		return _MSG_LOCAL_PRINTER_NOTFOUND;
	}

	FILE *fw = fopen(usbPrintDrvName, "w");

	if (fw == NULL)
	{
		return _MSG_LOCAL_PRINTER_NOTFOUND;
	}

	if (colour)
	{
		return _MSG_LOCAL_PRINTER_NOTSUPPORT;
	} else
	{
		//var def
		int pWidth = 800;
		int pHeight = 600;

		QPixmap pixmap(pWidth, pHeight);
		pixmap = pix.scaled(pWidth, pHeight);
		QBitmap bitMap = pixmap.createMaskFromColor(Qt::black);
		unsigned int cols = bitMap.width() / 8;
		unsigned int rows = bitMap.height();
		unsigned char cc;
		QByteArray bArray;
		QBuffer buffer(&bArray);
		bitMap.save(&buffer, "bmp");

		//remove head info
		if (bArray.length() > 62)
		{
			bArray.remove(0, 62);
		}

		//head
		fprintf(fw, "\33%%-12345X");
		fprintf(fw, "@PJL ENTER LANGUAGE=PCL\n");
		fprintf(fw, "\33E");
		fprintf(fw, "\033&l%dX", copyNums); //打印份数
		fprintf(fw, "\033&l%dO", orientation); //打印方向    0: 纵向           1：横向
		fprintf(fw, "\033&l%dA", pageSize); //纸张大小    2: Letter  25:A5   26:A4   27：A3  28:A2  29:A1
		fprintf(fw, "\033*p250x200Y"); //起始坐标
		fprintf(fw, "\033*t%dR", 96); //分辨率   96
		fprintf(fw, "\033*b0M");
		fprintf(fw, "\033*r0F");
		fprintf(fw, "\033*r1A");

		//data
		for (int i = rows - 1; i >= 0; i--)
		{
			fprintf(fw, "\033*b%dW", cols);

			for (int j = 0; j < cols; j++)
			{
				cc = (bArray[i * cols + j]);
				fwrite(&cc, sizeof(unsigned char), 1, fw);
			}
		}

		//end
		fprintf(fw, "\033*rC");
		fprintf(fw, "\033*p1200x2200Y"); // 打印TEXT
		fprintf(fw, "\033&a45L");
		fprintf(fw, "\033&a80M");
		fprintf(fw, "\033&l45E");
		fprintf(fw, "\33&t31P");
		fprintf(fw, "\33(19K");
		fprintf(fw, "\33(s1p%dv0s0b28752T", 15);
		fprintf(fw, "Page 1");
		fprintf(fw, "\f");
		fprintf(fw, "\33%%-12345X");
	}

	fclose(fw);

	return _RESULT_SUCCESS;
}

bool tSysScpi::updateRecordOfClear(void)
{
	QFile file(QCoreApplication::applicationDirPath() + "/update/update");

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
		QFile file(QCoreApplication::applicationDirPath() + "/update/update");

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

		updatingFileName = QCoreApplication::applicationDirPath() + "/update/temp" + QString(intToString(strList.count(), tempChar)).trimmed();
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
	QFile file(QCoreApplication::applicationDirPath() + "/update/update");

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
		QFile f("/home/sky/install");
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
				str1 = "file1=" + QCoreApplication::applicationDirPath() + "/update/" + sl.at(0).trimmed();
				str2 = "file2=" + sl.at(1).trimmed();
				ts << str1 << endl;
				ts << str2 << endl;
				//cmdString = "rm " + sl.at(1);
				//system(cmdString.toStdString().c_str());
				//cmdString = "mv " + QCoreApplication::applicationDirPath() + "/update/" + sl.at(0) + " " + sl.at(1);
				//system(cmdString.toStdString().c_str());
			}

			str3 = "if [ -f $file2 ]; then rm -f $file2; fi";
			ts << str3 << endl;

			if (sl.count() >= 3)
			{
				str3 = "if [ -f $file1 ]; then mv -f $file1 $file2; chmod " + sl.at(2).trimmed() + " $file2; fi";
				ts << str3 << endl;
				//cmdString = "chmod " + sl.at(2) + " " + sl.at(1);
				//system(cmdString.toStdString().c_str());
			} else
			{
				str3 = "if [ -f $file1 ]; then mv -f $file1 $file2; fi";
				ts << str3 << endl;
			}
		}

		if (strList.count() > 0)
		{
			//cmdString = "rm " + QCoreApplication::applicationDirPath() + "/update/*";
			//system(cmdString.toStdString().c_str());
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

//DDC控制
void tSysScpi::ddcControl(double value)
{
	if (rfHandle >= 0)
	{
		unsigned long freq = (unsigned long) (value / 30e6 * 4294967296);
		ioctl(rfHandle, 0xef, (freq >> 16) & 0xffff); //phase high
		ioctl(rfHandle, 0xee, freq & 0xffff); //phase low
		ioctl(rfHandle, 0x7c, 0x00); //phase data load
		ioctl(rfHandle, 0x10e, 0x00); //cic reset
		usleep(1000);
	}

}

//复位直采参数
void tSysScpi::resetZcParams(void)
{
	sysData.zc.segTotal = 0;
	sysData.zc.cic = 0;
	sysData.zc.cf1 = -0.5;
	sysData.zc.cf2 = 0.5;
	sysData.zc.bucLen = 0;
	sysData.zc.sBucCounter = 0;
	sysData.zc.sBucDetValue = 0;
	sysData.zc.curSegIndex = 0;
	sysData.zc.swt = 0;
	sysData.zc.prvCenterStr = {};
	sysData.zc.nowCenterStr = {};
	sysData.zc.execCount = 0;
	sysData.zc.reseted = false;
}

//重新计算直采参数
void tSysScpi::reCalculateZcParams(void)
{
	double startFreq = sysData.freq.start - sysData.freq.offset;
	double stopFreq = sysData.freq.stop - sysData.freq.offset;
	double span = stopFreq - startFreq; //总扫宽
	double cf1 = 0; //cf1
	double cf2 = 0; //cf2
	unsigned int cic = RAMCIC_ZC[getRbwIndex()]; //CIC抽取率
	double fs = 30e6; //采样率
	//double maxAnalysisBw = fs * 1.0 / (cic * 2.0);        //最大分析带宽
	double maxAnalysisBw = fs * 1.0 / (cic * 4.0); //最大分析带宽
	int swePoints = sysData.sweep.sweepPoints;
	if (swePoints < MINSWEEPPOINTS)
	{
		swePoints = MINSWEEPPOINTS;
	} else if (swePoints > MAXSWEEPPOINTS)
	{
		swePoints = MAXSWEEPPOINTS;
	}

	unsigned int segs = (unsigned int) (ceil(span / maxAnalysisBw));

	if (segs >= 1)
	{
		//cf1 = -0.5 * ((span) / segs) / (fs / (cic * 1.0));
		cf1 = -0.5 * ((span) / segs) / (fs / (cic * 1.0));
		cf2 = qAbs(cf1);

		for (unsigned int i = 0; i < segs; i++)
		{
			//sysData.zc.segs[i].ddcCenter = startFreq + span * (i * 2 + 1) / segs / 2;
			sysData.zc.segs[i].ddcCenter = startFreq + span * (i * 2 + 1) / segs / 2;
		}

		for (int i = 0; i < swePoints; i++)
		{
			sysData.zc.detIndex[i] = (unsigned int) (floor((i + 1) * (segs * FFTSIZE) / swePoints));
		}

		sysData.zc.bucLen = swePoints;
		sysData.zc.cf1 = cf1;
		sysData.zc.cf2 = cf2;
		sysData.zc.segTotal = segs;
		sysData.zc.cic = cic;
		sysData.zc.sBucCounter = 0;
		sysData.zc.curSegIndex = 0;

		//补偿值计算
		for (int i = 0; i < FFTSIZE; i++)
		{
			sysData.zc.cicCompensateValue[i] = ZC_COMPENSTATE_VALUE[0][i] * cf2;
		}

		//扫描时间计算
		double swt = (RAMCIC_ZC[getRbwIndex()] / fs) * FFTSIZE * 1000; //ms
		double totalSwt = 0; //ns

		if (sysData.bw.rbw <= 1e3)
		{
			totalSwt = (swt + 300.0) * segs * 1e6;
		} else
		{
			totalSwt = (swt + 150.0) * segs * 1e6;
		}

		if (totalSwt < MINSWEEPTIME * 1e6)
		{
			totalSwt = MINSWEEPTIME * 1e6;
		} else if (totalSwt > MAXSWEEPTIME * 1e6)
		{
			totalSwt = MAXSWEEPTIME * 1e6;
		}

		//如果扫描时间处于手动状态
		sysData.zc.idleSwt = 0;
		if (!sysData.sweep.sweepTimeAuto)
		{
			if (sysData.sweep.sweepTime <= totalSwt)
			{
				sysData.sweep.sweepTime = totalSwt;
			} else
			{
				sysData.zc.idleSwt = (sysData.sweep.sweepTime - totalSwt) / segs / 1e6;
			}
		} else
		{
			sysData.zc.idleSwt = 0;
			sysData.sweep.sweepTime = totalSwt;
		}

		//时间赋值
		sysData.zc.swt = swt;
		//sysData.sweep.sweepTime = totalSwt;
	}
}

void tSysScpi::checkMultiDisplay(multiDispType value)
{
	if (sysData.display.isZoomOn && value != mdtZoom)
	{
		setDisplayOfZoomOn(0);
	}

	if (sysData.markerTableOn && value != mdtMkrTable)
	{
		setMarkerTable(0);
	}

	if (sysData.peakTableOn && value != mdtPeakTable)
	{
		setPeakTable(0);
	}

	if (sysData.measure.isTimeSpectrumOn && value != mdtTimeSpec)
	{
		setMeasureOfTimeSpectrumOn(0);
	}

	if (sysData.measure.isAcprOn && value != mdtAcpr)
	{
		setMeasureOfAcprOn(0);
	}

	if (sysData.measure.isChannelPowerOn && value != mdtChannelPower)
	{
		setMeasureOfChannelPowerOn(0);
	}

	if (sysData.measure.isObwOn && value != mdtObw)
	{
		setMeasureOfObwOn(0);
	}

	if (sysData.disc.isDiscOn && value != mdtDisc)
	{
		setMeasureOfDiscState(0);
	}

	if (sysData.peakTableOn && value != mdtPeakTable)
	{
		setPeakTable(0);
	}

	if (sysData.measure.passFail.lineMeasOn && value != mdtPassFail)
	{
		setMeasureOfPassFailOfLineMeasState(0);
	}
}

//复位Pass-Fail数据
void tSysScpi::resetPassFailData(void)
{
	memset(&sysData.measure.passFail, 0, sizeof sysData.measure.passFail);

	if (!sysData.measure.passFail.winMeasOn && !sysData.measure.passFail.lineMeasOn)
	{
		/*
		 memset(&sysData.measure.passFail, 0, sizeof sysData.measure.passFail);
		 sysData.measure.passFail.winAmptUp = sysData.ampt.refLevel - sysData.ampt.scaleDiv * 1.0;
		 sysData.measure.passFail.winAmptLow = sysData.ampt.refLevel - sysData.ampt.scaleDiv * 9.0;
		 sysData.measure.passFail.winFreqStart = sysData.freq.center - sysData.span.span / 10.0;
		 sysData.measure.passFail.winFreqStop = sysData.freq.center + sysData.span.span / 10.0;
		 */

		loadPassFailLineData();
	}
}

//加载Pass-Fail迹线定义数据
void tSysScpi::loadPassFailLineData(void)
{

}

//获取Pass-Fail窗口测量结果
bool tSysScpi::getMeasureOfPassFailOfWinMeas(int mkrIndex)
{
	if (!sysData.measure.passFail.winMeasOn || mkrIndex < 0 || mkrIndex >= MARKERCOUNT || sysData.marker[mkrIndex].state == msClose)
	{
		return false;
	}

	double sFreq = sysData.measure.passFail.winFreqStart;
	double eFreq = sysData.measure.passFail.winFreqStop;
	double upAmpt = sysData.measure.passFail.winAmptUp;
	double lowAmpt = sysData.measure.passFail.winAmptLow;

	double mFreq = getMarkerXValue(mkrIndex);
	double mAmpt = getMarkerYValue(mkrIndex);

	return (mFreq >= sFreq && mFreq <= eFreq && mAmpt >= lowAmpt && mAmpt <= upAmpt);
}

//设置Pass-Fail窗口测量状态
int tSysScpi::setMeasureOfPassFailOfWinMeasState(int value)
{
	sysData.measure.passFail.winMeasOn = value == 1;

	if (sysData.measure.passFail.winMeasOn)
	{
		if (sysData.measure.passFail.lineMeasOn)
		{
			setMeasureOfPassFailOfLineMeasState(0);
		}

		setMarkerAllOff();

		if (isMarkerAllOff() == __SCPI_SUCCESS)
		{
			setMarkerNormal(sysData.markerSelIndex, sysData.traceSelIndex);
		} else if (isTheMarkerOff(sysData.markerSelIndex, sysData.marker[sysData.markerSelIndex].traceIndex) == __SCPI_SUCCESS)
		{
			setMarkerNormal(sysData.markerSelIndex, sysData.marker[sysData.markerSelIndex].traceIndex);
		}

		sysData.measure.passFail.winAmptOn = true;
		sysData.measure.passFail.winFreqOn = true;
		setFrequencyOffset(0);
		setAmptOfRefUnit(refUnits(rudBm));
		setAmptOfRefOffset(0);
		sysData.mode = smSpectrum;
		sysData.canvasNums = 1;
		sysData.isAutoPeakMaxing = true;
	} else
	{
		sysData.measure.passFail.winAmptOn = false;
		sysData.measure.passFail.winFreqOn = false;
	}

	return __SCPI_SUCCESS;
}

//设置Pass-Fail窗口测量状态
int tSysScpi::setMeasureOfPassFailOfWinMeasState(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	int tempValue = getBoolValue(value.toUpper().trimmed(), &isOk);

	if (isOk)
	{
		return setMeasureOfPassFailOfWinMeasState(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置Pass-Fail幅值线状态
int tSysScpi::setMeasureOfPassFailOfWinLimitLineState(int value)
{
	sysData.measure.passFail.winAmptOn = value == 1;

	return __SCPI_SUCCESS;
}

//设置Pass-Fail幅值线状态
int tSysScpi::setMeasureOfPassFailOfWinLimitLineState(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	int tempValue = getBoolValue(value.toUpper().trimmed(), &isOk);

	if (isOk)
	{
		return setMeasureOfPassFailOfWinLimitLineState(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置Pass-Fail频率线状态
int tSysScpi::setMeasureOfPassFailOfWinFreqLineState(int value)
{
	sysData.measure.passFail.winFreqOn = value == 1;

	return __SCPI_SUCCESS;
}

//设置Pass-Fail频率线状态
int tSysScpi::setMeasureOfPassFailOfWinFreqLineState(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	int tempValue = getBoolValue(value.toUpper().trimmed(), &isOk);

	if (isOk)
	{
		return setMeasureOfPassFailOfWinFreqLineState(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置Pass-Fail限制线索引
int tSysScpi::setMeasureOfPassFailOfWinLineValueMode(int value)
{
	sysData.measure.passFail.winNowAmptUpOrLow = value;

	return __SCPI_SUCCESS;
}

//设置Pass-Fail限制线索引
int tSysScpi::setMeasureOfPassFailOfWinLineValueMode(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	int tempValue = getBoolValue(value.toUpper().trimmed(), &isOk);

	if (isOk)
	{
		return setMeasureOfPassFailOfWinLineValueMode(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置Pass-Fail限制线幅值
int tSysScpi::setMeasureOfPassFailOfWinLineValue(double value)
{
	double maxValue = MAXREFLEVELDBM + sysData.ampt.refOffset;
	double minValue = MINREFLEVELDBM + sysData.ampt.refOffset - 10.0 * sysData.ampt.scaleDiv;
	double tempValue = value;

	if (tempValue > maxValue)
	{
		tempValue = maxValue;
	} else if (tempValue < minValue)
	{
		tempValue = minValue;
	}

	if (sysData.measure.passFail.winNowAmptUpOrLow == 0) //up
	{
		sysData.measure.passFail.winAmptUp = tempValue;

		if (sysData.measure.passFail.winAmptLow > tempValue)
		{
			sysData.measure.passFail.winAmptLow = tempValue;
		}
	} else //low
	{
		sysData.measure.passFail.winAmptLow = tempValue;

		if (sysData.measure.passFail.winAmptUp < tempValue)
		{
			sysData.measure.passFail.winAmptUp = tempValue;
		}
	}

	return __SCPI_SUCCESS;
}

//设置Pass-Fail限制线幅值
int tSysScpi::setMeasureOfPassFailOfWinLineValue(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	double tempValue = getAmptValue(value.toUpper().trimmed(), &isOk);

	if (isOk)
	{
		return setMeasureOfPassFailOfWinLineValue(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//方向键设置Pass-Fail限制线幅值
int tSysScpi::setMeasureOfPassFailOfWinLineValueWithArrow(bool up)
{
	double tempValue = 0;
	double stepValue = sysData.ampt.scaleDiv;

	if (sysData.measure.passFail.winNowAmptUpOrLow == 0)
	{
		tempValue = sysData.measure.passFail.winAmptUp;
	} else
	{
		tempValue = sysData.measure.passFail.winAmptLow;
	}

	if (up)
	{
		return setMeasureOfPassFailOfWinLineValue(tempValue + stepValue);
	} else
	{
		return setMeasureOfPassFailOfWinLineValue(tempValue - stepValue);
	}
}

//脉冲旋钮设置Pass-Fail限制线幅值
int tSysScpi::setMeasureOfPassFailOfWinLineValueWithRpg(int value)
{
	double tempValue = 0;
	double stepValue = sysData.ampt.scaleDiv * value / 100.0;

	if (sysData.measure.passFail.winNowAmptUpOrLow == 0)
	{
		tempValue = sysData.measure.passFail.winAmptUp;
	} else
	{
		tempValue = sysData.measure.passFail.winAmptLow;
	}

	return setMeasureOfPassFailOfWinLineValue(tempValue + stepValue);
}

//设置Pass-Fail频率索引
int tSysScpi::setMeasureOfPassFailOfWinFreqValueMode(int value)
{
	sysData.measure.passFail.winNowFreqStartOrStop = value;

	return __SCPI_SUCCESS;
}

//设置Pass-Fail频率索引
int tSysScpi::setMeasureOfPassFailOfWinFreqValueMode(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	int tempValue = getBoolValue(value.toUpper().trimmed(), &isOk);

	if (isOk)
	{
		return setMeasureOfPassFailOfWinFreqValueMode(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置Pass-Fail频率
int tSysScpi::setMeasureOfPassFailOfWinFreqValue(double value)
{
	double maxValue = sysData.freq.stop;
	double minValue = sysData.freq.start;
	double tempValue = value;

	if (tempValue > maxValue)
	{
		tempValue = maxValue;
	} else if (tempValue < minValue)
	{
		tempValue = minValue;
	}

	if (sysData.measure.passFail.winNowFreqStartOrStop == 0)
	{
		sysData.measure.passFail.winFreqStart = tempValue;

		if (sysData.measure.passFail.winFreqStop < tempValue)
		{
			sysData.measure.passFail.winFreqStop = tempValue;
		}
	} else
	{
		sysData.measure.passFail.winFreqStop = tempValue;

		if (sysData.measure.passFail.winFreqStart > tempValue)
		{
			sysData.measure.passFail.winFreqStart = tempValue;
		}
	}

	return __SCPI_SUCCESS;
}

//设置Pass-Fail频率
int tSysScpi::setMeasureOfPassFailOfWinFreqValue(QString value)
{
	bool isOk = false;
	double tempValue = 0;

	tempValue = getFreqValue(value, &isOk);

	if (isOk)
	{
		return setMeasureOfPassFailOfWinFreqValue(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//方向键设置Pass-Fail频率
int tSysScpi::setMeasureOfPassFailOfWinFreqValueWithArrow(bool up)
{
	double tempValue = 0;
	double stepValue = sysData.span.span / 10.0;

	if (sysData.measure.passFail.winNowFreqStartOrStop == 0)
	{
		tempValue = sysData.measure.passFail.winFreqStart;
	} else
	{
		tempValue = sysData.measure.passFail.winFreqStop;
	}

	if (up)
	{
		return setMeasureOfPassFailOfWinFreqValue(tempValue + stepValue);
	} else
	{
		return setMeasureOfPassFailOfWinFreqValue(tempValue - stepValue);
	}
}

//脉冲旋钮设置Pass-Fail频率
int tSysScpi::setMeasureOfPassFailOfWinFreqValueWithRpg(int value)
{
	double tempValue = 0;
	double stepValue = sysData.span.span * value / 100.0;

	if (sysData.measure.passFail.winNowFreqStartOrStop == 0)
	{
		tempValue = sysData.measure.passFail.winFreqStart;
	} else
	{
		tempValue = sysData.measure.passFail.winFreqStop;
	}

	return setMeasureOfPassFailOfWinFreqValue(tempValue + stepValue);
}

//设置Pass-Fail窗口扫描状态
int tSysScpi::setMeasureOfPassFailOfWinSweepState(int value)
{
	sysData.measure.passFail.winSweepOn = value;

	return __SCPI_SUCCESS;
}

//设置Pass-Fail窗口扫描状态
int tSysScpi::setMeasureOfPassFailOfWinSweepState(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	int tempValue = getBoolValue(value.toUpper().trimmed(), &isOk);

	if (isOk)
	{
		return setMeasureOfPassFailOfWinSweepState(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//获取Pass-Fail迹线测量结果
bool tSysScpi::getMeasureOfPassFailOfLineMeas(int mkrIndex, double canvasMinValue, double canvasMaxValue)
{
	if (!sysData.measure.passFail.lineMeasOn || mkrIndex < 0 || mkrIndex >= MARKERCOUNT || sysData.marker[mkrIndex].state == msClose || sysData.measure.passFail.line1TotalPoints <= 0 || sysData.measure.passFail.line2TotalPoints <= 0)
	{
		return false;
	}

	int points1 = sysData.measure.passFail.line1TotalPoints;
	int points2 = sysData.measure.passFail.line2TotalPoints;
	int points = points1 + points2;

	if (points <= 2 || points1 <= 1 || points2 <= 1)
	{
		return false;
	}

	QPolygonF pf(points);

	double sFreq = sysData.freq.start + sysData.freq.offset;
	double eFreq = sysData.freq.stop + sysData.freq.offset;
	double upLimit = sysData.ampt.refLevel;
	double lowLimit = sysData.ampt.refLevel - 10.0 * sysData.ampt.scaleDiv;
	double freq = 0;
	double ampt = 0;
	int index = 0;

	if ((canvasMinValue - canvasMaxValue) >= 0)
	{
		return false;
	}

	for (int i = 0; i < points1; i++)
	{
		freq = sysData.measure.passFail.line1Datas[i].freq + sysData.measure.passFail.lineShiftXOffset;
		ampt = sysData.measure.passFail.line1Datas[i].ampt + sysData.measure.passFail.lineShiftYOffset;

		if (freq < sFreq)
		{
			freq = sFreq;
		} else if (freq > eFreq)
		{
			freq = eFreq;
		}

		if (ampt > upLimit)
		{
			ampt = upLimit;
		} else if (ampt < lowLimit)
		{
			ampt = lowLimit;
		}

		pf[index++] = QPointF((freq - sFreq) * (sysData.sweep.sweepPoints - 1) / (eFreq - sFreq), (ampt - canvasMaxValue) * (sysData.ampt.scaleDiv * 10.0) / (canvasMinValue - canvasMaxValue));
	}

	for (int i = points2 - 1; i >= 0; i--)
	{
		freq = sysData.measure.passFail.line2Datas[i].freq + sysData.measure.passFail.lineShiftXOffset;
		ampt = sysData.measure.passFail.line2Datas[i].ampt + sysData.measure.passFail.lineShiftYOffset;

		if (freq < sFreq)
		{
			freq = sFreq;
		} else if (freq > eFreq)
		{
			freq = eFreq;
		}

		if (ampt > upLimit)
		{
			ampt = upLimit;
		} else if (ampt < lowLimit)
		{
			ampt = lowLimit;
		}

		pf[index++] = QPointF((freq - sFreq) * (sysData.sweep.sweepPoints - 1) / (eFreq - sFreq), (ampt - canvasMaxValue) * (sysData.ampt.scaleDiv * 10.0) / (canvasMinValue - canvasMaxValue));
	}

	double minFreq1 = sysData.measure.passFail.line1Datas[0].freq;
	double minFreq2 = sysData.measure.passFail.line1Datas[0].freq;
	double minFreq = (minFreq1 > minFreq2) ? minFreq2 : minFreq1;
	double maxFreq1 = sysData.measure.passFail.line1Datas[points1 - 1].freq;
	double maxFreq2 = sysData.measure.passFail.line1Datas[points2 - 1].freq;
	double maxFreq = (maxFreq1 > maxFreq2) ? maxFreq2 : maxFreq1;
	bool isOk = false;
	QPointF freqPoint;
	double objFreq = 0;

	QPainterPath path;
	path.addPolygon(pf);

	for (int i = 0; i < sysData.sweep.sweepPoints; i++)
	{
		objFreq = sysData.freq.start + sysData.span.span * i / sysData.sweep.sweepPoints;
		ampt = sysData.trace[sysData.traceSelIndex].value[i];

		if (objFreq < minFreq || objFreq > maxFreq)
		{
			continue;
		}

		freqPoint = QPointF((objFreq - sFreq) * (sysData.sweep.sweepPoints - 1) / (eFreq - sFreq), (ampt - canvasMaxValue) * (sysData.ampt.scaleDiv * 10.0) / (canvasMinValue - canvasMaxValue));
		isOk = path.contains(freqPoint);

		if (!isOk)
		{
			break;
		}
	}

	return isOk;

	/*

	 QPointF mPeak = QPointF((getMarkerXValue(mkrIndex) - sFreq) * (sysData.sweep.sweepPoints - 1) / (eFreq - sFreq),
	 (getMarkerYValue(mkrIndex) - canvasMaxValue) * (sysData.ampt.scaleDiv * 10.0) / (canvasMinValue - canvasMaxValue));
	 QPainterPath path;
	 path.addPolygon(pf);

	 return path.contains(mPeak);
	 */
}

//刷新Pass-Fail迹线测量上线页
void tSysScpi::refreshMeasureOfPassFailOLineUpPages(void)
{
	int pages = sysData.measure.passFail.line1TotalPoints / PASSFAILCOUNTPERPAGE;

	if ((sysData.measure.passFail.line1TotalPoints % PASSFAILCOUNTPERPAGE) > 0)
	{
		pages++;
	}

	sysData.measure.passFail.line1Pages = pages;
	sysData.measure.passFail.line1NowPageIndex = 0;
	sysData.measure.passFail.line1EditNowSelIndex = 0;

	if (sysData.measure.passFail.line1NowPageIndex == pages - 1)
	{
		sysData.measure.passFail.line1NowPagePoints = sysData.measure.passFail.line1TotalPoints - sysData.measure.passFail.line1NowPageIndex * PASSFAILCOUNTPERPAGE;
	} else if (sysData.measure.passFail.line1TotalPoints > 0)
	{
		sysData.measure.passFail.line1NowPagePoints = PASSFAILCOUNTPERPAGE;
	} else
	{
		sysData.measure.passFail.line1NowPagePoints = 0;
	}
}

//刷新Pass-Fail迹线测量下线页
void tSysScpi::refreshMeasureOfPassFailOLineLowPages(void)
{
	int pages = sysData.measure.passFail.line2TotalPoints / PASSFAILCOUNTPERPAGE;

	if ((sysData.measure.passFail.line2TotalPoints % PASSFAILCOUNTPERPAGE) > 0)
	{
		pages++;
	}

	sysData.measure.passFail.line2Pages = pages;
	sysData.measure.passFail.line2NowPageIndex = 0;
	sysData.measure.passFail.line2EditNowSelIndex = 0;

	if (sysData.measure.passFail.line2NowPageIndex == pages - 1)
	{
		sysData.measure.passFail.line2NowPagePoints = sysData.measure.passFail.line2TotalPoints - sysData.measure.passFail.line2NowPageIndex * PASSFAILCOUNTPERPAGE;
	} else if (sysData.measure.passFail.line2TotalPoints > 0)
	{
		sysData.measure.passFail.line2NowPagePoints = PASSFAILCOUNTPERPAGE;
	} else
	{
		sysData.measure.passFail.line2NowPagePoints = 0;
	}
}

//增加 Pass-Fail迹线测量点
void tSysScpi::addMeasureOfPassFailOLineUpPoint(void)
{
	int points = 0;
	int pages = 0;

	if (sysData.measure.passFail.line1Editing)
	{
		points = sysData.measure.passFail.line1TotalPoints;
	} else if (sysData.measure.passFail.line2Editing)
	{
		points = sysData.measure.passFail.line2TotalPoints;
	} else
	{
		return;
	}

	if (points <= 0)
	{
		points = 0;
	}

	points++;

	if (points > MAXPASSFAILROWS)
	{
		return;
	}

	mutexPassFail.lock();

	if (sysData.measure.passFail.line1Editing)
	{
		sysData.measure.passFail.line1TotalPoints = points;
		pages = points / PASSFAILCOUNTPERPAGE;

		if ((points % PASSFAILCOUNTPERPAGE) > 0)
		{
			pages++;
		}

		sysData.measure.passFail.line1Pages = pages;
		sysData.measure.passFail.line1NowPageIndex = pages - 1;
		sysData.measure.passFail.line1NowPagePoints = points - sysData.measure.passFail.line1NowPageIndex * PASSFAILCOUNTPERPAGE;
		sysData.measure.passFail.line1EditNowSelIndex = sysData.measure.passFail.line1NowPagePoints - 1;
		sysData.measure.passFail.line1Datas[points - 1].freq = sysData.freq.center;
		sysData.measure.passFail.line1Datas[points - 1].ampt = sysData.ampt.refLevel;
	} else if (sysData.measure.passFail.line2Editing)
	{
		sysData.measure.passFail.line2TotalPoints = points;
		pages = points / PASSFAILCOUNTPERPAGE;

		if ((points % PASSFAILCOUNTPERPAGE) > 0)
		{
			pages++;
		}

		sysData.measure.passFail.line2Pages = pages;
		sysData.measure.passFail.line2NowPageIndex = pages - 1;
		sysData.measure.passFail.line2NowPagePoints = points - sysData.measure.passFail.line2NowPageIndex * PASSFAILCOUNTPERPAGE;
		sysData.measure.passFail.line2EditNowSelIndex = sysData.measure.passFail.line2NowPagePoints - 1;
		sysData.measure.passFail.line2Datas[points - 1].freq = sysData.freq.center;
		sysData.measure.passFail.line2Datas[points - 1].ampt = sysData.ampt.refLevel;
	}

	mutexPassFail.unlock();
}

//插入 Pass-Fail迹线测量点
void tSysScpi::insertMeasureOfPassFailOLineUpPoint(void)
{
	int points = 0;
	int pages = 0;

	if (sysData.measure.passFail.line1Editing)
	{
		points = sysData.measure.passFail.line1TotalPoints;
	} else if (sysData.measure.passFail.line2Editing)
	{
		points = sysData.measure.passFail.line2TotalPoints;
	} else
	{
		return;
	}

	if (points <= 0)
	{
		points = 0;
	}

	points++;

	if (points > MAXPASSFAILROWS)
	{
		return;
	}

	int nowPagePoints = 0;
	int absIndex = 0;

	mutexPassFail.lock();

	if (sysData.measure.passFail.line1Editing)
	{
		sysData.measure.passFail.line1TotalPoints = points;
		pages = points / PASSFAILCOUNTPERPAGE;

		if ((points % PASSFAILCOUNTPERPAGE) > 0)
		{
			pages++;
		}

		sysData.measure.passFail.line1Pages = pages;
		nowPagePoints = sysData.measure.passFail.line1NowPagePoints;
		nowPagePoints++;

		if (nowPagePoints > PASSFAILCOUNTPERPAGE)
		{
			nowPagePoints = PASSFAILCOUNTPERPAGE;
		}

		absIndex = sysData.measure.passFail.line1EditNowSelIndex + sysData.measure.passFail.line1NowPageIndex * PASSFAILCOUNTPERPAGE;

		if (absIndex < 0)
		{
			absIndex = 0;
		}

		for (int i = points - 1; i > absIndex; i--)
		{
			sysData.measure.passFail.line1Datas[i] = sysData.measure.passFail.line1Datas[i - 1];
		}

		sysData.measure.passFail.line1NowPagePoints = nowPagePoints;
		sysData.measure.passFail.line1Datas[absIndex].freq = sysData.freq.center;
		sysData.measure.passFail.line1Datas[absIndex].ampt = sysData.ampt.refLevel;
	} else if (sysData.measure.passFail.line2Editing)
	{
		sysData.measure.passFail.line2TotalPoints = points;
		pages = points / PASSFAILCOUNTPERPAGE;

		if ((points % PASSFAILCOUNTPERPAGE) > 0)
		{
			pages++;
		}

		sysData.measure.passFail.line2Pages = pages;
		nowPagePoints = sysData.measure.passFail.line2NowPagePoints;
		nowPagePoints++;

		if (nowPagePoints > PASSFAILCOUNTPERPAGE)
		{
			nowPagePoints = PASSFAILCOUNTPERPAGE;
		}

		absIndex = sysData.measure.passFail.line2EditNowSelIndex + sysData.measure.passFail.line2NowPageIndex * PASSFAILCOUNTPERPAGE;

		if (absIndex < 0)
		{
			absIndex = 0;
		}

		for (int i = points - 1; i > absIndex; i--)
		{
			sysData.measure.passFail.line2Datas[i] = sysData.measure.passFail.line2Datas[i - 1];
		}

		sysData.measure.passFail.line2NowPagePoints = nowPagePoints;
		sysData.measure.passFail.line2Datas[absIndex].freq = sysData.freq.center;
		sysData.measure.passFail.line2Datas[absIndex].ampt = sysData.ampt.refLevel;
	}

	mutexPassFail.unlock();
}

//删除 Pass-Fail迹线测量点
void tSysScpi::deleteMeasureOfPassFailOLineUpPoint(void)
{
	mutexPassFail.lock();

	int absIndex = 0;
	int prvSelIndex = 0;
	int prvPageIndex = 0;
	int pages = 0;

	if (sysData.measure.passFail.line1Editing && sysData.measure.passFail.line1TotalPoints > 0)
	{
		prvSelIndex = sysData.measure.passFail.line1EditNowSelIndex;
		prvPageIndex = sysData.measure.passFail.line1NowPageIndex;

		absIndex = sysData.measure.passFail.line1EditNowSelIndex + sysData.measure.passFail.line1NowPageIndex * MAXPASSFAILROWS;

		for (int i = absIndex; i < sysData.measure.passFail.line1TotalPoints - 1; i++)
		{
			sysData.measure.passFail.line1Datas[i] = sysData.measure.passFail.line1Datas[i + 1];
		}

		sysData.measure.passFail.line1Datas[sysData.measure.passFail.line1TotalPoints - 1].freq = 0;
		sysData.measure.passFail.line1Datas[sysData.measure.passFail.line1TotalPoints - 1].ampt = 0;
		sysData.measure.passFail.line1TotalPoints--;
		pages = sysData.measure.passFail.line1TotalPoints / PASSFAILCOUNTPERPAGE;

		if ((sysData.measure.passFail.line1TotalPoints % PASSFAILCOUNTPERPAGE) > 0)
		{
			pages++;
		}

		sysData.measure.passFail.line1Pages = pages;

		if (pages <= 0)
		{
			sysData.measure.passFail.line1Pages = 0;
			sysData.measure.passFail.line1EditNowSelIndex = 0;
			sysData.measure.passFail.line1NowPageIndex = 0;
			sysData.measure.passFail.line1Pages = 0;
			sysData.measure.passFail.line1NowPagePoints = 0;
		} else
		{
			if (sysData.measure.passFail.line1NowPageIndex > pages - 1)
			{
				sysData.measure.passFail.line1NowPageIndex = pages - 1;
			}

			if (sysData.measure.passFail.line1NowPageIndex == pages - 1)
			{
				sysData.measure.passFail.line1NowPagePoints = sysData.measure.passFail.line1TotalPoints - sysData.measure.passFail.line1NowPageIndex * PASSFAILCOUNTPERPAGE;
			}

			if (prvPageIndex < sysData.measure.passFail.line1Pages)
			{
				sysData.measure.passFail.line1NowPageIndex = prvPageIndex;
			} else
			{
				sysData.measure.passFail.line1NowPageIndex = sysData.measure.passFail.line1Pages - 1;
			}

			if (prvSelIndex < sysData.measure.passFail.line1NowPagePoints)
			{
				sysData.measure.passFail.line1EditNowSelIndex = prvSelIndex;
			} else
			{
				sysData.measure.passFail.line1EditNowSelIndex = sysData.measure.passFail.line1NowPagePoints - 1;
			}
		}
	} else if (sysData.measure.passFail.line2Editing && sysData.measure.passFail.line2TotalPoints > 0)
	{
		prvSelIndex = sysData.measure.passFail.line2EditNowSelIndex;
		prvPageIndex = sysData.measure.passFail.line2NowPageIndex;

		absIndex = sysData.measure.passFail.line2EditNowSelIndex + sysData.measure.passFail.line2NowPageIndex * MAXPASSFAILROWS;

		for (int i = absIndex; i < sysData.measure.passFail.line2TotalPoints - 1; i++)
		{
			sysData.measure.passFail.line2Datas[i] = sysData.measure.passFail.line2Datas[i + 1];
		}

		sysData.measure.passFail.line2Datas[sysData.measure.passFail.line2TotalPoints - 1].freq = 0;
		sysData.measure.passFail.line2Datas[sysData.measure.passFail.line2TotalPoints - 1].ampt = 0;

		sysData.measure.passFail.line2TotalPoints--;
		pages = sysData.measure.passFail.line2TotalPoints / PASSFAILCOUNTPERPAGE;

		if ((sysData.measure.passFail.line2TotalPoints % PASSFAILCOUNTPERPAGE) > 0)
		{
			pages++;
		}

		sysData.measure.passFail.line2Pages = pages;

		if (pages <= 0)
		{
			sysData.measure.passFail.line2Pages = 0;
			sysData.measure.passFail.line2EditNowSelIndex = 0;
			sysData.measure.passFail.line2NowPageIndex = 0;
			sysData.measure.passFail.line2Pages = 0;
			sysData.measure.passFail.line2NowPagePoints = 0;
		} else
		{
			if (sysData.measure.passFail.line2NowPageIndex > pages - 1)
			{
				sysData.measure.passFail.line2NowPageIndex = pages - 1;
			}

			if (sysData.measure.passFail.line2NowPageIndex == pages - 1)
			{
				sysData.measure.passFail.line2NowPagePoints = sysData.measure.passFail.line2TotalPoints - sysData.measure.passFail.line2NowPageIndex * PASSFAILCOUNTPERPAGE;
			}

			if (prvPageIndex < sysData.measure.passFail.line2Pages)
			{
				sysData.measure.passFail.line2NowPageIndex = prvPageIndex;
			} else
			{
				sysData.measure.passFail.line2NowPageIndex = sysData.measure.passFail.line2Pages - 1;
			}

			if (prvSelIndex < sysData.measure.passFail.line2NowPagePoints)
			{
				sysData.measure.passFail.line2EditNowSelIndex = prvSelIndex;
			} else
			{
				sysData.measure.passFail.line2EditNowSelIndex = sysData.measure.passFail.line2NowPagePoints - 1;
			}
		}
	}

	mutexPassFail.unlock();
}

//清除 Pass-Fail迹线测量点
void tSysScpi::clearMeasureOfPassFailOLineUpPoint(void)
{
	mutexPassFail.lock();

	if (sysData.measure.passFail.line1Editing)
	{
		sysData.measure.passFail.line1Pages = 0;
		memset(sysData.measure.passFail.line1Datas, 0, sizeof sysData.measure.passFail.line1Datas);
		sysData.measure.passFail.line1EditNowSelIndex = 0;
		sysData.measure.passFail.line1TotalPoints = 0;
		sysData.measure.passFail.line1NowPageIndex = 0;
		sysData.measure.passFail.line1NowPagePoints = 0;
	} else if (sysData.measure.passFail.line2Editing)
	{
		sysData.measure.passFail.line2Pages = 0;
		memset(sysData.measure.passFail.line2Datas, 0, sizeof sysData.measure.passFail.line2Datas);
		sysData.measure.passFail.line2EditNowSelIndex = 0;
		sysData.measure.passFail.line2TotalPoints = 0;
		sysData.measure.passFail.line2NowPageIndex = 0;
		sysData.measure.passFail.line2NowPagePoints = 0;
	}

	mutexPassFail.unlock();
}

//设置Pass-Fail迹线测量状态
int tSysScpi::setMeasureOfPassFailOfLineMeasState(int value)
{
	sysData.measure.passFail.lineMeasOn = value == 1;

	if (sysData.measure.passFail.lineMeasOn)
	{
		if (sysData.measure.passFail.winMeasOn)
		{
			setMeasureOfPassFailOfWinMeasState(0);
		}

		setMarkerAllOff();

		if (isMarkerAllOff() == __SCPI_SUCCESS)
		{
			setMarkerNormal(sysData.markerSelIndex, sysData.traceSelIndex);
		} else if (isTheMarkerOff(sysData.markerSelIndex, sysData.marker[sysData.markerSelIndex].traceIndex) == __SCPI_SUCCESS)
		{
			setMarkerNormal(sysData.markerSelIndex, sysData.marker[sysData.markerSelIndex].traceIndex);
		}

		sysData.measure.passFail.line1On = true;
		sysData.measure.passFail.line2On = true;
		setFrequencyOffset(0);
		setAmptOfRefUnit(refUnits(rudBm));
		setAmptOfRefOffset(0);
		sysData.mode = smSpectrum;
		sysData.canvasNums = 1;
		sysData.isAutoPeakMaxing = true;
	} else
	{
		sysData.measure.passFail.line1On = false;
		sysData.measure.passFail.line2On = false;
	}

	return __SCPI_SUCCESS;
}

//设置Pass-Fail迹线测量状态
int tSysScpi::setMeasureOfPassFailOfLineMeasState(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	int tempValue = getBoolValue(value.toUpper().trimmed(), &isOk);

	if (isOk)
	{
		return setMeasureOfPassFailOfLineMeasState(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置Pass-Fail迹线测量上线状态
int tSysScpi::setMeasureOfPassFailOfLineUpState(int value)
{
	sysData.measure.passFail.line1On = value == 1;

	return __SCPI_SUCCESS;
}

//设置Pass-Fail迹线测量上线状态
int tSysScpi::setMeasureOfPassFailOfLineUpState(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	int tempValue = getBoolValue(value.toUpper().trimmed(), &isOk);

	if (isOk)
	{
		return setMeasureOfPassFailOfLineUpState(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置Pass-Fail迹线测量下线状态
int tSysScpi::setMeasureOfPassFailOfLineLowState(int value)
{
	sysData.measure.passFail.line2On = value == 1;

	return __SCPI_SUCCESS;
}

//设置Pass-Fail迹线测量下线状态
int tSysScpi::setMeasureOfPassFailOfLineLowState(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	int tempValue = getBoolValue(value.toUpper().trimmed(), &isOk);

	if (isOk)
	{
		return setMeasureOfPassFailOfLineLowState(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置Pass-Fail迹线测量偏置索引
int tSysScpi::setMeasureOfPassFailOfLineShiftMode(int value)
{
	sysData.measure.passFail.lineNowShiftXOrY = value;

	return __SCPI_SUCCESS;
}

//设置Pass-Fail迹线测量偏置索引
int tSysScpi::setMeasureOfPassFailOfLineShiftMode(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	int tempValue = getBoolValue(value.toUpper().trimmed(), &isOk);

	if (isOk)
	{
		return setMeasureOfPassFailOfLineShiftMode(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//设置Pass-Fail迹线测量偏置
int tSysScpi::setMeasureOfPassFailOfLineShiftXYValue(double value)
{
	double tempValue = value;
	double minValue = 0;
	double maxValue = 0;

	if (sysData.measure.passFail.lineNowShiftXOrY == 0)
	{
		minValue = MINFREQOFFSET;
		maxValue = MAXFREQOFFSET;
	} else
	{
		minValue = -1.0 * MAXREFOFFSET;
		maxValue = MAXREFOFFSET;
	}

	if (tempValue < minValue)
	{
		tempValue = minValue;
	} else if (tempValue > maxValue)
	{
		tempValue = maxValue;
	}

	if (sysData.measure.passFail.lineNowShiftXOrY == 0)
	{
		sysData.measure.passFail.lineShiftXOffset = tempValue;
	} else
	{
		sysData.measure.passFail.lineShiftYOffset = tempValue;
	}

	return __SCPI_SUCCESS;
}

//设置Pass-Fail迹线测量偏置
int tSysScpi::setMeasureOfPassFailOfLineShiftXYValue(QString value)
{

}

//方向键设置Pass-Fail迹线测量偏置
int tSysScpi::setMeasureOfPassFailOfLineShiftXYValueWithArrow(bool up)
{

}

//脉冲旋钮设置Pass-Fail迹线测量偏置
int tSysScpi::setMeasureOfPassFailOfLineShiftXYValueWithRpg(int value)
{

}

//获取Pass-Fail迹线测量上线页数
int tSysScpi::getMeasureOfPassFailOfLineMeasOfLineUpPages(void)
{

}

//获取Pass-Fail迹线测量下线页数
int tSysScpi::getMeasureOfPassFailOfLineMeasOfLineLowPages(void)
{

}

//设置Pass-Fail迹线频率
int tSysScpi::setMeasureOfPassFailOfLineFreq(double value)
{
	if (!sysData.measure.passFail.line1Editing && !sysData.measure.passFail.line2Editing)
	{
		return __SCPI_FAILED;
	}

	double tempValue = value;
	int absIndex = 0;

	if (tempValue > MAXFREQ)
	{
		tempValue = MAXFREQ;
	} else if (tempValue < MINFREQ)
	{
		tempValue = MINFREQ;
	}

	if (sysData.measure.passFail.line1Editing && sysData.measure.passFail.line1TotalPoints > 0 && sysData.measure.passFail.line1EditNowSelIndex >= 0 && sysData.measure.passFail.line1EditNowSelIndex < sysData.measure.passFail.line1NowPagePoints)
	{
		setFrequencyOffset(0);
		absIndex = sysData.measure.passFail.line1EditNowSelIndex + sysData.measure.passFail.line1NowPageIndex * PASSFAILCOUNTPERPAGE;
		sysData.measure.passFail.line1Datas[absIndex].freq = tempValue;
	} else if (sysData.measure.passFail.line2Editing && sysData.measure.passFail.line2TotalPoints > 0 && sysData.measure.passFail.line2EditNowSelIndex >= 0 && sysData.measure.passFail.line2EditNowSelIndex < sysData.measure.passFail.line2NowPagePoints)
	{
		setFrequencyOffset(0);
		absIndex = sysData.measure.passFail.line2EditNowSelIndex + sysData.measure.passFail.line2NowPageIndex * PASSFAILCOUNTPERPAGE;
		sysData.measure.passFail.line2Datas[absIndex].freq = tempValue;
	}

	return __SCPI_SUCCESS;
}

//设置Pass-Fail迹线频率
int tSysScpi::setMeasureOfPassFailOfLineFreq(QString value)
{

}

//方向键设置Pass-Fail迹线频率
int tSysScpi::setMeasureOfPassFailOfLineFreqWithArrow(bool up)
{

}

//脉冲旋钮设置Pass-Fail迹线频率
int tSysScpi::setMeasureOfPassFailOfLineFreqWithRpg(int value)
{

}

//设置Pass-Fail迹线幅度
int tSysScpi::setMeasureOfPassFailOfLineAmpt(double value)
{
	if (!sysData.measure.passFail.line1Editing && !sysData.measure.passFail.line2Editing)
	{
		return __SCPI_FAILED;
	}

	double tempValue = value;
	int absIndex = 0;

	if (tempValue > MAXREFLEVELDBM)
	{
		tempValue = MAXREFLEVELDBM;
	} else if (tempValue < MINREFLEVELDBM - 10.0 * sysData.ampt.scaleDiv)
	{
		tempValue = MINREFLEVELDBM - 10.0 * sysData.ampt.scaleDiv;
	}

	if (sysData.measure.passFail.line1Editing && sysData.measure.passFail.line1TotalPoints > 0 && sysData.measure.passFail.line1EditNowSelIndex >= 0 && sysData.measure.passFail.line1EditNowSelIndex < sysData.measure.passFail.line1NowPagePoints)
	{
		setAmptOfRefOffset(0);
		absIndex = sysData.measure.passFail.line1EditNowSelIndex + sysData.measure.passFail.line1NowPageIndex * PASSFAILCOUNTPERPAGE;
		sysData.measure.passFail.line1Datas[absIndex].ampt = tempValue;
	} else if (sysData.measure.passFail.line2Editing && sysData.measure.passFail.line2TotalPoints > 0 && sysData.measure.passFail.line2EditNowSelIndex >= 0 && sysData.measure.passFail.line2EditNowSelIndex < sysData.measure.passFail.line2NowPagePoints)
	{
		setAmptOfRefOffset(0);
		absIndex = sysData.measure.passFail.line2EditNowSelIndex + sysData.measure.passFail.line2NowPageIndex * PASSFAILCOUNTPERPAGE;
		sysData.measure.passFail.line2Datas[absIndex].ampt = tempValue;
	}

	return __SCPI_SUCCESS;
}

//设置Pass-Fail迹线幅度
int tSysScpi::setMeasureOfPassFailOfLineAmpt(QString value)
{

}

//方向键设置Pass-Fail迹线幅度
int tSysScpi::setMeasureOfPassFailOfLineAmptWithArrow(bool up)
{

}

//脉冲旋钮设置Pass-Fail迹线幅度
int tSysScpi::setMeasureOfPassFailOfLineAmptWithRpg(int value)
{

}

//Pass-Fail首页
void tSysScpi::gotoFirstPageOfMeasureOfPassFailLineData(void)
{
	int pages = 0;
	int total = 0;
	int pageIndex = 0;
	int pagePoints = 0;
	int selIndex = 0;

	mutexPassFail.lock();

	if (sysData.measure.passFail.line1Editing && sysData.measure.passFail.line1TotalPoints > 0)
	{
		total = sysData.measure.passFail.line1TotalPoints;
		pages = sysData.measure.passFail.line1Pages;
		pageIndex = sysData.measure.passFail.line1NowPageIndex;
		selIndex = sysData.measure.passFail.line1EditNowSelIndex;
		pagePoints = sysData.measure.passFail.line1NowPagePoints;

		pageIndex = 0;
		selIndex = 0;

		if (pageIndex < pages - 1)
		{
			pagePoints = PASSFAILCOUNTPERPAGE;
		} else if (pageIndex == pages - 1)
		{
			pagePoints = total - pageIndex * PASSFAILCOUNTPERPAGE;
		} else
		{
			pagePoints = 0;
		}

		sysData.measure.passFail.line1NowPageIndex = pageIndex;
		sysData.measure.passFail.line1NowPagePoints = pagePoints;
		sysData.measure.passFail.line1EditNowSelIndex = selIndex;
	} else if (sysData.measure.passFail.line2Editing && sysData.measure.passFail.line2TotalPoints > 0)
	{
		total = sysData.measure.passFail.line2TotalPoints;
		pages = sysData.measure.passFail.line2Pages;
		pageIndex = sysData.measure.passFail.line2NowPageIndex;
		selIndex = sysData.measure.passFail.line2EditNowSelIndex;
		pagePoints = sysData.measure.passFail.line2NowPagePoints;

		pageIndex = 0;
		selIndex = 0;

		if (pageIndex < pages - 1)
		{
			pagePoints = PASSFAILCOUNTPERPAGE;
		} else if (pageIndex == pages - 1)
		{
			pagePoints = total - pageIndex * PASSFAILCOUNTPERPAGE;
		} else
		{
			pagePoints = 0;
		}

		sysData.measure.passFail.line2NowPageIndex = pageIndex;
		sysData.measure.passFail.line2NowPagePoints = pagePoints;
		sysData.measure.passFail.line2EditNowSelIndex = selIndex;
	}

	mutexPassFail.unlock();
}

//Pass-Fail上一页
void tSysScpi::gotoPrevPageOfMeasureOfPassFailLineData(void)
{
	int pages = 0;
	int total = 0;
	int pageIndex = 0;
	int pagePoints = 0;
	int selIndex = 0;

	mutexPassFail.lock();

	if (sysData.measure.passFail.line1Editing && sysData.measure.passFail.line1TotalPoints > 0)
	{
		total = sysData.measure.passFail.line1TotalPoints;
		pages = sysData.measure.passFail.line1Pages;
		pageIndex = sysData.measure.passFail.line1NowPageIndex;
		selIndex = sysData.measure.passFail.line1EditNowSelIndex;
		pagePoints = sysData.measure.passFail.line1NowPagePoints;

		if (pageIndex > 0)
		{
			pageIndex--;
			selIndex = 0;
		}

		if (pageIndex < pages - 1)
		{
			pagePoints = PASSFAILCOUNTPERPAGE;
		} else if (pageIndex == pages - 1)
		{
			pagePoints = total - pageIndex * PASSFAILCOUNTPERPAGE;
		} else
		{
			pagePoints = 0;
		}

		sysData.measure.passFail.line1NowPageIndex = pageIndex;
		sysData.measure.passFail.line1NowPagePoints = pagePoints;
		sysData.measure.passFail.line1EditNowSelIndex = selIndex;
	} else if (sysData.measure.passFail.line2Editing && sysData.measure.passFail.line2TotalPoints > 0)
	{
		total = sysData.measure.passFail.line2TotalPoints;
		pages = sysData.measure.passFail.line2Pages;
		pageIndex = sysData.measure.passFail.line2NowPageIndex;
		selIndex = sysData.measure.passFail.line2EditNowSelIndex;
		pagePoints = sysData.measure.passFail.line2NowPagePoints;

		if (pageIndex > 0)
		{
			pageIndex--;
			selIndex = 0;
		}

		if (pageIndex < pages - 1)
		{
			pagePoints = PASSFAILCOUNTPERPAGE;
		} else if (pageIndex == pages - 1)
		{
			pagePoints = total - pageIndex * PASSFAILCOUNTPERPAGE;
		} else
		{
			pagePoints = 0;
		}

		sysData.measure.passFail.line2NowPageIndex = pageIndex;
		sysData.measure.passFail.line2NowPagePoints = pagePoints;
		sysData.measure.passFail.line2EditNowSelIndex = selIndex;
	}

	mutexPassFail.unlock();
}

//Pass-Fail下一页
void tSysScpi::gotoNextPageOfMeasureOfPassFailLineData(void)
{
	int pages = 0;
	int total = 0;
	int pageIndex = 0;
	int pagePoints = 0;
	int selIndex = 0;

	mutexPassFail.lock();

	if (sysData.measure.passFail.line1Editing && sysData.measure.passFail.line1TotalPoints > 0)
	{
		total = sysData.measure.passFail.line1TotalPoints;
		pages = sysData.measure.passFail.line1Pages;
		pageIndex = sysData.measure.passFail.line1NowPageIndex;
		selIndex = sysData.measure.passFail.line1EditNowSelIndex;
		pagePoints = sysData.measure.passFail.line1NowPagePoints;

		if (pageIndex < pages - 1)
		{
			pageIndex++;
			selIndex = 0;
		}

		if (pageIndex < pages - 1)
		{
			pagePoints = PASSFAILCOUNTPERPAGE;
		} else if (pageIndex == pages - 1)
		{
			pagePoints = total - pageIndex * PASSFAILCOUNTPERPAGE;
		} else
		{
			pagePoints = 0;
		}

		sysData.measure.passFail.line1NowPageIndex = pageIndex;
		sysData.measure.passFail.line1NowPagePoints = pagePoints;
		sysData.measure.passFail.line1EditNowSelIndex = selIndex;
	} else if (sysData.measure.passFail.line2Editing && sysData.measure.passFail.line2TotalPoints > 0)
	{
		total = sysData.measure.passFail.line2TotalPoints;
		pages = sysData.measure.passFail.line2Pages;
		pageIndex = sysData.measure.passFail.line2NowPageIndex;
		selIndex = sysData.measure.passFail.line2EditNowSelIndex;
		pagePoints = sysData.measure.passFail.line2NowPagePoints;

		if (pageIndex < pages - 1)
		{
			pageIndex++;
			selIndex = 0;
		}

		if (pageIndex < pages - 1)
		{
			pagePoints = PASSFAILCOUNTPERPAGE;
		} else if (pageIndex == pages - 1)
		{
			pagePoints = total - pageIndex * PASSFAILCOUNTPERPAGE;
		} else
		{
			pagePoints = 0;
		}

		sysData.measure.passFail.line2NowPageIndex = pageIndex;
		sysData.measure.passFail.line2NowPagePoints = pagePoints;
		sysData.measure.passFail.line2EditNowSelIndex = selIndex;
	}

	mutexPassFail.unlock();
}

//Pass-Fail尾页
void tSysScpi::gotoLastPageOfMeasureOfPassFailLineData(void)
{
	int pages = 0;
	int total = 0;
	int pageIndex = 0;
	int pagePoints = 0;
	int selIndex = 0;

	mutexPassFail.lock();

	if (sysData.measure.passFail.line1Editing && sysData.measure.passFail.line1TotalPoints > 0)
	{
		total = sysData.measure.passFail.line1TotalPoints;
		pages = sysData.measure.passFail.line1Pages;
		pageIndex = sysData.measure.passFail.line1NowPageIndex;
		selIndex = sysData.measure.passFail.line1EditNowSelIndex;
		pagePoints = sysData.measure.passFail.line1NowPagePoints;

		pageIndex = pages - 1;
		selIndex = 0;

		if (pageIndex < pages - 1)
		{
			pagePoints = PASSFAILCOUNTPERPAGE;
		} else if (pageIndex == pages - 1)
		{
			pagePoints = total - pageIndex * PASSFAILCOUNTPERPAGE;
		} else
		{
			pagePoints = 0;
		}

		sysData.measure.passFail.line1NowPageIndex = pageIndex;
		sysData.measure.passFail.line1NowPagePoints = pagePoints;
		sysData.measure.passFail.line1EditNowSelIndex = selIndex;
	} else if (sysData.measure.passFail.line2Editing && sysData.measure.passFail.line2TotalPoints > 0)
	{
		total = sysData.measure.passFail.line2TotalPoints;
		pages = sysData.measure.passFail.line2Pages;
		pageIndex = sysData.measure.passFail.line2NowPageIndex;
		selIndex = sysData.measure.passFail.line2EditNowSelIndex;
		pagePoints = sysData.measure.passFail.line2NowPagePoints;

		pageIndex = pages - 1;
		selIndex = 0;

		if (pageIndex < pages - 1)
		{
			pagePoints = PASSFAILCOUNTPERPAGE;
		} else if (pageIndex == pages - 1)
		{
			pagePoints = total - pageIndex * PASSFAILCOUNTPERPAGE;
		} else
		{
			pagePoints = 0;
		}

		sysData.measure.passFail.line2NowPageIndex = pageIndex;
		sysData.measure.passFail.line2NowPagePoints = pagePoints;
		sysData.measure.passFail.line2EditNowSelIndex = selIndex;
	}

	mutexPassFail.unlock();
}

//指定行
void tSysScpi::gotoRowOfMeasureOfPassFailLineData(int value)
{
	int absIndex = 0;
	int selIndex = 0;
	int pageIndex = 0;
	int total = 0;
	int pages = 0;
	int pagePoints = 0;

	mutexPassFail.lock();

	if (sysData.measure.passFail.line1Editing && sysData.measure.passFail.line1TotalPoints > 0)
	{
		selIndex = sysData.measure.passFail.line1EditNowSelIndex;
		pageIndex = sysData.measure.passFail.line1NowPageIndex;
		total = sysData.measure.passFail.line1TotalPoints;
		pages = sysData.measure.passFail.line1Pages;
		absIndex = selIndex + pageIndex * PASSFAILCOUNTPERPAGE;

		if (absIndex >= 0 && absIndex < total)
		{
			absIndex -= value;

			if (absIndex < 0)
			{
				absIndex = 0;
			} else if (absIndex >= total)
			{
				absIndex = total - 1;
			}

			selIndex = absIndex % PASSFAILCOUNTPERPAGE;
			pageIndex = absIndex / PASSFAILCOUNTPERPAGE;

			if (pageIndex < pages - 1)
			{
				pagePoints = PASSFAILCOUNTPERPAGE;
			} else if (pageIndex == pages - 1)
			{
				pagePoints = total - pageIndex * PASSFAILCOUNTPERPAGE;
			} else
			{
				pagePoints = 0;
			}

			if (selIndex >= 0 && selIndex < total && pageIndex >= 0 && pageIndex < pages)
			{
				sysData.measure.passFail.line1NowPageIndex = pageIndex;
				sysData.measure.passFail.line1NowPagePoints = pagePoints;
				sysData.measure.passFail.line1EditNowSelIndex = selIndex;
			}
		}
	} else if (sysData.measure.passFail.line2Editing && sysData.measure.passFail.line2TotalPoints > 0)
	{
		selIndex = sysData.measure.passFail.line2EditNowSelIndex;
		pageIndex = sysData.measure.passFail.line2NowPageIndex;
		total = sysData.measure.passFail.line2TotalPoints;
		pages = sysData.measure.passFail.line2Pages;
		absIndex = selIndex + pageIndex * PASSFAILCOUNTPERPAGE;

		if (absIndex >= 0 && absIndex < total)
		{
			absIndex -= value;

			if (absIndex < 0)
			{
				absIndex = 0;
			} else if (absIndex >= total)
			{
				absIndex = total - 1;
			}

			selIndex = absIndex % PASSFAILCOUNTPERPAGE;
			pageIndex = absIndex / PASSFAILCOUNTPERPAGE;

			if (pageIndex < pages - 1)
			{
				pagePoints = PASSFAILCOUNTPERPAGE;
			} else if (pageIndex == pages - 1)
			{
				pagePoints = total - pageIndex * PASSFAILCOUNTPERPAGE;
			} else
			{
				pagePoints = 0;
			}

			if (selIndex >= 0 && selIndex < total && pageIndex >= 0 && pageIndex < pages)
			{
				sysData.measure.passFail.line2NowPageIndex = pageIndex;
				sysData.measure.passFail.line2NowPagePoints = pagePoints;
				sysData.measure.passFail.line2EditNowSelIndex = selIndex;
			}
		}
	}

	mutexPassFail.unlock();
}

qint64 tSysScpi::getFreeDiskSpace(void)
{
	struct statfs fs;

	if (statfs("/", &fs) == 0)
	{
		return fs.f_bfree * fs.f_bsize;
	} else
	{
		return 0;
	}
}

//执行自检
void tSysScpi::execSelfCheck(void)
{
	struct selfCheckDef sc;

	sc.control = false;
	sc.aif = false;
	sc.arf = false;
	sc.power = true;

	if (SELFCHECK == 1)
	{
		presetSystemData();
		controlRf();
		usleep(100 * 1000);
		getDataFromIF();

		if (ifHandle >= 0)
		{
			sc.control = ioctl(ifHandle, 0xec, 0x00) == 0x5555;
		}

		for (int i = 0; i < 10; i++)
		{
			if (!sc.aif && i >= 6 && sysData.prjValue[i] >= -70 && sysData.prjValue[i] <= -50)
			{
				sc.aif = true;
			}

			if (!sc.arf && sysData.prjValue[i] >= 0)
			{
				sc.arf = true;
			}
		}

		selfCheck = sc;
	}
}

//获取自检参数文本
QString tSysScpi::getSelfCheckText(bool value)
{

}

//载入对数预定义数据
bool tSysScpi::loadLogarithmDefineData(void)
{
	QString fileName = QCoreApplication::applicationDirPath() + "/logarithm.def";
	memset(log10Table, 0, sizeof log10Table);

	if (QFile(fileName).exists())
	{
		FILE *fp;
		fp = fopen(fileName.toStdString().c_str(), "rb");
		if (fp != NULL)
		{
			fread(&log10Table, sizeof log10Table, 1, fp);
			fclose(fp);

			return true;
		}
	}

	return false;
}

void tSysScpi::loadDeviceType(void)
{
	devType = DEVICETYPE;

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

//退出校准
int tSysScpi::exitCalibrate(void)
{
	if (sysData.isFactoryCalibrating)
	{
		sysData.isFactoryCalibrating = false;
	}
	if (sysData.isUserCalibrating)
	{
		sysData.isUserCalibrating = false;
	}
	if (sysData.isPreamplifierCalibrating)
	{
		sysData.isPreamplifierCalibrating = false;
	}
	if (sysData.isZcCalibrating)
	{
		sysData.isZcCalibrating = false;
	}
	if (sysData.isZcPreamplifierCalibrating)
	{
		sysData.isZcPreamplifierCalibrating = false;
	}
	sysData.scpiData.stateT = false;
	sysData.scpiData.stateS = false;

	sysData.mode = smSpectrum;
	sysData.canvasNums = 1;


	return __SCPI_FAILED;
}

void tSysScpi::getSystemYSValue(void)
{
	sysErrValue = 0;
	/*
	 if (sysData.span.span > 0 &&
	 sysData.freq.stop <= 15e6 &&
	 sysData.ampt.ifValue > 10 &&
	 sysData.bw.rbw > 1e3 &&
	 !sysData.isCalibrating &&
	 !sysData.isFactoryCalibrating &&
	 !sysData.isUserCalibrating&&
	 !sysData.isPreamplifierCalibrating)
	 {
	 sysErrValue = sysData.ampt.ifValue - 10;
	 }*/
}

//获取增益补偿
void tSysScpi::getIfSoftValue(void)
{
	int ifZs = (int) sysData.ampt.ifValue / 10;
	int ifXs = (int) sysData.ampt.ifValue % 10;
	double temp = 0;

	/*增益软件补偿：
	 *1、 当中频增益小于等于30时，ifZs由中频放大器实现，ifXs由软件补偿实现
	 *2、当中频增益大于30时，30dB由中频放大器实现，剩下的由软件补偿实现
	 */

	if (sysData.ampt.ifValue <= MAXIF)
	{
		//四舍五入处理
		if (ifXs >= 5)
		{
			ifZs++;
			ifXs = 0;
		}

		temp = sysData.ampt.ifValue - ifZs * 10.0;
	} else
	{
		temp = sysData.ampt.ifValue - MAXIF;
	}

	sysData.ampt.ifSoftValue = temp;
}

//设置两级衰减
void tSysScpi::setAtt1AndAtt2(void)
{
	//  if (sysData.ampt.att < 20)
	//  {
	//	sysData.ampt.att1 = 0;
	//  } else if (sysData.ampt.att >= 20)
	//  {
	//    sysData.ampt.att1 = 20;
	//  }
	sysData.ampt.att1 = 0;
	sysData.ampt.att2 = sysData.ampt.att - sysData.ampt.att1;
}

//零扫宽处理
void tSysScpi::zeroSpanHandle(void)
{
	if ((sysData.span.prvSpan > 0) && (sysData.span.nowSpan <= 0))
	{
//		if (sysData.bw.rbwAuto)
//		{
//			sysData.bw.rbwAuto = false;
//		}
//		if (sysData.options.isFftOn && (sysData.bw.rbw <= MAXFFTRBW))
//		{
//			sysData.bw.rbw = 1e3;
//		}
//		sysData.sweep.sweepTimeAuto = false;
		//    if (sysData.sweep.sweepTimeAuto)
		//    {
		//      sysData.sweep.sweepTimeAuto = false;
		//    }
	} else if ((sysData.span.prvSpan <= 0) && (sysData.span.nowSpan > 0))
	{
//		if (!sysData.bw.rbwAuto)
//		{
//			sysData.bw.rbwAuto = true;
//		}

//		if (!sysData.sweep.sweepTimeAuto)
//		{
//			sysData.sweep.sweepTimeAuto = true;
//		}
	}

	if(sysData.span.nowSpan == 0)
	{
		sysData.span.isZeroSpan = true;
	} else
	{
		sysData.span.isZeroSpan = false;
	}
	ioctl(ramHandle, 0xff, sysData.span.isZeroSpan);
	if(sysData.span.isZeroSpan)
		ioctl(ramHandle, 0xfe, true);
	else
		ioctl(ramHandle, 0xfe, false);
}

//数据放大(1, step, 5, 10步进)
double tSysScpi::zoomInData(double value, int step)
{
	int bits10 = 0;
	double data = value;
	while (true)
	{
		if ((data / 10) >= 1)
		{
			bits10++;
			data /= 10;
		} else
		{
			break;
		}
	}

	if ((data >= 1) && (data < step))
	{
		data = step;
	} else if ((data >= step) && (data < 5))
	{
		data = 5;
	} else if ((data >= 5) && (data < 10))
	{
		data = 10;
	} else
	{
		data = 1;
	}
	return (int) (data) * (pow(10, bits10));
}

//数据缩小(1, step, 5, 10步进)
double tSysScpi::zoomOutData(double value, int step)
{
	int bits10 = 0;
	double data = value;

	while (true)
	{
		if ((data / 10) >= 1)
		{
			bits10++;
			data /= 10;
		} else
		{
			break;
		}
	}

	if ((data <= 10) && (data > 5))
	{
		data = 5;
	} else if ((data <= 5) && (data > step))
	{
		data = step;
	} else if ((data <= step) && (data > 1))
	{
		data = 1;
	} else if (data == 1)
	{
		if (bits10 > 0)
		{
			data = 5;
			bits10--;
		} else
		{
			data = 0;
		}
	} else
	{
		data = 0;
	}

	return (int) (data) * (pow(10, bits10));
}

//mV转化为dBm
double tSysScpi::getRefLevelmVTodBm(double value)
{
	return 10 * log10(0.001 * qPow(value, 2) / 50.0);
}

//获取当前幅度值
double tSysScpi::getRefLevelValue(double value)
{
	double tempValue = value;

	switch (sysData.ampt.refUnit)
	{
		case rudBm:
			return tempValue;
		case rudBmV:
			return tempValue + 47.0;
		case rudBuV:
			return tempValue + 107.0;
		case ruW:
			return qPow(10, 0.1 * tempValue);
		case ruV:
			return qSqrt(50000 * qPow(10, 0.1 * tempValue));
		case rudBpW:
			return tempValue + 90.0;
		case rudBuW:
			return tempValue + 30.0;
		case ruuV:
			return 1e6 * qSqrt(50000 * qPow(10, 0.1 * tempValue));
			;
		case rudBuVm:
		default:
			return tempValue;
	}
}

//获取幅度dBm值
double tSysScpi::getRefLeveldBmValue(void)
{
	double tempValue = sysData.ampt.refLevel;

	switch (sysData.ampt.refUnit)
	{
		case rudBm:
			return tempValue - sysData.ampt.refOffset;
		case rudBpW:
			return tempValue - 90.0 - sysData.ampt.refOffset;
		case rudBuW:
			return tempValue - 30.0 - sysData.ampt.refOffset;
		case rudBmV:
			return tempValue - 47.0 - sysData.ampt.refOffset;
		case rudBuV:
			return tempValue - 107.0 - sysData.ampt.refOffset;
		case ruW:
			return 10 * log10(tempValue) - sysData.ampt.refOffset;
		case ruV:
			return 10 * log10(0.001 * qPow(tempValue, 2) / 50.0) - sysData.ampt.refOffset;
		case ruuV:
			return 10 * log10(0.001 * qPow(tempValue * 1e-6, 2) / 50.0) - sysData.ampt.refOffset;
		case rudBuVm:
		default:
			return tempValue - sysData.ampt.refOffset;
	}
}

//获取dBm值
double tSysScpi::getdBmValue(double value)
{
	double tempValue = value;

	switch (sysData.ampt.refUnit)
	{
		case rudBm:
			return tempValue - sysData.ampt.refOffset;
		case rudBpW:
			return tempValue - 90.0 - sysData.ampt.refOffset;
		case rudBuW:
			return tempValue - 30.0 - sysData.ampt.refOffset;
		case rudBmV:
			return tempValue - 47.0 - sysData.ampt.refOffset;
		case rudBuV:
			return tempValue - 107.0 - sysData.ampt.refOffset;
		case ruW:
			return 10 * log10(tempValue) - sysData.ampt.refOffset;
		case ruV:
			return 10 * log10(0.001 * qPow(tempValue, 2) / 50.0) - sysData.ampt.refOffset;
		case ruuV:
			return 10 * log10(0.001 * qPow(tempValue * 1e-6, 2) / 50.0) - sysData.ampt.refOffset;
		case rudBuVm:
		default:
			return tempValue - sysData.ampt.refOffset;
	}
}

//获取分辨带宽索引
int tSysScpi::getRbwIndex(void)
{
	int result = 0;

	if (sysData.freq.isLowChannel)
	{
		for (int i = 0; i < sizeof RAMCIC_ZC_HZ / sizeof RAMCIC_ZC_HZ[0]; i++)
		{
			if ((unsigned long) sysData.bw.rbw == RAMCIC_ZC_HZ[i])
			{
				result = i;
				break;
			}
		}
		return result;
	}

	for (int i = 0; i < sizeof RAMCIC_HZ / sizeof RAMCIC_HZ[0]; i++)
	{
		if ((unsigned long) sysData.bw.rbw == RAMCIC_HZ[i])
		{
			result = i;
			break;
		}
	}

	return result;
}

//获取视频带宽索引
int tSysScpi::getVbwIndex(void)
{
	int result = 0;

	for (int i = 0; i < sizeof RAMCVC / sizeof RAMCVC[0]; i++)
	{
		if ((unsigned long) sysData.bw.vbw == RAMCIC_VBWHZ[i])
		{
			result = i;
			break;
		}
	}

	return result;
}

//获取视频带宽修正索引
int tSysScpi::getVbwModIndex(void)
{
	int rbwIndex = getRbwIndex();
	int vbwIndex = getVbwIndex();
	int errIndex = vbwIndex - rbwIndex;
	int result = errIndex;

	if (errIndex < 0)
	{
		result = 0;
	} else if (errIndex > (sizeof RAMCVC / sizeof RAMCVC[0] - 1))
	{
		result = sizeof RAMCVC / sizeof RAMCVC[0] - 1;
	}

	return result;
}

//FFT
void tSysScpi::firFilter(double* x, double* h, double* y, int nx, int nh, int ny, int err)
{
	if (err >= 1 && err <= VBWORDER)
	{
		double sum = 0;

		for (int i = 0; i < FIRORDER; i++)
		{
			FIRWIN[i] = FIRVALUE[err - 1][FIRORDER - 1 - i];
		}

		for (int j = 0; j < ny; j++)
		{
			sum = 0;

			for (int i = 0; i < nh; i++)
			{
				if ((i + j) <= (nx - 1))
				{
					sum += x[i + j] * FIRWIN[i];
				} else
				{
					sum += x[nx - 1] * FIRWIN[i];
				}
			}

			y[j] = sum;
		}
	}
}

//FFT测量条件判断检测
bool tSysScpi::checkFFTMeasure(void)
{
	bool isReqFftMeas = isRequireFFTMeas();

	if (not isReqFftMeas)
	{
		return false;
	}

	//memset(&sysData.fft, 0, sizeof sysData.fft);
	sysData.fft.isFftOut = true;
	sysData.fft.fftOn = false;
	sysData.fft.fftIndex = 0;
	sysData.fft.fftDataIndex = 0;
	sysData.fft.fftCount = 0;
	sysData.fft.fftSetted = false;
	sysData.fft.fftGetted = false;
	sysData.fft.prvCenterFreq = 0;
	sysData.fft.nowCenterFreq = 0;
	sysData.fft.isSweepSingle = false;
	sysData.fft.sweepCount = 0;
	sysData.fft.fftSumCount = 0;
	memset(sysData.fft.fftSigleSumPoint, 0, sizeof(sysData.fft.fftSigleSumPoint));

	int rIndex = getRbwIndex();
	//  double fftStartFreq = sysData.freq.start;
	double fftSpan = sysData.span.span;
	double fftRbw = sysData.bw.rbw;
	double fftCS = fftSpan * 1.0 / (fftRbw * 500);
	double fftSSpan = 0;

	sysData.fft.fftCount = (unsigned int) fftCS;

	if ((fftCS - (unsigned int) fftCS) > 0)
	{
		sysData.fft.fftCount++;
	}

	if (sysData.fft.fftCount <= 0)
	{
		return false;
	}

	fftSSpan = fftSpan / sysData.fft.fftCount;

	//FFT总共点数
	sysData.fft.fftSumCount = sysData.fft.fftCount * FFTDOTS;

	double tempSize = sysData.fft.fftSumCount * 1.0 / sysData.sweep.sweepPoints;

	//计算每个BUC包含FFT的点数
	for (int i = 0; i < sysData.sweep.sweepPoints; i++)
	{
		if (tempSize - (int) tempSize > 0) //不整除
		{
			if (i % 2 == 1) //奇数加1
			{
				sysData.fft.fftSigleSumPoint[i] = (int) (tempSize * (i + 1)) + 1;
			} else //偶数减1
			{
				sysData.fft.fftSigleSumPoint[i] = (int) (tempSize * (i + 1));
			}
		} else //整除
		{
			sysData.fft.fftSigleSumPoint[i] = (int) (tempSize * (i + 1));
		}

		if (i == sysData.sweep.sweepPoints - 1)
		{
			sysData.fft.fftSigleSumPoint[i] = sysData.fft.fftSumCount - 1;
		}

		sysData.fft.bands[i].start_Band = sysData.fft.bands[i].stop_Band = -1;
	}

	sysData.fft.cF1 = -0.5 * (fftSSpan) / (IFFS * 1.0 / RAMCIC[getRbwIndex()]);
	sysData.fft.cF2 = qAbs(sysData.fft.cF1);

	sysData.fft.fftIndex = 0;
	sysData.fft.fftDataIndex = 0;
	sysData.fft.fftIndex = 0;
	sysData.fft.fftSetted = false;
	sysData.fft.fftGetted = false;
	sysData.fft.fftOn = true;

	for (int i = 0; i < TRACECOUNT; i++)
	{
		sysData.trace[i].flags = 0;
	}

	return true;
}

//是否为直采模式
bool tSysScpi::isDirectGatherMode(void)
{
	return (sysData.options.directChannelOptionOn &&
					sysData.freq.isLowChannel &&
					sysData.freq.start >= (ZCMINFREQ + sysData.freq.offset) &&
					sysData.freq.stop <= (ZCMAXFREQ + sysData.freq.offset));
}

//是否需要进行FFT测量控制
bool tSysScpi::isRequireFFTMeas(void)
{
	return ((sysData.options.isFftOn) &&
						(!sysData.freq.isLowChannel) &&
				//		(fftHandle >= 0) &&
						(iqHandle >= 0) &&
						(sysData.span.span > 0) &&
						(sysData.bw.rbw <= MAXFFTRBW) &&
						(!sysData.isUserCalibrating) &&
						(!sysData.isFactoryCalibrating) &&
						(!sysData.isZcCalibrating) &&
						(!sysData.isZcPreamplifierCalibrating) &&
						(!sysData.isPreamplifierCalibrating)) || sysData.isFftCalibrating;

}

//获取直采通道中频增益下发值
unsigned char tSysScpi::getZcIfdown(double value)
{
	unsigned char preamptOn = 0;
	unsigned char ifDown = 0x00;
	unsigned char trigDown = 0;
	int ifZs = 0;
	int ifXs = 0;

	ifZs = (int) value / 10;
	ifXs = (int) value % 10;

	//四舍五入处理
	if (value <= MAXIF)
	{
		if (ifXs >= 5)
		{
			ifZs++;
			ifXs = 0;
		}
	}
	getIfSoftValue();

	switch (ifZs)
	{
		case 0: 	ifDown = 0x00;	break;
		case 1:		ifDown = 0x01;	break;
		case 2:		ifDown = 0x02;	break;
		case 3:		ifDown = 0x03;	break;
		default:	ifDown = 0x03;	break;
	}

	if (sysData.ampt.isPreamptOn)
	{
		preamptOn = 1;
	}

	if (sysData.trig.mode == tgRise || sysData.trig.mode == tgFall)
	{
		trigDown = 1;
	}
	return (ifDown | (preamptOn << 2) | (1 << 3) | (trigDown << 4));
}

//直采控制
bool tSysScpi::directGatherControl(void)
{
	unsigned char ifDown = 0x00;

	if (rfHandle >= 0)
	{
		if (isDirectGatherMode())
		{
			ifDown = getZcIfdown(sysData.ampt.ifValue);

			rfDownload(0xf9, ifDown);
			//地址：0x012<<2，说明：波段及衰减控制，其中衰减控制与变频通道复用
			rfDownload(0xfd, (15 - (int) (sysData.ampt.att / 3)) | (1 << 4) | (0 << 6));
			//			  sysData.ampt.refLevel,
			//			  sysData.ampt.ifValue,
			//			  sysData.ampt.att,
			//			  ifDown,
			//			  ifDown | (preamptOn << 2) | (1 << 3),
			//			  (15 -(int)(sysData.ampt.att / 3)) | (0 << 4 ) | (0 << 6));

			//DDC控制，控制NCO中心频率, copy from ouyang
			//ioctl(rfHandle, 0xf9, 0x08);  // 0x012<<2
			//ioctl(rfHandle, 0xfd, 0x2c);  // channel + att

			//ddcControl(5e6 * 1.0);

			/*
			 //DDC
			 ioctl(rfHandle, 0xef, 0x2aaa);  //phase high
			 ioctl(rfHandle, 0xee, 0xaaaa);  //phase low
			 ioctl(rfHandle, 0x7c, 0x00);    //phase reset
			 ioctl(rfHandle, 0x10e, 0x00);   //cic reset
			 */

			reCalculateZcParams();
		} else
		{
			rfDownload(0xf9, 0x00);
			rfDownload(0xfd, 0x6c);
			//ioctl(rfHandle, 0xf9, 0x00);  // 0x012<<2
			//ioctl(rfHandle, 0xfd, 0x6c);  // channel + att
			ddcControl(8.6e6 * 1.0);

			/*
			 //DDC, 8.6MHz
			 ioctl(rfHandle, 0xef, 0x4962);  //phase high
			 ioctl(rfHandle, 0xee, 0xfc96);  //phase low
			 ioctl(rfHandle, 0x7c, 0x00);    //phase reset
			 ioctl(rfHandle, 0x10e, 0x00);   //cic reset
			 */
		}
	}
}

//解调中频控制
bool tSysScpi::demodControlIF(void)
{
	int SBUC = 5;
	int SBUC_D = 0;

	//sweep points
	ifDownload(0xfb, DEMODSIZE);

	//sbuc
	SBUC_D = 65536 - SBUC;

	ifDownload(0x1a, ((SBUC_D >> 8) & 0xff));
	ifDownload(0x1a, (SBUC_D & 0xff));

	//rbw
	int rbwIndex = 6;

	if (sysData.emc.emcOn && (unsigned long) sysData.bw.rbw == 120e3)
	{
		rbwIndex = 6;
	}

	//  //vbw
	//  if (sysData.bw.vbwAuto)
	//  {
	//    ifDownload(0x08, RAMCVC[0]);
	//  } else
	//  {
	//    ifDownload(0x08, RAMCVC[getVbwModIndex()]);
	//  }

	//vbw 提速
	if (sysData.bw.vbw > 1e3)
	{
		ifDownload(0x1b, 0x80);
	} else
	{
		ifDownload(0x1b, 0x40);
	}

	ifDownload(0x08, RAMCVC[getVbwIndex()]);

	ifDownload(0x15, 0xff);
	ifDownload(0x15, 0x00);

	long int crcclk;
	int shift = (int) (5.0 * log10(double(sysData.options.cicValue)) / log10(2));

	int emcIndex = 0;

	if (sysData.emc.emcOn)
	{
		for (int i = 0; i < sizeof EMC_BW / sizeof EMC_BW[0]; i++)
		{
			if ((unsigned long) sysData.bw.rbw == EMC_BW[i])
			{
				shift = (int) (5.0 * log10(double(EMC_CIC[i])) / log10(2));
				emcIndex = i;
				break;
			}
		}
	}

	if (shift >= 74) //old is 63
	{
		shift = 74;
	}

	ifDownload(0x06, shift);

	// CIC CLK
	crcclk = (~sysData.options.cicValue) + 1;
	if (sysData.emc.emcOn)
	{
		crcclk = (~EMC_CIC[emcIndex]) + 1;
	}
	if (isRequireFFTMeas())
	{
		crcclk = (~RAMCIC[rbwIndex]) + 1;
	}

	ifDownload(0x03, (crcclk & 0xff));
	ifDownload(0x04, (crcclk >> 8) & 0xff);
	ifDownload(0x05, (crcclk >> 16) & 0xff);

	//CIC RST & LOAD
	unsigned char a7 = 0;
	//a7 = 0x01 & 0xf2;
	ifDownload(0x07, 0x01);
	//a7 = 0x12 & 0xf2;
	ifDownload(0x07, 0x12);

	if (sysData.bw.rbw >= 2e3)
	{
		rfDownload(0x64, 0);
	} else
	{
		rfDownload(0x64, 1);
	}

	if (rbwIndex <= 1) //RBW为3MHz和1MHz；
	{
		a7 = 0x00 & 0xf2;
		ifDownload(0x07, a7);
	} else if (rbwIndex <= 14) //RBW为11kHz~500kHz；
	{
		a7 = 0x10 & 0xf2;
		ifDownload(0x07, a7);
	} else if (rbwIndex <= 25) //RBW为900Hz~10kHz
	{
		a7 = 0x20 & 0xf2;
		ifDownload(0x07, a7);
	} else
	{
		a7 = 0x30 & 0xf2;
		ifDownload(0x07, a7);//RBW为10Hz~800Hz
	}

	//sample id
	controlIfDetector();

	//中频增益
	int ifDown = 0x0;
	int ifZs = 0;
	int ifXs = 0;

	ifZs = (int) sysData.ampt.ifValue / 10;
	ifXs = (int) sysData.ampt.ifValue % 10;

	//四舍五入处理
	if (sysData.ampt.ifValue <= MAXIF)
	{
		if (ifXs >= 5)
		{
			ifZs++;
			ifXs = 0;
		}
	}

	getIfSoftValue();

	switch (ifZs)
	{
		case 0:
			ifDown = 0x08;
			break;
		case 1:
			ifDown = 0x00;
			break;
		case 2:
			ifDown = 0x18;
			break;
		case 3:
			ifDown = 0x10;
			break;
		default:
			ifDown = 0x10;
			break;
	}

	//Buc Delay
	ifDownload(0xfc, 0);

	//freq count
	if (sysData.markerFctn.isFreqCountOn)
	{
		unsigned long freqBwCount = 16777216 - 1e7 / sysData.markerFctn.freqCountBw;

		ifDownload(0x1a, (freqBwCount & 0xff0000) >> 16);
		ifDownload(0x1a, (freqBwCount & 0xff00) >> 8);
		ifDownload(0x1a, (freqBwCount & 0xff) >> 0);
		ifDownload(0x1c, 0x01);
	} else
	{
		ifDownload(0x1c, 0x00);
	}

	//零扫宽控制、中频增益 & 音频解调
	bool isZeroSpan;
	if (sysData.span.span == 0)
	{
		isZeroSpan = true;
	} else
	{
		isZeroSpan = false;
	}

	rfDownload(0x54, (isZeroSpan << 3) | (sysData.demod.demodOn << 2) | (ifDown >> 3));
}

//解调控制
bool tSysScpi::demodControl(void)
{
	//open demod and set am or fm on
	if ((!sysData.options.isDemodOn) || (!sysData.options.amOn && !sysData.options.fmOn))
	{
		return false;
	}

	sysData.options.cicValue = (unsigned int) (IFFS / sysData.options.am_ifbw);

	if (sysData.options.fmOn)
	{
		sysData.options.cicValue = (unsigned int) (IFFS / sysData.options.fm_ifbw);
	}

	sysData.options.am_time_us = 1.0 * (DEMODSIZE + 4) * 1e6 / (sysData.options.am_ifbw);

	if (sysData.options.fmOn)
	{
		sysData.options.fm_time_us = 1.0 * (DEMODSIZE + 4) * 1e6 / (sysData.options.fm_ifbw);
	}

	sysData.sweep.sweepTimeAuto = false;//sweepTime by cal not couple
	sysData.sweep.sweepSingle = true;
	sysData.sweep.sweepTime = DEMODSIZE * 1e9 / (30e6 / sysData.options.cicValue);//unit ns
	sysData.span.span = 0;
	sysData.freq.start = sysData.freq.stop = sysData.options.am_carrierfreq;

	if (sysData.options.fmOn)
	{
		sysData.freq.start = sysData.freq.stop = sysData.options.fm_carrierfreq;
	}

	//Buc延时
	int BucLen = 0;
	double tempValue = 0;
	double tempValue1 = 0;
	double tempValue2 = 0;
	double tempAt1 = 0;
	double tempAt2 = 0;

	unsigned int trackatt_temp = 0;
	unsigned char trackatt_tempN1 = 0;
	unsigned char trackatt_tempN2 = 0;
	unsigned char trackatt_tempN3 = 0;
	unsigned char trackatt_tempN4 = 0;
	unsigned char trackatt_tempN5 = 0;

	double spanValue = 0;

	//MHz
	rfData.Ft1 = 1500;
	rfData.Ft2 = 2300;
	rfData.Ft3 = 3100;
	rfData.Ft4 = 3601;
	rfData.BucLen = DEMODSIZE;

	//demod开关
	spanValue = 0;

	//值计算
	rfData.Db = 40;
	double tempDb1 = 0;
	double tempDb2 = 0;
	double tempDb = 0;
	tempDb1 = 0.2 * spanValue / sysData.bw.rbw;
	tempDb2 = sysData.sweep.sweepTime / (1.0 * 1e9) * 3;
	rfData.Db = (tempDb1 < tempDb2) ? tempDb1 : tempDb2; //取二者最小值

	if (tempDb < 3.0)
	{
		rfData.Db = 3;
	} else if (tempDb > 60000.0)
	{
		rfData.Db = 60000;
	} else
	{
		rfData.Db = (int) tempDb;
	}

	rfData.Delta_f = (double) (spanValue / 1e6 / rfData.BucLen); //MHz
	rfData.Delta_n = (double) (rfData.Delta_f / (rfData.Db * 80.0));//
	rfData.Dnf_f = (unsigned long long) (rfData.Delta_n * 4294967295.0);

	rfData.Dnf_f1 = (rfData.Dnf_f & 0xff);
	rfData.Dnf_f2 = (rfData.Dnf_f >> 8) & 0xff;
	rfData.Dnf_f3 = (rfData.Dnf_f >> 16) & 0xff;
	rfData.Dnf_f4 = (rfData.Dnf_f >> 24) & 0xff;
	rfData.Dnf_f5 = 0x00;

	rfData.FLo1 = (double) ((sysData.options.am_carrierfreq - spanValue / 2) / 1e6);
	rfData.FLo2 = (double) ((sysData.options.am_carrierfreq + spanValue / 2) / 1e6);

	if (sysData.options.fmOn)
	{
		rfData.FLo1 = (double) ((sysData.options.fm_carrierfreq - spanValue / 2) / 1e6);
		rfData.FLo2 = (double) ((sysData.options.fm_carrierfreq + spanValue / 2) / 1e6);
	}

	double startErr1 = 0;
	double stopErr1 = 0;
	double startErr2 = 0;
	double stopErr2 = 0;

	if (rfData.FLo1 <= rfData.Ft1)
	{
		rfData.Start_Band = 0;
		startErr1 = qAbs(rfData.FLo1);
		startErr2 = qAbs(rfData.Ft1 * 1.0 - rfData.FLo1);
	} else if (rfData.FLo1 <= rfData.Ft2)
	{
		rfData.Start_Band = 1;
		startErr1 = qAbs(rfData.FLo1 - rfData.Ft1 * 1.0);
		startErr2 = qAbs(rfData.Ft2 * 1.0 - rfData.FLo1);
	} else if (rfData.FLo1 <= rfData.Ft3)
	{
		rfData.Start_Band = 2;
		startErr1 = qAbs(rfData.FLo1 - rfData.Ft2 * 1.0);
		startErr2 = qAbs(rfData.Ft3 * 1.0 - rfData.FLo1);
	} else
	{
		rfData.Start_Band = 3;
		startErr1 = qAbs(rfData.FLo1 - rfData.Ft3 * 1.0);
		startErr2 = qAbs(rfData.Ft4 * 1.0 - rfData.FLo1);
	}

	if (rfData.FLo2 <= rfData.Ft1)
	{
		rfData.Stop_Band = 0;
		stopErr1 = qAbs(rfData.FLo2);
		stopErr2 = qAbs(rfData.Ft1 * 1.0 - rfData.FLo2);
	} else if (rfData.FLo2 <= rfData.Ft2)
	{
		rfData.Stop_Band = 1;
		stopErr1 = qAbs(rfData.FLo2 - rfData.Ft1 * 1.0);
		stopErr2 = qAbs(rfData.Ft2 * 1.0 - rfData.FLo2);
	} else if (rfData.FLo2 <= rfData.Ft3)
	{
		rfData.Stop_Band = 2;
		stopErr1 = qAbs(rfData.FLo2 - rfData.Ft2 * 1.0);
		stopErr2 = qAbs(rfData.Ft3 * 1.0 - rfData.FLo2);
	} else if (rfData.FLo2 <= rfData.Ft4)
	{
		rfData.Stop_Band = 3;
		stopErr1 = qAbs(rfData.FLo2 - rfData.Ft3 * 1.0);
		stopErr2 = qAbs(rfData.Ft4 * 1.0 - rfData.FLo2);
	}

	double yxwc = 5;
	int bdc = rfData.Stop_Band - rfData.Start_Band;

	if (((bdc) > 0) && (sysData.span.span > 0) && ((startErr2 <= yxwc) || (stopErr1 <= yxwc)))
	{
		if ((startErr2 <= yxwc) && (stopErr1 <= yxwc) && (startErr2 <= stopErr1) && (bdc == 1))
		{
			rfData.Start_Band++;
		} else if ((startErr2 <= yxwc) && (stopErr1 <= yxwc) && (startErr2 > stopErr1) && (bdc == 1))
		{
			rfData.Stop_Band--;
		} else if ((startErr2 <= yxwc) && (stopErr1 > yxwc) && (bdc == 1))
		{
			rfData.Start_Band++;
		} else if ((startErr2 > yxwc) && (stopErr1 <= yxwc) && (bdc == 1))
		{
			rfData.Stop_Band--;
		} else if ((startErr2 <= yxwc) && (stopErr1 <= yxwc) && (bdc > 1))
		{
			rfData.Start_Band++;
			rfData.Stop_Band--;
		} else if ((startErr2 <= yxwc) && (stopErr1 > yxwc) && (bdc > 1))
		{
			rfData.Start_Band++;
		} else if ((startErr2 > yxwc) && (stopErr1 <= yxwc) && (bdc > 1))
		{
			rfData.Stop_Band--;
		}
	}

	double freqSep = RFFREQOFFSET; //15e-3;
	double freqLC1 = 2026.4;
	//波段方程
	switch (rfData.Start_Band)
	{
		case 0: //起点在0波段
			rfData.bandNumber = 0x00;

			if (rfData.FLo2 - rfData.FLo1 == 0)
			{
				rfData.N1 = (rfData.FLo1 + freqLC1 + freqSep) / 80.0;
				rfData.N2 = rfData.N3 = rfData.N4 = 0;
				rfData.BAND1_BUG = DEMODSIZE;
				rfData.BAND2_BUG = rfData.BAND3_BUG = rfData.BAND4_BUG = 0;
			} else
			{
				if ((rfData.Stop_Band - rfData.Start_Band) == 0)
				{
					rfData.bandNumber = 0x00;
					rfData.Start1 = rfData.FLo1;
					rfData.N1 = (rfData.Start1 + freqLC1 + freqSep) / 80.0;
					rfData.BAND1_BUG = (int) ((rfData.FLo2 - rfData.FLo1) / rfData.Delta_f + 0.5) + BucLen;
					rfData.BAND2_BUG = rfData.BAND3_BUG = rfData.BAND4_BUG = 0x00;
					rfData.Start2 = rfData.Start3 = rfData.Start4 = 0;
					rfData.N2 = 0x0;
					rfData.N3 = 0x0;
					rfData.N4 = 0x0;
				} else
				{
					rfData.Start1 = rfData.FLo1;
					rfData.N1 = (rfData.Start1 + freqLC1 + freqSep) / 80.0;
					rfData.BAND1_BUG = (int) ((rfData.Ft1 - rfData.Start1) / rfData.Delta_f + 0.5);
					rfData.Start2 = rfData.FLo1 + (rfData.BAND1_BUG + 0) * rfData.Delta_f;
					rfData.FLo3 = rfData.Start2 + 826.4 + freqSep; //MHZ
					rfData.N2 = rfData.FLo3 / 80.0;

					if (rfData.Stop_Band - rfData.Start_Band == 1)
					{
						rfData.BAND2_BUG = (int) ((rfData.FLo2 - rfData.Start2) / rfData.Delta_f + 0.5) + BucLen;
						rfData.BAND3_BUG = rfData.BAND4_BUG = 0x00;
						rfData.Start3 = rfData.Start4 = 0;
						rfData.N3 = 0x0;
						rfData.N4 = 0x0;
					} else
					{
						rfData.BAND2_BUG = (int) ((rfData.Ft2 - rfData.Start2) / rfData.Delta_f + 0.5);
						rfData.Start3 = rfData.Start2 + (rfData.BAND2_BUG + 0) * rfData.Delta_f;
						rfData.FLo4 = rfData.Start3 + 826.4 + freqSep; //MHZ
						rfData.N3 = rfData.FLo4 / 80.0;

						if (rfData.Stop_Band - rfData.Start_Band == 2)
						{
							rfData.BAND3_BUG = (int) ((rfData.FLo2 - rfData.Start3) / rfData.Delta_f + 0.5) + BucLen;
							rfData.BAND4_BUG = 0x00;

							rfData.Start4 = 0;
							rfData.N4 = 0x0;
						} else
						{
							rfData.BAND3_BUG = (int) ((rfData.Ft3 - rfData.Start3) / rfData.Delta_f + 0.5);
							rfData.Start4 = rfData.Start3 + (rfData.BAND3_BUG + 0) * rfData.Delta_f;
							rfData.BAND4_BUG = (int) ((rfData.FLo2 - rfData.Start4) / rfData.Delta_f + 0.5) + BucLen;
							rfData.FLo5 = rfData.Start4 - 826.4 - freqSep;
							rfData.N4 = rfData.FLo5 / 80.0;
						}
					}
				}
			}
			break;
		case 1: //起点在1波段
			rfData.bandNumber = 0x01;

			if (rfData.FLo2 - rfData.FLo1 == 0)
			{
				rfData.N2 = (rfData.FLo1 + 826.4 + freqSep) / 80.0;
				rfData.N1 = rfData.N3 = rfData.N4 = 0;
				rfData.BAND2_BUG = DEMODSIZE;
				rfData.BAND1_BUG = rfData.BAND3_BUG = rfData.BAND4_BUG = 0;
			} else
			{
				rfData.BAND1_BUG = 0x0;
				rfData.N1 = 0x0;
				rfData.Start1 = 0;
				rfData.Start2 = rfData.FLo1;
				rfData.N2 = (rfData.Start2 + 826.4 + freqSep) / 80.0;

				if (rfData.Stop_Band - rfData.Start_Band == 0)
				{
					rfData.BAND2_BUG = (int) ((rfData.FLo2 - rfData.Start2) / rfData.Delta_f + 0.5) + BucLen;
					rfData.BAND3_BUG = rfData.BAND4_BUG = 0x00;
					rfData.Start3 = rfData.Start4 = 0;
					rfData.N3 = rfData.N4 = 0x0;
				} else
				{
					rfData.BAND2_BUG = (int) ((rfData.Ft2 - rfData.Start2) / rfData.Delta_f + 0.5);
					rfData.Start3 = rfData.Start2 + (rfData.BAND2_BUG) * rfData.Delta_f;
					rfData.N3 = (rfData.Start3 + 826.4 + freqSep) / 80.0;

					if (rfData.Stop_Band - rfData.Start_Band == 1)
					{
						rfData.BAND3_BUG = (int) ((rfData.FLo2 - rfData.Start3) / rfData.Delta_f + 0.5) + BucLen;
						rfData.BAND4_BUG = 0x0;
						rfData.Start4 = 0;
						rfData.N4 = 0x0;
					} else
					{
						rfData.BAND3_BUG = (int) ((rfData.Ft3 - rfData.Start3) / rfData.Delta_f + 0.5);
						rfData.Start4 = rfData.Start3 + (rfData.BAND3_BUG + 0) * rfData.Delta_f;
						rfData.N4 = (rfData.Start4 - 826.4 - freqSep) / 80.0;
						rfData.BAND4_BUG = (int) ((rfData.FLo2 - rfData.Start4) / rfData.Delta_f + 0.5) + BucLen;
					}
				}
			}
			break;
		case 2: //起点在2波段
			rfData.bandNumber = 0x02;

			if (rfData.FLo2 - rfData.FLo1 == 0)
			{
				rfData.N3 = (rfData.FLo1 + 826.4 + freqSep) / 80.0;
				rfData.N1 = rfData.N2 = rfData.N4 = 0;
				rfData.BAND3_BUG = DEMODSIZE;
				rfData.BAND2_BUG = rfData.BAND1_BUG = rfData.BAND4_BUG = 0;
			} else
			{
				rfData.BAND1_BUG = rfData.BAND2_BUG = 0x0;
				rfData.N1 = rfData.N2 = 0x0;
				rfData.Start1 = rfData.Start2 = 0;
				rfData.Start3 = rfData.FLo1;
				rfData.N3 = (rfData.Start3 + 826.4 + freqSep) / 80.0;

				if (rfData.Stop_Band - rfData.Start_Band == 0)
				{
					rfData.BAND3_BUG = (int) ((rfData.FLo2 - rfData.Start3) / rfData.Delta_f + 0.5) + BucLen;
					rfData.BAND4_BUG = 0x0;
					rfData.Start4 = 0;
					rfData.N4 = 0x0;
				} else
				{
					rfData.BAND3_BUG = (int) ((rfData.Ft3 - rfData.Start3) / rfData.Delta_f + 0.5);
					rfData.Start4 = rfData.Start3 + (rfData.BAND3_BUG + 0) * rfData.Delta_f;
					rfData.N4 = (rfData.Start4 - 826.4 - freqSep) / 80.0;
					rfData.BAND4_BUG = (int) ((rfData.FLo2 - rfData.Start4) / rfData.Delta_f + 0.5) + BucLen;
				}
			}
			break;
		case 3: //起点在3波段
			rfData.bandNumber = 0x03;
			if (rfData.FLo2 - rfData.FLo1 == 0)
			{
				rfData.N4 = (rfData.FLo1 - 826.4 - freqSep) / 80.0;
				rfData.N1 = rfData.N3 = rfData.N3 = 0;
				rfData.BAND4_BUG = DEMODSIZE;
				rfData.BAND2_BUG = rfData.BAND3_BUG = rfData.BAND1_BUG = 0;
			} else
			{
				rfData.BAND1_BUG = 0x00;
				rfData.BAND2_BUG = 0x00;
				rfData.BAND3_BUG = 0x00;
				rfData.N1 = 0x0;
				rfData.N2 = 0x0;
				rfData.N3 = 0x0;
				rfData.Start1 = rfData.Start2 = rfData.Start3 = 0;
				rfData.Start4 = rfData.FLo1;
				rfData.BAND4_BUG = (int) ((rfData.FLo2 - rfData.FLo1) / rfData.Delta_f + 0.5) + BucLen;
				rfData.N4 = (rfData.FLo1 - 826.4 - freqSep) / 80.0;
			}
			break;
		default:
			break;
	}

	//N1 //高8位整数位，低32位是小数位   起点在第0波段起始频率
	rfData.N1_n = (long int) rfData.N1;
	rfData.N1_f = (unsigned long long) ((rfData.N1 - rfData.N1_n) * 4294967295.0);
	rfData.N1_f1 = (int) (rfData.N1_f & 0xff);
	rfData.N1_f2 = (int) ((rfData.N1_f >> 8) & 0xff);
	rfData.N1_f3 = (int) ((rfData.N1_f >> 16) & 0xff);
	rfData.N1_f4 = (int) ((rfData.N1_f >> 24) & 0xff);
	rfData.N1_n = (int) ((rfData.N1_n - 0) & 0xff);

	//N2 //高8位整数位，低32位是小数位   起点在第1 波段起始频率
	rfData.N2_n = (long int) rfData.N2;
	rfData.N2_f = (unsigned long long) ((rfData.N2 - rfData.N2_n) * 4294967295.0);
	rfData.N2_f1 = (int) (rfData.N2_f & 0xff);
	rfData.N2_f2 = (int) ((rfData.N2_f >> 8) & 0xff);
	rfData.N2_f3 = (int) ((rfData.N2_f >> 16) & 0xff);
	rfData.N2_f4 = (int) ((rfData.N2_f >> 24) & 0xff);
	rfData.N2_n = (int) ((rfData.N2_n - 0) & 0xff);

	//N3  //高8位整数位，低32位是小数位   起点在第2 波段起始频率
	rfData.N3_n = (long int) rfData.N3;
	rfData.N3_f = (unsigned long long) ((rfData.N3 - rfData.N3_n) * 4294967295.0);
	rfData.N3_f1 = (int) (rfData.N3_f & 0xff);
	rfData.N3_f2 = (int) ((rfData.N3_f >> 8) & 0xff);
	rfData.N3_f3 = (int) ((rfData.N3_f >> 16) & 0xff);
	rfData.N3_f4 = (int) ((rfData.N3_f >> 24) & 0xff);
	rfData.N3_n = (int) ((rfData.N3_n - 0) & 0xff);

	//N4  //高8位整数位，低32位是小数位   起点在第3 波段起始频率
	rfData.N4_n = (long int) rfData.N4;
	rfData.N4_f = (unsigned long long) ((rfData.N4 - rfData.N4_n) * 4294967295.0);
	rfData.N4_f1 = (int) (rfData.N4_f & 0xff);
	rfData.N4_f2 = (int) ((rfData.N4_f >> 8) & 0xff);
	rfData.N4_f3 = (int) ((rfData.N4_f >> 16) & 0xff);
	rfData.N4_f4 = (int) ((rfData.N4_f >> 24) & 0xff);
	rfData.N4_n = (int) ((rfData.N4_n - 0) & 0xff);

	rfDownload(0x18, 0x00);//count stop
	demodControlIF();

	//总的BUC的数目  （地址为S4）
	rfDownload(0x10, (((65535 - DEMODSIZE - BucLen) >> 8) & 0xff));
	rfDownload(0x10, ((65535 - DEMODSIZE - BucLen) & 0xff));

	//各个波段BUC的数目
	//0波段 （地址为S13）
	rfDownload(0x34, ((65535 - rfData.BAND1_BUG) >> 8) & 0xff);
	rfDownload(0x34, (65535 - rfData.BAND1_BUG) & 0xff);

	//1波段 （地址为S14）
	rfDownload(0x38, ((65535 - rfData.BAND2_BUG) >> 8) & 0xff);
	rfDownload(0x38, (65535 - rfData.BAND2_BUG) & 0xff);

	//2波段 （地址为S15）
	rfDownload(0x3C, ((65535 - rfData.BAND3_BUG) >> 8) & 0xff);
	rfDownload(0x3C, (65535 - rfData.BAND3_BUG) & 0xff);

	//3波段 （地址为S16）
	rfDownload(0x40, ((65535 - rfData.BAND4_BUG) >> 8) & 0xff);
	rfDownload(0x40, (65535 - rfData.BAND4_BUG) & 0xff);

	//sbuc(地址为S3)
	rfData.Db1 = 65536 - rfData.Db;
	rfDownload(0x0c, (rfData.Db1 >> 8) & 0xff);
	rfDownload(0x0c, (rfData.Db1) & 0xff);

	rfDownload(0x04, rfData.Dnf_f5);
	rfDownload(0x04, rfData.Dnf_f4);
	rfDownload(0x04, rfData.Dnf_f3);
	rfDownload(0x04, rfData.Dnf_f2);
	rfDownload(0x04, rfData.Dnf_f1);

	//start freq
	//0波段 （地址为S0）
	rfDownload(0x00, rfData.N1_n);
	rfDownload(0x00, rfData.N1_f4);
	rfDownload(0x00, rfData.N1_f3);
	rfDownload(0x00, rfData.N1_f2);
	rfDownload(0x00, rfData.N1_f1);

	//1波段 （地址为S10）
	rfDownload(0x28, rfData.N2_n);
	rfDownload(0x28, rfData.N2_f4);
	rfDownload(0x28, rfData.N2_f3);
	rfDownload(0x28, rfData.N2_f2);
	rfDownload(0x28, rfData.N2_f1);

	//2波段 （地址为S11）
	rfDownload(0x2c, rfData.N3_n);
	rfDownload(0x2c, rfData.N3_f4);
	rfDownload(0x2c, rfData.N3_f3);
	rfDownload(0x2c, rfData.N3_f2);
	rfDownload(0x2c, rfData.N3_f1);

	//3波段 （地址为S12）
	rfDownload(0x30, rfData.N4_n);
	rfDownload(0x30, rfData.N4_f4);
	rfDownload(0x30, rfData.N4_f3);
	rfDownload(0x30, rfData.N4_f2);
	rfDownload(0x30, rfData.N4_f1);

	//跟踪源衰减、跟踪源开关、内外参考
	if (sysData.source.isTrackGenOn)
	{
		//跟踪源
		trackatt_temp = 31 - (int) (sysData.source.attentionValue);
		trackatt_tempN1 = trackatt_temp & 0x01;
		trackatt_tempN2 = trackatt_temp >> 1 & 0x01;
		trackatt_tempN3 = trackatt_temp >> 2 & 0x01;
		trackatt_tempN4 = trackatt_temp >> 3 & 0x01;
		trackatt_tempN5 = trackatt_temp >> 4 & 0x01;

		rfDownload(0xfe, (trackatt_tempN1 << 4 | trackatt_tempN2 << 3 | trackatt_tempN3 << 2 | trackatt_tempN4 << 1 | trackatt_tempN5) | (sysData.source.isTrackGenOn << 5) | (sysData.freq.refInternal << 6));
		rfDownload(0xff, 0x00); //跟踪源送数
	} else
	{
		rfDownload(0xfe, 0 | (sysData.source.isTrackGenOn << 5) | (sysData.freq.refInternal << 6));
	}

	//衰减器控制
	rfDownload(0xfd, (15 - (int) (sysData.ampt.att / 3)) | (sysData.ampt.preamtOn << 4) | (1 << 6));

	//sweep time
	rfData.Sweep = sysData.sweep.sweepTime / 1e6;
	rfData.Da1 = rfData.Sweep * 1.0 / (rfData.Db * 1.0 * DEMODSIZE);//
	rfData.Da = (unsigned long) (16777216.0 - ((rfData.Da1 * 15000.0)));

	rfDownload(0x08, (rfData.Da >> 16) & 0xff);
	rfDownload(0x08, (rfData.Da >> 8) & 0xff);
	rfDownload(0x08, (rfData.Da) & 0xff);

	//single sweep
	rfDownload(0x18, (rfData.bandNumber << 5) | 0x02);//count load
	rfDownload(0x18, (rfData.bandNumber << 5) | 0x00);//

	resetBwOfAverageState();

	if (sysData.options.amOn)
	{
		sysData.options.am_cfgchanged = true;
	} else
	{
		sysData.options.fm_cfgchanged = true;
	}
}

int tSysScpi::copyTheMemoryScreen(QString fileName)
{
	if (fileName.isNull() || fileName.isEmpty())
	{
		return __SCPI_FAILED;
	}

	QString tempValue = fileName.trimmed();
	int index = tempValue.indexOf(".", 0);

	if (index != -1)
	{
		tempValue = tempValue.mid(0, index);
	}

	if (fileName.isNull() || fileName.isEmpty())
	{
		return __SCPI_FAILED;
	}

	QString pngFileName = "/home/sky/save/" + tempValue + ".png";

	if (!QFile(pngFileName).exists())
	{
		return __SCPI_FAILED;
	}

	QFile file(pngFileName);
	file.open(QFile::ReadOnly);
	file.seek(0);
	QByteArray ba(file.readAll());
	file.close();

	return returnScpiResult(ba);
}

int tSysScpi::copyTheMemoryTrace(QString fileName)
{
	if (fileName.isNull() || fileName.isEmpty())
	{
		return __SCPI_FAILED;
	}

	QString tempValue = fileName.trimmed();
	int index = tempValue.indexOf(".", 0);

	if (index != -1)
	{
		tempValue = tempValue.mid(0, index);
	}

	if (fileName.isNull() || fileName.isEmpty())
	{
		return __SCPI_FAILED;
	}

	QString pngFileName = "/home/sky/save/" + tempValue + ".csv";

	if (!QFile(pngFileName).exists())
	{
		return __SCPI_FAILED;
	}

	QFile file(pngFileName);
	file.open(QFile::ReadOnly);
	file.seek(0);
	QByteArray ba(file.readAll());
	file.close();

	return returnScpiResult(ba);
}

int tSysScpi::copyAllMemoryScreen(void)
{

}

int tSysScpi::copyAllMemoryTrace(void)
{

}

int tSysScpi::copyAllMemory(void)
{

}

int tSysScpi::deleteTheMemoryScreen(QString fileName)
{
	if (fileName.isNull() || fileName.isEmpty())
	{
		return __SCPI_FAILED;
	}

	QString tempValue = fileName.trimmed();
	int index = tempValue.indexOf(".", 0);

	if (index != -1)
	{
		tempValue = tempValue.mid(0, index);
	}

	if (fileName.isNull() || fileName.isEmpty())
	{
		return __SCPI_FAILED;
	}

	QString pngFileName = "rm /home/sky/save/" + tempValue + ".png";

	if (system(pngFileName.toStdString().c_str()) == 0)
	{
		return __SCPI_SUCCESS;
	} else
	{
		return __SCPI_FAILED;
	}
}

int tSysScpi::deleteTheMemoryTrace(QString fileName)
{
	if (fileName.isNull() || fileName.isEmpty())
	{
		return __SCPI_FAILED;
	}

	QString tempValue = fileName.trimmed();
	int index = tempValue.indexOf(".", 0);

	if (index != -1)
	{
		tempValue = tempValue.mid(0, index);
	}

	if (fileName.isNull() || fileName.isEmpty())
	{
		return __SCPI_FAILED;
	}

	QString csvFileName = "rm /home/sky/save/" + tempValue + ".csv";
	QString pngFileName = "rm /home/sky/save/trace/" + tempValue + ".png";

	if (system(csvFileName.toStdString().c_str()) == 0 || system(pngFileName.toStdString().c_str()) == 0)
	{
		return __SCPI_SUCCESS;
	} else
	{
		return __SCPI_FAILED;
	}
}

int tSysScpi::deleteAllMemoryScreen(void)
{
	system("rm /home/sky/save/*.png");

	return __SCPI_SUCCESS;
}

int tSysScpi::deleteAllMemoryTrace(void)
{
	system("rm /home/sky/save/*.csv");
	system("rm /home/sky/save/trace/*");

	return __SCPI_SUCCESS;
}

int tSysScpi::deleteAllMemory(void)
{
	system("rm /home/sky/save/*");
	system("rm /home/sky/save/trace/*");

	return __SCPI_SUCCESS;
}

int tSysScpi::loadTheMemoryScreen(QString fileName)
{
	if (fileName.isNull() || fileName.isEmpty())
	{
		sysData.mode = smSpectrum;
		sysData.canvasNums = 1;
		sysData.file.isRestoring = false;
		fileSelectName = "";
		return __SCPI_FAILED;
	}

	sysData.file.show = stAll;
	getSystemFileList();

	QString fileName1 = "";
	QString fileName2 = "";
	QString fileName3 = fileName.toUpper();

	for (int i = 0; i < fileList.count(); i++)
	{
		if (fileList.at(i).suffix().toUpper() == "PNG")
		{
			fileName1 = fileList.at(i).baseName().toUpper();
			fileName2 = fileList.at(i).baseName().toUpper() + "." + fileList.at(i).suffix().toUpper();

			if (fileName1 == fileName3 || fileName2 == fileName3)
			{
				sysData.mode = smFile;
				sysData.canvasNums = 1;
				sysData.file.isRestoring = true;
				fileSelectName = fileList.at(i).absoluteFilePath();
				fileSelectPixmap = QPixmap(fileSelectName).copy(QRect(0, TOPHEIGHT, SCREENWIDTH * CANVASWIDTH, SCREENHEIGHT - TOPHEIGHT - BOTTOMHEIGHT));

				return __SCPI_SUCCESS;
			}
		}
	}

	sysData.mode = smSpectrum;
	sysData.canvasNums = 1;
	sysData.file.isRestoring = false;
	fileSelectName = "";

	return __SCPI_FAILED;
}

int tSysScpi::loadTheMemoryTrace(QString fileName)
{
	if (fileName.isNull() || fileName.isEmpty())
	{
		sysData.mode = smSpectrum;
		sysData.canvasNums = 1;
		sysData.file.isRestoring = false;
		fileSelectName = "";
		return __SCPI_FAILED;
	}

	sysData.file.show = stAll;
	getSystemFileList();

	QString fileName1 = "";
	QString fileName2 = "";
	QString fileName3 = fileName.toUpper();

	for (int i = 0; i < fileList.count(); i++)
	{
		if (fileList.at(i).suffix().toUpper() == "CSV")
		{
			fileName1 = fileList.at(i).baseName().toUpper();
			fileName2 = fileList.at(i).baseName().toUpper() + "." + fileList.at(i).suffix().toUpper();

			if (fileName1 == fileName3 || fileName2 == fileName3)
			{
				sysData.mode = smFile;
				sysData.canvasNums = 1;
				sysData.file.isRestoring = true;
				fileSelectName = fileList.at(i).absolutePath() + "/trace/" + fileList.at(i).baseName() + ".png";

				if (QFile(fileSelectName).exists())
				{
					fileSelectPixmap = QPixmap(fileSelectName).copy(QRect(0, TOPHEIGHT, SCREENWIDTH * CANVASWIDTH, SCREENHEIGHT - TOPHEIGHT - BOTTOMHEIGHT));
					return __SCPI_SUCCESS;
				} else
				{
					break;
				}
			}
		}
	}

	sysData.mode = smSpectrum;
	sysData.canvasNums = 1;
	sysData.file.isRestoring = false;
	fileSelectName = "";

	return __SCPI_FAILED;
}

QString tSysScpi::getSystemFileListString(void)
{
	sysData.file.show = stAll;
	getSystemFileList();

	QString result = "";
	QString fileName = "";

	for (int i = 0; i < fileList.count(); i++)
	{
		fileName = fileList.at(i).baseName() + "." + fileList.at(i).suffix();

		if (i == 0)
		{
			result = fileName;
		} else
		{
			result += ", " + fileName;
		}
	}

	return result;
}

QString tSysScpi::getSystemDiskInformation(void)
{
	sysData.file.show = stAll;
	getSystemFileList();

	QString result = "";
	char labelChar[32] = { 0 };

	int screenCount = 0;
	int traceCount = 0;

	for (int i = 0; i < fileList.count(); i++)
	{
		if (fileList.at(i).suffix().toUpper() == "PNG")
		{
			screenCount++;
		} else if (fileList.at(i).suffix().toUpper() == "CSV")
		{
			traceCount++;
		}
	}

	result = getTheTranslatorString("Free disk space") + " = " + QString(floatToStringDot2(sysData.file.freeDiskSpace * 100, labelChar)).trimmed() + "%";
	result += ", " + getTheTranslatorString("Total Files") + " = " + QString(intToString(fileList.count(), labelChar)).trimmed();
	result += ", " + getTheTranslatorString("Screen Files") + " = " + QString(intToString(screenCount, labelChar)).trimmed();
	result += ", " + getTheTranslatorString("Trace Files") + " = " + QString(intToString(traceCount, labelChar)).trimmed();

	return result;
}

QString tSysScpi::getSystemInfoString(void)
{
	char temp[32] = { };
	QStringList list;

	list.clear();
	list << getTheTranslatorString("Serial Number") + " = " + QString(sysData.system.serialNo).trimmed();// intToString(sysData.system.serialNo, temp);
	sprintf(temp, "%d.%d.%d.%d.%d", VER_MAJOR, VER_MINOR, VER_REVISION, VER_BUILD, VER_TAG);
	list << getTheTranslatorString("Hardware Version") + " = " + QString(temp).trimmed();
	list << getTheTranslatorString("temperature") + " = " + QString(floatToStringDot2(sysData.temperature, temp)).trimmed() + "℃";

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

QString tSysScpi::getSystemLogString(void)
{
	QStringList list = loadLogOfFactoryCalibrate();

	QString result = "";

	for (int i = 0; i < list.count(); i++)
	{
		if (i == 0)
		{
			result = list.at(i).trimmed();
		} else
		{
			result += ", " + list.at(i).trimmed();
		}
	}

	return result;
}

QString tSysScpi::getSystemMeasureModeName(void)
{
	QString result = "";

	if (sysData.measure.isTimeSpectrumOn)
	{
		result = "SATime";
	} else if (sysData.measure.isAcprOn)
	{
		result = "ACPower";
	} else if (sysData.measure.isChannelPowerOn)
	{
		result = "CHPower";
	} else if (sysData.measure.isObwOn)
	{
		result = "OBW";
	} else if (sysData.disc.isDiscOn)
	{
		result = "DISCriminator";
	} else
	{
		result = "SANalyzer";
	}

	return result;
}

int tSysScpi::setMarkerMode(int mkrIndex, QString modeName)
{
	if (mkrIndex >= 0 && mkrIndex < MARKERCOUNT)
	{
		QString mode = modeName.toUpper().trimmed();

		if (mode == "CLOS" || mode == "CLOSE")
		{
			sysData.marker[mkrIndex].state = msClose;
			return __SCPI_SUCCESS;
		} else if (mode == "NORM" || mode == "NORMAL")
		{
			setMarkerNormal(mkrIndex, sysData.marker[mkrIndex].traceIndex);
			return __SCPI_SUCCESS;
		} else if (mode == "DELT" || mode == "DELTA")
		{
			setMarkerDelta(mkrIndex, sysData.marker[mkrIndex].traceIndex);
			return __SCPI_SUCCESS;
		}
	}

	return __SCPI_FAILED;
}

QString tSysScpi::getMarkerModeName(int index)
{
	if (index >= 0 && index <= (MARKERCOUNT - 1))
	{
		switch (sysData.marker[index].state)
		{
			case msClose:
				return "CLOSe";
				break;
			case msNormal:
				return "NORMal";
				break;
			case msDelta:
				return "DELTa";
				break;
		}
	}

	return "";
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
	QStringList unitList = (QStringList() << "DBM" << "DBMV" << "DBUV" << "MW" << "W" << "UW" << "NW" << "V" << "MV" << "UV" << "NV");

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
					case 0:
						tempValue *= 1;
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

//所有频标关闭
bool tSysScpi::isAllMarkerOff(void)
{
	for (int i = 0; i < MARKERCOUNT; i++)
	{
		if (sysData.marker[i].state != msClose)
		{
			return false;
		}
	}

	return true;
}

//指定频标关闭
bool tSysScpi::isTheMarkerOff(int index)
{
	return (index >= 0 && index < MARKERCOUNT && sysData.marker[index].state == msClose);
}

//日期合法
int tSysScpi::isValidDate(QString value)
{
	if (value.isNull() || value.isEmpty() || value.length() != 8)
	{
		return _MSG_DATE_INVALID;
	}

	QString tempStr = value.mid(0, 4);
	int year = 0;
	int month = 0;
	int day = 0;
	bool isOk = false;

	year = tempStr.toInt(&isOk);

	if (!isOk || year < 2000 || year > 3000)
	{
		return _MSG_DATE_YEAR_INVALID;
	}

	tempStr = value.mid(4, 2);
	month = tempStr.toInt(&isOk);

	if (!isOk || month < 1 || month > 12)
	{
		return _MSG_DATE_MONTH_INVALID;
	}

	tempStr = value.mid(6, 2);
	day = tempStr.toInt(&isOk);

	if (!isOk || day < 1 || day > 31)
	{
		return _MSG_DATE_DAY_INVALID;
	}

	if ((month == 4) || (month == 6) || (month == 9) || (month == 11))
	{
		if (day > 30)
		{
			return _MSG_DATE_MONTH_INVALID;
		}
	}

	if (((year % 4 == 0) && (year % 100 != 0)) || ((year % 100 == 0) && (year % 400 == 0)))
	{
		if ((day > 29) && (month == 2))
		{
			return _MSG_DATE_MONTH_INVALID;
		}
	} else
	{
		if ((day > 28) && (month == 2))
		{
			return _MSG_DATE_MONTH_INVALID;
		}
	}

	return _RESULT_SUCCESS;
}

//时间合法
int tSysScpi::isValidTime(QString value)
{
	if (value.isNull() || value.isEmpty() || value.length() != 6)
	{
		return _MSG_TIME_INVALID;
	}

	QString tempStr = value.mid(0, 4);
	int hour = 0;
	int minute = 0;
	int second = 0;
	bool isOk = false;

	tempStr = value.mid(0, 2);
	hour = tempStr.toInt(&isOk);

	if (!isOk || hour < 0 || hour > 24)
	{
		return _MSG_TIME_HOUR_INVALID;
	}

	tempStr = value.mid(2, 2);
	minute = tempStr.toInt(&isOk);

	if (!isOk || minute < 0 || minute > 59)
	{
		return _MSG_TIME_MINUTE_INVALID;
	}

	tempStr = value.mid(4, 2);
	second = tempStr.toInt(&isOk);

	if (!isOk || second < 0 || second > 59)
	{
		return _MSG_TIME_SECOND_INVALID;
	}

	return _RESULT_SUCCESS;
}

//设置新日期
int tSysScpi::setNewDate(QString value)
{
	int year = value.mid(0, 4).toInt();
	int month = value.mid(4, 2).toInt();
	int day = value.mid(6, 2).toInt();

	int tempValue = -1;
	char temp[32];
	getLocalDateTime(temp);

	sysData.system.datetime.year = year;
	sysData.system.datetime.month = month;
	sysData.system.datetime.day = day;

	tempValue = setLocalDateTime();
	return tempValue;
}

//设置新时间
int tSysScpi::setNewTime(QString value)
{
	int hour = value.mid(0, 2).toInt();
	int minute = value.mid(2, 2).toInt();
	int second = value.mid(4, 2).toInt();

	int tempValue = -1;
	char temp[32];
	getLocalDateTime(temp);

	sysData.system.datetime.hour = hour;
	sysData.system.datetime.minute = minute;
	sysData.system.datetime.second = second;

	tempValue = setLocalDateTime();
	return tempValue;
}

//获取新日期字符串
QString tSysScpi::getNewDateString(QString value)
{
	int year = value.mid(0, 4).toInt();
	int month = value.mid(4, 2).toInt();
	int day = value.mid(6, 2).toInt();
	char temp[64] = { };

	time_t t = time(NULL);
	tm *tp = localtime(&t);
	// sprintf(temp, "%02d%02d%02d%02d%04d.%02d", month, day, tp->tm_hour, tp->tm_min, year, tp->tm_sec);
	sprintf(temp, "%04d%02d%02d%02d%02d.%02d", year, month, day, tp->tm_hour, tp->tm_min, tp->tm_sec);

	return (QString) temp;
}

//获取新时间字符串
QString tSysScpi::getNewTimeString(QString value)
{
	int hour = value.mid(0, 2).toInt();
	int minute = value.mid(2, 2).toInt();
	int second = value.mid(4, 2).toInt();
	char temp[64] = { };

	time_t t = time(NULL);
	tm *tp = localtime(&t);
	sprintf(temp, "%04d%02d%02d%02d%02d.%02d", tp->tm_year + 1900, tp->tm_mon + 1, tp->tm_mday, hour, minute, second);

	return (QString) temp;
}

//获取峰值列表
int tSysScpi::getPeakList(int traceIndex)
{
	if (traceIndex < 0 || traceIndex >= TRACECOUNT)
	{
		return __SCPI_FAILED;
	}

	double ndb = 10.0;
	double diffValue[sysData.sweep.sweepPoints];
	int lIndex = 0;
	int rIndex = 0;
	double value = 0;
	double lValue = 0;
	double rValue = 0;

	if (sysData.disc.isDiscOn)
	{
		ndb = 1.0;
	}

	posPeakList.clear();
	diff(diffValue, sysData.trace[traceIndex].value, sysData.sweep.sweepPoints);

	for (int i = 0; i < sysData.sweep.sweepPoints; i++)
	{
		if (diffValue[i] >= 0)
		{
			if (i == sysData.sweep.sweepPoints - 1)
			{
				for (int j = i - 1; j >= 0; j--)
				{
					if (diffValue[j] >= 0)
					{
						lIndex = j;
						lValue = sysData.trace[traceIndex].value[lIndex];

						if (qAbs(value - lValue) >= ndb)
						{
							posPeakList.push_back(i);
							break;
						}
					} else
					{
						break;
					}
				}
			}

			continue;
		}

		lIndex = -1;
		rIndex = -1;
		value = sysData.trace[traceIndex].value[i];

		if (diffValue[i] >= 0)
		{
			continue;
		} else if (diffValue[i] < 0)
		{
			if (i == 0)
			{
				for (int j = i + 1; j < sysData.sweep.sweepPoints; j++)
				{
					if (diffValue[j] <= 0)
					{
						rIndex = j;
						rValue = sysData.trace[traceIndex].value[rIndex];

						if (qAbs(value - rValue) >= ndb)
						{
							posPeakList.push_back(i);
							break;
						}
					} else
					{
						break;
					}
				}
			} else if (i > 0 && diffValue[i - 1] >= 0)
			{
				lIndex = -1;
				rIndex = -1;

				for (int j = i - 1; j >= 0; j--)
				{
					if (diffValue[j] >= 0)
					{
						lIndex = j;
						lValue = sysData.trace[traceIndex].value[lIndex];
					} else
					{
						break;
					}
				}

				if (lIndex != -1)
				{
					for (int j = i + 1; j < sysData.sweep.sweepPoints; j++)
					{
						if (diffValue[j] > 0)
						{
							rIndex = j;
							rValue = sysData.trace[traceIndex].value[rIndex];
							break;
						}
					}
				}
			}
		}

		if ((lIndex != -1) && (rIndex != -1))
		{
			if ((qAbs(value - lValue) >= ndb) || (qAbs(value - rValue) >= ndb))
			{
				posPeakList.push_back(i);
			}
		}
	}

	if (sysData.span.span > 0 && sysData.freq.start < 5 * sysData.bw.rbw && posPeakList.count() > 0 && posPeakList[0] == 0 && !sysData.isFactoryCalibrating && !sysData.isZcCalibrating && !sysData.isZcPreamplifierCalibrating && !sysData.isPreamplifierCalibrating)
	{
		posPeakList.remove(0);
	}

	if (posPeakList.count() > 0)
	{
		return __SCPI_SUCCESS;
	} else
	{
		return __SCPI_FAILED;
	}
}

//数据求导
double* tSysScpi::diff(double* output, const double * input, int n)
{
	double* out = output;
	const double * in = input;

	for (int i = 0; i < n - 1; i++)
	{
		*out++ = *(in + 1) - *in;
		*in++;
	}

	*out = 0 - *in;

	return output;
}

//复位频率计数参数
void tSysScpi::resetFreqCountParam(void)
{
	if (!sysData.markerFctn.isFreqCountOn && sysData.markerFctn.freqCountParam.isSaved)
	{
		sysData.markerFctn.freqCountParam.isSaved = false;
		sysData.freq.center = sysData.markerFctn.freqCountParam.centerFreq;
		sysData.span.span = sysData.markerFctn.freqCountParam.span;
		sysData.freq.start = sysData.markerFctn.freqCountParam.startFreq;
		sysData.freq.stop = sysData.markerFctn.freqCountParam.stopFreq;

		sysData.bw.rbw = sysData.markerFctn.freqCountParam.rbw;
		sysData.bw.rbwAuto = sysData.markerFctn.freqCountParam.rbwAuto;
		sysData.bw.vbw = sysData.markerFctn.freqCountParam.vbw;
		sysData.bw.vbwAuto = sysData.markerFctn.freqCountParam.vbwAuto;

		sysData.ampt.refLevel = sysData.markerFctn.freqCountParam.refLevel;
		sysData.ampt.att = sysData.markerFctn.freqCountParam.att;
		sysData.ampt.att1 = sysData.markerFctn.freqCountParam.att1;
		sysData.ampt.att2 = sysData.markerFctn.freqCountParam.att2;
		sysData.ampt.attAuto = sysData.markerFctn.freqCountParam.attAuto;

		sysData.sweep.sweepTime = sysData.markerFctn.freqCountParam.sweepTime;
		sysData.sweep.sweepTimeAuto = sysData.markerFctn.freqCountParam.sweepTimeAuto;
	}
}

//设置测量扫宽
double tSysScpi::setSpanOfMeasure(double value)
{
	double tempValue = value;

	if (sysData.measure.isAcprOn || sysData.measure.isChannelPowerOn || sysData.measure.isObwOn)
	{
		if (sysData.measure.isChannelPowerOn || sysData.measure.isObwOn)
		{
			if (tempValue < sysData.measureSetup.channelWidth)
			{
				tempValue = sysData.measureSetup.channelWidth;
			}
		} else if (sysData.measure.isAcprOn)
		{
			double minValue = (sysData.measureSetup.channelNums * 2 + 1) * sysData.measureSetup.channelSpace;

			if (tempValue < minValue)
			{
				tempValue = minValue;
			}
		}
	}

	return tempValue;
}

//设置自动调谐频率
int tSysScpi::setAutoTuneFreq(double startFreq, double stopFreq)
{
	setFrequencyOfStart(startFreq);
	setFrequencyOfStop(stopFreq);
	controlRf();

	return __SCPI_SUCCESS;
}

//获取自动调谐峰值索引
int tSysScpi::getAutoTunePeakIndex(void)
{
	int index = 0;
	int index1 = -1;
	int index2 = -1;
	double value = 0;
	double lValue = 0;
	double rValue = 0;
	double downValue = 0;
	double upValue = 0;
	double powerLimit = 12;

	for (int i = 0; i < 2; i++)
	{
		usleep(sysData.sweep.sweepTime / 1e3 + 50 * 1000);
		getDataFromIF();
		index1 = getMarkerPeakMaxIndex(0, 0);

		usleep(sysData.sweep.sweepTime / 1e3 + 50 * 1000);
		getDataFromIF();
		index2 = getMarkerPeakMaxIndex(0, 0);

		if (index1 != -1 && index2 != -1 && qAbs(index1 - index2) <= 3)
		{
			index = (index1 + index2) / 2;
			value = sysData.trace[0].value[index];
			lValue = value;
			rValue = value;
			downValue = 0;
			upValue = 0;

			for (int j = index - 1; j >= 0; j--)
			{
				downValue = sysData.trace[0].value[j];
				upValue = sysData.trace[0].value[j + 1];

				if (downValue <= value && upValue <= value && downValue <= upValue)
				{
					lValue = downValue;
				} else
				{
					break;
				}
			}

			for (int j = index + 1; j < sysData.sweep.sweepPoints; j++)
			{
				downValue = sysData.trace[0].value[j];
				upValue = sysData.trace[0].value[j - 1];

				if (downValue <= value && upValue <= value && downValue <= upValue)
				{
					rValue = downValue;
				} else
				{
					break;
				}
			}

			if (qAbs(value - lValue) >= powerLimit || qAbs(value - rValue) >= powerLimit)
			{
				return index;
			}
		}
	}

	return -1;
}

//设置自动调谐峰值至中心频率
int tSysScpi::setAutoTunePeakToCenter(void)
{
	setMarkerPeakToCenterFreq(0, 0);

	if (sysData.span.span <= 0)
	{
		setFrequencyOfStop(MAXFREQ);
		setFrequencyOfStart(MAXFREQ - TUNESPAN);
		controlRf();
	} else if (sysData.span.span >= TUNESPAN)
	{
		setFrequencyOfSpan(TUNESPAN);
		controlRf();
	}

	return __SCPI_SUCCESS;
}

//设置自动调谐参考电平
int tSysScpi::setAutoTuneRefLevel(void)
{
	int peakIndex = -1;
	int index1 = -1;
	int index2 = -1;

	for (int i = 0; i < 3; i++)
	{
		usleep(sysData.sweep.sweepTime / 1e3 + 50 * 1000);
		getDataFromIF();
		index1 = getMarkerPeakMaxIndex(0, 0);

		usleep(sysData.sweep.sweepTime / 1e3 + 50 * 1000);
		getDataFromIF();
		index2 = getMarkerPeakMaxIndex(0, 0);

		if (index1 != -1 && index2 != -1 && qAbs(index1 - index2) <= 3)
		{
			peakIndex = (index1 + index2) / 2;
			break;
		}
	}

	if (peakIndex == -1)
	{
		return __SCPI_FAILED;
	}

	sysData.marker[0].markerNormal.xIndex = peakIndex;

	if (peakIndex != sysData.sweep.sweepPoints - 1)
	{
		setMarkerPeakToCenterFreq(0, 0);
	}

	usleep(sysData.sweep.sweepTime / 1e3 + 50 * 1000);
	getDataFromIF();
	peakIndex = getMarkerPeakMaxIndex(0, 0);

	if (peakIndex >= 0)
	{
		int ampt = (int) (getTheMarkerYValue(0, peakIndex) + 10.0);

		if (ampt <= -30.0)
		{
			ampt = -30.0;
			setBwOfRbwAuto(1);
		}

		setAmptOfRefLevel(ampt * 1.0);
		controlRf();
	}

	setMarkerPeakMax(0, 0);

	//last locate
	for (int i = 0; i < 3; i++)
	{
		usleep(sysData.sweep.sweepTime / 1e3 + 50 * 1000);
		getDataFromIF();
		index1 = getMarkerPeakMaxIndex(0, 0);

		usleep(sysData.sweep.sweepTime / 1e3 + 50 * 1000);
		getDataFromIF();
		index2 = getMarkerPeakMaxIndex(0, 0);

		if (index1 != -1 && index2 != -1 && qAbs(index1 - index2) <= 3)
		{
			peakIndex = (index1 + index2) / 2;
			break;
		}
	}

	if (peakIndex == -1)
	{
		return __SCPI_FAILED;
	}

	sysData.marker[0].markerNormal.xIndex = peakIndex;

	if (peakIndex != sysData.sweep.sweepPoints - 1)
	{
		setMarkerPeakToCenterFreq(0, 0);
	}

	return __SCPI_SUCCESS;
}

//关闭所有通信端口
void tSysScpi::closeScpiPortOfAll(void)
{
	closeScpiPortOfLan();
	closeScpiPortOfUsb();
	closeScpiPortOfGpib();
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

//关闭GPIB端口通信
void tSysScpi::closeScpiPortOfGpib(void)
{
	if (sysData.scpiData.gpibOpened)
	{
		sysData.scpiData.gpibOpened = false;
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

//信号源在线
int tSysScpi::signalIsOnline(bool isCom)
{
	if (isCom && comHandle <= 0)
	{
		return __SCPI_FAILED;
	}

	if (isCom)
	{
		closeScpiPortOfAll();
	}

	QString cmd = "*IDN?";
	int cmdLen = cmd.length();
	char buf[SCPIBUFSIZE] = { };
	int length = 0;

	for (int i = 0; i < 3; i++)
	{
		memset(&sysData.scpiData.buf, 0, sizeof sysData.scpiData.buf);
		sysData.scpiData.rcvLength = 0;
		sysData.scpiData.stateT = true;

		if (isCom)
		{
			length = read(comHandle, buf, sizeof buf);
		} else
		{
			if (cSocket->waitForReadyRead(3000))
			{
				length = cSocket->read(buf, sizeof buf);
			}
		}

		if (sendSerialData(isCom, cmd) != __SCPI_SUCCESS)
		{
			return __SCPI_FAILED;
		}

		usleep(1000 * 500);

		if (sysData.scpiData.rcvLength > 0)
		{
			return __SCPI_SUCCESS;
		} else
		{
			memset(buf, 0, sizeof buf);

			if (isCom)
			{
				length = read(comHandle, buf, sizeof buf);
			} else
			{
				if (cSocket->waitForReadyRead(3000))
				{
					length = cSocket->read(buf, sizeof buf);
				}
			}

			if (length > 0)
			{
				return __SCPI_SUCCESS;
			}
		}
	}

	return __SCPI_FAILED;
}

//信号源输出频率正常
int tSysScpi::signalOutputFreqIsvalid(bool isCom, double value)
{
	if (isCom && comHandle <= 0)
	{
		return __SCPI_FAILED;
	}

	char tempChar[32] = { };

	floatToString(value, 0, 0, tempChar);

	QString cmd = ":FREQ " + QString(tempChar).trimmed(); //cmd of set freq

	int cmdLen = cmd.length();
	QString rcvValue = "";
	bool isOk = false;
	double tempValue = 0;

	for (int i = 0; i < 3; i++)
	{
		memset(&sysData.scpiData.buf, 0, sizeof sysData.scpiData.buf);
		sysData.scpiData.rcvLength = 0;
		sysData.scpiData.stateT = true;

		cmd = ":FREQ " + QString(tempChar).trimmed();

		if (sendSerialData(isCom, cmd) != __SCPI_SUCCESS)
		{
			return __SCPI_FAILED;
		}

		cmd = ":FREQ?";

		if (sendSerialData(isCom, cmd) != __SCPI_SUCCESS)
		{
			return __SCPI_FAILED;
		}

		usleep(1000 * 50);

		if (sysData.scpiData.rcvLength > 0)//handle data received
		{
			rcvValue = (char*) sysData.scpiData.buf;
			isOk = false;
			tempValue = 0;
			if (!rcvValue.isNull() && !rcvValue.isEmpty())
			{
				tempValue = rcvValue.toDouble(&isOk);
				if (isOk && (unsigned long long) tempValue == (unsigned long long) value)
				{
					return __SCPI_SUCCESS;
				}
			}
		} else//receiving data
		{
			char buf[SCPIBUFSIZE] = { };
			unsigned int length = 0;

			memset(buf, 0, sizeof buf);

			if (isCom)
			{
				length = read(comHandle, buf, sizeof buf);
			} else
			{
				if (cSocket->waitForReadyRead(3000))
				{
					length = cSocket->read(buf, sizeof buf);
				}
			}

			if (length > 0)
			{
				__var(buf);
				__var(strdel(buf,"Hz"));
				rcvValue = (char*) buf;
				isOk = false;
				tempValue = 0;

				if (!rcvValue.isNull() && !rcvValue.isEmpty())
				{
					tempValue = rcvValue.toDouble(&isOk);
					if (isOk && (unsigned long long) tempValue == (unsigned long long) value)
					{
						printf("signal freq set %f success! \n",value);
						return __SCPI_SUCCESS;
					}
				}
			}
		}
	}

	if (!isCom)
	{
		disConnectFromServerSocket();
	}
	printf("signal generator set failed!\n");
	return __SCPI_FAILED;
}

//信号源输出幅度正常
int tSysScpi::signalOutputAmptIsvalid(bool isCom, double value)
{
	if (isCom && comHandle <= 0)
	{
		return __SCPI_FAILED;
	}

	char tempChar[32] = { };

	floatToString(value, 0, 0, tempChar);

	QString cmd = ":POW " + QString(tempChar).trimmed();

	int cmdLen = cmd.length();
	QString rcvValue = "";
	bool isOk = false;
	double tempValue = 0;

	for (int i = 0; i < 3; i++)
	{
		memset(&sysData.scpiData.buf, 0, sizeof sysData.scpiData.buf);
		sysData.scpiData.rcvLength = 0;
		sysData.scpiData.stateT = true;

		cmd = ":POW " + QString(tempChar).trimmed();//cmd of set pow 

		if (sendSerialData(isCom, cmd) != __SCPI_SUCCESS)
		{
			return __SCPI_FAILED;
		}

		cmd = ":POW?";

		if (sendSerialData(isCom, cmd) != __SCPI_SUCCESS)
		{
			return __SCPI_FAILED;
		}

		usleep(1000 * 500);

		if (sysData.scpiData.rcvLength > 0)
		{
			rcvValue = (char*) sysData.scpiData.buf;
			isOk = false;
			tempValue = 0;

			if (!rcvValue.isNull() && !rcvValue.isEmpty())
			{
				tempValue = rcvValue.toDouble(&isOk);

				if (isOk && (int) tempValue == (int) value)
				{
					return __SCPI_SUCCESS;
				}
			}
		} else
		{
			char buf[SCPIBUFSIZE] = { };
			unsigned int length = 0;

			memset(buf, 0, sizeof buf);

			if (isCom)
			{
				length = read(comHandle, buf, sizeof buf);
			} else
			{
				if (cSocket->waitForReadyRead(3000))
				{
					length = cSocket->read(buf, sizeof buf);
				}
			}

			if (length > 0)
			{
				__var(buf);
				__var(strdel(buf,"dBm"));
				rcvValue = (char*) buf;
				isOk = false;
				tempValue = 0;

				if (!rcvValue.isNull() && !rcvValue.isEmpty())
				{
					tempValue = rcvValue.toDouble(&isOk);

					if (isOk && (int) tempValue == (int) value)
					{
						printf("signal ampt set %f success!\n",value);
						return __SCPI_SUCCESS;
					}
				}
			}
		}
	}

	if (!isCom)
	{
		disConnectFromServerSocket();
	}

	return __SCPI_FAILED;
}

void tSysScpi::setFrontendAtt(double rfAtt, double ifAtt)
{
	feDownload(207, 0);         //本振送数开关
	setLowbandAtt(rfAtt);
	setHighbandAtt(rfAtt);
	setFrontEndIfAtt(ifAtt);
	rfPreamptDown(sysData.ampt.isPreamptOn);
	
	//feDownload(234, calHighbandAttValueOfFrontend(rfAtt));  //rf of high band 
	//feDownload(235, calLowbandAttValueOfFrontend(rfAtt));   //rf low band
	feDownload(236, calIfAttValueOfFrontend(ifAtt));   //if 
	feDownload(237, getPremptData(sysData.ampt.isPreamptOn));
	
	feDownload(207, 1);         //本振送数开关
	__var(rfAtt);
	__var(ifAtt);
	
}

//设置绝对幅度定标校准参数
void tSysScpi::setCalibrateParamOfAbsoluteAmplitude(void)
{
	setFrequencyOfCenter(1e9);
	setFrequencyOfSpan(1e6);
	setSweepOfTimeAuto(1);
	setBwOfRbwAuto(0);
//	setBwOfVbwAuto(0);
	setBwOfRbw(10e3);
//	setBwOfVbw(3e3);
//	setDetector(dmPosPeak);
	setAmptOfAttAuto(1);

	if (sysData.ampt.isPreamptOn)
	{
		setAmptOfRefLevel(-20);
	} else
	{
		setAmptOfRefLevel(0);
	}

//	sysData.ampt.attIf = 20;
//	sysData.ampt.attRf = 10;

	setFrontendAtt(10, 20);
	controlRf();
}

//设置直采模式绝对幅度定标校准参数
void tSysScpi::setZcCalibrateParamOfAbsoluteAmplitude(void)
{
	setFrequencyOfCenter(10e6);
	setFrequencyOfSpan(100e3); //200e3
	setSweepOfTimeAuto(1);
	setBwOfRbwAuto(0);
	setBwOfVbwAuto(0);
	setBwOfRbw(1e3);
	setBwOfVbw(3e3);
	setDetector(dmPosPeak);
	setAmptOfAttAuto(1);

	if (sysData.ampt.isPreamptOn)
	{
		setAmptOfRefLevel(-20);
	} else
	{
		setAmptOfRefLevel(0);
	}
	
	controlRf();
}

//设置衰减器校准参数
void tSysScpi::setCalibrateParamOfIfAttenuation(void)
{
	setFrequencyOfCenter(1e9);
	setFrequencyOfSpan(1e6);
//	setSweepOfTimeAuto(0);
	setBwOfRbwAuto(0);
//	setBwOfVbwAuto(0);
	setBwOfRbw(10e3);
//	setBwOfVbw(10e3);
//	setDetector(dmPosPeak);
	setAmptOfAttAuto(1);
//	setAmptOfAtt(9);

	if (sysData.ampt.isPreamptOn)
	{
		setAmptOfRefLevel(-20);
	} else
	{
		setAmptOfRefLevel(0);
	}
	//setFrontendAtt(10, 20);
	controlRf();
}

//设置衰减器校准参数
void tSysScpi::setCalibrateParamOfRfAttenuation(void)
{
	setFrequencyOfCenter(1e9);
	setFrequencyOfSpan(1e6);
//	setSweepOfTimeAuto(0);
	setBwOfRbwAuto(0);
//	setBwOfVbwAuto(0);
	setBwOfRbw(10e3);
//	setBwOfVbw(10e3);
//	setDetector(dmPosPeak);
//	setAmptOfAttAuto(0);
//	setAmptOfAtt(9);

	if (sysData.ampt.isPreamptOn)
	{
		setAmptOfRefLevel(-20);
	} else
	{
		setAmptOfRefLevel(0);
	}
	controlRf();
}

//设置两级衰减器参数
void tSysScpi::setCalibrateDistributeAtt(double ref, double att1, double att2)
{
	sysData.ampt.refLevel = ref;
	sysData.ampt.att = att1 + att2;

	//  if(sysData.freq.isLowChannel && (sysData.isZcCalibrating || sysData.isZcPreamplifierCalibrating))
	//  {
	//	//  setAmptOfAtt(sysData.ampt.att);
	//	//  setAmptOfRefLevel(sysData.ampt.refLevel);
	//	  setAmptOfRefLevel(ref);
	//	  setAmptOfAtt(att);
	//	  setAmptOfRefLevel(ref);
	//  }

	controlRf();
}

//设置直采中频增益校准参数
void tSysScpi::setZcCalibrateParamOfIF(void)
{
	setFrequencyOfCenter(5e6);
	setFrequencyOfSpan(1e6); //200e3
	setSweepOfTimeAuto(1);
	setBwOfRbwAuto(0);
	setBwOfVbwAuto(0);
	setBwOfRbw(10e3);
	setBwOfVbw(3e3);
	setDetector(dmPosPeak);
	setAmptOfAttAuto(1);

	if (sysData.ampt.isPreamptOn)
	{
		setAmptOfRefLevel(-20);
	} else
	{
		setAmptOfRefLevel(0);
	}
	setAmptOfAttAuto(0);
	setAmptOfAtt(30); //10
	controlRf();
}
//设置直采通道分辨率校准参数
void tSysScpi::setZcCalibrateParamOfRbw(void)
{
	setFrequencyOfCenter(5e6);
	setFrequencyOfSpan(1e6); //200e3
	setSweepOfTimeAuto(1);
	setBwOfRbwStepMode(1);
	setBwOfRbwAuto(1);
	setBwOfVbwAuto(1);
	setDetector(dmPosPeak);
	setAmptOfAttAuto(1);

	if (sysData.ampt.isPreamptOn)
	{
		setAmptOfRefLevel(-20);
	} else
	{
		setAmptOfRefLevel(0);
	}
	controlRf();
}

//设置中频增益校准参数
void tSysScpi::setCalibrateParamOfIF(void)
{
	setFrequencyOfCenter(440e6);
	setFrequencyOfSpan(100e3);
	setSweepOfTimeAuto(0);
	setSweepOfTime(150 * 1e6);
	setBwOfRbwAuto(0);
	setBwOfVbwAuto(0);
	setBwOfRbw(10e3);
	setBwOfVbw(10e3);
	setDetector(dmPosPeak);
	setAmptOfAttAuto(1);

	if (sysData.ampt.isPreamptOn)
	{
		setAmptOfRefLevel(-20);
	} else
	{
		setAmptOfRefLevel(0);
	}
	setAmptOfAttAuto(0);
	setAmptOfAtt(10);
	controlRf();
}

//设置直采通道中频增益参数
void tSysScpi::setZcCalibrateDistributeIF(double value)
{
	unsigned char ifDown = 0x00;

	if (ifHandle >= 0)
	{
		ifDown = getZcIfdown(value);

		rfDownload(0xf9, ifDown);
	}
}

//设置中频增益参数
void tSysScpi::setCalibrateDistributeIF(double value)
{
	int ifZs = (int) value / 10;

	if (ifHandle >= 0)
	{
		switch (ifZs)
		{
			case 0:
				rfDownload(0x54, (sysData.demod.demodOn << 2) | (0x08 >> 3));
				break;
			case 1:
				rfDownload(0x54, (sysData.demod.demodOn << 2) | (0x00 >> 3));
				break;
			case 2:
				rfDownload(0x54, (sysData.demod.demodOn << 2) | (0x18 >> 3));
				break;
			case 3:
				rfDownload(0x54, (sysData.demod.demodOn << 2) | (0x10 >> 3));
				break;
			default:
				rfDownload(0x54, (sysData.demod.demodOn << 2) | (0x10 >> 3));
				break;
		}
	}
}

//设置分辨率校准参数
void tSysScpi::setCalibrateParamOfRbw(void)
{
	setFrequencyOfCenter(440e6);
	setFrequencyOfSpan(100e3);
	setSweepOfTimeAuto(1);
	setBwOfRbwStepMode(1);
	setBwOfRbwAuto(1);
	setBwOfVbwAuto(1);
	setDetector(dmPosPeak);
	setAmptOfAttAuto(1);

	if (sysData.ampt.isPreamptOn)
	{
		setAmptOfRefLevel(-20);
	} else
	{
		setAmptOfRefLevel(0);
	}
	controlRf();
}

//设置分辨率参数
void tSysScpi::setCalibrateDistributeRbw(double value)
{
	setFrequencyOfSpan(value);
	controlRf();
}

//设置直采通道频率响应校准参数
void tSysScpi::setZcCalibrateParamOfFrequencyRespond(void)
{
	setSweepOfTimeAuto(1);
	setBwOfRbwAuto(0);
	setBwOfVbwAuto(1);
	setDetector(dmPosPeak);
	setAmptOfAttAuto(0);
	controlRf();
}

//设置频率响应校准参数
void tSysScpi::setCalibrateParamOfFrequencyRespond(void)
{
	setSweepOfTimeAuto(0);
	setSweepOfTime(150 * 1e6);
	setBwOfRbwAuto(0);
	setBwOfVbwAuto(1);
	setDetector(dmPosPeak);
	setAmptOfAttAuto(0);
	controlRf();
}

//设置直采通道频率响应参数
void tSysScpi::setZcCalibrateDistributeFrequencyRespond(int freqIndex, double ref, double att)
{
//	setAmptOfRefLevel(ref);
	setAmptOfAtt(att);
	setAmptOfRefLevel(ref);

	setSweepOfTimeAuto(1);

	setFrequencyOfCenter(FREQRESPHZ_ZC[freqIndex]);
	if (FREQRESPHZ_ZC[freqIndex] < 10e3)
	{
		setFrequencyOfSpan(1e3);
		setBwOfRbw(10);
	} else
	{
//		setFrequencyOfSpan(100e3);
//		setBwOfRbw(10e3);
		setFrequencyOfSpan(100e3);
		setBwOfRbw(5e3);
	}

//	if(ref == 0 && att == 9)
	controlRf();
}

//设置频率响应参数
void tSysScpi::setCalibrateDistributeFrequencyRespond(int freqIndex, double ref, double rfAtt, double ifAtt)
{
//	setAmptOfRefLevel(ref);
//	setAmptOfAtt(att);

	setFrontendAtt(rfAtt, ifAtt);

	if(freqIndex == 0)
	{
		setFrequencyOfStart(FREQRESPHZ[freqIndex]);
		setFrequencyOfStop(FREQRESPHZ[freqIndex] + 1e6);
	} else if(freqIndex == 372)
	{
		setFrequencyOfStart(FREQRESPHZ[freqIndex] - 1e6);
		setFrequencyOfStop(FREQRESPHZ[freqIndex]);
	} else
	{
		setFrequencyOfCenter(FREQRESPHZ[freqIndex]);
	}
	setFrequencyOfSpan(1e6);
	setBwOfRbw(10e3);
//	if (freqIndex == 0 || freqIndex == 1)
//	{
//	} else if (freqIndex == 2 || freqIndex == 3)
//	{
//		setFrequencyOfCenter(FREQRESPHZ[freqIndex]);
//		setFrequencyOfSpan(1e6);
//		setBwOfRbw(10e3);
//	} else if (freqIndex == 4)
//	{
//		setFrequencyOfCenter(FREQRESPHZ[freqIndex]);
//		setFrequencyOfSpan(200e3);
//		setBwOfRbw(10e3);
//	} else if (freqIndex == 36 || freqIndex == 37 || freqIndex == 55 || freqIndex == 56 || freqIndex == 74 || freqIndex == 75)
//	{
//		setFrequencyOfStop(FREQRESPHZ[freqIndex]);
//		setFrequencyOfStart(FREQRESPHZ[freqIndex] - 11e6);
//		setBwOfRbw(10e3);
//	} else if (freqIndex == 38 || freqIndex == 57 || freqIndex == 76)
//	{
//		setFrequencyOfStart(FREQRESPHZ[freqIndex]);
//		setFrequencyOfStop(FREQRESPHZ[freqIndex] + 11e6);
//		setBwOfRbw(10e3);
//	} else
//	{
//		setFrequencyOfCenter(FREQRESPHZ[freqIndex]);
//		setFrequencyOfSpan(200e3);
//		setBwOfRbw(10e3);
//	}

//	if (sysData.sweep.sweepTime < 50 * 1e6)
//	{
//		setSweepOfTimeAuto(0);
//		setSweepOfTime(sysData.sweep.sweepTime * 3);
//	}

	controlRf();
}

//获取校准运行参数
int tSysScpi::getCalibrateRunParam(void)
{

	memset(&calRunData, 0, sizeof calRunData);
	//usleep(sysData.sweep.sweepTime / 1e3 + 10 * 1000);
	usleep(sysData.sweep.sweepTime  + 100 * 1000);

	int maxIndex1 = 0;
	int maxIndex2 = 0;
	int minIndex1 = 0;
	int minIndex2 = 0;
	double maxValue1 = 0;
	double maxValue2 = 0;
	double minValue1 = 0;
	double minValue2 = 0;
	int ret = 0;
	double databuf[6][sysData.sweep.sweepPoints];

	for (int i = 0; i < 3; i++)
	{
		usleep(sysData.sweep.sweepTime + 100 * 1000);
		getDataFromIF();		
		maxIndex1 = 0;
		minIndex1 = 0;
		maxValue1 = sysData.trace[0].value[0];
		minValue1 = sysData.trace[0].value[0];
		
		for (int i = 1; i < sysData.sweep.sweepPoints; i++)
		{
			if (sysData.trace[0].value[i] > maxValue1)
			{
				maxValue1 = sysData.trace[0].value[i];
				maxIndex1 = i;
			} else if (sysData.trace[0].value[i] < minValue1)
			{
				minValue1 = sysData.trace[0].value[i];
				minIndex1 = i;
			}
		}
		
		usleep(sysData.sweep.sweepTime + 100 * 1000);
		getDataFromIF();
		maxIndex2 = 0;
		minIndex2 = 0;
		maxValue2 = sysData.trace[0].value[0];
		minValue2 = sysData.trace[0].value[0];
		for (int i = 1; i < sysData.sweep.sweepPoints; i++)
		{
			if (sysData.trace[0].value[i] > maxValue2)
			{
				maxValue2 = sysData.trace[0].value[i];
				maxIndex2 = i;
			} else if (sysData.trace[0].value[i] < minValue2)
			{
				minValue2 = sysData.trace[0].value[i];
				minIndex2 = i;
			}
		}
		
		printf("\n");
		if (qAbs(maxIndex1 - maxIndex2) <= 20 && qAbs(maxValue1 - maxValue2) <= 3.0 && qAbs(maxValue1 - minValue1) >= 20.0 && qAbs(maxValue2 - minValue2) >= 20.0)
		{
			calRunData.maxIndex = (maxIndex1 + maxIndex2) / 2;
			calRunData.minIndex = (minIndex1 + minIndex2) / 2;
			calRunData.maxValue = (maxValue1 + maxValue2) / 2;
			calRunData.minValue = (minValue1 + minValue2) / 2;
			__var(calRunData.maxIndex);
			//__var(calRunData.minIndex);
			__var(calRunData.maxValue);
			//__var(calRunData.minValue);
			printf("cal data get succeed!\n");

			return __SCPI_SUCCESS;
		}
	}
	__var(maxValue1);
	__var(maxValue2);
	__var(minValue1);
	__var(minValue2);
	__var(maxIndex1);
	__var(maxIndex2);
	__var(minIndex1);
	__var(minIndex2);
	printf("cal data get failed!\n");
	
	return __SCPI_FAILED;
}

//获取绝对幅度定标值
int tSysScpi::getCalibrateValueOfAbsoluteAmplitude(void)
{

}

//幅度校准
int tSysScpi::factoryCalibrationOfAmplitude(void)
{

}

//衰减器校准
int tSysScpi::factoryCalibrationOfAttenuation(void)
{

}

//中频增益校准
int tSysScpi::factoryCalibrationOfIF(void)
{

}

//分辨率校准
int tSysScpi::factoryCalibrationOfBw(void)
{

}

//频响校准
int tSysScpi::factoryCalibrationOfFreqResp(void)
{

}
//获取系统温度
int tSysScpi::getSystemTemperature(double* value)
{
	double tempValue = 0;

	for (int i = 0; i < 3; i++)
	{
		tempValue = sysData.temperature;
		if (tempValue > MINTEMPERATURE && tempValue < MAXTEMPERATURE)
		{
			*value = tempValue;
			return __SCPI_SUCCESS;
		}

		usleep(1000 * 2000);

		QCoreApplication::processEvents();
	}

	return __SCPI_FAILED;
}
//获取低频直采前置放大衰减器误差
double tSysScpi::getErrorOfAttenuationofLowfreqofPre(void)
{
	double attValue = sysData.ampt.att;
	double errValue = 0;

	if (attValue <= 9)
	{
		errValue = sysData.zcPreamplifierCalData.att0 + (sysData.zcPreamplifierCalData.att9 - sysData.zcPreamplifierCalData.att0) * (attValue - 0) / 9;
	} else if (attValue > 9 && attValue <= 21)
	{
		errValue = sysData.zcPreamplifierCalData.att9 + (sysData.zcPreamplifierCalData.att21 - sysData.zcPreamplifierCalData.att9) * (attValue - 9) / 12;
	} else if (attValue > 21 && attValue <= 30)
	{
		errValue = sysData.zcPreamplifierCalData.att21 + (sysData.zcPreamplifierCalData.att30 - sysData.zcPreamplifierCalData.att21) * (attValue - 21) / 9;
	} else if (attValue > 30 && attValue <= 39)
	{
		errValue = sysData.zcPreamplifierCalData.att30 + (sysData.zcPreamplifierCalData.att39 - sysData.zcPreamplifierCalData.att30) * (attValue - 30) / 9;
	} else if (attValue > 39 && attValue <= 45)
	{
		errValue = sysData.zcPreamplifierCalData.att39;
	}
	return errValue * -1;
}

//获取低频直采衰减器误差
double tSysScpi::getErrorOfAttenuationofLowfreq(void)
{
	double attValue = sysData.ampt.att;
	double errValue = 0;

	if (attValue <= 9)
	{
		errValue = sysData.zcCalData.att0 + (sysData.zcCalData.att9 - sysData.zcCalData.att0) * (attValue - 0) / 9;
	} else if (attValue > 9 && attValue <= 21)
	{
		errValue = sysData.zcCalData.att9 + (sysData.zcCalData.att21 - sysData.zcCalData.att9) * (attValue - 9) / 12;
	} else if (attValue > 21 && attValue <= 30)
	{
		errValue = sysData.zcCalData.att21 + (sysData.zcCalData.att30 - sysData.zcCalData.att21) * (attValue - 21) / 9;
	} else if (attValue > 30 && attValue <= 39)
	{
		errValue = sysData.zcCalData.att30 + (sysData.zcCalData.att39 - sysData.zcCalData.att30) * (attValue - 30) / 9;
	} else if (attValue > 39 && attValue <= 45)
	{
		errValue = sysData.zcCalData.att39;
	}
	return errValue * -1;
}
//获取前置放大衰减器误差
double tSysScpi::getErrorOfAttenuationofPre(void)
{
	double attValue = sysData.ampt.att;
	double errValue = 0;

	if (attValue <= 9)
	{
		errValue = sysData.preamplifierCalData.att0 + (sysData.preamplifierCalData.att9 - sysData.preamplifierCalData.att0) * (attValue - 0) / 9;
	} else if (attValue > 9 && attValue <= 21)
	{
		errValue = sysData.preamplifierCalData.att9 + (sysData.preamplifierCalData.att21 - sysData.preamplifierCalData.att9) * (attValue - 9) / 12;
	} else if (attValue > 21 && attValue <= 30)
	{
		errValue = sysData.preamplifierCalData.att21 + (sysData.preamplifierCalData.att30 - sysData.preamplifierCalData.att21) * (attValue - 21) / 9;
	} else if (attValue > 30 && attValue <= 39)
	{
		errValue = sysData.preamplifierCalData.att30 + (sysData.preamplifierCalData.att39 - sysData.preamplifierCalData.att30) * (attValue - 30) / 9;
	} else if (attValue > 39 && attValue <= 45)
	{
		errValue = sysData.preamplifierCalData.att39;
	}

	return errValue * -1;
}

//获取衰减器误差
double tSysScpi::getErrorOfAttenuation(void)
{
	double attValue = sysData.ampt.att;
	double errValue = 0;

	if (attValue <= 9)
	{
		errValue = sysData.factoryCalData.att0 + (sysData.factoryCalData.att9 - sysData.factoryCalData.att0) * (attValue - 0) / 9;
	} else if (attValue > 9 && attValue <= 21)
	{
		errValue = sysData.factoryCalData.att9 + (sysData.factoryCalData.att21 - sysData.factoryCalData.att9) * (attValue - 9) / 12;
	} else if (attValue > 21 && attValue <= 30)
	{
		errValue = sysData.factoryCalData.att21 + (sysData.factoryCalData.att30 - sysData.factoryCalData.att21) * (attValue - 21) / 9;
	} else if (attValue > 30 && attValue <= 39)
	{
		errValue = sysData.factoryCalData.att30 + (sysData.factoryCalData.att39 - sysData.factoryCalData.att30) * (attValue - 30) / 9;
	} else if (attValue > 39 && attValue <= 45)
	{
		errValue = sysData.factoryCalData.att39;
	}

	return errValue * -1;
}

//获取直采通道前置放大中频增益误差
double tSysScpi::getErrorOfIFofLowfreqofPre(void)
{
	double ifValue = sysData.ampt.ifValue;
	double errValue = 0;
	int ifZS = (int) ifValue / 10;
	int ifXS = (int) ifValue % 10;

	//四舍五入处理
	if (sysData.ampt.ifValue <= MAXIF)
	{
		if (ifXS >= 5)
		{
			ifZS++;
			ifXS = 0;
		}
	}

	if (ifZS == 0)
	{
		errValue = sysData.zcPreamplifierCalData.if0 + sysData.userCalData.if0dB;
	} else if (ifZS == 1)
	{
		errValue = sysData.zcPreamplifierCalData.if10 + sysData.userCalData.if10dB;
	} else if (ifZS == 2)
	{
		errValue = sysData.zcPreamplifierCalData.if20 + sysData.userCalData.if20dB;
	} else if (ifZS == 3)
	{
		errValue = sysData.zcPreamplifierCalData.if30 + sysData.userCalData.if30dB;
	} else
	{
		errValue = sysData.zcPreamplifierCalData.if30 + sysData.userCalData.if30dB;
	}
	return errValue + sysErrValue;
}

//获取前置放大中频增益误差
double tSysScpi::getErrorOfIFofPre(void)
{
	double ifValue = sysData.ampt.ifValue;
	double errValue = 0;
	int ifZS = (int) ifValue / 10;
	int ifXS = (int) ifValue % 10;

	//四舍五入处理
	if (sysData.ampt.ifValue <= MAXIF)
	{
		if (ifXS >= 5)
		{
			ifZS++;
			ifXS = 0;
		}
	}

	if (ifZS == 0)
	{
		errValue = sysData.preamplifierCalData.if0 + sysData.userCalData.if0dB;
	} else if (ifZS == 1)
	{
		errValue = sysData.preamplifierCalData.if10 + sysData.userCalData.if10dB;
	} else if (ifZS == 2)
	{
		errValue = sysData.preamplifierCalData.if20 + sysData.userCalData.if20dB;
	} else if (ifZS == 3)
	{
		errValue = sysData.preamplifierCalData.if30 + sysData.userCalData.if30dB;
	} else
	{
		errValue = sysData.preamplifierCalData.if30 + sysData.userCalData.if30dB;
	}

	return errValue + sysErrValue;
}

//获取直采通道中频增益误差
double tSysScpi::getErrorOfIFofLowfreq(void)
{
	double ifValue = sysData.ampt.ifValue;
	double errValue = 0;
	int ifZS = (int) ifValue / 10;
	int ifXS = (int) ifValue % 10;

	//四舍五入处理
	if (sysData.ampt.ifValue <= MAXIF)
	{
		if (ifXS >= 5)
		{
			ifZS++;
			ifXS = 0;
		}
	}

	if (ifZS == 0)
	{
		errValue = sysData.zcCalData.if0 + sysData.userCalData.if0dB;

	} else if (ifZS == 1)
	{
		errValue = sysData.zcCalData.if10 + sysData.userCalData.if10dB;
	} else if (ifZS == 2)
	{
		errValue = sysData.zcCalData.if20 + sysData.userCalData.if20dB;
	} else if (ifZS == 3)
	{
		errValue = sysData.zcCalData.if30 + sysData.userCalData.if30dB;
	} else
	{
		errValue = sysData.zcCalData.if30 + sysData.userCalData.if30dB;
	}
	return errValue + sysErrValue;
}

//获取中频增益误差
double tSysScpi::getErrorOfIF(void)
{
	int ifValue = (int)sysData.ampt.attIf;
	double errValue = 0;

	if(ifValue > MAXATT)
		ifValue = MAXATT;

	if(ifValue < MINATT)
		ifValue = MINATT;

	errValue = sysData.factoryCalData.attIf[ifValue];
	return errValue * -1 ;
}

//获取分辨率误差
double tSysScpi::getErrorOfRbw(void)
{
	if (sysData.options.isFftOn && isRequireFFTMeas())
	{
		return sysData.factoryCalData.rbw_fft[getRbwIndex()];
	} else
	{
		return sysData.factoryCalData.rbw[getRbwIndex()];
	}
}

//获取低频直采前置放大分辨率误差
double tSysScpi::getErrorOfRbwofLowfreqofPre(void)
{
	if (sysData.options.isFftOn && isRequireFFTMeas())
	{
		return sysData.zcPreamplifierCalData.rbw_fft[getRbwIndex()];
	} else
	{
		return sysData.zcPreamplifierCalData.rbw[getRbwIndex()];
	}
}

//获取低频直采分辨率误差
double tSysScpi::getErrorOfRbwofLowfreq(void)
{
	if (sysData.options.isFftOn && isRequireFFTMeas())
	{
		return sysData.zcCalData.rbw_fft[getRbwIndex()];
	} else
	{
		return sysData.zcCalData.rbw[getRbwIndex()];
	}
}

//获取前置放大分辨率误差
double tSysScpi::getErrorOfRbwofPre(void)
{
	if (sysData.options.isFftOn && isRequireFFTMeas())
	{
		return sysData.preamplifierCalData.rbw_fft[getRbwIndex()];
	} else
	{
		return sysData.preamplifierCalData.rbw[getRbwIndex()];
	}
}

//获取EMC分辨率误差
double tSysScpi::getErrorOfEmcRbw(void)
{
	int emcSize = sizeof EMC_BW / sizeof EMC_BW[0];

	for (int i = 0; i < emcSize; i++)
	{
		if ((unsigned long) sysData.bw.rbw == EMC_BW[i])
		{
			return EMC_ERROR[i];
		}
	}

	return 0;
}

//获取前放频响误差
double tSysScpi::getErrorOfFreqRespofPre(int index)
{
	int sBand = 0; //start band
	int eBand = 0; //stop band

	double errValue = 0;

	if (!sysData.isPreamplifierCalibrating)
	{
		sBand = rfData.Start_Band;
		eBand = rfData.Stop_Band;
		if (isRequireFFTMeas() && sysData.fft.bands[index].start_Band >= 0 && sysData.fft.bands[index].stop_Band >= 0)
		{
			sBand = sysData.fft.bands[index].start_Band;
			eBand = sysData.fft.bands[index].stop_Band;
		}
		int bdc = eBand - sBand; //band error
		int index1 = 0;
		int index2 = 0;
		int tIndex1 = 0;
		int tIndex2 = 0;
		int tableIndex1 = 0;
		int tableIndex2 = 0;
		double bc_freq_value = 0;
		double att1 = 0;
		double att2 = 0;
		double bd_att_percent = 0;
		double bd_x = 0;
		double bd_x1 = 0;
		double bd_x2 = 0;
		double bd_y = 0;
		double bd_y1 = 0;
		double bd_y2 = 0;
		double bc_value = 0;
		double P001 = 0;
		double P002 = 0;
		double P003 = 0;
		double P004 = 0;
		double P005 = 0;
		double P001_2 = 0;
		double P002_2 = 0;
		double P003_2 = 0;
		double P004_2 = 0;
		double P005_2 = 0;
		double freq = sysData.freq.start + index * sysData.span.span / (sysData.sweep.sweepPoints - 1) - sysData.freq.offset;

		if (sysData.ampt.att >= 0 && sysData.ampt.att < 9)
		{
			att1 = 0;
			att2 = 9;
			tableIndex1 = 0;
		} else if (sysData.ampt.att >= 9 && sysData.ampt.att < 21)
		{
			att1 = 9;
			att2 = 21;
			tableIndex1 = 1;
		} else if (sysData.ampt.att >= 21 && sysData.ampt.att < 30)
		{
			att1 = 21;
			att2 = 30;
			tableIndex1 = 2;
		} else if (sysData.ampt.att >= 30 && sysData.ampt.att <= 39)
		{
			att1 = 30;
			att2 = 39;
			tableIndex1 = 3;
		} else if (sysData.ampt.att > 39 && sysData.ampt.att <= 45)
		{
			att1 = 39;
			att2 = 45;
			tableIndex1 = 4;
		}

		tableIndex2 = tableIndex1 + 1;

		if (bdc == 0)
		{
			switch (sBand)
			{
				case 0:
					tIndex1 = 0;
					tIndex2 = 37;
					break;
				case 1:
					tIndex1 = 38;
					tIndex2 = 56;
					break;
				case 2:
					tIndex1 = 57;
					tIndex2 = 75;
					break;
				case 3:
					tIndex1 = 76;
					tIndex2 = 88;
					break;
			}

			for (int j = tIndex1; j < tIndex2; j++)
				if ((freq >= FREQRESPHZ[j]) && (freq <= FREQRESPHZ[j + 1]))
				{
					index1 = j;
					index2 = j + 1;
					break;
				}

			bc_freq_value = (freq - FREQRESPHZ[index1]) / (FREQRESPHZ[index2] - FREQRESPHZ[index1] + 1e-9);
			bd_att_percent = (sysData.ampt.att - att1) / (att2 - att1);
			bd_x1 = sysData.preamplifierCalData.freqResp[tableIndex1][index1];
			bd_y1 = sysData.preamplifierCalData.freqResp[tableIndex1][index2];
			bd_x2 = sysData.preamplifierCalData.freqResp[tableIndex2][index1];
			bd_y2 = sysData.preamplifierCalData.freqResp[tableIndex2][index2];
			bd_x = bd_x1 + (bd_x2 - bd_x1) * bd_att_percent;
			bd_y = bd_y1 + (bd_y2 - bd_y1) * bd_att_percent;
			bc_value = bd_x + (bd_y - bd_x) * bc_freq_value;

			if ((freq >= 9e3) && (freq <= 3e6))
			{
				P001 = sysData.preamplifierCalData.freqResp[tableIndex1][0];
				P002 = sysData.preamplifierCalData.freqResp[tableIndex1][1];
				P003 = sysData.preamplifierCalData.freqResp[tableIndex1][2];
				P004 = sysData.preamplifierCalData.freqResp[tableIndex1][3];
				P005 = sysData.preamplifierCalData.freqResp[tableIndex1][4];
				P001_2 = sysData.preamplifierCalData.freqResp[tableIndex2][0];
				P002_2 = sysData.preamplifierCalData.freqResp[tableIndex2][1];
				P003_2 = sysData.preamplifierCalData.freqResp[tableIndex2][2];
				P004_2 = sysData.preamplifierCalData.freqResp[tableIndex2][3];
				P005_2 = sysData.preamplifierCalData.freqResp[tableIndex2][4];

				if ((freq >= 9e3) && (freq <= 20e3))
				{
					P001 = P001 + (P001_2 - P001) * bd_att_percent;
					P002 = P002 + (P002_2 - P002) * bd_att_percent;
					bc_value = qPow((20e3 - freq) / 11e3, 1.2) * (P001 - P002) + P002;
				} else if ((freq > 20e3) && (freq <= 120e3))
				{
					P002 = P002 + (P002_2 - P002) * bd_att_percent;
					P003 = P003 + (P003_2 - P003) * bd_att_percent;
					bc_value = qPow((120e3 - freq) / 100e3, 2.0) * (P002 - P003) + P003;
				} else if ((freq > 120e3) && (freq <= 500e3))
				{
					P003 = P003 + (P003_2 - P003) * bd_att_percent;
					P004 = P004 + (P004_2 - P004) * bd_att_percent;
					bc_value = qPow((500e3 - freq) / 380e3, 3.0) * (P003 - P004) + P004;
				} else if ((freq > 500e3) && (freq <= 3e6))
				{
					P004 = P004 + (P004_2 - P004) * bd_att_percent;
					P005 = P005 + (P005_2 - P005) * bd_att_percent;
					bc_value = qPow((3e6 - freq) / 2.5e6, 2.0) * (P004 - P005) + P005;
				}
			}

			errValue = bc_value;
		} else{
			tIndex1 = freqIndex[index].tIndex1;
			tIndex2 = freqIndex[index].tIndex2;

			for (int j = tIndex1; j <= tIndex2; j++)
				if ((freq >= FREQRESPHZ[j]) && (freq <= FREQRESPHZ[j + 1]))
				{
					index1 = j;
					index2 = j + 1;
					break;
				}

			bc_freq_value = (freq - FREQRESPHZ[index1]) / (FREQRESPHZ[index2] - FREQRESPHZ[index1] + 1e-9);
			bd_att_percent = (sysData.ampt.att - att1) / (att2 - att1);
			bd_x1 = sysData.preamplifierCalData.freqResp[tableIndex1][index1];
			bd_y1 = sysData.preamplifierCalData.freqResp[tableIndex1][index2];
			bd_x2 = sysData.preamplifierCalData.freqResp[tableIndex2][index1];
			bd_y2 = sysData.preamplifierCalData.freqResp[tableIndex2][index2];
			bd_x = bd_x1 + (bd_x2 - bd_x1) * bd_att_percent;
			bd_y = bd_y1 + (bd_y2 - bd_y1) * bd_att_percent;
			bc_value = bd_x + (bd_y - bd_x) * bc_freq_value;

			if ((freq >= 9e3) && (freq <= 3e6))
			{
				P001 = sysData.preamplifierCalData.freqResp[tableIndex1][0];
				P002 = sysData.preamplifierCalData.freqResp[tableIndex1][1];
				P003 = sysData.preamplifierCalData.freqResp[tableIndex1][2];
				P004 = sysData.preamplifierCalData.freqResp[tableIndex1][3];
				P005 = sysData.preamplifierCalData.freqResp[tableIndex1][4];
				P001_2 = sysData.preamplifierCalData.freqResp[tableIndex2][0];
				P002_2 = sysData.preamplifierCalData.freqResp[tableIndex2][1];
				P003_2 = sysData.preamplifierCalData.freqResp[tableIndex2][2];
				P004_2 = sysData.preamplifierCalData.freqResp[tableIndex2][3];
				P005_2 = sysData.preamplifierCalData.freqResp[tableIndex2][4];

				if ((freq >= 9e3) && (freq <= 20e3))
				{
					P001 = P001 + (P001_2 - P001) * bd_att_percent;
					P002 = P002 + (P002_2 - P002) * bd_att_percent;
					bc_value = qPow((20e3 - freq) / 11e3, 1.2) * (P001 - P002) + P002;
				} else if ((freq > 20e3) && (freq <= 120e3))
				{
					P002 = P002 + (P002_2 - P002) * bd_att_percent;
					P003 = P003 + (P003_2 - P003) * bd_att_percent;
					bc_value = qPow((120e3 - freq) / 100e3, 2.0) * (P002 - P003) + P003;
				} else if ((freq > 120e3) && (freq <= 500e3))
				{
					P003 = P003 + (P003_2 - P003) * bd_att_percent;
					P004 = P004 + (P004_2 - P004) * bd_att_percent;
					bc_value = qPow((500e3 - freq) / 380e3, 3.0) * (P003 - P004) + P004;
				} else if ((freq > 500e3) && (freq <= 3e6))
				{
					P004 = P004 + (P004_2 - P004) * bd_att_percent;
					P005 = P005 + (P005_2 - P005) * bd_att_percent;
					bc_value = qPow((3e6 - freq) / 2.5e6, 2.0) * (P004 - P005) + P005;
				}
			}

			errValue = bc_value;
		}
	}

	//FFT频响补偿
	double fftResp = 0;

	//FFT频响应拟合
	//  if (!sysData.isPreamplifierCalibrating && sysData.options.isFftOn && isRequireFFTMeas())
	//  {
	//	int midIndex = 0;
	//	int startIndex = 0;
	//	int stopIndex = 0;
	//	double percentValue = 0;
	//
	//	for (int i = 0; i < sysData.fft.fftCount; i++)
	//	{
	//	  if (index >= sysData.fft.fftList[i].sIndex && index <= sysData.fft.fftList[i].eIndex)
	//	  {
	//		startIndex = sysData.fft.fftList[i].sIndex;
	//		stopIndex = sysData.fft.fftList[i].eIndex;
	//        midIndex = startIndex + (stopIndex - startIndex) / 2;
	//
	//        if (index <= midIndex)
	//        {
	//          percentValue = (midIndex - index) * 1.0 / (midIndex - startIndex) * 4.0 * qAbs(sysData.fft.fftList[i].cF1) / 0.25;
	//        } else if (index > midIndex)
	//        {
	//       	  percentValue = (index - midIndex) * 1.0 / (stopIndex - midIndex) * 4.0 * qAbs(sysData.fft.fftList[i].cF2) / 0.25;
	//        }
	//
	//        fftResp = percentValue;
	//
	//		break;
	//	  }
	//	}
	//  }

	return errValue + fftResp;
}

//获取直采通道前置放大频响误差
double tSysScpi::getErrorOfLowFreqRespofPre(int index)
{

	double errValue = 0;

	if (!sysData.isZcPreamplifierCalibrating)
	{
		int bdc = rfData.Stop_Band - rfData.Start_Band;
		int index1 = 0;
		int index2 = 0;
		int tIndex1 = 0;
		int tIndex2 = 0;
		int tableIndex1 = 0;
		int tableIndex2 = 0;
		double bc_freq_value = 0;
		double att1 = 0;
		double att2 = 0;
		double bd_att_percent = 0;
		double bd_x = 0;
		double bd_x1 = 0;
		double bd_x2 = 0;
		double bd_y = 0;
		double bd_y1 = 0;
		double bd_y2 = 0;
		double bc_value = 0;
		//    double P001 = 0;
		//    double P002 = 0;
		//    double P003 = 0;
		//    double P004 = 0;
		//    double P005 = 0;
		//    double P001_2 = 0;
		//    double P002_2 = 0;
		//    double P003_2 = 0;
		//    double P004_2 = 0;
		//    double P005_2 = 0;
		double freq = sysData.freq.start + index * sysData.span.span / (sysData.sweep.sweepPoints - 1) - sysData.freq.offset;

		if (sysData.ampt.att >= 0 && sysData.ampt.att < 9)
		{
			att1 = 0;
			att2 = 9;
			tableIndex1 = 0;
		} else if (sysData.ampt.att >= 9 && sysData.ampt.att < 21)
		{
			att1 = 9;
			att2 = 21;
			tableIndex1 = 1;
		} else if (sysData.ampt.att >= 21 && sysData.ampt.att < 30)
		{
			att1 = 21;
			att2 = 30;
			tableIndex1 = 2;
		} else if (sysData.ampt.att >= 30 && sysData.ampt.att <= 39)
		{
			att1 = 30;
			att2 = 39;
			tableIndex1 = 3;
		} else if (sysData.ampt.att > 39 && sysData.ampt.att <= 45)
		{
			att1 = 39;
			att2 = 45;
			tableIndex1 = 4;
		}

		tableIndex2 = tableIndex1 + 1;
		
		tIndex1 = 0;
		tIndex2 = FREQRESPCOUNT_ZC - 1;

		for (int j = tIndex1; j <= tIndex2; j++)
		{
			if ((freq >= FREQRESPHZ_ZC[j]) && (freq <= FREQRESPHZ_ZC[j + 1]))
			{
				index1 = j;
				index2 = j + 1;
				break;
			}
		}
		bc_freq_value = (freq - FREQRESPHZ_ZC[index1]) / (FREQRESPHZ_ZC[index2] - FREQRESPHZ_ZC[index1] + 1e-9);
		bd_att_percent = (sysData.ampt.att - att1) / (att2 - att1);
		bd_x1 = sysData.zcPreamplifierCalData.freqResp[tableIndex1][index1];
		bd_y1 = sysData.zcPreamplifierCalData.freqResp[tableIndex1][index2];
		bd_x2 = sysData.zcPreamplifierCalData.freqResp[tableIndex2][index1];
		bd_y2 = sysData.zcPreamplifierCalData.freqResp[tableIndex2][index2];
		bd_x = bd_x1 + (bd_x2 - bd_x1) * bd_att_percent;
		bd_y = bd_y1 + (bd_y2 - bd_y1) * bd_att_percent;
		bc_value = bd_x + (bd_y - bd_x) * bc_freq_value;

		errValue = bc_value;
		

	}

	return errValue;
}
//获取直采频响误差
double tSysScpi::getErrorOfLowFreqResp(int index)
{
	double errValue = 0;

	if (!sysData.isZcCalibrating)
	{
		int bdc = rfData.Stop_Band - rfData.Start_Band;
		int index1 = 0;
		int index2 = 0;
		int tIndex1 = 0;
		int tIndex2 = 0;
		int tableIndex = 0;
		double bc_freq_value = 0;
		double attrf = 0;
		double attif = 0;
		double bd_att_percent = 0;
		double bd_x = 0;
		double bd_x1 = 0;
		double bd_x2 = 0;
		double bd_y = 0;
		double bd_y1 = 0;
		double bd_y2 = 0;
		double bc_value = 0;
		//    double P001 = 0;
		//    double P002 = 0;
		//    double P003 = 0;
		//    double P004 = 0;
		//    double P005 = 0;
		//    double P001_2 = 0;
		//    double P002_2 = 0;
		//    double P003_2 = 0;
		//    double P004_2 = 0;
		//    double P005_2 = 0;
		double freq = sysData.freq.start + index * sysData.span.span / (sysData.sweep.sweepPoints - 1) - sysData.freq.offset;

		//
		if (sysData.ampt.attRf < 10 && sysData.ampt.attIf < 10)
		{
			attrf = 0;
			attif = 0;
			tableIndex = 10;
		} else if (sysData.ampt.attRf < 10 && sysData.ampt.attIf < 20)
		{
			attrf = 0;
			attif = 10;
			tableIndex = 6;
		} else if (sysData.ampt.attRf < 10 && sysData.ampt.attIf < 30)
		{
			attrf = 0;
			attif = 20;
			tableIndex = 2;
		} else if (sysData.ampt.attRf < 10 && sysData.ampt.attIf >= 30)
		{
			attrf = 0;
			attif = 30;
			tableIndex = 14;
		} else if (sysData.ampt.attRf < 20 && sysData.ampt.attIf < 10)
		{
			attrf = 10;
			attif = 0;
			tableIndex = 9;
		} else if (sysData.ampt.attRf < 20 && sysData.ampt.attIf < 20)
		{
			attrf = 10;
			attif = 10;
			tableIndex = 5;
		} else if (sysData.ampt.attRf < 20 && sysData.ampt.attIf < 30)
		{
			attrf = 10;
			attif = 20;
			tableIndex = 1;
		} else if (sysData.ampt.attRf < 20 && sysData.ampt.attIf >= 30)
		{
			attrf = 10;
			attif = 30;
			tableIndex = 13;
		} else if (sysData.ampt.attRf < 30 && sysData.ampt.attIf < 10)
		{
			attrf = 20;
			attif = 0;
			tableIndex = 11;
		} else if (sysData.ampt.attRf < 30 && sysData.ampt.attIf < 20)
		{
			attrf = 20;
			attif = 10;
			tableIndex = 7;
		} else if (sysData.ampt.attRf < 30 && sysData.ampt.attIf < 30)
		{
			attrf = 20;
			attif = 20;
			tableIndex = 3;
		} else if (sysData.ampt.attRf < 30 && sysData.ampt.attIf >= 30)
		{
			attrf = 20;
			attif = 30;
			tableIndex = 15;
		} else if (sysData.ampt.attRf >= 30 && sysData.ampt.attIf < 10)
		{
			attrf = 30;
			attif = 0;
			tableIndex = 12;
		} else if (sysData.ampt.attRf >= 30 && sysData.ampt.attIf < 20)
		{
			attrf = 30;
			attif = 10;
			tableIndex = 8;
		} else if (sysData.ampt.attRf >= 30 && sysData.ampt.attIf < 30)
		{
			attrf = 30;
			attif = 20;
			tableIndex = 4;
		}else if (sysData.ampt.attRf >= 30 && sysData.ampt.attIf >= 30)
		{
			attrf = 30;
			attif = 30;
			tableIndex = 15;
		}
		
		tIndex1 = 0;
		tIndex2 = FREQRESPCOUNT_ZC - 1;

		for (int j = tIndex1; j <= tIndex2; j++)
		{
			if ((freq >= FREQRESPHZ_ZC[j]) && (freq <= FREQRESPHZ_ZC[j + 1]))
			{
				index1 = j;
				index2 = j + 1;
				break;
			}
		}
		bc_freq_value = (freq - FREQRESPHZ_ZC[index1]) / (FREQRESPHZ_ZC[index2] - FREQRESPHZ_ZC[index1] + 1e-9);
		bd_x1 = sysData.zcCalData.freqResp[tableIndex][index1];
		bd_x2 = sysData.zcCalData.freqResp[tableIndex][index1];
		bd_x = bd_x1 + (bd_x2 - bd_x1) * bc_freq_value;
		bc_value = bd_x;

		errValue = bc_value;
	

	}

	return -1*errValue;
}

//获取频响误差
double tSysScpi::getErrorOfFreqResp(int index)
{
	int sBand = 0; //start band
	int eBand = 0; //end band
	double errValue = 0;

	if (!sysData.isFactoryCalibrating)
	{
		sBand = rfData.Start_Band;
		eBand = rfData.Stop_Band;
		if (isRequireFFTMeas() && sysData.fft.bands[index].start_Band >= 0 && sysData.fft.bands[index].stop_Band >= 0)
		{
			sBand = sysData.fft.bands[index].start_Band;
			eBand = sysData.fft.bands[index].stop_Band;
		}

		int bdc = eBand - sBand; //band error
		int index1 = 0;
		int index2 = 0;
		int tIndex1 = 0;
		int tIndex2 = 0;
		int tableIndex1 = 0;
		int tableIndex2 = 0;
		double bc_freq_value = 0;
		double att1 = 0;
		double att2 = 0;
		double bd_att_percent = 0;
		double bd_x = 0;
		double bd_x1 = 0;
		double bd_x2 = 0;
		double bd_y = 0;
		double bd_y1 = 0;
		double bd_y2 = 0;
		double bc_value = 0;
		double P001 = 0;
		double P002 = 0;
		double P003 = 0;
		double P004 = 0;
		double P005 = 0;
		double P001_2 = 0;
		double P002_2 = 0;
		double P003_2 = 0;
		double P004_2 = 0;
		double P005_2 = 0;
		double freq = sysData.freq.start + index * sysData.span.span / (sysData.sweep.sweepPoints - 1) - sysData.freq.offset;
//		if (sysData.ampt.att >= 0 && sysData.ampt.att < 9)
//		{
//			att1 = 0;
//			att2 = 9;
//			tableIndex1 = 0;
//		} else if (sysData.ampt.att >= 9 && sysData.ampt.att < 21)
//		{
//			att1 = 9;
//			att2 = 21;
//			tableIndex1 = 1;
//		} else if (sysData.ampt.att >= 21 && sysData.ampt.att < 30)
//		{
//			att1 = 21;
//			att2 = 30;
//			tableIndex1 = 2;
//		} else if (sysData.ampt.att >= 30 && sysData.ampt.att <= 39)
//		{
//			att1 = 30;
//			att2 = 39;
//			tableIndex1 = 3;
//		} else if (sysData.ampt.att > 39 && sysData.ampt.att <= 45)
//		{
//			att1 = 39;
//			att2 = 45;
//			tableIndex1 = 4;
//		}
		if (sysData.ampt.attRf >= 0 && sysData.ampt.attRf < 10)
		{
			att1 = 0;
			att2 = 10;
			tableIndex1 = 0;
		} else if (sysData.ampt.attRf >= 10 && sysData.ampt.attRf < 20)
		{
			att1 = 10;
			att2 = 20;
			tableIndex1 = 1;
		} else if (sysData.ampt.attRf >= 20 && sysData.ampt.attRf <=31)
		{
			att1 = 20;
			att2 = 31;
			tableIndex1 = 2;
		}

		tableIndex2 = tableIndex1 + 1;
//		if (bdc == 0)
//		{
//			switch (sBand)
//			{
//				case 0:
//					tIndex1 = 0;
//					tIndex2 = 37;
//					break;
//				case 1:
//					tIndex1 = 38;
//					tIndex2 = 56;
//					break;
//				case 2:
//					tIndex1 = 57;
//					tIndex2 = 75;
//					break;
//				case 3:
//					tIndex1 = 76;
//					tIndex2 = 88;
//					break;
//			}
//
//			for (int j = tIndex1; j < tIndex2; j++)
//				if ((freq >= FREQRESPHZ[j]) && (freq <= FREQRESPHZ[j + 1]))
//				{
//					index1 = j;
//					index2 = j + 1;
//					break;
//				}
//
//			bc_freq_value = (freq - FREQRESPHZ[index1]) / (FREQRESPHZ[index2] - FREQRESPHZ[index1] + 1e-9);
//			bd_att_percent = (sysData.ampt.att - att1) / (att2 - att1);
//			bd_x1 = sysData.factoryCalData.freqResp[tableIndex1][index1];
//			bd_y1 = sysData.factoryCalData.freqResp[tableIndex1][index2];
//			bd_x2 = sysData.factoryCalData.freqResp[tableIndex2][index1];
//			bd_y2 = sysData.factoryCalData.freqResp[tableIndex2][index2];
//			bd_x = bd_x1 + (bd_x2 - bd_x1) * bd_att_percent;
//			bd_y = bd_y1 + (bd_y2 - bd_y1) * bd_att_percent;
//			bc_value = bd_x + (bd_y - bd_x) * bc_freq_value;
//
//			if ((freq >= 9e3) && (freq <= 3e6))
//			{
//				P001 = sysData.factoryCalData.freqResp[tableIndex1][0];
//				P002 = sysData.factoryCalData.freqResp[tableIndex1][1];
//				P003 = sysData.factoryCalData.freqResp[tableIndex1][2];
//				P004 = sysData.factoryCalData.freqResp[tableIndex1][3];
//				P005 = sysData.factoryCalData.freqResp[tableIndex1][4];
//				P001_2 = sysData.factoryCalData.freqResp[tableIndex2][0];
//				P002_2 = sysData.factoryCalData.freqResp[tableIndex2][1];
//				P003_2 = sysData.factoryCalData.freqResp[tableIndex2][2];
//				P004_2 = sysData.factoryCalData.freqResp[tableIndex2][3];
//				P005_2 = sysData.factoryCalData.freqResp[tableIndex2][4];
//
//				if ((freq >= 9e3) && (freq <= 20e3))
//				{
//					P001 = P001 + (P001_2 - P001) * bd_att_percent;
//					P002 = P002 + (P002_2 - P002) * bd_att_percent;
//					bc_value = qPow((20e3 - freq) / 11e3, 1.2) * (P001 - P002) + P002;
//				} else if ((freq > 20e3) && (freq <= 120e3))
//				{
//					P002 = P002 + (P002_2 - P002) * bd_att_percent;
//					P003 = P003 + (P003_2 - P003) * bd_att_percent;
//					bc_value = qPow((120e3 - freq) / 100e3, 2.0) * (P002 - P003) + P003;
//				} else if ((freq > 120e3) && (freq <= 500e3))
//				{
//					P003 = P003 + (P003_2 - P003) * bd_att_percent;
//					P004 = P004 + (P004_2 - P004) * bd_att_percent;
//					bc_value = qPow((500e3 - freq) / 380e3, 3.0) * (P003 - P004) + P004;
//				} else if ((freq > 500e3) && (freq <= 3e6))
//				{
//					P004 = P004 + (P004_2 - P004) * bd_att_percent;
//					P005 = P005 + (P005_2 - P005) * bd_att_percent;
//					bc_value = qPow((3e6 - freq) / 2.5e6, 2.0) * (P004 - P005) + P005;
//				}
//			}
//
//			errValue = bc_value;
//		} else
		{
			int x1 = 0, x2 = 0;
			if(freq <= FREQ_START_HH_BAND1)
			{
				x1 = 0;
				x2 = 161;
			} else if(freq <= FREQ_START_HH_BAND2)
			{
				x1 = 162;
				x2 = 214;
			} else if(freq <= FREQ_START_HH_BAND3)
			{
				x1 = 215;
				x2 = 267;
			} else if(freq <= FREQ_START_HH_BAND4)
			{
				x1 = 268;
				x2 = 320;
			} else
			{
				x1 = 321;
				x2 = 372;
			}
			for(int j = x1; j < x2; j++)
			{
				if((freq >= FREQRESPHZ[j]) && (freq <= FREQRESPHZ[j + 1]))
				{
					index1 = j;
					index2 = j + 1;

					break;
				}
			}
			bc_freq_value = (freq - FREQRESPHZ[index1]) / (FREQRESPHZ[index2] - FREQRESPHZ[index1] + 1e-9);
			bd_att_percent = (sysData.ampt.attRf - att1) / (att2 - att1);
			bd_x1 = sysData.factoryCalData.freqResp[tableIndex1][index1];
			bd_y1 = sysData.factoryCalData.freqResp[tableIndex1][index2];
			bd_x2 = sysData.factoryCalData.freqResp[tableIndex2][index1];
			bd_y2 = sysData.factoryCalData.freqResp[tableIndex2][index2];
			bd_x = bd_x1 + (bd_x2 - bd_x1) * bd_att_percent;
			bd_y = bd_y1 + (bd_y2 - bd_y1) * bd_att_percent;
			bc_value = bd_x + (bd_y - bd_x) * bc_freq_value;

			errValue = bc_value;
		}

	}

	return -1 * errValue;
}

//获取FFT计算误差
double tSysScpi::getErrorOffftMeasureCalculate(double value, unsigned long long index, unsigned long long startIndex, unsigned long long stopIndex)
{
	unsigned long long tempStartIndex = startIndex;
	unsigned long long tempStopIndex = stopIndex;
	unsigned long long midIndex = 0;
	double percentValue = 0;
	double temp = 20 * log10(value);

	midIndex = startIndex + (stopIndex - startIndex) / 2;

	if (index <= midIndex)
	{
		percentValue = (midIndex - index) * 1.0 / (midIndex - startIndex) * 4.0 * qAbs(sysData.fft.cF1) / 0.25;
	} else if (index > midIndex)
	{
		percentValue = (index - midIndex) * 1.0 / (stopIndex - midIndex) * 4.0 * qAbs(sysData.fft.cF2) / 0.25;
	}

	double result = 0;

	result = (qPow(10, (temp + percentValue) / 20));
	return result;
}

//获取温度误差
double tSysScpi::getErrorOfTemperature(void)
{
	double wdValue = sysData.temperature;
	double errValue = 0;

	if (((!sysData.isPreamplifierCalibrating && sysData.preamplifierCalData.isCaled) || (!sysData.isZcCalibrating && sysData.zcCalData.isCaled) || (!sysData.isZcPreamplifierCalibrating && sysData.zcPreamplifierCalData.isCaled) || (!sysData.isFactoryCalibrating && sysData.factoryCalData.isCaled)) && wdValue > MINTEMPERATURE && wdValue < MAXTEMPERATURE)
	{
		if (sysData.freq.isLowChannel)
		{
			if (sysData.ampt.isPreamptOn)
			{
				errValue = (wdValue - sysData.zcPreamplifierCalData.temperature) * 0.04;
			} else
			{
				errValue = (wdValue - sysData.zcCalData.temperature) * 0.04;
			}
		} else
		{
			if (sysData.ampt.isPreamptOn)
			{
				errValue = (wdValue - sysData.preamplifierCalData.temperature) * 0.04;
			} else
			{
				errValue = (wdValue - sysData.factoryCalData.temperature) * 0.04;
			}
		}

		if (!sysData.isUserCalibrating && sysData.userCalData.temperature > MINTEMPERATURE && sysData.userCalData.temperature < MAXTEMPERATURE)
		{
			errValue = (wdValue - sysData.userCalData.temperature) * 0.04;
		}
	}

	return errValue;
}

//获取网络测量误差
double tSysScpi::getErrorOfNetworkMeasure(int index)
{
	if (sysData.source.isTrackGenNetworkOn)
	{
		return sysData.source.trackNetworkStandard.value[index];
	}

	return 0;
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

//保存屏幕
void tSysScpi::saveScreen(void)
{
	pixBackground = QPixmap::grabWindow(qApp->desktop()->winId(), 0, 0, SCREENWIDTH, SCREENHEIGHT);
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
	int sep = 20;
	int nums = 1;
	char tempChar[32] = { };
	QPainter p;
	QPixmap pix(rcCal.size());

	QRect rcTitle;
	QRect rcValue;

	if ((pix.height() - 2 * sep) <= 10)
	{
		sep = 2;
	}

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

	if ((pix.height() - 2 * sep) <= 10)
	{
		sep = 4;
	}

	QRect rcTitle;
	QRect rcProgress;

	if (sep <= 4)
	{
		rcTitle = QRect(sep, sep, 140, pix.height() - 2 * sep);
		rcProgress = QRect(rcTitle.left() + rcTitle.width() + sep, sep * 2, pix.width() - 2 * sep - rcTitle.left() - rcTitle.width() - 4, pix.height() - 4 * sep);
	} else
	{
		rcTitle = QRect(sep, sep, pix.width() - 2 * sep, (pix.height() - 2 * sep) / 2);
		rcProgress = QRect(rcTitle.left(), rcTitle.top() + rcTitle.height(), rcTitle.width(), 20);
	}

	//  QRect rcTitle = QRect(sep, sep, pix.width() - 2 * sep, (pix.height() - 2 * sep) / 2);
	//  QRect rcProgress;
	//  if (sep <= 4)
	//  {
	//    rcProgress = QRect(rcTitle.left(), rcTitle.top() + rcTitle.height() + sep, rcTitle.width(), 10);
	//  } else
	//  {
	//    rcProgress = QRect(rcTitle.left(), rcTitle.top() + rcTitle.height(), rcTitle.width(), 20);
	//  }

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
	if (sep <= 4)
	{
		p.drawText(rcTitle, Qt::AlignCenter, title);
	} else
	{
		p.drawText(rcTitle, Qt::AlignLeft | Qt::AlignVCenter, title);
	}
	p.fillRect(rcProgress, Qt::white);
	p.fillRect(rcPosition, Qt::green);
	p.setPen(Qt::black);
	p.drawRect(rcProgress);
	p.drawText(rcProgress, Qt::AlignCenter, QString(intToString((int) progress, tempChar)).trimmed() + "%");
	p.end();

	QScreen::instance()->blit(pix.toImage(), QPoint(rcCal.left(), rcCal.top()), rcCal);
}

double tSysScpi::dBmTomV(double value)
{
	return qSqrt(50000 * qPow(10, 0.1 * value));
}

//获取AM结果
bool tSysScpi::getAmResult(void)
{
	if (sysData.options.peakSize <= 3)
	{
		return false;
	}

	int pIndex = 0;
	int lIndex = 0;
	int rIndex = 0;
	double peak = 0;
	double lPeak = 0;
	double rPeak = 0;

	pIndex = 0;
	peak = sysData.options.peakList[0].value;

	for (int i = 1; i < sysData.options.peakSize; i++)
	{
		if (sysData.options.peakList[i].value > peak)
		{
			pIndex = i;
			peak = sysData.options.peakList[i].value;
		}
	}

	lIndex = 0;
	lPeak = sysData.options.peakList[0].value;

	for (int i = 1; i < pIndex; i++)
	{
		if (sysData.options.peakList[i].value > lPeak)
		{
			lIndex = i;
			lPeak = sysData.options.peakList[i].value;
		}
	}

	rIndex = pIndex + 1;
	rPeak = sysData.options.peakList[rIndex].value;

	for (int i = pIndex + 2; i < sysData.options.peakSize; i++)
	{
		if (sysData.options.peakList[i].value > rPeak)
		{
			rIndex = i;
			rPeak = sysData.options.peakList[i].value;
		}
	}

	if (pIndex >= 0 && lIndex >= 0 && rIndex >= 0 && pIndex < sysData.options.peakSize && lIndex < sysData.options.peakSize && rIndex < sysData.options.peakSize)
	{
		pIndex = sysData.options.peakList[pIndex].index;
		peak = sysData.options.fftData[pIndex];
		lIndex = sysData.options.peakList[lIndex].index;
		lPeak = sysData.options.fftData[lIndex];
		rIndex = sysData.options.peakList[rIndex].index;
		rPeak = sysData.options.fftData[rIndex];

		//freq
		if (qAbs(qAbs(pIndex - lIndex) - qAbs(rIndex - pIndex)) <= 100 && lIndex > 0 && rIndex < DEMODFFTSIZE - 1)
		{
			sysData.options.am_freq = qAbs(rIndex - lIndex) / 2.0 * IFFS / sysData.options.cicValue / DEMODFFTSIZE;

			/*
			 firDemod(sysData.options.mData, sysData.options.mDataFir, DEMODFFTSIZE, DEMODFIRN, DEMODFIRH);

			 double avgValue = 0;
			 double ts = 1.0 / sysData.options.am_ifbw;
			 double tz[DEMODFFTSIZE] = {0};
			 int n = DEMODFFTSIZE;
			 int j = 0;
			 double mod_rate = 0;

			 for (int i = 0; i < n; i++)
			 {
			 avgValue += sysData.options.mDataFir[i] / n;
			 }

			 for (int i = 0; i < n; i++)
			 {
			 sysData.options.mDataFir[i] -= avgValue;
			 }

			 for (int i = 0, j = 0; i < n-1; i++)
			 {
			 if (sysData.options.mDataFir[i] > 0 && sysData.options.mDataFir[i + 1] <= 0)
			 {
			 tz[j++] = (sysData.options.mDataFir[i + 1] * ts * i + sysData.options.mDataFir[i] * ts * (i + 1)) / (sysData.options.mDataFir[i] + sysData.options.mDataFir[i + 1]);
			 }
			 }

			 for (int i = 1, mod_rate = 0; i < j; i++)
			 {
			 mod_rate += 1.0 / ((tz[i] - tz[i-1]) * (j - 1));
			 }
			 */
		} else
		{
			return false;
		}

		//depth
		double maxValue = 0;
		double minValue = 0;
		double tempValueOfdepth = 0;

		maxValue = sysData.options.mData[0];
		minValue = sysData.options.mData[0];

		for (int i = 1; i < DEMODFFTSIZE; i++)
		{
			if (sysData.options.mData[i] > maxValue)
			{
				maxValue = sysData.options.mData[i];
			}

			if (sysData.options.mData[i] < minValue)
			{
				minValue = sysData.options.mData[i];
			}
		}

		if (qAbs(maxValue + minValue) > 0)
		{
			tempValueOfdepth = qAbs((maxValue - minValue) / (maxValue + minValue));
		} else
		{
			tempValueOfdepth = 0;
		}

		//当调制率大于等于10kHz时候，调制深度进行补偿
		if (sysData.options.am_freq >= 10e3)
		{
			tempValueOfdepth = tempValueOfdepth * (1 + sysData.options.am_freq / (400e3));
		}

		sysData.options.am_depth = tempValueOfdepth;
		//sysData.options.am_depth = (sysData.options.fftDataLine[rIndex] + sysData.options.fftDataLine[lIndex]) / sysData.options.fftDataLine[pIndex];

		if (sysData.options.am_depth < 0.05)
		{
			return false;
		}

		//carr power
		/*if (maxValue > 0)
		 {
		 sysData.options.am_carrierPower = -10.0 + (10 * log10(0.5 * pow(sysData.options.fftDataLine[pIndex], 2)) - 182.0);
		 }*/
	}

	return true;
}

//获取FM结果
bool tSysScpi::getFmResult(void)
{
	if (sysData.options.peakSize <= 3)
	{
		return false;
	}

	int pIndex = 0;
	int lIndex = 0;
	int rIndex = 0;

	QVector<struct demodPeakDataRec> peakList;
	peakList.clear();

	for (int i = 0; i < sysData.options.peakSize; i++)
	{
		if (sysData.options.peakList[i].index == sysData.options.fm_cIndex)
		{
			pIndex = i;
			peakList << sysData.options.peakList[i];
			break;
		}
	}

	if (pIndex <= 0)
	{
		return false;
	}

	int lIndexError = 0;
	int rIndexError = 0;

	for (int i = pIndex - 1; i >= 0; i--)
	{
		lIndex = sysData.options.peakList[i].index;
		lIndexError = qAbs(lIndex - sysData.options.fm_cIndex);

		for (int j = pIndex + 1; j < sysData.options.peakSize; j++)
		{
			rIndex = sysData.options.peakList[j].index;
			rIndexError = qAbs(rIndex - sysData.options.fm_cIndex);

			if (lIndexError == rIndexError)
			{
				peakList.push_front(sysData.options.peakList[i]);
				peakList.push_back(sysData.options.peakList[j]);
				break;
			}
		}
	}

	if (peakList.size() <= 2)
	{
		return false;
	}

	int cIndex = (peakList.size() - 1) / 2;

	//fm freq
	//sysData.options.fm_freq = qAbs(peakList.at(cIndex + 1).index - peakList.at(cIndex - 1).index) * 1.0 / 2.0 * sysData.options.fm_ifbw / DEMODFFTSIZE;
	int undemodSize = DEMODFFTSIZE - 1;
	int zDataSize = 0;
	int pDataSize = 0;
	double zData[DEMODFFTSIZE] = { 0 };
	double pData[DEMODFFTSIZE] = { 0 };
	double avgFreq = 0;

	for (int i = 0; i < undemodSize; i++)
	{
		if (sysData.options.mData[i] > 0 && sysData.options.mData[i + 1] <= 0)
		{
			zData[zDataSize++] = (sysData.options.mData[i + 1] * 1.0 / sysData.options.fm_ifbw * i + sysData.options.mData[i] * 1.0 / sysData.options.fm_ifbw * (i + 1)) / (sysData.options.mData[i] + sysData.options.mData[i + 1]);
		}
	}

	for (int i = 1; i < zDataSize; i++)
	{
		pData[pDataSize++] = 1.0 / (zData[i] - zData[i - 1]);
	}

	for (int i = 0; i < pDataSize; i++)
	{
		avgFreq += pData[i] / pDataSize;
	}

	sysData.options.fm_freq = qAbs(avgFreq);

	//fm deviation
	double datas[4] = { 0 };
	double tData[DEMODFFTSIZE] = { 0 };
	double maxValue = 0;
	double minValue = 0;
	double powValue = 0;

	for (int i = 0; i < DEMODFFTSIZE - 1; i++)
	{
		for (int j = 0; j < sizeof datas / sizeof datas[0]; j++)
		{
			datas[j] = sysData.options.iqData[(i + 1) * 2 + j];

			if (datas[j] >= 0x800000)
			{
				datas[j] -= 0x1000000;
			}
		}

		powValue = pow(datas[0], 2) + pow(datas[1], 2);

		if (powValue <= 0)
		{
			return false;
		}

		tData[i] = (datas[0] * datas[3] - datas[1] * datas[2]) / powValue;

		if (i == 0)
		{
			maxValue = tData[i];
			minValue = tData[i];
		} else
		{
			if (tData[i] > maxValue)
			{
				maxValue = tData[i];
			}

			if (tData[i] < minValue)
			{
				minValue = tData[i];
			}
		}
	}

	sysData.options.fm_deviation = qAbs(maxValue - minValue) / 2.0 * sysData.options.fm_ifbw / (2 * PI);

	//carr power
	/*if (sysData.options.fm_cIndex > 0 && sysData.options.fm_cIndex < DEMODFFTSIZE)
	 {
	 sysData.options.fm_carrierPower = -1.5 + (10 * log10(0.5 * pow(sysData.options.fftDataLine[sysData.options.fm_cIndex], 2)) - 182.0);
	 }*/

	return true;
}

//获取SINAD和THD
int tSysScpi::getSinadAndThd(double *unmodul_idata, double *unmodul_qdata, char lra, double fs, int *harm_num, double *sinad, double *thd, double *snr, double *enob, double *modul_freq)
{
	int i = 0, j = 0, k = 0;
	char error = 0;
	double dout_dB[DEMODFFTSIZE];
	double spectp[DEMODFFTSIZE];

	double temp = 0;
	double span = 0, //调制信号带宽
			spanh = 0, //谐波信号带宽（无调制信号）
			maxdB = 0; //最大幅值
	int Fin = 0; //最大幅值对应的频率

	double Fh_L[512], //1-10次谐波峰值点频率（左半边）
			lim_L[512][2], //1-10次谐波起始、终止频率
			Fh_R[512], //1-10次谐波峰值点频率（右半边）
			lim_R[512][2]; //1-10次谐波起始、终止频率

	int size_dis_points_L = 0, //计算谐波频率点数量（左半边）
			size_dis_points_R = 0, //计算谐波频率点数量（右半边）
			dis_points_L[DEMODFFTSIZE / 2], //记录所有谐波频率点（左半边）
			dis_points_R[DEMODFFTSIZE / 2]; //记录所有谐波频率点（右半边）

	double Psignal = 0, //全部频谱的调制信号总功率
			Psignal_L = 0, //左半边频谱的调制信号总功率
			Psignal_R = 0, //右半边频谱的调制信号总功率
			Ptotal = 0, //全部频谱的所有信号总功率
			Ptotal_L = 0, //左半边频谱的所有信号总功率
			Ptotal_R = 0, //右半边频谱的所有信号总功率
			Pharmonic = 0, //全部频谱的谐波信号总功率
			Pharmonic_L = 0, //左半边频谱的谐波信号总功率
			Pharmonic_R = 0, //右半边频谱的谐波信号总功率
			Pnoise = 0; //噪声信号总功率

	if ((*harm_num > 512) || (*harm_num < 2) || (lra < -1) || (lra > 1))
	{
		error = -1; //参数错误
		return error;
	}

	for (i = 0; i < DEMODFFTSIZE; i++)
	{
		//求模
		unmodul_idata[i] = qSqrt(unmodul_idata[i] * unmodul_idata[i] + unmodul_qdata[i] * unmodul_qdata[i]);

		//求平方
		spectp[i] = unmodul_idata[i] * unmodul_idata[i];

		//求对数
		dout_dB[i] = 20 * log10(unmodul_idata[i] + 1e-9);
	}

	//########################计算最大峰值（调制信号）频率########################
	for (i = 1; i < (DEMODFFTSIZE / 2); i++)
	{
		if (maxdB < dout_dB[i])
		{
			maxdB = dout_dB[i]; //寻找最大功率值
			Fin = i; //寻找最大功率值对应的频率 即调制率
		}
	}

	//注意：此处带宽选定依据还没搞清楚。
	span = DEMODFFTSIZE / 2000; //确定调制信号数据带宽
	spanh = DEMODFFTSIZE / 2000; //确定除调制信号外的谐波信号数据带宽

	//###################################计算参数##################################
	//--------------------（1）计算左半边频率范围内的频率与功率--------------------
	//if ((lra == -1) || (lra == 0))
	{
		//计算1-10次谐波信号的中心频率
		for (i = 1; i <= *harm_num; i++)
		{
			temp = (float) (i * Fin) / DEMODFFTSIZE;

			if (temp > 0.5)
			{
				error = -2; //谐波越界
				break;
			}

			Fh_L[i - 1] = temp;
		}

		*harm_num = i - 1; //实际计算的谐波次数

		//计算调制信号边带频率
		lim_L[0][0] = Fh_L[0] * DEMODFFTSIZE - span;
		if (lim_L[0][0] < 1)
			lim_L[0][0] = 1;
		lim_L[0][1] = Fh_L[0] * DEMODFFTSIZE + span;

		//计算2以上次数的谐波边带频率
		for (i = 1; i < *harm_num; i++)
		{
			lim_L[i][0] = Fh_L[i] * DEMODFFTSIZE - spanh + 1;

			if (lim_L[i][0] < 1)
			{
				lim_L[i][0] = 1;
			}

			lim_L[i][1] = Fh_L[i] * DEMODFFTSIZE + spanh + 1;
		}

		//将1以上次数的谐波带宽内的所有点频率，按照1-10次谐波的顺序排列，即分离谐波点频率。
		for (i = 0, k = 0; i < *harm_num; i++)
		{
			for (j = lim_L[i][0]; j <= lim_L[i][1]; j++, k++)
			{
				dis_points_L[k] = j;
			}
		}

		//计算谐波谐波点数
		size_dis_points_L = k;

		//计算调制信号带宽内的总功率
		for (i = lim_L[0][0], Psignal_L = 0; i <= lim_L[0][1]; i++)
		{
			Psignal_L += spectp[i];
		}

		//计算1以上次数的谐波信号带宽内的总功率
		for (i = 0, Pharmonic_L = 0; i < size_dis_points_L; i++)
		{
			Pharmonic_L += spectp[dis_points_L[i]];
		}

		//计算左半边频率范围内的信号总功率
		for (i = 1, Ptotal_L = 0; i < DEMODFFTSIZE / 2; i++)
		{
			Ptotal_L += spectp[i];
		}
	}

	//--------------------（2）计算右半边频率范围内的频率与功率--------------------
	//if ((lra == 1) || (lra == 0))
	{
		//计算1-10次谐波信号的中心频率
		for (i = 1; i <= *harm_num; i++)
		{
			temp = (float) (DEMODFFTSIZE - (i * (Fin))) / DEMODFFTSIZE;

			if (temp < 0.5)
			{
				error = -2; //谐波越界
				break;
			}

			Fh_R[i - 1] = temp;
		}

		*harm_num = i - 1; //实际计算的谐波次数

		//计算调制信号边带频率
		lim_R[0][0] = Fh_R[0] * DEMODFFTSIZE - span;

		if (lim_R[0][0] < 1)
		{
			lim_R[0][0] = 1;
		}

		lim_R[0][1] = Fh_R[0] * DEMODFFTSIZE + span;

		//计算2以上次数的谐波边带频率
		for (i = 1; i < *harm_num; i++)
		{
			lim_R[i][0] = Fh_R[i] * DEMODFFTSIZE - spanh - 1;

			if (lim_R[i][0] < 1)
			{
				lim_R[i][0] = 1;
			}

			lim_R[i][1] = Fh_R[i] * DEMODFFTSIZE + spanh - 1;
		}

		//将1以上次数的谐波带宽内的所有点频率，按照1-10次谐波的顺序排列，即分离谐波点频率。
		for (i = 0, k = 0; i < *harm_num; i++)
		{
			for (j = lim_R[i][0]; j <= lim_R[i][1]; j++, k++)
			{
				dis_points_R[k] = j;
			}
		}

		//计算谐波谐波点数
		size_dis_points_R = k;

		//计算调制信号带宽内的总功率
		for (i = lim_R[0][0], Psignal_R = 0; i <= lim_R[0][1]; i++)
		{
			Psignal_R += spectp[i];
		}

		//计算1-10次谐波信号带宽内的总功率
		for (i = 0, Pharmonic_R = 0; i < size_dis_points_R; i++)
		{
			Pharmonic_R += spectp[dis_points_R[i]];
		}

		//计算右半边频率范围内的信号总功率
		for (i = DEMODFFTSIZE / 2, Ptotal_R = 0; i < DEMODFFTSIZE; i++)
		{
			Ptotal_R += spectp[i];
		}
	}

	//------------------------（3）根据选择计算最终的功率-------------------------
	switch (lra)
	{
		case -1:
			Ptotal = Ptotal_L;
			Psignal = Psignal_L;
			Pharmonic = Pharmonic_L;
			break;

		case 0:
			Ptotal = Ptotal_R;
			Psignal = Psignal_R;
			Pharmonic = Pharmonic_R;
			break;

		case 1:
			Ptotal = Ptotal_L + Ptotal_R;
			Psignal = Psignal_L + Psignal_R;
			Pharmonic = Pharmonic_L + Pharmonic_R;
			break;

		default:
			break;
	}

	if (sysData.options.fmOn)
	{
		sysData.options.fm_carrierPower = -20.0 + (10 * log10(Psignal + 1e-9) - 178.0);
	} else
	{
		sysData.options.am_carrierPower = -20.0 + (10 * log10(Psignal + 1e-9) - 172.0);
	}

	//计算2以上次数的谐波信号总功率
	Pharmonic -= Psignal;

	//计算噪声信号总功率
	Pnoise = Ptotal - Psignal - Pharmonic;

	//----------（4）计算信纳德SINAD、总谐波失真度THD、信噪比SNR、有效位数ENOB-----------
	//此处使用10*log10，而不是20*log10。原因与之前spectp的计算有关。根据原理，spectp的值应当是和方根，而此程序spectp的值为平方和，没有进行开方，所以此处为10*log10，减少了运算步骤，提高了效率。

	//计算信纳德SINAD
	*sinad = 10 * log10(Psignal / (Pnoise + Pharmonic));

	//计算总谐波失真度THD
	*thd = 10 * log10(Psignal / Pharmonic);

	//计算信噪比SNR
	*snr = 10 * log10(Psignal / Pnoise);

	//计算有效位数ENOB
	*enob = (*sinad - 1.76) / 6.02;

	//调制率
	*modul_freq = Fin * fs / DEMODFFTSIZE;

	return (error);
}

//FIR滤波
void tSysScpi::firDemod(double xin[], double xout[], int length, int n, double *h)
{
	int i, j;
	double sum;

	double x1[4 * DEMODFFTSIZE];

	for (i = 0; i < length; i++)
	{
		x1[n + i - 1] = xin[i];
	}

	for (i = 0; i < length; i++)
	{
		sum = 0.0;

		for (j = 0; j < n; j++)
		{
			sum += h[j] * x1[i - j + n - 1];
		}

		xout[i] = sum;
	}

	for (i = 0; i < (n - 1); i++)
	{
		x1[n - i - 2] = xin[length - 1 - i];
	}
}

//获取峰值
void tSysScpi::getDemodPeakList(void)
{
	getDemodDiff(sysData.options.fftData, sysData.options.diff, DEMODFFTSIZE);

	sysData.options.peakSize = 0;

	double nValue = 0;
	double lValue = 0;
	double rValue = 0;

	for (int i = 0; i < DEMODFFTSIZE; i++)
	{
		if (sysData.options.diff[i] < 0)
		{
			if (i > 0 && sysData.options.diff[i - 1] < 0)
			{
				continue;
			}

			nValue = sysData.options.fftData[i];

			if (i == 0)
			{
				rValue = nValue;

				for (int j = i + 1; j < DEMODFFTSIZE; j++)
				{
					if (sysData.options.diff[j] > 0)
					{
						rValue = sysData.options.fftData[j];

						if (qAbs(nValue - rValue) >= DEMODPEAKDB)
						{
							sysData.options.peakSize++;
							sysData.options.peakList[sysData.options.peakSize - 1].index = i;
							sysData.options.peakList[sysData.options.peakSize - 1].value = sysData.options.fftData[i];
						}

						break;
					}
				}
			} else if (i == DEMODFFTSIZE - 1)
			{
				lValue = nValue;

				for (int j = i - 1; j >= 0; j--)
				{
					if (sysData.options.diff[j] < 0 && (j + 1) < i)
					{
						lValue = sysData.options.fftData[j + 1];

						if (qAbs(nValue - lValue) >= DEMODPEAKDB)
						{
							sysData.options.peakSize++;
							sysData.options.peakList[sysData.options.peakSize - 1].index = i;
							sysData.options.peakList[sysData.options.peakSize - 1].value = sysData.options.fftData[i];
						}
					}
				}
			} else
			{
				lValue = nValue;
				rValue = nValue;

				for (int j = i - 1; j >= 0; j--)
				{
					if (sysData.options.diff[j] < 0 && (j + 1) < i)
					{
						lValue = sysData.options.fftData[j + 1];
						break;
					}
				}

				for (int j = i + 1; j < DEMODFFTSIZE; j++)
				{
					if (sysData.options.diff[j] > 0)
					{
						rValue = sysData.options.fftData[j];
						break;
					}
				}

				if (qAbs(nValue - lValue) >= DEMODPEAKDB && qAbs(nValue - rValue) >= DEMODPEAKDB)
				{
					sysData.options.peakSize++;
					sysData.options.peakList[sysData.options.peakSize - 1].index = i;
					sysData.options.peakList[sysData.options.peakSize - 1].value = sysData.options.fftData[i];
				}
			}
		}
	}
}

void tSysScpi::getDemodDiff(double *data, double *diff, int len)
{
	if (len > 0 && len <= DEMODFFTSIZE)
	{
		for (int i = 0; i < len - 1; i++)
		{
			diff[i] = data[i + 1] - data[i];
		}

		diff[len - 1] = 0 - data[len - 1];
	}
}

//解调成功与否
bool tSysScpi::isDemodOk(unsigned long *iqData, double *mData, int n)
{
	if (n < (DEMODFFTSIZE * 2 - 4))
	{
		return false;
	}

	double iData[DEMODFFTSIZE] = { 0 };
	double qData[DEMODFFTSIZE] = { 0 };
	double mData1[DEMODFFTSIZE] = { 0 };
	double mData2[DEMODFFTSIZE] = { 0 };
	double fData[DEMODFFTSIZE] = { 0 };
	int len = DEMODFFTSIZE;
	double tempValue = 0;

	for (int i = 0; i < DEMODFFTSIZE; i++)
	{
		iData[i] = iqData[(i + 1) * 2 + 0] * 1.0;
		qData[i] = iqData[(i + 1) * 2 + 1] * 1.0;

		if (iData[i] >= 0x800000)
		{
			iData[i] -= 0x1000000;
		}

		if (qData[i] >= 0x800000)
		{
			qData[i] -= 0x1000000;
		}
	}

	mutexDemod.lock();

	for (int i = 0; i < DEMODFFTSIZE - 1; i++)
	{
		mData1[i] = (iData[i] * qData[i + 1] - iData[i + 1] * qData[i]) / (pow(iData[i], 2) + pow(qData[i], 2));
	}

	firDemod(mData1, mData, DEMODFFTSIZE - 1, DEMODFIRN, DEMODFIRH);
	mutexDemod.unlock();
	fft(iData, qData, DEMODFFTSIZE, 1);

	for (int i = 0; i < DEMODFFTSIZE; i++)
	{
		fData[i] = qSqrt(qPow(iData[i], 2) + qPow(qData[i], 2));
	}

	for (int i = 0; i < DEMODFFTSIZE / 2; i++)
	{
		tempValue = fData[i];
		fData[i] = fData[i + DEMODFFTSIZE / 2];
		fData[i + DEMODFFTSIZE / 2] = tempValue;
	}

	//-----get sinad and thd
	int harm_num = 25;
	double snr = 0;
	double enob = 0;
	double modul_freq = 0;
	getSinadAndThd(iData, qData, 0, sysData.options.fm_ifbw, &harm_num, &sysData.options.fm_sinad, &sysData.options.fm_thd, &snr, &enob, &modul_freq);
	//----------------------

	getDemodDiff(fData, sysData.options.diff, DEMODFFTSIZE);

	int cIndex = 0;
	int lIndex = 0;
	int rIndex = 0;
	double cValue = 0;
	double lValue = 0;
	double rValue = 0;

	cIndex = 0;
	cValue = fData[cIndex];

	for (int i = 1; i < DEMODFFTSIZE; i++)
	{
		if (fData[i] > cValue)
		{
			cValue = fData[i];
			cIndex = i;
		}
	}

	for (int i = cIndex - 1; i >= 0; i--)
	{
		if (sysData.options.diff[i] < 0)
		{
			lIndex = i + 1;
			lValue = fData[lIndex];
			break;
		}
	}

	for (int i = cIndex + 1; i < DEMODFFTSIZE; i++)
	{
		if (sysData.options.diff[i] > 0)
		{
			rIndex = i;
			rValue = fData[rIndex];
			break;
		}
	}

	cValue = 20 * log10(fData[cIndex] + 1e-9);
	lValue = 20 * log10(fData[lIndex] + 1e-9);
	rValue = 20 * log10(fData[rIndex] + 1e-9);

	for (int i = 0; i < DEMODFFTSIZE; i++)
	{
		sysData.options.fftDataLine[i] = fData[i];
		sysData.options.fftData[i] = 20 * log10(fData[i] + 1e-9);
	}

	sysData.options.fm_cIndex = cIndex;

	return (cIndex > 0 && lIndex > 0 && rIndex > 0 && (qAbs(cValue - lValue) > 30.0 || qAbs(cValue - rValue) > 30.0));
}

//解调成功与否
bool tSysScpi::isDemodOkOfAm(unsigned long *iqData, double *mData, int n)
{
	if (n < (DEMODFFTSIZE * 2 - 4))
	{
		return false;
	}

	double iData[DEMODFFTSIZE] = { 0 };
	double qData[DEMODFFTSIZE] = { 0 };
	double fData[DEMODFFTSIZE] = { 0 };
	int len = DEMODFFTSIZE;
	double tempValue = 0;

	for (int i = 0; i < DEMODFFTSIZE; i++)
	{
		iData[i] = iqData[(i + 1) * 2 + 0] * 1.0;
		qData[i] = iqData[(i + 1) * 2 + 1] * 1.0;

		if (iData[i] >= 0x800000)
		{
			iData[i] -= 0x1000000;
		}

		if (qData[i] >= 0x800000)
		{
			qData[i] -= 0x1000000;
		}

		mData[i] = qSqrt(qPow(iData[i], 2) + qPow(qData[i], 2));
	}

	fft(iData, qData, DEMODFFTSIZE, 1);

	for (int i = 0; i < DEMODFFTSIZE; i++)
	{
		fData[i] = qSqrt(qPow(iData[i], 2) + qPow(qData[i], 2));
	}

	for (int i = 0; i < DEMODFFTSIZE / 2; i++)
	{
		tempValue = fData[i];
		fData[i] = fData[i + DEMODFFTSIZE / 2];
		fData[i + DEMODFFTSIZE / 2] = tempValue;
	}

	//-----get sinad and thd
	int harm_num = 25;
	double snr = 0;
	double enob = 0;
	double modul_freq = 0;
	getSinadAndThd(iData, qData, 0, sysData.options.am_ifbw, &harm_num, &sysData.options.am_sinad, &sysData.options.am_thd, &snr, &enob, &modul_freq);
	//----------------------

	int maxIndex = 0;
	int minIndex = 0;
	double maxValue = fData[0];
	double minValue = fData[0];

	for (int i = 1; i < DEMODFFTSIZE; i++)
	{
		if (fData[i] > maxValue)
		{
			maxIndex = i;
			maxValue = fData[i];
		}

		if (fData[i] < minValue)
		{
			minIndex = i;
			minValue = fData[i];
		}
	}

	if (maxValue <= 0 || minValue <= 0)
	{
		return false;
	}

	maxValue = 20 * log10(maxValue);
	minValue = 20 * log10(minValue);

	for (int i = 0; i < DEMODFFTSIZE; i++)
	{
		sysData.options.fftDataLine[i] = fData[i];
		sysData.options.fftData[i] = 20 * log10(fData[i] + 1e-9);
	}

	return (qAbs(maxValue - minValue) >= 50 && qAbs(maxIndex - DEMODFFTSIZE / 2) <= 100);
}

//FM解调成功与否
bool tSysScpi::isDemodOkOfFm(unsigned long *iqData, double *mData, int n)
{
	if (n < (DEMODFFTSIZE * 2 - 4))
	{
		return false;
	}

	double iData[DEMODFFTSIZE] = { 0 };
	double qData[DEMODFFTSIZE] = { 0 };
	double fData[DEMODFFTSIZE] = { 0 };
	int len = DEMODFFTSIZE;
	double tempValue = 0;

	for (int i = 0; i < DEMODFFTSIZE; i++)
	{
		iData[i] = iqData[(i + 1) * 2 + 0] * 1.0;
		qData[i] = iqData[(i + 1) * 2 + 1] * 1.0;

		if (iData[i] >= 0x800000)
		{
			iData[i] -= 0x1000000;
		}

		if (qData[i] >= 0x800000)
		{
			qData[i] -= 0x1000000;
		}

		mData[i] = qSqrt(qPow(iData[i], 2) + qPow(qData[i], 2));
	}

	fft(iData, qData, DEMODFFTSIZE, 1);

	for (int i = 0; i < DEMODFFTSIZE; i++)
	{
		fData[i] = qSqrt(qPow(iData[i], 2) + qPow(qData[i], 2));
	}

	for (int i = 0; i < DEMODFFTSIZE / 2; i++)
	{
		tempValue = fData[i];
		fData[i] = fData[i + DEMODFFTSIZE / 2];
		fData[i + DEMODFFTSIZE / 2] = tempValue;
	}

	int maxIndex = 0;
	int minIndex = 0;
	double maxValue = fData[0];
	double minValue = fData[0];

	for (int i = 1; i < DEMODFFTSIZE; i++)
	{
		if (fData[i] > maxValue)
		{
			maxIndex = i;
			maxValue = fData[i];
		}

		if (fData[i] < minValue)
		{
			minIndex = i;
			minValue = fData[i];
		}
	}

	if (maxValue <= 0 || minValue <= 0)
	{
		return false;
	}

	maxValue = 20 * log10(maxValue);
	minValue = 20 * log10(minValue);

	for (int i = 0; i < DEMODFFTSIZE; i++)
	{
		sysData.options.fftDataLine[i] = fData[i];
		sysData.options.fftData[i] = 20 * log10(fData[i] + 1e-9);
	}

	return (qAbs(maxValue - minValue) >= 50 && qAbs(maxIndex - DEMODFFTSIZE / 2) <= 10);
}

void tSysScpi::getDemodDataOfAm(unsigned long *iqData, double *depth, double *freq, double fs, int n)
{
	if (n <= 1 || n > DEMODSIZE)
	{
		return;
	}

	int len = n / 2;
	double tData[DEMODSIZE / 2] = { 0 };
	double zData[DEMODSIZE / 2] = { 0 };
	double iData = 0;
	double qData = 0;
	double maxValue = 0;
	double minValue = 0;
	double avgValue = 0;

	for (int i = 0; i < len; i++)
	{
		iData = iqData[i * 2 + 0];
		qData = iqData[i * 2 + 1];

		if (iData >= 0x800000)
		{
			iData -= 0x1000000;
		}
		if (qData >= 0x800000)
		{
			qData -= 0x1000000;
		}

		sysData.options.fDataBuf[i] = qSqrt(qPow(iData, 2) + qPow(qData, 2));
		tData[i] = sysData.options.fDataBuf[i] / len;

		if (i <= 1)
		{
			maxValue = tData[i];
			minValue = tData[i];
		} else
		{
			if (tData[i] > maxValue)
			{
				maxValue = tData[i];
			}

			if (tData[i] < minValue)
			{
				minValue = tData[i];
			}
		}

		if (i > 0 && i < len - 1)
		{
			avgValue += tData[i] / len;
		}
	}

	//cal depth
	if ((maxValue + minValue) > 0)
	{
		*depth = (maxValue - minValue) / (maxValue + minValue);
	}

	//remove dc
	tData[0] = 0;
	for (int i = 1; i < len; i++)
	{
		tData[i] -= avgValue;
	}

	//cal rate
	//double ifFs = 1.0 / (30e6 / RAMCIC_NOFFT[getRbwIndex()]);
	double ifFs = 1.0 / (30e6 / sysData.options.cicValue);
	int zeroCount = 0;
	double tRate = 0;

	for (int i = 1; i < len - 2; i++)
	{
		if (tData[i] > 0 && tData[i + 1] <= 0)
		{
			if (qAbs(tData[i] + tData[i + 1]) != 0)
			{
				zData[zeroCount++] = (tData[i + 1] * ifFs * i + tData[i] * ifFs * (i + 1)) / (tData[i] + tData[i + 1]);
			}
		}
	}

	if (zeroCount > 1)
	{
		for (int i = 1; i < zeroCount; i++)
		{
			if (qAbs(zData[i] - zData[i - 1]) > 0)
			{
				tRate += 1.0 / ((zData[i] - zData[i - 1]) * (zeroCount - 1));
			}
		}

		*freq = tRate;
	}
}

void tSysScpi::getDemodDataOfFm(unsigned long *iqData, double *offset, double *freq, double fs, int n)
{
	if (n <= 3 || n > DEMODSIZE)
	{
		return;
	}

	int len = n / 2 - 1;
	double tData[DEMODSIZE / 2] = { 0 };
	double zData[DEMODSIZE / 2] = { 0 };
	double mFreq[DEMODSIZE / 2] = { 0 };
	double datas[4] = { 0 };
	double maxValue = 0;
	double minValue = 0;
	double avgValue = 0;

	for (int i = 0; i < len; i++)
	{
		for (int j = 0; j < sizeof datas / sizeof datas[0]; j++)
		{
			datas[j] = iqData[i * 2 + j];

			if (datas[j] >= 0x800000)
			{
				datas[j] -= 0x1000000;
			}
		}

		tData[i] = datas[0] * datas[3] - datas[1] * datas[2];

		if (i == 0)
		{
			maxValue = tData[i];
			minValue = tData[i];
		} else
		{
			if (tData[i] > maxValue)
			{
				maxValue = tData[i];
			}

			if (tData[i] < minValue)
			{
				minValue = tData[i];
			}
		}

		avgValue += tData[i] / (len - 1);
	}

	*offset = (maxValue - minValue) / 2.0;
	tData[0] = 0;

	for (int i = 1; i < len; i++)
	{
		tData[i] -= avgValue;
	}

	double ifFs = 1.0 / (30e6 / RAMCIC_NOFFT[getRbwIndex()]);
	int zeroCount = 0;

	for (int i = 0; i < len - 1; i++)
	{
		if (tData[i] > 0 && tData[i + 1] <= 0)
		{
			zData[zeroCount++] = (tData[i + 1] * ifFs * i + tData[i] * ifFs * (i + 1)) / (tData[i] + tData[i + 1]);
		}
	}

	mFreq[0] = 1.0 / (zData[1] - zData[0]);
	for (int i = 1; i < zeroCount - 1; i++)
	{
		mFreq[i] = 1.0 / (zData[i + 1] - zData[i]);
	}

	int j = 0;
	for (int i = 0; i < zeroCount - 1; i++)
	{
		if (mFreq[i] >= 0)
		{
			mFreq[j++] = mFreq[i];
		}
	}

	double tempFreq = 0;

	for (int i = 0; i < j; i++)
	{
		tempFreq += mFreq[i] / j;
	}

	*freq = tempFreq;
}

void tSysScpi::fft(double *x, double *y, int n, int sign)
{
	int i, j, k, l, m, n1, n2;
	double c, c1, e, s, s1, t, tr, ti;

	for (j = 1, i = 1; i < 16; i++)
	{
		m = i;
		j = 2 * j;

		if (j == n)
		{
			break;
		}
	}

	n1 = n - 1;

	for (j = 0, i = 0; i < n1; i++)
	{
		if (i < j)
		{
			tr = x[j];
			ti = y[j];
			x[j] = x[i];
			y[j] = y[i];
			x[i] = tr;
			y[i] = ti;
		}

		k = n / 2;

		while (k < (j + 1))
		{
			j = j - k;
			k = k / 2;
		}

		j = j + k;
	}

	n1 = 1;

	for (l = 1; l <= m; l++)
	{
		n1 = 2 * n1;
		n2 = n1 / 2;
		//e = 3.14159265359 / n2;
		e = 3.1415926 / n2;
		c = 1.0;
		s = 0.0;
		c1 = cos(e);
		s1 = -sign * sin(e);

		for (j = 0; j < n2; j++)
		{
			for (i = j; i < n; i += n1)
			{
				k = i + n2;
				tr = c * x[k] - s * y[k];
				ti = c * y[k] + s * x[k];
				x[k] = x[i] - tr;
				y[k] = y[i] - ti;
				x[i] = x[i] + tr;
				y[i] = y[i] + ti;
			}

			t = c;
			c = c * c1 - s * s1;
			s = t * s1 + s * c1;
		}
	}

	if (sign == -1)
	{
		for (i = 0; i < n; i++)
		{
			x[i] /= n;
			y[i] /= n;
		}
	}
}

char tSysScpi::calIfAttValueOfFrontend(double value)
{
	char temp = 0, channel = 0, refSelect = 0;

//	sysData.fe.ifAtt = value;

	//if(sysData.fe.freq >= 20e6 && sysData.fe.freq <= FREQ_START_HH_BAND1)
		//channel = 0;//20MHz~8GHz
	//else
		//channel = 1;//8GHz~18GHz

	int att =(int)value;
	temp = (~att) & 0x1f;
//	temp = ((att & 0x01) << 4) | ((att >> 1) & 0x01) << 3 | ((att >> 2) & 0x01) << 2 | ((att >> 3) & 0x01) << 1 | (att >> 4) & 0x01;
//	temp = (31 - att) & 0x1f;
//	refSelect = sysData.freq.refInternal;

	return temp;
}

//内外参考设置
int tSysScpi::setFrontendRefSelect(int value)
{
	if(value < 0)
		return __SCPI_FAILED;

//	if(value == 0)
//	  sysData.freq.refInternal = true;
//	else
//	  sysData.freq.refInternal = false;
	//char temp = calIfAttValueOfFrontend();
	sysData.freq.refInternal = value;
	feDownload(0xfff7,sysData.freq.refInternal);

	return __SCPI_SUCCESS;
}

int tSysScpi::setFrontendRefSelect(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	int tempValue = value.toInt();

	return setFrontendRefSelect(tempValue);

}

//内参考调谐电压设置
int tSysScpi::setFrontendRefVolt(double value)
{
	if(value < 0)
		return __SCPI_FAILED;
	sysData.fe.refVolt = value;

	feDownload(0xfff6, sysData.fe.refVolt);

	return __SCPI_SUCCESS;
}

int tSysScpi::setFrontendRefVolt(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	double  tempValue = value.toDouble();

	return setFrontendRefVolt(tempValue);

}

int tSysScpi::getPremptData(int val)
{
	double freq = sysData.freq.start;
	char band = 1, channel = 0, temp = 0,  band_low = 0, refsel = 0;
	char premptOn = 0, premptOn_low = 0, premptOn_high = 0;
	if(sysData.freq.refInternal)
		refsel = 0;//内参考
	else
		refsel = 1;//外参考
	
	if(freq <= 20e6)
		band_low = 1;//短波通道
	else
		band_low = 0;//变频通道
	if(freq <= FREQ_START_HH_BAND1 && freq > 20e6)
		band = 1;//变频低波段通道
	else
		band = 0;//变频高波段通道

	if(freq > FREQ_START_HH_BAND1 && freq <= FREQ_START_HH_BAND2)
		channel = 0;//高波段1
	else if(freq > FREQ_START_HH_BAND2 && freq <= FREQ_START_HH_BAND3)
		channel = 1;//高波段2
	else if(freq > FREQ_START_HH_BAND3 && freq <= FREQ_START_HH_BAND4)
		channel = 2;//高波段3
	else if(freq > FREQ_START_HH_BAND4 && freq <= MAXFREQ)
		channel = 3;//高波段4

	if(sysData.ampt.isPreamptOn)
	{
		premptOn = 0;//前置放大打开
		premptOn_low = 0;//低波段常规放大器打开
		premptOn_high = 0;//高波段常规放大器打开
	}
	else
	{
		premptOn = 1;
		premptOn_low = 1;
		premptOn_high = 1;//高波段常规放大器关闭
	}
	
	premptOn = 1;//closed

#if 0	
	if(freq > FREQ_START_HH_BAND1)
		premptOn_high = 0;//高波段常规放大器打开
	else
		premptOn_high = 1;//高波段常规放大器关闭

	premptOn_high = 0;//高波段常规放大器打开		
#endif

	temp = refsel << 0 | band_low << 1 | band << 2 | channel << 3 | premptOn << 5 \
					   | premptOn_low << 6 | premptOn_high << 7;

	//printf("to 237 is %02x\n",temp);
	return temp;

}

//射频前端前置放大设置
int tSysScpi::rfPreamptDown(int value)
{
	sysData.ampt.isPreamptOn = (value == 1);
	int temp = getPremptData(sysData.ampt.isPreamptOn);

	feDownload(0xfff4, temp);
	return __SCPI_SUCCESS;
}

int tSysScpi::rfPreamptDown(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	//int tempValue = getBoolValue(value.toUpper().trimmed(), &isOk);
    int tempValue = value.toInt();

	return rfPreamptDown(tempValue);

}

//中频衰减设置
int tSysScpi::setFrontEndIfAtt(double value)
{
	if(value < 0)
		return __SCPI_FAILED;

//	sysData.fe.ifAtt = value;
	sysData.ampt.attIf = value;
	int attif = (int)value;
	char temp = (~attif) & 0x1f;
	feDownload(0xfff5, temp);
	return __SCPI_SUCCESS;
}

int tSysScpi::setFrontEndIfAtt(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	double tempValue = value.toDouble();

	return setFrontEndIfAtt(tempValue);
}

//计算射频前端 高波段衰减器下发值
inline char tSysScpi::calHighbandAttValueOfFrontend(double value)
{
	int att = (int)value;
  return (~att) & 0x1f;
}

//高波段衰减器设置
int tSysScpi::setHighbandAtt(double value)
{
	if(value < 0)
	  return __SCPI_FAILED;

	sysData.ampt.attRf = value;

	int attrf = (int) value;
	char temp = (~attrf) & 0x1f;
	
	feDownload(0xfff3, temp);

	return __SCPI_SUCCESS;
}

int tSysScpi::setHighbandAtt(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	double att = value.toDouble();

	return setHighbandAtt(att);
}

//计算射频前端 低波段衰减器下发值
inline int tSysScpi::calLowbandAttValueOfFrontend(double value)
{
	int temp = (int)(value * 4);
	return (0x00 << 8) | (temp & 0xff);
}

//低波段衰减器设置
int tSysScpi::setLowbandAtt(double value)
{
	if(value < 0)
		return __SCPI_FAILED;

	sysData.ampt.attRf = value;
	int attrf = (int)(value * 4);
	int temp = (0x00 << 8 ) | (attrf & 0xff);
	feDownload(0xfff2, temp);
	return __SCPI_SUCCESS;
}

int tSysScpi::setLowbandAtt(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	double att = value.toDouble();

	return setLowbandAtt(att);
}

//本振频率设置
int tSysScpi::setFrontEndFreq(double value)
{
	if(value < 0)
		value = MINFREQ;

	sysData.fe.freq = value;
	sysData.freq.start = sysData.fe.freq;

	double LO1 = 0;
	if(sysData.fe.freq >= 20e6 && sysData.fe.freq <= FREQ_START_HH_BAND1)
		LO1 = sysData.fe.freq + L_lo1;
	else if(sysData.fe.freq > FREQ_START_HH_BAND1 && sysData.fe.freq <= FREQ_START_HH_BAND4)
	{
		LO1 = sysData.fe.freq + H_lo1;
		printf("LO1 is %f\n",LO1);
	}
	else if(sysData.fe.freq > FREQ_START_HH_BAND4 && sysData.fe.freq <= MAXFREQ)
		LO1 = sysData.fe.freq - H_lo1;

	unsigned int n = (unsigned int)(LO1 / (2 * 50 * 1e6));
	unsigned int f = (unsigned int)((LO1 / (2 * 50 * 1e6) - n )* pow2_24);

	feDownload(0xfff1, 0x0c007a3c);  //REG6 下发

	char addr,cmd;
	unsigned int temp3 = 0, temp4 = 0;
    int ret = 0;
	addr = 3;
	cmd = 0;
	temp3 = (cmd << 31) | ((addr & 0x3f) << 25) | (( n & ~(1 << 24)) << 1);

	feDownload(0xfff1, temp3);  //REG3 下发
	printf("REG3 is 0x%08x\n",temp3);
	ret = temp3;
	addr = 4;
	cmd = 0;
	temp4 = (cmd << 31) | ((addr & 0x3f) << 25) | (( f & ~(1 << 24)) << 1);

	feDownload(0xfff1, temp4);  //REG4 下发
	printf("REG4 is 0x%08x\n",temp4);

	//前放 高低波段选择
	rfPreamptDown(sysData.ampt.isPreamptOn);

	return ret;
}

int tSysScpi::setFrontEndFreq(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	double freq = value.toDouble();

	return setFrontEndFreq(freq);
}

//清除出厂校准
void tSysScpi::clearFactoryCalibrateData(void)
{
	clearUserCalibrateData();
	resetFactoryCalibrationData();
	resetPreamplifierCalibrationData();
	resetZcCalibrationData();
	resetZcPreamplifierCalibrationData();
	saveFactoryCalibrationData();
	savePreamplifierCalibrationData();
	saveZcFactoryCalibrationData();
	saveZcPreamplifierFactoryCalibrationData();
}

//清除用户校准
void tSysScpi::clearUserCalibrateData(void)
{
	resetUserCalibrationData();
	saveUserCalibrationData();
}

//是否正在校准
bool tSysScpi::isCalibrating(void)
{
	return (sysData.isCalibrating || sysData.isUserCalibrating || sysData.isPreamplifierCalibrating || sysData.isFftCalibrating || sysData.isFactoryCalibrating || sysData.isZcCalibrating || sysData.isZcPreamplifierCalibrating);
}

//前置放大校准
int tSysScpi::PreamplifierCalibrate(int comCal)
{
/*
	bool isCom = (comCal > 0);

	if (sysData.isPreamplifierCalibrating)
	{
		return __SCPI_FAILED;
	}

	if (isCom && comHandle <= 0)
	{
		return __SCPI_FAILED;
	}

	int progress = 0;
	char tempChar[32] = { };
	QString datetime = "";

	getLocalDateTime(tempChar);
	datetime = tempChar;
	presetSystemData();
	sysData.ampt.isPreamptOn = true;
	sysData.ampt.preamtOn = 0x01;
	//sysData.ampt.preamtOn = 0x02;
	controlRf();
	sysData.isPreamplifierCalibrating = true;
	sysData.mode = smCalibrate;
	sysData.canvasNums = 1;
	resetPreamplifierCalibrationData();

	QCoreApplication::processEvents();
	saveScreen();

	//1、help info
	progress = 0;
	outputToScreen(getTheTranslatorString("system auto calibrating") + "......", progress, 0);

	//2、connect to server
	if (!isCom)
	{
		progress = 3;
		sysData.system.io.lan.hostPort = SOCKETSEVERPORT;
		if (connecToServerSocket() == __SCPI_FAILED)
		{
			saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("connect to server failed"), getTheTranslatorString("disconnect to server"));
			outputToScreen(getTheTranslatorString("signal generator is off line"), progress, 1);
			return exitCalibrate();
		}
	}

	//3、check signal generator
	progress = 5;
	outputToScreen(getTheTranslatorString("signal generator checking") + "......", progress, 0);
	sendSerialData(isCom, "*rst");
	usleep(1000 * 3000);

	if (signalIsOnline(isCom) == __SCPI_FAILED)
	{
		saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("Preamplifier calibrate failed"), getTheTranslatorString("signal generator is off line"));
		outputToScreen(getTheTranslatorString("signal generator is off line"), progress, 1);
		return exitCalibrate();
	}

	//4、set signal generator mod off and rf on
	sendSerialData(isCom, ":OUTP:MOD OFF;:OUTP ON;");

	//5、check temperature
	double sysTemperature = 0;
	progress = 8;

	if (getSystemTemperature(&sysTemperature) == __SCPI_FAILED)
	{
		saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("Preamplifier calibrate failed"), getTheTranslatorString("can not get device temperature"));
		outputToScreen(getTheTranslatorString("can not get device temperature"), progress, 1);
		return exitCalibrate();
	}

	//6、reset calibration data
	resetPreamplifierCalibrationData();
	sysData.preamplifierCalData.isCaled = true;
	sysData.preamplifierCalData.temperature = sysTemperature;

	//7、amplitude
	setCalibrateParamOfAbsoluteAmplitude();
	progress = 10;
	outputToScreen(getTheTranslatorString("amplitude calibrating") + "......", progress, 0);

	if (signalOutputFreqIsvalid(isCom, 440e6) == __SCPI_FAILED || signalOutputAmptIsvalid(isCom, -40) == __SCPI_FAILED || getCalibrateRunParam() == __SCPI_FAILED)
	{
		saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("Preamplifier calibrate failed"), getTheTranslatorString("amplitude calibrate failed"));
		outputToScreen(getTheTranslatorString("amplitude calibrate failed"), progress, 1);
		return exitCalibrate();
	}

	sysData.preamplifierCalData.absoluteAmptValue = calRunData.maxValue + 40.0;
	savePreamplifierCalibrationData();

	//8、attenuation
	progress = 15;
//	setCalibrateParamOfAttenuation();
	outputToScreen(getTheTranslatorString("attenuation calibrating") + "......", progress, 0);

	//8.1、att9
	progress = 16;
	setCalibrateDistributeAtt(-20, 0, 9);
	outputToScreen(getTheTranslatorString("attenuation(9dB) calibrating") + "......", progress, 0);

	if (getCalibrateRunParam() == __SCPI_FAILED)
	{
		saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("Preamplifier calibrate failed"), getTheTranslatorString("attenuation(att21) calibrate failed"));
		outputToScreen(getTheTranslatorString("attenuation(9dB) calibrate failed"), progress, 1);
		return exitCalibrate();
	}

	sysData.preamplifierCalData.attBase = calRunData.maxValue;
	sysData.preamplifierCalData.att9 = 0;

	//8.2、att0
	progress = 17;
	setCalibrateDistributeAtt(-30, 0, 0);
	outputToScreen(getTheTranslatorString("attenuation(0dB) calibrating") + "......", progress, 0);

	if (getCalibrateRunParam() == __SCPI_FAILED)
	{
		saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("Preamplifier calibrate failed"), getTheTranslatorString("attenuation(att0) calibrate failed"));
		outputToScreen(getTheTranslatorString("attenuation(0dB) calibrate failed"), progress, 1);
		return exitCalibrate();
	}

	sysData.preamplifierCalData.att0 = calRunData.maxValue - sysData.preamplifierCalData.attBase;

	//8.3、att21
	progress = 18;
	setCalibrateDistributeAtt(-10, 0, 21);
	outputToScreen(getTheTranslatorString("attenuation(21dB) calibrating") + "......", progress, 0);

	if (getCalibrateRunParam() == __SCPI_FAILED)
	{
		saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("Preamplifier calibrate failed"), getTheTranslatorString("attenuation(att12) calibrate failed"));
		outputToScreen(getTheTranslatorString("attenuation(21dB) calibrate failed"), progress, 1);
		return exitCalibrate();
	}
	sysData.preamplifierCalData.att21 = calRunData.maxValue - sysData.preamplifierCalData.attBase;

	//8.4、att30
	progress = 19;
	setCalibrateDistributeAtt(-10, 0, 30);
	outputToScreen(getTheTranslatorString("attenuation(30dB) calibrating") + "......", progress, 0);

	if (getCalibrateRunParam() == __SCPI_FAILED)
	{
		saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("Preamplifier calibrate failed"), getTheTranslatorString("attenuation(att22) calibrate failed"));
		outputToScreen(getTheTranslatorString("attenuation(30dB) calibrate failed"), progress, 1);
		return exitCalibrate();
	}

	sysData.preamplifierCalData.att30 = calRunData.maxValue - sysData.preamplifierCalData.attBase;

	//8.5、att39
	progress = 20;
	setCalibrateDistributeAtt(-10, 0, 39);
	outputToScreen(getTheTranslatorString("attenuation(39dB) calibrating") + "......", progress, 0);

	if (getCalibrateRunParam() == __SCPI_FAILED)
	{
		saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("Preamplifier calibrate failed"), getTheTranslatorString("attenuation(att23) calibrate failed"));
		outputToScreen(getTheTranslatorString("attenuation(39dB) calibrate failed"), progress, 1);
		return exitCalibrate();
	}

	sysData.preamplifierCalData.att39 = calRunData.maxValue - sysData.preamplifierCalData.attBase;
	savePreamplifierCalibrationData();

	//9、if
	progress = 21;
	setCalibrateParamOfIF();
	outputToScreen(getTheTranslatorString("if calibrating") + "......", progress, 0);

	//9.1、10dB
	setCalibrateDistributeIF(10);
	outputToScreen(getTheTranslatorString("if(10dB) calibrating") + "......", progress, 0);

	if (getCalibrateRunParam() == __SCPI_FAILED)
	{
		saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("Preamplifier calibrate failed"), getTheTranslatorString("if(10dB) calibrate failed"));
		outputToScreen(getTheTranslatorString("if(10dB) calibrate failed"), progress, 1);
		return exitCalibrate();
	}
	sysData.preamplifierCalData.if10 = -40 - calRunData.maxValue;

	//9.2、0dB
	progress = 22;
	setCalibrateDistributeIF(0);
	outputToScreen(getTheTranslatorString("if(0dB) calibrating") + "......", progress, 0);

	if (getCalibrateRunParam() == __SCPI_FAILED)
	{
		saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("Preamplifier calibrate failed"), getTheTranslatorString("if(0dB) calibrate failed"));
		outputToScreen(getTheTranslatorString("if(0dB) calibrate failed"), progress, 1);
		return exitCalibrate();
	}

	sysData.preamplifierCalData.if0 = -50 - calRunData.maxValue;

	//9.3、20dB
	progress = 23;
	setCalibrateDistributeIF(20);
	outputToScreen(getTheTranslatorString("if(20dB) calibrating") + "......", progress, 0);

	if (getCalibrateRunParam() == __SCPI_FAILED)
	{
		saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("Preamplifier calibrate failed"), getTheTranslatorString("if(20dB) calibrate failed"));
		outputToScreen(getTheTranslatorString("if(20dB) calibrate failed"), progress, 1);
		return exitCalibrate();
	}

	sysData.preamplifierCalData.if20 = -30 - calRunData.maxValue;

	//9.4、30dB
	progress = 24;
	setCalibrateDistributeIF(30);
	outputToScreen(getTheTranslatorString("if(30dB) calibrating") + "......", progress, 0);

	if (getCalibrateRunParam() == __SCPI_FAILED)
	{
		saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("Preamplifier calibrate failed"), getTheTranslatorString("if(30dB) calibrate failed"));
		outputToScreen(getTheTranslatorString("if(30dB) calibrate failed"), progress, 1);
		return exitCalibrate();
	}

	sysData.preamplifierCalData.if30 = -20 - calRunData.maxValue;
	savePreamplifierCalibrationData();

	//10、rbw
	progress = 25;
	setCalibrateParamOfRbw();
	outputToScreen(getTheTranslatorString("rbw calibrating") + "......", progress, 0);

	//10.1、span 1MHz
	progress = 26;
	setCalibrateDistributeRbw(1e6);
	outputToScreen(getTheTranslatorString("rbw(100kHz) calibrating") + "......", progress, 0);

	if (getCalibrateRunParam() == __SCPI_FAILED)
	{
		saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("Preamplifier calibrate failed"), getTheTranslatorString("rbw(100kHz) calibrate failed"));
		outputToScreen(getTheTranslatorString("rbw(100kHz) calibrate failed"), progress, 1);
		return exitCalibrate();
	}

	sysData.preamplifierCalData.rbw10k = calRunData.maxValue;

	//10.2、span 100kHz
	progress = 27;
	setCalibrateDistributeRbw(100e3);
	outputToScreen(getTheTranslatorString("rbw(1kHz) calibrating") + "......", progress, 0);

	if (getCalibrateRunParam() == __SCPI_FAILED)
	{
		saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("Preamplifier calibrate failed"), getTheTranslatorString("rbw(1kHz) calibrate failed"));
		outputToScreen(getTheTranslatorString("rbw(1kHz) calibrate failed"), progress, 1);
		return exitCalibrate();
	}

	sysData.preamplifierCalData.rbw1k = calRunData.maxValue;

	//fft 900hz
	//10.3  fft calibrate: span 60kHz
	bool prvFftState = sysData.options.isFft10HzOn;
	progress = 28;
	sysData.options.isFft10HzOn = true;
	sysData.fft.fftOn = true;
	sysData.isFftCalibrating = true;
	setCalibrateDistributeRbw(60e3);
	outputToScreen(getTheTranslatorString("rbw(900Hz) calibrating") + "......", progress, 0);

	if (getCalibrateRunParam() == __SCPI_FAILED)
	{
		saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("ZC calibrate failed"), getTheTranslatorString("rbw(900Hz) calibrate failed"));
		outputToScreen(getTheTranslatorString("rbw(900Hz) calibrate failed"), progress, 1);
		return exitCalibrate();
	}

	sysData.preamplifierCalData.rbw900 = calRunData.maxValue;

	//
	for (int i = 0; i < 52; i++)
	{
		if (i < 24)
		{
			sysData.preamplifierCalData.rbw[i] = BWRESP[i];
		} else
		{
			sysData.preamplifierCalData.rbw[i] = BWRESP[i] - (sysData.preamplifierCalData.rbw1k - sysData.preamplifierCalData.rbw10k);

			if (i >= 26) //800Hz
			{
				sysData.preamplifierCalData.rbw[i] = sysData.preamplifierCalData.rbw[i] + 6.0;
			}

			//sysData.factoryCalData.rbw_fft[i] = BWRESP_FFT[i];
			//fft rbw calibrate------
			sysData.preamplifierCalData.rbw_fft[i] = BWRESP_FFT[i] - (sysData.preamplifierCalData.rbw900 - sysData.preamplifierCalData.rbw10k);
			//------
		}
	}

	sysData.options.isFft10HzOn = false;
	sysData.isFftCalibrating = false;
	savePreamplifierCalibrationData();

	//11、resp
	QString freqAndPowCmd = "";
	char freqChar[32] = { };
	int rfInPower = 0;
	int freqRespCount = sizeof FREQRESPHZ / sizeof FREQRESPHZ[0];
	int freqRespSum = 5 * freqRespCount;

	progress = 30;
	setCalibrateParamOfFrequencyRespond();
	outputToScreen(getTheTranslatorString("frequency respond calibrating") + "......", progress, 0);

	for (int i = 0; i < 5; i++)
	{
		switch (i)
		{
			case 0:
				rfInPower = -40;
				break;
			case 1:
				rfInPower = -40;
				break;
			case 2:
				rfInPower = -20;
				break;
			case 3:
				rfInPower = -20;
				break;
			case 4:
				rfInPower = -10;
				break;
				//	  case 5: rfInPower = -10; break;
		}

		freqAndPowCmd = ":POW " + QString(intToString(rfInPower, freqChar)).trimmed();
		sendSerialData(isCom, freqAndPowCmd);

		setCalibrateParamOfFrequencyRespond();
		for (int j = 0; j < freqRespCount; j++)
		{
			stLabelCalibrateProgress = progress + (i * freqRespCount + j) * 70 / freqRespSum;
			outputToScreen(getTheTranslatorString("frequency respond calibrating") + "(" + QString(getFreqAndUnitString(FREQRESPHZ[j], 0, freqChar)).trimmed() + ")......", stLabelCalibrateProgress, 0);

			if (j == 0 || j == 1)
			{
				if (j == 0)
				{
					sysData.preamplifierCalData.freqResp[i][j] = RESP9KHZ;
				} else if (j == 1)
				{
					sysData.preamplifierCalData.freqResp[i][j] = RESP20KHZ;
				}
				continue;
			}

			freqAndPowCmd = ":FREQ ";
			freqAndPowCmd += QString(floatToString(FREQRESPHZ[j], 0, 0, freqChar)).trimmed();
			sendSerialData(isCom, freqAndPowCmd);

			switch (i)
			{
				case 0:
					setCalibrateDistributeFrequencyRespond(j, -30, 0);
					break;
				case 1:
					setCalibrateDistributeFrequencyRespond(j, -20, 9);
					break;
				case 2:
					setCalibrateDistributeFrequencyRespond(j, -10, 21);
					break;
				case 3:
					setCalibrateDistributeFrequencyRespond(j, -10, 30);
					break;
				case 4:
					setCalibrateDistributeFrequencyRespond(j, 0, 39);
					break;
					//	    case 5: setCalibrateDistributeFrequencyRespond(j, 0, 50); break;
				default:
					continue;
			}

			if (getCalibrateRunParam() == __SCPI_FAILED)
			{
				saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("Preamplifier calibrate failed"), getTheTranslatorString("frequency respond calibrate failed On") + " " + QString(getFreqAndUnitString(FREQRESPHZ[j], 0, freqChar)).trimmed());
				outputToScreen(getTheTranslatorString("frequency respond calibrate failed On") + " " + QString(getFreqAndUnitString(FREQRESPHZ[j], 0, freqChar)).trimmed(), stLabelCalibrateProgress, 1);
				return exitCalibrate();
			}

			sysData.preamplifierCalData.freqResp[i][j] = rfInPower - calRunData.maxValue;
		}
	}
	savePreamplifierCalibrationData();

	progress = 100;
	sendSerialData(isCom, ":FREQ 440e6;:POW -40;");

	//close server
	if (!isCom)
	{
		disConnectFromServerSocket();
	}

	saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("Preamplifier calibrate successed"), "");
	outputToScreen(getTheTranslatorString("Preamplifier calibrate successed"), progress, 0);
	sysData.isPreamplifierCalibrating = false;
	sysData.mode = smSpectrum;
	sysData.canvasNums = 1;
	sysData.options.isFft10HzOn = prvFftState;
*/
	return __SCPI_SUCCESS;
}


//出厂校准
int tSysScpi::factoryCalibrate(int comCal)
{
	bool isCom = (comCal >= 1);

	if (sysData.isFactoryCalibrating)
	{
		return __SCPI_FAILED;
	}

	if (isCom && comHandle <= 0)
	{
		return __SCPI_FAILED;
	}

	int progress = 0;
	char tempChar[128] = { };
	QString datetime = "";

	QTime t = QTime::currentTime();
	t.restart();

	getLocalDateTime(tempChar);
	datetime = tempChar;
	presetSystemData();
	controlRf();
	sysData.isFactoryCalibrating = true;
	sysData.mode = smCalibrate;
	sysData.canvasNums = 1;
	resetFactoryCalibrationData();
	resetUserCalibrationData();
	QCoreApplication::processEvents();//processes all pending events as  a long operation
//	saveScreen();

	//1、help info
	progress = 0;
//	outputToScreen(getTheTranslatorString("system auto calibrating") + "......", progress, 0);

	//2、connect to server
	if (!isCom)
	{
		progress = 3;
		sysData.system.io.lan.hostPort = SOCKETSEVERPORT;
		if (connecToServerSocket() == __SCPI_FAILED)
		{
			saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("connect to server failed"), getTheTranslatorString("disconnect to server"));
	//		outputToScreen(getTheTranslatorString("disconnect to server"), progress, 1);
			sprintf(tempChar, "connect to server failed");
			returnScpiResult(QString(tempChar));
			sleep(2);
			return exitCalibrate();
		}
	}
	printf("signal generator connected!\n");
	//3、check signal generator
	progress = 5;
//	outputToScreen(getTheTranslatorString("signal generator checking") + "......", progress, 0);
	sendSerialData(isCom, "*RST");
	usleep(1000 * 3000);

	if (signalIsOnline(isCom) == __SCPI_FAILED)
	{
		saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("factory calibrate failed"), getTheTranslatorString("signal generator is off line"));
	//	outputToScreen(getTheTranslatorString("signal generator is off line"), progress, 1);
		sprintf(tempChar, "signal generator is off line");
		returnScpiResult(QString(tempChar));
		sleep(2);
		return exitCalibrate();
	}
	printf("signal generator online!\n");

	//4、set signal generator mod off and rf on
	sendSerialData(isCom, ":OUTP:MOD OFF;:OUTP ON;");

	//5 check power meter
	progress = 6;
	//	outputToScreen(getTheTranslatorString("power meter checking") + "......", progress, 0);
	usleep(1000);
	if(!connectToPowerMeter())
	{
		saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("factory calibrate failed"), getTheTranslatorString("power meter is off line"));
	//	outputToScreen(getTheTranslatorString("power meter is off line"), progress, 1);
		sprintf(tempChar, "power meter not connected...");
		returnScpiResult(QString(tempChar));
		sleep(2);
		return exitCalibrate();
	}
	printf("power sensor connected!\n");
	//5、check temperature
	double sysTemperature = 0;
	progress = 8;

//	if (getSystemTemperature(&sysTemperature) == __SCPI_FAILED)
//	{
//		saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("factory calibrate failed"), getTheTranslatorString("can not get device temperature"));
//		outputToScreen(getTheTranslatorString("can not get device temperature"), progress, 1);
//		sleep(2);
//		return exitCalibrate();
//	}

	//6、reset calibration data
	resetFactoryCalibrationData();
	sysData.factoryCalData.isCaled = true;
	sysData.factoryCalData.temperature = sysTemperature;

	//7、amplitude
	setCalibrateParamOfAbsoluteAmplitude();

	progress = 10;
//	outputToScreen(getTheTranslatorString("amplitude calibrating") + "......", progress, 0);

	if (signalOutputFreqIsvalid(isCom, 1e9) == __SCPI_FAILED || signalOutputAmptIsvalid(isCom, -24) == __SCPI_FAILED || getCalibrateRunParam() == __SCPI_FAILED || setPowerMeterFreq(1e9) == __SCPI_FAILED)
	{
		saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("factory calibrate failed"), getTheTranslatorString("amplitude calibrate failed"));
	//	outputToScreen(getTheTranslatorString("amplitude calibrate failed"), progress, 1);
		sprintf(tempChar, "amplitude calibrate failed");
		returnScpiResult(QString(tempChar));
		sleep(2);
		return exitCalibrate();
	}

//	usleep(1000);
	if(getDataFromPowerMeter() != __SCPI_SUCCESS)
	{
		saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("factory calibrate failed"), getTheTranslatorString("amplitude calibrate failed"));
	//	outputToScreen(getTheTranslatorString("amplitude calibrate failed"), progress, 1);
		sleep(2);
		return exitCalibrate();
	}
	sysData.factoryCalData.absoluteAmptValue = calRunData.maxValue - sysData.measure.powerMeter.ampt;
	saveFactoryCalibrationData();
	saveUserCalibrationData();
	printf("absoluteAmptValue cal success!\n");

	// 8 if attenuation
	progress = 20;
	setCalibrateParamOfIfAttenuation();
	setFrontendAtt(10, 20);
	//printf("setFrontendAtt(10, 20) is deled\n");
//	setFrontEndIfAtt(10);
//	outputToScreen(getTheTranslatorString("rf attenuation calibrating") + "......", progress, 0);
//	if (signalOutputFreqIsvalid(isCom, 1e9) == __SCPI_FAILED || signalOutputAmptIsvalid(isCom, -4) == __SCPI_FAILED || getCalibrateRunParam() == __SCPI_FAILED || setPowerMeterFreq(1e9) == __SCPI_FAILED)
//	{
//		saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("factory calibrate failed"), getTheTranslatorString("amplitude calibrate failed"));
//		outputToScreen(getTheTranslatorString("amplitude calibrate failed"), progress, 1);
//		sleep(2);
//		return exitCalibrate();
//	}
	QString cmd = "";
	cmd = ":FREQ 1e9";
	sendSerialData(isCom, cmd);
	cmd = ":POW " + QString(intToString(-4, tempChar)).trimmed();
	sendSerialData(isCom, cmd);
	setPowerMeterFreq(1e9);

	//setFrontendAtt(10, 20);
	//printf("setFrontendAtt(10, 20) is deled\n");
	//cmd = ":POW " + QString(intToString(-4, tempChar)).trimmed();

	getCalibrateRunParam();
	getDataFromPowerMeter();
	double ifVal_20 = calRunData.maxValue - sysData.measure.powerMeter.ampt;
	//double ifVal_20 = sysData.measure.powerMeter.ampt;
#if 1	
	printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	printf("if attenuation calibrate starting...!\n");
	sleep(3);
	for(int i = 0; i < 32; i++)
	{
	//	outputToScreen(getTheTranslatorString("if attenuation calibrating") + "......", progress + i * 10 / 30, 0);
		setFrontendAtt(10, i);

		cmd = ":POW " + QString(intToString(-24 + i, tempChar)).trimmed();
		sendSerialData(isCom, cmd);

		if (getCalibrateRunParam() == __SCPI_FAILED || getDataFromPowerMeter() != __SCPI_SUCCESS )
		{
			saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("factory calibrate failed"), getTheTranslatorString("if attenuation calibrate failed"));
//			outputToScreen(getTheTranslatorString("if attenuation calibrate failed"), progress, 1);
			sprintf(tempChar, "if att calibrate failed at %d", i);
			returnScpiResult(QString(tempChar));
			sleep(2);
			return exitCalibrate();
		}

		//sysData.factoryCalData.attIf[i] = calRunData.maxValue - sysData.measure.powerMeter.ampt - ifVal_20 + (i - 20);
		sysData.factoryCalData.attIf[i] = calRunData.maxValue - sysData.measure.powerMeter.ampt - ifVal_20;
		//sysData.factoryCalData.attIf[i] = calRunData.maxValue - sysData.measure.powerMeter.ampt;

		if(i == 20)
			sysData.factoryCalData.attIf[i] = 0;
		usleep(1000);
	}
	printf("if attenuation calibrate success!\n");
	saveFactoryCalibrationData();
#endif
	/*
	// 10 rbw
	progress = 30;
//	outputToScreen(getTheTranslatorString("rbw calibrate calibrating") + "......", progress, 0);
	for(int i = 0; i < 24; i++)
	{
		sysData.factoryCalData.rbw[i] = BWRESP[i];
	}
	saveFactoryCalibrationData();
*/
	progress = 30;
	sendSerialData(isCom, ":FREQ 1e9;:POW -4;");

	// 8 freq resp
	progress = 35;
	setCalibrateParamOfRfAttenuation();
	setFrontendAtt(20, 20);
//	outputToScreen(getTheTranslatorString("rf attenuation calibrating") + "......", progress, 0);
	if (signalOutputFreqIsvalid(isCom, 1e9) == __SCPI_FAILED || signalOutputAmptIsvalid(isCom, -4) == __SCPI_FAILED || getCalibrateRunParam() == __SCPI_FAILED || setPowerMeterFreq(1e9) == __SCPI_FAILED)
	{
		saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("factory calibrate failed"), getTheTranslatorString("amplitude calibrate failed"));
	//	outputToScreen(getTheTranslatorString("amplitude calibrate failed"), progress, 1);
		sleep(2);
		return exitCalibrate();
	}

	int  power = 0;
	double attRf = 0, attIf = 20;
	int freqRespCount =  sizeof FREQRESPHZ / sizeof FREQRESPHZ[0];
	int freqRespSum = 4 * freqRespCount;
//	char tempChar[32] = { };

	QFile file("/home/sky/factory.txt");
	file.open(QIODevice::ReadWrite | QIODevice::Append);
	QTextStream out(&file);
	
#if 1
	printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
	printf("rf attenuation calibrate starting...!\n");
	sleep(3);
	FILE * fp = fopen("caldata.c","w");
	for(int i = 0; i < 4; i++)
	{
		switch(i)
		{
			case 0:
				attRf = 0;
				power = -14;
				break;
			case 1:
				attRf = 10;
				power = -4;
				break;
			case 2:
				attRf = 20;
				power = 6;
				break;
			case 3:
				attRf = 30;
				power = 16;
				break;
		}

		setFrontendAtt(attRf, attIf);

		cmd = ":POW " + QString(intToString(power, tempChar)).trimmed();
		sendSerialData(isCom, cmd);

		for(int j = 0; j < freqRespCount; j++)
		{
	//		stLabelCalibrateProgress = progress + (i * freqRespCount + j) * 65 / freqRespSum;
	//		outputToScreen(getTheTranslatorString("frequency respond calibrating") + "(" + QString(getFreqAndUnitString(FREQRESPHZ[j], 0, tempChar)).trimmed() + ")......", stLabelCalibrateProgress, 0);
			cmd = ":FREQ ";
			cmd += QString(floatToString(FREQRESPHZ[j], 0, 0, tempChar)).trimmed();
			sendSerialData(isCom, cmd);
		//	sprintf(tempChar, "calibrate freq %s, rf att %d",getFreqAndUnitString(FREQRESPHZ[j], 0, tempChar), attRf);
		//	returnScpiResult(QString(tempChar));
			if(setPowerMeterFreq(FREQRESPHZ[j]) == __SCPI_FAILED)
			{
				saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("factory calibrate failed"), getTheTranslatorString("rf att calibrate failed")+ "," + "power meter freq set faild at" + QString(getFreqAndUnitString(FREQRESPHZ[j], 0, tempChar)).trimmed());
	//			outputToScreen(getTheTranslatorString("rf att calibrate failed"), progress, 1);
				sleep(2);
				return exitCalibrate();
			}

			setCalibrateDistributeFrequencyRespond(j, 0, attRf, attIf);
			usleep(5000);
			if(getCalibrateRunParam() == __SCPI_FAILED)
			{
				sprintf(tempChar, "calibrate freq %s, rf att %f", getFreqAndUnitString(FREQRESPHZ[j], 0, tempChar), attRf);
	//			returnScpiResult(QString(tempChar));
				sleep(2);
				printf("\ncalibrate freq is %d,attRf is %f,power is %d\n",\
												FREQRESPHZ[j],attRf,power);
				fprintf(fp,"calibrate freq is %f,attRf is %f,power is %d\n",\
												FREQRESPHZ[j],attRf,power);
				//return exitCalibrate();
			}
			if (getDataFromPowerMeter() == __SCPI_FAILED)
			{
				saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("factory calibrate failed"), getTheTranslatorString("rf att calibrate failed") + QString(getFreqAndUnitString(FREQRESPHZ[j], 0, tempChar)).trimmed());
		//		outputToScreen(getTheTranslatorString("rf att calibrate failed"), progress, 1);
		//		sprintf(tempChar, "get power meter freq %s", getFreqAndUnitString(FREQRESPHZ[j], 0, tempChar));
		//		returnScpiResult(QString(tempChar));
				sleep(2);
				return exitCalibrate();
			}
			sysData.factoryCalData.freqResp[i][j] = calRunData.maxValue - sysData.measure.powerMeter.ampt;
			//usleep(1000 * 100);
			out << "freq resp " << i <<"_" << j << " :" << sysData.factoryCalData.freqResp[i][j] << "\r\n";
		}
		sleep(3);
		printf("\n attrf=%f,power=%d calibrate succeed!\n\n",attRf,power);

	}
	
	fclose(fp);
#endif
	saveFactoryCalibrationData();
	sprintf(tempChar, "factory calibrate success, cal time %d ms", t.elapsed());
	returnScpiResult(QString(tempChar));
	saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("factory calibrate successed"), "");
//	outputToScreen(getTheTranslatorString("factory calibrate successed"), progress, 0);
	sysData.isFactoryCalibrating = false;
//	sysData.options.isFft10HzOn = prvFftState;
	sysData.mode = smSpectrum;
	sysData.canvasNums = 1;

	return __SCPI_SUCCESS;
}


//单频点校准
int tSysScpi::freqCalibrate(double freq)
{
	int index, freqRespCount =  sizeof FREQRESPHZ / sizeof FREQRESPHZ[0];
	if(freq < FREQRESPHZ[0] || freq > FREQRESPHZ[freqRespCount-1])
		return __SCPI_FAILED;

	QString cmd = "";
	char tempChar[128] = { };
	QString datetime = "";

	sysData.system.io.lan.hostPort = SOCKETSEVERPORT;
	if (connecToServerSocket() == __SCPI_FAILED)
	{
		return __SCPI_FAILED;
	}
	printf("signal generator connected!\n");
	sendSerialData(false, "*RST");
	usleep(1000 * 3000);

	if (signalIsOnline(false) == __SCPI_FAILED)
	{
		return __SCPI_FAILED;
	}
	printf("signal generator online!\n");

	sendSerialData(false, ":OUTP:MOD OFF;:OUTP ON;");

	usleep(1000);
	if(!connectToPowerMeter())
	{
		return __SCPI_FAILED;
	}
	printf("power sensor connected!\n");
	
	double sysTemperature = 0;
	sysData.factoryCalData.temperature = sysTemperature;
	setCalibrateParamOfAbsoluteAmplitude();
	setCalibrateParamOfIfAttenuation();
	
	int  power = 0;
	double attRf = 0, attIf = 20;
	int freqRespSum = 4 * freqRespCount;

	QFile file("/home/sky/factory.txt");
	file.open(QIODevice::ReadWrite | QIODevice::Append);
	QTextStream out(&file);

	for(int j = 0; j < freqRespCount; j++)
	{
		if(FREQRESPHZ[j] == freq)
		{
			index = j;
			break;
		}
	}
	for(int i = 0; i < 4; i++)
	{
		switch(i)
		{
			case 0:
				attRf = 0;
				power = -14;
				break;
			case 1:
				attRf = 10;
				power = -4;
				break;
			case 2:
				attRf = 20;
				power = 6;
				break;
			case 3:
				attRf = 30;
				power = 16;
				break;
		}
		cmd = ":POW " + QString(intToString(power, tempChar)).trimmed();
		sendSerialData(false, cmd);
		cmd = ":FREQ ";
		cmd += QString(floatToString(FREQRESPHZ[index], 0, 0, tempChar)).trimmed();
		sendSerialData(false, cmd);

		setFrontendAtt(attRf, attIf);
		
		if(getCalibrateRunParam() == __SCPI_FAILED)
		{
			sleep(2);
			return __SCPI_FAILED;
		}
		
		setCalibrateDistributeFrequencyRespond(index, 0, attRf, attIf);
		usleep(5000);
		
		if (getDataFromPowerMeter() == __SCPI_FAILED)
		{
			sleep(2);
			return __SCPI_FAILED;
		}
		sysData.factoryCalData.freqResp[i][index] = calRunData.maxValue - sysData.measure.powerMeter.ampt;
		out << "freq resp " << i <<"_" << index << " :" << sysData.factoryCalData.freqResp[i][index] << "\r\n";

		printf("\n freq=%f,attrf=%f,power=%d calibrate succeed!\n\n",freq,attRf,power);

	}
	
	saveFactoryCalibrationData();
	returnScpiResult(QString(tempChar));
	sysData.canvasNums = 1;

	return __SCPI_SUCCESS;
}






//直采通道前置放大校准
int tSysScpi::ZCPreamplifierCalibrate(int comCal)
{

	bool isCom = (comCal >= 1);

	if (sysData.isZcPreamplifierCalibrating)
	{
		return __SCPI_FAILED;
	}

	if (!sysData.options.directChannelOptionOn)
	{
		return __SCPI_FAILED;
	}

	if (isCom && comHandle <= 0)
	{
		return __SCPI_FAILED;
	}

	int progress = 0;
	char tempChar[32] = { };
	QString datetime = "";

	getLocalDateTime(tempChar);
	datetime = tempChar;
	presetSystemData();
	sysData.ampt.isPreamptOn = true;
	sysData.ampt.preamtOn = 0x01;
	sysData.freq.isLowChannel = true;
	setFrequencyOfLowChannel(sysData.freq.isLowChannel);
	controlRf();
	sysData.isZcPreamplifierCalibrating = true;//start calibrating
	sysData.mode = smCalibrate;
	sysData.canvasNums = 1;
	resetZcPreamplifierCalibrationData();
	//resetUserCalibrationData();

	QCoreApplication::processEvents();//processes all pending events as  a long operation
	saveScreen();

	//1、help info
	progress = 0;
	outputToScreen(getTheTranslatorString("Low Freq Preamplifier calibrating") + "......", progress, 0);

	//2、connect to server
	if (!isCom)
	{
		progress = 3;
		sysData.system.io.lan.hostPort = SOCKETSEVERPORT_ZC;
		strcpy(sysData.system.io.lan.hostIp, SOCKETSERVERIP_ZC);
		if (connecToServerSocket() == __SCPI_FAILED)
		{
			saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("connect to server failed"), getTheTranslatorString("disconnect to server"));
			outputToScreen(getTheTranslatorString("disconnect to server"), progress, 1);
			sleep(2);
			return exitCalibrate();
		}
	}

	//3、check signal generator
	progress = 5;
	outputToScreen(getTheTranslatorString("signal generator checking") + "......", progress, 0);
	sendSerialData(isCom, "*RST");
	usleep(1000 * 3000);

	if (signalIsOnline(isCom) == __SCPI_FAILED)
	{
		saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("ZC Preamplifier calibrate failed"), getTheTranslatorString("signal generator is off line"));
		outputToScreen(getTheTranslatorString("signal generator is off line"), progress, 1);
		sleep(2);
		return exitCalibrate();
	}

	//4、set signal generator mod off and rf on
	//  sendSerialData(isCom, ":OUTP:MOD OFF;:OUTP ON;");
	sendSerialData(isCom, ":SYST:MODO OFF");
	sendSerialData(isCom, ":SYST:RFO ON");

	//5、check temperature
	double sysTemperature = 0;
	progress = 8;

	if (getSystemTemperature(&sysTemperature) == __SCPI_FAILED)
	{
		saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("Low Freq Preamplifier calibrate failed"), getTheTranslatorString("can not get device temperature"));
		outputToScreen(getTheTranslatorString("can not get device temperature"), progress, 1);
		sleep(2);
		return exitCalibrate();
	}

	//6、reset zc preamplifier calibration data
	resetZcPreamplifierCalibrationData();
	sysData.zcPreamplifierCalData.isCaled = true;
	sysData.zcPreamplifierCalData.temperature = sysTemperature;

	//7、amplitude
	setZcCalibrateParamOfAbsoluteAmplitude();
	progress = 10;
	outputToScreen(getTheTranslatorString("amplitude calibrating") + "......", progress, 0);

	if (signalOutputFreqIsvalid(isCom, 5e6) == __SCPI_FAILED || signalOutputAmptIsvalid(isCom, -40) == __SCPI_FAILED || getCalibrateRunParam() == __SCPI_FAILED)
	{
		saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("Low Freq Preamplifier calibrate failed"), getTheTranslatorString("amplitude calibrate failed"));
		outputToScreen(getTheTranslatorString("amplitude calibrate failed"), progress, 1);
		sleep(2);
		return exitCalibrate();
	}
	sysData.zcPreamplifierCalData.absoluteAmptValue = calRunData.maxValue + 40.0;
	saveZcPreamplifierFactoryCalibrationData();
	//8、attenuation
	progress = 15;
	setZcCalibrateParamOfAbsoluteAmplitude();
	outputToScreen(getTheTranslatorString("attenuation calibrating") + "......", progress, 0);

	//8.1、att9
	progress = 16;
	signalOutputAmptIsvalid(isCom, -30);
	setCalibrateDistributeAtt(-20, 0, 9);
	outputToScreen(getTheTranslatorString("attenuation(9dB) calibrating") + "......", progress, 0);

	if (getCalibrateRunParam() == __SCPI_FAILED)
	{
		saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("Low Freq Preamplifier calibrate failed"), getTheTranslatorString("attenuation(9dB) calibrate failed"));
		outputToScreen(getTheTranslatorString("attenuation(9dB) calibrate failed"), progress, 1);
		sleep(2);
		return exitCalibrate();
	}
	sysData.zcPreamplifierCalData.attBase = calRunData.maxValue;
	sysData.zcPreamplifierCalData.att9 = 0;

	//8.2、att0
	progress = 17;
	setCalibrateDistributeAtt(-30, 0, 0);
	outputToScreen(getTheTranslatorString("attenuation(0dB) calibrating") + "......", progress, 0);

	if (getCalibrateRunParam() == __SCPI_FAILED)
	{
		saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("Low Freq Preamplifier calibrate failed"), getTheTranslatorString("attenuation(0dB) calibrate failed"));
		outputToScreen(getTheTranslatorString("attenuation(0dB) calibrate failed"), progress, 1);
		sleep(2);
		return exitCalibrate();
	}
	sysData.zcPreamplifierCalData.att0 = calRunData.maxValue - sysData.zcPreamplifierCalData.attBase;

	//8.3、att21
	progress = 18;
	setCalibrateDistributeAtt(-10, 0, 21);
	outputToScreen(getTheTranslatorString("attenuation(21dB) calibrating") + "......", progress, 0);

	if (getCalibrateRunParam() == __SCPI_FAILED)
	{
		saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("Low Freq Preamplifier calibrate failed"), getTheTranslatorString("attenuation(21dB) calibrate failed"));
		outputToScreen(getTheTranslatorString("attenuation(21dB) calibrate failed"), progress, 1);
		sleep(2);
		return exitCalibrate();
	}
	sysData.zcPreamplifierCalData.att21 = calRunData.maxValue - sysData.zcPreamplifierCalData.attBase;

	//8.4、att30
	progress = 19;
	setCalibrateDistributeAtt(-10, 0, 30);
	outputToScreen(getTheTranslatorString("attenuation(30dB) calibrating") + "......", progress, 0);

	if (getCalibrateRunParam() == __SCPI_FAILED)
	{
		saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("Low Freq Preamplifier calibrate failed"), getTheTranslatorString("attenuation(30dB) calibrate failed"));
		outputToScreen(getTheTranslatorString("attenuation(30dB) calibrate failed"), progress, 1);
		sleep(2);
		return exitCalibrate();
	}
	sysData.zcPreamplifierCalData.att30 = calRunData.maxValue - sysData.zcPreamplifierCalData.attBase;

	//8.5、att39
	progress = 20;
	setCalibrateDistributeAtt(-10, 0, 39);
	outputToScreen(getTheTranslatorString("attenuation(39dB) calibrating") + "......", progress, 0);

	if (getCalibrateRunParam() == __SCPI_FAILED)
	{
		saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("Low Freq Preamplifier calibrate failed"), getTheTranslatorString("attenuation(39dB) calibrate failed"));
		outputToScreen(getTheTranslatorString("attenuation(39dB) calibrate failed"), progress, 1);
		sleep(2);
		return exitCalibrate();
	}
	sysData.zcPreamplifierCalData.att39 = calRunData.maxValue - sysData.zcPreamplifierCalData.attBase;
	saveZcPreamplifierFactoryCalibrationData();

	//9、if
	progress = 21;
	signalOutputAmptIsvalid(isCom, -40);
	setZcCalibrateParamOfIF();
	outputToScreen(getTheTranslatorString("if calibrating") + "......", progress, 0);

	//9.1、10dB
	setZcCalibrateDistributeIF(10);
	outputToScreen(getTheTranslatorString("if(10dB) calibrating") + "......", progress, 0);

	if (getCalibrateRunParam() == __SCPI_FAILED)
	{
		saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("Low Freq Preamplifier calibrate failed"), getTheTranslatorString("if(10dB) calibrate failed"));
		outputToScreen(getTheTranslatorString("if(10dB) calibrate failed"), progress, 1);
		sleep(2);
		return exitCalibrate();
	}

	sysData.zcPreamplifierCalData.if10 = -40 - calRunData.maxValue;

	//9.2、0dB
	progress = 22;
	setZcCalibrateDistributeIF(0);
	outputToScreen(getTheTranslatorString("if(0dB) calibrating") + "......", progress, 0);

	if (getCalibrateRunParam() == __SCPI_FAILED)
	{
		saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("Low Freq Preamplifier calibrate failed"), getTheTranslatorString("if(0dB) calibrate failed"));
		outputToScreen(getTheTranslatorString("if(0dB) calibrate failed"), progress, 1);
		sleep(2);
		return exitCalibrate();
	}

	 sysData.zcPreamplifierCalData.if0 = -50 - calRunData.maxValue;

	//9.3、20dB
	progress = 23;
	setZcCalibrateDistributeIF(20);
	outputToScreen(getTheTranslatorString("if(20dB) calibrating") + "......", progress, 0);

	if (getCalibrateRunParam() == __SCPI_FAILED)
	{
		saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("Low Freq Preamplifier calibrate failed"), getTheTranslatorString("if(20dB) calibrate failed"));
		outputToScreen(getTheTranslatorString("if(20dB) calibrate failed"), progress, 1);
		sleep(2);
		return exitCalibrate();
	}

	sysData.zcPreamplifierCalData.if20 = -30 - calRunData.maxValue;

	//9.4、30dB
	progress = 24;
	setZcCalibrateDistributeIF(30);
	outputToScreen(getTheTranslatorString("if(30dB) calibrating") + "......", progress, 0);

	if (getCalibrateRunParam() == __SCPI_FAILED)
	{
		saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("Low Freq Preamplifier calibrate failed"), getTheTranslatorString("if(30dB) calibrate failed"));
		outputToScreen(getTheTranslatorString("if(30dB) calibrate failed"), progress, 1);
		sleep(2);
		return exitCalibrate();
	}

	sysData.zcPreamplifierCalData.if30 = -20 - calRunData.maxValue;
	saveZcPreamplifierFactoryCalibrationData();

	//10、rbw
	progress = 25;
	sysData.options.isFft10HzOn = true;
	setZcCalibrateParamOfRbw();
	outputToScreen(getTheTranslatorString("rbw calibrating") + "......", progress, 0);

/*	char rbwChar[32] = { };
	for (int i = 0; i < sizeof RAMCIC_ZC_HZ / sizeof RAMCIC_ZC_HZ[0]; i++)
	{
		if (RAMCIC_ZC_HZ[i] <= 20)
		{
			setFrequencyOfSpan(1e3);
		}
		setBwOfRbw(RAMCIC_ZC_HZ[i]);
		outputToScreen(getTheTranslatorString("rbw calibrating") + "(" + QString(getBwAndUnitString(RAMCIC_ZC_HZ[i], 0, rbwChar)).trimmed() + ")......", progress, 0);

		if (getCalibrateRunParam() == __SCPI_FAILED)
		{
			saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("Low Freq Preamplifier calibrate failed"), getTheTranslatorString("rbw calibrate failed On") + QString(getBwAndUnitString(RAMCIC_ZC_HZ[i], 0, rbwChar)).trimmed());
			outputToScreen(getTheTranslatorString("rbw calibrate failed On") + " " + QString(getBwAndUnitString(RAMCIC_ZC_HZ[i], 0, rbwChar)).trimmed(), progress, 1);
			sleep(2);
			return exitCalibrate();
		}
		sysData.zcPreamplifierCalData.rbw[i] = -40 - calRunData.maxValue;
	}
*/
  setBwOfRbw(10e3);
  setFrequencyOfSpan(10e3 * 100);
  controlRf();

  if (getCalibrateRunParam() == __SCPI_FAILED)
  {
    saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("ZC Preamplifier calibrate failed"), getTheTranslatorString("rbw(10kHz) calibrate failed"));
    outputToScreen(getTheTranslatorString("rbw(10kHz) calibrate failed"), progress, 1);
    sleep(2);
    return exitCalibrate();
  }
  sysData.zcPreamplifierCalData.rbw10k = calRunData.maxValue;

  for (int i = 0; i < sizeof BWRESP_ZC /sizeof BWRESP_ZC[0]; i++)
  {
    outputToScreen(getTheTranslatorString("rbw calibrating") + "(" + ((QString)getFreqAndUnitString(RAMCIC_ZC_HZ[i], 0, tempChar)).trimmed() + ")" + "......", progress, 0);

	if (i == 0)
	{
	  sysData.zcPreamplifierCalData.rbw[i] = 0;
	} else
	{
	  setBwOfRbw(RAMCIC_ZC_HZ[i]);
	  setFrequencyOfSpan(RAMCIC_ZC_HZ[i] * 100);
	  controlRf();
	  if (getCalibrateRunParam() == __SCPI_SUCCESS)
	  {
	    sysData.zcPreamplifierCalData.rbw[i] = (calRunData.maxValue - sysData.zcPreamplifierCalData.rbw10k) * -1.0;
	  } else
	  {
	    return exitCalibrate();
	  }
	}

    //sysData.zcPreamplifierCalData.rbw[i] = BWRESP_ZC[i];
  }

	saveZcPreamplifierFactoryCalibrationData();

	//11、resp
	QString freqAndPowCmd = "";
	char freqChar[32] = { };
	int rfInPower = 0;
	int freqRespCount = sizeof FREQRESPHZ_ZC / sizeof FREQRESPHZ_ZC[0];
	int freqRespSum = 5 * freqRespCount;

	progress = 30;
	setZcCalibrateParamOfFrequencyRespond();
	outputToScreen(getTheTranslatorString("frequency respond calibrating") + "......", progress, 0);

	for (int i = 0; i < 5; i++)
	{
		switch (i)
		{
			case 0:
				rfInPower = -40;
				break;
			case 1:
				rfInPower = -40;
				break;
			case 2:
				rfInPower = -20;
				break;
			case 3:
				rfInPower = -20;
				break;
			case 4:
				rfInPower = -10;
				break;
		}

		freqAndPowCmd = ":POW " + QString(intToString(rfInPower, freqChar)).trimmed();
		sendSerialData(isCom, freqAndPowCmd);

		setZcCalibrateParamOfFrequencyRespond();
		for (int j = 0; j < freqRespCount; j++)
		{
			stLabelCalibrateProgress = progress + (i * freqRespCount + j) * 70 / freqRespSum;
			outputToScreen(getTheTranslatorString("frequency respond calibrating") + "(" + QString(getFreqAndUnitString(FREQRESPHZ_ZC[j], 0, freqChar)).trimmed() + ")......", stLabelCalibrateProgress, 0);

			if (signalOutputFreqIsvalid(isCom, FREQRESPHZ_ZC[j]) == __SCPI_FAILED)
			{
				saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("Low Freq calibrate failed"), getTheTranslatorString("frequency respond calibrate failed On") + " " + QString(getFreqAndUnitString(FREQRESPHZ_ZC[j], 0, freqChar)).trimmed());
				outputToScreen(getTheTranslatorString("frequency respond calibrate failed On") + " " + QString(getFreqAndUnitString(FREQRESPHZ_ZC[j], 0, freqChar)).trimmed(), stLabelCalibrateProgress, 1);
				sleep(2);
				return exitCalibrate();
			}

			switch (i)
			{
				case 0:
					setZcCalibrateDistributeFrequencyRespond(j, -30, 0);
					break;
				case 1:
					setZcCalibrateDistributeFrequencyRespond(j, -20, 9);
					break;
				case 2:
					setZcCalibrateDistributeFrequencyRespond(j, -10, 21);
					break;
				case 3:
					setZcCalibrateDistributeFrequencyRespond(j, -10, 30);
					break;
				case 4:
					setZcCalibrateDistributeFrequencyRespond(j, 0, 39);
					break;

				default:
					continue;
			}

			if (getCalibrateRunParam() == __SCPI_FAILED)
			{
				saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("Low Freq Preamplifier calibrate failed"), getTheTranslatorString("frequency respond calibrate failed On") + " " + QString(getFreqAndUnitString(FREQRESPHZ_ZC[j], 0, freqChar)).trimmed());
				outputToScreen(getTheTranslatorString("frequency respond calibrate failed On") + " " + QString(getFreqAndUnitString(FREQRESPHZ_ZC[j], 0, freqChar)).trimmed(), stLabelCalibrateProgress, 1);
				sleep(2);
				return exitCalibrate();
			}

			sysData.zcPreamplifierCalData.freqResp[i][j] = rfInPower - calRunData.maxValue;
		}
	}
	saveZcPreamplifierFactoryCalibrationData();

	progress = 100;
	freqAndPowCmd = ":FREQ ";
	freqAndPowCmd += QString(floatToString(5e6, 0, 0, freqChar)).trimmed();
	sendSerialData(isCom, freqAndPowCmd);
	freqAndPowCmd = ":POW " + QString(intToString(-40, freqChar)).trimmed();
	sendSerialData(isCom, freqAndPowCmd);

	if (!isCom)
	{
		disConnectFromServerSocket();
	}

	saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("Low Freq Preamplifier calibrate successed"), "");
	outputToScreen(getTheTranslatorString("Low Freq Preamplifier calibrate successed"), progress, 0);
	sysData.isZcPreamplifierCalibrating = false;
	//  sysData.options.isFft10HzOn = prvFftState;
	sysData.mode = smSpectrum;
	sysData.canvasNums = 1;

	return __SCPI_SUCCESS;
}

//直采通道校准
int tSysScpi::ZCCalibrate(int comCal)
{
	bool isCom = (comCal >= 1);

	if (sysData.isZcCalibrating)
	{
		return __SCPI_FAILED;
	}

	if (!sysData.options.directChannelOptionOn)
	{
		printf("sysData.options.directChannelOptionOn\n");
		//return __SCPI_FAILED;
	}

	if (isCom && comHandle <= 0)
	{
		return __SCPI_FAILED;
	}

	int progress = 0;
	char tempChar[32] = { };
	QString datetime = "";

	getLocalDateTime(tempChar);
	datetime = tempChar;
	presetSystemData();
	sysData.freq.isLowChannel = true;
	setFrequencyOfLowChannel(sysData.freq.isLowChannel);
	controlRf();
	sysData.isZcCalibrating = true;
	sysData.mode = smCalibrate;
	sysData.canvasNums = 1;
	resetZcCalibrationData();
	resetUserCalibrationData();

	QCoreApplication::processEvents();//processes all pending events as  a long operation
	saveScreen();

	//1、help info
	progress = 0;
	//outputToScreen(getTheTranslatorString("Low Freq calibrating") + "......", progress, 0);

	//2、connect to server
	if (!isCom)
	{
		progress = 3;
		sysData.system.io.lan.hostPort = SOCKETSEVERPORT_ZC;
		strcpy(sysData.system.io.lan.hostIp, SOCKETSERVERIP_ZC);
		if (connecToServerSocket() == __SCPI_FAILED)
		{
			saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("connect to server failed"), getTheTranslatorString("disconnect to server"));
			//outputToScreen(getTheTranslatorString("disconnect to server"), progress, 1);
			sleep(2);
			return exitCalibrate();
		}
	}
	printf("signal generator connected!\n");

	//3、check signal generator
	progress = 5;
	//outputToScreen(getTheTranslatorString("signal generator checking") + "......", progress, 0);
	sendSerialData(isCom, "*RST");
	usleep(1000 * 3000);

	if (signalIsOnline(isCom) == __SCPI_FAILED)
	{
		saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("ZC calibrate failed"), getTheTranslatorString("signal generator is off line"));
		//outputToScreen(getTheTranslatorString("signal generator is off line"), progress, 1);
		sleep(2);
		return exitCalibrate();
	}
	printf("signal generator online!\n");
	sendSerialData(isCom, ":SYST:MODO OFF");
	sendSerialData(isCom, ":SYST:RFO ON");

	//5、check temperature
	double sysTemperature = 0;
	progress = 8;

	if (getSystemTemperature(&sysTemperature) == __SCPI_FAILED)
	{
		saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("Low Freq calibrate failed"), getTheTranslatorString("can not get device temperature"));
		//outputToScreen(getTheTranslatorString("can not get device temperature"), progress, 1);
		sleep(2);
		printf("getSystemTemperature failed!\n");
		return exitCalibrate();
	}

	//6、reset zc calibration data
	resetZcCalibrationData();
	sysData.zcCalData.isCaled = true;
	sysData.zcCalData.temperature = sysTemperature;

	//7、amplitude
	printf("absoluteAmptValue cal......\n");
	setZcCalibrateParamOfAbsoluteAmplitude();
	setFrontendAtt(10, 20);
	progress = 10;
	usleep(3000);
	//outputToScreen(getTheTranslatorString("amplitude calibrating") + "......", progress, 0);

	if (signalOutputFreqIsvalid(isCom, 10e6) == __SCPI_FAILED || signalOutputAmptIsvalid(isCom, -15) == __SCPI_FAILED || getCalibrateRunParam() == __SCPI_FAILED)
	{
		saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("Low Freq calibrate failed"), getTheTranslatorString("amplitude calibrate failed"));
		//outputToScreen(getTheTranslatorString("amplitude calibrate failed"), progress, 1);
		sleep(2);
		return exitCalibrate();
	}

	sysData.zcCalData.absoluteAmptValue = calRunData.maxValue + 15.0;
	saveZcFactoryCalibrationData();
	saveUserCalibrationData();
	printf("absoluteAmptValue cal succeed!\n");
#if 1	
	sleep(3);
	//8、attenuation
	progress = 15;
	setZcCalibrateParamOfAbsoluteAmplitude();
	setFrontendAtt(10, 20);
	//outputToScreen(getTheTranslatorString("attenuation calibrating") + "......", progress, 0);
	
	QString cmd = "";
	cmd = ":FREQ 10e6";
	sendSerialData(isCom, cmd);
	cmd = ":POW " + QString(intToString(-25, tempChar)).trimmed();
	sendSerialData(isCom, cmd);

	getCalibrateRunParam();
	double base = calRunData.maxValue;

	int  power = 0;
	double attRf = 0, attIf = 0;
	int freqRespCount = sizeof FREQRESPHZ_ZC / sizeof FREQRESPHZ_ZC[0];
	FILE* fp = fopen("zcdata.c","w");
	printf("ds freq res cal......");
	sleep(3);
	for(int i = 0; i < 16; i++)
	{
		switch (i)
		{
			case 0:
				attRf = 10;
				attIf = 20;
				power = -15;
				break;
			case 1:
				attRf = 0;
				attIf = 20;
				power = -25;
				break;
			case 2:
				attRf = 20;
				attIf = 20;
				power = -5;
				break;		
			case 3:
				attRf = 30;
				attIf = 20;
				power = 5;
				break;
			case 4: 
				attRf = 10;
				attIf = 10;
				power = -25;
				break;	
			case 5: 
				attRf = 0;
				attIf = 10;
				power = -35;
				break;	
			case 6: 
				attRf = 20;
				attIf = 10;
				power = -15;
				break;	
			case 7: 
				attRf = 30;
				attIf = 10;
				power = -5;
				break;	
			case 8: 
				attRf = 10;
				attIf = 0;
				power = -35;
				break;	
			case 9: 
				attRf = 0;
				attIf = 0;
				power = -45;
				break;	
			case 10: 
				attRf = 20;
				attIf = 0;
				power = -25;
				break;		
			case 11: 
				attRf = 30;
				attIf = 0;
				power = -15;
				break;		
			case 12: 
				attRf = 10;
				attIf = 30;
				power = -5;
				break;
			case 13: 
				attRf = 0;
				attIf = 30;
				power = -15;
				break;		
			case 14: 
				attRf = 20;
				attIf = 30;
				power = 5;
				break;		
			case 15: 
				attRf = 30;
				attIf = 30;
				power = 15;
				break;					

		}
		setFrontendAtt(attRf, attIf);

		cmd = ":POW " + QString(intToString(power, tempChar)).trimmed();
		sendSerialData(isCom, cmd);
		usleep(1000);
		
		for(int j = 0; j < freqRespCount; j++)
		{
			cmd = ":FREQ ";
			cmd += QString(floatToString(FREQRESPHZ_ZC[j], 0, 0, tempChar)).trimmed();
			sendSerialData(isCom, cmd);
			usleep(1000);

			// set ds param
			if(j < 5)
			{
				setFrequencyOfCenter(50e3);
			} else
			{
				setFrequencyOfCenter(FREQRESPHZ_ZC[j]);
			}
			setFrequencyOfSpan(100e3);
			setBwOfRbw(1e3);
			controlRf();
			usleep(5000);
			
			if(getCalibrateRunParam() == __SCPI_FAILED)
			{
				fprintf(fp,"attRf=%f,attIf=%f,power=%d,freq=%d failed\n",\
							attRf, attIf, power,FREQRESPHZ_ZC[j]);
				printf("*********getCalibrateRunParam failed!\n");
			}
	
			sysData.zcCalData.freqResp[i][j] = calRunData.maxValue -power;
		}
		printf("\nattRf=%f,attIf=%f,power=%d:ds freq res cal succeed!\n",attRf, attIf, power);
		sleep(3);

	}
	saveZcFactoryCalibrationData();
	fclose(fp);
#endif 
	progress = 100;

	if (!isCom)
	{
		disConnectFromServerSocket();
	}

	saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("Low Freq calibrate successed"), "");
	//outputToScreen(getTheTranslatorString("Low Freq calibrate successed"), progress, 0);
	sysData.isZcCalibrating = false;
	sysData.mode = smSpectrum;
	sysData.canvasNums = 1;	
#if 0
	//8.1、att9
	progress = 16;
	//	signalOutputAmptIsvalid(isCom, -20);
	//setCalibrateDistributeAtt(0, 0, 9);
	outputToScreen(getTheTranslatorString("attenuation(9dB) calibrating") + "......", progress, 0);

	if (getCalibrateRunParam() == __SCPI_FAILED)
	{
		saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("Low Freq calibrate failed"), getTheTranslatorString("attenuation(9dB) calibrate failed"));
		outputToScreen(getTheTranslatorString("attenuation(9dB) calibrate failed"), progress, 1);
		sleep(2);
		return exitCalibrate();
	}
	sysData.zcCalData.attBase = calRunData.maxValue;
	sysData.zcCalData.att9 = 0;

	//8.2、att0
	progress = 17;
	setCalibrateDistributeAtt(-10, 0, 0);
	outputToScreen(getTheTranslatorString("attenuation(0dB) calibrating") + "......", progress, 0);

	if (getCalibrateRunParam() == __SCPI_FAILED)
	{
		saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("Low Freq calibrate failed"), getTheTranslatorString("attenuation(0dB) calibrate failed"));
		outputToScreen(getTheTranslatorString("attenuation(0dB) calibrate failed"), progress, 1);
		sleep(2);
		return exitCalibrate();
	}
	sysData.zcCalData.att0 = calRunData.maxValue - sysData.zcCalData.attBase;

	//8.3、att21
	progress = 18;
	setCalibrateDistributeAtt(10, 0, 21);
	outputToScreen(getTheTranslatorString("attenuation(21dB) calibrating") + "......", progress, 0);

	if (getCalibrateRunParam() == __SCPI_FAILED)
	{
		saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("Low Freq calibrate failed"), getTheTranslatorString("attenuation(21dB) calibrate failed"));
		outputToScreen(getTheTranslatorString("attenuation(21dB) calibrate failed"), progress, 1);
		sleep(2);
		return exitCalibrate();
	}
	sysData.zcCalData.att21 = calRunData.maxValue - sysData.zcCalData.attBase;

	//8.4、att30
	progress = 19;
	setCalibrateDistributeAtt(10, 0, 30);
	outputToScreen(getTheTranslatorString("attenuation(30dB) calibrating") + "......", progress, 0);

	if (getCalibrateRunParam() == __SCPI_FAILED)
	{
		saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("Low Freq calibrate failed"), getTheTranslatorString("attenuation(30dB) calibrate failed"));
		outputToScreen(getTheTranslatorString("attenuation(30dB) calibrate failed"), progress, 1);
		sleep(2);
		return exitCalibrate();
	}
	sysData.zcCalData.att30 = calRunData.maxValue - sysData.zcCalData.attBase;

	//8.5、att39
	progress = 20;
	setCalibrateDistributeAtt(10, 0, 39);
	outputToScreen(getTheTranslatorString("attenuation(39dB) calibrating") + "......", progress, 0);

	if (getCalibrateRunParam() == __SCPI_FAILED)
	{
		saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("Low Freq calibrate failed"), getTheTranslatorString("attenuation(39dB) calibrate failed"));
		outputToScreen(getTheTranslatorString("attenuation(39dB) calibrate failed"), progress, 1);
		sleep(2);
		return exitCalibrate();
	}
	sysData.zcCalData.att39 = calRunData.maxValue - sysData.zcCalData.attBase;
	saveZcFactoryCalibrationData();

	//9、if
	progress = 21;
	signalOutputAmptIsvalid(isCom, -10);
	setZcCalibrateParamOfIF();
	outputToScreen(getTheTranslatorString("if calibrating") + "......", progress, 0);

	//9.1、10dB
	setZcCalibrateDistributeIF(10);
	outputToScreen(getTheTranslatorString("if(10dB) calibrating") + "......", progress, 0);

	if (getCalibrateRunParam() == __SCPI_FAILED)
	{
		saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("Low Freq calibrate failed"), getTheTranslatorString("if(10dB) calibrate failed"));
		outputToScreen(getTheTranslatorString("if(10dB) calibrate failed"), progress, 1);
		sleep(2);
		return exitCalibrate();
	}
	sysData.zcCalData.if10 = -20 - calRunData.maxValue;

	//9.2、0dB
	progress = 22;
	setZcCalibrateDistributeIF(0);
	outputToScreen(getTheTranslatorString("if(0dB) calibrating") + "......", progress, 0);

	if (getCalibrateRunParam() == __SCPI_FAILED)
	{
		saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("Low Freq calibrate failed"), getTheTranslatorString("if(0dB) calibrate failed"));
		outputToScreen(getTheTranslatorString("if(0dB) calibrate failed"), progress, 1);
		sleep(2);
		return exitCalibrate();
	}

	sysData.zcCalData.if0 = -30 - calRunData.maxValue;

	//9.3、20dB
	progress = 23;
	setZcCalibrateDistributeIF(20);
	outputToScreen(getTheTranslatorString("if(20dB) calibrating") + "......", progress, 0);

	if (getCalibrateRunParam() == __SCPI_FAILED)
	{
		saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("Low Freq calibrate failed"), getTheTranslatorString("if(20dB) calibrate failed"));
		outputToScreen(getTheTranslatorString("if(20dB) calibrate failed"), progress, 1);
		sleep(2);
		return exitCalibrate();
	}

	sysData.zcCalData.if20 = -10 - calRunData.maxValue;

	//9.4、30dB
	progress = 24;
	setZcCalibrateDistributeIF(30);
	outputToScreen(getTheTranslatorString("if(30dB) calibrating") + "......", progress, 0);

	if (getCalibrateRunParam() == __SCPI_FAILED)
	{
		saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("Low Freq calibrate failed"), getTheTranslatorString("if(30dB) calibrate failed"));
		outputToScreen(getTheTranslatorString("if(30dB) calibrate failed"), progress, 1);
		sleep(2);
		return exitCalibrate();
	}

	sysData.zcCalData.if30 = 0 - calRunData.maxValue;
	saveZcFactoryCalibrationData();

	//10、rbw
	progress = 25;
	setZcCalibrateParamOfRbw();
	outputToScreen(getTheTranslatorString("rbw calibrating") + "......", progress, 0);

	progress = 26;
/*	sysData.options.isFft10HzOn = true;

	char rbwChar[32] = { };
	for (int i = 0; i < sizeof RAMCIC_ZC_HZ / sizeof RAMCIC_ZC_HZ[0]; i++)
	{
		if (RAMCIC_ZC_HZ[i] <= 20)
		{
			setFrequencyOfSpan(1e3);
		}

		setBwOfRbw(RAMCIC_ZC_HZ[i]);
		outputToScreen(getTheTranslatorString("rbw calibrating") + "(" + QString(getBwAndUnitString(RAMCIC_ZC_HZ[i], 0, rbwChar)).trimmed() + ")......", progress, 0);

		if (getCalibrateRunParam() == __SCPI_FAILED)
		{
			saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("Low Freq calibrate failed"), getTheTranslatorString("rbw calibrate failed On") + QString(getBwAndUnitString(RAMCIC_ZC_HZ[i], 0, rbwChar)).trimmed());
			outputToScreen(getTheTranslatorString("rbw(10kHz) calibrate failed"), progress, 1);
			sleep(2);
			return exitCalibrate();
		}
		sysData.zcCalData.rbw[i] = -20 - calRunData.maxValue;
	}
*/
	  setBwOfRbw(10e3);
	  setFrequencyOfSpan(10e3 * 100);
	  controlRf();
	  outputToScreen(getTheTranslatorString("rbw(10kHz) calibrating") + "......", progress, 0);

	  if (getCalibrateRunParam() == __SCPI_FAILED)
	  {
	    saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("ZC calibrate failed"), getTheTranslatorString("rbw(10kHz) calibrate failed"));
	    outputToScreen(getTheTranslatorString("rbw(10kHz) calibrate failed"), progress, 1);
	    sleep(2);
	    return exitCalibrate();
	  }
	  sysData.zcCalData.rbw10k = calRunData.maxValue;

	  for (int i = 0; i < sizeof BWRESP_ZC /sizeof BWRESP_ZC[0]; i++)
	  {
	    outputToScreen(getTheTranslatorString("rbw calibrating") + "(" + ((QString)getFreqAndUnitString(RAMCIC_ZC_HZ[i], 0, tempChar)).trimmed() + ")" + "......", progress, 0);

	    if (i == 0)
	    {
	    	sysData.zcCalData.rbw[i] = 0;
	    } else
	    {
	    	setBwOfRbw(RAMCIC_ZC_HZ[i]);
	    	setFrequencyOfSpan(RAMCIC_ZC_HZ[i] * 100);
	    	controlRf();
	    	if (getCalibrateRunParam() == __SCPI_SUCCESS)
	    	{
	    		sysData.zcCalData.rbw[i] = (calRunData.maxValue - sysData.zcCalData.rbw10k) * -1.0;
	    	} else
	    	{
	    		return exitCalibrate();
	    	}
	    }

		//sysData.zcCalData.rbw[i] = BWRESP_ZC[i];
	}

	saveZcFactoryCalibrationData();

	//11、resp
	QString freqAndPowCmd = "";
	char freqChar[32] = { };
	int rfInPower = 0;
	int freqRespCount = sizeof FREQRESPHZ_ZC / sizeof FREQRESPHZ_ZC[0];
	int freqRespSum = 5 * freqRespCount;

	progress = 30;
	setZcCalibrateParamOfFrequencyRespond();
	outputToScreen(getTheTranslatorString("frequency respond calibrating") + "......", progress, 0);

	for (int i = 0; i < 5; i++)
	{
		switch (i)
		{
			case 0:
				rfInPower = -20;
				break;
			case 1:
				rfInPower = -20;
				break;
			case 2:
				rfInPower = 0;
				break;
			case 3:
				rfInPower = 0;
				break;
			case 4:
				rfInPower = 0;
				break;
		}

		freqAndPowCmd = ":POW " + QString(intToString(rfInPower, freqChar)).trimmed();
		sendSerialData(isCom, freqAndPowCmd);

		setZcCalibrateParamOfFrequencyRespond();
		for (int j = 0; j < freqRespCount; j++)
		{
			stLabelCalibrateProgress = progress + (i * freqRespCount + j) * 70 / freqRespSum;
			outputToScreen(getTheTranslatorString("frequency respond calibrating") + "(" + QString(getFreqAndUnitString(FREQRESPHZ_ZC[j], 0, freqChar)).trimmed() + ")......", stLabelCalibrateProgress, 0);

			if (signalOutputFreqIsvalid(isCom, FREQRESPHZ_ZC[j]) == __SCPI_FAILED)
			{
				saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("Low Freq calibrate failed"), getTheTranslatorString("frequency respond calibrate failed On") + " " + QString(getFreqAndUnitString(FREQRESPHZ_ZC[j], 0, freqChar)).trimmed());
				outputToScreen(getTheTranslatorString("frequency respond calibrate failed On") + " " + QString(getFreqAndUnitString(FREQRESPHZ_ZC[j], 0, freqChar)).trimmed(), stLabelCalibrateProgress, 1);
				sleep(2);
				return exitCalibrate();
			}

			switch (i)
			{
				case 0:
					setZcCalibrateDistributeFrequencyRespond(j, -10, 0);
					break;
				case 1:
					setZcCalibrateDistributeFrequencyRespond(j, 0, 9);
					break;
				case 2:
					setZcCalibrateDistributeFrequencyRespond(j, 10, 21);
					break;
				case 3:
					setZcCalibrateDistributeFrequencyRespond(j, 10, 30);
					break;
				case 4:
					setZcCalibrateDistributeFrequencyRespond(j, 20, 39);
					break;

				default:
					continue;
			}

			if (getCalibrateRunParam() == __SCPI_FAILED)
			{
				saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("Low Freq calibrate failed"), getTheTranslatorString("frequency respond calibrate failed On") + " " + QString(getFreqAndUnitString(FREQRESPHZ_ZC[j], 0, freqChar)).trimmed());
				outputToScreen(getTheTranslatorString("frequency respond calibrate failed On") + " " + QString(getFreqAndUnitString(FREQRESPHZ_ZC[j], 0, freqChar)).trimmed(), stLabelCalibrateProgress, 1);
				sleep(2);
				return exitCalibrate();
			}

			sysData.zcCalData.freqResp[i][j] = rfInPower - calRunData.maxValue;
		}
	}
	saveZcFactoryCalibrationData();

	progress = 100;

	freqAndPowCmd = ":FREQ ";
	freqAndPowCmd += QString(floatToString(5e6, 0, 0, freqChar)).trimmed();
	sendSerialData(isCom, freqAndPowCmd);
	freqAndPowCmd = ":POW " + QString(intToString(-20, freqChar)).trimmed();
	sendSerialData(isCom, freqAndPowCmd);

	if (!isCom)
	{
		disConnectFromServerSocket();
	}

	saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("Low Freq calibrate successed"), "");
	outputToScreen(getTheTranslatorString("Low Freq calibrate successed"), progress, 0);
	sysData.isZcCalibrating = false;
	//  sysData.options.isFft10HzOn = prvFftState;
	sysData.mode = smSpectrum;
	sysData.canvasNums = 1;
#endif

	return __SCPI_SUCCESS;
}

//用户校准
int tSysScpi::userCalibrate(void)
{
	if (sysData.isZcCalibrating || sysData.isZcPreamplifierCalibrating || sysData.isFactoryCalibrating || sysData.isPreamplifierCalibrating || sysData.isUserCalibrating)
	{
		return __SCPI_FAILED;
	}

	int progress = 0;
	char tempChar[32] = { };
	QString datetime = "";
	double if10dBValue = 0;
	double if0dBValue = 0;
	double if20dBValue = 0;
	double if30dBValue = 0;

	getLocalDateTime(tempChar);
	datetime = tempChar;
	presetSystemData();
	//sysData.system.isInternalSignalOn = true;
	controlRf();

	setDetector(dmPosPeak);
	setFrequencyOfCenter(440e6);
	setFrequencyOfSpan(10e6);
	setFrequencyOfStart(435e6);
	setFrequencyOfStop(445e6);
	controlRf();

	sysData.isUserCalibrating = true;
	sysData.mode = smCalibrate;
	sysData.canvasNums = 1;
	QCoreApplication::processEvents();//processes all pending events as  a long operation
	saveScreen();
	memset(&sysData.userCalData, 0, sizeof sysData.userCalData);

	//1、help info
	progress = 0;
	outputToScreen(getTheTranslatorString("system auto calibrating") + "......", progress, 0);

	//2、check internal signal
	progress = 10;
	if (getCalibrateRunParam() == __SCPI_FAILED || calRunData.maxIndex < (sysData.sweep.sweepPoints - 1) / 2 - 10 || calRunData.maxIndex > (sysData.sweep.sweepPoints - 1) / 2 + 10)
	{
		saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("system calibrate failed"), getTheTranslatorString("not found internal signal") + "(440MHz)");
		outputToScreen(getTheTranslatorString("not found internal signal") + "(440MHz)", progress, 1);
		exitCalibrate();
	}
	sysData.userCalData.absError = -20 - calRunData.maxValue;

	//3、get system temperature
	double sysTemperature = 0;
	progress = 20;
	if (getSystemTemperature(&sysTemperature) == __SCPI_FAILED)
	{
		saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("system calibrate failed"), getTheTranslatorString("can not get device temperature"));
		outputToScreen(getTheTranslatorString("can not get device temperature"), progress, 1);
		return exitCalibrate();
	}
	sysData.userCalData.temperature = sysTemperature;

	//4、if 10dB
	setCalibrateDistributeIF(10);
	progress = 30;
	outputToScreen(getTheTranslatorString("if(10dB) calibrating") + "......", progress, 0);
	if (getCalibrateRunParam() == __SCPI_FAILED)
	{
		saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("system calibrate failed"), getTheTranslatorString("if(10dB) calibrate failed"));
		outputToScreen(getTheTranslatorString("if(10dB) calibrate failed"), progress, 1);
		return exitCalibrate();
	}
	if10dBValue = -20 - calRunData.maxValue;

	//4.1 if 0dB
	setCalibrateDistributeIF(0);
	progress = 50;
	outputToScreen(getTheTranslatorString("if(0dB) calibrating") + "......", progress, 0);
	if (getCalibrateRunParam() == __SCPI_FAILED)
	{
		saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("system calibrate failed"), getTheTranslatorString("if(0dB) calibrate failed"));
		outputToScreen(getTheTranslatorString("if(0dB) calibrate failed"), progress, 1);
		return exitCalibrate();
	}
	if0dBValue = -30 - calRunData.maxValue;

	//4.2 if 20dB
	setCalibrateDistributeIF(20);
	progress = 70;
	outputToScreen(getTheTranslatorString("if(20dB) calibrating") + "......", progress, 0);
	if (getCalibrateRunParam() == __SCPI_FAILED)
	{
		saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("system calibrate failed"), getTheTranslatorString("if(20dB) calibrate failed"));
		outputToScreen(getTheTranslatorString("if(20dB) calibrate failed"), progress, 1);
		return exitCalibrate();
	}
	if20dBValue = -10 - calRunData.maxValue;

	//4.3 if 30dB
	setCalibrateDistributeIF(30);
	progress = 90;
	outputToScreen(getTheTranslatorString("if(30dB) calibrating") + "......", progress, 0);
	if (getCalibrateRunParam() == __SCPI_FAILED)
	{
		saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("system calibrate failed"), getTheTranslatorString("if(30dB) calibrate failed"));
		outputToScreen(getTheTranslatorString("if(30dB) calibrate failed"), progress, 1);
		return exitCalibrate();
	}
	if30dBValue = 0 - calRunData.maxValue;

	sysData.userCalData.if10dB = if10dBValue;
	sysData.userCalData.if0dB = if0dBValue;
	sysData.userCalData.if20dB = if20dBValue;
	sysData.userCalData.if30dB = if30dBValue;
	saveUserCalibrationData();

	progress = 100;
	saveLogOfFactoryCalibrate(datetime, getTheTranslatorString("system calibrate successed"), "");
	outputToScreen(getTheTranslatorString("system calibrate successed"), progress, 0);
	sysData.isUserCalibrating = false;
	sysData.mode = smSpectrum;
	sysData.canvasNums = 1;

	return __SCPI_SUCCESS;
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
	sysData.scpiData.gpibOpened = false;

	unsigned char buf[SCPIBUFSIZE] = { };
	unsigned int length = 0;

	memset(buf, 0, sizeof buf);
	length = read(comHandle, buf, sizeof buf);

	if (length > 0)
	{
		if (sysData.scpiData.stateT)
		{
			sysData.scpiData.stateT = false;
		}

		for (int i = 0; i < TRACECOUNT; i++)
		{
			traceDataString[i] = "";
			traceDataOrganized[i] = false;
		}

		sysData.scpiData.stateS = true;
		memcpy(sysData.scpiData.buf, buf, sizeof(char) * length);
		sysData.scpiData.rcvLength = length;

		if (!sysData.isZcPreamplifierCalibrating && !sysData.isZcCalibrating && !sysData.isFactoryCalibrating && !sysData.isPreamplifierCalibrating)
		{
			if (sysData.scpiData.interface != sitCom)
			{
				sysData.scpiData.interface = sitCom;
			}

			//      emit refreshButtonState();
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
void tSysScpi::readUsbSlaveData()
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
	sysData.scpiData.gpibOpened = false;

	int length = 0;
	int emptyNums = 0;
	char buf[2048] = { 0x0 };

	memset(buf, 0, sizeof buf);
	length = read(usbHandle, buf, 64);

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
			//	  emit refreshButtonState();
		}

		if (sysData.scpiData.stateT)
		{
			sysData.scpiData.stateT = false;
		}

		sysData.scpiData.stateS = true;
		memcpy(sysData.scpiData.buf, buf, sizeof(char) * length);
		sysData.scpiData.rcvLength = length;

		if (length <= 1024)
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
		emit refreshScpiState(getTheTranslatorString("switch to local control mode"));
		//    emit refreshButtonState();
	}
}

//读GPIB数据
void tSysScpi::readGpibData()
{
	if (gpibHandle <= 0)
	{
		return;
	}

	char buf[8192] = { 0x0 };

	memset(buf, 0, sizeof buf);

	int length = read(gpibHandle, buf, sizeof buf);

	if (length <= 0)
	{
		return;
	}

	if (!sysData.scpiData.gpibOpened)
	{
		sysData.scpiData.gpibOpened = true;
	}

	if (sysData.scpiData.interface != sitGpib)
	{
		sysData.scpiData.interface = sitGpib;
	}

	if (!sysData.scpiData.stateR)
	{
		sysData.scpiData.stateR = true;
		sysData.scpiData.stateL = false;
		emit refreshScpiState(getTheTranslatorString("switch to remote control mode"));
		//	emit refreshButtonState();
	}

	handleScpiCommand(scpiBuffer + QString((char *) buf) + ";");
	scpiBuffer = "";
}

//连接到服务器
bool tSysScpi::connecToServerSocket(void)
{
	disConnectFromServerSocket();
	printf("signal generator ip:%s;port:%d\n",sysData.system.io.lan.hostIp,sysData.system.io.lan.hostPort);
	cSocket->connectToHost(sysData.system.io.lan.hostIp, sysData.system.io.lan.hostPort, QTcpSocket::ReadWrite);

	if (cSocket->waitForConnected(1000))
	{
		sysData.scpiData.stateT = true;
		sysData.scpiData.stateS = true;
		return __SCPI_SUCCESS;
	} else
	{
		return __SCPI_FAILED;
	}
}

//断开服务器
bool tSysScpi::disConnectFromServerSocket(void)
{
	cSocket->disconnectFromHost();

	sysData.scpiData.stateR = false;
	sysData.scpiData.stateL = true;
	sysData.scpiData.stateT = false;
	sysData.scpiData.stateS = false;
}

//接受客户端建立连接
void tSysScpi::acceptClientSocketConnect(void)
{
	QTcpSocket* client;
	int index = 0;
	int maxTimeOutIndex = 0;
	unsigned long long maxTimeOutSeconds = 0;
	unsigned long long maxTimeOutSeconds2 = 0;

	while (serverSocket->hasPendingConnections())
	{
		index = -1;
		client = serverSocket->nextPendingConnection();

		for (int i = 0; i < MAXTCPCLIENTS; i++)
		{
			if (clientSockets[i].socket == NULL)
			{
				index = i;
				break;
			}
		}

		if (index == -1)
		{
			maxTimeOutIndex = 0;
			maxTimeOutSeconds = clientSockets[maxTimeOutIndex].sendTime.secsTo(clientSockets[maxTimeOutIndex].recvTime);

			for (int i = 1; i < MAXTCPCLIENTS; i++)
			{
				maxTimeOutSeconds2 = clientSockets[i].sendTime.secsTo(clientSockets[i].recvTime);

				if (maxTimeOutSeconds2 > maxTimeOutSeconds)
				{
					maxTimeOutIndex = i;
					maxTimeOutSeconds = maxTimeOutSeconds2;
				}
			}

			index = maxTimeOutIndex;
		}

		if (index >= 0 && index < MAXTCPCLIENTS)
		{
			if (clientSockets[index].socket != NULL)
			{
				disconnect(clientSockets[index].socket, SIGNAL(disconnected()), this, SLOT(clientSocketDisConnect()));
				disconnect(clientSockets[index].socket, SIGNAL(readyRead()), this, SLOT(clientSocketReadyRead()));
				disconnect(clientSockets[index].socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(clientSocketError(QAbstractSocket::SocketError)));
				clientSockets[index].socket->close();
				clientSockets[index].socket->deleteLater();
				clientSockets[index].socket = NULL;
			}

			clientSockets[index].socket = client;
			clientSockets[index].isReading = false;
			clientSockets[index].isWriting = false;
			clientSockets[index].sendTime = QDateTime::currentDateTime();
			clientSockets[index].recvTime = QDateTime::currentDateTime();
			clientSockets[index].socket->readAll();

			if (clientSockets[index].socket->readBufferSize() != SOCKETBUFSIZE)
			{
				clientSockets[index].socket->setReadBufferSize(SOCKETBUFSIZE);
			}
			connect(clientSockets[index].socket, SIGNAL(disconnected()), this, SLOT(clientSocketDisConnect()));
			connect(clientSockets[index].socket, SIGNAL(readyRead()), this, SLOT(clientSocketReadyRead()));
			connect(clientSockets[index].socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(clientSocketError(QAbstractSocket::SocketError)));

			sysData.scpiData.stateR = true;
			sysData.scpiData.stateL = false;
			sysData.scpiData.interface = sitLan;
			emit refreshScpiState(getTheTranslatorString("switch to remote control mode"));
			//emit refreshButtonState();
		}
	}

	/*
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
	 */
}

//客户端断开连接
void tSysScpi::clientSocketDisConnect(void)
{
	for (int i = 0; i < MAXTCPCLIENTS; i++)
	{
		if (clientSockets[i].socket != NULL && clientSockets[i].socket->state() == QAbstractSocket::UnconnectedState)
		{
			disconnect(clientSockets[i].socket, SIGNAL(disconnected()), this, SLOT(clientSocketDisConnect()));
			disconnect(clientSockets[i].socket, SIGNAL(readyRead()), this, SLOT(clientSocketReadyRead()));
			disconnect(clientSockets[i].socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(clientSocketError(QAbstractSocket::SocketError)));
			clientSockets[i].socket->close();
			clientSockets[i].socket->deleteLater();
			clientSockets[i].socket = NULL;
			clientSockets[i].isReading = false;
			clientSockets[i].isWriting = false;
		}
	}

	bool allDisconnected = true;
	for (int i = 0; i < MAXTCPCLIENTS; i++)
	{
		if (clientSockets[i].socket != NULL && clientSockets[i].socket->state() != QAbstractSocket::UnconnectedState)
		{
			allDisconnected = false;
			break;
		}
	}

	if (allDisconnected && sysData.scpiData.stateR)
	{
		sysData.scpiData.stateR = false;
		sysData.scpiData.stateL = true;
		sysData.scpiData.interface = sitNone;
		emit refreshScpiState(getTheTranslatorString("switch to local control mode"));
	}
}

void tSysScpi::clientSocketDataArrive(int fd, int len)
{
	if (fd < 0 || len <= 0)
	{
		return;
	}

	curSocket = fd;
	readBuf.clear();
	readBuf.append(sockBuf, len);

	for (int i = 0; i < TRACECOUNT; i++)
	{
		traceDataString[i] = "";
		traceDataOrganized[i] = false;
	}

	if (!sysData.scpiData.stateR)
	{
		sysData.scpiData.stateR = true;
		sysData.scpiData.stateL = false;
		emit refreshScpiState(getTheTranslatorString("switch to local control mode"));
	}

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
			//  totalRcved += readBuf.size();    //暂时去掉
			FILE* fp = fopen(updatingFileName.toStdString().c_str(), "ab");

			if (fp != NULL)
			{
				fwrite(readBuf, readBuf.size(), 1, fp);
				fclose(fp);
			}
		}
	} else
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

//客户端有数据到达
void tSysScpi::clientSocketReadyRead(void)
{
	for (int i = 0; i < TRACECOUNT; i++)
	{
		traceDataString[i] = "";
		traceDataOrganized[i] = false;
	}

	for (int i = 0; i < MAXTCPCLIENTS; i++)
		if (clientSockets[i].socket != NULL)
		{
			qint64 len = 0;
			len = clientSockets[i].socket->bytesAvailable();

			if (len < 1)
				continue;

			if (sysData.scpiData.interface != sitLan)
			{
				sysData.scpiData.interface = sitLan;
			}

			if (!sysData.scpiData.stateR)
			{
				sysData.scpiData.stateR = true;
				sysData.scpiData.stateL = false;
				emit refreshScpiState(getTheTranslatorString("switch to local control mode"));
				//	  emit refreshButtonState();
			}

			sysData.scpiData.stateS = true;
			readBuf = clientSockets[i].socket->readAll();
			sysData.scpiData.stateS = false;
			clientSockets[i].isWriting = true;

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
			} else
			{
				if (readBuf.length() <= 1024)
				{
					handleScpiCommand(scpiBuffer + (QString) readBuf);
				} else
				{
					scpiBuffer = "";
				}
			}

			clientSockets[i].isWriting = false;
		}

	/*
	 qint64 len = 0;

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
	 handleScpiCommand(scpiBuffer + (QString)readBuf);
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
	 handleScpiCommand(scpiBuffer + (QString)readBuf);
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
	 } else
	 {
	 if (readBuf.length() <= 1024)
	 {
	 handleScpiCommand(scpiBuffer + (QString)readBuf);
	 } else
	 {
	 scpiBuffer = "";
	 }
	 }
	 }
	 */
}

//客户端发生错误
void tSysScpi::clientSocketError(QAbstractSocket::SocketError)
{
	scpiBuffer = "";
	for (int i = 0; i < MAXTCPCLIENTS; i++)
	{

	}

	/*
	 scpiBuffer = "";

	 if (clientSocket != NULL)
	 {
	 clientSocketDisConnect();
	 emit refreshScpiState(getTheTranslatorString("switch to local control mode"));
	 }
	 */
}

//刷新菜单面板
void tSysScpi::reDrawMenuFrame(void)
{
	unsigned long mCode = 0;
	unsigned long sCode = 0;

	if (sysData.system.isScpiScreenRefresh)
	{
		if (nowMainKey != NULL && nowSubKey != NULL)
		{
			mCode = nowMainKey->keyValue;
			sCode = nowSubKey->funcCode;
		}

		emit refreshMenuFrame(mCode, sCode);
	}
}

//执行体
void tSocketThread::run()
{
//	sigset_t set;
//	sigemptyset(&set);
//	sigaddset(&set,SIGINT);
//	sigprocmask(SIG_SETMASK, &set, NULL);

  int listenport = LISTENPORT;
  int serverSocket, connfd, kdpfd, nfds, n, nread, curfds,acceptCount = 0;
  struct sockaddr_in servaddr, cliaddr;
  socklen_t socklen = sizeof(struct sockaddr_in);
  struct epoll_event ev;
  struct epoll_event events[MAXTCPCLIENTS];
  char buf[1024]={0};
  int clientfd[MAXTCPCLIENTS] = {0};

  bzero(&servaddr, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl (INADDR_ANY);
  servaddr.sin_port = htons (listenport);

  serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (serverSocket == -1) {
      perror("can't create socket file");
      return ;
  }

  int opt = 1;
  setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));//reuse local addr and port
  int netTimeout = 2000; //2秒
  setsockopt(serverSocket, SOL_SOCKET, SO_SNDTIMEO, (char *) &netTimeout, sizeof(int));//sent timeout
  setsockopt(serverSocket, SOL_SOCKET, SO_RCVTIMEO, (char *) &netTimeout, sizeof(int));//rcv timeout
  int bufferSize = 1024 * 1024;
  setsockopt(serverSocket, SOL_SOCKET, SO_RCVBUF, (char *) &bufferSize, sizeof(int));//rcv buffer

  if (fcntl(serverSocket, F_SETFL, fcntl(serverSocket, F_GETFD, 0)|O_NONBLOCK) < 0) {
      perror("setnonblock error");//set file to nonblock
  }

  if (bind(serverSocket, (struct sockaddr *) &servaddr, sizeof(struct sockaddr)) == -1)
  {
      perror("bind error");//bind socket file discription and ip
      return;
  }
  if (listen(serverSocket, MAXTCPCLIENTS) == -1)
  {
      perror("listen error");
      return ;
  }
  /* 创建 epoll 句柄，把监听 socket 加入到 epoll 集合里 */
  kdpfd = epoll_create(MAXTCPCLIENTS);
  ev.events = EPOLLIN | EPOLLET;
  ev.data.fd = serverSocket;
  if (epoll_ctl(kdpfd, EPOLL_CTL_ADD, serverSocket, &ev) < 0)
  {
      fprintf(stderr, "epoll set insertion error: fd=%d\n", serverSocket);
      return ;
  }
  curfds = 1;


  for (;;) {
      /* 等待有事件发生 */
      nfds = epoll_wait(kdpfd, events, curfds, -1);
      if (nfds == -1)
      {
          perror("epoll_wait");
          continue;
      }

      /* 处理所有事件 */
      for (n = 0; n < nfds; ++n)
      {
          if (events[n].data.fd == serverSocket)
          {
              connfd = accept(serverSocket, (struct sockaddr *)&cliaddr,&socklen);
              if (connfd < 0)
              {
                  perror("accept error");
                  continue;
              }

              sprintf(buf, "accept form %s:%d, connfd %d\r\n", inet_ntoa(cliaddr.sin_addr), cliaddr.sin_port,connfd);
              if (curfds > MAXTCPCLIENTS) {
                  close(clientfd[0]);
                  epoll_ctl(kdpfd, EPOLL_CTL_DEL, clientfd[0],&ev);
                  for(int i = 0; i < MAXTCPCLIENTS - 1; ++i)
                  {
                  	clientfd[i] = clientfd[i+1];
                  }
                  clientfd[MAXTCPCLIENTS - 1] = connfd;
                  curfds--;
               //   continue;
              }
              else
              	clientfd[curfds - 1] = connfd;

              if (fcntl(connfd, F_SETFL, fcntl(connfd, F_GETFD, 0)|O_NONBLOCK) < 0) {
                  perror("setnonblocking error");
              }
              ev.events = EPOLLIN | EPOLLET;
              ev.data.fd = connfd;
              if (epoll_ctl(kdpfd, EPOLL_CTL_ADD, connfd, &ev) < 0)
              {
                  fprintf(stderr, "add socket '%d' to epoll failed: %s\n", connfd, strerror(errno));
                  return;
              }

//              for(int i = 0; i < MAXTCPCLIENTS; i++)
//              {
//              }
              sysData.scpiData.interface = sitLan;
							sysData.scpiData.stateR = true;
							sysData.scpiData.stateL = false;

              curfds++;
              continue;
          }

          int ret = read(events[n].data.fd, sockBuf, sizeof sockBuf);//读取客户端socket流
          if (ret == 0) {
              close(events[n].data.fd);
              epoll_ctl(kdpfd, EPOLL_CTL_DEL, events[n].data.fd,&ev);
              int count = 0;
              for(int i = 0; i < MAXTCPCLIENTS; i++)
              {
              	if(clientfd[i] == events[n].data.fd)
              	{
              		count = i;
              		break;
              	}
              }
              for(int i = count; i < MAXTCPCLIENTS - 1; i++)
              {
              	clientfd[i] = clientfd[i + 1];
              }
              curfds--;

              if(curfds == 1)
              {
								sysData.scpiData.stateR = false;
								sysData.scpiData.stateL = true;
								sysData.scpiData.interface = sitNone;
              }
          } else if (ret < 0) {
              perror("read error");
              close(events[n].data.fd);
          } else {
          	/* 处理socket数据*/
          	memset(&sockBuf[ret], '\0', 1);
          	emit clientSocketDataArrive(events[n].data.fd, ret);
          }
      }
  }
  close(serverSocket);
  return ;
}


/*FS 模式扫描参数计算*/
void tSysScpi::updateScanParam(void)
{
	if(sysData.bw.rbw >= 1e3)
	{
		sysData.fscan.mode = fsSweep;

		int index = getRecentSweepRbwIndex(sysData.bw.rbw);
		double f_step = 0;
		sysData.fscan.cic = sweepDataList[index].cic;
		sysData.fscan.holdTime = sweepDataList[index].holdTime;
		sysData.fscan.Index = (sweepDataList[index].Index << 3) & 0x78;

		rfData.DBUC_FREQ = (sysData.freq.stop - sysData.freq.start) / 800;


		if(rfData.DBUC_FREQ >= sysData.bw.rbw)
		{
			if(sysData.bw.rbw >= 500e3)
				f_step = 100e3;
			else
				f_step = sysData.bw.rbw / 4;
		} 
		else
		{
			if(rfData.DBUC_FREQ >= 500e3)
			{
				f_step = 100e3;
			} else
			{
				f_step = rfData.DBUC_FREQ / 4;
			}
		}


		if(sysData.freq.stop <= 20e6)
			sysData.fscan.step = f_step;
		else
			sysData.fscan.step = round(pow2_24 * f_step / 400e6) * 400e6 / pow(2, 24);/*correct precision*/

		if(sysData.fscan.step == 0)
			sysData.fscan.step=1 * 400e6 / pow2_24;//prevent sysData.fscan.step=0 


	} else
	{
		sysData.fscan.mode = fsFFT;

		int index = getRecentFFTRbwIndex(sysData.bw.rbw);
		sysData.fscan.fft = fftBandList[index].fft;
		sysData.fscan.Index= fftBandList[index].Index & 0x07;
		sysData.fscan.cic = fftBandList[index].cic;
		sysData.fscan.step =  fftBandList[index].fstep;

	}

	if(sysData.span.isZeroSpan)
	{
		int index = getRecentSweepRbwIndex(sysData.bw.rbw);
		sysData.fscan.cic = sweepDataList[index].cic;
		sysData.fscan.sampleTime_ZeroSpan = sweepDataList[index].sampleTime;
		sysData.fscan.ZeroSpan_dataspeed = sweepDataList[index].dataspeed;
	}
	
	//index of rbw 
	feDownload(13, sysData.fscan.Index);


}

void tSysScpi::cicDown(void)
{
	int shift = 0;

	// cic reset, then configure cic
	feDownload(10, 1);
	feDownload(10, 0);

	//cic data
	feDownload(8, sysData.fscan.cic);

	shift = ceil(5.0 * log10(double(sysData.fscan.cic)) / log10(2));
	//cic shift
	feDownload(11, shift);

	//cic enable
	feDownload(9, 1);
	feDownload(9, 0);

}

int tSysScpi::setBwOfRbw(double value)
{
	setBwOfRbwAuto(0);

	double tempValue = value;

	if (tempValue > RAMCIC_HZ[0])
	{
		tempValue = RAMCIC_HZ[0];
	} else if (tempValue < RAMCIC_HZ[sizeof RAMCIC_HZ / sizeof RAMCIC_HZ[0] - 1])
	{
		tempValue = RAMCIC_HZ[sizeof RAMCIC_HZ / sizeof RAMCIC_HZ[0] - 1];
	} else
	{
		tempValue = getRecentBw(tempValue);
	}
	

	sysData.bw.rbw = tempValue;
	if (sysData.bw.vbwAuto)
	{
		sysData.bw.vbw = sysData.bw.rbw;
	
		if ((unsigned long) sysData.bw.rbw == 120e3)
		{
			sysData.bw.vbw = 100e3;
		}
	}
	updateScanParam();
	controlRf();
	return __SCPI_SUCCESS;
//	int index = getRecentScanRbwIndex(value);
//
//	if (index >= 0)
//	{
//		sysData.bw.rbw = fftBandList[index].rbw;
//		updateScanParam(sysData.bw.rbw);
//		controlRf();
//	//	printf("set rbw %f\r\n",sysData.bw.rbw);
//		return __SCPI_SUCCESS;
//	} else
//	{
//		return __SCPI_FAILED;
//	}

}

int tSysScpi::setBwOfRbw(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	double tempValue = 0;

	tempValue = getFreqValue(value, &isOk);

	if (isOk)
	{
		return setBwOfRbw(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

void tSysScpi::vbwDown(void)
{
	unsigned int vbwCvc = 0;
	vbwCvc = getVbwFactor(sysData.bw.vbw);
	// 测试用
	vbwCvc = 0x7fff;
	feDownload(26, vbwCvc);
}

//void tSysScpi::updateScanParam(int index)
//{
//	sysData.bw.rbw = pscanBandList[index].rbw;
//	sysData.fscan.fft = pscanBandList[index].fft;
//	sysData.fscan.fftval = pscanBandList[index].fftVal;
//	sysData.fscan.cic = pscanBandList[index].cic;
//	sysData.fscan.step =  pscanBandList[index].fstep;
//
//	if(sysData.bw.rbw >= 1e6)
//	{
//		sysData.fscan.mode = fsSweep;
//		rfData.DBUC_FREQ = (sysData.freq.stop - sysData.freq.start) / (RAMDATASIZE - 1);
//
//		if(rfData.DBUC_FREQ < 250e3)
//		{
//			sysData.fscan.step = rfData.DBUC_FREQ / 5;
//		} else
//		{
//			sysData.fscan.step = rfData.DBUC_FREQ / ceil(rfData.DBUC_FREQ / 50000);
//		}
//
//
//	} else
//	{
//		sysData.fscan.mode = fsFFT;
//	}
//}

//计算扫描时间 // 扫描时间耦合
void tSysScpi::calSweepTimeOfAuto(void)
{
	double sweepTime_NL = 0, sweepTime_HL = 0, sweepTime_HH = 0, sweepTime_HH4 = 0;
	double Ts = 0;
	double kr = 0;
	if(sysData.fscan.mode == fsFFT)
	{
		sysData.fscan.ffHoldTime =  2e3;//us
		sysData.fscan.ofHoldTime = 0.2e3;//us
		if (sysData.sweep.sweepTimeAuto == true){
			sweepTime_NL = sysData.fscan.ofHoldTime + rfData.N_L * (102.4 * sysData.fscan.fft / sysData.fscan.cic + 6 / 25.6);
			sweepTime_HL = sysData.fscan.ffHoldTime + sysData.fscan.ofHoldTime * (rfData.N_HL - 1) + rfData.N_HL * (102.4 * sysData.fscan.fft / sysData.fscan.cic + 78 / 25.6);
			sweepTime_HH = sysData.fscan.ffHoldTime + sysData.fscan.ofHoldTime * (rfData.N_HH - 1) + rfData.N_HH * (102.4 * sysData.fscan.fft / sysData.fscan.cic + 86 / 25.6);
			sweepTime_HH4 = sysData.fscan.ffHoldTime + sysData.fscan.ofHoldTime * (rfData.N_HH4 - 1) + rfData.N_HH4 * (102.4 * sysData.fscan.fft / sysData.fscan.cic + 78 / 25.6);
			Ts = sweepTime_NL + sweepTime_HL + sweepTime_HH + sweepTime_HH4; //us
		} 
		else{
			Ts = sysData.sweep.sweepTime;
			}

	} else
	{
		if (sysData.sweep.sweepTimeAuto == true){//ts caled when auto couple
		
			if (sysData.bw.rbw == 5e6)
				kr = 100;
			else if (sysData.bw.rbw == 3e6)
				kr = 92;
			else if (sysData.bw.rbw == 1e6)
				kr = 20;
			else if (sysData.bw.rbw == 0.5e6)
				kr = 10;
			else if (sysData.bw.rbw == 0.3e6)
				kr = 8.5;
			else if (sysData.bw.rbw == 0.1e6)
				kr = 3.25;
			else
				kr = 1.2069;

			if ( sysData.bw.vbw >= sysData.bw.rbw)
				Ts = ceil (kr * sysData.span.span / (sysData.bw.rbw * sysData.bw.rbw) * 1e6);//us
			else
				Ts = ceil (kr * sysData.span.span / (sysData.bw.rbw * sysData.bw.vbw) * 1e6);//us
		}
		else {//ts use value by  manmual
				Ts = sysData.sweep.sweepTime;
		}
		
			if (Ts <= 1e3)
				Ts = 1e3;	
			
		sysData.fscan.ffHoldTime = 2 * 1e3; //us
		sysData.fscan.ofHoldTime = Ts / sysData.fscan.fftFrame ; //us
			
    }
	sysData.sweep.sweepTime = Ts ;
	
	//if(sysData.span.isZeroSpan)
	//{
	//	sysData.sweep.sweepTime = 1e3;
	//}

}

void tSysScpi::sweepTimeDown(void)
{
	unsigned int Quant_ffHoldTime, Quant_ofHoldTime;

	calSweepTimeOfAuto();

	Quant_ffHoldTime = (unsigned int) (sysData.fscan.ffHoldTime * 50);
	Quant_ofHoldTime = (unsigned int) (sysData.fscan.ofHoldTime * 50);

	feDownload(249, (Quant_ffHoldTime >> 16) & 0xff);
	feDownload(250, Quant_ffHoldTime & 0xffff);
	feDownload(251, (Quant_ofHoldTime >> 16) & 0xff);
	feDownload(252, Quant_ofHoldTime & 0xffff);

}


int tSysScpi::getRecentSweepRbwIndex(double value)
{
	double tempValue = value;
	int count = sizeof sweepDataList / sizeof sweepDataList[0];
	double leftError = 0;
	double rightError = 0;
	int resultIndex = -1;

	if (count > 0)
	{
		if (tempValue < sweepDataList[0].rbw)
		{
			resultIndex = 0;//min
		} else if (tempValue > sweepDataList[count - 1].rbw)
		{
			resultIndex = count - 1;//max
		} 
		else
		{
			for (int i = 0; i < count - 1; i++)
			{
				if (tempValue >= sweepDataList[i].rbw && tempValue <= sweepDataList[i + 1].rbw)
				{
					leftError = qAbs(sweepDataList[i].rbw - tempValue);
					rightError = qAbs(tempValue - sweepDataList[i + 1].rbw);
					resultIndex = (leftError <= rightError) ? (i) : (i + 1);//more close  
					break;
				}
			}
		}
	}

	return resultIndex;
}


int tSysScpi::getRecentFFTRbwIndex(double value)
{
	double tempValue = value;
	int count = sizeof fftBandList / sizeof fftBandList[0];
	double leftError = 0;
	double rightError = 0;
	int resultIndex = -1;

	if (count > 0)
	{
		if (tempValue < fftBandList[0].rbw)
		{
			resultIndex = 0;
		} else if (tempValue > fftBandList[count - 1].rbw)
		{
			resultIndex = count - 1;
		} else
		{
			for (int i = 0; i < count - 1; i++)
			{
				if (tempValue >= fftBandList[i].rbw && tempValue <= fftBandList[i + 1].rbw)
				{
					leftError = qAbs(fftBandList[i].rbw - tempValue);
					rightError = qAbs(tempValue - fftBandList[i + 1].rbw);
					resultIndex = (leftError <= rightError) ? (i) : (i + 1);
					break;
				}
			}
		}
	}

	return resultIndex;
}

/*FFT 频段参数计算*/
void tSysScpi::updateBandParam(void)
{
	double f_start = 0, f_stop = 0, f_step = 0, f_dbuc = 0;
	double f_hllo_start = 0, f_hllo_stop = 0; //低波段起始频率对应的本振频率
	double f_hhlo_start = 0, f_hhlo_stop = 0; //高波段终止频率对应的本振频率
	double f_hh4lo_start = 0, f_hh4lo_stop = 0;
	
	double freq_hh1 = FREQ_START_HH_BAND1, freq_hh2 = FREQ_START_HH_BAND2, freq_hh3 = FREQ_START_HH_BAND3, freq_hh4 = FREQ_START_HH_BAND4;

	bool hband1 = false, hband2 = false, hband3 = false, hband4 = false;
	
	/*获取参数*/
	if(sysData.fscan.mode == fsFFT)
	{
		f_start = sysData.freq.start;
		f_stop = sysData.freq.stop;
		f_step = sysData.fscan.step;
		f_dbuc = f_step;
	} else
	{
		f_step = sysData.fscan.step;
		f_dbuc = rfData.DBUC_FREQ;
		f_start = sysData.freq.start;
		f_stop = sysData.freq.stop;
	}
	/*计算*/
	//1K~30M
	if(sysData.freq.start <= 30e6 && sysData.freq.stop <= 30e6)
	{
		rfData.SWEEP_BAND_INDEX = 1;

		if(sysData.fscan.mode == fsFFT)
		{
			rfData.FTW_Start = (unsigned int)((f_start + f_step / 2) * pow2_32/ 102.4e6);
			rfData.N_L = ceil((f_stop - f_start) / f_step);
		}	else
		{
			rfData.FTW_Start = (unsigned int)(f_start * pow2_32 / 102.4e6);
			rfData.N_L = ceil((f_stop - f_start) / f_step) + 1;
		}
		rfData.FTW_Step = (unsigned int)(f_step * pow2_32 / 102.4e6);
		rfData.N_HL_START_INT = 0;
		rfData.N_HL_START_FRAC = 0;
		rfData.N_HH_START_INT = 0;
		rfData.N_HH_START_FRAC = 0;
		rfData.N_HL = 0;
		rfData.N_HH = 0;
		rfData.N_QSTEP = 0;
		rfData.N_HH1 = 0;
		rfData.N_HH2 = 0;
		rfData.N_HH3 = 0;
		rfData.N_HH4 = 0;
		rfData.N_HH4_START_INT = 0;
		rfData.N_HH4_START_FRAC = 0;
		rfData.N_HH4_STOP_INT = 0;
		rfData.N_HH4_STOP_FRAC = 0;

	} 
	//1K~3.3g
	else if(sysData.freq.start < 10e6 - f_dbuc / 2 && sysData.freq.stop <= freq_hh1)//低波段
	{
		rfData.SWEEP_BAND_INDEX = 3;

		if(sysData.fscan.mode == fsFFT)
		{
			rfData.N_L = ceil((20e6 - f_start) / f_step);
			rfData.FTW_Start = (unsigned int)((f_start + f_step / 2) * pow2_32 / 102.4e6);
			rfData.N_HL = ceil((f_stop - rfData.N_L * f_step - f_start) / f_step);
		} else
		{
			rfData.N_L = ceil((20e6 - f_start) / f_step) + 1;
			rfData.FTW_Start = (unsigned int)(f_start * pow2_32 / 102.4e6);
			rfData.N_HL = ceil((f_stop - rfData.N_L * f_step - f_start) / f_step) + 1;
		}
		rfData.FTW_Step = (unsigned int)(f_step * pow2_32 / 102.4e6);

		if(rfData.N_HL <= 0)
		{
			rfData.N_HL = 0;
			rfData.N_HL_START_INT = 0;
			rfData.N_HL_START_FRAC = 0;
			rfData.N_QSTEP = 0;
			rfData.SWEEP_BAND_INDEX = 1;
			rfData.HIGH_BAND_INDEX = 0;
		} else
		{
			if(sysData.fscan.mode == fsFFT)
			{
				f_hllo_start = rfData.N_L * f_step + f_start + f_step / 2 + L_lo1;
				rfData.N_QSTEP = (unsigned int)(pow2_32 * (f_step / (2 * 50e6)) + 0.5);
			} else
			{
				f_hllo_start = rfData.N_L * f_step + f_start + L_lo1;
				f_hllo_stop = (rfData.N_L + rfData.N_HL - 1) * f_step + f_start + L_lo1;
				rfData.N_QSTEP = (unsigned int)(pow2_24 * (f_step / (2 * 50e6)) + 0.5);
			}
			rfData.N_HL_START_INT = floor(f_hllo_start / (2 * 50e6));
			rfData.N_HL_START_FRAC = (unsigned int)(pow2_32 * (f_hllo_start / (2 * 50e6) - rfData.N_HL_START_INT) + 0.5);

			rfData.N_HL_STOP_INT = (unsigned int)floor(f_hllo_stop / (2 * 50e6));
			rfData.N_HL_STOP_FRAC = (unsigned int)round(pow2_32 * (f_hllo_stop / (2 * 50e6) - rfData.N_HL_STOP_INT) + 0.5);
		}
		rfData.N_HH_START_INT = 0;
		rfData.N_HH_START_FRAC = 0;
		rfData.N_HH_STOP_INT = 0;
		rfData.N_HH_STOP_FRAC = 0;
		rfData.N_HH = 0;
		rfData.N_HH1 = 0;
		rfData.N_HH2 = 0;
		rfData.N_HH3 = 0;
		rfData.N_HH4 = 0;
		rfData.N_HH4_START_INT = 0;
		rfData.N_HH4_START_FRAC = 0;
		rfData.N_HH4_STOP_INT = 0;
		rfData.N_HH4_STOP_FRAC = 0;
	}
	//1K~10M 3.3g~6.6g
	else if(sysData.freq.start < 10e6 - f_dbuc / 2 && sysData.freq.stop > freq_hh1 && sysData.freq.stop <= freq_hh4)
	{
		rfData.SWEEP_BAND_INDEX = 7;

		if(sysData.fscan.mode == fsFFT)
		{
			rfData.N_L = ceil((20e6 - f_start) / f_step);
			rfData.FTW_Start = (unsigned int)((f_start + f_step / 2) * pow2_32/ 102.4e6);
			rfData.N_HL = ceil((freq_hh1 - rfData.N_L * f_step - f_start) / f_step);
			rfData.N_HH = ceil((f_stop - (rfData.N_HL + rfData.N_L) * f_step - f_start) / f_step);
			rfData.FTW_Step = (unsigned int)(f_step * pow2_32 / 102.4e6);
		} else
		{
			rfData.N_L = ceil((20e6 - f_start) / f_step) + 1;
			rfData.FTW_Start = (unsigned int)(f_start * pow2_32 / 102.4e6);
			rfData.N_HL = ceil((freq_hh1 - rfData.N_L * f_step - f_start) / f_step) + 1;
			rfData.N_HH = ceil((f_stop - (rfData.N_HL + rfData.N_L) * f_step - f_start) / f_step) + 1;
			rfData.FTW_Step = (unsigned int)(f_step * pow2_24 / 102.4e6);
		}

		rfData.N_HH4 = 0;
		rfData.N_HH4_START_INT = 0;
		rfData.N_HH4_START_FRAC = 0;
		rfData.N_HH4_STOP_INT = 0;
		rfData.N_HH4_STOP_FRAC = 0;

		if(rfData.N_HH <= 0)
		{
			rfData.SWEEP_BAND_INDEX = 3;
			rfData.HIGH_BAND_INDEX = 0;
			rfData.N_HH = 0;
			rfData.N_HH1 = 0;
			rfData.N_HH2 = 0;
			rfData.N_HH3 = 0;
		} else
		{
			if(f_stop <= freq_hh2)//3.3g~4.4g
			{
				if(sysData.fscan.mode == fsFFT)
					rfData.N_HH1 = ceil((f_stop - (rfData.N_HL + rfData.N_L) * f_step - f_start) / f_step);
				else
					rfData.N_HH1 = ceil((f_stop - (rfData.N_HL + rfData.N_L) * f_step - f_start) / f_step) + 1;
				rfData.N_HH2 = 0;
				rfData.N_HH3 = 0;
				rfData.HIGH_BAND_INDEX = 1;
			} else if(f_stop > freq_hh2 && f_stop <= freq_hh3)//4.4g~5.5g
			{
				if(sysData.fscan.mode == fsFFT)
				{
					rfData.N_HH1 = ceil((freq_hh2 - (rfData.N_HL + rfData.N_L) * f_step - f_start) / f_step);
					rfData.N_HH2 = ceil((f_stop - (rfData.N_HH1 + rfData.N_HL + rfData.N_L) * f_step - f_start) / f_step);
				} else
				{
					rfData.N_HH1 = ceil((freq_hh2 - (rfData.N_HL + rfData.N_L) * f_step - f_start) / f_step) + 1;
					rfData.N_HH2 = ceil((f_stop - (rfData.N_HH1 + rfData.N_HL + rfData.N_L) * f_step - f_start) / f_step) + 1;
				}
				rfData.N_HH3 = 0;
				rfData.HIGH_BAND_INDEX = 3;
				if(rfData.N_HH2 == 0)
					rfData.HIGH_BAND_INDEX = 1;
			} else if(f_stop > freq_hh3 && f_stop <= freq_hh4)//5.5g~6.6g
			{
				if(sysData.fscan.mode == fsFFT)
				{
					rfData.N_HH1 = ceil((freq_hh2 - (rfData.N_HL + rfData.N_L) * f_step - f_start) / f_step);
					rfData.N_HH2 = ceil((freq_hh3 - (rfData.N_HH1 + rfData.N_HL + rfData.N_L) * f_step - f_start) / f_step);
				} else
				{
					rfData.N_HH1 = ceil((freq_hh2 - (rfData.N_HL + rfData.N_L) * f_step - f_start) / f_step) + 1;
					rfData.N_HH2 = ceil((freq_hh3 - (rfData.N_HH1 + rfData.N_HL + rfData.N_L) * f_step - f_start) / f_step) + 1;
				}
				rfData.N_HH3 = rfData.N_HH - rfData.N_HH1 - rfData.N_HH2;
				rfData.HIGH_BAND_INDEX = 7;
				if(rfData.N_HH3 == 0)
					rfData.HIGH_BAND_INDEX = 3;
			}
		}

		if(sysData.fscan.mode == fsFFT)
		{
			f_hllo_start = rfData.N_L * f_step + f_start + f_step / 2 + L_lo1;
			f_hhlo_start = (rfData.N_L + rfData.N_HL) * f_step + f_start + f_step / 2 + H_lo1;
			rfData.N_QSTEP = (unsigned int)(pow2_32 * (f_step / (2 * 50e6)) + 0.5);
		} else
		{
			f_hllo_start = rfData.N_L * f_step + f_start + L_lo1;
			f_hllo_stop = (rfData.N_L + rfData.N_HL - 1) * f_step + f_start + L_lo1;

			f_hhlo_start = (rfData.N_L + rfData.N_HL) * f_step + f_start + H_lo1;
			f_hllo_stop = (rfData.N_L + rfData.N_HL + rfData.N_HH - 1) * f_step + f_start + H_lo1;

			rfData.N_QSTEP = (unsigned int)(pow2_24 * (f_step / (2 * 50e6)) + 0.5);
		}
		rfData.N_HL_START_INT = floor(f_hllo_start / (2 * 50e6));
		rfData.N_HL_START_FRAC = (unsigned int)(pow2_32* (f_hllo_start / (2 * 50e6) - rfData.N_HL_START_INT) + 0.5);
		rfData.N_HL_STOP_INT = floor(f_hllo_stop / (2 * 50e6));
		rfData.N_HL_STOP_FRAC = (unsigned int)(pow2_32* (f_hllo_stop / (2 * 50e6) - rfData.N_HL_STOP_INT) + 0.5);

		rfData.N_HH_START_INT = (unsigned int)floor(f_hhlo_start / (2 * 50e6));
		rfData.N_HH_START_FRAC = (unsigned int)(pow2_32* (f_hhlo_start / (2 * 50e6) - rfData.N_HH_START_INT) + 0.5);
		rfData.N_HH_STOP_INT = (unsigned int)floor(f_hhlo_stop / (2 * 50e6));
		rfData.N_HH_STOP_FRAC = (unsigned int)(pow2_32 * (f_hhlo_stop / (2 * 50e6) - rfData.N_HH_STOP_INT) + 0.5);

	} 
	//1k~10M + 6.6g~7.5g
	else if(sysData.freq.start < 10e6 - f_dbuc / 2 && sysData.freq.stop > freq_hh4 && sysData.freq.stop <= MAXFREQ)
	{
		rfData.SWEEP_BAND_INDEX = 0xf;
		rfData.HIGH_BAND_INDEX = 0x07;
		if(sysData.fscan.mode == fsFFT)
		{
			rfData.N_L = ceil((20e6 - f_start) / f_step);
			rfData.FTW_Start = (unsigned int)((f_start + f_step / 2) * pow2_32 / 102.4e6);
			rfData.N_HL = ceil((freq_hh1 - rfData.N_L * f_step - f_start) / f_step);
			rfData.N_HH = ceil((freq_hh4 - (rfData.N_HL + rfData.N_L) * f_step - f_start) / f_step);
			rfData.FTW_Step = (unsigned int)(f_step * pow2_32/ 102.4e6);
			rfData.N_HH1 = ceil((freq_hh2 - (rfData.N_HL + rfData.N_L) * f_step - f_start) / f_step);
			rfData.N_HH2 = ceil((freq_hh3 - (rfData.N_HH1 + rfData.N_HL + rfData.N_L) * f_step - f_start) / f_step);
			rfData.N_HH3 = rfData.N_HH - rfData.N_HH1 - rfData.N_HH2;
			rfData.N_HH4 = ceil((f_stop - (rfData.N_HH + rfData.N_HL + rfData.N_L) * f_step - f_start) / f_step);
			if(rfData.N_HH4 <= 0)
			{
				rfData.SWEEP_BAND_INDEX = 7;
				rfData.N_HH4 = 0;
			}
			f_hllo_start = rfData.N_L * f_step + f_start + f_step / 2 + L_lo1;
			rfData.N_QSTEP = (unsigned int)(pow2_32 * (f_step / (2 * 50e6)) + 0.5);
			f_hhlo_start = (rfData.N_L + rfData.N_HL) * f_step + f_start + f_step / 2 + H_lo1;
			f_hh4lo_start = (rfData.N_L + rfData.N_HL + rfData.N_HH) * f_step + f_start + f_step / 2 - H_lo1;

		} else
		{
			rfData.N_L = ceil((20e6 - f_start) / f_step) + 1;
			rfData.FTW_Start = (unsigned int)(f_start * pow2_32/ 102.4e6);
			rfData.N_HL = ceil((freq_hh1 - rfData.N_L * f_step - f_start) / f_step) + 1;
			rfData.N_HH = ceil((freq_hh4 - (rfData.N_HL + rfData.N_L) * f_step - f_start) / f_step) + 1;
			rfData.FTW_Step = (unsigned int)(f_step * pow2_24 / 102.4e6);
			rfData.N_HH1 = ceil((freq_hh2 - (rfData.N_HL + rfData.N_L) * f_step - f_start) / f_step) + 1;
			rfData.N_HH2 = ceil((freq_hh3 - (rfData.N_HH1 + rfData.N_HL + rfData.N_L) * f_step - f_start) / f_step) + 1;
			rfData.N_HH3 = rfData.N_HH - rfData.N_HH1 - rfData.N_HH2;
			rfData.N_HH4 = ceil((f_stop - (rfData.N_HH + rfData.N_HL + rfData.N_L) * f_step - f_start) / f_step) + 1;
			if(rfData.N_HH4 <= 0)
			{
				rfData.SWEEP_BAND_INDEX = 7;
				rfData.N_HH4 = 0;
			}
			f_hllo_start = rfData.N_L * f_step + f_start + L_lo1;
			f_hllo_stop = (rfData.N_L + rfData.N_HL - 1) * f_step + f_start + L_lo1;
			rfData.N_QSTEP = (unsigned int)(pow2_24 * (f_step / (2 * 50e6)) + 0.5);
			f_hhlo_start = (rfData.N_L + rfData.N_HL) * f_step + f_start + H_lo1;
			f_hhlo_stop = (rfData.N_L + rfData.N_HL + rfData.N_HH - 1) * f_step + f_start + H_lo1;
			f_hh4lo_start = (rfData.N_L + rfData.N_HL + rfData.N_HH) * f_step + f_start - H_lo1;
			f_hh4lo_stop = (rfData.N_L + rfData.N_HL + rfData.N_HH + rfData.N_HH4 - 1) * f_step + f_start - H_lo1;
		}
		rfData.N_HL_START_INT = floor(f_hllo_start / (2 * 50e6));
		rfData.N_HL_START_FRAC = (unsigned int)(pow2_32 * (f_hllo_start / (2 * 50e6) - rfData.N_HL_START_INT) + 0.5);
		rfData.N_HL_STOP_INT = floor(f_hllo_stop / (2 * 50e6));
		rfData.N_HL_STOP_FRAC = (unsigned int)(pow2_32 * (f_hllo_stop / (2 * 50e6) - rfData.N_HL_STOP_INT) + 0.5);

		rfData.N_HH_START_INT = (unsigned int)floor(f_hhlo_start / (2 * 50e6));
		rfData.N_HH_START_FRAC = (unsigned int)(pow2_32 * (f_hhlo_start / (2 * 50e6) - rfData.N_HH_START_INT) + 0.5);
		rfData.N_HH_STOP_INT = (unsigned int)floor(f_hhlo_stop / (2 * 50e6));
		rfData.N_HH_STOP_FRAC = (unsigned int)(pow2_32 * (f_hhlo_stop / (2 * 50e6) - rfData.N_HH_STOP_INT) + 0.5);

		rfData.N_HH4_START_INT = (unsigned int)floor(f_hh4lo_start / (2 * 50e6));
		rfData.N_HH4_START_FRAC = (unsigned int)(pow2_32* (f_hh4lo_start / (2 * 50e6) - rfData.N_HH4_START_INT) + 0.5);
		rfData.N_HH4_STOP_INT = (unsigned int)floor(f_hh4lo_stop / (2 * 50e6));
		rfData.N_HH4_STOP_FRAC = (unsigned int)(pow2_32 * (f_hh4lo_stop / (2 * 50e6) - rfData.N_HH4_STOP_INT) + 0.5);

	}
	//10M~3.3g
	else if(sysData.freq.start >= 10e6 - f_dbuc / 2 && sysData.freq.start < freq_hh1 - f_dbuc / 2 && sysData.freq.stop <= freq_hh1)
	{
		rfData.SWEEP_BAND_INDEX = 2;
		rfData.HIGH_BAND_INDEX = 0;
		rfData.N_L = 0;
		rfData.FTW_Start = 0x5e000000; //(unsigned int)(27.4e6 / 102.4e6 * pow(2, 32));
		rfData.FTW_Step = 0;

		if(sysData.fscan.mode == fsFFT)
			rfData.N_HL = ceil((f_stop - f_start) / f_step);
		else
			rfData.N_HL = ceil((f_stop - f_start) / f_step) + 1;
		rfData.N_HH = 0;
		rfData.N_HH_START_INT = 0;
		rfData.N_HH_START_FRAC = 0;
		rfData.N_HH_STOP_INT = 0;
		rfData.N_HH_STOP_FRAC = 0;
		rfData.N_HH1 = 0;
		rfData.N_HH2 = 0;
		rfData.N_HH3 = 0;
		rfData.N_HH4 = 0;
		rfData.N_HH4_START_INT = 0;
		rfData.N_HH4_START_FRAC = 0;
		rfData.N_HH4_STOP_INT = 0;
		rfData.N_HH4_STOP_FRAC = 0;
		if(sysData.fscan.mode == fsFFT)
		{
			f_hllo_start = f_start + f_step / 2 + L_lo1;
			rfData.N_QSTEP = (unsigned int)(pow2_32 * (f_step / (2 * 50e6)) + 0.5);
		} else
		{
			f_hllo_start = f_start + L_lo1 -f_step;//-f_step
			f_hllo_stop = (rfData.N_HL - 1) * f_step + f_start + L_lo1;
			rfData.N_QSTEP = (unsigned int)(pow2_24 * (f_step / (2 * 50e6)) + 0.5);
		}
		rfData.N_HL_START_INT = (unsigned int) floor(f_hllo_start / (2 * 50e6));
		rfData.N_HL_START_FRAC = (unsigned int)(pow2_32 * (f_hllo_start / (2 * 50e6) - rfData.N_HL_START_INT) + 0.5);

		rfData.N_HL_STOP_INT = (unsigned int) floor(f_hllo_stop / (2 * 50e6));
		rfData.N_HL_STOP_FRAC = (unsigned int)(pow2_32 * (f_hllo_stop / (2 * 50e6) - rfData.N_HL_STOP_INT) + 0.5);
	}
	//10M~6.6g
	else if(sysData.freq.start >= 10e6 - f_dbuc / 2 && sysData.freq.start < freq_hh1 - f_dbuc / 2 && sysData.freq.stop > freq_hh1 && sysData.freq.stop <= freq_hh4)
	{
		rfData.SWEEP_BAND_INDEX = 6;
		rfData.HIGH_BAND_INDEX = 7;
		rfData.N_L = 0;
		rfData.FTW_Start = 0x5e000000; //(unsigned int)(27.4e6 / 102.4e6 * pow(2, 32));
		rfData.FTW_Step = 0;

		if(sysData.fscan.mode == fsFFT)
		{
			rfData.N_HL = ceil((freq_hh1 - f_start) / f_step);
			rfData.N_HH = ceil((f_stop - rfData.N_HL * f_step - f_start) / f_step);
		} else
		{
			rfData.N_HL = ceil((freq_hh1 - f_start) / f_step) + 1;
			rfData.N_HH = ceil((f_stop - rfData.N_HL * f_step - f_start) / f_step) + 1;
		}
		rfData.N_HH4 = 0;
		rfData.N_HH4_START_INT = 0;
		rfData.N_HH4_START_FRAC = 0;
		rfData.N_HH4_STOP_INT = 0;
		rfData.N_HH4_STOP_FRAC = 0;
		if(rfData.N_HH <= 0)
		{
			rfData.SWEEP_BAND_INDEX = 2;
			rfData.HIGH_BAND_INDEX = 0;
			rfData.N_HH = 0;
			rfData.N_HH1 = 0;
			rfData.N_HH2 = 0;
			rfData.N_HH3 = 0;
		}
		else
		{
			if(f_stop <= freq_hh2)//3.3g~4.4g
			{
				if(sysData.fscan.mode == fsFFT)
					rfData.N_HH1 = ceil((f_stop - rfData.N_HL * f_step - f_start) / f_step);
				else
					rfData.N_HH1 = ceil((f_stop - rfData.N_HL * f_step - f_start) / f_step) + 1;

				rfData.N_HH2 = 0;
				rfData.N_HH3 = 0;
				rfData.HIGH_BAND_INDEX = 1;
			} else if(f_stop > freq_hh2 && f_stop <= freq_hh3)//4.4g~5.5g
			{
				if(sysData.fscan.mode == fsFFT)
					rfData.N_HH1 = ceil((freq_hh2 - rfData.N_HL * f_step - f_start) / f_step);
				else
					rfData.N_HH1 = ceil((freq_hh2 - rfData.N_HL * f_step - f_start) / f_step) + 1;
				if(rfData.N_HH1 < 0)
					rfData.N_HH1 = 0;

				if(sysData.fscan.mode == fsFFT)
					rfData.N_HH2 = ceil((f_stop - (rfData.N_HH1 + rfData.N_HL) * f_step - f_start) / f_step);
				else
					rfData.N_HH2 = ceil((f_stop - (rfData.N_HH1 + rfData.N_HL) * f_step - f_start) / f_step) + 1;
				if(rfData.N_HH2 < 0)
					rfData.N_HH2 = 0;

				rfData.N_HH3 = 0;
				rfData.HIGH_BAND_INDEX = 3;
				if(rfData.N_HH2)
					rfData.HIGH_BAND_INDEX = 1;
			} 
			//5.5g~6.6g
			else if(f_stop > freq_hh3 && f_stop <= freq_hh4)
			{
				if(sysData.fscan.mode == fsFFT)
					rfData.N_HH1 = ceil((freq_hh2 - rfData.N_HL * f_step - f_start) / f_step);
				else
					rfData.N_HH1 = ceil((freq_hh2 - rfData.N_HL * f_step - f_start) / f_step) + 1;
				if(rfData.N_HH1 < 0)
					rfData.N_HH1 = 0;

				if(sysData.fscan.mode == fsFFT)
					rfData.N_HH2 = ceil((freq_hh3 - (rfData.N_HH1 + rfData.N_HL) * f_step - f_start) / f_step);
				else
					rfData.N_HH2 = ceil((freq_hh3 - (rfData.N_HH1 + rfData.N_HL) * f_step - f_start) / f_step) + 1;
				if(rfData.N_HH2 < 0)
					rfData.N_HH2 = 0;

				if(sysData.fscan.mode == fsFFT)
					rfData.N_HH3 = ceil((f_stop - (rfData.N_HH2 + rfData.N_HH1 + rfData.N_HL) * f_step - f_start) / f_step);
				else
					rfData.N_HH3 = ceil((f_stop - (rfData.N_HH2 + rfData.N_HH1 + rfData.N_HL) * f_step - f_start) / f_step) + 1;
				rfData.HIGH_BAND_INDEX = 7;
				if(rfData.N_HH3 <= 0)
				{
					rfData.N_HH3 = 0;
					rfData.HIGH_BAND_INDEX = 3;
				}
			}
		}
		if(sysData.fscan.mode == fsFFT)
		{
			f_hllo_start = f_start + f_step / 2 + L_lo1;
			f_hhlo_start = rfData.N_HL * f_step + f_start + f_step / 2 + H_lo1;
			rfData.N_QSTEP = (unsigned int)(pow2_32 * (f_step / (2 * 50e6)) + 0.5);
		} else
		{
			f_hllo_start = f_start + L_lo1 -f_step;//-f_step
			f_hllo_stop = (rfData.N_HL - 1) * f_step + f_start + L_lo1;
			f_hhlo_start = rfData.N_HL * f_step + f_start + H_lo1 -f_step;//-f_step
			f_hhlo_stop = (rfData.N_HL + rfData.N_HH - 1) * f_step + f_start + H_lo1;
			rfData.N_QSTEP = (unsigned int)(pow2_24 * (f_step / (2 * 50e6)) + 0.5);
		}

		rfData.N_HL_START_INT = (unsigned int)floor(f_hllo_start / (2 * 50e6));
		rfData.N_HL_START_FRAC = (unsigned int)(pow2_32 * (f_hllo_start / (2 * 50e6) - rfData.N_HL_START_INT) + 0.5);
		rfData.N_HL_STOP_INT = (unsigned int)floor(f_hllo_stop / (2 * 50e6));
		rfData.N_HL_STOP_FRAC = (unsigned int)(pow2_32 * (f_hllo_stop / (2 * 50e6) - rfData.N_HL_STOP_INT) + 0.5);

		rfData.N_HH_START_INT = (unsigned int)floor(f_hhlo_start / (2 * 50e6));
		rfData.N_HH_START_FRAC = (unsigned int)(pow2_32 * (f_hhlo_start / (2 * 50e6) - rfData.N_HH_START_INT) + 0.5);
		rfData.N_HH_STOP_INT = (unsigned int)floor(f_hhlo_stop / (2 * 50e6));
		rfData.N_HH_STOP_FRAC = (unsigned int)(pow2_32 * (f_hhlo_stop / (2 * 50e6) - rfData.N_HH_STOP_INT) + 0.5);

	} 
	//10M~3.3g + 5.5g~7.5g
	else if(sysData.freq.start >= 10e6 - f_dbuc / 2 && sysData.freq.start < freq_hh1 - f_dbuc / 2 && sysData.freq.stop > freq_hh4 && sysData.freq.stop <= MAXFREQ)
	{
		rfData.SWEEP_BAND_INDEX = 0xe;
		rfData.HIGH_BAND_INDEX = 0x7;
		rfData.N_L = 0;
		rfData.FTW_Start = 0x5e000000; //(unsigned int)(27.4e6 / 102.4e6 * pow(2, 32));
		rfData.FTW_Step = 0;
		if(sysData.fscan.mode == fsFFT)
		{
			rfData.N_HL = ceil((freq_hh1 - f_start) / f_step);
			rfData.N_HH = ceil((freq_hh4 - rfData.N_HL * f_step - f_start) / f_step);
			rfData.N_HH1 = ceil((freq_hh2 - rfData.N_HL * f_step - f_start) / f_step);
			if(rfData.N_HH1 < 0)
				rfData.N_HH1 = 0;
			rfData.N_HH2 = ceil((freq_hh3 - (rfData.N_HH1 + rfData.N_HL) * f_step - f_start) / f_step);
			if(rfData.N_HH2 < 0)
				rfData.N_HH2 = 0;
			rfData.N_HH3 = rfData.N_HH - rfData.N_HH1 - rfData.N_HH2;
			rfData.N_HH4 = ceil((f_stop - (rfData.N_HH + rfData.N_HL) * f_step - f_start) / f_step);
			if(rfData.N_HH4 <= 0)
			{
				rfData.SWEEP_BAND_INDEX = 6;
				rfData.N_HH4 = 0;
			}

			f_hllo_start = f_start + f_step / 2 + L_lo1;
			rfData.N_QSTEP = (unsigned int)(pow2_32 * (f_step / (2 * 50e6)) + 0.5);

			f_hhlo_start = rfData.N_HL * f_step + f_start + f_step / 2 + 2.64e9;
			f_hh4lo_start = (rfData.N_HL + rfData.N_HH) * f_step + f_start + f_step / 2 - H_lo1;
		} else
		{
			rfData.N_HL = ceil((freq_hh1 - f_start) / f_step) + 1;
			rfData.N_HH = ceil((freq_hh4 - rfData.N_HL * f_step - f_start) / f_step) + 1;
			rfData.N_HH1 = ceil((freq_hh2 - rfData.N_HL * f_step - f_start) / f_step) + 1;
			if(rfData.N_HH1 < 0)
				rfData.N_HH1 = 0;
			rfData.N_HH2 = ceil((freq_hh3 - (rfData.N_HH1 + rfData.N_HL) * f_step - f_start) / f_step) + 1;
			if(rfData.N_HH2 < 0)
				rfData.N_HH2 = 0;
			rfData.N_HH3 = rfData.N_HH - rfData.N_HH1 - rfData.N_HH2;
			rfData.N_HH4 = ceil((f_stop - (rfData.N_HH + rfData.N_HL) * f_step - f_start) / f_step) + 1;
			if(rfData.N_HH4 <= 0)
			{
				rfData.SWEEP_BAND_INDEX = 6;
				rfData.N_HH4 = 0;
			}

			f_hllo_start = f_start + L_lo1;
			f_hllo_stop = (rfData.N_HL - 1) * f_step + f_start + L_lo1;

			rfData.N_QSTEP = (unsigned int)(pow2_24 * (f_step / (2 * 50e6)) + 0.5);

			f_hhlo_start = rfData.N_HL * f_step + f_start + H_lo1 -f_step;//-f_step
			f_hhlo_stop = (rfData.N_HL + rfData.N_HH - 1) * f_step + f_start + H_lo1;
			f_hh4lo_start = (rfData.N_HL + rfData.N_HH) * f_step + f_start - H_lo1 -f_step;//-f_step
			f_hh4lo_stop = (rfData.N_HL + rfData.N_HH + rfData.N_HH4 - 1) * f_step + f_start - H_lo1;
		}

		rfData.N_HL_START_INT = (unsigned int)floor(f_hllo_start / (2 * 50e6));
		rfData.N_HL_START_FRAC = (unsigned int)(pow2_32 * (f_hllo_start / (2 * 50e6) - rfData.N_HL_START_INT) + 0.5);
		rfData.N_HL_STOP_INT = (unsigned int)floor(f_hllo_stop / (2 * 50e6));
		rfData.N_HL_STOP_FRAC = (unsigned int)(pow2_32 * (f_hllo_stop / (2 * 50e6) - rfData.N_HL_STOP_INT) + 0.5);

		rfData.N_HH_START_INT = (unsigned int)floor(f_hhlo_start / (2 * 50e6));
		rfData.N_HH_START_FRAC = (unsigned int)(pow2_32 * (f_hhlo_start / (2 * 50e6) - rfData.N_HH_START_INT) + 0.5);
		rfData.N_HH_STOP_INT = (unsigned int)floor(f_hhlo_stop / (2 * 50e6));
		rfData.N_HH_STOP_FRAC = (unsigned int)(pow2_32 * (f_hhlo_stop / (2 * 50e6) - rfData.N_HH_STOP_INT) + 0.5);

		rfData.N_HH4_START_INT = (unsigned int)floor(f_hh4lo_start / (2 * 50e6));
		rfData.N_HH4_START_FRAC = (unsigned int)(pow2_32 * (f_hh4lo_start / (2 * 50e6) - rfData.N_HH4_START_INT) + 0.5);
		rfData.N_HH4_STOP_INT = (unsigned int)floor(f_hh4lo_stop / (2 * 50e6));
		rfData.N_HH4_STOP_FRAC = (unsigned int)(pow2_32 * (f_hh4lo_stop / (2 * 50e6) - rfData.N_HH4_STOP_INT) + 0.5);
	}
	//3.3g~6.6g
	else if(sysData.freq.start >= freq_hh1 - f_dbuc / 2 && sysData.freq.start <= freq_hh4 - f_dbuc / 2 && sysData.freq.stop <= freq_hh4)
	{
		rfData.SWEEP_BAND_INDEX = 4;
		rfData.N_L = 0;
		rfData.FTW_Start = 0x5e000000;//(unsigned int)(27.4e6 / 102.4e6 * pow(2, 32));
		rfData.FTW_Step = 0;

		rfData.N_HL = 0;
		rfData.N_HL_START_INT = 0;
		rfData.N_HL_START_FRAC = 0;
		rfData.N_HH4 = 0;
		rfData.N_HH4_START_INT = 0;
		rfData.N_HH4_START_FRAC = 0;
		rfData.N_HH4_STOP_INT = 0;
		rfData.N_HH4_STOP_FRAC = 0;

		if(sysData.fscan.mode == fsFFT)
			rfData.N_HH = ceil((f_stop - f_start) / f_step);
		else
			rfData.N_HH = ceil((f_stop - f_start) / f_step) + 1;

		if(f_stop <= freq_hh2)//3.3g~4.4g
		{
			if(sysData.fscan.mode == fsFFT)
				rfData.N_HH1 = ceil((f_stop - f_start) / f_step);
			else
				rfData.N_HH1 = ceil((f_stop - f_start) / f_step) + 1;
			rfData.N_HH2 = 0;
			rfData.N_HH3 = 0;
			rfData.HIGH_BAND_INDEX = 1;
		} else if(f_stop > freq_hh2 && f_stop <= freq_hh3)//4.4g~5.5g
		{
			if(sysData.fscan.mode == fsFFT)
				rfData.N_HH1 = ceil((freq_hh2 - f_start) / f_step);
			else
				rfData.N_HH1 = ceil((freq_hh2 - f_start) / f_step) + 1;
			if(rfData.N_HH1 <= 0)
				rfData.N_HH1 = 0;
			if(sysData.fscan.mode == fsFFT)
				rfData.N_HH2 = ceil((f_stop - rfData.N_HH1 * f_step - f_start) / f_step);
			else
				rfData.N_HH2 = ceil((f_stop - rfData.N_HH1 * f_step - f_start) / f_step) + 1;
			rfData.N_HH3 = 0;
			rfData.HIGH_BAND_INDEX = 3;
			if(rfData.N_HH2 == 0)
				rfData.HIGH_BAND_INDEX = 1;
			if(rfData.N_HH1 == 0)
				rfData.HIGH_BAND_INDEX = 2;
		} else if(f_stop > freq_hh3 && f_stop <= freq_hh4)//5.5g~6.6g
		{
			if(sysData.fscan.mode == fsFFT)
				rfData.N_HH1 = ceil((freq_hh2 - f_start) / f_step);
			else
				rfData.N_HH1 = ceil((freq_hh2 - f_start) / f_step) + 1;
			if(rfData.N_HH1 <= 0)
				rfData.N_HH1 = 0;

			if(sysData.fscan.mode == fsFFT)
				rfData.N_HH2 = ceil((freq_hh3 - rfData.N_HH1 * f_step - f_start) / f_step);
			else
				rfData.N_HH2 = ceil((freq_hh3 - rfData.N_HH1 * f_step - f_start) / f_step) + 1;
			if(rfData.N_HH2 <= 0)
				rfData.N_HH2 = 0;

			if(sysData.fscan.mode == fsFFT)
				rfData.N_HH3 = ceil((f_stop - (rfData.N_HH2 + rfData.N_HH1) * f_step - f_start) / f_step);
			else
				rfData.N_HH3 = ceil((f_stop - (rfData.N_HH2 + rfData.N_HH1) * f_step - f_start) / f_step) + 1;

			if(rfData.N_HH3 <= 0)
				rfData.N_HH3 = 0;
			rfData.HIGH_BAND_INDEX = 7;
			if(rfData.N_HH1 == 0 && rfData.N_HH2 == 0 && rfData.N_HH3 != 0)
				rfData.HIGH_BAND_INDEX = 4;
			else if(rfData.N_HH1 == 0 && rfData.N_HH2 != 0 && rfData.N_HH3 == 0)
				rfData.HIGH_BAND_INDEX = 2;
			else if(rfData.N_HH1 == 0 && rfData.N_HH2 != 0 && rfData.N_HH3 != 0)
				rfData.HIGH_BAND_INDEX = 6;
			else if(rfData.N_HH1 != 0 && rfData.N_HH2 != 0 && rfData.N_HH3 == 0)
				rfData.HIGH_BAND_INDEX = 3;

		}
		if(sysData.fscan.mode == fsFFT)
		{
			f_hhlo_start = f_start + f_step / 2 + H_lo1;
			rfData.N_QSTEP = (unsigned int)(pow2_32 * (f_step / (2 * 50e6)) + 0.5);
		} else
		{
			f_hhlo_start = f_start + H_lo1 -f_step;//-f_step
			f_hhlo_stop = (rfData.N_HH - 1) * f_step + f_start + H_lo1;
			rfData.N_QSTEP = (unsigned int)(pow2_24 * (f_step / (2 * 50e6)) + 0.5);
		}
		rfData.N_HH_START_INT = (unsigned int)floor(f_hhlo_start / (2 * 50e6));
		rfData.N_HH_START_FRAC = (unsigned int)(pow2_32 * (f_hhlo_start / (2 * 50e6) - rfData.N_HH_START_INT) + 0.5);
		rfData.N_HH_STOP_INT = (unsigned int)floor(f_hhlo_stop / (2 * 50e6));
		rfData.N_HH_STOP_FRAC = (unsigned int)(pow2_32 * (f_hhlo_stop / (2 * 50e6) - rfData.N_HH_STOP_INT) + 0.5);
	}
	//3.3g~7.5g
	else if(sysData.freq.start >= freq_hh1 - f_dbuc / 2 && sysData.freq.start <= freq_hh4 - f_dbuc / 2 && sysData.freq.stop > freq_hh4 && sysData.freq.stop <= MAXFREQ)
	{
		rfData.SWEEP_BAND_INDEX = 0xc;
		rfData.N_L = 0;
		rfData.FTW_Start = 0x5e000000;//(unsigned int)(27.4e6 / 102.4e6 * pow(2, 32));
		rfData.FTW_Step = 0;

		rfData.N_HL = 0;
		rfData.N_HL_START_INT = 0;
		rfData.N_HL_START_FRAC = 0;
		rfData.N_HL_STOP_INT = 0;
		rfData.N_HL_STOP_FRAC = 0;

		if(sysData.fscan.mode == fsFFT)
		{
			rfData.N_HH = ceil((freq_hh4 - f_start) / f_step);
			rfData.N_HH1 = ceil((freq_hh2 - rfData.N_HL * f_step - f_start) / f_step);
			if(rfData.N_HH1 < 0)
				rfData.N_HH1 = 0;
			rfData.N_HH2 = ceil((freq_hh3 - (rfData.N_HH1 + rfData.N_HL) * f_step - f_start) / f_step);
			if(rfData.N_HH2 < 0)
				rfData.N_HH2 = 0;
			rfData.N_HH3 = rfData.N_HH - rfData.N_HH1 - rfData.N_HH2;
			rfData.N_HH4 = ceil((f_stop - rfData.N_HH * f_step - f_start) / f_step);
			if(rfData.N_HH4 <= 0)
			{
				rfData.N_HH4 = 0;
				rfData.SWEEP_BAND_INDEX = 4;
				rfData.HIGH_BAND_INDEX = 4;
				if(rfData.N_HH1 == 0 && rfData.N_HH2 == 0)
					rfData.HIGH_BAND_INDEX = 4;
				else if(rfData.N_HH1 == 0 && rfData.N_HH2 != 0)
					rfData.HIGH_BAND_INDEX = 6;
				else if(rfData.N_HH1 != 0 && rfData.N_HH2 != 0)
					rfData.HIGH_BAND_INDEX = 7;
			}

			f_hhlo_start = f_start + f_step / 2 + H_lo1;
			f_hh4lo_start = rfData.N_HH * f_step + f_start + f_step / 2 - H_lo1;
			rfData.N_QSTEP = (unsigned int)(pow2_32 * (f_step / (2 * 50e6)) + 0.5);

		} else
		{
			rfData.N_HH = ceil((freq_hh4 - f_start) / f_step) + 1;
			rfData.N_HH1 = ceil((freq_hh2 - rfData.N_HL * f_step - f_start) / f_step) + 1;
			if(rfData.N_HH1 < 0)
				rfData.N_HH1 = 0;
			rfData.N_HH2 = ceil((freq_hh3 - (rfData.N_HH1 + rfData.N_HL) * f_step - f_start) / f_step) + 1;
			if(rfData.N_HH2 < 0)
				rfData.N_HH2 = 0;
			rfData.N_HH3 = rfData.N_HH - rfData.N_HH1 - rfData.N_HH2;
			rfData.N_HH4 = ceil((f_stop - rfData.N_HH * f_step - f_start) / f_step) + 1;
			if(rfData.N_HH4 <= 0)
			{
				rfData.N_HH4 = 0;
				rfData.SWEEP_BAND_INDEX = 4;
				rfData.HIGH_BAND_INDEX = 4;
				if(rfData.N_HH1 == 0 && rfData.N_HH2 == 0)
					rfData.HIGH_BAND_INDEX = 4;
				else if(rfData.N_HH1 == 0 && rfData.N_HH2 != 0)
					rfData.HIGH_BAND_INDEX = 6;
				else if(rfData.N_HH1 != 0 && rfData.N_HH2 != 0)
					rfData.HIGH_BAND_INDEX = 7;				
			}

			f_hhlo_start = f_start + H_lo1 -f_step;//-f_step
			f_hhlo_stop = (rfData.N_HH - 1) * f_step + f_start + H_lo1;
			f_hh4lo_start = rfData.N_HH * f_step + f_start - H_lo1 -f_step;//-f_step
			f_hh4lo_stop = (rfData.N_HH + rfData.N_HH4 - 1) * f_step + f_start - H_lo1;

			rfData.N_QSTEP = (unsigned int)(pow2_24 * (f_step / (2 * 50e6)) + 0.5);
		}


		rfData.N_HH_START_INT = (unsigned int)floor(f_hhlo_start / (2 * 50e6));
		rfData.N_HH_START_FRAC = (unsigned int)(pow2_32 * (f_hhlo_start / (2 * 50e6) - rfData.N_HH_START_INT) + 0.5);
		rfData.N_HH_STOP_INT = (unsigned int)floor(f_hhlo_stop / (2 * 50e6));
		rfData.N_HH_STOP_FRAC = (unsigned int)(pow2_32 * (f_hhlo_stop / (2 * 50e6) - rfData.N_HH_STOP_INT) + 0.5);

		rfData.N_HH4_START_INT = (unsigned int)floor(f_hh4lo_start / (2 * 50e6));
		rfData.N_HH4_START_FRAC = (unsigned int)(pow2_32 * (f_hh4lo_start / (2 * 50e6) - rfData.N_HH4_START_INT) + 0.5);
		rfData.N_HH4_STOP_INT = (unsigned int)floor(f_hh4lo_stop / (2 * 50e6));
		rfData.N_HH4_STOP_FRAC = (unsigned int)(pow2_32 * (f_hh4lo_stop / (2 * 50e6) - rfData.N_HH4_STOP_INT) + 0.5);
	} 
	//6.6g~7.5g
	else
	{
		rfData.SWEEP_BAND_INDEX = 8;
		rfData.HIGH_BAND_INDEX = 0;
		rfData.N_L = 0;
		rfData.FTW_Start = 0x5e000000;//(unsigned int)(27.4e6 / 102.4e6 * pow(2, 32));
		rfData.FTW_Step = 0;

		rfData.N_HL = 0;
		rfData.N_HL_START_INT = 0;
		rfData.N_HL_START_FRAC = 0;
		rfData.N_HL_STOP_INT = 0;
		rfData.N_HL_STOP_FRAC = 0;

		rfData.N_HH = 0;
		rfData.N_HH_START_INT = 0;
		rfData.N_HH_START_FRAC = 0;
		rfData.N_HH_STOP_INT = 0;
		rfData.N_HH_STOP_FRAC = 0;

		rfData.N_HH1 = 0;
		rfData.N_HH2 = 0;
		rfData.N_HH3 = 0;

		if(sysData.fscan.mode == fsFFT)
		{
			rfData.N_HH4 = ceil((f_stop - f_start) / f_step);
			f_hh4lo_start = f_start + f_step / 2 - H_lo1;
			rfData.N_QSTEP = (unsigned int)(pow2_32 * (f_step / (2 * 50e6)) + 0.5);
		}
		else
		{
			rfData.N_HH4 = ceil((f_stop - f_start) / f_step) + 1;
			f_hh4lo_start = f_start - H_lo1 -f_step; //-f_step
			f_hh4lo_stop = (rfData.N_HH4 - 1) * f_step + f_start - H_lo1;
			rfData.N_QSTEP = (unsigned int)(pow2_24 * (f_step / (2 * 50e6)) + 0.5);
		}
		rfData.N_HH4_START_INT = (unsigned int)floor(f_hh4lo_start / (2 * 50e6));
		rfData.N_HH4_START_FRAC = (unsigned int)(pow2_32 * (f_hh4lo_start / (2 * 50e6) - rfData.N_HH4_START_INT) + 0.5);
		rfData.N_HH4_STOP_INT = (unsigned int)floor(f_hh4lo_stop / (2 * 50e6));
		rfData.N_HH4_STOP_FRAC = (unsigned int)(pow2_32 * (f_hh4lo_stop / (2 * 50e6) - rfData.N_HH4_STOP_INT) + 0.5);

	}

}

//第一本振下发
void tSysScpi::downLo1(void)
{
	//feDownload(0xfff1, 0x0c007a3c);  //配置本振,下发REG6

						//*****混频低波段参数*****//
	//混频低波段起始频率
	feDownload(212, rfData.N_HL_START_INT & 0xffff);
	feDownload(213, (rfData.N_HL_START_FRAC >> 16) & 0xffff);
	feDownload(214, rfData.N_HL_START_FRAC & 0xffff);

	//混频低波段终止频率
	feDownload(253, rfData.N_HL_STOP_INT & 0xffff);
	feDownload(254, (rfData.N_HL_STOP_FRAC >> 16) & 0xffff);
	feDownload(255, rfData.N_HL_STOP_FRAC & 0xffff);
	
	//第一本振低波段步进次数
	feDownload(221, (rfData.N_HL >> 16) & 0xff);
	feDownload(222, rfData.N_HL & 0xffff);
	
	//本振步进量
	if(sysData.fscan.mode == fsFFT)
	{
		feDownload(215, (rfData.N_QSTEP >> 16) & 0xffff);
		feDownload(216, rfData.N_QSTEP & 0xffff);
	} else
	{
		feDownload(215, (rfData.N_QSTEP >> 16) & 0xff);
		feDownload(216, rfData.N_QSTEP & 0xffff);
	}


						//******混频高波段L段参数******//
	//混频高波段起始频率
	feDownload(217, rfData.N_HH_START_INT & 0xffff);
	feDownload(218, (rfData.N_HH_START_FRAC >> 16) & 0xffff);
	feDownload(219, rfData.N_HH_START_FRAC & 0xffff);

	//混频高波段L段终止频率
	feDownload(256, rfData.N_HH_STOP_INT & 0xffff);
	feDownload(257, (rfData.N_HH_STOP_FRAC >> 16) & 0xffff);
	feDownload(258, rfData.N_HH_STOP_FRAC & 0xffff);
	
	//第一本振高波段步进次数
	feDownload(223, (rfData.N_HH >> 16) & 0xff);
	feDownload(224, rfData.N_HH & 0xffff);
	
	//第一本振高波段L段次数
	feDownload(225, (rfData.N_HH1 >> 16) & 0xff);
	feDownload(226, rfData.N_HH1 & 0xffff);

	//第一本振高波段L段第二段
	feDownload(262, (rfData.N_HH2 >> 16) & 0xff);
	feDownload(263, rfData.N_HH2 & 0xffff);	


						//******混频高波段H段参数******//
	//混频高波段H段次数
	feDownload(245, rfData.N_HH4_START_INT & 0xffff);
	feDownload(246, (rfData.N_HH4_START_FRAC >> 16) & 0xffff);
	feDownload(247, rfData.N_HH4_START_FRAC & 0xffff);

	//混频高波段H段终止频率
	feDownload(259, rfData.N_HH4_STOP_INT & 0xffff);
	feDownload(260, (rfData.N_HH4_STOP_FRAC >> 16) & 0xffff);
	feDownload(261, rfData.N_HH4_STOP_FRAC & 0xffff);
	
	//第一本振高波段H段次数
	feDownload(227, (rfData.N_HH4 >> 16) & 0xff);
	feDownload(228, rfData.N_HH4 & 0xffff);

#if 0
	__var(rfData.N_HL_START_INT);usleep(1000);
	__var(rfData.N_HL_START_FRAC);usleep(1000);
	__var(rfData.N_HL_STOP_INT);usleep(1000);
	__var(rfData.N_HL_STOP_FRAC);usleep(1000);
	__var(rfData.N_HL);usleep(1000);
	__var(rfData.N_QSTEP);usleep(1000);
	__var(rfData.N_HH_START_INT);usleep(1000);
	__var(rfData.N_HH_START_FRAC);usleep(1000);
	__var(rfData.N_HH_STOP_INT);usleep(1000);
	__var(rfData.N_HH_STOP_FRAC);usleep(1000);
	__var(rfData.N_HH);usleep(1000);
	__var(rfData.N_HH1);usleep(1000);
	__var(rfData.N_HH2);usleep(1000);
	__var(rfData.N_HH4_START_INT);usleep(1000);
	__var(rfData.N_HH4_START_FRAC);usleep(1000);
	__var(rfData.N_HH4_STOP_INT);usleep(1000);
	__var(rfData.N_HH4_STOP_FRAC);usleep(1000);
	__var(rfData.N_HH4);usleep(1000);
	__var(sysData.fscan.step);usleep(1000);
	__var(rfData.DBUC_FREQ);usleep(1000);
#endif
}

//波段指示下发
inline void tSysScpi::setSweepBandIndex(void)
{
	feDownload(231, rfData.SWEEP_BAND_INDEX);
}

//高波段指示下发
inline void tSysScpi::setHighBandIndex(void)
{
	feDownload(232,rfData.HIGH_BAND_INDEX);
}

/*控制参数下发*/
void tSysScpi::setFTWOfDDS(void)
{
	feDownload(6, 1);  //DDS装载使能
	feDownload(4, (rfData.FTW_Start >> 16) & 0xffff);   //FTW 高16位
	feDownload(5, rfData.FTW_Start & 0xffff);           //FTW 低16位

	feDownload(210, (rfData.FTW_Step >> 16) & 0xffff);  // 短波段步进量 高16位
	feDownload(211, rfData.FTW_Step & 0xffff);			// 短波段步进量 低16位
	feDownload(220, (rfData.N_L >> 16) & 0xffff);			  // 短波段 步进次数高16位
	feDownload(235, rfData.N_L & 0xffff);								// 短波段 步进次数低16位
	feDownload(6, 0);
}

//本振初次步进使能
inline void tSysScpi::restartScan(void)
{
	feDownload(244, 1);
	usleep(1);
	feDownload(244, 0);
}

//频率扫描使能
inline void tSysScpi::setScanEnable(bool value)
{
	feDownload(229, value);
}

//fft控制
void tSysScpi::fftControl(void)
{

	sysData.fscan.fftFrame = rfData.N_L + rfData.N_HL + rfData.N_HH + rfData.N_HH4;
	if(sysData.fscan.mode == fsSweep)
	{
		if(rfData.DBUC_FREQ < 250e3)
			sysData.detector.length = 5;
		else
			sysData.detector.length = ceil(rfData.DBUC_FREQ / 50000);

		sysData.fscan.fftRealN = sysData.fscan.fftFrame; //sysData.detector.length * RAMDATASIZE;
		sysData.fscan.fftIdealN = sysData.fscan.fftRealN;
	}	else
	{
		sysData.fscan.fftRealN = (unsigned int)(((sysData.fscan.fftFrame - 1) + ((sysData.freq.stop - (sysData.fscan.fftFrame - 1) * sysData.fscan.step - sysData.freq.start) * 1.0 / sysData.fscan.step)) * (sysData.fscan.fft / 1.28) + 0.5);
		sysData.fscan.fftIdealN = (unsigned int)(sysData.fscan.fftFrame * sysData.fscan.fft / 1.28);
	}

	if(sysData.span.isZeroSpan)
	{
		//sysData.fscan.fftRealN = round(sysData.sweep.sweepTime / sysData.fscan.sampleTime_ZeroSpan);
		sysData.fscan.fftRealN = round(sysData.sweep.sweepTime * sysData.fscan.ZeroSpan_dataspeed);
		sysData.fscan.fftIdealN = sysData.fscan.fftRealN;
	}

//	//fft复位
//	feDownload(15, 1);
//	usleep(1);
//	feDownload(15, 0);
//	usleep(1000);

	//初始配置有效控制使能
	feDownload(14, 1);
	usleep(1);
	feDownload(14, 0);

	//频谱仪执行完一次扫描的帧数
	feDownload(20, (sysData.fscan.fftFrame >> 16) & 0xff);    //fft帧数 高16位
	feDownload(21, sysData.fscan.fftFrame & 0xffff);		  //fft帧数 低16位

	//频谱仪执行完一次扫描的实际总频点数
	feDownload(22, (sysData.fscan.fftRealN >> 16) & 0xffff);  // 高16位
	feDownload(23, sysData.fscan.fftRealN & 0xffff);		  // 低16位

	//FFT 理论扫描总频点数
	feDownload(24, (sysData.fscan.fftIdealN >> 16) & 0xffff);  // 高16位
	feDownload(25, sysData.fscan.fftIdealN & 0xffff);		   // 低16位

}

int tSysScpi::setUdpIP(QString value)
{
	if (value.isNull() || value.isEmpty() || QHostAddress(value.trimmed()).isNull())
	{
		return __SCPI_FAILED;
	}

	strcpy(sysData.system.io.lan.udpRemoteIP, value.trimmed().toStdString().c_str());
//	saveDeviceSetting();

	unsigned int val = QHostAddress(value.trimmed()).toIPv4Address();

	ramDownload(0xE0,val);

	return __SCPI_SUCCESS;
}

int tSysScpi::setUdpPort(QString value)
{
	if (value.isNull() || value.isEmpty())
	{
		return __SCPI_FAILED;
	}

	bool isOk = false;
	int intValue = 0;

	intValue = value.toInt(&isOk);

	if (!isOk)
	{
		return __SCPI_FAILED;
	}

	if (intValue <= 1000)
	{
		intValue = 1000;
	} else if (intValue >= 10000)
	{
		intValue = 10000;
	}

	sysData.system.io.lan.udpRemotePort = intValue;
//	saveDeviceSetting();

	ramDownload(0xE1,intValue);

	return __SCPI_SUCCESS;
}

//设置USB功率计频率
int tSysScpi::setPowerMeterFreq(double value)
{
	char sendString[128] = { 0 };
	char tempChar[32] = { 0 };
	int length = 0;
	if(value > POWERMETER_FREQ_MAX)
		value = POWERMETER_FREQ_MAX;

	if(value < POWERMETER_FREQ_MIN)
		value = POWERMETER_FREQ_MIN;

	//set freq
	QString cmd;
	//write(tmcHandle, "SYST:PRES DEF;CAL:ZERO:TYPE INT;CAL;", 512);
	cmd = "SYST:PRES DEF;SENS:FREQ " + ((QString)getPMFreqAndUnitString(value, 0, tempChar)).trimmed() + ";CAL:ZERO:TYPE INT;CAL;INIT:CONT ON;";
//	cmd = "SENS:FREQ " + ((QString)getPMFreqAndUnitString(value, 0, tempChar)).trimmed();
	length = cmd.size();
	memcpy(sendString, cmd.toStdString().c_str(), length);
		
	write(tmcHandle, sendString, length);
	usleep(1000);
	printf("power sensor freq set %f success!\n",value);
	return __SCPI_SUCCESS;
}

int tSysScpi::setPowerMeterFreq(QString value)
{
	bool isOk = false;
	double tempValue = 0;

	tempValue = getFreqValue(value, &isOk);

	if (isOk)
	{
		return setPowerMeterFreq(tempValue);
	} else
	{
		return __SCPI_FAILED;
	}
}

//获取功率计数据
int tSysScpi::getDataFromPowerMeter(void)
{
	char usbDataTempR[512];
	unsigned char sendString[64] = { 0 };
	double maxVal = 0, minVal = 0;
	double temp = 0;
	double val[5] = {0};
	int count = 0;
	for(count = 0; count < 5;)
	{
		usleep(1000*10);
		write(tmcHandle, "FETCH?", 6);

		memset(usbDataTempR, 0, sizeof(usbDataTempR));
	  read(tmcHandle, usbDataTempR, sizeof(usbDataTempR));
	  bool ok = false;
	  temp = QString(usbDataTempR).toDouble(&ok);
	  if(ok)
	  {
	  	val[count] = temp;
	  	count++;
	  	usleep(1000);
	  } else
	  {
	  	return __SCPI_FAILED;
	  }
	}

	maxVal = val[0];
	minVal = val[0];
	temp = 0;
	for(int i = 0; i < 5; i++)
	{
		if(val[i] > maxVal)
			maxVal = val[i];
		if(val[i] < minVal)
			minVal = val[i];
		temp += val[i];
	}
	sysData.measure.powerMeter.ampt = (temp - maxVal - minVal) / 3.0;
	__var(sysData.measure.powerMeter.ampt);
//	sysData.measure.powerMeter.ampt = temp;
	return __SCPI_SUCCESS;
}

//连接usb功率计
bool tSysScpi::connectToPowerMeter(void)
{
	if(!tmcHandle)
		tmcHandle = open("/dev/usbtmc0", O_RDWR | O_NONBLOCK);		

	if(tmcHandle > 0)
	{
		printf("open USB powermeter succeed!\n");
		write(tmcHandle, "SYST:PRES DEF;CAL:ZERO:TYPE INT;CAL;", 512);

		return true;
	} else
	{
		return false;
	}
}

//发送数据
bool tSysScpi::usbDeviceSend(unsigned char* data, int len, int endpoint)
{
	if ((not usbDeviceInitedSuccess) || (dev_handle == NULL))
	{
		return false;
	}

	int actual = 0;
	int r = libusb_bulk_transfer(dev_handle, endpoint, data, len, &actual, 2000);

	return (r == 0) && (actual == len);
}

//接收数据
bool tSysScpi::usbDeviceRecv(unsigned char* data, int dataLen, int* recvLen, int endpoint)
{
	if ((not usbDeviceInitedSuccess) || (dev_handle == NULL))
	{
		return false;
	}

	int r = 0;
	int actual = 0;

	r = libusb_bulk_transfer(dev_handle, endpoint, data, dataLen, &actual, 3000);
	if ((r == 0) && (actual > 0))
	{
		*recvLen = actual;
		return true;
	} else
	{
		*recvLen = 0;
		return false;
	}
}
