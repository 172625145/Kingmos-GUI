/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
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

//�õ�ָ����gifλͼ
HBITMAP	WINAPI Gif_GetBitmapHandle( HGIF hGif, UINT nIndex );

typedef struct _GIF_IMAGE
{
	DWORD		dwSize;		//  = sizeof( GIF_IMAGE )
	int		    left;	//��λͼ������ͼ���е���ƫ��
	int		    top;	//��λͼ������ͼ���е���ƫ��
    int         width;		//��λͼ�Ŀ��
	int         height;	//��λͼ�ĸ߶�
	HBITMAP		hBitmap;	//��λͼ�ľ��
}GIF_IMAGE, *LPGIF_IMAGE;
//�õ�ָ����gifλͼ���������Ϣ
BOOL WINAPI Gif_GetImage( HGIF hGif, UINT nIndex, LPGIF_IMAGE lpgifImage );

typedef struct _GIF_INFO
{
	DWORD dwSize;  // = sizeof( GIF_INFO )
    int   nWidth;		//λͼ�Ŀ��
	int   nHeight;		//λͼ�ĸ߶�
	int   nIndexNum;    //������λͼ������
}GIF_INFO, *LPGIF_INFO;

//�õ�GIF�������Ϣ
BOOL WINAPI Gif_GetInfo( HGIF hGif, LPGIF_INFO lpgifInfo );
// ��λ��0.01 ��
BOOL WINAPI Gif_GetDelay( HGIF hGif, UINT nIndex, LPDWORD lpdwDelay );

//����һ��gifλͼ
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
//�򵥵Ļ�ָ����gifλͼ
BOOL WINAPI Gif_DrawIndex(
  HGIF hGif,    // handle to source device context
  UINT  nIndex,  // bitmap index in gif

  HDC  hdc,      // handle to destination device context
  int  nXDest,   // x-coordinate of destination rectangle's upper-left 
                 // corner
  int  nYDest    // y-coordinate of destination rectangle's upper-left 
                 // corner  
);

//�����Ļ�ָ����gifλͼ
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

//�õ�gif����ʾ�����С
BOOL WINAPI Gif_GetRect( HGIF hGif, LPRECT lpRect );

#ifdef __cplusplus
}
#endif  //__cplusplus

#endif //__GIF2BMP_H


