#include "sysscpi.h"

//获取中频数据
void tSysScpi::getDataFromIF(void)
{
	if (sysData.freq.lineZero)
	{
		return;
	}

	double tempValue = 0;
	double bcValue = 0;
	bool isTrue = false;
	double tempavgValue = 0;
	double passFailFreq = 0;
	double passFailStartFreq = 0;
	double passFailStopFreq = 0;

	if (sysData.measure.passFail.winSweepOn)
	{
		passFailStartFreq = sysData.measure.passFail.winFreqStart;
		passFailStopFreq = sysData.measure.passFail.winFreqStop;
	}

	if (ramHandle > 0)
	{
		if (sysData.markerFctn.isFreqCountOn)
		{
			unsigned long buf[1];
			double freqValue = 0;

			read(ifHandle, buf, sizeof buf);
			if (rfData.Start_Band >= 0 && rfData.Start_Band <= 2)
			{
				freqValue = sysData.freq.center + 21.4e6 - ((buf[0] & 0x3ffffff) * sysData.markerFctn.freqCountBw);
			} else
			{
				freqValue = sysData.freq.center - 21.4e6 + ((buf[0] & 0x3ffffff) * sysData.markerFctn.freqCountBw);
			}

			sysData.markerFctn.freqCountValue = freqValue;
		}

		if (sysData.markerFctn.isFreqCountOn)
		{
			for (int i = 0; i < TRACECOUNT; i++)
			{
				memcpy(sysData.trace[i].value, tempTraceData[i], sizeof(double) * sysData.sweep.sweepPoints);
			}

			return;
		}

		unsigned long dataBufSize = sysData.sweep.sweepPoints;
		if (dataBufSize < MINSWEEPPOINTS)
		{
			dataBufSize = MINSWEEPPOINTS;
		} else if (dataBufSize > MAXSWEEPPOINTS)
		{
			dataBufSize = MAXSWEEPPOINTS;
		}

		unsigned long dataBuf[dataBufSize];

		if (sysData.fft.fftOn)
		{
			if (!isFFtnd)
			{
				return;
			}
			memcpy(dataBuf, sysData.fft.fDataBuf, sizeof dataBuf);
		} else if (sysData.options.amOn || sysData.options.fmOn)
		{
			//memcpy(dataBuf, &sysData.options.fDataBuf[1], sizeof(unsigned long) * dataBufSize);
			int midPos = dataBufSize / 2;
			int step = DEMODFFTSIZE / 2 / midPos;
			int valuePos = 0;
			double maxValue = 0;

			dataBuf[midPos] = sysData.options.fftDataLine[DEMODFFTSIZE / 2];
			valuePos = DEMODFFTSIZE / 2 - 1;

			for (int i = midPos - 1; i >= 0; i--)
			{
				valuePos -= step;
				maxValue = sysData.options.fftDataLine[valuePos];

				for (int j = 1; j < step; j++)
				{
					if (sysData.options.fftDataLine[j + valuePos] > maxValue)
					{
						maxValue = sysData.options.fftDataLine[j + valuePos];
					}
				}

				dataBuf[i] = maxValue;
			}

			valuePos = DEMODFFTSIZE / 2 + 1;

			for (int i = midPos + 1; i < dataBufSize; i++)
			{
				valuePos += step;
				maxValue = sysData.options.fftDataLine[valuePos];

				for (int j = 1; j < step; j++)
				{
					if (sysData.options.fftDataLine[valuePos - j] > maxValue)
					{
						maxValue = sysData.options.fftDataLine[valuePos - j];
					}
				}

				dataBuf[i] = maxValue;
			}
		} else
		{
			if (isDirectGatherMode())
			{
				//if (!devControling && !sysData.zc.reseted)
				if (!devControling)
				{
					memcpy(dataBuf, &sysData.zc.dataBuf[0], sizeof dataBuf);
				}
			} else
			{
				if (!devControling)
				{
				//读取数据
					read(ramHandle, dataBuf, sizeof dataBuf);					
				}
			}
		}

		//当扫描点数增大时，将原最后一个扫描点的数据赋值给新增加的点
		if (isFillData)
		{
			for (int i = sweepPrevPoints; i < dataBufSize; i++)
			{
				dataBuf[i] = dataBuf[sweepPrevPoints - 1];
			}

			//memset(&dataBuf[sysData.sweep.sweepPrevPoints], dataBuf[sysData.sweep.sweepPrevPoints - 1], sizeof dataBuf[0] * (dataBufSize - sysData.sweep.sweepPrevPoints));
			isFillData = false;
		}

		if (sysData.freq.isLowChannel)
		{
			if (!sysData.zcPreamplifierCalData.isCaled && sysData.ampt.isPreamptOn)
			{
				sysData.zcPreamplifierCalData.absoluteAmptValue = 150;
				bcValue = 0;
			} else if (!sysData.zcCalData.isCaled)
			{
				sysData.zcCalData.absoluteAmptValue = 150;
				bcValue = 0;
			} else
			{
				if (sysData.ampt.isPreamptOn)
				{
					//bc att
					double attbcValue = getErrorOfAttenuationofLowfreqofPre();

					//bc if
					double ifbcValue = getErrorOfIFofLowfreqofPre();

					//bc rbw
					double rbwbcValue = getErrorOfRbwofLowfreqofPre();

					//bc temperature
					double tempbcValue = getErrorOfTemperature();

					bcValue += attbcValue + ifbcValue + rbwbcValue + tempbcValue;
					//LMP_C
					if (sysData.isImpedanceTransformation)
					{
						bcValue += sysData.ampt.LMPValue;
					}

				} else
				{
					//bc att
					//double attbcValue = getErrorOfAttenuationofLowfreq();

					//bc if
					//double ifbcValue = getErrorOfIFofLowfreq();

					//bc rbw
					double rbwbcValue = getErrorOfRbwofLowfreq();

					//bc temperature
					double tempbcValue = getErrorOfTemperature();

					//bcValue += attbcValue + rbwbcValue + tempbcValue;
					bcValue += rbwbcValue + tempbcValue;

					//LMP_C
					if (sysData.isImpedanceTransformation)
					{
						bcValue += sysData.ampt.LMPValue;
					}

				}
			}
		} else
		{
			if (!sysData.preamplifierCalData.isCaled && sysData.ampt.isPreamptOn)
			{
				sysData.preamplifierCalData.absoluteAmptValue = 150;
				bcValue = 0;
			} else if (!sysData.factoryCalData.isCaled)
			{
				sysData.factoryCalData.absoluteAmptValue = 150;
				bcValue = 0;
			} else
			{
				if (sysData.ampt.isPreamptOn)
				{
					//bc att
					double attbcValue = getErrorOfAttenuationofPre();

					//bc if
					double ifbcValue = getErrorOfIFofPre();

					//bc rbw
					double rbwbcValue = getErrorOfRbwofPre();

					//bc emc rbw
					if (sysData.emc.emcOn)
					{
						bcValue -= getErrorOfEmcRbw();
					}

					//bc temperature
					double tempbcValue = getErrorOfTemperature();

					bcValue += attbcValue + ifbcValue + rbwbcValue + tempbcValue;

					//LMP_C
					if (sysData.isImpedanceTransformation)
					{
						bcValue += sysData.ampt.LMPValue;
					}
					
				} else
				{
					//bc att
			//		double attbcValue = getErrorOfAttenuation();

					//bc if
					double ifbcValue = getErrorOfIF();

					//bc rbw
					double rbwbcValue = getErrorOfRbw();

//					//bc emc rbw
//					if (sysData.emc.emcOn)
//					{
//						bcValue -= getErrorOfEmcRbw();
//					}

					//bc temperature
					double tempbcValue = getErrorOfTemperature();

					bcValue += ifbcValue + rbwbcValue + tempbcValue;

				}
			}
		}

		//迹线平均缓冲区计数
		if (sysData.bw.bwAverageOn)
		{
			sysData.bw.bwAverageFlag++;

			//计数器满
			if (!sysData.bw.bwAverageFulled && sysData.bw.bwAverageFlag > sysData.bw.bwAverage)
			{
				sysData.bw.bwAverageFulled = true;
			}

			//计数器限制
			if (sysData.bw.bwAverageFlag > sysData.bw.bwAverage)
			{
				sysData.bw.bwAverageFlag = sysData.bw.bwAverage;
			}
		}

		isTrue = !sysData.isCalibrating &&
						 !sysData.isPreamplifierCalibrating &&
						 !sysData.isZcCalibrating &&
						 !sysData.isZcPreamplifierCalibrating &&
						 sysData.source.isTrackGenNetworkOn &&
						 sysData.source.isTrackGenNetworkOn &&
						 sysData.source.trackNetworkStandard.sweepPoints == dataBufSize &&
						 (unsigned long long) sysData.freq.start == (unsigned long long) sysData.source.trackNetworkStandard.startFreq &&
						 (unsigned long long) sysData.freq.stop	== (unsigned long long) sysData.source.trackNetworkStandard.stopFreq;

		double maxvalue,ifvalue = 0, rfvalue = 0;
		maxvalue = sysData.initValue[0];
		for (int i = 0; i < dataBufSize; i++)
		{
			//Pass-Fail data
			if (sysData.measure.passFail.winSweepOn)
			{
				passFailFreq = sysData.freq.start + i * sysData.span.span / dataBufSize;

				if (passFailFreq >= passFailStartFreq && passFailFreq <= passFailStopFreq)
				{
					if (dataBuf[i] > 0)
					{
						sysData.initValue[i] = 20 * log10(dataBuf[i]);//20 => 10
					} else
					{
						sysData.initValue[i] = 1e-3;
					}
				}
			} else //normal data
			if (dataBuf[i] > 0)
			{
				sysData.initValue[i] = 20 * log10(dataBuf[i]); //20 => 10
			} else
			{
				//continue;
				sysData.initValue[i] = 1e-3;
			}

			if (sysData.options.amOn || sysData.options.fmOn)
			{
				sysData.initValue[i] -= 70;
			}

			if (sysData.freq.isLowChannel)
			{
				if (sysData.ampt.isPreamptOn)
				{
					//tempValue = -40.0 + sysData.initValue[i];
					if(valuechanged)
					{
						printf("sysData.zcPreamplifierCalData.absoluteAmptValue!\n");
						__var(sysData.ampt.dsattRf);
						__var(sysData.ampt.dsattIf);
					}
					valuechanged = 0;

					tempValue = -40.0 + sysData.initValue[i] - sysData.zcPreamplifierCalData.absoluteAmptValue + sysData.userCalData.absError + sysData.ampt.dsattRf + sysData.ampt.dsattIf + sysData.ampt.refOffset + bcValue;
				} else
				{
					//tempValue = -20.0 + sysData.initValue[i];
					tempValue = -20.0 + sysData.initValue[i] - sysData.zcCalData.absoluteAmptValue + sysData.userCalData.absError + sysData.ampt.dsattRf + sysData.ampt.dsattIf + sysData.ampt.refOffset + bcValue;
					if(valuechanged){
						printf("sysData.zcCalData.absoluteAmptValue!\n");
						__var(sysData.ampt.dsattRf);
						__var(sysData.ampt.dsattIf);						
					}
					valuechanged = 0;
				}
			} else
			{
				if (sysData.ampt.isPreamptOn)
				{
					if(valuechanged)
						printf("sysData.preamplifierCalData.absoluteAmptValue!\n");
					valuechanged = 0;
					//tempValue = -24.0 + sysData.initValue[i]; 
					tempValue = -24.0 + sysData.initValue[i] - sysData.preamplifierCalData.absoluteAmptValue + sysData.userCalData.absError + sysData.ampt.attRf + sysData.ampt.attIf + sysData.ampt.refOffset + bcValue;
				} else
				{
					if(valuechanged)
						printf("sysData.factoryCalData.absoluteAmptValue!\n");
					valuechanged = 0;
					//tempValue = -4.0 + sysData.initValue[i];
					tempValue = -4.0 + sysData.initValue[i] - sysData.factoryCalData.absoluteAmptValue + sysData.userCalData.absError + sysData.ampt.attRf + sysData.ampt.attIf + sysData.ampt.refOffset + bcValue;
				}
			}
			if (sysData.initValue[i] > maxvalue)
			{
				maxvalue = sysData.initValue[i];
				ifvalue = bcValue;
				rfvalue = getErrorOfFreqResp(i);
			}
			if (sysData.freq.isLowChannel)
			{
				if (sysData.ampt.isPreamptOn)
				{
					tempValue += getErrorOfLowFreqRespofPre(i);
				} else
				{
					tempValue += getErrorOfLowFreqResp(i);
				}
//				if(i == 300)
			} else
			{
				if (sysData.ampt.isPreamptOn)
				{
					tempValue += getErrorOfFreqRespofPre(i);
				} else
				{
					tempValue += getErrorOfFreqResp(i);
				}
			}

			if (isTrue)
			{
				tempValue += getErrorOfNetworkMeasure(i);
			}

			if (sysData.ampt.scaleLine)
			{
				tempValue = getRefLevelValue(tempValue);
			}
//			if(i == 399 || i == 400 || i == 401)
			//迹线平均数据计算
			if (sysData.bw.bwAverageOn && sysData.bw.bwAverageFlag > 0 && sysData.bw.bwAverageFlag <= sysData.bw.bwAverage)
			{
				//store
				if (sysData.bw.bwAverageFlag <= sysData.bw.bwAverage && !sysData.bw.bwAverageFulled)
				{
					sysData.avgValue[sysData.bw.bwAverageFlag - 1][i] = tempValue;
				} else
				{
					for (int k = 0; k < sysData.bw.bwAverage - 1; k++)
					{
						sysData.avgValue[k][i] = sysData.avgValue[k + 1][i];
					}

					sysData.avgValue[sysData.bw.bwAverage - 1][i] = tempValue;
				}

				//calculate
				if (sysData.bw.bwAverageFlag > 0)
				{
					tempavgValue = 0;

					for (int j = 0; j < sysData.bw.bwAverageFlag; j++)
					{
						tempavgValue += sysData.avgValue[j][i] / sysData.bw.bwAverageFlag;
					}
				}
				sysData.prjValue[i] = tempavgValue;
			} else
			{
				sysData.prjValue[i] = tempValue;
			}
		}
		if(valuechanged)
		{
			valuechanged = 0;
			//__var(ifvalue);
			//__var(rfvalue);
		}
		for (int i = 0; i < TRACECOUNT; i++)
		{
			switch (sysData.trace[i].state)
			{
				case tsClearWrite:
					memcpy(sysData.trace[i].value, sysData.prjValue, sizeof(double) * dataBufSize);
					break;

				case tsMaxHold:
					if (sysData.trace[i].flags == 0)
					{
						memcpy(sysData.trace[i].value, sysData.prjValue, sizeof(double) * dataBufSize);
						sysData.trace[i].flags++;
					} else
					{
						for (int j = 0; j < dataBufSize; j++)
						{
							if (sysData.trace[i].flags <= 0)
							{
								memcpy(sysData.trace[i].value, sysData.prjValue, sizeof(double) * dataBufSize);
								sysData.trace[i].flags++;
								break;
							}

							if (sysData.prjValue[j] > sysData.trace[i].value[j])
							{
								sysData.trace[i].value[j] = sysData.prjValue[j];
							}
						}
					}

					break;

				case tsMinHold:
					if (sysData.trace[i].flags == 0)
					{
						memcpy(sysData.trace[i].value, sysData.prjValue, sizeof(double) * dataBufSize);
						sysData.trace[i].flags++;
					} else
					{
						for (int j = 0; j < dataBufSize; j++)
						{
							if (sysData.trace[i].flags <= 0)
							{
								memcpy(sysData.trace[i].value, sysData.prjValue, sizeof(double) * dataBufSize);
								sysData.trace[i].flags++;
								break;
							}

							if (sysData.prjValue[j] < sysData.trace[i].value[j])
							{
								sysData.trace[i].value[j] = sysData.prjValue[j];
							}
						}
					}
					break;

				case tsView:
					break;

				case tsBlank:
					break;
			}
		}

		//auto max peak searching
		if (sysData.isAutoPeakMaxing && sysData.marker[sysData.markerSelIndex].state != msClose)
		{
			setMarkerPeakMax(sysData.markerSelIndex, sysData.marker[sysData.markerSelIndex].traceIndex);
		}

		//auto ndb measuring
		if (sysData.markerFctn.isNdBOn && sysData.marker[sysData.markerSelIndex].state != msClose)
		{
			getMarkerFunctionNdB(sysData.markerSelIndex);
		}

		//auto noise measuring
		if (sysData.markerFctn.isMkrNoiseOn && sysData.marker[sysData.markerSelIndex].state != msClose)
		{
			getMarkerFunctionNoise(sysData.markerSelIndex);
		}

		//measure calculate
		if (isMeasureOn() == __SCPI_SUCCESS)
		{
			int count = dataBufSize - 1;
			double absPower = 0;

			sysData.measure.channelPowerStartX = (int) (count / 2) - (int) (count * sysData.measureSetup.channelWidth / sysData.span.span / 2);
			sysData.measure.channelPowerStopX =  (int) (count / 2) + (int) (count * sysData.measureSetup.channelWidth / sysData.span.span / 2);

			//channel power
			if (sysData.measure.isChannelPowerOn)
			{
				for (int i = sysData.measure.channelPowerStartX; i <= sysData.measure.channelPowerStopX; i++)
				{
					absPower += pow(10.0, sysData.trace[sysData.traceSelIndex].value[i] / 10);
				}

				absPower /= (sysData.measure.channelPowerStopX - sysData.measure.channelPowerStartX + 1);
				absPower = absPower * sysData.measureSetup.channelWidth / sysData.bw.rbw;

				if (absPower != 0)
				{
					absPower = 10 * log10(absPower);
				}
			}

			sysData.measure.channelPower = absPower;

			//acpr
			if (sysData.measure.isAcprOn && sysData.span.span > 0)
			{
				double spacePercent = sysData.measureSetup.channelSpace / sysData.span.span;
				double widthPercent = sysData.measureSetup.channelWidth / sysData.span.span;
				double sumPower = 0;

				sysData.measure.acprTxChannelStartX = sysData.measure.channelPowerStartX;
				sysData.measure.acprTxChannelStopX = sysData.measure.channelPowerStopX;

				for (int i = 0; i < sysData.measureSetup.channelNums; i++)
				{
					sysData.measure.acprRects[i].startXL = sysData.measure.channelPowerStartX - (int) ((i + 1) * count * spacePercent);
					sysData.measure.acprRects[i].stopXL = sysData.measure.acprRects[i].startXL + (int) (count * widthPercent);
					sysData.measure.acprRects[i].startXR = sysData.measure.channelPowerStartX + (int) ((i + 1) * count * spacePercent);
					sysData.measure.acprRects[i].stopXR = sysData.measure.acprRects[i].startXR + (int) (count * widthPercent);
				}

				sysData.measure.acprTxChannelBw = sysData.measureSetup.channelWidth;

				for (int i = sysData.measure.acprTxChannelStartX; i <= sysData.measure.acprTxChannelStopX; i++)
				{
					sumPower += pow(10.0, sysData.trace[sysData.traceSelIndex].value[i] / 10);
				}

				sumPower /= (sysData.measure.acprTxChannelStopX - sysData.measure.acprTxChannelStartX + 1);
				sumPower = sumPower * sysData.measureSetup.channelWidth / sysData.bw.rbw;

				if (sumPower != 0)
				{
					sumPower = 10 * log10(sumPower);
				}

				sysData.measure.acprTxChannelPower = sumPower;

				for (int i = 0; i < sysData.measureSetup.channelNums; i++)
				{
					sumPower = 0;

					for (int j = sysData.measure.acprRects[i].startXL; j <= sysData.measure.acprRects[i].stopXL; j++)
					{
						sumPower += pow(10.0, sysData.trace[sysData.traceSelIndex].value[j] / 10);
					}

					sumPower /= (sysData.measure.acprRects[i].stopXL - sysData.measure.acprRects[i].startXL + 1);
					sumPower = sumPower * sysData.measureSetup.channelWidth / sysData.bw.rbw;

					if (sumPower != 0)
					{
						sumPower = 10 * log10(sumPower);
					}

					sysData.measure.acprAdjacent[i].lower = sumPower - sysData.measure.acprTxChannelPower;

					sumPower = 0;

					for (int j = sysData.measure.acprRects[i].startXR; j <= sysData.measure.acprRects[i].stopXR; j++)
					{
						sumPower += pow(10.0, sysData.trace[sysData.traceSelIndex].value[j] / 10);
					}

					sumPower /= (sysData.measure.acprRects[i].stopXR - sysData.measure.acprRects[i].startXR + 1);
					sumPower = sumPower * sysData.measureSetup.channelWidth / sysData.bw.rbw;

					if (sumPower != 0)
					{
						sumPower = 10 * log10(sumPower);
					}

					sysData.measure.acprAdjacent[i].upper = sumPower - sysData.measure.acprTxChannelPower;
				}
			}

			//obw
			if (sysData.measure.isObwOn)
			{
				double sumPower = 0;
				double dotPower[dataBufSize];
				double perValue = (100.0 - sysData.measureSetup.powerPercent) / 200.0;
				double perPower = 0;

				for (int i = 0; i < dataBufSize; i++)
				{
					dotPower[i] = pow(10.0, sysData.trace[sysData.traceSelIndex].value[i] / 10);
					sumPower += dotPower[i];
				}

				perPower = perValue * sumPower;
				sumPower = 0;

				for (int i = 0; i < dataBufSize; i++)
				{
					sumPower += dotPower[i];

					if (sumPower >= perPower)
					{
						sysData.measure.obwStartX = i;
						break;
					}
				}

				sumPower = 0;

				for (int i = sysData.sweep.sweepPoints - 1; i >= 0; i--)
				{
					sumPower += dotPower[i];

					if (sumPower >= perPower)
					{
						sysData.measure.obwStopX = i;
						break;
					}
				}

				sysData.measure.obwBandWidth = qAbs(sysData.measure.obwStopX - sysData.measure.obwStartX) * sysData.span.span / (sysData.sweep.sweepPoints - 1);
				sysData.measure.obwStartFreq = getTheMarkerXValue(sysData.markerSelIndex, sysData.measure.obwStartX);
				sysData.measure.obwStartFreqPower = getTheMarkerYValue(sysData.markerSelIndex, sysData.measure.obwStartX);
				sysData.measure.obwStopFreq = getTheMarkerXValue(sysData.markerSelIndex, sysData.measure.obwStopX);
				sysData.measure.obwStopFreqPower = getTheMarkerYValue(sysData.markerSelIndex, sysData.measure.obwStopX);
			}
		}
	}
}

//获取鉴频数据
void tSysScpi::getDataFromUsbDisc(void)
{
	unsigned char usbDataTemp[64] = { };
	unsigned char usbDataTempR[512] = { };
	int usbMaxLen = sizeof usbRcvBuf / sizeof usbRcvBuf[0];
	int usbLen = 0;
	int usbSumLen = 0;
	bool usbErr = false;
	int usbBeginPos = 0;
	double maxGain = 0;
	double minGain = 0;
	double usbFreq = 0;
	int usbBD = 0x1ff;
	int adDots = 0;
	int usbDataLength = sizeof usbDataTemp / sizeof usbDataTemp[0];
	int usbDataLengthR = sizeof usbDataTempR / sizeof usbDataTempR[0];
	int usbBufMaxLen = sizeof usbRcvBuf / sizeof usbRcvBuf[0];
	int usbFlag = 0;
	double discError = 0;
	int packetFlag = 0;

	if (not sysData.disc.isDiscOn)
	{
		return;
	}

	mutexDisc.lock();
	usbErr = false;
	adDots = 0;
	usbSumLen = 0;
	packetFlag = 0;

	switch (sysData.disc.inputGain)
	{
		case igtx1:
			minGain = -1.65;
			maxGain = -1.0 * minGain;
			usbBD = sysData.disc.discStandard[0];
			discError = (usbBD - 0x1ff) * 3.3 / 0x3ff;
			break;
		case igtx2:
			minGain = -1.65 / 2.0;
			maxGain = -1.0 * minGain;
			usbBD = sysData.disc.discStandard[1];
			discError = (usbBD - 0x1ff) * 1.65 / 0x3ff;
			break;
		case igtx4:
			minGain = -1.65 / 4.0;
			maxGain = -1.0 * minGain;
			usbBD = sysData.disc.discStandard[2];
			discError = (usbBD - 0x1ff) * 0.825 / 0x3ff;
			break;
		case igtx05:
			minGain = -1.65 * 2.0;
			maxGain = -1.0 * minGain;
			usbBD = sysData.disc.discStandard[3];
			discError = (usbBD - 0x1ff) * 6.6 / 0x3ff;
			break;
		case igtx025:
			minGain = -1.65 * 4.0;
			maxGain = -1.0 * minGain;
			usbBD = sysData.disc.discStandard[4];
			discError = (usbBD - 0x1ff) * 13.2 / 0x3ff;
			break;
		default:
			minGain = -1.65;
			maxGain = -1.0 * minGain;
			usbBD = 0x1ff;
			discError = 0;
			break;
	}

	for (int i = 0; i < sysData.sweep.sweepPoints; i++)
	{
		//if off
		if (!sysData.disc.isDiscOn || !sysData.source.isTrackGenOn)
		{
			break;
		}

		//freq control
		usbFreq = sysData.freq.start + i * sysData.span.span / ((sysData.sweep.sweepPoints - 1) * 1.0);
		setDiscriminatorFreq(usbFreq);

		//send
		adDots++;
		usbDataTemp[0] = 0x70;
		usbDataTemp[1] = ~usbDataTemp[0];
		usbDataTemp[2] = (adDots - 1) * 2 + 0;
		usbDataTemp[3] = (adDots - 1) * 2 + 1;
		usbDataTemp[4] = packetFlag;

		//if send failed
		if (!usbDiscriminatorSend(usbDataTemp, usbDataLength))
		{
			usbErr = true;
			break;
		}

		//c8051f320 ad clock 46kHz
		if (i == 0)
		{
			usleep(1000 * 20);
		} else
		{
			usleep(sysData.disc.sweepTime / 1e3 / (sysData.sweep.sweepPoints - 1) / 2);
		}

		//receive
		if ((adDots >= 32) || (i == sysData.sweep.sweepPoints - 1))
		{
			packetFlag++;
			adDots = 0;

			usbDataTemp[0] = 0x60;
			usbDataTemp[1] = ~usbDataTemp[0];

			if (!usbDiscriminatorSend(usbDataTemp, usbDataLength))
			{
				usbErr = true;
				break;
			}

			usleep(2000);

			usbLen = 0;
			if (usbDiscriminatorRecv(usbDataTempR, usbDataLengthR, &usbLen))
			{
				if ((usbSumLen + usbLen) <= usbBufMaxLen)
				{
					for (int j = 0; j < usbLen; j++)
					{
						usbRcvBuf[usbSumLen + j] = usbDataTempR[j];
					}

					usbSumLen += usbLen;
				} else
				{
					usbErr = true;
					break;
				}
			} else
			{
				usbErr = true;
				break;
			}

			if ((!usbErr) && (usbSumLen <= 1216))
			{
				for (int j = 0; j < sysData.sweep.sweepPoints; j++)
				{
					if ((j * 2 + 1) < usbSumLen)
					{
						sysData.prjValue[j] = sysData.disc.dcOffset + (maxGain - minGain) * ((usbRcvBuf[j * 2 + 0] << 8) + usbRcvBuf[j * 2 + 1] - usbBD) / 0x3ff / 0.95;
					}
				}

				//trace copy
				for (int j = 0; j < TRACECOUNT; j++)
				{
					switch (sysData.trace[j].state)
					{
						case tsClearWrite:
							memcpy(sysData.trace[j].value, sysData.prjValue, sizeof sysData.prjValue);
							break;

						case tsMaxHold:
							for (int k = 0; k < sysData.sweep.sweepPoints; k++)
							{
								if (sysData.prjValue[k] > sysData.trace[j].value[k])
								{
									sysData.trace[j].value[k] = sysData.prjValue[k];
								}
							}
							break;

						case tsMinHold:
							for (int k = 0; k < sysData.sweep.sweepPoints; k++)
							{
								if (sysData.prjValue[k] < sysData.trace[j].value[k])
								{
									sysData.trace[j].value[k] = sysData.prjValue[k];
								}
							}
							break;
					}
				}
				//
			}
		}
	}

	mutexDisc.unlock();

	//if communication error or hardeare reset
	if (usbErr)
	{
		mutexDisc.lock();

		usleep(1000 * 2000);
		disconectFromUSBDevice();

		for (int i = 0; i < 5; i++)
		{
			sysData.disc.isDiscOn = connectToDiscriminator();

			if (sysData.disc.isDiscOn)
			{
				usbDiscFailNums = 0;
				break;
			}

			if (i >= 4)
			{
				usbDiscFailNums = 0;
				presetSystem();
				emit showAlarmMessage(getTheTranslatorString("fd module communication error"), getTheTranslatorString("system auto reseting") + "......");
				emit refreshWindowState();
				break;
			}
		}

		mutexDisc.unlock();
	}
}

//获取解调数据
void tSysScpi::getDataFromDemod(void)
{
	if (demodHandle >= 0 && sysData.options.amOn && sysData.mode == smAM)
	{
		double swt = (1.0 / (30e6 / sysData.options.cicValue) * (DEMODSIZE + 2) / 2.0) * 1e3;
		ioctl(demodHandle, 0xf0, 0);
		usleep((unsigned long) (swt * 1000));
		ioctl(demodHandle, 0xf1, 0);
		mutexDemod.lock();
		read(demodHandle, sysData.options.iqData, sizeof sysData.options.iqData);
		mutexDemod.unlock();
		getDemodDataOfAm(sysData.options.iqData, &sysData.options.am_depth, &sysData.options.am_freq, 0, DEMODSIZE);
	} else if (demodHandle >= 0 && sysData.options.fmOn && sysData.mode == smFM)
	{
		double swt = (1.0 / (30e6 / sysData.options.cicValue) * (DEMODSIZE + 2) / 2.0) * 1000.0;
		ioctl(demodHandle, 0xf0, 0);
		usleep(swt * 1000);
		ioctl(demodHandle, 0xf1, 0);
		mutexDemod.lock();
		read(demodHandle, sysData.options.iqData, sizeof sysData.options.iqData);
		mutexDemod.unlock();
		getDemodDataOfFm(sysData.options.iqData, &sysData.options.fm_deviation, &sysData.options.fm_freq, 0, DEMODSIZE);
	}
}


