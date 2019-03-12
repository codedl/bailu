#include "mainform.h"

void mainForm::gotoTheEMINode(void)
{
	subKeyDef* tempSubKey = nowSubKey;

	if (tempSubKey == NULL)
	{
		return;
	}

	while (tempSubKey ->upNode != NULL)
	{
		tempSubKey = tempSubKey->upNode;
	}

	unsigned long funcCode = 0;

	if ((unsigned long) sysData.bw.rbw == 1e6)
	{
		funcCode = KEY_FUNC_BW_EMC_1MHZ;
	} else if ((unsigned long) sysData.bw.rbw == 120e3)
	{
		funcCode = KEY_FUNC_BW_EMC_120KHZ;
	} else if ((unsigned long) sysData.bw.rbw == 30e3)
	{
		funcCode = KEY_FUNC_BW_EMC_30KHZ;
	} else if ((unsigned long) sysData.bw.rbw == 9e3)
	{
		funcCode = KEY_FUNC_BW_EMC_9KHZ;
	} else if ((unsigned long) sysData.bw.rbw == 200)
	{
		funcCode = KEY_FUNC_BW_EMC_200HZ;
	} else
	{
		funcCode = KEY_FUNC_BW_EMC_EMC;
	}

	while (tempSubKey != NULL)
	{
		if (funcCode > 0 && tempSubKey->funcCode == funcCode)
		{
			nowSubKey = tempSubKey;
			return;
		}

		tempSubKey = tempSubKey->downNode;
	}
}

//上方向键
bool mainForm::isArrowUp(int keyValue)
{
	return keyValue == KEY_ARROW_UP;
}

//右脉冲旋钮
bool mainForm::isRpgRight(int keyValue)
{

}

//执行键返回
bool mainForm::isExecReturn(int funcCode)
{
	return  funcCode == KEY_FUNC_AMPT_REFUNIT_DBM ||
					funcCode == KEY_FUNC_AMPT_REFUNIT_DBMV ||
					funcCode == KEY_FUNC_AMPT_REFUNIT_DBUV ||
					funcCode == KEY_FUNC_AMPT_REFUNIT_W ||
					funcCode == KEY_FUNC_AMPT_REFUNIT_V ||
					funcCode == KEY_FUNC_AMPT_REFUNIT_DBPW ||
					funcCode == KEY_FUNC_AMPT_REFUNIT_DBuW ||

					funcCode == KEY_FUNC_BW_EMC_1MHZ ||
					funcCode == KEY_FUNC_BW_EMC_120KHZ ||
					funcCode == KEY_FUNC_BW_EMC_30KHZ ||
					funcCode == KEY_FUNC_BW_EMC_9KHZ ||
					funcCode == KEY_FUNC_BW_EMC_200HZ ||

					funcCode == KEY_FUNC_TRIG_EXTERNAL_FALL ||
					funcCode == KEY_FUNC_TRIG_EXTERNAL_RISE ||

					funcCode == KEY_FUNC_DISPLAY_STYLE_DEFAULT ||
					funcCode == KEY_FUNC_DISPLAY_STYLE_STYLE1 ||

					funcCode == KEY_FUNC_MEAS_DISCRIMINATOR_AMPLITUDE_INPUTGAIN_X1 ||
					funcCode == KEY_FUNC_MEAS_DISCRIMINATOR_AMPLITUDE_INPUTGAIN_X2 ||
					funcCode == KEY_FUNC_MEAS_DISCRIMINATOR_AMPLITUDE_INPUTGAIN_X4 ||
					funcCode == KEY_FUNC_MEAS_DISCRIMINATOR_AMPLITUDE_INPUTGAIN_X05 ||
					funcCode == KEY_FUNC_MEAS_DISCRIMINATOR_AMPLITUDE_INPUTGAIN_X025 ||

					funcCode == KEY_FUNC_PRINTSETUP_PAPER_A4 ||
					funcCode == KEY_FUNC_PRINTSETUP_PAPER_A3 ||
					funcCode == KEY_FUNC_PRINTSETUP_PAPER_B5 ||
					funcCode == KEY_FUNC_PRINTSETUP_PAPER_C5 ||
					funcCode == KEY_FUNC_PRINTSETUP_PAPER_LETTER ||

					funcCode == KEY_FUNC_SYSTEM_LANGUAGE_CHINESE ||
					funcCode == KEY_FUNC_SYSTEM_LANGUAGE_ENGLISH ||

					funcCode == KEY_FUNC_FILE_SHOW_SCREENS ||
					funcCode == KEY_FUNC_FILE_SHOW_TRACEDATAS ||
					funcCode == KEY_FUNC_FILE_SHOW_ALL ||

					funcCode == KEY_FUNC_FILE_OPERATIONS_SORT_NAME ||
					funcCode == KEY_FUNC_FILE_OPERATIONS_SORT_DATETIME ||
					funcCode == KEY_FUNC_FILE_OPERATIONS_SORT_SIZE ||

					funcCode == KEY_FUNC_SYSTEM_CONFIGIO_COM_BAUDRATE_1200 ||
					funcCode == KEY_FUNC_SYSTEM_CONFIGIO_COM_BAUDRATE_2400 ||
					funcCode == KEY_FUNC_SYSTEM_CONFIGIO_COM_BAUDRATE_4800 ||
					funcCode == KEY_FUNC_SYSTEM_CONFIGIO_COM_BAUDRATE_9600 ||
					funcCode == KEY_FUNC_SYSTEM_CONFIGIO_COM_BAUDRATE_19200 ||
					funcCode == KEY_FUNC_SYSTEM_CONFIGIO_COM_BAUDRATE_38400 ||
					funcCode == KEY_FUNC_SYSTEM_CONFIGIO_COM_BAUDRATE_57600 ||
					funcCode == KEY_FUNC_SYSTEM_CONFIGIO_COM_BAUDRATE_115200 ||

					funcCode == KEY_FUNC_SYSTEM_CONFIGIO_COM_DATABITS_5 ||
					funcCode == KEY_FUNC_SYSTEM_CONFIGIO_COM_DATABITS_6 ||
					funcCode == KEY_FUNC_SYSTEM_CONFIGIO_COM_DATABITS_7 ||
					funcCode == KEY_FUNC_SYSTEM_CONFIGIO_COM_DATABITS_8 ||

					funcCode == KEY_FUNC_SYSTEM_CONFIGIO_COM_STOPBITS_1 ||
					funcCode == KEY_FUNC_SYSTEM_CONFIGIO_COM_STOPBITS_2 ||

					funcCode == KEY_FUNC_SYSTEM_CONFIGIO_COM_CHECK_EVEN ||
					funcCode == KEY_FUNC_SYSTEM_CONFIGIO_COM_CHECK_ODD ||
					funcCode == KEY_FUNC_SYSTEM_CONFIGIO_COM_CHECK_NONE ||
					funcCode == KEY_FUNC_SYSTEM_CONFIGIO_COM_CHECK_SPACE ||

					funcCode == KEY_FUNC_SYSTEM_CONFIGIO_COM_FLOWCONTROL_XONXOFF ||
					funcCode == KEY_FUNC_SYSTEM_CONFIGIO_COM_FLOWCONTROL_HARDWARE ||
					funcCode == KEY_FUNC_SYSTEM_CONFIGIO_COM_FLOWCONTROL_NONE ||

					funcCode == KEY_FUNC_SYSTEM_POWERONPRESET_POWERON_FACTORY ||
					funcCode == KEY_FUNC_SYSTEM_POWERONPRESET_POWERON_USER ||
					funcCode == KEY_FUNC_SYSTEM_POWERONPRESET_POWERON_POWEROFF ||
					funcCode == KEY_FUNC_SYSTEM_POWERONPRESET_PRESET_FACTORY ||
					funcCode == KEY_FUNC_SYSTEM_POWERONPRESET_PRESET_USER ||
					funcCode == KEY_FUNC_SYSTEM_POWERONPRESET_PRESET_POWEROFF ||

					funcCode == KEY_FUNC_SYSTEM_DATETIME_FORMAT_YMDHMS ||
					funcCode == KEY_FUNC_SYSTEM_DATETIME_FORMAT_HMSYMD ||

					funcCode == KEY_FUNC_SOURCE_TRACKGEN_MEASTYPE_S21 ||
					funcCode == KEY_FUNC_SOURCE_TRACKGEN_MEASTYPE_S11;
}

//处理方向键
void mainForm::handleArrowKey(int funcCode, bool up)
{
	//passwd
	if (nowMainKey != NULL && nowSubKey != NULL && nowSubKey->isPassword && isPassword())
	{
		if (indexOfPassword <= 0 || indexOfPassword > 6)
		{
			return;
		}

		unsigned char tempValue = passwdString[indexOfPassword - 1];
		char result[1];
		QString tempPasswd = "";

		if (up)
		{
			if (tempValue < 0xf)
			{
				tempValue++;
			} else
			{
				tempValue = 0x0;
			}
		} else
		{
			if (tempValue > 0x0)
			{
				tempValue--;
			} else
			{
				tempValue = 0xf;
			}
		}

		tempPasswd = QString(intToHex(tempValue, result)).trimmed();
		execNumberInput(tempPasswd, false);
		return;
	}

	//Pass-Fail
	if (nowMainKey != NULL && nowSubKey != NULL && sysData.mode == smPassFail &&
			(nowSubKey->funcCode == KEY_FUNC_AMPT_PASSFAIL_LINE_LINE1EDIT_OPERATION_ADD ||
					nowSubKey->funcCode == KEY_FUNC_AMPT_PASSFAIL_LINE_LINE1EDIT_OPERATION_INSERT ||
					nowSubKey->funcCode == KEY_FUNC_AMPT_PASSFAIL_LINE_LINE1EDIT_OPERATION_DELETE ||
					nowSubKey->funcCode == KEY_FUNC_AMPT_PASSFAIL_LINE_LINE1EDIT_OPERATION_CLEAR ||
					nowSubKey->funcCode == KEY_FUNC_AMPT_PASSFAIL_LINE_LINE1EDIT_OPERATION_EDITFREQ ||
					nowSubKey->funcCode == KEY_FUNC_AMPT_PASSFAIL_LINE_LINE1EDIT_OPERATION_EDITLIMIT ||
					nowSubKey->funcCode == KEY_FUNC_AMPT_PASSFAIL_LINE_LINE1EDIT_FIRSTPAGE ||
					nowSubKey->funcCode == KEY_FUNC_AMPT_PASSFAIL_LINE_LINE1EDIT_PREVPAGE ||
					nowSubKey->funcCode == KEY_FUNC_AMPT_PASSFAIL_LINE_LINE1EDIT_NEXTPAGE ||
					nowSubKey->funcCode == KEY_FUNC_AMPT_PASSFAIL_LINE_LINE1EDIT_LASTPAGE ||
					nowSubKey->funcCode == KEY_FUNC_AMPT_PASSFAIL_LINE_LINE1EDIT_OPERATION ||
					nowSubKey->funcCode	== KEY_FUNC_AMPT_PASSFAIL_LINE_LINE2EDIT_OPERATION_ADD ||
					nowSubKey->funcCode == KEY_FUNC_AMPT_PASSFAIL_LINE_LINE2EDIT_OPERATION_INSERT ||
					nowSubKey->funcCode == KEY_FUNC_AMPT_PASSFAIL_LINE_LINE2EDIT_OPERATION_DELETE ||
					nowSubKey->funcCode == KEY_FUNC_AMPT_PASSFAIL_LINE_LINE2EDIT_OPERATION_CLEAR ||
					nowSubKey->funcCode == KEY_FUNC_AMPT_PASSFAIL_LINE_LINE2EDIT_OPERATION_EDITFREQ ||
					nowSubKey->funcCode == KEY_FUNC_AMPT_PASSFAIL_LINE_LINE2EDIT_OPERATION_EDITLIMIT ||
					nowSubKey->funcCode	== KEY_FUNC_AMPT_PASSFAIL_LINE_LINE2EDIT_FIRSTPAGE ||
					nowSubKey->funcCode == KEY_FUNC_AMPT_PASSFAIL_LINE_LINE2EDIT_PREVPAGE ||
					nowSubKey->funcCode == KEY_FUNC_AMPT_PASSFAIL_LINE_LINE2EDIT_NEXTPAGE ||
					nowSubKey->funcCode == KEY_FUNC_AMPT_PASSFAIL_LINE_LINE2EDIT_LASTPAGE ||
					nowSubKey->funcCode == KEY_FUNC_AMPT_PASSFAIL_LINE_LINE2EDIT_OPERATION))
	{
		if (up)
		{
			sysScpi->gotoRowOfMeasureOfPassFailLineData(1);
		} else
		{
			sysScpi->gotoRowOfMeasureOfPassFailLineData(-1);
		}

		return;
	}

	if (handleFileSelect(up, 1))
	{
		return;
	}

	switch (funcCode)
	{
		//AM、FM
		case KEY_FUNC_MODE_OPTIONS_DEMOD_AM_CARRIERFREQ:
			sysScpi->setDemodOfAmCarrierFreqWithArrow(up);
			break;
		case KEY_FUNC_MODE_OPTIONS_DEMOD_AM_IFBW:
			sysScpi->setDemodOfAmIFBWWithArrow(up);
			break;
		case KEY_FUNC_MODE_OPTIONS_DEMOD_FM_CARRIERFREQ:
			sysScpi->setDemodOfFmCarrierFreqWithArrow(up);
			break;
		case KEY_FUNC_MODE_OPTIONS_DEMOD_FM_IFBW:
			sysScpi->setDemodOfFmIFBWWithArrow(up);
			break;

		case KEY_FUNC_DEMOD_RADIOSET_1:
			sysScpi->setDemodOfTheFrequencyWithArrow(radio1, up);
			break;
		case KEY_FUNC_DEMOD_RADIOSET_MUSIC:
			sysScpi->setDemodOfTheFrequencyWithArrow(radio2, up);
			break;
		case KEY_FUNC_DEMOD_RADIOSET_TRAFFIC:
			sysScpi->setDemodOfTheFrequencyWithArrow(radio3, up);
			break;
		case KEY_FUNC_DEMOD_RADIOSET_2:
			sysScpi->setDemodOfTheFrequencyWithArrow(radio4, up);
			break;
		case KEY_FUNC_DEMOD_RADIOSET_3:
			sysScpi->setDemodOfTheFrequencyWithArrow(radio5, up);
			break;
		case KEY_FUNC_DEMOD_RADIOSET_4:
			sysScpi->setDemodOfTheFrequencyWithArrow(radio6, up);
			break;

		case KEY_FUNC_FREQ_CENTER:
			sysScpi->setFrequencyOfCenterWithArrow(up);
			break;
		case KEY_FUNC_FREQ_START:
			sysScpi->setFrequencyOfStartWithArrow(up);
			break;
		case KEY_FUNC_FREQ_STOP:
			sysScpi->setFrequencyOfStopWithArrow(up);
			break;
		case KEY_FUNC_FREQ_CFSTEP:
			sysScpi->setFrequencyOfCenterStepWithArrow(up);
			break;
		case KEY_FUNC_FREQ_OFFSET:
			sysScpi->setFrequencyOfOffsetWithArrow(up);
			break;

		case KEY_FUNC_SPAN_SPAN:
			sysScpi->setFrequencyOfSpanWithArrow(up);
			break;

		case KEY_FUNC_TRIG_VIDEO:
			sysScpi->setVideoTrigPowerWithArrow(up);
			break;

		case KEY_FUNC_AMPT_REFLEVEL:
			sysScpi->setAmptOfRefLevelWithArrow(up);
			break;
		case KEY_FUNC_AMPT_ATT:
			sysScpi->setAmptOfAttWithArrow(up);
			break;

		case KEY_FUNC_AUX_IMPEDANCE_CONVERSION:
			sysScpi->setAmptOfLMP_CWithArrow(up);
			break;
		case KEY_FUNC_AMPT_SCALEDIV:
			sysScpi->setAmptOfScaleDivWithArrow(up);
			break;
		case KEY_FUNC_AMPT_REFOFFSET:
			sysScpi->setAmptOfRefOffsetWithArrow(up);
			break;

		case KEY_FUNC_BW_RBW:
			sysScpi->setBwOfRbwWithArrow(up);
			break;
		case KEY_FUNC_BW_VBW:
			sysScpi->setBwOfVbwWithArrow(up);
			break;
		case KEY_FUNC_BW_AVERAGE:
			sysScpi->setBwOfAverageWithArrow(up);
			break;

		case KEY_FUNC_SWEEP_SWEEPTIME:
		case KEY_FUNC_SOURCE_TRACKGEN_NETWORK_SWEEPTIME:
			sysScpi->setSweepOfTimeWithArrow(up);
			break;
		case KEY_FUNC_SWEEP_SWEEPPOINTS:
		case KEY_FUNC_SOURCE_TRACKGEN_NETWORK_SWEEPPOINT:
			sysScpi->setSweepOfPointsWithArrow(up);
			break;

		case KEY_FUNC_DISPLAY_DISPLAYLINE:
			sysScpi->setDisplayOfLineValueWithArrow(up);
			break;

		case KEY_FUNC_MEASSETUP_CHANNELWIDTH:
			sysScpi->setMeasSetupOfChannelWidthWithArrow(up);
			break;
		case KEY_FUNC_MEASSETUP_CHANNELINTERVAL:
			sysScpi->setMeasSetupOfChannelSpaceWithArrow(up);
			break;
		case KEY_FUNC_MEASSETUP_CHANNELNUMS:
			sysScpi->setMeasSetupOfChannelNumsWithArrow(up);
			break;
		case KEY_FUNC_MEASSETUP_POWERBWPERCENT:
			sysScpi->setMeasSetupOfPowerPercentWithArrow(up);
			break;

		case KEY_FUNC_MEAS_DISCRIMINATOR_AMPLITUDE_OUTPUTPOWER:
			sysScpi->setMeasureOfDiscOutputPowerWithArrow(up);
			break;
		case KEY_FUNC_MEAS_DISCRIMINATOR_AMPLITUDE_SCALEDIV:
			sysScpi->setMeasureOfDiscScaleDivWithArrow(up);
			break;
		case KEY_FUNC_MEAS_DISCRIMINATOR_SWEEPTIME:
			sysScpi->setMeasureOfDiscSweepTimeWithArrow(up);
			break;
		case KEY_FUNC_MEAS_DISCRIMINATOR_AMPLITUDE_OFFSET:
			sysScpi->setMeasureOfDiscDCOffsetWithArrow(up);
			break;

		case KEY_FUNC_MARKERFCTN_NDB:
			sysScpi->setMarkerFunctionNdBValueWithArrow(up);
			break;
		case KEY_FUNC_MARKERFCTN_FREQCOUNT_BW:
			sysScpi->setMarkerFunctionFreqCountBwWithArrow(up);
			break;

		case KEY_FUNC_PRINTSETUP_NUMS:
			sysScpi->setPrintOfNumsWithArrow(up);
			break;

		case KEY_FUNC_SYSTEM_BACKLIGHT:
			sysScpi->setSystemLcdBacklightWidthArrow(up);
			break;
		case KEY_FUNC_SYSTEM_SERVICE_LANMACADDR:
			sysScpi->setSystemServiceOfLanMacAddressWidthArrow(up);
			break;
		case KEY_FUNC_SYSTEM_SERVICE_CALIBRATION_EDIT_9K:
			sysData.ampt.isPreamptOn = false;
			sysScpi->modifyFrequencyRespondOf9kHzWithArrow(up);
			break;
		case KEY_FUNC_SYSTEM_SERVICE_CALIBRATION_EDIT_20K:
			sysData.ampt.isPreamptOn = false;
			sysScpi->modifyFrequencyRespondOf20kHzWithArrow(up);
			break;

		case KEY_FUNC_SOURCE_TRACKGEN_POWER:
			sysScpi->setSourceTrackGenOutputPowerWithArrow(up);
			break;
		case KEY_FUNC_SOURCE_TRACKGEN_NETWORK_REFLEVEL:
			sysScpi->setSourceTrackGenNetworkReflevelWithArrow(up);
			break;
		case KEY_FUNC_SOURCE_TRACKGEN_NETWORK_POWER:
			sysScpi->setSourceTrackGenNetworkOutputPowerWithArrow(up);
			break;
		case KEY_FUNC_SOURCE_TRACKGEN_VSWR_REFERENCE:
			sysScpi->setSourceTrackGenVSWRRefLevelWithArrow(up);
			break;
		case KEY_FUNC_SOURCE_SIGNALGEN_FREQ:
			sysScpi->setSourceSignalGenOutputFreqWithArrow(up);
			break;
		case KEY_FUNC_SOURCE_SIGNALGEN_POWER:
			sysScpi->setSourceSignalGenOutputPowerWithArrow(up);
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_GPIB_ADDR:
			sysScpi->setGpibAddrWithArrow(up);
			break;

		case KEY_FUNC_DEMOD_SOUND:
			sysScpi->setDemodOfSoundWithArrow(up);
			break;

		case KEY_FUNC_AMPT_PASSFAIL_WINDOW_LIMITUPLOW:
			sysScpi->setMeasureOfPassFailOfWinLineValueWithArrow(up);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_WINDOW_FREQSTARTSTOP:
			sysScpi->setMeasureOfPassFailOfWinFreqValueWithArrow(up);
			break;

	}
}

//处理脉冲旋钮键
void mainForm::handleRpgKey(int funcCode, int nums)
{
	if (handleFileSelect(nums > 0, nums))
	{
		return;
	}

	//zoom in/out
	if (nowMainKey != NULL && nowSubKey != NULL && nowSubKey->funcCode == KEY_FUNC_DISPLAY_ZOOM && sysData.display.isZoomOn)
	{
		sysScpi->setFrequencyOfCenterWithRpg(nums);
		return;
	}

	//passwd
	if (nowMainKey != NULL && nowSubKey != NULL && nowSubKey->isPassword && isPassword())
	{
		unsigned char temp = passwdString[indexOfPassword - 1];
		char result[1];
		QString tempPasswd = "";

		if (indexOfPassword <= 0 || indexOfPassword > 6)
		{
			return;
		}

		if (nums >= 0)
		{
			if (temp < 0xf)
			{
				temp++;
			} else
			{
				temp = 0x0;
			}
		} else
		{
			if (temp > 0x0)
			{
				temp--;
			} else
			{
				temp = 0xf;
			}
		}

		tempPasswd = QString(intToHex(temp, result)).trimmed();
		execNumberInput(tempPasswd, false);
		return;
	}

	//marker
	if (nowMainKey != NULL && nowSubKey != NULL && (nowMainKey->keyValue == KEY_MAIN_MARKER || nowMainKey->keyValue == KEY_MAIN_MARKERFCTN || nowMainKey->keyValue == KEY_MAIN_MARKERTO || nowMainKey->keyValue == KEY_MAIN_PEAK || (nowMainKey->keyValue == KEY_MAIN_SOURCE && !nowSubKey->isInput)) && sysData.marker[sysData.markerSelIndex].state != msClose)
	{
		int tempValue = 0;

		if (sysData.marker[sysData.markerSelIndex].setted)
		{
			sysData.marker[sysData.markerSelIndex].setted = false;
			sysData.marker[sysData.markerSelIndex].settedValue = 0;
		}

		if (sysData.marker[sysData.markerSelIndex].state == msNormal)
		{
			tempValue = sysData.marker[sysData.markerSelIndex].markerNormal.xIndex;
			tempValue += nums;

			if (tempValue < 0)
			{
				tempValue = 0;
			} else if (tempValue >= sysData.sweep.sweepPoints)
			{
				tempValue = sysData.sweep.sweepPoints - 1;
			}

			if (sysData.marker[sysData.markerSelIndex].markerNormal.xIndex != tempValue)
			{
				sysData.marker[sysData.markerSelIndex].markerNormal.xIndex = tempValue;
			}
		} else if (sysData.marker[sysData.markerSelIndex].state == msDelta)
		{
			tempValue = sysData.marker[sysData.markerSelIndex].markerDelta.x1Index;
			tempValue += nums;

			if (tempValue < 0)
			{
				tempValue = 0;
			} else if (tempValue >= sysData.sweep.sweepPoints)
			{
				tempValue = sysData.sweep.sweepPoints - 1;
			}

			if (sysData.marker[sysData.markerSelIndex].markerDelta.x1Index != tempValue)
			{
				sysData.marker[sysData.markerSelIndex].markerDelta.x1Index = tempValue;
			}
		}

		return;
	}

	//Pass-Fail
	if (nowMainKey != NULL && nowSubKey != NULL && 	sysData.mode == smPassFail &&
			(nowSubKey->funcCode == KEY_FUNC_AMPT_PASSFAIL_LINE_LINE1EDIT_OPERATION_ADD ||
					nowSubKey->funcCode == KEY_FUNC_AMPT_PASSFAIL_LINE_LINE1EDIT_OPERATION_INSERT ||
					nowSubKey->funcCode == KEY_FUNC_AMPT_PASSFAIL_LINE_LINE1EDIT_OPERATION_DELETE ||
					nowSubKey->funcCode == KEY_FUNC_AMPT_PASSFAIL_LINE_LINE1EDIT_OPERATION_CLEAR ||
					nowSubKey->funcCode == KEY_FUNC_AMPT_PASSFAIL_LINE_LINE1EDIT_OPERATION_EDITFREQ	||
					nowSubKey->funcCode == KEY_FUNC_AMPT_PASSFAIL_LINE_LINE1EDIT_OPERATION_EDITLIMIT ||
					nowSubKey->funcCode == KEY_FUNC_AMPT_PASSFAIL_LINE_LINE1EDIT_FIRSTPAGE ||
					nowSubKey->funcCode == KEY_FUNC_AMPT_PASSFAIL_LINE_LINE1EDIT_PREVPAGE ||
					nowSubKey->funcCode == KEY_FUNC_AMPT_PASSFAIL_LINE_LINE1EDIT_NEXTPAGE ||
					nowSubKey->funcCode == KEY_FUNC_AMPT_PASSFAIL_LINE_LINE1EDIT_LASTPAGE ||
					nowSubKey->funcCode == KEY_FUNC_AMPT_PASSFAIL_LINE_LINE1EDIT_OPERATION ||
					nowSubKey->funcCode	== KEY_FUNC_AMPT_PASSFAIL_LINE_LINE2EDIT_OPERATION_ADD ||
					nowSubKey->funcCode == KEY_FUNC_AMPT_PASSFAIL_LINE_LINE2EDIT_OPERATION_INSERT ||
					nowSubKey->funcCode == KEY_FUNC_AMPT_PASSFAIL_LINE_LINE2EDIT_OPERATION_DELETE ||
					nowSubKey->funcCode == KEY_FUNC_AMPT_PASSFAIL_LINE_LINE2EDIT_OPERATION_CLEAR ||
					nowSubKey->funcCode == KEY_FUNC_AMPT_PASSFAIL_LINE_LINE2EDIT_OPERATION_EDITFREQ ||
					nowSubKey->funcCode == KEY_FUNC_AMPT_PASSFAIL_LINE_LINE2EDIT_OPERATION_EDITLIMIT ||
					nowSubKey->funcCode	== KEY_FUNC_AMPT_PASSFAIL_LINE_LINE2EDIT_FIRSTPAGE ||
					nowSubKey->funcCode == KEY_FUNC_AMPT_PASSFAIL_LINE_LINE2EDIT_PREVPAGE ||
					nowSubKey->funcCode == KEY_FUNC_AMPT_PASSFAIL_LINE_LINE2EDIT_NEXTPAGE ||
					nowSubKey->funcCode == KEY_FUNC_AMPT_PASSFAIL_LINE_LINE2EDIT_LASTPAGE ||
					nowSubKey->funcCode == KEY_FUNC_AMPT_PASSFAIL_LINE_LINE2EDIT_OPERATION))
	{
		sysScpi->gotoRowOfMeasureOfPassFailLineData(nums);
		return;
	}

	switch (funcCode)
	{
		//AM、FM
		case KEY_FUNC_MODE_OPTIONS_DEMOD_AM_CARRIERFREQ:
			sysScpi->setDemodOfAmCarrierFreqWithRpg(nums);
			break;
		case KEY_FUNC_MODE_OPTIONS_DEMOD_AM_IFBW:
			sysScpi->setDemodOfAmIFBWWithRpg(nums);
			break;
		case KEY_FUNC_MODE_OPTIONS_DEMOD_FM_CARRIERFREQ:
			sysScpi->setDemodOfFmCarrierFreqWithRpg(nums);
			break;
		case KEY_FUNC_MODE_OPTIONS_DEMOD_FM_IFBW:
			sysScpi->setDemodOfFmIFBWWithRpg(nums);
			break;

		case KEY_FUNC_DEMOD_RADIOSET_1:
			sysScpi->setDemodOfTheFrequencyWithRpg(radio1, nums);
			break;
		case KEY_FUNC_DEMOD_RADIOSET_MUSIC:
			sysScpi->setDemodOfTheFrequencyWithRpg(radio2, nums);
			break;
		case KEY_FUNC_DEMOD_RADIOSET_TRAFFIC:
			sysScpi->setDemodOfTheFrequencyWithRpg(radio3, nums);
			break;
		case KEY_FUNC_DEMOD_RADIOSET_2:
			sysScpi->setDemodOfTheFrequencyWithRpg(radio4, nums);
			break;
		case KEY_FUNC_DEMOD_RADIOSET_3:
			sysScpi->setDemodOfTheFrequencyWithRpg(radio5, nums);
			break;
		case KEY_FUNC_DEMOD_RADIOSET_4:
			sysScpi->setDemodOfTheFrequencyWithRpg(radio6, nums);
			break;

		case KEY_FUNC_FREQ_CENTER:
			sysScpi->setFrequencyOfCenterWithRpg(nums);
			break;
		case KEY_FUNC_FREQ_START:
			sysScpi->setFrequencyOfStartWithRpg(nums);
			break;
		case KEY_FUNC_FREQ_STOP:
			sysScpi->setFrequencyOfStopWithRpg(nums);
			break;
		case KEY_FUNC_FREQ_CFSTEP:
			sysScpi->setFrequencyOfCenterStepWithRpg(nums);
			break;
		case KEY_FUNC_FREQ_OFFSET:
			sysScpi->setFrequencyOfOffsetWithRpg(nums);
			break;

		case KEY_FUNC_SPAN_SPAN:
			sysScpi->setFrequencyOfSpanWithRpg(nums);
			break;

			//trig
		case KEY_FUNC_TRIG_VIDEO:
			sysScpi->setVideoTrigPowerWithRpg(nums);
			break;

		case KEY_FUNC_AMPT_REFLEVEL:
			sysScpi->setAmptOfRefLevelWithRpg(nums);
			break;
		case KEY_FUNC_AMPT_ATT:
			sysScpi->setAmptOfAttWithRpg(nums);
			break;
		case KEY_FUNC_AMPT_SCALEDIV:
			sysScpi->setAmptOfScaleDivWithRpg(nums);
			break;
		case KEY_FUNC_AMPT_REFOFFSET:
			sysScpi->setAmptOfRefOffsetWithRpg(nums);
			break;
		case KEY_FUNC_AUX_IMPEDANCE_CONVERSION:
			sysScpi->setAmptOfLMP_CWithRpg(nums);
			break;
		case KEY_FUNC_BW_RBW:
			sysScpi->setBwOfRbwWithRpg(nums);
			break;
		case KEY_FUNC_BW_VBW:
			sysScpi->setBwOfVbwWithRpg(nums);
			break;
		case KEY_FUNC_BW_AVERAGE:
			sysScpi->setBwOfAverageWithRpg(nums);
			break;

		case KEY_FUNC_SWEEP_SWEEPTIME:
		case KEY_FUNC_SOURCE_TRACKGEN_NETWORK_SWEEPTIME:
			sysScpi->setSweepOfTimeWithRpg(nums);
			break;
		case KEY_FUNC_SWEEP_SWEEPPOINTS:
		case KEY_FUNC_SOURCE_TRACKGEN_NETWORK_SWEEPPOINT:
			sysScpi->setSweepOfPointsWithRpg(nums);
			break;

		case KEY_FUNC_DISPLAY_DISPLAYLINE:
			sysScpi->setDisplayOfLineValueWithRpg(nums);
			break;

		case KEY_FUNC_MEASSETUP_CHANNELWIDTH:
			sysScpi->setMeasSetupOfChannelWidthWithRpg(nums);
			break;
		case KEY_FUNC_MEASSETUP_CHANNELINTERVAL:
			sysScpi->setMeasSetupOfChannelSpaceWithRpg(nums);
			break;
		case KEY_FUNC_MEASSETUP_CHANNELNUMS:
			sysScpi->setMeasSetupOfChannelNumsWithRpg(nums);
			break;
		case KEY_FUNC_MEASSETUP_POWERBWPERCENT:
			sysScpi->setMeasSetupOfPowerPercentWithRpg(nums);
			break;
		case KEY_FUNC_MEAS_DISCRIMINATOR_AMPLITUDE_OUTPUTPOWER:
			sysScpi->setMeasureOfDiscOutputPowerWithRpg(nums);
			break;
		case KEY_FUNC_MEAS_DISCRIMINATOR_AMPLITUDE_SCALEDIV:
			sysScpi->setMeasureOfDiscScaleDivWithRpg(nums);
			break;
		case KEY_FUNC_MEAS_DISCRIMINATOR_SWEEPTIME:
			sysScpi->setMeasureOfDiscSweepTimeWithRpg(nums);
			break;
		case KEY_FUNC_MEAS_DISCRIMINATOR_AMPLITUDE_OFFSET:
			sysScpi->setMeasureOfDiscDCOffsetWithRpg(nums);
			break;

		case KEY_FUNC_MARKERFCTN_NDB:
			sysScpi->setMarkerFunctionNdBValueWithRpg(nums);
			break;
		case KEY_FUNC_MARKERFCTN_FREQCOUNT_BW:
			sysScpi->setMarkerFunctionFreqCountBwWithRpg(nums);
			break;

		case KEY_FUNC_PRINTSETUP_NUMS:
			sysScpi->setPrintOfNumsWithRpg(nums);
			break;

		case KEY_FUNC_SYSTEM_BACKLIGHT:
			sysScpi->setSystemLcdBacklightWithRpg(nums);
			break;
		case KEY_FUNC_SYSTEM_SERVICE_LANMACADDR:
			sysScpi->setSystemServiceOfLanMacAddressWidthRpg(nums);
			break;
		case KEY_FUNC_SYSTEM_SERVICE_CALIBRATION_EDIT_9K:
			sysData.ampt.isPreamptOn = false;
			sysScpi->modifyFrequencyRespondOf9kHzWithRpg(nums);
			break;
		case KEY_FUNC_SYSTEM_SERVICE_CALIBRATION_EDIT_20K:
			sysData.ampt.isPreamptOn = false;
			sysScpi->modifyFrequencyRespondOf20kHzWithRpg(nums);
			break;

		case KEY_FUNC_SYSTEM_CONFIGIO_GPIB_ADDR:
			sysScpi->setGpibAddrWithRpg(nums);
			break;

		case KEY_FUNC_SOURCE_TRACKGEN_POWER:
			sysScpi->setSourceTrackGenOutputPowerWithRpg(nums);
			break;
		case KEY_FUNC_SOURCE_TRACKGEN_NETWORK_REFLEVEL:
			sysScpi->setSourceTrackGenNetworkReflevelWithRpg(nums);
			break;
		case KEY_FUNC_SOURCE_TRACKGEN_NETWORK_POWER:
			sysScpi->setSourceTrackGenNetworkOutputPowerWithRpg(nums);
			break;
		case KEY_FUNC_SOURCE_TRACKGEN_VSWR_REFERENCE:
			sysScpi->setSourceTrackGenVSWRRefLevelWithRpg(nums);
			break;
		case KEY_FUNC_SOURCE_SIGNALGEN_FREQ:
			sysScpi->setSourceSignalGenOutputFreqWithRpg(nums);
			break;
		case KEY_FUNC_SOURCE_SIGNALGEN_POWER:
			sysScpi->setSourceSignalGenOutputPowerWithRpg(nums);
			break;

		case KEY_FUNC_DEMOD_SOUND:
			sysScpi->setDemodOfSoundWithRpg(nums);
			break;

		case KEY_FUNC_AMPT_PASSFAIL_WINDOW_LIMITUPLOW:
			sysScpi->setMeasureOfPassFailOfWinLineValueWithRpg(nums);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_WINDOW_FREQSTARTSTOP:
			sysScpi->setMeasureOfPassFailOfWinFreqValueWithRpg(nums);
			break;

	}
}

//处理执行键
void mainForm::handleExecuteKey(int funcCode)
{
	QString str = "";
	long int code = 0;
	char temp[32] = { };
	long int result = 0;
	double tempValue = 0;

	switch (funcCode)
	{
		//trig mode
		case KEY_FUNC_TRIG_AUTORUN:
			sysScpi->setTrigMode(tgRun);
			break;
		case KEY_FUNC_TRIG_VIDEO:
			sysScpi->setTrigMode(tgVideo);
			break;
		case KEY_FUNC_TRIG_EXTERNAL_RISE:
			sysScpi->setTrigMode(tgRise);
			break;
		case KEY_FUNC_TRIG_EXTERNAL_FALL:
			sysScpi->setTrigMode(tgFall);
			break;

			//demod mode
		case KEY_FUNC_MODE_OPTIONS_DEMOD_DEMODMODE_FMW:
			sysScpi->setDemodOfMode(dmFMW);
			break;
		case KEY_FUNC_MODE_OPTIONS_DEMOD_DEMODMODE_FM:
			sysScpi->setDemodOfMode(dmFM);
			//     sysData.mode = smFM;
			//     sysScpi->checkMultiDisplay(mdtFM);
			break;
		case KEY_FUNC_MODE_OPTIONS_DEMOD_DEMODMODE_AM:
			sysScpi->setDemodOfMode(dmAM);
			//     sysData.mode = smAM;
			//     sysScpi->checkMultiDisplay(mdtAM);
			break;
		case KEY_FUNC_MODE_OPTIONS_DEMOD_DEMODMODE_USB:
			sysScpi->setDemodOfMode(dmUSB);
			break;
		case KEY_FUNC_MODE_OPTIONS_DEMOD_DEMODMODE_LSB:
			sysScpi->setDemodOfMode(dmLSB);
			break;
		case KEY_FUNC_SYSTEM_SERVICE_CLEAR_LOGDATA:
			str = QCoreApplication::applicationDirPath() + "/log.txt";
			if (QFile(str).exists())
			{
				QFile(str).remove();
				sysScpi->outputToScreen(getTheTranslatorString("cleared log data"), "");
				usleep(1000 * 500);
			}
			break;
		case KEY_FUNC_DISPLAY_FULLSCREEN:
			sysScpi->setDisplayOfFullScreen(1);
			drawAll();
			break;

		case KEY_FUNC_SPAN_FULL:
			sysScpi->setFrequencyOfSpan(MAXSPAN);
			break;
		case KEY_FUNC_SPAN_ZERO:
			sysScpi->setFrequencyOfSpan(0);
			break;
		case KEY_FUNC_SPAN_LAST:
			sysScpi->setFrequencyOfSpan(sysData.span.prvSpan);
			break;

		case KEY_FUNC_AMPT_REFUNIT_DBM:
			sysScpi->setAmptOfRefUnit(rudBm);
			break;
		case KEY_FUNC_AMPT_REFUNIT_DBMV:
			sysScpi->setAmptOfRefUnit(rudBmV);
			break;
		case KEY_FUNC_AMPT_REFUNIT_DBUV:
			sysScpi->setAmptOfRefUnit(rudBuV);
			break;
		case KEY_FUNC_AMPT_REFUNIT_W:
			sysScpi->setAmptOfRefUnit(ruW);
			break;
		case KEY_FUNC_AMPT_REFUNIT_V:
			sysScpi->setAmptOfRefUnit(ruV);
			break;
		case KEY_FUNC_AMPT_REFUNIT_DBuW:
			sysScpi->setAmptOfRefUnit(rudBuW);
			break;
		case KEY_FUNC_AMPT_REFUNIT_DBPW:
			sysScpi->setAmptOfRefUnit(rudBpW);
			break;

		case KEY_FUNC_BW_EMC_1MHZ:
			if (sysData.emc.emcOn)
			{
				sysScpi->setBwOfRbw(1e6);
			}
			break;
		case KEY_FUNC_BW_EMC_120KHZ:
			if (sysData.emc.emcOn)
			{
				sysScpi->setBwOfRbw(120e3);
			}
			break;
		case KEY_FUNC_BW_EMC_30KHZ:
			if (sysData.emc.emcOn)
			{
				sysScpi->setBwOfRbw(30e3);
			}
			break;
		case KEY_FUNC_BW_EMC_9KHZ:
			if (sysData.emc.emcOn)
			{
				sysScpi->setBwOfRbw(9e3);
			}
			break;
		case KEY_FUNC_BW_EMC_200HZ:
			if (sysData.emc.emcOn)
			{
				sysScpi->setBwOfRbw(200);
			}
			break;

		case KEY_FUNC_SWEEP_SWEEPMODE_SINGLE:
			sysScpi->setSweepOfMode(0);
			break;
		case KEY_FUNC_SWEEP_SWEEPMODE_CONTINUOUS:
			sysScpi->setSweepOfMode(1);
			break;

		case KEY_FUNC_TRACE_CLEARWRITE:
			sysScpi->setTraceMode(sysData.traceSelIndex, tsClearWrite);
			break;
		case KEY_FUNC_TRACE_MAXHOLD:
			sysScpi->setTraceMode(sysData.traceSelIndex, tsMaxHold);
			break;
		case KEY_FUNC_TRACE_MINHOLD:
			sysScpi->setTraceMode(sysData.traceSelIndex, tsMinHold);
			break;
		case KEY_FUNC_TRACE_VIEW:
			sysScpi->setTraceMode(sysData.traceSelIndex, tsView);
			break;
		case KEY_FUNC_TRACE_BLANK:
			sysScpi->setTraceMode(sysData.traceSelIndex, tsBlank);
			break;

		case KEY_FUNC_TRACE_OPERATIONS_12:
			sysScpi->setTraceOperationsOfExchange(0, 1);
			break;
		case KEY_FUNC_TRACE_OPERATIONS_2DLTO2:
			sysScpi->setTraceOperationsOfDisplayLine(1);
			break;
		case KEY_FUNC_TRACE_OPERATIONS_23:
			sysScpi->setTraceOperationsOfExchange(1, 2);
			break;
		case KEY_FUNC_TRACE_OPERATIONS_1TO3:
			sysScpi->setTraceOperationsOfChange(0, 2);
			break;
		case KEY_FUNC_TRACE_OPERATIONS_2TO3:
			sysScpi->setTraceOperationsOfChange(1, 2);
			break;

		case KEY_FUNC_DETECTOR_AUTO:
			sysScpi->setDetector(dmAuto);
			break;
		case KEY_FUNC_DETECTOR_NORMAL:
			sysScpi->setDetector(dmNormal);
			break;
		case KEY_FUNC_DETECTOR_POSPEAK:
			sysScpi->setDetector(dmPosPeak);
			break;
		case KEY_FUNC_DETECTOR_NEGPEAK:
			sysScpi->setDetector(dmNegPeak);
			break;
		case KEY_FUNC_DETECTOR_SAMPLE:
			sysScpi->setDetector(dmSample);
			break;
		case KEY_FUNC_DETECTOR_TRANSIENT:
			sysScpi->setDetector(dmTransient);
			break;

		case KEY_FUNC_MEAS_MEASUREOFF:
			sysScpi->setMeasureAllOff();
			break;
		case KEY_FUNC_MEAS_DISCRIMINATOR_AMPLITUDE_INPUTGAIN_X1:
			sysScpi->setMeasureOfDiscInputGain(igtx1);
			break;
		case KEY_FUNC_MEAS_DISCRIMINATOR_AMPLITUDE_INPUTGAIN_X2:
			sysScpi->setMeasureOfDiscInputGain(igtx2);
			break;
		case KEY_FUNC_MEAS_DISCRIMINATOR_AMPLITUDE_INPUTGAIN_X4:
			sysScpi->setMeasureOfDiscInputGain(igtx4);
			break;
		case KEY_FUNC_MEAS_DISCRIMINATOR_AMPLITUDE_INPUTGAIN_X05:
			sysScpi->setMeasureOfDiscInputGain(igtx05);
			break;
		case KEY_FUNC_MEAS_DISCRIMINATOR_AMPLITUDE_INPUTGAIN_X025:
			sysScpi->setMeasureOfDiscInputGain(igtx025);
			break;
		case KEY_FUNC_MEAS_DISCRIMINATOR_NORMALIZE:
			if (sysScpi->setMeasureOfDiscNormalize() == __SCPI_SUCCESS)
			{
				helpContent = getTheTranslatorString("discriminator calibrate success");
				sysScpi->setMeasureOfDiscInputGain(0);
				sysScpi->setMeasureOfDiscState(true);
			} else
			{
				sysScpi->setMeasureOfDiscState(false);
				helpContent = getTheTranslatorString("discriminator calibrate failed");
			}
			drawHelp();
			break;

		case KEY_FUNC_DISPLAY_STYLE_DEFAULT:
			sysScpi->setDisplayOfStyle(dsDefault);
			break;
		case KEY_FUNC_DISPLAY_STYLE_STYLE1:
			sysScpi->setDisplayOfStyle(dsWinXP);
			break;

		case KEY_FUNC_MARKERTO_CENTER:
			sysScpi->setMarkerToCenterFreq(sysData.markerSelIndex);
			resetMarkerToLogic();
			break;
		case KEY_FUNC_MARKERTO_CFSTEP:
			sysScpi->setMarkerToCenterFreqStep(sysData.markerSelIndex);
			resetMarkerToLogic();
			break;
		case KEY_FUNC_MARKERTO_START:
			sysScpi->setMarkerToStartFreq(sysData.markerSelIndex);
			resetMarkerToLogic();
			break;
		case KEY_FUNC_MARKERTO_STOP:
			sysScpi->setMarkerToStopFreq(sysData.markerSelIndex);
			resetMarkerToLogic();
			break;
		case KEY_FUNC_MARKERTO_REFLEVEL:
			sysScpi->setMarkerToRefLevel(sysData.markerSelIndex);
			resetMarkerToLogic();
			break;
		case KEY_FUNC_MARKERTO_DELTATOSPAN:
			sysScpi->setMarkerDeltaToSpan(sysData.markerSelIndex);
			resetMarkerToLogic();
			break;
		case KEY_FUNC_MARKERTO_DELTATOCF:
			sysScpi->setMarkerDeltaToCenterFreq(sysData.markerSelIndex);
			resetMarkerToLogic();
			break;

		case KEY_FUNC_MARKERFCTN_OFF:
			sysScpi->setMarkerFunctionAllOff();
			break;

		case KEY_FUNC_PEAK_MAXSEARCH:
			sysScpi->setMarkerPeakMax(sysData.markerSelIndex, sysData.marker[sysData.markerSelIndex].traceIndex);
			break;
		case KEY_FUNC_PEAK_NEXTPEAK:
			sysScpi->setMarkerPeakNext(sysData.markerSelIndex, sysData.marker[sysData.markerSelIndex].traceIndex);
			break;
		case KEY_FUNC_PEAK_LEFTPEAK:
			sysScpi->setMarkerPeakLeft(sysData.markerSelIndex, sysData.marker[sysData.markerSelIndex].traceIndex);
			break;
		case KEY_FUNC_PEAK_RIGHTPEAK:
			sysScpi->setMarkerPeakRight(sysData.markerSelIndex, sysData.marker[sysData.markerSelIndex].traceIndex);
			break;
		case KEY_FUNC_PEAK_MINSEARCH:
			sysScpi->setMarkerPeakMin(sysData.markerSelIndex, sysData.marker[sysData.markerSelIndex].traceIndex);
			break;
		case KEY_FUNC_PEAK_MKRTOCF:
			sysScpi->setMarkerPeakToCenterFreq(sysData.markerSelIndex, sysData.marker[sysData.markerSelIndex].traceIndex);
			break;

		case KEY_FUNC_SYSTEM_LANGUAGE_CHINESE:
			sysScpi->setSystemLanguage(0);
			drawAll();
			helpContent = getTheTranslatorString("Ready");
			drawHelp();
			break;
		case KEY_FUNC_SYSTEM_LANGUAGE_ENGLISH:
			sysScpi->setSystemLanguage(1);
			drawAll();
			helpContent = getTheTranslatorString("Ready");
			drawHelp();
			break;
		case KEY_FUNC_SYSTEM_SERVICE_CALIBRATION_EDIT_9K_RESET:
			sysScpi->resetFrequencyRespondOf9kHz(RESP9KHZ);
			break;
		case KEY_FUNC_SYSTEM_SERVICE_CALIBRATION_EDIT_20K_RESET:
			sysScpi->resetFrequencyRespondOf20kHz(RESP20KHZ);
			break;

		case KEY_FUNC_SOURCE_TRACKGEN_MEASTYPE_S21:
			sysScpi->setSourceTrackGenNetworkMeasType(S21);
			break;
		case KEY_FUNC_SOURCE_TRACKGEN_MEASTYPE_S11:
			sysScpi->setSourceTrackGenNetworkMeasType(S11);
			break;
		case KEY_FUNC_SOURCE_TRACKGEN_NETWORK_NORMALIZE:
			sysScpi->setSourceTrackGenNetworkNormalize();
			break;

		case KEY_FUNC_FILE_REFRESH:
			sysScpi->getSystemFileList();
			break;
		case KEY_FUNC_FILE_SHOW_SCREENS:
			sysData.file.show = stScreenPng;
			sysScpi->getSystemFileList();
			break;
		case KEY_FUNC_FILE_SHOW_TRACEDATAS:
			sysData.file.show = stTraceData;
			sysScpi->getSystemFileList();
			break;
		case KEY_FUNC_FILE_SHOW_ALL:
			sysData.file.show = stAll;
			sysScpi->getSystemFileList();
			break;
		case KEY_FUNC_FILE_FIRSTPAGE:
			if ((sysData.file.total > 0) && (sysData.file.nowPageIndex > 0))
			{
				sysData.file.nowPageIndex = 0;
				sysData.file.nowPageCount = sysData.file.perPage;
			}
			if (sysData.file.nowSelIndex < 0)
			{
				sysData.file.nowSelIndex = 0;
			} else if (sysData.file.nowSelIndex >= sysData.file.nowPageCount)
			{
				sysData.file.nowSelIndex = sysData.file.nowPageCount - 1;
			}
			break;
		case KEY_FUNC_FILE_PREVPAGE:
			if ((sysData.file.total > 0) && (sysData.file.nowPageIndex > 0))
			{
				sysData.file.nowPageIndex--;
				sysData.file.nowPageCount = sysData.file.perPage;
			}
			if (sysData.file.nowSelIndex < 0)
			{
				sysData.file.nowSelIndex = 0;
			} else if (sysData.file.nowSelIndex >= sysData.file.nowPageCount)
			{
				sysData.file.nowSelIndex = sysData.file.nowPageCount - 1;
			}
			break;
		case KEY_FUNC_FILE_NEXTPAGE:
			if ((sysData.file.total > 0) && (sysData.file.nowPageIndex < sysData.file.pages - 1))
			{
				sysData.file.nowPageIndex++;

				if (sysData.file.nowPageIndex == sysData.file.pages - 1)
				{
					sysData.file.nowPageCount = sysData.file.total - sysData.file.nowPageIndex * sysData.file.perPage;
				}
			}
			if (sysData.file.nowSelIndex < 0)
			{
				sysData.file.nowSelIndex = 0;
			} else if (sysData.file.nowSelIndex >= sysData.file.nowPageCount)
			{
				sysData.file.nowSelIndex = sysData.file.nowPageCount - 1;
			}
			break;
		case KEY_FUNC_FILE_LASTPAGE:
			if ((sysData.file.total > 0) && (sysData.file.nowPageIndex < sysData.file.pages - 1))
			{
				sysData.file.nowPageIndex = sysData.file.pages - 1;
				sysData.file.nowPageCount = sysData.file.total - sysData.file.nowPageIndex * sysData.file.perPage;
			}
			if (sysData.file.nowSelIndex < 0)
			{
				sysData.file.nowSelIndex = 0;
			} else if (sysData.file.nowSelIndex >= sysData.file.nowPageCount)
			{
				sysData.file.nowSelIndex = sysData.file.nowPageCount - 1;
			}
			break;
		case KEY_FUNC_FILE_OPERATIONS_SORT_NAME:
			if (sysData.file.sort != stName)
			{
				sysData.file.sort = stName;
				sysScpi->getSystemFileList();
			}
			break;
		case KEY_FUNC_FILE_OPERATIONS_SORT_DATETIME:
			if (sysData.file.sort != stTime)
			{
				sysData.file.sort = stTime;
				sysScpi->getSystemFileList();
			}
			break;
		case KEY_FUNC_FILE_OPERATIONS_SORT_SIZE:
			if (sysData.file.sort != stSize)
			{
				sysData.file.sort = stSize;
				sysScpi->getSystemFileList();
			}
			break;
		case KEY_FUNC_FILE_OPERATIONS_LOAD:
			if (sysData.file.nowPageCount > 0)
			{
				sysData.file.isRestoring = true;

				if (sysScpi->fileList.at(sysScpi->getFileOfNowSelectIndex()).suffix().toUpper() == "PNG")
				{
					sysScpi->fileSelectName = sysScpi->fileList.at(sysScpi->getFileOfNowSelectIndex()).absoluteFilePath();
				} else
				{
					sysScpi->fileSelectName = sysScpi->fileList.at(sysScpi->getFileOfNowSelectIndex()).absolutePath() + "/trace/" + sysScpi->fileList.at(sysScpi->getFileOfNowSelectIndex()).baseName() + ".png";
				}

				if (QFile(sysScpi->fileSelectName).exists())
				{
					sysScpi->fileSelectPixmap = QPixmap(sysScpi->fileSelectName).copy(QRect(0, TOPHEIGHT, SCREENWIDTH * CANVASWIDTH, SCREENHEIGHT - TOPHEIGHT - BOTTOMHEIGHT));
				}
			}
			break;
		case KEY_FUNC_FILE_OPERATIONS_DELETE_SELECT:
			if (sysScpi->setFileOfDeleteTheSelect() == __SCPI_SUCCESS)
			{
				sysScpi->outputToScreen(getTheTranslatorString("file delete success") + "......", 0);
				helpContent = getTheTranslatorString("file delete success");
			} else
			{
				sysScpi->showRunAlarm(getTheTranslatorString("file delete failed"));
				helpContent = getTheTranslatorString("file delete failed");
			}
			sleep(PROMPTTIME);
			drawHelp();
			break;
		case KEY_FUNC_FILE_OPERATIONS_DELETE_PAGE:
			if (sysScpi->setFileOfDeleteThePage() == __SCPI_SUCCESS)
			{
				sysScpi->outputToScreen(getTheTranslatorString("file delete success"), 0);
				helpContent = getTheTranslatorString("file delete success");
			} else
			{
				sysScpi->showRunAlarm(getTheTranslatorString("file delete failed"));
				helpContent = getTheTranslatorString("file delete failed");
			}
			sleep(PROMPTTIME);
			drawHelp();
			break;
		case KEY_FUNC_FILE_OPERATIONS_DELETE_ALL:
			if (sysScpi->setFileOfDeleteAll() == __SCPI_SUCCESS)
			{
				sysScpi->outputToScreen(getTheTranslatorString("file delete success"), 0);
				helpContent = getTheTranslatorString("file delete success");
			} else
			{
				sysScpi->showRunAlarm(getTheTranslatorString("file delete failed"));
				helpContent = getTheTranslatorString("file delete failed");
			}
			sleep(PROMPTTIME);
			drawHelp();
			break;
		case KEY_FUNC_FILE_OPERATIONS_COPY_SELECT_TOUSB:
			sysLocked = true;
			if (sysScpi->setFileOfExportTheSelect() == __SCPI_SUCCESS)
			{
				sysScpi->outputToScreen(getTheTranslatorString("file delete success"), 0);
				helpContent = getTheTranslatorString("file export success");
			} else
			{
				sysScpi->showRunAlarm(getTheTranslatorString("file export failed"));
				helpContent = getTheTranslatorString("file export failed");
			}
			sleep(PROMPTTIME);
			drawHelp();
			sysLocked = false;
			break;
		case KEY_FUNC_FILE_OPERATIONS_COPY_PAGE_TOUSB:
			sysLocked = true;
			if (sysScpi->setFileOfExportThePage() == __SCPI_SUCCESS)
			{
				sysScpi->outputToScreen(getTheTranslatorString("file delete success"), 0);
				helpContent = getTheTranslatorString("file export success");
			} else
			{
				sysScpi->showRunAlarm(getTheTranslatorString("file export failed"));
				helpContent = getTheTranslatorString("file export failed");
			}
			sleep(PROMPTTIME);
			drawHelp();
			sysLocked = false;
			break;
		case KEY_FUNC_FILE_OPERATIONS_COPY_ALL_TOUSB:
			sysLocked = true;
			if (sysScpi->setFileOfExportAll() == __SCPI_SUCCESS)
			{
				sysScpi->outputToScreen(getTheTranslatorString("file export success"), 0);
				helpContent = getTheTranslatorString("file export success");
			} else
			{
				sysScpi->showRunAlarm(getTheTranslatorString("file export failed"));
				helpContent = getTheTranslatorString("file export failed");
			}
			sleep(PROMPTTIME);
			drawHelp();
			sysLocked = false;
			break;

		case KEY_FUNC_SAVE_SCEEEN_TOLOCAL:
			if (sysScpi->getFreeDiskSpace() <= 2 * 1024 * 1024)
			{
				sysScpi->showRunAlarm(getTheTranslatorString("Free Disk Space is not enough!"));
				outputMessage(getTheTranslatorString("Free Disk Space is not enough!"));
			} else
			{
				outputMessage(getTheTranslatorString("saving file......"));
				str = getDateTimeFileName(temp);
				code = sysScpi->saveScreenPixmapToLocal(str);

				if (code == _RESULT_SUCCESS)
				{
					sysScpi->outputToScreen(str + ".png " + getTheTranslatorString("saved"), 0);
					outputMessage(str + ".png " + getTheTranslatorString("saved"));
				} else
				{
					sysScpi->showRunAlarm(getErrorMessage(code));
					outputMessage((QString)("[") + intToQString(code, temp) + (QString)("] ") + str + (QString)(".png ") + getTheTranslatorString("save failed") + ", " + getErrorMessage(code));
				}
			}
			sleep(PROMPTTIME);
			break;
		case KEY_FUNC_SAVE_SCEEEN_TOUSB:
			sysLocked = true;
			outputMessage(getTheTranslatorString("saving file......"));
			str = getDateTimeFileName(temp);
			code = sysScpi->saveScreenPixmapToUsb(str);

			if (code == _RESULT_SUCCESS)
			{
				sysScpi->outputToScreen(str + ".png " + getTheTranslatorString("saved"), 0);
				outputMessage(str + ".png " + getTheTranslatorString("saved"));
			} else
			{
				sysScpi->showRunAlarm(getErrorMessage(code));
				usleep(FORMCLOSETIME);
				outputMessage((QString)("[") + intToQString(code, temp) + (QString)("] ") + str + (QString)(".png ") + getTheTranslatorString("save failed") + ", " + getErrorMessage(code));
			}
			sleep(PROMPTTIME);
			sysLocked = false;
			break;
		case KEY_FUNC_SAVE_TRACE_TOLOCAL:
			sysLocked = true;
			if (sysScpi->getFreeDiskSpace() <= 2 * 1024 * 1024)
			{
				sysScpi->showRunAlarm(getTheTranslatorString("Free Disk Space is not enough!"));
				outputMessage(getTheTranslatorString("Free Disk Space is not enough!"));
			} else
			{
				outputMessage(getTheTranslatorString("saving file......"));
				str = getDateTimeFileName(temp);
				code = sysScpi->saveTraceDataToLocal(str);

				if (code == _RESULT_SUCCESS)
				{
					sysScpi->outputToScreen(str + ".csv " + getTheTranslatorString("saved"), 0);
					outputMessage(str + ".csv " + getTheTranslatorString("saved"));
				} else
				{
					sysScpi->showRunAlarm(getErrorMessage(code));
					outputMessage((QString)("[") + intToQString(code, temp) + (QString)("] ") + str + (QString)(".csv ") + getTheTranslatorString("save failed") + ", " + getErrorMessage(code));
				}
			}
			sleep(PROMPTTIME);
			sysLocked = false;
			break;
		case KEY_FUNC_SAVE_TRACE_TOUSB:
			sysLocked = true;
			outputMessage(getTheTranslatorString("saving file......"));
			str = getDateTimeFileName(temp);
			code = sysScpi->saveTraceDataToUsb(str);

			if (code == _RESULT_SUCCESS)
			{
				sysScpi->outputToScreen(str + ".csv " + getTheTranslatorString("saved"), 0);
				outputMessage(str + ".csv " + getTheTranslatorString("saved"));
			} else
			{
				sysScpi->showRunAlarm(getTheTranslatorString("export to usb disk failed"));
				usleep(FORMCLOSETIME);
				outputMessage((QString)("[") + intToQString(code, temp) + (QString)("] ") + str + (QString)(".csv ") + getTheTranslatorString("save failed") + ", " + getErrorMessage(code));
			}
			sleep(PROMPTTIME);
			sysLocked = false;
			break;
		case KEY_FUNC_SAVE_USER:
			if (sysScpi->getFreeDiskSpace() <= 2 * sizeof(sysData))
			{
				outputMessage(getTheTranslatorString("Free Disk Space is not enough!"));
			} else
			{
				outputMessage(getTheTranslatorString("saving user state") + "......");
				code = sysScpi->saveUserState();

				if (code == _RESULT_SUCCESS)
				{
					sysScpi->outputToScreen(getTheTranslatorString("user state saved"), 0);
					outputMessage(getTheTranslatorString("user state saved"));
				} else
				{
					sysScpi->showRunAlarm(getErrorMessage(code));
					outputMessage((QString)("[") + intToQString(code, temp) + (QString)("] ") + getTheTranslatorString("save user state failed") + ", " + getErrorMessage(code));
				}
			}
			sleep(PROMPTTIME);
			break;

		case KEY_FUNC_SYSTEM_SYSTEM_INFO:
			sysData.mode = smSystemInfo;
			sysData.canvasNums = 1;
			sysScpi->checkMultiDisplay(mdtSystemInfo);
			break;
		case KEY_FUNC_SYSTEM_SYSTEM_LOG:
			logList.clear();
			logList = sysScpi->loadLogOfFactoryCalibrate();
			sysData.mode = smSystemLog;
			sysData.canvasNums = 1;
			sysScpi->checkMultiDisplay(mdtSystemLog);
			break;

		case KEY_FUNC_SYSTEM_CONFIGIO_COM_BAUDRATE_1200:
			sysScpi->setSystemSerialOfBaudrate(br1200);
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_COM_BAUDRATE_2400:
			sysScpi->setSystemSerialOfBaudrate(br2400);
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_COM_BAUDRATE_4800:
			sysScpi->setSystemSerialOfBaudrate(br4800);
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_COM_BAUDRATE_9600:
			sysScpi->setSystemSerialOfBaudrate(br9600);
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_COM_BAUDRATE_19200:
			sysScpi->setSystemSerialOfBaudrate(br19200);
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_COM_BAUDRATE_38400:
			sysScpi->setSystemSerialOfBaudrate(br38400);
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_COM_BAUDRATE_57600:
			sysScpi->setSystemSerialOfBaudrate(br57600);
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_COM_BAUDRATE_115200:
			sysScpi->setSystemSerialOfBaudrate(br115200);
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_COM_DATABITS_5:
			sysScpi->setSystemSerialOfDatabits(db5);
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_COM_DATABITS_6:
			sysScpi->setSystemSerialOfDatabits(db6);
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_COM_DATABITS_7:
			sysScpi->setSystemSerialOfDatabits(db7);
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_COM_DATABITS_8:
			sysScpi->setSystemSerialOfDatabits(db8);
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_COM_STOPBITS_1:
			sysScpi->setSystemSerialOfStopbits(sb1);
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_COM_STOPBITS_2:
			sysScpi->setSystemSerialOfStopbits(sb2);
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_COM_CHECK_EVEN:
			sysScpi->setSystemSerialOfCheck(cmEven);
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_COM_CHECK_ODD:
			sysScpi->setSystemSerialOfCheck(cmOdd);
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_COM_CHECK_NONE:
			sysScpi->setSystemSerialOfCheck(cmNone);
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_COM_CHECK_SPACE:
			sysScpi->setSystemSerialOfCheck(cmSpace);
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_COM_FLOWCONTROL_XONXOFF:
			sysScpi->setSystemSerialOfFlowControl(fcXonXoff);
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_COM_FLOWCONTROL_HARDWARE:
			sysScpi->setSystemSerialOfFlowControl(fcHardware);
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_COM_FLOWCONTROL_NONE:
			sysScpi->setSystemSerialOfFlowControl(fcNone);
			break;

		case KEY_FUNC_SYSTEM_CONFIGIO_LAN_RESET:
			sysScpi->resetSystemIOOfLan();
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_COM_RESET:
			sysScpi->resetSystemIOOfSerial();
			break;

		case KEY_FUNC_SYSTEM_POWERONPRESET_POWERON_FACTORY:
			sysScpi->setSystemPoweronParam(spFactory);
			break;
		case KEY_FUNC_SYSTEM_POWERONPRESET_POWERON_USER:
			sysScpi->setSystemPoweronParam(spUser);
			break;
		case KEY_FUNC_SYSTEM_POWERONPRESET_POWERON_POWEROFF:
			sysScpi->setSystemPoweronParam(spPowerOff);
			break;
		case KEY_FUNC_SYSTEM_POWERONPRESET_PRESET_FACTORY:
			sysScpi->setSystemPresetParam(spFactory);
			break;
		case KEY_FUNC_SYSTEM_POWERONPRESET_PRESET_USER:
			sysScpi->setSystemPresetParam(spUser);
			break;
		case KEY_FUNC_SYSTEM_POWERONPRESET_PRESET_POWEROFF:
			sysScpi->setSystemPresetParam(spPowerOff);
			break;

		case KEY_FUNC_SYSTEM_DATETIME_FORMAT_YMDHMS:
			sysScpi->setSystemDateFormat(dtsYMDHMS);
			drawTop();
			break;
		case KEY_FUNC_SYSTEM_DATETIME_FORMAT_HMSYMD:
			sysScpi->setSystemDateFormat(dtsHMSYMD);
			drawTop();
			break;

		case KEY_FUNC_PRINTSETUP_PAPER_A4:
			sysData.print.paper = ptA4;
			break;
		case KEY_FUNC_PRINTSETUP_PAPER_A3:
			sysData.print.paper = ptA3;
			break;
		case KEY_FUNC_PRINTSETUP_PAPER_B5:
			sysData.print.paper = ptB5;
			break;
		case KEY_FUNC_PRINTSETUP_PAPER_C5:
			sysData.print.paper = ptC5;
			break;
		case KEY_FUNC_PRINTSETUP_PAPER_LETTER:
			sysData.print.paper = ptLetter;
			break;

		case KEY_FUNC_PRINTSETUP_PRINTTRACE:
			sysLocked = true;
			outputMessage(getTheTranslatorString("printing trace......"));
			code = sysScpi->execPrintTrace();

			if (code == _RESULT_SUCCESS)
			{
				sysScpi->outputToScreen(getTheTranslatorString("print trace finished") + "......", 0);
				outputMessage(getTheTranslatorString("print trace finished"));
			} else
			{
				sysScpi->showRunAlarm(getTheTranslatorString("print trace failed") + ", " + getErrorMessage(code));
				outputMessage((QString)("[") + intToQString(code, temp) + (QString)("] ") + getTheTranslatorString("print trace failed") + ", " + getErrorMessage(code));
			}
			sleep(PROMPTTIME);
			sysLocked = false;
			break;
		case KEY_FUNC_PRINTSETUP_PRINTSCREEN:
			sysLocked = true;
			outputMessage(getTheTranslatorString("printing screen pixmap......"));
			code = sysScpi->execPrintScreen();

			if (code == _RESULT_SUCCESS)
			{
				sysScpi->outputToScreen(getTheTranslatorString("print screen pixmap finished") + "......", 0);
				outputMessage(getTheTranslatorString("print screen pixmap finished"));
			} else
			{
				sysScpi->showRunAlarm(getTheTranslatorString("print screen pixmap failed") + ", " + getErrorMessage(code));
				outputMessage((QString)("[") + intToQString(code, temp) + (QString)("] ") + getTheTranslatorString("print screen pixmap failed") + ", " + getErrorMessage(code));
			}
			sleep(PROMPTTIME);
			sysLocked = false;
			break;

		case KEY_FUNC_MARKER_NORMAL:
			sysScpi->setMarkerNormal(sysData.markerSelIndex, sysData.marker[sysData.markerSelIndex].traceIndex);
			break;
		case KEY_FUNC_MARKER_DELTA:
			sysScpi->setMarkerDelta(sysData.markerSelIndex, sysData.marker[sysData.markerSelIndex].traceIndex);
			break;
		case KEY_FUNC_MARKER_OFF:
			sysData.marker[sysData.markerSelIndex].state = msClose;
			break;
		case KEY_FUNC_MARKER_ALLOFF:
			sysScpi->setMarkerAllOff();
			refreshMarkerState();
			break;

		case KEY_FUNC_SYSTEM_SERVICE_CALIBRATION_CALIBRATION:
			if ((sysScpi->factoryCalibrate(sysData.isComCalbration) == __SCPI_SUCCESS) && (sysScpi->PreamplifierCalibrate(sysData.isComCalbration) == __SCPI_SUCCESS))
			{
				presetSystem(false);
			} else
			{
				drawAll();
			}
			break;

		case KEY_FUNC_SYSTEM_SERVICE_CALIBRATION_PRECALIBRATION:
			if (sysScpi->PreamplifierCalibrate(sysData.isComCalbration) == __SCPI_SUCCESS)
			{
				presetSystem(false);
			} else
			{
				drawAll();
			}
			break;
		case KEY_FUNC_SYSTEM_SERVICE_CALIBRATION_LFCALIBRATION:
			if ((sysScpi->ZCCalibrate(sysData.isComCalbration) == __SCPI_SUCCESS)
					&&
					(sysScpi->ZCPreamplifierCalibrate(sysData.isComCalbration) == __SCPI_SUCCESS)
					)
			{
				presetSystem(false);
			} else
			{
				drawAll();
			}
			break;
		case KEY_FUNC_SYSTEM_SERVICE_CALIBRATION_DELETE:
			sysLocked = true;
			sysScpi->clearFactoryCalibrateData();
			sysScpi->presetSystemData();
			sysScpi->outputToScreen(getTheTranslatorString("cleared calibrate data"), 0);
			sleep(PROMPTTIME);
			sysLocked = false;
			break;
		case KEY_FUNC_SYSTEM_CALIBRATION_CALIBRATION:
			if (sysScpi->userCalibrate() == __SCPI_SUCCESS)
			{
				presetSystem(false);
			} else
			{
				drawAll();
			}
			break;
		case KEY_FUNC_SYSTEM_CALIBRATION_FACTORY:
			sysScpi->clearUserCalibrateData();
			sysScpi->presetSystemData();
			break;

		case KEY_FUNC_DEMOD_RADIOSET_1:
			sysScpi->setDemodModeOfFreq(radio1);
			break;
		case KEY_FUNC_DEMOD_RADIOSET_MUSIC:
			sysScpi->setDemodModeOfFreq(radio2);
			break;
		case KEY_FUNC_DEMOD_RADIOSET_TRAFFIC:
			sysScpi->setDemodModeOfFreq(radio3);
			break;
		case KEY_FUNC_DEMOD_RADIOSET_2:
			sysScpi->setDemodModeOfFreq(radio4);
			break;
		case KEY_FUNC_DEMOD_RADIOSET_3:
			sysScpi->setDemodModeOfFreq(radio5);
			break;
		case KEY_FUNC_DEMOD_RADIOSET_4:
			sysScpi->setDemodModeOfFreq(radio6);
			break;

			///
		case KEY_FUNC_SAVE_CLEAR_ALL:
			sysScpi->clearAllUserSelfState();
			break;
		case KEY_FUNC_SAVE_CLEAR_USER1:
			sysScpi->clearUserSelfState(0);
			break;
		case KEY_FUNC_SAVE_CLEAR_USER2:
			sysScpi->clearUserSelfState(1);
			break;
		case KEY_FUNC_SAVE_CLEAR_USER3:
			sysScpi->clearUserSelfState(2);
			break;
		case KEY_FUNC_SAVE_CLEAR_USER4:
			sysScpi->clearUserSelfState(3);
			break;
		case KEY_FUNC_SAVE_CLEAR_USER5:
			sysScpi->clearUserSelfState(4);
			break;
		case KEY_FUNC_SAVE_CLEAR_USER6:
			sysScpi->clearUserSelfState(5);
			break;
		case KEY_FUNC_SAVE_CLEAR_USER7:
			sysScpi->clearUserSelfState(6);
			break;
		case KEY_FUNC_SAVE_CLEAR_USER8:
			sysScpi->clearUserSelfState(7);
			break;
		case KEY_FUNC_SAVE_CLEAR_USER9:
			sysScpi->clearUserSelfState(8);
			break;
		case KEY_FUNC_SAVE_CLEAR_USER10:
			sysScpi->clearUserSelfState(9);
			break;
		case KEY_FUNC_SAVE_LOAD_USER1:
			result = sysScpi->loadUserSelfState(0);

			if (nowSubKey != NULL && result == __SCPI_SUCCESS)
			{
				outputMessage(getTranslatorString(nowSubKey->enName, nowSubKey->chName) + getTheTranslatorString("success loaded"));
			}
			break;
		case KEY_FUNC_SAVE_LOAD_USER2:
			result = sysScpi->loadUserSelfState(1);

			if (nowSubKey != NULL && result == __SCPI_SUCCESS)
			{
				outputMessage(getTranslatorString(nowSubKey->enName, nowSubKey->chName) + getTheTranslatorString("success loaded"));
			}
			break;
		case KEY_FUNC_SAVE_LOAD_USER3:
			result = sysScpi->loadUserSelfState(2);

			if (nowSubKey != NULL && result == __SCPI_SUCCESS)
			{
				outputMessage(getTranslatorString(nowSubKey->enName, nowSubKey->chName) + getTheTranslatorString("success loaded"));
			}
			break;
		case KEY_FUNC_SAVE_LOAD_USER4:
			result = sysScpi->loadUserSelfState(3);

			if (nowSubKey != NULL && result == __SCPI_SUCCESS)
			{
				outputMessage(getTranslatorString(nowSubKey->enName, nowSubKey->chName) + getTheTranslatorString("success loaded"));
			}
			break;
		case KEY_FUNC_SAVE_LOAD_USER5:
			result = sysScpi->loadUserSelfState(4);

			if (nowSubKey != NULL && result == __SCPI_SUCCESS)
			{
				outputMessage(getTranslatorString(nowSubKey->enName, nowSubKey->chName) + getTheTranslatorString("success loaded"));
			}
			break;
		case KEY_FUNC_SAVE_LOAD_USER6:
			result = sysScpi->loadUserSelfState(5);

			if (nowSubKey != NULL && result == __SCPI_SUCCESS)
			{
				outputMessage(getTranslatorString(nowSubKey->enName, nowSubKey->chName) + getTheTranslatorString("success loaded"));
			}
			break;
		case KEY_FUNC_SAVE_LOAD_USER7:
			result = sysScpi->loadUserSelfState(6);

			if (nowSubKey != NULL && result == __SCPI_SUCCESS)
			{
				outputMessage(getTranslatorString(nowSubKey->enName, nowSubKey->chName) + getTheTranslatorString("success loaded"));
			}
			break;
		case KEY_FUNC_SAVE_LOAD_USER8:
			result = sysScpi->loadUserSelfState(7);

			if (nowSubKey != NULL && result == __SCPI_SUCCESS)
			{
				outputMessage(getTranslatorString(nowSubKey->enName, nowSubKey->chName) + getTheTranslatorString("success loaded"));
			}
			break;
		case KEY_FUNC_SAVE_LOAD_USER9:
			result = sysScpi->loadUserSelfState(8);

			if (nowSubKey != NULL && result == __SCPI_SUCCESS)
			{
				outputMessage(getTranslatorString(nowSubKey->enName, nowSubKey->chName) + getTheTranslatorString("success loaded"));
			}
			break;
		case KEY_FUNC_SAVE_LOAD_USER10:
			result = sysScpi->loadUserSelfState(9);

			if (nowSubKey != NULL && result == __SCPI_SUCCESS)
			{
				outputMessage(getTranslatorString(nowSubKey->enName, nowSubKey->chName) + getTheTranslatorString("success loaded"));
			}
			break;
		case KEY_FUNC_SAVE_SAVE_USER1:
			if (sysScpi->getFreeDiskSpace() <= 2 * sizeof(sysData))
			{
				outputMessage(getTheTranslatorString("Free Disk Space is not enough!"));
			} else
			{
				result = sysScpi->saveUserSelfState(0);

				if (nowSubKey != NULL && result == __SCPI_SUCCESS)
				{
					outputMessage(getTranslatorString(nowSubKey->enName, nowSubKey->chName) + getTheTranslatorString("success saved"));
				}
			}
			break;
		case KEY_FUNC_SAVE_SAVE_USER2:
			if (sysScpi->getFreeDiskSpace() <= 2 * sizeof(sysData))
			{
				outputMessage(getTheTranslatorString("Free Disk Space is not enough!"));
			} else
			{
				result = sysScpi->saveUserSelfState(1);

				if (nowSubKey != NULL && result == __SCPI_SUCCESS)
				{
					outputMessage(getTranslatorString(nowSubKey->enName, nowSubKey->chName) + getTheTranslatorString("success saved"));
				}
			}
			break;
		case KEY_FUNC_SAVE_SAVE_USER3:
			if (sysScpi->getFreeDiskSpace() <= 2 * sizeof(sysData))
			{
				outputMessage(getTheTranslatorString("Free Disk Space is not enough!"));
			} else
			{
				result = sysScpi->saveUserSelfState(2);

				if (nowSubKey != NULL && result == __SCPI_SUCCESS)
				{
					outputMessage(getTranslatorString(nowSubKey->enName, nowSubKey->chName) + getTheTranslatorString("success saved"));
				}
			}
			break;
		case KEY_FUNC_SAVE_SAVE_USER4:
			if (sysScpi->getFreeDiskSpace() <= 2 * sizeof(sysData))
			{
				outputMessage(getTheTranslatorString("Free Disk Space is not enough!"));
			} else
			{
				result = sysScpi->saveUserSelfState(3);

				if (nowSubKey != NULL && result == __SCPI_SUCCESS)
				{
					outputMessage(getTranslatorString(nowSubKey->enName, nowSubKey->chName) + getTheTranslatorString("success saved"));
				}
			}
			break;
		case KEY_FUNC_SAVE_SAVE_USER5:
			if (sysScpi->getFreeDiskSpace() <= 2 * sizeof(sysData))
			{
				outputMessage(getTheTranslatorString("Free Disk Space is not enough!"));
			} else
			{
				result = sysScpi->saveUserSelfState(4);

				if (nowSubKey != NULL && result == __SCPI_SUCCESS)
				{
					outputMessage(getTranslatorString(nowSubKey->enName, nowSubKey->chName) + getTheTranslatorString("success saved"));
				}
			}
			break;
		case KEY_FUNC_SAVE_SAVE_USER6:
			if (sysScpi->getFreeDiskSpace() <= 2 * sizeof(sysData))
			{
				outputMessage(getTheTranslatorString("Free Disk Space is not enough!"));
			} else
			{
				result = sysScpi->saveUserSelfState(5);

				if (nowSubKey != NULL && result == __SCPI_SUCCESS)
				{
					outputMessage(getTranslatorString(nowSubKey->enName, nowSubKey->chName) + getTheTranslatorString("success saved"));
				}
			}
			break;
		case KEY_FUNC_SAVE_SAVE_USER7:
			if (sysScpi->getFreeDiskSpace() <= 2 * sizeof(sysData))
			{
				outputMessage(getTheTranslatorString("Free Disk Space is not enough!"));
			} else
			{
				result = sysScpi->saveUserSelfState(6);

				if (nowSubKey != NULL && result == __SCPI_SUCCESS)
				{
					outputMessage(getTranslatorString(nowSubKey->enName, nowSubKey->chName) + getTheTranslatorString("success saved"));
				}
			}
			break;
		case KEY_FUNC_SAVE_SAVE_USER8:
			if (sysScpi->getFreeDiskSpace() <= 2 * sizeof(sysData))
			{
				outputMessage(getTheTranslatorString("Free Disk Space is not enough!"));
			} else
			{
				result = sysScpi->saveUserSelfState(7);

				if (nowSubKey != NULL && result == __SCPI_SUCCESS)
				{
					outputMessage(getTranslatorString(nowSubKey->enName, nowSubKey->chName) + getTheTranslatorString("success saved"));
				}
			}
			break;
		case KEY_FUNC_SAVE_SAVE_USER9:
			if (sysScpi->getFreeDiskSpace() <= 2 * sizeof(sysData))
			{
				outputMessage(getTheTranslatorString("Free Disk Space is not enough!"));
			} else
			{
				result = sysScpi->saveUserSelfState(8);

				if (nowSubKey != NULL && result == __SCPI_SUCCESS)
				{
					outputMessage(getTranslatorString(nowSubKey->enName, nowSubKey->chName) + getTheTranslatorString("success saved"));
				}
			}
			break;
		case KEY_FUNC_SAVE_SAVE_USER10:
			if (sysScpi->getFreeDiskSpace() <= 2 * sizeof(sysData))
			{
				outputMessage(getTheTranslatorString("Free Disk Space is not enough!"));
			} else
			{
				result = sysScpi->saveUserSelfState(9);

				if (nowSubKey != NULL && result == __SCPI_SUCCESS)
				{
					outputMessage(getTranslatorString(nowSubKey->enName, nowSubKey->chName) + getTheTranslatorString("success saved"));
				}
			}
			break;

			//Pass-Fail
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE1EDIT_FIRSTPAGE:
			sysScpi->gotoFirstPageOfMeasureOfPassFailLineData();
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE1EDIT_PREVPAGE:
			sysScpi->gotoPrevPageOfMeasureOfPassFailLineData();
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE1EDIT_NEXTPAGE:
			sysScpi->gotoNextPageOfMeasureOfPassFailLineData();
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE1EDIT_LASTPAGE:
			sysScpi->gotoLastPageOfMeasureOfPassFailLineData();
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE1EDIT_OPERATION_ADD:
			sysScpi->addMeasureOfPassFailOLineUpPoint();
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE1EDIT_OPERATION_INSERT:
			sysScpi->insertMeasureOfPassFailOLineUpPoint();
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE1EDIT_OPERATION_DELETE:
			sysScpi->deleteMeasureOfPassFailOLineUpPoint();
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE1EDIT_OPERATION_CLEAR:
			sysScpi->clearMeasureOfPassFailOLineUpPoint();
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE2EDIT_FIRSTPAGE:
			sysScpi->gotoFirstPageOfMeasureOfPassFailLineData();
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE2EDIT_PREVPAGE:
			sysScpi->gotoPrevPageOfMeasureOfPassFailLineData();
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE2EDIT_NEXTPAGE:
			sysScpi->gotoNextPageOfMeasureOfPassFailLineData();
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE2EDIT_LASTPAGE:
			sysScpi->gotoLastPageOfMeasureOfPassFailLineData();
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE2EDIT_OPERATION_ADD:
			sysScpi->addMeasureOfPassFailOLineUpPoint();
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE2EDIT_OPERATION_INSERT:
			sysScpi->insertMeasureOfPassFailOLineUpPoint();
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE2EDIT_OPERATION_DELETE:
			sysScpi->deleteMeasureOfPassFailOLineUpPoint();
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE2EDIT_OPERATION_CLEAR:
			sysScpi->clearMeasureOfPassFailOLineUpPoint();
			break;

	}
}

//处理输入键
void mainForm::handleInputKey(int funcCode)
{
	char temp[64];
	QString auxStr = "";
	int selIndex = 0;
	int sumPoint = 0;

	switch (funcCode)
	{
		//音频解调频率
		case KEY_FUNC_DEMOD_RADIOSET_1:
			getFreqAndUnitString(sysData.demod.radioFreqSet[0], 0, temp);
			break;
		case KEY_FUNC_DEMOD_RADIOSET_MUSIC:
			getFreqAndUnitString(sysData.demod.radioFreqSet[1], 0, temp);
			break;
		case KEY_FUNC_DEMOD_RADIOSET_TRAFFIC:
			getFreqAndUnitString(sysData.demod.radioFreqSet[2], 0, temp);
			break;
		case KEY_FUNC_DEMOD_RADIOSET_2:
			getFreqAndUnitString(sysData.demod.radioFreqSet[3], 0, temp);
			break;
		case KEY_FUNC_DEMOD_RADIOSET_3:
			getFreqAndUnitString(sysData.demod.radioFreqSet[4], 0, temp);
			break;
		case KEY_FUNC_DEMOD_RADIOSET_4:
			getFreqAndUnitString(sysData.demod.radioFreqSet[5], 0, temp);
			break;

			//AM FM
		case KEY_FUNC_MODE_OPTIONS_DEMOD_AM_CARRIERFREQ:
			getFreqAndUnitString(sysData.options.am_carrierfreq, 0, temp);
			break;
		case KEY_FUNC_MODE_OPTIONS_DEMOD_FM_CARRIERFREQ:
			getFreqAndUnitString(sysData.options.fm_carrierfreq, 0, temp);
			break;
		case KEY_FUNC_MODE_OPTIONS_DEMOD_AM_IFBW:
			getFreqAndUnitString(sysData.options.am_ifbw, 0, temp);
			break;
		case KEY_FUNC_MODE_OPTIONS_DEMOD_FM_IFBW:
			getFreqAndUnitString(sysData.options.fm_ifbw, 0, temp);
			break;

		case KEY_FUNC_FREQ_CENTER:
			getFreqAndUnitString(sysData.freq.center, 0, temp);
			break;
		case KEY_FUNC_FREQ_START:
			getFreqAndUnitString(sysData.freq.start, 0, temp);
			break;
		case KEY_FUNC_FREQ_STOP:
			getFreqAndUnitString(sysData.freq.stop, 0, temp);
			break;
		case KEY_FUNC_FREQ_CFSTEP:
			getFreqAndUnitString(sysData.freq.cfStep, 0, temp);
			break;
		case KEY_FUNC_FREQ_OFFSET:
			getFreqAndUnitString(sysData.freq.offset, 0, temp);
			break;
		case KEY_FUNC_SPAN_SPAN:
			getFreqAndUnitString(sysData.span.span, 0, temp);
			break;

			//trig power
		case KEY_FUNC_TRIG_VIDEO:
			sysScpi->getAmptAndUnitString(sysData.trig.power, temp, 0);
			break;

			//ampt
		case KEY_FUNC_AMPT_REFLEVEL:
			sysScpi->getAmptAndUnitString(sysData.ampt.refLevel, temp, 0);
			break;
		case KEY_FUNC_AMPT_ATT:
			floatToStringDot2(sysData.ampt.att, temp);
			auxStr = " dB";
			break;
		case KEY_FUNC_AMPT_SCALEDIV:
			floatToStringDot2(sysData.ampt.scaleDiv, temp);
			auxStr = " dB";
			break;
		case KEY_FUNC_AMPT_REFOFFSET:
			floatToStringDot2(sysData.ampt.refOffset, temp);
			auxStr = " dB";
			break;

		case KEY_FUNC_BW_RBW:
			getFreqAndUnitString(sysData.bw.rbw, 0, temp);
			break;
		case KEY_FUNC_BW_VBW:
			getFreqAndUnitString(sysData.bw.vbw, 0, temp);
			break;
		case KEY_FUNC_BW_AVERAGE:
			intToString(sysData.bw.bwAverage, temp);
			break;

		case KEY_FUNC_SWEEP_SWEEPTIME:
		case KEY_FUNC_SOURCE_TRACKGEN_NETWORK_SWEEPTIME:
			getTimeAndUnitString(sysData.sweep.sweepTime, 0, temp);
			break;

		case KEY_FUNC_SWEEP_SWEEPPOINTS:
		case KEY_FUNC_SOURCE_TRACKGEN_NETWORK_SWEEPPOINT:
			intToString(sysData.sweep.sweepPoints, temp);
			break;

		case KEY_FUNC_DISPLAY_DISPLAYLINE:
			sysScpi->getAmptAndUnitString(sysData.display.dispLineValue, temp, 0);
			break;

		case KEY_FUNC_MEAS_DISCRIMINATOR_AMPLITUDE_OUTPUTPOWER:
			floatToStringDot2(sysData.disc.outputPower, temp);
			auxStr = " dBm";
			break;
		case KEY_FUNC_MEAS_DISCRIMINATOR_AMPLITUDE_SCALEDIV:
			floatToStringDot2(sysData.disc.scaleDiv, temp);
			auxStr = " v/div";
			break;
		case KEY_FUNC_MEAS_DISCRIMINATOR_AMPLITUDE_OFFSET:
			floatToStringDot2(sysData.disc.dcOffset, temp);
			auxStr = " V";
			break;
		case KEY_FUNC_MEAS_DISCRIMINATOR_SWEEPTIME:
			getTimeAndUnitString(sysData.disc.sweepTime, 0, temp);
			break;

		case KEY_FUNC_MEASSETUP_CHANNELWIDTH:
			getFreqAndUnitString(sysData.measureSetup.channelWidth, 0, temp);
			break;
		case KEY_FUNC_MEASSETUP_CHANNELINTERVAL:
			getFreqAndUnitString(sysData.measureSetup.channelSpace, 0, temp);
			break;
		case KEY_FUNC_MEASSETUP_CHANNELNUMS:
			intToString(sysData.measureSetup.channelNums, temp);
			break;
		case KEY_FUNC_MEASSETUP_POWERBWPERCENT:
			floatToStringDot2(sysData.measureSetup.powerPercent, temp);
			auxStr = "%";
			break;

		case KEY_FUNC_MARKERFCTN_NDB:
			floatToStringDot2(sysData.markerFctn.ndBValue, temp);
			auxStr = " dB";
			break;
		case KEY_FUNC_MARKERFCTN_FREQCOUNT_BW:
			getFreqAndUnitString(sysData.markerFctn.freqCountBw, 0, temp);
			break;

		case KEY_FUNC_SYSTEM_CONFIGIO_LAN_IP:
			strcpy(temp, sysData.system.io.lan.ip);
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_LAN_MASK:
			strcpy(temp, sysData.system.io.lan.mask);
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_LAN_GATE:
			strcpy(temp, sysData.system.io.lan.gate);
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_GPIB_ADDR:
			intToString(sysData.system.io.gpib.addr, temp);
			break;

		case KEY_FUNC_SYSTEM_SERVICE_SERIALNO:
			strcpy(temp, sysData.system.serialNo);
			break;//longIntToString(sysData.system.serialNo, temp); break;
		case KEY_FUNC_SYSTEM_SERVICE_CALIBRATION_EDIT_9K:
			floatToStringDot2(sysData.factoryCalData.freqResp[0][0], temp);
			break;
		case KEY_FUNC_SYSTEM_SERVICE_CALIBRATION_EDIT_20K:
			floatToStringDot2(sysData.factoryCalData.freqResp[0][1], temp);
			break;

		case KEY_FUNC_SYSTEM_BACKLIGHT:
			intToString(sysData.system.backLight, temp);
			break;
		case KEY_FUNC_SYSTEM_DATETIME_DATE:
			getLocalDate(temp);
			break;
		case KEY_FUNC_SYSTEM_DATETIME_TIME:
			getLocalTime(temp);
			break;

		case KEY_FUNC_SOURCE_TRACKGEN_POWER:
			sysScpi->getAmptAndUnitString(sysData.source.trackGenOutputPower, temp, 0);
			break;
		case KEY_FUNC_SOURCE_TRACKGEN_NETWORK_REFLEVEL:
			floatToStringDot2(sysData.source.trackGenNetworkRefLevel, temp);
			auxStr = " dB";
			break;
		case KEY_FUNC_SOURCE_TRACKGEN_NETWORK_POWER:
			sysScpi->getAmptAndUnitString(sysData.source.trackGenNetworkOutputPower, temp, 0);
			break;
		case KEY_FUNC_SOURCE_TRACKGEN_VSWR_REFERENCE:
			floatToStringDot2(sysData.source.vswrRef, temp);
			auxStr = " dB";
			break;
		case KEY_FUNC_SOURCE_SIGNALGEN_FREQ:
			getFreqAndUnitString(sysData.source.signalGenOutputFreq, 0, temp);
			break;
		case KEY_FUNC_SOURCE_SIGNALGEN_POWER:
			sysScpi->getAmptAndUnitString(sysData.source.signalGenOutputPower, temp, 0);
			break;

		case KEY_FUNC_PRINTSETUP_NUMS:
			intToString(sysData.print.nums, temp);
			break;

		case KEY_FUNC_DEMOD_SOUND:
			intToString(sysData.demod.sound, temp);
			break;
		case KEY_FUNC_AUX_IMPEDANCE_CONVERSION:
			floatToStringDot2(sysData.ampt.LMPValue, temp);
			auxStr = " dB";
			break;

		case KEY_FUNC_AMPT_PASSFAIL_WINDOW_LIMITUPLOW:
			if (sysData.measure.passFail.winNowAmptUpOrLow == 0)
			{
				sysScpi->getAmptAndUnitString(sysData.measure.passFail.winAmptUp, temp, 0);
			} else
			{
				sysScpi->getAmptAndUnitString(sysData.measure.passFail.winAmptLow, temp, 0);
			}
			break;
		case KEY_FUNC_AMPT_PASSFAIL_WINDOW_FREQSTARTSTOP:
			if (sysData.measure.passFail.winNowFreqStartOrStop == 0)
			{
				getFreqAndUnitString(sysData.measure.passFail.winFreqStart, 0, temp);
			} else
			{
				getFreqAndUnitString(sysData.measure.passFail.winFreqStop, 0, temp);
			}
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_SHIFTXY:
			if (sysData.measure.passFail.lineNowShiftXOrY == 0)
			{
				getFreqAndUnitString(sysData.measure.passFail.lineShiftXOffset, 0, temp);
			} else
			{
				sysScpi->getAmptAndUnitString(sysData.measure.passFail.lineShiftYOffset, temp, 0);
			}
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE1EDIT_OPERATION_EDITFREQ:
			selIndex = sysData.measure.passFail.line1EditNowSelIndex;
			sumPoint = sysData.measure.passFail.line1TotalPoints;

			if (selIndex >= 0 && sumPoint > 0 && selIndex < sumPoint)
			{
				getFreqAndUnitString(sysData.measure.passFail.line1Datas[selIndex + sysData.measure.passFail.line1NowPageIndex * PASSFAILCOUNTPERPAGE].freq, 0, temp);
			} else
			{
				sprintf(temp, "%s", "-");
			}
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE1EDIT_OPERATION_EDITLIMIT:
			selIndex = sysData.measure.passFail.line1EditNowSelIndex;
			sumPoint = sysData.measure.passFail.line1TotalPoints;

			if (selIndex >= 0 && sumPoint > 0 && selIndex < sumPoint)
			{
				sysScpi->getAmptAndUnitString(sysData.measure.passFail.line1Datas[selIndex + sysData.measure.passFail.line1NowPageIndex * PASSFAILCOUNTPERPAGE].ampt, temp, 0);
			} else
			{
				sprintf(temp, "%s", "-");
			}
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE2EDIT_OPERATION_EDITFREQ:
			selIndex = sysData.measure.passFail.line2EditNowSelIndex;
			sumPoint = sysData.measure.passFail.line2TotalPoints;

			if (selIndex >= 0 && sumPoint > 0 && selIndex < sumPoint)
			{
				getFreqAndUnitString(sysData.measure.passFail.line2Datas[selIndex + sysData.measure.passFail.line1NowPageIndex * PASSFAILCOUNTPERPAGE].freq, 0, temp);
			} else
			{
				sprintf(temp, "%s", "-");
			}
			break;

		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE2EDIT_OPERATION_EDITLIMIT:
			selIndex = sysData.measure.passFail.line2EditNowSelIndex;
			sumPoint = sysData.measure.passFail.line2TotalPoints;

			if (selIndex >= 0 && sumPoint > 0 && selIndex < sumPoint)
			{
				sysScpi->getAmptAndUnitString(sysData.measure.passFail.line2Datas[selIndex + sysData.measure.passFail.line1NowPageIndex * PASSFAILCOUNTPERPAGE].ampt, temp, 0);
			} else
			{
				sprintf(temp, "%s", "-");
			}
			break;
	}

	tagValue = temp;
	tagValue = tagValue.trimmed();
	tagValue += auxStr;
}

//处理输入键
void mainForm::handleListKey(int funcCode)
{
	int tempValue = 0;

	switch (funcCode)
	{
		case KEY_FUNC_TRACE_TRACE:
			tempValue = sysData.traceSelIndex;
			tempValue++;

			if (tempValue >= TRACECOUNT)
			{
				tempValue = 0;
			}

			sysData.traceSelIndex = tempValue;
			break;

		case KEY_FUNC_MARKER_MARKER:
			tempValue = sysData.markerSelIndex;
			tempValue++;

			if (tempValue >= MARKERCOUNT)
			{
				tempValue = 0;
			}

			sysData.markerSelIndex = tempValue;
			refreshMarkerState();
			break;

		case KEY_FUNC_MARKER_TRACE:
			tempValue = sysData.marker[sysData.markerSelIndex].traceIndex;
			tempValue++;

			if (tempValue >= TRACECOUNT)
			{
				tempValue = 0;
			}

			sysData.marker[sysData.markerSelIndex].traceIndex = tempValue;
			refreshMarkerState();
			break;

		case KEY_FUNC_SYSTEM_SERVICE_LANMACADDR:
			tempValue = sysData.system.lanMacAddrIndex;
			tempValue++;

			if (tempValue >= sizeof sysData.system.lanMacAddress / sizeof sysData.system.lanMacAddress[0])
			{
				tempValue = 0;
			}

			sysData.system.lanMacAddrIndex = tempValue;

			break;
	}
}

//处理取反键
void mainForm::handleReverseKey(int funcCode)
{
	double tempValue = 0;

	switch (funcCode)
	{
		//校准模式
		case KEY_FUNC_SYSTEM_SERVICE_CALIBRATION_MODE:
			sysScpi->setSystemServiceCalbrationMode(!sysData.isComCalbration);
			break;
			//fft
		case KEY_FUNC_SYSTEM_SERVICE_OPTION_FFT:
			sysScpi->setSystemServiceOptionFFT10Hz(!sysData.options.isFft10HzOn);
			break;
			break;
		case KEY_FUNC_SYSTEM_SERVICE_OPTION_AMFM:
			sysScpi->setSystemServiceOptionDemod(!sysData.options.isDemodOn);
			break;
		case KEY_FUNC_SYSTEM_SERVICE_OPTION_DIRECTCHANNEL:
			sysScpi->setSystemServiceOptionDirectChannel(!sysData.options.directChannelOptionOn);
			break;

			//AM & FM
		case KEY_FUNC_MODE_OPTIONS_DEMOD_AM_AM:
			sysScpi->setDemodOfSoundState(0);
			sysScpi->setDemodOfAmState(!sysData.options.amOn);
			break;
		case KEY_FUNC_MODE_OPTIONS_DEMOD_FM_FM:
			sysScpi->setDemodOfSoundState(0);
			sysScpi->setDemodOfFmState(!sysData.options.fmOn);
			break;
		case KEY_FUNC_MODE_OPTIONS_DEMOD_AM_IFBW:
			sysScpi->setDemodOfAmIFBWState(!sysData.options.am_ifbw_auto);
			break;
		case KEY_FUNC_MODE_OPTIONS_DEMOD_FM_IFBW:
			sysScpi->setDemodOfFmIFBWState(!sysData.options.fm_ifbw_auto);
			break;

		case KEY_FUNC_FREQ_CFSTEP:
			sysScpi->setFrequencyOfCenterStepAuto(!sysData.freq.cfStepAuto);
			break;
		case KEY_FUNC_FREQ_REFERENCE:
			if (sysData.freq.refInternal)
			{
				sysScpi->showRunMessage(getTheTranslatorString("Please Input 10MHz Reference"));
				usleep(2000 * 1000);
			}
			sysScpi->setFrequencyOfReference(!sysData.freq.refInternal);
			break;
		case KEY_FUNC_FREQ_LOW:
			sysScpi->setFrequencyOfLowChannel(!sysData.freq.isLowChannel);
			break;
		case KEY_FUNC_FREQ_SCALETYPE:
			sysScpi->setFrequencyOfScaleType(!sysData.freq.typeLine);
			break;

		case KEY_FUNC_AMPT_ATT:
			sysScpi->setAmptOfAttAuto(!sysData.ampt.attAuto);
			break;
		case KEY_FUNC_AMPT_SCALETYPE:
			sysScpi->setAmptOfScaleType(!sysData.ampt.scaleLine);
			resetAmptScaleLineLogic();
			break;
		case KEY_FUNC_AMPT_PREAMPT:
			sysScpi->setPreamptOfAttAuto(!sysData.ampt.isPreamptOn);
			break;

		case KEY_FUNC_BW_RBW:
			sysScpi->setBwOfRbwAuto(!sysData.bw.rbwAuto);
			break;
		case KEY_FUNC_BW_RBWMODE:
			sysScpi->setBwOfRbwStepMode(!sysData.bw.rbwStepCont);
			break;
		case KEY_FUNC_BW_VBW:
			sysScpi->setBwOfVbwAuto(!sysData.bw.vbwAuto);
			break;
		case KEY_FUNC_BW_VBWMODE:
			break;
		case KEY_FUNC_BW_AVERAGE:
			sysScpi->setBwOfAverageSwitch(!sysData.bw.bwAverageOn);
			break;
		case KEY_FUNC_BW_EMC_EMC:
			sysScpi->setBwOfEmcState(!sysData.emc.emcOn);
			if (sysData.emc.emcOn)
				gotoTheEMINode();
			break;

		case KEY_FUNC_SWEEP_SWEEPTIME:
		case KEY_FUNC_SOURCE_TRACKGEN_NETWORK_SWEEPTIME:
			sysScpi->setSweepOfTimeAuto(!sysData.sweep.sweepTimeAuto);
			break;
		case KEY_FUNC_DISPLAY_ZOOM:
			if (!sysData.display.isZoomOn)
			{
				sysLocked = true;
				img_Paused_Zoom = true;
				img_Paused = true;
				sysData.canvasNums = 2;
				drawCanvas();
				img_Paused_Zoom = false;
				img_Paused_Zoom_Full = true;
				img_Paused = true;
				sysData.display.isFullScreen = true;
				drawCanvas();
				img_Paused_Zoom_Full = false;
				sysData.canvasNums = 1;
				sysData.display.isFullScreen = false;
				img_Paused = false;
				sysLocked = false;
			}

			sysScpi->setDisplayOfZoomOn(!sysData.display.isZoomOn);
			break;
		case KEY_FUNC_DISPLAY_DISPLAYLINE:
			sysScpi->setDisplayOfLine(!sysData.display.isDispLineOn);
			break;
		case KEY_FUNC_DISPLAY_REFSCALE:
			sysScpi->setDisplayOfRefLevelGraticule(!sysData.display.isAmptGraticuleOn);
			break;
		case KEY_FUNC_DISPLAY_GRID:
			sysScpi->setDisplayOfGrid(!sysData.display.isGridOn);
			break;
		case KEY_FUNC_DISPLAY_LABEL:
			sysScpi->setDisplayOfLabel(!sysData.display.isLabelOn);
			break;

		case KEY_FUNC_MEAS_TIMESPEC:
			sysScpi->setMeasureOfTimeSpectrumOn(!sysData.measure.isTimeSpectrumOn);
			break;
		case KEY_FUNC_MEAS_ACPR:
			sysScpi->setMeasureOfAcprOn(!sysData.measure.isAcprOn);
			break;
		case KEY_FUNC_MEAS_CHANNELPOWER:
			sysScpi->setMeasureOfChannelPowerOn(!sysData.measure.isChannelPowerOn);
			break;
		case KEY_FUNC_MEAS_OBW:
			sysScpi->setMeasureOfObwOn(!sysData.measure.isObwOn);
			break;
		case KEY_FUNC_MEAS_DISCRIMINATOR_DISCRIMINATOR:
			if (!sysData.disc.isDiscOn)
			{
				sysScpi->showRunMessage(getTheTranslatorString("device connecting") + "......");
				usleep(FORMCREATETIME);
			}

			if (sysScpi->setMeasureOfDiscState(!sysData.disc.isDiscOn) == __SCPI_SUCCESS)
			{
				if (sysData.disc.isDiscOn)
				{
					helpContent = getTheTranslatorString("connect to discriminator success");
					drawHelp();
					break;
				} else
				{
					helpContent = getTheTranslatorString("disconnect from discriminator");
				}
			} else
			{
				if (!sysData.disc.isDiscOn)
				{
					sysScpi->showRunAlarm(getTheTranslatorString("connect to discriminator failed"));
					usleep(FORMCLOSETIME);
				}

				helpContent = getTheTranslatorString("connect to discriminator failed");
			}

			drawHelp();
			break;

		case KEY_FUNC_MARKER_LIST:
			sysScpi->setMarkerTable(!sysData.markerTableOn);
			break;

		case KEY_FUNC_MARKERFCTN_NDB:
			sysScpi->setMarkerFunctionNdBOn(!sysData.markerFctn.isNdBOn);
			break;
		case KEY_FUNC_MARKERFCTN_NOISE:
			sysScpi->setMarkerFunctionNoiseOn(!sysData.markerFctn.isMkrNoiseOn);
			break;
		case KEY_FUNC_MARKERFCTN_FREQCOUNT_FREQCOUNT:
			sysScpi->setMarkerFunctionFreqCountOn(!sysData.markerFctn.isFreqCountOn);
			break;

		case KEY_FUNC_PEAK_AUTOMAXSEARCH:
			sysScpi->setMarkerPeakMaxAuto(!sysData.isAutoPeakMaxing);
			break;

		case KEY_FUNC_SYSTEM_CHANNELSELECT:
			sysScpi->setSystemChannelSelect(!sysData.system.isInternalSignalOn);
			break;
		case KEY_FUNC_SYSTEM_SCPI_SCREENREFRESH:
			sysScpi->setSystemScpiScreenRefresh(!sysData.system.isScpiScreenRefresh);
			break;
		case KEY_FUNC_SYSTEM_DATETIME_DATETIME:
			sysScpi->setSystemDateTimeShow(!sysData.system.datetime.isVisible);
			drawTop();
			break;
		case KEY_FUNC_SYSTEM_SERVICE_EVALUATE:
			sysScpi->setSystemServiceOfEvalute(!sysScpi->eva.isEvaluate);
			break;

		case KEY_FUNC_SYSTEM_SERVICE_TRACK_SELECT:
			sysScpi->setSystemServiceOfTrackSelect(!sysData.options.isTrack36GHzOn);
			break;

		case KEY_FUNC_SYSTEM_SERVICE_DEVICE_LOGDISPLAY:
			sysScpi->setSystemServiceOfSelectLog(!sysData.system.isLogOn);
			break;
		case KEY_FUNC_SYSTEM_SERVICE_DEVICE_GPIB:
			sysScpi->setSystemServiceOfGpib(!sysData.system.isGpibOn);
			break;

		case KEY_FUNC_SOURCE_TRACKGEN_TRACKGEN:
			sysScpi->setSourceTrackGenState(!sysData.source.isTrackGenOn);
			break;
		case KEY_FUNC_SOURCE_TRACKGEN_NETWORK_NETWORK:
			sysScpi->setSourceTrackGenNetworkState(!sysData.source.isTrackGenNetworkOn);
			break;
		case KEY_FUNC_SOURCE_TRACKGEN_VSWR_VSWR:
			sysScpi->setSourceTrackGenVSWRState(!sysData.source.isVswrOn);
			break;
		case KEY_FUNC_SOURCE_SIGNALGEN_SIGNALGEN:
			sysScpi->setSourceSignalGenState(!sysData.source.isSignalGenOn, false);
			break;

		case KEY_FUNC_PRINTSETUP_LANGUAGE:
			sysData.print.isPclLanguage = !sysData.print.isPclLanguage;
			break;
		case KEY_FUNC_PRINTSETUP_COLOR:
			sysScpi->setPrintOfColor(!sysData.print.isColorPrinter);
			break;
		case KEY_FUNC_PRINTSETUP_ORIENTATION:
			sysScpi->setPrintOfOrientation(!sysData.print.isOrientationHor);
			break;

		case KEY_FUNC_DEMOD_DEMOD:
			sysScpi->setDemodOfAmState(0);
			sysScpi->setDemodOfFmState(0);
			sysScpi->setDemodOfSoundState(!sysData.demod.demodOn);
			break;

		case KEY_FUNC_AUX_IMPEDANCE_CONVERSION:
			sysScpi->setImpedanceOfState(!sysData.isImpedanceTransformation);
			break;

			//Pass-Fail
		case KEY_FUNC_AMPT_PASSFAIL_WINDOW_WINDOW:
			sysScpi->setMeasureOfPassFailOfWinMeasState(!sysData.measure.passFail.winMeasOn);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_WINDOW_LIMITLINEON:
			sysScpi->setMeasureOfPassFailOfWinLimitLineState(!sysData.measure.passFail.winAmptOn);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_WINDOW_FREQLINEON:
			sysScpi->setMeasureOfPassFailOfWinFreqLineState(!sysData.measure.passFail.winFreqOn);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_WINDOW_LIMITUPLOW:
			sysScpi->setMeasureOfPassFailOfWinLineValueMode(!sysData.measure.passFail.winNowAmptUpOrLow);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_WINDOW_FREQSTARTSTOP:
			sysScpi->setMeasureOfPassFailOfWinFreqValueMode(!sysData.measure.passFail.winNowFreqStartOrStop);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_WINDOW_WINDOWSWEEP:
			sysScpi->setMeasureOfPassFailOfWinSweepState(!sysData.measure.passFail.winSweepOn);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE:
			sysScpi->setMeasureOfPassFailOfLineMeasState(!sysData.measure.passFail.lineMeasOn);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE1ON:
			sysScpi->setMeasureOfPassFailOfLineUpState(!sysData.measure.passFail.line1On);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE2ON:
			sysScpi->setMeasureOfPassFailOfLineLowState(!sysData.measure.passFail.line2On);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_SHIFTXY:
			sysScpi->setMeasureOfPassFailOfLineShiftMode(!sysData.measure.passFail.lineNowShiftXOrY);
			break;
	}
}

//处理单位键
void mainForm::handleReverseUnitKey(int funcCode, QString value)
{
	bool isOk = false;
	double dValue = value.toDouble(&isOk);

	if (!isOk)
		return;
	dValue *= -1.0;

	switch (funcCode)
	{
		case KEY_FUNC_AMPT_REFLEVEL_DBM:
			sysScpi->setAmptOfRefLevel(dValue);
			break;
		case KEY_FUNC_TRIG_VIDEO_DBM:
			sysScpi->setAmptOfRefLevel(dValue);
			break;
		default:
			break;
	}
}

//处理单位键
void mainForm::handleUnitKey(int funcCode, QString value)
{
	bool requireControl = false;
	bool isOk = false;
	long int code = 0;
	char temp[32] = { };

	value.toDouble(&isOk);

	switch (funcCode)
	{
		case KEY_FUNC_AMPT_PASSFAIL_WINDOW_LIMITUPLOW_DBM:
			if (isOk)
				sysScpi->setMeasureOfPassFailOfWinLineValue(value.toDouble());
			break;
		case KEY_FUNC_AMPT_PASSFAIL_WINDOW_FREQSTARTSTOP_GHZ:
			if (isOk)
				sysScpi->setMeasureOfPassFailOfWinFreqValue(value.toDouble() * 1e9);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_WINDOW_FREQSTARTSTOP_MHZ:
			if (isOk)
				sysScpi->setMeasureOfPassFailOfWinFreqValue(value.toDouble() * 1e6);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_WINDOW_FREQSTARTSTOP_KHZ:
			if (isOk)
				sysScpi->setMeasureOfPassFailOfWinFreqValue(value.toDouble() * 1e3);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_WINDOW_FREQSTARTSTOP_HZ:
			if (isOk)
				sysScpi->setMeasureOfPassFailOfWinFreqValue(value.toDouble());
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE1EDIT_OPERATION_EDITFREQ_GHZ:
			if (isOk)
				sysScpi->setMeasureOfPassFailOfLineFreq(value.toDouble() * 1e9);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE1EDIT_OPERATION_EDITFREQ_MHZ:
			if (isOk)
				sysScpi->setMeasureOfPassFailOfLineFreq(value.toDouble() * 1e6);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE1EDIT_OPERATION_EDITFREQ_KHZ:
			if (isOk)
				sysScpi->setMeasureOfPassFailOfLineFreq(value.toDouble() * 1e3);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE1EDIT_OPERATION_EDITFREQ_HZ:
			if (isOk)
				sysScpi->setMeasureOfPassFailOfLineFreq(value.toDouble());
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE1EDIT_OPERATION_EDITLIMIT_DBM:
			if (isOk)
				sysScpi->setMeasureOfPassFailOfLineAmpt(value.toDouble());
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE2EDIT_OPERATION_EDITFREQ_GHZ:
			if (isOk)
				sysScpi->setMeasureOfPassFailOfLineFreq(value.toDouble() * 1e9);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE2EDIT_OPERATION_EDITFREQ_MHZ:
			if (isOk)
				sysScpi->setMeasureOfPassFailOfLineFreq(value.toDouble() * 1e6);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE2EDIT_OPERATION_EDITFREQ_KHZ:
			if (isOk)
				sysScpi->setMeasureOfPassFailOfLineFreq(value.toDouble() * 1e3);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE2EDIT_OPERATION_EDITFREQ_HZ:
			if (isOk)
				sysScpi->setMeasureOfPassFailOfLineFreq(value.toDouble());
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE2EDIT_OPERATION_EDITLIMIT_DBM:
			if (isOk)
				sysScpi->setMeasureOfPassFailOfLineAmpt(value.toDouble());
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_SHIFTXY_GHZ:
			if (sysData.measure.passFail.lineNowShiftXOrY == 0)
			{
				if (isOk)
					sysScpi->setMeasureOfPassFailOfLineShiftXYValue(value.toDouble() * 1e9);
			} else
			{
				if (isOk)
					sysScpi->setMeasureOfPassFailOfLineShiftXYValue(value.toDouble());
			}
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_SHIFTXY_MHZ:
			if (sysData.measure.passFail.lineNowShiftXOrY == 0)
			{
				if (isOk)
					sysScpi->setMeasureOfPassFailOfLineShiftXYValue(value.toDouble() * 1e6);
			} else
			{
				if (isOk)
					sysScpi->setMeasureOfPassFailOfLineShiftXYValue(value.toDouble());
			}
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_SHIFTXY_KHZ:
			if (sysData.measure.passFail.lineNowShiftXOrY == 0)
			{
				if (isOk)
					sysScpi->setMeasureOfPassFailOfLineShiftXYValue(value.toDouble() * 1e3);
			} else
			{
				if (isOk)
					sysScpi->setMeasureOfPassFailOfLineShiftXYValue(value.toDouble());
			}
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_SHIFTXY_HZ:
			if (isOk)
				sysScpi->setMeasureOfPassFailOfLineShiftXYValue(value.toDouble());
			break;

			//
		case KEY_FUNC_AUX_IMPEDANCE_CONVERSION_ENTER:
			//if (isOk && sysData.isImpedanceTransformation)
			if (isOk)
			{
				sysScpi->setLMP_CValue(value.toDouble());
			}
			break;

			//AM FM
		case KEY_FUNC_MODE_OPTIONS_DEMOD_AM_CARRIERFREQ_GHZ:
			if (isOk)
				sysScpi->setDemodOfAmCarrierFreq(value.toDouble() * 1e9);
			break;
		case KEY_FUNC_MODE_OPTIONS_DEMOD_AM_CARRIERFREQ_MHZ:
			if (isOk)
				sysScpi->setDemodOfAmCarrierFreq(value.toDouble() * 1e6);
			break;
		case KEY_FUNC_MODE_OPTIONS_DEMOD_AM_CARRIERFREQ_KHZ:
			if (isOk)
				sysScpi->setDemodOfAmCarrierFreq(value.toDouble() * 1e3);
			break;
		case KEY_FUNC_MODE_OPTIONS_DEMOD_AM_CARRIERFREQ_HZ:
			if (isOk)
				sysScpi->setDemodOfAmCarrierFreq(value.toDouble());
			break;
		case KEY_FUNC_MODE_OPTIONS_DEMOD_AM_IFBW_GHZ:
			if (isOk)
				sysScpi->setDemodOfAmIFBW(value.toDouble() * 1e9);
			break;
		case KEY_FUNC_MODE_OPTIONS_DEMOD_AM_IFBW_MHZ:
			if (isOk)
				sysScpi->setDemodOfAmIFBW(value.toDouble() * 1e6);
			break;
		case KEY_FUNC_MODE_OPTIONS_DEMOD_AM_IFBW_KHZ:
			if (isOk)
				sysScpi->setDemodOfAmIFBW(value.toDouble() * 1e3);
			break;
		case KEY_FUNC_MODE_OPTIONS_DEMOD_AM_IFBW_HZ:
			if (isOk)
				sysScpi->setDemodOfAmIFBW(value.toDouble());
			break;
		case KEY_FUNC_MODE_OPTIONS_DEMOD_FM_CARRIERFREQ_GHZ:
			if (isOk)
				sysScpi->setDemodOfFmCarrierFreq(value.toDouble() * 1e9);
			break;
		case KEY_FUNC_MODE_OPTIONS_DEMOD_FM_CARRIERFREQ_MHZ:
			if (isOk)
				sysScpi->setDemodOfFmCarrierFreq(value.toDouble() * 1e6);
			break;
		case KEY_FUNC_MODE_OPTIONS_DEMOD_FM_CARRIERFREQ_KHZ:
			if (isOk)
				sysScpi->setDemodOfFmCarrierFreq(value.toDouble() * 1e3);
			break;
		case KEY_FUNC_MODE_OPTIONS_DEMOD_FM_CARRIERFREQ_HZ:
			if (isOk)
				sysScpi->setDemodOfFmCarrierFreq(value.toDouble());
			break;
		case KEY_FUNC_MODE_OPTIONS_DEMOD_FM_IFBW_GHZ:
			if (isOk)
				sysScpi->setDemodOfFmIFBW(value.toDouble() * 1e9);
			break;
		case KEY_FUNC_MODE_OPTIONS_DEMOD_FM_IFBW_MHZ:
			if (isOk)
				sysScpi->setDemodOfFmIFBW(value.toDouble() * 1e6);
			break;
		case KEY_FUNC_MODE_OPTIONS_DEMOD_FM_IFBW_KHZ:
			if (isOk)
				sysScpi->setDemodOfFmIFBW(value.toDouble() * 1e3);
			break;
		case KEY_FUNC_MODE_OPTIONS_DEMOD_FM_IFBW_HZ:
			if (isOk)
				sysScpi->setDemodOfFmIFBW(value.toDouble());
			break;

			//demod freq
		case KEY_FUNC_DEMOD_RADIOSET_1_GHZ:
			if (isOk)
				sysScpi->setDemodOfTheFrequency(radio1, value.toDouble() * 1e9);
			break;
		case KEY_FUNC_DEMOD_RADIOSET_1_MHZ:
			if (isOk)
				sysScpi->setDemodOfTheFrequency(radio1, value.toDouble() * 1e6);
			break;
		case KEY_FUNC_DEMOD_RADIOSET_1_KHZ:
			if (isOk)
				sysScpi->setDemodOfTheFrequency(radio1, value.toDouble() * 1e3);
			break;
		case KEY_FUNC_DEMOD_RADIOSET_1_HZ:
			if (isOk)
				sysScpi->setDemodOfTheFrequency(radio1, value.toDouble());
			break;

		case KEY_FUNC_DEMOD_RADIOSET_MUSIC_GHZ:
			if (isOk)
				sysScpi->setDemodOfTheFrequency(radio2, value.toDouble() * 1e9);
			break;
		case KEY_FUNC_DEMOD_RADIOSET_MUSIC_MHZ:
			if (isOk)
				sysScpi->setDemodOfTheFrequency(radio2, value.toDouble() * 1e6);
			break;
		case KEY_FUNC_DEMOD_RADIOSET_MUSIC_KHZ:
			if (isOk)
				sysScpi->setDemodOfTheFrequency(radio2, value.toDouble() * 1e3);
			break;
		case KEY_FUNC_DEMOD_RADIOSET_MUSIC_HZ:
			if (isOk)
				sysScpi->setDemodOfTheFrequency(radio2, value.toDouble());
			break;

		case KEY_FUNC_DEMOD_RADIOSET_TRAFFIC_GHZ:
			if (isOk)
				sysScpi->setDemodOfTheFrequency(radio3, value.toDouble() * 1e9);
			break;
		case KEY_FUNC_DEMOD_RADIOSET_TRAFFIC_MHZ:
			if (isOk)
				sysScpi->setDemodOfTheFrequency(radio3, value.toDouble() * 1e6);
			break;
		case KEY_FUNC_DEMOD_RADIOSET_TRAFFIC_KHZ:
			if (isOk)
				sysScpi->setDemodOfTheFrequency(radio3, value.toDouble() * 1e3);
			break;
		case KEY_FUNC_DEMOD_RADIOSET_TRAFFIC_HZ:
			if (isOk)
				sysScpi->setDemodOfTheFrequency(radio3, value.toDouble());
			break;

		case KEY_FUNC_DEMOD_RADIOSET_2_GHZ:
			if (isOk)
				sysScpi->setDemodOfTheFrequency(radio4, value.toDouble() * 1e9);
			break;
		case KEY_FUNC_DEMOD_RADIOSET_2_MHZ:
			if (isOk)
				sysScpi->setDemodOfTheFrequency(radio4, value.toDouble() * 1e6);
			break;
		case KEY_FUNC_DEMOD_RADIOSET_2_KHZ:
			if (isOk)
				sysScpi->setDemodOfTheFrequency(radio4, value.toDouble() * 1e3);
			break;
		case KEY_FUNC_DEMOD_RADIOSET_2_HZ:
			if (isOk)
				sysScpi->setDemodOfTheFrequency(radio4, value.toDouble());
			break;

		case KEY_FUNC_DEMOD_RADIOSET_3_GHZ:
			if (isOk)
				sysScpi->setDemodOfTheFrequency(radio5, value.toDouble() * 1e9);
			break;
		case KEY_FUNC_DEMOD_RADIOSET_3_MHZ:
			if (isOk)
				sysScpi->setDemodOfTheFrequency(radio5, value.toDouble() * 1e6);
			break;
		case KEY_FUNC_DEMOD_RADIOSET_3_KHZ:
			if (isOk)
				sysScpi->setDemodOfTheFrequency(radio5, value.toDouble() * 1e3);
			break;
		case KEY_FUNC_DEMOD_RADIOSET_3_HZ:
			if (isOk)
				sysScpi->setDemodOfTheFrequency(radio5, value.toDouble());
			break;

		case KEY_FUNC_DEMOD_RADIOSET_4_GHZ:
			if (isOk)
				sysScpi->setDemodOfTheFrequency(radio6, value.toDouble() * 1e9);
			break;
		case KEY_FUNC_DEMOD_RADIOSET_4_MHZ:
			if (isOk)
				sysScpi->setDemodOfTheFrequency(radio6, value.toDouble() * 1e6);
			break;
		case KEY_FUNC_DEMOD_RADIOSET_4_KHZ:
			if (isOk)
				sysScpi->setDemodOfTheFrequency(radio6, value.toDouble() * 1e3);
			break;
		case KEY_FUNC_DEMOD_RADIOSET_4_HZ:
			if (isOk)
				sysScpi->setDemodOfTheFrequency(radio6, value.toDouble());
			break;

			//center freq
		case KEY_FUNC_FREQ_CENTER_GHZ:
			if (isOk)
				sysScpi->setFrequencyOfCenter(value.toDouble() * 1e9);
			break;
		case KEY_FUNC_FREQ_CENTER_MHZ:
			if (isOk)
				sysScpi->setFrequencyOfCenter(value.toDouble() * 1e6);
			break;
		case KEY_FUNC_FREQ_CENTER_KHZ:
			if (isOk)
				sysScpi->setFrequencyOfCenter(value.toDouble() * 1e3);
			break;
		case KEY_FUNC_FREQ_CENTER_HZ:
			if (isOk)
				sysScpi->setFrequencyOfCenter(value.toDouble());
			break;
			//start freq
		case KEY_FUNC_FREQ_START_GHZ:
			if (isOk)
				sysScpi->setFrequencyOfStart(value.toDouble() * 1e9);
			break;
		case KEY_FUNC_FREQ_START_MHZ:
			if (isOk)
				sysScpi->setFrequencyOfStart(value.toDouble() * 1e6);
			break;
		case KEY_FUNC_FREQ_START_KHZ:
			if (isOk)
				sysScpi->setFrequencyOfStart(value.toDouble() * 1e3);
			break;
		case KEY_FUNC_FREQ_START_HZ:
			if (isOk)
				sysScpi->setFrequencyOfStart(value.toDouble());
			break;
			//stop freq
		case KEY_FUNC_FREQ_STOP_GHZ:
			if (isOk)
				sysScpi->setFrequencyOfStop(value.toDouble() * 1e9);
			break;
		case KEY_FUNC_FREQ_STOP_MHZ:
			if (isOk)
				sysScpi->setFrequencyOfStop(value.toDouble() * 1e6);
			break;
		case KEY_FUNC_FREQ_STOP_KHZ:
			if (isOk)
				sysScpi->setFrequencyOfStop(value.toDouble() * 1e3);
			break;
		case KEY_FUNC_FREQ_STOP_HZ:
			if (isOk)
				sysScpi->setFrequencyOfStop(value.toDouble());
			break;
			//freq offset
		case KEY_FUNC_FREQ_OFFSET_GHZ:
			if (isOk)
				sysScpi->setFrequencyOffset(value.toDouble() * 1e9);
			break;
		case KEY_FUNC_FREQ_OFFSET_MHZ:
			if (isOk)
				sysScpi->setFrequencyOffset(value.toDouble() * 1e6);
			break;
		case KEY_FUNC_FREQ_OFFSET_KHZ:
			if (isOk)
				sysScpi->setFrequencyOffset(value.toDouble() * 1e3);
			break;
		case KEY_FUNC_FREQ_OFFSET_HZ:
			if (isOk)
				sysScpi->setFrequencyOffset(value.toDouble());
			break;
			//center freq step
		case KEY_FUNC_FREQ_CFSTEP_GHZ:
			if (isOk)
				sysScpi->setFrequencyOfCenterStep(value.toDouble() * 1e9);
			break;
		case KEY_FUNC_FREQ_CFSTEP_MHZ:
			if (isOk)
				sysScpi->setFrequencyOfCenterStep(value.toDouble() * 1e6);
			break;
		case KEY_FUNC_FREQ_CFSTEP_KHZ:
			if (isOk)
				sysScpi->setFrequencyOfCenterStep(value.toDouble() * 1e3);
			break;
		case KEY_FUNC_FREQ_CFSTEP_HZ:
			if (isOk)
				sysScpi->setFrequencyOfCenterStep(value.toDouble());
			break;

			//span
		case KEY_FUNC_SPAN_SPAN_GHZ:
			if (isOk)
				sysScpi->setFrequencyOfSpan(value.toDouble() * 1e9);
			break;
		case KEY_FUNC_SPAN_SPAN_MHZ:
			if (isOk)
				sysScpi->setFrequencyOfSpan(value.toDouble() * 1e6);
			break;
		case KEY_FUNC_SPAN_SPAN_KHZ:
			if (isOk)
				sysScpi->setFrequencyOfSpan(value.toDouble() * 1e3);
			break;
		case KEY_FUNC_SPAN_SPAN_HZ:
			if (isOk)
				sysScpi->setFrequencyOfSpan(value.toDouble());
			break;

			//trig
		case KEY_FUNC_TRIG_VIDEO_DBM:
			if (isOk)
			{
		//		if (sysData.ampt.refUnit == ruW || sysData.ampt.refUnit == ruV)
		//		{
		//			sysScpi->setVideoTrigPower(value.toDouble() * 1e3);
	  //		} else
		//		{
		//			sysScpi->setVideoTrigPower(value.toDouble());
		//		}
				sysScpi->setVideoTrigPower(value.toDouble());
			}
			break;

			//ref
		case KEY_FUNC_AMPT_REFLEVEL_DBM:
			if (isOk)
				sysScpi->setAmptOfRefLevel(value.toDouble());
			break;
		case KEY_FUNC_AMPT_ATT_DB:
			if (isOk)
				sysScpi->setAmptOfAtt(value.toDouble());
			break;
		case KEY_FUNC_AMPT_SCALEDIV_DB:
			if (isOk)
				sysScpi->setAmptOfScaleDiv(value.toDouble());
			break;
		case KEY_FUNC_AMPT_REFOFFSET_DB:
			if (isOk)
				sysScpi->setAmptOfRefOffset(value.toDouble());
			break;

			//rbw
		case KEY_FUNC_BW_RBW_GHZ:
			if (isOk)
				sysScpi->setBwOfRbw(value.toDouble() * 1e9);
			break;
		case KEY_FUNC_BW_RBW_MHZ:
			if (isOk)
				sysScpi->setBwOfRbw(value.toDouble() * 1e6);
			break;
		case KEY_FUNC_BW_RBW_KHZ:
			if (isOk)
				sysScpi->setBwOfRbw(value.toDouble() * 1e3);
			break;
		case KEY_FUNC_BW_RBW_HZ:
			if (isOk)
				sysScpi->setBwOfRbw(value.toDouble());
			break;
			//vbw
		case KEY_FUNC_BW_VBW_GHZ:
			if (isOk)
				sysScpi->setBwOfVbw(value.toDouble() * 1e9);
			break;
		case KEY_FUNC_BW_VBW_MHZ:
			if (isOk)
				sysScpi->setBwOfVbw(value.toDouble() * 1e6);
			break;
		case KEY_FUNC_BW_VBW_KHZ:
			if (isOk)
				sysScpi->setBwOfVbw(value.toDouble() * 1e3);
			break;
		case KEY_FUNC_BW_VBW_HZ:
			if (isOk)
				sysScpi->setBwOfVbw(value.toDouble());
			break;
		case KEY_FUNC_BW_AVERAGE_ENTER:
			if (isOk)
				sysScpi->setBwOfAverage(value.toDouble());
			break;

			//sweep time
		case KEY_FUNC_SWEEP_SWEEPTIME_S:
		case KEY_FUNC_SOURCE_TRACKGEN_NETWORK_SWEEPTIME_S:
			if (isOk)
			{
				sysScpi->setSweepOfTime(value.toDouble() * 1e9);
			}
			break;
		case KEY_FUNC_SWEEP_SWEEPTIME_MS:
		case KEY_FUNC_SOURCE_TRACKGEN_NETWORK_SWEEPTIME_MS:
			if (isOk)
			{
				sysScpi->setSweepOfTime(value.toDouble() * 1e6);
			}
			break;
		case KEY_FUNC_SWEEP_SWEEPTIME_US:
		case KEY_FUNC_SOURCE_TRACKGEN_NETWORK_SWEEPTIME_US:
			if (isOk)
			{
				sysScpi->setSweepOfTime(value.toDouble() * 1e3);
			}
			break;
		case KEY_FUNC_SWEEP_SWEEPTIME_NS:
		case KEY_FUNC_SOURCE_TRACKGEN_NETWORK_SWEEPTIME_NS:
			if (isOk)
			{
				sysScpi->setSweepOfTime(value.toDouble());
			}
			break;

			//sweep points
		case KEY_FUNC_SWEEP_SWEEPPOINTS_ENTER:
		case KEY_FUNC_SOURCE_TRACKGEN_NETWORK_SWEEPPOINT_ENTER:
			if (isOk)
			{
				sysScpi->setSweepOfPoints(value.toDouble());
			}
			break;

			//display
		case KEY_FUNC_DISPLAY_DISPLAYLINE_DBM:
			if (isOk)
			{
				if (sysData.ampt.refUnit == ruW || sysData.ampt.refUnit == ruV)
				{
					sysScpi->setDisplayOfLineValue(value.toDouble() * 1e3);
				} else
				{
					sysScpi->setDisplayOfLineValue(value.toDouble());
				}
			}
			break;

		case KEY_FUNC_MEAS_DISCRIMINATOR_AMPLITUDE_OUTPUTPOWER_ENTER:
			if (isOk)
			{
				sysScpi->setMeasureOfDiscOutputPower(value.toDouble());
			}
			break;
		case KEY_FUNC_MEAS_DISCRIMINATOR_AMPLITUDE_SCALEDIV_ENTER:
			if (isOk)
			{
				sysScpi->setMeasureOfDiscScaleDiv(value.toDouble());
			}
			break;
		case KEY_FUNC_MEAS_DISCRIMINATOR_SWEEPTIME_S:
			if (isOk)
			{
				sysScpi->setMeasureOfDiscSweepTime(value.toDouble() * 1e9);
			}
			break;
		case KEY_FUNC_MEAS_DISCRIMINATOR_SWEEPTIME_MS:
			if (isOk)
			{
				sysScpi->setMeasureOfDiscSweepTime(value.toDouble() * 1e6);
			}
			break;
		case KEY_FUNC_MEAS_DISCRIMINATOR_SWEEPTIME_US:
			if (isOk)
			{
				sysScpi->setMeasureOfDiscSweepTime(value.toDouble() * 1e3);
			}
			break;
		case KEY_FUNC_MEAS_DISCRIMINATOR_SWEEPTIME_NS:
			if (isOk)
			{
				sysScpi->setMeasureOfDiscSweepTime(value.toDouble());
			}
			break;
		case KEY_FUNC_MEAS_DISCRIMINATOR_AMPLITUDE_OFFSET_ENTER:
			if (isOk)
			{
				sysScpi->setMeasureOfDiscDCOffset(value.toDouble());
			}
			break;

			//measure setup
		case KEY_FUNC_MEASSETUP_CHANNELWIDTH_GHZ:
			if (isOk)
			{
				sysScpi->setMeasSetupOfChannelWidth(value.toDouble() * 1e9);
			}
			break;
		case KEY_FUNC_MEASSETUP_CHANNELWIDTH_MHZ:
			if (isOk)
			{
				sysScpi->setMeasSetupOfChannelWidth(value.toDouble() * 1e6);
			}
			break;
		case KEY_FUNC_MEASSETUP_CHANNELWIDTH_KHZ:
			if (isOk)
			{
				sysScpi->setMeasSetupOfChannelWidth(value.toDouble() * 1e3);
			}
			break;
		case KEY_FUNC_MEASSETUP_CHANNELWIDTH_HZ:
			if (isOk)
			{
				sysScpi->setMeasSetupOfChannelWidth(value.toDouble());
			}
			break;

		case KEY_FUNC_MEASSETUP_CHANNELINTERVAL_GHZ:
			if (isOk)
			{
				sysScpi->setMeasSetupOfChannelSpace(value.toDouble() * 1e9);
			}
			break;
		case KEY_FUNC_MEASSETUP_CHANNELINTERVAL_MHZ:
			if (isOk)
			{
				sysScpi->setMeasSetupOfChannelSpace(value.toDouble() * 1e6);
			}
			break;
		case KEY_FUNC_MEASSETUP_CHANNELINTERVAL_KHZ:
			if (isOk)
			{
				sysScpi->setMeasSetupOfChannelSpace(value.toDouble() * 1e3);
			}
			break;
		case KEY_FUNC_MEASSETUP_CHANNELINTERVAL_HZ:
			if (isOk)
			{
				sysScpi->setMeasSetupOfChannelSpace(value.toDouble());
			}
			break;

		case KEY_FUNC_MEASSETUP_CHANNELNUMS_ENTER:
			if (isOk)
			{
				sysScpi->setMeasSetupOfChannelNums(value.toDouble());
			}
			break;
		case KEY_FUNC_MEASSETUP_POWERBWPERCENT_ENTER:
			if (isOk)
			{
				sysScpi->setMeasSetupOfPowerPercent(value.toDouble());
			}
			break;

			//marker normal
		case KEY_FUNC_MARKER_NORMAL_GHZ:
			if (isOk)
			{
				sysScpi->setMarkerXValue(sysData.markerSelIndex, value.toDouble() * 1e9);
			}
			break;
		case KEY_FUNC_MARKER_NORMAL_MHZ:
			if (isOk)
			{
				sysScpi->setMarkerXValue(sysData.markerSelIndex, value.toDouble() * 1e6);
			}
			break;
		case KEY_FUNC_MARKER_NORMAL_KHZ:
			if (isOk)
			{
				sysScpi->setMarkerXValue(sysData.markerSelIndex, value.toDouble() * 1e3);
			}
			break;
		case KEY_FUNC_MARKER_NORMAL_HZ:
			if (isOk)
			{
				sysScpi->setMarkerXValue(sysData.markerSelIndex, value.toDouble());
			}
			break;
			//marker delta
		case KEY_FUNC_MARKER_DELTA_GHZ:
			if (isOk)
			{
				sysScpi->setMarkerXValue(sysData.markerSelIndex, value.toDouble() * 1e9);
			}
			break;
		case KEY_FUNC_MARKER_DELTA_MHZ:
			if (isOk)
			{
				sysScpi->setMarkerXValue(sysData.markerSelIndex, value.toDouble() * 1e6);
			}
			break;
		case KEY_FUNC_MARKER_DELTA_KHZ:
			if (isOk)
			{
				sysScpi->setMarkerXValue(sysData.markerSelIndex, value.toDouble() * 1e3);
			}
			break;
		case KEY_FUNC_MARKER_DELTA_HZ:
			if (isOk)
			{
				sysScpi->setMarkerXValue(sysData.markerSelIndex, value.toDouble());
			}
			break;

		case KEY_FUNC_MARKERFCTN_NDB_ENTER:
			if (isOk)
			{
				sysScpi->setMarkerFunctionNdBValue(value.toDouble());
			}
			break;
		case KEY_FUNC_MARKERFCTN_FREQCOUNT_BW_GHZ:
			if (isOk)
			{
				sysScpi->setMarkerFunctionFreqCountBw(value.toDouble() * 1e9);
			}
			break;
		case KEY_FUNC_MARKERFCTN_FREQCOUNT_BW_MHZ:
			if (isOk)
			{
				sysScpi->setMarkerFunctionFreqCountBw(value.toDouble() * 1e6);
			}
			break;
		case KEY_FUNC_MARKERFCTN_FREQCOUNT_BW_KHZ:
			if (isOk)
			{
				sysScpi->setMarkerFunctionFreqCountBw(value.toDouble() * 1e3);
			}
			break;
		case KEY_FUNC_MARKERFCTN_FREQCOUNT_BW_HZ:
			if (isOk)
			{
				sysScpi->setMarkerFunctionFreqCountBw(value.toDouble());
			}
			break;

			//datetime
		case KEY_FUNC_SYSTEM_DATETIME_DATE_ENTER:
			code = sysScpi->setSystemDate(inputValue);

			if (code == _RESULT_SUCCESS)
			{
				outputMessage(getTheTranslatorString("date set successed"));
			} else
			{
				outputMessage((QString)("[") + intToQString(code, temp) + (QString)("] ") + getTheTranslatorString("date set failed") + ", " + getErrorMessage(code));
			}
			break;
		case KEY_FUNC_SYSTEM_DATETIME_TIME_ENTER:
			code = sysScpi->setSystemTime(inputValue);

			if (code == _RESULT_SUCCESS)
			{
				outputMessage(getTheTranslatorString("time set successed"));
			} else
			{
				outputMessage((QString)("[") + intToQString(code, temp) + (QString)("] ") + getTheTranslatorString("time set failed") + ", " + getErrorMessage(code));
			}
			break;

			//lan
		case KEY_FUNC_SYSTEM_CONFIGIO_LAN_IP_ENTER:
			if (!QHostAddress(value).isNull())
			{
				sysScpi->setLanIP(value);
			} else
			{
				outputMessage(getTheTranslatorString("address") + " " + value + " " + getTheTranslatorString("is invalid"));
			}
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_LAN_MASK_ENTER:
			if (!QHostAddress(value).isNull())
			{
				sysScpi->setLanMask(value);
			} else
			{
				outputMessage(getTheTranslatorString("address") + " " + value + " " + getTheTranslatorString("is invalid"));
			}
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_LAN_GATE_ENTER:
			if (!QHostAddress(value).isNull())
			{
				sysScpi->setLanGate(value);
			} else
			{
				outputMessage(getTheTranslatorString("address") + " " + value + " " + getTheTranslatorString("is invalid"));
			}
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_GPIB_ADDR_ENTER:
			if (isOk)
			{
				sysScpi->setGpibAddr(value.toInt());
			}
			break;
		case KEY_FUNC_SYSTEM_SERVICE_SERIALNO_ENTER:
			sysScpi->setSystemServiceOfSerialNo(value);
			break;

		case KEY_FUNC_SOURCE_TRACKGEN_POWER_ENTER:
			if (isOk)
			{
				sysScpi->setSourceTrackGenOutputPower(value.toDouble());
			}
			break;
		case KEY_FUNC_SOURCE_TRACKGEN_NETWORK_REFLEVEL_ENTER:
			if (isOk)
			{
				sysScpi->setSourceTrackGenNetworkReflevel(value.toDouble());
			}
			break;
		case KEY_FUNC_SOURCE_TRACKGEN_NETWORK_POWER_ENTER:
			if (isOk)
			{
				sysScpi->setSourceTrackGenNetworkOutputPower(value.toDouble());
			}
			break;
		case KEY_FUNC_SOURCE_TRACKGEN_VSWR_REFERENCE_ENTER:
			if (isOk)
			{
				sysScpi->setSourceTrackGenVSWRRefLevel(value.toDouble());
			}
			break;
		case KEY_FUNC_SOURCE_SIGNALGEN_FREQ_GHZ:
			if (isOk)
			{
				sysScpi->setSourceSignalGenOutputFreq(value.toDouble() * 1e9);
			}
			break;
		case KEY_FUNC_SOURCE_SIGNALGEN_FREQ_MHZ:
			if (isOk)
			{
				sysScpi->setSourceSignalGenOutputFreq(value.toDouble() * 1e6);
			}
			break;
		case KEY_FUNC_SOURCE_SIGNALGEN_FREQ_KHZ:
			if (isOk)
			{
				sysScpi->setSourceSignalGenOutputFreq(value.toDouble() * 1e3);
			}
			break;
		case KEY_FUNC_SOURCE_SIGNALGEN_FREQ_HZ:
			if (isOk)
			{
				sysScpi->setSourceSignalGenOutputFreq(value.toDouble());
			}
			break;
		case KEY_FUNC_SOURCE_SIGNALGEN_POWER_ENTER:
			if (isOk)
			{
				sysScpi->setSourceSignalGenOutputPower(value.toDouble());
			}
			break;

			//print nums
		case KEY_FUNC_PRINTSETUP_NUMS_ENTER:
			if (isOk)
			{
				sysScpi->setPrintOfNums(value.toDouble());
			}
			break;

			//demod
		case KEY_FUNC_DEMOD_SOUND_ENTER:
			if (isOk)
			{
				sysScpi->setDemodOfSound(value.toDouble());
			}
			break;

			//system
		case KEY_FUNC_SYSTEM_BACKLIGHT_ENTER:
			if (isOk)
			{
				sysScpi->setSystemLcdBacklight(value.toDouble());
			}
			break;
		case KEY_FUNC_SYSTEM_SERVICE_CALIBRATION_EDIT_9K_ENTER:
			if (isOk)
			{
				sysData.ampt.isPreamptOn = false;
				sysScpi->modifyFrequencyRespondOf9kHz(value.toDouble());
			}
			break;
		case KEY_FUNC_SYSTEM_SERVICE_CALIBRATION_EDIT_20K_ENTER:
			if (isOk)
			{
				sysData.ampt.isPreamptOn = false;
				sysScpi->modifyFrequencyRespondOf20kHz(value.toDouble());
			}
			break;
	}

	if (requireControl)
	{
		sysScpi->controlRf();
	}
}

//复位所有逻辑
void mainForm::resetAllLogic(void)
{
	resetFreqLogic();
	resetAmptScaleLineLogic();
	resetMarkerToLogic();
	resetPeakLogic();
	resetMeasSetupLogic();
	// resetAUXLogic();
	resetDemodLogic();
}

//复位频率逻辑
void mainForm::resetFreqLogic(void)
{
	if (nowMainKey == NULL || nowMainKey->keyValue != KEY_MAIN_FREQ || nowSubKey == NULL)
	{
		return;
	}

	subKeyDef* tempSubKey = nowSubKey;

	while (tempSubKey->upNode != NULL)
	{
		tempSubKey = tempSubKey->upNode;
	}

	while (tempSubKey != NULL)
	{
		if (sysScpi->isMeasureOn() == __SCPI_SUCCESS)
		{
			tempSubKey->isEnabled = tempSubKey->funcCode != KEY_FUNC_FREQ_START && tempSubKey->funcCode != KEY_FUNC_FREQ_STOP;
		} else
		{
			tempSubKey->isEnabled = true;
		}

		tempSubKey = tempSubKey->downNode;
	}
}

//复位幅度线性逻辑
void mainForm::resetAmptScaleLineLogic(void)
{
	if (nowMainKey == NULL || nowMainKey->keyValue != KEY_MAIN_AMPT || nowSubKey == NULL)
	{
		return;
	}

	subKeyDef* tempSubKey = nowSubKey;

	while (tempSubKey->upNode != NULL)
	{
		tempSubKey = tempSubKey->upNode;
	}

	while (tempSubKey != NULL)
	{
		if (tempSubKey->funcCode == KEY_FUNC_AMPT_SCALEDIV || tempSubKey->funcCode == KEY_FUNC_AMPT_REFUNIT)
		{
			tempSubKey->isEnabled = !sysData.ampt.scaleLine;
		}

		tempSubKey = tempSubKey->downNode;
	}
}

//复位MarkerTo逻辑
void mainForm::resetMarkerToLogic(void)
{
	if (nowMainKey == NULL || nowMainKey->keyValue != KEY_MAIN_MARKERTO || nowSubKey == NULL)
	{
		return;
	}

	subKeyDef* tempSubKey = nowSubKey;

	while (tempSubKey->upNode != NULL)
	{
		tempSubKey = tempSubKey->upNode;
	}

	while (tempSubKey != NULL)
	{
		if (sysData.span.span > 0)
		{
			tempSubKey->isEnabled = true;

			switch (tempSubKey->funcCode)
			{
				case KEY_FUNC_MARKERTO_DELTATOSPAN:
					tempSubKey->isEnabled = sysData.marker[sysData.markerSelIndex].state == msDelta;
					break;
				case KEY_FUNC_MARKERTO_DELTATOCF:
					tempSubKey->isEnabled = sysData.marker[sysData.markerSelIndex].state == msDelta;
					break;
			}
		} else
		{
			tempSubKey->isEnabled = tempSubKey->funcCode == KEY_FUNC_MARKERTO_REFLEVEL;
		}

		tempSubKey = tempSubKey->downNode;
	}
}

//复位Peak逻辑
void mainForm::resetPeakLogic(void)
{
	if (nowMainKey == NULL || nowMainKey->keyValue != KEY_MAIN_PEAK || nowSubKey == NULL)
	{
		return;
	}

	subKeyDef* tempSubKey = nowSubKey;

	while (tempSubKey->upNode != NULL)
	{
		tempSubKey = tempSubKey->upNode;
	}

	while (tempSubKey != NULL)
	{
		if (sysData.span.span > 0)
		{
			tempSubKey->isEnabled = true;
		} else
		{
			tempSubKey->isEnabled = tempSubKey->funcCode != KEY_FUNC_PEAK_MKRTOCF;
		}

		tempSubKey = tempSubKey->downNode;
	}
}

//复位测量设置逻辑
void mainForm::resetMeasSetupLogic(void)
{
	if (nowMainKey == NULL || nowMainKey->keyValue != KEY_MAIN_MEASSETUP || nowSubKey == NULL)
	{
		return;
	}

	subKeyDef* tempSubKey = nowSubKey;

	while (tempSubKey->upNode != NULL)
	{
		tempSubKey = tempSubKey->upNode;
	}

	if (sysScpi->isMeasureAllOff() == __SCPI_SUCCESS)
	{
		while (tempSubKey != NULL)
		{
			tempSubKey->isEnabled = false;
			tempSubKey = tempSubKey->downNode;
		}
	} else
	{
		if (sysData.measure.isAcprOn)
		{
			while (tempSubKey != NULL)
			{
				tempSubKey->isEnabled = tempSubKey->funcCode != KEY_FUNC_MEASSETUP_POWERBWPERCENT;
				tempSubKey = tempSubKey->downNode;
			}
		} else if (sysData.measure.isChannelPowerOn)
		{
			while (tempSubKey != NULL)
			{
				tempSubKey->isEnabled = tempSubKey->funcCode == KEY_FUNC_MEASSETUP_CHANNELWIDTH;
				tempSubKey = tempSubKey->downNode;
			}
		} else if (sysData.measure.isObwOn)
		{
			while (tempSubKey != NULL)
			{
				tempSubKey->isEnabled = tempSubKey->funcCode == KEY_FUNC_MEASSETUP_CHANNELWIDTH || tempSubKey->funcCode == KEY_FUNC_MEASSETUP_POWERBWPERCENT;
				tempSubKey = tempSubKey->downNode;
			}
		}
	}
}

//复位解调逻辑
void mainForm::resetDemodLogic(void)
{
	if (nowMainKey == NULL || nowMainKey->keyValue != KEY_MAIN_DEMOD || nowSubKey == NULL)
	{
		return;
	}

	subKeyDef* tempSubKey = nowSubKey;

	while (tempSubKey->upNode != NULL)
	{
		tempSubKey = tempSubKey->upNode;
	}

	while (tempSubKey != NULL)
	{
		if (tempSubKey->funcCode == KEY_FUNC_MODE_OPTIONS_DIGITAL_DEMOD)
		{
			tempSubKey->isEnabled = sysData.options.isDemodOn;
		}

		if (tempSubKey->funcCode == KEY_FUNC_MODE_OPTIONS_DEMOD_AUDIO)
		{
			tempSubKey->isEnabled = sysData.options.isDemodOn;
		}

		tempSubKey = tempSubKey->downNode;
	}
}

//获取文件文本
void mainForm::getFileLabel(void)
{
	QString labelText = "";
	char labelChar[32] = { };

	labelText = getTheTranslatorString("free disk space") + ":  " + QString(floatToStringDot2(sysData.file.freeDiskSpace * 100, labelChar)).trimmed() + "%";

	if (stLabelFileDiskSpace.text() != labelText)
	{
		stLabelFileDiskSpace.setText(labelText);
	}

	if (sysData.file.pages <= 0)
	{
		labelText = "0 / 0";
	} else
	{
		labelText = QString(intToString(sysData.file.nowPageIndex + 1, labelChar)) + " / " + QString(intToString(sysData.file.pages, labelChar));
	}

	if (stLabelFilePage.text() != labelText)
	{
		stLabelFilePage.setText(labelText);
	}

	labelText = getTheTranslatorString("No");
	if (stLabelFileRecord[0][0].text() != labelText)
		stLabelFileRecord[0][0].setText(labelText);
	labelText = getTheTranslatorString("Name");
	if (stLabelFileRecord[0][1].text() != labelText)
		stLabelFileRecord[0][1].setText(labelText);
	labelText = getTheTranslatorString("Type");
	if (stLabelFileRecord[0][2].text() != labelText)
		stLabelFileRecord[0][2].setText(labelText);
	labelText = getTheTranslatorString("Size(Bytes)");
	if (stLabelFileRecord[0][3].text() != labelText)
		stLabelFileRecord[0][3].setText(labelText);
	labelText = getTheTranslatorString("DateTime");
	if (stLabelFileRecord[0][4].text() != labelText)
		stLabelFileRecord[0][4].setText(labelText);

	int absoluteIndex = 0;

	for (int i = 1; i <= sysData.file.nowPageCount; i++)
	{
		absoluteIndex = sysData.file.perPage * sysData.file.nowPageIndex + i - 1;

		for (int j = 0; j < FILECOLCOUNT; j++)
		{
			if (j == 0)
			{
				labelText = QString(intToString(absoluteIndex + 1, labelChar)).trimmed();
			} else if (j == 1)
			{
				labelText = sysScpi->fileList.at(absoluteIndex).baseName();
			} else if (j == 2)
			{
				labelText = sysScpi->fileList.at(absoluteIndex).suffix();
			} else if (j == 3)
			{
				labelText = QString(intToString(sysScpi->fileList.at(absoluteIndex).size(), labelChar)).trimmed();
			} else if (j == 4)
			{
				labelText = sysScpi->fileList.at(absoluteIndex).lastModified().toString("yyyy-MM-dd hh:mm:ss");
			}

			if (stLabelFileRecord[i][j].text() != labelText)
			{
				stLabelFileRecord[i][j].setText(labelText);
			}
		}
	}
}

//获取矢量文本
void mainForm::getVectorLabel(void)
{
	QString labelText = "";
	char labelChar[32] = { };

	if (sysData.disc.isDiscOn)
	{
		labelText = getTheTranslatorString("Line") + " ";
		labelText += QString(floatToStringDot2(sysData.disc.scaleDiv, labelChar)).trimmed();
		labelText += " V/div";
	} else
	{
		if (sysData.ampt.scaleLine)
		{
			labelText = getTheTranslatorString("Line") + " ";
		} else
		{
			labelText = getTheTranslatorString("Log") + " ";
		}

		if (!sysData.ampt.scaleLine)
		{
			if (qAbs(sysData.ampt.scaleDiv - (int) sysData.ampt.scaleDiv) > 0)
			{
				labelText += QString(floatToStringDot2(sysData.ampt.scaleDiv, labelChar)).trimmed();
			} else
			{
				labelText += QString(intToString((int) sysData.ampt.scaleDiv, labelChar)).trimmed();
			}

			labelText += " dB/div";
		}
	}

	if (stLabel[0].text() != labelText)
	{
		stLabel[0].setText(labelText);
	}

	//c1r0: detector
	if (sysData.disc.isDiscOn)
	{
		labelText = "";
	} else
	{
		labelText = getTheTranslatorString("Det");
		labelText += " ";

		switch (sysData.detector.detMode)
		{
			case dmAuto:
				labelText += getTheTranslatorString("Auto");
				break;
			case dmNormal:
				labelText += getTheTranslatorString("Normal");
				break;
			case dmPosPeak:
				labelText += getTheTranslatorString("PosPeak");
				break;
			case dmNegPeak:
				labelText += getTheTranslatorString("NegPeak");
				break;
			case dmSample:
				labelText += getTheTranslatorString("Sample");
				break;
			case dmTransient:
				labelText += getTheTranslatorString("Transient");
				break;
			case dmQuasiPeak:
				labelText += getTheTranslatorString("QuasiPeak");
				break;
			case dmAverage:
				labelText += getTheTranslatorString("Average");
				break;
			case dmRMS:
				labelText += getTheTranslatorString("RMS");
				break;
			default:
				labelText += getTheTranslatorString("unKnown");
				break;
		}
	}

	if (stLabel[1].text() != labelText)
	{
		stLabel[1].setText(labelText);
	}

	//c2r0: empty
	labelText = getTheTranslatorString("");
	if (stLabel[2].text() != labelText)
	{
		stLabel[2].setText(labelText);
	}

	//c3r0: marker x
	if (sysData.marker[sysData.markerSelIndex].state != msClose)
	{
		labelText = getTheMarkerXString(sysData.markerSelIndex, true);
	} else
	{
		labelText = getTheTranslatorString("");
	}

	if (stLabel[3].text() != labelText)
	{
		stLabel[3].setText(labelText);
	}

	//c0r1: ref level
	labelText = getTheTranslatorString("Ref");
	labelText += " ";

	if (sysData.disc.isDiscOn)
	{
		labelText += QString(floatToStringDot2(sysData.disc.scaleDiv * 5, labelChar)).trimmed() + " V";
	} else if (sysData.source.isTrackGenNetworkOn)
	{
		labelText = getTheTranslatorString("Reference");
		labelText += " ";
		labelText += QString(floatToStringDot2(sysData.source.trackGenNetworkRefLevel, labelChar)).trimmed() + " dB";
	} else if (sysData.source.isVswrOn)
	{
		labelText = getTheTranslatorString("Reference");
		labelText += " ";
		labelText += QString(floatToStringDot2(sysData.source.vswrRef, labelChar)).trimmed() + " dB";
	} else
	{
		labelText += QString(sysScpi->getAmptAndUnitString(sysData.ampt.refLevel, labelChar, 1)).trimmed();
	}

	if (stLabel[4].text() != labelText)
	{
		stLabel[4].setText(labelText);
	}

	//c1r1: att
	if (sysData.disc.isDiscOn)
	{
		labelText = getTheTranslatorString("InputGain");
		labelText += " ";
		switch (sysData.disc.inputGain)
		{
			case igtx2:
				labelText += "×2";
				break;
			case igtx4:
				labelText += "×4";
				break;
			case igtx05:
				labelText += "×0.5";
				break;
			case igtx025:
				labelText += "×0.25";
				break;
			default:
				labelText += "×1";
				break;
		}
	} else if (sysData.source.isTrackGenNetworkOn)
	{
		//输出功率显示
		labelText = getTheTranslatorString("Power");
		labelText += " ";
		labelText += QString(floatToStringDot2(sysData.source.trackGenOutputPower, labelChar)).trimmed() + " dBm";
	} else
	{
		labelText = getTheTranslatorString("Att");
		labelText += " ";
		labelText += QString(intToString((unsigned int) sysData.ampt.att, labelChar)).trimmed();
		labelText += " dB";
	}

	if (stLabel[5].text() != labelText)
	{
		stLabel[5].setText(labelText);
	}

	//c2r1: empty
	if (sysData.bw.bwAverageOn)
	{
		labelText = getTheTranslatorString("Average") + " " + QString(intToString(sysData.bw.bwAverageFlag, labelChar)).trimmed();
	} else if (sysData.measure.isChannelPowerOn)
	{
		labelText = getTheTranslatorString("ChPower") + " " + QString(floatToString(sysData.measure.channelPower, 0, 2, labelChar)).trimmed() + " dBm";
	} else
	{
		labelText = getTheTranslatorString("");
	}
	if (stLabel[6].text() != labelText)
	{
		stLabel[6].setText(labelText);
	}

	//c3r1: marker y
	if (sysData.marker[sysData.markerSelIndex].state != msClose)
	{
		labelText = getTheMarkerYString(sysData.markerSelIndex);
	} else
	{
		labelText = getTheTranslatorString("");
	}

	if (stLabel[7].text() != labelText)
	{
		stLabel[7].setText(labelText);
	}

	//label: center or start freq
	if (sysData.freq.isShowCenter)
	{
		labelText = getTheTranslatorString("Center");
		labelText += " ";
		labelText += QString(getFreqAndUnitString(sysData.freq.center, 0, labelChar)).trimmed();
	} else
	{
		labelText = getTheTranslatorString("Start");
		labelText += " ";
		labelText += QString(getFreqAndUnitString(sysData.freq.start, 0, labelChar)).trimmed();
	}
	if (stLabel[8].text() != labelText)
	{
		stLabel[8].setText(labelText);
	}

	//label: empty
	labelText = getTheTranslatorString("");
	if (stLabel[9].text() != labelText)
	{
		stLabel[9].setText(labelText);
	}

	//label: span or stop freq
	if (sysData.freq.isShowCenter)
	{
		labelText = getTheTranslatorString("Span");
		labelText += " ";
		labelText += QString(getFreqAndUnitString(sysData.span.span, 0, labelChar)).trimmed();
	} else
	{
		labelText = getTheTranslatorString("Stop");
		labelText += " ";
		labelText += QString(getFreqAndUnitString(sysData.freq.stop, 0, labelChar)).trimmed();
	}
	if (stLabel[10].text() != labelText)
	{
		stLabel[10].setText(labelText);
	}

	//label: rbw
	labelText = getTheTranslatorString("RBW");
	labelText += " ";
	labelText += QString(getFreqAndUnitString(sysData.bw.rbw, 0, labelChar)).trimmed();

	if (stLabel[11].text() != labelText)
	{
		stLabel[11].setText(labelText);
	}

	//label: vbw
	labelText = getTheTranslatorString("VBW");
	labelText += " ";
	labelText += QString(getFreqAndUnitString(sysData.bw.vbw, 0, labelChar)).trimmed();
	if (stLabel[12].text() != labelText)
	{
		stLabel[12].setText(labelText);
	}

	//label: sweep time
	labelText = getTheTranslatorString("Sweep Time");
	labelText += " ";

	if (sysData.disc.isDiscOn)
	{
		labelText += QString(getMsTimeAndUnitString(sysData.disc.sweepTime, labelChar)).trimmed();
	} else
	{
		labelText += QString(getMsTimeAndUnitString(sysData.sweep.sweepTime, labelChar)).trimmed();
	}

	if (stLabel[13].text() != labelText)
	{
		stLabel[13].setText(labelText);
	}

	if (nowMainKey != NULL && sysData.marker[sysData.markerSelIndex].state != msClose && (nowMainKey->keyValue == KEY_MAIN_MARKER || nowMainKey->keyValue == KEY_MAIN_MARKERTO || nowMainKey->keyValue == KEY_MAIN_PEAK))
	{
		labelText = getTheTranslatorString("Marker") + QString(intToString(sysData.markerSelIndex + 1, labelChar));

		if (sysData.marker[sysData.markerSelIndex].state == msDelta)
		{
			labelText += "△";
		}

		if (inputTag != labelText)
		{
			inputTag = labelText;
		}

		if (inputValue.isEmpty())
		{
			labelText += " " + getTheMarkerXString(sysData.markerSelIndex, false);

			if (stLabel[14].text() != labelText)
			{
				stLabel[14].setText(labelText);
			}

			labelText = getTheMarkerYString(sysData.markerSelIndex);

			if (stLabel[15].text() != labelText)
			{
				stLabel[15].setText(labelText);
			}
		} else
		{
			if (stLabel[14].text() != labelText)
			{
				stLabel[14].setText(labelText);
			}

			labelText = inputValue;

			if (stLabel[15].text() != labelText)
			{
				stLabel[15].setText(labelText);
			}
		}
	} else
	{
		if (!inputTag.isEmpty())
		{
			labelText = inputTag;

			if (stLabel[14].text() != labelText)
			{
				stLabel[14].setText(labelText);
			}

			if (inputValue.isEmpty())
			{
				labelText = tagValue;

				if (stLabel[15].text() != labelText)
				{
					stLabel[15].setText(labelText);
				}
			} else
			{
				labelText = inputValue;

				if (stLabel[15].text() != labelText)
				{
					stLabel[15].setText(labelText);
				}
			}
		}
	}

	if (sysData.measure.isChannelPowerOn)
	{
		labelText = getTheTranslatorString("Channel Power");
		labelText += " ";
		labelText += QString(sysScpi->getAmptAndUnitString(sysData.measure.channelPower, labelChar, 1)).trimmed();
		if (stLabel[17].text() != labelText)
		{
			stLabel[17].setText(labelText);
		}
	} else
	{
		labelText = "";

		if (stLabel[17].text() != labelText)
		{
			stLabel[17].setText(labelText);
		}
	}
}

//获取幅度标尺
void mainForm::getAmptGraticuleLabel(void)
{
	if (!sysData.display.isAmptGraticuleOn && !sysData.source.isTrackGenNetworkOn)
	{
		return;
	}

	QString labelText = "";
	char tempChar[32] = { };
	double topValue = 0;

	for (int i = 0; i < AMPTGRATICULECOUNT; i++)
	{
		if (sysData.disc.isDiscOn)
		{
			labelText = floatToString(sysData.disc.scaleDiv * 5 - sysData.disc.scaleDiv * (i + 1), 0, 2, tempChar);
			//labelText = intToString((int)(sysData.disc.scaleDiv * 5 - sysData.disc.scaleDiv * (i + 1)), tempChar);  //为了显示方便，将原来的floatToString  变为 intToString
		} else if (sysData.options.amOn)
		{
			labelText = intToString(100 - i * 20, tempChar);
			labelText += "%";
		} else if (sysData.source.isTrackGenNetworkOn)
		{
			topValue = sysData.source.trackGenNetworkRefLevel + 5 * sysData.ampt.scaleDiv;
			labelText = floatToString(topValue - (i + 1) * sysData.ampt.scaleDiv, 0, 2, tempChar);
		} else
		{
			if ((!sysData.ampt.scaleLine) && (sysData.ampt.refUnit == ruW || sysData.ampt.refUnit == ruV))
			{
				labelText = "";
			} else
			{
				labelText = floatToString(sysData.ampt.refLevel - sysData.ampt.scaleDiv * (i + 1), 0, 1, tempChar);
				//labelText = intToString((int)(sysData.ampt.refLevel - sysData.ampt.scaleDiv * (i + 1)), tempChar);
			}
		}

		if (stLabelGraticule[i].text() != labelText)
		{
			stLabelGraticule[i].setText(labelText);
		}
	}
}

//获取时间频谱文本
void mainForm::getTimeSpecLabel(void)
{
	if (!sysData.measure.isTimeSpectrumOn)
	{
		return;
	}

	QString labelText = "";
	char tempChar[32] = { };

	for (int i = 0; i < TIMESPECTEXTCOUNT; i++)
	{
		if ((!sysData.ampt.scaleLine) && (sysData.ampt.refUnit == ruW || sysData.ampt.refUnit == ruV))
		{
			labelText = "";
		} else
		{
			labelText = floatToString(sysData.ampt.refLevel - sysData.ampt.scaleDiv * i, 0, 1, tempChar);
		}

		if (stLabelTimeSpec[i].text() != labelText)
		{
			stLabelTimeSpec[i].setText(labelText);
		}
	}
}

//获取标记表格文本
void mainForm::getMarkerTableLabel(void)
{
	QString labelText = getTheTranslatorString("Marker");
	if (stLabelMarkerTable[0].text() != labelText)
	{
		stLabelMarkerTable[0].setText(labelText);
	}

	labelText = getTheTranslatorString("Trace");
	if (stLabelMarkerTable[1].text() != labelText)
	{
		stLabelMarkerTable[1].setText(labelText);
	}

	labelText = getTheTranslatorString("Type");
	if (stLabelMarkerTable[2].text() != labelText)
	{
		stLabelMarkerTable[2].setText(labelText);
	}

	labelText = getTheTranslatorString("X Axis");
	if (stLabelMarkerTable[3].text() != labelText)
	{
		stLabelMarkerTable[3].setText(labelText);
	}

	labelText = getTheTranslatorString("Amplitude");
	if (stLabelMarkerTable[4].text() != labelText)
	{
		stLabelMarkerTable[4].setText(labelText);
	}

	int labelIndex = 4;
	char tempChar[32] = { };

	for (int i = 0; i < MARKERCOUNT; i++)
	{
		if (sysData.marker[i].state != msClose)
		{
			for (int j = 0; j < 5; j++)
			{
				labelIndex++;

				if (j == 0)
				{
					labelText = QString(intToString(i + 1, tempChar)).trimmed();
				} else if (j == 1)
				{
					labelText = QString(intToString(sysData.marker[i].traceIndex + 1, tempChar)).trimmed();
				} else if (j == 2)
				{
					if (sysData.marker[i].state == msNormal)
					{
						labelText = getTheTranslatorString("Normal");
					} else if (sysData.marker[i].state == msDelta)
					{
						labelText = "△" + getTheTranslatorString("Delta");
					}
				} else if (j == 3)
				{
					labelText = getTheMarkerXString(i, false);
				} else if (j == 4)
				{
					labelText = getTheMarkerYString(i);
				}

				if (stLabelMarkerTable[labelIndex].text() != labelText)
				{
					stLabelMarkerTable[labelIndex].setText(labelText);
				}
			}
		} else
		{
			labelIndex += 5;
		}
	}
}

//获取邻道测量文本
void mainForm::getAcprLabel(void)
{
	QString labelText = "";
	char labelChar[32] = { };

	//Tx Channel
	labelText = getTheTranslatorString("Tx Channel");
	setStaticTextText(&stLabelAcpr[0], labelText);

	//BandWidth
	labelText = getTheTranslatorString("Band Width");
	setStaticTextText(&stLabelAcpr[1], labelText);

	//MHz
	labelText = QString(getFreqAndUnitString(sysData.measureSetup.channelWidth, 0, labelChar)).trimmed();
	setStaticTextText(&stLabelAcpr[2], labelText);

	//Power
	labelText = getTheTranslatorString("Power");
	setStaticTextText(&stLabelAcpr[3], labelText);

	//dBm
	labelText = QString(floatToStringDot2(sysData.measure.acprTxChannelPower, labelChar)).trimmed() + " dBm";
	setStaticTextText(&stLabelAcpr[4], labelText);

	if (sysData.measureSetup.channelNums >= 1)
	{
		//1st Adjacent Channel
		labelText = getTheTranslatorString("1st Adjacent Channel");
		setStaticTextText(&stLabelAcpr[5], labelText);

		//BandWidth
		labelText = getTheTranslatorString("Band Width");
		setStaticTextText(&stLabelAcpr[6], labelText);

		//MHz
		labelText = QString(getFreqAndUnitString(sysData.measureSetup.channelWidth, 0, labelChar)).trimmed();
		setStaticTextText(&stLabelAcpr[7], labelText);

		//Lower
		labelText = getTheTranslatorString("Lower");
		setStaticTextText(&stLabelAcpr[8], labelText);

		//dB
		labelText = QString(floatToStringDot2(sysData.measure.acprAdjacent[0].lower, labelChar)).trimmed() + " dB";
		setStaticTextText(&stLabelAcpr[9], labelText);

		//Spacing
		labelText = getTheTranslatorString("Spacing");
		setStaticTextText(&stLabelAcpr[10], labelText);

		//MHz
		labelText = QString(getFreqAndUnitString(sysData.measureSetup.channelSpace, 0, labelChar)).trimmed();
		setStaticTextText(&stLabelAcpr[11], labelText);

		//Upper
		labelText = getTheTranslatorString("Upper");
		setStaticTextText(&stLabelAcpr[12], labelText);

		//dB
		labelText = QString(floatToStringDot2(sysData.measure.acprAdjacent[0].upper, labelChar)).trimmed() + " dB";
		setStaticTextText(&stLabelAcpr[13], labelText);
	}

	if (sysData.measureSetup.channelNums >= 2)
	{
		//2nd Adjacent Channel
		labelText = getTheTranslatorString("2nd Adjacent Channel");
		setStaticTextText(&stLabelAcpr[14], labelText);

		//BandWidth
		labelText = getTheTranslatorString("Band Width");
		setStaticTextText(&stLabelAcpr[15], labelText);

		//MHz
		labelText = QString(getFreqAndUnitString(sysData.measureSetup.channelWidth, 0, labelChar)).trimmed();
		setStaticTextText(&stLabelAcpr[16], labelText);

		//Lower
		labelText = getTheTranslatorString("Lower");
		setStaticTextText(&stLabelAcpr[17], labelText);

		//dB
		labelText = QString(floatToStringDot2(sysData.measure.acprAdjacent[1].lower, labelChar)).trimmed() + " dB";
		setStaticTextText(&stLabelAcpr[18], labelText);

		//Spacing
		labelText = getTheTranslatorString("Spacing");
		setStaticTextText(&stLabelAcpr[19], labelText);

		//MHz
		labelText = QString(getFreqAndUnitString(sysData.measureSetup.channelSpace, 0, labelChar)).trimmed();
		setStaticTextText(&stLabelAcpr[20], labelText);

		//Upper
		labelText = getTheTranslatorString("Upper");
		setStaticTextText(&stLabelAcpr[21], labelText);

		//dB
		labelText = QString(floatToStringDot2(sysData.measure.acprAdjacent[1].upper, labelChar)).trimmed() + " dB";
		setStaticTextText(&stLabelAcpr[22], labelText);
	}

	if (sysData.measureSetup.channelNums >= 3)
	{
		//3rd Adjacent Channel
		labelText = getTheTranslatorString("3rd Adjacent Channel");
		setStaticTextText(&stLabelAcpr[23], labelText);

		//BandWidth
		labelText = getTheTranslatorString("Band Width");
		setStaticTextText(&stLabelAcpr[24], labelText);

		//MHz
		labelText = QString(getFreqAndUnitString(sysData.measureSetup.channelWidth, 0, labelChar)).trimmed();
		setStaticTextText(&stLabelAcpr[25], labelText);

		//Lower
		labelText = getTheTranslatorString("Lower");
		setStaticTextText(&stLabelAcpr[26], labelText);

		//dB
		labelText = QString(floatToStringDot2(sysData.measure.acprAdjacent[2].lower, labelChar)).trimmed() + " dB";
		setStaticTextText(&stLabelAcpr[27], labelText);

		//Spacing
		labelText = getTheTranslatorString("Spacing");
		setStaticTextText(&stLabelAcpr[28], labelText);

		//MHz
		labelText = QString(getFreqAndUnitString(sysData.measureSetup.channelSpace, 0, labelChar)).trimmed();
		setStaticTextText(&stLabelAcpr[29], labelText);

		//Upper
		labelText = getTheTranslatorString("Upper");
		setStaticTextText(&stLabelAcpr[30], labelText);

		//dB
		labelText = QString(floatToStringDot2(sysData.measure.acprAdjacent[2].upper, labelChar)).trimmed() + " dB";
		setStaticTextText(&stLabelAcpr[31], labelText);
	}
}

//获取占用带宽文本
void mainForm::getObwLabel(void)
{
	QString labelText = "";
	char labelChar[32] = { };

	//obw
	labelText = getTheTranslatorString("Obw");
	setStaticTextText(&stLabelObw[0], labelText);

	//obw bandwidth
	labelText = QString(getFreqAndUnitString(sysData.measure.obwBandWidth, 0, labelChar)).trimmed();
	setStaticTextText(&stLabelObw[1], labelText);

	//obw percent

	labelText = QString(floatToStringDot2(sysData.measureSetup.powerPercent, labelChar)).trimmed() + "%";
	setStaticTextText(&stLabelObw[2], labelText);

	//start freq label
	labelText = getTheTranslatorString("Start Freq");
	setStaticTextText(&stLabelObw[3], labelText);

	//start freq value
	labelText = QString(getFreqAndUnitString(sysData.measure.obwStartFreq, 0, labelChar)).trimmed();
	setStaticTextText(&stLabelObw[4], labelText);

	//start freq ampt
	labelText = QString(sysScpi->getAmptAndUnitString(sysData.measure.obwStartFreqPower, labelChar, 0)).trimmed();
	setStaticTextText(&stLabelObw[5], labelText);

	//stop freq label
	labelText = getTheTranslatorString("Stop Freq");
	setStaticTextText(&stLabelObw[6], labelText);

	//stop freq value
	labelText = QString(getFreqAndUnitString(sysData.measure.obwStopFreq, 0, labelChar)).trimmed();
	setStaticTextText(&stLabelObw[7], labelText);

	//stop freq ampt
	labelText = QString(sysScpi->getAmptAndUnitString(sysData.measure.obwStopFreqPower, labelChar, 0)).trimmed();
	setStaticTextText(&stLabelObw[8], labelText);
}

//获取指定标记X字符串
QString mainForm::getTheMarkerXString(int mkrIndex, bool name)
{
	QString result = "";
	QString tempStr = "";
	char labelChar[32] = { };

	if (sysData.markerFctn.isNdBOn)
	{
		if (name)
		{
			result = "N(";
			result += floatToString(sysData.markerFctn.ndBValue, 0, 0, labelChar);
			result += ")dB";
			result += "  ";
		}

		if(sysData.markerFctn.ndBFreq < 20)
		  result += QString(floatToStringDot1(sysData.markerFctn.ndBFreq, labelChar)).trimmed() + "Hz";
		else
		  result += QString(getFreqAndUnitString(sysData.markerFctn.ndBFreq, 0, labelChar)).trimmed();
	} else if (sysData.markerFctn.isFreqCountOn)
	{
		if (name)
		{
			result = getTheTranslatorString("Freq Count");
			result += "  ";
		}

		//result += QString(getFreqAndUnitString(sysData.markerFctn.freqCountValue, 0, labelChar)).trimmed();
		//频率读数与频率计数分辨率精度保持一致
		result += QString(getFCtFreqAndUnitString(sysData.markerFctn.freqCountValue, sysData.markerFctn.freqCountBw, labelChar)).trimmed();
	} else
	{
		if (name)
		{
			if (sysData.marker[mkrIndex].state == msNormal)
			{
				result = getTheTranslatorString("Mkr");
				result += QString(intToString(mkrIndex + 1, labelChar)).trimmed();
			} else if (sysData.marker[mkrIndex].state == msDelta)
			{
				result = getTheTranslatorString("Mkr");
				result += QString(intToString(mkrIndex + 1, labelChar)).trimmed();
				result += "△";
			}

			result += "  ";
		}

		if (sysData.span.span > 0)
		{
			result += QString(getFreqAndUnitString(sysScpi->getMarkerXValue(mkrIndex), 1, labelChar)).trimmed();
		} else
		{
			result += QString(getTimeAndUnitString(sysScpi->getMarkerXValue(mkrIndex), 1, labelChar)).trimmed();
		}
	}

	return result;
}

//获取指定标记Y字符串
QString mainForm::getTheMarkerYString(int mkrIndex)
{
	QString result = "";
	char labelChar[32] = { };

	if (sysData.markerFctn.isMkrNoiseOn)
	{
		result = getTheTranslatorString("Noise");
		result += "  ";

		if (sysData.marker[sysData.markerSelIndex].state == msNormal)
		{
			result += QString(sysScpi->getAmptAndUnitString(sysData.markerFctn.noiseValue, labelChar, 1)).trimmed();
			result += "/Hz";
		} else if (sysData.marker[sysData.markerSelIndex].state == msDelta)
		{
			result += QString(floatToStringDot2(sysData.markerFctn.noiseValue, labelChar)).trimmed();
			result += " dB/Hz";
		}
	} else if (sysData.source.isTrackGenNetworkOn)
	{
		result = QString(floatToStringDot2(sysScpi->getMarkerYValue(mkrIndex), labelChar)).trimmed();
		result += " dB";
	} else if (sysData.marker[mkrIndex].state == msNormal)
	{
		if (sysData.ampt.scaleLine)
		{
			result = QString(sysScpi->getAmptAndUnitString(sysScpi->getMarkerYValue(mkrIndex), labelChar, 1)).trimmed();
		} else
		{
			result = QString(sysScpi->getAmptAndUnitString(sysScpi->getRefLevelValue(sysScpi->getMarkerYValue(mkrIndex)), labelChar, 1)).trimmed();
		}
	} else if (sysData.marker[mkrIndex].state == msDelta)
	{
		if (sysData.disc.isDiscOn)
		{
			result = QString(floatToStringDot2(sysScpi->getMarkerYValue(mkrIndex), labelChar)).trimmed();
			result += " V";
		} else if (sysData.ampt.scaleLine)
		{
			result = QString(floatToStringDot3(sysScpi->getMarkerYValue(mkrIndex), labelChar)).trimmed();
			result += "%";
		} else
		{
			result = QString(floatToStringDot2(sysScpi->getMarkerYValue(mkrIndex), labelChar)).trimmed();
			result += " dB";
		}
	}

	return result;
}

//获取列2行1字符串
QString mainForm::getCol2Row1String(bool head, int mkrIndex)
{
	QString result = "";

	if (mkrIndex < 0 || mkrIndex >= MARKERCOUNT)
	{
		return result;
	}

	char temp[32] = { };

	if (sysData.bw.bwAverageOn)
	{
		result = QString(intToString(sysData.bw.bwAverageFlag, temp)).trimmed();

		if (head)
		{
			result = getTheTranslatorString("Average") + "  " + result;
		}
	} else if (sysData.measure.isChannelPowerOn)
	{
		result = QString(floatToStringDot2(sysData.measure.channelPower, temp)).trimmed() + " dBm";

		if (head)
		{
			result = getTheTranslatorString("ChPower") + "  " + result;
		}
	} else if (sysData.measure.isAcprOn)
	{
		result = QString(floatToStringDot2(sysData.measure.acprTxChannelPower, temp)).trimmed() + " dBm";

		if (head)
		{
			result = getTheTranslatorString("TxPower") + "  " + result;
		}
	} else if (sysData.measure.isObwOn)
	{
		result = QString(getFreqAndUnitString(sysData.measure.obwBandWidth, 0, temp)).trimmed();

		if (head)
		{
			result = getTheTranslatorString("Obw") + "  " + result;
		}
	}

	return result;
}

//获取数据颜色
QColor mainForm::getDataColor(double value)
{
	double topValue = sysData.ampt.refLevel;
	double bottomValue = topValue - sysData.ampt.scaleDiv * 10;
	double ampt = value;
	unsigned long RGB = 0;
	unsigned long RGB1 = 0;
	unsigned long RGB2 = 0;
	QColor color1 = Qt::black;
	QColor color2 = Qt::black;
	QColor resultColor = Qt::black;

	if (ampt < bottomValue)
	{
		ampt = bottomValue;
	} else if (ampt > topValue)
	{
		ampt = topValue;
	}

	double dataLimit[7];
	dataLimit[0] = topValue;
	dataLimit[6] = bottomValue;
	dataLimit[1] = dataLimit[0] - sysData.ampt.scaleDiv;
	dataLimit[2] = dataLimit[1] - sysData.ampt.scaleDiv * 2;
	dataLimit[3] = dataLimit[2] - sysData.ampt.scaleDiv * 2;
	dataLimit[4] = dataLimit[3] - sysData.ampt.scaleDiv * 2;
	dataLimit[5] = dataLimit[6] + sysData.ampt.scaleDiv;

	if ((ampt > dataLimit[1]) && (ampt <= dataLimit[0]))
	{
		color1 = Qt::darkRed;
		color2 = Qt::red;
		RGB1 = (color1.blue() << 16) + (color1.green() << 8) + (color1.red());
		RGB2 = (color2.blue() << 16) + (color2.green() << 8) + (color2.red());
		RGB = RGB1 + ((0x7f) * (ampt - dataLimit[0]) / (dataLimit[1] - dataLimit[0]));
		resultColor = QColor((RGB & 0xff) >> 0, (RGB & 0xffff) >> 8, (RGB & 0xffffff) >> 16);
	} else if ((ampt > dataLimit[2]) && (ampt <= dataLimit[1]))
	{
		color1 = Qt::red;
		color2 = Qt::yellow;
		RGB1 = (color1.blue() << 16) + (color1.green() << 8) + (color1.red());
		RGB2 = (color2.blue() << 16) + (color2.green() << 8) + (color2.red());
		RGB = RGB1 + ((unsigned char) (((0xff) * (ampt - dataLimit[1]) / (dataLimit[2] - dataLimit[1]))) << 8);
		resultColor = QColor((RGB & 0xff) >> 0, (RGB & 0xffff) >> 8, (RGB & 0xffffff) >> 16);
	} else if ((ampt > dataLimit[3]) && (ampt <= dataLimit[2]))
	{
		color1 = Qt::yellow;
		color2 = Qt::green;
		RGB1 = (color1.blue() << 16) + (color1.green() << 8) + (color1.red());
		RGB2 = (color2.blue() << 16) + (color2.green() << 8) + (color2.red());
		RGB = RGB1 - ((unsigned char) ((0xff) * (ampt - dataLimit[2]) / (dataLimit[3] - dataLimit[2])));
		resultColor = QColor((RGB & 0xff) >> 0, (RGB & 0xffff) >> 8, (RGB & 0xffffff) >> 16);
	} else if ((ampt > dataLimit[4]) && (ampt <= dataLimit[3]))
	{
		color1 = Qt::green;
		color2 = Qt::cyan;
		RGB1 = (color1.blue() << 16) + (color1.green() << 8) + (color1.red());
		RGB2 = (color2.blue() << 16) + (color2.green() << 8) + (color2.red());
		RGB = RGB1 + ((unsigned char) (((0xff) * (ampt - dataLimit[3]) / (dataLimit[4] - dataLimit[3]))) << 16);
		resultColor = QColor((RGB & 0xff) >> 0, (RGB & 0xffff) >> 8, (RGB & 0xffffff) >> 16);
	} else if ((ampt > dataLimit[5]) && (ampt <= dataLimit[4]))
	{
		color1 = Qt::cyan;
		color2 = Qt::blue;
		RGB1 = (color1.blue() << 16) + (color1.green() << 8) + (color1.red());
		RGB2 = (color2.blue() << 16) + (color2.green() << 8) + (color2.red());
		RGB = RGB1 - ((unsigned char) (((0xff) * (ampt - dataLimit[4]) / (dataLimit[5] - dataLimit[4]))) << 8);
		resultColor = QColor((RGB & 0xff) >> 0, (RGB & 0xffff) >> 8, (RGB & 0xffffff) >> 16);
	} else if ((ampt >= dataLimit[6]) && (ampt <= dataLimit[5]))
	{
		color1 = Qt::darkBlue;
		color2 = Qt::blue;
		RGB1 = (color1.blue() << 16) + (color1.green() << 8) + (color1.red());
		RGB2 = (color2.blue() << 16) + (color2.green() << 8) + (color2.red());
		RGB = RGB1 - ((unsigned char) (((0x7f) * (ampt - dataLimit[5]) / (dataLimit[6] - dataLimit[5]))) << 16);
		resultColor = QColor((RGB & 0xff) >> 0, (RGB & 0xffff) >> 8, (RGB & 0xffffff) >> 16);
	}

	return resultColor;
}

//设置文本
void mainForm::setStaticTextText(QStaticText* stLabel, QString value)
{
	if (stLabel == NULL || value.isNull())
	{
		return;
	}

	if (stLabel->text() != value)
	{
		stLabel->setText(value);
	}
}

//设置宽度
void mainForm::setStaticTextWidth(QStaticText* stLabel, double value)
{
	if (stLabel == NULL || value <= 0)
	{
		return;
	}

	int tWidth = (int) stLabel->textWidth();
	int oWidth = (int) value;

	if (tWidth != oWidth)
	{
		stLabel->setTextWidth(oWidth);
	}
}

bool mainForm::isInputPasswordValid(QString value)
{
	if (nowSubKey != NULL && nowSubKey->isPassword)
	{
		QString password = inputValue.toUpper();
		QString md5String = "";
		char temp[64] = { };

		if (strcmp(sysData.system.serialNo, SERIALNO) == 0)
		{
			md5String = QString(longLongIntToString(PASSWORD, temp)).trimmed();
		} else
		{
			md5String = sysScpi->getSystemServiceOfMD5(sysData.system.serialNo, true);
		}

		//	if (isNumberPassword && intPassword == PASSWORD && nowSubKey->rightNode != NULL)
		if (password == md5String.toUpper() && nowSubKey->rightNode != NULL)
		{
			nowSubKey = nowSubKey->rightNode;
			inputTag = "";
			tagValue = "";
			inputValue = "";

			if (nowSubKey->isInput)
			{
				inputTag = getTranslatorString(nowSubKey->enName, nowSubKey->chName);
				handleInputKey(nowSubKey->funcCode);
			}

			return true;
		}
	}

	return false;
}

//复位系统
void mainForm::presetSystem(bool factory)
{
	drawPreset();
	helpContent = getTheTranslatorString("reseting system......");
	drawHelp();
	sysData.isPreseting = true;

	systemParam powerOnParam = sysData.system.powerOnPreset.powerOnParam;
	systemParam presetParam = sysData.system.powerOnPreset.presetParam;

	if (factory)
	{
		sysScpi->presetSystemData();
		sysScpi->controlRf();
	} else
	{
		sysScpi->presetSystem();
	}

	sysData.system.powerOnPreset.powerOnParam = powerOnParam;
	sysData.system.powerOnPreset.presetParam = presetParam;

	nowMainKey = sysKey;

	if (nowMainKey != NULL)
	{
		nowSubKey = nowMainKey->rightNode;
	}

	sysData.isPreseting = false;
	helpContent = getTheTranslatorString("Ready");
	drawHelp();
}
