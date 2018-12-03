/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：图形设备驱动程序- 1bit / pixel 格式
版本号：2.0.0
开发时期：2000
作者：李林
修改记录：

******************************************************/

#include <eframe.h>
#include <estring.h>
#include <eassert.h>
#include <gwmeobj.h>

//#define COLOR_1BPP
//#ifndef COLOR_1BPP
//#error not define COLOR_1BPP in file eversion.h
///#endif

static COLORREF _PutPixel( _LPPIXELDATA lpPixelData );
static COLORREF _GetPixel( _LPPIXELDATA lpPixelData );
static BOOL _Line( _LPLINEDATA lpData );
static BOOL _BlkBitTransparentBlt( _LPBLKBITBLT lpData );
static BOOL _BlkBitMaskBlt( _LPBLKBITBLT lpData );
static BOOL _BlkBitBlt( _LPBLKBITBLT lpData );
static COLORREF _RealizeColor( COLORREF color, LPCDWORD lpcdwPal, UINT uPalNumber, UINT uPalFormat );
static COLORREF _UnrealizeColor( COLORREF color, LPCDWORD lpcdwPal, UINT uPalNumber, UINT uPalFormat );

//申明图形设备驱动程序接口对象
const _DISPLAYDRV _drvDisplay1BPP = {
    _PutPixel,
    _GetPixel,
    _Line,
    _BlkBitTransparentBlt,
    _BlkBitMaskBlt,
    _BlkBitBlt,
    _RealizeColor,
    _UnrealizeColor
};

//申明默认调色板
#define PALETTE_SIZE 2
static const PALETTEENTRY _rgbIdentity[PALETTE_SIZE] =                                               \
{                                                                                       \
    { 0x00, 0x00, 0x00, 0 },    /* 0 Sys Black      gray 0  */                          \
    { 0xff, 0xff, 0xff, 0 },    /* 1 Sys Dk Red  */                                     \
};																	   

//
static const BYTE leftFillMask[8]=
{
    0xff,
    0x7f,
    0x3f,
    0x1f,
    0x0f,
    0x07,
    0x03,
    0x01,
};

static const BYTE rightFillMask[8]=
{
    0xff,
    0x80,
    0xc0,
    0xe0,
    0xf0,
    0xf8,
    0xfc,
    0xfe
};

static const BYTE patternMask[8]=
{
    0x80,
    0x40,
    0x20,
    0x10,
    0x08,
    0x04,
    0x02,
    0x01
};

// *****************************************************************
//声明：static COLORREF _RealizeColor( COLORREF color, LPCDWORD lpcdwPal, UINT uPalNumber, UINT uPalFormat )
//参数：
//	IN color - RGB 颜色值
//返回值：
//	设备相关的颜色值
//功能描述：
//	由RGB颜色值得到设备相关的颜色值
//引用: 
//	
// *****************************************************************
static COLORREF _RealizeColor( COLORREF color, LPCDWORD lpcdwPal, UINT uPalNumber, UINT uPalFormat )
{
	if( lpcdwPal )
	{
		return _Gdi_RealizeColor( color, lpcdwPal, uPalNumber, uPalFormat );
	}
	else
	{		
		int like = 0;
		
		BYTE r = (BYTE)color;	//红
		BYTE g = (BYTE)(color >> 8);	//绿
		BYTE b = (BYTE)(color >> 16);	//蓝
		BYTE gray;
		//得到灰度值
		gray = (BYTE) ( (r * 30l + g * 59l + b * 11l) / 100l );
		if( gray < 0x80 )
			return 0; // black
		else
			return 1; // white;
	}
}

// *****************************************************************
//声明：static COLORREF _UnrealizeColor( COLORREF color, LPCDWORD lpcdwPal, UINT uPalNumber, UINT uPalFormat )
//参数：
//	IN color - 设备相关颜色值
//返回值：
//	设备无关颜色值
//功能描述：
//	由设备相关颜色值得到设备无关的RGB值
//引用: 
//	
// *****************************************************************

static COLORREF _UnrealizeColor( COLORREF color, LPCDWORD lpcdwPal, UINT uPalNumber, UINT uPalFormat )
{
	if( lpcdwPal )
	{
		return _Gdi_UnrealizeColor( color, lpcdwPal, uPalNumber, uPalFormat );
	}
	else
	{
		return *((COLORREF*)&_rgbIdentity[(BYTE)color&0x01]);
	}
}

// *****************************************************************
//声明：static COLORREF _PutPixel( _LPPIXELDATA lpPixelData )
//参数：
//	IN lpPixelData - _PIXELDATA结构指针，包含写点需要的数据
//返回值：
//	该点对应的颜色值
//功能描述：
//	在对应的xy点输出颜色值，并返回之前的值
//引用: 
//	
// *****************************************************************

static COLORREF _PutPixel( _LPPIXELDATA lpPixelData )
{
    LPBYTE lpVideoAdr;
    COLORREF c;
	UINT rop;
	//是否需要真实输出 ？
	if( (rop = lpPixelData->rop) != R2_NOP )
	{	//需要输出
		//得到x bit位置（在一个byte上的）
        BYTE xbit = patternMask[lpPixelData->x & 0x07];
		//得到输出显示面的内存位置
        lpVideoAdr = lpPixelData->lpDestImage->bmBits + lpPixelData->y * lpPixelData->lpDestImage->bmWidthBytes + (lpPixelData->x >> 3);
		//得到当前的颜色值
		c = *lpVideoAdr & xbit;
		//根据操作模式做相关动作
	    switch( rop )
		{
		case R2_COPYPEN:		//拷贝到目标
			if( lpPixelData->color )
				*lpVideoAdr |= xbit;
			else
				*lpVideoAdr &= ~xbit;
			break;
		case R2_XORPEN:			//异或
			if( lpPixelData->color )
				*lpVideoAdr ^= xbit;
			break;
		case R2_NOT:			//非
			*lpVideoAdr ^= xbit;
			break;
		case R2_BLACK:			//黑
			*lpVideoAdr &= ~xbit;//0;
			break;
		case R2_WHITE:			//白
			*lpVideoAdr |= xbit;//0xffff;
			break;
		case R2_MASKPEN:		// dest = dest-pixel-color and pen-color
			if( lpPixelData->color )
			    *lpVideoAdr &= 0xff;
			else
				*lpVideoAdr &= ~xbit;
			break;
		case R2_MERGEPEN:		// dest = dest-pixel-color or pen-color
			if( lpPixelData->color )
			    *lpVideoAdr |= xbit;
			break;
		case R2_MERGENOTPEN:	//
			if( !lpPixelData->color )  //dest |= ~src
			    *lpVideoAdr |= xbit;
			break;
		case R2_NOTCOPYPEN:              // dest = ~src
			if( !lpPixelData->color )
				*lpVideoAdr |= xbit;
			else
				*lpVideoAdr &= ~xbit;
			break;
		case R2_NOTXORPEN:
			if( lpPixelData->color )
			{
				;//*lpVideoAdr = ( *lpVideoAdr ^ xbit ) ^ xbit;
			}
			else
			{
				*lpVideoAdr ^= xbit;
			}
			break;
		}
	}
	else
		c = -1;
    return c;
}

// *****************************************************************
//声明：static COLORREF _GetPixel( _LPPIXELDATA lpPixelData )
//参数：
//	IN lpPixelData - _PIXELDATA结构指针，包含该点需要的数据
//返回值：
//	该点对应的颜色值
//功能描述：
//	得到对应的xy点的颜色值
//引用: 
//	
// *****************************************************************

static COLORREF _GetPixel( _LPPIXELDATA lpPixelData )
{
    LPBYTE lpVideoAdr;
    BYTE xbit = patternMask[lpPixelData->x & 0x07];

    lpVideoAdr = lpPixelData->lpDestImage->bmBits + lpPixelData->y * lpPixelData->lpDestImage->bmWidthBytes + (lpPixelData->x >> 3);
    return (*lpVideoAdr & xbit) ? 1 : 0;
}

// *****************************************************************
//声明：static BOOL __VertialSolidLine( _LPLINEDATA lpLineData, 
//										int x0, 
//										int y0, 
//										int yDir, 
//										unsigned int uLen )
//参数：
//	IN lpLineData - _LINEDATA结构指针，包含画线需要的数据
//	IN x0 - 起点 x 坐标
//	IN y0 - 起点 y 坐标
//	IN yDir - 方向（当 < 0 , 由下向上）
//	IN uLen - 点数
//返回值：
//	返回TRUE
//功能描述：
//	用纯色画垂直线
//引用: 
//	被 line.h 使用
// *****************************************************************

static BOOL __VertialSolidLine( _LPLINEDATA lpLineData, 
							   int x0, 
							   int y0, 
							   int yDir, 
							   unsigned int uLen )
{
    register LPBYTE lpVideoAdr;
    register int sum, widthBytes;
    register BYTE color, xbit;
	//扫描行宽度
    widthBytes = lpLineData->lpDestImage->bmWidthBytes;
	//显存位置
	lpVideoAdr = lpLineData->lpDestImage->bmBits + y0 * widthBytes + (x0 >> 3);

	if( yDir < 0 )
        widthBytes = -widthBytes;
    if( lpLineData->color )
        color = 0xff;
    else
        color = 0;
    //xbit = 0x80 >> (lpLineData->x0 & 0x07);
	xbit = 0x80 >> (x0 & 0x07);
    color &= xbit;

    sum = uLen;//lpLineData->sum;
	//根据操作模式做不同的处理
    if( lpLineData->rop == R2_COPYPEN )
    {
        if( lpLineData->color )
        {
            while( sum-- )
            {
                *lpVideoAdr |= color;
                lpVideoAdr += widthBytes;
            }
        }
        else
        {
            xbit = ~xbit;
            while( sum-- )
            {
                *lpVideoAdr &= xbit;
                lpVideoAdr += widthBytes;
            }
        }
    }
    else if( lpLineData->rop == R2_NOT )
    {
        while( sum-- )
        {
            *lpVideoAdr = *lpVideoAdr ^ xbit;
            lpVideoAdr += widthBytes;
        }
    }
    else if( lpLineData->rop == R2_XORPEN )
    {
        while( sum-- )
        {
            *lpVideoAdr = *lpVideoAdr ^ color;
            lpVideoAdr += widthBytes;
        }
    }
    return TRUE;
}

// *****************************************************************
//声明：static BOOL __VertialPatternLine( _LPLINEDATA lpLineData, 
//										int x0, 
//										int y0, 
//										int yDir, 
//										unsigned int uLen )
//参数：
//	IN lpLineData - _LINEDATA结构指针，包含画线需要的数据
//	IN x0 - 起点 x 坐标
//	IN y0 - 起点 y 坐标
//	IN yDir - 方向（当 < 0 , 由下向上）
//	IN uLen - 点数
//返回值：
//	返回TRUE
//功能描述：
//	带模板画垂直线
//引用: 
//	被 line.h 使用
// *****************************************************************

static BOOL __VertialPatternLine( _LPLINEDATA lpLineData, int x0, int y0, int yDir, unsigned int uLen )
{
    register LPBYTE lpVideoAdr;
    register int sum, widthBytes;
    register WORD clrFore, clrBack;
    register BYTE pattern, xbit, ixbit;

    widthBytes = lpLineData->lpDestImage->bmWidthBytes;
	lpVideoAdr = lpLineData->lpDestImage->bmBits + y0 * widthBytes + (x0 >> 3);

	if( yDir < 0 )
		widthBytes = -widthBytes;

    if( lpLineData->color )
        clrFore = 0xffff;
    else
        clrFore = 0;
    if( lpLineData->clrBack )
        clrBack = 0xffff;
    else
        clrBack = 0;

	xbit = 0x80 >> (x0 & 0x07);
    ixbit = ~xbit;
	//模板
    pattern = lpLineData->pattern;
	sum = uLen;
	//根据操作模式做不同的处理
    if( lpLineData->rop == R2_COPYPEN )
    {
        while( sum-- )
        {
            if( pattern & patternMask[y0&0x07] )
            {
                if( clrFore )
                    *lpVideoAdr |= xbit;
                else
                    *lpVideoAdr &= ixbit;
            }
            else
            {
                if( clrBack )
                    *lpVideoAdr |= xbit;
                else
                    *lpVideoAdr &= ixbit;
            }
            lpVideoAdr += widthBytes;
            y0 += yDir;//dir;
        }
    }
    else if( lpLineData->rop == R2_XORPEN )
    {
        while( sum-- )
        {
            if( pattern & patternMask[y0&0x07] )
            {
                if( clrFore )
                    *lpVideoAdr ^= xbit;
            }
            else
            {
                if( clrBack )
                    *lpVideoAdr ^= xbit;
            }
            lpVideoAdr += widthBytes;
            y0 += yDir;//dir;
        }
    }
    else if( lpLineData->rop == R2_NOT )
    {
        while( sum-- )
        {
            *lpVideoAdr ^= xbit;
            lpVideoAdr += widthBytes;
        }
    }
    return TRUE;
}

// *****************************************************************
//声明：static BOOL __VertialTransparentLine( _LPLINEDATA lpLineData, 
//										int x0, 
//										int y0, 
//										int yDir, 
//										unsigned int uLen )
//参数：
//	IN lpLineData - _LINEDATA结构指针，包含画线需要的数据
//	IN x0 - 起点 x 坐标
//	IN y0 - 起点 y 坐标
//	IN yDir - 方向（当 < 0 , 由下向上）
//	IN uLen - 点数
//返回值：
//	返回TRUE
//功能描述：
//	带模板画透明垂直线（即当模板对应位为1时，输出；为0时，不输出）
//引用: 
//	被 line.h 使用
// *****************************************************************

static BOOL __VertialTransparentLine( _LPLINEDATA lpLineData, int x0, int y0, int yDir, unsigned int uLen )
{
    register LPBYTE lpVideoAdr;
    register int sum, widthBytes;
    register WORD clrFore;
    register BYTE pattern, xbit, ixbit;

    widthBytes = lpLineData->lpDestImage->bmWidthBytes;
	lpVideoAdr = lpLineData->lpDestImage->bmBits + y0 * widthBytes + (x0 >> 3);
	if( yDir < 0 )
		widthBytes = -widthBytes;

    if( lpLineData->color )
        clrFore = 0xffff;
    else
        clrFore = 0;

	xbit = 0x80 >> (x0 & 0x07);
    ixbit = ~xbit;
    pattern = lpLineData->pattern;

    sum = uLen;//lpLineData->sum;
    if( lpLineData->rop == R2_COPYPEN )
    {
        if( clrFore )
        {
            while( sum-- )
            {
                if( pattern & patternMask[y0&0x07] )
                    *lpVideoAdr |= xbit;
                lpVideoAdr += widthBytes;
                y0 += yDir;//dir;
            }
        }
        else
        {
            while( sum-- )
            {
                if( pattern & patternMask[y0&0x07] )
                    *lpVideoAdr &= ixbit;
                lpVideoAdr += widthBytes;
                y0 += yDir;//dir;
            }
        }
    }
    else if( lpLineData->rop == R2_XORPEN )
    {
        if( clrFore )
        {
            while( sum-- )
            {
                if( pattern & patternMask[y0&0x07] )
                    *lpVideoAdr ^= xbit;
                lpVideoAdr += widthBytes;
                y0 += yDir;//dir;
            }
        }
    }
    else if( lpLineData->rop == R2_NOT )
    {
        while( sum-- )
        {
            *lpVideoAdr ^= xbit;
            lpVideoAdr += widthBytes;
        }
    }
    return TRUE;
}

// *****************************************************************
//声明：static BOOL __ScanSolidLine( _LPLINEDATA lpLineData, 
//										int x0, 
//										int y0, 
//										int xDir, 
//										unsigned int uLen )
//参数：
//	IN lpLineData - _LINEDATA结构指针，包含画线需要的数据
//	IN x0 - 起点 x 坐标
//	IN y0 - 起点 y 坐标
//	IN xDir - 方向（当 < 0 , 由右向左）
//	IN uLen - 点数
//返回值：
//	返回TRUE
//功能描述：
//	用纯色画水平线
//引用: 
//	被 line.h 使用
// *****************************************************************

static BOOL __ScanSolidLine( _LPLINEDATA lpLineData,  int x0, int y0, int xDir, unsigned int uLen )
{
    register LPBYTE lpVideoAdrStart, lpVideoAdrEnd;
    register int x1, y;//, dir;
    register BYTE color;
    register BYTE xbitStart;
    register BYTE xbitEnd;

	if( xDir > 0 )
    {
		x1 = x0 + uLen;
		xbitStart = leftFillMask[x0 & 0x07];
        xbitEnd = rightFillMask[x1 & 0x07];
        x1--;
    }
    else
    {
		x1 = x0 - uLen + 1; 
		xbitStart = rightFillMask[( x0 + 1 ) & 0x07];
        xbitEnd = leftFillMask[x1 & 0x07];
    }

    y = y0;

    y *= lpLineData->lpDestImage->bmWidthBytes;
	//在显存的开始地址
	lpVideoAdrStart = lpLineData->lpDestImage->bmBits + y + (x0 >> 3);
    //在显存的结束地址
	lpVideoAdrEnd = lpLineData->lpDestImage->bmBits + y + (x1 >> 3);

    if( lpLineData->color )
        color = 0xff;
    else
        color = 0;

    if( lpVideoAdrStart == lpVideoAdrEnd )
    {	//开始点和结束点在同一个地址
        xbitStart &= xbitEnd;
        switch( lpLineData->rop )
        {
        case R2_COPYPEN:
            *lpVideoAdrStart = (*lpVideoAdrStart & ~xbitStart) | ( color & xbitStart );
            break;
        case R2_XORPEN:
            *lpVideoAdrStart = (*lpVideoAdrStart & ~xbitStart) | ( (*lpVideoAdrStart ^ color) & xbitStart );
            break;
        case R2_NOT:
            *lpVideoAdrStart = (*lpVideoAdrStart & ~xbitStart) | ( ~*lpVideoAdrStart & xbitStart );
            break;
        }
    }
    else
    {   //开始点和结束点在不同地址
		// draw start byte
        switch( lpLineData->rop )
        {
        case R2_COPYPEN:
			//画开始地址的点
            *lpVideoAdrStart = (*lpVideoAdrStart & ~xbitStart) | (color & xbitStart );
            lpVideoAdrStart += xDir;//dir;
			//画中间的点
            for( ; lpVideoAdrStart != lpVideoAdrEnd; lpVideoAdrStart+= xDir )
                *lpVideoAdrStart = color;            
			//画结束地址的点
            *lpVideoAdrStart = (*lpVideoAdrStart & ~xbitEnd) | ( color & xbitEnd );
            break;
        case R2_XORPEN:
			//画开始地址的点
            *lpVideoAdrStart = (*lpVideoAdrStart & ~xbitStart) | ( (*lpVideoAdrStart ^ color) & xbitStart );
            lpVideoAdrStart += xDir;
			//画中间的点
            for( ; lpVideoAdrStart != lpVideoAdrEnd; lpVideoAdrStart+=xDir )
                *lpVideoAdrStart = (*lpVideoAdrStart ^ color);
			//画结束地址的点
            *lpVideoAdrStart = (*lpVideoAdrStart & ~xbitEnd) | ( (*lpVideoAdrStart ^ color) & xbitEnd );
            break;
        case R2_NOT:
			//画开始地址的点
            *lpVideoAdrStart = (*lpVideoAdrStart & ~xbitStart) | ( ~*lpVideoAdrStart & xbitStart );
            lpVideoAdrStart+=xDir;
            //画中间的点
            for( ; lpVideoAdrStart != lpVideoAdrEnd; lpVideoAdrStart+=xDir )
                *lpVideoAdrStart = ~*lpVideoAdrStart;
			//画结束地址的点
            *lpVideoAdrStart = (*lpVideoAdrStart & ~xbitEnd) | ( ~*lpVideoAdrStart & xbitEnd );
            break;
        }
    }
    return TRUE;
}

// *****************************************************************
//声明：static BOOL __ScanPatternLine( _LPLINEDATA lpLineData, 
//										int x0, 
//										int y0, 
//										int xDir, 
//										unsigned int uLen )
//参数：
//	IN lpLineData - _LINEDATA结构指针，包含画线需要的数据
//	IN x0 - 起点 x 坐标
//	IN y0 - 起点 y 坐标
//	IN xDir - 方向（当 < 0 , 由右向左）
//	IN uLen - 点数
//返回值：
//	返回TRUE
//功能描述：
//	带模板用纯色画水平线
//引用: 
//	被 line.h 使用
// *****************************************************************

static BOOL __ScanPatternLine( _LPLINEDATA lpLineData,  int x0, int y0, int xDir, unsigned int uLen )
{
    register LPBYTE lpVideoAdrStart, lpVideoAdrEnd;
    register int x1, y;//, dir;
    register WORD color, clrFore, clrBack;
    register BYTE xbitStart;
    register BYTE xbitEnd;

	if( xDir > 0 )
    {
		x1 = x0 + uLen;
		xbitStart = leftFillMask[x0 & 0x07];
        xbitEnd = rightFillMask[x1 & 0x07];
        x1--;
    }
    else
    {
		x1 = x0 - uLen + 1; 
		xbitStart = rightFillMask[(x0+1) & 0x07];
        xbitEnd = leftFillMask[x1 & 0x07];
    }

	y = y0;

    y *= lpLineData->lpDestImage->bmWidthBytes;
	//在显存的开始地址
	lpVideoAdrStart = lpLineData->lpDestImage->bmBits + y + (x0 >> 3);
	//在显存的结束地址
    lpVideoAdrEnd = lpLineData->lpDestImage->bmBits + y + (x1 >> 3);

    if( lpLineData->color )
        clrFore = 0xffff;
    else
        clrFore = 0;
    if( lpLineData->clrBack )
        clrBack = 0xffff;
    else
        clrBack = 0;
    color = (clrFore & lpLineData->pattern) | (clrBack & ~lpLineData->pattern );

    if( lpVideoAdrStart == lpVideoAdrEnd )
    {	//开始点和结束点在同一个地址
        xbitStart &= xbitEnd;
        switch( lpLineData->rop )
        {
        case R2_COPYPEN:
            *lpVideoAdrStart = (*lpVideoAdrStart & ~xbitStart) | ( color & xbitStart );
            break;
        case R2_XORPEN:
            *lpVideoAdrStart = (*lpVideoAdrStart & ~xbitStart) | ( (*lpVideoAdrStart ^ color) & xbitStart );
            break;
        case R2_NOT:
            *lpVideoAdrStart = (*lpVideoAdrStart & ~xbitStart) | ( ~*lpVideoAdrStart & xbitStart );
            break;
        }
    }
    else
    {   // //开始点和结束点在不同地址 draw start byte
        switch( lpLineData->rop )
        {
        case R2_COPYPEN:
			//画开始地址的点
            *lpVideoAdrStart = (*lpVideoAdrStart & ~xbitStart) | (color & xbitStart );
			//画中间的点   
            lpVideoAdrStart+=xDir;
            for( ; lpVideoAdrStart != lpVideoAdrEnd; lpVideoAdrStart+=xDir )
                *lpVideoAdrStart = (BYTE)color;
			//画结束地址的点
            *lpVideoAdrStart = (*lpVideoAdrStart & ~xbitEnd) | ( color & xbitEnd );
            break;
        case R2_XORPEN:
			//画开始地址的点
            *lpVideoAdrStart = (*lpVideoAdrStart & ~xbitStart) | ( (*lpVideoAdrStart ^ color) & xbitStart );
            //画中间的点
            lpVideoAdrStart+=xDir;
            for( ; lpVideoAdrStart != lpVideoAdrEnd; lpVideoAdrStart+=xDir )
                *lpVideoAdrStart = (*lpVideoAdrStart ^ color);
			//画结束地址的点
            *lpVideoAdrStart = (*lpVideoAdrStart & ~xbitEnd) | ( (*lpVideoAdrStart ^ color) & xbitEnd );
            break;
        case R2_NOT:
			//画开始地址的点
            *lpVideoAdrStart = (*lpVideoAdrStart & ~xbitStart) | ( ~*lpVideoAdrStart & xbitStart );
			//画中间的点
            lpVideoAdrStart+=xDir;
            for( ; lpVideoAdrStart != lpVideoAdrEnd; lpVideoAdrStart+=xDir )
                *lpVideoAdrStart = ~*lpVideoAdrStart;
			//画结束地址的点
            *lpVideoAdrStart = (*lpVideoAdrStart & ~xbitEnd) | ( ~*lpVideoAdrStart & xbitEnd );
            break;
        }
    }
    return TRUE;
}

// *****************************************************************
//声明：static BOOL __ScanTransparentLine( _LPLINEDATA lpLineData, 
//										int x0, 
//										int y0, 
//										int xDir, 
//										unsigned int uLen )
//参数：
//	IN lpLineData - _LINEDATA结构指针，包含画线需要的数据
//	IN x0 - 起点 x 坐标
//	IN y0 - 起点 y 坐标
//	IN xDir - 方向（当 < 0 , 由右向左）
//	IN uLen - 点数
//返回值：
//	返回TRUE
//功能描述：
//	带模板用纯色画透明水平线
//引用: 
//	被 line.h 使用
// *****************************************************************

static BOOL __ScanTransparentLine( _LPLINEDATA lpLineData,  int x0, int y0, int xDir, unsigned int uLen )
{
    LPBYTE lpVideoAdrStart, lpVideoAdrEnd;
    int x1, y;//, dir;
    WORD color;
    BYTE xbitStart;
    BYTE xbitEnd;
    register BYTE pattern;

	if( xDir > 0 )
    {
		x1 = x0 + uLen;
		xbitStart = leftFillMask[x0 & 0x07];
        xbitEnd = rightFillMask[x1 & 0x07];
        x1--;
    }
    else
    {
		x1 = x0 - uLen + 1;
		xbitStart = rightFillMask[(x0+1) & 0x07];
        xbitEnd = leftFillMask[x1 & 0x07];
    }

    y = y0;

    y *= lpLineData->lpDestImage->bmWidthBytes;
	//在显存的开始地址
	lpVideoAdrStart = lpLineData->lpDestImage->bmBits + y + (x0 >> 3);
    //在显存的结束地址
	lpVideoAdrEnd = lpLineData->lpDestImage->bmBits + y + (x1 >> 3);

    pattern = lpLineData->pattern;

    if( lpLineData->color )
        color = 0xffff;
    else
        color = 0;
    color = (color & pattern);

    if( lpVideoAdrStart == lpVideoAdrEnd )
    {	//开始点和结束点在同一个地址
        xbitStart &= xbitEnd;
        switch( lpLineData->rop )
        {
        case R2_COPYPEN:
            *lpVideoAdrStart = (*lpVideoAdrStart & ~xbitStart) | ( color & xbitStart );
            break;
        case R2_XORPEN:
            *lpVideoAdrStart = (*lpVideoAdrStart & ~xbitStart) | ( (*lpVideoAdrStart ^ color) & xbitStart );
            break;
        case R2_NOT:
            *lpVideoAdrStart = (*lpVideoAdrStart & ~xbitStart) | ( ~*lpVideoAdrStart & xbitStart );
            break;
        }
    }
    else
    {   //开始点和结束点在不同地址// draw start byte
        switch( lpLineData->rop )
        {
        case R2_COPYPEN:
			//画开始地址的点
            *lpVideoAdrStart = (*lpVideoAdrStart & ~xbitStart) | (color & xbitStart );
            
            //画中间的点
            lpVideoAdrStart+=xDir;
            for( ; lpVideoAdrStart != lpVideoAdrEnd; lpVideoAdrStart+=xDir )
                *lpVideoAdrStart = (*lpVideoAdrStart & ~pattern) | color;
			//画结束地址的点
            *lpVideoAdrStart = (*lpVideoAdrStart & ~xbitEnd) | ( color & xbitEnd );
            break;
        case R2_XORPEN:
			//画开始地址的点
            *lpVideoAdrStart = (*lpVideoAdrStart & ~xbitStart) | ( (*lpVideoAdrStart ^ color) & xbitStart );

            //画中间的点
            lpVideoAdrStart+=xDir;
            for( ; lpVideoAdrStart != lpVideoAdrEnd; lpVideoAdrStart+=xDir )
                *lpVideoAdrStart = (*lpVideoAdrStart & ~pattern) | ( (*lpVideoAdrStart ^ color) & pattern );
			//画结束地址的点
            *lpVideoAdrStart = (*lpVideoAdrStart & ~xbitEnd) | ( (*lpVideoAdrStart ^ color) & xbitEnd );
            break;
        case R2_NOT:
			//画开始地址的点
            *lpVideoAdrStart = (*lpVideoAdrStart & ~xbitStart) | ( ~*lpVideoAdrStart & xbitStart );

            //画中间的点
            lpVideoAdrStart+=xDir;
            for( ; lpVideoAdrStart != lpVideoAdrEnd; lpVideoAdrStart+=xDir )
                *lpVideoAdrStart = (*lpVideoAdrStart & ~pattern) | (~*lpVideoAdrStart & pattern);
			//画结束地址的点
            *lpVideoAdrStart = (*lpVideoAdrStart & ~xbitEnd) | ( ~*lpVideoAdrStart & xbitEnd );
            break;
        }
    }
    return TRUE;
}

#include "..\include\line.h"

#define _SRCCOPY             0xCC // dest = source
#define _SRCPAINT            0xEE // dest = source OR dest
#define _SRCAND              0x88 // dest = source AND dest
#define _SRCINVERT           0x66 // dest = source XOR dest

// *****************************************************************
//声明：static void __MoveBits( LPBYTE lpDest, int nBits, LPCBYTE lpSrc, int count )
//参数：
//	IN lpDest - 目标地址 
//	IN nBits - 移动的 bit 数
//	IN lpSrc - 源地址
//	IN count - 源bytes数
//返回值：
//	无
//功能描述：
//	将源移动 nBits 比特数。当 nBits > 0 为右移；< 0 为左移
//引用: 
//	
// *****************************************************************

static void __MoveBits( LPBYTE lpDest, int nBits, LPCBYTE lpSrc, int count )
{
    int vBits;

    if( nBits > 0 )     // >>
    {	//为右移
        *lpDest++ = *lpSrc++ >> nBits;
        vBits = 8 - nBits;
        count--;
        while( count )
        {
            *lpDest = (((WORD)*(lpSrc-1)) << vBits ) | (*lpSrc >> nBits);
            lpDest++; lpSrc++;
            count--;
        }
        // end byte
        *lpDest = *(lpSrc-1) << vBits;
    }
    else if( nBits < 0 )     // <<
    {	//为左移
        nBits = -nBits;
        vBits = 8-nBits;

        count--;   // read for end byte
        while( count )
        {
            *lpDest = (*lpSrc << nBits) | (*(lpSrc+1) >> vBits);
            lpDest++; lpSrc++;
            count--;
        }
        // end byte
        *lpDest = *lpSrc << nBits;
    }
    else     // No move
    {	//不需要移动
        while( count )
        {
            *lpDest++ = *lpSrc++;
            count--;
        }
    }
}

// *****************************************************************
//声明：static BOOL __TextSolidBitBlt( _LPBLKBITBLT lpData )
//参数：
//	IN lpData - _BLKBITBLT结构指针
//返回值：
//	返回TRUE
//功能描述：
//	用纯色输出文本
//引用: 
//	
// *****************************************************************

static BOOL __TextSolidBitBlt( _LPBLKBITBLT lpData )
{
    LPBYTE lpDestStart, lpDst;
    LPCBYTE lpMaskStart, lpStart;
    WORD clFore, clBack, color;
    WORD maskValue, lMask, rMask, rightMask, destBytes, maskBytes;
    int i, j, n,  rows, maskShift, dstShift, shift;
    BYTE mask;
	//目标地址
    lpDestStart = lpData->lpDestImage->bmBits + lpData->lprcDest->top * lpData->lpDestImage->bmWidthBytes + (lpData->lprcDest->left >> 3);
    //位模开始位置
	lpMaskStart = lpData->lpSrcImage->bmBits + lpData->lprcSrc->top * lpData->lpSrcImage->bmWidthBytes + (lpData->lprcSrc->left >> 3);

    i = lpData->lprcDest->right - lpData->lprcDest->left;

    mask = (lpData->lprcSrc->left + i) & 0x07;
    if ( mask )
        rightMask = (WORD)(0xffffff00l >> mask);
    else
        rightMask = (WORD)0xffffffffl;
	//需要的移位数
    maskShift = lpData->lprcSrc->left & 0x07;
    dstShift = lpData->lprcDest->left & 0x07;
	//需要的目标字节数
    destBytes =( (lpData->lprcDest->right-1) >> 3) - (lpData->lprcDest->left >> 3) + 1;
    maskBytes = ( (lpData->lprcSrc->left + i - 1) >> 3 ) - (lpData->lprcSrc->left >> 3) + 1;

    lMask = leftFillMask[lpData->lprcDest->left&0x07];
    rMask = rightFillMask[lpData->lprcDest->right&0x07];
    if( destBytes == 1 )
        lMask &= rMask;

    if( lpData->solidColor )
         clFore = 0xffff;
    else
         clFore = 0x0000;
    if( lpData->solidBkColor )
         clBack = 0xffff;
    else
         clBack = 0x0000;
	//需要处理的行数
    rows = lpData->lprcDest->bottom - lpData->lprcDest->top;
	//对每一行数据进行处理
    for( i = 0; i < rows; i++ )
    {
	    //处理开始的字节  handle first byte
        lpStart = lpMaskStart;
        lpDst = lpDestStart;

        maskValue = *lpStart;
        maskValue <<= (maskShift + 8);
        maskValue >>= dstShift;
        shift = maskShift + 8 - dstShift;

	    if( maskBytes == 1 )
	    {
            maskValue &= (rightMask << shift);
            shift = 0;
	    }
        lpStart++;
        j = 1;   /// count mask bytes
        n = 0;   // count dest bytes

        // fill first byte
        if( shift > 8 )
        {
            shift -= 8;
            maskValue |= *lpStart++ << shift;
            if( ++j == maskBytes )
            {
                maskValue &= (rightMask << shift);
                shift = 0;
            }
        }
        mask = maskValue >> 8;
        maskValue <<= 8;
        shift += 8;
        color = ((clFore & mask) | (clBack & ~mask));
        *lpDst++ = (*lpDst & ~lMask) | (color & lMask);
        n++;
		//处理中间的字节
        // fill middle bytes
        while( n < destBytes - 1 )
        {
            if( shift > 8 )
            {
                shift -= 8;
                maskValue |= *lpStart++ << shift;
                if( ++j == maskBytes )
                {
                    maskValue &= (rightMask << shift);
                    shift = 0;
                }
            }
            mask = maskValue >> 8;
            maskValue <<= 8;
            shift += 8;

            *lpDst++ = (clFore & mask) | (clBack & ~mask);
            n++;
        }

        // 假如可能，处理最后的字节 fill end byte
        if( n < destBytes )
        {
            if( shift > 8 )
            {
                shift -= 8;
                maskValue |= *lpStart++ << shift;
                if( ++j == maskBytes )
                {
                    maskValue &= (rightMask << shift);
                    shift = 0;
                }
            }
            mask = maskValue >> 8;
            color = ((clFore & mask) | (clBack & ~mask));
            *lpDst = (*lpDst & ~rMask) | (color & rMask);
        }
		//下一行
        lpMaskStart += lpData->lpSrcImage->bmWidthBytes;
        lpDestStart += lpData->lpDestImage->bmWidthBytes;
    }
    return TRUE;
}

// *****************************************************************
//声明：static BOOL __TextTransparentBitBlt( _LPBLKBITBLT lpData )
//参数：
//	IN lpData - _BLKBITBLT结构指针
//返回值：
//	返回TRUE
//功能描述：
//	用纯色输出透明文本（当位模bit为1时，输出；否则不输出）
//引用: 
//	
// *****************************************************************

static BOOL __TextTransparentBitBlt( _LPBLKBITBLT lpData )
{
    LPBYTE lpDestStart, lpDst;
    LPCBYTE lpMaskStart, lpStart;
    WORD clFore;
    WORD maskValue, rightMask, destBytes, maskBytes;
    int i, j, n,  rows, maskShift, dstShift, shift;
    BYTE mask;

    lpDestStart = lpData->lpDestImage->bmBits + lpData->lprcDest->top * lpData->lpDestImage->bmWidthBytes + (lpData->lprcDest->left >> 3);
    lpMaskStart = lpData->lpSrcImage->bmBits + lpData->lprcSrc->top * lpData->lpSrcImage->bmWidthBytes + (lpData->lprcSrc->left >> 3);

    i = lpData->lprcDest->right - lpData->lprcDest->left;
    mask = (lpData->lprcSrc->left + i) & 0x07;
    if ( mask )
        rightMask = (WORD)(0xffffff00l >> mask);
    else
        rightMask = (WORD)0xffffffffl;

    maskShift = lpData->lprcSrc->left & 0x07;
    dstShift = lpData->lprcDest->left & 0x07;

    destBytes =( (lpData->lprcDest->right-1) >> 3) - (lpData->lprcDest->left >> 3) + 1;
    maskBytes = ( (lpData->lprcSrc->left + i - 1) >> 3 ) - (lpData->lprcSrc->left >> 3) + 1;

    if( lpData->solidColor )
         clFore = 0xffff;
    else
         clFore = 0x0000;

    rows = lpData->lprcDest->bottom - lpData->lprcDest->top;

    for( i = 0; i < rows; i++ )
    {
	    //处理开始的字节 //  handle first byte
        lpStart = lpMaskStart;
        lpDst = lpDestStart;

        maskValue = *lpStart;
        maskValue <<= (maskShift + 8);
        maskValue >>= dstShift;
        shift = maskShift + 8 - dstShift;

	    if( maskBytes == 1 )
	    {
            maskValue &= (rightMask << shift);
            shift = 0;
	    }
        lpStart++;
        j = 1;   /// count mask bytes
        n = 0;   // count dest bytes
		//处理余下的字节
        while( n < destBytes )
        {
            if( shift > 8 )
            {
                shift -= 8;
                maskValue |= *lpStart++ << shift;
                if( ++j == maskBytes )
                {
                    maskValue &= (rightMask << shift);
                    shift = 0;
                }
            }
            mask = maskValue >> 8;
            maskValue <<= 8;
            shift += 8;

            *lpDst = (*lpDst & ~mask) | (clFore & mask);
            lpDst++;
            n++;
        }
		//下一行
        lpMaskStart += lpData->lpSrcImage->bmWidthBytes;
        lpDestStart += lpData->lpDestImage->bmWidthBytes;
    }
    return TRUE;
}

// *****************************************************************
//声明：static BOOL _TextBitBlt( _LPBLKBITBLT lpData )
//参数：
//	IN lpData - _BLKBITBLT结构指针
//返回值：
//	返回TRUE
//功能描述：
//	用输出文本或位模
//引用: 
//	
// *****************************************************************

static BOOL _TextBitBlt( _LPBLKBITBLT lpData )
{
    if( lpData->backMode == TRANSPARENT )
        return __TextTransparentBitBlt( lpData );
    else
        return __TextSolidBitBlt( lpData );
}

// *****************************************************************
//声明：static BOOL __BltFillTransparentPattern( _LPBLKBITBLT lpData )
//参数：
//	IN lpData - _BLKBITBLT结构指针
//返回值：
//	成功TRUE
//功能描述：
//	用模板透明填充
//引用: 
//	
// *****************************************************************

static BOOL __BltFillTransparentPattern( _LPBLKBITBLT lpData )
{
    int height = lpData->lprcDest->bottom - lpData->lprcDest->top;
    LPBYTE lpDstStart = lpData->lpDestImage->bmBits +
                   lpData->lpDestImage->bmWidthBytes * lpData->lprcDest->top +
                   (lpData->lprcDest->left >> 3);
    WORD scanBytes = lpData->lpDestImage->bmWidthBytes;
    WORD destBytes = ( (lpData->lprcDest->right - 1) >> 3 ) - ( lpData->lprcDest->left >> 3 ) + 1;
    BYTE leftMask = leftFillMask[lpData->lprcDest->left & 0x07];
    BYTE rightMask = rightFillMask[lpData->lprcDest->right & 0x07];
    LPCBYTE lpPattern = lpData->lpBrush->pattern;
    WORD clrFore;
	int xShift, yShift;
	register WORD Temp;

    if( destBytes == 1 )
        leftMask &= rightMask;
    if( lpData->lpBrush->color )
        clrFore = 0xffff;
    else
        clrFore = 0;

	if( lpData->lpptBrushOrg )
	{
		//xShift = (lpData->lprcMask->left - lpData->lpptBrushOrg->x ) % 0x07;// + iWidthPat) % iWidthPat; 
		//yShift = lpData->lprcMask->top - lpData->lpptBrushOrg->y;
		xShift = (lpData->lprcDest->left - lpData->lpptBrushOrg->x ) % 0x07;// + iWidthPat) % iWidthPat; 
		yShift = lpData->lprcDest->top - lpData->lpptBrushOrg->y;
	}
	else
	{
		yShift = xShift = 0;
	}


    // 假如可能，填充开始字节 fill left bytes if posible
    if( leftMask != 0xff )
    {
        register LPBYTE lpDst = lpDstStart;
        register int m;//, n = lpData->lprcDest->top;
        register BYTE pattern;

        for( m = 0; m < height; m++, yShift++, lpDst += scanBytes )
        {
            pattern = *(lpPattern+(yShift&0x07)) & leftMask;


			Temp = ( ((WORD)pattern)<<8 ) | pattern;
			pattern = Temp >> xShift;

            *lpDst = (*lpDst & ~pattern) | ( clrFore & pattern );
        }
        destBytes--;
        lpDstStart++;
    }
    // 假如可能，填充终点 fill right bytes if posible
    if( rightMask != 0xff && destBytes > 0 )
    {
        register LPBYTE lpDst = lpDstStart + destBytes - 1;
        register int m;//, n = lpData->lprcDest->top;
        register BYTE pattern;
        for( m = 0; m < height; m++, yShift++, lpDst += scanBytes )
        {
            pattern = *(lpPattern+(yShift&0x07)) & rightMask;

			Temp = ( ((WORD)pattern)<<8 ) | pattern;
			pattern = Temp >> xShift;

            *lpDst = (*lpDst & ~pattern) | (clrFore & pattern);
        }
        destBytes--;
    }

    // 填充中间的字节 fill middle bytes
    if( destBytes > 0 )
    {
        register LPBYTE lpDst;
        register int n, m;//, k = lpData->lprcDest->top;
        register BYTE pattern;

        for( m = 0; m < height; m++, yShift++ )
        {
            pattern = *(lpPattern+(yShift&0x07));

			Temp = ( ((WORD)pattern)<<8 ) | pattern;
			pattern = Temp >> xShift;

            lpDst = lpDstStart;
            // line copy
            for( n = 0; n < destBytes; n++, lpDst++ )
                *lpDst = (*lpDst & ~pattern) | (clrFore & pattern);

            lpDstStart += scanBytes;
        }
    }
    return TRUE;
}

#undef  FUNCTION
#undef  BLT_ROP
#define FUNCTION __BltFillPattern
#define BLT_ROP( bDst, bSrc )  ( (bSrc) )
#include "bltfpat.h"

#undef  FUNCTION
#undef  BLT_ROP
#define FUNCTION __BltPatInvertPattern
#define BLT_ROP( bDst, bSrc ) ( (bDst) ^ (bSrc) )
#include "bltfpat.h"

#undef  FUNCTION
#undef  BLT_ROP
#define FUNCTION __BltSrcCopy
#define BLT_ROP( bDst, bSrc )  ( (bSrc) )
#include "bltcpy.h"

#undef  FUNCTION
#undef  BLT_ROP
#define FUNCTION __BltSrcAnd
#define BLT_ROP( bDst, bSrc )  ( (bDst) & (bSrc) )
#include "bltcpy.h"

#undef  FUNCTION
#undef  BLT_ROP
#define FUNCTION __BltNotSrcCopy
#define BLT_ROP( bDst, bSrc )  ( ~(bSrc) )
#include "bltcpy.h"

#undef  FUNCTION
#undef  BLT_ROP
#define FUNCTION __BltSrcInvert
#define BLT_ROP( bDst, bSrc )  ( (bDst) ^ (bSrc) )
#include "bltcpy.h"

#undef  FUNCTION
#undef  BLT_ROP
#define FUNCTION __BltSrcPaint
#define BLT_ROP( bDst, bSrc )  ( (bDst) | (bSrc) )
#include "bltcpy.h"

#undef  FUNCTION
#undef  BLT_ROP
#define FUNCTION __BltFillSolid
#define BLT_ROP( bDst, bColor ) ( (bColor) )
#include "bltfill.h"

#undef  FUNCTION
#undef  BLT_ROP
#define FUNCTION __BltPatInvertSolid
#define BLT_ROP( bDst, bSrc ) ( (bDst) ^ (bSrc) )
#include "bltfill.h"

#undef  FUNCTION
#undef  BLT_ROP
#define FUNCTION __BltDstInvert
#define BLT_ROP( bDst, bSrc ) (~(bDst))
#include "bltfill.h"

// *****************************************************************
//声明：static BOOL _BlkBitTransparentBlt( _LPBLKBITBLT lpData )
//参数：
//	IN lpData - _BLKBITBLT结构指针
//返回值：
//	成功TRUE
//功能描述：
//	透明填充位模
//引用: 
//	
// *****************************************************************

static BOOL _BlkBitTransparentBlt( _LPBLKBITBLT lpData )
{
    if( lpData->lpBrush )	//是否带模板
        return __BltFillTransparentPattern( lpData );
    else
        return __TextTransparentBitBlt( lpData );
}

#undef FUNCTION
#undef SRC_ROP
#undef DEST_ROP
#define SRC_ROP( bSrc, bMask ) ((bSrc)&(bMask))
#define DEST_ROP( bDest, bMask ) ((bDest)&~(bMask))
#define FUNCTION __BltMaskSrcCopy
#include "bltmask.h"

#undef FUNCTION
#undef SRC_ROP
#undef DEST_ROP
#define SRC_ROP( bSrc, bMask ) ((bSrc)&(~(bMask)))
#define DEST_ROP( bDest, bMask ) ((bDest)&(bMask))
#define FUNCTION __BltInvertMaskSrcCopy
#include "bltmask.h"

// *****************************************************************
//声明：static BOOL _BlkBitMaskBlt( _LPBLKBITBLT lpData )
//参数：
//	IN lpData - _BLKBITBLT结构指针
//返回值：
//	成功TRUE
//功能描述：
//	带屏蔽位的位块传送
//引用: 
//	
// *****************************************************************

static BOOL _BlkBitMaskBlt( _LPBLKBITBLT lpData )
{
    if( lpData->lpSrcImage->bmBitsPixel == 1 )
    {
        if( lpData->dwRop == SRCCOPY )
            return __BltMaskSrcCopy( lpData );
        else if( lpData->dwRop == 1000 )
        {
            return __BltInvertMaskSrcCopy( lpData );
        }
    }
    return FALSE;

}

// *****************************************************************
//声明：static BOOL _BlkBitBlt( _LPBLKBITBLT lpData )
//参数：
//	IN lpData - _BLKBITBLT结构指针
//返回值：
//	成功TRUE
//功能描述：
//	位块传送
//引用: 
//	
// *****************************************************************

static BOOL _BlkBitBlt( _LPBLKBITBLT lpData )
{
    switch( lpData->dwRop )
    {
    case SRCCOPY:		// dest = src
        if( lpData->lpSrcImage != lpData->lpDestImage )
            return __TextSolidBitBlt( lpData );
        else
            return __BltSrcCopy( lpData );        
    case SRCAND:		// dest = dest & src
        return __BltSrcAnd( lpData );
    case SRCINVERT:		// dest = dest ^ src
        return __BltSrcInvert( lpData );
    case SRCPAINT:		// dest = dest | src
        return __BltSrcPaint( lpData );
    case PATCOPY:
        if( lpData->lpBrush == 0 )
            return __BltFillSolid( lpData );
        else
        {
            if( lpData->lpBrush->style == BS_SOLID )
            {
                lpData->solidColor = lpData->lpBrush->color;
                return __BltFillSolid( lpData );
            }
            else
                return __BltFillPattern( lpData );
        }
    case BLACKNESS:
        lpData->solidColor = CL_BLACK;
        return __BltFillSolid( lpData );
    case WHITENESS:
        lpData->solidColor = CL_WHITE;
        return __BltFillSolid( lpData );
    case PATINVERT:
        if( lpData->lpBrush == 0 )
            return __BltPatInvertSolid( lpData );
        else
            return __BltPatInvertPattern( lpData );
    case DSTINVERT:		// dest = ~dest
        return __BltDstInvert( lpData );
    case NOTSRCCOPY:	// dest = ~src
        return __BltNotSrcCopy( lpData );
    }
    return FALSE;
}


