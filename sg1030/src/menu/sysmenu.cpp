#include "sysmenu.h"
#include "../scpi/sysscpi.h"
//构造
tSysMenu::tSysMenu(QString xmlFileName)
{
	//解析XML文件
	loadFromXmlFile(xmlFileName);

	//键值和功能码对应
	getSystemKeyAndFuncValue();
}

//析构
tSysMenu::~tSysMenu()
{

}

//上方向键
bool tSysMenu::isUpArrow(int value)
{
	return value == KEY_ARROW_UP;
}

//右脉冲旋钮
bool tSysMenu::isRightRpg(int value)
{

	return 0;
}

//获取脉冲数
int tSysMenu::getRpgPulse(int value, int value2)
{
	if (value == 1)
		return value2 * 1;
	else if (value == 2)
		return value2 * (-1);
}

//按下的是确认键
bool tSysMenu::pressedIsEnterKey(int value)
{
	return value == KEY_RPG_ENTER;
}

//按下的是方向键
bool tSysMenu::pressedIsArrowKey(int value)
{
	for (int i = 0; i < sizeof ARROWKEYLIST / sizeof ARROWKEYLIST[0]; i++)
		if (ARROWKEYLIST[i] == value)
		{
			return true;
		}

	return false;
}

//按下的是脉冲旋钮键
bool tSysMenu::pressedIsRpgKey(int value)
{
	return (value == 0x02);
}

//按下的是主键
bool tSysMenu::pressedIsMainKey(int value)
{
	for (int i = 0; i < sizeof MAINKEYLIST / sizeof MAINKEYLIST[0]; i++)
		if (MAINKEYLIST[i] == value)
		{
			return true;
		}

	return false;
}

//按下的是通用键
bool tSysMenu::pressedIsCommonKey(int value)
{
	for (int i = 0; i < sizeof COMMONKEYLIST / sizeof COMMONKEYLIST[0]; i++)
		if (COMMONKEYLIST[i] == value)
		{
			return true;
		}

	return false;
}

//按下的是数字键
bool tSysMenu::pressedIsNumberKey(int value)
{
	for (int i = 0; i < sizeof NUMBERKEYLIST / sizeof NUMBERKEYLIST[0]; i++)
		if (NUMBERKEYLIST[i] == value)
		{
			return true;
		}

	return false;
}

//按下的是回滚键
bool tSysMenu::pressedIsBkspaceKey(int value)
{
	return value == KEY_NUM_BKSPACE;
}

//按下的是取消键
bool tSysMenu::pressedIsEscKey(int value)
{
	return value == KEY_NUM_ESC;
}

//按下的是单位键
bool tSysMenu::pressedIsUnitKey(int value)
{
	return false;
}

//获取主键指针
void tSysMenu::getMainKeyPointer(int value)
{
	nowMainKey = sysKey;

	while (nowMainKey != NULL)
	{
		if (nowMainKey->keyValue == value)
		{
			if (nowMainKey->rightNode != NULL)
			{
				nowSubKey = nowMainKey->rightNode;
			}
			return;
		}
		nowMainKey = nowMainKey->downNode;
	}

	nowMainKey = NULL;
	nowSubKey = NULL;
}

//获取子键指针
bool tSysMenu::getSubKeyPointer(int value)
{
	if (nowMainKey == NULL)
	{
		nowSubKey = NULL;
		return false;
	}

	subKeyDef* subKey = nowSubKey;

	if (nowSubKey == NULL)
	{
		nowSubKey = nowMainKey->rightNode;

		while (nowSubKey != NULL)
		{
			if (nowSubKey->keyValue == value)
			{
				return true;
			}

			nowSubKey = nowSubKey->downNode;
		}

		nowSubKey = subKey;
	} else
	{
		if (nowSubKey->keyValue == value)
		{
			return true;
		}

		subKey = nowSubKey;
		while (subKey != NULL)
		{
			if (subKey->upNode == NULL)
			{
				break;
			}

			subKey = subKey->upNode;
		}

		while (subKey != NULL)
		{
			if (subKey->keyValue == value)
			{
				nowSubKey = subKey;
				return true;
			}

			if (subKey->downNode == NULL)
			{
				break;
			}

			subKey = subKey->downNode;
		}
	}

	return false;
}

//返回上级
void tSysMenu::gotoUpLevel(void)
{
	while (nowSubKey != NULL)
	{
		if (nowSubKey->upNode == NULL)
		{
			break;
		}

		nowSubKey = nowSubKey->upNode;
	}

	if (nowSubKey->leftNode != NULL)
	{
		nowSubKey = nowSubKey->leftNode;
	}
}

//进入下级
//原本处理方式
#if 0
void tSysMenu::gotoDownLevel(void)
{
	if ((nowSubKey != NULL) && (nowSubKey->rightNode != NULL))
	{
		unsigned long int funcCode = 0;

		//如果需要对应显示，例如选择ASK4模式，退出后再次进入模式选择时，光标会显示在ASK4上，不会在返回键上
		/*  zpc changed
		 if(nowSubKey->funcCode == KEY_FUNC_FCTN_DIGITAL_ASK_MODE)
		 {
		 switch(sysData.digital.ask.mode)
		 {
		 case askm2: funcCode = KEY_FUNC_FCTN_DIGITAL_ASK_MODE_ASK2; break;
		 case askm4: funcCode = KEY_FUNC_FCTN_DIGITAL_ASK_MODE_ASK4; break;
		 case askm8: funcCode = KEY_FUNC_FCTN_DIGITAL_ASK_MODE_ASK8; break;
		 }

		 subKeyDef* tempSubKey = nowSubKey->rightNode;

		 while (tempSubKey != NULL)
		 {
		 if (tempSubKey->funcCode == funcCode)
		 {
		 nowSubKey = tempSubKey;
		 break;
		 }

		 tempSubKey = tempSubKey->downNode;
		 }
		 }else if(nowSubKey->funcCode == KEY_FUNC_FCTN_DIGITAL_FSK_MODE)
		 {
		 switch(sysData.digital.fsk.mode)
		 {
		 case fskm2: funcCode = KEY_FUNC_FCTN_DIGITAL_FSK_MODE_FSK2; break;
		 case fskm4: funcCode = KEY_FUNC_FCTN_DIGITAL_FSK_MODE_FSK4; break;
		 case fskm8: funcCode = KEY_FUNC_FCTN_DIGITAL_FSK_MODE_FSK8; break;
		 }

		 subKeyDef* tempSubKey = nowSubKey->rightNode;

		 while (tempSubKey != NULL)
		 {
		 if (tempSubKey->funcCode == funcCode)
		 {
		 nowSubKey = tempSubKey;
		 break;
		 }

		 tempSubKey = tempSubKey->downNode;
		 }
		 }else if(nowSubKey->funcCode == KEY_FUNC_FCTN_DIGITAL_PSK_MODE)
		 {
		 switch(sysData.digital.psk.mode)
		 {
		 case pskmBpsk: funcCode = KEY_FUNC_FCTN_DIGITAL_PSK_MODE_BPSK; break;
		 case pskmQpsk: funcCode = KEY_FUNC_FCTN_DIGITAL_PSK_MODE_QPSK; break;
		 case pskmPI4Psk: funcCode = KEY_FUNC_FCTN_DIGITAL_PSK_MODE_PI4PSK; break;
		 case pskm8: funcCode = KEY_FUNC_FCTN_DIGITAL_PSK_MODE_PSK8; break;
		 }

		 subKeyDef* tempSubKey = nowSubKey->rightNode;

		 while (tempSubKey != NULL)
		 {
		 if (tempSubKey->funcCode == funcCode)
		 {
		 nowSubKey = tempSubKey;
		 break;
		 }

		 tempSubKey = tempSubKey->downNode;
		 }
		 } else*/
		{
			nowSubKey = nowSubKey->rightNode;
		}
	}
}
#endif

//进入下级
void tSysMenu::gotoDownLevel(void)
{
	if ((nowSubKey != NULL) && (nowSubKey->rightNode != NULL))
	{
		unsigned long int funcCode = 0;

		subKeyDef* tempSubKey = nowSubKey->rightNode; //先进入子节点
		funcCode = tempSubKey->funcCode; //初始funcCode为第一个子节点的funcCode
		switch (tempSubKey->funcCode)
		//根据进入的子节点的名称来设置要寻找的funcCode
		{
			case KEY_FUNC_FREQREFSTATE_RETURN: //频率参数状态返回键
				if (sysData.freq.ref.state)
				{
					funcCode = KEY_FUNC_FREQREFSTATE_ON;
				} else
				{
					funcCode = KEY_FUNC_FREQREFSTATE_OFF;
				}
				break;
			case KEY_FUNC_FREQREF10M_RETURN: //参考时基返回键
				if (sysData.freq.ref10MHz == rmInternal)
				{
					funcCode = KEY_FUNC_FREQREF10M_INTERRIOR;
				} else if (sysData.freq.ref10MHz == rmExternal)
				{
					funcCode = KEY_FUNC_FREQREF10M_EXTERNAL;
				}
				break;
			case KEY_FUNC__AMPL_ALC_RETURN: //稳幅状态返回键
				if (sysData.ampl.alc.state)
				{
					funcCode = KEY_FUNC__AMPL_ALC_ON;
				} else
				{
					funcCode = KEY_FUNC__AMPL_ALC_OFF;
				}
				break;
			case KEY_FUNC_AMPLRESPONSE_RETURN: //稳幅响应返回键
				if (sysData.ampl.alc.response)
				{
					funcCode = KEY_FUNC_AMPLRESPONSE_FAST;
				} else
				{
					funcCode = KEY_FUNC_AMPLRESPONSE_SLOW;
				}
				break;
			case KEY_FUNC_AMPLREFSTATE_RETURN: //幅度参考状态返回键
				if (sysData.ampl.ref.state)
				{
					funcCode = KEY_FUNC_AMPLREFSTATE_ON;
				} else
				{
					funcCode = KEY_FUNC_AMPLREFSTATE_OFF;
				}
				break;
			case KEY_FUNC_SWEEP_MODE_RETURN: //扫描模式返回键
				switch (sysData.sweep.mode)
				{
					case smOff:
						funcCode = KEY_FUNC_SWEEP_MODE_OFF;
						break;
					case smFreq:
						funcCode = KEY_FUNC_SWEEP_MODE_FREQ;
						break;
					case smAmpl:
						funcCode = KEY_FUNC_SWEEP_MODE_AMPL;
						break;
					case smFreqAmpl:
						funcCode = KEY_FUNC_SWEEP_MODE_FREQAMPL;
						break;
					default:
						break;
				}
				break;
			case KEY_FUNC_SWEEP_TYPE_RETURN: //扫描类型返回键
				if (sysData.sweep.type == stStep)
				{
					funcCode = KEY_FUNC_SWEEP_TYPE_STEP;
				} else if (sysData.sweep.type == stList)
				{
					funcCode = KEY_FUNC_SWEEP_TYPE_LIST;
				}
				break;
			case KEY_FUNC_SWEEP_REPEAT_RETURN: //重复扫描返回键
				if (sysData.sweep.repeat == srSingle)
				{
					funcCode = KEY_FUNC_SWEEP_REPEAT_SINGLE;
				} else if (sysData.sweep.repeat == srConutinuos)
				{
					funcCode = KEY_FUNC_SWEEP_REPEAT_CONT;
				}
				break;
			case KEY_FUNC_SWEEP_MORE_SWEEPTRIGGER_RETURN: //扫描触发返回键
				if (sysData.sweep.trig == stAuto)
				{
					funcCode = KEY_FUNC_SWEEP_MORE_SWEEPTRIGGER_FREERUN;
				} else if (sysData.sweep.trig == stExt)
				{
					funcCode = KEY_FUNC_SWEEP_MORE_SWEEPTRIGGER_EXT;
				}
				break;
			case KEY_FUNC_SWEEP_MORE_SWEEPDIRECTION_RETURN: //扫描方向返回键
				if (sysData.sweep.direction == sdUp)
				{
					funcCode = KEY_FUNC_SWEEP_MORE_SWEEPDIRECTION_UP;
				} else if (sysData.sweep.direction == sdDown)
				{
					funcCode = KEY_FUNC_SWEEP_MORE_SWEEPDIRECTION_DOWN;
				}
				break;
			case KEY_FUNC_SWEEP_MORE_SWEEPRETRACE_RETURN: //扫描折返返回键
				if (sysData.sweep.retrace == srOn)
				{
					funcCode = KEY_FUNC_SWEEP_MORE_SWEEPRETRACE_ON;
				} else if (sysData.sweep.retrace == srOff)
				{
					funcCode = KEY_FUNC_SWEEP_MORE_SWEEPRETRACE_OFF;
				}
				break;
			case KEY_FUNC_FCTN_ANALOG_RETURN: //模拟调制返回键
				if (sysData.analog.am.state)
				{
					funcCode = KEY_FUNC_FCTN_ANALOG_AM;
				} else if (sysData.analog.fm.state)
				{
					funcCode = KEY_FUNC_FCTN_ANALOG_FM;
				} else if (sysData.analog.pm.state)
				{
					funcCode = KEY_FUNC_FCTN_ANALOG_PM;
				} else
				{
					funcCode = KEY_FUNC_FCTN_ANALOG_RETURN;
				}
				break;
			case KEY_FUNC_FCTN_ANALOG_AM_STATE_RETURN: //模拟调制调幅状态返回键
				if (sysData.analog.am.state)
				{
					funcCode = KEY_FUNC_FCTN_ANALOG_AM_STATE_ON;
				} else
				{
					funcCode = KEY_FUNC_FCTN_ANALOG_AM_STATE_OFF;
				}
				break;
			case KEY_FUNC_FCTN_ANALOG_FM_STATE_RETURN: //模拟调制调频状态返回键
				if (sysData.analog.fm.state)
				{
					funcCode = KEY_FUNC_FCTN_ANALOG_FM_STATE_ON;
				} else
				{
					funcCode = KEY_FUNC_FCTN_ANALOG_FM_STATE_OFF;
				}
				break;
			case KEY_FUNC_FCTN_ANALOG_PM_STATE_RETURN: //模拟调制调相状态返回键
				if (sysData.analog.pm.state)
				{
					funcCode = KEY_FUNC_FCTN_ANALOG_PM_STATE_ON;
				} else
				{
					funcCode = KEY_FUNC_FCTN_ANALOG_PM_STATE_OFF;
				}
				break;
			case KEY_FUNC_FCTN_ANALOG_SOURCE_RETURN: //模拟调制源返回键
				if (sysData.analog.source == astInternal)
				{
					funcCode = KEY_FUNC_FCTN_ANALOG_SOURCE_INTERNAL;
				} else if (sysData.analog.source == astExternal)
				{
					funcCode = KEY_FUNC_FCTN_ANALOG_SOURCE_EXTERNAL;
				} else
				{
					funcCode = KEY_FUNC_FCTN_ANALOG_SOURCE_RETURN;
				}
				break;
			case KEY_FUNC_FCTN_DIGITAL_STATE_RETURN: //数字调制状态返回键
				if (sysData.digital.digitalstate)
				{
					funcCode = KEY_FUNC_FCTN_DIGITAL_STATE_ON;
				} else
				{
					funcCode = KEY_FUNC_FCTN_DIGITAL_STATE_OFF;
				}
				break;
			case KEY_FUNC_FCTN_DIGITAL_SOURCE_RETURN:
				if (sysData.digital.source == dstInternal)
				{
					funcCode = KEY_FUNC_FCTN_DIGITAL_SOURCE_INTERNAL;
				} else if (sysData.digital.source == dstExternal)
				{
					funcCode = KEY_FUNC_FCTN_DIGITAL_SOURCE_EXTERNAL;
				}
				break;
			case KEY_FUNC_FCTN_DIGITAL_TYPE_RETURN: //数字调制类型返回键
				if (sysData.digital.digitalstate)
				{
					switch (sysData.digital.digitaltype)
					{
						case dt2ASK:
						case dt4ASK:
						case dt8ASK:
							funcCode = KEY_FUNC_FCTN_DIGITAL_TYPE_ASK;
							break;
						case dt2FSK:
						case dt4FSK:
						case dt8FSK:
							funcCode = KEY_FUNC_FCTN_DIGITAL_TYPE_FSK;
							break;
						case dt2PSK:
						case dt4PSK:
						case dt8PSK:
							funcCode = KEY_FUNC_FCTN_DIGITAL_TYPE_PSK;
							break;
						default:
							funcCode = KEY_FUNC_FCTN_DIGITAL_TYPE_RETURN;
							break;
					}
				} else
				{
					funcCode = KEY_FUNC_FCTN_DIGITAL_TYPE_RETURN;
				}
				break;
			case KEY_FUNC_FCTN_DIGITAL_TYPE_ASK_RETURN: //ASK类型返回键
				if (sysData.digital.digitalstate)
				{
					switch (sysData.digital.digitaltype)
					{
						case dt2ASK:
							funcCode = KEY_FUNC_FCTN_DIGITAL_TYPE_ASK_2ASK;
							break;
						default:
							funcCode = KEY_FUNC_FCTN_DIGITAL_TYPE_ASK_RETURN;
							break;
					}
				} else
				{
					funcCode = KEY_FUNC_FCTN_DIGITAL_TYPE_ASK_RETURN;
				}
				break;
			case KEY_FUNC_FCTN_DIGITAL_TYPE_FSK_RETURN: //FSK类型返回键
				if (sysData.digital.digitalstate)
				{
					switch (sysData.digital.digitaltype)
					{
						case dt2FSK:
							funcCode = KEY_FUNC_FCTN_DIGITAL_TYPE_FSK_2FSK;
							break;
						case dt4FSK:
							funcCode = KEY_FUNC_FCTN_DIGITAL_TYPE_FSK_4FSK;
							break;
						case dt8FSK:
							funcCode = KEY_FUNC_FCTN_DIGITAL_TYPE_FSK_8FSK;
							break;
						default:
							funcCode = KEY_FUNC_FCTN_DIGITAL_TYPE_FSK_RETURN;
							break;
					}
				} else
				{
					funcCode = KEY_FUNC_FCTN_DIGITAL_TYPE_FSK_RETURN;
				}
				break;
			case KEY_FUNC_FCTN_DIGITAL_TYPE_PSK_RETURN: //PSK类型返回键
				if (sysData.digital.digitalstate)
				{
					switch (sysData.digital.digitaltype)
					{
						case dt2PSK:
							funcCode = KEY_FUNC_FCTN_DIGITAL_TYPE_PSK_2PSK;
							break;
						case dt4PSK:
							funcCode = KEY_FUNC_FCTN_DIGITAL_TYPE_PSK_4PSK;
							break;
						case dt8PSK:
							funcCode = KEY_FUNC_FCTN_DIGITAL_TYPE_PSK_8PSK;
							break;
						default:
							funcCode = KEY_FUNC_FCTN_DIGITAL_TYPE_PSK_RETURN;
							break;
					}
				} else
				{
					funcCode = KEY_FUNC_FCTN_DIGITAL_TYPE_PSK_RETURN;
				}
				break;
			case KEY_FUNC_FCTN_PLUSE_STATE_RETURN: //脉冲调制状态返回键
				if (sysData.pluse.state)
				{
					funcCode = KEY_FUNC_FCTN_PLUSE_STATE_ON;
				} else
				{
					funcCode = KEY_FUNC_FCTN_PLUSE_STATE_OFF;
				}
				break;
			case KEY_FUNC_FCTN_PLUSE_SOURCE_RETURN: //脉冲源返回键
				if (sysData.pluse.state)
				{
					if (sysData.pluse.plusesource == psInternal)
					{
						funcCode = KEY_FUNC_FCTN_PLUSE_SOURCE_INTERNAL;
					} else if (sysData.pluse.plusesource == psExternal)
					{
						funcCode = KEY_FUNC_FCTN_PLUSE_SOURCE_EXTERNAL;
					}
				} else
				{
					funcCode = KEY_FUNC_FCTN_PLUSE_SOURCE_RETURN;
				}
				break;
			case KEY_FUNC_FCTN_PLUSE_METHOD_RETURN: //脉冲触发方式返回键
				if (sysData.pluse.state)
				{
					if (sysData.pluse.plusemethod == plmAuto)
					{
						funcCode = KEY_FUNC_FCTN_PLUSE_METHOD_AUTO;
					} else if (sysData.pluse.plusemethod == plmTrigg)
					{
						funcCode = KEY_FUNC_FCTN_PLUSE_METHOD_TRIG;
					}
				} else
				{
					funcCode = KEY_FUNC_FCTN_PLUSE_METHOD_RETURN;
				}
				break;
			case KEY_FUNC_FCTN_LF_STATE_RETURN: //低频输出状态返回键
				if (sysData.lf.state)
				{
					funcCode = KEY_FUNC_FCTN_LF_STATE_ON;
				} else
				{
					funcCode = KEY_FUNC_FCTN_LF_STATE_OFF;
				}
				break;
			case KEY_FUNC_FCTN_LF_SOURCE_RETURN: //低频输出波形返回键
				if (sysData.lf.state)
				{
					switch (sysData.lf.type)
					{
						case lotSine:
							funcCode = KEY_FUNC_FCTN_LF_WAVEFORM_SINE;
							break;
						case lotSquare:
							funcCode = KEY_FUNC_FCTN_LF_WAVEFORM_SQUARE;
							break;
						case lotTriangle:
							funcCode = KEY_FUNC_FCTN_LF_WAVEFORM_TRIANGULAR;
							break;
						case lotSawtooth:
							funcCode = KEY_FUNC_FCTN_LF_WAVEFORM_SAWTOOTH;
							break;
						default:
							funcCode = KEY_FUNC_FCTN_LF_SOURCE_RETURN;
							break;
					}
				} else
				{
					funcCode = KEY_FUNC_FCTN_LF_SOURCE_RETURN;
				}
				break;
			case KEY_FUNC_FCTN_EXTRF_EXTRF_RETURN: //上变频变频源选择键
				if (sysData.exfRef.type == erftInternal)
				{
					funcCode = KEY_FUNC_FCTN_EXTRF_EXTRF_INT;
				} else if (sysData.exfRef.type == erftExternal)
				{
					funcCode = KEY_FUNC_FCTN_EXTRF_EXTRF_EXT;
				}
				break;
			case KEY_FUNC_FCTN_DRG_STATE_RETURN: //DRG状态返回键
				if (sysData.drg.state)
				{
					funcCode = KEY_FUNC_FCTN_DRG_STATE_ON;
				} else
				{
					funcCode = KEY_FUNC_FCTN_DRG_STATE_OFF;
				}
				break;
			case KEY_FUNC_FCTN_DRG_MORE_WORKMODE_RETURN:
				if (sysData.drg.workmode == ddnormal)
				{
					funcCode = KEY_FUNC_FCTN_DRG_MORE_WORKMODE_NORMAL;
				} else if (sysData.drg.workmode == ddpositive)
				{
					funcCode = KEY_FUNC_FCTN_DRG_MORE_WORKMODE_POSITIVE;
				} else if (sysData.drg.workmode == ddnegative)
				{
					funcCode = KEY_FUNC_FCTN_DRG_MORE_WORKMODE_NEGATIVE;
				}
				break;
			case KEY_FUNC_AUX_SCPI_RETURN: //SCPI刷新返回键
				if (sysData.system.isScpiFresh)
				{
					funcCode = KEY_FUNC_AUX_SCPI_ON;
				} else
				{
					funcCode = KEY_FUNC_AUX_SCPI_OFF;
				}
				break;

			case KEY_FUNC_AUX_LANGUAGE_RETURN: //语言返回键
				if (sysData.system.isLanguageEn)
				{
					funcCode = KEY_FUNC_AUX_LANGUAGE_ENGLISH;
				} else
				{
					funcCode = KEY_FUNC_AUX_LANGUAGE_CHINESE;
				}
				break;

			default:
				nowSubKey = nowSubKey->rightNode; //默认是进入子节点
				return;
		}

		while (tempSubKey != NULL) //循环直到找到对应的键
		{
			if (tempSubKey->funcCode == funcCode)
			{
				nowSubKey = tempSubKey;
				break;
			}

			tempSubKey = tempSubKey->downNode;
		}
	}
}

//获取按键索引
int tSysMenu::getButtonIndex(int value)
{
	int index = -1;

	switch (value)
	{
		case KEY_SUB_MENU1:
			index = 0;
			break;
		case KEY_SUB_MENU2:
			index = 1;
			break;
		case KEY_SUB_MENU3:
			index = 2;
			break;
		case KEY_SUB_MENU4:
			index = 3;
			break;
		case KEY_SUB_MENU5:
			index = 4;
			break;
		case KEY_SUB_MENU6:
			index = 5;
			break;
		case KEY_SUB_MENU7:
			index = 6;
			break;
	}

	return index;
}

//获取系统键码值
void tSysMenu::getSystemKeyAndFuncValue(void)
{
	if (sysKey == NULL)
		return;

	mainKeyTemp = sysKey;

	while (mainKeyTemp != NULL)
	{
		if (mainKeyTemp->keyValueStr != NULL)
		{
			mainKeyTemp->keyValue = getKeyValue(mainKeyTemp->keyValueStr);
		}

		subKeyTemp = mainKeyTemp->rightNode;
		while (subKeyTemp != NULL)
		{
			if (subKeyTemp->keyValueStr != NULL)
			{
				subKeyTemp->keyValue = getKeyValue(subKeyTemp->keyValueStr);
			}

			if (subKeyTemp->funcCodeStr != NULL)
			{
				subKeyTemp->funcCode = getFuncCode(subKeyTemp->funcCodeStr);
			}

			getSubSubKey(subKeyTemp);

			subKeyTemp = subKeyTemp->downNode;
		}

		mainKeyTemp = mainKeyTemp->downNode;
	}
}

//递归获取子键
void tSysMenu::getSubSubKey(subKeyDef* key)
{
	if (key == NULL)
		return;

	key = key->rightNode;

	while (key != NULL)
	{
		if (key->keyValueStr != NULL)
		{
			key->keyValue = getKeyValue(key->keyValueStr);
		}

		if (key->funcCodeStr != NULL)
		{
			key->funcCode = getFuncCode(key->funcCodeStr);
		}

		if (key->rightNode != NULL)
		{
			getSubSubKey(key);
		}

		key = key->downNode;
	}
}

//获取键值
int tSysMenu::getKeyValue(QString keyValueString)
{
	if ((keyValueString.isEmpty()) || (keyValueString.isNull()))
	{
		return 0x0;
	}

	for (int i = 0; i < sizeof valueList / sizeof valueList[0]; i++)
		if (valueList[i].name == keyValueString)
		{
			return valueList[i].value;
		}

	return 0x0;
}

//获取功能码
int tSysMenu::getFuncCode(QString funcCodeString)
{
	if ((funcCodeString.isEmpty()) || (funcCodeString.isNull()))
	{
		return 0x0;
	}

	for (int i = 0; i < sizeof codeList / sizeof codeList[0]; i++)
		if (codeList[i].name == funcCodeString)
		{
			return codeList[i].value;
		}

	return 0x0;
}
