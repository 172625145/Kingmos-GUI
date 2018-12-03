/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/


/*****************************************************
�ļ�˵����ͼ���豸��������(32bit)
�汾�ţ�2.0.0
����ʱ�ڣ�2003
���ߣ�����
�޸ļ�¼��

******************************************************/

#include <eframe.h>
#include <gwmeobj.h>

//#ifndef COLOR_32BPP
//#error not define COLOR_32BPP in file version.h
//#endif

static COLORREF _PutPixel( _LPPIXELDATA lpPixelData );
static COLORREF _GetPixel( _LPPIXELDATA lpPixelData );
static BOOL _Line( _LPLINEDATA lpData );
static BOOL _BlkBitTransparentBlt( _LPBLKBITBLT lpData );
static BOOL _BlkBitMaskBlt( _LPBLKBITBLT lpData );
static BOOL _BlkBitBlt( _LPBLKBITBLT lpData );
static COLORREF _RealizeColor( COLORREF color, LPCDWORD lpcdwPal, UINT uPalNumber, UINT uPalFormat );
static COLORREF _UnrealizeColor( COLORREF color, LPCDWORD lpcdwPal, UINT uPalNumber, UINT uPalFormat );

//����ͼ���豸��������ӿڶ���
const _DISPLAYDRV _drvDisplay32BPP = {
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
//������static COLORREF _RealizeColor( COLORREF color )
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
{	// ��ʽ�� 8 8 8
	if( lpcdwPal )
	{
		return _Gdi_RealizeColor( color, lpcdwPal, uPalNumber, uPalFormat );
	}
	else
	{
		BYTE r = ((BYTE)color) & 0xff;
		BYTE g = ((BYTE)(color >> 8)) & 0xff;
		BYTE b = ((BYTE)(color >> 16)) & 0xff;
		
		return (((DWORD)r) << 16) | (((DWORD)g) << 8) | b;
	}
}

// *****************************************************************
//������static COLORREF _UnrealizeColor( COLORREF color )
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
		return _Gdi_UnrealizeColor( color, lpcdwPal, uPalNumber, uPalFormat );
	}
	else
	{
		DWORD r = (color >> 16) & 0xff;
		DWORD g = (color >> 8) & 0xff;
		DWORD b = (color) & 0xff;
		
		return RGB( r, g, b );
	}
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
    LPDWORD lpVideoAdr;
    COLORREF c;
	UINT rop;
	//�Ƿ���Ҫ��ʵ��� ��
	if( (rop = lpPixelData->rop) != R2_NOP )
	{	//��Ҫ���
		//�õ�x λ��/��ַ
		lpVideoAdr = (LPDWORD)(lpPixelData->lpDestImage->bmBits + lpPixelData->y * lpPixelData->lpDestImage->bmWidthBytes) + lpPixelData->x;
		//�õ���ǰ/���ص���ɫֵ
		c = *lpVideoAdr;		
		//���ݲ���ģʽ����ض���
		switch( rop )
		{
		case R2_COPYPEN:		//������Ŀ��
			*lpVideoAdr = (DWORD)lpPixelData->color;
			break;
		case R2_XORPEN:			//���
			*lpVideoAdr ^= (DWORD)lpPixelData->color;
			break;
		case R2_NOT:			//��
			*lpVideoAdr = ~*lpVideoAdr;
			break;
		case R2_BLACK:			//��
			*lpVideoAdr = 0;
			break;
		case R2_WHITE:			//��
			*lpVideoAdr = 0xffffff;//0xffffffffl;
			break;
		case R2_MASKPEN:   // dest = dest-pixel-color and pen-color
			*lpVideoAdr &= (DWORD)lpPixelData->color;
			break;
		case R2_MERGEPEN:  // dest = dest-pixel-color or pen-color
			*lpVideoAdr |= (DWORD)lpPixelData->color;
			break;
		case R2_MERGENOTPEN:
			*lpVideoAdr |= ~(DWORD)lpPixelData->color;
			break;
		case R2_NOTCOPYPEN:
			*lpVideoAdr = ~(DWORD)lpPixelData->color;
			break;
		case R2_NOTXORPEN:
			*lpVideoAdr = ~( *lpVideoAdr ^ (DWORD)lpPixelData->color );
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
    return *((LPDWORD)(lpPixelData->lpDestImage->bmBits + lpPixelData->y * lpPixelData->lpDestImage->bmWidthBytes) + lpPixelData->x);
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
    register LPDWORD lpDest;
    register int sum, widthBytes;
    register DWORD color;
	//ɨ���п��(�ֽ���)
    widthBytes = lpLineData->lpDestImage->bmWidthBytes;
	//Ŀ���ַ
	lpDest = (LPDWORD)(lpLineData->lpDestImage->bmBits + y0 * widthBytes) + x0;
    color = (DWORD)lpLineData->color;
    
    sum = len;
	//�������
	if( yDir < 0 )
        widthBytes = -widthBytes;	//����
	//���ݲ���ģʽ����ͬ�Ĵ���
    if( lpLineData->rop == R2_COPYPEN )
    {	//����
        while( sum-- )
        {
            *lpDest = color;
            lpDest = (LPDWORD)((LPBYTE)lpDest + widthBytes);
        }
    }
    else if( lpLineData->rop == R2_NOT )
    {	//��
        while( sum-- )
        {
            *lpDest = ~*lpDest;
            lpDest = (LPDWORD)((LPBYTE)lpDest + widthBytes);
        }
    }
    else if( lpLineData->rop == R2_XORPEN )
    {	//���
        while( sum-- )
        {
            *lpDest = *lpDest ^ color;
            lpDest = (LPDWORD)((LPBYTE)lpDest + widthBytes);
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
    register LPDWORD lpDest;
    register int sum, widthBytes;
    DWORD clrFore, clrBack;
    BYTE pattern;
	//ɨ���п��(�ֽ���)
    widthBytes = lpLineData->lpDestImage->bmWidthBytes;
	//Ŀ���ַ
	lpDest = (LPDWORD)(lpLineData->lpDestImage->bmBits + y0 * widthBytes) + x0;
	//ǰ���ͱ�����ɫ
    clrFore = (DWORD)lpLineData->color;
    clrBack = (DWORD)lpLineData->clrBack;

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
            {
                *lpDest = (DWORD)clrFore;
            }
            else
            {
                *lpDest = (DWORD)clrBack;
            }
            lpDest = (LPDWORD)((LPBYTE)lpDest + widthBytes);
            y0 += yDir;
        }
    }
    else if( lpLineData->rop == R2_XORPEN )
    {	//���
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
            lpDest = (LPDWORD)((LPBYTE)lpDest + widthBytes);
            y0 += yDir;
        }
    }
    else if( lpLineData->rop == R2_NOT )
    {	//��
        while( sum-- )
        {
            *lpDest = ~*lpDest;
            lpDest = (LPDWORD)((LPBYTE)lpDest + widthBytes);
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
    register LPDWORD lpDest;
    register int sum, widthBytes;
    DWORD clrFore;
    BYTE pattern;
	//ɨ���п��(�ֽ���)
    widthBytes = lpLineData->lpDestImage->bmWidthBytes;
	//Ŀ���ַ
	lpDest = (LPDWORD)(lpLineData->lpDestImage->bmBits + y0 * widthBytes) + x0;
	
    clrFore = (DWORD)lpLineData->color;
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
                *lpDest = (DWORD)clrFore;
            lpDest = (LPDWORD)((LPBYTE)lpDest + widthBytes);
            y0 += yDir;
        }
    }
    else if( lpLineData->rop == R2_XORPEN )
    {	//���
        while( sum-- )
        {
            if( pattern & patternMask[y0&0x07] )
                *lpDest ^= (DWORD)clrFore;
            lpDest = (LPDWORD)((LPBYTE)lpDest + widthBytes);
            y0 += yDir;
        }
    }
    else if( lpLineData->rop == R2_NOT )
    {	//��
        while( sum-- )
        {
            *lpDest = ~*lpDest;
            lpDest = (LPDWORD)((LPBYTE)lpDest + widthBytes);
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
    register LPDWORD lpDestStart;
    register DWORD color;
	//�õ���ʼ��ַ
	lpDestStart = (LPDWORD)(lpLineData->lpDestImage->bmBits + lpLineData->lpDestImage->bmWidthBytes * y0) + x0;

    color = (DWORD)lpLineData->color;
	//���ݲ���ģʽ����ͬ�Ĵ���
    switch( lpLineData->rop )
    {
    case R2_COPYPEN:		//����
		for( ; uLen; lpDestStart += xDir, uLen-- )
        {
            *lpDestStart = color;
        }
        break;
    case R2_XORPEN:			//���
		for( ; uLen; lpDestStart += xDir, uLen-- )
            *lpDestStart ^= color;
        break;
    case R2_NOT:			//��
		for( ; uLen; lpDestStart += xDir, uLen-- )
            *lpDestStart = ~*lpDestStart;
        break;
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
    register LPDWORD lpDestStart;
    register DWORD clrFore, clrBack;
    register BYTE pattern;
	//�õ���ʼ��ַ
	lpDestStart = (LPDWORD)(lpLineData->lpDestImage->bmBits + lpLineData->lpDestImage->bmWidthBytes * y0) + x0;
	//ǰ��ɫ�ͱ���ɫ
    clrFore = (DWORD)lpLineData->color;
    clrBack = (DWORD)lpLineData->clrBack;    
    pattern = lpLineData->pattern;
    if( lpLineData->rop == R2_COPYPEN )
    {	//����
		for( ; uLen; lpDestStart += xDir, x0 += xDir, uLen-- )
        {
            if( pattern & patternMask[ x0 & 0x07 ] )
                *lpDestStart = (DWORD)clrFore;
            else
                *lpDestStart = (DWORD)clrBack;
        }
    }
    else if( lpLineData->rop == R2_XORPEN )
    {	//���
		for( ; uLen; lpDestStart += xDir, x0 += xDir, uLen-- )
        {
            if( pattern & patternMask[x0&0x07] )
                *lpDestStart ^= clrFore;
            else
                *lpDestStart ^= clrBack;
        }
    }
    else if( lpLineData->rop == R2_NOT )
    {	//��
		for( ; uLen; lpDestStart += xDir, x0 += xDir, uLen-- )
        {
            *lpDestStart = ~*lpDestStart;
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
    register LPDWORD lpDestStart;
    register DWORD clrFore;
    register BYTE pattern;
	//�õ�Ŀ���ַ
	lpDestStart = (LPDWORD)(lpLineData->lpDestImage->bmBits + lpLineData->lpDestImage->bmWidthBytes * y0 ) + x0;

    clrFore = (DWORD)lpLineData->color;
    pattern = lpLineData->pattern;
	//���ݲ���ģʽ����ͬ�Ĵ���
    if( lpLineData->rop == R2_COPYPEN )
    {	//����
		for( ; uLen; lpDestStart += xDir, x0 += xDir, uLen-- )
        {
            if( pattern & patternMask[x0&0x07] )
                *lpDestStart = (DWORD)clrFore;
        }
    }
    else if( lpLineData->rop == R2_XORPEN )
    {	//���
		for( ; uLen; lpDestStart += xDir, x0 += xDir, uLen-- )
        {
            if( pattern & patternMask[x0&0x07] )
                *lpDestStart ^= (DWORD)clrFore;
        }
    }
    else if( lpLineData->rop == R2_NOT )
    {	//��
		for( ; uLen; lpDestStart += xDir, x0 += xDir, uLen-- )
        {
            *lpDestStart = ~*lpDestStart;
        }
    }
    return TRUE;
}

#include "..\include\line.h"

#define _SRCCOPY             0xCC // dest = source
#define _SRCPAINT            0xEE // dest = source OR dest
#define _SRCAND              0x88 // dest = source AND dest
#define _SRCINVERT           0x66 // dest = source XOR dest
/*
static void __MoveBits( LPBYTE lpDest, int nBits, LPCBYTE lpSrc, WORD count )
{
    int vBits;

    if( nBits > 0 )     // >>
    {
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
    {
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
*/

// *****************************************************************
//������static BOOL __BltFillTransparentPattern( _LPBLKBITBLT lpData )
//������
//	IN lpData - _BLKBITBLT�ṹָ��
//����ֵ��
//	����TRUE
//����������
//	�ô�ɫ���͸��λģ����λģbitΪ1ʱ����������������
//����: 
//	
// *****************************************************************
static BOOL __BltFillTransparentPattern( _LPBLKBITBLT lpData )
{
    LPDWORD lpDestStart, lpDest;
    LPCBYTE lpPattern;
    DWORD clrFore, widthBytes;
    int i, j, n,  rows, cols, shift;
    BYTE mask, bitMask;
	//Ŀ�꿪ʼ��ַ
    lpDestStart = (LPDWORD)(lpData->lpDestImage->bmBits + lpData->lprcDest->top * lpData->lpDestImage->bmWidthBytes) + lpData->lprcDest->left;
    lpPattern = lpData->lpBrush->pattern;
    rows = lpData->lprcDest->bottom - lpData->lprcDest->top;
    cols = lpData->lprcDest->right - lpData->lprcDest->left;
    //shift = lpData->lprcMask->left & 0x07;
    clrFore = (DWORD)lpData->lpBrush->color;
    //n = lpData->lprcDest->top;
	//ɨ�����ֽ���
    widthBytes = lpData->lpDestImage->bmWidthBytes;

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

	//��ÿһ�н��в���
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
                *lpDest = (DWORD)clrFore;
            }
            lpDest++;

            bitMask >>= 1;
        }
        lpDestStart = (LPDWORD)((LPBYTE)lpDestStart + widthBytes);
    }
    return TRUE;
}

                 
#undef  FUNCTION
#undef  BLT_ROP
#define FUNCTION __TextSrcCopy3201
#define BLT_ROP( bDst, bSrc )  ( (bSrc) )
#include "textblt.h"

#undef  FUNCTION
#undef  BLT_ROP
#define FUNCTION __TextNotSrcCopy3201
#define BLT_ROP( bDst, bSrc )  ( ~(bSrc) )
#include "textblt.h"

#undef  FUNCTION
#undef  BLT_ROP
#define FUNCTION __TextSrcInvert3201
#define BLT_ROP( bDst, bSrc )  ( (bDst) ^ (bSrc) )
#include "textblt.h"

#undef  FUNCTION
#undef  BLT_ROP
#define FUNCTION __TextSrcAnd3201
#define BLT_ROP( bDst, bSrc )  ( (bDst) & (bSrc) )
#include "textblt.h"

#undef  FUNCTION
#undef  BLT_ROP
#define FUNCTION __TextTransparentBlt3201
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

#undef  FUNCTION
#undef  BLT_ROP
#define FUNCTION __BltSrcCopy
#define BLT_ROP( bDst, bSrc )  ( (bSrc) )
#include "bltcpy.h"

#undef  FUNCTION
#undef  BLT_ROP
#define FUNCTION __BltSrcAnd3232
#define BLT_ROP( bDst, bSrc )  ( (bDst) & (bSrc) )
#include "bltcpy.h"

#undef  FUNCTION
#undef  BLT_ROP
#define FUNCTION __BltNotSrcCopy3232
#define BLT_ROP( bDst, bSrc )  ( ~(bSrc) )
#include "bltcpy.h"

#undef  FUNCTION
#undef  BLT_ROP
#define FUNCTION __BltSrcInvert3232
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
//������static BOOL _BlkBitTransparentBlt( _LPBLKBITBLT lpData )
//������
//	IN lpData - _BLKBITBLT�ṹָ��
//����ֵ��
//	����TRUE
//����������
//	͸�����λģ����λģbitΪ1ʱ����������������
//����: 
//	
// *****************************************************************
static BOOL _BlkBitTransparentBlt( _LPBLKBITBLT lpData )
{
    if( lpData->lpBrush )
        return __BltFillTransparentPattern( lpData );
    else if( lpData->lpSrcImage->bmBitsPixel == 1 )
    {
        return __TextTransparentBlt3201( lpData );
    }
    return FALSE;
}

#undef FUNCTION
#undef MASK_BLT_ROP
#define MASK_BLT_ROP( bDest, bSrc, bMask ) ( (bMask) ? (bSrc) : ( (bSrc) ^ (bDest) ) )
#define FUNCTION __BltMaskSrcCopySrcInvert
#include "bltmask.h"

#undef FUNCTION
#undef MASK_BLT_ROP
#define MASK_BLT_ROP( bDest, bSrc, bMask ) ((bMask) ? (bSrc) : ( (bSrc) ^ (bDest) ) )
#define FUNCTION __BltMaskBit1SrcCopySrcInvert
#include "bltmask1.h"

#undef FUNCTION
#undef MASK_BLT_ROP
#define MASK_BLT_ROP( bDest, bSrc, bMask ) ( (bMask) ? (bSrc) : (bDest) )
#define FUNCTION __BltMaskSrcCopyDestCopy
#include "bltmask.h"

#undef FUNCTION
#undef MASK_BLT_ROP
#define MASK_BLT_ROP( bDest, bSrc, bMask ) ( (bMask) ? (bSrc) : (bDest) )
#define FUNCTION __BltMaskBit1SrcCopyDestCopy
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
    if( lpData->lpMaskImage->bmBitsPixel == 1 )
    {
        if( lpData->dwRop == MAKEROP4(SRCCOPY, SRCINVERT) )
        {
            if( lpData->lpSrcImage->bmPlanes == 1 && lpData->lpSrcImage->bmBitsPixel == 1 )
            {
                return __BltMaskBit1SrcCopySrcInvert( lpData );
            }
            if( lpData->lpSrcImage->bmPlanes == 1 && lpData->lpSrcImage->bmBitsPixel == 32 )
                return __BltMaskSrcCopySrcInvert( lpData );
        }
        else if( lpData->dwRop == MAKEROP4(SRCCOPY, 0) )
        {
            if( lpData->lpSrcImage->bmPlanes == 1 && lpData->lpSrcImage->bmBitsPixel == 32 )
                return __BltMaskSrcCopyDestCopy( lpData );
            if( lpData->lpSrcImage->bmPlanes == 1 && lpData->lpSrcImage->bmBitsPixel == 1 )
                return __BltMaskBit1SrcCopyDestCopy( lpData );
        }
    }
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
			return __TextSrcCopy3201( lpData );
		}
		else if( lpData->lpSrcImage->bmBitsPixel == 32 )
			return __BltSrcCopy( lpData );
	case SRCAND:
		if( lpData->lpSrcImage->bmBitsPixel == 1 )  // mono bitmap
		{
			return __TextSrcAnd3201( lpData );
		}
		else if( lpData->lpSrcImage->bmBitsPixel == 32 )
			return __BltSrcAnd3232( lpData );
	case SRCINVERT:
		if( lpData->lpSrcImage->bmBitsPixel == 1 )  // mono bitmap
		{
			return __TextSrcInvert3201( lpData );
		}
		else if( lpData->lpSrcImage->bmBitsPixel == 32 )
			return __BltSrcInvert3232( lpData );
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
			return __TextNotSrcCopy3201( lpData );
		}
		else if( lpData->lpSrcImage->bmBitsPixel == 32 )
			return __BltNotSrcCopy3232( lpData );
	case MERGECOPY:
		if( lpData->lpBrush &&
			lpData->lpSrcImage &&
			lpData->lpSrcImage->bmBitsPixel == 32 )
		{
			return __BltMergeCopy( lpData );
		}
	case MERGEPAINT:
		if( lpData->lpSrcImage )
			return __BltMergePaint( lpData );
	}
    return FALSE;
}
