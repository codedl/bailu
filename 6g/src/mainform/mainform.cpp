#include "mainform.h"

#define BAUDRATE B115200
#define KBD_PORT "/dev/ttyS2"

//=====================================================================
//界面布局分布：
//		菜单栏左边的部分为 win1, 菜单栏为win2
//		win1从上到下分为 win1_1, win1_2, win1_3, win1_4, win1_5
//		win1_1分为win1_1_1和win1_1_2
//				win1_1_1是绘制频率主参数, win1_1_2是绘制幅度主参数，
//					对应的参数名是 drawMainPara, drawMainParaFreq, drawMainParaAmptd
//		win1_2是状态显示区，对应的参数名是drawState
//		win1_3是输入参数显示区，对应的参数名是 drawInputParamter
//		win1_4是功能参数显示区，对应的参数名由各功能决定
//		win1_5是系统参数显示区，显示时间等，对应的参数名是 drawSystemInf
//=====================================================================


int keyCount;

//构造函数
mainForm::mainForm(QWidget* parent) :
	QWidget(parent = 0)
{
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
	sysMenu = new tSysMenu("/home/sky/SG1060.xml");
	nowMainKey = sysKey;
	
	nowSubKey = nowMainKey->rightNode;
	HaveMoreKeyCount();
	connect(sysScpi, SIGNAL(drawMainPara(void)), this, SLOT(drawFreqAndAmplMainSlots(void)));
	connect(sysScpi, SIGNAL(drawState(void)), this, SLOT(drawStateSlots(void)));
	connect(sysScpi, SIGNAL(drawInputParamter(void)), this, SLOT(drawInputParamterSlots(void)));
	connect(sysScpi, SIGNAL(drawProcess(void)), this, SLOT(drawProcessSlots(void)));
	connect(sysScpi, SIGNAL(drawsListData(void)), this, SLOT(drawsListDataSlots(void)));
	connect(sysScpi, SIGNAL(drawProgBar(bool, float)), this, SLOT(drawProgBarSlots(bool, float)));
	connect(sysScpi, SIGNAL(drawDownloadProg(bool, int, int)), this, SLOT(drawDownloadProgSlots(bool, int, int)));
	connect(sysScpi, SIGNAL(drawDigitalModulate(void)), this, SLOT(drawDigitalModulateSlots(void)));
	connect(sysScpi, SIGNAL(drawPulseModulate(void)), this, SLOT(drawPluseModulateSlots(void)));
	connect(sysScpi, SIGNAL(drawSystemPara(void)), this, SLOT(drawSystemParaSlots(void)));
	connect(sysScpi, SIGNAL(drawFreqInParamter(void)), this, SLOT(drawFreqInParamterSlots(void)));
	connect(sysScpi, SIGNAL(drawAmplInParamter(void)), this, SLOT(drawAmplInParamterSlots(void)));
	connect(sysScpi, SIGNAL(drawSweep(int)), this, SLOT(drawSweepSlots(int)));
	connect(sysScpi, SIGNAL(drawExfFref(void)), this, SLOT(drawExfFrefSlots(void)));
	connect(sysScpi, SIGNAL(drawArbitraryWave(void)), this, SLOT(drawArbitraryWaveSlots(void)));
	connect(sysScpi, SIGNAL(drawDrgFunction(void)), this, SLOT(drawDrgFunctionSlots(void)));
	connect(sysScpi, SIGNAL(drawUSBPower(void)), this, SLOT(drawUSBPowerSlots(void)));
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
	showSplashScreen();
	evaDay = -1;
	evaIndex = 0;
	memset(evaArray, 0x0, sizeof evaArray);

	//默认选中频率
	readKeyBoardData();

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
	//将电源指示灯变绿
	sysScpi->keyBoardLEDSwitch(1, true);
	//sysScpi->keyBoardLEDSwitch(2, true);//启动后在频率菜单下，由于有More菜单所以需要点亮
}

//绘制SCPI状态
void mainForm::refreshScpiState(QString helpStr)
{
	if (!helpStr.isNull() && !helpStr.isEmpty() && helpStr != helpContent)
	{
		helpContent = helpStr;
		//	drawWin1_5();
	}
}

void mainForm::drawSelect(QPainter* painter, QPixmap* pixmap, QRect rc, QString str, int index)
{
	//  printf("str = %s\n", str.toStdString().c_str());
	/* zpc add test */
	//if (index == -1)
	//	return;
	/* add test end */

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


//绘制主参数显示区的频率显示区（win1_1_1）
void mainForm::drawMainParaFreq(void)
{
	if (sysScpi->isUpdating)
	{
		return;
	}
	//------------------------------------------------内存图像绘制------------------------------------------------------------
	QRect rc = QRect(WIN1_1_1X, WIN1_1_1Y, WIN1_1_1WIDTH, WIN1_1_1HEIGHT);				//win1_1_1整个区域
	QPainter p;
	QPixmap pix(rc.size());
	char tempChar[64] = {};

	if (!pix.isNull())
	{
		double upMidPercent = 0.35;
		double downMidPercent = 0.4;
		double namePercent = 0.3;
		p.begin(&pix);
		p.fillRect(pix.rect(), BACKCOLORWIN1); //填充窗口1全部区域的背景色
		//绘制“Frequency”字符串
		p.drawLine(QPointF(0, 0), QPointF(rc.width() * upMidPercent + 2, 0));
		p.drawLine(QPointF(rc.width() * upMidPercent + 2, 0), QPointF(rc.width() * downMidPercent, rc.height() * namePercent));
		QRect rcName = QRect(0, 2, rc.width() * upMidPercent, rc.height() * namePercent);
		p.fillRect(rcName, BACKCOLORWIN1);
		p.setFont(fontEnglishSmallBold);
		p.drawText(rcName, Qt::AlignLeft | Qt::AlignVCenter, QString("FREQUENCY"));	//绘制“Frequency”字符串
		//绘制Frequency的各种状态，如参考、偏置等
		QPointF statusPoints[] = {
			QPointF(rc.width() * upMidPercent + 2, 0),
			QPointF(rc.width(), 0),
			QPointF(rc.width(), rc.height() * namePercent),
			QPointF(rc.width() * downMidPercent, rc.height() * namePercent)
		};
		p.setBrush(QBrush(QColor(0, 0, 255)));
		p.drawPolygon(statusPoints, 4);	//绘制四边形框
		//绘制REF
		if (sysData.freq.ref.state == true)
		{
			if (sysData.freq.ref.value > 0)
			{
				QRect rcRef = QRect(rc.width() * 0.45, 1, rc.width() * 0.15, rc.height() * 0.3);
				p.fillRect(rcRef, BACKCOLORREFOFFSET);
				//qDrawPlainRect(&p, rcRef, GRIDCOLOR, 2, 0);
				p.drawText(rcRef, Qt::AlignCenter, QString("REF"));
			}
		}
		//绘制Offset
		if (sysData.freq.offset > 0)
		{
			QRect rcOffset = QRect(rc.width() * 0.65, 1, rc.width() * 0.25, rc.height() * 0.3);
			p.fillRect(rcOffset, BACKCOLORREFOFFSET);
			//qDrawPlainRect(&p, rcRef, GRIDCOLOR, 2, 0);
			p.drawText(rcOffset, Qt::AlignCenter, QString("OFFSET"));
		}
		//绘制频率数值
		if (sysData.sweep.mode == smAmpl || sysData.sweep.mode == smOff) //当扫描模式是“关”或“扫幅”时，频率显示黑色，否则灰度处理
		{
			p.setPen(Qt::black);
		}
		else
		{
			p.setPen(Qt::gray);
		}
		p.setFont(fontWin1En);
		QRect rcFreqValue = QRect(0, rc.height() * 0.3, rc.width() * 0.8, rc.height() * 0.7);	//绘制频率数值
		strcpy(tempChar, doubleToQValue(sysData.freq.freq).toStdString().c_str());
		p.drawText(rcFreqValue, Qt::AlignRight | Qt::AlignVCenter, insertThousandsCH(tempChar).trimmed());
		p.setFont(QFont("enb", 16, 75, false));
		QRect rcFreqUnit = QRect(rc.width() * 0.8, rc.height() * 0.3, rc.width() * 0.2, rc.height() * 0.7);		//绘制频率单位
		p.drawText(rcFreqUnit, Qt::AlignRight | Qt::AlignBottom, freqToQUnit(sysData.freq.freq));	
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
//绘制主参数显示区的频幅度显示区（win1_1_2）
void mainForm::drawMainParaAmptd(void)
{
	if (sysScpi->isUpdating)
	{
		return;
	}

	//------------------------------------------------内存图像绘制------------------------------------------------------------
	QRect rc = QRect(WIN1_1_2X + 1, WIN1_1_2Y, WIN1_1_2WIDTH - 1, WIN1_1_2HEIGHT); //窗口1全部区域
	QPainter p;
	QPixmap pix(rc.size());
	char tempChar[64] = {};
	if (!pix.isNull())
	{
		double upMidPercent = 0.45;
		double downMidPercent = 0.5;
		double namePercent = 0.3;
		p.begin(&pix);
		p.fillRect(pix.rect(), BACKCOLORWIN1); //填充窗口1全部区域的背景色
		QRect rcName = QRect(0, 2, rc.width() * upMidPercent, rc.height() * namePercent);
		if (sysData.led.RF1)	//根据RF状态绘制
		{
			p.fillRect(rcName, BACKCOLORWIN1);
			p.setFont(fontEnglishSmallBold);
			p.drawText(rcName, Qt::AlignLeft | Qt::AlignVCenter, QString("AMPLITUDE"));	//绘制“Amplitude”字符串
		}
		else
		{
			p.fillRect(QRect(rcName.x() + 5, rcName.y(), rcName.width() - 10, rcName.height()), QColor(255, 255, 0));
			p.drawRect(QRect(rcName.x() + 5, rcName.y(), rcName.width() - 10, rcName.height()));
			p.setFont(fontEnglishSmallBold);
			p.drawText(rcName, Qt::AlignCenter, QString("RF OFF"));	//绘制“Amplitude”字符串
		}
		//绘制Frequency的各种状态，如参考、偏置等
		QPointF statusPoints[] = {
			QPointF(rc.width() * upMidPercent, 0),
			QPointF(rc.width(), 0),
			QPointF(rc.width(), rc.height() * namePercent),
			QPointF(rc.width() * downMidPercent, rc.height() * namePercent)
		};
		p.setBrush(QBrush(QColor(0, 0, 255)));
		p.drawPolygon(statusPoints, 4);	//绘制四边形框
		if (sysData.ampl.ref.state)
		{
			if (sysData.ampl.ref.value >= -120)
			{
				QRect rcRef = QRect(rc.width() * downMidPercent + 5, 1, rc.width() * 0.2, rc.height() * namePercent);
				p.fillRect(rcRef, BACKCOLORREFOFFSET);
				p.drawText(rcRef, Qt::AlignCenter, QString("REF"));
			}
		}
		if (sysData.ampl.offset > 0)
		{
			QRect rcOffset = QRect(rc.width() * (downMidPercent + 0.25) + 5, 1, rc.width() * 0.2, rc.height() * namePercent);
			p.fillRect(rcOffset, BACKCOLORREFOFFSET);
			p.drawText(rcOffset, Qt::AlignCenter, QString("OFF"));
		}

		//绘制幅度值
		QRect rcValue = QRect(1, rc.height() * namePercent, rc.width() * 0.7, rc.height() * (1 - namePercent));
		if (sysData.led.RF1 == true) //当RF状态是开的时候，当扫描模式是“关”或“扫频”时，幅度显示黑色，否则灰度处理
		{
			if (sysData.sweep.mode == smOff || sysData.sweep.mode == smFreq)
			{
				p.setPen(Qt::black);
			}
			else
			{
				p.setPen(Qt::gray);
			}
		}
		else //RF关闭时，需要灰度处理
		{
			p.setPen(Qt::gray);
		}
		p.setFont(fontWin1En);
		sprintf(tempChar, "%.02f", sysData.ampl.ampl);
		p.drawText(rcValue, Qt::AlignRight | Qt::AlignVCenter, insertThousandsCH(tempChar).trimmed());
		//绘制幅度单位
		p.setFont(QFont("enb", 16, 75, false));
		QRect rcUnit = QRect(rcValue.right(), rcValue.y(), rc.width() - rcValue.width(), rcValue.height());
		if (sysData.ampl.ref.state)
		{
			p.drawText(rcUnit, Qt::AlignRight | Qt::AlignBottom | Qt::AlignVCenter, "dB");
		}
		else
		{
			switch (sysData.ampl.amplUnit)
			{
				case audBm:
					p.drawText(rcUnit, Qt::AlignRight | Qt::AlignBottom | Qt::AlignVCenter, "dBm");
					break;
				case audBuV:
					p.drawText(rcUnit, Qt::AlignRight | Qt::AlignBottom | Qt::AlignVCenter, "dBuV");
					break;
				case aumV:
					p.drawText(rcUnit, Qt::AlignRight | Qt::AlignBottom | Qt::AlignVCenter, "mV");
					break;
				default:
					break;
			}
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
//绘制主参数显示区
void mainForm::drawMainPara(void)
{
	drawMainParaFreq();
	drawMainParaAmptd();
}

void mainForm::drawFreqAndAmplMainSlots(void)
{
	drawMainPara();
}

//绘制状态显示区，即 win1_2
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
		QRect rc = QRect(WIN1_2X + 1, WIN1_2Y + 1, WIN1_2WIDTH - 2, WIN1_2HEIGHT - 1); //窗口2全部区域
		QRect rcRemoteState;
		QRect rcRfState;
		QRect rcModeState;
		QRect rcLfState;
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
			QColor bulue = QColor(0, 0, 255);
			p.fillRect(pix.rect(), bulue); //填充窗口2全部区域的背景色
			//p.drawRect(rc);
			p.drawLine(QPointF(rc.x(), rc.y()), QPointF(rc.x(), rc.height()));
			p.drawLine(QPointF(rc.width(), rc.y()), QPointF(rc.width(), rc.height()));
			p.drawLine(QPointF(rc.x(), rc.height()), QPointF(rc.width(), rc.height()));
			//远程控制
			if (sysData.scpiData.stateR)
			{
				p.setPen(Qt::yellow);
				p.setFont(fontWin2En);
				p.drawText(rcRemoteState, Qt::AlignLeft | Qt::AlignHCenter, "R");
			}
			p.setPen(Qt::black);
			QColor yellow = QColor(255, 255, 0);
			//mode On off
			p.setFont(fontData);
			if (sysData.led.Mod)
			{
				p.drawRect(rcModeState);
				p.fillRect(rcModeState, yellow);
				p.drawText(rcModeState, Qt::AlignCenter, "MOD");
			}
			//LF On Off
			p.setFont(fontData);
			if (sysData.led.LFOut)
			{
				p.drawRect(rcLfState);
				p.fillRect(rcLfState, yellow);
				p.drawText(rcLfState, Qt::AlignCenter, "LF");
			}
			//RF on/Off
			p.setFont(fontData);
			if (sysData.led.RF1)
			{
				p.drawRect(rcRfState);
				p.fillRect(rcRfState, yellow);
				p.drawText(rcRfState, Qt::AlignCenter, "RF");
			}
			//ALC ON/OFF
			p.setFont(fontData);
			if (sysData.ampl.alc.state)
			{
				p.drawRect(rcALCState);
				p.fillRect(rcALCState, yellow);
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
	} 
	else //超过试用期
	{
		int evaNums = sizeof evaArray / sizeof evaArray[0];
		int evaWidth = 30;
		int evaHeight = 30;
		QRect rc[evaNums];
		QVector<QRect> rcS;
		char tempChar[32] = { 0x0 };
		QRect rcPoint = QRect(0, 0, WINALLWIDTH + 2, WINALLHEIGHT); //整个背景
		QRect rcEnter = QRect(WIN2X + 30, WIN2Y, WIN2WIDTH * 0.6, WIN2HEIGHT / 7); //确认键
		QPainter p;
		QPixmap pix(rcPoint.size()); //整个背景

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
	}
}

void mainForm::drawStateSlots(void)
{
	drawState();
}

//绘制时间等相关信息即 win1_5
void mainForm::drawSystemInf(void)
{
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
		QRect rc = QRect(WIN1_5X + 1, WIN1_5Y, WIN1_5WIDTH - 2, WIN1_5HEIGHT + 1); //窗口2全部区域
		QPainter p;
		QPixmap pix(rc.size());
		if (!pix.isNull())
		{
			p.begin(&pix);
			p.fillRect(pix.rect(), BACKCOLORWIN3); //填充窗口2全部区域的背景色
			p.drawRect(rc);
			
			QRect rcTime = QRect(rc.width() * 0.7, 0, rc.width() * 0.3, rc.height());
			QString timeStr = (QTime::currentTime()).toString("HH:mm:ss");	//获取系统时间
			p.drawText(rcTime, Qt::AlignRight | Qt::AlignVCenter, timeStr);
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
	else
	{

	}
}

void mainForm::drawSystemInfSlots(void)
{
	drawSystemInf();
}

//绘制输入栏显示区，即 win1_3
void mainForm::drawInputParamter(void)
{
	if (sysScpi->isUpdating)
	{
		return;
	}

	QRect rc = QRect(WIN1_3X, WIN1_3Y, WIN1_3WIDTH, WIN1_3HEIGHT);
	QPainter p;
	QPixmap pix(rc.size());
	QString displayString, tempValue;
	char tempChar[32];

	if (!pix.isNull())
	{
		p.begin(&pix);
		//参数名称显示区域
		QRect rcName = QRect(2, 0, rc.width() * 0.3, rc.height());
		//参数值显示区域
		QRect rcValue = QRect(rc.width() * 0.4, 0, rc.width() * 0.4, rc.height());
		//参数单位显示区域
		QRect rcUnit = QRect(rc.width() * 0.8, 0, rc.width() * 0.2, rc.height());
		
		p.fillRect(pix.rect(), BACKCOLORWIN3);
		p.drawLine(QPointF(0, 0), QPointF(0, rc.height()));		
		p.drawLine(QPointF(rc.width() - 1, rc.height()), QPointF(rc.width() - 1, 0));
		p.setPen(QColor(0, 0, 0));
		p.setFont(fontWin2En);
		//draw name
		p.drawText(rcName, Qt::AlignLeft | Qt::AlignVCenter, QString(sysData.name).trimmed());
		//draw value
		if (!QString(sysData.value).isEmpty())
		{
			drawSelect(&p, &pix, rcValue, QString(sysData.value).trimmed(), sysData.cursorLocation);
		}
		//draw unit
		p.drawText(rcUnit, Qt::AlignCenter, QString(sysData.unit).trimmed());

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

void mainForm::drawInputParamterSlots(void)
{
	drawInputParamter();
}

/********************************************************************/



/********************************************************************/
//绘制频率参数在参数显示区
void mainForm::drawFreqInParamter(void)
{
	if (sysScpi->isUpdating)
	{
		return;
	}

	QRect rc = QRect(WIN1_4X, WIN1_4Y, WIN1_4WIDTH - 1, WIN1_4HEIGHT - 1);
	QRect rcTemp[2][16];
	QRect rcTitle[2]; //标题栏分为两列
	QPainter p;
	QPixmap pix(rc.size());
	int row = 6;
	int titelRightWidth = 80;	//显示页码的宽度

	if (!pix.isNull())
	{
		p.begin(&pix);

		//计算绘制空间
		QRect rcTemp1 = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width(), pix.rect().height()); //整个参数区（标题和参数）
		QRect rcPara = QRect(pix.rect().x(), pix.rect().y() + pix.rect().height() / 4, pix.rect().width(), pix.rect().height() / row * 4); //参数区
		rcTitle[0] = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width() - titelRightWidth, pix.rect().height() / row); //标题区0
		rcTitle[1] = QRect(pix.rect().width()- titelRightWidth, pix.rect().y(), titelRightWidth, pix.rect().height() / row); //标题区

		p.fillRect(rcTemp1, Qt::white);
		p.fillRect(rcTitle[0], BACKCOLORWIN4);
		p.fillRect(rcTitle[1], BACKCOLORWIN4);
		p.drawRect(pix.rect());
		int interval = 10;
		int tempRows = 5;
		int tempCols = 2;
		for (int i = 0; i < tempRows; i++)
		{
			for (int j = 0; j < tempCols; j++)
			{
				rcTemp[j][i] = QRect(rcPara.x() + rcPara.width() / tempCols * j + interval, rcPara.y() + i * rcPara.height() / tempRows, rcPara.width() / tempCols - interval, rcPara.height() / tempRows);
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
		p.drawText(rcTitle[0].x() + 2, rcTitle[0].y(), rcTitle[0].width(), rcTitle[0].height(), Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayString));

		p.setFont(fontEnglishBold);
		displayString = "Page: 1/1";
		p.drawRect(rcTitle[1]);
		p.drawText(rcTitle[1].x(), rcTitle[1].y(), rcTitle[1].width(), rcTitle[1].height(), Qt::AlignCenter, displayString);

		////绘制频率参考状态
		////	p.setFont(fontEnglishLight);
		if (sysData.system.isLanguageEn)
		{
			p.setFont(fontEnglishBold);
		} else
		{
			p.setFont(fontChinesePara);
		}

		//if (sysData.freq.ref.state)
		//{
		//	displayString = "Freq Ref State:\tOn";
		//} else
		//{
		//	displayString = "Freq Ref State:\tOff";
		//}

		//p.drawText(rcTemp[0][0], Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayString));

		////绘制频率参考值
		//if (sysData.freq.ref.state)
		//	p.setPen(Qt::black); //根据状态判断画笔是否灰色处理
		//else
		//	p.setPen(Qt::gray);

		//QtempV = doubleToQValue(sysData.freq.ref.value);
		//QtempV = insertThousandsQS(QtempV);
		//QtempU = freqToQUnit(sysData.freq.ref.value);
		//if (sysData.system.isLanguageEn)
		//{
		//	displayString = "Freq Ref:\t" + QtempV + "\t" + QtempU;
		//} else
		//{
		//	displayString = "频率参考:\t" + QtempV + "\t" + QtempU;
		//}
		//p.drawText(rcTemp[1][1], Qt::AlignLeft | Qt::AlignVCenter, displayString);

		//p.setPen(Qt::black);

		//绘制频率偏移值
		QtempV = doubleToQValue(sysData.freq.offset);
		QtempV = insertThousandsQS(QtempV);
		QtempU = freqToQUnit(sysData.freq.offset);
		if (sysData.system.isLanguageEn)
		{
			displayString = "Freq Offset:\t" + QtempV + "\t" + QtempU;
		} else
		{
			displayString = "频率偏移:\t" + QtempV + "\t" + QtempU;
		}
		p.drawText(rcTemp[0][0], Qt::AlignLeft | Qt::AlignVCenter, displayString);

		////绘制频率倍数值
		//QtempV = doubleToQValue(sysData.freq.multiplier);
		//QtempV = insertThousandsQS(QtempV);
		//QtempU = "x";
		//if (sysData.system.isLanguageEn)
		//{
		//	displayString = "Multiplier:\t" + QtempV + "\t" + QtempU;
		//} else
		//{
		//	displayString = "倍频:\t" + QtempV + "\t" + QtempU;
		//}
		//p.drawText(rcTemp[1][0], Qt::AlignLeft | Qt::AlignVCenter, displayString);

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
		p.drawText(rcTemp[1][0], Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayString));
		//频率增量
		QtempV = doubleToQValue(sysData.incrSet.FreqIncrValue);
		QtempV = insertThousandsQS(QtempV);
		QtempU = freqToQUnit(sysData.incrSet.FreqIncrValue);
		displayString = getTheTranslatorString("Freq Incr") + ":\t" + QtempV + " " + QtempU;
		p.drawText(rcTemp[0][1], Qt::AlignLeft | Qt::AlignVCenter, displayString);

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
	QRect rc = QRect(WIN1_4X, WIN1_4Y, WIN1_4WIDTH - 1, WIN1_4HEIGHT - 1);
	QRect rcTemp[2][16];
	QRect rcTitle[2]; //标题栏分为两列
	QPainter p;
	QPixmap pix(rc.size());
	int row = 6;
	int titelRightWidth = 80;	//显示页码的宽度

	if (!pix.isNull())
	{
		p.begin(&pix);

		//计算绘制空间
		QRect rcTemp1 = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width(), pix.rect().height()); //整个参数区（标题和参数）
		QRect rcPara = QRect(pix.rect().x(), pix.rect().y() + pix.rect().height() / 4, pix.rect().width(), pix.rect().height() / row * 4); //参数区
		rcTitle[0] = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width() - titelRightWidth, pix.rect().height() / row); //标题区0
		rcTitle[1] = QRect(pix.rect().width() - titelRightWidth, pix.rect().y(), titelRightWidth, pix.rect().height() / row); //标题区

		p.fillRect(rcTemp1, Qt::white);
		p.fillRect(rcTitle[0], BACKCOLORWIN4);
		p.fillRect(rcTitle[1], BACKCOLORWIN4);
		int interval = 10;
		int tempRows = 5;
		int tempCols = 2;
		for (int i = 0; i < tempRows; i++)
		{
			for (int j = 0; j < tempCols; j++)
			{
				rcTemp[j][i] = QRect(rcPara.x() + rcPara.width() / tempCols * j + interval, rcPara.y() + i * rcPara.height() / tempRows, rcPara.width() / tempCols - interval, rcPara.height() / tempRows);
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
		p.drawText(rcTitle[0].x() + 2, rcTitle[0].y(), rcTitle[0].width(), rcTitle[0].height(), Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayString));

		p.setFont(fontEnglishBold);
		displayString = "Page: 1/1";
		p.drawRect(rcTitle[1]);
		p.drawText(rcTitle[1].x(), rcTitle[1].y(), rcTitle[1].width(), rcTitle[1].height(), Qt::AlignCenter, displayString);

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
		////绘制幅度参考状态
		//p.setPen(Qt::black);
		//if (sysData.ampl.ref.state)
		//{
		//	displayString = "Ampl Ref State:\tOn";
		//} else
		//{
		//	displayString = "Ampl Ref State:\tOff";
		//}

		//p.drawText(rcTemp[1][1], Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayString));

		////绘制幅度参考值
		//if (sysData.ampl.ref.state)
		//	p.setPen(Qt::black);
		//else
		//	p.setPen(Qt::gray);
		//QtempV.sprintf("%.2f", sysData.ampl.ref.value);

		////	if(sysData.ampl.ref.state)
		////	{
		////	  QtempU = "dB";
		////	}else
		//{
		//	QtempU = "dBm";
		//	;
		//}
		//if (sysData.system.isLanguageEn)
		//{
		//	displayString = "Ampl Ref:\t" + QtempV + "\t" + QtempU;
		//} else
		//{
		//	displayString = "幅度参考值：" + QtempV + "\t" + QtempU;
		//}

		//p.drawText(rcTemp[0][1], Qt::AlignLeft | Qt::AlignVCenter, displayString);
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
		p.drawText(rcTemp[0][1], Qt::AlignLeft | Qt::AlignVCenter, displayString);

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
		p.drawText(rcTemp[1][1], Qt::AlignLeft | Qt::AlignVCenter, displayString);

		//绘制幅度增量
		QtempV.sprintf("%.2f", sysData.incrSet.AmptdIncrValue);
		QtempU = "dB";
		displayString = getTheTranslatorString("Ampl Incr") + ":\t" + QtempV + " " + QtempU;
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

void mainForm::drawAmplInParamterSlots(void)
{
	drawAmplInParamter();
}

//绘制 process
void mainForm::drawProcess(void)
{
#if 0
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
#endif
}

//绘制扫描参数区
void mainForm::drawSweep(int type)
{
	if (sysScpi->isUpdating)
	{
		return;
	}

	QString tempQChar;

	QRect rc = QRect(WIN1_4X, WIN1_4Y, WIN1_4WIDTH - 1, WIN1_4HEIGHT - 1);
	QRect rcTemp[2][16];
	QRect rcTitle[2]; //标题栏分为两列
	QPainter p;
	QPixmap pix(rc.size());
	int row = 6;
	int titelRightWidth = 80;

	if (!pix.isNull())
	{
		p.begin(&pix);

		//计算绘制空间
		QRect rcTemp1 = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width(), pix.rect().height()); //整个参数区（标题和参数）
		QRect rcPara = QRect(pix.rect().x(), pix.rect().y() + pix.rect().height() / 4, pix.rect().width(), pix.rect().height() / row * 4); //参数区
		rcTitle[0] = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width() - titelRightWidth, pix.rect().height() / row); //标题区0
		rcTitle[1] = QRect(pix.rect().width() - titelRightWidth, pix.rect().y(), titelRightWidth, pix.rect().height() / row); //标题区

		p.fillRect(rcTemp1, Qt::white);
		p.fillRect(rcTitle[0], BACKCOLORWIN4);
		p.fillRect(rcTitle[1], BACKCOLORWIN4);
		p.drawRect(pix.rect());
		int interval = 10;
		
		p.setPen(Qt::black);
		QString displayString, QtempV, QtempU, tempValue;
		//绘制标题栏
		if (sysData.system.isLanguageEn)
		{
			p.setFont(fontEnglishBold);
		}
		else
		{
			p.setFont(fontChinesePara);
		}
		displayString = "Sweep/List Status Information";
		p.drawRect(rcTitle[0]);
		p.drawText(rcTitle[0].x() + 2, rcTitle[0].y(), rcTitle[0].width(), rcTitle[0].height(), Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayString));

		p.setFont(fontEnglishBold);
		tempValue = doubleToQValue(sysData.sweep.nowpagenum);  //将数字转换成QT字符串
		displayString ="Page: " + tempValue + "/2";
		p.drawRect(rcTitle[1]);
		p.drawText(rcTitle[1], Qt::AlignCenter, displayString);

		if (sysData.sweep.type == stStep)	//扫描类型是步进
		{
			//步进扫描时，第一页显示起始、终止频率和幅度，第二页显示其他信息
			if (sysData.sweep.nowpagenum == 1)
			{
				//计算方框
				for (int i = 0; i < 5; i++)
				{
					rcTemp[0][i] = QRect(rcPara.x() + interval, rcPara.y() + i * rcPara.height() / 5, rcPara.width() - interval, rcPara.height() / 5);
				}
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
				p.drawText(rcTemp[0][0], Qt::AlignLeft | Qt::AlignVCenter, displayString);

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
				p.drawText(rcTemp[0][1], Qt::AlignLeft | Qt::AlignVCenter, displayString);

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
				p.drawText(rcTemp[0][2], Qt::AlignLeft | Qt::AlignVCenter, displayString);

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
				p.drawText(rcTemp[0][3], Qt::AlignLeft | Qt::AlignVCenter, displayString);
			}
			else if (sysData.sweep.nowpagenum == 2)	//第二页显示其他信息
			{
				for (int i = 0; i < 5; i++)
				{
					for (int j = 0; j < 2; j++)
					{
						rcTemp[j][i] = QRect(rcPara.x() + rcPara.width() / 2 * j + interval, rcPara.y() + i * rcPara.height() / 5, rcPara.width() / 2 - interval, rcPara.height() / 5);
					}
				}
				//绘制扫描模式
				if (sysData.system.isLanguageEn)
				{
					p.setFont(fontEnglishBold);
				}
				else
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
				}
				else if (sysData.sweep.type == stStep)
				{
					displayString = "Type:\vStep";
					;
				}
				p.drawText(rcTemp[0][1], Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayString));

				//绘制扫描驻留时间
				p.setPen(Qt::black);
				QtempV = doubleToQValue(sysData.sweep.step.dwell);
				QtempV = insertThousandsQS(QtempV);
				QtempU = timeToQUnit(sysData.sweep.step.dwell);
				tempValue = "Dwell";
				displayString = getTheTranslatorString(tempValue) + ":" + "\t" + QtempV + "\t" + QtempU;
				p.drawText(rcTemp[0][2], Qt::AlignLeft | Qt::AlignVCenter, displayString);

				//绘制扫描扫描点数
				p.setPen(Qt::black);
				QtempV = tempQChar.sprintf("%d", sysData.sweep.step.point);
				tempValue = "Points";
				displayString = getTheTranslatorString(tempValue) + ":" + "\t" + QtempV;
				p.drawText(rcTemp[0][3], Qt::AlignLeft | Qt::AlignVCenter, displayString);

				//绘制扫描折返
				if (sysData.sweep.retrace == srOn)
				{
					displayString = "Retrace:\vOn";
				}
				else if (sysData.sweep.retrace == srOff)
				{
					displayString = "Retrace:\vOff";
					;
				}
				p.drawText(rcTemp[1][0], Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayString));

				//绘制扫描连续
				if (sysData.sweep.repeat == srSingle)
				{
					displayString = "Repeat:\vSingle";
				}
				else if (sysData.sweep.repeat == srConutinuos)
				{
					displayString = "Repeat:\vContinuous";
					;
				}
				p.drawText(rcTemp[1][1], Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayString));

				//绘制扫描触发
				if (sysData.sweep.trig == stAuto)
				{
					displayString = "Trigger:\vAuto"; //自动扫描
				}
				else if (sysData.sweep.trig == stExt)
				{
					displayString = "Trigger:\vstExt"; //外部触发
				}
				else if (sysData.sweep.trig == stKey)
				{
					displayString = "Trigger:\vstKey";//按键触发
				}
				p.drawText(rcTemp[1][2], Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayString));

				//绘制扫描方向
				if (sysData.sweep.direction == sdUp)
				{
					displayString = "Direction:\vUp";
				}
				else if (sysData.sweep.direction == sdDown)
				{
					displayString = "Direction:\vDown";
				}
				p.drawText(rcTemp[1][3], Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayString));
			}
		}
		else if (sysData.sweep.type == stList)
		{
			//列表扫描时，不显示起始、终止频率和幅度和驻留时间
			for (int i = 0; i < 5; i++)
			{
				for (int j = 0; j < 2; j++)
				{
					rcTemp[j][i] = QRect(rcPara.x() + rcPara.width() / 2 * j + interval, rcPara.y() + i * rcPara.height() / 5, rcPara.width() / 2 - interval, rcPara.height() / 5);
				}
			}
			//绘制扫描模式
			if (sysData.system.isLanguageEn)
			{
				p.setFont(fontEnglishBold);
			}
			else
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
			}
			else if (sysData.sweep.type == stStep)
			{
				displayString = "Type:\vStep";
			}
			p.drawText(rcTemp[0][1], Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayString));

			//绘制扫描扫描点数
			p.setPen(Qt::black);
			QtempV = tempQChar.sprintf("%d", sysData.sweep.step.point);
			tempValue = "Points";
			displayString = getTheTranslatorString(tempValue) + ":" + "\t" + QtempV;
			p.drawText(rcTemp[0][3], Qt::AlignLeft | Qt::AlignVCenter, displayString);

			//绘制扫描折返
			if (sysData.sweep.retrace == srOn)
			{
				displayString = "Retrace:\vOn";
			}
			else if (sysData.sweep.retrace == srOff)
			{
				displayString = "Retrace:\vOff";
				;
			}
			p.drawText(rcTemp[1][0], Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayString));

			//绘制扫描连续
			if (sysData.sweep.repeat == srSingle)
			{
				displayString = "Repeat:\vSingle";
			}
			else if (sysData.sweep.repeat == srConutinuos)
			{
				displayString = "Repeat:\vContinuous";
				;
			}
			p.drawText(rcTemp[1][1], Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayString));

			//绘制扫描触发
			if (sysData.sweep.trig == stAuto)
			{
				displayString = "Trigger:\vAuto"; //自动扫描
			}
			else if (sysData.sweep.trig == stExt)
			{
				displayString = "Trigger:\vstExt"; //外部触发
			}
			p.drawText(rcTemp[1][2], Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayString));

			//绘制扫描方向
			if (sysData.sweep.direction == sdUp)
			{
				displayString = "Direction:\vUp";
			}
			else if (sysData.sweep.direction == sdDown)
			{
				displayString = "Direction:\vDown";
			}
			p.drawText(rcTemp[1][3], Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayString));
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

	QRect rc = QRect(WIN1_4X, WIN1_4Y, WIN1_4WIDTH - 1, WIN1_4HEIGHT - 1);
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
		rcTitle[0] = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width() - 60, pix.rect().height() / 6); //标题区0
		rcTitle[1] = QRect((pix.rect().x() + pix.rect().width()- 60), pix.rect().y(), 60, pix.rect().height() / 6); //标题区

		p.fillRect(rcTitle[0], BACKCOLORWIN4);
		p.fillRect(rcTitle[1], BACKCOLORWIN4);
		p.fillRect(rcPara, Qt::white);

		for (int i = 0; i < 5; i++)
		{
			rcNum[i] = QRect(rcPara.x() + 10, rcPara.y() + i * rcPara.height() / rowDisplay, 30, rcPara.height() / rowDisplay);
			rcTemp[i][0] = QRect(rcPara.x() + 50, rcPara.y() + i * rcPara.height() / rowDisplay, 140, rcPara.height() / rowDisplay);
			rcTemp[i][1] = QRect(rcPara.x() + 200, rcPara.y() + i * rcPara.height() / rowDisplay, 90, rcPara.height() / rowDisplay);
			rcTemp[i][2] = QRect(rcPara.x() + 300, rcPara.y() + i * rcPara.height() / rowDisplay, 70, rcPara.height() / rowDisplay);
		}

		//	QRect rcPageNum = QRect(rcPara.x() + 420, rcPara.y(), 50, rcPara.height() / rowDisplay);

		QString displayQS, QtempV, QtempU;

		//绘制标题栏
		p.setPen(Qt::black);
		p.setFont(fontEnglishBold);
		displayQS = "List Mode Values";
		p.drawRect(rcTitle[0]);
		p.drawText(rcTitle[0].x() + 2, rcTitle[0].y(), rcTitle[0].width(), rcTitle[0].height(), Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayQS));

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
#if 0
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
#endif
}

void mainForm::drawProgBarSlots(bool Display, float RatioProgBar)
{
	drawProgBar(Display, RatioProgBar);
}

//绘制下载进度
void mainForm::drawDownloadProg(bool Display, int current, int amount)
{
#if 0
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
#endif
}

void mainForm::drawDownloadProgSlots(bool Display, int current, int amount)
{
	drawDownloadProg(Display, current, amount);
}

//绘制AM
void mainForm::drawAM(void)
{
	if (sysScpi->isUpdating)
	{
		return;
	}

	QRect rc = QRect(WIN1_4X, WIN1_4Y, WIN1_4WIDTH - 1, WIN1_4HEIGHT - 1);
	QRect rcTemp[2][16];
	QRect rcTitle[2]; //标题栏分为两列
	QPainter p;
	QPixmap pix(rc.size());
	int row = 6;
	int titelRightWidth = 80;

	if (!pix.isNull())
	{
		p.begin(&pix);

		//计算绘制空间
		QRect rcTemp1 = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width(), pix.rect().height()); //整个参数区（标题和参数）
		QRect rcPara = QRect(pix.rect().x(), pix.rect().y() + pix.rect().height() / 4, pix.rect().width(), pix.rect().height() / row * 4); //参数区
		rcTitle[0] = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width() - titelRightWidth, pix.rect().height() / row); //标题区0
		rcTitle[1] = QRect(pix.rect().width() - titelRightWidth, pix.rect().y(), titelRightWidth, pix.rect().height() / row); //标题区

		p.fillRect(rcTemp1, Qt::white);
		p.fillRect(rcTitle[0], BACKCOLORWIN4);
		p.fillRect(rcTitle[1], BACKCOLORWIN4);
		p.drawRect(pix.rect());
		int interval = 10;
		int tempRows = 5;
		int tempCols = 2;
		for (int i = 0; i < tempRows; i++)
		{
			for (int j = 0; j < tempCols; j++)
			{
				rcTemp[j][i] = QRect(rcPara.x() + rcPara.width() / tempCols * j + interval, rcPara.y() + i * rcPara.height() / tempRows, rcPara.width() / tempCols - interval, rcPara.height() / tempRows);
			}
		}

		p.setPen(Qt::black);
		QString displayString, QtempV, QtempU, tempValue;
		p.setFont(fontEnglishBold);
		displayString = "AM Status Information";
		p.drawRect(rcTitle[0]);
		p.drawText(rcTitle[0].x() + 2, rcTitle[0].y(), rcTitle[0].width(), rcTitle[0].height(), Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayString));
		
		displayString = "Page: 1/1";
		p.drawRect(rcTitle[1]);
		p.drawText(rcTitle[1].x(), rcTitle[1].y(), rcTitle[1].width(), rcTitle[1].height(), Qt::AlignCenter, displayString);

		if (sysData.system.isLanguageEn)
		{
			p.setFont(fontEnglishBold);
		}
		else
		{
			p.setFont(fontChinesePara);
		}		
		//绘制AM调制状态
		if (sysData.am.state)
		{
			displayString = "State:\tOn";
		}
		else
		{
			displayString = "State:\tOff";
		}
		p.drawText(rcTemp[0][0], Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayString));

		if (sysData.am.state)
			p.setPen(Qt::black); //根据状态判断画笔是否灰色处理
		else
			p.setPen(Qt::gray);
		//绘制调制深度
		QtempV.sprintf("%.2f", sysData.am.depth);
		QtempU = "%";
		displayString = getTheTranslatorString("Moudulate Depth") + ":\t" + (QtempV + QtempU);
		p.drawText(rcTemp[1][0], Qt::AlignLeft | Qt::AlignVCenter, displayString);
		//绘制调制源
		if (sysData.am.source)
		{
			displayString = getTheTranslatorString("Modulation Source") + QString(":\t") + getTheTranslatorString("EXT");
		}
		else
		{
			displayString = getTheTranslatorString("Modulation Source") + QString(":\t") + getTheTranslatorString("INT");
		}
		p.drawText(rcTemp[0][1], Qt::AlignLeft | Qt::AlignVCenter, displayString);
		//绘制内部函数波形和频率
		if (!sysData.am.source)
		{
			QString waveStr;
			switch (sysData.am.waveType)
			{
				case aiwSine:
					waveStr = "Sine";
					break;
				case aiwTriangle:
					waveStr = "Triangle";
					break;
				case aiwSquare:
					waveStr = "Square";
					break;
				case aiwSawtooth:
					waveStr = "Sawtooth";
					break;
				default:
					break;
			}
			displayString = getTheTranslatorString("Wave") + QString(":\t") + getTheTranslatorString(waveStr);
			p.drawText(rcTemp[1][1], Qt::AlignLeft | Qt::AlignVCenter, displayString);
			//绘制频率
			QtempV = doubleToQValue(sysData.am.rate);
			QtempV = insertThousandsQS(QtempV);
			QtempU = freqToQUnit(sysData.am.rate);
			displayString = getTheTranslatorString("Freq") + QString(":\t") + QtempV + QtempU;
			p.drawText(rcTemp[0][2], Qt::AlignLeft | Qt::AlignVCenter, displayString);
		}
		else
		{
			//绘制深度耦合参数
			if (sysData.am.coupling)
			{
				displayString = getTheTranslatorString("Deep Coupling") + ":\t" + getTheTranslatorString("On");
			}
			else
			{
				displayString = getTheTranslatorString("Deep Coupling") + ":\t" + getTheTranslatorString("Off");
			}
			p.drawText(rcTemp[1][1], Qt::AlignLeft | Qt::AlignVCenter, displayString);
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

//绘制FMPM
void mainForm::drawFMPM(void)
{
	if (sysData.fmpm.mode == fdtFM)
	{
		drawFM();
	}
	else if (sysData.fmpm.mode == fdtPM)
	{
		drawPM();
	}
}
//绘制FM
void mainForm::drawFM(void)
{
	if (sysScpi->isUpdating)
	{
		return;
	}

	QRect rc = QRect(WIN1_4X, WIN1_4Y, WIN1_4WIDTH - 1, WIN1_4HEIGHT - 1);
	QRect rcTemp[2][16];
	QRect rcTitle[2]; //标题栏分为两列
	QPainter p;
	QPixmap pix(rc.size());
	int row = 6;
	int titelRightWidth = 80;

	if (!pix.isNull())
	{
		p.begin(&pix);

		//计算绘制空间
		QRect rcTemp1 = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width(), pix.rect().height()); //整个参数区（标题和参数）
		QRect rcPara = QRect(pix.rect().x(), pix.rect().y() + pix.rect().height() / 4, pix.rect().width(), pix.rect().height() / row * 4); //参数区
		rcTitle[0] = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width() - titelRightWidth, pix.rect().height() / row); //标题区0
		rcTitle[1] = QRect(pix.rect().width() - titelRightWidth, pix.rect().y(), titelRightWidth, pix.rect().height() / row); //标题区

		p.fillRect(rcTemp1, Qt::white);
		p.fillRect(rcTitle[0], BACKCOLORWIN4);
		p.fillRect(rcTitle[1], BACKCOLORWIN4);
		p.drawRect(pix.rect());
		int interval = 10;
		int tempRows = 5;
		int tempCols = 2;
		for (int i = 0; i < tempRows; i++)
		{
			for (int j = 0; j < tempCols; j++)
			{
				rcTemp[j][i] = QRect(rcPara.x() + rcPara.width() / tempCols * j + interval, rcPara.y() + i * rcPara.height() / tempRows, rcPara.width() / tempCols - interval, rcPara.height() / tempRows);
			}
		}

		p.setPen(Qt::black);
		QString displayString, QtempV, QtempU, tempValue;
		p.setFont(fontEnglishBold);
		displayString = "FM Status Information";
		p.drawRect(rcTitle[0]);
		p.drawText(rcTitle[0].x() + 2, rcTitle[0].y(), rcTitle[0].width(), rcTitle[0].height(), Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayString));

		displayString = "Page: 1/1";
		p.drawRect(rcTitle[1]);
		p.drawText(rcTitle[1].x(), rcTitle[1].y(), rcTitle[1].width(), rcTitle[1].height(), Qt::AlignCenter, displayString);

		if (sysData.system.isLanguageEn)
		{
			p.setFont(fontEnglishBold);
		}
		else
		{
			p.setFont(fontChinesePara);
		}
		//绘制调制类型
		displayString = "Modulate Mode:\tFM";
		p.drawText(rcTemp[0][0], Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayString));
		//绘制AM调制状态
		if (sysData.fmpm.fm.state)
		{
			displayString = "FM State:\tOn";
		}
		else
		{
			displayString = "FM State:\tOff";
		}
		p.drawText(rcTemp[0][1], Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayString));

		if (sysData.fmpm.fm.state)
			p.setPen(Qt::black); //根据状态判断画笔是否灰色处理
		else
			p.setPen(Qt::gray);
		//绘制调制偏移
		QtempV = doubleToQValue(sysData.fmpm.fm.deviation);
		QtempV = insertThousandsQS(QtempV);
		QtempU = freqToQUnit(sysData.fmpm.fm.deviation);
		displayString = getTheTranslatorString("Freq Offset") + ":\t" + (QtempV + QtempU);
		p.drawText(rcTemp[1][0], Qt::AlignLeft | Qt::AlignVCenter, displayString);
		//绘制调制源
		if (sysData.fmpm.fm.source)
		{
			displayString = getTheTranslatorString("Modulation Source") + QString(":\t") + getTheTranslatorString("EXT");
		}
		else
		{
			displayString = getTheTranslatorString("Modulation Source") + QString(":\t") + getTheTranslatorString("INT");
		}
		p.drawText(rcTemp[1][1], Qt::AlignLeft | Qt::AlignVCenter, displayString);
		//绘制内部函数波形和频率
		if (!sysData.fmpm.fm.source)
		{
			QString waveStr;
			switch (sysData.fmpm.fm.waveType)
			{
			case aiwSine:
				waveStr = "Sine";
				break;
			case aiwTriangle:
				waveStr = "Triangle";
				break;
			case aiwSquare:
				waveStr = "Square";
				break;
			case aiwSawtooth:
				waveStr = "Sawtooth";
				break;
			default:
				break;
			}
			displayString = getTheTranslatorString("Wave") + QString(":\t") + getTheTranslatorString(waveStr);
			p.drawText(rcTemp[0][2], Qt::AlignLeft | Qt::AlignVCenter, displayString);
			//绘制频率
			QtempV = doubleToQValue(sysData.fmpm.fm.rate);
			QtempV = insertThousandsQS(QtempV);
			QtempU = freqToQUnit(sysData.fmpm.fm.rate);
			displayString = getTheTranslatorString("Freq") + QString(":\t") + QtempV + QtempU;
			p.drawText(rcTemp[1][2], Qt::AlignLeft | Qt::AlignVCenter, displayString);
		}
		else
		{
			//绘制深度耦合参数
			if (sysData.fmpm.fm.coupling)
			{
				displayString = getTheTranslatorString("FM Dev Couple") + ":\t" + getTheTranslatorString("On");
			}
			else
			{
				displayString = getTheTranslatorString("FM Dev Couple") + ":\t" + getTheTranslatorString("Off");
			}
			p.drawText(rcTemp[0][2], Qt::AlignLeft | Qt::AlignVCenter, displayString);
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
//绘制PM
void mainForm::drawPM(void)
{
	if (sysScpi->isUpdating)
	{
		return;
	}

	QRect rc = QRect(WIN1_4X, WIN1_4Y, WIN1_4WIDTH - 1, WIN1_4HEIGHT - 1);
	QRect rcTemp[2][16];
	QRect rcTitle[2]; //标题栏分为两列
	QPainter p;
	QPixmap pix(rc.size());
	int row = 6;
	int titelRightWidth = 80;

	if (!pix.isNull())
	{
		p.begin(&pix);

		//计算绘制空间
		QRect rcTemp1 = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width(), pix.rect().height()); //整个参数区（标题和参数）
		QRect rcPara = QRect(pix.rect().x(), pix.rect().y() + pix.rect().height() / 4, pix.rect().width(), pix.rect().height() / row * 4); //参数区
		rcTitle[0] = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width() - titelRightWidth, pix.rect().height() / row); //标题区0
		rcTitle[1] = QRect(pix.rect().width() - titelRightWidth, pix.rect().y(), titelRightWidth, pix.rect().height() / row); //标题区

		p.fillRect(rcTemp1, Qt::white);
		p.fillRect(rcTitle[0], BACKCOLORWIN4);
		p.fillRect(rcTitle[1], BACKCOLORWIN4);
		p.drawRect(pix.rect());
		int interval = 10;
		int tempRows = 5;
		int tempCols = 2;
		for (int i = 0; i < tempRows; i++)
		{
			for (int j = 0; j < tempCols; j++)
			{
				rcTemp[j][i] = QRect(rcPara.x() + rcPara.width() / tempCols * j + interval, rcPara.y() + i * rcPara.height() / tempRows, rcPara.width() / tempCols - interval, rcPara.height() / tempRows);
			}
		}

		p.setPen(Qt::black);
		QString displayString, QtempV, QtempU, tempValue;
		p.setFont(fontEnglishBold);
		displayString = "φM Status Information";
		p.drawRect(rcTitle[0]);
		p.drawText(rcTitle[0].x() + 2, rcTitle[0].y(), rcTitle[0].width(), rcTitle[0].height(), Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayString));

		displayString = "Page: 1/1";
		p.drawRect(rcTitle[1]);
		p.drawText(rcTitle[1].x(), rcTitle[1].y(), rcTitle[1].width(), rcTitle[1].height(), Qt::AlignCenter, displayString);

		if (sysData.system.isLanguageEn)
		{
			p.setFont(fontEnglishBold);
		}
		else
		{
			p.setFont(fontChinesePara);
		}
		//绘制调制类型
		displayString = "Modulate Mode:\tφM";
		p.drawText(rcTemp[0][0], Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayString));
		//绘制AM调制状态
		if (sysData.fmpm.pm.state)
		{
			displayString = "φM State:\tOn";
		}
		else
		{
			displayString = "φM State:\tOff";
		}
		p.drawText(rcTemp[0][1], Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayString));

		if (sysData.fmpm.pm.state)
			p.setPen(Qt::black); //根据状态判断画笔是否灰色处理
		else
			p.setPen(Qt::gray);
		//绘制调制偏移
		QtempV.sprintf("%.2f", sysData.fmpm.pm.phase);
		QtempU = "°";
		displayString = getTheTranslatorString("Phase Offset") + ":\t" + (QtempV + QtempU);
		p.drawText(rcTemp[1][0], Qt::AlignLeft | Qt::AlignVCenter, displayString);
		//绘制调制源
		if (sysData.fmpm.pm.source)
		{
			displayString = getTheTranslatorString("Modulation Source") + QString(":\t") + getTheTranslatorString("EXT");
		}
		else
		{
			displayString = getTheTranslatorString("Modulation Source") + QString(":\t") + getTheTranslatorString("INT");
		}
		p.drawText(rcTemp[1][1], Qt::AlignLeft | Qt::AlignVCenter, displayString);
		//绘制内部函数波形和频率
		if (!sysData.fmpm.pm.source)
		{
			QString waveStr;
			switch (sysData.fmpm.pm.waveType)
			{
			case aiwSine:
				waveStr = "Sine";
				break;
			case aiwTriangle:
				waveStr = "Triangle";
				break;
			case aiwSquare:
				waveStr = "Square";
				break;
			case aiwSawtooth:
				waveStr = "Sawtooth";
				break;
			default:
				break;
			}
			displayString = getTheTranslatorString("Wave") + QString(":\t") + getTheTranslatorString(waveStr);
			p.drawText(rcTemp[0][2], Qt::AlignLeft | Qt::AlignVCenter, displayString);
			//绘制频率
			QtempV = doubleToQValue(sysData.fmpm.pm.rate);
			QtempV = insertThousandsQS(QtempV);
			QtempU = freqToQUnit(sysData.fmpm.pm.rate);
			displayString = getTheTranslatorString("Freq") + QString(":\t") + QtempV + QtempU;
			p.drawText(rcTemp[1][2], Qt::AlignLeft | Qt::AlignVCenter, displayString);
		}
		else
		{
			//绘制深度耦合参数
			if (sysData.fmpm.pm.coupling)
			{
				displayString = getTheTranslatorString("φM Dev Couple") + ":\t" + getTheTranslatorString("On");
			}
			else
			{
				displayString = getTheTranslatorString("φM Dev Couple") + ":\t" + getTheTranslatorString("Off");
			}
			p.drawText(rcTemp[0][2], Qt::AlignLeft | Qt::AlignVCenter, displayString);
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

void mainForm::drawIQ(void)
{
	if (sysScpi->isUpdating)
	{
		return;
	}

	QRect rc = QRect(WIN1_4X, WIN1_4Y, WIN1_4WIDTH - 1, WIN1_4HEIGHT - 1);
	QRect rcTemp[2][16];
	QRect rcTitle[2]; //标题栏分为两列
	QPainter p;
	QPixmap pix(rc.size());
	int row = 6;
	int titelRightWidth = 80;

	if (!pix.isNull())
	{
		p.begin(&pix);

		//计算绘制空间
		QRect rcTemp1 = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width(), pix.rect().height()); //整个参数区（标题和参数）
		QRect rcPara = QRect(pix.rect().x(), pix.rect().y() + pix.rect().height() / 4, pix.rect().width(), pix.rect().height() / row * 4); //参数区
		rcTitle[0] = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width() - titelRightWidth, pix.rect().height() / row); //标题区0
		rcTitle[1] = QRect(pix.rect().width() - titelRightWidth, pix.rect().y(), titelRightWidth, pix.rect().height() / row); //标题区

		p.fillRect(rcTemp1, Qt::white);
		p.fillRect(rcTitle[0], BACKCOLORWIN4);
		p.fillRect(rcTitle[1], BACKCOLORWIN4);
		p.drawRect(pix.rect());
		int interval = 10;
		int tempRows = 5;
		int tempCols = 2;
		for (int i = 0; i < tempRows; i++)
		{
			for (int j = 0; j < tempCols; j++)
			{
				rcTemp[j][i] = QRect(rcPara.x() + rcPara.width() / tempCols * j + interval, rcPara.y() + i * rcPara.height() / tempRows, rcPara.width() / tempCols - interval, rcPara.height() / tempRows);
			}
		}

		p.setPen(Qt::black);
		QString displayString, QtempV, QtempU, tempValue;
		p.setFont(fontEnglishBold);
		displayString = "IQ Information";
		p.drawRect(rcTitle[0]);
		p.drawText(rcTitle[0].x() + 2, rcTitle[0].y(), rcTitle[0].width(), rcTitle[0].height(), Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayString));

		displayString = "Page: 1/1";
		p.drawRect(rcTitle[1]);
		p.drawText(rcTitle[1].x(), rcTitle[1].y(), rcTitle[1].width(), rcTitle[1].height(), Qt::AlignCenter, displayString);

		if (sysData.system.isLanguageEn)
		{
			p.setFont(fontEnglishBold);
		}
		else
		{
			p.setFont(fontChinesePara);
		}
		//绘制调制类型
		displayString = "No Function";
		p.drawText(rcTemp[0][0], Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayString));
		
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


//绘制数字调制参数区
void mainForm::drawDigitalModulate(void)
{
	if (sysScpi->isUpdating)
	{
		return;
	}
	QRect rc = QRect(WIN1_4X, WIN1_4Y, WIN1_4WIDTH - 1, WIN1_4HEIGHT - 1);
	QRect rcTemp[2][16];
	QRect rcTitle[2]; //标题栏分为两列
	QPainter p;
	QPixmap pix(rc.size());
	int row = 6;
	int titelRightWidth = 80;

	if (!pix.isNull())
	{
		p.begin(&pix);
		//计算绘制空间
		QRect rcTemp1 = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width(), pix.rect().height()); //整个参数区（标题和参数）
		QRect rcPara = QRect(pix.rect().x(), pix.rect().y() + pix.rect().height() / 4, pix.rect().width(), pix.rect().height() / row * 4); //参数区
		rcTitle[0] = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width() - titelRightWidth, pix.rect().height() / row); //标题区0
		rcTitle[1] = QRect(pix.rect().width() - titelRightWidth, pix.rect().y(), titelRightWidth, pix.rect().height() / row); //标题区

		p.fillRect(rcTemp1, Qt::white);
		p.fillRect(rcTitle[0], BACKCOLORWIN4);
		p.fillRect(rcTitle[1], BACKCOLORWIN4);
		p.drawRect(pix.rect());
		int interval = 10;
		int tempRows = 5;
		int tempCols = 2;
		for (int i = 0; i < tempRows; i++)
		{
			for (int j = 0; j < tempCols; j++)
			{
				rcTemp[j][i] = QRect(rcPara.x() + rcPara.width() / tempCols * j + interval, rcPara.y() + i * rcPara.height() / tempRows, rcPara.width() / tempCols - interval, rcPara.height() / tempRows);
			}
		}

		p.setPen(Qt::black);
		QString displayString, QtempV, QtempU, tempValue;

		p.setFont(fontEnglishBold);
		displayString = "Digital Modelation Information";
		p.drawRect(rcTitle[0]);
		p.drawText(rcTitle[0].x() + 2, rcTitle[0].y(), rcTitle[0].width(), rcTitle[0].height(), Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayString));

		displayString = "Page: 1/1";
		p.drawRect(rcTitle[1]);
		p.drawText(rcTitle[1].x(), rcTitle[1].y(), rcTitle[1].width(), rcTitle[1].height(), Qt::AlignCenter, displayString);

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
			displayString = displayString + ":\t" + getTheTranslatorString("INT");
		} else if (sysData.digital.source == dstExternal)
		{
			displayString = displayString + ":\t" + getTheTranslatorString("EXT");
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
void mainForm::drawDigitalModulateSlots(void)
{
	drawDigitalModulate();
}

//绘制脉冲调制参数区
void mainForm::drawPulseModulate(void)
{
	QRect rc = QRect(WIN1_4X, WIN1_4Y, WIN1_4WIDTH - 1, WIN1_4HEIGHT - 1);
	QRect rcTemp[2][16];
	QRect rcTitle[2]; //标题栏分为两列
	QPainter p;
	QPixmap pix(rc.size());
	int row = 6;
	int titelRightWidth = 80;

	if (!pix.isNull())
	{
		p.begin(&pix);

		//计算绘制空间
		QRect rcTemp1 = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width(), pix.rect().height()); //整个参数区（标题和参数）
		QRect rcPara = QRect(pix.rect().x(), pix.rect().y() + pix.rect().height() / 4, pix.rect().width(), pix.rect().height() / row * 4); //参数区
		rcTitle[0] = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width() - titelRightWidth, pix.rect().height() / row); //标题区0
		rcTitle[1] = QRect(pix.rect().width() - titelRightWidth, pix.rect().y(), titelRightWidth, pix.rect().height() / row); //标题区

		p.fillRect(rcTemp1, Qt::white);
		p.fillRect(rcTitle[0], BACKCOLORWIN4);
		p.fillRect(rcTitle[1], BACKCOLORWIN4);
		p.drawRect(pix.rect());
		int interval = 10;
		int tempRows = 5;
		int tempCols = 2;
		for (int i = 0; i < tempRows; i++)
		{
			for (int j = 0; j < tempCols; j++)
			{
				rcTemp[j][i] = QRect(rcPara.x() + rcPara.width() / tempCols * j + interval, rcPara.y() + i * rcPara.height() / tempRows, rcPara.width() / tempCols - interval, rcPara.height() / tempRows);
			}
		}

		p.setPen(Qt::black);
		QString displayString, QtempV, QtempU, tempValue;
		p.setFont(fontEnglishBold);
		//绘制标题栏		
		displayString = "Pulse Modelation Information";
		p.drawRect(rcTitle[0]);
		p.drawText(rcTitle[0].x() + 2, rcTitle[0].y(), rcTitle[0].width(), rcTitle[0].height(), Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayString));

		displayString = "Page: 1/1";
		p.drawRect(rcTitle[1]);
		p.drawText(rcTitle[1].x(), rcTitle[1].y(), rcTitle[1].width(), rcTitle[1].height(), Qt::AlignCenter, displayString);

		//绘制脉冲调制状态
		if (sysData.system.isLanguageEn)
		{
			p.setFont(fontEnglishBold);
		} else
		{
			p.setFont(fontChinesePara);
		}
		if (sysData.pulse.state) //数字调制状态是打开
		{
			displayString = "Modelation Status:\tOn";
		} else
		{
			displayString = "Modelation Status:\tOff";
		}
		p.drawText(rcTemp[0][0], Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayString));

		if (sysData.pulse.state)
			p.setPen(Qt::black);
		else
			p.setPen(Qt::gray);

		displayString = "Modelation Method";
		//绘制脉冲触发方式
		switch (sysData.pulse.trigmethod)
		{
			case plmFreerun:	//自由运行
				displayString = getTheTranslatorString(displayString) + ":\t" + getTheTranslatorString("Free Run");
				break;
			case plmKeytrig:	//按键触发
				displayString = getTheTranslatorString(displayString) + ":\t" + getTheTranslatorString("Key Trig");
				break;
			case plmExttrig:	//外部触发
				displayString = getTheTranslatorString(displayString) + ":\t" + getTheTranslatorString("Ext Trig");
				break;
			default:
				break;
		}
		p.drawText(rcTemp[1][0], Qt::AlignLeft | Qt::AlignVCenter, displayString);

		//绘制脉冲源
		displayString = "Pulse Source";
		switch (sysData.pulse.pulsesource)
		{
			case psInternal:
				displayString = getTheTranslatorString(displayString) + ":\t" + getTheTranslatorString(QString("INT"));
				break;
			case psExternal:
				displayString = getTheTranslatorString(displayString) + ":\t" + getTheTranslatorString(QString("EXT"));
				break;
			default:
				break;
		}
		p.drawText(rcTemp[0][1], Qt::AlignLeft | Qt::AlignVCenter, displayString);

		//绘制脉冲周期
		displayString = "Pulse Period";
		QtempV = doubleToQValue(sysData.pulse.period);
		QtempV = insertThousandsQS(QtempV);
		QtempU = timeToQUnit(sysData.pulse.period * 1e-6);	//timeToQUnit参数以ms为单位，所以需要转换
		displayString = getTheTranslatorString(displayString) + ":\t" + QtempV + QtempU;
		p.drawText(rcTemp[1][1], Qt::AlignLeft | Qt::AlignVCenter, displayString);

		//绘制脉冲宽度
		displayString = "Pulse Width";
		QtempV = doubleToQValue(sysData.pulse.width);
		QtempV = insertThousandsQS(QtempV);
		QtempU = timeToQUnit(sysData.pulse.width * 1e-6);	//timeToQUnit参数以ms为单位，所以需要转换
		displayString = getTheTranslatorString(displayString) + ":\t" + QtempV + QtempU;
		p.drawText(rcTemp[0][2], Qt::AlignLeft | Qt::AlignVCenter, displayString);

		//绘制脉冲延时
		p.setPen(Qt::gray);
		displayString = "Pulse Delay";
		QtempV = doubleToQValue(sysData.pulse.delay);
		QtempV = insertThousandsQS(QtempV);
		QtempU = timeToQUnit(sysData.pulse.delay * 1e-6);	//timeToQUnit参数以ms为单位，所以需要转换
		displayString = getTheTranslatorString(displayString) + ":\t" + QtempV + QtempU;
		p.drawText(rcTemp[1][2], Qt::AlignLeft | Qt::AlignVCenter, displayString);

		//绘制脉冲极性
		p.setPen(Qt::gray);
		displayString = getTheTranslatorString("Pulse Polarity");
		if (sysData.pulse.polarity == pepRising)
		{
			displayString += ":\t" + getTheTranslatorString("Rasing");
		}
		else if (sysData.pulse.polarity == pepFall)
		{
			displayString += ":\t" + getTheTranslatorString("Fall");
		}
		p.drawText(rcTemp[0][3], Qt::AlignLeft | Qt::AlignVCenter, displayString);

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
	drawPulseModulate();
}

//绘制上变频功能
void mainForm::drawExfFref(void)
{
	if (sysScpi->isUpdating)
	{
		return;
	}
	QRect rc = QRect(WIN1_4X, WIN1_4Y, WIN1_4WIDTH - 1, WIN1_4HEIGHT - 1);
	QRect rcTemp[2][16];
	QRect rcTitle[2]; //标题栏分为两列
	QPainter p;
	QPixmap pix(rc.size());
	int row = 6;
	int titelRightWidth = 80;
	if (!pix.isNull())
	{
		p.begin(&pix);

		//计算绘制空间
		QRect rcTemp1 = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width(), pix.rect().height()); //整个参数区（标题和参数）
		QRect rcPara = QRect(pix.rect().x(), pix.rect().y() + pix.rect().height() / 4, pix.rect().width(), pix.rect().height() / row * 4); //参数区
		rcTitle[0] = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width() - titelRightWidth, pix.rect().height() / row); //标题区0
		rcTitle[1] = QRect(pix.rect().width() - titelRightWidth, pix.rect().y(), titelRightWidth, pix.rect().height() / row); //标题区

		p.fillRect(rcTemp1, Qt::white);
		p.fillRect(rcTitle[0], BACKCOLORWIN4);
		p.fillRect(rcTitle[1], BACKCOLORWIN4);
		p.drawRect(pix.rect());
		int interval = 10;
		int tempRows = 5;
		int tempCols = 2;
		for (int i = 0; i < tempRows; i++)
		{
			for (int j = 0; j < tempCols; j++)
			{
				rcTemp[j][i] = QRect(rcPara.x() + rcPara.width() / tempCols * j + interval, rcPara.y() + i * rcPara.height() / tempRows, rcPara.width() / tempCols - interval, rcPara.height() / tempRows);
			}
		}

		p.setPen(Qt::black);
		QString displayString, QtempV, QtempU, tempValue;
		p.setFont(fontEnglishBold);

		displayString = "Exf Ref Information";
		p.drawRect(rcTitle[0]);
		p.drawText(rcTitle[0].x() + 2, rcTitle[0].y(), rcTitle[0].width(), rcTitle[0].height(), Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayString));

		displayString = "Page: 1/1";
		p.drawRect(rcTitle[1]);
		p.drawText(rcTitle[1].x(), rcTitle[1].y(), rcTitle[1].width(), rcTitle[1].height(), Qt::AlignCenter, displayString);

		//绘制频率参考类型
		if (sysData.system.isLanguageEn)
		{
			p.setFont(fontEnglishBold);
		}
		else
		{
			p.setFont(fontChinesePara);
		}

		if (sysData.exfRef.type == erftInternal)
		{
			displayString = "IF Source :\tInternal";
		}
		else if (sysData.exfRef.type == erftExternal)
		{
			displayString = "IF Source :\tExternal";
		}
		p.drawText(rcTemp[0][0], Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayString));


		//绘制频率参考
		if (sysData.exfRef.type == erftInternal)
		{
			p.setPen(Qt::gray);
		}
		else if (sysData.exfRef.type == erftExternal)
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
			QString tempQchar = sysScpi->getQvalueFromModule();
			displayString = displayString + tempQchar.trimmed() + " dBm";
			rcTemp[0][2].setWidth(350);//修改宽度，添加单位显示
			p.drawText(rcTemp[0][2], Qt::AlignLeft | Qt::AlignVCenter, displayString);
			//printf("rect x:%d,y:%d, w:%d, h:%d\n",rcTemp[0][2].x(),rcTemp[0][2].y(),\
			//	rcTemp[0][2].width(),rcTemp[0][2].height());
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
void mainForm::drawExfFrefSlots(void)
{
	drawExfFref();
}


//绘制任意波参数区
void mainForm::drawArbitraryWave(void)
{
	if (sysScpi->isUpdating)
	{
		return;
	}

	QRect rc = QRect(WIN1_4X, WIN1_4Y, WIN1_4WIDTH - 1, WIN1_4HEIGHT - 1);
	QRect rcTemp[2][16];
	QRect rcTitle[2]; //标题栏分为两列
	QPainter p;
	QPixmap pix(rc.size());
	int row = 6;
	int titelRightWidth = 80;
	if (!pix.isNull())
	{
		p.begin(&pix);

		//计算绘制空间
		QRect rcTemp1 = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width(), pix.rect().height()); //整个参数区（标题和参数）
		QRect rcPara = QRect(pix.rect().x(), pix.rect().y() + pix.rect().height() / 4, pix.rect().width(), pix.rect().height() / row * 4); //参数区
		rcTitle[0] = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width() - titelRightWidth, pix.rect().height() / row); //标题区0
		rcTitle[1] = QRect(pix.rect().width() - titelRightWidth, pix.rect().y(), titelRightWidth, pix.rect().height() / row); //标题区

		p.fillRect(rcTemp1, Qt::white);
		p.fillRect(rcTitle[0], BACKCOLORWIN4);
		p.fillRect(rcTitle[1], BACKCOLORWIN4);
		p.drawRect(pix.rect());
		int interval = 10;
		int tempRows = 5;
		int tempCols = 2;
		for (int i = 0; i < tempRows; i++)
		{
			for (int j = 0; j < tempCols; j++)
			{
				rcTemp[j][i] = QRect(rcPara.x() + rcPara.width() / tempCols * j + interval, rcPara.y() + i * rcPara.height() / tempRows, rcPara.width() / tempCols - interval, rcPara.height() / tempRows);
			}
		}
		p.setPen(Qt::black);
		QString displayString, QtempV, QtempU, tempValue;
		p.setFont(fontEnglishBold);

		displayString = "Arbitrary Wave Status Information";
		p.drawRect(rcTitle[0]);
		p.drawText(rcTitle[0].x() + 2, rcTitle[0].y(), rcTitle[0].width(), rcTitle[0].height(), Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayString));
		displayString = "Page: 1/1";
		p.drawRect(rcTitle[1]);
		p.drawText(rcTitle[1].x(), rcTitle[1].y(), rcTitle[1].width(), rcTitle[1].height(), Qt::AlignCenter, displayString);

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
			displayString = getTheTranslatorString(QString("LFOut Status")) + ":\t" + getTheTranslatorString(QString("ON"));
		} else
		{
			displayString = getTheTranslatorString(QString("LFOut Status")) + ":\t" + getTheTranslatorString(QString("OFF"));
		}
		p.drawText(rcTemp[0][0], Qt::AlignLeft | Qt::AlignVCenter, displayString);

		if (sysData.lf.state)
			p.setPen(Qt::black);
		else
			p.setPen(Qt::gray);

		displayString = getTheTranslatorString("Wave");
		switch (sysData.lf.type)
		{
			case lotSine:
				displayString = displayString + ":\t" + getTheTranslatorString("Sine");
				break;
			case lotSquare:
				displayString = displayString + ":\t" + getTheTranslatorString("Square");
				break;
			case lotTriangle:
				displayString = displayString + ":\t" + getTheTranslatorString("Triangle");
				break;
			case lotSawtooth:
				displayString = displayString + ":\t" + getTheTranslatorString("Sawtooth");
				break;
		}
		p.drawText(rcTemp[1][0], Qt::AlignLeft | Qt::AlignVCenter, displayString);

		//绘制输出频率
		displayString = getTheTranslatorString("Frequency");
		char tempChar[64];
		char temp[64];
		strcpy(tempChar, getFreqStringInLF(sysData.lf.freq, temp));
		QtempV = QString(tempChar);
		QtempV = insertThousandsQS(QtempV).trimmed().toUpper();
		QtempU = freqToQUnit(sysData.lf.freq);
		displayString = displayString + ":\t" + QtempV + QtempU;
		p.drawText(rcTemp[0][1], Qt::AlignLeft | Qt::AlignVCenter, displayString);

		//绘制输出电压
		displayString = getTheTranslatorString("Voltage");
		QtempV = doubleToQValue(sysData.lf.ampl);
		QtempV = insertThousandsQS(QtempV);
		QtempU = vppToQUnit(sysData.lf.ampl);
		displayString = displayString + ":\t" + QtempV + QtempU;
		p.drawText(rcTemp[1][1], Qt::AlignLeft | Qt::AlignVCenter, displayString);

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
	QRect rc = QRect(WIN1_4X, WIN1_4Y, WIN1_4WIDTH - 1, WIN1_4HEIGHT - 1);
	QRect rcTemp[2][16];
	QRect rcTitle[2]; //标题栏分为两列
	QPainter p;
	QPixmap pix(rc.size());
	char tempChar[64];
	char temp[64];
	int row = 6;
	int titelRightWidth = 80;
	if (!pix.isNull())
	{
		p.begin(&pix);

		//计算绘制空间
		QRect rcTemp1 = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width(), pix.rect().height()); //整个参数区（标题和参数）
		QRect rcPara = QRect(pix.rect().x(), pix.rect().y() + pix.rect().height() / 4, pix.rect().width(), pix.rect().height() / row * 4); //参数区
		rcTitle[0] = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width() - titelRightWidth, pix.rect().height() / row); //标题区0
		rcTitle[1] = QRect(pix.rect().width() - titelRightWidth, pix.rect().y(), titelRightWidth, pix.rect().height() / row); //标题区

		p.fillRect(rcTemp1, Qt::white);
		p.fillRect(rcTitle[0], BACKCOLORWIN4);
		p.fillRect(rcTitle[1], BACKCOLORWIN4);
		p.drawRect(pix.rect());
		int interval = 10;
		int tempRows = 5;
		int tempCols = 2;
		for (int i = 0; i < tempRows; i++)
		{
			for (int j = 0; j < tempCols; j++)
			{
				rcTemp[j][i] = QRect(rcPara.x() + rcPara.width() / tempCols * j + interval, rcPara.y() + i * rcPara.height() / tempRows, rcPara.width() / tempCols - interval, rcPara.height() / tempRows);
			}
		}
		p.setPen(Qt::black);

		QString displayQS, QtempV, QtempU;
		//绘制标题栏
		p.setFont(fontEnglishBold);
		displayQS = getTheTranslatorString("DRG Information");
		p.drawRect(rcTitle[0]);
		p.drawText(rcTitle[0].x() + 2, rcTitle[0].y(), rcTitle[0].width(), rcTitle[0].height(), Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayQS));

		displayQS = "Page: 1/1";
		p.drawRect(rcTitle[1]);
		p.drawText(rcTitle[1].x() + 2, rcTitle[1].y(), rcTitle[1].width(), rcTitle[1].height(), Qt::AlignLeft | Qt::AlignVCenter, displayQS);

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
		strcpy(tempChar, doubleToQValue(sysData.drg.rate).toStdString().c_str());
		QtempV = QString(tempChar);
		QtempV = insertThousandsQS(QtempV);
		QtempU = timeToQUnit(sysData.drg.rate  * 1e-6);
		displayQS = displayQS + ":\t" + QtempV + QtempU;
		p.drawText(rcTemp[0][2], Qt::AlignLeft | Qt::AlignVCenter, displayQS);

		//绘制工作模式
		displayQS = getTheTranslatorString("Work Mode");
		if (sysData.drg.workmode == ddnormal)
		{
			displayQS = displayQS + ":\t" + getTheTranslatorString("Bothway");
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

//绘制USB功率计
void mainForm::drawUSBPower(void)
{
	if (sysScpi->isUpdating)
	{
		return;
	}
	QRect rc = QRect(WIN1_4X, WIN1_4Y, WIN1_4WIDTH - 1, WIN1_4HEIGHT - 1);
	QRect rcTemp[2][16];
	QRect rcTitle[2]; //标题栏分为两列
	QPainter p;
	QPixmap pix(rc.size());
	char tempChar[64];
	char temp[64];
	int row = 6;
	int titelRightWidth = 80;
	if (!pix.isNull())
	{
		p.begin(&pix);

		//计算绘制空间
		QRect rcTemp1 = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width(), pix.rect().height()); //整个参数区（标题和参数）
		QRect rcPara = QRect(pix.rect().x(), pix.rect().y() + pix.rect().height() / 4, pix.rect().width(), pix.rect().height() / row * 4); //参数区
		rcTitle[0] = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width() - titelRightWidth, pix.rect().height() / row); //标题区0
		rcTitle[1] = QRect(pix.rect().width() - titelRightWidth, pix.rect().y(), titelRightWidth, pix.rect().height() / row); //标题区

		p.fillRect(rcTemp1, Qt::white);
		p.fillRect(rcTitle[0], BACKCOLORWIN4);
		p.fillRect(rcTitle[1], BACKCOLORWIN4);
		p.drawRect(pix.rect());
		int interval = 10;
		int tempRows = 5;
		int tempCols = 2;
		for (int i = 0; i < tempRows; i++)
		{
			for (int j = 0; j < tempCols; j++)
			{
				rcTemp[j][i] = QRect(rcPara.x() + rcPara.width() / tempCols * j + interval, rcPara.y() + i * rcPara.height() / tempRows, rcPara.width() / tempCols - interval, rcPara.height() / tempRows);
			}
		}
		p.setPen(Qt::black);
		QString displayQS, QtempV, QtempU;
		//绘制标题栏
		p.setFont(fontEnglishBold);
		displayQS = getTheTranslatorString("USB Power Information");
		p.drawRect(rcTitle[0]);
		p.drawText(rcTitle[0].x() + 2, rcTitle[0].y(), rcTitle[0].width(), rcTitle[0].height(), Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayQS));

		displayQS = "Page: 1/1";
		p.drawRect(rcTitle[1]);
		p.drawText(rcTitle[1].x() + 2, rcTitle[1].y(), rcTitle[1].width(), rcTitle[1].height(), Qt::AlignCenter, displayQS);

		if (sysData.system.isLanguageEn)
		{
			p.setFont(fontEnglishBold);
		}
		else
		{
			p.setFont(fontChinesePara);
		}
		//绘制状态
		displayQS = getTheTranslatorString("State");
		if (sysData.usbPower.state)
		{
			displayQS = displayQS + ":\t" + getTheTranslatorString("ON");
		}
		else
		{
			displayQS = displayQS + ":\t" + getTheTranslatorString("OFF");
		}
		p.drawText(rcTemp[0][0], Qt::AlignLeft | Qt::AlignVCenter, displayQS);

		if (sysData.usbPower.state)
			p.setPen(Qt::black);
		else
			p.setPen(Qt::gray);

		//通道频率
		displayQS = getTheTranslatorString("Frequency");
		strcpy(tempChar, getFreqString(sysData.usbPower.freq, temp));
		QtempV = QString(tempChar);
		QtempV = insertThousandsQS(QtempV).trimmed().toUpper();
		QtempU = freqToQUnit(sysData.usbPower.freq).trimmed();
		displayQS = displayQS + ":\t" + QtempV + QtempU;
		p.drawText(rcTemp[1][0], Qt::AlignLeft | Qt::AlignVCenter, displayQS);
		//绘制功率偏移
		displayQS = getTheTranslatorString("Pow Offset");
		//printf("zpc add test, sysData.usbPower.powerOffset = %f\n", sysData.usbPower.powerOffset);
		QtempV = doubleToQValue(sysData.usbPower.powerOffset);
		QtempU = " dB";
		displayQS = displayQS + ":\t" + QtempV + QtempU;
		p.drawText(rcTemp[1][1], Qt::AlignLeft | Qt::AlignVCenter, displayQS);
		//绘制平均模式
		displayQS = getTheTranslatorString("Average Mode");
		if (sysData.usbPower.AdvMode == upamtOff)
		{
			displayQS += ":\t" + getTheTranslatorString("Off");
		}
		else if (sysData.usbPower.AdvMode == upamtAuto)
		{
			displayQS += ":\t" + getTheTranslatorString("Auto");
		}
		p.drawText(rcTemp[0][1], Qt::AlignLeft | Qt::AlignVCenter, displayQS);
		//绘制平均次数
		displayQS = getTheTranslatorString("Average Count") + ":\t";
		displayQS += QString(intToQString(sysData.usbPower.AdvCount, temp)).trimmed();
		p.drawText(rcTemp[0][2], Qt::AlignLeft | Qt::AlignVCenter, displayQS);
		//绘制测量单位
		displayQS = getTheTranslatorString("Meas Unit");
		if (sysData.usbPower.MeasUnit == upmutdBm)
		{
			displayQS = displayQS + ":\t" + "dBm";
		}
		else if (sysData.usbPower.MeasUnit == upmutW)
		{
			displayQS = displayQS + ":\t" + "W";
		}
		p.drawText(rcTemp[1][2], Qt::AlignLeft | Qt::AlignVCenter, displayQS);

		//绘制从功率计上传上来的功率
		if (sysData.usbPower.state)
		{
			double dispalyValue = 0;
			QString unit = " dBm";
			if (sysData.usbPower.AdvMode == upamtOff)
			{
				dispalyValue = sysData.usbPower.rcvValue;
			}
			else if (sysData.usbPower.AdvMode == upamtAuto)
			{
				if (sysData.usbPower.list.count() > 0)
				{
					for (int i = 0; i < sysData.usbPower.list.count(); i++)
					{
						dispalyValue += sysData.usbPower.list[i];	//计算和
					}
					dispalyValue /= sysData.usbPower.list.count();//计算平均
				}
			}
			if (sysData.usbPower.MeasUnit == upmutW)
			{
				dispalyValue = (pow(10, dispalyValue / 10)) / 1000;	//dBm转化成W
				unit = " W";
			}
			char tempChar[64] = {};
			displayQS = getTheTranslatorString("Ampl") + ":\t" + QString(floatToStringDot2(dispalyValue, tempChar)).trimmed() + unit;
			p.setFont(fontEnglishBold);//字体加粗
			p.drawText(rcTemp[0][3], Qt::AlignLeft | Qt::AlignVCenter, displayQS);
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
void mainForm::drawUSBPowerSlots(void)
{
	drawUSBPower();
}

//绘制系统信息参数区
void mainForm::drawSystemPara(void)
{
	if (sysScpi->isUpdating)
	{
		return;
	}

	QRect rc = QRect(WIN1_4X, WIN1_4Y, WIN1_4WIDTH - 1, WIN1_4HEIGHT - 1);
	QRect rcTemp[2][16];
	QRect rcTitle[2]; //标题栏分为两列
	QPainter p;
	QPixmap pix(rc.size());
	char tempChar[64];
	char temp[64];
	int row = 6;
	int titelRightWidth = 80;

	if (!pix.isNull())
	{
		p.begin(&pix);
		p.fillRect(pix.rect(), BACKCOLORWIN4);

		//计算绘制空间
		QRect rcTemp1 = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width(), pix.rect().height()); //整个参数区（标题和参数）
		QRect rcPara = QRect(pix.rect().x(), pix.rect().y() + pix.rect().height() / 4, pix.rect().width(), pix.rect().height() / row * 4); //参数区
		rcTitle[0] = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width() - titelRightWidth, pix.rect().height() / row); //标题区0
		rcTitle[1] = QRect(pix.rect().width() - titelRightWidth, pix.rect().y(), titelRightWidth, pix.rect().height() / row); //标题区

		p.fillRect(rcTemp1, Qt::white);
		p.fillRect(rcTitle[0], BACKCOLORWIN4);
		p.fillRect(rcTitle[1], BACKCOLORWIN4);
		p.drawRect(pix.rect());
		int interval = 10;
		int tempRows = 5;
		int tempCols = 2;
		for (int i = 0; i < tempRows; i++)
		{
			for (int j = 0; j < tempCols; j++)
			{
				rcTemp[j][i] = QRect(rcPara.x() + rcPara.width() / tempCols * j + interval, rcPara.y() + i * rcPara.height() / tempRows, rcPara.width() / tempCols - interval, rcPara.height() / tempRows);
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
		p.drawText(rcTitle[0].x() + 2, rcTitle[0].y(), rcTitle[0].width(), rcTitle[0].height(), Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayQS));

		p.setFont(fontEnglishBold);
		displayQS = "Page: 1/1";
		p.drawRect(rcTitle[1]);
		p.drawText(rcTitle[1].x() + 2, rcTitle[1].y(), rcTitle[1].width(), rcTitle[1].height(), Qt::AlignCenter, displayQS);

		//绘制名称
		displayQS = "IP:\t" + QString(sysData.system.io.lan.ip);
		p.drawText(rcTemp[0][0], Qt::AlignLeft | Qt::AlignVCenter, displayQS);
		displayQS = "Mask:\t" + QString(sysData.system.io.lan.mask);
		p.drawText(rcTemp[0][2], Qt::AlignLeft | Qt::AlignVCenter, displayQS);
		displayQS = "Gate:\t" + QString(sysData.system.io.lan.gate);
		p.drawText(rcTemp[0][1], Qt::AlignLeft | Qt::AlignVCenter, displayQS);

		//绘制软件版本号
		displayQS = "Soft Version:\t" + QString(intToString(VER_MAJOR, tempChar)).trimmed() + "." + QString(intToString(VER_MINOR, tempChar)).trimmed() + "." + QString(intToString(VER_REVISION,
				tempChar)).trimmed() + "." + QString(intToString(VER_BUILD, tempChar)).trimmed() + "." + QString(intToString(VER_TAG, tempChar)).trimmed();
		p.drawText(rcTemp[1][1], Qt::AlignLeft | Qt::AlignVCenter, displayQS);

#if 1
		//绘制模块软件版本号
		//	displayQS = "Module Software Ver:\t" + sysScpi->moduSoftVer;
		//	p.drawText(rcTemp[0][3],  Qt::AlignLeft | Qt::AlignVCenter, displayQS);

		//绘制序列号
		displayQS = "SN:\t" + sysScpi->moduSerial;
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
	QRect rc = QRect(WIN2X, WIN2Y, WIN2WIDTH, WIN2HEIGHT);	
	
	QRect rcTemp = QRect(BUTTONINTERVAL, BUTTONINTERVAL + BUTTONTITLEHEIGHT, rc.width(), (rc.height() - 2 * BUTTONTITLEHEIGHT) / BUTTONCOUNT - 1);//按键矩形框
	QRect rcButton;
	int btnIndex = 0;
	int btnNums = 0;

	QPainter p;

	QPixmap pix(rc.size()); //QT:像素映射
	p.begin(&pix);
	p.setFont(fontData);	//设置绘制字体
	p.fillRect(pix.rect(), BACKCOLORWIN5); //绘制按键后面区域为默认背景色
	p.setBrush(QBrush(QColor(100,149,237))); 

	//绘制菜单标题
	QPointF titlePoints[] = { 
		QPointF(0, 0),
		QPointF(rcTemp.width()-1, 0),
		QPointF(rcTemp.width() - (BUTTONTITLEWIDTH / 5), BUTTONTITLEHEIGHT) ,
		QPointF(0, BUTTONTITLEHEIGHT)
	};
	QRect rcTitleName = QRect(0, 0, BUTTONTITLEWIDTH / 6 * 5, BUTTONTITLEHEIGHT);
	p.drawPolygon(titlePoints, 4);	//绘制菜单标题框
	p.drawText(rcTitleName, Qt::AlignHCenter | Qt::AlignVCenter, QString(sysData.titleName));//绘制菜单标题名

	//绘制More键信息
	if (sysData.moreKeyCount.Count > 0)
	{
		QPointF morePoints[] = {
			QPointF(0, WIN2HEIGHT - BUTTONTITLEHEIGHT),
			QPointF(rcTemp.width() - (BUTTONTITLEWIDTH / 5), WIN2HEIGHT - BUTTONTITLEHEIGHT),
			QPointF(rcTemp.right() - 1, WIN2HEIGHT),
			QPointF(0, WIN2HEIGHT)
		};
		p.drawPolygon(morePoints, 4);	//绘制More信息
		QRect rcMore = QRect(rcTemp.x(), WIN2HEIGHT - BUTTONTITLEHEIGHT, BUTTONTITLEWIDTH / 6 * 5, BUTTONTITLEHEIGHT);
		p.drawText(rcMore, Qt::AlignHCenter | Qt::AlignVCenter, "More " + QString::number(sysData.moreKeyCount.index + 1) + " of  " + QString::number(sysData.moreKeyCount.Count + 1));//绘制菜单标题名
		//点亮More键指示灯
	}

	//------------------------------------------按键无效或错误 button is invalid or error----------------------------------------
	if (((nowMainKey == NULL) && (nowSubKey == NULL)) || ((nowMainKey == NULL) && (nowSubKey != NULL))) //如果按键无效或错误，则只绘制按键，不绘制文字信息
	{
		for (int i = 0; i < BUTTONCOUNT; i++)
		{
			if (i == 0)
			{
				//第一个按键不是矩形需要特殊处理
				QPointF FirstButtonPoints[] = { QPointF(rcTemp.x(), rcTemp.y()),
					QPointF(rcTemp.x() + rcTemp.width() / 6 * 5, rcTemp.y()),
					QPointF(rc.right(), rc.y() + BUTTONINTERVAL) ,
					QPointF(rcTemp.right(), rcTemp.bottom()),
					QPointF(rcTemp.left(), rcTemp.bottom())
				};
				p.setBrush(QBrush(BACKCOLORWIN5));
				p.drawPolygon(FirstButtonPoints, 5);	//绘制第一个按键
			}
			else if (i == BUTTONCOUNT - 1)
			{
				//最后一个按键不是矩形需要特殊处理
				QPointF LastButtonPoints[] = { QPointF(rcTemp.x(), rcTemp.y() + (rcTemp.height() * (BUTTONCOUNT - 1))),
					QPointF(rcTemp.right(), rcTemp.y() + (rcTemp.height() * (BUTTONCOUNT - 1))),
					QPointF(rcTemp.right(), rc.bottom() - BUTTONINTERVAL) ,
					QPointF(rcTemp.right() - rcTemp.width() / 6 * 5, rc.bottom() - BUTTONTITLEHEIGHT - BUTTONINTERVAL),
					QPointF(rcTemp.left(), rc.bottom() - BUTTONTITLEHEIGHT - BUTTONINTERVAL)
				};
				p.setBrush(QBrush(BACKCOLORWIN5));
				p.drawPolygon(LastButtonPoints, 5);	//绘制最后一个按键
			}
			else
			{
				rcButton = QRect(rcTemp.left(), rcTemp.y() + rcTemp.height() * i + BUTTONINTERVAL, rcTemp.width(), rcTemp.height());
				qDrawWinPanel(&p, rcButton, palette(), false, 0);
			}
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
		
		//node loop
		bool isNeedDrawButtonRect = false;
		while (keyTemp != NULL)
		{			
			btnIndex = sysMenu->getButtonIndex(keyTemp->keyValue); //通过btnIndex选择要绘制的按键索引
			btnNums++;
			if (btnNums > BUTTONCOUNT + 1) //如果索引越界，则返回
			{
				break;
			}		
			if ((btnIndex >= 0) && (btnIndex < BUTTONCOUNT)) //如果索引未越界则绘制
			{
				if (btnIndex == 0)
				{
					//第一个按键不是矩形需要特殊处理
					QPointF FirstButtonPoints[] = { QPointF(1, rcTemp.y() + 1),
						QPointF(rcTemp.width() - (BUTTONTITLEWIDTH / 5), rcTemp.y() + 1),
						QPointF(rcTemp.width(), rc.top() + 1) ,
						QPointF(rcTemp.width(), rcTemp.bottom() + 1),
						QPointF(1, rcTemp.bottom() + 1)
					};
					if (keyTemp == nowSubKey)
					{
						p.setBrush(QBrush(BUTTONSELECTCOLOR));
					}
					else
					{
						p.setBrush(QBrush(BUTTONUNSELECTCOLOR));
					}
					if (!keyTemp->isEnabled)
					{
						p.setBrush(QBrush(BUTTONUNSELECTCOLOR));
					}
					p.drawPolygon(FirstButtonPoints, 5);	//绘制第一个按键
					isNeedDrawButtonRect = false;
					rcButton = QRect(rcTemp.x(), rcTemp.y() + rcTemp.height() * btnIndex + (btnIndex + 1)*BUTTONINTERVAL + 1, rcTemp.width(), rcTemp.height());
					QPixmap pix(rcButton.width() - 2, rcButton.height() - 2);
					drawButtonMenu(&pix, keyTemp, isNeedDrawButtonRect); //将按键上的文本及位置等信息写入缓存
					p.drawPixmap(rcButton.left() + 1, rcButton.top(), pix); //显示	
				}
				else if (btnIndex == BUTTONCOUNT - 1)
				{
					//最后一个按键不是矩形需要特殊处理
					QPointF LastButtonPoints[] = { QPointF(rcTemp.x(), rcTemp.y() + (rcTemp.height()  * (BUTTONCOUNT - 1) + BUTTONINTERVAL * BUTTONCOUNT)),
						QPointF(rcTemp.right(), rcTemp.y() + (rcTemp.height()  * (BUTTONCOUNT - 1) + BUTTONINTERVAL * BUTTONCOUNT)),
						QPointF(rcTemp.right(), rc.bottom()) ,
						QPointF(rcTemp.right() - rcTemp.width() / 5, rc.bottom() - BUTTONTITLEHEIGHT - BUTTONINTERVAL),
						QPointF(rcTemp.x(), rc.bottom() - BUTTONTITLEHEIGHT - BUTTONINTERVAL)
					};
					if (keyTemp == nowSubKey)
					{
						p.setBrush(QBrush(BUTTONSELECTCOLOR));
					}
					else
					{
						p.setBrush(QBrush(BUTTONUNSELECTCOLOR));
					}
					if (!keyTemp->isEnabled)
					{
						p.setBrush(QBrush(BUTTONUNSELECTCOLOR));
					}
					p.drawPolygon(LastButtonPoints, 5);	//绘制最后一个按键
					isNeedDrawButtonRect = false;
					rcButton = QRect(rcTemp.x(), rcTemp.y() + rcTemp.height() * btnIndex + (btnIndex + 1)*BUTTONINTERVAL, rcTemp.width(), rcTemp.height());
					QPixmap pix(rcButton.width() - 2, rcButton.height() - 3);
					drawButtonMenu(&pix, keyTemp, isNeedDrawButtonRect); //将按键上的文本及位置等信息写入缓存
					p.drawPixmap(rcButton.left() + 1, rcButton.top()+1, pix); //显示	
				}
				else
				{
					isNeedDrawButtonRect = true;
					rcButton = QRect(rcTemp.x(), rcTemp.y() + rcTemp.height() * btnIndex + (btnIndex + 1)*BUTTONINTERVAL, rcTemp.width(), rcTemp.height());
					QPixmap pix(rcButton.width()-1, rcButton.height());
					drawButtonMenu(&pix, keyTemp, isNeedDrawButtonRect); //将按键上的文本及位置等信息写入缓存
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
	drawMainPara(); //绘制主参数显示区
	drawState(); //绘制分隔栏显示区
	drawButton();
}

void mainForm::drawErrorMessage(QString msg)
{
	QRect rc = QRect(WIN1_5X, WIN1_5Y, 150, WIN1_5HEIGHT + 1);
	QRect rcHelp = QRect(BORDERWIDTH, 0, rc.width() - 3 * BORDERWIDTH, rc.height() - 2 * BORDERWIDTH);
	QPainter p;
	QPixmap pix(rc.size());

	if (!pix.isNull())
	{
		p.begin(&pix);
		p.fillRect(pix.rect(), BACKCOLORALLWIN);
		p.fillRect(rcHelp, BACKCOLORWIN4);
		//p.drawRect(rcHelp);
		//QBrush b(QColor(128,128,64));
		//qDrawShadePanel(&p, rcPoint, palette(), true, FRAMELINEWIDTH, &b);
		p.setPen(QColor(255, 0, 0));
		p.setFont(fontWin4);
		rcHelp = QRect(rcHelp.left() + 5, rcHelp.top(), rcHelp.width() - 10, rcHelp.height());
		p.drawText(rcHelp, Qt::AlignLeft | Qt::AlignVCenter, msg);
		p.end();
		QScreen::instance()->blit(pix.toImage(), QPoint(rc.left(), rc.top()), rc);
	}
	usleep(2000 * 1000);
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
	sysScpi->drawWin1_5(helpContent);
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

			sysScpi->drawWin1_5(helpContent);
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

	sysScpi->drawWin1_5(helpContent);
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
				case KEY_CTL_ARROW_UP:
					index = 0;
					break;
				case KEY_CTL_ARROW_DOWN:
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
	strcpy(sysData.titleName, getTheTranslatorString("Freq").toStdString().c_str());
	sysStart = true;
	imgStart = new QPixmap(SCREENWIDTH, SCREENHEIGHT);
	drawMainPara();
	drawState();
	drawFreqInParamter();
	drawSystemInf();
	drawButton();
	strcpy(sysData.name, "");
	strcpy(sysData.value, "");
	strcpy(sysData.unit, "");
	drawInputParamter();
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

	/* zpc add test */
	//将老信号源的键盘转换成新的键盘
	//switch (recvbuf[3])
	//{
	//	case 0xC6:		recvbuf[3] = 0x17; break;	//preset
	//	case 0xCA:		recvbuf[3] = 0x13; break;	//FREQ
	//	case 0xD5:		recvbuf[3] = 0x23; break;	//AMPL
	//	case 0xD2:		recvbuf[3] = 0x33; break;	//SWEEP
	//	case 0xD8:		recvbuf[3] = 0x25; break;	//Fctn
	//	case 0xD9:		recvbuf[3] = 0x26; break;	//Aux -> Trigger
	//	case 0xDA:		recvbuf[3] = 0x35; break;	//MOD
	//	case 0xDB:		recvbuf[3] = 0x34; break;	//LF
	//	case 0xDC:		recvbuf[3] = 0x37; break;	//RF1
	//	case 0xC0:		recvbuf[3] = 0x41; break;	//sub1
	//	case 0xC1:		recvbuf[3] = 0x42; break;	//sub2
	//	case 0xC2:		recvbuf[3] = 0x43; break;	//sub3
	//	case 0xC3:		recvbuf[3] = 0x44; break;	//sub4
	//	case 0xC4:		recvbuf[3] = 0x45; break;	//sub5
	//	case 0xC5:		recvbuf[3] = 0x55; break;	//More
	//	case 0xD7:		recvbuf[3] = 0x54; break;	//0
	//	case 0xCF:		recvbuf[3] = 0x53; break;	//1
	//	case 0xD0:		recvbuf[3] = 0x63; break;	//2
	//	case 0xD1:		recvbuf[3] = 0x73; break;	//3
	//	case 0xCB:		recvbuf[3] = 0x52; break;	//4
	//	case 0xCC:		recvbuf[3] = 0x62; break;	//5
	//	case 0xCD:		recvbuf[3] = 0x72; break;	//6
	//	case 0xC7:		recvbuf[3] = 0x51; break;	//7
	//	case 0xC8:		recvbuf[3] = 0x61; break;	//8
	//	case 0xC9:		recvbuf[3] = 0x71; break;	//9
	//	case 0xD4:		recvbuf[3] = 0x64; break;	//.
	//	case 0xCE:		recvbuf[3] = 0x74; break;	//-
	//	case 0xD6:		recvbuf[3] = 0x65; break;	//Return
	//	default:
	//		break;
	//}
	/* add test end */

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
	}
	else
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

		if (keyValue == KEY_CTL_ARROW_LEFT) //左右键用来移动选中框
		{
			tempIndex -= 1;
			indexed = true;
		} else if (keyValue == KEY_CTL_ARROW_RIGHT) //左右键用来移动选中框
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
	
	//按下主键，包含FREQ, AMPT, SWEEP, AM, FM/PM, IQ, PRESET, PULSE, AUXFCTN, TRIGGER, SYSTEM, LOFOUT, MODON/OFF, RF2ON/OFF, RF1ON/OFF
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
		//默认选中第一个使能的子键
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
						break;
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
	if ((sysMenu->pressedIsCommonKey(keyValue)) && keyIsEnable(keyValue))
	{
        
        if (nowMainKey->keyValue == KEY_MAIN_FREQ || nowMainKey->keyValue == KEY_MAIN_AMPTD)
        {
            sysData.cursorLocation = -1;
        }
        else
        {
            sysData.cursorLocation = 0; //清零光标位置
        }
		if (sysMenu->getSubKeyPointer(keyValue))
		{
			handleMoreKey(nowSubKey->funcCode);	//处理
			
			//isMore
			if ((nowSubKey->isMore) && (nowSubKey->rightNode != NULL))
			{
				if (nowSubKey->isPassword) //是需要密码（“系统服务”按键）,则不执行gotoDownLevel()函数
				{
					previndexOfPassword = 0;
					indexOfPassword = 0;
					memset(passwdString, 0, sizeof passwdString);
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
				sysMenu->gotoUpLevel();
				canExecute = false;
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
				handleExecuteKey(nowSubKey->funcCode);	//处理执行键
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
					sysData.moreKeyCount.index = 0;
					char tempValue[64] = { 0 };
					if (nowSubKey->funcCode == KEY_FUNC_AMPL)
					{
						strcpy(sysData.name, "AMPL:");
						sprintf(tempValue, "%.2f", sysData.ampl.ampl);
						strcpy(sysData.value, tempValue);
						strcpy(sysData.unit, amplToQUnit(sysData.ampl.ampl).trimmed().toStdString().c_str());
						drawInputParamter();
					}
				}
			}
		} else
		{
			helpContent = "pressed key is ignored";
		}
	}
	//按下物理More键
	else if ((nowMainKey != NULL) && (nowSubKey != NULL) && sysMenu->pressedIsHardMoreKey(keyValue))
	{
		handleHardMoreKey();
		//如果More键后是输入键，需要处理
		if (nowSubKey != NULL & nowSubKey->isInput)
		{
			handleInputKey(nowSubKey->funcCode);
		}
	}
	//按下物理Return键
	else if ((nowMainKey != NULL) && (nowSubKey != NULL) && sysMenu->pressedIsHardReturnKey(keyValue))
	{
		handleHardReturnKey();
		afterReturnDraw();
	}
	//按下BankSpace键
	else if ((nowMainKey != NULL) && (nowSubKey != NULL) && sysMenu->pressedIsBkspaceKey(keyValue))
	{
		if (nowSubKey->isUnit || nowSubKey->isPassword)
		{
			if (!inputValue.isEmpty())
			{
				if (nowSubKey->isPassword) //如果是密码
				{
					indexOfPassword--;
					previndexOfPassword--;
				}
				inputValue.remove(inputValue.length() - 1, 1);
				strcpy(sysData.value, inputValue.toStdString().c_str()); //zpc add
				drawInputParamter();
			}
			return;
		}
	}
	//按下方向键
	else if ((nowMainKey != NULL) && (nowSubKey != NULL) && (nowSubKey->isEnabled) && (sysMenu->pressedIsArrowKey(keyValue)))
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
		handleArrowKey(nowSubKey->funcCode, keyValue);//处理方向键
	}
	//按下Select键
	else if ((nowMainKey != NULL) && (nowSubKey != NULL) && (nowSubKey->isEnabled) && (sysMenu->pressedIsSelectKey(keyValue)))
	{
		handleSelectKey(nowSubKey->funcCode);
	}
	//按下IncrSet键
	else if (sysMenu->pressedIsIncrsetKey(keyValue))
	{
		handleIncrsetKey(nowSubKey->funcCode);
	}
	//按下Trigger键
	else if (nowMainKey != NULL && sysMenu->pressedIsTriggerKey(keyValue))
	{
		handleTriggerKey(nowSubKey->funcCode);
	}
	//按下PageUp键
	else if (nowMainKey != NULL && sysMenu->pressedIsPageUpKey(keyValue))
	{
		handlePageUpKey(nowMainKey->keyValue);
	}
	//按下PageDown键
	else if (nowMainKey != NULL && sysMenu->pressedIsPageDownKey(keyValue))
	{
		handlePageDownKey(nowMainKey->keyValue);
	}
	//按下RPG键
	else if ((nowMainKey != NULL) && (nowSubKey != NULL) && (nowSubKey->isEnabled) && (sysMenu->pressedIsRpgKey(keyClass)))
	{
		handleRpgKey(nowSubKey->funcCode, sysMenu->getRpgPulse(keyValue, keyCount));
	}
	//按下数字键
	else if ((sysMenu->pressedIsNumberKey(keyValue)) && (nowSubKey != NULL) && (nowSubKey->isEnabled))
	{
        if (nowMainKey->funcCode == KEY_MAIN_FREQ || nowMainKey->funcCode == KEY_MAIN_AMPTD)
        {
            sysData.cursorLocation = -1;
        }
        else
        {
            sysData.cursorLocation = 0; //清零光标位置
        }
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
				drawInputParamter();
				if (__DEBUGKEY)
					printf("KEY_NUM_0 = %d\r\n", inputValue.toInt());
				break;
			case KEY_NUM_1:
				execNumberInput(QString("1"), true);
				drawInputParamter();
				if (__DEBUGKEY)
					printf("KEY_NUM_1 = %d\r\n", inputValue.toInt());
				break;
			case KEY_NUM_2:
				execNumberInput(QString("2"), true);
				drawInputParamter();
				if (__DEBUGKEY)
					printf("KEY_NUM_2 = %d\r\n", inputValue.toInt());
				break;
			case KEY_NUM_3:
				execNumberInput(QString("3"), true);
				drawInputParamter();
				if (__DEBUGKEY)
					printf("KEY_NUM_3 = %d\r\n", inputValue.toInt());
				break;
			case KEY_NUM_4:
				execNumberInput(QString("4"), true);
				drawInputParamter();
				if (__DEBUGKEY)
					printf("KEY_NUM_4 = %d\r\n", inputValue.toInt());
				break;
			case KEY_NUM_5:
				execNumberInput(QString("5"), true);
				drawInputParamter();
				if (__DEBUGKEY)
					printf("KEY_NUM_5 = %d\r\n", inputValue.toInt());
				break;
			case KEY_NUM_6:
				execNumberInput(QString("6"), true);
				drawInputParamter();
				if (__DEBUGKEY)
					printf("KEY_NUM_6 = %d\r\n", inputValue.toInt());
				break;
			case KEY_NUM_7:
				execNumberInput(QString("7"), true);
				drawInputParamter();
				if (__DEBUGKEY)
					printf("KEY_NUM_7 = %d\r\n", inputValue.toInt());
				break;
			case KEY_NUM_8:
				execNumberInput(QString("8"), true);
				drawInputParamter();
				if (__DEBUGKEY)
					printf("KEY_NUM_8 = %d\r\n", inputValue.toInt());
				break;
			case KEY_NUM_9:
				execNumberInput(QString("9"), true);
				drawInputParamter();
				if (__DEBUGKEY)
					printf("KEY_NUM_9 = %d\r\n", inputValue.toInt());
				break;
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
				drawInputParamter();
				break;
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
				drawInputParamter();
				break;
		}
	}
	//按下取消键
	else if ((sysMenu->pressedIsEscKey(keyValue)) && (nowSubKey != NULL))
	{
		if (nowSubKey->isUnit)
		{
			sysMenu->gotoUpLevel();
			drawInputParamter();
		}
		inputValue = "";
		return;
	}	
	//其它未知键
	else
	{
		return;
	}
	if (HaveMoreKeyCount() > 0)	//检查More键
	{
		//if (!sysData.led.More)		//原本是熄灭的则点亮
		{
			//printf("zpc add test, Control More LED On\n");
			sysData.led.More = true;
			//下发指令到面板
			sysScpi->keyBoardLEDSwitch(2, sysData.led.More);
		}
	}
	else
	{
		//if (sysData.led.More)	//原本是点亮的则熄灭
		{
			//printf("zpc add test, Control More LED Off\n");
			sysData.led.More = false;
			//下发指令到面板
			sysScpi->keyBoardLEDSwitch(2, sysData.led.More);
		}
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
	} 
	else if (isMacAddress()) //MAC地址
	{
		if (tempchar.remove('-').remove('.').length() < sysData.inputLength)
		{
			inputValue += value;
			sysData.system.lanMacAddrIndex++;
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
}

//口令
bool mainForm::isPassword(void)
{
	//  return false;
	return nowSubKey != NULL && nowSubKey->funcCode == KEY_FUNC_SYSTEM_SERVICE;
}

//MAC地址
bool mainForm::isMacAddress(void)
{
	return nowSubKey != NULL && nowSubKey->funcCode == KEY_FUNC_SYSTEM_SERVICE_LANMACADDR;
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
			case KEY_FUNC_SYSTEM_LAN_IP_ENTER:
				iType = itIP;
				break;
			case KEY_FUNC_SYSTEM_LAN_MASK_ENTER:
				iType = itIP;
				break;
			case KEY_FUNC_SYSTEM_LAN_GATEWAY_ENTER:
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
			//Incr
		case KEY_FUNC_INCRSET_SET:
		case KEY_FUNC_INCRSETSET_GHZ:
		case KEY_FUNC_INCRSETSET_MHZ:
		case KEY_FUNC_INCRSETSET_KHZ:
		case KEY_FUNC_INCRSETSET_HZ:
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
		case KEY_FUNC_AM_DEPTH:
		case KEY_FUNC_AM_DEPTH_PERCENT:
		case KEY_FUNC_AM_INTERNALFCTN_FREQ:
		case KEY_FUNC_AM_INTERNALFCTN_FREQ_MHZ:
		case KEY_FUNC_AM_INTERNALFCTN_FREQ_KHZ:
		case KEY_FUNC_AM_INTERNALFCTN_FREQ_HZ:
		case KEY_FUNC_FMPM_OFFSET:
		case KEY_FUNC_FMPM_OFFSET_MHZ:
		case KEY_FUNC_FMPM_OFFSET_KHZ:
		case KEY_FUNC_FMPM_OFFSET_HZ:
		case KEY_FUNC_FMPM_INTERNALFCTN_FREQ:
		case KEY_FUNC_FMPM_INTERNALFCTN_FREQ_MHZ:
		case KEY_FUNC_FMPM_INTERNALFCTN_FREQ_KHZ:
		case KEY_FUNC_FMPM_INTERNALFCTN_FREQ_HZ:
			//Digital
		case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA_DEPTH:
		case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA_DEPTH_PERCENT:
		case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA_DEVI:
		case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA_DEVI_MHZ:
		case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA_DEVI_KHZ:
		case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA_DEVI_HZ:
		case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA_PHASE:
		case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA_PHASE_DEG:
		case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA_RATE:
		case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA_RATE_MHZ:
		case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA_RATE_KHZ:
		case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA_RATE_HZ:
		case KEY_FUNC_AUXFCTN_DIGITALMOD_EXTLOAD_ENTER:
			//Pulse
		case KEY_FUNC_PULSE_PERIOD:
		case KEY_FUNC_PULSE_PERIOD_S:
		case KEY_FUNC_PULSE_PERIOD_MS:
		case KEY_FUNC_PULSE_PERIOD_US:
		case KEY_FUNC_PULSE_PERIOD_NS:
		case KEY_FUNC_PULSE_WIDTH:
		case KEY_FUNC_PULSE_WIDTH_S:
		case KEY_FUNC_PULSE_WIDTH_MS:
		case KEY_FUNC_PULSE_WIDTH_US:
		case KEY_FUNC_PULSE_WIDTH_NS:
		case KEY_FUNC_PULSE_DELAY:
		case KEY_FUNC_PULSE_DELAY_S:
		case KEY_FUNC_PULSE_DELAY_MS:
		case KEY_FUNC_PULSE_DELAY_US:
		case KEY_FUNC_PULSE_DELAY_NS:
			//LF
		case KEY_FUNC_AUXFCTN_LFOUT_AMPL:
		case KEY_FUNC_AUXFCTN_LFOUT_AMPL_MV:
		case KEY_FUNC_AUXFCTN_LFOUT_AMPL_V:
		case KEY_FUNC_AUXFCTN_LFOUT_FREQ:
		case KEY_FUNC_AUXFCTN_LFOUT_FREQ_KHZ:
		case KEY_FUNC_AUXFCTN_LFOUT_FREQ_HZ:
			//Extre
		case KEY_FUNC_AUXFCTN_EXTRF_EXTIF:
		case KEY_FUNC_AUXFCTN_EXTRF_EXTIF_MHZ:
		case KEY_FUNC_AUXFCTN_EXTRF_EXTIF_KHZ:
		case KEY_FUNC_AUXFCTN_EXTRF_EXTIF_HZ:
			//DRG
		case KEY_FUNC_AUXFCTN_DRG_UPPER:
		case KEY_FUNC_AUXFCTN_DRG_UPPER_GHZ:
		case KEY_FUNC_AUXFCTN_DRG_UPPER_MHZ:
		case KEY_FUNC_AUXFCTN_DRG_UPPER_KHZ:
		case KEY_FUNC_AUXFCTN_DRG_UPPER_HZ:
		case KEY_FUNC_AUXFCTN_DRG_LOW:
		case KEY_FUNC_AUXFCTN_DRG_LOW_GHZ:
		case KEY_FUNC_AUXFCTN_DRG_LOW_MHZ:
		case KEY_FUNC_AUXFCTN_DRG_LOW_KHZ:
		case KEY_FUNC_AUXFCTN_DRG_LOW_HZ:
		case KEY_FUNC_AUXFCTN_DRG_POINTS:
		case KEY_FUNC_AUXFCTN_DRG_POINTS_ENTER:
		case KEY_FUNC_AUXFCTN_DRG_RATE:
		case KEY_FUNC_AUXFCTN_DRG_RATE_NS:
		case KEY_FUNC_AUXFCTN_DRG_RATE_US:
			//AUX
		case KEY_FUNC_SYSTEM_LAN_IP:
		case KEY_FUNC_SYSTEM_LAN_IP_ENTER:
		case KEY_FUNC_SYSTEM_LAN_MASK:
		case KEY_FUNC_SYSTEM_LAN_MASK_ENTER:
		case KEY_FUNC_SYSTEM_LAN_GATEWAY:
		case KEY_FUNC_SYSTEM_LAN_GATEWAY_ENTER:
		case KEY_FUNC_SYSTEM_SERVICE:
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

		//case KEY_FUNC_FCTN_ANALOG_FM:
		//case KEY_FUNC_FCTN_ANALOG_PM:
		case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA:
		case KEY_FUNC_AUXFCTN_LFOUT:
		case KEY_FUNC_AUXFCTN_DRG:
		case KEY_FUNC_AUXFCTN_EXTRF:
		case KEY_FUNC_SYSTEM_LAN:
			strcpy(sysData.name, "");
			strcpy(sysData.value, "");
			strcpy(sysData.unit, "");
			drawInputParamter();
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
	sysScpi->drawWin1_5("Load data file...");

	for (int i = 0; i < 10; i++)
	{
		if (system("mount -t vfat /dev/sda1 /mnt/usb/") == 0)
		{
			break;
		}
		usleep(1000 * 1000);
		if (i == 9)
		{
			sysScpi->drawWin1_5("Load data file failed");
			usleep(1000 * 1000);
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
	QRect rc = QRect(WIN1_4X + 1, WIN1_4Y, WIN1_4WIDTH - 1, WIN1_4HEIGHT - 1);
	QRect rcTemp[2][16];
	QRect rcTitle[2]; //标题栏分为两列
	QPainter p;
	QPixmap pix(rc.size());
	int row = 6;
	int titelRightWidth = 80;
	if (!pix.isNull())
	{
		p.begin(&pix);
		//计算绘制空间
		QRect rcTemp1 = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width(), pix.rect().height()); //整个参数区（标题和参数）
		QRect rcPara = QRect(pix.rect().x(), pix.rect().y() + pix.rect().height() / 4, pix.rect().width(), pix.rect().height() / row * 4); //参数区
		rcTitle[0] = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width() - titelRightWidth, pix.rect().height() / row); //标题区0
		rcTitle[1] = QRect(pix.rect().width() - titelRightWidth, pix.rect().y(), titelRightWidth, pix.rect().height() / row); //标题区

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
		displayString = "Page: 1/1";
		p.drawRect(rcTitle[1]);
		p.drawText(rcTitle[1].x() + 2, rcTitle[1].y(), rcTitle[1].width(), rcTitle[1].height(), Qt::AlignCenter, displayString);
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

