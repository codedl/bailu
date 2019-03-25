#include <math.h>
#include <sys/ioctl.h>
#include "mainthread.h"

//主线程函数
void mainThread::run()
{
	emit drawAll();

	struct timeval delay;

	while (true)
	{
		//if system is calibrating
		if (sysScpi->isCalibrating() || sysData.isAutoSearching || sysLocked)
		{
			delay.tv_sec = 0;
			delay.tv_usec = 100 * 1000;
			select(0, NULL, NULL, NULL, &delay);
			continue;
		}

		//delay
		delay.tv_sec = 0;
		//delay.tv_usec = (lcdRefreshTimes + 50) * 1000;
		delay.tv_usec = (lcdRefreshTimes + 32) * 1000;
		select(0, NULL, NULL, NULL, &delay);

		//screen enable
		if (!sysData.system.isScpiScreenRefresh && sysData.scpiData.stateR)
		{
			continue;
		}

		//refresh screen
		emit drawCanvas();
	}
}

//次线程函数
void minorThread::run()
{
	struct timeval delay;
	//delay.tv_sec = 0;
	//delay.tv_usec = 500 * 1000;

	while (true)
	{
		delay.tv_sec = 0;
		delay.tv_usec = 500 * 1000;
		select(0, NULL, NULL, NULL, &delay);

		//if system is calibrating
		if (sysScpi->isCalibrating() || sysData.isAutoSearching || sysLocked)
		{
			continue;
		}

		//refresh screen
		emit drawTop();
		//	emit drawInput();
		//delay.tv_sec = 0;
		//delay.tv_usec = 500 * 1000;
		select(0, NULL, NULL, NULL, &delay);

		emit drawState();

		//============  save if data======================
//		if(intFlag == 1)
//		{
//			QFile myfile("/home/sky/ifdata.c");
//			if(myfile.open(QIODevice::ReadWrite | QIODevice::Append))
//			{
//				QTextStream dataOut(&myfile);
//				for(int i = 0; i < 800; i++)
//					dataOut << ifDataBuf[i] << ",";
//			}
//			intFlag = 0;
//		}
		//=================================================
	}
}

void fftThread::addWindow(float* re, float* im, int n, float quieoty)
{
	int i, Win_N, Win_J;
	double Win_TempData, Win_M, Win_A, WINDJ;
	//float *WindowData;

	Win_N = n;
	//WindowData = (float*)malloc(Win_N * sizeof(float));
	Win_TempData = 10.0; //产生高斯窗系数
	WINDJ = quieoty; //-0.05;
	Win_M = (Win_N - 1) / 2.0;
	Win_A = (Win_N - 1) / ((2.0 * (Win_N - 1) / 2.0) / Win_TempData);
	Win_J = 0;

	for (i = (int) (-1 * Win_M); i <= (int) (Win_M - 1); i++)
	{
		WindowData[Win_J++] = exp(WINDJ * (Win_A * i / Win_M) * (Win_A * i / Win_M));
	}

	for (i = 0; i <= Win_N - 1; i++)
	{
		re[i] *= WindowData[i];
		im[i] *= WindowData[i];
	}

	//free(WindowData);
}

void fftThread::fft(float *x, float *y, int n, int sign)
{
	int i, j, k, l, m, n1, n2;
	float c, c1, e, s, s1, t, tr, ti;
	for (j = 1, i = 1; i < 16; i++)
	{
		m = i;
		j = 2 * j;
		if (j == n)
			break;
	}
	n1 = n - 1;
	for (j = 0, i = 0; i < n1; i++)
	{
		if (i < j)
		{
			tr = x[j];
			ti = y[j];
			x[j] = x[i];
			y[j] = y[i];
			x[i] = tr;
			y[i] = ti;
		}
		k = (floor)(n / 2);
		while (k < (j + 1))
		{
			j = j - k;
			k = (floor)(k / 2);
		}
		j = j + k;
	}
	n1 = 1;
	for (l = 1; l <= m; l++)
	{
		n1 = 2 * n1;
		n2 = (floor)(n1 / 2);
		e = 3.14159265359 / n2;
		c = 1.0;
		s = 0.0;
		c1 = cos(e);
		s1 = -sign * sin(e);
		for (j = 0; j < n2; j++)
		{
			for (i = j; i < n; i += n1)
			{
				k = i + n2;
				tr = c * x[k] - s * y[k];
				ti = c * y[k] + s * x[k];
				x[k] = x[i] - tr;
				y[k] = y[i] - ti;
				x[i] = x[i] + tr;
				y[i] = y[i] + ti;
			}
			t = c;
			c = c * c1 - s * s1;
			s = t * s1 + s * c1;
		}
	}
	if (sign == -1)
	{
		for (i = 0; i < n; i++)
		{
			x[i] /= n;
			y[i] /= n;
		}
	}
}

void fftThread::czt(float *xr, float *xi, int n, int m, double f1, double f2)
{
	int i, j, n1, n2, len;
	double e, t, ar, ai, ph, pi, tr, ti;
	len = n + m - 1;
	for (j = 1, i = 1; i < 16; i++)
	{
		j = 2 * j;
		if (j >= len)
		{
			len = j;
			break;
		}
	}

	pi = 3.14159265358979;
	ph = 2.0 * pi * (f2 - f1) / (m - 1);
	n1 = (n >= m) ? n : m;
	for (i = 0; i < n1; i++)
	{
		e = ph * i * i / 2.0;
		wr[i] = cos(e);
		wi[i] = sin(e);
		wr1[i] = wr[i];
		wi1[i] = -wi[i];
	}
	n2 = len - n + 1;
	memset(&wr[n], 0, sizeof(float) * (n2 - m));
	memset(&wi[n], 0, sizeof(float) * (n2 - m));

	for (i = n2; i < len; i++)
	{
		j = len - i;
		wr[i] = wr[j];
		wi[i] = wi[j];
	}
	fft(wr, wi, len, 1);

	ph = -2.0 * pi * f1;
	for (i = 0; i < n; i++)
	{
		e = ph * i;
		ar = cos(e);
		ai = sin(e);
		tr = ar * wr1[i] - ai * wi1[i];
		ti = ai * wr1[i] + ar * wi1[i];
		t = xr[i] * tr - xi[i] * ti;
		xi[i] = xr[i] * ti + xi[i] * tr;
		xr[i] = t;
	}

	memset(&xr[n], 0, sizeof(float) * (len - n));
	memset(&xi[n], 0, sizeof(float) * (len - n));
	fft(xr, xi, len, 1);
	for (i = 0; i < len; i++)
	{
		tr = xr[i] * wr[i] - xi[i] * wi[i];
		xi[i] = xr[i] * wi[i] + xi[i] * wr[i];
		xr[i] = tr;
	}

	fft(xr, xi, len, -1);
	for (i = 0; i < m; i++)
	{
		tr = xr[i] * wr1[i] - xi[i] * wi1[i];
		xi[i] = xr[i] * wi1[i] + xi[i] * wr1[i];
		xr[i] = tr;
	}
}

//数据线程函数
void dataThread::run()
{
	int wdFlag = 0;
	int wdCount = 0;
	unsigned long wdIntValue = 0;
	double wdFloatValue = 0;
	double wdAverage = 0;
	char wdData[2] = { };
	double tempValue = 0, val = 0;
	struct timeval delay;

	unsigned char buf1[1] = { 0x00 };
	unsigned char buf2[1] = { 0x54 };

	//delay.tv_sec = 0;
	//delay.tv_usec = 400 * 1000;//120 : start
	//delay.tv_usec = 120 * 1000;

	while (true)
	{
		delay.tv_sec = 0;
		//delay.tv_usec = 400 * 1000;//120 : start
		delay.tv_usec = 120 * 1000;

		select(0, NULL, NULL, NULL, &delay);

		//if system is calibrating
		if (sysData.isPreamplifierCalibrating || sysData.isZcPreamplifierCalibrating || sysData.isZcCalibrating || sysData.isFactoryCalibrating || sysLocked)
		{
			continue;
		}

		//get system temperature
		if (tmptHandle >= 0)
		{
			read(tmptHandle, wdData, sizeof(wdData));

			if (wdData[0] & 0x80)
				tempValue = (((wdData[0] << 8) | wdData[1]) - 65536) * 0.0078125;
			else
				tempValue = ((wdData[0] << 8) | wdData[1]) * 0.0078125;
			
			
			if (tempValue != 0)
			{
				wdCount++;
				val += tempValue;
				if (wdCount > 10)
				{
					tempValue = val / wdCount;
					sysData.temperature = tempValue;
					val = 0;
					wdCount = 0;
				}
			}
		}

		//get spectrum detector data
		if (sysData.isPreseting)
		{
			usleep(1000 * 100);
			return;
		}
	if (!sysData.span.isZeroSpan)
	{
		mutexAmpt.lock();
		getDataFromIF();
		mutexAmpt.unlock();
	}
	}
}

void fftThread::run()
{
	int rbwIndex = 0;
	int stepDelayTime = 10; //ms
	unsigned int ifDelayTime = 0; //ms
	unsigned int dspDelayTime = 100; //ms
	float cztF1 = 0;
	float cztF2 = 0;
	double fftLineValue = 0;
	double fftMaxValue = 0;
	int fftIndex = 0;
	int readFlag = 0;
	int readMax = 0;
	double timeError = 0;
	double fftOffsetValue[5] = { };
	double fftBufValue[MAXSWEEPPOINTS] = { };
	double fftBufValueVBW[MAXSWEEPPOINTS] = { };
	int vbwError = 0;

	double swt = 0;
	unsigned int cic = 0;
	bool amSuccess = false;
	int amFailFlag = 0;
	int amNowStep = 0;
	bool fmSuccess = false;
	int fmFailFlag = 0;
	int fmNowStep = 0;

	unsigned long long fftDataIndex = 0;
	unsigned long long dataSize = 0;

	double fftStartFreq = 0;
	double fftSpan = 0;
	double fftRbw = 0;
	double fftSSpan = 0;

	int midIndex = 0;
	int startIndex = 0;
	int stopIndex = 0;
	double percentValue = 0;

	int fftCount = 0;
	float floatValue = 0;

	bool ddcControled = false;
	iqSave = false;
	iqFlag = 0;
	unsigned long fftBuffer[FFTSIZE] = { 0 };
	QTime zcSwtTime = QTime::currentTime();
	QTime zcTime = QTime::currentTime();
	QTime idleZcTime = QTime::currentTime();

	struct fftBandDef fftBand;

	while (true)
	{
		msleep(50);

		//am
		//if (sysData.options.isDemodOn && demodHandle >= 0 && sysData.options.amOn)
		if (sysData.options.isDemodOn && iqHandle >= 0 && sysData.options.amOn)
		{
			//if setup changed, reset param
			if (sysData.options.am_cfgchanged)
			{
				cic = 30;
				amSuccess = 0;
				amFailFlag = 0;
				amNowStep = 1;
				sysData.options.am_cfgchanged = false;
			}

			if (sysData.options.am_ifbw_auto && sysData.options.amOn)
			{
				//IF BW = 1Mhz
				if (amNowStep == 1)
				{
					//cic
					cic = 30;
					if ((unsigned long) (sysData.options.am_ifbw) != (unsigned long) (IFFS / cic))
					{
						sysData.options.am_ifbw = IFFS / cic;
					}

					//control
					if (!amSuccess && amFailFlag == 0)
					{
						sysData.options.am_ifbw = IFFS * 1.0 / cic;
						sysScpi->demodControl();//no need now
						sysData.options.am_cfgchanged = false;
					}

					usleep(10 * 1000);

					//read and write
					swt = (1.0 / (IFFS / cic) * (DEMODSIZE + 2) / 2.0) * 1e3;
					sysData.options.am_time_us = swt * 1000;
					ioctl(demodHandle, 0xf0, 0);
					usleep((unsigned long) (swt * 1000));
					ioctl(demodHandle, 0xf1, 0);
					mutexDemod.lock();
					//driver to read by contorl doc
					read(demodHandle, sysData.options.iqData, sizeof sysData.options.iqData);
					mutexDemod.unlock();

					//check the result
					if (sysScpi->isDemodOkOfAm(sysData.options.iqData, sysData.options.mData, DEMODSIZE))
					{
						sysScpi->getDemodPeakList();

						if (sysScpi->getAmResult())
						{
							amSuccess = true;
							amFailFlag = 0;
							continue;
						}
					}

					amFailFlag++;

					if (amFailFlag >= 3)
					{
						amNowStep = 2;
						amFailFlag = 0;
						amSuccess = false;
					}
				}

				//IF BW = 20kHz
				if (amNowStep == 2 && sysData.options.amOn)
				{
					//cic
					cic = 1500;

					if ((unsigned long) (sysData.options.am_ifbw) != (unsigned long) (IFFS / cic))
					{
						sysData.options.am_ifbw = IFFS / cic;
					}

					//control
					if (!amSuccess && amFailFlag == 0)
					{
						sysData.options.am_ifbw = IFFS * 1.0 / cic;
						sysScpi->demodControl();
						sysData.options.am_cfgchanged = false;
					}

					usleep(10 * 1000);

					//read and write
					swt = (1.0 / (IFFS / cic) * (DEMODSIZE + 2) / 2.0) * 1e3;
					sysData.options.am_time_us = swt * 1000;
					//ioctl(demodHandle, 0xf0, 0);
					ioctl(iqHandle, 0xf0, 0);
					usleep((unsigned long) (swt * 1000));
					//ioctl(demodHandle, 0xf1, 0);
					ioctl(iqHandle, 0xf1, 0);
					mutexDemod.lock();
					//read(demodHandle, sysData.options.iqData, sizeof sysData.options.iqData);
					read(iqHandle, sysData.options.iqData, sizeof sysData.options.iqData);
					mutexDemod.unlock();

					//check the result
					if (sysScpi->isDemodOkOfAm(sysData.options.iqData, sysData.options.mData, DEMODSIZE))
					{
						sysScpi->getDemodPeakList();

						if (sysScpi->getAmResult())
						{
							amSuccess = true;
							amFailFlag = 0;
							continue;
						}
					}

					amFailFlag++;

					if (amFailFlag >= 3)
					{
						amNowStep = 1;
						amFailFlag = 0;
						amSuccess = false;
					}
				}
			} else
			{
				//read and write
				swt = (1.0 / (sysData.options.am_ifbw) * (DEMODSIZE + 2) / 2.0) * 1e3;
				//ioctl(demodHandle, 0xf0, 0);
				ioctl(iqHandle, 0xf0, 0);
				usleep((unsigned long) (swt * 1000));
				//ioctl(demodHandle, 0xf1, 0);
				ioctl(iqHandle, 0xf1, 0);
				mutexDemod.lock();
				//read(demodHandle, sysData.options.iqData, sizeof sysData.options.iqData);
				read(iqHandle, sysData.options.iqData, sizeof sysData.options.iqData);
				mutexDemod.unlock();

				//check the result
				sysScpi->isDemodOkOfAm(sysData.options.iqData, sysData.options.mData, DEMODSIZE);
				sysScpi->getDemodPeakList();
				sysScpi->getAmResult();
			}

			continue;
		}

		//fm
		//if (sysData.options.isDemodOn && demodHandle >= 0 && sysData.options.fmOn)
		if (sysData.options.isDemodOn && iqHandle >= 0 && sysData.options.fmOn)
		{
			//if setup changed, reset param
			if (sysData.options.fm_cfgchanged)
			{
				cic = 30;
				fmSuccess = 0;
				fmFailFlag = 0;
				fmNowStep = 1;
				sysData.options.fm_cfgchanged = false;
			}

			if (sysData.options.fm_ifbw_auto)
			{
				//IF BW = 1Mhz
				if (fmNowStep <= 1)
				{
					//cic
					cic = 10;
					if ((unsigned long) (sysData.options.fm_ifbw) != (unsigned long) (IFFS / cic))
					{
						sysData.options.fm_ifbw = IFFS / cic;
					}

					//control
					if (!fmSuccess && fmFailFlag == 0)
					{
						sysData.options.fm_ifbw = IFFS * 1.0 / cic;
						sysScpi->demodControl();
						sysData.options.fm_cfgchanged = false;
					}

					usleep(10 * 1000);

					//read and write
					swt = (1.0 / (IFFS / cic) * (DEMODSIZE + 2) / 2.0) * 1e3;
					sysData.options.fm_time_us = swt * 1000;
					//ioctl(demodHandle, 0xf0, 0);
					ioctl(iqHandle, 0xf0, 0);
					usleep((unsigned long) (swt * 1000));
					//ioctl(demodHandle, 0xf1, 0);
					ioctl(iqHandle, 0xf1, 0);
					mutexDemod.lock();
					//read(demodHandle, sysData.options.iqData, sizeof sysData.options.iqData);
					read(iqHandle, sysData.options.iqData, sizeof sysData.options.iqData);
					mutexDemod.unlock();

					//check the result
					if (sysScpi->isDemodOk(sysData.options.iqData, sysData.options.mData, DEMODSIZE))
					{
						sysScpi->getDemodPeakList();

						if (sysScpi->getFmResult())
						{
							fmSuccess = true;
							fmFailFlag = 0;
							continue;
						}
					}

					fmFailFlag++;

					if (fmFailFlag >= 3)
					{
						fmNowStep = 2;
						fmFailFlag = 0;
						fmSuccess = false;
					}
				}

				//IF BW = 20kHz
				if (fmNowStep == 2)
				{
					//cic
					cic = 1500;

					if ((unsigned long) (sysData.options.fm_ifbw) != (unsigned long) (IFFS / cic))
					{
						sysData.options.fm_ifbw = IFFS / cic;
					}

					//control
					if (!fmSuccess && fmFailFlag == 0)
					{
						sysData.options.fm_ifbw = IFFS * 1.0 / cic;
						sysScpi->demodControl();
						sysData.options.fm_cfgchanged = false;
					}

					usleep(10 * 1000);

					//read and write
					swt = (1.0 / (IFFS / cic) * (DEMODSIZE + 2) / 2.0) * 1e3;
					sysData.options.fm_time_us = swt * 1000;
					//ioctl(demodHandle, 0xf0, 0);
					ioctl(iqHandle, 0xf0, 0);
					usleep((unsigned long) (swt * 1000));
					//ioctl(demodHandle, 0xf1, 0);
					ioctl(iqHandle, 0xf1, 0);
					mutexDemod.lock();
					//read(demodHandle, sysData.options.iqData, sizeof sysData.options.iqData);
					read(iqHandle, sysData.options.iqData, sizeof sysData.options.iqData);
					mutexDemod.unlock();

					//check the result
					if (sysScpi->isDemodOk(sysData.options.iqData, sysData.options.mData, DEMODSIZE))
					{
						sysScpi->getDemodPeakList();

						if (sysScpi->getFmResult())
						{
							fmSuccess = true;
							fmFailFlag = 0;
							continue;
						}
					}

					fmFailFlag++;

					if (fmFailFlag >= 3)
					{
						fmNowStep = 1;
						fmFailFlag = 0;
						fmSuccess = false;
					}
				}
			} else
			{
				//read and write
				swt = (1.0 / (sysData.options.fm_ifbw) * (DEMODSIZE + 2) / 2.0) * 1e3;
				sysData.options.fm_time_us = swt * 1000;
				//ioctl(demodHandle, 0xf0, 0);
				ioctl(iqHandle, 0xf0, 0);
				usleep((unsigned long) (swt * 1000));
				//ioctl(demodHandle, 0xf1, 0);
				ioctl(iqHandle, 0xf1, 0);
				mutexDemod.lock();
				//read(demodHandle, sysData.options.iqData, sizeof sysData.options.iqData);
				read(iqHandle, sysData.options.iqData, sizeof sysData.options.iqData);
				mutexDemod.unlock();

				//check the result
				if (sysScpi->isDemodOk(sysData.options.iqData, sysData.options.mData, DEMODSIZE))
				{
					sysScpi->getDemodPeakList();
					sysScpi->getFmResult();
				}
			}

			continue;
		}

		//fft
		if (sysData.fft.isFftOut)
		{
			sysData.fft.isFftOut = false;
		}

		//直采模式
		if (sysScpi->isDirectGatherMode())
		{
			unsigned long lineValue = 0;
			double dBmValue = 0;
			bool deted = false;
			unsigned long long detValue = 0;
			unsigned int detNo = 0;
			unsigned int nextDetIndex = 0;
			unsigned int nextDetValue = sysData.zc.detIndex[nextDetIndex];
			bool interrupted = false;

			double det_MaxValue = 0;
			double det_MinValue = 0;
			int det_TotalNums = 0;
			double det_TotalValue = 0;
			double det_AvgValue = 0;
			double det_SampleValue = 0;
			double det_RMSValue = 0;

			//如果改变了参数，则重置
			if (sysData.zc.reseted)
			{
				sysData.zc.reseted = false;
				sysData.zc.execCount = 0;
			}

			//如果是连续扫描或单次但未执行
			if ((sysData.sweep.sweepSingle && sysData.zc.execCount <= 0) || !sysData.sweep.sweepSingle)
			{
				for (int i = 0; i < sysData.zc.segTotal; i++)
				{

					//空闲时间复位
					idleZcTime.restart();

					//如果退出直采或重置参数
					if (!sysScpi->isDirectGatherMode() || sysData.zc.reseted)
					{
						interrupted = true;
						break;
					}

					//NCO中心频率控制
					getFreqAndUnitString(sysData.zc.segs[i].ddcCenter, 1, sysData.zc.nowCenterStr);
					if (sysData.zc.prvCenterStr != sysData.zc.nowCenterStr)
					{
						sysScpi->ddcControl(sysData.zc.segs[i].ddcCenter);
						strcpy(sysData.zc.prvCenterStr, sysData.zc.nowCenterStr);
					}

					//控制FPGA开始采集数据，直至采集完成
					ioctl(iqHandle, 0xf0, 0x00);
					/*Sets this time to the current time and returns the number of milliseconds 
					       that have elapsed since the last time start() or restart() was called*/
					zcTime.restart();
					while (true)
					{
						usleep(10 * 1000);
						/*returns the number of milliseconds 
					       that have elapsed since the last time start() or restart() was called*/
						if (zcTime.elapsed() >= sysData.zc.swt || !sysScpi->isDirectGatherMode() || sysData.zc.reseted)
						{
							interrupted = !sysScpi->isDirectGatherMode() || sysData.zc.reseted;
							break;
						}
					}

					//如果退出直采或重置参数
					if (!sysScpi->isDirectGatherMode() || sysData.zc.reseted || interrupted)
					{
						interrupted = true;
						break;
					}

					//停止采集数据
					ioctl(iqHandle, 0xf1, 0x00);
					if (!sysScpi->isDirectGatherMode() || sysData.zc.reseted)
					{
						interrupted = true;
						break;
					}

					/*
					 //保存IQ数据，供测试使用
					 QFile file("/home/sky/iq.c");
					 if (!file.exists("/home/sky/iq.c") && file.open(QIODevice::ReadWrite))
					 {
					 QTextStream out(&file);
					 for (int k = 0; k < 2048; k++)
					 out << sysData.zc.IQData[k * 2 + 0] << "," << sysData.zc.IQData[k * 2 + 1] << ",";
					 }
					 */

					//取回IQ数据
					read(iqHandle, &sysData.zc.IQData[0], sizeof sysData.zc.IQData);

					//IQ数据正负处理
					for (int j = 0; j < FFTSIZE; j++)
					{
						if (sysData.zc.IQData[j * 2 + 0] >= 0x800000)
						{
							sysData.zc.iData[j] = sysData.zc.IQData[j * 2 + 0] - 16777216.0;
						} else
						{
							sysData.zc.iData[j] = sysData.zc.IQData[j * 2 + 0] * 1.0;
						}

						if (sysData.zc.IQData[j * 2 + 1] >= 0x800000)
						{
							sysData.zc.qData[j] = sysData.zc.IQData[j * 2 + 1] - 16777216.0;
						} else
						{
							sysData.zc.qData[j] = sysData.zc.IQData[j * 2 + 1] * 1.0;
						}

						if (!sysScpi->isDirectGatherMode() || sysData.zc.reseted)
						{
							interrupted = true;
							break;
						}
					}

					//addWindow
					addWindow(sysData.zc.iData, sysData.zc.qData, FFTSIZE, -0.05);

					//czt
					czt(sysData.zc.iData, sysData.zc.qData, FFTSIZE, FFTSIZE, sysData.zc.cf1, sysData.zc.cf2);

					//数据检波
					for (int j = 0; j < FFTSIZE; j++)
					{
						if (!sysScpi->isDirectGatherMode() || sysData.zc.reseted)
						{
							interrupted = true;
							break;
						}

						//电平转换
						dBmValue = 20 * log10((unsigned long) (sqrt(sysData.zc.iData[j] * sysData.zc.iData[j] + sysData.zc.qData[j] * sysData.zc.qData[j])) + 1e3) + sysData.zc.cicCompensateValue[j] / 2;
						lineValue = qPow(10, dBmValue / 20);
						//lineValue = (unsigned long)(sqrt(sysData.zc.iData[j] * sysData.zc.iData[j] + sysData.zc.qData[j] * sysData.zc.qData[j])) + sysData.zc.cicCompensateValue[j];

						if (!deted)
						{
							det_MaxValue = det_MinValue = det_AvgValue = det_SampleValue = det_TotalValue = lineValue;
							det_RMSValue = lineValue * lineValue;
							det_TotalNums = 1;
							deted = true;
						} else
						{
							det_TotalNums++;
							det_TotalValue += lineValue;
							if (lineValue > det_MaxValue)
								det_MaxValue = lineValue;
							if (lineValue < det_MinValue)
								det_MinValue = lineValue;
							det_RMSValue += lineValue * lineValue;
						}

						//检波方式裁决
						switch (sysData.detector.detMode)
						{
							case dmAuto:
							case dmPosPeak:
							case dmTransient:
								detValue = det_MaxValue;
								break;
							case dmNormal:
								if ((nextDetIndex % 2) == 0)
									detValue = det_MaxValue;
								else
									detValue = det_MinValue;
								break;
							case dmNegPeak:
								detValue = det_MinValue;
								break;
							case dmSample:
								detValue = det_SampleValue;
								break;
						}

						detNo++;
						if (detNo == nextDetValue)
						{
							sysData.zc.dataBuf[nextDetIndex] = detValue;
							nextDetIndex++;
							if (nextDetIndex < sysData.zc.bucLen)
							{
								nextDetValue = sysData.zc.detIndex[nextDetIndex];
							}
							deted = false;
							det_TotalNums = 1;
						}
					}

					//空闲时间消耗
					if (sysData.zc.segTotal >= 1 && sysData.zc.idleSwt >= 60.0)
					{
						zcSwtTime.restart();
						while (zcSwtTime.elapsed() < sysData.zc.idleSwt - idleZcTime.elapsed())
						{
							usleep(1000);
							if (!sysScpi->isDirectGatherMode() || sysData.zc.reseted)
							{
								interrupted = true;
								break;
							}
						}
					}
				}

				//如果扫描被中断
				if (interrupted)
				{
					if (!sysScpi->isDirectGatherMode())
					{
						sysScpi->resetZcParams();
					}

					continue;
				}

				//扫描次数累加
				sysData.zc.execCount++;
				if (sysData.zc.execCount > 2)
				{
					sysData.zc.execCount = 2;
				}
			}
		}

		if (sysData.fft.isFftOut)
		{
			sysData.fft.isFftOut = false;
		}
/*
		//FFT小分辨率模式
		//if (sysData.isFftCalibrating  || (sysScpi->isRequireFFTMeas() && sysData.fft.fftOn && fftHandle >= 0 && ((!sysData.fft.isSweepSingle) || (sysData.fft.isSweepSingle && sysData.fft.sweepCount <= 0))))
		if (sysData.isFftCalibrating || (sysScpi->isRequireFFTMeas() && sysData.fft.fftOn && iqHandle >= 0 && ((!sysData.fft.isSweepSingle) || (sysData.fft.isSweepSingle && sysData.fft.sweepCount <= 0))))
		{
		//	static int x = 0;
			fftBand.start_Band = fftBand.stop_Band = -1;

			//计算延时
			rbwIndex = sysScpi->getRbwIndex();
			ifDelayTime = (unsigned int) (1000.0 * FFTDOTS / (30e6 / RAMCIC[rbwIndex])) + stepDelayTime;
			timeError = sysData.sweep.sweepTime / 1000000.0 / sysData.fft.fftCount - (ifDelayTime + 60 + dspDelayTime);

			fftIndex = sysData.fft.fftIndex;
			fftDataIndex = sysData.fft.fftDataIndex;

			fftStartFreq = sysData.freq.start;
			fftSpan = sysData.span.span;
			fftRbw = sysData.bw.rbw;
			fftSSpan = fftSpan / sysData.fft.fftCount;

			for (int i = 0; i < sysData.fft.fftCount; i++)
			{
				if (!sysData.fft.fftOn || sysData.fft.isFftOut)
				{
					fftIndex = 0;
					fftDataIndex = 0;
					fftBand.start_Band = fftBand.stop_Band = -1;
					break;
				}

				sysData.fft.prvCenterFreq = sysData.fft.nowCenterFreq;
				sysData.fft.nowCenterFreq = fftStartFreq + (i * 2 + 1) * fftSSpan / 2.0;

				mutexFFT.lock();


				//频率控制
				if ((unsigned long long) sysData.fft.prvCenterFreq != (unsigned long long) sysData.fft.nowCenterFreq)
				{
					sysScpi->fftControl(i);
					msleep(50);
				}

				//中频开始采集
				ioctl(iqHandle, 0xf0, 0x00);

				for (unsigned int j = 0; j < ifDelayTime; j++)
				{
					msleep(1);

					if (!sysData.fft.fftOn || sysData.fft.isFftOut)
					{
						fftIndex = 0;
						fftDataIndex = 0;
						break;
					}
				}

				if (!sysData.fft.fftOn || sysData.fft.isFftOut)
				{
					fftIndex = 0;
					fftDataIndex = 0;
					fftBand.start_Band = fftBand.stop_Band = -1;
					mutexFFT.unlock();
					break;
				}

				//设置DSP处理相关参数
				cztF1 = sysData.fft.cF1;
				cztF2 = sysData.fft.cF2;

				//中频停止采集并送入DSP
				if (ioctl(iqHandle, 0xf1, 0x00) == 0)
				{
					//等待DSP处理完成
					msleep(dspDelayTime + 100);

					read(iqHandle, sysData.fft.IQData, sizeof sysData.fft.IQData);

					for (int j = 0; j < FFTSIZE; j++)
					{
						if (sysData.fft.IQData[j * 2 + 0] >= 0x800000)
						{
							sysData.fft.fiData[j] = -(0x1000000 * 1.0 - sysData.fft.IQData[j * 2 + 0] * 1.0);
						} else
						{
							sysData.fft.fiData[j] = sysData.fft.IQData[j * 2 + 0] * 1.0;
						}

						if (sysData.fft.IQData[j * 2 + 1] >= 0x800000)
						{
							sysData.fft.fqData[j] = -(0x1000000 * 1.0 - sysData.fft.IQData[j * 2 + 1] * 1.0);
						} else
						{
							sysData.fft.fqData[j] = sysData.fft.IQData[j * 2 + 1] * 1.0;
						}
					}
					//for (int j = FFTSIZE; j < FFTSIZE * 2; j++)
					//{
					//  sysData.fft.fiData[j] = sysData.fft.fqData[j] = 0.0;
					//}


//					 QFile file("/home/sky/iq.c");
//					 x++;
//					 if (file.open(QIODevice::ReadWrite | QIODevice::Append) && (x == 50 || x == 51))
//					 {
//						 QTextStream out(&file);
//						 for (int k = 0; k < 2048; k++)
//							 out << sysData.fft.fiData[k] << "," << sysData.fft.fqData[k] << ",";
//					 }


					//addWindow
					addWindow(sysData.fft.fiData, sysData.fft.fqData, FFTSIZE, -0.05);

					//czt
					czt(sysData.fft.fiData, sysData.fft.fqData, FFTSIZE, FFTSIZE, cztF1, cztF2);

					for (int j = 0; j < FFTSIZE; j++)
					{
						floatValue = sqrt(sysData.fft.fiData[j] * sysData.fft.fiData[j] + sysData.fft.fqData[j] * sysData.fft.fqData[j]);
						sysData.fft.data[j] = *(unsigned int*) &floatValue;
					}

					//取回FFT处理结果
					//if (read(iqHandle, sysData.fft.data, sizeof sysData.fft.data) == 0)
					{
						readFlag = 0;

						if (sysData.fft.isFftOut)
						{
							fftBand.start_Band = fftBand.stop_Band = -1;
							fftIndex = 0;
							fftDataIndex = 0;
							mutexFFT.unlock();
							break;
						}

						//VBW offset on order 10
						if (rfData.Start_Band == 3)
						{
							for (int j = 0; j < FFTDOTS; j++)
							{
								sysData.fft.dataVbwInit[j] = *(float *) (&sysData.fft.data[FFTDOTS - 1 - j]);
							}
						} else
						{
							for (int j = 0; j < FFTDOTS; j++)
							{
								sysData.fft.dataVbwInit[j] = *(float *) (&sysData.fft.data[j]);
							}
						}

						for (int j = 0; j < FFTDOTS; j++)
						{
							sysData.fft.dataVbw[j] = sysData.fft.dataVbwInit[j];
						}

						if (sysData.bw.vbw < sysData.bw.rbw)
						{
							for (int j = 0; j < FIROFFSET; j++)
							{
								sysData.fft.dataVbwFir[j] = sysData.fft.dataVbwInit[0];
							}

							for (int j = 0; j < FFTDOTS; j++)
							{
								sysData.fft.dataVbwFir[j + FIROFFSET] = sysData.fft.dataVbwInit[j];
							}

							vbwError = qAbs(sysScpi->getRbwIndex() - sysScpi->getVbwIndex());

							if (vbwError >= VBWORDER)
							{
								vbwError = VBWORDER;
							}

							sysScpi->firFilter(sysData.fft.dataVbwFir, FIRWIN, sysData.fft.dataVbw, FFTDOTS + FIROFFSET, FIRORDER, FFTDOTS, vbwError);
						}

						//做二次抽取
						for (int k = 0; k < FFTDOTS; k++)
						{
							fftLineValue = sysData.fft.dataVbw[k];
							fftLineValue = sysScpi->getErrorOffftMeasureCalculate(fftLineValue, k, 0, FFTDOTS - 1);

							//赋值给BUC
							if (fftIndex == 0)
							{
								dataSize = sysData.fft.fftSigleSumPoint[fftIndex];
							} else
							{
								dataSize = sysData.fft.fftSigleSumPoint[fftIndex] - sysData.fft.fftSigleSumPoint[fftIndex - 1];
							}

							//数据检波
							//dmAuto, dmNormal, dmNegPeak, dmSample, dmPosPeak, dmQuasiPeak , dmAverage, dmRMS
							switch (sysData.detector.detMode)
							{
								case 1: //常态
									if ((fftIndex % 2) == 0)
									{
										if (fftDataIndex == 0 || fftLineValue > fftMaxValue)
										{
											fftMaxValue = fftLineValue;
											fftBand.start_Band = rfData.Start_Band;
											fftBand.stop_Band = rfData.Stop_Band;
										}
										//if (fftDataIndex == 0) fftMaxValue = fftLineValue; else if (fftLineValue > fftMaxValue) fftMaxValue = fftLineValue;
									} else
									{
										if (fftDataIndex == 0 || fftLineValue < fftMaxValue)
										{
											fftMaxValue = fftLineValue;
											fftBand.start_Band = rfData.Start_Band;
											fftBand.stop_Band = rfData.Stop_Band;
										}
										//if (fftDataIndex == 0) fftMaxValue = fftLineValue; else if (fftLineValue < fftMaxValue) fftMaxValue = fftLineValue;
									}
									break;
								case 3: //取样
									if (fftDataIndex == (unsigned long long) (dataSize / 2))
									{
										fftMaxValue = fftLineValue;
										fftBand.start_Band = rfData.Start_Band;
										fftBand.stop_Band = rfData.Stop_Band;
									}
									break;
								case 2: //负峰
									if (fftDataIndex == 0 || fftLineValue < fftMaxValue)
									{
										fftMaxValue = fftLineValue;
										fftBand.start_Band = rfData.Start_Band;
										fftBand.stop_Band = rfData.Stop_Band;
									}
									break;
								case 0: //自动
								default: //正峰
									if (fftDataIndex == 0 || fftLineValue > fftMaxValue)
									{
										fftMaxValue = fftLineValue;
										fftBand.start_Band = rfData.Start_Band;
										fftBand.stop_Band = rfData.Stop_Band;
									}
									break;
							}

							fftDataIndex++;

							if (fftDataIndex >= dataSize)
							{
								fftDataIndex = 0;

								if (!sysData.fft.fftOn || sysData.fft.isFftOut)
								{
									fftDataIndex = 0;
									fftIndex = 0;
									break;
								}

								fftBufValue[fftIndex] = fftMaxValue;
								sysData.fft.bands[fftIndex].start_Band = fftBand.start_Band;
								sysData.fft.bands[fftIndex].stop_Band = fftBand.stop_Band;

								fftIndex++;

								if (fftIndex >= sysData.sweep.sweepPoints)
								{
									fftDataIndex = 0;
									fftIndex = 0;
									break;
								}
							}
						}

						//fill the databuf
						for (int j = 0; j < sysData.sweep.sweepPoints; j++)
						{
							if (!sysData.fft.fftOn || sysData.fft.isFftOut)
							{
								break;
							}

							sysData.fft.fDataBuf[j] = fftBufValue[j];
						}
					}
				}

				mutexFFT.unlock();

				//手动延时
				if (timeError >= 10.0)
				{
					for (unsigned long j = 0; j < (unsigned long) timeError / 2; j++)
					{
						msleep(1);

						if (!sysData.fft.fftOn || sysData.fft.isFftOut)
						{
							fftIndex = 0;
							fftDataIndex = 0;
							break;
						}
					}
				}
			}

			//single sweep control
			if (sysData.fft.fftOn && sysData.fft.isSweepSingle && sysData.fft.sweepCount <= 0)
			{
				sysData.fft.sweepCount++;
			}

			for (int i = 0; i < TRACECOUNT; i++)
			{
				if (sysData.trace[i].flags <= 0)
				{
					sysData.trace[i].flags++;
				}
			}

			if (sysData.fft.isFftOut)
			{
				fftIndex = 0;
				fftDataIndex = 0;
			}

			isFFtnd = true;
		}
*/
	}
}

//获取中频数据
void dataThread::getDataFromIF(void)
{

	if (sysData.disc.isDiscOn)
	{
		sysScpi->getDataFromUsbDisc();
	} else if (sysData.options.amOn || sysData.options.fmOn)
	{
		//sysScpi->getDataFromDemod();
		sysScpi->getDataFromIF();
		
	} else
	{
		sysScpi->getDataFromIF();
	}
}

//获取鉴频数据
void dataThread::getDataFromDisc(void)
{

}
