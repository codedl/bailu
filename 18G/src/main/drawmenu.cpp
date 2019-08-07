#include "mainform.h"

//绘制按钮菜单
void mainForm::drawButtonMenu(QPixmap* pix, subKeyDef* key)
{
	if ((pix == NULL) || (pix->isNull() || key == NULL))
	{
		helpContent = "internal error";
		drawHelp();
		return;
	}

	QPainter p;
	p.begin(pix);

	if (key == nowSubKey)
	{
		p.fillRect(pix->rect(), BUTTONSELECTCOLOR);
	} else
	{
		p.fillRect(pix->rect(), BUTTONUNSELECTCOLOR);
	}

	p.setPen(Qt::black);
	if (!key->isEnabled)
	{
		p.setPen(BUTTONDISABLECOLOR);
	}

	if (key->isBlank)
	{
		p.end();
		return;
	}

	QString keyName = getTranslatorString(key->enName, key->chName);
	char tempChar[32] = { };
	QRect rc;
	int rcWidthFloat = 0;
	int rcNums = 0;
	int rows = 1;
	int sep = 8;
	int enSep = 2;
	int userIndex = 0;
	int pngSubSize = 10;
	int width = pix->width();
	int height = pix->height();
	p.setFont(fontBold);

	switch (key->funcCode)
	{
		//频率键
		case KEY_FUNC_FREQ_CENTER:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, ((QString) getFreqAndUnitString(sysData.freq.center, 1, tempChar)).trimmed());
			break;
		case KEY_FUNC_FREQ_CENTER_GHZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FREQ_CENTER_MHZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FREQ_CENTER_KHZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FREQ_CENTER_HZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FREQ_START:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, ((QString) getFreqAndUnitString(sysData.freq.start, 1, tempChar)).trimmed());
			break;
		case KEY_FUNC_FREQ_START_GHZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FREQ_START_MHZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FREQ_START_KHZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FREQ_START_HZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FREQ_STOP:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, ((QString) getFreqAndUnitString(sysData.freq.stop, 1, tempChar)).trimmed());
			break;
		case KEY_FUNC_FREQ_STOP_GHZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FREQ_STOP_MHZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FREQ_STOP_KHZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FREQ_STOP_HZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FREQ_CFSTEP:
			rows = 3;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, ((QString) getFreqAndUnitString(sysData.freq.cfStep, 1, tempChar)).trimmed());
			rc = QRect(sep, rc.top() + rc.height(), (width - 2 * sep) / 2, rc.height() + enSep);
			if (sysData.freq.cfStepAuto)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("Auto"));
			rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
			if (!sysData.freq.cfStepAuto)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Man"));
			break;
		case KEY_FUNC_FREQ_CFSTEP_GHZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FREQ_CFSTEP_MHZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FREQ_CFSTEP_KHZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FREQ_CFSTEP_HZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FREQ_OFFSET:
			key->isEnabled = !sysData.measure.passFail.winMeasOn && !sysData.measure.passFail.lineMeasOn;
			if (!key->isEnabled)
				p.setPen(BUTTONDISABLECOLOR);
			else
				p.setPen(BUTTONENABLECOLOR);

			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, ((QString) getFreqAndUnitString(sysData.freq.offset, 1, tempChar)).trimmed());
			break;
		case KEY_FUNC_FREQ_OFFSET_GHZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FREQ_OFFSET_MHZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FREQ_OFFSET_KHZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FREQ_OFFSET_HZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FREQ_REFERENCE:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.freq.refInternal)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("Int"));
			rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
			if (!sysData.freq.refInternal)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Ext"));
			break;
		case KEY_FUNC_FREQ_LOW:
			key->isEnabled = sysData.options.directChannelOptionOn;
			if (!key->isEnabled)
				p.setPen(BUTTONDISABLECOLOR);
			else
				p.setPen(BUTTONENABLECOLOR);
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.freq.isLowChannel)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("On"));
			rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
			if (!sysData.freq.isLowChannel)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Off"));
			break;
		case KEY_FUNC_FREQ_NEXTPAGE:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FREQ_SCALETYPE:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.freq.typeLine)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("Lin"));
			rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
			if (!sysData.freq.typeLine)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Log"));
			break;
		case KEY_FUNC_FREQ_NEXTPAGE_RETURN:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
			//扫宽键
		case KEY_FUNC_SPAN_SPAN:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, ((QString) getFreqAndUnitString(sysData.span.span, 1, tempChar)).trimmed());
			break;
		case KEY_FUNC_SPAN_SPAN_GHZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SPAN_SPAN_MHZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SPAN_SPAN_KHZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SPAN_SPAN_HZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SPAN_FULL:
			rows = 1;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SPAN_ZERO:
			rows = 1;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SPAN_LAST:
			rows = 1;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;

			//幅度键
		case KEY_FUNC_AMPT_REFLEVEL:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, ((QString) sysScpi->getAmptAndUnitString(sysData.ampt.refLevel, tempChar, 0)).trimmed());
			break;
		case KEY_FUNC_AMPT_REFLEVEL_DBM:
			switch (sysData.ampt.refUnit)
			{
				case rudBm:
					p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, "dBm");
					break;
				case rudBmV:
					p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, "dBmV");
					break;
				case rudBuV:
					p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, "dBuV");
					break;
				case ruW:
					p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, "mW");
					break;
				case ruV:
					p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, "mV");
					break;
				case rudBpW:
					p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, "dBpW");
					break;
				case rudBuW:
					p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, "dBuW");
					break;
			}
			break;
		case KEY_FUNC_AMPT_ATT:
			rows = 3;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			keyName = intToString(sysData.ampt.att, tempChar);
			keyName += " dB";
			p.setFont(fontData);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(sep, rc.top() + rc.height(), (width - 2 * sep) / 2, rc.height() + enSep);
			if (sysData.ampt.attAuto)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("Auto"));
			rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
			if (!sysData.ampt.attAuto)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Man"));
			break;
		case KEY_FUNC_AMPT_ATT_DB:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AMPT_SCALEDIV:
			key->isEnabled = !sysData.emc.emcOn && !sysData.ampt.scaleLine;
			//key->isEnabled = !sysData.emc.emcOn;
			if (!key->isEnabled)
				p.setPen(BUTTONDISABLECOLOR);
			else
				p.setPen(BUTTONENABLECOLOR);
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			keyName = floatToStringDot2(sysData.ampt.scaleDiv, tempChar);
			keyName += " dB";
			p.setFont(fontData);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AMPT_SCALEDIV_DB:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AMPT_SCALETYPE:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.ampt.scaleLine)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("Lin"));
			rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
			if (!sysData.ampt.scaleLine)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Log"));
			break;
		case KEY_FUNC_AMPT_REFOFFSET:
			key->isEnabled = !sysData.measure.passFail.winMeasOn && !sysData.measure.passFail.lineMeasOn;
			if (!key->isEnabled)
				p.setPen(BUTTONDISABLECOLOR);
			else
				p.setPen(BUTTONENABLECOLOR);
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			keyName = floatToStringDot2(sysData.ampt.refOffset, tempChar);
			keyName += " dB";
			p.setFont(fontData);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AMPT_REFOFFSET_DB:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;

		case KEY_FUNC_AMPT_REFUNIT:
			key->isEnabled = !sysData.measure.passFail.winMeasOn && !sysData.measure.passFail.lineMeasOn;
			if (!key->isEnabled)
				p.setPen(BUTTONDISABLECOLOR);
			else
				p.setPen(BUTTONENABLECOLOR);
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width(), rc.height());
			p.setFont(fontData);
			switch (sysData.ampt.refUnit)
			{
				case rudBm:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, "dBm");
					break;
				case rudBmV:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, "dBmV");
					break;
				case rudBuV:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, "dBuV");
					break;
				case ruW:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, "W");
					break;
				case ruV:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, "V");
					break;
				case rudBpW:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, "dBpW");
					break;
				case rudBuW:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, "dBuW");
					break;
				case rudBuVm:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, "dBuV/m");
					break;
				case ruuV:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, "uV");
					break;
				default:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, "unknown");
					break;
			}
			rc = QRect(width - sep - pngSubSize, (height - 16) / 2, 16, 16);
			if (!key->isEnabled)
				p.drawPixmap(rc, img_ArrowHui);
			else
				p.drawPixmap(rc, img_Arrow);
			break;
		case KEY_FUNC_AMPT_REFUNIT_DBM:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AMPT_REFUNIT_DBuW:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AMPT_REFUNIT_DBPW:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AMPT_REFUNIT_DBMV:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AMPT_REFUNIT_DBUV:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AMPT_REFUNIT_W:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AMPT_REFUNIT_V:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AMPT_PREAMPT:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.ampt.isPreamptOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("On"));
			rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
			if (!sysData.ampt.isPreamptOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Off"));
			break;

			//Pass-Failed
		case KEY_FUNC_AMPT_PASSFAIL:
			p.drawText(QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(width - sep - pngSubSize, (height - 16) / 2, 16, 16);
			if (!key->isEnabled)
				p.drawPixmap(rc, img_ArrowHui);
			else
				p.drawPixmap(rc, img_Arrow);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_WINDOW:
			p.drawText(QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(width - sep - pngSubSize, (height - 16) / 2, 16, 16);
			if (!key->isEnabled)
				p.drawPixmap(rc, img_ArrowHui);
			else
				p.drawPixmap(rc, img_Arrow);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_WINDOW_WINDOW:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.measure.passFail.winMeasOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("On"));
			rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
			if (!sysData.measure.passFail.winMeasOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Off"));
			break;
		case KEY_FUNC_AMPT_PASSFAIL_WINDOW_LIMITLINEON:
			key->isEnabled = sysData.measure.passFail.winMeasOn;
			if (!key->isEnabled)
				p.setPen(BUTTONDISABLECOLOR);
			else
				p.setPen(BUTTONENABLECOLOR);

			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.measure.passFail.winAmptOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("On"));
			rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
			if (!sysData.measure.passFail.winAmptOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Off"));
			break;
		case KEY_FUNC_AMPT_PASSFAIL_WINDOW_FREQLINEON:
			key->isEnabled = sysData.measure.passFail.winMeasOn;
			if (!key->isEnabled)
				p.setPen(BUTTONDISABLECOLOR);
			else
				p.setPen(BUTTONENABLECOLOR);

			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.measure.passFail.winFreqOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("On"));
			rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
			if (!sysData.measure.passFail.winFreqOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Off"));
			break;
		case KEY_FUNC_AMPT_PASSFAIL_WINDOW_LIMITUPLOW:
			key->isEnabled = sysData.measure.passFail.winMeasOn;
			if (!key->isEnabled)
				p.setPen(BUTTONDISABLECOLOR);
			else
				p.setPen(BUTTONENABLECOLOR);

			rows = 3;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());

			if (sysData.measure.passFail.winNowAmptUpOrLow == 1)
			{
				keyName = ((QString) sysScpi->getAmptAndUnitString(sysData.measure.passFail.winAmptLow, tempChar, 0)).trimmed();
			} else
			{
				keyName = ((QString) sysScpi->getAmptAndUnitString(sysData.measure.passFail.winAmptUp, tempChar, 0)).trimmed();
			}

			p.setFont(fontData);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(sep, rc.top() + rc.height(), (width - 2 * sep) / 2, rc.height() + enSep);
			if (!sysData.measure.passFail.winNowAmptUpOrLow)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("Up"));
			rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
			if (sysData.measure.passFail.winNowAmptUpOrLow)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Low"));
			break;
		case KEY_FUNC_AMPT_PASSFAIL_WINDOW_LIMITUPLOW_DBM:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_WINDOW_FREQSTARTSTOP:
			key->isEnabled = sysData.measure.passFail.winMeasOn;
			if (!key->isEnabled)
				p.setPen(BUTTONDISABLECOLOR);
			else
				p.setPen(BUTTONENABLECOLOR);

			rows = 3;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());

			if (sysData.measure.passFail.winNowFreqStartOrStop == 1)
			{
				keyName = ((QString) getFreqAndUnitString(sysData.measure.passFail.winFreqStop, 1, tempChar)).trimmed();
			} else
			{
				keyName = ((QString) getFreqAndUnitString(sysData.measure.passFail.winFreqStart, 1, tempChar)).trimmed();
			}

			p.setFont(fontData);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(sep, rc.top() + rc.height(), (width - 2 * sep) / 2, rc.height() + enSep);
			if (!sysData.measure.passFail.winNowFreqStartOrStop)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("Start"));
			rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
			if (sysData.measure.passFail.winNowFreqStartOrStop)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Stop"));
			break;
		case KEY_FUNC_AMPT_PASSFAIL_WINDOW_FREQSTARTSTOP_GHZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_WINDOW_FREQSTARTSTOP_MHZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_WINDOW_FREQSTARTSTOP_KHZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_WINDOW_FREQSTARTSTOP_HZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_WINDOW_WINDOWSWEEP:
			key->isEnabled = sysData.measure.passFail.winMeasOn;
			if (!key->isEnabled)
				p.setPen(BUTTONDISABLECOLOR);
			else
				p.setPen(BUTTONENABLECOLOR);

			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.measure.passFail.winSweepOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("On"));
			rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
			if (!sysData.measure.passFail.winSweepOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Off"));
			break;
		case KEY_FUNC_AMPT_PASSFAIL_WINDOW_RETURN:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE:
			p.drawText(QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(width - sep - pngSubSize, (height - 16) / 2, 16, 16);
			if (!key->isEnabled)
				p.drawPixmap(rc, img_ArrowHui);
			else
				p.drawPixmap(rc, img_Arrow);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.measure.passFail.lineMeasOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("On"));
			rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
			if (!sysData.measure.passFail.lineMeasOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Off"));
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE1ON:
			key->isEnabled = sysData.measure.passFail.lineMeasOn;
			if (!key->isEnabled)
				p.setPen(BUTTONDISABLECOLOR);
			else
				p.setPen(BUTTONENABLECOLOR);

			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.measure.passFail.line1On)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("On"));
			rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
			if (!sysData.measure.passFail.line1On)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Off"));
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE2ON:
			key->isEnabled = sysData.measure.passFail.lineMeasOn;
			if (!key->isEnabled)
				p.setPen(BUTTONDISABLECOLOR);
			else
				p.setPen(BUTTONENABLECOLOR);

			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.measure.passFail.line2On)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("On"));
			rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
			if (!sysData.measure.passFail.line2On)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Off"));
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE1EDIT:
			key->isEnabled = sysData.measure.passFail.lineMeasOn;
			if (!key->isEnabled)
				p.setPen(BUTTONDISABLECOLOR);
			else
				p.setPen(BUTTONENABLECOLOR);

			p.drawText(QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(width - sep - pngSubSize, (height - 16) / 2, 16, 16);
			if (!key->isEnabled)
				p.drawPixmap(rc, img_ArrowHui);
			else
				p.drawPixmap(rc, img_Arrow);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE1EDIT_FIRSTPAGE:
			if (sysData.measure.passFail.line1Editing && sysData.measure.passFail.line1Pages > 1)
			{
				key->isEnabled = true;
			} else
			{
				key->isEnabled = false;
			}
			if (!key->isEnabled)
			{
				p.setPen(BUTTONDISABLECOLOR);
			} else
			{
				p.setPen(BUTTONENABLECOLOR);
			}
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE1EDIT_PREVPAGE:
			if (sysData.measure.passFail.line1Editing && sysData.measure.passFail.line1Pages > 1 && sysData.measure.passFail.line1NowPageIndex >= sysData.measure.passFail.line1Pages - 1)
			{
				key->isEnabled = true;
			} else
			{
				key->isEnabled = false;
			}
			if (!key->isEnabled)
			{
				p.setPen(BUTTONDISABLECOLOR);
			} else
			{
				p.setPen(BUTTONENABLECOLOR);
			}
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE1EDIT_NEXTPAGE:
			if (sysData.measure.passFail.line1Editing && sysData.measure.passFail.line1Pages > 1 && sysData.measure.passFail.line1NowPageIndex < sysData.measure.passFail.line1Pages - 1)
			{
				key->isEnabled = true;
			} else
			{
				key->isEnabled = false;
			}
			if (!key->isEnabled)
			{
				p.setPen(BUTTONDISABLECOLOR);
			} else
			{
				p.setPen(BUTTONENABLECOLOR);
			}
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE1EDIT_LASTPAGE:
			if (sysData.measure.passFail.line1Editing && sysData.measure.passFail.line1Pages > 1)
			{
				key->isEnabled = true;
			} else
			{
				key->isEnabled = false;
			}
			if (!key->isEnabled)
			{
				p.setPen(BUTTONDISABLECOLOR);
			} else
			{
				p.setPen(BUTTONENABLECOLOR);
			}
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE1EDIT_OPERATION:
			p.drawText(QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(width - sep - pngSubSize, (height - 16) / 2, 16, 16);
			if (!key->isEnabled)
				p.drawPixmap(rc, img_ArrowHui);
			else
				p.drawPixmap(rc, img_Arrow);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE1EDIT_OPERATION_ADD:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE1EDIT_OPERATION_INSERT:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE1EDIT_OPERATION_DELETE:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE1EDIT_OPERATION_CLEAR:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE1EDIT_OPERATION_EDITFREQ:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);

			if (sysData.measure.passFail.line1TotalPoints > 0 && sysData.measure.passFail.line1EditNowSelIndex >= 0 && sysData.measure.passFail.line1EditNowSelIndex < sysData.measure.passFail.line1TotalPoints)
			{
				p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, ((QString) getFreqAndUnitString(sysData.measure.passFail.line1Datas[sysData.measure.passFail.line1EditNowSelIndex + sysData.measure.passFail.line1NowPageIndex * PASSFAILCOUNTPERPAGE].freq, 1, tempChar)).trimmed());
			} else
			{
				p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, "-");
			}
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE1EDIT_OPERATION_EDITFREQ_GHZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE1EDIT_OPERATION_EDITFREQ_MHZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE1EDIT_OPERATION_EDITFREQ_KHZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE1EDIT_OPERATION_EDITFREQ_HZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE1EDIT_OPERATION_EDITLIMIT:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);

			if (sysData.measure.passFail.line1TotalPoints > 0 && sysData.measure.passFail.line1EditNowSelIndex >= 0 && sysData.measure.passFail.line1EditNowSelIndex < sysData.measure.passFail.line1TotalPoints)
			{
				p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, ((QString) sysScpi->getAmptAndUnitString(sysData.measure.passFail.line1Datas[sysData.measure.passFail.line1EditNowSelIndex + sysData.measure.passFail.line1NowPageIndex * PASSFAILCOUNTPERPAGE].ampt, tempChar, 0)).trimmed());
			} else
			{
				p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, "-");
			}
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE1EDIT_OPERATION_EDITLIMIT_DBM:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE1EDIT_OPERATION_RETURN:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE1EDIT_RETURN:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE2EDIT:
			key->isEnabled = sysData.measure.passFail.lineMeasOn;
			if (!key->isEnabled)
				p.setPen(BUTTONDISABLECOLOR);
			else
				p.setPen(BUTTONENABLECOLOR);

			p.drawText(QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(width - sep - pngSubSize, (height - 16) / 2, 16, 16);
			if (!key->isEnabled)
				p.drawPixmap(rc, img_ArrowHui);
			else
				p.drawPixmap(rc, img_Arrow);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE2EDIT_FIRSTPAGE:
			if (sysData.measure.passFail.line2Editing && sysData.measure.passFail.line2Pages > 1)
			{
				key->isEnabled = true;
			} else
			{
				key->isEnabled = false;
			}
			if (!key->isEnabled)
			{
				p.setPen(BUTTONDISABLECOLOR);
			} else
			{
				p.setPen(BUTTONENABLECOLOR);
			}
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE2EDIT_PREVPAGE:
			if (sysData.measure.passFail.line2Editing && sysData.measure.passFail.line2Pages > 1 && sysData.measure.passFail.line2NowPageIndex >= sysData.measure.passFail.line2Pages - 1)
			{
				key->isEnabled = true;
			} else
			{
				key->isEnabled = false;
			}
			if (!key->isEnabled)
			{
				p.setPen(BUTTONDISABLECOLOR);
			} else
			{
				p.setPen(BUTTONENABLECOLOR);
			}
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE2EDIT_NEXTPAGE:
			if (sysData.measure.passFail.line2Editing && sysData.measure.passFail.line2Pages > 1 && sysData.measure.passFail.line2NowPageIndex < sysData.measure.passFail.line2Pages - 1)
			{
				key->isEnabled = true;
			} else
			{
				key->isEnabled = false;
			}
			if (!key->isEnabled)
			{
				p.setPen(BUTTONDISABLECOLOR);
			} else
			{
				p.setPen(BUTTONENABLECOLOR);
			}
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE2EDIT_LASTPAGE:
			if (sysData.measure.passFail.line2Editing && sysData.measure.passFail.line2Pages > 1)
			{
				key->isEnabled = true;
			} else
			{
				key->isEnabled = false;
			}
			if (!key->isEnabled)
			{
				p.setPen(BUTTONDISABLECOLOR);
			} else
			{
				p.setPen(BUTTONENABLECOLOR);
			}
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE2EDIT_OPERATION:
			p.drawText(QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(width - sep - pngSubSize, (height - 16) / 2, 16, 16);
			if (!key->isEnabled)
				p.drawPixmap(rc, img_ArrowHui);
			else
				p.drawPixmap(rc, img_Arrow);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE2EDIT_OPERATION_ADD:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE2EDIT_OPERATION_INSERT:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE2EDIT_OPERATION_DELETE:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE2EDIT_OPERATION_CLEAR:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE2EDIT_OPERATION_EDITFREQ:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);

			if (sysData.measure.passFail.line2TotalPoints > 0 && sysData.measure.passFail.line2EditNowSelIndex >= 0 && sysData.measure.passFail.line2EditNowSelIndex < sysData.measure.passFail.line2TotalPoints)
			{
				p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, ((QString) getFreqAndUnitString(sysData.measure.passFail.line2Datas[sysData.measure.passFail.line2EditNowSelIndex + sysData.measure.passFail.line2NowPageIndex * PASSFAILCOUNTPERPAGE].freq, 1, tempChar)).trimmed());
			} else
			{
				p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, "-");
			}
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE2EDIT_OPERATION_EDITFREQ_GHZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE2EDIT_OPERATION_EDITFREQ_MHZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE2EDIT_OPERATION_EDITFREQ_KHZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE2EDIT_OPERATION_EDITFREQ_HZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE2EDIT_OPERATION_EDITLIMIT:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);

			if (sysData.measure.passFail.line2TotalPoints > 0 && sysData.measure.passFail.line2EditNowSelIndex >= 0 && sysData.measure.passFail.line2EditNowSelIndex < sysData.measure.passFail.line2TotalPoints)
			{
				p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, ((QString) sysScpi->getAmptAndUnitString(sysData.measure.passFail.line2Datas[sysData.measure.passFail.line2EditNowSelIndex + sysData.measure.passFail.line2NowPageIndex * PASSFAILCOUNTPERPAGE].ampt, tempChar, 0)).trimmed());
			} else
			{
				p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, "-");
			}
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE2EDIT_OPERATION_EDITLIMIT_DBM:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE2EDIT_OPERATION_RETURN:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_LINE2EDIT_RETURN:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_SHIFTXY:
			key->isEnabled = sysData.measure.passFail.lineMeasOn;
			if (!key->isEnabled)
				p.setPen(BUTTONDISABLECOLOR);
			else
				p.setPen(BUTTONENABLECOLOR);

			rows = 3;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());

			if (sysData.measure.passFail.lineNowShiftXOrY == 0)
			{
				keyName = ((QString) getFreqAndUnitString(sysData.measure.passFail.lineShiftXOffset, 1, tempChar)).trimmed();
			} else
			{
				keyName = ((QString) sysScpi->getAmptAndUnitString(sysData.measure.passFail.lineShiftYOffset, tempChar, 0)).trimmed();
			}

			p.setFont(fontData);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(sep, rc.top() + rc.height(), (width - 2 * sep) / 2, rc.height() + enSep);
			if (sysData.measure.passFail.lineNowShiftXOrY == 0)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("Freq"));
			rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
			if (sysData.measure.passFail.lineNowShiftXOrY == 1)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Ampt"));
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_SHIFTXY_GHZ:
			if (sysData.measure.passFail.lineNowShiftXOrY == 0)
			{
				p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			} else
			{
				p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, "dBm");
			}
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_SHIFTXY_MHZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_SHIFTXY_KHZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_SHIFTXY_HZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_LINE_RETURN:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_AMPT_PASSFAIL_RETURN:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;

			//带宽键
		case KEY_FUNC_BW_RBW:
			if (sysData.emc.emcOn)
			{
				rows = 2;
				rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
				p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
				rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
				p.setFont(fontData);
				p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, ((QString) getFreqAndUnitString(sysData.bw.rbw, 1, tempChar)).trimmed());
			} else
			{
				rows = 3;
				rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
				p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
				rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
				p.setFont(fontData);
				p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, ((QString) getFreqAndUnitString(sysData.bw.rbw, 1, tempChar)).trimmed());
				rc = QRect(sep, rc.top() + rc.height(), (width - 2 * sep) / 2, rc.height() + enSep);
				if (sysData.bw.rbwAuto)
					p.setFont(fontUnderline);
				else
					p.setFont(fontLight);
				p.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("Auto"));
				rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
				if (!sysData.bw.rbwAuto)
					p.setFont(fontUnderline);
				else
					p.setFont(fontLight);
				p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Man"));
			}
			break;
		case KEY_FUNC_BW_RBW_GHZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_BW_RBW_MHZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_BW_RBW_KHZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_BW_RBW_HZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_BW_RBWMODE:
			key->isEnabled = !sysData.emc.emcOn;
			if (!key->isEnabled)
				p.setPen(BUTTONDISABLECOLOR);
			else
				p.setPen(BUTTONENABLECOLOR);
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (!sysData.bw.rbwStepCont)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("Def"));
			rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
			if (sysData.bw.rbwStepCont)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Cont"));
			break;
		case KEY_FUNC_BW_VBW:
			rows = 3;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, ((QString) getFreqAndUnitString(sysData.bw.vbw, 1, tempChar)).trimmed());
			rc = QRect(sep, rc.top() + rc.height(), (width - 2 * sep) / 2, rc.height() + enSep);
			if (sysData.bw.vbwAuto)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("Auto"));
			rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
			if (!sysData.bw.vbwAuto)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Man"));
			break;
		case KEY_FUNC_BW_VBW_GHZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_BW_VBW_MHZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_BW_VBW_KHZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_BW_VBW_HZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_BW_VBWMODE:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (!sysData.bw.vbwStepCont)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("Def"));
			rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
			if (sysData.bw.vbwStepCont)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Cont"));
			break;
		case KEY_FUNC_BW_AVERAGE:
			rows = 3;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, intToString(sysData.bw.bwAverage, tempChar));
			rc = QRect(sep, rc.top() + rc.height(), (width - 2 * sep) / 2, rc.height() + enSep);
			if (sysData.bw.bwAverageOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("On"));
			rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
			if (!sysData.bw.bwAverageOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Off"));
			break;
		case KEY_FUNC_BW_AVERAGE_ENTER:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_BW_EMC:
			rows = 1;
			p.drawText(QRect(sep - 2, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(width - sep - pngSubSize, (height - 16) / 2, 16, 16);
			p.drawPixmap(rc, img_Arrow);
			break;
		case KEY_FUNC_BW_EMC_VALUE:
			rows = 2;
			rc = QRect(0, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width(), rc.height());
			p.setFont(fontData);

			if (sysData.emc.emcOn)
			{
				if ((unsigned long) sysData.bw.rbw == 1e6)
				{
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, "1MHz");
				} else if ((unsigned long) sysData.bw.rbw == 120e3)
				{
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, "120kHz");
				} else if ((unsigned long) sysData.bw.rbw == 30e3)
				{
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, "30kHz");
				} else if ((unsigned long) sysData.bw.rbw == 9e3)
				{
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, "9kHz");
				} else if ((unsigned long) sysData.bw.rbw == 200)
				{
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, "200Hz");
				} else
				{
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Off"));
				}
			} else
			{
				p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Off"));
			}

			rc = QRect(width - sep - pngSubSize, (height - 16) / 2, 16, 16);
			if (!key->isEnabled)
				p.drawPixmap(rc, img_ArrowHui);
			else
				p.drawPixmap(rc, img_Arrow);
			break;
		case KEY_FUNC_BW_EMC_EMC:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.emc.emcOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("On"));
			rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
			if (!sysData.emc.emcOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Off"));
			break;
		case KEY_FUNC_BW_EMC_1MHZ:
			key->isEnabled = sysData.emc.emcOn;
			if (!key->isEnabled)
				p.setPen(BUTTONDISABLECOLOR);
			else
				p.setPen(BUTTONENABLECOLOR);
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_BW_EMC_120KHZ:
			key->isEnabled = sysData.emc.emcOn;
			if (!key->isEnabled)
				p.setPen(BUTTONDISABLECOLOR);
			else
				p.setPen(BUTTONENABLECOLOR);
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_BW_EMC_30KHZ:
			key->isEnabled = sysData.emc.emcOn;
			if (!key->isEnabled)
				p.setPen(BUTTONDISABLECOLOR);
			else
				p.setPen(BUTTONENABLECOLOR);
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_BW_EMC_9KHZ:
			key->isEnabled = sysData.emc.emcOn;
			if (!key->isEnabled)
				p.setPen(BUTTONDISABLECOLOR);
			else
				p.setPen(BUTTONENABLECOLOR);
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_BW_EMC_200HZ:
			key->isEnabled = sysData.emc.emcOn;
			if (!key->isEnabled)
				p.setPen(BUTTONDISABLECOLOR);
			else
				p.setPen(BUTTONENABLECOLOR);
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_BW_EMC_RETURN:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;

			//Sweep
		case KEY_FUNC_SWEEP_SWEEPTIME:
			rows = 3;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, ((QString) getMsTimeAndUnitString(sysData.sweep.sweepTime, tempChar)).trimmed());
			rc = QRect(sep, rc.top() + rc.height(), (width - 2 * sep) / 2, rc.height() + enSep);
			if (sysData.sweep.sweepTimeAuto)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("Auto"));
			rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
			if (!sysData.sweep.sweepTimeAuto)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Man"));
			break;
		case KEY_FUNC_SWEEP_SWEEPTIME_S:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SWEEP_SWEEPTIME_MS:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SWEEP_SWEEPTIME_US:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SWEEP_SWEEPTIME_NS:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SWEEP_SWEEPMODE_SINGLE:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SWEEP_SWEEPMODE_CONTINUOUS:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SWEEP_SWEEPPOINTS:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, intToString(sysData.sweep.sweepPoints, tempChar));
			break;
		case KEY_FUNC_SWEEP_SWEEPPOINTS_ENTER:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;

			//Trig
		case KEY_FUNC_TRIG_AUTORUN:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_TRIG_VIDEO:
			key->isEnabled = (sysData.span.span == 0);
			if (!key->isEnabled)
				p.setPen(BUTTONDISABLECOLOR);
			else
				p.setPen(BUTTONENABLECOLOR);
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, ((QString) sysScpi->getAmptAndUnitString(sysData.trig.power, tempChar, 0)).trimmed());
			//p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;

		case KEY_FUNC_TRIG_VIDEO_DBM:
		//	p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			switch (sysData.ampt.refUnit)
			{
				case rudBm:
					p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, "dBm");
					break;
				case rudBmV:
					p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, "dBmV");
					break;
				case rudBuV:
					p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, "dBuV");
					break;
				case ruW:
					p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, "mW");
					break;
				case ruV:
					p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, "mV");
					break;
				case rudBpW:
					p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, "dBpW");
					break;
				case rudBuW:
					p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, "dBuW");
					break;
			}
			break;
		case KEY_FUNC_TRIG_EXTERNAL:
			if (sysData.trig.mode == tgRise || sysData.trig.mode == tgFall)
			{
				rows = 2;
				rc = QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows);
				p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
				rc = QRect(rc.left(), rc.top() + rc.height(), rc.width(), rc.height());
				p.setFont(fontData);
				if (sysData.trig.mode == tgRise)
				{
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Rising edge"));
				} else
				{
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Falling edge"));
				}
			} else
			{
				rows = 1;
				p.drawText(QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			}
			rc = QRect(width - sep - pngSubSize, (height - 16) / 2, 16, 16);
			if (!key->isEnabled)
				p.drawPixmap(rc, img_ArrowHui);
			else
				p.drawPixmap(rc, img_Arrow);
			break;
		case KEY_FUNC_TRIG_EXTERNAL_FALL:
		case KEY_FUNC_TRIG_EXTERNAL_RISE:
		case KEY_FUNC_TRIG_EXTERNAL_RETURN:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;

			//meas
		case KEY_FUNC_MEAS_MEASUREOFF:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_MEAS_TIMESPEC:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.measure.isTimeSpectrumOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("On"));
			rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
			if (!sysData.measure.isTimeSpectrumOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Off"));
			break;
		case KEY_FUNC_MEAS_ACPR:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.measure.isAcprOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("On"));
			rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
			if (!sysData.measure.isAcprOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Off"));
			break;
		case KEY_FUNC_MEAS_CHANNELPOWER:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.measure.isChannelPowerOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("On"));
			rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
			if (!sysData.measure.isChannelPowerOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Off"));
			break;
		case KEY_FUNC_MEAS_OBW:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.measure.isObwOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("On"));
			rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
			if (!sysData.measure.isObwOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Off"));
			break;
			//Meas Setup
		case KEY_FUNC_MEASSETUP_CHANNELWIDTH:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, ((QString) getFreqAndUnitString(sysData.measureSetup.channelWidth, 1, tempChar)).trimmed());
			break;
		case KEY_FUNC_MEASSETUP_CHANNELWIDTH_GHZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_MEASSETUP_CHANNELWIDTH_MHZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_MEASSETUP_CHANNELWIDTH_KHZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_MEASSETUP_CHANNELWIDTH_HZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_MEASSETUP_CHANNELINTERVAL:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, ((QString) getFreqAndUnitString(sysData.measureSetup.channelSpace, 1, tempChar)).trimmed());
			break;
		case KEY_FUNC_MEASSETUP_CHANNELINTERVAL_GHZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_MEASSETUP_CHANNELINTERVAL_MHZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_MEASSETUP_CHANNELINTERVAL_KHZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_MEASSETUP_CHANNELINTERVAL_HZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_MEASSETUP_CHANNELNUMS:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, QString(intToString(sysData.measureSetup.channelNums, tempChar)).trimmed());
			break;
		case KEY_FUNC_MEASSETUP_CHANNELNUMS_ENTER:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_MEASSETUP_POWERBWPERCENT:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, QString(floatToStringDot2(sysData.measureSetup.powerPercent, tempChar)) + "%");
			break;
		case KEY_FUNC_MEASSETUP_POWERBWPERCENT_ENTER:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_MEAS_DISCRIMINATOR:
			p.drawText(QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(width - sep - pngSubSize, (height - 16) / 2, 16, 16);
			if (!key->isEnabled)
				p.drawPixmap(rc, img_ArrowHui);
			else
				p.drawPixmap(rc, img_Arrow);
			break;

		case KEY_FUNC_MEAS_DISCRIMINATOR_DISCRIMINATOR:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.disc.isDiscOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("On"));
			rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
			if (!sysData.disc.isDiscOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Off"));
			break;
		case KEY_FUNC_MEAS_DISCRIMINATOR_NORMALIZE:
			key->isEnabled = sysData.disc.isDiscOn;
			if (!key->isEnabled)
				p.setPen(BUTTONDISABLECOLOR);
			else
				p.setPen(BUTTONENABLECOLOR);

			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_MEAS_DISCRIMINATOR_AMPLITUDE:
			key->isEnabled = sysData.disc.isDiscOn;
			if (!key->isEnabled)
				p.setPen(BUTTONDISABLECOLOR);
			else
				p.setPen(BUTTONENABLECOLOR);

			p.drawText(QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(width - sep - pngSubSize, (height - 16) / 2, 16, 16);
			if (!key->isEnabled)
				p.drawPixmap(rc, img_ArrowHui);
			else
				p.drawPixmap(rc, img_Arrow);
			break;
		case KEY_FUNC_MEAS_DISCRIMINATOR_AMPLITUDE_OUTPUTPOWER:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, QString(floatToStringDot2(sysData.disc.outputPower, tempChar)).trimmed() + " dBm");
			break;
		case KEY_FUNC_MEAS_DISCRIMINATOR_AMPLITUDE_OUTPUTPOWER_ENTER:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_MEAS_DISCRIMINATOR_AMPLITUDE_INPUTGAIN:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width(), rc.height());
			p.setFont(fontData);
			switch (sysData.disc.inputGain)
			{
				case igtx1:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, "×1");
					break;
				case igtx2:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, "×2");
					break;
				case igtx4:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, "×4");
					break;
				case igtx05:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, "×0.5");
					break;
				case igtx025:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, "×0.25");
					break;
				default:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, "unKnown");
					break;
			}
			rc = QRect(width - sep - pngSubSize, (height - 16) / 2, 16, 16);
			if (!key->isEnabled)
				p.drawPixmap(rc, img_ArrowHui);
			else
				p.drawPixmap(rc, img_Arrow);
			break;
		case KEY_FUNC_MEAS_DISCRIMINATOR_AMPLITUDE_INPUTGAIN_X1:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_MEAS_DISCRIMINATOR_AMPLITUDE_INPUTGAIN_X2:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_MEAS_DISCRIMINATOR_AMPLITUDE_INPUTGAIN_X4:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_MEAS_DISCRIMINATOR_AMPLITUDE_INPUTGAIN_X05:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_MEAS_DISCRIMINATOR_AMPLITUDE_INPUTGAIN_X025:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_MEAS_DISCRIMINATOR_AMPLITUDE_INPUTGAIN_RETURN:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_MEAS_DISCRIMINATOR_AMPLITUDE_SCALEDIV:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, QString(floatToStringDot2(sysData.disc.scaleDiv, tempChar)).trimmed() + " v/div");
			break;
		case KEY_FUNC_MEAS_DISCRIMINATOR_AMPLITUDE_SCALEDIV_ENTER:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_MEAS_DISCRIMINATOR_AMPLITUDE_OFFSET:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, QString(floatToStringDot2(sysData.disc.dcOffset, tempChar)).trimmed() + " V");
			break;
		case KEY_FUNC_MEAS_DISCRIMINATOR_AMPLITUDE_OFFSET_ENTER:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_MEAS_DISCRIMINATOR_AMPLITUDE_RETURN:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_MEAS_DISCRIMINATOR_SWEEPTIME:
			key->isEnabled = sysData.disc.isDiscOn;
			if (!key->isEnabled)
				p.setPen(BUTTONDISABLECOLOR);
			else
				p.setPen(BUTTONENABLECOLOR);

			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, ((QString) getMsTimeAndUnitString(sysData.disc.sweepTime, tempChar)).trimmed());
			break;
		case KEY_FUNC_MEAS_DISCRIMINATOR_SWEEPTIME_S:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_MEAS_DISCRIMINATOR_SWEEPTIME_MS:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_MEAS_DISCRIMINATOR_SWEEPTIME_US:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_MEAS_DISCRIMINATOR_SWEEPTIME_NS:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_MEAS_DISCRIMINATOR_RETURN:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;

		case KEY_FUNC_MEAS_EMC:
			p.drawText(QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(width - sep - pngSubSize, (height - 16) / 2, 16, 16);
			if (!key->isEnabled)
				p.drawPixmap(rc, img_ArrowHui);
			else
				p.drawPixmap(rc, img_Arrow);
			break;

			//Trace
		case KEY_FUNC_TRACE_TRACE:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / TRACECOUNT, rc.height());

			for (int i = 0; i < TRACECOUNT; i++)
			{
				if (sysData.traceSelIndex == i)
				{
					p.setFont(fontUnderline);
				} else
				{
					p.setFont(fontData);
				}

				if (i > 0)
				{
					rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
				}

				p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, intToString(i + 1, tempChar));
			}
			break;
		case KEY_FUNC_TRACE_CLEARWRITE:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_TRACE_MAXHOLD:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_TRACE_MINHOLD:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_TRACE_VIEW:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_TRACE_BLANK:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_TRACE_OPERATIONS:
			p.drawText(QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(width - sep - pngSubSize, (height - 16) / 2, 16, 16);
			if (!key->isEnabled)
				p.drawPixmap(rc, img_ArrowHui);
			else
				p.drawPixmap(rc, img_Arrow);
			break;
		case KEY_FUNC_TRACE_OPERATIONS_12:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_TRACE_OPERATIONS_2DLTO2:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_TRACE_OPERATIONS_23:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_TRACE_OPERATIONS_1TO3:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_TRACE_OPERATIONS_2TO3:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_TRACE_OPERATIONS_RETURN:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;

			//Detector
		case KEY_FUNC_DETECTOR_AUTO:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_DETECTOR_NORMAL:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_DETECTOR_POSPEAK:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_DETECTOR_NEGPEAK:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_DETECTOR_SAMPLE:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_DETECTOR_TRANSIENT:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;

			//Marker
		case KEY_FUNC_MARKER_MARKER:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / MARKERCOUNT, rc.height());

			for (int i = 0; i < MARKERCOUNT; i++)
			{
				if (sysData.markerSelIndex == i)
				{
					p.setFont(fontUnderline);
				} else
				{
					p.setFont(fontData);
				}

				if (i > 0)
				{
					rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
				}

				p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, intToString(i + 1, tempChar));
			}
			break;
		case KEY_FUNC_MARKER_TRACE:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / TRACECOUNT, rc.height());

			for (int i = 0; i < TRACECOUNT; i++)
			{
				if (sysData.marker[sysData.markerSelIndex].traceIndex == i)
				{
					p.setFont(fontUnderline);
				} else
				{
					p.setFont(fontData);
				}

				if (i > 0)
				{
					rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
				}

				p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, intToString(i + 1, tempChar));
			}
			break;
		case KEY_FUNC_MARKER_NORMAL:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_MARKER_NORMAL_GHZ:
			if (sysData.span.span > 0)
			{
				p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			} else
			{
				p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, "s");
			}
			break;
		case KEY_FUNC_MARKER_NORMAL_MHZ:
			if (sysData.span.span > 0)
			{
				p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			} else
			{
				p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, "ms");
			}
			break;
		case KEY_FUNC_MARKER_NORMAL_KHZ:
			if (sysData.span.span > 0)
			{
				p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			} else
			{
				p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, "us");
			}
			break;
		case KEY_FUNC_MARKER_NORMAL_HZ:
			if (sysData.span.span > 0)
			{
				p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			} else
			{
				p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, "ns");
			}
			break;
		case KEY_FUNC_MARKER_DELTA:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_MARKER_DELTA_GHZ:
			if (sysData.span.span > 0)
			{
				p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			} else
			{
				p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, "s");
			}
			break;
		case KEY_FUNC_MARKER_DELTA_MHZ:
			if (sysData.span.span > 0)
			{
				p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			} else
			{
				p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, "ms");
			}
			break;
		case KEY_FUNC_MARKER_DELTA_KHZ:
			if (sysData.span.span > 0)
			{
				p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			} else
			{
				p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, "us");
			}
			break;
		case KEY_FUNC_MARKER_DELTA_HZ:
			if (sysData.span.span > 0)
			{
				p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			} else
			{
				p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, "ns");
			}
			break;
		case KEY_FUNC_MARKER_OFF:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_MARKER_ALLOFF:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;

		case KEY_FUNC_MARKER_LIST:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.markerTableOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("On"));
			rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
			if (!sysData.markerTableOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Off"));
			break;

			//Marker Fctn
		case KEY_FUNC_MARKERFCTN_OFF:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_MARKERFCTN_NDB:
			rows = 3;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, ((QString) floatToStringDot2(sysData.markerFctn.ndBValue, tempChar)).trimmed());
			rc = QRect(sep, rc.top() + rc.height(), (width - 2 * sep) / 2, rc.height() + enSep);
			if (sysData.markerFctn.isNdBOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("On"));
			rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
			if (!sysData.markerFctn.isNdBOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Off"));
			break;
		case KEY_FUNC_MARKERFCTN_NDB_ENTER:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_MARKERFCTN_NOISE:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.markerFctn.isMkrNoiseOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("On"));
			rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
			if (!sysData.markerFctn.isMkrNoiseOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Off"));
			break;
		case KEY_FUNC_MARKERFCTN_FREQCOUNT:
			p.drawText(QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(width - sep - pngSubSize, (height - 16) / 2, 16, 16);
			if (!key->isEnabled)
				p.drawPixmap(rc, img_ArrowHui);
			else
				p.drawPixmap(rc, img_Arrow);
			break;
		case KEY_FUNC_MARKERFCTN_FREQCOUNT_FREQCOUNT:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.markerFctn.isFreqCountOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("On"));
			rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
			if (!sysData.markerFctn.isFreqCountOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Off"));
			break;
		case KEY_FUNC_MARKERFCTN_FREQCOUNT_BW:
			key->isEnabled = sysData.markerFctn.isFreqCountOn;
			if (!key->isEnabled)
				p.setPen(BUTTONDISABLECOLOR);
			else
				p.setPen(BUTTONENABLECOLOR);

			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, ((QString) getFreqAndUnitString(sysData.markerFctn.freqCountBw, 0, tempChar)).trimmed());
			break;
		case KEY_FUNC_MARKERFCTN_FREQCOUNT_BW_GHZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_MARKERFCTN_FREQCOUNT_BW_MHZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_MARKERFCTN_FREQCOUNT_BW_KHZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_MARKERFCTN_FREQCOUNT_BW_HZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_MARKERFCTN_FREQCOUNT_BW_RETURN:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_MARKERFCTN_FREQCOUNT_RETURN:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;

			//MarkerTo
		case KEY_FUNC_MARKERTO_CENTER:
			p.drawText(QRect(0, sep, width - sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_MARKERTO_CFSTEP:
			p.drawText(QRect(0, sep, width - sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_MARKERTO_START:
			p.drawText(QRect(0, sep, width - sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_MARKERTO_STOP:
			p.drawText(QRect(0, sep, width - sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_MARKERTO_REFLEVEL:
			p.drawText(QRect(0, sep, width - sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_MARKERTO_DELTATOSPAN:
			p.drawText(QRect(0, sep, width - sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_MARKERTO_DELTATOCF:
			p.drawText(QRect(0, sep, width, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;

			//Peak
		case KEY_FUNC_PEAK_MAXSEARCH:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_PEAK_NEXTPEAK:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_PEAK_LEFTPEAK:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_PEAK_RIGHTPEAK:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_PEAK_MINSEARCH:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_PEAK_MKRTOCF:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_PEAK_AUTOMAXSEARCH:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.isAutoPeakMaxing)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("On"));
			rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
			if (!sysData.isAutoPeakMaxing)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Off"));
			break;

			//System
		case KEY_FUNC_SYSTEM_SYSTEM:
			p.drawText(QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(width - sep - pngSubSize, (height - 16) / 2, 16, 16);
			if (!key->isEnabled)
				p.drawPixmap(rc, img_ArrowHui);
			else
				p.drawPixmap(rc, img_Arrow);
			break;
		case KEY_FUNC_SYSTEM_SYSTEM_INFO:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_SYSTEM_LOG:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_SYSTEM_RETURN:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO:
			p.drawText(QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(width - sep - pngSubSize, (height - 16) / 2, 16, 16);
			if (!key->isEnabled)
				p.drawPixmap(rc, img_ArrowHui);
			else
				p.drawPixmap(rc, img_Arrow);
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_LAN:
			p.drawText(QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(width - sep - pngSubSize, (height - 16) / 2, 16, 16);
			if (!key->isEnabled)
				p.drawPixmap(rc, img_ArrowHui);
			else
				p.drawPixmap(rc, img_Arrow);
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_LAN_IP:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, sysData.system.io.lan.ip);
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_LAN_IP_ENTER:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_LAN_MASK:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, sysData.system.io.lan.mask);
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_LAN_MASK_ENTER:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_LAN_GATE:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, sysData.system.io.lan.gate);
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_LAN_GATE_ENTER:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_LAN_RESET:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_LAN_RETURN:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_GPIB:
			key->isEnabled = sysData.system.isGpibOn;
			if (!key->isEnabled)
				p.setPen(BUTTONDISABLECOLOR);
			else
				p.setPen(BUTTONENABLECOLOR);

			p.drawText(QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(width - sep - pngSubSize, (height - 16) / 2, 16, 16);
			if (!key->isEnabled)
				p.drawPixmap(rc, img_ArrowHui);
			else
				p.drawPixmap(rc, img_Arrow);
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_GPIB_ADDR:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, intToString(sysData.system.io.gpib.addr, tempChar));
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_GPIB_ADDR_ENTER:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_GPIB_RETURN:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_COM:
			p.drawText(QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(width - sep - pngSubSize, (height - 16) / 2, 16, 16);
			if (!key->isEnabled)
				p.drawPixmap(rc, img_ArrowHui);
			else
				p.drawPixmap(rc, img_Arrow);
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_COM_BAUDRATE:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width(), rc.height());
			p.setFont(fontData);
			switch (sysData.system.io.com.baudrate)
			{
				case br1200:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, "1200");
					break;
				case br2400:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, "2400");
					break;
				case br4800:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, "4800");
					break;
				case br9600:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, "9600");
					break;
				case br19200:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, "19200");
					break;
				case br38400:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, "38400");
					break;
				case br57600:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, "57600");
					break;
				case br115200:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, "115200");
					break;
				default:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, "unKnown");
					break;
			}
			rc = QRect(width - sep - pngSubSize, (height - 16) / 2, 16, 16);
			if (!key->isEnabled)
				p.drawPixmap(rc, img_ArrowHui);
			else
				p.drawPixmap(rc, img_Arrow);
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_COM_BAUDRATE_1200:
		case KEY_FUNC_SYSTEM_CONFIGIO_COM_BAUDRATE_2400:
		case KEY_FUNC_SYSTEM_CONFIGIO_COM_BAUDRATE_4800:
		case KEY_FUNC_SYSTEM_CONFIGIO_COM_BAUDRATE_9600:
		case KEY_FUNC_SYSTEM_CONFIGIO_COM_BAUDRATE_19200:
		case KEY_FUNC_SYSTEM_CONFIGIO_COM_BAUDRATE_38400:
		case KEY_FUNC_SYSTEM_CONFIGIO_COM_BAUDRATE_57600:
		case KEY_FUNC_SYSTEM_CONFIGIO_COM_BAUDRATE_115200:
		case KEY_FUNC_SYSTEM_CONFIGIO_COM_BAUDRATE_RETURN:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_COM_DATABITS:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width(), rc.height());
			p.setFont(fontData);
			switch (sysData.system.io.com.databits)
			{
				case db5:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, "5");
					break;
				case db6:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, "6");
					break;
				case db7:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, "7");
					break;
				case db8:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, "8");
					break;
				default:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, "unKnown");
					break;
			}
			rc = QRect(width - sep - pngSubSize, (height - 16) / 2, 16, 16);
			if (!key->isEnabled)
				p.drawPixmap(rc, img_ArrowHui);
			else
				p.drawPixmap(rc, img_Arrow);
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_COM_DATABITS_5:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_COM_DATABITS_6:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_COM_DATABITS_7:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_COM_DATABITS_8:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_COM_DATABITS_RETURN:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_COM_STOPBITS:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width(), rc.height());
			p.setFont(fontData);
			switch (sysData.system.io.com.stopbits)
			{
				case sb1:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, "1");
					break;
				case sb2:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, "2");
					break;
				default:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, "unKnown");
					break;
			}
			rc = QRect(width - sep - pngSubSize, (height - 16) / 2, 16, 16);
			if (!key->isEnabled)
				p.drawPixmap(rc, img_ArrowHui);
			else
				p.drawPixmap(rc, img_Arrow);
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_COM_STOPBITS_1:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_COM_STOPBITS_2:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_COM_STOPBITS_RETURN:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_COM_CHECK:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width(), rc.height());
			p.setFont(fontData);
			switch (sysData.system.io.com.check)
			{
				case cmEven:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Even"));
					break;
				case cmOdd:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Odd"));
					break;
				case cmNone:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("None"));
					break;
				case cmSpace:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Space"));
					break;
				default:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, "unKnown");
					break;
			}
			rc = QRect(width - sep - pngSubSize, (height - 16) / 2, 16, 16);
			if (!key->isEnabled)
				p.drawPixmap(rc, img_ArrowHui);
			else
				p.drawPixmap(rc, img_Arrow);
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_COM_CHECK_EVEN:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_COM_CHECK_ODD:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_COM_CHECK_NONE:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_COM_CHECK_SPACE:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_COM_CHECK_RETURN:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_COM_FLOWCONTROL:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width(), rc.height());
			p.setFont(fontData);
			switch (sysData.system.io.com.flow)
			{
				case fcXonXoff:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Xon/Xoff"));
					break;
				case fcHardware:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Hardware"));
					break;
				case fcNone:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("None"));
					break;
				default:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, "unKnown");
					break;
			}
			rc = QRect(width - sep - pngSubSize, (height - 16) / 2, 16, 16);
			if (!key->isEnabled)
				p.drawPixmap(rc, img_ArrowHui);
			else
				p.drawPixmap(rc, img_Arrow);
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_COM_FLOWCONTROL_XONXOFF:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_COM_FLOWCONTROL_HARDWARE:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_COM_FLOWCONTROL_NONE:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_COM_FLOWCONTROL_RETURN:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_COM_RESET:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_COM_RETURN:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_CONFIGIO_RETURN:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_POWERONPRESET:
			p.drawText(QRect(0, sep, width - sep - pngSubSize, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(width - sep - pngSubSize, (height - 16) / 2, 16, 16);
			if (!key->isEnabled)
				p.drawPixmap(rc, img_ArrowHui);
			else
				p.drawPixmap(rc, img_Arrow);
			break;
		case KEY_FUNC_SYSTEM_POWERONPRESET_POWERON:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width(), rc.height());
			p.setFont(fontData);

			switch (sysData.system.powerOnPreset.powerOnParam)
			{
				case spFactory:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Factory"));
					break;
				case spUser:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("User"));
					break;
				case spPowerOff:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Power Off"));
					break;
				default:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("unKnown"));
					break;
			}
			rc = QRect(width - sep - pngSubSize, (height - 16) / 2, 16, 16);
			if (!key->isEnabled)
				p.drawPixmap(rc, img_ArrowHui);
			else
				p.drawPixmap(rc, img_Arrow);
			break;
		case KEY_FUNC_SYSTEM_POWERONPRESET_POWERON_FACTORY:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_POWERONPRESET_POWERON_USER:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_POWERONPRESET_POWERON_POWEROFF:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_POWERONPRESET_POWERON_RETURN:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_POWERONPRESET_PRESET:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width(), rc.height());
			p.setFont(fontData);
			switch (sysData.system.powerOnPreset.presetParam)
			{
				case spFactory:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Factory"));
					break;
				case spUser:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("User"));
					break;
				case spPowerOff:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Power Off"));
					break;
				default:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("unKnown"));
					break;
			}
			rc = QRect(width - sep - pngSubSize, (height - 16) / 2, 16, 16);
			if (!key->isEnabled)
				p.drawPixmap(rc, img_ArrowHui);
			else
				p.drawPixmap(rc, img_Arrow);
			break;
		case KEY_FUNC_SYSTEM_POWERONPRESET_PRESET_FACTORY:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_POWERONPRESET_PRESET_USER:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_POWERONPRESET_PRESET_POWEROFF:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_POWERONPRESET_PRESET_RETURN:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_POWERONPRESET_RETURN:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_LANGUAGE:
			p.drawText(QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(width - sep - pngSubSize, (height - 16) / 2, 16, 16);
			if (!key->isEnabled)
				p.drawPixmap(rc, img_ArrowHui);
			else
				p.drawPixmap(rc, img_Arrow);
			break;
		case KEY_FUNC_SYSTEM_LANGUAGE_CHINESE:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_LANGUAGE_ENGLISH:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_LANGUAGE_RETURN:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;

		case KEY_FUNC_SYSTEM_DATETIME:
			p.drawText(QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(width - sep - pngSubSize, (height - 16) / 2, 16, 16);
			if (!key->isEnabled)
				p.drawPixmap(rc, img_ArrowHui);
			else
				p.drawPixmap(rc, img_Arrow);
			break;
		case KEY_FUNC_SYSTEM_DATETIME_DATETIME:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.system.datetime.isVisible)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("On"));
			rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
			if (!sysData.system.datetime.isVisible)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Off"));
			break;
		case KEY_FUNC_SYSTEM_DATETIME_FORMAT:
			p.drawText(QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(width - sep - pngSubSize, (height - 16) / 2, 16, 16);
			if (!key->isEnabled)
				p.drawPixmap(rc, img_ArrowHui);
			else
				p.drawPixmap(rc, img_Arrow);
			break;
		case KEY_FUNC_SYSTEM_DATETIME_FORMAT_YMDHMS:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_DATETIME_FORMAT_HMSYMD:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_DATETIME_FORMAT_RETURN:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_DATETIME_DATE:
			sysScpi->getSystemDateTime();
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, sysData.system.datetime.date);
			break;
		case KEY_FUNC_SYSTEM_DATETIME_DATE_ENTER:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_DATETIME_TIME:
			sysScpi->getSystemDateTime();
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, sysData.system.datetime.time);
			break;
		case KEY_FUNC_SYSTEM_DATETIME_TIME_ENTER:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_DATETIME_RETURN:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_CHANNELSELECT:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.system.isInternalSignalOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("Cal Int"));
			rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
			if (!sysData.system.isInternalSignalOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Rf In"));
			break;
		case KEY_FUNC_SYSTEM_CALIBRATION:
			p.drawText(QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(width - sep - pngSubSize, (height - 16) / 2, 16, 16);
			if (!key->isEnabled)
				p.drawPixmap(rc, img_ArrowHui);
			else
				p.drawPixmap(rc, img_Arrow);
			break;
		case KEY_FUNC_SYSTEM_CALIBRATION_CALIBRATION:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_CALIBRATION_FACTORY:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_CALIBRATION_RETURN:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_SERVICE:
			p.drawText(QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(width - sep - pngSubSize, (height - 16) / 2, 16, 16);
			if (!key->isEnabled)
				p.drawPixmap(rc, img_ArrowHui);
			else
				p.drawPixmap(rc, img_Arrow);
			break;

		case KEY_FUNC_SYSTEM_SERVICE_LANMACADDR:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);

			rcNums = sizeof sysData.system.lanMacAddress / sizeof sysData.system.lanMacAddress[0];
			rcWidthFloat = rc.width() / rcNums;
			rc = QRect(rc.right() - rcWidthFloat, rc.top() + rc.height(), rcWidthFloat, rc.height());

			for (int i = sizeof sysData.system.lanMacAddress / sizeof sysData.system.lanMacAddress[0] - 1; i >= 0; i--)
			{
				if (sysData.system.lanMacAddrIndex == i)
				{
					p.setFont(fontUnderline);
				} else
				{
					p.setFont(fontData);
				}

				if (i < sizeof sysData.system.lanMacAddress / sizeof sysData.system.lanMacAddress[0] - 1)
				{
					rc = QRect(rc.left() - rc.width(), rc.top(), rc.width(), rc.height());
				}

				p.drawText(rc, Qt::AlignCenter, intToHex(sysData.system.lanMacAddress[i], tempChar));
			}
			break;

		case KEY_FUNC_SYSTEM_SERVICE_SERIALNO:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, QString(sysData.system.serialNo).trimmed());
			break;
		case KEY_FUNC_SYSTEM_SERVICE_SERIALNO_ENTER:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_SERVICE_CALIBRATION:
			p.drawText(QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(width - sep - pngSubSize, (height - 16) / 2, 16, 16);
			if (!key->isEnabled)
				p.drawPixmap(rc, img_ArrowHui);
			else
				p.drawPixmap(rc, img_Arrow);
			break;

		case KEY_FUNC_SYSTEM_SERVICE_CALIBRATION_DELETE:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_SERVICE_CALIBRATION_EDIT:
			p.drawText(QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(width - sep - pngSubSize, (height - 16) / 2, 16, 16);
			if (!key->isEnabled)
				p.drawPixmap(rc, img_ArrowHui);
			else
				p.drawPixmap(rc, img_Arrow);
			break;
		case KEY_FUNC_SYSTEM_SERVICE_CALIBRATION_EDIT_9K:
			key->isEnabled = sysData.factoryCalData.isCaled;
			if (!key->isEnabled)
				p.setPen(BUTTONDISABLECOLOR);
			else
				p.setPen(BUTTONENABLECOLOR);

			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, QString(floatToStringDot2(sysData.factoryCalData.freqResp[0][0], tempChar)).trimmed());
			break;
		case KEY_FUNC_SYSTEM_SERVICE_CALIBRATION_EDIT_9K_ENTER:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;

		case KEY_FUNC_SYSTEM_SERVICE_CALIBRATION_EDIT_9K_RESET:
			key->isEnabled = sysData.factoryCalData.isCaled;
			if (!key->isEnabled)
				p.setPen(BUTTONDISABLECOLOR);
			else
				p.setPen(BUTTONENABLECOLOR);

			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_SERVICE_CALIBRATION_EDIT_20K:
			key->isEnabled = sysData.factoryCalData.isCaled;
			if (!key->isEnabled)
				p.setPen(BUTTONDISABLECOLOR);
			else
				p.setPen(BUTTONENABLECOLOR);

			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, QString(floatToStringDot2(sysData.factoryCalData.freqResp[0][1], tempChar)).trimmed());
			break;
		case KEY_FUNC_SYSTEM_SERVICE_CALIBRATION_EDIT_20K_ENTER:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;

		case KEY_FUNC_SYSTEM_SERVICE_CALIBRATION_EDIT_20K_RESET:
			key->isEnabled = sysData.factoryCalData.isCaled;
			if (!key->isEnabled)
				p.setPen(BUTTONDISABLECOLOR);
			else
				p.setPen(BUTTONENABLECOLOR);

			p.drawText(QRect(0, sep, width, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_SERVICE_CALIBRATION_EDIT_RETURN:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_SERVICE_CALIBRATION_CALIBRATION:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_SERVICE_CALIBRATION_PRECALIBRATION:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_SERVICE_CALIBRATION_LFCALIBRATION:
			key->isEnabled = sysData.options.directChannelOptionOn;
			if (!key->isEnabled)
				p.setPen(BUTTONDISABLECOLOR);
			else
				p.setPen(BUTTONENABLECOLOR);
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_SERVICE_CALIBRATION_MODE:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);

			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.isComCalbration)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("Com"));
			rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
			if (!sysData.isComCalbration)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Lan"));
			break;

		case KEY_FUNC_SYSTEM_SERVICE_CALIBRATION_RETURN:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_SERVICE_EVALUATE:
			rows = 2;
			if (sysData.system.isLanguageEn)
			{
				rc = QRect(sep, sep, width - sep, (height - 2 * sep) / rows);
			} else
			{
				rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			}

			if (sysScpi->eva.isEvaluate)
			{
				p.setPen(Qt::red);
				sysScpi->getSystemServiceOfEvaluteSYDays();

				if (sysData.system.isLanguageEn)
				{
					if ((sysScpi->eva.syDays / 24) >= 0)
					{
						p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, "(Remain " + QString(intToString(sysScpi->eva.syDays / 24, tempChar)).trimmed() + "Days)");
					} else
					{
						p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, "(Past Due " + QString(intToString(qAbs(sysScpi->eva.syDays / 24), tempChar)).trimmed() + "Days)");
					}
				} else
				{
					if ((sysScpi->eva.syDays / 24) >= 0)
					{
						p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName + "(剩余" + QString(intToString(sysScpi->eva.syDays / 24, tempChar)).trimmed() + "天)");
					} else
					{
						p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName + "(已过期" + QString(intToString(qAbs(sysScpi->eva.syDays / 24), tempChar)).trimmed() + "天)");
					}
				}
			} else
			{
				p.setPen(Qt::black);
				p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			}

			p.setPen(Qt::black);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysScpi->eva.isEvaluate)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("On"));
			rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
			if (!sysScpi->eva.isEvaluate)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Off"));
			break;
		case KEY_FUNC_SYSTEM_SERVICE_CLEAR_LOGDATA:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_SERVICE_OPTION:
			p.drawText(QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(width - sep - pngSubSize, (height - 16) / 2, 16, 16);
			if (!key->isEnabled)
				p.drawPixmap(rc, img_ArrowHui);
			else
				p.drawPixmap(rc, img_Arrow);
			break;
		case KEY_FUNC_SYSTEM_SERVICE_TRACK_SELECT:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.options.isTrack36GHzOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("3.6G"));
			rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
			if (!sysData.options.isTrack36GHzOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("1.5G"));
			break;

		case KEY_FUNC_SYSTEM_SERVICE_OPTION_FFT:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(sep, rc.top() + rc.height(), (width - 2 * sep) / 2, rc.height() + enSep);
			if (sysData.options.isFft10HzOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("On"));
			rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
			if (!sysData.options.isFft10HzOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Off"));
			break;

		case KEY_FUNC_SYSTEM_SERVICE_OPTION_AMFM:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.options.isDemodOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("On"));
			rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
			if (!sysData.options.isDemodOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Off"));
			break;

		case KEY_FUNC_SYSTEM_SERVICE_OPTION_DIRECTCHANNEL:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.options.directChannelOptionOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("On"));
			rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
			if (!sysData.options.directChannelOptionOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Off"));
			break;

		case KEY_FUNC_SYSTEM_SERVICE_OPTION_RETURN:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_SERVICE_DEVICE:
			p.drawText(QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(width - sep - pngSubSize, (height - 16) / 2, 16, 16);
			if (!key->isEnabled)
				p.drawPixmap(rc, img_ArrowHui);
			else
				p.drawPixmap(rc, img_Arrow);
			break;

		case KEY_FUNC_SYSTEM_SERVICE_DEVICE_LOGDISPLAY:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());

			if (sysData.system.isLogOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("On"));
			rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
			if (!sysData.system.isLogOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Off"));
			break;

		case KEY_FUNC_SYSTEM_SERVICE_DEVICE_GPIB:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());

			if (sysData.system.isGpibOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("On"));
			rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
			if (!sysData.system.isGpibOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Off"));
			break;

		case KEY_FUNC_SYSTEM_SERVICE_DEVICE_RETURN:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_SERVICE_RETURN:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;

		case KEY_FUNC_SYSTEM_NEXTPAGE:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;

		case KEY_FUNC_SYSTEM_BACKLIGHT:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);
			rc = QRect(rc.left() + sep, rc.top() + 5, rc.width() - sep, rc.height() - 5 * 2);
			p.fillRect(rc, Qt::gray);
			rc = QRect(rc.left(), rc.top(), rc.width() * sysData.system.backLight / 100, rc.height() - 1);
			p.fillRect(rc, Qt::green);
			p.setPen(Qt::gray);
			p.drawRect(rc);
			break;
		case KEY_FUNC_SYSTEM_BACKLIGHT_ENTER:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SYSTEM_SCPI_SCREENREFRESH:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.system.isScpiScreenRefresh)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("On"));
			rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
			if (!sysData.system.isScpiScreenRefresh)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Off"));
			break;
		case KEY_FUNC_SYSTEM_NEXTPAGE_RETURN:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;

			//Display
		case KEY_FUNC_DISPLAY_FULLSCREEN:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_DISPLAY_ZOOM:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.display.isZoomOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("On"));
			rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
			if (!sysData.display.isZoomOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Off"));
			break;
		case KEY_FUNC_DISPLAY_DISPLAYLINE:
			rows = 3;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, QString(sysScpi->getAmptAndUnitString(sysData.display.dispLineValue, tempChar, 0)).trimmed());
			rc = QRect(sep, rc.top() + rc.height(), (width - 2 * sep) / 2, rc.height() + enSep);
			if (sysData.display.isDispLineOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("On"));
			rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
			if (!sysData.display.isDispLineOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Off"));
			break;
		case KEY_FUNC_DISPLAY_DISPLAYLINE_DBM:
			switch (sysData.ampt.refUnit)
			{
				case rudBm:
					p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, "dBm");
					break;
				case rudBmV:
					p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, "dBmV");
					break;
				case rudBuV:
					p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, "dBuV");
					break;
				case ruW:
					p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, "W");
					break;
				case ruV:
					p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, "V");
					break;
				case rudBpW:
					p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, "dBpW");
					break;
				case rudBuW:
					p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, "dBuW");
					break;
				case rudBuVm:
					p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, "dBuV/m");
					break;
				case ruuV:
					p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, "uV");
					break;
				default:
					p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, "unknown");
					break;
			}
			break;
		case KEY_FUNC_DISPLAY_REFSCALE:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.display.isAmptGraticuleOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("On"));
			rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
			if (!sysData.display.isAmptGraticuleOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Off"));
			break;
		case KEY_FUNC_DISPLAY_GRID:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.display.isGridOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("On"));
			rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
			if (!sysData.display.isGridOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Off"));
			break;
		case KEY_FUNC_DISPLAY_LABEL:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.display.isLabelOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("On"));
			rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
			if (!sysData.display.isLabelOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Off"));
			break;
		case KEY_FUNC_DISPLAY_STYLE:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width(), rc.height());
			p.setFont(fontData);

			switch (sysData.display.winStyle)
			{
				case dsDefault:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Default"));
					break;
				case dsWinXP:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("WinXP"));
					break;
			}
			rc = QRect(width - sep - pngSubSize, (height - 16) / 2, 16, 16);
			if (!key->isEnabled)
				p.drawPixmap(rc, img_ArrowHui);
			else
				p.drawPixmap(rc, img_Arrow);
			break;
		case KEY_FUNC_DISPLAY_STYLE_DEFAULT:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_DISPLAY_STYLE_STYLE1:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_DISPLAY_STYLE_RETURN:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;

			//print setup
		case KEY_FUNC_PRINTSETUP_PAPER:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width(), rc.height());
			p.setFont(fontData);
			switch (sysData.print.paper)
			{
				case 0:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, "A4");
					break;
				case 1:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, "A3");
					break;
				case 2:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, "B5");
					break;
				case 3:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, "C5");
					break;
				case 4:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, "Letter");
					break;
				default:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, "unKnown");
					break;
			}
			rc = QRect(width - sep - pngSubSize, (height - 16) / 2, 16, 16);
			if (!key->isEnabled)
				p.drawPixmap(rc, img_ArrowHui);
			else
				p.drawPixmap(rc, img_Arrow);
			break;
		case KEY_FUNC_PRINTSETUP_PAPER_A4:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_PRINTSETUP_PAPER_A3:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_PRINTSETUP_PAPER_B5:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_PRINTSETUP_PAPER_C5:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_PRINTSETUP_PAPER_LETTER:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_PRINTSETUP_PAPER_RETURN:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_PRINTSETUP_LANGUAGE:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.print.isPclLanguage)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("Pcl"));
			rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
			if (!sysData.print.isPclLanguage)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Esc"));
			break;
		case KEY_FUNC_PRINTSETUP_COLOR:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (!sysData.print.isColorPrinter)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("Blak"));
			rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
			if (sysData.print.isColorPrinter)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Colr"));
			break;
		case KEY_FUNC_PRINTSETUP_ORIENTATION:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.print.isOrientationHor)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("Hor"));
			rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
			if (!sysData.print.isOrientationHor)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Ver"));
			break;

		case KEY_FUNC_PRINTSETUP_NUMS:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, intToString(sysData.print.nums, tempChar));
			break;
		case KEY_FUNC_PRINTSETUP_NUMS_ENTER:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_PRINTSETUP_PRINTTRACE:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_PRINTSETUP_PRINTSCREEN:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;

			//SAVE
		case KEY_FUNC_SAVE_SCEEEN:
			p.drawText(QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(width - sep - pngSubSize, (height - 16) / 2, 16, 16);
			if (!key->isEnabled)
				p.drawPixmap(rc, img_ArrowHui);
			else
				p.drawPixmap(rc, img_Arrow);
			break;
		case KEY_FUNC_SAVE_SCEEEN_TOLOCAL:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SAVE_SCEEEN_TOUSB:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SAVE_SCEEEN_RETURN:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SAVE_TRACE:
			p.drawText(QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(width - sep - pngSubSize, (height - 16) / 2, 16, 16);
			if (!key->isEnabled)
				p.drawPixmap(rc, img_ArrowHui);
			else
				p.drawPixmap(rc, img_Arrow);
			break;
		case KEY_FUNC_SAVE_TRACE_TOLOCAL:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SAVE_TRACE_TOUSB:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SAVE_TRACE_RETURN:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SAVE_USER:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SAVE_LOAD:
			p.drawText(QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(width - sep - pngSubSize, (height - 16) / 2, 16, 16);
			if (!key->isEnabled)
				p.drawPixmap(rc, img_ArrowHui);
			else
				p.drawPixmap(rc, img_Arrow);
			break;
		case KEY_FUNC_SAVE_CLEAR_USER1:
		case KEY_FUNC_SAVE_SAVE_USER1:
		case KEY_FUNC_SAVE_LOAD_USER1:
			rows = 2;
			userIndex = 0;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);

			if (sysData.userSelfState[userIndex].saved)
			{
				p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, ((QString) getFreqAndUnitString(sysData.userSelfState[userIndex].centerFreq, 1, tempChar)).trimmed());
			} else
			{
				p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, "---");
			}
			break;
		case KEY_FUNC_SAVE_CLEAR_USER2:
		case KEY_FUNC_SAVE_SAVE_USER2:
		case KEY_FUNC_SAVE_LOAD_USER2:
			rows = 2;
			userIndex = 1;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);

			if (sysData.userSelfState[userIndex].saved)
			{
				p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, ((QString) getFreqAndUnitString(sysData.userSelfState[userIndex].centerFreq, 1, tempChar)).trimmed());
			} else
			{
				p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, "---");
			}
			break;
		case KEY_FUNC_SAVE_CLEAR_USER3:
		case KEY_FUNC_SAVE_SAVE_USER3:
		case KEY_FUNC_SAVE_LOAD_USER3:
			rows = 2;
			userIndex = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);

			if (sysData.userSelfState[userIndex].saved)
			{
				p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, ((QString) getFreqAndUnitString(sysData.userSelfState[userIndex].centerFreq, 1, tempChar)).trimmed());
			} else
			{
				p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, "---");
			}
			break;
		case KEY_FUNC_SAVE_CLEAR_USER4:
		case KEY_FUNC_SAVE_SAVE_USER4:
		case KEY_FUNC_SAVE_LOAD_USER4:
			rows = 2;
			userIndex = 3;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);

			if (sysData.userSelfState[userIndex].saved)
			{
				p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, ((QString) getFreqAndUnitString(sysData.userSelfState[userIndex].centerFreq, 1, tempChar)).trimmed());
			} else
			{
				p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, "---");
			}
			break;
		case KEY_FUNC_SAVE_CLEAR_USER5:
		case KEY_FUNC_SAVE_SAVE_USER5:
		case KEY_FUNC_SAVE_LOAD_USER5:
			rows = 2;
			userIndex = 4;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);

			if (sysData.userSelfState[userIndex].saved)
			{
				p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, ((QString) getFreqAndUnitString(sysData.userSelfState[userIndex].centerFreq, 1, tempChar)).trimmed());
			} else
			{
				p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, "---");
			}
			break;
		case KEY_FUNC_SAVE_CLEAR_USER6:
		case KEY_FUNC_SAVE_SAVE_USER6:
		case KEY_FUNC_SAVE_LOAD_USER6:
			rows = 2;
			userIndex = 5;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);

			if (sysData.userSelfState[userIndex].saved)
			{
				p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, ((QString) getFreqAndUnitString(sysData.userSelfState[userIndex].centerFreq, 1, tempChar)).trimmed());
			} else
			{
				p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, "---");
			}
			break;
		case KEY_FUNC_SAVE_LOAD_NEXTPAGE:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SAVE_CLEAR_USER7:
		case KEY_FUNC_SAVE_SAVE_USER7:
		case KEY_FUNC_SAVE_LOAD_USER7:
			rows = 2;
			userIndex = 6;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);

			if (sysData.userSelfState[userIndex].saved)
			{
				p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, ((QString) getFreqAndUnitString(sysData.userSelfState[userIndex].centerFreq, 1, tempChar)).trimmed());
			} else
			{
				p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, "---");
			}
			break;
		case KEY_FUNC_SAVE_CLEAR_USER8:
		case KEY_FUNC_SAVE_SAVE_USER8:
		case KEY_FUNC_SAVE_LOAD_USER8:
			rows = 2;
			userIndex = 7;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);

			if (sysData.userSelfState[userIndex].saved)
			{
				p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, ((QString) getFreqAndUnitString(sysData.userSelfState[userIndex].centerFreq, 1, tempChar)).trimmed());
			} else
			{
				p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, "---");
			}
			break;
		case KEY_FUNC_SAVE_CLEAR_USER9:
		case KEY_FUNC_SAVE_SAVE_USER9:
		case KEY_FUNC_SAVE_LOAD_USER9:
			rows = 2;
			userIndex = 8;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);

			if (sysData.userSelfState[userIndex].saved)
			{
				p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, ((QString) getFreqAndUnitString(sysData.userSelfState[userIndex].centerFreq, 1, tempChar)).trimmed());
			} else
			{
				p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, "---");
			}
			break;
		case KEY_FUNC_SAVE_CLEAR_USER10:
		case KEY_FUNC_SAVE_SAVE_USER10:
		case KEY_FUNC_SAVE_LOAD_USER10:
			rows = 2;
			userIndex = 9;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);

			if (sysData.userSelfState[userIndex].saved)
			{
				p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, ((QString) getFreqAndUnitString(sysData.userSelfState[userIndex].centerFreq, 1, tempChar)).trimmed());
			} else
			{
				p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, "---");
			}
			break;
		case KEY_FUNC_SAVE_LOAD_RETURN:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SAVE_CLEAR:
		case KEY_FUNC_SAVE_SAVE:
			p.drawText(QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(width - sep - pngSubSize, (height - 16) / 2, 16, 16);
			if (!key->isEnabled)
				p.drawPixmap(rc, img_ArrowHui);
			else
				p.drawPixmap(rc, img_Arrow);
			break;
		case KEY_FUNC_SAVE_CLEAR_NEXTPAGE:
		case KEY_FUNC_SAVE_SAVE_NEXTPAGE:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SAVE_CLEAR_RETURN:
		case KEY_FUNC_SAVE_SAVE_RETURN:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SAVE_CLEAR_ALL:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;

			//file
		case KEY_FUNC_FILE_REFRESH:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FILE_SHOW:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width(), rc.height());
			p.setFont(fontData);
			switch (sysData.file.show)
			{
				case stScreenPng:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Screens"));
					break;
				case stTraceData:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("TraceDatas"));
					break;
				case stAll:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("All"));
					break;
			}
			rc = QRect(width - sep - pngSubSize, (height - 16) / 2, 16, 16);
			if (!key->isEnabled)
				p.drawPixmap(rc, img_ArrowHui);
			else
				p.drawPixmap(rc, img_Arrow);
			break;
		case KEY_FUNC_FILE_SHOW_SCREENS:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FILE_SHOW_TRACEDATAS:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FILE_SHOW_ALL:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FILE_SHOW_RETURN:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FILE_CATALOG:
			p.drawText(QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(width - sep - pngSubSize, (height - 16) / 2, 16, 16);
			if (!key->isEnabled)
				p.drawPixmap(rc, img_ArrowHui);
			else
				p.drawPixmap(rc, img_Arrow);
			break;
		case KEY_FUNC_FILE_FIRSTPAGE:
		  if ((sysData.file.total > 1) && (sysData.file.nowPageIndex > 0))
		  {
		  	key->isEnabled = true;
		  } else
		  {
		  	key->isEnabled = false;
		  }
		  if(!key->isEnabled)
		  {
		  	p.setPen(BUTTONDISABLECOLOR);
		  }else
		  {
		  	p.setPen(BUTTONENABLECOLOR);
		  }
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FILE_PREVPAGE:
			if ((sysData.file.total > 1) && (sysData.file.nowPageIndex > 0))
			{
				key->isEnabled = true;
			} else
			{
				key->isEnabled = false;
			}
			if (!key->isEnabled)
			{
				p.setPen(BUTTONDISABLECOLOR);
			} else
			{
				p.setPen(BUTTONENABLECOLOR);
			}
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FILE_NEXTPAGE:
			if ((sysData.file.total > 0) && (sysData.file.nowPageIndex < sysData.file.pages - 1))
			{
				key->isEnabled = true;
			} else
			{
				key->isEnabled = false;
			}
			if (!key->isEnabled)
			{
				p.setPen(BUTTONDISABLECOLOR);
			} else
			{
				p.setPen(BUTTONENABLECOLOR);
			}
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FILE_LASTPAGE:
			if ((sysData.file.total > 0) && (sysData.file.nowPageIndex < sysData.file.pages - 1))
			{
				key->isEnabled = true;
			} else
			{
				key->isEnabled = false;
			}
			if(!key->isEnabled)
			{
				p.setPen(BUTTONDISABLECOLOR);
			} else
			{
				p.setPen(BUTTONENABLECOLOR);
			}
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FILE_OPERATIONS:
			p.drawText(QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(width - sep - pngSubSize, (height - 16) / 2, 16, 16);
			p.drawPixmap(rc, img_Arrow);
			break;
		case KEY_FUNC_FILE_OPERATIONS_SORT:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width(), rc.height());
			p.setFont(fontData);
			switch (sysData.file.sort)
			{
				case stName:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Name"));
					break;
				case stTime:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Time"));
					break;
				case stSize:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Size"));
					break;
			}
			rc = QRect(width - sep - pngSubSize, (height - 16) / 2, 16, 16);
			if (!key->isEnabled)
				p.drawPixmap(rc, img_ArrowHui);
			else
				p.drawPixmap(rc, img_Arrow);
			break;
		case KEY_FUNC_FILE_OPERATIONS_SORT_NAME:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FILE_OPERATIONS_SORT_DATETIME:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FILE_OPERATIONS_SORT_SIZE:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FILE_OPERATIONS_SORT_RETURN:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FILE_OPERATIONS_DELETE:
			p.drawText(QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(width - sep - pngSubSize, (height - 16) / 2, 16, 16);
			if (!key->isEnabled)
				p.drawPixmap(rc, img_ArrowHui);
			else
				p.drawPixmap(rc, img_Arrow);
			break;
		case KEY_FUNC_FILE_OPERATIONS_DELETE_SELECT:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FILE_OPERATIONS_DELETE_PAGE:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FILE_OPERATIONS_DELETE_ALL:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FILE_OPERATIONS_DELETE_RETURN:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FILE_OPERATIONS_COPY:
			p.drawText(QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(width - sep - pngSubSize, (height - 16) / 2, 16, 16);
			if (!key->isEnabled)
				p.drawPixmap(rc, img_ArrowHui);
			else
				p.drawPixmap(rc, img_Arrow);
			break;
		case KEY_FUNC_FILE_OPERATIONS_COPY_SELECT_TOUSB:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FILE_OPERATIONS_COPY_PAGE_TOUSB:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FILE_OPERATIONS_COPY_ALL_TOUSB:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FILE_OPERATIONS_COPY_RETURN:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;

		case KEY_FUNC_FILE_OPERATIONS_LOAD:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_FILE_OPERATIONS_RETURN:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;

			//source
		case KEY_FUNC_SOURCE_TRACKGEN:
			p.drawText(QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(width - sep - pngSubSize, (height - 16) / 2, 16, 16);
			if (!key->isEnabled)
				p.drawPixmap(rc, img_ArrowHui);
			else
				p.drawPixmap(rc, img_Arrow);
			break;
		case KEY_FUNC_SOURCE_TRACKGEN_TRACKGEN:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.source.isTrackGenOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("On"));
			rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
			if (!sysData.source.isTrackGenOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Off"));
			break;

		case KEY_FUNC_SOURCE_TRACKGEN_MEASTYPE:
			key->isEnabled = sysData.source.isTrackGenNetworkOn;
			if (!key->isEnabled)
				p.setPen(BUTTONDISABLECOLOR);
			else
				p.setPen(BUTTONENABLECOLOR);
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width(), rc.height());
			p.setFont(fontData);
			switch (sysData.source.measType)
			{
				case S21:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, "S21");
					break;
				case S11:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, "S11");
					break;
				default:
					p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, "unknown");
					break;
			}
			rc = QRect(width - sep - pngSubSize, (height - 16) / 2, 16, 16);
			if (!key->isEnabled)
				p.drawPixmap(rc, img_ArrowHui);
			else
				p.drawPixmap(rc, img_Arrow);
			break;
		case KEY_FUNC_SOURCE_TRACKGEN_MEASTYPE_S21:
		case KEY_FUNC_SOURCE_TRACKGEN_MEASTYPE_S11:
		case KEY_FUNC_SOURCE_TRACKGEN_MEASTYPE_RETURN:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;

		case KEY_FUNC_SOURCE_TRACKGEN_POWER:
			key->isEnabled = sysData.source.isTrackGenOn;
			if (!key->isEnabled)
			{
				p.setPen(BUTTONDISABLECOLOR);
			} else
			{
				p.setPen(BUTTONENABLECOLOR);
			}

			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, ((QString) sysScpi->getAmptAndUnitString(sysData.source.trackGenOutputPower, tempChar, 0)).trimmed());
			break;
		case KEY_FUNC_SOURCE_TRACKGEN_POWER_ENTER:
			switch (sysData.ampt.refUnit)
			{
				case rudBm:
					p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, "dBm");
					break;
				case rudBmV:
					p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, "dBmV");
					break;
				case rudBuV:
					p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, "dBuV");
					break;
				case ruW:
					p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, "mW");
					break;
				case ruV:
					p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, "mV");
					break;
				case rudBuW:
					p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, "dBuW");
					break;
				case rudBpW:
					p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, "dBpW");
					break;
				case rudBuVm:
					p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, "dBuV/m");
					break;
				case ruuV:
					p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, "uV");
					break;
				default:
					p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, "unknown");
					break;
			}
			break;
		case KEY_FUNC_SOURCE_TRACKGEN_NETWORK:
			key->isEnabled = sysData.source.isTrackGenOn;
			if (!key->isEnabled)
			{
				p.setPen(BUTTONDISABLECOLOR);
			} else
			{
				p.setPen(BUTTONENABLECOLOR);
			}
			p.drawText(QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(width - sep - pngSubSize, (height - 16) / 2, 16, 16);
			if (!key->isEnabled)
				p.drawPixmap(rc, img_ArrowHui);
			else
				p.drawPixmap(rc, img_Arrow);
			break;
		case KEY_FUNC_SOURCE_TRACKGEN_NETWORK_NETWORK:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.source.isTrackGenNetworkOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("On"));
			rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
			if (!sysData.source.isTrackGenNetworkOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Off"));
			break;
		case KEY_FUNC_SOURCE_TRACKGEN_NETWORK_NORMALIZE:
			key->isEnabled = sysData.source.isTrackGenNetworkOn;
			if (!key->isEnabled)
			{
				p.setPen(BUTTONDISABLECOLOR);
			} else
			{
				p.setPen(BUTTONENABLECOLOR);
			}

			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SOURCE_TRACKGEN_NETWORK_REFLEVEL:
			key->isEnabled = sysData.source.isTrackGenNetworkOn;
			if (!key->isEnabled)
			{
				p.setPen(BUTTONDISABLECOLOR);
			} else
			{
				p.setPen(BUTTONENABLECOLOR);
			}
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, ((QString) floatToStringDot2(sysData.source.trackGenNetworkRefLevel, tempChar)).trimmed() + " dB");
			break;
		case KEY_FUNC_SOURCE_TRACKGEN_NETWORK_REFLEVEL_ENTER:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SOURCE_TRACKGEN_NETWORK_POWER:
			key->isEnabled = sysData.source.isTrackGenNetworkOn;
			if (!key->isEnabled)
			{
				p.setPen(BUTTONDISABLECOLOR);
			} else
			{
				p.setPen(BUTTONENABLECOLOR);
			}

			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, ((QString) sysScpi->getAmptAndUnitString(sysData.source.trackGenNetworkOutputPower, tempChar, 0)).trimmed());
			break;
		case KEY_FUNC_SOURCE_TRACKGEN_NETWORK_POWER_ENTER:
			switch (sysData.ampt.refUnit)
			{
				case rudBm:
					p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, "dBm");
					break;
				case rudBmV:
					p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, "dBmV");
					break;
				case rudBuV:
					p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, "dBuV");
					break;
				case ruW:
					p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, "mW");
					break;
				case ruV:
					p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, "mV");
					break;
				case rudBuW:
					p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, "dBuW");
					break;
				case rudBpW:
					p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, "dBpW");
					break;
				case rudBuVm:
					p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, "dBuV/m");
					break;
				case ruuV:
					p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, "uV");
					break;
				default:
					p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, "unknown");
					break;
			}
			break;
		case KEY_FUNC_SOURCE_TRACKGEN_NETWORK_SWEEPTIME:
			key->isEnabled = sysData.source.isTrackGenNetworkOn;
			if (!key->isEnabled)
			{
				p.setPen(BUTTONDISABLECOLOR);
			} else
			{
				p.setPen(BUTTONENABLECOLOR);
			}

			rows = 3;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, ((QString) getMsTimeAndUnitString(sysData.sweep.sweepTime, tempChar)).trimmed());
			rc = QRect(sep, rc.top() + rc.height(), (width - 2 * sep) / 2, rc.height() + enSep);
			if (sysData.sweep.sweepTimeAuto)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("Auto"));
			rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
			if (!sysData.sweep.sweepTimeAuto)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Man"));
			break;
		case KEY_FUNC_SOURCE_TRACKGEN_NETWORK_SWEEPTIME_S:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SOURCE_TRACKGEN_NETWORK_SWEEPTIME_MS:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SOURCE_TRACKGEN_NETWORK_SWEEPTIME_US:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SOURCE_TRACKGEN_NETWORK_SWEEPTIME_NS:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SOURCE_TRACKGEN_NETWORK_SWEEPPOINT:
			key->isEnabled = sysData.source.isTrackGenNetworkOn;
			if (!key->isEnabled)
			{
				p.setPen(BUTTONDISABLECOLOR);
			} else
			{
				p.setPen(BUTTONENABLECOLOR);
			}

			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, intToString(sysData.sweep.sweepPoints, tempChar));
			break;
		case KEY_FUNC_SOURCE_TRACKGEN_NETWORK_SWEEPPOINT_ENTER:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;

		case KEY_FUNC_SOURCE_TRACKGEN_NETWORK_RETURN:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SOURCE_TRACKGEN_VSWR:
			p.drawText(QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(width - sep - pngSubSize, (height - 16) / 2, 16, 16);
			if (!key->isEnabled)
				p.drawPixmap(rc, img_ArrowHui);
			else
				p.drawPixmap(rc, img_Arrow);
			break;
		case KEY_FUNC_SOURCE_TRACKGEN_VSWR_VSWR:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.source.isVswrOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("On"));
			rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
			if (!sysData.source.isVswrOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Off"));
			break;
		case KEY_FUNC_SOURCE_TRACKGEN_VSWR_NORMALIZE:
			key->isEnabled = sysData.source.isVswrOn;
			if (!key->isEnabled)
			{
				p.setPen(BUTTONDISABLECOLOR);
			} else
			{
				p.setPen(BUTTONENABLECOLOR);
			}
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SOURCE_TRACKGEN_VSWR_REFERENCE:
			key->isEnabled = sysData.source.isVswrOn;
			if (!key->isEnabled)
			{
				p.setPen(BUTTONDISABLECOLOR);
			} else
			{
				p.setPen(BUTTONENABLECOLOR);
			}
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, ((QString) floatToStringDot2(sysData.source.vswrRef, tempChar)).trimmed() + " dB");
			break;
		case KEY_FUNC_SOURCE_TRACKGEN_VSWR_REFERENCE_ENTER:
		case KEY_FUNC_SOURCE_TRACKGEN_VSWR_RETURN:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;

		case KEY_FUNC_SOURCE_TRACKGEN_RETURN:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SOURCE_SIGNALGEN:
			key->isEnabled = sysData.options.isTrack36GHzOn;
			if (!key->isEnabled)
				p.setPen(BUTTONDISABLECOLOR);
			else
				p.setPen(BUTTONENABLECOLOR);
			p.drawText(QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(width - sep - pngSubSize, (height - 16) / 2, 16, 16);
			if (!key->isEnabled)
				p.drawPixmap(rc, img_ArrowHui);
			else
				p.drawPixmap(rc, img_Arrow);
			break;
		case KEY_FUNC_SOURCE_SIGNALGEN_SIGNALGEN:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.source.isSignalGenOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("On"));
			rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
			if (!sysData.source.isSignalGenOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Off"));
			break;
		case KEY_FUNC_SOURCE_SIGNALGEN_FREQ:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, ((QString) getFreqAndUnitString(sysData.source.signalGenOutputFreq, 1, tempChar)).trimmed());
			break;
		case KEY_FUNC_SOURCE_SIGNALGEN_FREQ_GHZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SOURCE_SIGNALGEN_FREQ_MHZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SOURCE_SIGNALGEN_FREQ_KHZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SOURCE_SIGNALGEN_FREQ_HZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_SOURCE_SIGNALGEN_POWER:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, ((QString) sysScpi->getAmptAndUnitString(sysData.source.signalGenOutputPower, tempChar, 0)).trimmed());
			break;
		case KEY_FUNC_SOURCE_SIGNALGEN_POWER_ENTER:
			switch (sysData.ampt.refUnit)
			{
				case rudBm:
					p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, "dBm");
					break;
				case rudBmV:
					p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, "dBmV");
					break;
				case rudBuV:
					p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, "dBuV");
					break;
				case ruW:
					p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, "mW");
					break;
				case ruV:
					p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, "mV");
					break;
				case rudBuW:
					p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, "dBuW");
					break;
				case rudBpW:
					p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, "dBpW");
					break;
				case rudBuVm:
					p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, "dBuV/m");
					break;
				case ruuV:
					p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, "uV");
					break;
				default:
					p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, "unknown");
					break;
			}
			break;
		case KEY_FUNC_SOURCE_SIGNALGEN_RETURN:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;

		case KEY_FUNC_DEMOD_DEMOD:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(rc.left(), rc.top() + rc.height(), rc.width() / 2, rc.height());
			if (sysData.demod.demodOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("On"));
			rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
			if (!sysData.demod.demodOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Off"));
			break;
		case KEY_FUNC_DEMOD_SOUND:
			key->isEnabled = sysData.demod.demodOn;
			if (!key->isEnabled)
				p.setPen(BUTTONDISABLECOLOR);
			else
				p.setPen(BUTTONENABLECOLOR);

			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);
			rc = QRect(rc.left() + sep, rc.top() + 5, rc.width() - sep, rc.height() - 5 * 2);
			p.fillRect(rc, Qt::gray);
			rc = QRect(rc.left(), rc.top(), rc.width() * sysData.demod.sound / 100, rc.height() - 1);
			p.fillRect(rc, Qt::green);
			p.setPen(Qt::gray);
			p.drawRect(rc);
			break;
		case KEY_FUNC_DEMOD_SOUND_ENTER:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_DEMOD_RADIOSET:
			key->isEnabled = sysData.demod.demodOn;
			if (!key->isEnabled)
				p.setPen(BUTTONDISABLECOLOR);
			else
				p.setPen(BUTTONENABLECOLOR);

			p.drawText(QRect(0, sep, width - 1 * sep - pngSubSize, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(width - sep - pngSubSize, (height - 16) / 2, 16, 16);
			if (!key->isEnabled)
				p.drawPixmap(rc, img_ArrowHui);
			else
				p.drawPixmap(rc, img_Arrow);
			break;
		case KEY_FUNC_DEMOD_RADIOSET_1:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, ((QString) getFreqAndUnitString(sysData.demod.radioFreqSet[0], 0, tempChar)).trimmed());
			break;
		case KEY_FUNC_DEMOD_RADIOSET_MUSIC:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, ((QString) getFreqAndUnitString(sysData.demod.radioFreqSet[1], 0, tempChar)).trimmed());
			break;
		case KEY_FUNC_DEMOD_RADIOSET_TRAFFIC:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, ((QString) getFreqAndUnitString(sysData.demod.radioFreqSet[2], 0, tempChar)).trimmed());
			break;
		case KEY_FUNC_DEMOD_RADIOSET_2:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, ((QString) getFreqAndUnitString(sysData.demod.radioFreqSet[3], 0, tempChar)).trimmed());
			break;
		case KEY_FUNC_DEMOD_RADIOSET_3:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, ((QString) getFreqAndUnitString(sysData.demod.radioFreqSet[4], 0, tempChar)).trimmed());
			break;
		case KEY_FUNC_DEMOD_RADIOSET_4:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, ((QString) getFreqAndUnitString(sysData.demod.radioFreqSet[5], 0, tempChar)).trimmed());
			break;
		case KEY_FUNC_DEMOD_RADIOSET_RETURN:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;

		case KEY_FUNC_DEMOD_RETURN:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;

		case KEY_FUNC_AUX_IMPEDANCE_CONVERSION:
			rows = 3;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			keyName = floatToStringDot2(sysData.ampt.LMPValue, tempChar);
			keyName += " dB";
			p.setFont(fontData);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(sep, rc.top() + rc.height(), (width - 2 * sep) / 2, rc.height() + enSep);
			if (sysData.isImpedanceTransformation)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("On"));
			rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
			if (!sysData.isImpedanceTransformation)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Off"));
			break;
		case KEY_FUNC_AUX_IMPEDANCE_CONVERSION_ENTER:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;

			//AM、FM
		case KEY_FUNC_MODE_OPTIONS_DEMOD:
			p.drawText(QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(width - sep - pngSubSize, (height - 16) / 2, 16, 16);
			if (!key->isEnabled)
				p.drawPixmap(rc, img_ArrowHui);
			else
				p.drawPixmap(rc, img_Arrow);
			break;
		case KEY_FUNC_MODE_OPTIONS_DEMOD_AUDIO:
			key->isEnabled = sysData.options.isDemodOn;
			if (!key->isEnabled)
				p.setPen(BUTTONDISABLECOLOR);
			else
				p.setPen(BUTTONENABLECOLOR);

			p.drawText(QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(width - sep - pngSubSize, (height - 16) / 2, 16, 16);

			if (!key->isEnabled)
				p.drawPixmap(rc, img_ArrowHui);
			else
				p.drawPixmap(rc, img_Arrow);
			break;
		case KEY_FUNC_MODE_OPTIONS_DEMOD_AM_IFBW:
			key->isEnabled = sysData.options.amOn;
			if (!key->isEnabled)
				p.setPen(BUTTONDISABLECOLOR);
			else
				p.setPen(BUTTONENABLECOLOR);
			rows = 3;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, ((QString) getFreqAndUnitString(sysData.options.am_ifbw, 1, tempChar)).trimmed());
			rc = QRect(sep, rc.top() + rc.height(), (width - 2 * sep) / 2, rc.height() + enSep);
			if (sysData.options.am_ifbw_auto)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("Auto"));
			rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
			if (!sysData.options.am_ifbw_auto)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Man"));
			break;
		case KEY_FUNC_MODE_OPTIONS_DEMOD_FM_IFBW:
			key->isEnabled = sysData.options.fmOn;
			if (!key->isEnabled)
				p.setPen(BUTTONDISABLECOLOR);
			else
				p.setPen(BUTTONENABLECOLOR);

			rows = 3;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, ((QString) getFreqAndUnitString(sysData.options.fm_ifbw, 1, tempChar)).trimmed());
			rc = QRect(sep, rc.top() + rc.height(), (width - 2 * sep) / 2, rc.height() + enSep);
			if (sysData.options.fm_ifbw_auto)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("Auto"));
			rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
			if (!sysData.options.fm_ifbw_auto)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Man"));
			break;
		case KEY_FUNC_MODE_OPTIONS_DEMOD_AM_CARRIERFREQ:
			key->isEnabled = sysData.options.amOn;
			if (!key->isEnabled)
				p.setPen(BUTTONDISABLECOLOR);
			else
				p.setPen(BUTTONENABLECOLOR);

			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, ((QString) getFreqAndUnitString(sysData.options.am_carrierfreq, 1, tempChar)).trimmed());
			break;
		case KEY_FUNC_MODE_OPTIONS_DEMOD_FM_CARRIERFREQ:
			key->isEnabled = sysData.options.fmOn;
			if (!key->isEnabled)
				p.setPen(BUTTONDISABLECOLOR);
			else
				p.setPen(BUTTONENABLECOLOR);

			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(0, rc.top() + rc.height(), width - sep, rc.height());
			p.setFont(fontData);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, ((QString) getFreqAndUnitString(sysData.options.fm_carrierfreq, 1, tempChar)).trimmed());
			break;
		case KEY_FUNC_MODE_OPTIONS_DEMOD_AM:
		case KEY_FUNC_MODE_OPTIONS_DEMOD_FM:
			p.drawText(QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(width - sep - pngSubSize, (height - 16) / 2, 16, 16);
			p.drawPixmap(rc, img_Arrow);
			break;
		case KEY_FUNC_MODE_OPTIONS_DEMOD_AM_AM:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(sep - 2, rc.top() + rc.height(), (width - 2 * sep) / 2, rc.height() + enSep);
			if (sysData.options.amOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("On"));
			rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
			if (!sysData.options.amOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Off"));
			break;

		case KEY_FUNC_MODE_OPTIONS_DEMOD_FM_FM:
			rows = 2;
			rc = QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(sep - 2, rc.top() + rc.height(), (width - 2 * sep) / 2, rc.height() + enSep);
			if (sysData.options.fmOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignLeft | Qt::AlignVCenter, getTheTranslatorString("On"));
			rc = QRect(rc.left() + rc.width(), rc.top(), rc.width(), rc.height());
			if (!sysData.options.fmOn)
				p.setFont(fontUnderline);
			else
				p.setFont(fontLight);
			p.drawText(rc, Qt::AlignRight | Qt::AlignVCenter, getTheTranslatorString("Off"));
			break;
		case KEY_FUNC_MODE_OPTIONS_DEMOD_AM_CARRIERFREQ_GHZ:
		case KEY_FUNC_MODE_OPTIONS_DEMOD_AM_CARRIERFREQ_MHZ:
		case KEY_FUNC_MODE_OPTIONS_DEMOD_AM_CARRIERFREQ_KHZ:
		case KEY_FUNC_MODE_OPTIONS_DEMOD_AM_CARRIERFREQ_HZ:
		case KEY_FUNC_MODE_OPTIONS_DEMOD_FM_CARRIERFREQ_GHZ:
		case KEY_FUNC_MODE_OPTIONS_DEMOD_FM_CARRIERFREQ_MHZ:
		case KEY_FUNC_MODE_OPTIONS_DEMOD_FM_CARRIERFREQ_KHZ:
		case KEY_FUNC_MODE_OPTIONS_DEMOD_FM_CARRIERFREQ_HZ:
		case KEY_FUNC_MODE_OPTIONS_DEMOD_AM_IFBW_GHZ:
		case KEY_FUNC_MODE_OPTIONS_DEMOD_AM_IFBW_MHZ:
		case KEY_FUNC_MODE_OPTIONS_DEMOD_AM_IFBW_KHZ:
		case KEY_FUNC_MODE_OPTIONS_DEMOD_AM_IFBW_HZ:
		case KEY_FUNC_MODE_OPTIONS_DEMOD_FM_IFBW_GHZ:
		case KEY_FUNC_MODE_OPTIONS_DEMOD_FM_IFBW_MHZ:
		case KEY_FUNC_MODE_OPTIONS_DEMOD_FM_IFBW_KHZ:
		case KEY_FUNC_MODE_OPTIONS_DEMOD_FM_IFBW_HZ:
		case KEY_FUNC_MODE_OPTIONS_DEMOD_AM_RETURN:
		case KEY_FUNC_MODE_OPTIONS_DEMOD_FM_RETURN:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
		case KEY_FUNC_MODE_OPTIONS_DEMOD_RETRURN:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;

		case KEY_FUNC_MODE_OPTIONS_DEMOD_DEMODMODE:
			key->isEnabled = sysData.demod.demodOn;
			if (!key->isEnabled)
				p.setPen(BUTTONDISABLECOLOR);
			else
				p.setPen(BUTTONENABLECOLOR);

			p.drawText(QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(width - sep - pngSubSize, (height - 16) / 2, 16, 16);

			if (!key->isEnabled)
				p.drawPixmap(rc, img_ArrowHui);
			else
				p.drawPixmap(rc, img_Arrow);
			break;

		case KEY_FUNC_MODE_OPTIONS_DIGITAL_DEMOD:
			key->isEnabled = sysData.options.isDemodOn;
			if (!key->isEnabled)
				p.setPen(BUTTONDISABLECOLOR);
			else
				p.setPen(BUTTONENABLECOLOR);

			p.drawText(QRect(sep, sep, width - 2 * sep - pngSubSize, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			rc = QRect(width - sep - pngSubSize, (height - 16) / 2, 16, 16);

			if (!key->isEnabled)
				p.drawPixmap(rc, img_ArrowHui);
			else
				p.drawPixmap(rc, img_Arrow);
			break;

		case KEY_FUNC_MODE_OPTIONS_DEMOD_DEMODMODE_AM:
		case KEY_FUNC_MODE_OPTIONS_DEMOD_DEMODMODE_FM:
		case KEY_FUNC_MODE_OPTIONS_DEMOD_DEMODMODE_FMW:
		case KEY_FUNC_MODE_OPTIONS_DEMOD_DEMODMODE_LSB:
		case KEY_FUNC_MODE_OPTIONS_DEMOD_DEMODMODE_USB:
		case KEY_FUNC_MODE_OPTIONS_DEMOD_DEMODMODE_RETURN:
		case KEY_FUNC_MODE_OPTIONS_DIGITAL_DEMOD_RETURN:
		case KEY_FUNC_DEMOD_RADIOSET_1_GHZ:
		case KEY_FUNC_DEMOD_RADIOSET_1_MHZ:
		case KEY_FUNC_DEMOD_RADIOSET_1_KHZ:
		case KEY_FUNC_DEMOD_RADIOSET_1_HZ:
		case KEY_FUNC_DEMOD_RADIOSET_MUSIC_GHZ:
		case KEY_FUNC_DEMOD_RADIOSET_MUSIC_MHZ:
		case KEY_FUNC_DEMOD_RADIOSET_MUSIC_KHZ:
		case KEY_FUNC_DEMOD_RADIOSET_MUSIC_HZ:
		case KEY_FUNC_DEMOD_RADIOSET_TRAFFIC_GHZ:
		case KEY_FUNC_DEMOD_RADIOSET_TRAFFIC_MHZ:
		case KEY_FUNC_DEMOD_RADIOSET_TRAFFIC_KHZ:
		case KEY_FUNC_DEMOD_RADIOSET_TRAFFIC_HZ:
		case KEY_FUNC_DEMOD_RADIOSET_2_GHZ:
		case KEY_FUNC_DEMOD_RADIOSET_2_MHZ:
		case KEY_FUNC_DEMOD_RADIOSET_2_KHZ:
		case KEY_FUNC_DEMOD_RADIOSET_2_HZ:
		case KEY_FUNC_DEMOD_RADIOSET_3_GHZ:
		case KEY_FUNC_DEMOD_RADIOSET_3_MHZ:
		case KEY_FUNC_DEMOD_RADIOSET_3_KHZ:
		case KEY_FUNC_DEMOD_RADIOSET_3_HZ:
		case KEY_FUNC_DEMOD_RADIOSET_4_GHZ:
		case KEY_FUNC_DEMOD_RADIOSET_4_MHZ:
		case KEY_FUNC_DEMOD_RADIOSET_4_KHZ:
		case KEY_FUNC_DEMOD_RADIOSET_4_HZ:
			p.drawText(QRect(sep, sep, width - 2 * sep, (height - 2 * sep) / rows), Qt::AlignRight | Qt::AlignVCenter, keyName);
			break;
	}

	p.end();
}
