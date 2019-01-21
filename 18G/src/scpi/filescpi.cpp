#include "sysscpi.h"

//获取设备配置
void tSysScpi::loadDeviceConfig(void)
{
	sysData.system.isScpiScreenRefresh = true;
	sysData.system.datetime.isVisible = true;
	sysData.system.datetime.style = dtsYMDHMS;
	sysData.system.io.gpib.addr = 18;
	sysData.system.backLight = 50;
	//sysData.system.serialNo = 200001001;
	sysData.system.lanMacAddrIndex = 0;
	//  sysData.system.lanMacAddress[0] = 0xe;
	//  sysData.system.lanMacAddress[1] = 0x6;
	//  sysData.system.lanMacAddress[2] = 0x6;
	//  sysData.system.lanMacAddress[3] = 0xd;
	//  sysData.system.lanMacAddress[4] = 0x8;
	//  sysData.system.lanMacAddress[5] = 0xd;
	//  sysData.system.lanMacAddress[6] = 0xa;
	//  sysData.system.lanMacAddress[7] = 0x3;
	//  sysData.system.lanMacAddress[8] = 0x5;
	//  sysData.system.lanMacAddress[9] = 0x3;
	//  sysData.system.lanMacAddress[10] = 0x7;
	//  sysData.system.lanMacAddress[11] = 0xb;
	strncpy((char *) sysData.system.lanMacAddress, MACADDRESS, sizeof(sysData.system.lanMacAddress));
	sysData.system.powerOnPreset.powerOnParam = spFactory;
	sysData.system.powerOnPreset.presetParam = spFactory;
	sysData.system.io.com.baudrate = br115200;
	sysData.system.io.com.databits = db8;
	sysData.system.io.com.stopbits = sb1;
	sysData.system.io.com.check = cmNone;
	sysData.system.io.com.flow = fcNone;

	char tempChar[8];

	QString lcdFileName = QCoreApplication::applicationDirPath() + "/lcdBacklight.ini";
	if (QFile(lcdFileName).exists())
	{
		QSettings setting(lcdFileName, QSettings::IniFormat);
		setting.beginGroup("backLight");
		sysData.system.backLight = setting.value("backLight", sysData.system.backLight).toInt();
		setting.endGroup();
	}

	QString fileName = QCoreApplication::applicationDirPath() + "/setting.ini";
	QString macAddr = MACADDRESS;

	if (QFile(fileName).exists())
	{
		QSettings setting(fileName, QSettings::IniFormat);

		setting.beginGroup("device");
		sysData.options.isTrack36GHzOn = setting.value("isTrack36On", 0).toBool();
		//    sysData.system.backLight = setting.value("backLight", sysData.system.backLight).toInt();
		//    sysData.system.serialNo = setting.value("serialNo", sysData.system.serialNo).toLongLong();
		strcpy(sysData.system.serialNo, setting.value("serialNo", sysData.system.serialNo).toString().toStdString().c_str());
		sysData.system.io.gpib.addr = setting.value("gpibMainAddr", sysData.system.io.gpib.addr).toInt();

		//    sysData.system.io.lan.hostPort = setting.value("hostPort", sysData.system.io.lan.hostPort).toInt();
		//    strcpy( sysData.system.io.lan.hostIp ,setting.value("hostIp", sysData.system.io.lan.hostIp).toString().toStdString().c_str());
		strcpy(sysData.system.io.lan.ip, setting.value("lanIp", sysData.system.io.lan.ip).toString().toStdString().c_str());
		strcpy(sysData.system.io.lan.mask, setting.value("lanMask", sysData.system.io.lan.mask).toString().toStdString().c_str());
		strcpy(sysData.system.io.lan.gate, setting.value("lanGate", sysData.system.io.lan.gate).toString().toStdString().c_str());

		macAddr = setting.value("Mac", macAddr).toString();
		sysData.system.isLogOn = setting.value("isLogOn", 1).toBool();
		sysData.system.isGpibOn = setting.value("isGpibOn", 0).toBool();
		sysData.system.powerOnPreset.powerOnParam = systemParam(setting.value("powerOnParam", 0).toInt());
		sysData.system.powerOnPreset.presetParam = systemParam(setting.value("presetParam", 0).toInt());
		sysData.system.isScpiScreenRefresh = setting.value("isScpiScreenRefresh", true).toBool();
		sysData.system.datetime.isVisible = setting.value("datetimevisible", 1).toBool();
		sysData.system.datetime.style = dateTimeStyle(setting.value("datetimeformat", dtsYMDHMS).toInt());
		setting.endGroup();

		setting.beginGroup("serial");
		sysData.system.io.com.baudrate = baudRate(setting.value("baudrate", br115200).toInt());
		sysData.system.io.com.databits = dataBits(setting.value("databits", db8).toInt());
		sysData.system.io.com.stopbits = stopBits(setting.value("stopbits", sb1).toInt());
		sysData.system.io.com.check = checkMode(setting.value("check", cmNone).toInt());
		sysData.system.io.com.flow = flowControl(setting.value("flow", fcNone).toInt());
		setting.endGroup();

		setting.beginGroup("options");
		sysData.options.isFft10HzOn = setting.value("fft", 0).toBool();
		sysData.options.isDemodOn = setting.value("demod", 0).toBool();
		sysData.options.directChannelOptionOn = setting.value("directChannel", 0).toBool();
		setting.endGroup();

		setting.beginGroup("demod");
		for (int i = 0; i < sizeof sysData.demod.radioFreqSet / sizeof sysData.demod.radioFreqSet[0]; i++)
		{
			sysData.demod.radioFreqSet[i] = setting.value("radioFreqSet_" + QString(intToString(i, tempChar)).trimmed(), sysData.demod.radioFreqSet[i]).toDouble();
		}
		setting.endGroup();
	}

	if (!macAddr.isNull() && !macAddr.isEmpty())
	{
		QString hexStr = "";

		for (int i = 0; i < macAddr.length(); i++)
		{
			hexStr = "0x";
			hexStr += macAddr.at(i);
			sysData.system.lanMacAddress[i] = hexToI((char*) hexStr.toStdString().c_str());
		}
	}

	if (ifHandle >= 0)
	{
		ioctl(ifHandle, 0xf5, int(175 - 1.75 * sysData.system.backLight));
	}
}

//设置设备属性
void tSysScpi::saveDeviceConfig(void)
{
	QString fileName = QCoreApplication::applicationDirPath() + "/setting.ini";
	QString macAddr = "";
	QSettings setting(fileName, QSettings::IniFormat);
	char tempChar[8];

	for (int i = 0; i < sizeof sysData.system.lanMacAddress / sizeof sysData.system.lanMacAddress[0]; i++)
	{
		macAddr += QString(intToHex(sysData.system.lanMacAddress[i], tempChar)).trimmed();
	}

	setting.beginGroup("device");
	setting.setValue("isTrack36On", sysData.options.isTrack36GHzOn);
	setting.setValue("serialNo", sysData.system.serialNo);
	//  setting.setValue("serialNoMode", sysData.system.isSerialNoOldModeOn);
	//  setting.setValue("backLight", sysData.system.backLight);

	//  setting.setValue("hostPort", sysData.system.io.lan.hostPort);
	//  setting.setValue("hostIp", sysData.system.io.lan.hostIp);
	setting.setValue("lanIp", sysData.system.io.lan.ip);
	setting.setValue("lanMask", sysData.system.io.lan.mask);
	setting.setValue("lanGate", sysData.system.io.lan.gate);

	setting.setValue("gpibMainAddr", sysData.system.io.gpib.addr);
	setting.setValue("Mac", macAddr);
	setting.setValue("isLogOn", sysData.system.isLogOn);
	setting.setValue("isGpibOn", sysData.system.isGpibOn);
	setting.setValue("powerOnParam", sysData.system.powerOnPreset.powerOnParam);
	setting.setValue("presetParam", sysData.system.powerOnPreset.presetParam);
	setting.setValue("isScpiScreenRefresh", sysData.system.isScpiScreenRefresh);
	setting.setValue("datetimevisible", sysData.system.datetime.isVisible);
	setting.setValue("datetimeformat", sysData.system.datetime.style);
	setting.endGroup();

	setting.beginGroup("serial");
	setting.setValue("baudrate", sysData.system.io.com.baudrate);
	setting.setValue("databits", sysData.system.io.com.databits);
	setting.setValue("stopbits", sysData.system.io.com.stopbits);
	setting.setValue("check", sysData.system.io.com.check);
	setting.setValue("flow", sysData.system.io.com.flow);
	setting.endGroup();

	setting.beginGroup("options");
	setting.setValue("fft", sysData.options.isFft10HzOn);
	setting.setValue("demod", sysData.options.isDemodOn);
	setting.setValue("directChannel", sysData.options.directChannelOptionOn);
	setting.endGroup();

	setting.beginGroup("demod");
	for (int i = 0; i < sizeof sysData.demod.radioFreqSet / sizeof sysData.demod.radioFreqSet[0]; i++)
	{
		setting.setValue("radioFreqSet_" + QString(intToString(i, tempChar)).trimmed(), sysData.demod.radioFreqSet[i]);
	}
	setting.endGroup();
}

//保存LCD背光
void tSysScpi::saveLCDBacklight(void)
{
	QString fileName = QCoreApplication::applicationDirPath() + "/lcdBacklight.ini";
	QSettings setting(fileName, QSettings::IniFormat);

	setting.beginGroup("backLight");
	setting.setValue("backLight", sysData.system.backLight);
	setting.endGroup();
}

//获取出厂校准数据
void tSysScpi::getFactoryCalibrationData(void)
{
	resetFactoryCalibrationData();

	char tempChar[32] = { };
	QString fileName = QCoreApplication::applicationDirPath() + "/setting.ini";

	if (QFile(fileName).exists())
	{
		QSettings setting(fileName, QSettings::IniFormat);

		setting.beginGroup("factoryData");

		sysData.factoryCalData.isCaled = setting.value("isCaled", false).toBool();

		if (sysData.factoryCalData.isCaled)
		{
			sysData.factoryCalData.absoluteAmptValue = setting.value("absoluteAmptValue", 0).toDouble();
			sysData.factoryCalData.temperature = setting.value("temperature", 0).toDouble();

			sysData.factoryCalData.att9 = setting.value("att9", 0).toDouble();
			sysData.factoryCalData.att0 = setting.value("att0", 0).toDouble();
			sysData.factoryCalData.att21 = setting.value("att21", 0).toDouble();
			sysData.factoryCalData.att30 = setting.value("att30", 0).toDouble();
			sysData.factoryCalData.att39 = setting.value("att39", 0).toDouble();

			sysData.factoryCalData.if0 = setting.value("if0", 0).toDouble();
			sysData.factoryCalData.if10 = setting.value("if10", 0).toDouble();
			sysData.factoryCalData.if20 = setting.value("if20", 0).toDouble();
			sysData.factoryCalData.if30 = setting.value("if30", 0).toDouble();

			for (int i = 0; i < sizeof BWRESP / sizeof BWRESP[0]; i++)
			{
				sysData.factoryCalData.rbw[i] = setting.value("rbw" + QString(intToString(i, tempChar)).trimmed(), 0).toDouble();
			}

//			for (int i = 0; i < sizeof BWRESP_FFT / sizeof BWRESP_FFT[0]; i++)
//			{
//				sysData.factoryCalData.rbw_fft[i] = setting.value("rbw_fft" + QString(intToString(i, tempChar)).trimmed(), 0).toDouble();
//			}

			for(int i = 0; i < 32; i++)
			{
				sysData.factoryCalData.attIf[i] = setting.value("attIf_" + QString(intToString(i, tempChar)).trimmed(), 0).toDouble();
			}

			for (int i = 0; i < 5; i++)
			{
				for (int j = 0; j < sizeof FREQRESPHZ / sizeof FREQRESPHZ[0]; j++)
				{
					sysData.factoryCalData.freqResp[i][j] = setting.value("freqResp_" + QString(intToString(i, tempChar)).trimmed() + "_" + QString(intToString(j, tempChar)).trimmed(), 0).toDouble();
				}
			}
		}

		setting.endGroup();
	}
}
//获取前置放大校准数据
void tSysScpi::getPreamplifierCalibrationData(void)
{
	resetPreamplifierCalibrationData();

	char tempChar[32] = { };
	QString fileName = QCoreApplication::applicationDirPath() + "/setting.ini";

	if (QFile(fileName).exists())
	{
		QSettings setting(fileName, QSettings::IniFormat);

		setting.beginGroup("preamplifierCalData");
		sysData.preamplifierCalData.isCaled = setting.value("isCaled", false).toBool();

		if (sysData.preamplifierCalData.isCaled)
		{
			sysData.preamplifierCalData.absoluteAmptValue = setting.value("absoluteAmptValue", 0).toDouble();
			sysData.preamplifierCalData.temperature = setting.value("temperature", 0).toDouble();

			sysData.preamplifierCalData.att9 = setting.value("att9", 0).toDouble();
			sysData.preamplifierCalData.att0 = setting.value("att0", 0).toDouble();
			sysData.preamplifierCalData.att21 = setting.value("att21", 0).toDouble();
			sysData.preamplifierCalData.att30 = setting.value("att30", 0).toDouble();
			sysData.preamplifierCalData.att39 = setting.value("att39", 0).toDouble();

			sysData.preamplifierCalData.if0 = setting.value("if0", 0).toDouble();
			sysData.preamplifierCalData.if10 = setting.value("if10", 0).toDouble();
			sysData.preamplifierCalData.if20 = setting.value("if20", 0).toDouble();
			sysData.preamplifierCalData.if30 = setting.value("if30", 0).toDouble();

			for (int i = 0; i < sizeof BWRESP / sizeof BWRESP[0]; i++)
			{
				sysData.preamplifierCalData.rbw[i] = setting.value("rbw" + QString(intToString(i, tempChar)).trimmed(), 0).toDouble();
			}

			for (int i = 0; i < sizeof BWRESP_FFT / sizeof BWRESP_FFT[0]; i++)
			{
				sysData.preamplifierCalData.rbw_fft[i] = setting.value("rbw_fft" + QString(intToString(i, tempChar)).trimmed(), 0).toDouble();
			}

			for (int i = 0; i < 5; i++)
			{
				for (int j = 0; j < sizeof FREQRESPHZ / sizeof FREQRESPHZ[0]; j++)
				{
					sysData.preamplifierCalData.freqResp[i][j] = setting.value("freqResp_" + QString(intToString(i, tempChar)).trimmed() + "_" + QString(intToString(j, tempChar)).trimmed(), 0).toDouble();
				}
			}
		}

		setting.endGroup();
	}
}

//获取直采校准数据
void tSysScpi::getZcCalibrationData(void)
{
	resetZcCalibrationData();

	char tempChar[32] = { };
	QString fileName = QCoreApplication::applicationDirPath() + "/setting.ini";

	if (QFile(fileName).exists())
	{
		QSettings setting(fileName, QSettings::IniFormat);

		setting.beginGroup("ZcCalData");
		sysData.zcCalData.isCaled = setting.value("isCaled", false).toBool();

		if (sysData.zcCalData.isCaled)
		{
			sysData.zcCalData.absoluteAmptValue = setting.value("absoluteAmptValue", 0).toDouble();
			sysData.zcCalData.temperature = setting.value("temperature", 0).toDouble();

			sysData.zcCalData.att9 = setting.value("att9", 0).toDouble();
			sysData.zcCalData.att0 = setting.value("att0", 0).toDouble();
			sysData.zcCalData.att21 = setting.value("att21", 0).toDouble();
			sysData.zcCalData.att30 = setting.value("att30", 0).toDouble();
			sysData.zcCalData.att39 = setting.value("att39", 0).toDouble();

			sysData.zcCalData.if0 = setting.value("if0", 0).toDouble();
			sysData.zcCalData.if10 = setting.value("if10", 0).toDouble();
			sysData.zcCalData.if20 = setting.value("if20", 0).toDouble();
			sysData.zcCalData.if30 = setting.value("if30", 0).toDouble();

			for (int i = 0; i < sizeof BWRESP_ZC / sizeof BWRESP_ZC[0]; i++)
			{
				sysData.zcCalData.rbw[i] = setting.value("rbw" + QString(intToString(i, tempChar)).trimmed(), 0).toDouble();
			}

			//      for (int i = 0; i < sizeof BWRESP_FFT / sizeof BWRESP_FFT[0]; i++)
			//      {
			//   	    sysData.zcCalData.rbw_fft[i] = setting.value("rbw_fft" + QString(intToString(i, tempChar)).trimmed(), 0).toDouble();
			//      }

			for (int i = 0; i < 5; i++)
			{
				for (int j = 0; j < sizeof FREQRESPHZ_ZC / sizeof FREQRESPHZ_ZC[0]; j++)
				{
					sysData.zcCalData.freqResp[i][j] = setting.value("freqResp_" + QString(intToString(i, tempChar)).trimmed() + "_" + QString(intToString(j, tempChar)).trimmed(), 0).toDouble();
				}
			}
		}

		setting.endGroup();
	}
}

//获取直采通道前置放大校准数据
void tSysScpi::getZcPreamplifierCalibrationData(void)
{
	resetZcPreamplifierCalibrationData();

	char tempChar[32] = { };
	QString fileName = QCoreApplication::applicationDirPath() + "/setting.ini";

	if (QFile(fileName).exists())
	{
		QSettings setting(fileName, QSettings::IniFormat);

		setting.beginGroup("ZcPreamplifierCalData");
		sysData.zcPreamplifierCalData.isCaled = setting.value("isCaled", false).toBool();

		if (sysData.zcPreamplifierCalData.isCaled)
		{
			sysData.zcPreamplifierCalData.absoluteAmptValue = setting.value("absoluteAmptValue", 0).toDouble();
			sysData.zcPreamplifierCalData.temperature = setting.value("temperature", 0).toDouble();

			sysData.zcPreamplifierCalData.att9 = setting.value("att9", 0).toDouble();
			sysData.zcPreamplifierCalData.att0 = setting.value("att0", 0).toDouble();
			sysData.zcPreamplifierCalData.att21 = setting.value("att21", 0).toDouble();
			sysData.zcPreamplifierCalData.att30 = setting.value("att30", 0).toDouble();
			sysData.zcPreamplifierCalData.att39 = setting.value("att39", 0).toDouble();

			sysData.zcPreamplifierCalData.if0 = setting.value("if0", 0).toDouble();
			sysData.zcPreamplifierCalData.if10 = setting.value("if10", 0).toDouble();
			sysData.zcPreamplifierCalData.if20 = setting.value("if20", 0).toDouble();
			sysData.zcPreamplifierCalData.if30 = setting.value("if30", 0).toDouble();

			for (int i = 0; i < sizeof BWRESP_ZC / sizeof BWRESP_ZC[0]; i++)
			{
				sysData.zcPreamplifierCalData.rbw[i] = setting.value("rbw" + QString(intToString(i, tempChar)).trimmed(), 0).toDouble();
			}

			//      for (int i = 0; i < sizeof BWRESP_FFT / sizeof BWRESP_FFT[0]; i++)
			//      {
			//   	    sysData.zcPreamplifierCalData.rbw_fft[i] = setting.value("rbw_fft" + QString(intToString(i, tempChar)).trimmed(), 0).toDouble();
			//      }

			for (int i = 0; i < 5; i++)
			{
				for (int j = 0; j < sizeof FREQRESPHZ_ZC / sizeof FREQRESPHZ_ZC[0]; j++)
				{
					sysData.zcPreamplifierCalData.freqResp[i][j] = setting.value("freqResp_" + QString(intToString(i, tempChar)).trimmed() + "_" + QString(intToString(j, tempChar)).trimmed(), 0).toDouble();
				}
			}
		}

		setting.endGroup();
	}
}

//获取用户校准数据
void tSysScpi::getUserCalibrationData(void)
{
	memset(&sysData.userCalData, 0, sizeof sysData.userCalData);
	sysData.ampt.LMPValue = 0;

	QString fileName = QCoreApplication::applicationDirPath() + "/setting.ini";

	if (QFile(fileName).exists())
	{
		QSettings setting(fileName, QSettings::IniFormat);

		setting.beginGroup("impedance");
		sysData.ampt.LMPValue = setting.value("lmp", 0).toDouble();
		setting.endGroup();

		setting.beginGroup("userData");
		sysData.userCalData.temperature = setting.value("temperature", 0).toDouble();
		sysData.userCalData.absError = setting.value("absError", 0).toDouble();
		sysData.userCalData.if10dB = setting.value("if10dB", 0).toDouble();
		sysData.userCalData.if0dB = setting.value("if0dB", 0).toDouble();
		sysData.userCalData.if20dB = setting.value("if20dB", 0).toDouble();
		sysData.userCalData.if30dB = setting.value("if30dB", 0).toDouble();
		setting.endGroup();
	}
}

//保存75欧姆阻抗转换补偿值
void tSysScpi::saveImpedanceValue(void)
{
	QString fileName = QCoreApplication::applicationDirPath() + "/setting.ini";
	QSettings setting(fileName, QSettings::IniFormat);

	setting.beginGroup("impedance");
	setting.setValue("lmp", sysData.ampt.LMPValue);
	setting.endGroup();
}

//保存出厂校准数据
void tSysScpi::saveFactoryCalibrationData(void)
{
	char tempChar[32] = { };
	QString fileName = QCoreApplication::applicationDirPath() + "/setting.ini";
	QSettings setting(fileName, QSettings::IniFormat);

	setting.beginGroup("factoryData");
	setting.setValue("isCaled", sysData.factoryCalData.isCaled);
	setting.setValue("absoluteAmptValue", sysData.factoryCalData.absoluteAmptValue);
	setting.setValue("temperature", sysData.factoryCalData.temperature);

	setting.setValue("att9", sysData.factoryCalData.att9);
	setting.setValue("att0", sysData.factoryCalData.att0);
	setting.setValue("att21", sysData.factoryCalData.att21);
	setting.setValue("att30", sysData.factoryCalData.att30);
	setting.setValue("att39", sysData.factoryCalData.att39);

	setting.setValue("if0", sysData.factoryCalData.if0);
	setting.setValue("if10", sysData.factoryCalData.if10);
	setting.setValue("if20", sysData.factoryCalData.if20);
	setting.setValue("if30", sysData.factoryCalData.if30);

	for (int i = 0; i < 24; i++)
	{
		setting.setValue("rbw" + QString(intToString(i, tempChar)).trimmed(), sysData.factoryCalData.rbw[i]);
	}

//	for (int i = 0; i < sizeof BWRESP_FFT / sizeof BWRESP_FFT[0]; i++)
//	{
//		setting.setValue("rbw_fft" + QString(intToString(i, tempChar)).trimmed(), sysData.factoryCalData.rbw_fft[i]);
//	}

	for(int i = 0; i < 32; i++)
	{
		setting.setValue("attIf_" + QString(intToString(i, tempChar)).trimmed(), sysData.factoryCalData.attIf[i]);
	}

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < sizeof FREQRESPHZ / sizeof FREQRESPHZ[0]; j++)
		{
			setting.setValue("freqResp_" + QString(intToString(i, tempChar)).trimmed() + "_" + QString(intToString(j, tempChar)).trimmed(), sysData.factoryCalData.freqResp[i][j]);
		}
	}

	setting.endGroup();
}

//保存直采出厂校准数据
void tSysScpi::saveZcFactoryCalibrationData(void)
{
	char tempChar[32] = { };
	QString fileName = QCoreApplication::applicationDirPath() + "/setting.ini";
	QSettings setting(fileName, QSettings::IniFormat);

	setting.beginGroup("ZcCalData");
	setting.setValue("isCaled", sysData.zcCalData.isCaled);
	setting.setValue("absoluteAmptValue", sysData.zcCalData.absoluteAmptValue);
	setting.setValue("temperature", sysData.zcCalData.temperature);

	setting.setValue("att9", sysData.zcCalData.att9);
	setting.setValue("att0", sysData.zcCalData.att0);
	setting.setValue("att21", sysData.zcCalData.att21);
	setting.setValue("att30", sysData.zcCalData.att30);
	setting.setValue("att39", sysData.zcCalData.att39);

	setting.setValue("if0", sysData.zcCalData.if0);
	setting.setValue("if10", sysData.zcCalData.if10);
	setting.setValue("if20", sysData.zcCalData.if20);
	setting.setValue("if30", sysData.zcCalData.if30);

	for (int i = 0; i < sizeof BWRESP_ZC / sizeof BWRESP_ZC[0]; i++)
	{
		setting.setValue("rbw" + QString(intToString(i, tempChar)).trimmed(), sysData.zcCalData.rbw[i]);
	}

	//  for (int i = 0; i < sizeof BWRESP_FFT / sizeof BWRESP_FFT[0]; i++)
	//  {
	//	setting.setValue("rbw_fft" + QString(intToString(i, tempChar)).trimmed(), sysData.zcCalData.rbw_fft[i]);
	//  }


	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < sizeof FREQRESPHZ_ZC / sizeof FREQRESPHZ_ZC[0]; j++)
		{
			setting.setValue("freqResp_" + QString(intToString(i, tempChar)).trimmed() + "_" + QString(intToString(j, tempChar)).trimmed(), sysData.zcCalData.freqResp[i][j]);
		}
	}

	setting.endGroup();
}

//保存直采通道前置放大校准数据
void tSysScpi::saveZcPreamplifierFactoryCalibrationData(void)
{
	char tempChar[32] = { };
	QString fileName = QCoreApplication::applicationDirPath() + "/setting.ini";
	QSettings setting(fileName, QSettings::IniFormat);

	setting.beginGroup("ZcPreamplifierCalData");
	setting.setValue("isCaled", sysData.zcPreamplifierCalData.isCaled);
	setting.setValue("absoluteAmptValue", sysData.zcPreamplifierCalData.absoluteAmptValue);
	setting.setValue("temperature", sysData.zcPreamplifierCalData.temperature);

	setting.setValue("att9", sysData.zcPreamplifierCalData.att9);
	setting.setValue("att0", sysData.zcPreamplifierCalData.att0);
	setting.setValue("att21", sysData.zcPreamplifierCalData.att21);
	setting.setValue("att30", sysData.zcPreamplifierCalData.att30);
	setting.setValue("att39", sysData.zcPreamplifierCalData.att39);

	setting.setValue("if0", sysData.zcPreamplifierCalData.if0);
	setting.setValue("if10", sysData.zcPreamplifierCalData.if10);
	setting.setValue("if20", sysData.zcPreamplifierCalData.if20);
	setting.setValue("if30", sysData.zcPreamplifierCalData.if30);

	for (int i = 0; i < sizeof BWRESP_ZC / sizeof BWRESP_ZC[0]; i++)
	{
		setting.setValue("rbw" + QString(intToString(i, tempChar)).trimmed(), sysData.zcPreamplifierCalData.rbw[i]);
	}

	//  for (int i = 0; i < sizeof BWRESP_FFT / sizeof BWRESP_FFT[0]; i++)
	//  {
	//	setting.setValue("rbw_fft" + QString(intToString(i, tempChar)).trimmed(), sysData.zcPreamplifierCalData.rbw_fft[i]);
	//  }


	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < sizeof FREQRESPHZ_ZC / sizeof FREQRESPHZ_ZC[0]; j++)
		{
			setting.setValue("freqResp_" + QString(intToString(i, tempChar)).trimmed() + "_" + QString(intToString(j, tempChar)).trimmed(), sysData.zcPreamplifierCalData.freqResp[i][j]);
		}
	}

	setting.endGroup();
}

//保存前置放大校准数据
void tSysScpi::savePreamplifierCalibrationData(void)
{
	char tempChar[32] = { };
	QString fileName = QCoreApplication::applicationDirPath() + "/setting.ini";
	QSettings setting(fileName, QSettings::IniFormat);

	setting.beginGroup("preamplifierCalData");
	setting.setValue("isCaled", sysData.preamplifierCalData.isCaled);
	setting.setValue("absoluteAmptValue", sysData.preamplifierCalData.absoluteAmptValue);
	setting.setValue("temperature", sysData.preamplifierCalData.temperature);

	setting.setValue("att9", sysData.preamplifierCalData.att9);
	setting.setValue("att0", sysData.preamplifierCalData.att0);
	setting.setValue("att21", sysData.preamplifierCalData.att21);
	setting.setValue("att30", sysData.preamplifierCalData.att30);
	setting.setValue("att39", sysData.preamplifierCalData.att39);

	setting.setValue("if0", sysData.preamplifierCalData.if0);
	setting.setValue("if10", sysData.preamplifierCalData.if10);
	setting.setValue("if20", sysData.preamplifierCalData.if20);
	setting.setValue("if30", sysData.preamplifierCalData.if30);

	for (int i = 0; i < sizeof BWRESP / sizeof BWRESP[0]; i++)
	{
		setting.setValue("rbw" + QString(intToString(i, tempChar)).trimmed(), sysData.preamplifierCalData.rbw[i]);
	}

	for (int i = 0; i < sizeof BWRESP_FFT / sizeof BWRESP_FFT[0]; i++)
	{
		setting.setValue("rbw_fft" + QString(intToString(i, tempChar)).trimmed(), sysData.preamplifierCalData.rbw_fft[i]);
	}

	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < sizeof FREQRESPHZ / sizeof FREQRESPHZ[0]; j++)
		{
			setting.setValue("freqResp_" + QString(intToString(i, tempChar)).trimmed() + "_" + QString(intToString(j, tempChar)).trimmed(), sysData.preamplifierCalData.freqResp[i][j]);
		}
	}

	setting.endGroup();
}

//保存用户校准数据
void tSysScpi::saveUserCalibrationData(void)
{
	QString fileName = QCoreApplication::applicationDirPath() + "/setting.ini";
	QSettings setting(fileName, QSettings::IniFormat);

	setting.beginGroup("userData");
	setting.setValue("temperature", sysData.userCalData.temperature);
	setting.setValue("absError", sysData.userCalData.absError);
	setting.setValue("if10dB", sysData.userCalData.if10dB);
	setting.setValue("if0dB", sysData.userCalData.if0dB);
	setting.setValue("if20dB", sysData.userCalData.if20dB);
	setting.setValue("if30dB", sysData.userCalData.if30dB);
	setting.endGroup();
}

//保存系统日志
void tSysScpi::saveSystemLog(QString log)
{
	if (log.isNull() || log.isEmpty())
	{
		return;
	}

	QFile file(QCoreApplication::applicationDirPath() + "/log.txt");
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

	strList << log;

	while (strList.count() > MAXLOGCOUNT)
	{
		strList.removeAt(0);
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
}

//保存出厂校准日志
void tSysScpi::saveLogOfFactoryCalibrate(QString datetime, QString result, QString reason)
{
	QString log = datetime;

	log += 0x0d;
	log += result;
	log += 0x0d;
	log += reason;

	saveSystemLog(log);
	printf("%s!\n",reason.trimmed().toStdString().c_str());
	usleep(1000);
}

//载入出厂校准日志
QStringList tSysScpi::loadLogOfFactoryCalibrate(void)
{
	QFile file(QCoreApplication::applicationDirPath() + "/log.txt");
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
	}

	return strList;
}
