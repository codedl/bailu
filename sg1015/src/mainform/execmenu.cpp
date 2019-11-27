#include "mainform.h"
#include "stdlib.h"

double freqOld;
float amplOld;

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
	bool ret = false;
	switch (funcCode)
	{
		//频率
		case KEY_FUNC_FREQREFSTATE_OFF:
		case KEY_FUNC_FREQREFSTATE_ON:
		case KEY_FUNC_FREQREF10M_EXTERNAL:
		case KEY_FUNC_FREQREF10M_INTERRIOR:
			//幅度
		case KEY_FUNC_AMPLREFSTATE_OFF:
		case KEY_FUNC_AMPLREFSTATE_ON:
		case KEY_FUNC__AMPL_ALC_OFF:
		case KEY_FUNC__AMPL_ALC_ON:
		case KEY_FUNC_AMPLRESPONSE_FAST:
		case KEY_FUNC_AMPLRESPONSE_SLOW:
			//扫描
			//		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_MORE_PRESETLIST_YES:
			//		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_MORE_PRESETLIST_NO:
		case KEY_FUNC_SWEEP_MODE_OFF:
		case KEY_FUNC_SWEEP_MODE_FREQ:
		case KEY_FUNC_SWEEP_MODE_AMPL:
		case KEY_FUNC_SWEEP_MODE_FREQAMPL:
		case KEY_FUNC_SWEEP_TYPE_LIST:
		case KEY_FUNC_SWEEP_TYPE_STEP:
		case KEY_FUNC_SWEEP_REPEAT_SINGLE:
		case KEY_FUNC_SWEEP_REPEAT_CONT:
		case KEY_FUNC_SWEEP_MORE_SWEEPTRIGGER_FREERUN:
		case KEY_FUNC_SWEEP_MORE_SWEEPTRIGGER_EXT:
		case KEY_FUNC_SWEEP_MORE_SWEEPDIRECTION_DOWN:
		case KEY_FUNC_SWEEP_MORE_SWEEPDIRECTION_UP:
		case KEY_FUNC_SWEEP_MORE_SWEEPRETRACE_OFF:
		case KEY_FUNC_SWEEP_MORE_SWEEPRETRACE_ON:
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_MORE_PRESETLIST_YES:
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_MORE_PRESETLIST_NO:
			//模拟调制
		case KEY_FUNC_FCTN_ANALOG_SOURCE_INTERNAL:
		case KEY_FUNC_FCTN_ANALOG_SOURCE_EXTERNAL:
		case KEY_FUNC_FCTN_ANALOG_AM_STATE_ON:
		case KEY_FUNC_FCTN_ANALOG_AM_STATE_OFF:
		case KEY_FUNC_FCTN_ANALOG_FM_STATE_ON:
		case KEY_FUNC_FCTN_ANALOG_FM_STATE_OFF:
		case KEY_FUNC_FCTN_ANALOG_PM_STATE_ON:
		case KEY_FUNC_FCTN_ANALOG_PM_STATE_OFF:
			//数字调制
		case KEY_FUNC_FCTN_DIGITAL_STATE_ON:
		case KEY_FUNC_FCTN_DIGITAL_STATE_OFF:
		case KEY_FUNC_FCTN_DIGITAL_TYPE_ASK_2ASK:
		case KEY_FUNC_FCTN_DIGITAL_TYPE_FSK_2FSK:
		case KEY_FUNC_FCTN_DIGITAL_TYPE_FSK_4FSK:
		case KEY_FUNC_FCTN_DIGITAL_TYPE_FSK_8FSK:
		case KEY_FUNC_FCTN_DIGITAL_TYPE_PSK_2PSK:
		case KEY_FUNC_FCTN_DIGITAL_TYPE_PSK_4PSK:
		case KEY_FUNC_FCTN_DIGITAL_TYPE_PSK_8PSK:
		case KEY_FUNC_FCTN_DIGITAL_SOURCE_INTERNAL:
		case KEY_FUNC_FCTN_DIGITAL_SOURCE_EXTERNAL:
			//脉冲调制
		case KEY_FUNC_FCTN_PLUSE_STATE_OFF:
		case KEY_FUNC_FCTN_PLUSE_STATE_ON:
		case KEY_FUNC_FCTN_PLUSE_SOURCE_INTERNAL:
		case KEY_FUNC_FCTN_PLUSE_SOURCE_EXTERNAL:
		case KEY_FUNC_FCTN_PLUSE_METHOD_AUTO:
		case KEY_FUNC_FCTN_PLUSE_METHOD_TRIG:
			//低频输出
		case KEY_FUNC_FCTN_LF_STATE_ON:
		case KEY_FUNC_FCTN_LF_STATE_OFF:
		case KEY_FUNC_FCTN_LF_WAVEFORM_SINE:
		case KEY_FUNC_FCTN_LF_WAVEFORM_SQUARE:
		case KEY_FUNC_FCTN_LF_WAVEFORM_TRIANGULAR:
		case KEY_FUNC_FCTN_LF_WAVEFORM_SAWTOOTH:
			//上变频
		case KEY_FUNC_FCTN_EXTRF_EXTRF_INT:
		case KEY_FUNC_FCTN_EXTRF_EXTRF_EXT:
			//DRG
		case KEY_FUNC_FCTN_DRG_STATE_ON:
		case KEY_FUNC_FCTN_DRG_STATE_OFF:
		case KEY_FUNC_FCTN_DRG_MORE_WORKMODE_NORMAL:
		case KEY_FUNC_FCTN_DRG_MORE_WORKMODE_POSITIVE:
		case KEY_FUNC_FCTN_DRG_MORE_WORKMODE_NEGATIVE:
			//SCPI刷新
		case KEY_FUNC_AUX_SCPI_ON:
		case KEY_FUNC_AUX_SCPI_OFF:
			//中英文切换
		case KEY_FUNC_AUX_LANGUAGE_CHINESE:
		case KEY_FUNC_AUX_LANGUAGE_ENGLISH:

			ret = true;
			break;
		default:
			ret = false;
			break;
	}
	return ret;
}

bool mainForm::isReturnReturn(int funcCode)
{
	//	return funcCode == KEY_FUNC_SWEEP_RETURN;
}

//处理方向键
void mainForm::handleArrowKey(int funcCode, bool up)
{
	switch (funcCode)
	{
		case KEY_FUNC_FREQ:
		case KEY_FUNC_FREQREFSET:
		case KEY_FUNC_FREQOFFSET:
		case KEY_FUNC_FREQMULTIPLIER:

		case KEY_FUNC_AMPL:
			//	case KEY_FUNC_AMPL_ERROR:
		case KEY_FUNC_AMPLREFSET:
		case KEY_FUNC_AMPLOFFSET:
		case KEY_FUNC_AMPL_CABLE:

		case KEY_FUNC_SWEEP_STEP_MORE_STEPDWELL:
		case KEY_FUNC_SWEEP_STEP_FREQSTART:
		case KEY_FUNC_SWEEP_STEP_FREQSTOP:
		case KEY_FUNC_SWEEP_STEP_AMPLSTART:
		case KEY_FUNC_SWEEP_STEP_AMPLSTOP:
		case KEY_FUNC_SWEEP_STEP_MORE_POINTS:

		case KEY_FUNC_FCTN_ANALOG_AM_DEPTH:
		case KEY_FUNC_FCTN_ANALOG_FM_DEVIATION:
		case KEY_FUNC_FCTN_ANALOG_PM_PHASE:
		case KEY_FUNC_FCTN_ANALOG_AM_RATE:
		case KEY_FUNC_FCTN_ANALOG_FM_RATE:
		case KEY_FUNC_FCTN_ANALOG_PM_RATE:
		case KEY_FUNC_FCTN_DIGITAL_PARAMETER_ASKDEEP:
		case KEY_FUNC_FCTN_DIGITAL_PARAMETER_FSKOFFSET:
		case KEY_FUNC_FCTN_DIGITAL_PARAMETER_PSKDEVI:
		case KEY_FUNC_FCTN_DIGITAL_PARAMETER_RATE:

		case KEY_FUNC_FCTN_PLUSE_PERIOD:
		case KEY_FUNC_FCTN_PLUSE_WIDTH:
			//	case KEY_FUNC_FCTN_DIGITAL_ASK_RATE:
			//	case KEY_FUNC_FCTN_DIGITAL_FSK_RATE:
			//	case KEY_FUNC_FCTN_DIGITAL_PSK_RATE:
			//
			//	//case KEY_FUNC_FCTN_DIGITAL_PLUSE_PERIOD:
			//	//case KEY_FUNC_FCTN_DIGITAL_PLUSE_WIDTH:
			//	case KEY_FUNC_FCTN_DIGITAL_FSK_STEP:

		case KEY_FUNC_FCTN_LF_AMPL:
		case KEY_FUNC_FCTN_LF_FREQ:
		case KEY_FUNC_FCTN_DRG_UPLIMIT:
		case KEY_FUNC_FCTN_DRG_DOWNLIMIT:
		case KEY_FUNC_FCTN_DRG_MORE_RATE:
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_EDITITEM:
			//case KEY_FUNC_SWEEP_MORE_CONFIGLIST_MORE_GOTOROW:
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_GOTOROW:
		case KEY_FUNC_AUX_SERVICE_LANMACADDR:
			sysScpi->dirKeyToCursor(up);
			drawInputParamterNoParamter();
			break;

		case KEY_FUNC_SWEEP_MODE:
		case KEY_FUNC_SWEEP_TYPE:
		case KEY_FUNC_SWEEP_REPEAT:
		case KEY_FUNC_SWEEP_SINGLETRIGGER:
		case KEY_FUNC_SWEEP_CONFIGSTEP:
		case KEY_FUNC_SWEEP_MORE:
		case KEY_FUNC_SWEEP_MORE_RETURN:
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST:
		case KEY_FUNC_SWEEP_MORE_SWEEPTRIGGER:
		case KEY_FUNC_SWEEP_MORE_SWEEPDIRECTION:
		case KEY_FUNC_SWEEP_MORE_SWEEPRETRACE:
			if ((nowMainKey->keyValue == KEY_MAIN_SWEEP)) //当主键是sweep键，且子键是sweep的第一级子键（由case决定），翻页功能
			{
				if (up)
				{
					sysData.sweep.nowpagenum--;
					if (sysData.sweep.nowpagenum <= 1)
						sysData.sweep.nowpagenum = 1;
				} else
				{

					sysData.sweep.nowpagenum++;
					if (sysData.sweep.nowpagenum >= 2)
						sysData.sweep.nowpagenum = 2;
				}
				drawInputParamterNoParamter();
				drawSweep(sysData.sweep.type);
			}
			break;
		default:
			break;
	}
}

double value_old = 3.2e+9;

//处理脉冲旋钮键动作
void mainForm::handleRpgKey(int funcCode, int nums)
{
	int rowDisplay = 8; //行
	int lineDisplay = 4; //列
	int rowPitchOn = 0;
	int linePitchOn = 0;
	double freqRefValue = 0;
	double amplRefValue = 0;

	QString SendToMSG1030QS;
	char SendToMSG1030CH[50];

	if (handleFileSelect(nums > 0, nums))
	{
		return;
	}

	//系统服务的密码
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
		strcpy(sysData.value, inputValue.toStdString().c_str()); //zpc add
		drawInputParamterNoParamter();
		return;
	}

	double value;

	//  if(sysData.EnterPara.Display == true)
	switch (funcCode)
	{
		case KEY_FUNC_FCTN_EXTRF_FREQ:
			sysScpi->setExtRefFrequencyWithRpg(nums);

			//界面绘制
			drawInputParamterNoParamter();
			drawExfFref();
			break;
		case KEY_FUNC_FREQ:
			sysScpi->setFrequencyWithRpg(nums);

			//界面绘制
			drawFreqAndAmplMain();
			drawInputParamterNoParamter();
			//  	  drawFreqAndAmpl();
			drawFreqInParamter();
			break;

		case KEY_FUNC_FREQOFFSET:
			sysScpi->setFrequencyOffsetWithRpg(nums);
			drawFreqAndAmplMain();
			drawInputParamterNoParamter();
			//  	  drawFreqAndAmpl();
			drawFreqInParamter();
			break;

		case KEY_FUNC_FREQMULTIPLIER:
			sysScpi->setFrequencyMultipliterWithRpg(nums);
			drawFreqAndAmplMain();
			drawInputParamterNoParamter();
			//  	  drawFreqAndAmpl();
			drawFreqInParamter();
			break;

		case KEY_FUNC_AMPL:
			sysScpi->setAmplitudeWithRpg(nums);

			drawFreqAndAmplMain();
			drawInputParamterNoParamter();
			//   	  drawFreqAndAmpl();
			drawAmplInParamter();
			break;

		case KEY_FUNC_AMPLOFFSET:
			sysScpi->setAmplitudeOffsetWithRpg(nums);
			drawFreqAndAmplMain();
			drawInputParamterNoParamter();
			//  	  drawFreqAndAmpl();
			drawAmplInParamter();
			break;

		case KEY_FUNC_AMPL_CABLE:
			sysScpi->setAmplitudeErrorWithRpg(nums);
			drawFreqAndAmplMain();
			drawInputParamterNoParamter();
			drawAmplInParamter();
			break;

		case KEY_FUNC_SWEEP_STEP_FREQSTART:
			sysScpi->setStepStartFrequencyWithRpg(nums);

			//界面绘制
			/**********txl********************/
			drawInputParamterNoParamter();
			drawSweep(sysData.sweep.type);
			/**********txl********************/
			break;

		case KEY_FUNC_SWEEP_STEP_FREQSTOP:
			sysScpi->setStepStopFrequencyWithRpg(nums);

			//界面绘制
			drawInputParamterNoParamter();
			drawSweep(sysData.sweep.type);
			break;

		case KEY_FUNC_SWEEP_STEP_AMPLSTART:
			sysScpi->setStepStartAmplitudeWithRpg(nums);

			//界面绘制
			drawInputParamterNoParamter();
			drawSweep(sysData.sweep.type);
			break;

		case KEY_FUNC_SWEEP_STEP_AMPLSTOP:
			sysScpi->setStepStopAmplitudeWithRpg(nums);

			//界面绘制
			drawInputParamterNoParamter();
			drawSweep(sysData.sweep.type);
			break;

		case KEY_FUNC_SWEEP_STEP_MORE_STEPDWELL:
			sysScpi->setStepDwellWithRpg(nums);

			//界面绘制
			drawInputParamterNoParamter();
			drawSweep(sysData.sweep.type);
			break;

		case KEY_FUNC_SWEEP_STEP_MORE_POINTS:
			sysScpi->setStepPointsWithRpg(nums);

			//界面绘制
			drawInputParamterNoParamter();
			drawSweep(sysData.sweep.type);
			break;

		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_EDITITEM:
			if (sysData.sweep.list.selectLine == 0) //如果为频率值
			{
				sysData.sweep.list.data[sysData.sweep.list.selectRow].freq = sysScpi->setValueByRPG(sysData.sweep.list.data[sysData.sweep.list.selectRow].freq, nums, 0);
				if (sysData.sweep.list.data[sysData.sweep.list.selectRow].freq > 3 * 1e9)
					sysData.sweep.list.data[sysData.sweep.list.selectRow].freq = 3 * 1e9;
				else if (sysData.sweep.list.data[sysData.sweep.list.selectRow].freq < 0)
					sysData.sweep.list.data[sysData.sweep.list.selectRow].freq = 0;

				getFreqString(sysData.sweep.list.data[sysData.sweep.list.selectRow].freq, sysData.value);
				strcpy(sysData.unit, freqToQUnit(sysData.sweep.list.data[sysData.sweep.list.selectRow].freq).trimmed().toStdString().c_str());

				insertThousands(sysData.value);

				//界面绘制
				drawInputParamterNoParamter();
				drawsListData();
			} else if (sysData.sweep.list.selectLine == 1) //如果为幅度值
			{
				sysData.sweep.list.data[sysData.sweep.list.selectRow].ampl = sysScpi->setValueByRPG(sysData.sweep.list.data[sysData.sweep.list.selectRow].ampl, nums, 2);
				if (sysData.sweep.list.data[sysData.sweep.list.selectRow].ampl < -120)
					sysData.sweep.list.data[sysData.sweep.list.selectRow].ampl = -120;
				else if (sysData.sweep.list.data[sysData.sweep.list.selectRow].ampl > 10)
					sysData.sweep.list.data[sysData.sweep.list.selectRow].ampl = 10;

				//界面绘制
				floatToStringDot2(sysData.sweep.list.data[sysData.sweep.list.selectRow].ampl, sysData.value);

				drawInputParamterNoParamter();
				drawsListData();
			} else if (sysData.sweep.list.selectLine == 2) //如果为驻留时间
			{
				sysData.sweep.list.data[sysData.sweep.list.selectRow].dwell = sysScpi->setValueByRPG(sysData.sweep.list.data[sysData.sweep.list.selectRow].dwell, nums, 0);
				if (sysData.sweep.list.data[sysData.sweep.list.selectRow].dwell < 20)
					sysData.sweep.list.data[sysData.sweep.list.selectRow].dwell = 20;
				else if (sysData.sweep.list.data[sysData.sweep.list.selectRow].dwell > 50 * 1e3)
					sysData.sweep.list.data[sysData.sweep.list.selectRow].dwell = 50 * 1e3;
				getMsTimeValue(sysData.sweep.list.data[sysData.sweep.list.selectRow].dwell, sysData.value);
				strcpy(sysData.unit, timeToQUnit(sysData.sweep.list.data[sysData.sweep.list.selectRow].dwell).trimmed().toStdString().c_str());
				insertThousands(sysData.value);

				//界面绘制
				drawInputParamterNoParamter();
				drawsListData();
			}
			break;

			//case KEY_FUNC_SWEEP_MORE_CONFIGLIST_MORE_GOTOROW:
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_GOTOROW:
			sysData.cursorLocation = 0;
			sysData.sweep.list.allIndex = sysScpi->setValueByRPG(sysData.sweep.list.allIndex, nums, 0);

			if (sysData.sweep.list.allIndex < 0)
				sysData.sweep.list.allIndex = 0;

			if (sysData.sweep.list.allIndex > (sysData.sweep.list.allRow * 3 - 1))
				sysData.sweep.list.allIndex = sysData.sweep.list.allRow * 3 - 1;

			sysData.sweep.list.selectRow = sysData.sweep.list.allIndex / 3;
			sysData.sweep.list.selectLine = sysData.sweep.list.allIndex % 3;

			if (((sysData.sweep.list.selectRow + 1) * sysData.sweep.list.selectLine) > (sysData.sweep.list.allRow * 3))
			{
				sysData.sweep.list.selectRow = sysData.sweep.list.allRow;
				sysData.sweep.list.selectLine = 3;
			}

			sysData.sweep.list.displayPage = sysData.sweep.list.allIndex / 12;
			intToString(sysData.sweep.list.selectRow + 1, sysData.value);
			drawInputParamterNoParamter();
			drawsListData();
#if 0
			sysData.sweep.list.selectRow = sysScpi->setValueByRPG(sysData.sweep.list.selectRow, nums, 0);
			if(sysData.sweep.list.selectRow < 0)
			{
				sysData.sweep.list.selectRow = 0;
			} else if(sysData.sweep.list.selectRow > sysData.sweep.list.allRow-1)
			{
				sysData.sweep.list.selectRow = sysData.sweep.list.allRow - 1;
			}
			sysData.sweep.list.displayPage = sysData.sweep.list.selectRow / 4;
			sysData.sweep.list.allIndex = sysData.sweep.list.selectRow * 3 + sysData.sweep.list.selectLine;
			intToString((unsigned int)sysData.sweep.list.selectRow+1,sysData.value);

			drawInputParamterNoParamter();
			drawsListData();
#endif
			break;

		case KEY_FUNC_FCTN_ANALOG_AM_DEPTH:
			sysScpi->setAmDepthWithRpg(nums);

			//界面绘制
			drawInputParamterNoParamter();
			drawAnalogModulate();
			break;

		case KEY_FUNC_FCTN_ANALOG_FM_DEVIATION:
			sysScpi->setFmDevWithRpg(nums);

			//界面绘制
			drawInputParamterNoParamter();
			drawAnalogModulate();
			break;

		case KEY_FUNC_FCTN_ANALOG_PM_PHASE:
			sysScpi->setPmPhaseWithRpg(nums);

			//界面绘制
			drawInputParamterNoParamter();
			drawAnalogModulate();
			break;

		case KEY_FUNC_FCTN_ANALOG_AM_RATE:
			sysScpi->setAmRateWithRpg(nums);

			//界面绘制
			drawInputParamterNoParamter();
			drawAnalogModulate();
			break;

		case KEY_FUNC_FCTN_ANALOG_FM_RATE:
			sysScpi->setFmRateWithRpg(nums);

			//界面绘制
			drawInputParamterNoParamter();
			drawAnalogModulate();
			break;

		case KEY_FUNC_FCTN_ANALOG_PM_RATE:
			sysScpi->setPmRateWithRpg(nums);

			//界面绘制
			drawInputParamterNoParamter();
			drawAnalogModulate();
			break;

		case KEY_FUNC_FCTN_DIGITAL_PARAMETER_ASKDEEP:
			sysScpi->setDigitalModulateParaWithRpg(nums);
			drawInputParamterNoParamter();
			drawDigitalModulate();
			break;

		case KEY_FUNC_FCTN_DIGITAL_PARAMETER_FSKOFFSET:
			sysScpi->setDigitalModulateParaWithRpg(nums);
			drawInputParamterNoParamter();
			drawDigitalModulate();
			break;

		case KEY_FUNC_FCTN_DIGITAL_PARAMETER_PSKDEVI:
			sysScpi->setDigitalModulateParaWithRpg(nums);
			drawInputParamterNoParamter();
			drawDigitalModulate();
			break;

		case KEY_FUNC_FCTN_DIGITAL_PARAMETER_RATE:
			sysScpi->setDigitalModuateRateWithRpg(nums);
			drawInputParamterNoParamter();
			drawDigitalModulate();
			break;

			//  		case KEY_FUNC_FCTN_DIGITAL_ASK_RATE:
			//  		  sysScpi->setAskRateWithRpg(nums);
			//
			//  		  //界面绘制
			//  		  drawInputParamterNoParamter();
			//  		  drawDigitalModulate();
			//  		  break;

			//  		case KEY_FUNC_FCTN_DIGITAL_FSK_RATE:
			//  		  sysScpi->setFskRateWithRpg(nums);
			//
			//  		  //界面绘制
			//  		  drawInputParamterNoParamter();
			//  		  drawDigitalModulate();
			//  		  break;

			//  		case KEY_FUNC_FCTN_DIGITAL_PSK_RATE:
			//  		  sysScpi->setPskRateWithRpg(nums);
			//
			//  		  //界面绘制
			//  		  drawInputParamterNoParamter();
			//  		  drawDigitalModulate();
			//  		  break;

			//  		case KEY_FUNC_FCTN_DIGITAL_FSK_STEP:
			//  		  sysScpi->setFskStepWithRpg(nums);
			//
			//  		  //界面绘制
			//  		  drawInputParamterNoParamter();
			//  		  drawDigitalModulate();
			//  		  break;

			/*case KEY_FUNC_FCTN_DIGITAL_PLUSE_PERIOD:
			 sysScpi->setPlusePeriodWithRpg(nums);

			 //界面绘制
			 drawInputParamterNoParamter();
			 drawAnalogModulate();
			 break;*/

			/*case KEY_FUNC_FCTN_DIGITAL_PLUSE_WIDTH:
			 sysScpi->setPluseWidthWithRpg(nums);

			 //界面绘制
			 drawInputParamterNoParamter();
			 drawAnalogModulate();
			 break;*/

		case KEY_FUNC_FCTN_PLUSE_PERIOD:
			if (sysData.pluse.plusesource == psInternal)
			{
				sysScpi->setPlusePeriodWithRpg(nums);
				sprintf(sysData.value, doubleToQValue(sysData.pluse.period * 1e6).toStdString().c_str());
				insertThousands(sysData.value); //数组类型插入千分符
				sprintf(sysData.unit, timeToQUnit(sysData.pluse.period).toStdString().c_str());
				drawInputParamterNoParamter();
				drawPluseModulate();
			}
			break;

		case KEY_FUNC_FCTN_PLUSE_WIDTH:
			if (sysData.pluse.plusesource == psInternal)
			{
				sysScpi->setPluseWidthWithRpg(nums);
				sprintf(sysData.value, doubleToQValue(sysData.pluse.width * 1e6).toStdString().c_str());
				insertThousands(sysData.value); //数组类型插入千分符
				sprintf(sysData.unit, timeToQUnit(sysData.pluse.width).toStdString().c_str());
				drawInputParamterNoParamter();
				drawPluseModulate();
			}
			break;

		case KEY_FUNC_FCTN_LF_AMPL:
			sysScpi->setLfAmplWithRpg(nums);
			//		  sprintf(sysData.value, "%.0f", sysData.lf.ampl);
			//		  insertThousands(sysData.value);
			//		  sprintf(sysData.unit, vppToQUnit(sysData.lf.ampl).toStdString().c_str());

			//界面绘制
			drawInputParamterNoParamter();
			drawArbitraryWave();
			break;

		case KEY_FUNC_FCTN_LF_FREQ:
			sysScpi->setLfFreqWithRpg(nums);

			//界面绘制
			drawInputParamterNoParamter();
			drawArbitraryWave();
			break;
		case KEY_FUNC_FCTN_DRG_UPLIMIT:
			sysScpi->setDrgUplimitWithRpg(nums);
			drawInputParamterNoParamter();
			drawDrgFunction();
			break;
		case KEY_FUNC_FCTN_DRG_DOWNLIMIT:
			sysScpi->setDrgDownlimitWithRpg(nums);
			drawInputParamterNoParamter();
			drawDrgFunction();
			break;
		case KEY_FUNC_FCTN_DRG_POINTS:
			sysScpi->setDrgPointsWithRpg(nums);
			drawInputParamterNoParamter();
			drawDrgFunction();
			break;
		case KEY_FUNC_FCTN_DRG_MORE_RATE:
			sysScpi->setDrgRateWithRpg(nums * 20);
			drawInputParamterNoParamter();
			drawDrgFunction();
			break;
#if 0
			case KEY_FUNC_SWEEP_MORE_CONFIGLIST_SELECTITEM:
			//  		  sysData.EnterPara.Value = sysData.Sweep.List.AllIndex;
			sysData.cursorLocation = 0;
			sysData.sweep.list.allIndex = sysScpi->setValueByRPG(sysData.sweep.list.allIndex, nums, 0);

			if(sysData.sweep.list.allIndex < 0) sysData.sweep.list.allIndex = 0;

			if(sysData.sweep.list.allIndex > (sysData.sweep.list.allRow * 3 - 1))
			sysData.sweep.list.allIndex = sysData.sweep.list.allRow * 3 - 1;

			sysData.sweep.list.selectRow = sysData.sweep.list.allIndex / 3;
			sysData.sweep.list.selectLine = sysData.sweep.list.allIndex % 3;

			if(((sysData.sweep.list.selectRow + 1) * sysData.sweep.list.selectLine) > (sysData.sweep.list.allRow * 3))
			{
				sysData.sweep.list.selectRow = sysData.sweep.list.allRow;
				sysData.sweep.list.selectLine = 3;
			}

			sysData.sweep.list.displayPage = sysData.sweep.list.allIndex / 12;

			drawsListData();
			break;
#endif

		case KEY_FUNC_AUX_SERVICE_LANMACADDR:
			sysScpi->setSystemServiceOfLanMacAddressWidthRpg(nums);
			drawInputParamterNoParamter();
			drawSystemPara();
			break;

		default:
			break;
	}
}

//在列表扫描数据中插入一行数据
void mainForm::insertRow(int Row)
{
	//  for(int i = sysData.sweep.list.allRow - 1; i >= Row; i--)
	//  {
	//	sysData.sweep.list.data[i+1].freq = sysData.sweep.list.data[i].freq;
	//	sysData.sweep.list.data[i+1].ampl = sysData.sweep.list.data[i].ampl;
	//	sysData.sweep.list.data[i+1].dwell = sysData.sweep.list.data[i].dwell;
	//  }

	//  sysData.sweep.list.data[Row].freq = sysData.freq.downLoad;
	//  sysData.sweep.list.data[Row].ampl = sysData.ampl.downLoad;
	//  sysData.sweep.list.data[Row].dwell = 20;
	if (Row == sysData.sweep.list.allRow)
	{
		sysData.sweep.list.data[Row - 1].freq = sysData.sweep.list.data[Row - 2].freq;
		sysData.sweep.list.data[Row - 1].ampl = sysData.sweep.list.data[Row - 2].ampl;
		sysData.sweep.list.data[Row - 1].dwell = sysData.sweep.list.data[Row - 2].dwell;
	} else
	{
		for (int i = Row + 1; i < sysData.sweep.list.allRow; i++) //将插入行的下一行到最后一行后移
		{
			sysData.sweep.list.data[i].freq = sysData.sweep.list.data[i - 1].freq;
			sysData.sweep.list.data[i].ampl = sysData.sweep.list.data[i - 1].ampl;
			sysData.sweep.list.data[i].dwell = sysData.sweep.list.data[i - 1].dwell;
		}
		sysData.sweep.list.data[Row].freq = sysData.sweep.list.data[Row - 1].freq;
		sysData.sweep.list.data[Row].ampl = sysData.sweep.list.data[Row - 1].ampl;
		sysData.sweep.list.data[Row].dwell = sysData.sweep.list.data[Row - 1].dwell;
	}

	if (sysData.sweep.list.allRow == 1 && sysData.sweep.list.data[0].freq == 0)
	{
		sysData.sweep.list.data[0].freq = MAXFREQ;
		sysData.sweep.list.data[0].ampl = MINAMPL;
		sysData.sweep.list.data[0].dwell = 20;
	} else if (sysData.sweep.list.allRow == 2)
	{
		sysData.sweep.list.data[1].freq = sysData.sweep.list.data[0].freq;
		sysData.sweep.list.data[1].ampl = sysData.sweep.list.data[0].ampl;
		sysData.sweep.list.data[1].dwell = sysData.sweep.list.data[0].dwell;
	}
}

//在列表扫描数据中删除一行数据
void mainForm::deleteRow(int Row)
{
	for (int i = Row; i < sysData.sweep.list.allRow; i++)
	{
		sysData.sweep.list.data[i].freq = sysData.sweep.list.data[i + 1].freq;
		sysData.sweep.list.data[i].ampl = sysData.sweep.list.data[i + 1].ampl;
		sysData.sweep.list.data[i].dwell = sysData.sweep.list.data[i + 1].dwell;
	}
}

extern char *itoa(int i);

//处理执行键
void mainForm::handleExecuteKey(int funcCode)
{
	int i, j;
	char tempChar[64];
	char temp[64];
	QString name = "";
	QString value = "";
	QString unit = "";

	char SendToMSG1030CH[50];
	QString SendToMSG1030QS;
	QString SendToMSG1030QS1;

	switch (funcCode)
	{

		case KEY_FUNC_AUX_SERVICE_EVALUATE_ON:
			sysScpi->setSystemServiceOfEvalute(true);
			break;

		case KEY_FUNC_AUX_SERVICE_EVALUATE_OFF:
			sysScpi->setSystemServiceOfEvalute(false);
			break;

		case KEY_FUNC_AUX_LANGUAGE_CHINESE:
			sysScpi->setSystemLanguage(0);
			break;

		case KEY_FUNC_AUX_LANGUAGE_ENGLISH:
			sysScpi->setSystemLanguage(1);
			break;
		case KEY_FUNC_AUX_SCPI_ON:
			sysScpi->setScpiState(1);
			break;

		case KEY_FUNC_AUX_SCPI_OFF:
			sysScpi->setScpiState(0);
			break;

		case KEY_FUNC_FCTN_EXTRF_EXTRF_INT:
			sysScpi->setExtRefType(erftInternal);
			drawExfFref();
			break;
		case KEY_FUNC_FCTN_EXTRF_EXTRF_EXT:
			//      sysData.scpiData.stateS = false;
			sysScpi->setExtRefType(erftExternal);
			drawExfFref();
			break;

		case KEY_FUNC_FREQREFSET:
			sysData.sweep.mode = smOff;

			sysScpi->setFrequencyOfReference();

			sysData.inputLength = MAXFREQINPUTLENGTH;

			drawFreqAndAmplMain();
			drawInputParamterNoParamter();
			//	  drawFreqAndAmpl();
			drawFreqInParamter();
			break;
		case KEY_FUNC_AMPLREFSET:
			sysData.sweep.mode = smOff;
			sysScpi->setAmplitudeOfReference();
			//	  strcpy(sysData.unitAmpl,"dBm");
			sysData.inputLength = MAXAMPLINPUTLENGTH;

			drawFreqAndAmplMain();
			drawInputParamterNoParamter();
			//	  drawFreqAndAmpl();
			drawAmplInParamter();
			break;

		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_INSERTROW:
			/**********txl********************/
			//	  sysData.EnterPara.Display = false;
			sysData.sweep.list.allRow++;
			sysData.sweep.list.selectRow = sysData.sweep.list.selectRow + 1;
			sysData.sweep.list.allIndex += 3;

			if (sysData.sweep.list.allIndex > 1535)
			{
				sysData.sweep.list.allIndex = 1535;
			}

			if (sysData.sweep.list.selectRow > 511)
			{
				sysData.sweep.list.selectRow = 511;
			}

			if (sysData.sweep.list.allRow > 511)
			{
				sysData.sweep.list.allRow = 511;
				break;
			}

			sysData.sweep.list.displayPage = sysData.sweep.list.selectRow / 4;

			if ((sysData.sweep.list.allRow % 4) > 0)
			{
				sysData.sweep.list.allPage = sysData.sweep.list.allRow / 4 + 1;
			} else
			{
				sysData.sweep.list.allPage = sysData.sweep.list.allRow / 4;
			}

			sysData.sweep.list.allIndex = sysData.sweep.list.selectRow * 3 + sysData.sweep.list.selectLine;

			insertRow(sysData.sweep.list.selectRow);
			sysScpi->setSweepListDownload();
			drawInputParamterNoParamter();
			drawsListData();

			break;

		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_DELETEROW:
			/**********txl********************/
			//	  sysData.EnterPara.Display = false;
			sysData.sweep.list.allRow--;
			sysData.sweep.list.allIndex -= 3;

			if (sysData.sweep.list.allIndex < 0)
			{
				sysData.sweep.list.allIndex = 0;
			}

			if (sysData.sweep.list.selectRow < 0)
			{
				sysData.sweep.list.selectRow = 0;
			}

			if (sysData.sweep.list.allRow < 2)
			{
				sysData.sweep.list.allRow = 2;
				break;
			}

			deleteRow(sysData.sweep.list.selectRow);

			sysData.sweep.list.selectRow--;

			if (sysData.sweep.list.selectRow < 0)
			{
				sysData.sweep.list.selectRow = 0;
			}

			if (sysData.sweep.list.selectRow >= sysData.sweep.list.allRow)
			{
				sysData.sweep.list.selectRow = sysData.sweep.list.allRow - 1;
			}

			sysData.sweep.list.displayPage = sysData.sweep.list.selectRow / 4;

			if ((sysData.sweep.list.allRow % 4) > 0)
			{
				sysData.sweep.list.allPage = sysData.sweep.list.allRow / 4 + 1;
			} else
			{
				sysData.sweep.list.allPage = sysData.sweep.list.allRow / 4;
			}

			sysData.sweep.list.allIndex = sysData.sweep.list.selectRow * 3 + sysData.sweep.list.selectLine;

			sysScpi->setSweepListDownload();
			drawsListData();
			break;

		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_MORE_PRESETLIST_YES:
			sysData.sweep.list.allRow = 2;

			sysData.sweep.list.selectRow = 0;
			sysData.sweep.list.selectLine = 0;
			sysData.sweep.list.allIndex = 0;
			sysData.sweep.list.displayPage = 0;
			sysData.sweep.list.allPage = 1;

			/**********txl********************/
			sysData.sweep.list.data[0].freq = MAXFREQ;
			sysData.sweep.list.data[0].ampl = MINAMPL;
			sysData.sweep.list.data[0].dwell = 20;
			sysData.sweep.list.data[1].freq = MAXFREQ;
			sysData.sweep.list.data[1].ampl = MINAMPL;
			sysData.sweep.list.data[1].dwell = 20;

			//	  deleteRow(sysData.sweep.list.selectRow);

			/**********txl********************/
			drawInputParamterNoParamter();
			drawsListData();
			break;

		case KEY_FUNC_FREQREFSTATE_OFF:
			sysScpi->setFrequencyOfReferenceState(0);

			drawFreqAndAmplMain();
			//	  drawFreqAndAmpl();
			drawFreqInParamter();
			drawInputParamterNoParamter();
			break;

		case KEY_FUNC_FREQREFSTATE_ON:
			sysScpi->setFrequencyOfReferenceState(1);

			drawFreqAndAmplMain();
			//	  drawFreqAndAmpl();
			drawFreqInParamter();
			drawInputParamterNoParamter();
			break;

		case KEY_FUNC_AMPLREFSTATE_OFF:
			sysScpi->setAmplitudeOfReferenceState(0);

			drawFreqAndAmplMain();
			//	  drawFreqAndAmpl();
			drawAmplInParamter();
			drawInputParamterNoParamter();
			break;

		case KEY_FUNC_AMPLREFSTATE_ON:
			sysScpi->setAmplitudeOfReferenceState(1);

			drawFreqAndAmplMain();
			//	  drawFreqAndAmpl();
			drawAmplInParamter();
			drawInputParamterNoParamter();
			break;

		case KEY_FUNC_FREQREF10M_INTERRIOR:
			/**********txl********************/

			//	  //下发控制
			//	  printf("%s",":ROSCillator:SOURce INT;\r\n");
			sysScpi->setRef10MState(0);
			//	  drawFreqAndAmpl();
			drawFreqInParamter();
			break;

		case KEY_FUNC_FREQREF10M_EXTERNAL:
			/**********txl********************/
			//下发控制
			//	  strcpy(sysData.freq.ref10MHz, "rmExternal");  //zpc changed
			//	  printf("%s",":ROSCillator:SOURce EXT;\r\n");
			sysScpi->setRef10MState(1);
			//	  drawFreqAndAmpl();
			drawFreqInParamter();
			break;

		case KEY_FUNC__AMPL_ALC_ON: //稳幅状态开
			sysScpi->setALCState(1);
			//		drawFreqAndAmpl();
			drawAmplInParamter();
			break;

		case KEY_FUNC__AMPL_ALC_OFF: //稳幅状态关
			sysScpi->setALCState(0);
			//		drawFreqAndAmpl();
			drawAmplInParamter();
			break;

		case KEY_FUNC_AMPLRESPONSE_FAST:
			sysScpi->setAmplResponse(1);
			//		drawFreqAndAmpl();
			drawAmplInParamter();
			break;

		case KEY_FUNC_AMPLRESPONSE_SLOW:
			sysScpi->setAmplResponse(0);
			//		drawFreqAndAmpl();
			drawAmplInParamter();
			break;

		case KEY_FUNC_SWEEP_MODE_OFF:
			sysScpi->setSweepMode(smOff);
			//	  sysScpi->setSweepstate(0);

			//界面绘制
			drawFreqAndAmplMain();
			drawSweep(sysData.sweep.type);

			break;
		case KEY_FUNC_SWEEP_MODE_FREQ:
			sysScpi->setSweepMode(smFreq);
			//界面绘制
			drawFreqAndAmplMain();
			drawSweep(sysData.sweep.type);
			break;

		case KEY_FUNC_SWEEP_MODE_AMPL:
			sysScpi->setSweepMode(smAmpl);
			//界面绘制
			drawFreqAndAmplMain();
			drawSweep(sysData.sweep.type);
			break;

		case KEY_FUNC_SWEEP_MODE_FREQAMPL:
			sysScpi->setSweepMode(smFreqAmpl);
			//界面绘制
			drawFreqAndAmplMain();
			drawSweep(sysData.sweep.type);
			break;

#if 0
			case KEY_FUNC_SWEEP_MODE_FREQ_ON:

			//	  sysScpi->closeDemodState();
			sysData.sweep.freqstate = true;
			sysScpi->setSweepMode(smFreq);
			sysScpi->setSweepstate(1);

			//界面绘制
			drawSweep(sysData.sweep.type);
			break;

			case KEY_FUNC_SWEEP_MODE_FREQ_OFF:
			//		sysScpi->closeDemod();
			sysData.sweep.freqstate = false;
			if(sysData.sweep.amplstate)
			{
				sysScpi->setSweepMode(smAmpl);
			} else
			{
				sysScpi->setSweepstate(0);
			}

			//		sysScpi->setSweepMode(smFreqOff);
			drawSweep(sysData.sweep.type);
			break;

			case KEY_FUNC_SWEEP_MODE_AMPL_ON:
			//	  sysScpi->closeDemodState();
			sysData.sweep.amplstate = true;
			sysScpi->setSweepMode(smAmpl);
			sysScpi->setSweepstate(1);

			//界面绘制
			drawSweep(sysData.sweep.type);
			break;

			case KEY_FUNC_SWEEP_MODE_AMPL_OFF:
			//		sysScpi->closeDemodState();
			sysData.sweep.amplstate = false;
			if(sysData.sweep.freqstate)
			{
				sysScpi->setSweepMode(smFreq);
			} else
			{
				sysScpi->setSweepstate(0);
			}
			//		sysScpi->setSweepMode(smAmplOff);
			//界面绘制
			drawSweep(sysData.sweep.type);
			break;

			case KEY_FUNC_SWEEP_MODE_FREQAMPL:
			//	  sysScpi->closeDemodState();
			//	  sysData.sweep.freqstate = true;
			//	  sysData.sweep.amplstate = true;
			sysScpi->setSweepMode(smFreqAmpl);

			//界面绘制
			drawSweep(sysData.sweep.type);
			break;
#endif	  
		case KEY_FUNC_SWEEP_TYPE_LIST:
			//	  sysScpi->closeDemodState();

			sysData.sweep.type = stList;
			sysScpi->setSweepType(stList);
			sysScpi->setSweepListDownload(); //下发列表数据
			//界面绘制
			drawSweep(sysData.sweep.type);
			break;

		case KEY_FUNC_SWEEP_TYPE_STEP:
			//  	  sysScpi->closeDemodState();

			sysData.sweep.type = stStep;
			sysScpi->setSweepType(stStep);
			//界面绘制
			drawSweep(sysData.sweep.type);

			break;

		case KEY_FUNC_SWEEP_REPEAT_SINGLE:
			sysScpi->setSweepRepeat(srSingle);

			//界面绘制
			drawSweep(sysData.sweep.type);

			break;

		case KEY_FUNC_SWEEP_REPEAT_CONT:
			sysScpi->setSweepRepeat(srConutinuos);

			//界面绘制
			drawSweep(sysData.sweep.type);

			break;

		case KEY_FUNC_SWEEP_MORE_SWEEPRETRACE_ON: //折返
			sysScpi->setSweepRetrace(srOn);

			//界面绘制
			drawSweep(sysData.sweep.type);
			break;

		case KEY_FUNC_SWEEP_MORE_SWEEPRETRACE_OFF:
			sysScpi->setSweepRetrace(srOff);

			//界面绘制
			drawSweep(sysData.sweep.type);
			break;

		case KEY_FUNC_SWEEP_MORE_SWEEPDIRECTION_DOWN:
			sysScpi->setSweepDirection(sdDown);

			//界面绘制
			drawSweep(sysData.sweep.type);

			break;

		case KEY_FUNC_SWEEP_MORE_SWEEPDIRECTION_UP:
			sysScpi->setSweepDirection(sdUp);

			//界面绘制
			drawSweep(sysData.sweep.type);
			break;

		case KEY_FUNC_SWEEP_SINGLETRIGGER:
			sysScpi->setSweepRepeat(0);
			//	  sysData.sweep.repeat = sweepRepeatDef(0);
			sysScpi->sendSerialData(":SWEEP:STAT 1");
			//	  sysData.sweep.repeat = srSingle;
			//
			//	  strcpy(SendToMSG1030CH, ":INIT:CONT 0\n");
			//  	  printf("%s",SendToMSG1030CH);
			//
			//  	  strcpy(SendToMSG1030CH, ":INIT:TRIGg\n");
			//  	  printf("%s",SendToMSG1030CH);

			//界面绘制
			sysData.isBeginDrawSingleProcess = true;

			drawSweep(sysData.sweep.type);
			break;

		case KEY_FUNC_SWEEP_MORE_SWEEPTRIGGER_FREERUN:
			// 	  strcpy(sysData.sweep.trigger, "FreeRun");
			sysScpi->closeDemodState();
			sysScpi->setSweepTrig(0);
			drawSweep(sysData.sweep.type);
			break;

		case KEY_FUNC_SWEEP_MORE_SWEEPTRIGGER_TRIGGERKEY:
			//  	  strcpy(sysData.sweep.trigger, "TriggerKey");

			break;

		case KEY_FUNC_SWEEP_MORE_SWEEPTRIGGER_BUS:
			//	  strcpy(sysData.sweep.trigger, "Bus");
			break;

		case KEY_FUNC_SWEEP_MORE_SWEEPTRIGGER_EXT:
			//	  strcpy(sysData.sweep.trigger, "Ext");
			sysScpi->closeDemodState();
			sysScpi->setSweepTrig(1);
			drawSweep(sysData.sweep.type);
			break;

		case KEY_FUNC_SWEEP_MORE_SWEEPTRIGGER_TIMETRIGGER:
			// 	  strcpy(sysData.sweep.trigger, "TimeTrigger");
			break;

		case KEY_FUNC_FCTN_ANALOG_SOURCE_INTERNAL:
			sysScpi->setAnalogModulateSource(0);
			drawAnalogModulate();
			break;

		case KEY_FUNC_FCTN_ANALOG_SOURCE_EXTERNAL:
			sysScpi->setAnalogModulateSource(1);
			drawAnalogModulate();
			break;

		case KEY_FUNC_FCTN_ANALOG_AM_STATE_ON:
			sysScpi->closeDemodState();

			sysScpi->setAmState(1);

			//界面绘制
			drawAnalogModulate();
			break;

		case KEY_FUNC_FCTN_ANALOG_AM_STATE_OFF:
			sysScpi->closeDemodState();
			sysScpi->setAmState(0);

			//界面绘制
			drawAnalogModulate();
			break;

		case KEY_FUNC_FCTN_ANALOG_FM_STATE_ON:
			sysScpi->closeDemodState();
			sysScpi->setFmState(1);

			//界面绘制
			drawAnalogModulate();
			break;

		case KEY_FUNC_FCTN_ANALOG_FM_STATE_OFF:
			sysScpi->closeDemodState();
			sysScpi->setFmState(0);

			//界面绘制
			drawAnalogModulate();
			break;

		case KEY_FUNC_FCTN_ANALOG_PM_STATE_ON:
			sysScpi->closeDemodState();
			sysScpi->setPmState(1);

			//界面绘制
			drawAnalogModulate();
			break;

		case KEY_FUNC_FCTN_ANALOG_PM_STATE_OFF:
			sysScpi->closeDemodState();
			sysScpi->setPmState(0);

			//界面绘制
			drawAnalogModulate();
			break;

		case KEY_FUNC_FCTN_DIGITAL_STATE_ON: //设置数字调制状态为开
			sysScpi->closeDemodState();
			sysScpi->setDigitalModuateState(1);
			drawDigitalModulate(); //绘制界面
			break;

		case KEY_FUNC_FCTN_DIGITAL_STATE_OFF: //设置数字调制状态为关
			sysScpi->closeDemodState();
			sysScpi->setDigitalModuateState(0);
			drawDigitalModulate();
			break;

		case KEY_FUNC_FCTN_DIGITAL_SOURCE_INTERNAL:
			sysScpi->setDigitalModulateSource(0);
			drawDigitalModulate();
			break;

		case KEY_FUNC_FCTN_DIGITAL_SOURCE_EXTERNAL:
			sysScpi->setDigitalModulateSource(1);
			drawDigitalModulate();
			break;

		case KEY_FUNC_FCTN_DIGITAL_TYPE_ASK_2ASK:
			sysScpi->setDigitalModuateType(dt2ASK);
			drawDigitalModulate();
			break;

		case KEY_FUNC_FCTN_DIGITAL_TYPE_ASK_4ASK:
			sysScpi->setDigitalModuateType(dt4ASK);
			drawDigitalModulate();
			break;

		case KEY_FUNC_FCTN_DIGITAL_TYPE_ASK_8ASK:
			sysScpi->setDigitalModuateType(dt8ASK);
			drawDigitalModulate();
			break;

		case KEY_FUNC_FCTN_DIGITAL_TYPE_FSK_2FSK:
			sysScpi->setDigitalModuateType(dt2FSK);
			drawDigitalModulate();
			break;

		case KEY_FUNC_FCTN_DIGITAL_TYPE_FSK_4FSK:
			sysScpi->setDigitalModuateType(dt4FSK);
			drawDigitalModulate();
			break;

		case KEY_FUNC_FCTN_DIGITAL_TYPE_FSK_8FSK:
			sysScpi->setDigitalModuateType(dt8FSK);
			drawDigitalModulate();
			break;

		case KEY_FUNC_FCTN_DIGITAL_TYPE_PSK_2PSK:
			sysScpi->setDigitalModuateType(dt2PSK);
			drawDigitalModulate();
			break;

		case KEY_FUNC_FCTN_DIGITAL_TYPE_PSK_4PSK:
			sysScpi->setDigitalModuateType(dt4PSK);
			drawDigitalModulate();
			break;

		case KEY_FUNC_FCTN_DIGITAL_TYPE_PSK_8PSK:
			sysScpi->setDigitalModuateType(dt8PSK);
			drawDigitalModulate();
			break;

			//	case KEY_FUNC_FCTN_DIGITAL_ASK_STATE_ON:
			//      sysScpi->setAskState(1);
			//
			//	  //界面绘制
			//	  drawDigitalModulate();
			//	  break;

			//	case KEY_FUNC_FCTN_DIGITAL_ASK_STATE_OFF:
			//      sysScpi->setAskState(0);
			//
			//		//界面绘制
			//	  drawDigitalModulate();
			//	  break;

			//	case KEY_FUNC_FCTN_DIGITAL_FSK_STATE_ON:
			//      sysScpi->setFskState(1);
			//
			//	  //界面绘制
			//	  drawDigitalModulate();
			//	  break;

			//	case KEY_FUNC_FCTN_DIGITAL_FSK_STATE_OFF:
			//      sysScpi->setFskState(0);
			//	  //界面绘制
			//	  drawDigitalModulate();
			//	  break;

			//	case KEY_FUNC_FCTN_DIGITAL_PSK_STATE_ON:
			//  	  sysScpi->setPskState(1);
			//
			//  	  //界面绘制
			//  	  drawDigitalModulate();
			//  	  break;

			//	case KEY_FUNC_FCTN_DIGITAL_PSK_STATE_OFF:
			//      sysScpi->setPskState(0);
			//
			//  	  //界面绘制
			//      drawAnalogModulate();
			//  	  break;

			/*case KEY_FUNC_FCTN_DIGITAL_PLUSE_STATE_ON:
			 sysScpi->setPluseState(1);

			 //界面绘制
			 drawAnalogModulate();
			 break;*/

			/*case KEY_FUNC_FCTN_DIGITAL_PLUSE_STATE_OFF:
			 sysScpi->setPluseState(0);

			 //界面绘制
			 drawAnalogModulate();
			 break;*/
			/*
			 case KEY_FUNC_FCTN_DIGITAL_ASK_MODE_ASK2:
			 sysScpi->setAskMode(askm2);      //界面绘制
			 drawDigitalModulate();
			 break;

			 case KEY_FUNC_FCTN_DIGITAL_ASK_MODE_ASK4:
			 sysScpi->setAskMode(askm4);

			 //界面绘制
			 drawDigitalModulate();
			 break;

			 case KEY_FUNC_FCTN_DIGITAL_ASK_MODE_ASK8:
			 sysScpi->setAskMode(askm8);

			 //界面绘制
			 drawDigitalModulate();
			 break;

			 case KEY_FUNC_FCTN_DIGITAL_FSK_MODE_FSK2:
			 sysScpi->setFskMode(fskm2);

			 //界面绘制
			 drawDigitalModulate();
			 break;

			 case KEY_FUNC_FCTN_DIGITAL_FSK_MODE_FSK4:
			 sysScpi->setFskMode(fskm4);

			 //界面绘制
			 drawDigitalModulate();
			 break;

			 case KEY_FUNC_FCTN_DIGITAL_FSK_MODE_FSK8:
			 sysScpi->setPskMode(fskm8);

			 //界面绘制
			 drawDigitalModulate();
			 break;

			 case KEY_FUNC_FCTN_DIGITAL_PSK_MODE_BPSK:
			 sysScpi->setPskMode(pskmBpsk);

			 //界面绘制
			 drawDigitalModulate();
			 break;

			 case KEY_FUNC_FCTN_DIGITAL_PSK_MODE_QPSK:
			 sysScpi->setPskMode(pskmQpsk);
			 //界面绘制
			 drawDigitalModulate();
			 break;

			 case KEY_FUNC_FCTN_DIGITAL_PSK_MODE_PSK8:
			 sysScpi->setPskMode(pskm8);

			 //界面绘制
			 drawDigitalModulate();
			 break;
			 case KEY_FUNC_FCTN_DIGITAL_PSK_MODE_PI4PSK:
			 sysScpi->setPskMode(pskmPI4Psk);

			 //界面绘制
			 drawDigitalModulate();
			 break;
			 */
			//Pluse
		case KEY_FUNC_FCTN_PLUSE_STATE_ON:
			//		sysScpi->closeDemodState();
			sysScpi->setPluseState(1);
			drawPluseModulate();
			break;

		case KEY_FUNC_FCTN_PLUSE_STATE_OFF:
			//		sysScpi->closeDemodState();
			sysScpi->setPluseState(0);
			drawPluseModulate();
			break;
		case KEY_FUNC_FCTN_PLUSE_SOURCE_INTERNAL:
			sysScpi->setPluseSource(0);
			drawPluseModulate();
			break;
		case KEY_FUNC_FCTN_PLUSE_SOURCE_EXTERNAL:
			sysScpi->setPluseSource(1);
			drawPluseModulate();
			break;
		case KEY_FUNC_FCTN_PLUSE_METHOD_AUTO:
			if (sysData.pluse.plusesource == psInternal)
			{
				sysScpi->setPluseMethod(0);
				drawPluseModulate();
			}
			break;

		case KEY_FUNC_FCTN_PLUSE_METHOD_TRIG:
			if (sysData.pluse.plusesource == psInternal)
			{
				sysScpi->setPluseMethod(1);
				drawPluseModulate();
			}
			break;

			//	case KEY_FUNC_FCTN_PLUSE_METHOD_EXTER:
			//		sysScpi->setPluseMethod(2);
			//		drawPluseModulate();
			//		break;
			//LF
		case KEY_FUNC_FCTN_LF_STATE_ON:
			sysScpi->setLFState(1);
			sysData.keyBoardSwitchLF = 0x01;
			sysScpi->keyBoardLEDSwitch(2, sysData.keyBoardSwitchLF);
			drawArbitraryWave();
			break;

		case KEY_FUNC_FCTN_LF_STATE_OFF:
			sysScpi->setLFState(0);
			sysData.keyBoardSwitchLF = 0x00;
			sysScpi->keyBoardLEDSwitch(2, sysData.keyBoardSwitchLF);
			drawArbitraryWave();
			break;
		case KEY_FUNC_FCTN_LF_WAVEFORM_SINE:
			sysScpi->setLfType(lotSine);

			//界面绘制
			strcpy(sysData.name, "");
			strcpy(sysData.value, "");
			strcpy(sysData.unit, "");
			drawInputParamterNoParamter();
			drawArbitraryWave();
			break;

		case KEY_FUNC_FCTN_LF_WAVEFORM_SQUARE:
			sysScpi->setLfType(lotSquare);

			//界面绘制
			strcpy(sysData.name, "");
			strcpy(sysData.value, "");
			strcpy(sysData.unit, "");
			drawInputParamterNoParamter();
			drawArbitraryWave();
			break;

		case KEY_FUNC_FCTN_LF_WAVEFORM_TRIANGULAR:
			sysScpi->setLfType(lotTriangle);

			//界面绘制
			strcpy(sysData.name, "");
			strcpy(sysData.value, "");
			strcpy(sysData.unit, "");
			drawInputParamterNoParamter();
			drawArbitraryWave();
			break;

		case KEY_FUNC_FCTN_LF_WAVEFORM_SAWTOOTH:
			//射频控制
			sysScpi->setLfType(lotSawtooth);

			//界面绘制
			strcpy(sysData.name, "");
			strcpy(sysData.value, "");
			strcpy(sysData.unit, "");
			drawInputParamterNoParamter();
			drawArbitraryWave();
			break;

		case KEY_FUNC_FCTN_LF_SOURCE_SINC:
			//射频控制
			sysScpi->setLfType(lotSinc);

			//界面绘制
			strcpy(sysData.name, "");
			strcpy(sysData.value, "");
			strcpy(sysData.unit, "");
			drawInputParamterNoParamter();
			drawArbitraryWave();
			break;
			//DRG
		case KEY_FUNC_FCTN_DRG_STATE_ON:
			sysScpi->setDrgState(1);
			drawDrgFunction();
			break;
		case KEY_FUNC_FCTN_DRG_STATE_OFF:
			sysScpi->setDrgState(0);
			drawDrgFunction();
			break;
		case KEY_FUNC_FCTN_DRG_MORE_WORKMODE_NORMAL:
			sysScpi->setDrgWorkmode(ddnormal);
			strcpy(sysData.name, "");
			strcpy(sysData.value, "");
			strcpy(sysData.unit, "");
			drawInputParamterNoParamter();
			drawDrgFunction();
			break;
		case KEY_FUNC_FCTN_DRG_MORE_WORKMODE_POSITIVE:
			sysScpi->setDrgWorkmode(ddpositive);
			//界面绘制
			strcpy(sysData.name, "");
			strcpy(sysData.value, "");
			strcpy(sysData.unit, "");
			drawInputParamterNoParamter();
			drawDrgFunction();
			break;
		case KEY_FUNC_FCTN_DRG_MORE_WORKMODE_NEGATIVE:
			sysScpi->setDrgWorkmode(ddnegative);
			//界面绘制
			strcpy(sysData.name, "");
			strcpy(sysData.value, "");
			strcpy(sysData.unit, "");
			drawInputParamterNoParamter();
			drawDrgFunction();
			break;
	}
}

//处理主键
void mainForm::handleMainKey(int funcCode)
{
	QString sendToSG1030QS = "";
	char tempChar[64] = { 0 };
	char sendToSG1030CH[64] = { };

	switch (funcCode)
	{
		case KEY_MAIN_PRESET:
			strcpy(sysData.name, "");
			strcpy(sysData.value, "");
			strcpy(sysData.unit, "");
			presetSystem(false);
			sysScpi->initKeyBoardLED();
			//	  printf("*RST\n");
			sysScpi->sendSerialData("*RST");
			sysScpi->controlParamterDown();
//			sysScpi->savePowerOffState();

			//	  drawState();

			//	  strcpy(sysData.name, "");
			//	  strcpy(sysData.value, "");
			//	  strcpy(sysData.unit, "");
			//      drawInputParamterNoParamter();

			//      drawFreqAndAmplMain();
			//      drawFreqAndAmpl();
			//	  drawFreqInParamter();
			//	  sysScpi->controlParamterDown();
			break;

		case KEY_MAIN_SWEEP:
			drawSweep(sysData.sweep.type);

			strcpy(sysData.name, "");
			strcpy(sysData.value, "");
			strcpy(sysData.unit, "");
			drawInputParamterNoParamter();

			drawButton();
			drawSweep(0);
			break;

		case KEY_MAIN_FCTN:
			strcpy(sysData.name, "");
			strcpy(sysData.value, "");
			strcpy(sysData.unit, "");
			drawInputParamterNoParamter();

			drawButton();
			break;

		case KEY_MAIN_AUX:
			strcpy(sysData.name, "");
			strcpy(sysData.value, "");
			strcpy(sysData.unit, "");
			drawInputParamterNoParamter();

			drawButton();
			break;

		case KEY_MAIN_RF:
			if (nowSubKey->isUnit)
			{
				if (nowSubKey->leftNode != NULL)
					nowSubKey = nowSubKey->leftNode;
			}

			drawButton();
			funcCode = nowSubKey->funcCode;
			sysData.keyBoardSwitchRF = (~sysData.keyBoardSwitchRF) & 0x01;
			sysScpi->keyBoardLEDSwitch(3, sysData.keyBoardSwitchRF);
			sprintf(tempChar, "%d", sysData.keyBoardSwitchRF);
			sendToSG1030QS = QString(":SYST:RFO ") + QString(tempChar).trimmed();
			//	  sendToSG1030QS = QString(":SYST:RFO ") + QString(tempChar).trimmed() + "\n";
			//	  strcpy(sendToSG1030CH,sendToSG1030QS.toLatin1());
			//	  printf("%s",sendToSG1030CH);
			sysScpi->sendSerialData(sendToSG1030QS);

			drawFreqAndAmplMain();
			drawState();
			break;

		case KEY_MAIN_MOD:
			if (nowSubKey->isUnit)
			{
				if (nowSubKey->leftNode != NULL)
					nowSubKey = nowSubKey->leftNode;
			}
			drawButton();

			funcCode = nowSubKey->funcCode;
			sysData.keyBoardSwitchMod = (~sysData.keyBoardSwitchMod) & 0x01;
			sysScpi->keyBoardLEDSwitch(1, sysData.keyBoardSwitchMod);
			sprintf(tempChar, "%d", sysData.keyBoardSwitchMod);
			//	  sendToSG1030QS = QString(":SYST:MODO ") + QString(tempChar).trimmed() + "\n";
			//	  strcpy(sendToSG1030CH, sendToSG1030QS.toLatin1());
			sendToSG1030QS = QString(":SYST:MODO ") + QString(tempChar).trimmed();
			//	  printf("%s",sendToSG1030CH);
			sysScpi->sendSerialData(sendToSG1030QS);

			drawState();
			break;

		case KEY_MAIN_LF:
			if (nowSubKey->isUnit)
			{
				if (nowSubKey->leftNode != NULL)
					nowSubKey = nowSubKey->leftNode;
			}

			drawButton();
			funcCode = nowSubKey->funcCode;
			sysData.keyBoardSwitchLF = (~sysData.keyBoardSwitchLF) & 0x01;
			sysScpi->keyBoardLEDSwitch(2, sysData.keyBoardSwitchLF);
			sysData.lf.state = sysData.keyBoardSwitchLF;
			sprintf(tempChar, "%d", sysData.keyBoardSwitchLF);
			//	  sendToSG1030QS = QString(":LFOUT:SWITCH ") + QString(tempChar).trimmed() + "\n";
			//
			//	  strcpy(sendToSG1030CH, sendToSG1030QS.toLatin1());
			//	  printf("%s",sendToSG1030CH);
			sendToSG1030QS = QString(":LFOUT:SWITCH ") + QString(tempChar).trimmed();
			sysScpi->sendSerialData(sendToSG1030QS);
			if (funcCode == KEY_FUNC_FCTN_LF_RETURN || funcCode == KEY_FUNC_FCTN_LF_STATE || funcCode == KEY_FUNC_FCTN_LF_SOURCE || funcCode == KEY_FUNC_FCTN_LF_AMPL || funcCode == KEY_FUNC_FCTN_LF_FREQ)
			{
				drawArbitraryWave();
			}
			drawState();
			drawButton();
			break;
	}

	//save state
//	sysScpi->savePowerOffState();

}

//处理输入键
void mainForm::handleInputKey(int funcCode)
{
	char temp[64];
	QString auxStr = "";
	QString tempu;
	size_t temp_lenth;
	int rowDisplay = 8; //行
	int lineDisplay = 4; //列
	int rowPitchOn;
	int linePitchOn;
	int goToLine;

	QString name = "";
	QString value = "";
	QString unit = "";
	char tempValue[64] = { 0 };
	char tempMac[1] = { 0 };

	switch (funcCode)
	{
		case KEY_FUNC_FREQ:
			name = "FREQ:";
			strcpy(tempValue, getFreqString(sysData.freq.freq, temp));
			insertThousands(tempValue);
			value = QString(tempValue).trimmed();
			unit = freqToQUnit(sysData.freq.freq).trimmed();

			inputValue = "";
			sysData.inputLength = MAXFREQINPUTLENGTH;
			//关闭调制信号
			//      sysScpi->closeDemod();

			drawFreqAndAmplMain();
			//      drawFreqAndAmpl();
			drawFreqInParamter();
			break;

		case KEY_FUNC_FREQOFFSET:
			name = "FREQ";
			inputValue = "";
			strcpy(tempValue, getFreqString(sysData.freq.freq, temp));
			insertThousands(tempValue);
			value = QString(tempValue).trimmed();
			unit = freqToQUnit(sysData.freq.freq).trimmed();

			sysData.inputLength = MAXFREQINPUTLENGTH;

			drawFreqAndAmplMain();
			//      drawFreqAndAmpl();
			drawFreqInParamter();
			break;

		case KEY_FUNC_FREQMULTIPLIER:
			//      name = "Freq Mult:";
			//      value = QString(intToQString(sysData.freq.multiplier, temp)).trimmed();
			//      unit = "x";
			//      sysData.inputLength = MAXPOINTINPUTLENGTH;
			name = "FREQ:";
			inputValue = "";
			strcpy(tempValue, getFreqString(sysData.freq.freq, temp));
			insertThousands(tempValue);
			value = QString(tempValue).trimmed();
			unit = freqToQUnit(sysData.freq.freq).trimmed();
			sysData.inputLength = MAXFREQMULINPUTLENGTH;
			drawFreqAndAmplMain();
			//      drawFreqAndAmpl();
			drawFreqInParamter();
			break;

		case KEY_FUNC_AMPL:
			name = "AMPL:";
			strcpy(tempValue, floatToStringDot2(sysData.ampl.ampl, temp));
			unit = amplToQUnit(sysData.ampl.ampl);
			//	  strcpy(sysData.amplinitvalue ,tempValue);
			value = QString(tempValue).trimmed();
			/*
			 switch(sysData.ampl.amplinitUnit)
			 {
			 case audBm: 	unit = QString("dBm");		break;
			 case audBuV:	unit = QString("dBuV");		break;
			 case aumV:		unit = QString("mV");		break;
			 default:		unit = QString("dBm");		break;
			 }
			 */
			inputValue = "";
			sysData.inputLength = MAXAMPLINPUTLENGTH;

			//关闭调制信号
			//	  sysScpi->closeDemod();

			drawFreqAndAmplMain();
			//	  drawFreqAndAmpl();
			drawAmplInParamter();
			break;

			//  	case KEY_FUNC_AMPL_ERROR:
			//  	  name = "AMPL Error:";
			//  	  strcpy(tempValue, floatToStringDot2(sysData.ampl.error.value, temp));
			//  	  value = QString(tempValue).trimmed();
			//  	  unit = amplToQUnit(sysData.ampl.error.value).trimmed();
			//
			//  	  inputValue = "";
			//  	  sysData.inputLength = MAXAMDEPTH;
			//
			//  	  drawFreqAndAmplMain();
			//  	  drawFreqAndAmpl();
			//  	  break;

		case KEY_FUNC_AMPLOFFSET:
			name = "AMPL:";
			strcpy(tempValue, floatToStringDot2(sysData.ampl.ampl, temp));
			value = QString(tempValue).trimmed();
			unit = amplToQUnit(sysData.ampl.offset).trimmed();

			inputValue = "";
			sysData.inputLength = MAXAMPLINPUTLENGTH;

			drawFreqAndAmplMain();
			//   	  drawFreqAndAmpl();
			drawAmplInParamter();
			break;

		case KEY_FUNC_AMPL_CABLE:
			name = "I.L Cable:";
			strcpy(tempValue, floatToStringDot2(sysData.ampl.error.value, temp));
			value = QString(tempValue).trimmed();
			unit = "dB";
			inputValue = "";
			sysData.inputLength = MAXAMPLINPUTLENGTH;
			drawFreqAndAmplMain();
			drawAmplInParamter();
			break;
		case KEY_FUNC_SWEEP_STEP_MORE_STEPDWELL:
			name = "Dwell:";
			getMsTimeValue(sysData.sweep.step.dwell, temp);
			strcpy(tempValue, temp);
			//   	  strcpy(tempValue, floatToStringDot3(sysData.sweep.step.dwell, temp));
			value = QString(tempValue).trimmed();
			unit = timeToQUnit(sysData.sweep.step.dwell).trimmed();

			inputValue = "";
			sysData.inputLength = MAXTIMEINPUTLENGTH;
			drawSweep(sysData.sweep.type);
			break;

		case KEY_FUNC_SWEEP_STEP_FREQSTART:
			name = "Start Freq:";
			inputValue = "";
			strcpy(tempValue, getFreqString(sysData.sweep.step.freqStart, temp));
			insertThousands(tempValue);
			value = QString(tempValue).trimmed();
			unit = freqToQUnit(sysData.sweep.step.freqStart).trimmed();

			sysData.inputLength = MAXFREQINPUTLENGTH;

			drawSweep(sysData.sweep.type);
			break;

		case KEY_FUNC_SWEEP_STEP_FREQSTOP:
			name = "Stop Freq:";
			inputValue = "";
			strcpy(tempValue, getFreqString(sysData.sweep.step.freqStop, temp));
			insertThousands(tempValue);
			value = QString(tempValue).trimmed();
			unit = freqToQUnit(sysData.sweep.step.freqStop).trimmed();

			sysData.inputLength = MAXFREQINPUTLENGTH;

			drawSweep(sysData.sweep.type);
			break;

		case KEY_FUNC_SWEEP_STEP_AMPLSTART:
			name = "Start Ampl:";
			strcpy(tempValue, floatToStringDot2(sysData.sweep.step.amplStart, temp));
			value = QString(tempValue).trimmed();
			unit = amplToQUnit(sysData.sweep.step.amplStart).trimmed();

			inputValue = "";
			sysData.inputLength = MAXAMPLINPUTLENGTH;

			drawSweep(sysData.sweep.type);
			break;

		case KEY_FUNC_SWEEP_STEP_AMPLSTOP:
			name = "Stop Ampl:";
			strcpy(tempValue, floatToStringDot2(sysData.sweep.step.amplStop, temp));
			value = QString(tempValue).trimmed();
			unit = amplToQUnit(sysData.sweep.step.amplStop).trimmed();

			inputValue = "";
			sysData.inputLength = MAXAMPLINPUTLENGTH;

			drawSweep(sysData.sweep.type);
			break;

		case KEY_FUNC_SWEEP_STEP_MORE_POINTS:
			name = "Points:";
			strcpy(tempValue, intToString(sysData.sweep.step.point, temp));
			value = QString(tempValue).trimmed();
			//	  unit = amplToQUnit(sysData.sweep.step.point).trimmed();

			inputValue = "";
			sysData.inputLength = MAXPOINTINPUTLENGTH;

			drawSweep(sysData.sweep.type);
			break;

		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_EDITITEM:
			if (sysData.sweep.list.selectLine == 0) //如果为频率值
			{
				name = "Freq:";
				inputValue = "";
				strcpy(tempValue, getFreqString(sysData.sweep.list.data[sysData.sweep.list.selectRow].freq, temp));
				insertThousands(tempValue);
				value = QString(tempValue).trimmed();
				unit = freqToQUnit(sysData.sweep.list.data[sysData.sweep.list.selectRow].freq).trimmed();

				sysData.inputLength = MAXFREQINPUTLENGTH;
			} else if (sysData.sweep.list.selectLine == 1) //如果为幅度值
			{
				name = "Start Ampl:";
				strcpy(tempValue, floatToStringDot2(sysData.sweep.list.data[sysData.sweep.list.selectRow].ampl, temp));
				value = QString(tempValue).trimmed();
				unit = amplToQUnit(sysData.sweep.list.data[sysData.sweep.list.selectRow].ampl).trimmed();

				inputValue = "";
				sysData.inputLength = MAXAMPLINPUTLENGTH;
			} else if (sysData.sweep.list.selectLine == 2) //如果为驻留时间
			{
				name = "Dwell:";
				//		   strcpy(tempValue, floatToStringDot3(sysData.sweep.list.data[sysData.sweep.list.selectRow].dwell, temp));
				getMsTimeValue(sysData.sweep.list.data[sysData.sweep.list.selectRow].dwell, tempValue);
				value = QString(tempValue).trimmed();
				unit = timeToQUnit(sysData.sweep.list.data[sysData.sweep.list.selectRow].dwell).trimmed();

				inputValue = "";
				sysData.inputLength = MAXTIMEINPUTLENGTH;
			}

			break;

		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_MORE_DOWNLOAD:
			drawUSBFileEntry(LISTDATAENTRY);
			break;

			//case KEY_FUNC_SWEEP_MORE_CONFIGLIST_MORE_GOTOROW:
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_GOTOROW:
			name = "NUM:";
			inputValue = "";
			value = QString(intToQString(sysData.sweep.list.selectRow + 1, temp)).trimmed();
			unit = "";
			sysData.inputLength = MAXPOINTINPUTLENGTH;
			break;

		case KEY_FUNC_FCTN_ANALOG_AM_DEPTH:
			name = "AM Depth:";
			inputValue = "";
			value = QString(floatToStringDot2(sysData.analog.am.depth, temp)).trimmed();
			unit = "%";
			sysData.inputLength = MAXINPUTLENGTH;
			inputValue = "";
			drawAnalogModulate();
			break;

		case KEY_FUNC_FCTN_ANALOG_FM_DEVIATION:
			name = "FM Deviation:";
			inputValue = "";

			strcpy(tempValue, getFreqString(sysData.analog.fm.deviation, temp));
			insertThousands(tempValue);
			value = QString(tempValue).trimmed();
			unit = freqToQUnit(sysData.analog.fm.deviation).trimmed();

			sysData.inputLength = MAXFREQINPUTLENGTH;

			inputValue = "";
			drawAnalogModulate();
			break;

		case KEY_FUNC_FCTN_ANALOG_PM_PHASE:
			name = "PM Phase:";
			inputValue = "";
			value = doubleToQValue(sysData.analog.pm.phase).trimmed();
			unit = "°";
			sysData.inputLength = MAXINPUTLENGTH;
			inputValue = "";
			drawAnalogModulate();
			break;

		case KEY_FUNC_FCTN_ANALOG_AM_RATE:
			name = "AM Rate:";
			inputValue = "";

			strcpy(tempValue, getFreqString(sysData.analog.am.rate, temp));
			insertThousands(tempValue);
			value = QString(tempValue).trimmed();
			unit = freqToQUnit(sysData.analog.am.rate).trimmed();

			sysData.inputLength = MAXFREQINPUTLENGTH;

			inputValue = "";
			drawAnalogModulate();
			break;

		case KEY_FUNC_FCTN_ANALOG_FM_RATE:
			name = "FM Rate:";
			inputValue = "";

			strcpy(tempValue, getFreqString(sysData.analog.fm.rate, temp));
			insertThousands(tempValue);
			value = QString(tempValue).trimmed();
			unit = freqToQUnit(sysData.analog.fm.rate).trimmed();

			sysData.inputLength = MAXFREQINPUTLENGTH;

			inputValue = "";
			drawAnalogModulate();
			break;

		case KEY_FUNC_FCTN_ANALOG_PM_RATE:
			name = "PM Rate:";
			inputValue = "";

			strcpy(tempValue, getFreqString(sysData.analog.pm.rate, temp));
			insertThousands(tempValue);
			value = QString(tempValue).trimmed();
			unit = freqToQUnit(sysData.analog.pm.rate).trimmed();

			sysData.inputLength = MAXFREQINPUTLENGTH;

			inputValue = "";
			drawAnalogModulate();
			break;

		case KEY_FUNC_FCTN_DIGITAL_MORE_LOAD:
			drawUSBFileEntry(DIGITALDATAENTRY);
			break;

		case KEY_FUNC_FCTN_DIGITAL_PARAMETER_ASKDEEP:
			if ((sysData.digital.digitaltype == dt2ASK) || (sysData.digital.digitaltype == dt4ASK) || (sysData.digital.digitaltype == dt8ASK))
			{
				name = "Depth:";
				inputValue = "";

				value = QString(floatToStringDot2(sysData.digital.digitalpara.depth, temp)).trimmed();
				unit = "%";
				sysData.inputLength = MAXINPUTLENGTH;
				inputValue = "";
				drawDigitalModulate();
			}
			break;

		case KEY_FUNC_FCTN_DIGITAL_PARAMETER_FSKOFFSET:
			if ((sysData.digital.digitaltype == dt2FSK) || (sysData.digital.digitaltype == dt4FSK) || (sysData.digital.digitaltype == dt8FSK))
			{
				name = "Devitation:";
				inputValue = "";

				strcpy(tempValue, getFreqString(sysData.digital.digitalpara.deviation, temp));
				insertThousands(tempValue);
				value = QString(tempValue).trimmed();
				unit = freqToQUnit(sysData.digital.digitalpara.deviation).trimmed();

				sysData.inputLength = MAXINPUTLENGTH;
				inputValue = "";
				drawDigitalModulate();
			}
			break;

		case KEY_FUNC_FCTN_DIGITAL_PARAMETER_PSKDEVI:
			if ((sysData.digital.digitaltype == dt2PSK) || (sysData.digital.digitaltype == dt4PSK) || (sysData.digital.digitaltype == dt8PSK))
			{
				name = "Phase:";
				inputValue = "";

				sprintf(tempValue, "%.0f", sysData.digital.digitalpara.phase);
				insertThousands(tempValue);
				value = QString(tempValue).trimmed();
				unit = "°";

				sysData.inputLength = MAXINPUTLENGTH;
				inputValue = "";
				drawDigitalModulate();
			}
			break;

		case KEY_FUNC_FCTN_DIGITAL_PARAMETER_RATE:
			name = "Rate";
			inputValue = "";

			strcpy(tempValue, getFreqString(sysData.digital.digitalpara.rate, temp));
			insertThousands(tempValue);
			value = QString(tempValue).trimmed();
			unit = freqToQUnit(sysData.digital.digitalpara.rate).trimmed();

			sysData.inputLength = MAXINPUTLENGTH;
			inputValue = "";
			drawDigitalModulate();
			break;

			//	case KEY_FUNC_FCTN_DIGITAL_ASK_RATE:
			//	  name = "ASK Rate:";
			//	  inputValue = "";
			//
			//	  strcpy(tempValue, getFreqString(sysData.digital.ask.rate, temp));
			//	  insertThousands(tempValue);
			//	  value = QString(tempValue).trimmed();
			//	  unit = freqToQUnit(sysData.digital.ask.rate).trimmed();
			//
			//	  sysData.inputLength = MAXFREQINPUTLENGTH;
			//
			//	  inputValue = "";
			//	  drawDigitalModulate();
			//	  break;

			//	case KEY_FUNC_FCTN_DIGITAL_FSK_RATE:
			//	  name = "FSK Rate:";
			//	  inputValue = "";
			//
			//	  strcpy(tempValue, getFreqString(sysData.digital.fsk.rate, temp));
			//	  insertThousands(tempValue);
			//	  value = QString(tempValue).trimmed();
			//	  unit = freqToQUnit(sysData.digital.fsk.rate).trimmed();
			//
			//	  sysData.inputLength = MAXFREQINPUTLENGTH;
			//
			//	  inputValue = "";
			//	  drawDigitalModulate();
			//	  break;


			//	case KEY_FUNC_FCTN_DIGITAL_FSK_STEP:
			//	  name = "FSK Step:";
			//	  inputValue = "";
			//
			//	  strcpy(tempValue, getFreqString(sysData.digital.fsk.step, temp));
			//	  insertThousands(tempValue);
			//	  value = QString(tempValue).trimmed();
			//	  unit = freqToQUnit(sysData.digital.fsk.step).trimmed();
			//
			//	  sysData.inputLength = MAXFREQINPUTLENGTH;
			//
			//	  inputValue = "";
			//	  drawDigitalModulate();
			//	  break;


			//	case KEY_FUNC_FCTN_DIGITAL_PSK_RATE:
			//	  name = "PSK Rate:";
			//	  inputValue = "";
			//
			//	  strcpy(tempValue, getFreqString(sysData.digital.psk.rate, temp));
			//	  insertThousands(tempValue);
			//	  value = QString(tempValue).trimmed();
			//	  unit = freqToQUnit(sysData.digital.psk.rate).trimmed();
			//
			//	  sysData.inputLength = MAXFREQINPUTLENGTH;
			//
			//	  inputValue = "";
			//	  drawDigitalModulate();
			//	  break;

			/*case KEY_FUNC_FCTN_DIGITAL_PLUSE_PERIOD:
			 name = "Pluse Period:";
			 inputValue = "";

			 strcpy(tempValue,floatToStringDot3(valueToDValue(sysData.analog.pluse.period), temp));
			 value = QString(tempValue).trimmed();
			 unit = demodTimeToQUnit(sysData.analog.pluse.period).trimmed();

			 sysData.inputLength = MAXTIMEINPUTLENGTH;

			 inputValue = "";
			 drawAnalogModulate();
			 break;*/
			/*case KEY_FUNC_FCTN_DIGITAL_PLUSE_WIDTH:
			 name = "Pluse Width:";
			 inputValue = "";

			 strcpy(tempValue, floatToStringDot3(valueToDValue(sysData.analog.pluse.width), temp));
			 value = QString(tempValue).trimmed();
			 unit = demodTimeToQUnit(sysData.analog.pluse.width).trimmed();

			 sysData.inputLength = MAXTIMEINPUTLENGTH;

			 inputValue = "";
			 drawAnalogModulate();
			 break;*/

			//Pluse
		case KEY_FUNC_FCTN_PLUSE_PERIOD:
			if (sysData.pluse.plusesource == psInternal)
			{
				name = "Period:";
				value = doubleToQValue(sysData.pluse.period * 1e6);
				value = insertThousandsQS(value);
				unit = timeToQUnit(sysData.pluse.period);
				inputValue = "";
				sysData.inputLength = MAXPLUSEINPUTLENGTH;
				drawPluseModulate();
			}
			break;

		case KEY_FUNC_FCTN_PLUSE_WIDTH:
			if (sysData.pluse.plusesource == psInternal)
			{
				name = "Width:";
				value = doubleToQValue(sysData.pluse.width * 1e6);
				value = insertThousandsQS(value);
				unit = timeToQUnit(sysData.pluse.width);
				inputValue = "";
				sysData.inputLength = MAXPLUSEINPUTLENGTH;
				drawPluseModulate();
			}
			break;

			//LF
		case KEY_FUNC_FCTN_LF_AMPL:
			name = "LF Ampl:";
			value = doubleToQValue(sysData.lf.ampl);
			value = value.trimmed();
			unit = vppToQUnit(sysData.lf.ampl).trimmed();

			inputValue = "";
			sysData.inputLength = MAXLFINPUTLENGTH;

			drawArbitraryWave();
			break;

		case KEY_FUNC_FCTN_LF_FREQ:
			name = "LF Freq:";
			inputValue = "";

			strcpy(tempValue, getFreqStringInLF(sysData.lf.freq, temp));
			insertThousands(tempValue);
			value = QString(tempValue).trimmed();
			unit = freqToQUnit(sysData.lf.freq).trimmed();

			sysData.inputLength = MAXFREQINPUTLENGTH;
			drawArbitraryWave();
			break;

			//DRG
		case KEY_FUNC_FCTN_DRG_UPLIMIT:
			name = "Up Limit:";
			inputValue = "";
			strcpy(tempValue, getFreqString(sysData.drg.uplimit, temp));
			insertThousands(tempValue);
			value = QString(tempValue).trimmed();
			unit = freqToQUnit(sysData.drg.uplimit).trimmed();
			sysData.inputLength = MAXDRGFREQINPUTLENGTH;
			drawDrgFunction();
			break;
		case KEY_FUNC_FCTN_DRG_DOWNLIMIT:
			name = "Down Limit:";
			inputValue = "";
			strcpy(tempValue, getFreqString(sysData.drg.downlimit, temp));
			insertThousands(tempValue);
			value = QString(tempValue).trimmed();
			unit = freqToQUnit(sysData.drg.downlimit).trimmed();
			sysData.inputLength = MAXDRGFREQINPUTLENGTH;
			drawDrgFunction();
			break;
		case KEY_FUNC_FCTN_DRG_POINTS:
			name = "Points:";
			strcpy(tempValue, intToString(sysData.drg.points, temp));
			value = QString(tempValue).trimmed();
			inputValue = "";
			sysData.inputLength = MAXPOINTINPUTLENGTH;
			drawDrgFunction();
			break;
		case KEY_FUNC_FCTN_DRG_MORE_RATE:
			name = "Rate:";
			value = doubleToQValue(sysData.drg.rate * 1e6);
			value = insertThousandsQS(value);
			unit = timeToQUnit(sysData.drg.rate);
			inputValue = "";
			sysData.inputLength = MAXDRGTIMEINPUTLENGTH;
			drawDrgFunction();
			break;

			//AUX
		case KEY_FUNC_AUX_LAN_IP:
			name = "IP:";
			inputValue = "";

			value = QString(sysData.system.io.lan.ip).trimmed();
			unit = "";
			sysData.inputLength = 15;
			drawSystemPara();
			break;

		case KEY_FUNC_AUX_LAN_MASK:
			name = "Mask:";
			inputValue = "";

			value = QString(sysData.system.io.lan.mask).trimmed();
			unit = "";
			sysData.inputLength = 15;
			drawSystemPara();
			break;

		case KEY_FUNC_AUX_LAN_GATEWAY:
			name = "Gateway:";
			inputValue = "";

			value = QString(sysData.system.io.lan.gate).trimmed();
			unit = "";
			sysData.inputLength = 15;
			drawSystemPara();
			break;

		case KEY_FUNC_AUX_SERVICE: //系统服务
			name = "Pass Word:";
			value = "******";
			unit = "";
			sysData.inputLength = 6;
			break;

			//ADD BY STARX
		case KEY_FUNC_FCTN_EXTRF_FREQ:
			name = "EXT IF Freq:";
			inputValue = "";

			strcpy(tempValue, getFreqString(sysData.exfRef.freq, temp));
			insertThousands(tempValue);
			value = QString(tempValue).trimmed();
			unit = freqToQUnit(sysData.exfRef.freq).trimmed();

			sysData.inputLength = MAXFREQINPUTLENGTH;

			drawExfFref();

			break;

		case KEY_FUNC_AUX_SERVICE_LANMACADDR:
			name = "Mac Address:";
			inputValue = "";
			for (int i = 0; i < sizeof sysData.system.lanMacAddress / sizeof sysData.system.lanMacAddress[0]; i++)
			{
				intToHex(sysData.system.lanMacAddress[i], tempMac);
				value += QString(tempMac[0]);

			}
			//	  sprintf(tempValue, "%c", sysData.system.lanMacAddress);
			//	  value = QString(tempValue);
			unit = "";
			sysData.system.lanMacAddrIndex = 11;
			sysData.inputLength = 12;
			break;
	}

	strcpy(sysData.name, name.toStdString().c_str());
	strcpy(sysData.value, value.toStdString().c_str());
	strcpy(sysData.unit, unit.toStdString().c_str());

	drawInputParamterNoParamter();
}

//处理输入键
void mainForm::handleListKey(int funcCode)
{
	int tempValue = 0;

	switch (funcCode)
	{
	}
}

//处理进入键
void mainForm::handleMoreKey(int funcCode)
{
	//  sysData.EnterPara.Display = false;

	switch (funcCode)
	{
		case KEY_FUNC_FCTN_ANALOG:
			drawAnalogModulate();
			break;

		case KEY_FUNC_FCTN_DIGITAL:
			drawDigitalModulate();
			break;

		case KEY_FUNC_FCTN_PLUSE:
			drawPluseModulate();
			break;

		case KEY_FUNC_FCTN_LF:
			drawArbitraryWave();
			break;

		case KEY_FUNC_FCTN_EXTRF:
			drawExfFref();
			break;

		case KEY_FUNC_AUX_LAN:
			drawSystemPara();
			break;

		case KEY_FUNC_FCTN_DRG:
			drawDrgFunction();
			break;

		case KEY_FUNC_SWEEP_MORE_CONFIGLIST:
			drawsListData();
			//		drawSweep(sysData.sweep.type);
			break;
	}

	drawInputParamterNoParamter();
}

//处理取反键
void mainForm::handleReverseKey(int funcCode)
{
	double tempValue = 0;

}

//处理单位键
void mainForm::handleUnitKey(int funcCode, QString handleValue)
{
	bool isOk = false;

	double tempValue = 0;
	char tempChar[64] = { };
	char temp[64] = { };
	QString SendToMSG1030QS;
	char SendToMSG1030CH[64] = { };

	while (handleValue.indexOf(' ') > 0)
	{
		handleValue.remove(handleValue.indexOf(' '), 1);
	}

	handleValue.toDouble(&isOk);

	switch (funcCode)
	{
		//case KEY_FUNC_SWEEP_MORE_CONFIGLIST_MORE_GOTOROW_ENTER:
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_GOTOROW_ENTER:
			sysData.sweep.list.selectRow = handleValue.toDouble() - 1;
			if (sysData.sweep.list.selectRow >= sysData.sweep.list.allRow)
			{
				sysData.sweep.list.selectRow = sysData.sweep.list.allRow - 1;
			} else if (sysData.sweep.list.selectRow < 0)
			{
				sysData.sweep.list.selectRow = 0;
			}

			sysData.sweep.list.displayPage = sysData.sweep.list.selectRow / 4;
			sysData.sweep.list.allIndex = sysData.sweep.list.selectRow * 3 + sysData.sweep.list.selectLine;

			drawsListData();
			break;

		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_EDITITEM_GHZS:
			if (sysData.sweep.list.selectLine == 0) //如果为频率值
			{
				sysData.sweep.list.data[sysData.sweep.list.selectRow].freq = handleValue.toDouble() * 1e9;

				if (sysData.sweep.list.data[sysData.sweep.list.selectRow].freq < MINFREQ)
				{
					sysData.sweep.list.data[sysData.sweep.list.selectRow].freq = MINFREQ;
				} else if (sysData.sweep.list.data[sysData.sweep.list.selectRow].freq > MAXFREQ)
				{
					sysData.sweep.list.data[sysData.sweep.list.selectRow].freq = MAXFREQ;
				}

				strcpy(sysData.name, "Freq:");
				inputValue = "";
				strcpy(tempChar, getFreqString(sysData.sweep.list.data[sysData.sweep.list.selectRow].freq, temp));
				insertThousands(tempChar);
				strcpy(sysData.value, tempChar);
				strcpy(sysData.unit, freqToQUnit(sysData.sweep.list.data[sysData.sweep.list.selectRow].freq).trimmed().toStdString().c_str());

				sysData.inputLength = MAXFREQINPUTLENGTH;
			} else if (sysData.sweep.list.selectLine == 2) //如果为驻留时间
			{
				sysData.sweep.list.data[sysData.sweep.list.selectRow].dwell = handleValue.toDouble() * 1e3;
				if (sysData.sweep.list.data[sysData.sweep.list.selectRow].dwell < 20)
					sysData.sweep.list.data[sysData.sweep.list.selectRow].dwell = 20;
				else if (sysData.sweep.list.data[sysData.sweep.list.selectRow].dwell > 50e3)
					sysData.sweep.list.data[sysData.sweep.list.selectRow].dwell = 50e3;

				strcpy(sysData.name, "Dwell:");
				strcpy(sysData.value, floatToStringDot3(sysData.sweep.list.data[sysData.sweep.list.selectRow].dwell, temp));
				strcpy(sysData.unit, timeToQUnit(sysData.sweep.list.data[sysData.sweep.list.selectRow].dwell).trimmed().toStdString().c_str());

				inputValue = "";
				sysData.inputLength = MAXTIMEINPUTLENGTH;
			}

			sysScpi->setSweepListDownload();
			drawInputParamterNoParamter();
			drawsListData();
			break;

		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_EDITITEM_MHZMS:
			if (sysData.sweep.list.selectLine == 0) //如果为频率值
			{
				sysData.sweep.list.data[sysData.sweep.list.selectRow].freq = handleValue.toDouble() * 1e6;

				if (sysData.sweep.list.data[sysData.sweep.list.selectRow].freq < MINFREQ)
				{
					sysData.sweep.list.data[sysData.sweep.list.selectRow].freq = MINFREQ;
				} else if (sysData.sweep.list.data[sysData.sweep.list.selectRow].freq > MAXFREQ)
				{
					sysData.sweep.list.data[sysData.sweep.list.selectRow].freq = MAXFREQ;
				}

				strcpy(sysData.name, "Freq:");
				inputValue = "";
				strcpy(tempChar, getFreqString(sysData.sweep.list.data[sysData.sweep.list.selectRow].freq, temp));
				insertThousands(tempChar);
				strcpy(sysData.value, tempChar);
				strcpy(sysData.unit, freqToQUnit(sysData.sweep.list.data[sysData.sweep.list.selectRow].freq).trimmed().toStdString().c_str());

				sysData.inputLength = MAXFREQINPUTLENGTH;
			} else if (sysData.sweep.list.selectLine == 2) //如果为驻留时间
			{
				sysData.sweep.list.data[sysData.sweep.list.selectRow].dwell = handleValue.toDouble();
				sysData.sweep.list.data[sysData.sweep.list.selectRow].dwell = handleValue.toDouble();
				if (sysData.sweep.list.data[sysData.sweep.list.selectRow].dwell < 20)
					sysData.sweep.list.data[sysData.sweep.list.selectRow].dwell = 20;
				else if (sysData.sweep.list.data[sysData.sweep.list.selectRow].dwell > 50e3)
					sysData.sweep.list.data[sysData.sweep.list.selectRow].dwell = 50e3;

				strcpy(sysData.name, "Dwell:");
				strcpy(sysData.value, floatToStringDot3(sysData.sweep.list.data[sysData.sweep.list.selectRow].dwell, temp));
				strcpy(sysData.unit, timeToQUnit(sysData.sweep.list.data[sysData.sweep.list.selectRow].dwell).trimmed().toStdString().c_str());

				inputValue = "";
				sysData.inputLength = MAXTIMEINPUTLENGTH;
			}

			sysScpi->setSweepListDownload();
			drawInputParamterNoParamter();
			drawsListData();
			break;

		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_EDITITEM_KHZNS:
			if (sysData.sweep.list.selectLine == 0) //如果为频率值
			{
				sysData.sweep.list.data[sysData.sweep.list.selectRow].freq = handleValue.toDouble() * 1e3;

				if (sysData.sweep.list.data[sysData.sweep.list.selectRow].freq < MINFREQ)
				{
					sysData.sweep.list.data[sysData.sweep.list.selectRow].freq = MINFREQ;
				} else if (sysData.sweep.list.data[sysData.sweep.list.selectRow].freq > MAXFREQ)
				{
					sysData.sweep.list.data[sysData.sweep.list.selectRow].freq = MAXFREQ;
				}

				strcpy(sysData.name, "Freq:");
				inputValue = "";
				strcpy(tempChar, getFreqString(sysData.sweep.list.data[sysData.sweep.list.selectRow].freq, temp));
				insertThousands(tempChar);
				strcpy(sysData.value, tempChar);
				strcpy(sysData.unit, freqToQUnit(sysData.sweep.list.data[sysData.sweep.list.selectRow].freq).trimmed().toStdString().c_str());

				sysData.inputLength = MAXFREQINPUTLENGTH;
			}
			sysScpi->setSweepListDownload();
			drawsListData();
			break;

		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_EDITITEM_HZDBM:
			if (sysData.sweep.list.selectLine == 0) //如果为频率值
			{
				sysData.sweep.list.data[sysData.sweep.list.selectRow].freq = handleValue.toDouble();

				if (sysData.sweep.list.data[sysData.sweep.list.selectRow].freq < MINFREQ)
				{
					sysData.sweep.list.data[sysData.sweep.list.selectRow].freq = MINFREQ;
				} else if (sysData.sweep.list.data[sysData.sweep.list.selectRow].freq > MAXFREQ)
				{
					sysData.sweep.list.data[sysData.sweep.list.selectRow].freq = MAXFREQ;
				}

				strcpy(sysData.name, "Freq:");
				inputValue = "";
				strcpy(tempChar, getFreqString(sysData.sweep.list.data[sysData.sweep.list.selectRow].freq, temp));
				insertThousands(tempChar);
				strcpy(sysData.value, tempChar);
				strcpy(sysData.unit, freqToQUnit(sysData.sweep.list.data[sysData.sweep.list.selectRow].freq).trimmed().toStdString().c_str());

				sysData.inputLength = MAXFREQINPUTLENGTH;
			} else if (sysData.sweep.list.selectLine == 1) //如果为功率
			{
				sysData.sweep.list.data[sysData.sweep.list.selectRow].ampl = handleValue.toDouble();
				//	    sysData.sweep.list.data[sysData.sweep.list.selectRow].dwell = handleValue.toDouble() * 1e3;
				if (sysData.sweep.list.data[sysData.sweep.list.selectRow].ampl < -120)
					sysData.sweep.list.data[sysData.sweep.list.selectRow].ampl = -120;
				else if (sysData.sweep.list.data[sysData.sweep.list.selectRow].ampl > 10)
					sysData.sweep.list.data[sysData.sweep.list.selectRow].ampl = 10;

				strcpy(sysData.name, "Start Ampl:");
				strcpy(sysData.value, floatToStringDot2(sysData.sweep.list.data[sysData.sweep.list.selectRow].ampl, temp));
				strcpy(sysData.unit, amplToQUnit(sysData.sweep.list.data[sysData.sweep.list.selectRow].ampl).trimmed().toStdString().c_str());

				inputValue = "";
				sysData.inputLength = MAXAMDEPTH;
			}

			sysScpi->setSweepListDownload();
			drawInputParamterNoParamter();
			drawsListData();
			break;

		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_MORE_DOWNLOAD_ENTER:

			analysisListdata((int) handleValue.toDouble());
			//		if(system("umount /mnt/usb/") != 0)
			//		{
			//			return ;
			//		}
			drawsListData();
			break;

		case KEY_FUNC_FCTN_DIGITAL_MORE_LOAD_ENTER:
			analysisDigiSequencedata((int) handleValue.toDouble());
			sysScpi->setDigtalSequenceDownload();
			break;

		case KEY_FUNC_FCTN_EXTRF_FREQ_GHZ:
			if (isOk)
			{
				//		sysData.EnterPara.CursorStatus = CLOSE;
				sysScpi->setExtRefFrequency(handleValue.toDouble() * 1e9);

				inputValue = "";
				sysData.inputLength = MAXFREQINPUTLENGTH;

				drawInputParamterNoParamter();
				drawExfFref();
			}
			break;

		case KEY_FUNC_FCTN_EXTRF_FREQ_MHZ:
			if (isOk)
			{
				//		sysData.EnterPara.CursorStatus = CLOSE;
				sysScpi->setExtRefFrequency(handleValue.toDouble() * 1e6);

				inputValue = "";

				sysData.inputLength = MAXFREQINPUTLENGTH;

				drawInputParamterNoParamter();
				drawExfFref();
			}
			break;

		case KEY_FUNC_FCTN_EXTRF_FREQ_KHZ:
			if (isOk)
			{
				//		sysData.EnterPara.CursorStatus = CLOSE;
				sysScpi->setExtRefFrequency(handleValue.toDouble() * 1e3);

				inputValue = "";

				sysData.inputLength = MAXFREQINPUTLENGTH;

				drawInputParamterNoParamter();
				drawExfFref();
			}
			break;

		case KEY_FUNC_FCTN_EXTRF_FREQ_HZ:
			if (isOk)
			{
				//		sysData.EnterPara.CursorStatus = CLOSE;
				sysScpi->setExtRefFrequency(handleValue.toDouble());

				inputValue = "";
				sysData.inputLength = MAXFREQINPUTLENGTH;

				drawInputParamterNoParamter();
				drawExfFref();
			}
			break;

		case KEY_FUNC_FREQ_GHZ:
			if (isOk)
			{
				//		sysData.EnterPara.CursorStatus = CLOSE;
				sysScpi->setFrequency(handleValue.toDouble() * 1e9);

				inputValue = "";
				sysData.inputLength = MAXFREQINPUTLENGTH;

				drawInputParamterNoParamter();
				drawFreqAndAmplMain();
			}
			break;

		case KEY_FUNC_FREQ_MHZ:
			if (isOk)
			{
				//		sysData.EnterPara.CursorStatus = CLOSE;
				sysScpi->setFrequency(handleValue.toDouble() * 1e6);

				inputValue = "";
				sysData.inputLength = MAXFREQINPUTLENGTH;

				drawInputParamterNoParamter();
				drawFreqAndAmplMain();
			}
			break;

		case KEY_FUNC_FREQ_KHZ:
			if (isOk)
			{
				//			sysData.EnterPara.CursorStatus = CLOSE;
				sysScpi->setFrequency(handleValue.toDouble() * 1e3);

				inputValue = "";
				sysData.inputLength = MAXFREQINPUTLENGTH;

				drawInputParamterNoParamter();
				drawFreqAndAmplMain();
			}
			break;

		case KEY_FUNC_FREQ_HZ:
			if (isOk)
			{
				//		sysData.EnterPara.CursorStatus = CLOSE;
				sysScpi->setFrequency(handleValue.toDouble());

				inputValue = "";
				sysData.inputLength = MAXFREQINPUTLENGTH;

				drawInputParamterNoParamter();
				drawFreqAndAmplMain();
			}
			break;

		case KEY_FUNC_FREQOFFSET_GHZ:
			if (isOk)
			{
				//		sysData.EnterPara.CursorStatus = CLOSE;
				sysScpi->setFrequencyOffset(handleValue.toDouble() * 1e9);

				inputValue = "";

				sysData.inputLength = MAXFREQINPUTLENGTH;

				drawFreqAndAmplMain();
				//		drawFreqAndAmpl();
				drawFreqInParamter();
				drawInputParamterNoParamter();
			}
			break;

		case KEY_FUNC_FREQOFFSET_MHZ:
			if (isOk)
			{
				sysScpi->setFrequencyOffset(handleValue.toDouble() * 1e6);

				inputValue = "";

				sysData.inputLength = MAXFREQINPUTLENGTH;

				drawInputParamterNoParamter();
				drawFreqAndAmplMain();
				//		drawFreqAndAmpl();
				drawFreqInParamter();
			}
			break;

		case KEY_FUNC_FREQOFFSET_KHZ:
			if (isOk)
			{
				sysScpi->setFrequencyOffset(handleValue.toDouble() * 1e3);

				inputValue = "";
				sysData.inputLength = MAXFREQINPUTLENGTH;

				drawInputParamterNoParamter();
				drawFreqAndAmplMain();
				//		drawFreqAndAmpl();
				drawFreqInParamter();
			}
			break;

		case KEY_FUNC_FREQOFFSET_HZ:
			if (isOk)
			{
				//		sysData.EnterPara.CursorStatus = CLOSE;
				sysScpi->setFrequencyOffset(handleValue.toDouble());

				inputValue = "";

				sysData.inputLength = MAXFREQINPUTLENGTH;

				drawInputParamterNoParamter();
				drawFreqAndAmplMain();
				//		drawFreqAndAmpl();
				drawFreqInParamter();
			}
			break;

		case KEY_FUNC_FREQMULTIPLIER_ENTER:
			if (isOk)
			{
				//		sysData.EnterPara.CursorStatus = CLOSE;
				sysScpi->setFrequencyMultipliter(handleValue.toDouble());

				inputValue = "";

				drawInputParamterNoParamter();
				drawFreqAndAmplMain();
				//		drawFreqAndAmpl();
				drawFreqInParamter();
			}
			break;

		case KEY_FUNC_AMPL_DBM:
			if (isOk)
			{
				sysData.ampl.amplUnit = audBm;
				//		strcpy(sysData.unit, amplToQUnit(1).trimmed().toStdString().c_str());
				sysData.ampl.amplInit = handleValue.toDouble(); //保存原始值
				sysScpi->setAmplitude(handleValue.toDouble());
				//		sysData.ampl.amplinitUnit = audBm;
				inputValue = "";
				sysData.inputLength = MAXAMDEPTH;
				//		strcpy(sysData.unitAmpl, "dBm");

				drawInputParamterNoParamter();
				drawFreqAndAmplMain();
				//		drawFreqAndAmpl();
				drawAmplInParamter();
			}
			break;

		case KEY_FUNC_AMPL_DBUV:
			if (isOk)
			{
				sysData.ampl.amplUnit = audBuV;
				//		sysScpi->setAmplitude(handleValue.toDouble()-107);
				//		strcpy(sysData.unit, amplToQUnit(2).trimmed().toStdString().c_str());
				sysData.ampl.amplInit = handleValue.toDouble(); //保存原始值
				sysScpi->setAmplitude(handleValue.toDouble());
				//		sysData.ampl.amplinitUnit = audBuV;

				inputValue = "";
				sysData.inputLength = MAXAMDEPTH;
				//		strcpy(sysData.unitAmpl, "dBuV");

				drawInputParamterNoParamter();
				drawFreqAndAmplMain();
				//		drawFreqAndAmpl();
				drawAmplInParamter();
			}
			break;
		case KEY_FUNC_AMPL_MV:
			if (isOk)
			{
				sysData.ampl.amplUnit = aumV;
				sysData.ampl.amplInit = handleValue.toDouble(); //保存原始值
				sysScpi->setAmplitude(handleValue.toDouble());
				strcpy(sysData.value, handleValue.trimmed().toStdString().c_str());
				//		sysData.ampl.amplinitUnit = aumV;
				inputValue = "";
				sysData.inputLength = MAXAMDEPTH;
				//		strcpy(sysData.unitAmpl, "mV");
				drawInputParamterNoParamter();
				drawFreqAndAmplMain();
				//		drawFreqAndAmpl();
				drawAmplInParamter();
			}
			break;

			//	case KEY_FUNC_AMPL_ERROR_DBM:
			//	  if (isOk)
			//	  {
			//		sysScpi->setAmplitudeError(handleValue.toDouble());
			//
			//		inputValue = "";
			//		sysData.inputLength = MAXAMDEPTH;
			//
			//		drawInputParamterNoParamter();
			//		drawFreqAndAmplMain();
			//		drawFreqAndAmpl();
			//	  }
			//	  break;

		case KEY_FUNC_AMPLOFFSET_DBM:
			if (isOk)
			{
				sysScpi->setAmplitudeOffset(handleValue.toDouble());
				inputValue = "";
				sysData.inputLength = MAXAMPLINPUTLENGTH;

				drawInputParamterNoParamter();
				drawFreqAndAmplMain();
				//		drawFreqAndAmpl();
				drawAmplInParamter();

			}
			break;

		case KEY_FUNC_AMPL_CABLE_DB:
			if (isOk)
			{
				sysScpi->setAmplitudeError(handleValue.toDouble());
				inputValue = "";
				sysData.inputLength = MAXAMPLINPUTLENGTH;

				drawInputParamterNoParamter();
				drawFreqAndAmplMain();
				drawAmplInParamter();
			}
			break;
			//-----------------------------------------------------------------------------------
		case KEY_FUNC_SWEEP_STEP_MORE_STEPDWELL_MS:
			if (isOk)
			{
				sysScpi->setStepDwell(handleValue.toDouble());
				inputValue = "";
				sysData.inputLength = MAXTIMEINPUTLENGTH;

				drawInputParamterNoParamter();
				drawSweep(sysData.sweep.type);
			}
			break;

		case KEY_FUNC_SWEEP_STEP_MORE_STEPDWELL_S:
			if (isOk)
			{
				sysScpi->setStepDwell(handleValue.toDouble() * 1e3);

				inputValue = "";
				sysData.inputLength = MAXTIMEINPUTLENGTH;

				drawInputParamterNoParamter();
				drawSweep(sysData.sweep.type);
			}
			break;

			//---------------------------------------------------------------------------------------------------
		case KEY_FUNC_SWEEP_STEP_FREQSTART_GHZ:
			if (isOk)
			{
				sysScpi->setStepStartFrequency(handleValue.toDouble() * 1e9);
				sysData.inputLength = MAXFREQINPUTLENGTH;
				inputValue = "";

				drawInputParamterNoParamter();
				drawSweep(sysData.sweep.type);
			}
			break;

		case KEY_FUNC_SWEEP_STEP_FREQSTART_MHZ:
			if (isOk)
			{
				sysScpi->setStepStartFrequency(handleValue.toDouble() * 1e6);

				sysData.inputLength = MAXFREQINPUTLENGTH;
				inputValue = "";

				drawInputParamterNoParamter();
				drawSweep(sysData.sweep.type);
			}
			break;

		case KEY_FUNC_SWEEP_STEP_FREQSTART_KHZ:
			if (isOk)
			{
				sysScpi->setStepStartFrequency(handleValue.toDouble() * 1e3);
				sysData.inputLength = MAXFREQINPUTLENGTH;
				inputValue = "";
				drawInputParamterNoParamter();
				drawSweep(sysData.sweep.type);
			}
			break;

		case KEY_FUNC_SWEEP_STEP_FREQSTART_HZ:
			if (isOk)
			{
				sysScpi->setStepStartFrequency(handleValue.toDouble());
				sysData.inputLength = MAXFREQINPUTLENGTH;
				inputValue = "";
				drawSweep(sysData.sweep.type);
			}
			break;

		case KEY_FUNC_SWEEP_STEP_FREQSTOP_GHZ:
			if (isOk)
			{
				//				sysData.EnterPara.CursorStatus = CLOSE;
				inputValue = "";
				sysScpi->setStepStopFrequency(handleValue.toDouble() * 1e9);
				sysData.inputLength = MAXFREQINPUTLENGTH;

				drawInputParamterNoParamter();
				drawSweep(sysData.sweep.type);
			}
			break;

		case KEY_FUNC_SWEEP_STEP_FREQSTOP_MHZ:
			if (isOk)
			{
				sysScpi->setStepStopFrequency(handleValue.toDouble() * 1e6);
				sysData.inputLength = MAXFREQINPUTLENGTH;
				inputValue = "";

				drawInputParamterNoParamter();
				drawSweep(sysData.sweep.type);
			}
			break;

		case KEY_FUNC_SWEEP_STEP_FREQSTOP_KHZ:
			if (isOk)
			{
				sysScpi->setStepStopFrequency(handleValue.toDouble() * 1e3);
				sysData.inputLength = MAXFREQINPUTLENGTH;
				inputValue = "";

				drawInputParamterNoParamter();
				drawSweep(sysData.sweep.type);
			}
			break;

		case KEY_FUNC_SWEEP_STEP_FREQSTOP_HZ:
			if (isOk)
			{
				sysScpi->setStepStopFrequency(handleValue.toDouble());

				sysData.inputLength = MAXFREQINPUTLENGTH;
				inputValue = "";

				drawInputParamterNoParamter();
				drawSweep(sysData.sweep.type);
			}
			break;

			//-------------------------------------------------------------------------------------------
		case KEY_FUNC_SWEEP_STEP_AMPLSTART_DBM:
			if (isOk)
			{
				//		sysData.EnterPara.CursorStatus = CLOSE;
				sysScpi->setStepStartAmplitude(handleValue.toDouble());
				inputValue = "";
				sysData.inputLength = MAXAMPLINPUTLENGTH;
				drawInputParamterNoParamter();

				drawSweep(sysData.sweep.type);
			}
			break;

			//-------------------------------------------------------------------------------------------
		case KEY_FUNC_SWEEP_STEP_AMPLSTOP_DBM:
			if (isOk)
			{
				sysScpi->setStepStopAmplitude(handleValue.toDouble());

				inputValue = "";
				sysData.inputLength = MAXAMPLINPUTLENGTH;

				drawInputParamterNoParamter();

				drawSweep(sysData.sweep.type);
			}
			break;

			//-------------------------------------------------------------------------------------------
		case KEY_FUNC_SWEEP_STEP_MORE_POINTS_ENTER:
			if (isOk)
			{
				sysScpi->setStepPoints(handleValue.toDouble());

				inputValue = "";
				sysData.inputLength = MAXPOINTINPUTLENGTH;

				drawInputParamterNoParamter();

				drawSweep(sysData.sweep.type);
			}
			break;

			//-------------------------------------------------------------------------------------------
		case KEY_FUNC_FCTN_ANALOG_AM_RATE_KHZ:
			if (isOk)
			{
				sysScpi->setAmRate(handleValue.toDouble() * 1e3);

				sysData.inputLength = MAXFREQINPUTLENGTH;

				inputValue = "";

				drawInputParamterNoParamter();
				drawAnalogModulate();
			}
			break;

			//-------------------------------------------------------------------------------------------
		case KEY_FUNC_FCTN_ANALOG_AM_RATE_HZ:
			if (isOk)
			{
				sysScpi->setAmRate(handleValue.toDouble());
				sysData.inputLength = MAXFREQINPUTLENGTH;

				inputValue = "";

				drawInputParamterNoParamter();
				drawAnalogModulate();
			}
			break;

			//-------------------------------------------------------------------------------------------
		case KEY_FUNC_FCTN_ANALOG_FM_RATE_KHZ:
			if (isOk)
			{
				//		sysData.EnterPara.CursorStatus = CLOSE;
				sysScpi->setFmRate(handleValue.toDouble() * 1e3);
				sysData.inputLength = MAXFREQINPUTLENGTH;

				inputValue = "";

				drawInputParamterNoParamter();
				drawAnalogModulate();
			}

			break;

			//-------------------------------------------------------------------------------------------
		case KEY_FUNC_FCTN_ANALOG_FM_RATE_HZ:
			if (isOk)
			{
				sysScpi->setFmRate(handleValue.toDouble());

				sysData.inputLength = MAXFREQINPUTLENGTH;

				inputValue = "";

				drawInputParamterNoParamter();
				drawAnalogModulate();
			}
			break;

			//-------------------------------------------------------------------------------------------
		case KEY_FUNC_FCTN_ANALOG_PM_RATE_KHZ:
			if (isOk)
			{
				//		sysData.EnterPara.CursorStatus = CLOSE;
				sysScpi->setPmRate(handleValue.toDouble() * 1e3);
				sysData.inputLength = MAXFREQINPUTLENGTH;

				inputValue = "";

				drawInputParamterNoParamter();
				drawAnalogModulate();
			}
			break;

			//-------------------------------------------------------------------------------------------
		case KEY_FUNC_FCTN_ANALOG_PM_RATE_HZ:
			if (isOk)
			{
				sysScpi->setPmRate(handleValue.toDouble());
				sysData.inputLength = MAXFREQINPUTLENGTH;

				inputValue = "";

				drawInputParamterNoParamter();
				drawAnalogModulate();
			}
			break;
			//-------------------------------------------------------------------------------------------
		case KEY_FUNC_FCTN_DIGITAL_PARAMETER_ASKDEEP_PERCENT:
			if (isOk)
			{
				sysScpi->setDigitalModulatePara(handleValue.toDouble());
				sysData.inputLength = MAXFREQINPUTLENGTH;
				inputValue = "";
				drawInputParamterNoParamter();
				drawDigitalModulate();
			}
			break;
			//-------------------------------------------------------------------------------------------
		case KEY_FUNC_FCTN_DIGITAL_PARAMETER_FSKOFFSET_MHZ:
			if (isOk)
			{
				sysScpi->setDigitalModulatePara(handleValue.toDouble() * 1e6);
				sysData.inputLength = MAXFREQINPUTLENGTH;
				inputValue = "";
				drawInputParamterNoParamter();
				drawDigitalModulate();
			}
			break;

		case KEY_FUNC_FCTN_DIGITAL_PARAMETER_FSKOFFSET_KHZ:
			if (isOk)
			{
				sysScpi->setDigitalModulatePara(handleValue.toDouble() * 1e3);
				sysData.inputLength = MAXFREQINPUTLENGTH;
				inputValue = "";
				drawInputParamterNoParamter();
				drawDigitalModulate();
			}
			break;

		case KEY_FUNC_FCTN_DIGITAL_PARAMETER_FSKOFFSET_HZ:
			if (isOk)
			{
				sysScpi->setDigitalModulatePara(handleValue.toDouble());
				sysData.inputLength = MAXFREQINPUTLENGTH;
				inputValue = "";
				drawInputParamterNoParamter();
				drawDigitalModulate();
			}
			break;
			//-------------------------------------------------------------------------------------------
		case KEY_FUNC_FCTN_DIGITAL_PARAMETER_PSKDEVI_ANGLE:
			if (isOk)
			{
				sysScpi->setDigitalModulatePara(handleValue.toDouble());
				sysData.inputLength = MAXFREQINPUTLENGTH;
				inputValue = "";
				drawInputParamterNoParamter();
				drawDigitalModulate();
			}
			break;

		case KEY_FUNC_FCTN_DIGITAL_PARAMETER_RATE_KHZ:
			if (isOk)
			{
				sysScpi->setDigitalModuateRate(handleValue.toDouble() * 1e3);
				sysData.inputLength = MAXFREQINPUTLENGTH;
				inputValue = "";
				drawInputParamterNoParamter();
				drawDigitalModulate();
			}
			break;

		case KEY_FUNC_FCTN_DIGITAL_PARAMETER_RATE_HZ:
			if (isOk)
			{
				sysScpi->setDigitalModuateRate(handleValue.toDouble());
				sysData.inputLength = MAXFREQINPUTLENGTH;
				inputValue = "";
				drawInputParamterNoParamter();
				drawDigitalModulate();
			}
			break;
			//-------------------------------------------------------------------------------------------
			//	case KEY_FUNC_FCTN_DIGITAL_ASK_RATE_KHZ:
			//	  if (isOk)
			//	  {
			////		sysData.EnterPara.CursorStatus = CLOSE;
			//		sysScpi->setAskRate(handleValue.toDouble() * 1e3);
			//
			//		sysData.inputLength = MAXFREQINPUTLENGTH;
			//
			//		inputValue = "";
			//
			// 	    drawInputParamterNoParamter();
			//		drawDigitalModulate();
			//	  }
			//	  break;

			//-------------------------------------------------------------------------------------------
			//	case KEY_FUNC_FCTN_DIGITAL_ASK_RATE_HZ:
			//	  if (isOk)
			//	  {
			////		sysData.EnterPara.CursorStatus = CLOSE;
			//		sysScpi->setAskRate(handleValue.toDouble());
			//
			//		sysData.inputLength = MAXFREQINPUTLENGTH;
			//
			//		inputValue = "";
			//
			// 	    drawInputParamterNoParamter();
			//		drawDigitalModulate();
			//	  }
			//	  break;


			//	case KEY_FUNC_FCTN_DIGITAL_FSK_RATE_MHZ:
			//      if (isOk)
			//      {
			////		sysData.EnterPara.CursorStatus = CLOSE;
			//    	sysScpi->setFskRate(handleValue.toDouble() * 1e6);
			//
			//    	sysData.inputLength = MAXFREQINPUTLENGTH;
			//
			//    	inputValue = "";
			//
			// 	    drawInputParamterNoParamter();
			//    	drawDigitalModulate();
			//      }
			//      break;

			//-------------------------------------------------------------------------------------------
			//	case KEY_FUNC_FCTN_DIGITAL_FSK_RATE_KHZ:
			//	  if (isOk)
			//	  {
			//		sysScpi->setFskRate(handleValue.toDouble() * 1e3);
			//
			//    	sysData.inputLength = MAXFREQINPUTLENGTH;
			//
			//    	inputValue = "";
			//
			// 	    drawInputParamterNoParamter();
			//    	drawDigitalModulate();
			//	  }
			//	  break;
			//	case KEY_FUNC_FCTN_DIGITAL_FSK_RATE_HZ:
			//	  if (isOk)
			//	  {
			//		sysScpi->setFskRate(handleValue.toDouble());
			//
			//    	sysData.inputLength = MAXFREQINPUTLENGTH;
			//
			//    	inputValue = "";
			//
			// 	    drawInputParamterNoParamter();
			//    	drawDigitalModulate();
			//	  }
			//	  break;

			//	case KEY_FUNC_FCTN_DIGITAL_PSK_RATE_KHZ:
			//	  if (isOk)
			//	  {
			////			sysData.EnterPara.CursorStatus = CLOSE;
			//		sysScpi->setPskRate(handleValue.toDouble() * 1e3);
			//
			//		sysData.inputLength = MAXFREQINPUTLENGTH;
			//
			//		inputValue = "";
			//
			// 	    drawInputParamterNoParamter();
			//		drawDigitalModulate();
			//	  }
			//	  break;

			//-------------------------------------------------------------------------------------------
			//	case KEY_FUNC_FCTN_DIGITAL_PSK_RATE_HZ:
			//	  if (isOk)
			//	  {
			//		sysScpi->setPskRate(handleValue.toDouble());
			//		sysData.inputLength = MAXFREQINPUTLENGTH;
			//
			//		inputValue = "";
			//
			// 	    drawInputParamterNoParamter();
			//		drawDigitalModulate();
			//
			//	  }
			//		  break;

			//-------------------------------------------------------------------------------------------
		case KEY_FUNC_FCTN_ANALOG_AM_DEPTH_PERCENT:
			if (isOk)
			{
				sysScpi->setAmDepth(handleValue.toDouble());
				sysData.inputLength = MAXINPUTLENGTH;
				inputValue = "";

				drawInputParamterNoParamter();
				drawAnalogModulate();
			}
			break;

			//-------------------------------------------------------------------------------------------
		case KEY_FUNC_FCTN_ANALOG_FM_DEVIATION_MHZ:
			if (isOk)
			{
				sysScpi->setFmDev(handleValue.toDouble() * 1e6);
				sysData.inputLength = MAXFREQINPUTLENGTH;

				inputValue = "";

				drawInputParamterNoParamter();
				drawAnalogModulate();
			}
			break;

			//-------------------------------------------------------------------------------------------
		case KEY_FUNC_FCTN_ANALOG_FM_DEVIATION_KHZ:
			if (isOk)
			{
				//		sysData.EnterPara.CursorStatus = CLOSE;
				sysScpi->setFmDev(handleValue.toDouble() * 1e3);

				sysData.inputLength = MAXFREQINPUTLENGTH;

				inputValue = "";

				drawInputParamterNoParamter();
				drawAnalogModulate();
			}
			break;

			//-------------------------------------------------------------------------------------------
		case KEY_FUNC_FCTN_ANALOG_FM_DEVIATION_HZ:
			if (isOk)
			{
				//		sysData.EnterPara.CursorStatus = CLOSE;
				sysScpi->setFmDev(handleValue.toDouble());

				sysData.inputLength = MAXFREQINPUTLENGTH;

				inputValue = "";

				drawInputParamterNoParamter();
				drawAnalogModulate();
			}
			break;

			//-------------------------------------------------------------------------------------------
		case KEY_FUNC_FCTN_ANALOG_PM_PHASE_ANGLE:
			if (isOk)
			{
				//		sysData.EnterPara.CursorStatus = CLOSE;
				sysScpi->setPmPhase(handleValue.toDouble());
				sysData.inputLength = MAXINPUTLENGTH;
				inputValue = "";

				drawInputParamterNoParamter();
				drawAnalogModulate();
			}
			break;

			//-------------------------------------------------------------------------------------------
			//	case KEY_FUNC_FCTN_DIGITAL_FSK_STEP_MHZ:
			//	  if (isOk)
			//	  {
			////		sysData.EnterPara.CursorStatus = CLOSE;
			//		sysScpi->setFskStep(handleValue.toDouble() * 1e6);
			//
			//		sysData.inputLength = MAXFREQINPUTLENGTH;
			//
			//		inputValue = "";
			//
			// 	    drawInputParamterNoParamter();
			//		drawDigitalModulate();
			//	  }
			//	  break;

			//-------------------------------------------------------------------------------------------
			//	case KEY_FUNC_FCTN_DIGITAL_FSK_STEP_KHZ:
			//	  if (isOk)
			//	  {
			////		sysData.EnterPara.CursorStatus = CLOSE;
			//		sysScpi->setFskStep(handleValue.toDouble() * 1e3);
			//
			//		sysData.inputLength = MAXFREQINPUTLENGTH;
			//
			//		inputValue = "";
			//
			// 	    drawInputParamterNoParamter();
			//		drawDigitalModulate();
			//	  }
			//	  break;

			//-------------------------------------------------------------------------------------------
			//	case KEY_FUNC_FCTN_DIGITAL_FSK_STEP_HZ:
			//	  if (isOk)
			//	  {
			//		sysScpi->setFskStep(handleValue.toDouble());
			//
			//		sysData.inputLength = MAXFREQINPUTLENGTH;
			//
			//		inputValue = "";
			//
			// 	    drawInputParamterNoParamter();
			//		drawDigitalModulate();
			//	  }
			//	  break;

			//---------------------------------------------------------------------
			/*case KEY_FUNC_FCTN_DIGITAL_PLUSE_PERIOD_S:
			 if (isOk)
			 {
			 //		sysData.EnterPara.CursorStatus = CLOSE;
			 sysScpi->setPlusePeriod(handleValue.toDouble() * 1e9);

			 sysData.inputLength = MAXTIMEINPUTLENGTH;

			 inputValue = "";

			 drawInputParamterNoParamter();
			 drawAnalogModulate();
			 }

			 break;*/
			/*case KEY_FUNC_FCTN_DIGITAL_PLUSE_PERIOD_MS:
			 if (isOk)
			 {
			 //		sysData.EnterPara.CursorStatus = CLOSE;
			 sysScpi->setPlusePeriod(handleValue.toDouble() * 1e6);
			 sysData.inputLength = MAXTIMEINPUTLENGTH;

			 inputValue = "";

			 drawInputParamterNoParamter();
			 drawAnalogModulate();
			 }
			 break;*/
			/*case KEY_FUNC_FCTN_DIGITAL_PLUSE_PERIOD_US:
			 if (isOk)
			 {
			 //		sysData.EnterPara.CursorStatus = CLOSE;
			 sysScpi->setPlusePeriod(handleValue.toDouble() * 1e3);
			 inputValue = "";

			 drawInputParamterNoParamter();
			 drawAnalogModulate();
			 }
			 break;*/

			/*case KEY_FUNC_FCTN_DIGITAL_PLUSE_PERIOD_NS:
			 if (isOk)
			 {
			 //		sysData.EnterPara.CursorStatus = CLOSE;
			 sysScpi->setPlusePeriod(handleValue.toDouble());

			 sysData.inputLength = MAXTIMEINPUTLENGTH;

			 inputValue = "";

			 drawInputParamterNoParamter();
			 drawAnalogModulate();
			 }
			 break;*/

			//---------------------------------------------------------------------
			/*case KEY_FUNC_FCTN_DIGITAL_PLUSE_WIDTH_S:
			 if (isOk)
			 {
			 //		sysData.EnterPara.CursorStatus = CLOSE;
			 sysScpi->setPluseWidth(handleValue.toDouble() * 1e9);

			 sysData.inputLength = MAXTIMEINPUTLENGTH;

			 inputValue = "";

			 drawInputParamterNoParamter();
			 drawAnalogModulate();
			 }

			 break;*/
			/*case KEY_FUNC_FCTN_DIGITAL_PLUSE_WIDTH_MS:
			 if (isOk)
			 {
			 //		sysData.EnterPara.CursorStatus = CLOSE;
			 sysScpi->setPluseWidth(handleValue.toDouble() * 1e6);

			 sysData.inputLength = MAXTIMEINPUTLENGTH;

			 inputValue = "";

			 drawInputParamterNoParamter();
			 drawAnalogModulate();
			 }
			 break;*/
			/*case KEY_FUNC_FCTN_DIGITAL_PLUSE_WIDTH_US:
			 if (isOk)
			 {
			 //			sysData.EnterPara.CursorStatus = CLOSE;
			 sysScpi->setPluseWidth(handleValue.toDouble() * 1e3);
			 sysData.inputLength = MAXTIMEINPUTLENGTH;

			 inputValue = "";

			 drawInputParamterNoParamter();
			 drawAnalogModulate();
			 }
			 break;*/
			/*case KEY_FUNC_FCTN_DIGITAL_PLUSE_WIDTH_NS:
			 if (isOk)
			 {
			 sysScpi->setPluseWidth(handleValue.toDouble());

			 inputValue = "";
			 sysData.inputLength = MAXTIMEINPUTLENGTH;
			 drawAnalogModulate();
			 }
			 break;*/
			//Pluse
		case KEY_FUNC_FCTN_PLUSE_PERIOD_S:
			if (isOk)
			{
				sysScpi->setPlusePeriod(handleValue.toDouble() * 1e9);
				inputValue = "";
				sysData.inputLength = MAXAMDEPTH;

				drawInputParamterNoParamter();
				drawPluseModulate();
			}
			break;

		case KEY_FUNC_FCTN_PLUSE_PERIOD_MS:
			if (isOk)
			{
				sysScpi->setPlusePeriod(handleValue.toDouble() * 1e6);
				inputValue = "";
				sysData.inputLength = MAXAMDEPTH;

				drawInputParamterNoParamter();
				drawPluseModulate();
			}
			break;

		case KEY_FUNC_FCTN_PLUSE_PERIOD_US:
			if (isOk)
			{
				sysScpi->setPlusePeriod(handleValue.toDouble() * 1e3);
				inputValue = "";
				sysData.inputLength = MAXAMDEPTH;

				drawInputParamterNoParamter();
				drawPluseModulate();
			}
			break;

		case KEY_FUNC_FCTN_PLUSE_PERIOD_NS:
			if (isOk)
			{
				sysScpi->setPlusePeriod(handleValue.toDouble());
				inputValue = "";
				sysData.inputLength = MAXAMDEPTH;

				drawInputParamterNoParamter();
				drawPluseModulate();
			}
			break;

		case KEY_FUNC_FCTN_PLUSE_WIDTH_S:
			if (isOk)
			{
				sysScpi->setPluseWidth(handleValue.toDouble() * 1e9);
				inputValue = "";
				sysData.inputLength = MAXAMDEPTH;

				drawInputParamterNoParamter();
				drawPluseModulate();
			}
			break;

		case KEY_FUNC_FCTN_PLUSE_WIDTH_MS:
			if (isOk)
			{
				sysScpi->setPluseWidth(handleValue.toDouble() * 1e6);
				inputValue = "";
				sysData.inputLength = MAXAMDEPTH;

				drawInputParamterNoParamter();
				drawPluseModulate();
			}
			break;

		case KEY_FUNC_FCTN_PLUSE_WIDTH_US:
			if (isOk)
			{
				sysScpi->setPluseWidth(handleValue.toDouble() * 1e3);
				inputValue = "";
				sysData.inputLength = MAXAMDEPTH;

				drawInputParamterNoParamter();
				drawPluseModulate();
			}
			break;

		case KEY_FUNC_FCTN_PLUSE_WIDTH_NS:
			if (isOk)
			{
				sysScpi->setPluseWidth(handleValue.toDouble());
				inputValue = "";
				sysData.inputLength = MAXAMDEPTH;

				drawInputParamterNoParamter();
				drawPluseModulate();
			}
			break;

			//LF
		case KEY_FUNC_FCTN_LF_AMPL_VPP:
			if (isOk)
			{
				sysScpi->setLfAmpl(handleValue.toDouble() * 1e3);
				inputValue = "";
				sysData.inputLength = MAXLFINPUTLENGTH;

				drawInputParamterNoParamter();
				drawArbitraryWave();
			}
			break;

		case KEY_FUNC_FCTN_LF_AMPL_MVPP:
			if (isOk)
			{
				sysScpi->setLfAmpl(handleValue.toDouble());

				inputValue = "";
				sysData.inputLength = MAXLFINPUTLENGTH;

				drawInputParamterNoParamter();
				drawArbitraryWave();
			}
			break;

		case KEY_FUNC_FCTN_LF_FREQ_MHZ:
			if (isOk)
			{
				sysScpi->setLfFreq(handleValue.toDouble() * 1e6);

				inputValue = "";

				sysData.inputLength = MAXFREQINPUTLENGTH;

				drawInputParamterNoParamter();
				drawArbitraryWave();
			}
			break;

		case KEY_FUNC_FCTN_LF_FREQ_KHZ:
			if (isOk)
			{
				sysScpi->setLfFreq(handleValue.toDouble() * 1000);

				inputValue = "";

				sysData.inputLength = MAXFREQINPUTLENGTH;
				drawInputParamterNoParamter();
				drawArbitraryWave();
			}
			break;

		case KEY_FUNC_FCTN_LF_FREQ_HZ:
			if (isOk)
			{
				sysScpi->setLfFreq(handleValue.toDouble());

				inputValue = "";
				sysData.inputLength = MAXFREQINPUTLENGTH;

				drawInputParamterNoParamter();
				drawArbitraryWave();
			}
			break;

			//DRG
		case KEY_FUNC_FCTN_DRG_UPLIMIT_GHZ:
			if (isOk)
			{
				sysScpi->setDrgUplimit(handleValue.toDouble() * 1e9);
				inputValue = "";
				sysData.inputLength = MAXDRGFREQINPUTLENGTH;
				drawInputParamterNoParamter();
				drawDrgFunction();
			}
			break;
		case KEY_FUNC_FCTN_DRG_UPLIMIT_MHZ:
			if (isOk)
			{
				sysScpi->setDrgUplimit(handleValue.toDouble() * 1e6);
				inputValue = "";
				sysData.inputLength = MAXDRGFREQINPUTLENGTH;
				drawInputParamterNoParamter();
				drawDrgFunction();
			}
			break;
		case KEY_FUNC_FCTN_DRG_UPLIMIT_KHZ:
			if (isOk)
			{
				sysScpi->setDrgUplimit(handleValue.toDouble() * 1e3);
				inputValue = "";
				sysData.inputLength = MAXDRGFREQINPUTLENGTH;
				drawInputParamterNoParamter();
				drawDrgFunction();
			}
			break;
		case KEY_FUNC_FCTN_DRG_UPLIMIT_HZ:
			if (isOk)
			{
				sysScpi->setDrgUplimit(handleValue.toDouble());
				inputValue = "";
				sysData.inputLength = MAXDRGFREQINPUTLENGTH;
				drawInputParamterNoParamter();
				drawDrgFunction();
			}
			break;
		case KEY_FUNC_FCTN_DRG_DOWNLIMIT_GHZ:
			if (isOk)
			{
				sysScpi->setDrgDownlimit(handleValue.toDouble() * 1e9);
				inputValue = "";
				sysData.inputLength = MAXDRGFREQINPUTLENGTH;
				drawInputParamterNoParamter();
				drawDrgFunction();
			}
			break;
		case KEY_FUNC_FCTN_DRG_DOWNLIMIT_MHZ:
			if (isOk)
			{
				sysScpi->setDrgDownlimit(handleValue.toDouble() * 1e6);
				inputValue = "";
				sysData.inputLength = MAXDRGFREQINPUTLENGTH;
				drawInputParamterNoParamter();
				drawDrgFunction();
			}
			break;
		case KEY_FUNC_FCTN_DRG_DOWNLIMIT_KHZ:
			if (isOk)
			{
				sysScpi->setDrgDownlimit(handleValue.toDouble() * 1e3);
				inputValue = "";
				sysData.inputLength = MAXDRGFREQINPUTLENGTH;
				drawInputParamterNoParamter();
				drawDrgFunction();
			}
			break;
		case KEY_FUNC_FCTN_DRG_DOWNLIMIT_HZ:
			if (isOk)
			{
				sysScpi->setDrgDownlimit(handleValue.toDouble());
				inputValue = "";
				sysData.inputLength = MAXDRGFREQINPUTLENGTH;
				drawInputParamterNoParamter();
				drawDrgFunction();
			}
			break;
		case KEY_FUNC_FCTN_DRG_POINTS_ENTER:
			if (isOk)
			{
				sysScpi->setDrgPoints(handleValue.toDouble());
				inputValue = "";
				sysData.inputLength = 5;
				drawInputParamterNoParamter();
				drawDrgFunction();
			}
			break;
		case KEY_FUNC_FCTN_DRG_MORE_RATE_MS:
			if (isOk)
			{
				sysScpi->setDrgRate(handleValue.toDouble());
				inputValue = "";
				sysData.inputLength = MAXDRGTIMEINPUTLENGTH;
				drawInputParamterNoParamter();
				drawDrgFunction();
			}
			break;
		case KEY_FUNC_FCTN_DRG_MORE_RATE_US:
			if (isOk)
			{
				sysScpi->setDrgRate(handleValue.toDouble() * 1e-3);
				inputValue = "";
				sysData.inputLength = MAXDRGTIMEINPUTLENGTH;
				drawInputParamterNoParamter();
				drawDrgFunction();
			}
			break;
		case KEY_FUNC_FCTN_DRG_MORE_RATE_NS:
			if (isOk)
			{
				sysScpi->setDrgRate(handleValue.toDouble() * 1e-6);
				inputValue = "";
				sysData.inputLength = MAXDRGTIMEINPUTLENGTH;
				drawInputParamterNoParamter();
				drawDrgFunction();
			}
			break;
			//AUX-------------------------------------------------------------------------------------------
		case KEY_FUNC_AUX_LAN_IP_ENTER:
			if (!QHostAddress(handleValue).isNull())
			{
				sysScpi->setLanIP(handleValue);

				inputValue = "";

				sysData.inputLength = 15;

				drawInputParamterNoParamter();
				drawSystemPara();
			} else
			{
				outputMessage(getTheTranslatorString(QString("Address")) + " " + handleValue + " " + getTheTranslatorString(QString("is invalid")));
			}
			break;

		case KEY_FUNC_AUX_LAN_MASK_ENTER:
			if (!QHostAddress(handleValue).isNull())
			{
				sysScpi->setLanMask(handleValue);

				inputValue = "";
				sysData.inputLength = 15;

				drawInputParamterNoParamter();
				drawSystemPara();
			} else
			{
				outputMessage(getTheTranslatorString(QString("address")) + " " + handleValue + " " + getTheTranslatorString(QString("is invalid")));
			}
			break;

		case KEY_FUNC_AUX_LAN_GATEWAY_ENTER:
			if (!QHostAddress(handleValue).isNull())
			{
				sysScpi->setLanGate(handleValue);

				inputValue = "";
				sysData.inputLength = 15;

				drawInputParamterNoParamter();
				drawSystemPara();
			} else
			{
				outputMessage(getTheTranslatorString(QString("address")) + " " + handleValue + " " + getTheTranslatorString(QString("is invalid")));
			}
			break;

		case KEY_FUNC_AUX_SERVICE_LANMACADDR_ENTER:
			if (isOk)
			{
				sysScpi->setSystemServiceOfLanMacAddress(handleValue);
			}
			break;

	}
}

//复位所有逻辑
void mainForm::resetAllLogic(void)
{
	resetMeasSetupLogic();
}

void mainForm::resetMeasSetupLogic(void)
{

}

bool mainForm::isInputPasswordValid(QString value)
{
	if (nowSubKey != NULL && nowSubKey->isPassword)
	{
		//	bool isNumberPassword = false;
		//	int intPassword = 0;
		//	intPassword = inputValue.toInt(&isNumberPassword);

		QString password = inputValue.toUpper();
		QString md5String = "";
		char temp[64] = { };

		if (strcmp(sysScpi->moduSerial.toStdString().c_str(), SERIALNO) == 0) //没有底层模块时，密码为默认密码203093
		{
			md5String = QString(longLongIntToString(PASSWORD, temp)).trimmed();
		} else
		{
			md5String = sysScpi->getSystemServiceOfMD5(sysScpi->moduSerial, true); //有底层模块时，根据底层模块计算得到密码
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
	helpContent = getTheTranslatorString("reseting system......");

	if (factory)
	{
		sysScpi->presetSystemData();
	} else
	{
		sysScpi->presetSystem();
	}

	nowMainKey = sysKey;

	if (nowMainKey != NULL)
	{
		nowSubKey = nowMainKey->rightNode;
	}

	helpContent = getTheTranslatorString("Ready");
	drawFreqAndAmplMain();
	drawState();
	drawInputParamterNoParamter();
	//  drawFreqAndAmpl();
	drawFreqInParamter();
	//  drawAmplInParamter();
	drawButton();

}
//void mainForm::represetSystem(bool factory)
//{
//	presetSystem(factory);
//}
