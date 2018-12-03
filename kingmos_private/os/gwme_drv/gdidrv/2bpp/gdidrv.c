#include <estring.h>
#include <eframe.h>
//#include <edef.h>
//#include <esysfont.h>
//#include <eprivate.h>
//#include <egdi.h>
#include <gwmedrv.h>
#include <eassert.h>
#include <einitscr.h>

#ifndef COLOR_2BPP
#error not define COLOR_2BPP in file eversion.h
#endif

extern const _BITMAPDATA __displayBitmap;

const static WORD leftFillMask[8]=
{
    0xffff,
    0x3fff,
    0x0fff,
    0x03ff,
    0x00ff,
    0x003f,
    0x000f,
    0x0003
};

const static WORD rightFillMask[8]=
{
    0xffff,
    0xc000,
    0xf000,
    0xfc00,
    0xff00,
    0xffc0,
    0xfff0,
    0xfffc
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

const static BYTE bitMask[4]=
{
    0xc0,
    0x30,
    0x0c,
    0x03
};
const static WORD palette[4]=
{
    0x0000,
    0x5555,
    0xaaaa,
    0xffff
};

static const BYTE extPattern[16]={
    0x00,
    0x03,
    0x0c,
    0x0f,
    0x30,
    0x33,
    0x3c,
    0x3f,
    0xc0,
    0xc3,
    0xcc,
    0xcf,
    0xf0,
    0xf3,
    0xfc,
    0xff
};

#define EXT_PATTERN( pattern ) ( (extPattern[(pattern)>>4] << 8) | (extPattern[(pattern)&0x0f]) )

#define PALETTE_SIZE 4
static const PALETTEENTRY _rgbIdentity[PALETTE_SIZE] =
{
    { 0x00, 0x00, 0x00, 0 },  // 0 black
    { 0x80, 0x80, 0x80, 0 },  // 1 dark gray
    { 0xc0, 0xc0, 0xc0, 0 },  // 2 gray
    { 0xff, 0xff, 0xff, 0 }   // 3 black
};

COLORREF _RealizeColor( COLORREF color )
{
    int i, t;
    int diff, old, like = 0;
    BYTE r = (BYTE)color;
    BYTE g = (BYTE)(color >> 8);
    BYTE b = (BYTE)(color >> 16);
    t = r + g + b;

    old = 255 + 255 + 255;
    for( i = 0; i < PALETTE_SIZE; i++ )
    {
        PALETTEENTRY * lp = _rgbIdentity + i;
        if( *((COLORREF*)lp) == color )
            return i;
        // this is a simple map , to change it ...
        diff = abs( lp->peRed + lp->peGreen + lp->peBlue - t );
        if( diff < old )
        {
            old = diff; like = i;
        }
    }
    return like;
}

COLORREF _UnrealizeColor( COLORREF color )
{
    return *((COLORREF*)&_rgbIdentity[(BYTE)color]);
}

COLORREF _PutPixel( _LPPIXELDATA lpPixelData )
{
    LPBYTE lpVideoAdr;
    BYTE xbit = bitMask[lpPixelData->x & 0x03];

    lpVideoAdr = lpPixelData->lpDestImage->bmBits + lpPixelData->y * lpPixelData->lpDestImage->bmWidthBytes + (lpPixelData->x >> 2);

    switch ( lpPixelData->mode )
    {
    case R2_COPYPEN:
        *lpVideoAdr = (*lpVideoAdr & ~xbit) | (palette[lpPixelData->color] & xbit);
        break;
    case R2_XORPEN:
        *lpVideoAdr = (*lpVideoAdr & ~xbit) | ( (*lpVideoAdr ^ palette[lpPixelData->color]) & xbit);
        break;
    case R2_NOT:
        *lpVideoAdr = (*lpVideoAdr & ~xbit) | ( (~*lpVideoAdr) & xbit );
        break;
    default:
        return FALSE;
    }
    return TRUE;
}

COLORREF _GetPixel( _LPPIXELDATA lpPixelData )
{
    LPBYTE lpVideoAdr;
    BYTE xbit = bitMask[lpPixelData->x & 0x03];
    int offset = (3 - (lpPixelData->x & 0x03)) << 1;

    lpVideoAdr = lpPixelData->lpDestImage->bmBits + lpPixelData->y * lpPixelData->lpDestImage->bmWidthBytes + (lpPixelData->x >> 2);
    return ((*lpVideoAdr & xbit) >> offset);
}

static BOOL __VertialSolidLine( _LPLINEDATA lpLineData )
{
    register LPBYTE lpVideoAdr;
    register int y0, y1, widthBytes;
    register BYTE color, xbit;

    y0 = lpLineData->y0;
    y1 = lpLineData->y1;
    widthBytes = lpLineData->lpDestImage->bmWidthBytes;
    lpVideoAdr = lpLineData->lpDestImage->bmBits + y0 * widthBytes + (lpLineData->x0 >> 2);

    color = palette[lpLineData->color];
    xbit =bitMask[lpLineData->x0 & 0x03];

    color &= xbit;

    switch( lpLineData->rop )
    {
    case R2_COPYPEN:
        for( ; y0 < y1; y0++, lpVideoAdr += widthBytes )
            *lpVideoAdr = (*lpVideoAdr & ~xbit) | color;
        break;
    case R2_NOT:
        for( ; y0 < y1; y0++, lpVideoAdr += widthBytes )
            *lpVideoAdr = *lpVideoAdr ^ xbit;
        break;
    case R2_XORPEN:
        for( ; y0 < y1; y0++, lpVideoAdr += widthBytes )
        {
            *lpVideoAdr = *lpVideoAdr ^ color;
        }
        break;
    default:
        return FALSE;
    }
    return TRUE;
}

static BOOL __VertialPatternLine( _LPLINEDATA lpLineData )
{
    register LPBYTE lpVideoAdr;
    register int y0, y1, widthBytes;
    WORD clrFore, clrBack;
    BYTE pattern, xbit, ixbit;

    y0 = lpLineData->y0;
    y1 = lpLineData->y1;
    widthBytes = lpLineData->lpDestImage->bmWidthBytes;
    lpVideoAdr = lpLineData->lpDestImage->bmBits + y0 * widthBytes + (lpLineData->x0 >> 2);

    xbit =bitMask[lpLineData->x0 & 0x03];
    clrFore = palette[lpLineData->color] & xbit;
    clrBack = palette[lpLineData->clrBack] & xbit;

    ixbit = ~xbit;
    pattern = lpLineData->pattern;

    switch ( lpLineData->rop )
    {
    case R2_COPYPEN:
        for( ; y0 < y1; y0++, lpVideoAdr += widthBytes )
        {
            if( pattern & patternMask[y0&0x07] )
                *lpVideoAdr = (*lpVideoAdr & ixbit) | clrFore;
            else
                *lpVideoAdr = (*lpVideoAdr & ixbit) | clrBack;
        }
        break;
    case R2_XORPEN:
        for( ; y0 < y1; y0++, lpVideoAdr += widthBytes )
        {
            if( pattern & patternMask[y0&0x07] )
                *lpVideoAdr ^= clrFore;
            else
                *lpVideoAdr ^= clrBack;
        }
        break;
    case R2_NOT:
        for( ; y0 < y1; y0++, lpVideoAdr += widthBytes )
        {
//            if( pattern & patternMask[y0&0x07] )
                *lpVideoAdr ^= xbit;
        }
        break;
    default:
        return FALSE;
    }
    return TRUE;
}

static BOOL __VertialTransparentLine( _LPLINEDATA lpLineData )
{
    register LPBYTE lpVideoAdr;
    register int y0, y1, widthBytes;
    WORD clrFore;
    BYTE pattern, xbit, ixbit;

    y0 = lpLineData->y0;
    y1 = lpLineData->y1;
    widthBytes = lpLineData->lpDestImage->bmWidthBytes;
    lpVideoAdr = lpLineData->lpDestImage->bmBits + y0 * widthBytes + (lpLineData->x0 >> 2);

    xbit = bitMask[lpLineData->x0 & 0x03];
    clrFore = palette[lpLineData->color] & xbit;

    ixbit = ~xbit;
    pattern = lpLineData->pattern;

    switch( lpLineData->rop )
    {
    case R2_COPYPEN:
        for( ; y0 < y1; y0++, lpVideoAdr += widthBytes )
        {
            if( pattern & patternMask[y0&0x07] )
                *lpVideoAdr = (*lpVideoAdr & ixbit) | clrFore;
        }
        break;
    case R2_XORPEN:
        for( ; y0 < y1; y0++, lpVideoAdr += widthBytes )
        {
            if( pattern & patternMask[y0&0x07] )
                *lpVideoAdr = (*lpVideoAdr & ixbit) | ( (*lpVideoAdr ^ clrFore) & xbit );
        }
        break;
    case R2_NOT:
        for( ; y0 < y1; y0++, lpVideoAdr += widthBytes )
        {
//            if( pattern & patternMask[y0&0x07] )
                *lpVideoAdr = *lpVideoAdr ^ xbit;
        }
        break;
    default:
        return FALSE;
    }
    return TRUE;
}

BOOL _VertialLine( _LPLINEDATA lpLineData )
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

static BOOL __ScanSolidLine( _LPLINEDATA lpLineData )
{
    WORD * lpVideoAdrStart, * lpVideoAdrEnd;
    int x1, y;
    WORD color;
    WORD xbitStart, xbitEnd;

    x1 = lpLineData->x1 - 1;
    y = lpLineData->y0;

    y *= lpLineData->lpDestImage->bmWidthBytes;
    lpVideoAdrStart = (WORD*)(lpLineData->lpDestImage->bmBits + y) + (lpLineData->x0 >> 3);
    lpVideoAdrEnd = (WORD*)(lpLineData->lpDestImage->bmBits + y) + (x1 >> 3);

    xbitStart = leftFillMask[lpLineData->x0 & 0x07];
    xbitEnd = rightFillMask[lpLineData->x1 & 0x07];

    color = palette[lpLineData->color];

    if( lpVideoAdrStart == lpVideoAdrEnd )
    {
        xbitStart &= xbitEnd;
        switch( lpLineData->rop )
        {
        case R2_COPYPEN:
#ifdef EML_DOS
            *HIPTR(lpVideoAdrStart) = ( *HIPTR(lpVideoAdrStart) & ~HIBYTE(xbitStart)) | ( HIBYTE(color) & HIBYTE(xbitStart) );
            *LOPTR(lpVideoAdrStart) = ( *LOPTR(lpVideoAdrStart) & ~LOBYTE(xbitStart)) | ( LOBYTE(color) & LOBYTE(xbitStart) );
#else
            *lpVideoAdrStart = (*lpVideoAdrStart & ~xbitStart) | ( color & xbitStart );
#endif
            break;
        case R2_XORPEN:
#ifdef EML_DOS
            *HIPTR(lpVideoAdrStart) = (*HIPTR(lpVideoAdrStart) & ~HIBYTE(xbitStart)) | ( (*HIPTR(lpVideoAdrStart) ^ HIBYTE(color)) & HIBYTE(xbitStart) );
            *LOPTR(lpVideoAdrStart) = (*LOPTR(lpVideoAdrStart) & ~LOBYTE(xbitStart)) | ( (*LOPTR(lpVideoAdrStart) ^ LOBYTE(color)) & LOBYTE(xbitStart) );
#else
            *lpVideoAdrStart = (*lpVideoAdrStart & ~xbitStart) | ( (*lpVideoAdrStart ^ color) & xbitStart );
#endif
            break;
        case R2_NOT:
#ifdef EML_DOS
            *HIPTR(lpVideoAdrStart) = (*HIPTR(lpVideoAdrStart) & ~HIBYTE(xbitStart) ) | ( ~*HIPTR(lpVideoAdrStart) & HIBYTE(xbitStart) );
            *LOPTR(lpVideoAdrStart) = (*LOPTR(lpVideoAdrStart) & ~LOBYTE(xbitStart) ) | ( ~*LOPTR(lpVideoAdrStart) & LOBYTE(xbitStart) );
#else
            *lpVideoAdrStart = (*lpVideoAdrStart & ~xbitStart) | ( ~*lpVideoAdrStart & xbitStart );
#endif
            break;
        }
    }
    else
    {
        switch( lpLineData->rop )
        {
        case R2_COPYPEN:
#ifdef EML_DOS
            *HIPTR(lpVideoAdrStart) = (*HIPTR(lpVideoAdrStart) & ~HIBYTE(xbitStart)) | ( HIBYTE(color) & HIBYTE(xbitStart) );
            *LOPTR(lpVideoAdrStart) = (*LOPTR(lpVideoAdrStart) & ~LOBYTE(xbitStart)) | ( LOBYTE(color) & LOBYTE(xbitStart) );
#else
            *lpVideoAdrStart = (*lpVideoAdrStart & ~xbitStart) | (color & xbitStart );
#endif

            lpVideoAdrStart++;

            for( ; lpVideoAdrStart < lpVideoAdrEnd; lpVideoAdrStart++ )
            {
#ifdef EML_DOS
                *HIPTR(lpVideoAdrStart) = HIBYTE(color);
                *LOPTR(lpVideoAdrStart) = LOBYTE(color);
#else
                *lpVideoAdrStart = color;
#endif
            }
#ifdef EML_DOS
            *HIPTR(lpVideoAdrStart) = (*HIPTR(lpVideoAdrStart) & ~HIBYTE(xbitEnd)) | ( HIBYTE(color) & HIBYTE(xbitEnd) );
            *LOPTR(lpVideoAdrStart) = (*LOPTR(lpVideoAdrStart) & ~LOBYTE(xbitEnd)) | ( LOBYTE(color) & LOBYTE(xbitEnd) );
#else
            *lpVideoAdrStart = (*lpVideoAdrStart & ~xbitEnd) | ( color & xbitEnd );
#endif
            break;
        case R2_XORPEN:
#ifdef EML_DOS
            *HIPTR(lpVideoAdrStart) = (*HIPTR(lpVideoAdrStart) & ~HIBYTE(xbitStart)) | ( (*HIPTR(lpVideoAdrStart) ^ HIBYTE(color)) & HIBYTE(xbitStart) );
            *LOPTR(lpVideoAdrStart) = (*LOPTR(lpVideoAdrStart) & ~LOBYTE(xbitStart)) | ( (*LOPTR(lpVideoAdrStart) ^ LOBYTE(color)) & LOBYTE(xbitStart) );
#else
            *lpVideoAdrStart = (*lpVideoAdrStart & ~xbitStart) | ( (*lpVideoAdrStart ^ color) & xbitStart );
#endif
            lpVideoAdrStart++;

            for( ; lpVideoAdrStart < lpVideoAdrEnd; lpVideoAdrStart++ )
            {
#ifdef EML_DOS
                *HIPTR(lpVideoAdrStart) = (*HIPTR(lpVideoAdrStart) ^ HIBYTE(color));
                *LOPTR(lpVideoAdrStart) = (*LOPTR(lpVideoAdrStart) ^ LOBYTE(color));
#else
                *lpVideoAdrStart = (*lpVideoAdrStart ^ color);
#endif
            }
#ifdef EML_DOS
            *HIPTR(lpVideoAdrStart) = (*HIPTR(lpVideoAdrStart) & ~HIBYTE(xbitEnd)) | ( (*HIPTR(lpVideoAdrStart) ^ HIBYTE(color)) & HIBYTE(xbitEnd) );
            *LOPTR(lpVideoAdrStart) = (*LOPTR(lpVideoAdrStart) & ~LOBYTE(xbitEnd)) | ( (*LOPTR(lpVideoAdrStart) ^ LOBYTE(color)) & LOBYTE(xbitEnd) );
#else
            *lpVideoAdrStart = (*lpVideoAdrStart & ~xbitEnd) | ( (*lpVideoAdrStart ^ color) & xbitEnd );
#endif
            break;
        case R2_NOT:
#ifdef EML_DOS
            *HIPTR(lpVideoAdrStart) = (*HIPTR(lpVideoAdrStart) & ~HIBYTE(xbitStart)) | ( ~*HIPTR(lpVideoAdrStart) & HIBYTE(xbitStart) );
            *LOPTR(lpVideoAdrStart) = (*LOPTR(lpVideoAdrStart) & ~LOBYTE(xbitStart)) | ( ~*LOPTR(lpVideoAdrStart) & LOBYTE(xbitStart) );
#else
            *lpVideoAdrStart = (*lpVideoAdrStart & ~xbitStart) | ( ~*lpVideoAdrStart & xbitStart );
#endif
            lpVideoAdrStart++;

            for( ; lpVideoAdrStart < lpVideoAdrEnd; lpVideoAdrStart++ )
            {
#ifdef EML_DOS
                *HIPTR(lpVideoAdrStart) = ~*HIPTR(lpVideoAdrStart);
                *LOPTR(lpVideoAdrStart) = ~*LOPTR(lpVideoAdrStart);
#else
                *lpVideoAdrStart = ~*lpVideoAdrStart;
#endif
            }
#ifdef EML_DOS
            *HIPTR(lpVideoAdrStart) = (*HIPTR(lpVideoAdrStart) & ~HIBYTE(xbitEnd)) | ( ~*HIPTR(lpVideoAdrStart) & HIBYTE(xbitEnd) );
            *LOPTR(lpVideoAdrStart) = (*LOPTR(lpVideoAdrStart) & ~LOBYTE(xbitEnd)) | ( ~*LOPTR(lpVideoAdrStart) & LOBYTE(xbitEnd) );
#else
            *lpVideoAdrStart = (*lpVideoAdrStart & ~xbitEnd) | ( ~*lpVideoAdrStart & xbitEnd );
#endif
            break;
        }
    }
    return TRUE;
}

static BOOL __ScanPatternLine( _LPLINEDATA lpLineData )
{
    WORD * lpVideoAdrStart, * lpVideoAdrEnd;
    int x1, y;
    WORD color, clrFore, clrBack, pattern;
    WORD xbitStart, xbitEnd;

    x1 = lpLineData->x1 - 1;
    y = lpLineData->y0;

    y *= lpLineData->lpDestImage->bmWidthBytes;
    lpVideoAdrStart = (WORD*)(lpLineData->lpDestImage->bmBits + y) + (lpLineData->x0 >> 3);
    lpVideoAdrEnd = (WORD*)(lpLineData->lpDestImage->bmBits + y) + (x1 >> 3);

    xbitStart = leftFillMask[lpLineData->x0 & 0x07];
    xbitEnd = rightFillMask[lpLineData->x1 & 0x07];

    clrFore = palette[lpLineData->color];
    clrBack = palette[lpLineData->clrBack];
    pattern = lpLineData->pattern;
    pattern = EXT_PATTERN( pattern );
    color = (clrFore & pattern) | (clrBack & ~pattern );

    if( lpVideoAdrStart == lpVideoAdrEnd )
    {
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
    {   // draw start byte
        switch( lpLineData->rop )
        {
        case R2_COPYPEN:
            *lpVideoAdrStart = (*lpVideoAdrStart & ~xbitStart) | (color & xbitStart );
            lpVideoAdrStart++;

            for( ; lpVideoAdrStart < lpVideoAdrEnd; lpVideoAdrStart++ )
                *lpVideoAdrStart = color;

            *lpVideoAdrStart = (*lpVideoAdrStart & ~xbitEnd) | ( color & xbitEnd );
            break;
        case R2_XORPEN:
            *lpVideoAdrStart = (*lpVideoAdrStart & ~xbitStart) | ( (*lpVideoAdrStart ^ color) & xbitStart );
            lpVideoAdrStart++;

            for( ; lpVideoAdrStart < lpVideoAdrEnd; lpVideoAdrStart++ )
                *lpVideoAdrStart = (*lpVideoAdrStart ^ color);

            *lpVideoAdrStart = (*lpVideoAdrStart & ~xbitEnd) | ( (*lpVideoAdrStart ^ color) & xbitEnd );
            break;
        case R2_NOT:
            *lpVideoAdrStart = (*lpVideoAdrStart & ~xbitStart) | ( ~*lpVideoAdrStart & xbitStart );
            lpVideoAdrStart++;

            for( ; lpVideoAdrStart < lpVideoAdrEnd; lpVideoAdrStart++ )
                *lpVideoAdrStart = ~*lpVideoAdrStart;

            *lpVideoAdrStart = (*lpVideoAdrStart & ~xbitEnd) | ( ~*lpVideoAdrStart & xbitEnd );
            break;
        }
    }
    return TRUE;
}

static BOOL __ScanTransparentLine( _LPLINEDATA lpLineData )
{
    WORD * lpVideoAdrStart, * lpVideoAdrEnd;
    int x1, y;
    WORD color;
    WORD xbitStart, xbitEnd;
    register WORD pattern;

    x1 = lpLineData->x1 - 1;
    y = lpLineData->y0;

    y *= lpLineData->lpDestImage->bmWidthBytes;
    lpVideoAdrStart = (WORD*)(lpLineData->lpDestImage->bmBits + y) + (lpLineData->x0 >> 3);
    lpVideoAdrEnd = (WORD*)(lpLineData->lpDestImage->bmBits + y) + (x1 >> 3);

    pattern = lpLineData->pattern;
    pattern = EXT_PATTERN( pattern );

    xbitStart = leftFillMask[lpLineData->x0 & 0x07] & pattern;
    xbitEnd = rightFillMask[lpLineData->x1 & 0x07] & pattern;

    color = palette[lpLineData->color];
    color = (color & pattern);

    if( lpVideoAdrStart == lpVideoAdrEnd )
    {
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
    {   // draw start byte
        switch( lpLineData->rop )
        {
        case R2_COPYPEN:
            *lpVideoAdrStart = (*lpVideoAdrStart & ~xbitStart) | (color & xbitStart );
            lpVideoAdrStart++;

            for( ; lpVideoAdrStart < lpVideoAdrEnd; lpVideoAdrStart++ )
                *lpVideoAdrStart = (*lpVideoAdrStart & ~pattern) | color;

            *lpVideoAdrStart = (*lpVideoAdrStart & ~xbitEnd) | ( color & xbitEnd );
            break;
        case R2_XORPEN:
            *lpVideoAdrStart = (*lpVideoAdrStart & ~xbitStart) | ( (*lpVideoAdrStart ^ color) & xbitStart );
            lpVideoAdrStart++;

            for( ; lpVideoAdrStart < lpVideoAdrEnd; lpVideoAdrStart++ )
                *lpVideoAdrStart = (*lpVideoAdrStart & ~pattern) | ( (*lpVideoAdrStart ^ color) & pattern );

            *lpVideoAdrStart = (*lpVideoAdrStart & ~xbitEnd) | ( (*lpVideoAdrStart ^ color) & xbitEnd );
            break;
        case R2_NOT:
            *lpVideoAdrStart = (*lpVideoAdrStart & ~xbitStart) | ( ~*lpVideoAdrStart & xbitStart );
            lpVideoAdrStart++;

            for( ; lpVideoAdrStart < lpVideoAdrEnd; lpVideoAdrStart++ )
                *lpVideoAdrStart = (*lpVideoAdrStart & ~pattern) | (~*lpVideoAdrStart & pattern);

            *lpVideoAdrStart = (*lpVideoAdrStart & ~xbitEnd) | ( ~*lpVideoAdrStart & xbitEnd );
            break;
        }
    }
    return TRUE;
}

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

#define _SRCCOPY             0xCC // dest = source
#define _SRCPAINT            0xEE // dest = source OR dest
#define _SRCAND              0x88 // dest = source AND dest
#define _SRCINVERT           0x66 // dest = source XOR dest

void __MoveByteBits( LPBYTE lpDest, int nBits, LPCBYTE lpSrc, WORD count )
{
    int vBits;

    if( nBits > 0 )     // >>
    {
        *lpDest++ = *lpSrc++ >> nBits;
        vBits = 8 - nBits;
        count--;
        while( count )
        {
            *lpDest = ( *(lpSrc-1) << vBits ) | (*lpSrc >> nBits);
            lpDest++; lpSrc++;
            count--;
        }
        // end byte
        *lpDest = *(lpSrc-1) << vBits;
    }
    else if( nBits < 0 )     // <<
    {
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
    {
        while( count )
        {
            *lpDest++ = *lpSrc++;
            count--;
        }
    }
}

#define GETWORD( lp ) ( ( *LOPTR( (lp) ) << 8 ) | *HIPTR( (lp) ) )
void __MoveBits( LPWORD lpDest, int nBits, LPCWORD lpSrc, WORD count )
{
    int vBits;

//    if( nBits > 16 )
//        nBits = nBits;
    if( nBits > 0 )     // >>
    {
#ifdef EML_DOS
        {
            WORD dh = GETWORD( lpSrc );//((WORD)*HIPTR(lpSrc-1) << 8) | *LOPTR(lpSrc-1);
            *lpDest++ = dh >> nBits;
            lpSrc++;
        }
#else
        *lpDest++ = *lpSrc++ >> nBits;
#endif
        vBits = 16 - nBits;
        count--;
        while( count )
        {
#ifdef EML_DOS
            {
                WORD dh = GETWORD( lpSrc - 1 );//((WORD)*HIPTR(lpSrc-1) << 8) | *LOPTR(lpSrc-1);
                WORD dl = GETWORD( lpSrc );//((WORD)*HIPTR(lpSrc) << 8) | *LOPTR(lpSrc);
                *lpDest = ( dh << vBits ) | ( dl >> nBits );
            }
#else
            *lpDest = ( *(lpSrc-1) << vBits ) | (*lpSrc >> nBits);
#endif
            lpDest++; lpSrc++;
            count--;
        }
        // end byte
#ifdef EML_DOS
        {
            WORD dh = GETWORD( lpSrc - 1 );//((WORD)*HIPTR(lpSrc-1) << 8) | *LOPTR(lpSrc-1);
            *lpDest = dh << vBits;
        }
#else
        *lpDest = *(lpSrc-1) << vBits;
#endif
    }
    else if( nBits < 0 )     // <<
    {
        nBits = -nBits;
        vBits = 16 - nBits;
//        *lpDest++ = *lpSrc >> vBits;
        count--;   // read for end byte
        while( count )
        {
#ifdef EML_DOS
            {
                WORD dh = GETWORD( lpSrc );//((WORD)*HIPTR(lpSrc) << 8) | *LOPTR(lpSrc);
                WORD dl = GETWORD( lpSrc + 1 );//((WORD)*HIPTR(lpSrc+1) << 8) | *LOPTR(lpSrc+1);
                *lpDest = (dh << nBits) | (dl >> vBits);
            }
#else
            *lpDest = (*lpSrc << nBits) | (*(lpSrc+1) >> vBits);
#endif
            lpDest++; lpSrc++;
            count--;
        }
        // end byte
#ifdef EML_DOS
        {
            WORD dh = GETWORD( lpSrc );//((WORD)*HIPTR(lpSrc) << 8) | *LOPTR(lpSrc);
            *lpDest = dh << nBits;
        }
#else
        *lpDest = *lpSrc << nBits;
#endif
    }
    else     // No move
    {
        while( count )
        {
#ifdef EML_DOS
            {
                WORD dh = GETWORD( lpSrc );//((WORD)*HIPTR(lpSrc) << 8) | *LOPTR(lpSrc);
                *lpDest++ = dh;
                lpSrc++;
            }
#else
            *lpDest++ = *lpSrc++;
#endif
            count--;
        }
    }
}



static BOOL __TextSolidBitBlt( _LPBLKBITBLT lpData )
{
    LPWORD lpwDestStart, lpwDst;
    LPCBYTE lpMaskStart, lpStart;
    WORD clFore, clBack, color, scanDstWords;
    WORD maskValue, lMask, rMask, rightMask, destWords, maskBytes;
    int i, j, n,  rows, maskShift, dstShift, shift;
    WORD mask;

    scanDstWords = lpData->lpDestImage->bmWidthBytes >> 1;
    lpwDestStart = (LPWORD)(lpData->lpDestImage->bmBits) + lpData->lprcDest->top * scanDstWords + (lpData->lprcDest->left >> 3);
//    lpMaskStart = lpData->lpMaskImage->bmBits + lpData->lprcMask->top * lpData->lpMaskImage->bmWidthBytes + (lpData->lprcMask->left >> 3);
    lpMaskStart = lpData->lpSrcImage->bmBits + lpData->lprcSrc->top * lpData->lpSrcImage->bmWidthBytes + (lpData->lprcSrc->left >> 3);

    i = lpData->lprcDest->right - lpData->lprcDest->left;
//    mask = (lpData->lprcMask->left + i) & 0x07;
    mask = (lpData->lprcSrc->left + i) & 0x07;
    if ( mask )
        rightMask = 0xffffff00l >> mask;
    else
        rightMask = 0xffffffffl;

//    maskShift = lpData->lprcMask->left & 0x07;
    maskShift = lpData->lprcSrc->left & 0x07;
    dstShift = lpData->lprcDest->left & 0x07;

    destWords =( (lpData->lprcDest->right-1) >> 3) - (lpData->lprcDest->left >> 3) + 1;
//    maskBytes = ( (lpData->lprcMask->left + i - 1) >> 3 ) - (lpData->lprcMask->left >> 3) + 1;
    maskBytes = ( (lpData->lprcSrc->left + i - 1) >> 3 ) - (lpData->lprcSrc->left >> 3) + 1;

    lMask = leftFillMask[lpData->lprcDest->left&0x07];
    rMask = rightFillMask[lpData->lprcDest->right&0x07];
    if( destWords == 1 )
        lMask &= rMask;

    clFore = palette[lpData->solidColor];
    clBack = palette[lpData->solidBkColor];
/*
    if( lpData->solidColor )
         clFore = 0xffff;
    else
         clFore = 0x0000;
    if( lpData->solidBkColor )
         clBack = 0xffff;
    else
         clBack = 0x0000;
*/

    rows = lpData->lprcDest->bottom - lpData->lprcDest->top;

    for( i = 0; i < rows; i++ )
    {
	    //  handle first byte
        lpStart = lpMaskStart;
        lpwDst = lpwDestStart;

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

        mask = EXT_PATTERN( mask );
        color = ((clFore & mask) | (clBack & ~mask));
#ifdef EML_DOS
        *HIPTR(lpwDst) = (*HIPTR(lpwDst) & ~HIBYTE(lMask) ) | ( HIBYTE(color) & HIBYTE(lMask) );
        *LOPTR(lpwDst) = (*LOPTR(lpwDst) & ~LOBYTE(lMask) ) | ( LOBYTE(color) & LOBYTE(lMask) );
        lpwDst++;
#else
        *lpwDst++ = (*lpwDst & ~lMask) | (color & lMask);
#endif
        n++;

        // fill middle bytes
        while( n < destWords - 1 )
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

            mask = EXT_PATTERN( mask );
#ifdef EML_DOS
            *HIPTR(lpwDst) = ( HIBYTE(clFore) & HIBYTE(mask) ) | ( HIBYTE(clBack) & ~HIBYTE(mask) );
            *LOPTR(lpwDst) = ( LOBYTE(clFore) & LOBYTE(mask) ) | ( LOBYTE(clBack) & ~LOBYTE(mask) );
            lpwDst++;
#else
            *lpwDst++ = (clFore & mask) | (clBack & ~mask);
#endif
            n++;
        }

        // fill end byte
        if( n < destWords )
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

            mask = EXT_PATTERN( mask );
            color = ((clFore & mask) | (clBack & ~mask));
#ifdef EML_DOS
            *HIPTR(lpwDst) = (*HIPTR(lpwDst) & ~HIBYTE(rMask) ) | ( HIBYTE(color) & HIBYTE(rMask) );
            *LOPTR(lpwDst) = (*LOPTR(lpwDst) & ~LOBYTE(rMask) ) | ( LOBYTE(color) & LOBYTE(rMask) );
#else
            *lpwDst = (*lpwDst & ~rMask) | (color & rMask);
#endif
        }

//        lpMaskStart += lpData->lpMaskImage->bmWidthBytes;
        lpMaskStart += lpData->lpSrcImage->bmWidthBytes;
        lpwDestStart += scanDstWords;
    }
    return TRUE;
}

static BOOL __TextTransparentBitBlt( _LPBLKBITBLT lpData )
{
    LPWORD lpwDestStart, lpwDst;
    LPCBYTE lpMaskStart, lpStart;
    WORD clFore, scanDstWords;
    WORD maskValue, rightMask, destWords, maskBytes;
    int i, j, n,  rows, maskShift, dstShift, shift;
    WORD mask;

    scanDstWords = lpData->lpDestImage->bmWidthBytes >> 1;
    lpwDestStart = (LPWORD)lpData->lpDestImage->bmBits + lpData->lprcDest->top * scanDstWords + (lpData->lprcDest->left >> 3);
//    lpMaskStart = lpData->lpMaskImage->bmBits + lpData->lprcMask->top * lpData->lpMaskImage->bmWidthBytes + (lpData->lprcMask->left >> 3);
    lpMaskStart = lpData->lpSrcImage->bmBits + lpData->lprcSrc->top * lpData->lpSrcImage->bmWidthBytes + (lpData->lprcSrc->left >> 3);

    i = lpData->lprcDest->right - lpData->lprcDest->left;
//    mask = (lpData->lprcMask->left + i) & 0x07;
    mask = (lpData->lprcSrc->left + i) & 0x07;
    if ( mask )
        rightMask = 0xffffff00l >> mask;
    else
        rightMask = 0xffffffffl;

//    maskShift = lpData->lprcMask->left & 0x07;
    maskShift = lpData->lprcSrc->left & 0x07;
    dstShift = lpData->lprcDest->left & 0x07;

    destWords =( (lpData->lprcDest->right-1) >> 3) - (lpData->lprcDest->left >> 3) + 1;
//    maskBytes = ( (lpData->lprcMask->left + i - 1) >> 3 ) - (lpData->lprcMask->left >> 3) + 1;
    maskBytes = ( (lpData->lprcSrc->left + i - 1) >> 3 ) - (lpData->lprcSrc->left >> 3) + 1;

    clFore = palette[lpData->solidColor];
/*
    if( lpData->solidColor )
         clFore = 0xffff;
    else
         clFore = 0x0000;
*/

    rows = lpData->lprcDest->bottom - lpData->lprcDest->top;

    for( i = 0; i < rows; i++ )
    {
	    //  handle first byte
        lpStart = lpMaskStart;
        lpwDst = lpwDestStart;

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

        while( n < destWords )
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

            mask = EXT_PATTERN( mask );
#ifdef EML_DOS
            *HIPTR(lpwDst) = (*HIPTR(lpwDst) & ~HIBYTE(mask) ) | ( HIBYTE(clFore) & HIBYTE(mask) );
            *LOPTR(lpwDst) = (*LOPTR(lpwDst) & ~LOBYTE(mask) ) | ( LOBYTE(clFore) & LOBYTE(mask) );
#else
            *lpwDst = (*lpwDst & ~mask) | (clFore & mask);
#endif
            lpwDst++;
            n++;
        }
//        lpMaskStart += lpData->lpMaskImage->bmWidthBytes;
        lpMaskStart += lpData->lpSrcImage->bmWidthBytes;
        lpwDestStart += scanDstWords;
    }
    return TRUE;
}

BOOL _TextBitBlt( _LPBLKBITBLT lpData )
{
    if( lpData->backMode == TRANSPARENT )
        return __TextTransparentBitBlt( lpData );
    else
        return __TextSolidBitBlt( lpData );
}

static BOOL __BltFillTransparentPattern( _LPBLKBITBLT lpData )
{
    int height = lpData->lprcDest->bottom - lpData->lprcDest->top;
    WORD scanWords = lpData->lpDestImage->bmWidthBytes >> 1;
    LPWORD lpDstStart = (LPWORD)lpData->lpDestImage->bmBits +
                                scanWords * lpData->lprcDest->top +
                                (lpData->lprcDest->left >> 3);
    WORD destWords = ( (lpData->lprcDest->right - 1) >> 3 ) - ( lpData->lprcDest->left >> 3 ) + 1;
    WORD leftMask = leftFillMask[lpData->lprcDest->left & 0x07];
    WORD rightMask = rightFillMask[lpData->lprcDest->right & 0x07];
    LPCBYTE lpPattern = lpData->lpBrush->pattern;
    WORD clrFore;
	int xShift, yShift;
	register WORD Temp;

    if( destWords == 1 )
        leftMask &= rightMask;
    clrFore = palette[lpData->lpBrush->color];
/*
    if( lpData->lpBrush->color )
        clrFore = 0xffff;
    else
        clrFore = 0;
*/

    // fill left bytes if posible
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


    if( leftMask != 0xffff )
    {
        register LPWORD lpwDst = lpDstStart;
        register int m;//, n = lpData->lprcDest->top;
        register WORD pattern;

        for( m = 0; m < height; m++, yShift++, lpwDst += scanWords )
        {
            pattern = *(lpPattern+(yShift&0x07));

			Temp = ( ((WORD)pattern)<<8 ) | pattern;
			pattern = Temp >> xShift;

            pattern = EXT_PATTERN( pattern ) & leftMask;
#ifdef EML_DOS
            *HIPTR(lpwDst) = (*HIPTR(lpwDst) & ~HIBYTE(pattern) ) | ( HIBYTE(clrFore) & HIBYTE(pattern) );
            *LOPTR(lpwDst) = (*LOPTR(lpwDst) & ~LOBYTE(pattern) ) | ( LOBYTE(clrFore) & LOBYTE(pattern) );
#else
            *lpwDst = (*lpwDst & ~pattern) | ( clrFore & pattern );
#endif
        }
        destWords--;
        lpDstStart++;
    }
    // fill right bytes if posible
    if( rightMask != 0xffff && destWords > 0 )
    {
        register LPWORD lpwDst = lpDstStart + destWords - 1;
        register int m;//, n = lpData->lprcDest->top;
        register WORD pattern;
        for( m = 0; m < height; m++, yShift++, lpwDst += scanWords )
        {
            pattern = *(lpPattern+(yShift&0x07));

			Temp = ( ((WORD)pattern)<<8 ) | pattern;
			pattern = Temp >> xShift;

            pattern = EXT_PATTERN( pattern ) & rightMask;
#ifdef EML_DOS
            *HIPTR(lpwDst) = (*HIPTR(lpwDst) & ~HIBYTE(pattern) ) | ( HIBYTE(clrFore) & HIBYTE(pattern) );
            *LOPTR(lpwDst) = (*LOPTR(lpwDst) & ~LOBYTE(pattern) ) | ( LOBYTE(clrFore) & LOBYTE(pattern) );
#else
            *lpwDst = (*lpwDst & ~pattern) | (clrFore & pattern);
#endif
        }
        destWords--;
    }

    // fill middle bytes
    if( destWords > 0 )
    {
        register LPWORD lpwDst;
        register int n, m;//, k = lpData->lprcDest->top;
        register WORD pattern;

        for( m = 0; m < height; m++, k++ )
        {
            pattern = *(lpPattern+(k&0x07));

			Temp = ( ((WORD)pattern)<<8 ) | pattern;
			pattern = Temp >> xShift;

            pattern = EXT_PATTERN( pattern );
            lpwDst = lpDstStart;
            // line copy
            for( n = 0; n < destWords; n++, lpwDst++ )
            {
#ifdef EML_DOS
                *HIPTR(lpwDst) = (*HIPTR(lpwDst) & ~HIBYTE(pattern) ) | ( HIBYTE(clrFore) & HIBYTE(pattern) );
                *LOPTR(lpwDst) = (*LOPTR(lpwDst) & ~LOBYTE(pattern) ) | ( LOBYTE(clrFore) & LOBYTE(pattern) );
#else
                *lpwDst = (*lpwDst & ~pattern) | (clrFore & pattern);
#endif
            }

            lpDstStart += scanWords;
        }
    }
    return TRUE;
}

#undef  FUNCTION
#undef  BLT_ROP
#define FUNCTION __BltFillPattern
#define BLT_ROP( bDst, bSrc )  ( (bSrc) )
#include <bltfpat.h>

#undef  FUNCTION
#undef  BLT_ROP
#define FUNCTION __BltPatInvertPattern
#define BLT_ROP( bDst, bSrc ) ( (bDst) ^ (bSrc) )
#include <bltfpat.h>

//#undef  FUNCTION
//#undef  BLT_ROP
//#define FUNCTION __BltSrcCopyMono
//#define BLT_ROP( bDst, bSrc ) ( (bSrc) )
//#include <bltcpy1.h>

#undef  FUNCTION
#undef  BLT_ROP
#define FUNCTION __BltSrcCopy
#define BLT_ROP( bDst, bSrc )  ( (bSrc) )
#include <bltcpy.h>

#undef  FUNCTION
#undef  BLT_ROP
#define FUNCTION __BltSrcAnd
#define BLT_ROP( bDst, bSrc )  ( (bDst) & (bSrc) )
#include <bltcpy.h>

#undef  FUNCTION
#undef  BLT_ROP
#define FUNCTION __BltNotSrcCopy
#define BLT_ROP( bDst, bSrc )  ( ~(bSrc) )
#include <bltcpy.h>

#undef  FUNCTION
#undef  BLT_ROP
#define FUNCTION __BltSrcInvert
#define BLT_ROP( bDst, bSrc )  ( (bDst) ^ (bSrc) )
#include <bltcpy.h>

#undef  FUNCTION
#undef  BLT_ROP
#define FUNCTION __BltSrcPaint
#define BLT_ROP( bDst, bSrc )  ( (bDst) | (bSrc) )
#include <bltcpy.h>

#undef  FUNCTION
#undef  BLT_ROP
#define FUNCTION __BltFillSolid
#define BLT_ROP( bDst, bColor ) ( (bColor) )
#include <bltfill.h>

#undef  FUNCTION
#undef  BLT_ROP
#define FUNCTION __BltPatInvertSolid
#define BLT_ROP( bDst, bSrc ) ( (bDst) ^ (bSrc) )
#include <bltfill.h>

#undef  FUNCTION
#undef  BLT_ROP
#define FUNCTION __BltDstInvert
#define BLT_ROP( bDst, bSrc ) (~(bDst))
#include <bltfill.h>

BOOL _BlkBitTransparentBlt( _LPBLKBITBLT lpData )
{
    if( lpData->lpBrush )
        return __BltFillTransparentPattern( lpData );
    return FALSE;
}

#undef FUNCTION
#undef SRC_ROP
#undef DEST_ROP
#define SRC_ROP( bSrc, bMask ) ((bSrc)&(bMask))
#define DEST_ROP( bDest, bMask ) ((bDest)&~(bMask))
#define FUNCTION __BltMaskSrcBit2Copy
#include <bltmask2.h>

#undef FUNCTION
#undef SRC_ROP
#undef DEST_ROP
#define SRC_ROP( bSrc, bMask ) ((~(bSrc))&(bMask))
#define DEST_ROP( bDest, bMask ) ((bDest)&~(bMask))
#define FUNCTION __BltMaskSrcBit1Copy
#include <bltmask1.h>


BOOL _BlkBitMaskBlt( _LPBLKBITBLT lpData )
{
    if( lpData->lpSrcImage->bmPlanes == 1 && lpData->lpSrcImage->bmBitsPixel == 1 )
    {
        return __BltMaskSrcBit1Copy( lpData );
    }
    if( lpData->lpSrcImage->bmPlanes == 1 && lpData->lpSrcImage->bmBitsPixel == 2 )
        return __BltMaskSrcBit2Copy( lpData );
    else
        return FALSE;
}

BOOL _BlkBitBlt( _LPBLKBITBLT lpData )
{
    switch( lpData->dwRop )
    {
    case SRCCOPY:
        if( lpData->lpSrcImage->bmBitsPixel == 1 )  // mono bitmap
        {
            return __TextSolidBitBlt( lpData );
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
