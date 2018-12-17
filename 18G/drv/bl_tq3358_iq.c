/*
 * bl_tq3358_iq.c
 *
 *  Created on: 2015-12-14
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
#include <linux/poll.h>
#include <linux/irq.h>
#include <linux/interrupt.h>

#define DEVICE_NAME                        "BL_TQ3358_IQ"    //驱动名称

#define IF_BASEADDR                        0x02000080        //中频基地址
#define RF_BASEADDR                        0x02000000        //RF base addr
#define SIZE                               0xFF              //RF addr size

#define IQCOUNT                            40000             //IQ数据长度
#define IQOFFSET                           0                 //IQ数据偏移
static unsigned long iqData[IQCOUNT] = {0};                  //IQ数据

volatile u8 * remapBuf;                                      //fpga空间指针
volatile u8 * remapBufIF;                                    //中频空间指针

static int bl_fft_read(struct file *filp, char __user *buff, size_t count, loff_t *offp)
{
  //如果应用程序请求无效
  if (count == 0)
  {
	return -EINVAL;
  }

  unsigned int ilValue = 0;
  unsigned int ihValue = 0;
  unsigned int qlValue = 0;
  unsigned int qhValue = 0;
  unsigned int i = 0;
  unsigned int j = 0;
  unsigned int nums = count >> 3;

  if (nums >= IQCOUNT)
  {
	nums = IQCOUNT;
  }

  //循环单点取数
  for (i = IQOFFSET; i < nums + IQOFFSET; i++)
  {
    //RAM数据地址
    j = i - IQOFFSET;

	iowrite16(j + IQOFFSET, remapBuf + 0x1B * 4); wmb();   //addr count
	iowrite16(j + IQOFFSET, remapBuf + 0x1E * 4); wmb();   //clock

    //读I路数据
	ihValue = ioread16(remapBuf + 0x1A * 4); rmb();        //读24位
	ilValue = ioread16(remapBuf + 0x1D * 4); rmb();

	//读Q路数据
	qhValue = ioread16(remapBuf + 0x19 * 4); rmb();        //读24位
	qlValue = ioread16(remapBuf + 0x1C * 4); rmb();

    iqData[j * 2 + 0] = ((ihValue & 0xffff) << 8) + (ilValue & 0xff);
    iqData[j * 2 + 1] = ((qhValue & 0xffff) << 8) + (qlValue & 0xff);
  }

  copy_to_user(buff, iqData, (nums << 3));

  return 0;
}

static ssize_t bl_fft_write(struct file *filp, const char __user *buff, size_t count, loff_t *offp)
{
  return 0;
}

static int bl_fft_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
  if (cmd == 0xf0)         //开始采集
  {
    iowrite8(0x02, remapBufIF + 0x07 * 2); wmb();  //CIC Clear, D1
    iowrite8(0x00, remapBufIF + 0x07 * 2); wmb();
    iowrite8(0x00, remapBuf + 0x17 * 4); wmb();
  } else if (cmd == 0xf1)  //停止采集
  {
    iowrite8(0x00, remapBuf + 0x18 * 4); wmb();
  }

  return 0;
}

static struct file_operations dev_fops =
{
  .owner = THIS_MODULE,
  .read	 = bl_fft_read,
  .write = bl_fft_write,
  .unlocked_ioctl = bl_fft_ioctl,
};

static struct miscdevice misc =
{
  .minor = MISC_DYNAMIC_MINOR,
  .name = DEVICE_NAME,
  .fops = &dev_fops,
};

static int __init dev_init(void)
{
  int ret = misc_register(&misc);
  remapBuf = ioremap(RF_BASEADDR, SIZE);
  remapBufIF = ioremap(IF_BASEADDR, SIZE);
  printk(DEVICE_NAME " initialized successed\n");

  return ret;
}

static void __exit dev_exit(void)
{
  if (remapBuf != NULL)
  {
	iounmap(remapBuf);
  }

  if (remapBufIF != NULL)
  {
	iounmap(remapBufIF);
  }

  misc_deregister(&misc);
}

module_init(dev_init);
module_exit(dev_exit);

MODULE_LICENSE("GPL");
MODULE_VERSION ("V3.00");
MODULE_AUTHOR("anhui egret electronic company");
MODULE_DESCRIPTION("module driver of if iq data");
