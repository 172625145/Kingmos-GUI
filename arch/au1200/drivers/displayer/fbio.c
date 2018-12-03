#include <stdio.h>
#include <fcntl.h>
#include "fb.h"
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <time.h>
#define AU1200_LCD_FB_IOCTL 0x46FF

#define AU1200_LCD_SET_SCREEN 1
#define AU1200_LCD_GET_SCREEN 2
#define AU1200_LCD_SET_WINDOW 3
#define AU1200_LCD_GET_WINDOW 4
#define AU1200_LCD_SET_PANEL  5
#define AU1200_LCD_GET_PANEL  6

#define SCREEN_SIZE		    (1<< 1)
#define SCREEN_BACKCOLOR    (1<< 2)
#define SCREEN_BRIGHTNESS   (1<< 3)
#define SCREEN_COLORKEY     (1<< 4)
#define SCREEN_MASK         (1<< 5)
static int g_ifbd = -1;
typedef struct au1200_lcd_global_regs_t
{
    unsigned int flags;
    unsigned int xsize;
    unsigned int ysize;
    unsigned int backcolor;
    unsigned int brightness;
	unsigned int colorkey;
	unsigned int mask;
    unsigned int panel_choice;
    char panel_desc[80];

} au1200_lcd_global_regs_t;

#define WIN_POSITION            (1<< 0)
#define WIN_ALPHA_COLOR         (1<< 1)
#define WIN_ALPHA_MODE          (1<< 2)
#define WIN_PRIORITY            (1<< 3)
#define WIN_CHANNEL             (1<< 4)
#define WIN_BUFFER_FORMAT       (1<< 5)
#define WIN_COLOR_ORDER         (1<< 6)
#define WIN_PIXEL_ORDER         (1<< 7)
#define WIN_SIZE                (1<< 8)
#define WIN_COLORKEY_MODE       (1<< 9)
#define WIN_DOUBLE_BUFFER_MODE  (1<< 10)
#define WIN_RAM_ARRAY_MODE      (1<< 11)
#define WIN_BUFFER_SCALE        (1<< 12)
#define WIN_ENABLE	            (1<< 13)

typedef struct au1200_lcd_window_regs_t
{
    unsigned int flags;
    unsigned int xpos;
    unsigned int ypos;
    unsigned int alpha_color;
    unsigned int alpha_mode;
    unsigned int priority;
    unsigned int channel;
    unsigned int buffer_format;
    unsigned int color_order;
    unsigned int pixel_order;
    unsigned int xsize;
    unsigned int ysize;
    unsigned int colorkey_mode;
    unsigned int double_buffer_mode;
    unsigned int ram_array_mode;
    unsigned int xscale;
    unsigned int yscale;
    unsigned int enable;
} au1200_lcd_window_regs_t;


typedef struct au1200_lcd_iodata_t
{

    unsigned int subcmd;
    au1200_lcd_global_regs_t global;
    au1200_lcd_window_regs_t window;

} au1200_lcd_iodata_t;

typedef struct _VIEDO_INFO{
	unsigned long ulBitsPixel;
	unsigned long ulResx;
	unsigned long ulResy;
	unsigned long ulWidthBytes;
	unsigned char *pFrameBuf;	
}VIEDO_INFO, *PVIEDO_INFO;

int SetLcdBrightness(int iBrightNessVal)
{
	au1200_lcd_iodata_t iodata;
    iodata.subcmd = AU1200_LCD_SET_SCREEN;
    iodata.global.flags = SCREEN_BRIGHTNESS;
    iodata.global.brightness = iBrightNessVal;
    ioctl(g_ifbd, AU1200_LCD_FB_IOCTL, &iodata);	
	return 0;
}

int GetLcdCurntBrightness()
{	
	au1200_lcd_iodata_t iodata;
    iodata.subcmd = AU1200_LCD_GET_SCREEN;
    iodata.global.flags = SCREEN_BRIGHTNESS;

    ioctl(g_ifbd, AU1200_LCD_FB_IOCTL, &iodata);	
	return  iodata.global.brightness;
}

int GetLcdBrightnessRange(int *piMinVal, int *piMaxVal)
{
	*piMinVal = 30;
	*piMaxVal = 255;
	return 0;
}

int InitFb(PVIEDO_INFO pViedoInfo)
{
	char *	env;
	int	type, visual;
	int fb;
	int x, y, nPerPixel, size, nPageSize;
	struct fb_fix_screeninfo fb_fix;
	struct fb_var_screeninfo fb_var;

	printf("InitFb entry.\r\n");

	/* locate and open framebuffer, get info*/
	nPageSize = getpagesize();
	fb = open("/dev/fb0", O_RDWR);
	if(fb < 0) {
		printf("GAL fbcon engine: Error when opening dev. Please check kernel config.\n");
		return 0;
	}
	g_ifbd = fb;
	if(ioctl(fb, FBIOGET_FSCREENINFO, &fb_fix) == -1 ||
		ioctl(fb, FBIOGET_VSCREENINFO, &fb_var) == -1) {
			printf("GAL fbcon engine: Error when reading screen info: \n");
			return 0;
	}
	/* setup screen device from framebuffer info*/

	type = fb_fix.type;
	visual = fb_fix.visual;
	
	x = fb_var.xres;
	y = fb_var.yres;
	nPerPixel = fb_var.bits_per_pixel;
	printf("xres:%d, yres:%d, nBitPerPixel:%d\r\n", x, y, nPerPixel);

	size = (x * y * nPerPixel + 7) >> 3;
	size = (size + nPageSize - 1) / nPageSize * nPageSize;

	printf("memap info :pagesize(%d), mapsize(%d)\r\n", nPageSize, size);
	{
		unsigned char *paddr;
		paddr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fb, 0);
		if(paddr == NULL)
		{
			printf("map error \r\n");
		}
		pViedoInfo->ulBitsPixel = (unsigned long)nPerPixel;

		pViedoInfo->ulResx = (unsigned long)x;
		pViedoInfo->ulResy = (unsigned long)y;
		pViedoInfo->ulWidthBytes = fb_fix.line_length;
		pViedoInfo->pFrameBuf = paddr;
		{			
			printf("map addr:0x%x\r\n", paddr);
//			memset(paddr, 0x88, size / 2);
//			memset(paddr + size / 2, 0x55, size / 2);
//			while(1);
//#define TEST_WRITEBUFF
#ifdef TEST_WRITEBUFF			
			{
				struct timeval tv;

	//printf( "Sys_GetTickCount entry.\r\n" );
				unsigned long ulStartSec, ulTick;
				int i = 0;
				unsigned char colary[] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
				gettimeofday(&tv, NULL);
				ulStartSec = tv.tv_sec;
				ulTick = tv.tv_sec * 1000 + tv.tv_usec/1000;
				while(1)
				{
					memset(paddr, colary[i % sizeof(colary)], size);
					i ++;
					if(i == 10000)
						break;					
				}
				gettimeofday(&tv, NULL);
				ulTick = tv.tv_sec * 1000 + tv.tv_usec/1000 - ulTick;
				printf("############## Total: %d frame, sec:%d, speed %d frame per sec totalTick(%d) #############\r\n", i, tv.tv_sec - ulStartSec, i / (tv.tv_sec - ulStartSec), ulTick);
			}
#endif			
		}
		return 1;
	}
fail:
	return 0;
}

int DeInitFb()
{
	return 1;
}

//extern int Shell_WinMain(int, int, int, int);
extern void InitGWMEKernel( void );

int main(int argc, char *argv[])
{
	printf( "hello, Kingmos gwme(3.0) for linux.\r\n" );

	printf( "build data(%s),time(%s).\r\n", __DATE__, __TIME__ );

	InitGWMEKernel();

	sleep(-1);
	printf("-1 is a incorrect argument\r\n");
	sleep(99999);
	printf("Gwme exit.\r\n");
	return 1;
}
