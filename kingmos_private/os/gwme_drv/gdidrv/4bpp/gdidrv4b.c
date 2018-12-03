/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/


/*****************************************************
�ļ�˵����ͼ���豸��������(4bit)
�汾�ţ�2.0.0
����ʱ�ڣ�2000
���ߣ�����
�޸ļ�¼��

******************************************************/

#include <eframe.h>
#include <gwmeobj.h>

//#ifndef COLOR_4BPP
//#error not define COLOR_4BPP in file eversion.h
//#endif

extern const _BITMAPDATA __displayBitmap;
static BOOL _BlkBitBlt( _LPBLKBITBLT lpData );
static COLORREF _PutPixel( _LPPIXELDATA lpPixelData );
static COLORREF _GetPixel( _LPPIXELDATA lpPixelData );
static BOOL _BlkBitTransparentBlt( _LPBLKBITBLT lpData );
static BOOL _BlkBitMaskBlt( _LPBLKBITBLT lpData );
static COLORREF _RealizeColor( COLORREF color, LPCDWORD lpcdwPal, UINT uPalNumber, UINT uPalFormat );
static COLORREF _UnrealizeColor( COLORREF color, LPCDWORD lpcdwPal, UINT uPalNumber, UINT uPalFormat );
static BOOL _Line( _LPLINEDATA lpLine );

//����ͼ���豸��������ӿڶ���
const _DISPLAYDRV _drvDisplay4BPP = {
    _PutPixel,
    _GetPixel,
    _Line,
    _BlkBitTransparentBlt,
    _BlkBitMaskBlt,
    _BlkBitBlt,
    _RealizeColor,
    _UnrealizeColor
};

//#define BIG_ENDIAN
// default is LITTLE_ENDIAN

const static DWORD leftFillMask[]=
{ 
#ifndef BIG_ENDIAN
    0xffffffff,
    0xffffff0f,
    0xffffff00,
    0xffff0f00,
    0xffff0000,
    0xff0f0000,
    0xff000000,
    0x0f000000,
#else
	// BIG_ENDIAN
    0xffffffff,
    0x0fffffff,
    0x00ffffff,
    0x000fffff,
    0x0000ffff,
    0x00000fff,
    0x000000ff,
    0x0000000f,
#endif
};

const static DWORD rightFillMask[]=
{
#ifndef BIG_ENDIAN
	// LITTLE_ENDIAN
    0xffffffff,
    0x000000f0,
    0x000000ff,
    0x0000f0ff,
    0x0000ffff,
    0x00f0ffff,
    0x00ffffff,
    0xf0ffffff
#else
	// BIG_ENDIAN
    0xffffffff,
    0xf0000000,
    0xff000000,
    0xfff00000,
    0xffff0000,
    0xfffff000,
    0xffffff00,
    0xfffffff0
#endif
};

const static BYTE shiftValue[]=
{
#ifndef BIG_ENDIAN
	// LITTLE_ENDIAN
	//0x67452301
    4,
    0,
    12,
    8,
    20,
    16,
    28,
    24
#else
	// BIG_ENDIAN
	//0x76543210
    28,
    24,
    20,
    16,
    12,
    8,
    4,
    0
#endif
};

const static BYTE patternMask[8]=
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

const static DWORD bitMask[]=
{
#ifndef BIG_ENDIAN
	// LITTLE_ENDIAN
	0x000000f0,
	0x0000000f,
	0x0000f000,
	0x00000f00,

	0x00f00000,
	0x000f0000,
	0xf0000000,
	0x0f000000
#else
	// BIG_ENDIAN
	0xf0000000,
	0x0f000000,
	0x00f00000,
	0x000f0000,
	0x0000f000,
	0x00000f00,
	0x000000f0,
	0x0000000f
#endif
};


#define PAL_INDEX_MASK  0xf
const static DWORD dwPalette[16]=
{
    0x00000000,
	0x11111111, 
	0x22222222,
	0x33333333,
	0x44444444,
	0x55555555,
	0x66666666,
	0x77777777,
	0x88888888,
	0x99999999,
	0xaaaaaaaa,
	0xbbbbbbbb,
	0xcccccccc,
	0xdddddddd,
	0xeeeeeeee,
	0xffffffff
};

static const WORD extPattern[16]={
#ifndef BIG_ENDIAN
	// LITTLE_ENDIAN
	0x0000,
    0x0f00,     //00 0F 00 0F          -> 
    0xf000,
    0xff00,
    0x000f,
    0x0f0f,
    0xf00f,
    0xff0f,
    0x00f0,
    0x0ff0,
    0xf0f0,
    0xfff0,
    0x00ff,
    0x0fff,
    0xf0ff,
    0xffff
#else
	// BIG_ENDIAN
	0x0000,
    0x000f,
    0x00f0,
    0x00ff,
    0x0f00,
    0x0f0f,
    0x0ff0,
    0x0fff,
    0xf000,
    0xf00f,
    0xf0f0,
    0xf0ff,
    0xff00,
    0xff0f,
    0xfff0,
    0xffff
#endif
};

#ifndef BIG_ENDIAN
	// LITTLE_ENDIAN
#define EXT_PATTERN( pattern ) ( (extPattern[(pattern)>>4]) | (extPattern[(pattern)&0x0f] << 16) )

#else
	// BIG_ENDIAN
#define EXT_PATTERN( pattern ) ( (extPattern[(pattern)>>4] << 16) | (extPattern[(pattern)&0x0f]) )

#endif

#define PALETTE_SIZE 16
static const PALETTEENTRY _rgbIdentity[PALETTE_SIZE] =
{
    { 0x00, 0x00, 0x00, 0 },  // 0 black
    { 0x11, 0x11, 0x11, 0 },  // 1 dark gray
    { 0x22, 0x22, 0x22, 0 },  // 2 gray
    { 0x33, 0x33, 0x33, 0 },   // 3 black

    { 0x44, 0x44, 0x44, 0 },  // 0 black
    { 0x55, 0x55, 0x55, 0 },  // 1 dark gray
    { 0x66, 0x66, 0x66, 0 },  // 2 gray
    { 0x77, 0x77, 0x77, 0 },   // 3 black

    { 0x88, 0x88, 0x88, 0 },  // 0 black
    { 0x99, 0x99, 0x99, 0 },  // 1 dark gray
    { 0xaa, 0xaa, 0xaa, 0 },  // 2 gray
    { 0xbb, 0xbb, 0xbb, 0 },   // 3 black

    { 0xcc, 0xcc, 0xcc, 0 },  // 0 black
    { 0xdd, 0xdd, 0xdd, 0 },  // 1 dark gray
    { 0xee, 0xee, 0xee, 0 },  // 2 gray
    { 0xff, 0xff, 0xff, 0 }   // 3 black
/*

    { 0x00, 0x00, 0x00, 0 },  // 0 black
    { 0x80, 0x80, 0x80, 0 },  // 1 dark gray
    { 0xc0, 0xc0, 0xc0, 0 },  // 2 gray
    { 0xff, 0xff, 0xff, 0 },   // 3 black

    { 0x00, 0x00, 0x00, 0 },  // 0 black
    { 0x80, 0x80, 0x80, 0 },  // 1 dark gray
    { 0xc0, 0xc0, 0xc0, 0 },  // 2 gray
    { 0xff, 0xff, 0xff, 0 },   // 3 black

    { 0x00, 0x00, 0x00, 0 },  // 0 black
    { 0x80, 0x80, 0x80, 0 },  // 1 dark gray
    { 0xc0, 0xc0, 0xc0, 0 },  // 2 gray
    { 0xff, 0xff, 0xff, 0 },   // 3 black

    { 0x00, 0x00, 0x00, 0 },  // 0 black
    { 0x80, 0x80, 0x80, 0 },  // 1 dark gray
    { 0xc0, 0xc0, 0xc0, 0 },  // 2 gray
    { 0xff, 0xff, 0xff, 0 }   // 3 black
*/
};

// *****************************************************************
//������static COLORREF _RealizeColor( COLORREF color, LPCDWORD lpcdwPal, UINT uPalNumber, UINT uPalFormat )
//������
//	IN color - RGB ��ɫֵ
//����ֵ��
//	�豸��ص���ɫֵ
//����������
//	��RGB��ɫֵ�õ��豸��ص���ɫֵ
//����: 
//	��������ӿں���
// *****************************************************************
static COLORREF _RealizeColor( COLORREF color, LPCDWORD lpcdwPal, UINT uPalNumber, UINT uPalFormat )
{  // gray 
	if( lpcdwPal )
	{
		return _Gdi_RealizeColor( color, lpcdwPal, uPalNumber, uPalFormat );
	}
	else
	{
		BYTE r = (BYTE)color; 
		BYTE g = (BYTE)(color >> 8);
		BYTE b = (BYTE)(color >> 16);

		return (BYTE) ( (r * 30l + g * 59l + b * 11l) / 100l ) >> 4;
	}


/*
    int i, t;
    int diff, old, like = 0;
    BYTE r = (BYTE)color;
    BYTE g = (BYTE)(color >> 8);
    BYTE b = (BYTE)(color >> 16);
    t = r + g + b;

    old = 255 + 255 + 255;
    for( i = 0; i < PALETTE_SIZE; i++ )
    {
        PALETTEENTRY * lp = (PALETTEENTRY *)(_rgbIdentity + i);
        if( *((COLORREF*)lp) == color )
            return i;
        // this is a simple map , to change it ...
        diff = lp->peRed + lp->peGreen + lp->peBlue - t;
		if( diff < 0 )
			diff = -diff;
        if( diff < old )
        {
            old = diff; like = i;
        }
    }
    return like;
*/
}

// *****************************************************************
//������static COLORREF _UnrealizeColor( COLORREF color, LPCDWORD lpcdwPal, UINT uPalNumber, UINT uPalFormat )
//������
//	IN color - �豸�����ɫֵ
//����ֵ��
//	�豸�޹���ɫֵ
//����������
//	���豸�����ɫֵ�õ��豸�޹ص�RGBֵ
//����: 
//	��������ӿں���
// *****************************************************************
static COLORREF _UnrealizeColor( COLORREF color, LPCDWORD lpcdwPal, UINT uPalNumber, UINT uPalFormat )
{
	if( lpcdwPal )
	{
		if( color < uPalNumber )
			return _Gdi_UnrealizeColor( color, lpcdwPal, uPalNumber, uPalFormat );
		else
			return 0;
	}
	else
		return *((COLORREF*)&_rgbIdentity[color&0xf]);
}

// *****************************************************************
//������static COLORREF _PutPixel( _LPPIXELDATA lpPixelData )
//������
//	IN lpPixelData - _PIXELDATA�ṹָ�룬����д����Ҫ������
//����ֵ��
//	�õ��Ӧ����ɫֵ
//����������
//	�ڶ�Ӧ��xy�������ɫֵ��������֮ǰ��ֵ
//����: 
//	��������ӿں���
// *****************************************************************
static COLORREF _PutPixel( _LPPIXELDATA lpPixelData )
{
	int rop; 
	COLORREF c;
	//�Ƿ���Ҫ��ʵ��� ��
	if( (rop = lpPixelData->rop) != R2_NOP )
	{	//��Ҫ���
		
		LPDWORD lpdwVideoAdr;
		DWORD xbit = bitMask[lpPixelData->x & 0x7];
		UINT clr = dwPalette[lpPixelData->color&PAL_INDEX_MASK];
		//�õ�x λ��/��ַ
		lpdwVideoAdr = (LPDWORD)(lpPixelData->lpDestImage->bmBits + lpPixelData->y * lpPixelData->lpDestImage->bmWidthBytes) + (lpPixelData->x >> 3);
		//�õ���ǰ/���ص���ɫֵ
		c = *lpdwVideoAdr & xbit;
		//if( !(lpPixelData->x & 0x1) )
		c >>= shiftValue[lpPixelData->x & 0x7];//( ( (~lpPixelData->x) & 0x7 ) << 2 );
		//���ݲ���ģʽ����ض���
		switch ( rop )
		{
		case R2_COPYPEN:		//������Ŀ��
			*lpdwVideoAdr = (*lpdwVideoAdr & ~xbit) | (clr & xbit);
			break;
		case R2_XORPEN:			//���
			*lpdwVideoAdr = (*lpdwVideoAdr & ~xbit) | ( (*lpdwVideoAdr ^ clr) & xbit);
			break;
		case R2_NOT:			//��
			*lpdwVideoAdr = (*lpdwVideoAdr & ~xbit) | ( (~*lpdwVideoAdr) & xbit );
			break;
		case R2_BLACK:			//��
			*lpdwVideoAdr &= ~xbit;
			break;
		case R2_WHITE:			//��
			*lpdwVideoAdr |= xbit;
			break;
		case R2_MASKPEN:   // dest = dest-pixel-color and pen-color
			*lpdwVideoAdr &= ( clr & xbit ) | ~xbit;
			break;
		case R2_MERGEPEN:  // dest = dest-pixel-color or pen-color
			*lpdwVideoAdr |= ( clr & xbit );
			break;
		case R2_MERGENOTPEN:
			*lpdwVideoAdr |= ~clr & xbit;
			break;
		case R2_NOTCOPYPEN:
			*lpdwVideoAdr = (*lpdwVideoAdr & ~xbit) | (~clr & xbit);
			break;
		case R2_NOTXORPEN:
			*lpdwVideoAdr = (*lpdwVideoAdr & ~xbit) | ( (~(*lpdwVideoAdr ^ clr)) & xbit );
			break;
		}
	}
	else
		c = -1;
	return c;
}

// *****************************************************************
//������static COLORREF _GetPixel( _LPPIXELDATA lpPixelData )
//������
//	IN lpPixelData - _PIXELDATA�ṹָ�룬�����õ���Ҫ������
//����ֵ��
//	�õ��Ӧ����ɫֵ
//����������
//	�õ���Ӧ��xy�����ɫֵ
//����: 
//	��������ӿں���
// *****************************************************************
static COLORREF _GetPixel( _LPPIXELDATA lpPixelData )
{
    LPDWORD lpdwVideoAdr;
    DWORD xbit = bitMask[lpPixelData->x & 0x7];
	//�õ���ַ
    lpdwVideoAdr = (LPDWORD)( lpPixelData->lpDestImage->bmBits + lpPixelData->y * lpPixelData->lpDestImage->bmWidthBytes ) + (lpPixelData->x >> 3);
    //return (*lpdwVideoAdr & xbit) >> ( ( (~lpPixelData->x) & 0x7 ) << 2 );
	return (*lpdwVideoAdr & xbit) >> shiftValue[lpPixelData->x & 0x7];//( ( (~lpPixelData->x) & 0x7 ) << 2 );
}

// *****************************************************************
//������static BOOL __VertialSolidLine( _LPLINEDATA lpLineData, 
//										int x0, 
//										int y0, 
//										int yDir, 
//										unsigned int uLen )
//������
//	IN lpLineData - _LINEDATA�ṹָ�룬����������Ҫ������
//	IN x0 - ��� x ����
//	IN y0 - ��� y ����
//	IN yDir - ���򣨵� < 0 , �������ϣ�
//	IN uLen - ����
//����ֵ��
//	����TRUE
//����������
//	�ô�ɫ����ֱ��
//����: 
//	�� line.h ʹ��
// *****************************************************************
static BOOL __VertialSolidLine( _LPLINEDATA lpLineData, int x0, int y0, int yDir, int len )
{
    register LPDWORD lpdwDest;
    register int sum, widthBytes;
    register DWORD color;
	DWORD xbit;
	//ɨ���п��(�ֽ���)
    widthBytes = lpLineData->lpDestImage->bmWidthBytes;
	// align to dword
	//Ŀ���ַ
	lpdwDest = (LPDWORD)(lpLineData->lpDestImage->bmBits + y0 * widthBytes) + (x0 >> 3);

    color = dwPalette[lpLineData->color&PAL_INDEX_MASK];
    xbit =bitMask[x0 & 0x7];
    color &= xbit;

    sum = len;
	//�������
	if( yDir < 0 )
        widthBytes = -widthBytes;	//����
	//���ݲ���ģʽ����ͬ�Ĵ���
    if( lpLineData->rop == R2_COPYPEN )
    {	//����
        while( sum-- )
        {
			*lpdwDest = (*lpdwDest & ~xbit) | color;
            lpdwDest = (LPDWORD)((LPBYTE)lpdwDest + widthBytes);
        }
    }
    else if( lpLineData->rop == R2_NOT )
    {	//��
        while( sum-- )
        {
			*lpdwDest = *lpdwDest ^ xbit;
            lpdwDest = (LPDWORD)((LPBYTE)lpdwDest + widthBytes);
        }
    }
    else if( lpLineData->rop == R2_XORPEN )
    {	//���
        while( sum-- )
        {
			*lpdwDest = *lpdwDest ^ color;
            lpdwDest = (LPDWORD)((LPBYTE)lpdwDest + widthBytes);
        }
    }
    return TRUE;
}

// *****************************************************************
//������static BOOL __VertialPatternLine( _LPLINEDATA lpLineData, 
//										int x0, 
//										int y0, 
//										int yDir, 
//										unsigned int uLen )
//������
//	IN lpLineData - _LINEDATA�ṹָ�룬����������Ҫ������
//	IN x0 - ��� x ����
//	IN y0 - ��� y ����
//	IN yDir - ���򣨵� < 0 , �������ϣ�
//	IN uLen - ����
//����ֵ��
//	����TRUE
//����������
//	��ģ�廭��ֱ��
//����: 
//	�� line.h ʹ��
// *****************************************************************
static BOOL __VertialPatternLine( _LPLINEDATA lpLineData, int x0, int y0, int yDir, int len )
{
    register LPDWORD lpdwDest;
    register int sum, widthBytes;
    DWORD clrFore, clrBack;
    DWORD pattern, xbit, ixbit;
	//ɨ���п��(�ֽ���)
    widthBytes = lpLineData->lpDestImage->bmWidthBytes;
	//Ŀ���ַ
	lpdwDest = (LPDWORD)(lpLineData->lpDestImage->bmBits + y0 * widthBytes) + (x0 >> 3);

    xbit =bitMask[x0 & 0x7];
	//ǰ���ͱ�����ɫ
    clrFore = dwPalette[lpLineData->color&PAL_INDEX_MASK] & xbit;
    clrBack = dwPalette[lpLineData->clrBack&PAL_INDEX_MASK] & xbit;

    ixbit = ~xbit;
    pattern = lpLineData->pattern;
    
	sum = len;
	//�������
	if( yDir < 0 )
		widthBytes = -widthBytes;	//����
	//���ݲ���ģʽ����ͬ�Ĵ���
    if( lpLineData->rop == R2_COPYPEN )
    {	//����
        while( sum-- )
        {
            if( pattern & patternMask[y0&0x07] )
                *lpdwDest = (*lpdwDest & ixbit) | clrFore;
            else
                *lpdwDest = (*lpdwDest & ixbit) | clrBack;

            lpdwDest = (LPDWORD)((LPBYTE)lpdwDest + widthBytes);
            y0 += yDir;
        }
    }
    else if( lpLineData->rop == R2_XORPEN )
    {	//���
        while( sum-- )
        {
            if( pattern & patternMask[y0&0x07] )
                *lpdwDest ^= clrFore;
            else
                *lpdwDest ^= clrBack;

            lpdwDest = (LPDWORD)((LPBYTE)lpdwDest + widthBytes);
            y0 += yDir;
        }
    }
    else if( lpLineData->rop == R2_NOT )
    {	//��
        while( sum-- )
        {
            *lpdwDest ^= xbit;
            lpdwDest = (LPDWORD)((LPBYTE)lpdwDest + widthBytes);
        }        
    }
    return TRUE;
}

// *****************************************************************
//������static BOOL __VertialTransparentLine( _LPLINEDATA lpLineData, 
//										int x0, 
//										int y0, 
//										int yDir, 
//										unsigned int uLen )
//������
//	IN lpLineData - _LINEDATA�ṹָ�룬����������Ҫ������
//	IN x0 - ��� x ����
//	IN y0 - ��� y ����
//	IN yDir - ���򣨵� < 0 , �������ϣ�
//	IN uLen - ����
//����ֵ��
//	����TRUE
//����������
//	��ģ�廭͸����ֱ�ߣ�����ģ���ӦλΪ1ʱ�������Ϊ0ʱ���������
//����: 
//	�� line.h ʹ��
// *****************************************************************
static BOOL __VertialTransparentLine( _LPLINEDATA lpLineData, int x0, int y0, int yDir, int len )
{
    register LPDWORD lpdwDest;
    register int sum, widthBytes;
    DWORD clrFore;
    DWORD pattern, xbit, ixbit;
	//ɨ���п��(�ֽ���)
    widthBytes = lpLineData->lpDestImage->bmWidthBytes;
	//Ŀ���ַ
    lpdwDest = (LPDWORD)(lpLineData->lpDestImage->bmBits + y0 * widthBytes) + (x0 >> 3);

    xbit = bitMask[x0 & 0x7];
    clrFore = dwPalette[lpLineData->color&PAL_INDEX_MASK] & xbit;

    ixbit = ~xbit;

    pattern = lpLineData->pattern;

    sum = len;
	//�������
	if( yDir < 0 )
		widthBytes = -widthBytes;	//����
	//���ݲ���ģʽ����ͬ�Ĵ���
    if( lpLineData->rop == R2_COPYPEN )
    {	//����
        while( sum-- )
        {
            if( pattern & patternMask[y0&0x07] )
                *lpdwDest = (*lpdwDest & ixbit) | clrFore;
            lpdwDest = (LPDWORD)((LPBYTE)lpdwDest + widthBytes);
            y0 += yDir;
        }
    }
    else if( lpLineData->rop == R2_XORPEN )
    {	//���
        while( sum-- )
        {
            if( pattern & patternMask[y0&0x07] )
                *lpdwDest = (*lpdwDest & ixbit) | ( (*lpdwDest ^ clrFore) & xbit );

            lpdwDest = (LPDWORD)((LPBYTE)lpdwDest + widthBytes);
            y0 += yDir;
        }
    }
    else if( lpLineData->rop == R2_NOT )
    {	//��
        while( sum-- )
        {
            *lpdwDest = *lpdwDest ^ xbit;
            lpdwDest = (LPDWORD)((LPBYTE)lpdwDest + widthBytes);
        }
    }
    return TRUE;
}

// *****************************************************************
//������static BOOL __ScanSolidLine( _LPLINEDATA lpLineData, 
//										int x0, 
//										int y0, 
//										int xDir, 
//										unsigned int uLen )
//������
//	IN lpLineData - _LINEDATA�ṹָ�룬����������Ҫ������
//	IN x0 - ��� x ����
//	IN y0 - ��� y ����
//	IN xDir - ���򣨵� < 0 , ��������
//	IN uLen - ����
//����ֵ��
//	����TRUE
//����������
//	�ô�ɫ��ˮƽ��
//����: 
//	�� line.h ʹ��
// *****************************************************************
static BOOL __ScanSolidLine( _LPLINEDATA lpLineData, int x0, int y0, int xDir, unsigned int uLen )
{
    DWORD * lpdwVideoAdrStart, * lpdwVideoAdrEnd;
    int x1, y;
    DWORD color;
    DWORD xbitStart, xbitEnd;

    if( xDir > 0 )
	{	//����
		x1 = x0 + uLen;
        xbitStart = leftFillMask[x0 & 0x7];
        xbitEnd = rightFillMask[x1 & 0x7];
		x1--;
	}
	else
	{	//����
		x1 = x0 - uLen + 1;
		xbitStart = rightFillMask[( x0 + 1 ) & 0x07];
        xbitEnd = leftFillMask[x1 & 0x07];
	}

    y = y0 * lpLineData->lpDestImage->bmWidthBytes;
	// �� DWORD Ϊ��λ��һ���ж��� DWORD
	//�õ���ʼ�ͽ�����ַ
    lpdwVideoAdrStart = (DWORD*)(lpLineData->lpDestImage->bmBits + y) + (x0 >> 3);
    lpdwVideoAdrEnd = (DWORD*)(lpLineData->lpDestImage->bmBits + y) + (x1 >> 3);

    color = dwPalette[lpLineData->color&PAL_INDEX_MASK];
	
    if( lpdwVideoAdrStart == lpdwVideoAdrEnd )
    {	//��ʼ��ͽ�������ͬһ����ַ
        xbitStart &= xbitEnd;
		//���ݲ���ģʽ����ͬ�Ĵ���
        switch( lpLineData->rop )
        {
        case R2_COPYPEN:
            *lpdwVideoAdrStart = (*lpdwVideoAdrStart & ~xbitStart) | ( color & xbitStart );
            break;
        case R2_XORPEN:
            *lpdwVideoAdrStart = (*lpdwVideoAdrStart & ~xbitStart) | ( (*lpdwVideoAdrStart ^ color) & xbitStart );
            break;
        case R2_NOT:
            *lpdwVideoAdrStart = (*lpdwVideoAdrStart & ~xbitStart) | ( ~*lpdwVideoAdrStart & xbitStart );
            break;
        }
    }
    else
    {	//��ʼ��ͽ������ڲ�ͬ��ַ
		//���ݲ���ģʽ����ͬ�Ĵ���
        switch( lpLineData->rop )
        {
        case R2_COPYPEN:
			//����ʼ��ַ�ĵ�
            *lpdwVideoAdrStart = (*lpdwVideoAdrStart & ~xbitStart) | (color & xbitStart );
            lpdwVideoAdrStart += xDir;
			//���м�ĵ�
            for( ; lpdwVideoAdrStart != lpdwVideoAdrEnd; lpdwVideoAdrStart += xDir )
            {
                *lpdwVideoAdrStart = color;
            }
			//��������ַ�ĵ�
            *lpdwVideoAdrStart = (*lpdwVideoAdrStart & ~xbitEnd) | ( color & xbitEnd );
            break;
        case R2_XORPEN:
			//����ʼ��ַ�ĵ�
            *lpdwVideoAdrStart = (*lpdwVideoAdrStart & ~xbitStart) | ( (*lpdwVideoAdrStart ^ color) & xbitStart );
            lpdwVideoAdrStart += xDir;
			//���м�ĵ�
            for( ; lpdwVideoAdrStart != lpdwVideoAdrEnd; lpdwVideoAdrStart += xDir )
            {
                *lpdwVideoAdrStart = ( *lpdwVideoAdrStart ^ color );
            }
			//��������ַ�ĵ�
            *lpdwVideoAdrStart = ( *lpdwVideoAdrStart & ~xbitEnd ) | ( (*lpdwVideoAdrStart ^ color) & xbitEnd );
            break;
        case R2_NOT:
			//����ʼ��ַ�ĵ�
            *lpdwVideoAdrStart = (*lpdwVideoAdrStart & ~xbitStart) | ( ~*lpdwVideoAdrStart & xbitStart );
            lpdwVideoAdrStart += xDir;
			//���м�ĵ�
            for( ; lpdwVideoAdrStart != lpdwVideoAdrEnd; lpdwVideoAdrStart += xDir )
            {
                *lpdwVideoAdrStart = ~*lpdwVideoAdrStart;
            }
			//��������ַ�ĵ�
            *lpdwVideoAdrStart = (*lpdwVideoAdrStart & ~xbitEnd) | ( ~*lpdwVideoAdrStart & xbitEnd );
            break;
        }
    }
    return TRUE;
}

// *****************************************************************
//������static BOOL __ScanPatternLine( _LPLINEDATA lpLineData, 
//										int x0, 
//										int y0, 
//										int xDir, 
//										unsigned int uLen )
//������
//	IN lpLineData - _LINEDATA�ṹָ�룬����������Ҫ������
//	IN x0 - ��� x ����
//	IN y0 - ��� y ����
//	IN xDir - ���򣨵� < 0 , ��������
//	IN uLen - ����
//����ֵ��
//	����TRUE
//����������
//	��ģ���ô�ɫ��ˮƽ��
//����: 
//	�� line.h ʹ��
// *****************************************************************
static BOOL __ScanPatternLine( _LPLINEDATA lpLineData, int x0, int y0, int xDir, unsigned int uLen )
{
    DWORD * lpdwVideoAdrStart, * lpdwVideoAdrEnd;
    int x1, y;
    DWORD color, clrFore, clrBack, pattern;
    DWORD xbitStart, xbitEnd;

    if( xDir > 0 )
	{
		x1 = x0 + uLen;
        xbitStart = leftFillMask[x0 & 0x7];
        xbitEnd = rightFillMask[x1 & 0x7];
		x1--;
	}
	else
	{
		x1 = x0 - uLen + 1;
		xbitStart = rightFillMask[( x0 + 1 ) & 0x07];
        xbitEnd = leftFillMask[x1 & 0x07];
	}


    y = y0 * lpLineData->lpDestImage->bmWidthBytes;
	// �� DWORD Ϊ��λ��һ���ж��� DWORD
	//���Դ�Ŀ�ʼ��ַ
    lpdwVideoAdrStart = (DWORD*)(lpLineData->lpDestImage->bmBits + y) + (x0 >> 3);
	//���Դ�Ľ�����ַ
    lpdwVideoAdrEnd = (DWORD*)(lpLineData->lpDestImage->bmBits + y) + (x1 >> 3);

//    xbitStart = leftFillMask[x0 & 0x7];
    //xbitEnd = rightFillMask[x1 & 0x7];

    clrFore = dwPalette[lpLineData->color&PAL_INDEX_MASK];
    clrBack = dwPalette[lpLineData->clrBack&PAL_INDEX_MASK];
    pattern = lpLineData->pattern;
    pattern = EXT_PATTERN( pattern );
    color = (clrFore & pattern) | (clrBack & ~pattern );

    if( lpdwVideoAdrStart == lpdwVideoAdrEnd )
    {	//��ʼ��ͽ�������ͬһ����ַ
        xbitStart &= xbitEnd;
        switch( lpLineData->rop )
        {
        case R2_COPYPEN:
            *lpdwVideoAdrStart = (*lpdwVideoAdrStart & ~xbitStart) | ( color & xbitStart );
            break;
        case R2_XORPEN:
            *lpdwVideoAdrStart = (*lpdwVideoAdrStart & ~xbitStart) | ( (*lpdwVideoAdrStart ^ color) & xbitStart );
            break;
        case R2_NOT:
            *lpdwVideoAdrStart = (*lpdwVideoAdrStart & ~xbitStart) | ( ~*lpdwVideoAdrStart & xbitStart );
            break;
        }
    }
    else
    {   // draw start byte
		//��ʼ��ͽ������ڲ�ͬ��ַ
        switch( lpLineData->rop )
        {
        case R2_COPYPEN:
			//����ʼ��ַ�ĵ�
            *lpdwVideoAdrStart = (*lpdwVideoAdrStart & ~xbitStart) | (color & xbitStart );
            lpdwVideoAdrStart += xDir;
			//���м�ĵ�
            for( ; lpdwVideoAdrStart != lpdwVideoAdrEnd; lpdwVideoAdrStart += xDir )
                *lpdwVideoAdrStart = color;
			//��������ַ�ĵ�
            *lpdwVideoAdrStart = (*lpdwVideoAdrStart & ~xbitEnd) | ( color & xbitEnd );
            break;
        case R2_XORPEN:
			//����ʼ��ַ�ĵ�
            *lpdwVideoAdrStart = (*lpdwVideoAdrStart & ~xbitStart) | ( (*lpdwVideoAdrStart ^ color) & xbitStart );
            lpdwVideoAdrStart += xDir;
			//���м�ĵ�
            for( ; lpdwVideoAdrStart != lpdwVideoAdrEnd; lpdwVideoAdrStart += xDir )
                *lpdwVideoAdrStart = (*lpdwVideoAdrStart ^ color);
			//��������ַ�ĵ�
            *lpdwVideoAdrStart = (*lpdwVideoAdrStart & ~xbitEnd) | ( (*lpdwVideoAdrStart ^ color) & xbitEnd );
            break;
        case R2_NOT:
			//����ʼ��ַ�ĵ�
            *lpdwVideoAdrStart = (*lpdwVideoAdrStart & ~xbitStart) | ( ~*lpdwVideoAdrStart & xbitStart );
            lpdwVideoAdrStart += xDir;
			//���м�ĵ�
            for( ; lpdwVideoAdrStart != lpdwVideoAdrEnd; lpdwVideoAdrStart += xDir )
                *lpdwVideoAdrStart = ~*lpdwVideoAdrStart;
			//��������ַ�ĵ�
            *lpdwVideoAdrStart = (*lpdwVideoAdrStart & ~xbitEnd) | ( ~*lpdwVideoAdrStart & xbitEnd );
            break;
        }
    }
    return TRUE;
}

// *****************************************************************
//������static BOOL __ScanTransparentLine( _LPLINEDATA lpLineData, 
//										int x0, 
//										int y0, 
//										int xDir, 
//										unsigned int uLen )
//������
//	IN lpLineData - _LINEDATA�ṹָ�룬����������Ҫ������
//	IN x0 - ��� x ����
//	IN y0 - ��� y ����
//	IN xDir - ���򣨵� < 0 , ��������
//	IN uLen - ����
//����ֵ��
//	����TRUE
//����������
//	��ģ���ô�ɫ��͸��ˮƽ��
//����: 
//	�� line.h ʹ��
// *****************************************************************
static BOOL __ScanTransparentLine( _LPLINEDATA lpLineData, int x0, int y0, int xDir, unsigned int uLen )
{
    DWORD * lpdwVideoAdrStart, * lpdwVideoAdrEnd;
    int x1, y;
    DWORD color;
    DWORD xbitStart, xbitEnd;
    DWORD pattern;

    if( xDir > 0 )
	{
		x1 = x0 + uLen;
        xbitStart = leftFillMask[x0 & 0x7];
        xbitEnd = rightFillMask[x1 & 0x7];
		x1--;
	}
	else
	{
		x1 = x0 - uLen + 1;
		xbitStart = rightFillMask[( x0 + 1 ) & 0x07];
        xbitEnd = leftFillMask[x1 & 0x07];
	}


    y = y0 * lpLineData->lpDestImage->bmWidthBytes;
	// �� DWORD Ϊ��λ��һ���ж��� DWORD
	//���Դ�Ŀ�ʼ��ַ
    lpdwVideoAdrStart = (DWORD*)(lpLineData->lpDestImage->bmBits + y) + (x0 >> 3);
	//���Դ�Ľ�����ַ
    lpdwVideoAdrEnd = (DWORD*)(lpLineData->lpDestImage->bmBits + y) + (x1 >> 3);

    pattern = lpLineData->pattern;
    pattern = EXT_PATTERN( pattern );

    color = dwPalette[lpLineData->color&PAL_INDEX_MASK];
    color = (color & pattern);

    if( lpdwVideoAdrStart == lpdwVideoAdrEnd )
    {	//��ʼ��ͽ�������ͬһ����ַ
        xbitStart &= xbitEnd;
        switch( lpLineData->rop )
        {
        case R2_COPYPEN:
            *lpdwVideoAdrStart = (*lpdwVideoAdrStart & ~xbitStart) | ( color & xbitStart );
            break;
        case R2_XORPEN:
            *lpdwVideoAdrStart = (*lpdwVideoAdrStart & ~xbitStart) | ( (*lpdwVideoAdrStart ^ color) & xbitStart );
            break;
        case R2_NOT:
            *lpdwVideoAdrStart = (*lpdwVideoAdrStart & ~xbitStart) | ( ~*lpdwVideoAdrStart & xbitStart );
            break;
        }
    }
    else
    {   //��ʼ��ͽ������ڲ�ͬ��ַ// draw start byte
        switch( lpLineData->rop )
        {
        case R2_COPYPEN:
			//����ʼ��ַ�ĵ�
            *lpdwVideoAdrStart = (*lpdwVideoAdrStart & ~xbitStart) | (color & xbitStart );
            lpdwVideoAdrStart += xDir;
			//���м�ĵ�
            for( ; lpdwVideoAdrStart != lpdwVideoAdrEnd; lpdwVideoAdrStart += xDir )
                *lpdwVideoAdrStart = (*lpdwVideoAdrStart & ~pattern) | color;
			//��������ַ�ĵ�
            *lpdwVideoAdrStart = (*lpdwVideoAdrStart & ~xbitEnd) | ( color & xbitEnd );
            break;
        case R2_XORPEN:
			//����ʼ��ַ�ĵ�
            *lpdwVideoAdrStart = (*lpdwVideoAdrStart & ~xbitStart) | ( (*lpdwVideoAdrStart ^ color) & xbitStart );
            lpdwVideoAdrStart += xDir;
			//���м�ĵ�
            for( ; lpdwVideoAdrStart != lpdwVideoAdrEnd; lpdwVideoAdrStart += xDir )
                *lpdwVideoAdrStart = (*lpdwVideoAdrStart & ~pattern) | ( (*lpdwVideoAdrStart ^ color) & pattern );
			//��������ַ�ĵ�
            *lpdwVideoAdrStart = (*lpdwVideoAdrStart & ~xbitEnd) | ( (*lpdwVideoAdrStart ^ color) & xbitEnd );
            break;
        case R2_NOT:
			//����ʼ��ַ�ĵ�
            *lpdwVideoAdrStart = (*lpdwVideoAdrStart & ~xbitStart) | ( ~*lpdwVideoAdrStart & xbitStart );
            lpdwVideoAdrStart += xDir;
			//���м�ĵ�
            for( ; lpdwVideoAdrStart != lpdwVideoAdrEnd; lpdwVideoAdrStart += xDir )
                *lpdwVideoAdrStart = (*lpdwVideoAdrStart & ~pattern) | (~*lpdwVideoAdrStart & pattern);
			//��������ַ�ĵ�
            *lpdwVideoAdrStart = (*lpdwVideoAdrStart & ~xbitEnd) | ( ~*lpdwVideoAdrStart & xbitEnd );
            break;
        }
    }
    return TRUE;
}
/*
BOOL _ScanLine( _LPLINEDATA lpData )
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
// 0000 1011 1111 1111 1111 1100
// >> 17
// 0000 0000 0000 0000 0101 1111 1111 1111 1110

// 1011 1111 1111 1111 1100
// <<  
// 0000 0000 0000 0000 0101 1111 1111 1111 1110

// *****************************************************************
//������static void __MoveBits( LPBYTE lpDest, int nBits, LPCBYTE lpSrc, int count )
//������
//	IN lpDest - Ŀ���ַ 
//	IN nBits - �ƶ��� bit ��
//	IN lpSrc - Դ��ַ
//	IN count - Դbytes��
//����ֵ��
//	��
//����������
//	��Դ�ƶ� nBits ���������� nBits > 0 Ϊ���ƣ�< 0 Ϊ����
//����: 
//	
// *****************************************************************
void __MoveByteBits( LPBYTE lpDest, int nBits, LPCBYTE lpSrc, DWORD count )
{
    int vBits;

	while( nBits >= 8 )
	{
		//*lpDest++ = 0;
		lpDest++;
		nBits -= 8;
		//count--;
	}

	if( nBits <= -8 )
	{
		//LPBYTE lpbDestEnd = lpDest + count - 1;
		while( nBits <= -8 )
		{			
			//*lpbDestEnd-- = 0;
			lpSrc++;
			nBits += 8;
			//count--;
		}
	}

    if( nBits > 0 )     // >>
    {	//Ϊ����
		*lpDest++ = *lpSrc++ >> nBits;
        vBits = 8 - nBits;
        count--;
        while( count )
        {
            *lpDest = ( ( (WORD)*(lpSrc-1) ) << vBits ) | (*lpSrc >> nBits);
            lpDest++; lpSrc++;
            count--;
        }
        // end byte
        *lpDest = *(lpSrc-1) << vBits;
    }
    else if( nBits < 0 )     // <<
    {	//Ϊ����
        nBits = -nBits;
        vBits = 8 - nBits;
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
    {	//����Ҫ�ƶ�
        while( count )
        {
            *lpDest++ = *lpSrc++;
            count--;
        }
    }
}

//b  0101 0011  1100 0101
//x    0101 1100  0011 0101
//to:  >> 8
//b  0000 0101  0011 1100

//x  1100 0011  0101 0000

//b  10  80
//x  80  10

//to:
//b  21  00
//   00  21

// LITTLE_ENDIAN
/*
void __MoveBits( LPDWORD lpdwDest, int nBits, LPCDWORD lpdwSrc, DWORD count )
{
    int vBits;

    if( nBits > 0 )     // >>>>>>>
    {
        *lpdwDest++ = *lpdwSrc++ << nBits; //
        vBits = 32 - nBits;
        count--;
        while( count )
        {
            *lpdwDest = ( *(lpdwSrc-1) >> vBits ) | (*lpdwSrc << nBits);
            lpdwDest++; lpdwSrc++;
            count--;
        }
        // end byte
        *lpdwDest = *(lpdwSrc-1) >> vBits;
    }
    else if( nBits < 0 )     // <<<<<<<
    {
        nBits = -nBits;
        vBits = 32 - nBits;
        count--;   // read for end byte
        while( count )
        {
            *lpdwDest = (*lpdwSrc >> nBits) | (*(lpdwSrc+1) << vBits);
            lpdwDest++; lpdwSrc++;
            count--;
        }
        // end byte
        *lpdwDest = *lpdwSrc >> nBits;
    }
    else     // No move
    {
        while( count )
        {
            *lpdwDest++ = *lpdwSrc++;
            count--;
        }
    }
}
*/

// BIG_ENDIAN
//#define GETWORD( lp ) ( ( *LOPTR( (lp) ) << 8 ) | *HIPTR( (lp) ) )
/*
void __MoveBits( LPDWORD lpdwDest, int nBits, LPCDWORD lpdwSrc, DWORD count )
{
    int vBits;

    if( nBits > 0 )     // >>>>>>>
    {
        *lpdwDest++ = *lpdwSrc++ >> nBits;
        vBits = 32 - nBits;
        count--;
        while( count )
        {
            *lpdwDest = ( *(lpdwSrc-1) << vBits ) | (*lpdwSrc >> nBits);
            lpdwDest++; lpdwSrc++;
            count--;
        }
        // end byte
        *lpdwDest = *(lpdwSrc-1) << vBits;
    }
    else if( nBits < 0 )     // <<<<<<<
    {
        nBits = -nBits;
        vBits = 32 - nBits;
        count--;   // read for end byte
        while( count )
        {
            *lpdwDest = (*lpdwSrc << nBits) | (*(lpdwSrc+1) >> vBits);
            lpdwDest++; lpdwSrc++;
            count--;
        }
        // end byte
        *lpdwDest = *lpdwSrc << nBits;
    }
    else     // No move
    {
        while( count )
        {
            *lpdwDest++ = *lpdwSrc++;
            count--;
        }
    }
}
*/

// *****************************************************************
//������static BOOL __TextSrcCopy0401( _LPBLKBITBLT lpData )
//������
//	IN lpData - _BLKBITBLT�ṹָ��
//����ֵ��
//	����TRUE
//����������
//	�ô�ɫ����ı�
//����: 
//	
// *****************************************************************
static BOOL __TextSrcCopy0401( _LPBLKBITBLT lpData )
{
    LPDWORD lpdwDestStart, lpdwDst;
    LPCBYTE lpMaskStart, lpStart;
    DWORD clFore, clBack, color, scanDstDWords;
    DWORD mask, lMask, rMask, rightMask, destDWords, maskBytes;
    int i, j, n,  rows, maskShift, dstShift, shift;
	WORD maskValue;

    // �� DWORD Ϊ��λ��һ���ж��� DWORD
	scanDstDWords = lpData->lpDestImage->bmWidthBytes >> 2;
	// �� DWORD Ϊ��λ��Ŀ���ַ
	//Ŀ���ַ
    lpdwDestStart = (LPDWORD)(lpData->lpDestImage->bmBits) + lpData->lprcDest->top * scanDstDWords + (lpData->lprcDest->left >> 3);
	// �� BYTE Ϊ��λ��MaskĿ���ַ
    lpMaskStart = lpData->lpSrcImage->bmBits + lpData->lprcSrc->top * lpData->lpSrcImage->bmWidthBytes + (lpData->lprcSrc->left >> 3);

    i = lpData->lprcDest->right - lpData->lprcDest->left;
    mask = (lpData->lprcSrc->left + i) & 0x07;
    if ( mask )
        rightMask = 0xffffff00l >> mask;
    else
        rightMask = 0xffffffffl;

    maskShift = lpData->lprcSrc->left & 0x07;
    dstShift = lpData->lprcDest->left & 0x07;

    // �� DWORD Ϊ��λ��һ���ж��� DWORD
	destDWords =( (lpData->lprcDest->right-1) >> 3) - (lpData->lprcDest->left >> 3) + 1;
	// �� BYTE Ϊ��λ��һ���ж��� BYTE
    maskBytes = ( (lpData->lprcSrc->left + i - 1) >> 3 ) - (lpData->lprcSrc->left >> 3) + 1;

    lMask = leftFillMask[lpData->lprcDest->left&0x7];
    rMask = rightFillMask[lpData->lprcDest->right&0x7];
    if( destDWords == 1 )
        lMask &= rMask;
	//ǰ���ͱ�����ɫ
    clFore = dwPalette[lpData->solidColor&PAL_INDEX_MASK];
    clBack = dwPalette[lpData->solidBkColor&PAL_INDEX_MASK];
	//��Ҫ���������
    rows = lpData->lprcDest->bottom - lpData->lprcDest->top;
	//��ÿһ�����ݽ��д���
    for( i = 0; i < rows; i++ )
    {
	    //   handle first byte
        lpStart = lpMaskStart;
        lpdwDst = lpdwDestStart;

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

        //����ʼ���ֽ� fill first byte
        if( shift > 8 )
        {
            shift -= 8;
            maskValue |= *lpStart++ << shift;
            if( (DWORD)++j == maskBytes )
            {
                maskValue &= (rightMask << shift);
                shift = 0;
            }
        }

        mask = maskValue >> 8;
        maskValue <<= 8;
        shift += 8;

        mask = EXT_PATTERN( mask );
        color = ((clFore & mask) | (clBack & ~mask));

        *lpdwDst++ = (*lpdwDst & ~lMask) | (color & lMask);

        n++;

        //�����м���ֽ�// fill middle bytes
        while( (DWORD)n < destDWords - 1 )
        {
            if( shift > 8 )
            {
                shift -= 8;
                maskValue |= *lpStart++ << shift;
                if( (DWORD)++j == maskBytes )
                {
                    maskValue &= (rightMask << shift);
                    shift = 0;
                }
            }
            mask = maskValue >> 8;
            maskValue <<= 8;
            shift += 8;

            mask = EXT_PATTERN( mask );

            *lpdwDst++ = (clFore & mask) | (clBack & ~mask);
            n++;
        }

        // ������ܣ����������ֽ�// fill end byte
        if( (DWORD)n < destDWords )
        {
            if( shift > 8 )
            {
                shift -= 8;
                maskValue |= *lpStart++ << shift;
                if( (DWORD)++j == maskBytes )
                {
                    maskValue &= (rightMask << shift);
                    shift = 0;
                }
            }
            mask = maskValue >> 8;

            mask = EXT_PATTERN( mask );
            color = ((clFore & mask) | (clBack & ~mask));
            *lpdwDst = (*lpdwDst & ~rMask) | (color & rMask);
        }
		//��һ��
        lpMaskStart += lpData->lpSrcImage->bmWidthBytes;
        lpdwDestStart += scanDstDWords;
    }
    return TRUE;
}

#undef  FUNCTION
#undef  BLT_ROP
#define FUNCTION __TextTransparentBlt0401
#define BLT_ROP( bDst, bSrc )  ( (bSrc) )
#include "txttpblt.h"
/*
BOOL _TextBitBlt( _LPBLKBITBLT lpData )
{
    if( lpData->backMode == TRANSPARENT )
        return __TextTransparentBitBlt( lpData );
    else
        return __TextSolidBitBlt( lpData );
}
*/

// *****************************************************************
//������static BOOL __BltFillTransparentPattern( _LPBLKBITBLT lpData )
//������
//	IN lpData - _BLKBITBLT�ṹָ��
//����ֵ��
//	�ɹ�TRUE
//����������
//	��ģ��͸�����
//����: 
//	
// *****************************************************************
static BOOL __BltFillTransparentPattern( _LPBLKBITBLT lpData )
{
    int height = lpData->lprcDest->bottom - lpData->lprcDest->top;
    DWORD scanDWords = lpData->lpDestImage->bmWidthBytes >> 2;
    LPDWORD lpdwDstStart = (LPDWORD)lpData->lpDestImage->bmBits +
                                scanDWords * lpData->lprcDest->top +
                                (lpData->lprcDest->left >> 3);
    DWORD destDWords = ( (lpData->lprcDest->right - 1) >> 3 ) - ( lpData->lprcDest->left >> 3 ) + 1;
    DWORD leftMask = leftFillMask[lpData->lprcDest->left & 0x7];
    DWORD rightMask = rightFillMask[lpData->lprcDest->right & 0x7];
    LPCBYTE lpPattern = lpData->lpBrush->pattern;
    DWORD clrFore;
	int xShift, yShift;
	register WORD Temp;

    if( destDWords == 1 )
        leftMask &= rightMask;
    clrFore = dwPalette[lpData->lpBrush->color&PAL_INDEX_MASK];

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
    
	// ������ܣ���俪ʼ�ֽ� // fill left bytes if posible
    if( leftMask != 0xffffffff )
    {
        register LPDWORD lpdwDst = lpdwDstStart;
        register int m;//, n = lpData->lprcDest->top;
        register DWORD pattern;

        for( m = 0; m < height; m++, yShift++, lpdwDst += scanDWords )
        {
            pattern = *(lpPattern+(yShift&0x07));

			Temp = (WORD)( ( ((WORD)pattern)<<8 ) | pattern );
			pattern = Temp >> xShift;

            pattern = EXT_PATTERN( pattern ) & leftMask;
            *lpdwDst = (*lpdwDst & ~pattern) | ( clrFore & pattern );
        }
        destDWords--;
        lpdwDstStart++;
    }
    // ������ܣ�����յ� // fill right bytes if posible
    if( rightMask != 0xffff && destDWords > 0 )
    {
        register LPDWORD lpdwDst = lpdwDstStart + destDWords - 1;
        register int m;//, n = lpData->lprcDest->top;
        register DWORD pattern;
        for( m = 0; m < height; m++, yShift++, lpdwDst += scanDWords )
        {
            pattern = *(lpPattern+(yShift&0x07));

			Temp = (WORD)( ( ((WORD)pattern)<<8 ) | pattern );
			pattern = Temp >> xShift;

            pattern = EXT_PATTERN( pattern ) & rightMask;
            *lpdwDst = (*lpdwDst & ~pattern) | (clrFore & pattern);
        }
        destDWords--;
    }

    // ����м���ֽ� // fill middle bytes
    if( destDWords > 0 )
    {
        register LPDWORD lpdwDst;
        register int n, m;//, k = lpData->lprcDest->top;
        register DWORD pattern;

        for( m = 0; m < height; m++, yShift++ )
        {
            pattern = *(lpPattern+(yShift&0x07));

			Temp = (WORD)( ( ((WORD)pattern)<<8 ) | pattern );
			pattern = Temp >> xShift;

            pattern = EXT_PATTERN( pattern );
            lpdwDst = lpdwDstStart;
            // line copy
            for( n = 0; (DWORD)n < destDWords; n++, lpdwDst++ )
            {
                *lpdwDst = (*lpdwDst & ~pattern) | (clrFore & pattern);
            }

            lpdwDstStart += scanDWords;
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

//#undef  FUNCTION
//#undef  BLT_ROP
//#define FUNCTION __BltSrcCopyMono
//#define BLT_ROP( bDst, bSrc ) ( (bSrc) )
//#include <bltcpy1.h>

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
//������static BOOL _BlkBitTransparentBlt( _LPBLKBITBLT lpData )
//������
//	IN lpData - _BLKBITBLT�ṹָ��
//����ֵ��
//	�ɹ�TRUE
//����������
//	͸�����λģ
//����: 
//	
// *****************************************************************
static BOOL _BlkBitTransparentBlt( _LPBLKBITBLT lpData )
{
    if( lpData->lpBrush )
        return __BltFillTransparentPattern( lpData );
    else if( lpData->lpSrcImage->bmBitsPixel == 1 )
    {
        return __TextTransparentBlt0401( lpData );
    }
    return FALSE;
}

#undef FUNCTION
#undef SRC_ROP
#undef DEST_ROP
#define SRC_ROP( bSrc, bMask ) ((bSrc)&(bMask))
#define DEST_ROP( bDest, bMask ) ((bDest)&~(bMask))
#define FUNCTION __BltMaskSrcBit4Copy
#include "bltmask4.h"

#undef FUNCTION
#undef SRC_ROP
#undef DEST_ROP
#define SRC_ROP( bSrc, bMask ) ((~(bSrc))&(bMask))
#define DEST_ROP( bDest, bMask ) ((bDest)&~(bMask))
#define FUNCTION __BltMaskSrcBit1Copy
#include "bltmask1.h"

// *****************************************************************
//������static BOOL _BlkBitMaskBlt( _LPBLKBITBLT lpData )
//������
//	IN lpData - _BLKBITBLT�ṹָ��
//����ֵ��
//	�ɹ�TRUE
//����������
//	������λ��λ�鴫��
//����: 
//	
// *****************************************************************
static BOOL _BlkBitMaskBlt( _LPBLKBITBLT lpData )
{
    if( lpData->lpSrcImage->bmPlanes == 1 && lpData->lpSrcImage->bmBitsPixel == 1 )
    {
        return __BltMaskSrcBit1Copy( lpData );
    }
    if( lpData->lpSrcImage->bmPlanes == 1 && lpData->lpSrcImage->bmBitsPixel == 4 )
        return __BltMaskSrcBit4Copy( lpData );
    else
        return FALSE;
}

// *****************************************************************
//������static BOOL _BlkBitBlt( _LPBLKBITBLT lpData )
//������
//	IN lpData - _BLKBITBLT�ṹָ��
//����ֵ��
//	�ɹ�TRUE
//����������
//	λ�鴫��
//����: 
//	
// *****************************************************************
static BOOL _BlkBitBlt( _LPBLKBITBLT lpData )
{
    switch( lpData->dwRop )
    {
    case SRCCOPY:
        if( lpData->lpSrcImage->bmBitsPixel == 1 )  // mono bitmap
        {
            return __TextSrcCopy0401( lpData );
//            return __BltSrcCopyMono( lpData );
        }
        else
            return __BltSrcCopy( lpData );
    case SRCAND:
        return __BltSrcAnd( lpData );
    case SRCINVERT:
        return __BltSrcInvert( lpData );
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
        return __BltNotSrcCopy( lpData );
    }
    return FALSE;
}
