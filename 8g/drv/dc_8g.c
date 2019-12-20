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

#define DEVICE_NAME "BL_TQ3358_DC"

#define RF_BASEADDR  0x02000000     //RF base addr
#define RF_SIZE      0xFFFFF           //RF addr size


volatile u8 * remapBuf;

#define  REG_SET(offset)   do{ iowrite16(0x01, remapBuf + offset * 2); wmb(); } while (0)
#define  REG_CLR(offset)   do{ iowrite16(0x00, remapBuf + offset * 2); wmb(); } while (0)

#define  REG_SET_R(offset)   do{ iowrite16(0x00, remapBuf + offset * 2); wmb(); } while (0)
#define  REG_CLR_R(offset)   do{ iowrite16(0x01, remapBuf + offset * 2); wmb(); } while (0)

#define  REG_WRITE(offset,value)   do{ iowrite16(value, remapBuf + offset * 2); wmb(); } while (0)

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

#define DC_PA_ATT_SEL				217//DC放大器增益和DC衰减、通道选择、前置放大选择送数地址

#define ADDR_DC_AMP_EN				210//DC放大器增益控制使能送数地址
#define ADDR_DC_AMP_SCLK			211//DC放大器增益控制时钟送数地址
#define ADDR_DC_AMP_SDIO			212//DC放大器增益控制数据送数地址

#define ADDR_DC_CH_EN				214//DC衰减、通道选择、前置放大使能
#define ADDR_DC_CH_SCLK				213//DC衰减、通道选择、前置放大时钟
#define ADDR_DC_CH_SDIO				215//DC衰减、通道选择、前置放大数据

#define PA_CHANEL_SDAT_ADDR         259
#define PA_CHANEL_CLKLEN_ADDR       260
#define PA_CHANEL_JCLK_ADDR         261

#define HBAND_ATT_EN_OFFSET       	238
#define LBAND_ATT_EN_OFFSET       	239
#define IF_ATT_EN_OFFSET          	240
#define HC595_EN_OFFSET			  	241
#define RF_SDIO_OFFSET            	242
#define HIGHBAND_ATT_CLK			264


#define GPIO_TO_PIN(bank, gpio) (32 * (bank) + (gpio))

//dc通道放大器增益控制送数
//计算增益的公式在应用层
static void dc_amp(u32 data){
	u32 temp = data , bit = 1 << 9;//先送高位
	int i = 0;
	REG_SET(DC_PA_ATT_SEL);//选择DC放大器增益送数  
	REG_CLR(ADDR_DC_AMP_SCLK);//时钟上升沿有效
	REG_CLR(ADDR_DC_AMP_EN);//使能先高后低
	for(i=0; i<10; i++){
		REG_CLR(ADDR_DC_AMP_SCLK);
		if(temp & bit){
			REG_SET(ADDR_DC_AMP_SDIO);
		}else{
			REG_CLR(ADDR_DC_AMP_SDIO);
		}
		REG_SET(ADDR_DC_AMP_SCLK);
		temp <<= 1;
	}
	REG_SET(ADDR_DC_AMP_EN);
	REG_CLR(ADDR_DC_AMP_SDIO);
}

//DC衰减、通道选择、前置放大送数
static void dc_att_chan(u8 data){


	int i;
	u8 d;
	
	REG_CLR(DC_PA_ATT_SEL);//选择DC衰减、通道选择、前置放大数据
	REG_CLR(ADDR_DC_CH_EN);
	ndelay(50);
	REG_SET(ADDR_DC_CH_SCLK);

	//写入数据
	ndelay(10);
	d = data;
		
	for (i = 0; i < 8; i++)
	{
		if (d & 0x80)
			REG_SET(ADDR_DC_CH_SDIO);
		else
			REG_CLR(ADDR_DC_CH_SDIO);

		ndelay(10);
		REG_CLR(ADDR_DC_CH_SCLK);

		ndelay(10);
		REG_SET(ADDR_DC_CH_SCLK);

		d = d << 1;
		ndelay(10);
	}
	ndelay(10);
	REG_SET(ADDR_DC_CH_EN);
	ndelay(10);
	REG_CLR(ADDR_DC_CH_EN);
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
	//预定义控制衰减器B的送数
	int dx[] = {0x00, 0x01, 0x03, 0x05, 0x07, 0x09, 0x0c, 0x0f, 0x16, 0x1b, 0x29}; 
	printd("ioctl cmd = %x, arg = 0x%08x\r\n", cmd, arg);

	switch (cmd)
	{
		case 0xdc01://控制衰减器B送数
		{
			int att = arg;
			int data = 0;
			if(att > 30)
				att = 30;//dc板衰减器最大为30			
			data = dx[(int)att % 10];
			if(att == 20 || att == 30)
				data = 0x29;
			dc_amp(data);
			break;
		}
		case 0xdc02:////控制DC通道衰减,通道,前置放大
			dc_att_chan(arg);
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

