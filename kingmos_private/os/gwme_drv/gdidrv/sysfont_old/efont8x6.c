#include <eframe.h>
#include <gwmeobj.h>

static const HUGE BYTE eng8x8Mask[]=
{
/*
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
    0X80,0XE0,0XF8,0XFE,0XF8,0XE0,0X80,0X00,
    0X02,0X0E,0X3E,0XFE,0X3E,0X0E,0X02,0X00,
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
    0X00,0X18,0X3C,0X7E,0XFF,0XFF,0X00,0X00,
    0X00,0XFF,0XFF,0X7E,0X3C,0X18,0X00,0X00,
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
*/

    0X30,0X48,0X48,0X58,0X68,0X48,0X30,0X00,   //0
//    0X7C,0XC6,0XCE,0XDE,0XF6,0XE6,0X7C,0X00,
    0X10,0X30,0X10,0X10,0X10,0X10,0X38,0X00,   //1
//    0X30,0X70,0X30,0X30,0X30,0X30,0XFC,0X00,
    0X30,0X48,0X48,0X10,0X20,0X48,0X78,0X00,   //2
//    0X78,0XCC,0X0C,0X38,0X60,0XCC,0XFC,0X00,
    0X30,0X48,0X08,0X10,0X08,0X48,0X30,0X00,   //3
//    0X78,0XCC,0X0C,0X38,0X0C,0XCC,0X78,0X00,
    0X10,0X30,0X50,0X90,0XF8,0X10,0X10,0X00,   //4
//    0X1C,0X3C,0X6C,0XCC,0XFE,0X0C,0X1E,0X00,
    0X78,0X40,0X70,0X08,0X08,0X48,0X30,0X00,   //5
//    0XFC,0XC0,0XF8,0X0C,0X0C,0XCC,0X78,0X00,
    0X10,0X20,0X40,0X70,0X48,0X48,0X30,0X00,   //6
//    0X38,0X60,0XC0,0XF8,0XCC,0XCC,0X78,0X00,
    0X78,0X08,0X10,0X10,0X20,0X20,0X20,0X00,   //7
//    0XFC,0XCC,0X0C,0X18,0X30,0X30,0X30,0X00,
    0X30,0X48,0X48,0X30,0X48,0X48,0X30,0X00,   //8
//    0X78,0XCC,0XCC,0X78,0XCC,0XCC,0X78,0X00,
    0X30,0X48,0X48,0X38,0X08,0X48,0X30,0X00,   //9
//    0X78,0XCC,0XCC,0X7C,0X0C,0X18,0X70,0X00,
    0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,   // blank

/*    0X00,0X30,0X30,0X00,0X00,0X30,0X30,0X00,
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
*/
};

static DWORD ROM8x6_Init(void);
static BOOL ROM8x6_Deinit( DWORD dwData );
static BOOL ROM8x6_InstallFont( DWORD dwData, LPCTSTR lpszPathName );
static HANDLE ROM8x6_CreateFont( DWORD dwData, const LOGFONT *lplf );
static BOOL ROM8x6_DeleteFont( HANDLE );
static int ROM8x6_MaxHeight( HANDLE );
static int ROM8x6_MaxWidth( HANDLE );
static int ROM8x6_WordLength( HANDLE, const BYTE FAR* lpText );
static int ROM8x6_WordHeight( HANDLE, WORD aWord );
static int ROM8x6_WordWidth( HANDLE, WORD aWord );
static int ROM8x6_WordMask( HANDLE, const BYTE FAR* lpText, _LPCHAR_METRICS );
static int ROM8x6_TextWidth( HANDLE, const BYTE FAR* lpText, int aLimiteWidth );
static int ROM8x6_TextHeight( HANDLE, const BYTE FAR* lpText, int aLineWidth );
static const BYTE FAR* ROM8x6_NextWord( HANDLE, const BYTE FAR* lpText );


static void _DeleteObject( void );
static int _Resume();

//    DWORD (*lpInit)(void);
//    void  (*lpDeinit)( DWORD dwData );
//	BOOL (*lpInstallFont)( DWORD dwData, LPCTSTR lpszPathName );
//	HANDLE (*lpCreateFont)( DWORD dwData, CONST LOGFONT *lplf );
//	HANDLE (*lpDeleteFont)( HANDLE );
  //  int (*lpMaxHeight)( HANDLE );
    //int (*lpMaxWidth)( HANDLE );
    //int (*lpWordLength)( HANDLE, const BYTE FAR* lpText );
    //int (*lpWordHeight)( HANDLE, WORD wWord );
    //int (*lpWordWidth)( HANDLE, WORD wWord );
    //int (*lpWordBitmap)( HANDLE, const BYTE FAR* lpText, _LPBITMAPDATA );
    //int (*lpTextWidth)( HANDLE, const BYTE FAR* lpText, int iLimiteWidth );
    //int (*lpTextHeight)( HANDLE, const BYTE FAR* lpText, int iLineWidth );
    //const BYTE FAR* (*lpNextWord)( HANDLE, const BYTE FAR* );

const _FONTDRV FAR _ROM8x6Drv = {
//                      OBJ_FONT,
                      ROM8x6_Init,
                      ROM8x6_Deinit,
	                  ROM8x6_InstallFont,
	                  ROM8x6_CreateFont,
	                  ROM8x6_DeleteFont,
                      ROM8x6_MaxHeight,
                      ROM8x6_MaxWidth,
                      ROM8x6_WordLength,
                      ROM8x6_WordHeight,
                      ROM8x6_WordWidth,
                      ROM8x6_WordMask,
                      ROM8x6_TextWidth,
                      ROM8x6_TextHeight,
                      ROM8x6_NextWord 
};

//const _FONT _ROM8x6Font = { 
//	{ OBJ_FONT | OBJ_FREE_DISABLE, NULL, NULL, NULL, 0 },
//	1, 
//	(HANDLE)1,  
//	(_LPFONTDRV)&_ROM8x6Drv };

//HFONT hEngFont8x6 = (HFONT)&ROM8x6Font;
//const _FONT * _lpROM8x6Font;


#define MK_FARP( seg, ofs ) ( (void * )MAKELONG( ofs, seg ) )

#define IS_CHINESE( lpText ) ( (*(lpText) & 0x80) && ( *((lpText) + 1) & 0x80 ) )
#define IS_TEXT_MARK( v ) ( (v) == TEXT_SOFT_BREAK || (v) == TEXT_KEY_BREAK || (v) == TEXT_EOF )
#define _GET_CHINESE( lpText ) ( (((WORD)*(lpText)) << 8) | *((lpText) + 1) )
#define _GET_ENGLISH( lpText ) ( *(lpText) )
#define FONT_HEIGHT 8
#define FONT_WIDTH 8

// define all function

static void _DeleteObject( void )
{
}

static int _Resume( void )
{
    return 1;
}

static DWORD ROM8x6_Init( void )
{
   //_Resume();

	return 1;
}

static BOOL ROM8x6_Deinit( DWORD dwData )
{
    _DeleteObject();
	return TRUE;
}

BOOL ROM8x6_InstallFont( DWORD dwData, LPCTSTR lpszPathName )
{
	return FALSE;
}

HANDLE ROM8x6_CreateFont( DWORD dwData, const LOGFONT *lplf )
{
	return (HANDLE)1;
}

BOOL ROM8x6_DeleteFont( HANDLE handle )
{
	return TRUE;
}

static int ROM8x6_MaxHeight( HANDLE handle )
{
    return 8;
}

static int ROM8x6_MaxWidth( HANDLE handle )
{
    return 6;
}

static int ROM8x6_WordHeight( HANDLE handle, WORD aWord )
{
    return 8;
}

static int ROM8x6_WordWidth( HANDLE handle, WORD aWord )
{
    return 6;
}

static int ROM8x6_WordMask( HANDLE handle, const BYTE FAR* lpText, _LPCHAR_METRICS lpMask )
{
    int iPosition;
    iPosition=*lpText-'0';
    if (iPosition<0||iPosition>=10)
       iPosition=10; // This is Blank
    iPosition*=8;

    lpMask->bitmap.bmWidth = 6;
    lpMask->bitmap.bmHeight = 8;
    lpMask->bitmap.bmWidthBytes = 1;
    lpMask->bitmap.bmBits = (BYTE *)&eng8x8Mask[iPosition];
	lpMask->xAdvance = 6;
	lpMask->yAdvance = 0;
	lpMask->left = lpMask->top = 0;

    return 1;
}

static int ROM8x6_TextWidth( HANDLE handle, const BYTE FAR* lpText, int len )
{
   int  w = 0;

   if( lpText )
   {
       while( !IS_TEXT_MARK( *lpText ) && *lpText && len )
       {
           w += 6;
           lpText++;
           if( len > 0 )
               len--;
       }
   }
   return w;
}

static int ROM8x6_TextHeight( HANDLE handle, const BYTE FAR* lpText, int aLineWidth )
{
    int h = 0, w = 0;

    if( lpText )
    {
        h = FONT_HEIGHT;
        do
        {
            if( (aLineWidth > 0 && w + 6 >= aLineWidth) ||
                IS_TEXT_MARK( *lpText ) )
            {
                h += 8;
                w = 0;
            }
            else
                w += 6;
            lpText++;
        }while( *lpText );
    }
    return h;
}

const BYTE FAR* ROM8x6_NextWord( HANDLE handle, const BYTE FAR* lpText )
{
    return (lpText + 1);
}

int ROM8x6_WordLength( HANDLE handle, const BYTE FAR* lpText )
{
     int l = 0;
     while( *lpText++ )l++;
     return l;
}
