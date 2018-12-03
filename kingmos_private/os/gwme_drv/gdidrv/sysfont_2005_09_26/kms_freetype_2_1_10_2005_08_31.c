
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_CACHE_H

#include FT_CACHE_IMAGE_H
#include FT_CACHE_SMALL_BITMAPS_H
#include FT_BITMAP_H

//#include "ftcommon.c"

#include <edef.h>
#include <erect.h>
#include <egdi.h>
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
static int TTF_WordMask( HANDLE, const BYTE FAR* lpText, _LPCHAR_METRICS );
static int TTF_TextWidth( HANDLE, const BYTE FAR* lpText, int aLimiteWidth );
static int TTF_TextHeight( HANDLE, const BYTE FAR* lpText, int aLineWidth );
static const BYTE FAR* TTF_NextWord( HANDLE, const BYTE FAR* lpText );

static void _DeleteObject( void );
static int _Resume();
//static WORD GetCodeIndex( LPCBYTE );
typedef WORD (*LPGetCodeIndex)( LPCBYTE lpbCode );


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

#define IS_CHINESE( lpText ) ( (*(lpText) & 0x80) )
#define _GET_CHINESE( lpfd, lpText )  ( (lpfd)->lpGetCodeIndex( (lpText) )  )
#define _GET_ENGLISH( lpText ) ( *(lpText) )
#define IS_TEXT_MARK( v ) ( (v) == TEXT_SOFT_BREAK || (v) == TEXT_KEY_BREAK || (v) == TEXT_EOF )

  
typedef struct  TFont_
{
    const char*  filepathname;
    int          face_index;
    int          cmap_index;	
    int          num_indices;	
} TFont, *PFont;


static const char*  file_suffixes[] =
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
  
static int  lcd_mode  = 0;         /* 0 - 5                       */
static FT_Encoding  encoding = FT_ENCODING_UNICODE;//FT_ENCODING_NONE;

static FT_Bitmap  ft_bitmap;

//static  FT_Library       library;      /* the FreeType library            */
//static  FTC_Manager      manager;      /* the cache manager               */
//static  FTC_Image_Cache  image_cache;  /* the glyph image cache           */
//static  FTC_SBit_Cache   sbits_cache;  /* the glyph small bitmaps cache   */
static  FT_Library      library;       /* the FreeType library            */
static  FTC_Manager     cache_manager; /* the cache manager               */
static  FTC_ImageCache  image_cache;   /* the glyph image cache           */
static  FTC_SBitCache   sbits_cache;   /* the glyph small bitmaps cache   */
static  FTC_CMapCache   cmap_cache;    /* the charmap cache..             */


//static  FT_Face          face;         /* the font face                   */
//static  FT_Size          size;         /* the font size                   */
//static  FT_GlyphSlot     glyph;        /* the glyph slot                  */

//static  FTC_Image_Desc   current_font;
static FT_Error  error;
static int    use_sbits_cache  = 1;
static int    res = 72;
static   int  antialias = 0;//1;         /* is anti-aliasing active?    */

static int  hinted    = 1;         /* is glyph hinting active?    */

static   int  use_sbits = 1;         /* do we use embedded bitmaps? */
static   int  low_prec  = 0;         /* force low precision         */
static   int  autohint  = 0;//0;         /* force auto-hinting          */
static   int  Num;                   /* current first glyph index   */

typedef struct _FONT_DATA
{
	//FTC_Image_Desc font;
	FTC_ImageTypeRec font;

	FT_Face  lpFace;
	LPGetCodeIndex lpGetCodeIndex;
	int yBaseline;
}FONT_DATA, FAR * LPFONT_DATA;


  /* pixel mode constants */
  typedef enum grPixelMode
  {
    gr_pixel_mode_none = 0,
    gr_pixel_mode_mono,        /* monochrome bitmaps               */
    gr_pixel_mode_pal4,        /* 4-bit paletted - 16 colors       */
    gr_pixel_mode_pal8,        /* 8-bit paletted - 256 colors      */
    gr_pixel_mode_gray,        /* 8-bit gray levels                */
    gr_pixel_mode_rgb555,      /* 15-bits mode - 32768 colors      */
    gr_pixel_mode_rgb565,      /* 16-bits mode - 65536 colors      */
    gr_pixel_mode_rgb24,       /* 24-bits mode - 16 million colors */
    gr_pixel_mode_rgb32,       /* 32-bits mode - 16 million colors */
    gr_pixel_mode_lcd,         /* horizontal RGB-decimated         */
    gr_pixel_mode_lcdv,        /* vertical RGB-decimated           */
    gr_pixel_mode_lcd2,        /* horizontal BGR-decimated         */
    gr_pixel_mode_lcdv2,       /* vertical BGR-decimated           */

    gr_pixel_mode_max          /* don't remove */

  } grPixelMode;

  typedef struct grBitmap_
  {
    int             rows;
    int             width;
    int             pitch;
    grPixelMode     mode;
    int             grays;
    unsigned char*  buffer;

  } grBitmap;

  static FT_Error
  get_glyph_bitmap( 
					LPFONT_DATA lpfd,
					FT_ULong     Index,
                    grBitmap*    target,
                    int         *left,
                    int         *top,
                    int         *x_advance,
                    int         *y_advance,
                    FT_Pointer  *aglyf );

  static FT_Error
  glyph_to_bitmap( 
					LPFONT_DATA lpfd,
					FT_Glyph    glyf,
                   grBitmap*   target,
                   int        *left,
                   int        *top,
                   int        *x_advance,
                   int        *y_advance,
                   FT_Pointer *aref );




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

static void
init_freetype( void )
{
    error = FT_Init_FreeType( &library );
    //if ( error )
      //PanicZ( "could not initialize FreeType" );

    error = FTC_Manager_New( library, 0, 0, 0,
                             my_face_requester, 0, &cache_manager );
    //if ( error )
      //PanicZ( "could not initialize cache manager" );

    error = FTC_SBitCache_New( cache_manager, &sbits_cache );
    //if ( error )
      //PanicZ( "could not initialize small bitmaps cache" );

    error = FTC_ImageCache_New( cache_manager, &image_cache );
    //if ( error )
      //PanicZ( "could not initialize glyph image cache" );

    error = FTC_CMapCache_New( cache_manager, &cmap_cache );
    //if ( error )
      //PanicZ( "could not initialize charmap cache" );

    FT_Bitmap_New( &ft_bitmap );
}
/*
static int is_installed( const char*  filepath, int * index )
{
	int i;
	for( i = 0; i < num_fonts; i++ )
	{
		if( strcmp( fonts[i].filepathname, filepath ) == 0 )
		{
			if( index )
				*index = i;
			return 1;	
		}
	}
	return 0;
}
*/
static int install_font_file( const char*  filepath )
{
    static char   filename[1024 + 5];
    int           i, len, suffix_len, num_faces;
    const char**  suffix;
	FT_Face          face;         /* the font face                   */
	
    len = strlen( filepath );
    if ( len > 1024 )
		len = 1024;
	
    strncpy( filename, filepath, len );
    filename[len] = 0;
	
    error = FT_New_Face( library, filename, 0, &face );
    if ( !error )
		goto Success;
	
    /* could not open the file directly; we will now try various */
    /* suffixes like `.ttf' or `.pfb'                            */
	
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
		
		if ( encoding != FT_ENCODING_NONE )
		{
			error = FT_Select_Charmap( face, encoding );
			if ( error )
			{
				FT_Done_Face( face );
				face = NULL;
				continue;
			}
		}
		
		font = (PFont)malloc( sizeof ( *font ) );
		font->filepathname = (char*)malloc( strlen( filename ) + 1 );
		font->face_index   = i;
		font->cmap_index   = face->charmap ? FT_Get_Charmap_Index( face->charmap ) : 0;
		
		switch ( encoding )
		{
		case FT_ENCODING_NONE:
			font->num_indices = face->num_glyphs;
			break;
			
		case FT_ENCODING_UNICODE:
			font->num_indices = 0x110000L;
			break;
			
		case FT_ENCODING_MS_SYMBOL:
		case FT_ENCODING_ADOBE_LATIN_1:
		case FT_ENCODING_ADOBE_STANDARD:
		case FT_ENCODING_ADOBE_EXPERT:
		case FT_ENCODING_ADOBE_CUSTOM:
		case FT_ENCODING_APPLE_ROMAN:
			font->num_indices = 0x100L;
			break;
			
		default:
			font->num_indices = 0x10000L;
		}
		
		strcpy( (char*)font->filepathname, filename );
		
		FT_Done_Face( face );
		face = NULL;
		
		if ( max_fonts == 0 )
		{
			max_fonts = 16;
			fonts     = (PFont*)calloc( max_fonts, sizeof ( PFont ) );
		}
		else if ( num_fonts >= max_fonts )
		{
			max_fonts *= 2;
			fonts      = (PFont*)realloc( fonts, max_fonts * sizeof ( PFont ) );
			
			memset( &fonts[num_fonts], 0,
                ( max_fonts - num_fonts ) * sizeof ( PFont ) );
		}
		
		fonts[num_fonts++] = font;
    }
	
    return 0;
}


  static FT_Error
  glyph_to_bitmap( 
					LPFONT_DATA lpfd,
					FT_Glyph    glyf,
                   grBitmap*   target,
                   int        *left,
                   int        *top,
                   int        *x_advance,
                   int        *y_advance,
                   FT_Pointer *aref )
  {
    FT_BitmapGlyph  bitmap;
    FT_Bitmap*      source;


    *aref = NULL;

    error = FT_Err_Ok;

    if ( glyf->format == FT_GLYPH_FORMAT_OUTLINE )
    {
      /* render the glyph to a bitmap, don't destroy original */
      error = FT_Glyph_To_Bitmap( &glyf,
                                  antialias ? FT_RENDER_MODE_NORMAL
                                            : FT_RENDER_MODE_MONO,
                                  NULL, 0 );
      if ( error )
        goto Exit;

      *aref = glyf;
    }

    if ( glyf->format != FT_GLYPH_FORMAT_BITMAP )
	{
      ;//PanicZ( "invalid glyph format returned!" );
	}

    bitmap = (FT_BitmapGlyph)glyf;
    source = &bitmap->bitmap;

    target->rows   = source->rows;
    target->width  = source->width;
    target->pitch  = source->pitch;
    target->buffer = source->buffer;

    switch ( source->pixel_mode )
    {
    case FT_PIXEL_MODE_MONO:
      target->mode  = gr_pixel_mode_mono;
      target->grays = 2;
      break;

    case FT_PIXEL_MODE_GRAY:
      target->mode  = gr_pixel_mode_gray;
      target->grays = source->num_grays;
      break;

    case FT_PIXEL_MODE_GRAY2:
    case FT_PIXEL_MODE_GRAY4:
      (void)FT_Bitmap_Convert( library, source, &ft_bitmap, 1 );
      target->pitch  = ft_bitmap.pitch;
      target->buffer = ft_bitmap.buffer;
      target->mode   = gr_pixel_mode_gray;
      target->grays  = ft_bitmap.num_grays;
      break;

    case FT_PIXEL_MODE_LCD:
      target->mode  = lcd_mode == 2 ? gr_pixel_mode_lcd
                                    : gr_pixel_mode_lcd2;
      target->grays = source->num_grays;
      break;

    case FT_PIXEL_MODE_LCD_V:
      target->mode  = lcd_mode == 4 ? gr_pixel_mode_lcdv
                                    : gr_pixel_mode_lcdv2;
      target->grays = source->num_grays;
      break;

    default:
      return FT_Err_Invalid_Glyph_Format;
    }

    *left = bitmap->left;
    *top  = bitmap->top;

    *x_advance = ( glyf->advance.x + 0x8000 ) >> 16;
    *y_advance = ( glyf->advance.y + 0x8000 ) >> 16;

  Exit:
    return error;
  }

 static FT_UInt
  get_glyph_index( FT_UInt32  charcode, FTC_FaceID face_id )
  {
    //FTC_FaceID  face_id = current_font.face_id;
    PFont       font    = (PFont)face_id;


    return FTC_CMapCache_Lookup( cmap_cache, face_id,
                                 font->cmap_index, charcode );
  }


  static FT_Error
  get_glyph_bitmap( 
					LPFONT_DATA lpfd,
					FT_ULong     Index,
                    grBitmap*    target,
                    int         *left,
                    int         *top,
                    int         *x_advance,
                    int         *y_advance,
                    FT_Pointer  *aglyf )
  {
    *aglyf = NULL;

    if ( encoding != FT_ENCODING_NONE )
      Index = get_glyph_index( Index, lpfd->font.face_id );

    /* use the SBits cache to store small glyph bitmaps; this is a lot */
    /* more memory-efficient                                           */
    /*                                                                 */
    if ( use_sbits_cache          &&
         lpfd->font.width  < 48 &&
         lpfd->font.height < 48 )
    {
      FTC_SBit   sbit;
      FT_Bitmap  source;


      error = FTC_SBitCache_Lookup( sbits_cache,
                                    &lpfd->font,
                                    Index,
                                    &sbit,
                                    NULL );
      if ( error )
        goto Exit;

      if ( sbit->buffer )
      {
        target->rows   = sbit->height;
        target->width  = sbit->width;
        target->pitch  = sbit->pitch;
        target->buffer = sbit->buffer;

        switch ( sbit->format )
        {
        case FT_PIXEL_MODE_MONO:
          target->mode  = gr_pixel_mode_mono;
          target->grays = 2;
          break;

        case FT_PIXEL_MODE_GRAY:
          target->mode  = gr_pixel_mode_gray;
          target->grays = sbit->max_grays + 1;
          break;

        case FT_PIXEL_MODE_GRAY2:
        case FT_PIXEL_MODE_GRAY4:
          source.rows       = sbit->height;
          source.width      = sbit->width;
          source.pitch      = sbit->pitch;
          source.buffer     = sbit->buffer;
          source.pixel_mode = sbit->format;
          (void)FT_Bitmap_Convert( library, &source, &ft_bitmap, 1 );

          target->pitch  = ft_bitmap.pitch;
          target->buffer = ft_bitmap.buffer;
          target->mode   = gr_pixel_mode_gray;
          target->grays  = ft_bitmap.num_grays;
          break;

        case FT_PIXEL_MODE_LCD:
          target->mode  = lcd_mode == 2 ? gr_pixel_mode_lcd
                                        : gr_pixel_mode_lcd2;
          target->grays = sbit->max_grays + 1;
          break;

        case FT_PIXEL_MODE_LCD_V:
          target->mode  = lcd_mode == 4 ? gr_pixel_mode_lcdv
                                        : gr_pixel_mode_lcdv2;
          target->grays = sbit->max_grays + 1;
          break;

        default:
          return FT_Err_Invalid_Glyph_Format;
        }

        *left      = sbit->left;
        *top       = sbit->top;
        *x_advance = sbit->xadvance;
        *y_advance = sbit->yadvance;

        goto Exit;
      }
    }

    /* otherwise, use an image cache to store glyph outlines, and render */
    /* them on demand. we can thus support very large sizes easily..     */
    {
      FT_Glyph   glyf;

      error = FTC_ImageCache_Lookup( image_cache,
                                     &lpfd->font,
                                     Index,
                                     &glyf,
                                     NULL );

      if ( !error )
        error = glyph_to_bitmap( lpfd, glyf, target, left, top, x_advance, y_advance, aglyf );
    }

  Exit:
    /* don't accept a `missing' character with zero or negative width */
    if ( Index == 0 && *x_advance <= 0 )
      *x_advance = 1;

    return error;
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
static void done_freetype( void )
{
    int  i;
	
	
    for ( i = 0; i < max_fonts; i++ )
    {
		if ( fonts[i] )
		{
			if ( fonts[i]->filepathname )
				free( (void*)fonts[i]->filepathname );
			free( fonts[i] );
		}
    }
    free( fonts );
	
    FTC_Manager_Done( cache_manager );
    FT_Bitmap_Done( library, &ft_bitmap );
    FT_Done_FreeType( library );
}


static WORD GB2312_GetCodeIndex( LPCBYTE lpbCode )
{
	extern const unsigned short _936code[];
	//extern const unsigned short _949code[];

	int hiByte = *lpbCode++;
	int loByte = *lpbCode;
	int offset;
	// 936
    offset = (hiByte - 0x81) * 190 + (loByte - 0x40) - ( loByte > 0x7e ); 
	return _936code[offset];

}

static WORD BIG5_GetCodeIndex( LPCBYTE lpbCode )
{
	extern const unsigned short _950code[];
	//extern const unsigned short _949code[];

	int hiByte = *lpbCode++;
	int loByte = *lpbCode;
	int offset;
	// 936
    offset = (hiByte - 0x81) * 157 + (loByte - 0x40) - ( loByte > 0x7e ); 
	return _950code[offset];

}


static WORD HANGUL_GetCodeIndex( LPCBYTE lpbCode )
{
	extern const unsigned short _949code[];

	int hiByte = *lpbCode++;
	int loByte = *lpbCode;
	int offset;
	//949
	offset = (hiByte - 0x81) * 	178 + (loByte - 0x41) - ( loByte > 0x7e ); 
	return _949code[offset];	
}


static DWORD TTF_Init( void )
{
	static BOOL bIsInit = FALSE;
   //_Resume();
	if( bIsInit == FALSE )
	{
#ifdef VC386		
		_INTR_OFF();
#endif
		init_freetype();
		//install_font_file( "sunfon.ttf" );
		//install_font_file( "yhunifont.pcf" );
		//install_font_file( "gulim_1_30.ttc" );
		bIsInit = TRUE;	
#ifdef VC386		
		_INTR_ON();
#endif
	}
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
		char * lpszFontFileName;
		
		
		memset( lpfd, 0, sizeof( *lpfd ) );
		if( lplf )
		{
			lpfd->font.width  = (short)lplf->lfWidth;
			lpfd->font.height = (short)lplf->lfHeight;
			switch( lplf->lfCharSet )
			{
			case GB2312_CHARSET:
#ifdef LINUX_KERNEL
				lpszFontFileName = "./kingmos/sunfon.ttf";
#else
				lpszFontFileName = ".\\kingmos\\sunfon.ttf";
#endif
				lpfd->lpGetCodeIndex = GB2312_GetCodeIndex;
				break;
			case CHINESEBIG5_CHARSET:
#ifdef LINUX_KERNEL
				lpszFontFileName = "./kingmos/mingliu.ttc";
#else
				lpszFontFileName = ".\\kingmos\\mingliu.ttc";
#endif
				lpfd->lpGetCodeIndex = BIG5_GetCodeIndex;
				break;
			case HANGUL_CHARSET:
#ifdef LINUX_KERNEL
				lpszFontFileName = "./kingmos/gulim.ttc";
#else
				lpszFontFileName = ".\\kingmos\\gulim.ttc";
#endif
				lpfd->lpGetCodeIndex = HANGUL_GetCodeIndex;
				break;
			}
			
			//install_font_file( lpszFontFileName );
		}
			
		else
		{
			lpfd->font.width = 16;
			lpfd->font.height = 16;
#ifdef LINUX_KERNEL
			lpszFontFileName = "./kingmos/sunfon.ttf";
#else
			lpszFontFileName = ".\\kingmos\\sunfon.ttf";
#endif
			lpfd->lpGetCodeIndex = GB2312_GetCodeIndex;

			
		}
		if( lpszFontFileName )
		{
			int retv, index;
			
//			if( is_installed( lpszFontFileName, &index )  )
//			{
//
//				retv = 0;
//			}
//			else
			{
#ifdef VC386		
				_INTR_OFF();
#endif
				retv = install_font_file( lpszFontFileName );
				
#ifdef VC386		
				_INTR_ON();			
#endif		
				index = num_fonts - 1;
			}
			
			if( retv == 0 )
			{
				lpfd->font.face_id = (FTC_FaceID)fonts[index];
				
				
				lpfd->font.flags = antialias ? FT_LOAD_DEFAULT : FT_LOAD_TARGET_MONO;
				//lpfd->font.image_type = ftc_image_mono;
				
				if ( !hinted )
					lpfd->font.flags |= FT_LOAD_NO_HINTING;
				
				if ( autohint )
					lpfd->font.flags |= FT_LOAD_FORCE_AUTOHINT;
				
				if ( !use_sbits )
					lpfd->font.flags |= FT_LOAD_NO_BITMAP;
				lpfd->yBaseline = lpfd->font.height / 8;
			}
			else
			{
				free( lpfd );
				lpfd = NULL;
			}
		}
		else
		{
			free( lpfd );
			lpfd = NULL;
		}
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
    //FT_Done_Face( lpfd->lpFace->face ); //??
	free( lpfd );

	return TRUE;
}

static int TTF_MaxHeight( HANDLE handle )
{
	LPFONT_DATA lpfd = (LPFONT_DATA)handle;
	return lpfd->font.height;
}

static int TTF_MaxWidth( HANDLE handle )
{
	LPFONT_DATA lpfd = (LPFONT_DATA)handle;
    return lpfd->font.width / 2; //µ¥×Ö½Ú¿í¶È
}

static int TTF_WordHeight( HANDLE handle, WORD aWord )
{
    LPFONT_DATA lpfd = (LPFONT_DATA)handle;

	return lpfd->font.height;
/*
	int left, top, x_advance, y_advance;
	grBitmap grbmp;
	FT_Pointer  glyf;


   get_glyph_bitmap( 
		   lpfd,
		   aWord,
		   &grbmp,
		   &left,
		   &top,
		   &x_advance,
		   &y_advance,
		   &glyf );
   return grbmp.rows;
*/
}

static int TTF_WordWidth( HANDLE handle, WORD aWord )
{
    LPFONT_DATA lpfd = (LPFONT_DATA)handle;
	return ( aWord < 0xff ) ? lpfd->font.width / 2 : lpfd->font.width;

/*
	int left, top, x_advance, y_advance;
	grBitmap grbmp;
	FT_Pointer  glyf;


   get_glyph_bitmap( 
		   lpfd,
		   aWord,
		   &grbmp,//
		   &left,
		   &top,
		   &x_advance,
		   &y_advance,
		   &glyf );
   return x_advance;
*/
}

static int TTF_WordMask( HANDLE handle, const BYTE FAR* lpText, _LPCHAR_METRICS lpbmp )
{
    LPFONT_DATA lpfd = (LPFONT_DATA)handle;
	//int left, top;//, x_advance, y_advance;
	BYTE bUnicode[] = { 0x60, 0x4f };
	grBitmap grbmp;
	FT_Pointer  glyf;
	int n;


//   lpbmp->bitmap.bmPlanes = 1;
//   lpbmp->bitmap.bmType = 0;

   if( IS_CHINESE( lpText ) )
   {
	   get_glyph_bitmap( 
		   lpfd,
		   _GET_CHINESE( lpfd, lpText ),
		   &grbmp,//lpbmp,
		   &lpbmp->left,
		   &lpbmp->top,
		   &lpbmp->xAdvance,
		   &lpbmp->yAdvance,
		   &glyf );
	   lpbmp->bitmap.bmWidth = grbmp.width;
	   lpbmp->bitmap.bmHeight = grbmp.rows;
	   lpbmp->bitmap.bmBits = grbmp.buffer;
	   lpbmp->bitmap.bmWidthBytes = grbmp.pitch;

	   lpbmp->top = lpfd->font.height - lpbmp->top;
	   n = lpfd->font.height - lpbmp->bitmap.bmHeight;
	   if( lpbmp->top > n )
		   lpbmp->top = n;

	   //lpbmp->xAdvance -= (lpbmp->xAdvance - grbmp.width) /2;
	   //lpbmp->xAdvance = grbmp.width + lpbmp->left;


	   return 2;    
   }
   else
   {
	   
	   get_glyph_bitmap( 
		   lpfd,
		   *lpText,
		   &grbmp,//lpbmp,
		   &lpbmp->left,
		   &lpbmp->top,
		   &lpbmp->xAdvance,
		   &lpbmp->yAdvance,
		   &glyf );
	   lpbmp->bitmap.bmWidth = grbmp.width;
	   lpbmp->bitmap.bmHeight = grbmp.rows;
	   lpbmp->bitmap.bmBits = grbmp.buffer;
	   lpbmp->bitmap.bmWidthBytes = grbmp.pitch;

	   //lpbmp->top = lpfd->font.height - lpbmp->top - 1;
	   n = lpfd->font.height - lpbmp->top - lpfd->yBaseline;
	   if( n < 0 )
		   n = 0;
	   lpbmp->top = n;
	   n = lpfd->font.height - lpbmp->bitmap.bmHeight;
	   if( lpbmp->top > n )
		   lpbmp->top = n;


		//lpbmp->xAdvance = x_advance;
		//lpbmp->yAdvance = 0;		
	   lpbmp->xAdvance /= 2;//grbmp.width + lpbmp->left;

		//if( lpbmp->xAdvance > grbmp.width )
		//	lpbmp->xAdvance = grbmp.width + 1;//++;// -= grbmp.width + ;
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
   LPFONT_DATA lpfd = (LPFONT_DATA)handle;

   if( lpText )
   {
       while( !IS_TEXT_MARK( *lpText ) && *lpText && len )
       {
           if( IS_CHINESE( lpText ) )
           {
               w += TTF_WordWidth( handle, _GET_CHINESE( lpfd, lpText ) );
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
	LPFONT_DATA lpfd = (LPFONT_DATA)handle;
	

    if( lpText )
    {
        h = fh = TTF_WordHeight( handle, 'O' );//FONT_HEIGHT;
        do  {
            if( IS_CHINESE( lpText ) )
            {
				int cw = TTF_WordWidth( handle, _GET_CHINESE( lpfd, lpText ) );
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
