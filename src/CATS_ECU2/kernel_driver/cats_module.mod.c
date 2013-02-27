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
	{ 0xb0ac5d2a, "struct_module" },
	{ 0x28f0f052, "cdev_alloc" },
	{ 0x63919665, "cdev_del" },
	{ 0x2322e9e5, "cdev_init" },
	{ 0xd8e484f0, "register_chrdev_region" },
	{ 0xd502149, "malloc_sizes" },
	{ 0xbb15e6c0, "pci_disable_device" },
	{ 0xeae3dfd6, "__const_udelay" },
	{ 0x721f0c32, "pci_release_regions" },
	{ 0x7485e15e, "unregister_chrdev_region" },
	{ 0xd7474566, "__copy_to_user_ll" },
	{ 0x1af40e18, "__copy_from_user_ll" },
	{ 0x1b7d4074, "printk" },
	{ 0x745c0134, "fasync_helper" },
	{ 0xf43c1fde, "cdev_add" },
	{ 0xcd16af68, "kmem_cache_alloc" },
	{ 0xed633abc, "pv_irq_ops" },
	{ 0xe914d009, "ioremap_nocache" },
	{ 0x1fc91fb2, "request_irq" },
	{ 0x857972d5, "pci_unregister_driver" },
	{ 0x37a0cba, "kfree" },
	{ 0x2e60bace, "memcpy" },
	{ 0x92726c65, "pci_request_regions" },
	{ 0xedc03953, "iounmap" },
	{ 0xed79fc7, "__pci_register_driver" },
	{ 0xc7747db3, "kill_fasync" },
	{ 0x5b4bba86, "pci_enable_device" },
	{ 0x29537c9e, "alloc_chrdev_region" },
	{ 0xf20dabd8, "free_irq" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";

MODULE_ALIAS("pci:v00001B3Cd00000001sv*sd*bc*sc*i*");
