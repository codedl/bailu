/*
 * bl_tq_3358_gpmc.c
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

#define DEVICE_NAME 	"BL_TQ3358_GPMC"

#define GPMC_BASEADDR  				 0x50000000   //GPMC base address
#define CTRL_BASE			         0x44E10000   //Control Module baseaddress
#define GPMC_CTRL_SIZE               0x2000

#define DEVICE_ADDRESS_1             0x02000ff0
#define DEVICE_ADDRESS_2             0x02000000

#define DEVICE_ADDRESS_3             0x02001004
#define DEVICE_ADDRESS_4             0x02000014
#define  GPMC_CS  1

/*******************GPMC Config Register***********************/
#define GPMC_CONFIG1_1_OFFSET        0x90
#define GPMC_CONFIG2_1_OFFSET        0x94
#define GPMC_CONFIG3_1_OFFSET        0x98
#define GPMC_CONFIG4_1_OFFSET        0x9c
#define GPMC_CONFIG5_1_OFFSET        0xA0
#define GPMC_CONFIG6_1_OFFSET        0xA4
#define GPMC_CONFIG7_1_OFFSET        0xA8
#define GPMC_DATA_1_OFFSET           0xB4
#define GPMC_ADDRESS_1_OFFSET        0xB0

#define GPMC_DATA_1             	(GPMC_BASEADDR + GPMC_DATA_1_OFFSET)
#define GPMC_ADDRESS_1          	(GPMC_BASEADDR + GPMC_ADDRESS_1_OFFSET)
#define GPMC_CONFIG1_1_REG  		(GPMC_BASEADDR + GPMC_CONFIG1_1_OFFSET)
#define GPMC_CONFIG2_1_REG      	(GPMC_BASEADDR + GPMC_CONFIG2_1_OFFSET)
#define GPMC_CONFIG3_1_REG     	 	(GPMC_BASEADDR + GPMC_CONFIG3_1_OFFSET)
#define GPMC_CONFIG4_1_REG      	(GPMC_BASEADDR + GPMC_CONFIG4_1_OFFSET)
#define GPMC_CONFIG5_1_REG      	(GPMC_BASEADDR + GPMC_CONFIG5_1_OFFSET)
#define GPMC_CONFIG6_1_REG       	(GPMC_BASEADDR + GPMC_CONFIG6_1_OFFSET)
#define GPMC_CONFIG7_1_REG  		(GPMC_BASEADDR + GPMC_CONFIG7_1_OFFSET)
#define GPMC_IRQ_STATUS_REG      	(GPMC_BASEADDR + 0x18)
#define GPMC_IRQ_ENABLE_REG      	(GPMC_BASEADDR + 0x1c)
#define GPMC_SYSCONFIG_REG  		(GPMC_BASEADDR + 0x10)

//#define GPMC_CONFIG2_1 	0x00141401
//#define GPMC_CONFIG2_1 	0x00080802
//#define GPMC_CONFIG3_1 	0x00080800
//#define GPMC_CONFIG4_1 	0x0a000a00

#define GPMC_CONFIG2_1 	0x000a0a00
#define GPMC_CONFIG3_1 	0x000a0a00
#define GPMC_CONFIG4_1 	0x08020802

#define GPMC_CONFIG5_1 	0x01080a0a
#define GPMC_CONFIG6_1  0x1f070880

#define GPMC_SIZE_256M		0x0
#define GPMC_SIZE_128M		0x8
#define GPMC_SIZE_64M		0xC
#define GPMC_SIZE_32M		0xE
#define GPMC_SIZE_16M		0xF

#define GPMC_CONFIG7_CSVALID		    (1 << 6)
#define GPMC_CONFIG1_DEVICESIZE(val)    ((val & 3) << 12)
#define GPMC_CONFIG1_DEVICESIZE_16       GPMC_CONFIG1_DEVICESIZE(1)

/*********************************************************************/
#define SLEWCTRL	(0x1 << 6)
#define	RXACTIVE	(0x1 << 5)
#define	PULLUP_EN	(0x1 << 4) /* Pull UP Selection */
#define PULLUDEN	(0x0 << 3) /* Pull up enabled */
#define PULLUDDIS	(0x1 << 3) /* Pull up disabled */
#define MODE(val)	val

/*****************pad control register ***********/
#define CONTROL_PADCONF_GPMC_AD0                  0x0800
#define CONTROL_PADCONF_GPMC_AD1                  0x0804
#define CONTROL_PADCONF_GPMC_AD2                  0x0808
#define CONTROL_PADCONF_GPMC_AD3                  0x080C
#define CONTROL_PADCONF_GPMC_AD4                  0x0810
#define CONTROL_PADCONF_GPMC_AD5                  0x0814
#define CONTROL_PADCONF_GPMC_AD6                  0x0818
#define CONTROL_PADCONF_GPMC_AD7                  0x081C
#define CONTROL_PADCONF_GPMC_AD8                  0x0820
#define CONTROL_PADCONF_GPMC_AD9                  0x0824
#define CONTROL_PADCONF_GPMC_AD10                 0x0828
#define CONTROL_PADCONF_GPMC_AD11                 0x082C
#define CONTROL_PADCONF_GPMC_AD12                 0x0830
#define CONTROL_PADCONF_GPMC_AD13                 0x0834
#define CONTROL_PADCONF_GPMC_AD14                 0x0838
#define CONTROL_PADCONF_GPMC_AD15                 0x083C
#define CONTROL_PADCONF_GPMC_A0                   0x0840
#define CONTROL_PADCONF_GPMC_A1                   0x0844
#define CONTROL_PADCONF_GPMC_A2                   0x0848
#define CONTROL_PADCONF_GPMC_A3                   0x084C
#define CONTROL_PADCONF_GPMC_A4                   0x0850
#define CONTROL_PADCONF_GPMC_A5                   0x0854
#define CONTROL_PADCONF_GPMC_A6                   0x0858
#define CONTROL_PADCONF_GPMC_A7                   0x085C
#define CONTROL_PADCONF_GPMC_A8                   0x0860
#define CONTROL_PADCONF_GPMC_A9                   0x0864
#define CONTROL_PADCONF_GPMC_A10                  0x0868
#define CONTROL_PADCONF_GPMC_A11                  0x086C
#define CONTROL_PADCONF_GPMC_WAIT0                0x0870
#define CONTROL_PADCONF_GPMC_WPN                  0x0874
#define CONTROL_PADCONF_GPMC_BEN1                 0x0878
#define CONTROL_PADCONF_GPMC_CSN0                 0x087C
#define CONTROL_PADCONF_GPMC_CSN1                 0x0880
#define CONTROL_PADCONF_GPMC_CSN2                 0x0884
#define CONTROL_PADCONF_GPMC_CSN3                 0x0888
#define CONTROL_PADCONF_GPMC_CLK                  0x088C
#define CONTROL_PADCONF_GPMC_ADVN_ALE             0x0890
#define CONTROL_PADCONF_GPMC_OEN_REN              0x0894
#define CONTROL_PADCONF_GPMC_WEN                  0x0898
#define CONTROL_PADCONF_GPMC_BEN0_CLE             0x089C

/****************************************************/

static void pinmux_config(void)
{
	void __iomem *   conf_gpmc_remapbuf = ioremap(CTRL_BASE,GPMC_CTRL_SIZE);

	iowrite32((MODE(0) | PULLUDEN),conf_gpmc_remapbuf + CONTROL_PADCONF_GPMC_CSN1);

	iowrite32((MODE(0) | PULLUP_EN),conf_gpmc_remapbuf + CONTROL_PADCONF_GPMC_A0);
	iowrite32((MODE(0) | PULLUP_EN),conf_gpmc_remapbuf + CONTROL_PADCONF_GPMC_A1);
	iowrite32((MODE(0) | PULLUP_EN),conf_gpmc_remapbuf + CONTROL_PADCONF_GPMC_A2);
	iowrite32((MODE(0) | PULLUP_EN),conf_gpmc_remapbuf + CONTROL_PADCONF_GPMC_A3);
	iowrite32((MODE(0) | PULLUP_EN),conf_gpmc_remapbuf + CONTROL_PADCONF_GPMC_A4);
	iowrite32((MODE(0) | PULLUP_EN),conf_gpmc_remapbuf + CONTROL_PADCONF_GPMC_A5);
	iowrite32((MODE(0) | PULLUP_EN),conf_gpmc_remapbuf + CONTROL_PADCONF_GPMC_A6);
	iowrite32((MODE(0) | PULLUP_EN),conf_gpmc_remapbuf + CONTROL_PADCONF_GPMC_A7);
	iowrite32((MODE(0) | PULLUP_EN),conf_gpmc_remapbuf + CONTROL_PADCONF_GPMC_A8);
	iowrite32((MODE(0) | PULLUP_EN),conf_gpmc_remapbuf + CONTROL_PADCONF_GPMC_A9);
	iowrite32((MODE(0) | PULLUP_EN),conf_gpmc_remapbuf + CONTROL_PADCONF_GPMC_A10);
	iowrite32((MODE(0) | PULLUP_EN),conf_gpmc_remapbuf + CONTROL_PADCONF_GPMC_A11);

	iowrite32((MODE(0) | RXACTIVE),conf_gpmc_remapbuf + CONTROL_PADCONF_GPMC_AD0);
	iowrite32((MODE(0) | RXACTIVE),conf_gpmc_remapbuf + CONTROL_PADCONF_GPMC_AD1);
	iowrite32((MODE(0) | RXACTIVE),conf_gpmc_remapbuf + CONTROL_PADCONF_GPMC_AD2);
	iowrite32((MODE(0) | RXACTIVE),conf_gpmc_remapbuf + CONTROL_PADCONF_GPMC_AD3);
	iowrite32((MODE(0) | RXACTIVE),conf_gpmc_remapbuf + CONTROL_PADCONF_GPMC_AD4);
	iowrite32((MODE(0) | RXACTIVE),conf_gpmc_remapbuf + CONTROL_PADCONF_GPMC_AD5);
	iowrite32((MODE(0) | RXACTIVE),conf_gpmc_remapbuf + CONTROL_PADCONF_GPMC_AD6);
	iowrite32((MODE(0) | RXACTIVE),conf_gpmc_remapbuf + CONTROL_PADCONF_GPMC_AD7);
	iowrite32((MODE(0) | RXACTIVE),conf_gpmc_remapbuf + CONTROL_PADCONF_GPMC_AD8);
	iowrite32((MODE(0) | RXACTIVE),conf_gpmc_remapbuf + CONTROL_PADCONF_GPMC_AD9);
	iowrite32((MODE(0) | RXACTIVE),conf_gpmc_remapbuf + CONTROL_PADCONF_GPMC_AD10);
	iowrite32((MODE(0) | RXACTIVE),conf_gpmc_remapbuf + CONTROL_PADCONF_GPMC_AD11);
	iowrite32((MODE(0) | RXACTIVE),conf_gpmc_remapbuf + CONTROL_PADCONF_GPMC_AD12);
	iowrite32((MODE(0) | RXACTIVE),conf_gpmc_remapbuf + CONTROL_PADCONF_GPMC_AD13);
	iowrite32((MODE(0) | RXACTIVE),conf_gpmc_remapbuf + CONTROL_PADCONF_GPMC_AD14);
	iowrite32((MODE(0) | RXACTIVE),conf_gpmc_remapbuf + CONTROL_PADCONF_GPMC_AD15);

	iounmap(conf_gpmc_remapbuf);
}

/******************************************************************************/
static int gpmc_read(struct file *filp, char __user *buff, size_t count, loff_t *offp)
{
	return 0;
}
static int gpmc_write(struct file *filp,const char __user *buff, size_t count, loff_t *offp)
{
	return 0;
}

static struct file_operations dev_fops = {
  .owner =   THIS_MODULE,
  .read	 =   gpmc_read,
  .write =   gpmc_write,
};

static struct miscdevice misc = {
  .minor = MISC_DYNAMIC_MINOR,
  .name = DEVICE_NAME,
  .fops = &dev_fops,
};

static void gpmc_config(void)
{
	void __iomem * gpmc_config1_1  = ioremap(GPMC_CONFIG1_1_REG , 4);
	void __iomem * gpmc_config2_1  = ioremap(GPMC_CONFIG2_1_REG , 4);
	void __iomem * gpmc_config3_1  = ioremap(GPMC_CONFIG3_1_REG , 4);
	void __iomem * gpmc_config4_1  = ioremap(GPMC_CONFIG4_1_REG , 4);
	void __iomem * gpmc_config5_1  = ioremap(GPMC_CONFIG5_1_REG , 4);
	void __iomem * gpmc_config6_1  = ioremap(GPMC_CONFIG6_1_REG , 4);
	void __iomem * gpmc_config7_1  = ioremap(GPMC_CONFIG7_1_REG , 4);
//	void __iomem * gpmc_irq_enable = ioremap(GPMC_IRQ_ENABLE_REG , 4);
//	void __iomem * gpmc_syscfg   = ioremap(GPMC_SYSCONFIG_REG , 4);
	int GPMC_CONFIG7_1_reg,GPMC_CONFIG1_1_reg;

	iowrite32(GPMC_CONFIG2_1,gpmc_config2_1);wmb();
	iowrite32(GPMC_CONFIG3_1,gpmc_config3_1);wmb();
	iowrite32(GPMC_CONFIG4_1,gpmc_config4_1);wmb();
	iowrite32(GPMC_CONFIG5_1,gpmc_config5_1);wmb();
	iowrite32(GPMC_CONFIG6_1,gpmc_config6_1);wmb();

//	iowrite32(0x8,gpmc_syscfg);wmb();
//	iowrite32(0,gpmc_irq_enable);wmb();                  //disable irq

	GPMC_CONFIG7_1_reg = ioread32(gpmc_config7_1);
	GPMC_CONFIG1_1_reg = ioread32(gpmc_config1_1);

	GPMC_CONFIG7_1_reg &= ~GPMC_CONFIG7_CSVALID;         //disable chip select
	GPMC_CONFIG7_1_reg &= ~0x1f;
	GPMC_CONFIG7_1_reg |= 2 ;                            //chip select baseaddress  0x02000000
	GPMC_CONFIG7_1_reg |= GPMC_SIZE_16M << 8;	         //gpmc size 16M
	iowrite32(GPMC_CONFIG7_1_reg,gpmc_config7_1);wmb();  // GPMC_CONFIG7_1  0x00000f02

	GPMC_CONFIG1_1_reg |= GPMC_CONFIG1_DEVICESIZE_16;      //设置device size 16bit
	iowrite32(GPMC_CONFIG1_1_reg,gpmc_config1_1);wmb();    //GPMC_CONFIG1_1  0x00001000
	//printk("gpmc_config7_1:%08x,gpmc_config1_1:%08x\n",ioread32(gpmc_config7_1),ioread32(gpmc_config1_1));

	GPMC_CONFIG7_1_reg |= GPMC_CONFIG7_CSVALID;                     //enable chip select
	iowrite32(GPMC_CONFIG7_1_reg,gpmc_config7_1);
	//printk("config2_1:%08x, config4_1:%08x\n",ioread32(gpmc_config2_1),ioread32(gpmc_config4_1));
	//printk("config3_1:%08x, config5_1:%08x, config6_1:%08x\n",ioread32(gpmc_config3_1),ioread32(gpmc_config5_1),ioread32(gpmc_config6_1));

	iounmap(gpmc_config1_1);
	iounmap(gpmc_config2_1);
	iounmap(gpmc_config3_1);
	iounmap(gpmc_config4_1);
	iounmap(gpmc_config5_1);
	iounmap(gpmc_config6_1);
	iounmap(gpmc_config7_1);
//	iounmap(gpmc_irq_enable);
//  iounmap(gpmc_syscfg);
}


static int __init dev_init(void)
{
  int ret;

  ret = misc_register(&misc);

  if (ret != 0)
  {
    printk(DEVICE_NAME " initialized failed\n");
    return -1;
  }
  printk(DEVICE_NAME " initialized successed\n");

  pinmux_config();
  gpmc_config();

  return ret;
}

static void __exit dev_exit(void)
{
  misc_deregister(&misc);
  printk(DEVICE_NAME " exit\n");
}

module_init(dev_init);
module_exit(dev_exit);

MODULE_LICENSE("GPL");
MODULE_VERSION ("V3.00");
MODULE_AUTHOR("anhui egret electronic company");
MODULE_DESCRIPTION("module driver of gpmc");
