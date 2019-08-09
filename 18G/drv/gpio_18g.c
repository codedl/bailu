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
#include <linux/spi/spi.h>
#include <linux/module.h>
#include <linux/moduleparam.h>

#include "data.h"

#define GPIO_TO_PIN(bank, gpio) (32 * (bank) + (gpio))

#define DEVICE_NAME "BL_TQ3358_FPGA"

#define OUTPUT_HIGH  1
#define OUTPUT_LOW   0


static struct spi_board_info spi_device_info =
{
  .modalias     = "bl_tq3358_spi",
  .max_speed_hz = 5 * 1000 * 1000,  //(20 * 1000 * 1000) = 20MHz
  .bus_num      = 2,
  .chip_select  = 0,
  .mode         = SPI_MODE_0,
};
static struct spi_device *spi_device;

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
#if 0
static int adt7310_read_temp(void)
{
  int ret = 0;
  static uint8_t  rbuf[2]={0};

  {
	ret = spi_read(spi_device,rbuf,2);
	if (ret < 0)
	{
	  printk("SPI read error\n");
	  return ret;
	}
  }

  return 0;
}
#endif
ssize_t fpga_read(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
  int ret;
  static uint8_t  rbuf[2]={0};
  {
	ret = spi_read(spi_device,rbuf,2);
	if (ret < 0)
	{
	  printk("SPI read error\n");
	  return ret;
	}
  }
  ret = copy_to_user(buf,rbuf,2);   //数据发送给应用层
  if(ret < 0)
  	printk("read temp fail\n");
  return 0;
}

static struct file_operations fpga_fops=
{
  .owner			=	THIS_MODULE,
  .unlocked_ioctl	=	fpga_ioctl,
  .open 			= 	fpga_open,
  .release	 	    = 	fpga_close,
  .read				=	fpga_read,
};

static struct miscdevice misc =
{
  .minor	= MISC_DYNAMIC_MINOR,
  .name	= DEVICE_NAME,
  .fops	= &fpga_fops,
};

static int  loadbitfile(void)
{
	int i = 0, j = 0, ret = 0, count = 0;
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
	
	count = sizeof(gpiobit) / sizeof(gpiobit[0]);
	
	for( i = 0 ; i < count ; i ++ )
	{
	  for( j = 0 ; j < 8 ; j ++ )
	  {
		gpio_set_value(fpga_dev_data[0].gpio, 0);//set clk low
		gpio_set_value(fpga_dev_data[1].gpio, ((gpiobit[i] >> (7-j))) & 0x01);//send data
		gpio_set_value(fpga_dev_data[0].gpio, 1);//set clk high
	  }
	}
	return 0;

}

static int spi_dev_init(void)
{
    int ret;
    struct spi_master *master;

    /*To send data we have to know what spi port/pins should be used. This information
      can be found in the device-tree. */
    master = spi_busnum_to_master( spi_device_info.bus_num );
    if( !master )
    {
      printk("MASTER not found.\n");
      return -ENODEV;
    }

    // create a new slave device, given the master and device info
    spi_device = spi_new_device( master, &spi_device_info );

    if( !spi_device )
    {
      printk("FAILED to create slave.\n");
      return -ENODEV;
    }

    spi_device->bits_per_word = 8;

    ret = spi_setup( spi_device );//set spi mode and clock

    if (ret)
    {
      printk("FAILED to setup slave.\n");
      spi_unregister_device( spi_device );
      return -ENODEV;
    }

    return 0;
}


static void adt7310_reg_init(void)
{
  uint8_t	wbuf[] ={0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x08, 0x80, 0x54};
  spi_write(spi_device, &wbuf[0], sizeof(wbuf));
  msleep(500);
}

static int __init fpga_init(void)
{
  int ret;

  ret = misc_register(&misc);

  if (ret != 0)
  {
	printk("fpga gpio unsuccessfully!\n");
	return -1;
  }
  loadbitfile();
  spi_dev_init();
  adt7310_reg_init();

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
