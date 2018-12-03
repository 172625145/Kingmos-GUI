/******************************************************
Copyright(c) ��Ȩ���У�1998-2005΢�߼�����������Ȩ����
******************************************************/


/*****************************************************
�ļ�˵��������/װ��bitmap,icon,cursor��
          LoadImage, LoadCursor, LoadIcon
�汾�ţ�3.0.0
����ʱ�ڣ�1999
���ߣ�����
�޸ļ�¼��
	2005-05-31, ��1�� _WinGdi_ConvertImageColorValue 8~32bytes format û�м�Դ���ݵ�ƫ�ƣ�
	            ��2���Ż� _WinGdi_ConvertImageColorValue
    2003-07-14, _WinGdi_ConvertColorValue Ϊ SetDIBitsToDevice �ı���ýӿ�
    2003-07-09 LoadImage �� cxDesired & cyDesiredΪ0����û��LR_DEFAULTSIZE 
	           ʱӦΪ��ʵ��icon & cursor size
    2003-07-04 GetResFileHandle ��\\system\\system.res ��Ϊ �� hgwmeInst�õ�
    2003-05-06-2003.05.07: LN
	  1. ��ֱ�Ӵ��ļ���Ϊ��hmodule�õ��ļ������res��ƫ��
	  2. ȥ�����õ�Code
	  3. �� GetSystemMetrics ��Ϊ WinSys_GetMetrics
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
// ������_LPICONDATA _GetHICONPtr( HICON hIcon )
// ������
// 	IN hIcon - ICON�������
// ����ֵ��
//	����ɹ������� _ICONDATA �ṹָ�룻���򣬷��� NULL
// ����������
//	�����������õ�����ָ��
// ����: 
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
// ������BOOL __InitialGdiIconCursorMgr( void )
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	��ʼ�� ICON Cursor ���������
// ����: 
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
// ������void __DeInitialGdiIconCursorMgr( void )
// ������
// 	��
// ����ֵ��
//	��
// ����������
//	��__InitialGdiIconCursorMgr�෴���ͷŷ������Դ
// ����: 
//	
// ************************************************

void _DeInitialGdiIconCursorMgr( void )
{
	DeleteCriticalSection( &csIconObjList );
	DeleteCriticalSection( &csCursorObjList );
}

// **************************************************
// ������static HICON _CreateIconIndirect( LPICONINFO lpIconInfo, UINT uiName, BOOL bCopy )
// ������
// 	IN lpIconInfo - ����ICON��Ϣ��ICONINFO�ṹָ��
//	IN uiName - ICON id��
//	IN bCopy - �Ƿ����¿��� һ�� bitmap �������
// ����ֵ��
//	����ɹ������ؾ�����󣻷��򣬷���NULL
// ����������
//	���� ICON ����
// ����: 
//	
// ************************************************

static HICON _CreateIconIndirect( LPICONINFO lpIconInfo, UINT uiName, BOOL bCopy )
{
	_LPICONDATA lpIcon;

	lpIcon = (_LPICONDATA)BlockHeap_Alloc( hgwmeBlockHeap, 0, sizeof(_ICONDATA) );

	if( lpIcon )
	{		
		_BITMAPDATA * lpBitmap;
		//ICON����
		lpIcon->fIcon = lpIconInfo->fIcon; // define as ICON, else as CURSOR
		//�Ƿ񿽱�һ���µ�
		if( lpIconInfo->hbmColor && bCopy )
		{	//��
			lpBitmap = (_BITMAPDATA *)lpIconInfo->hbmColor;
		    lpIcon->hbmColor = WinGdi_CreateBitmap( lpBitmap->bmWidth, lpBitmap->bmHeight, lpBitmap->bmPlanes, lpBitmap->bmBitsPixel, lpBitmap->bmBits );
		}
		else
			lpIcon->hbmColor = lpIconInfo->hbmColor;
		//�Ƿ񿽱�һ���µ�
		if( lpIconInfo->hbmMask && bCopy )
		{	//��
			lpBitmap = (_BITMAPDATA *)lpIconInfo->hbmMask;
		    lpIcon->hbmMask = WinGdi_CreateBitmap( lpBitmap->bmWidth, lpBitmap->bmHeight, lpBitmap->bmPlanes, lpBitmap->bmBitsPixel, lpBitmap->bmBits );
		}
		else
		    lpIcon->hbmMask = lpIconInfo->hbmMask;

		if( lpIcon->hbmMask && lpIcon->hbmColor )
		{	//��ʼ������
			lpIcon->xHotspot = lpIconInfo->xHotspot;
			lpIcon->yHotspot = lpIconInfo->yHotspot;
			lpIcon->wIconName = uiName;
			
			if( lpIcon->fIcon )
			{	//����Icon��������
				EnterCriticalSection( &csIconObjList );
				ObjList_Init( &lpIconObjList, &lpIcon->obj, OBJ_ICON, (ULONG)GetCallerProcess() );
				LeaveCriticalSection( &csIconObjList );
			}
			else
			{	//����Cursor��������
				EnterCriticalSection( &csCursorObjList );
				ObjList_Init( &lpCursorObjList, &lpIcon->obj, OBJ_ICON, (ULONG)GetCallerProcess() );
				LeaveCriticalSection( &csCursorObjList );
			}
		}
		else
		{	//��ʼ������ʧ�ܣ��������������
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
// ������HICON WINAPI WinGdi_CreateIconIndirect( LPICONINFO lpIconInfo )
// ������
// 	IN lpIconInfo - ���� ICON ������Ϣ�Ľṹ
// ����ֵ��
//	����ɹ������ؾ�����󣻷��򣬷���NULL
// ����������
//	���� ICON ����
// ����: 
//	ϵͳAPI
// ************************************************

HICON WINAPI WinGdi_CreateIconIndirect( LPICONINFO lpIconInfo )
{
	return _CreateIconIndirect( lpIconInfo, -1, 1 );
}

// **************************************************
// ������BOOL WINAPI WinGdi_DestroyIcon( HICON hIcon )
// ������
// 	hIcon �Cͼ����
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�ƻ�֮ǰ������ͼ��
// ����: 
//	ϵͳAPI
// ************************************************
#define DEBUG_DESTROY_ICON 0
BOOL WINAPI WinGdi_DestroyIcon( HICON hIcon )
{	//�ɾ���õ�����ָ��
    _LPICONDATA lpIcon = _GetHICONPtr( hIcon );

	if( lpIcon )
	{
		if( Interlock_Decrement( (LPLONG)&lpIcon->obj.iRefCount ) == 0 )
		{	//��ǰû�жԸö�������ã��ͷ���
			EnterCriticalSection( &csIconObjList );
			//�Ӷ��������Ƴ�
			ObjList_Remove( &lpIconObjList, &lpIcon->obj );

			LeaveCriticalSection( &csIconObjList );
			//�ͷŶ�����Դ
			_FreeIconCursor( &lpIcon->obj, 0 );
		}
		else
		{	//��Ȼ��ʹ��
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
// ������BOOL WINAPI WinGdi_DestroyCursor( HCURSOR hCursor )
// ������
// 	hCursor �C�����
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�ƻ�֮ǰ�����Ĺ��
// ����: 
//	ϵͳAPI
// ************************************************

BOOL WINAPI WinGdi_DestroyCursor( HCURSOR hCursor )
{	//�ɾ���õ�����ָ��
    _LPICONDATA lpIcon = _GetHICONPtr( (HICON)hCursor );

	if( lpIcon )
	{
		if( Interlock_Decrement( (LPLONG)&lpIcon->obj.iRefCount ) == 0 )
		{	//��ǰû�жԸö�������ã��ͷ���
			EnterCriticalSection( &csCursorObjList );
			//�Ӷ��������Ƴ�
			ObjList_Remove( &lpCursorObjList, &lpIcon->obj );
			LeaveCriticalSection( &csCursorObjList );
			//�ͷŶ�����Դ
			_FreeIconCursor( &lpIcon->obj, 0 );
		}
		else
		{	//��Ȼ��ʹ��
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
// ������static HANDLE _FindIconCursor( 
//								HINSTANCE hInst, 
//								LPOBJLIST * lppList,
//								UINT uiName, 
//								int cxDesired, 
//								int cyDesired )
// ������
//	IN hInst - ����ӵ����
//	IN lppList - ��Ҫ���ҵĶ�������
//	IN uiName - ������
//	IN cxDesired -	����Ҫ��Ķ�����
//	IN cyDesired - ����Ҫ��Ķ���߶�
// ����ֵ��
//	����ɹ������ؾ�����󣻷��򣬷���NULL
// ����������
//	���������Ҷ���
// ����: 
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
			//�Ƿ�������� ��
			if( lpbmp &&
				IS_SIZE_MATCH( lpbmp->bmWidth, lpbmp->bmHeight, cxDesired, cyDesired ) )
			{	//�ҵ�
				return (HANDLE)lpIcon;
			}
		}
		//��һ������
		lpObj = lpObj->lpNext;
	}
	return NULL;
}

// **************************************************
// ������static BOOL CALLBACK _FreeIconCursor( LPOBJLIST lpObj, LPARAM lParam )
// ������
// 	IN lpObj - ��������
//	IN lParam - ö�ٶ���ص����������ĸ��Ӳ���
// ����ֵ��
//	����Ҫ����ö��ʱ������TRUE�����򣬷���FALSE
// ����������
//	ö�ٶ���ص��������
// ����: 
//	
// ************************************************
#define DEBUG_FREE_ICON_CURSOR 0
static BOOL CALLBACK _FreeIconCursor( LPOBJLIST lpObj, LPARAM lParam )
{
	// free mem
	//�ͷ���Դ
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
// ������void FreeInstanceIcon( HANDLE hOwner )
// ������
// 	IN hOwner - ����ӵ����
// ����ֵ��
//	��
// ����������
//	�ͷ�ӵ���ߵ�����ICON��Դ���������˳�ʱ��ϵͳ���������øú�����
// ����: 
//	
// ************************************************

void FreeInstanceIcon( HANDLE hOwner )
{
	EnterCriticalSection( &csIconObjList );
	ObjList_Delete( &lpIconObjList, (ULONG)hOwner, _FreeIconCursor, 1 );
	LeaveCriticalSection( &csIconObjList );
}

// **************************************************
// ������void FreeInstanceCursor( HANDLE hOwner )
// ������
// 	IN hOwner - ����ӵ����
// ����ֵ��
//	��
// ����������
//	�ͷ�ӵ���ߵ�����Cursor��Դ���������˳�ʱ��ϵͳ���������øú�����
// ����: 
//	
// ************************************************

void FreeInstanceCursor( HANDLE hOwner )
{
	EnterCriticalSection( &csCursorObjList );
	ObjList_Delete( &lpCursorObjList, (ULONG)hOwner, _FreeIconCursor, 1 );
	LeaveCriticalSection( &csCursorObjList );
}

// **************************************************
// ������static int GetColorTableNum( BITMAPINFOHEADER * lpbih )
// ������
// 	IN lpbih - �õ���ɫ������
// ����ֵ��
//	����ɹ���������ɫ�����������򣬷���0
// ����������
//	�õ� BITMAPINFOHEADER �ṹ�а�������ɫ������
// ����: 
//	
// ************************************************

static int GetColorTableNum( BITMAPINFOHEADER * lpbih )
{
    int clrTableItemNum = 0;
    if( lpbih->biCompression == BI_BITFIELDS )
        clrTableItemNum = 3;
    switch ( lpbih->biBitCount)
    {
    case 1:		//��ɫ �ڰ�
        if( lpbih->biClrUsed == 0 )
            clrTableItemNum += 2;
        else
            clrTableItemNum = lpbih->biClrUsed;
        break;
    case 2:		//4ɫ
        if( lpbih->biClrUsed == 0 )
            clrTableItemNum += 4;
        else
            clrTableItemNum = lpbih->biClrUsed;
        break;
    case 4:		//16ɫ
        if( lpbih->biClrUsed == 0 )
            clrTableItemNum += 16;
        else
            clrTableItemNum = lpbih->biClrUsed;
        break;
    case 8:		//256ɫ
        if( lpbih->biClrUsed == 0 )
            clrTableItemNum += 256;
        else
            clrTableItemNum = lpbih->biClrUsed;
        break;
    case 16:		//65536ɫ
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
// ������static int GetShiftValue( DWORD dwValue )
// ������
// 	IN dwValue - ֵ
// ����ֵ��
//	���������Чλ
// ����������
//	�õ���ֵ�������Чλ�����Ա����Ժ����λ����
// ����: 
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
// ������static void _ConvertRGBQUADColorRGBColor( BITMAPINFO * lpbi )
// ������
// 	IN/OUT lpbi - BITMAPINFO �ṹָ��
// ����ֵ��
//	��
// ����������
//	�� lpbi�е� RGBQUAD ��ɫ���ʽת��Ϊ RGB��ʽ
// ����: 
//	
// ************************************************

static void _ConvertRGBQUADColorRGBColor( BITMAPINFO * lpbi )
{
    RGBQUAD * lpquad = (RGBQUAD *)lpbi->bmiColors;
    COLORREF * lprgb = (COLORREF *)lpbi->bmiColors;
	//�õ���ɫ������
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
// ������static void _ConvertRGBQUADColorToPalColor( BITMAPINFO * lpbi )
// ������
// 	IN/OUT lpbi - BITMAPINFO �ṹָ��
// ����ֵ��
//	��
// ����������
//	�� lpbi�е� RGBQUAD ��ɫ���ʽת��Ϊ ��ɫ������ֵ
// ����: 
//	
// ************************************************

static void _ConvertRGBQUADColorToPalColor( BITMAPINFO * lpbi )
{
    RGBQUAD * lpquad = (RGBQUAD *)lpbi->bmiColors;
	INT16 * lpPal = (INT16 *)lpbi->bmiColors;
	//�õ���ɫ������
    int n = GetColorTableNum( &lpbi->bmiHeader );
    if( lpbi->bmiHeader.biCompression == BI_BITFIELDS )
    {
        n -= 3;
        lpquad += 3;
        lpPal += 6;
    }
    while( n )
    {	//�õ��豸��ص�ֵ
        *lpPal = (INT16)lpDrvDisplayDefault->lpRealizeColor( RGB( lpquad->rgbRed, lpquad->rgbGreen, lpquad->rgbBlue ), NULL, 0, 0 ); 
        lpPal++;
		lpquad++;
        n--;
    }
}

// **************************************************
// ������static void _ConvertMonoBitmap( 
//									_LPBITMAPDATA lpImage, 
//									DWORD dwStartScanLine, 
//									DWORD dwReadScanLine, 
//									DWORD dwScanLineWidth, 
//									LPBYTE lpbData, 
//									BITMAPINFO * lpbi, 
//									BOOL bInvert )
// ������
//	IN/OUT lpImage - Ŀ��λͼ����,��lpbData����ת�����ö���
//	IN dwStartScanLine - ��ʼɨ����
//	IN dwReadScanLine - ��Ҫ�����ɨ����
//	IN dwScanLineWidth - ɨ�����ֽ���
//	IN lpbData - ����λͼ���ݵ��ڴ�
//	IN lpbi - ���� lpbData ������Ϣ�Ľṹ
//	IN bInvert - �Ƿ�תλͼ����
// ����ֵ��
//	��
// ����������
//	�� lpbData ָ������ݿ����� lpImage��
// ����: 
//	
// ************************************************

static void _ConvertMonoBitmap( _LPBITMAPDATA lpImage, 
                                DWORD dwStartScanLine, 
                                DWORD dwReadScanLine, 
                                DWORD dwScanLineWidth, 
                                LPBYTE lpbData, 
                                BITMAPINFO * lpbi, 
                                BOOL bInvert )
{	//�Ƿ���ͬ�ĸ�ʽ ��
    if( lpbi->bmiHeader.biBitCount == 1 && 
        lpImage->bmBitsPixel == 1 )
    {	//��
        DWORD dwDestScanLineBytes = lpImage->bmWidthBytes;
		DWORD dwDestScanLineDwords = dwDestScanLineBytes >> 2;
		int iDestScanLineAddBytes = dwDestScanLineBytes;
        LPBYTE lpDest;

		if( lpbi->bmiHeader.biHeight > 0 )
		{	//λͼ����Ϊ�����ɵ͵�����
			lpDest = lpImage->bmBits + dwDestScanLineBytes * ( lpImage->bmHeight - dwStartScanLine - 1 );
			iDestScanLineAddBytes = -iDestScanLineAddBytes;
		}
		else
		{	//����
			lpDest = lpImage->bmBits + dwDestScanLineBytes * dwStartScanLine;
		}
		
        for( ; dwReadScanLine; dwReadScanLine--, lpbData += dwScanLineWidth )
        {
			LPBYTE lps, lpd;
			int i = dwDestScanLineDwords;
			int iWriteBytes = 0;
			lpd = lpDest; lps = lpbData;
			//�Ƿ�ת ��
			if( bInvert )
			{	//��ת  dest = ~source
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
			{	//ֱ�ӿ���
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
			//��һ��
            lpDest += iDestScanLineAddBytes;
        }
    }
}

// **************************************************
// ������int _WinGdi_ConvertImageColorValue( 
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
// ������
//	IN lpDrv - ��ʾ��������ӿ�
//	IN lpDestImage - ���ڽ���λͼ���ݵ�Ŀ��λͼ����
//	IN lprcDestClip - ��Ŀ���ϵĲü�����
//	IN lpbi - ����Դλͼ��Ϣ�Ľṹָ��
//	IN lprcSrcClip - ��Դλͼ�ϵĲü�����
//	IN dwStartScanLine - Դλͼ���ݵĿ�ʼ,  // src start scan line
//  IN dwScanLineNum, 
//	IN dwScanLineWidth,
//  IN lpbData,// src bitmap bits data
//	IN fuColorUse   // use src's RGB or PAL
// ����ֵ��
//	����ɹ�������ʵ�ʲ��������������򣬷���0
// ����������
//	��λͼ��һ�Ӹ�ʽת������һ�ָ�ʽ
// ����: 
//	
// ************************************************
#define MAX_CACHE_INDEXS   32   //������2�Ĵη� 2 ^ n
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

    //���Ŀ�귽��
	if( lpDestImage->bitmap.bmFlags & BF_DIB )
	{ // dest is dib format
		iDestDir = lpDestImage->biDir;
	}
	else
		iDestDir = 1;
	//���Դ����
	if( lpbi->bmiHeader.biHeight < 0 )
		iSrcDir = 1;
	else
		iSrcDir = -1;
	//�Ƿ����ü����� ��
	if( lprcDestClip || lprcSrcClip )
	{  // ��do clip 
		//���Դ�ü���ĺϷ���
		//�õ�Դλͼ����
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
		{	//Դλͼ������Դ�ü������ཻ
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
		//���Ŀ��ü���ĺϷ���
		if( lprcDestClip )
		{	//��Ŀ���豸����ϵת����Դ����ϵ
		    rcDestClip.left = lprcDestClip->left + xOffset; 
		    rcDestClip.top = lprcDestClip->top + yOffset;
		    rcDestClip.right = rcDestClip.left + (rcSrcClip.right-rcSrcClip.left);
            rcDestClip.bottom = rcDestClip.top + (rcSrcClip.bottom-rcSrcClip.top);
			//�õ���Դ����Ч����
		    if( IntersectRect( &rcDestClip, &rcDestClip, lprcDestClip ) == FALSE )
				return 0;
		}
		else
		{	//��Ŀ���豸����ϵת����Դ����ϵ
		    rcDestClip.left = xOffset;
		    rcDestClip.top = yOffset;
		    rcDestClip.right = rcDestClip.left + (rcSrcClip.right-rcSrcClip.left);
            rcDestClip.bottom = rcDestClip.top + (rcSrcClip.bottom-rcSrcClip.top);
		}
	}
	else
	{   //Ŀ���Դ�ĳ����ԭ������ͬ�� the dest and source's width and height and origin is same
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
	{	//Ŀ��������
		if( iSrcDir < 0 )
		{	//Դ�Ƿ��� b->u
			yDestStart = rcDestClip.bottom-1;//dwScanLineNum;//-1;
			lpbDestStart = lpDestImage->bitmap.bmBits + yDestStart * lpDestImage->bitmap.bmWidthBytes;
		    iDestWidthBytes = -lpDestImage->bitmap.bmWidthBytes;    
			yDestDir = -1; 		
		}
		else
		{	//Դ������
			yDestStart = rcDestClip.top;
			lpbDestStart = lpDestImage->bitmap.bmBits + yDestStart * lpDestImage->bitmap.bmWidthBytes;
		    iDestWidthBytes = lpDestImage->bitmap.bmWidthBytes;    
			yDestDir = 1; 
		}
	}
	else
	{   //Ŀ���Ƿ��� // b->u
		if( iSrcDir < 0 )
		{	//Դ�Ƿ��� // b->u
			yDestStart = lpDestImage->bitmap.bmHeight - rcDestClip.bottom;
			lpbDestStart = lpDestImage->bitmap.bmBits + yDestStart * lpDestImage->bitmap.bmWidthBytes;
		    iDestWidthBytes = lpDestImage->bitmap.bmWidthBytes;    
			yDestDir = 1; 
		}
		else
		{	//Դ������
			yDestStart = lpDestImage->bitmap.bmHeight - rcDestClip.top - 1;
			lpbDestStart = lpDestImage->bitmap.bmBits + yDestStart * lpDestImage->bitmap.bmWidthBytes;
		    iDestWidthBytes = -lpDestImage->bitmap.bmWidthBytes;    
			yDestDir = -1; 
		}
	}

	dwReadScanLine = dwScanLineNum;
	//׼��������ṹ
	pixelData.lpDestImage = &lpDestImage->bitmap;
    pixelData.pattern = 0;
    pixelData.rop = R2_COPYPEN;

    if( lpbi->bmiHeader.biBitCount == 1 )
    {	//Դ�ǵ�ɫλͼ��ʽ
        COLORREF * lprgb = (COLORREF *)lpbi->bmiColors;
		
		if( lpDestImage->bitmap.bmBitsPixel == 1 && 
			lprcDestClip == NULL &&
			lprcSrcClip == NULL )
		{	//Ŀ���ǵ�ɫλͼ��ʽ
			for( ; dwReadScanLine; dwReadScanLine--, lpbData += dwScanLineWidth  )
			{	//ֱ�ӿ���
				memcpy( lpbDestStart, lpbData, dwScanLineWidth );
				lpbDestStart += iDestWidthBytes;
			}
		}
		else
		{   //Ŀ���Ƿǵ�ɫλͼ��ʽ
			//�����д��ķ���
			int xStartBits = 0x80 >> ( xSrcStart & 0x7 ); // 2005-05-31�� add
			lpbData += (xSrcStart >> 3); // 2005-05-31�� add
			for( ; dwReadScanLine; dwReadScanLine--, yDestStart += yDestDir, lpbData += dwScanLineWidth  )
			{	//ÿ��
				int xe = xDestStart + xDestWidth;
				LPCBYTE lpbColor = lpbData;//2005-05-31, remove[ + (xSrcStart >> 3); ]
				int iBits = xStartBits;// 2005-05-31, remove[0x80 >> ( xSrcStart & 0x7 );]

				pixelData.y = yDestStart;
				pixelData.x = xDestStart;
				for( ; pixelData.x < xe; lpbColor++ )
				{	//ÿ��
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
    {	//Դ��2bit/pixelλͼ��ʽ
		INT16 * lpPal = (INT16 *)lpbi->bmiColors;
        RGBQUAD * lpquad = (RGBQUAD *)lpbi->bmiColors;
		UINT cIndex = 0xffffffffl;
		int iStartBits = 0xc0 >> ( ( xSrcStart & 0x3 ) << 1 );	// 2005-05-31�� add
		
		lpbData += (xSrcStart >> 2);  // 2005-05-31�� add
		//�����д��ķ���
        for( ; dwReadScanLine; dwReadScanLine--, yDestStart+=yDestDir, lpbData += dwScanLineWidth )
        {	//ÿ��
            LPCBYTE lpbColor = lpbData;//2005-05-31, remove[ + (xSrcStart >> 2);  ]
			int xe = xDestStart + xDestWidth;
			int iBits = iStartBits;// 2005-05-31, remove[0xc0 >> ( ( xSrcStart & 0x3 ) << 1 );]
            
            pixelData.y = yDestStart;//dwStartScanLine;  // dest 
			pixelData.x = xDestStart;
            for( ; pixelData.x < xe; lpbColor++ )
            {	//ÿ��   
                int iShift = 6;
                for( ; iBits && pixelData.x < xe; iBits >>= 2, pixelData.x++, iShift-= 2 )
                {
                    UINT v = (*lpbColor & iBits) >> iShift;
					//���ǰһ�ε���ɫֵ�뵱ǰ����ͬ������Ҫ
					//��ȥ�õ��豸��ص���ɫֵ��������Ҫ���¼���
					//�豸��ص���ɫֵ
					if( cIndex != v )
					{	//�õ��豸��ص���ɫֵ
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
    {	//Դ��4bit/pixelλͼ��ʽ
		INT16 * lpPal = (INT16 *)lpbi->bmiColors;
        RGBQUAD * lpquad = (RGBQUAD *)lpbi->bmiColors;
		UINT cIndex = 0xffffffffl;
		int iStartBits = 0xf0 >> ( ( xSrcStart & 0x1 ) << 2 ); 
		//�����д��ķ���
		lpbData += (xSrcStart >> 1); //// 2005-05-31�� add
        for( ; dwReadScanLine; dwReadScanLine--, yDestStart+=yDestDir, lpbData += dwScanLineWidth )
        {	//ÿ��
            LPCBYTE lpbColor = lpbData;// 2005-05-31�� remove[ + (xSrcStart >> 1);]
			int xe = xDestStart + xDestWidth;
			int iBits = iStartBits;//2005-05-31�� remove [ 0xf0 >> ( ( xSrcStart & 0x1 ) << 2 );]

            pixelData.y = yDestStart;//dwStartScanLine;
			pixelData.x = xDestStart;
            for( ; pixelData.x < xe; lpbColor++ )
            {	//ÿ��   
                int iShift = 4;
                for( ; iBits && pixelData.x < xe; iBits >>= 4, pixelData.x++, iShift -= 4 )
                {
                    UINT v = (*lpbColor & iBits) >> iShift;
					//���ǰһ�ε���ɫֵ�뵱ǰ����ͬ������Ҫ
					//��ȥ�õ��豸��ص���ɫֵ��������Ҫ���¼���
					//�豸��ص���ɫֵ

					if( cIndex != v )
					{	//�õ��豸��ص���ɫֵ
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
    {	//Դ��8bit/pixelλͼ��ʽ
		INT16 * lpPal = (INT16 *)lpbi->bmiColors;
        RGBQUAD * lpquad = (RGBQUAD *)lpbi->bmiColors;
		UINT v = 0xffffffffl;

		lpbData += xSrcStart; // 2005-05-31, add
        for( ; dwReadScanLine; dwReadScanLine--, yDestStart+=yDestDir, lpbData += dwScanLineWidth )
        {	//ÿ��
            LPCBYTE lpbColor = lpbData;
			int xe = xDestStart + xDestWidth;
            
            pixelData.y = yDestStart;//dwStartScanLine;
			pixelData.x = xDestStart;

            for( ; pixelData.x < xe; lpbColor++, pixelData.x++ )
            {	//ÿ��   
				//���ǰһ�ε���ɫֵ�뵱ǰ����ͬ������Ҫ
				//��ȥ�õ��豸��ص���ɫֵ��������Ҫ���¼���
				//�豸��ص���ɫֵ

				// ���� v == *lpbColor�� pixelData.color ��ֵ��������
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
    {	//Դ��16bit/pixelλͼ��ʽ
        DWORD dwRedMask, dwGreenMask, dwBlueMask;
		DWORD dwrgb, dwSrc = 0xffffffffl;
        
        int iRedShift, iGreenShift, iBlueShift;
		//������ɫλģ�� ��
        if( lpbi->bmiHeader.biCompression == BI_RGB )
        {	//Ĭ�ϵ�λ��ʽ = 5r-5g-5b format
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
        {	//��Ĭ�ϵ� �õ���ʽ
			//�õ���λ����ֵ
            dwRedMask = *(LPDWORD)lpbi->bmiColors;
            iRedShift = GetShiftValue(dwRedMask);
            dwGreenMask = *( (LPDWORD)lpbi->bmiColors + 1 );
            iGreenShift = GetShiftValue(dwGreenMask);
            dwBlueMask = *( (LPDWORD)lpbi->bmiColors + 2 );
            iBlueShift = GetShiftValue(dwBlueMask);
        }
		lpbData += xSrcStart << 1; // 2005-05-31, add
        for( ;dwReadScanLine; dwReadScanLine--, yDestStart+=yDestDir, lpbData += dwScanLineWidth )
        {	//ÿ��
            LPWORD lpwData = (LPWORD)lpbData;
			int xe = xDestStart + xDestWidth;

            pixelData.y = yDestStart;//dwStartScanLine;
			pixelData.x = xDestStart;
            for( ; pixelData.x < xe; pixelData.x++ )
            {	//ÿ��   
				//���ǰһ�ε���ɫֵ�뵱ǰ����ͬ������Ҫ
				//��ȥ�õ��豸��ص���ɫֵ��������Ҫ���¼���
				//�豸��ص���ɫֵ
				// ���� dwSrc == *lpwData�� pixelData.color ��ֵ��������
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
    {	//Դ��24bit/pixelλͼ��ʽ
		memset( dwCacheColor, -1, sizeof(dwCacheColor) );
		memset( dwCacheDeviceColor, -1, sizeof(dwCacheDeviceColor) );

		lpbData += (xSrcStart << 1) + xSrcStart; // 2005-05-31, add
        for( ; dwReadScanLine; dwReadScanLine--, yDestStart+=yDestDir, lpbData += dwScanLineWidth )
        {	//ÿ��
            LPCBYTE lpbColor = lpbData;
			int xe = xDestStart + xDestWidth;
			

            pixelData.y = yDestStart;//dwStartScanLine;
			pixelData.x = xDestStart;


            for( ; pixelData.x < xe; lpbColor+=3, pixelData.x++ )
            {	//ÿ��
				COLORREF rgbCurrent = RGB( *(lpbColor+2), *(lpbColor+1), *lpbColor );
				BYTE cacheIndex = *(lpbColor+2) + *(lpbColor+1) + *lpbColor;
				cacheIndex = ( cacheIndex + (cacheIndex >> 4) ) & (MAX_CACHE_INDEXS-1);

				if( rgbCurrent == dwCacheColor[cacheIndex] )
				{   //����cache
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
    {	//Դ��32bit/pixelλͼ��ʽ
		DWORD dwColor = ~( *( (LPDWORD)lpbData ) );
		lpbData += xSrcStart << 2; // 2005-05-31, add

		memset( dwCacheColor, -1, sizeof(dwCacheColor) );
		memset( dwCacheDeviceColor, -1, sizeof(dwCacheDeviceColor) );

        for( ; dwReadScanLine; dwReadScanLine--, yDestStart+=yDestDir, lpbData += dwScanLineWidth )
        {	//ÿ��
            LPBYTE lpbColor = (LPBYTE)lpbData;
			int xe = xDestStart + xDestWidth;

            pixelData.y = yDestStart;//dwStartScanLine;
			pixelData.x = xDestStart;
            for( ; pixelData.x < xe; lpbColor+=4, pixelData.x++ )
            {	//ÿ�� // the color bytes format is b + g + r , LN 2003-09-06
				BYTE cacheIndex = *(lpbColor+2) + *(lpbColor+1) + *lpbColor;				
				cacheIndex = ( cacheIndex + (cacheIndex >> 4) ) & (MAX_CACHE_INDEXS-1);

				if( *( (LPDWORD)lpbColor ) == dwCacheColor[cacheIndex] )
				{   //����cache
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
// ������static HBITMAP _HandleImageData( HANDLE hFile, BITMAPINFO *lpbi )
// ������
// 	IN hFile - λͼ�ļ����
//	IN lpbi - ����λͼ������Ϣ�Ľṹ
//	IN bShare - �Ƿ���
// ����ֵ��
//	����ɹ��������µ�λͼ�����������򣬷���NULL
// ����������
//	����λͼ�ļ��������µ�λͼ����
// ����: 
//	
// ************************************************
#define DEBUG_HANDLE_IMAGE_DATA 0
static HBITMAP _HandleImageData( HANDLE hFile, BITMAPINFO *lpbi, BOOL bShare )
{
    HBITMAP hbmp;
	int iHeight = lpbi->bmiHeader.biHeight;
	_LPBITMAPDATA lpImage;
	_LPCDISPLAYDRV lpDrv;
	//�Ƿ��� ��
	if( iHeight < 0 )
		iHeight = -iHeight;
    //����һ���ڴ�λͼ����
	if( lpbi->bmiHeader.biPlanes == 1 && lpbi->bmiHeader.biBitCount == 1 )
		hbmp = _WinGdi_CreateBitmap( lpbi->bmiHeader.biWidth, iHeight, 1, 1, 0, NULL, NULL, 0, NULL, bShare );
	   //WinGdi_CreateBitmap( lpbi->bmiHeader.biWidth, iHeight, 1, 1, 0 );
	else
		hbmp = _WinGdi_CreateBitmap( lpbi->bmiHeader.biWidth, iHeight, lpDisplayBitmap->bmPlanes, lpDisplayBitmap->bmBitsPixel, 0, NULL, NULL, 0, NULL, bShare );
	    //hbmp = WinGdi_CreateBitmap( lpbi->bmiHeader.biWidth, iHeight, lpDisplayBitmap->bmPlanes, lpDisplayBitmap->bmBitsPixel, 0 );
    
    if( hbmp )
    {	//ɨ���п��
		int iScanLineBytes = (lpbi->bmiHeader.biBitCount * lpbi->bmiHeader.biWidth + 31) / 32 * 4;  // align to dword
        DWORD dwTotalSize = iScanLineBytes * iHeight;
        LPBYTE lpbData;
        DWORD dwReadSize;
        DWORD dwReadScanLine;
        int iDir;

		lpImage = _GetHBITMAPPtr( hbmp );
		ASSERT( lpImage );
		//�õ���λͼ��ʽ֧�ֵ��豸��������
		lpDrv = GetDisplayDeviceDriver( lpImage );
		ASSERT( lpDrv );
		//�Ƿ���Ҫ̫�����ʱ�ڴ棬�����Ҫ̫�࣬�����д���
		//����һ�ζ������е�����
        if( dwTotalSize <= 1024 )
        {	//�������е�����
            lpbData = (LPBYTE)malloc( dwTotalSize );
            dwReadSize = dwTotalSize;
            dwReadScanLine = lpbi->bmiHeader.biHeight;
        }
        else
        {	//���д���
            lpbData = (LPBYTE)malloc( iScanLineBytes );
            dwReadSize = iScanLineBytes;
            dwReadScanLine = 1;
        }
		
        if( lpbData )
        {
            DWORD dwCount;
            DWORD dwStartScanLine;
			DWORD dwRealReadSize;
			//ת����ɫ��            
            _ConvertRGBQUADColorToPalColor( lpbi );

            if( (int)lpbi->bmiHeader.biHeight > 0 )
            {	//����
				dwStartScanLine = 0;
                iDir = -1;
            }
            else
            {	//����
				dwStartScanLine = 0;
                iDir = 1;
            }

            for( dwCount = 0;dwCount < dwTotalSize; dwCount += dwReadSize )
            {   //��λͼ����    
                ReadFile(hFile, lpbData, dwReadSize,&dwRealReadSize,NULL);
				
                if( dwRealReadSize != dwReadSize )
				{	// error
					WARNMSG( DEBUG_HANDLE_IMAGE_DATA, ( "_HandleImageData: error in _HandleImageData.\r\n" ) );
                    break;
				}
				//ת��
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
				//�¼�ɨ����
                dwStartScanLine += dwReadScanLine;
            }
			//�ͷ���ʱ�ڴ� ��
            free( lpbData );
        }
    }
    return hbmp;
}

// **************************************************
// ������static HBITMAP _HandleBitmapFile( HANDLE hFile, BOOL bShare )
// ������
// 	IN hFile - �ļ����
//	IN bShare - �Ƿ���
// ����ֵ��
//	����ɹ���������Ч�ķ�NULLλͼ��������򣬷���NULL
// ����������
//	�����ļ�����һ���ڴ�λͼ����
// ����: 
//	
// ************************************************

#define DEBUG_HANDLE_BITMAP_FILE 0
static HBITMAP _HandleBitmapFile( HANDLE hFile, BOOL bShare )
{
    MYBITMAPFILEHEADER bfh;
    DWORD dwReadSize;
    HBITMAP hBitmap = NULL;
    
	RETAILMSG( DEBUG_HANDLE_BITMAP_FILE, ( "_HandleBitmapFile entry.\r\n" ) );
	//���ļ�ͷ
    ReadFile( hFile, &bfh.bfType, sizeof(MYBITMAPFILEHEADER) - sizeof(WORD), &dwReadSize, NULL );
	//�Ƿ�λͼ�ļ� ��
    if( dwReadSize == (sizeof(MYBITMAPFILEHEADER) - sizeof(WORD)) && 
        *((LPBYTE)&bfh.bfType) == 'B' &&
        *( ((LPBYTE)&bfh.bfType) + 1 ) == 'M' )
    {	//��
		hBitmap = HandleBitmapData( hFile, bShare );
    }
	else
	{	//��
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
//ĳЩ�����������ж���ṹ����
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
// ������static BOOL IsValidIcoCursorData( 
//									DWORD dwFilePos, 
//                                  LPICOCURSORDESC lpDesc, 
//                                  int nImages, 
//                                  DWORD dwFileSize )

// ������
//	IN dwFilePos - ICON �� CURSOR �������ļ��е�ƫ��ֵ
//  IN lpDesc - ����ÿ��ICON �� CURSOR��Ŀ���ݵĽṹ
//  IN nImages - ICON �� CURSOR��Ŀ���ݽṹ�ĸ���
//  IN dwFileSize - �ļ��ܴ�С
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	����ļ��е�ICON �� CURSOR��Ŀ�����Ƿ���Ч
// ����: 
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
// ������static int GetNeedImageIndex(
//                             HANDLE hFile,
//                             LPICOCURSORDESC lpDesc,
//                             int nImages, 
//                             int cxDesired, 
//                             int cyDesired )
// ������
//  IN hFile - λͼ�ļ����
//  IN lpDesc - ����ÿ��ICON �� CURSOR��Ŀ���ݵĽṹ
//  IN nImages - ��Ҫ���ҵ��ļ���
//  IN cxDesired - Ҫ��Ŀ��
//  IN cyDesired - Ҫ��ĸ߶�
// ����ֵ��
//	����ɹ�������ƥ�����Ŀ�����ţ����򣬷���-1
// ����������
//	�õ�����Ҫ�����Ŀ���ļ����������/λ��
// ����: 
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
        {	//����Ҫ��
            return i;
        }        
        lpDesc++;	//��һ����Ŀ
    }
    return -1;
}

// **************************************************
// ������HANDLE IconCursorCreate(
//                int xHot, 
//				  int yHot,
//                BITMAPINFO * lpbi,
//                int uiType,
//				  int uiName )

// ������
//  IN xHot - �����ȵ�
//	IN yHot - �����ȵ�
//  IN lpbi - λͼ��Ϣ
//  IN uiType - λͼ���ͣ�
//			IMAGE_ICON - ͼ��
//			IMAGE_CURSOR - ���
//	IN uiName - ����ID
// ����ֵ��
//	����ɹ������ش�����λͼ��������򣬷���NULL
// ����������
//	����ͼ�������Դ
// ����: 
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
    {	//����
        cy = lpbi->bmiHeader.biHeight;
        iDir = -1;
    }
    else
    {	//����
        cy = -(int)lpbi->bmiHeader.biHeight;
        iDir = 1;
    }
	//������ɫλͼ
    if( lpbi->bmiHeader.biBitCount != 1 )
        hbmp = WinGdi_CreateBitmap( lpbi->bmiHeader.biWidth,  cy, lpDisplayBitmap->bmPlanes, lpDisplayBitmap->bmBitsPixel, 0 );
    else
        hbmp = WinGdi_CreateBitmap( lpbi->bmiHeader.biWidth,  cy, 1, 1, 0 );
	//�����ڰ�λģλͼ
    hbmpMono = WinGdi_CreateBitmap( lpbi->bmiHeader.biWidth, cy, 1, 1, 0 );
    
    if( hbmp && hbmpMono )
    {
        iBitCount = lpbi->bmiHeader.biBitCount;    
        cx = (int)lpbi->bmiHeader.biWidth;
        
        cbColorTable = GetColorTableNum( &lpbi->bmiHeader ) * sizeof(RGBQUAD);
        //ָ��λͼ����
        lpDIBBits = (LPBYTE)lpbi + sizeof(BITMAPINFOHEADER) + cbColorTable;
        //���뵽DWORD
		iScanLineBytes = (cx * iBitCount + 31 ) / 32 * 4;  // align to dword
        cbBits = iScanLineBytes * cy;
		//ת��ICON/CURSOR����ɫ����
        if( lpbi->bmiHeader.biBitCount != 1 )
        {
			_LPBITMAPDATA lpImage = _GetHBITMAPPtr(hbmp);
			//ת����ɫ���RQBQUAD��ʽ��PAL��ʽ
            _ConvertRGBQUADColorToPalColor( lpbi );
			//ת��λͼ���ݵ�Ŀ��
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
		{	//ת��λͼ���ݵ�Ŀ��
            _ConvertMonoBitmap( 
			                   _GetHBITMAPPtr(hbmp), 
							   dwStartScanLine, 
							   cy, 
							   iScanLineBytes, 
							   lpDIBBits, 
							   lpbi, 
							   0 );
		}
		//ָ��λģ����        
		//λģ�ǽ�������ɫ����
        lpDIBBits += cbBits;
        
        lpbi->bmiHeader.biBitCount = 1;
		//���뵽DWORD
		iScanLineBytes = (cx * 1 + 31) / 32 * 4;  //align to dword
		//ת��ICON/CURSOR��λģ����
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
		//����ICON����        
        hIcon = _CreateIconIndirect( &icon, uiName, 0 );
        if( hIcon == NULL )
        {	//ʧ�ܣ����
            DeleteObject( hbmp );
            DeleteObject( hbmpMono );
        }
    }
    else
    {	//ʧ�ܣ����
        if( hbmp )
            WinGdi_DeleteObject( hbmp );
        if( hbmp )
            WinGdi_DeleteObject( hbmpMono );
    }
    return hIcon;
}

// **************************************************
// ������static ICOCURSORDESC * AllocDescAndCheck( 
//									HANDLE hFile, 
//									int iResourceCount, 
//									DWORD dwFileSize )
// ������
//	IN hFile - �ļ����
//	IN iResourceCount - ��Ҫ�������Դ��
//	IN dwFileSize - �ļ��Ĵ�С
// ����ֵ��
//	����ɹ������ط���/����ʼ���õ�ICOCURSORDESC�ṹ�����򣬷���NULL
// ����������
//	������Ҫ�ĵ�ICOCURSORDESC�ṹ
//	���ļ������ICOCURSORDESC�ṹ����
// ����: 
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
    {	//����Ҫ��  ICOCURSORDESC �ṹ����
        if( ReadFile( hFile, lpDesc, uiDescSize, &dwReaded, NULL ) )
        {	//����Ƿ���Ч
            if( dwReaded == uiDescSize )
            {	// check valid ?
                DWORD dwFilePos = (DWORD)SetFilePointer( hFile, 0, NULL, FILE_CURRENT );
				//����Ƿ���Ч
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
// ������void FreeDesc( ICOCURSORDESC *lpDesc )
// ������
// 	IN lpDesc - ICOCURSORDESC �ṹָ��
// ����ֵ��
//	��
// ����������
//	�� AllocDescAndCheck �෴���ͷ� ICOCURSORDESC �ṹ�ڴ�
// ����: 
//	
// ************************************************

void FreeDesc( ICOCURSORDESC *lpDesc )
{
    free( lpDesc );
}

// **************************************************
// ������static HANDLE _HandleIconCursorFile( 
//								HANDLE hFile, 
//								UINT uType, 
//								int cxDesired, 
//								int cyDesired, 
//								int index, 
//								BOOL bLoadWithSize )
// ������
//	IN hFile - �ļ����
//	IN uType - ��Ҫ�� ���ͣ�Ϊ����ֵ��
//					IMAGE_CURSOR - ָ���
//					IMAGE_ICON   - ָͼ��
//	IN cxDesired - ��Ҫ�Ŀ�ȣ���bLoadWithSizeΪTRUEʱ����Ч��
//	IN cyDesired - ��Ҫ�ĸ߶ȣ���bLoadWithSizeΪTRUEʱ����Ч��
//	IN index - ���ļ���������ţ���bLoadWithSizeΪTRUEʱ����Ч��
//	IN bLoadWithSize - ˵���Ƿ���Ҫ����cxDesired��cyDesired���õ���Դ
// ����ֵ��
//	����ɹ���������Դ�ľ�������򣬷���NULL
// ����������
//	���ļ��õ�����Ҫ���ICON��CURSOR��Դ
// ����: 
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
	//�õ��ļ���С

	DEBUGMSG( DEBUG_HandleIconCursorFile, ( "_HandleIconCursorFile:  entry.\r\n" ) );

    dwFileSize = SetFilePointer( hFile, 0, NULL, FILE_END );
    SetFilePointer( hFile, 0, NULL, FILE_BEGIN );

    if( ReadFile( hFile, &hdr, ICOCURSORHDR_REAL_SIZE, &dwReaded, NULL ) )
    {   // �Ƿ���Ч check valid ?
        if( dwReaded == ICOCURSORHDR_REAL_SIZE && 
            hdr.wReserved == 0 &&
            hdr.wResourceCount < MAXIMAGES &&
            (hdr.wResourceType == 1 || hdr.wResourceType == 2)
          )
        {	//���䲢��ʼ����Ҫ�� ICOCURSORDESC �ṹ
            lpDesc = AllocDescAndCheck( hFile, hdr.wResourceCount, dwFileSize );
            if( lpDesc )
            {	//�������С�õ���Դ�������һ������Ҫ���
                if( bLoadWithSize )
                   index = GetNeedImageIndex( hFile, lpDesc, hdr.wResourceCount, cxDesired, cyDesired );
                if( index >= 0 )
                {	//����
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
// ������static HANDLE MakeIconCursor( 
//							 HANDLE hFile, 
//							 BOOL bIcon, 
//							 DWORD dwDIBOffset, 
//							 DWORD dwDIBSize, 
//							 WORD wName )
// ������
//	IN hFile - ��Դ�ļ����
//	IN bIcon - �Ƿ��Ǵ���ICON,���ΪFALSE,��ʾ��CURSOR
//	IN dwDIBOffset - DIB�������ļ��еı��� 
//	IN dwDIBSize - DIB�������ļ��еĴ�С
//	IN wName - ��Դ���ļ��е�ID
// ����ֵ��
//	����ɹ���������Ч�ķ�NULL��Դ��������򣬷���NULL
// ����������
//	���ļ��ж�����Ҫ����Դ����������Դ���󣬷��ؾ��
// ����: 
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

//�ļ��еĶ���
//����������Դ�Ľṹ
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
//����ICON��Դ��ͷ�ṹ
typedef struct _ICONHEADER
{
	WORD   wReserved;          // Currently zero 
	WORD   wType;              // 1 for icons 
	WORD   wCount;             // Number of components 
	WORD   wDumy;            // filler for DWORD alignment 
}ICONHEADER, FAR * LPICONHEADER;
//����ÿ��ICON��Դ�Ľṹ
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
//����ÿ��ICON��Դ�������ṹ
typedef struct _ICONINDEX
{	
	DWORD dwName;		//ICON��
	DWORD dwOffsetInFile;		//���ļ��еı���
}ICONINDEX, FAR * LPICONINDEX;

// **************************************************
// ������static HANDLE HandleIconCursorGroupData( 
//						HANDLE hFile, 
//						int id, 
//						LPCTSTR lpszName, 
//						LPICONINDEX lp, 
//						int iIndexCount, 
//						int cxDesired, 
//						int cyDesired )
// ������
//	IN hFile - �ļ����
//	IN id - ���ļ��е���Դ����ID
//	IN lpszName- - ��Դ��
//	IN lp - ICONINDEX �ṹָ��
//	IN iIndexCount - ICONINDEX �ṹָ����ָ���������
//	IN cxDesired - ��Ҫ�Ŀ��
//	IN cyDesired - ��Ҫ�ĸ߶�
// ����ֵ��
//	����ɹ���������Ч�ķ�NULL��Դ��������򣬷���NULL
// ����������
//	����ICON/CURSOR������
// ����: 
//	
// ************************************************

static HANDLE HandleIconCursorGroupData( HANDLE hFile, int id, LPCTSTR lpszName, LPICONINDEX lp, int iIndexCount, int cxDesired, int cyDesired )
{ 
	ICONHEADER ih;
	DWORD dwReaded;
	//��ͷ����
	ReadFile( hFile, &ih, sizeof(ih) - sizeof(WORD), &dwReaded, NULL ); 
	if( ih.wCount > 0 && iIndexCount )
	{
		ICONREC iconRec;
		int i;
		for( i = 0; i < ih.wCount; i++ )
		{	//��ÿһ�������
			ReadFile( hFile, &iconRec, sizeof(iconRec) - sizeof(WORD), &dwReaded, NULL ); 
			//�Ƿ����Ҫ��
			if( ( dwReaded == sizeof(iconRec) - sizeof(WORD) ) &&
				IS_SIZE_MATCH( iconRec.bWidth, iconRec.bHeight, cxDesired, cyDesired ) )
			{
				int n;
				//�ڸ����в�������ƥ�����Դ
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
// ������static HANDLE HandleBitmapData( HANDLE hFile, BOOL bShare )
// ������
//	IN hFile - �ļ����
//	IN bShare - �Ƿ���
// ����ֵ��
//	����ɹ���������Ч�ķ�NULL��Դ��������򣬷���NULL
// ����������
//	����λͼ�ļ�
// ����: 
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
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
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
// ������static HANDLE _LoadImageFromResFile( 
//									HANDLE hFile, 
//									DWORD dwOffset, 
//									UINT id, 
//									LPCTSTR lpszName, 
//									int cxDesired, 
//									int cyDesired,
//									BOOL bShare )
// ������
//	IN hFile - �ļ����
//	IN dwOffset - ��Դ���ļ��е�ƫ��
//	IN id - ��Ҫ����Դ���
//	IN lpszName - ��Դ����
//	IN cxDesired - ��Ҫ�Ŀ��
//	IN cyDesired - ��Ҫ�ĸ߶�
//	IN bShare - ����Դ�Ƿ�ᱻ����
// ����ֵ��
//	����ɹ������� ��Դ��������򣬷��� NULL
// ����������
//	���ļ��õ���Դ
// ����: 
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
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
//	
// ************************************************

//extern BOOL KL_GetInstanceFileName( HINSTANCE hInst, LPTSTR lpsz, UINT iDataLen );
// LN: 2003.05.08, ���ӹ���
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
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
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
		fuLoad &= ~LR_SHARED;	//Ŀǰ��֧��
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
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
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
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
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
	{  //û�з��֣����ļ��õ�		
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
			{	//�ض���ģ����
				Module_ReleaseFileHandle( hInst, hFile );
			}
			//
		}
	}

	return hRetv;
}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
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
	{   //û�з��֣����ļ��õ�
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
			{	//�ض���ģ����
				Module_ReleaseFileHandle( hInst, hFile );
			}
			//

		}
	}
	return hCursor;

}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
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
			{	//�ض���ģ����
				Module_ReleaseFileHandle( hInst, hFile );
			}
			//
		}
	}
	return hBitmap;

}

// **************************************************
// ������
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
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
		{	//��ϵͳ������Դ
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
	{   //û�з��֣����ļ��õ�
		if( fuLoad & LR_LOADFROMFILE )
		{
			hFile = GetResFileHandle( NULL, lpszName, uType, &dwOffset, &iFileType, &bFreeFile );
			lpszName = NULL; //������Դ��Ϊnull //2003-07-28
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
			{	//�ض���ģ����
				Module_ReleaseFileHandle( hInst, hFile );
			}
			//
		//RETAILMSG( 1, ( "k2.\r\n" ) );
			
		}
	}
	return hRetv;

}

// **************************************************
// ������static HANDLE _LoadStringFromResFile( HANDLE hFile, LPCTSTR lpszName, LPTSTR lpszBuf, UINT uBufLen, DWORD idLang )
// ������
//	IN hFile - �ļ����
//	IN lpszName - ��Դ����
//	OUT lpszBuf - ���� ����
//	IN uBufLen - lpszBuf ���ڴ��С
//	IN idLang - ����id
//	IN bShare - ����Դ�Ƿ�ᱻ����
// ����ֵ��
//	����ɹ������� ��Դ��������򣬷��� NULL
// ����������
//	���ļ��õ���Դ
// ����: 
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
			{	//�ض���ģ����
				Module_ReleaseFileHandle( hInstance, hFile );
			}
		}
	}
	return retv;
}
