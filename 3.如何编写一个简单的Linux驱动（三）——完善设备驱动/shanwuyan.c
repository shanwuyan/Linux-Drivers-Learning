/* 源代码文件名为"shanwuyan.c"*/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>

#define SHANWUYAN_NAME "shanwuyan"

struct shanwuyan_dev
{
	struct cdev c_dev;		//字符设备
	dev_t dev_id;			//设备号
	struct class *class;	//类
	struct device *device;	//设备
	int major;				//主设备号
	int minor;				//次设备号
};

struct shanwuyan_dev shanwuyan;	//定义一个设备结构体

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
	char device_data[] = "device data";
	copy_to_user(buf, device_data, sizeof(device_data));	//向用户程序传输设备数据
	return 0;
}

/*写设备*/
static ssize_t shanwuyan_write(struct file *filp, const char __user *buf, size_t count, loff_t *ppos)
{
	char user_data[50];
	copy_from_user(user_data, buf, count);		//获取用户程序写到设备的数据
	printk("device get data:%s\r\n", user_data);
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
	
	/*1.分配设备号*/
	shanwuyan.major = 0;	//主设备号设置为0，表示用户不给定主设备号，主次设备号都由系统分配
	if(shanwuyan.major)		//如果给定了主设备号，则由系统分配次设备号
	{
		shanwuyan.dev_id = MKDEV(shanwuyan.major, 0);	//把用户给的主设备号和0号次设备号合并成一个设备号
		ret = register_chrdev_region(shanwuyan.dev_id, 1, SHANWUYAN_NAME);	//因为我们只考虑一个设备的情况，所以只分配一个设备号，即设备号0
	}
	else					//如果没有给定主设备号，则主次设备号全部由系统分配
	{
		ret = alloc_chrdev_region(&(shanwuyan.dev_id), 0, 1, SHANWUYAN_NAME);	//只考虑一个设备的情况
		shanwuyan.major = MAJOR(shanwuyan.dev_id);	//获取主设备号
		shanwuyan.minor = MINOR(shanwuyan.dev_id);	//获取次设备号
	}
	if(ret < 0)	//设备号分配失败，则打印错误信息，然后返回
	{
		printk(KERN_EMERG "shanwuyan chrdev_region error!\r\n");
		return -EINVAL;
	}
	else	//如果设备号分配成功，则打印设备的主次设备号
	{
		printk(KERN_EMERG "shanwuyan.major = %d, shanwuyan.minor = %d\r\n", shanwuyan.major, shanwuyan.minor);
	}

	/*2.向内核添加字符设备*/
	shanwuyan.c_dev.owner = THIS_MODULE;
	cdev_init(&(shanwuyan.c_dev), &(shanwuyan_fops));	//初始化字符设备结构体
	cdev_add(&(shanwuyan.c_dev), shanwuyan.dev_id, 1);	//添加设备到内核

	/*3.自动创建设备节点*/
	shanwuyan.class = class_create(THIS_MODULE, SHANWUYAN_NAME);	//创建类
	shanwuyan.device = device_create(shanwuyan.class, NULL, shanwuyan.dev_id, NULL, SHANWUYAN_NAME);

	return 0;
}

static void __exit shanwuyan_exit(void)	//驱动出口函数
{
	/*注销设备号*/
	unregister_chrdev_region(shanwuyan.dev_id, 1);
	/*摧毁设备*/
	device_destroy(shanwuyan.class, shanwuyan.dev_id);
	/*摧毁类*/
	class_destroy(shanwuyan.class);
}

module_init(shanwuyan_init);	//注册入口函数
module_exit(shanwuyan_exit);	//注册出口函数

MODULE_LICENSE("GPL");	//同意GPL开源协议
MODULE_AUTHOR("shanwuyan");	//添加作者名称