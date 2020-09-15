//源代码名称为 "shanwuyanAPP.c"
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
*./shanwuyanAPP <filename> <0:1> 0表示LED灭，1表示LED亮
*/
int main(int argc, char *argv[])
{
	int ret = 0;
	int fd = 0;
	char *filename;

	char user_data;
	
	user_data = atoi(argv[2]);

	if(argc != 3)
	{
		printf("Error usage!\r\n");
		return -1;
	}
		
	filename = argv[1];	//获取设备名称

	fd = open(filename, O_RDWR);
	if(fd < 0)
	{
		printf("cannot open device %s\r\n", filename);
		return -1;
	}

	/*写操作*/
	ret = write(fd, &user_data, sizeof(char));
	if(ret < 0)
		printf("write error!\r\n");

	/*关闭操作*/
	ret = close(fd);
	if(ret < 0)
	{
		printf("close device %s failed\r\n", filename);
	}

	return 0;
}