#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

void mousetest()
{
	int mouse_fd;
	unsigned char c;
	printf("mousetest enter .......\r\n");
	mouse_fd = open ("/dev/usbmouse", O_RDONLY);
	if(mouse_fd < 0)
	{
		printf("open mouse fail \r\n");
		return;
	}
	while(1)
	{
		int n;
		n = read(mouse_fd, &c, 1);
		if(n == 1)
		{
			printf("mouse info: 0x%x \r\n", c);
		}
		else
		{
			usleep(50000);
		}
	}
	printf("mousetest leave .......\r\n");
}