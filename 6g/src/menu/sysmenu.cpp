#include "sysmenu.h"
#include "../scpi/sysscpi.h"
//����
tSysMenu::tSysMenu(QString xmlFileName)
{
	//����XML�ļ�
	loadFromXmlFile(xmlFileName);

	//��ֵ�͹������Ӧ
	getSystemKeyAndFuncValue();
}

//����
tSysMenu::~tSysMenu()
{

}

//�Ϸ����
bool tSysMenu::isUpArrow(int value)
{
	return value == KEY_CTL_ARROW_UP;
}

//��������ť
bool tSysMenu::isRightRpg(int value)
{

	return 0;
}

//��ȡ������
int tSysMenu::getRpgPulse(int value, int value2)
{
	if (value == 0xDC)
		return value2 * (-1);
	else if (value == 0xCD)
		return value2 * 1;
}

//���µ�������More��
bool tSysMenu::pressedIsHardMoreKey(int value)
{
	return value == KEY_NUM_MORE;
}

//���µ�������Return��
bool tSysMenu::pressedIsHardReturnKey(int value)
{
	return value == KEY_NUM_RETURN;
}

//���µ����˸��
bool tSysMenu::pressedIsBkspaceKey(int value)
{
	return value == KEY_NUM_BKSPACE;
}

//���µ���ȷ�ϼ�
bool tSysMenu::pressedIsEnterKey(int value)
{
	return false;
}

//���µ��Ƿ����
bool tSysMenu::pressedIsArrowKey(int value)
{
	for (int i = 0; i < sizeof ARROWKEYLIST / sizeof ARROWKEYLIST[0]; i++)
	{
		if (ARROWKEYLIST[i] == value)
		{
			return true;
		}
	}
	return false;
}

//���µ�IncrSet��
bool tSysMenu::pressedIsIncrsetKey(int value)
{
	return value == KEY_MAIN_INCRSET;
}

//��������Trigger��
bool tSysMenu::pressedIsTriggerKey(int value)
{
	return value == KEY_MAIN_TRIGGER;
}

//����PageUp��
bool tSysMenu::pressedIsPageUpKey(int value)
{
	return value == KEY_CTL_PAGEUP;
}

//����PageDown��
bool tSysMenu::pressedIsPageDownKey(int value)
{
	return value == KEY_CTL_PAGEDOWN;
}

//���µ���Select��
bool tSysMenu::pressedIsSelectKey(int value)
{
	return value == KEY_CTL_SELECT;
}

//���µ���������ť��
bool tSysMenu::pressedIsRpgKey(int value)
{
	return (value == 0x02);
}

//���µ�������
bool tSysMenu::pressedIsMainKey(int value)
{
	for (int i = 0; i < sizeof MAINKEYLIST / sizeof MAINKEYLIST[0]; i++)
	{
		if (MAINKEYLIST[i] == value)
		{
			return true;
		}
	}
	return false;
}

//���µ���ͨ�ü�
bool tSysMenu::pressedIsCommonKey(int value)
{
	for (int i = 0; i < sizeof COMMONKEYLIST / sizeof COMMONKEYLIST[0]; i++)
	{
		if (COMMONKEYLIST[i] == value)
		{
			return true;
		}
	}
	return false;
}

//���µ������ּ�
bool tSysMenu::pressedIsNumberKey(int value)
{
	for (int i = 0; i < sizeof NUMBERKEYLIST / sizeof NUMBERKEYLIST[0]; i++)
	{
		if (NUMBERKEYLIST[i] == value)
		{
			return true;
		}
	}
	return false;
}

//���µ���ȡ����
bool tSysMenu::pressedIsEscKey(int value)
{
	return value == KEY_CTL_CANCELESC;
}

//���µ��ǵ�λ��
bool tSysMenu::pressedIsUnitKey(int value)
{
	return false;
}

//��ȡ����ָ��
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

//ͨ��keyValue��ȡ�Ӽ�ָ��
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

//�����ϼ�
void tSysMenu::gotoUpLevel(void)
{
	while (nowSubKey != NULL)
	{
		if (nowSubKey->upNode == NULL)
		{
			if (nowSubKey->leftNode != NULL && nowSubKey->leftNode->keyValue == KEY_SUB_MENU6)
			{
				//�е������ܼ�����More��
				nowSubKey = nowSubKey->leftNode;
			}
			else
			{
				break;
			}
		}
		nowSubKey = nowSubKey->upNode;
	}
	if (nowSubKey->leftNode != NULL)
	{
		nowSubKey = nowSubKey->leftNode;
	}
}

//�����¼�
void tSysMenu::gotoDownLevel(void)
{
	if ((nowSubKey != NULL) && (nowSubKey->rightNode != NULL))
	{
		unsigned long int funcCode = 0;

		subKeyDef* tempSubKey = nowSubKey->rightNode;
		switch (tempSubKey->funcCode)
		//���ݽ�����ӽڵ������������ҪѰ�ҵ�funcCode
		{
			case KEY_FUNC_SWEEP_MODE_OFF:
				if (sysData.sweep.mode == smOff)
					funcCode = KEY_FUNC_SWEEP_MODE_OFF;
				else if (sysData.sweep.mode == smFreq)
					funcCode = KEY_FUNC_SWEEP_MODE_FREQ;
				else if (sysData.sweep.mode == smAmpl)
					funcCode = KEY_FUNC_SWEEP_MODE_AMPL;
				else if (sysData.sweep.mode == smFreqAmpl)
					funcCode = KEY_FUNC_SWEEP_MODE_FREQAMPL;
				else
					funcCode = KEY_FUNC_SWEEP_MODE_OFF;
				break;
			case KEY_FUNC_PULSE_TRIGGERWAY_FREERUN:
				if (sysData.pulse.trigmethod == plmFreerun)
					funcCode = KEY_FUNC_PULSE_TRIGGERWAY_FREERUN;
				else if (sysData.pulse.trigmethod == plmKeytrig)
					funcCode = KEY_FUNC_PULSE_TRIGGERWAY_KEYTRIG;
				else if (sysData.pulse.trigmethod == plmExttrig)
					funcCode = KEY_FUNC_PULSE_TRIGGERWAY_EXTTRIG;
				else
					funcCode = KEY_FUNC_PULSE_TRIGGERWAY_FREERUN;
				break;
			case KEY_FUNC_AUXFCTN_LFOUT_WAVE_SINE:
				if (sysData.lf.type == lotSine)
					funcCode = KEY_FUNC_AUXFCTN_LFOUT_WAVE_SINE;
				else if (sysData.lf.type == lotSquare)
					funcCode = KEY_FUNC_AUXFCTN_LFOUT_WAVE_SQUARE;
				else if (sysData.lf.type == lotTriangle)
					funcCode = KEY_FUNC_AUXFCTN_LFOUT_WAVE_TRIANGLE;
				else if (sysData.lf.type == lotSawtooth)
					funcCode = KEY_FUNC_AUXFCTN_LFOUT_WAVE_SAWTOOTH;
				else
					funcCode = KEY_FUNC_AUXFCTN_LFOUT_WAVE_SINE;
				break;
			case KEY_FUNC_AUXFCTN_DIGITALMOD_MODE_ASK:
				if (sysData.digital.digitaltype == dt2ASK || sysData.digital.digitaltype == dt4ASK || sysData.digital.digitaltype == dt8ASK)
					funcCode = KEY_FUNC_AUXFCTN_DIGITALMOD_MODE_ASK;
				else if (sysData.digital.digitaltype == dt2FSK || sysData.digital.digitaltype == dt4FSK || sysData.digital.digitaltype == dt8FSK)
					funcCode = KEY_FUNC_AUXFCTN_DIGITALMOD_MODE_FSK;
				else if (sysData.digital.digitaltype == dt2PSK || sysData.digital.digitaltype == dt4PSK || sysData.digital.digitaltype == dt8PSK)
					funcCode = KEY_FUNC_AUXFCTN_DIGITALMOD_MODE_PSK;
				else
					funcCode = KEY_FUNC_AUXFCTN_DIGITALMOD_MODE_ASK;
				break;
			case KEY_FUNC_AUXFCTN_DIGITALMOD_MODE_FSK_2FSK:
				if (sysData.digital.digitaltype == dt2FSK)
					funcCode = KEY_FUNC_AUXFCTN_DIGITALMOD_MODE_FSK_2FSK;
				else if (sysData.digital.digitaltype == dt4FSK)
					funcCode = KEY_FUNC_AUXFCTN_DIGITALMOD_MODE_FSK_4FSK;
				else if (sysData.digital.digitaltype == dt8FSK)
					funcCode = KEY_FUNC_AUXFCTN_DIGITALMOD_MODE_FSK_8FSK;
				else
					funcCode = KEY_FUNC_AUXFCTN_DIGITALMOD_MODE_FSK_2FSK;
				break;
			case KEY_FUNC_AUXFCTN_DIGITALMOD_MODE_PSK_2PSK:
				if (sysData.digital.digitaltype == dt2PSK)
					funcCode = KEY_FUNC_AUXFCTN_DIGITALMOD_MODE_PSK_2PSK;
				else if (sysData.digital.digitaltype == dt4PSK)
					funcCode = KEY_FUNC_AUXFCTN_DIGITALMOD_MODE_PSK_4PSK;
				else if(sysData.digital.digitaltype == dt8PSK)
					funcCode = KEY_FUNC_AUXFCTN_DIGITALMOD_MODE_FSK_8PSK;
				else
					funcCode = KEY_FUNC_AUXFCTN_DIGITALMOD_MODE_PSK_2PSK;
				break;
			default:
				nowSubKey = nowSubKey->rightNode; //Ĭ���ǽ����ӽڵ�
				return;
		}		
		while (tempSubKey != NULL) //ѭ��ֱ���ҵ���Ӧ�ļ�
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

//��ȡ��������
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
		//case KEY_SUB_MENU7:
		//	index = 6;
		//	break;
		default:
			break;
	}

	return index;
}

//��ȡϵͳ����ֵ
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

//�ݹ��ȡ�Ӽ�
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

//��ȡ��ֵ
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

//��ȡ������
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
