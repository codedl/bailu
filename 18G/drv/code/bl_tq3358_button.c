/*
 * bl_tq3358_button.c
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
#include <linux/sched.h>
#include <linux/timer.h>

#define DEVICE_NAME "BL_TQ3358_BUTTON"

#define GPMC_BASEADDR  				 0x50000000   //GPMC base address
#define CTRL_BASE			         0x44E10000   //Control Module baseaddress
#define GPMC_CTRL_SIZE               0x2000
#define CONTROL_PADCONF_GPMC_GPIO0_5  0x095c
#define SLEWCTRL	(0x1 << 6)
#define	RXACTIVE	(0x1 << 5)
#define	PULLUP_EN	(0x1 << 4) /* Pull UP Selection */
#define PULLUDEN	(0x0 << 3) /* Pull up enabled */
#define PULLUDDIS	(0x1 << 3) /* Pull up disabled */
#define MODE(val)	val

#define GPIO_TO_PIN(bank, gpio) (32 * (bank) + (gpio))

static struct timer_list button_timer;

//定义中断所用的结构体
struct button_irq_desc
{
  int pin;        //对应的 GPIO端口
  int number;     //中断值，以传递给应用层/用户态
  char *name;     //名称
};

typedef struct __GPIO_DESC
{
  int gpio;
  const char *desc;
} gpio_desc;

static gpio_desc button_dev_data[] =
{
  {GPIO_TO_PIN(0, 5), "button"},
};

static struct button_irq_desc button_irqs[] =
{
  {GPIO_TO_PIN(0, 5), 1, "INTR-Button"},
};

static irqreturn_t irq_interrupt(int irq, void *dev_id)
{
	//printk("interrupt \n");
	mod_timer(&button_timer, jiffies + HZ / 2);

	return IRQ_RETVAL(IRQ_HANDLED);
}

static long bl_button_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
  return 0;
}

static int bl_button_open(struct inode *inode, struct file *file)
{
  return 0;
}

static int bl_button_close(struct inode *inode, struct file *file)
{
  return 0;
}

static int bl_button_read(struct file *filp, char __user *buff, size_t count, loff_t *offp)
{

  return 0;
}


static struct file_operations button_fops=
{
  .owner					=		THIS_MODULE,
  .unlocked_ioctl	=		bl_button_ioctl,
  .open 					= 	bl_button_open,
  .release	 	    = 	bl_button_close,
  .read						= 	bl_button_read,
};

static struct miscdevice misc =
{
  .minor	= MISC_DYNAMIC_MINOR,
  .name	= DEVICE_NAME,
  .fops	= &button_fops,
};

static void button_timer_function(unsigned long data)
{
	if(gpio_get_value(button_irqs[0].pin) == 0)
		return;

	printk("button down \n");
}

static void pin_mux_config(void)
{
	void __iomem *   conf_remapbuf = ioremap(CTRL_BASE,GPMC_CTRL_SIZE);

	iowrite32((MODE(7) | RXACTIVE),conf_remapbuf + CONTROL_PADCONF_GPMC_GPIO0_5);

	iounmap(conf_remapbuf);
}

static int __init button_init(void)
{
  int i;
  int ret;
  ret = misc_register(&misc);

  if (ret != 0)
  {
  	printk("button register unsuccessfully!\n");
  	return -1;
  }

  init_timer(&button_timer);
  button_timer.function = button_timer_function;
  add_timer(&button_timer);

//  pin_mux_config();

  for (i = 0; i < sizeof(button_dev_data) / sizeof(button_dev_data[0]); i++)
  {
  	ret = gpio_request(button_dev_data[i].gpio, button_dev_data[i].desc);
  	if(ret < 0)
  	{
  		printk("failed to request GPIO %d{%s}, error %d\n", button_dev_data[i].gpio, button_dev_data[i].desc, ret);
  		return ret;
  	}

  	gpio_direction_input(button_dev_data[i].gpio);
  }

  for (i = 0; i < sizeof button_irqs / sizeof button_irqs[0]; i++)
  {
  	ret = request_irq(gpio_to_irq(button_irqs[i].pin), irq_interrupt, IRQ_TYPE_EDGE_RISING, button_irqs[i].name, (void *)&button_irqs[i]);
  	if(ret)
  	{
  		printk("KERNEL:irq request error!\n");
  	}
  }

  printk(DEVICE_NAME " initialized successed\n");

  return 0;
}

static void __exit button_exit(void)
{
  misc_deregister(&misc);

  int i = 0;

  for (i = 0; i < sizeof(button_irqs) / sizeof(button_irqs[0]); i++)
  {
	  free_irq(gpio_to_irq(button_irqs[i].pin), (void *)&button_irqs[i]);
  }

  del_timer(&button_timer);

  printk(DEVICE_NAME " exit \n");
}

module_init(button_init);
module_exit(button_exit);

MODULE_LICENSE("GPL");
MODULE_VERSION ("V3.00");
MODULE_AUTHOR("anhui egret electronic company");
MODULE_DESCRIPTION("module driver of button");

