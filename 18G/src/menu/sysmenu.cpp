#include "sysmenu.h"

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
  //value == 0x01;
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
  return value == KEY_NUM_ENTER;
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
  for (int i = 0; i < sizeof UNITKEYLIST / sizeof UNITKEYLIST[0]; i++)
  if (UNITKEYLIST[i] == value)
  {
	return true;
  }

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
    	nowSubKey = nowMainKey->rightNode;//获取子键指针
      }

      return;
    }

    nowMainKey = nowMainKey->downNode;//下一个子键
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
void tSysMenu::gotoDownLevel(void)
{
  if ((nowSubKey != NULL) && (nowSubKey->rightNode != NULL))
  {
	unsigned long int funcCode = 0;

	//如果需要对应显示
	if (nowSubKey->funcCode == KEY_FUNC_SOURCE_TRACKGEN_MEASTYPE)
	{
	  switch (sysData.source.measType)
	  {
	    case S21: funcCode = KEY_FUNC_SOURCE_TRACKGEN_MEASTYPE_S21; break;
	    case S11: funcCode = KEY_FUNC_SOURCE_TRACKGEN_MEASTYPE_S11; break;
	    default: nowSubKey = nowSubKey->rightNode; return;
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
	} else if (nowSubKey->funcCode == KEY_FUNC_AMPT_REFUNIT)
	{
      switch (sysData.ampt.refUnit)
      {
        case rudBm:
          funcCode = KEY_FUNC_AMPT_REFUNIT_DBM;
          break;
        case rudBmV:
       	  funcCode = KEY_FUNC_AMPT_REFUNIT_DBMV;
          break;
        case rudBuV:
       	  funcCode = KEY_FUNC_AMPT_REFUNIT_DBUV;
          break;
        case ruW:
       	  funcCode = KEY_FUNC_AMPT_REFUNIT_W;
          break;
        case ruV:
       	  funcCode = KEY_FUNC_AMPT_REFUNIT_V;
          break;
		case rudBuW:
		  funcCode = KEY_FUNC_AMPT_REFUNIT_DBuW;
		  break;
		case rudBpW:
		  funcCode = KEY_FUNC_AMPT_REFUNIT_DBPW;
		  break;

        default:
       	  nowSubKey = nowSubKey->rightNode;
          return;
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
	} else if (nowSubKey->funcCode == KEY_FUNC_SYSTEM_LANGUAGE)
	{
      if (sysData.system.isLanguageEn)
      {
    	funcCode = KEY_FUNC_SYSTEM_LANGUAGE_ENGLISH;
      } else
      {
   	    funcCode = KEY_FUNC_SYSTEM_LANGUAGE_CHINESE;
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
	} else if (nowSubKey->funcCode == KEY_FUNC_PRINTSETUP_PAPER)
	{
      switch (sysData.print.paper)
      {
        case ptA4: funcCode = KEY_FUNC_PRINTSETUP_PAPER_A4; break;
        case ptA3: funcCode = KEY_FUNC_PRINTSETUP_PAPER_A3; break;
        case ptB5: funcCode = KEY_FUNC_PRINTSETUP_PAPER_B5; break;
        case ptC5: funcCode = KEY_FUNC_PRINTSETUP_PAPER_C5; break;
        case ptLetter: funcCode = KEY_FUNC_PRINTSETUP_PAPER_LETTER; break;
        default: funcCode = KEY_FUNC_PRINTSETUP_PAPER_A4; break;
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
	} else if (nowSubKey->funcCode == KEY_FUNC_FILE_SHOW)
	{
      switch (sysData.file.show)
      {
        case stScreenPng: funcCode = KEY_FUNC_FILE_SHOW_SCREENS; break;
        case stTraceData: funcCode = KEY_FUNC_FILE_SHOW_TRACEDATAS; break;
        case stAll: funcCode = KEY_FUNC_FILE_SHOW_ALL; break;
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
	} else if (nowSubKey->funcCode == KEY_FUNC_FILE_OPERATIONS_SORT)
	{
      switch (sysData.file.sort)
      {
        case stName: funcCode = KEY_FUNC_FILE_OPERATIONS_SORT_NAME; break;
        case stTime: funcCode = KEY_FUNC_FILE_OPERATIONS_SORT_DATETIME; break;
        case stSize: funcCode = KEY_FUNC_FILE_OPERATIONS_SORT_SIZE; break;
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
	} else if (nowSubKey->funcCode == KEY_FUNC_SYSTEM_CONFIGIO_COM_BAUDRATE)
	{
      switch (sysData.system.io.com.baudrate)
      {
        case br1200: funcCode = KEY_FUNC_SYSTEM_CONFIGIO_COM_BAUDRATE_1200; break;
        case br2400: funcCode = KEY_FUNC_SYSTEM_CONFIGIO_COM_BAUDRATE_2400; break;
        case br4800: funcCode = KEY_FUNC_SYSTEM_CONFIGIO_COM_BAUDRATE_4800; break;
        case br9600: funcCode = KEY_FUNC_SYSTEM_CONFIGIO_COM_BAUDRATE_9600; break;
        case br19200: funcCode = KEY_FUNC_SYSTEM_CONFIGIO_COM_BAUDRATE_19200; break;
        case br38400: funcCode = KEY_FUNC_SYSTEM_CONFIGIO_COM_BAUDRATE_38400; break;
        case br57600: funcCode = KEY_FUNC_SYSTEM_CONFIGIO_COM_BAUDRATE_57600; break;
        case br115200: funcCode = KEY_FUNC_SYSTEM_CONFIGIO_COM_BAUDRATE_115200; break;
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
	} else if (nowSubKey->funcCode == KEY_FUNC_SYSTEM_CONFIGIO_COM_DATABITS)
	{
      switch (sysData.system.io.com.databits)
      {
        case db5: funcCode = KEY_FUNC_SYSTEM_CONFIGIO_COM_DATABITS_5; break;
        case db6: funcCode = KEY_FUNC_SYSTEM_CONFIGIO_COM_DATABITS_6; break;
        case db7: funcCode = KEY_FUNC_SYSTEM_CONFIGIO_COM_DATABITS_7; break;
        case db8: funcCode = KEY_FUNC_SYSTEM_CONFIGIO_COM_DATABITS_8; break;
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
	} else if (nowSubKey->funcCode == KEY_FUNC_SYSTEM_CONFIGIO_COM_STOPBITS)
	{
      switch (sysData.system.io.com.stopbits)
      {
        case sb1: funcCode = KEY_FUNC_SYSTEM_CONFIGIO_COM_STOPBITS_1; break;
        case sb2: funcCode = KEY_FUNC_SYSTEM_CONFIGIO_COM_STOPBITS_2; break;
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
	} else if (nowSubKey->funcCode == KEY_FUNC_SYSTEM_CONFIGIO_COM_CHECK)
	{
      switch (sysData.system.io.com.check)
      {
        case cmEven: funcCode = KEY_FUNC_SYSTEM_CONFIGIO_COM_CHECK_EVEN; break;
        case cmOdd: funcCode = KEY_FUNC_SYSTEM_CONFIGIO_COM_CHECK_ODD; break;
        case cmNone: funcCode = KEY_FUNC_SYSTEM_CONFIGIO_COM_CHECK_NONE; break;
        case cmSpace: funcCode = KEY_FUNC_SYSTEM_CONFIGIO_COM_CHECK_SPACE; break;
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
	} else if (nowSubKey->funcCode == KEY_FUNC_SYSTEM_CONFIGIO_COM_FLOWCONTROL)
	{
      switch (sysData.system.io.com.flow)
      {
        case fcXonXoff: funcCode = KEY_FUNC_SYSTEM_CONFIGIO_COM_FLOWCONTROL_XONXOFF; break;
        case fcHardware: funcCode = KEY_FUNC_SYSTEM_CONFIGIO_COM_FLOWCONTROL_HARDWARE; break;
        case fcNone: funcCode = KEY_FUNC_SYSTEM_CONFIGIO_COM_FLOWCONTROL_NONE; break;
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
	} else if (nowSubKey->funcCode == KEY_FUNC_SYSTEM_POWERONPRESET_POWERON)
	{
      switch (sysData.system.powerOnPreset.powerOnParam)
      {
        case spFactory: funcCode = KEY_FUNC_SYSTEM_POWERONPRESET_POWERON_FACTORY; break;
        case spUser: funcCode = KEY_FUNC_SYSTEM_POWERONPRESET_POWERON_USER; break;
        case spPowerOff: funcCode = KEY_FUNC_SYSTEM_POWERONPRESET_POWERON_POWEROFF; break;
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
	} else if (nowSubKey->funcCode == KEY_FUNC_SYSTEM_POWERONPRESET_PRESET)
	{
      switch (sysData.system.powerOnPreset.presetParam)
      {
        case spFactory: funcCode = KEY_FUNC_SYSTEM_POWERONPRESET_PRESET_FACTORY; break;
        case spUser: funcCode = KEY_FUNC_SYSTEM_POWERONPRESET_PRESET_USER; break;
        case spPowerOff: funcCode = KEY_FUNC_SYSTEM_POWERONPRESET_PRESET_POWEROFF; break;
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
	} else if (nowSubKey->funcCode == KEY_FUNC_SYSTEM_DATETIME_FORMAT)
	{
      switch (sysData.system.datetime.style)
      {
        case dtsYMDHMS: funcCode = KEY_FUNC_SYSTEM_DATETIME_FORMAT_YMDHMS; break;
        case dtsHMSYMD: funcCode = KEY_FUNC_SYSTEM_DATETIME_FORMAT_HMSYMD; break;
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
	} else if (nowSubKey->funcCode == KEY_FUNC_MEAS_DISCRIMINATOR_AMPLITUDE_INPUTGAIN)
	{
	  switch (sysData.disc.inputGain)
	  {
	    case igtx1: funcCode = KEY_FUNC_MEAS_DISCRIMINATOR_AMPLITUDE_INPUTGAIN_X1; break;
	    case igtx2: funcCode = KEY_FUNC_MEAS_DISCRIMINATOR_AMPLITUDE_INPUTGAIN_X2; break;
	    case igtx4: funcCode = KEY_FUNC_MEAS_DISCRIMINATOR_AMPLITUDE_INPUTGAIN_X4; break;
	    case igtx05: funcCode = KEY_FUNC_MEAS_DISCRIMINATOR_AMPLITUDE_INPUTGAIN_X05; break;
	    case igtx025: funcCode = KEY_FUNC_MEAS_DISCRIMINATOR_AMPLITUDE_INPUTGAIN_X025; break;
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
	} else if (nowSubKey->funcCode == KEY_FUNC_DISPLAY_STYLE)
	{
	  switch (sysData.display.winStyle)
	  {
	    case dsDefault: funcCode = KEY_FUNC_DISPLAY_STYLE_DEFAULT; break;
	    case dsWinXP: funcCode = KEY_FUNC_DISPLAY_STYLE_STYLE1; break;
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
	} else if (nowSubKey->funcCode == KEY_FUNC_BW_EMC_VALUE)
	{
	  if ((unsigned long)sysData.bw.rbw == 1e6)
	  {
	    funcCode = KEY_FUNC_BW_EMC_1MHZ;
	  } else if ((unsigned long)sysData.bw.rbw == 120e3)
	  {
	    funcCode = KEY_FUNC_BW_EMC_120KHZ;
	  } else if ((unsigned long)sysData.bw.rbw == 30e3)
	  {
	    funcCode = KEY_FUNC_BW_EMC_30KHZ;
	  } else if ((unsigned long)sysData.bw.rbw == 9e3)
	  {
	    funcCode = KEY_FUNC_BW_EMC_9KHZ;
	  } else if ((unsigned long)sysData.bw.rbw == 200)
	  {
	    funcCode = KEY_FUNC_BW_EMC_200HZ;
	  } else
	  {
	    funcCode = KEY_FUNC_BW_EMC_VALUE;
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
	} else if(nowSubKey->funcCode == KEY_FUNC_MODE_OPTIONS_DEMOD_DEMODMODE)
	{
	  switch(sysData.demod.mode)
	  {
	    case dmAM: funcCode = KEY_FUNC_MODE_OPTIONS_DEMOD_DEMODMODE_AM; break;
	    case dmFM: funcCode = KEY_FUNC_MODE_OPTIONS_DEMOD_DEMODMODE_FM; break;
	    case dmFMW: funcCode = KEY_FUNC_MODE_OPTIONS_DEMOD_DEMODMODE_FMW; break;
	    case dmUSB: funcCode = KEY_FUNC_MODE_OPTIONS_DEMOD_DEMODMODE_USB; break;
	    case dmLSB: funcCode = KEY_FUNC_MODE_OPTIONS_DEMOD_DEMODMODE_LSB; break;
	    default:break;
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
	}else if(nowSubKey->funcCode == KEY_FUNC_DEMOD_RADIOSET)
	{
	  switch(sysData.demod.radioSet)
	  {
		case radio1: funcCode = KEY_FUNC_DEMOD_RADIOSET_1; break;
		case radio2: funcCode = KEY_FUNC_DEMOD_RADIOSET_MUSIC; break;
		case radio3: funcCode = KEY_FUNC_DEMOD_RADIOSET_TRAFFIC; break;
		case radio4: funcCode = KEY_FUNC_DEMOD_RADIOSET_2; break;
		case radio5: funcCode = KEY_FUNC_DEMOD_RADIOSET_3; break;
		case radio6: funcCode = KEY_FUNC_DEMOD_RADIOSET_4; break;
		default:break;
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
	}else
	{
      nowSubKey = nowSubKey->rightNode;
	}
  }
}

//获取按键索引
int tSysMenu::getButtonIndex(int value)
{
  int index = -1;

  switch (value)
  {
    case KEY_SUB_MENU1: index = 0; break;
    case KEY_SUB_MENU2: index = 1; break;
    case KEY_SUB_MENU3: index = 2; break;
    case KEY_SUB_MENU4: index = 3; break;
    case KEY_SUB_MENU5: index = 4; break;
    case KEY_SUB_MENU6: index = 5; break;
    case KEY_SUB_MENU7: index = 6; break;
  }

  return index;
}

//获取系统键码值
void tSysMenu::getSystemKeyAndFuncValue(void)
{
  if (sysKey == NULL) return;

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
  if (key == NULL) return;

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
