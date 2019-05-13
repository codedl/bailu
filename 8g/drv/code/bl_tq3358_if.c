#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/ioport.h>
#include <linux/miscdevice.h>
#include <mach/hardware.h>
#include <asm/io.h>

#define __DEBUG 1
#if __DEBUG
#define printd(fmt,args...) printk("[driver debug]: "fmt,##args)
#else
#define printd(fmt,args...)
#endif

#define DEVICE_NAME "BL_TQ3358_IF"

#define RF_BASEADDR  0x02000000     //RF base addr
#define RF_SIZE      0xFFFFF           //RF addr size

#define RF_ADRS0_OFFSET  0
#define RF_ADRS1_OFFSET  4
#define RF_ADRS2_OFFSET  8

volatile u8 * remapbuf;
volatile u8 * addr2;
//static struct resource * region_resource;
static unsigned char val_74hc595;
//RF_ADRS(0)
static int PA = 0xFF;
#define SDIO_SET(offset)   			do{ iowrite16((PA=PA|0x01), remapbuf + offset); wmb(); } while (0)
#define SDIO_CLR(offset)   			do{ iowrite16((PA=PA&0xFE), remapbuf + offset); wmb(); } while (0)

#define SCLK_SET(offset)   			do{ iowrite16((PA=PA|0x02), remapbuf + offset); wmb(); } while (0)
#define SCLK_CLR(offset)   			do{ iowrite16((PA=PA&0xFD), remapbuf + offset); wmb(); } while (0)

#define H764_SEN_SET(offset)   		do{ iowrite16((PA=PA|0x04), remapbuf + offset); wmb(); } while (0)
#define H764_SEN_CLR(offset)   		do{ iowrite16((PA=PA&0xFB), remapbuf + offset); wmb(); } while (0)	

#define AD9910_CS_SET(offset)   	do{ iowrite16((PA=PA|0x08), remapbuf + offset); wmb(); } while (0)
#define AD9910_CS_CLR(offset)   	do{ iowrite16((PA=PA&0xF7), remapbuf + offset); wmb(); } while (0)	

#define AD9912_CS_SET(offset)   	do{ iowrite16((PA=PA|0x10), remapbuf + offset); wmb(); } while (0)
#define AD9912_CS_CLR(offset)   	do{ iowrite16((PA=PA&0xEF), remapbuf + offset); wmb(); } while (0)

#define AD8402_CS_SET(offset)   	do{ iowrite16((PA=PA|0x20), remapbuf + offset); wmb(); } while (0)
#define AD8402_CS_CLR(offset)   	do{ iowrite16((PA=PA&0xDF), remapbuf + offset); wmb(); } while (0)	

#define ALE1_SET(offset)   			do{ iowrite16((PA=PA|0x40), remapbuf + offset); wmb(); } while (0)
#define ALE1_CLR(offset)   			do{ iowrite16((PA=PA&0xBF), remapbuf + offset); wmb(); } while (0)

#define ALE2_SET(offset)   			do{ iowrite16((PA=PA|0x80), remapbuf + offset); wmb(); } while (0)
#define ALE2_CLR(offset)   			do{ iowrite16((PA=PA&0x7F), remapbuf + offset); wmb(); } while (0)	

//RF_ADRS(1)
static int PB = 0xFF;
#define DDS_IOUPDATE_SET(offset)	do{ iowrite16((PB=PB|0x01), remapbuf + offset); wmb(); } while (0)
#define DDS_IOUPDATE_CLR(offset)	do{ iowrite16((PB=PB&0xFE), remapbuf + offset); wmb(); } while (0)

#define AD5320_SYN_SET(offset)   	do{ iowrite16((PB=PB|0x02), remapbuf + offset); wmb(); } while (0)
#define AD5320_SYN_CLR(offset)   	do{ iowrite16((PB=PB&0xFD), remapbuf + offset); wmb(); } while (0)

#define SPCLK_SET(offset)   		do{ iowrite16((PB=PB|0x04), remapbuf + offset); wmb(); } while (0)
#define SPCLK_CLR(offset)   		do{ iowrite16((PB=PB&0xFB), remapbuf + offset); wmb(); } while (0)

//RF_ADRS(2)
static int PC = 0xFF;
#define REF_CON_SET(offset)			do{ iowrite16((PC=PC|0x01), remapbuf + offset); wmb(); } while (0)
#define REF_CON_CLR(offset)			do{ iowrite16((PC=PC&0xFE), remapbuf + offset); wmb(); } while (0)

#define HB_BFA_SET(offset)   		do{ iowrite16((PC=PC|0x02), remapbuf + offset); wmb(); } while (0)
#define HB_BFA_CLR(offset)   		do{ iowrite16((PC=PC&0xFD), remapbuf + offset); wmb(); } while (0)

#define HB_BFB_SET(offset)   		do{ iowrite16((PC=PC|0x04), remapbuf + offset); wmb(); } while (0)
#define HB_BFB_CLR(offset)   		do{ iowrite16((PC=PC&0xFB), remapbuf + offset); wmb(); } while (0)

#define DDS_RST_SET(offset)   		do{ iowrite16((PC=PC|0x08), remapbuf + offset); wmb(); } while (0)
#define DDS_RST_CLR(offset)   		do{ iowrite16((PC=PC&0xF7), remapbuf + offset); wmb(); } while (0)

/*

 static int PB = 0xFFFF;
 #define WLOPOW_CON__CLR(offset)	do { iowrite16( (PB=PB&0xFE), remapbuf + offset ); wmb(); } while (0)
 #define WLOPOW_CON__SET(offset)	do { iowrite16( (PB=PB|0x01), remapbuf + offset ); wmb(); } while (0)

 */

#define  REG_SET(offset)   do{ iowrite16(0x01, remapbuf + offset * 2); wmb(); } while (0)
#define  REG_CLR(offset)   do{ iowrite16(0x00, remapbuf + offset * 2); wmb(); } while (0)

#define LO_SDIO_OFFSET          	203
#define LO_SCLK_OFFSET          	204

#define DCM_RESET_OFFSET		   	0
#define REF_SETLECT_OFFSET 		  	12  //addr 12
#define WORK_STATE_OFFSET         	19  //addr 19

#define HBAND_ATT_EN_OFFSET       	238
#define LBAND_ATT_EN_OFFSET       	239
#define IF_ATT_EN_OFFSET          	240
#define HC595_EN_OFFSET			  	241
#define HC595_SDIO_OFFSET         	242
#define HC595_SCLK_OFFSET         	243

#define HMC704_SDIO_OFFSET     		LO_SDIO_OFFSET
#define HMC704_SCLK_OFFSET     		LO_SCLK_OFFSET
#define HMC704_EN_OFFSET       		205

#define HMC764_SDIO_OFFSET     		LO_SDIO_OFFSET
#define HMC764_SCLK_OFFSET     		LO_SCLK_OFFSET
#define HMC764_EN_OFFSET       		206

#define DDS_CHANNEL_OFFSET        207

#define REF_TIMER_CLK_OFFSET      200
#define REF_TIMER_SDIO_OFFSET     202
#define REF_TIMER_SYN_OFFSET      201

#define RF_SDIO_OFFSET            242
#define RF_SCLK_OFFSET						243
#define RF_ALE1_OFFSET						LBAND_ATT_EN_OFFSET
#define RF_ALE2_OFFSET						HBAND_ATT_EN_OFFSET
#define RF_ALE3_OFFSET						HC595_EN_OFFSET
#define RF_ALE4_OFFSET            IF_ATT_EN_OFFSET

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
static void input_AD9912(u8 cmd, u32 data1, u32 data2)
{
	int i;
	u8 reg = 0;
	u32 c = 0, d;
	printd("%s 0x%x data1 0x%x data2 0x%x\n",__FUNCTION__,cmd,data1,data2);
	SCLK_CLR(RF_ADRS0_OFFSET);
	DDS_IOUPDATE_CLR(RF_ADRS1_OFFSET);
	// AD9912_CS_SET(addr);
	AD9912_CS_CLR(RF_ADRS0_OFFSET); //Æ¬Ñ¡ÐÅºÅ µÍµçÆ½ÓÐÐ§

	//Ö¸Áî½×¶Î
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

	//Ð´ÈëÊý¾Ý
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

	DDS_IOUPDATE_SET(RF_ADRS1_OFFSET); //½«IO»º³åÊý¾Ý´«Êäµ½¼Ä´æÆ÷
	ndelay(5);
	DDS_IOUPDATE_CLR(RF_ADRS1_OFFSET);
	ndelay(10);
}

/*
 static void init_AD9912(u8 adr){

 input_AD9912(0x21AB1C71, adr); udelay(10);   //FTW

 input_AD9912(0x21A9C71C, adr); udelay(10);

 input_AD9912(0x21A771C7, adr); udelay(10);

 //   input_AD9912(0x21ADFFFF, adr); udelay(10);   //POW

 }
 */

static void input_AD9910(u8 cmd, u32 data1, u32 data2)
{
	int i;
	u8 reg = 0;
	u32 c = 0, d;
	printd("input_AD9910 0x%x data1 0x%x data2 0x%x\n",cmd,data1,data2);
	SCLK_CLR(RF_ADRS0_OFFSET);
	DDS_IOUPDATE_CLR(RF_ADRS1_OFFSET);
	// AD9910_CS_SET(addr);
	AD9910_CS_CLR(RF_ADRS0_OFFSET); //Æ¬Ñ¡ÐÅºÅ µÍµçÆ½ÓÐÐ§

	//Ö¸Áî½×¶Î
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

	//Ð´ÈëÊý¾Ý
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

	DDS_IOUPDATE_SET(RF_ADRS1_OFFSET); //½«IO»º³åÊý¾Ý´«Êäµ½¼Ä´æÆ÷
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

static void input_AD5320(u32 data)
{
	int i;
	printd("input_AD5320 0x%x\n",data);

	REG_CLR(REF_TIMER_SYN_OFFSET);
	REG_CLR(REF_TIMER_CLK_OFFSET);
	ndelay(5);
	REG_SET(REF_TIMER_CLK_OFFSET);
	ndelay(5);
	REG_CLR(REF_TIMER_CLK_OFFSET);
	REG_SET(REF_TIMER_SYN_OFFSET);
	ndelay(5);
	REG_SET(REF_TIMER_CLK_OFFSET);
	ndelay(5);
	REG_CLR(REF_TIMER_CLK_OFFSET);
	REG_CLR(REF_TIMER_SYN_OFFSET);
	ndelay(2);
	REG_SET(REF_TIMER_CLK_OFFSET);
	ndelay(2);

	for (i = 0; i < 16; i++)
	{
		if ((data & 0x8000) == 0x8000)
			REG_SET(REF_TIMER_SDIO_OFFSET);
		else
			REG_CLR(REF_TIMER_SDIO_OFFSET);

		REG_CLR(REF_TIMER_CLK_OFFSET);

		ndelay(10);
		REG_SET(REF_TIMER_CLK_OFFSET);

		data = data << 1;
		ndelay(10);
	}

	ndelay(5);
	REG_SET(REF_TIMER_SYN_OFFSET);
	ndelay(5);
}

static void input_74HC595(u8 data)
{
	int i;
	u8 d;
	printd("input 74HC595 0x%x\n",data);
	REG_CLR(RF_SCLK_OFFSET);

	//Ð´ÈëÊý¾Ý
	ndelay(10);
	d = data & 0x7f;
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
}

static void setPreampt(u8 value)
{
	printd("set preampt %d\n",value);
	REG_CLR(RF_ALE3_OFFSET);
	ndelay(5);
	input_74HC595(value);
	ndelay(5);
	REG_SET(RF_ALE3_OFFSET);
	ndelay(5);
	REG_CLR(RF_ALE3_OFFSET);
}

//ÖÐÆµË¥¼õÆ÷ÉèÖÃ
static void setIfAtt(u8 value)
{
	printd("set if att %d\n", value);
	REG_CLR(RF_ALE4_OFFSET);
	ndelay(5);
	input_74HC595(value);
	REG_SET(RF_ALE4_OFFSET);
	ndelay(5);
	REG_CLR(RF_ALE4_OFFSET);
}

//ÄÚÍâ²Î¿¼Ñ¡Ôñ
static void setRefSelect(u8 value)
{
	printd("ref select %d\n",value);

	if(value == 0){
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
	iowrite16(data, remapbuf + RF_ADRS2_OFFSET); wmb();
}

static void input_hmc704(u32 data)
{
	int i = 0, d;
	printd("input_hmc704 0x%x\n",data);

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
//	REG_SET(HMC704_EN_OFFSET);
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

static void init_hmc764(void)
{
	input_hmc764(0x0090B202); //0¼Ä´æÆ÷
	input_hmc764(0x0200010C); //1¼Ä´æÆ÷
	input_hmc764(0x04000004); //2¼Ä´æÆ÷
	input_hmc764(0x06000096); //3¼Ä´æÆ÷ (7.5G)
	input_hmc764(0x08000000); //4¼Ä´æÆ÷
	input_hmc764(0x0AA11298); //5¼Ä´æÆ÷
	input_hmc764(0x0CE05F0E); //6¼Ä´æÆ÷
	input_hmc764(0x0E0009F4); //7¼Ä´æÆ÷
	input_hmc764(0x100063BE); //8¼Ä´æÆ÷
	input_hmc764(0x13200700); //9¼Ä´æÆ÷
	input_hmc764(0x14000000); //A¼Ä´æÆ÷
	input_hmc764(0x160000E4); //B¼Ä´æÆ÷
	input_hmc764(0x18000000); //C¼Ä´æÆ÷
	input_hmc764(0x1A000014); //D¼Ä´æÆ÷
}

static void input_pe43704(u16 data)
{
	int i = 0, d;
	printd("input_pe43704 0x%x\n",data);

	REG_CLR(RF_ALE1_OFFSET);
	REG_CLR(RF_SCLK_OFFSET);
	ndelay(20);

	d = data;
	for (i = 0; i < 16; ++i)
	{
		if (d & 0x01)
			REG_SET(RF_SDIO_OFFSET);
		else
			REG_CLR(RF_SDIO_OFFSET);
		ndelay(10);
		REG_SET(RF_SCLK_OFFSET);
		ndelay(10);
		REG_CLR(RF_SCLK_OFFSET);
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

	REG_CLR(RF_ALE2_OFFSET);
	REG_SET(RF_SCLK_OFFSET);
	ndelay(10);
	REG_CLR(RF_SCLK_OFFSET);
//	ndelay(20);

	d = data;
	for (i = 0; i < 8; ++i)
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
		ndelay(4);
	}
	//ndelay(20);
	REG_SET(RF_ALE2_OFFSET);
	ndelay(10);
	REG_CLR(RF_ALE2_OFFSET);
	ndelay(20);

}

static void init_74HC595(void)
{
	input_74HC595(0x35);
}

static void init_AD5320(void)
{
	input_AD5320(0x07FF);
}

static void init_AD8402(void)
{

	input_AD8402(0x00, 0xF5);
	input_AD8402(0x01, 0xF5);

}

//AD9912³õÊ¼»¯
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

static void init_hmc704(void)
{
	input_hmc704(0x04000004); //REG2
	input_hmc704(0x0c401694); //REG6
	input_hmc704(0x10137dfe); //REG8
	input_hmc704(0x125b1ffe); //REG9
	input_hmc704(0x160f00e2); //REGB
	input_hmc704(0x06000036); //REG3
	input_hmc704(0x09b33333); //REG4
}

static void init_lowband_att()
{
	input_pe43704(0x0000);
}

static void init_highband_att()
{
	input_hmc1018(0x1f);
}

static int bl_if_read(struct file *filp, char __user *buff, size_t count, loff_t *offp)
{
	return 0;
}

static ssize_t bl_if_write(struct file *filp, const char __user *buff, size_t count, loff_t *offp)
{
	return 0;
}



static long bl_if_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	static int val1, val2, temp;
	printd("%s  cmd = 0x%x, arg = 0x%08x\r\n",__FUNCTION__, cmd, arg);

	switch (cmd)
	{
		default:
			iowrite16(arg, remapbuf + cmd * 2);wmb();
			break;
	}

	return 0;
}

static struct file_operations dev_fops = {
		.owner = THIS_MODULE,
		.unlocked_ioctl = bl_if_ioctl,
	//	.open = fpga_open,
	//	.release = fpga_close,
		.read = bl_if_read,
		.write = bl_if_write,
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

	printk("\n" DEVICE_NAME " initialized successed\n");

	return ret;
}

static void __exit dev_exit(void)
{
//	if(NULL != region_resource)
	{
		iounmap(remapbuf);
	//	release_mem_region(RF_BASEADDR, RF_SIZE);
	}
	misc_deregister(&misc);
}

module_init( dev_init);
module_exit( dev_exit);

MODULE_LICENSE("GPL");
MODULE_VERSION ("v0.1");
MODULE_AUTHOR("BAILU");
