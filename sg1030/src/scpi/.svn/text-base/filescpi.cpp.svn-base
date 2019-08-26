#include "sysscpi.h"

//获取设备配置
void tSysScpi::loadDeviceConfig(void)
{
  sysData.system.isScpiScreenRefresh = true;

  sysData.system.lanMacAddrIndex = 0;
  sysData.system.lanMacAddress[0] = 0xe;
  sysData.system.lanMacAddress[1] = 0x6;
  sysData.system.lanMacAddress[2] = 0x6;
  sysData.system.lanMacAddress[3] = 0xd;
  sysData.system.lanMacAddress[4] = 0x8;
  sysData.system.lanMacAddress[5] = 0xd;
  sysData.system.lanMacAddress[6] = 0xa;
  sysData.system.lanMacAddress[7] = 0x3;
  sysData.system.lanMacAddress[8] = 0x5;
  sysData.system.lanMacAddress[9] = 0x3;
  sysData.system.lanMacAddress[10] = 0x7;
  sysData.system.lanMacAddress[11] = 0xb;

  QString fileName = QCoreApplication::applicationDirPath() + "/setting.ini";
  QString macAddr = "e66d8da3537b";

  if (QFile(fileName).exists())
  {
    QSettings setting(fileName, QSettings::IniFormat);

    setting.beginGroup("device");
    strcpy( sysData.system.serialNo ,setting.value("serialNo", sysData.system.serialNo).toString().toStdString().c_str());

    strcpy( sysData.system.io.lan.ip ,setting.value("lanIp", sysData.system.io.lan.ip).toString().toStdString().c_str());
    strcpy( sysData.system.io.lan.mask ,setting.value("lanMask", sysData.system.io.lan.mask).toString().toStdString().c_str());
    strcpy( sysData.system.io.lan.gate ,setting.value("lanGate", sysData.system.io.lan.gate).toString().toStdString().c_str());

    macAddr = setting.value("Mac", macAddr).toString();

    setting.endGroup();

  }

  if (!macAddr.isNull() && !macAddr.isEmpty())
  {
	QString hexStr = "";

    for (int i = 0; i < macAddr.length(); i++)
    {
      hexStr = "0x";
      hexStr += macAddr.at(i);
      sysData.system.lanMacAddress[i] = hexToI((char*)hexStr.toStdString().c_str());
    }
  }

//  if (ifHandle >= 0)
//  {
//	  ioctl(ifHandle,0xf5,int(175- 1.75* sysData.system.backLight));
//  }
}

//设置设备属性
void tSysScpi::saveDeviceConfig(void)
{
	QString macAddr = "";
	QString fileName = QCoreApplication::applicationDirPath() + "/setting.ini";
	QSettings setting(fileName, QSettings::IniFormat);
	char tempChar[8];

	for (int i = 0; i < sizeof sysData.system.lanMacAddress / sizeof sysData.system.lanMacAddress[0]; i++)
	{
	  macAddr += QString(intToHex(sysData.system.lanMacAddress[i], tempChar)).trimmed();
	}

	setting.beginGroup("device");

	setting.setValue("serialNo", sysData.system.serialNo);

	setting.setValue("lanIp", sysData.system.io.lan.ip);
	setting.setValue("lanMask", sysData.system.io.lan.mask);
	setting.setValue("lanGate", sysData.system.io.lan.gate);

	setting.setValue("Mac", macAddr);
	setting.endGroup();

    //language
    setting.beginGroup("system");

    setting.setValue("language", sysData.system.isLanguageEn);
//    saveSystemSetting("system", "language", "english");
    setting.endGroup();
}

