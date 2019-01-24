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

struct bl_tq3358_ram{
	char * name;
	unsigned int baseaddr;
	unsigned int memsize;
	volatile u8 * remapbuf;
	struct fasync_struct *fasync_queue;
	unsigned char ev_press;
	bool zerospan;
	spinlock_t lock;
};
struct bl_tq3358_ram dev;

#define DEVICE_NAME "BL_TQ3358_RAM"

#define RF_BASEADDR  0x02000000     //RF base addr
#define RF_SIZE      0xFFF        //RF addr size

#define GPIO_TO_PIN(bank, gpio) (32 * (bank) + (gpio))

static DECLARE_WAIT_QUEUE_HEAD(fpga_waitq);    //�ж���ʱ

//�����ж����õĽṹ��
struct fpga_irq_desc
{
//  int irq;        //��Ӧ���жϺ�
  int pin;        //��Ӧ�� GPIO�˿�
//  int pin_name;   //��Ӧ������������ʵ�ʲ�δ�õ�������
  int number;     //�ж�ֵ���Դ��ݸ�Ӧ�ò�/�û�̬
  char *name;     //����
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

static gpio_desc fpga_dev_data[] =
{
  {GPIO_TO_PIN(0, 19), "fpga-intr"},
};

#define __DEBUG 0
#ifdef __DEBUG
#define printd(fmt,args...) printk("[ram driver debug]: "fmt,##args)
#else
#define printd(fmt,args...)
#endif

#define RAMBUFSIZE  1601//801

static unsigned int ramData[RAMBUFSIZE];
//static unsigned int ramData;


static irqreturn_t irq_interrupt(int irq, void *dev_id)
{
	dev.ev_press = 1;
	wake_up_interruptible(&fpga_waitq);//wake up wait queue 
	 if (dev.fasync_queue && dev.zerospan == true)		 
	 {
		 kill_fasync(&dev.fasync_queue, SIGIO, POLL_IN);//when zerospan send SIGIO signal
	 }

	return IRQ_RETVAL(IRQ_HANDLED);
}

static long bl_ram_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{

  switch(cmd)
  {
  	  case 0xff:
	  	if (arg)
			dev.zerospan = true;//set span is zero
		else
			dev.zerospan = false;
		break;
	  default:
		  iowrite16(arg, dev.remapbuf + cmd * 2);wmb();
	  break;
  }
  return 0;
}

//fasync������ʵ��
static int my_fasync(int fd, struct file *file, int on)
{
  int retval;

  //�����豸�Ǽǵ�fasync_queue������ȥ
  retval = fasync_helper(fd,file,on,&dev.fasync_queue);
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
  retval = my_fasync(inode, file, 1);
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

  poll_wait(file, &fpga_waitq, wait);
  if (dev.ev_press)
  {
	dev.ev_press = 0;
	mask |= POLLIN | POLLRDNORM;
  }

  return mask;
}

static int bl_ram_read(struct file *filp, char __user *buff, size_t count, loff_t *offp)
{
	int i = 0;
	unsigned int hVal, lVal;

	if(count > sizeof(ramData))
		count = sizeof(ramData);
	spin_lock(&dev.lock);
	for(i = 0; i < RAMBUFSIZE; i++)
	{
		iowrite16(i + 1, dev.remapbuf+ 37 * 2);wmb();
		iowrite16(1, dev.remapbuf + 38 * 2);wmb();
		iowrite16(0, dev.remapbuf + 38 * 2);wmb();
		hVal = ioread16(dev.remapbuf + 35 * 2);rmb();    //��8λ
		lVal = ioread16(dev.remapbuf + 36 * 2);rmb();    //��16λ

		ramData[i] = ((hVal & 0xff) << 16) + (lVal & 0xffff);
	}
	copy_to_user(buff, &ramData[0], count);
	spin_unlock(&dev.lock);
 // printk("count %d\n",count);
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
  dev.zerospan = false;
  ret = misc_register(&misc);

  if (ret != 0)
  {
	printk("ram register unsuccessfully!\n");
	return -1;
  }
  dev.baseaddr = RF_BASEADDR;
  dev.memsize  = RF_SIZE;
  dev.remapbuf = ioremap(dev.baseaddr, dev.memsize);
  spin_lock_init(&dev.lock);
  dev.ev_press = 0;

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
  misc_deregister(&misc);

  int i = 0;

  for (i = 0; i < sizeof(fpga_irqs) / sizeof(fpga_irqs[0]); i++)
  {
	  disable_irq(gpio_to_irq(fpga_irqs[i].pin));
	  free_irq(gpio_to_irq(fpga_irqs[i].pin), (void *)&fpga_irqs[i]);
  }
  iounmap(dev.remapbuf);
  printk(DEVICE_NAME " exit \n");
}

module_init(ram_init);
module_exit(ram_exit);

MODULE_LICENSE("GPL");
MODULE_VERSION ("V3.00");
MODULE_AUTHOR("anhui egret electronic company");
MODULE_DESCRIPTION("module driver of ram");

