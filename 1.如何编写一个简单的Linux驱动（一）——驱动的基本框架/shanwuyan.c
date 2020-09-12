/* 源代码文件名为：shanwuyan.c */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

static int __init shanwuyan_init(void)	//驱动入口函数
{
	printk(KERN_EMERG "shanwuyan_init\r\n");
	return 0;
}

static void __exit shanwuyan_exit(void)	//驱动出口函数
{
	printk(KERN_EMERG "shanwuyan_exit\r\n");
}

module_init(shanwuyan_init);	//注册入口函数
module_exit(shanwuyan_exit);	//注册出口函数

MODULE_LICENSE("GPL");	//同意GPL开源协议
MODULE_AUTHOR("shanwuyan");	//添加作者名称