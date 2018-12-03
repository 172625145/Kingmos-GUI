#include <termios.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <asm/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>

#include <linux/input.h>
//#define PCEMUL
#define DEVNUM 1
static int fdary[DEVNUM];
static int maxfd = 0;
int KeybdRead(int *pcode, int *ptype);
static fd_set fdwatchset;
struct termios old_flags, new_flags;
#ifndef PCEMUL
int KeybdInit()
{
	char devname[40];
	int i;
	for(i = 0; i < DEVNUM; i ++)
	{
		sprintf(devname, "/dev/input/event%d", i);
		fdary[i] = open(devname, O_RDONLY);
		
		if(fdary[i] < 0)
		{
			printf("Keyboard open dev %d file fail \r\n", i);
			int j;
			for(j = 0; j > i; j ++)
			{
				close(fdary[j]);
				FD_CLR(fdary[i], &fdwatchset);
			}
			return -1;
		}
		if(fdary[i] > maxfd)
			maxfd = fdary[i];
		
		FD_SET(fdary[i], &fdwatchset);
	}
	printf("Keyboard dev init sucess \r\n");
	return 0;
}
void KeybdDeinit()
{
	int i;
	for( i = 0; i < DEVNUM; i ++)
	{
		FD_CLR(fdary[i], &fdwatchset);
		close(fdary[i]);
	}	
}

int KeybdRead(int *pcode, int *pval)
{
    int yalv;           /* loop counter */
    size_t read_bytes;  /* how many bytes were read */
    struct input_event ev; /* the events (up to 64 at once) */
	fd_set rfdset;
	int e;
    /* read() requires a file descriptor, so we check for one, and then open it */
	rfdset = fdwatchset;
	e = select (maxfd + 1, &rfdset, NULL, NULL, NULL);
	printf("select out %d \r\n", e);
	if(e > 0)
	{
		int i;
		for(i = 0; i < DEVNUM; i ++)
		{
			
			if(FD_ISSET(fdary[i], &rfdset))
			{
				read_bytes = read(fdary[i], &ev, sizeof(struct input_event));
		
				if (read_bytes < (int) sizeof(struct input_event)) {
					printf("keyboard error information");
					return -1;
				}
				printf("Event: time %ld.%06ld, type %d, code %d, value %d\n",
					ev.time.tv_sec, ev.time.tv_usec, ev.type,
					ev.code, ev.value);
				*pcode = (int) ev.code;
				*pval = (int)ev.value;
			}
		}
	}
	else
	{
		return -1;
	}
	return 0;
}
#else
int KeybdRead(int *pcode, int *ptype)
{
	static iType = 0;
	static char c;
	if(iType)
	{
		*pcode = (int)c; //key up
		*ptype = 0;
		iType = 0;
		return 0;
	}
	else
	{
		iType = 1;
	}
	int fd = fileno(stdin);
	if( read(fd, &c, 1) == 1 )
	{
		if(c == '@')
			return -1;
		*pcode = (int)c;
		*ptype = 1; //key down
		return 0;
	}
	return -1;
}
int KeybdInit()
{
	int fd;
	fd = fileno(stdin);
	tcgetattr(fd, &old_flags);
	
	new_flags = old_flags;
	new_flags.c_lflag &= ~(ECHO | ICANON | ISIG);
	new_flags.c_iflag &= ~(BRKINT | ICRNL);
	new_flags.c_oflag &= ~OPOST;
	new_flags.c_cc[VTIME] = 0;
	new_flags.c_cc[VMIN] = 1;

	if(tcsetattr(fd, TCSAFLUSH, &new_flags) < 0)
	{
		printf("fail to change attr \r\n");
		return -1;
	}
	return 0;
}

void KeybdDeinit()
{
	int fd = fileno(stdin);
	tcsetattr(fd, TCSANOW, &old_flags);
}
#endif