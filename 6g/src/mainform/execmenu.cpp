#include "mainform.h"
#include "stdlib.h"

//上方向键
bool mainForm::isArrowUp(int keyValue)
{
	return keyValue == KEY_CTL_ARROW_UP;
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
		//case KEY_FUNC_SWEEP_REPEAT_SINGLE:
		//case KEY_FUNC_SWEEP_REPEAT_CONT:
		case KEY_FUNC_SWEEP_MORE_SWEEPTRIGGER_FREERUN:
		case KEY_FUNC_SWEEP_MORE_SWEEPTRIGGER_EXT:
		case KEY_FUNC_SWEEP_MORE_SWEEPDIRECTION_DOWN:
		case KEY_FUNC_SWEEP_MORE_SWEEPDIRECTION_UP:
		case KEY_FUNC_SWEEP_MORE_SWEEPRETRACE_OFF:
		case KEY_FUNC_SWEEP_MORE_SWEEPRETRACE_ON:
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_MORE_PRESETLIST_YES:
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_MORE_PRESETLIST_NO:
			//模拟调制
		//case KEY_FUNC_FCTN_ANALOG_SOURCE_INTERNAL:
		//case KEY_FUNC_FCTN_ANALOG_SOURCE_EXTERNAL:
		//case KEY_FUNC_FCTN_ANALOG_FM_STATE_ON:
		//case KEY_FUNC_FCTN_ANALOG_FM_STATE_OFF:
		//case KEY_FUNC_FCTN_ANALOG_PM_STATE_ON:
		//case KEY_FUNC_FCTN_ANALOG_PM_STATE_OFF:
			//数字调制
		case KEY_FUNC_AUXFCTN_DIGITALMOD_MODE_ASK:
		case KEY_FUNC_AUXFCTN_DIGITALMOD_MODE_FSK_2FSK:
		case KEY_FUNC_AUXFCTN_DIGITALMOD_MODE_FSK_4FSK:
		case KEY_FUNC_AUXFCTN_DIGITALMOD_MODE_FSK_8FSK:
		case KEY_FUNC_AUXFCTN_DIGITALMOD_MODE_PSK_2PSK:
		case KEY_FUNC_AUXFCTN_DIGITALMOD_MODE_PSK_4PSK:
		case KEY_FUNC_AUXFCTN_DIGITALMOD_MODE_FSK_8PSK:
			//脉冲调制
		case KEY_FUNC_PULSE_TRIGGERWAY_FREERUN:
		case KEY_FUNC_PULSE_TRIGGERWAY_KEYTRIG:
		case KEY_FUNC_PULSE_TRIGGERWAY_EXTTRIG:
			//低频输出
		case KEY_FUNC_AUXFCTN_LFOUT_WAVE_SINE:
		case KEY_FUNC_AUXFCTN_LFOUT_WAVE_SQUARE:
		case KEY_FUNC_AUXFCTN_LFOUT_WAVE_TRIANGLE:
		case KEY_FUNC_AUXFCTN_LFOUT_WAVE_SAWTOOTH:
			//DRG
		case KEY_FUNC_AUXFCTN_DRG_MODE_BOTHWAY:
		case KEY_FUNC_AUXFCTN_DRG_MODE_POSSINGLE:
		case KEY_FUNC_AUXFCTN_DRG_MODE_NEGSINGLE:
			//中英文切换
		case KEY_FUNC_SYSTEM_LANGUAGE_CHINESE:
		case KEY_FUNC_SYSTEM_LANGUAGE_ENGLISH:
			//试用
		case KEY_FUNC_SYSTEM_SERVICE_EVALUATE_ON:
		case KEY_FUNC_SYSTEM_SERVICE_EVALUATE_OFF:
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

//获取有多少个More键，即第六键
int mainForm::HaveMoreKeyCount(void)
{
	int moreCount = 0;
	int index = 0;
	subKeyDef *tempSubKey = nowSubKey;
	subKeyDef *midSubKey = NULL;
	subKeyDef *indexSubKey = NULL;
	//将指针指向本级菜单的最顶端
	while (tempSubKey != NULL)
	{
		if (tempSubKey->upNode == NULL)
		{
			if (tempSubKey->leftNode != NULL && tempSubKey->leftNode->keyValue == KEY_SUB_MENU6)
			{
				tempSubKey = tempSubKey->leftNode;
			}
			else
			{
				break;
			}
		}
		tempSubKey = tempSubKey->upNode;
	}
	indexSubKey = tempSubKey;
	//获取有多少个More键
	while (true)
	{
		midSubKey = isHaveMoreKey(tempSubKey);
		if (midSubKey != NULL)
		{
			tempSubKey = midSubKey;
			moreCount++;
		}
		else
		{
			break;
		}
	}
	//获取nowSubKey在moreKeyCount中的第几个
	while (true)
	{
		if (indexSubKey->funcCode ==nowSubKey->funcCode)
		{
			break;
		}
		indexSubKey = indexSubKey->downNode;
		if (indexSubKey == NULL)
		{
			break;
		}
		if (indexSubKey != NULL && indexSubKey->keyValue == KEY_SUB_MENU6 && indexSubKey->rightNode != NULL)
		{
			indexSubKey = indexSubKey->rightNode;
			index++;
		}
	}
	sysData.moreKeyCount.Count = moreCount;
	sysData.moreKeyCount.index = index;
	return moreCount;
}

//判断是否有More键，即第六键，若有则返回More键的第一个子键，若没有则返回NULL
subKeyDef* mainForm::isHaveMoreKey(subKeyDef *subKey)
{
	subKeyDef *tempSubKey = subKey;
	while (tempSubKey != NULL)
	{
		if (tempSubKey->keyValue == KEY_SUB_MENU6)
		{
			return tempSubKey->rightNode;
		}
		if (tempSubKey->downNode == NULL)
		{
			break;
		}
		tempSubKey = tempSubKey->downNode;
	}
	return NULL;
}

//执行物理More键
void mainForm::handleHardMoreKey(void)
{
	if (sysData.moreKeyCount.Count > 0)
	{
		if (sysData.moreKeyCount.index + 1 > sysData.moreKeyCount.Count)
		{
			sysData.moreKeyCount.index = 0;		//更新索引
			//返回本级菜单的最顶端
			subKeyDef *tempSubKey = nowSubKey;
			while (tempSubKey != NULL)
			{
				if (tempSubKey->upNode == NULL)
				{
					if (tempSubKey->leftNode != NULL && tempSubKey->leftNode->keyValue == KEY_SUB_MENU6)
					{
						tempSubKey = tempSubKey->leftNode;
					}
					else
					{
						break;
					}
				}
				tempSubKey = tempSubKey->upNode;
			}
			nowSubKey = tempSubKey;
		}
		else
		{
			sysData.moreKeyCount.index++;	//更新索引
			//进入下一页菜单
			subKeyDef *tempSubKey = NULL;
			tempSubKey = isHaveMoreKey(nowSubKey);
			if (tempSubKey != NULL)
			{
				nowSubKey = tempSubKey;
			}
		}
	}
	subKeyDef *tempSubKey = nowSubKey;
	while (tempSubKey != NULL)
	{
		if (tempSubKey->keyValue == KEY_SUB_MENU6)
		{
			if (tempSubKey->rightNode != NULL)
			{
				tempSubKey = tempSubKey->rightNode;
			}
		}
		if (tempSubKey->downNode == NULL)
		{
			break;
		}
		tempSubKey = tempSubKey->downNode;
	}
}

//执行物理Return键
void mainForm::handleHardReturnKey()
{
	//将指针指向本级菜单的最顶端
	subKeyDef *tempSubKey = nowSubKey;
	if (tempSubKey->isUnit)	//如果是单位键则直接返回
	{
		return;
	}
	while (tempSubKey != NULL)
	{
		if (tempSubKey->upNode == NULL)
		{
			if (tempSubKey->leftNode != NULL && tempSubKey->leftNode->keyValue == KEY_SUB_MENU6)
			{
				tempSubKey = tempSubKey->leftNode;
			}
			else
			{
				break;
			}
		}
		tempSubKey = tempSubKey->upNode;
	}
	//返回父菜单
	if (tempSubKey->leftNode != NULL)
	{
		nowSubKey = tempSubKey->leftNode;
		//更新More键属性
		HaveMoreKeyCount();
	}
}

//按下物理按键后是否需要绘制界面，如扫描的列表设置是需要重新绘制的
void mainForm::afterReturnDraw()
{
	int funcCode = nowSubKey->funcCode;
	switch (funcCode)
	{
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST:
			drawSweep(sysData.sweep.type);
			strcpy(sysData.name, "");
			strcpy(sysData.value, "");
			strcpy(sysData.unit, "");
			drawInputParamter();
			break;
		default:
			break;
	}
}

//处理左右方向键
void mainForm::handleArrowLeftRightKey(int funcCode, bool left)
{
	switch (funcCode)
	{
		case KEY_FUNC_FREQ:
		case KEY_FUNC_FREQREFSET:
		case KEY_FUNC_FREQOFFSET:
		case KEY_FUNC_FREQMULTIPLIER:

		case KEY_FUNC_AMPL:
		case KEY_FUNC_AMPLREFSET:
		case KEY_FUNC_AMPLOFFSET:
		case KEY_FUNC_AMPL_CABLE:

		case KEY_FUNC_SWEEP_STEP_MORE_STEPDWELL:
		case KEY_FUNC_SWEEP_STEP_FREQSTART:
		case KEY_FUNC_SWEEP_STEP_FREQSTOP:
		case KEY_FUNC_SWEEP_STEP_AMPLSTART:
		case KEY_FUNC_SWEEP_STEP_AMPLSTOP:
		case KEY_FUNC_SWEEP_STEP_MORE_POINTS:

		case KEY_FUNC_AM_DEPTH:
		case KEY_FUNC_FMPM_OFFSET_MHZ:
		case KEY_FUNC_FMPM_OFFSET_KHZ:
		case KEY_FUNC_FMPM_OFFSET_HZ:
		case KEY_FUNC_AM_INTERNALFCTN_FREQ:
		case KEY_FUNC_FMPM_INTERNALFCTN_FREQ_MHZ:
		case KEY_FUNC_FMPM_INTERNALFCTN_FREQ_KHZ:
		case KEY_FUNC_FMPM_INTERNALFCTN_FREQ_HZ:

		case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA_DEPTH:
		case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA_DEVI:
		case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA_PHASE:
		case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA_RATE:
		
		case KEY_FUNC_PULSE_PERIOD:
		case KEY_FUNC_PULSE_WIDTH:
		case KEY_FUNC_PULSE_DELAY:
		case KEY_FUNC_AUXFCTN_LFOUT_FREQ:
		case KEY_FUNC_AUXFCTN_LFOUT_AMPL:
		case KEY_FUNC_AUXFCTN_DRG_UPPER:
		case KEY_FUNC_AUXFCTN_DRG_LOW:
		case KEY_FUNC_AUXFCTN_DRG_RATE:
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_EDITITEM:
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_GOTOROW:
		case KEY_FUNC_SYSTEM_SERVICE_LANMACADDR:
        case KEY_FUNC_FMPM_OFFSET:
        case KEY_FUNC_FMPM_INTERNALFCTN_FREQ:
			sysScpi->dirKeyToCursor(left);
			drawInputParamter();
			break;
		default:
			break;
	}
}

//处理上下方向键
void mainForm::handleArrowUpDownKey(int funCode, bool up)
{
	if (up)	//处理上方向键
	{
		switch (funCode)
		{
			case KEY_FUNC_FREQ:		//根据频率增量设置频率
				if (sysData.cursorLocation == -1)	//按照频率增量来增减频率
				{
					sysScpi->setFrequency(sysData.freq.freq + sysData.incrSet.FreqIncrValue);
				}
				else
				{
					//按照当前鼠标的位置设置频率
					sysScpi->setFrequencyWithRpg(1);
				}
				drawMainPara();
				drawInputParamter();
				drawFreqInParamter();
				break;
			case KEY_FUNC_AMPL:		
                if (sysData.cursorLocation == -1)//根据幅度增量设置幅度
                    sysScpi->setAmplitude(sysData.ampl.ampl + sysData.incrSet.AmptdIncrValue);
                else    //根据当前鼠标位置设置幅度
                    sysScpi->setAmplitudeWithRpg(1);
				drawMainPara();
				drawInputParamter();
				drawAmplInParamter();
				break;
            case KEY_FUNC_AMPL_CABLE://缆损补偿
                if (sysData.cursorLocation != -1)
                    sysScpi->setAmplitudeErrorWithRpg(1);
                drawMainPara();
                drawInputParamter();
                drawAmplInParamter();
                break;
            case KEY_FUNC_SWEEP_STEP_FREQSTART://扫描——起始频率
                if (sysData.cursorLocation != -1)
                    sysScpi->setStepStartFrequencyWithRpg(1);
                drawMainPara();
                drawInputParamter();
                drawSweep(sysData.sweep.type);
                break;
            case KEY_FUNC_SWEEP_STEP_FREQSTOP://扫描——终止频率
                if (sysData.cursorLocation != -1)
                    sysScpi->setStepStopFrequencyWithRpg(1);
                drawMainPara();
                drawInputParamter();
                drawSweep(sysData.sweep.type);
                break;
            case KEY_FUNC_SWEEP_STEP_AMPLSTART://扫描——起始幅度
                if (sysData.cursorLocation != -1)
                    sysScpi->setStepStartAmplitudeWithRpg(1);
                drawMainPara();
                drawInputParamter();
                drawSweep(sysData.sweep.type);
                break;
            case KEY_FUNC_SWEEP_STEP_AMPLSTOP://扫描——终止幅度
                if (sysData.cursorLocation != -1)
                    sysScpi->setStepStopAmplitudeWithRpg(1);
                drawMainPara();
                drawInputParamter();
                drawSweep(sysData.sweep.type);
                break;
            case KEY_FUNC_SWEEP_STEP_MORE_STEPDWELL://扫描——驻留时间
                if (sysData.cursorLocation != -1)
                    sysScpi->setStepDwellWithRpg(1);
                drawMainPara();
                drawInputParamter();
                drawSweep(sysData.sweep.type);
                break;
			case KEY_FUNC_SWEEP_MORE_CONFIGLIST_GOTOROW:
			//case KEY_FUNC_SWEEP_MORE_CONFIGLIST_EDITITEM:
			//case KEY_FUNC_SWEEP_MORE_CONFIGLIST_INSERTROW:
			//case KEY_FUNC_SWEEP_MORE_CONFIGLIST_DELETEROW:
				sysData.cursorLocation = 0;
				sysData.sweep.list.allIndex -= 3;
				if (sysData.sweep.list.allIndex < 0)
					sysData.sweep.list.allIndex = 0;
				if (sysData.sweep.list.allIndex >(sysData.sweep.list.allRow * 3 - 1))
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
				drawInputParamter();
				drawsListData();
				break;
            case KEY_FUNC_AM_DEPTH://AM调制深度
                if (sysData.cursorLocation != -1)
                    sysScpi->setAmDepthWithRpg(1);
                drawInputParamter();
                drawAM();
                break;
            case KEY_FUNC_AM_INTERNALFCTN_FREQ://AM调制速率
                if (sysData.cursorLocation != -1)
                {
                    sysScpi->setAmRateWithRpg(1);
                    drawInputParamter();
                    drawAM();
                }
                break;
            case KEY_FUNC_FMPM_OFFSET://FM PM 调制偏移
                if (sysData.cursorLocation != -1)
                {
                    if (sysData.fmpm.mode == fdtFM)
                    {
                        sysScpi->setFmDevWithRpg(1);
                        drawInputParamter();
                    }
                    else if (sysData.fmpm.mode == fdtPM)
                    {
                        sysScpi->setPmPhaseWithRpg(1);
                        drawInputParamter();
                    }
                }
                break;
            case KEY_FUNC_FMPM_INTERNALFCTN_FREQ://FM PM 调制速率
                if (sysData.cursorLocation != -1)
                {
                    if (sysData.fmpm.mode == fdtFM)
                    {
                        sysScpi->setFmRateWithRpg(1);
                        drawInputParamter();
                    }
                    else if (sysData.fmpm.mode == fdtPM)
                    {
                        sysScpi->setPmRateWithRpg(1);
                        drawInputParamter();
                    }
                }
                break;
            case KEY_FUNC_PULSE_PERIOD://脉冲调制——脉冲周期
                if (sysData.cursorLocation != -1)
                {
                    if (sysData.pulse.pulsesource == psInternal)
                    {
                        sysScpi->setPulsePeriodWithRpg(1);
                        sprintf(sysData.value, doubleToQValue(sysData.pulse.period).toStdString().c_str());
                        insertThousands(sysData.value); //数组类型插入千分符
                        sprintf(sysData.unit, timeToQUnit(sysData.pulse.period * 1e-6).toStdString().c_str());
                        drawInputParamter();
                        drawPulseModulate();
                    }
                }
                break;
            case KEY_FUNC_PULSE_WIDTH://脉冲调制——脉冲宽度
                if (sysData.cursorLocation != -1)
                {
                    if (sysData.pulse.pulsesource == psInternal)
                    {
                        sysScpi->setPulseWidthWithRpg(1);
                        sprintf(sysData.value, doubleToQValue(sysData.pulse.width).toStdString().c_str());
                        insertThousands(sysData.value); //数组类型插入千分符
                        sprintf(sysData.unit, timeToQUnit(sysData.pulse.width * 1e-6).toStdString().c_str());
                        drawInputParamter();
                        drawPulseModulate();
                    }
                }
                break;
            case KEY_FUNC_AUXFCTN_LFOUT_FREQ://低频输出，输出频率
                if(sysData.cursorLocation  != -1)
                {
                    sysScpi->setLfFreqWithRpg(1);
                    drawInputParamter();
                    drawArbitraryWave();
                }
                break;
            case KEY_FUNC_AUXFCTN_LFOUT_AMPL://低频输出，输出功率
                if (sysData.cursorLocation != -1)
                {
                    sysScpi->setLfAmplWithRpg(1);
                    drawInputParamter();
                    drawArbitraryWave();
                }
                break;
            case KEY_FUNC_AUXFCTN_DRG_UPPER://线性调频，扫描上限
                if (sysData.cursorLocation != -1)
                {
                    sysScpi->setDrgUplimitWithRpg(1);
                    drawInputParamter();
                    drawDrgFunction();
                }
                break;
            case KEY_FUNC_AUXFCTN_DRG_LOW://线性调频，扫描下限
                if (sysData.cursorLocation != -1)
                {
                    sysScpi->setDrgDownlimitWithRpg(1);
                    drawInputParamter();
                    drawDrgFunction();
                }
                break;
            case KEY_FUNC_AUXFCTN_DRG_POINTS://线性调频，扫描点数
                if (sysData.cursorLocation != -1)
                {
                    sysScpi->setDrgPointsWithRpg(1);
                    drawInputParamter();
                    drawDrgFunction();
                }
                break;
            case KEY_FUNC_AUXFCTN_DRG_RATE://线性调频，扫描速率
                if (sysData.cursorLocation != -1)
                {
                    sysScpi->setDrgRateWithRpg(1);
                    drawInputParamter();
                    drawDrgFunction();
                }
                break;
            case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA_DEPTH://数字调制,调制深度
                if (sysData.cursorLocation != -1)
                {
                    sysScpi->setDigitalModulateParaWithRpg(1);
                    drawInputParamter();
                    drawDigitalModulate();
                }
                break;
            case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA_DEVI://数字调制，调制频偏
                if (sysData.cursorLocation != -1)
                {
                    sysScpi->setDigitalModulateParaWithRpg(1);
                    drawInputParamter();
                    drawDigitalModulate();
                }
                break;
            case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA_PHASE://数字调制，调制相位
                if (sysData.cursorLocation != -1)
                {
                    sysScpi->setDigitalModulateParaWithRpg(1);
                    drawInputParamter();
                    drawDigitalModulate();
                }
                break;
            case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA_RATE://数字调制，调制速率
                if (sysData.cursorLocation != -1)
                {
                    sysScpi->setDigitalModuateRateWithRpg(1);
                    drawInputParamter();
                    drawDigitalModulate();
                }
                break;
			default:
				break;
		}
	}
	else	//处理下方向键
	{
		switch (funCode)
		{
		case KEY_FUNC_FREQ:		//根据频率增量设置频率
			if (sysData.cursorLocation == -1)
			{
				//按照频率增量来增减频率
				sysScpi->setFrequency(sysData.freq.freq - sysData.incrSet.FreqIncrValue);
			}
			else
			{
				//按照当前鼠标的位置设置频率
				sysScpi->setFrequencyWithRpg(-1);
			}
			drawMainPara();
			drawInputParamter();
			drawFreqInParamter();
			break;
		case KEY_FUNC_AMPL:		
            if (sysData.cursorLocation == -1)   //根据幅度增量设置幅度
                sysScpi->setAmplitude(sysData.ampl.ampl - sysData.incrSet.AmptdIncrValue);
            else    //根据当前鼠标位置设置幅度
                sysScpi->setAmplitudeWithRpg(-1);
			drawMainPara();
			drawInputParamter();
			drawAmplInParamter();
			break;
        case KEY_FUNC_AMPL_CABLE://缆损补偿
            if (sysData.cursorLocation != -1)
                sysScpi->setAmplitudeErrorWithRpg(-1);
            drawMainPara();
            drawInputParamter();
            drawAmplInParamter();
            break;
        case KEY_FUNC_SWEEP_STEP_FREQSTART://扫描——起始频率
            if (sysData.cursorLocation != -1)
                sysScpi->setStepStartFrequencyWithRpg(-1);
            drawMainPara();
            drawInputParamter();
            drawSweep(sysData.sweep.type);
            break;
        case KEY_FUNC_SWEEP_STEP_FREQSTOP://扫描——终止频率
            if (sysData.cursorLocation != -1)
                sysScpi->setStepStopFrequencyWithRpg(-1);
            drawMainPara();
            drawInputParamter();
            drawSweep(sysData.sweep.type);
            break;
        case KEY_FUNC_SWEEP_STEP_AMPLSTART://扫描——起始幅度
            if (sysData.cursorLocation != -1)
                sysScpi->setStepStartAmplitudeWithRpg(-1);
            drawMainPara();
            drawInputParamter();
            drawSweep(sysData.sweep.type);
            break;
        case KEY_FUNC_SWEEP_STEP_AMPLSTOP://扫描——终止幅度
            if (sysData.cursorLocation != -1)
                sysScpi->setStepStopAmplitudeWithRpg(-1);
            drawMainPara();
            drawInputParamter();
            drawSweep(sysData.sweep.type);
            break;
        case KEY_FUNC_SWEEP_STEP_MORE_STEPDWELL://扫描——驻留时间
            if (sysData.cursorLocation != -1)
                sysScpi->setStepDwellWithRpg(-1);
            drawMainPara();
            drawInputParamter();
            drawSweep(sysData.sweep.type);
            break;
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_GOTOROW:
		//case KEY_FUNC_SWEEP_MORE_CONFIGLIST_EDITITEM:
		//case KEY_FUNC_SWEEP_MORE_CONFIGLIST_INSERTROW:
		//case KEY_FUNC_SWEEP_MORE_CONFIGLIST_DELETEROW:
			sysData.cursorLocation = 0;
			sysData.sweep.list.allIndex += 3;
			if (sysData.sweep.list.allIndex < 0)
				sysData.sweep.list.allIndex = 0;
			if (sysData.sweep.list.allIndex >(sysData.sweep.list.allRow * 3 - 1))
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
			drawInputParamter();
			drawsListData();
			break;
        case KEY_FUNC_AM_DEPTH://AM调制深度
            if (sysData.cursorLocation != -1)
                sysScpi->setAmDepthWithRpg(-1);
            drawInputParamter();
            drawAM();
            break;
        case KEY_FUNC_AM_INTERNALFCTN_FREQ://AM调制速率
            if (sysData.cursorLocation != -1)
            {
                sysScpi->setAmRateWithRpg(-1);
                drawInputParamter();
                drawAM();
            }
            break;
        case KEY_FUNC_FMPM_OFFSET://FM PM 调制偏移
            if (sysData.cursorLocation != -1)
            {
                if (sysData.fmpm.mode == fdtFM)
                {
                    sysScpi->setFmDevWithRpg(-1);
                    drawInputParamter();
                }
                else if (sysData.fmpm.mode == fdtPM)
                {
                    sysScpi->setPmPhaseWithRpg(-1);
                    drawInputParamter();
                }
            }
            break;
        case KEY_FUNC_FMPM_INTERNALFCTN_FREQ://FM PM 调制速率
            if (sysData.cursorLocation != -1)
            {
                if (sysData.fmpm.mode == fdtFM)
                {
                    sysScpi->setFmRateWithRpg(-1);
                    drawInputParamter();
                }
                else if (sysData.fmpm.mode == fdtPM)
                {
                    sysScpi->setPmRateWithRpg(-1);
                    drawInputParamter();
                }
            }
            break;
        case KEY_FUNC_PULSE_PERIOD://脉冲调制——脉冲周期
            if (sysData.cursorLocation != -1)
            {
                if (sysData.pulse.pulsesource == psInternal)
                {
                    sysScpi->setPulsePeriodWithRpg(-1);
                    sprintf(sysData.value, doubleToQValue(sysData.pulse.period).toStdString().c_str());
                    insertThousands(sysData.value); //数组类型插入千分符
                    sprintf(sysData.unit, timeToQUnit(sysData.pulse.period * 1e-6).toStdString().c_str());
                    drawInputParamter();
                    drawPulseModulate();
                }
            }
            break;
        case KEY_FUNC_PULSE_WIDTH://脉冲调制——脉冲宽度
            if (sysData.cursorLocation != -1)
            {
                if (sysData.pulse.pulsesource == psInternal)
                {
                    sysScpi->setPulseWidthWithRpg(-1);
                    sprintf(sysData.value, doubleToQValue(sysData.pulse.width).toStdString().c_str());
                    insertThousands(sysData.value); //数组类型插入千分符
                    sprintf(sysData.unit, timeToQUnit(sysData.pulse.width * 1e-6).toStdString().c_str());
                    drawInputParamter();
                    drawPulseModulate();
                }
            }
            break;
        case KEY_FUNC_AUXFCTN_LFOUT_FREQ://低频输出，输出频率
            if (sysData.cursorLocation != -1)
            {
                sysScpi->setLfFreqWithRpg(-1);
                drawInputParamter();
                drawArbitraryWave();
            }
            break;
        case KEY_FUNC_AUXFCTN_LFOUT_AMPL://低频输出，输出功率
            if (sysData.cursorLocation != -1)
            {
                sysScpi->setLfAmplWithRpg(-1);
                drawInputParamter();
                drawArbitraryWave();
            }
            break;
        case KEY_FUNC_AUXFCTN_DRG_UPPER://线性调频，扫描上限
            if (sysData.cursorLocation != -1)
            {
                sysScpi->setDrgUplimitWithRpg(-1);
                drawInputParamter();
                drawDrgFunction();
            }
            break;
        case KEY_FUNC_AUXFCTN_DRG_LOW://线性调频，扫描下限
            if (sysData.cursorLocation != -1)
            {
                sysScpi->setDrgDownlimitWithRpg(-1);
                drawInputParamter();
                drawDrgFunction();
            }
            break;
        case KEY_FUNC_AUXFCTN_DRG_POINTS://线性调频，扫描点数
            if (sysData.cursorLocation != -1)
            {
                sysScpi->setDrgPointsWithRpg(-1);
                drawInputParamter();
                drawDrgFunction();
            }
            break;
        case KEY_FUNC_AUXFCTN_DRG_RATE://线性调频，扫描速率
            if (sysData.cursorLocation != -1)
            {
                if (sysData.cursorLocation == 1)//扫描速率必须是20的整数倍，所以需要这么处理
                {
                    sysScpi->setDrgRateWithRpg(-2);
                }
                else
                {
                    sysScpi->setDrgRateWithRpg(-1);
                }
                drawInputParamter();
                drawDrgFunction();
            }
            break;
        case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA_DEPTH://数字调制,调制深度
            if (sysData.cursorLocation != -1)
            {
                sysScpi->setDigitalModulateParaWithRpg(-1);
                drawInputParamter();
                drawDigitalModulate();
            }
            break;
        case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA_DEVI://数字调制，调制频偏
            if (sysData.cursorLocation != -1)
            {
                sysScpi->setDigitalModulateParaWithRpg(-1);
                drawInputParamter();
                drawDigitalModulate();
            }
            break;
        case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA_PHASE://数字调制，调制相位
            if (sysData.cursorLocation != -1)
            {
                sysScpi->setDigitalModulateParaWithRpg(-1);
                drawInputParamter();
                drawDigitalModulate();
            }
            break;
        case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA_RATE://数字调制，调制速率
            if (sysData.cursorLocation != -1)
            {
                sysScpi->setDigitalModuateRateWithRpg(-1);
                drawInputParamter();
                drawDigitalModulate();
            }
            break;
		default:
			break;
		}
	}
}

//处理方向键
void mainForm::handleArrowKey(int funcCode, int keyValue)
{
	if (keyValue == KEY_CTL_ARROW_UP)
	{
		handleArrowUpDownKey(funcCode, true);
	}
	else if (keyValue == KEY_CTL_ARROW_DOWN)
	{
		handleArrowUpDownKey(funcCode, false);
	}
	else if (keyValue == KEY_CTL_ARROW_LEFT)
	{
		handleArrowLeftRightKey(funcCode, true);
	}
	else if(keyValue == KEY_CTL_ARROW_RIGHT)
	{
		handleArrowLeftRightKey(funcCode, false);
	}
	else
	{
		;
	}
}

//处理Select键
void mainForm::handleSelectKey(int funcCode)
{
	int tempCode = funcCode;
	switch (tempCode)
	{
		case KEY_FUNC_SYSTEM_SERVICE:
			//密码处理
			if (nowSubKey->isPassword)
			{
				//恢复初始化
				indexOfPassword = 0;
				previndexOfPassword = 0;
				memset(passwdString, 0, sizeof passwdString);
				if (isInputPasswordValid(inputValue))
				{
					inputValue = "";
					sysData.system.isServiceUnlock = true;
					drawButton();
					strcpy(sysData.value, "");
					strcpy(sysData.name, "");
					drawInputParamter();
					return;
				}
				else
				{
					inputValue = "";
					sysData.system.isServiceUnlock = false;
					strcpy(sysData.value, "password error");
					drawInputParamter();
					sleep(1);
					return;
				}
			}
			break;
		default:
			break;
	}
}

//处理IncrSet键
void mainForm::handleIncrsetKey(int funcCode)
{
	//IncrSet键只有在频率设置和幅度设置下才有效
	if (!((funcCode == KEY_FUNC_FREQ) || (funcCode == KEY_FUNC_AMPL) || (funcCode == KEY_FUNC_INCRSET_SET)))
	{
		return;
	}
	else
	{
		sysData.cursorLocation = -1;	//目的是在没有按左、右键时，通过上下键设置频率和幅度时是根据增量来设置的
	}
	QString name = "";
	QString value = "";
	QString unit = "";
	char tempValue[64] = { 0 };
	char temp[64];
	//InccSet键再XML文件中作为主键，主键下有一个设置键下有四个单位键
	//有一个结构体，有频率增量、幅度增量、当前增量类型（频率|幅度），单位键根据当前增量类型选择是 频率单位还是幅度单位
	switch (funcCode)
	{
		case KEY_FUNC_FREQ:
			sysData.incrSet.type = (incrsetTypeDef)(itdFreq);
			//主键切换到增量主键，切换子键
			sysMenu->getMainKeyPointer(KEY_MAIN_INCRSET);
			nowSubKey = nowMainKey->rightNode;
			//绘制界面
			handleInputKey(KEY_FUNC_INCRSET_SET);
			break;
		case KEY_FUNC_AMPL:
			sysData.incrSet.type = (incrsetTypeDef)(itdAmptd);
			//主键切换到增量主键，切换子键
			sysMenu->getMainKeyPointer(KEY_MAIN_INCRSET);
			nowSubKey = nowMainKey->rightNode;
			//绘制界面
			handleInputKey(KEY_FUNC_INCRSET_SET);
			break;
		case KEY_FUNC_INCRSET_SET:
			//根据当前增量类型，将主键切换回频率主键或是幅度主键，并更新子键到主键下第一个子键
			if (sysData.incrSet.type == itdFreq)
			{
				//主键切换到频率键
				sysMenu->getMainKeyPointer(KEY_MAIN_FREQ);
				nowSubKey = nowMainKey->rightNode;
				//绘制界面
				handleInputKey(KEY_FUNC_FREQ);
			}
			else if (sysData.incrSet.type == itdAmptd)
			{
				//主键切换到幅度键
				sysMenu->getMainKeyPointer(KEY_MAIN_AMPTD);
				nowSubKey = nowMainKey->rightNode;
				//绘制界面
				handleInputKey(KEY_FUNC_AMPL);
			}
			break;
		default:
			break;
	}
}

//处理操作面板上的Trigger键
void mainForm::handleTriggerKey(int keyValue)
{
	//根据当前子键来确定触发什么东西
	switch (keyValue)
	{
		case KEY_FUNC_SWEEP_MODE:		//面板上的Trigger键
		case KEY_FUNC_SWEEP_TYPE:
		case KEY_FUNC_SWEEP_REPEAT:
		case KEY_FUNC_SWEEP_SINGLETRIGGER:
		case KEY_FUNC_SWEEP_CONFIGSTEP:
		case KEY_FUNC_SWEEP_STEP_FREQSTART:
		case KEY_FUNC_SWEEP_STEP_FREQSTOP:
		case KEY_FUNC_SWEEP_STEP_AMPLSTART:
		case KEY_FUNC_SWEEP_STEP_AMPLSTOP:
		case KEY_FUNC_SWEEP_STEP_MORE_POINTS:
		case KEY_FUNC_SWEEP_STEP_MORE_STEPDWELL:
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST:
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_EDITITEM:
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_INSERTROW:
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_DELETEROW:
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_GOTOROW:
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_MORE_DOWNLOAD:
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_MORE_LOADSTORE:
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_MORE_PRESETLIST:
		case KEY_FUNC_SWEEP_MORE_SWEEPTRIGGER:
		case KEY_FUNC_SWEEP_MORE_SWEEPTRIGGER_FREERUN:
		case KEY_FUNC_SWEEP_MORE_SWEEPTRIGGER_TRIGGERKEY:
		case KEY_FUNC_SWEEP_MORE_SWEEPTRIGGER_EXT:
		case KEY_FUNC_SWEEP_MORE_SWEEPDIRECTION:
		case KEY_FUNC_SWEEP_MORE_SWEEPRETRACE:
			if (sysData.sweep.mode != smOff)
			{
				if (sysData.sweep.repeat == srSingle)
				{
					//下发开始扫描指令
					sysScpi->sendSerialData(":KEY:TRIG 1");
				}
			}
			break;
		case KEY_FUNC_PULSE_STATE:
		case KEY_FUNC_PULSE_TRIGGERWAY:
		case KEY_FUNC_PULSE_TRIGGERWAY_FREERUN:
		case KEY_FUNC_PULSE_TRIGGERWAY_KEYTRIG:
		case KEY_FUNC_PULSE_TRIGGERWAY_EXTTRIG:
		case KEY_FUNC_PULSE_SOURCE:
		case KEY_FUNC_PULSE_PERIOD:
		case KEY_FUNC_PULSE_WIDTH:
		case KEY_FUNC_PULSE_DELAY:
		case KEY_FUNC_PULSE_EXTPOLA:
			if (sysData.pulse.state)
			{
				if (sysData.pulse.trigmethod == plmKeytrig)
				{
					sysScpi->sendSerialData(":KEY:TRIG 1");
				}
			}
			break;
		default:
			break;
	}
}

//处理面板上的PageUp键
void mainForm::handlePageUpKey(int keyValue)
{
	switch (keyValue)
	{
		case KEY_MAIN_SWEEP:
			if (sysData.sweep.type == stStep)
			{
				if (sysData.sweep.nowpagenum == 2)
					sysData.sweep.nowpagenum = 1;
				drawSweep(sysData.sweep.type);
			}
			break;
		default:
			break;
	}
}

//处理面板上的PageDown键
void mainForm::handlePageDownKey(int keyValue)
{
	switch (keyValue)
	{
	case KEY_MAIN_SWEEP:
		if (sysData.sweep.type == stStep)
		{
			if (sysData.sweep.nowpagenum == 1)
				sysData.sweep.nowpagenum = 2;
			drawSweep(sysData.sweep.type);
		}
		break;
	default:
		break;
	}
}

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
		drawInputParamter();
		return;
	}
	double value;

	switch (funcCode)
	{
		case KEY_FUNC_AUXFCTN_EXTRF_EXTIF:		//上变频
			sysScpi->setExtRefFrequencyWithRpg(nums);
			drawInputParamter();
			drawExfFref();
			break;
		case KEY_FUNC_FREQ:
            if (sysData.cursorLocation != -1)//不显示光标时不RPG键无用
            {
                sysScpi->setFrequencyWithRpg(nums);
                drawMainPara();
                drawInputParamter();
                drawFreqInParamter();
            }
			break;
		case KEY_FUNC_FREQOFFSET:
			sysScpi->setFrequencyOffsetWithRpg(nums);
			drawMainPara();
			drawInputParamter();
			drawFreqInParamter();
			break;
		case KEY_FUNC_FREQMULTIPLIER:
			sysScpi->setFrequencyMultipliterWithRpg(nums);
			drawMainPara();
			drawInputParamter();
			drawFreqInParamter();
			break;
		case KEY_FUNC_AMPL:
            if (sysData.cursorLocation != -1)//不显示光标时不RPG键无用
            {
                sysScpi->setAmplitudeWithRpg(nums);
                drawMainPara();
                drawInputParamter();
                drawAmplInParamter();
            }
			break;
		case KEY_FUNC_AMPLOFFSET:
			sysScpi->setAmplitudeOffsetWithRpg(nums);
			drawMainPara();
			drawInputParamter();
			drawAmplInParamter();
			break;
		case KEY_FUNC_AMPL_CABLE:
			sysScpi->setAmplitudeErrorWithRpg(nums);
			drawMainPara();
			drawInputParamter();
			drawAmplInParamter();
			break;
		case KEY_FUNC_SWEEP_STEP_FREQSTART:
			sysScpi->setStepStartFrequencyWithRpg(nums);
			drawInputParamter();
			drawSweep(sysData.sweep.type);
			break;
		case KEY_FUNC_SWEEP_STEP_FREQSTOP:
			sysScpi->setStepStopFrequencyWithRpg(nums);
			drawInputParamter();
			drawSweep(sysData.sweep.type);
			break;
		case KEY_FUNC_SWEEP_STEP_AMPLSTART:
			sysScpi->setStepStartAmplitudeWithRpg(nums);
			drawInputParamter();
			drawSweep(sysData.sweep.type);
			break;
		case KEY_FUNC_SWEEP_STEP_AMPLSTOP:
			sysScpi->setStepStopAmplitudeWithRpg(nums);
			drawInputParamter();
			drawSweep(sysData.sweep.type);
			break;
		case KEY_FUNC_SWEEP_STEP_MORE_STEPDWELL:
			sysScpi->setStepDwellWithRpg(nums);
			drawInputParamter();
			drawSweep(sysData.sweep.type);
			break;
		case KEY_FUNC_SWEEP_STEP_MORE_POINTS:
			sysScpi->setStepPointsWithRpg(nums);
			drawInputParamter();
			drawSweep(sysData.sweep.type);
			break;
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_EDITITEM:
			if (sysData.sweep.list.selectLine == 0) //如果为频率值
			{
				sysData.sweep.list.data[sysData.sweep.list.selectRow].freq = sysScpi->setValueByRPG(sysData.sweep.list.data[sysData.sweep.list.selectRow].freq, nums, 0);
				if (sysData.sweep.list.data[sysData.sweep.list.selectRow].freq > MAXFREQ)
					sysData.sweep.list.data[sysData.sweep.list.selectRow].freq = MAXFREQ;
				else if (sysData.sweep.list.data[sysData.sweep.list.selectRow].freq < MINFREQ)
					sysData.sweep.list.data[sysData.sweep.list.selectRow].freq = MINFREQ;
				getFreqString(sysData.sweep.list.data[sysData.sweep.list.selectRow].freq, sysData.value);
				strcpy(sysData.unit, freqToQUnit(sysData.sweep.list.data[sysData.sweep.list.selectRow].freq).trimmed().toStdString().c_str());
				insertThousands(sysData.value);
				//界面绘制
				drawInputParamter();
				drawsListData();
			} else if (sysData.sweep.list.selectLine == 1) //如果为幅度值
			{
				sysData.sweep.list.data[sysData.sweep.list.selectRow].ampl = sysScpi->setValueByRPG(sysData.sweep.list.data[sysData.sweep.list.selectRow].ampl, nums, 2);
				if (sysData.sweep.list.data[sysData.sweep.list.selectRow].ampl < MINAMPL)
					sysData.sweep.list.data[sysData.sweep.list.selectRow].ampl = MINAMPL;
				else if (sysData.sweep.list.data[sysData.sweep.list.selectRow].ampl > MAXAMPL)
					sysData.sweep.list.data[sysData.sweep.list.selectRow].ampl = MAXAMPL;
				//界面绘制
				floatToStringDot2(sysData.sweep.list.data[sysData.sweep.list.selectRow].ampl, sysData.value);
				drawInputParamter();
				drawsListData();
			} else if (sysData.sweep.list.selectLine == 2) //如果为驻留时间
			{
				sysData.sweep.list.data[sysData.sweep.list.selectRow].dwell = sysScpi->setValueByRPG(sysData.sweep.list.data[sysData.sweep.list.selectRow].dwell, nums, 0);
				if (sysData.sweep.list.data[sysData.sweep.list.selectRow].dwell < MINDWELL)
					sysData.sweep.list.data[sysData.sweep.list.selectRow].dwell = MINDWELL;
				else if (sysData.sweep.list.data[sysData.sweep.list.selectRow].dwell > MAXDWELL)
					sysData.sweep.list.data[sysData.sweep.list.selectRow].dwell = MAXDWELL;
				getMsTimeValue(sysData.sweep.list.data[sysData.sweep.list.selectRow].dwell, sysData.value);
				strcpy(sysData.unit, timeToQUnit(sysData.sweep.list.data[sysData.sweep.list.selectRow].dwell).trimmed().toStdString().c_str());
				insertThousands(sysData.value);
				//界面绘制
				drawInputParamter();
				drawsListData();
			}
			break;
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
			drawInputParamter();
			drawsListData();
			break;
		case KEY_FUNC_AM_DEPTH:		//AM调制深度
			sysScpi->setAmDepthWithRpg(nums);
			drawInputParamter();
			drawAM();
			break;
		case KEY_FUNC_AM_INTERNALFCTN_FREQ:	//AM调制速率
			sysScpi->setAmRateWithRpg(nums);
			drawInputParamter();
			drawAM();
			break;
		case KEY_FUNC_FMPM_OFFSET://FM PM调制偏移
			if (sysData.fmpm.mode == fdtFM)
			{
				sysScpi->setFmDevWithRpg(nums);
				drawInputParamter();
			}
			else if (sysData.fmpm.mode == fdtPM)
			{
				sysScpi->setPmPhaseWithRpg(nums);
				drawInputParamter();
			}
			drawFMPM();
			break;
        case KEY_FUNC_FMPM_INTERNALFCTN_FREQ://FM PM调制速率
            if (sysData.fmpm.mode == fdtFM)
            {
                sysScpi->setFmRateWithRpg(nums);
                drawInputParamter();
            }
            else if (sysData.fmpm.mode == fdtPM)
            {
                sysScpi->setPmRateWithRpg(nums);
                drawInputParamter();
            }
            break;
		case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA_DEPTH:
			sysScpi->setDigitalModulateParaWithRpg(nums);
			drawInputParamter();
			drawDigitalModulate();
			break;
		case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA_DEVI:
			sysScpi->setDigitalModulateParaWithRpg(nums);
			drawInputParamter();
			drawDigitalModulate();
			break;
		case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA_PHASE:
			sysScpi->setDigitalModulateParaWithRpg(nums);
			drawInputParamter();
			drawDigitalModulate();
			break;
		case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA_RATE  :
			sysScpi->setDigitalModuateRateWithRpg(nums);
			drawInputParamter();
			drawDigitalModulate();
			break;
		case KEY_FUNC_PULSE_PERIOD:
			if (sysData.pulse.pulsesource == psInternal)
			{
				sysScpi->setPulsePeriodWithRpg(nums);
				sprintf(sysData.value, doubleToQValue(sysData.pulse.period).toStdString().c_str());
				insertThousands(sysData.value); //数组类型插入千分符
				sprintf(sysData.unit, timeToQUnit(sysData.pulse.period * 1e-6).toStdString().c_str());
				drawInputParamter();
				drawPulseModulate();
			}
			break;
		case KEY_FUNC_PULSE_WIDTH:
			if (sysData.pulse.pulsesource == psInternal)
			{
				sysScpi->setPulseWidthWithRpg(nums);
				sprintf(sysData.value, doubleToQValue(sysData.pulse.width).toStdString().c_str());
				insertThousands(sysData.value); //数组类型插入千分符
				sprintf(sysData.unit, timeToQUnit(sysData.pulse.width * 1e-6).toStdString().c_str());
				drawInputParamter();
				drawPulseModulate();
			}
			break;
		case KEY_FUNC_PULSE_DELAY:
			if (sysData.pulse.pulsesource == psInternal)
			{
				sysScpi->setPulseDelayWithRpg(nums);
				sprintf(sysData.value, doubleToQValue(sysData.pulse.delay).toStdString().c_str());
				insertThousands(sysData.value); //数组类型插入千分符
				sprintf(sysData.unit, timeToQUnit(sysData.pulse.delay * 1e-6).toStdString().c_str());
				drawInputParamter();
				drawPulseModulate();
			}
			break;
		case KEY_FUNC_AUXFCTN_LFOUT_AMPL:		//低频输出幅度
			sysScpi->setLfAmplWithRpg(nums);
			drawInputParamter();
			drawArbitraryWave();
			break;
		case KEY_FUNC_AUXFCTN_LFOUT_FREQ:		//低频输出频率
			sysScpi->setLfFreqWithRpg(nums);
			drawInputParamter();
			drawArbitraryWave();
			break;
		case KEY_FUNC_AUXFCTN_DRG_UPPER:		//DRG扫描上限
			sysScpi->setDrgUplimitWithRpg(nums);
			drawInputParamter();
			drawDrgFunction();
			break;
		case KEY_FUNC_AUXFCTN_DRG_LOW:			//DRG扫描下限
			sysScpi->setDrgDownlimitWithRpg(nums);
			drawInputParamter();
			drawDrgFunction();
			break;
		case KEY_FUNC_AUXFCTN_DRG_POINTS:		//DRG扫描点数
			sysScpi->setDrgPointsWithRpg(nums);
			drawInputParamter();
			drawDrgFunction();
			break;
		case KEY_FUNC_AUXFCTN_DRG_RATE:			//DRG扫描速率
			sysScpi->setDrgRateWithRpg(nums);
			drawInputParamter();
			drawDrgFunction();
			break;
		case KEY_FUNC_SYSTEM_SERVICE_LANMACADDR:	//MAC地址
			sysScpi->setSystemServiceOfLanMacAddressWidthRpg(nums);
			drawInputParamter();
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

		case KEY_FUNC_SYSTEM_SERVICE_EVALUATE_ON:	//试用开启
			sysScpi->setSystemServiceOfEvalute(true);
			break;
		case KEY_FUNC_SYSTEM_SERVICE_EVALUATE_OFF:	//试用关闭
			sysScpi->setSystemServiceOfEvalute(false);
			break;
		case KEY_FUNC_SYSTEM_LANGUAGE_CHINESE:		//显示中文
			sysScpi->setSystemLanguage(0);
			break;
		case KEY_FUNC_SYSTEM_LANGUAGE_ENGLISH:		//显示英文
			sysScpi->setSystemLanguage(1);
			break;
		case KEY_FUNC_FREQREFSET:
			sysData.sweep.mode = smOff;
			sysScpi->setFrequencyOfReference();
			sysData.inputLength = MAXFREQINPUTLENGTH;
			drawMainPara();
			drawInputParamter();
			drawFreqInParamter();
			break;
		case KEY_FUNC_AMPLREFSET:
			sysData.sweep.mode = smOff;
			sysScpi->setAmplitudeOfReference();
			sysData.inputLength = MAXAMPLINPUTLENGTH;
			drawMainPara();
			drawInputParamter();
			drawAmplInParamter();
			break;
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_INSERTROW:
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
			drawInputParamter();
			drawsListData();
			break;
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_DELETEROW:
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
			sysData.sweep.list.data[0].freq = MAXFREQ;
			sysData.sweep.list.data[0].ampl = MINAMPL;
			sysData.sweep.list.data[0].dwell = 20;
			sysData.sweep.list.data[1].freq = MAXFREQ;
			sysData.sweep.list.data[1].ampl = MINAMPL;
			sysData.sweep.list.data[1].dwell = 20;
			drawInputParamter();
			drawsListData();
			break;
		case KEY_FUNC_FREQREFSTATE_OFF:
			sysScpi->setFrequencyOfReferenceState(0);
			drawMainPara();
			drawFreqInParamter();
			drawInputParamter();
			break;
		case KEY_FUNC_FREQREFSTATE_ON:
			sysScpi->setFrequencyOfReferenceState(1);
			drawMainPara();
			drawFreqInParamter();
			drawInputParamter();
			break;
		case KEY_FUNC_AMPLREFSTATE_OFF:
			sysScpi->setAmplitudeOfReferenceState(0);

			drawMainPara();
			//	  drawFreqAndAmpl();
			drawAmplInParamter();
			drawInputParamter();
			break;

		case KEY_FUNC_AMPLREFSTATE_ON:
			sysScpi->setAmplitudeOfReferenceState(1);

			drawMainPara();
			//	  drawFreqAndAmpl();
			drawAmplInParamter();
			drawInputParamter();
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
			drawMainPara();
			drawSweep(sysData.sweep.type);

			break;
		case KEY_FUNC_SWEEP_MODE_FREQ:
			sysScpi->setSweepMode(smFreq);
			//界面绘制
			drawMainPara();
			drawSweep(sysData.sweep.type);
			break;

		case KEY_FUNC_SWEEP_MODE_AMPL:
			sysScpi->setSweepMode(smAmpl);
			//界面绘制
			drawMainPara();
			drawSweep(sysData.sweep.type);
			break;

		case KEY_FUNC_SWEEP_MODE_FREQAMPL:
			sysScpi->setSweepMode(smFreqAmpl);
			//界面绘制
			drawMainPara();
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

		//case KEY_FUNC_SWEEP_REPEAT_SINGLE:
		//	sysScpi->setSweepRepeat(srSingle);

		//	//界面绘制
		//	drawSweep(sysData.sweep.type);

		//	break;

		//case KEY_FUNC_SWEEP_REPEAT_CONT:
		//	sysScpi->setSweepRepeat(srConutinuos);

		//	//界面绘制
		//	drawSweep(sysData.sweep.type);

		//	break;
		
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
			sysScpi->closeDemodState();
			sysScpi->setSweepTrig(2);
			drawSweep(sysData.sweep.type);
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
//================================================= AM =============================================
		case KEY_FUNC_AM_INTERNALFCTN_WAVE_SINE:		//AM内部函数正弦波
			sysScpi->setAMInternalWave(aiwSine);
			drawAM();
			break;
		case KEY_FUNC_AM_INTERNALFCTN_WAVE_TRIANGLE:	//AM内部函数三角波
			sysScpi->setAMInternalWave(aiwTriangle);
			drawAM();
			break;
		case KEY_FUNC_AM_INTERNALFCTN_WAVE_SQUARE:		//AM内部函数方波
			sysScpi->setAMInternalWave(aiwSquare);
			drawAM();
			break;
		case KEY_FUNC_AM_INTERNALFCTN_WAVE_SAWTOOTH:	//AM内部函数锯齿波
			sysScpi->setAMInternalWave(aiwSawtooth);
			drawAM();
			break;
//================================================= FMPM =============================================
		case KEY_FUNC_FMPM_INTERNALFCTN_WAVE_SINE:	//FMPM内部函数正弦波
			if (sysData.fmpm.mode == fdtFM)
			{
				sysScpi->setFMInternalWave(fiwSine);
			}
			else if (sysData.fmpm.mode == fdtPM)
			{
				sysScpi->setPMInternalWave(piwSine);
			}
			drawFMPM();
			break;
		case KEY_FUNC_FMPM_INTERNALFCTN_WAVE_TRIANGLE://FMPM内部函数三角波
			if (sysData.fmpm.mode == fdtFM)
			{
				sysScpi->setFMInternalWave(fiwTriangle);
			}
			else if (sysData.fmpm.mode == fdtPM)
			{
				sysScpi->setPMInternalWave(piwTriangle);
			}
			drawFMPM();
			break;
		case KEY_FUNC_FMPM_INTERNALFCTN_WAVE_SQUARE://FMPM内部函数方波
			if (sysData.fmpm.mode == fdtFM)
			{
				sysScpi->setFMInternalWave(fiwSquare);
			}
			else if (sysData.fmpm.mode == fdtPM)
			{
				sysScpi->setPMInternalWave(piwSquare);
			}
			drawFMPM();
			break;
		case KEY_FUNC_FMPM_INTERNALFCTN_WAVE_SAWTOOTH://FMPM内部函数锯齿波
			if (sysData.fmpm.mode == fdtFM)
			{
				sysScpi->setFMInternalWave(fiwSawtooth);
			}
			else if (sysData.fmpm.mode == fdtPM)
			{
				sysScpi->setPMInternalWave(piwSawtooth);
			}
			drawFMPM();
			break;
//================================================= 数字调制 =============================================
		case KEY_FUNC_AUXFCTN_DIGITALMOD_MODE_ASK:
			sysScpi->setDigitalModuateType(dt2ASK);
			drawDigitalModulate();
			break;
		//case KEY_FUNC_FCTN_DIGITAL_TYPE_ASK_2ASK:
		//	sysScpi->setDigitalModuateType(dt2ASK);
		//	drawDigitalModulate();
		//	break;

		//case KEY_FUNC_FCTN_DIGITAL_TYPE_ASK_4ASK:
		//	sysScpi->setDigitalModuateType(dt4ASK);
		//	drawDigitalModulate();
		//	break;

		//case KEY_FUNC_FCTN_DIGITAL_TYPE_ASK_8ASK:
		//	sysScpi->setDigitalModuateType(dt8ASK);
		//	drawDigitalModulate();
		//	break;

		case KEY_FUNC_AUXFCTN_DIGITALMOD_MODE_FSK_2FSK:
			sysScpi->setDigitalModuateType(dt2FSK);
			drawDigitalModulate();
			break;
		case KEY_FUNC_AUXFCTN_DIGITALMOD_MODE_FSK_4FSK:
			sysScpi->setDigitalModuateType(dt4FSK);
			drawDigitalModulate();
			break;
		case KEY_FUNC_AUXFCTN_DIGITALMOD_MODE_FSK_8FSK:
			sysScpi->setDigitalModuateType(dt8FSK);
			drawDigitalModulate();
			break;
		case KEY_FUNC_AUXFCTN_DIGITALMOD_MODE_PSK_2PSK:
			sysScpi->setDigitalModuateType(dt2PSK);
			drawDigitalModulate();
			break;
		case KEY_FUNC_AUXFCTN_DIGITALMOD_MODE_PSK_4PSK:
			sysScpi->setDigitalModuateType(dt4PSK);
			drawDigitalModulate();
			break;
		case KEY_FUNC_AUXFCTN_DIGITALMOD_MODE_FSK_8PSK:
			sysScpi->setDigitalModuateType(dt8PSK);
			drawDigitalModulate();
			break;		
//============================================= Pulse ===================================
		case KEY_FUNC_PULSE_TRIGGERWAY_FREERUN:
			if (sysData.pulse.pulsesource == psInternal)
			{
				sysScpi->setPulseMethod(plmFreerun);
				drawPulseModulate();
			}
			break;
		case KEY_FUNC_PULSE_TRIGGERWAY_KEYTRIG:
			if (sysData.pulse.pulsesource == psInternal)
			{
				sysScpi->setPulseMethod(plmKeytrig);
				drawPulseModulate();
			}
			break;
		case KEY_FUNC_PULSE_TRIGGERWAY_EXTTRIG:
			if (sysData.pulse.pulsesource == psInternal)
			{
				sysScpi->setPulseMethod(plmExttrig);
				drawPulseModulate();
			}
			break;

		//=========================================AUX LF ===================================
		//case KEY_FUNC_FCTN_LF_STATE_ON:
		//	sysScpi->setLFState(1);
		//	sysData.keyBoardSwitchLF = 0x01;
		//	sysScpi->keyBoardLEDSwitch(2, sysData.keyBoardSwitchLF);
		//	drawArbitraryWave();
		//	break;

		//case KEY_FUNC_FCTN_LF_STATE_OFF:
		//	sysScpi->setLFState(0);
		//	sysData.keyBoardSwitchLF = 0x00;
		//	sysScpi->keyBoardLEDSwitch(2, sysData.keyBoardSwitchLF);
		//	drawArbitraryWave();
		//	break;
		case KEY_FUNC_AUXFCTN_LFOUT_WAVE_SINE:	//正弦波
			sysScpi->setLfType(lotSine);
			strcpy(sysData.name, "");
			strcpy(sysData.value, "");
			strcpy(sysData.unit, "");
			drawInputParamter();
			drawArbitraryWave();
			break;
		case KEY_FUNC_AUXFCTN_LFOUT_WAVE_SQUARE:	//方波
			sysScpi->setLfType(lotSquare);
			strcpy(sysData.name, "");
			strcpy(sysData.value, "");
			strcpy(sysData.unit, "");
			drawInputParamter();
			drawArbitraryWave();
			break;
		case KEY_FUNC_AUXFCTN_LFOUT_WAVE_TRIANGLE:	//三角波
			sysScpi->setLfType(lotTriangle);
			strcpy(sysData.name, "");
			strcpy(sysData.value, "");
			strcpy(sysData.unit, "");
			drawInputParamter();
			drawArbitraryWave();
			break;
		case KEY_FUNC_AUXFCTN_LFOUT_WAVE_SAWTOOTH:	//锯齿波
			sysScpi->setLfType(lotSawtooth);
			strcpy(sysData.name, "");
			strcpy(sysData.value, "");
			strcpy(sysData.unit, "");
			drawInputParamter();
			drawArbitraryWave();
			break;

		//case KEY_FUNC_FCTN_LF_SOURCE_SINC:
		//	sysScpi->setLfType(lotSinc);
		//	strcpy(sysData.name, "");
		//	strcpy(sysData.value, "");
		//	strcpy(sysData.unit, "");
		//	drawInputParamter();
		//	drawArbitraryWave();
		//	break;
			//DRG
		//case KEY_FUNC_FCTN_DRG_STATE_ON:
		//	sysScpi->setDrgState(1);
		//	drawDrgFunction();
		//	break;
		//case KEY_FUNC_FCTN_DRG_STATE_OFF:
		//	sysScpi->setDrgState(0);
		//	drawDrgFunction();
		//	break;
		case KEY_FUNC_AUXFCTN_DRG_MODE_BOTHWAY:
			sysScpi->setDrgWorkmode(ddnormal);
			strcpy(sysData.name, "");
			strcpy(sysData.value, "");
			strcpy(sysData.unit, "");
			drawInputParamter();
			drawDrgFunction();
			break;
		case KEY_FUNC_AUXFCTN_DRG_MODE_POSSINGLE:
			sysScpi->setDrgWorkmode(ddpositive);
			//界面绘制
			strcpy(sysData.name, "");
			strcpy(sysData.value, "");
			strcpy(sysData.unit, "");
			drawInputParamter();
			drawDrgFunction();
			break;
		case KEY_FUNC_AUXFCTN_DRG_MODE_NEGSINGLE:
			sysScpi->setDrgWorkmode(ddnegative);
			//界面绘制
			strcpy(sysData.name, "");
			strcpy(sysData.value, "");
			strcpy(sysData.unit, "");
			drawInputParamter();
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
		case KEY_MAIN_FREQ:		//频率键
			strcpy(sysData.titleName, getTheTranslatorString("Freq").toStdString().c_str());
			break;
		case KEY_MAIN_AMPTD:	//幅度键
			strcpy(sysData.titleName, getTheTranslatorString("Amptd").toStdString().c_str());
			break;
		case KEY_MAIN_PRESET:
			strcpy(sysData.titleName, getTheTranslatorString("Freq").toStdString().c_str());			
			presetSystem(false);
			sysScpi->initKeyBoardLED();
			sysScpi->sendSerialData("*RST");
			sysScpi->controlParamterDown();
			strcpy(sysData.name, "");
			strcpy(sysData.value, "");
			strcpy(sysData.unit, "");
			break;
		case KEY_MAIN_SWEEP:	//扫描键
			strcpy(sysData.titleName, getTheTranslatorString("Sweep").toStdString().c_str());
			drawSweep(sysData.sweep.type);
			strcpy(sysData.name, "");
			strcpy(sysData.value, "");
			strcpy(sysData.unit, "");
			drawInputParamter();
			drawButton();
			drawSweep(0);
			break;
		case KEY_MAIN_AM:	//AM
			strcpy(sysData.titleName, getTheTranslatorString("AM").toStdString().c_str());
			drawAM();
			strcpy(sysData.name, "");
			strcpy(sysData.value, "");
			strcpy(sysData.unit, "");
			drawInputParamter();
			drawButton();
			break;
		case KEY_MAIN_FMPM:	//FM/PM
			strcpy(sysData.titleName, getTheTranslatorString("FM/φM").toStdString().c_str());
			drawFMPM();
			strcpy(sysData.name, "");
			strcpy(sysData.value, "");
			strcpy(sysData.unit, "");
			drawInputParamter();
			drawButton();
			break;
		case KEY_MAIN_IQ:
			strcpy(sysData.titleName, getTheTranslatorString("IQ").toStdString().c_str());
			if (DEVICETYPE == 0)		//IQ调制只支持SG1030B
			{
				//drawErrorMessage(getTheTranslatorString("Do not support the function"));
			}
			drawIQ();
			strcpy(sysData.name, "");
			strcpy(sysData.value, "");
			strcpy(sysData.unit, "");
			drawInputParamter();
			drawButton();
			break;
		case KEY_MAIN_PULSE:	//Pulse
			strcpy(sysData.titleName, getTheTranslatorString("Pulse Mod").toStdString().c_str()); 
			drawPulseModulate();
			strcpy(sysData.name, "");
			strcpy(sysData.value, "");
			strcpy(sysData.unit, "");
			drawInputParamter();
			drawButton();
			break;
		case KEY_MAIN_AUXFCTN:	
			strcpy(sysData.name, "");
			strcpy(sysData.value, "");
			strcpy(sysData.unit, "");
			drawInputParamter();
			drawButton();
			break;
		case KEY_MAIN_RF1ONOFF:		//RF1
			//if (nowSubKey->isUnit)
			//{
			//	if (nowSubKey->leftNode != NULL)
			//		nowSubKey = nowSubKey->leftNode;
			//}
			//drawButton();
			//funcCode = nowSubKey->funcCode;
			//sysData.keyBoardSwitchRF = (~sysData.keyBoardSwitchRF) & 0x01;
			sysData.led.RF1 = !sysData.led.RF1;
			sysScpi->keyBoardLEDSwitch(5, sysData.led.RF1);
			sprintf(tempChar, "%d", sysData.led.RF1);
			sendToSG1030QS = QString(":SYST:RFO ") + QString(tempChar).trimmed();
			sysScpi->sendSerialData(sendToSG1030QS);
			drawMainPara();
			drawState();
			break;
		case KEY_MAIN_RF2ONOFF:		//RF2
			break;
		case KEY_MAIN_MODONOFF:		//MOD
			//if (nowSubKey->isUnit)
			//{
			//	if (nowSubKey->leftNode != NULL)
			//		nowSubKey = nowSubKey->leftNode;
			//}
			//drawButton();
			//funcCode = nowSubKey->funcCode;
			//sysData.keyBoardSwitchMod = (~sysData.keyBoardSwitchMod) & 0x01;
			sysData.led.Mod = !sysData.led.Mod;
			sysScpi->keyBoardLEDSwitch(4, sysData.led.Mod);
			sprintf(tempChar, "%d", sysData.led.Mod);
			sendToSG1030QS = QString(":SYST:MODO ") + QString(tempChar).trimmed();
			sysScpi->sendSerialData(sendToSG1030QS);
			drawState();
			break;
		case KEY_MAIN_LFOUT:		//LF
			//if (nowSubKey->isUnit)
			//{
			//	if (nowSubKey->leftNode != NULL)
			//		nowSubKey = nowSubKey->leftNode;
			//}
			//drawButton();
			funcCode = nowSubKey->funcCode;
			//sysData.keyBoardSwitchLF = (~sysData.keyBoardSwitchLF) & 0x01;
			sysData.led.LFOut = !sysData.led.LFOut;
			sysScpi->keyBoardLEDSwitch(3, sysData.led.LFOut);
			sysData.lf.state = sysData.led.LFOut;
			sprintf(tempChar, "%d", sysData.led.LFOut);
			sendToSG1030QS = QString(":LFOUT:SWITCH ") + QString(tempChar).trimmed();
			sysScpi->sendSerialData(sendToSG1030QS);
			if (funcCode == KEY_FUNC_AUXFCTN_LFOUT_WAVE || funcCode == KEY_FUNC_AUXFCTN_LFOUT_FREQ || funcCode == KEY_FUNC_AUXFCTN_LFOUT_AMPL)
			{
				drawArbitraryWave();
			}
			drawState();
			drawButton();
			break;
		case KEY_MAIN_SYSTEM:		//System
			strcpy(sysData.titleName, getTheTranslatorString("System").toStdString().c_str());
			strcpy(sysData.name, "");
			strcpy(sysData.value, "");
			strcpy(sysData.unit, "");
			drawInputParamter();
			drawSystemPara();
			drawButton();
			break;
		default:
			break;
	}
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
			//sysData.cursorLocation = -1;
			name = "FREQ:";
			strcpy(tempValue, getFreqString(sysData.freq.freq, temp));
			insertThousands(tempValue);
			value = QString(tempValue).trimmed();
			unit = freqToQUnit(sysData.freq.freq).trimmed();
			inputValue = "";
			sysData.inputLength = MAXFREQINPUTLENGTH;
			drawMainPara();
			drawFreqInParamter();
			break;
		case KEY_FUNC_FREQOFFSET:
			name = "FREQ:";
			inputValue = "";
			strcpy(tempValue, getFreqString(sysData.freq.freq, temp));
			insertThousands(tempValue);
			value = QString(tempValue).trimmed();
			unit = freqToQUnit(sysData.freq.freq).trimmed();
			sysData.inputLength = MAXFREQINPUTLENGTH;
			drawMainPara();
			drawFreqInParamter();
			break;
		case KEY_FUNC_FREQMULTIPLIER:
			name = "FREQ:";
			inputValue = "";
			strcpy(tempValue, getFreqString(sysData.freq.freq, temp));
			insertThousands(tempValue);
			value = QString(tempValue).trimmed();
			unit = freqToQUnit(sysData.freq.freq).trimmed();
			sysData.inputLength = MAXFREQMULINPUTLENGTH;
			drawMainPara();
			drawFreqInParamter();
			break;
		case KEY_FUNC_AMPL:
			name = "AMPL:";
			strcpy(tempValue, floatToStringDot2(sysData.ampl.ampl, temp));
			unit = amplToQUnit(sysData.ampl.ampl);
			value = QString(tempValue).trimmed();
			inputValue = "";
			sysData.inputLength = MAXAMPLINPUTLENGTH;
			drawMainPara();
			drawAmplInParamter();
			break;
		case KEY_FUNC_AMPLOFFSET:
			name = "AMPL:";
			strcpy(tempValue, floatToStringDot2(sysData.ampl.ampl, temp));
			value = QString(tempValue).trimmed();
			unit = amplToQUnit(sysData.ampl.offset).trimmed();
			inputValue = "";
			sysData.inputLength = MAXAMPLINPUTLENGTH;
			drawMainPara();
			drawAmplInParamter();
			break;
		case KEY_FUNC_AMPL_CABLE:
			name = "I.L Cable:";
			strcpy(tempValue, floatToStringDot2(sysData.ampl.error.value, temp));
			value = QString(tempValue).trimmed();
			unit = "dB";
			inputValue = "";
			sysData.inputLength = MAXAMPLINPUTLENGTH;
			drawMainPara();
			drawAmplInParamter();
			break;
//=================================== 增量 ========================================
		case KEY_FUNC_INCRSET_SET:
			if (sysData.incrSet.type == itdFreq)
			{
				name = "Incr Freq:";
				strcpy(tempValue, getFreqString(sysData.incrSet.FreqIncrValue, temp));
				insertThousands(tempValue);
				value = QString(tempValue).trimmed();
				unit = freqToQUnit(sysData.incrSet.FreqIncrValue).trimmed();
				inputValue = "";
				strcpy(sysData.name, name.toStdString().c_str());
				strcpy(sysData.value, value.toStdString().c_str());
				strcpy(sysData.unit, unit.toStdString().c_str());
				sysData.inputLength = MAXFREQINPUTLENGTH;
			}
			else if (sysData.incrSet.type == itdAmptd)
			{
				name = "Incr Ampl:";
				strcpy(tempValue, floatToStringDot2(sysData.incrSet.AmptdIncrValue, temp));
				unit = "dB";
				value = QString(tempValue).trimmed();
				inputValue = "";
				strcpy(sysData.name, name.toStdString().c_str());
				strcpy(sysData.value, value.toStdString().c_str());
				strcpy(sysData.unit, unit.toStdString().c_str());
				sysData.inputLength = MAXAMPLINPUTLENGTH;
			}
			break;
//=================================== 扫描 ========================================
		case KEY_FUNC_SWEEP_STEP_MORE_STEPDWELL:
			name = "Dwell:";
			getMsTimeValue(sysData.sweep.step.dwell, temp);
			strcpy(tempValue, temp);
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
	//======================================================== AM ===============================================
		case KEY_FUNC_AM_DEPTH:		//AM调制深度
			name = "AM Depth:";
			inputValue = "";
			value = QString(floatToStringDot2(sysData.am.depth, temp)).trimmed();
			unit = "%";
			sysData.inputLength = MAXINPUTLENGTH;
			inputValue = "";
			drawAM();
			break;
		case KEY_FUNC_AM_INTERNALFCTN_FREQ:	//AM调制速率
			name = "AM Freq:";
			inputValue = "";
			strcpy(tempValue, getFreqString(sysData.am.rate, temp));
			insertThousands(tempValue);
			value = QString(tempValue).trimmed();
			unit = freqToQUnit(sysData.am.rate).trimmed();
			sysData.inputLength = MAXFREQINPUTLENGTH;
			drawAM();
			break;
	//======================================================== FMPM ===============================================
		case KEY_FUNC_FMPM_OFFSET:	//调制偏移
			if (sysData.fmpm.mode == fdtFM)
			{
				name = "FM Deviation:";
				inputValue = "";
				strcpy(tempValue, getFreqString(sysData.fmpm.fm.deviation, temp));
				insertThousands(tempValue);
				value = QString(tempValue).trimmed();
				unit = freqToQUnit(sysData.fmpm.fm.deviation).trimmed();
				sysData.inputLength = MAXFREQINPUTLENGTH;
				inputValue = "";
				drawFMPM();
			}
			else if (sysData.fmpm.mode == fdtPM)
			{
				name = "PM Phase:";
				inputValue = "";
				value = doubleToQValue(sysData.fmpm.pm.phase).trimmed();
				unit = "°";
				sysData.inputLength = MAXINPUTLENGTH;
				inputValue = "";
				drawFMPM();
			}
			break;
		case KEY_FUNC_FMPM_INTERNALFCTN_FREQ:	//调制率
			if (sysData.fmpm.mode == fdtFM)
			{
				name = "FM Rate:";
				inputValue = "";
				strcpy(tempValue, getFreqString(sysData.fmpm.fm.rate, temp));
				insertThousands(tempValue);
				value = QString(tempValue).trimmed();
				unit = freqToQUnit(sysData.fmpm.fm.rate).trimmed();
				sysData.inputLength = MAXFREQINPUTLENGTH;
				inputValue = "";
				drawFMPM();
			}
			else if (sysData.fmpm.mode == fdtPM)
			{
				name = "PM Rate:";
				inputValue = "";
				strcpy(tempValue, getFreqString(sysData.fmpm.pm.rate, temp));
				insertThousands(tempValue);
				value = QString(tempValue).trimmed();
				unit = freqToQUnit(sysData.fmpm.pm.rate).trimmed();
				sysData.inputLength = MAXFREQINPUTLENGTH;
				inputValue = "";
				drawFMPM();
			}
			break;

		/*case KEY_FUNC_FCTN_ANALOG_FM_DEVIATION:
			name = "FM Deviation:";
			inputValue = "";

			strcpy(tempValue, getFreqString(sysData.fmpm.fm.deviation, temp));
			insertThousands(tempValue);
			value = QString(tempValue).trimmed();
			unit = freqToQUnit(sysData.fmpm.fm.deviation).trimmed();

			sysData.inputLength = MAXFREQINPUTLENGTH;

			inputValue = "";
			drawAnalogModulate();
			break;

		case KEY_FUNC_FCTN_ANALOG_PM_PHASE:
			name = "PM Phase:";
			inputValue = "";
			value = doubleToQValue(sysData.fmpm.pm.phase).trimmed();
			unit = "°";
			sysData.inputLength = MAXINPUTLENGTH;
			inputValue = "";
			drawAnalogModulate();
			break;

		case KEY_FUNC_FCTN_ANALOG_FM_RATE:
			name = "FM Rate:";
			inputValue = "";

			strcpy(tempValue, getFreqString(sysData.fmpm.fm.rate, temp));
			insertThousands(tempValue);
			value = QString(tempValue).trimmed();
			unit = freqToQUnit(sysData.fmpm.fm.rate).trimmed();

			sysData.inputLength = MAXFREQINPUTLENGTH;

			inputValue = "";
			drawAnalogModulate();
			break;

		case KEY_FUNC_FCTN_ANALOG_PM_RATE:
			name = "PM Rate:";
			inputValue = "";

			strcpy(tempValue, getFreqString(sysData.fmpm.pm.rate, temp));
			insertThousands(tempValue);
			value = QString(tempValue).trimmed();
			unit = freqToQUnit(sysData.fmpm.pm.rate).trimmed();

			sysData.inputLength = MAXFREQINPUTLENGTH;

			inputValue = "";
			drawAnalogModulate();
			break;
*/
		case KEY_FUNC_AUXFCTN_DIGITALMOD_EXTLOAD:
			drawUSBFileEntry(DIGITALDATAENTRY);
			break;

		case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA_DEPTH:
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

		case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA_DEVI:
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

		case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA_PHASE:
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

		case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA_RATE:
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
			 name = "Pulse Period:";
			 inputValue = "";

			 strcpy(tempValue,floatToStringDot3(valueToDValue(sysData.analog.pluse.period), temp));
			 value = QString(tempValue).trimmed();
			 unit = demodTimeToQUnit(sysData.analog.pluse.period).trimmed();

			 sysData.inputLength = MAXTIMEINPUTLENGTH;

			 inputValue = "";
			 drawAnalogModulate();
			 break;*/
			/*case KEY_FUNC_FCTN_DIGITAL_PLUSE_WIDTH:
			 name = "Pulse Width:";
			 inputValue = "";

			 strcpy(tempValue, floatToStringDot3(valueToDValue(sysData.analog.pluse.width), temp));
			 value = QString(tempValue).trimmed();
			 unit = demodTimeToQUnit(sysData.analog.pluse.width).trimmed();

			 sysData.inputLength = MAXTIMEINPUTLENGTH;

			 inputValue = "";
			 drawAnalogModulate();
			 break;*/

//========================================================= Pulse ============================================
		case KEY_FUNC_PULSE_PERIOD:
			if (sysData.pulse.pulsesource == psInternal)
			{
				name = "Period:";
				value = doubleToQValue(sysData.pulse.period);
				value = insertThousandsQS(value);
				unit = timeToQUnit(sysData.pulse.period * 1e-6);
				inputValue = "";
				sysData.inputLength = MAXPLUSEINPUTLENGTH;
				drawPulseModulate();
			}
			break;

		case KEY_FUNC_PULSE_WIDTH:
			if (sysData.pulse.pulsesource == psInternal)
			{
				name = "Width:";
				value = doubleToQValue(sysData.pulse.width);
				value = insertThousandsQS(value);
				unit = timeToQUnit(sysData.pulse.width * 1e-6);
				inputValue = "";
				sysData.inputLength = MAXPLUSEINPUTLENGTH;
				drawPulseModulate();
			}
			break;
		case KEY_FUNC_PULSE_DELAY:
			if (sysData.pulse.pulsesource == psInternal)
			{
				name = "Delay:";
				value = doubleToQValue(sysData.pulse.delay);
				value = insertThousandsQS(value);
				unit = timeToQUnit(sysData.pulse.delay * 1e-6);
				inputValue = "";
				sysData.inputLength = MAXPLUSEINPUTLENGTH;
				drawPulseModulate();
			}
			break;
//========================================================= Aux 上变频 ============================================
		case KEY_FUNC_AUXFCTN_EXTRF_EXTIF:
			name = "EXT IF Freq:";
			inputValue = "";
			strcpy(tempValue, getFreqString(sysData.exfRef.freq, temp));
			insertThousands(tempValue);
			value = QString(tempValue).trimmed();
			unit = freqToQUnit(sysData.exfRef.freq).trimmed();
			sysData.inputLength = MAXFREQINPUTLENGTH;
			drawExfFref();
			break;
//========================================================= Aux LF OUT ============================================
		case KEY_FUNC_AUXFCTN_LFOUT_AMPL:
			name = "LF Ampl:";
			value = doubleToQValue(sysData.lf.ampl);
			value = value.trimmed();
			unit = vppToQUnit(sysData.lf.ampl).trimmed();
			inputValue = "";
			sysData.inputLength = MAXLFINPUTLENGTH;
			drawArbitraryWave();
			break;
		case KEY_FUNC_AUXFCTN_LFOUT_FREQ:
			name = "LF Freq:";
			inputValue = "";
			strcpy(tempValue, getFreqStringInLF(sysData.lf.freq, temp));
			insertThousands(tempValue);
			value = QString(tempValue).trimmed();
			unit = freqToQUnit(sysData.lf.freq).trimmed();
			sysData.inputLength = MAXFREQINPUTLENGTH;
			drawArbitraryWave();
			break;
//========================================================= Aux DRG ============================================
		case KEY_FUNC_AUXFCTN_DRG_UPPER:	//扫描上限
			name = "Up Limit:";
			inputValue = "";
			strcpy(tempValue, getFreqString(sysData.drg.uplimit, temp));
			insertThousands(tempValue);
			value = QString(tempValue).trimmed();
			unit = freqToQUnit(sysData.drg.uplimit).trimmed();
			sysData.inputLength = MAXDRGFREQINPUTLENGTH;
			drawDrgFunction();
			break;
		case KEY_FUNC_AUXFCTN_DRG_LOW:	//扫描下限
			name = "Down Limit:";
			inputValue = "";
			strcpy(tempValue, getFreqString(sysData.drg.downlimit, temp));
			insertThousands(tempValue);
			value = QString(tempValue).trimmed();
			unit = freqToQUnit(sysData.drg.downlimit).trimmed();
			sysData.inputLength = MAXDRGFREQINPUTLENGTH;
			drawDrgFunction();
			break;
		case KEY_FUNC_AUXFCTN_DRG_POINTS:	//扫描点数
			name = "Points:";
			strcpy(tempValue, intToString(sysData.drg.points, temp));
			value = QString(tempValue).trimmed();
			inputValue = "";
			sysData.inputLength = MAXPOINTINPUTLENGTH;
			drawDrgFunction();
			break;
		case KEY_FUNC_AUXFCTN_DRG_RATE:	//扫描速率
			name = "Rate:";
			value = doubleToQValue(sysData.drg.rate);
			value = insertThousandsQS(value);
			unit = timeToQUnit(sysData.drg.rate * 1e-6);
			inputValue = "";
			sysData.inputLength = MAXDRGTIMEINPUTLENGTH;
			drawDrgFunction();
			break;
//========================================================= Aux USB Power ============================================
		case KEY_FUNC_AUXFCTN_USBPOWER_CHFREQ:	//通道频率
			name = "Freq:";
			strcpy(tempValue, getFreqString(sysData.usbPower.freq, temp));
			insertThousands(tempValue);
			value = QString(tempValue).trimmed();
			unit = freqToQUnit(sysData.usbPower.freq).trimmed();
			sysData.inputLength = MAXDRGFREQINPUTLENGTH;
			drawUSBPower();
			break;
		case KEY_FUNC_AUXFCTN_USBPOWER_AMPLOFS:	//功率偏移
			name = "Ampl Offset:";
			strcpy(tempValue, floatToStringDot2(sysData.usbPower.powerOffset, temp));
			unit = "dB";
			value = QString(tempValue).trimmed();
			inputValue = "";
			sysData.inputLength = MAXAMPLINPUTLENGTH;
			drawUSBPower();
			break;
		case KEY_FUNC_AUXFCTN_USBPOWER_AVERAGE_COUNT:	//平均次数
			name = "Count:";
			strcpy(tempValue, intToString(sysData.usbPower.AdvCount, temp));
			value = QString(tempValue).trimmed();
			inputValue = "";
			sysData.inputLength = MAXPOINTINPUTLENGTH;
			drawUSBPower();
			break;
//========================================================= System ============================================
		case KEY_FUNC_SYSTEM_LAN_IP:
			name = "IP:";
			inputValue = "";
			value = QString(sysData.system.io.lan.ip).trimmed();
			unit = "";
			sysData.inputLength = 15;
			drawSystemPara();
			break;
		case KEY_FUNC_SYSTEM_LAN_MASK:
			name = "Mask:";
			inputValue = "";
			value = QString(sysData.system.io.lan.mask).trimmed();
			unit = "";
			sysData.inputLength = 15;
			drawSystemPara();
			break;
		case KEY_FUNC_SYSTEM_LAN_GATEWAY:
			name = "Gateway:";
			inputValue = "";
			value = QString(sysData.system.io.lan.gate).trimmed();
			unit = "";
			sysData.inputLength = 15;
			drawSystemPara();
			break;
		case KEY_FUNC_SYSTEM_SERVICE: //系统服务
			name = "Pass Word:";
			value = "******";
			unit = "";
			sysData.inputLength = 6;
			break;
		case KEY_FUNC_SYSTEM_SERVICE_LANMACADDR:
			name = "Mac Address:";
			inputValue = "";
			for (int i = 0; i < sizeof sysData.system.lanMacAddress / sizeof sysData.system.lanMacAddress[0]; i++)
			{
				intToHex(sysData.system.lanMacAddress[i], tempMac);
				value += QString(tempMac[0]);

			}
			unit = "";
			sysData.system.lanMacAddrIndex = 11;
			sysData.inputLength = 12;
			break;
	}

	strcpy(sysData.name, name.toStdString().c_str());
	strcpy(sysData.value, value.toStdString().c_str());
	strcpy(sysData.unit, unit.toStdString().c_str());

	drawInputParamter();
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
	switch (funcCode)
	{
		case KEY_FUNC_AUXFCTN_EXTRF:	//上变频
			strcpy(sysData.titleName, getTheTranslatorString("Ext Rf").toStdString().c_str());
			drawExfFref();
			break;
		case KEY_FUNC_AUXFCTN_LFOUT:	//低频输出
			strcpy(sysData.titleName, getTheTranslatorString("LF Out").toStdString().c_str());
			drawArbitraryWave();
			break;
		case KEY_FUNC_AUXFCTN_DRG:		//DRG
			strcpy(sysData.titleName, getTheTranslatorString("DRG").toStdString().c_str());
			drawDrgFunction();
			break;
		case KEY_FUNC_AUXFCTN_DIGITALMOD:	//数字调制
			strcpy(sysData.titleName, getTheTranslatorString("Digital Mod").toStdString().c_str());
			drawDigitalModulate();
			break;
		case KEY_FUNC_AUXFCTN_USBPOWER:	//USB功率计
			strcpy(sysData.titleName, getTheTranslatorString("USB Power").toStdString().c_str());
			drawUSBPower();
			break;
		case KEY_FUNC_SYSTEM_LAN:	//LAN
			drawSystemPara();
			break;
		case KEY_FUNC_SYSTEM_LANGUAGE:
			strcpy(sysData.name, "");
			strcpy(sysData.value, "");
			strcpy(sysData.unit, "");
			//drawInputParamter();
			break;
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST:
			drawsListData();
			break;
		default:
			break;
	}
	drawInputParamter();
}

//处理取反键
void mainForm::handleReverseKey(int funcCode)
{
	double tempValue = 0;
	bool tempState = false;
	int retValue = 0;
	switch (funcCode)
	{
		//======================================================频率=====================================
		case KEY_FUNC_FREQREFSTATE:			//频率参考状态
			sysScpi->setFrequencyOfReferenceState(!sysData.freq.ref.state);
			drawMainPara();
			drawInputParamter();
			drawFreqInParamter();
			break;
		case KEY_FUNC_FREQREF10M:			//频率参考时基
			sysScpi->setRef10MState((sysData.freq.ref10MHz == rmInternal ? rmExternal : rmInternal));
			drawMainPara();
			drawInputParamter();
			drawFreqInParamter();
			break;
		//======================================================幅度====================================
		case KEY_FUNC_AMPLREFSTATE:			//幅度参考状态
			sysScpi->setAmplitudeOfReferenceState(!sysData.ampl.ref.state);
			drawMainPara();
			drawInputParamter();
			drawAmplInParamter();
			break;
		case KEY_FUNC_AMPLRESPONSE:			//稳幅响应
			sysScpi->setAmplResponse(!sysData.ampl.alc.response);
			drawMainPara();
			drawInputParamter();
			drawAmplInParamter();
			break;
		case KEY_FUNC_AMPL_ALC_STAT:		//稳幅状态
			sysScpi->setALCState(!sysData.ampl.alc.state);
			drawMainPara();
			drawInputParamter();
			drawAmplInParamter();
			break;
		//======================================================扫描====================================
		case KEY_FUNC_SWEEP_TYPE:			//扫描类型
			sysScpi->setSweepType(sysData.sweep.type == stStep ? stList : stStep);
			if (sysData.sweep.type == stList)
			{
				sysScpi->setSweepListDownload(); //下发列表数据
			}
			drawSweep(sysData.sweep.type);
			break;
		case KEY_FUNC_SWEEP_REPEAT:		//重复扫描
			sysScpi->setSweepRepeat(sysData.sweep.repeat == srSingle ? srConutinuos : srSingle);
			drawSweep(sysData.sweep.type);
			break;
		case KEY_FUNC_SWEEP_MORE_SWEEPRETRACE: //折返
			if (sysData.sweep.retrace == srOn)
				sysScpi->setSweepRetrace(srOff);
			else
				sysScpi->setSweepRetrace(srOn);
			//界面绘制
			drawSweep(sysData.sweep.type);
			break;
		case KEY_FUNC_SWEEP_MORE_SWEEPDIRECTION:	//扫描方向
			if (sysData.sweep.direction == sdUp)
				sysScpi->setSweepDirection(sdDown);
			else
				sysScpi->setSweepDirection(sdUp);
			//界面绘制
			drawSweep(sysData.sweep.type);
			break;
		//====================================================== AM ====================================
		case KEY_FUNC_AM_STATE:		//AM调制状态
			sysScpi->setAmState(!sysData.am.state);
			//绘制
			drawAM();
			break;
		case KEY_FUNC_AM_SOURCE:	//AM调制源
			sysScpi->setAMSource(!sysData.am.source);
			drawAM();
			break;
		case KEY_FUNC_AM_COUPLING:	//AM深度耦合
			sysScpi->setAmCoupling(!sysData.am.coupling);
			drawAM();
			break;
		//===================================================== FMPM ===================================
		case KEY_FUNC_FMPM_MODE:	//FMPM调制模式
			if (sysData.fmpm.mode == fdtFM)
				sysScpi->setFMPMMode(fdtPM);
			else if (sysData.fmpm.mode == fdtPM)
				sysScpi->setFMPMMode(fdtFM);
			strcpy(sysData.name, "");
			strcpy(sysData.value, "");
			strcpy(sysData.unit, "");
			drawInputParamter();
			drawFMPM();
			break;
		case KEY_FUNC_FMPM_STATUS:	//FMPM调制状态
			if (sysData.fmpm.mode == fdtFM)
				sysScpi->setFmState(!sysData.fmpm.fm.state);
			else if (sysData.fmpm.mode == fdtPM)
				sysScpi->setPmState(!sysData.fmpm.pm.state);
			drawFMPM();
			break;
		case KEY_FUNC_FMPM_SOURCE:	//调制源
			if (sysData.fmpm.mode == fdtFM)
				sysScpi->setFMSource(!sysData.fmpm.fm.source);
			else if (sysData.fmpm.mode == fdtPM)
				sysScpi->setPMSource(!sysData.fmpm.pm.source);
			drawFMPM();
			break;
		case KEY_FUNC_FMPM_COUPLING://偏移耦合
			if (sysData.fmpm.mode == fdtFM)
				sysScpi->setFMCoupling(!sysData.fmpm.fm.coupling);
			else if (sysData.fmpm.mode == fdtPM)
				sysScpi->setPMCoupling(!sysData.fmpm.pm.coupling);
			drawFMPM();
			break;
		//===================================================== Pulse ===================================
		case KEY_FUNC_PULSE_STATE:	//脉冲调制状态
			sysScpi->setPulseState(!sysData.pulse.state);
			drawPulseModulate();
			break;
		case KEY_FUNC_PULSE_SOURCE:	//脉冲源
			if (sysData.pulse.pulsesource == psInternal)
				sysScpi->setPulseSource(psExternal);
			else if (sysData.pulse.pulsesource == psExternal)
				sysScpi->setPulseSource(psInternal);
			drawPulseModulate();
			break;
		case KEY_FUNC_PULSE_EXTPOLA:	//外部触发信号极性
			if (sysData.pulse.polarity == pepRising)
				sysScpi->setPulseExtPolarity(pepFall);
			else if (sysData.pulse.polarity == pepFall)
				sysScpi->setPulseExtPolarity(pepRising);
			drawPulseModulate();
			break;
		//============================================ Aux 上变频 =======================================
		case KEY_FUNC_AUXFCTN_EXTRF_SOURCE:
			if (sysData.exfRef.type == erftInternal)
				sysScpi->setExtRefType(erftExternal);
			else if (sysData.exfRef.type == erftExternal)
				sysScpi->setExtRefType(erftInternal);
			drawExfFref();
			break;
		//============================================ Aux DRG =======================================
		case KEY_FUNC_AUXFCTN_DRG_STATE:
			sysScpi->setDrgState(!sysData.drg.state);
			drawDrgFunction();
			break;
		//========================================= Aux DigitalMod ===================================
		case KEY_FUNC_AUXFCTN_DIGITALMOD_STATE:
			tempState = !sysData.digital.digitalstate;	//closeDemodState()会将digitalstate设成false,所有先保存
			sysScpi->closeDemodState();
			sysScpi->setDigitalModuateState(tempState);
			drawDigitalModulate();
			break;
		case KEY_FUNC_AUXFCTN_DIGITALMOD_SEQUENCE:	//调制序列（调制源）
			if (sysData.digital.source == dstInternal)
				sysScpi->setDigitalModulateSource(dstExternal);
			else if (sysData.digital.source == dstExternal)
				sysScpi->setDigitalModulateSource(dstInternal);
			drawDigitalModulate();
			break;
		//========================================= Aux USB Power ===================================
		case KEY_FUNC_AUXFCTN_USBPOWER_STATE:	//状态
			retValue = sysScpi->setUSPPowerState(!sysData.usbPower.state);
			if (retValue == __SCPI_FAILED)
			{
				drawErrorMessage(getTheTranslatorString("Power Connected Failed"));
			}
			drawUSBPower();
			break;
		case KEY_FUNC_AUXFCTN_USBPOWER_AVERAGE_MODE:	//平均模式
			if (sysData.usbPower.AdvMode == upamtOff)
				sysScpi->setUSBPowerAdvMode(upamtAuto);
			else if (sysData.usbPower.AdvMode == upamtAuto)
				sysScpi->setUSBPowerAdvMode(upamtOff);
			drawUSBPower();
			break;
		case KEY_FUNC_AUXFCTN_USBPOWER_UNIT:	//测量单位
			if (sysData.usbPower.MeasUnit == upmutdBm)
				sysScpi->setUSBPowerMeasUnit(upmutW);
			else if(sysData.usbPower.MeasUnit == upmutW)
				sysScpi->setUSBPowerMeasUnit(upmutdBm);
			drawUSBPower();
		//========================================= System ===================================
		case KEY_FUNC_SYSTEM_SCPI:	//SCPI刷新屏幕
			sysScpi->setScpiState(!sysData.system.isScpiFresh);
			drawSystemPara();
			break;
		default:
			break;
	}
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
			}
			else if (sysData.sweep.list.selectRow < 0)
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
				}
				else if (sysData.sweep.list.data[sysData.sweep.list.selectRow].freq > MAXFREQ)
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
			else if (sysData.sweep.list.selectLine == 2) //如果为驻留时间
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
			drawInputParamter();
			drawsListData();
			break;

		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_EDITITEM_MHZMS:
			if (sysData.sweep.list.selectLine == 0) //如果为频率值
			{
				sysData.sweep.list.data[sysData.sweep.list.selectRow].freq = handleValue.toDouble() * 1e6;

				if (sysData.sweep.list.data[sysData.sweep.list.selectRow].freq < MINFREQ)
				{
					sysData.sweep.list.data[sysData.sweep.list.selectRow].freq = MINFREQ;
				}
				else if (sysData.sweep.list.data[sysData.sweep.list.selectRow].freq > MAXFREQ)
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
			else if (sysData.sweep.list.selectLine == 2) //如果为驻留时间
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
			drawInputParamter();
			drawsListData();
			break;

		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_EDITITEM_KHZNS:
			if (sysData.sweep.list.selectLine == 0) //如果为频率值
			{
				sysData.sweep.list.data[sysData.sweep.list.selectRow].freq = handleValue.toDouble() * 1e3;

				if (sysData.sweep.list.data[sysData.sweep.list.selectRow].freq < MINFREQ)
				{
					sysData.sweep.list.data[sysData.sweep.list.selectRow].freq = MINFREQ;
				}
				else if (sysData.sweep.list.data[sysData.sweep.list.selectRow].freq > MAXFREQ)
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
				}
				else if (sysData.sweep.list.data[sysData.sweep.list.selectRow].freq > MAXFREQ)
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
			else if (sysData.sweep.list.selectLine == 1) //如果为功率
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
			drawInputParamter();
			drawsListData();
			break;

		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_MORE_DOWNLOAD_ENTER:

			analysisListdata((int)handleValue.toDouble());
			//		if(system("umount /mnt/usb/") != 0)
			//		{
			//			return ;
			//		}
			drawsListData();
			break;

		case KEY_FUNC_AUXFCTN_DIGITALMOD_EXTLOAD_ENTER:
			analysisDigiSequencedata((int)handleValue.toDouble());
			sysScpi->setDigtalSequenceDownload();
			break;

		//case KEY_FUNC_FCTN_EXTRF_FREQ_GHZ:
		//	if (isOk)
		//	{
		//		//		sysData.EnterPara.CursorStatus = CLOSE;
		//		sysScpi->setExtRefFrequency(handleValue.toDouble() * 1e9);

		//		inputValue = "";
		//		sysData.inputLength = MAXFREQINPUTLENGTH;

		//		drawInputParamter();
		//		drawExfFref();
		//	}
		//	break;

		//case KEY_FUNC_FCTN_EXTRF_FREQ_MHZ:
		//	if (isOk)
		//	{
		//		//		sysData.EnterPara.CursorStatus = CLOSE;
		//		sysScpi->setExtRefFrequency(handleValue.toDouble() * 1e6);

		//		inputValue = "";

		//		sysData.inputLength = MAXFREQINPUTLENGTH;

		//		drawInputParamter();
		//		drawExfFref();
		//	}
		//	break;

		//case KEY_FUNC_FCTN_EXTRF_FREQ_KHZ:
		//	if (isOk)
		//	{
		//		//		sysData.EnterPara.CursorStatus = CLOSE;
		//		sysScpi->setExtRefFrequency(handleValue.toDouble() * 1e3);

		//		inputValue = "";

		//		sysData.inputLength = MAXFREQINPUTLENGTH;

		//		drawInputParamter();
		//		drawExfFref();
		//	}
		//	break;

		//case KEY_FUNC_FCTN_EXTRF_FREQ_HZ:
		//	if (isOk)
		//	{
		//		//		sysData.EnterPara.CursorStatus = CLOSE;
		//		sysScpi->setExtRefFrequency(handleValue.toDouble());

		//		inputValue = "";
		//		sysData.inputLength = MAXFREQINPUTLENGTH;

		//		drawInputParamter();
		//		drawExfFref();
		//	}
		//	break;

		case KEY_FUNC_FREQ_GHZ:
			if (isOk)
			{
				sysScpi->setFrequency(handleValue.toDouble() * 1e9);
				inputValue = "";
				sysData.inputLength = MAXFREQINPUTLENGTH;
				drawInputParamter();
				drawMainPara();
			}
			break;

		case KEY_FUNC_FREQ_MHZ:
			if (isOk)
			{
				//		sysData.EnterPara.CursorStatus = CLOSE;
				sysScpi->setFrequency(handleValue.toDouble() * 1e6);

				inputValue = "";
				sysData.inputLength = MAXFREQINPUTLENGTH;

				drawInputParamter();
				drawMainPara();
			}
			break;

		case KEY_FUNC_FREQ_KHZ:
			if (isOk)
			{
				//			sysData.EnterPara.CursorStatus = CLOSE;
				sysScpi->setFrequency(handleValue.toDouble() * 1e3);

				inputValue = "";
				sysData.inputLength = MAXFREQINPUTLENGTH;

				drawInputParamter();
				drawMainPara();
			}
			break;

		case KEY_FUNC_FREQ_HZ:
			if (isOk)
			{
				//		sysData.EnterPara.CursorStatus = CLOSE;
				sysScpi->setFrequency(handleValue.toDouble());

				inputValue = "";
				sysData.inputLength = MAXFREQINPUTLENGTH;

				drawInputParamter();
				drawMainPara();
			}
			break;

		case KEY_FUNC_FREQOFFSET_GHZ:
			if (isOk)
			{
				//		sysData.EnterPara.CursorStatus = CLOSE;
				sysScpi->setFrequencyOffset(handleValue.toDouble() * 1e9);

				inputValue = "";

				sysData.inputLength = MAXFREQINPUTLENGTH;

				drawMainPara();
				//		drawFreqAndAmpl();
				drawFreqInParamter();
				drawInputParamter();
			}
			break;

		case KEY_FUNC_FREQOFFSET_MHZ:
			if (isOk)
			{
				sysScpi->setFrequencyOffset(handleValue.toDouble() * 1e6);

				inputValue = "";

				sysData.inputLength = MAXFREQINPUTLENGTH;

				drawInputParamter();
				drawMainPara();
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

				drawInputParamter();
				drawMainPara();
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

				drawInputParamter();
				drawMainPara();
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

				drawInputParamter();
				drawMainPara();
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

				drawInputParamter();
				drawMainPara();
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

				drawInputParamter();
				drawMainPara();
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
				drawInputParamter();
				drawMainPara();
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
			//		drawInputParamter();
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

				drawInputParamter();
				drawMainPara();
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

				drawInputParamter();
				drawMainPara();
				drawAmplInParamter();
			}
			break;
//============================== 增量设置 ============================================
		case KEY_FUNC_INCRSETSET_GHZ:
			if (isOk)
			{
				if (sysData.incrSet.type == itdFreq)
				{
					sysScpi->setFreqIncr(handleValue.toDouble() * 1e9);
					inputValue = "";
					sysData.inputLength = MAXAMPLINPUTLENGTH;
					drawInputParamter();
					drawMainPara();
					drawFreqInParamter();
				}
				else if (sysData.incrSet.type == itdAmptd)
				{
					sysScpi->setAmplIncr(handleValue.toDouble());
					inputValue = "";
					sysData.inputLength = MAXAMPLINPUTLENGTH;
					drawInputParamter();
					drawMainPara();
					drawAmplInParamter();
				}
			}
			break;
		case KEY_FUNC_INCRSETSET_MHZ:
			if (isOk)
			{
				if (sysData.incrSet.type == itdFreq)
				{
					sysScpi->setFreqIncr(handleValue.toDouble() * 1e6);
					inputValue = "";
					sysData.inputLength = MAXAMPLINPUTLENGTH;
					drawInputParamter();
					drawMainPara();
					drawFreqInParamter();
				}
			}
			break;
		case KEY_FUNC_INCRSETSET_KHZ:
			if (isOk)
			{
				if (sysData.incrSet.type == itdFreq)
				{
					sysScpi->setFreqIncr(handleValue.toDouble() * 1e3);
					inputValue = "";
					sysData.inputLength = MAXAMPLINPUTLENGTH;
					drawInputParamter();
					drawMainPara();
					drawFreqInParamter();
				}
			}
			break;
		case KEY_FUNC_INCRSETSET_HZ:
			if (isOk)
			{
				if (sysData.incrSet.type == itdFreq)
				{
					sysScpi->setFreqIncr(handleValue.toDouble());
					inputValue = "";
					sysData.inputLength = MAXAMPLINPUTLENGTH;
					drawInputParamter();
					drawMainPara();
					drawFreqInParamter();
				}
			}
			break;
//-----------------------------------------------------------------------------------
		case KEY_FUNC_SWEEP_STEP_MORE_STEPDWELL_MS:
			if (isOk)
			{
				sysScpi->setStepDwell(handleValue.toDouble());
				inputValue = "";
				sysData.inputLength = MAXTIMEINPUTLENGTH;

				drawInputParamter();
				drawSweep(sysData.sweep.type);
			}
			break;

		case KEY_FUNC_SWEEP_STEP_MORE_STEPDWELL_S:
			if (isOk)
			{
				sysScpi->setStepDwell(handleValue.toDouble() * 1e3);

				inputValue = "";
				sysData.inputLength = MAXTIMEINPUTLENGTH;

				drawInputParamter();
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

				drawInputParamter();
				drawSweep(sysData.sweep.type);
			}
			break;

		case KEY_FUNC_SWEEP_STEP_FREQSTART_MHZ:
			if (isOk)
			{
				sysScpi->setStepStartFrequency(handleValue.toDouble() * 1e6);

				sysData.inputLength = MAXFREQINPUTLENGTH;
				inputValue = "";

				drawInputParamter();
				drawSweep(sysData.sweep.type);
			}
			break;

		case KEY_FUNC_SWEEP_STEP_FREQSTART_KHZ:
			if (isOk)
			{
				sysScpi->setStepStartFrequency(handleValue.toDouble() * 1e3);
				sysData.inputLength = MAXFREQINPUTLENGTH;
				inputValue = "";
				drawInputParamter();
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

				drawInputParamter();
				drawSweep(sysData.sweep.type);
			}
			break;

		case KEY_FUNC_SWEEP_STEP_FREQSTOP_MHZ:
			if (isOk)
			{
				sysScpi->setStepStopFrequency(handleValue.toDouble() * 1e6);
				sysData.inputLength = MAXFREQINPUTLENGTH;
				inputValue = "";

				drawInputParamter();
				drawSweep(sysData.sweep.type);
			}
			break;

		case KEY_FUNC_SWEEP_STEP_FREQSTOP_KHZ:
			if (isOk)
			{
				sysScpi->setStepStopFrequency(handleValue.toDouble() * 1e3);
				sysData.inputLength = MAXFREQINPUTLENGTH;
				inputValue = "";

				drawInputParamter();
				drawSweep(sysData.sweep.type);
			}
			break;

		case KEY_FUNC_SWEEP_STEP_FREQSTOP_HZ:
			if (isOk)
			{
				sysScpi->setStepStopFrequency(handleValue.toDouble());

				sysData.inputLength = MAXFREQINPUTLENGTH;
				inputValue = "";

				drawInputParamter();
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
				drawInputParamter();

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

				drawInputParamter();

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

				drawInputParamter();

				drawSweep(sysData.sweep.type);
			}
			break;
//============================================================= AM ===========================================
		case KEY_FUNC_AM_DEPTH_PERCENT:		//AM调制深度
			if (isOk)
			{
				sysScpi->setAmDepth(handleValue.toDouble());
				sysData.inputLength = MAXINPUTLENGTH;
				inputValue = "";
				drawInputParamter();
				drawAM();
			}
			break;
		case KEY_FUNC_AM_INTERNALFCTN_FREQ_MHZ:	//AM调制速率
			if (isOk)
			{
				sysScpi->setAmRate(handleValue.toDouble() * 1e6);
				sysData.inputLength = MAXINPUTLENGTH;
				inputValue = "";
				drawInputParamter();
				drawAM();
			}
			break;
		case KEY_FUNC_AM_INTERNALFCTN_FREQ_KHZ:	//AM调制速率
			if (isOk)
			{
				sysScpi->setAmRate(handleValue.toDouble() * 1e3);
				sysData.inputLength = MAXINPUTLENGTH;
				inputValue = "";
				drawInputParamter();
				drawAM();
			}
			break;
		case KEY_FUNC_AM_INTERNALFCTN_FREQ_HZ:	//AM调制速率
			if (isOk)
			{
				sysScpi->setAmRate(handleValue.toDouble());
				sysData.inputLength = MAXINPUTLENGTH;
				inputValue = "";
				drawInputParamter();
				drawAM();
			}
			break;
//============================================================= FMPM ===========================================
		case KEY_FUNC_FMPM_OFFSET_MHZ:	//调制偏移
			if (sysData.fmpm.mode == fdtFM)
			{
				if (isOk)
				{
					sysScpi->setFmDev(handleValue.toDouble() * 1e6);
					sysData.inputLength = MAXFREQINPUTLENGTH;
					inputValue = "";
					drawInputParamter();
					drawFMPM();
				}
			}
			else if (sysData.fmpm.mode == fdtPM)
			{
				if (isOk)
				{
					sysScpi->setPmPhase(handleValue.toDouble());
					sysData.inputLength = MAXINPUTLENGTH;
					inputValue = "";
					drawInputParamter();
					drawFMPM();
				}
			}
			break;
		case KEY_FUNC_FMPM_OFFSET_KHZ:	//调制偏移
			if (sysData.fmpm.mode == fdtFM)
			{
				if (isOk)
				{
					sysScpi->setFmDev(handleValue.toDouble() * 1e3);
					sysData.inputLength = MAXFREQINPUTLENGTH;
					inputValue = "";
					drawInputParamter();
					drawFMPM();
				}
			}
			break;
		case KEY_FUNC_FMPM_OFFSET_HZ:
			if (sysData.fmpm.mode == fdtFM)
			{
				if (isOk)
				{
					sysScpi->setFmDev(handleValue.toDouble());
					sysData.inputLength = MAXFREQINPUTLENGTH;
					inputValue = "";
					drawInputParamter();
					drawFMPM();
				}
			}
			break;
		case KEY_FUNC_FMPM_INTERNALFCTN_FREQ_MHZ:	//调制速率
			if (sysData.fmpm.mode == fdtFM)
			{
				if (isOk)
				{
					sysScpi->setFmRate(handleValue.toDouble() * 1e6);
					sysData.inputLength = MAXFREQINPUTLENGTH;
					inputValue = "";
					drawInputParamter();
					drawFMPM();
				}
			}
			else if (sysData.fmpm.mode == fdtPM)
			{
				if (isOk)
				{
					sysScpi->setPmRate(handleValue.toDouble() * 1e6);
					sysData.inputLength = MAXFREQINPUTLENGTH;
					inputValue = "";
					drawInputParamter();
					drawFMPM();
				}
			}
			break;
		case KEY_FUNC_FMPM_INTERNALFCTN_FREQ_KHZ:	//调制速率
			if (sysData.fmpm.mode == fdtFM)
			{
				if (isOk)
				{
					sysScpi->setFmRate(handleValue.toDouble() * 1e3);
					sysData.inputLength = MAXFREQINPUTLENGTH;
					inputValue = "";
					drawInputParamter();
					drawFMPM();
				}
			}
			else if (sysData.fmpm.mode == fdtPM)
			{
				if (isOk)
				{
					sysScpi->setPmRate(handleValue.toDouble() * 1e3);
					sysData.inputLength = MAXFREQINPUTLENGTH;
					inputValue = "";
					drawInputParamter();
					drawFMPM();
				}
			}
			break;
		case KEY_FUNC_FMPM_INTERNALFCTN_FREQ_HZ:	//调制速率
			if (sysData.fmpm.mode == fdtFM)
			{
				if (isOk)
				{
					sysScpi->setFmRate(handleValue.toDouble());
					sysData.inputLength = MAXFREQINPUTLENGTH;
					inputValue = "";
					drawInputParamter();
					drawFMPM();
				}
			}
			else if (sysData.fmpm.mode == fdtPM)
			{
				if (isOk)
				{
					sysScpi->setPmRate(handleValue.toDouble());
					sysData.inputLength = MAXFREQINPUTLENGTH;
					inputValue = "";
					drawInputParamter();
					drawFMPM();
				}
			}
			break;
//================================================== AUX 上变频 =============================================
		case KEY_FUNC_AUXFCTN_EXTRF_EXTIF_MHZ:
			if (isOk)
			{
				sysScpi->setExtRefFrequency(handleValue.toDouble() * 1e6);
				inputValue = "";
				sysData.inputLength = MAXFREQINPUTLENGTH;
				drawInputParamter();
				drawExfFref();
			}
			break;
		case KEY_FUNC_AUXFCTN_EXTRF_EXTIF_KHZ:
			if (isOk)
			{
				sysScpi->setExtRefFrequency(handleValue.toDouble() * 1e3);
				inputValue = "";
				sysData.inputLength = MAXFREQINPUTLENGTH;
				drawInputParamter();
				drawExfFref();
			}
			break;
		case KEY_FUNC_AUXFCTN_EXTRF_EXTIF_HZ:
			if (isOk)
			{
				sysScpi->setExtRefFrequency(handleValue.toDouble());
				inputValue = "";
				sysData.inputLength = MAXFREQINPUTLENGTH;
				drawInputParamter();
				drawExfFref();
			}
			break;

			//-------------------------------------------------------------------------------------------
		//case KEY_FUNC_FCTN_ANALOG_AM_RATE_KHZ:
		//	if (isOk)
		//	{
		//		sysScpi->setAmRate(handleValue.toDouble() * 1e3);

		//		sysData.inputLength = MAXFREQINPUTLENGTH;

		//		inputValue = "";

		//		drawInputParamter();
		//		drawAnalogModulate();
		//	}
		//	break;

		//	//-------------------------------------------------------------------------------------------
		//case KEY_FUNC_FCTN_ANALOG_AM_RATE_HZ:
		//	if (isOk)
		//	{
		//		sysScpi->setAmRate(handleValue.toDouble());
		//		sysData.inputLength = MAXFREQINPUTLENGTH;

		//		inputValue = "";

		//		drawInputParamter();
		//		drawAnalogModulate();
		//	}
		//	break;

			//-------------------------------------------------------------------------------------------
		//case KEY_FUNC_FCTN_ANALOG_FM_RATE_KHZ:
		//	if (isOk)
		//	{
		//		//		sysData.EnterPara.CursorStatus = CLOSE;
		//		sysScpi->setFmRate(handleValue.toDouble() * 1e3);
		//		sysData.inputLength = MAXFREQINPUTLENGTH;

		//		inputValue = "";

		//		drawInputParamter();
		//		drawAnalogModulate();
		//	}

		//	break;

		//	//-------------------------------------------------------------------------------------------
		//case KEY_FUNC_FCTN_ANALOG_FM_RATE_HZ:
		//	if (isOk)
		//	{
		//		sysScpi->setFmRate(handleValue.toDouble());

		//		sysData.inputLength = MAXFREQINPUTLENGTH;

		//		inputValue = "";

		//		drawInputParamter();
		//		drawAnalogModulate();
		//	}
		//	break;

		//	//-------------------------------------------------------------------------------------------
		//case KEY_FUNC_FCTN_ANALOG_PM_RATE_KHZ:
		//	if (isOk)
		//	{
		//		//		sysData.EnterPara.CursorStatus = CLOSE;
		//		sysScpi->setPmRate(handleValue.toDouble() * 1e3);
		//		sysData.inputLength = MAXFREQINPUTLENGTH;

		//		inputValue = "";

		//		drawInputParamter();
		//		drawAnalogModulate();
		//	}
		//	break;

		//	//-------------------------------------------------------------------------------------------
		//case KEY_FUNC_FCTN_ANALOG_PM_RATE_HZ:
		//	if (isOk)
		//	{
		//		sysScpi->setPmRate(handleValue.toDouble());
		//		sysData.inputLength = MAXFREQINPUTLENGTH;

		//		inputValue = "";

		//		drawInputParamter();
		//		drawAnalogModulate();
		//	}
		//	break;
			//-------------------------------------------------------------------------------------------
//================================================== AUX 数字调制 =============================================
		case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA_DEPTH_PERCENT:
			if (isOk)
			{
				sysScpi->setDigitalModulatePara(handleValue.toDouble());
				sysData.inputLength = MAXFREQINPUTLENGTH;
				inputValue = "";
				drawInputParamter();
				drawDigitalModulate();
			}
			break;
		case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA_DEVI_MHZ:
			if (isOk)
			{
				sysScpi->setDigitalModulatePara(handleValue.toDouble() * 1e6);
				sysData.inputLength = MAXFREQINPUTLENGTH;
				inputValue = "";
				drawInputParamter();
				drawDigitalModulate();
			}
			break;
		case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA_DEVI_KHZ:
			if (isOk)
			{
				sysScpi->setDigitalModulatePara(handleValue.toDouble() * 1e3);
				sysData.inputLength = MAXFREQINPUTLENGTH;
				inputValue = "";
				drawInputParamter();
				drawDigitalModulate();
			}
			break;
		case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA_DEVI_HZ:
			if (isOk)
			{
				sysScpi->setDigitalModulatePara(handleValue.toDouble());
				sysData.inputLength = MAXFREQINPUTLENGTH;
				inputValue = "";
				drawInputParamter();
				drawDigitalModulate();
			}
			break;
		case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA_PHASE_DEG:
			if (isOk)
			{
				sysScpi->setDigitalModulatePara(handleValue.toDouble());
				sysData.inputLength = MAXFREQINPUTLENGTH;
				inputValue = "";
				drawInputParamter();
				drawDigitalModulate();
			}
			break;
		case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA_RATE_MHZ:
			if (isOk)
			{
				sysScpi->setDigitalModuateRate(handleValue.toDouble() * 1e6);
				sysData.inputLength = MAXFREQINPUTLENGTH;
				inputValue = "";
				drawInputParamter();
				drawDigitalModulate();
			}
			break;
		case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA_RATE_KHZ:
			if (isOk)
			{
				sysScpi->setDigitalModuateRate(handleValue.toDouble() * 1e3);
				sysData.inputLength = MAXFREQINPUTLENGTH;
				inputValue = "";
				drawInputParamter();
				drawDigitalModulate();
			}
			break;
		case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA_RATE_HZ:
			if (isOk)
			{
				sysScpi->setDigitalModuateRate(handleValue.toDouble());
				sysData.inputLength = MAXFREQINPUTLENGTH;
				inputValue = "";
				drawInputParamter();
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
			// 	    drawInputParamter();
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
			// 	    drawInputParamter();
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
			// 	    drawInputParamter();
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
			// 	    drawInputParamter();
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
			// 	    drawInputParamter();
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
			// 	    drawInputParamter();
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
			// 	    drawInputParamter();
			//		drawDigitalModulate();
			//
			//	  }
			//		  break;

			//-------------------------------------------------------------------------------------------
		/*case KEY_FUNC_FCTN_ANALOG_AM_DEPTH_PERCENT:
			if (isOk)
			{
				sysScpi->setAmDepth(handleValue.toDouble());
				sysData.inputLength = MAXINPUTLENGTH;
				inputValue = "";

				drawInputParamter();
				drawAnalogModulate();
			}
			break;*/

			//-------------------------------------------------------------------------------------------
		//case KEY_FUNC_FCTN_ANALOG_FM_DEVIATION_MHZ:
		//	if (isOk)
		//	{
		//		sysScpi->setFmDev(handleValue.toDouble() * 1e6);
		//		sysData.inputLength = MAXFREQINPUTLENGTH;

		//		inputValue = "";

		//		drawInputParamter();
		//		drawAnalogModulate();
		//	}
		//	break;

		//	//-------------------------------------------------------------------------------------------
		//case KEY_FUNC_FCTN_ANALOG_FM_DEVIATION_KHZ:
		//	if (isOk)
		//	{
		//		//		sysData.EnterPara.CursorStatus = CLOSE;
		//		sysScpi->setFmDev(handleValue.toDouble() * 1e3);

		//		sysData.inputLength = MAXFREQINPUTLENGTH;

		//		inputValue = "";

		//		drawInputParamter();
		//		drawAnalogModulate();
		//	}
		//	break;

		//	//-------------------------------------------------------------------------------------------
		//case KEY_FUNC_FCTN_ANALOG_FM_DEVIATION_HZ:
		//	if (isOk)
		//	{
		//		//		sysData.EnterPara.CursorStatus = CLOSE;
		//		sysScpi->setFmDev(handleValue.toDouble());

		//		sysData.inputLength = MAXFREQINPUTLENGTH;

		//		inputValue = "";

		//		drawInputParamter();
		//		drawAnalogModulate();
		//	}
		//	break;

		//	//-------------------------------------------------------------------------------------------
		//case KEY_FUNC_FCTN_ANALOG_PM_PHASE_ANGLE:
		//	if (isOk)
		//	{
		//		//		sysData.EnterPara.CursorStatus = CLOSE;
		//		sysScpi->setPmPhase(handleValue.toDouble());
		//		sysData.inputLength = MAXINPUTLENGTH;
		//		inputValue = "";

		//		drawInputParamter();
		//		drawAnalogModulate();
		//	}
		//	break;
//======================================================  Pulse ==========================================
		case KEY_FUNC_PULSE_PERIOD_S:
			if (isOk)
			{
				sysScpi->setPulsePeriod(handleValue.toDouble() * 1e9);
				inputValue = "";
				sysData.inputLength = MAXAMDEPTH;
				drawInputParamter();
				drawPulseModulate();
			}
			break;
		case KEY_FUNC_PULSE_PERIOD_MS:
			if (isOk)
			{
				sysScpi->setPulsePeriod(handleValue.toDouble() * 1e6);
				inputValue = "";
				sysData.inputLength = MAXAMDEPTH;
				drawInputParamter();
				drawPulseModulate();
			}
			break;
		case KEY_FUNC_PULSE_PERIOD_US:
			if (isOk)
			{
				sysScpi->setPulsePeriod(handleValue.toDouble() * 1e3);
				inputValue = "";
				sysData.inputLength = MAXAMDEPTH;
				drawInputParamter();
				drawPulseModulate();
			}
			break;
		case KEY_FUNC_PULSE_PERIOD_NS:
			if (isOk)
			{
				sysScpi->setPulsePeriod(handleValue.toDouble());
				inputValue = "";
				sysData.inputLength = MAXAMDEPTH;
				drawInputParamter();
				drawPulseModulate();
			}
			break;
		case KEY_FUNC_PULSE_WIDTH_S:
			if (isOk)
			{
				sysScpi->setPulseWidth(handleValue.toDouble() * 1e9);
				inputValue = "";
				sysData.inputLength = MAXAMDEPTH;
				drawInputParamter();
				drawPulseModulate();
			}
			break;
		case KEY_FUNC_PULSE_WIDTH_MS:
			if (isOk)
			{
				sysScpi->setPulseWidth(handleValue.toDouble() * 1e6);
				inputValue = "";
				sysData.inputLength = MAXAMDEPTH;
				drawInputParamter();
				drawPulseModulate();
			}
			break;
		case KEY_FUNC_PULSE_WIDTH_US:
			if (isOk)
			{
				sysScpi->setPulseWidth(handleValue.toDouble() * 1e3);
				inputValue = "";
				sysData.inputLength = MAXAMDEPTH;
				drawInputParamter();
				drawPulseModulate();
			}
			break;
		case KEY_FUNC_PULSE_WIDTH_NS:
			if (isOk)
			{
				sysScpi->setPulseWidth(handleValue.toDouble());
				inputValue = "";
				sysData.inputLength = MAXAMDEPTH;
				drawInputParamter();
				drawPulseModulate();
			}
			break;
		case KEY_FUNC_PULSE_DELAY_S:
			if (isOk)
			{
				sysScpi->setPulseDelay(handleValue.toDouble() * 1e9);
				inputValue = "";
				sysData.inputLength = MAXAMDEPTH;
				drawInputParamter();
				drawPulseModulate();
			}
			break;
		case KEY_FUNC_PULSE_DELAY_MS:
			if (isOk)
			{
				sysScpi->setPulseDelay(handleValue.toDouble() * 1e6);
				inputValue = "";
				sysData.inputLength = MAXAMDEPTH;
				drawInputParamter();
				drawPulseModulate();
			}
			break;
		case KEY_FUNC_PULSE_DELAY_US:
			if (isOk)
			{
				sysScpi->setPulseDelay(handleValue.toDouble() * 1e3);
				inputValue = "";
				sysData.inputLength = MAXAMDEPTH;
				drawInputParamter();
				drawPulseModulate();
			}
			break;
		case KEY_FUNC_PULSE_DELAY_NS:
			if (isOk)
			{
				sysScpi->setPulseDelay(handleValue.toDouble());
				inputValue = "";
				sysData.inputLength = MAXAMDEPTH;
				drawInputParamter();
				drawPulseModulate();
			}
			break;
//============================================== AUX LF =========================================
		case KEY_FUNC_AUXFCTN_LFOUT_AMPL_V:
			if (isOk)
			{
				sysScpi->setLfAmpl(handleValue.toDouble() * 1e3);
				inputValue = "";
				sysData.inputLength = MAXLFINPUTLENGTH;
				drawInputParamter();
				drawArbitraryWave();
			}
			break;
		case KEY_FUNC_AUXFCTN_LFOUT_AMPL_MV:
			if (isOk)
			{
				sysScpi->setLfAmpl(handleValue.toDouble());
				inputValue = "";
				sysData.inputLength = MAXLFINPUTLENGTH;
				drawInputParamter();
				drawArbitraryWave();
			}
			break;
		case KEY_FUNC_AUXFCTN_LFOUT_FREQ_KHZ:
			if (isOk)
			{
				sysScpi->setLfFreq(handleValue.toDouble() * 1000);

				inputValue = "";

				sysData.inputLength = MAXFREQINPUTLENGTH;
				drawInputParamter();
				drawArbitraryWave();
			}
			break;

		case KEY_FUNC_AUXFCTN_LFOUT_FREQ_HZ:
			if (isOk)
			{
				sysScpi->setLfFreq(handleValue.toDouble());

				inputValue = "";
				sysData.inputLength = MAXFREQINPUTLENGTH;

				drawInputParamter();
				drawArbitraryWave();
			}
			break;
//============================================== AUX DRG =========================================
		case KEY_FUNC_AUXFCTN_DRG_UPPER_GHZ:	//扫描上限
			if (isOk)
			{
				sysScpi->setDrgUplimit(handleValue.toDouble() * 1e9);
				inputValue = "";
				sysData.inputLength = MAXDRGFREQINPUTLENGTH;
				drawInputParamter();
				drawDrgFunction();
			}
			break;
		case KEY_FUNC_AUXFCTN_DRG_UPPER_MHZ:
			if (isOk)
			{
				sysScpi->setDrgUplimit(handleValue.toDouble() * 1e6);
				inputValue = "";
				sysData.inputLength = MAXDRGFREQINPUTLENGTH;
				drawInputParamter();
				drawDrgFunction();
			}
			break;
		case KEY_FUNC_AUXFCTN_DRG_UPPER_KHZ:
			if (isOk)
			{
				sysScpi->setDrgUplimit(handleValue.toDouble() * 1e3);
				inputValue = "";
				sysData.inputLength = MAXDRGFREQINPUTLENGTH;
				drawInputParamter();
				drawDrgFunction();
			}
			break;
		case KEY_FUNC_AUXFCTN_DRG_UPPER_HZ:
			if (isOk)
			{
				sysScpi->setDrgUplimit(handleValue.toDouble());
				inputValue = "";
				sysData.inputLength = MAXDRGFREQINPUTLENGTH;
				drawInputParamter();
				drawDrgFunction();
			}
			break;
		case KEY_FUNC_AUXFCTN_DRG_LOW_GHZ:	//扫描下限
			if (isOk)
			{
				sysScpi->setDrgDownlimit(handleValue.toDouble() * 1e9);
				inputValue = "";
				sysData.inputLength = MAXDRGFREQINPUTLENGTH;
				drawInputParamter();
				drawDrgFunction();
			}
			break;
		case KEY_FUNC_AUXFCTN_DRG_LOW_MHZ:
			if (isOk)
			{
				sysScpi->setDrgDownlimit(handleValue.toDouble() * 1e6);
				inputValue = "";
				sysData.inputLength = MAXDRGFREQINPUTLENGTH;
				drawInputParamter();
				drawDrgFunction();
			}
			break;
		case KEY_FUNC_AUXFCTN_DRG_LOW_KHZ:
			if (isOk)
			{
				sysScpi->setDrgDownlimit(handleValue.toDouble() * 1e3);
				inputValue = "";
				sysData.inputLength = MAXDRGFREQINPUTLENGTH;
				drawInputParamter();
				drawDrgFunction();
			}
			break;
		case KEY_FUNC_AUXFCTN_DRG_LOW_HZ:
			if (isOk)
			{
				sysScpi->setDrgDownlimit(handleValue.toDouble());
				inputValue = "";
				sysData.inputLength = MAXDRGFREQINPUTLENGTH;
				drawInputParamter();
				drawDrgFunction();
			}
			break;
		case KEY_FUNC_AUXFCTN_DRG_POINTS_ENTER:		//扫描点数
			if (isOk)
			{
				sysScpi->setDrgPoints(handleValue.toDouble());
				inputValue = "";
				sysData.inputLength = 5;
				drawInputParamter();
				drawDrgFunction();
			}
			break;
		case KEY_FUNC_AUXFCTN_DRG_RATE_US:		//扫描速率
			if (isOk)
			{
				sysScpi->setDrgRate(handleValue.toDouble() * 1e3);
				inputValue = "";
				sysData.inputLength = MAXDRGTIMEINPUTLENGTH;
				drawInputParamter();
				drawDrgFunction();
			}
			break;
		case KEY_FUNC_AUXFCTN_DRG_RATE_NS:
			if (isOk)
			{
				sysScpi->setDrgRate(handleValue.toDouble());
				inputValue = "";
				sysData.inputLength = MAXDRGTIMEINPUTLENGTH;
				drawInputParamter();
				drawDrgFunction();
			}
			break;
//============================================ AUX USB Power ======================================
		case KEY_FUNC_AUXFCTN_USBPOWER_CHFREQ_GHZ:	//通道频率
			if (isOk)
			{
				sysScpi->setUSBPowerFreq(handleValue.toDouble() * 1e9);
				inputValue = "";
				sysData.inputLength = MAXDRGTIMEINPUTLENGTH;
				drawUSBPower();
			}
			break;
		case KEY_FUNC_AUXFCTN_USBPOWER_CHFREQ_MHZ:
			if (isOk)
			{
				sysScpi->setUSBPowerFreq(handleValue.toDouble() * 1e6);
				inputValue = "";
				sysData.inputLength = MAXDRGTIMEINPUTLENGTH;
				drawUSBPower();
			}
			break;
		case KEY_FUNC_AUXFCTN_USBPOWER_CHFREQ_KHZ:
			if (isOk)
			{
				sysScpi->setUSBPowerFreq(handleValue.toDouble() * 1e3);
				inputValue = "";
				sysData.inputLength = MAXDRGTIMEINPUTLENGTH;
				drawUSBPower();
			}
			break;
		case KEY_FUNC_AUXFCTN_USBPOWER_CHFREQ_HZ:
			if (isOk)
			{
				sysScpi->setUSBPowerFreq(handleValue.toDouble());
				inputValue = "";
				sysData.inputLength = MAXDRGTIMEINPUTLENGTH;
				drawUSBPower();
			}
			break;
		case KEY_FUNC_AUXFCTN_USBPOWER_AMPLOFS_DB:	//功率偏移
			if (isOk)
			{
				sysScpi->setUSBPowerAmplOffset(handleValue.toDouble());
				inputValue = "";
				sysData.inputLength = MAXDRGTIMEINPUTLENGTH;
				drawUSBPower();
			}
			break;
		case KEY_FUNC_AUXFCTN_USBPOWER_AVERAGE_COUNT_ENTER:	//平均次数
			if (isOk)
			{
				sysScpi->setUSBPowerAdvCount(handleValue.toDouble());
				inputValue = "";
				sysData.inputLength = MAXDRGTIMEINPUTLENGTH;
				drawUSBPower();
			}
			break;
//============================================== System =========================================
		case KEY_FUNC_SYSTEM_LAN_IP_ENTER:
			if (!QHostAddress(handleValue).isNull())
			{
				sysScpi->setLanIP(handleValue);
				inputValue = "";
				sysData.inputLength = 15;
				drawInputParamter();
				drawSystemPara();
			} else
			{
				outputMessage(getTheTranslatorString(QString("Address")) + " " + handleValue + " " + getTheTranslatorString(QString("is invalid")));
			}
			break;
		case KEY_FUNC_SYSTEM_LAN_MASK_ENTER:
			if (!QHostAddress(handleValue).isNull())
			{
				sysScpi->setLanMask(handleValue);
				inputValue = "";
				sysData.inputLength = 15;
				drawInputParamter();
				drawSystemPara();
			} else
			{
				outputMessage(getTheTranslatorString(QString("address")) + " " + handleValue + " " + getTheTranslatorString(QString("is invalid")));
			}
			break;
		case KEY_FUNC_SYSTEM_LAN_GATEWAY_ENTER:
			if (!QHostAddress(handleValue).isNull())
			{
				sysScpi->setLanGate(handleValue);
				inputValue = "";
				sysData.inputLength = 15;
				drawInputParamter();
				drawSystemPara();
			} else
			{
				outputMessage(getTheTranslatorString(QString("address")) + " " + handleValue + " " + getTheTranslatorString(QString("is invalid")));
			}
			break;
		//case KEY_FUNC_AUX_SERVICE_LANMACADDR_ENTER:
		//	if (isOk)
		//	{
		//		sysScpi->setSystemServiceOfLanMacAddress(handleValue);
		//	}
		//	break;

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
	drawMainPara();
	drawState();
	drawInputParamter();
	//  drawFreqAndAmpl();
	drawFreqInParamter();
	//  drawAmplInParamter();
	drawButton();

}

