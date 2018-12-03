/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

#ifndef __JPEGAPI_H
#define __JPEGAPI_H

#ifdef __cplusplus
extern "C" {
#endif      //__cplusplus

//�������ȵ��øú�����ֻ����һ��
BOOL WINAPI JPEG_Init( void );
//�˳�JPEG
BOOL WINAPI JPEG_Deinit( void );


// lpBitmap and lpbmpInfo ����ΪNULL
HBITMAP WINAPI JPEG_LoadByHandle( HANDLE hFile, BITMAP * lpBitmap );
// ���� lpbmpInfo ��Ч�� ��� biWidth �� biHeight �� biBitCount Ϊ0�� ����API���غ�����Ϊjpeg����Ӧֵ
HBITMAP WINAPI JPEG_LoadByHandleEx( HANDLE hFile, BITMAP * lpBitmap, LPBITMAPINFO lpbmpInfo );
// lpBitmap and lpbmpInfo ����ΪNULL
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

//װ����Ҫ��С������ͼ
HBITMAP WINAPI JPEG_LoadThumbNailByName( LPCTSTR lpszFileName, BITMAP * lpBitmap, UINT width, UINT height );


#ifdef __cplusplus
}
#endif  //__cplusplus

#endif //__JPEGAPI_H

