#include "mainform.h"

#define BAUDRATE B115200
#define KBD_PORT "/dev/ttyS2"

//=====================================================================
//���沼�ֲַ���
//		�˵�����ߵĲ���Ϊ win1, �˵���Ϊwin2
//		win1���ϵ��·�Ϊ win1_1, win1_2, win1_3, win1_4, win1_5
//		win1_1��Ϊwin1_1_1��win1_1_2
//				win1_1_1�ǻ���Ƶ��������, win1_1_2�ǻ��Ʒ�����������
//					��Ӧ�Ĳ������� drawMainPara, drawMainParaFreq, drawMainParaAmptd
//		win1_2��״̬��ʾ������Ӧ�Ĳ�������drawState
//		win1_3�����������ʾ������Ӧ�Ĳ������� drawInputParamter
//		win1_4�ǹ��ܲ�����ʾ������Ӧ�Ĳ������ɸ����ܾ���
//		win1_5��ϵͳ������ʾ������ʾʱ��ȣ���Ӧ�Ĳ������� drawSystemInf
//=====================================================================


int keyCount;

//���캯��
mainForm::mainForm(QWidget* parent) :
	QWidget(parent = 0)
{
	//����SCPI��Ӧ
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
	//����˵�
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
	connect(sysScpi, SIGNAL(resetUsbSlaveInterface()), this, SLOT(resetUsbSlaveInterface())); //��λUSB�ӿ�
	//��ȡϵͳͼ��ͼƬ
	getLogoPicture();
	//��������
	loadSystemDriver();
	
	//��ʼ�����̰�LEDָʾ��
	sysScpi->initKeyBoardLED();
	sysScpi->keyBoardLEDAllOpen();

	//��ʼ������
	sysScpi->presetSystemData();
	//��λ����
	sysScpi->resetSystemIOOfLan();
	//��ʼ�����̰�LEDָʾ��
	sysScpi->initKeyBoardLED();
	//��������
//	sysScpi->loadPowerOffState();

	sysScpi->controlParamterDown();
	//��ʾ��������
	showSplashScreen();
	evaDay = -1;
	evaIndex = 0;
	memset(evaArray, 0x0, sizeof evaArray);

	//Ĭ��ѡ��Ƶ��
	readKeyBoardData();

	int i = 0;
	while (true)
	{
		sysScpi->sendSerialData(QString("*IDNN?")); //�·�*IDNN?����ȡģ��������Ϣ
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
	//����Դָʾ�Ʊ���
	sysScpi->keyBoardLEDSwitch(1, true);
	//sysScpi->keyBoardLEDSwitch(2, true);//��������Ƶ�ʲ˵��£�������More�˵�������Ҫ����
}

//����SCPI״̬
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

	indexStr = qSitStr.indexOf('-', 0); //��ȡ��-���ַ����ַ����е�λ��
	//  printf("index = %d\n",indexStr);

	QRect rcDisplay1; //'.'�ַ���ʾλ��
	QRect rcDisplay2[16]; //' '�ַ���ʾλ��
	QRect rcDisplay3[16]; //��'.'��' '��������ַ���ʾλ��
	QRect rcDisplay4;

	int length = tempQSitStr.remove('.').remove('-').remove(' ').length();
	if (index >= length)
	{
		for (int i = 0; i < (index - length + 1); i++)
		{
			//		qSitStr.insert(0,'0');				//ԭ���Ĵ�����

			if (indexStr != -1) //zpc add,ȥ������һ�����µ��ǡ�-����ʱ������ʾ��0-����bug
			{
				if (index == 0 && length == 0) //����һ�����µ��ǡ�-����ʱ������ʾ��0-��
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

/* ����������ʾƵ��ֵ
 * ʹ�����Ҽ�����ť�ı�Ƶ��ֵ*/
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

	QRect rcDisplay1; //'.'�ַ���ʾλ��
	QRect rcDisplay2[16]; //' '�ַ���ʾλ��
	QRect rcDisplay3[16]; //��'.'��' '��������ַ���ʾλ��
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

/* ����������ʾ����
 * ʹ�����Ҽ�����ť�ı�Ƶ��ֵ*/
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

	QRect rcDisplay1; //'.'�ַ���ʾλ��
	QRect rcDisplay2[16]; //' '�ַ���ʾλ��
	QRect rcDisplay3[16]; //��'.'��' '��������ַ���ʾλ��
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

/* ����������ʾ����ֵ
 * ʹ�����Ҽ�����ť�ı书��ֵ*/
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

	QRect rcDisplay1; //'.'�ַ���ʾλ��
	QRect rcDisplay2[16]; //' '�ַ���ʾλ��
	QRect rcDisplay3[16]; //��'.'��' '��������ַ���ʾλ��
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


//������������ʾ����Ƶ����ʾ����win1_1_1��
void mainForm::drawMainParaFreq(void)
{
	if (sysScpi->isUpdating)
	{
		return;
	}
	//------------------------------------------------�ڴ�ͼ�����------------------------------------------------------------
	QRect rc = QRect(WIN1_1_1X, WIN1_1_1Y, WIN1_1_1WIDTH, WIN1_1_1HEIGHT);				//win1_1_1��������
	QPainter p;
	QPixmap pix(rc.size());
	char tempChar[64] = {};

	if (!pix.isNull())
	{
		double upMidPercent = 0.35;
		double downMidPercent = 0.4;
		double namePercent = 0.3;
		p.begin(&pix);
		p.fillRect(pix.rect(), BACKCOLORWIN1); //��䴰��1ȫ������ı���ɫ
		//���ơ�Frequency���ַ���
		p.drawLine(QPointF(0, 0), QPointF(rc.width() * upMidPercent + 2, 0));
		p.drawLine(QPointF(rc.width() * upMidPercent + 2, 0), QPointF(rc.width() * downMidPercent, rc.height() * namePercent));
		QRect rcName = QRect(0, 2, rc.width() * upMidPercent, rc.height() * namePercent);
		p.fillRect(rcName, BACKCOLORWIN1);
		p.setFont(fontEnglishSmallBold);
		p.drawText(rcName, Qt::AlignLeft | Qt::AlignVCenter, QString("FREQUENCY"));	//���ơ�Frequency���ַ���
		//����Frequency�ĸ���״̬����ο���ƫ�õ�
		QPointF statusPoints[] = {
			QPointF(rc.width() * upMidPercent + 2, 0),
			QPointF(rc.width(), 0),
			QPointF(rc.width(), rc.height() * namePercent),
			QPointF(rc.width() * downMidPercent, rc.height() * namePercent)
		};
		p.setBrush(QBrush(QColor(0, 0, 255)));
		p.drawPolygon(statusPoints, 4);	//�����ı��ο�
		//����REF
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
		//����Offset
		if (sysData.freq.offset > 0)
		{
			QRect rcOffset = QRect(rc.width() * 0.65, 1, rc.width() * 0.25, rc.height() * 0.3);
			p.fillRect(rcOffset, BACKCOLORREFOFFSET);
			//qDrawPlainRect(&p, rcRef, GRIDCOLOR, 2, 0);
			p.drawText(rcOffset, Qt::AlignCenter, QString("OFFSET"));
		}
		//����Ƶ����ֵ
		if (sysData.sweep.mode == smAmpl || sysData.sweep.mode == smOff) //��ɨ��ģʽ�ǡ��ء���ɨ����ʱ��Ƶ����ʾ��ɫ������Ҷȴ���
		{
			p.setPen(Qt::black);
		}
		else
		{
			p.setPen(Qt::gray);
		}
		p.setFont(fontWin1En);
		QRect rcFreqValue = QRect(0, rc.height() * 0.3, rc.width() * 0.8, rc.height() * 0.7);	//����Ƶ����ֵ
		strcpy(tempChar, doubleToQValue(sysData.freq.freq).toStdString().c_str());
		p.drawText(rcFreqValue, Qt::AlignRight | Qt::AlignVCenter, insertThousandsCH(tempChar).trimmed());
		p.setFont(QFont("enb", 16, 75, false));
		QRect rcFreqUnit = QRect(rc.width() * 0.8, rc.height() * 0.3, rc.width() * 0.2, rc.height() * 0.7);		//����Ƶ�ʵ�λ
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
//������������ʾ����Ƶ������ʾ����win1_1_2��
void mainForm::drawMainParaAmptd(void)
{
	if (sysScpi->isUpdating)
	{
		return;
	}

	//------------------------------------------------�ڴ�ͼ�����------------------------------------------------------------
	QRect rc = QRect(WIN1_1_2X + 1, WIN1_1_2Y, WIN1_1_2WIDTH - 1, WIN1_1_2HEIGHT); //����1ȫ������
	QPainter p;
	QPixmap pix(rc.size());
	char tempChar[64] = {};
	if (!pix.isNull())
	{
		double upMidPercent = 0.45;
		double downMidPercent = 0.5;
		double namePercent = 0.3;
		p.begin(&pix);
		p.fillRect(pix.rect(), BACKCOLORWIN1); //��䴰��1ȫ������ı���ɫ
		QRect rcName = QRect(0, 2, rc.width() * upMidPercent, rc.height() * namePercent);
		if (sysData.led.RF1)	//����RF״̬����
		{
			p.fillRect(rcName, BACKCOLORWIN1);
			p.setFont(fontEnglishSmallBold);
			p.drawText(rcName, Qt::AlignLeft | Qt::AlignVCenter, QString("AMPLITUDE"));	//���ơ�Amplitude���ַ���
		}
		else
		{
			p.fillRect(QRect(rcName.x() + 5, rcName.y(), rcName.width() - 10, rcName.height()), QColor(255, 255, 0));
			p.drawRect(QRect(rcName.x() + 5, rcName.y(), rcName.width() - 10, rcName.height()));
			p.setFont(fontEnglishSmallBold);
			p.drawText(rcName, Qt::AlignCenter, QString("RF OFF"));	//���ơ�Amplitude���ַ���
		}
		//����Frequency�ĸ���״̬����ο���ƫ�õ�
		QPointF statusPoints[] = {
			QPointF(rc.width() * upMidPercent, 0),
			QPointF(rc.width(), 0),
			QPointF(rc.width(), rc.height() * namePercent),
			QPointF(rc.width() * downMidPercent, rc.height() * namePercent)
		};
		p.setBrush(QBrush(QColor(0, 0, 255)));
		p.drawPolygon(statusPoints, 4);	//�����ı��ο�
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

		//���Ʒ���ֵ
		QRect rcValue = QRect(1, rc.height() * namePercent, rc.width() * 0.7, rc.height() * (1 - namePercent));
		if (sysData.led.RF1 == true) //��RF״̬�ǿ���ʱ�򣬵�ɨ��ģʽ�ǡ��ء���ɨƵ��ʱ��������ʾ��ɫ������Ҷȴ���
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
		else //RF�ر�ʱ����Ҫ�Ҷȴ���
		{
			p.setPen(Qt::gray);
		}
		p.setFont(fontWin1En);
		sprintf(tempChar, "%.02f", sysData.ampl.ampl);
		p.drawText(rcValue, Qt::AlignRight | Qt::AlignVCenter, insertThousandsCH(tempChar).trimmed());
		//���Ʒ��ȵ�λ
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
//������������ʾ��
void mainForm::drawMainPara(void)
{
	drawMainParaFreq();
	drawMainParaAmptd();
}

void mainForm::drawFreqAndAmplMainSlots(void)
{
	drawMainPara();
}

//����״̬��ʾ������ win1_2
void mainForm::drawState(void)
{
	QString temp_qstring;
	if (sysScpi->isUpdating)
	{
		return;
	}

	if (sysScpi->eva.isEvaluate && evaDay != QDate::currentDate().day()) //�ж��Ƿ��������ڣ����������ڼ����Ƿ񳬹�ʹ��ʱ��
	{
		evaDay = QDate::currentDate().day();
		sysScpi->getSystemServiceOfEvaluteSYDays();
	}

	if (!sysScpi->eva.isEvaluate || (sysScpi->eva.isEvaluate && !sysScpi->eva.evaluated)) //����������δ����ʹ��ʱ�䣬����������״̬
	{
		//---------------------------------------------�ڴ�ͼ�����---------------------------------------------
		QRect rc = QRect(WIN1_2X + 1, WIN1_2Y + 1, WIN1_2WIDTH - 2, WIN1_2HEIGHT - 1); //����2ȫ������
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
			p.fillRect(pix.rect(), bulue); //��䴰��2ȫ������ı���ɫ
			//p.drawRect(rc);
			p.drawLine(QPointF(rc.x(), rc.y()), QPointF(rc.x(), rc.height()));
			p.drawLine(QPointF(rc.width(), rc.y()), QPointF(rc.width(), rc.height()));
			p.drawLine(QPointF(rc.x(), rc.height()), QPointF(rc.width(), rc.height()));
			//Զ�̿���
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
	else //����������
	{
		int evaNums = sizeof evaArray / sizeof evaArray[0];
		int evaWidth = 30;
		int evaHeight = 30;
		QRect rc[evaNums];
		QVector<QRect> rcS;
		char tempChar[32] = { 0x0 };
		QRect rcPoint = QRect(0, 0, WINALLWIDTH + 2, WINALLHEIGHT); //��������
		QRect rcEnter = QRect(WIN2X + 30, WIN2Y, WIN2WIDTH * 0.6, WIN2HEIGHT / 7); //ȷ�ϼ�
		QPainter p;
		QPixmap pix(rcPoint.size()); //��������

		p.begin(&pix);
		p.setPen(Qt::black);
		p.fillRect(rcPoint, QColor(10, 10, 10)); //���������Ļ����ɫ

		p.setPen(Qt::white);
		rc[0] = QRect(rcPoint.left() + rcPoint.width() / 2 - 3.5 * evaWidth, rcPoint.top() + rcPoint.height() / 2 - evaHeight / 2, evaWidth, evaHeight); //��ȷ��rc[0]�����꣬����ķ��������Ǹ���rc[0]�ļ��������
		rcS << rc[0];
		p.drawText(rc[0], Qt::AlignCenter, QString(intToHex(evaArray[0], tempChar)).trimmed());

		for (int i = 1; i < evaNums; i++) //����ڶ������ڰ˸�С�����λ��
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
			if (i == evaIndex) //��ǰ���������ʾΪ��ɫ
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

//����ʱ��������Ϣ�� win1_5
void mainForm::drawSystemInf(void)
{
	if (sysScpi->isUpdating)
	{
		return;
	}
	if (sysScpi->eva.isEvaluate && evaDay != QDate::currentDate().day()) //�ж��Ƿ��������ڣ����������ڼ����Ƿ񳬹�ʹ��ʱ��
	{
		evaDay = QDate::currentDate().day();
		sysScpi->getSystemServiceOfEvaluteSYDays();
	}
	if (!sysScpi->eva.isEvaluate || (sysScpi->eva.isEvaluate && !sysScpi->eva.evaluated)) //����������δ����ʹ��ʱ�䣬����������״̬
	{
		QRect rc = QRect(WIN1_5X + 1, WIN1_5Y, WIN1_5WIDTH - 2, WIN1_5HEIGHT + 1); //����2ȫ������
		QPainter p;
		QPixmap pix(rc.size());
		if (!pix.isNull())
		{
			p.begin(&pix);
			p.fillRect(pix.rect(), BACKCOLORWIN3); //��䴰��2ȫ������ı���ɫ
			p.drawRect(rc);
			
			QRect rcTime = QRect(rc.width() * 0.7, 0, rc.width() * 0.3, rc.height());
			QString timeStr = (QTime::currentTime()).toString("HH:mm:ss");	//��ȡϵͳʱ��
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

//������������ʾ������ win1_3
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
		//����������ʾ����
		QRect rcName = QRect(2, 0, rc.width() * 0.3, rc.height());
		//����ֵ��ʾ����
		QRect rcValue = QRect(rc.width() * 0.4, 0, rc.width() * 0.4, rc.height());
		//������λ��ʾ����
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
//����Ƶ�ʲ����ڲ�����ʾ��
void mainForm::drawFreqInParamter(void)
{
	if (sysScpi->isUpdating)
	{
		return;
	}

	QRect rc = QRect(WIN1_4X, WIN1_4Y, WIN1_4WIDTH - 1, WIN1_4HEIGHT - 1);
	QRect rcTemp[2][16];
	QRect rcTitle[2]; //��������Ϊ����
	QPainter p;
	QPixmap pix(rc.size());
	int row = 6;
	int titelRightWidth = 80;	//��ʾҳ��Ŀ��

	if (!pix.isNull())
	{
		p.begin(&pix);

		//������ƿռ�
		QRect rcTemp1 = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width(), pix.rect().height()); //����������������Ͳ�����
		QRect rcPara = QRect(pix.rect().x(), pix.rect().y() + pix.rect().height() / 4, pix.rect().width(), pix.rect().height() / row * 4); //������
		rcTitle[0] = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width() - titelRightWidth, pix.rect().height() / row); //������0
		rcTitle[1] = QRect(pix.rect().width()- titelRightWidth, pix.rect().y(), titelRightWidth, pix.rect().height() / row); //������

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

		//���Ʊ�����
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

		////����Ƶ�ʲο�״̬
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

		////����Ƶ�ʲο�ֵ
		//if (sysData.freq.ref.state)
		//	p.setPen(Qt::black); //����״̬�жϻ����Ƿ��ɫ����
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
		//	displayString = "Ƶ�ʲο�:\t" + QtempV + "\t" + QtempU;
		//}
		//p.drawText(rcTemp[1][1], Qt::AlignLeft | Qt::AlignVCenter, displayString);

		//p.setPen(Qt::black);

		//����Ƶ��ƫ��ֵ
		QtempV = doubleToQValue(sysData.freq.offset);
		QtempV = insertThousandsQS(QtempV);
		QtempU = freqToQUnit(sysData.freq.offset);
		if (sysData.system.isLanguageEn)
		{
			displayString = "Freq Offset:\t" + QtempV + "\t" + QtempU;
		} else
		{
			displayString = "Ƶ��ƫ��:\t" + QtempV + "\t" + QtempU;
		}
		p.drawText(rcTemp[0][0], Qt::AlignLeft | Qt::AlignVCenter, displayString);

		////����Ƶ�ʱ���ֵ
		//QtempV = doubleToQValue(sysData.freq.multiplier);
		//QtempV = insertThousandsQS(QtempV);
		//QtempU = "x";
		//if (sysData.system.isLanguageEn)
		//{
		//	displayString = "Multiplier:\t" + QtempV + "\t" + QtempU;
		//} else
		//{
		//	displayString = "��Ƶ:\t" + QtempV + "\t" + QtempU;
		//}
		//p.drawText(rcTemp[1][0], Qt::AlignLeft | Qt::AlignVCenter, displayString);

		//��������ο�
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
		//Ƶ������
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
//���Ʒ��Ȳ����ڲ�����ʾ��
void mainForm::drawAmplInParamter(void)
{
	if (sysScpi->isUpdating)
	{
		return;
	}

	//if (!sysData.display.isFullScreen)
	QRect rc = QRect(WIN1_4X, WIN1_4Y, WIN1_4WIDTH - 1, WIN1_4HEIGHT - 1);
	QRect rcTemp[2][16];
	QRect rcTitle[2]; //��������Ϊ����
	QPainter p;
	QPixmap pix(rc.size());
	int row = 6;
	int titelRightWidth = 80;	//��ʾҳ��Ŀ��

	if (!pix.isNull())
	{
		p.begin(&pix);

		//������ƿռ�
		QRect rcTemp1 = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width(), pix.rect().height()); //����������������Ͳ�����
		QRect rcPara = QRect(pix.rect().x(), pix.rect().y() + pix.rect().height() / 4, pix.rect().width(), pix.rect().height() / row * 4); //������
		rcTitle[0] = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width() - titelRightWidth, pix.rect().height() / row); //������0
		rcTitle[1] = QRect(pix.rect().width() - titelRightWidth, pix.rect().y(), titelRightWidth, pix.rect().height() / row); //������

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

		//���Ʊ�����
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
		//�����ȷ�״̬
		if (sysData.ampl.alc.state)
		{
			displayString = "Ampl ALC State:\tOn";
		} else
		{
			displayString = "Ampl ALC State:\tOff";
		}
		p.drawText(rcTemp[0][0], Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayString));

		//�����ȷ���Ӧ
		if (sysData.ampl.alc.state)
			p.setPen(Qt::black); //����״̬�жϻ����Ƿ��ɫ����
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
		////���Ʒ��Ȳο�״̬
		//p.setPen(Qt::black);
		//if (sysData.ampl.ref.state)
		//{
		//	displayString = "Ampl Ref State:\tOn";
		//} else
		//{
		//	displayString = "Ampl Ref State:\tOff";
		//}

		//p.drawText(rcTemp[1][1], Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayString));

		////���Ʒ��Ȳο�ֵ
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
		//	displayString = "���Ȳο�ֵ��" + QtempV + "\t" + QtempU;
		//}

		//p.drawText(rcTemp[0][1], Qt::AlignLeft | Qt::AlignVCenter, displayString);
		p.setPen(Qt::black);

		//���Ʒ���ƫ��ֵ
		QtempV.sprintf("%.2f", sysData.ampl.offset);
		QtempU = "dB";
		if (sysData.system.isLanguageEn)
		{
			displayString = "Ampl Offset:\t" + QtempV + "\t" + QtempU;
		} else
		{
			displayString = "����ƫ��ֵ��" + QtempV + "\t" + QtempU;
		}
		p.drawText(rcTemp[0][1], Qt::AlignLeft | Qt::AlignVCenter, displayString);

		//�������𲹳�
		QtempV.sprintf("%.2f", sysData.ampl.error.value);
		QtempU = "dB";
		if (sysData.system.isLanguageEn)
		{
			displayString = "I.L Cable:\t" + QtempV + "\t" + QtempU;
		} else
		{
			displayString = "���𲹳�:\t" + QtempV + "\t" + QtempU;
		}
		p.drawText(rcTemp[1][1], Qt::AlignLeft | Qt::AlignVCenter, displayString);

		//���Ʒ�������
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

//���� process
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

//����ɨ�������
void mainForm::drawSweep(int type)
{
	if (sysScpi->isUpdating)
	{
		return;
	}

	QString tempQChar;

	QRect rc = QRect(WIN1_4X, WIN1_4Y, WIN1_4WIDTH - 1, WIN1_4HEIGHT - 1);
	QRect rcTemp[2][16];
	QRect rcTitle[2]; //��������Ϊ����
	QPainter p;
	QPixmap pix(rc.size());
	int row = 6;
	int titelRightWidth = 80;

	if (!pix.isNull())
	{
		p.begin(&pix);

		//������ƿռ�
		QRect rcTemp1 = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width(), pix.rect().height()); //����������������Ͳ�����
		QRect rcPara = QRect(pix.rect().x(), pix.rect().y() + pix.rect().height() / 4, pix.rect().width(), pix.rect().height() / row * 4); //������
		rcTitle[0] = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width() - titelRightWidth, pix.rect().height() / row); //������0
		rcTitle[1] = QRect(pix.rect().width() - titelRightWidth, pix.rect().y(), titelRightWidth, pix.rect().height() / row); //������

		p.fillRect(rcTemp1, Qt::white);
		p.fillRect(rcTitle[0], BACKCOLORWIN4);
		p.fillRect(rcTitle[1], BACKCOLORWIN4);
		p.drawRect(pix.rect());
		int interval = 10;
		
		p.setPen(Qt::black);
		QString displayString, QtempV, QtempU, tempValue;
		//���Ʊ�����
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
		tempValue = doubleToQValue(sysData.sweep.nowpagenum);  //������ת����QT�ַ���
		displayString ="Page: " + tempValue + "/2";
		p.drawRect(rcTitle[1]);
		p.drawText(rcTitle[1], Qt::AlignCenter, displayString);

		if (sysData.sweep.type == stStep)	//ɨ�������ǲ���
		{
			//����ɨ��ʱ����һҳ��ʾ��ʼ����ֹƵ�ʺͷ��ȣ��ڶ�ҳ��ʾ������Ϣ
			if (sysData.sweep.nowpagenum == 1)
			{
				//���㷽��
				for (int i = 0; i < 5; i++)
				{
					rcTemp[0][i] = QRect(rcPara.x() + interval, rcPara.y() + i * rcPara.height() / 5, rcPara.width() - interval, rcPara.height() / 5);
				}
				//����ɨ����ʼƵ��
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

				//����ɨ����ֹƵ��
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

				//����ɨ����ʼ����
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

				//����ɨ����ֹ����
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
			else if (sysData.sweep.nowpagenum == 2)	//�ڶ�ҳ��ʾ������Ϣ
			{
				for (int i = 0; i < 5; i++)
				{
					for (int j = 0; j < 2; j++)
					{
						rcTemp[j][i] = QRect(rcPara.x() + rcPara.width() / 2 * j + interval, rcPara.y() + i * rcPara.height() / 5, rcPara.width() / 2 - interval, rcPara.height() / 5);
					}
				}
				//����ɨ��ģʽ
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

				//����ɨ������
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

				//����ɨ��פ��ʱ��
				p.setPen(Qt::black);
				QtempV = doubleToQValue(sysData.sweep.step.dwell);
				QtempV = insertThousandsQS(QtempV);
				QtempU = timeToQUnit(sysData.sweep.step.dwell);
				tempValue = "Dwell";
				displayString = getTheTranslatorString(tempValue) + ":" + "\t" + QtempV + "\t" + QtempU;
				p.drawText(rcTemp[0][2], Qt::AlignLeft | Qt::AlignVCenter, displayString);

				//����ɨ��ɨ�����
				p.setPen(Qt::black);
				QtempV = tempQChar.sprintf("%d", sysData.sweep.step.point);
				tempValue = "Points";
				displayString = getTheTranslatorString(tempValue) + ":" + "\t" + QtempV;
				p.drawText(rcTemp[0][3], Qt::AlignLeft | Qt::AlignVCenter, displayString);

				//����ɨ���۷�
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

				//����ɨ������
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

				//����ɨ�败��
				if (sysData.sweep.trig == stAuto)
				{
					displayString = "Trigger:\vAuto"; //�Զ�ɨ��
				}
				else if (sysData.sweep.trig == stExt)
				{
					displayString = "Trigger:\vstExt"; //�ⲿ����
				}
				else if (sysData.sweep.trig == stKey)
				{
					displayString = "Trigger:\vstKey";//��������
				}
				p.drawText(rcTemp[1][2], Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayString));

				//����ɨ�跽��
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
			//�б�ɨ��ʱ������ʾ��ʼ����ֹƵ�ʺͷ��Ⱥ�פ��ʱ��
			for (int i = 0; i < 5; i++)
			{
				for (int j = 0; j < 2; j++)
				{
					rcTemp[j][i] = QRect(rcPara.x() + rcPara.width() / 2 * j + interval, rcPara.y() + i * rcPara.height() / 5, rcPara.width() / 2 - interval, rcPara.height() / 5);
				}
			}
			//����ɨ��ģʽ
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

			//����ɨ������
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

			//����ɨ��ɨ�����
			p.setPen(Qt::black);
			QtempV = tempQChar.sprintf("%d", sysData.sweep.step.point);
			tempValue = "Points";
			displayString = getTheTranslatorString(tempValue) + ":" + "\t" + QtempV;
			p.drawText(rcTemp[0][3], Qt::AlignLeft | Qt::AlignVCenter, displayString);

			//����ɨ���۷�
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

			//����ɨ������
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

			//����ɨ�败��
			if (sysData.sweep.trig == stAuto)
			{
				displayString = "Trigger:\vAuto"; //�Զ�ɨ��
			}
			else if (sysData.sweep.trig == stExt)
			{
				displayString = "Trigger:\vstExt"; //�ⲿ����
			}
			p.drawText(rcTemp[1][2], Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayString));

			//����ɨ�跽��
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

//�����б�ɨ��������
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
	int rowDisplay = 6; //��
	int lineDisplay = 4; //��

	if (!pix.isNull())
	{
		p.begin(&pix);
		p.fillRect(pix.rect(), BACKCOLORWIN4);

		//������ƿռ�
		QRect rcPara = QRect(pix.rect().x(), pix.rect().y() + pix.rect().height() / rowDisplay, pix.rect().width(), pix.rect().height() - pix.rect().height() / rowDisplay);
		rcTitle[0] = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width() - 60, pix.rect().height() / 6); //������0
		rcTitle[1] = QRect((pix.rect().x() + pix.rect().width()- 60), pix.rect().y(), 60, pix.rect().height() / 6); //������

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

		//���Ʊ�����
		p.setPen(Qt::black);
		p.setFont(fontEnglishBold);
		displayQS = "List Mode Values";
		p.drawRect(rcTitle[0]);
		p.drawText(rcTitle[0].x() + 2, rcTitle[0].y(), rcTitle[0].width(), rcTitle[0].height(), Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayQS));

		//�����б�����
		displayQS = "Num";
		p.drawText(rcNum[0], Qt::AlignCenter, getTheTranslatorString(displayQS));
		displayQS = "Frequency";
		p.drawText(rcTemp[0][0], Qt::AlignCenter, getTheTranslatorString(displayQS));
		displayQS = "Ampl";
		p.drawText(rcTemp[0][1], Qt::AlignCenter, getTheTranslatorString(displayQS));
		displayQS = "Dwell";
		p.drawText(rcTemp[0][2], Qt::AlignCenter, getTheTranslatorString(displayQS));

		//����ҳ��
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
					if (j == 0) //���Ʊ��
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

		p.drawRect(rcTemp[SelectRow + 1][SelectLine]); //	��ʾѡ��
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

//���ƽ�����
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

//�������ؽ���
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

//����AM
void mainForm::drawAM(void)
{
	if (sysScpi->isUpdating)
	{
		return;
	}

	QRect rc = QRect(WIN1_4X, WIN1_4Y, WIN1_4WIDTH - 1, WIN1_4HEIGHT - 1);
	QRect rcTemp[2][16];
	QRect rcTitle[2]; //��������Ϊ����
	QPainter p;
	QPixmap pix(rc.size());
	int row = 6;
	int titelRightWidth = 80;

	if (!pix.isNull())
	{
		p.begin(&pix);

		//������ƿռ�
		QRect rcTemp1 = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width(), pix.rect().height()); //����������������Ͳ�����
		QRect rcPara = QRect(pix.rect().x(), pix.rect().y() + pix.rect().height() / 4, pix.rect().width(), pix.rect().height() / row * 4); //������
		rcTitle[0] = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width() - titelRightWidth, pix.rect().height() / row); //������0
		rcTitle[1] = QRect(pix.rect().width() - titelRightWidth, pix.rect().y(), titelRightWidth, pix.rect().height() / row); //������

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
		//����AM����״̬
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
			p.setPen(Qt::black); //����״̬�жϻ����Ƿ��ɫ����
		else
			p.setPen(Qt::gray);
		//���Ƶ������
		QtempV.sprintf("%.2f", sysData.am.depth);
		QtempU = "%";
		displayString = getTheTranslatorString("Moudulate Depth") + ":\t" + (QtempV + QtempU);
		p.drawText(rcTemp[1][0], Qt::AlignLeft | Qt::AlignVCenter, displayString);
		//���Ƶ���Դ
		if (sysData.am.source)
		{
			displayString = getTheTranslatorString("Modulation Source") + QString(":\t") + getTheTranslatorString("EXT");
		}
		else
		{
			displayString = getTheTranslatorString("Modulation Source") + QString(":\t") + getTheTranslatorString("INT");
		}
		p.drawText(rcTemp[0][1], Qt::AlignLeft | Qt::AlignVCenter, displayString);
		//�����ڲ��������κ�Ƶ��
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
			//����Ƶ��
			QtempV = doubleToQValue(sysData.am.rate);
			QtempV = insertThousandsQS(QtempV);
			QtempU = freqToQUnit(sysData.am.rate);
			displayString = getTheTranslatorString("Freq") + QString(":\t") + QtempV + QtempU;
			p.drawText(rcTemp[0][2], Qt::AlignLeft | Qt::AlignVCenter, displayString);
		}
		else
		{
			//���������ϲ���
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

//����FMPM
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
//����FM
void mainForm::drawFM(void)
{
	if (sysScpi->isUpdating)
	{
		return;
	}

	QRect rc = QRect(WIN1_4X, WIN1_4Y, WIN1_4WIDTH - 1, WIN1_4HEIGHT - 1);
	QRect rcTemp[2][16];
	QRect rcTitle[2]; //��������Ϊ����
	QPainter p;
	QPixmap pix(rc.size());
	int row = 6;
	int titelRightWidth = 80;

	if (!pix.isNull())
	{
		p.begin(&pix);

		//������ƿռ�
		QRect rcTemp1 = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width(), pix.rect().height()); //����������������Ͳ�����
		QRect rcPara = QRect(pix.rect().x(), pix.rect().y() + pix.rect().height() / 4, pix.rect().width(), pix.rect().height() / row * 4); //������
		rcTitle[0] = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width() - titelRightWidth, pix.rect().height() / row); //������0
		rcTitle[1] = QRect(pix.rect().width() - titelRightWidth, pix.rect().y(), titelRightWidth, pix.rect().height() / row); //������

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
		//���Ƶ�������
		displayString = "Modulate Mode:\tFM";
		p.drawText(rcTemp[0][0], Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayString));
		//����AM����״̬
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
			p.setPen(Qt::black); //����״̬�жϻ����Ƿ��ɫ����
		else
			p.setPen(Qt::gray);
		//���Ƶ���ƫ��
		QtempV = doubleToQValue(sysData.fmpm.fm.deviation);
		QtempV = insertThousandsQS(QtempV);
		QtempU = freqToQUnit(sysData.fmpm.fm.deviation);
		displayString = getTheTranslatorString("Freq Offset") + ":\t" + (QtempV + QtempU);
		p.drawText(rcTemp[1][0], Qt::AlignLeft | Qt::AlignVCenter, displayString);
		//���Ƶ���Դ
		if (sysData.fmpm.fm.source)
		{
			displayString = getTheTranslatorString("Modulation Source") + QString(":\t") + getTheTranslatorString("EXT");
		}
		else
		{
			displayString = getTheTranslatorString("Modulation Source") + QString(":\t") + getTheTranslatorString("INT");
		}
		p.drawText(rcTemp[1][1], Qt::AlignLeft | Qt::AlignVCenter, displayString);
		//�����ڲ��������κ�Ƶ��
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
			//����Ƶ��
			QtempV = doubleToQValue(sysData.fmpm.fm.rate);
			QtempV = insertThousandsQS(QtempV);
			QtempU = freqToQUnit(sysData.fmpm.fm.rate);
			displayString = getTheTranslatorString("Freq") + QString(":\t") + QtempV + QtempU;
			p.drawText(rcTemp[1][2], Qt::AlignLeft | Qt::AlignVCenter, displayString);
		}
		else
		{
			//���������ϲ���
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
//����PM
void mainForm::drawPM(void)
{
	if (sysScpi->isUpdating)
	{
		return;
	}

	QRect rc = QRect(WIN1_4X, WIN1_4Y, WIN1_4WIDTH - 1, WIN1_4HEIGHT - 1);
	QRect rcTemp[2][16];
	QRect rcTitle[2]; //��������Ϊ����
	QPainter p;
	QPixmap pix(rc.size());
	int row = 6;
	int titelRightWidth = 80;

	if (!pix.isNull())
	{
		p.begin(&pix);

		//������ƿռ�
		QRect rcTemp1 = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width(), pix.rect().height()); //����������������Ͳ�����
		QRect rcPara = QRect(pix.rect().x(), pix.rect().y() + pix.rect().height() / 4, pix.rect().width(), pix.rect().height() / row * 4); //������
		rcTitle[0] = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width() - titelRightWidth, pix.rect().height() / row); //������0
		rcTitle[1] = QRect(pix.rect().width() - titelRightWidth, pix.rect().y(), titelRightWidth, pix.rect().height() / row); //������

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
		displayString = "��M Status Information";
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
		//���Ƶ�������
		displayString = "Modulate Mode:\t��M";
		p.drawText(rcTemp[0][0], Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayString));
		//����AM����״̬
		if (sysData.fmpm.pm.state)
		{
			displayString = "��M State:\tOn";
		}
		else
		{
			displayString = "��M State:\tOff";
		}
		p.drawText(rcTemp[0][1], Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayString));

		if (sysData.fmpm.pm.state)
			p.setPen(Qt::black); //����״̬�жϻ����Ƿ��ɫ����
		else
			p.setPen(Qt::gray);
		//���Ƶ���ƫ��
		QtempV.sprintf("%.2f", sysData.fmpm.pm.phase);
		QtempU = "��";
		displayString = getTheTranslatorString("Phase Offset") + ":\t" + (QtempV + QtempU);
		p.drawText(rcTemp[1][0], Qt::AlignLeft | Qt::AlignVCenter, displayString);
		//���Ƶ���Դ
		if (sysData.fmpm.pm.source)
		{
			displayString = getTheTranslatorString("Modulation Source") + QString(":\t") + getTheTranslatorString("EXT");
		}
		else
		{
			displayString = getTheTranslatorString("Modulation Source") + QString(":\t") + getTheTranslatorString("INT");
		}
		p.drawText(rcTemp[1][1], Qt::AlignLeft | Qt::AlignVCenter, displayString);
		//�����ڲ��������κ�Ƶ��
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
			//����Ƶ��
			QtempV = doubleToQValue(sysData.fmpm.pm.rate);
			QtempV = insertThousandsQS(QtempV);
			QtempU = freqToQUnit(sysData.fmpm.pm.rate);
			displayString = getTheTranslatorString("Freq") + QString(":\t") + QtempV + QtempU;
			p.drawText(rcTemp[1][2], Qt::AlignLeft | Qt::AlignVCenter, displayString);
		}
		else
		{
			//���������ϲ���
			if (sysData.fmpm.pm.coupling)
			{
				displayString = getTheTranslatorString("��M Dev Couple") + ":\t" + getTheTranslatorString("On");
			}
			else
			{
				displayString = getTheTranslatorString("��M Dev Couple") + ":\t" + getTheTranslatorString("Off");
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
	QRect rcTitle[2]; //��������Ϊ����
	QPainter p;
	QPixmap pix(rc.size());
	int row = 6;
	int titelRightWidth = 80;

	if (!pix.isNull())
	{
		p.begin(&pix);

		//������ƿռ�
		QRect rcTemp1 = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width(), pix.rect().height()); //����������������Ͳ�����
		QRect rcPara = QRect(pix.rect().x(), pix.rect().y() + pix.rect().height() / 4, pix.rect().width(), pix.rect().height() / row * 4); //������
		rcTitle[0] = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width() - titelRightWidth, pix.rect().height() / row); //������0
		rcTitle[1] = QRect(pix.rect().width() - titelRightWidth, pix.rect().y(), titelRightWidth, pix.rect().height() / row); //������

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
		//���Ƶ�������
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


//�������ֵ��Ʋ�����
void mainForm::drawDigitalModulate(void)
{
	if (sysScpi->isUpdating)
	{
		return;
	}
	QRect rc = QRect(WIN1_4X, WIN1_4Y, WIN1_4WIDTH - 1, WIN1_4HEIGHT - 1);
	QRect rcTemp[2][16];
	QRect rcTitle[2]; //��������Ϊ����
	QPainter p;
	QPixmap pix(rc.size());
	int row = 6;
	int titelRightWidth = 80;

	if (!pix.isNull())
	{
		p.begin(&pix);
		//������ƿռ�
		QRect rcTemp1 = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width(), pix.rect().height()); //����������������Ͳ�����
		QRect rcPara = QRect(pix.rect().x(), pix.rect().y() + pix.rect().height() / 4, pix.rect().width(), pix.rect().height() / row * 4); //������
		rcTitle[0] = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width() - titelRightWidth, pix.rect().height() / row); //������0
		rcTitle[1] = QRect(pix.rect().width() - titelRightWidth, pix.rect().y(), titelRightWidth, pix.rect().height() / row); //������

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

		//���Ƶ���״̬
		if (sysData.system.isLanguageEn)
		{
			p.setFont(fontEnglishBold);
		} else
		{
			p.setFont(fontChinesePara);
		}

		if (sysData.digital.digitalstate) //���ֵ���״̬�Ǵ�
		{
			displayString = "Modelation Status:\tOn";
		} else
		{
			displayString = "Modelation Status:\tOff";
		}
		p.drawText(rcTemp[0][0], Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayString));

		//���Ƶ�������
		displayString = "Modelation Type";
		if (sysData.digital.digitalstate)
			p.setPen(Qt::black); //����״̬���û��ʵ���ɫ�Ǻ�ɫ���ǻ�ɫ
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

		//���Ƶ��Ʋ���--���/Ƶƫ/��ƫ
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
				QtempV = insertThousandsQS(QtempV); //QSting���Ͳ���ǧ�ַ�
				QtempU = freqToQUnit(sysData.digital.digitalpara.deviation);
				break;
			case dt2PSK:
			case dt4PSK:
			case dt8PSK:
				displayString = "Phase";
				QtempV.sprintf("%.2f", sysData.digital.digitalpara.phase);
				QtempU = "��";
				break;
			default:
				break;
		}
		displayString = getTheTranslatorString(displayString) + ":\t" + QtempV + QtempU;
		p.drawText(rcTemp[1][0], Qt::AlignLeft | Qt::AlignVCenter, displayString);

		//���Ƶ��Ʋ���--����
		displayString = "Rate";
		QtempV = doubleToQValue(sysData.digital.digitalpara.rate);
		QtempV = insertThousandsQS(QtempV);
		QtempU = freqToQUnit(sysData.digital.digitalpara.rate);
		displayString = getTheTranslatorString(displayString) + ":\t" + QtempV + QtempU;
		p.drawText(rcTemp[1][1], Qt::AlignLeft | Qt::AlignVCenter, displayString);

		//���Ƶ���Դ
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

//����������Ʋ�����
void mainForm::drawPulseModulate(void)
{
	QRect rc = QRect(WIN1_4X, WIN1_4Y, WIN1_4WIDTH - 1, WIN1_4HEIGHT - 1);
	QRect rcTemp[2][16];
	QRect rcTitle[2]; //��������Ϊ����
	QPainter p;
	QPixmap pix(rc.size());
	int row = 6;
	int titelRightWidth = 80;

	if (!pix.isNull())
	{
		p.begin(&pix);

		//������ƿռ�
		QRect rcTemp1 = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width(), pix.rect().height()); //����������������Ͳ�����
		QRect rcPara = QRect(pix.rect().x(), pix.rect().y() + pix.rect().height() / 4, pix.rect().width(), pix.rect().height() / row * 4); //������
		rcTitle[0] = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width() - titelRightWidth, pix.rect().height() / row); //������0
		rcTitle[1] = QRect(pix.rect().width() - titelRightWidth, pix.rect().y(), titelRightWidth, pix.rect().height() / row); //������

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
		//���Ʊ�����		
		displayString = "Pulse Modelation Information";
		p.drawRect(rcTitle[0]);
		p.drawText(rcTitle[0].x() + 2, rcTitle[0].y(), rcTitle[0].width(), rcTitle[0].height(), Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString(displayString));

		displayString = "Page: 1/1";
		p.drawRect(rcTitle[1]);
		p.drawText(rcTitle[1].x(), rcTitle[1].y(), rcTitle[1].width(), rcTitle[1].height(), Qt::AlignCenter, displayString);

		//�����������״̬
		if (sysData.system.isLanguageEn)
		{
			p.setFont(fontEnglishBold);
		} else
		{
			p.setFont(fontChinesePara);
		}
		if (sysData.pulse.state) //���ֵ���״̬�Ǵ�
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
		//�������崥����ʽ
		switch (sysData.pulse.trigmethod)
		{
			case plmFreerun:	//��������
				displayString = getTheTranslatorString(displayString) + ":\t" + getTheTranslatorString("Free Run");
				break;
			case plmKeytrig:	//��������
				displayString = getTheTranslatorString(displayString) + ":\t" + getTheTranslatorString("Key Trig");
				break;
			case plmExttrig:	//�ⲿ����
				displayString = getTheTranslatorString(displayString) + ":\t" + getTheTranslatorString("Ext Trig");
				break;
			default:
				break;
		}
		p.drawText(rcTemp[1][0], Qt::AlignLeft | Qt::AlignVCenter, displayString);

		//��������Դ
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

		//������������
		displayString = "Pulse Period";
		QtempV = doubleToQValue(sysData.pulse.period);
		QtempV = insertThousandsQS(QtempV);
		QtempU = timeToQUnit(sysData.pulse.period * 1e-6);	//timeToQUnit������msΪ��λ��������Ҫת��
		displayString = getTheTranslatorString(displayString) + ":\t" + QtempV + QtempU;
		p.drawText(rcTemp[1][1], Qt::AlignLeft | Qt::AlignVCenter, displayString);

		//����������
		displayString = "Pulse Width";
		QtempV = doubleToQValue(sysData.pulse.width);
		QtempV = insertThousandsQS(QtempV);
		QtempU = timeToQUnit(sysData.pulse.width * 1e-6);	//timeToQUnit������msΪ��λ��������Ҫת��
		displayString = getTheTranslatorString(displayString) + ":\t" + QtempV + QtempU;
		p.drawText(rcTemp[0][2], Qt::AlignLeft | Qt::AlignVCenter, displayString);

		//����������ʱ
		p.setPen(Qt::gray);
		displayString = "Pulse Delay";
		QtempV = doubleToQValue(sysData.pulse.delay);
		QtempV = insertThousandsQS(QtempV);
		QtempU = timeToQUnit(sysData.pulse.delay * 1e-6);	//timeToQUnit������msΪ��λ��������Ҫת��
		displayString = getTheTranslatorString(displayString) + ":\t" + QtempV + QtempU;
		p.drawText(rcTemp[1][2], Qt::AlignLeft | Qt::AlignVCenter, displayString);

		//�������弫��
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

//�����ϱ�Ƶ����
void mainForm::drawExfFref(void)
{
	if (sysScpi->isUpdating)
	{
		return;
	}
	QRect rc = QRect(WIN1_4X, WIN1_4Y, WIN1_4WIDTH - 1, WIN1_4HEIGHT - 1);
	QRect rcTemp[2][16];
	QRect rcTitle[2]; //��������Ϊ����
	QPainter p;
	QPixmap pix(rc.size());
	int row = 6;
	int titelRightWidth = 80;
	if (!pix.isNull())
	{
		p.begin(&pix);

		//������ƿռ�
		QRect rcTemp1 = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width(), pix.rect().height()); //����������������Ͳ�����
		QRect rcPara = QRect(pix.rect().x(), pix.rect().y() + pix.rect().height() / 4, pix.rect().width(), pix.rect().height() / row * 4); //������
		rcTitle[0] = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width() - titelRightWidth, pix.rect().height() / row); //������0
		rcTitle[1] = QRect(pix.rect().width() - titelRightWidth, pix.rect().y(), titelRightWidth, pix.rect().height() / row); //������

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

		//����Ƶ�ʲο�����
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


		//����Ƶ�ʲο�
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

		//���ƴӵײ㴫�����ķ���
		if (sysData.exfRef.type == erftExternal) //����ƵԴ���ⲿ��ʱ��Ž��н����������Ĳ���
		{
			p.setFont(QFont("enb", 16, 50, false));
			displayString = getTheTranslatorString("Ext IF Ampl") + ":\t";
			QString tempQchar = sysScpi->getQvalueFromModule();
			displayString = displayString + tempQchar.trimmed() + " dBm";
			rcTemp[0][2].setWidth(350);//�޸Ŀ�ȣ���ӵ�λ��ʾ
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


//�������Ⲩ������
void mainForm::drawArbitraryWave(void)
{
	if (sysScpi->isUpdating)
	{
		return;
	}

	QRect rc = QRect(WIN1_4X, WIN1_4Y, WIN1_4WIDTH - 1, WIN1_4HEIGHT - 1);
	QRect rcTemp[2][16];
	QRect rcTitle[2]; //��������Ϊ����
	QPainter p;
	QPixmap pix(rc.size());
	int row = 6;
	int titelRightWidth = 80;
	if (!pix.isNull())
	{
		p.begin(&pix);

		//������ƿռ�
		QRect rcTemp1 = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width(), pix.rect().height()); //����������������Ͳ�����
		QRect rcPara = QRect(pix.rect().x(), pix.rect().y() + pix.rect().height() / 4, pix.rect().width(), pix.rect().height() / row * 4); //������
		rcTitle[0] = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width() - titelRightWidth, pix.rect().height() / row); //������0
		rcTitle[1] = QRect(pix.rect().width() - titelRightWidth, pix.rect().y(), titelRightWidth, pix.rect().height() / row); //������

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

		//���������������
		if (sysData.system.isLanguageEn)
		{
			p.setFont(fontEnglishBold);
		} else
		{
			p.setFont(fontChinesePara);
		}

		//����LF״̬
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

		//�������Ƶ��
		displayString = getTheTranslatorString("Frequency");
		char tempChar[64];
		char temp[64];
		strcpy(tempChar, getFreqStringInLF(sysData.lf.freq, temp));
		QtempV = QString(tempChar);
		QtempV = insertThousandsQS(QtempV).trimmed().toUpper();
		QtempU = freqToQUnit(sysData.lf.freq);
		displayString = displayString + ":\t" + QtempV + QtempU;
		p.drawText(rcTemp[0][1], Qt::AlignLeft | Qt::AlignVCenter, displayString);

		//���������ѹ
		displayString = getTheTranslatorString("Voltage");
		QtempV = doubleToQValue(sysData.lf.ampl);
		QtempV = insertThousandsQS(QtempV);
		QtempU = vppToQUnit(sysData.lf.ampl);
		displayString = displayString + ":\t" + QtempV + QtempU;
		p.drawText(rcTemp[1][1], Qt::AlignLeft | Qt::AlignVCenter, displayString);

		p.setPen(Qt::black);

#if 0
		//��������
		displayQS = "Wave"; p.drawText(rcTemp[1][0], Qt::AlignCenter, displayQS);
		displayQS = "Freq"; p.drawText(rcTemp[1][1], Qt::AlignCenter, displayQS);
		displayQS = "Voltage"; p.drawText(rcTemp[1][2], Qt::AlignCenter, displayQS);

		//����IF����
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

//����DRG
void mainForm::drawDrgFunction(void)
{
	if (sysScpi->isUpdating)
	{
		return;
	}
	QRect rc = QRect(WIN1_4X, WIN1_4Y, WIN1_4WIDTH - 1, WIN1_4HEIGHT - 1);
	QRect rcTemp[2][16];
	QRect rcTitle[2]; //��������Ϊ����
	QPainter p;
	QPixmap pix(rc.size());
	char tempChar[64];
	char temp[64];
	int row = 6;
	int titelRightWidth = 80;
	if (!pix.isNull())
	{
		p.begin(&pix);

		//������ƿռ�
		QRect rcTemp1 = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width(), pix.rect().height()); //����������������Ͳ�����
		QRect rcPara = QRect(pix.rect().x(), pix.rect().y() + pix.rect().height() / 4, pix.rect().width(), pix.rect().height() / row * 4); //������
		rcTitle[0] = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width() - titelRightWidth, pix.rect().height() / row); //������0
		rcTitle[1] = QRect(pix.rect().width() - titelRightWidth, pix.rect().y(), titelRightWidth, pix.rect().height() / row); //������

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
		//���Ʊ�����
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
		//����״̬
		displayQS = getTheTranslatorString("State");
		if (sysData.drg.state)
		{
			displayQS = displayQS + ":\t" + getTheTranslatorString("ON");
		} else
		{
			displayQS = displayQS + ":\t" + getTheTranslatorString("OFF");
		}
		p.drawText(rcTemp[0][0], Qt::AlignLeft | Qt::AlignVCenter, displayQS);
		
		//����ɨ������
		displayQS = getTheTranslatorString("Up Limit");
		strcpy(tempChar, getFreqString(sysData.drg.uplimit, temp));
		QtempV = QString(tempChar);
		QtempV = insertThousandsQS(QtempV).trimmed().toUpper();
		QtempU = freqToQUnit(sysData.drg.uplimit).trimmed();
		displayQS = displayQS + ":\t" + QtempV + QtempU;
		p.drawText(rcTemp[1][0], Qt::AlignLeft | Qt::AlignVCenter, displayQS);

		//����ɨ������
		displayQS = getTheTranslatorString("Down Limit");
		strcpy(tempChar, getFreqString(sysData.drg.downlimit, temp));
		QtempV = QString(tempChar);
		QtempV = insertThousandsQS(QtempV).trimmed().toUpper();
		QtempU = freqToQUnit(sysData.drg.downlimit).trimmed();
		displayQS = displayQS + ":\t" + QtempV + QtempU;
		p.drawText(rcTemp[1][1], Qt::AlignLeft | Qt::AlignVCenter, displayQS);

		//����ɨ�����
		displayQS = getTheTranslatorString("Points");
		strcpy(tempChar, intToQString(sysData.drg.points, temp));
		QtempV = QString(tempChar);
		QtempV = insertThousandsQS(QtempV);
		displayQS = displayQS + ":\t" + QtempV;
		p.drawText(rcTemp[0][1], Qt::AlignLeft | Qt::AlignVCenter, displayQS);

		//����ɨ������
		displayQS = getTheTranslatorString("Rate");
		strcpy(tempChar, doubleToQValue(sysData.drg.rate).toStdString().c_str());
		QtempV = QString(tempChar);
		QtempV = insertThousandsQS(QtempV);
		QtempU = timeToQUnit(sysData.drg.rate  * 1e-6);
		displayQS = displayQS + ":\t" + QtempV + QtempU;
		p.drawText(rcTemp[0][2], Qt::AlignLeft | Qt::AlignVCenter, displayQS);

		//���ƹ���ģʽ
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

//����USB���ʼ�
void mainForm::drawUSBPower(void)
{
	if (sysScpi->isUpdating)
	{
		return;
	}
	QRect rc = QRect(WIN1_4X, WIN1_4Y, WIN1_4WIDTH - 1, WIN1_4HEIGHT - 1);
	QRect rcTemp[2][16];
	QRect rcTitle[2]; //��������Ϊ����
	QPainter p;
	QPixmap pix(rc.size());
	char tempChar[64];
	char temp[64];
	int row = 6;
	int titelRightWidth = 80;
	if (!pix.isNull())
	{
		p.begin(&pix);

		//������ƿռ�
		QRect rcTemp1 = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width(), pix.rect().height()); //����������������Ͳ�����
		QRect rcPara = QRect(pix.rect().x(), pix.rect().y() + pix.rect().height() / 4, pix.rect().width(), pix.rect().height() / row * 4); //������
		rcTitle[0] = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width() - titelRightWidth, pix.rect().height() / row); //������0
		rcTitle[1] = QRect(pix.rect().width() - titelRightWidth, pix.rect().y(), titelRightWidth, pix.rect().height() / row); //������

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
		//���Ʊ�����
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
		//����״̬
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

		//ͨ��Ƶ��
		displayQS = getTheTranslatorString("Frequency");
		strcpy(tempChar, getFreqString(sysData.usbPower.freq, temp));
		QtempV = QString(tempChar);
		QtempV = insertThousandsQS(QtempV).trimmed().toUpper();
		QtempU = freqToQUnit(sysData.usbPower.freq).trimmed();
		displayQS = displayQS + ":\t" + QtempV + QtempU;
		p.drawText(rcTemp[1][0], Qt::AlignLeft | Qt::AlignVCenter, displayQS);
		//���ƹ���ƫ��
		displayQS = getTheTranslatorString("Pow Offset");
		//printf("zpc add test, sysData.usbPower.powerOffset = %f\n", sysData.usbPower.powerOffset);
		QtempV = doubleToQValue(sysData.usbPower.powerOffset);
		QtempU = " dB";
		displayQS = displayQS + ":\t" + QtempV + QtempU;
		p.drawText(rcTemp[1][1], Qt::AlignLeft | Qt::AlignVCenter, displayQS);
		//����ƽ��ģʽ
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
		//����ƽ������
		displayQS = getTheTranslatorString("Average Count") + ":\t";
		displayQS += QString(intToQString(sysData.usbPower.AdvCount, temp)).trimmed();
		p.drawText(rcTemp[0][2], Qt::AlignLeft | Qt::AlignVCenter, displayQS);
		//���Ʋ�����λ
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

		//���ƴӹ��ʼ��ϴ������Ĺ���
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
						dispalyValue += sysData.usbPower.list[i];	//�����
					}
					dispalyValue /= sysData.usbPower.list.count();//����ƽ��
				}
			}
			if (sysData.usbPower.MeasUnit == upmutW)
			{
				dispalyValue = (pow(10, dispalyValue / 10)) / 1000;	//dBmת����W
				unit = " W";
			}
			char tempChar[64] = {};
			displayQS = getTheTranslatorString("Ampl") + ":\t" + QString(floatToStringDot2(dispalyValue, tempChar)).trimmed() + unit;
			p.setFont(fontEnglishBold);//����Ӵ�
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

//����ϵͳ��Ϣ������
void mainForm::drawSystemPara(void)
{
	if (sysScpi->isUpdating)
	{
		return;
	}

	QRect rc = QRect(WIN1_4X, WIN1_4Y, WIN1_4WIDTH - 1, WIN1_4HEIGHT - 1);
	QRect rcTemp[2][16];
	QRect rcTitle[2]; //��������Ϊ����
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

		//������ƿռ�
		QRect rcTemp1 = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width(), pix.rect().height()); //����������������Ͳ�����
		QRect rcPara = QRect(pix.rect().x(), pix.rect().y() + pix.rect().height() / 4, pix.rect().width(), pix.rect().height() / row * 4); //������
		rcTitle[0] = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width() - titelRightWidth, pix.rect().height() / row); //������0
		rcTitle[1] = QRect(pix.rect().width() - titelRightWidth, pix.rect().y(), titelRightWidth, pix.rect().height() / row); //������

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

		//���Ʊ�����
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

		//��������
		displayQS = "IP:\t" + QString(sysData.system.io.lan.ip);
		p.drawText(rcTemp[0][0], Qt::AlignLeft | Qt::AlignVCenter, displayQS);
		displayQS = "Mask:\t" + QString(sysData.system.io.lan.mask);
		p.drawText(rcTemp[0][2], Qt::AlignLeft | Qt::AlignVCenter, displayQS);
		displayQS = "Gate:\t" + QString(sysData.system.io.lan.gate);
		p.drawText(rcTemp[0][1], Qt::AlignLeft | Qt::AlignVCenter, displayQS);

		//��������汾��
		displayQS = "Soft Version:\t" + QString(intToString(VER_MAJOR, tempChar)).trimmed() + "." + QString(intToString(VER_MINOR, tempChar)).trimmed() + "." + QString(intToString(VER_REVISION,
				tempChar)).trimmed() + "." + QString(intToString(VER_BUILD, tempChar)).trimmed() + "." + QString(intToString(VER_TAG, tempChar)).trimmed();
		p.drawText(rcTemp[1][1], Qt::AlignLeft | Qt::AlignVCenter, displayQS);

#if 1
		//����ģ������汾��
		//	displayQS = "Module Software Ver:\t" + sysScpi->moduSoftVer;
		//	p.drawText(rcTemp[0][3],  Qt::AlignLeft | Qt::AlignVCenter, displayQS);

		//�������к�
		displayQS = "SN:\t" + sysScpi->moduSerial;
		p.drawText(rcTemp[1][0], Qt::AlignLeft | Qt::AlignVCenter, displayQS);

		//����ģ������
		//	displayQS = "Device Type:\t" + sysScpi->moduDevType;
		//	p.drawText(rcTemp[0][2],  Qt::AlignLeft | Qt::AlignVCenter, displayQS);

		//����MAC��ַ
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

		//����ʣ����������
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

//���ư�ť����ʾ��(70ms)
void mainForm::drawButton(void)
{
	if (sysScpi->isUpdating)
	{
		return;
	}
	QRect rc = QRect(WIN2X, WIN2Y, WIN2WIDTH, WIN2HEIGHT);	
	
	QRect rcTemp = QRect(BUTTONINTERVAL, BUTTONINTERVAL + BUTTONTITLEHEIGHT, rc.width(), (rc.height() - 2 * BUTTONTITLEHEIGHT) / BUTTONCOUNT - 1);//�������ο�
	QRect rcButton;
	int btnIndex = 0;
	int btnNums = 0;

	QPainter p;

	QPixmap pix(rc.size()); //QT:����ӳ��
	p.begin(&pix);
	p.setFont(fontData);	//���û�������
	p.fillRect(pix.rect(), BACKCOLORWIN5); //���ư�����������ΪĬ�ϱ���ɫ
	p.setBrush(QBrush(QColor(100,149,237))); 

	//���Ʋ˵�����
	QPointF titlePoints[] = { 
		QPointF(0, 0),
		QPointF(rcTemp.width()-1, 0),
		QPointF(rcTemp.width() - (BUTTONTITLEWIDTH / 5), BUTTONTITLEHEIGHT) ,
		QPointF(0, BUTTONTITLEHEIGHT)
	};
	QRect rcTitleName = QRect(0, 0, BUTTONTITLEWIDTH / 6 * 5, BUTTONTITLEHEIGHT);
	p.drawPolygon(titlePoints, 4);	//���Ʋ˵������
	p.drawText(rcTitleName, Qt::AlignHCenter | Qt::AlignVCenter, QString(sysData.titleName));//���Ʋ˵�������

	//����More����Ϣ
	if (sysData.moreKeyCount.Count > 0)
	{
		QPointF morePoints[] = {
			QPointF(0, WIN2HEIGHT - BUTTONTITLEHEIGHT),
			QPointF(rcTemp.width() - (BUTTONTITLEWIDTH / 5), WIN2HEIGHT - BUTTONTITLEHEIGHT),
			QPointF(rcTemp.right() - 1, WIN2HEIGHT),
			QPointF(0, WIN2HEIGHT)
		};
		p.drawPolygon(morePoints, 4);	//����More��Ϣ
		QRect rcMore = QRect(rcTemp.x(), WIN2HEIGHT - BUTTONTITLEHEIGHT, BUTTONTITLEWIDTH / 6 * 5, BUTTONTITLEHEIGHT);
		p.drawText(rcMore, Qt::AlignHCenter | Qt::AlignVCenter, "More " + QString::number(sysData.moreKeyCount.index + 1) + " of  " + QString::number(sysData.moreKeyCount.Count + 1));//���Ʋ˵�������
		//����More��ָʾ��
	}

	//------------------------------------------������Ч����� button is invalid or error----------------------------------------
	if (((nowMainKey == NULL) && (nowSubKey == NULL)) || ((nowMainKey == NULL) && (nowSubKey != NULL))) //���������Ч�������ֻ���ư�����������������Ϣ
	{
		for (int i = 0; i < BUTTONCOUNT; i++)
		{
			if (i == 0)
			{
				//��һ���������Ǿ�����Ҫ���⴦��
				QPointF FirstButtonPoints[] = { QPointF(rcTemp.x(), rcTemp.y()),
					QPointF(rcTemp.x() + rcTemp.width() / 6 * 5, rcTemp.y()),
					QPointF(rc.right(), rc.y() + BUTTONINTERVAL) ,
					QPointF(rcTemp.right(), rcTemp.bottom()),
					QPointF(rcTemp.left(), rcTemp.bottom())
				};
				p.setBrush(QBrush(BACKCOLORWIN5));
				p.drawPolygon(FirstButtonPoints, 5);	//���Ƶ�һ������
			}
			else if (i == BUTTONCOUNT - 1)
			{
				//���һ���������Ǿ�����Ҫ���⴦��
				QPointF LastButtonPoints[] = { QPointF(rcTemp.x(), rcTemp.y() + (rcTemp.height() * (BUTTONCOUNT - 1))),
					QPointF(rcTemp.right(), rcTemp.y() + (rcTemp.height() * (BUTTONCOUNT - 1))),
					QPointF(rcTemp.right(), rc.bottom() - BUTTONINTERVAL) ,
					QPointF(rcTemp.right() - rcTemp.width() / 6 * 5, rc.bottom() - BUTTONTITLEHEIGHT - BUTTONINTERVAL),
					QPointF(rcTemp.left(), rc.bottom() - BUTTONTITLEHEIGHT - BUTTONINTERVAL)
				};
				p.setBrush(QBrush(BACKCOLORWIN5));
				p.drawPolygon(LastButtonPoints, 5);	//�������һ������
			}
			else
			{
				rcButton = QRect(rcTemp.left(), rcTemp.y() + rcTemp.height() * i + BUTTONINTERVAL, rcTemp.width(), rcTemp.height());
				qDrawWinPanel(&p, rcButton, palette(), false, 0);
			}
		}
	}
	//-------------------------------------------------������Ч button valid-----------------------------------------------------
	else
	{
		subKeyDef* keyTemp = NULL;

		if (nowSubKey == NULL) //��λ�򿪻��� preset or poweron
		{
			keyTemp = nowMainKey->rightNode;
		} else //�������� normal run
		{
			keyTemp = nowSubKey;
			while (keyTemp->upNode != NULL) //��������ڵ� goto top node
			{
				keyTemp = keyTemp->upNode;
			}
		}
		
		//node loop
		bool isNeedDrawButtonRect = false;
		while (keyTemp != NULL)
		{			
			btnIndex = sysMenu->getButtonIndex(keyTemp->keyValue); //ͨ��btnIndexѡ��Ҫ���Ƶİ�������
			btnNums++;
			if (btnNums > BUTTONCOUNT + 1) //�������Խ�磬�򷵻�
			{
				break;
			}		
			if ((btnIndex >= 0) && (btnIndex < BUTTONCOUNT)) //�������δԽ�������
			{
				if (btnIndex == 0)
				{
					//��һ���������Ǿ�����Ҫ���⴦��
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
					p.drawPolygon(FirstButtonPoints, 5);	//���Ƶ�һ������
					isNeedDrawButtonRect = false;
					rcButton = QRect(rcTemp.x(), rcTemp.y() + rcTemp.height() * btnIndex + (btnIndex + 1)*BUTTONINTERVAL + 1, rcTemp.width(), rcTemp.height());
					QPixmap pix(rcButton.width() - 2, rcButton.height() - 2);
					drawButtonMenu(&pix, keyTemp, isNeedDrawButtonRect); //�������ϵ��ı���λ�õ���Ϣд�뻺��
					p.drawPixmap(rcButton.left() + 1, rcButton.top(), pix); //��ʾ	
				}
				else if (btnIndex == BUTTONCOUNT - 1)
				{
					//���һ���������Ǿ�����Ҫ���⴦��
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
					p.drawPolygon(LastButtonPoints, 5);	//�������һ������
					isNeedDrawButtonRect = false;
					rcButton = QRect(rcTemp.x(), rcTemp.y() + rcTemp.height() * btnIndex + (btnIndex + 1)*BUTTONINTERVAL, rcTemp.width(), rcTemp.height());
					QPixmap pix(rcButton.width() - 2, rcButton.height() - 3);
					drawButtonMenu(&pix, keyTemp, isNeedDrawButtonRect); //�������ϵ��ı���λ�õ���Ϣд�뻺��
					p.drawPixmap(rcButton.left() + 1, rcButton.top()+1, pix); //��ʾ	
				}
				else
				{
					isNeedDrawButtonRect = true;
					rcButton = QRect(rcTemp.x(), rcTemp.y() + rcTemp.height() * btnIndex + (btnIndex + 1)*BUTTONINTERVAL, rcTemp.width(), rcTemp.height());
					QPixmap pix(rcButton.width()-1, rcButton.height());
					drawButtonMenu(&pix, keyTemp, isNeedDrawButtonRect); //�������ϵ��ı���λ�õ���Ϣд�뻺��
					p.drawPixmap(rcButton.left(), rcButton.top(), pix); //��ʾ	
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

//����process
void mainForm::drawProcessSlots(void)
{
	drawProcess();
}

//����ȫ����ʾ��
void mainForm::drawAll(void)
{
	drawMainPara(); //������������ʾ��
	drawState(); //���Ʒָ�����ʾ��
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

//��������
void mainForm::loadSystemDriver(void)
{
	//���ؼ�������
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

	//����USB�ӿ�����
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

	//���ش�������
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

//�����Ϣ
void mainForm::outputMessage(QString msg)
{
	helpContent = msg;
	sysScpi->drawWin1_5(helpContent);
	usleep(3000 * 1000);
}

//�����Ϣ
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

//��ȡ������Ϣ
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

//��ȡ��������
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

//��ȡ�ӽڵ����һ���ڵ�
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

//USB���豸����
bool mainForm::isUsbHostConnect(void)
{
	QString dirName = "/sys/bus/usb/devices/1-1:1.0";

	return QDir(dirName).exists();
}

//USB���豸����
bool mainForm::isUsbDeviceConnect(void)
{
	QString dirName = "/sys/bus/usb/devices/usb1";
	return QDir(dirName).exists();
}

//��������
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

//��ʾ��������
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

//��ȡϵͳ����
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

//����ϵͳ����
void mainForm::saveSystemSetting(QString section, QString name, QVariant value)
{
	QString fileName = QCoreApplication::applicationDirPath() + "/setting.ini";
	QSettings setting(fileName, QSettings::IniFormat);
	setting.beginGroup(section);
	setting.setValue(name, value);
	setting.endGroup();
}

//��ȡ�ַ���
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

//��ȡ�ַ���
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

//��ȡLOGOͼƬ
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

//����������
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

	//��ȡ����
	keyNotifier->setEnabled(false);
	recvCount = read(keyHandle, recvbuf, 8);
	keyNotifier->setEnabled(true);
	//printf("zpc add test, %02x,%02x,%02x,%02x,%02x,%02x,%02x,%02x\r\n",recvbuf[0],recvbuf[1],recvbuf[2],recvbuf[3],recvbuf[4],recvbuf[5],recvbuf[6],recvbuf[7]);

	/* zpc add test */
	//�����ź�Դ�ļ���ת�����µļ���
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

	//��ӡ��ֵ
	if (__DEBUG)
	{
		printf("key pressed value is keyValue =0x%04x,0x%04x,0x%04x\n", keyValue, keyClass, keyCount);
	}

	if (sysScpi->isUpdating)
	{
		return;
	}

	//evaluate��������
	if (sysScpi->eva.isEvaluate && sysScpi->eva.evaluated) //����ʱ���ѹ�������Ҫ����ע����
	{
		int nums = 0;
		int tempValue = evaArray[evaIndex]; //8���ַ�������Ҫ�����8��16������
		int tempIndex = evaIndex;
		bool valued = false;
		bool indexed = false;
		char tempChar[32] = { 0x0 };
		QString inputString = "";
		QString md5String = "";

		if (keyValue == KEY_CTL_ARROW_LEFT) //���Ҽ������ƶ�ѡ�п�
		{
			tempIndex -= 1;
			indexed = true;
		} else if (keyValue == KEY_CTL_ARROW_RIGHT) //���Ҽ������ƶ�ѡ�п�
		{
			tempIndex += 1;
			indexed = true;
		} else if (sysMenu->pressedIsRpgKey(keyClass)) //������ť���������ַ����ַ���Χ 0x0 ~ 0xF
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
		} else if (keyValue == KEY_SUB_MENU1) //��SUB_MENU1������ȷ�ϼ�
		{
			for (int i = 0; i < sizeof evaArray / sizeof evaArray[0]; i++)
			{
				inputString += QString(intToHex(evaArray[i], tempChar)).trimmed(); //��������MD5ֵ
			}

			md5String = sysScpi->getSystemServiceOfMD5(sysScpi->moduSerial.trimmed(), false); //����ģ���ţ�����MD5ֵ
			if (inputString.toUpper() == md5String.toUpper()) //�Ƚ������MD5ֵ�ͼ���õ���MD5ֵ
			{
				sysScpi->unLockSystem(); //����ϵͳ
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

			evaIndex = tempIndex; //���뵽�ڼ����ַ�
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

			evaArray[evaIndex] = tempValue; //�����ֵ��ֵ��������
		}
		return;
	}

	//Զ�̿���
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

	//ִ�б���
	bool canExecute = true;
	bool mkrExecute = true;
	
	//��������������FREQ, AMPT, SWEEP, AM, FM/PM, IQ, PRESET, PULSE, AUXFCTN, TRIGGER, SYSTEM, LOFOUT, MODON/OFF, RF2ON/OFF, RF1ON/OFF
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
		//Ĭ��ѡ�е�һ��ʹ�ܵ��Ӽ�
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
	
	//�����Ӽ�
	if ((sysMenu->pressedIsCommonKey(keyValue)) && keyIsEnable(keyValue))
	{
        
        if (nowMainKey->keyValue == KEY_MAIN_FREQ || nowMainKey->keyValue == KEY_MAIN_AMPTD)
        {
            sysData.cursorLocation = -1;
        }
        else
        {
            sysData.cursorLocation = 0; //������λ��
        }
		if (sysMenu->getSubKeyPointer(keyValue))
		{
			handleMoreKey(nowSubKey->funcCode);	//����
			
			//isMore
			if ((nowSubKey->isMore) && (nowSubKey->rightNode != NULL))
			{
				if (nowSubKey->isPassword) //����Ҫ���루��ϵͳ���񡱰�����,��ִ��gotoDownLevel()����
				{
					previndexOfPassword = 0;
					indexOfPassword = 0;
					memset(passwdString, 0, sizeof passwdString);
					handleInputKey(nowSubKey->funcCode); //������Ҫ�������룬����InputKey�����룬��ʵ�ʲ˵���û��ѡ��isInput
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
				cleanInputParamter(); //ĳЩ�����ڰ��·��ؼ�����ղ�����������ʾ������
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
				handleExecuteKey(nowSubKey->funcCode);	//����ִ�м�
				if (isExecReturn(nowSubKey->funcCode))
				{
					switch (nowSubKey->funcCode)
					//ִ�м�ʱ���������ϼ�����
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
					if (nowSubKey->funcCode != KEY_FUNC_SWEEP_MORE_CONFIGLIST_MORE_DOWNLOAD) //zpc add �����ص��б�ɨ��װ��ʱ��ִ��handleInputKey()����
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
	//��������More��
	else if ((nowMainKey != NULL) && (nowSubKey != NULL) && sysMenu->pressedIsHardMoreKey(keyValue))
	{
		handleHardMoreKey();
		//���More���������������Ҫ����
		if (nowSubKey != NULL & nowSubKey->isInput)
		{
			handleInputKey(nowSubKey->funcCode);
		}
	}
	//��������Return��
	else if ((nowMainKey != NULL) && (nowSubKey != NULL) && sysMenu->pressedIsHardReturnKey(keyValue))
	{
		handleHardReturnKey();
		afterReturnDraw();
	}
	//����BankSpace��
	else if ((nowMainKey != NULL) && (nowSubKey != NULL) && sysMenu->pressedIsBkspaceKey(keyValue))
	{
		if (nowSubKey->isUnit || nowSubKey->isPassword)
		{
			if (!inputValue.isEmpty())
			{
				if (nowSubKey->isPassword) //���������
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
	//���·����
	else if ((nowMainKey != NULL) && (nowSubKey != NULL) && (nowSubKey->isEnabled) && (sysMenu->pressedIsArrowKey(keyValue)))
	{
		if (nowSubKey->isMac) //�����MAC��ַ
		{
			if (sysMenu->isUpArrow(keyValue))
			{

				sysData.system.lanMacAddrIndex--; //����MAC������������
				if (sysData.system.lanMacAddrIndex < 0)
					sysData.system.lanMacAddrIndex = 11;
			} else
			{
				sysData.system.lanMacAddrIndex++;
				if (sysData.system.lanMacAddrIndex > 11)
					sysData.system.lanMacAddrIndex = 0;
			}
		}
		handleArrowKey(nowSubKey->funcCode, keyValue);//�������
	}
	//����Select��
	else if ((nowMainKey != NULL) && (nowSubKey != NULL) && (nowSubKey->isEnabled) && (sysMenu->pressedIsSelectKey(keyValue)))
	{
		handleSelectKey(nowSubKey->funcCode);
	}
	//����IncrSet��
	else if (sysMenu->pressedIsIncrsetKey(keyValue))
	{
		handleIncrsetKey(nowSubKey->funcCode);
	}
	//����Trigger��
	else if (nowMainKey != NULL && sysMenu->pressedIsTriggerKey(keyValue))
	{
		handleTriggerKey(nowSubKey->funcCode);
	}
	//����PageUp��
	else if (nowMainKey != NULL && sysMenu->pressedIsPageUpKey(keyValue))
	{
		handlePageUpKey(nowMainKey->keyValue);
	}
	//����PageDown��
	else if (nowMainKey != NULL && sysMenu->pressedIsPageDownKey(keyValue))
	{
		handlePageDownKey(nowMainKey->keyValue);
	}
	//����RPG��
	else if ((nowMainKey != NULL) && (nowSubKey != NULL) && (nowSubKey->isEnabled) && (sysMenu->pressedIsRpgKey(keyClass)))
	{
		handleRpgKey(nowSubKey->funcCode, sysMenu->getRpgPulse(keyValue, keyCount));
	}
	//�������ּ�
	else if ((sysMenu->pressedIsNumberKey(keyValue)) && (nowSubKey != NULL) && (nowSubKey->isEnabled))
	{
        if (nowMainKey->funcCode == KEY_MAIN_FREQ || nowMainKey->funcCode == KEY_MAIN_AMPTD)
        {
            sysData.cursorLocation = -1;
        }
        else
        {
            sysData.cursorLocation = 0; //������λ��
        }
		if ((nowSubKey->isInput) && (nowSubKey->rightNode != NULL))
		{
			nowSubKey = nowSubKey->rightNode;
			drawButton(); //Input�����Ƶ�λ��
		}
		if (!isNumberKeyInput()) //�жϰ������ְ������Ƿ���Ч
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
	//����ȡ����
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
	//����δ֪��
	else
	{
		return;
	}
	if (HaveMoreKeyCount() > 0)	//���More��
	{
		//if (!sysData.led.More)		//ԭ����Ϩ��������
		{
			//printf("zpc add test, Control More LED On\n");
			sysData.led.More = true;
			//�·�ָ����
			sysScpi->keyBoardLEDSwitch(2, sysData.led.More);
		}
	}
	else
	{
		//if (sysData.led.More)	//ԭ���ǵ�������Ϩ��
		{
			//printf("zpc add test, Control More LED Off\n");
			sysData.led.More = false;
			//�·�ָ����
			sysScpi->keyBoardLEDSwitch(2, sysData.led.More);
		}
	}
	//���Ʋ˵�
	drawButton();
}

//��λUSB�ӿ�
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

//ִ���ļ�ѡ��
bool mainForm::handleFileSelect(bool up, int nums)
{
	return false;
}

//��ȡ��������
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

//ִ����������
void mainForm::execNumberInput(QString value, bool isNumber)
{
	QString tempchar;
	tempchar = inputValue;
	if (isPassword()) //��������
	{
		char tempPasswd[6];
		QString passwdStr = "";

		//�Ƿ�Ϊ���ּ���ÿ��һ�����ּ��������λ�ü�1
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
			if (indexOfPassword > previndexOfPassword) //��ʾ�ַ���λ�øı�
			{
				inputValue += value;

				if (previndexOfPassword == size - 1)
				{
					prevInput = inputValue;
				}
			} else //��ʾ�ַ���λ�ò��ı�
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
	else if (isMacAddress()) //MAC��ַ
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

//����
bool mainForm::isPassword(void)
{
	//  return false;
	return nowSubKey != NULL && nowSubKey->funcCode == KEY_FUNC_SYSTEM_SERVICE;
}

//MAC��ַ
bool mainForm::isMacAddress(void)
{
	return nowSubKey != NULL && nowSubKey->funcCode == KEY_FUNC_SYSTEM_SERVICE_LANMACADDR;
}

//������ȷ
bool mainForm::isPasswordValid(void)
{
	return false;
}

//��������Ч
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

//������Ч
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

//�ж����ְ�������ʱ�Ƿ�ִ����������,�磺����sweep�������ٰ������ּ�����Ļ�������ֲ���ʾ
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

//���Ⱥ�Ƶ�ʵ�������ú���Ҫ���������õ����صİ����ϣ���Ƶ�����ð�����������ð���
//Ƶ�ʲο����úͷ��Ȳο��������������ط������ʲ����ٴ˴�����
bool mainForm::isNeedReturnFreqOrAmplSet(void)
{
	bool ret;
	switch (nowSubKey->funcCode)
	{
		case KEY_FUNC_FREQREFSTATE:
		case KEY_FUNC_FREQOFFSET:
			//		case KEY_FUNC_FREQMULTIPLIER:
		case KEY_FUNC_FREQREF10M:
			sysData.inputLength = MAXFREQINPUTLENGTH; //���ÿ���������ĳ���
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

//���·��ؼ������������������ʾ�����磺�ڲ���ɨ��ֵ��������ʼƵ�ʺ���Ҫ���������������ʾ����"Start Freq:"
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
			drawSweep(sysData.sweep.type); //�б�ɨ�裬�ص���ɨ��/�б�״̬��Ϣ������
			break;
		default:
			break;
	}
}

//�����б������ļ�
void mainForm::analysisListdata(int num)
{
	if (sysScpi->isUpdating)
	{
		return;
	}
	//  QString filename = QCoreApplication::applicationDirPath() + "/ListData/listdata.txt";
	//  QString filename = "/mnt/usb/ListData/" + listDataEntry.at(num-1);		//U�̹��غ���б������ļ���λ��
	int tempNum = num;
	if (tempNum > listDataEntry.count()) //�������Ŵ������ļ������򷵻�
	{
		return;
	} else if (tempNum < 1) //��������С��1ʱ����
	{
		return;
	}

	QString folder = "";
	/* Ѱ���ļ����� */
	for (int i = 0; i < sizeof Upan / sizeof Upan[0]; i++)
	{
		if (Upan[i].num == LISTDATAENTRY)
		{
			folder = QString(Upan[i].folder).trimmed();
			break;
		}
	}
	if (folder == "") //û��Ѱ�ҵ��ļ��򷵻�
	{
		return;
	}

	//  QString filename = QCoreApplication::applicationDirPath() + "/ListData/" + listDataEntry.at(tempNum - 1);		//U�̹��غ���б������ļ���λ��
	QString filename = QCoreApplication::applicationDirPath() + "/" + folder + "/" + listDataEntry.at(tempNum - 1); //U�̹��غ���б������ļ���λ��
	QFile file(filename);

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		return;
	}
	QTextStream in(&file);
	QStringList dataList;
	QString temp;
	int i = 0;
	QString line = in.readLine(); //.txt�ĵ��ĵ�һ���ǡ���š���Ƶ�ʡ�����Ϣ��������Ҫ�ȶ�һ��

	memset(&sysData.sweep.list, 0, sizeof(sysData.sweep.list)); //���ԭ�����б���Ϣ
	while (!in.atEnd())
	{
		line = in.readLine(); //һ��һ�еĶ�
		dataList = line.split(QChar(0x09), QString::SkipEmptyParts); //0x09��'\t'��ÿһ���������������ݼ�ʹ��'\t'�����������ʹ��"hexdump -c �ļ���"��"hexdump -C �ļ���"�鿴
		temp = dataList.at(1); //Ƶ��
		sysData.sweep.list.data[i].freq = StrToValue(temp.trimmed().toUpper());
		if (sysData.sweep.list.data[i].freq > MAXFREQ)
		{
			sysData.sweep.list.data[i].freq = MAXFREQ;
		} else if (sysData.sweep.list.data[i].freq < MINFREQ)
		{
			sysData.sweep.list.data[i].freq = MINFREQ;
		}

		temp = dataList.at(2); //����
		sysData.sweep.list.data[i].ampl = StrToValue(temp);
		if (sysData.sweep.list.data[i].ampl > MAXAMPL)
		{
			sysData.sweep.list.data[i].ampl = MAXAMPL;
		} else if (sysData.sweep.list.data[i].ampl < MINAMPL)
		{
			sysData.sweep.list.data[i].ampl = MINAMPL;
		}

		temp = dataList.at(3); //פ��ʱ��
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
		sysData.sweep.list.allPage = i / 4 + 1; //������ʾ����ҳ��
	} else
	{
		sysData.sweep.list.allPage = i / 4;
	}

}

//�������ֵ��Ƶ������������ļ�
void mainForm::analysisDigiSequencedata(int num)
{
	if (sysScpi->isUpdating)
	{
		return;
	}
	int tempNum = num;
	if (tempNum > listDataEntry.count()) //�������Ŵ������ļ������򷵻�
	{
		return;
	} else if (tempNum < 1) //��������С��1ʱ����
	{
		return;
	}

	QString folder = "";
	/* Ѱ���ļ����� */
	for (int i = 0; i < sizeof Upan / sizeof Upan[0]; i++)
	{
		if (Upan[i].num == DIGITALDATAENTRY)
		{
			folder = QString(Upan[i].folder).trimmed();
			break;
		}
	}
	if (folder == "") //û��Ѱ�ҵ��ļ��򷵻�
	{
		return;
	}

	QString filename = QCoreApplication::applicationDirPath() + "/" + folder + "/" + listDataEntry.at(tempNum - 1); //U�̹��غ���б������ļ���λ��
	QFile file(filename);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		return;
	}
	QTextStream in(&file);
	int i = 0;
	QString line = "";
	memset(&sysData.digital.sequence, 0, sizeof(sysData.digital.sequence)); //���ԭ��������ֵ
	sysData.digital.sequenceCntBefore = 0;
	while (!in.atEnd())
	{
		line = in.readLine(8).simplified(); //8��һ�еĶ�,ȥ��\t���ַ�

		if (line.length() > 0)
		{
			sysData.digital.sequenceCntBefore += line.length();
			sysData.digital.sequence[i] = bqstringToUchar(line); //ת�����ַ�0,1��ʾ������Ϊ8λ�޷�������
			i++;
		}
		if (i > 3072)
		{
			break;
		}
	}
	sysData.digital.sequenceCnt = i; //��������ת����ĸ���
}

//��USB�в���num��Ӧ���ļ������е��ļ�Ŀ¼
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
	/* Ѱ���ļ����� */
	for (int i = 0; i < sizeof Upan / sizeof Upan[0]; i++)
	{
		if (Upan[i].num == num)
		{
			folder = QString(Upan[i].folder).trimmed();
			break;
		}
	}
	if (folder == "") //û��Ѱ�ҵ��ļ��򷵻�
	{
		return false;
	}

	QString filedir = QCoreApplication::applicationDirPath() + "/" + folder; //���ش�����ݵ��ļ���
	QDir dir(filedir);
	QString uFolderName;
	uFolderName = "/mnt/usb/" + folder; //����U�̺������ļ���Ŀ¼
	QDir udir(uFolderName);
	QString tempStr = "";

	if (!dir.exists()) //�ж��ļ����Ƿ���ڣ������������½�
	{
		QString temp = "mkdir " + filedir;
		system(temp.toStdString().c_str());
	}
	if (udir.exists()) //�ж�U�̹��سɹ������ɹ�����ɾ�����ص��ļ����µ�ȫ���ļ�
	{
		tempStr = "rm " + filedir + "/*";
		system(tempStr.toStdString().c_str());
	}
	tempStr = "cp " + uFolderName + "/* " + filedir;
	if (system(tempStr.toStdString().c_str()) != 0) //���������ļ�������
	{
		system("umount /mnt/usb/");
		return false;
	}

	system("umount /mnt/usb/"); //ж��U��

	dir.setFilter(QDir::NoDotAndDotDot | QDir::Files); //�����ļ���ʾ���ˣ�����ʾ��.���͡�..��

	listDataEntry = dir.entryList(); //��ȡĿ¼�¸��ļ���

	return true;
}

//����USB������Ŀ¼
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
	QRect rcTitle[2]; //��������Ϊ����
	QPainter p;
	QPixmap pix(rc.size());
	int row = 6;
	int titelRightWidth = 80;
	if (!pix.isNull())
	{
		p.begin(&pix);
		//������ƿռ�
		QRect rcTemp1 = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width(), pix.rect().height()); //����������������Ͳ�����
		QRect rcPara = QRect(pix.rect().x(), pix.rect().y() + pix.rect().height() / 4, pix.rect().width(), pix.rect().height() / row * 4); //������
		rcTitle[0] = QRect(pix.rect().x(), pix.rect().y(), pix.rect().width() - titelRightWidth, pix.rect().height() / row); //������0
		rcTitle[1] = QRect(pix.rect().width() - titelRightWidth, pix.rect().y(), titelRightWidth, pix.rect().height() / row); //������

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

		//���Ʊ�����
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
		for (int i = 0; i < listDataEntry.count(); i++) //������ź��ļ���
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

