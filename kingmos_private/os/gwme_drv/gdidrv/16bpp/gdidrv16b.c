/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/


/*****************************************************
文件说明：图形设备驱动程序（16bit(1byte) / pixel ）
版本号：2.0.0
开发时期：2000
作者：李林
修改记录：
     2.LN, 2003-07-16, 增加 MERGECOPY 和 MERGEPAINT
     1.LN 2003-05-06, Hatch brush有错误

******************************************************/

#include <eframe.h>
#include <gwmeobj.h>

//#ifndef COLOR_16BPP
//#error not define COLOR_16BPP in file version.h
//#endif

//extern const _BITMAPDATA __displayBitmap;

static COLORREF _PutPixel( _LPPIXELDATA lpPixelData );
static COLORREF _GetPixel( _LPPIXELDATA lpPixelData );
static BOOL _Line( _LPLINEDATA lpData );
static BOOL _BlkBitTransparentBlt( _LPBLKBITBLT lpData );
static BOOL _BlkBitMaskBlt( _LPBLKBITBLT lpData );
static BOOL _BlkBitBlt( _LPBLKBITBLT lpData );
static COLORREF _RealizeColor( COLORREF color, LPCDWORD lpcdwPal, UINT uPalNumber, UINT uPalFormat );
static COLORREF _UnrealizeColor( COLORREF color, LPCDWORD lpcdwPal, UINT uPalNumber, UINT uPalFormat );

//申明图形设备驱动程序接口对象
const _DISPLAYDRV _drvDisplay16BPP = {
    _PutPixel,
    _GetPixel,
    _Line,
    _BlkBitTransparentBlt,
    _BlkBitMaskBlt,
    _BlkBitBlt,
    _RealizeColor,
    _UnrealizeColor
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
//声明：static COLORREF _RealizeColor( COLORREF color )
//参数：
//	IN color - RGB 颜色值
//返回值：
//	设备相关的颜色值
//功能描述：
//	由RGB颜色值得到设备相关的颜色值
//	默认的设备相关的颜色格式: 16bits: BBBBB GGGGGG RRRRR 
//引用: 
//	
// *****************************************************************
static COLORREF _RealizeColor( COLORREF color, LPCDWORD lpcdwPal, UINT uPalNumber, UINT uPalFormat )
{  // r5 6g b5
	// bbbb bbbb gggg gggg rrrr rrrr ->
	// 0000 0000 rrrr rggg gggb bbbb

//    DWORD r = ( color << 8 ) & 0xf800;
  //  DWORD g = ( color >> 5) & 0x07e0;
    //DWORD b = ( color >> 19) & 0x001f;
	//return r | g | b;

	if( lpcdwPal )
	{
		return _Gdi_RealizeColor( color, lpcdwPal, uPalNumber, uPalFormat );
	}
	else
	{

		DWORD r;
		DWORD g;
		DWORD b;
		
		// 默认的颜色格式: 16bits: BBBBB GGGGGG RRRRR    . format to 5b 6g 5r
		r = ((color << 24) >> 16) & 0xf800;
		g = ((color << 16) >> 21) & 0x07e0;
		b = ((color << 8) >> 27) & 0x001f;
		return (b | g | r);
	}

/*  
	这代码将会导致 灰色失真（偏绿）
	DWORD r, g, b;

	r = ( (color & 0xff) * 0x1f / 0xff ) << 11;	
	g = ( ( (color >> 8 ) & 0xff) * 0x3f / 0xff ) << 5;
	b = ( ( (color >> 16 ) & 0xff) * 0x1f / 0xff );
	
	return r | g | b;
*/

}

// *****************************************************************
//声明：static COLORREF _UnrealizeColor( COLORREF color, LPCDWORD lpcdwPal, UINT uPalNumber, UINT uPalFormat )
//参数：
//	IN color - 设备相关颜色值
//返回值：
//	设备无关颜色值
//功能描述：
//	由设备相关颜色值得到设备无关的RGB值
//	默认的设备相关的颜色格式: 16bits: BBBBB GGGGGG RRRRR
//引用: 
//	
// *****************************************************************
static COLORREF _UnrealizeColor( DWORD color, LPCDWORD lpcdwPal, UINT uPalNumber, UINT uPalFormat )
{
	// 1111 1111 rrrr rggg gggb bbbb ->
	// bbbb bbbb gggg gggg rrrr rrrr

	if( lpcdwPal )
	{
		return _Gdi_UnrealizeColor( color, lpcdwPal, uPalNumber, uPalFormat );
	}
	else
	{
		DWORD r = (color >> 11) & 0x1f;	
		DWORD g = (color >> 5) & 0x3f;
		DWORD b = (color & 0x1f);
/*		
		if( r == 0x1f )
			r = 0xff;
		if( g == 0x3f )
			g = 0xff;
		if( b == 0x1f )
			b = 0xff;    
*/
		return (b << 19) | (g << 10) | r << 3;
	}
/*

	DWORD r = color & 0xf800;
    DWORD g = color & 0x07e0;
	DWORD b = color & 0x001f;

    r = ((r << 16) >> 24);
    g = ((g << 21) >> 16);
	b = ((b << 27) >> 8);
    return (b | g | r);
*/

  // r5 6g b5

/*  这代码将会导致 灰色失真（偏绿）
	DWORD r, g, b;

    r = ( (color & 0xf800) >> 11) * 0xff / 0x1f;
	g = ( ( (color & 0x07e0) >> 5) * 0xff / 0x3f ) << 8;
	b = ( ( (color & 0x001f)) * 0xff / 0x1f ) << 16;

	return r | g | b;
*/
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
    LPWORD lpVideoAdr;
    COLORREF c;
	UINT rop;
	//是否需要真实输出 ？
	if( (rop = lpPixelData->rop) != R2_NOP )
	{	//需要输出
		//得到x 位置/地址
		lpVideoAdr = (LPWORD)(lpPixelData->lpDestImage->bmBits + lpPixelData->y * lpPixelData->lpDestImage->bmWidthBytes) + lpPixelData->x;
		//得到当前/返回的颜色值
		c = *lpVideoAdr;
		//根据操作模式做相关动作
		switch( rop )
		{
		case R2_COPYPEN:			//拷贝到目标
			*lpVideoAdr = (WORD)lpPixelData->color;
			break;
		case R2_XORPEN:				//异或
			*lpVideoAdr ^= (WORD)lpPixelData->color;
			break;
		case R2_NOT:				//非
			*lpVideoAdr = ~*lpVideoAdr;
			break;
		case R2_BLACK:				//黑
			*lpVideoAdr = 0;
			break;
		case R2_WHITE:				//白
			*lpVideoAdr = 0xffff;
			break;
		case R2_MASKPEN:   // dest = dest-pixel-color and pen-color
			*lpVideoAdr &= (WORD)lpPixelData->color;
			break;
		case R2_MERGEPEN:  // dest = dest-pixel-color or pen-color
			*lpVideoAdr |= (WORD)lpPixelData->color;
			break;
		case R2_MERGENOTPEN:
			*lpVideoAdr |= ~(WORD)lpPixelData->color;
			break;
		case R2_NOTCOPYPEN:
			*lpVideoAdr = ~(WORD)lpPixelData->color;
			break;
		case R2_NOTXORPEN:
			*lpVideoAdr = ~( *lpVideoAdr ^ (WORD)lpPixelData->color );
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
    return *((LPWORD)(lpPixelData->lpDestImage->bmBits + lpPixelData->y * lpPixelData->lpDestImage->bmWidthBytes) + lpPixelData->x);
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
static BOOL __VertialSolidLine( _LPLINEDATA lpLineData, int x0, int y0, int yDir, int len )
{
    register LPWORD lpDest;
    register int sum, widthBytes;
    register WORD color;
	UINT rop;
	//扫描行宽度(字节数)
    widthBytes = lpLineData->lpDestImage->bmWidthBytes;
	//目标地址
	lpDest = (LPWORD)(lpLineData->lpDestImage->bmBits + y0 * widthBytes) + x0;
    color = (WORD)lpLineData->color;
    
    sum = len;
	//正向或反向
	if( yDir < 0 )
        widthBytes = -widthBytes;		//反向
	//根据操作模式做不同的处理
    if( (rop = lpLineData->rop) == R2_COPYPEN )
    {	//拷贝
        while( sum-- )
        {
            *lpDest = color;
            lpDest = (LPWORD)((LPBYTE)lpDest + widthBytes);
        }
    }
    else if( rop == R2_NOT )
    {	//非
        while( sum-- )
        {
            *lpDest = ~*lpDest;
            lpDest = (LPWORD)((LPBYTE)lpDest + widthBytes);
        }
    }
    else if( rop == R2_XORPEN )
    {	//异或
        while( sum-- )
        {
            *lpDest = *lpDest ^ color;
            lpDest = (LPWORD)((LPBYTE)lpDest + widthBytes);
        }
    }
    else if( rop == R2_NOTXORPEN )
    {	//NOT 异或
        while( sum-- )
        {
            *lpDest = ~(*lpDest ^ color);
            lpDest = (LPWORD)((LPBYTE)lpDest + widthBytes);
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
static BOOL __VertialPatternLine( _LPLINEDATA lpLineData, int x0, int y0, int yDir, int len )
{
    register LPWORD lpDest;
    register int sum, widthBytes;
    WORD clrFore, clrBack;
    BYTE pattern;
	UINT rop;
	//扫描行宽度(字节数)
    widthBytes = lpLineData->lpDestImage->bmWidthBytes;
	//目标地址
	lpDest = (LPWORD)(lpLineData->lpDestImage->bmBits + y0 * widthBytes) + x0;
	//前景和背景颜色
    clrFore = (WORD)lpLineData->color;
    clrBack = (WORD)lpLineData->clrBack;

    pattern = lpLineData->pattern;
	sum = len;
	//正向或反向
	if( yDir < 0 )
		widthBytes = -widthBytes;	//反向
	//根据操作模式做不同的处理
    if( (rop = lpLineData->rop) == R2_COPYPEN )
    {	//拷贝
        while( sum-- )
        {
            if( pattern & patternMask[y0&0x07] )
            {
                *lpDest = (WORD)clrFore;
            }
            else
            {
                *lpDest = (WORD)clrBack;
            }
            lpDest = (LPWORD)((LPBYTE)lpDest + widthBytes);
            y0 += yDir;//dir;
        }
    }
    else if( rop == R2_NOT )
    {	//非       
        while( sum-- )
        {
            *lpDest = ~*lpDest;
            lpDest = (LPWORD)((LPBYTE)lpDest + widthBytes);
        }        
    }
    else if( rop == R2_XORPEN )
    {	//异或
        while( sum-- )
        {
            if( pattern & patternMask[y0&0x07] )
            {
                *lpDest ^= clrFore;
            }
            else
            {
                *lpDest ^= clrBack;
            }
            lpDest = (LPWORD)((LPBYTE)lpDest + widthBytes);
            y0 += yDir;//dir;
        }
    }
	else if( rop == R2_NOTXORPEN )
	{
        while( sum-- )
        {
            if( pattern & patternMask[y0&0x07] )
            {
                *lpDest = ~( *lpDest ^ clrFore );
            }
            else
            {
                *lpDest = ~(*lpDest ^ clrBack);
            }
            lpDest = (LPWORD)((LPBYTE)lpDest + widthBytes);
            y0 += yDir;//dir;
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
static BOOL __VertialTransparentLine( _LPLINEDATA lpLineData, int x0, int y0, int yDir, int len )
{
    register LPWORD lpDest;
    register int sum, widthBytes;
    WORD clrFore;
    BYTE pattern;
	UINT rop;
    //扫描行宽度(字节数)
    widthBytes = lpLineData->lpDestImage->bmWidthBytes;
	//目标地址
	lpDest = (LPWORD)(lpLineData->lpDestImage->bmBits + y0 * widthBytes) + x0;
	//前景和背景颜色
    clrFore = (WORD)lpLineData->color;
    pattern = lpLineData->pattern;
    sum = len;
	//正向或反向
	if( yDir < 0 )
		widthBytes = -widthBytes;	//反向
	//根据操作模式做不同的处理
    if( (rop = lpLineData->rop) == R2_COPYPEN )
    {	//拷贝
        while( sum-- )
        {
            if( pattern & patternMask[y0&0x07] )
                *lpDest = (WORD)clrFore;
            lpDest = (LPWORD)((LPBYTE)lpDest + widthBytes);
            y0 += yDir;//dir;
        }
    }
    else if( rop == R2_NOT )
    {	//非
        while( sum-- )
        {
            *lpDest = ~*lpDest;
            lpDest = (LPWORD)((LPBYTE)lpDest + widthBytes);
        }
    }
    else if( rop == R2_XORPEN )
    {	//异或
        while( sum-- )
        {
            if( pattern & patternMask[y0&0x07] )
                *lpDest ^= (WORD)clrFore;
            lpDest = (LPWORD)((LPBYTE)lpDest + widthBytes);
            y0 += yDir;//dir;
        }
    }
    else if( rop == R2_NOTXORPEN )
    {	//NOT异或
        while( sum-- )
        {
            if( pattern & patternMask[y0&0x07] )
                *lpDest = ~( *lpDest ^ (WORD)clrFore );
            lpDest = (LPWORD)((LPBYTE)lpDest + widthBytes);
            y0 += yDir;//dir;
        }
    }
    return TRUE;
}
/*
static BOOL _VertialLine( _LPLINEDATA lpLineData )
{
    if( lpLineData->pattern == 0xff )
    {
        return __VertialSolidLine( lpLineData );
    }
    else
    {
        if( lpLineData->backMode == TRANSPARENT )
            return __VertialTransparentLine( lpLineData );
        else
            return __VertialPatternLine( lpLineData );
    }
}
*/

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
static BOOL __ScanSolidLine( _LPLINEDATA lpLineData, int x0, int y0, int xDir, unsigned int uLen )
{
    register LPWORD lpDestStart;
    register WORD color;
	//得到开始地址
	lpDestStart = (LPWORD)(lpLineData->lpDestImage->bmBits + lpLineData->lpDestImage->bmWidthBytes * y0) + x0;

    color = (WORD)lpLineData->color;
	//根据操作模式做不同的处理
    switch( lpLineData->rop )
    {
    case R2_COPYPEN:		//拷贝
		for( ; uLen; lpDestStart += xDir, uLen-- )
        {
            *lpDestStart = color;
        }
        break;
    case R2_NOT:			//非
		for( ; uLen; lpDestStart += xDir, uLen-- )
            *lpDestStart = ~*lpDestStart;
        break;
    case R2_XORPEN:			//异或
		for( ; uLen; lpDestStart += xDir, uLen-- )
            *lpDestStart ^= color;
        break;
    case R2_NOTXORPEN:			//NOT异或
		for( ; uLen; lpDestStart += xDir, uLen-- )
            *lpDestStart = ~(*lpDestStart ^ color);
        break;
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
static BOOL __ScanPatternLine( _LPLINEDATA lpLineData, int x0, int y0, int xDir, unsigned int uLen )
{
    register LPWORD lpDestStart;
    register WORD clrFore, clrBack;
    register BYTE pattern;
	UINT rop;
	//得到开始地址
	lpDestStart = (LPWORD)(lpLineData->lpDestImage->bmBits + lpLineData->lpDestImage->bmWidthBytes * y0) + x0;
	//前景色和背景色
    clrFore = (WORD)lpLineData->color;
    clrBack = (WORD)lpLineData->clrBack;

    pattern = lpLineData->pattern;
    if( (rop = lpLineData->rop) == R2_COPYPEN )
    {	//拷贝
		for( ; uLen; lpDestStart += xDir, x0 += xDir, uLen-- )
        {
            if( pattern & patternMask[ x0 & 0x07 ] )
                *lpDestStart = (WORD)clrFore;
            else
                *lpDestStart = (WORD)clrBack;
        }
    }
    else if( rop == R2_NOT )
    {	//非
		for( ; uLen; lpDestStart += xDir, x0 += xDir, uLen-- )
        {
            *lpDestStart = ~*lpDestStart;
        }
    }
    else if( rop == R2_XORPEN )
    {	//异或
		for( ; uLen; lpDestStart += xDir, x0 += xDir, uLen-- )
        {
            if( pattern & patternMask[x0&0x07] )
                *lpDestStart ^= clrFore;
            else
                *lpDestStart ^= clrBack;
        }
    }
    else if( rop == R2_NOTXORPEN )
    {	//异或
		for( ; uLen; lpDestStart += xDir, x0 += xDir, uLen-- )
        {
            if( pattern & patternMask[x0&0x07] )
                *lpDestStart = ~(*lpDestStart ^ clrFore);
            else
                *lpDestStart = ~(*lpDestStart ^ clrBack);
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
static BOOL __ScanTransparentLine( _LPLINEDATA lpLineData, int x0, int y0, int xDir, unsigned int uLen )
{
    register LPWORD lpDestStart;
    register WORD clrFore;
    register BYTE pattern;
	UINT rop;
	
	//得到目标地址
	lpDestStart = (LPWORD)(lpLineData->lpDestImage->bmBits + lpLineData->lpDestImage->bmWidthBytes * y0 ) + x0;

    clrFore = (WORD)lpLineData->color;
    
    pattern = lpLineData->pattern;
	//根据操作模式做不同的处理
    if( (rop = lpLineData->rop) == R2_COPYPEN )
    {	//拷贝
		for( ; uLen; lpDestStart += xDir, x0 += xDir, uLen-- )
        {
            if( pattern & patternMask[x0&0x07] )
                *lpDestStart = (WORD)clrFore;
        }
    }
    else if( rop == R2_NOT )
    {	//非
		for( ; uLen; lpDestStart += xDir, x0 += xDir, uLen-- )
        {
            *lpDestStart = ~*lpDestStart;
        }
    }
    else if( rop == R2_XORPEN )
    {	//异或
		for( ; uLen; lpDestStart += xDir, x0 += xDir, uLen-- )
        {
            if( pattern & patternMask[x0&0x07] )
                *lpDestStart ^= (WORD)clrFore;
        }
    }
    else if( rop == R2_NOTXORPEN )
    {	//NOT异或
		for( ; uLen; lpDestStart += xDir, x0 += xDir, uLen-- )
        {
            if( pattern & patternMask[x0&0x07] )
                *lpDestStart = ~( *lpDestStart ^ (WORD)clrFore );
        }
    }
    return TRUE;
}
/*
static BOOL _ScanLine( _LPLINEDATA lpData )
{
    if( lpData->pattern == 0xff )
    {
        return __ScanSolidLine( lpData );
    }
    else
    {
        if( lpData->backMode == TRANSPARENT )
            return __ScanTransparentLine( lpData );
        else
            return __ScanPatternLine( lpData );
    }
}
*/
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
static void __MoveBits( LPBYTE lpDest, int nBits, LPCBYTE lpSrc, WORD count )
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
    {
        while( count )
        {
            *lpDest++ = *lpSrc++;
            count--;
        }
    }
}

/*
BOOL _TextBitBlt( _LPBLKBITBLT lpData )
{
    LPBYTE lpDestStart, lpDest;
    LPCBYTE lpMaskStart, lpMask;
    WORD clFore, clBack;
    short i, j, rows, cols, shift;
    BOOL bEraseBack = (lpData->backMode == OPAQUE);
    BYTE mask, bitMask;


    lpDestStart = lpData->lpDestImage->bmBits + lpData->lprcDest->top * lpData->lpDestImage->bmWidthBytes + lpData->lprcDest->left;
//    lpMaskStart = lpData->lpMaskImage->bmBits + lpData->lprcMask->top * lpData->lpMaskImage->bmWidthBytes + (lpData->lprcMask->left >> 3);
    lpMaskStart = lpData->lpSrcImage->bmBits + lpData->lprcSrc->top * lpData->lpSrcImage->bmWidthBytes + (lpData->lprcSrc->left >> 3);
    rows = lpData->lprcDest->bottom - lpData->lprcDest->top;
    cols = lpData->lprcDest->right - lpData->lprcDest->left;
//    shift = lpData->lprcMask->left & 0x07;
    shift = lpData->lprcSrc->left & 0x07;
    clFore = lpData->solidColor;
    clBack = lpData->solidBkColor;

    for( i = 0; i < rows; i++ )
    {
        lpDest = lpDestStart;
        lpMask = lpMaskStart;

        mask = *lpMask++;
        bitMask = 0x80 >> shift;
        for( j = 0; j < cols; j++ )
        {
            if( bitMask == 0 )
            {
                mask = *lpMask++;
                bitMask = 0x80;
            }
            if( mask & bitMask )
            {
                *lpDest = clFore;
            }
            else if( bEraseBack )
            {
                *lpDest = clBack;
            }
            lpDest++;
            bitMask >>= 1;
        }
        lpDestStart += lpData->lpDestImage->bmWidthBytes;
//        lpMaskStart += lpData->lpMaskImage->bmWidthBytes;
        lpMaskStart += lpData->lpSrcImage->bmWidthBytes;
    }
    return TRUE;
}
*/

// *****************************************************************
//声明：static BOOL __BltFillTransparentPattern( _LPBLKBITBLT lpData )
//参数：
//	IN lpData - _BLKBITBLT结构指针
//返回值：
//	返回TRUE
//功能描述：
//	用纯色输出透明位模（当位模bit为1时，输出；否则不输出）
//引用: 
//	
// *****************************************************************
static BOOL __BltFillTransparentPattern( _LPBLKBITBLT lpData )
{
    LPWORD lpDestStart, lpDest;
    LPCBYTE lpPattern;
    WORD clrFore;
	int widthBytes;
    int i, j, n,  rows, cols, shift;
    BYTE mask, bitMask;
	//目标开始地址
    lpDestStart = (LPWORD)(lpData->lpDestImage->bmBits + lpData->lprcDest->top * lpData->lpDestImage->bmWidthBytes) + lpData->lprcDest->left;
    lpPattern = lpData->lpBrush->pattern;
    rows = lpData->lprcDest->bottom - lpData->lprcDest->top;
    cols = lpData->lprcDest->right - lpData->lprcDest->left;
    //shift = lpData->lprcMask->left & 0x07;
	if( lpData->lpptBrushOrg )
	{
		//shift = (lpData->lprcMask->left - lpData->lpptBrushOrg->x ) % 0x07;// + iWidthPat) % iWidthPat; 
		//n = ( lpData->lprcMask->top - lpData->lpptBrushOrg->y );
		shift = (lpData->lprcDest->left - lpData->lpptBrushOrg->x ) % 0x07;// + iWidthPat) % iWidthPat; 
		n = ( lpData->lprcDest->top - lpData->lpptBrushOrg->y );
	}
	else
	{
		n = shift = 0;
	}

    clrFore = (WORD)lpData->lpBrush->color;
    //n = lpData->lprcDest->top;
	//扫描行字节数
    widthBytes = lpData->lpDestImage->bmWidthBytes;
	//对每一行进行操作
    for( i = 0; i < rows; i++ )
    {
        mask = *(lpPattern+(n&0x07));
		n++;
        bitMask = 0x80 >> shift;
        lpDest = lpDestStart;
        for( j = 0; j < cols; j++ )
        {
            if( bitMask == 0 )
            {
                bitMask = 0x80;
            }
            if( mask & bitMask )
            {
                *lpDest = (WORD)clrFore;
            }
            lpDest++;

            bitMask >>= 1;
        }
        lpDestStart = (LPWORD)((LPBYTE)lpDestStart + widthBytes);
    }
    return TRUE;
}

#include "textalph.h"
                 
#undef  FUNCTION
#undef  BLT_ROP
#define FUNCTION __TextSrcCopy1601
#define BLT_ROP( bDst, bSrc )  ( (bSrc) )
//#include <16bpp\textblt.h>
#include "textblt.h"

#undef  FUNCTION
#undef  BLT_ROP
#define FUNCTION __TextNotSrcCopy1601
#define BLT_ROP( bDst, bSrc )  ( ~(bSrc) )
//#include <16bpp\textblt.h>
#include "textblt.h"

#undef  FUNCTION
#undef  BLT_ROP
#define FUNCTION __TextSrcInvert1601
#define BLT_ROP( bDst, bSrc )  ( (bDst) ^ (bSrc) )
//#include <16bpp\textblt.h>
#include "textblt.h"

#undef  FUNCTION
#undef  BLT_ROP
#define FUNCTION __TextSrcAnd1601
#define BLT_ROP( bDst, bSrc )  ( (bDst) & (bSrc) )
//#include <16bpp\textblt.h>
#include "textblt.h"

#undef  FUNCTION
#undef  BLT_ROP
#define FUNCTION __TextTransparentBlt1601
#define BLT_ROP( bDst, bSrc )  ( (bSrc) )
//#include <16bpp\txttpblt.h>
#include "txttpblt.h"


#undef  FUNCTION
#undef  BLT_ROP
#define FUNCTION __BltFillPattern
#define BLT_ROP( bDst, bSrc )  ( (bSrc) )
//#include <16bpp\bltfpat.h>
#include "bltfpat.h"


#undef  FUNCTION
#undef  BLT_ROP
#define FUNCTION __BltFillBitmapPattern
#define BLT_ROP( bDst, bSrc )  ( (bSrc) )
//#include <16bpp\bltfpat.h>
#include "bltfpat16.h"


#undef  FUNCTION
#undef  BLT_ROP
#define FUNCTION __BltPatInvertPattern
#define BLT_ROP( bDst, bSrc ) ( (bDst) ^ (bSrc) )
//#include <16bpp\bltfpat.h>
#include "bltfpat.h"


#undef  FUNCTION
#undef  BLT_ROP
#define FUNCTION __BltMergeCopy
#define BLT_ROP( bSrc, bBrush ) ( (bSrc) & (bBrush) )
#include "bltmpat.h"


//#undef  FUNCTION
//#undef  BLT_ROP
//#define FUNCTION __BltSrcCopyMono
//#define BLT_ROP( bDst, bSrc )  ( (bSrc) )
//#include <bltcpy1.h>

#undef  FUNCTION
#undef  BLT_ROP
#define FUNCTION __BltSrcCopy
#define BLT_ROP( bDst, bSrc )  ( (bSrc) )
//#include <16bpp\bltcpy.h>
#ifdef ARM_CPU
#define FAST_OP 
#endif
#include "bltcpy.h"
#undef FAST_OP 

#undef  FUNCTION
#undef  BLT_ROP
#define FUNCTION __BltSrcAnd1616
#define BLT_ROP( bDst, bSrc )  ( (bDst) & (bSrc) )
//#include <16bpp\bltcpy.h>
#include "bltcpy.h"

#undef  FUNCTION
#undef  BLT_ROP
#define FUNCTION __BltNotSrcCopy1616
#define BLT_ROP( bDst, bSrc )  ( ~(bSrc) )
//#include <16bpp\bltcpy.h>
#include "bltcpy.h"

#undef  FUNCTION
#undef  BLT_ROP
#define FUNCTION __BltSrcInvert1616
#define BLT_ROP( bDst, bSrc )  ( (bDst) ^ (bSrc) )
//#include <16bpp\bltcpy.h>
#include "bltcpy.h"

#undef  FUNCTION
#undef  BLT_ROP
#define FUNCTION __BltSrcPaint
#define BLT_ROP( bDst, bSrc )  ( (bDst) | (bSrc) )
//#include <16bpp\bltcpy.h>
#include "bltcpy.h"

#undef  FUNCTION
#undef  BLT_ROP
#define FUNCTION __BltMergePaint
#define BLT_ROP( bDst, bSrc )  ( (bDst) | (~(bSrc)) )
#include "bltcpy.h"



#undef  FUNCTION
#undef  BLT_ROP
#define FUNCTION __BltFillSolid
#define BLT_ROP( bDst, bColor ) ( (bColor) )
//#include <16bpp\bltfill.h>
#include "bltfill.h"

#undef  FUNCTION
#undef  BLT_ROP
#define FUNCTION __BltPatInvertSolid
#define BLT_ROP( bDst, bSrc ) ( (bDst) ^ (bSrc) )
//#include <16bpp\bltfill.h>
#include "bltfill.h"

#undef  FUNCTION
#undef  BLT_ROP
#define FUNCTION __BltDstInvert
#define BLT_ROP( bDst, bSrc ) (~(bDst))
//#include <16bpp\bltfill.h>
#include "bltfill.h"

// *****************************************************************
//声明：static BOOL _BlkBitTransparentBlt( _LPBLKBITBLT lpData )
//参数：
//	IN lpData - _BLKBITBLT结构指针
//返回值：
//	返回TRUE
//功能描述：
//	透明输出位模（当位模bit为1时，输出；否则不输出）
//引用: 
//	
// *****************************************************************
static BOOL _BlkBitTransparentBlt( _LPBLKBITBLT lpData )
{
    if( lpData->lpBrush )
        return __BltFillTransparentPattern( lpData );
    else if( lpData->lpSrcImage->bmBitsPixel == 1 )
    {
        return __TextTransparentBlt1601( lpData );
    }
    else if( lpData->lpSrcImage->bmBitsPixel == 8 )
    {
        return __TextAlpha1616( lpData, FALSE );    
	}
	
    return FALSE;
}

//#undef FUNCTION
//#undef SRC_ROP
//#define SRC_ROP( bSrc ) ((bSrc))
//#define FUNCTION __BltMaskSrcCopy
//#include <8bpp\bltmask.h>
#undef FUNCTION
#undef MASK_BLT_ROP
#define MASK_BLT_ROP( bDest, bSrc, bMask ) ( (bMask) ? (bSrc) : ( (bSrc) ^ (bDest) ) )
#define FUNCTION __BltMaskSrcCopySrcInvert
//#include <16bpp\bltmask.h>
#include "bltmask.h"

//#undef FUNCTION
//#undef MASK_BLT_ROP
//#define MASK_BLT_ROP( bDest, bSrc, bMask ) ( (bMask) ? (bDest) : (bSrc) )
//#define FUNCTION __BltInvertMaskSrcCopy
//#include <16bpp\bltmask.h>

//#undef FUNCTION
//#undef SRC_ROP
//#define SRC_ROP( bSrc ) ((bSrc))
//#define FUNCTION __BltMaskSrcBit1Copy
//#include <8bpp\bltmask1.h>

#undef FUNCTION
#undef MASK_BLT_ROP
#define MASK_BLT_ROP( bDest, bSrc, bMask ) ((bMask) ? (bSrc) : ( (bSrc) ^ (bDest) ) )
#define FUNCTION __BltMaskBit1SrcCopySrcInvert
//#include <16bpp\bltmask1.h>
#include "bltmask1.h"

//#undef FUNCTION
//#undef MASK_BLT_ROP
//#define MASK_BLT_ROP( bDest, bSrc, bMask ) ( (bMask) ? (bDest) : (bSrc))
//#define FUNCTION __BltInvertMaskSrcBit1Copy
//#include <16bpp\bltmask1.h>

#undef FUNCTION
#undef MASK_BLT_ROP
#define MASK_BLT_ROP( bDest, bSrc, bMask ) ( (bMask) ? (bSrc) : (bDest) )
#define FUNCTION __BltMaskSrcCopyDestCopy
//#include <16bpp\bltmask.h>
#include "bltmask.h"

#undef FUNCTION
#undef MASK_BLT_ROP
#define MASK_BLT_ROP( bDest, bSrc, bMask ) ( (bMask) ? (bSrc) : (bDest) )
#define FUNCTION __BltMaskBit1SrcCopyDestCopy
//#include <16bpp\bltmask1.h>
#include "bltmask1.h"

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
    if( lpData->lpMaskImage->bmBitsPixel == 1 )
    {
        if( lpData->dwRop == MAKEROP4(SRCCOPY, SRCINVERT) )
        {
            if( lpData->lpSrcImage->bmPlanes == 1 && lpData->lpSrcImage->bmBitsPixel == 1 )
            {
                return __BltMaskBit1SrcCopySrcInvert( lpData );
            }
            if( lpData->lpSrcImage->bmPlanes == 1 && lpData->lpSrcImage->bmBitsPixel == 16 )
                return __BltMaskSrcCopySrcInvert( lpData );
        }
        else if( lpData->dwRop == MAKEROP4(SRCCOPY, 0) )
        {
            if( lpData->lpSrcImage->bmPlanes == 1 && lpData->lpSrcImage->bmBitsPixel == 16 )
                return __BltMaskSrcCopyDestCopy( lpData );
            if( lpData->lpSrcImage->bmPlanes == 1 && lpData->lpSrcImage->bmBitsPixel == 1 )
                return __BltMaskBit1SrcCopyDestCopy( lpData );
        }
/*
        else if( lpData->dwRop == 1000 )
        {
            if( lpData->lpSrcImage->bmPlanes == 1 && lpData->lpSrcImage->bmBitsPixel == 1 )
            {
                return __BltInvertMaskSrcBit1Copy( lpData );
            }
            if( lpData->lpSrcImage->bmPlanes == 1 && lpData->lpSrcImage->bmBitsPixel == 16 )
                return __BltInvertMaskSrcCopy( lpData );
        }
*/
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
	case SRCCOPY:
		if( lpData->lpSrcImage->bmBitsPixel == 1 )  // mono bitmap
		{
			return __TextSrcCopy1601( lpData );
		}
		else if( lpData->lpSrcImage->bmBitsPixel == 16 )
			return __BltSrcCopy( lpData );
		else if( lpData->lpSrcImage->bmBitsPixel == 8 )
			return __TextAlpha1616( lpData, TRUE );
	case SRCAND:
		if( lpData->lpSrcImage->bmBitsPixel == 1 )  // mono bitmap
		{
			return __TextSrcAnd1601( lpData );
		}
		else if( lpData->lpSrcImage->bmBitsPixel == 16 )
			return __BltSrcAnd1616( lpData );
	case SRCINVERT:
		if( lpData->lpSrcImage->bmBitsPixel == 1 )  // mono bitmap
		{
			return __TextSrcInvert1601( lpData );
		}
		else if( lpData->lpSrcImage->bmBitsPixel == 16 )
			return __BltSrcInvert1616( lpData );
	case SRCPAINT:
		return __BltSrcPaint( lpData );
	case PATCOPY:
		if( lpData->lpBrush == 0 )
		{				
			return __BltFillSolid( lpData );
		}
		else
		{
			UINT style;
			if( (style = lpData->lpBrush->style) == BS_SOLID )
			{
				lpData->solidColor = lpData->lpBrush->color;
				return __BltFillSolid( lpData );
			}
			else if( style == BS_HATCHED )
				return __BltFillPattern( lpData );
			else if( style == BS_PATTERN )
			{
				return __BltFillBitmapPattern( lpData );
			}
		}
		break;
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
	case DSTINVERT:
		return __BltDstInvert( lpData );
	case NOTSRCCOPY:
		if( lpData->lpSrcImage->bmBitsPixel == 1 )  // mono bitmap
		{
			return __TextNotSrcCopy1601( lpData );
		}
		else if( lpData->lpSrcImage->bmBitsPixel == 16 )
			return __BltNotSrcCopy1616( lpData );
	case MERGECOPY:
		if( lpData->lpBrush &&
			lpData->lpSrcImage &&
			lpData->lpSrcImage->bmBitsPixel == 16 )
		{
			return __BltMergeCopy( lpData );
		}
	case MERGEPAINT:
		if( lpData->lpSrcImage )
			return __BltMergePaint( lpData );
	}
    return FALSE;
}
