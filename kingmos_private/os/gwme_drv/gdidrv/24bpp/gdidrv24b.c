/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/


/*****************************************************
文件说明：图形设备驱动程序 24bit(3byte) / pixel
版本号：1.0.0
开发时期：2004-05-12
作者：李林
修改记录：

******************************************************/

#include <eframe.h>
#include <gwmeobj.h>

//#ifndef COLOR_8BPP
//#error not define COLOR_8BPP in file version.h
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

#define PIXEL_BYTES 3
#define GET_LINE_OFFSET( x ) ( (x) * PIXEL_BYTES )


//#define SET_PIXEL( lpAdr, color ) \
//{ \
	//*( (LPBYTE)lpAdr ) = *( (LPBYTE)&(color) ); \
	//*( (LPBYTE)lpAdr + 1 ) = *( ( (LPBYTE)&(color) ) + 1 ); \
	//*( (LPBYTE)lpAdr + 2 ) = *( ( (LPBYTE)&(color) ) + 2 ); \
//}

//#define GET_PIXEL( lpAdr, color ) \
//{ \    
	//*( (LPBYTE)&(color) ) = *( (LPBYTE)(lpAdr) );  \
	//*( ( (LPBYTE)&(color) ) + 1 ) = *( (LPBYTE)(lpAdr) + 1 ); \
	//*( ( (LPBYTE)&(color) ) + 2 ) = *( (LPBYTE)(lpAdr) + 2 ); \
//}


//申明图形设备驱动程序接口对象
const _DISPLAYDRV _drvDisplay24BPP = {
    _PutPixel,
    _GetPixel,
    _Line,
    _BlkBitTransparentBlt,
    _BlkBitMaskBlt,
    _BlkBitBlt,
    _RealizeColor,
    _UnrealizeColor
};

//
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
		BYTE r = ((BYTE)color) & 0xff;
		BYTE g = ((BYTE)(color >> 8)) & 0xff;
		BYTE b = ((BYTE)(color >> 16)) & 0xff;
		*((LPBYTE)&color) = b;
		*((LPBYTE)&color+1) = g;
		*((LPBYTE)&color+2) = r;
		return color;    
	}
	//return (((DWORD)r) << 16) | (((DWORD)g) << 8) | b;
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
		BYTE b = *((LPBYTE)&color);
		BYTE g = *((LPBYTE)&color+1);
		BYTE r = *((LPBYTE)&color+2);
		return RGB( r, g, b );	
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
    COLORREF cRet,cCur;
	UINT rop;
	//是否需要真实输出 ？
	if( (rop = lpPixelData->rop) != R2_NOP )
	{	//需要输出
		//得到x 位置/地址
		lpVideoAdr = (LPBYTE)(lpPixelData->lpDestImage->bmBits + lpPixelData->y * lpPixelData->lpDestImage->bmWidthBytes) + GET_LINE_OFFSET( lpPixelData->x );//* 3;
		//得到当前/返回的颜色值
		//GET_PIXEL( lpVideoAdr, cRet );
		*((LPBYTE)&cRet) = *lpVideoAdr;
		*((LPBYTE)&cRet+1) = *(lpVideoAdr+1);
		*((LPBYTE)&cRet+2) = *(lpVideoAdr+2);
		//根据操作模式做相关动作
		cCur = lpPixelData->color;
		switch( rop )
		{
		case R2_COPYPEN:		//拷贝到目标
			//*lpVideoAdr = (BYTE)lpPixelData->color;
			*lpVideoAdr = *( (LPBYTE)&cCur );
			*( lpVideoAdr+1 ) = *( (LPBYTE)&cCur + 1 );
			*( lpVideoAdr+2 ) = *( (LPBYTE)&cCur + 2 );
			break;
		case R2_XORPEN:			//异或
			//*lpVideoAdr ^= (BYTE)lpPixelData->color;
			*lpVideoAdr ^= *( (LPBYTE)&cCur );
			*( lpVideoAdr+1 ) ^= *( (LPBYTE)&cCur + 1 );
			*( lpVideoAdr+2 ) ^= *( (LPBYTE)&cCur + 2 );
			break;
		case R2_NOT:			//非
			//*lpVideoAdr = ~*lpVideoAdr;
			*lpVideoAdr = ~*( (LPBYTE)&cCur );
			*( lpVideoAdr+1 ) = ~*( (LPBYTE)&cCur + 1 );
			*( lpVideoAdr+2 ) = ~*( (LPBYTE)&cCur + 2 );
			break;
		case R2_BLACK:			//黑
			//*lpVideoAdr = 0;
			*lpVideoAdr = 0;
			*( lpVideoAdr+1 ) = 0;
			*( lpVideoAdr+2 ) = 0;
			break;
		case R2_WHITE:			//白
			//*lpVideoAdr = 0xff;
			*lpVideoAdr = 0xff;
			*( lpVideoAdr+1 ) = 0xff;
			*( lpVideoAdr+2 ) = 0xff;
			break;
		case R2_MASKPEN:   // dest = dest-pixel-color and pen-color
			//*lpVideoAdr &= (BYTE)lpPixelData->color;
			*lpVideoAdr &= *( (LPBYTE)&cCur );
			*( lpVideoAdr+1 ) &= *( (LPBYTE)&cCur + 1 );
			*( lpVideoAdr+2 ) &= *( (LPBYTE)&cCur + 2 );
			break;
		case R2_MERGEPEN:  // dest = dest-pixel-color or pen-color
			//*lpVideoAdr |= (BYTE)lpPixelData->color;
			*lpVideoAdr |= *( (LPBYTE)&cCur );
			*( lpVideoAdr+1 ) |= *( (LPBYTE)&cCur + 1 );
			*( lpVideoAdr+2 ) |= *( (LPBYTE)&cCur + 2 );
			break;
		case R2_MERGENOTPEN:
			//*lpVideoAdr |= ~(BYTE)lpPixelData->color;
			*lpVideoAdr |= ~*( (LPBYTE)&cCur );
			*( lpVideoAdr+1 ) |= ~*( (LPBYTE)&cCur + 1 );
			*( lpVideoAdr+2 ) |= ~*( (LPBYTE)&cCur + 2 );
			break;
		case R2_NOTCOPYPEN:
			//*lpVideoAdr = ~(BYTE)lpPixelData->color;
			*lpVideoAdr = ~*( (LPBYTE)&cCur );
			*( lpVideoAdr+1 ) = ~*( (LPBYTE)&cCur + 1 );
			*( lpVideoAdr+2 ) = ~*( (LPBYTE)&cCur + 2 );
			break;
		case R2_NOTXORPEN:
			*lpVideoAdr = ~( *lpVideoAdr ^ *( (LPBYTE)&cCur ) );
			lpVideoAdr++;
			*lpVideoAdr = ~( *lpVideoAdr ^ *( (LPBYTE)&cCur + 1 ) );
			lpVideoAdr++;
			*lpVideoAdr = ~( *lpVideoAdr ^ *( (LPBYTE)&cCur + 2 ) );
			break;
		}
	}
	else
		cRet = -1;
    return cRet;
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
    LPBYTE lpbAdr = (lpPixelData->lpDestImage->bmBits + lpPixelData->y * lpPixelData->lpDestImage->bmWidthBytes + GET_LINE_OFFSET( lpPixelData->x ) );
	COLORREF color;
	*((LPBYTE)&color) = *lpbAdr;
	*((LPBYTE)&color+1) = *(lpbAdr+1);
	*((LPBYTE)&color+2) = *(lpbAdr+2);
	return color;    
   
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
static BOOL __VertialSolidLine( _LPLINEDATA lpLineData, int x0, int y0, int yDir, UINT uLen )
{
    LPBYTE lpDest;
    int widthBytes;
	UINT sum; 
    DWORD color;
	//扫描行宽度(字节数)
    widthBytes = lpLineData->lpDestImage->bmWidthBytes;
	//目标地址
	lpDest = lpLineData->lpDestImage->bmBits + y0 * widthBytes + GET_LINE_OFFSET( x0 );
    color = lpLineData->color;
    
    sum = uLen;
	//正向或反向
	if( yDir < 0 )
        widthBytes = -widthBytes;	//反向
	//根据操作模式做不同的处理
    if( lpLineData->rop == R2_COPYPEN )
    {	//拷贝
        while( sum-- )
        {
            //*lpDest = color;
			*lpDest = *( (LPBYTE)&color );
			*( lpDest+1 ) = *( (LPBYTE)&color + 1 );
			*( lpDest+2 ) = *( (LPBYTE)&color + 2 );

            lpDest += widthBytes;
        }
    }
    else if( lpLineData->rop == R2_NOT )
    {	//非
        while( sum-- )
        {
            //*lpDest = ~*lpDest;
			*lpDest = ~*lpDest;
			*( lpDest+1 ) = ~*( lpDest+1 );
			*( lpDest+2 ) = ~*( lpDest+2 );


            lpDest += widthBytes;
        }
    }
    else if( lpLineData->rop == R2_XORPEN )
    {	//异或
        while( sum-- )
        {
            //*lpDest = *lpDest ^ color;
			*lpDest = *lpDest ^ *( (LPBYTE)&color );
			*( lpDest+1 ) = *( lpDest+1 ) ^ *( (LPBYTE)&color + 1 );
			*( lpDest+2 ) = *( lpDest+2 ) ^ *( (LPBYTE)&color + 2 );

            lpDest += widthBytes;
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
static BOOL __VertialPatternLine( _LPLINEDATA lpLineData, int x0, int y0, int yDir, UINT uLen )
{
    LPBYTE lpDest;
    int widthBytes;
	UINT sum;
    DWORD clrFore, clrBack;
    BYTE pattern;
	//扫描行宽度(字节数)
    widthBytes = lpLineData->lpDestImage->bmWidthBytes;
	//目标地址
	lpDest = lpLineData->lpDestImage->bmBits + y0 * widthBytes + GET_LINE_OFFSET( x0 );
	//前景和背景颜色
    clrFore = lpLineData->color;
    clrBack = lpLineData->clrBack;

    pattern = lpLineData->pattern;
	sum = uLen;
	//正向或反向
	if( yDir < 0 )
		widthBytes = -widthBytes;	//反向
	//根据操作模式做不同的处理
    if( lpLineData->rop == R2_COPYPEN )
    {	//拷贝
        while( sum-- )
        {
            if( pattern & patternMask[ y0 & 0x07 ] )
            {
                //*lpDest = (BYTE)clrFore;
				*lpDest = *( (LPBYTE)&clrFore );
				*( lpDest+1 ) = *( (LPBYTE)&clrFore + 1 );
				*( lpDest+2 ) = *( (LPBYTE)&clrFore + 2 );
				
            }
            else
            {
                //*lpDest = (BYTE)clrBack;
				*lpDest = *( (LPBYTE)&clrBack );
				*( lpDest+1 ) = *( (LPBYTE)&clrBack + 1 );
				*( lpDest+2 ) = *( (LPBYTE)&clrBack + 2 );
            }
            lpDest += widthBytes;
            y0 += yDir;
        }
    }
    else if( lpLineData->rop == R2_XORPEN )
    {	//异或
        while( sum-- )
        {
            if( pattern & patternMask[ y0 & 0x07 ] )
            {
                //*lpDest ^= clrFore;
				*lpDest ^= *( (LPBYTE)&clrFore );
				*( lpDest+1 ) ^= *( (LPBYTE)&clrFore + 1 );
				*( lpDest+2 ) ^= *( (LPBYTE)&clrFore + 2 );
            }
            else
            {
                //*lpDest ^= clrBack;
				*lpDest ^= *( (LPBYTE)&clrBack );
				*( lpDest+1 ) ^= *( (LPBYTE)&clrBack + 1 );
				*( lpDest+2 ) ^= *( (LPBYTE)&clrBack + 2 );

            }
            lpDest += widthBytes;
            y0 += yDir;
        }
    }
    else if( lpLineData->rop == R2_NOT )
    {	//非
        while( sum-- )
        {
            //*lpDest = ~*lpDest;
			*lpDest = ~*( (LPBYTE)&clrBack );
			*( lpDest+1 ) = ~*( (LPBYTE)&clrBack + 1 );
			*( lpDest+2 ) = ~*( (LPBYTE)&clrBack + 2 );

            lpDest += widthBytes;
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
static BOOL __VertialTransparentLine( _LPLINEDATA lpLineData, int x0, int y0, int yDir, UINT uLen )
{
    LPBYTE lpDest;
    int widthBytes;
	UINT sum;
    DWORD clrFore;
    BYTE pattern;
	//扫描行宽度(字节数)
    widthBytes = lpLineData->lpDestImage->bmWidthBytes;
	//目标地址
	lpDest = lpLineData->lpDestImage->bmBits + y0 * widthBytes + GET_LINE_OFFSET( x0 );

    clrFore = lpLineData->color;
    pattern = lpLineData->pattern;

	sum = uLen;
	//正向或反向
	if( yDir < 0 )
		widthBytes = -widthBytes;
	//根据操作模式做不同的处理		
    if( lpLineData->rop == R2_COPYPEN )
    {	//拷贝
        while( sum-- )
        {
            if( pattern & patternMask[y0&0x07] )
			{
                //*lpDest = (BYTE)clrFore;
				*lpDest = *( (LPBYTE)&clrFore );
				*( lpDest+1 ) = *( (LPBYTE)&clrFore + 1 );
				*( lpDest+2 ) = *( (LPBYTE)&clrFore + 2 );
			}
            lpDest += widthBytes;
            y0 += yDir;
        }
    }
    else if( lpLineData->rop == R2_XORPEN )
    {	//异或
        while( sum-- )
        {
            if( pattern & patternMask[y0&0x07] )
			{
                //*lpDest ^= (BYTE)clrFore;
				*lpDest ^= *( (LPBYTE)&clrFore );
				*( lpDest+1 ) ^= *( (LPBYTE)&clrFore + 1 );
				*( lpDest+2 ) ^= *( (LPBYTE)&clrFore + 2 );
			}
            lpDest += widthBytes;
            y0 += yDir;
        }
    }
    else if( lpLineData->rop == R2_NOT )
    {	//非
        while( sum-- )
        {
            //*lpDest = ~*lpDest;
			*lpDest = ~*lpDest;
			*( lpDest+1 ) = ~*( lpDest+1 );
			*( lpDest+2 ) = ~*( lpDest+2 );

            lpDest += widthBytes;
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
static BOOL __ScanSolidLine( _LPLINEDATA lpLineData, int x0, int y0, int xDir, UINT uLen )
{
    LPBYTE lpDestStart, lpDestEnd;
    DWORD color;
	//得到开始地址
    lpDestStart = lpLineData->lpDestImage->bmBits + y0 * lpLineData->lpDestImage->bmWidthBytes + GET_LINE_OFFSET( x0 );
	if( xDir > 0 )
    {	//正向
		lpDestEnd = lpDestStart + uLen;		
    }
    else
    {	//反向
		lpDestEnd = lpDestStart - uLen;
    }
	xDir *= 3;
	
    color = lpLineData->color;
    //根据操作模式做不同的处理
	switch( lpLineData->rop )
    {
    case R2_COPYPEN:
        for( ; lpDestStart != lpDestEnd; lpDestStart += xDir )
		{
            //*lpDestStart = color;
			*lpDestStart = *( (LPBYTE)&color );
			*( lpDestStart+1 ) = *( (LPBYTE)&color + 1 );
			*( lpDestStart+2 ) = *( (LPBYTE)&color + 2 );
		}
        break;
    case R2_XORPEN:
        for( ; lpDestStart != lpDestEnd; lpDestStart += xDir )
		{
            //*lpDestStart ^= color;
			*lpDestStart ^= *( (LPBYTE)&color );
			*( lpDestStart+1 ) ^= *( (LPBYTE)&color + 1 );
			*( lpDestStart+2 ) ^= *( (LPBYTE)&color + 2 );
		}
        break;
    case R2_NOT:
        for( ; lpDestStart != lpDestEnd; lpDestStart += xDir )
		{
            //*lpDestStart = ~*lpDestStart;
			*lpDestStart = ~*lpDestStart;
			*( lpDestStart + 1 ) = ~*( lpDestStart + 1 );
			*( lpDestStart + 2 ) = ~*( lpDestStart + 2 );
		}
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
static BOOL __ScanPatternLine( _LPLINEDATA lpLineData, int x0, int y0, int xDir, UINT uLen )
{
    LPBYTE lpDestStart, lpDestEnd;
	int x, xByteOffset;
    DWORD clrFore, clrBack, color;
    BYTE pattern;    
	//得到开始地址
    lpDestStart = lpLineData->lpDestImage->bmBits + y0 * lpLineData->lpDestImage->bmWidthBytes + GET_LINE_OFFSET( x0 );
	if( xDir > 0 )
    {	//正向
		lpDestEnd = lpDestStart + uLen;
    }
    else
    {	//反向
		lpDestEnd = lpDestStart - uLen;
    }
    //前景色
    clrFore = lpLineData->color;
	//背景色
    clrBack = lpLineData->clrBack;
	x = x0;
    pattern = lpLineData->pattern;
	xByteOffset = xDir * 3;  //三个字节
	//根据操作模式做不同的处理
    if( lpLineData->rop == R2_COPYPEN )
    {
        for( ; lpDestStart != lpDestEnd; lpDestStart += xByteOffset, x += xDir )
        {
            if( pattern & patternMask[ x & 0x07 ] )
			{
				color = clrFore;
                //*lpDestStart = (BYTE)clrFore;
			}
            else
			{
                //*lpDestStart = (BYTE)clrBack;
				color = clrBack;
			}
			*lpDestStart = *( (LPBYTE)&color );
			*( lpDestStart+1 ) = *( (LPBYTE)&color + 1 );
			*( lpDestStart+2 ) = *( (LPBYTE)&color + 2 );
        }
    }
    else if( lpLineData->rop == R2_XORPEN )
    {
        for( ; lpDestStart != lpDestEnd; lpDestStart += xByteOffset, x += xDir )
        {
            if( pattern & patternMask[x&0x07] )
			{
                //*lpDestStart ^= clrFore;
				color = clrFore;
			}
            else
			{
                //*lpDestStart ^= clrBack;
				color = clrBack;
			}
			*lpDestStart ^= *( (LPBYTE)&color );
			*( lpDestStart+1 ) ^= *( (LPBYTE)&color + 1 );
			*( lpDestStart+2 ) ^= *( (LPBYTE)&color + 2 );
        }
    }
    else if( lpLineData->rop == R2_NOT )
    {
        for( ; lpDestStart != lpDestEnd; lpDestStart += xByteOffset, x += xDir )
        {
            //*lpDestStart = ~*lpDestStart;
			*lpDestStart = ~*lpDestStart;
			*( lpDestStart + 1 ) = ~*( lpDestStart + 1 );
			*( lpDestStart + 2 ) = ~*( lpDestStart + 2 );
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

static BOOL __ScanTransparentLine( _LPLINEDATA lpLineData, int x0, int y0, int xDir, UINT uLen )
{
    LPBYTE lpDestStart, lpDestEnd;
	int x, xByteOffset;
    DWORD clrFore;
    BYTE pattern;

	//得到目标地址
    lpDestStart = lpLineData->lpDestImage->bmBits + y0 * lpLineData->lpDestImage->bmWidthBytes + GET_LINE_OFFSET( x0 );

	if( xDir > 0 )
    {	//正向
		lpDestEnd = lpDestStart + uLen;
    }
    else
    {	//反向
		lpDestEnd = lpDestStart - uLen;
    }
    //前景色
    clrFore = lpLineData->color;
	x = x0;
	xByteOffset = xDir * 3;
    pattern = lpLineData->pattern;
	//根据操作模式做不同的处理
    if( lpLineData->rop == R2_COPYPEN )
    {	//拷贝
        for( ; lpDestStart != lpDestEnd; lpDestStart += xByteOffset, x += xDir )
        {
            if( pattern & patternMask[ x & 0x07 ] )
			{
                //*lpDestStart = (BYTE)clrFore;
				*lpDestStart = *( (LPBYTE)&clrFore );
				*( lpDestStart+1 ) = *( (LPBYTE)&clrFore + 1 );
				*( lpDestStart+2 ) = *( (LPBYTE)&clrFore + 2 );
			}
        }
    }
    else if( lpLineData->rop == R2_XORPEN )
    {	//异或
        for( ; lpDestStart != lpDestEnd; lpDestStart += xByteOffset, x += xDir )
        {
            if( pattern & patternMask[ x & 0x07 ] )
			{
                //*lpDestStart ^= (BYTE)clrFore;
				*lpDestStart ^= *( (LPBYTE)&clrFore );
				*( lpDestStart+1 ) ^= *( (LPBYTE)&clrFore + 1 );
				*( lpDestStart+2 ) ^= *( (LPBYTE)&clrFore + 2 );
			}
        }
    }
    else if( lpLineData->rop == R2_NOT )
    {	//非
        for( ; lpDestStart != lpDestEnd; lpDestStart += xByteOffset, x += xDir )
        {
            //*lpDestStart = ~*lpDestStart;
			*lpDestStart = ~*lpDestStart;
			*( lpDestStart + 1 ) = ~*( lpDestStart + 1 );
			*( lpDestStart + 2 ) = ~*( lpDestStart + 2 );
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
    LPBYTE lpDestStart, lpDest;
    LPCBYTE lpPattern;
    DWORD clrFore;
    int i, j, n,  rows, cols, shift;
    BYTE mask, bitMask;
	//目标开始地址
    lpDestStart = lpData->lpDestImage->bmBits + lpData->lprcDest->top * lpData->lpDestImage->bmWidthBytes + GET_LINE_OFFSET( lpData->lprcDest->left );
    lpPattern = lpData->lpBrush->pattern;
    rows = lpData->lprcDest->bottom - lpData->lprcDest->top;
    cols = lpData->lprcDest->right - lpData->lprcDest->left;
    //shift = lpData->lprcMask->left & 0x07;
	//刷子前景色
    clrFore = lpData->lpBrush->color;
    //n = lpData->lprcDest->top;

	if( lpData->lpptBrushOrg )
	{
		//shift = (lpData->lprcMask->left - lpData->lpptBrushOrg->x ) % 0x07;// + iWidthPat) % iWidthPat; 
		//n = lpData->lprcMask->top - lpData->lpptBrushOrg->y;
		shift = (lpData->lprcDest->left - lpData->lpptBrushOrg->x ) % 0x07;// + iWidthPat) % iWidthPat; 
		n = lpData->lprcDest->top - lpData->lpptBrushOrg->y;
	}
	else
	{
		shift = n = 0;
	}
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
                //*lpDest = (BYTE)clrFore;
				*lpDest = *( (LPBYTE)&clrFore );
				*( lpDest+1 ) = *( (LPBYTE)&clrFore + 1 );
				*( lpDest+2 ) = *( (LPBYTE)&clrFore + 2 );

            }
            lpDest+=PIXEL_BYTES;

            bitMask >>= 1;
        }
		//下一行
        lpDestStart += lpData->lpDestImage->bmWidthBytes;
    }
    return TRUE;
}

                 
#undef  FUNCTION
#undef  BLT_ROP
#define FUNCTION __TextSrcCopy2401
#define BLT_ROP( bDst, bSrc )  ( (bSrc) )
#include "textblt.h"

#undef  FUNCTION
#undef  BLT_ROP
#define FUNCTION __TextNotSrcCopy2401
#define BLT_ROP( bDst, bSrc )  ( ~(bSrc) )
#include "textblt.h"

#undef  FUNCTION
#undef  BLT_ROP
#define FUNCTION __TextSrcInvert2401
#define BLT_ROP( bDst, bSrc )  ( (bDst) ^ (bSrc) )
#include "textblt.h"

#undef  FUNCTION
#undef  BLT_ROP
#define FUNCTION __TextSrcAnd2401
#define BLT_ROP( bDst, bSrc )  ( (bDst) & (bSrc) )
#include "textblt.h"

#undef  FUNCTION
#undef  BLT_ROP
#define FUNCTION __TextTransparentBlt2401
#define BLT_ROP( bDst, bSrc )  ( (bSrc) )
#include "txttpblt.h"

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
#include "bltcpy.h"

#undef  FUNCTION
#undef  BLT_ROP
#define FUNCTION __BltSrcAnd2424
#define BLT_ROP( bDst, bSrc )  ( (bDst) & (bSrc) )
#include "bltcpy.h"

#undef  FUNCTION
#undef  BLT_ROP
#define FUNCTION __BltNotSrcCopy2424
#define BLT_ROP( bDst, bSrc )  ( ~(bSrc) )
#include "bltcpy.h"

#undef  FUNCTION
#undef  BLT_ROP
#define FUNCTION __BltSrcInvert2424
#define BLT_ROP( bDst, bSrc )  ( (bDst) ^ (bSrc) )
#include "bltcpy.h"

#undef  FUNCTION
#undef  BLT_ROP
#define FUNCTION __BltSrcPaint
#define BLT_ROP( bDst, bSrc )  ( (bDst) | (bSrc) )
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
        return __TextTransparentBlt2401( lpData );
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
#include "bltmask.h"

//#include <8bpp\bltmask.h>
#undef FUNCTION
#undef MASK_BLT_ROP
#define MASK_BLT_ROP( bDest, bSrc, bMask ) ( (bMask) ? (bSrc) : (bDest) )
#define FUNCTION __BltMaskSrcCopyDestCopy
#include "bltmask.h"

/*  // the code is no use now, reverse inner, no standard
#undef FUNCTION
#undef MASK_BLT_ROP
#define MASK_BLT_ROP( bDest, bSrc, bMask ) ( (bMask) ? (bDest) : (bSrc) )
#define FUNCTION __BltInvertMaskSrcCopy
#include <8bpp\bltmask.h>

//#undef FUNCTION
//#undef SRC_ROP
//#define SRC_ROP( bSrc ) ((bSrc))
//#define FUNCTION __BltMaskSrcBit1Copy
//#include <8bpp\bltmask1.h>


#undef FUNCTION
#undef MASK_BLT_ROP
#define MASK_BLT_ROP( bDest, bSrc, bMask ) ( (bMask) ? (bDest) : (bSrc))
#define FUNCTION __BltInvertMaskSrcBit1Copy
#include <8bpp\bltmask1.h>
*/

#undef FUNCTION
#undef MASK_BLT_ROP
#define MASK_BLT_ROP( bDest, bSrc, bMask ) ( (bMask) ? (bSrc) : ( (bSrc) ^ (bDest) ) )
#define FUNCTION __BltMaskBit1SrcCopySrcInvert
#include "bltmask1.h"

#undef FUNCTION
#undef MASK_BLT_ROP
#define MASK_BLT_ROP( bDest, bSrc, bMask ) ( (bMask) ? (bSrc) : (bDest) )
#define FUNCTION __BltMaskBit1SrcCopyDestCopy
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
    {	//黑白
        if( lpData->dwRop == MAKEROP4(SRCCOPY, SRCINVERT) )
        {
            if( lpData->lpSrcImage->bmPlanes == 1 && lpData->lpSrcImage->bmBitsPixel == 8 )
                return __BltMaskSrcCopySrcInvert( lpData );
            else if( lpData->lpSrcImage->bmPlanes == 1 && lpData->lpSrcImage->bmBitsPixel == 1 )
            {
                return __BltMaskBit1SrcCopySrcInvert( lpData );            
            }
        }
		else if( lpData->dwRop == MAKEROP4(SRCCOPY, 0) )
		{
            if( lpData->lpSrcImage->bmPlanes == 1 && lpData->lpSrcImage->bmBitsPixel == 8 )
                return __BltMaskSrcCopyDestCopy( lpData );
            if( lpData->lpSrcImage->bmPlanes == 1 && lpData->lpSrcImage->bmBitsPixel == 1 )
            {
                return __BltMaskBit1SrcCopyDestCopy( lpData );
            }
		}
/*  // the code is no use now, reverse inner, no standard
        else if( lpData->dwRop == 1000 )
        {
            if( lpData->lpSrcImage->bmPlanes == 1 && lpData->lpSrcImage->bmBitsPixel == 1 )
            {
                return __BltInvertMaskSrcBit1Copy( lpData );
            }
            if( lpData->lpSrcImage->bmPlanes == 1 && lpData->lpSrcImage->bmBitsPixel == 8 )
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
	//根据操作模式做不同的处理
	switch( lpData->dwRop )
	{
	case SRCCOPY:
		if( lpData->lpSrcImage->bmBitsPixel == 1 )  // mono bitmap
		{
			return __TextSrcCopy2401( lpData );
		}
		else if( lpData->lpSrcImage->bmBitsPixel == 24 )
			return __BltSrcCopy( lpData );
	case SRCAND:
		if( lpData->lpSrcImage->bmBitsPixel == 1 )  // mono bitmap
		{
			return __TextSrcAnd2401( lpData );
		}
		else if( lpData->lpSrcImage->bmBitsPixel == 24 )
			return __BltSrcAnd2424( lpData );
	case SRCINVERT:
		if( lpData->lpSrcImage->bmBitsPixel == 1 )  // mono bitmap
		{
			return __TextSrcInvert2401( lpData );
		}
		else if( lpData->lpSrcImage->bmBitsPixel == 24 )
			return __BltSrcInvert2424( lpData );
	case SRCPAINT:
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
	case DSTINVERT:
		return __BltDstInvert( lpData );
	case NOTSRCCOPY:
		if( lpData->lpSrcImage->bmBitsPixel == 1 )  // mono bitmap
		{
			return __TextNotSrcCopy2401( lpData );
		}
		else if( lpData->lpSrcImage->bmBitsPixel == 24 )
			return __BltNotSrcCopy2424( lpData );
	case MERGECOPY:
		if( lpData->lpBrush &&
			lpData->lpSrcImage &&
			lpData->lpSrcImage->bmBitsPixel == 24 )
		{
			return __BltMergeCopy( lpData );
		}
	case MERGEPAINT:
		if( lpData->lpSrcImage )
			return __BltMergePaint( lpData );
	}
    return FALSE;
}
