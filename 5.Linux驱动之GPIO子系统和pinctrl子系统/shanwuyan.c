/* 源代码文件名为"shanwuyan.c"*/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>
//新添加如下四个头文件
#include <linux/of_gpio.h>
#include <linux/gpio.h>
#include <linux/of.h>
#include <linux/io.h>

#define SHANWUYAN_NAME "shanwuyan_leds"	//修改驱动名称
#define LEDS_NUM 2	//LED灯的个数为2
#define LEDS_ON	1	//LED灯的开启状态
#define LEDS_OFF 0	//lED灯的关闭状态

struct shanwuyan_dev
{
	struct cdev c_dev;		//字符设备
	dev_t dev_id;			//设备号
	struct class *class;	//类
	struct device *device;	//设备
	int major;				//主设备号
	int minor;				//次设备号
	struct device_node *node;	//用于获取设备树节点
	int led_gpios[2];	//用于获取两个led的GPIO编号
};

struct shanwuyan_dev shanwuyan;	//定义一个设备结构体

/*打开设备*/
static int shanwuyan_open(struct inode *inode, struct file *filp)
{
	printk(KERN_EMERG "shanwuyan_open\r\n");
	filp->private_data = &shanwuyan;
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
	int i = 0;
	int ret = 0;

	char user_data;	//保存用户数据

	struct shanwuyan_dev *led_dev = filp->private_data;	//获取私有变量
	
	ret = copy_from_user(&user_data, buf, count);	//获取用户数据
	if(ret < 0)
		return -EINVAL;

	if(user_data == LEDS_ON)	//如果接到的命令是打开LED
		for(i = 0; i < LEDS_NUM; i++)
			gpio_set_value(led_dev->led_gpios[i], LEDS_ON);
	else if(user_data == LEDS_OFF)	//如果接到的命令是关闭LED
		for(i = 0; i < LEDS_NUM; i++)
			gpio_set_value(led_dev->led_gpios[i], LEDS_OFF);

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

static int leds_init(struct shanwuyan_dev *leds_dev)//初始化led的GPIO
{
	int ret = 0;
	int i = 0;
	char led_labels[][20] = {"led_gpio_0", "led_gpio_1"};	//定义两个设备标签

	/*1.根据设备节点在设备树中的路径，获取设备树中的设备节点*/
	leds_dev->node = of_find_node_by_path("/shanwuyan_leds");
	if(leds_dev->node == NULL)
	{
		ret = -EINVAL;
		printk("cannot find node /shanwuyan_leds\r\n");
		goto fail_find_node;
	}
	
	/*2.获取led对应的gpio*/
	for(i = 0; i < LEDS_NUM; i++)
	{
		leds_dev->led_gpios[i] = of_get_named_gpio(leds_dev->node, "led-gpios", i);	
		if(leds_dev->led_gpios[i] < 0)	//如果获取失败
		{
			printk("cannot get led_gpio_%d\r\n", i);
			ret = -EINVAL;
			goto fail_get_gpio;
		}	
	}		
	for(i = 0; i < LEDS_NUM; i++)	//打印出获取的gpio编号
		printk("led_gpio_%d = %d\r\n", i, leds_dev->led_gpios[i]);

	/*3.申请gpio*/
	for(i = 0; i < LEDS_NUM; i++)
	{
		ret = gpio_request(leds_dev->led_gpios[i], led_labels[i]);
		if(ret)	//如果申请失败
		{
			printk("cannot request the led_gpio_%d\r\n", i);
			ret = -EINVAL;
			if(i == 1)
				goto fail_request_gpio_1;
			else
				goto fail_request_gpio_0;
		}
	}
	

	/*4.使用gpio：设置为输出*/
	for(i = 0; i < LEDS_NUM; i++)
	{
		ret = gpio_direction_output(leds_dev->led_gpios[i], 1);
		if(ret)	//如果是指失败
		{
			printk("failed to set led_gpio_%d\r\n", i);
			ret = -EINVAL;
			goto fail_set_output;
		}
	}
	
	/*5.设置输出高电平，led会亮*/
	for(i = 0; i < LEDS_NUM; i++)
		gpio_set_value(leds_dev->led_gpios[i], 1);

	return 0;

fail_set_output:
	gpio_free(leds_dev->led_gpios[1]);	//释放掉led_gpio_1
fail_request_gpio_1:
	gpio_free(leds_dev->led_gpios[0]);//如果led_gpio_1申请失败，则也要把led_gpio_0也要释放掉
fail_request_gpio_0:
fail_get_gpio:
fail_find_node:
	return ret;
}

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

	/*4.初始化GPIO*/
	leds_init(&shanwuyan);

	return 0;
}

static void __exit shanwuyan_exit(void)	//驱动出口函数
{
	int i = 0;
	/*释放GPIO*/
	for(i = 0; i < LEDS_NUM; i++)
		gpio_free(shanwuyan.led_gpios[i]);
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