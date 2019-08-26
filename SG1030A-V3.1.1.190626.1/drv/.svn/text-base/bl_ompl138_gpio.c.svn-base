/*
 * bl_ompl138_gpio.c
 *
 *  Created on: 2016-08-02
 *      Author: Administrator
 *      last modified on 2016-07-09
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

#define GPIO_TO_PIN(bank, gpio) (16 * (bank) + (gpio))

#define DEVICE_NAME "BL_OMPL138_GPIO"

#define OUTPUT_HIGH  1                    /* high level */
#define OUTPUT_LOW   0                    /* low level */

#if 0
typedef struct __GPIO_DESC
{
  int gpio;
  const char *desc;
} gpio_desc;

static gpio_desc fpga_dev_data[] =
{
  {GPIO_TO_PIN(0, 13), "fpga-clk"},       /* clk */
  {GPIO_TO_PIN(0, 12), "fpga-data"},      /* data */
  {GPIO_TO_PIN(2, 0),  "fpga-program"},   /* program */
};
#endif

#if 0
//文件类型定义
enum bitFileType {bftSA, bftEMI};

//加载BIT文件
static int loadBitFile(enum bitFileType value)
{
  //reset fpga program pin
  mdelay(1);
  gpio_set_value(fpga_dev_data[2].gpio, OUTPUT_LOW);
  mdelay(1);
  gpio_set_value(fpga_dev_data[2].gpio, OUTPUT_HIGH);
  mdelay(1);

  unsigned int i = 0;
  unsigned int j = 0;
  unsigned int count = 0;

  switch (value)
  {
    //加载频谱仪BIT文件
    case bftSA:
   	  count = sizeof(gpiobit_sa) / sizeof(gpiobit_sa[0]);
      printk("load sa bit file\n");

   	  for (i = 0; i < count; i++)
   	  {
   	    for (j = 0; j < 8; j++)
   		{
   	      gpio_set_value(fpga_dev_data[0].gpio, OUTPUT_LOW);
   	      gpio_set_value(fpga_dev_data[1].gpio, (gpiobit_sa[i] >> (7 - j)) & 0x01);
   	      gpio_set_value(fpga_dev_data[0].gpio, OUTPUT_HIGH);
   		}
   	  }

      break;

    //加载EMI接收机BIT文件
    case bftEMI:
   	  count = sizeof(gpiobit_emi) / sizeof(gpiobit_emi[0]);
   	  printk("load emi bit file\n");

   	  for (i = 0; i < count; i++)
   	  {
   	    for (j = 0; j < 8; j++)
   		{
   	      gpio_set_value(fpga_dev_data[0].gpio, OUTPUT_LOW);
   	      gpio_set_value(fpga_dev_data[1].gpio, (gpiobit_emi[i] >> (7 - j)) & 0x01);
   	      gpio_set_value(fpga_dev_data[0].gpio, OUTPUT_HIGH);
   		}
   	  }

      break;
  }

  //gpio_set_value(fpga_dev_data[2].gpio, OUTPUT_LOW);

  return 0;
}
#endif

//driver control
static long ompl138_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
  if (cmd == OUTPUT_HIGH)		
  {
    gpio_set_value(GPIO_TO_PIN(1,12), OUTPUT_HIGH);
  } 
  else if (cmd == OUTPUT_LOW)
  {
    gpio_set_value(GPIO_TO_PIN(1,12), OUTPUT_LOW);
  }

  return 0;
}

//driver open
static int ompl138_open(struct inode *inode, struct file *file)
{
  return 0;
}

//driver close
static int ompl138_close(struct inode *inode, struct file *file)
{
  int i = 0;
/*
  for (i = 0 ; i < sizeof(fpga_dev_data) / sizeof(fpga_dev_data[0]); i++)
  {
    gpio_free(fpga_dev_data[i].gpio);
  }
*/
  gpio_free(GPIO_TO_PIN(1,12));
  return 0;
}

static struct file_operations ompl138_fops=
{
  .owner			=	THIS_MODULE,
  .unlocked_ioctl	=	ompl138_ioctl,
  .open 			= 	ompl138_open,
  .release	 	    = 	ompl138_close,
};

static struct miscdevice misc =
{
  .minor	= MISC_DYNAMIC_MINOR,
  .name	= DEVICE_NAME,
  .fops	= &ompl138_fops,
};


//设置寄存器
//reg_addr: 寄存器地址
//reg_value: 寄存器对应位的值，如：(1 << 12)
static void set_register(unsigned int reg_addr, unsigned int reg_value)
{
	unsigned int tempvalue = 0;
	void __iomem * remapbuf = ioremap(reg_addr, 4);
	tempvalue = ioread32(remapbuf);rmb();
	tempvalue |= reg_value;
	iowrite32(tempvalue, remapbuf);wmb();
	iounmap(remapbuf);
}

/*
static void test(void)
{
	unsigned int dirvalue = 0;
	unsigned int outputvalue = 0;
	unsigned int muxvalue = 0;
	void __iomem * gpio1_12_dir_remapbuf = ioremap(0x01E26010,4);
	void __iomem * gpio1_12_out_remapbuf = ioremap(0x01E26014,4);
	//void __iomem * gpio1_12_mux_remapbuf = ioremap(0x01C14128,4);
	dirvalue = ioread32(gpio1_12_dir_remapbuf);rmb();
	outputvalue = ioread32(gpio1_12_out_remapbuf);rmb();
	//muxvalue = ioread32(gpio1_12_mux_remapbuf);rmb(); 
	//muxvalue |= (0x4 << 12);
	//iowrite32(muxvalue, gpio1_12_mux_remapbuf);wmb();
	printk("dirvalue = %x, outputvalue = %x,  ", dirvalue, outputvalue);
	
	set_register(0x01C14128, (0x4 << 12));
	iounmap(gpio1_12_dir_remapbuf);
	iounmap(gpio1_12_out_remapbuf);
	//iounmap(gpio1_12_mux_remapbuf);
}
*/

//driver initialize
static int __init ompl138_init(void)
{
  int i = 0;
  int ret = 0;
  
  ret = misc_register(&misc); 
  if (ret != 0)
  {
	printk("ompl138 gpio unsuccessfully!\n");
	return -1;
  }
  
  set_register(0x01C14128, (0x4 << 12));		//复选，选择功能为GPIO功能
  gpio_request(GPIO_TO_PIN(1,12), "Ref time");
  gpio_direction_output(GPIO_TO_PIN(1,12), 0);	//设置GPIO为输出，且输出低电平
/*
  for (i = 0; i < sizeof(fpga_dev_data) / sizeof(fpga_dev_data[0]); i++)
  {
	ret = gpio_request(fpga_dev_data[i].gpio, fpga_dev_data[i].desc);
	if(ret < 0)
	{
	  printk("failed to request GPIO %d{%s}, error %d\n", fpga_dev_data[i].gpio, fpga_dev_data[i].desc, ret);
	  return ret;
	}

	gpio_direction_output(fpga_dev_data[i].gpio, OUTPUT_LOW);
	gpio_set_value(fpga_dev_data[i].gpio, OUTPUT_LOW);
  }

  //default enter emi mode
  loadBitFile(bftEMI);
*/
  printk(DEVICE_NAME " initialized successed\n");

  return 0;
}

//exit driver module
static void __exit ompl138_exit(void)
{
  gpio_free(GPIO_TO_PIN(1,12));
  misc_deregister(&misc);
}

module_init(ompl138_init);
module_exit(ompl138_exit);

MODULE_LICENSE("GPL");
MODULE_VERSION ("V3.0");
MODULE_AUTHOR("anhui egret electronic company");
MODULE_DESCRIPTION("module driver of fpga");
