#include <linux/kernel.h>
#include <linux/major.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <linux/miscdevice.h>

#define DEVICE_NAME "BL_TQ3358_VERSION"

#define VER_MAJOR            1          //主版本号
#define VER_MINOR            0          //次版本号

static unsigned char rbuf[2];

static long version_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{

  return 0;
}

static int version_open(struct inode *inode, struct file *file)
{
  return 0;
}

static int version_close(struct inode *inode, struct file *file)
{

  return 0;
}
ssize_t version_write(struct file *filp, const char *buf, size_t count,loff_t *f_pos)
{

}

ssize_t version_read(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
	rbuf[0] = VER_MAJOR;
	rbuf[1] = VER_MINOR;
	copy_to_user(buf,rbuf,2);
	return 0;
}

static struct file_operations version_fops=
{
  .owner			=	THIS_MODULE,
  .unlocked_ioctl	=	version_ioctl,
  .open 			= 	version_open,
  .release    = 	version_close,
  .write      =   version_write,
  .read				= 	version_read,
};

static struct miscdevice misc =
{
  .minor	= MISC_DYNAMIC_MINOR,
  .name	= DEVICE_NAME,
  .fops	= &version_fops,
};

static int __init version_init(void)
{
  int ret;

  ret = misc_register(&misc);
  if (ret != 0)
  {
  	printk(DEVICE_NAME "initialized failed!\n");
  	return -1;
  }

  printk(DEVICE_NAME " initialized successed\n");
  return ret;

}

static void __exit version_exit(void)
{
  misc_deregister(&misc);
  printk("exit version driver!\n");
}

module_init(version_init);
module_exit(version_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("anhui egret electronic company");
MODULE_DESCRIPTION("version module driver");
