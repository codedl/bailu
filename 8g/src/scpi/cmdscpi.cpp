#include "sysscpi.h"

//处理SCPI指令
void tSysScpi::handleScpiCommand(QString cmdStr)
{
	QString tempStr = cmdStr;
	QStringList cmdList;
	QString command = "";
	resultDef result;
	QString updateDir = "";
	QString updateStr = "";
	char tempChar[32] = { };
	int exeResult = __SCPI_FAILED;
	int tempIndex = 0;
	QString returnString = "";
	QString tempValue = "";
	bool ok;

	char socketData[sysData.sweep.sweepPoints * 4 + 2];
	char *tempData;
	float temp = 0.0;

	//异常处理
	if (!isUpdating)
	{
		if (cmdStr.isNull() || cmdStr.isEmpty() || cmdStr.length() > 1024)
		{
			scpiBuffer = "";
			return;
		}
	}

	//多帧处理
	for (int i = 0; i < tempStr.length(); i++)
	{
		if (tempStr.at(i) == QChar(';') || tempStr.at(i) == QChar(0x0d) || tempStr.at(i) == QChar(0x0a))
		{
			if (tempStr.at(i) != QChar(';'))
			{
				tempStr.replace(i, 1, ";");
			}
		}
	}

	//拼帧处理
	cmdList = tempStr.split(";");

	if (tempStr.at(tempStr.length() - 1) != QChar(';'))
	{
		if (cmdList.count() <= 1)
		{
			scpiBuffer = tempStr;
		} else
		{
			scpiBuffer = cmdList.at(cmdList.count() - 1);
		}

		cmdList.removeAt(cmdList.count() - 1);
	} else
	{
		scpiBuffer = "";
	}

	int auxInt = 0;
	bool auxBool = 0;
	int traceInt = 0;
	bool traceBool = 0;
	QString tempDemodFreq = "";
	QString demodFreq = "";
	QStringList tempDemodFreqValue;

	//指令解析
	for (int i = 0; i < cmdList.count(); i++)
	{
		command = cmdList.at(i).trimmed();

		if (command.isNull() || command.isEmpty())
		{
			continue;
		}

		////prevent error resolve, eg. "xxx?;"
		if (command.at(0) != QChar(':') && command.at(0) != QChar('*'))
		{
			command = ":" + command;
		}

		memset(parseCmd, 0, sizeof parseCmd);
		returnString = "";
		exeResult = __SCPI_FAILED;
		result = GetFunctionCode(command);

		if (__CMDDEBUG)
		{
			printf("cmd[%d] = %s , cmdCode = %x\r\n", i, command.toStdString().c_str(), result.cmdCode);
		}

		//update command
		switch (result.cmdCode)
		{
			case SCPI_CMD_UPDATE_BEGIN:
				exeResult = __SCPI_RETURN;
				returnString = "NO";
				isUpdating = true;
				isRcvingFile = false;
				outputToScreen(getTheTranslatorString("Updating System"), "");
				updateDir = QCoreApplication::applicationDirPath() + "/update";

				if (!QDir(updateDir).exists())
				{
					system(QString("mkdir " + updateDir).toStdString().c_str());
				}

				if (QDir(updateDir).exists())
				{
					system(QString("chmod 777 " + updateDir).toStdString().c_str());
					system(QString("rm " + updateDir + "/*").toStdString().c_str());

					if (updateRecordOfClear())
					{
						returnString = "YES";
					} else
					{
						isUpdating = false;
					}
				}

				break;
			case SCPI_CMD_UPDATE_FILE:
				exeResult = __SCPI_RETURN;
				returnString = "NO";

				if (updateRecordOfAdd(result.value.trimmed()))
				{
					returnString = "YES";
					outputToScreen(getTheTranslatorString("Updating System"), updateFileName(result.value.trimmed()) + "......");
					isRcvingFile = true;
				} else
				{
					isRcvingFile = false;
				}
				break;
			case SCPI_CMD_UPDATE_END:
				isRcvingFile = false;
				isUpdating = false;
				updateSystem();
				exeResult = __SCPI_RETURN;
				returnString = "YES";

				QApplication::processEvents();
				switch (exeResult)
				{
					case __SCPI_RETURN:
						returnScpiResult(returnString);
						break;
					case __SCPI_FAILED:
						returnScpiResult((QString) SCPICMDERROR);
						break;
					case __SCPI_UNSUPPORT:
						returnScpiResult((QString) SCPICMDUNSUPPORT);
						break;
				}
				QApplication::processEvents();

				getLocalDateTime(tempChar);
				saveLogOfFactoryCalibrate(QString(tempChar).trimmed(), getTheTranslatorString("Update success to") + result.value.trimmed(), getTheTranslatorString("Old Version is") + versionString);
				outputToScreen(getTheTranslatorString("Updating Success") + "(" + result.value.trimmed() + "), " + getTheTranslatorString("Rebooting") + "......", "");
				usleep(1000 * 2000);
				system("reboot&");
				return;
			case SCPI_CMD_UPDATE_CANCEL:
				isRcvingFile = false;
				isUpdating = false;
				exeResult = __SCPI_RETURN;
				returnString = "YES";
				break;
			case SCPI_CMD_UPDATE_CRC:
				exeResult = __SCPI_RETURN;
				returnString = "0";

				if (QFile(updatingFileName).exists())
				{
					returnString = QString(longLongIntToString(QFile(updatingFileName).size(), tempChar)).trimmed();
				}

				break;
		}

		if (!isUpdating)
			switch (result.cmdCode)
			{
				case SCPI_CMD_UPDATE_CANCEL:
					isRcvingFile = false;
					isUpdating = false;
					exeResult = __SCPI_RETURN;
					returnString = "YES";
					break;

					//standard command
				case SCPI_CMD_STANDARD_CLS:
					scpiBuffer = "";
					gpibReaded = false;
					gpibList.clear();
					exeResult = __SCPI_SUCCESS;
					break;
				case SCPI_CMD_STANDARD_IDN_QUERY:
					exeResult = __SCPI_RETURN;
					returnString = (QString) idnHead + ", " + devType + ", SN" + QString(sysData.system.serialNo).trimmed() + ", " + versionString;
					break;
				case SCPI_CMD_STANDARD_RST:
					exeResult = presetSystem();
					break;
				case SCPI_CMD_STANDARD_VER:
					exeResult = __SCPI_RETURN;
					returnString = versionString;
					break;

					//cal
				case SCPI_CMD_STANDARD_CAL:
					exeResult = __SCPI_RETURN;
					if (factoryCalibrate(result.value.toInt()) == __SCPI_SUCCESS)
					{
						presetSystem();
					}
					break;
				case SCPI_CMD_STANDARD_ZCCAL:
					exeResult = __SCPI_RETURN;
					printf("SCPI_CMD_STANDARD_ZCCAL!\n");
					if (ZCCalibrate(result.value.toInt()) == __SCPI_SUCCESS)
					{
						presetSystem();
					}					
					break;
				case SCPI_CMD_STANDARD_PRECAL:
					exeResult = __SCPI_RETURN;
					if (PreamplifierCalibrate(result.value.toInt()) == __SCPI_SUCCESS)
					{
						presetSystem();
					}
					break;

					//清除校准数据
				case SCPI_CMD_STANDARD_CLEAR:
					exeResult = __SCPI_RETURN;
					clearFactoryCalibrateData();
					presetSystemData();
					break;

				case SCPI_CMD_STANDARD_CAL_9KHZ_SET:
					exeResult = modifyFrequencyRespondOf9kHz(result.value.trimmed());
					break;
				case SCPI_CMD_STANDARD_CAL_9KHZ_GET:
					exeResult = __SCPI_RETURN;
					returnString = QString(floatToString(sysData.factoryCalData.freqResp[0][0], 0, 0, tempChar)).trimmed();
					break;

				case SCPI_CMD_STANDARD_CAL_20KHZ_SET:
					exeResult = modifyFrequencyRespondOf20kHz(result.value.trimmed());
					break;
				case SCPI_CMD_STANDARD_CAL_20KHZ_GET:
					exeResult = __SCPI_RETURN;
					returnString = QString(floatToString(sysData.factoryCalData.freqResp[0][1], 0, 0, tempChar)).trimmed();
					break;

					//register command
				case SCPI_CMD_STANDARD_REG:
					exeResult = __SCPI_RETURN;

					if (result.value.toUpper().trimmed() == getSystemServiceOfMD5(sysData.system.serialNo, false).toUpper().trimmed())
					{
						unLockSystem();
						returnString = getTheTranslatorString("Register Success");
					} else
					{
						returnString = getTheTranslatorString("Register Failed");
					}
					break;

					//freq command
				case SCPI_CMD_FREQ_CENTER_SET:
					exeResult = setFrequencyOfCenter(result.value.trimmed());
					//valuechanged = 1;
					//reDrawMenuFrame();
					break;
				case SCPI_CMD_FREQ_CENTER_GET:
					exeResult = __SCPI_RETURN;
					returnString = QString(floatToString(sysData.freq.center, 0, 0, tempChar)).trimmed();
				//	sysData.fscan.centerFreq = sysData.fscan.startFreq + (sysData.fscan.stopFreq - sysData.fscan.startFreq ) / 2;
				//	returnString = QString(floatToString((sysData.fscan.stopFreq - sysData.fscan.startFreq ) / 2, 0, 0, tempChar)).trimmed();
					break;
				case SCPI_CMD_FREQ_START_SET:
					exeResult = setFrequencyOfStart(result.value.trimmed());
					//reDrawMenuFrame();
					break;
				case SCPI_CMD_FREQ_START_GET:
					exeResult = __SCPI_RETURN;
					returnString = QString(floatToString(sysData.freq.start, 0, 0, tempChar)).trimmed();
					//returnString = QString(floatToString(sysData.fscan.startFreq, 0, 0, tempChar)).trimmed();
					break;
				case SCPI_CMD_FREQ_STOP_SET:
					exeResult = setFrequencyOfStop(result.value.trimmed());
					//reDrawMenuFrame();
					break;
				case SCPI_CMD_FREQ_STOP_GET:
					exeResult = __SCPI_RETURN;
					returnString = QString(floatToString(sysData.freq.stop, 0, 0, tempChar)).trimmed();
				  //returnString = QString(floatToString(sysData.fscan.stopFreq, 0, 0, tempChar)).trimmed();
					break;
				case SCPI_CMD_FREQ_CFSTEP_SET:
					exeResult = setFrequencyOfCenterStep(result.value.trimmed());
					//reDrawMenuFrame();
					break;
				case SCPI_CMD_FREQ_CFSTEP_GET:
					exeResult = __SCPI_RETURN;
					returnString = QString(floatToString(sysData.freq.cfStep, 0, 0, tempChar)).trimmed();
					break;
				case SCPI_CMD_FREQ_CFSTEP_AUTO_SET:
					exeResult = setFrequencyOfCenterStepAuto(result.value.trimmed());
					//reDrawMenuFrame();
					break;
				case SCPI_CMD_FREQ_CFSTEP_AUTO_GET:
					exeResult = __SCPI_RETURN;
					returnString = boolToString(sysData.freq.cfStepAuto);
					break;
				case SCPI_CMD_FREQ_OFFSET_SET:
					exeResult = setFrequencyOffset(result.value.trimmed());
					//reDrawMenuFrame();
					break;
				case SCPI_CMD_FREQ_OFFSET_GET:
					exeResult = __SCPI_RETURN;
					returnString = QString(floatToString(sysData.freq.offset, 0, 0, tempChar)).trimmed();
					break;
				case SCPI_CMD_FREQ_REFERENCE_SET:
					exeResult = setFrequencyOfReference(result.value.toUpper().trimmed() == "INT" || result.value.toUpper().trimmed() == "INTERNAL");
					//reDrawMenuFrame();
					break;
				case SCPI_CMD_FREQ_REFERENCE_GET:
					exeResult = __SCPI_RETURN;
					if (sysData.freq.refInternal)
					{
						returnString = "INTernal";
					} else
					{
						returnString = "EXTernal";
					}
					break;
				case SCPI_CMD_FREQ_SCALETYPE_SET:
					exeResult = setFrequencyOfScaleType(result.value.toUpper().trimmed());
					//reDrawMenuFrame();
					break;
				case SCPI_CMD_FREQ_SCALETYPE_GET:
					exeResult = __SCPI_RETURN;
					if (sysData.freq.typeLine)
					{
						returnString = "LINear";
					} else
					{
						returnString = "LOGarithmic";
					}
					break;
					//span command
				case SCPI_CMD_SPAN_SET:
					exeResult = setFrequencyOfSpan(result.value.trimmed());
					//reDrawMenuFrame();
					break;
				case SCPI_CMD_SPAN_GET:
					exeResult = __SCPI_RETURN;
					returnString = QString(floatToString(sysData.span.span, 0, 0, tempChar)).trimmed();
					//returnString = QString(floatToString(sysData.fscan.stopFreq - sysData.fscan.startFreq, 0, 0, tempChar)).trimmed();
					break;
				case SCPI_CMD_SPAN_FULL:
					exeResult = setFrequencyOfSpan(MAXSPAN);
					//reDrawMenuFrame();
					break;
				case SCPI_CMD_SPAN_ZERO:
					exeResult = setFrequencyOfSpan(0);
					//reDrawMenuFrame();
					break;
				case SCPI_CMD_SPAN_PREVIOUS:
					exeResult = setFrequencyOfSpan(sysData.span.prvSpan);
					//reDrawMenuFrame();
					break;

					//ampt
				case SCPI_CMD_AMPT_REFLEVEL_SET:
					exeResult = setAmptOfRefLevel(result.value.toUpper().trimmed());
					//reDrawMenuFrame();
					break;
				case SCPI_CMD_AMPT_REFLEVEL_GET:
					exeResult = __SCPI_RETURN;
					returnString = QString(floatToStringDot2(getdBmValue(sysData.ampt.refLevel), tempChar)).trimmed();
					break;
				case SCPI_CMD_AMPT_ATT_SET:
					//printf("set att %s\r\n",result.value.toStdString());
					exeResult = setAmptOfAtt(result.value.toUpper().trimmed());
					//reDrawMenuFrame();
					break;
				case SCPI_CMD_AMPT_ATT_GET:
					exeResult = __SCPI_RETURN;
					returnString = QString(intToString((int) sysData.ampt.att, tempChar)).trimmed();
					break;
				case SCPI_CMD_AMPT_ATT_AUTO_SET:
					exeResult = setAmptOfAttAuto(result.value.toUpper().trimmed());
					//reDrawMenuFrame();
					break;
				case SCPI_CMD_AMPT_ATT_AUTO_GET:
					exeResult = __SCPI_RETURN;
					returnString = boolToString(sysData.ampt.attAuto);
					break;

				case SCPI_CMD_AMPT_PREAMPT_SET:
					exeResult = setPreamptOfAttAuto(result.value.toUpper().trimmed());
					//reDrawMenuFrame();
					break;
				case SCPI_CMD_AMPT_PREAMPT_GET:
					exeResult = __SCPI_RETURN;
					returnString = boolToString(sysData.ampt.isPreamptOn);
					break;

				case SCPI_CMD_AMPT_SCALEDIV_SET:
					exeResult = setAmptOfScaleDiv(result.value.toUpper().trimmed());
					//reDrawMenuFrame();
					break;
				case SCPI_CMD_AMPT_SCALEDIV_GET:
					exeResult = __SCPI_RETURN;
					returnString = QString(floatToStringDot2(sysData.ampt.scaleDiv, tempChar)).trimmed();
					break;
				case SCPI_CMD_AMPT_SCALETYPE_SET:
					exeResult = setAmptOfScaleType(result.value.toUpper().trimmed());
					//reDrawMenuFrame();
					break;
				case SCPI_CMD_AMPT_SCALETYPE_GET:
					exeResult = __SCPI_RETURN;
					if (sysData.ampt.scaleLine)
					{
						returnString = "LINear";
					} else
					{
						returnString = "LOGarithmic";
					}
					break;
				case SCPI_CMD_AMPT_REFLEVEL_OFFSET_SET:
					exeResult = setAmptOfRefOffset(result.value.toUpper().trimmed());
					//reDrawMenuFrame();
					break;
				case SCPI_CMD_AMPT_REFLEVEL_OFFSET_GET:
					exeResult = __SCPI_RETURN;
					returnString = QString(floatToStringDot2(sysData.ampt.refOffset, tempChar)).trimmed();
					break;
				case SCPI_CMD_AMPT_REFUNIT_SET:
					exeResult = setAmptOfRefUnit(result.value.toUpper().trimmed());
					//reDrawMenuFrame();
					break;
				case SCPI_CMD_AMPT_REFUNIT_GET:
					exeResult = __SCPI_RETURN;
					returnString = "dBm";
					switch (sysData.ampt.refUnit)
					{
						case rudBmV:
							returnString = "dBmV";
							break;
						case rudBuV:
							returnString = "dBuV";
							break;
						case ruW:
							returnString = "mW";
							break;
						case ruV:
							returnString = "mV";
							break;
						case rudBuW:
							returnString = "dBuW";
							break;
						case rudBpW:
							returnString = "dBpW";
							break;
					}
					break;

					//tune
				case SCPI_CMD_TUNE_SET:
					exeResult = setAutoTune();
					break;
				case SCPI_CMD_TUNE_GET:
					exeResult = __SCPI_RETURN;
					returnString = boolToString(sysData.isAutoSearching);
					break;

					//bw
				case SCPI_CMD_BW_RBW_SET:
					exeResult = setBwOfRbw(result.value.toUpper().trimmed());
					//reDrawMenuFrame();
					break;
				case SCPI_CMD_BW_RBW_GET:
					exeResult = __SCPI_RETURN;
					returnString = QString(floatToString(sysData.bw.rbw, 0, 0, tempChar)).trimmed();
					//returnString = QString(floatToString(sysData.fscan.rbw, 0, 0, tempChar)).trimmed();
					break;
				case SCPI_CMD_BW_RBW_AUTO_SET:
					exeResult = setBwOfRbwAuto(result.value.toUpper().trimmed());
					//reDrawMenuFrame();
					break;
				case SCPI_CMD_BW_RBW_AUTO_GET:
					exeResult = __SCPI_RETURN;
					returnString = boolToString(sysData.bw.rbwAuto);
					break;
				case SCPI_CMD_BW_RBW_STEP_MODE_SET:
					exeResult = setBwOfRbwStepMode(result.value.toUpper().trimmed());
					//reDrawMenuFrame();
					break;
				case SCPI_CMD_BW_RBW_STEP_MODE_GET:
					exeResult = __SCPI_RETURN;
					if (sysData.bw.rbwStepCont)
					{
						//   	      returnString = "CONTinuous"; //新框架
						returnString = "1"; //老框架
					} else
					{
						//   	      returnString = "DEFault"; //新框架
						returnString = "0";//老框架
					}
					break;
				case SCPI_CMD_BW_VBW_SET:
					exeResult = setBwOfVbw(result.value.toUpper().trimmed());
					//reDrawMenuFrame();
					break;
				case SCPI_CMD_BW_VBW_GET:
					exeResult = __SCPI_RETURN;
					returnString = QString(floatToString(sysData.bw.vbw, 0, 0, tempChar)).trimmed();
					break;
				case SCPI_CMD_BW_VBW_AUTO_SET:
					exeResult = setBwOfVbwAuto(result.value.toUpper().trimmed());
					//reDrawMenuFrame();
					break;
				case SCPI_CMD_BW_VBW_AUTO_GET:
					exeResult = __SCPI_RETURN;
					returnString = boolToString(sysData.bw.vbwAuto);
					break;
				case SCPI_CMD_BW_AVERAGE_COUNT_SET:
					exeResult = setBwOfAverage(result.value.toUpper().trimmed());
					//reDrawMenuFrame();
					break;
				case SCPI_CMD_BW_AVERAGE_COUNT_GET:
					exeResult = __SCPI_RETURN;
					returnString = QString(intToString(sysData.bw.bwAverage, tempChar)).trimmed();
					break;
				case SCPI_CMD_BW_AVERAGE_STATE_SET:
					exeResult = setBwOfAverageSwitch(result.value.toUpper().trimmed());
					//reDrawMenuFrame();
					break;
				case SCPI_CMD_BW_AVERAGE_STATE_GET:
					exeResult = __SCPI_RETURN;
					returnString = boolToString(sysData.bw.bwAverageOn);
					break;

				case SCPI_CMD_BW_EMC_SET:
					exeResult = setBwOfRbw(result.value.toUpper().trimmed());
					//reDrawMenuFrame();
					break;
				case SCPI_CMD_BW_EMC_GET:
					exeResult = __SCPI_RETURN;
					returnString = QString(floatToString(sysData.bw.rbw, 0, 0, tempChar)).trimmed();
					break;
				case SCPI_CMD_BW_EMC_STATE_SET:
					exeResult = setBwOfEmcState(result.value.toUpper().trimmed());
					break;
				case SCPI_CMD_BW_EMC_STATE_GET:
					exeResult = __SCPI_RETURN;
					returnString = boolToString(sysData.emc.emcOn);
					break;

					//sweep
				case SCPI_CMD_SWEEP_TIME_SET:// get ns of sysData.sweep.sweepTime 
					exeResult = setSweepOfTime(result.value.toUpper().trimmed());
					//reDrawMenuFrame();
					break;
				case SCPI_CMD_SWEEP_TIME_GET:// make sysData.sweep.sweepTime to ms
					exeResult = __SCPI_RETURN;
					returnString = QString(floatToStringDot3(sysData.sweep.sweepTime * 1e-3, tempChar)).trimmed();
					break;
				case SCPI_CMD_SWEEP_TIME_AUTO_SET:
					exeResult = setSweepOfTimeAuto(result.value.toUpper().trimmed());
					//reDrawMenuFrame();
					break;
				case SCPI_CMD_SWEEP_TIME_AUTO_GET:
					exeResult = __SCPI_RETURN;
					returnString = boolToString(sysData.sweep.sweepTimeAuto);
					break;
				case SCPI_CMD_SWEEP_MODE_SET:
					exeResult = setSweepOfMode(result.value.toUpper().trimmed());
					//reDrawMenuFrame();
					break;
				case SCPI_CMD_SWEEP_MODE_GET:
					exeResult = __SCPI_RETURN;
					returnString = boolToString(!sysData.sweep.sweepSingle);
					break;
				case SCPI_CMD_SWEEP_POINTS_SET:
					exeResult = setSweepOfPoints(result.value.toUpper().trimmed());
					//reDrawMenuFrame();
					break;
				case SCPI_CMD_SWEEP_POINTS_GET:
					exeResult = __SCPI_RETURN;
					returnString = QString(intToString(sysData.sweep.sweepPoints, tempChar)).trimmed();
					break;

					//meas & meassetup
				case SCPI_CMD_MEAS_SET_ST:
					exeResult = setMeasureOfTimeSpectrumOn(1);
					//reDrawMenuFrame();
					break;
				case SCPI_CMD_MEAS_SET_ACP:
					exeResult = setMeasureOfAcprOn(1);
					//reDrawMenuFrame();
					break;
				case SCPI_CMD_MEAS_SET_CHP:
					exeResult = setMeasureOfChannelPowerOn(1);
					//reDrawMenuFrame();
					break;
				case SCPI_CMD_MEAS_SET_OBW:
					exeResult = setMeasureOfObwOn(1);
					//reDrawMenuFrame();
					break;
				case SCPI_CMD_MEAS_SET_DISC:
					break;
				case SCPI_CMD_MEAS_SET_EMC:
					break;
				case SCPI_CMD_MEAS_SET_SA:
					exeResult = setMeasureAllOff();
					//reDrawMenuFrame();
					break;
				case SCPI_CMD_MEAS_GET:
					exeResult = __SCPI_RETURN;
					returnString = getSystemMeasureModeName();
					break;

				case SCPI_CMD_MEASSETUP_ACP_BANDWIDTH_INT_SET:
					exeResult = setMeasSetupOfChannelWidth(result.value.toUpper().trimmed());
					break;
				case SCPI_CMD_MEASSETUP_ACP_BANDWIDTH_INT_GET:
					exeResult = __SCPI_RETURN;
					returnString = QString(floatToString(sysData.measureSetup.channelWidth, 0, 0, tempChar)).trimmed();
					break;
				case SCPI_CMD_MEASSETUP_ACP_CHANNEL_COUNT_SET:
					exeResult = setMeasSetupOfChannelNums(result.value.toUpper().trimmed());
					break;
				case SCPI_CMD_MEASSETUP_ACP_CHANNEL_COUNT_GET:
					exeResult = __SCPI_RETURN;
					returnString = QString(intToString(sysData.measureSetup.channelNums, tempChar)).trimmed();
					break;
				case SCPI_CMD_MEASSETUP_ACP_SPACE_SET:
					exeResult = setMeasSetupOfChannelSpace(result.value.toUpper().trimmed());
					break;
				case SCPI_CMD_MEASSETUP_ACP_SPACE_GET:
					exeResult = __SCPI_RETURN;
					returnString = QString(floatToString(sysData.measureSetup.channelSpace, 0, 0, tempChar)).trimmed();
					break;
				case SCPI_CMD_MEASSETUP_CHPOWER_FREQ_SPAN_SET:
					exeResult = setMeasSetupOfChannelPowerOfSpan(result.value.toUpper().trimmed());
					break;
				case SCPI_CMD_MEASSETUP_CHPOWER_FREQ_SPAN_GET:
					exeResult = __SCPI_RETURN;
					returnString = QString(floatToString(sysData.span.span, 0, 0, tempChar)).trimmed();
					break;
				case SCPI_CMD_MEASSETUP_OBW_PERCENT_SET:
					exeResult = setMeasSetupOfPowerPercent(result.value.toUpper().trimmed());
					break;
				case SCPI_CMD_MEASSETUP_OBW_PERCENT_GET:
					exeResult = __SCPI_RETURN;
					returnString = QString(floatToString(sysData.measureSetup.powerPercent * 0.01, 0, 4, tempChar)).trimmed();
					break;
				case SCPI_CMD_MEASSETUP_OBW_FREQ_SPAN_SET:
					exeResult = setMeasSetupOfPowerPercentOfSpan(result.value.toUpper().trimmed());
					break;
				case SCPI_CMD_MEASSETUP_OBW_FREQ_SPAN_GET:
					exeResult = __SCPI_RETURN;
					returnString = QString(floatToString(sysData.span.span, 0, 0, tempChar)).trimmed();
					break;
				case SCPI_CMD_MEASSETUP_FETCH_ACP_MAIN:
					if (sysData.measure.isAcprOn)
					{
						exeResult = __SCPI_RETURN;
						returnString = QString(floatToStringDot2(sysData.measure.acprTxChannelPower, tempChar)).trimmed();
					} else
					{
						exeResult = __SCPI_FAILED;
					}
					break;
				case SCPI_CMD_MEASSETUP_FETCH_ACP_LIST_GET:
					if (sysData.measure.isAcprOn)
					{
						exeResult = __SCPI_RETURN;
						returnString = QString(floatToStringDot2(sysData.measure.acprAdjacent[0].lower, tempChar)).trimmed() + "," + QString(floatToStringDot2(sysData.measure.acprAdjacent[0].upper, tempChar)).trimmed();

						for (int i = 1; i < sysData.measureSetup.channelNums; i++)
						{
							returnString += "," + QString(floatToStringDot2(sysData.measure.acprAdjacent[i].lower, tempChar)).trimmed() + "," + QString(floatToStringDot2(sysData.measure.acprAdjacent[i].upper, tempChar)).trimmed();
						}
					} else
					{
						exeResult = __SCPI_FAILED;
					}
					break;
				case SCPI_CMD_MEASSETUP_FETCH_ACP_LOWER:
					if (sysData.measure.isAcprOn)
					{
						exeResult = __SCPI_RETURN;
						returnString = QString(floatToStringDot2(sysData.measure.acprAdjacent[0].lower, tempChar)).trimmed();
					} else
					{
						exeResult = __SCPI_FAILED;
					}
					break;
				case SCPI_CMD_MEASSETUP_FETCH_ACP_UPPER:
					if (sysData.measure.isAcprOn)
					{
						exeResult = __SCPI_RETURN;
						returnString = QString(floatToStringDot2(sysData.measure.acprAdjacent[0].upper, tempChar)).trimmed();
					} else
					{
						exeResult = __SCPI_FAILED;
					}
					break;
				case SCPI_CMD_MEASSETUP_FETCH_CHPOWER_POWER_GET:
					if (sysData.measure.isChannelPowerOn)
					{
						exeResult = __SCPI_RETURN;
						returnString = QString(floatToStringDot2(sysData.measure.channelPower, tempChar)).trimmed();
					} else
					{
						exeResult = __SCPI_FAILED;
					}
					break;
				case SCPI_CMD_MEASSETUP_FETCH_OBW_BAND_GET:
					if (sysData.measure.isObwOn)
					{
						exeResult = __SCPI_RETURN;
						returnString = QString(floatToString(sysData.measure.obwBandWidth, 0, 0, tempChar)).trimmed();
					} else
					{
						exeResult = __SCPI_FAILED;
					}
					break;

					//trace
				case SCPI_CMD_TRACE_MODE_SET:
					auxInt = getCharValue(QString(result.auxValue2), &auxBool);

					if (auxBool)
					{
						exeResult = setTraceMode(auxInt - 1, result.value.toUpper().trimmed());
					} else
					{
						exeResult = __SCPI_FAILED;
					}
					//reDrawMenuFrame();
					break;
				case SCPI_CMD_TRACE_MODE_GET:
					exeResult = __SCPI_RETURN;
					auxInt = getCharValue(QString(result.auxValue2), &auxBool);

					if (auxBool && auxInt >= 1 && auxInt <= TRACECOUNT)
					{
						switch (sysData.trace[auxInt - 1].state)
						{
							case tsClearWrite:
								returnString = "WRITe";
								break;
							case tsMaxHold:
								returnString = "MAXHold";
								break;
							case tsMinHold:
								returnString = "MINHold";
								break;
							case tsView:
								returnString = "VIEW";
								break;
							case tsBlank:
								returnString = "BLANk";
								break;
						}
					} else
					{
						exeResult = __SCPI_FAILED;
					}
					break;

					case SCPI_CMD_TRACE_DATA_GET:
					{
					exeResult = __SCPI_RETURN;
					returnString = "";
					tempIndex = -1;

					if (result.value.toUpper().trimmed() == "TRACE1")
						tempIndex = 0;
					else if (result.value.toUpper().trimmed() == "TRACE2")
						tempIndex = 1;
					else if (result.value.toUpper().trimmed() == "TRACE3")
						tempIndex = 2;
					else if (result.value.toUpper().trimmed() == "TRACE4")
						tempIndex = 3;
					else if (result.value.toUpper().trimmed() == "TRACE5")
						tempIndex = 4;

					if (tempIndex == -1 || sysData.sweep.sweepPoints <= 0 || sysData.sweep.sweepPoints > MAXSWEEPPOINTS)
					{
						exeResult = __SCPI_FAILED;
						break;
					}
					int swppoint = sysData.sweep.sweepPoints;
					if (sysData.scpiData.interface == sitUsb || sysData.scpiData.interface == sitCom)
					{
						for (int i = 0; i < swppoint - 1; i++)
						{
							scpiFloatToLen8String(sysData.trace[tempIndex].value[i], tempChar);
							returnString += tempChar;
							returnString += ",";
						}

						scpiFloatToLen8String(sysData.trace[tempIndex].value[swppoint - 1], tempChar);
						returnString += tempChar;
					} else
					{
						if (!traceDataOrganized[tempIndex])
						{
							for (int i = 0; i < swppoint - 1; i++)
							{
								scpiFloatToLen8String(sysData.trace[tempIndex].value[i], tempChar);
								returnString += tempChar;
								returnString += ",";
							}

							scpiFloatToLen8String(sysData.trace[tempIndex].value[swppoint - 1], tempChar);
							returnString += tempChar;

							traceDataString[tempIndex] = returnString;
							//    traceDataOrganized[tempIndex] = true;
						} else
						{
							returnString = traceDataString[tempIndex];
						}
					}

					/*
					 for (int i = 0; i < sysData.sweep.sweepPoints - 1; i++)
					 {
					 scpiFloatToLen8String(sysData.trace[tempIndex].value[i], tempChar);
					 returnString += tempChar;
					 returnString += ",";
					 }

					 scpiFloatToLen8String(sysData.trace[tempIndex].value[sysData.sweep.sweepPoints - 1], tempChar);
					 returnString += tempChar;
					 */
				}
					break;

					//socket
				case SCPI_CMD_SPECIAL_TRACE_754DATA_GET:
					exeResult = __SCPI_RETURN;
					returnString = "";
					tempIndex = -1;

					if (result.value.toUpper().trimmed() == "TRACE1")
						tempIndex = 0;
					else if (result.value.toUpper().trimmed() == "TRACE2")
						tempIndex = 1;
					else if (result.value.toUpper().trimmed() == "TRACE3")
						tempIndex = 2;
					else if (result.value.toUpper().trimmed() == "TRACE4")
						tempIndex = 3;
					else if (result.value.toUpper().trimmed() == "TRACE5")
						tempIndex = 4;

					if (tempIndex == -1 || sysData.sweep.sweepPoints <= 0 || sysData.sweep.sweepPoints > MAXSWEEPPOINTS)
					{
						exeResult = __SCPI_FAILED;
						break;
					}

					/*
					 for (int i = 0; i <= sysData.sweep.sweepPoints - 1; i++)
					 {
					 temp = (float )(sysData.trace[tempIndex].value[i]);
					 tempData = (unsigned char * )( & temp);
					 socketData[4 * i] = *(tempData + 3);
					 socketData[4 * i + 1] = *(tempData + 2);
					 socketData[4 * i + 2] = *(tempData + 1);
					 socketData[4 * i + 3] = *(tempData);
					 }

					 socketData[4 * sysData.sweep.sweepPoints]= 0x0D;
					 socketData[4 * sysData.sweep.sweepPoints + 1]= 0x0A;
					 */
					if (sysData.scpiData.interface == sitUsb || sysData.scpiData.interface == sitCom)
					{
						for (int i = 0; i <= sysData.sweep.sweepPoints - 1; i++)
						{
							temp = (float) (sysData.trace[tempIndex].value[i]);
							tempData = (char *) (&temp);
							socketData[4 * i] = *(tempData + 3);
							socketData[4 * i + 1] = *(tempData + 2);
							socketData[4 * i + 2] = *(tempData + 1);
							socketData[4 * i + 3] = *(tempData);
						}

						socketData[4 * sysData.sweep.sweepPoints] = 0x0D;
						socketData[4 * sysData.sweep.sweepPoints + 1] = 0x0A;
					} else
					{
						if (!traceDataOrganized[tempIndex])
						{
							for (int i = 0; i <= sysData.sweep.sweepPoints - 1; i++)
							{
								temp = (float) (sysData.trace[tempIndex].value[i]);
								tempData = (char *) (&temp);
								socketData[4 * i] = *(tempData + 3);
								socketData[4 * i + 1] = *(tempData + 2);
								socketData[4 * i + 2] = *(tempData + 1);
								socketData[4 * i + 3] = *(tempData);
							}

							socketData[4 * sysData.sweep.sweepPoints] = 0x0D;
							socketData[4 * sysData.sweep.sweepPoints + 1] = 0x0A;

							traceDataString[tempIndex] = returnString;
							traceDataOrganized[tempIndex] = true;
						}
					}

					switch (sysData.scpiData.interface)
					{
						case sitLan:
							sysData.scpiData.stateT = true;

							//            for (int i = 0; i < MAXTCPCLIENTS; i++)
							//            {
							//          	  if (clientSockets[i].socket != NULL && clientSockets[i].isWriting)
							//          	  {
							//                clientSockets[i].socket->write((char*)socketData, sizeof(socketData));
							//          	  }
							//            }
							send(curSocket, (void *) socketData, sizeof(socketData), 0);

							//            clientSocket->write((const char*)socketData , sizeof(socketData));
							sysData.scpiData.stateT = false;
							break;

						case sitUsb:
							sysData.scpiData.stateT = true;
							write(usbHandle, (char *) socketData, sizeof(socketData));
							sysData.scpiData.stateT = false;
							break;
						case sitCom:
							sysData.scpiData.stateT = true;
							sendSerialData(true, (char *) socketData);
							sysData.scpiData.stateT = false;
							break;
						default:
							break;
					}
					break;

					//detector
				case SCPI_CMD_DETECTOR_SET:
					exeResult = setDetector(result.value.toUpper().trimmed());
					printf("setDetector:%s\n",result.value.toStdString().c_str());
					//reDrawMenuFrame();
					break;
				case SCPI_CMD_DETECTOR_GET:
					exeResult = __SCPI_RETURN;

					switch (sysData.detector.detMode)
					{
						case dmAuto:
							returnString = "AUTO";
							break;
						case dmNormal:
							returnString = "NORMal";
							break;
						case dmPosPeak:
							returnString = "POSitive";
							break;
						case dmNegPeak:
							returnString = "NEGative";
							break;
						case dmSample:
							returnString = "SAMPle";
							break;
						case dmTransient:
							returnString = "TRANsient";
							break;
					}
					break;

					//display
				case SCPI_CMD_DISPLAY_FULLSCREEN_SET:
					exeResult = setDisplayOfFullScreen(result.value.toUpper().trimmed());
					emit refreshWindowState();
					break;
				case SCPI_CMD_DISPLAY_FULLSCREEN_GET:
					exeResult = __SCPI_RETURN;
					returnString = boolToString(sysData.display.isFullScreen);
					break;
				case SCPI_CMD_DISPLAY_FORMAT_ZOOM_SET:
					exeResult = setDisplayOfZoomOn(result.value.toUpper().trimmed());
					break;
				case SCPI_CMD_DISPLAY_FORMAT_ZOOM_GET:
					exeResult = __SCPI_RETURN;
					returnString = boolToString(sysData.display.isZoomOn);
					break;
				case SCPI_CMD_DISPLAY_LINE_Y_SET:
					exeResult = setDisplayOfLineValue(result.value.toUpper().trimmed());
					//reDrawMenuFrame();
					break;
				case SCPI_CMD_DISPLAY_LINE_Y_GET:
					exeResult = __SCPI_RETURN;
					returnString = QString(floatToStringDot2(sysData.display.dispLineValue, tempChar)).trimmed();
					break;
				case SCPI_CMD_DISPLAY_LINE_Y_STATE_SET:
					exeResult = setDisplayOfLine(result.value.toUpper().trimmed());
					break;
				case SCPI_CMD_DISPLAY_LINE_Y_STATE_GET:
					exeResult = __SCPI_RETURN;
					returnString = boolToString(sysData.display.isDispLineOn);
					break;
				case SCPI_CMD_DISPLAY_Y_GAUGE_STATE_SET:
					exeResult = setDisplayOfRefLevelGraticule(result.value.toUpper().trimmed());
					//reDrawMenuFrame();
					break;
				case SCPI_CMD_DISPLAY_Y_GAUGE_STATE_GET:
					exeResult = __SCPI_RETURN;
					returnString = boolToString(sysData.display.isAmptGraticuleOn);
					break;
				case SCPI_CMD_DISPLAY_GRID_STATE_SET:
					exeResult = setDisplayOfGrid(result.value.toUpper().trimmed());
					break;
				case SCPI_CMD_DISPLAY_GRID_STATE_GET:
					exeResult = __SCPI_RETURN;
					returnString = boolToString(sysData.display.isGridOn);
					break;
				case SCPI_CMD_DISPLAY_LABEL_STATE_SET:
					exeResult = setDisplayOfLabel(result.value.toUpper().trimmed());
					//reDrawMenuFrame();
					break;
				case SCPI_CMD_DISPLAY_LABEL_STATE_GET:
					exeResult = __SCPI_RETURN;
					returnString = boolToString(sysData.display.isLabelOn);
					break;
				case SCPI_CMD_DISPLAY_SKIN_SET:
					exeResult = setDisplayOfStyle(result.value.toUpper().trimmed());
					//reDrawMenuFrame();
					break;
				case SCPI_CMD_DISPLAY_SKIN_GET:
					exeResult = __SCPI_RETURN;
					if (sysData.display.winStyle == dsDefault)
						returnString = "DEFault";
					else if (sysData.display.winStyle == dsWinXP)
						returnString = "WINxp";
					break;

					//marker
				case SCPI_CMD_MARKER_ALLOFF:
					exeResult = setMarkerAllOff();
					//reDrawMenuFrame();
					break;
				case SCPI_CMD_MARKER_STATE_SET:
					auxInt = getCharValue(QString(result.auxValue), &auxBool);

					if (auxBool && auxInt >= 1 && auxInt <= MARKERCOUNT)
					{
						if (getBoolValue(result.value, &auxBool) == 0x01)
						{
							if (sysData.marker[auxInt - 1].state == msClose)
							{
								setMarkerNormal(auxInt - 1, 0);
							} else if (sysData.marker[auxInt - 1].state == msNormal)
							{
								setMarkerNormal(auxInt - 1, sysData.marker[auxInt - 1].traceIndex);
							} else if (sysData.marker[auxInt - 1].state == msDelta)
							{
								setMarkerDelta(auxInt - 1, sysData.marker[auxInt - 1].traceIndex);
							}
						} else
						{
							sysData.marker[auxInt - 1].state = msClose;
						}

						exeResult = __SCPI_SUCCESS;
					} else if (!auxBool && auxInt == 0x0)
					{
						auxInt = 0x01;

						if (getBoolValue(result.value, &auxBool) == 0x01)
						{
							if (sysData.marker[auxInt - 1].state == msClose)
							{
								setMarkerNormal(auxInt - 1, 0);
							} else if (sysData.marker[auxInt - 1].state == msNormal)
							{
								setMarkerNormal(auxInt - 1, sysData.marker[auxInt - 1].traceIndex);
							} else if (sysData.marker[auxInt - 1].state == msDelta)
							{
								setMarkerDelta(auxInt - 1, sysData.marker[auxInt - 1].traceIndex);
							}
						} else
						{
							sysData.marker[auxInt - 1].state = msClose;
						}

						exeResult = __SCPI_SUCCESS;
					} else
					{
						exeResult = __SCPI_FAILED;
					}
					//reDrawMenuFrame();
					break;
				case SCPI_CMD_MARKER_STATE_GET:
					exeResult = __SCPI_RETURN;
					auxInt = getCharValue(QString(result.auxValue), &auxBool);

					if (auxBool && auxInt >= 1 && auxInt <= MARKERCOUNT)
					{
						returnString = boolToString(!isTheMarkerOff(auxInt - 1));
					} else if (!auxBool && result.auxValue == 0x0)
					{
						auxInt = 0x01;
						returnString = boolToString(!isTheMarkerOff(auxInt - 1));
					} else
					{
						exeResult = __SCPI_FAILED;
					}
					break;
				case SCPI_CMD_MARKER_TRACE_SET:
					auxInt = getCharValue(QString(result.auxValue), &auxBool);
					traceInt = getCharValue(QString(result.value), &traceBool);

					if (auxBool && traceBool && auxInt >= 1 && auxInt <= MARKERCOUNT && traceInt >= 1 && traceInt <= TRACECOUNT)
					{
						sysData.marker[auxInt - 1].traceIndex = traceInt - 1;
						exeResult = __SCPI_SUCCESS;
					} else
					{
						exeResult = __SCPI_FAILED;
					}
					//reDrawMenuFrame();
					break;
				case SCPI_CMD_MARKER_TRACE_GET:
					auxInt = getCharValue(QString(result.auxValue), &auxBool);

					if (auxBool && auxInt >= 1 && auxInt <= MARKERCOUNT)
					{
						exeResult = __SCPI_RETURN;
						returnString = QString(intToString(sysData.marker[auxInt - 1].traceIndex + 1, tempChar)).trimmed();
					} else
					{
						exeResult = __SCPI_FAILED;
					}
					break;
				case SCPI_CMD_MARKER_MODE_SET:
					auxInt = getCharValue(QString(result.auxValue), &auxBool);

					if (auxBool && auxInt >= 1 && auxInt <= MARKERCOUNT)
					{
						exeResult = setMarkerMode(auxInt - 1, result.value.toUpper().trimmed());
					} else if (!auxBool && result.auxValue == 0x0)
					{
						//exeResult = setMarkerMode(auxInt - 1, result.value.toUpper().trimmed());
						exeResult = setMarkerMode(0, result.value.toUpper().trimmed());
					} else
					{
						exeResult = __SCPI_FAILED;
					}
					//reDrawMenuFrame();
					break;
				case SCPI_CMD_MARKER_MODE_GET:
					exeResult = __SCPI_RETURN;
					auxInt = getCharValue(QString(result.auxValue), &auxBool);

					if (auxBool && auxInt >= 1 && auxInt <= MARKERCOUNT)
					{
						returnString = getMarkerModeName(auxInt - 1);
					} else if (!auxBool && result.auxValue == 0x0)
					{
						returnString = getMarkerModeName(0);
					} else
					{
						exeResult = __SCPI_FAILED;
					}
					break;
				case SCPI_CMD_MARKER_TABLE_STATE_SET:
					exeResult = setMarkerTable(result.value.toUpper().trimmed());
					//reDrawMenuFrame();
					break;
				case SCPI_CMD_MARKER_TABLE_STATE_GET:
					exeResult = __SCPI_RETURN;
					returnString = boolToString(sysData.markerTableOn);
					break;
				case SCPI_CMD_MARKER_X_SET:
					auxInt = getCharValue(QString(result.auxValue), &auxBool);

					if (auxBool && auxInt >= 1 && auxInt <= MARKERCOUNT && !isTheMarkerOff(auxInt - 1))
					{
						exeResult = setMarkerXValue(auxInt - 1, result.value.toUpper().trimmed());
					} else if (!auxBool && result.auxValue == 0x0 && !isTheMarkerOff(0))
					{
						exeResult = setMarkerXValue(0, result.value.toUpper().trimmed());
					} else
					{
						exeResult = __SCPI_FAILED;
					}
					break;
				case SCPI_CMD_MARKER_X_GET:
					exeResult = __SCPI_RETURN;
					auxInt = getCharValue(QString(result.auxValue), &auxBool);

					if (auxBool && auxInt >= 1 && auxInt <= MARKERCOUNT && !isTheMarkerOff(auxInt - 1))
					{
						returnString = QString(floatToString(getMarkerXValue(auxInt - 1), 0, 0, tempChar)).trimmed();
					} else if (!auxBool && result.auxValue == 0x0 && !isTheMarkerOff(0))
					{
						returnString = QString(floatToString(getMarkerXValue(0), 0, 0, tempChar)).trimmed();
					} else
					{
						exeResult = __SCPI_FAILED;
					}
					break;
				case SCPI_CMD_MARKER_Y_GET:
					exeResult = __SCPI_RETURN;
					auxInt = getCharValue(QString(result.auxValue), &auxBool);

					if (auxBool && auxInt >= 1 && auxInt <= MARKERCOUNT && !isTheMarkerOff(auxInt - 1))
					{
						returnString = QString(floatToStringDot2(getMarkerYValue(auxInt - 1), tempChar)).trimmed();
					} else if (!auxBool && result.auxValue == 0x0 && !isTheMarkerOff(0))
					{
						returnString = QString(floatToStringDot2(getMarkerYValue(0), tempChar)).trimmed();
					} else
					{
						exeResult = __SCPI_FAILED;
					}
					break;

					//marker function
				case SCPI_CMD_MARKER_FCTN_ALLOFF:
					exeResult = setMarkerFunctionAllOff();
					break;
				case SCPI_CMD_MARKER_FCTN_FREQCOUNT_STATE_SET:
					auxInt = getCharValue(QString(result.auxValue), &auxBool);

					if (auxBool && auxInt >= 1 && auxInt <= MARKERCOUNT)
					{
						if (isTheMarkerOff(auxInt - 1))
						{
							setMarkerNormal(auxInt - 1, 0);
						}

						exeResult = setMarkerFunctionFreqCountOn(result.value.toUpper().trimmed());
					} else if (!auxBool && result.auxValue == 0x0)
					{
						if (isTheMarkerOff(0))
						{
							setMarkerNormal(0, 0);
						}

						exeResult = setMarkerFunctionFreqCountOn(result.value.toUpper().trimmed());
					} else
					{
						exeResult = __SCPI_FAILED;
					}
					//reDrawMenuFrame();
					break;
				case SCPI_CMD_MARKER_FCTN_FREQCOUNT_STATE_GET:
					exeResult = __SCPI_RETURN;
					returnString = boolToString(sysData.markerFctn.isFreqCountOn);
					break;
				case SCPI_CMD_MARKER_FCTN_FREQCOUNT_X_GET:
					exeResult = __SCPI_RETURN;
					returnString = QString(floatToString(sysData.markerFctn.freqCountValue, 0, 0, tempChar)).trimmed();
					break;
				case SCPI_CMD_MARKER_FCTN_FREQCOUNT_RBW_SET:
					exeResult = setMarkerFunctionFreqCountBw(result.value.toUpper().trimmed());
					//reDrawMenuFrame();
					break;
				case SCPI_CMD_MARKER_FCTN_FREQCOUNT_RBW_GET:
					exeResult = __SCPI_RETURN;
					returnString = QString(floatToString(sysData.markerFctn.freqCountBw, 0, 0, tempChar)).trimmed();
					break;
				case SCPI_CMD_MARKER_FCTN_NOISE_STATE_SET:
					auxInt = getCharValue(QString(result.auxValue), &auxBool);

					if (auxBool && auxInt >= 1 && auxInt <= MARKERCOUNT)
					{
						sysData.markerSelIndex = auxInt - 1;

						if (isTheMarkerOff(auxInt - 1))
						{
							setMarkerNormal(auxInt - 1, 0);
						}

						exeResult = setMarkerFunctionNoiseOn(result.value.toUpper().trimmed());
					} else if (!auxBool && result.auxValue == 0x0)
					{
						sysData.markerSelIndex = 0;

						if (isTheMarkerOff(0))
						{
							setMarkerNormal(0, 0);
						}

						exeResult = setMarkerFunctionNoiseOn(result.value.toUpper().trimmed());
					} else
					{
						exeResult = __SCPI_FAILED;
					}
					break;
				case SCPI_CMD_MARKER_FCTN_NOISE_STATE_GET:
					exeResult = __SCPI_RETURN;
					returnString = boolToString(sysData.markerFctn.isMkrNoiseOn);
					break;
				case SCPI_CMD_MARKER_FCTN_NOISE_Y_GET:
					exeResult = __SCPI_RETURN;
					returnString = QString(floatToStringDot2(sysData.markerFctn.noiseValue, tempChar)).trimmed();
					break;
				case SCPI_CMD_MARKER_FCTN_NOISE_SFREQ_SET:
					exeResult = setMarkerFunctionNoiseStartFreq(result.value.toUpper().trimmed());
					//reDrawMenuFrame();
					break;
				case SCPI_CMD_MARKER_FCTN_NOISE_SFREQ_GET:
					exeResult = __SCPI_RETURN;
					returnString = getMarkerFunctionNoiseStartFreq();
					break;
				case SCPI_CMD_MARKER_FCTN_NOISE_SPAN_SET:
					exeResult = setMarkerFunctionNoiseSpanFreq(result.value.toUpper().trimmed());
					break;
				case SCPI_CMD_MARKER_FCTN_NOISE_SPAN_GET:
					exeResult = __SCPI_RETURN;
					returnString = getMarkerFunctionNoiseSpanFreq();
					break;
				case SCPI_CMD_MARKER_FCTN_NDB_STATE_SET:
					if (sysData.marker[sysData.markerSelIndex].state == msClose)
					{
						setMarkerNormal(sysData.markerSelIndex, sysData.marker[sysData.markerSelIndex].traceIndex);
					}
					exeResult = setMarkerFunctionNdBOn(result.value.toUpper().trimmed());
					//reDrawMenuFrame();
					break;
				case SCPI_CMD_MARKER_FCTN_NDB_STATE_GET:
					exeResult = __SCPI_RETURN;
					returnString = boolToString(sysData.markerFctn.isNdBOn);
					break;
				case SCPI_CMD_MARKER_FCTN_NDB_NDB_SET:
					exeResult = setMarkerFunctionNdBValue(result.value.toUpper().trimmed());
					//reDrawMenuFrame();
					break;
				case SCPI_CMD_MARKER_FCTN_NDB_NDB_GET:
					exeResult = __SCPI_RETURN;
					returnString = QString(floatToStringDot2(sysData.markerFctn.ndBValue, tempChar)).trimmed();
					break;
				case SCPI_CMD_MARKER_FCTN_NDB_RESULT_GET:
					exeResult = __SCPI_RETURN;
					returnString = QString(floatToString(sysData.markerFctn.ndBFreq, 0, 0, tempChar)).trimmed();
					break;

					//markerto
				case SCPI_CMD_MARKER_TO_CENTER:
					auxInt = getCharValue(QString(result.auxValue), &auxBool);

					if (auxBool && auxInt >= 1 && auxInt <= MARKERCOUNT)
					{
						sysData.markerSelIndex = auxInt - 1;
						exeResult = setMarkerToCenterFreq(sysData.markerSelIndex);
					} else if (!auxBool && result.auxValue == 0x0)
					{
						sysData.markerSelIndex = 0;
						exeResult = setMarkerToCenterFreq(sysData.markerSelIndex);
					} else
					{
						exeResult = __SCPI_FAILED;
					}
					break;
				case SCPI_CMD_MARKER_TO_STEP:
					auxInt = getCharValue(QString(result.auxValue), &auxBool);

					if (auxBool && auxInt >= 1 && auxInt <= MARKERCOUNT)
					{
						sysData.markerSelIndex = auxInt - 1;
						exeResult = setMarkerToCenterFreqStep(sysData.markerSelIndex);
					} else if (!auxBool && result.auxValue == 0x0)
					{
						sysData.markerSelIndex = 0;
						exeResult = setMarkerToCenterFreqStep(sysData.markerSelIndex);
					} else
					{
						exeResult = __SCPI_FAILED;
					}
					break;
				case SCPI_CMD_MARKER_TO_START:
					auxInt = getCharValue(QString(result.auxValue), &auxBool);

					if (auxBool && auxInt >= 1 && auxInt <= MARKERCOUNT)
					{
						sysData.markerSelIndex = auxInt - 1;
						exeResult = setMarkerToStartFreq(sysData.markerSelIndex);
					} else if (!auxBool && result.auxValue == 0x0)
					{
						sysData.markerSelIndex = 0;
						exeResult = setMarkerToStartFreq(sysData.markerSelIndex);
					} else
					{
						exeResult = __SCPI_FAILED;
					}
					break;
				case SCPI_CMD_MARKER_TO_STOP:
					auxInt = getCharValue(QString(result.auxValue), &auxBool);

					if (auxBool && auxInt >= 1 && auxInt <= MARKERCOUNT)
					{
						sysData.markerSelIndex = auxInt - 1;
						exeResult = setMarkerToStopFreq(sysData.markerSelIndex);
					} else if (!auxBool && result.auxValue == 0x0)
					{
						sysData.markerSelIndex = 0;
						exeResult = setMarkerToStopFreq(sysData.markerSelIndex);
					} else
					{
						exeResult = __SCPI_FAILED;
					}
					break;
				case SCPI_CMD_MARKER_TO_REFLEVEL:
					auxInt = getCharValue(QString(result.auxValue), &auxBool);

					if (auxBool && auxInt >= 1 && auxInt <= MARKERCOUNT)
					{
						sysData.markerSelIndex = auxInt - 1;
						exeResult = setMarkerToRefLevel(sysData.markerSelIndex);
					} else if (!auxBool && result.auxValue == 0x0)
					{
						sysData.markerSelIndex = 0;
						exeResult = setMarkerToRefLevel(sysData.markerSelIndex);
					} else
					{
						exeResult = __SCPI_FAILED;
					}
					break;
				case SCPI_CMD_MARKER_DELTA_TO_SPAN:
					auxInt = getCharValue(QString(result.auxValue), &auxBool);

					if (auxBool && auxInt >= 1 && auxInt <= MARKERCOUNT)
					{
						sysData.markerSelIndex = auxInt - 1;
						exeResult = setMarkerDeltaToSpan(sysData.markerSelIndex);
					} else if (!auxBool && result.auxValue == 0x0)
					{
						sysData.markerSelIndex = 0;
						exeResult = setMarkerDeltaToSpan(sysData.markerSelIndex);
					} else
					{
						exeResult = __SCPI_FAILED;
					}
					break;
				case SCPI_CMD_MARKER_DELTA_TO_CENTER:
					auxInt = getCharValue(QString(result.auxValue), &auxBool);

					if (auxBool && auxInt >= 1 && auxInt <= MARKERCOUNT)
					{
						sysData.markerSelIndex = auxInt - 1;
						exeResult = setMarkerDeltaToCenterFreq(sysData.markerSelIndex);
					} else if (!auxBool && result.auxValue == 0x0)
					{
						sysData.markerSelIndex = 0;
						exeResult = setMarkerDeltaToCenterFreq(sysData.markerSelIndex);
					} else
					{
						exeResult = __SCPI_FAILED;
					}
					break;

					//peak
				case SCPI_CMD_MARKER_PEAK_MAXIMUM:
					auxInt = getCharValue(QString(result.auxValue), &auxBool);

					if (auxBool && auxInt >= 1 && auxInt <= MARKERCOUNT)
					{
						sysData.markerSelIndex = auxInt - 1;

						if (sysData.marker[sysData.markerSelIndex].state == msClose)
						{
							setMarkerNormal(sysData.markerSelIndex, sysData.marker[sysData.markerSelIndex].traceIndex);
						}

						exeResult = setMarkerPeakMax(sysData.markerSelIndex, sysData.marker[sysData.markerSelIndex].traceIndex);
					} else if (!auxBool && result.auxValue == 0x0)
					{
						sysData.markerSelIndex = 0;

						if (sysData.marker[sysData.markerSelIndex].state == msClose)
						{
							setMarkerNormal(sysData.markerSelIndex, sysData.marker[sysData.markerSelIndex].traceIndex);
						}

						exeResult = setMarkerPeakMax(sysData.markerSelIndex, sysData.marker[sysData.markerSelIndex].traceIndex);
					} else
					{
						exeResult = __SCPI_FAILED;
					}
					break;
				case SCPI_CMD_MARKER_PEAK_MAXIMUM_LEFT:
					auxInt = getCharValue(QString(result.auxValue), &auxBool);

					if (auxBool && auxInt >= 1 && auxInt <= MARKERCOUNT)
					{
						sysData.markerSelIndex = auxInt - 1;

						if (sysData.marker[sysData.markerSelIndex].state == msClose)
						{
							setMarkerNormal(sysData.markerSelIndex, sysData.marker[sysData.markerSelIndex].traceIndex);
						}

						exeResult = setMarkerPeakLeft(sysData.markerSelIndex, sysData.marker[sysData.markerSelIndex].traceIndex);
					} else if (!auxBool && result.auxValue == 0x0)
					{
						sysData.markerSelIndex = 0;

						if (sysData.marker[sysData.markerSelIndex].state == msClose)
						{
							setMarkerNormal(sysData.markerSelIndex, sysData.marker[sysData.markerSelIndex].traceIndex);
						}

						exeResult = setMarkerPeakLeft(sysData.markerSelIndex, sysData.marker[sysData.markerSelIndex].traceIndex);
					} else
					{
						exeResult = __SCPI_FAILED;
					}
					break;
				case SCPI_CMD_MARKER_PEAK_MAXIMUM_RIGHT:
					auxInt = getCharValue(QString(result.auxValue), &auxBool);

					if (auxBool && auxInt >= 1 && auxInt <= MARKERCOUNT)
					{
						sysData.markerSelIndex = auxInt - 1;

						if (sysData.marker[sysData.markerSelIndex].state == msClose)
						{
							setMarkerNormal(sysData.markerSelIndex, sysData.marker[sysData.markerSelIndex].traceIndex);
						}

						exeResult = setMarkerPeakRight(sysData.markerSelIndex, sysData.marker[sysData.markerSelIndex].traceIndex);
					} else if (!auxBool && result.auxValue == 0x0)
					{
						sysData.markerSelIndex = 0;

						if (sysData.marker[sysData.markerSelIndex].state == msClose)
						{
							setMarkerNormal(sysData.markerSelIndex, sysData.marker[sysData.markerSelIndex].traceIndex);
						}

						exeResult = setMarkerPeakRight(sysData.markerSelIndex, sysData.marker[sysData.markerSelIndex].traceIndex);
					} else
					{
						exeResult = __SCPI_FAILED;
					}
					break;
				case SCPI_CMD_MARKER_PEAK_MAXIMUM_NEXT:
					auxInt = getCharValue(QString(result.auxValue), &auxBool);

					if (auxBool && auxInt >= 1 && auxInt <= MARKERCOUNT)
					{
						sysData.markerSelIndex = auxInt - 1;

						if (sysData.marker[sysData.markerSelIndex].state == msClose)
						{
							setMarkerNormal(sysData.markerSelIndex, sysData.marker[sysData.markerSelIndex].traceIndex);
						}

						exeResult = setMarkerPeakNext(sysData.markerSelIndex, sysData.marker[sysData.markerSelIndex].traceIndex);
					} else if (!auxBool && result.auxValue == 0x0)
					{
						sysData.markerSelIndex = 0;

						if (sysData.marker[sysData.markerSelIndex].state == msClose)
						{
							setMarkerNormal(sysData.markerSelIndex, sysData.marker[sysData.markerSelIndex].traceIndex);
						}

						exeResult = setMarkerPeakNext(sysData.markerSelIndex, sysData.marker[sysData.markerSelIndex].traceIndex);
					} else
					{
						exeResult = __SCPI_FAILED;
					}
					break;
				case SCPI_CMD_MARKER_PEAK_MINIMUM:
					auxInt = getCharValue(QString(result.auxValue), &auxBool);

					if (auxBool && auxInt >= 1 && auxInt <= MARKERCOUNT)
					{
						sysData.markerSelIndex = auxInt - 1;

						if (sysData.marker[sysData.markerSelIndex].state == msClose)
						{
							setMarkerNormal(sysData.markerSelIndex, sysData.marker[sysData.markerSelIndex].traceIndex);
						}

						exeResult = setMarkerPeakMin(sysData.markerSelIndex, sysData.marker[sysData.markerSelIndex].traceIndex);
					} else if (!auxBool && result.auxValue == 0x0)
					{
						sysData.markerSelIndex = 0;

						if (sysData.marker[sysData.markerSelIndex].state == msClose)
						{
							setMarkerNormal(sysData.markerSelIndex, sysData.marker[sysData.markerSelIndex].traceIndex);
						}

						exeResult = setMarkerPeakMin(sysData.markerSelIndex, sysData.marker[sysData.markerSelIndex].traceIndex);
					} else
					{
						exeResult = __SCPI_FAILED;
					}
					break;
				case SCPI_CMD_MARKER_PEAK_CONTINUOUS_SET:
					auxInt = getCharValue(QString(result.auxValue), &auxBool);

					if (auxBool && auxInt >= 1 && auxInt <= MARKERCOUNT)
					{
						sysData.markerSelIndex = auxInt - 1;

						if (sysData.marker[sysData.markerSelIndex].state == msClose)
						{
							setMarkerNormal(sysData.markerSelIndex, sysData.marker[sysData.markerSelIndex].traceIndex);
						}

						exeResult = setMarkerPeakMaxAuto(result.value.toUpper().trimmed());
					} else if (!auxBool && result.auxValue == 0x0)
					{
						sysData.markerSelIndex = 0;

						if (sysData.marker[sysData.markerSelIndex].state == msClose)
						{
							setMarkerNormal(sysData.markerSelIndex, sysData.marker[sysData.markerSelIndex].traceIndex);
						}

						exeResult = setMarkerPeakMaxAuto(result.value.toUpper().trimmed());
					} else
					{
						exeResult = __SCPI_FAILED;
					}
					break;
				case SCPI_CMD_MARKER_PEAK_CONTINUOUS_GET:
					exeResult = __SCPI_RETURN;
					returnString = boolToString(sysData.isAutoPeakMaxing);
					break;

					//system
				case SCPI_CMD_SYSTEM_LANGUAGE_SET:
					exeResult = setSystemLanguage(result.value.toUpper().trimmed());
					emit refreshWindowState();
					break;
				case SCPI_CMD_SYSTEM_LANGUAGE_GET:
					exeResult = __SCPI_RETURN;

					if (sysData.system.isLanguageEn)
					{
						returnString = "ENGLish";
					} else
					{
						returnString = "CHINese";
					}
					break;
				case SCPI_CMD_SYSTEM_BRIGHTNESS_SET:
					exeResult = setSystemLcdBacklight(result.value.toUpper().trimmed());
					break;
				case SCPI_CMD_SYSTEM_BRIGHTNESS_GET:
					exeResult = __SCPI_RETURN;
					returnString = QString(intToString(sysData.system.backLight, tempChar)).trimmed();
					break;
				case SCPI_CMD_SYSTEM_DISPLAY_ENABLE_SET:
					exeResult = setSystemScpiScreenRefresh(result.value.toUpper().trimmed());
					break;
				case SCPI_CMD_SYSTEM_DISPLAY_ENABLE_GET:
					exeResult = __SCPI_RETURN;
					returnString = boolToString(sysData.system.isScpiScreenRefresh);
					break;
				case SCPI_CMD_SYSTEM_CAL_CHANNEL_MODE_SET:
					exeResult = setSystemChannelSelect(result.value.toUpper().trimmed());
					break;
				case SCPI_CMD_SYSTEM_CAL_CHANNEL_MODE_GET:
					exeResult = __SCPI_RETURN;

					if (sysData.system.isInternalSignalOn)
					{
						returnString = "INTernal";
					} else
					{
						returnString = "EXTernal";
					}
					break;
				case SCPI_CMD_SYSTEM_CLOCK_STATE_SET:
					exeResult = setSystemDateTimeShow(result.value.toUpper().trimmed());
					//reDrawMenuFrame();
					break;
				case SCPI_CMD_SYSTEM_CLOCK_STATE_GET:
					exeResult = __SCPI_RETURN;
					returnString = boolToString(sysData.system.datetime.isVisible);
					break;
				case SCPI_CMD_SYSTEM_CLOCK_DATE_SET:
					if (setSystemDate(result.value.toUpper().trimmed()) == _RESULT_SUCCESS)
					{
						exeResult = __SCPI_SUCCESS;
					}
					break;
				case SCPI_CMD_SYSTEM_CLOCK_DATE_GET:
					exeResult = __SCPI_RETURN;
					sprintf(tempChar, "%04d-%02d-%02d", QDate::currentDate().year(), QDate::currentDate().month(), QDate::currentDate().day());
					returnString = QString(tempChar).trimmed();
					break;
				case SCPI_CMD_SYSTEM_CLOCK_TIME_SET:
					if (setSystemTime(result.value.toUpper().trimmed()) == _RESULT_SUCCESS)
					{
						exeResult = __SCPI_SUCCESS;
					}
					break;
				case SCPI_CMD_SYSTEM_CLOCK_TIME_GET:
					exeResult = __SCPI_RETURN;
					sprintf(tempChar, "%02d:%02d:%02d", QTime::currentTime().hour(), QTime::currentTime().minute(), QTime::currentTime().second());
					returnString = QString(tempChar).trimmed();
					break;
				case SCPI_CMD_SYSTEM_CLOCK_DATE_FORMAT_SET:
					exeResult = setSystemDateFormat(result.value.toUpper().trimmed());
					break;
				case SCPI_CMD_SYSTEM_CLOCK_DATE_FORMAT_GET:
					exeResult = __SCPI_RETURN;

					if (sysData.system.datetime.style == dtsYMDHMS)
					{
						returnString = "YMDhms";
					} else
					{
						returnString = "HMSymd";
					}
					break;
				case SCPI_CMD_SYSTEM_CONFIGURE_INFORMATION_GET:
					exeResult = __SCPI_RETURN;
					returnString = getSystemInfoString();
					break;
				case SCPI_CMD_SYSTEM_CONFIGURE_MESSAGE_GET:
					exeResult = __SCPI_RETURN;
					returnString = getSystemLogString();
					break;
				case SCPI_CMD_SYSTEM_POWERON_TYPE_SET:
					exeResult = setSystemPoweronParam(result.value.toUpper().trimmed());
					//reDrawMenuFrame();
					break;
				case SCPI_CMD_SYSTEM_POWERON_TYPE_GET:
					exeResult = __SCPI_RETURN;

					switch (sysData.system.powerOnPreset.powerOnParam)
					{
						//      	  case spFactory: returnString = "FACTory"; break;  //新框架
						case spFactory:
							returnString = "FACT";
							break; //老框架
						case spUser:
							returnString = "USER";
							break;
						case spPowerOff:
							returnString = "LAST";
							break;
					}
					break;
				case SCPI_CMD_SYSTEM_PRESET_TYPE_SET:
					exeResult = setSystemPresetParam(result.value.toUpper().trimmed());
					//reDrawMenuFrame();
					break;
				case SCPI_CMD_SYSTEM_PRESET_TYPE_GET:
					exeResult = __SCPI_RETURN;

					switch (sysData.system.powerOnPreset.presetParam)
					{
						//      	  case spFactory: returnString = "FACTory"; break;  //新框架
						case spFactory:
							returnString = "FACT";
							break; //老框架
						case spUser:
							returnString = "USER";
							break;
						case spPowerOff:
							returnString = "LAST";
							break;
					}
					break;
				case SCPI_CMD_SYSTEM_PRESET_USER_SAVE:
					if (saveUserState() == _RESULT_SUCCESS)
					{
						exeResult = __SCPI_SUCCESS;
					}
					break;
				case SCPI_CMD_USER_CALIBRATION_SET:
					exeResult = userCalibrate();
					presetSystem();
					break;
				case SCPI_CMD_USER_CALIBRATION_GET:
					exeResult = __SCPI_RETURN;
					returnString = boolToString(sysData.isUserCalibrating);
					break;
				case SCPI_CMD_USER_CALIBRATION_RESTORE:
					clearUserCalibrateData();
					presetSystemData();
					break;

					//设置IP
				case SCPI_CMD_SYSTEM_COMMUNICATE_LAN_IP_SET:
					if (!QHostAddress(result.value.toUpper().trimmed()).isNull())
					{
						exeResult = setLanIP(result.value.toUpper().trimmed());
					} else
					{
						outputMessage(getTheTranslatorString("address") + " " + result.value + " " + getTheTranslatorString("is invalid"));
					}
					//reDrawMenuFrame();
					break;
				case SCPI_CMD_SYSTEM_COMMUNICATE_LAN_IP_GET:
					exeResult = __SCPI_RETURN;
					returnString = QString(sysData.system.io.lan.ip).trimmed();
					break;
				case SCPI_CMD_SYSTEM_COMMUNICATE_LAN_MASK_SET:
					if (!QHostAddress(result.value.toUpper().trimmed()).isNull())
					{
						exeResult = setLanMask(result.value.toUpper().trimmed());
					} else
					{
						outputMessage(getTheTranslatorString("address") + " " + result.value + " " + getTheTranslatorString("is invalid"));
					}
					//reDrawMenuFrame();
					break;
				case SCPI_CMD_SYSTEM_COMMUNICATE_LAN_MASK_GET:
					exeResult = __SCPI_RETURN;
					returnString = QString(sysData.system.io.lan.mask).trimmed();
					break;
				case SCPI_CMD_SYSTEM_COMMUNICATE_LAN_GATE_SET:
					if (!QHostAddress(result.value.toUpper().trimmed()).isNull())
					{
						exeResult = setLanGate(result.value.toUpper().trimmed());
					} else
					{
						outputMessage(getTheTranslatorString("address") + " " + result.value + " " + getTheTranslatorString("is invalid"));
					}
					//reDrawMenuFrame();
					break;
				case SCPI_CMD_SYSTEM_COMMUNICATE_LAN_GATE_GET:
					exeResult = __SCPI_RETURN;
					returnString = QString(sysData.system.io.lan.gate).trimmed();
					break;

					//file & save
				case SCPI_CMD_MEMORY_CATALOG:
					exeResult = __SCPI_RETURN;
					returnString = getSystemFileListString();
					break;
				case SCPI_CMD_MEMORY_DISK_INFORMATION:
					exeResult = __SCPI_RETURN;
					returnString = getSystemDiskInformation();
					break;
				case SCPI_CMD_MEMORY_STORE_STATE:
					if (saveUserState() == _RESULT_SUCCESS)
					{
						exeResult = __SCPI_SUCCESS;
					}
					break;
				case SCPI_CMD_MEMORY_STORE_TRACE:
					if (saveTraceDataToLocal(result.value.trimmed()) == _RESULT_SUCCESS)
					{
						exeResult = __SCPI_SUCCESS;
					}
					break;
				case SCPI_CMD_MEMORY_STORE_SCREEN:
					if (saveScreenPixmapToLocal(result.value.trimmed()) == _RESULT_SUCCESS)
					{
						exeResult = __SCPI_SUCCESS;
					}
					break;
				case SCPI_CMD_MEMORY_LOAD_STATE:
					exeResult = loadUserState();
					break;
				case SCPI_CMD_MEMORY_LOAD_TRACE:
					exeResult = loadTheMemoryTrace(result.value.trimmed());
					break;
				case SCPI_CMD_MEMORY_LOAD_SCREEN:
					exeResult = loadTheMemoryScreen(result.value.trimmed());
					break;
				case SCPI_CMD_MEMORY_DELETE_TRACE:
					exeResult = deleteTheMemoryTrace(result.value.trimmed());
					getSystemFileList();
					break;
				case SCPI_CMD_MEMORY_DELETE_SCREEN:
					exeResult = deleteTheMemoryScreen(result.value.trimmed());
					getSystemFileList();
					break;
				case SCPI_CMD_MEMORY_DELETE_TRACE_ALL:
					exeResult = deleteAllMemoryTrace();
					getSystemFileList();
					break;
				case SCPI_CMD_MEMORY_DELETE_SCREEN_ALL:
					exeResult = deleteAllMemoryScreen();
					getSystemFileList();
					break;
				case SCPI_CMD_MEMORY_DELETE_ALL:
					exeResult = deleteAllMemory();
					getSystemFileList();
					break;
				case SCPI_CMD_MEMORY_COPY_ALL:
					exeResult = __SCPI_UNSUPPORT;
					//exeResult = copyAllMemory();
					break;
				case SCPI_CMD_MEMORY_COPY_TRACE_ALL:
					exeResult = __SCPI_UNSUPPORT;
					//exeResult = copyAllMemoryTrace();
					break;
				case SCPI_CMD_MEMORY_COPY_TRACE:
					exeResult = copyTheMemoryTrace(result.value.trimmed());
					break;
				case SCPI_CMD_MEMORY_COPY_SCREEN_ALL:
					exeResult = __SCPI_UNSUPPORT;
					//exeResult = copyAllMemoryScreen();
					break;
				case SCPI_CMD_MEMORY_COPY_SCREEN:
					exeResult = copyTheMemoryScreen(result.value.trimmed());
					break;

					//source
				case SCPI_CMD_SOURCE_TRACKGEN_STATE_SET:
					exeResult = setSourceTrackGenState(result.value.toUpper().trimmed());
					//reDrawMenuFrame();
					break;
				case SCPI_CMD_SOURCE_TRACKGEN_STATE_GET:
					exeResult = __SCPI_RETURN;
					returnString = boolToString(sysData.source.isTrackGenOn);
					break;
				case SCPI_CMD_SOURCE_TRACKGEN_POWER_SET:
					exeResult = setSourceTrackGenOutputPower(result.value.toUpper().trimmed());
					//reDrawMenuFrame();
					break;
				case SCPI_CMD_SOURCE_TRACKGEN_POWER_GET:
					exeResult = __SCPI_RETURN;
					returnString = QString(floatToStringDot2(sysData.source.trackGenOutputPower, tempChar)).trimmed();
					break;
				case SCPI_CMD_SOURCE_TRACKGEN_NETMEAS_STATE_SET:
					exeResult = setSourceTrackGenNetworkState(result.value.toUpper().trimmed());
					break;
				case SCPI_CMD_SOURCE_TRACKGEN_NETMEAS_STATE_GET:
					exeResult = __SCPI_RETURN;
					returnString = boolToString(sysData.source.isTrackGenNetworkOn);
					break;
				case SCPI_CMD_SOURCE_TRACKGEN_NETMEAS_NORMALIZE:
					exeResult = setSourceTrackGenNetworkNormalize();
					break;
				case SCPI_CMD_SOURCE_TRACKGEN_NETMEAS_RLEVEL_SET:
					exeResult = setSourceTrackGenNetworkReflevel(result.value.toUpper().trimmed());
					break;
				case SCPI_CMD_SOURCE_TRACKGEN_NETMEAS_RLEVEL_GET:
					exeResult = __SCPI_RETURN;
					returnString = QString(floatToStringDot2(sysData.source.trackGenNetworkRefLevel, tempChar)).trimmed();
					break;
				case SCPI_CMD_SOURCE_TRACKGEN_NETMEAS_POWER_SET:
					exeResult = setSourceTrackGenOutputPower(result.value.toUpper().trimmed());
					break;
				case SCPI_CMD_SOURCE_TRACKGEN_NETMEAS_POWER_GET:
					exeResult = __SCPI_RETURN;
					returnString = QString(floatToStringDot2(sysData.source.trackGenOutputPower, tempChar)).trimmed();
					break;
				case SCPI_CMD_SOURCE_SOURCEGEN_STATE_SET:
					exeResult = setSourceSignalGenState(result.value.toUpper().trimmed(), false);
					break;
				case SCPI_CMD_SOURCE_SOURCEGEN_STATE_GET:
					exeResult = __SCPI_RETURN;
					returnString = boolToString(sysData.source.isSignalGenOn);
					break;
				case SCPI_CMD_SOURCE_SOURCEGEN_FREQUENCY_SET:
					exeResult = setSourceSignalGenOutputFreq(result.value.toUpper().trimmed());
					break;
				case SCPI_CMD_SOURCE_SOURCEGEN_FREQUENCY_GET:
					exeResult = __SCPI_RETURN;
					returnString = QString(floatToString(sysData.source.signalGenOutputFreq, 0, 0, tempChar)).trimmed();
					break;
				case SCPI_CMD_SOURCE_SOURCEGEN_POWER_SET:
					exeResult = setSourceSignalGenOutputPower(result.value.toUpper().trimmed());
					break;
				case SCPI_CMD_SOURCE_SOURCEGEN_POWER_GET:
					exeResult = __SCPI_RETURN;
					returnString = QString(floatToStringDot2(sysData.source.signalGenOutputPower, tempChar)).trimmed();
					break;

					//print
				case SCPI_CMD_PRINT_IMAGE_COLOR_SET:
					exeResult = setPrintOfColor(result.value.toUpper().trimmed());
					break;
				case SCPI_CMD_PRINT_IMAGE_COLOR_GET:
					exeResult = __SCPI_RETURN;
					returnString = boolToString(sysData.print.isColorPrinter);
					break;
				case SCPI_CMD_PRINT_IMAGE_SCREEN:
					if (execPrintScreen() == _RESULT_SUCCESS)
					{
						exeResult = __SCPI_SUCCESS;
					} else
					{
						exeResult = __SCPI_FAILED;
					}
					break;
				case SCPI_CMD_PRINT_IMAGE_TRACE:
					if (execPrintTrace() == _RESULT_SUCCESS)
					{
						exeResult = __SCPI_SUCCESS;
					} else
					{
						exeResult = __SCPI_FAILED;
					}
					break;
				case SCPI_CMD_PRINT_IMAGE_ORIENTATION_SET:
					exeResult = setPrintOfOrientation(result.value.toUpper().trimmed());
					break;
				case SCPI_CMD_PRINT_IMAGE_ORIENTATION_GET:
					exeResult = __SCPI_RETURN;

					if (sysData.print.isOrientationHor)
					{
						returnString = "LANDscape";
					} else
					{
						returnString = "PORTrait";
					}
					break;
				case SCPI_CMD_PRINT_IMAGE_SIZE_SET:
					exeResult = setPrintOfPaperSize(result.value.toUpper().trimmed());
					break;
				case SCPI_CMD_PRINT_IMAGE_SIZE_GET:
					exeResult = __SCPI_RETURN;

					switch (sysData.print.paper)
					{
						case ptA4:
							returnString = "A4";
							break;
						case ptA3:
							returnString = "A3";
							break;
						case ptB5:
							returnString = "B5";
							break;
						case ptC5:
							returnString = "C5";
							break;
						case ptLetter:
							returnString = "LETTer";
							break;
					}

					break;
				case SCPI_CMD_PRINT_IMAGE_NUMS_SET:
					exeResult = setPrintOfNums(result.value.toUpper().trimmed());
					break;
				case SCPI_CMD_PRINT_IMAGE_NUMS_GET:
					exeResult = __SCPI_RETURN;
					returnString = QString(intToString(sysData.print.nums, tempChar)).trimmed();
					break;

					//demod
				case SCPI_CMD_DEMOD_STATE_SET:
					exeResult = setDemodOfSoundState(result.value.toUpper().trimmed());
					break;
				case SCPI_CMD_DEMOD_STATE_GET:
					exeResult = __SCPI_RETURN;
					returnString = boolToString(sysData.demod.demodOn);
					break;
				case SCPI_CMD_DEMOD_SOUND_SET:
					exeResult = setDemodOfSound(result.value.toUpper().trimmed());
					break;
				case SCPI_CMD_DEMOD_SOUND_GET:
					exeResult = __SCPI_RETURN;
					returnString = intToString(sysData.demod.sound, tempChar);
					break;
				case SCPI_CMD_DEMOD_MODE_SET:
					exeResult = setDemodOfMode(result.value.toUpper().trimmed());
					break;
				case SCPI_CMD_DEMOD_MODE_GET:
					exeResult = __SCPI_RETURN;

					switch (sysData.demod.mode)
					{
						case dmFMW:
							returnString = "FMW";
							break;
						case dmFM:
							returnString = "FM";
							break;
						case dmAM:
							returnString = "AM";
							break;
						case dmUSB:
							returnString = "USB";
							break;
						case dmLSB:
							returnString = "LSB";
							break;
					}
					break;
				case SCPI_CMD_DEMOD_FREQ_SET:
					tempDemodFreqValue = result.value.split(",");
					tempDemodFreq = tempDemodFreqValue.at(0).toUpper().trimmed();

					if (tempDemodFreq == "RADIO1")
					{
						auxInt = 0;
					} else if (tempDemodFreq == "RADIO2")
					{
						auxInt = 1;
					} else if (tempDemodFreq == "RADIO3")
					{
						auxInt = 2;
					} else if (tempDemodFreq == "RADIO4")
					{
						auxInt = 3;
					} else if (tempDemodFreq == "RADIO5")
					{
						auxInt = 4;
					} else if (tempDemodFreq == "RADIO6")
					{
						auxInt = 5;
					} else
					{
						exeResult = __SCPI_FAILED;
					}

					exeResult = setDemodOfTheFrequency(auxInt, tempDemodFreqValue.at(1).toUpper().trimmed());

					//reDrawMenuFrame();
					break;
				case SCPI_CMD_DEMOD_FREQ_GET:
					exeResult = __SCPI_RETURN;
					tempDemodFreq = result.value.toUpper().trimmed();

					if (tempDemodFreq == "RADIO1")
					{
						returnString = QString(floatToString(sysData.demod.radioFreqSet[0], 0, 0, tempChar)).trimmed();
					} else if (tempDemodFreq == "RADIO2")
					{
						returnString = QString(floatToString(sysData.demod.radioFreqSet[1], 0, 0, tempChar)).trimmed();
					} else if (tempDemodFreq == "RADIO3")
					{
						returnString = QString(floatToString(sysData.demod.radioFreqSet[2], 0, 0, tempChar)).trimmed();
					} else if (tempDemodFreq == "RADIO4")
					{
						returnString = QString(floatToString(sysData.demod.radioFreqSet[3], 0, 0, tempChar)).trimmed();
					} else if (tempDemodFreq == "RADIO5")
					{
						returnString = QString(floatToString(sysData.demod.radioFreqSet[4], 0, 0, tempChar)).trimmed();
					} else if (tempDemodFreq == "RADIO6")
					{
						returnString = QString(floatToString(sysData.demod.radioFreqSet[5], 0, 0, tempChar)).trimmed();
					} else
					{
						exeResult = __SCPI_FAILED;
					}
					break;

					//device
				case SCPI_CMD_DEVICE_SERVICE_SET:
					exeResult = __SCPI_RETURN;
					returnString = "NO";

					if (result.value.toUpper().trimmed() == getSystemServiceOfMD5(sysData.system.serialNo, true).toUpper().trimmed())
					{
						sysData.system.isServiceUnlock = true;
						returnString = "YES";
					} else
					{
						sysData.system.isServiceUnlock = false;
					}
					break;
				case SCPI_CMD_DEVICE_LOGO_SET_BEGIN:
					if (QFile("/home/sky/logo.png.bak").exists())
					{
						system("rm /home/sky/logo.png.bak");
					}
					isRcvingLogoFile = true;
					exeResult = __SCPI_RETURN;
					returnString = "YES";
					break;
				case SCPI_CMD_DEVICE_LOGO_SET_END:
					isRcvingLogoFile = false;
					exeResult = __SCPI_RETURN;
					returnString = "YES";
					if (QFile("/home/sky/logo.png.bak").exists())
					{
						if (QFile("/home/sky/logo.png").exists())
						{
							system("rm /home/sky/logo.png");
						}

						if (system("mv /home/sky/logo.png.bak /home/sky/logo.png") == 0)
						{
							logo.load("/home/sky/logo.png");
						}
					}
					break;
				case SCPI_CMD_DEVICE_LOGO_SET:
					break;
				case SCPI_CMD_DEVICE_LOGO_GET:
					break;
				case SCPI_CMD_DEVICE_SERIALNO_SET:
					exeResult = __SCPI_RETURN;
					returnString = "NO";

					if (setSystemServiceOfSerialNo(result.value.trimmed()) == __SCPI_SUCCESS)
					{
						returnString = "YES";
					}
					break;
				case SCPI_CMD_DEVICE_SERIALNO_GET:
					exeResult = __SCPI_RETURN;
					returnString = "Y" + QString(sysData.system.serialNo).trimmed();
					break;
				case SCPI_CMD_DEVICE_MAC_SET:
					exeResult = __SCPI_RETURN;
					returnString = "NO";

					if (setSystemServiceOfLanMacAddress(result.value.trimmed()) == __SCPI_SUCCESS)
					{
						returnString = "YES";
					}
					break;
				case SCPI_CMD_DEVICE_MAC_GET:
					exeResult = __SCPI_RETURN;
					returnString = "Y";

					for (int i = 0; i < sizeof sysData.system.lanMacAddress / sizeof sysData.system.lanMacAddress[0]; i++)
					{
						returnString += QString(intToHex(sysData.system.lanMacAddress[i], tempChar)).trimmed();
					}
					break;
				case SCPI_CMD_DEVICE_TYPE_SET:
					devType = result.value.trimmed();
					saveDeviceType(devType);
					exeResult = __SCPI_RETURN;
					returnString = "YES";
					break;
				case SCPI_CMD_DEVICE_TYPE_GET:
					exeResult = __SCPI_RETURN;
					returnString = "Y" + devType;
					break;
				case SCPI_CMD_DEVICE_STATE_GET:
					exeResult = __SCPI_RETURN;
					returnString = "Y";

					if (eva.isEvaluate)
					{
						returnString += "Y";
					} else
					{
						returnString += "N";
					}
					break;
				case SCPI_CMD_DEVICE_STATE_EVA_SET:
					setSystemServiceOfEvalute(true);
					exeResult = __SCPI_RETURN;
					returnString = "YES";
					break;
				case SCPI_CMD_DEVICE_STATE_FULL_SET:
					setSystemServiceOfEvalute(false);
					exeResult = __SCPI_RETURN;
					returnString = "YES";
					break;

					//fft command
				case SCPI_CMD_OPTIONS_FFT_STATE_SET:
					exeResult = setSystemServiceOptionFFT10Hz(result.value.trimmed());
					break;
				case SCPI_CMD_OPTIONS_FFT_STATE_GET:
					exeResult = __SCPI_RETURN;
					returnString = boolToString(sysData.options.isFft10HzOn);
					break;
				case SCPI_CMD_OPTIONS_FFT_IDATA_GET:
					break;
				case SCPI_CMD_OPTIONS_FFT_QDATA_GET:
					break;

				case SCPI_CMD_OPTIONS_FFT_IQTESTDATA_GET:
					exeResult = __SCPI_RETURN;
					returnString = "";

					mutexFFT.lock();

					for (int i = 0; i < sizeof(sysData.fft.IQTestData) / sizeof(sysData.fft.IQTestData[0]); i++)
					{
						sprintf(tempChar, "%ld", sysData.fft.IQTestData[i]);
						returnString += tempChar;
						returnString += ",";
					}
					mutexFFT.unlock();
					break;

				case SCPI_CMD_OPTIONS_FFT_IQDATA_GET:
					exeResult = __SCPI_RETURN;
					returnString = "";

					mutexFFT.lock();
					read(fftHandle, sysData.fft.IQData, sizeof(sysData.fft.IQData));

					for (int i = 0; i < sizeof(sysData.fft.IQData) / sizeof(sysData.fft.IQData[0]); i++)
					{
						//    	  sprintf(tempChar, "%08x", sysData.fft.IQData[i]);
						sprintf(tempChar, "%.02f", sysData.fft.IQData[i] * 1.0);
						returnString += tempChar;
						returnString += ",";
					}

					//       	sprintf(tempChar, "%08x", sysData.fft.IQData[4095]);
					//       	sprintf(tempChar, "%d", sysData.fft.IQData[4095]);
					//    	returnString += tempChar;
					mutexFFT.unlock();
					break;
					//demod
				case SCPI_CMD_OPTIONS_DEMOD_AM_STATE_SET:
					exeResult = setDemodOfAmState(result.value.trimmed());
					break;
				case SCPI_CMD_OPTIONS_DEMOD_AM_STATE_GET:
					exeResult = __SCPI_RETURN;
					returnString = boolToString(sysData.options.amOn);
					break;
				case SCPI_CMD_OPTIONS_DEMOD_AM_DATA_GET:
					mutexDemod.lock();
					exeResult = __SCPI_RETURN;
					returnString = "";
					for (int j = 0; j < DEMODSIZE - 1; j++)
					{
						sprintf(tempChar, "%ld", sysData.options.iqData[j]);
						returnString += tempChar;
						returnString += ",";
					}
					sprintf(tempChar, "%ld", sysData.options.iqData[DEMODSIZE - 1]);
					returnString += tempChar;
					mutexDemod.unlock();
					break;
				case SCPI_CMD_OPTIONS_DEMOD_AM_FFTDATA_GET:
					mutexDemod.lock();
					exeResult = __SCPI_RETURN;
					returnString = "";
					for (int j = 0; j < DEMODFFTSIZE - 1; j++)
					{
						sprintf(tempChar, "%16.0f", sysData.options.fftData[j]);
						returnString += QString(tempChar).trimmed();
						returnString += ",";
					}
					sprintf(tempChar, "%16.0f", sysData.options.fftData[DEMODFFTSIZE - 1]);
					returnString += QString(tempChar).trimmed();
					mutexDemod.unlock();
					break;

					//选件配置
				case SCPI_CMD_OPTIONS_ON_SET:
					exeResult = setSystemServiceOptionOn(result.value.toUpper().trimmed());
					break;
				case SCPI_CMD_OPTIONS_OFF_SET:
					exeResult = setSystemServiceOptionOff(result.value.toUpper().trimmed());
					break;
				case SCPI_CMD_OPTIONS_ONOFF_GET:
					exeResult = __SCPI_RETURN;
					tempValue = result.value.toUpper().trimmed();

					if (tempValue == "TRAC" || tempValue == "TRACK")
					{
						returnString = boolToString(sysData.options.isTrack36GHzOn);
					} else if (tempValue == "FFT")
					{
						returnString = boolToString(sysData.options.isFft10HzOn);
					} else if (tempValue == "DEMOD")
					{
						returnString = boolToString(sysData.options.isDemodOn);
					} else if (tempValue == "FFT")
					{
						returnString = boolToString(sysData.options.isFft10HzOn);
					} else
					{
						returnString = __SCPI_UNSUPPORT;
					}
					break;

				case SCPI_CMD_DEVICE_ON_SET:
					exeResult = setSystemServiceDeviceOn(result.value.toUpper().trimmed());
					break;
				case SCPI_CMD_DEVICE_OFF_SET:
					exeResult = setSystemServiceDeviceOff(result.value.toUpper().trimmed());
					break;
				case SCPI_CMD_DEVICE_ONOFF_GET:
					exeResult = __SCPI_RETURN;
					tempValue = result.value.toUpper().trimmed();

					if (tempValue == "LOG")
					{
						returnString = boolToString(sysData.system.isLogOn);
					} else if (tempValue == "GPIB")
					{
						returnString = boolToString(sysData.system.isGpibOn);
					} else if (tempValue == "EVALUATE" || tempValue == "EVA")
					{
						returnString = boolToString(eva.isEvaluate);
					} else
					{
						returnString = __SCPI_UNSUPPORT;
					}
					break;

				case SCPI_CMD_TRIG_MODE_SET:
					exeResult = setTrigMode(result.value.toUpper().trimmed());
					break;
				case SCPI_CMD_TRIG_MODE_GET:
					exeResult = __SCPI_RETURN;

					switch (sysData.trig.mode)
					{
						case tgRun:
							returnString = "RUN";
							break;
						case tgVideo:
							returnString = "VIDeo";
							break;
						case tgRise:
							returnString = "POSitive";
							break;
						case tgFall:
							returnString = "NEGative";
							break;
						default:
							returnString = __SCPI_UNSUPPORT;
							break;
					}
					break;

				case SCPI_CMD_TRIG_MODE_VIDEO_POWER_SET:
					exeResult = setVideoTrigPower(result.value.toUpper().trimmed());
					break;

				case SCPI_CMD_TRIG_MODE_VIDEO_POWER_GET:
					exeResult = __SCPI_RETURN;
					returnString = QString(floatToStringDot2(getdBmValue(sysData.trig.power), tempChar)).trimmed();
					break;

				case SCPI_CMD_TEST:
					exeResult = __SCPI_RETURN;
					returnString = "";

					mutexAmpt.lock();

					read(ifHandle, sysData.test, sizeof sysData.test);
					for (int i = 0; i < sysData.sweep.sweepPoints; i++)
					{
						sprintf(tempChar, "%u", sysData.test[i]);
						returnString += tempChar;
						returnString += ",";
					}
					mutexAmpt.unlock();
					break;

				//射频前端
				case SCPI_CMD_FRONTEND_TEST:
					exeResult = __SCPI_RETURN;
					returnString = "OK";
					feDownload(207, result.value.toInt());    //本振送数开关
					//setFrontEndFreq(result.value.trimmed());
					break;
				case SCPI_CMD_FRONTEND_FREQ:
					exeResult = __SCPI_RETURN;
					returnString = "OK";
					setFrontEndFreq(result.value.trimmed());
					//rfPreamptDown(sysData.ampt.isPreamptOn);
					//setHighbandAtt(sysData.fe.highbandAtt);
					break;
				case SCPI_CMD_FRONTEND_PREAMPT_ON:
					exeResult = __SCPI_RETURN;
					returnString = "OK";
					rfPreamptDown(result.value.trimmed());
					break;
				case SCPI_CMD_FRONTEND_IF_ATT:
					exeResult = __SCPI_RETURN;
					returnString = "OK";
					setFrontEndIfAtt(result.value.trimmed());
					break;
				case SCPI_CMD_FRONTEND_PE43704:
					exeResult = __SCPI_RETURN;
					returnString = "OK";
					setLowbandAtt(result.value.trimmed());
					break;
				case SCPI_CMD_FRONTEND_HMC1018:
					exeResult = __SCPI_RETURN;
					returnString = "OK";
					setHighbandAtt(result.value.trimmed());
					break;
				case SCPI_CMD_FRONTEND_AD5320:
					exeResult = __SCPI_RETURN;
					returnString = "OK";
					setFrontendRefVolt(result.value.trimmed());
					break;
				case SCPI_CMD_FRONTEND_REFSEL:
					exeResult = __SCPI_RETURN;
					returnString = "OK";
					setFrontendRefSelect(result.value.trimmed());
					break;
				case SCPI_CMD_FRONTEND_HMC704:
					exeResult = __SCPI_RETURN;
					returnString = "OK";
					feDownload(0xfff1,result.value.toInt(&ok, 16));
					break;
				case SCPI_CMD_GPMC_SET:
					{
						exeResult = __SCPI_RETURN;
						returnString = "OK";
						feDownload(0xa0,result.value.toInt());
						break;
					}
				case SCPI_CMD_GPMC_CLR:
					{
						exeResult = __SCPI_RETURN;
						returnString = "OK";
						feDownload(0xa1,result.value.toInt());
						break;
					}
				case SCPI_CMD_READ_TEST:
					{
						int temp = 0;
						exeResult = __SCPI_RETURN;
						returnString = "SCPI_CMD_READ_TEST";
//						feDownload(500, 1);
//						usleep(1);
//						feDownload(500, 0);
					//	read(feHandle, &temp, 4);

						// 地址  :37
						ramDownload(37, result.value.toInt());

						//时钟 :38
						ramDownload(38, 1);
						ramDownload(38, 0);

						read(ramHandle, &temp, 4);
						//feDownload(0xfffa,result.value.toInt());
					}
					break;
				case SCPI_CMD_WRITE_TEST:
					{
						feDownload(0xfffb,result.value.toInt());
						exeResult = __SCPI_RETURN;
						returnString = "SCPI_CMD_WRITE_TEST";
					}
					break;
				case SCPI_CMD_DATA_SAVE:
					{
						QFile myfile("/home/sky/ifdata.c");
						if(myfile.open(QIODevice::ReadWrite | QIODevice::Append))
						{
							QTextStream dataOut(&myfile);
							for(int i = 0; i < RAMDATASIZE; i++)
								dataOut << ifDataBuf[i] << ",";
						}
					}
					break;
			  case SCPI_CMD_UDP_REMOTE_IP_SET:
			  	setUdpIP(result.value.trimmed());
			  	exeResult = __SCPI_RETURN;
			  	break;
			  case SCPI_CMD_UDP_REMOTE_IP_GET:
			  	exeResult = __SCPI_RETURN;
			  //	loadDeviceSetting();
			  //	returnString = uintToString(sysData.system.io.lan.udpRemoteIP);
			  //	returnString = QString(sysData.system.io.lan.udpRemoteIP);
			  	break;
			  case SCPI_CMD_UDP_REMOTE_PORT_SET:
			  	exeResult = __SCPI_RETURN;
			  	setUdpPort(result.value.trimmed());
			  	break;
			  case SCPI_CMD_UDP_REMOTE_PORT_GET:
			  	exeResult = __SCPI_RETURN;
			  	{
			  //	 short port = 0;
			  //	 read(edmaHandle,&port,2);
			  //	  loadDeviceSetting();
			  //	  returnString = intToString(sysData.system.io.lan.udpRemotePort);
			  	}
			  	break;

				case SCPI_CMD_DATA_START:
					{
						int temp = 0;
						exeResult = __SCPI_RETURN;
						returnString = "SCPI_CMD_DATA_START";
						feDownload(500, 1);
						usleep(1);
						feDownload(500, 0);
					}
					break;
				case SCPI_CMD_FRONTDET:
					controlIfDetector();
					break;

				case SCPI_CMD_POWERMETER_FREQ_SET:
					if(connectToPowerMeter() == false)
					{
					}
					exeResult = setPowerMeterFreq(result.value.trimmed());
					getDataFromPowerMeter();
					break;
				case SCPI_CMD_POWERMETER_FREQ_GET:
					break;
				case SCPI_CMD_POWERMETER_AMPT_GET:
					if(getDataFromPowerMeter() == __SCPI_SUCCESS)
					{
						exeResult = __SCPI_RETURN;
						returnString = QString(floatToStringDot3(sysData.measure.powerMeter.ampt, tempChar)).trimmed();
					} else
					{
						exeResult = __SCPI_FAILED;
					}
					break;
				case SCPI_CMD_CALAFREQ:
					printf("SCPI_CMD_CALAFREQ:%s\n",result.value.trimmed().toStdString().c_str());
					break;
				case SCPI_CMD_CLEAR:
					system("mv /home/sky/setting.ini /home/sky/factory.txt /home/sky/file");
					printf("mv caldata succeed!\n");
					break;	
				case SCPI_CMD_RF:
					#if 1
				{
					bool ok;
					int dx = result.value.trimmed().toUInt(&ok,16);
					qDebug() << "ATTSET:" << dx;
					int fd = open(DEVICE_NAME_DC, O_RDWR | O_NONBLOCK);
					if(fd < 0){
						printf("open DEVICE_NAME_DC failed\n");
						return;
					}

					ioctl(fd,0xdc01,dx);//控制衰减器B
					close(fd);
				}
					#endif
					break;
				case SCPI_CMD_RESET:
				{
					int fd = open(DEVICE_NAME_DC, O_RDWR | O_NONBLOCK);
					if(fd < 0){
						printf("open DEVICE_NAME_DC failed\n");
						return;
					}
					printf("open DEVICE_NAME_DC succeed\n");
					ioctl(fd,350,0);
					ioctl(fd,350,1);
					qDebug() << "SCPI_CMD_RESET";
					close(fd);
				}
					break;
				case SCPI_CMD_START:
				{
					int fd = open(DEVICE_NAME_DC, O_RDWR | O_NONBLOCK);
					if(fd < 0){
						printf("open DEVICE_NAME_DC failed\n");
						return;
					}
					printf("open DEVICE_NAME_DC succeed\n");
					ioctl(fd,351,1);
					ioctl(fd,351,0);
					qDebug() << "SCPI_CMD_START";
					close(fd);
				}
					break;
				case SCPI_CMD_ALLLENGTH:
					fft_alllength = result.value.trimmed().toUInt();
					qDebug() << "SCPI_CMD_ALLLENGTH: " << fft_alllength;
					controlDc();
					break;
				case SCPI_CMD_FFTLENGTH:
					fft_length = result.value.trimmed().toUInt();	
					qDebug() << "SCPI_CMD_FFTLENGTH: " << fft_length;
					controlDc();		
					break;
				case SCPI_CMD_FFTDET:					
					fft_det = result.value.trimmed().toUInt() & 0x03;
					qDebug() << "SCPI_CMD_FFTDET: " << fft_det;
					controlDc();
					break;
				case SCPI_CMD_CICCFG:
					cic_config_data = result.value.trimmed().toUInt();
					qDebug() << "SCPI_CMD_CICCFG: " << cic_config_data;
					controlDc();
					break;
				case SCPI_CMD_CTLDC:
					controlDc();					
					qDebug() << "SCPI_CMD_CTLDC " ;
					
					break;
				default:
					exeResult = __SCPI_UNSUPPORT;
					break;
			}
		switch (exeResult)
		{
			case __SCPI_RETURN:
				returnScpiResult(returnString);
				break;
			case __SCPI_FAILED:
				returnScpiResult((QString) SCPICMDERROR);
				break;
			case __SCPI_UNSUPPORT:
				returnScpiResult((QString) SCPICMDUNSUPPORT);
				break;
			default:
				break;
		}
		reDrawMenuFrame();
	}
}

//返回SCPI结果
void tSysScpi::returnScpiResult(QString value)
{
	//printf("%s\n",value.toStdString().c_str());
	if (value.isNull() || value.isEmpty())
	{
		return;
	}

	QString sendString = value + returnEofString;
	unsigned long dataLength = sendString.length();
	unsigned long sendLength = 0;

	switch (sysData.scpiData.interface)
	{
		case sitLan:
			sysData.scpiData.stateT = true;
			//     for (int i = 0; i < MAXTCPCLIENTS; i++)
			{
				//    	if (clientSockets[i].socket != NULL && clientSockets[i].isWriting)
				//if (client_sockfd != 0)
				{
					//          sendLength = clientSockets[i].socket->write(sendString.toStdString().c_str());
					send(curSocket, (void *) sendString.toStdString().c_str(), dataLength, 0);
				}

			}
			sysData.scpiData.stateT = false;

			/*
			 sysData.scpiData.stateT = true;
			 sendLength = clientSocket->write(sendString.toStdString().c_str());
			 sysData.scpiData.stateT = false;

			 if (!clientSocket->waitForBytesWritten(30000) || sendLength != dataLength)
			 {
			 //clientSocketDisConnect();
			 }
			 */
			break;

		case sitUsb:
			sysData.scpiData.stateT = true;
			sendLength = write(usbHandle, sendString.toStdString().c_str(), sendString.length() * sizeof(char));
			sysData.scpiData.stateT = false;
			break;

		case sitGpib:
			write(gpibHandle, sendString.toStdString().c_str(), sendString.length());
			/*
			 if (gpibReaded)
			 {
			 write(gpibHandle, sendString.toStdString().c_str(), sendString.length());

			 if (gpibList.count() > 0)
			 {
			 gpibList.removeAt(0);
			 }
			 } else
			 {
			 gpibList << sendString;

			 if (gpibList.count() > 64)
			 {
			 gpibList.removeAt(0);
			 }
			 }
			 */
			break;

		case sitCom:
			sendSerialData(true, sendString);
			break;
	}
}

//返回SCPI结果
int tSysScpi::returnScpiResult(QByteArray value)
{
	if (value.isNull() || value.length() <= 0)
	{
		return __SCPI_FAILED;
	}

	switch (sysData.scpiData.interface)
	{
		case sitLan:
			sysData.scpiData.stateT = true;
			for (int i = 0; i < MAXTCPCLIENTS; i++)
			{
				if (clientSockets[i].socket != NULL && clientSockets[i].isWriting)
				{
					clientSockets[i].socket->write(value);
					clientSockets[i].socket->write((char*) SCPIRESULTEOF);
				}
			}
			//clientSocket->write(value);
			//clientSocket->write((char*)SCPIRESULTEOF);
			sysData.scpiData.stateT = false;
			return __SCPI_SUCCESS;
			break;
		case sitUsb:
			break;
	}

	return __SCPI_FAILED;
}
