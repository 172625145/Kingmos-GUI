/******************************************************
Copyright(c) 版权所有，1998-2005微逻辑。保留所有权利。
******************************************************/


/*****************************************************
文件说明：图形设备界面模块
版本号：3.0.0
开发时期：1998
作者：李林
修改记录：
	2005-10-14, 将所有API 的出口（返回）放到最后位置，并在最后调用 LeaveDCPtr
	 2005-09-15, 修改刷子便移矩形， lprcMask
	 2005-08-11 为TrueType 字体输出做修改
	 2005-08-11 增加 AlphaBlend
	 2005-06-08 add WindowFromDC
	 2005-06-31, WinGdi_SetDIBitsToDevice 返回值有错（始终是0）
	 2004-09-15，DrawText 的下画线 画在了上面
     2004-05-21, 增加 StretchBlt
	 2004-05-10: 增加 TransparentBlt
     2003-09-24: DoExtSelectClipRgn 没有对expose区域做处理
     2003-09-13, SelectClipRgn( hdc, NULL ) mean valid the hdc default display surface
     2003-09-13, TA_CENTER aligned horizontally not vertizontally
     2003-09-06: LN, 在BRUSH & PEN结构增加 ->clrRef 成员
     2003-07-22, SelectClipRgn( hdc, hrgn ), 当 hrgn==NULL时，清空 region
     2003-07-22: DeleteDC( ... ) 不用 删掉 hBitmap, hBrush, hPen, ...
     2003-07-14: 增加Arc 功能 和 GDCDATA-> arcMode 成员
     2003-06-06: _GDCDATA  加入 OBJLIST   obj,取代objType和ulOwner
     2003-05-14 : CreateBitmap 为避免Heap的碎片化 对bitmap，当其所用缓存大小 大于 某大小时， 用VirtualAlloc
     2003-05-06 : 
	    1.LN 当SetBkMode( TRANSPARENT ) 并且 调用Rectangle 
	       (用WHITE_BRUSH)有错误
		2.增加对 FillRect( hdc, (COLOR_WINDOW+1) )的支持

******************************************************/

#include <eframe.h>
#include <efile.h>
#include <eassert.h>
#include <enls.h>
#include <eapisrv.h>
#include <gwmeobj.h>
#include <gdc.h>
#include <bheap.h>
#include <epwin.h>
#include <eprgn.h>
#include <winsrv.h>
#include <gdisrv.h>


#define XOFFSET( lpdc ) ((lpdc)->deviceOrg.x + (lpdc)->viewportOrg.x - (lpdc)->windowOrg.x)
#define YOFFSET( lpdc ) ((lpdc)->deviceOrg.y + (lpdc)->viewportOrg.y - (lpdc)->windowOrg.y)
#define WIDTH_LEFT( w ) ( (w) >> 1 )
#define WIDTH_TOP( w ) ( (w) >> 1 )
#define WIDTH_RIGHT( w ) ( (w) - ((w) >> 1) - 1 )
#define WIDTH_BOTTOM( w ) ( (w) - ((w) >> 1) - 1 )
#define ABS( x ) ( (x) >= 0 ? (x) : -(x) )

typedef struct __FILLRGN
{
    const _RECTNODE FAR* lprNodes;
    RECT rect;
}_FILLRGN;
typedef void ( * LPFILLFUN )( _LPCDISPLAYDRV lpDispDrv, _LPBLKBITBLT lpFillData, _FILLRGN * lpFillRgn );


#define _MAX_GDIOBJECT_NUM 10
static _GDCDATA * lpSysDC;//[_MAX_GDIOBJECT_NUM];

static CRITICAL_SECTION csDCList;
static LPOBJLIST lpDCObjList = NULL;
static HANDLE hSempore = NULL;
static int cwEllipsis;

_LPBITMAPDATA _GetHBITMAPPtr( HBITMAP );
_LPBRUSHDATA _GetHBRUSHPtr( HBRUSH hBrush );
BOOL ReleasePaintDC( HWND hWnd, HDC hdc );


extern HGDIOBJ WINAPI OEM_GetStockObject( int fObject );

static BOOL _DrawLine( _LPCDISPLAYDRV lpDispDrv, _LPRECTNODE lprn, _LINEDATA * lpLine, int x0, int y0, int x1, int y1 );
static void _FillTransparentRgn( _LPCDISPLAYDRV lpDispDrv, _LPBLKBITBLT lpFillData, _FILLRGN * lpFillRgn );
static void _FillRgn( _LPCDISPLAYDRV lpDispDrv, _LPBLKBITBLT lpFillData, _FILLRGN * lpFillRgn );
static int DoExtSelectClipRgn( HDC hdc, HRGN hrgn, int mode );

// **************************************************
// 声明：BOOL _InitialGdi( void )
// 参数：
// 	无
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	初始化GDI	
// 引用: 
//	当GWME系统初始化时，回调用该函数
// ************************************************

BOOL _InitialGdi( void )
{
	int i;
	HFONT hFont;
    _LPFONT lpFont;
    InitializeCriticalSection( &csDCList );
	csDCList.lpcsName = "CS-GDC";

	lpSysDC = malloc( sizeof(_GDCDATA) * _MAX_GDIOBJECT_NUM );
	if( lpSysDC )
	{
		for( i = 0; i < _MAX_GDIOBJECT_NUM; i++ )
		{
			lpSysDC[i].obj.objType = OBJ_NULL;
		}
	}
	hFont = OEM_GetStockObject(SYSTEM_FONT);//系统默认的字体
	lpFont = _GetHFONTPtr( hFont );
	cwEllipsis = lpFont->lpDriver->lpWordWidth( lpFont->handle, ' ' );
	//初始化信号量
	hSempore = CreateSemaphore( NULL, _MAX_GDIOBJECT_NUM, _MAX_GDIOBJECT_NUM, NULL ); 
	return TRUE;
}

// **************************************************
// 声明：void _DeInitialGdi( void )
// 参数：
//    无
// 返回值：
//	无
// 功能描述：
//	关闭GDI	
// 引用: 
//	与_InitialGdi对应，该函数释放已分配的资源
// ************************************************

void _DeInitialGdi( void )
{

	CloseHandle( hSempore );
	DeleteCriticalSection( &csDCList );
}

// **************************************************
// 声明：_LPCDISPLAYDRV GetDisplayDeviceDriver( _LPBITMAPDATA lpBitmap )
// 参数：
// 	IN lpBitmap - _BITMAPDATA结构指针
// 返回值：
//	假入成功，返回显示驱动程序接口；否则，返回NULL
// 功能描述：
//	得到一个兼容显示面的图形驱动程序接口
// 引用: 
//	
// ************************************************

_LPCDISPLAYDRV GetDisplayDeviceDriver( _LPBITMAPDATA lpBitmap )
{
	_LPCDISPLAYDRV lpdd = NULL;

	if( lpBitmap &&
		lpBitmap->bmPlanes == 1 )
	{
		if( lpBitmap->bmBitsPixel == lpDisplayBitmap->bmBitsPixel )
			lpdd = lpDrvDisplayDefault;
#ifdef HAVE_GDC_1BPP
		else if( lpBitmap->bmBitsPixel == 1 )
			lpdd = &_drvDisplay1BPP;
#endif
#ifdef HAVE_GDC_4BPP
		else if( lpBitmap->bmBitsPixel == 4 )
			lpdd = &_drvDisplay4BPP;
#endif
#ifdef HAVE_GDC_8BPP
		else if( lpBitmap->bmBitsPixel == 8 ) //
			lpdd = &_drvDisplay8BPP;
#endif
#ifdef HAVE_GDC_16BPP
		else if( lpBitmap->bmBitsPixel == 16 )  //
			lpdd = &_drvDisplay16BPP;
#endif
#ifdef HAVE_GDC_32BPP
		else if( lpBitmap->bmBitsPixel == 32 )  //
			lpdd = &_drvDisplay32BPP;
#endif
#ifdef HAVE_GDC_24BPP
		else if( lpBitmap->bmBitsPixel == 24 )  
			lpdd = &_drvDisplay24BPP;
#endif
	}
	return lpdd;
}

// **************************************************
// 声明：static VOID _CheckGDCData( void )
// 参数：
// 	无 
// 返回值：
//	 无
// 功能描述：
//	检查DC
// 引用: 
//	
// ************************************************

static VOID _CheckGDCData( void )
{
	int i;
	_LPGDCDATA p = NULL;

	// 申请一个系统DC资源
	p = lpSysDC;
    for( i = 0; i < _MAX_GDIOBJECT_NUM; i++, p++ )
	{
		if( p->hwnd )
		{
			_LPWINDATA lpws;
		    //由窗口句柄得到其指针对象
			lpws = _GetHWNDPtr( p->hwnd );
			if( lpws )
			{
				DWORD dwOldPerm = SetProcPermissions( ~0 );  //得到对进程的存取权限
				RETAILMSG( 1, ( "_CheckGDCData: win=%s,class=%s,flags=0x%x.\r\n", lpws->lpWinText, lpws->lpClass->wc.lpszClassName, p->uiFlags ) );
				SetProcPermissions( dwOldPerm );  //得到对进程的存取权限
			}
		}
		else
		{
			RETAILMSG( 1, ( "_CheckGDCData: null window,objType=0x%x,flags=0x%x.\r\n", p->obj.objType, p->uiFlags ) );
		}
	}
}

// **************************************************
// 声明：static _LPGDCDATA _AllocGDCData( void )
// 参数：
// 	无 
// 返回值：
//	 假入成功，返回_GDCDATA结构指针；否则，返回NULL
// 功能描述：
//	从系统保留的DC里分配一个DC数据结构
// 引用: 
//	
// ************************************************
#define DEBUG_AllocGDCData 0
static _LPGDCDATA _AllocGDCData( void )
{
    int i, count;
	_LPGDCDATA p = NULL;

	// 申请一个系统DC资源信号量，如果申请不到，则循环申请20次
	// 我应该用更好的办法去做该功能。。。。
	count = 0;  
	while( count < 20 )
	{
		if( WaitForSingleObject( hSempore, 100 ) == WAIT_OBJECT_0 )
			break;
		WARNMSG( DEBUG_AllocGDCData, (TEXT("_AllocGDCData:try get dc(%d)\r\n"), count ) );
		count++;
	}

	if( count == 20 )
	{	//没有得到资源，通常是系统有什么错误。
		WARNMSG( DEBUG_AllocGDCData, (TEXT("_AllocGDCData:no enough DC solt, thread=0x%x\r\n"), GetCurrentThreadId() ) );
		_CheckGDCData();
		return NULL;
	}

	// 申请一个系统DC资源
	count = 20;	//试20次
	while( count-- )
	{
		p = lpSysDC;
		for( i = 0; i < _MAX_GDIOBJECT_NUM; i++, p++ )
		{
			if( p->obj.objType == OBJ_NULL &&
				Interlock_TestExchange( (LPLONG)&p->obj.objType, OBJ_NULL, OBJ_DC ) == OBJ_NULL )
			{
				p->obj.ulOwner = (ULONG)GetCallerProcess();
				return p;
			}
		}
	}
	
	// 不应该到这里
	ASSERT( 0 );
	ERRORMSG( DEBUG_AllocGDCData, (TEXT("error in _AllocGDCData: undef error!.\r\n") ) );	

	return NULL;
}

// **************************************************
// 声明：int UpdateDCState( UINT uiState )
// 参数：
// 	IN uiState - 
// 返回值：
//	返回1；
// 功能描述：
//	更新DC状态
// 引用: 
//	当窗口的Z序、大小改变并且窗口当前有正在使用的DC时，该DC对应的
//    窗口的内容（大小、裁剪区域）也虽之改变。这里就是告诉DC去重新更新
//	相关内容
// ************************************************

int UpdateDCState( UINT uiState )
{
    int i;
	_LPGDCDATA lpdc = lpSysDC;

    for( i = 0; i < _MAX_GDIOBJECT_NUM; i++, lpdc++ )
    {		
        if( lpdc->obj.objType != OBJ_NULL &&
			lpdc->hwnd )
        {   
			Interlock_TestExchange( (LPLONG)&lpdc->uiState, 0, 1 );
        }
    }
	return 1;
}

// **************************************************
// 声明：static BOOL CALLBACK DelMemDCObj( LPOBJLIST lpObj, LPARAM lParam )
// 参数：
// 	IN lpObj - _GDCDATA对象
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	删除一个内存DC对象
// 引用: 
//	
// ************************************************

static BOOL CALLBACK DelMemDCObj( LPOBJLIST lpObj, LPARAM lParam )
{
	_LPGDCDATA lpdc = (_LPGDCDATA)lpObj;

    ASSERT( lpObj->objType == OBJ_MEMDC );

    WinGdi_DeleteObject( lpdc->hrgn );
    lpdc->obj.objType = OBJ_NULL;

    BlockHeap_Free( hgwmeBlockHeap, 0, lpdc, sizeof(_GDCDATA) );
    
	return TRUE;

}

// **************************************************
// 声明：BOOL WINAPI WinGdi_DeleteDC( HDC hdc )
// 参数：
// 	IN hdc - DC句柄
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	删除设备对象。你不能删除用GetDC得到的hdc, 你能用ReleaseDC去释放GetDC得到的hdc
// 引用: 
//	系统API
// ************************************************

BOOL WINAPI WinGdi_DeleteDC( HDC hdc )
{
    _LPGDCDATA lpdc = _GetHDCPtr( hdc );

    if( lpdc && lpdc->obj.objType == OBJ_MEMDC )  // 合法吗？
    {
	    EnterCriticalSection( &csDCList );  //进入互斥段
	    ObjList_Remove( &lpDCObjList, &lpdc->obj );  // 从链表移出
	    LeaveCriticalSection( &csDCList );  //离开互斥段

		return DelMemDCObj( &lpdc->obj, 0 );//释放
    }
    else
    {
    	WARNMSG( 1, ( "error at WinGdi_DeleteDC: invalid hdc(0x%x).\r\n", hdc ) );
        return FALSE;
    }
}

// **************************************************
// 声明：static BOOL DelSysDC( _LPGDCDATA lpdc )
// 参数：
// 	IN lpdc - _GDCDATA结构指针
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	删除DC,这里的DC是指用BeginPaint 或 GetDC 或 GetDCEx得到的
// 引用: 
//	
// ************************************************

static BOOL DelSysDC( _LPGDCDATA lpdc )
{
	WinGdi_DeleteObject( lpdc->hrgn );

	lpdc->uiState = 0; 
	lpdc->obj.ulOwner = 0;
	Interlock_Exchange( (LPLONG)&lpdc->obj.objType, OBJ_NULL );
	ReleaseSemaphore( hSempore, 1, NULL );  // 释放一个DC资源（信号量）
	return TRUE;
}

// **************************************************
// 声明：BOOL ReleasePaintDC( HWND hWnd, HDC hdc )
// 参数：
//	IN hWnd - 窗口句柄
//	IN hdc - DC句柄
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	释放一个基于窗口的DC
// 引用: 
//	
// ************************************************

// the option export for win.c
BOOL ReleasePaintDC( HWND hWnd, HDC hdc )
{
    _LPGDCDATA lpdc = _GetHDCPtr( hdc );

    if( lpdc )
    {	// 测试该DC是否与该窗口相关
		if( lpdc->obj.objType == OBJ_DC )
		{
			if( (LONG)hWnd == Interlock_TestExchange( (LONG*)&lpdc->hwnd, (LONG)hWnd, NULL ) )
			{	//是，释放它
				return DelSysDC( lpdc );
			}
			else
			{
				WARNMSG( 1, ( "error at ReleasePaintDC: the hdc(0x%x) did'nt alloc by the hWnd(0x%x).\r\n", hdc, hWnd ) );
			}
		}
		else if( lpdc->obj.objType == OBJ_MEMDC )
		{	// 必定是由 WS_EX_LAYERED 窗口分配的
			ufNeedFlushGDICount++;
			return ReleaseLayerWindowDC( hWnd, hdc );
		}
    }
    return FALSE;
}

// **************************************************
// 声明：HDC GetPaintDC( HWND hwnd, LPCRECT lprect, HRGN hrgnClip, UINT uiFlags )
// 参数：
// 	IN hwnd - 窗口句柄
// 	IN lprect - 窗口矩形（设备坐标）
//	 IN hrgnClip - 裁剪区域
// 	IN uiFlags - 标志
// 返回值：
//	假入成功，返回分配的DC句柄；否则，返回NULL
// 功能描述：
//	从系统分配一个DC用于绘图
// 引用: 
//	
// ************************************************

HDC GetPaintDC( HWND hwnd, LPCRECT lprect, HRGN hrgnClip, UINT uiFlags )
{
    _LPGDCDATA lpdc = _AllocGDCData();// 得到一个DC结构

    if( lpdc )
    {
		
        lpdc->hrgn = hrgnClip;
        lpdc->hPen = OEM_GetStockObject(BLACK_PEN);//系统默认的PEN
		lpdc->penAttrib = _GetHPENPtr( lpdc->hPen )->penAttrib;

        lpdc->hBrush = OEM_GetStockObject(WHITE_BRUSH);//系统默认刷子
		lpdc->brushAttrib = _GetHBRUSHPtr( lpdc->hBrush )->brushAttrib;

        lpdc->hFont = OEM_GetStockObject(SYSTEM_FONT);//系统默认的字体
		// 2004-02-16
        lpdc->hBitmap = (HBITMAP)hbmpCurrentFrameBuffer;//hbmpDisplayBitmap;//lpDisplayBitmap;//系统默认位图
		//
		lpdc->lpDispDrv = lpDrvDisplayDefault;//系统默认图形驱动程序

        lpdc->deviceOrg.x = lprect->left;//窗口在设备的左上角x坐标
        lpdc->deviceOrg.y = lprect->top;//窗口在设备的左上角y坐标
        lpdc->deviceExt.cx = lprect->right - lprect->left;//窗口宽度
        lpdc->deviceExt.cy = lprect->bottom - lprect->top;//窗口高度
        lpdc->stretchMode = 0;//伸缩模式
        lpdc->mapMode = MM_TEXT;//映射模式MM_TEXT
        lpdc->rop = R2_COPYPEN;//笔操作模式
		lpdc->arcMode = AD_CLOCKWISE;//顺时针画弧, AD_COUNTERCLOCKWISE;
        lpdc->backMode = OPAQUE;//不透明背景
        lpdc->backColor = (DWORD)OEM_GetStockObject( SYS_STOCK_WHITE );//默认的背景色
        lpdc->textColor = (DWORD)OEM_GetStockObject( SYS_STOCK_BLACK );//默认的文本色
        lpdc->textAlign = TA_TOP | TA_LEFT;//文本对齐模式
        lpdc->position.x = lpdc->position.y = 0;//当前的默认绘图起点
        lpdc->windowOrg = lpdc->viewportOrg = lpdc->position;//窗口原点，视区原点

		lpdc->uiState = 0; // 状态
		lpdc->uiFlags = uiFlags;//标志
        lpdc->hwnd = hwnd;//关联窗口
		lpdc->lpdwPal = 0;  //默认的调色板
		lpdc->wPalNumber = 0;  //默认的调色板
		lpdc->wPalFormat = 0;  //默认的调色板
		lpdc->ptBrushOrg.x = lprect->left;
		lpdc->ptBrushOrg.y = lprect->top;

		return (HDC)PTR_TO_HANDLE( lpdc ); // 将DC指针转化为句柄
    }

	return NULL;

}

// **************************************************
// 声明：BOOL DeleteDCObject( HANDLE hOwner )
// 参数：
// 	IN hOwner - 拥有者句柄
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	清除拥有者的所有之前分配而没有释放的DC。通常当一个进程退出时，调用该功能去清除
//	进程没有释放的DC
// 引用: 
//	
// ************************************************

BOOL DeleteDCObject( HANDLE hOwner )
{
    int i;
	_LPGDCDATA lpdc;

    EnterCriticalSection( &csDCList );//进入互斥段
	//清除所有的属于拥有者对象
    ObjList_Delete( &lpDCObjList, (ULONG)hOwner, DelMemDCObj, 0 );

    LeaveCriticalSection( &csDCList );//离开互斥段

	// release sysdc
	lpdc = lpSysDC;

    for( i = 0; i < _MAX_GDIOBJECT_NUM; i++, lpdc++ )
    {		
        if( lpdc->obj.objType != OBJ_NULL &&
			lpdc->obj.ulOwner == (ULONG)hOwner )  //属于拥有者 ？
        {   //是
			InterlockedExchange( (LONG*)&lpdc->hwnd, NULL );
			DelSysDC( lpdc );
        }
    }
	return 1;
}

// **************************************************
// 声明：HDC WINAPI WinGdi_CreateCompatibleDC(HDC hdc)
// 参数：
// 	IN hdc - DC句柄
// 返回值：
//	假入成功，返回DC句柄；否则，返回NULL
// 功能描述：
//	创建兼容于hdc的内存DC
// 引用: 
//	系统API
// ************************************************

HDC WINAPI WinGdi_CreateCompatibleDC(HDC hdc)
{
    _LPGDCDATA lpSrcDC = NULL;
	_LPGDCDATA lpdc;

    if( hdc )
	{
        lpSrcDC = _GetHDCPtr( hdc );
		if( lpSrcDC == NULL )
			return NULL;
	}

    lpdc = (_LPGDCDATA)BlockHeap_Alloc( hgwmeBlockHeap, BLOCKHEAP_ZERO_MEMORY, sizeof(_GDCDATA) ); // 分配DC结构
//    lpdc = (_LPGDCDATA)BLK_Alloc( 0, sizeof(_GDCDATA) ); // 分配DC结构

    if( lpdc )  // 分配成功？
    {	//是，初始化DC为1x1的黑白位图格式
		// hrgn 不能用 OEM_GetStockObject, 因为当选进bitmap后，需要改变其裁剪矩形的大小
		lpdc->lpDispDrv = &_drvDisplay1BPP;

		lpdc->lpdwPal = 0;  //默认的调色板
		lpdc->wPalNumber = 0;  //默认的调色板
		lpdc->wPalFormat = 0;  //默认的调色板

        lpdc->hrgn = WinRgn_CreateRect( 0, 0, 1, 1 );//OEM_GetStockObject(SYS_STOCK_RGN);
		//初始化为黑笔
        lpdc->hPen = OEM_GetStockObject(BLACK_PEN);
		//设置设备依赖笔数据结构
		lpdc->penAttrib = _GetHPENPtr( lpdc->hPen )->penAttrib;
		//设置设备依赖颜色
		lpdc->penAttrib.color = lpdc->lpDispDrv->lpRealizeColor(lpdc->penAttrib.clrRef, lpdc->lpdwPal, lpdc->wPalNumber, lpdc->wPalFormat ); 
		//设置设备依赖刷子数据结构
        lpdc->hBrush = OEM_GetStockObject(WHITE_BRUSH);
		lpdc->brushAttrib = _GetHBRUSHPtr( lpdc->hBrush )->brushAttrib;
		//设置设备依赖颜色
		lpdc->brushAttrib.color = lpdc->lpDispDrv->lpRealizeColor(lpdc->brushAttrib.clrRef, lpdc->lpdwPal, lpdc->wPalNumber, lpdc->wPalFormat); 
		//默认字体
        lpdc->hFont = OEM_GetStockObject(SYSTEM_FONT);
		//默认显示面
		lpdc->hBitmap = OEM_GetStockObject(SYS_STOCK_BITMAP1x1);
		ASSERT( lpdc->hBitmap );		
		//设备原点
        lpdc->deviceOrg.x = 0;
        lpdc->deviceOrg.y = 0;
        lpdc->deviceExt.cx = 0;
        lpdc->deviceExt.cy = 0;
        lpdc->stretchMode = 0;

        lpdc->mapMode = MM_TEXT;
        lpdc->rop = R2_COPYPEN;
        lpdc->backMode = OPAQUE;
        lpdc->backColor = 0xffffffffl;
        lpdc->textColor = 0;
        lpdc->textAlign = TA_TOP | TA_LEFT;
        lpdc->position.x = lpdc->position.y = 0;
        lpdc->windowOrg = lpdc->viewportOrg = lpdc->position;
        lpdc->hwnd = NULL;

    }
	if( lpdc )
	{	//插入DC对象链表
		EnterCriticalSection( &csDCList );
		ObjList_Init( &lpDCObjList, &lpdc->obj, OBJ_MEMDC, (ULONG)GetCallerProcess() );
		LeaveCriticalSection( &csDCList );

		return (HDC)PTR_TO_HANDLE( lpdc ); // 转化为句柄
	}

	return NULL;
}

// **************************************************
// 声明：HBITMAP WINAPI WinGdi_CreateCompatibleBitmap(HDC hdc, int iWidth, int iHeight)
// 参数：
//    IN hdc - 设备DC
//	IN iWidth - 位图宽度
//	IN iHeight - 位图高度
// 返回值：
//	假入成功，返回非NULL位图句柄；否则，返回NULL
// 功能描述：
//	创建兼容DC的位图
// 引用: 
//	系统API
// ************************************************

HBITMAP WINAPI WinGdi_CreateCompatibleBitmap(HDC hdc, int iWidth, int iHeight)
{
    if( hdc )
	{
		_LPGDCDATA lpdc = _GetHDCPtr( hdc );
		if( lpdc )
		{	//有效
			_LPBITMAPDATA lpImage = _GetHBITMAPPtr( lpdc->hBitmap );
			return lpImage ? WinGdi_CreateBitmap( iWidth, iHeight, lpImage->bmPlanes, lpImage->bmBitsPixel, 0 ) : NULL;
		}
	}
	else
	{   // 用屏幕DC, use screen dc
		return WinGdi_CreateBitmap( iWidth, iHeight, lpDisplayBitmap->bmPlanes, lpDisplayBitmap->bmBitsPixel, 0 );
	}
    return NULL;
}


// **************************************************
// 声明：_LPGDCDATA _GetSafeDrawPtr( HDC hdc )
// 参数：
// 	IN hdc - DC句柄 
// 返回值：
//	假如成功，返回_LPGDCDATA指针；否则，返回NULL
// 功能描述：
//	得到安全的DC.
//	1.检查DC是否合法; 2.检查DC的状态是否改变，如果是，更新它
// 引用: 
//	
// ************************************************

extern int _GetRgnInfo(HRGN hrgn);
#define DEBUG_GETSAFEDRAWPTR 0
_LPGDCDATA _GetSafeDrawPtr( HDC hdc )
{	// 由句柄得到其对象指针
	_LPGDCDATA lpdc = (_LPGDCDATA)HANDLE_TO_PTR( hdc );
	if( lpdc )
	{	
		if( ( (WORD)GET_OBJ_TYPE( lpdc) == OBJ_MEMDC || 
			  (WORD)GET_OBJ_TYPE( lpdc ) == OBJ_DC ) )
		{	// 类型有效
			if( lpdc->hwnd )
			{	// 该DC 与窗口关联，如果这时窗口坐标改变，需要同时更新DC
				int iUpdate = 0;
				int iRgn;
				while( lpdc->uiState &&  
					   Interlock_TestExchange( (LPLONG)&lpdc->uiState, 1, 0 ) )
				{	//改变
					_LPWINDATA lpws;
					RECT rect;
					HRGN hRgn;
					UINT uiFlags = lpdc->uiFlags;
					
					iUpdate++;  // use to test

					iRgn = SIMPLEREGION;
				    //由窗口句柄得到其指针对象
					lpws = _GetHWNDPtr( lpdc->hwnd );
					//得到DC的显示屏幕矩形
					if( uiFlags & DCX_WINDOW )
					{	//窗口DC
						rect = lpws->rectWindow;
					}
					else
					{	//客户DC
						rect = lpws->rectClient;
						OffsetRect( &rect, lpws->rectWindow.left, lpws->rectWindow.top );
					}
					//设备原点
					lpdc->deviceOrg.x = rect.left;
					lpdc->deviceOrg.y = rect.top;
					lpdc->deviceExt.cx = rect.right - rect.left;
					lpdc->deviceExt.cy = rect.bottom - rect.top;				
					
//					LockWindow();  ////2003.1.26
					//得到窗口的暴露矩形域
				    iRgn = GetExposeRgn( lpdc->hwnd, uiFlags, &hRgn );
					if( iRgn == NULLREGION ||
						iRgn == ERROR )
					{	//暴露矩形域为空或有一个错误
						WinRgn_SetRect( lpdc->hrgn, 0, 0, 0, 0 );
					}
					else
					{
						if( ( uiFlags & (DCX_VALIDATE | DCX_INTERSECTUPDATE) ) == (DCX_VALIDATE | DCX_INTERSECTUPDATE) )
						{   // 当前DC 用户调用BeginPaint返回的，需要与系统重绘区域进行运算 为a dc from beginpaint
							HRGN hClip;
							//锁住系统重绘区域
							__LockRepaintRgn( &hClip );  // 2003.1.26
							iRgn = WinRgn_Combine( lpdc->hrgn, lpdc->hrgn, hClip, RGN_OR );
							__UnlockRepaintRgn();   //2003.1.26
							if( !( iRgn == NULLREGION ||
								   iRgn == ERROR ) )
								iRgn = WinRgn_Combine( lpdc->hrgn, lpdc->hrgn, hRgn, RGN_AND );
							DEBUGMSG( DEBUG_GETSAFEDRAWPTR, ( "update dc from BeginPaint,%d,class=%s.\r\n", iUpdate, lpws->lpClass->wc.lpszClassName ) );
						}
						else
						{   // 将新的可视区域拷贝到DC now copy hRgn to dc
							WinRgn_Combine( lpdc->hrgn, hRgn, NULL, RGN_COPY );
						}
					}
					//删除临时 区域
					WinGdi_DeleteObject( hRgn );

//					UnlockWindow();   //2003.1.26
					 
					if( iRgn == NULLREGION ||
						iRgn == ERROR )
					{
						DEBUGMSG( DEBUG_GETSAFEDRAWPTR, ( "GetSafeDrawPtr_0:null rgn, class=%s, title=%s.\r\n", lpws->lpClass->wc.lpszClassName, lpws->lpWinText ) );
						return NULL;
					}
#ifdef __DEBUG
					if( iUpdate > 5 )
					{ 
						DEBUGMSG( DEBUG_GETSAFEDRAWPTR, ( "update dc loop...%d.\r\n", iUpdate ) );
					} 
#endif
				}
				if( _GetRgnInfo( lpdc->hrgn ) == NULLREGION )
				{
#ifdef __DEBUG
					//_LPWINDATA lpws = _GetHWNDPtr( ((_LPGDCDATA)hdc)->hwnd );
				
					//RETAILMSG( 1, ( "GetSafeDrawPtr_1:null rgn, class=%s, title=%s.\r\n", lpws->lpClass->wc.lpszClassName, lpws->lpWinText ) );
#endif
					return NULL;
				}
			}
			return lpdc;
		}
	}
	SetLastError( ERROR_INVALID_HANDLE );
	WARNMSG( 1, (TEXT( "Invalid hdc handle: 0x%x\r\n" ), hdc ) );
    return NULL;
}

// **************************************************
// 声明：_LPGDCDATA _GetHDCPtr( HDC hdc )
// 参数：
// 	IN hdc - DC 句柄
// 返回值：
//	假如成功，返回_LPGDCDATA指针；否则，返回NULL
// 功能描述：
//	得到安全的DC.
//	1.检查DC是否合法; 
// 引用: 
//	
// ************************************************

_LPGDCDATA _GetHDCPtr( HDC hdc )
{
	_LPGDCDATA lpdc = (_LPGDCDATA)HANDLE_TO_PTR( hdc );

	if( lpdc &&
		( (WORD)GET_OBJ_TYPE( lpdc) == OBJ_MEMDC || 
		  (WORD)GET_OBJ_TYPE( lpdc ) == OBJ_DC 
		) 
	  )
	{
		return lpdc;
	}
	SetLastError( ERROR_INVALID_HANDLE );
    WARNMSG( 1, ("error: Invalid HDC handle=0x%x\r\n", hdc) );
    return NULL;
}

// **************************************************
// 声明：VOID _LeaveDCPtr( _LPGDCDATA lpgdc, DWORD dwFlags )
// 参数：
// 	IN lpgdc - 之前调用 GetSafeDrawPtr 或 GetHDCPtr 返回的指针
//	IN dwFlags - DCF_WRITE, DCF_READ ...
// 返回值：
//	无
// 功能描述：
//	当不需要hdc时， 做某些动作...
// 引用: 
//	
// ************************************************


#define DCF_READ     1
#define DCF_WRITE    2

#define DEBUG_GETSAFEDRAWPTR 0
VOID _LeaveDCPtr( _LPGDCDATA lpgdc, DWORD dwFlags )
{
	if( lpgdc )
	{
		if( lpgdc->hBitmap == hbmpCurrentFrameBuffer && 
			dwFlags & DCF_WRITE )
		{
			ufNeedFlushGDICount++;
		}
	}
}


// **************************************************
// 声明：DWORD _Gdi_UnrealizeColor( DWORD dwRealizeColor, DWORD * lpPal, UINT uiPalNum, UINT uiSrcColorType )
// 参数：
// 	IN dwRealizeColor - 设备相关的颜色值
//	IN uiSrcColorType - 颜色类型
//	IN lpPal - 色板指针
//	IN uiPalNum - 色板项目数
// 返回值：
//	假如成功，返回设备无关的颜色值；否则，返回0
// 功能描述：
//	将设备相关的颜色值转化为设备无关的颜色值
// 引用: 
//	
// ************************************************

DWORD _Gdi_UnrealizeColor( DWORD dwRealizeColor, LPCDWORD lpPal, UINT uiPalNum, UINT uiSrcColorType )
{
	DWORD dwRetv;

	if( uiSrcColorType == PAL_INDEX )
		dwRetv = lpPal[dwRealizeColor];	// 颜色类型为基于色板的索引值
	else if( uiSrcColorType == PAL_BITFIELD )
	{	//颜色类型bit位组合， lpPal里保存有三个 r g b 的 mask值
		int leftShift, rightShift;
		DWORD dwColorMask;

		dwRetv = 0;
		for( rightShift = 24; rightShift >= 0 && uiPalNum; rightShift -= 8, uiPalNum--, lpPal++ )
		{
			dwColorMask = *lpPal;
			//得到 需要移位的值
			for( leftShift = 32; dwColorMask; leftShift-- )
				dwColorMask >>= 1;

			dwRetv |= ( ( dwRealizeColor & *lpPal ) << leftShift ) >> rightShift;
		}
	}
	else
		dwRetv = 0;
	return dwRetv;
}


// **************************************************
// 声明：static DWORD _Gdi_RealizeColor( COLORREF clRgbColor, DWORD * lpPal, UINT uiPalNum, UINT uiDestColorType )
// 参数：
// 	IN clRgbColor - 设备无关的颜色值
//	IN uiDestColorType - 颜色类型
//	IN lpPal - 色板指针
//	IN uiPalNum - 色板项目数
// 返回值：
//	假如成功，返回设备相关的颜色值；否则，返回0
// 功能描述：
//	将设备无关的颜色值转化为设备相关的颜色值
// 引用: 
//	
// ************************************************

// return dest format

DWORD _Gdi_RealizeColor( COLORREF clRgbColor, LPCDWORD lpPal, UINT uiPalNum, UINT uiDestColorType )
{
	DWORD dwRetv;

	if( uiDestColorType == PAL_INDEX )
	{	// 颜色类型为基于色板的索引值
		long minDiff, curDiff;
		UINT uiPalCount = uiPalNum;
		

		minDiff = 0x7fffffffl;
		dwRetv = uiPalNum;
			
		for( ; uiPalCount; uiPalCount--, lpPal++ )
		{
			if( clRgbColor == *lpPal )
			{
				dwRetv = uiPalCount;
				break;
			}
			else
			{  //得到距离
				LPBYTE lpbSrc = (LPBYTE)&clRgbColor;
				LPBYTE lpbDest = (LPBYTE)lpPal;
				long lDiff;

				curDiff = *lpbSrc++ - *lpbDest++;
				curDiff *= curDiff;
				lDiff = *lpbSrc++ - *lpbDest++;
				curDiff += lDiff * lDiff;
				lDiff = *lpbSrc - *lpbDest;
				curDiff += lDiff * lDiff;				
			}
			if( curDiff < minDiff )
			{	//得到最短距离
			    minDiff = curDiff;
			    dwRetv = uiPalCount;
			}
		}
		dwRetv = uiPalNum - dwRetv;
	}
	else if( uiDestColorType == PAL_BITFIELD )
	{	//颜色类型bit位组合， lpPal里保存有三个 r g b 的 mask值
		int leftShift, rightShift;
		DWORD dwColorMask;

		dwRetv = 0;
		for( leftShift = 24; leftShift >= 0 && uiPalNum; leftShift -= 8, uiPalNum--, lpPal++ )
		{
			dwColorMask = *lpPal;
			//得到 需要移位的值
			for( rightShift = 32; dwColorMask; rightShift-- )
				dwColorMask >>= 1;

			dwRetv |= ( ( clRgbColor << leftShift ) >> rightShift ) & *lpPal;
		}
	}
	else
		dwRetv = 0;
	return dwRetv;
}

// **************************************************
// 声明：BOOL PixelTransfer( 
//							_LPGDCDATA lpdcDest,//_LPCDISPLAYDRV lpDestDispDrv,
//							_LPPIXELDATA lpPixDest,
//							LPRECT lprcDest,
//							_LPGDCDATA lpdcSrc,//_LPCDISPLAYDRV lpSrcDispDrv,
//							_LPPIXELDATA lpPixSrc,
//							LPRECT lprcSrc,
//							_LPPIXELDATA lpPixMask,   // if bltmask , it is valid
//							UINT uiMaskRopSrc,        // if bltmask , it is valid
//							UINT uiMaskRopDest        // if bltmask , it is valid
//							DWORD clrTransparent			// if == -1 , menau not transparent option invalid
//						 )

// 参数：
//	IN lpdcDest - 目标DC对象  //lpDestDispDrv - 目标驱动程序接口
//	IN lpPixDest - 目标点结构指针
//	IN lprcDest - 目标矩形
//	IN lpdcSrc - 源DC对象  //lpSrcDispDrv - 源驱动程序接口
//	IN lpPixSrc - 源点结构指针
//	IN lprcSrc - 源矩形
//	IN lpPixMask - 屏蔽点（假如有屏蔽位传送功能）   // if bltmask , it is valid
//	IN uiMaskRopSrc - 对源的屏蔽操作（假如有屏蔽位传送功能）       // if bltmask , it is valid
//	IN uiMaskRopDest - 对目标屏蔽操作 （假如有屏蔽位传送功能）      // if bltmask , it is valid
//	IN clrTransparent - 透明颜色，用于透明传输（如果为-1，非透明）	// if == -1 , not transparent option
//						假如 (clTransparent & 0x80000000) != 0, 
//						则说明 clTransparent 是表示一个在源位图的设备相关的颜色值，否则，是一个RGB值
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	在两个不同的位图格式之间传送位图
// 引用: 
//	
// ************************************************
#define DEBUG_PIXEL_TRANSFER 0
static BOOL PixelTransfer( _LPGDCDATA lpdcDest,//_LPCDISPLAYDRV lpDestDispDrv,
						   _LPPIXELDATA lpPixDest,
						   LPRECT lprcDest,

						   _LPGDCDATA lpdcSrc,//_LPCDISPLAYDRV lpSrcDispDrv,
						   _LPPIXELDATA lpPixSrc,
						   LPRECT lprcSrc,

						   _LPPIXELDATA lpPixMask,   // if bltmask , it is valid
						   UINT uiMaskRopSrc,        // if bltmask , it is valid
						   UINT uiMaskRopDest,        // if bltmask , it is valid
						   DWORD clrTransparent			// if not -1 , invalid
						 )
{
	_LPCDISPLAYDRV lpDestDispDrv = lpdcDest->lpDispDrv;
	_LPCDISPLAYDRV lpSrcDispDrv = lpdcSrc->lpDispDrv;
	int x, y;
	int width, height;
	int xSrcOffset, ySrcOffset, xDestOffset, yDestOffset;
	COLORREF clrPrev = -1;
	COLORREF clrCur, clrRgb = 0;
	_LPBITMAP_DIB lpSrcDIB = (_LPBITMAP_DIB)lpPixSrc->lpDestImage;
	WORD bmType = lpSrcDIB->bitmap.bmFlags & BF_DIB;
	DWORD * lpPal;
	UINT uiPalNum;
	int yDir = 1;
	UINT clrRealTransparent = -1;	//设备相关的颜色值
	
	if( bmType )
	{	//源位图为DIB格式
		lpPal = (DWORD*)&lpSrcDIB->palEntry[0];
		uiPalNum = lpSrcDIB->biClrUsed;
		yDir = lpSrcDIB->biDir;
		//颜色值为指向调色板的索引值
		if( lpSrcDIB->biCompression == BI_RGB )
		    bmType = PAL_INDEX;
		else if( lpSrcDIB->biCompression == BI_BITFIELDS )
			bmType = PAL_BITFIELD;  //颜色值为位图域
		else
			return FALSE;
	}
	//目标宽度和高度
	width = lprcDest->right - lprcDest->left;
	height = lprcDest->bottom - lprcDest->top;

	//RETAILMSG( 1, ( "PixelTransfer.\r\n" ) );
	//源起始位置在位图中的偏移
	xSrcOffset = lprcSrc->left;
	ySrcOffset = lprcSrc->top;
	//目标起始位置在位图中的偏移
	xDestOffset = lprcDest->left;
	yDestOffset = lprcDest->top;
	if( clrTransparent != -1 )
	{	//需要透明传输
		if( clrTransparent & TB_DEV_COLOR )
		{	//clrTransparent 是设备相关的值
			clrRealTransparent = clrTransparent & (~TB_DEV_COLOR);
		}
		else
		{	// clrTransparent是RGB值，将其转到设备相关的值
			if( bmType && uiPalNum )
			{
				clrRealTransparent = _Gdi_RealizeColor( clrTransparent, lpPal, uiPalNum, bmType );
			}
			else  //get default 
				clrRealTransparent = lpSrcDispDrv->lpRealizeColor( clrTransparent, NULL, 0, 0 );
		}
	}
		
	for( y = 0; y < height; y++ )
	{	// 每一行
		for( x = 0; x < width; x++ )
		{	// 每一列
			//2004.07-01 - remove, 多判断
			//if( bmType && yDir < 0 )
			if( yDir < 0 )	
			//
			{
			    lpPixSrc->y = lpSrcDIB->bitmap.bmHeight - (y + ySrcOffset) - 1;
			}
			else
			{
			    lpPixSrc->y = y + ySrcOffset;
			}
		    lpPixSrc->x = x + xSrcOffset;
			//得到源点设备相关颜色值
			clrCur = lpSrcDispDrv->lpGetPixel( lpPixSrc );
			if( clrCur != clrRealTransparent )
			{	//如果非透明色，则画之
				if( clrCur != clrPrev )
				{	//与之前的颜色值相同吗，如果不相同，则将其转化为
					//设备无关的值
					if( bmType && uiPalNum )
					{
						clrRgb = _Gdi_UnrealizeColor( clrCur, lpPal, uiPalNum, bmType );  
					}
					else
						clrRgb = lpSrcDispDrv->lpUnrealizeColor( clrCur, NULL, 0, 0 );
				}
				lpPixDest->x = x + xDestOffset;
				lpPixDest->y = y + yDestOffset;
				if( clrCur != clrPrev )
				{	//得到目标相关的值
					lpPixDest->color = lpDestDispDrv->lpRealizeColor( clrRgb, lpdcDest->lpdwPal, lpdcDest->wPalNumber, lpdcDest->wPalFormat );
				}
				//如果需要屏蔽操作，得到对应x,y坐标的屏蔽位
				if( lpPixMask )
				{
					lpPixMask->x = lpPixSrc->x;
					lpPixMask->y = lpPixSrc->y;
					if( _drvDisplay1BPP.lpGetPixel( lpPixMask ) )
						lpPixMask->rop = uiMaskRopSrc;
					else
						lpPixMask->rop = uiMaskRopDest;
				}
				//写点到目标位图
				lpDestDispDrv->lpPutPixel( lpPixDest );
				//缓存当前的颜色值
				clrPrev = clrCur;
			}
		}
	}
	return TRUE;
}


// **************************************************
// 声明：BOOL WINAPI WinGdi_BitBlt(
//							HDC hdcDest - 目标DC
//							int xDest - 目标x坐标
//							int yDest - 目标x坐标
//							int width, 
//							int height,
//							HDC hdcSrc, 
//							int xSrc, 
//							int ySrc, 
//							DWORD dwRop )
// 参数：
//	IN hdcDest - 目标DC
//	IN xDest - 目标矩形左上角X坐标
//	IN yDest - 目标矩形左上角Y坐标
//	IN width - 目标矩形宽度
//	IN height - 目标矩形高度
//	IN hdcSrc - 源DC
//	IN xSrc - 源矩形左上角X坐标
//	IN ySrc - 源矩形左上角Y坐标
//	IN dwRop - 光栅操作码,包括：
//			SRCCOPY-拷贝源矩形到目标矩形
//			SRCAND--源数据与目标数据做与运算(AND)
//			SRCINVERT-源数据与目标数据做异或运算(XOR)
//			SRCPAINT-源数据与目标数据做或运算(OR)
//			PATCOPY-拷贝模板到目标矩形
//			BLACKNESS-用黑色填充目标矩形
//			WHITENESS-用白色填充目标矩形
//			PATINVERT-模板颜色与目标颜色做异或运算(XOR)
//			DSTINVERT-反转目标颜色
//			NOTSRCCOPY-拷贝反转的源颜色到目标矩形 
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	从源DC向目标DC进行位图传送
// 引用: 
//	系统API
// ************************************************
						   
BOOL WINAPI WinGdi_BitBlt(HDC hdcDest, int xDest, int yDest, int width, int height,
				HDC hdcSrc, int xSrc, int ySrc, DWORD dwRop )
{
    _LPGDCDATA lpdcDest = _GetSafeDrawPtr( hdcDest );//得到目标DC对象指针
	_LPGDCDATA lpdcSrc = NULL;// = _GetSafeDrawPtr( hdcSrc );//得到源DC对象指针
    
    _LPRGNDATA lprgn;
    _LPRECTNODE lprnNode;
    _BLKBITBLT blt;
    RECT rcSrc, rcTemp, rcClip, rcDest;
    int xoff, yoff, xSrcOff, ySrcOff;
	BOOL retv = FALSE;

	// check param valid
	if( hdcSrc == NULL )
	{
		switch( dwRop )
		{
		case PATCOPY:
		case PATINVERT:
		case DSTINVERT:
		case BLACKNESS:
		case WHITENESS:
			lpdcSrc = NULL;
			break;
		default:
			goto _return;//return FALSE;   //错误参数
		}
	}
	else
	{
		lpdcSrc = _GetSafeDrawPtr( hdcSrc );//得到源DC对象指针
		if( lpdcSrc == NULL )
			goto _return;//return FALSE;
	}

    //if( lpdcDest && lpdcSrc && lpdcDest->lpDispDrv )
	if( lpdcDest && lpdcDest->lpDispDrv )
    {		
		RECT rcMask = { xDest, yDest, xDest + width, yDest + height };
        _LPCDISPLAYDRV lpDispDrv = lpdcDest->lpDispDrv;  //目标DC驱动程序
		lprgn = _GetHRGNPtr( lpdcDest->hrgn );
		//源/目标位图数据结构
        blt.lpDestImage = _GetHBITMAPPtr( lpdcDest->hBitmap );
		
		if( lpdcSrc )
			blt.lpSrcImage = _GetHBITMAPPtr( lpdcSrc->hBitmap );
		else
			blt.lpSrcImage = NULL;

		//当前刷子
		// 2005-09-15
        //blt.lpBrush = &lpdcSrc->brushAttrib;
		blt.lpBrush = &lpdcDest->brushAttrib;
		blt.lpptBrushOrg = &lpdcDest->ptBrushOrg;
		//

		//if( !(lprgn && blt.lpDestImage && blt.lpSrcImage && blt.lpBrush) )
		if( !(lprgn && blt.lpDestImage && blt.lpBrush) )
			goto _return;//goto _ERROR;

        blt.lprcSrc = &rcSrc;
        blt.lprcDest = &rcDest;
		blt.lprcMask = &rcMask;
        blt.dwRop = dwRop;
        // in bitblt, mono bitmap , 1 mean backcolor, 0 mean textcolor
        blt.solidColor = lpdcDest->backColor;
        blt.solidBkColor = lpdcDest->textColor;
        blt.backMode = lpdcDest->backMode;
		//当前源DC在显示设备上的偏移
		if( lpdcSrc )
		{
			xSrcOff = XOFFSET( lpdcSrc );
			ySrcOff = YOFFSET( lpdcSrc );
		}
		else
		{
			xSrcOff = 0;
			ySrcOff = 0;
		}

		//转换源坐标到设备坐标
        rcTemp.left = xSrc;
        rcTemp.top = ySrc;
        rcTemp.right = rcTemp.left + width;
        rcTemp.bottom = rcTemp.top + height;
        OffsetRect( &rcTemp, xSrcOff, ySrcOff );
		//当前目标DC在显示设备上的偏移
        xoff = XOFFSET( lpdcDest );
        yoff = YOFFSET( lpdcDest );
		//转换目标坐标到设备坐标
        rcClip.left = xDest;
        rcClip.top = yDest;
        rcClip.right = rcClip.left + width;
        rcClip.bottom = rcClip.top + height;
        OffsetRect( &rcClip, xoff, yoff );
		//得到源和目标的共同区域
        if( rcTemp.left < 0 )
        {
            rcClip.left -= rcTemp.left;
        }
        if( rcTemp.top < 0 )
        {
            rcClip.top -= rcTemp.top;
        }
		if( lpdcSrc )
		{
			if( rcTemp.right > blt.lpSrcImage->bmWidth )
			{
				rcClip.right -= rcTemp.right - blt.lpSrcImage->bmWidth;
			}
			if( rcTemp.bottom > blt.lpSrcImage->bmHeight )
			{
				rcClip.bottom -= rcTemp.bottom - blt.lpSrcImage->bmHeight;
			}
		}
		//源坐标在目标上的投射点相对偏移
		xoff = xSrc + xSrcOff - xDest - xoff;
		yoff = ySrc + ySrcOff - yDest - yoff;
		//目标裁剪域
        lprnNode = lprgn->lpNodeFirst;

        if( blt.lpSrcImage == NULL || 
			blt.lpDestImage->bmBits == blt.lpSrcImage->bmBits )
        {	//相同的显示面
			//遍历每一个裁剪域，如果可显示则绘制
            while( lprnNode )
            {	//得到内交区域
                if( IntersectRect( &rcDest, &rcClip, &lprnNode->rect ) )
                {
                    rcSrc = rcDest;
                    OffsetRect( &rcSrc, xoff, yoff );
					//设置绘制方向为从左到右，从上到下
                    blt.yPositive = 1;
                    blt.xPositive = 1;
					//
                    if( IntersectRect( &rcTemp, &rcSrc, &rcDest ) )
                    {	//因为是相同的显示面，所以必须确定正确的拷贝顺序
						//这里表示目标矩形与源矩形有重叠并且必须反向拷贝
                        if( rcSrc.top  < blt.lprcDest->top )
                            blt.yPositive = 0;	//从右到左
                        if( rcSrc.left  < blt.lprcDest->left )
                            blt.xPositive = 0;	//从下到上
                    }
                    lpDispDrv->lpBlkBitBlt( &blt );	//绘制到显示面
                }
                lprnNode = lprnNode->lpNext;//下一个裁剪区
            }
        }
        else
        {	//非同一个显示面
			if( ( blt.lpDestImage->bmBitsPixel == blt.lpSrcImage->bmBitsPixel ||
				  blt.lpSrcImage->bmBitsPixel == 1 ) )
			{	//数据格式相同或者源格式是黑白位图格式
				
				//设置绘制方向为从左到右，从上到下
				blt.yPositive = 1;
				blt.xPositive = 1;				
				//遍历每一个裁剪域，如果可显示则绘制
				while( lprnNode )
				{	//得到内交区域
					if( IntersectRect( &rcDest, &rcClip, &lprnNode->rect ) )
					{
						rcSrc = rcDest;
						OffsetRect( &rcSrc, xoff, yoff );
						lpDispDrv->lpBlkBitBlt( &blt );	//绘制到显示面
					}
					lprnNode = lprnNode->lpNext;	//下一个裁剪区
				}
			}
			else if( lpdcSrc->lpDispDrv )
			{   // 不同的位图格式， 需要对每一个点进行处理 ，点对点拷贝 pixel -> pixel
				// 这个将会大量的处理时间！
				_LPCDISPLAYDRV lpSrcDispDrv = lpdcSrc->lpDispDrv;
				_PIXELDATA pxSrc, pxDest;
				
	            pxSrc.lpDestImage = blt.lpSrcImage;
	            pxSrc.pattern = 0xff;
	            pxSrc.rop = R2_NOP;// read only

				pxDest.lpDestImage = blt.lpDestImage;
	            pxDest.pattern = 0xff;

				switch( dwRop )
				{
				case SRCCOPY:  // dest = src
					pxDest.rop = R2_COPYPEN;
					break;
				case SRCAND:   // dest = src & dst
					pxDest.rop = R2_MASKPEN;
					break;
				case SRCINVERT:  // dest = src ^ dst
					pxDest.rop = R2_MASKPEN;
					break;
				case SRCPAINT:  // dest = src | dst
					pxDest.rop = R2_MERGEPEN;
					break;
				case NOTSRCCOPY:  // dest = ~src
					pxDest.rop = R2_NOTCOPYPEN;
					break;
				case MERGEPAINT:  // dest = (~src) | dest
					pxDest.rop = R2_MERGENOTPEN;
					break;
				default:
					SetLastError( ERROR_INVALID_PARAMETER );
					goto _return;//goto _ERROR;
				}
				//遍历每一个裁剪域，如果可显示则绘制
				while( lprnNode )
				{	//得到内交区域
					if( IntersectRect( &rcDest, &rcClip, &lprnNode->rect ) )
					{
						rcSrc = rcDest;
						OffsetRect( &rcSrc, xoff, yoff );
						//对每个点进行转换
						//PixelTransfer( lpDispDrv, &pxDest, &rcDest, lpSrcDispDrv, &pxSrc, &rcSrc, NULL, 0, 0, -1 );
						PixelTransfer( lpdcDest, &pxDest, &rcDest, lpdcSrc, &pxSrc, &rcSrc, NULL, 0, 0, -1 );
					}
					lprnNode = lprnNode->lpNext;//下一个裁剪区
				}
			}
        }
        retv = TRUE;//return TRUE;
    }
//_ERROR:
_return:
	_LeaveDCPtr( lpdcDest, DCF_WRITE );
	_LeaveDCPtr( lpdcSrc, DCF_READ );
    return retv;//FALSE;
}



// **************************************************
// 声明：BOOL WINAPI WinGdi_TransparentBlt(
//							HDC hdcDest - 目标DC
//							int xDest - 目标x坐标
//							int yDest - 目标x坐标
//							int width, 
//							int height,
//							HDC hdcSrc, 
//							int xSrc, 
//							int ySrc, 
//							int widthSrc,
//							int heightSrc,
//							DWORD clTransparent )
// 参数：
//	IN hdcDest - 目标DC
//	IN xDest - 目标矩形左上角X坐标
//	IN yDest - 目标矩形左上角Y坐标
//	IN width - 目标矩形宽度
//	IN height - 目标矩形高度
//	IN hdcSrc - 源DC
//	IN xSrc - 源矩形左上角X坐标
//	IN ySrc - 源矩形左上角Y坐标
//	IN width - 源矩形宽度
//	IN height - 源矩形高度
//	IN clTransparent - 透明色,假如 (clTransparent & 0x80000000) != 0, 
//				则说明 clTransparent 是表示一个在源位图的设备相关的颜色值，否则，是一个RGB值
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	从源DC向目标DC进行透明位图传送
// 引用: 
//	系统API
// ************************************************
						   
BOOL WINAPI WinGdi_TransparentBlt(HDC hdcDest, 
								  int xDest, 
								  int yDest, 
								  int width, 
								  int height,
								  HDC hdcSrc,
								  int xSrc, 
								  int ySrc, 
								  int widthSrc,
								  int heightSrc,
								  DWORD clTransparent )
{
    _LPGDCDATA lpdcDest = _GetSafeDrawPtr( hdcDest );//得到目标DC对象指针
    _LPGDCDATA lpdcSrc = _GetSafeDrawPtr( hdcSrc );//得到源DC对象指针
    _LPRGNDATA lprgn;
    _LPRECTNODE lprnNode;
    _BLKBITBLT blt;
    RECT rcSrc, rcTemp, rcClip, rcDest;
    int xoff, yoff, xSrcOff, ySrcOff;
	BOOL retv = FALSE;

    if( lpdcDest && lpdcSrc && lpdcDest->lpDispDrv )
    {	
        _LPCDISPLAYDRV lpDispDrv = lpdcDest->lpDispDrv;  //目标DC驱动程序
		lprgn = _GetHRGNPtr( lpdcDest->hrgn );
		//源/目标位图数据结构
        blt.lpDestImage = _GetHBITMAPPtr( lpdcDest->hBitmap );
        blt.lpSrcImage = _GetHBITMAPPtr( lpdcSrc->hBitmap );
		//当前刷子
        blt.lpBrush = NULL;//&lpdcSrc->brushAttrib;
		blt.lpptBrushOrg = NULL;

		if( !(lprgn && blt.lpDestImage && blt.lpSrcImage) )
			goto _return;//goto _ERROR;

        blt.lprcSrc = &rcSrc;
        blt.lprcDest = &rcDest;
        blt.dwRop = SRCCOPY;
        // in bitblt, mono bitmap , 1 mean backcolor, 0 mean textcolor
        blt.solidColor = lpdcSrc->lpDispDrv->lpRealizeColor( clTransparent, lpdcSrc->lpdwPal, lpdcSrc->wPalNumber, lpdcSrc->wPalFormat );//lpdcDest->backColor;
        blt.solidBkColor = lpdcDest->textColor;
        blt.backMode = TRANSPARENT;//lpdcDest->backMode;
		//当前源DC在显示设备上的偏移
        xSrcOff = XOFFSET( lpdcSrc );
        ySrcOff = YOFFSET( lpdcSrc );
		//转换源坐标到设备坐标
        rcTemp.left = xSrc;
        rcTemp.top = ySrc;
        rcTemp.right = rcTemp.left + width;
        rcTemp.bottom = rcTemp.top + height;
        OffsetRect( &rcTemp, xSrcOff, ySrcOff );
		//当前目标DC在显示设备上的偏移
        xoff = XOFFSET( lpdcDest );
        yoff = YOFFSET( lpdcDest );
		//转换目标坐标到设备坐标
        rcClip.left = xDest;
        rcClip.top = yDest;
        rcClip.right = rcClip.left + width;
        rcClip.bottom = rcClip.top + height;
        OffsetRect( &rcClip, xoff, yoff );
		//得到源和目标的共同区域
        if( rcTemp.left < 0 )
        {
            rcClip.left -= rcTemp.left;
        }
        if( rcTemp.top < 0 )
        {
            rcClip.top -= rcTemp.top;
        }
        if( rcTemp.right > blt.lpSrcImage->bmWidth )
        {
            rcClip.right -= rcTemp.right - blt.lpSrcImage->bmWidth;
        }
        if( rcTemp.bottom > blt.lpSrcImage->bmHeight )
        {
            rcClip.bottom -= rcTemp.bottom - blt.lpSrcImage->bmHeight;
        }
		//源坐标在目标上的投射点相对偏移
        xoff = xSrc + XOFFSET( lpdcSrc ) - xDest - xoff;
        yoff = ySrc + YOFFSET( lpdcSrc ) - yDest - yoff;
		//目标裁剪域
        lprnNode = lprgn->lpNodeFirst;
		/* supprt later
        if( blt.lpDestImage->bmBits == blt.lpSrcImage->bmBits )
        {	//相同的显示面
			//遍历每一个裁剪域，如果可显示则绘制
            while( lprnNode )
            {	//得到内交区域
                if( IntersectRect( &rcDest, &rcClip, &lprnNode->rect ) )
                {
                    rcSrc = rcDest;
                    OffsetRect( &rcSrc, xoff, yoff );
					//设置绘制方向为从左到右，从上到下
                    blt.yPositive = 1;
                    blt.xPositive = 1;
					//
                    if( IntersectRect( &rcTemp, &rcSrc, &rcDest ) )
                    {	//因为是相同的显示面，所以必须确定正确的拷贝顺序
						//这里表示目标矩形与源矩形有重叠并且必须反向拷贝
                        if( rcSrc.top  < blt.lprcDest->top )
                            blt.yPositive = 0;	//从右到左
                        if( rcSrc.left  < blt.lprcDest->left )
                            blt.xPositive = 0;	//从下到上
                    }
                    lpDispDrv->lpBlkBitTransparentBlt( &blt );	//绘制到显示面
                }
                lprnNode = lprnNode->lpNext;//下一个裁剪区
            }
        }
        else
		*/
        {	//非同一个显示面
			/*	//support later
			if( ( blt.lpDestImage->bmBitsPixel == blt.lpSrcImage->bmBitsPixel ||
				  blt.lpSrcImage->bmBitsPixel == 1 ) )
			{	//数据格式相同或者源格式是黑白位图格式
				
				//设置绘制方向为从左到右，从上到下
				blt.yPositive = 1;
				blt.xPositive = 1;				
				//遍历每一个裁剪域，如果可显示则绘制
				while( lprnNode )
				{	//得到内交区域
					if( IntersectRect( &rcDest, &rcClip, &lprnNode->rect ) )
					{
						rcSrc = rcDest;
						OffsetRect( &rcSrc, xoff, yoff );
						lpDispDrv->lpBlkBitTransparentBlt( &blt );	//绘制到显示面
					}
					lprnNode = lprnNode->lpNext;	//下一个裁剪区
				}
			}
			else if( lpdcSrc->lpDispDrv )
			*/
			{   // 不同的位图格式， 需要对每一个点进行处理 ，点对点拷贝 pixel -> pixel
				// 这个将会大量的处理时间！
				_LPCDISPLAYDRV lpSrcDispDrv = lpdcSrc->lpDispDrv;
				_PIXELDATA pxSrc, pxDest;
				
	            pxSrc.lpDestImage = blt.lpSrcImage;
	            pxSrc.pattern = 0xff;
	            pxSrc.rop = R2_NOP;// read only

				pxDest.lpDestImage = blt.lpDestImage;
	            pxDest.pattern = 0xff;
				pxDest.rop = R2_COPYPEN;

				//遍历每一个裁剪域，如果可显示则绘制
				while( lprnNode )
				{	//得到内交区域
					if( IntersectRect( &rcDest, &rcClip, &lprnNode->rect ) )
					{
						rcSrc = rcDest;
						OffsetRect( &rcSrc, xoff, yoff );
						//对每个点进行转换
						PixelTransfer( lpdcDest, &pxDest, &rcDest, lpdcSrc, &pxSrc, &rcSrc, NULL, 0, 0, clTransparent );
					}
					lprnNode = lprnNode->lpNext;//下一个裁剪区
				}
			}
        }
        retv = TRUE;//return TRUE;
    }
//_ERROR:
    //return FALSE;
_return:
	_LeaveDCPtr( lpdcDest, DCF_WRITE );
	_LeaveDCPtr( lpdcSrc, DCF_READ );

	return retv;
}


// **************************************************
// 声明：
// 参数：
// 	IN 
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	
// 引用: 
//	
// ************************************************

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
// 声明：int WINAPI WinGdi_SetDIBitsToDevice(
//						 HDC hdc,                 // handle to DC
//						 int xDest,               // x-coord of destination upper-left corner
//						 int yDest,               // y-coord of destination upper-left corner 
//						 DWORD dwWidth,           // source rectangle width
//						 DWORD dwHeight,          // source rectangle height
//						 int xSrc,                // x-coord of source lower-left corner
//						 int ySrc,                // y-coord of source lower-left corner
//						 UINT uStartScan,         // first scan line in array
//						 UINT cScanLines,         // number of scan lines
//						 CONST VOID *lpvBits,     // array of DIB bits
//						 CONST BITMAPINFO *lpbmi, // bitmap information
//						 UINT fuColorUse          // RGB or palette indexes
//						 )
// 参数：
//	IN hdc – 目标DC
//	IN xDest – 输出到目标矩形的x坐标
//	IN yDest – 输出到目标矩形的y坐标
//	IN dwWidth – 输出到目标矩形的宽度
//	IN dwHeight – 输出到目标矩形的高度
//	IN xSrc – 源DIB点数据的开始x位置
//	IN ySrc – 源DIB点数据的开始y位置
//	IN uStartScan – DIB数据的开始扫描行
//	IN cScanLines – DIB数据的扫描行行数
//	IN lpvBits – 包含DIB数据的指针
//	IN lpbmi - BITMAPINFO结构指针，用于描述lpvBits的数据信息
//	IN fuColorUse - 说明BITMAPINFO结构成员bmiColors的类型：
//					DIB_PAL_COLORS – bmiColors为16索引数组，每一个索引值指向当前hdc的调色板的对应项 
//					DIB_RGB_COLORS – bmiColors为红、绿、蓝颜色数组
// 返回值：
//	假如成功，返回实际拷贝的扫描行数；否则，返回0
// 功能描述：
//	设置DIB数据到目标设备对应的矩形里
// 引用: 
//	系统API
// ************************************************
									
int WINAPI WinGdi_SetDIBitsToDevice(
						 HDC hdc,                 // handle to DC

						 int xDest,               // x-coord of destination upper-left corner
						 int yDest,               // y-coord of destination upper-left corner 
						 DWORD dwWidth,           // source rectangle width
						 DWORD dwHeight,          // source rectangle height
						 int xSrc,                // x-coord of source lower-left corner
						 int ySrc,                // y-coord of source lower-left corner

						 UINT uStartScan,         // first scan line in array
						 UINT cScanLines,         // number of scan lines
						 CONST VOID *lpvBits,     // array of DIB bits
						 CONST BITMAPINFO *lpbmi, // bitmap information
						 UINT fuColorUse          // RGB or palette indexes
						 )

{	// 得到DC对象指针
    _LPGDCDATA lpdc = _GetSafeDrawPtr( hdc );
	int retv = 0;

	if( lpdc && lpbmi )
	{
        _LPBITMAPDATA lpImage = NULL;//
		_LPCDISPLAYDRV lpDrv;
		UINT uiHeight = ABS(lpbmi->bmiHeader.biHeight);
		//得到DC显示面
		lpImage = _GetHBITMAPPtr( lpdc->hBitmap );		

	    lpDrv = lpdc->lpDispDrv;
	    if( lpDrv == NULL )
		    goto _return;//return 0;	//非法

		if( lpImage )
		{
			int iDir;
			// 源扫描行字节数
			int iScanLineBytes = (lpbmi->bmiHeader.biBitCount * lpbmi->bmiHeader.biWidth + 31) / 32 * 4;  // align to dword
			int iMinWidth, iMinHeight;
			RECT rcSrc, rcDst, rcTemp;
			int xoff = XOFFSET( lpdc );
			int yoff = YOFFSET( lpdc );
			_LPRGNDATA lprgn;
			_LPRECTNODE lprnNode;
            //颜色数据是从底到顶排列的吗 ？ 			
			if( lpbmi->bmiHeader.biHeight > 0 )
				iDir = -1;	//是从底到顶排列的
			else
				iDir = 1;
			//得到目标和源的相交区域		
			rcDst.left = xDest;
			rcDst.top = yDest;
			rcDst.right = xDest + dwWidth;
			rcDst.bottom = yDest + dwHeight;
			OffsetRect( &rcDst, xoff, yoff );
			rcTemp.left = 0;
			rcTemp.top = 0;
			rcTemp.right = lpImage->bmWidth;
			rcTemp.bottom = lpImage->bmHeight;
			//目标区域与目标显示面相交
			if( IntersectRect( &rcDst, &rcDst, &rcTemp ) )
			{	//有相交区域
				dwWidth = rcDst.right - rcDst.left;
				dwHeight = rcDst.bottom - rcDst.top;
				
				rcSrc.left = xSrc;
				rcSrc.top = ySrc;
				rcSrc.right = xSrc + dwWidth;
				rcSrc.bottom = ySrc + dwHeight;
				rcTemp.left = 0;
				rcTemp.top = 0;
				rcTemp.right = lpbmi->bmiHeader.biWidth;
				rcTemp.bottom = uiHeight;
				//源区域与源显示面矩形相交
				if( IntersectRect( &rcSrc, &rcSrc, &rcTemp ) )
				{	//有相交区域
					iMinWidth = rcSrc.right - rcSrc.left;
					iMinHeight = rcSrc.bottom - rcSrc.top;
					
					rcDst.right = rcDst.left + iMinWidth;
					rcDst.bottom = rcDst.top + iMinHeight;
				}
				else
					goto _return;	//没有相交区域，退出
			}
			else
				goto _return;	//没有相交区域，退出
			xoff = xSrc - xDest - xoff;
			yoff = ySrc - yDest - yoff;
			//得到目标的裁剪区域
			lprgn = _GetHRGNPtr( lpdc->hrgn );
			ASSERT( lprgn );
			//遍历目标的所有裁剪区域，如果有可绘制区域，则输出
			lprnNode = lprgn->lpNodeFirst;
			while( lprnNode )
			{
				RECT rcDest;
				if( IntersectRect( &rcDest, &rcDst, &lprnNode->rect ) )
				{	//相交
					rcSrc = rcDest;
					OffsetRect( &rcSrc, xoff, yoff );
					//转换位图数据到目标显示面
					retv = _WinGdi_ConvertImageColorValue( 
						lpDrv,
						(_LPBITMAP_DIB)lpImage,
						&rcDest,
						lpbmi,
						&rcSrc,
						uStartScan,
						cScanLines,
						iScanLineBytes,
						lpvBits,
						fuColorUse );
				}
				lprnNode = lprnNode->lpNext;	//下一个裁剪区
			}
		}
	}
_return:
	_LeaveDCPtr( lpdc, DCF_WRITE );
	return retv;
}

// **************************************************
// 声明：BOOL WINAPI WinGdi_Line( HDC hdc, int x0, int y0, int x1, int y1 )
// 参数：
//	IN hdc - DC句柄
//	IN x0 - 起点坐标X
//	IN y0 - 起点坐标Y
//	IN x1 - 结束坐标X
//	IN y1 - 结束坐标Y
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	画线段
// 引用: 
//	系统API
// ************************************************

BOOL WINAPI WinGdi_Line( HDC hdc, int x0, int y0, int x1, int y1 )
{
    POINT pt;
	BOOL retv;

    WinGdi_MoveTo( hdc, x0, y0, &pt );
    retv = WinGdi_LineTo( hdc, x1, y1 );
    WinGdi_MoveTo( hdc, pt.x, pt.y, 0 );
    return retv;
}

// **************************************************
// 声明：COLORREF WINAPI WinGdi_GetPixel( HDC hdc, int x, int y )
// 参数：
//	IN hdc - DC句柄
//	IN x - 点坐标X
//	IN y - 点坐标Y
// 返回值：
//	成功：返回RGB值
//	否则：返回CLR_INVALID
// 功能描述：
//	得到点颜色值
// 引用: 
//	系统API
// ************************************************

COLORREF WINAPI WinGdi_GetPixel( HDC hdc, int x, int y )
{
    _LPGDCDATA lpdc = _GetSafeDrawPtr( hdc );
	COLORREF retv = CLR_INVALID;

    if( lpdc && lpdc->lpDispDrv )
    {
        _PIXELDATA pixelData;
        pixelData.lpDestImage = _GetHBITMAPPtr( lpdc->hBitmap );
		//转化到目标DC设备坐标
		if( pixelData.lpDestImage )
		{
			pixelData.x = x + XOFFSET( lpdc );
			pixelData.y = y + YOFFSET( lpdc );
			//需要的点是否在显示面上 ？
			if( WinRgn_PtInRegion( lpdc->hrgn, pixelData.x, pixelData.y ) )
				retv = lpdc->lpDispDrv->lpUnrealizeColor( lpdc->lpDispDrv->lpGetPixel( &pixelData ), lpdc->lpdwPal, lpdc->wPalNumber, lpdc->wPalFormat );
		}
    }

	_LeaveDCPtr( lpdc, DCF_READ );
    return retv;//CLR_INVALID;
}

// **************************************************
// 声明：COLORREF WINAPI WinGdi_SetPixel( HDC hdc, int x, int y, COLORREF color )
// 参数：
//	IN hdc - 目标DC句柄
//	IN x - x坐标
//	IN y - y坐标
//	IN color - RGB颜色值
// 返回值：
//	返回实际写的RGB值, 失败，返回 -1
// 功能描述：
//	用给定的颜色写一个点
// 引用: 
//	系统API
// ************************************************

COLORREF WINAPI WinGdi_SetPixel( HDC hdc, int x, int y, COLORREF color )
{
    _LPGDCDATA lpdc = _GetSafeDrawPtr( hdc );//_GetHDCPtr( hdc );
	COLORREF retv = -1;

    if( lpdc )
    {
        _PIXELDATA pixelData;

        pixelData.lpDestImage = _GetHBITMAPPtr( lpdc->hBitmap );
		if( pixelData.lpDestImage )
		{
			pixelData.x = x + XOFFSET( lpdc );
			pixelData.y = y + YOFFSET( lpdc );
			pixelData.color = lpdc->lpDispDrv->lpRealizeColor(color, lpdc->lpdwPal, lpdc->wPalNumber, lpdc->wPalFormat);
			pixelData.pattern = 0;
			pixelData.rop = lpdc->rop;
			
			if( WinRgn_PtInRegion( lpdc->hrgn, pixelData.x, pixelData.y ) )
			{
				lpdc->lpDispDrv->lpPutPixel( &pixelData );
				retv = color;//return color;
			}
			//else
				//goto _return;//return -1;			
		}
    }
	_LeaveDCPtr( lpdc, DCF_WRITE );
    return retv;//-1;
}

// **************************************************
// 声明：static int Sqrt32( int x )
// 参数：
// 	IN x
// 返回值：
//	平方根
// 功能描述：
//	对 整数 x 求其平方根
// 引用: 
//	
// ************************************************

static int Sqrt32( int x )
{
    ULONG val, root, newroot, mask;

    root = 0;
    mask = 0x40000000L;
    val  = (ULONG)x;

    do
    {
		newroot = root + mask;
		if ( newroot <= val )
		{
			val -= newroot;
			root = newroot + mask;
		}
		
		root >>= 1;
		mask >>= 2;
		
    } while ( mask != 0 );

    return root;
}

#define EF_OUTLINE  0x0001
#define EF_FILL     0x0002
#define EF_ROUNDRECT     0x0004
// 每一个扫描行的最大最小结构
typedef struct _MINMAX
{
	int maxl;
	int minr;
}MINMAX;
//弧数据结构
typedef struct _ARC_DATA
{
    int nXStartArc; // 第一条射线的x结束点 x-coord of first radial ending point
    int nYStartArc; // 第一条射线的y结束点 y-coord of first radial ending point
	int nXEndArc;   // 第二条射线的x结束点 x-coord of second radial ending point
	int nYEndArc;    // 第二条射线的y结束点 y-coord of second radial ending point

    int cxStart;    // = (nXStartArc - x0)
    int cyStart;    // = (nYStartArc - y0)
    int cxEnd;      // = (nXEndArc - x0)
    int cyEnd;      // = (nYEndArc - y0)

    int iStartQuadrant; //开始象限（基于0）
    int iEndQuadrant;//结束象限（基于0）
    int iArcDirect;//  // 顺时针方向 = -1, 否则 = 1
	BOOL bInvert;   //  当 开始点与结束点的方向与 iArcDirect方向相反，则iInvert = TRUE
}ARC_DATA, * PARC_DATA;

#define DRAW_1_Q   0x01  //     1 象限  //quadrant
#define DRAW_2_Q   0x02  //     2 象限  //quadrant
#define DRAW_3_Q   0x04  //     3 象限  //quadrant
#define DRAW_4_Q   0x08  //     4 象限  //quadrant
#define DRAW_ALL_Q ( DRAW_1_Q | DRAW_2_Q | DRAW_3_Q | DRAW_4_Q )

// **************************************************
// 声明：static DWORD JudgeArcMiddlePoint( PARC_DATA pArc )
// 参数：
// 	IN pArc - ARC_DATA 结构指针
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	判断 弧的两个端点之间的象限（始终有效）
// 引用: 
//	
// ************************************************

//判断 弧的两个端点之间的象限（始终有效）
static DWORD JudgeArcMiddlePoint( PARC_DATA pArc )
{
	DWORD dwDrawMode = 0;
	int dir = pArc->iArcDirect;
	if( pArc->iEndQuadrant != pArc->iStartQuadrant )
	{
		int i;
		i = (pArc->iStartQuadrant + dir + 4) & 3;
		while( i != pArc->iEndQuadrant )
		{
			dwDrawMode |= (0x01 << i);
			i = (i + dir + 4) & 3;
		};
	}
	else
	{  
		// pArc->cyStart / pArc->cxStart >= pArc->cyEnd / pArc->cxEnd
		int iStart = pArc->cyStart * pArc->cxEnd;
		int iEnd = pArc->cyEnd * pArc->cxStart;
		if( iStart == iEnd )
			dwDrawMode = DRAW_ALL_Q;
		if(  (dir > 0 && iStart > iEnd) ||        // 顺时针方向
			 (dir < 0 && iStart < iEnd)  )        // 反时针方向
		{   //
			dwDrawMode = DRAW_ALL_Q & ( ~(0x1 << pArc->iStartQuadrant) );
		}
	}
	return dwDrawMode;
}

// **************************************************
// 声明：static DWORD JudgeArcEndPoint( 
//					PARC_DATA pArc, 
//				    int x0, //     中心点
//					int y0,
//					POINT points[4]
//					)
// 参数：
// 	IN pArc - ARC_DATA结构指针
//	IN x0 - 弧的中心点 x
//	IN y0 - 弧的中心点 y
//	IN points - 弧所在的矩形的四个点
// 返回值：
//	返回弧的两个端点所在象限
// 功能描述：
//  判断同弧的两个端点所在象限 相同的点是否有效
//	
// 引用: 
//	
// ************************************************


static DWORD JudgeArcEndPoint( PARC_DATA pArc, 
				    int x0, //     中心点
					int y0,
					POINT points[4]
					)
{
	int cxS = points[pArc->iStartQuadrant].x - x0;  //开始象限的点
	int cyS = points[pArc->iStartQuadrant].y - y0;
	int cxE = points[pArc->iEndQuadrant].x - x0;  //结束象限的点
	int cyE = points[pArc->iEndQuadrant].y - y0;
	DWORD dwDrawMode  = 0;
	int dir = pArc->iArcDirect;
	int bValidStart, bValidEnd;

	if( dir > 0 )
	{   // 顺时针方向
		//cyS / cxS >= pArc->cyStart / pArc->cxStart 
		bValidStart = ( cyS * pArc->cxStart >= pArc->cyStart * cxS );

		//cyE / cxE < pArc->cyEnd / pArc->cxEnd 
		bValidEnd = ( cyE * pArc->cxEnd < pArc->cyEnd * cxE );
	}
	else
	{   //反时针方向
		//cyS / cxS <= pArc->cyStart / pArc->cxStart 
		bValidStart = (cyS * pArc->cxStart <= pArc->cyStart * cxS);

		//cyE / cxE > pArc->cyEnd / pArc->cxEnd 
		bValidEnd = ( cyE * pArc->cxEnd > pArc->cyEnd * cxE );
	}
	
	if( pArc->iEndQuadrant == pArc->iStartQuadrant )
	{	//开始象限 = 结束象限
		if( !pArc->bInvert )
		{   //在同样的象限 并且 与 ArcDirect 是顺方向
			if( bValidStart && bValidEnd ) // 必须同时成立
				dwDrawMode |= 0x01 << pArc->iStartQuadrant;
			goto _return;
		}
	}
	if( bValidStart )
		dwDrawMode |= 0x01 << pArc->iStartQuadrant;
	if( bValidEnd )
		dwDrawMode |= 0x01 << pArc->iEndQuadrant;

_return:
	return dwDrawMode;
}

// **************************************************
// 声明：static BOOL DrawEllipseRect( 
//						_LPCDISPLAYDRV lpDispDrv, 
//		                _LPLINEDATA lpLineData,
//						_LPRECTNODE lprNodes,
//                      _LPBLKBITBLT lpBlkData,
//                      _FILLRGN * lpFillRgn,
//						int x0,   // ellips's origin x point 
//						int y0,   // ellips's origin y point 
//						int r1,   // ellips's origin a len 
//						int r2,   // ellips's origin b len
//						int lw,   // pen's left width
//						int rw,   // pen's right width
//						UINT uiFlag,
//						int nRectWidth,
//						int nRectHeight,
//						MINMAX * lpMinMax,
//						PARC_DATA pArc
//						)
// 参数：
//	IN lpDispDrv - 显示驱动程序
//	IN lpLineData - 画线数据结构
//	IN lprNodes - 裁剪域
//  IN lpBlkData - 块填充结构
//  IN lpFillRgn - 填充区域结构
//	IN x0 - 椭圆的x远点    // ellips's origin x point 
//	IN y0 - 椭圆的y远点    // ellips's origin y point 
//	IN r1 - 椭圆的 a 半径长度    // ellips's origin a len 
//	IN r2 - 椭圆的 b 半径长度   // ellips's origin b len
//	IN lw - 笔的左部分宽度,   // pen's left width
//	IN rw - 笔的右部分宽度,   // pen's right width
//	IN uiFlag - 画标志：
//				EF_FILL - 填充椭圆
//				EF_ROUNDRECT - 画矩形的圆弧轮廓
//				EF_OUTLINE - 画椭圆外轮廓
//	IN nRectWidth - 矩形宽度
//	IN nRectHeight - 矩形高度
//	IN lpMinMax - 最大最小结构，当画有一定宽度的轮廓线时，该结构保存轮廓线在扫描线上的最大/最小宽度值。
//	IN pArc - ARC_DATA结构指针，画弧（Arc）时有效
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	画椭圆
// 引用: 
//	该函数最终解决  Ellipse， RoundRect， Arc 的绘制问题
// ************************************************

static BOOL DrawEllipseRect( 
						_LPCDISPLAYDRV lpDispDrv, 
		                _LPLINEDATA lpLineData,
						_LPRECTNODE lprNodes,
                        _LPBLKBITBLT lpBlkData,
                        _FILLRGN * lpFillRgn,
						int x0,   // ellips's origin x point 
						int y0,   // ellips's origin y point 
						int r1,   // ellips's origin a len 
						int r2,   // ellips's origin b len
						int lw,   // pen's left width
						int rw,   // pen's right width
						UINT uiFlag,
						int nRectWidth,
						int nRectHeight,
						MINMAX * lpMinMax,
						PARC_DATA pArc
						)
{
	long rs;
	long r12, r22;
    int x, y, xmax, xyCur, yFillStart;
	long tn;
	LPFILLFUN lpFillFun;
	int l, r, t, b;
	MINMAX * lpCurSave, * lpEnd, * lpCurUse;
	int yPrev;
	int wdiff = rw - lw;
	DWORD dwMiddleMask;
	DWORD dwDrawMask = DRAW_ALL_Q;

	if( r1 + r2 == 0 )
		return TRUE;
	if( uiFlag & EF_FILL )
	{	//需要填充椭圆的内部
		if( lpBlkData->backMode == TRANSPARENT && lpBlkData->lpBrush->style != BS_SOLID )
			lpFillFun = _FillTransparentRgn;	//透明填充
		else
			lpFillFun = _FillRgn;
	}
	else
		lpFillFun = 0; 

	x = 0; y = r2;
	r12 = r1 * r1;
	r22 = r2 * r2;

	xmax = r12 / Sqrt32( r12 + r22 );
	//xmax = Sqrt32( (r12 * r12) / ( r12 + r22 ) );
	if( xmax )
		xmax++;

	tn = r12 - 2 * r2 * r12;
	xyCur = y + 1;
	yFillStart = y - lw;

	if( lw > 1 )
	{
	    yPrev = y;
	    lpEnd = lpMinMax + lw;
	    lpCurUse = lpCurSave = lpMinMax;
	}
	else
		lpCurUse = lpCurSave = NULL;
	if( pArc )
	{	//判断弧的中间部分所在的象限
		dwMiddleMask = JudgeArcMiddlePoint( pArc );
	}
	
	// 第一段弧
	while( x <= xmax )
	{
		if( tn < 0 || y == 0 )
			tn += ( 4 * x + 2 ) * r22;
		else
		{
			tn += ( 4 * x + 2 ) * r22 + ( 1 - y ) * 4 * r12;
			y--;
		}

		l = x0 - x;
		r = x0 + x + nRectWidth;
		t = y0 - y;
		b = y0 + y + nRectHeight;

		// 画外轮廓 draw outline
		if( uiFlag & EF_OUTLINE )
		{
			if( x == 0 && (uiFlag & EF_ROUNDRECT) )
			{	//画圆角矩形的最顶/最底上下部分
				_DrawLine( lpDispDrv, lprNodes, lpLineData, l, b + wdiff, r + 1, b + wdiff );//一象限
				_DrawLine( lpDispDrv, lprNodes, lpLineData, l, t, r + 1, t );//四象限
			}
			else
			{
				if( pArc && (dwMiddleMask != DRAW_ALL_Q) )
				{   //画弧
					POINT pts[4];

					pts[0].x = r;
					pts[0].y = b;
					pts[1].x = l;
					pts[1].y = b;
					pts[2].x = l;
					pts[2].y = t;					
					pts[3].x = r;
					pts[3].y = t;
					//得到弧的两个端点所在的象限
					dwDrawMask = dwMiddleMask | JudgeArcEndPoint( pArc, x0, y0, pts );
				}
				if( dwDrawMask & DRAW_1_Q )
				    _DrawLine( lpDispDrv, lprNodes, lpLineData, r, b, r, b - 1 );//一象限
				if( dwDrawMask & DRAW_2_Q )
				    _DrawLine( lpDispDrv, lprNodes, lpLineData, l, b, l, b - 1 );//二象限
				if( dwDrawMask & DRAW_4_Q )
				    _DrawLine( lpDispDrv, lprNodes, lpLineData, r, t, r, t + 1 );//四象限
				if( dwDrawMask & DRAW_3_Q )
				    _DrawLine( lpDispDrv, lprNodes, lpLineData, l, t, l, t + 1 );//三象限
			}
		}

		if( lpFillFun && 
			xyCur != y &&
			y <= yFillStart
          )
		{   //需要填充
			//准备填充数据
			if( lpCurUse )
			{
			    if( lpCurUse == lpEnd )
				    lpCurUse = lpMinMax;
			    lpFillRgn->rect.left = lpCurUse->maxl + lw;
			    lpFillRgn->rect.right = lpCurUse->minr + 1 - rw;
				lpCurUse++;
			}
			else
			{
			    lpFillRgn->rect.left = l + lw;
			    lpFillRgn->rect.right = r + 1 - rw;
			}
			lpFillRgn->rect.top = b;
			lpFillRgn->rect.bottom = b + 1;

			//填充一部分
            lpFillFun( lpDispDrv, lpBlkData, lpFillRgn );

			//填充对称的另一部分
			lpFillRgn->rect.top = t;
			lpFillRgn->rect.bottom = t + 1;

            lpFillFun( lpDispDrv, lpBlkData, lpFillRgn );
			xyCur = y;
		}

		if( yPrev != y && lpCurSave && lpFillFun )
		{   // 当笔的厚度对大于1时，这些数据有用。the data use when pen width > 1 and fill enable
			if( lpCurSave == lpEnd )
				lpCurSave = lpMinMax;
			lpCurSave->maxl = l;
			lpCurSave->minr = r;
			lpCurSave++;
			yPrev = y;
		}

		x++;
	}

	// 第二段弧
	yPrev = y;
	rs = r1; 
	r1 = r2;
	r2 = rs;    // 

	x = 0; y = r2;
	r12 = r1 * r1; 
	r22 = r2 * r2;
	xmax = r12 / Sqrt32( r12 + r22 ); 
	//xmax = Sqrt32( (r12 * r12) / ( r12 + r22 ) );

	xmax = MIN( xmax, yPrev-1 );
	tn = r12 - 2 * r2 * r12;

	if( lpCurSave == lpMinMax )
		lpCurSave = NULL;  // no save any data
	else if( lpCurSave )
	{
	    yPrev -= lw;
	    lpCurSave--;
	}

	while( x <= xmax )
	{
		if( tn < 0 || y == 0 )
			tn += ( 4 * x + 2 ) * r22;
		else
		{
			tn += ( 4 * x + 2 ) * r22 + ( 1 - y ) * 4 * r12;
			y--;
		}

		l = x0 - y;
		r = x0 + y + nRectWidth;
		t = y0 - x;
		b = y0 + x + nRectHeight;

		if( uiFlag & EF_OUTLINE )
		{	// 画外轮廓
            if( x == 0 && (uiFlag & EF_ROUNDRECT) )
			{	//画圆角矩形的最顶/最底上下部分
				_DrawLine( lpDispDrv, lprNodes, lpLineData, r, t, r, b + 1 );//一象限
				_DrawLine( lpDispDrv, lprNodes, lpLineData, l, t, l, b + 1 );//四象限
			}
			else
			{
                if( pArc && (dwMiddleMask != DRAW_ALL_Q) )
				{   //画弧
					POINT pts[4];

					pts[0].x = r;
					pts[0].y = b;
					pts[1].x = l;
					pts[1].y = b;
					pts[2].x = l;
					pts[2].y = t;
					pts[3].x = r;
					pts[3].y = t;
					//得到弧的两个端点所在的象限					
					dwDrawMask = dwMiddleMask | JudgeArcEndPoint( pArc, x0, y0, pts );
				}
				if( dwDrawMask & DRAW_1_Q )
				    _DrawLine( lpDispDrv, lprNodes, lpLineData, r, b, r, b - 1 );//一象限
                if( dwDrawMask & DRAW_4_Q )
				    _DrawLine( lpDispDrv, lprNodes, lpLineData, r, t, r, t + 1 );//四象限
                if( dwDrawMask & DRAW_2_Q )
					_DrawLine( lpDispDrv, lprNodes, lpLineData, l, b, l, b - 1 );//二象限
                if( dwDrawMask & DRAW_3_Q )
					_DrawLine( lpDispDrv, lprNodes, lpLineData, l, t, l, t + 1 );//三象限
			}
		}
		//填充
		if( lpFillFun )
		{	//需要填充内部
			if( lpCurSave == NULL || x <= yPrev )
			{
				lpFillRgn->rect.left = l + lw;
				lpFillRgn->rect.top = b;
				lpFillRgn->rect.right = r + 1 - rw;
				lpFillRgn->rect.bottom = b + 1;
                //RETAILMSG( 1, ( "2,l=%d,t=%d,r=%d,b=%d.\r\n",lpFillRgn->rect.left,lpFillRgn->rect.top,lpFillRgn->rect.right,lpFillRgn->rect.bottom ) );
				//填充一部分
				lpFillFun( lpDispDrv, lpBlkData, lpFillRgn );				
				
				lpFillRgn->rect.top = t;				
				lpFillRgn->rect.bottom = t + 1;
				//RETAILMSG( 1, ( "3,l=%d,t=%d,r=%d,b=%d.\r\n",lpFillRgn->rect.left,lpFillRgn->rect.top,lpFillRgn->rect.right,lpFillRgn->rect.bottom ) );
				//填充对称的另一部分
				lpFillFun( lpDispDrv, lpBlkData, lpFillRgn );
			}
			else
			{	//因为外轮廓已经绘制了扫描线的一部分，因此只需要填充扫描线的局部
				lpFillRgn->rect.left = lpCurSave->maxl + lw;
				lpFillRgn->rect.top = b;
				lpFillRgn->rect.right = lpCurSave->minr + 1 - rw;
				lpFillRgn->rect.bottom = b + 1;
				//填充一部分
				lpFillFun( lpDispDrv, lpBlkData, lpFillRgn );
    			if( lpCurSave == lpMinMax )
   				    lpCurSave = lpEnd - 1;
				else
				    lpCurSave--;
				
				lpFillRgn->rect.top = t;
				lpFillRgn->rect.bottom = t + 1;
				//RETAILMSG( 1, ( "4,l=%d,t=%d,r=%d,b=%d.\r\n",lpFillRgn->rect.left,lpFillRgn->rect.top,lpFillRgn->rect.right,lpFillRgn->rect.bottom ) );
				//填充对称的另一部分
				lpFillFun( lpDispDrv, lpBlkData, lpFillRgn );
			}
		}

		x++;	
	}
	if( lpFillFun && 
		(uiFlag & EF_ROUNDRECT) )
	{	//如果是绘制的圆角矩形，这里绘制中间的矩形部分
		lpFillRgn->rect.left = x0 - rs + lw;
		lpFillRgn->rect.top = y0 + 1;
		lpFillRgn->rect.right = x0 + nRectWidth + rs - rw + 1;
		lpFillRgn->rect.bottom = y0 + nRectHeight;

		//RETAILMSG( 1, ( "5,l=%d,t=%d,r=%d,b=%d.\r\n",lpFillRgn->rect.left,lpFillRgn->rect.top,lpFillRgn->rect.right,lpFillRgn->rect.bottom ) );
        lpFillFun( lpDispDrv, lpBlkData, lpFillRgn );
	}

	return TRUE;
}


// **************************************************
// 声明：static BOOL EllipseRect(
//						   HDC hdc, 
//						   int left, // Ellipse's left else RoundRect' left
//						   int top, // Ellipse's top .....
//						   int right, // Ellipse's right .....
//						   int bottom,// Ellipse's bottom .....
//						   int nWidth, // if bRoundRect == TRUE, rect width .....
//						   int nHeight, //if bRoundRect == TRUE, rect height .....
// 						   BOOL bRoundRect,
// 						   PARC_DATA pArc
//						   )
// 参数：
//	IN hdc - 绘图DC
//	IN left - 矩形的左, // Ellipse's left else RoundRect' left
//	IN top - 矩形的顶 // Ellipse's top .....
//	IN right - 矩形的右 // Ellipse's right .....
//	IN bottom - 矩形的底 // Ellipse's bottom .....
//	IN nWidth - 矩形的宽度 // if bRoundRect == TRUE, rect width .....
//	IN nHeight - 矩形的高度 //if bRoundRect == TRUE, rect height .....
//	IN bRoundRect - 是否是圆角矩形
//	IN pArc - 当绘制 arc 时有效
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	绘制 Ellipse, RoundRect Arc
// 引用: 
//	
// ************************************************

///////////////////////////////////
//          rect width           //
//    .--------------------> .   //
//    ^                          //
//    |                          //
//    |                          //
//    | rect height              //
//    .                      .   //
//                               //
///////////////////////////////////
static BOOL EllipseRect(
						   HDC hdc, 
						   int left, // Ellipse's left else RoundRect' left
						   int top, // Ellipse's top .....
						   int right, // Ellipse's right .....
						   int bottom,// Ellipse's bottom .....
						   int nWidth, // if bRoundRect == TRUE, rect width .....
						   int nHeight, //if bRoundRect == TRUE, rect height .....
   						   BOOL bRoundRect,
   						   PARC_DATA pArc
						   )
{
    _LPGDCDATA lpdc;
	BOOL retv = FALSE;
	//得到安全的 DC指针
    lpdc = _GetSafeDrawPtr( hdc );
    if( lpdc && lpdc->lpDispDrv )
    {	
        _LPRECTNODE lprNodes;
        _LPBITMAPDATA lpBitmap;
		_LINEDATA lineData;
		_LPPENATTRIB lpPenAttrib;
        _FILLRGN aFillRgn;
        _BLKBITBLT blkData;
        int hw, xoffset, yoffset;
		_LPBRUSHATTRIB lpBrushAttrib;
		UINT uiFlag = 0;
		int x0, y0, ra, rb, lw,rw;
		_LPCDISPLAYDRV lpDispDrv = lpdc->lpDispDrv;
		RECT rcBrushMask = { left, top, right, bottom }; //2005-09-15 , add

		//裁剪域
        lprNodes = _GetHRGNPtr( lpdc->hrgn )->lpNodeFirst;
        if( lprNodes == 0 )
		{
			retv = TRUE;
            goto _return;//return TRUE;
		}
		//显示面
        lpBitmap = _GetHBITMAPPtr( lpdc->hBitmap );
        if( lpBitmap == 0 )
		{
            goto _return;//return FALSE;
		}
		// 将逻辑坐标转化为设备坐标 make origin point
        xoffset = XOFFSET( lpdc );
        yoffset = YOFFSET( lpdc );
        left += xoffset;
        top += yoffset;
        right += xoffset;
        bottom += yoffset;

		ra = (right - left - 1) / 2;
		rb = (bottom - top - 1) / 2;
		x0 = left + ra;
		y0 = top + rb;
		if( bRoundRect )
		{  //修正 nWidth & nHeight
			nWidth += (right - left) - (ra * 2 + 1);
			nHeight += (bottom - top) - (rb * 2 + 1);
		}

		// get pen data
		lpPenAttrib = &lpdc->penAttrib;

        if( lpPenAttrib->pattern != PS_NULL )
        {	//准备笔结构
            lineData.backMode = lpdc->backMode;
            lineData.clrBack = lpdc->backColor;
            lineData.color = lpPenAttrib->color;
            lineData.lpDestImage = lpBitmap;
            lineData.pattern = lpPenAttrib->pattern;
            lineData.rop = lpdc->rop;
            lineData.width = lpPenAttrib->width;
			lineData.lpfnCallback = NULL;
			uiFlag |= EF_OUTLINE;
        }

		lw = rw = 0;

		lpBrushAttrib = &lpdc->brushAttrib;

		if( pArc == NULL && 
			lpBrushAttrib->style != BS_NULL )
        {   // 准备填充结构 do fill
            if( lpPenAttrib->pattern != PS_NULL && lpPenAttrib->width >= 1 )
            {				
                hw = lpPenAttrib->width;
				lw = WIDTH_RIGHT( hw )+1;
				rw = WIDTH_LEFT( hw )+1;
            }
            aFillRgn.lprNodes = lprNodes;

            blkData.lpDestImage = lpBitmap;
            blkData.lpBrush = lpBrushAttrib;
			blkData.lpptBrushOrg = &lpdc->ptBrushOrg;
			//2005-09-15, modify
			//blkData.lprcMask = &aFillRgn.rect;
			blkData.lprcMask = &rcBrushMask;
			//
            blkData.solidBkColor = lpdc->backColor;
            blkData.backMode = lpdc->backMode;
            blkData.dwRop = PATCOPY;

			uiFlag |= EF_FILL;

        }
		if( uiFlag )
		{	//需要绘制...
			if( pArc )
			{   // arc 功能
				//准备 ARC 结构
				pArc->nXStartArc += xoffset;
				pArc->nYStartArc += yoffset;
				pArc->nXEndArc += xoffset;
				pArc->nYEndArc += yoffset;

				pArc->cxStart = pArc->nXStartArc - x0;
				pArc->cyStart = pArc->nYStartArc - y0;
				pArc->cxEnd = pArc->nXEndArc - x0;
				pArc->cyEnd = pArc->nYEndArc - y0;

				if( pArc->cxStart >= 0 && pArc->cyStart >= 0 )
				    pArc->iStartQuadrant = 0;  // 1 象限
				else if( pArc->cxStart <= 0 && pArc->cyStart >= 0 )
				    pArc->iStartQuadrant = 1;  // 2 象限
				else if( pArc->cxStart <= 0 && pArc->cyStart <= 0 )
				    pArc->iStartQuadrant = 2;  // 3 象限
				else if( pArc->cxStart >= 0 && pArc->cyStart <= 0 )
				    pArc->iStartQuadrant = 3;  // 4 象限
				else
				{
					ASSERT( 0 );
				}
				if( pArc->cxEnd >= 0 && pArc->cyEnd >= 0 )
				    pArc->iEndQuadrant = 0;  // 1 象限
				else if( pArc->cxEnd <= 0 && pArc->cyEnd >= 0 )
				    pArc->iEndQuadrant = 1;  // 2 象限
				else if( pArc->cxEnd <= 0 && pArc->cyEnd <= 0 )
				    pArc->iEndQuadrant = 2;  // 3 象限
				else if( pArc->cxEnd >= 0 && pArc->cyEnd <= 0 )
				    pArc->iEndQuadrant = 3;  // 4 象限
				else
				{
					ASSERT( 0 );
				}
				//绘制方向（顺时针或反时针）
				pArc->iArcDirect = (lpdc->arcMode == AD_COUNTERCLOCKWISE) ? -1 : 1;
				
				//  当 开始点与结束点的方向也 iArcDirect方向相反，则iInvert = TRUE
				pArc->bInvert = FALSE;
				if( pArc->iStartQuadrant != pArc->iEndQuadrant )
				{
					if( ( pArc->iArcDirect < 0 &&
						pArc->iStartQuadrant < pArc->iEndQuadrant ) ||
						( pArc->iArcDirect > 0 &&
						pArc->iStartQuadrant > pArc->iEndQuadrant ) )
					{
						pArc->bInvert = TRUE;
					}					
				}
				else
				{
					int iStart = pArc->cyStart * pArc->cxEnd;
					int iEnd = pArc->cyEnd * pArc->cxStart;
					if(  (pArc->iArcDirect > 0 && iStart > iEnd) ||        // 顺时针方向
						(pArc->iArcDirect < 0 && iStart < iEnd)  )        // 反时针方向
					{
						pArc->bInvert = TRUE;
					}
				}
				
				DrawEllipseRect( lpDispDrv, &lineData, lprNodes, &blkData, &aFillRgn, x0, y0, ra, rb, lw, rw, uiFlag, nWidth, nHeight, NULL, pArc );
			}
			else
			{   // 椭圆 或 圆角椭圆 epllise or round rect
				if( bRoundRect )
					uiFlag |= EF_ROUNDRECT;
				//当笔有厚度时，必须准备一个临时缓存去保留外轮廓坐标，例如：
				// [***]        ........................... scan1   
				// [****]       ........................... scan2
				// [****]       ........................... scan3
				//  [*****]     ........................... scan4
				//   [******]   ........................... scan5

				if( lw <= 16 )
				{	//快速，用 sp 
					MINMAX minmax[16];
					DrawEllipseRect( lpDispDrv, &lineData, lprNodes, &blkData, &aFillRgn, x0, y0, ra, rb, lw, rw, uiFlag, nWidth, nHeight, &minmax[0], NULL );
				}
				else
				{	//慢速, 动态
					MINMAX * lpminmax = malloc( lw * sizeof( MINMAX ) );
					if( lpminmax )
					{
						DrawEllipseRect( lpDispDrv, &lineData, lprNodes, &blkData, &aFillRgn, x0, y0, ra, rb, lw, rw, uiFlag, nWidth, nHeight, lpminmax, NULL );
						free( lpminmax );
					}
				}
			}
		}
        
		retv = TRUE;//return TRUE;
    }
_return:
    //return FALSE;
	_LeaveDCPtr( lpdc, DCF_WRITE );
	return retv;
}

// **************************************************
// 声明：BOOL WINAPI WinGdi_Ellipse(
//						   HDC hdc, 
//						   int left, 
//						   int top, 
//						   int right, 
//						   int bottom
//						   )

// 参数：
// 	IN hdc - 绘图DC
//	IN left - 包围椭圆的矩形 左
//	IN top - 包围椭圆的矩形 顶
//	IN right - 包围椭圆的矩形 右
//	IN bottom - 包围椭圆的矩形 底
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	画椭圆，椭圆的中心是矩形的中心，椭圆的轮廓用当前笔来绘制并用当前刷子来填充
// 引用: 
//	系统API
// ************************************************

BOOL WINAPI WinGdi_Ellipse(
						   HDC hdc, 
						   int left, 
						   int top, 
						   int right, 
						   int bottom
						   )
{
    int tmp;

	//检查参数
	if( left == right || top == bottom )
		return TRUE;
	if( left > right )
	{
		tmp = left;
		left = right;
		right = tmp;
	}
	if(  top > bottom )
	{
		tmp = top;
		top = bottom;
		bottom = tmp;
	}
	return EllipseRect( hdc, left, top, right, bottom, 0, 0, FALSE, NULL );
}

// **************************************************
// 声明：BOOL WINAPI WinGdi_RoundRect(
//						   HDC hdc, 
//						   int left, 
//						   int top, 
//						   int right, 
//						   int bottom,
//						   int nWidth,   // ellipse width
//						   int nHeight   // ellipse height
//						   )
// 参数：
//	IN hdc - DC句柄
//	IN left - 矩形的左上角x坐标
//	IN top - 矩形的左上角y坐标
//	IN right - 矩形的右下角x坐标
//	IN bottom -矩形的右下角y坐标
//	IN nWidth – 圆角的宽度
//	IN nHeight - 圆角的高度
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	画圆叫角矩形
// 引用: 
//	
// ************************************************

BOOL WINAPI WinGdi_RoundRect(
						   HDC hdc, 
						   int left, 
						   int top, 
						   int right, 
						   int bottom,
						   int nWidth,   // ellipse width
						   int nHeight   // ellipse height
						   )
{
    int tmp;
	//检查参数
	if( left == right || top == bottom )
		return TRUE;
	
	if( left > right )
	{
		tmp = left;
		left = right;
		right = tmp;
	}
	if(  top > bottom )
	{
		tmp = top;
		top = bottom;
		bottom = tmp;
	}
	tmp = right - left;
	if( nWidth > tmp )
		nWidth = tmp;
	tmp = bottom - top;
	if( nHeight > tmp )
		nHeight = tmp;
	if( nWidth && nHeight )
	    return EllipseRect( hdc, left, top, left + nWidth, top + nHeight, right - left - nWidth, bottom - top - nHeight, TRUE, NULL );
	else
		return WinGdi_Rectangle( hdc, left, top, right, bottom );
}

// **************************************************
// 声明：BOOL WINAPI WinGdi_Arc(
//				HDC hdc,
//				int left,
//				int top,
//				int right,
//				int bottom,
//				int nXStartArc,
//				int nYStartArc,
//				int nXEndArc,
//				int nYEndArc
//				)

// 参数：
//	IN hdc - DC句柄
//	IN left – 矩形的左上x坐标
//	IN top – 矩形的左上y坐标
//	IN right – 矩形的右下x坐标
//	IN bottom – 矩形的右下y坐标
//	IN nXStartArc – 起始射线的结束点x坐标
//	IN nYStartArc – 起始射线的结束点y坐标
//	IN nXEndArc – 终止射线的结束点x坐标
//	IN nYEndArc – 终止射线的结束点y坐标
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	画椭圆弧
// 引用: 
//	
// ************************************************

BOOL WINAPI WinGdi_Arc(
				HDC hdc,
				int left,
				int top,
				int right,
				int bottom,
				int nXStartArc,
				int nYStartArc,
				int nXEndArc,
				int nYEndArc
				)
{
	int tmp;
	ARC_DATA arcData;
	//检查参数	
	if( left == right || top == bottom )
		return TRUE;
	if( left > right )
	{
		tmp = left;
		left = right;
		right = tmp;
	}
	if(  top > bottom )
	{
		tmp = top;
		top = bottom;
		bottom = tmp;
	}
	arcData.nXStartArc = nXStartArc;
	arcData.nYStartArc = nYStartArc;
	arcData.nXEndArc = nXEndArc;
	arcData.nYEndArc = nYEndArc;

	return EllipseRect( hdc, left, top, right, bottom, 0, 0, FALSE, &arcData );
}


//x is main advance
//y = ( initerror + up * x - down ) / ( up - down )
//ierror = -( y * ( up - down ) - initerror - up * x )
//ierror = initerror + up * x - y * ( up - down );
//minx = (y * (up - down) - initerror + down + (up-1) ) / up;
//maxx = ((y+1) * (up - down) - initerror + down + (up-1)) / up - 1;


// 4\5|6/7
//-----+------ 
// 3/2|1\0


// **************************************************
// 声明：static BOOL _NewClipLine( 
//						_LINEDATA * lpLine, 
//						int iInitError, 
//						int x0, 
//						int y0, 
//						int dx, 
//						int dy, 
//						LPRECT lprcClip )
// 参数：
// 	IN/OUT lpLine - 线结构数据指针,用于接受裁剪后的线段
//	IN iInitError - 误差
//	IN x0 - 线段的起点x0
//	IN y0 - 线段的起点y0
//	IN dx - 线段的x方向长度
//	IN dy - 线段的y方向长度
//	IN lprcClip - 裁剪矩形
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	在矩形内裁剪线段
// 引用: 
//	
// ************************************************

static BOOL _NewClipLine( _LINEDATA * lpLine, int iInitError, int x0, int y0, int dx, int dy, LPRECT lprcClip )
{
	int dm = ABS( dx );
	int dn = ABS( dy );
	int x1 = x0 + dx;
	int y1 = y0 + dy;
	int left, top, right, bottom;
	int k, i;
	int x, y;
	int count, sum, testv;
	int minv, maxv;
	int yDir = dy > 0 ? 1 : -1;
	int xDir = dx > 0 ? 1 : -1;
	POINT pt[6];
	int sums[6];

	int iAdjUp = (long)(lpLine->dn);
	int iAdjDown = (long)(lpLine->dn) - (long)(lpLine->dm);

	left = lprcClip->left;
	top = lprcClip->top;
	right = lprcClip->right - 1;
	bottom = lprcClip->bottom - 1;
	count = 0;

	if( x0 >= left && x0 <= right &&
		y0 >= top && y0 <= bottom )
	{
		pt[count].x = x0;
		pt[count].y = y0;
		sums[count] = 0;
		count++;
	}

	if( x1 >= left && x1 <= right &&
		y1 >= top && y1 <= bottom )
	{
		pt[count].x = x1;
		pt[count].y = y1;
		sums[count] = max( dm, dn );
		count++;
	}
	if( count == 2 )
	{
		lpLine->cPels = max( dm, dn );
		lpLine->xStart = x0;
		lpLine->yStart = y0;
		return TRUE;
	}

	if( dm > dn )
	{   // x is main
		// test left and right
		testv = left;		
		for( i = 0; i < 2; i++ )
		{
			sum = (testv - x0) * xDir;
			if( sum >= 0 && sum <= dm )
			{				
				y = (int)( iInitError + iAdjUp * sum - iAdjDown ) / (int)( iAdjUp - iAdjDown );
				y = y0 + y * yDir;
				if( y >= top && y <= bottom )
				{// ok , the 
					sums[count] = sum;
					if( (xDir < 0 && testv == left) ||
						(xDir > 0 && testv == right) )
					{  // leave point
						sums[count]++;
					}
					pt[count].x = testv;
					pt[count].y = y;
					count++;
				}
			}
			// test right
			testv = right;			
		}
		// test top and bottom
		testv = top;		
		for( i = 0; i < 2 && count < 3 ; i++ )
		{
			sum = (testv - y0) * yDir;
			if( sum >= 0 && sum <= dn )
			{
				minv = (int)( sum * ( iAdjUp - iAdjDown ) - iInitError + iAdjDown + ( iAdjUp-1) ) / iAdjUp;
				if( minv < 0 )
					minv = 0;		
				maxv = (int)( (sum+1) * ( iAdjUp - iAdjDown ) - iInitError + iAdjDown + ( iAdjUp-1) ) / iAdjUp - 1;
				//minv += 1;
				if( xDir < 0 )
				{
					k = minv;
					minv = maxv;
					maxv = k;				
				}
				
				minv = x0 + minv * xDir;
				maxv = x0 + maxv * xDir;
				minv = MAX( left, minv );
				maxv = MIN( right, maxv );
				
				if(  minv <= maxv )
				{
					if( (yDir > 0 && testv == bottom) ||
						(yDir < 0 && testv == top) )
					{  // leave point, get max
						if( xDir < 0 )
						{
							pt[count].x = minv;//maxv;
							sums[count] = x0 - minv;//x0 - maxv;
						}
						else
						{
							pt[count].x = maxv;//minv;
							sums[count] = maxv - x0;//minv - x0;
						}
						sums[count]++;
					}
					else
					{ // enter point, get min
						if( xDir < 0 )
						{
							pt[count].x = maxv;
							sums[count] = x0 - maxv;
						}
						else
						{
							pt[count].x = minv;
							sums[count] = minv - x0;
						}
					}
					pt[count].y = testv;
					
					count++;
				}
			}
			// test bottom
			testv = bottom;			
		}
	}
	else
	{   // y is main
		// test top and bottom
		testv = top;		
		for( i = 0; i < 2; i++ )
		{
			sum = (testv - y0) * yDir;
			if( sum >= 0 && sum <= dn )
			{		
				x = (int)( iInitError + iAdjUp * sum - iAdjDown ) / (int)( iAdjUp - iAdjDown );
				x = x0 + x * xDir;
				if( x >= left && x <= right )
				{// ok , the 
                    sums[count] = sum;
					if( (yDir < 0 && testv == top) ||
						(yDir > 0 && testv == bottom) )
					{  // leave point
						sums[count]++;
					}
					pt[count].y = testv;
					pt[count].x = x;
					count++;
				}
			}
			// test bottom
			testv = bottom;
		}

		// test left and right
		testv = left;		
		for( i = 0; i < 2 && count < 3; i++ )
		{
			sum = (testv - x0) * xDir;
			if( sum >= 0 && sum <= dm )
			{
				minv = (int)( sum * ( iAdjUp - iAdjDown ) - iInitError + iAdjDown + ( iAdjUp-1) ) / iAdjUp;
				if( minv < 0 )
					minv = 0;		
				maxv = (int)( (sum+1) * ( iAdjUp - iAdjDown ) - iInitError + iAdjDown + ( iAdjUp-1) ) / iAdjUp - 1;
				//minv += 1;
				if( yDir < 0 )
				{
					k = minv;
					minv = maxv;
					maxv = k;
				}
				minv = y0 + minv * yDir;
				maxv = y0 + maxv * yDir;
				minv = MAX( top, minv );
				maxv = MIN( bottom, maxv );
				
				if(  minv <= maxv )
				{
					if( (xDir > 0 && testv == right) ||
						(xDir < 0 && testv == left) )
					{   // leave point, get max
						if( yDir < 0 )
						{
							pt[count].y = minv;//maxv;
							sums[count] = y0 - minv;//x0 - maxv;
						}
						else
						{
							pt[count].y = maxv;//minv;
							sums[count] = maxv - y0;//minv - x0;
						}
						sums[count]++;
					}
					else
					{ // enter point, get min
						if( yDir < 0 )
						{
							pt[count].y = maxv;
							sums[count] = y0 - maxv;
						}
						else
						{
							pt[count].y = minv;
							sums[count] = minv - y0;
						}
					}

					pt[count].x = testv;
					count++;
				}
			}
			// test right
			testv = right;
		}
	}
	if( count >= 2 )
	{
		int maxsum, minsum;
		maxsum = minsum = sums[0];		
		maxv = minv = 0;
		for( k = 1; k < count; k++ )
		{
			if( minsum > sums[k] )
			{
				minsum = sums[k];
				minv = k;
			}
			else if( maxsum < sums[k] )
			{
				maxsum = sums[k];				
				maxv = k;
			}
		}
		if( dm > dn )
		{
		    k = pt[minv].y - y0;
			k = ABS( k );
		}
		else
		{
		    k = pt[minv].x - x0;
			k = ABS( k );			
		}
		lpLine->iErrorCount = iInitError + iAdjUp * minsum - k * ( iAdjUp - iAdjDown );

		lpLine->xStart = (short)pt[minv].x;
		lpLine->yStart = (short)pt[minv].y;
		lpLine->cPels = maxsum-minsum;
		return TRUE;
	}
	return FALSE;
}

// **************************************************
// 声明：static BOOL _DrawLine( 
//					_LPCDISPLAYDRV lpDispDrv, 
//					_LPRECTNODE lprn, 
//					_LINEDATA * lpLine, 
//					int x0, 
//					int y0, 
//					int x1, 
//					int y1 )
// 参数：
//	IN lpDispDrv - 显示驱动程序
//	IN lprn - 裁剪节点
//	IN lpLine - 线结构数据
//	IN x0 - 线段的起点x
//	IN y0 - 线段的起点y
//	IN x1 - 线段的终点x
//	IN y1 - 线段的终点y
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	在不同裁剪区域上画线
// 引用: 
//	
// ************************************************

// 4\5|6/7
// ---+---
// 3/2|1\0

static BOOL _DrawLine( _LPCDISPLAYDRV lpDispDrv, _LPRECTNODE lprn, _LINEDATA * lpLine, int x0, int y0, int x1, int y1 )
{
	int dx = x1 - x0;
	int dy = y1 - y0;
	int dm, dn;
	int iInitErrorCount;	
	
	int k;
	int half;
	int width;

	// init iDir dn dm iErrorCount
	//2005-09-29, add code by lilin
	width = lpLine->width;
	half = (width+1) >> 1;
	
	//

	if( dx == 0 || dy == 0 )
	{
		lpLine->dn = 0;
		if( dx == 0 )
			lpLine->iDir = dy > 0 ? 1 : 6;
		else// dy == 0
			lpLine->iDir = dx > 0 ? 0 : 3;
	}
	else 
	{
		dm = ABS( dx );
		dn = ABS( dy );
		k = dm > dn;
		if( k )
		{
			lpLine->dn = dn;
			lpLine->dm = dm;
		}
		else
		{
			lpLine->dn = dm;
			lpLine->dm = dn;
		}
		lpLine->dn += lpLine->dn;
        iInitErrorCount = lpLine->iErrorCount = lpLine->dn - lpLine->dm;
		lpLine->dm += lpLine->dm;

		if( dx > 0 )
		{
			if( dy > 0 )
				lpLine->iDir = k ? 0 : 1;
			else
				lpLine->iDir = k ? 7 : 6;
		}
		else if( dx < 0 )
		{
			if( dy > 0 )
				lpLine->iDir = k ? 3 : 2;
			else
				lpLine->iDir = k ? 4 : 5;
		}
		else if( dy > 0 )
		{
			if( dx > 0 )
				lpLine->iDir = k ? 0 : 1;
			else
				lpLine->iDir = k ? 3 : 2;
		}
		else
		{ // dy < 0 
			if( dx > 0 )
				lpLine->iDir = k ? 7 : 6;
			else
				lpLine->iDir = k ? 4 : 5;
		}
	}

	// 在每一个裁剪区域上画线 now draw line in clip region

	while( lprn )
	{
		lpLine->lprcClip = &lprn->rect;
		if( dx == 0 )
		{   // 垂直线 vert line
			// 对宽线处理不对, 2005-09-29
			BOOL bDrawLine;
			if( width > 1 )
			{
				//RECT rc = { x0 - half, y0 - half, x0 + half, y0 + half };
				//bDrawLine = IntersectRect( &rc, &rc, lprn->rect );
				int minx = x0 - half;
				int maxx = x0 + half;
				//minx = MIN( minx, lprn->rect.left );
				minx = MAX( minx, lprn->rect.left );
				maxx = MIN( maxx, lprn->rect.right );
				bDrawLine = ( minx < maxx );

			}
			else
			{
				bDrawLine = ( x0 >= lprn->rect.left && x0 < lprn->rect.right );
			}

			if( bDrawLine )
			{
				int t, b;
				int tc, bc;
				if( dy > 0 )
				{
					if( width > 1 )
					{
						tc = y0 - half; bc = y1 + half;
					}
					else
					{	
						tc = y0; bc = y1;						
					}
					t = y0; b = y1;
				}
				else
				{
					if( width > 1 )
					{
						tc = y1 + 1 - half;
						bc = y0 + 1 + half;
					}					
					else
					{
						tc = y1 + 1; bc = y0 + 1;
					}
					
					t = y1 + 1; b = y0 + 1;
				}
				tc = MAX( tc, lprn->rect.top );
				bc = MIN( bc, lprn->rect.bottom );
				if( tc < bc )
				{
					t = MAX( t, lprn->rect.top );
					b = MIN( b, lprn->rect.bottom );

					lpLine->xStart = x0;
					if( dy > 0 )
						lpLine->yStart = t;
					else
						lpLine->yStart = b - 1;
					lpLine->cPels = b - t;
					lpDispDrv->lpLine( lpLine );
				}
			}
		}
		else if( dy == 0 )
		{   // 水平线 hori line
			// 对宽线处理不对, 2005-09-29
			BOOL bDrawLine;
			if( width > 1 )
			{
				//RECT rc = { x0 - half, y0 - half, x1 + half, y0 + half };
				//int minx = x0 - half;
				//int maxx = x0 + half;
				int miny = y0 - half;
				int maxy = y0 + half;
				//minx = MIN( minx, lprn->rect.left );
				miny = MAX( miny, lprn->rect.top );
				maxy = MIN( maxy, lprn->rect.bottom );
				bDrawLine = ( miny < maxy );
			}
			else
			{
				bDrawLine = ( y0 >= lprn->rect.top && y0 < lprn->rect.bottom );
			}

			if( bDrawLine )
			{
				int l, r;
				if( dx > 0 )
				{
					l = x0; r = x1;
				}
				else
				{
					l = x1 + 1; r = x0 + 1;
				}
				l = MAX( l, lprn->rect.left );
				r = MIN( r, lprn->rect.right );
				if( l < r )
				{
					lpLine->yStart = y0;
					if( dx > 0 )
						lpLine->xStart = l;
					else
						lpLine->xStart = r - 1;
					lpLine->cPels = r - l;
					lpDispDrv->lpLine( lpLine );
				}
			}
		}
		else
		{   // 斜线 dia line			
			if( _NewClipLine( lpLine, iInitErrorCount, x0, y0, dx, dy, &lprn->rect ) )
			{				
				lpDispDrv->lpLine( lpLine );
			}
			
		}
		
		lprn = lprn->lpNext;	//下一个裁剪矩形
	}
	return TRUE;

}

// **************************************************
// 声明：BOOL WINAPI WinGdi_LineTo( HDC hdc, int x1, int y1 )
// 参数：
//	IN hdc-目标DC
//	IN x1-线结束点X坐标
//	IN y1-线结束点Y坐标
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	画线到结束点,但不包含该点
//	假如调用成功, current position(CP)值移到结束点
// 引用: 
//	系统API
// ************************************************

BOOL WINAPI WinGdi_LineTo( HDC hdc, int x1, int y1 )
{	//得到安全 DC
    _LPGDCDATA lpdc = _GetSafeDrawPtr( hdc );
	BOOL retv = FALSE;

    if( lpdc && lpdc->lpDispDrv )
    {
        _LPRGNDATA lprn = _GetHRGNPtr( lpdc->hrgn );
        _LPBITMAPDATA lpBitmap = _GetHBITMAPPtr( lpdc->hBitmap );
        _LINEDATA lineData;
        _LPPENATTRIB lpPenAttrib;
        int xoffset = XOFFSET( lpdc );
        int yoffset = YOFFSET( lpdc );
        int x0 = lpdc->position.x;
        int y0 = lpdc->position.y;

		// update cp
        lpdc->position.x = x1;
        lpdc->position.y = y1;

		//逻辑坐标到设备坐标
        // make origin point
        x0 += xoffset;
        y0 += yoffset;
        x1 += xoffset;
        y1 += yoffset;

        lpPenAttrib = &lpdc->penAttrib;//_GetHPENPtr( lpdc->hPen );
		//设置线数据		
        lineData.backMode = lpdc->backMode;
        lineData.clrBack = lpdc->backColor;
        lineData.color = lpPenAttrib->color;
        lineData.lpDestImage = lpBitmap;
        lineData.pattern = lpPenAttrib->pattern;
        lineData.rop = lpdc->rop;
        lineData.width = lpPenAttrib->width;
		lineData.lpfnCallback = NULL;
		//绘制线段		      
        _DrawLine( lpdc->lpDispDrv, lprn->lpNodeFirst, &lineData, x0, y0, x1, y1 );
        retv = TRUE;//return TRUE;
    }
	_LeaveDCPtr( lpdc, DCF_WRITE );
    return retv;////return FALSE;
}

// **************************************************
// 声明：BOOL WINAPI WinGdi_MoveTo( HDC hdc, int x, int y, LPPOINT lppt )
// 参数：
//	IN hdc-目标句柄
//	IN x-新当前位置X坐标
//	IN y-新当前位置Y坐标
//	OUT lppt-接收之前设置的原点,POINT结构指针
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	设置当前位置
// 引用: 
//	系统 API
// ************************************************

BOOL WINAPI WinGdi_MoveTo( HDC hdc, int x, int y, LPPOINT lppt )
{
    _LPGDCDATA lpdc = _GetHDCPtr( hdc );

    if( lpdc )
    {
        // save old pos
        if( lppt )
            *lppt = lpdc->position;
        lpdc->position.x = x;
        lpdc->position.y = y;
        return TRUE;
    }
    return FALSE;
}

// **************************************************
// 声明：BOOL WINAPI WinGdi_Polyline( HDC hdc, const POINT* lppts, int nCount )
// 参数：
//	IN hdc-目标DC
//	IN lpcPoints-包含x,y坐标对的点数组,POINT结构指针
//	IN nCount-点数组个数
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	将许多点以顺序连成线段
// 引用: 
//	系统API 
// ************************************************

BOOL WINAPI WinGdi_Polyline( HDC hdc, const POINT* lppts, int nCount )
{
    int  i;
    POINT pt;
    if( nCount >= 2 )
    {
        WinGdi_MoveTo( hdc, lppts->x, lppts->y, &pt );
		lppts++;
		//画每一个点
        for( i = 1; i < nCount; i++, lppts++ )
	        WinGdi_LineTo( hdc, lppts->x, lppts->y );
        WinGdi_MoveTo( hdc, pt.x, pt.y, 0 );
	    return TRUE;
    }
    return FALSE;
}

// **************************************************
// 声明：static void _FillRgn( _LPCDISPLAYDRV lpDispDrv, _LPBLKBITBLT lpFillData, _FILLRGN * lpFillRgn )
// 参数：
// 	IN lpDispDrv - 显示驱动程序
//	IN lpFillData - 块操作结构
//	IN lpFillRgn - 裁剪域结构
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	在不同的裁剪域填充图形
// 引用: 
//	
// ************************************************

static void _FillRgn( _LPCDISPLAYDRV lpDispDrv, _LPBLKBITBLT lpFillData, _FILLRGN * lpFillRgn )
{
    register const _RECTNODE FAR* lprNodes = lpFillRgn->lprNodes;
    RECT rectClip;
	//
    lpFillData->lprcDest = &rectClip;
	//遍历每一个裁剪域
    while( lprNodes )
    {
	    if( IntersectRect( &rectClip, &lprNodes->rect, &lpFillRgn->rect ) )   // not empty
            lpDispDrv->lpBlkBitBlt( lpFillData );
	    lprNodes = lprNodes->lpNext;
    }
}

// **************************************************
// 声明：static void _FillTransparentRgn( _LPCDISPLAYDRV lpDispDrv, _LPBLKBITBLT lpFillData, _FILLRGN * lpFillRgn )
// 参数：
// 	IN lpDispDrv - 显示驱动程序
//	IN lpFillData - 块操作结构
//	IN lpFillRgn - 裁剪域结构
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	在不同的裁剪域用透明方式填充图形
// 引用: 
//	
// ************************************************

static void _FillTransparentRgn( _LPCDISPLAYDRV lpDispDrv, _LPBLKBITBLT lpFillData, _FILLRGN * lpFillRgn )
{
    register const _RECTNODE FAR* lprNodes = lpFillRgn->lprNodes;
    RECT rectClip;

    lpFillData->lprcDest = &rectClip;
	//遍历每一个裁剪域
    while( lprNodes )
    {
	    if( IntersectRect( &rectClip, &lprNodes->rect, &lpFillRgn->rect ) )   // not empty
            lpDispDrv->lpBlkBitTransparentBlt( lpFillData );
	    lprNodes = lprNodes->lpNext;
    }
}

// **************************************************
// 声明：BOOL WINAPI WinGdi_DrawFocusRect( HDC hdc, LPCRECT lprc )
// 参数：
//	IN hdc-目标DC
//	IN lprc-边框,RECT结构指针
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	画聚焦边框
// 引用: 
//	系统API
// ************************************************

BOOL WINAPI WinGdi_DrawFocusRect( HDC hdc, LPCRECT lprc )
{	//得到安全的DC
    _LPGDCDATA lpdc = _GetSafeDrawPtr( hdc );
	BOOL retv = FALSE;

	if( lpdc && lpdc->lpDispDrv )
    {
        _LPRECTNODE lprNodes;
        _LPBITMAPDATA lpBitmap;
        _LINEDATA lineData;
		_LPCDISPLAYDRV lpDispDrv = lpdc->lpDispDrv;
        int xoffset, yoffset;
        int left, right, top, bottom;
		//得到显示面
        lpBitmap = _GetHBITMAPPtr( lpdc->hBitmap );
		//得到裁剪域
        lprNodes = _GetHRGNPtr( lpdc->hrgn )->lpNodeFirst;

        xoffset = XOFFSET( lpdc );
        yoffset = YOFFSET( lpdc );
        left = lprc->left + xoffset;
        top = lprc->top + yoffset;
        right = lprc->right + xoffset;
        bottom = lprc->bottom + yoffset;
		//准备画线数据
        lineData.backMode = TRANSPARENT;
        lineData.clrBack = lpdc->backColor;
        lineData.color = lpDispDrv->lpRealizeColor( CL_LIGHTGRAY, lpdc->lpdwPal, lpdc->wPalNumber, lpdc->wPalFormat );
        lineData.lpDestImage = lpBitmap;
        lineData.pattern = 0x55;
        lineData.rop = R2_XORPEN;
        lineData.width = 1;
		lineData.lpfnCallback = NULL;
		//画四边
        _DrawLine( lpdc->lpDispDrv, lprNodes, &lineData, left, top, right, top );        
        _DrawLine( lpdc->lpDispDrv, lprNodes, &lineData, right-1, top, right-1, bottom );
        _DrawLine( lpdc->lpDispDrv, lprNodes, &lineData, left, top, left, bottom );        
        _DrawLine( lpdc->lpDispDrv, lprNodes, &lineData, left, bottom - 1, right, bottom - 1 );

        retv = TRUE;//return TRUE;
    }
	_LeaveDCPtr( lpdc, DCF_WRITE );
    return retv;//FALSE;
}

// **************************************************
// 声明：BOOL WINAPI WinGdi_Rectangle( HDC hdc, int left, int top, int right, int bottom )
// 参数：
//	IN hdc-目标DC
//	IN left-目标矩形左上X
//	IN top-目标矩形左上Y
//	IN right-目标矩形右下X
//	IN bottom-目标矩形右下Y
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	绘制矩形---用当前笔画边框,用当前刷子填充边框内部
// 引用: 
//	系统 API
// ************************************************

BOOL WINAPI WinGdi_Rectangle( HDC hdc, int left, int top, int right, int bottom )
{
    _LPGDCDATA lpdc;
	BOOL retv = FALSE;

    lpdc = _GetSafeDrawPtr( hdc );
    if( lpdc && lpdc->lpDispDrv )
    {
        _LPRECTNODE lprNodes;
        _LPBITMAPDATA lpBitmap;
		_LPPENATTRIB lpPenAttrib;
        _FILLRGN aFillRgn;
        _BLKBITBLT blkData;
        int hw, xoffset, yoffset;
		_LPBRUSHATTRIB lpBrushAttrib;
		_LPCDISPLAYDRV lpDispDrv = lpdc->lpDispDrv;
		RECT rcBrushMask; //2005-09-15 , add

		//检查参数有效性
        if( left == right || top == bottom )
            goto _return;//return FALSE;
		//是否对调参数 ？
        if( left > right )
        {
            hw = left;
            left = right;
            right = hw;
        }
        if(  top > bottom )
        {
            hw = top;
            top = bottom;
            bottom = hw;
        }
		//裁剪域
        lprNodes = _GetHRGNPtr( lpdc->hrgn )->lpNodeFirst;
        if( lprNodes == 0 )
		{
			retv = TRUE;
			goto _return;//            return TRUE;
		}
		//显示域
        lpBitmap = _GetHBITMAPPtr( lpdc->hBitmap );
        if( lpBitmap == 0 )
		{
            goto _return;//return FALSE;
		}
		rcBrushMask.left = left;
		rcBrushMask.top = top;
		rcBrushMask.right = right;
		rcBrushMask.bottom = bottom;
		// 逻辑点到设备点 make origin point
        xoffset = XOFFSET( lpdc );
        yoffset = YOFFSET( lpdc );
        left += xoffset;
        top += yoffset;
        right += xoffset;
        bottom += yoffset;
		// get pen data        
		lpPenAttrib = &lpdc->penAttrib;

        if( lpPenAttrib->pattern != PS_NULL )
        {
            _LINEDATA lineData;
            lineData.backMode = lpdc->backMode;
            lineData.clrBack = lpdc->backColor;
            lineData.color = lpPenAttrib->color;
            lineData.lpDestImage = lpBitmap;
            lineData.pattern = lpPenAttrib->pattern;
            lineData.rop = lpdc->rop;
            lineData.width = lpPenAttrib->width;
			lineData.lpfnCallback = NULL;
            // 画边框
            _DrawLine( lpdc->lpDispDrv, lprNodes, &lineData, left, top, right, top );    
            _DrawLine( lpdc->lpDispDrv, lprNodes, &lineData, right-1, top, right-1, bottom );
            _DrawLine( lpdc->lpDispDrv, lprNodes, &lineData, left, top, left, bottom );
            _DrawLine( lpdc->lpDispDrv, lprNodes, &lineData, left, bottom - 1, right, bottom - 1 );
        }
		lpBrushAttrib = &lpdc->brushAttrib;

        if( lpBrushAttrib->style != BS_NULL )
        {   // 填充内部 do fill
            if( lpPenAttrib->pattern != PS_NULL && lpPenAttrib->width >= 1 )
            {	//根据边框厚度校正内部矩形的大小
                hw = lpPenAttrib->width;

                left += WIDTH_RIGHT( hw )+1;
                top += WIDTH_BOTTOM( hw )+1;
                right -= WIDTH_LEFT( hw )+1;
                bottom -= WIDTH_TOP( hw )+1;
            }
			//准备填充数据
            aFillRgn.rect.left = left;
            aFillRgn.rect.top = top;
            aFillRgn.rect.right = right;
            aFillRgn.rect.bottom = bottom;
            aFillRgn.lprNodes = lprNodes;

            blkData.lpDestImage = lpBitmap;
            blkData.lpBrush = lpBrushAttrib;
			blkData.lpptBrushOrg = &lpdc->ptBrushOrg;
			//2005-09-15, modify
			//blkData.lprcMask = &aFillRgn.rect;
			blkData.lprcMask = &rcBrushMask;//&aFillRgn.rect;
			//
            blkData.solidBkColor = lpdc->backColor;
            blkData.backMode = lpdc->backMode;
            blkData.dwRop = PATCOPY;
            //填充
			if( blkData.backMode == TRANSPARENT && lpBrushAttrib->style != BS_SOLID )  
                _FillTransparentRgn( lpDispDrv, &blkData, &aFillRgn );
            else
                _FillRgn( lpDispDrv, &blkData, &aFillRgn );
        }
        retv = TRUE;//return TRUE;
    }
_return:
	_LeaveDCPtr( lpdc, DCF_WRITE );
    return retv;//FALSE;
}

// **************************************************
// 声明：BOOL WINAPI WinGdi_FillRect( HDC hdc, LPCRECT lpRect, HBRUSH hBrush )
// 参数：
//	IN hdc-目标DC
//	IN lpRect-目标矩形,RECT结构指针
//	IN hBrush-刷子
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	用刷子填充矩形
// 引用: 
//	系统 API
// ************************************************

BOOL WINAPI WinGdi_FillRect( HDC hdc, LPCRECT lpRect, HBRUSH hBrush )
{	//得到安全 DC
    _LPGDCDATA lpdc = _GetSafeDrawPtr( hdc );
	BOOL retv = FALSE;

	if( hBrush && lpdc && lpdc->lpDispDrv )
    {
        _FILLRGN aFillRgn;
        _LPBITMAPDATA lpBitmap;
        _BLKBITBLT blkData;
		_LPBRUSHDATA lpBrush;
		_BRUSHATTRIB brushAttrib;
		//如果是系统颜色刷，得到真实的句柄
		if( (UINT)hBrush <= (UINT)SYS_COLOR_NUM )
			hBrush = WinSys_GetColorBrush( (int)hBrush - 1 );
	    lpBrush = _GetHBRUSHPtr( hBrush );		
        lpBitmap = _GetHBITMAPPtr( lpdc->hBitmap );
        if( lpBitmap && lpBrush )
        {	//拷贝刷子对象
			brushAttrib = lpBrush->brushAttrib;
			//得到设备相关颜色
			brushAttrib.color = lpdc->lpDispDrv->lpRealizeColor( brushAttrib.clrRef, lpdc->lpdwPal, lpdc->wPalNumber, lpdc->wPalFormat );

			aFillRgn.rect = *lpRect;
			// 逻辑坐标到设备坐标 make origin point
            OffsetRect( &aFillRgn.rect, XOFFSET( lpdc ), YOFFSET( lpdc ) );
			// do fill
            aFillRgn.lprNodes = _GetHRGNPtr( lpdc->hrgn )->lpNodeFirst;
			// 准备刷子数据
            blkData.lpDestImage = lpBitmap;
            blkData.lpBrush = &brushAttrib;
			blkData.lpptBrushOrg = &lpdc->ptBrushOrg;
			////2005-09-15, modify
			//blkData.lprcMask = &aFillRgn.rect;  
			blkData.lprcMask = lpRect;
			//
            blkData.solidBkColor = lpdc->backColor;
            blkData.dwRop = PATCOPY;
			// 填充
            _FillRgn( lpdc->lpDispDrv, &blkData, &aFillRgn );
            retv = TRUE;//return TRUE;
        }
    }
	_LeaveDCPtr( lpdc, DCF_WRITE );
    return retv;//FALSE;
}

// **************************************************
// 声明：BOOL WINAPI WinGdi_FillRgn( HDC hdc, HRGN hrgn, HBRUSH hBrush )
// 参数：
//	IN hdc-目标DC
//	IN hrgn-目标区域
//	IN hBrush-刷子
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	用给定的刷子填充给定的区域
// 引用: 
//	系统 API
// ************************************************

BOOL WINAPI WinGdi_FillRgn( HDC hdc, HRGN hrgn, HBRUSH hBrush )
{	//得到安全 DC
    _LPGDCDATA lpdc = _GetSafeDrawPtr( hdc );
	BOOL retv = FALSE;

    if( hBrush && hrgn && lpdc && lpdc->lpDispDrv )
    {
        _FILLRGN aFillRgn;
        _BLKBITBLT blkData;
		RECT rcBrushMask;

        HRGN hrgnClip;
        _LPBITMAPDATA lpBitmap;
		_LPBRUSHDATA lpBrush;
		//得到显示面
        lpBitmap = _GetHBITMAPPtr( lpdc->hBitmap );
        if( lpBitmap )
        {	//拷贝一个新的显示区域
			WinRgn_GetBox( hrgn, &rcBrushMask ); //2005-09-15, add
            hrgnClip = WinRgn_CreateRect( 0, 0, 0, 0 );
            WinRgn_Combine( hrgnClip, hrgn, 0, RGN_COPY );
			//转化到设备坐标
            WinRgn_Offset( hrgnClip, XOFFSET( lpdc ), YOFFSET( lpdc ) );
			//与DC的裁剪域相交
            WinRgn_Combine( hrgnClip, hrgnClip, lpdc->hrgn, RGN_AND );
			//得到最大的裁剪域的矩形
            WinRgn_GetBox( hrgnClip, &aFillRgn.rect );
            aFillRgn.lprNodes = _GetHRGNPtr( hrgnClip )->lpNodeFirst;			
			//准备块传输数据
            blkData.lpDestImage = lpBitmap;			
            blkData.lpBrush = ( lpBrush = _GetHBRUSHPtr( hBrush ) ) ? &lpBrush->brushAttrib : NULL;
			blkData.lpptBrushOrg = &lpdc->ptBrushOrg;
			// 2005-09-15, modify by ln
			//blkData.lprcMask = &aFillRgn.rect;
			blkData.lprcMask = &rcBrushMask;
			//
            blkData.solidBkColor = lpdc->backColor;
            blkData.dwRop = PATCOPY;
			//填充
            _FillRgn( lpdc->lpDispDrv, &blkData, &aFillRgn );
            WinGdi_DeleteObject( hrgnClip );
            retv = TRUE;//return TRUE;
        }
    }
	_LeaveDCPtr( lpdc, DCF_WRITE );
    return retv;//FALSE;
}

// **************************************************
// 声明：BOOL WINAPI WinGdi_InvertRect( HDC hdc, LPCRECT lpcRect )
// 参数：
//	IN hdc-目标DC句柄
//	IN hrgn-目标矩形
//	IN lpcRect-RECT结构指针
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	反转目标矩形
// 引用: 
//	系统API
// ************************************************

BOOL WINAPI WinGdi_InvertRect( HDC hdc, LPCRECT lpcRect )
{
    return WinGdi_PatBlt( hdc, lpcRect->left, lpcRect->top, lpcRect->right-lpcRect->left, lpcRect->bottom-lpcRect->top, DSTINVERT );
}

// **************************************************
// 声明：BOOL WINAPI WinGdi_InvertRgn( HDC hdc, HRGN hrgn )
// 参数：
// 	IN hdc-目标DC
//	IN hrgn-目标区域
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	反转目标区域
// 引用: 
//	系统API
// ************************************************

BOOL WINAPI WinGdi_InvertRgn( HDC hdc, HRGN hrgn )
{	//得到安全 DC
    _LPGDCDATA lpdc = _GetSafeDrawPtr( hdc );//
	BOOL retv = FALSE;

    if( hrgn && lpdc && lpdc->lpDispDrv )
    {
        _FILLRGN aFillRgn;
        HRGN hrgnClip;
        _LPBITMAPDATA lpBitmap;
        _BLKBITBLT blkData;
		//得到显示面
        lpBitmap = _GetHBITMAPPtr( lpdc->hBitmap );
        if( lpBitmap )
        {	//拷贝区域
            hrgnClip = WinRgn_CreateRect( 0, 0, 0, 0 );
            WinRgn_Combine( hrgnClip, hrgn, 0, RGN_COPY );
			//逻辑坐标到设备坐标
            WinRgn_Offset( hrgnClip, XOFFSET( lpdc ), YOFFSET( lpdc ) );
			//与DC的裁剪区域内交
            WinRgn_Combine( hrgnClip, hrgnClip, lpdc->hrgn, RGN_AND );

            WinRgn_GetBox( hrgnClip, &aFillRgn.rect );
			//裁剪区域
            aFillRgn.lprNodes = _GetHRGNPtr( hrgnClip )->lpNodeFirst;

            blkData.lpDestImage = lpBitmap;
            blkData.dwRop = DSTINVERT;
			//填充
            _FillRgn( lpdc->lpDispDrv, &blkData, &aFillRgn );

            WinGdi_DeleteObject( hrgnClip );
            retv = TRUE;//return TRUE;
        }
    }
	_LeaveDCPtr( lpdc, DCF_WRITE );
    return retv;//FALSE;
}

// **************************************************
// 声明：BOOL WINAPI WinGdi_PatBlt(HDC hdc, int nX, int nY,  int nWidth, int nHeight, DWORD dwRop)
// 参数：
//	IN hdc-目标DC
//	IN nX-目标矩形左上角X坐标
//	IN nY-目标矩形左上角Y坐标
//	IN nWidth-目标矩形宽度
//	IN nHeight-目标矩形高度
//	IN dwRop-光栅操作码PATCOPY-拷贝模板到目标矩形
//		PATINVERT-模板颜色与目标颜色做异或运算(XOR)
//		BLACKNESS-用黑色填充目标矩形
//		WHITENESS-用白色填充目标矩形
//		DSTINVERT-反转目标颜色
// 返回值：
//	假如成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	用当前刷子(brush)绘制目标面
// 引用: 
//	系统 API
// ************************************************

BOOL WINAPI WinGdi_PatBlt(HDC hdc, int nX, int nY,  int nWidth, int nHeight, DWORD dwRop)
{	//得到安全 DC
    _LPGDCDATA lpdc= _GetSafeDrawPtr( hdc );
	BOOL retv = FALSE;

    if( lpdc && lpdc->lpDispDrv )
    {
        _FILLRGN aFillRgn;
        _LPBITMAPDATA lpBitmap;
        _BLKBITBLT blkData;
		RECT rcBrushMask; //2005-09-15 , add

        aFillRgn.rect.left = nX;
        aFillRgn.rect.top = nY;
        aFillRgn.rect.right = nX + nWidth;
        aFillRgn.rect.bottom = nY + nHeight;
		rcBrushMask = aFillRgn.rect;//2005-09-15 , add

        lpBitmap = _GetHBITMAPPtr( lpdc->hBitmap );
        if( lpBitmap )
        {
			// make origin point
            OffsetRect( &aFillRgn.rect, XOFFSET( lpdc ), YOFFSET( lpdc ) );
            aFillRgn.lprNodes = _GetHRGNPtr( lpdc->hrgn )->lpNodeFirst;
			// 准备填充结构 do fill
            blkData.lpDestImage = lpBitmap;
            blkData.lpBrush = &lpdc->brushAttrib;
			blkData.lpptBrushOrg = &lpdc->ptBrushOrg;
			////2005-09-15, modify
			//blkData.lprcMask = &aFillRgn.rect;
			blkData.lprcMask = &rcBrushMask;
			//
            blkData.solidBkColor = lpdc->backColor;
            blkData.dwRop = dwRop;

            _FillRgn( lpdc->lpDispDrv, &blkData, &aFillRgn );
            retv = TRUE;//return TRUE;
        }
    }
	_LeaveDCPtr( lpdc, DCF_WRITE );
    return retv;//FALSE;
}

// **************************************************
// 声明：BOOL WINAPI WinGdi_MaskBlt( HDC hdc,
//                  int xDest,
//                  int yDest,
//                  int width,
//                  int height,
//                  HDC hdcSrc,
//                  int xSrc,
//                  int ySrc,
//                  HBITMAP hbmMask,
//                  int xMask,
//                  int yMask,
//                  DWORD dwRop )

// 参数：
//	IN hdcDest-目标DC
//	IN nXDest-目标矩形左上角X坐标
//	IN nYDest-目标矩形左上角Y坐标
//	IN nWidth-目标矩形宽度
//	IN nHeight-目标矩形高度
//	IN hdcSrc-源DC
//	IN nXSrc-源矩形左上角X坐标
//	IN nYSrc-源矩形左上角Y坐标
//	IN hbmMask-用作掩模的位图
//	IN nXMask-掩模左上角X坐标
//	IN nYMask-掩模左上角Y坐标
//	IN dwRop-光栅操作码( 仅支持SRCCOPY)
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	带屏蔽位位图传送
// 引用: 
//	系统API
// ************************************************

BOOL WINAPI WinGdi_MaskBlt( HDC hdc,
                  int xDest,
                  int yDest,
                  int width,
                  int height,
                  HDC hdcSrc,
                  int xSrc,
                  int ySrc,
                  HBITMAP hbmMask,
                  int xMask,
                  int yMask,
                  DWORD dwRop )
{	//得到安全DC
    _LPGDCDATA lpdcDest = _GetSafeDrawPtr( hdc );
    _LPGDCDATA lpdcSrc = _GetSafeDrawPtr( hdcSrc );
    _LPRGNDATA lprgn;
    _LPRECTNODE lprnNode;
    _BLKBITBLT blt;
    RECT rcSrc, rcDest, rcMask, rcTemp, rcClip;
    int xoff, yoff, xOffSrc, yOffSrc, xOffMask, yOffMask;
	_LPCDISPLAYDRV lpDispDrv;
	BOOL retv = FALSE;

    if( lpdcDest && lpdcSrc && lpdcDest->lpDispDrv )
    {
        lpDispDrv = lpdcDest->lpDispDrv;
		//准备块操作结构
		blt.lpDestImage = _GetHBITMAPPtr( lpdcDest->hBitmap );
        blt.lpSrcImage = _GetHBITMAPPtr( lpdcSrc->hBitmap );
        blt.lpMaskImage = _GetHBITMAPPtr( hbmMask );
        blt.lprcSrc = &rcSrc;
        blt.lprcDest = &rcDest;
        blt.lprcMask = &rcMask;
        blt.solidColor = lpdcDest->textColor;
        blt.solidBkColor = lpdcDest->backColor;
        blt.dwRop = dwRop;
		//检查目标和源矩形的的重叠部份
        xOffSrc = XOFFSET( lpdcSrc );
        yOffSrc = YOFFSET( lpdcSrc );
        rcTemp.left = xSrc;
        rcTemp.top = ySrc;
        rcTemp.right = rcTemp.left + width;
        rcTemp.bottom = rcTemp.top + height;
        OffsetRect( &rcTemp, xOffSrc, yOffSrc );

        xoff = XOFFSET( lpdcDest );
        yoff = YOFFSET( lpdcDest );

        rcClip.left = xDest;
        rcClip.top = yDest;
        rcClip.right = xDest + width;
        rcClip.bottom = yDest + height;
        OffsetRect( &rcClip, xoff, yoff );

        if( rcTemp.left < 0 )
            rcClip.left -= rcTemp.left;
        if( rcTemp.top < 0 )
            rcClip.top -= rcTemp.top;
        if( rcTemp.right > blt.lpSrcImage->bmWidth )
            rcClip.right -= rcTemp.right - blt.lpSrcImage->bmWidth;
        if( rcTemp.bottom > blt.lpSrcImage->bmHeight )
            rcClip.bottom -= rcTemp.bottom - blt.lpSrcImage->bmHeight;

        xOffSrc = xSrc + xOffSrc - xDest - xoff;
        yOffSrc = ySrc + yOffSrc - yDest - yoff;

        xOffMask = xMask + 0 - xDest - xoff;
        yOffMask = yMask + 0 - yDest - yoff;

        lprgn = _GetHRGNPtr( lpdcDest->hrgn );
        lprnNode = lprgn->lpNodeFirst;

        if( blt.lpDestImage->bmBits == blt.lpSrcImage->bmBits )
        {	//源和目标在相同的显示面
            while( lprnNode )
            {	//检查每一个裁剪区域，看是否内交
                if( IntersectRect( &rcDest, &rcClip, &lprnNode->rect ) )
                {	//设置绘制方向为从左到右，从上到下
                    rcMask = rcSrc = rcDest;
                    OffsetRect( &rcSrc, xOffSrc, yOffSrc );
                    OffsetRect( &rcMask, xOffMask, yOffMask );

                    blt.yPositive = 1;	//从上到下
                    blt.xPositive = 1;	//从左到右

                    if( IntersectRect( &rcTemp, &rcSrc, &rcDest ) )
                    {	//因为是相同的显示面，所以必须确定正确的拷贝顺序
						//这里表示目标矩形与源矩形有重叠并且必须反向拷贝
                        if( rcSrc.top  < blt.lprcDest->top )
                            blt.yPositive = 0;	//从下到上
                        if( rcSrc.left  < blt.lprcDest->left )
                            blt.xPositive = 0;	//从右到左
                    }
                    lpDispDrv->lpBlkBitMaskBlt( &blt );
                }
                lprnNode = lprnNode->lpNext;
            }
        }
        else
        {	//不同显示面
			if( ( blt.lpDestImage->bmBitsPixel == blt.lpSrcImage->bmBitsPixel ||
				blt.lpSrcImage->bmBitsPixel == 1 ) )
			{	//数据格式相同或者源格式是黑白位图格式				
				//设置绘制方向为从左到右，从上到下
				
				blt.yPositive = 1;
				blt.xPositive = 1;
				
				while( lprnNode )
				{
					if( IntersectRect( &rcDest, &rcClip, &lprnNode->rect ) )
					{
						rcMask = rcSrc = rcDest;
						OffsetRect( &rcSrc, xOffSrc, yOffSrc );
						OffsetRect( &rcMask, xOffMask, yOffMask );
						lpDispDrv->lpBlkBitMaskBlt( &blt );
					}
					lprnNode = lprnNode->lpNext;
				}
			}
			else if( lpdcSrc->lpDispDrv )
			{   // 不同的格式， 点对点拷贝 pixel -> pixel
				_LPCDISPLAYDRV lpSrcDispDrv = lpdcSrc->lpDispDrv;
				_PIXELDATA pxSrc, pxDest, pxMask;
				UINT uiRopSrc, uiRopDest;
				
	            pxSrc.lpDestImage = blt.lpSrcImage;
	            pxSrc.pattern = 0xff;
	            pxSrc.rop = R2_NOP;  // 读仅仅 read only

				pxMask.lpDestImage = blt.lpMaskImage;
	            pxMask.pattern = 0xff;
	            pxMask.rop = R2_NOP; // 读仅仅  read only

				pxDest.lpDestImage = blt.lpDestImage;
	            pxDest.pattern = 0xff;
				//准备操作功能
				switch( dwRop )
				{
				case MAKEROP4(SRCCOPY, SRCINVERT):  // dest = src & dest = src ^ dst
					uiRopSrc = R2_COPYPEN;
					uiRopDest = R2_MASKPEN;
					break;
				case MAKEROP4(SRCCOPY, 0):  // dest = src & dest = src ^ dst
					uiRopSrc = R2_COPYPEN;
					uiRopDest = R2_NOP;
				default:
					SetLastError( ERROR_INVALID_PARAMETER );
					goto _return;//_ERROR;
				}
				//对每一个裁剪区域进行操作
				while( lprnNode )
				{
					if( IntersectRect( &rcDest, &rcClip, &lprnNode->rect ) )
					{
						rcSrc = rcDest;
						OffsetRect( &rcSrc, xoff, yoff );

						PixelTransfer( lpdcDest, &pxDest, &rcDest, lpdcSrc, &pxSrc, &rcSrc, &pxMask, uiRopSrc, uiRopDest, -1 );
					}
					lprnNode = lprnNode->lpNext;
				}
			}
        }
        retv = TRUE;//return TRUE;
    }
//_ERROR:
_return:
	_LeaveDCPtr( lpdcDest, DCF_WRITE );
	_LeaveDCPtr( lpdcSrc, DCF_READ );
	
    return retv;//FALSE;
}

// **************************************************
// 声明：BOOL WINAPI WinGdi_DrawEdge( HDC hdc, LPCRECT lpRect, UINT edgeType, UINT flags )
// 参数：
//	IN hdc-目标DC
//	IN lpcrc-边框，RECT结构指针
//	IN uType-内外边框的形状，包含：
//	    BDR_RAISEDOUTER-外边框是凸的
//		BDR_SUNKENOUTER-外边框是凹的
//		BDR_RAISEDINNER-内边框是凸的
//		BDR_SUNKENINNER-内边框是凹的
//		你也能用以下组合：
//		EDGE_RAISED-(BDR_RAISEDOUTER | BDR_RAISEDINNER)
//      EDGE_SUNKEN-(BDR_SUNKENOUTER | BDR_SUNKENINNER)
//      EDGE_ETCHED-(BDR_SUNKENOUTER | BDR_RAISEDINNER)
//      EDGE_BUMP-(BDR_RAISEDOUTER | BDR_SUNKENINNER) 
//	IN uFlags-边框类型
//		BF_DIAGONAL-对角线
//		BF_MIDDLE-填充边框内部
//		BF_LEFT-左边框
//		BF_TOP-顶边框
//		BF_RIGHT-右边框
//		BF_BOTTOM-底边框
//		你也能用以下组合：
//		BF_TOPLEFT-(BF_TOP | BF_LEFT)
//      BF_BOTTOMRIGHT-(BF_BOTTOM | BF_RIGHT)
//      BF_RECT-(BF_TOP | BF_LEFT | BF_BOTTOM | BF_RIGHT)
//      BF_DIAGONAL_ENDTOPRIGHT-(BF_DIAGONAL | BF_TOP | BF_RIGHT)
//      BF_DIAGONAL_ENDTOPLEFT-(BF_DIAGONAL | BF_TOP | BF_LEFT)
//      BF_DIAGONAL_ENDBOTTOMLEFT-(BF_DIAGONAL | BF_BOTTOM | BF_LEFT)
//      BF_DIAGONAL_ENDBOTTOMRIGHT-(BF_DIAGONAL | BF_BOTTOM | BF_RIGHT)


// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	用不同颜色画边框以产生凹凸感
// 引用: 
//	系统API
// ************************************************

BOOL WINAPI WinGdi_DrawEdge( HDC hdc, LPCRECT lpRect, UINT edgeType, UINT flags )
{	//得到安全DC
    _LPGDCDATA lpdc= _GetSafeDrawPtr( hdc );
	BOOL retv = FALSE;

	if( lpdc && lpdc->lpDispDrv )
    {
        _LPRECTNODE lprNodes;
        _LPBITMAPDATA lpBitmap;
        RECT rect = *lpRect;
        _LINEDATA lineData;

        UINT b[2] = { BDR_OUTER, BDR_INNER };
        int i;
		COLORREF clBorder[2][2];
		//得到每个边的颜色
		clBorder[0][0] = lpdc->lpDispDrv->lpRealizeColor( WinSys_GetColor(COLOR_BTNHILIGHT), lpdc->lpdwPal, lpdc->wPalNumber, lpdc->wPalFormat );//LN:2003-05-06,代替 GetSysColor 为 WinSys_GetColor
        clBorder[0][1] = lpdc->lpDispDrv->lpRealizeColor( WinSys_GetColor(COLOR_3DDKSHADOW), lpdc->lpdwPal, lpdc->wPalNumber, lpdc->wPalFormat );//LN:2003-05-06,代替 GetSysColor 为 WinSys_GetColor
		clBorder[1][0] = lpdc->lpDispDrv->lpRealizeColor( WinSys_GetColor(COLOR_3DLIGHT), lpdc->lpdwPal, lpdc->wPalNumber, lpdc->wPalFormat );//LN:2003-05-06,代替 GetSysColor 为 WinSys_GetColor
        clBorder[1][1] = lpdc->lpDispDrv->lpRealizeColor( WinSys_GetColor(COLOR_BTNSHADOW), lpdc->lpdwPal, lpdc->wPalNumber, lpdc->wPalFormat );//LN:2003-05-06,代替 GetSysColor 为 WinSys_GetColor
        
		//得到显示面
        if( (lpBitmap = _GetHBITMAPPtr( lpdc->hBitmap )) != 0 )
        {
            lprNodes = _GetHRGNPtr( lpdc->hrgn )->lpNodeFirst;
			//逻辑坐标到设备坐标
            OffsetRect( &rect, XOFFSET( lpdc ), YOFFSET( lpdc ) );

            if( flags & BF_MIDDLE )
            {   // 清除背景 clear background
                WinGdi_FillRect( hdc, lpRect, WinSys_GetColorBrush( COLOR_BTNFACE ) );
            }
			//准备画线结构
            lineData.backMode = OPAQUE;
            lineData.clrBack = 0;            
            lineData.lpDestImage = lpBitmap;
            lineData.pattern = 0xff;
            lineData.rop = R2_COPYPEN;
            lineData.width = 1;
			lineData.lpfnCallback = NULL;

            if( flags & BF_DIAGONAL )
            {	//斜线
                if( (flags & BF_TOPLEFT) == BF_TOPLEFT ||
                    (flags & BF_BOTTOMRIGHT) == BF_BOTTOMRIGHT )
                {
                    if( edgeType & BDR_OUTER )
                    {	//画外轮廓
                        if( (edgeType & BDR_RAISEDOUTER) )
                            lineData.color = clBorder[0][0];
                        else
                            lineData.color = clBorder[0][1];
                        _DrawLine( lpdc->lpDispDrv, lprNodes, &lineData, rect.left, rect.top, rect.right, rect.bottom );
                    }
                    if( edgeType & BDR_INNER )
                    {	//画内轮廓
                        if( edgeType & BDR_RAISEDINNER )
                            lineData.color = clBorder[1][0];
                        else
                            lineData.color = clBorder[1][1];
                        _DrawLine( lpdc->lpDispDrv, lprNodes, &lineData, rect.left, rect.top+1, rect.right-1, rect.bottom );
                    }
                }
                else
                {
                    if( (edgeType & BDR_RAISEDOUTER) )
                        lineData.color = clBorder[0][0];
                    else
                        lineData.color = clBorder[0][1];
                    _DrawLine( lpdc->lpDispDrv, lprNodes, &lineData, rect.left, rect.bottom-1, rect.right, rect.top-1 );
                    if( edgeType & BDR_RAISEDINNER )
                        lineData.color = clBorder[1][0];
                    else
                        lineData.color = clBorder[1][1];
                    _DrawLine( lpdc->lpDispDrv, lprNodes, &lineData, rect.left+1, rect.bottom-1, rect.right, rect.top );
                }
            }
            else
            {
                for( i = 0; i < 2; i++ )
                {   // draw left and top frame
                    if( edgeType & b[i] )
                    {
                        if( (edgeType & b[i] ) & BDR_RAISED )
                        {	//凸
#ifdef COLOR_1BPP
                            lineData.color = clBorder[i][1];
                            lineData.pattern = 0xaa;
#else
                            lineData.color = clBorder[i][0];
#endif
                        }
                        else
                        {	//凹
#ifdef COLOR_1BPP
                            lineData.pattern = 0xff;
#endif
                            lineData.color = clBorder[i][1];
                        }
                        if( flags & BF_TOP )                            
                            _DrawLine( lpdc->lpDispDrv, lprNodes, &lineData, rect.left+i, rect.top+i, rect.right-i, rect.top+i );
                        if( flags & BF_LEFT )                            
                            _DrawLine( lpdc->lpDispDrv, lprNodes, &lineData, rect.left+i, rect.top+i, rect.left+i, rect.bottom-i );
                        // draw right and bottom frame
                        if( (edgeType & b[i] ) & BDR_RAISED )
                        {	//凸
#ifdef COLOR_1BPP
                            lineData.pattern = 0xff;
#endif
                            lineData.color = clBorder[i][1];
                        }
                        else
                        {	//凹
#ifdef COLOR_1BPP
                            lineData.pattern = 0xaa;
                            lineData.color = clBorder[i][1];
#else
                            lineData.color = clBorder[i][0];
#endif
                        }
                        if( flags & BF_BOTTOM )                            
                            _DrawLine( lpdc->lpDispDrv, lprNodes, &lineData, rect.left+i, rect.bottom-i-1, rect.right-i, rect.bottom-i-1 );
                        if( flags & BF_RIGHT )                            
                            _DrawLine( lpdc->lpDispDrv, lprNodes, &lineData, rect.right-i-1, rect.top+i, rect.right-i-1, rect.bottom-i );
                    }
                }
            }
            retv = TRUE;//return TRUE;
        }
    }
	_LeaveDCPtr( lpdc, DCF_WRITE );
    return retv;//FALSE;
}

// **************************************************
// 声明：BOOL WINAPI WinGdi_DrawIconEx( HDC hdc, 
//										int x, 
//										int y, 
//										HICON hIcon,
//										int cxWidth, 
//										int cyWidth, 
//										UINT indexAnimal, 
//										HBRUSH hBrush, 
//										UINT uiFlags )
// 参数：
//	IN hdc - 目标DC
//	IN x - 需要输出的左上角x坐标
//	IN y - 需要输出的左上角y坐标
//	IN hIcon - 需要输出的ICON图象
//	IN cxWidth - 输出图象宽度
//	IN cyWidth - 输出图象高度
//	IN uiIndexAnimal - 如果是动画ICON，表示其索引值（目前不支持，必须为NULL）
//	IN hBrush - 刷子（目前不支持，必须为NULL）
//	IN uiFlags - 输出功能（必须为DI_NORMAL）
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	画图标ICON
// 引用: 
//	系统API
// ************************************************

BOOL WINAPI WinGdi_DrawIconEx( HDC hdc, int x, int y, HICON hIcon, int cxWidth, int cyWidth, UINT indexAnimal, HBRUSH hBrush, UINT uiFlags )
{	//由句柄得到DC指针
    //_LPGDCDATA lpdc = _GetHDCPtr( hdc );
	BOOL ret = FALSE;

    //if( lpdc && hIcon )
	if( hIcon )
    {
        ICONINFO icfo;
		//得到ICON信息
        WinGdi_GetIconInfo( hIcon, &icfo );
		
        if( icfo.hbmColor && icfo.hbmMask )
		{
			HDC hMemDC;
            BITMAP bmpInfo;
            if( !icfo.fIcon )
            {   // cursor
                x -= icfo.xHotspot;
                y -= icfo.yHotspot;
            }
			//得到ICON的位图数据
            WinGdi_GetObjectInfo( icfo.hbmColor, sizeof(bmpInfo), &bmpInfo );
			//创建一个内存DC
			hMemDC = WinGdi_CreateCompatibleDC( hdc );
			if( hMemDC )
			{	//创建成功
				HBITMAP hOld;
				hOld = WinGdi_SelectObject( hMemDC, icfo.hbmColor );
				if( hOld )
				{	//得到宽度和高度
					if( cxWidth == 0 )
					{
						if( uiFlags & DI_DEFAULTSIZE )
						    cxWidth = GetSystemMetrics( SM_CXICON );
						else
							cxWidth = bmpInfo.bmWidth;
					}
					if( cyWidth == 0 )
					{
						if( uiFlags & DI_DEFAULTSIZE)
						    cyWidth = WinSys_GetMetrics( SM_CYICON );
						else
							cyWidth = bmpInfo.bmHeight;
					}
					//绘制
					ret = WinGdi_MaskBlt( hdc,
								 x,
								 y,
								 cxWidth,
								 cyWidth,
								 hMemDC,
								 0,
								 0,
								 icfo.hbmMask,
								 0,
								 0,
								 MAKEROP4(SRCCOPY, 0) );
				    WinGdi_SelectObject( hMemDC, hOld );
				}
				//删除内存DC
				WinGdi_DeleteDC( hMemDC );
			}
		}
    }
    return ret;
}

// **************************************************
// 声明：BOOL WINAPI WinGdi_DrawIcon( HDC hdc, int x, int y, HICON hIcon )
// 参数：
// 	IN hdc-目标DC句柄
// 	IN nX-输出点X坐标
// 	IN nY-输出点Y坐标
// 	IN hIcon-ICON句柄
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	画图标
// 引用: 
//	系统API
// ************************************************

BOOL WINAPI WinGdi_DrawIcon( HDC hdc, int x, int y, HICON hIcon )
{
	return WinGdi_DrawIconEx( hdc, x, y, hIcon, 0, 0, 0, NULL, DI_NORMAL );
}



// **************************************************
// 声明：static int _ScanText( _LPFONT lpFont, LPCSTR lpstr, UINT uFormat, int* lpxWidth, int* lpwCount, int xClip, int nCount, int * lpEllipsisPos )
// 参数：
// 	IN lpFont - 字体对象
// 	IN lpstr - 包含字符串的指针
// 	IN uFormat - 扫描功能，
//				DT_SINGLELINE - 单行文本
//				DT_NOPREFIX - 没有前缀符（ &Hello, '&'是前缀符 )  
//				DT_TABSTOP - 设定TAB字符宽,默认为8个字符宽
//				DT_END_ELLIPSIS - 用省略号表示无法在裁剪区显示的字符
// 	OUT lpxWidth - 用于接受字符串的水平宽度
// 	IN lpwCount - 用于接受字符串个数
// 	IN xClip - 裁剪宽度(显示的字符串不能超过该宽度）
// 	IN nCount - 字符串的指针指向的字符个数
// 	IN lpEllipsisPos - 省略号位置
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	扫描字符串，得到在裁剪宽度范围内的字符个数
// 引用: 
//	
// ************************************************

//#define TEXT_PREFIX_WORD  '&'
#define TEXT_LINE_ENTER   0x0d
#define TEXT_LINE_NEWLINE 0x0a
#define TEXT_ENDFLAG      0
#define TAB_CHAR          '\t'
#define DFLT_TAB          8
#define DFLT_TAB_WIDTH    8
#define IS_LINE_MARK( v ) ( (v) == TEXT_LINE_ENTER || (v) == TEXT_LINE_NEWLINE || (v) == 0 )

#define DEBUG_SCANTEXT 0
static int _ScanText( _LPFONT lpFont, LPCSTR lpstr, UINT uFormat, int* lpxWidth, int* lpwCount, int xClip, int nCount, int * lpEllipsisPos )
{
    _CHAR_METRICS textBitmap;
    int firstPreFix = 0;
    int prexWidth, xWidth = 0;
    int cw=0;
	int precw; //前一个字符的bytes数
	_LPFONTDRV lpFontDrv = lpFont->lpDriver;
	HANDLE hFont = lpFont->handle;

	DEBUGMSG( DEBUG_SCANTEXT, ( "xClip=%d,nCount=%d.\r\n",xClip, nCount ) );
	//先设定默认值
    *lpxWidth = *lpwCount = 0;
    if( uFormat & DT_END_ELLIPSIS )
	    *lpEllipsisPos = nCount;

    while( *lpwCount < nCount )
    {
		prexWidth = xWidth;
	    xWidth = 0;
		precw=cw;
        cw = 1;

        if( *lpstr == 0 )
            break;
        else if( (uFormat & DT_SINGLELINE) == 0 && 
			     *lpstr == TEXT_LINE_ENTER && 
				 *(lpstr+1) == TEXT_LINE_NEWLINE )
        {	//非单行
            (*lpwCount)+=2;
            break;
        }
	    if( *lpstr == TEXT_PREFIX_WORD && (!(uFormat & DT_NOPREFIX)) && firstPreFix == 0 )
	    {	//有前缀
	        firstPreFix = 1 - firstPreFix;
	    }
	    else if( *lpstr  == TAB_CHAR )
	    {	//为tab字符
	        if( uFormat & DT_TABSTOP )
		        xWidth = HIBYTE(uFormat)*DFLT_TAB_WIDTH;
	        else
		        xWidth = DFLT_TAB*DFLT_TAB_WIDTH;
	        firstPreFix = 0;
	    }
	    else
        {	//通常的字符
            cw = lpFontDrv->lpWordBitmap( hFont, (LPCBYTE)lpstr, nCount - *lpwCount, &textBitmap );
            xWidth = textBitmap.xAdvance;
	        firstPreFix = 0;
	    }
		if( (*lpxWidth + xWidth) > xClip )
		{	//超过了裁剪区
			if( (uFormat & DT_SINGLELINE) && 
				(uFormat & DT_END_ELLIPSIS) == 0 )
			{	//单行无省略号
				(*lpxWidth) += xWidth;
				(*lpwCount) += cw;
			}
			else
			{	//
				if( *lpwCount == 0 && xClip > 0 )
				{	//首
					(*lpxWidth) += xWidth;
					(*lpwCount) += cw;
				}
				if( (uFormat & DT_END_ELLIPSIS) && *lpwCount > cw )
				{
					//*lpEllipsisPos = *lpwCount - cw;//省略号位置
					if( *lpxWidth + cwEllipsis > xClip )
					{	//如果不可以显示 "..."
						*lpEllipsisPos = *lpwCount - precw;//省略号位置						
					    *lpxWidth = *lpxWidth - prexWidth + cwEllipsis;
						*lpxWidth = MIN( *lpxWidth, xClip );
					}
					else
					{	//可以显示 "..."
						*lpEllipsisPos = *lpwCount;//省略号位置
						(*lpwCount) ++;	//包含省略号的显示字符数
						(*lpxWidth) += cwEllipsis;//包含省略号的显示宽度
						
					}
				}
			}
			break;
		}
		//加实际显示宽度
	    (*lpxWidth) += xWidth;
		//加实际显示数
	    (*lpwCount) += cw;
		//下一个字符
	    lpstr += cw;
    }
    return *lpwCount;
}

// **************************************************
// 声明：int WINAPI WinGdi_DrawText( HDC hdc, LPCTSTR lpstr, int nCount, LPRECT lpRect, UINT uFormat )
// 参数：
// 	IN hdc - 绘图DC句柄
// 	IN lpstr - 需要显示的字符串
// 	IN nCount-输出字符个数, 假如等于-1, lpcstr必须以'\0'结束
// 	IN lpcrc-RECT结构指针,输出字符串在该矩形内进行格式化
// 	IN uFormat-格式化标志,包含：
// 	        DT_TOP-上对齐(仅单行文本DT_SINGLELINE)
// 	        DT_LEFT-左对齐(默认)
// 	        DT_SINGLELINE-单行文本
// 	        DT_BOTTOM-底端对齐(仅单行文本DT_SINGLELINE)
// 	        DT_VCENTER-垂直居中(仅单行文本DT_SINGLELINE)
// 	        DT_RIGHT-右对齐
// 	        DT_CENTER-水平居中
// 	        DT_TABSTOP-设定TAB字符宽,默认为8个字符宽
// 	        DT_NOPREFIX-关闭前缀符
// 	        DT_NOCLIP-不裁剪
// 	        DT_WORDBREAK-自动换行
//			DT_END_ELLIPSIS - 用省略号表示不能显示的字符
// 返回值：
//	成功：返回绘制的文本高度；失败，返回FALSE
// 功能描述：
//	格式化文本输出
// 引用: 
//	系统API
// ************************************************

int WINAPI WinGdi_DrawText( HDC hdc, LPCTSTR lpstr, int nCount, LPRECT lpRect, UINT uFormat )
{
    _LPGDCDATA lpdc = _GetSafeDrawPtr( hdc );
	int retv = 0;

	if( lpdc && lpdc->lpDispDrv )
    {
        _LPFONT lpFont = _GetHFONTPtr( lpdc->hFont );
		_LPFONTDRV lpFontDrv = lpFont->lpDriver;
		HANDLE hFont = lpFont->handle;

        _LPRGNDATA lprgn = _GetHRGNPtr( lpdc->hrgn );
        _LINEDATA lineData;
        _LPBITMAPDATA lpBitmap;
        _CHAR_METRICS textBitmap;
        _BLKBITBLT textBitblt;
		_LPCDISPLAYDRV lpDispDrv = lpdc->lpDispDrv;
		_BLKBITBLT bkBitblt;

        int num, bw, bm;
        int x, y, xWidth, wCount, tbWidth, yUnderLine, iTextHeight, iTextWidth;
        int fh = lpFontDrv->lpMaxHeight( hFont );
        int cw;
        int xoff = XOFFSET( lpdc );
        int yoff = YOFFSET( lpdc );
		int iEllipsis;
//		int yTextHeight;
        BOOL fUnderLine = FALSE;
        RECT rectClip = *lpRect;
        RECT rectSave, r, rMask, rCharBox, rCharBitmap;
        const _RECTNODE FAR* lprNodes;
		//得到位图指针
        lpBitmap = _GetHBITMAPPtr( lpdc->hBitmap );
        if( lpBitmap == 0 )
            goto _return;//return FALSE;
		//逻辑坐标到设备坐标
        OffsetRect( &rectClip, xoff, yoff );
		//假如可能，得到字符串个数
        if( nCount <= 0 )
	        nCount = strlen( lpstr );
        y = rectClip.top;
        if( uFormat & DT_SINGLELINE )
        {	//单行
	        if( uFormat & DT_BOTTOM )
	            y = rectClip.bottom - fh;
	        else if( uFormat & DT_VCENTER )
	            y = rectClip.top + ( (rectClip.bottom-rectClip.top) - fh ) / 2;
        }
		//2004-06-15,增加对 DT_END_ELLIPSIS 的处理
		//
		if( (uFormat & DT_CALCRECT) && 
			(uFormat & DT_SINGLELINE) &&
			(uFormat & DT_END_ELLIPSIS) == 0 )
			cw = 0x7fff;		//统计显示矩形而不显示
		else
			cw = lpRect->right - lpRect->left;
		//
		//初始化块绘制结构
        textBitmap.bitmap.bmPlanes = 1;
        textBitmap.bitmap.bmBitsPixel = 1;
        textBitblt.lpDestImage = lpBitmap;
        textBitblt.lprcDest = &rCharBitmap;
        textBitblt.lpBrush = 0;
		textBitblt.lpptBrushOrg = NULL;
        textBitblt.lpSrcImage = &textBitmap.bitmap;
        textBitblt.lprcSrc = &rMask;

        textBitblt.solidColor= lpdc->textColor;
        textBitblt.solidBkColor= lpdc->backColor;
        textBitblt.yPositive = 1;
        textBitblt.xPositive = 1;
        textBitblt.backMode = lpdc->backMode;
        bm = (lpdc->backMode == OPAQUE);
        textBitblt.dwRop = SRCCOPY;
		//初始化线绘制结构
        lineData.backMode = OPAQUE;
        lineData.clrBack = lpdc->backColor;
        lineData.color = textBitblt.solidColor;
        lineData.lpDestImage = lpBitmap;
        lineData.pattern = 0xff;
        lineData.rop = R2_COPYPEN;
        lineData.width = 1;
		lineData.dn = 0;
		lineData.iDir = 0;
		lineData.lpfnCallback = NULL;

        bkBitblt.lpDestImage = lpBitmap;
        bkBitblt.lprcDest = &rCharBox;
        bkBitblt.lpBrush = 0;
		bkBitblt.lpptBrushOrg = NULL;
        bkBitblt.lpSrcImage = NULL;
        bkBitblt.lprcSrc =NULL;
        bkBitblt.solidColor= lpdc->backColor;
        bkBitblt.dwRop = PATCOPY;

		//yTextHeight = lpFontDrv->lpMaxHeight( hFont );

		iTextHeight = 0;
	    iTextWidth = 0;
		//画每一个字符
        while( nCount )
        {	// 扫描字符串并得到一显示行 get new line			
	        if( _ScanText( lpFont, lpstr, uFormat, &xWidth, &wCount, cw, nCount, &iEllipsis ) == 0 )   // is eof?
	            break;		//如果字符串结束，退出

			if( uFormat & DT_CALCRECT ) 
			{   // 仅仅统计显示矩形，only calc the rect bound, no thing to do
				if( iTextWidth < xWidth )
					iTextWidth = xWidth;
				iTextHeight += fh;
			    nCount -= wCount;
				lpstr += wCount;

				if( uFormat & DT_SINGLELINE )
					break;	//单行，已统计完，退出
				continue;
			}
				
	        if( uFormat & DT_RIGHT )
	            x = rectClip.right - xWidth;	//右对齐
	        else if( uFormat & DT_CENTER )
                x = rectClip.left + (cw - xWidth) / 2;	//居中
	        else
                x = rectClip.left;		//默认左对齐

			// 以下代码处理一行的字符串数据 draw a line
            num = 0;
	        while( num < wCount )
            {    // 是否为tab字符 is tab char
				int xMaskOff, yMaskOff;
                if( *lpstr == TAB_CHAR )
	            {
                    if( uFormat & DT_TABSTOP )
		                tbWidth = HIBYTE(uFormat)*DFLT_TAB_WIDTH;
                    else
   		                tbWidth = DFLT_TAB*DFLT_TAB_WIDTH;        // default
		            lpstr++;
                    x += tbWidth;
		            num++;
                    continue;
                }
				// 是否为断行标志 is break char
	            if( IS_LINE_MARK( *lpstr ) )
	            {   // 准备下一行 clear line
		            lpstr++;
		            num++;
                    continue;
                }
				// 是否为前缀标志 is prefix char
	            if( *lpstr == TEXT_PREFIX_WORD )
                {
		            if( !(uFormat & DT_NOPREFIX) )
                    {	//处理前缀标志
                        lpstr++;
                        num++;
		                fUnderLine = TRUE;
		            }
	            }
				// 显示一个字符 draw one word's mask
				if( uFormat & DT_END_ELLIPSIS )
				{	//有省略号风格
					TCHAR bEllipsis[4];
					bEllipsis[0] = 0;
					if( num && num == iEllipsis )
					{	// 当前为一行的结束 line end
						if( *(lpstr+1) != 0 )	//刚好结束吗 ？
						{	//否
							if( (uFormat & DT_SINGLELINE) ||
								(y + fh >= rectClip.bottom) )
							{	// 条件满足，画一个省略号
								// yes, drawELLIPSIS
								//bEllipsis[0] = (char)'…';
								bEllipsis[0]= (BYTE)'…'; //未了得到空格的宽度
								bEllipsis[1]= 0;
								//bEllipsis[2]= 0;
								num = wCount;
							}
						}
					}
					if( bEllipsis[0] )
					{	//需要画省略号,得到字符的 字模
						bw = lpFontDrv->lpWordBitmap( hFont, (LPCBYTE)bEllipsis, wCount - num, &textBitmap );
					}
					else 
						bw = lpFontDrv->lpWordBitmap( hFont, (LPCBYTE)lpstr, wCount - num, &textBitmap );//得到字符的 字模
					// handle the DT_END_ELLIPSIS end
				}
				else
                    bw = lpFontDrv->lpWordBitmap( hFont, (LPCBYTE)lpstr, wCount - num, &textBitmap );//得到字符的 字模


				rCharBox.left = x;
				rCharBox.top = y;
				rCharBox.right = rCharBox.left + textBitmap.xAdvance;
				rCharBox.bottom = rCharBox.top + fh;//yTextHeight;//


				//得到字符显示的位置
                xMaskOff = r.left = x + textBitmap.left;
				yMaskOff = r.top = y + textBitmap.top; 

                r.right = r.left + textBitmap.bitmap.bmWidth; 
                r.bottom = r.top + textBitmap.bitmap.bmHeight;
                yUnderLine = r.bottom;
				//与裁剪区求交
                //if( IntersectRect( &rectSave, &r, &lprgn->rect ) )
				if( IntersectRect( &rectSave, &rCharBox, &lprgn->rect ) )
				
                {   // in rgn box, now decice rect which intersect with word
	                lprNodes = lprgn->lpNodeFirst;
	                if( !(uFormat & DT_NOCLIP) )
		                IntersectRect( &rectSave, &rectSave, &rectClip );
					//对每一个裁剪矩形与显示字符矩形求交，如果可以显示，则输出
                    while( lprNodes )
	                {
		                //if( IntersectRect( &r, &rectSave, &lprNodes->rect ) )
						if( IntersectRect( &rCharBox, &rectSave, &lprNodes->rect ) )						
		                {   // 有显示部分 not empty
                            //j = r.top - y;
							BOOL bShowBitmap;
							
							bShowBitmap = IntersectRect( &rCharBitmap, &r, &rCharBox );
                            rMask = rCharBitmap;
                            OffsetRect( &rMask, -xMaskOff, -yMaskOff );

                            if( bm )
                                //lpDispDrv->lpBlkBitBlt( &textBitblt );
								lpDispDrv->lpBlkBitBlt( &bkBitblt );
                            //else	//透明
							if( bShowBitmap )
                                lpDispDrv->lpBlkBitTransparentBlt( &textBitblt );

                            // 假如需要画下画线，则画之 draw under line if possible
                            if( fUnderLine && r.bottom == yUnderLine )
                            {
                                lineData.xStart = (short)r.left;
                                lineData.yStart = (short)yUnderLine;//r.top;2004-09-15,画在了上面
								lineData.cPels = r.right - r.left;
                                lineData.lprcClip = &r;
                                lpDispDrv->lpLine( &lineData );                                
                            }
		                }
						//下一个裁剪矩形
		                lprNodes = lprNodes->lpNext;
	                }
                }
                fUnderLine = FALSE;
				// 准备下一个显示字符 set next word
                x += textBitmap.xAdvance;//bitmap.bmWidth;
                lpstr += bw;
                num += bw;
            }
			// set next line y position
			//准备下一行显示字符
	        if( uFormat & DT_SINGLELINE )
				break;
	        y += fh;
			if( (uFormat & DT_NOCLIP) == 0 )
			{
			    if( y >= rectClip.bottom )
				    break;
			}
	        nCount -= num;
        }
		if( uFormat & DT_CALCRECT ) 
		{
			lpRect->left = lpRect->top = 0;
			lpRect->right = iTextWidth;
			lpRect->bottom = iTextHeight;
			//return iTextHeight;  // sucess , return Text Height
			retv = iTextHeight;	// sucess , return Text Height
		}
		else
		{
			//return y - rectClip.top;//iTextHeight;  // sucess , return Text Height
			retv = y - rectClip.top;//iTextHeight;  // sucess , return Text Height
		}
    }
_return:
	_LeaveDCPtr( lpdc, DCF_WRITE );
    return retv;
}

// **************************************************
// 声明：static BOOL _ExtTextOut( 
//							_LPGDCDATA lpdc,
//							int x,
//							int y,
//							UINT option,
//							LPCRECT lpRect,
//							LPCSTR lpstr,
//							UINT count,
//							int * lpNextPos )

// 参数：
// 	IN lpdc - 绘图DC指针
//	IN x - 文本显示开始点x坐标
//	IN y - 文本显示开始点y坐标
//	IN option-用lpcrc的方法,包含:
//			ETO_CLIPPED-文本被lpcrc裁剪,即矩形外的文本不能显示
//			ETO_OPAQUE-在显示文本前先用当前背景色填充
//	IN lpRect-RECT结构指针
//	IN lpstr-输出字符串
//	IN nCount-输出字符个数
//	IN lpNextPos-下一个字符输出位置
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	文本输出
// 引用: 
//	
// ************************************************

static BOOL _ExtTextOut( _LPGDCDATA lpdc,
						 int x,
						 int y,
                         UINT option,
						 LPCRECT lpRect,
                         LPCSTR lpstr,
                         UINT count,
                         int * lpNextPos )
{
	if( lpRect == NULL )
	{
		option &= ~(ETO_CLIPPED | ETO_OPAQUE);
	}

	if( lpdc )
    {
        _LPBITMAPDATA lpBitmap;
        _CHAR_METRICS textBitmap;
        _BRUSHDATA brushData;
        _LINEDATA lineData;
        RECT rectClip, r, rectSave, rMask, rCharBox, rCharBitmap;
        _FILLRGN aFillRgn;
        _BLKBITBLT textBitblt;
		_BLKBITBLT bkBitblt;
		_LPCDISPLAYDRV lpDispDrv = lpdc->lpDispDrv;

        const _RECTNODE FAR* lprNodes;
        _LPFONT lpFont;
		_LPFONTDRV lpFontDrv;
		HANDLE hFont;
        _LPRGNDATA lprgn;
        int xoff = XOFFSET( lpdc );
        int yoff = YOFFSET( lpdc );
        int j, cw;
        int num = 0, bm;
		int yTextHeight;

        lpBitmap = _GetHBITMAPPtr( lpdc->hBitmap );
        if( lpBitmap == 0 )
            return FALSE;
        textBitblt.backMode = lpdc->backMode;
		// make origin point

		if( lpdc->textAlign & TA_UPDATECP )
		{   //忽略原始的 x, y
		    x = lpdc->position.x;
			y = lpdc->position.y;
		}

        x += xoff;
        y += yoff;
        if( option & (ETO_CLIPPED|ETO_OPAQUE) )
        {	//需要做裁剪或需要清除背景
            rectClip = *lpRect;
			//逻辑坐标到设备坐标
            OffsetRect( &rectClip, xoff, yoff );
        }

		// clear background if possible
    
        if( option & ETO_OPAQUE )
        {   // 清除背景 clear background
            brushData.brushAttrib.style = BS_SOLID;
            brushData.brushAttrib.color = lpdc->backColor;

            aFillRgn.lprNodes = _GetHRGNPtr( lpdc->hrgn )->lpNodeFirst;
            aFillRgn.rect = rectClip;

            textBitblt.lpDestImage = lpBitmap;
            textBitblt.solidColor= lpdc->backColor;
            textBitblt.lpBrush = 0;  // fill use solidColor
			textBitblt.lpptBrushOrg = NULL;
            textBitblt.dwRop = PATCOPY;
            _FillRgn( lpDispDrv, &textBitblt, &aFillRgn );
        }

		if( !(lpstr && count && count != -1) )
			goto _return_success;

		lpFont = _GetHFONTPtr( lpdc->hFont );
		lpFontDrv = lpFont->lpDriver;
		hFont = lpFont->handle;

        // 如果用户指定了显示位置，则计算宽度 align border
		cw = 0;
		if( lpNextPos )
		{
			for( j = 0; (UINT)j < count; j++ )
				cw += *lpNextPos++;
		}
        if( (lpdc->textAlign & TA_RIGHT) ||
			(lpdc->textAlign & TA_CENTER)  )
		{	//处理右对齐和居中对齐
			int w = lpFontDrv->lpTextWidth( hFont, (LPCBYTE)lpstr, count ) + cw;
			if( (lpdc->textAlign & TA_RIGHT) )
	            x -= w;
			else  // ta_center
				x -= w / 2;
		}
		//底对齐
		yTextHeight = lpFontDrv->lpMaxHeight( hFont );
        if( lpdc->textAlign & TA_BOTTOM )
	        y -= yTextHeight;
		//初始化画线操作结构
        lineData.rop = R2_COPYPEN;
        lineData.lpDestImage = lpBitmap;
		lineData.lpfnCallback = NULL;

        textBitmap.bitmap.bmPlanes = 1;
        textBitmap.bitmap.bmBitsPixel = 1;
		//初始化画块操作结构
        textBitblt.lpDestImage = lpBitmap;
        textBitblt.lprcDest = &rCharBitmap;//r;
        textBitblt.lpBrush = 0;
		textBitblt.lpptBrushOrg = NULL;
        textBitblt.lpSrcImage = &textBitmap.bitmap;
        textBitblt.lprcSrc = &rMask;

        textBitblt.solidColor= lpdc->textColor;
        textBitblt.solidBkColor= lpdc->backColor;
        textBitblt.yPositive = 1;
        textBitblt.xPositive = 1;
        textBitblt.dwRop = SRCCOPY;

        bkBitblt.lpDestImage = lpBitmap;
        bkBitblt.lprcDest = &rCharBox;
        bkBitblt.lpBrush = 0;
		bkBitblt.lpptBrushOrg = NULL;
        bkBitblt.lpSrcImage = NULL;
        bkBitblt.lprcSrc =NULL;
        bkBitblt.solidColor= lpdc->backColor;
        //bkBitblt.solidBkColor= lpdc->backColor;
        //bkBitblt.yPositive = 1;
        //bkBitblt.xPositive = 1;
        bkBitblt.dwRop = PATCOPY;
		

        bm = lpdc->backMode == OPAQUE;

		// show word
        lprgn = _GetHRGNPtr( lpdc->hrgn );
        while( *lpstr && count )
        {	//得到字模
			int xMaskOff, yMaskOff;

            cw = lpFontDrv->lpWordBitmap( hFont, (LPCBYTE)lpstr, count, &textBitmap );
			//得到字符的显示矩形
            //r.left = x;
            //r.top = y;
            //r.right = x + textBitmap.bitmap.bmWidth;
            //r.bottom = y + textBitmap.bitmap.bmHeight;
			rCharBox.left = x;
			rCharBox.top = y;
            rCharBox.right = rCharBox.left + textBitmap.xAdvance;
            rCharBox.bottom = rCharBox.top + yTextHeight;//textBitmap.top + textBitmap.bitmap.bmHeight;//textBitmap.yAdvance;

            xMaskOff = r.left = x + textBitmap.left;
		    yMaskOff = r.top = y + textBitmap.top; 

            r.right = r.left + textBitmap.bitmap.bmWidth; 
            r.bottom = r.top + textBitmap.bitmap.bmHeight;

			//得到显示面，是否有显示面
            //if( IntersectRect( &rectSave, &r, &lprgn->rect ) )
			if( IntersectRect( &rectSave, &rCharBox, &lprgn->rect ) )
            {   // in rgn box, now decice rect which intersect with word
	            lprNodes = lprgn->lpNodeFirst;
	            if( option & ETO_CLIPPED )
				{
		            IntersectRect( &rectSave, &rectSave, &rectClip );
				}
				//对每一个裁剪矩形与字符求交，如果可以显示，则显示之
                while( lprNodes )
	            {	//
		            //if( IntersectRect( &r, &rectSave, &lprNodes->rect ) )
					if( IntersectRect( &rCharBox, &rectSave, &lprNodes->rect ) )
		            {   // not empty
                        //j = r.top - y;
						BOOL bShowBitmap;

                        bShowBitmap = IntersectRect( &rCharBitmap, &r, &rCharBox );
						rMask = rCharBitmap;
                        OffsetRect( &rMask, -xMaskOff, -yMaskOff );
						//显示
                        if( bm )
						{
                            //lpDispDrv->lpBlkBitBlt( &textBitblt );
							lpDispDrv->lpBlkBitBlt( &bkBitblt );							
						}
                        //else
						if( bShowBitmap )
                            lpDispDrv->lpBlkBitTransparentBlt( &textBitblt );
		            }
		            lprNodes = lprNodes->lpNext;
	            }
            }
			//准备下一个字符
	        if( lpNextPos != 0 )
			{
	            //x += textBitmap.bitmap.bmWidth + *lpNextPos++;
				x += textBitmap.xAdvance + *lpNextPos++;
			}
	        else
			{
	            //x += textBitmap.bitmap.bmWidth;
				x += textBitmap.xAdvance;
			}
	        num++;
            if( count > (UINT)cw )
                count -= cw;
			else
				break;
            lpstr += cw;
        }
_return_success:
		if( lpdc->textAlign & TA_UPDATECP )
		{
		    lpdc->position.x = x - xoff;
			lpdc->position.y = y - yoff;
		}

        return TRUE;
    }

    return FALSE;
}

// **************************************************
// 声明：BOOL WINAPI WinGdi_ExtTextOut( HDC hdc, 
//							  int x, 
//							  int y,
//							  UINT option,
//							  LPCRECT lpRect,
//							  LPCTSTR lpstr,
//							  UINT count,
//							  int * lpNextPos )
// 参数：
//	IN hdc-目标DC
//	IN x-输出点X坐标
//	IN y-输出点Y坐标
//	IN option-用lpcrc的方法
//			ETO_CLIPPED-文本被lpcrc裁剪,即矩形外的文本不能显示
//			ETO_OPAQUE-在显示文本前先用当前背景色填充
//	IN lpRect-RECT结构指针
//	IN lpstr-输出字符串
//	IN count-输出字符个数
//	IN lpNextPos-下一个字符输出位置

// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	文本输出
// 引用: 
//	系统API
// ************************************************

BOOL WINAPI WinGdi_ExtTextOut( HDC hdc, 
							  int x, 
							  int y,
							  UINT option,
							  LPCRECT lpRect,
							  LPCTSTR lpstr,
							  UINT count,
							  int * lpNextPos )
{
    _LPGDCDATA lpdc;
	//得到安全的DC指针
	BOOL retv = FALSE;
	if( ( lpdc = _GetSafeDrawPtr( hdc ) ) != NULL )
	{
		retv = _ExtTextOut( lpdc, x, y, option, lpRect, lpstr, count, lpNextPos );
	}
	_LeaveDCPtr( lpdc, DCF_WRITE );
	return retv;
	//return FALSE;
}

// **************************************************
// 声明：BOOL WINAPI WinGdi_TextOut( HDC hdc, int x, int y, LPCTSTR lpString, int count )
// 参数：
//	IN hdc-目标DC
//	IN x-输出点X坐标
//	IN y-输出点Y坐标
//	IN lpString-输出字符串
//	IN count-输出字符个数
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	文本输出
// 引用: 
//	系统API
// ************************************************

BOOL WINAPI WinGdi_TextOut( HDC hdc, int x, int y, LPCTSTR lpString, int count )
{
    _LPGDCDATA lpdc = NULL;
	BOOL retv = FALSE;

    if( lpString && 
		count > 0 &&
		(lpdc = _GetSafeDrawPtr( hdc ) ) != NULL )
    {	//是否透明输出 ？
        if( lpdc->backMode == OPAQUE )
	        retv = _ExtTextOut( lpdc, x, y, ETO_OPAQUE, 0, lpString, count, 0 );
        else	//否
	        retv = _ExtTextOut( lpdc, x, y, 0, 0, lpString, count, 0 );
    }
	_LeaveDCPtr( lpdc, DCF_WRITE );
	return retv;
    //return FALSE;
}

// **************************************************
// 声明：BOOL WINAPI WinGdi_TextOutW(
//							HDC hdc,
//							int x,
//							int y,
//							LPCTSTR lpString,
//							int nCount
//							)
// 参数：
//	IN hdc-目标DC
//	IN x-输出点x坐标
//	IN y-输出点y坐标
//	IN lpcstr-输出字符串（unicode 格式）
//	IN nCount-输出字符个数
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	文本输出, uincode 版本
// 引用: 
//	系统API
// ************************************************

BOOL WINAPI WinGdi_TextOutW(
							HDC hdc,
							int x,
							int y,
							LPCTSTR lpString,
							int nCount
							)
{
	const WORD * lpcwsStr = (const WORD *)lpString;
    _LPGDCDATA lpdc = NULL;
	BOOL retv = FALSE;
	//检查参数
    if( lpString &&
		nCount &&
		( lpdc = _GetSafeDrawPtr( hdc ) ) != NULL )
    {
        _LPFONT lpFont = _GetHFONTPtr( lpdc->hFont );
		_LPFONTDRV  lpFontDrv = lpFont->lpDriver;
		HANDLE hFont = lpFont->handle;	
		WORD cpOldMode = lpdc->textAlign;
		
		while( nCount > 0 )
		{
			int k = MIN( nCount, 64 );	//每次处理最大64个字节 cache 
			int nByte;
			BYTE bString[256];	//每次处理64个字节 cache 
			//先转化为单字节
			if( 0 != ( nByte = WideCharToMultiByte( CP_GB2312, 0, lpcwsStr, k, (LPSTR)bString, sizeof(bString), NULL, NULL ) ) )
			{	//输出
				if( lpdc->backMode == OPAQUE )
				{
					retv = _ExtTextOut( lpdc, x, y, ETO_OPAQUE, 0, (LPCSTR)bString, nByte, 0 );
					goto _return;
				}
				else
				{
					retv = _ExtTextOut( lpdc, x, y, 0, 0, (LPCSTR)bString, nByte, 0 );
					goto _return;
				}
				lpdc->textAlign |= TA_UPDATECP;
			}
			else
				break;
			nCount -= k;
			lpcwsStr += k;
		}
		lpdc->textAlign = cpOldMode;
		retv = TRUE;//return TRUE;
	}

_return:
	_LeaveDCPtr( lpdc, DCF_WRITE );
	//return FALSE;
	return retv;
}

// **************************************************
// 声明：COLORREF WINAPI WinGdi_GetTextColor( HDC hdc )
// 参数：
//	IN hdc-目标DC句柄
// 返回值：
//	假入成功，返回文本颜色；否则，返回CLR_INVALID
// 功能描述：
//	得到文本颜色
// 引用: 
//	系统API
// ************************************************

COLORREF WINAPI WinGdi_GetTextColor( HDC hdc )
{
    _LPGDCDATA lpdc = _GetHDCPtr( hdc );
    if( lpdc && lpdc->lpDispDrv )
        return lpdc->lpDispDrv->lpUnrealizeColor(lpdc->textColor, lpdc->lpdwPal, lpdc->wPalNumber, lpdc->wPalFormat);
     else
        return CLR_INVALID;
}

// **************************************************
// 声明：UINT WINAPI WinGdi_GetTextAlign( HDC hdc )
// 参数：
//	IN hdc-目标DC句柄
// 返回值：
//	假如成功，返回以下值的组合：
//		TA_BASELINE – 参考点是文本的基线
//		TA_BOTTOM – 参考点是矩形的底边
//		TA_TOP - 参考点是矩形的顶边
//		TA_CENTER - 参考点是矩形中心的水平线
//		TA_LEFT - 参考点是矩形的左边
//		TA_RIGHT - 参考点是矩形的右边
//		TA_NOUPDATECP – 当前的输出点在每一次文本输出后不会更新
//		TA_UPDATECP – 当前的输出点在每一次文本输出后会更新
//	假如不成功，返回GDI_ERROR
// 功能描述：
//	得到当前文本对齐方式
// 引用: 
//	系统API
// ************************************************

UINT WINAPI WinGdi_GetTextAlign( HDC hdc )
{
    _LPGDCDATA lpdc = _GetHDCPtr( hdc );
    if( lpdc )
        return lpdc->textAlign;
     else
        return GDI_ERROR;
}

// **************************************************
// 声明：static BOOL _GetTextExtentPoint32( 
//								HFONT hFont, 
//								_LPFONTDRV lpFontDrv, 
//                              LPCTSTR lpcstr, 
//								int nCount,
//								LPSIZE lpSize )
// 参数：
//	IN hFont - 字体对象句柄
//	IN lpFontDrv - 字体驱动程序
//	IN lpcstr - 文本指针(不以'\0'为结束副符,nLength决定文本长度)
//	IN nCount - 文本长度
//	OUT lpSize - SIZE结构指针,接收文本的长度和宽度
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	计算文本的长度和宽度
// 引用: 
//	
// ************************************************

static BOOL _GetTextExtentPoint32( HFONT hFont, 
								 _LPFONTDRV lpFontDrv, 
                                 LPCTSTR lpcstr, 
								 int nCount,
								 LPSIZE lpSize )
{
    int v;
    LPCTSTR lps;
    _CHAR_METRICS textBitmap;
	//先初始化为0
	lpSize->cx = lpSize->cy = 0;
	//求每一个字符的尺寸，并进行累加
	while( nCount > 0 )
	{
		lpFontDrv->lpWordBitmap( hFont, (LPCBYTE)lpcstr, nCount, &textBitmap );
		
		lpSize->cx += textBitmap.xAdvance;// textBitmap.bitmap.bmWidth;
		v = textBitmap.bitmap.bmHeight;
		lpSize->cy = MAX( v, lpSize->cy );
		lps = (LPCTSTR)lpFontDrv->lpNextWord( hFont, (LPCBYTE)lpcstr );
		nCount -= (lps - lpcstr);
		lpcstr = lps;
	}
	return TRUE;
}

// **************************************************
// 声明：BOOL WINAPI WinGdi_GetTextExtentPoint32( HDC hdc, LPCTSTR lpcstr, int nCount, LPSIZE lpSize )
// 参数：
//	IN hdc-对象句柄
//	IN lpcstr-文本指针(不以'\0'为结束副符,nLength决定文本长度)
//	IN nCount-文本长度
//	OUT lpSize-SIZE结构指针,接收文本的长度和宽度
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	计算文本的长度和宽度
// 引用: 
//	系统API
// ************************************************

BOOL WINAPI WinGdi_GetTextExtentPoint32( HDC hdc, LPCTSTR lpcstr, int nCount, LPSIZE lpSize )
{
    _LPGDCDATA lpdc;
	//得到有效的DC 指针
    if( (lpdc = _GetHDCPtr( hdc )) != 0 )
    {
        _LPFONT lpFont = _GetHFONTPtr( lpdc->hFont );
		_LPFONTDRV  lpFontDrv = lpFont->lpDriver;
		HANDLE hFont = lpFont->handle;

		return _GetTextExtentPoint32( hFont, lpFontDrv, lpcstr, nCount, lpSize );
    }
    return FALSE;
}


// **************************************************
// 声明：BOOL WINAPI WinGdi_GetTextExtentPoint32W( HDC hdc, LPCTSTR lpcstr, int nCount, LPSIZE lpSize )
// 参数：
//	IN hdc-对象句柄
//	IN lpcstr-文本指针(不以'\0'为结束副符,nLength决定文本长度， unicode格式)
//	IN nCount-文本长度
//	OUT lpSize-SIZE结构指针,接收文本的长度和宽度
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	计算文本的长度和宽度（ unicode 版本 ）
// 引用: 
//	系统API
// ************************************************

BOOL WINAPI WinGdi_GetTextExtentPoint32W( HDC hdc, LPCTSTR lpcstr, int nCount, LPSIZE lpSize )
{
    _LPGDCDATA lpdc;
    if( lpSize &&
		lpcstr &&
		(lpdc = _GetHDCPtr( hdc )) != NULL )
    {
        _LPFONT lpFont = _GetHFONTPtr( lpdc->hFont );
		_LPFONTDRV  lpFontDrv = lpFont->lpDriver;
		HANDLE hFont = lpFont->handle;	
		SIZE size;
		BYTE bString[256];
		const WORD * lpcwsStr = (const WORD *)lpcstr;

		lpSize->cy = lpSize->cx = 0;		
		while( nCount > 0 )
		{
			int k = MIN( nCount, 64 );	//最大64个字节
			int nByte;
			//先将unicode格式转化为单字节格式
			if( 0 != ( nByte = WideCharToMultiByte( CP_GB2312, 0, lpcwsStr, k, (LPSTR)bString, sizeof(bString), NULL, NULL ) ) )
			{
				_GetTextExtentPoint32( hFont, lpFontDrv, (LPCSTR)bString, nByte, &size );
				lpSize->cx += size.cx;
				lpSize->cy = MAX( lpSize->cy, size.cy );
			}
			else
				break;
			nCount -= k;
			lpcwsStr += k;
		}
		return TRUE;
	}

	return FALSE;
}


// **************************************************
// 声明：BOOL WINAPI WinGdi_GetTextExtentExPoint( 
//									HDC hdc,
//									LPCTSTR lpszStr,
//									int nString,
//									int nMaxExtent,
//									LPINT lpFit,
//									LPINT lpDx,
//									LPSIZE lpSize )
// 参数：
//	IN hdc-对象句柄
//	IN lpszStr-文本指针(不以'\0'为结束副符,nLength决定文本长度)
//	IN nString-文本长度
//	IN nMaxExtent-文本的最大许可计算宽度
//	OUT lpFit-用于接受限制长度内的文本字符个数
//	OUT lpDx-用于接受第一个字符到第N字符的文本的长度
//	OUT lpSize-SIZE结构指针,用于接收文本的长度和宽度
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	计算文本的长度和宽度
// 引用: 
//	
// ************************************************

BOOL WINAPI WinGdi_GetTextExtentExPoint( HDC hdc,
                               LPCTSTR lpszStr,
                               int nString,
                               int nMaxExtent,
                               LPINT lpFit,
                               LPINT lpDx,
                               LPSIZE lpSize )
{
    _LPGDCDATA lpdc;

    if( (lpdc = _GetHDCPtr( hdc )) != 0 )
    {
        _LPFONT lpFont = _GetHFONTPtr( lpdc->hFont );
        _LPFONTDRV lpFontDrv = lpFont->lpDriver;
	    HANDLE hFont = lpFont->handle;
        _CHAR_METRICS textBitmap;
        int v, ws;

        lpSize->cx = lpSize->cy = 0;
        while( nString > 0 )
        {
       		if( lpFit && lpSize->cx >= nMaxExtent )
				 break;
			//得到字符属性（宽和高）
			ws = lpFontDrv->lpWordBitmap( hFont, (LPCBYTE)lpszStr, nString, &textBitmap );
			//累加相关的值
            lpSize->cx += textBitmap.xAdvance;//bitmap.bmWidth;
            v = textBitmap.bitmap.bmHeight;
            lpSize->cy = MAX( v, lpSize->cy );
			
            if( lpDx )
                *lpDx++ = lpSize->cx;
            if( lpFit )
                (*lpFit)++;
			//下一个字符
			nString -= ws;
			lpszStr += ws;
        }
        return TRUE;
    }
    return FALSE;
}

// **************************************************
// 声明：COLORREF WINAPI WinGdi_SetTextColor( HDC hdc, COLORREF color )
// 参数：
// 	IN hdc - DC句柄
//	IN color - RGB颜色值
// 返回值：
//	成功：返回老的RGB值
//	否则：返回CLR_INVALID
// 功能描述：
//	设置文本颜色
// 引用: 
//	
// ************************************************

COLORREF WINAPI WinGdi_SetTextColor( HDC hdc, COLORREF color )
{
    _LPGDCDATA lpdc=_GetHDCPtr( hdc );

    if( lpdc && lpdc->lpDispDrv )
    {
        int c = lpdc->textColor;
		//得到设备依赖的颜色值
        lpdc->textColor = lpdc->lpDispDrv->lpRealizeColor(color, lpdc->lpdwPal, lpdc->wPalNumber, lpdc->wPalFormat);
		//得到设备不依赖的颜色值（RGB）
        return lpdc->lpDispDrv->lpUnrealizeColor(c, lpdc->lpdwPal, lpdc->wPalNumber, lpdc->wPalFormat);
    }
    return CLR_INVALID;
}

// **************************************************
// 声明：UINT WINAPI WinGdi_SetTextAlign( HDC hdc, UINT uiAlignMode )
// 参数：
//	IN hdc - DC句柄
//	IN uiAlignMode – 新的对齐模式，可以为以下值：
//			TA_BASELINE – 参考点是文本的基线
//			TA_BOTTOM – 参考点是矩形的底边
//			TA_TOP - 参考点是矩形的顶边
//			TA_CENTER - 参考点是矩形中心的水平线
//			TA_LEFT - 参考点是矩形的左边
//			TA_RIGHT - 参考点是矩形的右边
//			TA_NOUPDATECP – 当前的输出点在每一次文本输出后不会更新
//			TA_UPDATECP – 当前的输出点在每一次文本输出后会更新
// 返回值：
//	假如成功，返回之前的模式值；假如不成功，返回GDI_ERROR
// 功能描述：
//	设置文本对齐模式
// 引用: 
//	系统API
// ************************************************

UINT WINAPI WinGdi_SetTextAlign( HDC hdc, UINT uiAlignMode )
{
    _LPGDCDATA lpdc=_GetHDCPtr( hdc );

    if( lpdc )
    {
        UINT old = lpdc->textAlign;

        lpdc->textAlign = uiAlignMode;
		return old;
    }
    return GDI_ERROR;
}

// **************************************************
// 声明：COLORREF WINAPI WinGdi_GetBkColor( HDC hdc )
// 参数：
//	IN hdc-目标DC
// 返回值：
//	成功：返回颜色
//	否则：返回CLR_INVALID
// 功能描述：
//	得到背景色
// 引用: 
//	系统API
// ************************************************

COLORREF WINAPI WinGdi_GetBkColor( HDC hdc )
{
    _LPGDCDATA lpdc = _GetHDCPtr( hdc );

    if( lpdc && lpdc->lpDispDrv )
        return lpdc->lpDispDrv->lpUnrealizeColor(lpdc->backColor, lpdc->lpdwPal, lpdc->wPalNumber, lpdc->wPalFormat);//得到设备不依赖的颜色值
     return CLR_INVALID;
}

// **************************************************
// 声明：COLORREF WINAPI WinGdi_SetBkColor( HDC hdc, COLORREF color )
// 参数：
//	IN hdc-目标DC
//	IN color-RGB颜色
// 返回值：
//	假如成功,返回老的背景色;否则,返回CLR_INVALID
// 功能描述：
//	设置背景色
// 引用: 
//	系统API
// ************************************************

COLORREF WINAPI WinGdi_SetBkColor( HDC hdc, COLORREF color )
{
    _LPGDCDATA lpdc=_GetHDCPtr( hdc );

    if( lpdc && lpdc->lpDispDrv )
    {
        COLORREF c = lpdc->backColor;
		//得到设备依赖的颜色值
        lpdc->backColor = lpdc->lpDispDrv->lpRealizeColor(color, lpdc->lpdwPal, lpdc->wPalNumber, lpdc->wPalFormat);
        return lpdc->lpDispDrv->lpUnrealizeColor(c, lpdc->lpdwPal, lpdc->wPalNumber, lpdc->wPalFormat);//返回设备不依赖的颜色值
    }
    return CLR_INVALID;
}

// **************************************************
// 声明：int WINAPI WinGdi_GetBkMode( HDC hdc )
// 参数：
// 	IN hdc-目标DC
// 返回值：
//	成功：返回背景模式
//	否则：返回0
// 功能描述：
//	得到背景模式
// 引用: 
//	系统API
// ************************************************

int WINAPI WinGdi_GetBkMode( HDC hdc )
{
    _LPGDCDATA lpdc;

    if( (lpdc=_GetHDCPtr( hdc )) != 0 )
        return lpdc->backMode;
    return 0;
}

// **************************************************
// 声明：int WINAPI WinGdi_SetBkMode( HDC hdc, int iMode )
// 参数：
//	IN hdc-目标DC
//	IN mode-背景模式,包含：
//			OPAQUE-在输出文本,刷子,笔时,背景先用当前背景色填充
//			TRANSPARENT-背景色保持不变
// 返回值：
//	假如成功,返回老的背景模式;否则,返回0
// 功能描述：
//	设置背景模式
// 引用: 
//	系统API
// ************************************************

int WINAPI WinGdi_SetBkMode( HDC hdc, int iMode )
{
    _LPGDCDATA lpdc;
    if( (lpdc = _GetHDCPtr( hdc )) != 0 )
    {
        int m = lpdc->backMode;
        lpdc->backMode = iMode;
        return m;
    }
    return 0;
}

// **************************************************
// 声明：HGDIOBJ WINAPI WinGdi_SelectObject(HDC hdc, HGDIOBJ hgdi)
// 参数：
//	IN hdc-目标DC
//	IN handle-对象句柄 ,包含：
//			HBITMAP-位图对象
//			HBRUSH-刷子对象
//			HFONT-字体对象
//			HPEN-笔对象
//			HRGN-区域对象
// 返回值：
//	成功:若选进的对象不是区域对象,返回相同类型的老对象
//	否则,返回以下值：
//	    SIMPLEREGION-区域有一个矩形
//	    COMPLEXREGION-区域有多个矩形
//	    NULLREGION-区域为空失败:若选进的对象不是区域对象, 返回GDI_ERROR;
//					否则,返回NULL
// 功能描述：
//	将对象选进设备
// 引用: 
//	系统API
// ************************************************
#define DEBUG_SELECTOBJECT 0
HGDIOBJ WINAPI WinGdi_SelectObject(HDC hdc, HGDIOBJ hgdi)
{
    _LPGDCDATA lpdc = _GetHDCPtr( hdc );
    HGDIOBJ hold=(HGDIOBJ)GDI_ERROR;

    if( lpdc  )
    {
        _LPBITMAPDATA lpBitmap;
        UINT type = WinGdi_GetObjectType( hgdi );

		if( type != OBJ_BITMAP && lpdc->lpDispDrv == 0 )
		{
			ASSERT( 0 );
			return (HGDIOBJ)GDI_ERROR;
		}

        if( type == OBJ_PEN )
        {	//笔对象
            _LPPENDATA lpPen = _GetHPENPtr( hgdi );
			if( lpPen )
			{
				hold = lpdc->hPen;
				lpdc->hPen = hgdi;
				lpdc->penAttrib = lpPen->penAttrib;
				lpdc->penAttrib.color = lpdc->lpDispDrv->lpRealizeColor( lpdc->penAttrib.clrRef, lpdc->lpdwPal, lpdc->wPalNumber, lpdc->wPalFormat );
			}
			else
				return (HGDIOBJ)GDI_ERROR;
        }
        else if( type == OBJ_BRUSH )
        {	//刷子对象
            _LPBRUSHDATA lpBrush = _GetHBRUSHPtr( hgdi );

			if( lpBrush )
			{		
				hold = lpdc->hBrush;
				lpdc->hBrush = hgdi;
				lpdc->brushAttrib = lpBrush->brushAttrib;
				lpdc->brushAttrib.color = lpdc->lpDispDrv->lpRealizeColor( lpdc->brushAttrib.clrRef, lpdc->lpdwPal, lpdc->wPalNumber, lpdc->wPalFormat );
				//lpdc->ptBrushOrg.x = lpdc->ptBrushOrg.y = 0;
				lpdc->ptBrushOrg = lpdc->deviceOrg;
			}
			else
				return (HGDIOBJ)GDI_ERROR;
        }
        else if( type == OBJ_BITMAP )
        {	//位图对象
            if( lpdc->obj.objType == OBJ_MEMDC )
            {   // 内存DC mem dc
				_LPCDISPLAYDRV lpdd = NULL;
                
				hold = lpdc->hBitmap;
				ASSERT( hold );

                lpBitmap = _GetHBITMAPPtr( hgdi );
				lpdd = GetDisplayDeviceDriver( lpBitmap );
				if( lpdd )
				{
					lpdc->hBitmap = hgdi;
					lpdc->lpDispDrv = lpdd;
					
					//设置调色板
					if( lpBitmap->bmFlags & BF_DIB )
					{
						_LPBITMAP_DIB lpSrcDIB = (_LPBITMAP_DIB)lpBitmap;
						lpdc->wPalNumber = (WORD)lpSrcDIB->biClrUsed;
						if( lpdc->wPalNumber )
						{
						    lpdc->lpdwPal = (LPDWORD)lpSrcDIB->palEntry;
							if( lpSrcDIB->biCompression == BI_RGB )
								lpdc->wPalFormat = PAL_INDEX;
							else if( lpSrcDIB->biCompression == BI_BITFIELDS )
								lpdc->wPalFormat = PAL_BITFIELD;  //颜色值为位图域
						}
						else
						{
							lpdc->lpdwPal = 0;
							lpdc->wPalFormat = 0;
						}
					}
					else
					{
						lpdc->lpdwPal = 0;
						lpdc->wPalFormat = 0;
						lpdc->wPalNumber = 0;
					}

					lpdc->deviceOrg.x = lpdc->deviceOrg.y = 0;
					lpdc->deviceExt.cx = (short)lpBitmap->bmWidth;
					lpdc->deviceExt.cy = (short)lpBitmap->bmHeight;
					// 需要设置为设备依赖颜色值
                    lpdc->backColor = lpdd->lpRealizeColor( CL_WHITE, lpdc->lpdwPal, lpdc->wPalNumber, lpdc->wPalFormat );
                    lpdc->textColor = lpdd->lpRealizeColor( CL_BLACK, lpdc->lpdwPal, lpdc->wPalNumber, lpdc->wPalFormat );
					// 需要设置为设备依赖颜色值
					lpdc->brushAttrib.color = lpdd->lpRealizeColor( lpdc->brushAttrib.clrRef, lpdc->lpdwPal, lpdc->wPalNumber, lpdc->wPalFormat );
					lpdc->penAttrib.color = lpdd->lpRealizeColor( lpdc->penAttrib.clrRef, lpdc->lpdwPal, lpdc->wPalNumber, lpdc->wPalFormat );
					// 需要显示裁剪区域为整个显示面
					WinRgn_SetRect( lpdc->hrgn,
						lpdc->deviceOrg.x,
						lpdc->deviceOrg.y,
						lpdc->deviceOrg.x + lpdc->deviceExt.cx,
						lpdc->deviceOrg.y + lpdc->deviceExt.cy );
				}
				else
				{   // no driver to support 					
					ERRORMSG( DEBUG_SELECTOBJECT, ( "no supprt the bitmap format: bmBitsPixel=%d,bmPlanes=%d.\r\n", lpBitmap->bmBitsPixel, lpBitmap->bmPlanes ) );
					return (HGDIOBJ)GDI_ERROR;
				}
            }
			else
			{
				ASSERT( 0 );
			}
        }
        else if( type == OBJ_REGION )
        {   //裁剪区域
			hold = (HGDIOBJ)DoExtSelectClipRgn( hdc, hgdi, RGN_COPY );			
        }
        else if( type == OBJ_FONT )
        {	//字体对象
            hold = lpdc->hFont;
            lpdc->hFont = hgdi;
        }
        else
		{	//非法对象
            ASSERT( 0 );
			WARNMSG( DEBUG_SELECTOBJECT, ( "error in SelectObject:hgdi=0x%x.\r\n", hgdi ) );
		}
    }
    return hold;
}

// **************************************************
// 声明：static int DoExtSelectClipRgn( HDC hdc, HRGN hrgn, int mode )
// 参数：
//	IN hdc - DC句柄
//	IN hrgn – 区域句柄
//	IN iMode – 与当前DC里的裁剪区域组合模式，可以为以下值：
//			RGN_AND – 新的裁剪区域为当前DC里的裁剪区域与hrgn进行“与”（AND）运算
//			RGN_COPY - 新的裁剪区域为hrgn的拷贝，假如hrgn为NULL，新的裁剪区域为空
//			RGN_DIFF - 新的裁剪区域为当前DC里的裁剪区域除去hrgn所剩下的部分
//			RGN_OR -新的裁剪区域为当前DC里的裁剪区域与hrgn进行“或”（OR）运算
//			RGN_XOR -新的裁剪区域为当前DC里的裁剪区域与hrgn进行“异或”（XOR）运算
// 返回值：
//	NULLREGION - 裁剪区域是空的
//	SIMPLEREGION - 裁剪区域只有一个矩形
//	COMPLEXREGION - 裁剪区域含多个矩形
//	ERROR – 错误
// 功能描述：
//	选入裁剪区域到DC
// 引用: 
//	
// ************************************************
#define DEBUG_EXT_SELECT_CLIP_RGN 0
static int DoExtSelectClipRgn( HDC hdc, HRGN hrgn, int mode )
{
	int iretv = ERROR;
    _LPGDCDATA lpdc = _GetHDCPtr( hdc );
    if( lpdc )
    {
		HRGN hrgnExpose = NULL;
		if( lpdc->hwnd )
		{	//该DC是与窗口关联的，必须与窗口的显示区域进行相关操作
			if( (iretv = GetExposeRgn( lpdc->hwnd, lpdc->uiFlags, &hrgnExpose )) == ERROR )
			{
				WARNMSG( DEBUG_EXT_SELECT_CLIP_RGN, ( "error at DoExtSelectClipRgn.\r\n" ) );
				goto _return;
			}
			if( iretv == NULLREGION )
			{	//当前窗口无可显示区域，直接返回
				WinGdi_DeleteObject( hrgnExpose );
				goto _return;
			}
		}
		else
		{	//
			hrgnExpose = WinRgn_CreateRect( lpdc->deviceOrg.x, lpdc->deviceOrg.y, lpdc->deviceOrg.x + lpdc->deviceExt.cx, lpdc->deviceOrg.y + lpdc->deviceExt.cy );
		}

		if( hrgnExpose )
		{	//
			if( hrgn )  
			{	//将hrgn转化到设备坐标
				WinRgn_Offset( hrgn, lpdc->deviceOrg.x, lpdc->deviceOrg.y );
				//与hrgnExpose相交
				WinRgn_Combine( hrgnExpose, hrgnExpose, hrgn, RGN_AND );
				//恢复
				WinRgn_Offset( hrgn, -lpdc->deviceOrg.x, -lpdc->deviceOrg.y );
			}
			//按mode进行操作
			if( mode = RGN_COPY )
				iretv = WinRgn_Combine( lpdc->hrgn, hrgnExpose, NULL, mode );
			else
				iretv = WinRgn_Combine( lpdc->hrgn, lpdc->hrgn, hrgnExpose, mode );
			WinGdi_DeleteObject( hrgnExpose );
		}
    }
_return:
    return iretv;
}

// **************************************************
// 声明：int WINAPI WinGdi_ExtSelectClipRgn( HDC hdc, HRGN hrgn, int mode )
// 参数：
//	IN hdc - DC句柄
//	IN hrgn – 区域句柄
//	IN mode – 与当前DC里的裁剪区域组合模式，可以为以下值：
//			RGN_AND – 新的裁剪区域为当前DC里的裁剪区域与hrgn进行“与”（AND）运算
//			RGN_COPY - 新的裁剪区域为hrgn的拷贝，假如hrgn为NULL，新的裁剪区域为空
//			RGN_DIFF - 新的裁剪区域为当前DC里的裁剪区域除去hrgn所剩下的部分
//			RGN_OR -新的裁剪区域为当前DC里的裁剪区域与hrgn进行“或”（OR）运算
//			RGN_XOR -新的裁剪区域为当前DC里的裁剪区域与hrgn进行“异或”（XOR）运算
// 返回值：
//	NULLREGION - 裁剪区域是空的
//	SIMPLEREGION - 裁剪区域只有一个矩形
//	COMPLEXREGION - 裁剪区域含多个矩形
//	ERROR – 错误
// 功能描述：
//	将裁剪区域与DC当前的裁剪区域做相关操作后 选入裁剪区域到DC
// 引用: 
//	系统API
// ************************************************

// the GetSafeDrawPtr will return NULL when rgn == NULL_REGION
int WINAPI WinGdi_ExtSelectClipRgn( HDC hdc, HRGN hrgn, int mode )
{
	return DoExtSelectClipRgn( hdc, hrgn, mode );
}

// **************************************************
// 声明：int WINAPI WinGdi_SelectClipRgn( HDC hdc, HRGN hrgn )
// 参数：
//	IN hdc - DC句柄
//	IN hrgn – 区域句柄
// 返回值：
//	NULLREGION - 裁剪区域是空的
//	SIMPLEREGION - 裁剪区域只有一个矩形
//	COMPLEXREGION - 裁剪区域含多个矩形
//	ERROR – 错误
// 功能描述：
//	选入裁剪区域到DC
// 引用: 
//	系统API
// ************************************************

// the GetSafeDrawPtr will return NULL when rgn == NULL_REGION
int WINAPI WinGdi_SelectClipRgn( HDC hdc, HRGN hrgn )
{
	return DoExtSelectClipRgn( hdc, hrgn, RGN_COPY );
}

// **************************************************
// 声明：int WINAPI WinGdi_GetClipBox( HDC hdc, LPRECT lpRect )
// 参数：
//	IN hdc-目标DC
//	OUT lpRect-用于得到矩形, RECT结构指针
// 返回值：
//		NULLREGION-新区域为空
//		SIMPLEREGION-新区域有一个矩形
//		COMPLEXREGION0-新区域有多个矩形
//		ERROR-错误
// 功能描述：
//	得到设备裁剪区域的边界
// 引用: 
//	系统API
// ************************************************

// the GetSafeDrawPtr will return NULL when rgn == NULL_REGION
int WINAPI WinGdi_GetClipBox( HDC hdc, LPRECT lpRect )
{
    _LPGDCDATA lpdc = _GetHDCPtr( hdc );
	int retv = ERROR;

    if( lpdc )
	{
        retv = WinRgn_GetBox( lpdc->hrgn, lpRect );
		//转化到逻辑坐标
		OffsetRect( lpRect, -lpdc->deviceOrg.x, -lpdc->deviceOrg.y );
	}
	return retv;
}

// **************************************************
// 声明：int WINAPI WinGdi_GetClipRgn( HDC hdc, HRGN hrgn )
// 参数：
//	IN hdc-目标DC
//  IN/OUT hrgn-用于得到区域的句柄
// 返回值：
//		0-没有区域(空区域)
//		1-有区域
//		ERROR-错误
// 功能描述：
//	得到设备区域
// 引用: 
//	系统API
// ************************************************

// the GetSafeDrawPtr will return NULL when rgn == NULL_REGION
int WINAPI WinGdi_GetClipRgn( HDC hdc, HRGN hrgn )
{
    _LPGDCDATA lpdc = _GetHDCPtr( hdc );
    if( lpdc )
    {	//拷贝到hrgn
		int retv = WinRgn_Combine( hrgn, lpdc->hrgn, 0, RGN_COPY );
		if( retv == NULLREGION )
			return 0;
		else if( retv != ERROR )
		{
			WinRgn_Offset( hrgn, -lpdc->deviceOrg.x, -lpdc->deviceOrg.y );
			return 1;
		}
    }
    return -1;
}

// **************************************************
// 声明：HGDIOBJ WINAPI WinGdi_GetCurrentObject( HDC hdc, UINT uObjectType )
// 参数：
//	IN hdc-目标DC
//	IN uObjectType-对象类型，包含：
//	    OBJ_PEN-笔对象
//	    OBJ_BRUSH-刷子对象
//	    OBJ_FONT-字体对象
//	    OBJ_BITMAP-位图对象
// 返回值：
//	成功：返回相应对象
//	否则：返回NULL
// 功能描述：
//	得到当前被选进设备的对象
// 引用: 
//	系统API
// ************************************************

HGDIOBJ WINAPI WinGdi_GetCurrentObject( HDC hdc, UINT uObjectType )
{
    _LPGDCDATA lpdc;

    if( (lpdc = _GetHDCPtr( hdc )) != 0 )
    {
        switch( uObjectType )
        {
            case OBJ_PEN:
                return lpdc->hPen;
            case OBJ_BRUSH:
                return lpdc->hBrush;
            case OBJ_FONT:
                return lpdc->hFont;
            case OBJ_BITMAP:
                return lpdc->hBitmap;
            default:
                ASSERT(0);
        }
    }
    return 0;
}

// **************************************************
// 声明：BOOL WINAPI WinGdi_GetIconInfo( HICON hIcon, LPICONINFO lpInfo )
// 参数：
//	IN hIcon - 图标句柄
//	IN lpInfo –ICONINFO数据结构指针，用于接受图标信息的
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	得到图标信息
// 引用: 
//	系统API
// ************************************************

BOOL WINAPI WinGdi_GetIconInfo( HICON hIcon, LPICONINFO lpInfo )
{
    _LPICONDATA lpIconData = _GetHICONPtr( hIcon );

    if( lpIconData )
    {
        *lpInfo = *((LPICONINFO)&lpIconData->fIcon);
        return TRUE;
    }
	else
	{
		memset( lpInfo, 0, sizeof( *lpInfo ) );
	}
    return FALSE;
}

// **************************************************
// 声明：DWORD WINAPI WinGdi_GetObjectType( HGDIOBJ h )
// 参数：
// 	IN h - 对象句柄
// 返回值：
//	成功：返回对象类型
//	对象类型包含：
//		OBJ_BITMAP-位图
//		OBJ_BRUSH-刷子
//		OBJ_FONT-字体
//		OBJ_PEN-笔
//		OBJ_REGION-区域
//		OBJ_DC-dc
//		OBJ_MEMDC-内存dc
//	否则：返回0
// 功能描述：
//	得到对象类型
// 引用: 
//	系统 API
// ************************************************

DWORD WINAPI WinGdi_GetObjectType( HGDIOBJ h )
{
	LPVOID lpvData = (LPVOID)HANDLE_TO_PTR( h );
	if( lpvData )
        return (WORD)( *((DWORD*)lpvData) );
	SetLastError( ERROR_INVALID_PARAMETER  );
	return 0;
}

// **************************************************
// 声明：int WINAPI WinGdi_GetObjectInfo( HGDIOBJ h, int bufferSize, void * lpBuf )
// 参数：
//	IN h-对象句柄
//	IN bufferSize-接受缓存的大小
//	IN lpBuf-用于接受对象信息的缓存
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	假如成功并且lpvBuf是有效地址,返回数据大小,假如lpvBuf是NULL, 返回数据大小;假如失败,返回0
// 引用: 
//	系统API
// ************************************************

int WINAPI WinGdi_GetObjectInfo( HGDIOBJ h, int bufferSize, void * lpBuf )
{
    union {
        _LPPENDATA lpPen;
        _LPBRUSHDATA lpBrush;
        _LPBITMAPDATA lpBitmap;
        _LPFONT lpFont;
		_LPBITMAP_DIB lpDIB;
    }obj;
    int retv = 0;

    switch( WinGdi_GetObjectType( h ) )
    {
        case OBJ_BITMAP:			//位图对象
            retv = sizeof( BITMAP );
			if( lpBuf )
			{				
				if( bufferSize == sizeof( BITMAP ) ||
					bufferSize == sizeof(DIBSECTION) )
				{
					obj.lpBitmap = _GetHBITMAPPtr( h );
					((LPBITMAP)lpBuf)->bmType = 0;
					((LPBITMAP)lpBuf)->bmWidth = obj.lpBitmap->bmWidth;
					((LPBITMAP)lpBuf)->bmHeight = obj.lpBitmap->bmHeight;
					((LPBITMAP)lpBuf)->bmWidthBytes = obj.lpBitmap->bmWidthBytes;
					((LPBITMAP)lpBuf)->bmPlanes = obj.lpBitmap->bmPlanes;
					((LPBITMAP)lpBuf)->bmBitsPixel = obj.lpBitmap->bmBitsPixel;
					if( obj.lpBitmap->bmFlags & BF_DIB )
					{
						((LPBITMAP)lpBuf)->bmBits = obj.lpBitmap->bmBits;
					}
					else
						((LPBITMAP)lpBuf)->bmBits = 0;
					if( bufferSize == sizeof(DIBSECTION) )
					{
						((LPDIBSECTION)lpBuf)->dsBmih.biSize = sizeof(BITMAPINFOHEADER);
						((LPDIBSECTION)lpBuf)->dsBmih.biBitCount = obj.lpDIB->bitmap.bmBitsPixel;
						((LPDIBSECTION)lpBuf)->dsBmih.biClrImportant = obj.lpDIB->biClrImportant;
						((LPDIBSECTION)lpBuf)->dsBmih.biClrUsed = obj.lpDIB->biClrUsed;
						((LPDIBSECTION)lpBuf)->dsBmih.biCompression = obj.lpDIB->biCompression;
						((LPDIBSECTION)lpBuf)->dsBmih.biHeight = obj.lpDIB->biDir < 0 ? obj.lpDIB->bitmap.bmHeight : -obj.lpDIB->bitmap.bmHeight;
						((LPDIBSECTION)lpBuf)->dsBmih.biPlanes = obj.lpDIB->bitmap.bmPlanes;
						((LPDIBSECTION)lpBuf)->dsBmih.biSizeImage = obj.lpDIB->biSizeImage;
						((LPDIBSECTION)lpBuf)->dsBmih.biWidth = obj.lpDIB->bitmap.bmWidth;
						((LPDIBSECTION)lpBuf)->dsBmih.biXPelsPerMeter = obj.lpDIB->biXPelsPerMeter;
						((LPDIBSECTION)lpBuf)->dsBmih.biYPelsPerMeter = obj.lpDIB->biYPelsPerMeter;
						((LPDIBSECTION)lpBuf)->dsOffset = 0;
						((LPDIBSECTION)lpBuf)->dshSection = NULL;
						if( obj.lpDIB->bitmap.bmBitsPixel > 8 && obj.lpDIB->biClrUsed )
						{
							ASSERT( obj.lpDIB->biClrUsed == 3 );
							((LPDIBSECTION)lpBuf)->dsBitfields[0] = *( (DWORD*)&obj.lpDIB->palEntry[0] );
							((LPDIBSECTION)lpBuf)->dsBitfields[1] = *( (DWORD*)&obj.lpDIB->palEntry[1] );
							((LPDIBSECTION)lpBuf)->dsBitfields[2] = *( (DWORD*)&obj.lpDIB->palEntry[2] );
						}
						else
						{
							memset( ((LPDIBSECTION)lpBuf)->dsBitfields, 0, sizeof(DWORD) * 3 );
						}
						retv = sizeof(DIBSECTION);
					}
				}
				else
				{
					WARNMSG( 0, ("Call GetObject Error: ptr size error.\r\n") );
					retv = 0;
				}
			}
            break;
        case OBJ_PEN:			//笔对象
            retv = sizeof( LOGPEN );
			if( lpBuf )
			{
				if( bufferSize >= retv )
				{				
					obj.lpPen = _GetHPENPtr( h );
					((LPLOGPEN)lpBuf)->lognStyle = obj.lpPen->penAttrib.pattern;
					((LPLOGPEN)lpBuf)->lognWidth.x = obj.lpPen->penAttrib.width;
					((LPLOGPEN)lpBuf)->lognWidth.y = obj.lpPen->penAttrib.width;
					((LPLOGPEN)lpBuf)->lognColor = obj.lpPen->penAttrib.clrRef;
				}
				else
				{
					WARNMSG( 0, ("Call GetObject Error: ptr size error.\r\n") );
					retv = 0;
				}
			}
            break;
        case OBJ_BRUSH:			//刷子对象
            retv = sizeof( LOGBRUSH );
			if( lpBuf )
			{
				if( bufferSize >= retv )
				{
					obj.lpBrush = _GetHBRUSHPtr( h );
					((LPLOGBRUSH)lpBuf)->lbStyle = obj.lpBrush->brushAttrib.style;
					((LPLOGBRUSH)lpBuf)->lbColor = obj.lpBrush->brushAttrib.clrRef;
					((LPLOGBRUSH)lpBuf)->lbHatch = 0;
				}
				else
				{
					WARNMSG( 0, ("Call GetObject Error: ptr size error.\r\n") );
					retv = 0;
				}
			}
            break;
        case OBJ_FONT:			//字体对象
            retv = sizeof( LOGFONT );
			if( lpBuf )
			{
				if( bufferSize >= retv )
				{
					_LPFONTDRV lpFontDrv;
					HANDLE hFont;
					
					obj.lpFont = _GetHFONTPtr( h );
					lpFontDrv = obj.lpFont->lpDriver;
					hFont = obj.lpFont->handle;
					
					((LPLOGFONT)lpBuf)->lfHeight = lpFontDrv->lpMaxHeight( hFont );
					((LPLOGFONT)lpBuf)->lfWidth = lpFontDrv->lpMaxWidth( hFont );
				}
				else
				{
					WARNMSG( 0, ("Call GetObject Error: ptr size error.\r\n") );
					retv = 0;
				}
			}
            break;
        default:
            ASSERT_NOTIFY( 0, "Call GetObject Error:Invalid HGDIOBJ handle\r\n" );
    }
    return retv;
}

// **************************************************
// 声明：int WINAPI WinGdi_ExcludeClipRect( HDC hdc, int left, int top, int right, int bottom )
// 参数：
//	IN hdc-目标DC
//	IN left-矩形左边
//	IN top-矩形顶边
//	IN right-矩形右边
//	IN bottom-矩形底边
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	从设备区域中去掉给定的矩形
// 引用: 
//	系统API
// ************************************************

int WINAPI WinGdi_ExcludeClipRect( HDC hdc, int left, int top, int right, int bottom )
{
    _LPGDCDATA lpdc = _GetHDCPtr( hdc );
    int retv = ERROR;

    if( lpdc )
    {	//创建一个区域对象
        HRGN hrgn = WinRgn_CreateRect( left, top, right, bottom );
        if( hrgn )
        {	//逻辑坐标到设备坐标
            WinRgn_Offset( hrgn, XOFFSET( lpdc ), YOFFSET( lpdc ) );
			//去掉给定的矩形
            retv = WinRgn_Combine( lpdc->hrgn, lpdc->hrgn, hrgn, RGN_DIFF );
            WinGdi_DeleteObject( hrgn );
        }
    }
    return retv;
}

// **************************************************
// 声明：int WINAPI WinGdi_IntersectClipRect( HDC hdc, int left, int top, int right, int bottom )
// 参数：
//	IN hdc-目标DC
//	IN left-矩形左边
//	IN top-矩形顶边
//	IN right-矩形右边
//	IN bottom-矩形底边
// 返回值：
//	NULLREGION-新区域为空
//	SIMPLEREGION-新区域有一个矩形
//	COMPLEXREGION-新区域有多个矩形
//	ERROR-错误
// 功能描述：
//	将设备区域与给定的矩形作内交(与)运算
// 引用: 
//	系统API
// ************************************************

int WINAPI WinGdi_IntersectClipRect( HDC hdc, int left, int top, int right, int bottom )
{
    _LPGDCDATA lpdc = _GetHDCPtr( hdc );
    int retv = ERROR;

    if( lpdc )
    {
        HRGN hrgn = WinRgn_CreateRect( left, top, right, bottom );
        if( hrgn )
        {	//逻辑坐标到设备坐标
            WinRgn_Offset( hrgn, XOFFSET( lpdc ), YOFFSET( lpdc ) );
            retv = WinRgn_Combine( lpdc->hrgn, lpdc->hrgn, hrgn, RGN_AND );
            WinGdi_DeleteObject( hrgn );
        }
    }
    return retv;
}

// **************************************************
// 声明：BOOL WINAPI WinGdi_PtVisible( HDC hdc, int x, int y )
// 参数：
//	IN hdc-目标句柄
//	IN x-点X坐标
//	IN y-点Y坐标
// 返回值：
//	假如点在裁剪区域内,返回TRUE
//	否则,返回FALSE
// 功能描述：
//	判定点是否在裁剪区域内
// 引用: 
//	系统 API
// ************************************************

BOOL WINAPI WinGdi_PtVisible( HDC hdc, int x, int y )
{
    _LPGDCDATA lpdc = _GetHDCPtr( hdc );

    if( lpdc )
    {	//逻辑坐标到设备坐标
        x += XOFFSET( lpdc );
        y += YOFFSET( lpdc );
        return WinRgn_PtInRegion( lpdc->hrgn, x, y );
    }
    return FALSE;
}

// **************************************************
// 声明：BOOL WINAPI WinGdi_RectVisible( HDC hdc, LPCRECT lpcRect )
// 参数：
//	IN hdc-目标句柄
//	IN lpcRect-矩形, RECT指针
// 返回值：
//	假如矩形在裁剪区域内,返回TRUE
//	否则，返回FALSE
// 功能描述：
//	判定矩形是否在裁剪区域内
// 引用: 
//	系统API
// ************************************************

BOOL WINAPI WinGdi_RectVisible( HDC hdc, LPCRECT lpcRect )
{
    _LPGDCDATA lpdc = _GetHDCPtr( hdc );

    if( lpdc )
    {
        RECT rect = *lpcRect;
		//逻辑坐标到设备坐标
        OffsetRect( &rect, XOFFSET( lpdc ), YOFFSET( lpdc ) );
        return WinRgn_RectInRegion( lpdc->hrgn, &rect );
    }
    return FALSE;
}

// **************************************************
// 声明：int WINAPI WinGdi_GetROP2( HDC hdc )
// 参数：
//	IN hdc-目标句柄 
// 返回值：
//	成功：返回操作码值
//	否则：返回0
// 功能描述：
//	得到笔光栅操作码
// 引用: 
//	系统API
// ************************************************

int WINAPI WinGdi_GetROP2( HDC hdc )
{
    _LPGDCDATA lpdc;

    if( (lpdc = _GetHDCPtr( hdc ))!=0 )
    {
        return lpdc->rop;
    }
    return 0;
}

// **************************************************
// 声明：int WINAPI WinGdi_SetROP2( HDC hdc, int rop )
// 参数：
//	IN hdc-目标句柄
//	IN rop-光栅操作码
// 返回值：
//	成功：返回码老码值
//	否则：返回0
// 功能描述：
//	设置笔光栅操作码
// 引用: 
//	系统API
// ************************************************

int WINAPI WinGdi_SetROP2( HDC hdc, int rop )
{
    _LPGDCDATA lpdc;

    if( (lpdc = _GetHDCPtr( hdc ))!=0 )
    {
        int rpSave = lpdc->rop;
        lpdc->rop = rop;
        return rpSave;
    }
    return 0;
}

// **************************************************
// 声明：static void _Offset( LPBYTE lp, int xOffset, int yOffset )
// 参数：
// 	IN lp - 需要进行位移动的数据
//	IN xOffset - 在 x 方向的移动
//	IN yOffset - 在 y 方向的移动
// 返回值：
//	无
// 功能描述：
//	对一个 8 bits * 8 的位矩阵进行位移动
//   ----> x 方向	 
//  0000 0000		^
//  0000 0000		|
//  0000 0000		|	y方向
//  0000 0000		|
//  0000 0000		|
//  0000 0000
//  0000 0000
//  0000 0000
//
// 引用: 
//	
// ************************************************

static void _Offset( LPBYTE lp, int xOffset, int yOffset )
{
    WORD Temp;
    BYTE buf[8];
    LPBYTE p = lp;
    int i;
	//
    if( xOffset > 0 )              //  right move
    {	//右移
		for( i = 0; i < 8; i++ )
		{
			Temp = ( ((WORD)*p)<<8 ) | *p;
			*p = Temp >> xOffset;
			p++;
		}
    }
    else if( xOffset < 0 )   // left move
    {	//左移
		xOffset = -xOffset;
		for( i = 0; i < 8; i++ )
		{
			Temp = ( ((WORD)*p)<<8 ) | *p;
			*lp = Temp << xOffset;
			p++;
		}
    }
    p = lp;
    if( yOffset > 0 )
    {	//下移
		for( i = 0; i < 8; i++ )
			buf[(yOffset+i) & 0x07] = *p++;
		memcpy( lp, buf, 8 );
    }
    else if( yOffset < 0 )
    {	//上移
		for( i = 0; i < 8; i++ )
			*(buf+i) = lp[(i+yOffset) & 0x07];
		memcpy( lp, buf, 8 );
    }
}

// **************************************************
// 声明：BOOL WINAPI WinGdi_SetBrushOrg( HDC hdc, int xOrg, int yOrg, LPPOINT lppt )
// 参数：
//	IN hdc-目标DC
//	IN xOrg-新原点X坐标
//	IN yOrg-新原点Y坐标
//	IN lppt-用于接收老原点坐标,POINT结构针
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	设置刷子原点
// 引用: 
//	系统API
// ************************************************

BOOL WINAPI WinGdi_SetBrushOrg( HDC hdc, int xOrg, int yOrg, LPPOINT lppt )
{
	_LPBRUSHATTRIB lpBrushAttrib;
    _LPGDCDATA lpdc;
	//检查参数是否合法
    //if( xOrg < 0 || yOrg < 0 || xOrg > 7 || yOrg > 7 )
      //  return FALSE;

    if( (lpdc = _GetHDCPtr( hdc )) != 0 )
    {
        lpBrushAttrib = &lpdc->brushAttrib;

        if(lppt)
            *lppt = lpdc->ptBrushOrg;//origin;
		xOrg += lpdc->deviceOrg.x;//( ( 8 - (lpdc->deviceOrg.x % 8) ) + xOrg ) % 8;
		yOrg += lpdc->deviceOrg.y;//( ( 8 - (lpdc->deviceOrg.y % 8) ) + yOrg ) % 8;

		lpdc->ptBrushOrg.x = xOrg;
		lpdc->ptBrushOrg.y = yOrg;

        return TRUE;


		// 2005-09-15, remove by lilin
        //xOrg = (xOrg + XOFFSET(lpdc) ) & 0x07;
        //yOrg = (yOrg + YOFFSET(lpdc) ) & 0x07;
        //xOrg -= lpBrushAttrib->origin.x;
        //yOrg -= lpBrushAttrib->origin.y;
		//

		/*
		if( lpBrushAttrib->style == BS_HATCHED )
        {
            if(lppt)
                *lppt = lpdc->ptBrushOrg;//origin;
			//xOrg &= 0x07;
			//yOrg &= 0x07;
			xOrg += lpdc->deviceOrg.x;//( ( 8 - (lpdc->deviceOrg.x % 8) ) + xOrg ) % 8;
			yOrg += lpdc->deviceOrg.y;//( ( 8 - (lpdc->deviceOrg.y % 8) ) + yOrg ) % 8;


			//变换刷子字模
			//_Offset( lpBrushAttrib->pattern, xOrg, yOrg );
            //lpBrushAttrib->origin.x = xOrg;
            //lpBrushAttrib->origin.y = yOrg;
			lpdc->ptBrushOrg.x = xOrg;
			lpdc->ptBrushOrg.y = yOrg;
            return TRUE;
        }
		else if( lpBrushAttrib->style == BS_PATTERN )
		{
            if(lppt)
                *lppt = lpdc->ptBrushOrg;
			//xOrg %= lpBrushAttrib->lpbmpBrush->bmWidth;
			//yOrg %= lpBrushAttrib->lpbmpBrush->bmHeight;
			//xOrg = ( ( lpBrushAttrib->lpbmpBrush->bmWidth - ( lpdc->deviceOrg.x % lpBrushAttrib->lpbmpBrush->bmWidth ) ) + xOrg ) % lpBrushAttrib->lpbmpBrush->bmWidth;
			//yOrg = ( ( lpBrushAttrib->lpbmpBrush->bmHeight - ( lpdc->deviceOrg.y % lpBrushAttrib->lpbmpBrush->bmHeight ) ) + yOrg ) % lpBrushAttrib->lpbmpBrush->bmHeight;
			xOrg = lpdc->deviceOrg.x + xOrg;
			yOrg = lpdc->deviceOrg.y + yOrg;

			//yOrg = lpBrushAttrib->lpbmpBrush->bmHeight;

			//变换刷子字模
            //lpBrushAttrib->origin.x = xOrg;
            //lpBrushAttrib->origin.y = yOrg;
			lpdc->ptBrushOrg.x = xOrg;
			lpdc->ptBrushOrg.y = yOrg;

            return TRUE;
		}
		*/
    }
    return FALSE;
}

// **************************************************
// 声明：BOOL WINAPI WinGdi_GetTextMetrics( HDC hdc, LPTEXTMETRIC lptm )
// 参数：
//	IN hdc-目标句柄
//	IN lptm-TEXTMETRIC结构指针,用于接受字体参数
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	得到文本度量
// 引用: 
//	系统API
// ************************************************

BOOL WINAPI WinGdi_GetTextMetrics( HDC hdc, LPTEXTMETRIC lptm )
{
    _LPGDCDATA lpdc ;
    _LPFONT lpFont;

    if( (lpdc = _GetHDCPtr( hdc )) != 0 )
    { 
        memset( lptm, 0, sizeof( TEXTMETRIC ) );
        lpFont = _GetHFONTPtr( lpdc->hFont );
        lptm->tmHeight = lpFont->lpDriver->lpMaxHeight( lpFont->handle );
		lptm->tmAscent = lptm->tmHeight;
		lptm->tmMaxCharWidth = lptm->tmAveCharWidth = lpFont->lpDriver->lpWordWidth( lpFont->handle, 'X' );
        return TRUE;
    }
    return FALSE;
}

// **************************************************
// 声明：BOOL WINAPI WinGdi_GetCharWidth( HDC hdc, WORD iFirst, WORD iLast, LPINT lpiBuf )
// 参数：
//	IN hdc-目标句柄
//	IN iFirst-开始字符
//	IN iLast-结束字符
//	IN lpiBuf-保存宽度的缓存
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	得到字符宽度
// 引用: 
//	系统API
// ************************************************

BOOL WINAPI WinGdi_GetCharWidth( HDC hdc, WORD iFirst, WORD iLast, LPINT lpiBuf )
{
    _LPGDCDATA lpdc;
	_LPFONT lpFont;
    _LPFONTDRV lpFontDrv;
	HANDLE hFont;

    if( (lpdc = _GetHDCPtr( hdc )) != 0 )
    {
        lpFont = _GetHFONTPtr( lpdc->hFont );
		lpFontDrv = lpFont->lpDriver;
		hFont = lpFont->handle;
        for( ; iFirst <= iLast; )
            *lpiBuf++ = lpFontDrv->lpWordWidth( hFont, iFirst++ );
        return TRUE;
    }
    return FALSE;
}

// **************************************************
// 声明：BOOL WINAPI WinGdi_LPtoDP( HDC hdc, LPPOINT lpPoints, int nCount )
// 参数：
//	IN hdc-目标DC
//	IN/OUT lpPoints-点数组,POINT结构指针,并接受经转换后的坐标
//	IN nCount-点个数
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	将逻辑坐标转化为设备坐标
// 引用: 
//	系统API
// ************************************************

BOOL WINAPI WinGdi_LPtoDP( HDC hdc, LPPOINT lpPoints, int nCount )
{
	_LPGDCDATA lpdc;

	ASSERT( lpPoints );
	
    if( (lpdc = _GetHDCPtr( hdc )) != 0 )
	{
		int i;
		for( i = 0; i < nCount; i++ )
		{
			lpPoints->x += lpdc->deviceOrg.x;
			lpPoints->y += lpdc->deviceOrg.y;
			lpPoints++;
		}
		return TRUE;
	}
	return FALSE;
}

// **************************************************
// 声明：BOOL WINAPI WinGdi_DPtoLP( HDC hdc, LPPOINT lpPoints, int nCount )
// 参数：
//	IN hdc-目标DC
//	IN/OUT lpPoints-点数组,POINT结构指针,并接受经转换后的坐标
//	IN nCount-点个数
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	将设备坐标转化为逻辑坐标
// 引用: 
//	系统API
// ************************************************

BOOL WINAPI WinGdi_DPtoLP( HDC hdc, LPPOINT lpPoints, int nCount )
{
	_LPGDCDATA lpdc;

	ASSERT( lpPoints );
 
    if( (lpdc = _GetHDCPtr( hdc )) != 0 )
	{
		int i;
		for( i = 0; i < nCount; i++ )
		{
			lpPoints->x -= lpdc->deviceOrg.x;
			lpPoints->y -= lpdc->deviceOrg.y;
			lpPoints++;
		}
		return TRUE;
	}
	return FALSE;
}

// **************************************************
// 声明：BOOL WINAPI WinGdi_GetCurrentPosition( HDC hdc, LPPOINT lpPoint )
// 参数：
//	IN hdc-目标DC
//	IN lpPoint-用于得到点坐标,POINT结构指针
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	得到当前的位置
// 引用: 
//	系统API
// ************************************************

BOOL WINAPI WinGdi_GetCurrentPosition( HDC hdc, LPPOINT lpPoint )
{
	_LPGDCDATA lpdc;

	ASSERT( lpPoint ); 
    if( (lpdc = _GetHDCPtr( hdc )) != 0 )
	{
		*lpPoint = lpdc->position;
		return TRUE;
	}
	return FALSE;
}

// **************************************************
// 声明：BOOL WINAPI WinGdi_GetViewportOrg( HDC hdc, LPPOINT lpPoint )
// 参数：
//	IN hdc-目标句柄
//	IN lpPoint-接收原点,POINT结构指针
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	得到视区原点
// 引用: 
//	系统API
// ************************************************

BOOL WINAPI WinGdi_GetViewportOrg( HDC hdc, LPPOINT lpPoint )
{  
	_LPGDCDATA lpdc;

	ASSERT( lpPoint );
	if( (lpdc = _GetHDCPtr( hdc )) != 0 )
    {
	   *lpPoint = lpdc->viewportOrg;
	   return TRUE;
    }
	return FALSE;
}

// **************************************************
// 声明：BOOL WINAPI WinGdi_SetViewportOrg( HDC hdc, int xOrg, int yOrg, LPPOINT lppt )
// 参数：
//	IN hdc-目标句柄
//	IN xOrg-新原点X坐标
//	IN yOrg-新原点Y坐标
//	OUT lppt-接收原点,POINT结构指针
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	设置视区原点
// 引用: 
//	系统API
// ************************************************

BOOL WINAPI WinGdi_SetViewportOrg( HDC hdc, int xOrg, int yOrg, LPPOINT lppt )
{
	_LPGDCDATA lpdc;

	if( (lpdc = _GetHDCPtr( hdc )) != 0 )
    {		
		if( lppt )
			*lppt = lpdc->viewportOrg;
		lpdc->viewportOrg.x = xOrg;
		lpdc->viewportOrg.y = yOrg;
		return TRUE;
    }
	return FALSE;

}

// **************************************************
// 声明：BOOL WINAPI WinGdi_GetWindowOrg( HDC hdc, LPPOINT lpPoint )
// 参数：
//	IN hdc-目标句柄
//	OUT lpPoint-接收原点,POINT结构指针
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	得到窗口原点
// 引用: 
//	系统API
// ************************************************

BOOL WINAPI WinGdi_GetWindowOrg( HDC hdc, LPPOINT lpPoint )
{  
	_LPGDCDATA lpdc;

	ASSERT( lpPoint );

	if( (lpdc = _GetHDCPtr( hdc )) != 0 )
    {
	   *lpPoint = lpdc->windowOrg;
	   return TRUE;
    }
	return FALSE;
}

// **************************************************
// 声明：BOOL WINAPI WinGdi_SetWindowOrg( HDC hdc, int xOrg, int yOrg, LPPOINT lppt )
// 参数：
//	IN hdc-目标句柄
//	IN xOrg - 新的原点x
//	IN yOrg - 新的原点y
//	OUT lpPoint-接收之前的原点,POINT结构指针
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	设置窗口原点
// 引用: 
//	
// ************************************************

BOOL WINAPI WinGdi_SetWindowOrg( HDC hdc, int xOrg, int yOrg, LPPOINT lppt )
{
	_LPGDCDATA lpdc;
	
	if( (lpdc = _GetHDCPtr( hdc )) != 0 )
    {
		if( lppt )
			*lppt = lpdc->windowOrg; 
		lpdc->windowOrg.x = xOrg;
		lpdc->windowOrg.y = yOrg;
		return TRUE;
    }
	return FALSE;
}

// **************************************************
// 声明：int WINAPI WinGdi_OffsetClipRgn( HDC hdc, int xOff, int yOff )
// 参数：
//	IN hdc – DC句柄
//	IN xOff – x偏移量
//	IN yOff – y偏移量
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//		NULLREGION - 裁剪区域是空的
//		SIMPLEREGION - 裁剪区域只有一个矩形
//		COMPLEXREGION - 裁剪区域含多个矩形
//		ERROR – 错误
// 引用: 
//	系统API
// ************************************************

int WINAPI WinGdi_OffsetClipRgn( HDC hdc, int xOff, int yOff )
{
	_LPGDCDATA lpdc = _GetHDCPtr( hdc );

	if( lpdc )
	{
		return WinRgn_Offset( lpdc->hrgn, xOff, yOff );
	} 
	return ERROR;
}

// **************************************************
// 声明：BOOL WINAPI WinGdi_OffsetViewportOrg( HDC hdc, int xOff, int yOff, LPPOINT lppt )
// 参数：
//	IN hdc-目标句柄
//	IN xOff-X偏移
//	IN yOff-Y偏移
//	IN lppt-接收原点,POINT结构指针
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	偏移视区原点
// 引用: 
//	系统API
// ************************************************

BOOL WINAPI WinGdi_OffsetViewportOrg( HDC hdc, int xOff, int yOff, LPPOINT lppt )
{
	_LPGDCDATA lpdc;

	if( (lpdc = _GetHDCPtr( hdc )) != NULL )
	{
		if( lppt )
			*lppt = lpdc->viewportOrg;
		lpdc->viewportOrg.x += xOff;
		lpdc->viewportOrg.y += yOff;
		return TRUE;
	} 
	return FALSE;
}

// **************************************************
// 声明：BOOL WINAPI WinGdi_OffsetWindowOrg( HDC hdc, int xOff, int yOff, LPPOINT lppt )
// 参数：
//	IN hdc-目标句柄
//	IN xOff-X偏移
//	IN yOff-Y偏移
//	IN lppt-接收原点,POINT 结构指针
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	偏移窗口原点
// 引用: 
//	系统API
// ************************************************

BOOL WINAPI WinGdi_OffsetWindowOrg( HDC hdc, int xOff, int yOff, LPPOINT lppt )
{
	_LPGDCDATA lpdc;

	if( (lpdc = _GetHDCPtr( hdc )) != NULL )
	{
		if( lppt )
			*lppt = lpdc->windowOrg;
		lpdc->windowOrg.x += xOff;
		lpdc->windowOrg.y += yOff;
		return TRUE;
	} 
	return FALSE;
}

// **************************************************
// 声明：HGDIOBJ WINAPI WinGdi_GetStockObject( int fObject )
// 参数：
//	IN fObject-对象名,包含：
//			WHITE_BRUSH-白刷子
//			LTGRAY_BRUSH-轻灰刷子
//			GRAY_BRUSH-灰刷子
//			DKGRAY_BRUSH-重灰刷子
//			BLACK_BRUSH-黑刷子
//			NULL_BRUSH-空刷子
//			WHITE_PEN-白笔
//			BLACK_PEN-黑笔
//			NULL_PEN-空笔
//			ANSI_FIXED_FONT-固定大小字体(仅限English)
//			ANSI_VAR_FONT-可变大小字体(仅限English)
//			SYSTEM_FONT-系统字体(仅限English)
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	得到系统预定义对象
// 引用: 
//	系统API
// ************************************************

HGDIOBJ WINAPI WinGdi_GetStockObject( int fObject )
{
	//if( fObject < STOCK_GDIOBJ_NUM )	//由 dll 判断该部分
	if( fObject < SYS_STOCK_GDIOBJ_NUM )
	    return OEM_GetStockObject( fObject );
	return NULL;
}

// **************************************************
// 声明：int WINAPI WinGdi_GetMapMode( HDC hdc )
// 参数：
// 	IN hdc - DC句柄
// 返回值：
//	假入成功，返回当前模式；否则，返回0
// 功能描述：
//	得到当前模式
// 引用: 
//	系统API
// ************************************************

int WINAPI WinGdi_GetMapMode( HDC hdc )
{
	_LPGDCDATA lpdc;

	if( (lpdc = _GetHDCPtr( hdc )) != NULL )
	{
		return lpdc->mapMode;
	} 
	return 0;

}

// **************************************************
// 声明：int WINAPI WinGdi_SetMapMode( HDC hdc, int iNewMapMode )
// 参数：
// 	IN hdc - DC句柄
//	IN iNewMapMode - 新的映射模式
// 返回值：
//	假入成功，返回当前模式；否则，返回0
// 功能描述：
//	设置新的映射模式
// 引用: 
//	系统API
// ************************************************

int WINAPI WinGdi_SetMapMode( HDC hdc, int iNewMapMode )
{
	_LPGDCDATA lpdc;
	int oldMode = 0;

	if( (lpdc = _GetHDCPtr( hdc )) != NULL )
	{
		oldMode = lpdc->mapMode;
		lpdc->mapMode = iNewMapMode;
	} 
	return oldMode;
}

// **************************************************
// 声明：BOOL DoStretchBlt(
//				HDC hdcDest, 
//				int xDest, 
//				int yDest, 
//				int nWidthDest, 
//				int nHeightDest, 
//				HDC hdcSrc, 
//				int xSrc, 
//				int ySrc, 
//				int nWidthSrc, 
//				int nHeightSrc, 
//				DWORD dwRop )

// 参数：
//	IN hdcDest - 目标DC
//	IN xDest - 目标矩形左上角X坐标
//	IN yDest - 目标矩形左上角Y坐标
//	IN nWidthDest - 目标矩形宽度
//	IN nHeightDest - 目标矩形高度
//	IN hdcSrc - 源DC
//	IN xSrc - 源矩形左上角X坐标
//	IN ySrc - 源矩形左上角Y坐标
//	IN nWidthSrc - 源矩形矩形宽度
//	IN nHeightSrc - 源矩形矩形高度
//	IN dwRop - 光栅操作码,包括：
//			SRCCOPY-拷贝源矩形到目标矩形
//			SRCAND--源数据与目标数据做与运算(AND)
//			SRCINVERT-源数据与目标数据做异或运算(XOR)
//			SRCPAINT-源数据与目标数据做或运算(OR)
//			PATCOPY-拷贝模板到目标矩形
//			BLACKNESS-用黑色填充目标矩形
//			WHITENESS-用白色填充目标矩形
//			PATINVERT-模板颜色与目标颜色做异或运算(XOR)
//			DSTINVERT-反转目标颜色
//			NOTSRCCOPY-拷贝反转的源颜色到目标矩形 
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	从源DC向目标DC进行位图传送（伸缩功能）
//	这是一个慢功能，我在将来将会改善它
// 引用: 
//	系统API
// ************************************************

#define MAX_CACHE_INDEXS   16   //必须是2的次方 2 ^ n

static BOOL DoStretchBlt(
				HDC hdcDest, 
				int xDest, 
				int yDest, 
				int nWidthDest, 
				int nHeightDest, 
				HDC hdcSrc, 
				int xSrc, 
				int ySrc, 
				int nWidthSrc, 
				int nHeightSrc, 
				DWORD dwRop )
{
	int xMultiple, yMultiple;
	int xMode, yMode;
	COLORREF color, clrPrev = -1;
	COLORREF colorSrc, clrPrevSrc = -1;
	int xm, ym;
	int ydPrev, ysPrev;
	int xsPrev;
	_LPGDCDATA lpdcDest = NULL;
	_LPGDCDATA lpdcSrc = NULL;
    _PIXELDATA pixelDataSrc, pixelDataDest;
	int yDestEnd;
	int xDestStart, xDestEnd;
	int xSrcStart;	
    UINT bmType = 0;
	int yDir = 1;
	int cySrcBitmapHeight;
	PIXEL_CACHE srcCache[MAX_CACHE_INDEXS];
	PIXEL_CACHE destCache[MAX_CACHE_INDEXS];
	BOOL retv;


	if( nWidthDest <= 0 ||
		nHeightDest <= 0 ||
		nWidthSrc <= 0 ||
		nHeightSrc <= 0 ||
		dwRop != SRCCOPY )
	{
		//SetLastError( ERROR_INVALID_PARAMETER );
		//goto _ERROR;//return FALSE;
		SetLastError( ERROR_INVALID_PARAMETER );
		goto _return;
	}

    lpdcDest = _GetSafeDrawPtr( hdcDest );
    if( lpdcDest && lpdcDest->lpDispDrv )
    {
        pixelDataDest.lpDestImage = _GetHBITMAPPtr( lpdcDest->hBitmap );

        pixelDataDest.pattern = 0;
        pixelDataDest.rop = R2_COPYPEN;//lpdc->rop;
	}
	else
	{
		SetLastError( ERROR_INVALID_PARAMETER );
		goto _return;//_ERROR;
	}

    lpdcSrc = _GetSafeDrawPtr( hdcSrc );
    if( lpdcSrc && lpdcSrc->lpDispDrv )
    {
        pixelDataSrc.lpDestImage = _GetHBITMAPPtr( lpdcSrc->hBitmap );
		bmType = pixelDataSrc.lpDestImage->bmFlags & BF_DIB;
		if( bmType )
			yDir = ( (_LPBITMAP_DIB)pixelDataSrc.lpDestImage )->biDir;
		cySrcBitmapHeight = pixelDataSrc.lpDestImage->bmHeight;
	}
	else
	{
		SetLastError( ERROR_INVALID_PARAMETER );
		goto _return;//_ERROR;
	}

	xDest += XOFFSET( lpdcDest );
	xDestEnd = xDest + nWidthDest;
	
	yDest += YOFFSET( lpdcDest );
	yDestEnd = yDest + nHeightDest;

	xSrc += XOFFSET( lpdcSrc );
	ySrc += YOFFSET( lpdcSrc );

	xMultiple = nWidthSrc / nWidthDest;
	xMode = nWidthSrc % nWidthDest;
	yMultiple = nHeightSrc / nHeightDest;
	yMode = nHeightSrc % nHeightDest;

	INIT_PIXEL_CAHE( srcCache, -1 );
	INIT_PIXEL_CAHE( destCache, -1 );

	ydPrev = ysPrev = -1;

	for( ym = 0; yDest < yDestEnd; yDest++, ySrc += yMultiple )
	{
		LPBYTE lpIndex;
		UINT index;

	//	if( ydPrev != yd || ysPrev != ys )
		{
			xsPrev = -1;
			xDestStart = xDest;
			xSrcStart = xSrc;
			if( yDir < 0 )
				pixelDataSrc.y = cySrcBitmapHeight - ySrc - 1;
			else
				pixelDataSrc.y = ySrc;
			pixelDataDest.y = yDest;
			for( xm = 0; xDestStart < xDestEnd; xDestStart++, xSrcStart += xMultiple )
			{
                if( WinRgn_PtInRegion( lpdcDest->hrgn, xDestStart, yDest ) )
				{
					if( xsPrev !=  xSrcStart )
					{
						pixelDataSrc.x = xSrcStart;
						if( WinRgn_PtInRegion( lpdcSrc->hrgn, pixelDataSrc.x, pixelDataSrc.y ) )
						{
							colorSrc = lpdcSrc->lpDispDrv->lpGetPixel( &pixelDataSrc );
							
							if( colorSrc != clrPrevSrc )
							{   
								lpIndex = (LPBYTE)&colorSrc;
								index = *lpIndex + *(lpIndex+1) + *(lpIndex+2) + *(lpIndex+3);
								index = index & (MAX_CACHE_INDEXS-1);
								// at cache ?
								if( !AT_PIXEL_CACHE( srcCache, index, colorSrc ) )
								{  // 不在
									color = lpdcSrc->lpDispDrv->lpUnrealizeColor( colorSrc, lpdcSrc->lpdwPal, lpdcSrc->wPalNumber, lpdcSrc->wPalFormat );
									SET_PIXEL_CACHE_DATA( srcCache, index, colorSrc, color );
								}
								else
								{
									color = GET_PIXEL_CACHE_DATA( srcCache, index );
								}
								clrPrevSrc = colorSrc;
							}
						}
						else
						{
							color = 0;
							clrPrevSrc = -1;
						}
						//color = GetPixel( hdcSrc, xSrc, ySrc );
						xsPrev = xSrcStart;
					}

					pixelDataDest.x = xDestStart;

					
					if( clrPrev != color )
					{
						lpIndex = (LPBYTE)&color;
						index = *lpIndex + *(lpIndex+1) + *(lpIndex+2) + *(lpIndex+3);
						index = index & (MAX_CACHE_INDEXS-1);
						// at cache ?
						if( !AT_PIXEL_CACHE( destCache, index, color ) )
						{  // 不在
							pixelDataDest.color = lpdcDest->lpDispDrv->lpRealizeColor( color, lpdcDest->lpdwPal, lpdcDest->wPalNumber, lpdcDest->wPalFormat );
							SET_PIXEL_CACHE_DATA( destCache, index, color, pixelDataDest.color  );
						}
						else
						{
							pixelDataDest.color = GET_PIXEL_CACHE_DATA( destCache, index );
						}
						clrPrev = color;
					}

					lpdcDest->lpDispDrv->lpPutPixel( &pixelDataDest );
				}

				//SetPixel( hdcDest, xDest, yDest, color );

				xm += xMode;
				if( xm >= nWidthDest )
				{
					xSrcStart++;
					xm -= nWidthDest;
				}
			}
	//		ydPrev = yd;
			//ysPrev = ys;
		}

		ym += yMode;
		if( ym >= nHeightDest )
		{
			ySrc++;
			ym -= nHeightDest;
		}
	}

	retv = TRUE;//return TRUE;

_return:
//_ERROR:
//	SetLastError( ERROR_INVALID_PARAMETER );
	//return FALSE;
	_LeaveDCPtr( lpdcDest, DCF_WRITE );
	_LeaveDCPtr( lpdcSrc, DCF_READ );
	return retv;
}

// **************************************************
// 声明：BOOL WINAPI WinGdi_StretchBlt(
//				HDC hdcDest, 
//				int xDest, 
//				int yDest, 
//				int nWidthDest, 
//				int nHeightDest, 
//				HDC hdcSrc, 
//				int xSrc, 
//				int ySrc, 
//				int nWidthSrc, 
//				int nHeightSrc, 
//				DWORD dwRop )

// 参数：
//	IN hdcDest - 目标DC
//	IN xDest - 目标矩形左上角X坐标
//	IN yDest - 目标矩形左上角Y坐标
//	IN nWidthDest - 目标矩形宽度
//	IN nHeightDest - 目标矩形高度
//	IN hdcSrc - 源DC
//	IN xSrc - 源矩形左上角X坐标
//	IN ySrc - 源矩形左上角Y坐标
//	IN nWidthSrc - 源矩形矩形宽度
//	IN nHeightSrc - 源矩形矩形高度
//	IN dwRop - 光栅操作码,包括：
//			SRCCOPY-拷贝源矩形到目标矩形
//			SRCAND--源数据与目标数据做与运算(AND)
//			SRCINVERT-源数据与目标数据做异或运算(XOR)
//			SRCPAINT-源数据与目标数据做或运算(OR)
//			PATCOPY-拷贝模板到目标矩形
//			BLACKNESS-用黑色填充目标矩形
//			WHITENESS-用白色填充目标矩形
//			PATINVERT-模板颜色与目标颜色做异或运算(XOR)
//			DSTINVERT-反转目标颜色
//			NOTSRCCOPY-拷贝反转的源颜色到目标矩形 
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	从源DC向目标DC进行位图传送（伸缩功能）
//	这是一个慢功能，我在将来将会改善它
// 引用: 
//	系统API
// ************************************************

BOOL WINAPI WinGdi_StretchBlt(
				HDC hdcDest, 
				int xDest, 
				int yDest, 
				int nWidthDest, 
				int nHeightDest, 
				HDC hdcSrc, 
				int xSrc, 
				int ySrc, 
				int nWidthSrc, 
				int nHeightSrc, 
				DWORD dwRop )
{
	if( nWidthDest == nWidthSrc &&
		nHeightDest == nHeightSrc )
	{
		return WinGdi_BitBlt( hdcDest, xDest, yDest, nWidthDest, nHeightDest, hdcSrc, xSrc, ySrc, dwRop );
	}
	else
	{
		return DoStretchBlt( hdcDest, xDest, yDest, nWidthDest, nHeightDest, hdcSrc, xSrc, ySrc, nWidthSrc, nHeightSrc, dwRop );
	}
}


// **************************************************
// 声明：HWND WINAPI WinGdi_WindowFromDC( HDC hdc )
// 参数：
// 	IN hdc - DC句柄
// 返回值：
//	假入成功，返回 hdc 对应的窗口句柄；否则，返回 NULL
// 功能描述：
//	得到DC对应的窗口句柄
// 引用: 
//	系统API
// ************************************************

HWND WINAPI WinGdi_WindowFromDC( HDC hdc )
{
	_LPGDCDATA lpdc;

	if( (lpdc = _GetHDCPtr( hdc )) != NULL )
	{
		return lpdc->hwnd;
	} 
	return NULL;
}

// **************************************************
// 声明：UINT WINAPI WinGdi_SetDIBColorTable( HDC hdc,
//											  UINT uStartIndex,
//											  UINT cEntries,
//											  CONST RGBQUAD *pColors )
// 参数：
// 	IN hdc - DC句柄
//  IN uStartIndex - 需要改变的调色板开始索引
//  IN cEntries - 需要改变的调色板项数
//  IN pColors - 提供的调色板项
// 返回值：
//	假入成功，返回 已经设置的表项数；否则，返回 0
// 功能描述：
//	设置 当前新的调色板，针对 1，4，8 BPP
// 引用: 
//	系统API
// ************************************************

UINT WINAPI WinGdi_SetDIBColorTable( HDC hdc, UINT uStartIndex, UINT cEntries, CONST RGBQUAD *pColors )
{
	_LPGDCDATA lpdc;
	UINT retv  = 0;

	if( (lpdc = _GetHDCPtr( hdc )) != NULL )
	{
		_LPBITMAP_DIB lpImage = (_LPBITMAP_DIB)_GetHBITMAPPtr( lpdc->hBitmap );
		if( lpImage && 
			(lpImage->bitmap.bmFlags & BF_DIB) )
		{
			PALETTEENTRY * lpPal = &lpImage->palEntry[uStartIndex];
			UINT n = cEntries;
			while( (uStartIndex < lpImage->biClrUsed) && n )
			{
				lpPal->peRed = pColors->rgbRed;
				lpPal->peRed = pColors->rgbRed;
				lpPal->peRed = pColors->rgbRed;
				lpPal++; pColors++;
				uStartIndex++;
				n--;
			}
			retv = cEntries - n; 
		}
	} 
	return retv;
}

// **************************************************
// 声明： int WINAPI WinGdi_GetDeviceCaps( HDC hdc, int nIndex )
// 参数：
// 	IN hdc - DC句柄
//  IN nIndex - 索引
// 返回值：
//	假入成功，返回 对应的值；否则，返回 0
// 功能描述：
//	得到设备信息
// 引用: 
//	系统API
// ************************************************


int WINAPI WinGdi_GetDeviceCaps( HDC hdc, int nIndex )
{
	_LPGDCDATA lpdc;
	UINT retv  = 0;

	if( (lpdc = _GetHDCPtr( hdc )) != NULL )
	{
		_LPBITMAPDATA lpImage = (_LPBITMAPDATA)_GetHBITMAPPtr( lpdc->hBitmap );
		switch( nIndex )
		{
		case DRIVERVERSION:
			return 1;
		case TECHNOLOGY:
			return DT_RASDISPLAY;
		case HORZSIZE:
			return 0;
		case VERTSIZE:
			return 0;
		case HORZRES:
			return lpImage->bmWidth;
		case VERTRES:
			return lpImage->bmHeight;
		case LOGPIXELSX:
			return 0;
		case LOGPIXELSY:
			return 0;
		case BITSPIXEL:
			return lpImage->bmBitsPixel;
		case PLANES:
			return lpImage->bmPlanes;
		case NUMBRUSHES:
			return 1;
		case NUMPENS:
			return 1;
		case NUMFONTS:
			return 1;
		case NUMCOLORS:
			if( lpImage->bmBitsPixel == 1 ) 
				return 1;
			else if( lpImage->bmBitsPixel == 2 ) 
				return 4;
			else if( lpImage->bmBitsPixel == 4 ) 
				return 16;
			else if( lpImage->bmBitsPixel == 8 ) 
				return 256;
			else
				return -1;
		case ASPECTX:
			return 1;
		case ASPECTY:
			return 1;
		case ASPECTXY:
			return 1;
		case PDEVICESIZE:
			return 0;
		case CLIPCAPS:
			return 1;
		case SIZEPALETTE:
			return 0;
		case NUMRESERVED:
			return 0;
		case COLORRES:
			return 0;
		case PHYSICALWIDTH:
			return 0;
		case PHYSICALHEIGHT:
			return 0;
		case PHYSICALOFFSETX:
			return 0;
		case PHYSICALOFFSETY:
			return 0;
		case RASTERCAPS:
			return RC_BANDING | RC_BITBLT | RC_BITMAP64 | RC_DI_BITMAP | RC_DIBTODEV | RC_SCALING | RC_STRETCHBLT;
		case CURVECAPS:
			return CC_ELLIPSES | CC_CIRCLES | CC_ROUNDRECT | CC_STYLED | CC_WIDE | CC_WIDESTYLED;
		case LINECAPS:
			return LC_INTERIORS | LC_POLYLINE | LC_STYLED | LC_WIDE | LC_WIDESTYLED;
		case POLYGONALCAPS:
			return PC_NONE;
		case TEXTCAPS:
			return TC_RA_ABLE | TC_OP_CHARACTER;
		}
	}
	return 0;
}



/*
static InitAlphaCache( BYTE bAlphaSrc[256], BYTE bAlphaDest[256], BYTE alpha )
{
	LPBYTE lpbAlphaSrc = bAlphaSrc;
	LPBYTE lpbAlphaDest = bAlphaDest;
	BYTE invAlpha = 255 - alpha;
	int i;

	for( i = 0; i < 256; i++, lpbAlphaSrc++, lpbAlphaDest++ )
	{
		*lpbAlphaSrc = i * alpha / 255;
		*lpbAlphaDest = i * invAlpha / 255;
		//*lpbAlphaDest = i * (255 - alpha) / 255;
		//*lpbAlphaDest = (i * 255 - i * alpha) / 255;
		//*lpbAlphaDest = i * 255 / 255 - i * alpha / 255;
		*lpbAlphaDest = i - i * alpha / 255;
	}
}
*/
static InitAlphaCache( BYTE bAlphaCache[256], BYTE alpha )
{
	LPBYTE lpbAlphaCache = bAlphaCache;
	int i;

	for( i = 0; i < 256; i++, lpbAlphaCache++ )
	{
		*lpbAlphaCache = i * alpha / 255;
	}
}

// **************************************************
// 声明：BOOL PixelAlphaTransfer( 
//							_LPGDCDATA lpdcDest,//_LPCDISPLAYDRV lpDestDispDrv,
//							_LPPIXELDATA lpPixDest,
//							LPRECT lprcDest,
//							_LPGDCDATA lpdcSrc,//_LPCDISPLAYDRV lpSrcDispDrv,
//							_LPPIXELDATA lpPixSrc,
//							LPRECT lprcSrc,
//							LPBYTE lpAlphaCache    //已经算好的Alpha值
//						 )

// 参数：
//	IN lpdcDest - 目标DC对象  //lpDestDispDrv - 目标驱动程序接口
//	IN lpPixDest - 目标点结构指针
//	IN lprcDest - 目标矩形
//	IN lpdcSrc - 源DC对象  //lpSrcDispDrv - 源驱动程序接口
//	IN lpPixSrc - 源点结构指针
//	IN lprcSrc - 源矩形
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	在两个的位图之间进行Alpha传送位图
// 引用: 
//	
// ************************************************

#define DEBUG_PIXEL_ALPHA_TRANSFER 0
static BOOL PixelAlphaTransfer( _LPGDCDATA lpdcDest,//_LPCDISPLAYDRV lpDestDispDrv,
								_LPPIXELDATA lpPixDest,
								LPRECT lprcDest,

								_LPGDCDATA lpdcSrc,//_LPCDISPLAYDRV lpSrcDispDrv,
								_LPPIXELDATA lpPixSrc,
								LPRECT lprcSrc,

								LPBYTE lpAlphaCache,
								//LPBYTE lpAlphaSrcCache,
								//LPBYTE lpAlphaDestCache,
								DWORD rgbTransparent
							)
{
	_LPCDISPLAYDRV lpDestDispDrv = lpdcDest->lpDispDrv;
	_LPCDISPLAYDRV lpSrcDispDrv = lpdcSrc->lpDispDrv;
	int x, y;
	int width, height;
	int xSrcOffset, ySrcOffset, xDestOffset, yDestOffset;
	COLORREF clrPrev = -1;
	COLORREF clrCur, clrRgb = 0;

	COLORREF clrDestPrev = -1;
	COLORREF clrDestCur, clrDestRgb = 0;

	COLORREF clrTempPrev = -1;
	COLORREF clrTempCur, clrTempRgb = 0;

	//源
	_LPBITMAP_DIB lpSrcDIB = (_LPBITMAP_DIB)lpPixSrc->lpDestImage;
	WORD bmType = lpSrcDIB->bitmap.bmFlags & BF_DIB;
	DWORD * lpPal = NULL;
	UINT uiPalNum = 0;

	//目标
	_LPBITMAP_DIB lpDestDIB = (_LPBITMAP_DIB)lpPixDest->lpDestImage;
	WORD bmDestType = lpDestDIB->bitmap.bmFlags & BF_DIB;
	DWORD * lpDestPal = NULL;
	UINT uiDestPalNum = 0;
	int yDestDir = 1;

	BYTE srcRed, srcGreen, srcBlue;
	BYTE destRed, destGreen, destBlue;
	BYTE red, green, blue;

	int yDir = 1;
	UINT clrRealTransparent = -1;	//设备相关的颜色值
	
	if( bmType )
	{	//源位图为DIB格式
		lpPal = (DWORD*)&lpSrcDIB->palEntry[0];
		uiPalNum = lpSrcDIB->biClrUsed;
		yDir = lpSrcDIB->biDir;
		//颜色值为指向调色板的索引值
		if( lpSrcDIB->biCompression == BI_RGB )
		    bmType = PAL_INDEX;
		else if( lpSrcDIB->biCompression == BI_BITFIELDS )
			bmType = PAL_BITFIELD;  //颜色值为位图域
		else
			return FALSE;
	}

	if( bmDestType )
	{	//源位图为DIB格式
		lpDestPal = (DWORD*)&lpDestDIB->palEntry[0];
		uiDestPalNum = lpDestDIB->biClrUsed;
		yDestDir = lpDestDIB->biDir;
		//颜色值为指向调色板的索引值
		if( lpDestDIB->biCompression == BI_RGB )
		    bmDestType = PAL_INDEX;
		else if( lpDestDIB->biCompression == BI_BITFIELDS )
			bmDestType = PAL_BITFIELD;  //颜色值为位图域
		else
			return FALSE;
	}

	//目标宽度和高度
	width = lprcDest->right - lprcDest->left;
	height = lprcDest->bottom - lprcDest->top;

	//RETAILMSG( 1, ( "PixelTransfer.\r\n" ) );
	//源起始位置在位图中的偏移
	xSrcOffset = lprcSrc->left;
	ySrcOffset = lprcSrc->top;
	//目标起始位置在位图中的偏移
	xDestOffset = lprcDest->left;
	yDestOffset = lprcDest->top;

	if( rgbTransparent != -1 )
	{
		rgbTransparent = lpSrcDispDrv->lpRealizeColor( rgbTransparent, lpPal, uiPalNum, bmType );
	}
		
	for( y = 0; y < height; y++ )
	{	// 每一行
		for( x = 0; x < width; x++ )
		{	// 每一列
			if( yDir < 0 )	//检查位图的方向
			{
			    lpPixSrc->y = lpSrcDIB->bitmap.bmHeight - (y + ySrcOffset) - 1;
			}
			else
			{
			    lpPixSrc->y = y + ySrcOffset;
			}
		    lpPixSrc->x = x + xSrcOffset;
			//得到源点设备相关颜色值
			clrCur = lpSrcDispDrv->lpGetPixel( lpPixSrc );

			if( clrCur != rgbTransparent )
			{	//如果非透明色，则画之
				if( clrCur != clrPrev )
				{	//与之前的颜色值相同吗，如果不相同，则将其转化为
					//设备无关的值
					//					if( bmType && uiPalNum )
					//					{   // _Gdi_UnrealizeColor 会自动 call 
					//						clrRgb = _Gdi_UnrealizeColor( clrCur, lpPal, uiPalNum, bmType );  
					//					}
					//					else
					clrRgb = lpSrcDispDrv->lpUnrealizeColor( clrCur, lpPal, uiPalNum, bmType ); //  会自动 call _Gdi_UnrealizeColor
					
					
					srcRed = GetRValue( clrRgb );
					srcGreen = GetGValue( clrRgb );
					srcBlue = GetBValue( clrRgb );
					
					clrPrev = clrCur;
				}
				
				// 得到目标RGB
				
				lpPixDest->x = x + xDestOffset;
				lpPixDest->y = y + yDestOffset;
				clrDestCur = lpDestDispDrv->lpGetPixel( lpPixDest );
				if( clrDestCur != clrDestPrev )
				{	//与之前的颜色值相同吗，如果不相同，则将其转化为
					//设备无关的值
					//if( bmDestType && uiDestPalNum )
					//{
					//	clrDestRgb = _Gdi_UnrealizeColor( clrDestCur, lpDestPal, uiDestPalNum, bmDestType );  
					//}
					//else
					clrDestRgb = lpDestDispDrv->lpUnrealizeColor( clrDestCur, lpDestPal, uiDestPalNum, bmDestType ); //会自动 call _Gdi_UnrealizeColor
					destRed = GetRValue( clrDestRgb );
					destGreen = GetGValue( clrDestRgb );
					destBlue = GetBValue( clrDestRgb );
					
					clrDestPrev = clrDestCur;					
				}
				
				
				//red = lpAlphaSrcCache[srcRed] + lpAlphaDestCache[destRed];
				//green = lpAlphaSrcCache[srcGreen] + lpAlphaDestCache[destGreen];
				//blue = lpAlphaSrcCache[srcBlue] + lpAlphaDestCache[destBlue];

				red = lpAlphaCache[srcRed] + destRed - lpAlphaCache[destRed];
				green = lpAlphaCache[srcGreen] + destGreen - lpAlphaCache[destGreen];
				blue = lpAlphaCache[srcBlue] + destBlue - lpAlphaCache[destBlue];

				clrTempCur = RGB(red,green,blue);
				if( clrTempCur != clrTempPrev )
				{
					lpPixDest->color = lpDestDispDrv->lpRealizeColor( RGB(red,green,blue), lpdcDest->lpdwPal, lpdcDest->wPalNumber, lpdcDest->wPalFormat );
					clrTempPrev = clrTempCur;
				}
				
				//if( clrCur != clrPrev )
				//{	//得到目标相关的值
				
				//}
				//写点到目标位图
				lpDestDispDrv->lpPutPixel( lpPixDest );
			}
		}
	}
	return TRUE;
}


// alphe 覆盖，当前支持　nWidthDest = nWidthSrc && hHeightDest == nHeightSrc
BOOL WINAPI WinGdi_AlphaBlendEx(
  HDC hdcDest,
  int nXOriginDest,
  int nYOriginDest,
  int nWidthDest,
  int nHeightDest,
  HDC hdcSrc,
  int nXOriginSrc,
  int nYOriginSrc,
  int nWidthSrc,
  int nHeightSrc,
  LPBLENDFUNCTION_EX lpBlendFunction
)
{
	_LPGDCDATA lpdcDest = NULL;
	_LPGDCDATA lpdcSrc = NULL;
    _LPRGNDATA lprgn;
    _LPRECTNODE lprnNode;
    //_BLKBITBLT blt;
    RECT rcSrc, rcTemp, rcClip, rcDest;
    int xoff, yoff, xSrcOff, ySrcOff;
	//BYTE alphaSrcCache[256];
	//BYTE alphaDestCache[256];
	BYTE alphaCache[256];

	BOOL bRetv = FALSE;

	DWORD rgbTransparent = lpBlendFunction->rgbTransparent;
	//检测参数
	if( (nWidthDest != nWidthSrc) || (nHeightDest != nHeightSrc) )
	{
		goto _return;
	}
	if( !lpBlendFunction )
		goto _return;
	if( lpBlendFunction->bf.BlendOp != AC_SRC_OVER )
		goto _return;

    lpdcDest = _GetSafeDrawPtr( hdcDest );//得到目标DC对象指针
    lpdcSrc = _GetSafeDrawPtr( hdcSrc );//得到源DC对象指针

    if( lpdcDest && lpdcSrc && lpdcDest->lpDispDrv )
    {	
        _LPCDISPLAYDRV lpDispDrv = lpdcDest->lpDispDrv;  //目标DC驱动程序
		_LPBITMAPDATA lpDestImage = _GetHBITMAPPtr( lpdcDest->hBitmap );
		_LPBITMAPDATA lpSrcImage = _GetHBITMAPPtr( lpdcSrc->hBitmap );

		lprgn = _GetHRGNPtr( lpdcDest->hrgn );
		//源/目标位图数据结构
        //blt.lpDestImage = _GetHBITMAPPtr( lpdcDest->hBitmap );
        //blt.lpSrcImage = _GetHBITMAPPtr( lpdcSrc->hBitmap );
		//当前刷子
        //blt.lpBrush = &lpdcSrc->brushAttrib;

		//if( !(lprgn && blt.lpDestImage && blt.lpSrcImage && blt.lpBrush) )
		//	goto _ERROR;
		if( !(lprgn &&  lpDispDrv && lpDestImage && lpSrcImage) )
			goto _return;

        //blt.lprcSrc = &rcSrc;
        //blt.lprcDest = &rcDest;
        //blt.dwRop = dwRop;
        // in bitblt, mono bitmap , 1 mean backcolor, 0 mean textcolor
        //blt.solidColor = lpdcDest->backColor;
        //blt.solidBkColor = lpdcDest->textColor;
        //blt.backMode = lpdcDest->backMode;
		//当前源DC在显示设备上的偏移
        xSrcOff = XOFFSET( lpdcSrc );
        ySrcOff = YOFFSET( lpdcSrc );
		//转换源坐标到设备坐标
        rcTemp.left = nXOriginSrc;
        rcTemp.top = nYOriginSrc;
        rcTemp.right = rcTemp.left + nWidthDest;
        rcTemp.bottom = rcTemp.top + nHeightDest;
        OffsetRect( &rcTemp, xSrcOff, ySrcOff );
		//当前目标DC在显示设备上的偏移
        xoff = XOFFSET( lpdcDest );
        yoff = YOFFSET( lpdcDest );
		//转换目标坐标到设备坐标
        rcClip.left = nXOriginDest;
        rcClip.top = nYOriginDest;
        rcClip.right = rcClip.left + nWidthDest;
        rcClip.bottom = rcClip.top + nHeightDest;
        OffsetRect( &rcClip, xoff, yoff );
		//得到源和目标的共同区域
        if( rcTemp.left < 0 )
        {
            rcClip.left -= rcTemp.left;
        }
        if( rcTemp.top < 0 )
        {
            rcClip.top -= rcTemp.top;
        }
        if( rcTemp.right > lpSrcImage->bmWidth )
        {
            rcClip.right -= rcTemp.right - lpSrcImage->bmWidth;
        }
        if( rcTemp.bottom > lpSrcImage->bmHeight )
        {
            rcClip.bottom -= rcTemp.bottom - lpSrcImage->bmHeight;
        }
		//源坐标在目标上的投射点相对偏移
        xoff = nXOriginSrc + XOFFSET( lpdcSrc ) - nXOriginDest - xoff;
        yoff = nYOriginSrc + YOFFSET( lpdcSrc ) - nYOriginDest - yoff;
		//目标裁剪域
        lprnNode = lprgn->lpNodeFirst;

		if( lpdcSrc->lpDispDrv )
		{   //需要对每一个点进行处理 ，点对点拷贝 pixel -> pixel
			// 这个将会大量的处理时间！
			_LPCDISPLAYDRV lpSrcDispDrv = lpdcSrc->lpDispDrv;
			_PIXELDATA pxSrc, pxDest;

			//InitAlphaCache( alphaSrcCache, alphaDestCache, lpBlendFunction->bf.SourceConstantAlpha );
			InitAlphaCache( alphaCache, lpBlendFunction->bf.SourceConstantAlpha );
			
			pxSrc.lpDestImage = lpSrcImage;
			pxSrc.pattern = 0xff;
			pxSrc.rop = R2_NOP;// read only
			
			pxDest.lpDestImage = lpDestImage;
			pxDest.pattern = 0xff;
			
			pxDest.rop = R2_COPYPEN;

			//遍历每一个裁剪域，如果可显示则绘制
			while( lprnNode )
			{	//得到内交区域
				if( IntersectRect( &rcDest, &rcClip, &lprnNode->rect ) )
				{
					rcSrc = rcDest;
					OffsetRect( &rcSrc, xoff, yoff );
					//对每个点进行转换

					//PixelAlphaTransfer( lpdcDest, &pxDest, &rcDest, lpdcSrc, &pxSrc, &rcSrc, alphaSrcCache, alphaDestCache, rgbTransparent );
					PixelAlphaTransfer( lpdcDest, &pxDest, &rcDest, lpdcSrc, &pxSrc, &rcSrc, alphaCache, rgbTransparent );
					
				}
				lprnNode = lprnNode->lpNext;//下一个裁剪区
			}
		}

        bRetv = TRUE;
    }

_return:
	 
	_LeaveDCPtr( lpdcDest, DCF_WRITE );
    _LeaveDCPtr( lpdcSrc, DCF_READ );

	return bRetv;
}


// **************************************************
// 声明：BOOL WINAPI WinGdi_SetDeviceWindow( HDC hdc, int xOrg, int yOrg, LPPOINT lppt )
// 参数：
//	IN hdc-目标句柄
//	IN xOrg - 新的原点x
//	IN yOrg - 新的原点y
//	OUT lpPoint-接收之前的原点,POINT结构指针
// 返回值：
//	假入成功，返回TRUE；否则，返回FALSE
// 功能描述：
//	设置窗口原点
// 引用: 
//	
// ************************************************

BOOL WINAPI WinGdi_SetDeviceWindow( HDC hdc, LPCRECT lprc )
{
	_LPGDCDATA lpdc;
	
	if( (lpdc = _GetHDCPtr( hdc )) != 0 )
    {
		lpdc->deviceOrg.x = lprc->left;
		lpdc->deviceOrg.y = lprc->top;
        lpdc->deviceExt.cx = lprc->right - lprc->left;//窗口宽度
        lpdc->deviceExt.cy = lprc->bottom - lprc->top;//窗口高度		
		return TRUE;
    }
	return FALSE;
}


HRGN WINAPI WinGdi_SetDeviceRgn( HDC hdc, HRGN hrgn )
{
	_LPGDCDATA lpdc;
	
	if( (lpdc = _GetHDCPtr( hdc )) != 0 )
    {
		HRGN hrgnOld = lpdc->hrgn;
		lpdc->hrgn = hrgn;
		//lpdc->deviceOrg.x = lprc->left;
		//lpdc->deviceOrg.y = lprc->top;
        //lpdc->deviceExt.cx = lprc->right - lprc->left;//窗口宽度
        //lpdc->deviceExt.cy = lprc->bottom - lprc->top;//窗口高度		
		return hrgnOld;
    }
	return NULL;
}
