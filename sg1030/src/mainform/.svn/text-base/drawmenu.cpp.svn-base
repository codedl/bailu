#include "mainform.h"

//绘制按钮菜单
void mainForm::drawButtonMenu(QPixmap* pix, subKeyDef* key)
{
	if ((pix == NULL) || (pix->isNull() || key == NULL))
	{
		helpContent = "internal error";
		//    drawWin4();
		return;
	}

	QPainter p;
	p.begin(pix);
	if (key == nowSubKey)
	{
		p.setPen(Qt::white);
		p.fillRect(pix->rect(), BUTTONSELECTCOLOR);
	} else
	{
		p.setPen(Qt::black);
		p.fillRect(pix->rect(), BUTTONUNSELECTCOLOR);
	}

	if (!key->isEnabled)
	{
		p.setPen(BUTTONDISABLECOLOR);
		p.fillRect(pix->rect(), BUTTONUNSELECTCOLOR);
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
	int sep = 0;
	int enSep = 2;
	int arrowSep = 2;
	int pngSubSize = 10;
	int width = pix->width();
	int height = pix->height();
	p.setFont(fontBold);

	//	printf("w=%d,h=%d\r\n",(width - 2 * sep),((height - 2 * sep) / rows));
	//	printf("key->funcCode = 0x%x\n",key->funcCode);

	switch (key->funcCode)
	{
		//Freq& Ampl

		case KEY_FUNC_FREQ:
		case KEY_FUNC_FREQREFSET:
		case KEY_FUNC_FREQREFSTATE_OFF:
		case KEY_FUNC_FREQREFSTATE_ON:
		case KEY_FUNC_FREQOFFSET:
		case KEY_FUNC_FREQMULTIPLIER:
		case KEY_FUNC_FREQREF10M_INTERRIOR:
		case KEY_FUNC_FREQREF10M_EXTERNAL:
		case KEY_FUNC_AMPL:
		case KEY_FUNC_AMPLREFSET:
		case KEY_FUNC_AMPLREFSTATE_OFF:
		case KEY_FUNC_AMPLREFSTATE_ON:
			//	case KEY_FUNC_AMPL_ERROR:
		case KEY_FUNC_AMPLOFFSET:
		case KEY_FUNC_AMPLRESPONSE_FAST:
		case KEY_FUNC_AMPLRESPONSE_SLOW:
		case KEY_FUNC__AMPL_ALC_OFF:
		case KEY_FUNC__AMPL_ALC_ON:
		case KEY_FUNC_AMPL_CABLE:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;

		case KEY_FUNC_FREQ_GHZ:
		case KEY_FUNC_FREQ_MHZ:
		case KEY_FUNC_FREQ_KHZ:
		case KEY_FUNC_FREQ_HZ:
		case KEY_FUNC_FREQREFSET_GHZ:
		case KEY_FUNC_FREQREFSET_MHZ:
		case KEY_FUNC_FREQREFSET_KHZ:
		case KEY_FUNC_FREQREFSET_HZ:
		case KEY_FUNC_FREQOFFSET_GHZ:
		case KEY_FUNC_FREQOFFSET_MHZ:
		case KEY_FUNC_FREQOFFSET_KHZ:
		case KEY_FUNC_FREQOFFSET_HZ:
		case KEY_FUNC_FREQMULTIPLIER_ENTER:
			//	case KEY_FUNC_AMPL_ERROR_DBM:
		case KEY_FUNC_AMPLOFFSET_DBM:
		case KEY_FUNC_AMPL_CABLE_DB:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;

		case KEY_FUNC_AMPL_DBM:
			if (sysData.ampl.ref.state)
			{
				keyName = "dB";
			} else
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

		case KEY_FUNC_FREQREFSTATE:
		case KEY_FUNC_FREQREF10M:
		case KEY_FUNC_AMPLREFSTATE:
		case KEY_FUNC_AMPLRESPONSE:
		case KEY_FUNC_AMPL_ALC_STAT:
			//p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / 2), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			p.drawText(QRect(sep, sep + 4, width - 2 * sep, (height - 2 * sep) / 2), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			//	  p.drawPixmap(QRect(sep, sep + (height - 2 * sep) / 2, width - 2 * sep, (height - 2 * sep) / 2), img_IN);
			p.drawPixmap(QRect(width / 4, sep + 4 + (height - 2 * sep) / 2, (width - 2 * sep) / 2, (height - 2 * sep) / 4), img_IN);//zpc changed
			break;

		case KEY_FUNC_FREQREFSTATE_RETURN:
		case KEY_FUNC_FREQREF10M_RETURN:
		case KEY_FUNC_AMPLREFSTATE_RETURN:
		case KEY_FUNC_AMPLRESPONSE_RETURN:
		case KEY_FUNC__AMPL_ALC_RETURN:
			p.drawPixmap(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), img_OUT);
			break;

			//sweep
		case KEY_FUNC_SWEEP_MODE:
			p.drawText(QRect(sep, sep + 4, width - 2 * sep, (height - 2 * sep) / 2), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			p.drawPixmap(QRect(width / 4, sep + 4 + (height - 2 * sep) / 2, (width - 2 * sep) / 2, (height - 2 * sep) / 4), img_IN);//zpc changed
			break;
		case KEY_FUNC_SWEEP_TYPE:
		case KEY_FUNC_SWEEP_REPEAT:
			//	case KEY_FUNC_SWEEP_CONFIGSTEP:
			//	case KEY_FUNC_SWEEP_MORE_CONFIGLIST:
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_MORE_DWELLTYPE:
		case KEY_FUNC_SWEEP_MORE_SWEEPDIRECTION:
		case KEY_FUNC_SWEEP_MORE_SWEEPRETRACE:
		case KEY_FUNC_SWEEP_MORE_SWEEPTRIGGER:
			key->isEnabled = ((sysData.sweep.mode == smAmpl) || (sysData.sweep.mode == smFreq) || (sysData.sweep.mode == smFreqAmpl));
			if (key->isEnabled)
				p.setPen(BUTTONENABLECOLOR);
			else
				p.setPen(BUTTONDISABLECOLOR);
			p.drawText(QRect(sep, sep + 4, width - 2 * sep, (height - 2 * sep) / 2), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			//p.drawPixmap(QRect(sep, sep + (height - 2 * sep) / 2, width - 2 * sep, (height - 2 * sep) / 2), img_IN);
			p.drawPixmap(QRect(width / 4, sep + 4 + (height - 2 * sep) / 2, (width - 2 * sep) / 2, (height - 2 * sep) / 4), img_IN);//zpc changed
			break;
		case KEY_FUNC_SWEEP_CONFIGSTEP:
			key->isEnabled = (sysData.sweep.type == stStep && sysData.sweep.mode != smOff);
			if (key->isEnabled)
				p.setPen(BUTTONENABLECOLOR);
			else
				p.setPen(BUTTONDISABLECOLOR);
			p.drawText(QRect(sep, sep + 4, width - 2 * sep, (height - 2 * sep) / 2), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			p.drawPixmap(QRect(width / 4, sep + 4 + (height - 2 * sep) / 2, (width - 2 * sep) / 2, (height - 2 * sep) / 4), img_IN);//zpc changed
			break;

		case KEY_FUNC_SWEEP_MORE_CONFIGLIST:
			key->isEnabled = (sysData.sweep.type == stList && sysData.sweep.mode != smOff);
			if (key->isEnabled)
				p.setPen(BUTTONENABLECOLOR);
			else
				p.setPen(BUTTONDISABLECOLOR);
			p.drawText(QRect(sep, sep + 4, width - 2 * sep, (height - 2 * sep) / 2), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			p.drawPixmap(QRect(width / 4, sep + 4 + (height - 2 * sep) / 2, (width - 2 * sep) / 2, (height - 2 * sep) / 4), img_IN);//zpc changed
			break;

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
			//case KEY_FUNC_SWEEP_MORE_CONFIGLIST_MORE_GOTOROW_ENTER:
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_GOTOROW_ENTER:
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_MORE_DOWNLOAD_ENTER:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;

		case KEY_FUNC_SWEEP_MODE_OFF:
		case KEY_FUNC_SWEEP_MODE_FREQ:
			//	case KEY_FUNC_SWEEP_MODE_FREQ_OFF:  //zpc add
			//	case KEY_FUNC_SWEEP_MODE_FREQ_ON:   //zpc add
		case KEY_FUNC_SWEEP_MODE_AMPL:
			//	case KEY_FUNC_SWEEP_MODE_AMPL_OFF:  //zpc add
			//	case KEY_FUNC_SWEEP_MODE_AMPL_ON:   //zpc add
		case KEY_FUNC_SWEEP_MODE_FREQAMPL:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SWEEP_TYPE_LIST:
		case KEY_FUNC_SWEEP_TYPE_STEP:
		case KEY_FUNC_SWEEP_REPEAT_SINGLE:
		case KEY_FUNC_SWEEP_REPEAT_CONT:
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_MORE_DOWNLOAD:
		case KEY_FUNC_SWEEP_STEP_MORE_POINTS:
		case KEY_FUNC_SWEEP_STEP_MORE_STEPDWELL:
			//case KEY_FUNC_SWEEP_MORE_CONFIGLIST_SELECTITEM:
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_EDITITEM:
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_INSERTROW:
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_DELETEROW:
			//case KEY_FUNC_SWEEP_MORE_CONFIGLIST_GOTOROW:
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_MORE_LOADSTORE:
			//case KEY_FUNC_SWEEP_MORE_CONFIGLIST_MORE_GOTOROW:
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
			//	case KEY_FUNC_SWEEP_SINGLETRIGGER:
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
			//	case KEY_FUNC_SWEEP_MODE_FREQ_RETURN:  //zpc add
			//	case KEY_FUNC_SWEEP_MODE_AMPL_RETURN:  //zpc add
		case KEY_FUNC_SWEEP_TYPE_RETURN:
		case KEY_FUNC_SWEEP_REPEAT_RETURN:
		case KEY_FUNC_SWEEP_STEP_RETURN:
		case KEY_FUNC_SWEEP_MORE_SWEEPTRIGGER_RETURN:
			p.drawPixmap(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), img_OUT);
			break;

		case KEY_FUNC_SWEEP_MORE_RETURN:
		case KEY_FUNC_SWEEP_STEP_MORE_RETURN:
		case KEY_FUNC_SWEEP_MORE_CONFIGLIST_MORE_RETURN:
			p.drawPixmap(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), img_UP);
			break;

			//analog
		case KEY_FUNC_FCTN_ANALOG:
		case KEY_FUNC_FCTN_ANALOG_AM:
		case KEY_FUNC_FCTN_ANALOG_AM_STATE:
		case KEY_FUNC_FCTN_ANALOG_FM:
		case KEY_FUNC_FCTN_ANALOG_FM_STATE:
		case KEY_FUNC_FCTN_ANALOG_PM:
		case KEY_FUNC_FCTN_ANALOG_PM_STATE:
		case KEY_FUNC_FCTN_ANALOG_SOURCE:
			//case KEY_FUNC_FCTN_DIGITAL_PLUSE:
			//case KEY_FUNC_FCTN_DIGITAL_PLUSE_STATE:
			//p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / 2), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			p.drawText(QRect(sep, sep + 4, width - 2 * sep, (height - 2 * sep) / 2), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			//p.drawPixmap(QRect(sep, sep + (height - 2 * sep) / 2, width - 2 * sep, (height - 2 * sep) / 2), img_IN);
			p.drawPixmap(QRect(width / 4, sep + 4 + (height - 2 * sep) / 2, (width - 2 * sep) / 2, (height - 2 * sep) / 4), img_IN);//zpc changed
			break;

		case KEY_FUNC_FCTN_ANALOG_RETURN:
		case KEY_FUNC_FCTN_ANALOG_AM_STATE_RETURN:
		case KEY_FUNC_FCTN_ANALOG_AM_RETURN:
		case KEY_FUNC_FCTN_ANALOG_FM_STATE_RETURN:
		case KEY_FUNC_FCTN_ANALOG_FM_RETURN:
		case KEY_FUNC_FCTN_ANALOG_PM_STATE_RETURN:
		case KEY_FUNC_FCTN_ANALOG_PM_RETURN:
		case KEY_FUNC_FCTN_ANALOG_SOURCE_RETURN:
			p.drawPixmap(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), img_OUT);
			break;

		case KEY_FUNC_FCTN_ANALOG_SOURCE_INTERNAL:
		case KEY_FUNC_FCTN_ANALOG_SOURCE_EXTERNAL:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;

		case KEY_FUNC_FCTN_ANALOG_AM_STATE_ON:
		case KEY_FUNC_FCTN_ANALOG_AM_STATE_OFF:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;

		case KEY_FUNC_FCTN_ANALOG_AM_DEPTH:
		case KEY_FUNC_FCTN_ANALOG_AM_DEPTH_PERCENT:
			key->isEnabled = sysData.analog.am.state;
			if (key->isEnabled)
				p.setPen(BUTTONENABLECOLOR);
			else
				p.setPen(BUTTONDISABLECOLOR);
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FCTN_ANALOG_AM_RATE:
		case KEY_FUNC_FCTN_ANALOG_AM_RATE_KHZ:
		case KEY_FUNC_FCTN_ANALOG_AM_RATE_HZ:
			key->isEnabled = sysData.analog.am.state && (sysData.analog.source == astInternal);
			if (key->isEnabled)
				p.setPen(BUTTONENABLECOLOR);
			else
				p.setPen(BUTTONDISABLECOLOR);
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;

		case KEY_FUNC_FCTN_ANALOG_FM_STATE_ON:
		case KEY_FUNC_FCTN_ANALOG_FM_STATE_OFF:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;

		case KEY_FUNC_FCTN_ANALOG_FM_DEVIATION:
		case KEY_FUNC_FCTN_ANALOG_FM_DEVIATION_MHZ:
		case KEY_FUNC_FCTN_ANALOG_FM_DEVIATION_KHZ:
		case KEY_FUNC_FCTN_ANALOG_FM_DEVIATION_HZ:
			key->isEnabled = sysData.analog.fm.state;
			if (key->isEnabled)
				p.setPen(BUTTONENABLECOLOR);
			else
				p.setPen(BUTTONDISABLECOLOR);
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FCTN_ANALOG_FM_RATE:
		case KEY_FUNC_FCTN_ANALOG_FM_RATE_KHZ:
		case KEY_FUNC_FCTN_ANALOG_FM_RATE_HZ:
			key->isEnabled = sysData.analog.fm.state && (sysData.analog.source == astInternal);
			if (key->isEnabled)
				p.setPen(BUTTONENABLECOLOR);
			else
				p.setPen(BUTTONDISABLECOLOR);
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;

		case KEY_FUNC_FCTN_ANALOG_PM_STATE_ON:
		case KEY_FUNC_FCTN_ANALOG_PM_STATE_OFF:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;

		case KEY_FUNC_FCTN_ANALOG_PM_PHASE:
		case KEY_FUNC_FCTN_ANALOG_PM_PHASE_ANGLE:
			key->isEnabled = sysData.analog.pm.state;
			if (key->isEnabled)
				p.setPen(BUTTONENABLECOLOR);
			else
				p.setPen(BUTTONDISABLECOLOR);
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FCTN_ANALOG_PM_RATE:
		case KEY_FUNC_FCTN_ANALOG_PM_RATE_KHZ:
		case KEY_FUNC_FCTN_ANALOG_PM_RATE_HZ:
			key->isEnabled = sysData.analog.pm.state && (sysData.analog.source == astInternal);
			if (key->isEnabled)
				p.setPen(BUTTONENABLECOLOR);
			else
				p.setPen(BUTTONDISABLECOLOR);
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
			/*
			 case KEY_FUNC_FCTN_DIGITAL_PLUSE_PERIOD:
			 case KEY_FUNC_FCTN_DIGITAL_PLUSE_PERIOD_MS:
			 case KEY_FUNC_FCTN_DIGITAL_PLUSE_PERIOD_S:
			 case KEY_FUNC_FCTN_DIGITAL_PLUSE_PERIOD_US:
			 case KEY_FUNC_FCTN_DIGITAL_PLUSE_PERIOD_NS:
			 case KEY_FUNC_FCTN_DIGITAL_PLUSE_STATE_OFF:
			 case KEY_FUNC_FCTN_DIGITAL_PLUSE_STATE_ON:
			 case KEY_FUNC_FCTN_DIGITAL_PLUSE_WIDTH:
			 case KEY_FUNC_FCTN_DIGITAL_PLUSE_WIDTH_MS:
			 case KEY_FUNC_FCTN_DIGITAL_PLUSE_WIDTH_NS:
			 case KEY_FUNC_FCTN_DIGITAL_PLUSE_WIDTH_S:
			 case KEY_FUNC_FCTN_DIGITAL_PLUSE_WIDTH_US:
			 */
			//	  p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			//	  break;

			//digital
		case KEY_FUNC_FCTN_DIGITAL:
		case KEY_FUNC_FCTN_DIGITAL_STATE:
			p.drawText(QRect(sep, sep + 4, width - 2 * sep, (height - 2 * sep) / 2), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			p.drawPixmap(QRect(width / 4, sep + 4 + (height - 2 * sep) / 2, (width - 2 * sep) / 2, (height - 2 * sep) / 4), img_IN);//zpc changed
			break;

		case KEY_FUNC_FCTN_DIGITAL_TYPE:
		case KEY_FUNC_FCTN_DIGITAL_TYPE_ASK:
		case KEY_FUNC_FCTN_DIGITAL_TYPE_FSK:
		case KEY_FUNC_FCTN_DIGITAL_TYPE_PSK:
		case KEY_FUNC_FCTN_DIGITAL_TYPE_QAM:
		case KEY_FUNC_FCTN_DIGITAL_PARAMETER:
		case KEY_FUNC_FCTN_DIGITAL_DATASOURCE:
		case KEY_FUNC_FCTN_DIGITAL_DATASOURCE_PRBS:
		case KEY_FUNC_FCTN_DIGITAL_MORE_SYMBOLRATE:
		case KEY_FUNC_FCTN_DIGITAL_MORE_FILTERSELECT:
		case KEY_FUNC_FCTN_DIGITAL_MORE_FILTERPARA:
		case KEY_FUNC_FCTN_DIGITAL_SOURCE:
			key->isEnabled = sysData.digital.digitalstate;
			if (key->isEnabled)
				p.setPen(BUTTONENABLECOLOR);
			else
				p.setPen(BUTTONDISABLECOLOR);
			//p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / 2), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			p.drawText(QRect(sep, sep + 4, width - 2 * sep, (height - 2 * sep) / 2), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			//p.drawPixmap(QRect(sep, sep + (height - 2 * sep) / 2, width - 2 * sep, (height - 2 * sep) / 2), img_IN);
			p.drawPixmap(QRect(width / 4, sep + 4 + (height - 2 * sep) / 2, (width - 2 * sep) / 2, (height - 2 * sep) / 4), img_IN);//zpc changed
			break;

		case KEY_FUNC_FCTN_DIGITAL_RETURN:
		case KEY_FUNC_FCTN_DIGITAL_STATE_RETURN:
		case KEY_FUNC_FCTN_DIGITAL_TYPE_RETURN:
		case KEY_FUNC_FCTN_DIGITAL_TYPE_ASK_RETURN:
		case KEY_FUNC_FCTN_DIGITAL_TYPE_FSK_RETURN:
		case KEY_FUNC_FCTN_DIGITAL_TYPE_PSK_RETURN:
		case KEY_FUNC_FCTN_DIGITAL_TYPE_QAM_RETURN:
		case KEY_FUNC_FCTN_DIGITAL_PARAMETER_RETURN:
		case KEY_FUNC_FCTN_DIGITAL_DATASOURCE_RETURN:
		case KEY_FUNC_FCTN_DIGITAL_DATASOURCE_PRBS_RETURN:
		case KEY_FUNC_FCTN_DIGITAL_MORE_FILTERSELECT_RETURN:
		case KEY_FUNC_FCTN_DIGITAL_MORE_FILTERPARA_RETURN:
		case KEY_FUNC_FCTN_DIGITAL_SOURCE_RETURN:
			p.drawPixmap(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), img_OUT);
			break;

		case KEY_FUNC_FCTN_DIGITAL_MORE_RETURN:
		case KEY_FUNC_FCTN_DIGITAL_TYPE_PSK_MORE_RETURN:
			p.drawPixmap(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), img_UP);
			break;

		case KEY_FUNC_FCTN_DIGITAL_STATE_ON:
		case KEY_FUNC_FCTN_DIGITAL_STATE_OFF:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;

		case KEY_FUNC_FCTN_DIGITAL_TYPE_ASK_2ASK:
		case KEY_FUNC_FCTN_DIGITAL_TYPE_ASK_4ASK:
		case KEY_FUNC_FCTN_DIGITAL_TYPE_ASK_8ASK:
		case KEY_FUNC_FCTN_DIGITAL_TYPE_FSK_MSK:
		case KEY_FUNC_FCTN_DIGITAL_TYPE_FSK_2FSK:
		case KEY_FUNC_FCTN_DIGITAL_TYPE_FSK_4FSK:
		case KEY_FUNC_FCTN_DIGITAL_TYPE_FSK_8FSK:
		case KEY_FUNC_FCTN_DIGITAL_TYPE_PSK_2PSK:
		case KEY_FUNC_FCTN_DIGITAL_TYPE_PSK_4PSK:
		case KEY_FUNC_FCTN_DIGITAL_TYPE_PSK_8PSK:
		case KEY_FUNC_FCTN_DIGITAL_TYPE_PSK_BPSK:
		case KEY_FUNC_FCTN_DIGITAL_TYPE_PSK_QPSK:
		case KEY_FUNC_FCTN_DIGITAL_TYPE_PSK_OQPSK:
		case KEY_FUNC_FCTN_DIGITAL_TYPE_PSK_PI2DBPSK:
		case KEY_FUNC_FCTN_DIGITAL_TYPE_PSK_MORE_PI4QPSK:
		case KEY_FUNC_FCTN_DIGITAL_TYPE_PSK_MORE_PI4DQPSK:
		case KEY_FUNC_FCTN_DIGITAL_TYPE_PSK_MORE_8PSK:
		case KEY_FUNC_FCTN_DIGITAL_TYPE_QAM_QAM16:
		case KEY_FUNC_FCTN_DIGITAL_TYPE_QAM_QAM32:
		case KEY_FUNC_FCTN_DIGITAL_TYPE_QAM_QAM64:
		case KEY_FUNC_FCTN_DIGITAL_TYPE_QAM_QAM128:
			//	case KEY_FUNC_FCTN_DIGITAL_PARAMETER_ASKDEEP:
		case KEY_FUNC_FCTN_DIGITAL_PARAMETER_ASKDEEP_PERCENT:
			//	case KEY_FUNC_FCTN_DIGITAL_PARAMETER_FSKOFFSET:
		case KEY_FUNC_FCTN_DIGITAL_PARAMETER_FSKOFFSET_MHZ:
		case KEY_FUNC_FCTN_DIGITAL_PARAMETER_FSKOFFSET_KHZ:
		case KEY_FUNC_FCTN_DIGITAL_PARAMETER_FSKOFFSET_HZ:
			//	case KEY_FUNC_FCTN_DIGITAL_PARAMETER_PSKDEVI:
		case KEY_FUNC_FCTN_DIGITAL_PARAMETER_PSKDEVI_ANGLE:
		case KEY_FUNC_FCTN_DIGITAL_PARAMETER_RATE:
		case KEY_FUNC_FCTN_DIGITAL_PARAMETER_RATE_KHZ:
		case KEY_FUNC_FCTN_DIGITAL_PARAMETER_RATE_HZ:
		case KEY_FUNC_FCTN_DIGITAL_DATASOURCE_ALL0:
		case KEY_FUNC_FCTN_DIGITAL_DATASOURCE_ALL1:
		case KEY_FUNC_FCTN_DIGITAL_DATASOURCE_PRBS_PRBS7:
		case KEY_FUNC_FCTN_DIGITAL_DATASOURCE_PRBS_PRBS9:
		case KEY_FUNC_FCTN_DIGITAL_DATASOURCE_PRBS_PRBS11:
		case KEY_FUNC_FCTN_DIGITAL_DATASOURCE_PRBS_PRBS15:
		case KEY_FUNC_FCTN_DIGITAL_DATASOURCE_PATTERN:
		case KEY_FUNC_FCTN_DIGITAL_MORE_SYMBOLRATE_SYMLS:
		case KEY_FUNC_FCTN_DIGITAL_MORE_SYMBOLRATE_KSYMLS:
		case KEY_FUNC_FCTN_DIGITAL_MORE_SYMBOLRATE_MSYMLS:
		case KEY_FUNC_FCTN_DIGITAL_MORE_FILTERSELECT_ROOTCOSINE:
		case KEY_FUNC_FCTN_DIGITAL_MORE_FILTERSELECT_COSINE:
		case KEY_FUNC_FCTN_DIGITAL_MORE_FILTERSELECT_GAUSSPURE:
		case KEY_FUNC_FCTN_DIGITAL_MORE_FILTERSELECT_GAUSSFSK:
		case KEY_FUNC_FCTN_DIGITAL_MORE_FILTERPARA_BT:
		case KEY_FUNC_FCTN_DIGITAL_MORE_FILTERPARA_STOPFREQ:
		case KEY_FUNC_FCTN_DIGITAL_SOURCE_INTERNAL:
		case KEY_FUNC_FCTN_DIGITAL_SOURCE_EXTERNAL:
		case KEY_FUNC_FCTN_DIGITAL_MORE_LOAD:
		case KEY_FUNC_FCTN_DIGITAL_MORE_LOAD_ENTER:
			key->isEnabled = sysData.digital.digitalstate;
			if (key->isEnabled)
				p.setPen(BUTTONENABLECOLOR);
			else
				p.setPen(BUTTONDISABLECOLOR);
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;

		case KEY_FUNC_FCTN_DIGITAL_PARAMETER_ASKDEEP:
			if ((sysData.digital.digitalstate == 1) && ((sysData.digital.digitaltype == dt2ASK) || (sysData.digital.digitaltype == dt4ASK) || (sysData.digital.digitaltype == dt8ASK)))
			{
				key->isEnabled = true;
				p.setPen(BUTTONENABLECOLOR);
			} else
			{
				key->isEnabled = false;
				p.setPen(BUTTONDISABLECOLOR);
			}
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FCTN_DIGITAL_PARAMETER_FSKOFFSET:
			if ((sysData.digital.digitalstate == 1) && ((sysData.digital.digitaltype == dt2FSK) || (sysData.digital.digitaltype == dt4FSK) || (sysData.digital.digitaltype == dt8FSK)))
			{
				key->isEnabled = true;
				p.setPen(BUTTONENABLECOLOR);
			} else
			{
				key->isEnabled = false;
				p.setPen(BUTTONDISABLECOLOR);
			}
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FCTN_DIGITAL_PARAMETER_PSKDEVI:
			if ((sysData.digital.digitalstate == 1) && ((sysData.digital.digitaltype == dt2PSK) || (sysData.digital.digitaltype == dt4PSK) || (sysData.digital.digitaltype == dt8PSK)))
			{
				key->isEnabled = true;
				p.setPen(BUTTONENABLECOLOR);
			} else
			{
				key->isEnabled = false;
				p.setPen(BUTTONDISABLECOLOR);
			}
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;

		case KEY_FUNC_FCTN_DIGITAL_TYPE_PSK_MORE:
		case KEY_FUNC_FCTN_DIGITAL_MORE:
			p.drawPixmap(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), img_DOWN);
			break;

			//	  脉冲 PLUSE
		case KEY_FUNC_FCTN_PLUSE:
		case KEY_FUNC_FCTN_PLUSE_STATE:
			p.drawText(QRect(sep, sep + 4, width - 2 * sep, (height - 2 * sep) / 2), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			p.drawPixmap(QRect(width / 4, sep + 4 + (height - 2 * sep) / 2, (width - 2 * sep) / 2, (height - 2 * sep) / 4), img_IN);//zpc changed
			break;

		case KEY_FUNC_FCTN_PLUSE_SOURCE:
			key->isEnabled = sysData.pluse.state;
			if (key->isEnabled)
				p.setPen(BUTTONENABLECOLOR);
			else
				p.setPen(BUTTONDISABLECOLOR);
			p.drawText(QRect(sep, sep + 4, width - 2 * sep, (height - 2 * sep) / 2), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			p.drawPixmap(QRect(width / 4, sep + 4 + (height - 2 * sep) / 2, (width - 2 * sep) / 2, (height - 2 * sep) / 4), img_IN);//zpc changed
			break;

		case KEY_FUNC_FCTN_PLUSE_METHOD:
			key->isEnabled = (sysData.pluse.plusesource == psInternal && sysData.pluse.state);
			if (key->isEnabled)
				p.setPen(BUTTONENABLECOLOR);
			else
				p.setPen(BUTTONDISABLECOLOR);
			p.drawText(QRect(sep, sep + 4, width - 2 * sep, (height - 2 * sep) / 2), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			p.drawPixmap(QRect(width / 4, sep + 4 + (height - 2 * sep) / 2, (width - 2 * sep) / 2, (height - 2 * sep) / 4), img_IN);//zpc changed
			break;

		case KEY_FUNC_FCTN_PLUSE_STATE_ON:
		case KEY_FUNC_FCTN_PLUSE_STATE_OFF:
			//	case KEY_FUNC_FCTN_PLUSE_METHOD_AUTO:
			//	case KEY_FUNC_FCTN_PLUSE_METHOD_TRIG:
			//	case KEY_FUNC_FCTN_PLUSE_METHOD_EXTER:
		case KEY_FUNC_FCTN_PLUSE_SOURCE_INTERNAL:
		case KEY_FUNC_FCTN_PLUSE_SOURCE_EXTERNAL:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;

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
		case KEY_FUNC_FCTN_PLUSE_METHOD_AUTO:
		case KEY_FUNC_FCTN_PLUSE_METHOD_TRIG:
			if (sysData.pluse.plusesource == psInternal && sysData.pluse.state)
			{
				key->isEnabled = true;
				p.setPen(BUTTONENABLECOLOR);
			} else
			{
				key->isEnabled = false;
				p.setPen(BUTTONDISABLECOLOR);
			}
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;

		case KEY_FUNC_FCTN_PLUSE_RETURN:
		case KEY_FUNC_FCTN_PLUSE_STATE_RETURN:
		case KEY_FUNC_FCTN_PLUSE_METHOD_RETURN:
		case KEY_FUNC_FCTN_PLUSE_SOURCE_RETURN:
			p.drawPixmap(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), img_OUT);
			break;

			//ext Rf
		case KEY_FUNC_FCTN_EXTRF:
			key->isEnabled = sysData.freq.freq > 300e6; //设置频率小于300MHz时，上变频功能不能用
			if (key->isEnabled)
				p.setPen(BUTTONENABLECOLOR);
			else
				p.setPen(BUTTONDISABLECOLOR);
			//p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / 2), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			p.drawText(QRect(sep, sep + 4, width - 2 * sep, (height - 2 * sep) / 2), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			//p.drawPixmap(QRect(sep, sep + (height - 2 * sep) / 2, width - 2 * sep, (height - 2 * sep) / 2), img_IN);
			p.drawPixmap(QRect(width / 4, sep + 4 + (height - 2 * sep) / 2, (width - 2 * sep) / 2, (height - 2 * sep) / 4), img_IN);//zpc changed
			break;
		case KEY_FUNC_FCTN_EXTRF_EXTRF:
			//p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / 2), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			p.drawText(QRect(sep, sep + 4, width - 2 * sep, (height - 2 * sep) / 2), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			//p.drawPixmap(QRect(sep, sep + (height - 2 * sep) / 2, width - 2 * sep, (height - 2 * sep) / 2), img_IN);
			p.drawPixmap(QRect(width / 4, sep + 4 + (height - 2 * sep) / 2, (width - 2 * sep) / 2, (height - 2 * sep) / 4), img_IN);//zpc changed
			break;

		case KEY_FUNC_FCTN_EXTRF_EXTRF_INT:
		case KEY_FUNC_FCTN_EXTRF_EXTRF_EXT:
		case KEY_FUNC_FCTN_EXTRF_FREQ_GHZ:
		case KEY_FUNC_FCTN_EXTRF_FREQ_MHZ:
		case KEY_FUNC_FCTN_EXTRF_FREQ_KHZ:
		case KEY_FUNC_FCTN_EXTRF_FREQ_HZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;

		case KEY_FUNC_FCTN_EXTRF_FREQ:
			//	  key->isEnabled = sysData.exfRef.type == erftExternal;
			//	  if (!key->isEnabled) p.setPen(BUTTONDISABLECOLOR); else p.setPen(BUTTONENABLECOLOR);
			key->isEnabled = ((sysData.exfRef.type == erftExternal) && sysData.freq.freq > 300e6);
			if (key->isEnabled)
				p.setPen(BUTTONENABLECOLOR);
			else
				p.setPen(BUTTONDISABLECOLOR);
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;

		case KEY_FUNC_FCTN_EXTRF_EXTRF_RETURN:
		case KEY_FUNC_FCTN_EXTRF_RETURN:
			p.drawPixmap(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), img_OUT);
			break;

			//LF
			//	case KEY_FUNC_FCTN_DIGITAL_MODWAVE:
		case KEY_FUNC_FCTN_LF:
		case KEY_FUNC_FCTN_LF_STATE:
			//	case KEY_FUNC_FCTN_LF_SOURCE_INTERNAL:
			//p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / 2), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			p.drawText(QRect(sep, sep + 4, width - 2 * sep, (height - 2 * sep) / 2), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			//p.drawPixmap(QRect(sep, sep + (height - 2 * sep) / 2, width - 2 * sep, (height - 2 * sep) / 2), img_IN);
			p.drawPixmap(QRect(width / 4, sep + 4 + (height - 2 * sep) / 2, (width - 2 * sep) / 2, (height - 2 * sep) / 4), img_IN);//zpc changed
			break;

		case KEY_FUNC_FCTN_LF_SOURCE:
			key->isEnabled = sysData.lf.state;
			if (key->isEnabled)
				p.setPen(BUTTONENABLECOLOR);
			else
				p.setPen(BUTTONDISABLECOLOR);
			p.drawText(QRect(sep, sep + 4, width - 2 * sep, (height - 2 * sep) / 2), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			p.drawPixmap(QRect(width / 4, sep + 4 + (height - 2 * sep) / 2, (width - 2 * sep) / 2, (height - 2 * sep) / 4), img_IN);//zpc changed
			break;

			//	case KEY_FUNC_FCTN_DIGITAL_MODWAVE_RETURN:
		case KEY_FUNC_FCTN_LF_RETURN:
		case KEY_FUNC_FCTN_LF_STATE_RETURN:
		case KEY_FUNC_FCTN_LF_SOURCE_RETURN:
			//	case KEY_FUNC_FCTN_LF_WAVEFORM_RETURN:
			p.drawPixmap(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), img_OUT);
			break;

			//	case KEY_FUNC_FCTN_DIGITAL_MODWAVE_MARRAY:
		case KEY_FUNC_FCTN_LF_STATE_ON:
		case KEY_FUNC_FCTN_LF_STATE_OFF:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;

		case KEY_FUNC_FCTN_LF_WAVEFORM_SINE:
		case KEY_FUNC_FCTN_LF_WAVEFORM_SQUARE:
		case KEY_FUNC_FCTN_LF_WAVEFORM_TRIANGULAR:
		case KEY_FUNC_FCTN_LF_WAVEFORM_SAWTOOTH:
		case KEY_FUNC_FCTN_LF_SOURCE_SINC:
		case KEY_FUNC_FCTN_LF_AMPL:
		case KEY_FUNC_FCTN_LF_AMPL_VPP:
		case KEY_FUNC_FCTN_LF_AMPL_MVPP:
		case KEY_FUNC_FCTN_LF_FREQ:
		case KEY_FUNC_FCTN_LF_FREQ_MHZ:
		case KEY_FUNC_FCTN_LF_FREQ_KHZ:
		case KEY_FUNC_FCTN_LF_FREQ_HZ:
			//	case KEY_FUNC_FCTN_LF_SOURCE_EXTERNAL:
			key->isEnabled = sysData.lf.state;
			if (key->isEnabled)
				p.setPen(BUTTONENABLECOLOR);
			else
				p.setPen(BUTTONDISABLECOLOR);
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;

			//	case KEY_FUNC_FCTN_DIGITAL_MORE:
			//	  p.drawPixmap(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), img_DOWN);
			//	  break;

			//	case KEY_FUNC_FCTN_DIGITAL_MORE_RETURN:
			//	  p.drawPixmap(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), img_UP);
			//	  break;


			//DRG
		case KEY_FUNC_FCTN_DRG:
		case KEY_FUNC_FCTN_DRG_STATE:
			p.drawText(QRect(sep, sep + 4, width - 2 * sep, (height - 2 * sep) / 2), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			p.drawPixmap(QRect(width / 4, sep + 4 + (height - 2 * sep) / 2, (width - 2 * sep) / 2, (height - 2 * sep) / 4), img_IN);
			break;
		case KEY_FUNC_FCTN_DRG_MORE_WORKMODE:
			key->isEnabled = sysData.drg.state;
			if (key->isEnabled)
				p.setPen(BUTTONENABLECOLOR);
			else
				p.setPen(BUTTONDISABLECOLOR);
			p.drawText(QRect(sep, sep + 4, width - 2 * sep, (height - 2 * sep) / 2), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			p.drawPixmap(QRect(width / 4, sep + 4 + (height - 2 * sep) / 2, (width - 2 * sep) / 2, (height - 2 * sep) / 4), img_IN);
			break;
		case KEY_FUNC_FCTN_DRG_UPLIMIT:
		case KEY_FUNC_FCTN_DRG_UPLIMIT_GHZ:
		case KEY_FUNC_FCTN_DRG_UPLIMIT_MHZ:
		case KEY_FUNC_FCTN_DRG_UPLIMIT_KHZ:
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
		case KEY_FUNC_FCTN_DRG_MORE_WORKMODE_NORMAL:
		case KEY_FUNC_FCTN_DRG_MORE_WORKMODE_POSITIVE:
		case KEY_FUNC_FCTN_DRG_MORE_WORKMODE_NEGATIVE:
			key->isEnabled = sysData.drg.state;
			if (key->isEnabled)
				p.setPen(BUTTONENABLECOLOR);
			else
				p.setPen(BUTTONDISABLECOLOR);
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FCTN_DRG_STATE_ON:
		case KEY_FUNC_FCTN_DRG_STATE_OFF:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FCTN_DRG_MORE:
			p.drawPixmap(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), img_DOWN);
			break;
		case KEY_FUNC_FCTN_DRG_MORE_RETURN:
			p.drawPixmap(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), img_UP);
			break;
		case KEY_FUNC_FCTN_DRG_RETURN:
		case KEY_FUNC_FCTN_DRG_STATE_RETURN:
		case KEY_FUNC_FCTN_DRG_MORE_WORKMODE_RETURN:
			p.drawPixmap(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), img_OUT);
			break;

			//AUX
		case KEY_FUNC_AUX_LAN:
		case KEY_FUNC_AUX_LANGUAGE:
		case KEY_FUNC_AUX_SCPI:
			//p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / 2), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			p.drawText(QRect(sep, sep + 4, width - 2 * sep, (height - 2 * sep) / 2), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			//p.drawPixmap(QRect(sep, sep + (height - 2 * sep) / 2, width - 2 * sep, (height - 2 * sep) / 2), img_IN);
			p.drawPixmap(QRect(width / 4, sep + 4 + (height - 2 * sep) / 2, (width - 2 * sep) / 2, (height - 2 * sep) / 4), img_IN);//zpc changed
			break;

		case KEY_FUNC_AUX_LAN_IP:
		case KEY_FUNC_AUX_LAN_IP_ENTER:
		case KEY_FUNC_AUX_LAN_MASK:
		case KEY_FUNC_AUX_LAN_MASK_ENTER:
		case KEY_FUNC_AUX_LAN_GATEWAY:
			//	case KEY_FUNC_AUX_SCPI_RETURN:
		case KEY_FUNC_AUX_SCPI_OFF:
		case KEY_FUNC_AUX_SCPI_ON:
		case KEY_FUNC_AUX_LAN_GATEWAY_ENTER:
		case KEY_FUNC_AUX_LANGUAGE_CHINESE:
		case KEY_FUNC_AUX_LANGUAGE_ENGLISH:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;

		case KEY_FUNC_AUX_LAN_RETURN:
		case KEY_FUNC_AUX_LANGUAGE_RETURN:
		case KEY_FUNC_AUX_SCPI_RETURN:
			p.drawPixmap(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), img_OUT);
			break;

			//系统服务
		case KEY_FUNC_AUX_SERVICE:
		case KEY_FUNC_AUX_SERVICE_EVALUATE:
			p.drawText(QRect(sep, sep + 4, width - 2 * sep, (height - 2 * sep) / 2), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			p.drawPixmap(QRect(width / 4, sep + 4 + (height - 2 * sep) / 2, (width - 2 * sep) / 2, (height - 2 * sep) / 4), img_IN);
			break;
		case KEY_FUNC_AUX_SERVICE_EVALUATE_ON:
		case KEY_FUNC_AUX_SERVICE_EVALUATE_OFF:
		case KEY_FUNC_AUX_SERVICE_LANMACADDR:
		case KEY_FUNC_AUX_SERVICE_LANMACADDR_ENTER:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignHCenter | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AUX_SERVICE_RETURN:
		case KEY_FUNC_AUX_SERVICE_EVALUATE_RETURN:
			p.drawPixmap(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), img_OUT);
			break;

		default:
			break;
	}

	p.end();
}
