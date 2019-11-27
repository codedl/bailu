#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/ioport.h>
#include <linux/miscdevice.h>
#include <mach/hardware.h>
#include <asm/io.h>
#include <linux/irq.h>
#include <asm/irq.h>
#include <linux/interrupt.h>

#define __DEBUG 0
#if __DEBUG
#define printd(fmt,args...) printk("[frontend driver debug]: "fmt,##args)
#else
#define printd(fmt,args...)
#endif

#define DEVICE_NAME "BL_TQ3358_FE"

#define RF_BASEADDR  0x02000000     //RF base addr
#define RF_SIZE      0xFFFFF           //RF addr size

#define RF_ADRS0_OFFSET  0
#define RF_ADRS1_OFFSET  4
#define RF_ADRS2_OFFSET  8

volatile u8 * remapBuf;
static int version_fpga;
//static struct resource * region_resource;
//static unsigned char val_74hc595;
static int val_1018;
//RF_ADRS(0)
#define  REG_SET(offset)   do{ iowrite16(0x01, remapBuf + offset * 2); wmb(); } while (0)
#define  REG_CLR(offset)   do{ iowrite16(0x00, remapBuf + offset * 2); wmb(); } while (0)

#define  REG_SET_R(offset)   do{ iowrite16(0x00, remapBuf + offset * 2); wmb(); } while (0)
#define  REG_CLR_R(offset)   do{ iowrite16(0x01, remapBuf + offset * 2); wmb(); } while (0)

#define  REG_WRITE(offset,value)   do{ iowrite16(value, remapBuf + offset * 2); wmb(); } while (0)

#define LO_SDIO_OFFSET          	222 //	203
#define LO_SCLK_OFFSET          	222 //	204

#define DCM_RESET_OFFSET		   	2
#define ADF4360_SLE_OFFSET			3
#define ADF4360_SCLK_OFFSET			4
#define ADF4360_SDAT_OFFSET			5

#define REF_SETLECT_OFFSET 		  	12  //set ref vol
#define WORK_STATE_OFFSET         	19  //model power control

#define HBAND_ATT_EN_OFFSET       	238
#define LBAND_ATT_EN_OFFSET       	239
#define IF_ATT_EN_OFFSET          	240
#define HC595_EN_OFFSET			  	241
//第一本振送数地址
#define HMC703_SDIO_OFFSET     		208//data of first osc
#define HMC703_SCLK_OFFSET     		206//clk of first osc
#define HMC703_EN_OFFSET       		207//data enable of first osc
//第二本振送数地址
#define HMC840_SDIO_OFFSET     		205//data of second soc
#define HMC840_SCLK_OFFSET     		204//clk of second osc
#define HMC840_EN_OFFSET       		203//data enable of second osc
//第三本振送数地址
#define HMC835_SDIO_OFFSET     		202//data of second soc
#define HMC835_SCLK_OFFSET     		201//6clk of second osc
#define HMC835_EN_OFFSET       		200//data enable of second osc


#define DDS_CHANNEL_OFFSET        	207

#define REF_TIMER_CLK_OFFSET      	200
#define REF_TIMER_SDIO_OFFSET     	208
#define REF_TIMER_SYN_OFFSET      	201

#define RF_SDIO_OFFSET            	242
#define RF_SCLK_OFFSET				243
#define RF_ALE1_OFFSET				239
#define RF_ALE2_OFFSET				238
#define RF_ALE3_OFFSET				241
#define RF_ALE4_OFFSET            IF_ATT_EN_OFFSET

#define HIGHBAND_ATT_CLK			264
#define LOWBAND_ATT_CLK				265

#define ATT_SLEN_ADDR				256
#define ATT_SCLK_ADDR				257
#define ATT_SDAT_ADDR				258

#define PA_CHANEL_SDAT_ADDR         259
#define PA_CHANEL_CLKLEN_ADDR       260
#define PA_CHANEL_JCLK_ADDR         261
#define VERSION_FPGA                1

#define DC_PA_ATT_SEL				217//DC放大器增益和DC衰减、通道选择、前置放大选择送数地址

#define ADDR_DC_AMP_EN				210//DC放大器增益控制使能送数地址
#define ADDR_DC_AMP_SCLK			211//DC放大器增益控制时钟送数地址
#define ADDR_DC_AMP_SDIO			212//DC放大器增益控制数据送数地址

#define ADDR_DC_CH_EN				214//DC衰减、通道选择、前置放大使能
#define ADDR_DC_CH_SCLK				213//DC衰减、通道选择、前置放大时钟
#define ADDR_DC_CH_SDIO				215//DC衰减、通道选择、前置放大数据


#define GPIO_TO_PIN(bank, gpio) (32 * (bank) + (gpio))

static void input_74HC595(u8 data)
{
	int i;
	u8 d;
	REG_CLR(PA_CHANEL_CLKLEN_ADDR);
	ndelay(50);
	REG_SET(PA_CHANEL_JCLK_ADDR);

	//写入数据
	ndelay(10);
	d = data;
		
	for (i = 0; i < 8; i++)
	{
		if (d & 0x80)
			REG_SET(PA_CHANEL_SDAT_ADDR);
		else
			REG_CLR(PA_CHANEL_SDAT_ADDR);

		ndelay(10);
		REG_CLR(PA_CHANEL_JCLK_ADDR);

		ndelay(10);
		REG_SET(PA_CHANEL_JCLK_ADDR);

		d = d << 1;
		ndelay(10);
	}
	ndelay(10);
	REG_SET(PA_CHANEL_CLKLEN_ADDR);
	ndelay(10);
	REG_CLR(PA_CHANEL_CLKLEN_ADDR);
}

//前放，波段选择
static void setPreampt(u8 value)
{
	printd("set preampt %d\n",value);
	input_74HC595(value);
}

//中频衰减器设置
static void setIfAtt(u8 value)
{
	printd("set if att %d\n", value);
	input_74HC595(value);
}

//内外参考选择
static void setRefSelect(u8 value)
{
	printd("ref select %d\n",value);

	if(value == 1){
		REG_CLR(REF_SETLECT_OFFSET);
		printd(" ref select value = 0\n");
	} else {
		REG_SET(REF_SETLECT_OFFSET);
		printd(" ref select value = 1\n");
	}
}

static void input_hmc703(u32 data)
{
	int i = 0, d,bit = 1 << 31;

	//hmc mode
	REG_CLR(HMC703_EN_OFFSET);
	ndelay(50);
	REG_CLR(HMC703_SCLK_OFFSET);
	ndelay(50);
	REG_SET(HMC703_EN_OFFSET);

	d = data;
	
	for (i = 0; i < 32; ++i)
	{
		if (d & bit)
			REG_SET(HMC703_SDIO_OFFSET);
		else
			REG_CLR(HMC703_SDIO_OFFSET);
		ndelay(50);
		REG_SET(HMC703_SCLK_OFFSET);
		ndelay(50);
		REG_CLR(HMC703_SCLK_OFFSET);
		d = d << 1;
	}
	REG_CLR(HMC703_EN_OFFSET);
	ndelay(100);

 //open mode
/*
  REG_CLR(HMC704_EN_OFFSET);

	d = data;
	for (i = 0; i < 32; ++i)
	{
		REG_CLR(HMC704_SCLK_OFFSET);
		if (d & (1 << 31))
			REG_SET(HMC704_SDIO_OFFSET);
		else
			REG_CLR(HMC704_SDIO_OFFSET);
		ndelay(10);
		REG_SET(HMC704_SCLK_OFFSET);
		ndelay(10);
		d = d << 1;
	}
	ndelay(10);
	REG_SET(HMC704_EN_OFFSET);
	ndelay(20);
	REG_CLR(HMC704_EN_OFFSET);
	ndelay(100);
*/
}

static void input_hmc840(u32 data)
{
	int i = 0, d,bit = 1 << 31;

	REG_SET(HMC840_EN_OFFSET);
	REG_SET(HMC840_SCLK_OFFSET);

	d = data;
	for (i = 0; i < 32; ++i)
	{
	
		REG_CLR(HMC840_SCLK_OFFSET);
		if (d & bit)
			REG_SET(HMC840_SDIO_OFFSET);
		else
			REG_CLR(HMC840_SDIO_OFFSET);
		REG_SET(HMC840_SCLK_OFFSET);
		d = d << 1;
	}
	REG_CLR(HMC840_EN_OFFSET);
	ndelay(10);
}

static void input_hmc835(u32 data)
{
	int i = 0, d, bit = 1 << 31;

	REG_SET(HMC835_EN_OFFSET);
	REG_SET(HMC835_SCLK_OFFSET);

	d = data;
	for (i = 0; i < 32; ++i)
	{
	
		REG_CLR(HMC835_SCLK_OFFSET);
		if (d & bit)
			REG_SET(HMC835_SDIO_OFFSET);
		else
			REG_CLR(HMC835_SDIO_OFFSET);
		REG_SET(HMC835_SCLK_OFFSET);
		d = d << 1;
	}
	REG_CLR(HMC835_EN_OFFSET);
	ndelay(1000);
}

static void input_ADF4360(u32 data)
{
	int i, bit = 1 << 23;//先发高位
	printd("input adf4360 %x\n",data);
	REG_CLR(ADF4360_SDAT_OFFSET);
	REG_CLR(ADF4360_SCLK_OFFSET);
	REG_CLR(ADF4360_SLE_OFFSET);

	for (i = 0; i < 24; i++) {
		if (data & bit) {
			REG_SET(ADF4360_SDAT_OFFSET);
		} else {
			REG_CLR(ADF4360_SDAT_OFFSET);
		}

		REG_SET(ADF4360_SCLK_OFFSET);
		ndelay(10);
		REG_CLR(ADF4360_SCLK_OFFSET);
		data = data << 1;
	}

	REG_SET(ADF4360_SLE_OFFSET);
	ndelay(50);
	REG_CLR(ADF4360_SLE_OFFSET);

}


static void init_lband_att(u32 data)
{
	int i=0,d=data;
	printd("input attrf %x\n",data);
	REG_CLR(ATT_SLEN_ADDR);
	REG_CLR(ATT_SCLK_ADDR);
	//REG_CLR(RF_DATA);
	ndelay(20);

	for (i = 0; i < 16; i++) {
		if (d & 0X01) {
			REG_SET(ATT_SDAT_ADDR);
		} else {
			REG_CLR(ATT_SDAT_ADDR);
		}
		ndelay(10);
		REG_SET(ATT_SCLK_ADDR);
		ndelay(10);
		REG_CLR(ATT_SCLK_ADDR);
		d = d >> 1;
		ndelay(4);
	}
	ndelay(20);
	REG_SET(ATT_SLEN_ADDR);
	ndelay(10);
	REG_CLR(ATT_SCLK_ADDR);
	ndelay(10);
}


static void init_hmc840(void)
{

	input_hmc840(0x00000000); //0寄存器
	input_hmc840(0X02000004); //1寄存器
	input_hmc840(0x04000004); //2寄存器
	input_hmc840(0x0E00029A); //3寄存器 (3G)
	input_hmc840(0x11837DFE); //4寄存器
	input_hmc840(0x1400440A); //5寄存器
	input_hmc840(0x161F00C2); //6寄存器
	input_hmc840(0x18000000); //7寄存器
	input_hmc840(0x1E000012); //8寄存器
	input_hmc840(0x0C405E94); //9寄存器
	input_hmc840(0x12AD7FFE); //A寄存器
	input_hmc840(0x20000040); //B寄存器
	input_hmc840(0x2200FFFE); //C寄存器
	input_hmc840(0x24000000); //F寄存器
	input_hmc840(0x26000000); //14寄存器
	input_hmc840(0x0A01D01A); //15寄存器
	input_hmc840(0x0A00452A); //16寄存器
	input_hmc840(0x0A01C63A); //17寄存器
	input_hmc840(0x0A00000A); //18寄存器
	input_hmc840(0x06000070); //19寄存器
	input_hmc840(0x08000000); //1A寄存器
}

static void init_hmc835(void)
{

	input_hmc835(0x00000000); //
	input_hmc835(0x00000208); //REG 2
	input_hmc835(0x00000210); //
	input_hmc835(0x00004218); ////////4218
	input_hmc835(0xCCCCCD20); /////////
	input_hmc835(0x00000028); //
	input_hmc835(0x030F0A30); //
	input_hmc835(0x20084438); //
	input_hmc835(0x01BFFF40); //
	input_hmc835(0x54726448); //REG 6
	input_hmc835(0x00204650); //REG 9
	input_hmc835(0x07C02158); //
	input_hmc835(0x00000060); //
	input_hmc835(0x00008178); //
	input_hmc835(0x000220A0); //
	input_hmc835(0x0F48A0A8); //REG5=vco
	input_hmc835(0x000782B0); //REG5
	input_hmc835(0x00039BB8); //REG5//////
	input_hmc835(0x0054C1C0); //REG5
	input_hmc835(0x000AAAC8); //REG3
	input_hmc835(0xB29D0BD0); //REG4

}


static void input_hmc1018(u8 data)
{
	int i = 0, d;
	printd("input_hmc1018 0x%x\n",data);
	val_1018 = data;
	REG_SET(HBAND_ATT_EN_OFFSET);
	REG_CLR(HBAND_ATT_EN_OFFSET);
//	REG_SET(RF_SCLK_OFFSET);
//	ndelay(10);
//	REG_CLR(RF_SCLK_OFFSET);
//	ndelay(20);

	d = data;
	for (i = 0; i < 8; ++i)
	{
		if (d & 0x80)
			REG_SET(RF_SDIO_OFFSET);
		else
			REG_CLR(RF_SDIO_OFFSET);
		ndelay(10);
		REG_SET(HIGHBAND_ATT_CLK);
		ndelay(10);
		REG_CLR(HIGHBAND_ATT_CLK);
		d = d << 1;
		ndelay(4);
	}
	//ndelay(20);
	REG_SET(HBAND_ATT_EN_OFFSET);
	ndelay(20);
//	REG_CLR(RF_ALE2_OFFSET);
//	ndelay(20);

}
#if 0
static void init_AD5320(void)
{
	input_AD5320(0x07FF);
}

static void init_AD8402(void)
{

	input_AD8402(0x00, 0xF5);
	input_AD8402(0x01, 0xF5);

}

//AD9912初始化
static void init_AD9912(void)
{
	input_AD9912(0x0E, 0x08B50000, 0x1c71c71c);
	input_AD9912(0x02, 0x1f3fc000, 0);
}

static void init_AD9910(void)
{
	input_AD9910(0x0E, 0x08B50000, 0x22222222);
	input_AD9910(0x02, 0x1f3fc000, 0);
}
#endif

static void init_hmc703(void)
{
	REG_CLR(HMC703_EN_OFFSET);
	REG_CLR(HMC703_SCLK_OFFSET);
	mdelay(10);

	//hmc mode
	input_hmc703(0x04000004); //REG2
	input_hmc703(0x0c007a3e); //REG6
	input_hmc703(0x10076ffe); //REG8 
	input_hmc703(0x137ffffe); //REG9
	input_hmc703(0x1603c0e2); //REGB 

	input_hmc703(0x0600006E); //REG3
	input_hmc703(0x088F5C28); //REG4

	REG_SET(HMC703_EN_OFFSET);//debug

}


static void init_adf4360(void)
{
  input_ADF4360(0x300065); mdelay(2); // R rigster
  input_ADF4360(0x0ff128); mdelay(2);
  input_ADF4360(0x404002); mdelay(1); // N rigster*/
}

static int bl_fe_read(struct file *filp, char __user *buff, size_t count, loff_t *offp)
{
	return 0;
}

static ssize_t bl_fe_write(struct file *filp, const char __user *buff, size_t count, loff_t *offp)
{
	return 0;
}


//@cmd送数地址
//@arg数据值
static long bl_fe_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	printd("ioctl cmd = %x, arg = 0x%08x\r\n", cmd, arg);

	switch (cmd)
	{
		case 0xfff0:
			if(arg == 0)
			{
				REG_CLR(DDS_CHANNEL_OFFSET);
				printd("dds channel set 0\n");
			}
			else
			{
				REG_SET(DDS_CHANNEL_OFFSET);
				printd("dds channel set 1\n");
			}
			break;
		case 0xfff1:
			//printk("ioctl 0xfff1,arg is 0x%08x\n",arg);
			//init_hmc703();
			input_hmc703(arg);      //频率设置
			break;
		case 0xfff2:
			init_lband_att(arg);     //低波段衰减器
			break;
		case 0xfff3:
			input_hmc1018(arg);     //高波段衰减器
			break;
		case 0xfff4:
			setPreampt(arg);        //前放
			break;
		case 0xfff5:
			setIfAtt(arg);			//中频衰减器
			break;
		case 0xfff6:
			init_lband_att(arg);    //attrf
			break;
		case 0xfff7:
			setRefSelect(arg);		//内外参考选择
			break;
		case 0xfff8:
			init_hmc703();		//一本初始化
			break;
		case 0xfff9:
			init_hmc840();		//二本初始化
			break;
		case 0xfffc:
			init_hmc835();		//三本初始化
			break;
		case 0xffe0:
			REG_SET(DCM_RESET_OFFSET);
			REG_CLR(DCM_RESET_OFFSET);    //时钟复位
			break;
		case 0xffe1:
			//printk("ioctl 0xffe1\n");
			input_hmc703(0x0c007a3c); //REG6
			//init_hmc703();
			break;
		case 0xfffa:
			{
			  int val = ioread16(remapBuf + arg * 2);
			  printk("val = %d\n",val);
			}
			break;
		case 0xfffb:
			iowrite16(arg, remapBuf + arg * 2);wmb();
			break;
		default:
			iowrite16(arg, remapBuf + cmd * 2);wmb();
			break;
	}

	return 0;
}

static int bl_fe_open(struct inode *inode, struct file *file)
{
  return 0;
}

static int bl_fe_close(struct inode *inode, struct file *file)
{
  return 0;
}

static struct file_operations dev_fops = {
		.owner = THIS_MODULE,
		.unlocked_ioctl = bl_fe_ioctl,
		.open = bl_fe_open,
		.release = bl_fe_close,
		.read = bl_fe_read,
		.write = bl_fe_write,
	//	.poll = bl_fe_poll,
};

static struct miscdevice misc = {
		.minor = MISC_DYNAMIC_MINOR,
		.name = DEVICE_NAME,
		.fops = &dev_fops,
};

static int __init dev_init(void)
{
	int ret = misc_register(&misc);

	remapBuf = ioremap(RF_BASEADDR, RF_SIZE);
	mdelay(1);

	//version_fpga = ioread16(VERSION_FPGA);
	version_fpga = ioread16(remapBuf + VERSION_FPGA * 2);rmb();
	//printk(DEVICE_NAME " fpga version:%d\n",version_fpga);
	
	init_adf4360();
	mdelay(10);

	REG_SET(DCM_RESET_OFFSET);
	REG_CLR(DCM_RESET_OFFSET);    //时钟复位

	mdelay(10);

	//REG_CLR(DDS_CHANNEL_OFFSET);  //本振送数开关

	REG_SET(WORK_STATE_OFFSET);   //电源控制

	mdelay(10);

	init_lband_att(0x0300);			//低波段衰减器初始化为10

	REG_WRITE(262, 0);	    //中频10db增益打开
	REG_WRITE(263, 0);		//中频20db增益打开

	setPreampt(0Xe2);      //内参考,前放关闭，高波段通道，高波段1,108M~200M滤波通道

	init_hmc703();         //第一本振初始化

	init_hmc840();         //第二本振初始化

	init_hmc835();         //第三本振初始化

	printk(DEVICE_NAME " initialized successed\n");

	return ret;
}

static void __exit dev_exit(void)
{
//	if(NULL != region_resource)
	{
		iounmap(remapBuf);
	//	release_mem_region(RF_BASEADDR, RF_SIZE);
	}
	misc_deregister(&misc);
	printk(DEVICE_NAME " exit\n");
}

module_init( dev_init);
module_exit( dev_exit);

MODULE_LICENSE("GPL");
MODULE_VERSION ("v0.1");
MODULE_AUTHOR("BAILU");
