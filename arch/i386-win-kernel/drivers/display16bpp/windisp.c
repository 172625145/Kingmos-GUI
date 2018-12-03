#include <windows.h>
#include <eversion.h>
#include <edevice.h>
#include "..\include\w32cfg.h"
#include <eobjtype.h>
#include <crtdbg.h>
//#include <edef.h>


typedef struct _DIRTYRECT
{
    LONG left;//short left;
	LONG top;//short top;
	LONG right;//short right;
	LONG bottom;//short bottom;
}_DIRTYRECT, *LPDIRTYRECT;

typedef struct _BITMAPDATA
{
    DWORD        objType;      // == OBJ_BITMAP
	DWORD       dwDumy1;
	DWORD       dwDumy2;
	DWORD       dwDumy3;
	DWORD       dwDumy4;

    //BYTE      bmType;
	//BYTE      bmDumy;
	WORD      bmFlags;//
	BYTE      bmPlanes;
	BYTE      bmBitsPixel;

    int       bmWidth;
    int       bmHeight;
    int       bmWidthBytes;
    //WORD        bmPlanes;
    //WORD        bmBitsPixel;
    LPBYTE    bmBits;
}_BITMAPDATA, * _LPBITMAPDATA;


_BITMAPDATA oemDisplayBitmap;// = { OBJ_BITMAP, 0, 160, 160, 160, 1, 8, 0 };

//LPBYTE __lpwDisplayFrameBuf = 0;

//HDC hDeskTopDC = 0;
//HDC hMemDC = 0;
RECT rcDirty = { 0, 0, 0, 0 };
extern CRITICAL_SECTION csDisplay;


void DrawDirtyRect( _LPBITMAPDATA lpDestImage, void * lp )
{
	//extern HANDLE hFlushEvent;
    LPDIRTYRECT lprect;
//    COLORREF color;
    RECT rc;
    RECT rcLimit = { 0, 0, iDisplayWidth, iDisplayHeight };

//    HBITMAP h;
    //static int f = 0;
   
    EnterCriticalSection(&csDisplay);

	lprect = (LPDIRTYRECT)lp;
    rc.left = lprect->left;
    rc.top = lprect->top;
    rc.right = lprect->right;
    rc.bottom = lprect->bottom;

    UnionRect(&rcDirty,&rcDirty,&rc);
	IntersectRect( &rcDirty, &rcDirty, &rcLimit );    //
	_ASSERT( rcDirty.left >=0 && rcDirty.right <= iDisplayWidth &&
		     rcDirty.top  >=0 && rcDirty.bottom <= iDisplayHeight );


    LeaveCriticalSection(&csDisplay);
    //SetEvent( hFlushEvent );
    //return ;
/*

    h = CreateBitmap( iDisplayWidth, iDisplayHeight, iPlane, iBitsPerPel, __lpwDisplayFrameBuf );
    //f = 1 - f;
	h = (HBITMAP)SelectObject( hMemDC, h );
    color = SetBkColor( hDeskTopDC, CL_BKLIGHT );
    //SetBkMode( hDeskTopDC, OPAQUE );
	BitBlt( hDeskTopDC, lprect->left+iDisplayOffsetX, lprect->top+iDisplayOffsetY,
		    (lprect->right - lprect->left),
			(lprect->bottom - lprect->top ),
			hMemDC,
			lprect->left, lprect->top,
			SRCCOPY );
    //SetBkColor( hDeskTopDC, color );
    h = (HBITMAP)SelectObject( hMemDC, h );
	DeleteObject( h );
*/
}

BOOL DoDisplayerDestroy( void )
{
    //DeleteDC( hMemDC );
	//ReleaseDC( hwndDeskTop, hDeskTopDC );
	return TRUE;
}

LPVOID lpvSaveFrameBuffer = NULL;
static bPowerOff = FALSE;
VOID DoPowerOff( VOID )
{
	if( lpvSaveFrameBuffer )
	{
		EnterCriticalSection(&csDisplay);
		memcpy( lpvSaveFrameBuffer, oemDisplayBitmap.bmBits, oemDisplayBitmap.bmWidthBytes * oemDisplayBitmap.bmHeight );
		memset( oemDisplayBitmap.bmBits, 0, oemDisplayBitmap.bmWidthBytes * oemDisplayBitmap.bmHeight );
		rcDirty.left = rcDirty.top = 0;
		rcDirty.right = iDisplayWidth;
		rcDirty.bottom = iDisplayHeight;
		LeaveCriticalSection(&csDisplay);
	}
	bPowerOff = TRUE;
}

VOID DoPowerOn( VOID )
{
	if( bPowerOff && lpvSaveFrameBuffer )
	{
		EnterCriticalSection(&csDisplay);
		memcpy( oemDisplayBitmap.bmBits, lpvSaveFrameBuffer, oemDisplayBitmap.bmWidthBytes * oemDisplayBitmap.bmHeight );
		rcDirty.left = rcDirty.top = 0;
		rcDirty.right = iDisplayWidth;
		rcDirty.bottom = iDisplayHeight;

		LeaveCriticalSection(&csDisplay);
	}
	bPowerOff = FALSE;
}


extern LPBYTE GetFrameBuf( void );
int DoDisplayerCreate( void )
{
	memset( &oemDisplayBitmap, 0, sizeof( oemDisplayBitmap ) );
    oemDisplayBitmap.objType = OBJ_BITMAP;
    oemDisplayBitmap.bmFlags = 0;
    oemDisplayBitmap.bmWidth = iDisplayWidth;
    oemDisplayBitmap.bmHeight = iDisplayHeight;
    oemDisplayBitmap.bmPlanes = iPlane;
    oemDisplayBitmap.bmBitsPixel = iBitsPerPel;
    oemDisplayBitmap.bmWidthBytes = iDisplayWidth * iBitsPerPel / 8;
    oemDisplayBitmap.bmBits = GetFrameBuf();//malloc( oemDisplayBitmap.bmWidthBytes * oemDisplayBitmap.bmHeight );
	lpvSaveFrameBuffer = malloc( oemDisplayBitmap.bmWidthBytes * oemDisplayBitmap.bmHeight );
	//__lpwDisplayFrameBuf = oemDisplayBitmap.bmBits;

	/*
	if( hwndDeskTop && hDeskTopDC == 0 )
	{
		hDeskTopDC = GetDC( hwndDeskTop );
		if( hDeskTopDC )
		{
            hMemDC = CreateCompatibleDC( hDeskTopDC );
		}
	}
*/
	return 1;
}
/*
int CALLBACK __DevDisplayEnter( int msg, DWORD lParam )
{
    switch( msg )
    {
    case DEV_CREATE:
        return DoCreate();
    case DEV_DESTROY:
        return DoDestroy();
    case DEV_GETMODENUM:
        return 1;
    case DEV_GETBITMAP:
        *((_LPBITMAPDATA)lParam) = *((_LPBITMAPDATA)&oemDisplayBitmap);
        return 1;
    case DEV_GETBITMAPPTR:
        *((_LPBITMAPDATA*)lParam) = &oemDisplayBitmap;
        return 1;
    }
    return 0;
}
*/

