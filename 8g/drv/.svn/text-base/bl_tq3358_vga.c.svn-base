/*
 * bl_tq3358_vga.c
 *
 *  Created on: 2015-12-16
 *      Author: Administrator
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/mutex.h>
#include <linux/sysfs.h>
#include <linux/mod_devicetable.h>
#include <linux/log2.h>
#include <linux/bitops.h>
#include <linux/jiffies.h>
#include <linux/of.h>
#include <linux/i2c.h>

#define DEVICE_NAME "BL_TQ3358_VGA"

//#define DEBUG
#ifdef DEBUG
  #define printd(fmt,args...) printk(fmt,##args)
#else
  #define printd(fmt,args...)
#endif

struct vgareg
{
  int reg;
  int val;
};

static struct vgareg vgaregs[] = {

{ 0x02, 0x01 },
{ 0x02, 0x03 },
{ 0x03, 0x00 },
{ 0x04, 0x39 },
{ 0x07, 0x1B },
{ 0x08, 0x08 },
{ 0x09, 0x80 },
{ 0x0C, 0x02 },
{ 0x0D, 0x08 },
{ 0x0F, 0x23 },
{ 0x10, 0x20 },
{ 0x11, 0x4C },
{ 0x12, 0x41 },
{ 0x13, 0x04 },
{ 0x14, 0x14 },
{ 0x15, 0x12 },
{ 0x16, 0x58 },
{ 0x17, 0x80 },
{ 0x19, 0x1C },
{ 0x1A, 0x0A },
{ 0x1B, 0x23 },
{ 0x1C, 0x20 },
{ 0x1D, 0x20 },
{ 0x1F, 0x28 },
{ 0x20, 0x80 },
{ 0x21, 0x12 },
{ 0x22, 0x58 },
{ 0x23, 0x74 },
{ 0x25, 0x01 },
{ 0x26, 0x04 },
{ 0x37, 0x20 },
{ 0x39, 0x20 },
{ 0x3B, 0x20 },
{ 0x41, 0x9A },
{ 0x4D, 0x03 },
{ 0x4E, 0x50 },
{ 0x4F, 0xDA },
{ 0x50, 0x74 },
{ 0x51, 0x4B },
{ 0x52, 0x12 },
{ 0x53, 0x13 },
{ 0x55, 0xE5 },
{ 0x5E, 0x80 },
{ 0x69, 0x64 },
{ 0x7D, 0x62 },
{ 0x04, 0x38 },
{ 0x06, 0x71 },

/*
NOTE: The following five repeated sentences are used here to wait memory initial complete, please don't remove...(you could refer to Appendix A of programming guide document (CH7025(26)B Programming Guide Rev2.03.pdf or later version) for detailed information about memory initialization!
*/
{ 0x03, 0x00 },
{ 0x03, 0x00 },
{ 0x03, 0x00 },
{ 0x03, 0x00 },
{ 0x03, 0x00 },

{ 0x06, 0x70 },
{ 0x02, 0x02 },
{ 0x02, 0x03 },
{ 0x04, 0x00 },

};

struct vga_data
{
  struct device	*i2c_dev;
  struct mutex	update_lock;
};

static const unsigned short normal_i2c[] = {0x76, I2C_CLIENT_END};

static const struct i2c_device_id vga_ids[] =
{
  { "vga", 1, },
  {/* LIST END */}
};

MODULE_DEVICE_TABLE(i2c, vga_ids);

static int vga_write_value(struct i2c_client *client,u8 reg, u16 value)
{
  printd("vga write reg:%02x val:%02x\n",reg,value);
  return i2c_smbus_write_byte_data(client, reg, value);
}

static int vga_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
  struct vga_data *data;
  int i = 0, ret;

  if (!i2c_check_functionality(client->adapter, I2C_FUNC_SMBUS_BYTE_DATA | I2C_FUNC_SMBUS_WORD_DATA))
  {
    return -EIO;
  }

  data = kzalloc(sizeof(struct vga_data), GFP_KERNEL);
  if (!data)
  {
	return -ENOMEM;
  }

  i2c_set_clientdata(client, data);
  mutex_init(&data->update_lock);

  printd("write reg val\n");
  for (i = 0; i < ARRAY_SIZE(vgaregs); ++i)
  {
    ret = vga_write_value(client,vgaregs[i].reg,vgaregs[i].val);
	if (ret < 0)
	{
	  printk("i2c write error\n");
	  mutex_unlock(&data->update_lock);
	  return ret;
	}

	udelay(1000);
  }

  mutex_unlock(&data->update_lock);
  printk(DEVICE_NAME " initialized successed\n");

  return 0;
}

static int vga_remove(struct i2c_client *client)
{
  struct vga_data *data = i2c_get_clientdata(client);

  //hwmon_device_unregister(data->hwmon_dev);
  printd("vga remove\n");
  kfree(data);
  return 0;
}

/*
static const struct dev_pm_ops vga_dev_pm_ops = {
	.suspend = vga_suspend,
	.resume = vga_resume,
};
*/

static struct i2c_driver vga_driver =
{
  .driver =
  {
	.name = "vga",
	.owner = THIS_MODULE,
    //.pm = &vga_dev_pm_ops,
  },

  .probe = vga_probe,
  .remove = vga_remove,
  .id_table = vga_ids,
  .address_list = normal_i2c,
};

static int __init vga_init(void)
{
  return i2c_add_driver(&vga_driver);
}

module_init(vga_init);

static void __exit vga_exit(void)
{
  i2c_del_driver(&vga_driver);
}

module_exit(vga_exit);

MODULE_LICENSE("GPL");
MODULE_VERSION ("V3.00");
MODULE_AUTHOR("anhui egret electronic company");
MODULE_DESCRIPTION("module driver of vga(800 * 600)");
