/*
 * bl_tq3358_adf4360.c
 *
 *  Created on: 2015-12-8
 *      Author: Administrator
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/poll.h>
#include <linux/irq.h>
#include <asm/io.h>
#include <linux/interrupt.h>
#include <linux/miscdevice.h>

#define DEVICE_NAME "BL_TQ3358_ADF4360"

#define IF_CTRL_BASE  0x02000080    //中频控制基地址
#define IF_CTRL_SIZE  0x7F          //中频控制基地址长度

#define IF_PROC_BASE  0x02000100    //中频DSP基地址
#define IF_PROC_SIZE  0x7F          //中频DSP基地址长度

#define DELAYTIME      10            //延时10ms
#define BUCLEN         500           //BUC size
#define BUCLENSIZE     BUCLEN + 1
#define NDELAYTIME     200

volatile u8* if_ctrl_remapbuf;
volatile u8* if_proc_remapbuf;

static unsigned long sampleid = 0;   //0 ~ 5
static unsigned long data[BUCLENSIZE];

static unsigned char bytell = 0x00;
static unsigned char bytelh = 0x00;
static unsigned char bytehl = 0x00;
static unsigned char bytehh = 0x00;
static unsigned long dataFreq[1];

static  unsigned  int backlight = 0;

static int bucOffAddr = 0;

//ADF4360 操作指令
static int PA = 0;
#define SET_DATA_BIT(offset) do { iowrite8( (PA=PA|0x01), if_ctrl_remapbuf + offset );wmb(); } while (0)
#define CLR_DATA_BIT(offset) do { iowrite8( (PA=PA&0xFE), if_ctrl_remapbuf + offset );wmb(); } while (0)
#define SET_CLK_BIT(offset)	 do { iowrite8( (PA=PA|0x02), if_ctrl_remapbuf + offset );wmb(); } while (0)
#define CLR_CLK_BIT(offset)	 do { iowrite8( (PA=PA&0xFD), if_ctrl_remapbuf + offset );wmb(); } while (0)
#define SET_CS_BIT(offset)	 do { iowrite8( (PA=PA|0x04), if_ctrl_remapbuf + offset );wmb(); } while (0)
#define CLR_CS_BIT(offset)	 do { iowrite8( (PA=PA&0xFB), if_ctrl_remapbuf + offset );wmb(); } while (0)

static void input_ADF4360(u32 data,u8 addr)
{
  int i;
  CLR_DATA_BIT(addr);
  CLR_CLK_BIT(addr);
  CLR_CS_BIT(addr);

  ndelay(20);

  for (i = 0; i < 24; i++)
  {
	if((data & 0x800000) == 0)
	  CLR_DATA_BIT(addr);
	else
	  SET_DATA_BIT(addr);

	SET_CLK_BIT(addr);
	ndelay(25);
	CLR_CLK_BIT(addr);
	ndelay(25);

	data = data << 1;
  }

  ndelay(15);
  SET_CS_BIT(addr);
  ndelay(50);
  CLR_CS_BIT(addr);
}

static int readdpramvalue(void)
{
  return 0;
}

//DSP初始化
static int bl_ram_initialize(void)
{
  //CLK RST
  iowrite8(0x80, if_proc_remapbuf + 0x1f *2); wmb();
  iowrite8(0x00, if_proc_remapbuf + 0x1f *2); wmb();
  mdelay(DELAYTIME);

  //ADF4360 con, CLK_Gen
  input_ADF4360(0x05f9a4,0x04); mdelay(1);  //con rigster
  input_ADF4360(0x21681a,0x04); mdelay(1);  //N rigster
  input_ADF4360(0x100029,0x04); mdelay(1);  //R rigster
  mdelay(100);

  return 0;
}

static int bl_ram_read(struct file *filp, char __user *buff, size_t count, loff_t *offp)
{
  return 0;
}

static int bl_ram_write(struct file *filp,const char __user *buff, size_t count, loff_t *offp)
{
  return 0;
}

static int bl_ram_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
  return 0;
}

static unsigned int bl_ram_poll( struct file *file, struct poll_table_struct *wait)
{
  return 0;
}

static struct file_operations dev_fops = {
  .owner =   THIS_MODULE,
  .read	 =   bl_ram_read,
  .write =   bl_ram_write,
  .unlocked_ioctl =   bl_ram_ioctl,
  .poll	 =   bl_ram_poll,
};

static struct miscdevice misc = {
  .minor = MISC_DYNAMIC_MINOR,
  .name = DEVICE_NAME,
  .fops = &dev_fops,
};

static int __init dev_init(void)
{
  int ret;

  ret = misc_register(&misc);
  sampleid = 3;

  if_ctrl_remapbuf = ioremap(IF_CTRL_BASE, IF_CTRL_SIZE);
  if_proc_remapbuf = ioremap(IF_PROC_BASE, IF_PROC_SIZE);

  bl_ram_initialize();

  printk(DEVICE_NAME " initialized successed\n");
  return ret;
}

static void __exit dev_exit(void)
{
  iounmap((void *)if_ctrl_remapbuf);
  iounmap((void *)if_proc_remapbuf);
  misc_deregister(&misc);
}

module_init(dev_init);
module_exit(dev_exit);

MODULE_LICENSE("GPL");
MODULE_VERSION ("V3.00");
MODULE_AUTHOR("anhui egret electronic company");
MODULE_DESCRIPTION("module driver of adf4360");
