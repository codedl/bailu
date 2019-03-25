#include "mainform.h"

extern int keyCount;

void sigio(int);

//构造函数
mainForm::mainForm(QWidget* parent) :
	QWidget(parent = 0)
{
	//
	//  QTime t;
	//  t.restart();
	//  for (int i = 1; i < 65535; i++) Log10(i);
	//  t.restart();
	//  for (int i = 1; i < 65535; i++) log10(i);
	//
	//  t.restart();
	//  for (int i = 1; i < 2048; i++) Log10(i);
	//  t.restart();
	//  for (int i = 1; i < 2048; i++) log10(i);

	//窗口配置
	setWindowFlags(Qt::FramelessWindowHint);
	setMinimumSize(SCREENWIDTH, SCREENHEIGHT);
	setMaximumSize(SCREENWIDTH, SCREENHEIGHT);
	resize(QSize(SCREENWIDTH, SCREENHEIGHT).expandedTo(minimumSizeHint()));

	//载入设置
	sysErrValue = 0;
	sysLocked = false;
	img_Paused = false;
	img_Paused_Zoom = false;
	img_Paused_Zoom_Full = false;
	sysData.temperature = 0;
	sysData.ampt.LMPValue = 0;
	indexOfPassword = 0;
	memset(passwdString, 0, sizeof passwdString);
	loadSystemSetting();

	//static text of file
	stLabelFileDiskSpace.setText("");
	stLabelFileDiskSpace.setTextWidth(100);
	stLabelFileDiskSpace.setTextFormat(Qt::PlainText);
	stLabelFileDiskSpace.setTextOption(QTextOption(Qt::AlignLeft | Qt::AlignVCenter));

	stLabelFilePage.setText("");
	stLabelFilePage.setTextWidth(100);
	stLabelFilePage.setTextFormat(Qt::PlainText);
	stLabelFilePage.setTextOption(QTextOption(Qt::AlignRight | Qt::AlignVCenter));

	for (int i = 0; i < FILEROWCOUNT; i++)
	{
		for (int j = 0; j < FILECOLCOUNT; j++)
		{
			stLabelFileRecord[i][j].setText("");
			stLabelFileRecord[i][j].setTextWidth(100);
			stLabelFileRecord[i][j].setTextFormat(Qt::PlainText);
			stLabelFileRecord[i][j].setTextOption(QTextOption(Qt::AlignCenter));
		}
	}

	//static text of calibrate
	stLabelCalibrate.setText("");
	stLabelCalibrate.setTextWidth(100);
	stLabelCalibrate.setTextFormat(Qt::PlainText);
	stLabelCalibrate.setTextOption(QTextOption(Qt::AlignLeft | Qt::AlignVCenter));

	//static text of marker table
	for (int i = 0; i < 30; i++)
	{
		stLabelMarkerTable[i].setText("");
		stLabelMarkerTable[i].setTextWidth(100);
		stLabelMarkerTable[i].setTextFormat(Qt::PlainText);
		stLabelMarkerTable[i].setTextOption(QTextOption(Qt::AlignCenter));
	}

	//static text of Peak marker table
	for (int i = 0; i < 30; i++)
	{
		stLabelPeakTable[i].setText("");
		stLabelPeakTable[i].setTextWidth(100);
		stLabelPeakTable[i].setTextFormat(Qt::PlainText);
		stLabelPeakTable[i].setTextOption(QTextOption(Qt::AlignCenter));
	}

	//static text of canvas label
	for (int i = 0; i < VECTORLABELCOUNT; i++)
	{
		stLabel[i].setText("");
		stLabel[i].setTextWidth(120);
		stLabel[i].setTextFormat(Qt::PlainText);

		if (i == 0 || i == 1 || i == 2 || i == 4 || i == 5 || i == 6 || i == 8 || i == 11 || i == 14 || i == 15 || i == 16)
		{
			stLabel[i].setTextOption(QTextOption(Qt::AlignLeft | Qt::AlignVCenter));
		} else if (i == 3 || i == 7 || i == 10 || i == 13)
		{
			stLabel[i].setTextOption(QTextOption(Qt::AlignRight | Qt::AlignVCenter));
		} else
		{
			stLabel[i].setTextOption(QTextOption(Qt::AlignCenter));
		}
	}

	//static text of marker
	char tempChar[32];

	for (int i = 0; i < MARKERCOUNT; i++)
	{
		stLabelMarker[i].setTextWidth(20);
		stLabelMarkerR[i].setTextWidth(20);
		stLabelMarker[i].setTextOption(QTextOption(Qt::AlignCenter));
		stLabelMarkerR[i].setTextOption(QTextOption(Qt::AlignCenter));
		stLabelMarker[i].setText(QString(intToString(i + 1, tempChar)).trimmed());
		stLabelMarkerR[i].setText(QString(intToString(i + 1, tempChar)).trimmed() + "R");
	}

	//static text of ampt graticule
	for (int i = 0; i < AMPTGRATICULECOUNT; i++)
	{
		stLabelGraticule[i].setTextWidth(100);
		stLabelGraticule[i].setTextOption(Qt::AlignLeft | Qt::AlignVCenter);
		stLabelGraticule[i].setText("");
	}

	//static text of time spec
	for (int i = 0; i < TIMESPECTEXTCOUNT; i++)
	{
		stLabelTimeSpec[i].setTextWidth(100);
		stLabelTimeSpec[i].setTextOption(Qt::AlignLeft | Qt::AlignVCenter);
		stLabelTimeSpec[i].setText("");
	}

	//static text of acpr
	for (int i = 0; i < ACPRTEXTCOUNT; i++)
	{
		stLabelAcpr[i].setTextWidth(100);
		stLabelAcpr[i].setText("");

		if (i == 2 || i == 4 || i == 7 || i == 9 || i == 11 || i == 13 || i == 16 || i == 18 || i == 20 || i == 22 || i == 25 || i == 27 || i == 29 || i == 31)
		{
			stLabelAcpr[i].setTextOption(Qt::AlignRight | Qt::AlignVCenter);
		} else
		{
			stLabelAcpr[i].setTextOption(Qt::AlignLeft | Qt::AlignVCenter);
		}
	}

	//static text of obw
	for (int i = 0; i < OBWTEXTCOUNT; i++)
	{
		stLabelObw[i].setTextWidth(100);
		stLabelObw[i].setText("");

		if (i == 0 || i == 3 || i == 6)
		{
			stLabelObw[i].setTextOption(Qt::AlignLeft | Qt::AlignVCenter);
		} else
		{
			stLabelObw[i].setTextOption(Qt::AlignRight | Qt::AlignVCenter);
		}
	}

	//variable
	eventObject = this;
	lcdRefreshTimes = 200;
	helpMessage = "";
	inputTag = "";
	tagValue = "";
	inputValue = "";
	prvKeyValue = 0;
	helpContent = getTheTranslatorString("Ready");

	//pixInput
	pixInput = new QPixmap(SCREENWIDTH / 2, 72);
	pixInput->fill(Qt::transparent);

	//载入菜单
	sysMenu = new tSysMenu(QCoreApplication::applicationDirPath() + "/menu.xml");
	nowMainKey = sysKey;
	nowSubKey = NULL;

	//载入SCPI
	sysScpi = new tSysScpi;
	connect(sysScpi, SIGNAL(refreshScpiState(QString)), this, SLOT(refreshScpiState(QString)));
	connect(sysScpi, SIGNAL(refreshWindowState()), this, SLOT(drawAll()));
	connect(sysScpi, SIGNAL(resetUsbSlaveInterface()), this, SLOT(resetUsbSlaveInterface()));
	connect(sysScpi, SIGNAL(showAlarmMessage(QString, QString)), this, SLOT(showAlaramMessages(QString, QString)));
	connect(sysScpi, SIGNAL(refreshMenuFrame(unsigned long, unsigned long)), this, SLOT(refreshMenuFrame(unsigned long, unsigned long)));

	//获取系统时间
	if (setPCF8563toLocalDateTime() == _RESULT_SUCCESS)
	{
		sysData.system.datetime.isPCF8563 = true;
	} else
	{
		sysData.system.datetime.isPCF8563 = false;
	}

	//获取系统LOGO
	getLogoPicture();

	//载入驱动
	loadSystemDriver();

	fftEvent = new QEvent(fftInt);

	//控制
	sysScpi->presetSystemData();
	sysScpi->controlRf();
	sysScpi->execSelfCheck();
	//sysScpi->resetSystemIOOfLan();
	printf("ip:%s, mask:%s, gate:%s, mac:%s\r\n",sysData.system.io.lan.ip,sysData.system.io.lan.mask,sysData.system.io.lan.gate,sysData.system.io.lan.mac);

	//damage file
	QString hisFileName = QCoreApplication::applicationDirPath() + "/poweroff.dat";

	if (QFile(hisFileName).exists())
	{
		if (QFile(hisFileName).size() != sizeof sysData)
		{
			QFile(hisFileName).remove();
		}
	}

	hisFileName = QCoreApplication::applicationDirPath() + "/user.dat";

	if (QFile(hisFileName).exists())
	{
		if (QFile(hisFileName).size() != sizeof sysData)
		{
			QFile(hisFileName).remove();
		}
	}

	//开机参数
	systemParam powerOnParam = sysData.system.powerOnPreset.powerOnParam;
	systemParam presetParam = sysData.system.powerOnPreset.presetParam;

	if (sysData.system.powerOnPreset.powerOnParam == spUser)
	{
		sysScpi->loadUserState();
		sysScpi->controlRf();
	} else if (sysData.system.powerOnPreset.powerOnParam == spPowerOff)
	{
		sysScpi->loadPowerOffState();
		sysScpi->controlRf();
	}

	sysData.system.powerOnPreset.powerOnParam = powerOnParam;
	sysData.system.powerOnPreset.presetParam = presetParam;

	//载入串口驱动
	comHandle = -1;
	if (__COM__)
	{
		comHandle = open(DEVICE_NAME_COM, O_RDWR | O_NONBLOCK | O_NOCTTY);
	}
	if (comHandle >= 0)
	{
		if (__DEBUG)
		{
			printf("%s\n", "load serial driver successed");
		}

		char buf[8192] = { };
		read(comHandle, buf, sizeof buf);
		sysScpi->resetSystemIOOfSerial();
		comNotifier = new QSocketNotifier(comHandle, QSocketNotifier::Read, this);
		connect	(comNotifier, SIGNAL(activated(int)), sysScpi, SLOT(readSerialData()));
	} else
	{
		if (__DEBUG)
		{
			printf("%s\n", "load serial driver failed");
		}
	}

	//加载USB从口驱动
	usbHandle = -1;
	usbHandle = open(DEVICE_NAME_USB, O_RDWR | O_NONBLOCK);

	if (usbHandle >= 0)
	{
		if (__DEBUG)
		{
			printf("load usb slave driver successed\n");
		}

		usbNotifier = new QSocketNotifier(usbHandle, QSocketNotifier::Read, this);
		connect(usbNotifier, SIGNAL(activated(int)), sysScpi, SLOT(readUsbSlaveData()));
	} else
	{
		if (__DEBUG)
		{
			printf("load usb slave driver failed\n");
		}
	}

	//加载GPIB驱动
	gpibHandle = -1;
	gpibHandle = open(DEVICE_NAME_GPIB, O_RDWR | O_NONBLOCK);

	if (gpibHandle >= 0)
	{
		if (__DEBUG)
		{
			printf("load gpib driver successed\n");
		}

		gpibNotifier = new QSocketNotifier(gpibHandle, QSocketNotifier::Read, this);
		connect(gpibNotifier, SIGNAL(activated(int)), sysScpi, SLOT(readGpibData()));
		sysScpi->setSystemIOGpibMainAddr(sysData.system.io.gpib.addr);
	} else
	{
		if(__DEBUG)
		{
			printf("load gpib driver failed\n");
		}
	}

	//  //poweroff
	//  powerOffMinute = QTime::currentTime().minute();
	//  sysScpi->savePowerOffState();

	evaDay = -1;
	evaIndex = 0;
	memset(evaArray, 0x0, sizeof evaArray);

	//加载GPIO驱动
	//  gpioHandle = -1;
	//  gpioHandle = open(DEVICE_NAME_GPIO, O_RDWR | O_NONBLOCK);
	//
	//  if (gpioHandle >= 0)
	//  {
	//    if (__DEBUG)
	//    {
	//      printf("%s\n", "load gpio driver successed");
	//    }
	//
	//  	ioctl(gpioHandle ,0x01,0x00);
	//  } else
	//  {
	//    if (__DEBUG)
	//    {
	//      printf("%s\n", "load gpio driver failed");
	//    }
	//  }

	showSplashScreen();
}

void mainForm::drawHelp(QPainter* painter, QPixmap* pixmap, QRect rc)
{
	if (painter == NULL || !painter->isActive() || pixmap == NULL || pixmap->isNull())
	{
		return;
	}

	int penWidth = 5;
	int sep = 10;
	QRect rcContent = QRect(rc.width() / 8, rc.height() / 4, rc.width() * 6 / 8, rc.height() / 2);
	QRect rcText = QRect(rcContent.left() + penWidth + sep, rcContent.top() + penWidth + sep, rcContent.width() - 2 * (penWidth + sep), rcContent.height() - 2 * (penWidth + sep));
	QPen pen;
	pen.setWidth(penWidth);
	pen.setColor(Qt::darkGray);
	painter->setPen(pen);

	painter->fillRect(rcContent, Qt::gray);
	painter->drawRect(rcContent);

	painter->setFont(fontBold);
	painter->setPen(Qt::black);
	painter->drawText(rcText, Qt::TextWrapAnywhere, getTheTranslatorString("Help") + "\n\n" + helpMessage);
}

void mainForm::drawSystemLog(QPainter* painter, QPixmap* pixmap, QRect rc)
{
	if (painter == NULL || !painter->isActive() || pixmap == NULL || pixmap->isNull())
	{
		return;
	}

	char temp[32] = { };
	int sep = 20;
	int rowHeight = 38;
	QRect rect = QRect(sep, sep, rc.width() - 1.55 * sep, rowHeight);
	QRect rcNo;
	QRect rcContent;
	QVector<QRect> rects;
	double noPercent = 0.1;
	QString spaceSep = "  ";

	painter->fillRect(rc, Qt::black);
	painter->setFont(fontData);
	painter->setPen(Qt::white);

	rcNo = QRect(rect.left(), rect.top(), rect.width() * noPercent, rect.height());
	rcContent = QRect(rcNo.left() + rcNo.width(), rcNo.top(), rect.width() - rcNo.width(), rect.height());
	rects << rcNo << rcContent;
	painter->drawText(rcNo, Qt::AlignCenter, getTheTranslatorString("No"));
	painter->drawText(rcContent, Qt::AlignLeft | Qt::AlignVCenter, spaceSep + getTheTranslatorString("Message"));

	for (int i = 0; i < logList.count(); i++)
	{
		if (i < LOGCOUNT)
		{
			rcNo = QRect(rcNo.left(), rcNo.top() + rcNo.height(), rcNo.width(), rcNo.height());
			rcContent = QRect(rcContent.left(), rcContent.top() + rcContent.height(), rcContent.width(), rcContent.height());
			rects << rcNo << rcContent;

			painter->drawText(rcNo, Qt::AlignCenter, intToString(i + 1, temp));
			painter->drawText(rcContent, Qt::AlignLeft | Qt::AlignVCenter, spaceSep + logList.at(i));
		}
	}

	painter->setPen(GRIDCOLOR);
	painter->drawRects(rects);
}

void mainForm::drawSystemInfo(QPainter* painter, QPixmap* pixmap, QRect rc)
{
	if (painter == NULL || !painter->isActive() || pixmap == NULL || pixmap->isNull())
	{
		return;
	}

	char temp[64] = { };
	int sep = 20;
	int rowHeight = 32;
	int blankSep = 4;
	double percent = 0.25;
	QVector<QRect> rects;
	QRect rect = QRect(sep, sep, rc.width() - 1.55 * sep, rowHeight);
	QRect rcName;
	QRect rcContent;
	QString space = "  ";
	QPen pen;

	pen.setColor(GRIDCOLOR);
	pen.setWidth(2);
	painter->fillRect(rc, Qt::black);
	painter->setFont(fontData);

	//serial no
	rcName = QRect(rect.left(), rect.top(), rect.width() * percent, rect.height());
	rcContent = QRect(rcName.left() + rcName.width(), rcName.top(), rect.width() - rcName.width(), rcName.height());
	rects << rcName << rcContent;
	painter->setPen(Qt::white);
	painter->drawText(rcName, Qt::AlignLeft | Qt::AlignVCenter, space + getTheTranslatorString("Serial Number"));

	if (sysScpi->eva.isEvaluate)
	{
		painter->setPen(Qt::red);
		painter->drawText(rcContent, Qt::AlignLeft | Qt::AlignVCenter, space + QString(sysData.system.serialNo) + "(EVA)");
	} else
	{
		painter->drawText(rcContent, Qt::AlignLeft | Qt::AlignVCenter, space + QString(sysData.system.serialNo));
	}

	painter->setPen(pen);

	//devicetype
	rcName = QRect(rcName.left(), rcName.top() + rcName.height(), rcName.width(), rcName.height());
	rcContent = QRect(rcContent.left(), rcContent.top() + rcContent.height(), rcContent.width(), rcContent.height());
	rects << rcName << rcContent;
	painter->setPen(Qt::white);
	painter->drawText(rcName, Qt::AlignLeft | Qt::AlignVCenter, space + getTheTranslatorString("Device Type"));
	painter->drawText(rcContent, Qt::AlignLeft | Qt::AlignVCenter, space + sysScpi->devType);

	//sep
	rcName = QRect(rcName.left(), rcName.top() + rcName.height(), rcName.width(), rcName.height() / blankSep);
	rcContent = QRect(rcContent.left(), rcContent.top() + rcContent.height(), rcContent.width(), rcContent.height() / blankSep);
	rects << rcName << rcContent;
	painter->fillRect(rcName, GRIDCOLOR);
	painter->fillRect(rcContent, GRIDCOLOR);

	//software version
	rcName = QRect(rcName.left(), rcName.top() + rcName.height(), rcName.width(), rect.height());
	rcContent = QRect(rcContent.left(), rcContent.top() + rcContent.height(), rcContent.width(), rect.height());
	rects << rcName << rcContent;
	painter->setPen(Qt::white);
	painter->drawText(rcName, Qt::AlignLeft | Qt::AlignVCenter, space + getTheTranslatorString("Software Version"));
	sprintf(temp, "%d.%d.%d.%d.%d", VER_MAJOR, VER_MINOR, VER_REVISION, VER_BUILD, VER_TAG);
	painter->drawText(rcContent, Qt::AlignLeft | Qt::AlignVCenter, space + (QString) temp);
	painter->setPen(pen);

	//firmware version
	rcName = QRect(rcName.left(), rcName.top() + rcName.height(), rcName.width(), rect.height());
	rcContent = QRect(rcContent.left(), rcContent.top() + rcContent.height(), rcContent.width(), rect.height());
	rects << rcName << rcContent;
	painter->setPen(Qt::white);
	painter->drawText(rcName, Qt::AlignLeft | Qt::AlignVCenter, space + getTheTranslatorString("Firmware Version"));
	sprintf(temp, "%d.%d", sysData.system.firmwareVersion[0], sysData.system.firmwareVersion[1]);
	painter->drawText(rcContent, Qt::AlignLeft | Qt::AlignVCenter, space + (QString) temp);
	painter->setPen(pen);

	//sep
	rcName = QRect(rcName.left(), rcName.top() + rcName.height(), rcName.width(), rcName.height() / blankSep);
	rcContent = QRect(rcContent.left(), rcContent.top() + rcContent.height(), rcContent.width(), rcContent.height() / blankSep);
	rects << rcName << rcContent;
	painter->fillRect(rcName, GRIDCOLOR);
	painter->fillRect(rcContent, GRIDCOLOR);

	//temperature
	rcName = QRect(rcName.left(), rcName.top() + rcName.height(), rcName.width(), rect.height());
	rcContent = QRect(rcContent.left(), rcContent.top() + rcContent.height(), rcContent.width(), rect.height());
	rects << rcName << rcContent;
	painter->setPen(Qt::white);
	painter->drawText(rcName, Qt::AlignLeft | Qt::AlignVCenter, space + getTheTranslatorString("temperature"));
	painter->drawText(rcContent, Qt::AlignLeft | Qt::AlignVCenter, space + QString(floatToStringDot2(sysData.temperature, temp)).trimmed() + "℃");
	painter->setPen(pen);

	//sep
	rcName = QRect(rcName.left(), rcName.top() + rcName.height(), rcName.width(), rcName.height() / blankSep);
	rcContent = QRect(rcContent.left(), rcContent.top() + rcContent.height(), rcContent.width(), rcContent.height() / blankSep);
	rects << rcName << rcContent;
	painter->fillRect(rcName, GRIDCOLOR);
	painter->fillRect(rcContent, GRIDCOLOR);

	QStringList lanList = sysScpi->getLanRealIPAndAddress();
	QString lanHead = "";

	if (lanList.count() < 3)
	{
		lanHead = "*";
	}

	//lan state
	rcName = QRect(rcName.left(), rcName.top() + rcName.height(), rcName.width(), rect.height());
	rcContent = QRect(rcContent.left(), rcContent.top() + rcContent.height(), rcContent.width(), rect.height());
	rects << rcName << rcContent;
	painter->setPen(Qt::white);
	painter->drawText(rcName, Qt::AlignLeft | Qt::AlignVCenter, space + getTheTranslatorString("Local Network"));
	/* if (lanList.count() >= 4 && lanList.at(3) == "UP")
	 {
	 painter->drawText(rcContent, Qt::AlignLeft | Qt::AlignVCenter, space + getTheTranslatorString("Connect"));
	 } else
	 {
	 painter->setPen(Qt::red);
	 painter->drawText(rcContent, Qt::AlignLeft | Qt::AlignVCenter, space + getTheTranslatorString("DisConnect"));
	 painter->setPen(Qt::white);
	 }
	 */

	if (isNetworkConnect())
	{
		painter->drawText(rcContent, Qt::AlignLeft | Qt::AlignVCenter, space + getTheTranslatorString("Connect"));
	} else
	{
		painter->setPen(Qt::red);
		painter->drawText(rcContent, Qt::AlignLeft | Qt::AlignVCenter, space + getTheTranslatorString("DisConnect"));
		painter->setPen(Qt::white);
	}
	painter->setPen(pen);

	//ip address
	rcName = QRect(rcName.left(), rcName.top() + rcName.height(), rcName.width(), rcName.height());
	rcContent = QRect(rcContent.left(), rcContent.top() + rcContent.height(), rcContent.width(), rcContent.height());
	rects << rcName << rcContent;
	painter->setPen(Qt::white);
	painter->drawText(rcName, Qt::AlignLeft | Qt::AlignVCenter, space + lanHead + getTheTranslatorString("IP Address"));
	if (lanList.count() >= 3)
	{
		painter->drawText(rcContent, Qt::AlignLeft | Qt::AlignVCenter, space + lanList.at(1));
	} else
	{
		painter->drawText(rcContent, Qt::AlignLeft | Qt::AlignVCenter, space + (QString) sysData.system.io.lan.ip);
	}
	painter->setPen(pen);

	//mask
	rcName = QRect(rcName.left(), rcName.top() + rcName.height(), rcName.width(), rcName.height());
	rcContent = QRect(rcContent.left(), rcContent.top() + rcContent.height(), rcContent.width(), rcContent.height());
	rects << rcName << rcContent;
	painter->setPen(Qt::white);
	painter->drawText(rcName, Qt::AlignLeft | Qt::AlignVCenter, space + lanHead + getTheTranslatorString("Mask"));
	if (lanList.count() >= 3)
	{
		painter->drawText(rcContent, Qt::AlignLeft | Qt::AlignVCenter, space + lanList.at(2));
	} else
	{
		painter->drawText(rcContent, Qt::AlignLeft | Qt::AlignVCenter, space + (QString) sysData.system.io.lan.mask);
	}
	painter->setPen(pen);

	//gate
	rcName = QRect(rcName.left(), rcName.top() + rcName.height(), rcName.width(), rcName.height());
	rcContent = QRect(rcContent.left(), rcContent.top() + rcContent.height(), rcContent.width(), rcContent.height());
	rects << rcName << rcContent;
	painter->setPen(Qt::white);
	painter->drawText(rcName, Qt::AlignLeft | Qt::AlignVCenter, space + lanHead + getTheTranslatorString("Gate"));
	painter->drawText(rcContent, Qt::AlignLeft | Qt::AlignVCenter, space + (QString) sysData.system.io.lan.gate);
	painter->setPen(pen);

	//mac
	rcName = QRect(rcName.left(), rcName.top() + rcName.height(), rcName.width(), rcName.height());
	rcContent = QRect(rcContent.left(), rcContent.top() + rcContent.height(), rcContent.width(), rcContent.height());
	rects << rcName << rcContent;
	painter->setPen(Qt::white);
	painter->drawText(rcName, Qt::AlignLeft | Qt::AlignVCenter, space + lanHead + getTheTranslatorString("Mac Physical Address"));

	sprintf(temp, "%X%X:%X%X:%X%X:%X%X:%X%X:%X%X", sysData.system.lanMacAddress[0], sysData.system.lanMacAddress[1], sysData.system.lanMacAddress[2], sysData.system.lanMacAddress[3], sysData.system.lanMacAddress[4], sysData.system.lanMacAddress[5], sysData.system.lanMacAddress[6], sysData.system.lanMacAddress[7], sysData.system.lanMacAddress[8], sysData.system.lanMacAddress[9], sysData.system.lanMacAddress[10], sysData.system.lanMacAddress[11]);
	QString lanStr = "";

	if (lanList.count() > 0)
	{
		for (int i = 0; i < lanList.at(0).length(); i++)
		{
			if (lanList.at(0).at(i) != QChar(':'))
			{
				lanStr += lanList.at(0).at(i);
			} else
			{
				lanStr += QChar('-');
			}
		}
	}

	if (lanList.count() >= 3)
	{
		if (lanList.at(0) != QString(temp).trimmed())
		{
			painter->setPen(Qt::red);
		}

		painter->drawText(rcContent, Qt::AlignLeft | Qt::AlignVCenter, space + lanStr);
		painter->setPen(Qt::white);
	} else
	{
		painter->drawText(rcContent, Qt::AlignLeft | Qt::AlignVCenter, space + lanStr);
	}
	painter->setPen(pen);

	//port
	rcName = QRect(rcName.left(), rcName.top() + rcName.height(), rcName.width(), rcName.height());
	rcContent = QRect(rcContent.left(), rcContent.top() + rcContent.height(), rcContent.width(), rcContent.height());
	rects << rcName << rcContent;
	painter->setPen(Qt::white);
	painter->drawText(rcName, Qt::AlignLeft | Qt::AlignVCenter, space + lanHead + getTheTranslatorString("Listen Port"));
	painter->drawText(rcContent, Qt::AlignLeft | Qt::AlignVCenter, space + (QString) intToString(LISTENPORT, temp));

	//cpu

	//memory
#if 0
	if (SELFCHECK == 1)
	{
		//sep
		rcName = QRect(rcName.left(), rcName.top() + rcName.height(), rcName.width(), rcName.height() / blankSep);
		rcContent = QRect(rcContent.left(), rcContent.top() + rcContent.height(), rcContent.width(), rcContent.height() / blankSep);
		rects << rcName << rcContent;
		painter->fillRect(rcName, GRIDCOLOR);
		painter->fillRect(rcContent, GRIDCOLOR);

		//Self Check
		rcName = QRect(rcName.left(), rcName.top() + rcName.height(), rcName.width(), rcName.height() * blankSep);
		rcContent = QRect(rcContent.left(), rcContent.top() + rcContent.height(), rcContent.width(), rcContent.height() * blankSep);
		rects << rcName << rcContent;
		painter->setPen(Qt::white);
		painter->drawText(rcName, Qt::AlignLeft | Qt::AlignVCenter, space + lanHead + getTheTranslatorString("Power Check"));
		if (selfCheck.power)
		{
			painter->drawText(rcContent, Qt::AlignLeft | Qt::AlignVCenter, space + getTheTranslatorString("OK"));
		} else
		{
			painter->setPen(Qt::red);
			painter->drawText(rcContent, Qt::AlignLeft | Qt::AlignVCenter, space + getTheTranslatorString("FAULT"));
			painter->setPen(Qt::white);
		}

		rcName = QRect(rcName.left(), rcName.top() + rcName.height(), rcName.width(), rcName.height());
		rcContent = QRect(rcContent.left(), rcContent.top() + rcContent.height(), rcContent.width(), rcContent.height());
		rects << rcName << rcContent;
		painter->setPen(Qt::white);
		painter->drawText(rcName, Qt::AlignLeft | Qt::AlignVCenter, space + lanHead + getTheTranslatorString("Control Check"));
		if (selfCheck.control)
		{
			painter->drawText(rcContent, Qt::AlignLeft | Qt::AlignVCenter, space + getTheTranslatorString("OK"));
		} else
		{
			painter->setPen(Qt::red);
			painter->drawText(rcContent, Qt::AlignLeft | Qt::AlignVCenter, space + getTheTranslatorString("FAULT"));
			painter->setPen(Qt::white);
		}

		rcName = QRect(rcName.left(), rcName.top() + rcName.height(), rcName.width(), rcName.height());
		rcContent = QRect(rcContent.left(), rcContent.top() + rcContent.height(), rcContent.width(), rcContent.height());
		rects << rcName << rcContent;
		painter->setPen(Qt::white);
		painter->drawText(rcName, Qt::AlignLeft | Qt::AlignVCenter, space + lanHead + getTheTranslatorString("IF Check"));
		if (selfCheck.aif)
		{
			painter->drawText(rcContent, Qt::AlignLeft | Qt::AlignVCenter, space + getTheTranslatorString("OK"));
		} else
		{
			painter->setPen(Qt::red);
			painter->drawText(rcContent, Qt::AlignLeft | Qt::AlignVCenter, space + getTheTranslatorString("FAULT"));
			painter->setPen(Qt::white);
		}

		rcName = QRect(rcName.left(), rcName.top() + rcName.height(), rcName.width(), rcName.height());
		rcContent = QRect(rcContent.left(), rcContent.top() + rcContent.height(), rcContent.width(), rcContent.height());
		rects << rcName << rcContent;
		painter->setPen(Qt::white);
		painter->drawText(rcName, Qt::AlignLeft | Qt::AlignVCenter, space + lanHead + getTheTranslatorString("RF Check"));
		if (selfCheck.arf)
		{
			painter->drawText(rcContent, Qt::AlignLeft | Qt::AlignVCenter, space + getTheTranslatorString("OK"));
		} else
		{
			painter->setPen(Qt::red);
			painter->drawText(rcContent, Qt::AlignLeft | Qt::AlignVCenter, space + getTheTranslatorString("FAULT"));
			painter->setPen(Qt::white);
		}
	}
#endif
	//com
	rcName = QRect(rcName.left(), rcName.top() + rcName.height(), rcName.width(), rcName.height());
	rcContent = QRect(rcContent.left(), rcContent.top() + rcContent.height(), rcContent.width(), rcContent.height());
	rects << rcName << rcContent;
	painter->setPen(Qt::white);
	painter->drawText(rcName, Qt::AlignLeft | Qt::AlignVCenter, space + lanHead + getTheTranslatorString("COM"));
	if (comHandle >= 0)
	{
		painter->drawText(rcContent, Qt::AlignLeft | Qt::AlignVCenter, space + getTheTranslatorString("VALID"));
	} else
	{
		painter->setPen(Qt::yellow);
		painter->drawText(rcContent, Qt::AlignLeft | Qt::AlignVCenter, space + getTheTranslatorString("INVALID"));
		painter->setPen(Qt::white);
	}

	painter->setPen(GRIDCOLOR);
	painter->setPen(pen);
	painter->drawRects(rects);
}

void mainForm::drawFile(QPainter* painter, QPixmap* pixmap, QRect rc)
{
	if (painter == NULL || !painter->isActive() || pixmap == NULL || pixmap->isNull())
	{
		return;
	}

	if (sysData.file.isRestoring && QFile(sysScpi->fileSelectName).exists())
	{
		QPen pen;
		pen.setWidth(3);
		pen.setColor(Qt::blue);
		painter->drawPixmap(0, 0, sysScpi->fileSelectPixmap);
		painter->setPen(pen);
		painter->drawRect(BORDERWIDTH, BORDERWIDTH, pixmap->width() - 2 * BORDERWIDTH - 1, pixmap->height() - 2 * BORDERWIDTH + 1);
		return;
	}

	painter->fillRect(rc, Qt::black);
	painter->setFont(fontData);
	painter->setPen(Qt::white);

	int Sep = 20;
	QRect rcTable;
	QRect rcThe;
	QVector<QRect> rcS;
	double colPercent[] = { 0.1, 0.3, 0.15, 0.2, 0.25 };
	int rowHeight = (rc.height() - Sep) / (FILEROWCOUNT + 2);
	QRect rcCol[FILECOLCOUNT];
	int topPos = 5;

	//get text
	getFileLabel();

	//total
	rcTable = QRect(Sep, Sep, rc.width() - 1.55 * Sep, rowHeight);
	rcThe = QRect(rcTable.left(), rcTable.top(), rcTable.width() * 0.5, rcTable.height());
	if (stLabelFileDiskSpace.textWidth() != rcThe.width())
		stLabelFileDiskSpace.setTextWidth(rcThe.width());
	painter->drawStaticText(rcThe.left(), rcThe.top(), stLabelFileDiskSpace);
	rcThe = QRect(rcThe.left() + rcThe.width(), rcThe.top(), rcTable.width() - rcThe.width(), rowHeight);
	if (stLabelFilePage.textWidth() != rcThe.width())
		stLabelFilePage.setTextWidth(rcThe.width());
	painter->drawStaticText(rcThe.left(), rcThe.top(), stLabelFilePage);

	//title
	rcCol[0] = QRect(rcTable.left(), rcTable.top() + rcTable.height(), rcTable.width() * colPercent[0], rowHeight);
	rcCol[1] = QRect(rcCol[0].left() + rcCol[0].width(), rcCol[0].top(), rcTable.width() * colPercent[1], rowHeight);
	rcCol[2] = QRect(rcCol[1].left() + rcCol[1].width(), rcCol[1].top(), rcTable.width() * colPercent[2], rowHeight);
	rcCol[3] = QRect(rcCol[2].left() + rcCol[2].width(), rcCol[2].top(), rcTable.width() * colPercent[3], rowHeight);
	rcCol[4] = QRect(rcCol[3].left() + rcCol[3].width(), rcCol[3].top(), rcTable.width() + Sep - rcCol[3].left() - rcCol[3].width(), rowHeight);

	for (int i = 0; i < FILECOLCOUNT; i++)
	{
		if (stLabelFileRecord[0][i].textWidth() != rcCol[i].width())
		{
			stLabelFileRecord[0][i].setTextWidth(rcCol[i].width());
		}

		painter->drawStaticText(rcCol[i].left(), rcCol[i].top() + topPos, stLabelFileRecord[0][i]);
		rcS << rcCol[i];
	}

	//row file
	for (int i = 1; i <= sysData.file.nowPageCount; i++)
	{
		for (int j = 0; j < FILECOLCOUNT; j++)
		{
			rcThe = QRect(rcCol[j].left(), rcCol[j].top() + i * rowHeight, rcCol[j].width(), rcCol[j].height());
			rcS << rcThe;

			if (stLabelFileRecord[i][j].textWidth() != rcThe.width())
			{
				stLabelFileRecord[i][j].setTextWidth(rcThe.width());
			}

			painter->drawStaticText(rcThe.left(), rcThe.top() + topPos, stLabelFileRecord[i][j]);
		}
	}

	//draw rectss
	painter->setPen(GRIDCOLOR);
	painter->drawRects(rcS);

	//draw select
	if (sysData.file.pages > 0 && sysData.file.nowPageCount > 0 && sysData.file.nowSelIndex >= 0 && sysData.file.nowSelIndex < sysData.file.nowPageCount)
	{
		QRect rcSel = QRect(rcCol[0].left(), rcCol[0].top() + rcCol[0].height() * (sysData.file.nowSelIndex + 1), rcTable.width(), rcCol[0].height());
		painter->setPen(QColor(0x00, 0xff, 0x80));
		painter->drawRect(rcSel);
		painter->setPen(Qt::white);
	}
}

void mainForm::drawDemodVectorOfAm(QPainter* painter, QPixmap* pixmap, QRect rc)
{
	if (painter == NULL || !painter->isActive() || pixmap == NULL || pixmap->isNull())
	{
		return;
	}

	painter->fillRect(rc, Qt::black);

	//draw Table
	QRect rcTable;
	if (sysData.display.isFullScreen)
	{
		rcTable = QRect(TABLELEFT, TABLETOP, rc.width() - TABLELEFT - TABLERIGHT, rc.height() - 1.5 * TABLETOP);
	} else
	{
		rcTable = QRect(rc.width() - TABLEWIDTH - TABLERIGHT, TABLETOP, TABLEWIDTH, rc.height() - 1.5 * TABLETOP);
	}

	double rowHeight = rcTable.height() / 10.0;
	double colWidth = rcTable.width() / 10.0;
	QVector<QLine> lines;

	//draw preset
	rcPreset = QRect(rc.width() - TABLERIGHT - 32, rcTable.top(), 32, 32);

	if (sysData.isPreseting)
	{
		drawPreset();
	}

	//time domain
	for (int i = 0; i < 11; i++)
	{
		if (!sysData.display.isGridOn)
		{
			if (i == 0 || i == 10)
			{
				lines << QLine(rcTable.left(), rcTable.top() + rowHeight * i, rcTable.left() + rcTable.width(), rcTable.top() + rowHeight * i);
				lines << QLine(rcTable.left() + colWidth * i, rcTable.top(), rcTable.left() + colWidth * i, rcTable.top() + rcTable.height());
			}
		} else
		{
			lines << QLine(rcTable.left(), rcTable.top() + rowHeight * i, rcTable.left() + rcTable.width(), rcTable.top() + rowHeight * i);
			lines << QLine(rcTable.left() + colWidth * i, rcTable.top(), rcTable.left() + colWidth * i, rcTable.top() + rcTable.height());
		}

		if (i == 0)
		{
			lines << QLine(rcTable.left(), rcTable.top() + rowHeight * i + 1, rcTable.left() + rcTable.width(), rcTable.top() + rowHeight * i + 1);
			lines << QLine(rcTable.left(), rcTable.top() + rowHeight * i + 2, rcTable.left() + rcTable.width(), rcTable.top() + rowHeight * i + 2);
			lines << QLine(rcTable.left() + colWidth * i + 1, rcTable.top(), rcTable.left() + colWidth * i + 1, rcTable.top() + rcTable.height());
			lines << QLine(rcTable.left() + colWidth * i + 2, rcTable.top(), rcTable.left() + colWidth * i + 2, rcTable.top() + rcTable.height());
		}

		if (i == 5 && sysData.display.isGridOn)
		{
			lines << QLine(rcTable.left(), rcTable.top() + rowHeight * i - 1, rcTable.left() + rcTable.width(), rcTable.top() + rowHeight * i - 1);
			lines << QLine(rcTable.left(), rcTable.top() + rowHeight * i + 1, rcTable.left() + rcTable.width(), rcTable.top() + rowHeight * i + 1);
			lines << QLine(rcTable.left() + colWidth * i - 1, rcTable.top(), rcTable.left() + colWidth * i - 1, rcTable.top() + rcTable.height());
			lines << QLine(rcTable.left() + colWidth * i + 1, rcTable.top(), rcTable.left() + colWidth * i + 1, rcTable.top() + rcTable.height());
		}

		if (i == 10)
		{
			lines << QLine(rcTable.left(), rcTable.top() + rowHeight * i - 1, rcTable.left() + rcTable.width(), rcTable.top() + rowHeight * i - 1);
			lines << QLine(rcTable.left(), rcTable.top() + rowHeight * i - 2, rcTable.left() + rcTable.width(), rcTable.top() + rowHeight * i - 2);
			lines << QLine(rcTable.left() + colWidth * i - 1, rcTable.top(), rcTable.left() + colWidth * i - 1, rcTable.top() + rcTable.height());
			lines << QLine(rcTable.left() + colWidth * i - 2, rcTable.top(), rcTable.left() + colWidth * i - 2, rcTable.top() + rcTable.height());
		}
	}

	painter->setPen(GRIDCOLOR);
	painter->drawLines(lines);

	//draw ampt graticule
	if (sysData.display.isLabelOn)
	{
		//getAmptGraticuleLabel();

		QRect rcAmpt;
		int sep = 2;
		int textHeight = 16;
		char tempChar[32] = { };

		painter->setPen(Qt::white);
		painter->setFont(fontDataBig);
		rcAmpt = QRect(sep, rcTable.top(), rcTable.left() + sep * 5, textHeight);
		painter->drawText(rcAmpt, Qt::AlignLeft, "100%");
		rcAmpt = QRect(sep, rcTable.bottom() - textHeight + 1, rcTable.left() + sep * 5, textHeight);
		painter->drawText(rcAmpt, Qt::AlignLeft, "-100%");

		//carrier freq and att
		rcAmpt = QRect(rcTable.left(), rcTable.top() - TABLETOP / 2 - sep, rcTable.width() / 5, TABLETOP / 2);
		painter->drawText(rcAmpt, Qt::AlignLeft, getTheTranslatorString("AM"));
		rcAmpt = QRect(rcTable.right() - 0.4 * rcTable.width(), rcAmpt.top(), 0.4 * rcTable.width(), rcAmpt.height());
		painter->drawText(rcAmpt, Qt::AlignRight, getTheTranslatorString("CarrFreq") + " " + QString(getFreqAndUnitString(sysData.options.am_carrierfreq, 1, tempChar)).trimmed());
		rcAmpt = QRect(rcAmpt.left() - rcAmpt.width(), rcAmpt.top(), rcAmpt.width(), rcAmpt.height());
		painter->drawText(rcAmpt, Qt::AlignRight, getTheTranslatorString("Att") + " " + QString(intToString(sysData.ampt.att, tempChar)).trimmed() + " dB");

		//time
		rcAmpt = QRect(rcTable.left(), rcTable.bottom() + 2 * sep, rcTable.width() / 3, TABLETOP / 2);
		painter->drawText(rcAmpt, Qt::AlignLeft, "0.000");
		rcAmpt = QRect(rcAmpt.right(), rcAmpt.top(), rcAmpt.width(), rcAmpt.height());
		painter->drawText(rcAmpt, Qt::AlignCenter, getTheTranslatorString("Time") + "(ms)");
		rcAmpt = QRect(rcAmpt.right(), rcAmpt.top(), rcAmpt.width(), rcAmpt.height());
		sprintf(tempChar, "%16.3f", sysData.options.am_time_us / 1e3);
		painter->drawText(rcAmpt, Qt::AlignRight, QString(tempChar).trimmed() + " ms");
	}

	//draw vector curve
	double maxValue = 0;
	double minValue = 0;
	double avgValue = 0;

	mutexDemod.lock();
	maxValue = sysData.options.mData[0];
	minValue = sysData.options.mData[0];
	avgValue = sysData.options.mData[0];

	for (int i = 1; i < sysData.sweep.sweepPoints; i++)
	{
		if (sysData.options.mData[i] > maxValue)
		{
			maxValue = sysData.options.mData[i];
		}

		if (sysData.options.mData[i] < minValue)
		{
			minValue = sysData.options.mData[i];
		}

		avgValue += sysData.options.mData[i] / sysData.sweep.sweepPoints;
	}

	double midValue = minValue + (maxValue - minValue) / 2.0;
	double depth = 0;

	if ((maxValue + minValue) > 0)
	{
		depth = (maxValue - minValue) / (maxValue + minValue);
	}

	if (depth > 1.0)
	{
		depth = 1.0;
	}

	double canvasMaxValue = 1.0;
	double canvasMinValue = -1.0;
	double scaleDiv = 0.2;

	double scaleX = (rcTable.width() * 1.0) / ((sysData.sweep.sweepPoints - 1) * 1.0);
	double scaleY = (rcTable.height() * 1.0) / (scaleDiv * 10.0);
	double realValue = 0;
	double pixelValue = 0;

	QPolygonF trace(sysData.sweep.sweepPoints);

	for (int i = 0; i < sysData.sweep.sweepPoints; i++)
	{
		realValue = sysData.options.mData[i];

		if (realValue >= maxValue)
		{
			realValue = depth;
		} else if (realValue <= minValue)
		{
			realValue = -1.0 * depth;
		} else
		{
			if ((maxValue - minValue) <= 0)
			{
				realValue = 0;
			} else
			{
				realValue = depth - (maxValue - realValue) * 2 * depth / (maxValue - minValue);
			}
		}

		trace[i].setX(i);
		trace[i].setY((realValue - canvasMaxValue) * (scaleDiv * 10.0) / (canvasMinValue - canvasMaxValue));
	}

	mutexDemod.unlock();

	//world matrix
	painter->setPen(QColor(sysData.trace[0].colorR, sysData.trace[0].colorG, sysData.trace[0].colorB));
	matrix.translate(rcTable.left(), rcTable.top());
	matrix.scale(scaleX, scaleY);
	matrix.shear(0, 0);
	painter->setMatrix(matrix);
	painter->drawPolyline(trace);
	matrix.reset();
	painter->resetMatrix();

	//draw label
	if (!inputTag.isEmpty())
	{
		int inputSep = 6;
		int inputHeight = 30;
		int labelIndex = 13;
		int sep = 4;

		getVectorLabel();
		painter->setPen(Qt::white);
		labelIndex++;
		painter->setFont(fontInput);
		QRect rcInput = QRect(rcTable.left() + inputSep, rcTable.top() + 2 * rowHeight + inputSep / 2, rcTable.width() - 2 * inputSep, inputHeight);
		if ((int) stLabel[labelIndex].textWidth() != rcInput.width())
		{
			stLabel[labelIndex].setTextWidth(rcInput.width());
		}
		painter->drawStaticText(rcInput.left(), rcInput.top() + sep, stLabel[labelIndex]); //input name
		rcInput = QRect(rcInput.left(), rcInput.top() + rcInput.height(), rcInput.width(), rcInput.height());
		labelIndex++;
		if ((int) stLabel[labelIndex].textWidth() != rcInput.width())
		{
			stLabel[labelIndex].setTextWidth(rcInput.width());
		}

		if (inputValue.isEmpty())
		{
			painter->drawStaticText(rcInput.left(), rcInput.top() + sep, stLabel[labelIndex]); //tag value
		} else
		{
			painter->drawStaticText(rcInput.left(), rcInput.top() + sep, stLabel[labelIndex]); //input value
		}
	}
}

void mainForm::drawDemodVectorOfFm(QPainter* painter, QPixmap* pixmap, QRect rc)
{
	if (painter == NULL || !painter->isActive() || pixmap == NULL || pixmap->isNull())
	{
		return;
	}

	painter->fillRect(rc, Qt::black);

	//draw Table
	QRect rcTable;
	if (sysData.display.isFullScreen)
	{
		rcTable = QRect(TABLELEFT, TABLETOP, rc.width() - TABLELEFT - TABLERIGHT, rc.height() - 1.5 * TABLETOP);
	} else
	{
		rcTable = QRect(rc.width() - TABLEWIDTH - TABLERIGHT, TABLETOP, TABLEWIDTH, rc.height() - 1.5 * TABLETOP);
	}

	double rowHeight = rcTable.height() / 10.0;
	double colWidth = rcTable.width() / 10.0;
	QVector<QLine> lines;

	//draw preset
	rcPreset = QRect(rc.width() - TABLERIGHT - 32, rcTable.top(), 32, 32);

	if (sysData.isPreseting)
	{
		drawPreset();
	}

	//time domain
	for (int i = 0; i < 11; i++)
	{
		if (!sysData.display.isGridOn)
		{
			if (i == 0 || i == 10)
			{
				lines << QLine(rcTable.left(), rcTable.top() + rowHeight * i, rcTable.left() + rcTable.width(), rcTable.top() + rowHeight * i);
				lines << QLine(rcTable.left() + colWidth * i, rcTable.top(), rcTable.left() + colWidth * i, rcTable.top() + rcTable.height());
			}
		} else
		{
			lines << QLine(rcTable.left(), rcTable.top() + rowHeight * i, rcTable.left() + rcTable.width(), rcTable.top() + rowHeight * i);
			lines << QLine(rcTable.left() + colWidth * i, rcTable.top(), rcTable.left() + colWidth * i, rcTable.top() + rcTable.height());
		}

		if (i == 0)
		{
			lines << QLine(rcTable.left(), rcTable.top() + rowHeight * i + 1, rcTable.left() + rcTable.width(), rcTable.top() + rowHeight * i + 1);
			lines << QLine(rcTable.left(), rcTable.top() + rowHeight * i + 2, rcTable.left() + rcTable.width(), rcTable.top() + rowHeight * i + 2);
			lines << QLine(rcTable.left() + colWidth * i + 1, rcTable.top(), rcTable.left() + colWidth * i + 1, rcTable.top() + rcTable.height());
			lines << QLine(rcTable.left() + colWidth * i + 2, rcTable.top(), rcTable.left() + colWidth * i + 2, rcTable.top() + rcTable.height());
		}

		if (i == 5 && sysData.display.isGridOn)
		{
			lines << QLine(rcTable.left(), rcTable.top() + rowHeight * i - 1, rcTable.left() + rcTable.width(), rcTable.top() + rowHeight * i - 1);
			lines << QLine(rcTable.left(), rcTable.top() + rowHeight * i + 1, rcTable.left() + rcTable.width(), rcTable.top() + rowHeight * i + 1);
			lines << QLine(rcTable.left() + colWidth * i - 1, rcTable.top(), rcTable.left() + colWidth * i - 1, rcTable.top() + rcTable.height());
			lines << QLine(rcTable.left() + colWidth * i + 1, rcTable.top(), rcTable.left() + colWidth * i + 1, rcTable.top() + rcTable.height());
		}

		if (i == 10)
		{
			lines << QLine(rcTable.left(), rcTable.top() + rowHeight * i - 1, rcTable.left() + rcTable.width(), rcTable.top() + rowHeight * i - 1);
			lines << QLine(rcTable.left(), rcTable.top() + rowHeight * i - 2, rcTable.left() + rcTable.width(), rcTable.top() + rowHeight * i - 2);
			lines << QLine(rcTable.left() + colWidth * i - 1, rcTable.top(), rcTable.left() + colWidth * i - 1, rcTable.top() + rcTable.height());
			lines << QLine(rcTable.left() + colWidth * i - 2, rcTable.top(), rcTable.left() + colWidth * i - 2, rcTable.top() + rcTable.height());
		}
	}

	painter->setPen(GRIDCOLOR);
	painter->drawLines(lines);

	//draw ampt graticule
	if (sysData.display.isLabelOn)
	{
		//getAmptGraticuleLabel();

		QRect rcAmpt;
		int sep = 2;
		int textHeight = 16;
		char tempChar[32] = { };

		painter->setPen(Qt::white);
		painter->setFont(fontDataBig);
		rcAmpt = QRect(sep, rcTable.top(), rcTable.left() + sep * 20 + 20, textHeight);
		painter->drawText(rcAmpt, Qt::AlignLeft, QString(getFreqAndUnitString(sysData.options.fm_freq, 1, tempChar)).trimmed());
		rcAmpt = QRect(sep, rcTable.bottom() - textHeight + 1, rcTable.left() + sep * 20 + 20, textHeight);
		painter->drawText(rcAmpt, Qt::AlignLeft, "-" + QString(getFreqAndUnitString(sysData.options.fm_freq, 1, tempChar)).trimmed());

		//carrier freq and att
		rcAmpt = QRect(rcTable.left(), rcTable.top() - TABLETOP / 2 - sep, rcTable.width() / 5, TABLETOP / 2);
		painter->drawText(rcAmpt, Qt::AlignLeft, getTheTranslatorString("FM"));
		rcAmpt = QRect(rcTable.right() - 0.4 * rcTable.width(), rcAmpt.top(), 0.4 * rcTable.width(), rcAmpt.height());
		painter->drawText(rcAmpt, Qt::AlignRight, getTheTranslatorString("CarrFreq") + " " + QString(getFreqAndUnitString(sysData.options.fm_carrierfreq, 1, tempChar)).trimmed());
		rcAmpt = QRect(rcAmpt.left() - rcAmpt.width(), rcAmpt.top(), rcAmpt.width(), rcAmpt.height());
		painter->drawText(rcAmpt, Qt::AlignRight, getTheTranslatorString("Att") + " " + QString(intToString(sysData.ampt.att, tempChar)).trimmed() + " dB");

		//time
		rcAmpt = QRect(rcTable.left(), rcTable.bottom() + 2 * sep, rcTable.width() / 3, TABLETOP / 2);
		painter->drawText(rcAmpt, Qt::AlignLeft, "0.000");
		rcAmpt = QRect(rcAmpt.right(), rcAmpt.top(), rcAmpt.width(), rcAmpt.height());
		painter->drawText(rcAmpt, Qt::AlignCenter, getTheTranslatorString("Time") + "(ms)");
		rcAmpt = QRect(rcAmpt.right(), rcAmpt.top(), rcAmpt.width(), rcAmpt.height());
		sprintf(tempChar, "%16.3f", sysData.options.fm_time_us / 1e3);
		painter->drawText(rcAmpt, Qt::AlignRight, QString(tempChar).trimmed() + " ms");
	}

	//draw vector curve
	double maxValue = 0;
	double minValue = 0;
	int swePoints = sysData.sweep.sweepPoints;
	int pointOffset = 30;
	mutexDemod.lock();
	maxValue = sysData.options.mData[pointOffset];
	minValue = sysData.options.mData[pointOffset];

	for (int i = 1; i < swePoints; i++)
	{
		if (sysData.options.mData[i + pointOffset] > maxValue)
		{
			maxValue = sysData.options.mData[i + pointOffset];
		}

		if (sysData.options.mData[i + pointOffset] < minValue)
		{
			minValue = sysData.options.mData[i + pointOffset];
		}
	}

	double canvasMaxValue = maxValue;
	double canvasMinValue = minValue;
	double canvasErrValue = canvasMaxValue - canvasMinValue;
	double scaleDiv = (maxValue - minValue) / 10.0;
	double scaleX = (rcTable.width() * 1.0) / ((swePoints - 1) * 1.0);
	double scaleY = 0;
	if (canvasErrValue > 0)
	{
		scaleY = rcTable.height() * 1.0 / canvasErrValue;
	}
	double realValue = 0;
	double pixelValue = 0;

	QPolygonF trace(swePoints);

	if (canvasErrValue > 0)
	{
		for (int i = 0; i < swePoints; i++)
		{
			realValue = sysData.options.mData[i + pointOffset];
			if (isnan(realValue))
			{
				realValue = canvasMaxValue;
			} else if (realValue > canvasMaxValue)
			{
				realValue = canvasMaxValue;
			} else if (realValue < canvasMinValue)
			{
				realValue = canvasMinValue;
			}

			trace[i].setX(i);
			trace[i].setY((canvasMaxValue - realValue) / canvasErrValue * (scaleDiv * 10.0));
		}
	};

	mutexDemod.unlock();

	//world matrix
	painter->setPen(QColor(sysData.trace[0].colorR, sysData.trace[0].colorG, sysData.trace[0].colorB));
	matrix.translate(rcTable.left(), rcTable.top());
	matrix.scale(scaleX, scaleY);
	matrix.shear(0, 0);
	painter->setMatrix(matrix);
	painter->drawPolyline(trace);
	matrix.reset();
	painter->resetMatrix();

	//draw label
	if (!inputTag.isEmpty())
	{
		int inputSep = 6;
		int inputHeight = 30;
		int labelIndex = 13;
		int sep = 4;

		getVectorLabel();
		painter->setPen(Qt::white);
		labelIndex++;
		painter->setFont(fontInput);
		QRect rcInput = QRect(rcTable.left() + inputSep, rcTable.top() + 2 * rowHeight + inputSep / 2, rcTable.width() - 2 * inputSep, inputHeight);
		if ((int) stLabel[labelIndex].textWidth() != rcInput.width())
		{
			stLabel[labelIndex].setTextWidth(rcInput.width());
		}
		painter->drawStaticText(rcInput.left(), rcInput.top() + sep, stLabel[labelIndex]); //input name
		rcInput = QRect(rcInput.left(), rcInput.top() + rcInput.height(), rcInput.width(), rcInput.height());
		labelIndex++;
		if ((int) stLabel[labelIndex].textWidth() != rcInput.width())
		{
			stLabel[labelIndex].setTextWidth(rcInput.width());
		}

		if (inputValue.isEmpty())
		{
			painter->drawStaticText(rcInput.left(), rcInput.top() + sep, stLabel[labelIndex]); //tag value
		} else
		{
			painter->drawStaticText(rcInput.left(), rcInput.top() + sep, stLabel[labelIndex]); //input value
		}
	}
}

void mainForm::drawDemodOfAm(QPainter* painter, QPixmap* pixmap, QRect rc)
{
	if (painter == NULL || !painter->isActive() || pixmap == NULL || pixmap->isNull())
	{
		return;
	}

	painter->fillRect(rc, Qt::black);
	painter->setPen(Qt::white);
	painter->setFont(fontDataBig);

	QRect rcName, rcValue;
	int nameWidth = 100;
	int textHeight = 24;
	char tempChar[32] = { };
	QVector<QRect> rects;
	QString leftStr = "  ";
	/*
	 //carr power
	 rcName = QRect(TABLELEFT, TABLETOP, nameWidth, textHeight);
	 rcValue = QRect(rcName.right() + 1, rcName.top(), rc.width() - TABLESEP - rcName.right(), rcName.height());
	 painter->drawText(rcName, Qt::AlignLeft | Qt::AlignVCenter, leftStr + getTheTranslatorString("CarrPower"));
	 sprintf(tempChar, "%16.2f", sysData.options.am_carrierPower);
	 painter->drawText(rcValue, Qt::AlignLeft | Qt::AlignVCenter, leftStr + QString(tempChar).trimmed() + " dBm");
	 rects << rcName << rcValue;
	 */
	//am freq
	//  rcName = QRect(rcName.left(), rcName.bottom() + 1, rcName.width(), rcName.height());
	//  rcValue = QRect(rcValue.left(), rcValue.bottom() + 1, rcValue.width(), rcValue.height());
	rcName = QRect(TABLELEFT, TABLETOP, nameWidth, textHeight);
	rcValue = QRect(rcName.right() + 1, rcName.top(), rc.width() - TABLESEP - rcName.right(), rcName.height());
	painter->drawText(rcName, Qt::AlignLeft | Qt::AlignVCenter, leftStr + getTheTranslatorString("AM Freq"));
	painter->drawText(rcValue, Qt::AlignLeft | Qt::AlignVCenter, leftStr + QString(getFreqAndUnitString(sysData.options.am_freq, 1, tempChar)).trimmed());
	rects << rcName << rcValue;

	//depth
	rcName = QRect(rcName.left(), rcName.bottom() + 1, rcName.width(), rcName.height());
	rcValue = QRect(rcValue.left(), rcValue.bottom() + 1, rcValue.width(), rcValue.height());
	painter->drawText(rcName, Qt::AlignLeft | Qt::AlignVCenter, leftStr + getTheTranslatorString("Depth"));
	sprintf(tempChar, "%16.2f", sysData.options.am_depth * 100);
	painter->drawText(rcValue, Qt::AlignLeft | Qt::AlignVCenter, leftStr + QString(tempChar).trimmed() + " %");
	rects << rcName << rcValue;

	/*
	 //sinad
	 rcName = QRect(rcName.left(), rcName.bottom() + 1, rcName.width(), rcName.height());
	 rcValue = QRect(rcValue.left(), rcValue.bottom() + 1, rcValue.width(), rcValue.height());
	 painter->drawText(rcName, Qt::AlignLeft | Qt::AlignVCenter, leftStr + getTheTranslatorString("SINAD"));
	 sprintf(tempChar, "%16.2f", sysData.options.am_sinad);
	 painter->drawText(rcValue, Qt::AlignLeft | Qt::AlignVCenter, leftStr + QString(tempChar).trimmed() + " dB");
	 rects << rcName << rcValue;

	 //thd
	 rcName = QRect(rcName.left(), rcName.bottom() + 1, rcName.width(), rcName.height());
	 rcValue = QRect(rcValue.left(), rcValue.bottom() + 1, rcValue.width(), rcValue.height());
	 painter->drawText(rcName, Qt::AlignLeft | Qt::AlignVCenter, leftStr + getTheTranslatorString("THD"));
	 sprintf(tempChar, "%16.2f", sysData.options.am_thd);
	 painter->drawText(rcValue, Qt::AlignLeft | Qt::AlignVCenter, leftStr + QString(tempChar).trimmed() + " dB");
	 rects << rcName << rcValue;
	 */
	painter->setPen(GRIDCOLOR);
	painter->drawRects(rects);
}

void mainForm::drawDemodOfFm(QPainter* painter, QPixmap* pixmap, QRect rc)
{
	if (painter == NULL || !painter->isActive() || pixmap == NULL || pixmap->isNull())
	{
		return;
	}

	painter->fillRect(rc, Qt::black);
	painter->setPen(Qt::white);
	painter->setFont(fontDataBig);

	QRect rcName, rcValue;
	int nameWidth = 100;
	int textHeight = 24;
	char tempChar[32] = { };
	QVector<QRect> rects;
	QString leftStr = "  ";

	/*
	 //carr power
	 rcName = QRect(TABLELEFT, TABLETOP, nameWidth, textHeight);
	 rcValue = QRect(rcName.right() + 1, rcName.top(), rc.width() - TABLESEP - rcName.right(), rcName.height());
	 painter->drawText(rcName, Qt::AlignLeft | Qt::AlignVCenter, leftStr + getTheTranslatorString("CarrPower"));
	 sprintf(tempChar, "%16.2f", sysData.options.fm_carrierPower);
	 painter->drawText(rcValue, Qt::AlignLeft | Qt::AlignVCenter, leftStr + QString(tempChar).trimmed() + " dBm");
	 rects << rcName << rcValue;
	 */
	//fm freq
	//  rcName = QRect(rcName.left(), rcName.bottom() + 1, rcName.width(), rcName.height());
	//  rcValue = QRect(rcValue.left(), rcValue.bottom() + 1, rcValue.width(), rcValue.height());

	rcName = QRect(TABLELEFT, TABLETOP, nameWidth, textHeight);
	rcValue = QRect(rcName.right() + 1, rcName.top(), rc.width() - TABLESEP - rcName.right(), rcName.height());
	painter->drawText(rcName, Qt::AlignLeft | Qt::AlignVCenter, leftStr + getTheTranslatorString("FM Freq"));
	painter->drawText(rcValue, Qt::AlignLeft | Qt::AlignVCenter, leftStr + QString(getFreqAndUnitString(sysData.options.fm_freq, 1, tempChar)).trimmed());
	rects << rcName << rcValue;

	//deviation
	rcName = QRect(rcName.left(), rcName.bottom() + 1, rcName.width(), rcName.height());
	rcValue = QRect(rcValue.left(), rcValue.bottom() + 1, rcValue.width(), rcValue.height());
	painter->drawText(rcName, Qt::AlignLeft | Qt::AlignVCenter, leftStr + getTheTranslatorString("Deviation"));
	painter->drawText(rcValue, Qt::AlignLeft | Qt::AlignVCenter, leftStr + QString(getFreqAndUnitString(sysData.options.fm_deviation, 1, tempChar)).trimmed());
	rects << rcName << rcValue;

	/*
	 //sinad
	 rcName = QRect(rcName.left(), rcName.bottom() + 1, rcName.width(), rcName.height());
	 rcValue = QRect(rcValue.left(), rcValue.bottom() + 1, rcValue.width(), rcValue.height());
	 painter->drawText(rcName, Qt::AlignLeft | Qt::AlignVCenter, leftStr + getTheTranslatorString("SINAD"));
	 sprintf(tempChar, "%16.2f", sysData.options.fm_sinad);
	 painter->drawText(rcValue, Qt::AlignLeft | Qt::AlignVCenter, leftStr + QString(tempChar).trimmed() + " dB");
	 rects << rcName << rcValue;

	 //thd
	 rcName = QRect(rcName.left(), rcName.bottom() + 1, rcName.width(), rcName.height());
	 rcValue = QRect(rcValue.left(), rcValue.bottom() + 1, rcValue.width(), rcValue.height());
	 painter->drawText(rcName, Qt::AlignLeft | Qt::AlignVCenter, leftStr + getTheTranslatorString("THD"));
	 sprintf(tempChar, "%16.2f", sysData.options.fm_thd);
	 painter->drawText(rcValue, Qt::AlignLeft | Qt::AlignVCenter, leftStr + QString(tempChar).trimmed() + " dB");
	 rects << rcName << rcValue;
	 */
	painter->setPen(GRIDCOLOR);
	painter->drawRects(rects);
}

void mainForm::drawSourceNetworkMeasTable(QPainter* painter, QPixmap* pixmap, QRect rc)
{
	if (painter == NULL || !painter->isActive() || pixmap == NULL || pixmap->isNull())
	{
		return;
	}

	char S11Tex[6][20] = { "Marker", "Trace", "Frequency", "RL", "P", "SWR" };

	painter->fillRect(rc, Qt::black);
	painter->setPen(Qt::white);
	painter->setFont(fontData);

	double p = 0;
	double swr = 0;
	int topSep = 4;
	int rcNums = 6;
	char tempChar[32];
	QRect rcC;
	QRect rcS[rcNums];
	QVector<QRect> rects;
	int rcTableWidth = rc.width() - TABLELEFT - TABLERIGHT;
	int rcTableHeight = (rc.height() - 2 * TABLETOP) / 6;
	rcS[0] = QRect(TABLELEFT, TABLETOP, rcTableWidth * 0.10, rcTableHeight);
	rcS[1] = QRect(rcS[0].left() + rcS[0].width(), TABLETOP, rcS[0].width(), rcS[0].height());
	rcS[2] = QRect(rcS[1].left() + rcS[1].width(), TABLETOP, rcS[1].width() + 60, rcS[1].height());
	rcS[3] = QRect(rcS[2].left() + rcS[2].width(), TABLETOP, rcS[2].width(), rcS[2].height());
	rcS[4] = QRect(rcS[3].left() + rcS[3].width(), TABLETOP, rcS[3].width(), rcS[3].height());
	rcS[5] = QRect(rcS[4].left() + rcS[4].width(), TABLETOP, rcTableWidth - rcS[0].width() - rcS[1].width() - rcS[2].width() - rcS[3].width() - rcS[4].width(), rcS[4].height());

	int labelIndex = -1;

	painter->drawText(QRect(TABLELEFT, TABLETOP - 40, rcTableWidth, 40), Qt::AlignVCenter | Qt::AlignLeft, getTheTranslatorString("VSWR"));
	for (int i = 0; i < rcNums; i++)
	{
		labelIndex++;
		painter->drawText(rcS[labelIndex], Qt::AlignCenter, getTheTranslatorString(S11Tex[labelIndex]));
		rects << rcS[i];
	}

	labelIndex = -1;
	for (int i = 0; i < MARKERCOUNT; i++)
		if (sysData.marker[i].state == msNormal)
		{
			labelIndex++;

			for (int j = 0; j < rcNums; j++)
			{
				rcC = QRect(rcS[j].left(), rcS[j].top() + rcS[j].height() + labelIndex * rcS[0].height(), rcS[j].width(), rcS[j].height());

				switch (j)
				{
					case 0:
						painter->drawText(rcC, Qt::AlignCenter, QString(intToString(i + 1, tempChar)).trimmed());
						break;
					case 1:
						painter->drawText(rcC, Qt::AlignCenter, QString(intToString(sysData.marker[i].traceIndex + 1, tempChar)).trimmed());
						break;
					case 2:
						painter->drawText(rcC, Qt::AlignCenter, getTheMarkerXString(i, false));
						break;
					case 3:
						painter->drawText(rcC, Qt::AlignCenter, QString(floatToStringDot2(sysScpi->getMarkerYValue(i), tempChar)).trimmed() + " dB");
						break;
					case 4:
						painter->drawText(rcC, Qt::AlignCenter, QString(floatToStringDot6(qPow(10, -qAbs(sysScpi->getMarkerYValue(i)) / 20), tempChar)).trimmed());
						break;
					case 5:
						p = qPow(10, -qAbs(sysScpi->getMarkerYValue(i)) / 20);
						swr = 0;
						if (qAbs(1 - p) > 0)
						{
							swr = (1 + p) / (1 - p);
						}

						painter->drawText(rcC, Qt::AlignCenter, QString(floatToStringDot6(swr, tempChar)).trimmed());
						break;
						break;
				}

				rects << rcC;
			}
		}

	painter->setPen(GRIDCOLOR);
	painter->drawRects(rects);
}

void mainForm::drawMarkerTable(QPainter* painter, QPixmap* pixmap, QRect rc)
{
	if (sysData.source.measType == S11)
	{
		drawSourceNetworkMeasTable(painter, pixmap, rc);
		return;
	}

	if (painter == NULL || !painter->isActive() || pixmap == NULL || pixmap->isNull())
	{
		return;
	}

	getMarkerTableLabel();

	painter->fillRect(rc, Qt::black);
	painter->setPen(Qt::white);
	painter->setFont(fontData);

	int topSep = 4;
	int rcNums = 5;
	QRect rcS[rcNums];
	QVector<QRect> rects;
	int rcTableWidth = rc.width() - TABLELEFT - TABLERIGHT;
	int rcTableHeight = (rc.height() - 2 * TABLETOP) / 6;
	rcS[0] = QRect(TABLELEFT, TABLETOP, rcTableWidth * 0.16, rcTableHeight);
	rcS[1] = QRect(rcS[0].left() + rcS[0].width(), TABLETOP, rcS[0].width(), rcS[0].height());
	rcS[2] = QRect(rcS[1].left() + rcS[1].width(), TABLETOP, rcS[1].width(), rcS[1].height());
	rcS[3] = QRect(rcS[2].left() + rcS[2].width(), TABLETOP, rcTableWidth * 0.26, rcS[2].height());
	rcS[4] = QRect(rcS[3].left() + rcS[3].width(), TABLETOP, rcTableWidth - rcS[0].width() - rcS[1].width() - rcS[2].width() - rcS[3].width(), rcS[3].height());

	int labelIndex = -1;

	for (int i = 0; i < rcNums; i++)
	{
		labelIndex++;

		if (stLabelMarkerTable[labelIndex].textWidth() != rcS[i].width())
		{
			stLabelMarkerTable[labelIndex].setTextWidth(rcS[i].width());
		}

		painter->drawStaticText(rcS[i].left(), rcS[i].top() + topSep, stLabelMarkerTable[i]);
		rects << rcS[i];
	}

	for (int i = 0; i < MARKERCOUNT; i++)
	{
		if (sysData.marker[i].state != msClose)
		{
			for (int j = 0; j < rcNums; j++)
			{
				labelIndex++;
				rcS[j] = QRect(rcS[j].left(), rcS[j].top() + rcS[j].height(), rcS[j].width(), rcS[j].height());

				if (stLabelMarkerTable[labelIndex].textWidth() != rcS[j].width())
				{
					stLabelMarkerTable[labelIndex].setTextWidth(rcS[j].width());
				}

				painter->drawStaticText(rcS[j].left(), rcS[j].top() + topSep, stLabelMarkerTable[labelIndex]);
				rects << rcS[j];
			}
		} else
		{
			labelIndex += rcNums;
		}
	}

	painter->setPen(GRIDCOLOR);
	painter->drawRects(rects);
}

void mainForm::drawVector(QPainter* painter, QPixmap* pixmap, QRect rc)
{
	return;
	if (painter == NULL || !painter->isActive() || pixmap == NULL || pixmap->isNull())
	{
		return;
	}

	painter->fillRect(rc, Qt::black);

	//draw Table
	QRect rcTable;
	if (sysData.display.isFullScreen)
	{
		rcTable = QRect(TABLELEFT, TABLETOP, rc.width() - TABLELEFT - TABLERIGHT, rc.height() - 2 * TABLETOP);
	} else
	{
		rcTable = QRect(rc.width() - TABLEWIDTH - TABLERIGHT, TABLETOP, TABLEWIDTH, rc.height() - 2 * TABLETOP);
	}

	double rowHeight = rcTable.height() / 10.0;
	double colWidth = rcTable.width() / 10.0;
	QVector<QLine> lines;

	rcPreset = QRect(rc.width() - TABLERIGHT - 32, rcTable.top(), 32, 32);

	if (sysData.isPreseting)
	{
		drawPreset();
	}

	if (sysData.freq.typeLine)
	{
		for (int i = 0; i < 11; i++)
		{
			if (!sysData.display.isGridOn)
			{
				if (i == 0 || i == 10)
				{
					lines << QLine(rcTable.left(), rcTable.top() + rowHeight * i, rcTable.left() + rcTable.width(), rcTable.top() + rowHeight * i);
					lines << QLine(rcTable.left() + colWidth * i, rcTable.top(), rcTable.left() + colWidth * i, rcTable.top() + rcTable.height());
				}
			} else
			{
				lines << QLine(rcTable.left(), rcTable.top() + rowHeight * i, rcTable.left() + rcTable.width(), rcTable.top() + rowHeight * i);
				lines << QLine(rcTable.left() + colWidth * i, rcTable.top(), rcTable.left() + colWidth * i, rcTable.top() + rcTable.height());
			}

			if (i == 0)
			{
				lines << QLine(rcTable.left(), rcTable.top() + rowHeight * i + 1, rcTable.left() + rcTable.width(), rcTable.top() + rowHeight * i + 1);
				lines << QLine(rcTable.left(), rcTable.top() + rowHeight * i + 2, rcTable.left() + rcTable.width(), rcTable.top() + rowHeight * i + 2);
				lines << QLine(rcTable.left() + colWidth * i + 1, rcTable.top(), rcTable.left() + colWidth * i + 1, rcTable.top() + rcTable.height());
				lines << QLine(rcTable.left() + colWidth * i + 2, rcTable.top(), rcTable.left() + colWidth * i + 2, rcTable.top() + rcTable.height());
			}

			if (i == 5 && sysData.display.isGridOn)
			{
				lines << QLine(rcTable.left(), rcTable.top() + rowHeight * i - 1, rcTable.left() + rcTable.width(), rcTable.top() + rowHeight * i - 1);
				lines << QLine(rcTable.left(), rcTable.top() + rowHeight * i + 1, rcTable.left() + rcTable.width(), rcTable.top() + rowHeight * i + 1);
				lines << QLine(rcTable.left() + colWidth * i - 1, rcTable.top(), rcTable.left() + colWidth * i - 1, rcTable.top() + rcTable.height());
				lines << QLine(rcTable.left() + colWidth * i + 1, rcTable.top(), rcTable.left() + colWidth * i + 1, rcTable.top() + rcTable.height());
			}

			if (i == 10)
			{
				lines << QLine(rcTable.left(), rcTable.top() + rowHeight * i - 1, rcTable.left() + rcTable.width(), rcTable.top() + rowHeight * i - 1);
				lines << QLine(rcTable.left(), rcTable.top() + rowHeight * i - 2, rcTable.left() + rcTable.width(), rcTable.top() + rowHeight * i - 2);
				lines << QLine(rcTable.left() + colWidth * i - 1, rcTable.top(), rcTable.left() + colWidth * i - 1, rcTable.top() + rcTable.height());
				lines << QLine(rcTable.left() + colWidth * i - 2, rcTable.top(), rcTable.left() + colWidth * i - 2, rcTable.top() + rcTable.height());
			}

			//网络测量功能激活
			if (i == 5 && (sysData.source.isTrackGenNetworkOn || sysData.source.isVswrOn))
			{
				QPolygonF refPoints(4);
				double triangleWidth = 4.0;
				double xPos = 0;
				double yPos = rcTable.top() + rowHeight * i;

				//left triangle
				xPos = rcTable.left() - 1.0;
				refPoints[0] = QPoint(xPos, yPos);
				refPoints[1] = QPoint(xPos - triangleWidth, yPos - triangleWidth);
				refPoints[2] = QPoint(xPos - triangleWidth, yPos + triangleWidth);
				refPoints[3] = refPoints[0];
				painter->setBrush(Qt::white);
				painter->setPen(Qt::white);
				painter->drawPolygon(refPoints);

				//right triangle
				xPos = rcTable.left() + rcTable.width() + 1.0;
				refPoints[0] = QPoint(xPos, yPos);
				refPoints[1] = QPoint(xPos + triangleWidth, yPos - triangleWidth);
				refPoints[2] = QPoint(xPos + triangleWidth, yPos + triangleWidth);
				refPoints[3] = refPoints[0];
				painter->drawPolygon(refPoints);
			}
		}
	} else
	{
		for (int i = 0; i < 11; i++)
		{
			if (!sysData.display.isGridOn)
			{
				if (i == 0 || i == 10)
				{
					lines << QLine(rcTable.left(), rcTable.top() + rowHeight * i, rcTable.left() + rcTable.width(), rcTable.top() + rowHeight * i);
					lines << QLine(rcTable.left() + colWidth * i, rcTable.top(), rcTable.left() + colWidth * i, rcTable.top() + rcTable.height());
				}
			} else
			{
				lines << QLine(rcTable.left(), rcTable.top() + rowHeight * i, rcTable.left() + rcTable.width(), rcTable.top() + rowHeight * i);
			}

			if (i == 0)
			{
				lines << QLine(rcTable.left(), rcTable.top() + rowHeight * i + 0, rcTable.left() + rcTable.width(), rcTable.top() + rowHeight * i + 0);
				lines << QLine(rcTable.left(), rcTable.top() + rowHeight * i + 1, rcTable.left() + rcTable.width(), rcTable.top() + rowHeight * i + 1);
				lines << QLine(rcTable.left() + colWidth * i + 0, rcTable.top(), rcTable.left() + colWidth * i + 0, rcTable.top() + rcTable.height());
				lines << QLine(rcTable.left() + colWidth * i + 1, rcTable.top(), rcTable.left() + colWidth * i + 1, rcTable.top() + rcTable.height());
			}

			if (i == 10)
			{
				lines << QLine(rcTable.left(), rcTable.top() + rowHeight * i - 0, rcTable.left() + rcTable.width(), rcTable.top() + rowHeight * i - 0);
				lines << QLine(rcTable.left(), rcTable.top() + rowHeight * i - 1, rcTable.left() + rcTable.width(), rcTable.top() + rowHeight * i - 1);
				lines << QLine(rcTable.left() + colWidth * i - 0, rcTable.top(), rcTable.left() + colWidth * i - 0, rcTable.top() + rcTable.height());
				lines << QLine(rcTable.left() + colWidth * i - 1, rcTable.top(), rcTable.left() + colWidth * i - 1, rcTable.top() + rcTable.height());
			}
		}

		if (sysData.span.span > 0 && sysData.display.isGridOn)
		{
			int sIndex = sysScpi->getStartLogFreqIndex(sysData.freq.start - sysData.freq.offset + 1.0);
			int eIndex = sysScpi->getStopLogFreqIndex(sysData.freq.stop - sysData.freq.offset - 1.0);

			if (sIndex != -1 && eIndex != -1)
			{
				double sValue = log10(sysData.freq.start - sysData.freq.offset);
				double eValue = log10(sysData.freq.stop - sysData.freq.offset);
				double fValue = 0;

				for (int i = sIndex; i <= eIndex; i++)
				{
					fValue = log10(LOGFREQLIST[i]);
					lines << QLine(rcTable.left() + rcTable.width() * qAbs((fValue - sValue) / (eValue - sValue)), rcTable.top(), rcTable.left() + rcTable.width() * qAbs((fValue - sValue) / (eValue - sValue)), rcTable.top() + rcTable.height());
				}
			}
		}
	}

	painter->setPen(GRIDCOLOR);
	painter->drawLines(lines);

	//zero line
	if (sysData.disc.isDiscOn)
	{
		QPolygon zeroPic(3);
		int zeroSep = 6;

		painter->setPen(Qt::black);
		painter->setBrush(Qt::lightGray);

		zeroPic[0] = QPoint(rcTable.left() - zeroSep - 1, rcTable.top() + rowHeight * 5 - zeroSep);
		zeroPic[1] = QPoint(zeroPic[0].x(), zeroPic[0].y() + 2 * zeroSep);
		zeroPic[2] = QPoint(zeroPic[0].x() + zeroSep, zeroPic[0].y() + zeroSep);
		painter->drawPolygon(zeroPic);

		zeroPic[0] = QPoint(rcTable.right() + 1, rcTable.top() + rowHeight * 5);
		zeroPic[1] = QPoint(zeroPic[0].x() + zeroSep, zeroPic[0].y() + zeroSep);
		zeroPic[2] = QPoint(zeroPic[0].x() + zeroSep, zeroPic[0].y() - zeroSep);
		painter->drawPolygon(zeroPic);
	}

	//draw ampt graticule
	if (sysData.display.isAmptGraticuleOn || sysData.source.isTrackGenNetworkOn || sysData.source.isVswrOn)
	{
		getAmptGraticuleLabel();

		QRect rcAmpt;
		int sep = 8;
		int textHeight = 18;
		if (sysData.display.isFullScreen)
		{
			sep = 1;
		}

		rcAmpt = QRect(sep, rcTable.top() - textHeight / 2, rcTable.left() - sep - 2, textHeight);
		painter->setPen(Qt::white);
		painter->setFont(fontSmall);

		for (int i = 0; i < AMPTGRATICULECOUNT; i++)
		{
			rcAmpt = QRect(rcAmpt.left(), rcAmpt.top() + rowHeight, rcAmpt.width(), rcAmpt.height());

			if (stLabelGraticule[i].textWidth() != rcAmpt.width())
			{
				stLabelGraticule[i].setTextWidth(rcAmpt.width());
			}

			painter->drawStaticText(rcAmpt.left(), rcAmpt.top() + i * 0.5, stLabelGraticule[i]);
		}
	}

	//draw vector curve
	double canvasMaxValue = sysScpi->getRefLeveldBmValue() + sysData.ampt.refOffset;
	if (sysData.source.isTrackGenNetworkOn)
	{
		canvasMaxValue = sysData.source.trackGenNetworkRefLevel + 5.0 * sysData.ampt.scaleDiv;
	} else if (sysData.source.isVswrOn)
	{
		canvasMaxValue = sysData.source.vswrRef + 5.0 * sysData.ampt.scaleDiv;
	}

	double canvasMinValue = canvasMaxValue - qAbs(10.0 * sysData.ampt.scaleDiv);
	double scaleDiv = sysData.ampt.scaleDiv;

	if (sysData.disc.isDiscOn)
	{
		canvasMaxValue = sysData.disc.scaleDiv * 5.0;
		canvasMinValue = sysData.disc.scaleDiv * -5.0;
		scaleDiv = sysData.disc.scaleDiv;
	}

	if (sysData.ampt.scaleLine)
	{
		canvasMaxValue = sysData.ampt.refLevel;
		canvasMinValue = 0;
	}

	unsigned int swePoints = sysData.sweep.sweepPoints;
	if (swePoints < MINSWEEPPOINTS)
	{
		swePoints = MINSWEEPPOINTS;
	} else if (swePoints > MAXSWEEPPOINTS)
	{
		swePoints = MAXSWEEPPOINTS;
	}

	//1 double scaleX = (rcTable.width() * 1.0) / ((sysData.sweep.sweepPoints - 1) * 1.0);
	double scaleX = (rcTable.width() * 1.0) / ((swePoints - 1) * 1.0);
	double scaleY = (rcTable.height() * 1.0) / (scaleDiv * 10.0);
	double realValue = 0;
	double pixelValue = 0;

	//2 QPolygonF trace(sysData.sweep.sweepPoints);
	QPolygonF trace(swePoints);

	if ((!sysData.freq.typeLine) && (sysData.span.span > 0))
	{
		scaleX = (rcTable.width() * 1.0) / (log10(sysData.freq.stop - sysData.freq.offset) - log10(sysData.freq.start - sysData.freq.offset));
	}

	//S11测试归一化线
	if (sysData.source.isTrackGenNetworkOn && sysData.source.measType == S11 && sysData.source.trackNetworkStandard.normalized)
	{
		painter->setPen(Qt::darkMagenta);
		for (int i = 0; i < swePoints; i++)
		{
			trace[i].setX(i);
			realValue = sysData.source.trackNetworkStandard.normalizeData[i];
			if (realValue > canvasMaxValue)
			{
				realValue = canvasMaxValue;
			} else if (realValue < canvasMinValue)
			{
				realValue = canvasMinValue;
			}
			trace[i].setY((sysData.source.trackNetworkStandard.normalizeData[i] - canvasMaxValue) * (scaleDiv * 10.0) / (canvasMinValue - canvasMaxValue));
		}

		matrix.translate(rcTable.left(), rcTable.top());
		matrix.scale(scaleX, scaleY);
		matrix.shear(0, 0);
		painter->setMatrix(matrix);
		painter->drawPolyline(trace);

		//对称线
		painter->setPen(Qt::darkGreen);
		for (int i = 0; i < swePoints; i++)
		{
			trace[i].setX(i);
			realValue = 0 - sysData.prjValue[i];
			if (realValue > canvasMaxValue)
			{
				realValue = canvasMaxValue;
			} else if (realValue < canvasMinValue)
			{
				realValue = canvasMinValue;
			}
			trace[i].setY((realValue - canvasMaxValue) * (scaleDiv * 10.0) / (canvasMinValue - canvasMaxValue));
		}
		painter->drawPolyline(trace);

		//复位
		matrix.reset();
	}

	for (int i = 0; i < TRACECOUNT; i++)
	{
		if (sysData.trace[i].state != tsBlank)
		{
			//3 for (int j = 0; j < sysData.sweep.sweepPoints; j++)
			for (int j = 0; j < swePoints; j++)
			{
				realValue = sysData.trace[i].value[j];

				if (realValue > canvasMaxValue)
				{
					realValue = canvasMaxValue;
				} else if (realValue < canvasMinValue)
				{
					realValue = canvasMinValue;
				}

				//trace[j].setX(j);
				if ((sysData.freq.typeLine) || (((!sysData.freq.typeLine) && (sysData.span.span <= 0))))
				{
					trace[j].setX(j);
				} else if ((!sysData.freq.typeLine) && (sysData.span.span > 0))
				{
					//4 trace[j].setX(log10(sysData.freq.start - sysData.freq.offset + j * sysData.span.span / (sysData.sweep.sweepPoints - 1)) - log10(sysData.freq.start - sysData.freq.offset));
					trace[j].setX(log10(sysData.freq.start - sysData.freq.offset + j * sysData.span.span / (swePoints - 1)) - log10(sysData.freq.start - sysData.freq.offset));
				}

				if (sysData.ampt.isPreamptOn)
				{
					trace[j].setY((realValue - canvasMaxValue) * (scaleDiv * 10.0) / (canvasMinValue - canvasMaxValue));
				} else
				{
					trace[j].setY((realValue - canvasMaxValue) * (scaleDiv * 10.0) / (canvasMinValue - canvasMaxValue));
				}
			}

			//world matrix
			painter->setPen(QColor(sysData.trace[i].colorR, sysData.trace[i].colorG, sysData.trace[i].colorB));
			matrix.translate(rcTable.left(), rcTable.top());
			matrix.scale(scaleX, scaleY);
			matrix.shear(0, 0);
			painter->setMatrix(matrix);
			painter->drawPolyline(trace);

			//draw ndb
			if (sysData.markerFctn.isNdBOn)
			{
				if (sysData.marker[sysData.markerSelIndex].state != msClose && sysData.marker[sysData.markerSelIndex].traceIndex == i)
				{
					QPolygonF pfMarker(6);
					int ndBIndex = sysData.markerFctn.ndBIndex1;

					pfMarker[0] = QPointF(ndBIndex - (5 + 1) / scaleX, trace[ndBIndex].y());
					pfMarker[1] = QPointF(ndBIndex - (5 + 1) / scaleX, trace[ndBIndex].y() - 5 / scaleY);
					pfMarker[2] = QPointF(ndBIndex - (0 + 1) / scaleX, trace[ndBIndex].y());
					pfMarker[3] = QPointF(ndBIndex - (5 + 1) / scaleX, trace[ndBIndex].y() + 5 / scaleY);
					pfMarker[4] = QPointF(ndBIndex - (5 + 1) / scaleX, trace[ndBIndex].y());
					pfMarker[5] = QPointF(ndBIndex - (15 + 1) / scaleX, trace[ndBIndex].y());

					painter->setBrush(Qt::green);
					painter->setPen(Qt::green);
					painter->drawPolygon(pfMarker);

					ndBIndex = sysData.markerFctn.ndBIndex2;
					pfMarker[0] = QPointF(ndBIndex + (5 + 1) / scaleX, trace[ndBIndex].y());
					pfMarker[1] = QPointF(ndBIndex + (5 + 1) / scaleX, trace[ndBIndex].y() - 5 / scaleY);
					pfMarker[2] = QPointF(ndBIndex + (0 + 1) / scaleX, trace[ndBIndex].y());
					pfMarker[3] = QPointF(ndBIndex + (5 + 1) / scaleX, trace[ndBIndex].y() + 5 / scaleY);
					pfMarker[4] = QPointF(ndBIndex + (5 + 1) / scaleX, trace[ndBIndex].y());
					pfMarker[5] = QPointF(ndBIndex + (15 + 1) / scaleX, trace[ndBIndex].y());
					painter->drawPolygon(pfMarker);
				}
			}

			//draw marker
			for (int j = 0; j < MARKERCOUNT; j++)
			{
				if (sysData.marker[j].state != msClose && sysData.marker[j].traceIndex == i)
				{
					QPolygonF pfMarker(5);
					QRectF rcMarker;
					double y2Value = 0;
					double mkrDeltaY = 0;

					painter->setBrush(Qt::magenta);
					painter->setPen(Qt::black);

					if (sysData.marker[j].state == msNormal)
					{
						pfMarker[0] = QPointF(trace[sysData.marker[j].markerNormal.xIndex].x(), trace[sysData.marker[j].markerNormal.xIndex].y() - 1 / scaleY);
					} else if (sysData.marker[j].state == msDelta)
					{
						pfMarker[0] = QPointF(trace[sysData.marker[j].markerDelta.x1Index].x(), trace[sysData.marker[j].markerDelta.x1Index].y() - 1 / scaleY);
					}

					pfMarker[1] = QPointF(pfMarker[0].x() - 5 / scaleX, pfMarker[0].y() - 5 / scaleY);
					pfMarker[2] = QPointF(pfMarker[0].x(), pfMarker[0].y() - 10 / scaleY);
					pfMarker[3] = QPointF(pfMarker[0].x() + 5 / scaleX, pfMarker[0].y() - 5 / scaleY);
					pfMarker[4] = QPointF(pfMarker[0].x(), pfMarker[0].y());

					painter->drawPolygon(pfMarker);

					painter->setMatrixEnabled(false);
					rcMarker = QRectF(pfMarker[2].x() * scaleX - 10 + rcTable.left(), pfMarker[2].y() * scaleY - 16 + rcTable.top(), 20, 16);
					painter->setPen(Qt::white);
					painter->setFont(fontData);

					//draw pass-fail marker
					if (sysData.measure.passFail.winMeasOn && j == sysData.markerSelIndex)
					{
						rcMarker = QRectF(rcMarker.left() - 20, rcMarker.top() - rcMarker.height(), rcMarker.width() + 40, rcMarker.height() * 2);
						painter->setFont(fontInput);

						if (sysScpi->getMeasureOfPassFailOfWinMeas(j))
						{
							painter->setPen(Qt::green);
							painter->drawText(rcMarker, Qt::AlignCenter, "PASS");
						} else
						{
							painter->setPen(Qt::red);
							painter->drawText(rcMarker, Qt::AlignCenter, "FAIL");
						}

						painter->setFont(fontData);
						painter->setPen(Qt::white);
					} else if (sysData.measure.passFail.lineMeasOn && j == sysData.markerSelIndex)
					{
						rcMarker = QRectF(rcMarker.left() - 20, rcMarker.top() - rcMarker.height(), rcMarker.width() + 40, rcMarker.height() * 2);
						painter->setFont(fontInput);

						if (sysScpi->getMeasureOfPassFailOfLineMeas(j, canvasMinValue, canvasMaxValue))
						{
							painter->setPen(Qt::green);
							painter->drawText(rcMarker, Qt::AlignCenter, "PASS");
						} else
						{
							painter->setPen(Qt::red);
							painter->drawText(rcMarker, Qt::AlignCenter, "FAIL");
						}

						painter->setFont(fontData);
						painter->setPen(Qt::white);
					} else
					{
						painter->drawStaticText(rcMarker.left(), rcMarker.top(), stLabelMarker[j]);
					}

					painter->setMatrixEnabled(true);

					if (sysData.marker[j].state == msDelta)
					{
						//MarkerDelta_R
						y2Value = sysData.marker[j].markerDelta.y2Value;

						if (y2Value > sysData.ampt.refLevel)
						{
							y2Value = sysData.ampt.refLevel;
						}

						mkrDeltaY = (y2Value - canvasMaxValue) * (scaleDiv * 10.0) / (canvasMinValue - canvasMaxValue);
						pfMarker[0] = QPointF(trace[sysData.marker[j].markerDelta.x2Index].x(), mkrDeltaY - 1 / scaleY);
						pfMarker[1] = QPointF(pfMarker[0].x() - 5 / scaleX, pfMarker[0].y() - 5 / scaleY);
						pfMarker[2] = QPointF(pfMarker[0].x(), pfMarker[0].y() - 10 / scaleY);
						pfMarker[3] = QPointF(pfMarker[0].x() + 5 / scaleX, pfMarker[0].y() - 5 / scaleY);
						pfMarker[4] = QPointF(pfMarker[0].x(), pfMarker[0].y());
						painter->setPen(Qt::black);
						painter->drawPolygon(pfMarker);
						painter->setMatrixEnabled(false);
						rcMarker = QRectF(pfMarker[2].x() * scaleX - 10 + rcTable.left(), pfMarker[2].y() * scaleY - 16 + rcTable.top(), 20, 16);
						painter->setPen(Qt::white);
						painter->setFont(fontData);
						painter->drawStaticText(rcMarker.left(), rcMarker.top(), stLabelMarkerR[j]);
						painter->setMatrixEnabled(true);
					}
				}
			}

			matrix.reset();
			painter->resetMatrix();

			if (sysData.freq.lineZero)
			{
				sysData.freq.lineZero = false;
			}
		}
	}

	//draw trig line
	if (sysData.trig.isDisplayVideoLine)
	{
		QPolygonF line(2);
		matrix.translate(rcTable.left(), rcTable.top());
		matrix.scale(scaleX, scaleY);
		matrix.shear(0, 0);
		painter->setMatrix(matrix);
		line[0].setX(0);
//		realValue = sysScpi->getdBmValue(sysData.trig.power);
		if(sysData.ampt.scaleLine)
			realValue = sysData.trig.power;
		else
			realValue = sysScpi->getdBmValue(sysData.trig.power);

		if (realValue > canvasMaxValue)
		{
			realValue = canvasMaxValue;
		} else if (realValue < canvasMinValue)
		{
			realValue = canvasMinValue;
		}

		line[0].setY((realValue - canvasMaxValue) * (sysData.ampt.scaleDiv * 10.0) / (canvasMinValue - canvasMaxValue));
		//5 line[1].setX(sysData.sweep.sweepPoints - 1);
		line[1].setX(swePoints - 1);
		line[1].setY(line[0].y());
		painter->setPen(Qt::green);
		painter->drawPolygon(line);
		matrix.reset();
		painter->resetMatrix();
	}

	//draw display line
	if (sysData.display.isDispLineOn)
	{
		QPolygonF line(2);
		matrix.translate(rcTable.left(), rcTable.top());
		matrix.scale(scaleX, scaleY);
		matrix.shear(0, 0);
		painter->setMatrix(matrix);
		line[0].setX(0);
		realValue = sysScpi->getdBmValue(sysData.display.dispLineValue);

		if (realValue > canvasMaxValue)
		{
			realValue = canvasMaxValue;
		} else if (realValue < canvasMinValue)
		{
			realValue = canvasMinValue;
		}

		line[0].setY((realValue - canvasMaxValue) * (sysData.ampt.scaleDiv * 10.0) / (canvasMinValue - canvasMaxValue));
		//6 line[1].setX(sysData.sweep.sweepPoints - 1);
		line[1].setX(swePoints - 1);
		line[1].setY(line[0].y());
		painter->setPen(Qt::green);
		painter->drawPolygon(line);
		matrix.reset();
		painter->resetMatrix();
	}

	//draw pass-fail window measure ampt line
	if (sysData.measure.passFail.winAmptOn)
	{
		QPolygonF line(2);
		matrix.translate(rcTable.left(), rcTable.top());
		matrix.scale(scaleX, scaleY);
		matrix.shear(0, 0);
		painter->setMatrix(matrix);

		//draw up
		line[0].setX(0);
		realValue = sysScpi->getdBmValue(sysData.measure.passFail.winAmptUp);

		if (realValue > canvasMaxValue)
		{
			realValue = canvasMaxValue;
		} else if (realValue < canvasMinValue)
		{
			realValue = canvasMinValue;
		}

		line[0].setY((realValue - canvasMaxValue) * (sysData.ampt.scaleDiv * 10.0) / (canvasMinValue - canvasMaxValue));
		//7 line[1].setX(sysData.sweep.sweepPoints - 1);
		line[1].setX(swePoints - 1);
		line[1].setY(line[0].y());
		painter->setPen(Qt::green);
		painter->drawPolygon(line);

		//draw low
		line[0].setX(0);
		realValue = sysScpi->getdBmValue(sysData.measure.passFail.winAmptLow);

		if (realValue > canvasMaxValue)
		{
			realValue = canvasMaxValue;
		} else if (realValue < canvasMinValue)
		{
			realValue = canvasMinValue;
		}

		line[0].setY((realValue - canvasMaxValue) * (sysData.ampt.scaleDiv * 10.0) / (canvasMinValue - canvasMaxValue));
		//8 line[1].setX(sysData.sweep.sweepPoints - 1);
		line[1].setX(swePoints - 1);
		line[1].setY(line[0].y());
		painter->setPen(Qt::green);
		painter->drawPolygon(line);

		//reset matrix
		matrix.reset();
		painter->resetMatrix();
	}

	//draw pass-fail window measure freq line
	if (sysData.measure.passFail.winFreqOn)
	{
		QPolygonF line(2);
		double startFreq = sysData.freq.start + sysData.freq.offset;
		double stopFreq = sysData.freq.stop + sysData.freq.offset;
		double freq = sysData.display.freqLineStart;
		double xValue = 0;

		matrix.translate(rcTable.left(), rcTable.top());
		matrix.scale(scaleX, scaleY);
		matrix.shear(0, 0);
		painter->setMatrix(matrix);

		//draw freq start
		if ((stopFreq - startFreq) <= 0)
		{
			xValue = 0;
		} else
		{
			//9 xValue = (sysData.measure.passFail.winFreqStart - startFreq) * (sysData.sweep.sweepPoints - 1) / (stopFreq - startFreq);
			xValue = (sysData.measure.passFail.winFreqStart - startFreq) * (swePoints - 1) / (stopFreq - startFreq);
		}

		if (xValue <= 0)
		{
			xValue = 0;
			//} else if (xValue > sysData.sweep.sweepPoints - 1)
		} else if (xValue > swePoints - 1)
		{
			//11 xValue = sysData.sweep.sweepPoints - 1;
			xValue = swePoints - 1;
		}

		line[0].setX(xValue);
		line[0].setY(0);
		line[1].setX(line[0].x());
		line[1].setY((canvasMinValue - canvasMaxValue) * (sysData.ampt.scaleDiv * 10.0) / (canvasMinValue - canvasMaxValue));
		painter->setPen(Qt::green);
		painter->drawPolygon(line);

		if ((stopFreq - startFreq) <= 0)
		{
			xValue = 0;
		} else
		{
			//12 xValue = (sysData.measure.passFail.winFreqStop - startFreq) * (sysData.sweep.sweepPoints - 1) / (stopFreq - startFreq);
			xValue = (sysData.measure.passFail.winFreqStop - startFreq) * (swePoints - 1) / (stopFreq - startFreq);
		}

		if (xValue <= 0)
		{
			xValue = 0;
			//} else if (xValue >= sysData.sweep.sweepPoints - 1)
		} else if (xValue >= swePoints - 1)
		{
			//14 xValue = sysData.sweep.sweepPoints - 1;
			xValue = swePoints - 1;
		}

		line[0].setX(xValue);
		line[1].setY(0);
		line[1].setX(line[0].x());
		line[1].setY((canvasMinValue - canvasMaxValue) * (sysData.ampt.scaleDiv * 10.0) / (canvasMinValue - canvasMaxValue));
		painter->drawPolygon(line);

		matrix.reset();
		painter->resetMatrix();
	}

	//draw pass-fail line measure up line
	if (sysData.measure.passFail.line1On && sysData.measure.passFail.line1TotalPoints > 0 && sysData.measure.passFail.line1TotalPoints < MAXPASSFAILROWS)
	{
		int points = sysData.measure.passFail.line1TotalPoints;
		double freq = 0;
		double ampt = 0;
		double xValue = 0;
		double yValue = 0;
		double startFreq = sysData.freq.start + sysData.freq.offset;
		double stopFreq = sysData.freq.stop + sysData.freq.offset;
		QPolygonF line(MAXPASSFAILROWS);

		if (points > MAXPASSFAILROWS)
		{
			points = MAXPASSFAILROWS;
		}

		mutexPassFail.lock();

		matrix.translate(rcTable.left(), rcTable.top());
		matrix.scale(scaleX, scaleY);
		matrix.shear(0, 0);
		painter->setMatrix(matrix);

		for (int i = 0; i < points; i++)
		{
			freq = sysData.measure.passFail.line1Datas[i].freq + sysData.measure.passFail.lineShiftXOffset;
			ampt = sysData.measure.passFail.line1Datas[i].ampt + sysData.measure.passFail.lineShiftYOffset;

			//draw freq
			if ((stopFreq - startFreq) <= 0)
			{
				xValue = 0;
			} else
			{
				//15 xValue = (freq - startFreq) * (sysData.sweep.sweepPoints - 1) / (stopFreq - startFreq);
				xValue = (freq - startFreq) * (swePoints - 1) / (stopFreq - startFreq);
			}

			if (xValue <= 0)
			{
				xValue = 0;
				//16 } else if (xValue > sysData.sweep.sweepPoints - 1)
			} else if (xValue > swePoints - 1)
			{
				//17 xValue = sysData.sweep.sweepPoints - 1;
				xValue = swePoints - 1;
			}

			//draw ampt
			if (ampt > sysData.ampt.refLevel)
			{
				ampt = sysData.ampt.refLevel;
			} else if (ampt < sysData.ampt.refLevel - 10.0 * sysData.ampt.scaleDiv)
			{
				ampt = sysData.ampt.refLevel - 10.0 * sysData.ampt.scaleDiv;
			}

			yValue = (ampt - canvasMaxValue) * (sysData.ampt.scaleDiv * 10.0) / (canvasMinValue - canvasMaxValue);

			line[i].setX(xValue);
			line[i].setY(yValue);
		}

		painter->setPen(Qt::green);
		painter->setBrush(Qt::green);

		for (int i = 0; i < points; i++)
		{
			painter->drawEllipse(QPointF(line[i].x(), line[i].y()), 3 / scaleX, 3 / scaleY);

			if (i > 0)
			{
				painter->drawLine(line[i - 1].x(), line[i - 1].y(), line[i].x(), line[i].y());
			}
		}

		matrix.reset();
		painter->resetMatrix();
		mutexPassFail.unlock();
	}

	//draw pass-fail line measure low line
	if (sysData.measure.passFail.line2On && sysData.measure.passFail.line2TotalPoints > 0 && sysData.measure.passFail.line2TotalPoints < MAXPASSFAILROWS)
	{
		int points = sysData.measure.passFail.line2TotalPoints;
		double freq = 0;
		double ampt = 0;
		double xValue = 0;
		double yValue = 0;
		double startFreq = sysData.freq.start + sysData.freq.offset;
		double stopFreq = sysData.freq.stop + sysData.freq.offset;
		QPolygonF line(MAXPASSFAILROWS);

		if (points > MAXPASSFAILROWS)
		{
			points = MAXPASSFAILROWS;
		}

		mutexPassFail.lock();

		matrix.translate(rcTable.left(), rcTable.top());
		matrix.scale(scaleX, scaleY);
		matrix.shear(0, 0);
		painter->setMatrix(matrix);

		for (int i = 0; i < points; i++)
		{
			freq = sysData.measure.passFail.line2Datas[i].freq + sysData.measure.passFail.lineShiftXOffset;
			ampt = sysData.measure.passFail.line2Datas[i].ampt + sysData.measure.passFail.lineShiftYOffset;

			//draw freq
			if ((stopFreq - startFreq) <= 0)
			{
				xValue = 0;
			} else
			{
				//18 xValue = (freq - startFreq) * (sysData.sweep.sweepPoints - 1) / (stopFreq - startFreq);
				xValue = (freq - startFreq) * (swePoints - 1) / (stopFreq - startFreq);
			}

			if (xValue <= 0)
			{
				xValue = 0;
				//19 } else if (xValue > sysData.sweep.sweepPoints - 1)
			} else if (xValue > swePoints - 1)
			{
				//20 xValue = sysData.sweep.sweepPoints - 1;
				xValue = swePoints - 1;
			}

			//draw ampt
			if (ampt > sysData.ampt.refLevel)
			{
				ampt = sysData.ampt.refLevel;
			} else if (ampt < sysData.ampt.refLevel - 10.0 * sysData.ampt.scaleDiv)
			{
				ampt = sysData.ampt.refLevel - 10.0 * sysData.ampt.scaleDiv;
			}

			yValue = (ampt - canvasMaxValue) * (sysData.ampt.scaleDiv * 10.0) / (canvasMinValue - canvasMaxValue);

			line[i].setX(xValue);
			line[i].setY(yValue);
		}

		painter->setPen(Qt::green);
		painter->setBrush(Qt::green);

		for (int i = 0; i < points; i++)
		{
			painter->drawEllipse(QPointF(line[i].x(), line[i].y()), 3 / scaleX, 3 / scaleY);

			if (i > 0)
			{
				painter->drawLine(line[i - 1].x(), line[i - 1].y(), line[i].x(), line[i].y());
			}
		}

		matrix.reset();
		painter->resetMatrix();
		mutexPassFail.unlock();
	}

	//draw measure
	if (sysData.measure.isChannelPowerOn)
	{
		if (sysData.span.span > 0)
		{
			QBrush brush;
			QColor color = QColor(150, 200, 200);
			double measWidth = sysData.measureSetup.channelWidth * rcTable.width() / sysData.span.span;
			QRectF rcMeas = QRectF(rcTable.left() + (rcTable.width() - measWidth) / 2, rcTable.top(), measWidth, rcTable.height());

			brush.setColor(color);
			brush.setStyle(Qt::Dense7Pattern);
			painter->fillRect(rcMeas, brush);
			painter->setPen(color);
			painter->drawLine(rcMeas.left(), rcMeas.top(), rcMeas.left(), rcMeas.top() + rcMeas.height());
			painter->drawLine(rcMeas.left() + rcMeas.width(), rcMeas.top(), rcMeas.left() + rcMeas.width(), rcMeas.top() + rcMeas.height());
		}
	} else if (sysData.measure.isAcprOn)
	{
		if (sysData.span.span > 0)
		{
			QBrush brush;
			QColor color = QColor(150, 200, 200);
			//21 int count = sysData.sweep.sweepPoints - 1;
			int count = swePoints - 1;
			int xl = sysData.measure.channelPowerStartX;
			int xr = sysData.measure.channelPowerStopX;

			QRectF rcMeas = QRectF(rcTable.left() + xl * rcTable.width() / count, rcTable.top(), rcTable.width() * (xr - xl) / count, rcTable.height());

			if (rcMeas.left() < rcTable.left())
				rcMeas = QRect(rcTable.left(), rcMeas.top(), rcMeas.width() - (rcTable.left() - rcMeas.left()), rcMeas.height());
			if (rcMeas.right() > rcTable.right())
				rcMeas = QRect(rcMeas.left(), rcMeas.top(), rcMeas.width() - (rcMeas.right() - rcTable.right()), rcMeas.height());

			brush.setColor(color);
			brush.setStyle(Qt::HorPattern);
			painter->fillRect(rcMeas, brush);
			painter->setPen(color);
			painter->drawLine(rcMeas.left(), rcMeas.top(), rcMeas.left(), rcMeas.top() + rcMeas.height());
			painter->drawLine(rcMeas.left() + rcMeas.width(), rcMeas.top(), rcMeas.left() + rcMeas.width(), rcMeas.top() + rcMeas.height());

			brush.setStyle(Qt::BDiagPattern);
			painter->setBrush(brush);

			for (int i = 0; i < sysData.measureSetup.channelNums; i++)
			{
				xl = sysData.measure.acprRects[i].startXL;
				xr = sysData.measure.acprRects[i].stopXL;
				rcMeas = QRectF(rcTable.left() + xl * rcTable.width() / count, rcTable.top(), rcTable.width() * (xr - xl) / count, rcTable.height());
				if (rcMeas.left() < rcTable.left())
					rcMeas = QRect(rcTable.left(), rcMeas.top(), rcMeas.width() - (rcTable.left() - rcMeas.left()), rcMeas.height());
				if (rcMeas.right() > rcTable.right())
					rcMeas = QRect(rcMeas.left(), rcMeas.top(), rcMeas.width() - (rcMeas.right() - rcTable.right()), rcMeas.height());
				painter->fillRect(rcMeas, brush);
				painter->drawLine(rcMeas.left(), rcMeas.top(), rcMeas.left(), rcMeas.top() + rcMeas.height());
				painter->drawLine(rcMeas.left() + rcMeas.width(), rcMeas.top(), rcMeas.left() + rcMeas.width(), rcMeas.top() + rcMeas.height());

				xl = sysData.measure.acprRects[i].startXR;
				xr = sysData.measure.acprRects[i].stopXR;
				rcMeas = QRectF(rcTable.left() + xl * rcTable.width() / count, rcTable.top(), rcTable.width() * (xr - xl) / count, rcTable.height());
				if (rcMeas.left() < rcTable.left())
					rcMeas = QRect(rcTable.left(), rcMeas.top(), rcMeas.width() - (rcTable.left() - rcMeas.left()), rcMeas.height());
				if (rcMeas.right() > rcTable.right())
					rcMeas = QRect(rcMeas.left(), rcMeas.top(), rcMeas.width() - (rcMeas.right() - rcTable.right()), rcMeas.height());
				painter->fillRect(rcMeas, brush);
				painter->drawLine(rcMeas.left(), rcMeas.top(), rcMeas.left(), rcMeas.top() + rcMeas.height());
				painter->drawLine(rcMeas.left() + rcMeas.width(), rcMeas.top(), rcMeas.left() + rcMeas.width(), rcMeas.top() + rcMeas.height());
			}
		}
	} else if (sysData.measure.isObwOn)
	{
		if (sysData.span.span > 0)
		{
			QBrush brush;
			QColor color = QColor(150, 200, 200);

			//22 double leftPos = sysData.measure.obwStartX * 1.0 / (sysData.sweep.sweepPoints - 1);
			double leftPos = sysData.measure.obwStartX * 1.0 / (swePoints - 1);
			//23 double rightPos = qAbs(sysData.measure.obwStopX - sysData.measure.obwStartX) * 1.0 / (sysData.sweep.sweepPoints - 1);
			double rightPos = qAbs(sysData.measure.obwStopX - sysData.measure.obwStartX) * 1.0 / (swePoints - 1);
			QRectF rcMeas = QRectF(rcTable.left() + rcTable.width() * leftPos, rcTable.top(), rcTable.width() * rightPos, rcTable.height());

			brush.setColor(color);
			brush.setStyle(Qt::Dense7Pattern);
			painter->fillRect(rcMeas, brush);
			painter->setPen(color);
			painter->drawLine(rcMeas.left(), rcMeas.top(), rcMeas.left(), rcMeas.top() + rcMeas.height());
			painter->drawLine(rcMeas.left() + rcMeas.width(), rcMeas.top(), rcMeas.left() + rcMeas.width(), rcMeas.top() + rcMeas.height());
		}
	}

	//draw am & fm information
	if (sysData.mode == smAM && sysData.options.amOn)
	{
		QRect rcAm1, rcAm2;
		double rowHeight = rcTable.height() / 10.0;
		double colWidth = rcTable.width() / 10.0;
		QFont fontDemod1 = painter->font();
		QFont fontDemod2 = painter->font();
		fontDemod2.setPointSize(24);
		painter->setFont(fontDemod2);
		rcAm1 = QRect(rcTable.left() + 10, rcTable.bottom() - rowHeight, colWidth * 5 - 10, rowHeight);
		rcAm2 = QRect(rcAm1.right() + 10, rcAm1.top(), rcAm1.width() - 10, rcAm1.height());
		painter->setPen(Qt::green);
		char tempChar[64] = { };
		QString amString1 = getTheTranslatorString("AM Freq");
		amString1 += ": ";
		QString amString2 = getTheTranslatorString("Depth");
		amString2 += ": ";
		amString1 += QString(getFreqAndUnitString(sysData.options.am_freq, 0, tempChar)).trimmed();
		sprintf(tempChar, "%16.1f", sysData.options.am_depth * 100);
		amString2 += QString(tempChar).trimmed() + "%";
		painter->drawText(rcAm1, Qt::AlignLeft | Qt::AlignVCenter, amString1);
		painter->drawText(rcAm2, Qt::AlignLeft | Qt::AlignVCenter, amString2);
		painter->setFont(fontDemod1);
	} else if (sysData.mode == smFM && sysData.options.fmOn)
	{
		QRect rcFm1, rcFm2;
		double rowHeight = rcTable.height() / 10.0;
		double colWidth = rcTable.width() / 10.0;
		QFont fontDemod1 = painter->font();
		QFont fontDemod2 = painter->font();
		fontDemod2.setPointSize(24);
		painter->setFont(fontDemod2);
		rcFm1 = QRect(rcTable.left() + 10, rcTable.bottom() - rowHeight, colWidth * 5 - 10, rowHeight);
		rcFm2 = QRect(rcFm1.right() + 10, rcFm1.top(), rcFm1.width() - 10, rcFm1.height());
		painter->setPen(Qt::green);
		char tempChar[64] = { };
		QString fmString1 = getTheTranslatorString("FM Freq");
		fmString1 += ": ";
		QString fmString2 = getTheTranslatorString("Deviation");
		fmString2 += ": ";
		fmString1 += QString(getFreqAndUnitString(sysData.options.fm_freq, 0, tempChar)).trimmed();
		sprintf(tempChar, "%16.1f", sysData.options.am_depth * 100);
		fmString2 += QString(getFreqAndUnitString(sysData.options.fm_deviation, 0, tempChar)).trimmed();
		painter->drawText(rcFm1, Qt::AlignLeft | Qt::AlignVCenter, fmString1);
		painter->drawText(rcFm2, Qt::AlignLeft | Qt::AlignVCenter, fmString2);
		painter->setFont(fontDemod1);
	}

	//get label content
	getVectorLabel();

	//draw label
	QRect rcLabel;
	int sep = 4;
	int labelIndex = -1;

	painter->setPen(Qt::white);
	painter->setFont(fontData);

	//c0r0: trace
	labelIndex++;
	rcLabel = QRect(rc.left() + TABLERIGHT, 0, (rc.width() - 2 * TABLERIGHT) / 4, TABLETOP / 2);
	if (sysData.display.isLabelOn)
	{
		if ((int) stLabel[labelIndex].textWidth() != rcLabel.width())
		{
			stLabel[labelIndex].setTextWidth(rcLabel.width());
		}
		painter->drawStaticText(rcLabel.left(), rcLabel.top() + sep, stLabel[labelIndex]);
	}

	//c1r0: detector
	labelIndex++;
	rcLabel = QRect(rcLabel.left() + rcLabel.width(), rcLabel.top(), rcLabel.width(), rcLabel.height());
	if (sysData.display.isLabelOn)
	{
		if ((int) stLabel[labelIndex].textWidth() != rcLabel.width())
		{
			stLabel[labelIndex].setTextWidth(rcLabel.width());
		}
		painter->drawStaticText(rcLabel.left(), rcLabel.top() + sep, stLabel[labelIndex]);
	}

	//c2r0: empty
	labelIndex++;
	rcLabel = QRect(rcLabel.left() + rcLabel.width(), rcLabel.top(), rcLabel.width(), rcLabel.height());
	if (sysData.display.isLabelOn)
	{
		if ((int) stLabel[labelIndex].textWidth() != rcLabel.width())
		{
			stLabel[labelIndex].setTextWidth(rcLabel.width());
		}
		painter->drawStaticText(rcLabel.left(), rcLabel.top() + sep, stLabel[labelIndex]);
	}

	//c3r0: empty
	labelIndex++;
	rcLabel = QRect(rcLabel.left() + rcLabel.width() - 200, rcLabel.top(), rcLabel.width() + 200, rcLabel.height());
	if (sysData.display.isLabelOn)
	{
		if ((int) stLabel[labelIndex].textWidth() != rcLabel.width())
		{
			stLabel[labelIndex].setTextWidth(rcLabel.width());
		}
		painter->drawStaticText(rcLabel.left(), rcLabel.top() + sep, stLabel[labelIndex]);
	}

	//c0r1: ref level
	labelIndex++;
	rcLabel = QRect(rc.left() + TABLERIGHT, TABLETOP / 2, (rc.width() - 2 * TABLERIGHT) / 4, TABLETOP / 2);
	if (sysData.display.isLabelOn)
	{
		if ((int) stLabel[labelIndex].textWidth() != rcLabel.width())
		{
			stLabel[labelIndex].setTextWidth(rcLabel.width());
		}
		painter->drawStaticText(rcLabel.left(), rcLabel.top() + sep, stLabel[labelIndex]);
	}

	//c1r1: att
	labelIndex++;
	rcLabel = QRect(rcLabel.left() + rcLabel.width(), rcLabel.top(), rcLabel.width(), rcLabel.height());
	if (sysData.display.isLabelOn)
	{
		if ((int) stLabel[labelIndex].textWidth() != rcLabel.width())
		{
			stLabel[labelIndex].setTextWidth(rcLabel.width());
		}
		painter->drawStaticText(rcLabel.left(), rcLabel.top() + sep, stLabel[labelIndex]);
	}

	//c2r1: empty
	labelIndex++;
	rcLabel = QRect(rcLabel.left() + rcLabel.width(), rcLabel.top(), rcLabel.width(), rcLabel.height());
	if (sysData.display.isLabelOn)
	{
		if ((int) stLabel[labelIndex].textWidth() != rcLabel.width())
		{
			stLabel[labelIndex].setTextWidth(rcLabel.width());
		}
		painter->drawStaticText(rcLabel.left(), rcLabel.top() + sep, stLabel[labelIndex]);
	}

	//c3r1: empty
	labelIndex++;
	rcLabel = QRect(rcLabel.left() + rcLabel.width(), rcLabel.top(), rcLabel.width(), rcLabel.height());
	if (sysData.display.isLabelOn)
	{
		if ((int) stLabel[labelIndex].textWidth() != rcLabel.width())
		{
			stLabel[labelIndex].setTextWidth(rcLabel.width());
		}
		painter->drawStaticText(rcLabel.left(), rcLabel.top() + sep, stLabel[labelIndex]);
	}

	//label: center or start freq
	labelIndex++;
	rcLabel = QRect(rc.left() + TABLERIGHT, rc.height() - TABLETOP, (rc.width() - 2 * TABLERIGHT) / 3, TABLETOP / 2);
	if (sysData.display.isLabelOn)
	{
		if ((int) stLabel[labelIndex].textWidth() != rcLabel.width())
		{
			stLabel[labelIndex].setTextWidth(rcLabel.width());
		}
		painter->drawStaticText(rcLabel.left(), rcLabel.top() + sep, stLabel[labelIndex]);
	}

	//label: empty
	labelIndex++;
	rcLabel = QRect(rcLabel.left() + rcLabel.width(), rcLabel.top(), rcLabel.width(), rcLabel.height());
	if (sysData.display.isLabelOn)
	{
		if ((int) stLabel[labelIndex].textWidth() != rcLabel.width())
		{
			stLabel[labelIndex].setTextWidth(rcLabel.width());
		}
		painter->drawStaticText(rcLabel.left(), rcLabel.top() + sep, stLabel[labelIndex]);
	}

	//label: span or stop freq
	labelIndex++;
	rcLabel = QRect(rcLabel.left() + rcLabel.width(), rcLabel.top(), rcLabel.width(), rcLabel.height());
	if (sysData.display.isLabelOn)
	{
		if ((int) stLabel[labelIndex].textWidth() != rcLabel.width())
		{
			stLabel[labelIndex].setTextWidth(rcLabel.width());
		}
		painter->drawStaticText(rcLabel.left(), rcLabel.top() + sep, stLabel[labelIndex]);
	}

	//label: rbw
	labelIndex++;
	rcLabel = QRect(rc.left() + TABLERIGHT, rc.height() - TABLETOP / 2, (rc.width() - 2 * TABLERIGHT) / 3, TABLETOP / 2);
	if (sysData.display.isLabelOn)
	{
		if ((int) stLabel[labelIndex].textWidth() != rcLabel.width())
		{
			stLabel[labelIndex].setTextWidth(rcLabel.width());
		}
		painter->drawStaticText(rcLabel.left(), rcLabel.top() + sep, stLabel[labelIndex]);
	}

	//label: vbw
	labelIndex++;
	rcLabel = QRect(rcLabel.left() + rcLabel.width(), rcLabel.top(), rcLabel.width(), rcLabel.height());
	if (sysData.display.isLabelOn)
	{
		if ((int) stLabel[labelIndex].textWidth() != rcLabel.width())
		{
			stLabel[labelIndex].setTextWidth(rcLabel.width());
		}
		painter->drawStaticText(rcLabel.left(), rcLabel.top() + sep, stLabel[labelIndex]);
	}

	//label: sweep time
	labelIndex++;
	rcLabel = QRect(rcLabel.left() + rcLabel.width(), rcLabel.top(), rcLabel.width(), rcLabel.height());
	if (sysData.display.isLabelOn)
	{
		if ((int) stLabel[labelIndex].textWidth() != rcLabel.width())
		{
			stLabel[labelIndex].setTextWidth(rcLabel.width());
		}
		painter->drawStaticText(rcLabel.left(), rcLabel.top() + sep, stLabel[labelIndex]);
	}

	int inputSep = 6;
	int inputHeight = 30;

	if (!inputTag.isEmpty())
	{
		labelIndex++;
		painter->setFont(fontInput);
		QRect rcInput = QRect(rcTable.left() + inputSep, rcTable.top() + 2 * rowHeight + inputSep / 2, rcTable.width() - 2 * inputSep, inputHeight);

		if ((int) stLabel[labelIndex].textWidth() != rcInput.width())
		{
			stLabel[labelIndex].setTextWidth(rcInput.width());
		}

		painter->drawStaticText(rcInput.left(), rcInput.top() + sep, stLabel[labelIndex]); //input name
		rcInput = QRect(rcInput.left(), rcInput.top() + rcInput.height(), rcInput.width(), rcInput.height());
		labelIndex++;

		if ((int) stLabel[labelIndex].textWidth() != rcInput.width())
		{
			stLabel[labelIndex].setTextWidth(rcInput.width());
		}

		if (inputValue.isEmpty())
		{
			painter->drawStaticText(rcInput.left(), rcInput.top() + sep, stLabel[labelIndex]); //tag value
		} else
		{
			painter->drawStaticText(rcInput.left(), rcInput.top() + sep, stLabel[labelIndex]); //input value
		}
	}

	if (sysData.markerFctn.isNdBOn)
	{
		QRect rcInput = QRect(rcTable.left() + inputSep, rcTable.top() + 2 * rowHeight + inputSep / 2, rcTable.width() - 2 * inputSep, inputHeight);
		rcInput = QRect(rcInput.left(), rcInput.top() + rcInput.height(), rcInput.width(), rcInput.height());
		rcInput = QRect(rcInput.left(), rcInput.top() + rcInput.height(), rcInput.width(), rcInput.height());
		labelIndex = 16;

		QString ndBString = getTheMarkerXString(sysData.markerSelIndex, true);

		if (stLabel[labelIndex].text() != ndBString)
		{
			stLabel[labelIndex].setText(ndBString);
		}

		if ((int) stLabel[labelIndex].textWidth() != rcInput.width())
		{
			stLabel[labelIndex].setTextWidth(rcInput.width());
		}

		painter->setFont(fontInput);
		painter->setPen(Qt::green);
		painter->drawStaticText(rcInput.left(), rcInput.top() + sep, stLabel[labelIndex]);
	} else if (sysData.markerFctn.isMkrNoiseOn && nowMainKey != NULL && nowSubKey != NULL && nowSubKey->funcCode == KEY_FUNC_MARKERFCTN_NOISE)
	{
		if (stLabel[14].text() != stLabel[3].text())
		{
			stLabel[14].setText(stLabel[3].text());
		}

		if (stLabel[15].text() != stLabel[7].text())
		{
			stLabel[15].setText(stLabel[7].text());
		}

		painter->setFont(fontInput);
		QRect rcInput = QRect(rcTable.left() + inputSep, rcTable.top() + 2 * rowHeight + inputSep / 2, rcTable.width() - 2 * inputSep, inputHeight);

		if ((int) stLabel[14].textWidth() != rcInput.width())
		{
			stLabel[14].setTextWidth(rcInput.width());
		}

		painter->drawStaticText(rcInput.left(), rcInput.top() + sep, stLabel[14]);
		rcInput = QRect(rcInput.left(), rcInput.top() + rcInput.height(), rcInput.width(), rcInput.height());

		if ((int) stLabel[15].textWidth() != rcInput.width())
		{
			stLabel[15].setTextWidth(rcInput.width());
		}

		painter->drawStaticText(rcInput.left(), rcInput.top() + sep, stLabel[15]);
	} else if (sysData.markerFctn.isFreqCountOn && nowMainKey != NULL && nowSubKey != NULL && nowSubKey->funcCode == KEY_FUNC_MARKERFCTN_FREQCOUNT_FREQCOUNT)
	{
		if (stLabel[14].text() != stLabel[3].text())
		{
			stLabel[14].setText(stLabel[3].text());
		}

		if (stLabel[15].text() != stLabel[7].text())
		{
			stLabel[15].setText(stLabel[7].text());
		}

		painter->setFont(fontInput);
		QRect rcInput = QRect(rcTable.left() + inputSep, rcTable.top() + 2 * rowHeight + inputSep / 2, rcTable.width() - 2 * inputSep, inputHeight);

		if ((int) stLabel[14].textWidth() != rcInput.width())
		{
			stLabel[14].setTextWidth(rcInput.width());
		}

		painter->drawStaticText(rcInput.left(), rcInput.top() + sep, stLabel[14]);
		rcInput = QRect(rcInput.left(), rcInput.top() + rcInput.height(), rcInput.width(), rcInput.height());

		if ((int) stLabel[15].textWidth() != rcInput.width())
		{
			stLabel[15].setTextWidth(rcInput.width());
		}

		painter->drawStaticText(rcInput.left(), rcInput.top() + sep, stLabel[15]);
	} else if (sysData.measure.isChannelPowerOn && nowMainKey != NULL && nowSubKey != NULL && nowSubKey->funcCode == KEY_FUNC_MEAS_CHANNELPOWER)
	{
		char labelChar[32] = { };
		QString labelName = getTheTranslatorString("Channel Power");
		QString labelValue = QString(floatToString(sysData.measure.channelPower, 0, 2, labelChar)).trimmed() + " dBm";

		if (stLabel[14].text() != labelName)
		{
			stLabel[14].setText(labelName);
		}

		if (stLabel[15].text() != labelValue)
		{
			stLabel[15].setText(labelValue);
		}

		painter->setFont(fontInput);
		QRect rcInput = QRect(rcTable.left() + inputSep, rcTable.top() + 2 * rowHeight + inputSep / 2, rcTable.width() - 2 * inputSep, inputHeight);

		if ((int) stLabel[14].textWidth() != rcInput.width())
		{
			stLabel[14].setTextWidth(rcInput.width());
		}

		painter->drawStaticText(rcInput.left(), rcInput.top() + sep, stLabel[14]);
		rcInput = QRect(rcInput.left(), rcInput.top() + rcInput.height(), rcInput.width(), rcInput.height());

		if ((int) stLabel[15].textWidth() != rcInput.width())
		{
			stLabel[15].setTextWidth(rcInput.width());
		}

		painter->drawStaticText(rcInput.left(), rcInput.top() + sep, stLabel[15]);
	}

	rcCal = QRect(rcTable.left() + rcTable.width() / 4, rcTable.top() + rcTable.height() * 4 / 10, rcTable.width() / 2, rcTable.height() * 2 / 10);
}

void mainForm::drawZoom(QPainter* painter, QPixmap* pixmap, QRect rc)
{
	if (painter == NULL || !painter->isActive() || pixmap == NULL || pixmap->isNull())
	{
		return;
	}

	painter->fillRect(rc, Qt::black);

	//draw zoom pixmap
	if (sysData.display.isFullScreen)
	{
		painter->drawPixmap(0, 0, img_Zoom_Full);
	} else
	{
		painter->drawPixmap(0, 0, img_Zoom);
	}

	//draw Table
	QRect rcTable;
	if (sysData.display.isFullScreen)
	{
		rcTable = QRect(TABLELEFT, TABLETOP, rc.width() - TABLELEFT - TABLERIGHT, rc.height() - 2 * TABLETOP);
	} else
	{
		rcTable = QRect(rc.width() - TABLEWIDTH - TABLERIGHT, TABLETOP, TABLEWIDTH, rc.height() - 2 * TABLETOP);
	}

	double rowHeight = rcTable.height() / 10.0;
	double colWidth = rcTable.width() / 10.0;
	QVector<QLine> lines;

	double posLeftPercent = 0;
	double posRightPercent = 0;

	if (sysData.display.zoomSpan <= 0)
	{
		posLeftPercent = 0;
		posRightPercent = 1;
	} else
	{
		if (sysData.freq.start <= sysData.display.zoomStartFreq)
		{
			posLeftPercent = 0;
		} else if (sysData.freq.start >= sysData.display.zoomStopFreq)
		{
			posLeftPercent = 1;
		} else
		{
			posLeftPercent = (sysData.freq.start - sysData.display.zoomStartFreq) / sysData.display.zoomSpan;
		}

		if (sysData.freq.stop >= sysData.display.zoomStopFreq)
		{
			posRightPercent = 1;
		} else if (sysData.freq.stop <= sysData.display.zoomStartFreq)
		{
			posRightPercent = 0;
		} else
		{
			posRightPercent = (sysData.freq.stop - sysData.display.zoomStartFreq) / sysData.display.zoomSpan;
		}
	}

	lines << QLine(rcTable.left() + rcTable.width() * posLeftPercent, rcTable.top(), rcTable.left() + rcTable.width() * posLeftPercent, rcTable.top() + rcTable.height());
	lines << QLine(rcTable.left() + rcTable.width() * posRightPercent, rcTable.top(), rcTable.left() + rcTable.width() * posRightPercent, rcTable.top() + rcTable.height());

	painter->setPen(Qt::cyan);
	painter->drawLines(lines);
}

void mainForm::drawTimeSpec(QPainter* painter, QPixmap* pixmap, QRect rc)
{
	if (painter == NULL || !painter->isActive() || pixmap == NULL || pixmap->isNull())
	{
		return;
	}

	if (!sysData.measure.isTimeSpectrumOn)
	{
		return;
	}

	int sep = 16;

	QRect rcTable;

	if (sysData.display.isFullScreen)
	{
		rcTable = QRect(TABLELEFT, sep, rc.width() - TABLELEFT - TABLERIGHT, rc.height() - 2 * sep);
	} else
	{
		rcTable = QRect(rc.width() - TABLEWIDTH - TABLERIGHT, sep, TABLEWIDTH, rc.height() - 2 * sep);
	}

	painter->fillRect(rcTable, Qt::black);

	QRect rcGradient = QRect(rcTable.left() - 10, rcTable.top(), 5, rcTable.height());
	QLinearGradient gradient(0, 0, rcGradient.width(), rcGradient.height());
	gradient.setColorAt(0, Qt::darkRed);
	gradient.setColorAt(0.1, Qt::red);
	gradient.setColorAt(0.3, Qt::yellow);
	gradient.setColorAt(0.5, Qt::green);
	gradient.setColorAt(0.7, Qt::cyan);
	gradient.setColorAt(0.9, Qt::blue);
	gradient.setColorAt(1, Qt::darkBlue);
	painter->setBrush(gradient);
	painter->drawRect(rcGradient);

	double lineSep = rcGradient.height() / 20.0;
	QVector<QLineF> lines;

	for (int i = 0; i < 21; i++)
	{
		if ((i % 2) == 0)
		{
			lines << QLineF(rcGradient.left() - 6, rcGradient.top() + i * lineSep, rcGradient.left() - 2, rcGradient.top() + i * lineSep);
		} else
		{
			lines << QLineF(rcGradient.left() - 4, rcGradient.top() + i * lineSep, rcGradient.left() - 2, rcGradient.top() + i * lineSep);
		}
	}

	lines << QLineF(rcGradient.left() - 2, rcGradient.top(), rcGradient.left() - 2, rcGradient.bottom());
	painter->setPen(Qt::white);
	painter->drawLines(lines);

	//draw text
	getTimeSpecLabel();

	QRect rcAmpt;
	int textHeight = 18;

	rcAmpt = QRect(6, rcTable.top() - textHeight / 2, rcTable.left() - 18, textHeight);
	painter->setPen(Qt::white);
	painter->setFont(fontSmall);

	for (int i = 0; i < TIMESPECTEXTCOUNT; i++)
	{
		rcAmpt = QRect(rcAmpt.left(), rcTable.top() + i * 2 * lineSep - textHeight / 2, rcAmpt.width(), rcAmpt.height());

		if (stLabelTimeSpec[i].textWidth() != rcAmpt.width())
		{
			stLabelTimeSpec[i].setTextWidth(rcAmpt.width());
		}

		painter->drawStaticText(rcAmpt.left(), rcAmpt.top(), stLabelTimeSpec[i]);
	}

	//draw color data point on rcTable
	double xPos = rcTable.width() * 1.0 / (sysData.sweep.sweepPoints - 1);
	sysData.measure.timeSpecFlag++;

	if (sysData.measure.timeSpecFlag > 1 && (pixTimeSpec.width() - 1) != rcTable.width())
	{
		sysData.measure.timeSpecFlag = 1;
	}

	if (sysData.measure.timeSpecFlag <= rcTable.height())
	{
		if (sysData.measure.timeSpecFlag > 1)
		{
			painter->drawPixmap(rcTable.left(), rcTable.top() + 1, pixTimeSpec);
		}
	} else
	{
		sysData.measure.timeSpecFlag = rcTable.height();
		painter->drawPixmap(rcTable.left(), rcTable.top() + 1, pixTimeSpec);
	}

	for (int i = 0; i < sysData.sweep.sweepPoints; i++)
	{
		painter->setPen(getDataColor(sysData.trace[sysData.traceSelIndex].value[i]));
		painter->drawPoint(QPointF(rcTable.left() + i * xPos, rcTable.top()));
	}

	if (sysData.measure.timeSpecFlag >= rcTable.height())
	{
		pixTimeSpec = pixmap->copy(rcTable.left(), rcTable.top(), rcTable.width() + 1, sysData.measure.timeSpecFlag - 1);
	} else
	{
		pixTimeSpec = pixmap->copy(rcTable.left(), rcTable.top(), rcTable.width() + 1, sysData.measure.timeSpecFlag);
	}
}

void mainForm::drawAcpr(QPainter* painter, QPixmap* pixmap, QRect rc)
{
	if (painter == NULL || !painter->isActive() || pixmap == NULL || pixmap->isNull())
	{
		return;
	}

	if (!sysData.measure.isAcprOn)
	{
		return;
	}

	getAcprLabel();

	painter->fillRect(rc, Qt::black);
	painter->setPen(Qt::white);
	painter->setFont(fontData);

	QRectF rcS;
	QVector<QRectF> rects;
	int rcTableTop = 10;
	int rcTableWidth = rc.width() - TABLELEFT - TABLERIGHT;
	int rcTableHeight = (rc.height() - 2 * rcTableTop) / 11;
	int sep = 2;
	int nums = 3;
	QColor bkColor = QColor(60, 90, 90);

	//major
	rcS = QRectF(TABLELEFT, rcTableTop, rcTableWidth, rcTableHeight);
	rects << rcS;
	painter->fillRect(rcS, bkColor);
	setStaticTextWidth(&stLabelAcpr[0], rcS.width() - nums * 2 * sep);
	painter->drawStaticText(rcS.left() + nums * sep, rcS.top() + sep, stLabelAcpr[0]);

	rcS = QRectF(rcS.left(), rcS.top() + rcS.height(), rcTableWidth * 0.2, rcS.height());
	rects << rcS;
	setStaticTextWidth(&stLabelAcpr[1], rcS.width() - nums * 2 * sep);
	painter->drawStaticText(rcS.left() + nums * sep, rcS.top() + sep, stLabelAcpr[1]);

	rcS = QRectF(rcS.left() + rcS.width(), rcS.top(), rcTableWidth * 0.3, rcS.height());
	rects << rcS;
	setStaticTextWidth(&stLabelAcpr[2], rcS.width() - nums * 2 * sep);
	painter->drawStaticText(rcS.left() + nums * sep, rcS.top() + sep, stLabelAcpr[2]);

	rcS = QRectF(rcS.left() + rcS.width(), rcS.top(), rcTableWidth * 0.2, rcS.height());
	rects << rcS;
	setStaticTextWidth(&stLabelAcpr[3], rcS.width() - nums * 2 * sep);
	painter->drawStaticText(rcS.left() + nums * sep, rcS.top() + sep, stLabelAcpr[3]);

	rcS = QRectF(rcS.left() + rcS.width(), rcS.top(), TABLELEFT + rcTableWidth - rcS.right(), rcS.height());
	rects << rcS;
	setStaticTextWidth(&stLabelAcpr[4], rcS.width() - nums * 2 * sep);
	painter->drawStaticText(rcS.left() + nums * sep, rcS.top() + sep, stLabelAcpr[4]);

	//adjacent1
	if (sysData.measureSetup.channelNums >= 1)
	{
		rcS = QRectF(TABLELEFT, rcS.top() + rcS.height(), rcTableWidth, rcTableHeight);
		rects << rcS;
		painter->fillRect(rcS, bkColor);
		setStaticTextWidth(&stLabelAcpr[5], rcS.width() - nums * 2 * sep);
		painter->drawStaticText(rcS.left() + nums * sep, rcS.top() + sep, stLabelAcpr[5]);

		rcS = QRectF(rcS.left(), rcS.top() + rcS.height(), rcTableWidth * 0.2, rcS.height());
		rects << rcS;
		setStaticTextWidth(&stLabelAcpr[6], rcS.width() - nums * 2 * sep);
		painter->drawStaticText(rcS.left() + nums * sep, rcS.top() + sep, stLabelAcpr[6]);

		rcS = QRectF(rcS.left() + rcS.width(), rcS.top(), rcTableWidth * 0.3, rcS.height());
		rects << rcS;
		setStaticTextWidth(&stLabelAcpr[7], rcS.width() - nums * 2 * sep);
		painter->drawStaticText(rcS.left() + nums * sep, rcS.top() + sep, stLabelAcpr[7]);

		rcS = QRectF(rcS.left() + rcS.width(), rcS.top(), rcTableWidth * 0.2, rcS.height());
		rects << rcS;
		setStaticTextWidth(&stLabelAcpr[8], rcS.width() - nums * 2 * sep);
		painter->drawStaticText(rcS.left() + nums * sep, rcS.top() + sep, stLabelAcpr[8]);

		rcS = QRectF(rcS.left() + rcS.width(), rcS.top(), TABLELEFT + rcTableWidth - rcS.right(), rcS.height());
		rects << rcS;
		setStaticTextWidth(&stLabelAcpr[9], rcS.width() - nums * 2 * sep);
		painter->drawStaticText(rcS.left() + nums * sep, rcS.top() + sep, stLabelAcpr[9]);

		rcS = QRectF(TABLELEFT, rcS.top() + rcS.height(), rcTableWidth * 0.2, rcS.height());
		rects << rcS;
		setStaticTextWidth(&stLabelAcpr[10], rcS.width() - nums * 2 * sep);
		painter->drawStaticText(rcS.left() + nums * sep, rcS.top() + sep, stLabelAcpr[10]);

		rcS = QRectF(rcS.left() + rcS.width(), rcS.top(), rcTableWidth * 0.3, rcS.height());
		rects << rcS;
		setStaticTextWidth(&stLabelAcpr[11], rcS.width() - nums * 2 * sep);
		painter->drawStaticText(rcS.left() + nums * sep, rcS.top() + sep, stLabelAcpr[11]);

		rcS = QRectF(rcS.left() + rcS.width(), rcS.top(), rcTableWidth * 0.2, rcS.height());
		rects << rcS;
		setStaticTextWidth(&stLabelAcpr[12], rcS.width() - nums * 2 * sep);
		painter->drawStaticText(rcS.left() + nums * sep, rcS.top() + sep, stLabelAcpr[12]);

		rcS = QRectF(rcS.left() + rcS.width(), rcS.top(), TABLELEFT + rcTableWidth - rcS.right(), rcS.height());
		rects << rcS;
		setStaticTextWidth(&stLabelAcpr[13], rcS.width() - nums * 2 * sep);
		painter->drawStaticText(rcS.left() + nums * sep, rcS.top() + sep, stLabelAcpr[13]);
	}

	//adjacent2
	if (sysData.measureSetup.channelNums >= 2)
	{
		rcS = QRectF(TABLELEFT, rcS.top() + rcS.height(), rcTableWidth, rcTableHeight);
		rects << rcS;
		painter->fillRect(rcS, bkColor);
		setStaticTextWidth(&stLabelAcpr[14], rcS.width() - nums * 2 * sep);
		painter->drawStaticText(rcS.left() + nums * sep, rcS.top() + sep, stLabelAcpr[14]);

		rcS = QRectF(rcS.left(), rcS.top() + rcS.height(), rcTableWidth * 0.2, rcS.height());
		rects << rcS;
		setStaticTextWidth(&stLabelAcpr[15], rcS.width() - nums * 2 * sep);
		painter->drawStaticText(rcS.left() + nums * sep, rcS.top() + sep, stLabelAcpr[15]);

		rcS = QRectF(rcS.left() + rcS.width(), rcS.top(), rcTableWidth * 0.3, rcS.height());
		rects << rcS;
		setStaticTextWidth(&stLabelAcpr[16], rcS.width() - nums * 2 * sep);
		painter->drawStaticText(rcS.left() + nums * sep, rcS.top() + sep, stLabelAcpr[16]);

		rcS = QRectF(rcS.left() + rcS.width(), rcS.top(), rcTableWidth * 0.2, rcS.height());
		rects << rcS;
		setStaticTextWidth(&stLabelAcpr[17], rcS.width() - nums * 2 * sep);
		painter->drawStaticText(rcS.left() + nums * sep, rcS.top() + sep, stLabelAcpr[17]);

		rcS = QRectF(rcS.left() + rcS.width(), rcS.top(), TABLELEFT + rcTableWidth - rcS.right(), rcS.height());
		rects << rcS;
		setStaticTextWidth(&stLabelAcpr[18], rcS.width() - nums * 2 * sep);
		painter->drawStaticText(rcS.left() + nums * sep, rcS.top() + sep, stLabelAcpr[18]);

		rcS = QRectF(TABLELEFT, rcS.top() + rcS.height(), rcTableWidth * 0.2, rcS.height());
		rects << rcS;
		setStaticTextWidth(&stLabelAcpr[19], rcS.width() - nums * 2 * sep);
		painter->drawStaticText(rcS.left() + nums * sep, rcS.top() + sep, stLabelAcpr[19]);

		rcS = QRectF(rcS.left() + rcS.width(), rcS.top(), rcTableWidth * 0.3, rcS.height());
		rects << rcS;
		setStaticTextWidth(&stLabelAcpr[20], rcS.width() - nums * 2 * sep);
		painter->drawStaticText(rcS.left() + nums * sep, rcS.top() + sep, stLabelAcpr[20]);

		rcS = QRectF(rcS.left() + rcS.width(), rcS.top(), rcTableWidth * 0.2, rcS.height());
		rects << rcS;
		setStaticTextWidth(&stLabelAcpr[21], rcS.width() - nums * 2 * sep);
		painter->drawStaticText(rcS.left() + nums * sep, rcS.top() + sep, stLabelAcpr[21]);

		rcS = QRectF(rcS.left() + rcS.width(), rcS.top(), TABLELEFT + rcTableWidth - rcS.right(), rcS.height());
		rects << rcS;
		setStaticTextWidth(&stLabelAcpr[22], rcS.width() - nums * 2 * sep);
		painter->drawStaticText(rcS.left() + nums * sep, rcS.top() + sep, stLabelAcpr[22]);
	}

	//adjacent3
	if (sysData.measureSetup.channelNums >= 3)
	{
		rcS = QRectF(TABLELEFT, rcS.top() + rcS.height(), rcTableWidth, rcTableHeight);
		rects << rcS;
		painter->fillRect(rcS, bkColor);
		setStaticTextWidth(&stLabelAcpr[23], rcS.width() - nums * 2 * sep);
		painter->drawStaticText(rcS.left() + nums * sep, rcS.top() + sep, stLabelAcpr[23]);

		rcS = QRectF(rcS.left(), rcS.top() + rcS.height(), rcTableWidth * 0.2, rcS.height());
		rects << rcS;
		setStaticTextWidth(&stLabelAcpr[24], rcS.width() - nums * 2 * sep);
		painter->drawStaticText(rcS.left() + nums * sep, rcS.top() + sep, stLabelAcpr[24]);

		rcS = QRectF(rcS.left() + rcS.width(), rcS.top(), rcTableWidth * 0.3, rcS.height());
		rects << rcS;
		setStaticTextWidth(&stLabelAcpr[25], rcS.width() - nums * 2 * sep);
		painter->drawStaticText(rcS.left() + nums * sep, rcS.top() + sep, stLabelAcpr[25]);

		rcS = QRectF(rcS.left() + rcS.width(), rcS.top(), rcTableWidth * 0.2, rcS.height());
		rects << rcS;
		setStaticTextWidth(&stLabelAcpr[26], rcS.width() - nums * 2 * sep);
		painter->drawStaticText(rcS.left() + nums * sep, rcS.top() + sep, stLabelAcpr[26]);

		rcS = QRectF(rcS.left() + rcS.width(), rcS.top(), TABLELEFT + rcTableWidth - rcS.right(), rcS.height());
		rects << rcS;
		setStaticTextWidth(&stLabelAcpr[27], rcS.width() - nums * 2 * sep);
		painter->drawStaticText(rcS.left() + nums * sep, rcS.top() + sep, stLabelAcpr[27]);

		rcS = QRectF(TABLELEFT, rcS.top() + rcS.height(), rcTableWidth * 0.2, rcS.height());
		rects << rcS;
		setStaticTextWidth(&stLabelAcpr[28], rcS.width() - nums * 2 * sep);
		painter->drawStaticText(rcS.left() + nums * sep, rcS.top() + sep, stLabelAcpr[28]);

		rcS = QRectF(rcS.left() + rcS.width(), rcS.top(), rcTableWidth * 0.3, rcS.height());
		rects << rcS;
		setStaticTextWidth(&stLabelAcpr[29], rcS.width() - nums * 2 * sep);
		painter->drawStaticText(rcS.left() + nums * sep, rcS.top() + sep, stLabelAcpr[29]);

		rcS = QRectF(rcS.left() + rcS.width(), rcS.top(), rcTableWidth * 0.2, rcS.height());
		rects << rcS;
		setStaticTextWidth(&stLabelAcpr[30], rcS.width() - nums * 2 * sep);
		painter->drawStaticText(rcS.left() + nums * sep, rcS.top() + sep, stLabelAcpr[30]);

		rcS = QRectF(rcS.left() + rcS.width(), rcS.top(), TABLELEFT + rcTableWidth - rcS.right(), rcS.height());
		rects << rcS;
		setStaticTextWidth(&stLabelAcpr[31], rcS.width() - nums * 2 * sep);
		painter->drawStaticText(rcS.left() + nums * sep, rcS.top() + sep, stLabelAcpr[31]);
	}

	painter->setPen(GRIDCOLOR);
	painter->drawRects(rects);
}

void mainForm::drawObw(QPainter* painter, QPixmap* pixmap, QRect rc)
{
	if (painter == NULL || !painter->isActive() || pixmap == NULL || pixmap->isNull())
	{
		return;
	}

	if (!sysData.measure.isObwOn)
	{
		return;
	}

	getObwLabel();

	painter->fillRect(rc, Qt::black);
	painter->setPen(Qt::white);
	painter->setFont(fontData);

	QRectF rcS;
	QVector<QRectF> rects;
	int rcTableWidth = rc.width() - TABLELEFT - TABLERIGHT;
	int rcTableHeight = (rc.height() - 2 * TABLETOP) / 6;
	int sep = 4;

	rcS = QRectF(TABLELEFT, TABLETOP, rcTableWidth * 0.2, rcTableHeight);
	rects << rcS;
	setStaticTextWidth(&stLabelObw[0], rcS.width() - 2 * sep);
	painter->drawStaticText(rcS.left() + sep, rcS.top() + sep, stLabelObw[0]);

	rcS = QRect(rcS.left() + rcS.width(), rcS.top(), rcTableWidth * 0.4, rcTableHeight);
	rects << rcS;
	setStaticTextWidth(&stLabelObw[1], rcS.width() - 2 * sep);
	painter->drawStaticText(rcS.left() + sep, rcS.top() + sep, stLabelObw[1]);

	rcS = QRect(rcS.left() + rcS.width(), rcS.top(), rcTableWidth * 0.4, rcTableHeight);
	rects << rcS;
	setStaticTextWidth(&stLabelObw[2], rcS.width() - 2 * sep);
	painter->drawStaticText(rcS.left() + sep, rcS.top() + sep, stLabelObw[2]);

	rcS = QRect(TABLELEFT, rcS.top() + rcS.height(), rcTableWidth * 0.2, rcTableHeight);
	rects << rcS;
	setStaticTextWidth(&stLabelObw[3], rcS.width() - 2 * sep);
	painter->drawStaticText(rcS.left() + sep, rcS.top() + sep, stLabelObw[3]);
	rcS = QRect(rcS.left() + rcS.width(), rcS.top(), rcTableWidth * 0.4, rcTableHeight);
	rects << rcS;
	setStaticTextWidth(&stLabelObw[4], rcS.width() - 2 * sep);
	painter->drawStaticText(rcS.left() + sep, rcS.top() + sep, stLabelObw[4]);
	rcS = QRect(rcS.left() + rcS.width(), rcS.top(), rcTableWidth * 0.4, rcTableHeight);
	rects << rcS;
	setStaticTextWidth(&stLabelObw[5], rcS.width() - 2 * sep);
	painter->drawStaticText(rcS.left() + sep, rcS.top() + sep, stLabelObw[5]);

	rcS = QRect(TABLELEFT, rcS.top() + rcS.height(), rcTableWidth * 0.2, rcTableHeight);
	rects << rcS;
	setStaticTextWidth(&stLabelObw[6], rcS.width() - 2 * sep);
	painter->drawStaticText(rcS.left() + sep, rcS.top() + sep, stLabelObw[6]);
	rcS = QRect(rcS.left() + rcS.width(), rcS.top(), rcTableWidth * 0.4, rcTableHeight);
	rects << rcS;
	setStaticTextWidth(&stLabelObw[7], rcS.width() - 2 * sep);
	painter->drawStaticText(rcS.left() + sep, rcS.top() + sep, stLabelObw[7]);
	rcS = QRect(rcS.left() + rcS.width(), rcS.top(), rcTableWidth * 0.4, rcTableHeight);
	rects << rcS;
	setStaticTextWidth(&stLabelObw[8], rcS.width() - 2 * sep);
	painter->drawStaticText(rcS.left() + sep, rcS.top() + sep, stLabelObw[8]);

	painter->setPen(GRIDCOLOR);
	painter->drawRects(rects);
}

void mainForm::drawPassFailLineEdit(QPainter* painter, QPixmap* pixmap, QRect rc)
{
	if (painter == NULL || !painter->isActive() || pixmap == NULL || pixmap->isNull())
	{
		return;
	}

	painter->fillRect(rc, Qt::black);
	painter->setPen(Qt::white);
	painter->setFont(fontData);

	char temp[32] = { };
	QVector<QRectF> rects;
	QRectF rcNo, rcFreq, rcAmpt, rcTitle, rcPage, rcSelect;
	double rcTableWidth = rc.width() - TABLELEFT - TABLERIGHT;
	double rcHeight = (rc.height() - TABLETOP) / (PASSFAILCOUNTPERPAGE + 1);

	//draw title and pages
	rcTitle = QRectF(TABLELEFT, TABLETOP / 3, (rc.width() - TABLELEFT - TABLERIGHT) / 2, rcHeight);
	rcPage = QRect(rcTitle.right(), rcTitle.top(), rcTitle.width(), rcTitle.height());

	mutexPassFail.lock();

	if (sysData.measure.passFail.line1Editing)
	{
		painter->drawText(rcTitle, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("Upper Line"));

		if (sysData.measure.passFail.line1Pages > 0)
		{
			painter->drawText(rcPage, Qt::AlignVCenter | Qt::AlignRight, getTheTranslatorString("Pages") + "  " + QString(intToString(sysData.measure.passFail.line1NowPageIndex + 1, temp)).trimmed() + " / " + QString(intToString(sysData.measure.passFail.line1Pages, temp)).trimmed());
		} else
		{
			painter->drawText(rcPage, Qt::AlignVCenter | Qt::AlignRight, getTheTranslatorString("Pages") + "  " + "0 / 0");
		}
	} else if (sysData.measure.passFail.line2Editing)
	{
		painter->drawText(rcTitle, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("Lower Line"));

		if (sysData.measure.passFail.line2Pages > 0)
		{
			painter->drawText(rcPage, Qt::AlignVCenter | Qt::AlignRight, getTheTranslatorString("Pages") + "  " + QString(intToString(sysData.measure.passFail.line2NowPageIndex + 1, temp)).trimmed() + " / " + QString(intToString(sysData.measure.passFail.line2Pages, temp)).trimmed());
		} else
		{
			painter->drawText(rcPage, Qt::AlignVCenter | Qt::AlignRight, getTheTranslatorString("Pages") + "  0 / 0");
		}
	}

	//draw column header
	rcNo = QRectF(TABLELEFT, rcPage.bottom(), rcTableWidth * 0.2, rcHeight + 6);
	rcFreq = QRectF(rcNo.right(), rcNo.top(), rcTableWidth * 0.4, rcNo.height());
	rcAmpt = QRectF(rcFreq.right(), rcFreq.top(), rcTableWidth * 0.4, rcFreq.height());
	rects << rcNo << rcFreq << rcAmpt;
	painter->drawText(rcNo, Qt::AlignCenter, getTheTranslatorString("No"));
	painter->drawText(rcFreq, Qt::AlignCenter, getTheTranslatorString("Frequency"));
	painter->drawText(rcAmpt, Qt::AlignCenter, getTheTranslatorString("Amplitude"));

	int absIndex = 0;

	if (sysData.measure.passFail.line1Editing)
	{
		for (int i = 0; i < sysData.measure.passFail.line1NowPagePoints; i++)
		{
			rcNo = QRectF(rcNo.left(), rcNo.bottom(), rcNo.width(), rcHeight);
			rcFreq = QRectF(rcFreq.left(), rcFreq.bottom(), rcFreq.width(), rcHeight);
			rcAmpt = QRectF(rcAmpt.left(), rcAmpt.bottom(), rcAmpt.width(), rcHeight);
			rects << rcNo << rcFreq << rcAmpt;

			absIndex = (sysData.measure.passFail.line1NowPageIndex) * PASSFAILCOUNTPERPAGE;
			painter->drawText(rcNo, Qt::AlignCenter, QString(intToString(absIndex + 1 + i, temp)).trimmed());
			painter->drawText(rcFreq, Qt::AlignCenter, ((QString) getFreqAndUnitString(sysData.measure.passFail.line1Datas[absIndex + i].freq, 1, temp)).trimmed());
			painter->drawText(rcAmpt, Qt::AlignCenter, ((QString) sysScpi->getAmptAndUnitString(sysData.measure.passFail.line1Datas[absIndex + i].ampt, temp, 0)).trimmed());

			if (i == sysData.measure.passFail.line1EditNowSelIndex)
			{
				rcSelect = QRectF(rcNo.left(), rcNo.top(), rcAmpt.right() - rcNo.left(), rcNo.height());
			}
		}
	} else if (sysData.measure.passFail.line2Editing)
	{
		for (int i = 0; i < sysData.measure.passFail.line2NowPagePoints; i++)
		{
			rcNo = QRectF(rcNo.left(), rcNo.bottom(), rcNo.width(), rcHeight);
			rcFreq = QRectF(rcFreq.left(), rcFreq.bottom(), rcFreq.width(), rcHeight);
			rcAmpt = QRectF(rcAmpt.left(), rcAmpt.bottom(), rcAmpt.width(), rcHeight);
			rects << rcNo << rcFreq << rcAmpt;

			absIndex = (sysData.measure.passFail.line2NowPageIndex) * PASSFAILCOUNTPERPAGE;
			painter->drawText(rcNo, Qt::AlignCenter, QString(intToString(absIndex + 1 + i, temp)).trimmed());
			painter->drawText(rcFreq, Qt::AlignCenter, ((QString) getFreqAndUnitString(sysData.measure.passFail.line2Datas[absIndex + i].freq, 1, temp)).trimmed());
			painter->drawText(rcAmpt, Qt::AlignCenter, ((QString) sysScpi->getAmptAndUnitString(sysData.measure.passFail.line2Datas[absIndex + i].ampt, temp, 0)).trimmed());

			if (i == sysData.measure.passFail.line2EditNowSelIndex)
			{
				rcSelect = QRectF(rcNo.left(), rcNo.top(), rcAmpt.right() - rcNo.left(), rcNo.height());
			}
		}
	}

	mutexPassFail.unlock();
	painter->setPen(GRIDCOLOR);
	painter->drawRects(rects);
	painter->setPen(QColor(0x00, 0xff, 0x80));
	painter->drawRect(rcSelect);
}

//绘制告警信息
void mainForm::showAlaramMessages(QString title, QString msg)
{
	sysLocked = true;
	sysScpi->showRunAlarm(title);
	usleep(1000 * 1500);
	helpContent = getTheTranslatorString("Ready");
	drawHelp();
	sysScpi->showRunAlarm(msg);
	usleep(1000 * 1500);
	sysLocked = false;
}

//刷新菜单面板
void mainForm::refreshMenuFrame(unsigned long mainCode, unsigned long subCode)
{
	unsigned long mCode = mainCode;
	unsigned long sCode = subCode;
	unsigned long fCode = 0;

	if (sysKey != NULL && sysKey->rightNode != NULL && mCode == 0x0 && sCode == 0x0)
	{
		mCode = sysKey->keyValue;
		sCode = sysKey->rightNode->funcCode;
	}

	switch (mCode)
	{
		case KEY_MAIN_FREQ:
			if (sCode == KEY_FUNC_FREQ_CENTER ||
					sCode == KEY_FUNC_FREQ_START ||
					sCode == KEY_FUNC_FREQ_STOP ||
					sCode == KEY_FUNC_FREQ_CFSTEP ||
					sCode == KEY_FUNC_FREQ_OFFSET ||
					sCode == KEY_FUNC_FREQ_REFERENCE ||
					sCode == KEY_FUNC_FREQ_SCALETYPE)
			{
				drawButton();
			}

			break;

		case KEY_MAIN_SPAN:
			if (sCode == KEY_FUNC_SPAN_SPAN ||
					sCode == KEY_FUNC_SPAN_FULL ||
					sCode == KEY_FUNC_SPAN_ZERO ||
					sCode == KEY_FUNC_SPAN_LAST)
			{
				drawButton();
			}

			break;

		case KEY_MAIN_AMPT:
			if (sCode == KEY_FUNC_AMPT_REFLEVEL ||
					sCode == KEY_FUNC_AMPT_ATT ||
					sCode == KEY_FUNC_AMPT_SCALEDIV ||
					sCode == KEY_FUNC_AMPT_SCALETYPE ||
					sCode == KEY_FUNC_AMPT_REFOFFSET ||
					sCode == KEY_FUNC_AMPT_PREAMPT)
			{
				drawButton();
			} else if (sCode == KEY_FUNC_AMPT_REFUNIT_DBM ||
					sCode == KEY_FUNC_AMPT_REFUNIT_DBMV ||
					sCode == KEY_FUNC_AMPT_REFUNIT_DBUV ||
					sCode == KEY_FUNC_AMPT_REFUNIT_W ||
					sCode == KEY_FUNC_AMPT_REFUNIT_V ||
					sCode == KEY_FUNC_AMPT_REFUNIT_DBPW ||
					sCode == KEY_FUNC_AMPT_REFUNIT_DBuW)
			{
				subKeyDef* tempKey = nowSubKey;
				fCode = 0x0;

				if (tempKey != NULL)
				{
					switch (sysData.ampt.refUnit)
					{
						case rudBm:
							fCode = KEY_FUNC_AMPT_REFUNIT_DBM;
							break;
						case rudBmV:
							fCode = KEY_FUNC_AMPT_REFUNIT_DBMV;
							break;
						case rudBuV:
							fCode = KEY_FUNC_AMPT_REFUNIT_DBUV;
							break;
						case ruW:
							fCode = KEY_FUNC_AMPT_REFUNIT_W;
							break;
						case ruV:
							fCode = KEY_FUNC_AMPT_REFUNIT_V;
							break;
						case rudBpW:
							fCode = KEY_FUNC_AMPT_REFUNIT_DBPW;
							break;
						case rudBuW:
							fCode = KEY_FUNC_AMPT_REFUNIT_DBuW;
							break;
					}

					while (tempKey->upNode != NULL)
					{
						tempKey = tempKey->upNode;
					}

					while (tempKey != NULL)
					{
						if (tempKey->funcCode == fCode)
						{
							nowSubKey = tempKey;
							drawButton();
							break;
						}

						tempKey = tempKey->downNode;
					}
				}
			}

			break;

		case KEY_MAIN_BW:
			if (sCode == KEY_FUNC_BW_RBW ||
					sCode == KEY_FUNC_BW_RBWMODE ||
					sCode == KEY_FUNC_BW_VBW ||
					sCode == KEY_FUNC_BW_AVERAGE)
			{
				drawButton();
			}

			break;

		case KEY_MAIN_SWEEP:
			if (sCode == KEY_FUNC_SWEEP_SWEEPTIME ||
					sCode == KEY_FUNC_SWEEP_SWEEPMODE_SINGLE ||
					sCode == KEY_FUNC_SWEEP_SWEEPMODE_CONTINUOUS ||
					sCode == KEY_FUNC_SWEEP_SWEEPPOINTS)
			{
				drawButton();
			}

			break;

		case KEY_MAIN_TRACE:
			if (sCode == KEY_FUNC_TRACE_TRACE ||
					sCode == KEY_FUNC_TRACE_CLEARWRITE ||
					sCode == KEY_FUNC_TRACE_MAXHOLD ||
					sCode == KEY_FUNC_TRACE_MINHOLD ||
					sCode == KEY_FUNC_TRACE_VIEW ||
					sCode == KEY_FUNC_TRACE_BLANK)
			{
				subKeyDef* tempKey = nowSubKey;
				fCode = 0x0;

				if (tempKey != NULL)
				{
					switch (sysData.trace[sysData.traceSelIndex].state)
					{
						case tsClearWrite:
							fCode = KEY_FUNC_TRACE_CLEARWRITE;
							break;
						case tsMaxHold:
							fCode = KEY_FUNC_TRACE_MAXHOLD;
							break;
						case tsMinHold:
							fCode = KEY_FUNC_TRACE_MINHOLD;
							break;
						case tsView:
							fCode = KEY_FUNC_TRACE_VIEW;
							break;
						case tsBlank:
							fCode = KEY_FUNC_TRACE_BLANK;
							break;
					}

					while (tempKey->upNode != NULL)
					{
						tempKey = tempKey->upNode;
					}

					while (tempKey != NULL)
					{
						if (tempKey->funcCode == fCode)
						{
							nowSubKey = tempKey;
							drawButton();
							break;
						}

						tempKey = tempKey->downNode;
					}
				}
			}

			break;

		case KEY_MAIN_DETECTOR:
			if (sCode == KEY_FUNC_DETECTOR_AUTO ||
					sCode == KEY_FUNC_DETECTOR_NORMAL ||
					sCode == KEY_FUNC_DETECTOR_POSPEAK ||
					sCode == KEY_FUNC_DETECTOR_NEGPEAK ||
					sCode == KEY_FUNC_DETECTOR_SAMPLE)
			{
				subKeyDef* tempKey = nowSubKey;
				fCode = 0x0;

				if (tempKey != NULL)
				{
					switch (sysData.detector.detMode)
					{
						case dmAuto:
							fCode = KEY_FUNC_DETECTOR_AUTO;
							break;
						case dmNormal:
							fCode = KEY_FUNC_DETECTOR_NORMAL;
							break;
						case dmPosPeak:
							fCode = KEY_FUNC_DETECTOR_POSPEAK;
							break;
						case dmNegPeak:
							fCode = KEY_FUNC_DETECTOR_NEGPEAK;
							break;
						case dmSample:
							fCode = KEY_FUNC_DETECTOR_SAMPLE;
							break;
					}

					while (tempKey->upNode != NULL)
					{
						tempKey = tempKey->upNode;
					}

					while (tempKey != NULL)
					{
						if (tempKey->funcCode == fCode)
						{
							nowSubKey = tempKey;
							drawButton();
							break;
						}

						tempKey = tempKey->downNode;
					}
				}
			}

			break;

		case KEY_MAIN_MARKER:
			if (sCode == KEY_FUNC_MARKER_LIST)
			{
				drawButton();
			} else if (sCode == KEY_FUNC_MARKER_MARKER ||
					sCode == KEY_FUNC_MARKER_TRACE ||
					sCode == KEY_FUNC_MARKER_NORMAL ||
					sCode == KEY_FUNC_MARKER_DELTA ||
					sCode == KEY_FUNC_MARKER_OFF ||
					sCode == KEY_FUNC_MARKER_ALLOFF)
			{
				subKeyDef* tempKey = nowSubKey;
				fCode = 0x0;

				if (tempKey != NULL)
				{
					switch (sysData.marker[sysData.markerSelIndex].state)
					{
						case msClose:
							fCode = KEY_FUNC_MARKER_OFF;
							break;
						case msNormal:
							fCode = KEY_FUNC_MARKER_NORMAL;
							break;
						case msDelta:
							fCode = KEY_FUNC_MARKER_DELTA;
							break;
					}

					while (tempKey->upNode != NULL)
					{
						tempKey = tempKey->upNode;
					}

					while (tempKey != NULL)
					{
						if (tempKey->funcCode == fCode)
						{
							nowSubKey = tempKey;
							drawButton();
							break;
						}

						tempKey = tempKey->downNode;
					}
				}
			}

			break;
		default:
			drawButton();
			break;
	}
}

//绘制SCPI状态
void mainForm::refreshScpiState(QString helpStr)
{
	if (!helpStr.isNull() && !helpStr.isEmpty() && helpStr != helpContent)
	{
		helpContent = helpStr;
		drawHelp();
	}
}

//绘制Preset
void mainForm::drawPreset(void)
{
	QRect rc = QRect(rcPreset.left() - 3, rcPreset.top() + TOPHEIGHT + 3, rcPreset.width(), rcPreset.height());

	QPainter p;
	QPixmap pix(rcPreset.size());

	if (!pix.isNull())
	{
		p.begin(&pix);
		p.fillRect(pix.rect(), Qt::black);
		p.setPen(Qt::yellow);
		p.setFont(fontLight);
		p.drawText(pix.rect(), Qt::AlignCenter, "※");
		p.end();

		QScreen::instance()->blit(pix.toImage(), QPoint(rc.left(), rc.top()), rc);
	}
}

//绘制TOP(15ms)
void mainForm::drawTop(void)
{
	if (not sysData.display.isFullScreen)
	{
		//background
		QRect rc = QRect(0, 0, SCREENWIDTH * CANVASWIDTH, TOPHEIGHT);
		QPainter p;
		QPixmap pix(rc.size());

		if (!pix.isNull())
		{
			p.begin(&pix);
			p.fillRect(pix.rect(), BACKGROUNDCOLOR);
			p.setFont(fontData);

			QRect rcPoint;

			/*
			 //空军工程大学定制
			 //logo
			 rcPoint = QRect(0, 0, 220, rc.height());

			 if (sysData.system.isLanguageEn)
			 {
			 p.drawText(rcPoint, Qt::AlignLeft | Qt::AlignVCenter, " WTS-15B");
			 } else
			 {
			 p.drawText(rcPoint, Qt::AlignLeft | Qt::AlignVCenter, " WTS-15B 信号检测组合");
			 }
			 */

			//logo
			if (sysData.system.isLogOn)
			{
				rcPoint = QRect(0, 0, LOGOWIDTH, rc.height());
				p.drawPixmap(rcPoint.left() + BORDERWIDTH, rcPoint.top() + BORDERWIDTH + 2, sysScpi->logo);

				//title
				rcPoint = QRect(rcPoint.left() + rcPoint.width(), rcPoint.top(), SCREENWIDTH * CANVASWIDTH * TITLEWIDTH - rcPoint.width(), rcPoint.height());
			} else
			{
				rcPoint = QRect(0, 0, 0, rc.height());

				//title
				rcPoint = QRect(0, rcPoint.top(), SCREENWIDTH * CANVASWIDTH * TITLEWIDTH - rcPoint.width(), rcPoint.height());
			}

			if (sysData.emc.emcOn)
			{
				if (sysData.system.isLanguageEn)
				{
					p.drawText(rcPoint, Qt::AlignLeft | Qt::AlignVCenter, "EMI Measure");
				} else
				{
					p.drawText(rcPoint, Qt::AlignLeft | Qt::AlignVCenter, "EMI测量");
				}
			} else
			{
				if (sysData.system.isLanguageEn)
				{
					p.drawText(rcPoint, Qt::AlignLeft | Qt::AlignVCenter, "Spectrum Analyzer");
				} else
				{
					p.drawText(rcPoint, Qt::AlignLeft | Qt::AlignVCenter, "频谱分析仪");
				}
			}

			//datetime
			if (sysData.system.datetime.isVisible)
			{
				rcPoint = QRect(rcPoint.left() + rcPoint.width(), rcPoint.top(), TIMEWIDTH, rcPoint.height());
				char datetime[32];

				switch (sysData.system.datetime.style)
				{
					case dtsHMSYMD:
						getLocalDateTimeConvert(datetime);
						break;
					default:
						getLocalDateTime(datetime);
						break;
				}

				p.drawText(rcPoint, Qt::AlignLeft | Qt::AlignVCenter, datetime);
			}

			//scpi
			rcPoint = QRect(SCREENWIDTH * CANVASWIDTH - SCPIBTNSIZE, rcPoint.top(), SCPIBTNSIZE, rcPoint.height());

			if (sysData.scpiData.stateR)
			{
				p.setPen(Qt::blue);
				p.drawText(rcPoint, Qt::AlignCenter, "R");
			} else if (sysData.scpiData.stateL)
			{
				p.setPen(Qt::black);
				p.drawText(rcPoint, Qt::AlignCenter, "L");
			}

			p.end();

			if (sysStart)
			{
				p.begin(imgStart);
				p.drawPixmap(QPoint(rc.left(), rc.top()), pix);
				p.end();
			} else
			{
				QScreen::instance()->blit(pix.toImage(), QPoint(rc.left(), rc.top()), rc);
			}
		}
	}
}

//绘制矢量区(80ms)
void mainForm::drawCanvas(void)
{
	if (sysScpi->isUpdating)
	{
		return;
	}

	QTime t;
	t.start();

	if (sysScpi->eva.isEvaluate && evaDay != QDate::currentDate().day())
	{
		evaDay = QDate::currentDate().day();
		sysScpi->getSystemServiceOfEvaluteSYDays();
	}

	//canvas
	QRect rc;

	if (sysData.display.isFullScreen)
	{
		rc = QRect(0, 0, SCREENWIDTH, SCREENHEIGHT);
	} else
	{
		rc = QRect(0, TOPHEIGHT, SCREENWIDTH * CANVASWIDTH, SCREENHEIGHT - TOPHEIGHT - BOTTOMHEIGHT);
	}
	//no display
	if (sysData.canvasNums < 1)
	{
		QPainter p;
		QPixmap pix(rc.size());

		if (!pix.isNull())
		{
			p.begin(&pix);
			p.fillRect(pix.rect(), BACKGROUNDCOLOR);
			QBrush b(Qt::black);
			QRect rcPoint;

			if (sysData.display.isFullScreen)
			{
				rcPoint = QRect(BORDERWIDTH, BORDERWIDTH, rc.width() - 2 * BORDERWIDTH, rc.height() - 2 * BORDERWIDTH);
			} else
			{
				rcPoint = QRect(BORDERWIDTH, 0, rc.width() - BORDERWIDTH, rc.height());
			}

			qDrawShadePanel(&p, rcPoint, palette(), true, FRAMELINEWIDTH, &b);
			p.end();
			QScreen::instance()->blit(pix.toImage(), QPoint(rc.left(), rc.top()), rc);
		}

		return;
	}

	//normal multiple display
	QRect rcMany[sysData.canvasNums];
	int rcHeight = rc.height() / sysData.canvasNums;

	for (int i = 1; i <= sysData.canvasNums; i++)
	{
		if ((i > 1) && (i == sysData.canvasNums))
		{
			rcMany[i - 1] = QRect(rc.left(), rc.top() + (sysData.canvasNums - 1) * rcHeight, rc.width(), rc.height() - (sysData.canvasNums - 1) * rcHeight);
		} else
		{
			rcMany[i - 1] = QRect(rc.left(), rc.top() + (i - 1) * rcHeight, rc.width(), rcHeight);
		}

		QPainter p;
		QPixmap pix(rcMany[i - 1].size());
		p.begin(&pix);
		if ((sysData.canvasNums == 1) && (sysData.display.isFullScreen))
		{
			p.fillRect(pix.rect(), Qt::black);
		} else
		{
			p.fillRect(pix.rect(), BACKGROUNDCOLOR);
		}
		QRect rcPoint;

		if (sysData.display.isFullScreen)
		{
			if (i == sysData.canvasNums)
			{
				rcPoint = QRect(BORDERWIDTH, BORDERWIDTH, rcMany[i - 1].width() - 2 * BORDERWIDTH, rcMany[i - 1].height() - 2 * BORDERWIDTH);
			} else
			{
				rcPoint = QRect(BORDERWIDTH, BORDERWIDTH, rcMany[i - 1].width() - 2 * BORDERWIDTH, rcMany[i - 1].height() - BORDERWIDTH + BORDERWIDTH - 1);
			}
		} else
		{
			if (i == sysData.canvasNums)
			{
				rcPoint = QRect(BORDERWIDTH, 0, rcMany[i - 1].width() - BORDERWIDTH, rcMany[i - 1].height() + 1);
			} else
			{
				rcPoint = QRect(BORDERWIDTH, 0, rcMany[i - 1].width() - BORDERWIDTH, rcMany[i - 1].height() - BORDERWIDTH + BORDERWIDTH - 1);
			}
		}

		QBrush b(Qt::black);

		if ((!sysData.display.isFullScreen) || ((sysData.display.isFullScreen) && (sysData.canvasNums > 1)))
		{
			qDrawShadePanel(&p, rcPoint, palette(), true, FRAMELINEWIDTH, &b);
		} else
		{
			qDrawShadePanel(&p, rcPoint, palette(), false, 0, &b);
		}

		if (!sysScpi->eva.isEvaluate || (sysScpi->eva.isEvaluate && !sysScpi->eva.evaluated))
		{
			switch (sysData.mode)
			{
				case smFile:
					drawFile(&p, &pix, QRect(rcPoint.left() + FRAMELINEWIDTH, rcPoint.top() + FRAMELINEWIDTH, rcPoint.width() - 2 * FRAMELINEWIDTH, rcPoint.height() - 2 * FRAMELINEWIDTH));
					break;
				case smSystemInfo:
					drawSystemInfo(&p, &pix, QRect(rcPoint.left() + FRAMELINEWIDTH, rcPoint.top() + FRAMELINEWIDTH, rcPoint.width() - 2 * FRAMELINEWIDTH, rcPoint.height() - 2 * FRAMELINEWIDTH));
					break;
				case smSystemLog:
					drawSystemLog(&p, &pix, QRect(rcPoint.left() + FRAMELINEWIDTH, rcPoint.top() + FRAMELINEWIDTH, rcPoint.width() - 2 * FRAMELINEWIDTH, rcPoint.height() - 2 * FRAMELINEWIDTH));
					break;
				case smHelp:
					drawHelp(&p, &pix, QRect(rcPoint.left() + FRAMELINEWIDTH, rcPoint.top() + FRAMELINEWIDTH, rcPoint.width() - 2 * FRAMELINEWIDTH, rcPoint.height() - 2 * FRAMELINEWIDTH));
					break;
				case smMarkerTable:
					if (i == 1)
					{
						drawVector(&p, &pix, QRect(rcPoint.left() + FRAMELINEWIDTH, rcPoint.top() + FRAMELINEWIDTH, rcPoint.width() - 2 * FRAMELINEWIDTH, rcPoint.height() - 2 * FRAMELINEWIDTH));
					} else if (i == 2)
					{
						drawMarkerTable(&p, &pix, QRect(rcPoint.left() + FRAMELINEWIDTH, rcPoint.top() + FRAMELINEWIDTH, rcPoint.width() - 2 * FRAMELINEWIDTH, rcPoint.height() - 2 * FRAMELINEWIDTH));
					}
					break;
					//
				case smZoom:
					if (i == 1)
					{
						if (!sysData.display.zoomed)
						{
							drawVector(&p, &pix, QRect(rcPoint.left() + FRAMELINEWIDTH, rcPoint.top() + FRAMELINEWIDTH, rcPoint.width() - 2 * FRAMELINEWIDTH, rcPoint.height() - 2 * FRAMELINEWIDTH));
							sysData.display.zoomed = true;
							sysScpi->setDisplayOfZoomFrequency(0);
						} else
						{
							drawZoom(&p, &pix, QRect(rcPoint.left() + FRAMELINEWIDTH, rcPoint.top() + FRAMELINEWIDTH, rcPoint.width() - 2 * FRAMELINEWIDTH, rcPoint.height() - 2 * FRAMELINEWIDTH));
						}
					} else if (i == 2)
					{
						drawVector(&p, &pix, QRect(rcPoint.left() + FRAMELINEWIDTH, rcPoint.top() + FRAMELINEWIDTH, rcPoint.width() - 2 * FRAMELINEWIDTH, rcPoint.height() - 2 * FRAMELINEWIDTH));
					}
					break;
				case smTimeSpec:
					if (i == 1)
					{
						drawVector(&p, &pix, QRect(rcPoint.left() + FRAMELINEWIDTH, rcPoint.top() + FRAMELINEWIDTH, rcPoint.width() - 2 * FRAMELINEWIDTH, rcPoint.height() - 2 * FRAMELINEWIDTH));
					} else if (i == 2)
					{
						drawTimeSpec(&p, &pix, QRect(rcPoint.left() + FRAMELINEWIDTH, rcPoint.top() + FRAMELINEWIDTH, rcPoint.width() - 2 * FRAMELINEWIDTH, rcPoint.height() - 2 * FRAMELINEWIDTH));
					}
					break;
				case smAcpr:
					if (i == 1)
					{
						drawVector(&p, &pix, QRect(rcPoint.left() + FRAMELINEWIDTH, rcPoint.top() + FRAMELINEWIDTH, rcPoint.width() - 2 * FRAMELINEWIDTH, rcPoint.height() - 2 * FRAMELINEWIDTH));
					} else if (i == 2)
					{
						drawAcpr(&p, &pix, QRect(rcPoint.left() + FRAMELINEWIDTH, rcPoint.top() + FRAMELINEWIDTH, rcPoint.width() - 2 * FRAMELINEWIDTH, rcPoint.height() - 2 * FRAMELINEWIDTH));
					}
					break;
				case smObw:
					if (i == 1)
					{
						drawVector(&p, &pix, QRect(rcPoint.left() + FRAMELINEWIDTH, rcPoint.top() + FRAMELINEWIDTH, rcPoint.width() - 2 * FRAMELINEWIDTH, rcPoint.height() - 2 * FRAMELINEWIDTH));
					} else if (i == 2)
					{
						drawObw(&p, &pix, QRect(rcPoint.left() + FRAMELINEWIDTH, rcPoint.top() + FRAMELINEWIDTH, rcPoint.width() - 2 * FRAMELINEWIDTH, rcPoint.height() - 2 * FRAMELINEWIDTH));
					}
					break;
				case smCalibrate:
					drawVector(&p, &pix, QRect(rcPoint.left() + FRAMELINEWIDTH, rcPoint.top() + FRAMELINEWIDTH, rcPoint.width() - 2 * FRAMELINEWIDTH, rcPoint.height() - 2 * FRAMELINEWIDTH));
					break;
				case smDiscriminator:
					drawVector(&p, &pix, QRect(rcPoint.left() + FRAMELINEWIDTH, rcPoint.top() + FRAMELINEWIDTH, rcPoint.width() - 2 * FRAMELINEWIDTH, rcPoint.height() - 2 * FRAMELINEWIDTH));
					break;
					break;

				case smAM:
					if (i == 1)
					{
						drawDemodVectorOfAm(&p, &pix, QRect(rcPoint.left() + FRAMELINEWIDTH, rcPoint.top() + FRAMELINEWIDTH, rcPoint.width() - 2 * FRAMELINEWIDTH, rcPoint.height() - 2 * FRAMELINEWIDTH));
					} else if (i == 2)
					{
						drawDemodOfAm(&p, &pix, QRect(rcPoint.left() + FRAMELINEWIDTH, rcPoint.top() + FRAMELINEWIDTH, rcPoint.width() - 2 * FRAMELINEWIDTH, rcPoint.height() - 2 * FRAMELINEWIDTH));
					}
					break;
				case smFM:
					if (i == 1)
					{
						drawDemodVectorOfFm(&p, &pix, QRect(rcPoint.left() + FRAMELINEWIDTH, rcPoint.top() + FRAMELINEWIDTH, rcPoint.width() - 2 * FRAMELINEWIDTH, rcPoint.height() - 2 * FRAMELINEWIDTH));
					} else if (i == 2)
					{
						drawDemodOfFm(&p, &pix, QRect(rcPoint.left() + FRAMELINEWIDTH, rcPoint.top() + FRAMELINEWIDTH, rcPoint.width() - 2 * FRAMELINEWIDTH, rcPoint.height() - 2 * FRAMELINEWIDTH));
					}
					break;
				case smPassFail:
					if (i == 1)
					{
						drawVector(&p, &pix, QRect(rcPoint.left() + FRAMELINEWIDTH, rcPoint.top() + FRAMELINEWIDTH, rcPoint.width() - 2 * FRAMELINEWIDTH, rcPoint.height() - 2 * FRAMELINEWIDTH));
					} else if (i == 2)
					{
						drawPassFailLineEdit(&p, &pix, QRect(rcPoint.left() + FRAMELINEWIDTH, rcPoint.top() + FRAMELINEWIDTH, rcPoint.width() - 2 * FRAMELINEWIDTH, rcPoint.height() - 2 * FRAMELINEWIDTH));
					}
					break;

				default:
					smSpectrum: drawVector(&p, &pix, QRect(rcPoint.left() + FRAMELINEWIDTH, rcPoint.top() + FRAMELINEWIDTH, rcPoint.width() - 2 * FRAMELINEWIDTH, rcPoint.height() - 2 * FRAMELINEWIDTH));
					break;
			}
		} else
		{
			int evaNums = sizeof evaArray / sizeof evaArray[0];
			int evaWidth = 32;
			int evaHeight = 32;
			QRect rc[evaNums];
			QVector<QRect> rcS;
			char tempChar[32] = { 0x0 };

			p.setPen(Qt::white);
			rc[0] = QRect(rcPoint.left() + rcPoint.width() / 2 - 3.5 * evaWidth, rcPoint.top() + rcPoint.height() / 2 - evaHeight / 2, evaWidth, evaHeight);
			rcS << rc[0];
			p.drawText(rc[0], Qt::AlignCenter, QString(intToHex(evaArray[0], tempChar)).trimmed());

			for (int i = 1; i < evaNums; i++)
			{
				rc[i] = QRect(rc[i - 1].left() + rc[i - 1].width(), rc[i - 1].top(), rc[i - 1].width(), rc[i - 1].height());
				rcS << rc[i];

				p.drawText(rc[i], Qt::AlignCenter, QString(intToHex(evaArray[i], tempChar)).trimmed());
			}

			p.setPen(Qt::white);
			p.drawRects(rcS);

			p.drawText(QRect(rc[0].left(), rc[0].top() - evaHeight, rc[0].width() * (evaNums + 2), evaHeight), Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("Please input RegCode") + "(" + QString(sysData.system.serialNo).trimmed() + ")");

			for (int i = 0; i < evaNums; i++)
			{
				if (i == evaIndex)
				{
					p.setPen(Qt::green);
					p.drawRect(rc[i]);
					break;
				}
			}
		}

		p.end();

		if (img_Paused)
		{
			if (i == 1)
			{
				if (img_Paused_Zoom)
				{
					img_Zoom = pix;
				} else if (img_Paused_Zoom_Full)
				{
					img_Zoom_Full = pix;
				}
			}
		} else
		{
			if (sysStart)
			{
				p.begin(imgStart);
				p.drawPixmap(QPoint(rcMany[i - 1].left(), rcMany[i - 1].top()), pix);
				p.end();
			} else
			{
				QScreen::instance()->blit(pix.toImage(), QPoint(rcMany[i - 1].left(), rcMany[i - 1].top()), rcMany[i - 1]);
			}
		}
	}

	int elapsedTime = t.elapsed();

	if (elapsedTime < 0 || elapsedTime > 2000)
	{
		lcdRefreshTimes = 200;
	} else
	{
		lcdRefreshTimes = elapsedTime;
	}

}

//绘制帮助区(10ms)
void mainForm::drawHelp(void)
{
	if (sysScpi->isUpdating)
	{
		return;
	}

	if (!sysData.display.isFullScreen)
	{
		QRect rc = QRect(0, SCREENHEIGHT - BOTTOMHEIGHT, SCREENWIDTH * CANVASWIDTH, BOTTOMHEIGHT);
		QRect rcPoint = QRect(BORDERWIDTH, BORDERWIDTH, rc.width() - 1 * BORDERWIDTH, rc.height() - 2 * BORDERWIDTH);
		QPainter p;
		QPixmap pix(rc.size());

		if (!pix.isNull())
		{
			p.begin(&pix);
			p.fillRect(pix.rect(), BACKGROUNDCOLOR);
			p.setFont(fontData);
			QBrush b(Qt::black);
			qDrawShadePanel(&p, rcPoint, palette(), true, FRAMELINEWIDTH, &b);
			p.setPen(Qt::darkYellow);
			rcPoint = QRect(rcPoint.left() + 5, rcPoint.top(), rcPoint.width() - 10, rcPoint.height());
			p.drawText(rcPoint, Qt::AlignLeft | Qt::AlignVCenter, helpContent);
			p.end();

			if (sysStart)
			{
				p.begin(imgStart);
				p.drawPixmap(QPoint(rc.left(), rc.top()), pix);
				p.end();
			} else
			{
				QScreen::instance()->blit(pix.toImage(), QPoint(rc.left(), rc.top()), rc);
			}
		}
	}
}

//绘制按键输入区
void mainForm::drawInput(void)
{
	if (sysScpi->isUpdating || sysData.mode != smSpectrum)
	{
		return;
	}

	QRect rc = QRect(0, TOPHEIGHT, SCREENWIDTH * CANVASWIDTH, KBDINPUTHEIGHT);
	QRect rcPoint = QRect(BORDERWIDTH, BORDERWIDTH, rc.width() - BORDERWIDTH, rc.height());
	QBrush b(Qt::black);
	QPainter painter;
	QPixmap pix(SCREENWIDTH * CANVASWIDTH, KBDINPUTHEIGHT);
	painter.begin(&pix);
	painter.fillRect(pix.rect(), BACKGROUNDCOLOR);
	qDrawShadePanel(&painter, rcPoint, palette(), true, FRAMELINEWIDTH, &b);
	if (!inputTag.isNull() && !inputTag.isEmpty())
	{
		int TextSep = 10;
		QString Text = inputTag;
		if (!inputValue.isNull() && !inputValue.isEmpty())
		{
			Text += "：  ";
			Text += inputValue;
			Text += "_";
		} else if (!tagValue.isNull() && !tagValue.isEmpty())
		{
			Text += "： ";
			Text += tagValue;
		}

		QRect rcText = QRect(rcPoint.left() + TextSep, rcPoint.top(), rcPoint.width() - TextSep * 2, rcPoint.height());
		painter.setFont(fontDataBig);
		painter.setPen(Qt::white);
		painter.drawText(rcText, Qt::AlignLeft | Qt::AlignVCenter, Text);
		if (!inputValue.isNull() && !inputValue.isEmpty())
			painter.drawText(rcText, Qt::AlignRight | Qt::AlignVCenter, "*");
	}

	painter.end();
	QScreen::instance()->blit(pix.toImage(), QPoint(rc.left(), rc.top()), rc);
}

//绘制按钮区(70ms)
void mainForm::drawButton(void)
{
	if (sysScpi->isUpdating)
	{
		return;
	}

	if (!sysData.display.isFullScreen)
	{
		QRect rc = QRect(SCREENWIDTH * CANVASWIDTH, 0, SCREENWIDTH - SCREENWIDTH * CANVASWIDTH, SCREENHEIGHT - HELPHEIGHT);
		QRect rcPoint;
		int btnIndex = 0;
		int btnNums = 0;

		QPainter p;
		QPixmap pix(rc.size());
		p.begin(&pix);
		p.fillRect(pix.rect(), BACKGROUNDCOLOR);
		p.setFont(fontData);

		//button is invalid or error
		if (((nowMainKey == NULL) && (nowSubKey == NULL)) || ((nowMainKey == NULL) && (nowSubKey != NULL)))
		{
			for (int i = 0; i < BUTTONCOUNT; i++)
			{
				rcPoint = QRect(BORDERWIDTH, rc.height() - (BUTTONHEIGHT + BUTTONINTERVAL) * (i + 1), rc.width() - 2 * BORDERWIDTH, BUTTONHEIGHT);

				if (sysData.display.winStyle == dsWinXP)
				{
					qDrawPlainRect(&p, rcPoint, GRIDCOLOR, 2, 0);
				} else
				{
					qDrawWinPanel(&p, rcPoint, palette(), false, 0);
				}
			}
		} else //button valid
		{
			subKeyDef* keyTemp = NULL;

			if (nowSubKey == NULL) //preset or poweron
			{
				keyTemp = nowMainKey->rightNode;
			} else //normal run
			{
				keyTemp = nowSubKey;

				//goto top node
				while (keyTemp->upNode != NULL)
				{
					keyTemp = keyTemp->upNode;

					if (keyTemp->upNode == NULL)
					{
						break;
					}
				}
			}

			QPixmap pix(rc.width() - 2 * BORDERWIDTH - 4, BUTTONHEIGHT - 4);

			//node loop
			while (keyTemp != NULL)
			{
				btnIndex = sysMenu->getButtonIndex(keyTemp->keyValue);
				btnNums++;

				//button count limit
				if (btnNums > BUTTONCOUNT)
				{
					break;
				}

				//button index limit
				if ((btnIndex >= 0) && (btnIndex < BUTTONCOUNT))
				{
					rcPoint = QRect(BORDERWIDTH, rc.height() - (BUTTONHEIGHT + BUTTONINTERVAL) * (BUTTONCOUNT - btnIndex), rc.width() - 2 * BORDERWIDTH, BUTTONHEIGHT);
					drawButtonMenu(&pix, keyTemp);

					if (sysData.display.winStyle == dsWinXP)
					{
						qDrawPlainRect(&p, rcPoint, GRIDCOLOR, 1, 0);
					} else
					{
						qDrawWinPanel(&p, rcPoint, palette(), false, 0);
					}

					p.drawPixmap(rcPoint.left() + 2, rcPoint.top() + 2, pix);
				}

				//next node
				if (keyTemp->downNode == NULL)
				{
					break;
				} else
				{
					keyTemp = keyTemp->downNode;
				}
			}
		}

		//menu name
		rcPoint = QRect(BORDERWIDTH + TABLESEP, 0, rc.width() - 2 * BORDERWIDTH - 2 * TABLESEP, TITLEHEIGHT - 2);
		p.setFont(fontDataBig);
		p.drawText(rcPoint, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString(getMainKeyName()));
		p.end();

		if (sysStart)
		{
			p.begin(imgStart);
			p.drawPixmap(QPoint(rc.left(), rc.top()), pix);
			p.end();
		} else
		{
			QScreen::instance()->blit(pix.toImage(), QPoint(rc.left(), rc.top()), rc);
		}
	}
}

//绘制状态区
void mainForm::drawState(void)
{
	if (sysScpi->isUpdating)
	{
		return;
	}

	if (!sysData.display.isFullScreen)
	{
		QRect rc = QRect(SCREENWIDTH * CANVASWIDTH, SCREENHEIGHT - BOTTOMHEIGHT, SCREENWIDTH - SCREENWIDTH * CANVASWIDTH, BOTTOMHEIGHT);
		QRect rcPoint = QRect(BORDERWIDTH, BORDERWIDTH, rc.width() - 2 * BORDERWIDTH, rc.height() - 2 * BORDERWIDTH);
		QPainter p;
		QPixmap pix(rc.size());
		p.begin(&pix);
		p.fillRect(pix.rect(), BACKGROUNDCOLOR);
		p.setFont(fontData);
		QBrush b(Qt::black);
		qDrawShadePanel(&p, rcPoint, palette(), true, FRAMELINEWIDTH, &b);

		int pixWidth = 24;
		int pixTBSep = 3;
		int pixLRSep = 7;

		QRect rcState;

		pixWidth = (rcPoint.width() - 5 * pixLRSep) / 4;

		//network
		rcState = QRect(rcPoint.right() - pixWidth - pixLRSep + 1, rcPoint.top() + pixTBSep, pixWidth, rcPoint.height() - 2 * pixTBSep);

		if (isNetworkConnect())
		{
			p.drawPixmap(rcState, img_NetOn);
		} else
		{
			p.drawPixmap(rcState, img_NetOff);
		}

		//usb
		rcState = QRect(rcState.left() - pixWidth - pixLRSep, rcState.top(), rcState.width(), rcState.height());

		if (isUsbDeviceConnect())
		{
			p.drawPixmap(rcState, img_UsbOn);
		} else
		{
			p.drawPixmap(rcState, img_UsbOff);
		}

		//gpib
		rcState = QRect(rcState.left() - pixWidth - pixLRSep, rcState.top(), rcState.width(), rcState.height());

		if (sysData.scpiData.interface == sitGpib)
		{
			p.drawPixmap(rcState, img_GpibOn);
		} else
		{
			p.drawPixmap(rcState, img_GpibOff);
		}

		//temperature
		rcState = QRect(rcState.left() - pixWidth - pixLRSep, rcState.top(), rcState.width() - 4, rcState.height());
		rcState = QRect(rcState.left(), rcState.top() + 3, rcState.width(), rcState.height() - 6);
		QRectF rcWdText = QRectF(rcState.left(), rcState.top(), rcState.width(), rcState.height() * (1 - sysData.temperature / MAXTEMPERATURE));
		QRectF rcWdValue = QRectF(rcState.left(), rcWdText.top() + rcWdText.height(), rcWdText.width(), rcState.height() - rcWdText.height());
		p.fillRect(rcWdText, BACKGROUNDCOLOR);
		p.fillRect(rcWdValue, QColor(50, 140, 100));
		rcWdText = QRect(rcState.left(), rcState.top(), rcState.width(), rcState.height() / 2);
		p.setFont(fontSmall);
		p.setPen(Qt::black);
		char tempChar[32] = { };
		p.drawText(rcState, Qt::AlignCenter, QString(intToString((int) sysData.temperature, tempChar)).trimmed());
		p.end();

		if (sysStart)
		{
			p.begin(imgStart);
			p.drawPixmap(QPoint(rc.left(), rc.top()), pix);
			p.end();
		} else
		{
			QScreen::instance()->blit(pix.toImage(), QPoint(rc.left(), rc.top()), rc);
		}
	}
}

//绘制全部
void mainForm::drawAll(void)
{
	drawTop();
	drawButton();
	drawHelp();
	drawState();
	drawCanvas();
}

//载入键盘驱动
void mainForm::loadSystemDriver(void)
{
	//加载键盘驱动
	keyHandle = -2;
	keyFlag = false;
	keyHandle = open(KBD_PORT, O_RDONLY | O_NONBLOCK);//NON| O_NOCTTY

	struct termios tio;
	tio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
	tio.c_iflag = IGNPAR | ICRNL;
	tio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	tio.c_oflag &= ~OPOST;
	tio.c_cc[VMIN] = 0;
	tio.c_cc[VTIME] = 0;

	tcflush(keyHandle, TCIFLUSH);
	tcsetattr(keyHandle, TCSANOW, &tio);

	if (keyHandle >= 0)
	{
		keyNotifier = new QSocketNotifier(keyHandle, QSocketNotifier::Read, this);
		connect(keyNotifier, SIGNAL(activated(int)), this, SLOT(readKeyBoardData()));
		readKeyBoardData();

		keyFlag = true;

		if (__DEBUG)
		{
			printf("%s\n", "load keyBoard driver successed");
		}
	} else
	{
		if (__DEBUG)
		{
			printf("%s\n", "load keyBoard driver failed");
		}
	}

	//加载射频驱动
	rfHandle = -1;
	rfHandle = open(DEVICE_NAME_RF, O_RDWR | O_NONBLOCK);

	if (rfHandle >= 0)
	{
		if (__DEBUG)
		{
			printf("%s\n", "load rf driver successed");
		}
	} else
	{
		if (__DEBUG)
		{
			printf("%s\n", "load rf driver failed");
		}
	}

//	//加载中频驱动
//	ifHandle = -1;
//	ifHandle = open(DEVICE_NAME_IF, O_RDWR | O_NONBLOCK);
//
//	if (ifHandle >= 0)
//	{
//		if (__DEBUG)
//		{
//			printf("%s\n", "load if driver successed");
//		}
//	} else
//	{
//		if (__DEBUG)
//		{
//			printf("%s\n", "load if driver failed");
//		}
//	}

	//加载射频前端驱动
	feHandle = -1;
	feHandle = open(DEVICE_NAME_FE, O_RDWR | O_NONBLOCK);

	if (feHandle >= 0)
	{
		if (__DEBUG)
		{
			printf("%s\n", "load fe driver successed");
		}
//		//监听系统文件操作，将操作转换为QT事件进入系统的消息队列，并调用预先设置的事件接受函数，处理事件。
//		/*1、启动信号驱动机制
//		 signal(SIGIO, input_handler);
//		 将SIGIO信号同input_handler函数关联起来， 一旦产生SIGIO信号,就会执行input_handler，
//		 */
//		signal(SIGIO, sigio);
//
//		/* 2、
//		 fcntl(STDIN_FILENO, F_SETOWN, getpid());
//		 STDIN_FILENO是打开的设备文件描述符,
//		 F_SETOWN用来决定操作是干什么的,
//		 getpid()是个系统调用，功能是返回当前进程的进程号
//		 整个函数的功能是STDIN_FILENO设置这个设备文件的拥有者为当前进程。
//		 */
//		fcntl(feHandle, F_SETOWN, ::getpid());
//
//		/*3、
//		 得到打开文件描述符的状态
//		 oflags = fcntl(STDIN_FILENO, F_GETFL);
//		 */
//		int flag = fcntl(feHandle, F_GETFL);
//
//		/*4、
//		 fcntl(STDIN_FILENO, F_SETFL, oflags | FASYNC);
//		 设置文件描述符的状态为oflags | FASYNC属性,
//		 一旦文件描述符被设置成具有FASYNC属性的状态，也就是将设备文件切换到异步操作模式。
//		 这时系统就会自动调用驱动程序的fasync方法。
//		 */
//		fcntl(feHandle, F_SETFL, flag | FASYNC); //异步
//		//fcntl(feHandle,F_SETFL,flag);   //同步

	} else
	{
		if (__DEBUG)
		{
			printf("%s\n", "load fe driver failed");
		}
	}

//	//加载IF驱动
//	ifHandleTQ = -1;
//	ifHandleTQ = open(DEVICE_NAME_IF, O_RDWR | O_NONBLOCK);
//
//	if (ifHandleTQ >= 0)
//	{
//		if (__DEBUG)
//		{
//			printf("%s\n", "load if driver successed");
//		}
//	} else
//	{
//		if (__DEBUG)
//		{
//			printf("%s\n", "load if driver failed");
//		}
//	}

	//加载ADF4360驱动
	ramADF4360Handle = -1;
	ramADF4360Handle = open(DEVICE_NAME_ADF4360, O_RDWR | O_NONBLOCK);

	if (ramADF4360Handle >= 0)
	{
		if (__DEBUG)
		{
			printf("load ram ADF4360 driver successed\n");
		}
	} else
	{
		if (__DEBUG)
		{
			printf("load ram ADF4360 driver failed\n");
		}
	}

	//加载温度驱动
	tmptHandle = -1;
	tmptHandle = open(DEVICE_NAME_WD, O_RDWR | O_NONBLOCK);

	if (tmptHandle >= 0)
	{
		if (__DEBUG)
		{
			printf("%s\n", "load temperature driver successed");
		}
		//ioctl(tmptHandle, 0xf2, 0);//温度传感器初始化
	} else
	{
		if (__DEBUG)
		{
			printf("%s\n", "load temperature driver failed");
		}
	}

	//加载IQ句柄
	iqHandle = -1;
	iqHandle = open(DEVICE_NAME_IQ, O_RDWR | O_NONBLOCK);

	if (iqHandle >= 0)
	{
		if (__DEBUG)
		{
			printf("load iq driver successed\n");
		}
	} else
	{
		if (__DEBUG)
		{
			printf("load iq driver failed\n");
		}
	}

	//加固件版本驱动
	firVerHandle = -1;
	firVerHandle = open(DEVICE_NAME_VERSION, O_RDWR);

	FIRMWARE_VER_MAJOR = 0;
	FIRMWARE_VER_MINOR = 0;
	if (firVerHandle >= 0)
	{
		if (__DEBUG)
		{
			printf("%s\n", "load version driver successed");
		}
		int ret;
		unsigned char rbuf[2];
		ret = read(firVerHandle, rbuf, 2);
		FIRMWARE_VER_MAJOR = rbuf[0];
		FIRMWARE_VER_MINOR = rbuf[1];
		if (ret < 0)
		{
			printf("read version error\n");
		}
	} else
	{
		if (__DEBUG)
		{
			printf("%s\n", "load version driver failed");
		}
	}
/*
	//加载FFT驱动
	fftHandle = -1;
	fftHandle = open(DEVICE_NAME_FFT, O_RDWR | O_NONBLOCK);

	if (fftHandle >= 0)
	{
		if (__DEBUG)
		{
			printf("load fft driver successed\n");
		}
	} else
	{
		if (__DEBUG)
		{
			printf("load fft driver failed\n");
		}
	}
*/
	//加载Demod驱动
/*	demodHandle = -1;
	demodHandle = open(DEVICE_NAME_DEMOD, O_RDWR | O_NONBLOCK);

	if (demodHandle >= 0)
	{
		if (__DEBUG)
		{
			printf("load demod driver successed\n");
		}
	} else
	{
		if (__DEBUG)
		{
			printf("load demod driver failed\n");
		}
	}
	*/

	ramHandle = -1;
	ramHandle = open(DEVICE_NAME_RAM, O_RDWR | O_NONBLOCK);

	if (ramHandle >= 0)
	{
	    //ramNotifier = new QSocketNotifier(ramHandle, QSocketNotifier::Read, this);
        //connect(ramNotifier, SIGNAL(activated(int)), this, SLOT(getDataFromIF()));
		if (__DEBUG)
		{
			printf("%s\n", "load ram driver successed");
		}
#if 1
		//监听系统文件操作，将操作转换为QT事件进入系统的消息队列，并调用预先设置的事件接受函数，处理事件。
		/*change signal handle function */
		signal(SIGIO, sigio);

		/* set file(dev) owner to progress */
		fcntl(ramHandle, F_SETOWN, ::getpid());

		/*get file flags */
		int flag = fcntl(ramHandle, F_GETFL);

		/* add FASYNC to file flag */
		fcntl(ramHandle, F_SETFL, flag | FASYNC); //异步
		//fcntl(rmHandle,F_SETFL,flag);   //同步
#endif
	} else
	{
		if (__DEBUG)
		{
			printf("%s\n", "load ram driver failed");
		}
	}
}

void mainForm::getDataFromIF(void)
{
	sysScpi->getDataFromIF();
}


//输出信息
void mainForm::outputMessage(QString msg)
{
	helpContent = msg;
	drawHelp();
}

//输出信息
void mainForm::outputMessage(long int code)
{
	for (int i = 0; i < sizeof errMessage / sizeof errMessage[0]; i++)
	{
		if (code == errMessage[i].code)
		{
			if (sysData.system.isLanguageEn)
			{
				helpContent = errMessage[i].msgEn;
			} else
			{
				helpContent = errMessage[i].msgCh;
			}

			drawHelp();

			return;
		}
	}

	if (sysData.system.isLanguageEn)
	{
		helpContent = unknownMessage[0].msgEn;
	} else
	{
		helpContent = unknownMessage[0].msgCh;
	}

	drawHelp();
}

//获取错误信息
QString mainForm::getErrorMessage(long int code)
{
	for (int i = 0; i < sizeof errMessage / sizeof errMessage[0]; i++)
	{
		if (code == errMessage[i].code)
		{
			if (sysData.system.isLanguageEn)
			{
				return (QString) errMessage[i].msgEn;
			} else
			{
				return (QString) errMessage[i].msgCh;
			}
		}
	}

	if (sysData.system.isLanguageEn)
	{
		return (QString) unknownMessage[0].msgEn;
	} else
	{
		return (QString) unknownMessage[0].msgCh;
	}
}

//获取帮助内容
QString mainForm::getHelpMessage(int keyValue)
{
	QString result = "";

	mainKeyDef* tpMainKey = sysKey;
	subKeyDef* tpSubKey = nowSubKey;
	int index = 0;

	if (sysMenu != NULL)
	{
		if (sysMenu->pressedIsMainKey(keyValue))
		{
			while (tpMainKey != NULL)
			{
				if (tpMainKey->keyValue == keyValue)
				{
					if (sysData.system.isLanguageEn)
					{
						result = QString(tpMainKey->enHelp);
					} else
					{
						result = QString(tpMainKey->chHelp);
					}

					break;
				}

				tpMainKey = tpMainKey->downNode;
			}
		} else if (sysMenu->pressedIsCommonKey(keyValue))
		{
			if (tpSubKey == NULL && nowMainKey != NULL)
			{
				tpSubKey = nowMainKey->rightNode;
			}

			if (tpSubKey != NULL)
			{
				while (tpSubKey->upNode != NULL)
				{
					tpSubKey = tpSubKey->upNode;
				}

				while (tpSubKey != NULL)
				{
					if (tpSubKey->keyValue == keyValue)
					{
						if (sysData.system.isLanguageEn)
						{
							result = QString(tpSubKey->enHelp);
						} else
						{
							result = QString(tpSubKey->chHelp);
						}

						break;
					}

					tpSubKey = tpSubKey->downNode;
				}
			}
		} else if (sysMenu->pressedIsRpgKey(keyValue))
		{
			if (sysData.system.isLanguageEn)
			{
				result = QString(rpgTrans[index].enName);
			} else
			{
				result = QString(rpgTrans[index].chName);
			}
		} else if (sysMenu->pressedIsArrowKey(keyValue))
		{
			index = -1;

			switch (keyValue)
			{
				case KEY_ARROW_UP:
					index = 0;
					break;
				case KEY_ARROW_DOWN:
					index = 1;
					break;
			}

			if (index >= 0 && index < sizeof arrowTrans / sizeof arrowTrans[0])
			{
				if (sysData.system.isLanguageEn)
				{
					result = QString(arrowTrans[index].enName);
				} else
				{
					result = QString(arrowTrans[index].chName);
				}
			}
		} else if (sysMenu->pressedIsEscKey(keyValue))
		{
			if (sysData.system.isLanguageEn)
			{
				result = QString(cancelTrans[index].enName);
			} else
			{
				result = QString(cancelTrans[index].chName);
			}
		} else if (sysMenu->pressedIsBkspaceKey(keyValue))
		{
			if (sysData.system.isLanguageEn)
			{
				result = QString(bkspaceTrans[index].enName);
			} else
			{
				result = QString(bkspaceTrans[index].chName);
			}
		} else if (sysMenu->pressedIsEnterKey(keyValue))
		{
			if (sysData.system.isLanguageEn)
			{
				result = QString(enterTrans[index].enName);
			} else
			{
				result = QString(enterTrans[index].chName);
			}
		} else if (sysMenu->pressedIsNumberKey(keyValue))
		{
			index = -1;

			switch (keyValue)
			{
				case KEY_NUM_0:
					index = 0;
					break;
				case KEY_NUM_1:
					index = 1;
					break;
				case KEY_NUM_2:
					index = 2;
					break;
				case KEY_NUM_3:
					index = 3;
					break;
				case KEY_NUM_4:
					index = 4;
					break;
				case KEY_NUM_5:
					index = 5;
					break;
				case KEY_NUM_6:
					index = 6;
					break;
				case KEY_NUM_7:
					index = 7;
					break;
				case KEY_NUM_8:
					index = 8;
					break;
				case KEY_NUM_9:
					index = 9;
					break;
				case KEY_NUM_DOT:
					index = 10;
					break;
				case KEY_NUM_NEG:
					index = 11;
					break;
			}

			if (index >= 0 && index < sizeof numberTrans / sizeof numberTrans[0])
			{
				if (sysData.system.isLanguageEn)
				{
					result = QString(numberTrans[index].enName);
				} else
				{
					result = QString(numberTrans[index].chName);
				}
			}
		} else if (sysMenu->pressedIsUnitKey(keyValue))
		{
			index = -1;

			switch (keyValue)
			{
				case KEY_UNIT_GHZ:
					index = 0;
					break;
				case KEY_UNIT_MHZ:
					index = 1;
					break;
				case KEY_UNIT_KHZ:
					index = 2;
					break;
				case KEY_UNIT_HZ:
					index = 3;
					break;
			}

			if (index >= 0 && index < sizeof unitTrans / sizeof unitTrans[0])
			{
				if (sysData.system.isLanguageEn)
				{
					result = QString(unitTrans[index].enName);
				} else
				{
					result = QString(unitTrans[index].chName);
				}
			}
		}
	}

	return result;
}

//USB设备连接
bool mainForm::isUsbDeviceConnect(void)
{
	QString dirName = "/sys/bus/usb/devices/1-1:1.0";

	return QDir(dirName).exists();
}

//网络连接
bool mainForm::isNetworkConnect(void)
{
	QString fileName = "/sys/class/net/eth0/carrier";

	if (QFile(fileName).exists())
	{
		QFile file(fileName);
		QString value = "";

		if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			return false;
		}

		QTextStream out(&file);
		value = out.readAll();
		out.flush();
		file.close();

		return (!value.isNull() && !value.isEmpty() && value.trimmed() == "1");
	}
}

//显示启动画面
void mainForm::showSplashScreen(void)
{
	sysStart = true;
	traceState tcState[TRACECOUNT];

	for (int i = 0; i < TRACECOUNT; i++)
	{
		tcState[i] = sysData.trace[i].state;
		sysData.trace[i].state = tsBlank;
	}

	imgStart = new QPixmap(SCREENWIDTH, SCREENHEIGHT);
	drawAll();

	for (int i = 0; i < TRACECOUNT; i++)
	{
		sysData.trace[i].state = tcState[i];
	}

	QScreen::instance()->blit(imgStart->toImage(), QPoint(0, 0), QRect(0, 0, SCREENWIDTH, SCREENHEIGHT));
	sysStart = false;
}

//读取系统配置
void mainForm::loadSystemSetting(void)
{
	QString fileName = QCoreApplication::applicationDirPath() + "/setting.ini";

	//default factory value
	strcpy(serialNo, SERIALNO);
	sysData.system.isLanguageEn = true;

	QSettings setting(fileName, QSettings::IniFormat);
	QVariant value;

	if (QFile(fileName).exists())
	{
		setting.beginGroup("device");
		strcpy(serialNo, setting.value("serialNo", serialNo).toString().toStdString().c_str());
		setting.endGroup();

		setting.beginGroup("version");
		if (setting.value("major", VER_MAJOR) != VER_MAJOR)
			setting.setValue("major", VER_MAJOR);
		if (setting.value("minor", VER_MAJOR) != VER_MINOR)
			setting.setValue("minor", VER_MINOR);
		if (setting.value("revision", VER_REVISION) != VER_REVISION)
			setting.setValue("revision", VER_REVISION);
		if (setting.value("build", VER_BUILD) != VER_BUILD)
			setting.setValue("build", VER_BUILD);
		if (setting.value("tag", VER_TAG) != VER_TAG)
			setting.setValue("tag", VER_TAG);
		setting.endGroup();

		setting.beginGroup("system");
		sysData.system.isLanguageEn = setting.value("language", "") == "english";
		setting.endGroup();
	} else
	{
		//serial no
		setting.beginGroup("device");
		setting.setValue("serialNo", serialNo);
		setting.endGroup();

		//write version
		setting.beginGroup("version");
		setting.setValue("major", VER_MAJOR);
		setting.setValue("minor", VER_MINOR);
		setting.setValue("revision", VER_REVISION);
		setting.setValue("build", VER_BUILD);
		setting.setValue("tag", VER_TAG);
		setting.endGroup();

		//language
		setting.beginGroup("system");
		setting.setValue("language", sysData.system.isLanguageEn);
		setting.endGroup();
	}
}

//保存系统配置
void mainForm::saveSystemSetting(QString section, QString name, QVariant value)
{
	QString fileName = QCoreApplication::applicationDirPath() + "/setting.ini";
	QSettings setting(fileName, QSettings::IniFormat);
	setting.beginGroup(section);
	setting.setValue(name, value);
	setting.endGroup();
}

//获取字符串
QString mainForm::getTranslatorString(QString enStr, QString chStr)
{
	if (sysData.system.isLanguageEn)
	{
		return enStr;
	} else
	{
		return chStr;
	}
}

//获取字符串
QString mainForm::getTheTranslatorString(QString enStr)
{
	if (enStr.isNull() || enStr.isEmpty())
	{
		return "";
	}

	if (sysData.system.isLanguageEn)
	{
		return enStr;
	}

	QString enStr1 = enStr.toUpper().trimmed();
	QString enStr2 = "";

	for (int i = 0; i < sizeof sysTrans / sizeof sysTrans[0]; i++)
	{
		enStr2 = sysTrans[i].enName;
		enStr2 = enStr2.toUpper().trimmed();

		if (enStr1 == enStr2)
		{
			return sysTrans[i].chName;
			break;
		}
	}

	return enStr;
}

//获取LOGO图片
void mainForm::getLogoPicture(void)
{
	logo.loadFromData(logoBits, sizeof logoBits / sizeof logoBits[0]);
	img_NetOn.loadFromData(netOnBits, sizeof netOnBits / sizeof netOnBits[0]);
	img_NetOff.loadFromData(netOffBits, sizeof netOffBits / sizeof netOffBits[0]);
	img_GpibOn.loadFromData(gpibOnBits, sizeof gpibOnBits / sizeof gpibOnBits[0]);
	img_GpibOff.loadFromData(gpibOffBits, sizeof gpibOffBits / sizeof gpibOffBits[0]);
	img_UsbOn.loadFromData(usbOnBits, sizeof usbOnBits / sizeof usbOnBits[0]);
	img_UsbOff.loadFromData(usbOffBits, sizeof usbOffBits / sizeof usbOffBits[0]);
	img_Arrow.loadFromData(arrowBits, sizeof arrowBits / sizeof arrowBits[0]);
	img_ArrowHui.loadFromData(arrowHuiBits, sizeof arrowHuiBits / sizeof arrowHuiBits[0]);
}

//读键盘数据
void mainForm::readKeyBoardData()
{
	int keyID = -1;
	uint8_t keyValue = 0;
	uint8_t keyClass = 0;
	uint8_t keyCount = 0;
	uint8_t devType = 0;
	uint8_t rpgPluse = 0;
	uint8_t recvCount = 0;
	uint8_t buf[8] = { 0 };

	bool isRpgRotate = false;
	unsigned long colorValue = 0;
	unsigned char usbTempChar[64];
	int tempState = 0;
	int usbRcvLen = 0;

	if (keyFlag == false)
	{
		return;
	}

	//读取键码
	keyNotifier->setEnabled(false);
	recvCount = read(keyHandle, buf, sizeof buf);
	keyNotifier->setEnabled(true);

	tcflush(keyHandle, TCIFLUSH);
	if (recvCount != 8)
	{
		return;
	}

	if (recvCount == 8 && buf[0] == 0xAA && buf[7] == 0xBB)
	{
		keyValue = buf[3];
		keyClass = buf[1];
		keyCount = buf[5];
	} else
	{
		return;
	}

	//打印键值
	if (__KEYVALDEBUG)
	{
		printf("key pressed value is keyValue =0x%04x,0x%04x,0x%04x\n", keyValue, keyClass, keyCount);
	}

	if (sysLocked)
	{
		return;
	}

	if (sysScpi->isUpdating)
	{
		return;
	}

	if (keyValue == KEY_ARROW_DOWN)
	{
		iqSave = true;
	}

	//evaluate
	if (sysScpi->eva.isEvaluate && sysScpi->eva.evaluated)
	{
		int nums = 0;
		int tempValue = evaArray[evaIndex];
		int tempIndex = evaIndex;
		bool valued = false;
		bool indexed = false;
		char tempChar[32] = { 0x0 };
		QString inputString = "";
		QString md5String = "";

		if (keyValue == KEY_ARROW_UP || keyValue == KEY_NUM_8)
		{
			tempValue++;
			valued = true;
		} else if (keyValue == KEY_ARROW_DOWN || keyValue == KEY_NUM_2)
		{
			tempValue--;
			valued = true;
		} else if (keyValue == KEY_NUM_4)
		{
			tempIndex--;
			indexed = true;
		} else if (keyValue == KEY_NUM_6)
		{
			tempIndex++;
			indexed = true;
		} else if (sysMenu->pressedIsRpgKey(keyClass))
		{
			nums = sysMenu->getRpgPulse(keyValue, keyCount);

			if (nums >= 0)
			{
				tempIndex += 1;
			} else
			{
				tempIndex -= 1;
			}

			indexed = true;
		} else if (keyValue == KEY_NUM_ENTER)
		{
			for (int i = 0; i < sizeof evaArray / sizeof evaArray[0]; i++)
			{
				inputString += QString(intToHex(evaArray[i], tempChar)).trimmed();
			}

			md5String = sysScpi->getSystemServiceOfMD5(sysData.system.serialNo, false);

			if (inputString.toUpper() == md5String.toUpper())
			{
				sysScpi->unLockSystem();
			}

			return;
		}

		if (indexed)
		{
			if (tempIndex <= 0)
			{
				tempIndex = 0;
			} else if (tempIndex >= sizeof evaArray / sizeof evaArray[0])
			{
				tempIndex = sizeof evaArray / sizeof evaArray[0] - 1;
			}

			evaIndex = tempIndex;
		}

		if (valued)
		{
			if (tempValue < 0)
			{
				tempValue = 0xf;
			} else if (tempValue > 0xf)
			{
				tempValue = 0x0;
			}

			evaArray[evaIndex] = tempValue;
		}
		return;
	}

	//文件载入
	if (sysData.mode == smFile && sysData.file.isRestoring)
	{
		sysData.file.isRestoring = false;
	}

	//远程控制
	if (sysData.scpiData.stateR)
	{
		if (sysMenu->pressedIsEscKey(keyValue))
		{
			sysData.scpiData.stateR = false;
			sysData.scpiData.stateL = true;
			helpContent = getTheTranslatorString("switch to local control mode");
		} else
		{
			helpContent = getTheTranslatorString("remote control mode, pressed key is ignored");
		}

		drawHelp();
		return;
	}

	//系统自动校准
	if (sysData.isPreamplifierCalibrating || sysData.isZcPreamplifierCalibrating || sysData.isZcCalibrating || sysData.isFactoryCalibrating || sysData.isUserCalibrating || sysData.isCalibrating || sysData.mode == smHelp)
	{
		if (keyValue == KEY_MAIN_HELP && sysData.mode == smHelp)
		{
			sysData.mode = smSpectrum;
			helpMessage = "";
			sysScpi->checkMultiDisplay(mdtSpectrum);
			return;
		}

		if (sysData.mode == smHelp)
		{
			helpMessage = getHelpMessage(keyValue);
		}

		return;
	}

	//显示帮助
	if (keyValue == KEY_MAIN_HELP)
	{
		if (sysData.mode == smHelp)
		{
			if (sysData.canvasNums != FHelpPrvCanvasNums)
			{
				sysData.canvasNums = FHelpPrvCanvasNums;
			}

			sysData.mode = FHelpPrvMode;
		} else
		{
			FHelpPrvMode = sysData.mode;
			FHelpPrvCanvasNums = sysData.canvasNums;

			if (sysData.canvasNums != 1)
			{
				sysData.canvasNums = 1;
			}

			helpMessage = "※ " + getTheTranslatorString("Press any key for show help content") + "\n" + "※ " + getTheTranslatorString("Pressed help key again to close");
			sysData.mode = smHelp;
		}

		return;
	}

	//enable key
	if (nowSubKey != NULL)
	{
		if (prvKeyValue != keyValue)
		{
			nowSubKey->keyTimes = 1;
		} else
		{
			nowSubKey->keyTimes++;

			if (nowSubKey->keyTimes >= 2)
			{
				nowSubKey->keyTimes = 2;
			}
		}
	}

	prvKeyValue = keyValue;

	//如果有全屏取消全屏
	if (sysData.display.isFullScreen)
	{
		sysData.display.isFullScreen = false;
		drawAll();
		return;
	}

	//如果系统处于封锁状态
	if (sysData.isAutoSearching)
	{
		if (sysMenu->pressedIsMainKey(keyValue) && (keyValue == KEY_MAIN_PRESET || keyValue == KEY_MAIN_TUNE))
		{
			sysData.isAutoSearching = false;
		}
		return;
	}

	//执行变量
	bool canExecute = true;
	bool mkrExecute = true;

	//按下主键
	if (sysMenu->pressedIsMainKey(keyValue))
	{
		inputTag = "";
		tagValue = "";
		inputValue = "";
		prvKeyValue = 0;
		sysMenu->getMainKeyPointer(keyValue);

		if (nowMainKey == NULL)
		{
			return;
		}

		//如果是复位系统
		if (keyValue == KEY_MAIN_PRESET)
		{
			sysLocked = true;
			presetSystem(false);
			sysLocked = false;
		} else

		//自动搜索
		if (keyValue == KEY_MAIN_TUNE)
		{
			if (!sysData.isAutoSearching)
			{
				nowMainKey = sysKey;

				if (nowMainKey != NULL && nowMainKey->rightNode != NULL)
				{
					nowSubKey = nowMainKey->rightNode;
				}

				sysScpi->showRunMessage(getTheTranslatorString("signal searching") + "......");
				usleep(FORMCREATETIME);

				if (sysScpi->setAutoTune() == __SCPI_SUCCESS)
				{
					helpContent = getTheTranslatorString("Auto tune successed");
				} else
				{
					helpContent = getTheTranslatorString("Auto tune failed");
					sysScpi->showRunAlarm(getTheTranslatorString("searching failed"));
					usleep(1000 * 200);
				}

				drawHelp();
				drawButton();
				return;
			}
		} else

		//如果Marker Function, Marker To, Marker Peak
		if (keyValue == KEY_MAIN_MARKERFCTN || keyValue == KEY_MAIN_MARKERTO || keyValue == KEY_MAIN_PEAK)
		{
			if (sysData.mode == smFile)
			{
				sysData.mode = smSpectrum;
			}

			if (sysScpi->isMarkerAllOff() == __SCPI_SUCCESS)
			{
				sysScpi->setMarkerNormal(sysData.markerSelIndex, sysData.traceSelIndex);
			} else if (sysScpi->isTheMarkerOff(sysData.markerSelIndex, sysData.marker[sysData.markerSelIndex].traceIndex) == __SCPI_SUCCESS)
			{
				sysScpi->setMarkerNormal(sysData.markerSelIndex, sysData.marker[sysData.markerSelIndex].traceIndex);
			}

			if (keyValue == KEY_MAIN_PEAK)
			{
				sysScpi->setMarkerPeakMax(sysData.markerSelIndex, sysData.marker[sysData.markerSelIndex].traceIndex);
			}
		} else

		//如果显示文件
		if (keyValue == KEY_MAIN_FILE)
		{
			//      sysData.markerTableOn = false;
			//      sysData.peakTableOn = false;
			//      sysData.display.isZoomOn = false;
			sysScpi->getSystemFileList();
			sysScpi->checkMultiDisplay(mdtFile);
			sysData.canvasNums = 1;
			sysData.mode = smFile;
		} else

		//如果显示帮助
		if (keyValue == KEY_MAIN_HELP)
		{
			if (sysData.mode == smHelp)
			{
				sysData.mode = smSpectrum;

				if (sysData.canvasNums != FHelpPrvCanvasNums)
				{
					sysData.canvasNums = FHelpPrvCanvasNums;
				}
			} else
			{
				FHelpPrvCanvasNums = sysData.canvasNums;

				if (sysData.canvasNums != 1)
				{
					sysData.canvasNums = 1;
				}

				helpMessage = "※ " + getTheTranslatorString("Press any key for show help content") + "\n" + "※ " + getTheTranslatorString("Pressed help key again to close");
				sysData.mode = smHelp;
				sysScpi->checkMultiDisplay(mdtHelp);
			}
		} else
		{
			if (sysData.markerTableOn)
			{
				sysData.mode = smMarkerTable;
			} else if (sysData.display.isZoomOn)
			{
				sysData.mode = smZoom;
			} else if (sysData.measure.isTimeSpectrumOn)
			{
				sysData.mode = smTimeSpec;
			} else if (sysData.measure.isAcprOn)
			{
				sysData.mode = smAcpr;
			} else if (sysData.measure.isObwOn)
			{
				sysData.mode = smObw;
			} else if (sysData.peakTableOn)
			{
				sysData.mode = smPeakTable;
			} else
			{
				if (sysData.mode == smPassFail && sysData.canvasNums > 1)
				{
					sysData.canvasNums = 1;
					sysData.measure.passFail.line1Editing = false;
					sysData.measure.passFail.line2Editing = false;
				}

				sysData.mode = smSpectrum;
			}
		}

		//AM、FM新加逻辑，保证在AM、FM打开时候，其它主键不影响AM、FM功能
		if (keyValue != KEY_MAIN_FILE)
		{
			if (sysData.options.amOn)
			{
				sysData.canvasNums = 2;
				sysData.mode = smAM;
			} else if (sysData.options.fmOn)
			{
				sysData.canvasNums = 2;
				sysData.mode = smFM;
			}
		}

		//恢复系统逻辑
		if ((nowMainKey != NULL) && (nowMainKey->keyValue == KEY_MAIN_FREQ || nowMainKey->keyValue == KEY_MAIN_AMPT || nowMainKey->keyValue == KEY_MAIN_MARKERTO || nowMainKey->keyValue == KEY_MAIN_PEAK || nowMainKey->keyValue == KEY_MAIN_MEASSETUP))
		{
			resetAllLogic();
		}

		//默认选中第一个子键
		if (nowMainKey != NULL && nowMainKey->rightNode != NULL)
		{
			if (nowMainKey->rightNode->isEnabled)
			{
				nowSubKey = nowMainKey->rightNode;
				keyValue = nowSubKey->keyValue;
				canExecute = false;
			} else
			{
				subKeyDef* tempSubKey = nowMainKey->rightNode;
				int enableCount = 0;

				while (tempSubKey != NULL)
				{
					if (tempSubKey->isEnabled)
					{
						enableCount = 1;
						nowSubKey = tempSubKey;
						keyValue = nowSubKey->keyValue;
						canExecute = false;
					}

					tempSubKey = tempSubKey->downNode;
				}

				if (enableCount == 0)
				{
					nowSubKey = nowMainKey->rightNode;
					keyValue = nowSubKey->keyValue;
					canExecute = false;
					drawButton();
					return;
				}
			}
		}

		//如果需要对应显示
		if ((nowMainKey != NULL) && (nowSubKey != NULL))
		{
			struct subKeyDef* tempSubKey = nowSubKey;
			unsigned long long funcCode = 0;

			if (nowMainKey->keyValue == KEY_MAIN_TRIG)
			{
				switch (sysData.trig.mode)
				{
					case tgRun:
						funcCode = KEY_FUNC_TRIG_AUTORUN;
						break;
					case tgVideo:
						funcCode = KEY_FUNC_TRIG_VIDEO;
						break;
					case tgRise:
						funcCode = KEY_FUNC_TRIG_EXTERNAL_RISE;
						break;
					case tgFall:
						funcCode = KEY_FUNC_TRIG_EXTERNAL_FALL;
						break;
					default:
						break;
				}

				while (tempSubKey != NULL)
				{
					if (tempSubKey->funcCode == KEY_FUNC_TRIG_EXTERNAL)
					{
						tempSubKey = tempSubKey->rightNode;
					}

					if (tempSubKey->funcCode == funcCode)
					{
						nowSubKey = tempSubKey;
						keyValue = nowSubKey->keyValue;
						break;
					}

					tempSubKey = tempSubKey->downNode;
				}
			} else if (nowMainKey->keyValue == KEY_MAIN_DETECTOR) //显示检波方式
			{
				switch (sysData.detector.detMode)
				{
					case dmAuto:
						funcCode = KEY_FUNC_DETECTOR_AUTO;
						break;
					case dmNormal:
						funcCode = KEY_FUNC_DETECTOR_NORMAL;
						break;
					case dmPosPeak:
						funcCode = KEY_FUNC_DETECTOR_POSPEAK;
						break;
					case dmNegPeak:
						funcCode = KEY_FUNC_DETECTOR_NEGPEAK;
						break;
					case dmSample:
						funcCode = KEY_FUNC_DETECTOR_SAMPLE;
						break;
					default:
						break;
				}

				while (tempSubKey != NULL)
				{
					if (tempSubKey->funcCode == funcCode)
					{
						nowSubKey = tempSubKey;
						keyValue = nowSubKey->keyValue;
						break;
					}

					tempSubKey = tempSubKey->downNode;
				}
			} else if (nowMainKey->keyValue == KEY_MAIN_TRACE) //显示迹线状态
			{
				switch (sysData.trace[sysData.traceSelIndex].state)
				{
					case tsClearWrite:
						funcCode = KEY_FUNC_TRACE_CLEARWRITE;
						break;
					case tsMaxHold:
						funcCode = KEY_FUNC_TRACE_MAXHOLD;
						break;
					case tsMinHold:
						funcCode = KEY_FUNC_TRACE_MINHOLD;
						break;
					case tsView:
						funcCode = KEY_FUNC_TRACE_VIEW;
						break;
					case tsBlank:
						funcCode = KEY_FUNC_TRACE_BLANK;
						break;
				}

				while (tempSubKey != NULL)
				{
					if (tempSubKey->funcCode == funcCode)
					{
						nowSubKey = tempSubKey;
						keyValue = nowSubKey->keyValue;
						break;
					}

					tempSubKey = tempSubKey->downNode;
				}
			} else if (nowMainKey->keyValue == KEY_MAIN_MEAS) //显示测量
			{
				if (sysData.measure.isTimeSpectrumOn || sysData.measure.isAcprOn || sysData.measure.isChannelPowerOn || sysData.measure.isObwOn)
				{
					if (sysData.measure.isTimeSpectrumOn)
					{
						funcCode = KEY_FUNC_MEAS_TIMESPEC;
					} else if (sysData.measure.isAcprOn)
					{
						funcCode = KEY_FUNC_MEAS_ACPR;
					} else if (sysData.measure.isChannelPowerOn)
					{
						funcCode = KEY_FUNC_MEAS_CHANNELPOWER;
					} else if (sysData.measure.isObwOn)
					{
						funcCode = KEY_FUNC_MEAS_OBW;
					}

					while (tempSubKey != NULL)
					{
						if (tempSubKey->funcCode == funcCode)
						{
							nowSubKey = tempSubKey;
							keyValue = nowSubKey->keyValue;
							break;
						}
						tempSubKey = tempSubKey->downNode;
					}
				}
			} else if (nowMainKey->keyValue == KEY_MAIN_MARKERFCTN) //显示标记功能
			{
				if (sysData.markerFctn.isNdBOn)
				{
					funcCode = KEY_FUNC_MARKERFCTN_NDB;
				} else if (sysData.markerFctn.isMkrNoiseOn)
				{
					funcCode = KEY_FUNC_MARKERFCTN_NOISE;
				}

				while (tempSubKey != NULL)
				{
					if (tempSubKey->funcCode == funcCode)
					{
						nowSubKey = tempSubKey;
						keyValue = nowSubKey->keyValue;
						break;
					}

					tempSubKey = tempSubKey->downNode;
				}
			} else if (nowMainKey->keyValue == KEY_MAIN_MARKER) //显示标记
			{
				if (sysScpi->isMarkerAllOff() == __SCPI_SUCCESS)
				{
					sysScpi->setMarkerNormal(sysData.markerSelIndex, sysData.traceSelIndex);
				}

				switch (sysData.marker[sysData.markerSelIndex].state)
				{
					case msClose:
						funcCode = KEY_FUNC_MARKER_OFF;
						break;
					case msNormal:
						funcCode = KEY_FUNC_MARKER_NORMAL;
						break;
					case msDelta:
						funcCode = KEY_FUNC_MARKER_DELTA;
						break;
				}

				while (tempSubKey != NULL)
				{
					if (tempSubKey->funcCode == funcCode)
					{
						nowSubKey = tempSubKey;
						keyValue = nowSubKey->keyValue;
						break;
					}

					tempSubKey = tempSubKey->downNode;
				}

				mkrExecute = false;
			}
		}
	}

	//按下子键
	if (sysMenu->pressedIsCommonKey(keyValue) && keyIsEnable(keyValue))
	{
		if (sysMenu->getSubKeyPointer(keyValue))
		{
			helpContent = getTheTranslatorString("Ready");
			drawHelp();

			//initialize pass fail window meas param
			if (nowSubKey->funcCode == KEY_FUNC_AMPT_PASSFAIL_WINDOW)
			{
				if (!sysData.measure.passFail.winMeasOn && !sysData.measure.passFail.winFreqOn && !sysData.measure.passFail.winAmptOn && sysData.measure.passFail.winFreqStart <= 0 && sysData.measure.passFail.winFreqStop <= 0 && sysData.measure.passFail.winAmptUp <= 0 && sysData.measure.passFail.winAmptLow <= 0)
				{
					sysData.measure.passFail.winFreqStart = sysData.freq.center - sysData.span.span / 10.0;
					sysData.measure.passFail.winFreqStop = sysData.freq.center + sysData.span.span / 10.0;
					sysData.measure.passFail.winAmptUp = sysData.ampt.refLevel - sysData.ampt.scaleDiv * 1.0;
					sysData.measure.passFail.winAmptLow = sysData.ampt.refLevel - sysData.ampt.scaleDiv * 2.0;
				}
			}

			//enter pass fail mode
			if (sysData.mode != smPassFail && (nowSubKey->funcCode == KEY_FUNC_AMPT_PASSFAIL_LINE_LINE1EDIT || nowSubKey->funcCode == KEY_FUNC_AMPT_PASSFAIL_LINE_LINE2EDIT))
			{
				sysScpi->checkMultiDisplay(mdtPassFail);
				sysData.mode = smPassFail;
				sysData.canvasNums = 2;
				sysData.measure.passFail.line1Editing = nowSubKey->funcCode == KEY_FUNC_AMPT_PASSFAIL_LINE_LINE1EDIT;
				sysData.measure.passFail.line2Editing = nowSubKey->funcCode == KEY_FUNC_AMPT_PASSFAIL_LINE_LINE2EDIT;

				if (sysData.measure.passFail.line1Editing)
				{
					sysScpi->refreshMeasureOfPassFailOLineUpPages();
				}

				if (sysData.measure.passFail.line2Editing)
				{
					sysScpi->refreshMeasureOfPassFailOLineLowPages();
				}
			}

			//isShow Center Freq
			if ((nowSubKey->funcCode == KEY_FUNC_FREQ_CENTER) && (!sysData.freq.isShowCenter))
			{
				sysData.freq.isShowCenter = true;
			} else if (((nowSubKey->funcCode == KEY_FUNC_FREQ_START) || (nowSubKey->funcCode == KEY_FUNC_FREQ_STOP)) && (sysData.freq.isShowCenter))
			{
				sysData.freq.isShowCenter = false;
			}

			//isMore
			if ((nowSubKey->isMore) && (nowSubKey->rightNode != NULL))
			{
				if (nowSubKey->isPassword)
				{
					inputTag = getTheTranslatorString("password");
					tagValue = "******";
					inputValue = "******";
					prevInput = "";
					previndexOfPassword = 0;
					indexOfPassword = 0;
					memset(passwdString, 0, sizeof passwdString);
				} else
				{
					inputTag = "";
					tagValue = "";
					inputValue = "";
					prevInput = "";

					if (nowSubKey != NULL && nowSubKey->rightNode != NULL && nowSubKey->funcCode == KEY_FUNC_SOURCE_TRACKGEN_NETWORK)
					{
						sysData.source.trackGenNetworkRefLevel = sysData.ampt.refLevel;
						sysData.source.trackGenNetworkOutputPower = sysData.source.trackGenOutputPower;
					} else if (nowSubKey != NULL && nowSubKey->rightNode != NULL && nowSubKey->funcCode == KEY_FUNC_SYSTEM_CALIBRATION)
					{
						sysScpi->showRunAlarm(getTheTranslatorString("Please connect RF:") + "440MHz, -20dBm");
						sleep(2);
					}

					if (nowSubKey != NULL && nowSubKey->funcCode == KEY_FUNC_TRIG_EXTERNAL)
					{
						if (sysData.trig.mode == tgRise)
						{
							nowSubKey = nowSubKey->rightNode;
							//handleExecuteKey(nowSubKey->funcCode);
						} else if (sysData.trig.mode == tgFall)
						{
							nowSubKey = nowSubKey->rightNode->downNode;
							//handleExecuteKey(nowSubKey->funcCode);
						} else
						{
							//handleExecuteKey(nowSubKey->rightNode->funcCode);
						}

						sysMenu->gotoDownLevel();
					}

					if (canExecute)
					{
						sysMenu->gotoDownLevel();
					}

					canExecute = false;

					if (nowSubKey != NULL && nowSubKey->isInput)
					{
						inputTag = getTranslatorString(nowSubKey->enName, nowSubKey->chName);
						handleInputKey(nowSubKey->funcCode);
					}
				}
			} else

			//isReturn
			if (nowSubKey->isReturn)
			{
				inputTag = "";
				tagValue = "";
				inputValue = "";
				prevInput = "";

				sysMenu->gotoUpLevel();
				canExecute = false;

				if (sysData.mode == smSystemInfo || sysData.mode == smSystemLog)
				{
					sysData.mode = smSpectrum;
					sysData.canvasNums = 1;
				}

				//pass fail mode return
				if (sysData.mode == smPassFail && (nowSubKey->funcCode == KEY_FUNC_AMPT_PASSFAIL_LINE_LINE1EDIT || nowSubKey->funcCode == KEY_FUNC_AMPT_PASSFAIL_LINE_LINE2EDIT))
				{
					sysData.mode = smSpectrum;
					sysData.canvasNums = 1;
				}
			} else

			//isInput & isReverse
			if ((nowSubKey->isInput) && (nowSubKey->isReverse))
			{
				if (nowSubKey->keyTimes <= 0)
				{
					nowSubKey->keyTimes++;
				}

				if (nowSubKey->keyTimes == 2)
				{
					handleReverseKey(nowSubKey->funcCode);
					handleInputKey(nowSubKey->funcCode);
				}

				if (nowSubKey->keyTimes == 1)
				{
					inputTag = getTranslatorString(nowSubKey->enName, nowSubKey->chName);

					//pass-fail
					switch (nowSubKey->funcCode)
					{
						case KEY_FUNC_AMPT_PASSFAIL_WINDOW_LIMITUPLOW:
							if (sysData.measure.passFail.winNowAmptUpOrLow == 0)
							{
								inputTag = getTheTranslatorString("Up") + inputTag;
							} else
							{
								inputTag = getTheTranslatorString("Low") + inputTag;
							}
							break;
						case KEY_FUNC_AMPT_PASSFAIL_WINDOW_FREQSTARTSTOP:
							if (sysData.measure.passFail.winNowFreqStartOrStop == 0)
							{
								inputTag = getTheTranslatorString("Start") + inputTag;
							} else
							{
								inputTag = getTheTranslatorString("Stop") + inputTag;
							}
							break;
						case KEY_FUNC_AMPT_PASSFAIL_LINE_SHIFTXY:
							if (sysData.measure.passFail.lineNowShiftXOrY == 0)
							{
								inputTag = getTheTranslatorString("Offset") + "X";
							} else
							{
								inputTag = getTheTranslatorString("Offset") + "Y";
							}
							break;
					}

					handleInputKey(nowSubKey->funcCode);
				}
			} else

			//isInput & isExecute
			if (nowSubKey->isInput && nowSubKey->isExecute)
			{
				//        inputTag = "";
				//        tagValue = "";
				//        inputValue = "";
				//        prevInput = "";
				inputTag = getTranslatorString(nowSubKey->enName, nowSubKey->chName);

				if (nowSubKey->funcCode == KEY_FUNC_TRIG_VIDEO)
				{
					inputTag = getTheTranslatorString("Trigger Level");
					handleExecuteKey(nowSubKey->funcCode);
					handleInputKey(nowSubKey->funcCode);
				}

				if (nowSubKey->funcCode == KEY_FUNC_MARKER_DELTA && !mkrExecute)
				{

				} else
				{
					handleExecuteKey(nowSubKey->funcCode);
					handleInputKey(nowSubKey->funcCode);
				}

				if (isExecReturn(nowSubKey->funcCode))
				{
					sysMenu->gotoUpLevel();
				}
			} else

			//isReverse
			if ((nowSubKey->isReverse) && (canExecute))
			{
				inputTag = "";
				tagValue = "";
				inputValue = "";
				prevInput = "";
				handleReverseKey(nowSubKey->funcCode);
			} else

			//isList
			if (nowSubKey->isList)
			{
				inputTag = "";
				tagValue = "";
				inputValue = "";
				prevInput = "";

				if (canExecute)
				{
					handleListKey(nowSubKey->funcCode);

					if ((nowMainKey != NULL) && (nowSubKey != NULL))
					{
						struct subKeyDef* tempSubKey = nowSubKey;

						if (nowMainKey->keyValue == KEY_MAIN_TRACE)
						{
							unsigned long long funcCode = 0;

							switch (sysData.trace[sysData.traceSelIndex].state)
							{
								case tsClearWrite:
									funcCode = KEY_FUNC_TRACE_CLEARWRITE;
									break;
								case tsMaxHold:
									funcCode = KEY_FUNC_TRACE_MAXHOLD;
									break;
								case tsMinHold:
									funcCode = KEY_FUNC_TRACE_MINHOLD;
									break;
								case tsView:
									funcCode = KEY_FUNC_TRACE_VIEW;
									break;
								case tsBlank:
									funcCode = KEY_FUNC_TRACE_BLANK;
									break;
							}

							while (tempSubKey != NULL)
							{
								if (tempSubKey->funcCode == funcCode)
								{
									nowSubKey = tempSubKey;
									keyValue = nowSubKey->keyValue;
									break;
								}

								tempSubKey = tempSubKey->downNode;
							}
						}
					}
				}
			} else

			//isInput
			if (nowSubKey->isInput)
			{
				inputTag = getTranslatorString(nowSubKey->enName, nowSubKey->chName);
				tagValue = "";
				inputValue = "";
				nowSubKey->keyTimes++;
				handleInputKey(nowSubKey->funcCode);
			} else

			//isExecute
			if ((nowSubKey->isExecute) && (canExecute))
			{
				inputTag = "";
				tagValue = "";
				inputValue = "";
				handleExecuteKey(nowSubKey->funcCode);

				if (isExecReturn(nowSubKey->funcCode))
				{
					sysMenu->gotoUpLevel();
				}
			} else

			//isUnit
			if (nowSubKey->isUnit)
			{
				int unitKeyFuncCode = nowSubKey->funcCode;

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
					handleUnitKey(unitKeyFuncCode, inputValue);
					inputValue = "";
					handleInputKey(nowSubKey->funcCode);
					prvKeyValue = nowSubKey->keyValue;
					nowSubKey->keyTimes = 1;
				}
			} else
			{

			}
		} else
		{
			helpContent = "pressed key is ignored";
			drawHelp();
		}
	} else

	//按下方向键
	if ((nowMainKey != NULL) && (nowSubKey != NULL) && (nowSubKey->isEnabled) && (sysMenu->pressedIsArrowKey(keyValue)))
	{
		//是不是选中密码位置
		if (nowSubKey->isPassword && indexOfPassword < 1)
		{
			return;
		}

		handleArrowKey(nowSubKey->funcCode, sysMenu->isUpArrow(keyValue));
		handleInputKey(nowSubKey->funcCode);

		if (nowSubKey != NULL && nowSubKey->isInput && nowSubKey->isReverse)
		{
			nowSubKey->keyTimes = 2;
		}
	} else

	//按下RPG键
	if ((nowMainKey != NULL) && (nowSubKey != NULL) && (nowSubKey->isEnabled) && (sysMenu->pressedIsRpgKey(keyClass)))
	{
		//是不是选中密码位置
		if (nowSubKey->isPassword && indexOfPassword < 1)
		{
			return;
		}

		handleRpgKey(nowSubKey->funcCode, sysMenu->getRpgPulse(keyValue, keyCount));
		handleInputKey(nowSubKey->funcCode);

		if (nowSubKey != NULL && nowSubKey->isInput && nowSubKey->isReverse)
		{
			nowSubKey->keyTimes = 2;
		}
	} else

	//按下数字键
	if ((sysMenu->pressedIsNumberKey(keyValue)) && (nowSubKey != NULL) && (nowSubKey->isEnabled))
	{
		if ((nowSubKey->isInput) && (nowSubKey->rightNode != NULL))
		{
			if (nowSubKey->funcCode == KEY_FUNC_AMPT_PASSFAIL_LINE_SHIFTXY)
			{
				if (sysData.measure.passFail.lineNowShiftXOrY == 0)
				{
					if (nowSubKey->rightNode->downNode != NULL)
						nowSubKey->rightNode->downNode->keyValue = KEY_SUB_MENU2;
					if (nowSubKey->rightNode->downNode->downNode != NULL)
						nowSubKey->rightNode->downNode->downNode->keyValue = KEY_SUB_MENU3;
					if (nowSubKey->rightNode->downNode->downNode->downNode != NULL)
						nowSubKey->rightNode->downNode->downNode->downNode->keyValue = KEY_SUB_MENU4;
				} else
				{
					if (nowSubKey->rightNode->downNode != NULL)
						nowSubKey->rightNode->downNode->keyValue = 0x0;
					if (nowSubKey->rightNode->downNode->downNode != NULL)
						nowSubKey->rightNode->downNode->downNode->keyValue = 0x0;
					if (nowSubKey->rightNode->downNode->downNode->downNode != NULL)
						nowSubKey->rightNode->downNode->downNode->downNode->keyValue = 0x0;
				}
			}

			nowSubKey = nowSubKey->rightNode;
			drawButton();
		} else if (nowSubKey->isExecute) //2015.07.03 频谱仪质量提升
		{
			return;
		}

		switch (keyValue)
		{
			case KEY_NUM_0:
				execNumberInput("0", true);
				return;// if (isInputPasswordValid(inputValue)) drawButton(); return;
			case KEY_NUM_1:
				execNumberInput("1", true);
				return;// if (isInputPasswordValid(inputValue)) drawButton(); return;
			case KEY_NUM_2:
				execNumberInput("2", true);
				return;// if (isInputPasswordValid(inputValue)) drawButton(); return;
			case KEY_NUM_3:
				execNumberInput("3", true);
				return;// if (isInputPasswordValid(inputValue)) drawButton(); return;
			case KEY_NUM_4:
				execNumberInput("4", true);
				return;// if (isInputPasswordValid(inputValue)) drawButton(); return;
			case KEY_NUM_5:
				execNumberInput("5", true);
				return;// if (isInputPasswordValid(inputValue)) drawButton(); return;
			case KEY_NUM_6:
				execNumberInput("6", true);
				return;// if (isInputPasswordValid(inputValue)) drawButton(); return;
			case KEY_NUM_7:
				execNumberInput("7", true);
				return;// if (isInputPasswordValid(inputValue)) drawButton(); return;
			case KEY_NUM_8:
				execNumberInput("8", true);
				return;// if (isInputPasswordValid(inputValue)) drawButton(); return;
			case KEY_NUM_9:
				execNumberInput("9", true);
				return;// if (isInputPasswordValid(inputValue)) drawButton(); return;
			case KEY_NUM_DOT:
				if (!nowSubKey->isPassword && dotInputIsValid())
				{
					if (inputValue.isEmpty())
					{
						inputValue = "0.";
					} else if (inputValue == "-")
					{
						inputValue = "-0.";
					} else
					{
						inputValue += ".";
					}
				}

				return;

			case KEY_NUM_NEG:
				if (!nowSubKey->isPassword)
				{
					if ((inputValue.isEmpty()) || ((inputValue.length() > 0) && (inputValue.at(0) != '-')))
					{
						inputValue = "-" + inputValue;
					} else
					{
						inputValue.remove(0, 1);
					}
				}

				return;
		}
	} else

	//按下回滚键
	if ((sysMenu->pressedIsBkspaceKey(keyValue)) && (nowSubKey != NULL))
	{
		if (nowSubKey->isPassword)
		{
			if (!inputValue.isEmpty())
			{
				indexOfPassword--;
				previndexOfPassword--;
				inputValue.remove(inputValue.length() - 1, 1);
			}

			if (inputValue.isEmpty() || indexOfPassword < 0 || previndexOfPassword < 0)
			{
				indexOfPassword = 0;
				previndexOfPassword = 0;
				inputValue = "******";
			}
		} else
		{
			if (!inputValue.isEmpty())
			{
				inputValue.remove(inputValue.length() - 1, 1);
			}

			if ((inputValue.isEmpty()))
			{
				//        inputTag = "";
				//        tagValue = "";
				//        inputValue = "";

				if (nowSubKey->isUnit)
				{
					sysMenu->gotoUpLevel();
					drawButton();
				}
			}
		}

		return;
	} else

	//按下取消键
	if ((sysMenu->pressedIsEscKey(keyValue)) && (nowSubKey != NULL))
	{
		if (nowSubKey->isUnit)
		{
			sysMenu->gotoUpLevel();
			drawButton();
		}

		indexOfPassword = 0;
		previndexOfPassword = 0;
		memset(passwdString, 0, sizeof passwdString);

		if (nowSubKey->isPassword)
		{
			prevInput = "";
			inputValue = "******";
		} else
		{
			inputValue = "";
		}

		return;
	} else

	//密码确认键
	if (sysMenu->pressedIsEnterKey(keyValue) && (nowSubKey != NULL) && (nowSubKey->rightNode != NULL) && (!inputValue.isNull()) && (!inputValue.isEmpty()))
	{
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
				return;
			} else
			{
				inputValue = "******";
				sysData.system.isServiceUnlock = false;
				sysScpi->showRunAlarm(getTheTranslatorString("Passwd error, Please Resume Input!"));
				sleep(1);
				return;
			}
		}
	} else

	//按下确认键
	if (sysMenu->pressedIsEnterKey(keyValue) && (nowSubKey != NULL) && (nowSubKey->leftNode != NULL) && (!inputValue.isNull()) && (!inputValue.isEmpty()))
	{
		subKeyDef* tempSubKey = nowSubKey;

		while (tempSubKey->downNode != NULL)
		{
			if (tempSubKey->downNode == NULL)
			{
				break;
			}

			tempSubKey = tempSubKey->downNode;
		}

		handleUnitKey(tempSubKey->funcCode, inputValue);
		nowSubKey = nowSubKey->leftNode;
		inputValue = "";
		handleInputKey(nowSubKey->funcCode);
		prvKeyValue = nowSubKey->keyValue;
		nowSubKey->keyTimes = 2;
	} else

	//按下硬单位键
	if (sysMenu->pressedIsUnitKey(keyValue) && (nowSubKey != NULL) && (nowSubKey->leftNode != NULL) && (!inputValue.isNull()) && (!inputValue.isEmpty()))
	{
		subKeyDef* tempSubKey = nowSubKey;

		while (tempSubKey->upNode != NULL)
		{
			if (tempSubKey->upNode == NULL)
			{
				break;
			}

			tempSubKey = tempSubKey->upNode;
		}

		if ((keyValue == KEY_UNIT_GHZ) && (tempSubKey != NULL))
		{
			handleUnitKey(tempSubKey->funcCode, inputValue);
		} else if ((keyValue == KEY_UNIT_MHZ) && (tempSubKey != NULL) && (tempSubKey->downNode != NULL))
		{
			handleUnitKey(tempSubKey->downNode->funcCode, inputValue);
		} else if ((keyValue == KEY_UNIT_KHZ) && (tempSubKey != NULL) && (tempSubKey->downNode != NULL) && (tempSubKey->downNode->downNode != NULL))
		{
			handleUnitKey(tempSubKey->downNode->downNode->funcCode, inputValue);
		} else if ((keyValue == KEY_UNIT_HZ) && (tempSubKey != NULL) && (tempSubKey->downNode != NULL) && (tempSubKey->downNode->downNode != NULL) && (tempSubKey->downNode->downNode->downNode != NULL))
		{
			handleUnitKey(tempSubKey->downNode->downNode->downNode->funcCode, inputValue);
		} else if (tempSubKey != NULL)
		{
			handleUnitKey(tempSubKey->funcCode, inputValue);
		}

		nowSubKey = nowSubKey->leftNode;
		inputValue = "";
		handleInputKey(nowSubKey->funcCode);
		prvKeyValue = nowSubKey->keyValue;
		nowSubKey->keyTimes = 2;
	} else

	//其它未知键
	{
		inputValue = "";
		return;
	}

	//绘制菜单
	drawButton();
}

//复位USB从口
void mainForm::resetUsbSlaveInterface()
{
	if (usbHandle >= 0)
	{
		disconnect(usbNotifier, SIGNAL(activated(int)), sysScpi, SLOT(readUsbSlaveData()));
		::close(usbHandle);
	}

	usbHandle = -1;
	usbHandle = open(DEVICE_NAME_USB, O_RDWR | O_NONBLOCK);

	if (usbHandle >= 0)
	{
connect	(usbNotifier, SIGNAL(activated(int)), sysScpi, SLOT(readUsbSlaveData()));
}
}

//执行文件选择
bool mainForm::handleFileSelect(bool up, int nums)
{
	if (nowMainKey != NULL && nowMainKey->keyValue == KEY_MAIN_FILE && sysData.file.nowPageCount > 0)
	{
		int index = sysData.file.nowSelIndex;

		if (up && index > 0)
		{
			index -= qAbs(nums);
		} else if (!up && index < sysData.file.nowPageCount - 1)
		{
			index += qAbs(nums);
		}

		if (index < 0)
		{
			index = 0;
		} else if (index > sysData.file.nowPageCount - 1)
		{
			index = sysData.file.nowPageCount - 1;
		}

		sysData.file.nowSelIndex = index;

		return true;
	}

	return false;
}

//获取主键名称
QString mainForm::getMainKeyName(void)
{
	if (nowMainKey == NULL)
	{
		return getTheTranslatorString("unKnown");
	}

	switch (nowMainKey->keyValue)
	{
		case KEY_MAIN_FREQ:
			return getTheTranslatorString("Frequency");
		case KEY_MAIN_SPAN:
			return getTheTranslatorString("Span");
		case KEY_MAIN_AMPT:
			return getTheTranslatorString("Amplitude");
		case KEY_MAIN_TUNE:
			return getTheTranslatorString("Tune");

		case KEY_MAIN_BW:
			return getTheTranslatorString("Band Width");
		case KEY_MAIN_TRACE:
			return getTheTranslatorString("Trace");
		case KEY_MAIN_TRIG:
			return getTheTranslatorString("Trig");
		case KEY_MAIN_MEAS:
			return getTheTranslatorString("Measure");

		case KEY_MAIN_SWEEP:
			return getTheTranslatorString("Sweep");
		case KEY_MAIN_DETECTOR:
			return getTheTranslatorString("Detector");
		case KEY_MAIN_DISPLAY:
			return getTheTranslatorString("Display");
		case KEY_MAIN_MEASSETUP:
			return getTheTranslatorString("Measure Setup");

		case KEY_MAIN_MARKER:
			return getTheTranslatorString("Marker");
		case KEY_MAIN_MARKERFCTN:
			return getTheTranslatorString("Marker Function");
		case KEY_MAIN_MARKERTO:
			return getTheTranslatorString("Maker To");
		case KEY_MAIN_PEAK:
			return getTheTranslatorString("Peak");

		case KEY_MAIN_SYSTEM:
			return getTheTranslatorString("System");
		case KEY_MAIN_FILE:
			return getTheTranslatorString("File");
		case KEY_MAIN_SAVE:
			return getTheTranslatorString("Save");
		case KEY_MAIN_SOURCE:
			return getTheTranslatorString("Source");

		case KEY_MAIN_PRESET:
			return getTheTranslatorString("Preset");
		case KEY_MAIN_PRINT:
			return getTheTranslatorString("Print");
		case KEY_MAIN_HELP:
			return getTheTranslatorString("Help");
		case KEY_MAIN_DEMOD:
			return getTheTranslatorString("Demod");

		default:
			return getTheTranslatorString("unKnown");
	}
}

//更新标记状态
void mainForm::refreshMarkerState(void)
{
	if (nowSubKey == NULL)
	{
		return;
	}

	subKeyDef* tempSubKey = nowSubKey;
	unsigned long funcCode = 0;

	switch (sysData.marker[sysData.markerSelIndex].state)
	{
		case msClose:
			funcCode = KEY_FUNC_MARKER_OFF;
			break;
		case msNormal:
			funcCode = KEY_FUNC_MARKER_NORMAL;
			break;
		case msDelta:
			funcCode = KEY_FUNC_MARKER_DELTA;
			break;
	}

	while (tempSubKey->upNode != NULL)
	{
		tempSubKey = tempSubKey->upNode;
	}

	while (tempSubKey != NULL)
	{
		if (tempSubKey->funcCode == funcCode)
		{
			nowSubKey = tempSubKey;
			break;
		}

		tempSubKey = tempSubKey->downNode;
	}
}

//执行数字输入
void mainForm::execNumberInput(QString value, bool isNumber)
{
	if (isPassword())
	{
		char tempPasswd[6];
		QString passwdStr = "";

		//是否为数字键，每按一次数字键，密码的位置加1
		if (isNumber)
		{
			indexOfPassword++;

			if (indexOfPassword > MAXINPUTLENGTH)
			{
				indexOfPassword = MAXINPUTLENGTH;
			}
		}

		int index = indexOfPassword - 1;

		switch (value.at(0).toAscii())
		{
			case '0':
				passwdString[index] = 0;
				break;
			case '1':
				passwdString[index] = 1;
				break;
			case '2':
				passwdString[index] = 2;
				break;
			case '3':
				passwdString[index] = 3;
				break;
			case '4':
				passwdString[index] = 4;
				break;
			case '5':
				passwdString[index] = 5;
				break;
			case '6':
				passwdString[index] = 6;
				break;
			case '7':
				passwdString[index] = 7;
				break;
			case '8':
				passwdString[index] = 8;
				break;
			case '9':
				passwdString[index] = 9;
				break;
			case 'a':
				passwdString[index] = 10;
				break;
			case 'A':
				passwdString[index] = 10;
				break;
			case 'b':
				passwdString[index] = 11;
				break;
			case 'B':
				passwdString[index] = 11;
				break;
			case 'c':
				passwdString[index] = 12;
				break;
			case 'C':
				passwdString[index] = 12;
				break;
			case 'd':
				passwdString[index] = 13;
				break;
			case 'D':
				passwdString[index] = 13;
				break;
			case 'e':
				passwdString[index] = 14;
				break;
			case 'E':
				passwdString[index] = 14;
				break;
			case 'f':
				passwdString[index] = 15;
				break;
			case 'F':
				passwdString[index] = 15;
				break;
		}

		int size = sizeof passwdString / sizeof passwdString[0];

		if (inputValue == "******" || previndexOfPassword == 0 || indexOfPassword == 0)
		{
			inputValue = value;
		} else
		{
			if (indexOfPassword > previndexOfPassword) //显示字符的位置改变
			{
				inputValue += value;

				if (previndexOfPassword == size - 1)
				{
					prevInput = inputValue;
				}
			} else //显示字符的位置不改变
			{
				if (isNumber)
				{
					inputValue = prevInput;
				} else
				{
					passwdStr = inputValue.mid(0, indexOfPassword - 1);
					passwdStr += value;
					inputValue = passwdStr;
					prevInput = inputValue;
				}
			}
		}

		previndexOfPassword = indexOfPassword;
	} else
	{
		if (inputValue.length() <= MAXINPUTLENGTH)
		{
			inputValue += value;
		}
	}
}

//口令
bool mainForm::isPassword(void)
{
	return nowSubKey != NULL && nowSubKey->funcCode == KEY_FUNC_SYSTEM_SERVICE;
}

//口令正确
bool mainForm::isPasswordValid(void)
{
	return nowSubKey != NULL && nowSubKey->funcCode == KEY_FUNC_SYSTEM_SERVICE && !inputValue.isEmpty() && inputValue == (QString) PASSWORD;
}

//点输入有效
bool mainForm::dotInputIsValid(void)
{
	bool result = false;
	inputType iType = itUnDefine;

	if (nowSubKey != NULL)
	{
		switch (nowSubKey->funcCode)
		{
			case KEY_FUNC_SYSTEM_CONFIGIO_LAN_IP_ENTER:
				iType = itIP;
				break;
			case KEY_FUNC_SYSTEM_CONFIGIO_LAN_MASK_ENTER:
				iType = itIP;
				break;
			case KEY_FUNC_SYSTEM_CONFIGIO_LAN_GATE_ENTER:
				iType = itIP;
				break;
			default:
				iType = itDouble;
				break;
		}
	}

	switch (iType)
	{
		case itDouble:
			result = inputValue.isEmpty() || inputValue.indexOf('.') == -1;
			break;
		case itIP:
			result = inputValue.isEmpty() || inputValue.count('.') <= 2;
			break;
		default:
			result = true;
	}

	return result;
}

//按键有效
bool mainForm::keyIsEnable(int value)
{
	if (nowMainKey == NULL)
	{
		return false;
	}

	subKeyDef* tempSubKey = nowSubKey;

	if (nowSubKey != NULL)
	{
		while (tempSubKey->upNode != NULL)
		{
			tempSubKey = tempSubKey->upNode;
		}
	} else
	{
		tempSubKey = nowMainKey->rightNode;
	}

	while (tempSubKey != NULL)
	{
		if (tempSubKey->keyValue == value)
		{
			return tempSubKey->isEnabled;
		}

		tempSubKey = tempSubKey->downNode;
	}

	return false;
}

void mainForm::customEvent(QEvent* e)
{
	if (e == NULL)
	{
		//return;
	}

	switch (e->type())
	{
		case fftInt:
			readIntData();
			break;
		default:
			break;
	}

}

void sigio(int x)
{
	tSysScpi* sysScpi;
	if (eventObject != NULL && sysData.span.isZeroSpan == true)
	{
		//postEvent 实现将自定义的消息发送到队列，且new QEvent(CustomEvent_Login)只能动态分配
		//只要实现父类中的event()或customEvent()函数就可以 实现   消息队列将信息抛出来 的功能
			QCoreApplication::sendEvent(eventObject, fftEvent);
			//printf("SIGIO processing...\n");
			//sysScpi->getDataFromIF();
			//printf("SIGIO processed\n");
	}
}

void mainForm::readIntData(void)
{
//	return;
	if (sysData.span.isZeroSpan)
		sysScpi->getDataFromIF();
#if 0
	static int x = 0;
	x++;
	if(x >= 1)
	{
		//__pile();
		//int i = 0;
		//  QTime t;
		//  t.restart();
		  mutexIF.lock();
			//for( i = 0; i < RAMDATASIZE; i++)
			//{
				// 地址  :37
				//ramDownload(37, i + 1);

				//时钟 :38
				//ramDownload(38, 1);
				//ramDownload(38, 0);

				//读取数据
				//read(ramHandle, ifDataBuf, sizeof ifDataBuf);
				//for (i = 0; i < RAMDATASIZE; i++)
					//fprintf(calfp,"%d,%d\n",i+1, ifDataBuf[i]);
		
				sysScpi->getDataFromIF();
				
			//}
			mutexIF.unlock();
			//intFlag = 1;
		x = 0;
	}
#endif
}
