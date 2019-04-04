/*
 * bl_tq3358_ram.c
 *
 *  Created on: 2018-3-12
 *      Author: Administrator
 */

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/miscdevice.h>
#include <linux/kernel.h>
#include <linux/major.h>
#include <linux/mutex.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/stat.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/tty.h>
#include <linux/kmod.h>
#include <linux/gfp.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/pm.h>
#include <linux/slab.h>
#include <linux/sysctl.h>
#include <linux/proc_fs.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/poll.h>
#include <linux/irq.h>
#include <asm/irq.h>
#include <linux/interrupt.h>

#define DEVICE_NAME "BL_TQ3358_RAM"

#define RF_BASEADDR  0x02000000     //RF base addr
#define RF_SIZE      0xFFF        //RF addr size

static volatile u8 * remapBuf;

#define GPIO_TO_PIN(bank, gpio) (32 * (bank) + (gpio))


//首先是定义一个结构体，其实这个结构体存放的是一个列表，这个列表保存的是一系列设备文件，SIGIO信号就发送到这些设备上
static struct fasync_struct *fasync_queue;
//static int ev_press = 0;                       //中断触发
static bool zerospan;
static DECLARE_WAIT_QUEUE_HEAD(fpga_waitq);    //中断延时

//定义中断所用的结构体
struct fpga_irq_desc
{
//  int irq;        //对应的中断号
  int pin;        //对应的 GPIO端口
//  int pin_name;   //对应的引脚描述，实际并未用到，保留
  int number;     //中断值，以传递给应用层/用户态
  char *name;     //名称
};

static struct fpga_irq_desc fpga_irqs[] =
{
  {GPIO_TO_PIN(0, 19), 1, "INTR0"},
};

typedef struct __GPIO_DESC
{
  int gpio;
  const char *desc;
} gpio_desc;

//static gpio_desc fpga_dev_data[] =
//{
//  {GPIO_TO_PIN(0, 19), "fpga-intr"},
//};

#define __DEBUG 0
#ifdef __DEBUG
#define printd(fmt,args...) printk("[ram driver debug]: "fmt,##args)
#else
#define printd(fmt,args...)
#endif

#define RAMBUFSIZE  1601//801

static unsigned int hVal, lVal;
static unsigned int ramData[RAMBUFSIZE];
//static unsigned int ramData;


static irqreturn_t irq_interrupt(int irq, void *dev_id)
{
	 if (fasync_queue && zerospan == true)		
	 {
		 kill_fasync(&fasync_queue, SIGIO, POLL_IN); // send SIGIO
	 }

	return IRQ_RETVAL(IRQ_HANDLED);
}

static long bl_ram_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
//  printd("%s: cmd = %x, arg = %x\n",__FUNCTION__,cmd,arg);

  switch(cmd)
  {
  	  case 0xff:
	  		if (arg)
				zerospan = true;
			else
				zerospan = false;
			//printk("zerospan is %d\n",zerospan);
			break;
	  default:
		  iowrite16(arg, remapBuf + cmd * 2);wmb();
	  break;
  }
  return 0;
}

//fasync方法的实现
static int my_fasync(int fd, struct file *file, int on)
{
  int retval;

  //将该设备登记到fasync_queue队列中去
  retval = fasync_helper(fd,file,on,&fasync_queue);
//  printk("rm:%d\n",retval);

  if(retval < 0)
  {
	return retval;
  }
  return 0;
}

static int bl_ram_open(struct inode *inode, struct file *file)
{
  int retval;

  /*..processing..*/
  retval = my_fasync((int)inode, file, 1);
  if(retval < 0)
  {
	printd("KERNEL:fasync_helper apply error!\n");
  }
  return 0;
}

static int bl_ram_close(struct inode *inode, struct file *file)
{
  int retval;
  /*..processing..*/
  retval = my_fasync(-1, file, 0);

  if(retval < 0)
  {
	printk("KERNEL:fasync_helper free error!\n");
  }
  return 0;
}

static unsigned int bl_ram_poll( struct file *file, struct poll_table_struct *wait)
{
  unsigned int mask = 0;

//  poll_wait(file, &fpga_waitq, wait);
//  if (ev_press)
//  {
//	ev_press = 0;
//	mask |= POLLIN | POLLRDNORM;
//  }

  return mask;
}

static int bl_ram_read(struct file *filp, char __user *buff, size_t count, loff_t *offp)
{
	int i = 0;

	if(count > sizeof(ramData))
		count = sizeof(ramData);

	for(i = 0; i < RAMBUFSIZE; i++)
	{
		iowrite16(i + 1, remapBuf + 37 * 2);wmb();
		iowrite16(1, remapBuf + 38 * 2);wmb();
		iowrite16(0, remapBuf + 38 * 2);wmb();
		hVal = ioread16(remapBuf + 35 * 2);rmb();    //高8位
		lVal = ioread16(remapBuf + 36 * 2);rmb();    //低16位

		ramData[i] = ((hVal & 0xff) << 16) + (lVal & 0xffff);
	}
	i = copy_to_user(buff, &ramData[0], count);
	if(i < 0)
		printk("data get failed!\n");
  return 0;
}

static struct file_operations ram_fops=
{
  .owner			=	THIS_MODULE,
  .unlocked_ioctl	=	bl_ram_ioctl,
  .open 			= 	bl_ram_open,
  .release	 	    = 	bl_ram_close,
  .read				= 	bl_ram_read,
  .poll				= 	bl_ram_poll,
};

static struct miscdevice misc =
{
  .minor	= MISC_DYNAMIC_MINOR,
  .name	= DEVICE_NAME,
  .fops	= &ram_fops,
};

static int __init ram_init(void)
{
  int i;
  int ret;
  zerospan = false;
  ret = misc_register(&misc);

  if (ret != 0)
  {
	printk("ram register unsuccessfully!\n");
	return -1;
  }

  remapBuf = ioremap(RF_BASEADDR, RF_SIZE);

  for (i = 0; i < sizeof fpga_irqs / sizeof fpga_irqs[0]; i++)
  {
	ret = request_irq(gpio_to_irq(fpga_irqs[i].pin), irq_interrupt, IRQ_TYPE_EDGE_RISING, fpga_irqs[i].name, (void *)&fpga_irqs[i]);
	if(ret)
	{
		printk("KERNEL:irq request error!\n");
	}
  }


  printk(DEVICE_NAME " initialized successed\n");

  return 0;
}

static void __exit ram_exit(void)
{
  int i = 0;

  misc_deregister(&misc);

  for (i = 0; i < sizeof(fpga_irqs) / sizeof(fpga_irqs[0]); i++)
  {
	  disable_irq(gpio_to_irq(fpga_irqs[i].pin));
	  free_irq(gpio_to_irq(fpga_irqs[i].pin), (void *)&fpga_irqs[i]);
  }
  iounmap(remapBuf);
  printk(DEVICE_NAME " exit \n");
}

module_init(ram_init);
module_exit(ram_exit);

MODULE_LICENSE("GPL");
MODULE_VERSION ("V3.00");
MODULE_AUTHOR("anhui egret electronic company");
MODULE_DESCRIPTION("module driver of ram");

