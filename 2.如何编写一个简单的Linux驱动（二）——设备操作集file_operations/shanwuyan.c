/* 源代码文件名为"shanwuyan.c"*/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define SHANWUYAN_MAJOR 200
#define SHANWUYAN_NAME "shanwuyan"

/*打开设备*/
static int shanwuyan_open(struct inode *inode, struct file *filp)
{
	printk(KERN_EMERG "shanwuyan_open\r\n");
	return 0;
}

/*释放（关闭）设备*/
static int shanwuyan_release(struct inode *inode, struct file *filp)
{
	printk(KERN_EMERG "shanwuyan_close\r\n");
	return 0;
}

/*读设备*/
static ssize_t shanwuyan_read(struct file *filp, char __user *buf, size_t count, loff_t *ppos)
{
	printk(KERN_EMERG "shanwuyan_read\r\n");
	return 0;
}

/*写设备*/
static ssize_t shanwuyan_write(struct file *filp, const char __user *buf, size_t count, loff_t *ppos)
{
	printk(KERN_EMERG "shanwuyan_write\r\n");
	return 0;
}

static struct file_operations shanwuyan_fops = 
{
	.owner = THIS_MODULE,			//默认
	.open = shanwuyan_open,			//打开设备
	.release = shanwuyan_release,	//关闭设备
	.read = shanwuyan_read,			//读设备
	.write = shanwuyan_write,		//写设备
};

static int __init shanwuyan_init(void)	//驱动入口函数
{
	int ret = 0;

	ret = register_chrdev(SHANWUYAN_MAJOR, SHANWUYAN_NAME, &shanwuyan_fops);
	if(ret < 0)
		printk(KERN_EMERG "init failed\r\n");	//注册失败
	else
		printk(KERN_EMERG "shanwuyan_init\r\n");//注册成功
	return 0;
}

static void __exit shanwuyan_exit(void)	//驱动出口函数
{
	unregister_chrdev(SHANWUYAN_MAJOR, SHANWUYAN_NAME);	//注销字符设备
	printk(KERN_EMERG "shanwuyan_exit\r\n");
}

module_init(shanwuyan_init);	//注册入口函数
module_exit(shanwuyan_exit);	//注册出口函数

MODULE_LICENSE("GPL");	//同意GPL开源协议
MODULE_AUTHOR("shanwuyan");	//添加作者名称