/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef __GIF2BMP_H
#define __GIF2BMP_H

#ifdef __cplusplus
extern "C" {
#endif      //__cplusplus

//typedef void * HGIF;
DEFINE_HANDLE(HGIF);

BOOL WINAPI Gif_Destroy( HGIF hGif );
HGIF WINAPI Gif_LoadFromFile( const TCHAR *lpszGifFile );

//得到指定的gif位图
HBITMAP	WINAPI Gif_GetBitmapHandle( HGIF hGif, UINT nIndex );

typedef struct _GIF_IMAGE
{
	DWORD		dwSize;		//  = sizeof( GIF_IMAGE )
	int		    left;	//该位图在整个图形中的左偏移
	int		    top;	//该位图在整个图形中的上偏移
    int         width;		//该位图的宽度
	int         height;	//该位图的高度
	HBITMAP		hBitmap;	//该位图的句柄
}GIF_IMAGE, *LPGIF_IMAGE;
//得到指定的gif位图极其相关信息
BOOL WINAPI Gif_GetImage( HGIF hGif, UINT nIndex, LPGIF_IMAGE lpgifImage );

typedef struct _GIF_INFO
{
	DWORD dwSize;  // = sizeof( GIF_INFO )
    int   nWidth;		//位图的宽度
	int   nHeight;		//位图的高度
	int   nIndexNum;    //包含的位图索引数
}GIF_INFO, *LPGIF_INFO;

//得到GIF的相关信息
BOOL WINAPI Gif_GetInfo( HGIF hGif, LPGIF_INFO lpgifInfo );
// 单位：0.01 秒
BOOL WINAPI Gif_GetDelay( HGIF hGif, UINT nIndex, LPDWORD lpdwDelay );

//画第一个gif位图
BOOL WINAPI Gif_Draw(
  HDC hdc,     // handle to destination device context
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
);
//简单的画指定的gif位图
BOOL WINAPI Gif_DrawIndex(
  HGIF hGif,    // handle to source device context
  UINT  nIndex,  // bitmap index in gif

  HDC  hdc,      // handle to destination device context
  int  nXDest,   // x-coordinate of destination rectangle's upper-left 
                 // corner
  int  nYDest    // y-coordinate of destination rectangle's upper-left 
                 // corner  
);

//更灵活的画指定的gif位图
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
);

//得到gif的显示区域大小
BOOL WINAPI Gif_GetRect( HGIF hGif, LPRECT lpRect );

#ifdef __cplusplus
}
#endif  //__cplusplus

#endif //__GIF2BMP_H


