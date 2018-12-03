#include <edef.h>
#include <erect.h>
#include <gwmedrv.h>
//#include <egdi.h>
#include <esysfont.h>
#include <eobjtype.h>
//#include <windef.h>
//#include <xms.h>
//#include <stdio.h>
//#include <malloc.h>
//#include <conio.h>
//#include <process.h> 
static void Construct(void);
static void Destruct(void);
static int FontHeight(void);
static int FontWidth(void);
static int WordLength( LPCBYTE lpText );

static int WordHeight( WORD aWord );
static int WordWidth( WORD aWord );
static int WordMask( LPCBYTE lpText, _LPBITMAPDATA );
static int TextWidth( LPCBYTE lpText, int aLimiteWidth );
static int TextHeight( LPCBYTE, int aLineWidth );
static LPCBYTE NextWord( LPCBYTE );

static void _DeleteObject( void );
static LPCBYTE _GetChineseMask( WORD );
static int _Resume();

const _FONTDRV FAR _SYSFont= {
                      OBJ_FONT,
                      Construct,
                      Destruct,
                      FontHeight,
                      FontWidth,
                      WordLength,
                      WordHeight,
                      WordWidth,
                      WordMask,
                      TextWidth,
                      TextHeight,
                      NextWord };

enum{
    USE_NOFONT = 0,
    USE_ROMFONT = 1,
    USE_FILEFONT = 2
};

#define IS_CHINESE( lpText ) ( (*(lpText) & 0x80) && ( *((lpText) + 1) & 0x80 ) )
#define IS_TEXT_MARK( v ) ( (v) == TEXT_SOFT_BREAK || (v) == TEXT_KEY_BREAK || (v) == TEXT_EOF )
#define _GET_CHINESE( lpText ) ( (WORD)( (((WORD)*(lpText)) << 8) | *((lpText) + 1) ) )
#define _GET_ENGLISH( lpText ) ( *(lpText) )
#define FONT_HEIGHT 16
#define CHINESE_WIDTH 16
#define ENGLISH_WIDTH 8


_LPFONTDRV _GetHFONTPtr( HFONT hFont )
{
    return (_LPFONTDRV)hFont;
};

void _DeleteObject( void )
{
}

int _Resume( void )
{
    return 1;
}

static void Construct()
{
   _Resume();
}

static void Destruct()
{
    _DeleteObject();
}

static int FontHeight()
{
    return FONT_HEIGHT;
}

static int FontWidth()
{
    return 8;
}

static int WordHeight( WORD unused )
{
    return FONT_HEIGHT;
}

static int WordWidth( WORD aWord )
{
    return aWord < 0xff ? ENGLISH_WIDTH : CHINESE_WIDTH;
}

static int WordMask( LPCBYTE lpText, _LPBITMAPDATA lpMask )
{
#ifdef EML_DOS
    static const WORD FAR chineseMask[16] =
    { 0xfffe,
      0x8002,
      0x8002,
      0x8002,
      0x8002,
      0x8002,
      0x8002,
      0x8002,
      0x8002,
      0x8002,
      0x8002,
      0x8002,
      0x8002,
      0x8002,
      0xfffe,
      0x0000 };
    static const BYTE FAR englishMask[16] =
    { 0xfe,
      0x82,
      0x82,
      0x82,
      0x82,
      0x82,
      0x82,
      0x82,
      0x82,
      0x82,
      0x82,
      0x82,
      0x82,
      0x82,
      0xfe,
      0x00 };

   if( IS_CHINESE( lpText ) )
   {
       lpMask->bmWidth = CHINESE_WIDTH;
       lpMask->bmHeight = FONT_HEIGHT;
       lpMask->bmWidthBytes = 2;
       lpMask->bmBits = (BYTE FAR*)chineseMask;
       return 2;
   }
   else
   {
       lpMask->bmWidth = ENGLISH_WIDTH;
       lpMask->bmHeight = FONT_HEIGHT;
       lpMask->bmWidthBytes = 1;
       lpMask->bmBits = englishMask;
       return 1;
   }
#else
   extern const unsigned char eng16Mask[];
   if( IS_CHINESE( lpText ) )
   {
       lpMask->bmWidth = 16;
       lpMask->bmHeight = 16;
       lpMask->bmWidthBytes = 2;
       lpMask->bmBits = _GetChineseMask( _GET_CHINESE( lpText ) );
       return 2;
   }
   else
   {
       lpMask->bmWidth = 8;
       lpMask->bmHeight = 16;
       lpMask->bmWidthBytes = 1;
       lpMask->bmBits = eng16Mask+_GET_ENGLISH( lpText )*16;//_ptrInRomEFont+ _GET_ENGLISH( lpText )*16;
       return 1;
   }
#endif
}

static int TextWidth( LPCBYTE lpText, int len )
{
   int  w = 0;

   if( lpText )
   {
       while( !IS_TEXT_MARK( *lpText ) && *lpText && len )
       {
           if( IS_CHINESE( lpText ) )
           {
               w += WordWidth( _GET_CHINESE( lpText ) );
               lpText += 2;
               if( len > 0 )
                   len -= 2;
           }
           else
           {
               w += WordWidth( _GET_ENGLISH( lpText ) );
               lpText += 1;
               if( len > 0 )
                   len--;
           }
       }
   }
   return w;

}

static int TextHeight( LPCBYTE lpText, int aLineWidth )
{
    int h = 0, w = 0;

    if( lpText )
    {
        h = FONT_HEIGHT;
        do  {
            if( IS_CHINESE( lpText ) )
            {
                if( aLineWidth > 0 && w + CHINESE_WIDTH >= aLineWidth )
                {
                    h += FONT_HEIGHT;
                    w = 0;
                }
                else
                    w += CHINESE_WIDTH;
                lpText += 2;
            }
            else
            {  // english font
                if( (aLineWidth > 0 && w + ENGLISH_WIDTH >= aLineWidth) ||
                    IS_TEXT_MARK( *lpText ) )
                {
                    h += FONT_HEIGHT;
                    w = 0;
                }
                else
                    w += ENGLISH_WIDTH;
                lpText++;
            }
        }while( *lpText );
    }
    return h;
}

static LPCBYTE NextWord( LPCBYTE lpText )
{
    return IS_CHINESE( lpText ) ? (lpText + 2) : (lpText + 1);
}

int WordLength( LPCBYTE lpText )
{
     int l = 0;
     if( lpText )
     {
         while( *lpText )
         {
             if( IS_CHINESE( lpText ) )
                 l+= 2;
             else
                 l++;
             lpText++;
         }
     }
     return l;
}

static LPCBYTE _GetChineseMask( WORD aWord )
{
}
