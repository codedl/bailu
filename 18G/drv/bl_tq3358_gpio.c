/*
 * bl_tq3358_gpio.c
 *
 *  Created on: 2015-12-8
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
#include "data.h"

#define GPIO_TO_PIN(bank, gpio) (32 * (bank) + (gpio))

#define DEVICE_NAME "BL_TQ3358_FPGA"

#define OUTPUT_HIGH  1
#define OUTPUT_LOW   0

typedef struct __GPIO_DESC
{
  int gpio;
  const char *desc;
} gpio_desc;

static gpio_desc fpga_dev_data[] =
{
  {GPIO_TO_PIN(1, 28), "fpga-clk"},   /* clk */
  {GPIO_TO_PIN(1, 31), "fpga-data"},  /* data */

};

static long fpga_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
  return 0;
}

static int fpga_open(struct inode *inode, struct file *file)
{
  return 0;
}

static int fpga_close(struct inode *inode, struct file *file)
{
  int i = 0;

  for (i = 0 ; i < sizeof(fpga_dev_data) / sizeof(gpio_desc); i++)
  {
    gpio_free(fpga_dev_data[i].gpio);
  }

  return 0;
}

static struct file_operations fpga_fops=
{
  .owner			=	THIS_MODULE,
  .unlocked_ioctl	=	fpga_ioctl,
  .open 			= 	fpga_open,
  .release	 	    = 	fpga_close,
};

static struct miscdevice misc =
{
  .minor	= MISC_DYNAMIC_MINOR,
  .name	= DEVICE_NAME,
  .fops	= &fpga_fops,
};

static int __init fpga_init(void)
{
  int i;
  int ret;
  ret = misc_register(&misc);

  if (ret != 0)
  {
	printk("fpga gpio unsuccessfully!\n");
	return -1;
  }

  for (i = 0; i < sizeof(fpga_dev_data) / sizeof(gpio_desc); i++)
  {
	ret = gpio_request(fpga_dev_data[i].gpio, fpga_dev_data[i].desc);
	if(ret < 0)
	{
	  printk("failed to request GPIO %d{%s}, error %d\n", fpga_dev_data[i].gpio, fpga_dev_data[i].desc, ret);
	  return ret;
	}

	gpio_direction_output(fpga_dev_data[i].gpio, 0);
	gpio_set_value(fpga_dev_data[i].gpio, 0);
  }

  int j, count;
  count = sizeof(gpiobit) / sizeof(gpiobit[0]);

//  gpio_set_value(fpga_dev_data[2].gpio, OUTPUT_LOW);
//  mdelay(1);
//  gpio_set_value(fpga_dev_data[2].gpio, OUTPUT_HIGH);
//  mdelay(1);

  for( i = 0 ; i < count ; i ++ )
  {
    for( j = 0 ; j < 8 ; j ++ )
	{
      gpio_set_value(fpga_dev_data[0].gpio, 0);
      gpio_set_value(fpga_dev_data[1].gpio, ((gpiobit[i] >> (7-j))) & 0x01);
      gpio_set_value(fpga_dev_data[0].gpio, 1);
	}
  }

  printk(DEVICE_NAME " initialized successed\n");

  return 0;
}

static void __exit fpga_exit(void)
{
  int i = 0;

  for (i = 0 ; i < sizeof(fpga_dev_data) / sizeof(gpio_desc); i++)
  {
    gpio_free(fpga_dev_data[i].gpio);
  }

  misc_deregister(&misc);
  printk(DEVICE_NAME " exit\n");
}

module_init(fpga_init);
module_exit(fpga_exit);

MODULE_LICENSE("GPL");
MODULE_VERSION ("V3.00");
MODULE_AUTHOR("anhui egret electronic company");
MODULE_DESCRIPTION("module driver of fpga");
