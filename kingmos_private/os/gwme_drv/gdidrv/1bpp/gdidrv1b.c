/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵����ͼ���豸��������- 1bit / pixel ��ʽ
�汾�ţ�2.0.0
����ʱ�ڣ�2000
���ߣ�����
�޸ļ�¼��

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

//����ͼ���豸��������ӿڶ���
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

//����Ĭ�ϵ�ɫ��
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
//������static COLORREF _RealizeColor( COLORREF color, LPCDWORD lpcdwPal, UINT uPalNumber, UINT uPalFormat )
//������
//	IN color - RGB ��ɫֵ
//����ֵ��
//	�豸��ص���ɫֵ
//����������
//	��RGB��ɫֵ�õ��豸��ص���ɫֵ
//����: 
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
		
		BYTE r = (BYTE)color;	//��
		BYTE g = (BYTE)(color >> 8);	//��
		BYTE b = (BYTE)(color >> 16);	//��
		BYTE gray;
		//�õ��Ҷ�ֵ
		gray = (BYTE) ( (r * 30l + g * 59l + b * 11l) / 100l );
		if( gray < 0x80 )
			return 0; // black
		else
			return 1; // white;
	}
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
//������static COLORREF _PutPixel( _LPPIXELDATA lpPixelData )
//������
//	IN lpPixelData - _PIXELDATA�ṹָ�룬����д����Ҫ������
//����ֵ��
//	�õ��Ӧ����ɫֵ
//����������
//	�ڶ�Ӧ��xy�������ɫֵ��������֮ǰ��ֵ
//����: 
//	
// *****************************************************************

static COLORREF _PutPixel( _LPPIXELDATA lpPixelData )
{
    LPBYTE lpVideoAdr;
    COLORREF c;
	UINT rop;
	//�Ƿ���Ҫ��ʵ��� ��
	if( (rop = lpPixelData->rop) != R2_NOP )
	{	//��Ҫ���
		//�õ�x bitλ�ã���һ��byte�ϵģ�
        BYTE xbit = patternMask[lpPixelData->x & 0x07];
		//�õ������ʾ����ڴ�λ��
        lpVideoAdr = lpPixelData->lpDestImage->bmBits + lpPixelData->y * lpPixelData->lpDestImage->bmWidthBytes + (lpPixelData->x >> 3);
		//�õ���ǰ����ɫֵ
		c = *lpVideoAdr & xbit;
		//���ݲ���ģʽ����ض���
	    switch( rop )
		{
		case R2_COPYPEN:		//������Ŀ��
			if( lpPixelData->color )
				*lpVideoAdr |= xbit;
			else
				*lpVideoAdr &= ~xbit;
			break;
		case R2_XORPEN:			//���
			if( lpPixelData->color )
				*lpVideoAdr ^= xbit;
			break;
		case R2_NOT:			//��
			*lpVideoAdr ^= xbit;
			break;
		case R2_BLACK:			//��
			*lpVideoAdr &= ~xbit;//0;
			break;
		case R2_WHITE:			//��
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
//������static COLORREF _GetPixel( _LPPIXELDATA lpPixelData )
//������
//	IN lpPixelData - _PIXELDATA�ṹָ�룬�����õ���Ҫ������
//����ֵ��
//	�õ��Ӧ����ɫֵ
//����������
//	�õ���Ӧ��xy�����ɫֵ
//����: 
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

static BOOL __VertialSolidLine( _LPLINEDATA lpLineData, 
							   int x0, 
							   int y0, 
							   int yDir, 
							   unsigned int uLen )
{
    register LPBYTE lpVideoAdr;
    register int sum, widthBytes;
    register BYTE color, xbit;
	//ɨ���п��
    widthBytes = lpLineData->lpDestImage->bmWidthBytes;
	//�Դ�λ��
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
	//���ݲ���ģʽ����ͬ�Ĵ���
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
	//ģ��
    pattern = lpLineData->pattern;
	sum = uLen;
	//���ݲ���ģʽ����ͬ�Ĵ���
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
	//���Դ�Ŀ�ʼ��ַ
	lpVideoAdrStart = lpLineData->lpDestImage->bmBits + y + (x0 >> 3);
    //���Դ�Ľ�����ַ
	lpVideoAdrEnd = lpLineData->lpDestImage->bmBits + y + (x1 >> 3);

    if( lpLineData->color )
        color = 0xff;
    else
        color = 0;

    if( lpVideoAdrStart == lpVideoAdrEnd )
    {	//��ʼ��ͽ�������ͬһ����ַ
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
    {   //��ʼ��ͽ������ڲ�ͬ��ַ
		// draw start byte
        switch( lpLineData->rop )
        {
        case R2_COPYPEN:
			//����ʼ��ַ�ĵ�
            *lpVideoAdrStart = (*lpVideoAdrStart & ~xbitStart) | (color & xbitStart );
            lpVideoAdrStart += xDir;//dir;
			//���м�ĵ�
            for( ; lpVideoAdrStart != lpVideoAdrEnd; lpVideoAdrStart+= xDir )
                *lpVideoAdrStart = color;            
			//��������ַ�ĵ�
            *lpVideoAdrStart = (*lpVideoAdrStart & ~xbitEnd) | ( color & xbitEnd );
            break;
        case R2_XORPEN:
			//����ʼ��ַ�ĵ�
            *lpVideoAdrStart = (*lpVideoAdrStart & ~xbitStart) | ( (*lpVideoAdrStart ^ color) & xbitStart );
            lpVideoAdrStart += xDir;
			//���м�ĵ�
            for( ; lpVideoAdrStart != lpVideoAdrEnd; lpVideoAdrStart+=xDir )
                *lpVideoAdrStart = (*lpVideoAdrStart ^ color);
			//��������ַ�ĵ�
            *lpVideoAdrStart = (*lpVideoAdrStart & ~xbitEnd) | ( (*lpVideoAdrStart ^ color) & xbitEnd );
            break;
        case R2_NOT:
			//����ʼ��ַ�ĵ�
            *lpVideoAdrStart = (*lpVideoAdrStart & ~xbitStart) | ( ~*lpVideoAdrStart & xbitStart );
            lpVideoAdrStart+=xDir;
            //���м�ĵ�
            for( ; lpVideoAdrStart != lpVideoAdrEnd; lpVideoAdrStart+=xDir )
                *lpVideoAdrStart = ~*lpVideoAdrStart;
			//��������ַ�ĵ�
            *lpVideoAdrStart = (*lpVideoAdrStart & ~xbitEnd) | ( ~*lpVideoAdrStart & xbitEnd );
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
	//���Դ�Ŀ�ʼ��ַ
	lpVideoAdrStart = lpLineData->lpDestImage->bmBits + y + (x0 >> 3);
	//���Դ�Ľ�����ַ
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
    {	//��ʼ��ͽ�������ͬһ����ַ
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
    {   // //��ʼ��ͽ������ڲ�ͬ��ַ draw start byte
        switch( lpLineData->rop )
        {
        case R2_COPYPEN:
			//����ʼ��ַ�ĵ�
            *lpVideoAdrStart = (*lpVideoAdrStart & ~xbitStart) | (color & xbitStart );
			//���м�ĵ�   
            lpVideoAdrStart+=xDir;
            for( ; lpVideoAdrStart != lpVideoAdrEnd; lpVideoAdrStart+=xDir )
                *lpVideoAdrStart = (BYTE)color;
			//��������ַ�ĵ�
            *lpVideoAdrStart = (*lpVideoAdrStart & ~xbitEnd) | ( color & xbitEnd );
            break;
        case R2_XORPEN:
			//����ʼ��ַ�ĵ�
            *lpVideoAdrStart = (*lpVideoAdrStart & ~xbitStart) | ( (*lpVideoAdrStart ^ color) & xbitStart );
            //���м�ĵ�
            lpVideoAdrStart+=xDir;
            for( ; lpVideoAdrStart != lpVideoAdrEnd; lpVideoAdrStart+=xDir )
                *lpVideoAdrStart = (*lpVideoAdrStart ^ color);
			//��������ַ�ĵ�
            *lpVideoAdrStart = (*lpVideoAdrStart & ~xbitEnd) | ( (*lpVideoAdrStart ^ color) & xbitEnd );
            break;
        case R2_NOT:
			//����ʼ��ַ�ĵ�
            *lpVideoAdrStart = (*lpVideoAdrStart & ~xbitStart) | ( ~*lpVideoAdrStart & xbitStart );
			//���м�ĵ�
            lpVideoAdrStart+=xDir;
            for( ; lpVideoAdrStart != lpVideoAdrEnd; lpVideoAdrStart+=xDir )
                *lpVideoAdrStart = ~*lpVideoAdrStart;
			//��������ַ�ĵ�
            *lpVideoAdrStart = (*lpVideoAdrStart & ~xbitEnd) | ( ~*lpVideoAdrStart & xbitEnd );
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
	//���Դ�Ŀ�ʼ��ַ
	lpVideoAdrStart = lpLineData->lpDestImage->bmBits + y + (x0 >> 3);
    //���Դ�Ľ�����ַ
	lpVideoAdrEnd = lpLineData->lpDestImage->bmBits + y + (x1 >> 3);

    pattern = lpLineData->pattern;

    if( lpLineData->color )
        color = 0xffff;
    else
        color = 0;
    color = (color & pattern);

    if( lpVideoAdrStart == lpVideoAdrEnd )
    {	//��ʼ��ͽ�������ͬһ����ַ
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
    {   //��ʼ��ͽ������ڲ�ͬ��ַ// draw start byte
        switch( lpLineData->rop )
        {
        case R2_COPYPEN:
			//����ʼ��ַ�ĵ�
            *lpVideoAdrStart = (*lpVideoAdrStart & ~xbitStart) | (color & xbitStart );
            
            //���м�ĵ�
            lpVideoAdrStart+=xDir;
            for( ; lpVideoAdrStart != lpVideoAdrEnd; lpVideoAdrStart+=xDir )
                *lpVideoAdrStart = (*lpVideoAdrStart & ~pattern) | color;
			//��������ַ�ĵ�
            *lpVideoAdrStart = (*lpVideoAdrStart & ~xbitEnd) | ( color & xbitEnd );
            break;
        case R2_XORPEN:
			//����ʼ��ַ�ĵ�
            *lpVideoAdrStart = (*lpVideoAdrStart & ~xbitStart) | ( (*lpVideoAdrStart ^ color) & xbitStart );

            //���м�ĵ�
            lpVideoAdrStart+=xDir;
            for( ; lpVideoAdrStart != lpVideoAdrEnd; lpVideoAdrStart+=xDir )
                *lpVideoAdrStart = (*lpVideoAdrStart & ~pattern) | ( (*lpVideoAdrStart ^ color) & pattern );
			//��������ַ�ĵ�
            *lpVideoAdrStart = (*lpVideoAdrStart & ~xbitEnd) | ( (*lpVideoAdrStart ^ color) & xbitEnd );
            break;
        case R2_NOT:
			//����ʼ��ַ�ĵ�
            *lpVideoAdrStart = (*lpVideoAdrStart & ~xbitStart) | ( ~*lpVideoAdrStart & xbitStart );

            //���м�ĵ�
            lpVideoAdrStart+=xDir;
            for( ; lpVideoAdrStart != lpVideoAdrEnd; lpVideoAdrStart+=xDir )
                *lpVideoAdrStart = (*lpVideoAdrStart & ~pattern) | (~*lpVideoAdrStart & pattern);
			//��������ַ�ĵ�
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

static void __MoveBits( LPBYTE lpDest, int nBits, LPCBYTE lpSrc, int count )
{
    int vBits;

    if( nBits > 0 )     // >>
    {	//Ϊ����
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
    {	//Ϊ����
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
    {	//����Ҫ�ƶ�
        while( count )
        {
            *lpDest++ = *lpSrc++;
            count--;
        }
    }
}

// *****************************************************************
//������static BOOL __TextSolidBitBlt( _LPBLKBITBLT lpData )
//������
//	IN lpData - _BLKBITBLT�ṹָ��
//����ֵ��
//	����TRUE
//����������
//	�ô�ɫ����ı�
//����: 
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
	//Ŀ���ַ
    lpDestStart = lpData->lpDestImage->bmBits + lpData->lprcDest->top * lpData->lpDestImage->bmWidthBytes + (lpData->lprcDest->left >> 3);
    //λģ��ʼλ��
	lpMaskStart = lpData->lpSrcImage->bmBits + lpData->lprcSrc->top * lpData->lpSrcImage->bmWidthBytes + (lpData->lprcSrc->left >> 3);

    i = lpData->lprcDest->right - lpData->lprcDest->left;

    mask = (lpData->lprcSrc->left + i) & 0x07;
    if ( mask )
        rightMask = (WORD)(0xffffff00l >> mask);
    else
        rightMask = (WORD)0xffffffffl;
	//��Ҫ����λ��
    maskShift = lpData->lprcSrc->left & 0x07;
    dstShift = lpData->lprcDest->left & 0x07;
	//��Ҫ��Ŀ���ֽ���
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
	//��Ҫ���������
    rows = lpData->lprcDest->bottom - lpData->lprcDest->top;
	//��ÿһ�����ݽ��д���
    for( i = 0; i < rows; i++ )
    {
	    //����ʼ���ֽ�  handle first byte
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
		//�����м���ֽ�
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

        // ������ܣ����������ֽ� fill end byte
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
		//��һ��
        lpMaskStart += lpData->lpSrcImage->bmWidthBytes;
        lpDestStart += lpData->lpDestImage->bmWidthBytes;
    }
    return TRUE;
}

// *****************************************************************
//������static BOOL __TextTransparentBitBlt( _LPBLKBITBLT lpData )
//������
//	IN lpData - _BLKBITBLT�ṹָ��
//����ֵ��
//	����TRUE
//����������
//	�ô�ɫ���͸���ı�����λģbitΪ1ʱ����������������
//����: 
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
	    //����ʼ���ֽ� //  handle first byte
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
		//�������µ��ֽ�
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
		//��һ��
        lpMaskStart += lpData->lpSrcImage->bmWidthBytes;
        lpDestStart += lpData->lpDestImage->bmWidthBytes;
    }
    return TRUE;
}

// *****************************************************************
//������static BOOL _TextBitBlt( _LPBLKBITBLT lpData )
//������
//	IN lpData - _BLKBITBLT�ṹָ��
//����ֵ��
//	����TRUE
//����������
//	������ı���λģ
//����: 
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


    // ������ܣ���俪ʼ�ֽ� fill left bytes if posible
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
    // ������ܣ�����յ� fill right bytes if posible
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

    // ����м���ֽ� fill middle bytes
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
    if( lpData->lpBrush )	//�Ƿ��ģ��
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


