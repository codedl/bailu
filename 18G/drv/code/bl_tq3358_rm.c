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
#include <asm/irq.h>
#include <linux/interrupt.h>
//#include <mach/mux.h>
#include <asm/gpio.h>
#include <mach/edma.h>
#include <linux/dma-mapping.h>
#include <linux/mm.h>
//#include <mach/memory.h>
//#include <mach/hardware.h>
#include <linux/types.h>
//#include <linux/module.h>
#include <linux/cdev.h>
//#include <linux/fs.h>
//#include <linux/device.h>
#include <linux/gpio.h>
#include <linux/irq.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/uaccess.h>
#include <mach/gpio.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <asm/io.h>
//input: D1; output: D2;

#define DEVICE_NAME        "bl_dsa_rm"         //驱动名称

#undef EDMA3_DEBUG
#define EDMA3_DEBUG

#ifdef EDMA3_DEBUG
  #define DMA_PRINTK(ARGS...)  printk(KERN_INFO "<%s>: ",__FUNCTION__);printk(ARGS)
  #define DMA_FN_IN printk(KERN_INFO "[%s]: start\n", __FUNCTION__)
  #define DMA_FN_OUT printk(KERN_INFO "[%s]: end\n",__FUNCTION__)
#else
  #define DMA_PRINTK( x... )
  #define DMA_FN_IN
  #define DMA_FN_OUT
#endif

#define ISP_BASEADDR       0x01C14128          //ISP基地址
#define GPIO_BASEADDR      0x01e26010          //GPIO基地址
#define PUPD_BASEADDR      0x01e2C00C          //PULL UP/DOWN 基地址
#define ISP_SIZE           0x08                //ISP SIZE
#define GPIO_SIZE          0xff                //GPIO SIZE
#define PUPD_SIZE          0x10                //PUPD SIZE

#define IF_PROC_BASE       0x60000100          	//中频控制基地址
#define IF_PROC_SIZE       0xFF              	//中频控制基地址长度

#define DMA_CHANNEL_IF_DATA_NUMBER      6              //全景中频分析
#define DMA_CHANNEL_DEMOD_IO_NUMBER     6              //解调IO数据

#define DMA_CHANNEL_IF_DATA_ADDRESS     0x60010000
#define DMA_CHANNEL_DEMOD_IO_ADDRESS    0x60014000

volatile u8* if_proc_remapbuf;                 //中频控制指针
volatile u8* if_data_remapbuf;                 //中频数据指针
volatile u8* if_addr_remapbuf;                 //中频控制指针

static int irq = 0;                            //中断
static unsigned long buf[4] = {0};             //中断缓冲区  buf[0]:RAM1中断   buf[1]:RAM2中断  buf[2]:中断序号  buf[3]:备用

#define NO_USE 0                               //系统分配
#define IFDATASIZE  (8192 + 4)                 //中频数据点数(Sweep: 2048 / FFT: 2048)
#define FSDATASIZE  1024                       //场强点数
#define FFTDATASIZE  2048                       //FFT点数

static unsigned long ifDataPoint = 0;          //中频点数
static unsigned long ifData[IFDATASIZE];       //中频数据
static unsigned long fsData[FSDATASIZE];       //场强数据

int dataMode = 0;                       //数据模式    0:无       1:扫频       2:FFT   3:场强
static int ev_press = 0;                       //中断触发
static unsigned long counterStart = 0;         //缓冲区偏移

static DECLARE_WAIT_QUEUE_HEAD(fpga_waitq);    //中断延时


//首先是定义一个结构体，其实这个结构体存放的是一个列表，这个列表保存的是一系列设备文件，SIGIO信号就发送到这些设备上
static struct fasync_struct *fasync_queue;

static volatile int irqraised = 0;

#define MAX_BUFF_IN_BYTES 			(IFDATASIZE * 2)
#define MAX_DMA_TRANSFER_IN_BYTES   (MAX_BUFF_IN_BYTES * 2)
#define STATIC_SHIFT                3
#define TCINTEN_SHIFT               20
#define ITCINTEN_SHIFT              21
#define TCCHEN_SHIFT                22
#define ITCCHEN_SHIFT               23

dma_addr_t dmaphysdest = 0;

unsigned short *dmabufdest = NULL;   //全景中频
//unsigned short FpgaData[MAX_DMA_TRANSFER_IN_BYTES] = {0};

static int point = 4096;//2048;
static int bcnt = 1;
static int ccnt = 1;

int dmaChannel = 0;

module_param(point, int, S_IRUGO);
module_param(bcnt, int, S_IRUGO);
module_param(ccnt, int, S_IRUGO);

int edma3_memtomemcpy_dma(int channel,  unsigned long long srcAddress, int pointx, int bcnt, int ccnt, int sync_mode, int event_queue);

#define GPIO_TO_PIN(bank, gpio) (32 * (bank) + (gpio))

//定义中断所用的结构体
struct fpga_irq_desc
{
  int irq;        //对应的中断号
  int pin;        //对应的 GPIO端口
  int pin_name;   //对应的引脚描述，实际并未用到，保留
  int number;     //中断值，以传递给应用层/用户态
  char *name;     //名称
};

static struct fpga_irq_desc fpga_irqs[] =
{
  {NO_USE, GPIO_TO_PIN(1,28), 28, 1, "EINTD2"},   //EDMA 扫频
  //{NO_USE, GPIO_TO_PIN(0,6), 6, 1, "EINTD1"},   //EDMA IQ数据中断
};


static irqreturn_t irq_interrupt(int irq, void *dev_id)
{
//  printk("iq interrupt\n");
  return IRQ_RETVAL(IRQ_HANDLED);		//IRQ_HANDLED：ISR被正确调用且确实是它对应的设备产生了中断。
}

//#define TESTEDMADATA
#define PRINTEDMADATA

//#define MAIN

#ifdef TESTEDMADATA
int time = 0;
int error = 0;
int right = 0;
#endif

int xxxxx = 0;
static void callback(unsigned lch, u16 ch_status, void *data)
{
	  int count = 0;

	  switch(ch_status)
	  {
		case DMA_COMPLETE:

	#ifdef PRINTEDMADATA

			for(count = 0; count < 10; count++)
			{
				printk ("dmabufdest[%d] = 0x%04x\n",count,dmabufdest[count]);
			}

			printk ("xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\n",count,dmabufdest[count]);

			for(count = (point/2) - 10; count < (point/2 + 5); count++)
			{
				printk ("dmabufdest[%d] = 0x%04x\n",count,dmabufdest[count]);
			}
			memset(dmabufdest,0,sizeof(dmabufdest));
	#endif

	#ifdef TESTEDMADATA
			if(time != 0)
			{
				for(count = 0u; count < MAX_BUFF_IN_BYTES; count++)
				{
					if(dmabufdest[count] != dmabufsrc[count])
					{
						error++;
						right--;
						break;
					}
				}
				right++;
				printk("KERNEL:Num:%d | Right:%d | Error:%d  \n", time, right, error);
			}
			memcpy(dmabufsrc,dmabufdest,MAX_DMA_TRANSFER_IN_BYTES);
	#endif

	#ifdef MAIN
//			printk("rm driver\n");
			if(fasync_queue)
			{
//		      printk("rm driver: fasync_queue\n");
			  kill_fasync(&fasync_queue, SIGIO, POLL_IN);
			}
	#endif

	#ifdef TESTEDMADATA
			for (count = 0u; count < IFDATASIZE; count++)
			{
				dmabufdest[count] = 0;
			}

			time++;

			if(time > 0xffffffff)	time = 1;
			if(error > 0xffffffff)	error = 1;
			if(right > 0xffffffff)	right = 1;

			iowrite16(0x0001, if_proc_remapbuf + 0x32 * 2); wmb();
	#endif

//			for (count = 0u; count < MAX_BUFF_IN_BYTES; count++)
//			{
//				dmabufdest[count] = 0x55;
//			}


			break;

		case DMA_CC_ERROR:
			printk ("\n From Callback 1: DMA_CC_ERROR Channel %d status is: %u\n", lch, ch_status);
			break;

		default:
		  break;
	  }
}

/* DMA Channel, Mem-2-Mem Copy, ASYNC Mode, INCR Mode */
int edma3_memtomemcpy_dma(int channel,  unsigned long long srcAddress, int pointx, int bcnt, int ccnt, int sync_mode, int event_queue)
{
	int result = 0;
	unsigned int dma_ch = 0;
	int i;
	int count = 0;
	unsigned int Istestpassed = 0u;
	unsigned int numenabled = 0;
	unsigned int BRCnt = 0;
	int srcbidx = 0;
	int desbidx = 0;
	int srccidx = 0;
	int descidx = 0;

	printk("edma3 memory to memory copy\n");
	/*内核中定义的EDMA结构体
    struct edmacc_param
    {
	  unsigned int opt;
	  unsigned int src;
	  unsigned int a_b_cnt;
	  unsigned int dst;
	  unsigned int src_dst_bidx;
	  unsigned int link_bcntrld;
	  unsigned int src_dst_cidx;
	  unsigned int ccnt;
    };
     */
	//
	struct edmacc_param param_set;

	/* Initalize source and destination buffers */
	for (count = 0u; count < MAX_DMA_TRANSFER_IN_BYTES / 2; count++)
	{
		dmabufdest[count] = 0;
	}

	/* Set B count reload as B count. */
	BRCnt = bcnt;

	/* Setting up the SRC/DES Index */
	srcbidx = 0; //pointx;
	desbidx = 0;

	/* A Sync Transfer Mode */
	srccidx = pointx;
	descidx = pointx;

    //申请DMA通道
	/*
	 * int edma_alloc_channel(int channel,
	 *                        void (*callback)(unsigned channel, u16 ch_status, void *data),
                              void *data,
                              enum dma_event_q);
                返回值：成功则是通道号，失败则为负数
             参数channel：为你要申请的通道号，设为负数则为申请任意一个可用的通道；
     Callback函数：是DMA传输完成中断回到函数，传输完成后会调用该函数，参数data就是edma_alloc_channel()的第三个参数data传递过来的；
               参数data：传递给callback函数；
             参数dma_event_q：事件队列选择，定义如下：
                      enum dma_event_q
                      {
                        EVENTQ_0 = 0,
                        EVENTQ_1 = 1,
                        EVENTQ_2 = 2,
                        EVENTQ_3 = 3,
                        EVENTQ_DEFAULT = -1
                      };
                      EVENTQ_0优先级最高，EVENTQ_3优先级最低。
	 * */

	result = edma_alloc_channel (channel, callback, NULL, event_queue);

	if (result < 0)
	{
	  DMA_PRINTK ("\nedma3_memtomemcpytest_dma::edma_alloc_channel failed for dma_ch, error:%d\n", result);
	  return result;
	}

	dma_ch = result;

    edma_set_src (dma_ch, srcAddress, FIFO, W16BIT);   //全景中频
	//edma_set_src (dma_ch, srcAddress, INCR, W16BIT);
	edma_set_dest (dma_ch, (unsigned long)(dmaphysdest), INCR, W16BIT);
	//edma_set_dest (dma_ch, (unsigned long)(dmaphysdest), INCR, W16BIT);

	edma_set_src_index (dma_ch, srcbidx, srccidx);
	edma_set_dest_index (dma_ch, desbidx, descidx);
	/* A Sync Transfer Mode */
	edma_set_transfer_params (dma_ch, pointx, bcnt, ccnt, BRCnt, ASYNC);

	/* Enable the Interrupts on Channel 1 */
	edma_read_slot (dma_ch, &param_set);
	param_set.opt = 0;
	param_set.opt |= (1 << TCINTEN_SHIFT);
	param_set.opt |= (1 << STATIC_SHIFT);
	param_set.opt |= EDMA_TCC(EDMA_CHAN_SLOT(dma_ch));
	edma_write_slot (dma_ch, &param_set);

	/*
	 * Now enable the transfer as many times as calculated above.
	 */
	result = edma_start(dma_ch);

	if (result != 0)
	{
	  DMA_PRINTK ("edma3_memtomemcpytest_dma: davinci_start_dma failed \n");
	}

	return result;
}

static int init_edma(void)
{
  int result = 0;

  //全景中频DMA通道初始化
  edma3_memtomemcpy_dma(DMA_CHANNEL_IF_DATA_NUMBER, DMA_CHANNEL_IF_DATA_ADDRESS, 2 * point * 4, bcnt, ccnt, 0, 1);
  edma_stop(DMA_CHANNEL_IF_DATA_NUMBER);

//  //解调的IO数据 DMA 通道初始化
//  edma3_memtomemcpy_dma(DMA_CHANNEL_DEMOD_IO_NUMBER, DMA_CHANNEL_DEMOD_IO_ADDRESS, point * 4, bcnt, ccnt, 0, 0);
//  edma_stop(DMA_CHANNEL_DEMOD_IO_NUMBER);

  return result;
}

static void remove_edma(int channel)
{
  edma_stop(channel);
  edma_free_slot(channel);
  edma_free_channel(channel);
}

static int bl_isp_read(struct file *filp, char __user *buff, size_t count, loff_t *offp)
{
  unsigned long err;
  unsigned int i = 0;
  unsigned short hValue = 0;
  unsigned short lValue = 0;
  unsigned int no = 0;

//  printk("dataMode = %d\n",dataMode);

//  dataMode = 3;
  if (dataMode == 0)  //默认状态
  {
//    if (sizeof buf == count)  //读中断信息
//    {
//	  err = copy_to_user(buff, buf, sizeof buf);
//
//      if (buf[0] > 0) buf[0] = 0;
//      if (buf[1] > 0) buf[1] = 0;
//
//      return err ? -EFAULT : sizeof(buf);
//    }
  }
  else if (dataMode == 1)   //扫频模式
  {
	ifData[0] = buf[0];   // = 1 RAM1中断
	ifData[1] = buf[1];   // = 2 RAM2中断
	ifData[2] = buf[2];
	ifData[3] = buf[3];


	//fft data
	ifDataPoint = point;

	for (i = 0; i < ifDataPoint; i++)
	{
	  //FFT data
	  lValue = dmabufdest[2 * i];
	  hValue = dmabufdest[2 * i + 1];
	  ifData[i + 4] = ((hValue & 0xffff) << 16) | (lValue & 0xffff);         //偏移4位，逐个填充
	}

	err = copy_to_user(buff, ifData, count);

	return err ? -EFAULT : (sizeof(unsigned long) * (point * 2 + 4));
  }else if(dataMode == 3)
  {
	ifData[0] = buf[0];   // = 1 RAM1中断
	ifData[1] = buf[1];   // = 2 RAM2中断
	ifData[2] = buf[2];
	ifData[3] = buf[3];


	//fft data
	ifDataPoint = point;
	for (i = 0; i < ifDataPoint; i++)
	{
	  //FFT data
	  lValue = dmabufdest[2 * (i + ifDataPoint)];
	  hValue = dmabufdest[2 * (i + ifDataPoint) + 1];
	  ifData[i + 4] = ((hValue & 0xffff) << 16) | (lValue & 0xffff);         //偏移4位，逐个填充
	}

	err = copy_to_user(buff, ifData, count);

	return err ? -EFAULT : (sizeof(unsigned long) * (point * 2 + 4));
  }

  return 0;
}

static ssize_t bl_isp_write(struct file *filp, const char __user *buff, size_t count, loff_t *offp)
{
  return 0;
}

static int bl_isp_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
  int result = 0;

  if (cmd == 0xF1)  //设置中频数据点数，默认为2048
  {
	if (arg >= 0 && arg <= IFDATASIZE)
	{
	  ifDataPoint = arg;
	}
	else
	{
	  ifDataPoint = 0;
	}
  }
  else if (cmd == 0xF0)  //设置读取方式，扫频或FFT
  {
//	printk("arg = %d", arg);
	dataMode = arg;        //1代表扫频   2代表FFT 3 解调IO数据
  }
  else if (cmd == 0xF2)
  {
	counterStart = arg;
  }
//  else if (cmd == 0xE1)	//修改FFT点数
//  {
//	iowrite8(arg, if_proc_remapbuf + 0x02 * 2); wmb();
//  }

  else if (cmd == 0xC0)	//停止EDMA
  {
	edma_stop(DMA_CHANNEL_IF_DATA_NUMBER);
  }
  else if (cmd == 0xC1)	//启动EDMA
  {
    edma_start(DMA_CHANNEL_IF_DATA_NUMBER);
  }
  else
  {
    iowrite8(arg, if_proc_remapbuf + cmd * 2); wmb();
  }

  return 0;
}

static unsigned int bl_isp_poll( struct file *file, struct poll_table_struct *wait)
{
  unsigned int mask = 0;

  poll_wait(file, &fpga_waitq, wait);
  if (ev_press)
  {
	ev_press = 0;
	mask |= POLLIN | POLLRDNORM;
  }

  return mask;
}

//fasync方法的实现
static int my_fasync(int fd, struct file *filp, int on)
{
  int retval;

  //将该设备登记到fasync_queue队列中去
  retval = fasync_helper(fd,filp,on,&fasync_queue);
//  printk("rm:%d\n",retval);

  if(retval < 0)
  {
	return retval;
  }
  return 0;
}

static int bl_isp_open (struct inode *inode, struct file *filp)
{
  int retval;

  /*..processing..*/
  retval = my_fasync(inode, filp, 1);
  if(retval < 0)
  {
	printk("KERNEL:fasync_helper apply error!\n");
  }
  return 0;
}

//在驱动的release方法中我们再调用my_fasync方法
//.release	=	bl_isp_close,
/*在需要的地方（比如中断）调用下面的代码,就会向fasync_queue队列里的设备发送SIGIO信号
，应用程序收到信号，执行处理程序
  if (fasync_queue)
  kill_fasync(&fasync_queue, SIGIO, POLL_IN);
*/

static int bl_isp_close (struct inode *inode, struct file *filp)
{
  int retval;
  /*..processing..*/
  retval = my_fasync(-1, filp, 0);

  if(retval < 0)
  {
	printk("KERNEL:fasync_helper free error!\n");
  }
  return 0;
}

static struct file_operations dev_fops =
{
  .owner	=   THIS_MODULE,
  .open		= 	bl_isp_open,
  .release	=	bl_isp_close,
  .read	    =   bl_isp_read,
  .write	=   bl_isp_write,
  .poll     =   bl_isp_poll,
  .unlocked_ioctl	=   bl_isp_ioctl,
};

static struct miscdevice misc =
{
  .minor = MISC_DYNAMIC_MINOR,
  .name  = DEVICE_NAME,
  .fops  = &dev_fops,
};

#define CE2CFG_BASEADDR  0x68000010
#define PINMUX_BASEADDR  0x01C1414C
//#define EDMA1CFG_BASEADDR 0x01C1417C
//#define EDMA2CFG_BASEADDR 0x01C14180
#define SDRAM_CONFIG_ADDRESS 0X4C000008

static void init_emif(void)
{
//  int CE2CFG_reg  = 0, SDCR_reg = 0, DDRSDCR_reg = 0, DDRSDCR2_reg = 0, PINMUX_reg = 0;
//  void __iomem * remapbufCE2CFG  = ioremap(CE2CFG_BASEADDR , 4);
//  void __iomem * remapbufPINMUX  = ioremap(PINMUX_BASEADDR , 4);
////	void __iomem * remapbufEDMA1  = ioremap(EDMA1CFG_BASEADDR , 4);
////	void __iomem * remapbufEDMA2  = ioremap(EDMA2CFG_BASEADDR , 4);
//
//  PINMUX_reg = 0x11111111;
//  iowrite32(PINMUX_reg ,remapbufPINMUX) ;wmb();
//
//  CE2CFG_reg  = ioread32(remapbufCE2CFG); rmb();
//  CE2CFG_reg  |= 0x41;
//  iowrite32(CE2CFG_reg ,remapbufCE2CFG) ;wmb();
//
////	CE2CFG_reg  = ioread32(remapbufEDMA1); rmb();
////	CE2CFG_reg  |= 0x0A;
////	iowrite32(CE2CFG_reg ,remapbufEDMA1) ;wmb();
////
////	CE2CFG_reg  = ioread32(remapbufEDMA2); rmb();
////	CE2CFG_reg  |= (0x02 << 13);
////	iowrite32(CE2CFG_reg ,remapbufEDMA2) ;wmb();
//
//  iounmap(remapbufCE2CFG);
//  iounmap(remapbufPINMUX);
////	iounmap(remapbufEDMA1);
////	iounmap(remapbufEDMA2);

	void __iomem * sdram_config_addr  = ioremap(SDRAM_CONFIG_ADDRESS , 4);
	int sdram_config_reg;

	sdram_config_reg =  ioread32(sdram_config_addr); rmb();

	sdram_config_reg &= ~(7 << 29);
	sdram_config_reg |= (3 << 29);  //reg_sdram_type  0:ddr1   1:lpddr1   2:ddr2   3:ddr3

	sdram_config_reg &= ~(3 << 14);
	sdram_config_reg |= 1;           //16bit

	iounmap(sdram_config_addr);
}

static int __init dev_init(void)
{
  int ret = misc_register(&misc);
  int err = 0;
  int i = 0;

  if_proc_remapbuf = ioremap(IF_PROC_BASE, IF_PROC_SIZE);
  if(!if_proc_remapbuf)
  {
	printk("KERNEL:if_proc_remapbuf error!\n");
  }

  dataMode = 0;

  for (i = 0; i < sizeof fpga_irqs / sizeof fpga_irqs[0]; i++)
  {
	err = request_irq(gpio_to_irq(fpga_irqs[i].pin), irq_interrupt, IRQ_TYPE_EDGE_RISING, fpga_irqs[i].name, (void *)&fpga_irqs[i]);

	if(err)
	{
	  printk("KERNEL:irq request error!\n");
	}
  }

  dmabufdest = dma_alloc_coherent (NULL, MAX_DMA_TRANSFER_IN_BYTES, &dmaphysdest, 0);

  if (!dmabufdest)
  {
	printk("KERNEL:dmabufdest request error!\n");
	dma_free_coherent(NULL, MAX_DMA_TRANSFER_IN_BYTES, dmabufdest, dmaphysdest);
	return -ENOMEM;
  }

 // init_emif();

  init_edma();

  printk(DEVICE_NAME " initialized successed\n");

  return ret;
}

static void __exit dev_exit(void)
{
	int i = 0;

	for (i = 0; i < sizeof(fpga_irqs) / sizeof(fpga_irqs[0]); i++)
	{
		if (fpga_irqs[i].irq < 0)
		{
		  continue;
		}

		disable_irq(gpio_to_irq(fpga_irqs[i].pin));
		free_irq(gpio_to_irq(fpga_irqs[i].pin), (void *)&fpga_irqs[i]);
	}

	if (if_proc_remapbuf != NULL)
	{
	  iounmap((void *)if_proc_remapbuf);
	}

	remove_edma(DMA_CHANNEL_IF_DATA_NUMBER);

	dma_free_coherent(NULL, MAX_DMA_TRANSFER_IN_BYTES, dmabufdest, dmaphysdest);

	misc_deregister(&misc);
}

module_init(dev_init);
module_exit(dev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("BAILU");
MODULE_DESCRIPTION("RM200 RECEIVER FPGA INTERRUPT DRIVER");
