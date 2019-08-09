/*
 * bl_tq3358_rf.c
 *
 *  Created on: 2015-12-8
 *      Author: Administrator
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/ioport.h>
#include <linux/miscdevice.h>
#include <mach/hardware.h>
#include <asm/io.h>

#define DEVICE_NAME "BL_TQ3358_RF"
#define RF_BASEADDR  0x02000000     //RF base addr
#define RF_SIZE      0xFF           //RF addr size
#define S0  0x00
#define S1	0x04
#define S2	0x08
#define S3	0x0C
#define S4	0x10
#define S5	0x14
#define S6	0x18
#define S7	0x1C
#define S8	0x20
#define S9	0x24
#define S10	0x28
#define S11	0x2C
#define S12	0x30
#define S13	0x34
#define S14	0x38
//#define S15 0x3C

#define ADF4350_ADDR  S5
#define RF_LOAD_DATA 0x7E

#define SYSCLK_RST_OFFSET     					0x00
#define ADF4360_SLE_OFFSET    					0x01
#define ADF4360_SCLK_OFFSET   					0x02
#define ADF4360_SDAT_OFFSET   					0x03
#define DDS_FTW_HIGH_OFFSET   					0x04
#define DDS_FTW_LOW_OFFSET    					0x05
#define DDS_FTW_SLEN_OFFSET   					0x06
#define ADC_CHANNEL_SEL_OFFSET 					0x07
#define CIC_CONFIG_DATA_OFFSET					0x08
#define CIC_CONFIG_VALID_OFFSET					0x09
#define CIC_RST_OFFSET							0x0A
#define CIC_SHIFT_OFFSET						0x0B
#define FFT_FRAME_TRIG_OFFSET					0x0C
#define FFT_LENGTH_OFFSET						0x0D
#define FFT_CONFIG_VALID_OFFSET					0x0E

#define DDS_STEP_FTW_HIGH_OFFSET				210
#define DDS_STEP_FTW_LOW_OFFSET					211
#define LO1_HLSTART_NINT_OFFSET					212
#define LO1_HLSTART_NFRAC_HIGH_OFFSET			213
#define LO1_HLSTART_NFRAC_LOW_OFFSET			214
#define LO1_HSTEP_QUART_HIGH_OFFSET				215
#define LO1_HSTEP_QUART_LOW_OFFSET				216
#define LO1_HHSTART_NINT_OFFSET					217
#define LO1_HHSTART_NFRAC_HIGH_OFFSET  			218
#define LO1_HHSTART_NFRAC_LOW_OFFSET			219
#define DDS_STEP_NUM_OFFSET						220
#define LO1_HLSTEP_NUM_HIGH_OFFSET				221
#define LO1_HLSTEP_NUM_LOW_OFFSET				222
#define LO1_HHSTEP_NUM_HIGH_OFFSET				223
#define LO1_HHSTEP_NUM_LOW_OFFSET				224
#define LO1_HH1STEP_NUM_OFFSET					225
#define LO1_HH2STEP_NUM_OFFSET					226
#define LO1_HH3STEP_NUM_OFFSET					227
#define LO1_HH4STEP_NUM_OFFSET					228
#define SWEEP_TRIG_OFFSET						229
#define SWEEP_CONTINUE_OFFSET					230
#define SWEEP_BAND_INDEX_OFFSET					231
#define HIGH_BAND_INDEX_OFFSET					232


//ADF4360 操作指令
static int PA = 0x000C;
#define SET_DATA_BIT0(offset)	do { iowrite16( (PA=PA|0x01), remapbuf + offset );wmb(); } while (0)
#define CLR_DATA_BIT0(offset)	do { iowrite16( (PA=PA&0xFE), remapbuf + offset );wmb(); } while (0)
#define SET_CLK_BIT1(offset)	do { iowrite16( (PA=PA|0x02), remapbuf + offset );wmb(); } while (0)
#define CLR_CLK_BIT1(offset)	do { iowrite16( (PA=PA&0xFD), remapbuf + offset );wmb(); } while (0)

#define SET_CS_BIT2(offset)	do { iowrite16( (PA=PA|0x04|0x08), remapbuf + offset );wmb(); } while (0)
#define CLR_CS_BIT2(offset)	do { iowrite16( (PA=PA&0xFB), remapbuf + offset );wmb(); } while (0)
#define SET_CS_BIT3(offset)	do { iowrite16( (PA=PA|0x08|0x04), remapbuf + offset );wmb(); } while (0)
#define CLR_CS_BIT3(offset)	do { iowrite16( (PA=PA&0xF7), remapbuf + offset );wmb(); } while (0)
#define SET_CS_BIT4(offset)	do { iowrite16( (PA=PA|0x10), remapbuf + offset );wmb(); } while (0)
#define CLR_CS_BIT4(offset)	do { iowrite16( (PA=PA&0xEF), remapbuf + offset );wmb(); } while (0)

#define SET_CS_BIT2_PU(offset)	do { iowrite16( (PA=PA|0x04), remapbuf + offset );wmb(); } while (0)
#define SET_CS_BIT3_PU(offset)	do { iowrite16( (PA=PA|0x08), remapbuf + offset );wmb(); } while (0)

extern u8 clockbit[];
volatile u8 * remapbuf;
static struct resource * region_resource;

#define DEBUG_BL
#ifdef DEBUG_BL
	#define printd(fmt,args...) printk("[RF_DEBUG:] "fmt,##args)
#else
	#define printd(fmt,args...)
#endif

static void input_ADF4350_2(u32 data, u8 addr)
{
	int i;
	CLR_DATA_BIT0(addr);
	CLR_CLK_BIT1(addr);
	CLR_CS_BIT2(addr);

	for (i = 0; i < 32; i++)
	{
		if ((data & 0x80000000) == 0)
			CLR_DATA_BIT0(addr);
		else
			SET_DATA_BIT0(addr);

		SET_CLK_BIT1(addr);
		CLR_CLK_BIT1(addr);
		data = data << 1;
	}

	SET_CS_BIT2_PU(addr);
	CLR_CS_BIT2(addr);
	// udelay(100);
}

static void input_ADF4350_3(u32 data, u8 addr)
{
	int i;
	CLR_DATA_BIT0(addr);
	CLR_CLK_BIT1(addr);

	CLR_CS_BIT3(addr);

	for (i = 0; i < 32; i++)
	{
		if ((data & 0x80000000) == 0)
			CLR_DATA_BIT0(addr);
		else
			SET_DATA_BIT0(addr);

		SET_CLK_BIT1(addr);
		CLR_CLK_BIT1(addr);
		data = data << 1;
	}

	SET_CS_BIT3_PU(addr);
	CLR_CS_BIT3(addr);
	// udelay(100);
}

//跟踪源二本
static void input_ADF4350_Track_2(u32 data, u8 addr)
{
	int i;
	CLR_CLK_BIT1(addr);
	SET_CS_BIT2(addr);
	ndelay(20);
	CLR_CS_BIT2(addr);
	ndelay(20);

	for (i = 0; i < 32; i++)
	{
		//CLR_CLK_BIT1(addr);
		if ((data & 0x80000000) == 0)
			CLR_DATA_BIT0(addr);
		else
			SET_DATA_BIT0(addr);

		SET_CLK_BIT1(addr);
		CLR_CLK_BIT1(addr);
		data = data << 1;
	}

	SET_CS_BIT2(addr);
	// CLR_CS_BIT2(addr);
}

//跟踪源三本
static void input_ADF4350_Track_3(u32 data, u8 addr)
{
	int i;
	//SET_CS_BIT2(addr);
	SET_CS_BIT3(addr);

	//CLR_DATA_BIT0(addr);
	CLR_CLK_BIT1(addr);

	ndelay(200);
	CLR_CS_BIT3(addr);
	ndelay(200);
	for (i = 0; i < 32; i++)
	{
		if ((data & 0x80000000) == 0)
			CLR_DATA_BIT0(addr);
		else
			SET_DATA_BIT0(addr);

		SET_CLK_BIT1(addr);
		CLR_CLK_BIT1(addr);
		data = data << 1;
	}

	ndelay(2000);
	SET_CS_BIT3(addr);
	//CLR_CS_BIT3(addr);
}

static void input_ADF4350_Track(u32 data, u8 addr)
{
	int i;
	CLR_DATA_BIT0(addr);
	CLR_CLK_BIT1(addr);
	CLR_CS_BIT4(addr);

	for (i = 0; i < 32; i++)
	{
		if ((data & 0x80000000) == 0)
			CLR_DATA_BIT0(addr);
		else
			SET_DATA_BIT0(addr);

		SET_CLK_BIT1(addr);
		CLR_CLK_BIT1(addr);
		data = data << 1;
	}

	SET_CS_BIT4(addr);
	CLR_CS_BIT4(addr);
}

//源模块衰减器控制
static void sourceAttdownload(u8 data, u8 addr)
{
	int i;
	CLR_DATA_BIT0(addr);
	CLR_CLK_BIT1(addr);
	CLR_CS_BIT2(addr);

	for (i = 0; i < 8; i++)
	{
		if ((data & 0x80) == 0)
			CLR_DATA_BIT0(addr);
		else
			SET_DATA_BIT0(addr);

		SET_CLK_BIT1(addr);
		CLR_CLK_BIT1(addr);
		data = data << 1;
	}

	SET_CS_BIT2_PU(addr);
	CLR_CS_BIT2(addr);
	//udelay(100);
}

//衰减器下发
static void attDownload(u8 data, u8 addr)
{
	int i;
	CLR_DATA_BIT0(addr);
	CLR_CLK_BIT1(addr);
	CLR_CS_BIT2(addr);
	CLR_CS_BIT3(addr);

	for (i = 0; i < 8; i++)
	{
		if ((data & 0x80) == 0)
			CLR_DATA_BIT0(addr);
		else
			SET_DATA_BIT0(addr);

		SET_CLK_BIT1(addr);
		CLR_CLK_BIT1(addr);
		data = data << 1;
	}

	SET_CS_BIT2_PU(addr);
	CLR_CS_BIT2(addr);
	//udelay(100);
}

//跟踪源衰减器下发
static void trackAttDownload(u8 data, u8 addr)
{
	int i;
	CLR_DATA_BIT0(addr);
	CLR_CLK_BIT1(addr);
	CLR_CS_BIT2(addr);
	CLR_CS_BIT3(addr);

	for (i = 0; i < 8; i++)
	{
		if ((data & 0x80) == 0)
			CLR_DATA_BIT0(addr);
		else
			SET_DATA_BIT0(addr);

		SET_CLK_BIT1(addr);
		CLR_CLK_BIT1(addr);
		data = data << 1;
	}

	SET_CS_BIT3_PU(addr);
	CLR_CS_BIT3(addr);
	// udelay(100);
}

static int bl_rf_read(struct file *filp, char __user *buff, size_t count, loff_t *offp)
{
	return 0;
}

static ssize_t bl_rf_write(struct file *filp, const char __user *buff, size_t count, loff_t *offp)
{
	return 0;
}

static int bl_rf_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	printd("bl_rf_ioctl cmd = 0x%x, arg = 0x%08x\r\n", cmd, arg);

/*
	if (cmd == 0xff)
	{
		input_ADF4350_Track(0x00000005, ADF4350_ADDR);  udelay(100);
		input_ADF4350_Track(0x0095002C, ADF4350_ADDR);  udelay(100);
		input_ADF4350_Track(0x000004B3, ADF4350_ADDR);	udelay(100);
		input_ADF4350_Track(0x00004EC2, ADF4350_ADDR);	udelay(100);
  	input_ADF4350_Track(0x080080C9, ADF4350_ADDR); udelay(100);
  	input_ADF4350_Track(0x00CA8038, ADF4350_ADDR); udelay(100); //2026.4MHz

//		input_ADF4350_Track(0x08009F41, ADF4350_ADDR);	udelay(100);
//		input_ADF4350_Track(0x00CA88D8, ADF4350_ADDR);	udelay(100); //2026.4MHz + 15KHz
	} else if (cmd == 0xfd)
	{
		attDownload(arg, S9);
	} else if (cmd == 0xfe)
	{
		trackAttDownload(arg, S9);
	} else if (cmd == 0xfc)
	{
		sourceAttdownload(arg, S8);
	} else if (cmd == 0xfb)
	{
		if (arg == 0x00)
		{
			input_ADF4350_Track_2(0x00000005, S7); //udelay(100);  //二本振送数
			input_ADF4350_Track_2(0x0095002C, S7); //udelay(100);
			input_ADF4350_Track_2(0x000004B3, S7); //udelay(100);
			input_ADF4350_Track_2(0x00004EC2, S7); //udelay(100);
  	  input_ADF4350_Track_2(0x080080C9, S7); //udelay(100);
		  input_ADF4350_Track_2(0x00CA8038, S7); //udelay(100); //2026.4MHz

//			input_ADF4350_Track_2(0x08009F41, S7); //udelay(100);
//			input_ADF4350_Track_2(0x00CA88D8, S7); //udelay(100); //2026.4MHz + 15KHz

			udelay(5);
			input_ADF4350_Track_3(0x00000005, S7); //udelay(100); // 三本振上电
			input_ADF4350_Track_3(0x00A5002C, S7); //udelay(100);
			input_ADF4350_Track_3(0x000004B3, S7); //udelay(100);
			input_ADF4350_Track_3(0x00004EC2, S7); //udelay(100);
			input_ADF4350_Track_3(0x080080C9, S7); //udelay(100);
			input_ADF4350_Track_3(0x00A50070, S7); //udelay(100);


			input_ADF4350_Track_3(0x00000005, S7); //udelay(100); // 三本振送数
			input_ADF4350_Track_3(0x00A5002C, S7); //udelay(100);
			input_ADF4350_Track_3(0x000004B3, S7); //udelay(100);
			input_ADF4350_Track_3(0x00004EC2, S7); //udelay(100);
		  input_ADF4350_Track_3(0x080080C9, S7); //udelay(100);
		  input_ADF4350_Track_3(0x00A50070, S7); //udelay(100); //826.4MHz

//			input_ADF4350_Track_3(0x08008FA1, S7); //udelay(100);
//			input_ADF4350_Track_3(0x00A508D8, S7); //udelay(100); //826.4MHz + 15kHz
		} else if (arg == 0x01)
		{
			input_ADF4350_Track_3(0x00000005, S7); //udelay(100); // 三本振
			input_ADF4350_Track_3(0x00A5002C, S7); //udelay(100);
			input_ADF4350_Track_3(0x000004B3, S7); //udelay(100);
			input_ADF4350_Track_3(0x00004EE2, S7); //udelay(100);
			input_ADF4350_Track_3(0x080080C9, S7); //udelay(100);
			input_ADF4350_Track_3(0x00A50070, S7); //udelay(100); //826.4MHz
		}
	} else if (cmd == 0xfa)
	{
		input_ADF4350_Track_2(arg, S7);
		//udelay(100);  //二本振
	} else if (cmd == 0xf9) //射频通道选择
	{
		iowrite8(arg, remapbuf + 0x48);	wmb(); //IF_CHN_CRTL_R2
	} else if (cmd == 0xef) //DDC HIGH
	{
		iowrite16(arg, remapbuf + 0x4c); wmb();
	} else if (cmd == 0xee) //DDC LOW
	{
		iowrite16(arg, remapbuf + 0x58); wmb();
	} else
	{
		iowrite8(arg, remapbuf + cmd); wmb();
	}
*/
	switch(cmd)
	{
//	  case 0xa0:
//		  break;
	  default:
		  iowrite16(arg, remapbuf + cmd * 2); wmb();
		  break;
	}

	return 0;
}

static struct file_operations dev_fops = {
		.owner = THIS_MODULE,
		.read = bl_rf_read,
		.write = bl_rf_write,
		.unlocked_ioctl = bl_rf_ioctl,
};

static struct miscdevice misc = {
		.minor = MISC_DYNAMIC_MINOR,
		.name = DEVICE_NAME,
		.fops = &dev_fops,
};

static int __init dev_init(void)
{
	int ret = misc_register(&misc);
	remapbuf = ioremap(RF_BASEADDR, RF_SIZE);

	//送内参考、跟踪源衰减为31dB
	trackAttDownload(0x40, S9);

	//源模块
	//sourceAttdownload(0xC0, S8);

	//二本送数
	/*
	 input_ADF4350_2(0x00000005, ADF4350_ADDR); udelay(100);
	 input_ADF4350_2(0x0095002C, ADF4350_ADDR); udelay(100);//mdelay(1);
	 input_ADF4350_2(0x000004B3, ADF4350_ADDR); udelay(100);// mdelay(1);
	 input_ADF4350_2(0x00004EC2, ADF4350_ADDR); udelay(100);// mdelay(1);
	 input_ADF4350_2(0x08008191, ADF4350_ADDR); udelay(100);//mdelay(1);
	 input_ADF4350_2(0x00770000, ADF4350_ADDR); udelay(100);//mdelay(1);  1190MHz//
	 */

	input_ADF4350_2(0x00000005, ADF4350_ADDR); udelay(100);
	input_ADF4350_2(0x0095002C, ADF4350_ADDR); udelay(100);//mdelay(1);
	input_ADF4350_2(0x000004B3, ADF4350_ADDR); udelay(100);// mdelay(1);
	input_ADF4350_2(0x00004EC2, ADF4350_ADDR); udelay(100);// mdelay(1);
	input_ADF4350_2(0x08008191, ADF4350_ADDR); udelay(100);//mdelay(1);
	input_ADF4350_2(0x00780000, ADF4350_ADDR); udelay(100);//mdelay(1);  1200MHz//

	//三本送数
	input_ADF4350_3(0x00000005, ADF4350_ADDR); udelay(100);//mdelay(1);
	input_ADF4350_3(0x00A5002C, ADF4350_ADDR); udelay(100);//mdelay(1);
	input_ADF4350_3(0x000004B3, ADF4350_ADDR); udelay(100);// mdelay(1);
	input_ADF4350_3(0x00004EC2, ADF4350_ADDR); udelay(100);// mdelay(1);
	input_ADF4350_3(0x080080C9, ADF4350_ADDR); udelay(100);//mdelay(1);
	input_ADF4350_3(0x00A10000, ADF4350_ADDR); udelay(100);// mdelay(1);//805MHz
//	input_ADF4350_3(0x08008fa1, ADF4350_ADDR); udelay(100);//mdelay(1);
//	input_ADF4350_3(0x00A10018, ADF4350_ADDR); udelay(100);// mdelay(1);//805MHz + 15kHz


	iowrite16(0x4962, remapbuf + 0x4c); wmb(); //DDC_PHASE_CTRL_R2
	iowrite16(0xfc96, remapbuf + 0x58); wmb(); //DDC_PHASE_CTRL_R1
	iowrite16(0x00, remapbuf + 0x7c); wmb();

	printk(DEVICE_NAME " initialized successed\n");

	return ret;
}

static void __exit dev_exit(void)
{
	if (NULL != region_resource)
	{
		iounmap(remapbuf);
		release_mem_region(RF_BASEADDR, RF_SIZE);
	}

	misc_deregister(&misc);
}

module_init( dev_init);
module_exit( dev_exit);

MODULE_LICENSE("GPL");
MODULE_VERSION ("V3.00");
MODULE_AUTHOR("anhui egret electronic company");
MODULE_DESCRIPTION("module driver of rf");
