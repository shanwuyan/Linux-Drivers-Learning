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
*./shanwuyanAPP <filename> <r:w> r表示读，w表示写
*/
int main(int argc, char *argv[])
{
	int ret = 0;
	int fd = 0;
	char *filename;
	char readbuf[50];
	char user_data[] = "user data";

	if(argc != 3)
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
	/*读操作*/
	if(!strcmp(argv[2], "r"))
	{
		read(fd, readbuf, 50);
		printf("user get data:%s\r\n", readbuf);
	}
	/*写操作*/
	else if(!strcmp(argv[2], "w"))
	{
		write(fd, user_data, 50);
	}
	else
	{
		printf("ERROR usage!\r\n");
	}

	/*关闭操作*/
	ret = close(fd);
	if(ret < 0)
	{
		printf("close file %s failed\r\n", filename);
	}

	return 0;
}