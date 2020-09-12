//文件名为"shanwuyan_APP.c"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

/*
*argc:应用程序参数个数，包括应用程序本身
*argv[]:具体的参数内容，字符串形式
*./shanwuyan_APP <filename> <r:w>	r表示读，w表示写
*/
int main(int argc, char *argv[])
{
	int ret = 0;
	int fd = 0;
	char *filename;

	if(argc != 3)	//共有三个参数
	{
		printf("Error usage!\r\n");
		return -1;
	}
		
	filename = argv[1];	//获取文件名称

	fd = open(filename, O_RDWR);
	if(fd < 0)
	{
		printf("cannot open file %s\r\n", filename);
		return -1;
	}

	if(!strcmp(argv[2], "r"))	//读设备
	{
		
		read(fd, NULL, 0);	//只是使用读函数，但不读出数据
	}
	else if(!strcmp(argv[2], "w"))	//写设备
	{
		write(fd, NULL, 0);	//只是使用

	}
	else
	{
		printf("ERROR usage!\r\n");
	}

	/*关闭设备*/
	close(fd);

	return 0;
}