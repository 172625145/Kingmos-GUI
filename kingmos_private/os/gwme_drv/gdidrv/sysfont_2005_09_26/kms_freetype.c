
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_CACHE_H

#include FT_CACHE_IMAGE_H
#include FT_CACHE_SMALL_BITMAPS_H

//#include "ftcommon.c"

#include <eframe.h>
#include <gwmeobj.h>
#include <eobjtype.h>

static DWORD TTF_Init( void );
static BOOL TTF_Deinit( DWORD dwData );
static BOOL TTF_InstallFont( DWORD dwData, LPCTSTR lpszPathName );
static HANDLE TTF_CreateFont( DWORD dwData, CONST LOGFONT *lplf );
static BOOL TTF_DeleteFont( HANDLE );

static int TTF_MaxHeight( HANDLE );
static int TTF_MaxWidth( HANDLE );
static int TTF_WordLength( HANDLE, const BYTE FAR* lpText );
static int TTF_WordHeight( HANDLE, WORD aWord );
static int TTF_WordWidth( HANDLE, WORD aWord );
static int TTF_WordMask( HANDLE, const BYTE FAR* lpText, _LPBITMAPDATA );
static int TTF_TextWidth( HANDLE, const BYTE FAR* lpText, int aLimiteWidth );
static int TTF_TextHeight( HANDLE, const BYTE FAR* lpText, int aLineWidth );
static const BYTE FAR* TTF_NextWord( HANDLE, const BYTE FAR* lpText );

static void _DeleteObject( void );
static int _Resume();

static WORD GetCodeIndex( LPCBYTE );


const _FONTDRV FAR _TrueTypeDrv = {
//                      OBJ_FONT,
                      TTF_Init,
                      TTF_Deinit,
	                  TTF_InstallFont,
	                  TTF_CreateFont,
	                  TTF_DeleteFont,
                      TTF_MaxHeight,
                      TTF_MaxWidth,
                      TTF_WordLength,
                      TTF_WordHeight,
                      TTF_WordWidth,
                      TTF_WordMask,
                      TTF_TextWidth,
                      TTF_TextHeight,
                      TTF_NextWord 
};
/*
static const _FONT TTFFont = { 
	OBJ_FONT, 
	1, 
	(HANDLE)1,  
	(_LPFONTDRV)&_TTFDrv };
*/
//_FONTDRV * lpTTFDriver = 
//HFONT hTrueTypeFont = (HFONT)&TTFFont;

#define IS_CHINESE( lpText ) ( (*(lpText) & 0x80) )    //&& ( *((lpText) + 1) & 0x80 ) )
//#define _GET_CHINESE( lpText ) ( (WORD)( (((WORD)*(lpText)) << 8) | *((lpText) + 1) ) )
#define _GET_CHINESE( lpText )  GetCodeIndex( (lpText) ) 
//#define _GET_CHINESE( lpText ) ( (WORD)( (((WORD)*((lpText)+1)) << 8) | *(lpText) ) )
#define _GET_ENGLISH( lpText ) ( *(lpText) )
#define IS_TEXT_MARK( v ) ( (v) == TEXT_SOFT_BREAK || (v) == TEXT_KEY_BREAK || (v) == TEXT_EOF )

  /* this simple record is used to model a given `installed' face */
  typedef struct  TFont_
  {
    const char*  filepathname;
    int          face_index;

    int          num_glyphs;

  } TFont, *PFont;

  static
  const char*  file_suffixes[] =
  {
    ".ttf",
    ".ttc",
    ".pfa",
    ".pfb",
    0
  };

  static int     num_fonts;
  static int     max_fonts = 0;
  static PFont*  fonts;



static  FT_Library       library;      /* the FreeType library            */
static  FTC_Manager      manager;      /* the cache manager               */
static  FTC_Image_Cache  image_cache;  /* the glyph image cache           */
static  FTC_SBit_Cache   sbits_cache;  /* the glyph small bitmaps cache   */

static  FT_Face          face;         /* the font face                   */
static  FT_Size          size;         /* the font size                   */
static  FT_GlyphSlot     glyph;        /* the glyph slot                  */

//static  FTC_Image_Desc   current_font;
static FT_Error  error;
static int              use_sbits_cache  = 0;
static int  res = 72;
static   int  antialias = 0;//1;         /* is anti-aliasing active?    */

static int  hinted    = 1;         /* is glyph hinting active?    */

static   int  use_sbits = 1;         /* do we use embedded bitmaps? */
static   int  low_prec  = 0;         /* force low precision         */
static   int  autohint  = 1;//0;         /* force auto-hinting          */
static   int  Num;                   /* current first glyph index   */



typedef struct _FONT_DATA
{
	FTC_Image_Desc font;
	FT_Face  lpFace;
}FONT_DATA, FAR * LPFONT_DATA;



  /*************************************************************************/
  /*                                                                       */
  /* The face requester is a function provided by the client application   */
  /* to the cache manager, whose role is to translate an `abstract' face   */
  /* ID into a real FT_Face object.                                        */
  /*                                                                       */
  /* In this program, the face IDs are simply pointers to TFont objects.   */
  /*                                                                       */
  FT_CALLBACK_DEF(FT_Error)
  my_face_requester( FTC_FaceID  face_id,
                     FT_Library  lib,
                     FT_Pointer  request_data,
                     FT_Face*    aface )
  {
    PFont  font = (PFont)face_id;

    FT_UNUSED( request_data );


    return FT_New_Face( lib,
                        font->filepathname,
                        font->face_index,
                        aface );
  }



static  void  init_freetype( void )
{
    error = FT_Init_FreeType( &library );
    //if ( error )
      //PanicZ( "could not initialize FreeType" );

    error = FTC_Manager_New( library, 0, 0, 0,
                             my_face_requester, 0, &manager );
    //if ( error )
      //PanicZ( "could not initialize cache manager" );

    error = FTC_SBit_Cache_New( manager, &sbits_cache );
    //if ( error )
      //PanicZ( "could not initialize small bitmaps cache" );

    error = FTC_Image_Cache_New( manager, &image_cache );
    //if ( error )
      //PanicZ( "could not initialize glyph image cache" );
}

static int  install_font_file( char*  filepath )
{
    static char   filename[1024 + 5];
    int           i, len, suffix_len, num_faces;
    const char**  suffix;


    len = strlen( filepath );
    if ( len > 1024 )
      len = 1024;

    strncpy( filename, filepath, len );
    filename[len] = 0;

    error = FT_New_Face( library, filename, 0, &face );
    if ( !error )
      goto Success;

    /* could not open the file directly; we will know try various */
    /* suffixes like `.ttf' or `.pfb'                             */

#ifndef macintosh

    suffix     = file_suffixes;
    suffix_len = 0;
    i          = len - 1;

    while ( i > 0 && filename[i] != '\\' && filename[i] != '/' )
    {
		if ( filename[i] == '.' )
		{
			suffix_len = i;
			break;
		}
		i--;
    }
    if ( suffix_len == 0 )
    {
		for ( suffix = file_suffixes; suffix[0]; suffix++ )
		{
			/* try with current suffix */
			strcpy( filename + len, suffix[0] );
			
			error = FT_New_Face( library, filename, 0, &face );
			if ( !error )
				goto Success;
		}
    }

#endif /* !macintosh */

    /* really couldn't open this file */
  return -1;

  Success:

    /* allocate new font object */
  num_faces = face->num_faces;
  for ( i = 0; i < num_faces; i++ )
  {
      PFont  font;
	  
	  
      if ( i > 0 )
      {
		  error = FT_New_Face( library, filename, i, &face );
		  if ( error )
			  continue;
      }
	  
      font = (PFont)malloc( sizeof ( *font ) );
      font->filepathname = (char*)malloc( strlen( filename ) + 1 );
      font->face_index   = i;
      font->num_glyphs   = face->num_glyphs;
	  
      strcpy( (char*)font->filepathname, filename );
	  
      FT_Done_Face( face );
	  
      if ( max_fonts == 0 )
      {
		  max_fonts = 16;
		  fonts = (PFont*)malloc( max_fonts * sizeof ( PFont ) );
      }
      else if ( num_fonts >= max_fonts )
      {
		  max_fonts *= 2;
		  fonts = (PFont*)realloc( fonts, max_fonts * sizeof ( PFont ) );
      }
	  
      fonts[num_fonts++] = font;
  }
  
  return 0;
}

static FT_Error get_glyph_bitmap( 
								  LPFONT_DATA lpfd,
								  WORD aWord,
                                  //grBitmap*  target,
								  _LPBITMAPDATA lpBmp,
                                  int       *left,
                                  int       *top,
                                  int       *x_advance,
                                  int       *y_advance )
  {
    int glyph_index;
	FT_Face lpface;
	
	FTC_Manager_Lookup_Face( manager, lpfd->font.font.face_id, &lpface);

	glyph_index = FT_Get_Char_Index( lpface, aWord );

	if (use_sbits_cache)
    {
      FTC_SBit  sbit;


      error = FTC_SBit_Cache_Lookup( sbits_cache,
                                     &lpfd->font,//&current_font,
                                     glyph_index,
                                     &sbit );
      if ( !error )
      {
		  lpBmp->bmBits =  sbit->buffer;
		  lpBmp->bmHeight = sbit->height;
		  lpBmp->bmWidth = sbit->width;
		  lpBmp->bmWidthBytes = sbit->pitch;

        //target->rows   = sbit->height;
        //target->width  = sbit->width;
        //target->pitch  = sbit->pitch;
        //target->buffer = sbit->buffer;
/*
        switch ( sbit->format )
        {
        case ft_pixel_mode_mono:
          target->mode  = gr_pixel_mode_mono;
          break;

        case ft_pixel_mode_grays:
          target->mode  = gr_pixel_mode_gray;
          target->grays = 256;
          break;

        default:
          return FT_Err_Invalid_Glyph_Format;
        }
*/
        *left      = sbit->left;
        *top       = sbit->top;
        *x_advance = sbit->xadvance;
        *y_advance = sbit->yadvance;
      }
    }
    else
    {
      FT_Glyph  glyf;


      error = FTC_Image_Cache_Lookup( image_cache,
                                      &lpfd->font,////&current_font,
                                      glyph_index,
                                      &glyf );
      if ( !error )
      {
        FT_BitmapGlyph  bitmap = (FT_BitmapGlyph)glyf;
        FT_Bitmap*      source = &bitmap->bitmap;


        if ( glyf->format != ft_glyph_format_bitmap )
          //PanicZ( "invalid glyph format returned!" );
		  DbgOutString("invalid glyph format returned!");

		  lpBmp->bmBits =  source->buffer;
		  lpBmp->bmHeight = source->rows;
		  lpBmp->bmWidth = source->width;
		  lpBmp->bmWidthBytes = source->pitch;
/*

        target->rows   = source->rows;
        target->width  = source->width;
        target->pitch  = source->pitch;
        target->buffer = source->buffer;

        switch ( source->pixel_mode )
        {
        case ft_pixel_mode_mono:
          target->mode  = gr_pixel_mode_mono;
          break;

        case ft_pixel_mode_grays:
          target->mode  = gr_pixel_mode_gray;
          target->grays = source->num_grays;
          break;

        default:
          return FT_Err_Invalid_Glyph_Format;
        }
*/
        *left = bitmap->left;
        *top  = bitmap->top;

        *x_advance = (glyf->advance.x+0x8000) >> 16;
        *y_advance = (glyf->advance.y+0x8000) >> 16;
      }
    }
    return 0;
  }

//static void  set_current_face( PFont  font )
//{
  //  current_font.font.face_id = (FTC_FaceID)font;
//}

//static void  set_current_size( int  pixel_size )
//{
  //  current_font.font.pix_width  = pixel_size;
    //current_font.font.pix_height = pixel_size;
//}

//static  void  set_current_pointsize( int  point_size )
//{
  //  set_current_size( ( point_size * res + 36 ) / 72 );
//}
/*
static void  set_current_image_type( void )
{
    current_font.image_type = antialias ? ftc_image_grays : ftc_image_mono;

    if ( !hinted )
      current_font.image_type |= ftc_image_flag_unhinted;

    if ( autohint )
      current_font.image_type |= ftc_image_flag_autohinted;

    if ( !use_sbits )
      current_font.image_type |= ftc_image_flag_no_sbits;
}
*/
static  void  done_freetype( void )
{
    FTC_Manager_Done( manager );
    FT_Done_FreeType( library );
}

static WORD GetCodeIndex( LPCBYTE lpbCode )
{
	extern const unsigned short _936code[];
	int hiByte = *lpbCode++;
	int loByte = *lpbCode;
	int offset;

    offset = (hiByte - 0x81) * 190 + (loByte - 0x40) - ( loByte > 0x7e ); 


	return _936code[offset];
}

static DWORD TTF_Init( void )
{
   //_Resume();
	_INTR_OFF();
	init_freetype();
	install_font_file( "sunfon.ttf" );
	_INTR_ON();
	return (DWORD)1;
	// this must be remove to open...
}

static BOOL TTF_Deinit( DWORD dwData )
{
	done_freetype();
	//_DeleteObject();
	return TRUE;
}

static BOOL TTF_InstallFont( DWORD dwData, LPCTSTR lpszPathName )
{
	install_font_file( lpszPathName );
	return TRUE;
}


static HANDLE TTF_CreateFont( DWORD dwData, CONST LOGFONT *lplf )
{
	LPFONT_DATA lpfd = (LPFONT_DATA)malloc( sizeof( FONT_DATA ) );
	if( lpfd )
	{
		lpfd->font.font.face_id = fonts[0];

		if( lplf )
			lpfd->font.font.pix_width  = (short)lplf->lfWidth;
		else
			lpfd->font.font.pix_width = 32;

		if( lplf )
			lpfd->font.font.pix_height = (short)lplf->lfHeight;
		else
			lpfd->font.font.pix_height = 32;

		lpfd->font.image_type = antialias ? ftc_image_grays : ftc_image_mono;
		//lpfd->font.image_type = ftc_image_mono;
		
		if ( !hinted )
			lpfd->font.image_type |= ftc_image_flag_unhinted;
		
		if ( autohint )
			lpfd->font.image_type |= ftc_image_flag_autohinted;
		
		if ( !use_sbits )
			lpfd->font.image_type |= ftc_image_flag_no_sbits;
	}
	return lpfd;

	//set_current_face( fonts[font_index] );
	//set_current_face( fonts[0] );
    //set_current_pointsize( 36 );
    //set_current_image_type();
	
}

static BOOL TTF_DeleteFont( HANDLE handle  )
{
	LPFONT_DATA lpfd = (LPFONT_DATA)handle;
     //FT_Done_Face( face ); //??
	free( lpfd );

	return TRUE;
}

static int TTF_MaxHeight( HANDLE handle )
{
	LPFONT_DATA lpfd = (LPFONT_DATA)handle;
	return lpfd->font.font.pix_height;
}

static int TTF_MaxWidth( HANDLE handle )
{
	LPFONT_DATA lpfd = (LPFONT_DATA)handle;
    return lpfd->font.font.pix_width;
}

static int TTF_WordHeight( HANDLE handle, WORD aWord )
{
    LPFONT_DATA lpfd = (LPFONT_DATA)handle;
	int left, top, x_advance, y_advance;
    _BITMAPDATA bmp;


//   bmp.bmDir = 0;
   //bmp.bmPlanes = 1;
   //bmp.bmType = 0;

   get_glyph_bitmap( 
		   lpfd,
		   aWord,
		   &bmp,
		   &left,
		   &top,
		   &x_advance,
		   &y_advance );
   return bmp.bmHeight;
}

static int TTF_WordWidth( HANDLE handle, WORD aWord )
{
    LPFONT_DATA lpfd = (LPFONT_DATA)handle;
	int left, top, x_advance, y_advance;
    _BITMAPDATA bmp;


   //bmp.bitmap.bmPlanes = 1;
   //bmp.bitmap.bmType = 0;
   //bmp.bitmap.bmDir = 0;

   get_glyph_bitmap( 
		   lpfd,
		   aWord,
		   &bmp,
		   &left,
		   &top,
		   &x_advance,
		   &y_advance );
   return bmp.bmWidth;
}

static int TTF_WordMask( HANDLE handle, const BYTE FAR* lpText, _LPBITMAPDATA lpbmp )
{
    LPFONT_DATA lpfd = (LPFONT_DATA)handle;
	int left, top, x_advance, y_advance;
	BYTE bUnicode[] = { 0x60, 0x4f };


//   lpbmp->bitmap.bmPlanes = 1;
//   lpbmp->bitmap.bmType = 0;

   if( IS_CHINESE( lpText ) )
   {
	   get_glyph_bitmap( 
		   lpfd,
		   _GET_CHINESE( lpText ),
		   lpbmp,
		   &left,
		   &top,
		   &x_advance,
		   &y_advance );
	   return 2;    
   }
   else
   {
	   get_glyph_bitmap( 
		   lpfd,
		   *lpText,
		   lpbmp,
		   &left,
		   &top,
		   &x_advance,
		   &y_advance );
	   return 1;
   }
	//lpMask->bmWidth = 8;
    //lpMask->bmHeight = 8;
    //lpMask->bmWidthBytes = 1;
    //lpMask->bmBits = &eng8x8Mask[(*lpText) << 3];
}

static int TTF_TextWidth( HANDLE handle, const BYTE FAR* lpText, int len )
{
   int  w = 0;

   if( lpText )
   {
       while( !IS_TEXT_MARK( *lpText ) && *lpText && len )
       {
           if( IS_CHINESE( lpText ) )
           {
               w += TTF_WordWidth( handle, _GET_CHINESE( lpText ) );
               lpText += 2;
               if( len > 0 )
                   len -= 2;
           }
           else
           {
               w += TTF_WordWidth( handle, _GET_ENGLISH( lpText ) );
               lpText += 1;
               if( len > 0 )
                   len--;
           }
       }
   }
   return w;
}

static int TTF_TextHeight( HANDLE handle, const BYTE FAR* lpText, int aLineWidth )
{
    int h = 0, w = 0;
	int fh;

    if( lpText )
    {
        h = fh = TTF_WordHeight( handle, 'O' );//FONT_HEIGHT;
        do  {
            if( IS_CHINESE( lpText ) )
            {
				int cw = TTF_WordWidth( handle, _GET_CHINESE( lpText ) );
                if( aLineWidth > 0 && 
					w + cw >= aLineWidth )
                {
                    h += fh;
                    w = 0;
                }
                else
				{
                    //w += CHINESE_WIDTH;
					w += cw;//TTF_WordWidth( handle, _GET_CHINESE( lpText ) );
				}
                lpText += 2;
            }
            else
            {  // english font
				int ew = TTF_WordWidth( handle, _GET_ENGLISH( lpText ) );
                if( (aLineWidth > 0 && 
					 w + ew >= aLineWidth) ||
                    IS_TEXT_MARK( *lpText ) )
                {
                    h += fh;
                    w = 0;
                }
                else
				{
                    //w += ENGLISH_WIDTH;
					w += ew;//WordWidth( handle, _GET_ENGLISH( lpText ) );
				}
                lpText++;
            }
        }while( *lpText );
    }
    return h;
}

static const BYTE FAR* TTF_NextWord( HANDLE handle, const BYTE FAR* lpText )
{
    return IS_CHINESE( lpText ) ? (lpText + 2) : (lpText + 1);
}

static int TTF_WordLength( HANDLE handle, const BYTE FAR* lpText )
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
