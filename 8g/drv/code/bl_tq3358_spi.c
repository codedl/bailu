#include <linux/interrupt.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/sysfs.h>
#include <linux/list.h>
#include <linux/spi/spi.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/poll.h>
#include <linux/irq.h>
#include <asm/io.h>
#include <linux/gpio.h>
#include <linux/miscdevice.h>
//#include "bl_tq3358_gpio.h"
//#include "bl_tq3358_gpio_emi.h"

#define DEVICE_NAME "BL_TQ3358_SPI"

#define GPIO_TO_PIN(bank, gpio) (32 * (bank) + (gpio))

#define OUTPUT_HIGH  1                    /* high level */
#define OUTPUT_LOW   0                    /* low level */

#define CMD_SA_MODE  0xf0
#define CMD_EMI_MODE 0xf1

#define DEBUG_BL_SPI
#ifdef DEBUG_BL_SPI
	#define printd(fmt,args...) printk("[KERNEL_DEBUG:] "fmt,##args)
#else
	#define printd(fmt,args...)
#endif

//文件类型定义
enum bitFileType {bftSA, bftEMI};

static struct 	spi_device *spi_device;
static uint8_t  wbuf[] ={0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x08, 0x80, 0x54};
static uint8_t 	rbuf[2]={0};

static struct spi_board_info spi_device_info = {
    .modalias     = "bl_tq3358_spi2",
    .max_speed_hz = 20 * 1000 * 1000,
    .bus_num      = 2,
    .chip_select  = 1,
    .mode         = SPI_MODE_0,
};

typedef struct __GPIO_DESC
{
  int gpio;
  const char *desc;
} gpio_desc;

static gpio_desc adt7310_data[] =
{
  {GPIO_TO_PIN(2, 0),  "fpga-program"},   /* program */
};


static int fpga_gpio_init(void)
{
	int i = 0;
	int ret = 0;

	//fpga gpio init
	for (i = 0; i < sizeof(adt7310_data) / sizeof(adt7310_data[0]); i++)
	{
		ret = gpio_request(adt7310_data[i].gpio, adt7310_data[i].desc);
		if(ret < 0)
		{
		  printk("failed to request GPIO %d{%s}, error %d\n", adt7310_data[i].gpio, adt7310_data[i].desc, ret);
		  return ret;
		}

		gpio_direction_output(adt7310_data[i].gpio, OUTPUT_LOW);
		gpio_set_value(adt7310_data[i].gpio, OUTPUT_LOW);
	}
	return 0;
}

static int loadBitFile(enum bitFileType value)
{
	unsigned long count = 0;
	int step = 150;       //  < 160??
	int n;
	unsigned long i=0;
	printd("load bit file\n");
	//reset fpga program pin
	mdelay(1);
	gpio_set_value(adt7310_data[0].gpio, OUTPUT_LOW);
	mdelay(1);
	gpio_set_value(adt7310_data[0].gpio, OUTPUT_HIGH);
	mdelay(1);

	switch(value)
	{
	case bftSA:
		//bit 文件加载
		count = sizeof(gpiobit_sa) / sizeof(gpiobit_sa[0]);
		for(i = 0;i < count;i = i + step)
		{
			n = step < (count - i)? step :(count - i);
			spi_write(spi_device ,&gpiobit_sa[i], n);       //每次向spi总线写入n个字节数据
		}
		break;
	case bftEMI:
		//bit 文件加载
		count = sizeof(gpiobit_emi) / sizeof(gpiobit_emi[0]);
		for(i = 0;i < count;i = i + step)
		{
			n = step < (count - i)? step :(count - i);
			spi_write(spi_device ,&gpiobit_emi[i], n);       //每次向spi总线写入n个字节数据
		}
		break;
	}
	printd("load over\n");

	return 0;
}

static int spi_dev_init(void)
{
	int ret;
    struct spi_master *master;

     
    /*To send data we have to know what spi port/pins should be used. This information 
      can be found in the device-tree. */
    master = spi_busnum_to_master( spi_device_info.bus_num );
    if( !master ){
        printk("MASTER not found.\n");
        return -ENODEV;
    }
     
    // create a new slave device, given the master and device info
    spi_device = spi_new_device( master, &spi_device_info );
 
    if( !spi_device ) {
        printk("FAILED to create slave.\n");
        return -ENODEV;
    }
     
    spi_device->bits_per_word = 8;

    ret = spi_setup( spi_device );
     
    if( ret ){
        printk("FAILED to setup slave.\n");
        spi_unregister_device( spi_device );
        return -ENODEV;
    }
    return 0;
}

static void adt7310_reg_init(void)
{
	printd("adt7310 reg init\n");
	spi_write(spi_device,&wbuf[0],sizeof(wbuf));
	msleep(500);
}

static int adt7310_read_temp(void)
{
	int ret = 0;

	{
		ret = spi_read(spi_device,rbuf,2);
		if(ret < 0){
			printk("SPI read error\n");
			return ret;
		}
		printd("rbuf %02x %02x\n",rbuf[0],rbuf[1]);
//		msleep(500);
	}

	return 0;
}

ssize_t spi_dev_read(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
	adt7310_read_temp();        //读取温度传感器ADT7310数据
	copy_to_user(buf,rbuf,2);   //数据发送给应用层

	return 0;
}


static long spi_dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
  printk("ioctl cmd %x\n",cmd);
  switch(cmd)
  {
    case CMD_SA_MODE:
	  loadBitFile(bftSA);
	  adt7310_reg_init();
	  printd("SA mode\n");
	  break;
    case CMD_EMI_MODE:
	  loadBitFile(bftEMI);
	  adt7310_reg_init();
	  printd("EMI mode\n");
	  break;
    default:
	  break;
  }

  return 0;
}

static int spi_dev_open(struct inode *inode, struct file *file)
{
  return 0;
}

static int spi_dev_close(struct inode *inode, struct file *file)
{

  return 0;
}

static struct file_operations spi_fops=
{
  .owner			=	THIS_MODULE,
  .unlocked_ioctl	=	spi_dev_ioctl,
  .open 			= 	spi_dev_open,
  .release	 	    = 	spi_dev_close,
  .read				=	spi_dev_read,
};

static struct miscdevice misc =
{
  .minor	= MISC_DYNAMIC_MINOR,
  .name		= DEVICE_NAME,
  .fops		= &spi_fops,
};


static int __init spi_init(void)
{
    int ret;

    ret = misc_register(&misc);
    if (ret != 0)
    {
    	printk(DEVICE_NAME "initialized failed!\n");
    	return -1;
    }

    spi_dev_init();         //SPI总线初始化
    fpga_gpio_init();       //GPIO初始化
    loadBitFile(bftSA);          //加载bit文件
    adt7310_reg_init();     //adt7310初始化
    printk(DEVICE_NAME " initialized successed\n");
    return 0;
}
 
static void __exit spi_exit(void)
{

	if( spi_device ){
        spi_unregister_device( spi_device );
    }

	int i = 0;

	for (i = 0 ; i < sizeof(adt7310_data) / sizeof(adt7310_data[0]); i++)
	{
		gpio_free(adt7310_data[i].gpio);
	}

    misc_deregister(&misc);

    printk(DEVICE_NAME " exit\n");
}
 
module_init(spi_init);
module_exit(spi_exit);
 
MODULE_LICENSE("GPL");
MODULE_AUTHOR("anhui egret electronic company");
MODULE_DESCRIPTION("Driver for SPI");
