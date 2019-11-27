#include "mainform.h"
#include "sysscpi.h"
#include "sysmenu.h"
double StrToValue(QString str)
{
	int multiple = 0;
	QString tempQSNum;
	QString tempQSChar;

	for (int i = 0; i < str.length(); i++)
	{
		if ((str[i] >= '*') && (str[i] <= '9'))
		{
			tempQSNum.append(str[i]);
		} else if ((str[i] >= 'A') && (str[i] <= 'z'))
		{
			tempQSChar.append(str[i]);
		}
	}

	QString temp = tempQSChar.trimmed().toUpper();

	if (tempQSChar == "GHZ")
	{
		multiple = 1000e6;
		return tempQSNum.toDouble() * multiple;
	} else if (tempQSChar == "MHZ")
	{
		multiple = 1000e3;
		return tempQSNum.toDouble() * multiple;
	} else if (tempQSChar == "KHZ")
	{
		multiple = 1000;
		return tempQSNum.toDouble() * multiple;
	} else if (tempQSChar == "HZ")
	{
		multiple = 1;
		return tempQSNum.toDouble() * multiple;
	} else if (tempQSChar == "DBM")
	{
		multiple = 1;
		return tempQSNum.toDouble() * multiple;
	} else if (tempQSChar == "MS")
	{
		multiple = 1;
		return tempQSNum.toDouble() * multiple;
	} else if (tempQSChar == "S")
	{
		multiple = 1000;
		return tempQSNum.toDouble() * multiple;
	} else if (tempQSChar == "VPP")
	{
		multiple = 1000;
		return tempQSNum.toDouble() * multiple;
	} else if (tempQSChar == "MVPP")
	{
		multiple = 1;
		return tempQSNum.toDouble() * multiple;
	} else
	{
		return tempQSNum.toDouble();
	}
}

bool StrToBool(QString str)
{
	QString temp = str.toUpper();
	bool rt;
	if (temp == "ON")
		rt = true;
	else if (temp == "OFF")
		rt = false;
	else if (temp == "LIST")
		rt = true;
	else if (temp == "STEP")
		rt = false;
	else if (temp == "DOWN")
		rt = true;
	else if (temp == "UP")
		rt = false;
	else
		rt = str.toDouble();

	return rt;
}

int StrToNum(QString str)
{
	QString temp = str.toUpper();

	int rt;

	if (temp == "SINE")
		rt = 1;
	else if ((temp == "SQU") || (temp == "SQUARE"))
		rt = 2;
	else if ((temp == "TRI") || (temp == "TRIANGLE"))
		rt = 3;
	else if (temp == "SQU")
		rt = 2;
	else if (temp == "TRI")
		rt = 3;
	else if (temp == "RAMP")
		rt = 4;
	else
		rt = temp.toDouble();

	return rt;
}

int ListDataDispose(QString str, int* num, double* value)
{
	int index = 0;
	QString strTemp1, strTemp2;
	index = str.indexOf(",");

	if (index == -1)
		return -1;

	strTemp1 = str.left(index);
	*num = strTemp1.toInt();

	strTemp2.insert(0, str.right(str.length() - index - 1));
	*value = StrToValue(strTemp2);
	return 0;
}

//����SCPIָ��
void tSysScpi::handleScpiCommand(QString cmdStr)
{
	int i, j;
	QString tempStr = cmdStr;
	QStringList cmdList;
	char sendbuf[9] = { 0x00 };
	QString command = "";
	resultDef result;
	QString updateDir = "";
	QString updateStr = "";
	char tempChar[32] = { };
	int exeResult = __SCPI_FAILED;
	int tempIndex = 0;
	QString returnString = "";
	QString refTempValue = "";
	QString sendToMSG1030QS;
	QString sendToMSG1030QS1;
	char SendToMSG1030CH[50];
	unsigned char *tempData;
	float temp = 0.0;
	uint8_t value;

	uint32_t N1 = 0, N2 = 0; //�ܲ�����
	uint32_t m1 = 0, m2 = 0; //�����ȼ���ֵ
	uint32_t n1 = 0, n2 = 0; //����������ֵ
	uint32_t d1 = 0, d2 = 0;
	uint32_t nco1, nco2;
	int tempint;

	//�쳣����
	if (!isUpdating)
	{
		if (cmdStr.isNull() || cmdStr.isEmpty() || cmdStr.length() > 1024)
		{
			scpiBuffer = "";
			return;
		}
	}

	//��֡����
	for (int i = 0; i < tempStr.length(); i++)
	{
		if (tempStr.at(i) == QChar(';') || tempStr.at(i) == QChar(0x0d) || tempStr.at(i) == QChar(0x0a))
		{
			if (tempStr.at(i) != QChar(';')) //������";"��β�Ľ����滻��";"
			{
				tempStr.replace(i, 1, ";");
			}
		}
	}

	//ƴ֡����
	//	cmdList = tempStr.split(";");		//ԭ���Ĵ�����
	cmdList = tempStr.split(";", QString::SkipEmptyParts); //zpc add

	if (tempStr.at(tempStr.length() - 1) != QChar(';'))
	{
		//���յ��������ʱ�������������Ǹ�����浽scpiBuffer�У��´ν���ʱ���ۼ�
		if (cmdList.count() <= 1)
		{
			scpiBuffer = tempStr;
		} else
		{
			scpiBuffer = cmdList.at(cmdList.count() - 1);
		}

		cmdList.removeAt(cmdList.count() - 1); //��QT�б��У�ɾ�����������Ǹ�����
	} else //�����������ʱ�����scpiBuffer
	{
		scpiBuffer = "";
	}

	int auxInt = 0;
	bool auxBool = 0;
	int traceInt = 0;
	bool traceBool = 0;
	int Num = 0;
	double Value = 0.0;

	//ָ�����
	for (int i = 0; i < cmdList.count(); i++) //��CPU��138��û�л��Ƶ�����£���һ��ָ�ʼ���ڶ���ָ�ʼ��ʱ����Ϊ15ms���ң�����ָ���ڽ��գ�ʹ�����ڷ��ͣ��������������Ҫ��ʱ��ԼΪ5ms��һ�����Ƶ�ʱ��ԼΪ20~25ms
	{

		command = cmdList.at(i).trimmed();
		if (command.isNull() || command.isEmpty())
		{
			continue;
		}

		memset(parseCmd, 0, sizeof parseCmd);
		returnString = "";
		exeResult = __SCPI_FAILED;
		result = GetFunctionCode(command);

		printf("cmd[%d] on client[%d] = %s, len = %d, funcCode = 0x%08x\n", i, cmdList.count(), cmdList.at(i).trimmed().toStdString().c_str(), cmdList.at(i).length(), result.cmdCode);

		//update command
		switch (result.cmdCode)
		{

			case SCPI_CMD_UPDATE_BEGIN:
				exeResult = __SCPI_RETURN;
				returnString = "NO";
				isUpdating = true;
				isRcvingFile = false;

				drawWin4(getTheTranslatorString(QString("Updating System")));
    			updateDir = UpdateDirName + "/update";

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
				printf("0000000000000000000000000000000000000000000000000\n");
			case SCPI_CMD_UPDATE_FILE:
				exeResult = __SCPI_RETURN;
				returnString = "NO";

				if (updateRecordOfAdd(result.value.trimmed()))
				{
					returnString = "YES";
					printf("11111111111111111111111111111111111111111111\n");
					outputToScreen(getTheTranslatorString("Updating System"), updateFileName(result.value.trimmed()) + "......");
					isRcvingFile = true;
				} else
				{
					
					printf("2222222222222222222222222222222222222\n");
					isRcvingFile = false;
				}
				break;
			case SCPI_CMD_UPDATE_END:
				isRcvingFile = false;
				isUpdating = false;
				updateSystem();
				exeResult = __SCPI_RETURN;
				returnString = "YES";
				printf("3333333333333333333333333333333333\n");
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

				//		    getLocalDateTime(tempChar);
				outputToScreen(getTheTranslatorString("Updating Success") + "(" + result.value.trimmed() + "), " + getTheTranslatorString("Rebooting") + "......", "");

				usleep(1000 * 2000);
				system("reboot&");
				return;
			case SCPI_CMD_UPDATE_CANCEL:
				printf("4444444444444444444444444444444444444444444444\n");
				isRcvingFile = false;
				isUpdating = false;
				exeResult = __SCPI_RETURN;
				returnString = "YES";
				break;
			case SCPI_CMD_UPDATE_CRC:
				printf("55555555555555555555555555555555555555555555555\n");
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
					//		   	sendSerialData(QString("*IDN"));
					usleep(10 * 1000);
					//		   	returnString = company + ", " + devType + ", SN" + QString(sysData.system.serialNo).trimmed() + ", " + versionString;
					returnString = company + ", " + moduDevType + ", SN" + moduSerial + ", " + moduSoftVer + ", " + versionString;
					break;
					//RST
				case SCPI_CMD_STANDARD_RST:
					exeResult = __SCPI_RETURN;
					presetSystem();
					initKeyBoardLED();
					sendSerialData("*RST");
//					savePowerOffState();
					//			sysData.EnterPara.Display = false;
					//			sysLocked = true;
					//			sysLocked = false;
					//���ƽ���
					emit drawInputParamterNoParamter();
					emit drawFreqAndAmplMain();
					emit drawFreqInParamter();
					break;

				case SCPI_CMD_STANDARD_VER:
					exeResult = __SCPI_RETURN;
					returnString = versionString;
					break;

					//device
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

				case SCPI_CMD_DEVICE_COMPANY_SET:
					company = result.value.trimmed();

					saveDeviceCompany(company);
					exeResult = __SCPI_RETURN;
					returnString = "YES";
					break;

				case SCPI_CMD_DEVICE_COMPANY_GET:
					exeResult = __SCPI_RETURN;
					returnString = "Y" + company;
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
					//�������ð汾
				case SCPI_CMD_DEVICE_STATE_EVA_SET:
					setSystemServiceOfEvalute(true);
					exeResult = __SCPI_RETURN;
					returnString = "YES";
					break;
					//�������ð汾
				case SCPI_CMD_DEVICE_STATE_FULL_SET:
					setSystemServiceOfEvalute(false);
					exeResult = __SCPI_RETURN;
					returnString = "YES";
					break;

					/*
					 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
					 case SCPI_CMD_TEST_TESTS:
					 exeResult = __SCPI_RETURN;
					 if (__DEBUGSCPI)
					 {
					 printf("SCPI_CMD_TEST\r\n");
					 }
					 break;
					 */
					//����Ƶ��
				case SCPI_CMD_FREQ_SET:
					exeResult = setFrequency(result.value.trimmed().toUpper());

					//���ƽ���
					if (sysData.system.isScpiFresh) //����SCPIˢ���Ƿ�����ж��Ƿ�ˢ�½���
					{
						emit drawInputParamterNoParamter();
						//				emit drawFreqAndAmplMain();
						emit drawFreqInParamter();
					}
					emit drawFreqAndAmplMain();

					if (__DEBUGSCPI)
					{
						printf("SCPI_CMD_FREQ\r\n");
					}
					break;

					//��ȡƵ��ֵ
				case SCPI_CMD_FREQ_GET:
					exeResult = __SCPI_RETURN;

					returnString = doubleToString(sysData.freq.freq).trimmed() + "Hz";
					break;

					//����Ƶ�ʲο�
				case SCPI_CMD_FREQ_REF_SET:
					setFrequency(StrToValue(result.value.trimmed().toUpper()));
					exeResult = setFrequencyOfReference();

					//���ƽ���
					if (sysData.system.isScpiFresh)
					{
						emit drawInputParamterNoParamter();
						emit
						drawFreqAndAmplMain();
						emit drawFreqInParamter();
					}
					break;
					//��ȡƵ�ʲο�ֵ
				case SCPI_CMD_FREQ_REF_GET:
					exeResult = __SCPI_RETURN;
					returnString = doubleToString(sysData.freq.ref.value).trimmed() + "Hz";
					break;

					//����Ƶ�ʲο�״̬
				case SCPI_CMD_FREQ_REF_STATE_SET:
					exeResult = setFrequencyOfReferenceState(result.value.trimmed().toUpper());

					//���ƽ���
					if (sysData.system.isScpiFresh)
					{
						emit drawInputParamterNoParamter();
						emit
						drawFreqAndAmplMain();
						emit drawFreqInParamter();
					}
					break;
					//��ȡƵ�ʲο�״̬
				case SCPI_CMD_FREQ_REF_STATE_GET:
					exeResult = __SCPI_RETURN;
					returnString = boolToString(sysData.freq.ref.state).trimmed();
					break;

					//����Ƶ��ƫ��ֵ
				case SCPI_CMD_FREQ_OFFS_SET:
					exeResult = setFrequencyOffset(result.value.trimmed().toUpper());

					//���ƽ���
					if (sysData.system.isScpiFresh)
					{
						emit drawInputParamterNoParamter();
						emit
						drawFreqAndAmplMain();
						emit drawFreqInParamter();
					}
					break;
					//��ȡƵ��ƫ��ֵ
				case SCPI_CMD_FREQ_OFFS_GET:
					exeResult = __SCPI_RETURN;
					returnString = doubleToString(sysData.freq.offset).trimmed() + "Hz";
					break;

					//����Ƶ�ʱ�Ƶֵ
				case SCPI_CMD_FREQ_MULT_SET:
					exeResult = setFrequencyMultipliter(result.value.trimmed().toUpper());

					//���ƽ���
					if (sysData.system.isScpiFresh)
					{
						emit drawInputParamterNoParamter();
						emit
						drawFreqAndAmplMain();
						emit drawFreqInParamter();
					}
					break;
					//��ȡƵ�ʱ�Ƶֵ
				case SCPI_CMD_FREQ_MULT_GET:
					exeResult = __SCPI_RETURN;
					returnString = doubleToString(sysData.freq.multiplier).trimmed();
					break;

					//���÷���ֵ
				case SCPI_CMD_POW_SET:

					//�رյ����ź�
					//closeDemod();
					exeResult = setAmplitude(result.value.trimmed().toUpper());
					//���ƽ���
					if (sysData.system.isScpiFresh)
					{
						emit drawInputParamterNoParamter();
						emit
						drawFreqAndAmplMain();
						emit drawAmplInParamter();
					}
					emit drawFreqAndAmplMain();
					break;
					//��ȡ����ֵ
				case SCPI_CMD_POW_GET:
					exeResult = __SCPI_RETURN;
					//   			  returnString = doubleToString(sysData.ampl.ampl).trimmed();
					returnString = doubleToString(sysData.ampl.downLoad).trimmed() + "dBm"; //��ȡ��ֵӦ�����·���ֵ
					break;

					//���÷��Ȳο�ֵ
				case SCPI_CMD_POW_REF_SET:
					setAmplitude(StrToValue(result.value));
					exeResult = setAmplitudeOfReference();

					//���ƽ���
					if (sysData.system.isScpiFresh)
					{
						emit drawInputParamterNoParamter();
						emit
						drawFreqAndAmplMain();
						emit drawAmplInParamter();
					}
					break;
					//��ȡ���Ȳο�ֵ
				case SCPI_CMD_POW_REF_GET:
					exeResult = __SCPI_RETURN;
					returnString = doubleToString(sysData.ampl.ref.value).trimmed() + "dBm";
					break;

					//���÷��Ȳο�״̬
				case SCPI_CMD_POW_REF_STAT_SET:
					exeResult = setAmplitudeOfReferenceState(result.value.trimmed().toUpper());
					//���ƽ���
					if (sysData.system.isScpiFresh)
					{
						emit drawInputParamterNoParamter();
						emit
						drawFreqAndAmplMain();
						emit drawAmplInParamter();
					}
					break;
					//��ȡ���Ȳο�״̬
				case SCPI_CMD_POW_REF_STAT_GET:
					exeResult = __SCPI_RETURN;
					returnString = boolToString(sysData.ampl.ref.state).trimmed();
					break;

					//���÷���ƫ��ֵ
				case SCPI_CMD_POW_OFFS_SET:
					exeResult = setAmplitudeOffset(result.value.trimmed().toUpper());
					//���ƽ���
					if (sysData.system.isScpiFresh)
					{
						emit drawInputParamterNoParamter();
						emit
						drawFreqAndAmplMain();
						emit drawAmplInParamter();
					}
					break;
					//��ȡ����ƫ��ֵ
				case SCPI_CMD_POW_OFFS_GET:
					exeResult = __SCPI_RETURN;
					returnString = doubleToString(sysData.ampl.offset).trimmed() + "dB";
					break;

					//�������²���
				case SCPI_CMD_POW_ERROR_SET:
					exeResult = setAmplitudeError(result.value.trimmed().toUpper());

					//���ƽ���
					if (sysData.system.isScpiFresh)
					{
						emit drawInputParamterNoParamter();
						emit
						drawFreqAndAmplMain();
						emit drawAmplInParamter();
					}
					break;
					//��ȡ���²���
				case SCPI_CMD_POW_ERROR_GET:
					exeResult = __SCPI_RETURN;
					returnString = doubleToString(sysData.ampl.error.value).trimmed() + "dB";
					break;

					//����ɨ��״̬
#if 0
					case SCPI_CMD_SWEEP_STAT_SET:
					//				exeResult = setSweepstate(StrToBool(result.value.trimmed().toUpper()));

					strcpy(sysData.name, "");
					strcpy(sysData.value, "");
					strcpy(sysData.unit, "");
					emit drawSweep(sysData.sweep.type);
					break;
					case SCPI_CMD_SWEEP_STAT_GET:
					exeResult = __SCPI_RETURN;
					switch(sysData.sweep.mode)
					{
						case smFreq:
						returnString = "Freq";
						break;
						case smAmpl:
						returnString = "Ampl";
						break;
						case smFreqAmpl:
						returnString = "Freq and Ampl";
						break;
					}
					break;
#endif				
					//����ɨ��ģʽ
				case SCPI_CMD_SWEEP_MODE_SET:
					exeResult = setSweepMode(result.value.trimmed().toUpper());
					//���ƽ���
					if (sysData.system.isScpiFresh)
					{
						strcpy(sysData.name, "");
						strcpy(sysData.value, "");
						strcpy(sysData.unit, "");
						emit
						drawInputParamterNoParamter();
						emit drawSweep(sysData.sweep.type);
					}
					break;
					//��ȡɨ��ģʽ
				case SCPI_CMD_SWEEP_MODE_GET:
					exeResult = __SCPI_RETURN;
					sprintf(tempChar, "%d", sysData.sweep.mode);
					returnString = QString(tempChar).trimmed();
					//					switch(sysData.sweep.mode)
					//					{
					//						case smOff:  returnString = "Off"; break;
					//						case smFreq:  returnString = "Freq"; break;
					//						case smAmpl:  returnString = "Ampl"; break;
					//						case smFreqAmpl:  returnString = "Freq&Ampl"; break;
					//					}
					break;

					//ɨ����������
				case SCPI_CMD_OPER_TYPE_SET:
					exeResult = __SCPI_SUCCESS;

					if (StrToBool(result.value.trimmed().toUpper()) == false)
					{
						setSweepType(0);
					} else
					{
						setSweepType(1);
					}
					//���ƽ���
					if (sysData.system.isScpiFresh)
					{
						strcpy(sysData.name, "");
						strcpy(sysData.value, "");
						strcpy(sysData.unit, "");
						emit
						drawInputParamterNoParamter();
						emit drawSweep(sysData.sweep.type);
					}
					break;
					//��ȡɨ������
				case SCPI_CMD_OPER_TYPE_GET:
					exeResult = __SCPI_RETURN;
					sprintf(tempChar, "%d", sysData.sweep.type);
					returnString = QString(tempChar).trimmed();
					//				  if(sysData.sweep.type == stStep)
					//				  {
					//					returnString = "Step";
					//				  }else if(sysData.sweep.type = stList)
					//				  {
					//					returnString = "List";
					//				  }
					break;
					//����ɨƵ��ʼƵ��
				case SCPI_CMD_FREQ_STAR_SET:
					exeResult = __SCPI_SUCCESS;
					setStepStartFrequency(result.value.trimmed().toUpper());
					//���ƽ���
					if (sysData.system.isScpiFresh)
					{
						strcpy(sysData.name, "");
						strcpy(sysData.value, "");
						strcpy(sysData.unit, "");
						emit
						drawInputParamterNoParamter();
						emit drawSweep(sysData.sweep.type);
					}

					if (__DEBUGSCPI)
					{
						printf("SCPI_CMD_FREQ_STAR\r\n");
					}
					break;
				case SCPI_CMD_FREQ_STAR_GET: //ɨƵ��ʼƵ��
					exeResult = __SCPI_RETURN;
					returnString = doubleToString(sysData.sweep.step.freqStart).trimmed();
					break;
					//����ɨƵ��ֹƵ��
				case SCPI_CMD_FREQ_STOP_SET:
					exeResult = __SCPI_SUCCESS;
					setStepStopFrequency(result.value.trimmed().toUpper());

					//���ƽ���
					if (sysData.system.isScpiFresh)
					{
						strcpy(sysData.name, "");
						strcpy(sysData.value, "");
						strcpy(sysData.unit, "");
						emit
						drawInputParamterNoParamter();
						emit drawSweep(sysData.sweep.type);
					}

					if (__DEBUGSCPI)
					{
						printf("SCPI_CMD_FREQ_STOP\r\n");
					}
					break;

				case SCPI_CMD_FREQ_STOP_GET: //ɨƵ��ֹƵ��
					exeResult = __SCPI_RETURN;
					returnString = doubleToString(sysData.sweep.step.freqStop).trimmed();
					break;
					//����ɨ����ʼ����
				case SCPI_CMD_POW_STAR_SET:
					exeResult = __SCPI_SUCCESS;
					//				sysData.sweep.step.amplStart = StrToValue(result.value);
					setStepStartAmplitude(result.value.trimmed().toUpper());

					//���ƽ���
					if (sysData.system.isScpiFresh)
					{
						strcpy(sysData.name, "");
						strcpy(sysData.value, "");
						strcpy(sysData.unit, "");
						emit
						drawInputParamterNoParamter();
						emit drawSweep(sysData.sweep.type);
					}

					if (__DEBUGSCPI)
					{
						printf("SCPI_CMD_POW_STAR\r\n");
					}
					break;
				case SCPI_CMD_POW_STAR_GET: //ɨ����ʼ����
					exeResult = __SCPI_RETURN;
					returnString = doubleToString(sysData.sweep.step.amplStart).trimmed();
					break;
					//����ɨ����ֹ����
				case SCPI_CMD_POW_STOP_SET:
					exeResult = __SCPI_SUCCESS;
					//				sysData.sweep.step.amplStop = StrToValue(result.value);
					setStepStopAmplitude(result.value.trimmed().toUpper());

					//���ƽ���
					if (sysData.system.isScpiFresh)
					{
						strcpy(sysData.name, "");
						strcpy(sysData.value, "");
						strcpy(sysData.unit, "");
						emit
						drawInputParamterNoParamter();
						emit drawSweep(sysData.sweep.type);
					}

					if (__DEBUGSCPI)
					{
						printf("SCPI_CMD_POW_STOP\r\n");
					}
					break;
				case SCPI_CMD_POW_STOP_GET: //ɨ����ֹ����
					exeResult = __SCPI_RETURN;
					returnString = doubleToString(sysData.sweep.step.amplStop).trimmed();
					break;
					//����ɨ��פ��ʱ��
				case SCPI_CMD_SWE_DWEL_SET:
					//				sysData.sweep.step.dwell = StrToValue(result.value);
					exeResult = setStepDwell(result.value.trimmed().toUpper());

					//���ƽ���
					if (sysData.system.isScpiFresh)
					{
						strcpy(sysData.name, "");
						strcpy(sysData.value, "");
						strcpy(sysData.unit, "");
						emit
						drawInputParamterNoParamter();
						emit drawSweep(sysData.sweep.type);
					}
					break;
				case SCPI_CMD_SWE_DWEL_GET: //ɨ��פ��ʱ��
					exeResult = __SCPI_RETURN;
					returnString = doubleToString(sysData.sweep.step.dwell).trimmed() + QString("ms");
					break;
					//����ɨ�����
				case SCPI_CMD_SWE_POIN_SET: //ɨ�����
					exeResult = setStepPoints(result.value.trimmed().toUpper());

					//���ƽ���
					if (sysData.system.isScpiFresh)
					{
						emit drawSweep(sysData.sweep.type);
					}
					break;
				case SCPI_CMD_SWE_POIN_GET: //ɨ�����
					exeResult = __SCPI_RETURN;
					returnString = doubleToString(sysData.sweep.step.point).trimmed();
					break;

					/*
					 case SCPI_CMD_OPER_FREQ_STAT_SET:	//ɨ��Ƶ��״̬
					 exeResult = __SCPI_SUCCESS;
					 if(StrToBool(result.value) == true)
					 {
					 setSweepMode(smFreqOn);
					 }
					 else
					 {
					 setSweepMode(smFreqOff);
					 }
					 //���ƽ���
					 strcpy(sysData.name, "");
					 strcpy(sysData.value, "");
					 strcpy(sysData.unit, "");
					 emit drawInputParamterNoParamter();
					 emit drawSweep(sysData.sweep.type);
					 break;

					 case SCPI_CMD_OPER_FREQ_STAT_GET:	//ɨ��Ƶ��״̬
					 exeResult = __SCPI_RETURN;

					 if(sysData.sweep.mode == smFreqOff || sysData.sweep.mode == smOff)
					 {
					 returnString = "0";
					 }else if(sysData.sweep.mode == smFreqOn || sysData.sweep.mode == smFreqAmpl)
					 {
					 returnString = "1";
					 }
					 break;
					 */
					//ɨ������		ONѭ��  OFF����ɨ��
				case SCPI_CMD_OPER_CONT_SET:
					exeResult = setSweepRepeat(StrToBool(result.value.trimmed().toUpper()));

					//���ƽ���
					if (sysData.system.isScpiFresh)
					{
						strcpy(sysData.name, "");
						strcpy(sysData.value, "");
						strcpy(sysData.unit, "");
						emit
						drawInputParamterNoParamter();
						emit drawSweep(sysData.sweep.type);
					}
					break;
				case SCPI_CMD_OPER_CONT_GET: //ɨ������		0������ 		1������
					exeResult = __SCPI_RETURN;
					sprintf(tempChar, "%d", sysData.sweep.repeat);
					returnString = QString(tempChar).trimmed();
					//	          if(sysData.sweep.repeat == srSingle)
					//	          {
					//	        	returnString = "Single";
					//	          }else if(sysData.sweep.repeat == srConutinuos)
					//	          {
					//	        	returnString = "Conutinuos";
					//	          }
					break;
					//ɨ�败����ʽ
				case SCPI_CMD_TRIG_TYPE_SET:
					exeResult = setSweepTrig(StrToBool(result.value.trimmed().toUpper()));
					//���ƽ���
					if (sysData.system.isScpiFresh)
					{
						strcpy(sysData.name, "");
						strcpy(sysData.value, "");
						strcpy(sysData.unit, "");
						emit
						drawInputParamterNoParamter();
						emit drawSweep(sysData.sweep.type);
					}
					break;
				case SCPI_CMD_TRIG_TYPE_GET:
					exeResult = __SCPI_RETURN;
					sprintf(tempChar, "%d", sysData.sweep.trig);
					returnString = QString(tempChar).trimmed();
					//				if(sysData.sweep.trig == stAuto)
					//				{
					//					returnString = "Auto";
					//				}else if(sysData.sweep.trig == stExt)
					//				{
					//					returnString = "External";
					//				}
					break;

					//ɨ�跽��		1������ 		0������
				case SCPI_CMD_OPER_DIR_SET:
					exeResult = setSweepDirection(StrToBool(result.value.trimmed().toUpper()));

					//���ƽ���
					if (sysData.system.isScpiFresh)
					{
						strcpy(sysData.name, "");
						strcpy(sysData.value, "");
						strcpy(sysData.unit, "");
						emit
						drawInputParamterNoParamter();
						emit drawSweep(sysData.sweep.type);
					}
					break;

				case SCPI_CMD_OPER_DIR_GET:
					exeResult = __SCPI_RETURN;

					if ((sysData.sweep.direction) == sdUp)
					{
						returnString = "0";
					} else if ((sysData.sweep.direction) == sdDown)
					{
						returnString = "1";
					}
					break;
					//����ɨ���۷�   ON�۷� OFF���۷�
				case SCPI_CMD_OPER_RETR_SET: //ɨ���۷�		0���۷� 		1�����۷�
					exeResult = __SCPI_SUCCESS;
					if (StrToBool(result.value) == true)
					{
						sysData.sweep.retrace = srOn;
					} else
					{
						sysData.sweep.retrace = srOff;
					}
					setSweepRetrace(sysData.sweep.retrace);

					//���ƽ���
					if (sysData.system.isScpiFresh)
					{
						strcpy(sysData.name, "");
						strcpy(sysData.value, "");
						strcpy(sysData.unit, "");
						emit
						drawInputParamterNoParamter();
						emit drawSweep(sysData.sweep.type);
					}
					break;

				case SCPI_CMD_OPER_RETR_GET: //ɨ���۷�		0���۷� 		1�����۷�
					exeResult = __SCPI_RETURN;

					if (sysData.sweep.retrace == srOff)
					{
						returnString = "OFF";
					} else if (sysData.sweep.retrace == srOn)
					{
						returnString = "ON";
					}
					break;
					/*
					 case SCPI_CMD_OPER_INIT_SET:      	//ɨ�败��		0�������� 	1������
					 exeResult = __SCPI_SUCCESS;

					 //��Ƶ����
					 sendToMSG1030QS = ":INITiate:TRIGger;\r\n";
					 strcpy(SendToMSG1030CH,sendToMSG1030QS.toLatin1());
					 printf("%s",SendToMSG1030CH);
					 break;
					 */

					/*
					 case SCPI_CMD_LIST_FREQ_SET:
					 exeResult = __SCPI_SUCCESS;

					 ListDataDispose(result.value, &Num, &Value);
					 sysData.Sweep.List.Data[Num - 1].Freq = Value;

					 if(sysData.Sweep.List.Data[Num - 1].Freq < sysData.Freq.MinValue)
					 sysData.Sweep.List.Data[Num - 1].Freq = sysData.Freq.MinValue;
					 else if (sysData.Sweep.List.Data[Num - 1].Freq > sysData.Freq.MaxValue)
					 sysData.Sweep.List.Data[Num - 1].Freq = sysData.Freq.MaxValue;

					 //���ƽ���
					 redrawsListData();
					 break;
					 case SCPI_CMD_LIST_FREQ_GET:
					 exeResult = __SCPI_RETURN;
					 returnString = doubleToString(sysData.Sweep.List.Data[Num - 1].Freq).trimmed();
					 break;

					 case SCPI_CMD_LIST_POW_SET:
					 exeResult = __SCPI_SUCCESS;

					 ListDataDispose(result.value, &Num, &Value);
					 sysData.Sweep.List.Data[Num - 1].Ampl = Value;

					 if(sysData.Sweep.List.Data[Num - 1].Ampl < sysData.Ampl.MinValue)
					 sysData.Sweep.List.Data[Num - 1].Ampl = sysData.Ampl.MinValue;
					 else if (sysData.Sweep.List.Data[Num - 1].Ampl > sysData.Ampl.MaxValue)
					 sysData.Sweep.List.Data[Num - 1].Ampl = sysData.Ampl.MaxValue;

					 //���ƽ���
					 redrawsListData();
					 break;
					 case SCPI_CMD_LIST_POW_GET:
					 exeResult = __SCPI_RETURN;
					 returnString = doubleToString(sysData.Sweep.List.Data[Num - 1].Ampl).trimmed();
					 break;

					 case SCPI_CMD_LIST_DWEL_SET:
					 exeResult = __SCPI_SUCCESS;

					 ListDataDispose(result.value, &Num, &Value);
					 sysData.Sweep.List.Data[Num - 1].Dwell = Value;

					 if(sysData.Sweep.List.Data[Num - 1].Dwell < sysData.Sweep.Dwell.MinValue)
					 sysData.Sweep.List.Data[Num - 1].Dwell = sysData.Sweep.Dwell.MinValue;
					 else if (sysData.Sweep.List.Data[Num - 1].Dwell > sysData.Sweep.Dwell.MaxValue)
					 sysData.Sweep.List.Data[Num - 1].Dwell = sysData.Sweep.Dwell.MaxValue;

					 //���ƽ���
					 redrawsListData();
					 break;
					 case SCPI_CMD_LIST_DWEL_GET:
					 exeResult = __SCPI_RETURN;
					 returnString = doubleToString(sysData.Sweep.List.Data[Num - 1].Dwell).trimmed();
					 break;

					 case SCPI_CMD_LIST_POINT_SET:
					 exeResult = __SCPI_SUCCESS;

					 sysData.Sweep.List.AllRow = result.value.toInt();
					 if(sysData.Sweep.List.AllRow < 2)	sysData.Sweep.List.AllRow = 2;

					 sysData.Sweep.List.AllPage = sysData.Sweep.List.AllRow / 8 + 1;

					 sysData.Sweep.List.DisplayPage = 0;
					 sysData.Sweep.List.SelectRow = 0;
					 sysData.Sweep.List.SelectLine = 0;

					 if((sysData.Sweep.List.AllRow % 6) > 0)
					 sysData.Sweep.List.AllPage = sysData.Sweep.List.AllRow / 6 + 1;
					 else
					 sysData.Sweep.List.AllPage = sysData.Sweep.List.AllRow / 6;

					 //��Ƶ����
					 sendToMSG1030QS.sprintf("%d", sysData.Sweep.List.AllRow);
					 sendToMSG1030QS = ":LIST:POINts " + sendToMSG1030QS + ";\r\n";
					 strcpy(SendToMSG1030CH,sendToMSG1030QS.toLatin1());
					 printf("%s",SendToMSG1030CH);

					 //���ƽ���
					 redrawsListData();
					 break;
					 case SCPI_CMD_LIST_POINT_GET:
					 exeResult = __SCPI_RETURN;
					 returnString = QString(intToQString(sysData.Sweep.List.AllRow, tempChar)).trimmed();
					 break;

					 case SCPI_CMD_OPER_LOAD_SET:
					 exeResult = __SCPI_SUCCESS;

					 for(i = 0; i < sysData.Sweep.List.AllRow; i++)
					 {
					 j = i + 1;
					 sendToMSG1030QS.sprintf("%.0f",sysData.Sweep.List.Data[i].Freq);
					 sendToMSG1030QS1.sprintf("%d",j);
					 sendToMSG1030QS = ":LIST:FREQuency " + sendToMSG1030QS1 + "," + sendToMSG1030QS + ";\r\n";
					 strcpy(SendToMSG1030CH,sendToMSG1030QS.toLatin1());
					 printf("%s",SendToMSG1030CH);

					 sendToMSG1030QS.sprintf("%.0f",sysData.Sweep.List.Data[i].Ampl);
					 sendToMSG1030QS1.sprintf("%d",j);
					 sendToMSG1030QS = ":LIST:POWer " + sendToMSG1030QS1 + "," + sendToMSG1030QS + ";\r\n";
					 strcpy(SendToMSG1030CH,sendToMSG1030QS.toLatin1());
					 printf("%s",SendToMSG1030CH);

					 sendToMSG1030QS.sprintf("%.0f",sysData.Sweep.List.Data[i].Dwell);
					 sendToMSG1030QS1.sprintf("%d",j);
					 sendToMSG1030QS = ":LIST:DWELl " + sendToMSG1030QS1 + "," + sendToMSG1030QS + ";\r\n";
					 strcpy(SendToMSG1030CH,sendToMSG1030QS.toLatin1());
					 printf("%s",SendToMSG1030CH);

					 sysData.Sweep.List.Download = true;

					 emit redrawDownloadProg(sysData.Sweep.List.Download, j, sysData.Sweep.List.AllRow);
					 }

					 if(sysData.Sweep.repeat == srConutinuos)
					 {
					 //��Ƶ����
					 strcpy(SendToMSG1030CH, ":INITiate;\r\n");
					 printf("%s",SendToMSG1030CH);
					 }
					 sysData.Sweep.List.Download = false;
					 emit redrawsListData();

					 exeResult = __SCPI_SUCCESS;
					 returnString = "OK";

					 break;
					 */
					//�б��·�
				case SCPI_CMD_LIST_DOWN:
					exeResult = setSweepListDownload();
					break;

				case SCPI_CMD_AM_STATE_SET: //AM���ƿ���		0���ر� 		1����
					exeResult = __SCPI_SUCCESS;
					setAmState(StrToBool(result.value.trimmed().toUpper()));

					//���ƽ���
					if (sysData.system.isScpiFresh)
					{
						strcpy(sysData.name, "");
						strcpy(sysData.value, "");
						strcpy(sysData.unit, "");
						emit drawAnalogModulate();
					}
					break;
					//ģ����Ƶĵ���Դ
				case SCPI_CMD_ANALOG_SOURCE_SET:
					exeResult = __SCPI_SUCCESS;
					setAnalogModulateSource(result.value.trimmed().toUpper());
					if (sysData.system.isScpiFresh)
					{
						strcpy(sysData.name, "");
						strcpy(sysData.value, "");
						strcpy(sysData.unit, "");
						emit drawAnalogModulate();
					}
					break;
				case SCPI_CMD_ANALOG_SOURCE_GET:
					exeResult = __SCPI_RETURN;
					if (sysData.analog.source == astInternal)
					{
						returnString = "0";
					} else if (sysData.analog.source == astExternal)
					{
						returnString = "1";
					} else
					{
						returnString = "0";
					}
					break;
					//AM
				case SCPI_CMD_AM_STATE_GET: //AM���ƿ���		0���ر� 		1����
					exeResult = __SCPI_RETURN;
					returnString = boolToString(sysData.analog.am.state).trimmed();
					break;

				case SCPI_CMD_AM_DEPTH_SET: //AM�������
					exeResult = __SCPI_SUCCESS;
					setAmDepth(result.value.trimmed().toUpper());

					//���ƽ���
					if (sysData.system.isScpiFresh)
					{
						strcpy(sysData.name, "");
						strcpy(sysData.value, "");
						strcpy(sysData.unit, "");
						emit drawAnalogModulate();
					}
					break;

				case SCPI_CMD_AM_DEPTH_GET: //AM�������
					exeResult = __SCPI_RETURN;
					returnString = doubleToString(sysData.analog.am.depth).trimmed();
					break;

				case SCPI_CMD_AM_RATE_SET: //AM������
					exeResult = __SCPI_SUCCESS;
					setAmRate(result.value.trimmed().toUpper());

					//���ƽ���
					if (sysData.system.isScpiFresh)
					{
						strcpy(sysData.name, "");
						strcpy(sysData.value, "");
						strcpy(sysData.unit, "");
						emit drawAnalogModulate();
					}
					break;
				case SCPI_CMD_AM_RATE_GET: //AM������
					exeResult = __SCPI_RETURN;
					returnString = doubleToString(sysData.analog.am.rate).trimmed();
					break;

				case SCPI_CMD_FM_STATE_SET: //FM���ƿ���		0���ر� 		1����
					exeResult = __SCPI_SUCCESS;
					setFmState(StrToBool(result.value.trimmed().toUpper()));

					//���ƽ���
					if (sysData.system.isScpiFresh)
					{
						strcpy(sysData.name, "");
						strcpy(sysData.value, "");
						strcpy(sysData.unit, "");
						emit drawAnalogModulate();
					}
					break;
				case SCPI_CMD_FM_STATE_GET: //FM���ƿ���		0���ر� 		1����
					exeResult = __SCPI_RETURN;
					returnString = boolToString(sysData.analog.fm.state).trimmed();
					break;

				case SCPI_CMD_FM_DEV_SET: //FM���ƿ�ȣ�Ƶƫ��
					exeResult = __SCPI_SUCCESS;
					setFmDev(result.value.trimmed().toUpper());

					//���ƽ���
					if (sysData.system.isScpiFresh)
					{
						strcpy(sysData.name, "");
						strcpy(sysData.value, "");
						strcpy(sysData.unit, "");
						emit drawAnalogModulate();
					}
					break;

				case SCPI_CMD_FM_DEV_GET: //FM���ƿ�ȣ�Ƶƫ��
					exeResult = __SCPI_RETURN;
					returnString = doubleToString(sysData.analog.fm.deviation).trimmed();
					break;

				case SCPI_CMD_FM_RATE_SET: //FM������
					exeResult = __SCPI_SUCCESS;
					setFmRate(result.value.trimmed().toUpper());

					//���ƽ���
					if (sysData.system.isScpiFresh)
					{
						strcpy(sysData.name, "");
						strcpy(sysData.value, "");
						strcpy(sysData.unit, "");
						emit drawAnalogModulate();
					}
					break;

				case SCPI_CMD_FM_RATE_GET: //FM������
					exeResult = __SCPI_RETURN;
					returnString = doubleToString(sysData.analog.fm.rate).trimmed();
					break;

				case SCPI_CMD_PM_STATE_SET: //PM���ƿ���		0���ر� 		1����
					exeResult = __SCPI_SUCCESS;
					setPmState(StrToBool(result.value.trimmed().toUpper()));
					//���ƽ���
					if (sysData.system.isScpiFresh)
					{
						strcpy(sysData.name, "");
						strcpy(sysData.value, "");
						strcpy(sysData.unit, "");
						emit drawAnalogModulate();
					}
					break;
					//PM
				case SCPI_CMD_PM_STATE_GET: //PM���ƿ���		0���ر� 		1����
					exeResult = __SCPI_RETURN;
					returnString = boolToString(sysData.analog.pm.state).trimmed();
					break;

				case SCPI_CMD_PM_DEV_SET: //PM������λ
					exeResult = __SCPI_SUCCESS;
					setPmPhase(result.value.trimmed().toUpper());
					//���ƽ���
					if (sysData.system.isScpiFresh)
					{
						strcpy(sysData.name, "");
						strcpy(sysData.value, "");
						strcpy(sysData.unit, "");
						emit drawAnalogModulate();
					}
					break;

				case SCPI_CMD_PM_DEV_GET: //PM������λ
					exeResult = __SCPI_RETURN;
					returnString = doubleToString(sysData.analog.pm.phase).trimmed();
					break;

				case SCPI_CMD_PM_RATE_SET: //PM������
					exeResult = __SCPI_SUCCESS;
					setPmRate(result.value.trimmed().toUpper());
					//���ƽ���
					if (sysData.system.isScpiFresh)
					{
						strcpy(sysData.name, "");
						strcpy(sysData.value, "");
						strcpy(sysData.unit, "");
						emit drawAnalogModulate();
					}
					break;

				case SCPI_CMD_PM_RATE_GET: //PM������
					exeResult = __SCPI_RETURN;
					returnString = doubleToString(sysData.analog.pm.rate).trimmed();
					break;
					//ASK,FSK,PSK���ƿ���״̬����
				case SCPI_CMD_AFPSK_STATE_SET: //���ƿ���	0���ر� 		1����
				//			  if(sysData.digital.digitaltype == dt2ASK || sysData.digital.digitaltype == dt4ASK
				//					  || sysData.digital.digitaltype == dt8ASK)  //���ԭ�������;���ASK�򲻸ı�
				//			  {
				//				  ;
				//			  }else  //���ԭ�������Ͳ���AKS,�����͸�ΪASK
				//			  {
				//				  sysData.digital.digitaltype = dt2ASK;
				//			  }
					exeResult = setDigitalModuateState(StrToBool(result.value.trimmed().toUpper()));

					//���ƽ���
					if (sysData.system.isScpiFresh)
					{
						strcpy(sysData.name, "");
						strcpy(sysData.value, "");
						strcpy(sysData.unit, "");
						emit drawDigitalModulate();
					}
					break;
					//ASK,FSK,PSK���ƿ���״̬��ȡ
				case SCPI_CMD_AFPSK_STATE_GET: //ASK���ƿ���	0���ر� 		1����
					exeResult = __SCPI_RETURN;
					returnString = boolToString(sysData.digital.digitalstate);
					//			  if(sysData.digital.digitalstate)
					//			  {
					//				returnString = "1";
					//			  }else
					//			  {
					//				returnString = "0";
					//			  }
					break;

					//���ֵ����ⲿ����
				case SCPI_CMD_DIGITAL_SOURCE_SET:
					exeResult = setDigitalModulateSource(result.value.trimmed().toUpper());
					if (sysData.system.isScpiFresh)
					{
						strcpy(sysData.name, "");
						strcpy(sysData.value, "");
						strcpy(sysData.unit, "");
						emit drawDigitalModulate();
					}
					break;
				case SCPI_CMD_DIGITAL_SOURCE_GET:
					exeResult = __SCPI_RETURN;
					if (sysData.digital.source == dstInternal)
					{
						returnString = "0";
					} else if (sysData.digital.source == dstExternal)
					{
						returnString = "1";
					} else
					{
						returnString = "0";
					}
					break;

				case SCPI_CMD_DIGITAL_SEQUENCE_BEGIN:
					exeResult = __SCPI_SUCCESS;
					isRcvingSequence = true;
					sysData.digital.sequenceCnt = 0;
					memset(&sysData.digital.sequence, 0, sizeof(sysData.digital.sequence));
					sequenceStr = "";
					break;
				case SCPI_CMD_DIGITAL_SEQUENCE_CANCEL:
					exeResult = __SCPI_SUCCESS;
					if (isRcvingSequence)
					{
						analysisDigiSequencedataFormLan();
						setDigtalSequenceDownload();
					}
					//������յ����У����͵��������飬����ת��Ϊunsigned char��ĸ���
					sequenceStr = "";
					memset(&sysData.digital.sequence, 0, sizeof(sysData.digital.sequence));
					sysData.digital.sequenceCnt = 0;
					sysData.digital.sequenceCntBefore = 0;
					break;
				case SCPI_CMD_DIGITAL_SEQUENCE_END:
					exeResult = __SCPI_SUCCESS;
					isRcvingSequence = false;
					break;

					//ASK,FSK,PSK������������
				case SCPI_CMD_AFPSK_RATE_SET:
					exeResult = setDigitalModuateRate(result.value.trimmed().toUpper());
					//���ƽ���
					if (sysData.system.isScpiFresh)
					{
						strcpy(sysData.name, "");
						strcpy(sysData.value, "");
						strcpy(sysData.unit, "");
						emit drawDigitalModulate();
					}
					break;

					//ASK,FSK,PSK�������ʻ�ȡ
				case SCPI_CMD_AFPSK_RATE_GET:
					exeResult = __SCPI_RETURN;
					returnString = doubleToString(sysData.digital.digitalpara.rate);
					break;

					//ASK������������	2��2ASK 4:4ASK 8:8ASK
				case SCPI_CMD_ASK_TYPE_SET:
					switch ((int) StrToValue(result.value.trimmed()))
					{
						case 2:
							exeResult = setDigitalModuateType(dt2ASK);
							break;
						case 4:
							exeResult = setDigitalModuateType(dt4ASK);
							break;
						case 8:
							exeResult = setDigitalModuateType(dt8ASK);
							break;
						default:
							break;
					}
					//���ƽ���
					if (sysData.system.isScpiFresh)
					{
						strcpy(sysData.name, "");
						strcpy(sysData.value, "");
						strcpy(sysData.unit, "");
						emit drawDigitalModulate();
					}
					break;
					/*
					 case SCPI_CMD_ASK_TYPE_GET:       	//ASK��������	2��2ASK 4:4ASK 8:8ASK
					 exeResult = __SCPI_RETURN;

					 switch(sysData.ASK.Mode)
					 {
					 case askm2: returnString = "2ASK"; break;
					 case askm4: returnString = "4ASK"; break;
					 case askm8: returnString = "8ASK"; break;
					 default: break;
					 }
					 break;
					 *//*
					 case SCPI_CMD_ASK_RATE_SET:      	//ASK������
					 if(sysData.digital.digitaltype == dt2ASK || sysData.digital.digitaltype == dt4ASK
					 || sysData.digital.digitaltype == dt8ASK)
					 {
					 ;
					 }else
					 {
					 sysData.digital.digitaltype = dt2ASK;
					 }
					 exeResult = setDigitalModuateRate(result.value.trimmed());
					 //���ƽ���
					 emit redrawDigitalModulate();
					 break;

					 case SCPI_CMD_ASK_RATE_GET:      	//ASK������
					 exeResult = __SCPI_RETURN;
					 getFreqAndUnitString(sysData.ASK.Rate.Value, 1, tempChar);
					 returnString = QString(tempChar).trimmed();
					 break;
					 */
					//ASK�����������
				case SCPI_CMD_ASK_DEPTH_SET:
					if (sysData.digital.digitaltype == dt2ASK || sysData.digital.digitaltype == dt4ASK || sysData.digital.digitaltype == dt8ASK)
					{
						exeResult = setDigitalModulatePara(result.value.trimmed().toUpper());
					} else
					{
						exeResult = __SCPI_RETURN;
						returnString = "ERR! Please select the ASK type first!";
					}
					//���ƽ���
					if (sysData.system.isScpiFresh)
					{
						strcpy(sysData.name, "");
						strcpy(sysData.value, "");
						strcpy(sysData.unit, "");
						emit drawDigitalModulate();
					}
					break;
					//ASK������Ȼ�ȡ
				case SCPI_CMD_ASK_DEPTH_GET:
					exeResult = __SCPI_RETURN;
					returnString = doubleToString(sysData.digital.digitalpara.depth).trimmed();
					break;
					/*
					 case SCPI_CMD_FSK_STATE_SET:      	//FSK���ƿ���	0���ر� 		1����
					 exeResult = setFskState(result.value.trimmed());

					 //���ƽ���
					 emit redrawDigitalModulate();

					 break;
					 case SCPI_CMD_FSK_STATE_GET:      	//FSK���ƿ���	0���ر� 		1����
					 exeResult = __SCPI_RETURN;

					 if(sysData.FSK.State)
					 {
					 returnString = "On";
					 }else
					 {
					 returnString = "Off";
					 }
					 break;
					 */
					//FSK������������
				case SCPI_CMD_FSK_TYPE_SET: //FSK��������	2��2FSK 4:4FSK 8:8FSK
					switch ((int) StrToValue(result.value.trimmed()))
					{
						case 2:
							exeResult = setDigitalModuateType(dt2FSK);
							break;
						case 4:
							exeResult = setDigitalModuateType(dt4FSK);
							break;
						case 8:
							exeResult = setDigitalModuateType(dt8FSK);
							break;
						default:
							break;
					}
					if (sysData.system.isScpiFresh)
					{
						strcpy(sysData.name, "");
						strcpy(sysData.value, "");
						strcpy(sysData.unit, "");
						emit drawDigitalModulate();
					}
					//���ƽ���
					break;
					/*
					 case SCPI_CMD_FSK_TYPE_GET:       	//FSK��������	2��2FSK 4:4FSK 8:8FSK
					 exeResult = __SCPI_RETURN;

					 switch(sysData.FSK.Mode)
					 {
					 case fskm2: returnString = "2FSK"; break;
					 case fskm4: returnString = "4FSK"; break;
					 case fskm8: returnString = "8FSK"; break;
					 default: break;
					 }

					 break;

					 case SCPI_CMD_FSK_RATE_SET:      	//FSK������
					 exeResult = setFskRate(result.value.trimmed().toUpper());

					 //���ƽ���
					 emit redrawDigitalModulate();
					 break;

					 case SCPI_CMD_FSK_RATE_GET:      	//FSK������
					 exeResult = __SCPI_RETURN;
					 getFreqAndUnitString(sysData.FSK.Rate.Value, 1, tempChar);
					 returnString = QString(tempChar).trimmed();
					 break;
					 */
					//FSK������СƵƫ����
				case SCPI_CMD_FSK_OFFSET_SET:
					if (sysData.digital.digitaltype == dt2FSK || sysData.digital.digitaltype == dt4FSK || sysData.digital.digitaltype == dt8FSK)
					{
						exeResult = setDigitalModulatePara(result.value.trimmed().toUpper());
					} else
					{
						exeResult = __SCPI_RETURN;
						returnString = "ERR! Please select the FSK type first!";
					}
					//���ƽ���
					if (sysData.system.isScpiFresh)
					{
						strcpy(sysData.name, "");
						strcpy(sysData.value, "");
						strcpy(sysData.unit, "");
						emit drawDigitalModulate();
					}
					break;

					//FSK������СƵƫ��ȡ
				case SCPI_CMD_FSK_OFFSET_GET: //FSK
					exeResult = __SCPI_RETURN;
					returnString = doubleToString(sysData.digital.digitalpara.deviation);
					break;
					/*
					 case SCPI_CMD_PSK_STATE_SET:      	//PSK���ƿ���	0���ر� 		1����
					 exeResult = setPskState(result.value);

					 //���ƽ���
					 emit redrawDigitalModulate();
					 break;
					 case SCPI_CMD_PSK_STATE_GET:      	//PSK���ƿ���	0���ر� 		1����
					 exeResult = __SCPI_RETURN;

					 if(sysData.PSK.State)
					 {
					 returnString = "On";
					 }else
					 {
					 returnString = "Off";
					 }
					 break;
					 */
					//PSK������������  	2��2PSK 4:4PSK 8:8PSK
				case SCPI_CMD_PSK_TYPE_SET:
					switch ((int) StrToValue(result.value.trimmed()))
					{
						case 2:
							exeResult = setDigitalModuateType(dt2PSK);
							break;
						case 4:
							exeResult = setDigitalModuateType(dt4PSK);
							break;
						case 8:
							exeResult = setDigitalModuateType(dt8PSK);
							break;
						default:
							break;
					}
					//���ƽ���
					if (sysData.system.isScpiFresh)
					{
						strcpy(sysData.name, "");
						strcpy(sysData.value, "");
						strcpy(sysData.unit, "");
						emit drawDigitalModulate();
					}
					break;

					//PSK������ƫ����
				case SCPI_CMD_PSK_PHASE_SET:
					if (sysData.digital.digitaltype == dt2PSK || sysData.digital.digitaltype == dt4PSK || sysData.digital.digitaltype == dt8PSK)
					{
						exeResult = setDigitalModulatePara(result.value.trimmed().toUpper());
					} else
					{
						exeResult = __SCPI_RETURN;
						returnString = "ERR! Please select the PSK type first!";
					}
					//���ƽ���
					if (sysData.system.isScpiFresh)
					{
						strcpy(sysData.name, "");
						strcpy(sysData.value, "");
						strcpy(sysData.unit, "");
						emit drawDigitalModulate();
					}
					break;

					//PSK������ƫ��ȡ
				case SCPI_CMD_PSK_PHASE_GET:
					exeResult = __SCPI_RETURN;
					returnString = doubleToString(sysData.digital.digitalpara.phase);
					break;

					//ASK,FSK,PSK�������ͻ�ȡ
				case SCPI_CMD_AFPSK_TYPE_GET:
					exeResult = __SCPI_RETURN;
					switch (sysData.digital.digitaltype)
					{
						case dt2ASK:
							returnString = "2ASK";
							break;
						case dt4ASK:
							returnString = "4ASK";
							break;
						case dt8ASK:
							returnString = "8ASK";
							break;
						case dt2FSK:
							returnString = "2FSK";
							break;
						case dt4FSK:
							returnString = "4FSK";
							break;
						case dt8FSK:
							returnString = "8FSK";
							break;
						case dt2PSK:
							returnString = "2PSK";
							break;
						case dt4PSK:
							returnString = "4PSK";
							break;
						case dt8PSK:
							returnString = "8PSK";
							break;
						default:
							break;
					}
					break;

					//PLUSE ״̬����
				case SCPI_CMD_PLUSE_STATE_SET:
					//			  exeResult = setPluseState(StrToBool(result.value.trimmed()));
					exeResult = setPluseState(result.value.trimmed());

					//���ƽ���
					if (sysData.system.isScpiFresh)
					{
						emit drawPluseModulate();
					}
					break;
					//PLUSE ״̬��ȡ
				case SCPI_CMD_PLUSE_STATE_GET:
					exeResult = __SCPI_RETURN;

					if (sysData.pluse.state)
					{
						returnString = "1";
					} else
					{
						returnString = "0";
					}
					break;
					//PLUSE ����Դ����
				case SCPI_CMD_PLUSE_SOURCE_SET:
					exeResult = setPluseSource((result.value.trimmed().toUpper()));
					//���ƽ���
					if (sysData.system.isScpiFresh)
					{
						emit drawPluseModulate();
					}
					break;
					//PLUSE ����Դ��ȡ
				case SCPI_CMD_PLUSE_SOURCE_GET:
					exeResult = __SCPI_RETURN;
					if (sysData.pluse.plusesource == psInternal)
					{
						returnString = "0";
					} else if (sysData.pluse.plusesource == psExternal)
					{
						returnString = "1";
					}
					break;
					//PLUSE ������ʽ����
				case SCPI_CMD_PLUSE_TRIGGER_SET:
					exeResult = setPluseMethod(result.value.trimmed().toUpper());
					//���ƽ���
					if (sysData.system.isScpiFresh)
					{
						emit drawPluseModulate();
					}
					break;
					//PLUSE ������ʽ��ȡ
				case SCPI_CMD_PLUSE_TRIGGER_GET:
					exeResult = __SCPI_RETURN;
					if (sysData.pluse.plusemethod == plmAuto)
					{
						returnString = "0";
					} else if (sysData.pluse.plusemethod == plmTrigg)
					{
						returnString = "1";
					}
					break;
					//PLUSE ��������
				case SCPI_CMD_PLUSE_PERIOD_SET:
					exeResult = setPlusePeriod(result.value.trimmed().toUpper());

					//���ƽ���
					if (sysData.system.isScpiFresh)
					{
						emit drawPluseModulate();
					}
					break;
					//PLUSE ���ڻ�ȡ
				case SCPI_CMD_PLUSE_PERIOD_GET:
					exeResult = __SCPI_RETURN;

					getTimeAndUnitString(sysData.pluse.period * 1e6, 1, tempChar);
					returnString = QString(tempChar).trimmed();
					break;
					//PLUSE �������
				case SCPI_CMD_PLUSE_WIDTH_SET:
					exeResult = setPluseWidth(result.value.trimmed().toUpper());

					//���ƽ���
					if (sysData.system.isScpiFresh)
					{
						emit drawPluseModulate();
					}
					break;
					//PLUSE ��Ȼ�ȡ
				case SCPI_CMD_PLUSE_WIDTH_GET:
					exeResult = __SCPI_RETURN;

					//			  getTimeAndUnitString(sysData.pluse.width * 1e6, 1, tempChar);
					//			  returnString = QString(tempChar).trimmed();
					returnString = doubleToQValue(sysData.pluse.width * 1e6).trimmed() + " " + timeToQUnit(sysData.pluse.width).trimmed();
					break;
					/*
					 case SCPI_CMD_OSK_STATE_SET:
					 exeResult = setOSKState(result.value.trimmed());

					 //���ƽ���
					 emit redrawDigitalModulate();
					 break;
					 case SCPI_CMD_OSK_STATE_GET:      	//PSK���ƿ���	0���ر� 		1����
					 exeResult = __SCPI_RETURN;

					 if(sysData.OSK.State)
					 {
					 returnString = "On";
					 }else
					 {
					 returnString = "Off";
					 }
					 break;
					 case SCPI_CMD_OSK_STEP_SET:
					 exeResult = setOSKStep(result.value.trimmed().toUpper());

					 //���ƽ���
					 emit redrawDigitalModulate();
					 break;
					 case SCPI_CMD_OSK_STEP_GET:       	//PSK��������	2��2PSK 4:4PSK 8:8PSK
					 exeResult = __SCPI_RETURN;

					 switch(sysData.OSK.Step)
					 {
					 case osks1: returnString = "1"; break;
					 case osks2: returnString = "2"; break;
					 case osks3: returnString = "3"; break;
					 case osks4: returnString = "4"; break;
					 }
					 break;

					 case SCPI_CMD_OSK_INTERVAL_SET:
					 exeResult = setOSKInterval(result.value.trimmed().toUpper());

					 //���ƽ���
					 emit redrawDigitalModulate();
					 break;
					 case SCPI_CMD_OSK_INTERVAL_GET:
					 exeResult = __SCPI_RETURN;

					 getTimeAndUnitString(sysData.OSK.Interval.Value, 1, tempChar);
					 returnString = QString(tempChar).trimmed();
					 break;


					 case SCPI_CMD_AWM_FREQ_SET:			//AWM����Ƶ��
					 exeResult = __SCPI_SUCCESS;
					 strcpy(SendToMSG1030CH,cmdStr.toLatin1());
					 printf("%s",SendToMSG1030CH);
					 break;
					 case SCPI_CMD_AWM_FREQ_GET:			//AWM����Ƶ��
					 break;
					 */
					//LF״̬����
				case SCPI_CMD_LF_SWITCH_SET:
					exeResult = setLFState(result.value.trimmed().toUpper());
					sysData.keyBoardSwitchLF = StrToBool(result.value.trimmed().toUpper()); //��LF��LED��
					keyBoardLEDSwitch(2, sysData.keyBoardSwitchLF);
					//���ƽ���
					if (sysData.system.isScpiFresh)
					{
						emit drawArbitraryWave();
					}
					break;
					//LF״̬��ȡ
				case SCPI_CMD_LF_SWITCH_GET:
					exeResult = __SCPI_RETURN;
					if (sysData.lf.state)
					{
						returnString = "ON";
					} else
					{
						returnString = "OFF";
					}
					break;
					//LF��������
				case SCPI_CMD_LF_TYPE_SET:
					exeResult = setLfType(result.value.trimmed().toUpper());
					//���ƽ���
					if (sysData.system.isScpiFresh)
					{
						emit drawArbitraryWave();
					}
					break;
					//LF���λ�ȡ
				case SCPI_CMD_LF_TYPE_GET: //IF����ѡ��
					exeResult = __SCPI_RETURN;
					switch (sysData.lf.type)
					{
						case lotSine:
							returnString = "0";
							break;
						case lotSquare:
							returnString = "1";
							break;
						case lotTriangle:
							returnString = "2";
							break;
						case lotSawtooth:
							returnString = "3";
							break;
							//case lotSinc:		returnString = "Sinc";		break;
						default:
							break;
					}
					break;
					//LFƵ������
				case SCPI_CMD_LF_FREQ_SET:
					exeResult = setLfFreq(result.value.trimmed().toUpper());
					//���ƽ���
					if (sysData.system.isScpiFresh)
					{
						emit drawArbitraryWave();
					}
					break;
					//LFƵ�ʻ�ȡ
				case SCPI_CMD_LF_FREQ_GET: //LFƵ������
					exeResult = __SCPI_RETURN;
					returnString = doubleToString(sysData.lf.freq).trimmed();
					break;
					//LF��������
				case SCPI_CMD_LF_AMPL_SET:
					exeResult = setLfAmpl(result.value.trimmed().toUpper());
					//���ƽ���
					if (sysData.system.isScpiFresh)
					{
						emit drawArbitraryWave();
					}
					break;
					//LF���Ȼ�ȡ
				case SCPI_CMD_LF_AMPL_GET: //LFƵ������
					exeResult = __SCPI_RETURN;
					sprintf(tempChar, "%.0f", sysData.lf.ampl);
					returnString = QString(tempChar).trimmed();
					break;

					//�ϱ�Ƶ ��ƵԴ����
				case SCPI_CMD_CONV_SET:
					exeResult = setExtRefType(StrToBool(result.value.trimmed().toUpper()));
					//���ƽ���
					if (sysData.system.isScpiFresh)
					{
						emit drawExfFref();
					}
					break;
					//�ϱ�Ƶ ��ƵԴ��ȡ
				case SCPI_CMD_CONV_GET:
					exeResult = __SCPI_RETURN;
					if (sysData.exfRef.type == erftInternal)
					{
						returnString = "0";
					} else if (sysData.exfRef.type == erftExternal)
					{
						returnString = "1";
					}
					break;
					//�ϱ�Ƶ �ⲿƵ������
				case SCPI_CMD_CONV_FREQ_SET:
					if (sysData.exfRef.type == erftInternal)
					{
						exeResult = __SCPI_RETURN;
						returnString = "ERR! Please select the External type!";
					} else if (sysData.exfRef.type == erftExternal)
					{
						exeResult = setExtRefFrequency(result.value.trimmed().toUpper());
					}

					//���ƽ���
					if (sysData.system.isScpiFresh)
					{
						emit drawExfFref();
					}
					break;
					//�ϱ�Ƶ �ⲿƵ�ʻ�ȡ
				case SCPI_CMD_CONV_FREQ_GET:
					exeResult = __SCPI_RETURN;
					sprintf(tempChar, "%.0f", sysData.exfRef.freq);
					returnString = QString(tempChar).trimmed();
					break;
					//DRG����
				case SCPI_CMD_DRG_STATE_SET:
					exeResult = setDrgState(result.value.trimmed().toUpper());
					if (sysData.system.isScpiFresh) //����SCPIˢ���Ƿ�����ж��Ƿ�ˢ�½���
					{
						emit drawInputParamterNoParamter();
						emit drawDrgFunction();
					}
					emit drawFreqAndAmplMain();
					break;
				case SCPI_CMD_DRG_STATE_GET:
					exeResult = __SCPI_RETURN;
					if (sysData.drg.state)
					{
						returnString = "1";
					} else
					{
						returnString = "0";
					}
					break;
				case SCPI_CMD_DRG_UPLIMIT_SET:
					exeResult = setDrgUplimit(result.value.trimmed().toUpper());
					if (sysData.system.isScpiFresh) //����SCPIˢ���Ƿ�����ж��Ƿ�ˢ�½���
					{
						emit drawInputParamterNoParamter();
						emit drawDrgFunction();
					}
					emit drawFreqAndAmplMain();
					break;
				case SCPI_CMD_DRG_UPLIMIT_GET:
					exeResult = __SCPI_RETURN;
					sprintf(tempChar, "%.0f", sysData.drg.uplimit);
					returnString = QString(tempChar).trimmed();
					break;
				case SCPI_CMD_DRG_DOWNLIMIT_SET:
					exeResult = setDrgDownlimit(result.value.trimmed().toUpper());
					if (sysData.system.isScpiFresh) //����SCPIˢ���Ƿ�����ж��Ƿ�ˢ�½���
					{
						emit drawInputParamterNoParamter();
						emit drawDrgFunction();
					}
					emit drawFreqAndAmplMain();
					break;
				case SCPI_CMD_DRG_DOWNLIMIT_GET:
					exeResult = __SCPI_RETURN;
					sprintf(tempChar, "%.0f", sysData.drg.downlimit);
					returnString = QString(tempChar).trimmed();
					break;
				case SCPI_CMD_DRG_POINTS_SET:
					exeResult = setDrgPoints(result.value.trimmed().toUpper());
					//���ƽ���
					if (sysData.system.isScpiFresh)
					{
						emit drawInputParamterNoParamter();
						emit drawDrgFunction();
					}
					break;
				case SCPI_CMD_DRG_POINTS_GET:
					exeResult = __SCPI_RETURN;
					sprintf(tempChar, "%.d", sysData.drg.points);
					returnString = QString(tempChar).trimmed();
					break;
				case SCPI_CMD_DRG_RATE_SET:
					exeResult = setDrgRate(result.value.trimmed().toUpper());
					if (sysData.system.isScpiFresh)
					{
						emit drawInputParamterNoParamter();
						emit drawDrgFunction();
					}
					break;
				case SCPI_CMD_DRG_RATE_GET:
					exeResult = __SCPI_RETURN;
					sprintf(tempChar, "%.0f", (sysData.drg.rate * 1e6));
					returnString = QString(tempChar).trimmed();
					break;
				case SCPI_CMD_DRG_WORKMODE_SET:
					exeResult = setDrgWorkmode(result.value.trimmed().toUpper());
					if (sysData.system.isScpiFresh)
					{
						emit drawInputParamterNoParamter();
						emit drawDrgFunction();
					}
					break;
				case SCPI_CMD_DRG_WORKMODE_GET:
					exeResult = __SCPI_RETURN;
					if (sysData.drg.workmode == ddnormal)
					{
						returnString = "1";
					}
					if (sysData.drg.workmode == ddpositive)
					{
						returnString = "2";
					} else if (sysData.drg.workmode == ddnegative)
					{
						returnString = "3";
					}
					break;

					//RF�������
				case SCPI_CMD_SYSTEM_RFOUT_SET:
					exeResult = __SCPI_SUCCESS;

					sysData.keyBoardSwitchRF = StrToBool(result.value);

					//				printf("keyBoardSwitchRF = %d\n",sysData.keyBoardSwitchRF);

					keyBoardLEDSwitch(3, sysData.keyBoardSwitchRF);
					sendToMSG1030QS.sprintf("%d", sysData.keyBoardSwitchRF);
					//				sendToMSG1030QS = ":SYST:RFO " + sendToMSG1030QS + "\n";
					//				strcpy(SendToMSG1030CH,sendToMSG1030QS.toLatin1());
					//				printf("%s",SendToMSG1030CH);
					sendToMSG1030QS = ":SYST:RFO " + sendToMSG1030QS;
					sendSerialData(sendToMSG1030QS);
					emit drawFreqAndAmplMain();
					break;

				case SCPI_CMD_SYSTEM_RFOUT_GET: //RF�������
					exeResult = __SCPI_RETURN;
					returnString = boolToString(sysData.keyBoardSwitchRF).trimmed();
					break;

				case SCPI_CMD_SYSTEM_MODOUT_SET: //MOD�������
					exeResult = __SCPI_SUCCESS;

					sysData.keyBoardSwitchMod = StrToBool(result.value);

					keyBoardLEDSwitch(1, sysData.keyBoardSwitchMod);
					sendToMSG1030QS.sprintf("%d", sysData.keyBoardSwitchMod);
					//				sendToMSG1030QS = ":SYST:MODO " + sendToMSG1030QS + "\n";
					//				strcpy(SendToMSG1030CH,sendToMSG1030QS.toLatin1());
					//				printf("%s",SendToMSG1030CH);
					sendToMSG1030QS = ":SYST:MODO " + sendToMSG1030QS;
					sendSerialData(sendToMSG1030QS);
					break;

				case SCPI_CMD_SYSTEM_MODOUT_GET: //MOD�������
					exeResult = __SCPI_RETURN;
					returnString = boolToString(sysData.keyBoardSwitchMod).trimmed();
					break;
					/*
					 case SCPI_CMD_SYSTEM_LFOUT_SET:      	//LF�������
					 exeResult = __SCPI_SUCCESS;

					 sysData.keyBoardSwitchLF = StrToBool(result.value);
					 keyBoardLEDSwitch(2, sysData.keyBoardSwitchLF);
					 sendToMSG1030QS.sprintf("%d", sysData.keyBoardSwitchLF);
					 sendToMSG1030QS = ":SYSTem:LFO " + sendToMSG1030QS + ";\r\n";
					 strcpy(SendToMSG1030CH,sendToMSG1030QS.toLatin1());
					 printf("%s",SendToMSG1030CH);

					 sysData.LF.Enable =  sysData.keyBoardSwitchLF;
					 emit redrawArbitraryWave();
					 break;
					 case SCPI_CMD_SYSTEM_LFOUT_GET:      	//LF�������
					 exeResult = __SCPI_RETURN;
					 returnString = boolToString(sysData.keyBoardSwitchLF).trimmed();
					 break;

					 case SCPI_CMD_FREQ_CONVERSION_TYPE_SET:
					 exeResult = setExtRefType(result.value.trimmed());
					 break;

					 case SCPI_CMD_FREQ_CONVERSION_TYPE_GET:
					 exeResult = __SCPI_RETURN;

					 switch (sysData.exfRef.type)
					 {
					 case erftInternal: returnString = "INTernal"; break;
					 case erftExternal: returnString = "EXTernal"; break;
					 default: break;
					 }
					 break;

					 case SCPI_CMD_FREQ_CONVERSION_FREQ_SET:
					 exeResult = setExtRefFrequency(result.value.trimmed());
					 break;

					 case SCPI_CMD_FREQ_CONVERSION_FREQ_GET:
					 exeResult = __SCPI_RETURN;

					 returnString = doubleToString(sysData.exfRef.freq).trimmed();
					 break;
					 */

				case SCPI_CMD_FREQ_10MHZREF_SET:
					exeResult = __SCPI_SUCCESS;
					setRef10MState(!StrToBool(result.value.toUpper()));

					//��ͼ
					if (sysData.system.isScpiFresh)
					{
						drawFreqInParamter();
					}
					break;
				case SCPI_CMD_FREQ_10MHZREF_GET:
					exeResult = __SCPI_RETURN;
					if (sysData.freq.ref10MHz == rmInternal)
					{
						returnString = QString("Internal").trimmed();
					} else if (sysData.freq.ref10MHz == rmExternal)
					{
						returnString = QString("External").trimmed();
					}
					break;
					//�����ȷ�״̬
				case SCPI_CMD_SYSTEM_ALC_SET:
					exeResult = __SCPI_SUCCESS;
					setALCState(StrToBool(result.value.trimmed().toUpper()));
					//��ͼ
					if (sysData.system.isScpiFresh)
					{
						drawAmplInParamter();
					}
					break;
					//��ȡ�ȷ�״̬
				case SCPI_CMD_SYSTEM_ALC_GET:
					exeResult = __SCPI_RETURN;
					returnString = boolToString(sysData.ampl.alc.state).trimmed();
					break;

				default:
					exeResult = __SCPI_UNSUPPORT;
					break;

					/*****************************************************************************************************/
					//�����Ǵӵײ㵥Ƭ���ϴ�������ָ��

					//�ϱ�Ƶ�����У��ⲿ�����źŷ���
				case SCPI_RCV_CMD_EXT_IF_AMPL:
					exeResult = __SCPI_SUCCESS;
					saveQvalueFromModule(result.value.trimmed());
					//���ƽ���
					switch (nowSubKey->funcCode)
					//��ָ���ڡ��ϱ�Ƶ�������еġ����ء���ƵԴѡ��Ƶ��ʱˢ�½��桱
					{
						case KEY_FUNC_FCTN_EXTRF_RETURN:
						case KEY_FUNC_FCTN_EXTRF_EXTRF:
						case KEY_FUNC_FCTN_EXTRF_FREQ:
							emit drawExfFref();
							break;
						default:
							break;
					}
					break;

					//ģ�����к�
				case SCPI_RCV_SERIAL_NUM:
					exeResult = __SCPI_SUCCESS;
					moduSerial = QString("BL") + result.value.trimmed();
					break;
					//ģ������汾��
				case SCPI_RCV_MODU_SOFT_VER:
					exeResult = __SCPI_SUCCESS;
					moduSoftVer = result.value.trimmed();
					break;
					//ģ���豸����
				case SCPI_RCV_MODU_DEV_TYPE:
					exeResult = __SCPI_SUCCESS;
					moduDevType = result.value.trimmed();
					break;

					//zpc add test
				case SCPI_CMD_STANDARD_IDNN_QUERY_SET:
					exeResult = __SCPI_SUCCESS;
					sendSerialData(QString("*IDNN?"));
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
		}
	}
}

//����SCPI���
void tSysScpi::returnScpiResult(QString value)
{
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
			sendLength = clientSocket->write(sendString.toStdString().c_str());
			sysData.scpiData.stateT = false;

			if (!clientSocket->waitForBytesWritten(30000) || sendLength != dataLength)
			{
				//clientSocketDisConnect();
			}

			break;

		case sitUsb:
			sysData.scpiData.stateT = true;
			sendLength = write(usbHandle, sendString.toStdString().c_str(), sendString.length() * sizeof(char));
			sysData.scpiData.stateT = false;
			break;
		case sitCom:
			sendSerialData(sendString.toStdString().c_str());
			break;
	}
}

//����SCPI���
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
			clientSocket->write(value);
			clientSocket->write((char*) SCPIRESULTEOF);
			sysData.scpiData.stateT = false;
			return __SCPI_SUCCESS;
			break;
		case sitUsb:
			break;
	}

	return __SCPI_FAILED;
}

