#include "mainform.h"

//绘制按钮菜单
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
	QColor arrowBackColor;//箭头的背景
	QColor arrowFillColor = QColor(0, 0, 0);;//填充箭头的颜色
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
		p.drawRect(pix->rect().x(), pix->rect().y(), pix->rect().width(), pix->rect().height() - 1);	//绘制矩形框
	}

	if (!key->isEnabled)
	{
		p.setPen(BUTTONDISABLECOLOR);
		p.fillRect(pix->rect(), BUTTONUNSELECTCOLOR);
		if (isNeedDrawButtonRect)
		{
			p.setPen(QColor(0, 0, 0));
			p.drawRect(pix->rect().x(), pix->rect().y(), pix->rect().width(), pix->rect().height() - 1);	//绘制矩形框
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
	//  index = keyName.indexOf("→",0);

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
		//====================================================== 频率 ======================================================
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
		case KEY_FUNC_FREQREFSTATE:		//参考状态
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
		case KEY_FUNC_FREQREF10M:		//参考时基
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
		//=================================================== 幅度 =========================================================
		//==================================================================================================================
		case KEY_FUNC_AMPL:		//幅度设置
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
		case KEY_FUNC_AMPLREFSET:	//幅度参考设置
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AMPLREFSTATE:	//幅度参考状态
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
		case KEY_FUNC_AMPLOFFSET:	//幅度偏置设置
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AMPLOFFSET_DBM:		
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AMPLRESPONSE:	//稳幅响应
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
		case KEY_FUNC_AMPL_ALC_STAT:	//稳幅状态
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
		case KEY_FUNC_AMPL_CABLE:		//缆损
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AMPL_CABLE_DB:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		//==================================================================================================================
		//==================================================== 增量设置 ====================================================
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
		//====================================================== 扫描 ======================================================
		//==================================================================================================================
		case KEY_FUNC_SWEEP_MODE:		//扫描模式
			rc = QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			rc = QRect(rc.right() + sep, (height - pngSubSize) / 2, pngSubSize, pngSubSize);
			p.drawPixmap(rc, GetImgRight(pngSubSize, pngSubSize, arrowBackColor, arrowFillColor));
			break;
		case KEY_FUNC_SWEEP_MODE_OFF:	//扫描模式——关闭
		case KEY_FUNC_SWEEP_MODE_FREQ:	//扫描模式——频率
		case KEY_FUNC_SWEEP_MODE_AMPL:	//扫描模式——幅度
		case KEY_FUNC_SWEEP_MODE_FREQAMPL:	//扫描模式——频率+幅度
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SWEEP_TYPE:		//扫描类型
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
		case KEY_FUNC_SWEEP_REPEAT:		//重复扫描
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
		case KEY_FUNC_SWEEP_CONFIGSTEP:	//步进设置
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
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST:	//列表设置
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
		case KEY_FUNC_SWEEP_MORE_SWEEPTRIGGER:	//扫描触发
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
		case KEY_FUNC_SWEEP_MORE_SWEEPDIRECTION:	//扫描方向
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
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("Down"));	//反向
			rc = QRect(rc.right(), rc.top(), rc.width(), rc.height());
			if (sysData.sweep.direction == sdUp)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("Up"));		//正向
			break;
		case KEY_FUNC_SWEEP_MORE_SWEEPRETRACE:		//扫描折返
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
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("On"));	//关闭
			rc = QRect(rc.right(), rc.top(), rc.width(), rc.height());
			if (sysData.sweep.retrace == srOff)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("Off"));		//开启
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
		case KEY_FUNC_AM_STATE:		//AM状态
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
		case KEY_FUNC_AM_DEPTH:		//AM调制深度
			p.drawText(QRect(sep, sep + 4, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AM_DEPTH_PERCENT:	//AM调制深度
			p.drawText(QRect(sep, sep + 4, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AM_SOURCE:	//调制源
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.am.source == true)	//外部
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("EXT"));
			rc = QRect(rc.right(), rc.top(), rc.width(), rc.height());
			if (sysData.am.source == false)	//内部
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("INT"));
			break;
		case KEY_FUNC_AM_INTERNALFCTN:	//内部函数建立
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
		case KEY_FUNC_AM_INTERNALFCTN_WAVE:	//内部函数建立_波形
			rc = QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			rc = QRect(rc.right() + sep, (height - pngSubSize) / 2, pngSubSize, pngSubSize);
			p.drawPixmap(rc, GetImgRight(pngSubSize, pngSubSize, arrowBackColor, arrowFillColor));
			break;
		case KEY_FUNC_AM_INTERNALFCTN_WAVE_SINE://内部函数建立_波形_正弦波
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AM_INTERNALFCTN_WAVE_TRIANGLE://内部函数建立_波形_三角波
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AM_INTERNALFCTN_WAVE_SQUARE://内部函数建立_波形_方波
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AM_INTERNALFCTN_WAVE_SAWTOOTH://内部函数建立_波形_锯齿波
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AM_INTERNALFCTN_FREQ://内部函数建立_频率
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AM_INTERNALFCTN_FREQ_MHZ://内部函数建立_频率
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AM_INTERNALFCTN_FREQ_KHZ://内部函数建立_频率
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AM_INTERNALFCTN_FREQ_HZ://内部函数建立_频率
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AM_COUPLING:	//深度耦合
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
		case KEY_FUNC_FMPM_MODE:	//调制方式
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.fmpm.mode == fdtPM)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("φM"));
			rc = QRect(rc.right(), rc.top(), rc.width(), rc.height());
			if (sysData.fmpm.mode == fdtFM)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("FM"));
			break;
		case KEY_FUNC_FMPM_STATUS:	//调制状态
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			if (sysData.fmpm.mode == fdtPM)
			{
				p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("φM State"));
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
		case KEY_FUNC_FMPM_OFFSET:		//调制偏移
			if (sysData.fmpm.mode == fdtFM)
			{
				p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, getTheTranslatorString("Freq Offset"));
			}
			else if(sysData.fmpm.mode == fdtPM)
			{
				p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, getTheTranslatorString("Phase Offset"));
			}
			break;
		case KEY_FUNC_FMPM_OFFSET_MHZ:	//调制偏移 MHz | °
			if (sysData.fmpm.mode == fdtFM)
			{
				p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, "MHz");
			}
			else if (sysData.fmpm.mode == fdtPM)
			{
				p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, "°");
			}
			break;
		case KEY_FUNC_FMPM_OFFSET_KHZ:	//调制偏移 kHz
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
		case KEY_FUNC_FMPM_OFFSET_HZ:	//调制偏移 Hz
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
		case KEY_FUNC_FMPM_SOURCE:	//调制源
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.fmpm.mode == fdtFM)
			{
				if (sysData.fmpm.fm.source == true)	//外部
					p.setFont(fontUnderline);
				else
					p.setFont(fontLight);
				p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("EXT"));
				rc = QRect(rc.right(), rc.top(), rc.width(), rc.height());
				if (sysData.fmpm.fm.source == false)	//内部
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
				if (sysData.fmpm.pm.source == false)	//内部
					p.setFont(fontUnderline);
				else
					p.setFont(fontLight);
				p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("INT"));
			}
			break;
		case KEY_FUNC_FMPM_INTERNALFCTN:	//内部函数
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
		case KEY_FUNC_FMPM_INTERNALFCTN_WAVE:	//内部函数建立_波形
			rc = QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignCenter, keyName);
			rc = QRect(rc.right() + sep, (height - pngSubSize) / 2, pngSubSize, pngSubSize);
			p.drawPixmap(rc, GetImgRight(pngSubSize, pngSubSize, arrowBackColor, arrowFillColor));
			break;
		case KEY_FUNC_FMPM_INTERNALFCTN_WAVE_SINE://内部函数建立_波形_正弦波
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FMPM_INTERNALFCTN_WAVE_TRIANGLE://内部函数建立_波形_三角波
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FMPM_INTERNALFCTN_WAVE_SQUARE://内部函数建立_波形_方波
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FMPM_INTERNALFCTN_WAVE_SAWTOOTH://内部函数建立_波形_锯齿波
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FMPM_INTERNALFCTN_FREQ:	//调制速率
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FMPM_INTERNALFCTN_FREQ_MHZ://调制速率_MHz
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FMPM_INTERNALFCTN_FREQ_KHZ://调制速率_kHz
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FMPM_INTERNALFCTN_FREQ_HZ://调制速率_Hz
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FMPM_COUPLING:	//调制偏移耦合
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
				p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("φM Dev Couple"));
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
		case KEY_FUNC_PULSE_STATE:	//调制状态
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
		case KEY_FUNC_PULSE_TRIGGERWAY:	//触发方式
			rc = QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			rc = QRect(rc.right() + sep, (height - pngSubSize) / 2, pngSubSize, pngSubSize);
			p.drawPixmap(rc, GetImgRight(pngSubSize, pngSubSize, arrowBackColor, arrowFillColor));
			break;
		case KEY_FUNC_PULSE_TRIGGERWAY_FREERUN:	//触发方式_自由运行
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_PULSE_TRIGGERWAY_KEYTRIG:	//触发方式_按键触发
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
		case KEY_FUNC_PULSE_TRIGGERWAY_EXTTRIG:	//触发方式_外部触发
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
		case KEY_FUNC_PULSE_SOURCE:	//脉冲源
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.pulse.pulsesource == psExternal)	//外部
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("EXT"));
			rc = QRect(rc.right(), rc.top(), rc.width(), rc.height());
			if (sysData.pulse.pulsesource == psInternal)	//内部
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("INT"));
			break;
		case KEY_FUNC_PULSE_PERIOD:	//脉冲周期
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_PULSE_PERIOD_S:	//脉冲周期_S
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_PULSE_PERIOD_MS:	//脉冲周期_mS
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_PULSE_PERIOD_US:	//脉冲周期_uS
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_PULSE_PERIOD_NS:	//脉冲周期_nS
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_PULSE_WIDTH:	//脉冲宽度
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_PULSE_WIDTH_S:	//脉冲宽度_S
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_PULSE_WIDTH_MS:	//脉冲宽度_mS
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_PULSE_WIDTH_US:	//脉冲宽度_uS
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_PULSE_WIDTH_NS:	//脉冲宽度_uS
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_PULSE_DELAY:		//脉冲延时
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_PULSE_DELAY_S:	//脉冲延时_S
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_PULSE_DELAY_MS:	//脉冲延时_mS
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_PULSE_DELAY_US:	//脉冲延时_uS
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_PULSE_DELAY_NS:	//脉冲延时_nS
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_PULSE_EXTPOLA:	//外部触发信号极性
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
			//====================================================== AUX 上变频 ================================================
			//==================================================================================================================
		case KEY_FUNC_AUXFCTN_EXTRF:	//上变频
			key->isEnabled = sysData.freq.freq > 300e6; //设置频率小于300MHz时，上变频功能不能用
			if (key->isEnabled)
				p.setPen(BUTTONENABLECOLOR);
			else
				p.setPen(BUTTONDISABLECOLOR);
			rc = QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			rc = QRect(rc.right() + sep, (height - pngSubSize) / 2, pngSubSize, pngSubSize);
			p.drawPixmap(rc, GetImgRight(pngSubSize, pngSubSize, arrowBackColor, arrowFillColor));
			break;
		case KEY_FUNC_AUXFCTN_EXTRF_SOURCE:	//变频源
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.exfRef.type == erftExternal)	//外部
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("EXT"));
			rc = QRect(rc.right(), rc.top(), rc.width(), rc.height());
			if (sysData.exfRef.type == erftInternal)	//内部
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("INT"));
			break;
		case KEY_FUNC_AUXFCTN_EXTRF_EXTIF:	//外部中频频率
			key->isEnabled = sysData.exfRef.type == erftExternal;
			if (!key->isEnabled)
				p.setPen(BUTTONDISABLECOLOR);
			else
				p.setPen(BUTTONENABLECOLOR);
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_EXTRF_EXTIF_MHZ:	//外部中频频率_MHZ
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_EXTRF_EXTIF_KHZ:	//外部中频频率_KHZ
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_EXTRF_EXTIF_HZ:	//外部中频频率_HZ
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
			//==================================================================================================================
			//====================================================== AUX 低频输出 ==============================================
			//==================================================================================================================
		case KEY_FUNC_AUXFCTN_LFOUT:	//低频输出
			rc = QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			rc = QRect(rc.right() + sep, (height - pngSubSize) / 2, pngSubSize, pngSubSize);
			p.drawPixmap(rc, GetImgRight(pngSubSize, pngSubSize, arrowBackColor, arrowFillColor));
			break;
		case KEY_FUNC_AUXFCTN_LFOUT_WAVE:	//波形
			rc = QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			rc = QRect(rc.right() + sep, (height - pngSubSize) / 2, pngSubSize, pngSubSize);
			p.drawPixmap(rc, GetImgRight(pngSubSize, pngSubSize, arrowBackColor, arrowFillColor));
			break;
		case KEY_FUNC_AUXFCTN_LFOUT_WAVE_SINE:	//正弦波
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_LFOUT_WAVE_SQUARE:	//方波
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_LFOUT_WAVE_TRIANGLE:	//三角波
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_LFOUT_WAVE_SAWTOOTH:	//锯齿波
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_LFOUT_FREQ:	//频率
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_LFOUT_FREQ_KHZ:	//频率_kHz
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_LFOUT_FREQ_HZ:	//频率_Hz
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_LFOUT_AMPL:	//幅度
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_LFOUT_AMPL_MV:	//幅度mV
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_LFOUT_AMPL_V:	//幅度mV
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
		case KEY_FUNC_AUXFCTN_DRG_STATE:	//状态
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.drg.state)	//开启
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("On"));
			rc = QRect(rc.right(), rc.top(), rc.width(), rc.height());
			if (!sysData.drg.state)	//关闭
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("Off"));
			break;
		case KEY_FUNC_AUXFCTN_DRG_UPPER:	//扫描上限
			key->isEnabled = sysData.drg.state;
			if (key->isEnabled)
				p.setPen(BUTTONENABLECOLOR);
			else
				p.setPen(BUTTONDISABLECOLOR);
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_DRG_UPPER_GHZ:	//扫描上限_GHz
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_DRG_UPPER_MHZ:	//扫描上限_MHz
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_DRG_UPPER_KHZ:	//扫描上限_kHz
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_DRG_UPPER_HZ:		//扫描上限_Hz
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_DRG_LOW:	//扫描下限
			key->isEnabled = sysData.drg.state;
			if (key->isEnabled)
				p.setPen(BUTTONENABLECOLOR);
			else
				p.setPen(BUTTONDISABLECOLOR);
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_DRG_LOW_GHZ:	//扫描下限_GHz
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_DRG_LOW_MHZ:	//扫描下限_MHz
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_DRG_LOW_KHZ:	//扫描下限_kHz
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_DRG_LOW_HZ:	//扫描下限_Hz
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_DRG_POINTS:	//扫描点数
			key->isEnabled = sysData.drg.state;
			if (key->isEnabled)
				p.setPen(BUTTONENABLECOLOR);
			else
				p.setPen(BUTTONDISABLECOLOR);
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_DRG_POINTS_ENTER:	//扫描点数
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_DRG_RATE:	//扫描速率
			key->isEnabled = sysData.drg.state;
			if (key->isEnabled)
				p.setPen(BUTTONENABLECOLOR);
			else
				p.setPen(BUTTONDISABLECOLOR);
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_DRG_RATE_NS:	//扫描速率_ns
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_DRG_RATE_US:	//扫描速率_us
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_DRG_MODE:	//工作模式
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
		case KEY_FUNC_AUXFCTN_DRG_MODE_BOTHWAY:	//双向扫描
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_DRG_MODE_POSSINGLE:	//正向单路
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_DRG_MODE_NEGSINGLE:	//负向单路
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		//==================================================================================================================
		//======================================================= AUX 数字调制 =============================================
		//==================================================================================================================
		case KEY_FUNC_AUXFCTN_DIGITALMOD:		//数字调制，该功能只有SG1060A有
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
		case KEY_FUNC_AUXFCTN_DIGITALMOD_STATE:	//状态
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.digital.digitalstate)	//开启
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("On"));
			rc = QRect(rc.right(), rc.top(), rc.width(), rc.height());
			if (!sysData.digital.digitalstate)	//关闭
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("Off"));
			break;
		case KEY_FUNC_AUXFCTN_DIGITALMOD_MODE:	//调制模式
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
		case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA:	//调制参数
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
		case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA_DEPTH:	//调制深度
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
		case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA_DEPTH_PERCENT:	//调制深度_%
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
		case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA_DEVI:	//最大频偏
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
		case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA_DEVI_MHZ:	//最大频偏_MHz
		case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA_DEVI_KHZ:	//最大频偏_kHz
		case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA_DEVI_HZ:	//最大频偏_Hz
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
		case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA_PHASE:	//相位偏移
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
		case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA_PHASE_DEG:	//相位偏移_°
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
		case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA_RATE:		//调制速率
			key->isEnabled = sysData.digital.digitalstate;
			if (key->isEnabled)
				p.setPen(BUTTONENABLECOLOR);
			else
				p.setPen(BUTTONDISABLECOLOR);
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA_RATE_MHZ:	//调制速率_MHz
		case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA_RATE_KHZ:	//调制速率_kHz
		case KEY_FUNC_AUXFCTN_DIGITALMOD_PARA_RATE_HZ:	//调制速率_Hz
			key->isEnabled = sysData.digital.digitalstate;
			if (key->isEnabled)
				p.setPen(BUTTONENABLECOLOR);
			else
				p.setPen(BUTTONDISABLECOLOR);
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_DIGITALMOD_SEQUENCE:	//调制序列
			key->isEnabled = sysData.digital.digitalstate;
			if (key->isEnabled)
				p.setPen(BUTTONENABLECOLOR);
			else
				p.setPen(BUTTONDISABLECOLOR);
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.digital.source == dstExternal)	//外部
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("EXT"));
			rc = QRect(rc.right(), rc.top(), rc.width(), rc.height());
			if (sysData.digital.source == dstInternal)	//内部
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("INT"));
			break;
		case KEY_FUNC_AUXFCTN_DIGITALMOD_EXTLOAD:	//外部装载
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
		//======================================================= USB功率计 ================================================
		//==================================================================================================================
		case KEY_FUNC_AUXFCTN_USBPOWER:	//功率计
			rc = QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			rc = QRect(rc.right() + sep, (height - pngSubSize) / 2, pngSubSize, pngSubSize);
			p.drawPixmap(rc, GetImgRight(pngSubSize, pngSubSize, arrowBackColor, arrowFillColor));
			break;
		case KEY_FUNC_AUXFCTN_USBPOWER_STATE:	//功率计状态
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.usbPower.state)	//开启
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("On"));
			rc = QRect(rc.right(), rc.top(), rc.width(), rc.height());
			if (!sysData.usbPower.state)	//关闭
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("Off"));
			break;
		case KEY_FUNC_AUXFCTN_USBPOWER_CHFREQ:	//通道频率
			key->isEnabled = sysData.usbPower.state;
			if (key->isEnabled)
				p.setPen(BUTTONENABLECOLOR);
			else
				p.setPen(BUTTONDISABLECOLOR);
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_USBPOWER_CHFREQ_GHZ:	//通道频率_GHz
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_USBPOWER_CHFREQ_MHZ:	//通道频率_MHz
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_USBPOWER_CHFREQ_KHZ:	//通道频率_kHz
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_USBPOWER_CHFREQ_HZ:	//通道频率_Hz
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_USBPOWER_AMPLOFS:	//功率偏移
			key->isEnabled = sysData.usbPower.state;
			if (key->isEnabled)
				p.setPen(BUTTONENABLECOLOR);
			else
				p.setPen(BUTTONDISABLECOLOR);
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_USBPOWER_AMPLOFS_DB:	//功率偏移_dB
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_USBPOWER_AVERAGE:	//平均
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
		case KEY_FUNC_AUXFCTN_USBPOWER_AVERAGE_MODE:	//平均模式
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.usbPower.AdvMode == upamtAuto)	//自动
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("Auto"));
			rc = QRect(rc.right(), rc.top(), rc.width(), rc.height());
			if (sysData.usbPower.AdvMode == upamtOff)	//关闭
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("Off"));
			break;
		case KEY_FUNC_AUXFCTN_USBPOWER_AVERAGE_COUNT:	//平均次数
			key->isEnabled = sysData.usbPower.state;
			if (key->isEnabled)
				p.setPen(BUTTONENABLECOLOR);
			else
				p.setPen(BUTTONDISABLECOLOR);
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_USBPOWER_AVERAGE_COUNT_ENTER:	//平均次数
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_AUXFCTN_USBPOWER_UNIT:	//测量单位
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
			if (sysData.usbPower.MeasUnit == upmutdBm)	//关闭
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
		case KEY_FUNC_SYSTEM_LAN_MASK:	//子网掩码
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_LAN_MASK_ENTER:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_LAN_GATEWAY:	//网关
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_LAN_GATEWAY_ENTER:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_SCPI:		//远程状态是否刷新
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
			if (!sysData.system.isScpiFresh)	//关闭
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignCenter, getTheTranslatorString("Off"));
			break;
		case KEY_FUNC_SYSTEM_LANGUAGE:		//语言
			rc = QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			rc = QRect(rc.right() + sep, (height - pngSubSize) / 2, pngSubSize, pngSubSize);
			p.drawPixmap(rc, GetImgRight(pngSubSize, pngSubSize, arrowBackColor, arrowFillColor));
			break;
		case KEY_FUNC_SYSTEM_LANGUAGE_CHINESE:	//中文
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_LANGUAGE_ENGLISH:	//英文
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_SERVICE:		//服务
			rc = QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			rc = QRect(rc.right() + sep, (height - pngSubSize) / 2, pngSubSize, pngSubSize);
			p.drawPixmap(rc, GetImgRight(pngSubSize, pngSubSize, arrowBackColor, arrowFillColor));
			break;
		case KEY_FUNC_SYSTEM_SERVICE_EVALUATE:	//试用
			rc = QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			rc = QRect(rc.right() + sep, (height - pngSubSize) / 2, pngSubSize, pngSubSize);
			p.drawPixmap(rc, GetImgRight(pngSubSize, pngSubSize, arrowBackColor, arrowFillColor));
			break;
		case KEY_FUNC_SYSTEM_SERVICE_EVALUATE_ON:	//试用开
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_SERVICE_EVALUATE_OFF:	//试用关
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_SERVICE_LANMACADDR:	//网卡地址
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
		//	//系统服务
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

//获取向右箭头的图像
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

//获取向左箭头的图像
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