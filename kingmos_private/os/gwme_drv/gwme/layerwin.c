/******************************************************
Copyright(c) 版权所有，1998-2005微逻辑。保留所有权利。
******************************************************/


/*****************************************************
文件说明：Layered Window 管理模块
版本号：1.0.0
开发时期：2005-10-14
作者：李林
修改记录：
******************************************************/

#include <eframe.h>
#include <estring.h>
#include <eassert.h>
#include <ekeybrd.h>
#include <eapisrv.h>

#include <bheap.h>
#include <epwin.h>
#include <gwmeobj.h>
#include <winsrv.h>
#include <gdisrv.h>
#include <eobjcall.h>
#include <gdc.h>

static HANDLE hFlushEvent;
static DWORD WINAPI FlushProc(
  LPVOID lpParameter   // thread data
);


// 设置 layer 窗口属性
BOOL WINAPI Win_SetLayeredAttributes(
  HWND hWnd,           // 窗口句柄
  COLORREF clrTransparentKey,      // 透明颜色
  BYTE bAlpha,         // value for the blend function
  DWORD dwFlags        // 需要设置的动作： LWA_COLORKEY, clrTransparentKey 有效
					 	//				   LWA_ALPHA,  bAlpha 有效

)
{
	_LPWINDATA lpws;
	lpws = _GetHWNDPtr( hWnd );
	
	if( lpws )
	{		
		if( lpws->dwExStyle & WS_EX_LAYERED )
		{	// 
			if( lpws->lpLayer == NULL )
			{
				lpws->lpLayer = BlockHeap_Alloc( hgwmeBlockHeap, 0, sizeof(_LAYERED_DATA) );
				memset( lpws->lpLayer, 0, sizeof(_LAYERED_DATA) );
			}
			if( lpws->lpLayer )
			{
				if( dwFlags & LWA_COLORKEY )
					lpws->lpLayer->clrTransparentKey = clrTransparentKey;
				else
					lpws->lpLayer->clrTransparentKey = -1;
				if( dwFlags & LWA_ALPHA )
					lpws->lpLayer->bAlpha = bAlpha;
				AddMsgToThreadQueue( lpws->hThis, WM_NCPAINT, 1, 0, ATQ_ONLYONE | QS_POSTMESSAGE );
				AddMsgToThreadQueue( lpws->hThis, WM_PAINT, 0, 0, ATQ_ONLYONE | QS_POSTMESSAGE );
				return TRUE;
			}				
		}
	}
	return FALSE;
}

_LPWINDATA NeedLayered( _LPWINDATA lpws )
{
	while( lpws )
	{
		if( lpws->dwMainStyle & WS_CHILD )
			lpws = lpws->lpwsParent;
		else
		{
			if( lpws->lpLayer && (lpws->dwExStyle & WS_EX_LAYERED) )
				return lpws; // 顶层窗口
			return NULL;
		}
	}
	return NULL;
}

HDC GetLayerWindowDC( _LPWINDATA lpwsThis, _LPWINDATA lpwsLayered, DWORD dwFlag )
{
	if( lpwsLayered->lpLayer->hBitmap == 0 )
	{
		HBITMAP hBitmap = WinGdi_CreateCompatibleBitmap( NULL,  lpwsLayered->rectWindow.right - lpwsLayered->rectWindow.left, lpwsLayered->rectWindow.bottom - lpwsLayered->rectWindow.top );
		lpwsLayered->lpLayer->hBitmap = hBitmap;
	}

	if( lpwsLayered->lpLayer->hBitmap )
	{
		RECT rcThis;
		HDC hdc = WinGdi_CreateCompatibleDC( NULL );

		if( hdc )
		{
			SelectObject( hdc, lpwsLayered->lpLayer->hBitmap );
			if( dwFlag & DCX_WINDOW )
				rcThis = lpwsThis->rectWindow;
			else
			{
				rcThis = lpwsThis->rectClient;
				OffsetRect( &rcThis, lpwsThis->rectWindow.left, lpwsThis->rectWindow.top ); // to screen 坐标
			}
			if( IntersectRect( &rcThis, &rcThis, &lpwsLayered->rectWindow ) )
			{
				OffsetRect( &rcThis, -lpwsLayered->rectWindow.left, -lpwsLayered->rectWindow.top );
			}
			ResetDCData( hdc, &rcThis );			
		}
		return hdc;
	}
	return NULL;
}

BOOL ReleaseLayerWindowDC( HWND hWnd, HDC hdc )
{
	_LPWINDATA lpws;
	
	WinGdi_DeleteDC( hdc );

	lpws = _GetHWNDPtr( hWnd );
	if( lpws )
	{
		return _FlushGdi( &lpws->rectWindow );
	}
	return FALSE;	
}

static VOID UpdateLayerWindow( _LPWINDATA lpwsLayered, HBITMAP hbmpFrameBuf, LPCRECT lprcDirty )
{
	if( lpwsLayered->lpLayer )
	{
		if( lpwsLayered->lpLayer->hBitmap )
		{
			BLENDFUNCTION_EX bfex;
			//HDC hdcScreen;
			//RECT rcClip = *lprcDirty;
			HRGN hrgnExpose;
			int retv;
			RECT rc;
			// 逻辑坐标
			//OffsetRect( &rcClip, -lpwsLayered->rectWindow.left, -lpwsLayered->rectWindow.top );			
			if( IntersectRect( &rc, lprcDirty, &lpwsLayered->rectWindow ) == FALSE )
				return;
			
			//lpwsLayered->dwExStyle &= ~WS_EX_LAYERED;
			retv = GetExposeRgn( lpwsLayered->hThis, DCX_NOLAYERED | DCX_CLIPSIBLINGS | DCX_WINDOW, &hrgnExpose );
			if( hrgnExpose && retv != ERROR && retv != NULLREGION )
			{
				HRGN hrgn = CreateRectRgn( lprcDirty->left, lprcDirty->top, lprcDirty->right, lprcDirty->bottom );
				int retv = CombineRgn( hrgnExpose, hrgnExpose, hrgn, RGN_AND );
				if( hrgn && retv != ERROR && retv != NULLREGION )
				{
					extern BOOL WINAPI WinGdi_SetDeviceWindow( HDC hdc, LPCRECT lprc );
					extern HRGN WINAPI WinGdi_SetDeviceRgn( HDC hdc, HRGN hrgn );

					HDC hdcMem = WinGdi_CreateCompatibleDC( NULL );
					HDC hdcScreen = WinGdi_CreateCompatibleDC( NULL );
					HRGN hrgnOld;
					
					SelectObject( hdcMem, lpwsLayered->lpLayer->hBitmap );
					
					SelectObject( hdcScreen, hbmpFrameBuf );
					WinGdi_SetDeviceWindow( hdcScreen, &lpwsLayered->rectWindow );
					hrgnOld = WinGdi_SetDeviceRgn( hdcScreen, hrgnExpose );
					//SelectObject( hdcScreen, hrgnExpose );
					
					
					bfex.bf.AlphaFormat = AC_SRC_ALPHA;
					bfex.bf.BlendFlags = 0;
					bfex.bf.BlendOp = AC_SRC_OVER;
					bfex.bf.SourceConstantAlpha = lpwsLayered->lpLayer->bAlpha;						
					bfex.rgbTransparent = lpwsLayered->lpLayer->clrTransparentKey;						
					WinGdi_AlphaBlendEx(hdcScreen, 
						0, 0, 
						lpwsLayered->rectWindow.right - lpwsLayered->rectWindow.left,
						lpwsLayered->rectWindow.bottom - lpwsLayered->rectWindow.top,
						hdcMem, 0, 0, 
						lpwsLayered->rectWindow.right - lpwsLayered->rectWindow.left,
						lpwsLayered->rectWindow.bottom - lpwsLayered->rectWindow.top, &bfex );

					WinGdi_SetDeviceRgn( hdcScreen, hrgnOld );
					
					
					DeleteDC( hdcScreen );
					DeleteDC( hdcMem );					
				}
				if( hrgn )
					DeleteObject( hrgn );
			}
			if( hrgnExpose )
				DeleteObject( hrgnExpose );

		}
	}
}

/*
static VOID UpdateLayerWindow( _LPWINDATA lpwsLayered, HBITMAP hbmpFrameBuf, LPCRECT lprcDirty )
{
	if( lpwsLayered->lpLayer )
	{
		if( lpwsLayered->lpLayer->hBitmap )
		{
			BLENDFUNCTION_EX bfex;
			//HDC hdcScreen;
			RECT rcClip = *lprcDirty;
			HRGN hrgnExpose;
			HDC hdcMem = WinGdi_CreateCompatibleDC( NULL );
			HDC hdcScreen;
			int retv;
			HRGN hrgn;
			
			// 逻辑坐标
			OffsetRect( &rcClip, -lpwsLayered->rectWindow.left, -lpwsLayered->rectWindow.top );			
			hrgn = CreateRectRgn( rcClip.left, rcClip.top, rcClip.right, rcClip.bottom );
			
			//lpwsLayered->dwExStyle &= ~WS_EX_LAYERED;
			hdcScreen = WinGdi_GetDCEx( lpwsLayered->hThis, hrgn, DCX_NOLAYERED | DCX_CLIPSIBLINGS | DCX_WINDOW | DCX_INTERSECTRGN );
			DeleteObject( hrgn );

			SwitchMainFrameBuffer( hdcScreen, hbmpFrameBuf );
			
			//lpwsLayered->dwExStyle |= WS_EX_LAYERED;
			
			SelectObject( hdcMem, lpwsLayered->lpLayer->hBitmap );
			
			bfex.bf.AlphaFormat = AC_SRC_ALPHA;
			bfex.bf.BlendFlags = 0;
			bfex.bf.BlendOp = AC_SRC_OVER;
			bfex.bf.SourceConstantAlpha = lpwsLayered->lpLayer->bAlpha;
			
			bfex.rgbTransparent = lpwsLayered->lpLayer->clrTransparentKey;
			
			
			WinGdi_AlphaBlendEx(hdcScreen, 
				0, 0, 
				lpwsLayered->rectWindow.right - lpwsLayered->rectWindow.left,
				lpwsLayered->rectWindow.bottom - lpwsLayered->rectWindow.top,
				hdcMem, 0, 0, 
				lpwsLayered->rectWindow.right - lpwsLayered->rectWindow.left,
				lpwsLayered->rectWindow.bottom - lpwsLayered->rectWindow.top, &bfex );
			
			WinGdi_DeleteDC( hdcMem );
			ReleaseDC( lpwsLayered->hThis, hdcScreen );
		}
	}
}
*/

static HBITMAP hMainLayerFrameBuffer;
_LPBITMAPDATA lpMainLayerImage; 
extern HBITMAP hbmpCurrentFrameBuffer;    // 当前所用的显示面

static CRITICAL_SECTION csDCFlush;

extern const _DISPLAYDRV _vDisplayDriver;
static RECT rcFlush;

BOOL _InitLayer( VOID )
{
	hMainLayerFrameBuffer = WinGdi_CreateCompatibleBitmap( NULL, GetSystemMetrics( SM_CXSCREEN ),  GetSystemMetrics( SM_CYSCREEN ) );
	lpMainLayerImage = _GetHBITMAPPtr( hMainLayerFrameBuffer );
	lpDrvDisplayDefault = &_vDisplayDriver;
	hbmpCurrentFrameBuffer = hMainLayerFrameBuffer;////SwitchMainFrameBuffer( hMainLayerFrameBuffer );
	//InitializeCriticalSection( &csDCFlush );
	//hFlushEvent = CreateEvent( NULL, 0, 0, 0 );
	//CreateThread( NULL, 0, FlushProc, NULL, 0, NULL );
	//rcFlush.left = rcFlush.top = rcFlush.right = rcFlush.bottom = 0;
	
	return TRUE;
}

static VOID UpdateAllLayerWindow( HBITMAP hbmpFrameBuf, LPCRECT lprcDirty )
{
	_LPWINDATA lpws = hwndScreen.lpwsChild;
	HWND hWnd;

	if( lpws )
	{
		hWnd = GetWindow( lpws->hThis, GW_HWNDLAST );
		lpws = _GetHWNDPtr( hWnd );
		while( lpws )
		{
			if( lpws->dwExStyle & WS_EX_LAYERED )
				UpdateLayerWindow( lpws, hbmpFrameBuf, lprcDirty );
			lpws = lpws->lpwsPrev;
		}
	}
}

BOOL _kFlushGdi( LPRECT lprcDirty )
{
	EnterCriticalSection( &csDCFlush );  //进入互斥段

	UnionRect( &rcFlush, &rcFlush, lprcDirty );
	SetEvent( hFlushEvent );

	LeaveCriticalSection( &csDCFlush );  //离开互斥段
}

BOOL _FlushGdi( LPCRECT lprcDirty )
{
	static HBITMAP hbmpFrameBuf = NULL;
	static HDC hdcFrame = NULL;
	static DWORD dwFrameBufSize = 0;
	static _LPBITMAPDATA lpFrameBuf = NULL;

	//DWORD dwCurTick = GetTickCount();
	
	if( hdcFrame == NULL )
	{
		hbmpFrameBuf = WinGdi_CreateCompatibleBitmap( NULL, GetSystemMetrics( SM_CXSCREEN ),  GetSystemMetrics( SM_CYSCREEN ) );
		if( hbmpFrameBuf )
		{
			hdcFrame = WinGdi_CreateCompatibleDC( NULL );
			if( hdcFrame )
			{
				SelectObject( hdcFrame, hbmpFrameBuf );
				lpFrameBuf = _GetHBITMAPPtr( hbmpFrameBuf );
				dwFrameBufSize = lpFrameBuf->bmWidthBytes * lpFrameBuf->bmHeight;
			}
			else
				DeleteObject( hbmpFrameBuf );
		}
	}

	if( hdcFrame )
	{
		_BLKBITBLT blt;

		HDC hdcScreen;
		HBITMAP hOld;
		//_LPBITMAPDATA lpMainImage = _GetHBITMAPPtr( hMainLayerFrameBuffer );
		//_LPBITMAPDATA lpDeviceImage = _GetHBITMAPPtr( hbmpDisplayBitmap );
		//memcpy( lpFrameBuf->bmBits, lpMainLayerImage->bmBits, dwFrameBufSize );


        blt.lpDestImage = lpFrameBuf;		
		blt.lpSrcImage = lpMainLayerImage;

		blt.lpBrush = NULL;
		blt.lpptBrushOrg = NULL;

        blt.lprcSrc = lprcDirty;
        blt.lprcDest = lprcDirty;
		blt.lprcMask = NULL;
        blt.dwRop = SRCCOPY;

		lpDrvDisplayDevice->lpBlkBitBlt( &blt );


		
/*
		hdcScreen = GetDC( NULL );
		BitBlt( hdcFrame, 0, 0, 
				GetSystemMetrics( SM_CXSCREEN ), 
				GetSystemMetrics( SM_CYSCREEN ),
				hdcScreen,
				0,
				0,
				SRCCOPY );
		ReleaseDC( NULL, hdcScreen );
*/

		// Lock DC
		UpdateAllLayerWindow(hbmpFrameBuf, lprcDirty);

		//memcpy( lpDeviceImage->bmBits, lpFrameBuf->bmBits, dwFrameBufSize );

/*
		hdcScreen = GetDC( NULL );
		SwitchMainFrameBuffer( hdcScreen, hbmpDisplayBitmap );
		BitBlt( hdcScreen, 0, 0, 
				GetSystemMetrics( SM_CXSCREEN ), 
				GetSystemMetrics( SM_CYSCREEN ),
				hdcFrame,
				0,
				0,
				SRCCOPY );
		ReleaseDC( NULL, hdcScreen );
*/
        blt.lpDestImage = lpDisplayBitmap;		
		blt.lpSrcImage = lpFrameBuf;
		lpDrvDisplayDevice->lpBlkBitBlt( &blt );
		//lpGwdiDisplayEnter( GWDI_FLUSH, 0, 0 );

		// Release DC	
	}
	//RETAILMSG( 1, ( "dwTick=%d.\r\n", GetTickCount() - dwCurTick ) );
	return TRUE;
}

BOOL Gwme_FlushGdi( void )
{
	//SetEvent( hFlushEvent );
	return TRUE;
/*
	if( ufNeedFlushGDICount )
	{
		ufNeedFlushGDICount = 0;
		
		_FlushGdi();
	}
	return TRUE;
*/
}
#if 0
static DWORD WINAPI FlushProc(
							  LPVOID lpParameter   // thread data
							  )
{
	RECT rc;
	while( 1 )
	{
		WaitForSingleObject( hFlushEvent, -1 );
		EnterCriticalSection( &csDCFlush );  //进入互斥段

		rc = rcFlush;
		rcFlush.left = rcFlush.top = rcFlush.right = rcFlush.bottom = 0;

		LeaveCriticalSection( &csDCFlush ); 
		__FlushGdi( &rc );
		
		//while( ufNeedFlushGDICount )
		//{
		//	ufNeedFlushGDICount = 0;			
			//_FlushGdi();
		//}
	}
}
#endif