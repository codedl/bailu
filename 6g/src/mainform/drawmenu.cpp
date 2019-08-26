#include "mainform.h"

//���ư�ť�˵�
void mainForm::drawButtonMenu(QPixmap* pix, subKeyDef* key, bool isNeedDrawButtonRect)
{
	if ((pix == NULL) || (pix->isNull() || key == NULL))
	{
		helpContent = "internal error";
		//    drawWin1_5();
		return;
	}

	QPainter p;
	p.begin(pix);
	QColor arrowBackColor;//��ͷ�ı���
	QColor arrowFillColor = QColor(0, 0, 0);;//����ͷ����ɫ
	if (key == nowSubKey)
	{
		p.fillRect(pix->rect(), BUTTONSELECTCOLOR);
		arrowBackColor = BUTTONSELECTCOLOR;
	} else
	{
		p.fillRect(pix->rect(), BUTTONUNSELECTCOLOR);
		arrowBackColor = BUTTONUNSELECTCOLOR;
	}

	if (isNeedDrawButtonRect)
	{
		p.setPen(QColor(0, 0, 0));
		p.drawRect(pix->rect().x(), pix->rect().y(), pix->rect().width(), pix->rect().height() - 1);	//���ƾ��ο�
	}

	if (!key->isEnabled)
	{
		p.setPen(BUTTONDISABLECOLOR);
		p.fillRect(pix->rect(), BUTTONUNSELECTCOLOR);
		if (isNeedDrawButtonRect)
		{
			p.setPen(QColor(0, 0, 0));
			p.drawRect(pix->rect().x(), pix->rect().y(), pix->rect().width(), pix->rect().height() - 1);	//���ƾ��ο�
			p.setPen(BUTTONDISABLECOLOR);
		}
		arrowBackColor = BUTTONUNSELECTCOLOR;
	}

	if (key->isBlank)
	{
		p.end();
		return;
	}

	

	QString keyName = getTranslatorString(key->enName, key->chName);
	//  QString keyName = key->enName;
	QString keyNamefont = "";
	QString keyNameback = "";

	int index = -1;
	//  index = keyName.indexOf("��",0);

	if (index >= 1)
	{
		keyNamefont = keyName.mid(0, index + 1);
		keyNameback = keyName.mid(index + 1, keyName.length() - 1);
	}

	char tempChar[32] = { };
	QRect rc;
	int rcWidthFloat = 0;
	int rcNums = 0;
	int rows = 1;
	int sep = 1;
	int enSep = 2;
	int arrowSep = 2;
	int pngSubSize = 12;
	int width = pix->width();
	int height = pix->height();
	p.setFont(fontBold);

	//	printf("w=%d,h=%d\r\n",(width - 2 * sep),((height - 2 * sep) / rows));
	//	printf("key->funcCode = 0x%x\n",key->funcCode);

	switch (key->funcCode)
	{
		//==================================================================================================================
		//====================================================== Ƶ�� ======================================================
		//==================================================================================================================
		case KEY_FUNC_FREQ:
		case KEY_FUNC_FREQ_GHZ:
		case KEY_FUNC_FREQ_MHZ:
		case KEY_FUNC_FREQ_KHZ:
		case KEY_FUNC_FREQ_HZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FREQREFSET:
		case KEY_FUNC_FREQREFSET_GHZ:
		case KEY_FUNC_FREQREFSET_MHZ:
		case KEY_FUNC_FREQREFSET_KHZ:
		case KEY_FUNC_FREQREFSET_HZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FREQREFSTATE:		//�ο�״̬
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.freq.ref.state)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("On"));
			rc = QRect(rc.right(), rc.top(), rc.width(), rc.height());
			if (!sysData.freq.ref.state)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("OFF"));
			break;
		case KEY_FUNC_FREQOFFSET:
		case KEY_FUNC_FREQOFFSET_GHZ:
		case KEY_FUNC_FREQOFFSET_MHZ:
		case KEY_FUNC_FREQOFFSET_KHZ:
		case KEY_FUNC_FREQOFFSET_HZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FREQMULTIPLIER:
		case KEY_FUNC_FREQMULTIPLIER_ENTER:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FREQREF10M:		//�ο�ʱ��
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.freq.ref10MHz == rmExternal)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("EXT"));
			rc = QRect(rc.right(), rc.top(), rc.width(), rc.height());
			if (sysData.freq.ref10MHz == rmInternal)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("INT"));
			break;

		//==================================================================================================================
		//=================================================== ���� =========================================================
		//==================================================================================================================
		case KEY_FUNC_AMPL:		//��������
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AMPL_DBM:
			if (sysData.ampl.ref.state)
			{
				keyName = "dB";
			}
			else
			{
				keyName = "dBm";
			}
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AMPL_DBUV:
		case KEY_FUNC_AMPL_MV:
			key->isEnabled = !sysData.ampl.ref.state;
			if (key->isEnabled)
				p.setPen(BUTTONENABLECOLOR);
			else
				p.setPen(BUTTONDISABLECOLOR);
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AMPLREFSET:	//���Ȳο�����
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AMPLREFSTATE:	//���Ȳο�״̬
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.ampl.ref.state)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("On"));
			rc = QRect(rc.right(), rc.top(), rc.width(), rc.height());
			if (!sysData.ampl.ref.state)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("OFF"));
			break;
		case KEY_FUNC_AMPLOFFSET:	//����ƫ������
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AMPLOFFSET_DBM:		
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AMPLRESPONSE:	//�ȷ���Ӧ
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.ampl.alc.response)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("Fast"));
			rc = QRect(rc.right(), rc.top(), rc.width(), rc.height());
			if (!sysData.ampl.alc.response)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("Slow"));
			break;	
		case KEY_FUNC_AMPL_ALC_STAT:	//�ȷ�״̬
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.ampl.alc.state)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("On"));
			rc = QRect(rc.right(), rc.top(), rc.width(), rc.height());
			if (!sysData.ampl.alc.state)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("OFF"));
			break;		
		case KEY_FUNC_AMPL_CABLE:		//����
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AMPL_CABLE_DB:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		//==================================================================================================================
		//==================================================== �������� ====================================================
		//==================================================================================================================
		case KEY_FUNC_INCRSET_SET:
			if (sysData.incrSet.type == itdFreq)
			{
				p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, getTheTranslatorString("Freq Incr"));
			}
			else if (sysData.incrSet.type == itdAmptd)
			{
				p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, getTheTranslatorString("Ampl Incr"));
			}
			break;
		case KEY_FUNC_INCRSETSET_GHZ:
			if (sysData.incrSet.type == itdFreq)
			{
				p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, "GHz");
			}
			else if (sysData.incrSet.type == itdAmptd)
			{
				p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, "dB");
			}
			break;
		case KEY_FUNC_INCRSETSET_MHZ:
			if (sysData.incrSet.type == itdFreq)
			{
				key->isEnabled = true;
				p.setPen(BUTTONENABLECOLOR);
				p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, "MHz");
			}
			else if (sysData.incrSet.type == itdAmptd)
			{
				key->isEnabled = false;
				p.setPen(BUTTONDISABLECOLOR);
				p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, "");
			}
			break;
		case KEY_FUNC_INCRSETSET_KHZ:
			if (sysData.incrSet.type == itdFreq)
			{
				key->isEnabled = true;
				p.setPen(BUTTONENABLECOLOR);
				p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, "kHz");
			}
			else if (sysData.incrSet.type == itdAmptd)
			{
				key->isEnabled = false;
				p.setPen(BUTTONDISABLECOLOR);
				p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, "");
			}
			break;
		case KEY_FUNC_INCRSETSET_HZ:
			if (sysData.incrSet.type == itdFreq)
			{
				key->isEnabled = true;
				p.setPen(BUTTONENABLECOLOR);
				p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, "Hz");
			}
			else if (sysData.incrSet.type == itdAmptd)
			{
				key->isEnabled = false;
				p.setPen(BUTTONDISABLECOLOR);
				p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, "");
			}
			break;
		//==================================================================================================================
		//====================================================== ɨ�� ======================================================
		//==================================================================================================================
		case KEY_FUNC_SWEEP_MODE:		//ɨ��ģʽ
			rc = QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			rc = QRect(rc.right() + sep, (height - pngSubSize) / 2, pngSubSize, pngSubSize);
			p.drawPixmap(rc, GetImgRight(pngSubSize, pngSubSize, arrowBackColor, arrowFillColor));
			break;
		case KEY_FUNC_SWEEP_MODE_OFF:	//ɨ��ģʽ�����ر�
		case KEY_FUNC_SWEEP_MODE_FREQ:	//ɨ��ģʽ����Ƶ��
		case KEY_FUNC_SWEEP_MODE_AMPL:	//ɨ��ģʽ��������
		case KEY_FUNC_SWEEP_MODE_FREQAMPL:	//ɨ��ģʽ����Ƶ��+����
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SWEEP_TYPE:		//ɨ������
			key->isEnabled = ((sysData.sweep.mode == smAmpl) || (sysData.sweep.mode == smFreq) || (sysData.sweep.mode == smFreqAmpl));
			if (key->isEnabled)
			{
				p.setPen(BUTTONENABLECOLOR);
			}
			else
			{
				p.setPen(BUTTONDISABLECOLOR);
			}
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.sweep.type == stList)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("List"));
			rc = QRect(rc.right(), rc.top(), rc.width(), rc.height());
			if (sysData.sweep.type == stStep)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("Step"));
			break;
		case KEY_FUNC_SWEEP_REPEAT:		//�ظ�ɨ��
			key->isEnabled = ((sysData.sweep.mode == smAmpl) || (sysData.sweep.mode == smFreq) || (sysData.sweep.mode == smFreqAmpl));
			if (key->isEnabled)
				p.setPen(BUTTONENABLECOLOR);
			else
				p.setPen(BUTTONDISABLECOLOR);
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.sweep.repeat == srConutinuos)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("Cont"));
			rc = QRect(rc.right(), rc.top(), rc.width(), rc.height());
			if (sysData.sweep.repeat == srSingle)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("Single"));
			break;
		case KEY_FUNC_SWEEP_CONFIGSTEP:	//��������
			key->isEnabled = (sysData.sweep.type == stStep && sysData.sweep.mode != smOff);
			if (key->isEnabled)
			{
				p.setPen(BUTTONENABLECOLOR);
			}
			else
			{
				p.setPen(BUTTONDISABLECOLOR);
			}
			rc = QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			rc = QRect(rc.right() + sep, (height - pngSubSize) / 2, pngSubSize, pngSubSize);
			p.drawPixmap(rc, GetImgRight(pngSubSize, pngSubSize, arrowBackColor, arrowFillColor));
			break;
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST:	//�б�����
			key->isEnabled = (sysData.sweep.type == stList && sysData.sweep.mode != smOff);
			if (key->isEnabled)
				p.setPen(BUTTONENABLECOLOR);
			else
				p.setPen(BUTTONDISABLECOLOR);
			rc = QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			rc = QRect(rc.right() + sep, (height - pngSubSize) / 2, pngSubSize, pngSubSize);
			p.drawPixmap(rc, GetImgRight(pngSubSize, pngSubSize, arrowBackColor, arrowFillColor));
			break;
		case KEY_FUNC_SWEEP_MORE_SWEEPTRIGGER:	//ɨ�败��
			key->isEnabled = ((sysData.sweep.mode == smAmpl) || (sysData.sweep.mode == smFreq) || (sysData.sweep.mode == smFreqAmpl));
			if (key->isEnabled)
				p.setPen(BUTTONENABLECOLOR);
			else
				p.setPen(BUTTONDISABLECOLOR);
			rc = QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			rc = QRect(rc.right() + sep, (height - pngSubSize) / 2, pngSubSize, pngSubSize);
			p.drawPixmap(rc, GetImgRight(pngSubSize, pngSubSize, arrowBackColor, arrowFillColor));
			break;
		case KEY_FUNC_SWEEP_MORE_SWEEPDIRECTION:	//ɨ�跽��
			key->isEnabled = ((sysData.sweep.mode == smAmpl) || (sysData.sweep.mode == smFreq) || (sysData.sweep.mode == smFreqAmpl));
			if (key->isEnabled)
				p.setPen(BUTTONENABLECOLOR);
			else
				p.setPen(BUTTONDISABLECOLOR);
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if(sysData.sweep.direction == sdDown)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("Down"));	//����
			rc = QRect(rc.right(), rc.top(), rc.width(), rc.height());
			if (sysData.sweep.direction == sdUp)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("Up"));		//����
			break;
		case KEY_FUNC_SWEEP_MORE_SWEEPRETRACE:		//ɨ���۷�
			key->isEnabled = ((sysData.sweep.mode == smAmpl) || (sysData.sweep.mode == smFreq) || (sysData.sweep.mode == smFreqAmpl));
			if (key->isEnabled)
				p.setPen(BUTTONENABLECOLOR);
			else
				p.setPen(BUTTONDISABLECOLOR);			
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignCenter, keyName);
			if (sysData.sweep.retrace == srOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("On"));	//�ر�
			rc = QRect(rc.right(), rc.top(), rc.width(), rc.height());
			if (sysData.sweep.retrace == srOff)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("Off"));		//����
			break;
		

		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_MORE_DWELLTYPE:

		case KEY_FUNC_SWEEP_STEP_FREQSTART_GHZ:
		case KEY_FUNC_SWEEP_STEP_FREQSTART_MHZ:
		case KEY_FUNC_SWEEP_STEP_FREQSTART_KHZ:
		case KEY_FUNC_SWEEP_STEP_FREQSTART_HZ:
		case KEY_FUNC_SWEEP_STEP_FREQSTOP_GHZ:
		case KEY_FUNC_SWEEP_STEP_FREQSTOP_MHZ:
		case KEY_FUNC_SWEEP_STEP_FREQSTOP_KHZ:
		case KEY_FUNC_SWEEP_STEP_FREQSTOP_HZ:
		case KEY_FUNC_SWEEP_STEP_AMPLSTART_DBM:
		case KEY_FUNC_SWEEP_STEP_AMPLSTOP_DBM:
		case KEY_FUNC_SWEEP_STEP_MORE_POINTS_ENTER:
		case KEY_FUNC_SWEEP_STEP_MORE_STEPDWELL_S:
		case KEY_FUNC_SWEEP_STEP_MORE_STEPDWELL_MS:
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_EDITITEM_GHZS:
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_EDITITEM_MHZMS:
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_EDITITEM_KHZNS:
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_EDITITEM_HZDBM:
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_GOTOROW_ENTER:
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_MORE_DOWNLOAD_ENTER:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;


		case KEY_FUNC_SWEEP_TYPE_LIST:
		case KEY_FUNC_SWEEP_TYPE_STEP:
		//case KEY_FUNC_SWEEP_REPEAT_SINGLE:
		//case KEY_FUNC_SWEEP_REPEAT_CONT:
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_MORE_DOWNLOAD:
		case KEY_FUNC_SWEEP_STEP_MORE_POINTS:
		case KEY_FUNC_SWEEP_STEP_MORE_STEPDWELL:
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_EDITITEM:
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_INSERTROW:
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_DELETEROW:
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_MORE_LOADSTORE:
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_GOTOROW:
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_MORE_PRESETLIST:
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_MORE_PRESETLIST_YES:
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_MORE_PRESETLIST_NO:
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_MORE_INSERTITEM:
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_MORE_DELETEITEM:
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_MORE_DWELLTYPE_LIST:
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_MORE_DWELLTYPE_STEP:
		case KEY_FUNC_SWEEP_MORE_SWEEPDIRECTION_DOWN:
		case KEY_FUNC_SWEEP_MORE_SWEEPDIRECTION_UP:
		case KEY_FUNC_SWEEP_MORE_SWEEPRETRACE_OFF:
		case KEY_FUNC_SWEEP_MORE_SWEEPRETRACE_ON:
		case KEY_FUNC_SWEEP_MORE_SWEEPTRIGGER_FREERUN:
		case KEY_FUNC_SWEEP_MORE_SWEEPTRIGGER_TRIGGERKEY:
		case KEY_FUNC_SWEEP_MORE_SWEEPTRIGGER_BUS:
		case KEY_FUNC_SWEEP_MORE_SWEEPTRIGGER_EXT:
		case KEY_FUNC_SWEEP_MORE_SWEEPTRIGGER_TIMETRIGGER:
			key->isEnabled = sysData.sweep.mode != smOff;
			if (key->isEnabled)
				p.setPen(BUTTONENABLECOLOR);
			else
				p.setPen(BUTTONDISABLECOLOR);
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SWEEP_SINGLETRIGGER:
			key->isEnabled = ((sysData.sweep.mode == smAmpl) || (sysData.sweep.mode == smFreq) || (sysData.sweep.mode == smFreqAmpl));
			if (key->isEnabled)
				p.setPen(BUTTONENABLECOLOR);
			else
				p.setPen(BUTTONDISABLECOLOR);
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SWEEP_STEP_FREQSTART:
			key->isEnabled = (sysData.sweep.mode == smFreq) || (sysData.sweep.mode == smFreqAmpl);
			if (!key->isEnabled)
				p.setPen(BUTTONDISABLECOLOR);
			else
				p.setPen(BUTTONENABLECOLOR);

			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SWEEP_STEP_FREQSTOP:
			key->isEnabled = (sysData.sweep.mode == smFreq) || (sysData.sweep.mode == smFreqAmpl);
			if (!key->isEnabled)
				p.setPen(BUTTONDISABLECOLOR);
			else
				p.setPen(BUTTONENABLECOLOR);

			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SWEEP_STEP_AMPLSTART:
			key->isEnabled = (sysData.sweep.mode == smAmpl) || (sysData.sweep.mode == smFreqAmpl);
			if (!key->isEnabled)
				p.setPen(BUTTONDISABLECOLOR);
			else
				p.setPen(BUTTONENABLECOLOR);

			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;

		case KEY_FUNC_SWEEP_STEP_AMPLSTOP:
			key->isEnabled = (sysData.sweep.mode == smAmpl) || (sysData.sweep.mode == smFreqAmpl);
			if (!key->isEnabled)
				p.setPen(BUTTONDISABLECOLOR);
			else
				p.setPen(BUTTONENABLECOLOR);

			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;

		case KEY_FUNC_SWEEP_STEP_MORE:
		case KEY_FUNC_SWEEP_MORE:
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_MORE:
			p.drawPixmap(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), img_DOWN);
			break;

		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_RETURN:
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_MORE_DWELLTYPE_RETURN:
		case KEY_FUNC_SWEEP_MORE_SWEEPDIRECTION_RETURN:
		case KEY_FUNC_SWEEP_MORE_SWEEPRETRACE_RETURN:
		case KEY_FUNC_SWEEP_MODE_RETURN:
		case KEY_FUNC_SWEEP_TYPE_RETURN:
		case KEY_FUNC_SWEEP_STEP_RETURN:
		case KEY_FUNC_SWEEP_MORE_SWEEPTRIGGER_RETURN:
			p.drawPixmap(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), img_OUT);
			break;

		case KEY_FUNC_SWEEP_MORE_RETURN:
		case KEY_FUNC_SWEEP_STEP_MORE_RETURN:
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_MORE_RETURN:
			p.drawPixmap(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), img_UP);
			break;
		//==================================================================================================================
		//===================================================== AM =========================================================
		//==================================================================================================================
		case KEY_FUNC_AM_STATE:		//AM״̬
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.am.state == true)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("On"));	
			rc = QRect(rc.right(), rc.top(), rc.width(), rc.height());
			if (sysData.am.state == false)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("Off"));	
			break;
		case KEY_FUNC_AM_DEPTH:		//AM�������
			p.drawText(QRect(sep, sep + 4, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AM_DEPTH_PERCENT:	//AM�������
			p.drawText(QRect(sep, sep + 4, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AM_SOURCE:	//����Դ
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.am.source == true)	//�ⲿ
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("EXT"));
			rc = QRect(rc.right(), rc.top(), rc.width(), rc.height());
			if (sysData.am.source == false)	//�ڲ�
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("INT"));
			break;
		case KEY_FUNC_AM_INTERNALFCTN:	//�ڲ���������
			key->isEnabled = !sysData.am.source;
			if (!key->isEnabled)
				p.setPen(BUTTONDISABLECOLOR);
			else
				p.setPen(BUTTONENABLECOLOR);
			rc = QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			rc = QRect(rc.right() + sep, (height - pngSubSize) / 2, pngSubSize, pngSubSize);
			p.drawPixmap(rc, GetImgRight(pngSubSize, pngSubSize, arrowBackColor, arrowFillColor));
			break;
		case KEY_FUNC_AM_INTERNALFCTN_WAVE:	//�ڲ���������_����
			rc = QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			rc = QRect(rc.right() + sep, (height - pngSubSize) / 2, pngSubSize, pngSubSize);
			p.drawPixmap(rc, GetImgRight(pngSubSize, pngSubSize, arrowBackColor, arrowFillColor));
			break;
		case KEY_FUNC_AM_INTERNALFCTN_WAVE_SINE://�ڲ���������_����_���Ҳ�
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AM_INTERNALFCTN_WAVE_TRIANGLE://�ڲ���������_����_���ǲ�
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AM_INTERNALFCTN_WAVE_SQUARE://�ڲ���������_����_����
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AM_INTERNALFCTN_WAVE_SAWTOOTH://�ڲ���������_����_��ݲ�
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AM_INTERNALFCTN_FREQ://�ڲ���������_Ƶ��
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AM_INTERNALFCTN_FREQ_MHZ://�ڲ���������_Ƶ��
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AM_INTERNALFCTN_FREQ_KHZ://�ڲ���������_Ƶ��
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AM_INTERNALFCTN_FREQ_HZ://�ڲ���������_Ƶ��
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AM_COUPLING:	//������
			key->isEnabled = sysData.am.source;
			if (!key->isEnabled)
				p.setPen(BUTTONDISABLECOLOR);
			else
				p.setPen(BUTTONENABLECOLOR);
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.am.coupling == true)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("On"));
			rc = QRect(rc.right(), rc.top(), rc.width(), rc.height());
			if (sysData.am.coupling == false)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("Off"));
			break;
		//==================================================================================================================
		//========================================================= FMPM ===================================================
		//==================================================================================================================
		case KEY_FUNC_FMPM_MODE:	//���Ʒ�ʽ
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.fmpm.mode == fdtPM)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("��M"));
			rc = QRect(rc.right(), rc.top(), rc.width(), rc.height());
			if (sysData.fmpm.mode == fdtFM)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("FM"));
			break;
		case KEY_FUNC_FMPM_STATUS:	//����״̬
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			if (sysData.fmpm.mode == fdtPM)
			{
				p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("��M State"));
				rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
				if(sysData.fmpm.pm.state)
					p.setFont(fontUnderline);
				else
					p.setFont(fontLight);
				p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("On"));
				rc = QRect(rc.right(), rc.top(), rc.width(), rc.height());
				if (!sysData.fmpm.pm.state)
					p.setFont(fontUnderline);
				else
					p.setFont(fontLight);
				p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("Off"));
			}
			else if (sysData.fmpm.mode == fdtFM)
			{
				p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("FM State"));
				rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
				if(sysData.fmpm.fm.state)
					p.setFont(fontUnderline);
				else
					p.setFont(fontLight);
				p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("On"));
				rc = QRect(rc.right(), rc.top(), rc.width(), rc.height());
				if (!sysData.fmpm.fm.state)
					p.setFont(fontUnderline);
				else
					p.setFont(fontLight);
				p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("Off"));
			}
			break;
		case KEY_FUNC_FMPM_OFFSET:		//����ƫ��
			if (sysData.fmpm.mode == fdtFM)
			{
				p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, getTheTranslatorString("Freq Offset"));
			}
			else if(sysData.fmpm.mode == fdtPM)
			{
				p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, getTheTranslatorString("Phase Offset"));
			}
			break;
		case KEY_FUNC_FMPM_OFFSET_MHZ:	//����ƫ�� MHz | ��
			if (sysData.fmpm.mode == fdtFM)
			{
				p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, "MHz");
			}
			else if (sysData.fmpm.mode == fdtPM)
			{
				p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, "��");
			}
			break;
		case KEY_FUNC_FMPM_OFFSET_KHZ:	//����ƫ�� kHz
			if (sysData.fmpm.mode == fdtFM)
			{
				key->isEnabled = true;
				p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, "kHz");
			}
			else if (sysData.fmpm.mode == fdtPM)
			{
				key->isEnabled = false;
				p.setPen(BUTTONDISABLECOLOR);
				p.fillRect(pix->rect(), BUTTONUNSELECTCOLOR);
				p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, "");
			}
			break;
		case KEY_FUNC_FMPM_OFFSET_HZ:	//����ƫ�� Hz
			if (sysData.fmpm.mode == fdtFM)
			{
				key->isEnabled = true;
				p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, "Hz");
			}
			else if (sysData.fmpm.mode == fdtPM)
			{
				key->isEnabled = false;
				p.setPen(BUTTONDISABLECOLOR);
				p.fillRect(pix->rect(), BUTTONUNSELECTCOLOR);
				p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, "");
			}
			break;
		case KEY_FUNC_FMPM_SOURCE:	//����Դ
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.fmpm.mode == fdtFM)
			{
				if (sysData.fmpm.fm.source == true)	//�ⲿ
					p.setFont(fontUnderline);
				else
					p.setFont(fontLight);
				p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("EXT"));
				rc = QRect(rc.right(), rc.top(), rc.width(), rc.height());
				if (sysData.fmpm.fm.source == false)	//�ڲ�
					p.setFont(fontUnderline);
				else
					p.setFont(fontLight);
				p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("INT"));
			}
			else if (sysData.fmpm.mode == fdtPM)
			{
				if(sysData.fmpm.pm.source == true)
					p.setFont(fontUnderline);
				else
					p.setFont(fontLight);
				p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("EXT"));
				rc = QRect(rc.right(), rc.top(), rc.width(), rc.height());
				if (sysData.fmpm.pm.source == false)	//�ڲ�
					p.setFont(fontUnderline);
				else
					p.setFont(fontLight);
				p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("INT"));
			}
			break;
		case KEY_FUNC_FMPM_INTERNALFCTN:	//�ڲ�����
			if (sysData.fmpm.mode == fdtFM)
			{
				key->isEnabled = !sysData.fmpm.fm.source;
			}
			else if (sysData.fmpm.mode == fdtPM)
			{
				key->isEnabled = !sysData.fmpm.pm.source;
			}
			if (!key->isEnabled)
				p.setPen(BUTTONDISABLECOLOR);
			else
				p.setPen(BUTTONENABLECOLOR);
			rc = QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			rc = QRect(rc.right() + sep, (height - pngSubSize) / 2, pngSubSize, pngSubSize);
			p.drawPixmap(rc, GetImgRight(pngSubSize, pngSubSize, arrowBackColor, arrowFillColor));
			break;
		case KEY_FUNC_FMPM_INTERNALFCTN_WAVE:	//�ڲ���������_����
			rc = QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignCenter, keyName);
			rc = QRect(rc.right() + sep, (height - pngSubSize) / 2, pngSubSize, pngSubSize);
			p.drawPixmap(rc, GetImgRight(pngSubSize, pngSubSize, arrowBackColor, arrowFillColor));
			break;
		case KEY_FUNC_FMPM_INTERNALFCTN_WAVE_SINE://�ڲ���������_����_���Ҳ�
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FMPM_INTERNALFCTN_WAVE_TRIANGLE://�ڲ���������_����_���ǲ�
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FMPM_INTERNALFCTN_WAVE_SQUARE://�ڲ���������_����_����
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FMPM_INTERNALFCTN_WAVE_SAWTOOTH://�ڲ���������_����_��ݲ�
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FMPM_INTERNALFCTN_FREQ:	//��������
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FMPM_INTERNALFCTN_FREQ_MHZ://��������_MHz
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FMPM_INTERNALFCTN_FREQ_KHZ://��������_kHz
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FMPM_INTERNALFCTN_FREQ_HZ://��������_Hz
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FMPM_COUPLING:	//����ƫ�����
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			if (sysData.fmpm.mode == fdtFM)
			{
				key->isEnabled = sysData.fmpm.fm.source;
				if (!key->isEnabled)
					p.setPen(BUTTONDISABLECOLOR);
				else
					p.setPen(BUTTONENABLECOLOR);
				p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("FM Dev Couple"));
				rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
				if (sysData.fmpm.fm.coupling)
					p.setFont(fontUnderline);
				else
					p.setFont(fontLight);
				p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("On"));
				rc = QRect(rc.right(), rc.top(), rc.width(), rc.height());
				if (!sysData.fmpm.fm.coupling)
					p.setFont(fontUnderline);
				else
					p.setFont(fontLight);
				p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("Off"));
			}
			else if (sysData.fmpm.mode == fdtPM)
			{
				key->isEnabled = sysData.fmpm.pm.source;
				if (!key->isEnabled)
					p.setPen(BUTTONDISABLECOLOR);
				else
					p.setPen(BUTTONENABLECOLOR);
				p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("��M Dev Couple"));
				rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
				if (sysData.fmpm.pm.coupling)
					p.setFont(fontUnderline);
				else
					p.setFont(fontLight);
				p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("On"));
				rc = QRect(rc.right(), rc.top(), rc.width(), rc.height());
				if (!sysData.fmpm.pm.coupling)
					p.setFont(fontUnderline);
				else
					p.setFont(fontLight);
				p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("Off"));
			}
			break;

			//==================================================================================================================
			//========================================================= IQ ====================================================
			//==================================================================================================================
		case KEY_FUNC_IQ_NOFUNCTION:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;

			//==================================================================================================================
			//========================================================= Pulse ===================================================
			//==================================================================================================================
		case KEY_FUNC_PULSE_STATE:	//����״̬
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.pulse.state)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("On"));
			rc = QRect(rc.right(), rc.top(), rc.width(), rc.height());
			if (!sysData.pulse.state)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("Off"));
			break;
		case KEY_FUNC_PULSE_TRIGGERWAY:	//������ʽ
			rc = QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			rc = QRect(rc.right() + sep, (height - pngSubSize) / 2, pngSubSize, pngSubSize);
			p.drawPixmap(rc, GetImgRight(pngSubSize, pngSubSize, arrowBackColor, arrowFillColor));
			break;
		case KEY_FUNC_PULSE_TRIGGERWAY_FREERUN:	//������ʽ_��������
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_PULSE_TRIGGERWAY_KEYTRIG:	//������ʽ_��������
			if (sysData.pulse.pulsesource == psExternal)
				key->isEnabled = false;
			else
				key->isEnabled = true;
			if (!key->isEnabled)
				p.setPen(BUTTONDISABLECOLOR);
			else
				p.setPen(BUTTONENABLECOLOR);
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_PULSE_TRIGGERWAY_EXTTRIG:	//������ʽ_�ⲿ����
			if (sysData.pulse.pulsesource == psExternal)
				key->isEnabled = false;
			else
				key->isEnabled = true;
			if (!key->isEnabled)
				p.setPen(BUTTONDISABLECOLOR);
			else
				p.setPen(BUTTONENABLECOLOR);
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_PULSE_SOURCE:	//����Դ
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.pulse.pulsesource == psExternal)	//�ⲿ
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("EXT"));
			rc = QRect(rc.right(), rc.top(), rc.width(), rc.height());
			if (sysData.pulse.pulsesource == psInternal)	//�ڲ�
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("INT"));
			break;
		case KEY_FUNC_PULSE_PERIOD:	//��������
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_PULSE_PERIOD_S:	//��������_S
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_PULSE_PERIOD_MS:	//��������_mS
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_PULSE_PERIOD_US:	//��������_uS
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_PULSE_PERIOD_NS:	//��������_nS
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_PULSE_WIDTH:	//������
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_PULSE_WIDTH_S:	//������_S
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_PULSE_WIDTH_MS:	//������_mS
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_PULSE_WIDTH_US:	//������_uS
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_PULSE_WIDTH_NS:	//������_uS
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_PULSE_DELAY:		//������ʱ
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_PULSE_DELAY_S:	//������ʱ_S
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_PULSE_DELAY_MS:	//������ʱ_mS
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_PULSE_DELAY_US:	//������ʱ_uS
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_PULSE_DELAY_NS:	//������ʱ_nS
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_PULSE_EXTPOLA:	//�ⲿ�����źż���
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.pulse.polarity == pepRising)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("Rasing"));
			rc = QRect(rc.right(), rc.top(), rc.width(), rc.height());
			if (sysData.pulse.polarity == pepFall)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("Fall"));
			break;
			//==================================================================================================================
			//====================================================== AUX �ϱ�Ƶ ================================================
			//==================================================================================================================
		case KEY_FUNC_AUXFCTN_EXTRF:	//�ϱ�Ƶ
			key->isEnabled = sysData.freq.freq > 300e6; //����Ƶ��С��300MHzʱ���ϱ�Ƶ���ܲ�����
			if (key->isEnabled)
				p.setPen(BUTTONENABLECOLOR);
			else
				p.setPen(BUTTONDISABLECOLOR);
			rc = QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			rc = QRect(rc.right() + sep, (height - pngSubSize) / 2, pngSubSize, pngSubSize);
			p.drawPixmap(rc, GetImgRight(pngSubSize, pngSubSize, arrowBackColor, arrowFillColor));
			break;
		case KEY_FUNC_AUXFCTN_EXTRF_SOURCE:	//��ƵԴ
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.exfRef.type == erftExternal)	//�ⲿ
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("EXT"));
			rc = QRect(rc.right(), rc.top(), rc.width(), rc.height());
			if (sysData.exfRef.type == erftInternal)	//�ڲ�
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("INT"));
			break;
		case KEY_FUNC_AUXFCTN_EXTRF_EXTIF:	//�ⲿ��ƵƵ��
			key->isEnabled = sysData.exfRef.type == erftExternal;
			if (!key->isEnabled)
				p.setPen(BUTTONDISABLECOLOR);
			else
				p.setPen(BUTTONENABLECOLOR);
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_EXTRF_EXTIF_MHZ:	//�ⲿ��ƵƵ��_MHZ
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_EXTRF_EXTIF_KHZ:	//�ⲿ��ƵƵ��_KHZ
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_EXTRF_EXTIF_HZ:	//�ⲿ��ƵƵ��_HZ
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
			//==================================================================================================================
			//====================================================== AUX ��Ƶ��� ==============================================
			//==================================================================================================================
		case KEY_FUNC_AUXFCTN_LFOUT:	//��Ƶ���
			rc = QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			rc = QRect(rc.right() + sep, (height - pngSubSize) / 2, pngSubSize, pngSubSize);
			p.drawPixmap(rc, GetImgRight(pngSubSize, pngSubSize, arrowBackColor, arrowFillColor));
			break;
		case KEY_FUNC_AUXFCTN_LFOUT_WAVE:	//����
			rc = QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			rc = QRect(rc.right() + sep, (height - pngSubSize) / 2, pngSubSize, pngSubSize);
			p.drawPixmap(rc, GetImgRight(pngSubSize, pngSubSize, arrowBackColor, arrowFillColor));
			break;
		case KEY_FUNC_AUXFCTN_LFOUT_WAVE_SINE:	//���Ҳ�
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_LFOUT_WAVE_SQUARE:	//����
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_LFOUT_WAVE_TRIANGLE:	//���ǲ�
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_LFOUT_WAVE_SAWTOOTH:	//��ݲ�
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_LFOUT_FREQ:	//Ƶ��
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_LFOUT_FREQ_KHZ:	//Ƶ��_kHz
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_LFOUT_FREQ_HZ:	//Ƶ��_Hz
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_LFOUT_AMPL:	//����
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_LFOUT_AMPL_MV:	//����mV
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_LFOUT_AMPL_V:	//����mV
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
			//==================================================================================================================
			//======================================================= AUX DRG ==================================================
			//==================================================================================================================
		case KEY_FUNC_AUXFCTN_DRG:	//DRG
			rc = QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			rc = QRect(rc.right() + sep, (height - pngSubSize) / 2, pngSubSize, pngSubSize);
			p.drawPixmap(rc, GetImgRight(pngSubSize, pngSubSize, arrowBackColor, arrowFillColor));
			break;
		case KEY_FUNC_AUXFCTN_DRG_STATE:	//״̬
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.drg.state)	//����
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("On"));
			rc = QRect(rc.right(), rc.top(), rc.width(), rc.height());
			if (!sysData.drg.state)	//�ر�
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("Off"));
			break;
		case KEY_FUNC_AUXFCTN_DRG_UPPER:	//ɨ������
			key->isEnabled = sysData.drg.state;
			if (key->isEnabled)
				p.setPen(BUTTONENABLECOLOR);
			else
				p.setPen(BUTTONDISABLECOLOR);
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_DRG_UPPER_GHZ:	//ɨ������_GHz
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_DRG_UPPER_MHZ:	//ɨ������_MHz
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_DRG_UPPER_KHZ:	//ɨ������_kHz
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_DRG_UPPER_HZ:		//ɨ������_Hz
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_DRG_LOW:	//ɨ������
			key->isEnabled = sysData.drg.state;
			if (key->isEnabled)
				p.setPen(BUTTONENABLECOLOR);
			else
				p.setPen(BUTTONDISABLECOLOR);
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_DRG_LOW_GHZ:	//ɨ������_GHz
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_DRG_LOW_MHZ:	//ɨ������_MHz
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_DRG_LOW_KHZ:	//ɨ������_kHz
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_DRG_LOW_HZ:	//ɨ������_Hz
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_DRG_POINTS:	//ɨ�����
			key->isEnabled = sysData.drg.state;
			if (key->isEnabled)
				p.setPen(BUTTONENABLECOLOR);
			else
				p.setPen(BUTTONDISABLECOLOR);
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_DRG_POINTS_ENTER:	//ɨ�����
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_DRG_RATE:	//ɨ������
			key->isEnabled = sysData.drg.state;
			if (key->isEnabled)
				p.setPen(BUTTONENABLECOLOR);
			else
				p.setPen(BUTTONDISABLECOLOR);
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_DRG_RATE_NS:	//ɨ������_ns
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_DRG_RATE_US:	//ɨ������_us
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_DRG_MODE:	//����ģʽ
			key->isEnabled = sysData.drg.state;
			if (key->isEnabled)
				p.setPen(BUTTONENABLECOLOR);
			else
				p.setPen(BUTTONDISABLECOLOR);
			rc = QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			rc = QRect(rc.right() + sep, (height - pngSubSize) / 2, pngSubSize, pngSubSize);
			p.drawPixmap(rc, GetImgRight(pngSubSize, pngSubSize, arrowBackColor, arrowFillColor));
			break;
		case KEY_FUNC_AUXFCTN_DRG_MODE_BOTHWAY:	//˫��ɨ��
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_DRG_MODE_POSSINGLE:	//����·
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_DRG_MODE_NEGSINGLE:	//����·
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		//==================================================================================================================
		//======================================================= AUX ���ֵ��� =============================================
		//==================================================================================================================
		case KEY_FUNC_AUXFCTN_DIGITALMOD:		//���ֵ��ƣ��ù���ֻ��SG1060A��
			if (DEVICETYPE == 0)
				key->isEnabled = true;
			else
				key->isEnabled = false;
			if (key->isEnabled)
				p.setPen(BUTTONENABLECOLOR);
			else
				p.setPen(BUTTONDISABLECOLOR);
			rc = QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			rc = QRect(rc.right() + sep, (height - pngSubSize) / 2, pngSubSize, pngSubSize);
			p.drawPixmap(rc, GetImgRight(pngSubSize, pngSubSize, arrowBackColor, arrowFillColor));
			break;
		case KEY_FUNC_AUXFCTN_DIGITALMOD_STATE:	//״̬
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.digital.digitalstate)	//����
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("On"));
			rc = QRect(rc.right(), rc.top(), rc.width(), rc.height());
			if (!sysData.digital.digitalstate)	//�ر�
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("Off"));
			break;
		case KEY_FUNC_AUXFCTN_DIGITALMOD_MODE:	//����ģʽ
			key->isEnabled = sysData.digital.digitalstate;
			if (key->isEnabled)
				p.setPen(BUTTONENABLECOLOR);
			else
				p.setPen(BUTTONDISABLECOLOR);
			rc = QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			rc = QRect(rc.right() + sep, (height - pngSubSize) / 2, pngSubSize, pngSubSize);
			p.drawPixmap(rc, GetImgRight(pngSubSize, pngSubSize, arrowBackColor, arrowFillColor));
			break;
		case KEY_FUNC_AUXFCTN_DIGITALMOD_MODE_ASK:	//ASK
			key->isEnabled = sysData.digital.digitalstate;
			if (key->isEnabled)
				p.setPen(BUTTONENABLECOLOR);
			else
				p.setPen(BUTTONDISABLECOLOR);
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_DIGITALMOD_MODE_FSK:	//FSK
			key->isEnabled = sysData.digital.digitalstate;
			if (key->isEnabled)
				p.setPen(BUTTONENABLECOLOR);
			else
				p.setPen(BUTTONDISABLECOLOR);
			rc = QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			rc = QRect(rc.right() + sep, (height - pngSubSize) / 2, pngSubSize, pngSubSize);
			p.drawPixmap(rc, GetImgRight(pngSubSize, pngSubSize, arrowBackColor, arrowFillColor));
			break;
		case KEY_FUNC_AUXFCTN_DIGITALMOD_MODE_FSK_2FSK:	//2FSK
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_DIGITALMOD_MODE_FSK_4FSK:	//4FSK
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_DIGITALMOD_MODE_FSK_8FSK:	//8FSK
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_DIGITALMOD_MODE_PSK:	//PSK
			key->isEnabled = sysData.digital.digitalstate;
			if (key->isEnabled)
				p.setPen(BUTTONENABLECOLOR);
			else
				p.setPen(BUTTONDISABLECOLOR);
			rc = QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			rc = QRect(rc.right() + sep, (height - pngSubSize) / 2, pngSubSize, pngSubSize);
			p.drawPixmap(rc, GetImgRight(pngSubSize, pngSubSize, arrowBackColor, arrowFillColor));
			break;
		case KEY_FUNC_AUXFCTN_DIGITALMOD_MODE_PSK_2PSK:	//2PSK
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_DIGITALMOD_MODE_PSK_4PSK:	//4PSK
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_DIGITALMOD_MODE_FSK_8PSK:	//8PSK
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA:	//���Ʋ���
			key->isEnabled = sysData.digital.digitalstate;
			if (key->isEnabled)
				p.setPen(BUTTONENABLECOLOR);
			else
				p.setPen(BUTTONDISABLECOLOR);
			rc = QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			rc = QRect(rc.right() + sep, (height - pngSubSize) / 2, pngSubSize, pngSubSize);
			p.drawPixmap(rc, GetImgRight(pngSubSize, pngSubSize, arrowBackColor, arrowFillColor));
			break;
		case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA_DEPTH:	//�������
			if (sysData.digital.digitaltype == dt2ASK || sysData.digital.digitaltype == dt4ASK || sysData.digital.digitaltype == dt8ASK)
				key->isEnabled = true;
			else
				key->isEnabled = false;
			if (key->isEnabled)
				p.setPen(BUTTONENABLECOLOR);
			else
				p.setPen(BUTTONDISABLECOLOR);
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA_DEPTH_PERCENT:	//�������_%
			if (sysData.digital.digitaltype == dt2ASK || sysData.digital.digitaltype == dt4ASK || sysData.digital.digitaltype == dt8ASK)
				key->isEnabled = true;
			else
				key->isEnabled = false;
			if (key->isEnabled)
				p.setPen(BUTTONENABLECOLOR);
			else
				p.setPen(BUTTONDISABLECOLOR);
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA_DEVI:	//���Ƶƫ
			if (sysData.digital.digitaltype == dt2FSK || sysData.digital.digitaltype == dt4FSK || sysData.digital.digitaltype == dt8FSK)
				key->isEnabled = true;
			else
				key->isEnabled = false;
			if (key->isEnabled)
				p.setPen(BUTTONENABLECOLOR);
			else
				p.setPen(BUTTONDISABLECOLOR);
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA_DEVI_MHZ:	//���Ƶƫ_MHz
		case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA_DEVI_KHZ:	//���Ƶƫ_kHz
		case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA_DEVI_HZ:	//���Ƶƫ_Hz
			if (sysData.digital.digitaltype == dt2FSK || sysData.digital.digitaltype == dt4FSK || sysData.digital.digitaltype == dt8FSK)
				key->isEnabled = true;
			else
				key->isEnabled = false;
			if (key->isEnabled)
				p.setPen(BUTTONENABLECOLOR);
			else
				p.setPen(BUTTONDISABLECOLOR);
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA_PHASE:	//��λƫ��
			if (sysData.digital.digitaltype == dt2PSK || sysData.digital.digitaltype == dt4PSK || sysData.digital.digitaltype == dt8PSK)
				key->isEnabled = true;
			else
				key->isEnabled = false;
			if (key->isEnabled)
				p.setPen(BUTTONENABLECOLOR);
			else
				p.setPen(BUTTONDISABLECOLOR);
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA_PHASE_DEG:	//��λƫ��_��
			if (sysData.digital.digitaltype == dt2PSK || sysData.digital.digitaltype == dt4PSK || sysData.digital.digitaltype == dt8PSK)
				key->isEnabled = true;
			else
				key->isEnabled = false;
			if (key->isEnabled)
				p.setPen(BUTTONENABLECOLOR);
			else
				p.setPen(BUTTONDISABLECOLOR);
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA_RATE:		//��������
			key->isEnabled = sysData.digital.digitalstate;
			if (key->isEnabled)
				p.setPen(BUTTONENABLECOLOR);
			else
				p.setPen(BUTTONDISABLECOLOR);
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA_RATE_MHZ:	//��������_MHz
		case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA_RATE_KHZ:	//��������_kHz
		case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA_RATE_HZ:	//��������_Hz
			key->isEnabled = sysData.digital.digitalstate;
			if (key->isEnabled)
				p.setPen(BUTTONENABLECOLOR);
			else
				p.setPen(BUTTONDISABLECOLOR);
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_DIGITALMOD_SEQUENCE:	//��������
			key->isEnabled = sysData.digital.digitalstate;
			if (key->isEnabled)
				p.setPen(BUTTONENABLECOLOR);
			else
				p.setPen(BUTTONDISABLECOLOR);
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.digital.source == dstExternal)	//�ⲿ
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("EXT"));
			rc = QRect(rc.right(), rc.top(), rc.width(), rc.height());
			if (sysData.digital.source == dstInternal)	//�ڲ�
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("INT"));
			break;
		case KEY_FUNC_AUXFCTN_DIGITALMOD_EXTLOAD:	//�ⲿװ��
			key->isEnabled = sysData.digital.digitalstate;
			if (key->isEnabled)
				p.setPen(BUTTONENABLECOLOR);
			else
				p.setPen(BUTTONDISABLECOLOR);
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_DIGITALMOD_EXTLOAD_ENTER:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		//==================================================================================================================
		//======================================================= USB���ʼ� ================================================
		//==================================================================================================================
		case KEY_FUNC_AUXFCTN_USBPOWER:	//���ʼ�
			rc = QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			rc = QRect(rc.right() + sep, (height - pngSubSize) / 2, pngSubSize, pngSubSize);
			p.drawPixmap(rc, GetImgRight(pngSubSize, pngSubSize, arrowBackColor, arrowFillColor));
			break;
		case KEY_FUNC_AUXFCTN_USBPOWER_STATE:	//���ʼ�״̬
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.usbPower.state)	//����
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("On"));
			rc = QRect(rc.right(), rc.top(), rc.width(), rc.height());
			if (!sysData.usbPower.state)	//�ر�
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("Off"));
			break;
		case KEY_FUNC_AUXFCTN_USBPOWER_CHFREQ:	//ͨ��Ƶ��
			key->isEnabled = sysData.usbPower.state;
			if (key->isEnabled)
				p.setPen(BUTTONENABLECOLOR);
			else
				p.setPen(BUTTONDISABLECOLOR);
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_USBPOWER_CHFREQ_GHZ:	//ͨ��Ƶ��_GHz
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_USBPOWER_CHFREQ_MHZ:	//ͨ��Ƶ��_MHz
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_USBPOWER_CHFREQ_KHZ:	//ͨ��Ƶ��_kHz
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_USBPOWER_CHFREQ_HZ:	//ͨ��Ƶ��_Hz
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_USBPOWER_AMPLOFS:	//����ƫ��
			key->isEnabled = sysData.usbPower.state;
			if (key->isEnabled)
				p.setPen(BUTTONENABLECOLOR);
			else
				p.setPen(BUTTONDISABLECOLOR);
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_USBPOWER_AMPLOFS_DB:	//����ƫ��_dB
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_USBPOWER_AVERAGE:	//ƽ��
			key->isEnabled = sysData.usbPower.state;
			if (key->isEnabled)
				p.setPen(BUTTONENABLECOLOR);
			else
				p.setPen(BUTTONDISABLECOLOR);
			rc = QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			rc = QRect(rc.right() + sep, (height - pngSubSize) / 2, pngSubSize, pngSubSize);
			p.drawPixmap(rc, GetImgRight(pngSubSize, pngSubSize, arrowBackColor, arrowFillColor));
			break;
		case KEY_FUNC_AUXFCTN_USBPOWER_AVERAGE_MODE:	//ƽ��ģʽ
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.usbPower.AdvMode == upamtAuto)	//�Զ�
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("Auto"));
			rc = QRect(rc.right(), rc.top(), rc.width(), rc.height());
			if (sysData.usbPower.AdvMode == upamtOff)	//�ر�
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("Off"));
			break;
		case KEY_FUNC_AUXFCTN_USBPOWER_AVERAGE_COUNT:	//ƽ������
			key->isEnabled = sysData.usbPower.state;
			if (key->isEnabled)
				p.setPen(BUTTONENABLECOLOR);
			else
				p.setPen(BUTTONDISABLECOLOR);
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_USBPOWER_AVERAGE_COUNT_ENTER:	//ƽ������
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_USBPOWER_UNIT:	//������λ
			key->isEnabled = sysData.usbPower.state;
			if(key->isEnabled)
				p.setPen(BUTTONENABLECOLOR);
			else
				p.setPen(BUTTONDISABLECOLOR);
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.usbPower.MeasUnit == upmutW)	//W
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, "W");
			rc = QRect(rc.right(), rc.top(), rc.width(), rc.height());
			if (sysData.usbPower.MeasUnit == upmutdBm)	//�ر�
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, "dBm");
			break;
		//==================================================================================================================
		//======================================================= System ===================================================
		//==================================================================================================================
		case KEY_FUNC_SYSTEM_LAN:		//LAN
			rc = QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			rc = QRect(rc.right() + sep, (height - pngSubSize) / 2, pngSubSize, pngSubSize);
			p.drawPixmap(rc, GetImgRight(pngSubSize, pngSubSize, arrowBackColor, arrowFillColor));
			break;
		case KEY_FUNC_SYSTEM_LAN_IP:	//LAN_IP
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_LAN_IP_ENTER:	
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_LAN_MASK:	//��������
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_LAN_MASK_ENTER:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_LAN_GATEWAY:	//����
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_LAN_GATEWAY_ENTER:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_SCPI:		//Զ��״̬�Ƿ�ˢ��
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.system.isScpiFresh)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("On"));
			rc = QRect(rc.right(), rc.top(), rc.width(), rc.height());
			if (!sysData.system.isScpiFresh)	//�ر�
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("Off"));
			break;
		case KEY_FUNC_SYSTEM_LANGUAGE:		//����
			rc = QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			rc = QRect(rc.right() + sep, (height - pngSubSize) / 2, pngSubSize, pngSubSize);
			p.drawPixmap(rc, GetImgRight(pngSubSize, pngSubSize, arrowBackColor, arrowFillColor));
			break;
		case KEY_FUNC_SYSTEM_LANGUAGE_CHINESE:	//����
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_LANGUAGE_ENGLISH:	//Ӣ��
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_SERVICE:		//����
			rc = QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			rc = QRect(rc.right() + sep, (height - pngSubSize) / 2, pngSubSize, pngSubSize);
			p.drawPixmap(rc, GetImgRight(pngSubSize, pngSubSize, arrowBackColor, arrowFillColor));
			break;
		case KEY_FUNC_SYSTEM_SERVICE_EVALUATE:	//����
			rc = QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			rc = QRect(rc.right() + sep, (height - pngSubSize) / 2, pngSubSize, pngSubSize);
			p.drawPixmap(rc, GetImgRight(pngSubSize, pngSubSize, arrowBackColor, arrowFillColor));
			break;
		case KEY_FUNC_SYSTEM_SERVICE_EVALUATE_ON:	//���ÿ�
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_SERVICE_EVALUATE_OFF:	//���ù�
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_SERVICE_LANMACADDR:	//������ַ
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		//case KEY_FUNC_AUX_LANGUAGE:
		//case KEY_FUNC_AUX_SCPI:
		//	rc = QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows);
		//	p.drawText(rc, Qt::AlignHCenter | Qt::AlignVCenter, keyName);
		//	rc = QRect(rc.right() + sep, (height - pngSubSize) / 2, pngSubSize, pngSubSize);
		//	p.drawPixmap(rc, GetImgRight(pngSubSize, pngSubSize, arrowBackColor, arrowFillColor));
		//	break;
		//case KEY_FUNC_AUX_LAN_IP:
		//case KEY_FUNC_AUX_LAN_IP_ENTER:
		//case KEY_FUNC_AUX_LAN_MASK:
		//case KEY_FUNC_AUX_LAN_MASK_ENTER:
		//case KEY_FUNC_AUX_LAN_GATEWAY:
		//case KEY_FUNC_AUX_SCPI_OFF:
		//case KEY_FUNC_AUX_SCPI_ON:
		//case KEY_FUNC_AUX_LAN_GATEWAY_ENTER:
		//case KEY_FUNC_AUX_LANGUAGE_CHINESE:
		//case KEY_FUNC_AUX_LANGUAGE_ENGLISH:
		//	p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
		//	break;
		//case KEY_FUNC_AUX_LAN_RETURN:
		//case KEY_FUNC_AUX_LANGUAGE_RETURN:
		//case KEY_FUNC_AUX_SCPI_RETURN:
		//	p.drawPixmap(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), img_OUT);
		//	break;
		//	//ϵͳ����
		//case KEY_FUNC_AUX_SERVICE:
		//case KEY_FUNC_AUX_SERVICE_EVALUATE:
		//	p.drawText(QRect(sep, sep + 4, width - 2 * sep, (height - 2 * sep) / 2), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
		//	p.drawPixmap(QRect(width / 4, sep + 4 + (height - 2 * sep) / 2, (width - 2 * sep) / 2, (height - 2 * sep) / 4), img_IN);
		//	break;
		//case KEY_FUNC_AUX_SERVICE_EVALUATE_ON:
		//case KEY_FUNC_AUX_SERVICE_EVALUATE_OFF:
		//case KEY_FUNC_AUX_SERVICE_LANMACADDR:
		//case KEY_FUNC_AUX_SERVICE_LANMACADDR_ENTER:
		//	p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
		//	break;
		//case KEY_FUNC_AUX_SERVICE_RETURN:
		//case KEY_FUNC_AUX_SERVICE_EVALUATE_RETURN:
		//	p.drawPixmap(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), img_OUT);
		//	break;
		//
		default:
			break;
	}

	
	

	p.end();
}

//��ȡ���Ҽ�ͷ��ͼ��
QPixmap mainForm::GetImgRight(int width, int height, QColor backColor, QColor fillColor)
{
	QPixmap map(width, height);
	QPainter p;
	p.begin(&map);
	p.fillRect(map.rect(), backColor);
	int sep = 2;
	QPointF points[] = {
		QPointF(sep, sep),
		QPointF(width - sep, height / 2),
		QPointF(sep, height - sep)
	};
	p.setBrush(fillColor);
	p.drawPolygon(points, 3);
	p.end();
	return map;
}

//��ȡ�����ͷ��ͼ��
QPixmap mainForm::GetImgLeft(int width, int height, QColor backColor, QColor fillColor)
{
	QPixmap map(width, height);
	QPainter p;
	p.begin(&map);
	p.fillRect(map.rect(), backColor);
	int sep = 2;
	QPointF points[] = {
		QPointF(sep, height / 2),
		QPointF(width - sep, sep),
		QPointF(width - sep, height - sep)
	};
	p.setBrush(fillColor);
	p.drawPolygon(points, 3);
	p.end();
	return map;
}