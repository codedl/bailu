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
	{ 0x4bc1a69e, "kill_fasync" },
	{ 0xb9e52429, "__wake_up" },
	{ 0xd6b8e852, "request_threaded_irq" },
	{ 0x40a6f522, "__arm_ioremap" },
	{ 0x297aefda, "misc_register" },
	{ 0x67c2fa54, "__copy_to_user" },
	{ 0x826a8b08, "fasync_helper" },
	{ 0x27e1a049, "printk" },
	{ 0x45a55ec8, "__iounmap" },
	{ 0xf20dabd8, "free_irq" },
	{ 0x3ce4ca6f, "disable_irq" },
	{ 0x11f447ce, "__gpio_to_irq" },
	{ 0x9a177b5, "misc_deregister" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "4BE71DDD2A9C8DA5C5C770F");
