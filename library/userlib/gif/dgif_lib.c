/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/


/*****************************************************
文件说明：将gif库
版本号：1.0.0
开发时期：2003-03-06
作者：孔
修改记录：
******************************************************/

#include <ewindows.h>
#include "gif_lib.h"

#define PROGRAM_NAME			"GIF_LIBRARY"

#define COMMENT_EXT_FUNC_CODE	0xfe /* Extension function code for comment. */
#define GIF_STAMP				"GIFVER"	 /* First chars in file - GIF stamp. */
#define GIF_STAMP_LEN			sizeof(GIF_STAMP) - 1
#define GIF_VERSION_POS			3		/* Version first character in stamp. */

#define LZ_MAX_CODE				4095		/* Biggest code possible in 12 bits. */
#define LZ_BITS					12

//#define FILE_STATE_READ			0x01/* 1 write, 0 read - EGIF_LIB compatible.*/

//#define FLUSH_OUTPUT			4096    /* Impossible code, to signal flush. */
//#define FIRST_CODE				4097    /* Impossible code, to signal first. */
#define NO_SUCH_CODE			4098    /* Impossible code, to signal empty. */

#define IS_READABLE(lpPrivate)	TRUE

typedef struct GifFilePrivateType
{
    int				BitsPerPixel;	    /* Bits per pixel (Codes uses at list this + 1). */
	int				ClearCode;				       /* The CLEAR LZ code. */
	int				EOFCode;				         /* The EOF LZ code. */
	int				RunningCode;		    /* The next code algorithm can generate. */
	int				RunningBits;/* The number of bits required to represent RunningCode. */
	int				MaxCode1;  /* 1 bigger than max. possible code, in RunningBits bits. */
	int				LastCode;		        /* The code before the current code. */
	int				CrntCode;				  /* Current algorithm code. */
	int				StackPtr;		         /* For character stack (see below). */
	int				CrntShiftState;		        /* Number of bits in CrntShiftDWord. */
    unsigned long	CrntShiftDWord;     /* For bytes decomposition into codes. */
	unsigned long	PixelCount;		       /* Number of pixels in image. */
    HANDLE			hFile;						  /* File as stream. */
    BYTE			Buf[256];	       /* Compressed input is buffered here. */
    BYTE			Stack[LZ_MAX_CODE];	 /* Decoded pixels are stacked here. */
    BYTE			Suffix[LZ_MAX_CODE+1];	       /* So we can trace the codes. */
    unsigned int	Prefix[LZ_MAX_CODE+1];
} GIFFILEPRIVATETYPE, *LPGIFFILEPRIVATETYPE ;

extern int g_GifError;

static int DGifGetWord( HANDLE hFile, int *Word );
static int DGifSetupDecompress( LPGIFFILETYPE lpGifFile );
static int DGifDecompressLine( LPGIFFILETYPE lpGifFile, LPBYTE lpbLine,
								int LineLen);
static int DGifGetPrefixChar( unsigned int *Prefix, int Code, int ClearCode);
static int DGifDecompressInput( LPGIFFILEPRIVATETYPE lpPrivate, int *Code);
static int DGifBufferedInput( HANDLE hFile, BYTE *Buf,
						     BYTE *NextByte );

/******************************************************************************
   Open a new gif file for read, given by its name.			      
   Returns GifFileType pointer dynamically allocated which serves as the gif 
   info record. g_GifError is cleared if succesfull.			      
******************************************************************************/
LPGIFFILETYPE DGifOpenFileName( const TCHAR *FileName )
{
	HANDLE		hFile ;

	hFile = CreateFile( FileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, NULL ) ;
	if( hFile == INVALID_HANDLE_VALUE )
	{
		g_GifError = D_GIF_ERR_OPEN_FAILED;
		return NULL;
    }

    return DGifOpenFileHandle( hFile );
}

/******************************************************************************
   Update a new gif file, given its file handle.			      
   Returns GifFileType pointer dynamically allocated which serves as the gif 
   info record. g_GifError is cleared if succesfull.			      
******************************************************************************/
LPGIFFILETYPE DGifOpenFileHandle( HANDLE hFile )
{
    char					Buf[GIF_STAMP_LEN+1];
    LPGIFFILETYPE			lpGifFile;
    LPGIFFILEPRIVATETYPE	lpPrivate;
	DWORD					dwSize ;
	BOOL					bRet ;

    if ( ( lpGifFile = ( LPGIFFILETYPE )malloc( sizeof( GIFFILETYPE ) ) ) == NULL )
	{
		g_GifError = D_GIF_ERR_NOT_ENOUGH_MEM;
		return NULL;
    }

    if( ( lpPrivate = ( LPGIFFILEPRIVATETYPE )malloc( sizeof( GIFFILEPRIVATETYPE ) ) ) == NULL)
	{
		g_GifError = D_GIF_ERR_NOT_ENOUGH_MEM;
		free( ( char * ) lpGifFile );
		return NULL;
    }
    lpGifFile -> lpPrivate = ( void * ) lpPrivate;
    lpGifFile -> SColorMap = lpGifFile -> IColorMap = NULL;
    lpPrivate -> hFile = hFile;

    /* Lets see if this is GIF file: Read the gif file header
	*/
	bRet = ReadFile( hFile, Buf, GIF_STAMP_LEN, &dwSize, NULL ) ;
    if( ( bRet == FALSE )||( dwSize != GIF_STAMP_LEN ) )
	{
		g_GifError = D_GIF_ERR_READ_FAILED;
		free((char *) lpPrivate);
		free((char *) lpGifFile);
		return NULL;
    }

    /* The GIF Version number is ignored at this time. Maybe we should do    */
    /* something more useful with it.					     */
    Buf[GIF_STAMP_LEN] = 0;
//    if( ( strncmp("GIF89a", Buf, GIF_STAMP_LEN ) != 0 )&&
//		( strncmp("GIF87a", Buf, GIF_STAMP_LEN ) != 0 ) )
//!!!!!!!!!!!Gif89a still have some problem, update later!!!!!!!!!!!
// by kingkong 2002.3.26
	
	//lilin -add this code-2004-05-05
    if( strncmp(GIF_STAMP, Buf, GIF_VERSION_POS ) != 0 )
	{
		// the file is not gif file or the file struct is unknow gif...
		g_GifError = D_GIF_ERR_NOT_GIF_FILE;
		free((char *) lpPrivate);
		free((char *) lpGifFile);
		return NULL;
    }
	//
	

    if( DGifGetScreenDesc( lpGifFile ) == GIF_ERROR )
	{
		free((char *) lpPrivate);
		free((char *) lpGifFile);
		return NULL;
    }

    g_GifError = 0;

    return lpGifFile;
}

/******************************************************************************
	This routine should be called before any other DGif calls. Note that      
	this routine is called automatically from DGif file open routines.
	this routine get the gif global informations
******************************************************************************/
int DGifGetScreenDesc( LPGIFFILETYPE lpGifFile )
{
    int						iSize;
    BYTE					Buf[3];
    LPGIFFILEPRIVATETYPE	lpPrivate = ( LPGIFFILEPRIVATETYPE )lpGifFile->lpPrivate;
	DWORD					dwSize ;
	BOOL					bRet ;

    if( !IS_READABLE( lpPrivate ) )
	{
		/* This file was NOT open for reading: */
		g_GifError = D_GIF_ERR_NOT_READABLE;
		return GIF_ERROR;
    }
    /* Put the screen descriptor into the file: */
    if( DGifGetWord( lpPrivate->hFile, &lpGifFile->SWidth ) == GIF_ERROR ||
		DGifGetWord( lpPrivate->hFile, &lpGifFile->SHeight ) == GIF_ERROR )
	return GIF_ERROR;

	bRet = ReadFile( lpPrivate->hFile, Buf, 3, &dwSize, NULL ) ;
    if( ( bRet == FALSE )||( dwSize != 3 ) )
	{
		g_GifError = D_GIF_ERR_READ_FAILED;
		return GIF_ERROR;
    }
    lpGifFile->SColorResolution	= ( ( ( Buf[0] & 0x70 ) + 1 ) >> 4 ) + 1 ;
    lpGifFile->SBitsPerPixel		= ( Buf[0] & 0x07 ) + 1 ;
    lpGifFile->SBackGroundColor	= Buf[1] ;
    if( Buf[0] & 0x80 )
	{		     /* Do we have global color map? */
		int iBytes;
		iSize = ( 1 << lpGifFile->SBitsPerPixel );
		iBytes = sizeof( GIFCOLORTYPE ) * iSize;
		lpGifFile->SColorMap = ( LPGIFCOLORTYPE )malloc( iBytes );
		if( lpGifFile->SColorMap == NULL )
			return GIF_ERROR ;
		bRet = ReadFile( lpPrivate->hFile, lpGifFile->SColorMap, iBytes, &dwSize, NULL ) ;
		if( ( bRet == FALSE )||( dwSize != (DWORD)iBytes ) )
		{
			g_GifError = D_GIF_ERR_READ_FAILED;
			free( lpGifFile->SColorMap );
			return GIF_ERROR;
		}

/*
		for( i = 0; i < iSize ; i ++ )
		{		// Get the global color map: 
			bRet = ReadFile( lpPrivate->hFile, Buf, 3, &dwSize, NULL ) ;
			if( ( bRet == FALSE )||( dwSize != 3 ) )
			{
				g_GifError = D_GIF_ERR_READ_FAILED;
				free( lpGifFile->SColorMap );
				return GIF_ERROR;
			}
		    lpGifFile -> SColorMap[i].Red = Buf[0];
		    lpGifFile -> SColorMap[i].Green = Buf[1];
			lpGifFile -> SColorMap[i].Blue = Buf[2];
		}
*/
    }
    return GIF_OK;
}
/******************************************************************************
   This routine should be called before any attemp to read an image. 
   this routine read gif file and get the record type of next record
******************************************************************************/
int DGifGetRecordType( LPGIFFILETYPE lpGifFile, LPGIFRECORDTYPE Type )
{
    BYTE					Buf;
    LPGIFFILEPRIVATETYPE	lpPrivate = ( LPGIFFILEPRIVATETYPE )lpGifFile->lpPrivate;
	DWORD					dwSize ;
	BOOL					bRet ;

    if( !IS_READABLE( lpPrivate ) )
	{
		/* This file was NOT open for reading: */
		g_GifError = D_GIF_ERR_NOT_READABLE;
		return GIF_ERROR;
    }

	bRet = ReadFile( lpPrivate->hFile, &Buf, 1, &dwSize, NULL );
    if( ( bRet == FALSE )||( dwSize != 1 ) )
	{
		g_GifError = D_GIF_ERR_READ_FAILED;
		return GIF_ERROR;
    }

    switch( Buf )
	{
		case ',':
		    *Type = IMAGE_DESC_RECORD_TYPE;
			break;
		case '!':
		    *Type = EXTENSION_RECORD_TYPE;
			break;
		case ';':
		    *Type = TERMINATE_RECORD_TYPE;
			break;
		default:
		    *Type = UNDEFINED_RECORD_TYPE;
			g_GifError = D_GIF_ERR_WRONG_RECORD;
			return GIF_ERROR;
    }
    return GIF_OK;
}

//数值GIF文件读写位置
int DGifSetImageOffset( LPGIFFILETYPE lpGifFile, LONG lOffset )
{
	LPGIFFILEPRIVATETYPE	lpPrivate = ( LPGIFFILEPRIVATETYPE )lpGifFile->lpPrivate;
    if( !IS_READABLE( lpPrivate ) )
	{
		/* This file was NOT open for reading: */
		g_GifError = D_GIF_ERR_NOT_READABLE;
		return GIF_ERROR;
    }
	return SetFilePointer( lpPrivate->hFile, lOffset, NULL, FILE_BEGIN );
}

DWORD DGifGetImageOffset( LPGIFFILETYPE lpGifFile )
{
	LPGIFFILEPRIVATETYPE	lpPrivate = ( LPGIFFILEPRIVATETYPE )lpGifFile->lpPrivate;
    if( !IS_READABLE( lpPrivate ) )
	{
		/* This file was NOT open for reading: */
		g_GifError = D_GIF_ERR_NOT_READABLE;
		return GIF_ERROR;
    }
	return SetFilePointer( lpPrivate->hFile, 0, NULL, FILE_CURRENT );
}

/******************************************************************************
   This routine should be called before any attemp to read an image.
   Note it is assumed the Image desc. header (',') has been read.	      
******************************************************************************/
int DGifGetImageDesc( LPGIFFILETYPE lpGifFile )
{
    int						iSize;
    BYTE					Buf[3];
    LPGIFFILEPRIVATETYPE	lpPrivate = ( LPGIFFILEPRIVATETYPE )lpGifFile->lpPrivate;
	DWORD					dwSize = 0 ;
	BOOL					bRet ;

    if( !IS_READABLE( lpPrivate ) )
	{
		/* This file was NOT open for reading: */
		g_GifError = D_GIF_ERR_NOT_READABLE;
		return GIF_ERROR;
    }

    if( DGifGetWord( lpPrivate->hFile, &lpGifFile->ILeft) == GIF_ERROR ||
		DGifGetWord( lpPrivate->hFile, &lpGifFile->ITop) == GIF_ERROR ||
		DGifGetWord( lpPrivate->hFile, &lpGifFile->IWidth) == GIF_ERROR ||
		DGifGetWord( lpPrivate->hFile, &lpGifFile->IHeight) == GIF_ERROR )
		return GIF_ERROR;
	bRet = ReadFile( lpPrivate->hFile, Buf, 1, &dwSize, NULL );
    if( ( bRet == FALSE )||( dwSize != 1 ) )
	{
		g_GifError = D_GIF_ERR_READ_FAILED;
		return GIF_ERROR;
    }
    lpGifFile->IBitsPerPixel	= ( Buf[0] & 0x07 ) + 1;
    lpGifFile->IInterlace		= ( Buf[0] & 0x40 );
    if( Buf[0] & 0x80 )
	{	    /* Does this image have local color map? */
		int iBytes;
		iSize = ( 1 << lpGifFile->IBitsPerPixel );
		if( lpGifFile->IColorMap )
			free( ( char * )lpGifFile->IColorMap ) ;
		iBytes = sizeof( GIFCOLORTYPE ) * iSize;
		lpGifFile->IColorMap = ( LPGIFCOLORTYPE )malloc( iBytes );
		if( lpGifFile->IColorMap == NULL )
			return GIF_ERROR ;
		bRet = ReadFile( lpPrivate->hFile, lpGifFile->IColorMap, iBytes, &dwSize, NULL );
		if( ( dwSize != (DWORD)iBytes )||( bRet == FALSE ) )
		{
			g_GifError = D_GIF_ERR_READ_FAILED;
			return GIF_ERROR;
		}

/*
		for( i = 0 ; i < iSize ; i ++ )
		{	   // Get the image local color map: 
			bRet = ReadFile( lpPrivate->hFile, Buf, 3, &dwSize, NULL );
			if( ( dwSize != 3 )||( bRet == FALSE ) )
			{
				g_GifError = D_GIF_ERR_READ_FAILED;
				return GIF_ERROR;
			}
		    lpGifFile->IColorMap[i].Red		= Buf[0];
		    lpGifFile->IColorMap[i].Green	= Buf[1];
			lpGifFile->IColorMap[i].Blue	= Buf[2];
		}
*/
    }

    lpPrivate->PixelCount = ( long )lpGifFile->IWidth * ( long )lpGifFile->IHeight ;

    DGifSetupDecompress( lpGifFile ) ;  /* Reset decompress algorithm parameters. */

    return GIF_OK;
}

/******************************************************************************
*  Get one full scanned line (Line) of length LineLen from GIF file.	      *
******************************************************************************/
int DGifGetLine( LPGIFFILETYPE lpGifFile, LPBYTE lpbLine, int LineLen )
{
    BYTE					*lpDummy;
    LPGIFFILEPRIVATETYPE	lpPrivate = ( LPGIFFILEPRIVATETYPE )lpGifFile->lpPrivate;

    if( !IS_READABLE( lpPrivate ) )
	{
		/* This file was NOT open for reading: */
		g_GifError = D_GIF_ERR_NOT_READABLE;
		return GIF_ERROR;
    }
    if( !LineLen )
		LineLen = lpGifFile->IWidth;

    if( ( lpPrivate->PixelCount -= LineLen ) > 0xffff0000 )
	{
		g_GifError = D_GIF_ERR_DATA_TOO_BIG;
		return GIF_ERROR;
    }

    if( DGifDecompressLine( lpGifFile, lpbLine, LineLen ) == GIF_OK )
	{
		if( lpPrivate->PixelCount == 0 )
		{
			/* We probably would not be called any more, so lets clean 	     */
			/* everything before we return: need to flush out all rest of    */
			/* image until empty block (size 0) detected. We use GetCodeNext.*/
			do{
				if( DGifGetCodeNext( lpGifFile, &lpDummy ) == GIF_ERROR )
					return GIF_ERROR;
			}while( lpDummy != NULL );
		}
		return GIF_OK;
    }else
		return GIF_ERROR;
}

/******************************************************************************
* Put one pixel (Pixel) into GIF file.					      *
******************************************************************************/
int DGifGetPixel( LPGIFFILETYPE lpGifFile, BYTE Pixel )
{
    BYTE					*lpDummy;
    LPGIFFILEPRIVATETYPE	lpPrivate = ( LPGIFFILEPRIVATETYPE )lpGifFile->lpPrivate;

    if( !IS_READABLE( lpPrivate ) )
	{
		/* This file was NOT open for reading: */
		g_GifError = D_GIF_ERR_NOT_READABLE;
		return GIF_ERROR;
    }

    if( --lpPrivate->PixelCount > 0xffff0000 )
    {
		g_GifError = D_GIF_ERR_DATA_TOO_BIG;
		return GIF_ERROR;
    }

    if( DGifDecompressLine( lpGifFile, &Pixel, 1 ) == GIF_OK )
	{
		if( lpPrivate->PixelCount == 0 )
		{
			/* We probably would not be called any more, so lets clean 	     */
			/* everything before we return: need to flush out all rest of    */
			/* image until empty block (size 0) detected. We use GetCodeNext.*/
			do{
				if( DGifGetCodeNext( lpGifFile, &lpDummy ) == GIF_ERROR )
					return GIF_ERROR;
			}while( lpDummy != NULL );
		}
		return GIF_OK;
    }
    else
		return GIF_ERROR;
}

/******************************************************************************
 Get an extension block (see GIF manual) from gif file. This routine only
 returns the first data block, and DGifGetExtensionNext shouldbe called
 after this one until NULL extension is returned.
 The Extension should NOT be freed by the user (not dynamically allocated).
 Note it is assumed the Extension desc. header ('!') has been read.
******************************************************************************/
int DGifGetExtension( LPGIFFILETYPE lpGifFile, int *ExtCode,
						    BYTE **Extension )
{
    BYTE					Buf;
    LPGIFFILEPRIVATETYPE	lpPrivate = ( LPGIFFILEPRIVATETYPE )lpGifFile->lpPrivate;
	DWORD					dwSize ;
	BOOL					bRet ;

    if( !IS_READABLE( lpPrivate ) )
	{
		/* This file was NOT open for reading: */
		g_GifError = D_GIF_ERR_NOT_READABLE;
		return GIF_ERROR;
    }

	bRet = ReadFile( lpPrivate->hFile, &Buf, 1, &dwSize, NULL ) ;
    if( ( bRet == FALSE )||( dwSize != 1 ) )
	{
		g_GifError = D_GIF_ERR_READ_FAILED;
		return GIF_ERROR;
    }
    *ExtCode = Buf;

    return DGifGetExtensionNext( lpGifFile, Extension );
}

/******************************************************************************
Get a following extension block (see GIF manual) from gif file. This
routine sould be called until NULL Extension is returned.
The Extension should NOT be freed by the user (not dynamically allocated).
******************************************************************************/
int DGifGetExtensionNext( LPGIFFILETYPE lpGifFile, BYTE **Extension )
{
    BYTE					Buf;
    LPGIFFILEPRIVATETYPE	lpPrivate = ( LPGIFFILEPRIVATETYPE )lpGifFile->lpPrivate;
	DWORD					dwSize ;
	BOOL					bRet ;

	bRet = ReadFile( lpPrivate->hFile, &Buf, 1, &dwSize, NULL );
    if( ( bRet == FALSE )||( dwSize != 1 ) )
	{
		g_GifError = D_GIF_ERR_READ_FAILED;
		return GIF_ERROR;
    }
    if( Buf > 0 )
	{
		*Extension = lpPrivate -> Buf;           /* Use private unused buffer. */
		(*Extension)[0] = Buf;  /* Pascal strings notation (pos. 0 is len.). */
		bRet = ReadFile( lpPrivate->hFile, &((*Extension)[1]), Buf, &dwSize, NULL );
//		{//for debug by kingkong
//			Buf = (*Extension)[1] ;
//			Buf = (*Extension)[2] ;
//			Buf = (*Extension)[3] ;
//			Buf = (*Extension)[4] ;
//		}
		if( ( bRet == FALSE )||( dwSize != (DWORD)Buf ) )
		{
		    g_GifError = D_GIF_ERR_READ_FAILED;
			return GIF_ERROR;
		}
    }
    else
		*Extension = NULL;

    return GIF_OK;
}

/******************************************************************************
This routine should be called last, to close GIF file.
******************************************************************************/
int DGifCloseFile( LPGIFFILETYPE lpGifFile )
{
    LPGIFFILEPRIVATETYPE	lpPrivate;
	HANDLE					hFile ;

    if( lpGifFile == NULL )
		return GIF_ERROR;

    lpPrivate = ( LPGIFFILEPRIVATETYPE )lpGifFile->lpPrivate;

    if( !IS_READABLE( lpPrivate ) )
	{
		/* This file was NOT open for reading: */
		g_GifError = D_GIF_ERR_NOT_READABLE;
		return GIF_ERROR;
    }

    hFile = lpPrivate->hFile;

    if( lpGifFile->IColorMap )
		free( ( char * )lpGifFile->IColorMap );
    if( lpGifFile->SColorMap )
		free( ( char * )lpGifFile->SColorMap );
    if( lpPrivate )
		free( ( char * )lpPrivate );
    free( lpGifFile );

    if( CloseHandle( hFile ) == FALSE )
	{
		g_GifError = D_GIF_ERR_CLOSE_FAILED;
		return GIF_ERROR;
    }
    return GIF_OK;
}

/******************************************************************************
Get 2 bytes (word) from the given file:	
******************************************************************************/
static int DGifGetWord( HANDLE hFile, int *Word )
{
    unsigned char c[2];
	DWORD			dwSize ;
	BOOL			bRet ;

	bRet = ReadFile( hFile, c, 2, &dwSize, NULL );
    if( ( bRet == FALSE )||( dwSize != 2 ) )
	{
		g_GifError = D_GIF_ERR_READ_FAILED;
		return GIF_ERROR;
    }

    *Word = ( ( ( unsigned int )c[1]) << 8 ) + c[0];
    return GIF_OK;
}

/******************************************************************************
Get the image code in compressed form. his routine can be called if the
information needed to be piped out as is. Obviously this is much faster
than decoding and encoding again. This routine should be followed by calls
to DGifGetCodeNext, until NULL block is returned.
The block should NOT be freed by the user (not dynamically allocated).
******************************************************************************/
int DGifGetCode( LPGIFFILETYPE lpGifFile, int *lpCodeSize, LPBYTE *lppCodeBlock )
{
    LPGIFFILEPRIVATETYPE lpPrivate = ( LPGIFFILEPRIVATETYPE )lpGifFile->lpPrivate;

    if( ! IS_READABLE( lpPrivate ) )
	{
		/* This file was NOT open for reading: */
		g_GifError = D_GIF_ERR_NOT_READABLE;
		return GIF_ERROR;
    }

    *lpCodeSize = lpPrivate -> BitsPerPixel;

    return DGifGetCodeNext( lpGifFile, lppCodeBlock );
}

/******************************************************************************
*   Continue to get the image code in compressed form. This routine should be *
* called until NULL block is returned.					      *
*   The block should NOT be freed by the user (not dynamically allocated).    *
******************************************************************************/
int DGifGetCodeNext( LPGIFFILETYPE lpGifFile, LPBYTE *lppCodeBlock )
{
    BYTE					Buf;
    LPGIFFILEPRIVATETYPE	lpPrivate = ( LPGIFFILEPRIVATETYPE )lpGifFile->lpPrivate;
	DWORD					dwSize ;
	BOOL					bRet ;

	bRet = ReadFile( lpPrivate->hFile, &Buf, 1, &dwSize, NULL );
    if( ( bRet == FALSE )||( dwSize != 1 ) )
	{
		g_GifError = D_GIF_ERR_READ_FAILED;
		return GIF_ERROR;
    }

    if( Buf > 0 )
	{
		*lppCodeBlock = lpPrivate->Buf;	       /* Use private unused buffer. */
		(*lppCodeBlock)[0] = Buf;  /* Pascal strings notation (pos. 0 is len.). */
		bRet = ReadFile( lpPrivate->hFile, &((*lppCodeBlock)[1]), Buf, &dwSize, NULL );
		if( ( bRet == FALSE )||( dwSize != (DWORD)Buf ) )
		{
		    g_GifError = D_GIF_ERR_READ_FAILED;
		    return GIF_ERROR;
		}
    }else
	{
		*lppCodeBlock = NULL;
		lpPrivate -> Buf[0] = 0;		   /* Make sure the buffer is empty! */
		lpPrivate -> PixelCount = 0;   /* And local info. indicate image read. */
    }

    return GIF_OK;
}

/******************************************************************************
*   Setup the LZ decompression for this image:				      *
******************************************************************************/
static int DGifSetupDecompress( LPGIFFILETYPE lpGifFile )
{
    int						i, BitsPerPixel;
    BYTE					CodeSize;
    unsigned int			*Prefix;
    LPGIFFILEPRIVATETYPE	lpPrivate = ( LPGIFFILEPRIVATETYPE )lpGifFile->lpPrivate;
	DWORD					dwSize ;
	BOOL					bRet ;

	bRet = ReadFile( lpPrivate->hFile, &CodeSize, 1, &dwSize, NULL );
    /* Read Code size from file. */
	if( ( bRet == FALSE )||( dwSize != 1 ) )
	{
	    g_GifError = D_GIF_ERR_READ_FAILED;
		return GIF_ERROR;
	}
    BitsPerPixel = CodeSize;

    lpPrivate->Buf[0]			= 0;			      /* Input Buffer empty. */
    lpPrivate->BitsPerPixel	= BitsPerPixel;
    lpPrivate->ClearCode		= ( 1 << BitsPerPixel );
    lpPrivate->EOFCode		= lpPrivate->ClearCode + 1;
    lpPrivate->RunningCode	= lpPrivate->EOFCode + 1;
    lpPrivate->RunningBits	= BitsPerPixel + 1;	 /* Number of bits per code. */
    lpPrivate->MaxCode1		= 1 << lpPrivate->RunningBits;     /* Max. code + 1. */
    lpPrivate->StackPtr		= 0;		    /* No pixels on the pixel stack. */
    lpPrivate->LastCode		= NO_SUCH_CODE;
    lpPrivate->CrntShiftState = 0;	/* No information in CrntShiftDWord. */
    lpPrivate->CrntShiftDWord = 0;

    Prefix = lpPrivate->Prefix;
    for( i = 0; i <= LZ_MAX_CODE; i ++ )
		Prefix[i] = NO_SUCH_CODE;

    return GIF_OK;
}

/******************************************************************************
*   The LZ decompression routine:					      *
*   This version decompress the given gif file into Line of length LineLen.   *
*   This routine can be called few times (one per scan line, for example), in *
* order the complete the whole image.					      *
******************************************************************************/
static int DGifDecompressLine( LPGIFFILETYPE lpGifFile, LPBYTE lpbLine, int LineLen )
{
    int						i = 0, j ; 
	int						CrntCode, EOFCode, ClearCode, CrntPrefix, LastCode, StackPtr;
    BYTE					*Stack, *Suffix;
    unsigned int			*Prefix;
    LPGIFFILEPRIVATETYPE	lpPrivate = ( LPGIFFILEPRIVATETYPE )lpGifFile->lpPrivate;

    StackPtr		= lpPrivate->StackPtr;
    Prefix			= lpPrivate->Prefix;
    Suffix			= lpPrivate->Suffix;
    Stack			= lpPrivate->Stack;
    EOFCode			= lpPrivate->EOFCode;
    ClearCode		= lpPrivate->ClearCode;
    LastCode		= lpPrivate->LastCode;

    if( StackPtr != 0 )
	{
		/* Let pop the stack off before continueing to read the gif file: */
		while( StackPtr != 0 && i < LineLen )
			lpbLine[i++] = Stack[--StackPtr];
    }

    while( i < LineLen )
	{			    /* Decode LineLen items. */
		if( DGifDecompressInput( lpPrivate, &CrntCode ) == GIF_ERROR )
    	    return GIF_ERROR;

		if( CrntCode == EOFCode )
		{
			/* Note however that usually we will not be here as we will stop */
			/* decoding as soon as we got all the pixel, or EOF code will    */
			/* not be read at all, and DGifGetLine/Pixel clean everything.   */
			if( i != LineLen - 1 || lpPrivate->PixelCount != 0 )
			{
				g_GifError = D_GIF_ERR_EOF_TOO_SOON;
				return GIF_ERROR;
			}
			i++;
		}else if( CrntCode == ClearCode )
		{
			/* We need to start over again: */
			for( j = 0; j <= LZ_MAX_CODE; j ++ )
				Prefix[j] = NO_SUCH_CODE;
			lpPrivate->RunningCode	= lpPrivate->EOFCode + 1;
			lpPrivate->RunningBits	= lpPrivate->BitsPerPixel + 1;
			lpPrivate->MaxCode1		= 1 << lpPrivate->RunningBits;
			LastCode				= lpPrivate->LastCode = NO_SUCH_CODE;
		}else
		{
			/* Its regular code - if in pixel range simply add it to output  */
			/* stream, otherwise trace to codes linked list until the prefix */
			/* is in pixel range:					     */
			if( CrntCode < ClearCode )
			{
				/* This is simple - its pixel scalar, so add it to output:   */
				lpbLine[i++] = CrntCode;
			}else
			{
				/* Its a code to needed to be traced: trace the linked list  */
				/* until the prefix is a pixel, while pushing the suffix     */
				/* pixels on our stack. If we done, pop the stack in reverse */
				/* (thats what stack is good for!) order to output.	     */
				if( Prefix[CrntCode] == NO_SUCH_CODE )
				{
					/* Only allowed if CrntCode is exactly the running code: */
					/* In that case CrntCode = XXXCode, CrntCode or the	     */
				    /* prefix code is last code and the suffix char is	     */
				    /* exactly the prefix of last code!			     */
					if( CrntCode == lpPrivate -> RunningCode - 2 )
					{
						CrntPrefix = LastCode;
						Suffix[lpPrivate -> RunningCode - 2] = Stack[StackPtr++] = 
							DGifGetPrefixChar(Prefix, LastCode, ClearCode);
					}else
					{
						g_GifError = D_GIF_ERR_IMAGE_DEFECT;
						return GIF_ERROR;
					}
				}else
					CrntPrefix = CrntCode;

				/* Now (if image is O.K.) we should not get an NO_SUCH_CODE  */
				/* During the trace. As we might loop forever, in case of    */
				/* defective image, we count the number of loops we trace    */
				/* and stop if we got LZ_MAX_CODE. obviously we can not      */
				/* loop more than that.					     */
				j = 0;
				while( j++ <= LZ_MAX_CODE &&
			       CrntPrefix > ClearCode &&
			       CrntPrefix <= LZ_MAX_CODE )
				{
					Stack[StackPtr++] = Suffix[CrntPrefix];
				    CrntPrefix = Prefix[CrntPrefix];
				}
				if( j >= LZ_MAX_CODE || CrntPrefix > LZ_MAX_CODE )
				{
					g_GifError = D_GIF_ERR_IMAGE_DEFECT;
					return GIF_ERROR;
				}
				/* Push the last character on stack: */
				Stack[StackPtr++] = CrntPrefix;
	
				/* Now lets pop all the stack into output: */
				while( StackPtr != 0 && i < LineLen )
				    lpbLine[i++] = Stack[--StackPtr];
		    }
			if( LastCode != NO_SUCH_CODE )
			{
				Prefix[lpPrivate->RunningCode - 2] = LastCode;

				if( CrntCode == lpPrivate->RunningCode - 2 )
				{
					/* Only allowed if CrntCode is exactly the running code: */
				    /* In that case CrntCode = XXXCode, CrntCode or the	     */
					/* prefix code is last code and the suffix char is	     */
				    /* exactly the prefix of last code!			     */
				    Suffix[lpPrivate -> RunningCode - 2] =
					DGifGetPrefixChar( Prefix, LastCode, ClearCode );
				}else
				{
					Suffix[lpPrivate -> RunningCode - 2] =
					DGifGetPrefixChar( Prefix, CrntCode, ClearCode );
				}
			}
			LastCode = CrntCode;
		}
    }

    lpPrivate -> LastCode = LastCode;
    lpPrivate -> StackPtr = StackPtr;

    return GIF_OK;
}

/******************************************************************************
* Routine to trace the Prefixes linked list until we get a prefix which is    *
* not code, but a pixel value (less than ClearCode). Returns that pixel value.*
* If image is defective, we might loop here forever, so we limit the loops to *
* the maximum possible if image O.k. - LZ_MAX_CODE times.		      *
******************************************************************************/
static int DGifGetPrefixChar( unsigned int *Prefix, int Code, int ClearCode )
{
    int i = 0;

    while( Code > ClearCode && i++ <= LZ_MAX_CODE )
		Code = Prefix[Code];
    return Code;
}

/******************************************************************************
*   Interface for accessing the LZ codes directly. Set Code to the real code  *
* (12bits), or to -1 if EOF code is returned.				      *
******************************************************************************/
int DGifGetLZCodes( LPGIFFILETYPE lpGifFile, int *Code )
{
    BYTE					*CodeBlock;
    LPGIFFILEPRIVATETYPE	lpPrivate = ( LPGIFFILEPRIVATETYPE )lpGifFile->lpPrivate;

    if( !IS_READABLE( lpPrivate ) )
	{
		/* This file was NOT open for reading: */
		g_GifError = D_GIF_ERR_NOT_READABLE;
		return GIF_ERROR;
    }

    if( DGifDecompressInput( lpPrivate, Code ) == GIF_ERROR )
		return GIF_ERROR;

    if( *Code == lpPrivate->EOFCode )
	{
		/* Skip rest of codes (hopefully only NULL terminating block): */
		do{
			if( DGifGetCodeNext( lpGifFile, &CodeBlock ) == GIF_ERROR )
    			return GIF_ERROR;
		}while( CodeBlock != NULL );

		*Code = -1;
    }else if( *Code == lpPrivate->ClearCode )
	{
		/* We need to start over again: */
		lpPrivate->RunningCode	= lpPrivate->EOFCode + 1;
		lpPrivate->RunningBits	= lpPrivate->BitsPerPixel + 1;
		lpPrivate->MaxCode1		= 1 << lpPrivate->RunningBits;
    }

    return GIF_OK;
}

/******************************************************************************
*   The LZ decompression input routine:					      *
*   This routine is responsable for the decompression of the bit stream from  *
* 8 bits (bytes) packets, into the real codes.				      *
*   Returns GIF_OK if read succesfully.					      *
******************************************************************************/
static int DGifDecompressInput( LPGIFFILEPRIVATETYPE lpPrivate, int *Code )
{
    BYTE NextByte;
    static unsigned int CodeMasks[] = 
	{
		0x0000, 0x0001, 0x0003, 0x0007,
		0x000f, 0x001f, 0x003f, 0x007f,
		0x00ff, 0x01ff, 0x03ff, 0x07ff,
		0x0fff
    };

    while( lpPrivate->CrntShiftState < lpPrivate->RunningBits )
	{
		/* Needs to get more bytes from input stream for next code: */
		if( DGifBufferedInput( lpPrivate->hFile, lpPrivate->Buf, &NextByte )
			== GIF_ERROR )
		{
			return GIF_ERROR;
		}
		lpPrivate->CrntShiftDWord |= ( ( unsigned long )NextByte)<<lpPrivate->CrntShiftState;
		lpPrivate->CrntShiftState += 8;
    }
    *Code = lpPrivate->CrntShiftDWord&CodeMasks[lpPrivate->RunningBits];

    lpPrivate->CrntShiftDWord >>= lpPrivate->RunningBits;
    lpPrivate->CrntShiftState -= lpPrivate->RunningBits;

    /* If code cannt fit into RunningBits bits, must raise its size. Note */
    /* however that codes above 4095 are used for special signaling.      */
    if( ++lpPrivate->RunningCode > lpPrivate->MaxCode1&&
			lpPrivate -> RunningBits < LZ_BITS)
	{
		lpPrivate -> MaxCode1 <<= 1;
		lpPrivate -> RunningBits++;
    }
    return GIF_OK;
}

/******************************************************************************
*   This routines read one gif data block at a time and buffers it internally *
* so that the decompression routine could access it.			      *
*   The routine returns the next byte from its internal buffer (or read next  *
* block in if buffer empty) and returns GIF_OK if succesful.		      *
******************************************************************************/
static int DGifBufferedInput(HANDLE hFile, BYTE *Buf,
						      BYTE *NextByte)
{
	DWORD			dwSize ;
	BOOL			bRet ;
    if( Buf[0] == 0 )
	{
		/* Needs to read the next buffer - this one is empty: */
		bRet = ReadFile( hFile, Buf, 1, &dwSize, NULL );
		if( ( bRet == FALSE )||( dwSize != 1 ) )
		{
		    g_GifError = D_GIF_ERR_READ_FAILED;
			return GIF_ERROR;
		}
		bRet = ReadFile( hFile, &Buf[1], Buf[0], &dwSize, NULL );
		if( ( bRet == FALSE )||( dwSize != (DWORD)Buf[0] ) )
		{
		    g_GifError = D_GIF_ERR_READ_FAILED;
			return GIF_ERROR;
		}
		*NextByte = Buf[1];
		Buf[1] = 2;	   /* We use now the second place as last char read! */
		Buf[0]--;
    }else
	{
		*NextByte = Buf[Buf[1]++];
		Buf[0]--;
    }
    return GIF_OK;
}

