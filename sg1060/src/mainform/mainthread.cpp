#include <math.h>
#include <sys/ioctl.h>
#include "mainthread.h"

//���̺߳���
void mainThread::run()
{
	emit drawSystemInf();
	emit drawState();	
	//
	struct timeval delay;
	//
	while (true)
	{
		//    //if system is calibrating ���ϵͳ����У׼
		////	if (sysData.isPreamplifierCalibrating || sysData.isFactoryCalibrating || sysData.isUserCalibrating || sysData.isCalibrating || sysData.isAutoSearching || sysLocked)
		////	{
		////	  delay.tv_sec = 0;
		////	  delay.tv_usec = 100 * 1000;
		////	  select(0, NULL, NULL, NULL, &delay);
		////	  continue;
		////	}
		//
		//	//delay	��ʱ
		delay.tv_sec = 0;
		//delay.tv_usec = (lcdRefreshTimes + 60) * 1000;
		delay.tv_usec = (lcdRefreshTimes + 60) * 1000;
		select(0, NULL, NULL, NULL, &delay);
		//
		////	//screen enable ˢ��ʹ��
		//	if (!sysData.system.isScpiScreenRefresh && sysData.scpiData.stateR)
		//	{
		//	  continue;
		//	}
		////
		////	//refresh screen ˢ��
		emit drawState();
		emit drawSystemInf();
	}
}

//���̺߳���
void minorThread::run()
{
	struct timeval delay;
	int points = 0;
	int process = 0;
	while (true)
	{
#if 1
		if (!sysScpi->getAmptStrFromUSBPower())		//USB���ʼ�
		{
			delay.tv_sec = 0;
			delay.tv_usec = 100 * 1000;
			select(0, NULL, NULL, NULL, &delay);
		}
		else
		{
			emit drawUSBPower();
		}
#endif
#if 0
		//ɨ�������
		if (sysScpi->isStartToDrawStepProcess())
		{
			points = (int) sysData.sweep.step.point;

			if (sysData.sweep.direction == sdUp) //����ɨ��ʱ����ͼ�ǵ���
			{
				if (sysData.indexPoints < points - 1)
				{
					sysData.process = (int) (sysData.indexPoints * 100.0 / (points - 1));

					emit drawProcess();
					sysData.indexPoints++;

				} else
				{
					sysData.process = (int) (sysData.indexPoints * 100.0 / (points - 1)); //zpc add
					emit drawProcess(); //zpc add
					sysData.indexPoints = 0;
					sysData.isBeginDrawSingleProcess = false;
				}
			} else //����ɨ��ʱ����ͼ�ǵݼ�
			{
				if (sysData.indexPoints > 0)
				{
					sysData.process = (int) (sysData.indexPoints * 100.0 / (points - 1));
					emit drawProcess();
					sysData.indexPoints--;
				} else
				{
					sysData.process = 1;
					emit drawProcess();
					sysData.indexPoints = sysData.sweep.step.point;
					sysData.isBeginDrawSingleProcess = false;
				}
			}

			//refresh screen
			delay.tv_sec = 0;
			delay.tv_usec = sysData.sweep.step.dwell * 1000;
			select(0, NULL, NULL, NULL, &delay);

		} else if (sysScpi->isStartToDrawListProcess())
		{
			points = sysData.sweep.list.allRow;

			if (sysData.sweep.direction == sdUp) //����ɨ��ʱ����ͼ�ǵ���
			{
				if (sysData.indexPoints < points)
				{
					sysData.process = (int) (sysData.indexPoints * 100.0 / (points - 1));

					emit drawProcess();
					sysData.indexPoints++;
				} else
				{
					sysData.indexPoints = 0;
					sysData.isBeginDrawSingleProcess = false;
				}
			} else //����ɨ��ʱ����ͼ�ǵݼ�
			{
				if (sysData.indexPoints > 0)
				{
					sysData.process = (int) (sysData.indexPoints * 100.0 / (points - 1));
					emit drawProcess();
					sysData.indexPoints--;
				} else
				{
					sysData.process = 1;
					emit drawProcess();
					sysData.indexPoints = sysData.sweep.list.allRow;
					sysData.isBeginDrawSingleProcess = false;
				}
			}

			//refresh screen

			if (sysData.sweep.list.allRow > sysData.indexPoints)
			{
				delay.tv_sec = 0;
				if (sysData.sweep.list.data[sysData.indexPoints].dwell <= 0) //ȷ��ʱ�䲻����0��ʱ����0ʱ�ᵼ������
				{
					sysData.sweep.list.data[sysData.indexPoints].dwell = 20;
				}
				delay.tv_usec = sysData.sweep.list.data[sysData.indexPoints].dwell * 1000;
				select(0, NULL, NULL, NULL, &delay);
			}

		}
		{
			//refresh screen
			//      delay.tv_sec = 0;
			//      delay.tv_usec = 500 * 1000;
			//      delay.tv_usec = (int)sysData.sweep.step.dwell * 180;			//zpc add
			//      select(0, NULL, NULL, NULL, &delay);
		}
#endif
	}
}

