#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
 .arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x372d36bb, "module_layout" },
	{ 0x67c2fa54, "__copy_to_user" },
	{ 0xf9a482f9, "msleep" },
	{ 0x885dde13, "spi_sync" },
	{ 0xfa2a45e, "__memzero" },
	{ 0x5f754e5a, "memset" },
	{ 0x701e8042, "device_unregister" },
	{ 0x2d213583, "spi_setup" },
	{ 0x74b31f1b, "spi_new_device" },
	{ 0xe7106dd, "spi_busnum_to_master" },
	{ 0x432fd7f6, "__gpio_set_value" },
	{ 0xa8f59416, "gpio_direction_output" },
	{ 0x47229b5c, "gpio_request" },
	{ 0x297aefda, "misc_register" },
	{ 0x27e1a049, "printk" },
	{ 0x9a177b5, "misc_deregister" },
	{ 0xfe990052, "gpio_free" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "95E2E32DED7F3321A49B012");
