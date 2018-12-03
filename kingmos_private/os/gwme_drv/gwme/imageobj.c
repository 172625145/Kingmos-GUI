/******************************************************
Copyright(c) 版权所有，1998-2005微逻辑。保留所有权利。
******************************************************/


/*****************************************************
文件说明：对图形对象进行管理(brush,bitmap,pen,font,region,palette)
版本号：3.0.0
开发时期：1999
作者：李林
修改记录：
	2004-02-21: WinGdi_CreateDIBSection  用 hgwmeProcess 代替 GetCurrentProcess
     2003-05-14 : CreateBitmap 为避免Heap的碎片化 对bitmap，
	             当其所用缓存大小 大于 某大小时， 用VirtualAlloc
				 当bmType = 1, 用VirtualAlloc；否则用,malloc
    2003-05-07: LN:将CreateBitmap的默认值由白改为黑
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
// 声明：BOOL __InitialGdiObjMgr( void )
// 参数：
// 	无
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	初始化 GDI 图形对象管理器
// 引用: 
//	
// ************************************************

BOOL _InitialGdiObjMgr( void )
{
	InitializeCriticalSection( &csGdiObjList );
	csGdiObjList.lpcsName = "CS-GOM";
	
	return TRUE;
}

// **************************************************
// 声明：void __DeInitialGdiObjMgr( void )
// 参数：
// 	无
// 返回值：
//	无
// 功能描述：
//	与 __InitialGdiObjMgr相反，释放图形对象管理器
// 引用: 
//	
// ************************************************

void _DeInitialGdiObjMgr( void )
{
	DeleteCriticalSection( &csGdiObjList );
}

// **************************************************
// 声明：BOOL AddToObjMgr( LPOBJLIST lpObj, DWORD objType, ULONG hOwner )
// 参数：
// 	IN lpObj - OBJLIST 结构指针
//	IN objType - 对象类型
//	IN hOwner - 对象拥有者进程
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	将对象加入对象链表
// 引用: 
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
// 声明：BOOL RemoveFromObjMgr( LPOBJLIST lpObj )
// 参数：
// 	IN lpObj - OBJLIST 结构指针
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	与 AddToObjMgr 相反，将对象移出对象链表
// 引用: 
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
// 声明：HBITMAP _GetShareBitmapObj( HINSTANCE hInst, DWORD dwName )
// 参数：
// 	IN hInst - 实例句柄
//	IN dwName - 对象名
// 返回值：
//	假如成功，返回位图对象句柄；否则，返回 NULL
// 功能描述：
//	得到共享对象句柄
// 引用: 
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
// 声明：HBITMAP _WinGdi_CreateBitmap( 
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
// 参数：
//	IN nWidth - 位图的宽度
//	IN nHeight - 位图的高度
//	IN cPlanes - 位面数
//	IN cBitsPerPel - 每个点的 bit数
//	IN iDir - 位图的方向（从顶向底或相反）
//	IN lpvBits - 位图数据
//	OUT lppBitmap - 用于接受描述位图格式的结构的指针
//	IN uiObjFlag - 附加的对象标志
//	IN lpbmi - 描述lpvBits位图格式的结构的指针,可以为NULL
//  IN bIsShareResource - 说明新对象是否需要共享（来自于资源文件， lpbmi必须= NULL)
// 返回值：
//	假如成功，返回位图对象句柄；否则，返回NULL
// 功能描述：
//	按要求创建位图对象
// 引用: 
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
	//扫描行字节宽度
	scanBytes = (cBitsPerPel * nWidth+31) / 32 * 4;  // align to dword
	//总的位图需要的字节数
	size = scanBytes * nHeight * cPlanes;
	//
	if( lpbmi == NULL )
	{	//默认的位图格式，分配 _BITMAPDATA结构对象
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
	{	//lpbmi 包含设备无关信息
		DWORD dwAllocSize = 0;
		DWORD dwPalSize = 0;
		UINT  uiPalIndexNum = 0;

		if( lpbmi->bmiHeader.biBitCount <= 8 )
		{	// 可能有调色板
			uiPalIndexNum = (1 << lpbmi->bmiHeader.biBitCount);
			if( lpbmi->bmiHeader.biClrUsed )			
			    uiPalIndexNum = MIN( lpbmi->bmiHeader.biClrUsed, uiPalIndexNum );
			dwPalSize =  uiPalIndexNum * sizeof(RGBQUAD);
		}
		else if( ( lpbmi->bmiHeader.biBitCount == 16 ||
			       lpbmi->bmiHeader.biBitCount == 32 )   && 
			     lpbmi->bmiHeader.biCompression == BI_BITFIELDS )
		{	//无需调色板，应该有三个DWORD的 RGB 占位描述符
			uiPalIndexNum = 3;
			dwPalSize = sizeof( DWORD ) * uiPalIndexNum;
		}
		//计算 _BITMAPDATA 结构对象大小
		dwAllocSize = sizeof( _BITMAP_DIB ) + dwPalSize - sizeof( DWORD );
		//分配 _BITMAPDATA结构对象
		lpBitmap = (_LPBITMAPDATA)BlockHeap_AllocString( hgwmeBlockHeap, 0, dwAllocSize );
		if( lpBitmap )
		{
			UINT i;
			PALETTEENTRY * lpPal = ((_LPBITMAP_DIB)lpBitmap)->palEntry;
			RGBQUAD * lpQuad = (RGBQUAD *)lpbmi->bmiColors;

			memset( lpBitmap, 0, dwAllocSize );
			//初始化调色板信息
			if( lpbmi->bmiHeader.biCompression == BI_BITFIELDS )
			{	//三个 DWORD  mask
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
    //分配位图数据缓存，为防止堆碎片。
	//如果缓存较小，则用BlockHeap_Alloc分配，
	//否则用Page_AllocMem分配
	if( bmType & BF_DIB )
	{   //frame buffer 会被 user 访问， so PM_SHARE
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
    {	//分配成功
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
		{	//如果有用户位图数据，则对齐到 WORD 
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
		{	//没有需要初始化的位图数据，为0
			memset( lpBitmap->bmBits, 0, size );
		}
        //加入对象链表
		AddToObjMgr( &lpBitmap->obj, OBJ_BITMAP | uiObjFlag, (ULONG)GetCallerProcess() );
    }
	else
	{	//错误，做清除工作
		if( lpBitmap )
		{
			BlockHeap_FreeString( hgwmeBlockHeap, 0, lpBitmap);
			//BLK_FreeString( 0, lpBitmap);
		}

		
		if( lpbBuf )
		{	//释放分配的位图缓存
			if( bmType & 1 )
			{	//该lpbBuf是用 Page_AllocMem分配的
				Page_FreeMem( lpbBuf, size );
			}
			else{	//该lpbBuf是用 BlockHeap_Alloc 分配的
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
// 声明：HBITMAP WINAPI WinGdi_CreateBitmap( 
//										int nWidth, 
//										int nHeight, 
//										UINT cPlanes, 
//										UINT cBitsPerPel, 
//										const VOID *lpvBits )
// 参数：
//	IN nWidth-位图宽度
//	IN nHeight-位图高度
//	IN cPlanes-位图位面数 (仅支持1位面)
//	IN cBitsPerPel-每点bit数
//	IN lpvBits-位图数据
// 返回值：
//	假如成功，返回有效的位图句柄；否则，返回 NULL
// 功能描述：
//	创建位图对象
// 引用: 
//	系统API
// ************************************************

HBITMAP WINAPI WinGdi_CreateBitmap( int nWidth, int nHeight, UINT cPlanes, UINT cBitsPerPel, const VOID *lpvBits )
{
	return _WinGdi_CreateBitmap( nWidth, nHeight, cPlanes, cBitsPerPel, 0, lpvBits, NULL, 0, NULL, FALSE );
}

// **************************************************
// 声明：int WINAPI WinGdi_GetDIBits(
//						 HDC hdc,           
//						 HBITMAP hbmp,      
//						 UINT uStartScan,   
//						 UINT cScanLines,   
//						 LPVOID lpvBits,    
//						 LPBITMAPINFO lpbi, 
//						 UINT uiUsage )
// 参数：
//	IN hdc -目标DC
//	IN hbmp - 位图句柄
//	IN uStartScan - 从位图的第几扫描行开始得到数据
//	IN cScanLines - 共需要得到的扫描行数
//	IN lpvBits - 用于得到位图数据的缓存，假如是NULL，该功能在lpbi里返回位图的大小和格式
//	IN lpbi - BITMAPINFO结构指针，说明需要返回的数据类型
//	IN uiUsage - 说明BITMAPINFO成员bmiColors的格式，可以为：
//			DIB_PAL_COLORS - 颜色表包含16bits的索引值数组，用于指向当前的调色板
//			DIB_RGB_COLORS - 颜色表包含红、绿、蓝值
// 返回值：
//	假如成功并且lpvBits为NULL，该功能填充lpbi结构并返回
//	位图总的扫描函数；假如成功并且lpvBits不为NULL，该功
//	能返回实际拷贝的扫描行数，lpvBits为得到的位图数据
// 功能描述：
//	用规定的格式返回位图的数据
// 引用: 
//	系统API
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
	{	//有效的位图对象
		if( lpvBits == NULL )
		{   // 填充位图信息 fill the lpbi data
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
		{	//需要位图数据
			_BITMAPDATA bmp;
			BITMAPINFO bi;
			RECT rcDestClip;
			_LPCDISPLAYDRV lpDrv;

			int iDir, iMinHeight, iMinWidth;
			//定义需要的位图格式数据
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
				return 0;	//系统不支持该位图格式
			//定义源位图格式数据
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
			//将源位图转化到目标格式			
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
// 声明：int WINAPI WinGdi_SetDIBits(
//						 HDC hdc,                  // handle to device context
//						 HBITMAP hbmp,             // handle to bitmap
//						 UINT uStartScan,          // starting scan line
//						 UINT cScanLines,          // number of scan lines
//						 CONST VOID *lpvBits,      // array of bitmap bits
//						 CONST BITMAPINFO *lpbmi,  // address of structure with bitmap data
//						 UINT fuColorUse           // type of color indexes to use
//						 )
// 参数：
//	IN hdc - DC句柄
//	IN hbmp - 需要设置数据的位图
//	IN uStartScan - 说明lpvBits的数据的开始扫描行
//	IN cScanLines -说明lpvBits的数据所具有的扫描行行数
//	IN lpvBits - 保存有位图数据的指针，该数据将拷贝到hbmp
//	IN lpbmi - BITMAPINFO数据结构，用于描述lpvBits指向的数据信息
//	IN fuColorUse - 说明BITMAPINFO结构成员bmiColors的类型：
//			DIB_PAL_COLORS - bmiColors为16索引数组，每一个索引值指向当前hdc的调色板的对应项 
//			DIB_RGB_COLORS - bmiColors为红、绿、蓝颜色数组
// 返回值：
//	假如成功，返回拷贝到hbmp的行数；否则，返回0
// 功能描述：
//	根据DIB数据格式设置位图数据到 hbmp
// 引用: 
//	系统API
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
	//得到该位图的驱动程序接口
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
		{	//方向为底到顶
			iDir = -1;
			iMinHeight = MIN( lpImage->bmHeight, nHeight );
		}
		else
		{	//方向为顶到底
			iDir = 1;
			iMinHeight = MIN( lpImage->bmHeight, nHeight );
		}

		iMinWidth = MIN( lpImage->bmWidth, lpbmi->bmiHeader.biWidth );
		rcDestClip.left = 0;
		rcDestClip.top = 0;
		rcDestClip.right = iMinWidth;
		rcDestClip.bottom = iMinHeight;
		//从源到目标转化位图数据
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
// 声明：HBITMAP WINAPI WinGdi_CreateDIBSection( 
//						 HDC hdc, 
//						 CONST BITMAPINFO *lpbmi,
//						 UINT iUsage,       //must = PAL
//						 VOID ** lppvBits,
//						 HANDLE hSection,  // must = NULL
//						 DWORD dwOffset   // = 0
//						 )

// 参数：
//	IN hdc - DC句柄
//	IN lpbmi - BITMAPINFO结构指针，用于说明需要创建的位图信息
//	IN iUsage - 说明BITMAPINFO结构成员bmiColors的类型：
//		DIB_PAL_COLORS - bmiColors为16索引数组，每一个索引值指向当前hdc的调色板的对应项 
//		DIB_RGB_COLORS - bmiColors为红、绿、蓝颜色数组
//	IN lppvBits - 用于接收指向位图数据的内存地址
//	IN hSection - 必须为NULL
//	IN dwOffset - 必须为0
// 返回值：
//	假如成功，返回有效的位图句柄；否则，返回NULL
// 功能描述：
//	创建一个用户能直接写位图数据的不依赖设备的位图
// 引用: 
//	系统API
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
	//我将来会支持该功能

	if( lppvBits )
		*lppvBits = NULL;
    {
        _LPBITMAPDATA lpBitmap;
		HBITMAP hBitmap;
		int iHeight = lpbmi->bmiHeader.biHeight;
		int iDir;

		if( iHeight < 0 )
		{	//方向为底到顶
			iDir = 0;
			iHeight = -iHeight;
		}
		else
			iDir = 1;
		//创建位图对象
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
		{	//因为该内存为 GWME 进程创建，其它进程没有对其访问的权，所以需要映射
			//以后将改变该方法，直接从调用者进程分配
			if( lppvBits )
				*lppvBits = MapPtrToProcess( (LPVOID)lpBitmap->bmBits, hgwmeProcess );//GetCurrentProcess() );
			return hBitmap;
		}	
    }
    return NULL;
}

// **************************************************
// 声明：HBITMAP WINAPI WinGdi_CreateDIBitmap(
//								  HDC hdc,                        // handle to DC
//								  CONST BITMAPINFOHEADER *lpbmih, // bitmap data
//								  DWORD fdwInit,                  // initialization option
//								  CONST VOID *lpbInit,            // initialization data
//								  CONST BITMAPINFO *lpbmi,        // color-format data
//								  UINT fuUsage                    // color-data usage
//								  )
// 参数：
//	IN hdc - DC句柄
//	IN lpbmih – BITMAPINFOHEADER结构指针，用于描述需要创建的位图的高度和宽度
//	IN fdwInit – 初始化标志。假如是CBM_INIT，系统用lpbInit 和lpbmi去初始化位图数据
//	IN lpbInit – 包含需要初始化的位图数据，数据格式由lpbmi说明
//	IN lpbmi – BITMAPINFO结构指针，用于描述lpbInit指向的数据格式
//	IN fuUsage - 说明BITMAPINFO结构成员bmiColors的类型：
//			DIB_PAL_COLORS – bmiColors为16索引数组，每一个索引值指向当前hdc的调色板的对应项 
//			DIB_RGB_COLORS – bmiColors为红、绿、蓝颜色数组
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	创建一个设备依赖的位图并从设备不依赖的位图拷贝数据
// 引用: 
//	系统API
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
	//创建位图对象
	hBitmap = WinGdi_CreateCompatibleBitmap( hdc, lpbmih->biWidth, iHeight );
	
	if( hBitmap && fdwInit == CBM_INIT )
	{	//初始化位图图形数据
		if( WinGdi_SetDIBits( hdc, hBitmap, 0, iHeight, lpbInit, lpbmi, fuUsage ) == 0 )
		{	//失败，释放之前创建的对象
			WinGdi_DeleteObject( hBitmap );
			hBitmap = NULL;
		}
	}
	return hBitmap;
}

// **************************************************
// 声明：HFONT WinGdi_CreateFontIndirect(  const LOGFONT *lplf )
// 参数：
// 	IN lplf - LOGFONT 结构指针，包含字体数据
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	创建逻辑字体对象
// 引用: 
//	系统API
// ************************************************

HFONT WINAPI WinGdi_CreateFontIndirect(  const LOGFONT *lplf )
{
#ifdef HAVE_TRUETYPE
	extern const _FONTDRV FAR _TrueTypeDrv;

	_FONT * lpFont;
//	LOGFONT logFont;
	LOGFONT * lpLogfont = NULL;
	//分配字体对象
	lpFont = BlockHeap_Alloc( hgwmeBlockHeap, 0, sizeof(_FONT) );
//	lpFont = BLK_Alloc( 0, sizeof(_FONT) );
	if( lpFont )
	{	//初始化对象数据
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
// 声明：static HPEN _CreatePen( 
//						DWORD dwStyle, 
//						int iWidth, 
//						COLORREF clrColor, 
//						DWORD dwObjFlag, 
//						_LPPENDATA * lppPen )
// 参数：
//	IN dwStyle-笔风格，包含：
//			PS_SOLID-实心线
//			PS_DOT-点线
//			PS_CENTER-中心线
//			PS_DASHED-破折线
//			PS_NULL-笔不可见
//	IN iWidth - 笔宽度(2点以上仅支持PS_SOLID笔风格)
//	IN clrColor - 笔颜色
//	IN dwObjFlag - 对象标志
//	OUT lppPen - 用于接受笔对象指针
// 返回值：
//	假如成功，返回笔对象句柄；否则，返回NULL
// 功能描述：
//	创建笔对象
// 引用: 
//	
// ************************************************
 
static HPEN _CreatePen( DWORD dwStyle, int iWidth, COLORREF clrColor, DWORD dwObjFlag, _LPPENDATA * lppPen )
{
    _LPPENDATA lpPen = (_LPPENDATA)BlockHeap_Alloc( hgwmeBlockHeap, 0, sizeof(_PENDATA) );//malloc( sizeof( _PENDATA ) );
//    _LPPENDATA lpPen = (_LPPENDATA)BLK_Alloc( 0, sizeof(_PENDATA) );//malloc( sizeof( _PENDATA ) );
    if( lpPen )
    {	//初始化对象数据
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
// 声明：HPEN WINAPI WinGdi_CreatePenIndirect(const LOGPEN * lpPenData)
// 参数：
// 	IN lpPenData-指向LOGPEN结构
// 返回值：
//	假如成功，返回笔对象句柄；否则，返回NULL
// 功能描述：
//	创建逻辑笔
// 引用: 
//	系统API
// ************************************************

HPEN WINAPI WinGdi_CreatePenIndirect(const LOGPEN * lpPenData)
{
	return _CreatePen( lpPenData->lognStyle, lpPenData->lognWidth.x, lpPenData->lognColor, 0, NULL );
}

// **************************************************
// 声明：HPEN WINAPI WinGdi_CreatePen( int style, int width, COLORREF color )
// 参数：
//	IN style-笔风格，包含：
//			PS_SOLID-实心线
//			PS_DOT-点线
//			PS_CENTER-中心线
//			PS_DASHED-破折线
//			PS_NULL-笔不可见
//	IN width-笔宽度(2点以上仅支持PS_SOLID笔风格)
//	IN color-笔颜色
// 返回值：
//	假如成功，返回笔对象句柄；否则，返回NULL
// 功能描述：
//	创建逻辑笔
// 引用: 
//	系统API
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
// 声明：static HBRUSH _CreateBrush( 
//								DWORD dwStyle, 
//								COLORREF color,
//								DWORD dwHatch, 
//								DWORD dwObjFlag, 
//								_LPBRUSHDATA * lppBrush )
// 参数：
//	IN dwStyle - 刷子风格
//	IN color - 颜色值
//	IN dwHatch - 阴影线类型，包含
//			HS_HORIZONTAL - 象 ----- 
//			HS_VERTICAL - 象 ||||| 
//			HS_FDIAGONAL - 象 \\\\\ 
//			HS_BDIAGONAL - 象 ///// 
//			HS_CROSS - 象 +++++ 
//			HS_DIAGCROSS - 象 xxxxx 
//	IN dwObjFlag - 对象标志
//	OUT lppBrush - 用于接受刷子对象指针
// 返回值：
//	假如成功，返回刷子对象句柄；否则，返回NULL
// 功能描述：
//	创建刷子
// 引用: 
//	
// ************************************************

static HBRUSH _CreateBrush( DWORD dwStyle, COLORREF color, DWORD dwHatch, DWORD dwObjFlag, _LPBRUSHDATA * lppBrush )
{	//分配对象

	if( dwStyle == BS_SOLID || dwStyle == BS_HATCHED || dwStyle == BS_PATTERN || dwStyle == BS_NULL )
	{
		_LPBRUSHDATA lpBrush = (_LPBRUSHDATA)BlockHeap_Alloc( hgwmeBlockHeap, 0, sizeof(_BRUSHDATA) );//malloc( sizeof( _BRUSHDATA ) );
		//    _LPBRUSHDATA lpBrush = (_LPBRUSHDATA)BLK_Alloc( 0, sizeof(_BRUSHDATA) );//malloc( sizeof( _BRUSHDATA ) );
		
		if( lpBrush )
		{	//初始化数据
			memset( lpBrush, 0, sizeof(*lpBrush) );
			lpBrush->brushAttrib.style = (BYTE)dwStyle;
//			lpBrush->brushAttrib.origin.x = lpBrush->brushAttrib.origin.y = 0;
			lpBrush->brushAttrib.clrRef = color;
			lpBrush->brushAttrib.color = lpDrvDisplayDefault->lpRealizeColor( color, NULL, 0, 0 );
			
			lpBrush->brushAttrib.style = (BYTE)dwStyle;
			if( dwStyle == BS_HATCHED )
			{	//阴影线风格				
				lpBrush->brushAttrib.hatch = (BYTE)(dwHatch % STOCK_BRUSH_NUM);
				if( lpBrush->brushAttrib.hatch != BS_NULL )
					memcpy( lpBrush->brushAttrib.pattern, _pattern[lpBrush->brushAttrib.hatch], 8 );
			}
			else if( dwStyle == BS_SOLID )
			{   //纯色类型 BS_SOLID				
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
			//加入对象管理链表
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
// 声明：HBRUSH WINAPI WinGdi_CreateHatchBrush(int style, COLORREF color)
// 参数：
// 	IN style - 阴影线类型，包含
//			HS_HORIZONTAL - 象 ----- 
//			HS_VERTICAL - 象 ||||| 
//			HS_FDIAGONAL - 象 \\\\\ 
//			HS_BDIAGONAL - 象 ///// 
//			HS_CROSS - 象 +++++ 
//			HS_DIAGCROSS - 象 xxxxx 
// 返回值：
//	假如成功，返回刷子对象句柄；否则，返回NULL
// 功能描述：
//	创建刷子
// 引用: 
//	系统API
// ************************************************

HBRUSH WINAPI WinGdi_CreateHatchBrush(int style, COLORREF color)
{
	return _CreateBrush( BS_HATCHED, color, (DWORD)style, 0, NULL );
}

// **************************************************
// 声明：HBRUSH WINAPI WinGdi_CreateBrushIndirect(const LOGBRUSH *lpBrushData)
// 参数：
// 	IN lpBrushData-指向LOGBRUSH结构指针
// 返回值：
//	假如成功，返回刷子对象句柄；否则，返回NULL
// 功能描述：
//	创建刷子
// 引用: 
//	系统API
// ************************************************

HBRUSH WINAPI WinGdi_CreateBrushIndirect(const LOGBRUSH *lpBrushData)
{

	return _CreateBrush( lpBrushData->lbStyle, lpBrushData->lbColor, (DWORD)lpBrushData->lbHatch, 0, NULL );
}

// **************************************************
// 声明：HBRUSH WINAPI WinGdi_CreateSolidBrush( COLORREF color )
// 参数：
// 	IN color - 刷子颜色
// 返回值：
//	假如成功，返回刷子对象句柄；否则，返回NULL
// 功能描述：
//	创建纯色刷子
// 引用: 
//	系统API
// ************************************************

HBRUSH WINAPI WinGdi_CreateSolidBrush( COLORREF color )
{
	return _CreateBrush( BS_SOLID, color, 0, 0, NULL );
}

// **************************************************
// 声明：HBRUSH _CreateSysBrush( COLORREF color, UINT uiStyle )
// 参数：
// 	IN color - 刷子颜色
//	IN uiStyle - 刷子风格
// 返回值：
//	假如成功，返回刷子对象句柄；否则，返回NULL
// 功能描述：
//	创建系统刷子
// 引用: 
//	
// ************************************************

HBRUSH _CreateSysBrush( COLORREF color, UINT uiStyle )
{
	return _CreateBrush( uiStyle, color, 0, OBJ_FREE_DISABLE, NULL );
}

// **************************************************
// 声明：void _DeleteSysSolidBrush( HBRUSH hBrush )
// 参数：
// 	IN hBrush - 刷子句柄
// 返回值：
//	无
// 功能描述：
//	删除刷子
// 引用: 
//	
// ************************************************

void _DeleteSysSolidBrush( HBRUSH hBrush )
{
	_LPBRUSHDATA lpBrush;

	EnterCriticalSection( &csGdiObjList );

	lpBrush = _GetHBRUSHPtr( hBrush );
	if( lpBrush )
	{	//从对象管理链表移出
		RemoveFromObjMgr( (LPOBJLIST)lpBrush );
		//删除
		BlockHeap_Free( hgwmeBlockHeap, 0, lpBrush, sizeof(_BRUSHDATA) );
//		BLK_Free( 0, lpBrush, sizeof(_BRUSHDATA) );
	}

	LeaveCriticalSection( &csGdiObjList );
}

// **************************************************
// 声明：BOOL _SetSysSolidBrushColor( HBRUSH hBrush , COLORREF color )
// 参数：
// 	IN hBrush - 刷子对象句柄
//	IN color - 颜色
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	设置刷子的颜色
// 引用: 
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
// 声明：HPEN _CreateSysPen( COLORREF color, UINT uiStyle )
// 参数：
// 	IN color - 笔颜色
//	IN uiStyle - 笔风格
// 返回值：
//	假如成功，返回笔对象句柄；否则，返回NULL
// 功能描述：
//	创建系统笔
// 引用: 
//	
// ************************************************

HPEN _CreateSysPen( COLORREF color, UINT uiStyle )
{
	return _CreatePen( uiStyle, 1, color, OBJ_FREE_DISABLE, NULL );
}

// **************************************************
// 声明：HFONT _CreateSysFont( UINT id )
// 参数：
// 	IN id - 字体对象ID
// 返回值：
//	假如成功，返回字体对象句柄；否则，返回NULL
// 功能描述：
//	创建系统字体对象
// 引用: 
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
	//分配字体对象
	lpFont = BlockHeap_Alloc( hgwmeBlockHeap, 0, sizeof(_FONT) );
//	lpFont = BLK_Alloc( 0, sizeof(_FONT) );
	if( lpFont )
	{	//初始化对象数据
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
// 声明：HBITMAP _CreateSysBitmap( 
//							int nWidth, 
//							int nHeight, 
//							UINT cBitsPerPel )
// 参数：
// 	IN nWidth - 位图宽度
//	IN nHeight - 位图高度
//	IN cBitsPerPel - 位图的点格式（每个点的bit数）
// 返回值：
//	假如成功，返回位图句柄；否则，返回NULL
// 功能描述：
//	创建系统位图对象
// 引用: 
//	
// ************************************************

HBITMAP _CreateSysBitmap( int nWidth, 
						  int nHeight, 
						  UINT cBitsPerPel )
{
	return _WinGdi_CreateBitmap( nWidth, nHeight, 1, cBitsPerPel, 0, NULL, NULL, OBJ_FREE_DISABLE, NULL, FALSE );
}

// **************************************************
// 声明：static BOOL _DeletePenObject(HGDIOBJ hgdi)
// 参数：
// 	IN hgdi - 图形对象
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	删除笔对象
// 引用: 
//	
// ************************************************

static BOOL _DeletePenObject(HGDIOBJ hgdi)
{
	_LPPENDATA lpObj = _GetHPENPtr( hgdi );
	//有效并可以释放吗？
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
// 声明：static BOOL _DeleteBrushObject(HGDIOBJ hgdi)
// 参数：
// 	IN hgdi - 图形对象
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	删除刷子对象
// 引用: 
//	
// ************************************************

static BOOL _DeleteBrushObject(HGDIOBJ hgdi)
{
    _LPBRUSHDATA lpObj = _GetHBRUSHPtr( hgdi );
	//有效并可以释放吗？
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
// 声明：static BOOL _DeleteBitmapObject(HGDIOBJ hgdi)
// 参数：
// 	IN hgdi - 图形对象
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	删除位图对象
// 引用: 
//	
// ************************************************

static BOOL _DeleteBitmapObject(HGDIOBJ hgdi)
{
    _LPBITMAPDATA lpObj = _GetHBITMAPPtr( hgdi );
	//有效并可以释放吗？
    if( lpObj && !(lpObj->obj.objType & OBJ_FREE_DISABLE) )
	{	//是
		lpObj->obj.objType = OBJ_NULL;
		if( lpObj->bmFlags & 0x1 )
		{	//位图指针使用 Page_AllocMem 得到的
			Page_FreeMem( lpObj->bmBits, lpObj->bmWidthBytes * lpObj->bmHeight * lpObj->bmPlanes );
		}
		else
		{	//位图指针使用 BlockHeap_Alloc 得到的
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
// 声明：static BOOL _DeleteFontObject(HGDIOBJ hgdi)
// 参数：
// 	IN hgdi - 图形对象
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	删除字体对象
// 引用: 
//	
// ************************************************

static BOOL _DeleteFontObject(HGDIOBJ hgdi)
{
    _LPFONT lpObj = _GetHFONTPtr( hgdi );
	//有效并可以释放吗？
	if( lpObj && !(lpObj->obj.objType & OBJ_FREE_DISABLE) )
	{	//是
		lpObj->obj.objType = OBJ_NULL;
		BlockHeap_Free( hgwmeBlockHeap, 0, lpObj, sizeof(_FONT) );//free( lpObj );
//		BLK_Free( 0, lpObj, sizeof(_FONT) );//free( lpObj );
		return TRUE;
	}
	return FALSE;
}

// **************************************************
// 声明：BOOL WINAPI WinGdi_DeleteObject( HGDIOBJ hgdiobj )
// 参数：
// 	IN hgdi - 图形对象
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	删除图形对象
// 引用: 
//	系统API
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
		//可以释放吗 ？
		if( !(dwType & OBJ_FREE_DISABLE) )
		{	//可以
			switch( (WORD)dwType )
			{
			case OBJ_REGION:			//区域对象
				ObjList_Remove( &_lpGdiObjList, lpvObj );//hgdi );
				bRetv = _DeleteRgnObject( hgdiobj );
				break;
			case OBJ_PEN:				//笔对象
				ObjList_Remove( &_lpGdiObjList, lpvObj );
				bRetv = _DeletePenObject( hgdiobj );
				break;
			case OBJ_BRUSH:				//刷子对象
				ObjList_Remove( &_lpGdiObjList, lpvObj );
				bRetv = _DeleteBrushObject( hgdiobj );
				break;
			case OBJ_BITMAP:			//位图对象
				if( --( ( (LPOBJLIST)lpvObj )->iRefCount ) == 0 )
				{
				    ObjList_Remove( &_lpGdiObjList, lpvObj );
				    bRetv = _DeleteBitmapObject( hgdiobj );
				}
				break;
			case OBJ_FONT:				//字体对象
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
// 声明：BOOL DeleteGdiObject( HANDLE hOwner )
// 参数：
// 	IN hOwner - 对象的拥有者
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	释放该拥有者的所有图形对象
// 引用: 
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
// 声明：_LPPENDATA _GetHPENPtr( HPEN hPen )
// 参数：
// 	IN hPen - 笔对象
// 返回值：
//	假如成功，返回对象指针；否则，返回NULL
// 功能描述：
//	检查对象句柄并得到对象指针
// 引用: 
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
// 声明：_LPBITMAPDATA _GetHBITMAPPtr( HBITMAP hBitmap )
// 参数：
// 	IN hBitmap - 位图对象
// 返回值：
//	假如成功，返回对象指针；否则，返回NULL
// 功能描述：
//	检查对象句柄并得到对象指针
// 引用: 
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
// 声明：_LPBRUSHDATA _GetHBRUSHPtr( HBRUSH hBrush )
// 参数：
// 	IN hBrush - 刷子对象
// 返回值：
//	假如成功，返回对象指针；否则，返回NULL
// 功能描述：
//	检查对象句柄并得到对象指针
// 引用: 
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
// 声明：_LPPALETTE_DATA _GetHPALETTEPtr( HPALETTE hBrush )
// 参数：
// 	IN hBrush - 刷子对象
// 返回值：
//	假如成功，返回对象指针；否则，返回NULL
// 功能描述：
//	检查对象句柄并得到对象指针
// 引用: 
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
// 声明：static BOOL _DeletePaletteObject(HGDIOBJ hgdi)
// 参数：
// 	IN hgdi - 图形对象
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	删除调色板对象
// 引用: 
//	
// ************************************************

static BOOL _DeletePaletteObject(HGDIOBJ hgdi)
{
    _LPPALETTE_DATA lpObj = _GetHPALETTEPtr( hgdi );
	//有效并可以释放吗？
    if( lpObj && !(lpObj->obj.objType & OBJ_FREE_DISABLE) )
	{
		lpObj->obj.objType = OBJ_NULL;
		BlockHeap_FreeString( hgwmeBlockHeap, 0, lpObj );
		return TRUE;
	}
	return FALSE;
}


// **************************************************
// 声明：HPALETTE WinGdi_CreatePalette( CONST LOGPALETTE *lplgpl );
// 参数：
// 	IN lplgpl - 逻辑调色板结构
// 返回值：
//	假如成功，返回对象句柄；否则，返回NULL
// 功能描述：
//	创建调色板对象
// 引用: 
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
// 声明：UINT WinGdi_RealizePalette( HDC hdc );
// 参数：
// 	IN hdc - 设备句柄
// 返回值：
//	假如成功，返回实际映射的调色板项目数；否则，返回 GDI_ERROR
// 功能描述：
//	映射调色板到DC
// 引用: 
//	
// ************************************************

UINT WINAPI WinGdi_RealizePalette( HDC hdc )
{
	WARNMSG( 1, ("warn: the function RealizePalette is'nt support!.\r\n") );
	return GDI_ERROR;
}

// **************************************************
// 声明：UINT WinGdi_GetNearestPaletteIndex( HPALETTE hpal, COLORREF crColor );
// 参数：
// 	IN hpal - 调色板句柄对象
//	IN crColor - RGB颜色
// 返回值：
//	假如成功，返回在调色板对象的项目索引号；否则，返回 CLR_INVALID
// 功能描述：
//	得到RGB颜色在调色板对象中对应的项目索引号
// 引用: 
//	系统API
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
		
		//初始化误差为最大的值
		lMinValue = MAXLONG;
		
		for( like = i = 0; i < num; i++, lp++ )
		{
			if( *((COLORREF*)lp) == color )
				return i;	//最佳匹配
			lpsrc = (const BYTE *)lp;
			lpdst = (const BYTE *)&color;
			//得到差值
			d1 = *lpdst++ - *lpsrc++; 
			d2 = *lpdst++ - *lpsrc++; 
			d3 = *lpdst++ - *lpsrc++;
			lDiff = d1 * d1 + d2 * d2 + d3 * d3;
			//选择误差较小的
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
// 声明：UINT WINAPI WinGdi_GetPaletteEntries(   HPALETTE hpal,
//												UINT iStartIndex,
//												UINT nEntries,
//												LPPALETTEENTRY lppe
//												);
// 参数：
// 	IN hpal - 调色板句柄对象
//	IN iStartIndex - 开始索引号
//	IN nEntries - 需要拷贝的数量
//  IN lppe - 用于接收调色板条目值的内存指针
// 返回值：
//	假如成功，返回并且lppe有效，返回实际拷贝的在调色板对象的条目数；
//	假如成功，返回并且lppe为NULL，返回hpal的z总的条目数；
//  失败，返回 0
// 功能描述：
//	得到指定调色板对象的条目
// 引用: 
//	系统API
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

