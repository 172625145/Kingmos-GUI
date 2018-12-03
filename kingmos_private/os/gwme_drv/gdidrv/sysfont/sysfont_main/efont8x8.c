/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/


/*****************************************************
文件说明：8x8 ASCII 字模驱动程序
版本号：1.0.0
开发时期：2000
作者：李林
修改记录：

******************************************************/
#include <eframe.h>
#include <gwmeobj.h>

//字模
static const HUGE BYTE eng8x8Mask[]=
{
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X7E,0X81,0XA5,0X81,0XBD,0X99,0X81,0X7E,
    0X7E,0XFF,0XDB,0XFF,0XC3,0XE7,0XFF,0X7E,
    0X6C,0XFE,0XFE,0XFE,0X7C,0X38,0X10,0X00,
    0X10,0X38,0X7C,0XFE,0X7C,0X38,0X10,0X00,
    0X38,0X7C,0X38,0XFE,0XFE,0X7C,0X38,0X7C,
    0X10,0X10,0X38,0X7C,0XFE,0X7C,0X38,0X7C,
    0X00,0X00,0X18,0X3C,0X3C,0X18,0X00,0X00,
    0XFF,0XFF,0XE7,0XC3,0XC3,0XE7,0XFF,0XFF,
    0X00,0X3C,0X66,0X42,0X42,0X66,0X3C,0X00,
    0XFF,0XC3,0X99,0XBD,0XBD,0X99,0XC3,0XFF,
    0X0F,0X07,0X0F,0X7D,0XCC,0XCC,0XCC,0X78,
    0X3C,0X66,0X66,0X66,0X3C,0X18,0X7E,0X18,
    0X3F,0X33,0X3F,0X30,0X30,0X70,0XF0,0XE0,
    0X7F,0X63,0X7F,0X63,0X63,0X67,0XE6,0XC0,
    0X99,0X5A,0X3C,0XE7,0XE7,0X3C,0X5A,0X99,
//    0X80,0XE0,0XF8,0XFE,0XF8,0XE0,0X80,0X00,
//    0X02,0X0E,0X3E,0XFE,0X3E,0X0E,0X02,0X00,
//    0X80>>2,0Xc0>>2,0Xe0>>2,0XF0>>2,0Xe0>>2,0Xc0>>2,0X80>>2,0X00>>2,
//    0X01<<2,0X03<<2,0X07<<2,0X0f<<2,0X07<<2,0X03<<2,0X01<<2,0X00<<2,
    0x00,0X00,0X10,0X18,0X1c,0X18,0X10,0X00,
    0X00,0X00,0X08,0X18,0X38,0X18,0X08,0X00,

    0X18,0X3C,0X7E,0X18,0X18,0X7E,0X3C,0X18,
    0X66,0X66,0X66,0X66,0X66,0X00,0X66,0X00,
    0X7F,0XDB,0XDB,0X7B,0X1B,0X1B,0X1B,0X00,
    0X3E,0X63,0X38,0X6C,0X6C,0X38,0XCC,0X78,
    0X00,0X00,0X00,0X00,0X7E,0X7E,0X7E,0X00,
    0X18,0X3C,0X7E,0X18,0X7E,0X3C,0X18,0XFF,
    0X18,0X3C,0X7E,0X18,0X18,0X18,0X18,0X00,
    0X18,0X18,0X18,0X18,0X7E,0X3C,0X18,0X00,
    0X00,0X18,0X0C,0XFE,0X0C,0X18,0X00,0X00,
    0X00,0X30,0X60,0XFE,0X60,0X30,0X00,0X00,
    0X00,0X00,0XC0,0XC0,0XC0,0XFE,0X00,0X00,
    0X00,0X24,0X66,0XFF,0X66,0X24,0X00,0X00,
//    0X00,0X18,0X3C,0X7E,0XFF,0XFF,0X00,0X00,
//    0X00,0XFF,0XFF,0X7E,0X3C,0X18,0X00,0X00,
//    0x00,0x00,0x08,0x1c,0x3e,0x7f,0x00,0x00,
//    0x00,0x00,0x7f,0x3e,0x1c,0x08,0x00,0x00,
    0x00,0x00,0x08,0x1c,0x3e,0x00,0x00,0x00,
    0x00,0x00,0x00,0x3e,0x1c,0x08,0x00,0x00,


    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
    0X30,0X78,0X78,0X30,0X30,0X00,0X30,0X00,
    0X6C,0X6C,0X6C,0X00,0X00,0X00,0X00,0X00,
    0X6C,0X6C,0XFE,0X6C,0XFE,0X6C,0X6C,0X00,
    0X30,0X7C,0XC0,0X78,0X0C,0XF8,0X30,0X00,
    0X00,0XC6,0XCC,0X18,0X30,0X66,0XC6,0X00,
    0X38,0X6C,0X38,0X76,0XDC,0XCC,0X76,0X00,
    0X60,0X60,0XC0,0X00,0X00,0X00,0X00,0X00,
    0X18,0X30,0X60,0X60,0X60,0X30,0X18,0X00,
    0X60,0X30,0X18,0X18,0X18,0X30,0X60,0X00,
    0X00,0X66,0X3C,0XFF,0X3C,0X66,0X00,0X00,
    0X00,0X30,0X30,0XFC,0X30,0X30,0X00,0X00,
    0X00,0X00,0X00,0X00,0X00,0X30,0X30,0X60,
    0X00,0X00,0X00,0XFC,0X00,0X00,0X00,0X00,
    0X00,0X00,0X00,0X00,0X00,0X30,0X30,0X00,
    0X06,0X0C,0X18,0X30,0X60,0XC0,0X80,0X00,
    0X7C,0XC6,0XCE,0XDE,0XF6,0XE6,0X7C,0X00,
    0X30,0X70,0X30,0X30,0X30,0X30,0XFC,0X00,
    0X78,0XCC,0X0C,0X38,0X60,0XCC,0XFC,0X00,
    0X78,0XCC,0X0C,0X38,0X0C,0XCC,0X78,0X00,
    0X1C,0X3C,0X6C,0XCC,0XFE,0X0C,0X1E,0X00,
    0XFC,0XC0,0XF8,0X0C,0X0C,0XCC,0X78,0X00,
    0X38,0X60,0XC0,0XF8,0XCC,0XCC,0X78,0X00,
    0XFC,0XCC,0X0C,0X18,0X30,0X30,0X30,0X00,
    0X78,0XCC,0XCC,0X78,0XCC,0XCC,0X78,0X00,
    0X78,0XCC,0XCC,0X7C,0X0C,0X18,0X70,0X00,
    0X00,0X30,0X30,0X00,0X00,0X30,0X30,0X00,
    0X00,0X30,0X30,0X00,0X00,0X30,0X30,0X60,
    0X18,0X30,0X60,0XC0,0X60,0X30,0X18,0X00,
    0X00,0X00,0XFC,0X00,0X00,0XFC,0X00,0X00,
    0X60,0X30,0X18,0X0C,0X18,0X30,0X60,0X00,
    0X78,0XCC,0X0C,0X18,0X30,0X00,0X30,0X00,
    0X7C,0XC6,0XDE,0XDE,0XDE,0XC0,0X78,0X00,
    0X30,0X78,0XCC,0XCC,0XFC,0XCC,0XCC,0X00,
    0XFC,0X66,0X66,0X7C,0X66,0X66,0XFC,0X00,
    0X3C,0X66,0XC0,0XC0,0XC0,0X66,0X3C,0X00,
    0XF8,0X6C,0X66,0X66,0X66,0X6C,0XF8,0X00,
    0XFE,0X62,0X68,0X78,0X68,0X62,0XFE,0X00,
    0XFE,0X62,0X68,0X78,0X68,0X60,0XF0,0X00,
    0X3C,0X66,0XC0,0XC0,0XCE,0X66,0X3E,0X00,
    0XCC,0XCC,0XCC,0XFC,0XCC,0XCC,0XCC,0X00,
    0X78,0X30,0X30,0X30,0X30,0X30,0X78,0X00,
    0X1E,0X0C,0X0C,0X0C,0XCC,0XCC,0X78,0X00,
    0XE6,0X66,0X6C,0X78,0X6C,0X66,0XE6,0X00,
    0XF0,0X60,0X60,0X60,0X62,0X66,0XFE,0X00,
    0XC6,0XEE,0XFE,0XFE,0XD6,0XC6,0XC6,0X00,
    0XC6,0XE6,0XF6,0XDE,0XCE,0XC6,0XC6,0X00,
    0X38,0X6C,0XC6,0XC6,0XC6,0X6C,0X38,0X00,
    0XFC,0X66,0X66,0X7C,0X60,0X60,0XF0,0X00,
    0X78,0XCC,0XCC,0XCC,0XDC,0X78,0X1C,0X00,
    0XFC,0X66,0X66,0X7C,0X6C,0X66,0XE6,0X00,
    0X78,0XCC,0XE0,0X70,0X1C,0XCC,0X78,0X00,
    0XFC,0XB4,0X30,0X30,0X30,0X30,0X78,0X00,
    0XCC,0XCC,0XCC,0XCC,0XCC,0XCC,0XFC,0X00,
    0XCC,0XCC,0XCC,0XCC,0XCC,0X78,0X30,0X00,
    0XC6,0XC6,0XC6,0XD6,0XFE,0XEE,0XC6,0X00,
    0XC6,0XC6,0X6C,0X38,0X38,0X6C,0XC6,0X00,
    0XCC,0XCC,0XCC,0X78,0X30,0X30,0X78,0X00,
    0XFE,0XC6,0X8C,0X18,0X32,0X66,0XFE,0X00,
    0X78,0X60,0X60,0X60,0X60,0X60,0X78,0X00,
    0XC0,0X60,0X30,0X18,0X0C,0X06,0X02,0X00,
    0X78,0X18,0X18,0X18,0X18,0X18,0X78,0X00,
    0X10,0X38,0X6C,0XC6,0X00,0X00,0X00,0X00,
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0XFF,
    0X30,0X30,0X18,0X00,0X00,0X00,0X00,0X00,
    0X00,0X00,0X78,0X0C,0X7C,0XCC,0X76,0X00,
    0XE0,0X60,0X7C,0X6C,0X66,0X66,0XDC,0X00,
    0X00,0X00,0X78,0XCC,0XC0,0XCC,0X78,0X00,
    0X1C,0X0C,0X7C,0XCC,0XCC,0XCC,0X76,0X00,
    0X00,0X00,0X78,0XCC,0XFC,0XC0,0X78,0X00,
    0X1C,0X36,0X30,0X78,0X30,0X30,0X78,0X00,
    0X00,0X00,0X76,0XCC,0XCC,0X7C,0X0C,0XF8,
    0XE0,0X60,0X6C,0X76,0X66,0X66,0XE6,0X00,
    0X30,0X00,0X70,0X30,0X30,0X30,0X78,0X00,
    0X0C,0X00,0X0C,0X0C,0X0C,0XCC,0XCC,0X78,
    0XE0,0X60,0X66,0X6C,0X78,0X6C,0XE6,0X00,
    0X38,0X18,0X18,0X18,0X18,0X18,0X3C,0X00,
    0X00,0X00,0XCC,0XFE,0XFE,0XD6,0XC6,0X00,
    0X00,0X00,0XF8,0XCC,0XCC,0XCC,0XCC,0X00,
    0X00,0X00,0X78,0XCC,0XCC,0XCC,0X78,0X00,
    0X00,0X00,0XDC,0X66,0X66,0X7C,0X60,0XF0,
    0X00,0X00,0X76,0XCC,0XCC,0X7C,0X0C,0X1E,
    0X00,0X00,0XDC,0X76,0X66,0X60,0XF0,0X00,
    0X00,0X00,0X7C,0XC0,0X78,0X0C,0XF8,0X00,
    0X10,0X30,0X7C,0X30,0X30,0X34,0X18,0X00,
    0X00,0X00,0XCC,0XCC,0XCC,0XCC,0X76,0X00,
    0X00,0X00,0XCC,0XCC,0XCC,0X78,0X30,0X00,
    0X00,0X00,0XC6,0XD6,0XFE,0XFE,0X6C,0X00,
    0X00,0X00,0XC6,0X6C,0X38,0X6C,0XC6,0X00,
    0X00,0X00,0XCC,0XCC,0XCC,0X7C,0X0C,0XF8,
    0X00,0X00,0XFC,0X98,0X30,0X64,0XFC,0X00,
    0X1C,0X30,0X30,0XE0,0X30,0X30,0X1C,0X00,
    0X18,0X18,0X18,0X00,0X18,0X18,0X18,0X00,
    0XE0,0X30,0X30,0X1C,0X30,0X30,0XE0,0X00,
    0X76,0XDC,0X00,0X00,0X00,0X00,0X00,0X00,
    0X00,0X10,0X38,0X6C,0XC6,0XC6,0XFE,0X00,
    0X78,0XCC,0XC0,0XCC,0X78,0X18,0X0C,0X78,
    0X00,0XCC,0X00,0XCC,0XCC,0XCC,0X7E,0X00,
    0X1C,0X00,0X78,0XCC,0XFC,0XC0,0X78,0X00,
    0X7E,0XC3,0X3C,0X06,0X3E,0X66,0X3F,0X00,
    0XCC,0X00,0X78,0X0C,0X7C,0XCC,0X7E,0X00,
    0XE0,0X00,0X78,0X0C,0X7C,0XCC,0X7E,0X00,
    0X30,0X30,0X78,0X0C,0X7C,0XCC,0X7E,0X00,
    0X00,0X00,0X78,0XC0,0XC0,0X78,0X0C,0X38,
    0X7E,0XC3,0X3C,0X66,0X7E,0X60,0X3C,0X00,
    0XCC,0X00,0X78,0XCC,0XFC,0XC0,0X78,0X00,
    0XE0,0X00,0X78,0XCC,0XFC,0XC0,0X78,0X00,
    0XCC,0X00,0X70,0X30,0X30,0X30,0X78,0X00,
    0X7C,0XC6,0X38,0X18,0X18,0X18,0X3C,0X00,
    0XE0,0X00,0X70,0X30,0X30,0X30,0X78,0X00,
    0XC6,0X38,0X6C,0XC6,0XFE,0XC6,0XC6,0X00,
    0X30,0X30,0X00,0X78,0XCC,0XFC,0XCC,0X00,
    0X1C,0X00,0XFC,0X60,0X78,0X60,0XFC,0X00,
    0X00,0X00,0X7F,0X0C,0X7F,0XCC,0X7F,0X00,
    0X3E,0X6C,0XCC,0XFE,0XCC,0XCC,0XCE,0X00,
    0X78,0XCC,0X00,0X78,0XCC,0XCC,0X78,0X00,
    0X00,0XCC,0X00,0X78,0XCC,0XCC,0X78,0X00,
    0X00,0XE0,0X00,0X78,0XCC,0XCC,0X78,0X00,
    0X78,0XCC,0X00,0XCC,0XCC,0XCC,0X7E,0X00,
    0X00,0XE0,0X00,0XCC,0XCC,0XCC,0X7E,0X00,
    0X00,0XCC,0X00,0XCC,0XCC,0X7C,0X0C,0XF8,
    0XC3,0X18,0X3C,0X66,0X66,0X3C,0X18,0X00,
    0XCC,0X00,0XCC,0XCC,0XCC,0XCC,0X78,0X00,
    0X18,0X18,0X7E,0XC0,0XC0,0X7E,0X18,0X18,
    0X38,0X6C,0X64,0XF0,0X60,0XE6,0XFC,0X00,
    0XCC,0XCC,0X78,0XFC,0X30,0XFC,0X30,0X30,
    0XF8,0XCC,0XCC,0XFA,0XC6,0XCF,0XC6,0XC7,
    0X0E,0X1B,0X18,0X3C,0X18,0X18,0XD8,0X70,
    0X1C,0X00,0X78,0X0C,0X7C,0XCC,0X7E,0X00,
    0X38,0X00,0X70,0X30,0X30,0X30,0X78,0X00,
    0X00,0X1C,0X00,0X78,0XCC,0XCC,0X78,0X00,
    0X00,0X1C,0X00,0XCC,0XCC,0XCC,0X7E,0X00,
    0X00,0XF8,0X00,0XF8,0XCC,0XCC,0XCC,0X00,
    0XFC,0X00,0XCC,0XEC,0XFC,0XDC,0XCC,0X00,
    0X3C,0X6C,0X6C,0X3E,0X00,0X7E,0X00,0X00,
    0X38,0X6C,0X6C,0X38,0X00,0X7C,0X00,0X00,
    0X30,0X00,0X30,0X60,0XC0,0XCC,0X78,0X00,
    0X00,0X00,0X00,0XFC,0XC0,0XC0,0X00,0X00,
    0X00,0X00,0X00,0XFC,0X0C,0X0C,0X00,0X00,
    0XC3,0XC6,0XCC,0XDE,0X33,0X66,0XCC,0X0F,
    0XC3,0XC6,0XCC,0XDB,0X37,0X6F,0XCF,0X03,
    0X18,0X18,0X00,0X18,0X18,0X18,0X18,0X00,
    0X00,0X33,0X66,0XCC,0X66,0X33,0X00,0X00,
    0X00,0XCC,0X66,0X33,0X66,0XCC,0X00,0X00,
    0X22,0X88,0X22,0X88,0X22,0X88,0X22,0X88,
    0X55,0XAA,0X55,0XAA,0X55,0XAA,0X55,0XAA,
    0XDB,0X77,0XDB,0XEE,0XDB,0X77,0XDB,0XEE,
    0X18,0X18,0X18,0X18,0X18,0X18,0X18,0X18,
    0X18,0X18,0X18,0X18,0XF8,0X18,0X18,0X18,
    0X18,0X18,0XF8,0X18,0XF8,0X18,0X18,0X18,
    0X36,0X36,0X36,0X36,0XF6,0X36,0X36,0X36,
    0X00,0X00,0X00,0X00,0XFE,0X36,0X36,0X36,
    0X00,0X00,0XF8,0X18,0XF8,0X18,0X18,0X18,
    0X36,0X36,0XF6,0X06,0XF6,0X36,0X36,0X36,
    0X36,0X36,0X36,0X36,0X36,0X36,0X36,0X36,
    0X00,0X00,0XFE,0X06,0XF6,0X36,0X36,0X36,
    0X36,0X36,0XF6,0X06,0XFE,0X00,0X00,0X00,
    0X36,0X36,0X36,0X36,0XFE,0X00,0X00,0X00,
    0X18,0X18,0XF8,0X18,0XF8,0X00,0X00,0X00,
    0X00,0X00,0X00,0X00,0XF8,0X18,0X18,0X18,
    0X18,0X18,0X18,0X18,0X1F,0X00,0X00,0X00,
    0X18,0X18,0X18,0X18,0XFF,0X00,0X00,0X00,
    0X00,0X00,0X00,0X00,0XFF,0X18,0X18,0X18,
    0X18,0X18,0X18,0X18,0X1F,0X18,0X18,0X18,
    0X00,0X00,0X00,0X00,0XFF,0X00,0X00,0X00,
    0X18,0X18,0X18,0X18,0XFF,0X18,0X18,0X18,
    0X18,0X18,0X1F,0X18,0X1F,0X18,0X18,0X18,
    0X36,0X36,0X36,0X36,0X37,0X36,0X36,0X36,
    0X36,0X36,0X37,0X30,0X3F,0X00,0X00,0X00,
    0X00,0X00,0X3F,0X30,0X37,0X36,0X36,0X36,
    0X36,0X36,0XF7,0X00,0XFF,0X00,0X00,0X00,
    0X00,0X00,0XFF,0X00,0XF7,0X36,0X36,0X36,
    0X36,0X36,0X37,0X30,0X37,0X36,0X36,0X36,
    0X00,0X00,0XFF,0X00,0XFF,0X00,0X00,0X00,
    0X36,0X36,0XF7,0X00,0XF7,0X36,0X36,0X36,
    0X18,0X18,0XFF,0X00,0XFF,0X00,0X00,0X00,
    0X36,0X36,0X36,0X36,0XFF,0X00,0X00,0X00,
    0X00,0X00,0XFF,0X00,0XFF,0X18,0X18,0X18,
    0X00,0X00,0X00,0X00,0XFF,0X36,0X36,0X36,
    0X36,0X36,0X36,0X36,0X3F,0X00,0X00,0X00,
    0X18,0X18,0X1F,0X18,0X1F,0X00,0X00,0X00,
    0X00,0X00,0X1F,0X18,0X1F,0X18,0X18,0X18,
    0X00,0X00,0X00,0X00,0X3F,0X36,0X36,0X36,
    0X36,0X36,0X36,0X36,0XFF,0X36,0X36,0X36,
    0X18,0X18,0XFF,0X18,0XFF,0X18,0X18,0X18,
    0X18,0X18,0X18,0X18,0XF8,0X00,0X00,0X00,
    0X00,0X00,0X00,0X00,0X1F,0X18,0X18,0X18,
    0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
    0X00,0X00,0X00,0X00,0XFF,0XFF,0XFF,0XFF,
    0XF0,0XF0,0XF0,0XF0,0XF0,0XF0,0XF0,0XF0,
    0X0F,0X0F,0X0F,0X0F,0X0F,0X0F,0X0F,0X0F,
    0XFF,0XFF,0XFF,0XFF,0X00,0X00,0X00,0X00,
    0X00,0X00,0X76,0XDC,0XC8,0XDC,0X76,0X00,
    0X00,0X78,0XCC,0XF8,0XCC,0XF8,0XC0,0XC0,
    0X00,0XFC,0XCC,0XC0,0XC0,0XC0,0XC0,0X00,
    0X00,0XFE,0X6C,0X6C,0X6C,0X6C,0X6C,0X00,
    0XFC,0XCC,0X60,0X30,0X60,0XCC,0XFC,0X00,
    0X00,0X00,0X7E,0XD8,0XD8,0XD8,0X70,0X00,
    0X00,0X66,0X66,0X66,0X66,0X7C,0X60,0XC0,
    0X00,0X76,0XDC,0X18,0X18,0X18,0X18,0X00,
    0XFC,0X30,0X78,0XCC,0XCC,0X78,0X30,0XFC,
    0X38,0X6C,0XC6,0XFE,0XC6,0X6C,0X38,0X00,
    0X38,0X6C,0XC6,0XC6,0X6C,0X6C,0XEE,0X00,
    0X1C,0X30,0X18,0X7C,0XCC,0XCC,0X78,0X00,
    0X00,0X00,0X7E,0XDB,0XDB,0X7E,0X00,0X00,
    0X06,0X0C,0X7E,0XDB,0XDB,0X7E,0X60,0XC0,
    0X38,0X60,0XC0,0XF8,0XC0,0X60,0X38,0X00,
    0X78,0XCC,0XCC,0XCC,0XCC,0XCC,0XCC,0X00,
    0X00,0XFC,0X00,0XFC,0X00,0XFC,0X00,0X00,
    0X30,0X30,0XFC,0X30,0X30,0X00,0XFC,0X00,
    0X60,0X30,0X18,0X30,0X60,0X00,0XFC,0X00,
    0X18,0X30,0X60,0X30,0X18,0X00,0XFC,0X00,
    0X0E,0X1B,0X1B,0X18,0X18,0X18,0X18,0X18,
    0X18,0X18,0X18,0X18,0X18,0XD8,0XD8,0X70,
    0X30,0X30,0X00,0XFC,0X00,0X30,0X30,0X00,
    0X00,0X76,0XDC,0X00,0X76,0XDC,0X00,0X00,
    0X38,0X6C,0X6C,0X38,0X00,0X00,0X00,0X00,
    0X00,0X00,0X00,0X18,0X18,0X00,0X00,0X00,
    0X00,0X00,0X00,0X00,0X18,0X00,0X00,0X00,
    0X0F,0X0C,0X0C,0X0C,0XEC,0X6C,0X3C,0X1C,
    0X78,0X6C,0X6C,0X6C,0X6C,0X00,0X00,0X00,
    0X70,0X18,0X30,0X60,0X78,0X00,0X00,0X00,
    0X00,0X00,0X3C,0X3C,0X3C,0X3C,0X00,0X00,
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00
};

static DWORD ROM8x8_Init(void);
static BOOL ROM8x8_Deinit( DWORD dwData );
static BOOL ROM8x8_InstallFont( DWORD dwData, LPCTSTR lpszPathName );
static HANDLE ROM8x8_CreateFont( DWORD dwData, const LOGFONT *lplf );
static BOOL ROM8x8_DeleteFont( HANDLE );
static int ROM8x8_MaxHeight( HANDLE );
static int ROM8x8_MaxWidth( HANDLE );
static int ROM8x8_WordLength( HANDLE, const BYTE FAR* lpText );
static int ROM8x8_WordHeight( HANDLE, WORD aWord );
static int ROM8x8_WordWidth( HANDLE, WORD aWord );
static int ROM8x8_WordMask( HANDLE, const BYTE FAR* lpText, UINT len, _LPCHAR_METRICS );
static int ROM8x8_TextWidth( HANDLE, const BYTE FAR* lpText, int aLimiteWidth );
static int ROM8x8_TextHeight( HANDLE, const BYTE FAR* lpText, int aLineWidth );
static const BYTE FAR* ROM8x8_NextWord( HANDLE, const BYTE FAR* lpText );

//	驱动程序接口函数
const _FONTDRV FAR _ROM8x8Drv = {
                      ROM8x8_Init,
                      ROM8x8_Deinit,
	                  ROM8x8_InstallFont,
	                  ROM8x8_CreateFont,
	                  ROM8x8_DeleteFont,
                      ROM8x8_MaxHeight,
                      ROM8x8_MaxWidth,
                      ROM8x8_WordLength,
                      ROM8x8_WordHeight,
                      ROM8x8_WordWidth,
                      ROM8x8_WordMask,
                      ROM8x8_TextWidth,
                      ROM8x8_TextHeight,
                      ROM8x8_NextWord 
};

#define IS_TEXT_MARK( v ) ( (v) == TEXT_SOFT_BREAK || (v) == TEXT_KEY_BREAK || (v) == TEXT_EOF )
#define FONT_HEIGHT 8
#define FONT_WIDTH 8

// define all function

// *****************************************************************
// 声明：static DWORD ROM8x8_Init( void )
// 参数：
//	无
// 返回值：
//	假如成功，返回对象句柄；否则，返回NULL
// 功能描述：
//	初始化字体驱动程序
// 引用: 
//	驱动程序接口函数
// *****************************************************************
static DWORD ROM8x8_Init( void )
{
	return 1;
}

// *****************************************************************
// 声明：static BOOL ROM8x8_Deinit( DWORD dwData )
// 参数：
//	IN dwData - 由ROM8x8_Init返回的对象句柄
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	释放对象
// 引用: 
//	驱动程序接口函数
// *****************************************************************

static BOOL ROM8x8_Deinit( DWORD dwData )
{
	return TRUE;
}

// *****************************************************************
// 声明：BOOL ROM8x8_InstallFont( DWORD dwData, LPCTSTR lpszPathName )
// 参数：
//	IN dwData - 由ROM8x8_Init返回的对象句柄
//	IN lpszPathName - 文件路径名（字体文件）
// 返回值：
//	假如字体文件被该驱动程序识别，返回TRUE; 否则，返回FALSE
// 功能描述：
//	安装一个字体
// 引用: 
//	驱动程序接口函数
// *****************************************************************

BOOL ROM8x8_InstallFont( DWORD dwData, LPCTSTR lpszPathName )
{
	return FALSE;
}

// *****************************************************************
// 声明：HANDLE ROM8x8_CreateFont( DWORD dwData, const LOGFONT *lplf )
// 参数：
//	IN dwData - 由ROM8x8_Init返回的对象句柄
//	IN lplf - LOGFONT 结构指针，包含逻辑字体属性
// 返回值：
//	假如成功，返回打开的逻辑字体对象句柄
// 功能描述：
//	创建逻辑字体对象
// 引用: 
//	驱动程序接口函数
// *****************************************************************

HANDLE ROM8x8_CreateFont( DWORD dwData, const LOGFONT *lplf )
{
	return (HANDLE)1;
}

// *****************************************************************
// 声明：BOOL ROM8x8_DeleteFont( HANDLE handle )
// 参数：
//	IN handle - 由 ROM8x8_CreateFont 返回的逻辑字体对象句柄
// 返回值：
//	假如成功，返回TRUE; 否则，返回FALSE
// 功能描述：
//	删除逻辑字体对象
// 引用: 
//	驱动程序接口函数
// *****************************************************************

BOOL ROM8x8_DeleteFont( HANDLE handle )
{
	return TRUE;
}

// *****************************************************************
// 声明：static int ROM8x8_MaxHeight( HANDLE handle )
// 参数：
//	IN handle - 由 ROM8x8_CreateFont 返回的逻辑字体对象句柄
// 返回值：
//	返回逻辑字体的高度
// 功能描述：
//	得到逻辑字体的高度
// 引用: 
//	驱动程序接口函数
// *****************************************************************

static int ROM8x8_MaxHeight( HANDLE handle )
{
    return 8;
}

// *****************************************************************
// 声明：static int ROM8x8_MaxWidth( HANDLE handle )
// 参数：
//	IN handle - 由 ROM8x8_CreateFont 返回的逻辑字体对象句柄
// 返回值：
//	返回逻辑字体的宽度
// 功能描述：
//	得到逻辑字体的宽度
// 引用: 
//	驱动程序接口函数
// *****************************************************************

static int ROM8x8_MaxWidth( HANDLE handle )
{
    return 8;
}

// *****************************************************************
// 声明：static int ROM8x8_WordHeight( HANDLE handle, WORD aWord )
// 参数：
//	IN handle - 由 ROM8x8_CreateFont 返回的逻辑字体对象句柄
//	IN aWord - 字符代码
// 返回值：
//	返回字符的高度
// 功能描述：
//	得到字符的高度
// 引用: 
//	驱动程序接口函数
// *****************************************************************

static int ROM8x8_WordHeight( HANDLE handle, WORD aWord )
{
    return 8;
}

// *****************************************************************
// 声明：static int ROM8x8_WordWidth( HANDLE handle, WORD aWord )
// 参数：
//	IN handle - 由 ROM8x8_CreateFont 返回的逻辑字体对象句柄
//	IN aWord - 字符代码
// 返回值：
//	返回字符的宽度
// 功能描述：
//	得到字符的宽度
// 引用: 
//	驱动程序接口函数
// *****************************************************************

static int ROM8x8_WordWidth( HANDLE handle, WORD aWord )
{
    return 8;
}

// *****************************************************************
// 声明：static int ROM8x8_WordMask( HANDLE handle, const BYTE FAR* lpText, _LPCHAR_METRICS lpMask )
// 参数：
//	IN handle - 由 ROM8x8_CreateFont 返回的逻辑字体对象句柄
//	IN lpText - 文本指针
//	OUT lpMask - 用于接受字模的结构指针
// 返回值：
//	返回字符代码的字节数（宽字符为2），lpMask包含该字符的长宽和字模地址
// 功能描述：
//	得到字符代码的字节数（宽字符为2）和字符的长宽和字模地址
// 引用: 
//	驱动程序接口函数
// *****************************************************************

static int ROM8x8_WordMask( HANDLE handle, const BYTE FAR* lpText, UINT len, _LPCHAR_METRICS lpMask )
{
    lpMask->bitmap.bmWidth = 8;
    lpMask->bitmap.bmHeight = 8;
    lpMask->bitmap.bmWidthBytes = 1;
    lpMask->bitmap.bmBits = (LPBYTE)&eng8x8Mask[(*lpText) << 3];

	lpMask->xAdvance = 8;
	lpMask->yAdvance = 0;
	lpMask->left = lpMask->top = 0;
    return 1;
}

// *****************************************************************
// 声明：static int ROM8x8_TextWidth( HANDLE handle, const BYTE FAR* lpText, int len )
// 参数：
//	IN handle - 由 ROM8x8_CreateFont 返回的逻辑字体对象句柄
//	IN lpText - 文本指针
//	IN len - 需要统计的文本长度
// 返回值：
//	文本的象素长度
// 功能描述：
//	得到文本的象素长度，如果文本长度（len）中间包含换行字符，则统计到换行字符位置为止。
// 引用: 
//	驱动程序接口函数
// *****************************************************************

static int ROM8x8_TextWidth( HANDLE handle, const BYTE FAR* lpText, int len )
{
   int  w = 0;

   if( lpText )
   {
       while( !IS_TEXT_MARK( *lpText ) && *lpText && len )
       {
           w += 8;
           lpText++;
           if( len > 0 )
               len--;
       }
   }
   return w;
}

// *****************************************************************
// 声明：static int ROM8x8_TextHeight( HANDLE handle, const BYTE FAR* lpText, int aLineWidth )
// 参数：
//	IN handle - 由 ROM8x8_CreateFont 返回的逻辑字体对象句柄
//	IN lpText - 文本指针
//	IN aLineWidth - 需要统计的象素最大宽度，如果为0，则没有宽度限制
// 返回值：
//	文本的象素高度
// 功能描述：
//	得到文本的象素高度，如果文本象素长度 大于等于aLineWidth则增加字体高度；
//	如果文本中间包含换行字符，则增加字体高度。
//	如果aLineWidth为0，则没有宽度限制，以换行字符确定下一行
// 引用: 
//	驱动程序接口函数
// *****************************************************************

static int ROM8x8_TextHeight( HANDLE handle, const BYTE FAR* lpText, int aLineWidth )
{
    int h = 0, w = 0;

    if( lpText )
    {
        h = FONT_HEIGHT;
        do
        {
            if( (aLineWidth > 0 && w + 8 >= aLineWidth) ||
                IS_TEXT_MARK( *lpText ) )
            {	//换行或宽度限制已到
                h += 8;
                w = 0;
            }
            else
                w += 8;
            lpText++;
        }while( *lpText );
    }
    return h;
}

// *****************************************************************
// 声明：const BYTE FAR* ROM8x8_NextWord( HANDLE handle, const BYTE FAR* lpText )
// 参数：
//	IN handle - 由 ROM8x8_CreateFont 返回的逻辑字体对象句柄
//	IN lpText - 文本指针
// 返回值：
//	文本的下一个字符地址指针
// 功能描述：
//	得到文本的下一个字符地址指针
// 引用: 
//	驱动程序接口函数
// *****************************************************************

const BYTE FAR* ROM8x8_NextWord( HANDLE handle, const BYTE FAR* lpText )
{
    return (lpText + 1);
}

// *****************************************************************
// 声明：int ROM8x8_WordLength( HANDLE handle, const BYTE FAR* lpText )
// 参数：
//	IN handle - 由 ROM8x8_CreateFont 返回的逻辑字体对象句柄
//	IN lpText - 文本指针
// 返回值：
//	文本中以字符为单位的数量
// 功能描述：
//	得到文本中以字符为单位的数量
// 引用: 
//	驱动程序接口函数
// *****************************************************************

int ROM8x8_WordLength( HANDLE handle, const BYTE FAR* lpText )
{
     int l = 0;
     while( *lpText++ )l++;
     return l;
}
