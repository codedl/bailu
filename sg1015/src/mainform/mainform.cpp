#include "mainform.h"

#define BAUDRATE B115200
#define KBD_PORT "/dev/ttyS2"

int keyCount;

//构造函数
mainForm::mainForm(QWidget* parent) :
	QWidget(parent = 0)
{
	isPowerOnSelect = false;

	//载入SCPI响应
	sysScpi = new tSysScpi();

	loadSystemSetting();

	//variable
	lcdRefreshTimes = 200;
	helpMessage = "";
	inputTag = "";
	tagValue = "";
	inputValue = "";
	prvKeyValue = 0;

	indexOfPassword = 0;
	memset(passwdString, 0, sizeof passwdString);

	//载入菜单
	sysMenu = new tSysMenu("/home/sky/SG1030.xml");
	nowMainKey = sysKey;
	nowSubKey = nowMainKey->rightNode;

	//  printf("2\n");


	//  printf("3\n");
	connect(sysScpi, SIGNAL(drawFreqAndAmplMain(void)), this, SLOT(drawFreqAndAmplMainSlots(void)));
	connect(sysScpi, SIGNAL(drawState(void)), this, SLOT(drawStateSlots(void)));
	connect(sysScpi, SIGNAL(drawInputParamterNoParamter(void)), this, SLOT(drawInputParamterSlotsNoParamter(void)));
	connect(sysScpi, SIGNAL(drawInputParamter(char*, int)), this, SLOT(drawInputParamterSlots(char *,int)));
	connect(sysScpi, SIGNAL(drawProcess(void)), this, SLOT(drawProcessSlots(void)));
	connect(sysScpi, SIGNAL(drawsListData(void)), this, SLOT(drawsListDataSlots(void)));
	connect(sysScpi, SIGNAL(drawProgBar(bool, float)), this, SLOT(drawProgBarSlots(bool, float)));
	connect(sysScpi, SIGNAL(drawDownloadProg(bool, int, int)), this, SLOT(drawDownloadProgSlots(bool, int, int)));
	connect(sysScpi, SIGNAL(drawAnalogModulate(void)), this, SLOT(drawAnalogModulateSlots(void)));
	connect(sysScpi, SIGNAL(drawDigitalModulate(void)), this, SLOT(drawDigitalModulateSlots(void)));
	connect(sysScpi, SIGNAL(drawPluseModulate(void)), this, SLOT(drawPluseModulateSlots(void)));
	connect(sysScpi, SIGNAL(drawSystemPara(void)), this, SLOT(drawSystemParaSlots(void)));
	//  connect(sysScpi, SIGNAL(drawFreqAndAmpl(void)), this, SLOT(drawFreqAndAmplSlots(void)));
	connect(sysScpi, SIGNAL(drawFreqInParamter(void)), this, SLOT(drawFreqInParamterSlots(void)));
	connect(sysScpi, SIGNAL(drawAmplInParamter(void)), this, SLOT(drawAmplInParamterSlots(void)));
	connect(sysScpi, SIGNAL(drawSweep(int)), this, SLOT(drawSweepSlots(int)));
	connect(sysScpi, SIGNAL(drawExfFref(void)), this, SLOT(drawExfFrefSlots(void)));
	connect(sysScpi, SIGNAL(drawArbitraryWave(void)), this, SLOT(drawArbitraryWaveSlots(void)));
	connect(sysScpi, SIGNAL(drawDrgFunction(void)), this, SLOT(drawDrgFunctionSlots(void)));

	connect(sysScpi, SIGNAL(resetUsbSlaveInterface()), this, SLOT(resetUsbSlaveInterface())); //复位USB从口

	//获取系统图标图片
	getLogoPicture();

	//载入驱动
	loadSystemDriver();

	//初始化键盘板LED指示灯
	sysScpi->initKeyBoardLED();
	sysScpi->keyBoardLEDAllOpen();

	//初始化参数
	sysScpi->presetSystemData();

	//复位网口
	sysScpi->resetSystemIOOfLan();

	//初始化键盘板LED指示灯
	sysScpi->initKeyBoardLED();

	//开机参数
//	sysScpi->loadPowerOffState();

	sysScpi->controlParamterDown();

	//显示启动画面
	//  初始化脉冲信号源信号发生板
	showSplashScreen();

	evaDay = -1;
	evaIndex = 0;
	memset(evaArray, 0x0, sizeof evaArray);

	//默认选中频率
	isPowerOnSelect = true;
	readKeyBoardData();
	isPowerOnSelect = false;

	int i = 0;
	while (true)
	{
		sysScpi->sendSerialData(QString("*IDNN?")); //下发*IDNN?，获取模块的相关信息
		if (sysScpi->moduSerial != QString(""))
		{
			break;
		}
		i++;
		usleep(10 * 1000);
		if (i >= 3)
		{
			sysScpi->moduSerial = SERIALNO;
			break;
		}
	}

}

//绘制SCPI状态
void mainForm::refreshScpiState(QString helpStr)
{
	if (!helpStr.isNull() && !helpStr.isEmpty() && helpStr != helpContent)
	{
		helpContent = helpStr;
		//	drawWin4();
	}
}

void mainForm::drawSelect(QPainter* painter, QPixmap* pixmap, QRect rc, QString str, int index)
{
	//  printf("str = %s\n", str.toStdString().c_str());

	int indexStr = -1;

	QString tempQSitStr = str.trimmed();
	QString qSitStr = str.trimmed();

	indexStr = qSitStr.indexOf('-', 0); //获取‘-’字符在字符串中的位置
	//  printf("index = %d\n",indexStr);

	QRect rcDisplay1; //'.'字符显示位置
	QRect rcDisplay2[16]; //' '字符显示位置
	QRect rcDisplay3[16]; //除'.'和' '外的数字字符显示位置
	QRect rcDisplay4;

	int length = tempQSitStr.remove('.').remove('-').remove(' ').length();
	if (index >= length)
	{
		for (int i = 0; i < (index - length + 1); i++)
		{
			//		qSitStr.insert(0,'0');				//原来的处理方法

			if (indexStr != -1) //zpc add,去除当第一个按下的是“-”号时，会显示“0-”的bug
			{
				if (index == 0 && length == 0) //当第一个按下的是“-”号时，会显示“0-”
				{
					;
				} else
				{
					qSitStr.insert(1, '0');
				}

			} else
			{
				qSitStr.insert(0, '0');
			}

		}
	}

	char freqCH[15];
	strcpy(freqCH, qSitStr.toLatin1());
	int lengthIT = qSitStr.length();

	int j = 0, k = 0;
	for (int i = 0; i < lengthIT; i++)
	{
		painter->setPen(Qt::black);
		if (freqCH[lengthIT - i - 1] == '.')
		{
			rcDisplay1 = QRect(rc.x() + (14 - i) * rc.width() / 15, rc.y(), rc.width() / 15, rc.height());
			painter->drawText(rcDisplay1, Qt::AlignHCenter | Qt::AlignVCenter, QString(freqCH[lengthIT - i - 1]));
		}

		if (freqCH[lengthIT - i - 1] == ' ')
		{
			rcDisplay2[j] = QRect(rc.x() + i * rc.width() / 15, rc.y(), rc.width() / 15, rc.height());
			painter->drawText(rcDisplay2[j], Qt::AlignHCenter | Qt::AlignVCenter, QString(freqCH[lengthIT - i - 1]));
			j++;
		}
	}

	int i;
	for (i = 0; i < lengthIT; i++)
	{
		if ((freqCH[lengthIT - i - 1] != '.') && (freqCH[lengthIT - i - 1] != ' '))
		{
			rcDisplay3[k] = QRect(rc.x() + (14 - i) * rc.width() / 15, rc.y(), rc.width() / 15, rc.height());
			if (index == k)
			{
				painter->setPen(Qt::white);
				painter->fillRect(rcDisplay3[k], Qt::black);
				painter->drawText(rcDisplay3[k], Qt::AlignHCenter | Qt::AlignVCenter, QString(freqCH[lengthIT - i - 1]));
			} else
			{
				painter->setPen(Qt::black);
				painter->fillRect(rcDisplay3[k], Qt::white);
				painter->drawText(rcDisplay3[k], Qt::AlignHCenter | Qt::AlignVCenter, QString(freqCH[lengthIT - i - 1]));
			}
			k++;
		}
	}

	//  if(indexStr >= 0)
	//  {
	//	rcDisplay4 = QRect(rc.x() + (14 - i) * rc.width() / 15, rc.y(), rc.width() / 15, rc.height());
	//	painter->setPen(Qt::black);
	//	painter->fillRect(rcDisplay4,Qt::white);
	//	painter->drawText(rcDisplay4, Qt::AlignHCenter | Qt::AlignVCenter, QString('-'));
	//  }

	painter->setPen(Qt::black);
}

/* 在输入栏显示频率值
 * 使用左右键和旋钮改变频率值*/
void mainForm::drawFreq(QPainter* painter, QPixmap* pixmap, QRect rc, double freq, int index)
{
	int tempindex;
	if (freq < 0)
		tempindex = -1;
	else
		tempindex = 1;

	freq *= tempindex;

	QString freqQS = doubleToQValue(freq);
	QString freqQSIT = insertThousandsQS(freqQS);

	QRect rcDisplay1; //'.'字符显示位置
	QRect rcDisplay2[16]; //' '字符显示位置
	QRect rcDisplay3[16]; //除'.'和' '外的数字字符显示位置
	QRect rcDisplay4;

	int length = freqQS.remove('.').remove('-').length();

	if (index >= length)
	{
		for (int i = 0; i < (index - length + 1); i++)
		{
			freqQSIT.insert(0, '0');
		}
	}

	char freqCH[15];
	strcpy(freqCH, freqQSIT.toLatin1());
	int lengthIT = freqQSIT.length();

	int j = 0, k = 0;
	for (int i = 0; i < lengthIT; i++)
	{
		painter->setPen(Qt::black);
		if (freqCH[lengthIT - i - 1] == '.')
		{
			rcDisplay1 = QRect(rc.x() + (14 - i) * rc.width() / 15, rc.y(), rc.width() / 15, rc.height());
			painter->drawText(rcDisplay1, Qt::AlignHCenter | Qt::AlignVCenter, QString(freqCH[lengthIT - i - 1]));
		}

		if (freqCH[lengthIT - i - 1] == ' ')
		{
			rcDisplay2[j] = QRect(rc.x() + i * rc.width() / 15, rc.y(), rc.width() / 15, rc.height());
			painter->drawText(rcDisplay2[j], Qt::AlignHCenter | Qt::AlignVCenter, QString(freqCH[lengthIT - i - 1]));
			j++;
		}
	}

	int i;
	for (i = 0; i < lengthIT; i++)
	{
		if ((freqCH[lengthIT - i - 1] != '.') && (freqCH[lengthIT - i - 1] != ' '))
		{
			rcDisplay3[k] = QRect(rc.x() + (14 - i) * rc.width() / 15, rc.y(), rc.width() / 15, rc.height());
			if (index == k)
			{
				painter->setPen(Qt::white);
				painter->fillRect(rcDisplay3[k], Qt::black);
				painter->drawText(rcDisplay3[k], Qt::AlignHCenter | Qt::AlignVCenter, QString(freqCH[lengthIT - i - 1]));
			} else
			{
				painter->setPen(Qt::black);
				painter->fillRect(rcDisplay3[k], Qt::white);
				painter->drawText(rcDisplay3[k], Qt::AlignHCenter | Qt::AlignVCenter, QString(freqCH[lengthIT - i - 1]));
			}
			k++;
		}
	}

	if (tempindex == -1)
	{
		rcDisplay4 = QRect(rc.x() + (14 - i) * rc.width() / 15, rc.y(), rc.width() / 15, rc.height());
		painter->setPen(Qt::black);
		painter->fillRect(rcDisplay4, Qt::white);
		painter->drawText(rcDisplay4, Qt::AlignHCenter | Qt::AlignVCenter, QString('-'));
	}

	painter->setPen(Qt::black);

}

/* 在输入栏显示点数
 * 使用左右键和旋钮改变频率值*/
void mainForm::drawPoint(QPainter* painter, QPixmap* pixmap, QRect rc, double freq, int index)
{
	int tempindex;
	if (freq < 0)
		tempindex = -1;
	else
		tempindex = 1;

	freq *= tempindex;

	QString freqQS;
	freqQS.sprintf("%.0f", freq);
	QString freqQSIT = insertThousandsQS(freqQS);

	QRect rcDisplay1; //'.'字符显示位置
	QRect rcDisplay2[16]; //' '字符显示位置
	QRect rcDisplay3[16]; //除'.'和' '外的数字字符显示位置
	QRect rcDisplay4;

	int length = freqQS.remove('.').length();
	if (index >= length)
	{
		for (int i = 0; i < (index - length + 1); i++)
		{
			freqQSIT.insert(0, '0');
		}
	}

	char freqCH[15];
	strcpy(freqCH, freqQSIT.toLatin1());
	int lengthIT = freqQSIT.length();

	int j = 0, k = 0;
	for (int i = 0; i < lengthIT; i++)
	{
		painter->setPen(Qt::black);
		if (freqCH[lengthIT - i - 1] == '.')
		{
			rcDisplay1 = QRect(rc.x() + (14 - i) * rc.width() / 15, rc.y(), rc.width() / 15, rc.height());
			painter->drawText(rcDisplay1, Qt::AlignHCenter | Qt::AlignVCenter, QString(freqCH[lengthIT - i - 1]));
		}

		if (freqCH[lengthIT - i - 1] == ' ')
		{
			rcDisplay2[j] = QRect(rc.x() + i * rc.width() / 15, rc.y(), rc.width() / 15, rc.height());
			painter->drawText(rcDisplay2[j], Qt::AlignHCenter | Qt::AlignVCenter, QString(freqCH[lengthIT - i - 1]));
			j++;
		}
	}

	int i;
	for (i = 0; i < lengthIT; i++)
	{
		if ((freqCH[lengthIT - i - 1] != '.') && (freqCH[lengthIT - i - 1] != ' '))
		{
			rcDisplay3[k] = QRect(rc.x() + (14 - i) * rc.width() / 15, rc.y(), rc.width() / 15, rc.height());
			if (index == k)
			{
				painter->setPen(Qt::white);
				painter->fillRect(rcDisplay3[k], Qt::black);
				painter->drawText(rcDisplay3[k], Qt::AlignHCenter | Qt::AlignVCenter, QString(freqCH[lengthIT - i - 1]));
			} else
			{
				painter->setPen(Qt::black);
				painter->fillRect(rcDisplay3[k], Qt::white);
				painter->drawText(rcDisplay3[k], Qt::AlignHCenter | Qt::AlignVCenter, QString(freqCH[lengthIT - i - 1]));
			}

			k++;
		}
	}

	if (tempindex == -1)
	{
		rcDisplay4 = QRect(rc.x() + (14 - i) * rc.width() / 15, rc.y(), rc.width() / 15, rc.height());
		painter->setPen(Qt::black);
		painter->fillRect(rcDisplay4, Qt::white);
		painter->drawText(rcDisplay4, Qt::AlignHCenter | Qt::AlignVCenter, QString('-'));
	}

	painter->setPen(Qt::black);
}

/* 在输入栏显示功率值
 * 使用左右键和旋钮改变功率值*/
void mainForm::drawAmpl(QPainter* painter, QPixmap* pixmap, QRect rc, float ampl, int index)
{
	int tempindex;
	if (ampl < 0)
		tempindex = -1;
	else
		tempindex = 1;

	ampl *= tempindex;

	QString freqQS;
	ampl = int(ampl * 1000) / 1000.00;
	freqQS.sprintf("%.2f", ampl);
	QString freqQSIT = insertThousandsQS(freqQS);

	QRect rcDisplay1; //'.'字符显示位置
	QRect rcDisplay2[16]; //' '字符显示位置
	QRect rcDisplay3[16]; //除'.'和' '外的数字字符显示位置
	QRect rcDisplay4;

	int length = freqQS.remove('.').length();
	if (index >= length)
	{
		for (int i = 0; i < (index - length + 1); i++)
		{
			freqQSIT.insert(0, '0');
		}
	}

	char freqCH[15];
	strcpy(freqCH, freqQSIT.toLatin1());
	int lengthIT = freqQSIT.length();

	int j = 0, k = 0;
	for (int i = 0; i < lengthIT; i++)
	{
		painter->setPen(Qt::black);
		if (freqCH[lengthIT - i - 1] == '.')
		{
			rcDisplay1 = QRect(rc.x() + (14 - i) * rc.width() / 15, rc.y(), rc.width() / 15, rc.height());
			painter->drawText(rcDisplay1, Qt::AlignHCenter | Qt::AlignVCenter, QString(freqCH[lengthIT - i - 1]));
		}

		if (freqCH[lengthIT - i - 1] == ' ')
		{
			rcDisplay2[j] = QRect(rc.x() + i * rc.width() / 15, rc.y(), rc.width() / 15, rc.height());
			painter->drawText(rcDisplay2[j], Qt::AlignHCenter | Qt::AlignVCenter, QString(freqCH[lengthIT - i - 1]));
			j++;
		}
	}

	int i;
	for (i = 0; i < lengthIT; i++)
	{
		if ((freqCH[lengthIT - i - 1] != '.') && (freqCH[lengthIT - i - 1] != ' '))
		{
			rcDisplay3[k] = QRect(rc.x() + (14 - i) * rc.width() / 15, rc.y(), rc.width() / 15, rc.height());

			if (index == k)
			{
				painter->setPen(Qt::white);
				painter->fillRect(rcDisplay3[k], Qt::black);
				painter->drawText(rcDisplay3[k], Qt::AlignHCenter | Qt::AlignVCenter, QString(freqCH[lengthIT - i - 1]));
			} else
			{
				painter->setPen(Qt::black);
				painter->fillRect(rcDisplay3[k], Qt::white);
				painter->drawText(rcDisplay3[k], Qt::AlignHCenter | Qt::AlignVCenter, QString(freqCH[lengthIT - i - 1]));
			}

			k++;
		}
	}

	if ((tempindex == -1) && (ampl != 0.00))
	{
		rcDisplay4 = QRect(rc.x() + (14 - i) * rc.width() / 15, rc.y(), rc.width() / 15, rc.height());
		painter->setPen(Qt::black);
		painter->fillRect(rcDisplay4, Qt::white);
		painter->drawText(rcDisplay4, Qt::AlignHCenter | Qt::AlignVCenter, QString('-'));
	}

	painter->setPen(Qt::black);
}

//绘制主参数显示区
void mainForm::drawFreqAndAmplMain(void)
{
	if (sysScpi->isUpdating)
	{
		return;
	}

	//------------------------------------------------内存图像绘制------------------------------------------------------------
	QRect rc = QRect(WIN1X + 2 * BORDERWIDTH, WIN1Y, WIN1WIDTH - 2 * BORDERWIDTH, WIN1HEIGHT); //窗口1全部区域
	QRect rcPara = QRect(0, 0, rc.width() - BORDERWIDTH, rc.height() - BORDERWIDTH); //窗口1参数显示区域
	QPainter p;
	QPixmap pix(rc.size());

	if (!pix.isNull())
	{
		p.begin(&pix);
		p.fillRect(pix.rect(), BACKCOLORWIN1); //填充窗口1全部区域的背景色
		p.fillRect(rcPara, BACKCOLORALLWIN); //填充窗口1参数显示区域的背景色

		QString tempv = doubleToQValue(sysData.freq.freq);
		QString tempu = freqToQUnit(sysData.freq.freq);
		char tempChar[64] = { };

		//-------------------------------------频率参数内存图像绘制-------------------------------------------------
		QRect rcWin1_1 = QRect(rcPara.x(), rcPara.y(), rcPara.width() * WIN1_1WIDTH - BORDERWIDTH, rcPara.height()); //频率参数显示位置
		p.setPen(Qt::white);
		p.drawRect(rcWin1_1);
		p.setPen(Qt::black);

		//	p.setFont(fontWin1);
		QRect rcWin1_1_1 = QRect(rcWin1_1.x(), rcWin1_1.y(), rcWin1_1.width(), rcWin1_1.height() / 2); //频率状态显示位置

		QRect rcWin1_1_1_1 = QRect(rcWin1_1_1.x(), rcWin1_1_1.y(), rcWin1_1_1.width() * 0.3, rcWin1_1_1.height()); //频率名称显示位置

		QRect rcWin1_1_1_2 = QRect(rcWin1_1_1.x() + rcWin1_1_1.width() * 0.3 + 40, rcWin1_1_1.y() + 2, rcWin1_1_1.width() * 0.3 - 40, rcWin1_1_1.height() - 4); //频率REF显示位置

		QRect rcWin1_1_1_3 = QRect(rcWin1_1_1.x() + rcWin1_1_1.width() * 0.6 + 20, rcWin1_1_1.y() + 2, rcWin1_1_1.width() * 0.3 - 20, rcWin1_1_1.height() - 4); //频率OFFSET显示位置

		p.drawText(rcWin1_1_1_1, Qt::AlignLeft | Qt::AlignVCenter, QString(" FREQUENCY"));

		if (sysData.freq.ref.state == true)
		{
			if (sysData.freq.ref.value > 0x00)
			{
				p.fillRect(rcWin1_1_1_2, BACKCOLORWIN2);
				p.setPen(Qt::white);
				p.drawText(rcWin1_1_1_2, Qt::AlignCenter, getTheTranslatorString(QString("REF")));
			}
		}

		if (sysData.freq.offset > 0x00)
		{
			p.fillRect(rcWin1_1_1_3, BACKCOLORWIN2);
			p.setPen(Qt::white);
			p.drawText(rcWin1_1_1_3, Qt::AlignCenter, getTheTranslatorString(QString("OFFSET")));
		}

		if (sysData.sweep.mode == smAmpl || sysData.sweep.mode == smOff) //当扫描模式是“关”或“扫幅”时，频率显示黑色，否则灰度处理
		{
			p.setPen(Qt::black);
		} else
		{
			p.setPen(Qt::gray);
		}

		//	p.setPen(Qt::black);
		p.setFont(fontWin1En);
		QRect rcWin1_1_2 = QRect(rcWin1_1.x() + rcWin1_1.width() * WIN1_1_1WIDTH, rcWin1_1.y() + rcWin1_1.height() / 2, rcWin1_1.width() * WIN1_1_2WIDTH, rcWin1_1.height() / 2); //频率值显示位置

		strcpy(tempChar, doubleToQValue(sysData.freq.freq).toStdString().c_str());
		p.drawText(rcWin1_1_2, Qt::AlignRight | Qt::AlignVCenter, insertThousandsCH(tempChar).trimmed());

		QRect rcWin1_1_3 = QRect(rcWin1_1.x() + rcWin1_1.width() * (WIN1_1_1WIDTH + WIN1_1_2WIDTH), rcWin1_1.y() + rcWin1_1.height() / 2, rcWin1_1.width() * WIN1_1_3WIDTH, rcWin1_1.height() / 2); //频率单位显示位置
		p.drawText(rcWin1_1_3, Qt::AlignHCenter | Qt::AlignVCenter, freqToQUnit(sysData.freq.freq));

		//------------------------------------------时钟2频率参数内存图像绘制-------------------------------------------------
		QRect rcWin2_1 = QRect(rcPara.x() + rcPara.width() * WIN1_1WIDTH, rcPara.y(), rcPara.width() * WIN1_2WIDTH + 1, rcPara.height()); //频率参数显示位置
		p.setPen(Qt::white);
		p.drawRect(rcWin2_1);
		p.setPen(Qt::black);

		//	p.setFont(fontWin1);
		p.setFont(QFont("enl", 13, 30, false));
		QRect rcWin2_1_1 = QRect(rcWin2_1.x(), rcWin2_1.y(), rcWin2_1.width(), rcWin2_1.height() / 2); //幅度显示位置

		QRect rcWin2_1_1_1 = QRect(rcWin2_1_1.x(), rcWin2_1_1.y(), rcWin2_1_1.width() * 0.45, rcWin2_1_1.height()); //幅度名称显示位置

		QRect rcWin2_1_1_2 = QRect(rcWin2_1_1.x() + rcWin2_1_1.width() * 0.45, rcWin2_1_1.y() + 2, rcWin2_1_1.width() * 0.20, rcWin2_1_1.height() - 4); //幅度REF显示位置

		QRect rcWin2_1_1_3 = QRect(rcWin2_1_1.x() + rcWin2_1_1.width() * 0.7, rcWin2_1_1.y() + 2, rcWin2_1_1.width() * 0.3, rcWin2_1_1.height() - 4); //幅度OFFSET显示位置

		p.drawText(rcWin2_1_1_1, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(QString(" AMPLITUDE")));

		if (sysData.ampl.ref.state)
		{
			if (sysData.ampl.ref.value >= -120)
			{
				p.fillRect(rcWin2_1_1_2, BACKCOLORWIN2);
				p.setPen(Qt::white);
				p.drawText(rcWin2_1_1_2, Qt::AlignCenter, getTheTranslatorString(QString("REF")));
			}
		}

		if (sysData.ampl.offset > 0x00)
		{
			p.fillRect(rcWin2_1_1_3, BACKCOLORWIN2);
			p.setPen(Qt::white);
			p.drawText(rcWin2_1_1_3, Qt::AlignCenter, getTheTranslatorString(QString("OFFSET")));
		}

		//	p.setPen(Qt::black);
		if (sysData.keyBoardSwitchRF == true) //当RF状态是开的时候，当扫描模式是“关”或“扫频”时，幅度显示黑色，否则灰度处理
		{
			if (sysData.sweep.mode == smOff || sysData.sweep.mode == smFreq)
			{
				p.setPen(Qt::black);
			} else
			{
				p.setPen(Qt::gray);
			}
		} else //RF关闭时，需要灰度处理
		{
			p.setPen(Qt::gray);
		}
		p.setFont(fontWin1En);
		QRect rcWin2_1_2 = QRect(rcWin2_1.x() + rcWin2_1.width() * WIN1_2_1WIDTH - 5, rcWin2_1.y() + rcWin1_1.height() / 2, rcWin2_1.width() * WIN1_2_2WIDTH, rcWin2_1.height() / 2); //幅度值显示位置

		sprintf(tempChar, "%.02f", sysData.ampl.ampl);

		p.drawText(rcWin2_1_2, Qt::AlignRight | Qt::AlignVCenter, insertThousandsCH(tempChar).trimmed());

		QRect rcWin2_1_3 = QRect(rcWin2_1.x() + rcWin2_1.width() * (WIN1_2_1WIDTH + WIN1_2_2WIDTH) - 5, rcWin2_1.y() + rcWin1_1.height() / 2, rcWin2_1.width() * WIN1_2_3WIDTH + 5, rcWin2_1.height() / 2);

		//频率单位显示位置
		/*	if(sysData.ampl.ref.state)
		 {
		 p.drawText(rcWin2_1_3, Qt::AlignHCenter | Qt::AlignVCenter, "dB");
		 }else
		 {
		 p.drawText(rcWin2_1_3, Qt::AlignHCenter | Qt::AlignVCenter, "dBm");
		 }
		 */
		if (sysData.ampl.ref.state)
		{
			p.drawText(rcWin2_1_3, Qt::AlignHCenter | Qt::AlignVCenter, "dB");
		} else
		{
			switch (sysData.ampl.amplUnit)
			{
				case audBm:
					p.drawText(rcWin2_1_3, Qt::AlignHCenter | Qt::AlignVCenter, "dBm");
					break;
				case audBuV:
					p.drawText(rcWin2_1_3, Qt::AlignHCenter | Qt::AlignVCenter, "dBuV");
					break;
				case aumV:
					p.drawText(rcWin2_1_3, Qt::AlignHCenter | Qt::AlignVCenter, "mV");
					break;
				default:
					break;
			}
		}
		p.setPen(Qt::black);

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

void mainForm::drawFreqAndAmplMainSlots(void)
{
	drawFreqAndAmplMain();
}

#if 0
//原本的绘制方法
//绘制分隔栏显示区
void mainForm::drawState(void)
{

	QString temp_qstring;
	if (sysScpi->isUpdating)
	{
		return;
	}

	//---------------------------------------------内存图像绘制---------------------------------------------
	QRect rc = QRect(WIN2X + 2 * BORDERWIDTH, WIN2Y, WIN2WIDTH - 2 * BORDERWIDTH, WIN2HEIGHT); //窗口2全部区域
	QRect rcRemoteState;// = QRect(pix.rect().x() + 5, rc.top(), rc.width()/2, rc.height());
	QRect rcRfState;// = QRect(rc.right() - 15, rc.top(), 10, rc.height());
	QRect rcModeState;// = QRect(rcRfState.left() - 15, rcRfState.top(), 10, rcRfState.height());
	QRect rcLfState; //= QRect(rcModeState.left() - 15, rcModeState.top(), 10, rcModeState.height());
	QRect rcALCState;

	QPainter p;
	QPixmap pix(rc.size());

	if (!pix.isNull())
	{
		p.begin(&pix);

		rcRemoteState = QRect(pix.rect().x() + 5, pix.rect().y(), 26, pix.height());
		rcRfState = QRect(pix.rect().right() - 28 - 5, pix.rect().y() + 2, 26, pix.height() - 4);
		rcLfState = QRect(rcRfState.left() - rcRfState.width() - 2 - 5, rcRfState.top(), rcRfState.width(), rcRfState.height());
		rcModeState = QRect(rcLfState.left() - rcLfState.width() - 8 - 5, rcLfState.top(), rcLfState.width() + 8, rcLfState.height());
		rcALCState = QRect(rcModeState.left() - rcModeState.width() -8 , rcModeState.top(),rcModeState.width() , rcModeState.height());
		//	printf("%d,%d,%d, %d\n", rcRemoteState.left(), rcRemoteState.top(), rcRemoteState.width(),rcRemoteState.height());

		p.fillRect(pix.rect(), BACKCOLORWIN2); //填充窗口2全部区域的背景色

		//远程控制
		//	printf("stateR = %d\n",sysData.scpiData.stateR);
		if(sysData.scpiData.stateR)
		{
			//      p.fillRect(rcRemoteState, Qt::black);
			p.setPen(Qt::blue);
			p.setFont(fontWin2En);
			p.drawText(rcRemoteState, Qt::AlignLeft | Qt::AlignHCenter, "R");
		}

		p.setPen(Qt::black);

		//mode On off
		p.setFont(fontData);

		if(sysData.keyBoardSwitchMod)
		{
			p.drawRect(rcModeState);
			p.fillRect(rcModeState.left() + 2, rcModeState.top() + 2, rcModeState.width() - 3, rcModeState.height() - 3, QColor(181,230,29));
			p.drawText(rcModeState, Qt::AlignCenter, "MOD");
		}

		//LF On Off
		p.setFont(fontData);

		if(sysData.keyBoardSwitchLF)
		{
			p.drawRect(rcLfState);
			p.fillRect(rcLfState.left() + 2, rcLfState.top() + 2, rcLfState.width() - 3, rcLfState.height() - 3, QColor(181,230,29));
			p.drawText(rcLfState, Qt::AlignCenter, "LF");
		}

		//RF on/Off
		p.setFont(fontData);
		if(sysData.keyBoardSwitchRF)
		{
			p.drawRect(rcRfState);
			p.fillRect(rcRfState.left() + 2, rcRfState.top() + 2, rcRfState.width() - 3, rcRfState.height() - 3, QColor(181,230,29));
			p.drawText(rcRfState, Qt::AlignCenter, "RF");
		}

		//ALC ON/OFF
		p.setFont(fontData);
		if(sysData.ampl.alc.state)
		{
			p.drawRect(rcALCState);
			p.fillRect(rcALCState.left() + 2, rcALCState.top() + 2, rcALCState.width() - 3, rcALCState.height() -3, QColor(181,230,29));
			p.drawText(rcALCState, Qt::AlignCenter, "ALC");
		}

		p.end();

		if (sysStart)
		{
			p.begin(imgStart);
			p.drawPixmap(QPoint(rc.left(), rc.top()), pix);
			p.end();
		}
		else
		{
			QScreen::instance()->blit(pix.toImage(), QPoint(rc.left(), rc.top()), rc);
		}
	}

}
#endif

//绘制分隔栏显示区
void mainForm::drawState(void)
{

	QString temp_qstring;
	if (sysScpi->isUpdating)
	{
		return;
	}

	if (sysScpi->eva.isEvaluate && evaDay != QDate::currentDate().day()) //判断是否在试用期，若在试用期计算是否超过使用时间
	{
		evaDay = QDate::currentDate().day();
		sysScpi->getSystemServiceOfEvaluteSYDays();
	}

	if (!sysScpi->eva.isEvaluate || (sysScpi->eva.isEvaluate && !sysScpi->eva.evaluated)) //在试用期且未超过使用时间，或是在正常状态
	{
		//---------------------------------------------内存图像绘制---------------------------------------------
		QRect rc = QRect(WIN2X + 2 * BORDERWIDTH, WIN2Y, WIN2WIDTH - 2 * BORDERWIDTH, WIN2HEIGHT); //窗口2全部区域
		QRect rcRemoteState;// = QRect(pix.rect().x() + 5, rc.top(), rc.width()/2, rc.height());
		QRect rcRfState;// = QRect(rc.right() - 15, rc.top(), 10, rc.height());
		QRect rcModeState;// = QRect(rcRfState.left() - 15, rcRfState.top(), 10, rcRfState.height());
		QRect rcLfState; //= QRect(rcModeState.left() - 15, rcModeState.top(), 10, rcModeState.height());
		QRect rcALCState;

		QPainter p;
		QPixmap pix(rc.size());
		if (!pix.isNull())
		{
			p.begin(&pix);

			rcRemoteState = QRect(pix.rect().x() + 5, pix.rect().y(), 26, pix.height());
			rcRfState = QRect(pix.rect().right() - 28 - 5, pix.rect().y() + 2, 26, pix.height() - 4);
			rcLfState = QRect(rcRfState.left() - rcRfState.width() - 2 - 5, rcRfState.top(), rcRfState.width(), rcRfState.height());
			rcModeState = QRect(rcLfState.left() - rcLfState.width() - 8 - 5, rcLfState.top(), rcLfState.width() + 8, rcLfState.height());
			rcALCState = QRect(rcModeState.left() - rcModeState.width() - 8, rcModeState.top(), rcModeState.width(), rcModeState.height());
			//	printf("%d,%d,%d, %d\n", rcRemoteState.left(), rcRemoteState.top(), rcRemoteState.width(),rcRemoteState.height());

			p.fillRect(pix.rect(), BACKCOLORWIN2); //填充窗口2全部区域的背景色

			//远程控制
			//	printf("stateR = %d\n",sysData.scpiData.stateR);
			if (sysData.scpiData.stateR)
			{
				//      p.fillRect(rcRemoteState, Qt::black);
				p.setPen(Qt::blue);
				p.setFont(fontWin2En);
				p.drawText(rcRemoteState, Qt::AlignLeft | Qt::AlignHCenter, "R");
			}

			p.setPen(Qt::black);

			//mode On off
			p.setFont(fontData);

			if (sysData.keyBoardSwitchMod)
			{
				p.drawRect(rcModeState);
				p.fillRect(rcModeState.left() + 2, rcModeState.top() + 2, rcModeState.width() - 3, rcModeState.height() - 3, QColor(181, 230, 29));
				p.drawText(rcModeState, Qt::AlignCenter, "MOD");
			}

			//LF On Off
			p.setFont(fontData);

			if (sysData.keyBoardSwitchLF)
			{
				p.drawRect(rcLfState);
				p.fillRect(rcLfState.left() + 2, rcLfState.top() + 2, rcLfState.width() - 3, rcLfState.height() - 3, QColor(181, 230, 29));
				p.drawText(rcLfState, Qt::AlignCenter, "LF");
			}

			//RF on/Off
			p.setFont(fontData);
			if (sysData.keyBoardSwitchRF)
			{
				p.drawRect(rcRfState);
				p.fillRect(rcRfState.left() + 2, rcRfState.top() + 2, rcRfState.width() - 3, rcRfState.height() - 3, QColor(181, 230, 29));
				p.drawText(rcRfState, Qt::AlignCenter, "RF");
			}

			//ALC ON/OFF
			p.setFont(fontData);
			if (sysData.ampl.alc.state)
			{
				p.drawRect(rcALCState);
				p.fillRect(rcALCState.left() + 2, rcALCState.top() + 2, rcALCState.width() - 3, rcALCState.height() - 3, QColor(181, 230, 29));
				p.drawText(rcALCState, Qt::AlignCenter, "ALC");
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
	} else //超过试用期
	{

		int evaNums = sizeof evaArray / sizeof evaArray[0];
		int evaWidth = 30;
		int evaHeight = 30;
		QRect rc[evaNums];
		QVector<QRect> rcS;
		char tempChar[32] = { 0x0 };
		QRect rcPoint = QRect(0, 0, ALLWIDTH + 2, ALLHEIGHT); //整个背景
		QRect rcEnter = QRect(2, ALLHEIGHT - 34, 74, 30); //确认键
		QPainter p;
		//	QPainter pEnter;
		QPixmap pix(rcPoint.size()); //整个背景
		//	QPixmap pixEnter(rcEnter.size());


		p.begin(&pix);
		p.setPen(Qt::black);
		p.fillRect(rcPoint, QColor(10, 10, 10)); //填充整个屏幕背景色

		p.setPen(Qt::white);
		rc[0] = QRect(rcPoint.left() + rcPoint.width() / 2 - 3.5 * evaWidth, rcPoint.top() + rcPoint.height() / 2 - evaHeight / 2, evaWidth, evaHeight); //先确定rc[0]的坐标，后面的方框坐标是根据rc[0]的计算出来的
		rcS << rc[0];
		p.drawText(rc[0], Qt::AlignCenter, QString(intToHex(evaArray[0], tempChar)).trimmed());

		for (int i = 1; i < evaNums; i++) //计算第二个到第八个小方框的位置
		{
			rc[i] = QRect(rc[i - 1].left() + rc[i - 1].width(), rc[i - 1].top(), rc[i - 1].width(), rc[i - 1].height());
			rcS << rc[i];

			p.drawText(rc[i], Qt::AlignCenter, QString(intToHex(evaArray[i], tempChar)).trimmed());
		}

		rcS << rcEnter;
		p.drawText(rcEnter, Qt::AlignCenter, QString("Enter"));

		p.setPen(Qt::white);
		p.drawRects(rcS);

		p.drawText(QRect(rc[0].left(), rc[0].top() - evaHeight, rc[0].width() * (evaNums + 2), evaHeight), Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("Please input RegCode") + "("
				+ QString(sysScpi->moduSerial).trimmed() + ")");

		for (int i = 0; i < evaNums; i++)
		{
			if (i == evaIndex) //当前的输入框显示为绿色
			{
				p.setPen(Qt::green);
				p.drawRect(rc[i]);
				break;
			}
		}
		p.end();
		if (sysStart)
		{
			p.begin(imgStart);
			p.drawPixmap(QPoint(rcPoint.left(), rcPoint.top()), pix);
			p.end();
		} else
		{
			QScreen::instance()->blit(pix.toImage(), QPoint(rcPoint.left(), rcPoint.top()), rcPoint);
		}

		//绘制确认键
		//	pEnter.begin(&pixEnter);
		//
		//	pEnter.setPen(Qt::black);
		//	pEnter.fillRect(rcEnter,QColor(10,10,10));		//填充整个屏幕背景色
		////	pEnter.setPen(Qt::white);
		////	pEnter.drawRect(rcEnterFont);
		////	pEnter.drawText(rcEnterFont, Qt::AlignCenter, QString("Enter"));
		//
		//	pEnter.end();
		//
		//	if (sysStart)
		//	{
		//		pEnter.begin(imgStart);
		//		pEnter.drawPixmap(QPoint(rcEnter.left(), rcEnter.top()), pixEnter);
		//		pEnter.end();
		//	}
		//	else
		//	{
		//	  QScreen::instance()->blit(pixEnter.toImage(), QPoint(rcEnter.left(), rcEnter.top()), rcEnter);
		//	}

	}

}

void mainForm::drawStateSlots(void)
{
	drawState();
}

//绘制输入栏显示区
void mainForm::drawInputParamter(char * str, int funcCode)
{

	QString temp_qstring;
	if (sysScpi->isUpdating)
	{
		return;
	}

	if (funcCode == KEY_MAIN_PRESET)
	{
		return;
	}

	//---------------------------------------------内存图像绘制---------------------------------------------
	//窗口3全部区域
	QRect rc = QRect(WIN3X + 2 * BORDERWIDTH, WIN3Y, WIN3WIDTH - 2 * BORDERWIDTH, WIN3HEIGHT);

	//窗口3参数显示区域
	QRect rcInput = QRect(20, BORDERWIDTH, rc.width() - 20, rc.height() - 2 * BORDERWIDTH);

	//参数名称显示区域
	QRect rcName = QRect(rcInput.x(), rcInput.y(), rcInput.width() * WIN3_1WIDTH, rcInput.height());

	//参数值显示区域
	QRect rcValue = QRect(rcInput.x() + rcInput.width() * WIN3_1WIDTH, rcInput.y(), rcInput.width() * WIN3_2WIDTH, rcInput.height());

	//参数单位显示区域
	QRect rcUnit = QRect(rcInput.x() + rcInput.width() * (WIN3_1WIDTH + WIN3_2WIDTH), rcInput.y(), rcInput.width() * WIN3_3WIDTH, rcInput.height());

	//开始绘制
	QPainter p;
	QPixmap pix(rc.size());
	if (!pix.isNull())
	{
		p.begin(&pix);

		p.fillRect(pix.rect(), BACKCOLORWIN3); //填充窗口2全部区域的背景色

		p.setPen(QColor(0, 0, 0));

		p.setFont(fontWin2En);

		//draw name
		p.drawText(rcName, Qt::AlignLeft | Qt::AlignVCenter, sysData.name);

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

void mainForm::drawInputParamterSlots(char * str, int funcCode)
{
	drawInputParamter(str, funcCode);
}

void mainForm::drawInputParamterNoParamter(void)
{

	if (sysScpi->isUpdating)
	{
		return;
	}

	QRect rc = QRect(WIN3X + 2 * BORDERWIDTH, WIN3Y, WIN3WIDTH - 2 * BORDERWIDTH, WIN3HEIGHT);

	//窗口3参数显示区域
	QRect rcInput = QRect(20, BORDERWIDTH, rc.width() - 20, rc.height() - 2 * BORDERWIDTH);

	//参数名称显示区域
	QRect rcName = QRect(rcInput.x(), rcInput.y(), rcInput.width() * WIN3_1WIDTH, rcInput.height());

	//参数值显示区域
	QRect rcValue = QRect(rcInput.x() + rcInput.width() * WIN3_1WIDTH, rcInput.y(), rcInput.width() * WIN3_2WIDTH, rcInput.height());

	//参数单位显示区域
	QRect rcUnit = QRect(rcInput.x() + rcInput.width() * (WIN3_1WIDTH + WIN3_2WIDTH), rcInput.y(), rcInput.width() * WIN3_3WIDTH, rcInput.height());
	QPainter p;
	QPixmap pix(rc.size());
	QString displayString, tempValue;
	char tempChar[32];

	if (!pix.isNull())
	{
		p.begin(&pix);
		p.fillRect(pix.rect(), BACKCOLORWIN3);
		p.setPen(QColor(0, 0, 0));

		p.setFont(fontWin2En);

		//draw name
		p.drawText(rcName, Qt::AlignLeft | Qt::AlignVCenter, QString(sysData.name).trimmed());

		//draw value
		//    switch(sysData.drawType)
		//    {
		//      case dt
		//    }

		//    p.drawText(rcValue, Qt::AlignRight | Qt::AlignVCenter, QString(sysData.value).trimmed());
		if (!QString(sysData.value).isEmpty())
		{
			drawSelect(&p, &pix, rcValue, QString(sysData.value).trimmed(), sysData.cursorLocation);
			//	  tempValue = QString(sysData.value);
			//	  insertThousandsQS(tempValue);
		}

		//draw unit
		/*
		 if(nowMainKey->keyValue == KEY_MAIN_AMPL )
		 {
		 //		if(nowSubKey->funcCode != KEY_FUNC_AMPL_DBM && nowSubKey->funcCode != KEY_FUNC_AMPLOFFSET_DBM )
		 {
		 switch(sysData.ampl.amplUnit)
		 {
		 case audBm:   displayString = "dBm";    break;
		 case audBuV:  displayString = "dBuV";   break;
		 case aumV:    displayString = "mV";     break;
		 default:      displayString = "";		break;
		 }
		 p.drawText(rcUnit, Qt::AlignCenter, displayString.trimmed());
		 }

		 }else*/
		{
			p.drawText(rcUnit, Qt::AlignCenter, QString(sysData.unit).trimmed());
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

void mainForm::drawInputParamterSlotsNoParamter(void)
{
	drawInputParamterNoParamter();
}

/********************************************************************/

//绘制
void mainForm::drawExfFref(void)
{
	if (sysScpi->isUpdating)
	{
		return;
	}
	//  if (!sysData.display.isFullScreen)
	{
		QRect rc = QRect(WIN4X + 2 * BORDERWIDTH, WIN4Y, WIN4WIDTH - 2 * BORDERWIDTH - 2 * WIN4X, WIN4HEIGHT);
		QRect rcTemp[2][16];
		QRect rcTitle[2]; //标题栏分为两列
		QPainter p;
		QPixmap pix(rc.size());
		int row = 6;
		char tempChar[64] = { };
		QString tempQchar;

		if (!pix.isNull())
		{
			p.begin(&pix);

			//计算绘制空间
			QRect rcTemp1 = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width(), pix.rect().height());
			QRect rcPara = QRect(pix.rect().x() + 30, pix.rect().y() + pix.rect().height() / 4, pix.rect().width(), pix.rect().height() / row * 4);
			rcTitle[0] = QRect(pix.rect().x() - 2, pix.rect().y(), pix.rect().width() + 2 - 60, pix.rect().height() / row); //标题区0
			rcTitle[1] = QRect((pix.rect().x() + pix.rect().width() + 2 - 60) - 2, pix.rect().y(), 60, pix.rect().height() / row); //标题区

			p.fillRect(rcTemp1, Qt::white);
			p.fillRect(rcTitle[0], BACKCOLORWIN4);
			p.fillRect(rcTitle[1], BACKCOLORWIN4);

			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 2; j++)
				{
					rcTemp[j][i] = QRect(rcPara.x() + j * (rcPara.width() - 30) / 2, rcPara.y() + i * rcPara.height() / 4, rcPara.width() / 2, rcPara.height() / 4);
				}
			}
			p.setPen(Qt::black);
			QString displayString, QtempV, QtempU;

			//绘制标题栏
			if (sysData.system.isLanguageEn)
			{
				p.setFont(fontEnglishBold);
			} else
			{
				p.setFont(fontChinesePara);
			}
			displayString = "Exf Ref Information";
			p.drawRect(rcTitle[0]);
			p.drawText(rcTitle[0].x() + 20, rcTitle[0].y(), rcTitle[0].width(), rcTitle[0].height(), Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayString));

			p.setFont(fontEnglishBold);
			displayString = "1/1";
			p.drawRect(rcTitle[1]);
			p.drawText(rcTitle[1].x() + 20, rcTitle[1].y(), rcTitle[1].width(), rcTitle[1].height(), Qt::AlignLeft | Qt::AlignVCenter, displayString);

			//绘制频率参考类型
			if (sysData.system.isLanguageEn)
			{
				p.setFont(fontEnglishBold);
			} else
			{
				p.setFont(fontChinesePara);
			}

			if (sysData.exfRef.type == erftInternal)
			{
				displayString = "IF Source :\tInternal";
			} else if (sysData.exfRef.type == erftExternal)
			{
				displayString = "IF Source :\tExternal";
			}
			p.drawText(rcTemp[0][0], Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayString));

			//绘制频率参考
			if (sysData.exfRef.type == erftInternal)
			{
				p.setPen(Qt::gray);
			} else if (sysData.exfRef.type == erftExternal)
			{
				p.setPen(Qt::black);
			}

			QtempV = doubleToQValue(sysData.exfRef.freq);
			QtempV = insertThousandsQS(QtempV);
			QtempU = freqToQUnit(sysData.exfRef.freq);
			displayString = getTheTranslatorString("Ext IF Freq") + ":\t" + QtempV + QtempU;
			p.drawText(rcTemp[0][1], Qt::AlignLeft | Qt::AlignVCenter, displayString);

			//绘制从底层传上来的幅度
			if (sysData.exfRef.type == erftExternal) //当变频源是外部的时候才进行解析传上来的参数
			{
				p.setFont(QFont("enb", 16, 50, false));
				displayString = getTheTranslatorString("Ext IF Ampl") + ":\t";
				//	    strcpy(tempChar, (const char *)sysData.scpiData.buf);
				tempQchar = sysScpi->getQvalueFromModule();
				displayString = displayString + tempQchar.trimmed() + " dBm";
				//	    displayString = displayString + QString(tempChar) + "dBm";
				p.drawText(rcTemp[0][2], Qt::AlignLeft | Qt::AlignVCenter, displayString);
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

void mainForm::drawExfFrefSlots(void)
{
	drawExfFref();
}

/********************************************************************/
/*
 //绘制频率参数
 void mainForm::drawFreqAndAmpl(void)
 {
 if (sysScpi->isUpdating)
 {
 return;
 }

 //	if (!sysData.display.isFullScreen)
 QRect rc = QRect(WIN4X + 2 * BORDERWIDTH, WIN4Y, WIN4WIDTH - 2 * BORDERWIDTH - 2 * WIN4X, WIN4HEIGHT);
 QRect rcTemp[2][16];
 QPainter p;
 QPixmap pix(rc.size());
 int row = 7;

 if (!pix.isNull())
 {
 p.begin(&pix);

 //计算绘制空间
 QRect rcTemp1 = QRect(pix.rect().x(),pix.rect().y(),pix.rect().width(), pix.rect().height());  //整个参数区（标题和参数）
 QRect rcPara = QRect(pix.rect().x() + 60,pix.rect().y() + pix.rect().height() / row,pix.rect().width(), pix.rect().height() - pix.rect().height() / row);  //参数区
 QRect rcTitle = QRect(pix.rect().x() - 2,pix.rect().y(),pix.rect().width() + 2, pix.rect().height() / 9);  //标题区

 p.fillRect(rcTemp1, Qt::white);
 p.fillRect(rcTitle, BACKCOLORWIN4);

 for(int i = 0; i < row; i++)
 {
 for(int j = 0; j < 2; j++)
 {
 rcTemp[j][i] = QRect(rcPara.x() + j * rcPara.width() / 2, rcPara.y() + i * rcPara.height() / row, rcPara.width() / 2, rcPara.height() / row);
 }
 }

 p.setPen(Qt::black);
 QString displayString, QtempV, QtempU, tempValue;

 //绘制标题栏
 if (sysData.system.isLanguageEn)
 {
 p.setFont(fontEnglishBold);
 }else
 {
 p.setFont(fontChinese);
 }
 displayString = "Frequency/Amplitude Status Information";
 p.drawRect(rcTitle);
 //p.drawText(rcTitle.x() + 20, rcTitle.y(), rcTitle.width(), rcTitle.height(), Qt::AlignLeft | Qt::AlignVCenter, displayString);
 p.drawText(rcTitle.x() + 20, rcTitle.y(), rcTitle.width(), rcTitle.height(), Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayString));//zpc changed

 //绘制频率参考状态
 p.setFont(fontEnglishLight);

 if(sysData.freq.ref.state)
 {
 displayString = "Freq Ref State:\tOn";
 }else
 {
 displayString = "Freq Ref State:\tOff";
 }

 p.drawText(rcTemp[0][1], Qt::AlignLeft | Qt::AlignVCenter, displayString);

 //绘制频率参考值
 if(sysData.freq.ref.state)	p.setPen(Qt::black);
 else	p.setPen(Qt::gray);

 QtempV = doubleToQValue(sysData.freq.ref.value);
 QtempV = insertThousandsQS(QtempV);
 QtempU = freqToQUnit(sysData.freq.ref.value);
 displayString = "Freq Ref:\t" + QtempV + "\t" + QtempU;
 p.drawText(rcTemp[0][2], Qt::AlignLeft | Qt::AlignVCenter, displayString);

 p.setPen(Qt::black);

 //绘制频率偏移值
 QtempV = doubleToQValue(sysData.freq.offset);
 QtempV = insertThousandsQS(QtempV);
 QtempU = freqToQUnit(sysData.freq.offset);
 displayString = "Freq Offset:\t" + QtempV + "\t" + QtempU;
 p.drawText(rcTemp[0][3], Qt::AlignLeft | Qt::AlignVCenter, displayString);

 //绘制频率倍数值
 QtempV = doubleToQValue(sysData.freq.multiplier);
 QtempV = insertThousandsQS(QtempV);
 QtempU = "x";
 displayString = "Multiplier:\t" + QtempV + "\t" + QtempU;
 p.drawText(rcTemp[0][4], Qt::AlignLeft | Qt::AlignVCenter, displayString);

 //绘制内外参考
 switch(sysData.freq.ref10MHz)
 {
 case rmInternal: tempValue = "Internal"; break;
 case rmExternal: tempValue = "External"; break;
 default:break;
 }
 displayString = "Ref 10MHz:\t" + tempValue;
 p.drawText(rcTemp[0][5], Qt::AlignLeft | Qt::AlignVCenter, displayString);

 //绘制幅度参考状态
 if(sysData.ampl.ref.state)
 {
 displayString = "Ampl Ref State:\tOn";
 }else
 {
 displayString = "Ampl Ref State:\tOff";
 }

 p.drawText(rcTemp[1][1], Qt::AlignLeft | Qt::AlignVCenter, displayString);

 //绘制幅度参考值
 if(sysData.ampl.ref.state)	p.setPen(Qt::black);
 else	p.setPen(Qt::gray);
 QtempV.sprintf("%.2f", sysData.ampl.ref.value);

 if(sysData.ampl.ref.state)
 {
 QtempU = "dB";
 }else
 {
 QtempU = "dBm";;
 }

 displayString = "Ampl Ref:\t" + QtempV + "\t" + QtempU;
 p.drawText(rcTemp[1][2], Qt::AlignLeft | Qt::AlignVCenter, displayString);
 p.setPen(Qt::black);

 //绘制幅度偏移值
 QtempV.sprintf("%.2f", sysData.ampl.offset);
 QtempU = "dB";
 displayString = "Ampl Offset:\t" + QtempV + "\t" + QtempU;
 p.drawText(rcTemp[1][3], Qt::AlignLeft | Qt::AlignVCenter, displayString);

 p.setPen(Qt::black);
 //绘制幅度偏移值
 QtempV.sprintf("%.2f", sysData.ampl.error.value);
 QtempU = "dB";
 displayString = "Ampl Error:\t" + QtempV + "\t" + QtempU;
 p.drawText(rcTemp[1][4], Qt::AlignLeft | Qt::AlignVCenter, displayString);

 //
 p.end();

 if (sysStart)
 {
 p.begin(imgStart);
 p.drawPixmap(QPoint(rc.left(), rc.top()), pix);
 p.end();
 }
 else
 {
 QScreen::instance()->blit(pix.toImage(), QPoint(rc.left(), rc.top()), rc);
 }
 }
 }
 */

/********************************************************************/
//绘制频率参数在参数显示区
void mainForm::drawFreqInParamter(void)
{
	if (sysScpi->isUpdating)
	{
		return;
	}

	//if (!sysData.display.isFullScreen)
	QRect rc = QRect(WIN4X + 2 * BORDERWIDTH, WIN4Y, WIN4WIDTH - 2 * BORDERWIDTH - 2 * WIN4X, WIN4HEIGHT);
	QRect rcTemp[2][16];
	QRect rcTitle[2]; //标题栏分为两列
	QPainter p;
	QPixmap pix(rc.size());
	int row = 6;

	if (!pix.isNull())
	{
		p.begin(&pix);

		//计算绘制空间
		QRect rcTemp1 = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width(), pix.rect().height()); //整个参数区（标题和参数）
		QRect rcPara = QRect(pix.rect().x() + 30, pix.rect().y() + pix.rect().height() / 4, pix.rect().width(), pix.rect().height() / row * 4); //参数区
		rcTitle[0] = QRect(pix.rect().x() - 2, pix.rect().y(), pix.rect().width() + 2 - 60, pix.rect().height() / row); //标题区0
		rcTitle[1] = QRect((pix.rect().x() + pix.rect().width() + 2 - 60) - 2, pix.rect().y(), 60, pix.rect().height() / row); //标题区

		p.fillRect(rcTemp1, Qt::white);
		p.fillRect(rcTitle[0], BACKCOLORWIN4);
		p.fillRect(rcTitle[1], BACKCOLORWIN4);

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				rcTemp[j][i] = QRect(rcPara.x() + j * (rcPara.width() - 30) / 2, rcPara.y() + i * rcPara.height() / 4, rcPara.width() / 2, rcPara.height() / 4);
			}
		}

		p.setPen(Qt::black);
		QString displayString, QtempV, QtempU, tempValue;

		//绘制标题栏
		if (sysData.system.isLanguageEn)
		{
			p.setFont(fontEnglishBold);
		} else
		{
			p.setFont(fontChinesePara);
		}
		displayString = "Frequency Status Information";
		p.drawRect(rcTitle[0]);
		//p.drawText(rcTitle.x() + 20, rcTitle.y(), rcTitle.width(), rcTitle.height(), Qt::AlignLeft | Qt::AlignVCenter, displayString);
		p.drawText(rcTitle[0].x() + 20, rcTitle[0].y(), rcTitle[0].width(), rcTitle[0].height(), Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayString));//zpc changed

		p.setFont(fontEnglishBold);
		displayString = "1/1";
		p.drawRect(rcTitle[1]);
		p.drawText(rcTitle[1].x() + 20, rcTitle[1].y(), rcTitle[1].width(), rcTitle[1].height(), Qt::AlignLeft | Qt::AlignVCenter, displayString);

		//绘制频率参考状态
		//	p.setFont(fontEnglishLight);
		if (sysData.system.isLanguageEn)
		{
			p.setFont(fontEnglishBold);
		} else
		{
			p.setFont(fontChinesePara);
		}

		if (sysData.freq.ref.state)
		{
			displayString = "Freq Ref State:\tOn";
		} else
		{
			displayString = "Freq Ref State:\tOff";
		}

		p.drawText(rcTemp[0][0], Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayString));

		//绘制频率参考值
		if (sysData.freq.ref.state)
			p.setPen(Qt::black); //根据状态判断画笔是否灰色处理
		else
			p.setPen(Qt::gray);

		QtempV = doubleToQValue(sysData.freq.ref.value);
		QtempV = insertThousandsQS(QtempV);
		QtempU = freqToQUnit(sysData.freq.ref.value);
		if (sysData.system.isLanguageEn)
		{
			displayString = "Freq Ref:\t" + QtempV + "\t" + QtempU;
		} else
		{
			displayString = "频率参考:\t" + QtempV + "\t" + QtempU;
		}
		p.drawText(rcTemp[1][1], Qt::AlignLeft | Qt::AlignVCenter, displayString);

		p.setPen(Qt::black);

		//绘制频率偏移值
		QtempV = doubleToQValue(sysData.freq.offset);
		QtempV = insertThousandsQS(QtempV);
		QtempU = freqToQUnit(sysData.freq.offset);
		if (sysData.system.isLanguageEn)
		{
			displayString = "Freq Offset:\t" + QtempV + "\t" + QtempU;
		} else
		{
			displayString = "频率偏置:\t" + QtempV + "\t" + QtempU;
		}
		p.drawText(rcTemp[0][1], Qt::AlignLeft | Qt::AlignVCenter, displayString);

		//绘制频率倍数值
		QtempV = doubleToQValue(sysData.freq.multiplier);
		QtempV = insertThousandsQS(QtempV);
		QtempU = "x";
		if (sysData.system.isLanguageEn)
		{
			displayString = "Multiplier:\t" + QtempV + "\t" + QtempU;
		} else
		{
			displayString = "倍频:\t" + QtempV + "\t" + QtempU;
		}
		p.drawText(rcTemp[1][0], Qt::AlignLeft | Qt::AlignVCenter, displayString);

		//绘制内外参考
		switch (sysData.freq.ref10MHz)
		{
			case rmInternal:
				tempValue = "Internal";
				break;
			case rmExternal:
				tempValue = "External";
				break;
			default:
				break;
		}
		displayString = "Ref 10MHz:\t" + tempValue;
		p.drawText(rcTemp[0][2], Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayString));

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

void mainForm::drawFreqInParamterSlots(void)
{
	drawFreqInParamter();
}

/********************************************************************/
//绘制幅度参数在参数显示区
void mainForm::drawAmplInParamter(void)
{
	if (sysScpi->isUpdating)
	{
		return;
	}

	//if (!sysData.display.isFullScreen)
	QRect rc = QRect(WIN4X + 2 * BORDERWIDTH, WIN4Y, WIN4WIDTH - 2 * BORDERWIDTH - 2 * WIN4X, WIN4HEIGHT);
	QRect rcTemp[2][16];
	QRect rcTitle[2]; //标题栏分为两列
	QPainter p;
	QPixmap pix(rc.size());
	int row = 6;

	if (!pix.isNull())
	{
		p.begin(&pix);

		//计算绘制空间
		QRect rcTemp1 = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width(), pix.rect().height()); //整个参数区（标题和参数）
		QRect rcPara = QRect(pix.rect().x() + 30, pix.rect().y() + pix.rect().height() / 4, pix.rect().width(), pix.rect().height() / row * 4); //参数区
		rcTitle[0] = QRect(pix.rect().x() - 2, pix.rect().y(), pix.rect().width() + 2 - 60, pix.rect().height() / row); //标题区0
		rcTitle[1] = QRect((pix.rect().x() + pix.rect().width() + 2 - 60) - 2, pix.rect().y(), 60, pix.rect().height() / row); //标题区

		p.fillRect(rcTemp1, Qt::white);
		p.fillRect(rcTitle[0], BACKCOLORWIN4);
		p.fillRect(rcTitle[1], BACKCOLORWIN4);

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				rcTemp[j][i] = QRect(rcPara.x() + j * (rcPara.width() - 30) / 2, rcPara.y() + i * rcPara.height() / 4, rcPara.width() / 2, rcPara.height() / 4);
			}
		}

		p.setPen(Qt::black);
		QString displayString, QtempV, QtempU, tempValue;

		//绘制标题栏
		if (sysData.system.isLanguageEn)
		{
			p.setFont(fontEnglishBold);
		} else
		{
			p.setFont(fontChinesePara);
		}
		displayString = "Amplitude Status Information";
		p.drawRect(rcTitle[0]);
		//p.drawText(rcTitle.x() + 20, rcTitle.y(), rcTitle.width(), rcTitle.height(), Qt::AlignLeft | Qt::AlignVCenter, displayString);
		p.drawText(rcTitle[0].x() + 20, rcTitle[0].y(), rcTitle[0].width(), rcTitle[0].height(), Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayString));

		p.setFont(fontEnglishBold);
		displayString = "1/1";
		p.drawRect(rcTitle[1]);
		p.drawText(rcTitle[1].x() + 20, rcTitle[1].y(), rcTitle[1].width(), rcTitle[1].height(), Qt::AlignLeft | Qt::AlignVCenter, displayString);

		if (sysData.system.isLanguageEn)
		{
			p.setFont(fontEnglishBold);
		} else
		{
			p.setFont(fontChinesePara);
		}
		//绘制稳幅状态
		if (sysData.ampl.alc.state)
		{
			displayString = "Ampl ALC State:\tOn";
		} else
		{
			displayString = "Ampl ALC State:\tOff";
		}
		p.drawText(rcTemp[0][0], Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayString));

		//绘制稳幅响应
		if (sysData.ampl.alc.state)
			p.setPen(Qt::black); //根据状态判断画笔是否灰色处理
		else
			p.setPen(Qt::gray);
		if (sysData.ampl.alc.response)
		{
			displayString = "Ampl ALC Response:\tFast";
		} else
		{
			displayString = "Ampl ALC Response:\tSlow";
		}
		p.drawText(rcTemp[1][0], Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayString));
		//绘制幅度参考状态
		p.setPen(Qt::black);
		if (sysData.ampl.ref.state)
		{
			displayString = "Ampl Ref State:\tOn";
		} else
		{
			displayString = "Ampl Ref State:\tOff";
		}

		p.drawText(rcTemp[1][1], Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayString));

		//绘制幅度参考值
		if (sysData.ampl.ref.state)
			p.setPen(Qt::black);
		else
			p.setPen(Qt::gray);
		QtempV.sprintf("%.2f", sysData.ampl.ref.value);

		//	if(sysData.ampl.ref.state)
		//	{
		//	  QtempU = "dB";
		//	}else
		{
			QtempU = "dBm";
			;
		}
		if (sysData.system.isLanguageEn)
		{
			displayString = "Ampl Ref:\t" + QtempV + "\t" + QtempU;
		} else
		{
			displayString = "幅度参考值：" + QtempV + "\t" + QtempU;
		}

		p.drawText(rcTemp[0][1], Qt::AlignLeft | Qt::AlignVCenter, displayString);
		p.setPen(Qt::black);

		//绘制幅度偏移值
		QtempV.sprintf("%.2f", sysData.ampl.offset);
		QtempU = "dB";
		if (sysData.system.isLanguageEn)
		{
			displayString = "Ampl Offset:\t" + QtempV + "\t" + QtempU;
		} else
		{
			displayString = "幅度偏置值：" + QtempV + "\t" + QtempU;
		}
		p.drawText(rcTemp[0][2], Qt::AlignLeft | Qt::AlignVCenter, displayString);

		//绘制缆损补偿
		QtempV.sprintf("%.2f", sysData.ampl.error.value);
		QtempU = "dB";
		if (sysData.system.isLanguageEn)
		{
			displayString = "I.L Cable:\t" + QtempV + "\t" + QtempU;
		} else
		{
			displayString = "缆损补偿:\t" + QtempV + "\t" + QtempU;
		}
		p.drawText(rcTemp[1][2], Qt::AlignLeft | Qt::AlignVCenter, displayString);

		p.setPen(Qt::black);

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

void mainForm::drawAmplInParamterSlots(void)
{
	drawAmplInParamter();
}

/********************************************************************/
//绘制频率和幅度参数在参数显示区
/*
 void mainForm::drawFreqAndAmpl(void)
 {
 if (sysScpi->isUpdating)
 {
 return;
 }
 if(nowMainKey->keyValue == KEY_MAIN_FREQ)
 {
 drawFreqInParamter();
 }
 if(nowMainKey->keyValue == KEY_MAIN_AMPL)
 {
 drawAmplInParamter();
 }
 }
 */

//void mainForm::drawFreqAndAmplSlots(void)
//{
//  drawFreqAndAmpl();
//}

//绘制 process
void mainForm::drawProcess(void)
{

	if (sysScpi->isUpdating)
	{
		return;
	}

	//  QRect rc = QRect(297, 113 + 75, 120, 28);
	QRect rc = QRect(333, 113 + 87, 120, 28);
	//  QRect rc = QRect(356, 43 + 83, 120, 28);


	int sep = 1;
	char tempChar[32] = { };
	QPainter p;
	QPixmap pix(rc.size());

	int progress = sysData.process;
	//  progress = 50;

	QRect rcTitle = QRect(sep, sep, pix.width() - 2 * sep, (pix.height() - 2 * sep) / 2);
	QRect rcProgress = QRect(rcTitle.left(), rcTitle.top() + rcTitle.height(), rcTitle.width(), rcTitle.height());
	QRect rcPosition = QRect(rcProgress.left() + 1, rcProgress.top() + 1, (rcProgress.width() - 2) * progress / 100, rcProgress.height() - 2);

	p.begin(&pix);
	p.fillRect(pix.rect(), BACKGROUNDCOLOR);

	//  if (normal == 0)
	//  {
	//    p.setPen(Qt::black);
	//  } else
	//  {
	//	p.setPen(Qt::red);
	//  }

	p.setPen(Qt::black);

	p.setFont(fontEnglishLight);
	p.drawText(rcTitle, Qt::AlignLeft | Qt::AlignVCenter, "Process");
	p.fillRect(rcProgress, Qt::white);
	p.fillRect(rcPosition, Qt::green);
	p.setPen(Qt::black);
	p.drawRect(rcProgress);
	p.end();

	QScreen::instance()->blit(pix.toImage(), QPoint(rc.left(), rc.top()), rc);
}

//绘制扫描参数区
void mainForm::drawSweep(int type)
{
	if (sysScpi->isUpdating)
	{
		return;
	}

	QString tempQChar;

	QRect rc = QRect(WIN4X + 2 * BORDERWIDTH, WIN4Y, WIN4WIDTH - 2 * BORDERWIDTH - 2 * WIN4X, WIN4HEIGHT);
	QRect rcTemp[2][16];
	QRect rcTitle[2]; //标题栏分为两列
	QPainter p;
	QPixmap pix(rc.size());
	int row = 6;

	if (!pix.isNull())
	{
		p.begin(&pix);

		//计算绘制空间
		QRect rcTemp1 = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width(), pix.rect().height()); //整个参数区（标题和参数）
		QRect rcPara = QRect(pix.rect().x() + 30, pix.rect().y() + pix.rect().height() / 4, pix.rect().width(), pix.rect().height() / row * 4); //参数区
		rcTitle[0] = QRect(pix.rect().x() - 2, pix.rect().y(), pix.rect().width() + 2 - 60, pix.rect().height() / row); //标题区0
		rcTitle[1] = QRect((pix.rect().x() + pix.rect().width() + 2 - 60) - 2, pix.rect().y(), 60, pix.rect().height() / row); //标题区
		p.fillRect(rcTitle[0], BACKCOLORWIN4);
		p.fillRect(rcTitle[1], BACKCOLORWIN4);
		p.fillRect(rcTemp1, Qt::white);

		for (int i = 0; i < 6; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				rcTemp[j][i] = QRect(rcPara.x() + j * (rcPara.width() - 30) / 2, rcPara.y() + i * rcPara.height() / 6, rcPara.width() / 2, rcPara.height() / 6);
			}
		}

		p.setPen(Qt::black);

		QString displayString, QtempV, QtempU, tempValue;

		//绘制标题栏
		if (sysData.system.isLanguageEn)
		{
			p.setFont(fontEnglishBold);
		} else
		{
			p.setFont(fontChinesePara);
		}
		displayString = "Sweep/List Status Information";
		p.drawRect(rcTitle[0]);
		p.drawText(rcTitle[0].x() + 20, rcTitle[0].y(), rcTitle[0].width(), rcTitle[0].height(), Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayString));

		p.setFont(fontEnglishBold);
		//	tempValue = doubleToQValue(sysData.sweep.nowpagenum);  //将数字转换成QT字符串
		//	displayString = tempValue + "/2";
		displayString = "1/1";
		p.drawRect(rcTitle[1]);
		p.drawText(rcTitle[1].x() + 20, rcTitle[1].y(), rcTitle[1].width(), rcTitle[1].height(), Qt::AlignLeft | Qt::AlignVCenter, displayString);

		//    if(sysData.sweep.nowpagenum == 1)  //当前页数是第一页时
		{
			//绘制扫描模式
			if (sysData.system.isLanguageEn)
			{
				p.setFont(fontEnglishBold);
			} else
			{
				p.setFont(fontChinesePara);
			}

			QString tempSweepMode = "";

			switch (sysData.sweep.mode)
			{
				case smOff:
					tempSweepMode = "Off";
					break;
				case smFreq:
					tempSweepMode = "Freq";
					break;
				case smAmpl:
					tempSweepMode = "Ampl";
					break;
				case smFreqAmpl:
					tempSweepMode = "Freq&Ampl";
					break;
				default:
					break;
			}
			displayString = getTheTranslatorString("Sweep:\t") + getTheTranslatorString(tempSweepMode);
			p.drawText(rcTemp[0][0], Qt::AlignLeft | Qt::AlignVCenter, displayString);

			//绘制扫描类型
			p.setPen(Qt::black);
			if (sysData.sweep.type == stList)
			{
				displayString = "Type:\vList";
			} else if (sysData.sweep.type == stStep)
			{
				displayString = "Type:\vStep";
				;
			}
			p.drawText(rcTemp[1][0], Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayString));

			if (sysData.sweep.type == 0) //根据扫描类型分别绘制，当扫描类型是步进时
			{
				//绘制扫描起始频率
				if (sysData.sweep.mode == smFreq || sysData.sweep.mode == smFreqAmpl)
					p.setPen(Qt::black);
				else
					p.setPen(Qt::gray);

				QtempV = doubleToQValue(sysData.sweep.step.freqStart);
				QtempV = insertThousandsQS(QtempV);
				QtempU = freqToQUnit(sysData.sweep.step.freqStart);
				tempValue = "Freq Start:";
				displayString = getTheTranslatorString(tempValue) + "\t" + QtempV + "\t" + QtempU;
				p.drawText(rcTemp[0][1], Qt::AlignLeft | Qt::AlignVCenter, displayString);

				//绘制扫描终止频率
				if (sysData.sweep.mode == smFreq || sysData.sweep.mode == smFreqAmpl)
					p.setPen(Qt::black);
				else
					p.setPen(Qt::gray);

				QtempV = doubleToQValue(sysData.sweep.step.freqStop);
				QtempV = insertThousandsQS(QtempV);
				QtempU = freqToQUnit(sysData.sweep.step.freqStop);
				tempValue = "Freq Stop:";
				displayString = getTheTranslatorString(tempValue) + "\t" + QtempV + "\t" + QtempU;
				p.drawText(rcTemp[0][2], Qt::AlignLeft | Qt::AlignVCenter, displayString);

				//绘制扫描起始幅度
				if (sysData.sweep.mode == smAmpl || sysData.sweep.mode == smFreqAmpl)
					p.setPen(Qt::black);
				else
					p.setPen(Qt::gray);

				QtempV.sprintf("%.2f", sysData.sweep.step.amplStart);
				QtempV = insertThousandsQS(QtempV);
				QtempU = amplToQUnit(sysData.sweep.step.amplStart);
				tempValue = "Ampl Start:";
				displayString = getTheTranslatorString(tempValue) + "\t" + QtempV + "\t" + QtempU;
				p.drawText(rcTemp[1][1], Qt::AlignLeft | Qt::AlignVCenter, displayString);

				//绘制扫描终止幅度
				if (sysData.sweep.mode == smAmpl || sysData.sweep.mode == smFreqAmpl)
					p.setPen(Qt::black);
				else
					p.setPen(Qt::gray);

				QtempV.sprintf("%.2f", sysData.sweep.step.amplStop);
				QtempV = insertThousandsQS(QtempV);
				QtempU = amplToQUnit(sysData.sweep.step.amplStop);
				tempValue = "Ampl Stop:";
				displayString = getTheTranslatorString(tempValue) + "\t" + QtempV + "\t" + QtempU;
				p.drawText(rcTemp[1][2], Qt::AlignLeft | Qt::AlignVCenter, displayString);

				//绘制扫描驻留时间
				p.setPen(Qt::black);
				QtempV = doubleToQValue(sysData.sweep.step.dwell);
				QtempV = insertThousandsQS(QtempV);
				QtempU = timeToQUnit(sysData.sweep.step.dwell);
				tempValue = "Dwell";
				displayString = getTheTranslatorString(tempValue) + ":" + "\t" + QtempV + "\t" + QtempU;
				p.drawText(rcTemp[0][5], Qt::AlignLeft | Qt::AlignVCenter, displayString);

				//绘制扫描扫描点数
				p.setPen(Qt::black);
				//		  QtempV = doubleToQValue(sysData.sweep.step.point);
				QtempV = tempQChar.sprintf("%d", sysData.sweep.step.point);
				//		  QtempV = insertThousandsQS(QtempV);
				tempValue = "Points";
				displayString = getTheTranslatorString(tempValue) + ":" + "\t" + QtempV;
				p.drawText(rcTemp[1][5], Qt::AlignLeft | Qt::AlignVCenter, displayString);

			} else if (sysData.sweep.type == 1) //当扫描类型是列表时
			{
				//绘制列表扫描点数
				p.setPen(Qt::black);
				QtempV = doubleToQValue(sysData.sweep.list.allRow);
				//		  QtempV = insertThousandsQS(QtempV);
				tempValue = "Points";
				displayString = getTheTranslatorString(tempValue) + ":\v" + QtempV;
				p.drawText(rcTemp[0][5], Qt::AlignLeft | Qt::AlignVCenter, displayString);

				//绘制扫描点数
				//		  displayString = "Points:\v" + QString(QString::number(sysData.sweep.step.point));
				//		  p.drawText(rcTemp[1][3], Qt::AlignLeft | Qt::AlignVCenter, displayString);
			}
		}
		//    else if (sysData.sweep.nowpagenum == 2)  //当前页数是第二页
		{
			if (sysData.system.isLanguageEn)
			{
				p.setFont(fontEnglishBold);
			} else
			{
				p.setFont(fontChinesePara);
			}

			//绘制扫描折返
			if (sysData.sweep.retrace == srOn)
			{
				displayString = "Retrace:\vOn";
			} else if (sysData.sweep.retrace == srOff)
			{
				displayString = "Retrace:\vOff";
				;
			}
			p.drawText(rcTemp[0][3], Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayString));

			//绘制扫描连续
			if (sysData.sweep.repeat == srSingle)
			{
				displayString = "Repeat:\vSingle";
			} else if (sysData.sweep.repeat == srConutinuos)
			{
				displayString = "Repeat:\vContinuous";
				;
			}
			p.drawText(rcTemp[1][3], Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayString));

			//绘制扫描触发
			if (sysData.sweep.trig == stAuto)
			{
				displayString = "Trigger:\vAuto"; //自动扫描
			} else if (sysData.sweep.trig == stExt)
			{
				displayString = "Trigger:\vstExt"; //外部触发
			}
			p.drawText(rcTemp[0][4], Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayString));

			//绘制扫描方向
			if (sysData.sweep.direction == sdUp)
			{
				displayString = "Direction:\vUp";
			} else if (sysData.sweep.direction == sdDown)
			{
				displayString = "Direction:\vDown";
			}
			p.drawText(rcTemp[1][4], Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayString));

		}
#if 0
		//绘制 扫描状态
		if(sysData.sweep.state)
		{
			QRect pRect = QRect(430, 110, 30, 30);
			p.fillRect(pRect, QColor(90,95,226)); //填充窗口2全部区域的背景色

			if((sysData.sweep.direction) == sdUp && (sysData.sweep.repeat == srSingle))
			{
				p.drawPixmap(pRect, img_Right);
			} else if((sysData.sweep.direction) == sdDown && sysData.sweep.repeat == srSingle)
			{
				p.drawPixmap(pRect, img_Left);
			} else if(sysData.sweep.retrace == srOn &&
					sysData.sweep.direction == sdUp &&
					sysData.sweep.repeat == srConutinuos)
			{
				p.drawPixmap(pRect, img_RePeatUp);
			} else if(sysData.sweep.retrace == srOn &&
					sysData.sweep.direction == sdDown &&
					sysData.sweep.repeat == srConutinuos)
			{
				p.drawPixmap(pRect, img_RePeatDown);
			} else if(sysData.sweep.retrace == srOff &&
					sysData.sweep.direction == sdUp &&
					sysData.sweep.repeat == srConutinuos)
			{
				p.drawPixmap(pRect, img_LoopUP);
			} else if(sysData.sweep.retrace == srOff &&
					(sysData.sweep.direction) == sdDown &&
					sysData.sweep.repeat == srConutinuos)
			{
				p.drawPixmap(pRect, img_LoopDown);
			}
		}
#endif
		/*
		 //绘制扫描连续
		 if(sysData.sweep.repeat == srSingle)
		 {
		 displayString = "Repeat:\vSingle";
		 }else if(sysData.sweep.repeat == srConutinuos)
		 {
		 displayString = "Repeat:\vContinuous";;
		 }
		 p.drawText(rcTemp[1][2], Qt::AlignLeft | Qt::AlignVCenter, displayString);

		 //绘制扫描折返
		 if(sysData.sweep.retrace == srOn)
		 {
		 displayString = "Retrace:\vOn";
		 }else if(sysData.sweep.retrace == srOff)
		 {
		 displayString = "Retrace:\vOff";;
		 }

		 p.drawText(rcTemp[1][3], Qt::AlignLeft | Qt::AlignVCenter, displayString);

		 //绘制扫描方向
		 if(sysData.sweep.direction == sdUp)
		 {
		 displayString = "Direction:\vUp";
		 }else if(sysData.sweep.direction == sdDown)
		 {
		 displayString = "Direction:\vDown";
		 }

		 p.drawText(rcTemp[1][4], Qt::AlignLeft | Qt::AlignVCenter, displayString);

		 //绘制 扫描状态
		 if(sysData.sweep.mode != smOff)
		 {
		 QRect pRect = QRect(430, 110, 30, 30);
		 p.fillRect(pRect, QColor(90,95,226));	//填充窗口2全部区域的背景色

		 if((sysData.sweep.direction) == sdUp && (sysData.sweep.repeat == srSingle))
		 {
		 p.drawPixmap(pRect, img_Right);
		 }else if((sysData.sweep.direction) == sdDown && sysData.sweep.repeat == srSingle)
		 {
		 p.drawPixmap(pRect, img_Left);
		 }else if(sysData.sweep.retrace == srOn    &&
		 sysData.sweep.direction == sdUp  &&
		 sysData.sweep.repeat == srConutinuos)
		 {
		 p.drawPixmap(pRect, img_RePeatUp);
		 }else if(sysData.sweep.retrace == srOn      &&
		 sysData.sweep.direction == sdDown  &&
		 sysData.sweep.repeat == srConutinuos)
		 {
		 p.drawPixmap(pRect, img_RePeatDown);
		 }else if(sysData.sweep.retrace == srOff  &&
		 sysData.sweep.direction == sdUp &&
		 sysData.sweep.repeat == srConutinuos)
		 {
		 p.drawPixmap(pRect, img_LoopUP);
		 }else if(sysData.sweep.retrace == srOff     &&
		 (sysData.sweep.direction) == sdDown &&
		 sysData.sweep.repeat == srConutinuos)
		 {
		 p.drawPixmap(pRect, img_LoopDown);
		 }
		 }
		 */
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

void mainForm::drawSweepSlots(int type)
{
	drawSweep(type);
}

//绘制列表扫描数据区
void mainForm::drawsListData(void)
{
	if (sysScpi->isUpdating)
	{
		return;
	}

	QRect rc = QRect(WIN4X + 2 * BORDERWIDTH, WIN4Y, WIN4WIDTH - 2 * BORDERWIDTH - 2 * WIN4X, WIN4HEIGHT);
	QRect rcTemp[8][3];
	QRect rcNum[8];
	QRect rcTitle[2];
	QPainter p;
	QPixmap pix(rc.size());
	int rowDisplay = 6; //行
	int lineDisplay = 4; //列

	if (!pix.isNull())
	{
		p.begin(&pix);
		p.fillRect(pix.rect(), BACKCOLORWIN4);

		//计算绘制空间
		QRect rcPara = QRect(pix.rect().x(), pix.rect().y() + pix.rect().height() / rowDisplay, pix.rect().width(), pix.rect().height() - pix.rect().height() / rowDisplay);
		//	QRect rcTitle = QRect(pix.rect().x() - 2,pix.rect().y(),pix.rect().width() + 2, pix.rect().height() / 9);
		rcTitle[0] = QRect(pix.rect().x() - 2, pix.rect().y(), pix.rect().width() + 2 - 60, pix.rect().height() / 6); //标题区0
		rcTitle[1] = QRect((pix.rect().x() + pix.rect().width() + 2 - 60) - 2, pix.rect().y(), 60, pix.rect().height() / 6); //标题区

		p.fillRect(rcTitle[0], BACKCOLORWIN4);
		p.fillRect(rcTitle[1], BACKCOLORWIN4);
		p.fillRect(rcPara, Qt::white);

		for (int i = 0; i < 5; i++)
		{
			rcNum[i] = QRect(rcPara.x() + 30, rcPara.y() + i * rcPara.height() / rowDisplay, 50, rcPara.height() / rowDisplay);
			rcTemp[i][0] = QRect(rcPara.x() + 100, rcPara.y() + i * rcPara.height() / rowDisplay, 140, rcPara.height() / rowDisplay);
			rcTemp[i][1] = QRect(rcPara.x() + 265, rcPara.y() + i * rcPara.height() / rowDisplay, 90, rcPara.height() / rowDisplay);
			rcTemp[i][2] = QRect(rcPara.x() + 360, rcPara.y() + i * rcPara.height() / rowDisplay, 70, rcPara.height() / rowDisplay);
		}

		//	QRect rcPageNum = QRect(rcPara.x() + 420, rcPara.y(), 50, rcPara.height() / rowDisplay);

		QString displayQS, QtempV, QtempU;

		//绘制标题栏
		p.setPen(Qt::black);
		p.setFont(fontEnglishBold);
		displayQS = "List Mode Values";
		p.drawRect(rcTitle[0]);
		p.drawText(rcTitle[0].x() + 20, rcTitle[0].y(), rcTitle[0].width(), rcTitle[0].height(), Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayQS));

		//绘制列表名称
		displayQS = "Num";
		p.drawText(rcNum[0], Qt::AlignCenter, getTheTranslatorString(displayQS));
		displayQS = "Frequency";
		p.drawText(rcTemp[0][0], Qt::AlignCenter, getTheTranslatorString(displayQS));
		displayQS = "Ampl";
		p.drawText(rcTemp[0][1], Qt::AlignCenter, getTheTranslatorString(displayQS));
		displayQS = "Dwell";
		p.drawText(rcTemp[0][2], Qt::AlignCenter, getTheTranslatorString(displayQS));

		//绘制页号
		QtempV.sprintf("%d", sysData.sweep.list.displayPage + 1);
		QtempU.sprintf("%d", sysData.sweep.list.allPage);
		displayQS = QtempV + "/" + QtempU;
		p.drawRect(rcTitle[1]);
		p.drawText(rcTitle[1].x() + 20, rcTitle[1].y(), rcTitle[1].width(), rcTitle[1].height(), Qt::AlignLeft | Qt::AlignVCenter, displayQS);

		p.setFont(fontEnglishLight);

		for (int i = 1; i < 6; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if ((sysData.sweep.list.displayPage * 4 + i) <= sysData.sweep.list.allRow)
				{
					if (j == 0) //绘制编号
					{
						displayQS.sprintf("%d", sysData.sweep.list.displayPage * 4 + i);
						p.drawText(rcNum[i], Qt::AlignCenter, displayQS);
					} else if (j == 1)
					{
						QtempV = doubleToQValue(sysData.sweep.list.data[sysData.sweep.list.displayPage * 4 + i - 1].freq);
						QtempV = insertThousandsQS(QtempV);
						QtempU = freqToQUnit(sysData.sweep.list.data[sysData.sweep.list.displayPage * 4 + i - 1].freq);
						displayQS = (QtempV + QtempU);
						p.drawText(rcTemp[i][0], Qt::AlignCenter, displayQS);
					} else if (j == 2)
					{
						QtempV.sprintf("%0.2f", sysData.sweep.list.data[sysData.sweep.list.displayPage * 4 + i - 1].ampl);
						QtempU = amplToQUnit(sysData.sweep.list.data[sysData.sweep.list.displayPage * 4 + i - 1].ampl);
						displayQS = (QtempV + QtempU);
						p.drawText(rcTemp[i][1], Qt::AlignCenter, displayQS);
					} else if (j == 3)
					{
						QtempV = doubleToQValue(sysData.sweep.list.data[sysData.sweep.list.displayPage * 4 + i - 1].dwell);
						QtempV = insertThousandsQS(QtempV);
						QtempU = timeToQUnit(sysData.sweep.list.data[sysData.sweep.list.displayPage * 4 + i - 1].dwell);
						displayQS = (QtempV + QtempU);
						p.drawText(rcTemp[i][2], Qt::AlignCenter, displayQS);
					}
				}
			}
		}

		int SelectRow = sysData.sweep.list.selectRow % 4;
		int SelectLine = sysData.sweep.list.selectLine;

		p.drawRect(rcTemp[SelectRow + 1][SelectLine]); //	显示选中
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

void mainForm::drawsListDataSlots(void)
{
	drawsListData();
}

//绘制进度条
void mainForm::drawProgBar(bool Display, float RatioProgBar)
{
	if (sysScpi->isUpdating)
	{
		return;
	}

	QRect rc = QRect(WIN4X + 2 * BORDERWIDTH + 300, WIN4Y + 2, 150, 12);
	QPainter p;
	QPixmap pix(rc.size());

	if (!pix.isNull())
	{
		p.begin(&pix);

		QRect rcTitleProBar, rcPara, rcTitle;

		if (Display == true)
		{
			rcTitleProBar = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width() * RatioProgBar, pix.rect().height());
			p.setPen(BACKCOLORWIN2);
			p.drawRect(rcTitleProBar);
			p.fillRect(rcTitleProBar, BACKCOLORWIN2);
		} else
		{
			rcTitleProBar = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width() * RatioProgBar, pix.rect().height());
			p.fillRect(rcTitleProBar, BACKCOLORWIN4);
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

void mainForm::drawProgBarSlots(bool Display, float RatioProgBar)
{
	drawProgBar(Display, RatioProgBar);
}

//绘制下载进度
void mainForm::drawDownloadProg(bool Display, int current, int amount)
{
	if (sysScpi->isUpdating)
	{
		return;
	}

	QRect rc = QRect(WIN4X + 2 * BORDERWIDTH + 150, WIN4Y + 2 + 40, 180, 30);
	QPainter p;
	QPixmap pix(rc.size());

	if (!pix.isNull())
	{
		p.begin(&pix);

		QString TempQS, TempQS1;

		if (Display == true)
		{
			p.setPen(BACKCOLORWIN2);
			p.fillRect(pix.rect(), Qt::gray);
			p.setPen(Qt::black);
			TempQS.sprintf("%d", current);
			TempQS1.sprintf("%d", amount);
			TempQS = "Download: " + TempQS + " / " + TempQS1;
			p.drawText(pix.rect(), Qt::AlignCenter, TempQS);
		} else
		{
			p.fillRect(pix.rect(), BACKCOLORWIN4);
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

void mainForm::drawDownloadProgSlots(bool Display, int current, int amount)
{
	drawDownloadProg(Display, current, amount);
}

//绘制模拟调制参数区
void mainForm::drawAnalogModulate(void)
{
	if (sysScpi->isUpdating)
	{
		return;
	}

	QRect rc = QRect(WIN4X + 2 * BORDERWIDTH, WIN4Y, WIN4WIDTH - 2 * BORDERWIDTH - 2 * WIN4X, WIN4HEIGHT);
	QRect rcTemp[8][5];
	QRect rcTitle[2]; //标题栏分为两列
	QPainter p;
	QPixmap pix(rc.size());
	int row = 6; //行
	int line = 4; //列

	if (!pix.isNull())
	{
		p.begin(&pix);
		p.fillRect(pix.rect(), BACKCOLORWIN4);

		//计算绘制空间
		QRect rcPara = QRect(pix.rect().x(), pix.rect().y() + pix.rect().height() / row, pix.rect().width(), pix.rect().height() - pix.rect().height() / row);
		rcTitle[0] = QRect(pix.rect().x() - 2, pix.rect().y(), pix.rect().width() + 2 - 60, pix.rect().height() / 6); //标题区0
		rcTitle[1] = QRect((pix.rect().x() + pix.rect().width() + 2 - 60) - 2, pix.rect().y(), 60, pix.rect().height() / 6); //标题区

		p.fillRect(rcTitle[0], BACKCOLORWIN4);
		p.fillRect(rcTitle[1], BACKCOLORWIN4);
		p.fillRect(rcPara, Qt::white);

		for (int i = 0; i < row; i++)
		{
			for (int j = 0; j < line; j++)
			{
				rcTemp[i][j] = QRect(rcPara.x() + j * rcPara.width() / line, rcPara.y() + i * rcPara.height() / row, rcPara.width() / line, rcPara.height() / row);
			}
		}

		QString displayQS, QtempV, QtempU;

		//绘制标题栏
		p.setPen(Qt::black);
		if (sysData.system.isLanguageEn)
		{
			p.setFont(fontEnglishBold);
		} else
		{
			p.setFont(fontChinesePara);
		}

		displayQS = "Analog Modelation Information";
		p.drawRect(rcTitle[0]);
		p.drawText(rcTitle[0].x() + 20, rcTitle[0].y(), rcTitle[0].width(), rcTitle[0].height(), Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayQS));

		p.setFont(fontEnglishBold);
		displayQS = "1/1";
		p.drawRect(rcTitle[1]);
		p.drawText(rcTitle[1].x() + 20, rcTitle[1].y(), rcTitle[1].width(), rcTitle[1].height(), Qt::AlignLeft | Qt::AlignVCenter, displayQS);

		if (sysData.system.isLanguageEn)
		{
			p.setFont(fontEnglishBold);
		} else
		{
			p.setFont(fontChinesePara);
		}
		//绘制名称
		displayQS = "Modulate";
		p.drawText(rcTemp[0][0], Qt::AlignCenter, getTheTranslatorString(displayQS));
		displayQS = "State";
		p.drawText(rcTemp[0][1], Qt::AlignCenter, getTheTranslatorString(displayQS));
		displayQS = "Depth/Dev";
		p.drawText(rcTemp[0][2], Qt::AlignCenter, getTheTranslatorString(displayQS));
		displayQS = "Rate/Width";
		p.drawText(rcTemp[0][3], Qt::AlignCenter, getTheTranslatorString(displayQS));

		//绘制AM参数
		p.setFont(fontEnglishLight);
		if (sysData.analog.am.state)
			p.setPen(Qt::black);
		else
			p.setPen(Qt::gray);

		displayQS = "AM";
		p.drawText(rcTemp[1][0], Qt::AlignCenter, getTheTranslatorString(displayQS));

		if (sysData.analog.am.state)
		{
			displayQS = "On";
		} else
		{
			displayQS = "Off";
		}
		p.drawText(rcTemp[1][1], Qt::AlignCenter, getTheTranslatorString(displayQS));

		QtempV.sprintf("%.2f", sysData.analog.am.depth);
		QtempU = "%";
		displayQS = (QtempV + QtempU);
		p.drawText(rcTemp[1][2], Qt::AlignCenter, displayQS);

		QtempV = doubleToQValue(sysData.analog.am.rate);
		QtempV = insertThousandsQS(QtempV);
		QtempU = freqToQUnit(sysData.analog.am.rate);
		displayQS = (QtempV + QtempU);
		if (sysData.analog.source == astExternal)
			p.setPen(Qt::gray);
		p.drawText(rcTemp[1][3], Qt::AlignCenter, displayQS);

		//绘制FM参数
		if (sysData.analog.fm.state)
			p.setPen(Qt::black);
		else
			p.setPen(Qt::gray);

		displayQS = "FM";
		p.drawText(rcTemp[2][0], Qt::AlignCenter, getTheTranslatorString(displayQS));

		if (sysData.analog.fm.state)
		{
			displayQS = "On";
		} else
		{
			displayQS = "Off";
		}
		p.drawText(rcTemp[2][1], Qt::AlignCenter, getTheTranslatorString(displayQS));

		QtempV = doubleToQValue(sysData.analog.fm.deviation);
		QtempV = insertThousandsQS(QtempV);
		QtempU = freqToQUnit(sysData.analog.fm.deviation);
		displayQS = (QtempV + QtempU);
		p.drawText(rcTemp[2][2], Qt::AlignCenter, displayQS);

		QtempV = doubleToQValue(sysData.analog.fm.rate);
		QtempV = insertThousandsQS(QtempV);
		QtempU = freqToQUnit(sysData.analog.fm.rate);
		displayQS = (QtempV + QtempU);
		if (sysData.analog.source == astExternal)
			p.setPen(Qt::gray);
		p.drawText(rcTemp[2][3], Qt::AlignCenter, displayQS);

		//绘制φM参数
		if (sysData.analog.pm.state)
			p.setPen(Qt::black);
		else
			p.setPen(Qt::gray);

		displayQS = "φM";
		p.drawText(rcTemp[3][0], Qt::AlignCenter, getTheTranslatorString(displayQS));

		if (sysData.analog.pm.state)
		{
			displayQS = "On";
		} else
		{
			displayQS = "Off";
		}

		p.drawText(rcTemp[3][1], Qt::AlignCenter, getTheTranslatorString(displayQS));

		QtempV.sprintf("%.2f", sysData.analog.pm.phase);
		QtempU = QString("°");
		displayQS = (QtempV + QtempU);
		p.drawText(rcTemp[3][2], Qt::AlignCenter, displayQS);

		QtempV = doubleToQValue(sysData.analog.pm.rate);
		QtempV = insertThousandsQS(QtempV);
		QtempU = freqToQUnit(sysData.analog.pm.rate);
		displayQS = (QtempV + QtempU);
		if (sysData.analog.source == astExternal)
			p.setPen(Qt::gray);
		p.drawText(rcTemp[3][3], Qt::AlignCenter, displayQS);
		/*
		 //绘制Pluse参数
		 if(sysData.analog.pluse.state)	p.setPen(Qt::black);
		 else p.setPen(Qt::gray);

		 displayQS = "Pluse";
		 p.drawText(rcTemp[5][0], Qt::AlignCenter, displayQS);

		 if(sysData.analog.pluse.state)
		 {
		 displayQS = "On";
		 }else
		 {
		 displayQS = "Off";
		 }

		 p.drawText(rcTemp[5][1], Qt::AlignCenter, displayQS);

		 char tempChar[64]={0};
		 QtempV = QString(floatToStringDot3(valueToDValue(sysData.analog.pluse.period), tempChar)).trimmed();
		 QtempU = demodTimeToQUnit(sysData.analog.pluse.period);
		 displayQS = ( QtempV + QtempU);
		 p.drawText(rcTemp[5][2], Qt::AlignCenter, displayQS);

		 QtempV = QString(floatToStringDot3(valueToDValue(sysData.analog.pluse.width), tempChar)).trimmed();
		 QtempU = demodTimeToQUnit(sysData.analog.pluse.width);
		 displayQS = (QtempV + QtempU);
		 p.drawText(rcTemp[5][3], Qt::AlignCenter, displayQS);
		 */
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

void mainForm::drawAnalogModulateSlots(void)
{
	drawAnalogModulate();
}

//绘制数字调制参数区
void mainForm::drawDigitalModulate(void)
{
	if (sysScpi->isUpdating)
	{
		return;
	}
	QRect rc = QRect(WIN4X + 2 * BORDERWIDTH, WIN4Y, WIN4WIDTH - 2 * BORDERWIDTH - 2 * WIN4X, WIN4HEIGHT);
	QRect rcTemp[2][16];
	QRect rcTitle[2]; //标题栏分为两列
	QPainter p;
	QPixmap pix(rc.size());
	int row = 6;

	if (!pix.isNull())
	{
		p.begin(&pix);
		//计算绘制空间
		QRect rcTemp1 = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width(), pix.rect().height());
		QRect rcPara = QRect(pix.rect().x() + 30, pix.rect().y() + pix.rect().height() / 4, pix.rect().width(), pix.rect().height() / row * 4);
		rcTitle[0] = QRect(pix.rect().x() - 2, pix.rect().y(), pix.rect().width() + 2 - 60, pix.rect().height() / row); //标题区0
		rcTitle[1] = QRect((pix.rect().x() + pix.rect().width() + 2 - 60) - 2, pix.rect().y(), 60, pix.rect().height() / row); //标题区

		p.fillRect(rcTemp1, Qt::white);
		p.fillRect(rcTitle[0], BACKCOLORWIN4);
		p.fillRect(rcTitle[1], BACKCOLORWIN4);

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				rcTemp[j][i] = QRect(rcPara.x() + j * (rcPara.width() - 30) / 2, rcPara.y() + i * rcPara.height() / 4, rcPara.width() / 2, rcPara.height() / 4);
			}
		}

		p.setPen(Qt::black);
		QString displayString, QtempV, QtempU, tempValue;

		//绘制标题栏
		if (sysData.system.isLanguageEn)
		{
			p.setFont(fontEnglishBold);
		} else
		{
			p.setFont(fontChinesePara);
		}
		displayString = "Digital Modelation Information";
		p.drawRect(rcTitle[0]);
		p.drawText(rcTitle[0].x() + 20, rcTitle[0].y(), rcTitle[0].width(), rcTitle[0].height(), Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayString));

		p.setFont(fontEnglishBold);
		displayString = "1/1";
		p.drawRect(rcTitle[1]);
		p.drawText(rcTitle[1].x() + 20, rcTitle[1].y(), rcTitle[1].width(), rcTitle[1].height(), Qt::AlignLeft | Qt::AlignVCenter, displayString);

		//绘制调制状态
		if (sysData.system.isLanguageEn)
		{
			p.setFont(fontEnglishBold);
		} else
		{
			p.setFont(fontChinesePara);
		}

		if (sysData.digital.digitalstate) //数字调制状态是打开
		{
			displayString = "Modelation Status:\tOn";
		} else
		{
			displayString = "Modelation Status:\tOff";
		}
		p.drawText(rcTemp[0][0], Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayString));

		//绘制调制类型
		displayString = "Modelation Type";
		if (sysData.digital.digitalstate)
			p.setPen(Qt::black); //根据状态设置画笔的颜色是黑色还是灰色
		else
			p.setPen(Qt::gray);
		switch (sysData.digital.digitaltype)
		{
			//ASK
			case dt2ASK:
				displayString = getTheTranslatorString(displayString) + ":\t" + "2ASK";
				break;
			case dt4ASK:
				displayString = getTheTranslatorString(displayString) + ":\t" + "4ASK";
				break;
			case dt8ASK:
				displayString = getTheTranslatorString(displayString) + ":\t" + "8ASK";
				break;
				//FSK
			case dt2FSK:
				displayString = getTheTranslatorString(displayString) + ":\t" + "2FSK";
				break;
			case dt4FSK:
				displayString = getTheTranslatorString(displayString) + ":\t" + "4FSK";
				break;
			case dt8FSK:
				displayString = getTheTranslatorString(displayString) + ":\t" + "8FSK";
				break;
				//PSK
			case dt2PSK:
				displayString = getTheTranslatorString(displayString) + ":\t" + "2PSK";
				break;
			case dt4PSK:
				displayString = getTheTranslatorString(displayString) + ":\t" + "4PSK";
				break;
			case dt8PSK:
				displayString = getTheTranslatorString(displayString) + ":\t" + "8PSK";
				break;
			default:
				break;
		}
		p.drawText(rcTemp[0][2], Qt::AlignLeft | Qt::AlignVCenter, displayString);

		//绘制调制参数--深度/频偏/相偏
		switch (sysData.digital.digitaltype)
		{
			case dt2ASK:
			case dt4ASK:
			case dt8ASK:
				displayString = "Depth";
				QtempV.sprintf("%.2f", sysData.digital.digitalpara.depth);
				QtempU = "%";
				break;
			case dt2FSK:
			case dt4FSK:
			case dt8FSK:
				displayString = "Devitation";
				QtempV = doubleToQValue(sysData.digital.digitalpara.deviation);
				QtempV = insertThousandsQS(QtempV); //QSting类型插入千分符
				QtempU = freqToQUnit(sysData.digital.digitalpara.deviation);
				break;
			case dt2PSK:
			case dt4PSK:
			case dt8PSK:
				displayString = "Phase";
				QtempV.sprintf("%.2f", sysData.digital.digitalpara.phase);
				QtempU = "°";
				break;
			default:
				break;
		}
		displayString = getTheTranslatorString(displayString) + ":\t" + QtempV + QtempU;
		p.drawText(rcTemp[1][0], Qt::AlignLeft | Qt::AlignVCenter, displayString);

		//绘制调制参数--速率
		displayString = "Rate";
		QtempV = doubleToQValue(sysData.digital.digitalpara.rate);
		QtempV = insertThousandsQS(QtempV);
		QtempU = freqToQUnit(sysData.digital.digitalpara.rate);
		displayString = getTheTranslatorString(displayString) + ":\t" + QtempV + QtempU;
		p.drawText(rcTemp[1][1], Qt::AlignLeft | Qt::AlignVCenter, displayString);

		//绘制调制源
		displayString = getTheTranslatorString("Digital Source");
		if (sysData.digital.source == dstInternal)
		{
			displayString = displayString + ":\t" + getTheTranslatorString("Internal");
		} else if (sysData.digital.source == dstExternal)
		{
			displayString = displayString + ":\t" + getTheTranslatorString("External");
		}
		p.drawText(rcTemp[0][1], Qt::AlignLeft | Qt::AlignVCenter, displayString);

		p.setPen(Qt::black);

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

//绘制数字调制参数区(原来的绘制方式)
/*
 void mainForm::drawDigitalModulate(void)
 {
 if (sysScpi->isUpdating)
 {
 return;
 }

 QRect rc = QRect(WIN4X + 2 * BORDERWIDTH, WIN4Y, WIN4WIDTH - 2 * BORDERWIDTH - 2 * WIN4X, WIN4HEIGHT);
 QRect rcTemp[8][5];
 char tempChar[32];
 QPainter p;
 QPixmap pix(rc.size());
 int row = 8;	//行
 int line = 4;	//列

 if (!pix.isNull())
 {
 p.begin(&pix);
 p.fillRect(pix.rect(), BACKCOLORWIN4);
 p.drawLine(0, 0, rc.width(), 0);

 //计算绘制空间
 QRect rcPara = QRect(pix.rect().x(),pix.rect().y() + pix.rect().height() / row,pix.rect().width(), pix.rect().height() - pix.rect().height() / row);
 QRect rcTitle = QRect(pix.rect().x()-2,pix.rect().y(),pix.rect().width()+2, pix.rect().height() / 9);

 p.fillRect(rcTitle, BACKCOLORWIN4);
 p.fillRect(rcPara, Qt::white);

 for(int i = 0; i < row; i++)
 {
 for(int j = 0; j < line; j++)
 {
 rcTemp[i][j] = QRect(rcPara.x() + j * rcPara.width() / line, rcPara.y() + i * rcPara.height() / row, rcPara.width() / line, rcPara.height() / row);
 }
 }

 QString displayQS, QtempV, QtempU;

 //绘制标题栏
 p.setPen(Qt::black);
 p.setFont(fontEnglishBold);
 displayQS = "Frequency Modelation Status Information";
 p.drawRect(rcTitle);
 p.drawText(rcTitle.x() + 20, rcTitle.y(), rcTitle.width(), rcTitle.height(), Qt::AlignLeft | Qt::AlignVCenter, displayQS);

 //绘制名称
 displayQS = "Modulate";		p.drawText(rcTemp[1][0], Qt::AlignCenter, displayQS);
 displayQS = "State";		p.drawText(rcTemp[1][1], Qt::AlignCenter, displayQS);
 displayQS = "Mode";			p.drawText(rcTemp[1][2], Qt::AlignCenter, displayQS);
 displayQS = "Rate";	        p.drawText(rcTemp[1][3], Qt::AlignCenter, displayQS);
 displayQS = "Step";    	    p.drawText(rcTemp[1][4], Qt::AlignCenter, displayQS);

 //绘制ASK参数
 if(sysData.digital.ask.state)	p.setPen(Qt::black);	else p.setPen(Qt::gray);
 displayQS = "ASK";
 p.drawText(rcTemp[2][0], Qt::AlignCenter, displayQS);

 if(sysData.digital.ask.state)
 {
 displayQS = "On";
 }else
 {
 displayQS = "Off";
 }

 p.drawText(rcTemp[2][1], Qt::AlignCenter, displayQS);

 switch (sysData.digital.ask.mode)
 {
 case askm2: displayQS = "2ASK"; break;
 case askm4: displayQS = "4ASK"; break;
 case askm8: displayQS = "8ASK"; break;
 default:
 break;
 }

 p.drawText(rcTemp[2][2], Qt::AlignCenter, displayQS);

 QtempV = doubleToQValue(sysData.digital.ask.rate);
 QtempV = insertThousandsQS(QtempV);
 QtempU = freqToQUnit(sysData.digital.ask.rate);
 displayQS = ( QtempV + QtempU );
 p.drawText(rcTemp[2][3], Qt::AlignCenter, displayQS);

 //绘制FSK参数
 if(sysData.digital.fsk.state)	p.setPen(Qt::black);	else p.setPen(Qt::gray);
 displayQS = "FSK";
 p.drawText(rcTemp[3][0], Qt::AlignCenter, displayQS);

 if(sysData.digital.fsk.state)
 {
 displayQS = "On";
 }else
 {
 displayQS = "Off";
 }

 p.drawText(rcTemp[3][1], Qt::AlignCenter, displayQS);

 switch (sysData.digital.fsk.mode)
 {
 case fskm2: displayQS = "2FSK"; break;
 case fskm4: displayQS = "4FSK"; break;
 case fskm8: displayQS = "8FSK"; break;
 default:
 break;
 }
 p.drawText(rcTemp[3][2], Qt::AlignCenter, displayQS);

 QtempV = doubleToQValue(sysData.digital.fsk.rate);
 QtempV = insertThousandsQS(QtempV);
 QtempU = freqToQUnit(sysData.digital.fsk.rate);
 displayQS = ( QtempV + QtempU );
 p.drawText(rcTemp[3][3], Qt::AlignCenter, displayQS);

 QtempV = doubleToQValue(sysData.digital.fsk.step);
 QtempV = insertThousandsQS(QtempV);
 QtempU = freqToQUnit(sysData.digital.fsk.step);
 displayQS = ( QtempV + QtempU );
 p.drawText(rcTemp[3][4], Qt::AlignCenter, displayQS);

 //绘制PSK参数
 if(sysData.digital.psk.state)	p.setPen(Qt::black);
 else p.setPen(Qt::gray);

 displayQS = "PSK";
 p.drawText(rcTemp[4][0], Qt::AlignCenter, displayQS);

 if(sysData.digital.psk.state)
 {
 displayQS = "On";
 }else
 {
 displayQS = "Off";
 }

 p.drawText(rcTemp[4][1], Qt::AlignCenter, displayQS);

 switch (sysData.digital.psk.mode)
 {
 case pskmBpsk: displayQS = "BPSK"; break;
 case pskmQpsk: displayQS = "QPSK"; break;
 case pskmPI4Psk: displayQS = "PI/4 PSK"; break;
 case pskm8: displayQS = "8PSK"; break;
 default: break;
 }
 p.drawText(rcTemp[4][2], Qt::AlignCenter, displayQS);

 QtempV = doubleToQValue(sysData.digital.psk.rate);
 QtempV = insertThousandsQS(QtempV);
 QtempU = freqToQUnit(sysData.digital.psk.rate);
 displayQS = ( QtempV + QtempU );
 p.drawText(rcTemp[4][3], Qt::AlignCenter, displayQS);

 p.end();

 if (sysStart)
 {
 p.begin(imgStart);
 p.drawPixmap(QPoint(rc.left(), rc.top()), pix);
 p.end();
 }
 else
 {
 QScreen::instance()->blit(pix.toImage(), QPoint(rc.left(), rc.top()), rc);
 }
 }
 }
 */
void mainForm::drawDigitalModulateSlots(void)
{
	drawDigitalModulate();
}

//绘制脉冲调制参数区
void mainForm::drawPluseModulate(void)
{
	if (sysScpi->isUpdating)
	{
		return;
	}
	QRect rc = QRect(WIN4X + 2 * BORDERWIDTH, WIN4Y, WIN4WIDTH - 2 * BORDERWIDTH - 2 * WIN4X, WIN4HEIGHT);
	QRect rcTemp[2][16];
	QRect rcTitle[2]; //标题栏分为两列
	QPainter p;
	QPixmap pix(rc.size());
	int row = 6;

	if (!pix.isNull())
	{
		p.begin(&pix);
		//计算绘制空间
		QRect rcTemp1 = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width(), pix.rect().height());
		QRect rcPara = QRect(pix.rect().x() + 30, pix.rect().y() + pix.rect().height() / 4, pix.rect().width(), pix.rect().height() / row * 4);
		rcTitle[0] = QRect(pix.rect().x() - 2, pix.rect().y(), pix.rect().width() + 2 - 60, pix.rect().height() / row); //标题区0
		rcTitle[1] = QRect((pix.rect().x() + pix.rect().width() + 2 - 60) - 2, pix.rect().y(), 60, pix.rect().height() / row); //标题区

		p.fillRect(rcTemp1, Qt::white);
		p.fillRect(rcTitle[0], BACKCOLORWIN4);
		p.fillRect(rcTitle[1], BACKCOLORWIN4);

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				rcTemp[j][i] = QRect(rcPara.x() + j * (rcPara.width() - 30) / 2, rcPara.y() + i * rcPara.height() / 4, rcPara.width() / 2, rcPara.height() / 4);
			}
		}

		p.setPen(Qt::black);
		QString displayString, QtempV, QtempU, tempValue;

		//绘制标题栏
		if (sysData.system.isLanguageEn)
		{
			p.setFont(fontEnglishBold);
		} else
		{
			p.setFont(fontChinesePara);
		}
		displayString = "Pluse Modelation Information";
		p.drawRect(rcTitle[0]);
		p.drawText(rcTitle[0].x() + 20, rcTitle[0].y(), rcTitle[0].width(), rcTitle[0].height(), Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayString));

		p.setFont(fontEnglishBold);
		displayString = "1/1";
		p.drawRect(rcTitle[1]);
		p.drawText(rcTitle[1].x() + 20, rcTitle[1].y(), rcTitle[1].width(), rcTitle[1].height(), Qt::AlignLeft | Qt::AlignVCenter, displayString);

		//绘制脉冲调制状态
		if (sysData.system.isLanguageEn)
		{
			p.setFont(fontEnglishBold);
		} else
		{
			p.setFont(fontChinesePara);
		}
		if (sysData.pluse.state) //数字调制状态是打开
		{
			displayString = "Modelation Status:\tOn";
		} else
		{
			displayString = "Modelation Status:\tOff";
		}
		p.drawText(rcTemp[0][0], Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayString));

		//绘制脉冲源
		displayString = "Pulse Source";
		if (sysData.pluse.state)
			p.setPen(Qt::black);
		else
			p.setPen(Qt::gray);
		switch (sysData.pluse.plusesource)
		{
			case psInternal:
				displayString = getTheTranslatorString(displayString) + ":\t" + getTheTranslatorString(QString("Internal"));
				break;
			case psExternal:
				displayString = getTheTranslatorString(displayString) + ":\t" + getTheTranslatorString(QString("External"));
				break;
			default:
				break;
		}
		p.drawText(rcTemp[1][0], Qt::AlignLeft | Qt::AlignVCenter, displayString);

		//绘制脉冲触发方式
		displayString = "Modelation Method";
		if (sysData.pluse.state)
		{
			if (sysData.pluse.plusesource == psInternal)
				p.setPen(Qt::black);
			else
				p.setPen(Qt::gray);
		} else
		{
			p.setPen(Qt::gray);
		}
		switch (sysData.pluse.plusemethod)
		{
			case plmAuto:
				displayString = getTheTranslatorString(displayString) + ":\t" + getTheTranslatorString(QString("Auto"));
				break;
			case plmTrigg:
				displayString = getTheTranslatorString(displayString) + ":\t" + getTheTranslatorString(QString("Trig"));
				break;
				//		case plmExter:
				//			displayString = getTheTranslatorString(displayString) + ":\t" + getTheTranslatorString(QString("Exter"));
				//			break;
			default:
				break;
		}
		p.drawText(rcTemp[0][1], Qt::AlignLeft | Qt::AlignVCenter, displayString);

		//绘制脉冲周期
		displayString = "Pluse Period";
		QtempV = doubleToQValue(sysData.pluse.period * 1e6);
		QtempV = insertThousandsQS(QtempV);
		QtempU = timeToQUnit(sysData.pluse.period);
		displayString = getTheTranslatorString(displayString) + ":\t" + QtempV + QtempU;
		p.drawText(rcTemp[1][1], Qt::AlignLeft | Qt::AlignVCenter, displayString);

		//绘制脉冲宽度
		displayString = "Pluse Width";
		QtempV = doubleToQValue(sysData.pluse.width * 1e6);
		QtempV = insertThousandsQS(QtempV);
		QtempU = timeToQUnit(sysData.pluse.width);
		displayString = getTheTranslatorString(displayString) + ":\t" + QtempV + QtempU;
		p.drawText(rcTemp[0][2], Qt::AlignLeft | Qt::AlignVCenter, displayString);

		p.setPen(Qt::black);

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

void mainForm::drawPluseModulateSlots(void)
{
	drawPluseModulate();
}

//绘制任意波参数区
void mainForm::drawArbitraryWave(void)
{
	if (sysScpi->isUpdating)
	{
		return;
	}

	QRect rc = QRect(WIN4X + 2 * BORDERWIDTH, WIN4Y, WIN4WIDTH - 2 * BORDERWIDTH - 2 * WIN4X, WIN4HEIGHT);
	QRect rcTemp[2][16];
	QRect rcTitle[2]; //标题栏分为两列
	QPainter p;
	QPixmap pix(rc.size());
	char temp[64];
	char tempChar[64] = { 0 };
	int row = 6; //行

	if (!pix.isNull())
	{
		p.begin(&pix);

		//计算绘制空间
		QRect rcTemp1 = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width(), pix.rect().height());
		QRect rcPara = QRect(pix.rect().x() + 30, pix.rect().y() + pix.rect().height() / 4, pix.rect().width(), pix.rect().height() / row * 4);
		rcTitle[0] = QRect(pix.rect().x() - 2, pix.rect().y(), pix.rect().width() + 2 - 60, pix.rect().height() / row); //标题区0
		rcTitle[1] = QRect((pix.rect().x() + pix.rect().width() + 2 - 60) - 2, pix.rect().y(), 60, pix.rect().height() / row); //标题区

		p.fillRect(rcTemp1, Qt::white);
		p.fillRect(rcTitle[0], BACKCOLORWIN4);
		p.fillRect(rcTitle[1], BACKCOLORWIN4);

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				rcTemp[j][i] = QRect(rcPara.x() + j * (rcPara.width() - 30) / 2, rcPara.y() + i * rcPara.height() / 4, rcPara.width() / 2, rcPara.height() / 4);
			}
		}
		p.setPen(Qt::black);
		QString displayQS, QtempV, QtempU;

		//绘制标题栏
		if (sysData.system.isLanguageEn)
		{
			p.setFont(fontEnglishBold);
		} else
		{
			p.setFont(fontChinesePara);
		}
		displayQS = "Arbitrary Wave Status Information";
		p.drawRect(rcTitle[0]);
		p.drawText(rcTitle[0].x() + 20, rcTitle[0].y(), rcTitle[0].width(), rcTitle[0].height(), Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayQS));

		p.setFont(fontEnglishBold);
		displayQS = "1/1";
		p.drawRect(rcTitle[1]);
		p.drawText(rcTitle[1].x() + 20, rcTitle[1].y(), rcTitle[1].width(), rcTitle[1].height(), Qt::AlignLeft | Qt::AlignVCenter, displayQS);

		//绘制输出波形名称
		if (sysData.system.isLanguageEn)
		{
			p.setFont(fontEnglishBold);
		} else
		{
			p.setFont(fontChinesePara);
		}

		//绘制LF状态
		if (sysData.lf.state)
		{
			displayQS = getTheTranslatorString(QString("LFOut Status")) + ":\t" + getTheTranslatorString(QString("ON"));
		} else
		{
			displayQS = getTheTranslatorString(QString("LFOut Status")) + ":\t" + getTheTranslatorString(QString("OFF"));
		}
		p.drawText(rcTemp[0][0], Qt::AlignLeft | Qt::AlignVCenter, displayQS);

		if (sysData.lf.state)
			p.setPen(Qt::black);
		else
			p.setPen(Qt::gray);

		displayQS = getTheTranslatorString("Wave");
		switch (sysData.lf.type)
		{
			case lotSine:
				displayQS = displayQS + ":\t" + getTheTranslatorString("Sine");
				break;
			case lotSquare:
				displayQS = displayQS + ":\t" + getTheTranslatorString("Square");
				break;
			case lotTriangle:
				displayQS = displayQS + ":\t" + getTheTranslatorString("Triangle");
				break;
			case lotSawtooth:
				displayQS = displayQS + ":\t" + getTheTranslatorString("Sawtooth");
				break;
				//case lotSinc: displayQS = displayQS + ":\t" + getTheTranslatorString("Sinc"); break;
		}
		p.drawText(rcTemp[1][0], Qt::AlignLeft | Qt::AlignVCenter, displayQS);

		//绘制输出频率
		displayQS = getTheTranslatorString("Frequency");
		//	QtempV = doubleToQValue(sysData.lf.freq);
		strcpy(tempChar, getFreqStringInLF(sysData.lf.freq, temp));
		QtempV = QString(tempChar);
		QtempV = insertThousandsQS(QtempV).trimmed().toUpper();
		QtempU = freqToQUnit(sysData.lf.freq);
		displayQS = displayQS + ":\t" + QtempV + QtempU;
		p.drawText(rcTemp[0][1], Qt::AlignLeft | Qt::AlignVCenter, displayQS);

		//绘制输出电压
		displayQS = getTheTranslatorString("Voltage");
		QtempV = doubleToQValue(sysData.lf.ampl);
		QtempV = insertThousandsQS(QtempV);
		QtempU = vppToQUnit(sysData.lf.ampl);
		displayQS = displayQS + ":\t" + QtempV + QtempU;
		p.drawText(rcTemp[1][1], Qt::AlignLeft | Qt::AlignVCenter, displayQS);

		p.setPen(Qt::black);

#if 0
		//绘制名称
		displayQS = "Wave"; p.drawText(rcTemp[1][0], Qt::AlignCenter, displayQS);
		displayQS = "Freq"; p.drawText(rcTemp[1][1], Qt::AlignCenter, displayQS);
		displayQS = "Voltage"; p.drawText(rcTemp[1][2], Qt::AlignCenter, displayQS);

		//绘制IF参数
		p.setFont(fontEnglishLight);
		p.setPen(Qt::black);

		switch(sysData.lf.type)
		{
			case lotSine: displayQS = "Sine"; break;
			case lotSquare: displayQS = "Square"; break;
			case lotTriangle: displayQS = "Triangle"; break;
			case lotSawtooth: displayQS = "Sawtooth"; break;
			case lotSinc: displayQS = "Sinc"; break;
		}

		p.drawText(rcTemp[3][0], Qt::AlignCenter, displayQS);

		//	QtempV = doubleToQValue(sysData.lf.freq);
		strcpy(tempChar, getFreqStringInLF(sysData.lf.freq,temp));
		QtempV = QString(tempChar);
		QtempV = insertThousandsQS(QtempV);
		QtempU = freqToQUnit(sysData.lf.freq);
		displayQS = ( QtempV + QtempU);
		p.drawText(rcTemp[3][1], Qt::AlignCenter, displayQS);

		QtempV = doubleToQValue(sysData.lf.ampl);
		QtempV = insertThousandsQS(QtempV);
		QtempU = vppToQUnit(sysData.lf.ampl);
		displayQS = ( QtempV + QtempU);
		p.drawText(rcTemp[3][2], Qt::AlignCenter, displayQS);
#endif
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

void mainForm::drawArbitraryWaveSlots(void)
{
	drawArbitraryWave();
}

//绘制DRG
void mainForm::drawDrgFunction(void)
{
	if (sysScpi->isUpdating)
	{
		return;
	}
	QRect rc = QRect(WIN4X + 2 * BORDERWIDTH, WIN4Y, WIN4WIDTH - 2 * BORDERWIDTH - 2 * WIN4X, WIN4HEIGHT);
	QRect rcTemp[2][16];
	QRect rcTitle[2]; //标题栏分为两列
	QPainter p;
	QPixmap pix(rc.size());
	char temp[64];
	char tempChar[64] = { 0 };
	int row = 6; //行
	if (!pix.isNull())
	{
		p.begin(&pix);
		p.fillRect(pix.rect(), BACKCOLORWIN4);
		//计算空间
		QRect rcTemp1 = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width(), pix.rect().height());
		QRect rcPara = QRect(pix.rect().x() + 30, pix.rect().y() + pix.rect().height() / 4, pix.rect().width(), pix.rect().height() / row * 4);
		rcTitle[0] = QRect(pix.rect().x() - 2, pix.rect().y(), pix.rect().width() + 2 - 60, pix.rect().height() / row); //标题区0
		rcTitle[1] = QRect((pix.rect().x() + pix.rect().width() + 2 - 60) - 2, pix.rect().y(), 60, pix.rect().height() / row); //标题区

		p.fillRect(rcTemp1, Qt::white);
		p.fillRect(rcTitle[0], BACKCOLORWIN4);
		p.fillRect(rcTitle[1], BACKCOLORWIN4);
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				rcTemp[j][i] = QRect(rcPara.x() + j * (rcPara.width() - 30) / 2, rcPara.y() + i * rcPara.height() / 4, rcPara.width() / 2, rcPara.height() / 4);
			}
		}
		p.setPen(Qt::black);

		QString displayQS, QtempV, QtempU;
		//绘制标题栏
		if (sysData.system.isLanguageEn)
		{
			p.setFont(fontEnglishBold);
		} else
		{
			p.setFont(fontChinesePara);
		}
		displayQS = getTheTranslatorString("DRG Information");
		p.drawRect(rcTitle[0]);
		p.drawText(rcTitle[0].x() + 20, rcTitle[0].y(), rcTitle[0].width(), rcTitle[0].height(), Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayQS));

		p.setFont(fontEnglishBold);
		displayQS = "1/1";
		p.drawRect(rcTitle[1]);
		p.drawText(rcTitle[1].x() + 20, rcTitle[1].y(), rcTitle[1].width(), rcTitle[1].height(), Qt::AlignLeft | Qt::AlignVCenter, displayQS);

		if (sysData.system.isLanguageEn)
		{
			p.setFont(fontEnglishBold);
		} else
		{
			p.setFont(fontChinesePara);
		}
		//绘制状态
		displayQS = getTheTranslatorString("State");
		if (sysData.drg.state)
		{
			displayQS = displayQS + ":\t" + getTheTranslatorString("ON");
		} else
		{
			displayQS = displayQS + ":\t" + getTheTranslatorString("OFF");
		}
		p.drawText(rcTemp[0][0], Qt::AlignLeft | Qt::AlignVCenter, displayQS);
		//绘制扫描上限
		displayQS = getTheTranslatorString("Up Limit");
		strcpy(tempChar, getFreqString(sysData.drg.uplimit, temp));
		QtempV = QString(tempChar);
		QtempV = insertThousandsQS(QtempV).trimmed().toUpper();
		QtempU = freqToQUnit(sysData.drg.uplimit).trimmed();
		displayQS = displayQS + ":\t" + QtempV + QtempU;
		p.drawText(rcTemp[1][0], Qt::AlignLeft | Qt::AlignVCenter, displayQS);

		//绘制扫描下限
		displayQS = getTheTranslatorString("Down Limit");
		strcpy(tempChar, getFreqString(sysData.drg.downlimit, temp));
		QtempV = QString(tempChar);
		QtempV = insertThousandsQS(QtempV).trimmed().toUpper();
		QtempU = freqToQUnit(sysData.drg.downlimit).trimmed();
		displayQS = displayQS + ":\t" + QtempV + QtempU;
		p.drawText(rcTemp[1][1], Qt::AlignLeft | Qt::AlignVCenter, displayQS);

		//绘制扫描点数
		displayQS = getTheTranslatorString("Points");
		strcpy(tempChar, intToQString(sysData.drg.points, temp));
		QtempV = QString(tempChar);
		QtempV = insertThousandsQS(QtempV);
		displayQS = displayQS + ":\t" + QtempV;
		p.drawText(rcTemp[0][1], Qt::AlignLeft | Qt::AlignVCenter, displayQS);

		//绘制扫描速率
		displayQS = getTheTranslatorString("Rate");
		strcpy(tempChar, doubleToQValue(sysData.drg.rate * 1e6).toStdString().c_str());
		QtempV = QString(tempChar);
		QtempV = insertThousandsQS(QtempV);
		QtempU = timeToQUnit(sysData.drg.rate);
		displayQS = displayQS + ":\t" + QtempV + QtempU;
		p.drawText(rcTemp[0][2], Qt::AlignLeft | Qt::AlignVCenter, displayQS);

		//绘制工作模式
		displayQS = getTheTranslatorString("Work Mode");
		if (sysData.drg.workmode == ddnormal)
		{
			displayQS = displayQS + ":\t" + getTheTranslatorString("Normal");
		} else if (sysData.drg.workmode == ddpositive)
		{
			displayQS = displayQS + ":\t" + getTheTranslatorString("Positive");
		} else if (sysData.drg.workmode == ddnegative)
		{
			displayQS = displayQS + ":\t" + getTheTranslatorString("Negative");
		}
		p.drawText(rcTemp[1][2], Qt::AlignLeft | Qt::AlignVCenter, displayQS);

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
void mainForm::drawDrgFunctionSlots(void)
{
	drawDrgFunction();
}

//绘制系统信息参数区
void mainForm::drawSystemPara(void)
{
	if (sysScpi->isUpdating)
	{
		return;
	}

	QRect rc = QRect(WIN4X + 2 * BORDERWIDTH, WIN4Y, WIN4WIDTH - 2 * BORDERWIDTH - 2 * WIN4X, WIN4HEIGHT);
	QRect rcTemp[2][16];
	QRect rcTitle[2]; //标题栏分为两列
	QPainter p;
	QPixmap pix(rc.size());
	int row = 6; //行
	char tempChar[32] = { };

	if (!pix.isNull())
	{
		p.begin(&pix);
		p.fillRect(pix.rect(), BACKCOLORWIN4);

		//计算绘制空间

		QRect rcTemp1 = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width(), pix.rect().height());
		QRect rcPara = QRect(pix.rect().x() + 30, pix.rect().y() + pix.rect().height() / 4, pix.rect().width(), pix.rect().height() / row * 4);
		rcTitle[0] = QRect(pix.rect().x() - 2, pix.rect().y(), pix.rect().width() + 2 - 60, pix.rect().height() / row); //标题区0
		rcTitle[1] = QRect((pix.rect().x() + pix.rect().width() + 2 - 60) - 2, pix.rect().y(), 60, pix.rect().height() / row); //标题区

		p.fillRect(rcTemp1, Qt::white);
		p.fillRect(rcTitle[0], BACKCOLORWIN4);
		p.fillRect(rcTitle[1], BACKCOLORWIN4);

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				rcTemp[j][i] = QRect(rcPara.x() + j * (rcPara.width() - 30) / 2, rcPara.y() + i * rcPara.height() / 4, rcPara.width() / 2, rcPara.height() / 4);
			}
		}

		p.setPen(Qt::black);

		QString displayQS, QtempV, QtempU, macString;

		//绘制标题栏
		if (sysData.system.isLanguageEn)
		{
			p.setFont(fontEnglishBold);
		} else
		{
			p.setFont(fontChinesePara);
		}
		displayQS = getTheTranslatorString("Network Status Information");
		p.drawRect(rcTitle[0]);
		p.drawText(rcTitle[0].x() + 20, rcTitle[0].y(), rcTitle[0].width(), rcTitle[0].height(), Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayQS));

		p.setFont(fontEnglishBold);
		displayQS = "1/1";
		p.drawRect(rcTitle[1]);
		p.drawText(rcTitle[1].x() + 20, rcTitle[1].y(), rcTitle[1].width(), rcTitle[1].height(), Qt::AlignLeft | Qt::AlignVCenter, displayQS);

		//绘制名称
		displayQS = "IP:\t" + QString(sysData.system.io.lan.ip);
		p.drawText(rcTemp[0][0], Qt::AlignLeft | Qt::AlignVCenter, displayQS);
		displayQS = "Mask:\t" + QString(sysData.system.io.lan.mask);
		p.drawText(rcTemp[0][2], Qt::AlignLeft | Qt::AlignVCenter, displayQS);
		displayQS = "Gate:\t" + QString(sysData.system.io.lan.gate);
		p.drawText(rcTemp[0][1], Qt::AlignLeft | Qt::AlignVCenter, displayQS);

		//绘制软件版本号
		displayQS = "Software Version:\t" + QString(intToString(VER_MAJOR, tempChar)).trimmed() + "." + QString(intToString(VER_MINOR, tempChar)).trimmed() + "." + QString(intToString(VER_REVISION,
				tempChar)).trimmed() + "." + QString(intToString(VER_BUILD, tempChar)).trimmed() + "." + QString(intToString(VER_TAG, tempChar)).trimmed();
		p.drawText(rcTemp[1][1], Qt::AlignLeft | Qt::AlignVCenter, displayQS);

#if 1
		//绘制模块软件版本号
		//	displayQS = "Module Software Ver:\t" + sysScpi->moduSoftVer;
		//	p.drawText(rcTemp[0][3],  Qt::AlignLeft | Qt::AlignVCenter, displayQS);

		//绘制序列号
		displayQS = "Serial Number:\t" + sysScpi->moduSerial;
		p.drawText(rcTemp[1][0], Qt::AlignLeft | Qt::AlignVCenter, displayQS);

		//绘制模块类型
		//	displayQS = "Device Type:\t" + sysScpi->moduDevType;
		//	p.drawText(rcTemp[0][2],  Qt::AlignLeft | Qt::AlignVCenter, displayQS);

		//绘制MAC地址
		for (int i = 0; i < sizeof sysData.system.lanMacAddress / sizeof sysData.system.lanMacAddress[0]; i++)
		{
			if (i > 0 && (i % 2) == 0)
			{
				macString += "-";
			}
			macString += QString(intToHex(sysData.system.lanMacAddress[i], tempChar)).trimmed();
		}
		displayQS = "MAC:\t" + macString;
		p.drawText(rcTemp[1][2], Qt::AlignLeft | Qt::AlignVCenter, displayQS);

		//绘制剩余试用天数
		if (sysScpi->eva.isEvaluate && !sysScpi->eva.evaluated)
		{
			sysScpi->eva.syDays = sysScpi->eva.timesOut - qAbs(QDate::currentDate().day() - sysScpi->eva.day + (QDate::currentDate().month() - sysScpi->eva.month) * 31 + (QDate::currentDate().year()
					- sysScpi->eva.year) * 365);
			displayQS = getTheTranslatorString(QString("Remain")) + ":\t" + QString(intToString(qAbs(sysScpi->eva.syDays / 24), tempChar)) + getTheTranslatorString(QString(" Days"));
			p.drawText(rcTemp[0][3], Qt::AlignLeft | Qt::AlignVCenter, displayQS);
		}

#endif

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

void mainForm::drawSystemParaSlots(void)
{
	drawSystemPara();
}

//绘制按钮栏显示区(70ms)
void mainForm::drawButton(void)
{
	if (sysScpi->isUpdating)
	{
		return;
	}

	QRect rc = QRect(WIN5X + 2 * BORDERWIDTH, WIN5Y, WIN5WIDTH - 2 * BORDERWIDTH, WIN5HEIGHT);
	QRect rcTemp = QRect(rc.x() + BUTTONINTERVAL, rc.y() + BUTTONINTERVAL, rc.width() / BUTTONCOUNT, rc.height() - 2 * BUTTONINTERVAL );
	QRect rcTemp1 = QRect(rcTemp.x() + BUTTONINTERVAL, rcTemp.y() + BUTTONINTERVAL, rcTemp.width() - 1 * BUTTONINTERVAL, rcTemp.height() - 2 * BUTTONINTERVAL );
	QRect rcButton;
	int btnIndex = 0;
	int btnNums = 0;

	QPainter p;

	QPixmap pix(rc.size()); //QT:像素映射
	p.begin(&pix);
	p.fillRect(pix.rect(), BACKCOLORWIN5); //绘制按键后面区域为默认背景色
	p.drawLine(rc.x(), rc.y(), rc.x() + rc.width(), rc.y());

	p.setFont(fontData);

	//------------------------------------------按键无效或错误 button is invalid or error----------------------------------------
	if (((nowMainKey == NULL) && (nowSubKey == NULL)) || ((nowMainKey == NULL) && (nowSubKey != NULL))) //如果按键无效或错误，则只绘制按键，不绘制文字信息
	{
		for (int i = 0; i < BUTTONCOUNT; i++)
		{
			rcButton = QRect(BUTTONINTERVAL + rcTemp.width() * i, BUTTONINTERVAL, rcTemp.width() - 2 * BUTTONINTERVAL, rcTemp.height());

			qDrawWinPanel(&p, rcButton, palette(), false, 0);
		}
	}
	//-------------------------------------------------按键有效 button valid-----------------------------------------------------
	else
	{
		subKeyDef* keyTemp = NULL;

		if (nowSubKey == NULL) //复位或开机键 preset or poweron
		{
			keyTemp = nowMainKey->rightNode;
		} else //正常运行 normal run
		{
			keyTemp = nowSubKey;
			while (keyTemp->upNode != NULL) //跳至顶层节点 goto top node
			{
				keyTemp = keyTemp->upNode;
			}
		}

		QPixmap pix(rcTemp.size());

		//node loop
		while (keyTemp != NULL)
		{
			btnIndex = sysMenu->getButtonIndex(keyTemp->keyValue); //通过btnIndex选择要绘制的按键索引

			btnNums++;
			//button count limit
			if (nowMainKey->keyValue == 0xD5 || nowMainKey->keyValue == 0xDA || nowMainKey->keyValue == 0xDB || nowMainKey->keyValue == 0xDC) //AMPL特殊处理,加了缆损
			{
				if (btnNums > 7)
				{
					break;
				}
			} else
			{
				if (btnNums > BUTTONCOUNT) //如果索引越界，则返回
				{
					break;
				}
			}

			//button index limit
			if (nowMainKey->keyValue == 0xD5 || nowMainKey->keyValue == 0xDA || nowMainKey->keyValue == 0xDB || nowMainKey->keyValue == 0xDC) //AMPL特殊处理,由于按下MOD,LF,RF也绘制button，所以也需要排除
			{
				if ((btnIndex >= 0) && (btnIndex < 7)) //如果索引未越界则绘制
				{
					rcButton = QRect(BUTTONINTERVAL + rcTemp.width() * btnIndex, BUTTONINTERVAL, rcTemp.width() - 2 * BUTTONINTERVAL, rcTemp.height());
					QPixmap pix(rcButton.size());
					drawButtonMenu(&pix, keyTemp); //将按键上的文本及位置等信息写入缓存

					qDrawWinPanel(&p, rcButton, palette(), false, 0);
					p.drawPixmap(rcButton.left(), rcButton.top(), pix); //显示
				}
			} else
			{

				if ((btnIndex >= 0) && (btnIndex < BUTTONCOUNT)) //如果索引未越界则绘制
				{
					rcButton = QRect(BUTTONINTERVAL + rcTemp.width() * btnIndex, BUTTONINTERVAL, rcTemp.width() - 2 * BUTTONINTERVAL, rcTemp.height());
					QPixmap pix(rcButton.size());

					drawButtonMenu(&pix, keyTemp); //将按键上的文本及位置等信息写入缓存

					qDrawWinPanel(&p, rcButton, palette(), false, 0);
					p.drawPixmap(rcButton.left(), rcButton.top(), pix); //显示
				}
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

void mainForm::drawButtonSlots(void)
{
	drawButton();
}

//绘制process
void mainForm::drawProcessSlots(void)
{
	drawProcess();
}

//绘制全部显示区
void mainForm::drawAll(void)
{
	drawFreqAndAmplMain(); //绘制主参数显示区
	drawState(); //绘制分隔栏显示区
	drawButton();
}

//载入驱动
void mainForm::loadSystemDriver(void)
{
	//加载键盘驱动
	keyHandle = -2;
	keyFlag = false;
	keyHandle = open(KBD_PORT, O_RDWR | O_NONBLOCK);//NON| O_NOCTTY

	struct termios tio;
	tio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
	tio.c_iflag = IGNPAR | ICRNL;
	tio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	//  tio.c_oflag &= ~(INLCR | IGNCR | ICRNL);
	tio.c_oflag &= ~OPOST;
	//  tio.c_oflag &= ~(ONLCR | OCRNL);
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
	} else
	{
		if (__DEBUG)
		{
			printf("%s\n", "load keyBoard driver failed");
		}
	}

	//加载USB从口驱动
	usbHandle = -1;
	usbHandle = open("/dev/ttyGS0", O_RDWR | O_NONBLOCK);

	if (usbHandle >= 0)
	{
		usbNotifier = new QSocketNotifier(usbHandle, QSocketNotifier::Read, this);
		connect(usbNotifier, SIGNAL(activated(int)), sysScpi, SLOT(readUsbSlaveData()));
	} else
	{
		if (__DEBUG)
		{
			printf("load usb slave driver failed\n");
		}
	}

	//加载串口驱动
	comHandle = -1;
	//  comHandle = open("/dev/ttyS0", O_RDWR | O_NONBLOCK);
	comHandle = open("/dev/ttyS1", O_RDWR | O_NONBLOCK);

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
		connect(comNotifier, SIGNAL(activated(int)), sysScpi, SLOT(readSerialData()));

	} else
	{
		if (__DEBUG)
		{
			printf("%s\n", "load serial driver failed");
		}
	}

}

//输出信息
void mainForm::outputMessage(QString msg)
{
	helpContent = msg;
	sysScpi->drawWin4(helpContent);
	usleep(3000 * 1000);
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

			sysScpi->drawWin4(helpContent);
			usleep(3000 * 1000);

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

	sysScpi->drawWin4(helpContent);
	usleep(3000 * 1000);
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
		}
	}

	return result;
}

//获取子节点最后一个节点
struct subKeyDef* mainForm::getChildLastNode(struct subKeyDef* firstNode)
{
	if (firstNode != NULL)
	{
		struct subKeyDef* tempNode = firstNode;

		while (tempNode != NULL)
		{
			if (tempNode->funcCode == KEY_FUNC_FREQ || tempNode->funcCode == KEY_FUNC_AMPL)
			{
				return tempNode;
			}

			tempNode = tempNode->downNode;
		}
	}

	return NULL;
}

//USB主设备连接
bool mainForm::isUsbHostConnect(void)
{
	QString dirName = "/sys/bus/usb/devices/1-1:1.0";

	return QDir(dirName).exists();
}

//USB从设备连接
bool mainForm::isUsbDeviceConnect(void)
{
	QString dirName = "/sys/bus/usb/devices/usb1";
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
	imgStart = new QPixmap(SCREENWIDTH, SCREENHEIGHT);
	drawFreqAndAmplMain();
	drawState();
	//  drawFreqAndAmpl();
	drawFreqInParamter();
	drawButton();
	strcpy(sysData.name, "");
	strcpy(sysData.value, "");
	strcpy(sysData.unit, "");
	drawInputParamterNoParamter();
	QScreen::instance()->blit(imgStart->toImage(), QPoint(0, 0), QRect(0, 0, SCREENWIDTH, SCREENHEIGHT));
	sysStart = false;
}

//读取系统配置
void mainForm::loadSystemSetting(void)
{
	QString fileName = QCoreApplication::applicationDirPath() + "/setting.ini";

	//default factory value
	//  strcpy(serialNo, "103010001");
	strcpy(serialNo, SERIALNO);
	sysData.system.isLanguageEn = false;

	QSettings setting(fileName, QSettings::IniFormat);
	QVariant value;

	if (QFile(fileName).exists())
	{
		setting.beginGroup("device");
		strcpy(serialNo, setting.value("serialNo", serialNo).toString().toStdString().c_str());
		setting.endGroup();

		setting.beginGroup("version");
		if (setting.value("major", VER_MAJOR) != VER_MAJOR)
		{
			setting.setValue("major", VER_MAJOR);
		}
		if (setting.value("minor", VER_MAJOR) != VER_MINOR)
		{
			setting.setValue("minor", VER_MINOR);
		}
		if (setting.value("revision", VER_REVISION) != VER_REVISION)
		{
			setting.setValue("revision", VER_REVISION);
		}
		if (setting.value("build", VER_BUILD) != VER_BUILD)
		{
			setting.setValue("build", VER_BUILD);
		}
		if (setting.value("tag", VER_TAG) != VER_TAG)
		{
			setting.setValue("tag", VER_TAG);
		}
		setting.endGroup();

		setting.beginGroup("system");
		sysData.system.isLanguageEn = setting.value("language", false).toBool();
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
		//    saveSystemSetting("system", "language", "english");
//		sysScpi->savePowerOffState();
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
	img_Arrow.loadFromData(arrowBits, sizeof arrowBits / sizeof arrowBits[0]);
	img_CLK1On.loadFromData(clk1OnBits, sizeof clk1OnBits / sizeof clk1OnBits[0]);
	img_CLK2On.loadFromData(clk2OnBits, sizeof clk2OnBits / sizeof clk2OnBits[0]);
	img_DR1.loadFromData(DR1Bits, sizeof DR1Bits / sizeof DR1Bits[0]);
	img_DR2.loadFromData(DR2Bits, sizeof DR2Bits / sizeof DR2Bits[0]);
	img_Delay1.loadFromData(Delay1Bits, sizeof Delay1Bits / sizeof Delay1Bits[0]);
	img_Delay2.loadFromData(Delay2Bits, sizeof Delay2Bits / sizeof Delay2Bits[0]);
	img_DIR1.loadFromData(DIR1Bits, sizeof DIR1Bits / sizeof DIR1Bits[0]);
	img_DIR2.loadFromData(DIR2Bits, sizeof DIR2Bits / sizeof DIR2Bits[0]);
	img_DIR3.loadFromData(DIR3Bits, sizeof DIR3Bits / sizeof DIR3Bits[0]);
	img_DIR4.loadFromData(DIR4Bits, sizeof DIR4Bits / sizeof DIR4Bits[0]);
	img_OUT1.loadFromData(OUT1Bits, sizeof OUT1Bits / sizeof OUT1Bits[0]);
	img_OUT2.loadFromData(OUT2Bits, sizeof OUT2Bits / sizeof OUT2Bits[0]);
	img_Velue1_33.loadFromData(Velue1_33Bits, sizeof Velue1_33Bits / sizeof Velue1_33Bits[0]);
	img_Velue2_33.loadFromData(Velue2_33Bits, sizeof Velue2_33Bits / sizeof Velue2_33Bits[0]);
	img_Velue1_25.loadFromData(Velue1_25Bits, sizeof Velue1_25Bits / sizeof Velue1_25Bits[0]);
	img_Velue2_25.loadFromData(Velue2_25Bits, sizeof Velue2_25Bits / sizeof Velue2_25Bits[0]);
	img_Velue1_18.loadFromData(Velue1_18Bits, sizeof Velue1_18Bits / sizeof Velue1_18Bits[0]);
	img_Velue2_18.loadFromData(Velue2_18Bits, sizeof Velue2_18Bits / sizeof Velue2_18Bits[0]);
	img_Velue1_15.loadFromData(Velue1_15Bits, sizeof Velue1_15Bits / sizeof Velue1_15Bits[0]);
	img_Velue2_15.loadFromData(Velue2_15Bits, sizeof Velue2_15Bits / sizeof Velue2_15Bits[0]);
	img_Link.loadFromData(Link, sizeof Link / sizeof Link[0]);
	img_LinkBroken.loadFromData(LinkBroken, sizeof LinkBroken / sizeof LinkBroken[0]);
	img_UP.loadFromData(UP_Bits, sizeof UP_Bits / sizeof UP_Bits[0]);
	img_DOWN.loadFromData(DOWN_Bits, sizeof DOWN_Bits / sizeof DOWN_Bits[0]);
	img_IN.loadFromData(IN_Bits, sizeof IN_Bits / sizeof IN_Bits[0]);
	img_OUT.loadFromData(OUT_Bits, sizeof OUT_Bits / sizeof OUT_Bits[0]);

	img_LoopUP.loadFromData(loopUpBits, sizeof loopUpBits / sizeof loopUpBits[0]);
	img_RePeatUp.loadFromData(repeatUpBits, sizeof repeatUpBits / sizeof repeatUpBits[0]);
	img_LoopDown.loadFromData(loopDownBits, sizeof loopDownBits / sizeof loopDownBits[0]);
	img_RePeatDown.loadFromData(repeatDownBits, sizeof repeatDownBits / sizeof repeatDownBits[0]);
	img_Left.loadFromData(leftArrowBits, sizeof leftArrowBits / sizeof leftArrowBits[0]);
	img_Right.loadFromData(rightArrowBits, sizeof rightArrowBits / sizeof rightArrowBits[0]);
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
	uint8_t recvbuf[8] = { 0 };
	uint8_t sendbuf[9] = { 0 };

	bool isRpgRotate = false;

	unsigned long colorValue = 0;
	int tempState = 0;

	unsigned char usbTempChar[64];
	int usbRcvLen = 0;

	if (keyFlag == false)
	{
		return;
	}

	//读取键码
	keyNotifier->setEnabled(false);
	recvCount = read(keyHandle, recvbuf, 8);
	keyNotifier->setEnabled(true);
	//printf("zpc add test, %02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x\r\n",recvbuf[0],recvbuf[1],recvbuf[2],recvbuf[3],recvbuf[4],recvbuf[5],recvbuf[6],recvbuf[7]);
	//如果是上电第一次，强置为频率键
	if (isPowerOnSelect)
	{
		recvbuf[0] = 0xAA;
		recvbuf[1] = 0x1;
		recvbuf[2] = 0x0;
		recvbuf[3] = 0xCA;
		recvbuf[4] = 0x0;
		recvbuf[5] = 0x0;
		recvbuf[6] = 0x0;
		recvbuf[7] = 0xBB;
		recvCount = 8;
		isPowerOnSelect = false;
	}

	tcflush(keyHandle, TCIFLUSH);

	if (recvCount != 8)
	{
		return;
	}

	if (recvCount == 8 && recvbuf[0] == 0xAA && recvbuf[7] == 0xBB)
	{
		keyValue = recvbuf[3];
		keyClass = recvbuf[1];
		keyCount = recvbuf[5];
	} else
	{
		return;
	}

	//打印键值
	if (__DEBUG)
	{
		printf("key pressed value is keyValue =0x%04x,0x%04x,0x%04x\n", keyValue, keyClass, keyCount);
	}

	if (sysScpi->isUpdating)
	{
		return;
	}

	//evaluate，即试用
	if (sysScpi->eva.isEvaluate && sysScpi->eva.evaluated) //试用时间已过，即需要输入注册码
	{
		int nums = 0;
		int tempValue = evaArray[evaIndex]; //8个字符，代表要输入的8个16进制数
		int tempIndex = evaIndex;
		bool valued = false;
		bool indexed = false;
		char tempChar[32] = { 0x0 };
		QString inputString = "";
		QString md5String = "";
		/*
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
		 } else if (keyValue == KEY_NUM_ENTER)		//按下确认键
		 {
		 for (int i = 0; i < sizeof evaArray / sizeof evaArray[0]; i++)
		 {
		 inputString += QString(intToHex(evaArray[i], tempChar)).trimmed();
		 }

		 md5String = sysScpi->getSystemServiceOfMD5(QString(sysData.system.serialNo).trimmed(), false);	//根据模块编号，计算MD5值

		 if (inputString.toUpper() == md5String.toUpper())		//比较输入的MD5值和计算得到的MD5值
		 {
		 sysScpi->unLockSystem();		//解锁系统
		 }

		 return;
		 }
		 */

		if (keyValue == KEY_ARROW_UP) //左右键用来移动选中框
		{
			tempIndex -= 1;
			indexed = true;
		} else if (keyValue == KEY_ARROW_DOWN) //左右键用来移动选中框
		{
			tempIndex += 1;
			indexed = true;
		} else if (sysMenu->pressedIsRpgKey(keyClass)) //脉冲旋钮用来输入字符，字符范围 0x0 ~ 0xF
		{
			nums = sysMenu->getRpgPulse(keyValue, keyCount);
			if (nums >= 0)
			{
				tempValue++;
			} else
			{
				tempValue--;
			}
			valued = true;
		} else if (keyValue == KEY_SUB_MENU1) //将SUB_MENU1键当做确认键
		{
			for (int i = 0; i < sizeof evaArray / sizeof evaArray[0]; i++)
			{
				inputString += QString(intToHex(evaArray[i], tempChar)).trimmed(); //组合输入的MD5值
			}

			md5String = sysScpi->getSystemServiceOfMD5(sysScpi->moduSerial.trimmed(), false); //根据模块编号，计算MD5值
			if (inputString.toUpper() == md5String.toUpper()) //比较输入的MD5值和计算得到的MD5值
			{
				sysScpi->unLockSystem(); //解锁系统
				presetSystem(false);
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

			evaIndex = tempIndex; //输入到第几个字符
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

			evaArray[evaIndex] = tempValue; //输入的值赋值到数组中

		}
		return;

	}

	//远程控制
	if (sysData.scpiData.stateR)
	{
		if (sysMenu->pressedIsEscKey(keyValue))
		{
			sysData.scpiData.stateR = false;
			sysData.scpiData.stateL = true;
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

		handleMainKey(keyValue);

		//默认选中第一个子键
		if (nowMainKey != NULL && nowMainKey->rightNode != NULL)
		{
			//频率和幅度主键特殊处理
			if (nowMainKey->keyValue == KEY_MAIN_FREQ || nowMainKey->keyValue == KEY_MAIN_AMPL)
			{
				nowSubKey = getChildLastNode(nowMainKey->rightNode);
				//频率和幅度默认激活输入但不显示选中
				if (nowSubKey != NULL)
				{
					nowSubKey->keyValue = KEY_SUB_MENU7;
				}
				keyValue = nowSubKey->keyValue;
				canExecute = false;
			} else

			//遵从原先规则
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
	}

	//按下子键
	if ((sysMenu->pressedIsCommonKey(keyValue) || (keyValue == KEY_SUB_MENU7)) && keyIsEnable(keyValue))
	{
		sysData.cursorLocation = 0; //清零光标位置
		if (sysMenu->getSubKeyPointer(keyValue))
		{
			//isMore
			if ((nowSubKey->isMore) && (nowSubKey->rightNode != NULL))
			{
				handleMoreKey(nowSubKey->funcCode);

				if (nowSubKey->isPassword) //是需要密码（“系统服务”按键）,则不执行gotoDownLevel()函数
				{
					//          inputTag = getTheTranslatorString("password");
					//          tagValue = "******";
					//          inputValue = "******";
					//          prevInput = "";
					previndexOfPassword = 0;
					indexOfPassword = 0;
					memset(passwdString, 0, sizeof passwdString);
					//    		nowSubKey->keyTimes++;
					handleInputKey(nowSubKey->funcCode); //由于需要输入密码，故做InputKey键输入，但实际菜单里没有选中isInput
				} else
				{
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
			}

			//isReturn
			else if (nowSubKey->isReturn)
			{
				//    	cleanInputParamter();		//某些设置在按下返回键后清空参数输入栏显示区内容
				sysMenu->gotoUpLevel();
				canExecute = false;
				//    	handleMoreKey(nowSubKey->funcCode);
				cleanInputParamter(); //某些设置在按下返回键后清空参数输入栏显示区内容
				keyValue = nowSubKey->keyValue;
			}

			//isInput & isReverse
			else if ((nowSubKey->isInput) && (nowSubKey->isReverse))
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
					handleInputKey(nowSubKey->funcCode);
				}
			}

			//isReverse
			else if ((nowSubKey->isReverse) && (canExecute))
			{
				inputTag = "";
				tagValue = "";
				inputValue = "";
				handleReverseKey(nowSubKey->funcCode);
			}

			//isList
			else if (nowSubKey->isList)
			{
				inputTag = "";
				tagValue = "";
				inputValue = "";

				if (canExecute)
				{
					handleListKey(nowSubKey->funcCode);

					if ((nowMainKey != NULL) && (nowSubKey != NULL))
					{
						struct subKeyDef* tempSubKey = nowSubKey;
					}
				}
			}

			//isInput
			else if (nowSubKey->isInput)
			{
				inputTag = getTranslatorString(nowSubKey->enName, nowSubKey->chName);
				nowSubKey->keyTimes++;
				handleInputKey(nowSubKey->funcCode);
			}

			//isExecute
			else if ((nowSubKey->isExecute) && (canExecute))
			{
				inputTag = "";
				tagValue = "";
				inputValue = "";
				handleExecuteKey(nowSubKey->funcCode);

				if (isExecReturn(nowSubKey->funcCode))
				{
					switch (nowSubKey->funcCode)
					//执行键时不做返回上级处理
					{
						case KEY_FUNC_SWEEP_MORE_CONFIGLIST_INSERTROW:
						case KEY_FUNC_SWEEP_MORE_CONFIGLIST_DELETEROW:
							//    	    case KEY_FUNC_FREQREFSET:
							//    	    case KEY_FUNC_AMPLREFSET:
							break;
							//    	    case KEY_FUNC_AMPLREFSET:
							//    	    	nowSubKey = getChildLastNode(nowMainKey->rightNode);
							//    	    	keyValue = nowSubKey->keyValue;
							//    	    	break;
						default:
							sysMenu->gotoUpLevel();
							break;
					}
				}
				if (isNeedReturnFreqOrAmplSet()) //zpc add
				{
					sysMenu->gotoUpLevel();
				}
			}

			//isUnit
			else if (nowSubKey->isUnit)
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
					if (nowSubKey->funcCode != KEY_FUNC_SWEEP_MORE_CONFIGLIST_MORE_DOWNLOAD) //zpc add 当返回到列表扫描装载时不执行handleInputKey()函数
					{
						handleInputKey(nowSubKey->funcCode);
					}
					prvKeyValue = nowSubKey->keyValue;
					nowSubKey->keyTimes = 1;
				}
				if (isNeedReturnFreqOrAmplSet()) //zpc add
				{
					sysMenu->gotoUpLevel();
					char tempValue[64] = { 0 };
					if (nowSubKey->funcCode == KEY_FUNC_AMPL)
					{
						strcpy(sysData.name, "AMPL:");
						sprintf(tempValue, "%.2f", sysData.ampl.ampl);
						strcpy(sysData.value, tempValue);
						strcpy(sysData.unit, amplToQUnit(sysData.ampl.ampl).trimmed().toStdString().c_str());
						drawInputParamterNoParamter();
					}
				}
			}
		} else
		{
			helpContent = "pressed key is ignored";
		}
	}
	//按下方向键,按键功能复用，左方向键与回滚键公用同一按键
	else if ((nowMainKey != NULL) && (nowSubKey != NULL) && (nowSubKey->isEnabled) && (sysMenu->pressedIsArrowKey(keyValue)))
	{
		//按下回滚键
		if ((sysMenu->pressedIsBkspaceKey(keyValue)) && (nowSubKey != NULL) && ((nowSubKey->isUnit) || (nowSubKey->isPassword)))
		{
			if (!inputValue.isEmpty())
			{
				if (nowSubKey->isPassword) //如果是密码
				{
					indexOfPassword--;
					previndexOfPassword--;
				}
				//	      else if(nowSubKey->isMac)	//如果是MAC地址
				//	      {
				//	    	  sysData.system.lanMacAddrIndex--;
				//	      }
				inputValue.remove(inputValue.length() - 1, 1);
				strcpy(sysData.value, inputValue.toStdString().c_str()); //zpc add
				drawInputParamterNoParamter();
			}
			return;
		} else //按下方向键
		{

			if (nowSubKey->isMac) //如果是MAC地址
			{
				if (sysMenu->isUpArrow(keyValue))
				{

					sysData.system.lanMacAddrIndex--; //计算MAC存放数组的索引
					if (sysData.system.lanMacAddrIndex < 0)
						sysData.system.lanMacAddrIndex = 11;
				} else
				{
					sysData.system.lanMacAddrIndex++;
					if (sysData.system.lanMacAddrIndex > 11)
						sysData.system.lanMacAddrIndex = 0;
				}

			}
			handleArrowKey(nowSubKey->funcCode, sysMenu->isUpArrow(keyValue));
		}
	}
	//按下RPG键
	else if ((nowMainKey != NULL) && (nowSubKey != NULL) && (nowSubKey->isEnabled) && (sysMenu->pressedIsRpgKey(keyClass)))
	{
		handleRpgKey(nowSubKey->funcCode, sysMenu->getRpgPulse(keyValue, keyCount));
	}
	//按下数字键
	else if ((sysMenu->pressedIsNumberKey(keyValue)) && (nowSubKey != NULL) && (nowSubKey->isEnabled))
	{
		sysData.cursorLocation = 0; //清零光标位置
		if ((nowSubKey->isInput) && (nowSubKey->rightNode != NULL))
		{
			nowSubKey = nowSubKey->rightNode;
			drawButton(); //Input键绘制单位键
		}

		if (!isNumberKeyInput()) //判断按下数字按键后是否有效
		{
			return;
		}

		switch (keyValue)
		{
			case KEY_NUM_0:
				execNumberInput(QString("0"), true);
				drawInputParamterNoParamter();
				if (__DEBUGKEY)
					printf("KEY_NUM_0 = %d\r\n", inputValue.toInt());
				return;
			case KEY_NUM_1:
				execNumberInput(QString("1"), true);
				drawInputParamterNoParamter();
				if (__DEBUGKEY)
					printf("KEY_NUM_1 = %d\r\n", inputValue.toInt());
				return;
			case KEY_NUM_2:
				execNumberInput(QString("2"), true);
				drawInputParamterNoParamter();
				if (__DEBUGKEY)
					printf("KEY_NUM_2 = %d\r\n", inputValue.toInt());
				return;
			case KEY_NUM_3:
				execNumberInput(QString("3"), true);
				drawInputParamterNoParamter();
				if (__DEBUGKEY)
					printf("KEY_NUM_3 = %d\r\n", inputValue.toInt());
				return;
			case KEY_NUM_4:
				execNumberInput(QString("4"), true);
				drawInputParamterNoParamter();
				if (__DEBUGKEY)
					printf("KEY_NUM_4 = %d\r\n", inputValue.toInt());
				return;
			case KEY_NUM_5:
				execNumberInput(QString("5"), true);
				drawInputParamterNoParamter();
				if (__DEBUGKEY)
					printf("KEY_NUM_5 = %d\r\n", inputValue.toInt());
				return;
			case KEY_NUM_6:
				execNumberInput(QString("6"), true);
				drawInputParamterNoParamter();
				if (__DEBUGKEY)
					printf("KEY_NUM_6 = %d\r\n", inputValue.toInt());
				return;
			case KEY_NUM_7:
				execNumberInput(QString("7"), true);
				drawInputParamterNoParamter();
				if (__DEBUGKEY)
					printf("KEY_NUM_7 = %d\r\n", inputValue.toInt());
				return;
			case KEY_NUM_8:
				execNumberInput(QString("8"), true);
				drawInputParamterNoParamter();
				if (__DEBUGKEY)
					printf("KEY_NUM_8 = %d\r\n", inputValue.toInt());
				return;
			case KEY_NUM_9:
				execNumberInput(QString("9"), true);
				drawInputParamterNoParamter();
				if (__DEBUGKEY)
					printf("KEY_NUM_9 = %d\r\n", inputValue.toInt());
				return;
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
				strcpy(sysData.value, inputValue.toStdString().c_str()); //zpc add
				strcpy(sysData.unit, "");
				drawInputParamterNoParamter();
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
				strcpy(sysData.value, inputValue.toStdString().c_str()); //zpc add
				strcpy(sysData.unit, "");
				drawInputParamterNoParamter();
				return;
		}
	}

	//按下取消键
	else if ((sysMenu->pressedIsEscKey(keyValue)) && (nowSubKey != NULL))
	{
		if (nowSubKey->isUnit)
		{
			sysMenu->gotoUpLevel();
			drawInputParamterNoParamter();
		}

		inputValue = "";
		return;
	}

	//密码确认键
	else if (sysMenu->pressedIsEnterKey(keyValue) && (nowSubKey != NULL) && (nowSubKey->rightNode != NULL) && (!inputValue.isNull()) && (!inputValue.isEmpty()))
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
				strcpy(sysData.value, "");
				strcpy(sysData.name, "");
				drawInputParamterNoParamter();
				return;
			} else
			{
				inputValue = "";
				sysData.system.isServiceUnlock = false;
				//		sysScpi->showRunAlarm(getTheTranslatorString("Passwd error, Please Resume Input!"));
				strcpy(sysData.value, "password error");
				drawInputParamterNoParamter();
				sleep(1);
				return;
			}
		}
	}

	//其它未知键
	else
	{
//		sysScpi->savePowerOffState();
		return;
	}
	//save state
//	sysScpi->savePowerOffState();

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
	usbHandle = open("/dev/ttyGS0", O_RDWR | O_NONBLOCK);

	if (usbHandle >= 0)
	{
		connect(usbNotifier, SIGNAL(activated(int)), sysScpi, SLOT(readUsbSlaveData()));
	}
}

//执行文件选择
bool mainForm::handleFileSelect(bool up, int nums)
{
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
		default:
			return getTheTranslatorString("unKnown");
	}
}

//执行数字输入
#if 0
void mainForm::execNumberInput(QString value)
{
	QString tempchar;
	tempchar = inputValue;
	if (isPassword())
	{
		if (inputValue == "******")
		{
			inputValue = value;
		}
		else
		{
			inputValue += value;
		}
	}
	else
	{
		if (tempchar.remove('-').remove('.').length() < sysData.inputLength)
		{
			inputValue += value;
		}
	}

	strcpy(sysData.value, inputValue.toStdString().c_str());
	strcpy(sysData.unit, "");
	//  sysData.ampl.amplUnit = amplUnitDef(4);
}
#endif

void mainForm::execNumberInput(QString value, bool isNumber)
{
	QString tempchar;
	tempchar = inputValue;
	if (isPassword()) //服务密码
	{
		char tempPasswd[6];
		QString passwdStr = "";

		//是否为数字键，每按一次数字键，密码的位置加1
		if (isNumber)
		{
			indexOfPassword++;

			if (indexOfPassword > 6)
			{
				indexOfPassword = 6;
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
	} else if (isMacAddress()) //MAC地址
	{
		if (tempchar.remove('-').remove('.').length() < sysData.inputLength)
		{
			inputValue += value;
			sysData.system.lanMacAddrIndex++;
		}
	} else
	{
		if (tempchar.remove('-').remove('.').length() < sysData.inputLength)
		{
			inputValue += value;
		}
	}

	strcpy(sysData.value, inputValue.toStdString().c_str());
	strcpy(sysData.unit, "");
	//  sysData.ampl.amplUnit = amplUnitDef(4);
}

//口令
bool mainForm::isPassword(void)
{
	//  return false;
	return nowSubKey != NULL && nowSubKey->funcCode == KEY_FUNC_AUX_SERVICE;
}

//MAC地址
bool mainForm::isMacAddress(void)
{
	return nowSubKey != NULL && nowSubKey->funcCode == KEY_FUNC_AUX_SERVICE_LANMACADDR_ENTER;
}

//口令正确
bool mainForm::isPasswordValid(void)
{
	return false;
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
			case KEY_FUNC_AUX_LAN_IP_ENTER:
				iType = itIP;
				break;
			case KEY_FUNC_AUX_LAN_MASK_ENTER:
				iType = itIP;
				break;
			case KEY_FUNC_AUX_LAN_GATEWAY_ENTER:
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

//判断数字按键按下时是否执行数字输入,如：按下sweep主键后，再按下数字键后屏幕第三部分不显示
bool mainForm::isNumberKeyInput(void)
{
	bool ret = false;
	switch (nowSubKey->funcCode)
	{
		//Freq
		case KEY_FUNC_FREQ:
		case KEY_FUNC_FREQ_GHZ:
		case KEY_FUNC_FREQ_MHZ:
		case KEY_FUNC_FREQ_KHZ:
		case KEY_FUNC_FREQ_HZ:
		case KEY_FUNC_FREQOFFSET:
		case KEY_FUNC_FREQOFFSET_GHZ:
		case KEY_FUNC_FREQOFFSET_MHZ:
		case KEY_FUNC_FREQOFFSET_KHZ:
		case KEY_FUNC_FREQOFFSET_HZ:
		case KEY_FUNC_FREQMULTIPLIER:
		case KEY_FUNC_FREQMULTIPLIER_ENTER:
			//Ampl
		case KEY_FUNC_AMPL:
		case KEY_FUNC_AMPL_DBM:
		case KEY_FUNC_AMPL_DBUV:
		case KEY_FUNC_AMPL_MV:
		case KEY_FUNC_AMPLOFFSET:
		case KEY_FUNC_AMPLOFFSET_DBM:
		case KEY_FUNC_AMPL_CABLE:
		case KEY_FUNC_AMPL_CABLE_DB:
			//Sweep
		case KEY_FUNC_SWEEP_STEP_FREQSTART:
		case KEY_FUNC_SWEEP_STEP_FREQSTART_GHZ:
		case KEY_FUNC_SWEEP_STEP_FREQSTART_MHZ:
		case KEY_FUNC_SWEEP_STEP_FREQSTART_KHZ:
		case KEY_FUNC_SWEEP_STEP_FREQSTART_HZ:
		case KEY_FUNC_SWEEP_STEP_FREQSTOP:
		case KEY_FUNC_SWEEP_STEP_FREQSTOP_GHZ:
		case KEY_FUNC_SWEEP_STEP_FREQSTOP_MHZ:
		case KEY_FUNC_SWEEP_STEP_FREQSTOP_KHZ:
		case KEY_FUNC_SWEEP_STEP_FREQSTOP_HZ:
		case KEY_FUNC_SWEEP_STEP_AMPLSTART:
		case KEY_FUNC_SWEEP_STEP_AMPLSTART_DBM:
		case KEY_FUNC_SWEEP_STEP_AMPLSTOP:
		case KEY_FUNC_SWEEP_STEP_AMPLSTOP_DBM:
		case KEY_FUNC_SWEEP_STEP_MORE_POINTS:
		case KEY_FUNC_SWEEP_STEP_MORE_POINTS_ENTER:
		case KEY_FUNC_SWEEP_STEP_MORE_STEPDWELL:
		case KEY_FUNC_SWEEP_STEP_MORE_STEPDWELL_S:
		case KEY_FUNC_SWEEP_STEP_MORE_STEPDWELL_MS:
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_EDITITEM:
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_EDITITEM_GHZS:
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_EDITITEM_MHZMS:
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_EDITITEM_KHZNS:
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_EDITITEM_HZDBM:
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_GOTOROW:
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_GOTOROW_ENTER:
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_MORE_DOWNLOAD:
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_MORE_DOWNLOAD_ENTER:
			//AM,FM,PM
		case KEY_FUNC_FCTN_ANALOG_AM_DEPTH:
		case KEY_FUNC_FCTN_ANALOG_AM_DEPTH_PERCENT:
		case KEY_FUNC_FCTN_ANALOG_AM_RATE:
		case KEY_FUNC_FCTN_ANALOG_AM_RATE_KHZ:
		case KEY_FUNC_FCTN_ANALOG_AM_RATE_HZ:
		case KEY_FUNC_FCTN_ANALOG_FM_DEVIATION:
		case KEY_FUNC_FCTN_ANALOG_FM_DEVIATION_MHZ:
		case KEY_FUNC_FCTN_ANALOG_FM_DEVIATION_KHZ:
		case KEY_FUNC_FCTN_ANALOG_FM_DEVIATION_HZ:
		case KEY_FUNC_FCTN_ANALOG_FM_RATE:
		case KEY_FUNC_FCTN_ANALOG_FM_RATE_KHZ:
		case KEY_FUNC_FCTN_ANALOG_FM_RATE_HZ:
		case KEY_FUNC_FCTN_ANALOG_PM_PHASE:
		case KEY_FUNC_FCTN_ANALOG_PM_PHASE_ANGLE:
		case KEY_FUNC_FCTN_ANALOG_PM_RATE:
		case KEY_FUNC_FCTN_ANALOG_PM_RATE_KHZ:
		case KEY_FUNC_FCTN_ANALOG_PM_RATE_HZ:
			//Digital
		case KEY_FUNC_FCTN_DIGITAL_PARAMETER_ASKDEEP:
		case KEY_FUNC_FCTN_DIGITAL_PARAMETER_ASKDEEP_PERCENT:
		case KEY_FUNC_FCTN_DIGITAL_PARAMETER_FSKOFFSET:
		case KEY_FUNC_FCTN_DIGITAL_PARAMETER_FSKOFFSET_MHZ:
		case KEY_FUNC_FCTN_DIGITAL_PARAMETER_FSKOFFSET_KHZ:
		case KEY_FUNC_FCTN_DIGITAL_PARAMETER_FSKOFFSET_HZ:
		case KEY_FUNC_FCTN_DIGITAL_PARAMETER_PSKDEVI:
		case KEY_FUNC_FCTN_DIGITAL_PARAMETER_PSKDEVI_ANGLE:
		case KEY_FUNC_FCTN_DIGITAL_PARAMETER_RATE:
		case KEY_FUNC_FCTN_DIGITAL_PARAMETER_RATE_KHZ:
		case KEY_FUNC_FCTN_DIGITAL_PARAMETER_RATE_HZ:
		case KEY_FUNC_FCTN_DIGITAL_MORE_LOAD_ENTER:
			//Pulse
		case KEY_FUNC_FCTN_PLUSE_PERIOD:
		case KEY_FUNC_FCTN_PLUSE_PERIOD_S:
		case KEY_FUNC_FCTN_PLUSE_PERIOD_MS:
		case KEY_FUNC_FCTN_PLUSE_PERIOD_US:
		case KEY_FUNC_FCTN_PLUSE_PERIOD_NS:
		case KEY_FUNC_FCTN_PLUSE_WIDTH:
		case KEY_FUNC_FCTN_PLUSE_WIDTH_S:
		case KEY_FUNC_FCTN_PLUSE_WIDTH_MS:
		case KEY_FUNC_FCTN_PLUSE_WIDTH_US:
		case KEY_FUNC_FCTN_PLUSE_WIDTH_NS:
			//LF
		case KEY_FUNC_FCTN_LF_AMPL:
		case KEY_FUNC_FCTN_LF_AMPL_VPP:
		case KEY_FUNC_FCTN_LF_AMPL_MVPP:
		case KEY_FUNC_FCTN_LF_FREQ:
		case KEY_FUNC_FCTN_LF_FREQ_MHZ:
		case KEY_FUNC_FCTN_LF_FREQ_KHZ:
		case KEY_FUNC_FCTN_LF_FREQ_HZ:
			//Extre
		case KEY_FUNC_FCTN_EXTRF_FREQ:
		case KEY_FUNC_FCTN_EXTRF_FREQ_GHZ:
		case KEY_FUNC_FCTN_EXTRF_FREQ_MHZ:
		case KEY_FUNC_FCTN_EXTRF_FREQ_KHZ:
		case KEY_FUNC_FCTN_EXTRF_FREQ_HZ:
			//DRG
		case KEY_FUNC_FCTN_DRG_UPLIMIT:
		case KEY_FUNC_FCTN_DRG_UPLIMIT_GHZ:
		case KEY_FUNC_FCTN_DRG_UPLIMIT_MHZ:
		case KEY_FUNC_FCTN_DRG_UPLIMIT_HZ:
		case KEY_FUNC_FCTN_DRG_DOWNLIMIT:
		case KEY_FUNC_FCTN_DRG_DOWNLIMIT_GHZ:
		case KEY_FUNC_FCTN_DRG_DOWNLIMIT_MHZ:
		case KEY_FUNC_FCTN_DRG_DOWNLIMIT_KHZ:
		case KEY_FUNC_FCTN_DRG_DOWNLIMIT_HZ:
		case KEY_FUNC_FCTN_DRG_POINTS:
		case KEY_FUNC_FCTN_DRG_POINTS_ENTER:
		case KEY_FUNC_FCTN_DRG_MORE_RATE:
		case KEY_FUNC_FCTN_DRG_MORE_RATE_MS:
		case KEY_FUNC_FCTN_DRG_MORE_RATE_US:
		case KEY_FUNC_FCTN_DRG_MORE_RATE_NS:
			//AUX
		case KEY_FUNC_AUX_LAN_IP:
		case KEY_FUNC_AUX_LAN_IP_ENTER:
		case KEY_FUNC_AUX_LAN_MASK:
		case KEY_FUNC_AUX_LAN_MASK_ENTER:
		case KEY_FUNC_AUX_LAN_GATEWAY:
		case KEY_FUNC_AUX_LAN_GATEWAY_ENTER:
		case KEY_FUNC_AUX_SERVICE:
			ret = true;
			break;
		default:
			ret = false;
			break;
	}
	return ret;
}

//幅度和频率的相关设置后，需要将按键设置到隐藏的按键上，即频率设置按键或幅度设置按键
//频率参考设置和幅度参考设置已在其他地方处理，故不能再此处处理
bool mainForm::isNeedReturnFreqOrAmplSet(void)
{
	bool ret;
	switch (nowSubKey->funcCode)
	{
		case KEY_FUNC_FREQREFSTATE:
		case KEY_FUNC_FREQOFFSET:
			//		case KEY_FUNC_FREQMULTIPLIER:
		case KEY_FUNC_FREQREF10M:
			sysData.inputLength = MAXFREQINPUTLENGTH; //设置可输入参数的长度
			ret = true;
			break;
		case KEY_FUNC_FREQMULTIPLIER:
			if (sysData.freq.multiplier >= 1 && sysData.freq.multiplier < 10)
			{
				sysData.inputLength = MAXFREQINPUTLENGTH;
			} else if (sysData.freq.multiplier >= 10 && sysData.freq.multiplier <= 100)
			{
				sysData.inputLength = MAXFREQINPUTLENGTH + 1;
			} else
			{
				sysData.inputLength = MAXFREQINPUTLENGTH;
			}

			ret = true;
			break;
		case KEY_FUNC_AMPLREFSTATE:
		case KEY_FUNC_AMPLOFFSET:
		case KEY_FUNC_AMPL_CABLE:

			ret = true;
			break;
		default:
			ret = false;
			break;
	}

	return ret;
}

//按下返回键后，清除参数输入栏显示区，如：在步进扫描值，设置起始频率后，需要清除参数输入栏显示区的"Start Freq:"
void mainForm::cleanInputParamter(void)
{
	switch (nowSubKey->funcCode)
	{
		case KEY_FUNC_SWEEP_CONFIGSTEP:

		case KEY_FUNC_FCTN_ANALOG_AM:
		case KEY_FUNC_FCTN_ANALOG_FM:
		case KEY_FUNC_FCTN_ANALOG_PM:
		case KEY_FUNC_FCTN_DIGITAL_PARAMETER:
		case KEY_FUNC_FCTN_PLUSE:
		case KEY_FUNC_FCTN_LF:
		case KEY_FUNC_FCTN_DRG:
		case KEY_FUNC_FCTN_EXTRF:
		case KEY_FUNC_AUX_LAN:
			strcpy(sysData.name, "");
			strcpy(sysData.value, "");
			strcpy(sysData.unit, "");
			drawInputParamterNoParamter();
			break;

		case KEY_FUNC_SWEEP_MORE_CONFIGLIST:
			strcpy(sysData.name, "");
			strcpy(sysData.value, "");
			strcpy(sysData.unit, "");
			drawSweep(sysData.sweep.type); //列表扫描，回到“扫描/列表状态信息”界面
			break;
		default:
			break;
	}
}

//解析列表数据文件
void mainForm::analysisListdata(int num)
{
	if (sysScpi->isUpdating)
	{
		return;
	}
	//  QString filename = QCoreApplication::applicationDirPath() + "/ListData/listdata.txt";
	//  QString filename = "/mnt/usb/ListData/" + listDataEntry.at(num-1);		//U盘挂载后的列表数据文件的位置
	int tempNum = num;
	if (tempNum > listDataEntry.count()) //输入的序号大于总文件数，则返回
	{
		return;
	} else if (tempNum < 1) //输入的序号小于1时返回
	{
		return;
	}

	QString folder = "";
	/* 寻找文件夹名 */
	for (int i = 0; i < sizeof Upan / sizeof Upan[0]; i++)
	{
		if (Upan[i].num == LISTDATAENTRY)
		{
			folder = QString(Upan[i].folder).trimmed();
			break;
		}
	}
	if (folder == "") //没有寻找到文件则返回
	{
		return;
	}

	//  QString filename = QCoreApplication::applicationDirPath() + "/ListData/" + listDataEntry.at(tempNum - 1);		//U盘挂载后的列表数据文件的位置
	QString filename = QCoreApplication::applicationDirPath() + "/" + folder + "/" + listDataEntry.at(tempNum - 1); //U盘挂载后的列表数据文件的位置
	QFile file(filename);

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		return;
	}
	QTextStream in(&file);
	QStringList dataList;
	QString temp;
	int i = 0;
	QString line = in.readLine(); //.txt文档的第一行是“序号”“频率”等信息，所以需要先读一次

	memset(&sysData.sweep.list, 0, sizeof(sysData.sweep.list)); //清空原本的列表信息
	while (!in.atEnd())
	{
		line = in.readLine(); //一行一行的读
		dataList = line.split(QChar(0x09), QString::SkipEmptyParts); //0x09是'\t'，每一行例，数据与数据间使用'\t'隔开；具体可使用"hexdump -c 文件名"和"hexdump -C 文件名"查看
		temp = dataList.at(1); //频率
		sysData.sweep.list.data[i].freq = StrToValue(temp.trimmed().toUpper());
		if (sysData.sweep.list.data[i].freq > MAXFREQ)
		{
			sysData.sweep.list.data[i].freq = MAXFREQ;
		} else if (sysData.sweep.list.data[i].freq < MINFREQ)
		{
			sysData.sweep.list.data[i].freq = MINFREQ;
		}

		temp = dataList.at(2); //幅度
		sysData.sweep.list.data[i].ampl = StrToValue(temp);
		if (sysData.sweep.list.data[i].ampl > MAXAMPL)
		{
			sysData.sweep.list.data[i].ampl = MAXAMPL;
		} else if (sysData.sweep.list.data[i].ampl < MINAMPL)
		{
			sysData.sweep.list.data[i].ampl = MINAMPL;
		}

		temp = dataList.at(3); //驻留时间
		sysData.sweep.list.data[i].dwell = StrToValue(temp);
		if (sysData.sweep.list.data[i].dwell > MAXDWELL)
		{
			sysData.sweep.list.data[i].dwell = MAXDWELL;
		} else if (sysData.sweep.list.data[i].dwell < MINDWELL)
		{
			sysData.sweep.list.data[i].dwell = MINDWELL;
		}

		i++;
		if (i > 511)
		{
			break;
		}
	}
	sysData.sweep.list.allRow = i;
	sysData.sweep.list.displayPage = 0;
	if ((sysData.sweep.list.allRow % 4) > 0)
	{
		sysData.sweep.list.allPage = i / 4 + 1; //计算显示的总页数
	} else
	{
		sysData.sweep.list.allPage = i / 4;
	}

}

//解析数字调制调制序列数据文件
void mainForm::analysisDigiSequencedata(int num)
{
	if (sysScpi->isUpdating)
	{
		return;
	}
	int tempNum = num;
	if (tempNum > listDataEntry.count()) //输入的序号大于总文件数，则返回
	{
		return;
	} else if (tempNum < 1) //输入的序号小于1时返回
	{
		return;
	}

	QString folder = "";
	/* 寻找文件夹名 */
	for (int i = 0; i < sizeof Upan / sizeof Upan[0]; i++)
	{
		if (Upan[i].num == DIGITALDATAENTRY)
		{
			folder = QString(Upan[i].folder).trimmed();
			break;
		}
	}
	if (folder == "") //没有寻找到文件则返回
	{
		return;
	}

	QString filename = QCoreApplication::applicationDirPath() + "/" + folder + "/" + listDataEntry.at(tempNum - 1); //U盘挂载后的列表数据文件的位置
	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		return;
	}
	QTextStream in(&file);
	int i = 0;
	QString line = "";
	memset(&sysData.digital.sequence, 0, sizeof(sysData.digital.sequence)); //清空原本的序列值
	sysData.digital.sequenceCntBefore = 0;
	while (!in.atEnd())
	{
		line = in.readLine(8).simplified(); //8个一行的读,去掉\t等字符

		if (line.length() > 0)
		{
			sysData.digital.sequenceCntBefore += line.length();
			sysData.digital.sequence[i] = bqstringToUchar(line); //转换用字符0,1表示的序列为8位无符号整型
			i++;
		}
		if (i > 3072)
		{
			break;
		}
	}
	sysData.digital.sequenceCnt = i; //保存序列转换后的个数
}

//读USB中参数num对应的文件夹名中的文件目录
bool mainForm::readUSBFileEntry(unsigned long num)
{
	if (sysScpi->isUpdating)
	{
		return false;
	}

	int errNum = 0;
	sysScpi->drawWin4("Load data file...");

	for (int i = 0; i < 10; i++)
	{
		if (system("mount -t vfat /dev/sda1 /mnt/usb/") == 0)
		{
			break;
		}
		usleep(1000 * 1000);
		if (i == 9)
		{
			sysScpi->drawWin4("Load data file failed.");
			return false;
		}
	}

	QString folder = "";
	/* 寻找文件夹名 */
	for (int i = 0; i < sizeof Upan / sizeof Upan[0]; i++)
	{
		if (Upan[i].num == num)
		{
			folder = QString(Upan[i].folder).trimmed();
			break;
		}
	}
	if (folder == "") //没有寻找到文件则返回
	{
		return false;
	}

	QString filedir = QCoreApplication::applicationDirPath() + "/" + folder; //本地存放数据的文件夹
	QDir dir(filedir);
	QString uFolderName;
	uFolderName = "/mnt/usb/" + folder; //挂载U盘后数据文件的目录
	QDir udir(uFolderName);
	QString tempStr = "";

	if (!dir.exists()) //判断文件夹是否存在，若不存在则新建
	{
		QString temp = "mkdir " + filedir;
		system(temp.toStdString().c_str());
	}
	if (udir.exists()) //判断U盘挂载成功，若成功，则删除本地的文件夹下的全部文件
	{
		tempStr = "rm " + filedir + "/*";
		system(tempStr.toStdString().c_str());
	}
	tempStr = "cp " + uFolderName + "/* " + filedir;
	if (system(tempStr.toStdString().c_str()) != 0) //复制数据文件到本地
	{
		system("umount /mnt/usb/");
		return false;
	}

	system("umount /mnt/usb/"); //卸载U盘

	dir.setFilter(QDir::NoDotAndDotDot | QDir::Files); //设置文件显示过滤，不显示“.”和“..”

	listDataEntry = dir.entryList(); //获取目录下各文件名

	return true;
}

//绘制USB中数据目录
void mainForm::drawUSBFileEntry(unsigned long num)
{
	if (sysScpi->isUpdating)
	{
		return;
	}
	if (readUSBFileEntry(num) == false)
	{
		return;
	}

	QRect rc = QRect(WIN4X + 2 * BORDERWIDTH, WIN4Y, WIN4WIDTH - 2 * BORDERWIDTH - 2 * WIN4X, WIN4HEIGHT);
	QRect rcTemp[2][16];
	QRect rcTitle[2]; //标题栏分为两列
	QPainter p;
	QPixmap pix(rc.size());
	int row = 6;

	if (!pix.isNull())
	{
		p.begin(&pix);
		//计算绘制空间
		QRect rcTemp1 = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width(), pix.rect().height());
		QRect rcPara = QRect(pix.rect().x() + 30, pix.rect().y() + pix.rect().height() / 4, pix.rect().width(), pix.rect().height() / row * 4);
		rcTitle[0] = QRect(pix.rect().x() - 2, pix.rect().y(), pix.rect().width() + 2 - 60, pix.rect().height() / row); //标题区0
		rcTitle[1] = QRect((pix.rect().x() + pix.rect().width() + 2 - 60) - 2, pix.rect().y(), 60, pix.rect().height() / row); //标题区

		p.fillRect(rcTemp1, Qt::white);
		p.fillRect(rcTitle[0], BACKCOLORWIN4);
		p.fillRect(rcTitle[1], BACKCOLORWIN4);

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 2; j++)
			{
				rcTemp[j][i] = QRect(rcPara.x() + j * (rcPara.width() - 30) / 2, rcPara.y() + i * rcPara.height() / 4, rcPara.width() / 2, rcPara.height() / 4);
			}
		}

		p.setPen(Qt::black);
		QString displayString, QtempV, QtempU, tempValue;

		//绘制标题栏
		if (sysData.system.isLanguageEn)
		{
			p.setFont(fontEnglishBold);
		} else
		{
			p.setFont(fontChinesePara);
		}
		displayString = "Data File Information";
		p.drawRect(rcTitle[0]);
		p.drawText(rcTitle[0].x() + 20, rcTitle[0].y(), rcTitle[0].width(), rcTitle[0].height(), Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayString));
		p.setFont(fontEnglishBold);
		displayString = "1/1";
		p.drawRect(rcTitle[1]);
		p.drawText(rcTitle[1].x() + 20, rcTitle[1].y(), rcTitle[1].width(), rcTitle[1].height(), Qt::AlignLeft | Qt::AlignVCenter, displayString);

		for (int i = 0; i < listDataEntry.count(); i++) //绘制序号和文件名
		{
			if (i < 4)
			{
				QtempV = doubleToQValue((double) i + 1);
				displayString = QtempV + ".\t" + listDataEntry.at(i);
				p.drawText(rcTemp[0][i], Qt::AlignLeft | Qt::AlignVCenter, displayString);
			} else
			{
				QtempV = doubleToQValue((double) i + 1);
				displayString = QtempV + ".\t" + listDataEntry.at(i);
				p.drawText(rcTemp[1][i - 4], Qt::AlignLeft | Qt::AlignVCenter, displayString);
			}

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
}//

