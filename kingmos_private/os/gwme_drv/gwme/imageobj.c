/******************************************************
Copyright(c) ��Ȩ���У�1998-2005΢�߼�����������Ȩ����
******************************************************/


/*****************************************************
�ļ�˵������ͼ�ζ�����й���(brush,bitmap,pen,font,region,palette)
�汾�ţ�3.0.0
����ʱ�ڣ�1999
���ߣ�����
�޸ļ�¼��
	2004-02-21: WinGdi_CreateDIBSection  �� hgwmeProcess ���� GetCurrentProcess
     2003-05-14 : CreateBitmap Ϊ����Heap����Ƭ�� ��bitmap��
	             �������û����С ���� ĳ��Сʱ�� ��VirtualAlloc
				 ��bmType = 1, ��VirtualAlloc��������,malloc
    2003-05-07: LN:��CreateBitmap��Ĭ��ֵ�ɰ׸�Ϊ��
******************************************************/

#include <eframe.h>
#include <eapisrv.h>
#include <bheap.h>

#include <epwin.h>
#include <gdc.h>
#include <winsrv.h>
#include <gdisrv.h>

#define MALLOC_SIZE_LIMIT  (4 * 1024)

static LPOBJLIST _lpGdiObjList = NULL;
static CRITICAL_SECTION csGdiObjList;
static BOOL _DeletePaletteObject(HGDIOBJ hgdi);

BOOL AddToObjMgr( LPOBJLIST lpObj, DWORD objType, ULONG hOwner );
BOOL RemoveFromObjMgr( LPOBJLIST lpObj );

extern HBITMAP WINAPI WinGdi_CreateCompatibleBitmap(HDC hdc, int iWidth, int iHeight);
extern int _WinGdi_ConvertImageColorValue( 
									_LPCDISPLAYDRV lpDrv,
								    _LPBITMAP_DIB lpDestImage,  // dest image data
									LPCRECT lprcDestClip,
									CONST BITMAPINFO * lpbi,// src image info
									LPCRECT lprcSrcClip,
								    DWORD dwStartScanLine,  // src start scan line
                                    DWORD dwScanLineNum, 
                                    DWORD dwScanLineWidth,
                                    LPCBYTE lpbData,// src bitmap bits data
								    int fuColorUse   // use src's RGB or PAL
									);


// **************************************************
// ������BOOL __InitialGdiObjMgr( void )
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	��ʼ�� GDI ͼ�ζ��������
// ����: 
//	
// ************************************************

BOOL _InitialGdiObjMgr( void )
{
	InitializeCriticalSection( &csGdiObjList );
	csGdiObjList.lpcsName = "CS-GOM";
	
	return TRUE;
}

// **************************************************
// ������void __DeInitialGdiObjMgr( void )
// ������
// 	��
// ����ֵ��
//	��
// ����������
//	�� __InitialGdiObjMgr�෴���ͷ�ͼ�ζ��������
// ����: 
//	
// ************************************************

void _DeInitialGdiObjMgr( void )
{
	DeleteCriticalSection( &csGdiObjList );
}

// **************************************************
// ������BOOL AddToObjMgr( LPOBJLIST lpObj, DWORD objType, ULONG hOwner )
// ������
// 	IN lpObj - OBJLIST �ṹָ��
//	IN objType - ��������
//	IN hOwner - ����ӵ���߽���
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	����������������
// ����: 
//	
// ************************************************

BOOL AddToObjMgr( LPOBJLIST lpObj, DWORD objType, ULONG hOwner )
{
	BOOL bRetv;

	EnterCriticalSection( &csGdiObjList );

	bRetv = ObjList_Init( &_lpGdiObjList, lpObj, objType, (ULONG)hOwner );

	LeaveCriticalSection( &csGdiObjList );
	return bRetv;
}

// **************************************************
// ������BOOL RemoveFromObjMgr( LPOBJLIST lpObj )
// ������
// 	IN lpObj - OBJLIST �ṹָ��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�� AddToObjMgr �෴���������Ƴ���������
// ����: 
//	
// ************************************************

BOOL RemoveFromObjMgr( LPOBJLIST lpObj )
{
	EnterCriticalSection( &csGdiObjList );
	ObjList_Remove( &_lpGdiObjList, lpObj );
	LeaveCriticalSection( &csGdiObjList );
	return TRUE;
}


// **************************************************
// ������HBITMAP _GetShareBitmapObj( HINSTANCE hInst, DWORD dwName )
// ������
// 	IN hInst - ʵ�����
//	IN dwName - ������
// ����ֵ��
//	����ɹ�������λͼ�����������򣬷��� NULL
// ����������
//	�õ����������
// ����: 
//	
// ************************************************

HBITMAP _GetShareBitmapObj( HINSTANCE hInst, DWORD dwName )
{
	HBITMAP hRetv = NULL;
	LPOBJLIST lpObjList;

	EnterCriticalSection( &csGdiObjList );
	
	lpObjList = _lpGdiObjList;
	while( lpObjList )
	{
		if( ( (WORD)lpObjList->objType ) == OBJ_BITMAP )
		{
			_LPBITMAP_SHARE lpBitmap = (_LPBITMAP_SHARE)lpObjList;
			if( lpBitmap->bitmap.bmFlags & BF_SHARE )
			{
				if( hInst == lpBitmap->hInst &&
					dwName == lpBitmap->szName )
				{
					lpObjList->iRefCount++;
					hRetv = (HBITMAP)PTR_TO_HANDLE( lpBitmap );
					break;
				}
			}
		}
		lpObjList = lpObjList->lpNext;
	}

	LeaveCriticalSection( &csGdiObjList );
	return hRetv;
}


// **************************************************
// ������HBITMAP _WinGdi_CreateBitmap( 
//								int nWidth, 
//								int nHeight, 
//								UINT cPlanes,
//								UINT cBitsPerPel,
//								int  iDir,
//								const VOID *lpvBits,									
//								_LPBITMAPDATA * lppBitmap,
//								UINT uiObjFlag,
//								CONST BITMAPINFO *lpbmi,
//								BOOL bIsShareResource
//								)
// ������
//	IN nWidth - λͼ�Ŀ��
//	IN nHeight - λͼ�ĸ߶�
//	IN cPlanes - λ����
//	IN cBitsPerPel - ÿ����� bit��
//	IN iDir - λͼ�ķ��򣨴Ӷ���׻��෴��
//	IN lpvBits - λͼ����
//	OUT lppBitmap - ���ڽ�������λͼ��ʽ�Ľṹ��ָ��
//	IN uiObjFlag - ���ӵĶ����־
//	IN lpbmi - ����lpvBitsλͼ��ʽ�Ľṹ��ָ��,����ΪNULL
//  IN bIsShareResource - ˵���¶����Ƿ���Ҫ������������Դ�ļ��� lpbmi����= NULL)
// ����ֵ��
//	����ɹ�������λͼ�����������򣬷���NULL
// ����������
//	��Ҫ�󴴽�λͼ����
// ����: 
//	
// ************************************************

HBITMAP _WinGdi_CreateBitmap( int nWidth, 
							  int nHeight, 
							  UINT cPlanes,
							  UINT cBitsPerPel,
							  int  iDir,
							  const VOID *lpvBits,
							  _LPBITMAPDATA * lppBitmap,
							  UINT uiObjFlag,
							  CONST BITMAPINFO *lpbmi,
							  BOOL bIsShareResource
							 )
{
    _LPBITMAPDATA lpBitmap;
	LPBYTE lpbBuf;
    int scanBytes, size;
	DWORD bmType = 0;

	ASSERT( nWidth >= 0 && nHeight >= 0 );
	//ɨ�����ֽڿ��
	scanBytes = (cBitsPerPel * nWidth+31) / 32 * 4;  // align to dword
	//�ܵ�λͼ��Ҫ���ֽ���
	size = scanBytes * nHeight * cPlanes;
	//
	if( lpbmi == NULL )
	{	//Ĭ�ϵ�λͼ��ʽ������ _BITMAPDATA�ṹ����
		if( bIsShareResource )
		{
			lpBitmap = (_LPBITMAPDATA)BlockHeap_AllocString( hgwmeBlockHeap, 0, sizeof(_BITMAP_SHARE) );
			if( lpBitmap )
				memset( lpBitmap, 0, sizeof(_BITMAP_SHARE) );
			bmType |= BF_SHARE;
		}
		else
		{
			lpBitmap = (_LPBITMAPDATA)BlockHeap_AllocString( hgwmeBlockHeap, 0, sizeof(_BITMAPDATA) );
			if( lpBitmap )
				memset( lpBitmap, 0, sizeof(_BITMAPDATA) );
		}
		if( lpBitmap == NULL )
		{
			ASSERT( 0 );
			return NULL;
		}
	}
	else
	{	//lpbmi �����豸�޹���Ϣ
		DWORD dwAllocSize = 0;
		DWORD dwPalSize = 0;
		UINT  uiPalIndexNum = 0;

		if( lpbmi->bmiHeader.biBitCount <= 8 )
		{	// �����е�ɫ��
			uiPalIndexNum = (1 << lpbmi->bmiHeader.biBitCount);
			if( lpbmi->bmiHeader.biClrUsed )			
			    uiPalIndexNum = MIN( lpbmi->bmiHeader.biClrUsed, uiPalIndexNum );
			dwPalSize =  uiPalIndexNum * sizeof(RGBQUAD);
		}
		else if( ( lpbmi->bmiHeader.biBitCount == 16 ||
			       lpbmi->bmiHeader.biBitCount == 32 )   && 
			     lpbmi->bmiHeader.biCompression == BI_BITFIELDS )
		{	//�����ɫ�壬Ӧ��������DWORD�� RGB ռλ������
			uiPalIndexNum = 3;
			dwPalSize = sizeof( DWORD ) * uiPalIndexNum;
		}
		//���� _BITMAPDATA �ṹ�����С
		dwAllocSize = sizeof( _BITMAP_DIB ) + dwPalSize - sizeof( DWORD );
		//���� _BITMAPDATA�ṹ����
		lpBitmap = (_LPBITMAPDATA)BlockHeap_AllocString( hgwmeBlockHeap, 0, dwAllocSize );
		if( lpBitmap )
		{
			UINT i;
			PALETTEENTRY * lpPal = ((_LPBITMAP_DIB)lpBitmap)->palEntry;
			RGBQUAD * lpQuad = (RGBQUAD *)lpbmi->bmiColors;

			memset( lpBitmap, 0, dwAllocSize );
			//��ʼ����ɫ����Ϣ
			if( lpbmi->bmiHeader.biCompression == BI_BITFIELDS )
			{	//���� DWORD  mask
				for( i = 0; i < uiPalIndexNum; i++ )
				{
					*((LPDWORD)lpPal) = *( (LPDWORD)lpQuad );
					lpPal++;
					lpQuad++;
				}
			}
			else
			{
				for( i = 0; i < uiPalIndexNum; i++ )
				{
					lpPal->peRed = lpQuad->rgbRed;
					lpPal->peGreen = lpQuad->rgbGreen;
					lpPal->peBlue = lpQuad->rgbBlue;
					lpPal->peFlags = 0;
					lpPal++;
					lpQuad++;
				}
			}

			bmType |= BF_DIB;

			((_LPBITMAP_DIB)lpBitmap)->biClrImportant = lpbmi->bmiHeader.biClrImportant;
			((_LPBITMAP_DIB)lpBitmap)->biClrUsed = uiPalIndexNum;
			((_LPBITMAP_DIB)lpBitmap)->biCompression = lpbmi->bmiHeader.biCompression;
			((_LPBITMAP_DIB)lpBitmap)->biDir = lpbmi->bmiHeader.biHeight > 0 ? -1 : 1;
			((_LPBITMAP_DIB)lpBitmap)->biSizeImage = size;
		}
    }
    //����λͼ���ݻ��棬Ϊ��ֹ����Ƭ��
	//��������С������BlockHeap_Alloc���䣬
	//������Page_AllocMem����
	if( bmType & BF_DIB )
	{   //frame buffer �ᱻ user ���ʣ� so PM_SHARE
		lpbBuf = Page_AllocMem( size, NULL, PM_SHARE );
		bmType |= 1;
	}
	else
	{
		if( size <= MALLOC_SIZE_LIMIT )
		{
			lpbBuf  = (LPBYTE)BlockHeap_Alloc( hgwmeBlockHeap, 0, size );
			//lpbBuf  = (LPBYTE)BLK_Alloc( 0, size );
		}
		else
		{
			lpbBuf = Page_AllocMem( size, NULL, 0 );
			bmType |= 1;
		}
	}

	ASSERT( lpBitmap && lpbBuf );

    if( lpBitmap && lpbBuf )
    {	//����ɹ�
		int i;
        if( nWidth <= 0 )
            nWidth = 1;
        if( nHeight <= 0 )
            nHeight = 1;		

        lpBitmap->bmFlags = (WORD)bmType;
		lpBitmap->bmWidth = nWidth;
        lpBitmap->bmHeight = nHeight;
        lpBitmap->bmWidthBytes = scanBytes;
        lpBitmap->bmPlanes = cPlanes;
        lpBitmap->bmBitsPixel = cBitsPerPel;        
		lpBitmap->bmBits = lpbBuf;

		if( lpvBits )
		{	//������û�λͼ���ݣ�����뵽 WORD 
			int iDataByteWidth = (cBitsPerPel * nWidth+15) / 16 * 2;// align to word
			LPBYTE lpBuf = (LPBYTE)lpBitmap->bmBits;
			
			for( i = 0; i < nHeight; i++ )
			{
				memcpy( lpBuf, lpvBits, iDataByteWidth );
				lpBuf += scanBytes;
				(LPBYTE)lpvBits += iDataByteWidth;
			}
		}
		else
		{	//û����Ҫ��ʼ����λͼ���ݣ�Ϊ0
			memset( lpBitmap->bmBits, 0, size );
		}
        //�����������
		AddToObjMgr( &lpBitmap->obj, OBJ_BITMAP | uiObjFlag, (ULONG)GetCallerProcess() );
    }
	else
	{	//�������������
		if( lpBitmap )
		{
			BlockHeap_FreeString( hgwmeBlockHeap, 0, lpBitmap);
			//BLK_FreeString( 0, lpBitmap);
		}

		
		if( lpbBuf )
		{	//�ͷŷ����λͼ����
			if( bmType & 1 )
			{	//��lpbBuf���� Page_AllocMem�����
				Page_FreeMem( lpbBuf, size );
			}
			else{	//��lpbBuf���� BlockHeap_Alloc �����
			    BlockHeap_Free( hgwmeBlockHeap, 0, lpbBuf, size );
				//BLK_Free( 0, lpbBuf, size );
			}
		}
		lpBitmap = NULL;
		lpbBuf = NULL;
	}    
	
	if( lppBitmap )
		*lppBitmap = lpBitmap;
	if( lpBitmap )
		return (HBITMAP)PTR_TO_HANDLE( lpBitmap );	
	ASSERT( 0 );
	return NULL;
}

// **************************************************
// ������HBITMAP WINAPI WinGdi_CreateBitmap( 
//										int nWidth, 
//										int nHeight, 
//										UINT cPlanes, 
//										UINT cBitsPerPel, 
//										const VOID *lpvBits )
// ������
//	IN nWidth-λͼ���
//	IN nHeight-λͼ�߶�
//	IN cPlanes-λͼλ���� (��֧��1λ��)
//	IN cBitsPerPel-ÿ��bit��
//	IN lpvBits-λͼ����
// ����ֵ��
//	����ɹ���������Ч��λͼ��������򣬷��� NULL
// ����������
//	����λͼ����
// ����: 
//	ϵͳAPI
// ************************************************

HBITMAP WINAPI WinGdi_CreateBitmap( int nWidth, int nHeight, UINT cPlanes, UINT cBitsPerPel, const VOID *lpvBits )
{
	return _WinGdi_CreateBitmap( nWidth, nHeight, cPlanes, cBitsPerPel, 0, lpvBits, NULL, 0, NULL, FALSE );
}

// **************************************************
// ������int WINAPI WinGdi_GetDIBits(
//						 HDC hdc,           
//						 HBITMAP hbmp,      
//						 UINT uStartScan,   
//						 UINT cScanLines,   
//						 LPVOID lpvBits,    
//						 LPBITMAPINFO lpbi, 
//						 UINT uiUsage )
// ������
//	IN hdc -Ŀ��DC
//	IN hbmp - λͼ���
//	IN uStartScan - ��λͼ�ĵڼ�ɨ���п�ʼ�õ�����
//	IN cScanLines - ����Ҫ�õ���ɨ������
//	IN lpvBits - ���ڵõ�λͼ���ݵĻ��棬������NULL���ù�����lpbi�ﷵ��λͼ�Ĵ�С�͸�ʽ
//	IN lpbi - BITMAPINFO�ṹָ�룬˵����Ҫ���ص���������
//	IN uiUsage - ˵��BITMAPINFO��ԱbmiColors�ĸ�ʽ������Ϊ��
//			DIB_PAL_COLORS - ��ɫ�����16bits������ֵ���飬����ָ��ǰ�ĵ�ɫ��
//			DIB_RGB_COLORS - ��ɫ������졢�̡���ֵ
// ����ֵ��
//	����ɹ�����lpvBitsΪNULL���ù������lpbi�ṹ������
//	λͼ�ܵ�ɨ�躯��������ɹ�����lpvBits��ΪNULL���ù�
//	�ܷ���ʵ�ʿ�����ɨ��������lpvBitsΪ�õ���λͼ����
// ����������
//	�ù涨�ĸ�ʽ����λͼ������
// ����: 
//	ϵͳAPI
// ************************************************

int WINAPI WinGdi_GetDIBits(
						 HDC hdc,           
						 HBITMAP hbmp,      
						 UINT uStartScan,   
						 UINT cScanLines,   
						 LPVOID lpvBits,    
						 LPBITMAPINFO lpbi, 
						 UINT uiUsage )
{   // device depent format -> device indepent format
	_LPBITMAPDATA lpImage = _GetHBITMAPPtr( hbmp );
	if( lpImage )
	{	//��Ч��λͼ����
		if( lpvBits == NULL )
		{   // ���λͼ��Ϣ fill the lpbi data
			if( lpbi->bmiHeader.biSize == sizeof( BITMAPINFO ) )
			{
				lpbi->bmiHeader.biHeight = lpImage->bmHeight;
				lpbi->bmiHeader.biPlanes = lpImage->bmPlanes;
				lpbi->bmiHeader.biWidth = lpImage->bmWidth;
				if( lpImage->bmBitsPixel == 16 || lpImage->bmBitsPixel == 32 )
					lpbi->bmiHeader.biCompression = BI_BITFIELDS;
				else
					lpbi->bmiHeader.biCompression = BI_RGB;
				lpbi->bmiHeader.biClrImportant = 0;
				lpbi->bmiHeader.biClrUsed = 0;
				lpbi->bmiHeader.biSizeImage = lpImage->bmWidthBytes * lpImage->bmHeight;

				lpbi->bmiHeader.biBitCount = lpImage->bmBitsPixel;				
				return lpbi->bmiHeader.biHeight;
			}
		}
		else
		{	//��Ҫλͼ����
			_BITMAPDATA bmp;
			BITMAPINFO bi;
			RECT rcDestClip;
			_LPCDISPLAYDRV lpDrv;

			int iDir, iMinHeight, iMinWidth;
			//������Ҫ��λͼ��ʽ����
			// define dest bitmap
			bmp.bmBits = lpvBits;
			bmp.bmBitsPixel = (BYTE)lpbi->bmiHeader.biBitCount;
			bmp.bmHeight = cScanLines;
			bmp.bmPlanes = (BYTE)lpbi->bmiHeader.biPlanes;
			bmp.bmWidth = (WORD)lpbi->bmiHeader.biWidth;
			bmp.bmFlags = 0;
			bmp.bmWidthBytes = (lpbi->bmiHeader.biBitCount * lpbi->bmiHeader.biWidth + 31) / 32 * 4;  // align to dword
			lpDrv = GetDisplayDeviceDriver( &bmp );
			if( lpDrv == NULL )
				return 0;	//ϵͳ��֧�ָ�λͼ��ʽ
			//����Դλͼ��ʽ����
			// define the source bitmap
			bi.bmiHeader.biBitCount = lpImage->bmBitsPixel;
			bi.bmiHeader.biClrImportant = 0;
			bi.bmiHeader.biClrUsed = 0;
			bi.bmiHeader.biCompression = BI_RGB;
			if( (lpImage->bmFlags & BF_DIB) &&
				( (_LPBITMAP_DIB)lpImage )->biDir < 0 )
			{
			    bi.bmiHeader.biHeight = lpImage->bmHeight;
			}
			else
				bi.bmiHeader.biHeight = -lpImage->bmHeight;

			bi.bmiHeader.biPlanes = lpImage->bmPlanes;
			bi.bmiHeader.biSize = sizeof( bi.bmiHeader );
			bi.bmiHeader.biSizeImage = 0;
			bi.bmiHeader.biWidth = lpImage->bmWidth;
			bi.bmiHeader.biXPelsPerMeter = 0;
			bi.bmiHeader.biYPelsPerMeter = 0;

			if( bi.bmiHeader.biHeight > 0 )
			{
				iDir = -1;
				iMinHeight = MIN( bmp.bmHeight, bi.bmiHeader.biHeight );
			}
			else
			{
				iDir = 1;
				iMinHeight = MIN( bmp.bmHeight, (-bi.bmiHeader.biHeight) );
			}
			iMinWidth = MIN( bmp.bmWidth, bi.bmiHeader.biWidth );	// LN, 2003-07-14, ADD
			rcDestClip.left = 0;
			rcDestClip.top = 0;
			rcDestClip.right = iMinWidth;
			rcDestClip.bottom = iMinHeight;
			//��Դλͼת����Ŀ���ʽ			
			return _WinGdi_ConvertImageColorValue(
				                                lpDrv,
				                                 (_LPBITMAP_DIB)&bmp,
												 &rcDestClip,
												 &bi,
												 NULL,  
												 uStartScan,
												 cScanLines,
												 lpImage->bmWidthBytes,
												 lpImage->bmBits + lpImage->bmWidthBytes * uStartScan,
												 uiUsage );
		}
	}
	return 0;
}

// **************************************************
// ������int WINAPI WinGdi_SetDIBits(
//						 HDC hdc,                  // handle to device context
//						 HBITMAP hbmp,             // handle to bitmap
//						 UINT uStartScan,          // starting scan line
//						 UINT cScanLines,          // number of scan lines
//						 CONST VOID *lpvBits,      // array of bitmap bits
//						 CONST BITMAPINFO *lpbmi,  // address of structure with bitmap data
//						 UINT fuColorUse           // type of color indexes to use
//						 )
// ������
//	IN hdc - DC���
//	IN hbmp - ��Ҫ�������ݵ�λͼ
//	IN uStartScan - ˵��lpvBits�����ݵĿ�ʼɨ����
//	IN cScanLines -˵��lpvBits�����������е�ɨ��������
//	IN lpvBits - ������λͼ���ݵ�ָ�룬�����ݽ�������hbmp
//	IN lpbmi - BITMAPINFO���ݽṹ����������lpvBitsָ���������Ϣ
//	IN fuColorUse - ˵��BITMAPINFO�ṹ��ԱbmiColors�����ͣ�
//			DIB_PAL_COLORS - bmiColorsΪ16�������飬ÿһ������ֵָ��ǰhdc�ĵ�ɫ��Ķ�Ӧ�� 
//			DIB_RGB_COLORS - bmiColorsΪ�졢�̡�����ɫ����
// ����ֵ��
//	����ɹ������ؿ�����hbmp�����������򣬷���0
// ����������
//	����DIB���ݸ�ʽ����λͼ���ݵ� hbmp
// ����: 
//	ϵͳAPI
// ************************************************

#define ABS( v ) ( (v) > 0 ? (v) : -(v) )
int WINAPI WinGdi_SetDIBits(
						 HDC hdc,                  // handle to device context
						 HBITMAP hbmp,             // handle to bitmap
						 UINT uStartScan,          // starting scan line
						 UINT cScanLines,          // number of scan lines
						 CONST VOID *lpvBits,      // array of bitmap bits
						 CONST BITMAPINFO *lpbmi,  // address of structure with bitmap data
						 UINT fuColorUse           // type of color indexes to use
						 )
{
    _LPBITMAPDATA lpImage = _GetHBITMAPPtr( hbmp );
	int nHeight;
	RECT rcDestClip;
	_LPCDISPLAYDRV lpDrv;
	//�õ���λͼ����������ӿ�
	lpDrv = GetDisplayDeviceDriver( lpImage );
	if( lpDrv == NULL )
		return 0;

	if( lpbmi )
		nHeight = ABS(lpbmi->bmiHeader.biHeight);

	if( lpbmi &&
		lpImage )
	{
		int iDir;
		int iScanLineBytes = (lpbmi->bmiHeader.biBitCount * lpbmi->bmiHeader.biWidth + 31) / 32 * 4;  // align to dword
		int iMinWidth, iMinHeight;
		
		if( lpbmi->bmiHeader.biHeight > 0 )
		{	//����Ϊ�׵���
			iDir = -1;
			iMinHeight = MIN( lpImage->bmHeight, nHeight );
		}
		else
		{	//����Ϊ������
			iDir = 1;
			iMinHeight = MIN( lpImage->bmHeight, nHeight );
		}

		iMinWidth = MIN( lpImage->bmWidth, lpbmi->bmiHeader.biWidth );
		rcDestClip.left = 0;
		rcDestClip.top = 0;
		rcDestClip.right = iMinWidth;
		rcDestClip.bottom = iMinHeight;
		//��Դ��Ŀ��ת��λͼ����
		return _WinGdi_ConvertImageColorValue( 
			lpDrv,
			(_LPBITMAP_DIB)lpImage,
			&rcDestClip,
			lpbmi,
			NULL,
			uStartScan,
			cScanLines,
			iScanLineBytes,
			lpvBits,
			fuColorUse
			);
		
	}
	return 0;
}

// **************************************************
// ������HBITMAP WINAPI WinGdi_CreateDIBSection( 
//						 HDC hdc, 
//						 CONST BITMAPINFO *lpbmi,
//						 UINT iUsage,       //must = PAL
//						 VOID ** lppvBits,
//						 HANDLE hSection,  // must = NULL
//						 DWORD dwOffset   // = 0
//						 )

// ������
//	IN hdc - DC���
//	IN lpbmi - BITMAPINFO�ṹָ�룬����˵����Ҫ������λͼ��Ϣ
//	IN iUsage - ˵��BITMAPINFO�ṹ��ԱbmiColors�����ͣ�
//		DIB_PAL_COLORS - bmiColorsΪ16�������飬ÿһ������ֵָ��ǰhdc�ĵ�ɫ��Ķ�Ӧ�� 
//		DIB_RGB_COLORS - bmiColorsΪ�졢�̡�����ɫ����
//	IN lppvBits - ���ڽ���ָ��λͼ���ݵ��ڴ��ַ
//	IN hSection - ����ΪNULL
//	IN dwOffset - ����Ϊ0
// ����ֵ��
//	����ɹ���������Ч��λͼ��������򣬷���NULL
// ����������
//	����һ���û���ֱ��дλͼ���ݵĲ������豸��λͼ
// ����: 
//	ϵͳAPI
// ************************************************

HBITMAP WINAPI WinGdi_CreateDIBSection( 
						 HDC hdc, 
						 CONST BITMAPINFO *lpbmi,
						 UINT iUsage,       //must = PAL
						 VOID ** lppvBits,
						 HANDLE hSection,  // must = NULL
						 DWORD dwOffset   // = 0
						 )
{
	//Handle to a device context. If the value of iUsage is DIB_PAL_COLORS, 
	//the function uses this device context's logical palette to initialize the DIB's colors
	//�ҽ�����֧�ָù���

	if( lppvBits )
		*lppvBits = NULL;
    {
        _LPBITMAPDATA lpBitmap;
		HBITMAP hBitmap;
		int iHeight = lpbmi->bmiHeader.biHeight;
		int iDir;

		if( iHeight < 0 )
		{	//����Ϊ�׵���
			iDir = 0;
			iHeight = -iHeight;
		}
		else
			iDir = 1;
		//����λͼ����
		hBitmap = _WinGdi_CreateBitmap( 
			              lpbmi->bmiHeader.biWidth, 
						  iHeight,
			              lpbmi->bmiHeader.biPlanes, 
			              lpbmi->bmiHeader.biBitCount,
						  iDir,
			              NULL,	              
			              &lpBitmap,
						  0,
						  lpbmi,
						  FALSE );
		if( lpBitmap )
		{	//��Ϊ���ڴ�Ϊ GWME ���̴�������������û�ж�����ʵ�Ȩ��������Ҫӳ��
			//�Ժ󽫸ı�÷�����ֱ�Ӵӵ����߽��̷���
			if( lppvBits )
				*lppvBits = MapPtrToProcess( (LPVOID)lpBitmap->bmBits, hgwmeProcess );//GetCurrentProcess() );
			return hBitmap;
		}	
    }
    return NULL;
}

// **************************************************
// ������HBITMAP WINAPI WinGdi_CreateDIBitmap(
//								  HDC hdc,                        // handle to DC
//								  CONST BITMAPINFOHEADER *lpbmih, // bitmap data
//								  DWORD fdwInit,                  // initialization option
//								  CONST VOID *lpbInit,            // initialization data
//								  CONST BITMAPINFO *lpbmi,        // color-format data
//								  UINT fuUsage                    // color-data usage
//								  )
// ������
//	IN hdc - DC���
//	IN lpbmih �C BITMAPINFOHEADER�ṹָ�룬����������Ҫ������λͼ�ĸ߶ȺͿ��
//	IN fdwInit �C ��ʼ����־��������CBM_INIT��ϵͳ��lpbInit ��lpbmiȥ��ʼ��λͼ����
//	IN lpbInit �C ������Ҫ��ʼ����λͼ���ݣ����ݸ�ʽ��lpbmi˵��
//	IN lpbmi �C BITMAPINFO�ṹָ�룬��������lpbInitָ������ݸ�ʽ
//	IN fuUsage - ˵��BITMAPINFO�ṹ��ԱbmiColors�����ͣ�
//			DIB_PAL_COLORS �C bmiColorsΪ16�������飬ÿһ������ֵָ��ǰhdc�ĵ�ɫ��Ķ�Ӧ�� 
//			DIB_RGB_COLORS �C bmiColorsΪ�졢�̡�����ɫ����
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	����һ���豸������λͼ�����豸��������λͼ��������
// ����: 
//	ϵͳAPI
// ************************************************

HBITMAP WINAPI WinGdi_CreateDIBitmap(
								  HDC hdc,                        // handle to DC
								  CONST BITMAPINFOHEADER *lpbmih, // bitmap data
								  DWORD fdwInit,                  // initialization option
								  CONST VOID *lpbInit,            // initialization data
								  CONST BITMAPINFO *lpbmi,        // color-format data
								  UINT fuUsage                    // color-data usage
								  )
{
	HBITMAP hBitmap;
	int iHeight = lpbmih->biHeight;

	if( iHeight < 0 )
		iHeight = -iHeight;
	//����λͼ����
	hBitmap = WinGdi_CreateCompatibleBitmap( hdc, lpbmih->biWidth, iHeight );
	
	if( hBitmap && fdwInit == CBM_INIT )
	{	//��ʼ��λͼͼ������
		if( WinGdi_SetDIBits( hdc, hBitmap, 0, iHeight, lpbInit, lpbmi, fuUsage ) == 0 )
		{	//ʧ�ܣ��ͷ�֮ǰ�����Ķ���
			WinGdi_DeleteObject( hBitmap );
			hBitmap = NULL;
		}
	}
	return hBitmap;
}

// **************************************************
// ������HFONT WinGdi_CreateFontIndirect(  const LOGFONT *lplf )
// ������
// 	IN lplf - LOGFONT �ṹָ�룬������������
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�����߼��������
// ����: 
//	ϵͳAPI
// ************************************************

HFONT WINAPI WinGdi_CreateFontIndirect(  const LOGFONT *lplf )
{
#ifdef HAVE_TRUETYPE
	extern const _FONTDRV FAR _TrueTypeDrv;

	_FONT * lpFont;
//	LOGFONT logFont;
	LOGFONT * lpLogfont = NULL;
	//�����������
	lpFont = BlockHeap_Alloc( hgwmeBlockHeap, 0, sizeof(_FONT) );
//	lpFont = BLK_Alloc( 0, sizeof(_FONT) );
	if( lpFont )
	{	//��ʼ����������
		memset( lpFont, 0, sizeof(_FONT) );
		lpFont->obj.objType = OBJ_FONT;//|OBJ_PROTECTED;
		lpFont->handle = (HANDLE)1;
		lpFont->dwData = 1;
		lpFont->lpDriver = (_LPFONTDRV)&_TrueTypeDrv;

		//lpFont->dwData = lpFont->lpDriver->lpInit();
		//lpFont->handle = lpFont->lpDriver->lpCreateFont( lpFont->dwData, lplf );
		//return (HFONT)PTR_TO_HANDLE( lpFont );
		if( ( lpFont->dwData = lpFont->lpDriver->lpInit() ) )
		{
			if( ( lpFont->handle = lpFont->lpDriver->lpCreateFont( lpFont->dwData, lplf ) ) );
			{
				AddToObjMgr( &lpFont->obj, OBJ_FONT, (ULONG)GetCallerProcess() );
				return (HFONT)PTR_TO_HANDLE( lpFont );
			}
		}
		BlockHeap_Free( hgwmeBlockHeap, 0, lpFont, sizeof(_FONT) );		
		DEBUGMSG( 1, ("not create font object.\r\n" ) );

	}
	return NULL;	

#else
    return NULL;
#endif
}

// **************************************************
// ������static HPEN _CreatePen( 
//						DWORD dwStyle, 
//						int iWidth, 
//						COLORREF clrColor, 
//						DWORD dwObjFlag, 
//						_LPPENDATA * lppPen )
// ������
//	IN dwStyle-�ʷ�񣬰�����
//			PS_SOLID-ʵ����
//			PS_DOT-����
//			PS_CENTER-������
//			PS_DASHED-������
//			PS_NULL-�ʲ��ɼ�
//	IN iWidth - �ʿ��(2�����Ͻ�֧��PS_SOLID�ʷ��)
//	IN clrColor - ����ɫ
//	IN dwObjFlag - �����־
//	OUT lppPen - ���ڽ��ܱʶ���ָ��
// ����ֵ��
//	����ɹ������رʶ����������򣬷���NULL
// ����������
//	�����ʶ���
// ����: 
//	
// ************************************************
 
static HPEN _CreatePen( DWORD dwStyle, int iWidth, COLORREF clrColor, DWORD dwObjFlag, _LPPENDATA * lppPen )
{
    _LPPENDATA lpPen = (_LPPENDATA)BlockHeap_Alloc( hgwmeBlockHeap, 0, sizeof(_PENDATA) );//malloc( sizeof( _PENDATA ) );
//    _LPPENDATA lpPen = (_LPPENDATA)BLK_Alloc( 0, sizeof(_PENDATA) );//malloc( sizeof( _PENDATA ) );
    if( lpPen )
    {	//��ʼ����������
        lpPen->penAttrib.pattern = (BYTE)dwStyle;
        lpPen->penAttrib.width = (BYTE)iWidth;
		lpPen->penAttrib.clrRef = clrColor;
        lpPen->penAttrib.color = lpDrvDisplayDefault->lpRealizeColor( clrColor, NULL, 0, 0 );
		AddToObjMgr( &lpPen->obj, OBJ_PEN|dwObjFlag, (ULONG)GetCallerProcess() );
		if( lppPen )
			*lppPen = lpPen;
		return (HPEN)PTR_TO_HANDLE( lpPen );
    }
	return NULL;
}

// **************************************************
// ������HPEN WINAPI WinGdi_CreatePenIndirect(const LOGPEN * lpPenData)
// ������
// 	IN lpPenData-ָ��LOGPEN�ṹ
// ����ֵ��
//	����ɹ������رʶ����������򣬷���NULL
// ����������
//	�����߼���
// ����: 
//	ϵͳAPI
// ************************************************

HPEN WINAPI WinGdi_CreatePenIndirect(const LOGPEN * lpPenData)
{
	return _CreatePen( lpPenData->lognStyle, lpPenData->lognWidth.x, lpPenData->lognColor, 0, NULL );
}

// **************************************************
// ������HPEN WINAPI WinGdi_CreatePen( int style, int width, COLORREF color )
// ������
//	IN style-�ʷ�񣬰�����
//			PS_SOLID-ʵ����
//			PS_DOT-����
//			PS_CENTER-������
//			PS_DASHED-������
//			PS_NULL-�ʲ��ɼ�
//	IN width-�ʿ��(2�����Ͻ�֧��PS_SOLID�ʷ��)
//	IN color-����ɫ
// ����ֵ��
//	����ɹ������رʶ����������򣬷���NULL
// ����������
//	�����߼���
// ����: 
//	ϵͳAPI
// ************************************************

HPEN WINAPI WinGdi_CreatePen( int style, int width, COLORREF color )
{
	return _CreatePen( style, width, color, 0, NULL );
}

//define stock brush
static const BYTE _pattern[STOCK_BRUSH_NUM][8] = {
    { 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00 },//HS_HORIZONTAL              /* ----- */
    { 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa, 0xaa },//HS_VERTICAL                /* ||||| */
    { 0x88, 0x44, 0x22, 0x11, 0x88, 0x44, 0x22, 0x11 },//HS_FDIAGONAL               /* \\\\\ */
    { 0x11, 0x22, 0x44, 0x88, 0x11, 0x22, 0x44, 0x88 },//HS_BDIAGONAL               /* ///// */
    { 0x55, 0xff, 0x55, 0xff, 0x55, 0xff, 0x55, 0xff },//HS_CROSS                   /* +++++ */
    { 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55 }// HS_DIAGCROSS               /* xxxxx */
};

// **************************************************
// ������static HBRUSH _CreateBrush( 
//								DWORD dwStyle, 
//								COLORREF color,
//								DWORD dwHatch, 
//								DWORD dwObjFlag, 
//								_LPBRUSHDATA * lppBrush )
// ������
//	IN dwStyle - ˢ�ӷ��
//	IN color - ��ɫֵ
//	IN dwHatch - ��Ӱ�����ͣ�����
//			HS_HORIZONTAL - �� ----- 
//			HS_VERTICAL - �� ||||| 
//			HS_FDIAGONAL - �� \\\\\ 
//			HS_BDIAGONAL - �� ///// 
//			HS_CROSS - �� +++++ 
//			HS_DIAGCROSS - �� xxxxx 
//	IN dwObjFlag - �����־
//	OUT lppBrush - ���ڽ���ˢ�Ӷ���ָ��
// ����ֵ��
//	����ɹ�������ˢ�Ӷ����������򣬷���NULL
// ����������
//	����ˢ��
// ����: 
//	
// ************************************************

static HBRUSH _CreateBrush( DWORD dwStyle, COLORREF color, DWORD dwHatch, DWORD dwObjFlag, _LPBRUSHDATA * lppBrush )
{	//�������

	if( dwStyle == BS_SOLID || dwStyle == BS_HATCHED || dwStyle == BS_PATTERN || dwStyle == BS_NULL )
	{
		_LPBRUSHDATA lpBrush = (_LPBRUSHDATA)BlockHeap_Alloc( hgwmeBlockHeap, 0, sizeof(_BRUSHDATA) );//malloc( sizeof( _BRUSHDATA ) );
		//    _LPBRUSHDATA lpBrush = (_LPBRUSHDATA)BLK_Alloc( 0, sizeof(_BRUSHDATA) );//malloc( sizeof( _BRUSHDATA ) );
		
		if( lpBrush )
		{	//��ʼ������
			memset( lpBrush, 0, sizeof(*lpBrush) );
			lpBrush->brushAttrib.style = (BYTE)dwStyle;
//			lpBrush->brushAttrib.origin.x = lpBrush->brushAttrib.origin.y = 0;
			lpBrush->brushAttrib.clrRef = color;
			lpBrush->brushAttrib.color = lpDrvDisplayDefault->lpRealizeColor( color, NULL, 0, 0 );
			
			lpBrush->brushAttrib.style = (BYTE)dwStyle;
			if( dwStyle == BS_HATCHED )
			{	//��Ӱ�߷��				
				lpBrush->brushAttrib.hatch = (BYTE)(dwHatch % STOCK_BRUSH_NUM);
				if( lpBrush->brushAttrib.hatch != BS_NULL )
					memcpy( lpBrush->brushAttrib.pattern, _pattern[lpBrush->brushAttrib.hatch], 8 );
			}
			else if( dwStyle == BS_SOLID )
			{   //��ɫ���� BS_SOLID				
				//lpBrush->brushAttrib.hatch = 0;
				//memset( lpBrush->brushAttrib.pattern, 0xffffffff, 8 );
				//
				;
			}
			else if( dwStyle == BS_PATTERN )
			{				
				lpBrush->brushAttrib.lpbmpBrush = _GetHBITMAPPtr( (HANDLE)dwHatch );
				if( lpBrush->brushAttrib.lpbmpBrush == NULL )
				{
					BlockHeap_Free( hgwmeBlockHeap, 0, lpBrush, sizeof(_BRUSHDATA) );
					goto _return;
				}
			}
			//��������������
			AddToObjMgr( &lpBrush->obj, OBJ_BRUSH|dwObjFlag, (ULONG)GetCallerProcess() );
			if( lppBrush )
				*lppBrush = lpBrush;
			return (HBRUSH)PTR_TO_HANDLE( lpBrush );
		}
	}
_return:

	return NULL;
}

// **************************************************
// ������HBRUSH WINAPI WinGdi_CreateHatchBrush(int style, COLORREF color)
// ������
// 	IN style - ��Ӱ�����ͣ�����
//			HS_HORIZONTAL - �� ----- 
//			HS_VERTICAL - �� ||||| 
//			HS_FDIAGONAL - �� \\\\\ 
//			HS_BDIAGONAL - �� ///// 
//			HS_CROSS - �� +++++ 
//			HS_DIAGCROSS - �� xxxxx 
// ����ֵ��
//	����ɹ�������ˢ�Ӷ����������򣬷���NULL
// ����������
//	����ˢ��
// ����: 
//	ϵͳAPI
// ************************************************

HBRUSH WINAPI WinGdi_CreateHatchBrush(int style, COLORREF color)
{
	return _CreateBrush( BS_HATCHED, color, (DWORD)style, 0, NULL );
}

// **************************************************
// ������HBRUSH WINAPI WinGdi_CreateBrushIndirect(const LOGBRUSH *lpBrushData)
// ������
// 	IN lpBrushData-ָ��LOGBRUSH�ṹָ��
// ����ֵ��
//	����ɹ�������ˢ�Ӷ����������򣬷���NULL
// ����������
//	����ˢ��
// ����: 
//	ϵͳAPI
// ************************************************

HBRUSH WINAPI WinGdi_CreateBrushIndirect(const LOGBRUSH *lpBrushData)
{

	return _CreateBrush( lpBrushData->lbStyle, lpBrushData->lbColor, (DWORD)lpBrushData->lbHatch, 0, NULL );
}

// **************************************************
// ������HBRUSH WINAPI WinGdi_CreateSolidBrush( COLORREF color )
// ������
// 	IN color - ˢ����ɫ
// ����ֵ��
//	����ɹ�������ˢ�Ӷ����������򣬷���NULL
// ����������
//	������ɫˢ��
// ����: 
//	ϵͳAPI
// ************************************************

HBRUSH WINAPI WinGdi_CreateSolidBrush( COLORREF color )
{
	return _CreateBrush( BS_SOLID, color, 0, 0, NULL );
}

// **************************************************
// ������HBRUSH _CreateSysBrush( COLORREF color, UINT uiStyle )
// ������
// 	IN color - ˢ����ɫ
//	IN uiStyle - ˢ�ӷ��
// ����ֵ��
//	����ɹ�������ˢ�Ӷ����������򣬷���NULL
// ����������
//	����ϵͳˢ��
// ����: 
//	
// ************************************************

HBRUSH _CreateSysBrush( COLORREF color, UINT uiStyle )
{
	return _CreateBrush( uiStyle, color, 0, OBJ_FREE_DISABLE, NULL );
}

// **************************************************
// ������void _DeleteSysSolidBrush( HBRUSH hBrush )
// ������
// 	IN hBrush - ˢ�Ӿ��
// ����ֵ��
//	��
// ����������
//	ɾ��ˢ��
// ����: 
//	
// ************************************************

void _DeleteSysSolidBrush( HBRUSH hBrush )
{
	_LPBRUSHDATA lpBrush;

	EnterCriticalSection( &csGdiObjList );

	lpBrush = _GetHBRUSHPtr( hBrush );
	if( lpBrush )
	{	//�Ӷ�����������Ƴ�
		RemoveFromObjMgr( (LPOBJLIST)lpBrush );
		//ɾ��
		BlockHeap_Free( hgwmeBlockHeap, 0, lpBrush, sizeof(_BRUSHDATA) );
//		BLK_Free( 0, lpBrush, sizeof(_BRUSHDATA) );
	}

	LeaveCriticalSection( &csGdiObjList );
}

// **************************************************
// ������BOOL _SetSysSolidBrushColor( HBRUSH hBrush , COLORREF color )
// ������
// 	IN hBrush - ˢ�Ӷ�����
//	IN color - ��ɫ
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	����ˢ�ӵ���ɫ
// ����: 
//	
// ************************************************

BOOL _SetSysSolidBrushColor( HBRUSH hBrush , COLORREF color )
{
	_LPBRUSHDATA lpBrush = _GetHBRUSHPtr( hBrush );

    if( lpBrush )
    {
		lpBrush->brushAttrib.clrRef = color;
        lpBrush->brushAttrib.color = lpDrvDisplayDefault->lpRealizeColor( color, NULL, 0, 0 );
		return TRUE;
    }
    return FALSE;
}

// **************************************************
// ������HPEN _CreateSysPen( COLORREF color, UINT uiStyle )
// ������
// 	IN color - ����ɫ
//	IN uiStyle - �ʷ��
// ����ֵ��
//	����ɹ������رʶ����������򣬷���NULL
// ����������
//	����ϵͳ��
// ����: 
//	
// ************************************************

HPEN _CreateSysPen( COLORREF color, UINT uiStyle )
{
	return _CreatePen( uiStyle, 1, color, OBJ_FREE_DISABLE, NULL );
}

// **************************************************
// ������HFONT _CreateSysFont( UINT id )
// ������
// 	IN id - �������ID
// ����ֵ��
//	����ɹ���������������������򣬷���NULL
// ����������
//	����ϵͳ�������
// ����: 
//	
// ************************************************

HFONT _CreateSysFont( UINT id )
{
	extern const _FONTDRV FAR * _lpSYSFontDrv;
	extern const _FONTDRV FAR _ROM8x8Drv;
	extern const _FONTDRV FAR _ROM8x6Drv;
	extern const _FONTDRV FAR _PhoneticDrv;
	extern const _FONTDRV FAR _SymbolDrv;
	extern const _FONTDRV FAR _SYSFont16Drv;
	extern const _FONTDRV FAR _SYSFont24Drv;
	extern const _FONTDRV FAR _TrueTypeDrv;

	_FONT * lpFont;
	LOGFONT logFont;
	LOGFONT * lpLogfont = NULL;
	//�����������
	lpFont = BlockHeap_Alloc( hgwmeBlockHeap, 0, sizeof(_FONT) );
//	lpFont = BLK_Alloc( 0, sizeof(_FONT) );
	if( lpFont )
	{	//��ʼ����������
		memset( lpFont, 0, sizeof(_FONT) );
		lpFont->obj.objType = OBJ_FONT|OBJ_FREE_DISABLE;
		lpFont->handle = (HANDLE)1;
		lpFont->dwData = 1;
		switch( id )
		{
		//case SYSTEM_FONT:
			//lpFont->lpDriver = (_LPFONTDRV)_lpSYSFontDrv;//(_LPFONTDRV)&_SYSFontDrv;
			//break;
		//case SYSTEM_FONT_SYMBOL:
			//lpFont->lpDriver = (_LPFONTDRV)_SymbolDrv;//(_LPFONTDRV)&_SYSFontDrv;
			//break;
#ifdef HAVE_16X16_CHS_FIXED
		case SYSTEM_FONT_CHINESE16X16:
			lpFont->lpDriver = (_LPFONTDRV)&_SYSFont16Drv;
			break;
#endif	//HAVE_16X16_CHS_FIXED
#ifdef HAVE_24X24_CHS_FIXED
		case SYSTEM_FONT_CHINESE24X24:
			lpFont->lpDriver = (_LPFONTDRV)&_SYSFont24Drv;
			break;
#endif	//HAVE_24X24_CHS_FIXED
#ifdef HAVE_16X16_SYMBOL_FIXED
		case SYSTEM_FONT_SYMBOL16X16:
			lpFont->lpDriver = (_LPFONTDRV)&_SymbolDrv;
			logFont.lfHeight = logFont.lfWidth = 16;
			lpLogfont = &logFont;
			break;
#endif	//HAVE_16X16_SYMBOL_FIXED
#ifdef HAVE_24X24_SYMBOL_FIXED
		case SYSTEM_FONT_SYMBOL24X24:
			lpFont->lpDriver = (_LPFONTDRV)&_SymbolDrv;
			logFont.lfHeight = logFont.lfWidth = 24;
			lpLogfont = &logFont;
			break;
#endif	//HAVE_24X24_SYMBOL_FIXED
#ifdef HAVE_8X16_PHONETIC_FIXED
		case SYSTEM_FONT_PHONETIC:
			lpFont->lpDriver = (_LPFONTDRV)&_PhoneticDrv;
			break;
#endif	//HAVE_8X16_PHONETIC_FIXED
#ifdef HAVE_8X8_ENG_FIXED
		case SYSTEM_FONT_ENGLISH8X8:
			lpFont->lpDriver = (_LPFONTDRV)&_ROM8x8Drv;
			break;
#endif	//HAVE_8X8_ENG_FIXED
#ifdef HAVE_8X6_ENG_FIXED
		case SYSTEM_FONT_ENGLISH8X6:
			lpFont->lpDriver = (_LPFONTDRV)&_ROM8x6Drv;
			break;
#endif	//HAVE_8X6_ENG_FIXED
#ifdef HAVE_TRUETYPE
		case SYSTEM_FONT_TRUETYPE:
//			lpFont->lpDriver = (_LPFONTDRV)&_TrueTypeDrv;
//			break;
		case SYSTEM_FONT_TRUETYPE_BIG:
			lpFont->lpDriver = (_LPFONTDRV)&_TrueTypeDrv;
			logFont.lfHeight = logFont.lfWidth = 24;
			logFont.lfCharSet = GB2312_CHARSET;//ANSI_CHARSET;
			lpLogfont = &logFont;
			break;
		case SYSTEM_FONT_TRUETYPE_SMALL:
			lpFont->lpDriver = (_LPFONTDRV)&_TrueTypeDrv;
			logFont.lfHeight = logFont.lfWidth = 16;
			logFont.lfCharSet = GB2312_CHARSET;//ANSI_CHARSET;
			lpLogfont = &logFont;
			break;
#endif	//HAVE_8X6_ENG_FIXED

		}
		lpFont->dwData = lpFont->lpDriver->lpInit();
		lpFont->handle = lpFont->lpDriver->lpCreateFont( lpFont->dwData, lpLogfont );
		return (HFONT)PTR_TO_HANDLE( lpFont );
	}
	return NULL;	
}

// **************************************************
// ������HBITMAP _CreateSysBitmap( 
//							int nWidth, 
//							int nHeight, 
//							UINT cBitsPerPel )
// ������
// 	IN nWidth - λͼ���
//	IN nHeight - λͼ�߶�
//	IN cBitsPerPel - λͼ�ĵ��ʽ��ÿ�����bit����
// ����ֵ��
//	����ɹ�������λͼ��������򣬷���NULL
// ����������
//	����ϵͳλͼ����
// ����: 
//	
// ************************************************

HBITMAP _CreateSysBitmap( int nWidth, 
						  int nHeight, 
						  UINT cBitsPerPel )
{
	return _WinGdi_CreateBitmap( nWidth, nHeight, 1, cBitsPerPel, 0, NULL, NULL, OBJ_FREE_DISABLE, NULL, FALSE );
}

// **************************************************
// ������static BOOL _DeletePenObject(HGDIOBJ hgdi)
// ������
// 	IN hgdi - ͼ�ζ���
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	ɾ���ʶ���
// ����: 
//	
// ************************************************

static BOOL _DeletePenObject(HGDIOBJ hgdi)
{
	_LPPENDATA lpObj = _GetHPENPtr( hgdi );
	//��Ч�������ͷ���
    if( lpObj && !(lpObj->obj.objType & OBJ_FREE_DISABLE) )
	{
		lpObj->obj.objType = OBJ_NULL;
		BlockHeap_Free( hgwmeBlockHeap, 0, lpObj, sizeof(_PENDATA) );
		//BLK_Free( 0, lpObj, sizeof(_PENDATA) );
		return TRUE;
	}
	return FALSE;
}

// **************************************************
// ������static BOOL _DeleteBrushObject(HGDIOBJ hgdi)
// ������
// 	IN hgdi - ͼ�ζ���
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	ɾ��ˢ�Ӷ���
// ����: 
//	
// ************************************************

static BOOL _DeleteBrushObject(HGDIOBJ hgdi)
{
    _LPBRUSHDATA lpObj = _GetHBRUSHPtr( hgdi );
	//��Ч�������ͷ���
    if( lpObj && !(lpObj->obj.objType & OBJ_FREE_DISABLE) )
	{
		lpObj->obj.objType = OBJ_NULL;
		BlockHeap_Free( hgwmeBlockHeap, 0, lpObj, sizeof(_BRUSHDATA) );//free( lpObj );
		//BLK_Free( 0, lpObj, sizeof(_BRUSHDATA) );//free( lpObj );
		return TRUE;
	}
	return FALSE;
}

// **************************************************
// ������static BOOL _DeleteBitmapObject(HGDIOBJ hgdi)
// ������
// 	IN hgdi - ͼ�ζ���
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	ɾ��λͼ����
// ����: 
//	
// ************************************************

static BOOL _DeleteBitmapObject(HGDIOBJ hgdi)
{
    _LPBITMAPDATA lpObj = _GetHBITMAPPtr( hgdi );
	//��Ч�������ͷ���
    if( lpObj && !(lpObj->obj.objType & OBJ_FREE_DISABLE) )
	{	//��
		lpObj->obj.objType = OBJ_NULL;
		if( lpObj->bmFlags & 0x1 )
		{	//λͼָ��ʹ�� Page_AllocMem �õ���
			Page_FreeMem( lpObj->bmBits, lpObj->bmWidthBytes * lpObj->bmHeight * lpObj->bmPlanes );
		}
		else
		{	//λͼָ��ʹ�� BlockHeap_Alloc �õ���
			DWORD size = lpObj->bmWidthBytes * lpObj->bmHeight * lpObj->bmPlanes;

			BlockHeap_Free( hgwmeBlockHeap, 0, lpObj->bmBits, size );
			//BLK_Free( 0, lpObj->bmBits, size );
		}
		BlockHeap_FreeString( hgwmeBlockHeap, 0, lpObj );
		//BLK_FreeString( 0, lpObj );
		return TRUE;
	}
	return FALSE;
}

// **************************************************
// ������static BOOL _DeleteFontObject(HGDIOBJ hgdi)
// ������
// 	IN hgdi - ͼ�ζ���
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	ɾ���������
// ����: 
//	
// ************************************************

static BOOL _DeleteFontObject(HGDIOBJ hgdi)
{
    _LPFONT lpObj = _GetHFONTPtr( hgdi );
	//��Ч�������ͷ���
	if( lpObj && !(lpObj->obj.objType & OBJ_FREE_DISABLE) )
	{	//��
		lpObj->obj.objType = OBJ_NULL;
		BlockHeap_Free( hgwmeBlockHeap, 0, lpObj, sizeof(_FONT) );//free( lpObj );
//		BLK_Free( 0, lpObj, sizeof(_FONT) );//free( lpObj );
		return TRUE;
	}
	return FALSE;
}

// **************************************************
// ������BOOL WINAPI WinGdi_DeleteObject( HGDIOBJ hgdiobj )
// ������
// 	IN hgdi - ͼ�ζ���
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	ɾ��ͼ�ζ���
// ����: 
//	ϵͳAPI
// ************************************************

BOOL WINAPI WinGdi_DeleteObject( HGDIOBJ hgdiobj )
{
	extern BOOL _DeleteRgnObject( HRGN hrgn );
	BOOL bRetv = FALSE;
	LPVOID lpvObj;

	EnterCriticalSection( &csGdiObjList );

	lpvObj = (LPVOID)HANDLE_TO_PTR( hgdiobj );

    if( lpvObj )
	{
		DWORD dwType  = *( (DWORD*)lpvObj );
		//�����ͷ��� ��
		if( !(dwType & OBJ_FREE_DISABLE) )
		{	//����
			switch( (WORD)dwType )
			{
			case OBJ_REGION:			//�������
				ObjList_Remove( &_lpGdiObjList, lpvObj );//hgdi );
				bRetv = _DeleteRgnObject( hgdiobj );
				break;
			case OBJ_PEN:				//�ʶ���
				ObjList_Remove( &_lpGdiObjList, lpvObj );
				bRetv = _DeletePenObject( hgdiobj );
				break;
			case OBJ_BRUSH:				//ˢ�Ӷ���
				ObjList_Remove( &_lpGdiObjList, lpvObj );
				bRetv = _DeleteBrushObject( hgdiobj );
				break;
			case OBJ_BITMAP:			//λͼ����
				if( --( ( (LPOBJLIST)lpvObj )->iRefCount ) == 0 )
				{
				    ObjList_Remove( &_lpGdiObjList, lpvObj );
				    bRetv = _DeleteBitmapObject( hgdiobj );
				}
				break;
			case OBJ_FONT:				//�������
				ObjList_Remove( &_lpGdiObjList, lpvObj );
				bRetv = _DeleteFontObject( hgdiobj );
				break;
			case OBJ_PAL:
				ObjList_Remove( &_lpGdiObjList, lpvObj );
				bRetv = _DeletePaletteObject( hgdiobj );
				break;
			default:
				SetLastError( ERROR_INVALID_HANDLE );
				WARNMSG( 1, ( "invalid HGDIOBJ handle(0x%x) type when call DeleteObject.\r\n", hgdiobj ) );
				break;
			}
		}
		else
		    bRetv = TRUE; 
	}
	else
	{
		//ASSERT_NOTIFY( 0, "invalid HGDIOBJ handle when call DeleteObject\r\n" );
		WARNMSG( 1, ( "invalid HGDIOBJ handle(0x%x) when call DeleteObject.\r\n", hgdiobj ) );
	}

	LeaveCriticalSection( &csGdiObjList );

    return bRetv;
}

// **************************************************
// ������BOOL DeleteGdiObject( HANDLE hOwner )
// ������
// 	IN hOwner - �����ӵ����
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�ͷŸ�ӵ���ߵ�����ͼ�ζ���
// ����: 
//	
// ************************************************

BOOL DeleteGdiObject( HANDLE hOwner )
{
    EnterCriticalSection( &csGdiObjList );

	{
		LPOBJLIST lpObj = _lpGdiObjList;
		
		if( lpObj )
		{
			do{
				if( lpObj->ulOwner == (ULONG)hOwner )
				{
					LPOBJLIST lpTemp = lpObj->lpNext;
#ifdef __DEBUG
					WARNMSG( 1, ("not release gdi object:handle(0x%x),objtype(%d).\r\n", PTR_TO_HANDLE( (HGDIOBJ)lpObj ), lpObj->objType ) );
					
#endif
					WinGdi_DeleteObject( (HGDIOBJ)PTR_TO_HANDLE( (HGDIOBJ)lpObj ) );
					lpObj = lpTemp;
				}
				else
					lpObj = lpObj->lpNext;
			}while( lpObj );
		}
	}

	LeaveCriticalSection( &csGdiObjList );
	return TRUE;
}

// **************************************************
// ������_LPPENDATA _GetHPENPtr( HPEN hPen )
// ������
// 	IN hPen - �ʶ���
// ����ֵ��
//	����ɹ������ض���ָ�룻���򣬷���NULL
// ����������
//	�����������õ�����ָ��
// ����: 
//	
// ************************************************

_LPPENDATA _GetHPENPtr( HPEN hPen )
{
    _LPPENDATA lpPen = (_LPPENDATA)HANDLE_TO_PTR( hPen );

	if( lpPen && (WORD)GET_OBJ_TYPE(lpPen) == OBJ_PEN )
        return (_LPPENDATA)lpPen;
	WARNMSG( 1, ("error: Invalid HPEN handle=0x%x\r\n", hPen) );
	SetLastError( ERROR_INVALID_HANDLE );
    return NULL;
}

// **************************************************
// ������_LPBITMAPDATA _GetHBITMAPPtr( HBITMAP hBitmap )
// ������
// 	IN hBitmap - λͼ����
// ����ֵ��
//	����ɹ������ض���ָ�룻���򣬷���NULL
// ����������
//	�����������õ�����ָ��
// ����: 
//	
// ************************************************

_LPBITMAPDATA _GetHBITMAPPtr( HBITMAP hBitmap )
{
	if( hBitmap == hbmpDisplayBitmap )
	{
		return (_LPBITMAPDATA)lpDisplayBitmap;
	}
	else
	{
		_LPBITMAPDATA lpbmp = (_LPBITMAPDATA)HANDLE_TO_PTR( hBitmap );
		
		if( lpbmp && (WORD)GET_OBJ_TYPE(lpbmp) == OBJ_BITMAP )
			return lpbmp;
		WARNMSG( 1, ("error: Invalid HBITMAP handle=0x%x\r\n", hBitmap) );
		SetLastError( ERROR_INVALID_HANDLE );
		return NULL;
	}	
}

// **************************************************
// ������_LPBRUSHDATA _GetHBRUSHPtr( HBRUSH hBrush )
// ������
// 	IN hBrush - ˢ�Ӷ���
// ����ֵ��
//	����ɹ������ض���ָ�룻���򣬷���NULL
// ����������
//	�����������õ�����ָ��
// ����: 
//	
// ************************************************

_LPBRUSHDATA _GetHBRUSHPtr( HBRUSH hBrush )
{
	_LPBRUSHDATA lpBrush = (_LPBRUSHDATA)HANDLE_TO_PTR( hBrush );

	if( lpBrush && (WORD)GET_OBJ_TYPE(lpBrush) == OBJ_BRUSH )
        return lpBrush;
	WARNMSG( 1, ("error: Invalid HBRUSH handle=0x%x\r\n", hBrush) );
	SetLastError( ERROR_INVALID_HANDLE );
    return NULL;
}


// **************************************************
// ������_LPPALETTE_DATA _GetHPALETTEPtr( HPALETTE hBrush )
// ������
// 	IN hBrush - ˢ�Ӷ���
// ����ֵ��
//	����ɹ������ض���ָ�룻���򣬷���NULL
// ����������
//	�����������õ�����ָ��
// ����: 
//	
// ************************************************

_LPPALETTE_DATA _GetHPALETTEPtr( HPALETTE hPal )
{
	_LPPALETTE_DATA lpPal = (_LPPALETTE_DATA)HANDLE_TO_PTR( hPal );

	if( lpPal && (WORD)GET_OBJ_TYPE(lpPal) == OBJ_PAL )
        return lpPal;
	WARNMSG( 1, ("error: Invalid HPALETTE handle=0x%x\r\n", hPal) );
	SetLastError( ERROR_INVALID_HANDLE );
    return NULL;
}


// **************************************************
// ������static BOOL _DeletePaletteObject(HGDIOBJ hgdi)
// ������
// 	IN hgdi - ͼ�ζ���
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	ɾ����ɫ�����
// ����: 
//	
// ************************************************

static BOOL _DeletePaletteObject(HGDIOBJ hgdi)
{
    _LPPALETTE_DATA lpObj = _GetHPALETTEPtr( hgdi );
	//��Ч�������ͷ���
    if( lpObj && !(lpObj->obj.objType & OBJ_FREE_DISABLE) )
	{
		lpObj->obj.objType = OBJ_NULL;
		BlockHeap_FreeString( hgwmeBlockHeap, 0, lpObj );
		return TRUE;
	}
	return FALSE;
}


// **************************************************
// ������HPALETTE WinGdi_CreatePalette( CONST LOGPALETTE *lplgpl );
// ������
// 	IN lplgpl - �߼���ɫ��ṹ
// ����ֵ��
//	����ɹ������ض����������򣬷���NULL
// ����������
//	������ɫ�����
// ����: 
//	
// ************************************************

HPALETTE WINAPI WinGdi_CreatePalette( CONST LOGPALETTE *lplgpl )
{
	if( lplgpl )
	{
		if( lplgpl->palNumEntries && lplgpl->palNumEntries < 256 )
		{
			UINT sp = sizeof(LOGPALETTE) + (lplgpl->palNumEntries - 1) * sizeof(PALETTEENTRY);
			UINT s = sizeof(_PALETTE_DATA) + sp;
			_LPPALETTE_DATA lpPalObj;
			
			lpPalObj = BlockHeap_AllocString( hgwmeBlockHeap, 0, sizeof(s) );
			if( lpPalObj )
			{
				memcpy( &lpPalObj->logPal, lplgpl, sp );
				AddToObjMgr( &lpPalObj->obj, OBJ_PAL, (ULONG)GetCallerProcess() );
				return (HPALETTE)PTR_TO_HANDLE( lpPalObj );
			}
		}		
	}
	return NULL;
}

// **************************************************
// ������UINT WinGdi_RealizePalette( HDC hdc );
// ������
// 	IN hdc - �豸���
// ����ֵ��
//	����ɹ�������ʵ��ӳ��ĵ�ɫ����Ŀ�������򣬷��� GDI_ERROR
// ����������
//	ӳ���ɫ�嵽DC
// ����: 
//	
// ************************************************

UINT WINAPI WinGdi_RealizePalette( HDC hdc )
{
	WARNMSG( 1, ("warn: the function RealizePalette is'nt support!.\r\n") );
	return GDI_ERROR;
}

// **************************************************
// ������UINT WinGdi_GetNearestPaletteIndex( HPALETTE hpal, COLORREF crColor );
// ������
// 	IN hpal - ��ɫ��������
//	IN crColor - RGB��ɫ
// ����ֵ��
//	����ɹ��������ڵ�ɫ��������Ŀ�����ţ����򣬷��� CLR_INVALID
// ����������
//	�õ�RGB��ɫ�ڵ�ɫ������ж�Ӧ����Ŀ������
// ����: 
//	ϵͳAPI
// ************************************************
UINT WINAPI WinGdi_GetNearestPaletteIndex( HPALETTE hpal, COLORREF color )
{
    _LPPALETTE_DATA lpPal = _GetHPALETTEPtr( hpal );

	if( lpPal )
	{
		UINT num = lpPal->logPal.palNumEntries;
		const PALETTEENTRY * lp = &lpPal->logPal.palPalEntry[0];

		const BYTE * lpsrc, * lpdst;
		LONG lMinValue, lDiff, d1, d2, d3;
		UINT like, i;
		
		//��ʼ�����Ϊ����ֵ
		lMinValue = MAXLONG;
		
		for( like = i = 0; i < num; i++, lp++ )
		{
			if( *((COLORREF*)lp) == color )
				return i;	//���ƥ��
			lpsrc = (const BYTE *)lp;
			lpdst = (const BYTE *)&color;
			//�õ���ֵ
			d1 = *lpdst++ - *lpsrc++; 
			d2 = *lpdst++ - *lpsrc++; 
			d3 = *lpdst++ - *lpsrc++;
			lDiff = d1 * d1 + d2 * d2 + d3 * d3;
			//ѡ������С��
			if( lDiff < lMinValue )
			{
				lMinValue = lDiff;
				like = i;
			}
		}
		return like;
	}
	return CLR_INVALID;
}

// **************************************************
// ������UINT WINAPI WinGdi_GetPaletteEntries(   HPALETTE hpal,
//												UINT iStartIndex,
//												UINT nEntries,
//												LPPALETTEENTRY lppe
//												);
// ������
// 	IN hpal - ��ɫ��������
//	IN iStartIndex - ��ʼ������
//	IN nEntries - ��Ҫ����������
//  IN lppe - ���ڽ��յ�ɫ����Ŀֵ���ڴ�ָ��
// ����ֵ��
//	����ɹ������ز���lppe��Ч������ʵ�ʿ������ڵ�ɫ��������Ŀ����
//	����ɹ������ز���lppeΪNULL������hpal��z�ܵ���Ŀ����
//  ʧ�ܣ����� 0
// ����������
//	�õ�ָ����ɫ��������Ŀ
// ����: 
//	ϵͳAPI
// ************************************************

UINT WINAPI WinGdi_GetPaletteEntries(
  HPALETTE hpal,
  UINT iStartIndex,
  UINT nEntries,
  LPPALETTEENTRY lppe
)
{
    _LPPALETTE_DATA lpPal = _GetHPALETTEPtr( hpal );
	UINT retv = 0;

	if( lpPal )
	{
		if( lppe )
		{
			if( iStartIndex < lpPal->logPal.palNumEntries )
			{
				UINT end = nEntries + iStartIndex;
				end = MIN( end, lpPal->logPal.palNumEntries );
				retv = ( end - iStartIndex );
				memcpy( lppe, &lpPal->logPal.palPalEntry[iStartIndex], sizeof(PALETTEENTRY) * retv );
			}
		}
		else
		{
			retv = lpPal->logPal.palNumEntries;
		}
	}
	return retv;
}

