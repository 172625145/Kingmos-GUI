/******************************************************
Copyright(c) 版权所有，1998-2005微逻辑。保留所有权利。
******************************************************/


/*****************************************************
文件说明：加载/装入bitmap,icon,cursor等
          LoadImage, LoadCursor, LoadIcon
版本号：3.0.0
开发时期：1999
作者：李林
修改记录：
	2005-05-31, （1） _WinGdi_ConvertImageColorValue 8~32bytes format 没有加源数据的偏移，
	            （2）优化 _WinGdi_ConvertImageColorValue
    2003-07-14, _WinGdi_ConvertColorValue 为 SetDIBitsToDevice 改变调用接口
    2003-07-09 LoadImage 当 cxDesired & cyDesired为0并且没有LR_DEFAULTSIZE 
	           时应为真实的icon & cursor size
    2003-07-04 GetResFileHandle 将\\system\\system.res 改为 从 hgwmeInst得到
    2003-05-06-2003.05.07: LN
	  1. 将直接打开文件改为从hmodule得到文件句柄和res段偏移
	  2. 去掉无用的Code
	  3. 将 GetSystemMetrics 改为 WinSys_GetMetrics
******************************************************/

#include <eframe.h>
#include <efile.h>
#include <eapisrv.h>
#include <gdc.h>

#include <bheap.h>
#include <winsrv.h>
#include <gdisrv.h>
#include <enls.h>

#define ACTUAL_WIDTH (-1)
#define ACTUAL_HEIGHT (-1)
#define IS_SIZE_MATCH( cxActual, cyActual, cxDesired, cyDesired ) ( ( (cxDesired) == ACTUAL_WIDTH || (cxDesired) == (cxActual) ) && ( (cyDesired) == ACTUAL_HEIGHT || (cyDesired) == (cyActual) ) )

extern const _DISPLAYDRV _drvDisplay1BPP;

static LPOBJLIST lpIconObjList = NULL;
static CRITICAL_SECTION csIconObjList;
static LPOBJLIST lpCursorObjList = NULL;
static CRITICAL_SECTION csCursorObjList;
static HANDLE HandleBitmapData( HANDLE hFile, BOOL bShare );
static HANDLE MakeIconCursor( HANDLE hFile, BOOL bIcon, DWORD dwDIBOffset, DWORD dwDIBSize, WORD wName );
static BOOL CALLBACK _FreeIconCursor( LPOBJLIST lpObj, LPARAM lParam );

extern HBITMAP WINAPI WinGdi_CreateBitmap( int nWidth, int nHeight, UINT cPlanes, UINT cBitsPerPel, const VOID *lpvBits);

// **************************************************
// 声明：_LPICONDATA _GetHICONPtr( HICON hIcon )
// 参数：
// 	IN hIcon - ICON句柄对象
// 返回值：
//	假如成功，返回 _ICONDATA 结构指针；否则，返回 NULL
// 功能描述：
//	检查对象句柄并得到对象指针
// 引用: 
//	
// ************************************************
#define DEBUG_GetHICONPtr 0
_LPICONDATA _GetHICONPtr( HICON hIcon )
{
    ASSERT( hIcon && (WORD)GET_OBJ_TYPE( hIcon ) == OBJ_ICON );
    if( hIcon && (WORD)GET_OBJ_TYPE( hIcon ) == OBJ_ICON )
        return (_LPICONDATA)hIcon;
	SetLastError( ERROR_INVALID_PARAMETER );
	WARNMSG( DEBUG_GetHICONPtr, ( "_GetHICONPtr: invalid hIcon(0x%x).\r\n", hIcon ) );
    return NULL;
}

// **************************************************
// 声明：BOOL __InitialGdiIconCursorMgr( void )
// 参数：
// 	无
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	初始化 ICON Cursor 对象管理器
// 引用: 
//	
// ************************************************

BOOL _InitialGdiIconCursorMgr( void )
{
	InitializeCriticalSection( &csIconObjList );
#ifdef __DEBUG
	csIconObjList.lpcsName = "CS-ION";
#endif
	InitializeCriticalSection( &csCursorObjList );
#ifdef __DEBUG
	csCursorObjList.lpcsName = "CS-CUR";
#endif
	return TRUE;
}

// **************************************************
// 声明：void __DeInitialGdiIconCursorMgr( void )
// 参数：
// 	无
// 返回值：
//	无
// 功能描述：
//	与__InitialGdiIconCursorMgr相反，释放分配的资源
// 引用: 
//	
// ************************************************

void _DeInitialGdiIconCursorMgr( void )
{
	DeleteCriticalSection( &csIconObjList );
	DeleteCriticalSection( &csCursorObjList );
}

// **************************************************
// 声明：static HICON _CreateIconIndirect( LPICONINFO lpIconInfo, UINT uiName, BOOL bCopy )
// 参数：
// 	IN lpIconInfo - 包含ICON信息的ICONINFO结构指针
//	IN uiName - ICON id名
//	IN bCopy - 是否重新拷贝 一个 bitmap 句柄数据
// 返回值：
//	假如成功，返回句柄对象；否则，返回NULL
// 功能描述：
//	创建 ICON 对象
// 引用: 
//	
// ************************************************

static HICON _CreateIconIndirect( LPICONINFO lpIconInfo, UINT uiName, BOOL bCopy )
{
	_LPICONDATA lpIcon;

	lpIcon = (_LPICONDATA)BlockHeap_Alloc( hgwmeBlockHeap, 0, sizeof(_ICONDATA) );

	if( lpIcon )
	{		
		_BITMAPDATA * lpBitmap;
		//ICON类型
		lpIcon->fIcon = lpIconInfo->fIcon; // define as ICON, else as CURSOR
		//是否拷贝一个新的
		if( lpIconInfo->hbmColor && bCopy )
		{	//是
			lpBitmap = (_BITMAPDATA *)lpIconInfo->hbmColor;
		    lpIcon->hbmColor = WinGdi_CreateBitmap( lpBitmap->bmWidth, lpBitmap->bmHeight, lpBitmap->bmPlanes, lpBitmap->bmBitsPixel, lpBitmap->bmBits );
		}
		else
			lpIcon->hbmColor = lpIconInfo->hbmColor;
		//是否拷贝一个新的
		if( lpIconInfo->hbmMask && bCopy )
		{	//是
			lpBitmap = (_BITMAPDATA *)lpIconInfo->hbmMask;
		    lpIcon->hbmMask = WinGdi_CreateBitmap( lpBitmap->bmWidth, lpBitmap->bmHeight, lpBitmap->bmPlanes, lpBitmap->bmBitsPixel, lpBitmap->bmBits );
		}
		else
		    lpIcon->hbmMask = lpIconInfo->hbmMask;

		if( lpIcon->hbmMask && lpIcon->hbmColor )
		{	//初始化对象
			lpIcon->xHotspot = lpIconInfo->xHotspot;
			lpIcon->yHotspot = lpIconInfo->yHotspot;
			lpIcon->wIconName = uiName;
			
			if( lpIcon->fIcon )
			{	//加入Icon对象链表
				EnterCriticalSection( &csIconObjList );
				ObjList_Init( &lpIconObjList, &lpIcon->obj, OBJ_ICON, (ULONG)GetCallerProcess() );
				LeaveCriticalSection( &csIconObjList );
			}
			else
			{	//加入Cursor对象链表
				EnterCriticalSection( &csCursorObjList );
				ObjList_Init( &lpCursorObjList, &lpIcon->obj, OBJ_ICON, (ULONG)GetCallerProcess() );
				LeaveCriticalSection( &csCursorObjList );
			}
		}
		else
		{	//初始化对象失败，这里做清除工作
			if( lpIcon->hbmColor )
				WinGdi_DeleteObject( lpIcon->hbmColor );
			if( lpIcon->hbmMask )
				WinGdi_DeleteObject( lpIcon->hbmMask );

			BlockHeap_Free( hgwmeBlockHeap, 0, lpIcon, sizeof(_ICONDATA) );
			return NULL;
		}
	}
	return (HICON)lpIcon;
}

// **************************************************
// 声明：HICON WINAPI WinGdi_CreateIconIndirect( LPICONINFO lpIconInfo )
// 参数：
// 	IN lpIconInfo - 包含 ICON 对象信息的结构
// 返回值：
//	假如成功，返回句柄对象；否则，返回NULL
// 功能描述：
//	创建 ICON 对象
// 引用: 
//	系统API
// ************************************************

HICON WINAPI WinGdi_CreateIconIndirect( LPICONINFO lpIconInfo )
{
	return _CreateIconIndirect( lpIconInfo, -1, 1 );
}

// **************************************************
// 声明：BOOL WINAPI WinGdi_DestroyIcon( HICON hIcon )
// 参数：
// 	hIcon –图标句柄
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	破坏之前创建的图标
// 引用: 
//	系统API
// ************************************************
#define DEBUG_DESTROY_ICON 0
BOOL WINAPI WinGdi_DestroyIcon( HICON hIcon )
{	//由句柄得到对象指针
    _LPICONDATA lpIcon = _GetHICONPtr( hIcon );

	if( lpIcon )
	{
		if( Interlock_Decrement( (LPLONG)&lpIcon->obj.iRefCount ) == 0 )
		{	//当前没有对该对象的引用，释放它
			EnterCriticalSection( &csIconObjList );
			//从对象链表移出
			ObjList_Remove( &lpIconObjList, &lpIcon->obj );

			LeaveCriticalSection( &csIconObjList );
			//释放对象资源
			_FreeIconCursor( &lpIcon->obj, 0 );
		}
		else
		{	//仍然被使用
			WARNMSG( DEBUG_DESTROY_ICON, ( "WinGdi_DestroyIcon: current refcount:%d.\r\n", lpIcon->obj.iRefCount ) );
		}
		return TRUE;
	}
	else
	{
		WARNMSG( DEBUG_DESTROY_ICON, ("WinGdi_DestroyIcon : invalid hicon handle(0x%x).\r\n", hIcon) );
	}
	return FALSE;
}

// **************************************************
// 声明：BOOL WINAPI WinGdi_DestroyCursor( HCURSOR hCursor )
// 参数：
// 	hCursor –光标句柄
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	破坏之前创建的光标
// 引用: 
//	系统API
// ************************************************

BOOL WINAPI WinGdi_DestroyCursor( HCURSOR hCursor )
{	//由句柄得到对象指针
    _LPICONDATA lpIcon = _GetHICONPtr( (HICON)hCursor );

	if( lpIcon )
	{
		if( Interlock_Decrement( (LPLONG)&lpIcon->obj.iRefCount ) == 0 )
		{	//当前没有对该对象的引用，释放它
			EnterCriticalSection( &csCursorObjList );
			//从对象链表移出
			ObjList_Remove( &lpCursorObjList, &lpIcon->obj );
			LeaveCriticalSection( &csCursorObjList );
			//释放对象资源
			_FreeIconCursor( &lpIcon->obj, 0 );
		}
		else
		{	//仍然被使用
			WARNMSG( DEBUG_DESTROY_ICON, ( "WinGdi_DestroyCursor: current refcount:%d.\r\n", lpIcon->obj.iRefCount ) );
		}
		return TRUE;
	}
	else
	{
		WARNMSG( DEBUG_DESTROY_ICON, ("WinGdi_DestroyCursor : invalid hicon handle(0x%x).\r\n", hCursor) );
	}
	return FALSE;
}

// **************************************************
// 声明：static HANDLE _FindIconCursor( 
//								HINSTANCE hInst, 
//								LPOBJLIST * lppList,
//								UINT uiName, 
//								int cxDesired, 
//								int cyDesired )
// 参数：
//	IN hInst - 对象拥有者
//	IN lppList - 需要查找的对象链表
//	IN uiName - 对象名
//	IN cxDesired -	查找要求的对象宽度
//	IN cyDesired - 查找要求的对象高度
// 返回值：
//	假如成功，返回句柄对象；否则，返回NULL
// 功能描述：
//	按条件查找对象
// 引用: 
//	
// ************************************************

#define DEBUG_FIND_ICON_CURSOR 0
static HANDLE _FindIconCursor( HINSTANCE hInst, 
							   LPOBJLIST * lppList,
							   UINT uiName, 
							   int cxDesired, 
							   int cyDesired )
{
	LPOBJLIST lpObj = *lppList;

	DEBUGMSG( DEBUG_FIND_ICON_CURSOR, ( "_FindIconCursor: entry.\r\n" ) );

	while( lpObj )
	{
		_LPICONDATA lpIcon = (_LPICONDATA)lpObj;
		if( lpIcon &&
			lpIcon->hInst == hInst && 
			lpIcon->wIconName == uiName )
		{
			_LPBITMAPDATA lpbmp = _GetHBITMAPPtr( lpIcon->hbmMask );
			//是否符合条件 ？
			if( lpbmp &&
				IS_SIZE_MATCH( lpbmp->bmWidth, lpbmp->bmHeight, cxDesired, cyDesired ) )
			{	//找到
				return (HANDLE)lpIcon;
			}
		}
		//下一个对象
		lpObj = lpObj->lpNext;
	}
	return NULL;
}

// **************************************************
// 声明：static BOOL CALLBACK _FreeIconCursor( LPOBJLIST lpObj, LPARAM lParam )
// 参数：
// 	IN lpObj - 对象链表
//	IN lParam - 枚举对象回调函数所传的附加参数
// 返回值：
//	当需要继续枚举时，返回TRUE；否则，返回FALSE
// 功能描述：
//	枚举对象回调函数入口
// 引用: 
//	
// ************************************************
#define DEBUG_FREE_ICON_CURSOR 0
static BOOL CALLBACK _FreeIconCursor( LPOBJLIST lpObj, LPARAM lParam )
{
	// free mem
	//释放资源
	DeleteObject( ( (_LPICONDATA)lpObj )->hbmColor );
	DeleteObject( ( (_LPICONDATA)lpObj )->hbmMask );
	lpObj->objType = OBJ_NULL;
	BlockHeap_Free( hgwmeBlockHeap, 0, lpObj, sizeof(_ICONDATA) );

	if( lParam )
	{  // print warn message
		WARNMSG( DEBUG_FREE_ICON_CURSOR, ( "_FreeIconCursor:not free icon or cursor handle: 0x%x.\r\n", lpObj ) );
	}
	return TRUE;
}

// **************************************************
// 声明：void FreeInstanceIcon( HANDLE hOwner )
// 参数：
// 	IN hOwner - 对象拥有者
// 返回值：
//	无
// 功能描述：
//	释放拥有者的所有ICON资源（当进程退出时，系统会主动调用该函数）
// 引用: 
//	
// ************************************************

void FreeInstanceIcon( HANDLE hOwner )
{
	EnterCriticalSection( &csIconObjList );
	ObjList_Delete( &lpIconObjList, (ULONG)hOwner, _FreeIconCursor, 1 );
	LeaveCriticalSection( &csIconObjList );
}

// **************************************************
// 声明：void FreeInstanceCursor( HANDLE hOwner )
// 参数：
// 	IN hOwner - 对象拥有者
// 返回值：
//	无
// 功能描述：
//	释放拥有者的所有Cursor资源（当进程退出时，系统会主动调用该函数）
// 引用: 
//	
// ************************************************

void FreeInstanceCursor( HANDLE hOwner )
{
	EnterCriticalSection( &csCursorObjList );
	ObjList_Delete( &lpCursorObjList, (ULONG)hOwner, _FreeIconCursor, 1 );
	LeaveCriticalSection( &csCursorObjList );
}

// **************************************************
// 声明：static int GetColorTableNum( BITMAPINFOHEADER * lpbih )
// 参数：
// 	IN lpbih - 得到颜色表项数
// 返回值：
//	假如成功，返回颜色表项数；否则，返回0
// 功能描述：
//	得到 BITMAPINFOHEADER 结构中包含的颜色表项数
// 引用: 
//	
// ************************************************

static int GetColorTableNum( BITMAPINFOHEADER * lpbih )
{
    int clrTableItemNum = 0;
    if( lpbih->biCompression == BI_BITFIELDS )
        clrTableItemNum = 3;
    switch ( lpbih->biBitCount)
    {
    case 1:		//单色 黑白
        if( lpbih->biClrUsed == 0 )
            clrTableItemNum += 2;
        else
            clrTableItemNum = lpbih->biClrUsed;
        break;
    case 2:		//4色
        if( lpbih->biClrUsed == 0 )
            clrTableItemNum += 4;
        else
            clrTableItemNum = lpbih->biClrUsed;
        break;
    case 4:		//16色
        if( lpbih->biClrUsed == 0 )
            clrTableItemNum += 16;
        else
            clrTableItemNum = lpbih->biClrUsed;
        break;
    case 8:		//256色
        if( lpbih->biClrUsed == 0 )
            clrTableItemNum += 256;
        else
            clrTableItemNum = lpbih->biClrUsed;
        break;
    case 16:		//65536色
        if( lpbih->biCompression == BI_RGB )
			clrTableItemNum = 0; //default
        else if( lpbih->biCompression == BI_BITFIELDS )
			clrTableItemNum = 3; // 3 dword mask of red, green, blue
        break;
    case 24:
        clrTableItemNum = lpbih->biClrUsed;
        break;
    case 32:
        clrTableItemNum += lpbih->biClrUsed;
    }
    return clrTableItemNum;
}

// **************************************************
// 声明：static int GetShiftValue( DWORD dwValue )
// 参数：
// 	IN dwValue - 值
// 返回值：
//	返回最高有效位
// 功能描述：
//	得到数值的最高有效位数，以便于以后的移位操作
// 引用: 
//	
// ************************************************

static int GetShiftValue( DWORD dwValue )
{
    int i;
	for( i = 0; dwValue; i++ )
		dwValue >>= 1;
	return 32-i;
}

// **************************************************
// 声明：static void _ConvertRGBQUADColorRGBColor( BITMAPINFO * lpbi )
// 参数：
// 	IN/OUT lpbi - BITMAPINFO 结构指针
// 返回值：
//	无
// 功能描述：
//	将 lpbi中的 RGBQUAD 颜色表格式转化为 RGB格式
// 引用: 
//	
// ************************************************

static void _ConvertRGBQUADColorRGBColor( BITMAPINFO * lpbi )
{
    RGBQUAD * lpquad = (RGBQUAD *)lpbi->bmiColors;
    COLORREF * lprgb = (COLORREF *)lpbi->bmiColors;
	//得到颜色表项数
    int n = GetColorTableNum( &lpbi->bmiHeader );
    if( lpbi->bmiHeader.biCompression == BI_BITFIELDS )
    {
        n -= 3;
        lpquad += 3;
        lprgb += 3;
    }
    while( n )
    {
        *lprgb = RGB( lpquad->rgbRed, lpquad->rgbGreen, lpquad->rgbBlue );
        lprgb++;
        lpquad++;
        n--;
    }
}

// **************************************************
// 声明：static void _ConvertRGBQUADColorToPalColor( BITMAPINFO * lpbi )
// 参数：
// 	IN/OUT lpbi - BITMAPINFO 结构指针
// 返回值：
//	无
// 功能描述：
//	将 lpbi中的 RGBQUAD 颜色表格式转化为 颜色表索引值
// 引用: 
//	
// ************************************************

static void _ConvertRGBQUADColorToPalColor( BITMAPINFO * lpbi )
{
    RGBQUAD * lpquad = (RGBQUAD *)lpbi->bmiColors;
	INT16 * lpPal = (INT16 *)lpbi->bmiColors;
	//得到颜色表项数
    int n = GetColorTableNum( &lpbi->bmiHeader );
    if( lpbi->bmiHeader.biCompression == BI_BITFIELDS )
    {
        n -= 3;
        lpquad += 3;
        lpPal += 6;
    }
    while( n )
    {	//得到设备相关的值
        *lpPal = (INT16)lpDrvDisplayDefault->lpRealizeColor( RGB( lpquad->rgbRed, lpquad->rgbGreen, lpquad->rgbBlue ), NULL, 0, 0 ); 
        lpPal++;
		lpquad++;
        n--;
    }
}

// **************************************************
// 声明：static void _ConvertMonoBitmap( 
//									_LPBITMAPDATA lpImage, 
//									DWORD dwStartScanLine, 
//									DWORD dwReadScanLine, 
//									DWORD dwScanLineWidth, 
//									LPBYTE lpbData, 
//									BITMAPINFO * lpbi, 
//									BOOL bInvert )
// 参数：
//	IN/OUT lpImage - 目标位图对象,将lpbData数据转化到该对象
//	IN dwStartScanLine - 开始扫描行
//	IN dwReadScanLine - 需要处理的扫描行
//	IN dwScanLineWidth - 扫描行字节数
//	IN lpbData - 包含位图数据的内存
//	IN lpbi - 描述 lpbData 数据信息的结构
//	IN bInvert - 是否反转位图数据
// 返回值：
//	无
// 功能描述：
//	将 lpbData 指向的数据拷贝到 lpImage里
// 引用: 
//	
// ************************************************

static void _ConvertMonoBitmap( _LPBITMAPDATA lpImage, 
                                DWORD dwStartScanLine, 
                                DWORD dwReadScanLine, 
                                DWORD dwScanLineWidth, 
                                LPBYTE lpbData, 
                                BITMAPINFO * lpbi, 
                                BOOL bInvert )
{	//是否相同的格式 ？
    if( lpbi->bmiHeader.biBitCount == 1 && 
        lpImage->bmBitsPixel == 1 )
    {	//是
        DWORD dwDestScanLineBytes = lpImage->bmWidthBytes;
		DWORD dwDestScanLineDwords = dwDestScanLineBytes >> 2;
		int iDestScanLineAddBytes = dwDestScanLineBytes;
        LPBYTE lpDest;

		if( lpbi->bmiHeader.biHeight > 0 )
		{	//位图数据为反向（由低到顶）
			lpDest = lpImage->bmBits + dwDestScanLineBytes * ( lpImage->bmHeight - dwStartScanLine - 1 );
			iDestScanLineAddBytes = -iDestScanLineAddBytes;
		}
		else
		{	//正向
			lpDest = lpImage->bmBits + dwDestScanLineBytes * dwStartScanLine;
		}
		
        for( ; dwReadScanLine; dwReadScanLine--, lpbData += dwScanLineWidth )
        {
			LPBYTE lps, lpd;
			int i = dwDestScanLineDwords;
			int iWriteBytes = 0;
			lpd = lpDest; lps = lpbData;
			//是否反转 ？
			if( bInvert )
			{	//反转  dest = ~source
				while( i-- )
				{
					*(LPDWORD)lpd = ~(*(LPDWORD)lps);
					( (LPDWORD)lpd )++; ( (LPDWORD)lps )++;
					iWriteBytes+=4;
				}
				while( iWriteBytes <  (int)dwDestScanLineBytes )
				{
					*lpd = ~(*lps);
					lpd++; lps++;
					iWriteBytes++;
				}
			}
			else
			{	//直接拷贝
				while( i-- )
				{
					//*(LPDWORD)lpd = ~(*(LPDWORD)lps); //2004-04-07 delete
					*(LPDWORD)lpd = (*(LPDWORD)lps);  //
					//
					( (LPDWORD)lpd )++; ( (LPDWORD)lps )++;
					iWriteBytes+=4;
				}
				while( iWriteBytes < (int)dwDestScanLineBytes )
				{
					*lpd = (*lps);
					lpd++; lps++;
					iWriteBytes++;
				}
			}
			//下一行
            lpDest += iDestScanLineAddBytes;
        }
    }
}

// **************************************************
// 声明：int _WinGdi_ConvertImageColorValue( 
//								    _LPCDISPLAYDRV lpDrv,
//								    _LPBITMAP_DIB lpDestImage,  // dest image data
//									LPCRECT lprcDestClip,
//									CONST BITMAPINFO * lpbi,// src image info
//									LPCRECT lprcSrcClip,
//								    DWORD dwStartScanLine,  // src start scan line
//                                  DWORD dwScanLineNum, 
//									DWORD dwScanLineWidth,
//                                  LPCBYTE lpbData,// src bitmap bits data
//								    int fuColorUse   // use src's RGB or PAL
//									)
// 参数：
//	IN lpDrv - 显示驱动程序接口
//	IN lpDestImage - 用于接受位图数据的目标位图对象
//	IN lprcDestClip - 在目标上的裁剪矩形
//	IN lpbi - 包含源位图信息的结构指针
//	IN lprcSrcClip - 在源位图上的裁剪矩形
//	IN dwStartScanLine - 源位图数据的开始,  // src start scan line
//  IN dwScanLineNum, 
//	IN dwScanLineWidth,
//  IN lpbData,// src bitmap bits data
//	IN fuColorUse   // use src's RGB or PAL
// 返回值：
//	假如成功，返回实际操作的行数；否则，返回0
// 功能描述：
//	将位图由一钟格式转化到另一种格式
// 引用: 
//	
// ************************************************
#define MAX_CACHE_INDEXS   32   //必须是2的次方 2 ^ n
int _WinGdi_ConvertImageColorValue( 
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
									)
{
	DWORD dwReadScanLine;
    _PIXELDATA pixelData;
	LPBYTE lpbDestStart;
	int    iDestWidthBytes;
	int    iSrcDir;
	int    iDestDir;
	int    yDestDir;

	int xDestStart;
	int yDestStart;
	int xDestWidth;

	int xSrcStart;
	int xOffset, yOffset;
	RECT rcSrcClip, rcDestClip; 
	DWORD dwCacheColor[MAX_CACHE_INDEXS];
	DWORD dwCacheDeviceColor[MAX_CACHE_INDEXS];

	//ASSERT( lpDrv );

    //检查目标方向
	if( lpDestImage->bitmap.bmFlags & BF_DIB )
	{ // dest is dib format
		iDestDir = lpDestImage->biDir;
	}
	else
		iDestDir = 1;
	//检查源方向
	if( lpbi->bmiHeader.biHeight < 0 )
		iSrcDir = 1;
	else
		iSrcDir = -1;
	//是否做裁剪工作 ？
	if( lprcDestClip || lprcSrcClip )
	{  // 是do clip 
		//检查源裁剪面的合法性
		//得到源位图矩形
		rcSrcClip.left = 0;
		rcSrcClip.right = lpbi->bmiHeader.biWidth;
		if( iSrcDir > 0 )
		{   // top - >bottom			
			rcSrcClip.top = -lpbi->bmiHeader.biHeight - dwStartScanLine - dwScanLineNum;
		}
		else
		{  // bottom - >top
			rcSrcClip.top = lpbi->bmiHeader.biHeight - dwStartScanLine - dwScanLineNum;
		}
		rcSrcClip.bottom = rcSrcClip.top + dwScanLineNum;

		if( lprcSrcClip )
		{	//源位图矩形与源裁剪矩形相交
		    if( IntersectRect( &rcSrcClip, &rcSrcClip, lprcSrcClip ) == FALSE )
				return 0;
			xOffset = rcSrcClip.left - lprcSrcClip->left;
			yOffset = rcSrcClip.top - lprcSrcClip->top;
        }
		else
		{
			xOffset = 0;
		    yOffset = rcSrcClip.top;
		}
		//检查目标裁剪面的合法性
		if( lprcDestClip )
		{	//将目标设备坐标系转换到源坐标系
		    rcDestClip.left = lprcDestClip->left + xOffset; 
		    rcDestClip.top = lprcDestClip->top + yOffset;
		    rcDestClip.right = rcDestClip.left + (rcSrcClip.right-rcSrcClip.left);
            rcDestClip.bottom = rcDestClip.top + (rcSrcClip.bottom-rcSrcClip.top);
			//得到在源的有效矩形
		    if( IntersectRect( &rcDestClip, &rcDestClip, lprcDestClip ) == FALSE )
				return 0;
		}
		else
		{	//将目标设备坐标系转换到源坐标系
		    rcDestClip.left = xOffset;
		    rcDestClip.top = yOffset;
		    rcDestClip.right = rcDestClip.left + (rcSrcClip.right-rcSrcClip.left);
            rcDestClip.bottom = rcDestClip.top + (rcSrcClip.bottom-rcSrcClip.top);
		}
	}
	else
	{   //目标和源的长宽和原点是相同的 the dest and source's width and height and origin is same
		rcDestClip.left = rcSrcClip.left = 0;
		rcDestClip.right = rcSrcClip.right = lpDestImage->bitmap.bmWidth;
		if( iSrcDir < 0 )
		{
			rcDestClip.top = rcSrcClip.top = lpbi->bmiHeader.biHeight - dwStartScanLine - dwScanLineNum;
		}
		else
		{
			rcDestClip.top = rcSrcClip.top = -lpbi->bmiHeader.biHeight - dwStartScanLine - dwScanLineNum;
		}
	    rcDestClip.bottom = rcSrcClip.bottom = rcDestClip.top + dwScanLineNum;
	}
	//
	dwScanLineNum = rcDestClip.bottom - rcDestClip.top;
	xDestStart = rcDestClip.left;
	xDestWidth = rcDestClip.right - xDestStart;
    xSrcStart = rcSrcClip.left;

	if( iDestDir > 0 )
	{	//目标是正向
		if( iSrcDir < 0 )
		{	//源是反向 b->u
			yDestStart = rcDestClip.bottom-1;//dwScanLineNum;//-1;
			lpbDestStart = lpDestImage->bitmap.bmBits + yDestStart * lpDestImage->bitmap.bmWidthBytes;
		    iDestWidthBytes = -lpDestImage->bitmap.bmWidthBytes;    
			yDestDir = -1; 		
		}
		else
		{	//源是正向
			yDestStart = rcDestClip.top;
			lpbDestStart = lpDestImage->bitmap.bmBits + yDestStart * lpDestImage->bitmap.bmWidthBytes;
		    iDestWidthBytes = lpDestImage->bitmap.bmWidthBytes;    
			yDestDir = 1; 
		}
	}
	else
	{   //目标是反向 // b->u
		if( iSrcDir < 0 )
		{	//源是反向 // b->u
			yDestStart = lpDestImage->bitmap.bmHeight - rcDestClip.bottom;
			lpbDestStart = lpDestImage->bitmap.bmBits + yDestStart * lpDestImage->bitmap.bmWidthBytes;
		    iDestWidthBytes = lpDestImage->bitmap.bmWidthBytes;    
			yDestDir = 1; 
		}
		else
		{	//源是正向
			yDestStart = lpDestImage->bitmap.bmHeight - rcDestClip.top - 1;
			lpbDestStart = lpDestImage->bitmap.bmBits + yDestStart * lpDestImage->bitmap.bmWidthBytes;
		    iDestWidthBytes = -lpDestImage->bitmap.bmWidthBytes;    
			yDestDir = -1; 
		}
	}

	dwReadScanLine = dwScanLineNum;
	//准备点操作结构
	pixelData.lpDestImage = &lpDestImage->bitmap;
    pixelData.pattern = 0;
    pixelData.rop = R2_COPYPEN;

    if( lpbi->bmiHeader.biBitCount == 1 )
    {	//源是单色位图格式
        COLORREF * lprgb = (COLORREF *)lpbi->bmiColors;
		
		if( lpDestImage->bitmap.bmBitsPixel == 1 && 
			lprcDestClip == NULL &&
			lprcSrcClip == NULL )
		{	//目标是单色位图格式
			for( ; dwReadScanLine; dwReadScanLine--, lpbData += dwScanLineWidth  )
			{	//直接拷贝
				memcpy( lpbDestStart, lpbData, dwScanLineWidth );
				lpbDestStart += iDestWidthBytes;
			}
		}
		else
		{   //目标是非单色位图格式
			//用逐个写点的方法
			int xStartBits = 0x80 >> ( xSrcStart & 0x7 ); // 2005-05-31， add
			lpbData += (xSrcStart >> 3); // 2005-05-31， add
			for( ; dwReadScanLine; dwReadScanLine--, yDestStart += yDestDir, lpbData += dwScanLineWidth  )
			{	//每行
				int xe = xDestStart + xDestWidth;
				LPCBYTE lpbColor = lpbData;//2005-05-31, remove[ + (xSrcStart >> 3); ]
				int iBits = xStartBits;// 2005-05-31, remove[0x80 >> ( xSrcStart & 0x7 );]

				pixelData.y = yDestStart;
				pixelData.x = xDestStart;
				for( ; pixelData.x < xe; lpbColor++ )
				{	//每列
					for( ; iBits && pixelData.x < xe; iBits >>= 1, pixelData.x++ )
					{
						if( *lpbColor & iBits )
						{
							pixelData.color = *(lprgb+1);
						}
						else
						{
							pixelData.color = *lprgb;
						}
						lpDrv->lpPutPixel( &pixelData );
					}
					iBits = 0x80;
				}
			}
        }
    }
    else if( lpbi->bmiHeader.biBitCount == 2 )
    {	//源是2bit/pixel位图格式
		INT16 * lpPal = (INT16 *)lpbi->bmiColors;
        RGBQUAD * lpquad = (RGBQUAD *)lpbi->bmiColors;
		UINT cIndex = 0xffffffffl;
		int iStartBits = 0xc0 >> ( ( xSrcStart & 0x3 ) << 1 );	// 2005-05-31， add
		
		lpbData += (xSrcStart >> 2);  // 2005-05-31， add
		//用逐个写点的方法
        for( ; dwReadScanLine; dwReadScanLine--, yDestStart+=yDestDir, lpbData += dwScanLineWidth )
        {	//每行
            LPCBYTE lpbColor = lpbData;//2005-05-31, remove[ + (xSrcStart >> 2);  ]
			int xe = xDestStart + xDestWidth;
			int iBits = iStartBits;// 2005-05-31, remove[0xc0 >> ( ( xSrcStart & 0x3 ) << 1 );]
            
            pixelData.y = yDestStart;//dwStartScanLine;  // dest 
			pixelData.x = xDestStart;
            for( ; pixelData.x < xe; lpbColor++ )
            {	//每列   
                int iShift = 6;
                for( ; iBits && pixelData.x < xe; iBits >>= 2, pixelData.x++, iShift-= 2 )
                {
                    UINT v = (*lpbColor & iBits) >> iShift;
					//如果前一次的颜色值与当前的相同，则不需要
					//再去得到设备相关的颜色值；否则，需要重新计算
					//设备相关的颜色值
					if( cIndex != v )
					{	//得到设备相关的颜色值
						cIndex = v;
						if( fuColorUse == DIB_PAL_COLORS )
							pixelData.color = lpPal[v];
						else
							pixelData.color = lpDrv->lpRealizeColor( RGB( lpquad[v].rgbRed, lpquad[v].rgbGreen, lpquad[v].rgbBlue ), NULL, 0, 0 ); 
					}

                    lpDrv->lpPutPixel( &pixelData );
                }
				iBits = 0xc0;
            }
        }
    }
    else if( lpbi->bmiHeader.biBitCount == 4 )
    {	//源是4bit/pixel位图格式
		INT16 * lpPal = (INT16 *)lpbi->bmiColors;
        RGBQUAD * lpquad = (RGBQUAD *)lpbi->bmiColors;
		UINT cIndex = 0xffffffffl;
		int iStartBits = 0xf0 >> ( ( xSrcStart & 0x1 ) << 2 ); 
		//用逐个写点的方法
		lpbData += (xSrcStart >> 1); //// 2005-05-31， add
        for( ; dwReadScanLine; dwReadScanLine--, yDestStart+=yDestDir, lpbData += dwScanLineWidth )
        {	//每行
            LPCBYTE lpbColor = lpbData;// 2005-05-31， remove[ + (xSrcStart >> 1);]
			int xe = xDestStart + xDestWidth;
			int iBits = iStartBits;//2005-05-31， remove [ 0xf0 >> ( ( xSrcStart & 0x1 ) << 2 );]

            pixelData.y = yDestStart;//dwStartScanLine;
			pixelData.x = xDestStart;
            for( ; pixelData.x < xe; lpbColor++ )
            {	//每列   
                int iShift = 4;
                for( ; iBits && pixelData.x < xe; iBits >>= 4, pixelData.x++, iShift -= 4 )
                {
                    UINT v = (*lpbColor & iBits) >> iShift;
					//如果前一次的颜色值与当前的相同，则不需要
					//再去得到设备相关的颜色值；否则，需要重新计算
					//设备相关的颜色值

					if( cIndex != v )
					{	//得到设备相关的颜色值
						cIndex = v;
						if( fuColorUse == DIB_PAL_COLORS )
							pixelData.color = lpPal[v];//lprgb[v];
						else
							pixelData.color = lpDrv->lpRealizeColor( RGB( lpquad[v].rgbRed, lpquad[v].rgbGreen, lpquad[v].rgbBlue ), NULL, 0, 0 ); 
					}

                    lpDrv->lpPutPixel( &pixelData );
                }
				iBits = 0xf0;
            }
        }
    }
    else if( lpbi->bmiHeader.biBitCount == 8 )
    {	//源是8bit/pixel位图格式
		INT16 * lpPal = (INT16 *)lpbi->bmiColors;
        RGBQUAD * lpquad = (RGBQUAD *)lpbi->bmiColors;
		UINT v = 0xffffffffl;

		lpbData += xSrcStart; // 2005-05-31, add
        for( ; dwReadScanLine; dwReadScanLine--, yDestStart+=yDestDir, lpbData += dwScanLineWidth )
        {	//每行
            LPCBYTE lpbColor = lpbData;
			int xe = xDestStart + xDestWidth;
            
            pixelData.y = yDestStart;//dwStartScanLine;
			pixelData.x = xDestStart;

            for( ; pixelData.x < xe; lpbColor++, pixelData.x++ )
            {	//每列   
				//如果前一次的颜色值与当前的相同，则不需要
				//再去得到设备相关的颜色值；否则，需要重新计算
				//设备相关的颜色值

				// 假如 v == *lpbColor， pixelData.color 的值可以重用
				if( v != *lpbColor )
				{  
					v = *lpbColor;
                    if( fuColorUse == DIB_PAL_COLORS )
					    pixelData.color = lpPal[v];
				    else
					{
					    pixelData.color = lpDrv->lpRealizeColor( RGB( lpquad[v].rgbRed, lpquad[v].rgbGreen, lpquad[v].rgbBlue ), NULL, 0, 0 ); 
					}
				}

                lpDrv->lpPutPixel( &pixelData );
            }
        }
    }
    else if( lpbi->bmiHeader.biBitCount == 16 )
    {	//源是16bit/pixel位图格式
        DWORD dwRedMask, dwGreenMask, dwBlueMask;
		DWORD dwrgb, dwSrc = 0xffffffffl;
        
        int iRedShift, iGreenShift, iBlueShift;
		//包含颜色位模吗 ？
        if( lpbi->bmiHeader.biCompression == BI_RGB )
        {	//默认的位格式 = 5r-5g-5b format
            /*
			dwRedMask = 0x001F;//0x7C00;
            iRedShift = GetShiftValue( dwRedMask );//0;//10;
            dwGreenMask = 0x07e0;//0x03E0;
            iGreenShift = GetShiftValue( dwGreenMask );//5;
            dwBlueMask = 0xf800;//0x001F;
            iBlueShift = GetShiftValue( dwBlueMask );//11;//0;
			*/
			/*
				The bitmap has a maximum of 2^16 colors. 
				If the biCompression member of the BITMAPINFOHEADER is BI_RGB,
				the bmiColors member of BITMAPINFO is NULL. Each WORD in the 
				bitmap array represents a single pixel. The relative 
				intensities of red, green, and blue are represented 
				with five bits for each color component. The value for blue is 
				in the least significant five bits, followed by five bits each 
				for green and red. The most significant bit is not used. 
			*/
			dwRedMask = 0x7C00;
            iRedShift = GetShiftValue( dwRedMask );//0;//10;
            dwGreenMask = 0x03E0;
            iGreenShift = GetShiftValue( dwGreenMask );//5;
            dwBlueMask = 0x001F;
            iBlueShift = GetShiftValue( dwBlueMask );//11;//0;

        }
        else
        {	//非默认的 得到格式
			//得到移位操作值
            dwRedMask = *(LPDWORD)lpbi->bmiColors;
            iRedShift = GetShiftValue(dwRedMask);
            dwGreenMask = *( (LPDWORD)lpbi->bmiColors + 1 );
            iGreenShift = GetShiftValue(dwGreenMask);
            dwBlueMask = *( (LPDWORD)lpbi->bmiColors + 2 );
            iBlueShift = GetShiftValue(dwBlueMask);
        }
		lpbData += xSrcStart << 1; // 2005-05-31, add
        for( ;dwReadScanLine; dwReadScanLine--, yDestStart+=yDestDir, lpbData += dwScanLineWidth )
        {	//每行
            LPWORD lpwData = (LPWORD)lpbData;
			int xe = xDestStart + xDestWidth;

            pixelData.y = yDestStart;//dwStartScanLine;
			pixelData.x = xDestStart;
            for( ; pixelData.x < xe; pixelData.x++ )
            {	//每列   
				//如果前一次的颜色值与当前的相同，则不需要
				//再去得到设备相关的颜色值；否则，需要重新计算
				//设备相关的颜色值
				// 假如 dwSrc == *lpwData， pixelData.color 的值可以重用
				if( dwSrc != *lpwData )
				{
					dwSrc = *lpwData;
					dwrgb = 
						( ( ( dwSrc & dwRedMask ) << iRedShift ) >> 24 ) |
						( ( ( dwSrc & dwGreenMask ) << iGreenShift ) >> 16 ) |
						( ( ( dwSrc & dwBlueMask ) << iBlueShift ) >> 8 );
                    
					pixelData.color = lpDrv->lpRealizeColor( dwrgb, NULL, 0, 0 );
				}
				lpDrv->lpPutPixel( &pixelData );

                lpwData++;
            }
        }
    }
    else if( lpbi->bmiHeader.biBitCount == 24 )
    {	//源是24bit/pixel位图格式
		memset( dwCacheColor, -1, sizeof(dwCacheColor) );
		memset( dwCacheDeviceColor, -1, sizeof(dwCacheDeviceColor) );

		lpbData += (xSrcStart << 1) + xSrcStart; // 2005-05-31, add
        for( ; dwReadScanLine; dwReadScanLine--, yDestStart+=yDestDir, lpbData += dwScanLineWidth )
        {	//每行
            LPCBYTE lpbColor = lpbData;
			int xe = xDestStart + xDestWidth;
			

            pixelData.y = yDestStart;//dwStartScanLine;
			pixelData.x = xDestStart;


            for( ; pixelData.x < xe; lpbColor+=3, pixelData.x++ )
            {	//每列
				COLORREF rgbCurrent = RGB( *(lpbColor+2), *(lpbColor+1), *lpbColor );
				BYTE cacheIndex = *(lpbColor+2) + *(lpbColor+1) + *lpbColor;
				cacheIndex = ( cacheIndex + (cacheIndex >> 4) ) & (MAX_CACHE_INDEXS-1);

				if( rgbCurrent == dwCacheColor[cacheIndex] )
				{   //发现cache
					pixelData.color = dwCacheDeviceColor[cacheIndex];

				}
				else
				{
                    pixelData.color = lpDrv->lpRealizeColor( rgbCurrent, NULL, 0, 0 );
					dwCacheColor[cacheIndex] = rgbCurrent;
					dwCacheDeviceColor[cacheIndex] = pixelData.color;
				}

                lpDrv->lpPutPixel( &pixelData );
            }
        }
    }
    else if( lpbi->bmiHeader.biBitCount == 32 )
    {	//源是32bit/pixel位图格式
		DWORD dwColor = ~( *( (LPDWORD)lpbData ) );
		lpbData += xSrcStart << 2; // 2005-05-31, add

		memset( dwCacheColor, -1, sizeof(dwCacheColor) );
		memset( dwCacheDeviceColor, -1, sizeof(dwCacheDeviceColor) );

        for( ; dwReadScanLine; dwReadScanLine--, yDestStart+=yDestDir, lpbData += dwScanLineWidth )
        {	//每行
            LPBYTE lpbColor = (LPBYTE)lpbData;
			int xe = xDestStart + xDestWidth;

            pixelData.y = yDestStart;//dwStartScanLine;
			pixelData.x = xDestStart;
            for( ; pixelData.x < xe; lpbColor+=4, pixelData.x++ )
            {	//每列 // the color bytes format is b + g + r , LN 2003-09-06
				BYTE cacheIndex = *(lpbColor+2) + *(lpbColor+1) + *lpbColor;				
				cacheIndex = ( cacheIndex + (cacheIndex >> 4) ) & (MAX_CACHE_INDEXS-1);

				if( *( (LPDWORD)lpbColor ) == dwCacheColor[cacheIndex] )
				{   //发现cache
					pixelData.color = dwCacheDeviceColor[cacheIndex];
				}
				else
				{  
					pixelData.color = lpDrv->lpRealizeColor( RGB( *(lpbColor+2), *(lpbColor+1), *lpbColor ), NULL, 0, 0 );
					dwCacheDeviceColor[cacheIndex] = pixelData.color;
					dwCacheColor[cacheIndex] = *( (LPDWORD)lpbColor );
				}

				//if( dwColor != *( (LPDWORD)lpbColor ) )
				//{   // the color bytes format is b + g + r , LN 2003-09-06
				//	dwColor = *( (LPDWORD)lpbColor );                    
				//	pixelData.color = lpDrv->lpRealizeColor( RGB( *(lpbColor+2), *(lpbColor+1), *lpbColor ), NULL, 0, 0 );
				//}
                lpDrv->lpPutPixel( &pixelData );
            }
        }
    }

	return (int)(dwScanLineNum - dwReadScanLine);
}



typedef struct _MYBITMAPFILEHEADER 
{
        WORD    bfDumy;  // to align
        WORD    bfType;
        DWORD   bfSize;
        WORD    bfReserved1;
        WORD    bfReserved2;
        DWORD   bfOffBits;
}MYBITMAPFILEHEADER, * PMYBITMAPFILEHEADER, FAR * LPMYBITMAPFILEHEADER;

// **************************************************
// 声明：static HBITMAP _HandleImageData( HANDLE hFile, BITMAPINFO *lpbi )
// 参数：
// 	IN hFile - 位图文件句柄
//	IN lpbi - 包含位图基本信息的结构
//	IN bShare - 是否共享
// 返回值：
//	假如成功，返回新的位图对象句柄；否则，返回NULL
// 功能描述：
//	处理位图文件，创建新的位图对象
// 引用: 
//	
// ************************************************
#define DEBUG_HANDLE_IMAGE_DATA 0
static HBITMAP _HandleImageData( HANDLE hFile, BITMAPINFO *lpbi, BOOL bShare )
{
    HBITMAP hbmp;
	int iHeight = lpbi->bmiHeader.biHeight;
	_LPBITMAPDATA lpImage;
	_LPCDISPLAYDRV lpDrv;
	//是否反向 ？
	if( iHeight < 0 )
		iHeight = -iHeight;
    //创建一个内存位图对象
	if( lpbi->bmiHeader.biPlanes == 1 && lpbi->bmiHeader.biBitCount == 1 )
		hbmp = _WinGdi_CreateBitmap( lpbi->bmiHeader.biWidth, iHeight, 1, 1, 0, NULL, NULL, 0, NULL, bShare );
	   //WinGdi_CreateBitmap( lpbi->bmiHeader.biWidth, iHeight, 1, 1, 0 );
	else
		hbmp = _WinGdi_CreateBitmap( lpbi->bmiHeader.biWidth, iHeight, lpDisplayBitmap->bmPlanes, lpDisplayBitmap->bmBitsPixel, 0, NULL, NULL, 0, NULL, bShare );
	    //hbmp = WinGdi_CreateBitmap( lpbi->bmiHeader.biWidth, iHeight, lpDisplayBitmap->bmPlanes, lpDisplayBitmap->bmBitsPixel, 0 );
    
    if( hbmp )
    {	//扫描行宽度
		int iScanLineBytes = (lpbi->bmiHeader.biBitCount * lpbi->bmiHeader.biWidth + 31) / 32 * 4;  // align to dword
        DWORD dwTotalSize = iScanLineBytes * iHeight;
        LPBYTE lpbData;
        DWORD dwReadSize;
        DWORD dwReadScanLine;
        int iDir;

		lpImage = _GetHBITMAPPtr( hbmp );
		ASSERT( lpImage );
		//得到该位图格式支持的设备驱动程序
		lpDrv = GetDisplayDeviceDriver( lpImage );
		ASSERT( lpDrv );
		//是否需要太多的临时内存，如果需要太多，则逐行处理
		//否则一次读出所有的数据
        if( dwTotalSize <= 1024 )
        {	//读出所有的数据
            lpbData = (LPBYTE)malloc( dwTotalSize );
            dwReadSize = dwTotalSize;
            dwReadScanLine = lpbi->bmiHeader.biHeight;
        }
        else
        {	//逐行处理
            lpbData = (LPBYTE)malloc( iScanLineBytes );
            dwReadSize = iScanLineBytes;
            dwReadScanLine = 1;
        }
		
        if( lpbData )
        {
            DWORD dwCount;
            DWORD dwStartScanLine;
			DWORD dwRealReadSize;
			//转化调色板            
            _ConvertRGBQUADColorToPalColor( lpbi );

            if( (int)lpbi->bmiHeader.biHeight > 0 )
            {	//反向
				dwStartScanLine = 0;
                iDir = -1;
            }
            else
            {	//正向
				dwStartScanLine = 0;
                iDir = 1;
            }

            for( dwCount = 0;dwCount < dwTotalSize; dwCount += dwReadSize )
            {   //读位图数据    
                ReadFile(hFile, lpbData, dwReadSize,&dwRealReadSize,NULL);
				
                if( dwRealReadSize != dwReadSize )
				{	// error
					WARNMSG( DEBUG_HANDLE_IMAGE_DATA, ( "_HandleImageData: error in _HandleImageData.\r\n" ) );
                    break;
				}
				//转化
				_WinGdi_ConvertImageColorValue( 
					                           lpDrv,
					                           (_LPBITMAP_DIB)lpImage, // dest
					                           NULL, 
											   lpbi,
											   NULL,
											   dwStartScanLine,
											   dwReadScanLine,
											   iScanLineBytes,
											   lpbData,
											   DIB_PAL_COLORS
											   );
				//下几扫描行
                dwStartScanLine += dwReadScanLine;
            }
			//释放临时内存 ？
            free( lpbData );
        }
    }
    return hbmp;
}

// **************************************************
// 声明：static HBITMAP _HandleBitmapFile( HANDLE hFile, BOOL bShare )
// 参数：
// 	IN hFile - 文件句柄
//	IN bShare - 是否共享
// 返回值：
//	假如成功，返回有效的非NULL位图句柄；否则，返回NULL
// 功能描述：
//	根据文件创建一个内存位图对象
// 引用: 
//	
// ************************************************

#define DEBUG_HANDLE_BITMAP_FILE 0
static HBITMAP _HandleBitmapFile( HANDLE hFile, BOOL bShare )
{
    MYBITMAPFILEHEADER bfh;
    DWORD dwReadSize;
    HBITMAP hBitmap = NULL;
    
	RETAILMSG( DEBUG_HANDLE_BITMAP_FILE, ( "_HandleBitmapFile entry.\r\n" ) );
	//对文件头
    ReadFile( hFile, &bfh.bfType, sizeof(MYBITMAPFILEHEADER) - sizeof(WORD), &dwReadSize, NULL );
	//是否位图文件 ？
    if( dwReadSize == (sizeof(MYBITMAPFILEHEADER) - sizeof(WORD)) && 
        *((LPBYTE)&bfh.bfType) == 'B' &&
        *( ((LPBYTE)&bfh.bfType) + 1 ) == 'M' )
    {	//是
		hBitmap = HandleBitmapData( hFile, bShare );
    }
	else
	{	//否
		SetLastError( ERROR_BAD_FORMAT );
		WARNMSG( DEBUG_HANDLE_BITMAP_FILE, ( "_HandleBitmapFile error: invalid bitmap format !" ) );
	}
	RETAILMSG( DEBUG_HANDLE_BITMAP_FILE, ( "_HandleBitmapFile leave.\r\n" ) );
    return hBitmap;
}

typedef struct _ICOCURSORHDR
{
    WORD wReserved;            // always 0 
    WORD wResourceType;
    WORD wResourceCount;       // number of resources in file 
}ICOCURSORHDR;
//某些编译器可能有对齐结构功能
#define ICOCURSORHDR_REAL_SIZE 6

typedef struct _ICOCURSORDESC
{
    BYTE bWidth;               // width of image (icons only ) 
    BYTE bHeight;              // height of image(icons only) 
    BYTE bColorCount;          // number of colors in image 
    BYTE bUnused;              //  
    WORD wHotspotX;            // hotspot x coordinate (CURSORS only) 
    WORD wHotspotY;            // hotspot y coordinate (CURSORS only) 
    DWORD dwDIBSize;             // size of DIB for this image 
    DWORD dwDIBOffset;           // offset to DIB for this image 
} ICOCURSORDESC, FAR * LPICOCURSORDESC;

#define MAXIMAGES               64      // Maximum images in an ico/cur file.

// **************************************************
// 声明：static BOOL IsValidIcoCursorData( 
//									DWORD dwFilePos, 
//                                  LPICOCURSORDESC lpDesc, 
//                                  int nImages, 
//                                  DWORD dwFileSize )

// 参数：
//	IN dwFilePos - ICON 和 CURSOR 数据在文件中的偏移值
//  IN lpDesc - 包含每个ICON 和 CURSOR项目数据的结构
//  IN nImages - ICON 和 CURSOR项目数据结构的个数
//  IN dwFileSize - 文件总大小
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	检查文件中的ICON 和 CURSOR项目数据是否有效
// 引用: 
//	
// ************************************************
#define DEBUG_IsValidIcoCursorData 0
static BOOL IsValidIcoCursorData( DWORD dwFilePos, 
                                  LPICOCURSORDESC lpDesc, 
                                  int nImages, 
                                  DWORD dwFileSize )
{
    int i;
    for (i = 0; i < nImages; i++) 
    {
        if (lpDesc->dwDIBOffset != dwFilePos ||
            dwFilePos + lpDesc->dwDIBSize > dwFileSize )
        {
			WARNMSG( DEBUG_IsValidIcoCursorData, ( "IsValidIcoCursorData: error i(%d),lpDesc->dwDIBOffset(%d),dwFilePos(%d),lpDesc->dwDIBSize(%d),dwFileSize(%d).\r\n",i,lpDesc->dwDIBOffset,dwFilePos,lpDesc->dwDIBSize,dwFileSize ) );
            return FALSE;
        }        
        dwFilePos += lpDesc->dwDIBSize;
        lpDesc++;
    }
    return TRUE;
}

// **************************************************
// 声明：static int GetNeedImageIndex(
//                             HANDLE hFile,
//                             LPICOCURSORDESC lpDesc,
//                             int nImages, 
//                             int cxDesired, 
//                             int cyDesired )
// 参数：
//  IN hFile - 位图文件句柄
//  IN lpDesc - 包含每个ICON 和 CURSOR项目数据的结构
//  IN nImages - 需要查找的文件名
//  IN cxDesired - 要求的宽度
//  IN cyDesired - 要求的高度
// 返回值：
//	假如成功，返回匹配的项目索引号；否则，返回-1
// 功能描述：
//	得到符合要求的项目在文件里的索引号/位置
// 引用: 
//	
// ************************************************

static int GetNeedImageIndex(
                             HANDLE hFile,
                             LPICOCURSORDESC lpDesc,                              
                             int nImages, 
                             int cxDesired, 
                             int cyDesired )
{
    int i; 
    BITMAPINFO bi;

    if( cxDesired == 0 || cyDesired == 0 )
        return 0;
    for (i = 0; i < nImages; i++) 
    {   
        DWORD dwReaded;
        SetFilePointer( hFile, lpDesc->dwDIBOffset, NULL, FILE_BEGIN );
        ReadFile( hFile, &bi, sizeof( bi ), &dwReaded, NULL );

		if( IS_SIZE_MATCH( bi.bmiHeader.biWidth, bi.bmiHeader.biHeight / 2, cxDesired, cyDesired ) )
        {	//符合要求
            return i;
        }        
        lpDesc++;	//下一个项目
    }
    return -1;
}

// **************************************************
// 声明：HANDLE IconCursorCreate(
//                int xHot, 
//				  int yHot,
//                BITMAPINFO * lpbi,
//                int uiType,
//				  int uiName )

// 参数：
//  IN xHot - 光标的热点
//	IN yHot - 光标的热点
//  IN lpbi - 位图信息
//  IN uiType - 位图类型：
//			IMAGE_ICON - 图标
//			IMAGE_CURSOR - 光标
//	IN uiName - 对象ID
// 返回值：
//	假如成功，返回创建的位图句柄；否则，返回NULL
// 功能描述：
//	创建图标或光标资源
// 引用: 
//	
// ************************************************

HANDLE IconCursorCreate(
                int xHot, 
				int yHot,
                BITMAPINFO * lpbi,
                int uiType,
				int uiName )
{
    int iBitCount;
    int cx;
    int cy;
    DWORD cbColorTable;
    DWORD cbBits;
    LPBYTE lpDIBBits;
    HICON hIcon = NULL;
    ICONINFO icon;
    DWORD dwStartScanLine;
    int iDir;
    int iScanLineBytes;
    HBITMAP hbmp;
    HBITMAP hbmpMono;

    lpbi->bmiHeader.biHeight /= 2;
	dwStartScanLine = 0;
    if( (int)lpbi->bmiHeader.biHeight > 0 )
    {	//反向
        cy = lpbi->bmiHeader.biHeight;
        iDir = -1;
    }
    else
    {	//正向
        cy = -(int)lpbi->bmiHeader.biHeight;
        iDir = 1;
    }
	//创建彩色位图
    if( lpbi->bmiHeader.biBitCount != 1 )
        hbmp = WinGdi_CreateBitmap( lpbi->bmiHeader.biWidth,  cy, lpDisplayBitmap->bmPlanes, lpDisplayBitmap->bmBitsPixel, 0 );
    else
        hbmp = WinGdi_CreateBitmap( lpbi->bmiHeader.biWidth,  cy, 1, 1, 0 );
	//创建黑白位模位图
    hbmpMono = WinGdi_CreateBitmap( lpbi->bmiHeader.biWidth, cy, 1, 1, 0 );
    
    if( hbmp && hbmpMono )
    {
        iBitCount = lpbi->bmiHeader.biBitCount;    
        cx = (int)lpbi->bmiHeader.biWidth;
        
        cbColorTable = GetColorTableNum( &lpbi->bmiHeader ) * sizeof(RGBQUAD);
        //指向位图数据
        lpDIBBits = (LPBYTE)lpbi + sizeof(BITMAPINFOHEADER) + cbColorTable;
        //对齐到DWORD
		iScanLineBytes = (cx * iBitCount + 31 ) / 32 * 4;  // align to dword
        cbBits = iScanLineBytes * cy;
		//转换ICON/CURSOR的颜色部分
        if( lpbi->bmiHeader.biBitCount != 1 )
        {
			_LPBITMAPDATA lpImage = _GetHBITMAPPtr(hbmp);
			//转换颜色表从RQBQUAD格式到PAL格式
            _ConvertRGBQUADColorToPalColor( lpbi );
			//转换位图数据到目标
			_WinGdi_ConvertImageColorValue(
				                           GetDisplayDeviceDriver( lpImage ),
				                           (_LPBITMAP_DIB)lpImage,
				                           NULL,
										   lpbi,
										   NULL,
										   dwStartScanLine, 
										   cy, 
										   iScanLineBytes, 
										   lpDIBBits, 
										   DIB_PAL_COLORS
										   );
        }
        else
		{	//转换位图数据到目标
            _ConvertMonoBitmap( 
			                   _GetHBITMAPPtr(hbmp), 
							   dwStartScanLine, 
							   cy, 
							   iScanLineBytes, 
							   lpDIBBits, 
							   lpbi, 
							   0 );
		}
		//指向位模部分        
		//位模是紧跟着颜色部分
        lpDIBBits += cbBits;
        
        lpbi->bmiHeader.biBitCount = 1;
		//对齐到DWORD
		iScanLineBytes = (cx * 1 + 31) / 32 * 4;  //align to dword
		//转换ICON/CURSOR的位模部分
        _ConvertMonoBitmap( 
			               _GetHBITMAPPtr(hbmpMono), 
						   dwStartScanLine, 
						   cy, 
						   iScanLineBytes,
						   lpDIBBits,
						   lpbi,
						   1 );
        icon.fIcon = uiType;
        icon.hbmColor = hbmp;
        icon.hbmMask = hbmpMono;
        
        icon.xHotspot = xHot;
        icon.yHotspot = yHot;
		//创建ICON对象        
        hIcon = _CreateIconIndirect( &icon, uiName, 0 );
        if( hIcon == NULL )
        {	//失败，清除
            DeleteObject( hbmp );
            DeleteObject( hbmpMono );
        }
    }
    else
    {	//失败，清除
        if( hbmp )
            WinGdi_DeleteObject( hbmp );
        if( hbmp )
            WinGdi_DeleteObject( hbmpMono );
    }
    return hIcon;
}

// **************************************************
// 声明：static ICOCURSORDESC * AllocDescAndCheck( 
//									HANDLE hFile, 
//									int iResourceCount, 
//									DWORD dwFileSize )
// 参数：
//	IN hFile - 文件句柄
//	IN iResourceCount - 需要分配的资源数
//	IN dwFileSize - 文件的大小
// 返回值：
//	假如成功，返回分配/并初始化好的ICOCURSORDESC结构；否则，返回NULL
// 功能描述：
//	分配需要的的ICOCURSORDESC结构
//	从文件里读出ICOCURSORDESC结构数据
// 引用: 
//	
// ************************************************
#define DEBUG_AllocDescAndCheck 0
static ICOCURSORDESC * AllocDescAndCheck( HANDLE hFile, int iResourceCount, DWORD dwFileSize )
{
    DWORD dwReaded;
    UINT uiDescSize = sizeof(ICOCURSORDESC) * iResourceCount; 
    ICOCURSORDESC *lpDesc = (ICOCURSORDESC *)malloc( uiDescSize );

	DEBUGMSG( DEBUG_AllocDescAndCheck, ( "AllocDescAndCheck:  entry.\r\n" ) );
    if( lpDesc )
    {	//读需要的  ICOCURSORDESC 结构数据
        if( ReadFile( hFile, lpDesc, uiDescSize, &dwReaded, NULL ) )
        {	//检查是否有效
            if( dwReaded == uiDescSize )
            {	// check valid ?
                DWORD dwFilePos = (DWORD)SetFilePointer( hFile, 0, NULL, FILE_CURRENT );
				//检查是否有效
                if( IsValidIcoCursorData( dwFilePos, lpDesc, iResourceCount, dwFileSize ) )
                    return lpDesc;
				else
				{
					WARNMSG( DEBUG_AllocDescAndCheck, ( "AllocDescAndCheck:  invalid data.\r\n" ) );
				}
            }
			else
			{
				WARNMSG( DEBUG_AllocDescAndCheck, ( "AllocDescAndCheck:  dwReaded(%d) != uiDescSize(%d).\r\n", dwReaded, uiDescSize ) );
			}
        }
		else
		{
			WARNMSG( DEBUG_AllocDescAndCheck, ( "AllocDescAndCheck:  can't read file.\r\n" ) );
		}
		free( lpDesc );
		return NULL;
    }
	else
	{
		WARNMSG( DEBUG_AllocDescAndCheck, ( "AllocDescAndCheck:  can't alloc memory.\r\n" ) );
	}
    return NULL;
}

// **************************************************
// 声明：void FreeDesc( ICOCURSORDESC *lpDesc )
// 参数：
// 	IN lpDesc - ICOCURSORDESC 结构指针
// 返回值：
//	无
// 功能描述：
//	与 AllocDescAndCheck 相反，释放 ICOCURSORDESC 结构内存
// 引用: 
//	
// ************************************************

void FreeDesc( ICOCURSORDESC *lpDesc )
{
    free( lpDesc );
}

// **************************************************
// 声明：static HANDLE _HandleIconCursorFile( 
//								HANDLE hFile, 
//								UINT uType, 
//								int cxDesired, 
//								int cyDesired, 
//								int index, 
//								BOOL bLoadWithSize )
// 参数：
//	IN hFile - 文件句柄
//	IN uType - 需要的 类型，为以下值：
//					IMAGE_CURSOR - 指光标
//					IMAGE_ICON   - 指图标
//	IN cxDesired - 需要的宽度（当bLoadWithSize为TRUE时，有效）
//	IN cyDesired - 需要的高度（当bLoadWithSize为TRUE时，有效）
//	IN index - 在文件里的索引号（当bLoadWithSize为TRUE时，无效）
//	IN bLoadWithSize - 说明是否需要根据cxDesired和cyDesired来得到资源
// 返回值：
//	假如成功，返回资源的句柄；否则，返回NULL
// 功能描述：
//	从文件得到符合要求的ICON或CURSOR资源
// 引用: 
//	
// ************************************************

#define DEBUG_HandleIconCursorFile 0
static HANDLE _HandleIconCursorFile( HANDLE hFile, UINT uType, int cxDesired, int cyDesired, int index, BOOL bLoadWithSize )
{
    ICOCURSORHDR hdr;
    LPICOCURSORDESC lpDesc;
    DWORD dwReaded;
    DWORD dwFileSize;
    HANDLE handle = NULL;
	//得到文件大小

	DEBUGMSG( DEBUG_HandleIconCursorFile, ( "_HandleIconCursorFile:  entry.\r\n" ) );

    dwFileSize = SetFilePointer( hFile, 0, NULL, FILE_END );
    SetFilePointer( hFile, 0, NULL, FILE_BEGIN );

    if( ReadFile( hFile, &hdr, ICOCURSORHDR_REAL_SIZE, &dwReaded, NULL ) )
    {   // 是否有效 check valid ?
        if( dwReaded == ICOCURSORHDR_REAL_SIZE && 
            hdr.wReserved == 0 &&
            hdr.wResourceCount < MAXIMAGES &&
            (hdr.wResourceType == 1 || hdr.wResourceType == 2)
          )
        {	//分配并初始化需要的 ICOCURSORDESC 结构
            lpDesc = AllocDescAndCheck( hFile, hdr.wResourceCount, dwFileSize );
            if( lpDesc )
            {	//如果按大小得到资源，则查找一个符合要求的
                if( bLoadWithSize )
                   index = GetNeedImageIndex( hFile, lpDesc, hdr.wResourceCount, cxDesired, cyDesired );
                if( index >= 0 )
                {	//创建
					handle = MakeIconCursor( hFile, 
						                     uType == IMAGE_ICON, 
											 lpDesc[index].dwDIBOffset, 
											 lpDesc[index].dwDIBSize, 
											 (WORD)(bLoadWithSize ? -1 : index) );
                }
				else
				{
					WARNMSG( DEBUG_HandleIconCursorFile, ( "_HandleIconCursorFile: error index(%d)!.\r\n", index ) );
				}
                FreeDesc( lpDesc );
            }
			else
			{
				WARNMSG( DEBUG_HandleIconCursorFile, ( "_HandleIconCursorFile: AllocDescAndCheck failure.!.\r\n" ) );
			}
        }
    }
	else
	{
		WARNMSG( DEBUG_HandleIconCursorFile, ( "_HandleIconCursorFile: can't read file.!.\r\n" ) );
	}
    return handle;
}

// **************************************************
// 声明：static HANDLE MakeIconCursor( 
//							 HANDLE hFile, 
//							 BOOL bIcon, 
//							 DWORD dwDIBOffset, 
//							 DWORD dwDIBSize, 
//							 WORD wName )
// 参数：
//	IN hFile - 资源文件句柄
//	IN bIcon - 是否是创建ICON,如果为FALSE,表示是CURSOR
//	IN dwDIBOffset - DIB数据在文件中的便移 
//	IN dwDIBSize - DIB数据在文件中的大小
//	IN wName - 资源在文件中的ID
// 返回值：
//	假如成功，返回有效的非NULL资源句柄；否则，返回NULL
// 功能描述：
//	从文件中读出需要的资源，并创建资源对象，返回句柄
// 引用: 
//	
// ************************************************

static HANDLE MakeIconCursor( 
							 HANDLE hFile, 
							 BOOL bIcon, 
							 DWORD dwDIBOffset, 
							 DWORD dwDIBSize, 
							 WORD wName )
{
	BITMAPINFO *lpbi;
	HANDLE handle = NULL;
	SetFilePointer( hFile, dwDIBOffset, NULL, FILE_BEGIN );
	if( (lpbi = malloc( dwDIBSize )) )
	{
		DWORD dwReaded;
		ReadFile( hFile, lpbi, dwDIBSize, &dwReaded, NULL );
		handle = IconCursorCreate( 0, 0, lpbi, 
			                       bIcon,
								   wName );
		free( lpbi );
	}
	return handle;
}

//文件中的定义
//描述各种资源的结构
typedef struct _RESHEADER{
	DWORD dwDataSize;     // size of data without header 
	DWORD dwHeaderSize;   // length of the header 
	WORD  wType[2];        // type identifier, id or string , if is string, it's various length and null ending.
	WORD  wName[2];        // type identifier, id or string , if is string, it's various length and null ending.
	DWORD dwDataVersion;
	WORD  wMemFlag;
	WORD  wLanguageID;
	DWORD dwVersion;
	DWORD dwCharacteristics;
}RESHEADER, FAR * LPRESHEADER;
//描述ICON资源的头结构
typedef struct _ICONHEADER
{
	WORD   wReserved;          // Currently zero 
	WORD   wType;              // 1 for icons 
	WORD   wCount;             // Number of components 
	WORD   wDumy;            // filler for DWORD alignment 
}ICONHEADER, FAR * LPICONHEADER;
//描述每个ICON资源的结构
typedef struct _ICONREC
{
	BYTE   bWidth; 
	BYTE   bHeight; 
	BYTE   bColorCount; 
	BYTE   bReserved; 
	WORD   wPlanes; 
	WORD   wBitCount; 
	DWORD  lBytesInRes; 
	WORD   wNameOrdinal;       // Points to component 
	WORD   wDumy;            // filler for DWORD alignment 
}ICONREC, FAR * LPICONREC; 
//描述每个ICON资源的索引结构
typedef struct _ICONINDEX
{	
	DWORD dwName;		//ICON名
	DWORD dwOffsetInFile;		//在文件中的便移
}ICONINDEX, FAR * LPICONINDEX;

// **************************************************
// 声明：static HANDLE HandleIconCursorGroupData( 
//						HANDLE hFile, 
//						int id, 
//						LPCTSTR lpszName, 
//						LPICONINDEX lp, 
//						int iIndexCount, 
//						int cxDesired, 
//						int cyDesired )
// 参数：
//	IN hFile - 文件句柄
//	IN id - 在文件中的资源类型ID
//	IN lpszName- - 资源名
//	IN lp - ICONINDEX 结构指针
//	IN iIndexCount - ICONINDEX 结构指针所指向的索引数
//	IN cxDesired - 需要的宽度
//	IN cyDesired - 需要的高度
// 返回值：
//	假如成功，返回有效的非NULL资源句柄；否则，返回NULL
// 功能描述：
//	处理ICON/CURSOR组数据
// 引用: 
//	
// ************************************************

static HANDLE HandleIconCursorGroupData( HANDLE hFile, int id, LPCTSTR lpszName, LPICONINDEX lp, int iIndexCount, int cxDesired, int cyDesired )
{ 
	ICONHEADER ih;
	DWORD dwReaded;
	//对头数据
	ReadFile( hFile, &ih, sizeof(ih) - sizeof(WORD), &dwReaded, NULL ); 
	if( ih.wCount > 0 && iIndexCount )
	{
		ICONREC iconRec;
		int i;
		for( i = 0; i < ih.wCount; i++ )
		{	//读每一组的数据
			ReadFile( hFile, &iconRec, sizeof(iconRec) - sizeof(WORD), &dwReaded, NULL ); 
			//是否符合要求
			if( ( dwReaded == sizeof(iconRec) - sizeof(WORD) ) &&
				IS_SIZE_MATCH( iconRec.bWidth, iconRec.bHeight, cxDesired, cyDesired ) )
			{
				int n;
				//在该组中查找名字匹配的资源
				for( n  = 0; n < iIndexCount; n++ )
				{
					if( lp->dwName == iconRec.wNameOrdinal )
						return MakeIconCursor( hFile, 
						                       (BOOL)( (WORD)id == (DWORD)RT_ICON ),
						                       lp->dwOffsetInFile,
						                       iconRec.lBytesInRes,
						                       (WORD)((DWORD)lpszName) );
					lp++;
				}
			}
		}
	}
	return NULL;
}

// **************************************************
// 声明：static HANDLE HandleBitmapData( HANDLE hFile, BOOL bShare )
// 参数：
//	IN hFile - 文件句柄
//	IN bShare - 是否共享
// 返回值：
//	假如成功，返回有效的非NULL资源句柄；否则，返回NULL
// 功能描述：
//	处理位图文件
// 引用: 
//	
// ************************************************
#define DEBUG_HANDLE_BITMAP_DATA 0
static HANDLE HandleBitmapData( HANDLE hFile, BOOL bShare )
{
	BITMAPINFO *lpbi;
	DWORD dwReadSize;
	HBITMAP hBitmap = NULL;
	
	DEBUGMSG( DEBUG_HANDLE_BITMAP_DATA, ( "HandleBitmapData: entry.\r\n" ) );

	lpbi = (BITMAPINFO *)malloc( sizeof(BITMAPINFO) );
	if( lpbi )
	{
		DEBUGMSG( DEBUG_HANDLE_BITMAP_DATA, ( "HandleBitmapData: read header.\r\n" ) );
		ReadFile( hFile, &lpbi->bmiHeader, sizeof(lpbi->bmiHeader), &dwReadSize, NULL );
		if( dwReadSize == sizeof(lpbi->bmiHeader) &&
			lpbi->bmiHeader.biSize == sizeof(lpbi->bmiHeader) )
		{
			if( lpbi->bmiHeader.biPlanes == 1 &&
				( lpbi->bmiHeader.biCompression == BI_RGB ||
				lpbi->bmiHeader.biCompression == BI_BITFIELDS ) )
			{   // support 1 planes format
				int items = GetColorTableNum( &lpbi->bmiHeader );
				
				if( items > 1 )
				{
					LPVOID p;
					p = realloc( lpbi, (items - 1)* sizeof(RGBQUAD) + sizeof(BITMAPINFO)  );
					if( p  )
					{
						lpbi = (BITMAPINFO *)p;
						ReadFile(hFile, lpbi->bmiColors, items* sizeof(RGBQUAD), &dwReadSize, NULL );
						if( items * sizeof(RGBQUAD) == dwReadSize )
						{
							hBitmap = _HandleImageData( hFile, lpbi, bShare );
						}
					}
				}
				else
					hBitmap = _HandleImageData( hFile, lpbi, bShare );
			}
		}
		else
		{
			RETAILMSG( 1, ( "error: invalid bitmap format .\r\n" ) );
		}
		free( lpbi );
	}
	return hBitmap;
}

// **************************************************
// 声明：
// 参数：
// 	无
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	
// 引用: 
//	
// ************************************************

BOOL IsIconCursorSizeMatch( HANDLE hFile, UINT uiTypeint, int cxDesired, int cyDesired )
{
	BITMAPINFO bi;
	DWORD dwReaded;
	DWORD dwOffset; 

	dwOffset = SetFilePointer( hFile, 0, NULL, FILE_CURRENT );
	ReadFile( hFile, &bi, sizeof(bi), &dwReaded, NULL );
	if( dwReaded == sizeof(bi) )
	{
		if( IS_SIZE_MATCH( bi.bmiHeader.biWidth, bi.bmiHeader.biHeight / 2, cxDesired, cyDesired ) )
		{
			SetFilePointer( hFile, dwOffset, NULL, FILE_BEGIN );
			return TRUE;
		}
	}
	SetFilePointer( hFile, dwOffset, NULL, FILE_BEGIN );
	return FALSE;
}

// **************************************************
// 声明：static HANDLE _LoadImageFromResFile( 
//									HANDLE hFile, 
//									DWORD dwOffset, 
//									UINT id, 
//									LPCTSTR lpszName, 
//									int cxDesired, 
//									int cyDesired,
//									BOOL bShare )
// 参数：
//	IN hFile - 文件句柄
//	IN dwOffset - 资源在文件中的偏移
//	IN id - 需要的资源类别
//	IN lpszName - 资源名字
//	IN cxDesired - 需要的宽度
//	IN cyDesired - 需要的高度
//	IN bShare - 该资源是否会被共享
// 返回值：
//	假如成功，返回 资源句柄；否则，返回 NULL
// 功能描述：
//	从文件得到资源
// 引用: 
//	
// ************************************************

#define DEBUG_LoadImageFromResFile 0
static HANDLE _LoadImageFromResFile( HANDLE hFile, DWORD dwOffset, UINT id, LPCTSTR lpszName, int cxDesired, int cyDesired, BOOL bShare )
{
	int iIndexSize = 8;
	int iIndexCount = 0;
	HANDLE hImage = NULL;
	LPICONINDEX lpIndex, lpIndexHeader;

	lpIndexHeader = (LPICONINDEX)malloc( sizeof( ICONINDEX ) * iIndexSize );

	if( lpIndexHeader )
	{			
		RESHEADER rh;
		DWORD dwReaded;
		int i;
		int iFilePos = 0;
		
		lpIndex  = lpIndexHeader;
		
		iFilePos = SetFilePointer( hFile, dwOffset, NULL, FILE_BEGIN );

		while(1)
		{
			//RETAILMSG( 1, ( "i0.\r\n" ) );
			ReadFile( hFile, &rh, sizeof(rh), &dwReaded, NULL );
			//RETAILMSG( 1, ( "i1.\r\n" ) );
			if( dwReaded != sizeof(rh) )
			{
				DEBUGMSG( DEBUG_LoadImageFromResFile, ( "_LoadImageFromResFile:file format error.\r\n" ) );
				break;
			}
			//RETAILMSG( 1, ( "_LoadImageFromResFile:rh.wType[0]=%d,rh.wType[1]=%d.\r\n", rh.wType[0],rh.wType[1] ) );
			if( rh.wType[0] == 0xffff && 
				( rh.wType[1] == id || rh.wType[1] == (id+DIFFERENCE) ) )
			{   // find it
				
				if( id == (DWORD)RT_ICON || id == (DWORD)RT_CURSOR )
				{ 	// save the component index data  when the id is cursor or icon
					//RETAILMSG( 1, ( "Find :rh.wType[0]=%d,rh.wType[1]=%d.\r\n", rh.wType[0],rh.wType[1] ) );
					if( rh.wType[1] == id )
					{
						if( iIndexCount < iIndexSize )
						{
							lpIndex->dwName = rh.wName[1];
							lpIndex->dwOffsetInFile = SetFilePointer( hFile, 0, NULL, FILE_CURRENT );
							lpIndex++; iIndexCount++;
						}
						else
						{
							ASSERT( 0 );
							break;
						}
					}
					if( rh.wName[1] == (DWORD)lpszName )
					{
						if( rh.wType[1] == (id+DIFFERENCE) )
						{
							//RETAILMSG( 1, ( "loadimag0:name=%d.\r\n", lpszName ) );
						    hImage = HandleIconCursorGroupData( hFile, id, lpszName, lpIndexHeader, iIndexCount, cxDesired, cyDesired );
						}
						else
						{  // id self
							//RETAILMSG( 1, ( "loadimag1:name=%d.\r\n", lpszName ) );
							hImage = MakeIconCursor( hFile, id == (DWORD)RT_ICON, (lpIndex - 1)->dwOffsetInFile, rh.dwDataSize, (WORD)( (DWORD)lpszName ) );
						}
						break;
					}
					else if( lpszName == 0 && id == rh.wType[1] )
					{  // match the size
						if( IsIconCursorSizeMatch( hFile, id, cxDesired, cyDesired  ) )
						{
							hImage = MakeIconCursor( hFile, id == (DWORD)RT_ICON, (lpIndex - 1)->dwOffsetInFile, rh.dwDataSize, (WORD)( (DWORD)lpszName ) );
							break;
						}
					}
					if( rh.wType[1] == (id+DIFFERENCE) )
					{
						lpIndex = lpIndexHeader;
						iIndexCount = 0;
					}
				}
				else if( (char*)id == RT_BITMAP && 
					     rh.wName[1] == (DWORD)lpszName )
				{
					hImage = HandleBitmapData( hFile, bShare ); 
					break;
				}
			}

			if( ( i = rh.dwDataSize + rh.dwHeaderSize ) < 0 )
			{
				ASSERT( 0 );
				break;			
			}
			// search next resource
			i = (i + 3) & (~3); // align to dword

			i = SetFilePointer( hFile, iFilePos + i, NULL, FILE_BEGIN );

			if( i > 0 )
				iFilePos = i;
			else
			{
				DEBUGMSG( DEBUG_LoadImageFromResFile, ( "_LoadImageFromResFile:file size error.\r\n" ) );
				break;
			}
		}
		free( lpIndexHeader );
	}
	else
	{
		DEBUGMSG( DEBUG_LoadImageFromResFile, ( "_LoadImageFromResFile:not enough memory.\r\n" ) );
	}
	//RETAILMSG( 1, ( "s-.\r\n" ) );
	return hImage;
}

// **************************************************
// 声明：
// 参数：
// 	无
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	
// 引用: 
//	
// ************************************************

//extern BOOL KL_GetInstanceFileName( HINSTANCE hInst, LPTSTR lpsz, UINT iDataLen );
// LN: 2003.05.08, 增加功能
static HANDLE GetShareResource( 
								HANDLE hInst, 
								LPCTSTR lpszImageName, 
								UINT uType, 
                                int cxDesired,
                                int cyDesired )
{
	HANDLE hImage = NULL;

	if( uType == IMAGE_ICON )
	{
		EnterCriticalSection( &csIconObjList );
		if( (hImage = _FindIconCursor( hInst, &lpIconObjList, (DWORD)lpszImageName, cxDesired, cyDesired ) ) != NULL )
		{
			Interlock_Increment( (LPLONG)&((LPOBJLIST)hImage)->iRefCount );
		}
		LeaveCriticalSection( &csIconObjList );
	}
	else if( uType == IMAGE_CURSOR )
	{
		EnterCriticalSection( &csCursorObjList );
		if( (hImage = _FindIconCursor( hInst, &lpCursorObjList, (DWORD)lpszImageName, cxDesired, cyDesired ) ) != NULL )
		{
			Interlock_Increment( (LPLONG)&((LPOBJLIST)hImage)->iRefCount );
		}
		LeaveCriticalSection( &csCursorObjList );
	}
	else if( uType == IMAGE_BITMAP )
	{
		hImage = _GetShareBitmapObj( hInst, (DWORD)lpszImageName );
	}

	return hImage;
}


// **************************************************
// 声明：
// 参数：
// 	无
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	
// 引用: 
//	
// ************************************************

#define RES_FILE 1
#define ICO_FILE 2
#define CUR_FILE 3
#define BMP_FILE 4

static HANDLE DoLoadImage( 
					 HANDLE hInst,
                     //LPCTSTR lpcszFileName,//is valid if LR_FROMFILE
					 HANDLE hResFile,
					 int iFileType,  // res file type
					 DWORD dwOffset,  // the res offset in module file
					 LPCTSTR lpszImageName,
                     UINT uType,
                     int cxDesired,
                     int cyDesired,
					 UINT fuLoad )
{
    HANDLE hImage = NULL;

	if( !( uType == IMAGE_BITMAP ||
           uType == IMAGE_CURSOR ||
           uType == IMAGE_ICON
          ) )
	{
		SetLastError( ERROR_INVALID_PARAMETER );
		return NULL;
	}
	// check file type
	if( iFileType == 0 || hResFile == INVALID_HANDLE_VALUE )
	{
		SetLastError( ERROR_INVALID_PARAMETER );
		return NULL;
	}

	if( fuLoad & LR_LOADFROMFILE )
	{
		fuLoad &= ~LR_SHARED;	//目前不支持
	}

    {			
        if( iFileType == BMP_FILE )//IMAGE_BITMAP )
            hImage = _HandleBitmapFile( hResFile, fuLoad & LR_SHARED );
		else if( iFileType == CUR_FILE || iFileType == ICO_FILE )
		{
            hImage = _HandleIconCursorFile( hResFile, uType, cxDesired, cyDesired, 0, TRUE );
		}
		else if( iFileType == RES_FILE )
		{
            if( uType == IMAGE_CURSOR )
				hImage = _LoadImageFromResFile( hResFile, dwOffset, (UINT)RT_CURSOR, lpszImageName, cxDesired, cyDesired, (fuLoad & LR_SHARED) );
			else if( uType == IMAGE_ICON )
				hImage = _LoadImageFromResFile( hResFile, dwOffset, (UINT)RT_ICON, lpszImageName, cxDesired, cyDesired, (fuLoad & LR_SHARED) );
			else if( uType == IMAGE_BITMAP )
				hImage = _LoadImageFromResFile( hResFile, dwOffset, (UINT)RT_BITMAP, lpszImageName, cxDesired, cyDesired, (fuLoad & LR_SHARED) );
		}
		if( hImage && (fuLoad & LR_SHARED) )
		{
			if( uType == IMAGE_CURSOR || uType == IMAGE_ICON )
			{
				_LPICONDATA lpIcon = _GetHICONPtr( hImage );
				lpIcon->hInst = hInst;
			}
			else if( uType == IMAGE_BITMAP )
			{
				_LPBITMAP_SHARE lpBitmap = (_LPBITMAP_SHARE)_GetHBITMAPPtr( hImage );
				lpBitmap->hInst = hInst;
				lpBitmap->szName = (DWORD)lpszImageName;
			}
		}
    }
    return hImage;
}

// **************************************************
// 声明：
// 参数：
// 	无
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	
// 引用: 
//	
// ************************************************

#define DEBUG_GETRESFILEHANDLE 0
static HANDLE GetResFileHandle( HINSTANCE hInst, LPCTSTR lpcszFileName, UINT uType, DWORD * lpdwOffset, int * lpiFileType, BOOL * lpbFreeFile )
{
	HANDLE hFile;
	int iFileType = 0;
	int i;

	DEBUGMSG( DEBUG_GETRESFILEHANDLE, ( ("GetResFileHandle entry.hInst=(0x%x),lpcszFileName=(0x%x).\r\n" ), hInst,lpcszFileName ) );
	
    *lpdwOffset = 0;
	*lpbFreeFile = FALSE;
	
	if( hInst == NULL && lpcszFileName == NULL )
	{
		extern HINSTANCE hgwmeInstance;
		hInst = hgwmeInstance;
	}

	if( hInst )
	{
		extern HANDLE WINAPI Module_GetFileHandle( HMODULE );
		extern HANDLE WINAPI Module_GetSectionOffset( HMODULE, LPCTSTR );

		hFile = Module_GetFileHandle( (HMODULE)hInst );
		*lpdwOffset = (DWORD)Module_GetSectionOffset( (HMODULE)hInst, ".res" );
		iFileType = RES_FILE;
		DEBUGMSG( DEBUG_GETRESFILEHANDLE, ( ("open res file from hinst(0x%x) hFile(0x%x).\r\n" ), hInst, hFile ) );
	}
	else
	{  //lpcszFileName is valid
		hFile = CreateFile( lpcszFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL );
		*lpbFreeFile = TRUE;
		DEBUGMSG( DEBUG_GETRESFILEHANDLE, ( ("open res file from file(%s),hFile(0x%x).\r\n" ), lpcszFileName, hFile ) );
	}

	if( hFile != INVALID_HANDLE_VALUE && iFileType == 0 && lpcszFileName )
	{	// detect res format		
		if( (i = strlen(lpcszFileName))  >= 5 )
		{
			if( stricmp( lpcszFileName + i - 4, ".bmp" ) == 0 )
			{
				iFileType = BMP_FILE;
			}
			else if( stricmp( lpcszFileName + i - 4, ".res" ) == 0 )
			{
				iFileType = RES_FILE;
			}
			else if( stricmp( lpcszFileName + i - 4, ".ico" ) == 0 )
			{
				iFileType = ICO_FILE;
			}
			else if( stricmp( lpcszFileName + i - 4, ".cur" ) == 0 )
			{
				iFileType = CUR_FILE;
			}
		}
		// the file ext name is not define
		if( iFileType == 0 )
		{
			if( uType == IMAGE_BITMAP )
			{
				iFileType = BMP_FILE;
			}
			else if( uType == IMAGE_CURSOR )
			{
				iFileType = CUR_FILE;
			}
			else if( uType == IMAGE_ICON )
			{
				iFileType = ICO_FILE;
			}
		}
	}
	*lpiFileType = iFileType;

	DEBUGMSG( hFile == INVALID_HANDLE_VALUE || hFile == 0, ( ("can't open res file handle.\r\n" ) ) );

	return hFile;
}

// **************************************************
// 声明：
// 参数：
// 	无
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	
// 引用: 
//	
// ************************************************

HICON WINAPI WinGdi_LoadIcon(
  HINSTANCE hInst, // handle to application instance
  LPCTSTR lpIconName   // icon-name string or icon resource 
                       // identifier
)
{
	DWORD dwOffset;
	int iFileType;
	HANDLE hFile;
	HICON hRetv = NULL;
	BOOL bFreeFile;

	if( hInst == NULL )
	{
		extern HINSTANCE hgwmeInstance;
		hInst = hgwmeInstance;
	}

	hRetv = GetShareResource( hInst, lpIconName, IMAGE_ICON, WinSys_GetMetrics(SM_CXICON), WinSys_GetMetrics(SM_CYICON) );
	if( hRetv == NULL )
	{  //没有发现，从文件得到		
		hFile = GetResFileHandle( hInst, NULL, IMAGE_ICON, &dwOffset, &iFileType, &bFreeFile );
		
		if( hFile != INVALID_HANDLE_VALUE )	
		{
			hRetv = DoLoadImage( 
				hInst,
				hFile,
				iFileType,
				dwOffset,
				lpIconName,
				IMAGE_ICON,
				WinSys_GetMetrics(SM_CXICON),
				WinSys_GetMetrics(SM_CYICON),
				LR_SHARED );
			if( bFreeFile )
				CloseHandle( hFile );
			//2004-10-29, add
			else
			{	//必定是模块句柄
				Module_ReleaseFileHandle( hInst, hFile );
			}
			//
		}
	}

	return hRetv;
}

// **************************************************
// 声明：
// 参数：
// 	无
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	
// 引用: 
//	
// ************************************************

HCURSOR WINAPI WinGdi_LoadCursor(
  HINSTANCE hInst, // handle to application instance
  LPCTSTR lpIconName   // icon-name string or icon resource 
                      // identifier
)
{
	DWORD dwOffset;
	int iFileType;
	HANDLE hFile;
	HCURSOR hCursor = NULL;
	BOOL bFreeFile;

	if( hInst == NULL )
	{
		extern HINSTANCE hgwmeInstance;
		hInst = hgwmeInstance;
	}

    hCursor = GetShareResource( hInst, lpIconName, IMAGE_ICON, WinSys_GetMetrics(SM_CXCURSOR), WinSys_GetMetrics(SM_CYCURSOR) );
	if( hCursor )
	{   //没有发现，从文件得到
		hFile = GetResFileHandle( hInst, NULL, IMAGE_ICON, &dwOffset, &iFileType, &bFreeFile );
		
		if( hFile != INVALID_HANDLE_VALUE )	
		{
			hCursor = DoLoadImage( 
				hInst,
				hFile,
				iFileType,
				dwOffset,
				lpIconName,
				IMAGE_ICON,
				WinSys_GetMetrics(SM_CXCURSOR),
				WinSys_GetMetrics(SM_CYCURSOR),
				LR_SHARED );
			if( bFreeFile )
				CloseHandle( hFile );
			//2004-10-29, add
			else
			{	//必定是模块句柄
				Module_ReleaseFileHandle( hInst, hFile );
			}
			//

		}
	}
	return hCursor;

}

// **************************************************
// 声明：
// 参数：
// 	无
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	
// 引用: 
//	
// ************************************************

HBITMAP WinGdi_LoadBitmap(
					   HINSTANCE hInst, // handle to application instance
                       LPCTSTR lpBitmapName   // icon-name string or icon resource 
                       )
{
	DWORD dwOffset;
	int iFileType;
	HANDLE hFile;
	HBITMAP hBitmap = NULL;
	BOOL bFreeFile;

	if( hInst == NULL )
	{
		extern HINSTANCE hgwmeInstance;
		hInst = hgwmeInstance;
	}

	hBitmap = GetShareResource( hInst, lpBitmapName, IMAGE_BITMAP, 0, 0 );
	if( hBitmap == NULL )
	{		
		hFile = GetResFileHandle( hInst, NULL, IMAGE_ICON, &dwOffset, &iFileType, &bFreeFile );
		
		if( hFile != INVALID_HANDLE_VALUE )	
		{
			hBitmap = DoLoadImage( 
				hInst,
				hFile,
				iFileType,
				dwOffset,
				lpBitmapName,
				IMAGE_BITMAP,
				0,
				0,
				LR_SHARED );
			if( bFreeFile )
				CloseHandle( hFile );
			//2004-10-29, add
			else
			{	//必定是模块句柄
				Module_ReleaseFileHandle( hInst, hFile );
			}
			//
		}
	}
	return hBitmap;

}

// **************************************************
// 声明：
// 参数：
// 	无
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	
// 引用: 
//	
// ************************************************
 
HANDLE WINAPI WinGdi_LoadImage( 
                     HINSTANCE hInst,
                     LPCTSTR lpszName,
                     UINT uType,
                     int cxDesired,
                     int cyDesired, 
                     UINT fuLoad 
                     )
{
	DWORD dwOffset;
	int iFileType;
	HANDLE hFile;
	HANDLE hRetv = NULL;
	BOOL bFreeFile;

	if(  !( uType == IMAGE_BITMAP ||
            uType == IMAGE_CURSOR ||
            uType == IMAGE_ICON )  
	   )
	{
		SetLastError( ERROR_INVALID_PARAMETER );
		return NULL;
	}
	if( fuLoad & LR_LOADFROMFILE )
	{
		//SetLastError( ERROR_INVALID_PARAMETER );
		hInst = NULL;
	}
	else
	{	
		if( hInst == NULL )
		{	//从系统加载资源
			extern HINSTANCE hgwmeInstance;
			hInst = hgwmeInstance;
		}
	}

	if( uType == IMAGE_CURSOR || uType == IMAGE_ICON )
	{
	    if( cxDesired == 0 )
		{
			if( fuLoad & LR_DEFAULTSIZE )
			    cxDesired = WinSys_GetMetrics( uType == IMAGE_ICON ? SM_CXICON : SM_CXCURSOR );
		    else 
			{
				cxDesired = ACTUAL_WIDTH; // LN, 2003-07-09, 
			}
		}
	    if( cyDesired == 0 )
		{
			if( fuLoad & LR_DEFAULTSIZE )
			    cyDesired = WinSys_GetMetrics( uType == IMAGE_ICON ? SM_CYICON : SM_CYCURSOR );
			else
			{
				cyDesired = ACTUAL_HEIGHT; // LN, 2003-07-09, 
			}
		}
	}
    if( (fuLoad & LR_SHARED) && (fuLoad & LR_LOADFROMFILE) == 0 )
        hRetv = GetShareResource( hInst, lpszName, uType, cxDesired, cyDesired );


	if( hRetv == NULL )
	{   //没有发现，从文件得到
		if( fuLoad & LR_LOADFROMFILE )
		{
			hFile = GetResFileHandle( NULL, lpszName, uType, &dwOffset, &iFileType, &bFreeFile );
			lpszName = NULL; //设置资源名为null //2003-07-28
		}
		else
			hFile = GetResFileHandle( hInst, NULL, uType, &dwOffset, &iFileType, &bFreeFile );
		
		
		if( hFile != INVALID_HANDLE_VALUE )	
		{
			//RETAILMSG( 1, ( "k0.\r\n" ) );
			hRetv = DoLoadImage( 
				hInst,
				hFile,
				iFileType,
				dwOffset,
				lpszName,
				uType,
				cxDesired,
				cyDesired,
				fuLoad );
			//RETAILMSG( 1, ( "k1.\r\n" ) );
			if( bFreeFile )
				CloseHandle( hFile );
			//2004-10-29, add
			else
			{	//必定是模块句柄
				Module_ReleaseFileHandle( hInst, hFile );
			}
			//
		//RETAILMSG( 1, ( "k2.\r\n" ) );
			
		}
	}
	return hRetv;

}

// **************************************************
// 声明：static HANDLE _LoadStringFromResFile( HANDLE hFile, LPCTSTR lpszName, LPTSTR lpszBuf, UINT uBufLen, DWORD idLang )
// 参数：
//	IN hFile - 文件句柄
//	IN lpszName - 资源名字
//	OUT lpszBuf - 接受 数据
//	IN uBufLen - lpszBuf 的内存大小
//	IN idLang - 语言id
//	IN bShare - 该资源是否会被共享
// 返回值：
//	假如成功，返回 资源句柄；否则，返回 NULL
// 功能描述：
//	从文件得到资源
// 引用: 
//	
// ************************************************

#define DEBUG_LoadStringFromResFile 0
static int _LoadStringFromResFile( HANDLE hFile, LPCTSTR lpszName, LPTSTR lpszBuf, UINT uBufLen, DWORD idLang )
{
	int iIndexSize = 8;
	int iIndexCount = 0;
	HANDLE hImage = NULL;
	int retv = 0;	
	RESHEADER rh;
	DWORD dwReaded;
	int i;
	int iFilePos = 0;	
	UINT codePage;
	
	char szCodePage[16];

	if( GetLocaleInfo( idLang, LOCALE_IDEFAULTANSICODEPAGE, szCodePage, sizeof( szCodePage) ) )
	{
		codePage = atoi( szCodePage );
	}
	else
		return 0;

	iFilePos = SetFilePointer( hFile, 0, NULL, FILE_BEGIN );
	
	while(1)
	{
		ReadFile( hFile, &rh, sizeof(rh), &dwReaded, NULL );
		if( dwReaded != sizeof(rh) )
		{
			break;
		}
		if( rh.wType[0] == 0xffff && 
			rh.wType[1] == (WORD)RT_STRING &&
			rh.wLanguageID == (WORD)idLang )
		{   // find it								
			UINT order = rh.wName[1];
			if( rh.wName[0] == 0xffff && 
				(DWORD)lpszName < (order * 16)  )
			{   // find it
				int start;
				int end = (DWORD)lpszName - (order - 1) * 16;
				WORD wTemp[128]; //
				WORD * lpwStringBuf;
				WORD wDataLength;
				//DWORD dwCurPos;
				//dwCurPos = SetFilePointer( hFile, 0, NULL, FILE_CURRENT );
				for( start = 0; start < end; start++ ) 
				{
					WORD wDataLength;
					DWORD dwReaded;
					ReadFile( hFile, &wDataLength, sizeof(wDataLength), &dwReaded, NULL );
					SetFilePointer( hFile, wDataLength * 2, NULL, FILE_CURRENT );
				}
				ReadFile( hFile, &wDataLength, sizeof(wDataLength), &dwReaded, NULL );
				if( wDataLength < sizeof( wTemp ) )
				{
					lpwStringBuf = wTemp;
				}
				else
				{
					lpwStringBuf = malloc( wDataLength );
				}
				if( lpwStringBuf )
				{
					ReadFile( hFile, lpwStringBuf, wDataLength * 2, &dwReaded, NULL );
					retv = WideCharToMultiByte( codePage, 0, lpwStringBuf, wDataLength, lpszBuf, uBufLen, NULL, NULL );
					if( lpwStringBuf != wTemp )
						free( lpwStringBuf );
				}
				break;
			}
		}
		
		if( ( i = rh.dwDataSize + rh.dwHeaderSize ) < 0 )
		{
			ASSERT( 0 );
			break;			
		}
		// search next resource
		i = (i + 3) & (~3); // align to dword
		
		i = SetFilePointer( hFile, iFilePos + i, NULL, FILE_BEGIN );
		
		if( i > 0 )
			iFilePos = i;
		else
		{
			WARNMSG( 0, ( "_LoadStringFromResFile:file size error.\r\n" ) );
			break;
		}
	}
	return retv;
}


int WINAPI WinGdi_LoadString(
					 HINSTANCE hInstance, 
					 UINT uIDName, 
					 LPTSTR lpBuffer, 
					 int nBufferMax 
					 )
{
	DWORD dwOffset;
	int iFileType;
	int retv = 0;
	BOOL bFreeFile;

	if( nBufferMax > 0 && lpBuffer )
	{
		HANDLE hFile = GetResFileHandle( hInstance, NULL, 0, &dwOffset, &iFileType, &bFreeFile );
		
		if( hFile != INVALID_HANDLE_VALUE )
		{
			retv = _LoadStringFromResFile( hFile, (LPCTSTR)uIDName, lpBuffer, nBufferMax-1, GetSystemDefaultLangID() );
			lpBuffer[retv] = 0;
			if( bFreeFile )
				CloseHandle( hFile );
			else
			{	//必定是模块句柄
				Module_ReleaseFileHandle( hInstance, hFile );
			}
		}
	}
	return retv;
}
