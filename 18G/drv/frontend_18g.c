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

//static struct resource * region_resource;
static int val_1018;
//RF_ADRS(0)
#if 0

static int PA = 0xFF;
#define SDIO_SET(offset)   			do{ iowrite16((PA=PA|0x01), remapBuf + offset); wmb(); } while (0)
#define SDIO_CLR(offset)   			do{ iowrite16((PA=PA&0xFE), remapBuf + offset); wmb(); } while (0)

#define SCLK_SET(offset)   			do{ iowrite16((PA=PA|0x02), remapBuf + offset); wmb(); } while (0)
#define SCLK_CLR(offset)   			do{ iowrite16((PA=PA&0xFD), remapBuf + offset); wmb(); } while (0)

#define H764_SEN_SET(offset)   		do{ iowrite16((PA=PA|0x04), remapBuf + offset); wmb(); } while (0)
#define H764_SEN_CLR(offset)   		do{ iowrite16((PA=PA&0xFB), remapBuf + offset); wmb(); } while (0)

#define AD9910_CS_SET(offset)   	do{ iowrite16((PA=PA|0x08), remapBuf + offset); wmb(); } while (0)
#define AD9910_CS_CLR(offset)   	do{ iowrite16((PA=PA&0xF7), remapBuf + offset); wmb(); } while (0)

#define AD9912_CS_SET(offset)   	do{ iowrite16((PA=PA|0x10), remapBuf + offset); wmb(); } while (0)
#define AD9912_CS_CLR(offset)   	do{ iowrite16((PA=PA&0xEF), remapBuf + offset); wmb(); } while (0)

#define AD8402_CS_SET(offset)   	do{ iowrite16((PA=PA|0x20), remapBuf + offset); wmb(); } while (0)
#define AD8402_CS_CLR(offset)   	do{ iowrite16((PA=PA&0xDF), remapBuf + offset); wmb(); } while (0)

#define ALE1_SET(offset)   			do{ iowrite16((PA=PA|0x40), remapBuf + offset); wmb(); } while (0)
#define ALE1_CLR(offset)   			do{ iowrite16((PA=PA&0xBF), remapBuf + offset); wmb(); } while (0)

#define ALE2_SET(offset)   			do{ iowrite16((PA=PA|0x80), remapBuf + offset); wmb(); } while (0)
#define ALE2_CLR(offset)   			do{ iowrite16((PA=PA&0x7F), remapBuf + offset); wmb(); } while (0)

//RF_ADRS(1)
static int PB = 0xFF;
#define DDS_IOUPDATE_SET(offset)	do{ iowrite16((PB=PB|0x01), remapBuf + offset); wmb(); } while (0)
#define DDS_IOUPDATE_CLR(offset)	do{ iowrite16((PB=PB&0xFE), remapBuf + offset); wmb(); } while (0)

#define AD5320_SYN_SET(offset)   	do{ iowrite16((PB=PB|0x02), remapBuf + offset); wmb(); } while (0)
#define AD5320_SYN_CLR(offset)   	do{ iowrite16((PB=PB&0xFD), remapBuf + offset); wmb(); } while (0)

#define SPCLK_SET(offset)   		do{ iowrite16((PB=PB|0x04), remapBuf + offset); wmb(); } while (0)
#define SPCLK_CLR(offset)   		do{ iowrite16((PB=PB&0xFB), remapBuf + offset); wmb(); } while (0)

//RF_ADRS(2)
static int PC = 0xFF;
#define REF_CON_SET(offset)			do{ iowrite16((PC=PC|0x01), remapBuf + offset); wmb(); } while (0)
#define REF_CON_CLR(offset)			do{ iowrite16((PC=PC&0xFE), remapBuf + offset); wmb(); } while (0)

#define HB_BFA_SET(offset)   		do{ iowrite16((PC=PC|0x02), remapBuf + offset); wmb(); } while (0)
#define HB_BFA_CLR(offset)   		do{ iowrite16((PC=PC&0xFD), remapBuf + offset); wmb(); } while (0)

#define HB_BFB_SET(offset)   		do{ iowrite16((PC=PC|0x04), remapBuf + offset); wmb(); } while (0)
#define HB_BFB_CLR(offset)   		do{ iowrite16((PC=PC&0xFB), remapBuf + offset); wmb(); } while (0)

#define DDS_RST_SET(offset)   		do{ iowrite16((PC=PC|0x08), remapBuf + offset); wmb(); } while (0)
#define DDS_RST_CLR(offset)   		do{ iowrite16((PC=PC&0xF7), remapBuf + offset); wmb(); } while (0)
#endif
/*

 static int PB = 0xFFFF;
 #define WLOPOW_CON__CLR(offset)	do { iowrite16( (PB=PB&0xFE), remapBuf + offset ); wmb(); } while (0)
 #define WLOPOW_CON__SET(offset)	do { iowrite16( (PB=PB|0x01), remapBuf + offset ); wmb(); } while (0)

 */

#define  REG_SET(offset)   do{ iowrite16(0x01, remapBuf + offset * 2); wmb(); } while (0)
#define  REG_CLR(offset)   do{ iowrite16(0x00, remapBuf + offset * 2); wmb(); } while (0)

#define  REG_SET_R(offset)   do{ iowrite16(0x00, remapBuf + offset * 2); wmb(); } while (0)
#define  REG_CLR_R(offset)   do{ iowrite16(0x01, remapBuf + offset * 2); wmb(); } while (0)


#define LO_SDIO_OFFSET          222 //	203
#define LO_SCLK_OFFSET          222 //	204

#define DCM_RESET_OFFSET		   	0
#define ADF4360_SLE_OFFSET			1
#define ADF4360_SCLK_OFFSET			2
#define ADF4360_SDAT_OFFSET			3

#define REF_SETLECT_OFFSET 		  	12  //addr 12
#define WORK_STATE_OFFSET         	19  //addr 19

#define HBAND_ATT_EN_OFFSET       	238
#define LBAND_ATT_EN_OFFSET       	239
#define IF_ATT_EN_OFFSET          	240
#define HC595_EN_OFFSET			  	241
#define HC595_SDIO_OFFSET         	242
#define HC595_SCLK_OFFSET         	243

#define HMC704_SDIO_OFFSET     		208
#define HMC704_SCLK_OFFSET     		204//clk of first osc
#define HMC704_EN_OFFSET       		205//data enable of first osc

#define HMC764_SDIO_OFFSET     		208
#define HMC764_SCLK_OFFSET     		203//clk of second osc
#define HMC764_EN_OFFSET       		206//data enable of second osc

#define DDS_CHANNEL_OFFSET        207

#define REF_TIMER_CLK_OFFSET      200
#define REF_TIMER_SDIO_OFFSET     208
#define REF_TIMER_SYN_OFFSET      201

#define RF_SDIO_OFFSET            242
#define RF_SCLK_OFFSET						243
#define RF_ALE1_OFFSET						239
#define RF_ALE2_OFFSET						238
#define RF_ALE3_OFFSET						241
#define RF_ALE4_OFFSET            240

#define HIGHBAND_ATT_CLK				264
#define LOWBAND_ATT_CLK					265


/*
 static void input_AD9912(u32 data)
 {
 int i;
 printd("input_AD9912 0x%x\n",data);
 DDS_IOUPDATE_SET(RF_ADRS1_OFFSET);
 SCLK_SET(RF_ADRS0_OFFSET);

 AD9912_CS_SET(RF_ADRS0_OFFSET);
 AD9912_CS_CLR(RF_ADRS0_OFFSET);
 //       SDIO_SET(addr);

 ndelay(50);

 for(i=0;i<32;i++){

 if((data&0x80000000)==0x80000000) SDIO_SET(RF_ADRS0_OFFSET);
 else  SDIO_CLR(RF_ADRS0_OFFSET);

 SCLK_SET(RF_ADRS0_OFFSET);

 ndelay(10);
 SCLK_CLR(RF_ADRS0_OFFSET);

 data = data << 1;

 ndelay(10);

 }
 ndelay(5);
 AD9912_CS_SET(RF_ADRS0_OFFSET);
 SDIO_CLR(RF_ADRS0_OFFSET);
 DDS_IOUPDATE_SET(RF_ADRS1_OFFSET);
 ndelay(10);
 DDS_IOUPDATE_CLR(RF_ADRS1_OFFSET);

 }
 */

#define GPIO_TO_PIN(bank, gpio) (32 * (bank) + (gpio))

#if 0
static void input_AD9912(u8 cmd, u32 data1, u32 data2)
{
	int i;
	u32 c = 0, d;
	printd("%s 0x%x data1 0x%x data2 0x%x\n",__FUNCTION__,cmd,data1,data2);
	SCLK_CLR(RF_ADRS0_OFFSET);
	DDS_IOUPDATE_CLR(RF_ADRS1_OFFSET);
	// AD9912_CS_SET(addr);
	AD9912_CS_CLR(RF_ADRS0_OFFSET); //片选信号 低电平有效

	//指令阶段
	ndelay(20);
	c = cmd;
	for (i = 0; i < 8; i++)
	{

		if ((c & 0x80) == 0x80)
			SDIO_SET(RF_ADRS0_OFFSET);
		else
			SDIO_CLR(RF_ADRS0_OFFSET);

		ndelay(5);
		SCLK_SET(RF_ADRS0_OFFSET);

		ndelay(10);
		SCLK_CLR(RF_ADRS0_OFFSET);

		c = c << 1;

		ndelay(4);

	}

	//写入数据
	ndelay(20);
	d = data1;
	for (i = 0; i < 32; i++)
	{
		//if((d&0x80000000)==0x80000000)
		if (d & (1 << 31))
			SDIO_SET(RF_ADRS0_OFFSET);
		else
			SDIO_CLR(RF_ADRS0_OFFSET);

		ndelay(5);
		SCLK_SET(RF_ADRS0_OFFSET);

		ndelay(10);
		SCLK_CLR(RF_ADRS0_OFFSET);

		d = d << 1;

		ndelay(4);

	}

	d = data2;
	if (d != 0)
	{
		for (i = 0; i < 32; i++)
		{

			if (d & (1 << 31))
				SDIO_SET(RF_ADRS0_OFFSET);
			else
				SDIO_CLR(RF_ADRS0_OFFSET);

			ndelay(5);
			SCLK_SET(RF_ADRS0_OFFSET);

			ndelay(10);
			SCLK_CLR(RF_ADRS0_OFFSET);
			d = d << 1;
			ndelay(4);
		}

	}
	ndelay(10);
	SDIO_CLR(RF_ADRS0_OFFSET);
	AD9912_CS_SET(RF_ADRS0_OFFSET);

	DDS_IOUPDATE_SET(RF_ADRS1_OFFSET); //将IO缓冲数据传输到寄存器
	ndelay(5);
	DDS_IOUPDATE_CLR(RF_ADRS1_OFFSET);
	ndelay(10);
}

static void input_AD9910(u8 cmd, u32 data1, u32 data2)
{
	int i;
	u32 c = 0, d;
	printd("input_AD9910 0x%x data1 0x%x data2 0x%x\n",cmd,data1,data2);
	SCLK_CLR(RF_ADRS0_OFFSET);
	DDS_IOUPDATE_CLR(RF_ADRS1_OFFSET);
	// AD9910_CS_SET(addr);
	AD9910_CS_CLR(RF_ADRS0_OFFSET); //片选信号 低电平有效

	//指令阶段
	ndelay(20);
	c = cmd;
	for (i = 0; i < 8; i++)
	{

		if ((c & 0x80) == 0x80)
			SDIO_SET(RF_ADRS0_OFFSET);
		else
			SDIO_CLR(RF_ADRS0_OFFSET);

		ndelay(5);
		SCLK_SET(RF_ADRS0_OFFSET);

		ndelay(10);
		SCLK_CLR(RF_ADRS0_OFFSET);

		c = c << 1;

		ndelay(4);

	}

	//写入数据
	ndelay(20);
	d = data1;
	for (i = 0; i < 32; i++)
	{
		//if((d&0x80000000)==0x80000000)
		if (d & (1 << 31))
			SDIO_SET(RF_ADRS0_OFFSET);
		else
			SDIO_CLR(RF_ADRS0_OFFSET);

		ndelay(5);
		SCLK_SET(RF_ADRS0_OFFSET);

		ndelay(10);
		SCLK_CLR(RF_ADRS0_OFFSET);

		d = d << 1;

		ndelay(4);

	}

	d = data2;
	if (d != 0)
	{
		for (i = 0; i < 32; i++)
		{

			if (d & (1 << 31))
				SDIO_SET(RF_ADRS0_OFFSET);
			else
				SDIO_CLR(RF_ADRS0_OFFSET);

			ndelay(5);
			SCLK_SET(RF_ADRS0_OFFSET);

			ndelay(10);
			SCLK_CLR(RF_ADRS0_OFFSET);
			d = d << 1;
			ndelay(4);
		}

	}
	ndelay(10);
	SDIO_CLR(RF_ADRS0_OFFSET);
	AD9910_CS_SET(RF_ADRS0_OFFSET);

	DDS_IOUPDATE_SET(RF_ADRS1_OFFSET); //将IO缓冲数据传输到寄存器
	ndelay(5);
	DDS_IOUPDATE_CLR(RF_ADRS1_OFFSET);
	ndelay(10);
}

static void input_AD8402(u8 cmd, u8 data)
{
	int i;

	printd("input_AD8402 cmd 0x%x data 0x%x\n",cmd,data);
	SCLK_CLR(RF_ADRS0_OFFSET);

	//   AD8402_CS_SET(addr);
	AD8402_CS_CLR(RF_ADRS0_OFFSET);

	ndelay(50);

	for (i = 0; i < 2; i++)
	{

		if ((cmd & 0x02) == 0x02)
			SDIO_SET(RF_ADRS0_OFFSET);
		else
			SDIO_CLR(RF_ADRS0_OFFSET);

		SCLK_SET(RF_ADRS0_OFFSET);

		ndelay(10);
		SCLK_CLR(RF_ADRS0_OFFSET);

		cmd = cmd << 1;

		ndelay(10);

	}

	for (i = 0; i < 8; i++)
	{

		if ((data & 0x80) == 0x80)
			SDIO_SET(RF_ADRS0_OFFSET);
		else
			SDIO_CLR(RF_ADRS0_OFFSET);

		SCLK_SET(RF_ADRS0_OFFSET);

		ndelay(10);
		SCLK_CLR(RF_ADRS0_OFFSET);

		data = data << 1;

		ndelay(10);

	}

	ndelay(5);
	AD8402_CS_SET(RF_ADRS0_OFFSET);
	SDIO_CLR(RF_ADRS0_OFFSET);
	ndelay(5);

}
#endif

static void input_AD5320(u32 data)
{
	int i;
	printd("input_AD5320 0x%x\n",data);

//	REG_CLR(REF_TIMER_SYN_OFFSET);
//	REG_CLR(REF_TIMER_CLK_OFFSET);
//	ndelay(5);
//	REG_SET(REF_TIMER_CLK_OFFSET);
//	ndelay(5);
//	REG_CLR(REF_TIMER_CLK_OFFSET);
//	REG_SET(REF_TIMER_SYN_OFFSET);
//	ndelay(5);
//	REG_SET(REF_TIMER_CLK_OFFSET);
//	ndelay(5);
//	REG_CLR(REF_TIMER_CLK_OFFSET);
//	REG_CLR(REF_TIMER_SYN_OFFSET);
//	ndelay(2);
//	REG_SET(REF_TIMER_CLK_OFFSET);
//	ndelay(2);
//
//	for (i = 0; i < 16; i++)
//	{
//		if ((data & 0x8000) == 0x8000)
//			REG_SET(REF_TIMER_SDIO_OFFSET);
//		else
//			REG_CLR(REF_TIMER_SDIO_OFFSET);
//
//		REG_CLR(REF_TIMER_CLK_OFFSET);
//
//		ndelay(10);
//		REG_SET(REF_TIMER_CLK_OFFSET);
//
//		data = data << 1;
//		ndelay(10);
//	}
//
//	ndelay(5);
//	REG_SET(REF_TIMER_SYN_OFFSET);
//	ndelay(5);

	REG_SET_R(REF_TIMER_SYN_OFFSET);
	REG_SET_R(REF_TIMER_CLK_OFFSET);
	ndelay(5);
	REG_CLR_R(REF_TIMER_CLK_OFFSET);
	ndelay(5);
	REG_SET_R(REF_TIMER_CLK_OFFSET);
	REG_CLR_R(REF_TIMER_SYN_OFFSET);
	ndelay(5);
	REG_CLR_R(REF_TIMER_CLK_OFFSET);
	ndelay(5);
	REG_SET_R(REF_TIMER_CLK_OFFSET);
	REG_SET_R(REF_TIMER_SYN_OFFSET);
	ndelay(2);
	REG_CLR_R(REF_TIMER_CLK_OFFSET);
	ndelay(2);

	for (i = 0; i < 16; i++)
	{
		if ((data & 0x8000) == 0x8000)
			REG_SET(REF_TIMER_SDIO_OFFSET);
		else
			REG_CLR(REF_TIMER_SDIO_OFFSET);

		REG_SET_R(REF_TIMER_CLK_OFFSET);

		ndelay(10);
		REG_CLR_R(REF_TIMER_CLK_OFFSET);

		data = data << 1;
		ndelay(10);
	}

	ndelay(5);
	REG_CLR_R(REF_TIMER_SYN_OFFSET);
	ndelay(5);
}

static void input_74HC595(u8 data)
{
	int i;
	u8 d;
	printd("input 74HC595 0x%x\n",data);
	REG_CLR(HC595_EN_OFFSET);
	ndelay(5);
	REG_CLR(RF_SCLK_OFFSET);

	//写入数据
	ndelay(10);
	d = data;
	for (i = 0; i < 8; i++)
	{
		if (d & 0x80)
			REG_SET(RF_SDIO_OFFSET);
		else
			REG_CLR(RF_SDIO_OFFSET);

		ndelay(10);
		REG_SET(RF_SCLK_OFFSET);

		ndelay(10);
		REG_CLR(RF_SCLK_OFFSET);

		d = d << 1;
		ndelay(5);
	}
	ndelay(10);
	REG_SET(HC595_EN_OFFSET);
	ndelay(5);
	REG_CLR(HC595_EN_OFFSET);
}

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

/*
 static void wlopower_con(u8 adr, u8 cmd){

 if(cmd==1) WLOPOW_CON__SET(adr);
 else WLOPOW_CON__CLR(adr);

 }
 */
 #if 0
static void wloref_con(u8 adr, u8 cmd)
{

	if (cmd == 1)
		REF_CON_SET(adr);
	else
		REF_CON_CLR(adr);

}

static void dds_rest(u8 adr)
{

	DDS_RST_CLR(adr);
	ndelay(10);
	DDS_RST_SET(adr);
	ndelay(30);
	DDS_RST_CLR(adr);
	ndelay(10);

}

static void rf_adrs2(u8 data)
{
	printd("rf_adrs2 0x%x\n",data);
	iowrite16(data, remapBuf + RF_ADRS2_OFFSET); wmb();
}
#endif


static void input_hmc704(u32 data)
{
	int i = 0, d;
	printd("input_hmc704 0x%x\n",data);

	//hmc mode
	REG_CLR(HMC704_EN_OFFSET);
	ndelay(10);
	REG_CLR(HMC704_SCLK_OFFSET);
	ndelay(10);
	REG_SET(HMC704_EN_OFFSET);

	d = data;
	for (i = 0; i < 32; ++i)
	{
		if (d & (1 << 31))
			REG_SET(HMC704_SDIO_OFFSET);
		else
			REG_CLR(HMC704_SDIO_OFFSET);
		ndelay(10);
		REG_SET(HMC704_SCLK_OFFSET);
		ndelay(10);
		REG_CLR(HMC704_SCLK_OFFSET);
		d = d << 1;
	}
	REG_CLR(HMC704_EN_OFFSET);
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

static void input_hmc764(u32 data)
{
	int i = 0, d;
	printd("input_hmc764 0x%x\n",data);

	REG_CLR(HMC764_EN_OFFSET);
	ndelay(10);
	REG_CLR(HMC764_SCLK_OFFSET);
	ndelay(10);
	REG_SET(HMC764_EN_OFFSET);

	d = data;
	for (i = 0; i < 32; ++i)
	{
		if (d & (1 << 31))
			REG_SET(HMC764_SDIO_OFFSET);
		else
			REG_CLR(HMC764_SDIO_OFFSET);
		ndelay(10);
		REG_SET(HMC764_SCLK_OFFSET);
		ndelay(10);
		REG_CLR(HMC764_SCLK_OFFSET);
		d = d << 1;
	}
	REG_CLR(HMC764_EN_OFFSET);
	ndelay(100);
//	REG_SET(HMC764_EN_OFFSET);
}

static void input_ADF4360(u32 data)
{
	int i;
	printd("input adf4360 %x\n",data);
	REG_CLR(ADF4360_SDAT_OFFSET);
	REG_CLR(ADF4360_SCLK_OFFSET);
	REG_CLR(ADF4360_SLE_OFFSET);

	for (i = 0; i < 24; i++) {
		if (data & (1 << 23)) {
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

static void init_hmc764(void)
{
	input_hmc764(0x0090B202); //0寄存器
	input_hmc764(0x0200010C); //1寄存器
	input_hmc764(0x04000004); //2寄存器
	input_hmc764(0x06000096); //3寄存器 (7.5G)
	input_hmc764(0x08000000); //4寄存器
	input_hmc764(0x0AA11298); //5寄存器
	input_hmc764(0x0CE05F0E); //6寄存器
	input_hmc764(0x0E0009F4); //7寄存器
	input_hmc764(0x100063BE); //8寄存器
	input_hmc764(0x13200700); //9寄存器
	input_hmc764(0x14000000); //A寄存器
	input_hmc764(0x160000E4); //B寄存器
	input_hmc764(0x18000000); //C寄存器
	input_hmc764(0x1A000014); //D寄存器
}

static void input_pe43704(u16 data)
{
	int i = 0, d;

	printd("input_pe43704 0x%x\n",data);

	REG_CLR(RF_ALE1_OFFSET);
	REG_CLR(LOWBAND_ATT_CLK);
	ndelay(20);

	d = data;
	for (i = 0; i < 16; ++i)
	{
		if (d & 0x01)
			REG_SET(RF_SDIO_OFFSET);
		else
			REG_CLR(RF_SDIO_OFFSET);
		ndelay(10);
		REG_SET(LOWBAND_ATT_CLK);
		ndelay(10);
		REG_CLR(LOWBAND_ATT_CLK);
		d = d >> 1;
		ndelay(4);
	}
	ndelay(20);
	REG_SET(RF_ALE1_OFFSET);
	ndelay(10);
	REG_CLR(RF_ALE1_OFFSET);
	ndelay(10);
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

static void init_hmc704(void)
{
	REG_CLR(HMC704_EN_OFFSET);
	//REG_SET(HMC704_SCLK_OFFSET);
	REG_CLR(HMC704_SCLK_OFFSET);
	mdelay(10);

	//hmc mode
	input_hmc704(0x04000004); //REG2
	input_hmc704(0x0c007a3c); //REG6
	input_hmc704(0x10076ffe); //REG8 0x10137dfe
	input_hmc704(0x137ffffe); //REG9
	input_hmc704(0x1607c0e2); //REGB 0x160f00e2
	input_hmc704(0x06000036); //REG3
	input_hmc704(0x09b33333); //REG4

	//open mode
//	input_hmc704(0x00000210); //REG2
//	input_hmc704(0x200b4a30); //REG6
//	input_hmc704(0x09beff40); //REG8
//	input_hmc704(0x3fffff48); //REG9
//	input_hmc704(0x07807158); //REGB
//	input_hmc704(0x00001b18); //REG3
//	input_hmc704(0xd9999920); //REG4
}

static void init_lowband_att(void)
{
	input_pe43704(0x0000);
}

static void init_highband_att(void)
{
	input_hmc1018(0x1f);
}

static void init_adf4360(void)
{
//   input_ADF4360(0x300015); mdelay(2); // R rigster
//   input_ADF4360(0x4ff188); mdelay(2);// con rigster
//   input_ADF4360(0x404002); mdelay(1); // N rigster*/
  input_ADF4360(0x300065); mdelay(2); // R rigster
//  input_ADF4360(0x0ff108); mdelay(2);// con rigster
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
			input_hmc704(arg);      //频率设置
			break;
		case 0xfff2:
			input_pe43704(arg);     //低波段衰减器
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
			input_AD5320(arg);    //内参考调谐电压
			break;
		case 0xfff7:
			setRefSelect(arg);		//内外参考选择
			break;
		case 0xfff8:
			init_hmc704();		//一本初始化
			break;
		case 0xfff9:
			init_hmc764();		//二本初始化
			break;
		case 0xffe0:
			REG_SET(DCM_RESET_OFFSET);
			REG_CLR(DCM_RESET_OFFSET);    //时钟复位
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

	input_AD5320(0x07ff);  //内参考调谐电压

	setRefSelect(1);       //内参考

	mdelay(1);

	init_adf4360();

	mdelay(10);

	REG_SET(DCM_RESET_OFFSET);
	REG_CLR(DCM_RESET_OFFSET);    //时钟复位

	mdelay(10);

	REG_CLR(DDS_CHANNEL_OFFSET);  //本振送数开关

	REG_SET(WORK_STATE_OFFSET);   //电源控制

	mdelay(10);

	init_lowband_att();    //低波段衰减器

	init_highband_att();   //高波段衰减器

	setIfAtt(0x1f);		   //中频衰减器

	setPreampt(0x35);      //前放关闭

	init_hmc704();         //第一本振初始化

	init_hmc764();         //第二本振初始化

	REG_SET(DDS_CHANNEL_OFFSET);  //本振送数开关

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
