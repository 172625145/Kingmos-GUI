/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef __GIF_LIB_H
#define __GIF_LIB_H

#define GIF_LIB_VERSION	" Version 1.2, "

#define	GIF_ERROR	0
#define GIF_OK		1

#ifndef TRUE
#define TRUE		1
#define FALSE		0
#endif

#define GIF_FILE_BUFFER_SIZE 16384  /* Files uses bigger buffers than usual. */


#define GIF_MESSAGE(Msg) EdbgOutputDebugString("\n%s: %s\n", PROGRAM_NAME, Msg)
#define GIF_EXIT(Msg)	{ GIF_MESSAGE(Msg); exit(-3); }

//#define VoidPtr void *

typedef struct GifColorType 
{
    BYTE Red;
	BYTE Green;
	BYTE Blue;
} _PACKED_ GIFCOLORTYPE, *LPGIFCOLORTYPE;

typedef GIFCOLORTYPE		GifColorType ;

/* Note entries prefixed with S are of Screen information, while entries     */
/* prefixed with I are of the current defined Image.			     */
typedef struct GifFileType
{
    int SWidth, SHeight,			       /* Screen dimensions. */
	SColorResolution, SBitsPerPixel, /* How many colors can we generate? */
	SBackGroundColor,		/* I hope you understand this one... */
	ILeft, ITop, IWidth, IHeight,		/* Current image dimensions. */
	IInterlace,			     /* Sequential/Interlaced lines. */
	IBitsPerPixel;			  /* How many colors this image has? */
    GIFCOLORTYPE *SColorMap, *IColorMap;	      /* NULL if not exists. */
    void  *lpPrivate;	  /* The regular user should not mess with this one! */
} _PACKED_ GIFFILETYPE, *LPGIFFILETYPE ;

typedef GIFFILETYPE		GifFileType ;

typedef enum
{
    UNDEFINED_RECORD_TYPE,
    SCREEN_DESC_RECORD_TYPE,
    IMAGE_DESC_RECORD_TYPE,				   /* Begin with ',' */
    EXTENSION_RECORD_TYPE,				   /* Begin with '!' */
    TERMINATE_RECORD_TYPE				   /* Begin with ';' */
} _PACKED_ GIFRECORDTYPE, *LPGIFRECORDTYPE;

typedef GIFRECORDTYPE		GifRecordType ;

LPGIFFILETYPE DGifOpenFileName( const TCHAR *GifFileName );

LPGIFFILETYPE DGifOpenFileHandle( HANDLE hFile );

int DGifGetScreenDesc( LPGIFFILETYPE lpGifFile );

int DGifGetRecordType( LPGIFFILETYPE lpGifFile, LPGIFRECORDTYPE lpGifType);

int DGifGetImageDesc( LPGIFFILETYPE lpGifFile );
int DGifSetImageOffset( LPGIFFILETYPE lpGifFile, LONG dwOffset );
DWORD DGifGetImageOffset( LPGIFFILETYPE lpGifFile );

int DGifGetLine( LPGIFFILETYPE lpGifFile , BYTE *GifLine, int GifLineLen);

int DGifGetPixel( LPGIFFILETYPE lpGifFile , BYTE GifPixel);

int DGifGetComment( LPGIFFILETYPE lpGifFile , char *GifComment);

int DGifGetExtension( LPGIFFILETYPE lpGifFile , int *GifExtCode,
						BYTE **GifExtension);

int DGifGetExtensionNext( LPGIFFILETYPE lpGifFile , BYTE **GifExtension);

int DGifGetCode( LPGIFFILETYPE lpGifFile , int *GifCodeSize,
						BYTE **GifCodeBlock);

int DGifGetCodeNext( LPGIFFILETYPE lpGifFile , BYTE **GifCodeBlock);

int DGifGetLZCodes( LPGIFFILETYPE lpGifFile , int *GifCode);

int DGifCloseFile( LPGIFFILETYPE lpGifFile );

#define	D_GIF_ERR_OPEN_FAILED			101		/* And DGif possible errors. */
#define	D_GIF_ERR_READ_FAILED			102
#define	D_GIF_ERR_NOT_GIF_FILE			103
#define D_GIF_ERR_NO_SCRN_DSCR			104
#define D_GIF_ERR_NO_IMAG_DSCR			105
#define D_GIF_ERR_NO_COLOR_MAP			106
#define D_GIF_ERR_WRONG_RECORD			107
#define D_GIF_ERR_DATA_TOO_BIG			108
#define D_GIF_ERR_NOT_ENOUGH_MEM		109
#define D_GIF_ERR_CLOSE_FAILED			110
#define D_GIF_ERR_NOT_READABLE			111
#define D_GIF_ERR_IMAGE_DEFECT			112
#define D_GIF_ERR_EOF_TOO_SOON			113

/******************************************************************************
* O.k. here are the routines from GIF_LIB file GIF_ERR.C.		      *
******************************************************************************/
void PrintGifError( void );
int GifLastError( void );


#endif // __GIF_LIB_H
