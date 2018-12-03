/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef __JPEGAPI_H
#define __JPEGAPI_H

#ifdef __cplusplus
extern "C" {
#endif      //__cplusplus

//必须首先调用该函数，只调用一次
BOOL WINAPI JPEG_Init( void );
//退出JPEG
BOOL WINAPI JPEG_Deinit( void );


// lpBitmap and lpbmpInfo 可以为NULL
HBITMAP WINAPI JPEG_LoadByHandle( HANDLE hFile, BITMAP * lpBitmap );
// 假如 lpbmpInfo 有效， 如果 biWidth 和 biHeight 和 biBitCount 为0， 则在API返回后设置为jpeg的相应值
HBITMAP WINAPI JPEG_LoadByHandleEx( HANDLE hFile, BITMAP * lpBitmap, LPBITMAPINFO lpbmpInfo );
// lpBitmap and lpbmpInfo 可以为NULL
HBITMAP WINAPI JPEG_LoadByName( LPCTSTR lpszFileName, BITMAP * lpBitmap );
HBITMAP WINAPI JPEG_LoadByNameEx( LPCTSTR lpszFileName, BITMAP * lpBitmap, LPBITMAPINFO lpbmpInfo );

// save flags
#define JPEG_FAST           1
#define JPEG_ACCURATE       2
#define JPEG_QUALITYSUPERB  0x80
#define JPEG_QUALITYGOOD    0x100
#define JPEG_QUALITYNORMAL  0x200
#define JPEG_QUALITYAVERAGE 0x400
#define JPEG_QUALITYBAD     0x800

BOOL WINAPI JPEG_SaveByHandle( HANDLE hFile, LPBITMAPINFO lpbmpInfo, LPVOID lpBits, UINT flags );
BOOL WINAPI JPEG_SaveByName( LPCTSTR lpszFileName, LPBITMAPINFO lpbmpInfo, LPVOID lpBits, UINT flags );

//装入需要大小的缩略图
HBITMAP WINAPI JPEG_LoadThumbNailByName( LPCTSTR lpszFileName, BITMAP * lpBitmap, UINT width, UINT height );


#ifdef __cplusplus
}
#endif  //__cplusplus

#endif //__JPEGAPI_H

