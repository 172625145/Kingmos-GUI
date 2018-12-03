/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/


/*****************************************************
文件说明：将gif 格式转成 bmp格式
版本号：1.0.0
开发时期：2003-03-06
作者：孔
修改记录：
******************************************************/

#include <ewindows.h>
#include <gif2bmp.h>
#include "gif_lib.h"
#include "gifbmp.h"

static const int	InterlacedOffset[] = { 0, 4, 2, 1 }, /* The way Interlaced image should. */
			InterlacedJumps[] = { 8, 8, 4, 2 };    /* be read - offsets and jumps... */
static BOOL DoDrawIndexEx(
	HGIF hGif,  // handle to source device context
    UINT  nIndex,  // bitmap index in gif
	
	HDC hdc,     // handle to destination device context
	int nXDest,  // x-coordinate of destination rectangle's upper-left 
               // corner
	int nYDest,  // y-coordinate of destination rectangle's upper-left 
               // corner
	int nWidth,  // width of destination rectangle
	int nHeight, // height of destination rectangle
  
	int nXSrc,   // x-coordinate of source rectangle's upper-left 
               // corner
	int nYSrc,   // y-coordinate of source rectangle's upper-left 
               // corner
	DWORD dwRop,  // raster operation code  
	UINT flag      //
);

//检查合法性
static LPGIF_OBJ GetGIFOBJPtr( HGIF hGif )
{
	LPGIF_OBJ lpgifObj = (LPGIF_OBJ)hGif;

	if( lpgifObj && lpgifObj->hThis == (HANDLE)lpgifObj )
	{
		return lpgifObj;
	}
	return NULL;
}

//
//
BOOL WINAPI Gif_Destroy( HGIF hGif )
{
	LPGIF_OBJ lpgifObj = GetGIFOBJPtr( hGif );
	LPPTRLIST		lpGifList ;
	LPGBITMAP		lpgb ;
	DWORD			*lpdwType ;
	int				i ;

	//ASSERT( hGif != NULL ) ;
	if( lpgifObj )
	{
		lpGifList = lpgifObj->lpGifList;//( LPPTRLIST )hGif ;
		for( i = 0 ; i < lpGifList->count ; i ++ )
		{
			lpdwType = ( DWORD* )PtrListAt( lpGifList, i ) ;
			ASSERT( lpdwType != NULL ) ;
			if( *lpdwType == TYPE_BITMAP )
			{
				lpgb = ( LPGBITMAP )PtrListAt( lpGifList, i ) ;
				ASSERT( lpgb != NULL ) ;
				DeleteObject( lpgb->hBitmap ) ;
			}
		}
		PtrListFreeAll( lpGifList ) ;
		PtrListDestroy( lpGifList ) ;
		free(  lpGifList ) ;
		lpgifObj->hThis = NULL;
		//2005-02-18, add by lilin
		if( lpgifObj->hbmpCache )
		{
			DeleteDC(lpgifObj->hMemDC );
			DeleteObject(lpgifObj->hbmpCache );
		}
		//

		free( lpgifObj );
		
		return TRUE ;
	}
	return FALSE;
}

static void Rgb2Rgbquad( LPRGBQUAD lpQuad, LPGIFCOLORTYPE	lpColorMap, int num )
{
	while( num )
	{
		lpQuad->rgbRed = lpColorMap->Red;
		lpQuad->rgbGreen = lpColorMap->Green;
		lpQuad->rgbBlue = lpColorMap->Blue;
		lpQuad->rgbReserved = 0;
		lpQuad++;
		lpColorMap++;
		num--;
	}
}
/*
static HBITMAP ConvertToBitmap( LPGIFCOLORTYPE	lpColorMap, 
			LPBYTE lpBitmapData, int iWidth, int iHeight )
{
    int					i, j;
    LPBYTE				lpGifRow;
    LPGIFCOLORTYPE		lpgct;
	COLORREF			rgbcl = 0  ;
	HDC					hdc, hMemDC ;
	HBITMAP				hBitmap ;
	int iScanBytes;

	hdc = GetDC( NULL ) ;// get screen dc

	hMemDC = CreateCompatibleDC( hdc ) ;
	
	hBitmap = CreateCompatibleBitmap( hdc, iWidth, iHeight ) ;
	hBitmap = SelectObject( hMemDC, hBitmap ) ;
	iScanBytes = (iWidth + 3) & (~0x3); // align to dword

	for(i = 0; i < iHeight ; i++ )
	{
	    lpGifRow = (LPBYTE)(lpBitmapData+i*iScanBytes) ;
	    for( j = 0 ; j < iWidth; j ++ )
		{
			lpgct	= &lpColorMap[lpGifRow[j]];
			rgbcl = RGB( lpgct->Red, lpgct->Green, lpgct->Blue ) ;
			rgbcl = SetPixel( hMemDC, j, i, rgbcl );
	    }
	}
	hBitmap = SelectObject( hMemDC, hBitmap ) ;
	DeleteDC( hMemDC ) ;
	ReleaseDC( 0, hdc ) ;
	return hBitmap ;
}
*/
/*
				if( nIndex == 0 )
				{	//first image
					if( lpgifObj->uGlobalFlag & 0x1 )
					{	//
						RECT rc = { nXDest, nYDest, nXDest + nWidth, nYDest + nHeight };
						HBRUSH hBrush = CreateSolidBrush(lpgifObj->rgbBackGroundColor);
						FillRect( hdc, &rc, hBrush );
						DeleteObject( hBrush );
					}
				}

*/
static HBITMAP ConvertToBitmap( LPGIFCOLORTYPE	lpColorMap, 
			LPBYTE lpBitmapData, int iWidth, int iHeight, int iBitsPerPixel )
{
	HBITMAP				hBitmap ;
	BITMAPINFO *lpbmi;  

	ASSERT( iBitsPerPixel <= 8 );

	lpbmi = malloc( sizeof( BITMAPINFO ) + ((1 << iBitsPerPixel) - 1) * sizeof(RGBQUAD) );
	if( lpbmi )
	{
		memset( lpbmi, 0, sizeof( BITMAPINFO ) );
		lpbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		lpbmi->bmiHeader.biBitCount = 8;//iBitsPerPixel;
        lpbmi->bmiHeader.biWidth = iWidth;
        lpbmi->bmiHeader.biHeight = -iHeight;
        lpbmi->bmiHeader.biPlanes = 1;
        lpbmi->bmiHeader.biCompression  = BI_RGB; 
		lpbmi->bmiHeader.biClrUsed = 1 << iBitsPerPixel;
		Rgb2Rgbquad( lpbmi->bmiColors, lpColorMap, 1<<iBitsPerPixel );

		hBitmap = CreateCompatibleBitmap( NULL, iWidth, iHeight );
		if( hBitmap )
		    SetDIBits( NULL, hBitmap, 0, iHeight, lpBitmapData, lpbmi, DIB_RGB_COLORS );
		free( lpbmi );		
		return hBitmap ;
	}
	else
		return NULL;
}

static HBITMAP GetDIBSection( LPGIFCOLORTYPE	lpColorMap, 
			                  int iWidth, int iHeight, int iBitsPerPixel, LPVOID * lpBuf )
{
	HBITMAP				hBitmap = NULL;
	BITMAPINFO *lpbmi;  

	ASSERT( iBitsPerPixel <= 8 );

	lpbmi = malloc( sizeof( BITMAPINFO ) + ((1 << iBitsPerPixel) - 1) * sizeof(RGBQUAD) );
	if( lpbmi )
	{
		memset( lpbmi, 0, sizeof( BITMAPINFO ) );
		lpbmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		lpbmi->bmiHeader.biBitCount = 8;//iBitsPerPixel;
        lpbmi->bmiHeader.biWidth = iWidth;
        lpbmi->bmiHeader.biHeight = -iHeight;
        lpbmi->bmiHeader.biPlanes = 1;
        lpbmi->bmiHeader.biCompression  = BI_RGB; 
		lpbmi->bmiHeader.biClrUsed = 1 << iBitsPerPixel;
		Rgb2Rgbquad( lpbmi->bmiColors, lpColorMap, 1<<iBitsPerPixel );

		//hBitmap = CreateCompatibleBitmap( NULL, iWidth, iHeight );
		//if( hBitmap )
		    //SetDIBits( NULL, hBitmap, 0, iHeight, lpBitmapData, lpbmi, DIB_RGB_COLORS );
		hBitmap = CreateDIBSection( NULL, lpbmi, DIB_RGB_COLORS, lpBuf, NULL, 0 );
		free( lpbmi );		
		return hBitmap ;
	}
	else
		return NULL;
}

static HBITMAP GetBitmapFromGif( LPGIFFILETYPE lpGifFile )
{
    int				i, j, Row, Col, Width, Height, Count ;	
	LPBYTE			lpBitmapData ;
	int				iBitmapSize ;
	int             iScanBytes;
	HBITMAP			hBitmap = 0 ;
	LPGIFCOLORTYPE	lpColorMap;

	if( DGifGetImageDesc( lpGifFile ) == GIF_ERROR )
	{
	    PrintGifError();
		goto Error_End ;
	}
	Row		= lpGifFile -> ITop; /* Image Position relative to Screen. */
	Col		= lpGifFile -> ILeft;
	Width	= lpGifFile -> IWidth;
	Height	= lpGifFile -> IHeight;
	if( lpGifFile->ILeft + lpGifFile->IWidth > lpGifFile->SWidth
		||lpGifFile->ITop + lpGifFile->IHeight > lpGifFile->SHeight )
	{
	    EdbgOutputDebugString( "===Image %d is not confined to screen dimension, aborted.\n");
	    goto Error_End ;
	}
	
    //iBitmapSize = Width*Height*sizeof(BYTE);/* Size in bytes one row.*/
	iScanBytes = (Width + 3) & (~0x3); // align to dword
	iBitmapSize = iScanBytes*Height;/* Size in bytes one row.*/
	
	lpColorMap = ( lpGifFile->IColorMap ?
		lpGifFile->IColorMap:lpGifFile->SColorMap );
	//不能创建一个设备相关的位图（会失去Transparent颜色，
	//例如当转到16bits时，（0x255,0,0） 和 Transparent（0x255,1,2）是相同的值）
	lpBitmapData = NULL;
	hBitmap = GetDIBSection( lpColorMap, Width,
		Height, lpGifFile->IColorMap ? lpGifFile->IBitsPerPixel : lpGifFile->SBitsPerPixel, &lpBitmapData );

	//if( ( lpBitmapData = ( LPBYTE )malloc( iBitmapSize ) ) == NULL )
	if( hBitmap == NULL )
	{
		EdbgOutputDebugString("===Failed to allocate memory required, aborted.");
		goto Error_End ;
	}

	if( lpGifFile->IInterlace )
	{	/* Need to perform 4 passes on the images: */
		for( Count = i = 0; i < 4; i ++ )
			for( j = Row + InterlacedOffset[i]; j < Row + Height; j += InterlacedJumps[i] )
			{
			//	if( DGifGetLine( lpGifFile, (LPBYTE)(lpBitmapData+Width*j+Col), Width ) == GIF_ERROR )
         	    if( DGifGetLine( lpGifFile, (LPBYTE)(lpBitmapData+iScanBytes*j+Col), Width ) == GIF_ERROR )
				{
					PrintGifError();
					goto Error_End ;
				}
			}
	}else
	{
		for( i = 0; i < Height; i++ )
		{
			//if( DGifGetLine( lpGifFile, (LPBYTE)(lpBitmapData+(Width*i)+0), Width ) == GIF_ERROR )
			if( DGifGetLine( lpGifFile, (LPBYTE)(lpBitmapData+(iScanBytes*i)+0), Width ) == GIF_ERROR )
			{
				PrintGifError();
				goto Error_End ;
			}
		}
	}
	return hBitmap;
    
	//lpColorMap = ( lpGifFile->IColorMap ?
		//lpGifFile->IColorMap:lpGifFile->SColorMap );
	//hBitmap = ConvertToBitmap( lpColorMap, lpBitmapData, Width,
		//Height, lpGifFile->IColorMap ? lpGifFile->IBitsPerPixel : lpGifFile->SBitsPerPixel ) ;

Error_End:
	//if( lpBitmapData != NULL )
		//free( lpBitmapData ) ;
	if( hBitmap )
	{
		DeleteObject( hBitmap );
		hBitmap = NULL;
	}
	return hBitmap ;
}

//
//
HGIF WINAPI Gif_LoadFromFile( const TCHAR *lpszGifFile )
{
    int				ExtCode;
    GIFRECORDTYPE	RecordType;
    LPBYTE			Extension;
    LPGIFFILETYPE	lpGifFile;
	HBITMAP			hBitmap ;
	LPPTRLIST		lpGifList ;
	LPGBITMAP		lpgb ;
	//BYTE            cPrevPackedField = 0;
	//LPGCONTROL		lpgc ;
	//UINT indexImage = 0;
	UINT indexRecord = 0;
    LPGIF_OBJ lpgifObj = calloc( 1, sizeof( GIF_OBJ ) );
	if( lpgifObj == NULL )
		return NULL;

	if( ( lpGifFile = DGifOpenFileName( lpszGifFile ) ) == NULL )
	{
	    PrintGifError();
		free( lpgifObj );
		return 0 ;
	}

	lpGifList = ( LPPTRLIST )malloc( sizeof( PTRLIST ) ) ;
	if( lpGifList == NULL )
	{
		DGifCloseFile( lpGifFile ) ;
		free( lpgifObj );
		return 0;
	}
	PtrListCreate( lpGifList, 2, 1 ) ;
	lpgb = NULL;


    do{
		if( DGifGetRecordType( lpGifFile, &RecordType ) == GIF_ERROR )
		{
			PrintGifError();
			goto Error_End ;
		}
		switch( RecordType )
		{
		case IMAGE_DESC_RECORD_TYPE:
			//lpgb->dwOffsetInFile = DGifGetImageOffset( lpGifFile );
		    hBitmap = GetBitmapFromGif( lpGifFile ) ;

			if( hBitmap != NULL )
			{	//在 控制部分的代码（下面的case ） 是否已经分配 ？
				if( lpgb == NULL )
				{	//没有
					lpgb = ( LPGBITMAP )calloc( 1, sizeof( GBITMAP ) ) ;
					if( lpgb == NULL )
						goto Error_End ;
					lpgb->ctrl.uDelayTime = -1; // 没有控制段，
				}
				lpgb->dwType	= TYPE_BITMAP ;
				lpgb->left		= lpGifFile->ILeft ;
				lpgb->top		= lpGifFile->ITop ;
				lpgb->width = lpGifFile->IWidth;
				lpgb->height = lpGifFile->IHeight;

				lpgb->hBitmap	= hBitmap ;
				lpgb->uIndex = indexRecord++;//indexImage++;

				
				if( lpgb->ctrl.bValid )
				{
					//if( lpgb->ctrl.cPackedField & 0x1 )
					//{	// transpant color valid
						//BYTE r = lpGifFile->SColorMap[lpgb->ctrl.clrTransColor].Red;
						//BYTE g = lpGifFile->SColorMap[lpgb->ctrl.clrTransColor].Green;
						//BYTE b = lpGifFile->SColorMap[lpgb->ctrl.clrTransColor].Blue;
						//lpgb->ctrl.clrTransColor = RGB( r, g, b );
					//}
				}
				
				PtrListInsert( lpGifList, lpgb ) ;
				lpgb = NULL; //阻止在用
			}else
				goto Error_End ;

			break;
		case EXTENSION_RECORD_TYPE:
			if( DGifGetExtension( lpGifFile, &ExtCode, &Extension ) == GIF_ERROR )
			{
				PrintGifError(); 
				goto Error_End ;
			}
			if( ExtCode == IMAGE_CONTROL_BLOCK )
			{	//之前是否已经分配一个 ？
				// size == blocsize ?
				if( Extension[0] == 4 )
				{	//valid
					if( lpgb == NULL )
					{	//没有
						lpgb = ( LPGBITMAP )calloc( 1, sizeof( GBITMAP ) ) ;
						if( lpgb == NULL )
							goto Error_End ;
					}
					else
					{	//已经分配，因为一个控制块对应一个位图，如果没有则之前的是无效的
						//清除
						memset( lpgb, 0, sizeof(GBITMAP) );
					}
					//lpgb->ctrl.dwType = TYPE_CONTROL;
					
					lpgb->ctrl.cPackedField = (unsigned char)Extension[1];
					lpgb->ctrl.uDelayTime = (unsigned int)Extension[2] ;
					lpgb->ctrl.clrTransColor = (unsigned char)Extension[4] ;
					lpgb->ctrl.bValid = 1;
					//lpgb->ctrl.cPrevPackedField = cPrevPackedField;
					//cPrevPackedField = lpgb->ctrl.cPackedField;
				}
				//lpgb->uIndex = indexRecord;
				//lpgc->wIndex = (WORD)indexControl++;

				/*
				lpgc = ( LPGCONTROL )malloc( sizeof( GCONTROL ) ) ;
				if( lpgc == NULL )
					goto Error_End ;
				lpgc->dwType			= TYPE_CONTROL ;
				lpgc->cPackedField		= (unsigned char)Extension[1] ;
				lpgc->uDelayTime		= (unsigned int)Extension[2] ;
				lpgc->cTransColorIndex	= (unsigned char)Extension[4] ;
				lpgc->wIndex = (WORD)indexControl++;
				*/

				//PtrListInsert( lpGifList, lpgc ) ;
			}
			while( Extension != NULL )
			{
				if( DGifGetExtensionNext( lpGifFile, &Extension ) == GIF_ERROR )
				{
					PrintGifError();
					goto Error_End ;
				}
			}
			break;
		case TERMINATE_RECORD_TYPE:
			break;
		default:		    /* Should be traps by DGifGetRecordType. */
			break;
		}
    }while( RecordType != TERMINATE_RECORD_TYPE );

	if( lpgb )
	{	//无用的块
		free(lpgb);
	}

	lpgifObj->gifWidth = lpGifFile->SWidth;
	lpgifObj->gifHeight = lpGifFile->SHeight;
	lpgifObj->lpGifList = lpGifList;
	lpgifObj->nTotalImage = indexRecord;	
	lpgifObj->hThis = lpgifObj;
	lpgifObj->uGlobalFlag = lpGifFile->SColorMap ? 0x1 : 0;
	if( lpgifObj->uGlobalFlag & 0x1 )
	{
		BYTE r = lpGifFile->SColorMap[lpGifFile->SBackGroundColor].Red;
		BYTE g = lpGifFile->SColorMap[lpGifFile->SBackGroundColor].Green;
		BYTE b = lpGifFile->SColorMap[lpGifFile->SBackGroundColor].Blue;
		lpgifObj->rgbBackGroundColor = RGB( r, g, b );
	}
	//2005-02-18, add by lilin
	lpgifObj->hbmpCache = CreateCompatibleBitmap( NULL, lpgifObj->gifWidth, lpgifObj->gifHeight );
	if( lpgifObj->hbmpCache )
	{
		lpgifObj->hMemDC = CreateCompatibleDC( NULL );
		if( lpgifObj->hMemDC == NULL )
		{
			DeleteObject( lpgifObj->hbmpCache );
			lpgifObj->hbmpCache = NULL;
		}
		else
		{
			SelectObject( lpgifObj->hMemDC, lpgifObj->hbmpCache );
		}
	}
	//
	

    DGifCloseFile( lpGifFile ) ;
	return (HGIF)lpgifObj->hThis;//lpGifList ;
Error_End:
    free( lpgifObj );
	DGifCloseFile( lpGifFile ) ;
	Gif_Destroy( (HGIF)lpGifList ) ;
	return 0 ;

}

static LPVOID DoGetRecord( HGIF hGif, UINT nIndex, UINT uType )
{
	LPGIF_OBJ lpgifObj = GetGIFOBJPtr( hGif );
	LPPTRLIST		lpGifList;
//	LPGBITMAP		lpgb;
	int				i;
	DWORD			*lpdwType ;
	
	ASSERT( lpgifObj != NULL ) ;
	if( lpgifObj  )
	{		
		lpGifList = lpgifObj->lpGifList;//( LPPTRLIST )hGif ;
		for( i = 0 ; i < lpGifList->count ; i ++ )
		{
			lpdwType = ( DWORD* )PtrListAt( lpGifList, i ) ;
			ASSERT( lpdwType != NULL ) ;
			if( *lpdwType == uType )
			{
				if( uType == TYPE_BITMAP ) //|| uType == TYPE_CONTROL )
				{
					if ( ( (LPGBITMAP)lpdwType )->uIndex == nIndex )
					{
						return lpdwType;
/*
						if( uType == TYPE_BITMAP )
						{
							LPGIF_IMAGE lpgifImage = (LPGIF_IMAGE)lpRecord;
							//lpgb = ( LPGBITMAP )PtrListAt( lpGifList, i ) ;
							//ASSERT( lpgb != NULL ) ;
							lpgifImage->hBitmap = ( (LPGBITMAP)lpdwType )->hBitmap ;
							lpgifImage->height = ( (LPGBITMAP)lpdwType )->height;
							lpgifImage->width = ( (LPGBITMAP)lpdwType )->width;
							lpgifImage->left = ( (LPGBITMAP)lpdwType )->left;
							lpgifImage->top = ( (LPGBITMAP)lpdwType )->top;
							return TRUE;
						}
						else
						{	//control
							//LPGCONTROL lpctrl = &((LPGBITMAP)lpdwType)->ctrl;
							LPGCONTROL lpctrl = (LPGCONTROL)lpRecord;
							//lpctrl->cPackedField = ( (LPGCONTROL)lpdwType )
							if( ((LPGBITMAP)lpdwType)->ctrl.bValid )
							{
							    *lpctrl = *( &((LPGBITMAP)lpdwType)->ctrl );
								return TRUE;							
							}
							else
								return FALSE;
						}
*/
					}
				}
/*
				else if( uType == TYPE_CONTROL )
				{
					if ( ( (LPGCONTROL)lpdwType )->wIndex == nIndex )
					{
						LPGCONTROL lpctrl = (LPGCONTROL)lpRecord;
						//lpctrl->cPackedField = ( (LPGCONTROL)lpdwType )
						*lpctrl = *( (LPGCONTROL)lpdwType );
						return TRUE;
					}
				}
*/
			}
		}
	}
	return FALSE;
}

//根据索引得到gif的位图和该位图的信息
BOOL WINAPI Gif_GetImage( HGIF hGif, UINT nIndex, LPGIF_IMAGE lpgifImage )
{
	if( lpgifImage && lpgifImage->dwSize == sizeof( GIF_IMAGE ) )
	{
		LPGBITMAP lpInfo = (LPGBITMAP)DoGetRecord( hGif, nIndex, TYPE_BITMAP );
		if( lpInfo )
		{
			lpgifImage->hBitmap = lpInfo->hBitmap ;
			lpgifImage->height = lpInfo->height;
			lpgifImage->width = lpInfo->width;
			lpgifImage->left = lpInfo->left;
			lpgifImage->top = lpInfo->top;
			return TRUE;
		}
	}
	return FALSE;
}

//根据索引得到gif的位图
HBITMAP	WINAPI Gif_GetBitmapHandle( HGIF hGif, UINT nIndex )
{
//	GIF_IMAGE gifImage;
	LPGBITMAP lpInfo = (LPGBITMAP)DoGetRecord( hGif, nIndex, TYPE_BITMAP );
	if( lpInfo )
		return lpInfo->hBitmap;
	return NULL;
}

//
//
BOOL WINAPI Gif_Draw(
  HDC hdc, // handle to destination device context
  int nXDest,  // x-coordinate of destination rectangle's upper-left 
               // corner
  int nYDest,  // y-coordinate of destination rectangle's upper-left 
               // corner
  int nWidth,  // width of destination rectangle
  int nHeight, // height of destination rectangle
  HGIF hGif,  // handle to source device context
  int nXSrc,   // x-coordinate of source rectangle's upper-left 
               // corner
  int nYSrc,   // y-coordinate of source rectangle's upper-left 
               // corner
  DWORD dwRop  // raster operation code
)
{
	return DoDrawIndexEx( hGif, 0, hdc, nXDest, nYDest, nWidth, nHeight, nXSrc, nYSrc, dwRop, 1 );
}

//
//

BOOL WINAPI Gif_GetRect( HGIF hGif, LPRECT lpRect )
{
	LPGIF_OBJ lpgifObj = GetGIFOBJPtr( hGif );
	
	if( lpgifObj && lpRect )
	{
		lpRect->left=0;
		lpRect->right=lpgifObj->gifWidth;
		lpRect->top=0;
		lpRect->bottom=lpgifObj->gifHeight;
		return TRUE;
	}
	return FALSE;

/*
	HBITMAP hBitmap;
	BITMAP bitmap;

	hBitmap = Gif_GetBitmapHandle( hGif, 0 ) ;		
	if( hBitmap == 0)
		return FALSE;
	GetObject(hBitmap,sizeof(BITMAP),&bitmap);
	lpRect->left=0;
	lpRect->right=bitmap.bmWidth;
	lpRect->top=0;
	lpRect->bottom=bitmap.bmHeight;
	return TRUE;
	*/
}

//得到GIF的信息
BOOL WINAPI Gif_GetInfo( HGIF hGif, LPGIF_INFO lpgifInfo )
{
	LPGIF_OBJ lpgifObj = GetGIFOBJPtr( hGif );
	
	if( lpgifObj && lpgifInfo && lpgifInfo->dwSize == sizeof(GIF_INFO) )
	{
		lpgifInfo->nWidth = lpgifObj->gifWidth;
		lpgifInfo->nHeight = lpgifObj->gifHeight;
		lpgifInfo->nIndexNum = lpgifObj->nTotalImage;
		return TRUE;
	}
	return FALSE;
}


static BOOL DoDrawIndexEx(
	HGIF hGif,  // handle to source device context
    UINT  nIndex,  // bitmap index in gif
	
	HDC hdcDest,     // handle to destination device context
	int nXDest,  // x-coordinate of destination rectangle's upper-left 
                 // corner
	int nYDest,  // y-coordinate of destination rectangle's upper-left 
                 // corner
	int nWidth,  // width of destination rectangle
	int nHeight, // height of destination rectangle
  
	int nXSrc,   // x-coordinate of source rectangle's upper-left 
               // corner
	int nYSrc,   // y-coordinate of source rectangle's upper-left 
               // corner
	DWORD dwRop,  // raster operation code  
	UINT flag	  // nXDest和nYDest是 0: 相对位置（nXDest+xOffset）,nYDest+yOffset
	              //                  1 绝对位置
)
{
	BOOL bResult =FALSE;
	LPGIF_OBJ lpgifObj = GetGIFOBJPtr( hGif );
	
	if( lpgifObj && nIndex < lpgifObj->nTotalImage )
	{
		HDC hMemoryDC;
		HBITMAP hBitmap;
		//GIF_IMAGE gifImage;
		LPGBITMAP lpInfo = DoGetRecord( hGif, nIndex, TYPE_BITMAP );
		LPGBITMAP lpPreInfo = nIndex ? DoGetRecord( hGif, nIndex-1, TYPE_BITMAP ) : NULL;
		BOOL bEraseBk = FALSE;
		RECT rcEraseBk;

		int ngifXDest;
		int ngifYDest;
		int ngifWidth;
		int ngifHeight;			
		int ngifXSrc;
		int ngifYSrc;
		HDC hdc;


		if( lpInfo )
		{
			//HBITMAP hSaveBitmap = NULL;
			//hBitmap = Gif_GetBitmapHandle( hGif, nIndex );
			//ASSERT( hBitmap );
			//if( hBitmap )
			//2005-02-18, add by lilin
			if( lpgifObj->hbmpCache )
			{
				//hSaveBitmap = SelectObject( hdc, lpgifObj->hbmpCache );
				ngifXDest = ngifYDest = 0;
				ngifWidth = lpgifObj->gifWidth;
				ngifHeight = lpgifObj->gifHeight;
				ngifXSrc = ngifYSrc = 0;
				hdc = lpgifObj->hMemDC;				
				//SelectObject( hdc, hSaveBitmap );
			}
			else
			{
				ngifXDest = nXDest;
				ngifYDest = nYDest;
				ngifWidth = nWidth;//lpgifObj->gifWidth;
				ngifHeight = nHeight;//lpgifObj->gifHeight;
				ngifXSrc = nXSrc;
				ngifYSrc = nYSrc;
				hdc = hdcDest;
			}
			//

			if( ngifWidth == 0 )
			{
				ngifWidth = lpgifObj->gifWidth;
			}
			if( ngifHeight == 0 )
			{
				ngifHeight = lpgifObj->gifHeight;
			}
			if( nIndex == 0 )
			{
				if( flag )
				{
					//绝对位置
					//RECT rect = { nXDest, nYDest, nXDest + nWidth, nYDest + nHeight };
					rcEraseBk.left = ngifXDest;
					rcEraseBk.top = ngifYDest;
					rcEraseBk.right = ngifXDest + ngifWidth;
					rcEraseBk.bottom = ngifYDest + ngifHeight;
				}
				else
				{
					rcEraseBk.left = ngifXDest+lpInfo->left;
					rcEraseBk.top = ngifYDest+lpInfo->top;
					rcEraseBk.right = rcEraseBk.left + lpInfo->width;//nXDest + nWidth;
					rcEraseBk.bottom = rcEraseBk.top + lpInfo->height;
					//RECT rect = { nXDest+lpInfo->left, nYDest+lpInfo->top, nXDest+lpInfo->left + lpInfo->width, nYDest+lpInfo->top + lpInfo->height };
				}
				bEraseBk = TRUE;
			}
			else if( (lpPreInfo->ctrl.cPackedField & 0x1c) == 0x08 )
			{	//动画显示，前一个图画需要有清除工作
				if( flag )
				{
					//绝对位置
					//RECT rect = { nXDest, nYDest, nXDest + nWidth, nYDest + nHeight };
					rcEraseBk.left = ngifXDest;
					rcEraseBk.top = ngifYDest;
					rcEraseBk.right = ngifXDest + ngifWidth;
					rcEraseBk.bottom = ngifYDest + ngifHeight;
				}
				else
				{
					rcEraseBk.left = ngifXDest+lpPreInfo->left;
					rcEraseBk.top = ngifYDest+lpPreInfo->top;
					rcEraseBk.right = rcEraseBk.left + lpPreInfo->width;//nXDest + nWidth;
					rcEraseBk.bottom = rcEraseBk.top + lpPreInfo->height;
					//RECT rect = { nXDest+lpInfo->left, nYDest+lpInfo->top, nXDest+lpInfo->left + lpInfo->width, nYDest+lpInfo->top + lpInfo->height };
				}
				bEraseBk = TRUE;
			}
			/*
			if( lpInfo->left == 0 &&
				lpInfo->top == 0 &&
				lpInfo->height == lpgifObj->gifHeight &&
				lpInfo->width == lpgifObj->gifWidth )
			{
				bEraseBk = TRUE;
			}
			*/

			
			{
				hMemoryDC = CreateCompatibleDC(hdc);
				hBitmap = SelectObject(hMemoryDC,lpInfo->hBitmap);
				if( flag )
				{	//绝对位置
					/*
					if( lpInfo->ctrl.bValid && 
						( ( lpInfo->ctrl.cPackedField & 0x1c ) >> 2 ) == 2 )
						*/
					if( bEraseBk )
					{	//erase background first
						//RECT rect = { nXDest, nYDest, nXDest + nWidth, nYDest + nHeight };
						//HBRUSH hBrush = CreateSolidBrush( RGB( 0xff, 0xff, 0xff ) );//lpgifObj->rgbBackGroundColor );
						FillRect( hdc, &rcEraseBk, GetStockObject( WHITE_BRUSH ) );
						//DeleteObject( hBrush );
					}

					//if( bEraseBk )
					//{
					//	bResult=BitBlt( hdc, nXDest,nYDest,nWidth,nHeight,
					//					hMemoryDC,nXSrc,nYSrc,dwRop);
					//}
					//else
					{						
						if( lpInfo->ctrl.bValid && 
							( lpInfo->ctrl.cPackedField & 0x1 ) )
						{
							bResult=TransparentBlt( hdc, ngifXDest, ngifYDest, ngifWidth, ngifHeight, hMemoryDC, 0, 0, lpInfo->width, lpInfo->height, lpInfo->ctrl.clrTransColor | TB_DEV_COLOR );
						}
						else
						{
							bResult=BitBlt( hdc, ngifXDest,ngifYDest,ngifWidth,ngifHeight,
								hMemoryDC,ngifXSrc,ngifYSrc,dwRop);
						}
					}
				}
				else
				{	//相对位置
					//if( lpInfo->ctrl.bValid && 
						//( ( lpInfo->ctrl.cPackedField & 0x1c ) >> 2 ) == 2 )
					if( bEraseBk ) 
					{	//erase background first
						//RECT rect = { nXDest+lpInfo->left, nYDest+lpInfo->top, nXDest+lpInfo->left + lpInfo->width, nYDest+lpInfo->top + lpInfo->height };
						//HBRUSH hBrush = CreateSolidBrush( RGB( 0xff, 0xff, 0xff ) );//lpgifObj->rgbBackGroundColor );
						FillRect( hdc, &rcEraseBk, GetStockObject( WHITE_BRUSH ) );//hBrush );
						//DeleteObject( hBrush );
					}
					//if( bEraseBk )
					//{
					//	bResult=BitBlt( hdc, nXDest+lpInfo->left,nYDest+lpInfo->top,nWidth,nHeight,
//											hMemoryDC,nXSrc,nYSrc,dwRop);
					//}
					//else
					{						
						if( lpInfo->ctrl.bValid && 
							( lpInfo->ctrl.cPackedField & 0x1 ) )
						{
							bResult=TransparentBlt( hdc, ngifXDest+lpInfo->left, ngifYDest+lpInfo->top, ngifWidth, ngifHeight, hMemoryDC, 0, 0, lpInfo->width, lpInfo->height, lpInfo->ctrl.clrTransColor | TB_DEV_COLOR );
						}
						else
						{
							bResult=BitBlt( hdc, ngifXDest+lpInfo->left,ngifYDest+lpInfo->top,ngifWidth,ngifHeight,
											hMemoryDC,ngifXSrc,ngifYSrc,dwRop);
						}
					}
				}				

				if( lpgifObj->hbmpCache )
				{
					//SelectObject( hdc, hSaveBitmap );
					//SelectObject( hMemoryDC, lpgifObj->hbmpCache );
					if( nWidth == 0 )
					{
						nWidth = lpgifObj->gifWidth;
					}
					if( nHeight == 0 )
					{
						nHeight = lpgifObj->gifHeight;
					}
					bResult=BitBlt( hdcDest, nXDest,nYDest,nWidth,nHeight,
								hdc,nXSrc,nYSrc,dwRop);

/*
					if( flag )
					{
						bResult=BitBlt( hdcDest, nXDest,nYDest,nWidth,nHeight,
								hdc,nXSrc,nYSrc,dwRop);

					}
					else
					{
						bResult=BitBlt( hdcDest, nXDest+lpInfo->left,nYDest+lpInfo->top,nWidth,nHeight,
										hdc,nXSrc+lpInfo->left,nYSrc+lpInfo->top,dwRop);
					}
*/
				}
				hBitmap = SelectObject(hMemoryDC,hBitmap);

				DeleteDC(hMemoryDC);		
			}
		}
	}
	return bResult;
}

BOOL WINAPI Gif_DrawIndex(
  HGIF hGif,    // handle to source device context
  UINT  nIndex,  // bitmap index in gif

  HDC  hdc,      // handle to destination device context
  int  nXDest,   // x-coordinate of destination rectangle's upper-left 
                 // corner
  int  nYDest    // y-coordinate of destination rectangle's upper-left 
                 // corner  
)
{
    return DoDrawIndexEx( hGif, nIndex, hdc, nXDest, nYDest, 0, 0, 0, 0, SRCCOPY, 0 );
}

BOOL WINAPI Gif_DrawIndexEx(
	HGIF hGif,  // handle to source device context
    UINT  nIndex,  // bitmap index in gif
	
	HDC hdc,     // handle to destination device context
	int nXDest,  // x-coordinate of destination rectangle's upper-left 
               // corner
	int nYDest,  // y-coordinate of destination rectangle's upper-left 
               // corner
	int nWidth,  // width of destination rectangle
	int nHeight, // height of destination rectangle
  
	int nXSrc,   // x-coordinate of source rectangle's upper-left 
               // corner
	int nYSrc,   // y-coordinate of source rectangle's upper-left 
               // corner
	DWORD dwRop  // raster operation code  
)
{
	return DoDrawIndexEx( hGif, nIndex, hdc, nXDest, nYDest, nWidth, nHeight, nXSrc, nYSrc, dwRop, 1 );
}

BOOL WINAPI Gif_GetDelay( HGIF hGif, UINT nIndex, LPDWORD lpdwDelay )
{
	LPGIF_OBJ lpgifObj = GetGIFOBJPtr( hGif );
	
	if( lpgifObj && lpdwDelay && nIndex < lpgifObj->nTotalImage )
	{
		LPGBITMAP lpInfo  = (LPGBITMAP)DoGetRecord( hGif, nIndex, TYPE_BITMAP );
		if( lpInfo )
		{
			*lpdwDelay = lpInfo->ctrl.uDelayTime;
			return TRUE;
		}
	}
	return FALSE;
}

