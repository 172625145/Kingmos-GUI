/******************************************************
Copyright(c) ��Ȩ���У�1998-2005΢�߼�����������Ȩ����
******************************************************/


/*****************************************************
�ļ�˵����ͼ���豸����ģ��
�汾�ţ�3.0.0
����ʱ�ڣ�1998
���ߣ�����
�޸ļ�¼��
	2005-10-14, ������API �ĳ��ڣ����أ��ŵ����λ�ã����������� LeaveDCPtr
	 2005-09-15, �޸�ˢ�ӱ��ƾ��Σ� lprcMask
	 2005-08-11 ΪTrueType ����������޸�
	 2005-08-11 ���� AlphaBlend
	 2005-06-08 add WindowFromDC
	 2005-06-31, WinGdi_SetDIBitsToDevice ����ֵ�д�ʼ����0��
	 2004-09-15��DrawText ���»��� ����������
     2004-05-21, ���� StretchBlt
	 2004-05-10: ���� TransparentBlt
     2003-09-24: DoExtSelectClipRgn û�ж�expose����������
     2003-09-13, SelectClipRgn( hdc, NULL ) mean valid the hdc default display surface
     2003-09-13, TA_CENTER aligned horizontally not vertizontally
     2003-09-06: LN, ��BRUSH & PEN�ṹ���� ->clrRef ��Ա
     2003-07-22, SelectClipRgn( hdc, hrgn ), �� hrgn==NULLʱ����� region
     2003-07-22: DeleteDC( ... ) ���� ɾ�� hBitmap, hBrush, hPen, ...
     2003-07-14: ����Arc ���� �� GDCDATA-> arcMode ��Ա
     2003-06-06: _GDCDATA  ���� OBJLIST   obj,ȡ��objType��ulOwner
     2003-05-14 : CreateBitmap Ϊ����Heap����Ƭ�� ��bitmap���������û����С ���� ĳ��Сʱ�� ��VirtualAlloc
     2003-05-06 : 
	    1.LN ��SetBkMode( TRANSPARENT ) ���� ����Rectangle 
	       (��WHITE_BRUSH)�д���
		2.���Ӷ� FillRect( hdc, (COLOR_WINDOW+1) )��֧��

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
// ������BOOL _InitialGdi( void )
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	��ʼ��GDI	
// ����: 
//	��GWMEϵͳ��ʼ��ʱ���ص��øú���
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
	hFont = OEM_GetStockObject(SYSTEM_FONT);//ϵͳĬ�ϵ�����
	lpFont = _GetHFONTPtr( hFont );
	cwEllipsis = lpFont->lpDriver->lpWordWidth( lpFont->handle, ' ' );
	//��ʼ���ź���
	hSempore = CreateSemaphore( NULL, _MAX_GDIOBJECT_NUM, _MAX_GDIOBJECT_NUM, NULL ); 
	return TRUE;
}

// **************************************************
// ������void _DeInitialGdi( void )
// ������
//    ��
// ����ֵ��
//	��
// ����������
//	�ر�GDI	
// ����: 
//	��_InitialGdi��Ӧ���ú����ͷ��ѷ������Դ
// ************************************************

void _DeInitialGdi( void )
{

	CloseHandle( hSempore );
	DeleteCriticalSection( &csDCList );
}

// **************************************************
// ������_LPCDISPLAYDRV GetDisplayDeviceDriver( _LPBITMAPDATA lpBitmap )
// ������
// 	IN lpBitmap - _BITMAPDATA�ṹָ��
// ����ֵ��
//	����ɹ���������ʾ��������ӿڣ����򣬷���NULL
// ����������
//	�õ�һ��������ʾ���ͼ����������ӿ�
// ����: 
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
// ������static VOID _CheckGDCData( void )
// ������
// 	�� 
// ����ֵ��
//	 ��
// ����������
//	���DC
// ����: 
//	
// ************************************************

static VOID _CheckGDCData( void )
{
	int i;
	_LPGDCDATA p = NULL;

	// ����һ��ϵͳDC��Դ
	p = lpSysDC;
    for( i = 0; i < _MAX_GDIOBJECT_NUM; i++, p++ )
	{
		if( p->hwnd )
		{
			_LPWINDATA lpws;
		    //�ɴ��ھ���õ���ָ�����
			lpws = _GetHWNDPtr( p->hwnd );
			if( lpws )
			{
				DWORD dwOldPerm = SetProcPermissions( ~0 );  //�õ��Խ��̵Ĵ�ȡȨ��
				RETAILMSG( 1, ( "_CheckGDCData: win=%s,class=%s,flags=0x%x.\r\n", lpws->lpWinText, lpws->lpClass->wc.lpszClassName, p->uiFlags ) );
				SetProcPermissions( dwOldPerm );  //�õ��Խ��̵Ĵ�ȡȨ��
			}
		}
		else
		{
			RETAILMSG( 1, ( "_CheckGDCData: null window,objType=0x%x,flags=0x%x.\r\n", p->obj.objType, p->uiFlags ) );
		}
	}
}

// **************************************************
// ������static _LPGDCDATA _AllocGDCData( void )
// ������
// 	�� 
// ����ֵ��
//	 ����ɹ�������_GDCDATA�ṹָ�룻���򣬷���NULL
// ����������
//	��ϵͳ������DC�����һ��DC���ݽṹ
// ����: 
//	
// ************************************************
#define DEBUG_AllocGDCData 0
static _LPGDCDATA _AllocGDCData( void )
{
    int i, count;
	_LPGDCDATA p = NULL;

	// ����һ��ϵͳDC��Դ�ź�����������벻������ѭ������20��
	// ��Ӧ���ø��õİ취ȥ���ù��ܡ�������
	count = 0;  
	while( count < 20 )
	{
		if( WaitForSingleObject( hSempore, 100 ) == WAIT_OBJECT_0 )
			break;
		WARNMSG( DEBUG_AllocGDCData, (TEXT("_AllocGDCData:try get dc(%d)\r\n"), count ) );
		count++;
	}

	if( count == 20 )
	{	//û�еõ���Դ��ͨ����ϵͳ��ʲô����
		WARNMSG( DEBUG_AllocGDCData, (TEXT("_AllocGDCData:no enough DC solt, thread=0x%x\r\n"), GetCurrentThreadId() ) );
		_CheckGDCData();
		return NULL;
	}

	// ����һ��ϵͳDC��Դ
	count = 20;	//��20��
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
	
	// ��Ӧ�õ�����
	ASSERT( 0 );
	ERRORMSG( DEBUG_AllocGDCData, (TEXT("error in _AllocGDCData: undef error!.\r\n") ) );	

	return NULL;
}

// **************************************************
// ������int UpdateDCState( UINT uiState )
// ������
// 	IN uiState - 
// ����ֵ��
//	����1��
// ����������
//	����DC״̬
// ����: 
//	�����ڵ�Z�򡢴�С�ı䲢�Ҵ��ڵ�ǰ������ʹ�õ�DCʱ����DC��Ӧ��
//    ���ڵ����ݣ���С���ü�����Ҳ��֮�ı䡣������Ǹ���DCȥ���¸���
//	�������
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
// ������static BOOL CALLBACK DelMemDCObj( LPOBJLIST lpObj, LPARAM lParam )
// ������
// 	IN lpObj - _GDCDATA����
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	ɾ��һ���ڴ�DC����
// ����: 
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
// ������BOOL WINAPI WinGdi_DeleteDC( HDC hdc )
// ������
// 	IN hdc - DC���
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	ɾ���豸�����㲻��ɾ����GetDC�õ���hdc, ������ReleaseDCȥ�ͷ�GetDC�õ���hdc
// ����: 
//	ϵͳAPI
// ************************************************

BOOL WINAPI WinGdi_DeleteDC( HDC hdc )
{
    _LPGDCDATA lpdc = _GetHDCPtr( hdc );

    if( lpdc && lpdc->obj.objType == OBJ_MEMDC )  // �Ϸ���
    {
	    EnterCriticalSection( &csDCList );  //���뻥���
	    ObjList_Remove( &lpDCObjList, &lpdc->obj );  // �������Ƴ�
	    LeaveCriticalSection( &csDCList );  //�뿪�����

		return DelMemDCObj( &lpdc->obj, 0 );//�ͷ�
    }
    else
    {
    	WARNMSG( 1, ( "error at WinGdi_DeleteDC: invalid hdc(0x%x).\r\n", hdc ) );
        return FALSE;
    }
}

// **************************************************
// ������static BOOL DelSysDC( _LPGDCDATA lpdc )
// ������
// 	IN lpdc - _GDCDATA�ṹָ��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	ɾ��DC,�����DC��ָ��BeginPaint �� GetDC �� GetDCEx�õ���
// ����: 
//	
// ************************************************

static BOOL DelSysDC( _LPGDCDATA lpdc )
{
	WinGdi_DeleteObject( lpdc->hrgn );

	lpdc->uiState = 0; 
	lpdc->obj.ulOwner = 0;
	Interlock_Exchange( (LPLONG)&lpdc->obj.objType, OBJ_NULL );
	ReleaseSemaphore( hSempore, 1, NULL );  // �ͷ�һ��DC��Դ���ź�����
	return TRUE;
}

// **************************************************
// ������BOOL ReleasePaintDC( HWND hWnd, HDC hdc )
// ������
//	IN hWnd - ���ھ��
//	IN hdc - DC���
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�ͷ�һ�����ڴ��ڵ�DC
// ����: 
//	
// ************************************************

// the option export for win.c
BOOL ReleasePaintDC( HWND hWnd, HDC hdc )
{
    _LPGDCDATA lpdc = _GetHDCPtr( hdc );

    if( lpdc )
    {	// ���Ը�DC�Ƿ���ô������
		if( lpdc->obj.objType == OBJ_DC )
		{
			if( (LONG)hWnd == Interlock_TestExchange( (LONG*)&lpdc->hwnd, (LONG)hWnd, NULL ) )
			{	//�ǣ��ͷ���
				return DelSysDC( lpdc );
			}
			else
			{
				WARNMSG( 1, ( "error at ReleasePaintDC: the hdc(0x%x) did'nt alloc by the hWnd(0x%x).\r\n", hdc, hWnd ) );
			}
		}
		else if( lpdc->obj.objType == OBJ_MEMDC )
		{	// �ض����� WS_EX_LAYERED ���ڷ����
			ufNeedFlushGDICount++;
			return ReleaseLayerWindowDC( hWnd, hdc );
		}
    }
    return FALSE;
}

// **************************************************
// ������HDC GetPaintDC( HWND hwnd, LPCRECT lprect, HRGN hrgnClip, UINT uiFlags )
// ������
// 	IN hwnd - ���ھ��
// 	IN lprect - ���ھ��Σ��豸���꣩
//	 IN hrgnClip - �ü�����
// 	IN uiFlags - ��־
// ����ֵ��
//	����ɹ������ط����DC��������򣬷���NULL
// ����������
//	��ϵͳ����һ��DC���ڻ�ͼ
// ����: 
//	
// ************************************************

HDC GetPaintDC( HWND hwnd, LPCRECT lprect, HRGN hrgnClip, UINT uiFlags )
{
    _LPGDCDATA lpdc = _AllocGDCData();// �õ�һ��DC�ṹ

    if( lpdc )
    {
		
        lpdc->hrgn = hrgnClip;
        lpdc->hPen = OEM_GetStockObject(BLACK_PEN);//ϵͳĬ�ϵ�PEN
		lpdc->penAttrib = _GetHPENPtr( lpdc->hPen )->penAttrib;

        lpdc->hBrush = OEM_GetStockObject(WHITE_BRUSH);//ϵͳĬ��ˢ��
		lpdc->brushAttrib = _GetHBRUSHPtr( lpdc->hBrush )->brushAttrib;

        lpdc->hFont = OEM_GetStockObject(SYSTEM_FONT);//ϵͳĬ�ϵ�����
		// 2004-02-16
        lpdc->hBitmap = (HBITMAP)hbmpCurrentFrameBuffer;//hbmpDisplayBitmap;//lpDisplayBitmap;//ϵͳĬ��λͼ
		//
		lpdc->lpDispDrv = lpDrvDisplayDefault;//ϵͳĬ��ͼ����������

        lpdc->deviceOrg.x = lprect->left;//�������豸�����Ͻ�x����
        lpdc->deviceOrg.y = lprect->top;//�������豸�����Ͻ�y����
        lpdc->deviceExt.cx = lprect->right - lprect->left;//���ڿ��
        lpdc->deviceExt.cy = lprect->bottom - lprect->top;//���ڸ߶�
        lpdc->stretchMode = 0;//����ģʽ
        lpdc->mapMode = MM_TEXT;//ӳ��ģʽMM_TEXT
        lpdc->rop = R2_COPYPEN;//�ʲ���ģʽ
		lpdc->arcMode = AD_CLOCKWISE;//˳ʱ�뻭��, AD_COUNTERCLOCKWISE;
        lpdc->backMode = OPAQUE;//��͸������
        lpdc->backColor = (DWORD)OEM_GetStockObject( SYS_STOCK_WHITE );//Ĭ�ϵı���ɫ
        lpdc->textColor = (DWORD)OEM_GetStockObject( SYS_STOCK_BLACK );//Ĭ�ϵ��ı�ɫ
        lpdc->textAlign = TA_TOP | TA_LEFT;//�ı�����ģʽ
        lpdc->position.x = lpdc->position.y = 0;//��ǰ��Ĭ�ϻ�ͼ���
        lpdc->windowOrg = lpdc->viewportOrg = lpdc->position;//����ԭ�㣬����ԭ��

		lpdc->uiState = 0; // ״̬
		lpdc->uiFlags = uiFlags;//��־
        lpdc->hwnd = hwnd;//��������
		lpdc->lpdwPal = 0;  //Ĭ�ϵĵ�ɫ��
		lpdc->wPalNumber = 0;  //Ĭ�ϵĵ�ɫ��
		lpdc->wPalFormat = 0;  //Ĭ�ϵĵ�ɫ��
		lpdc->ptBrushOrg.x = lprect->left;
		lpdc->ptBrushOrg.y = lprect->top;

		return (HDC)PTR_TO_HANDLE( lpdc ); // ��DCָ��ת��Ϊ���
    }

	return NULL;

}

// **************************************************
// ������BOOL DeleteDCObject( HANDLE hOwner )
// ������
// 	IN hOwner - ӵ���߾��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	���ӵ���ߵ�����֮ǰ�����û���ͷŵ�DC��ͨ����һ�������˳�ʱ�����øù���ȥ���
//	����û���ͷŵ�DC
// ����: 
//	
// ************************************************

BOOL DeleteDCObject( HANDLE hOwner )
{
    int i;
	_LPGDCDATA lpdc;

    EnterCriticalSection( &csDCList );//���뻥���
	//������е�����ӵ���߶���
    ObjList_Delete( &lpDCObjList, (ULONG)hOwner, DelMemDCObj, 0 );

    LeaveCriticalSection( &csDCList );//�뿪�����

	// release sysdc
	lpdc = lpSysDC;

    for( i = 0; i < _MAX_GDIOBJECT_NUM; i++, lpdc++ )
    {		
        if( lpdc->obj.objType != OBJ_NULL &&
			lpdc->obj.ulOwner == (ULONG)hOwner )  //����ӵ���� ��
        {   //��
			InterlockedExchange( (LONG*)&lpdc->hwnd, NULL );
			DelSysDC( lpdc );
        }
    }
	return 1;
}

// **************************************************
// ������HDC WINAPI WinGdi_CreateCompatibleDC(HDC hdc)
// ������
// 	IN hdc - DC���
// ����ֵ��
//	����ɹ�������DC��������򣬷���NULL
// ����������
//	����������hdc���ڴ�DC
// ����: 
//	ϵͳAPI
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

    lpdc = (_LPGDCDATA)BlockHeap_Alloc( hgwmeBlockHeap, BLOCKHEAP_ZERO_MEMORY, sizeof(_GDCDATA) ); // ����DC�ṹ
//    lpdc = (_LPGDCDATA)BLK_Alloc( 0, sizeof(_GDCDATA) ); // ����DC�ṹ

    if( lpdc )  // ����ɹ���
    {	//�ǣ���ʼ��DCΪ1x1�ĺڰ�λͼ��ʽ
		// hrgn ������ OEM_GetStockObject, ��Ϊ��ѡ��bitmap����Ҫ�ı���ü����εĴ�С
		lpdc->lpDispDrv = &_drvDisplay1BPP;

		lpdc->lpdwPal = 0;  //Ĭ�ϵĵ�ɫ��
		lpdc->wPalNumber = 0;  //Ĭ�ϵĵ�ɫ��
		lpdc->wPalFormat = 0;  //Ĭ�ϵĵ�ɫ��

        lpdc->hrgn = WinRgn_CreateRect( 0, 0, 1, 1 );//OEM_GetStockObject(SYS_STOCK_RGN);
		//��ʼ��Ϊ�ڱ�
        lpdc->hPen = OEM_GetStockObject(BLACK_PEN);
		//�����豸���������ݽṹ
		lpdc->penAttrib = _GetHPENPtr( lpdc->hPen )->penAttrib;
		//�����豸������ɫ
		lpdc->penAttrib.color = lpdc->lpDispDrv->lpRealizeColor(lpdc->penAttrib.clrRef, lpdc->lpdwPal, lpdc->wPalNumber, lpdc->wPalFormat ); 
		//�����豸����ˢ�����ݽṹ
        lpdc->hBrush = OEM_GetStockObject(WHITE_BRUSH);
		lpdc->brushAttrib = _GetHBRUSHPtr( lpdc->hBrush )->brushAttrib;
		//�����豸������ɫ
		lpdc->brushAttrib.color = lpdc->lpDispDrv->lpRealizeColor(lpdc->brushAttrib.clrRef, lpdc->lpdwPal, lpdc->wPalNumber, lpdc->wPalFormat); 
		//Ĭ������
        lpdc->hFont = OEM_GetStockObject(SYSTEM_FONT);
		//Ĭ����ʾ��
		lpdc->hBitmap = OEM_GetStockObject(SYS_STOCK_BITMAP1x1);
		ASSERT( lpdc->hBitmap );		
		//�豸ԭ��
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
	{	//����DC��������
		EnterCriticalSection( &csDCList );
		ObjList_Init( &lpDCObjList, &lpdc->obj, OBJ_MEMDC, (ULONG)GetCallerProcess() );
		LeaveCriticalSection( &csDCList );

		return (HDC)PTR_TO_HANDLE( lpdc ); // ת��Ϊ���
	}

	return NULL;
}

// **************************************************
// ������HBITMAP WINAPI WinGdi_CreateCompatibleBitmap(HDC hdc, int iWidth, int iHeight)
// ������
//    IN hdc - �豸DC
//	IN iWidth - λͼ���
//	IN iHeight - λͼ�߶�
// ����ֵ��
//	����ɹ������ط�NULLλͼ��������򣬷���NULL
// ����������
//	��������DC��λͼ
// ����: 
//	ϵͳAPI
// ************************************************

HBITMAP WINAPI WinGdi_CreateCompatibleBitmap(HDC hdc, int iWidth, int iHeight)
{
    if( hdc )
	{
		_LPGDCDATA lpdc = _GetHDCPtr( hdc );
		if( lpdc )
		{	//��Ч
			_LPBITMAPDATA lpImage = _GetHBITMAPPtr( lpdc->hBitmap );
			return lpImage ? WinGdi_CreateBitmap( iWidth, iHeight, lpImage->bmPlanes, lpImage->bmBitsPixel, 0 ) : NULL;
		}
	}
	else
	{   // ����ĻDC, use screen dc
		return WinGdi_CreateBitmap( iWidth, iHeight, lpDisplayBitmap->bmPlanes, lpDisplayBitmap->bmBitsPixel, 0 );
	}
    return NULL;
}


// **************************************************
// ������_LPGDCDATA _GetSafeDrawPtr( HDC hdc )
// ������
// 	IN hdc - DC��� 
// ����ֵ��
//	����ɹ�������_LPGDCDATAָ�룻���򣬷���NULL
// ����������
//	�õ���ȫ��DC.
//	1.���DC�Ƿ�Ϸ�; 2.���DC��״̬�Ƿ�ı䣬����ǣ�������
// ����: 
//	
// ************************************************

extern int _GetRgnInfo(HRGN hrgn);
#define DEBUG_GETSAFEDRAWPTR 0
_LPGDCDATA _GetSafeDrawPtr( HDC hdc )
{	// �ɾ���õ������ָ��
	_LPGDCDATA lpdc = (_LPGDCDATA)HANDLE_TO_PTR( hdc );
	if( lpdc )
	{	
		if( ( (WORD)GET_OBJ_TYPE( lpdc) == OBJ_MEMDC || 
			  (WORD)GET_OBJ_TYPE( lpdc ) == OBJ_DC ) )
		{	// ������Ч
			if( lpdc->hwnd )
			{	// ��DC �봰�ڹ����������ʱ��������ı䣬��Ҫͬʱ����DC
				int iUpdate = 0;
				int iRgn;
				while( lpdc->uiState &&  
					   Interlock_TestExchange( (LPLONG)&lpdc->uiState, 1, 0 ) )
				{	//�ı�
					_LPWINDATA lpws;
					RECT rect;
					HRGN hRgn;
					UINT uiFlags = lpdc->uiFlags;
					
					iUpdate++;  // use to test

					iRgn = SIMPLEREGION;
				    //�ɴ��ھ���õ���ָ�����
					lpws = _GetHWNDPtr( lpdc->hwnd );
					//�õ�DC����ʾ��Ļ����
					if( uiFlags & DCX_WINDOW )
					{	//����DC
						rect = lpws->rectWindow;
					}
					else
					{	//�ͻ�DC
						rect = lpws->rectClient;
						OffsetRect( &rect, lpws->rectWindow.left, lpws->rectWindow.top );
					}
					//�豸ԭ��
					lpdc->deviceOrg.x = rect.left;
					lpdc->deviceOrg.y = rect.top;
					lpdc->deviceExt.cx = rect.right - rect.left;
					lpdc->deviceExt.cy = rect.bottom - rect.top;				
					
//					LockWindow();  ////2003.1.26
					//�õ����ڵı�¶������
				    iRgn = GetExposeRgn( lpdc->hwnd, uiFlags, &hRgn );
					if( iRgn == NULLREGION ||
						iRgn == ERROR )
					{	//��¶������Ϊ�ջ���һ������
						WinRgn_SetRect( lpdc->hrgn, 0, 0, 0, 0 );
					}
					else
					{
						if( ( uiFlags & (DCX_VALIDATE | DCX_INTERSECTUPDATE) ) == (DCX_VALIDATE | DCX_INTERSECTUPDATE) )
						{   // ��ǰDC �û�����BeginPaint���صģ���Ҫ��ϵͳ�ػ������������ Ϊa dc from beginpaint
							HRGN hClip;
							//��סϵͳ�ػ�����
							__LockRepaintRgn( &hClip );  // 2003.1.26
							iRgn = WinRgn_Combine( lpdc->hrgn, lpdc->hrgn, hClip, RGN_OR );
							__UnlockRepaintRgn();   //2003.1.26
							if( !( iRgn == NULLREGION ||
								   iRgn == ERROR ) )
								iRgn = WinRgn_Combine( lpdc->hrgn, lpdc->hrgn, hRgn, RGN_AND );
							DEBUGMSG( DEBUG_GETSAFEDRAWPTR, ( "update dc from BeginPaint,%d,class=%s.\r\n", iUpdate, lpws->lpClass->wc.lpszClassName ) );
						}
						else
						{   // ���µĿ������򿽱���DC now copy hRgn to dc
							WinRgn_Combine( lpdc->hrgn, hRgn, NULL, RGN_COPY );
						}
					}
					//ɾ����ʱ ����
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
// ������_LPGDCDATA _GetHDCPtr( HDC hdc )
// ������
// 	IN hdc - DC ���
// ����ֵ��
//	����ɹ�������_LPGDCDATAָ�룻���򣬷���NULL
// ����������
//	�õ���ȫ��DC.
//	1.���DC�Ƿ�Ϸ�; 
// ����: 
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
// ������VOID _LeaveDCPtr( _LPGDCDATA lpgdc, DWORD dwFlags )
// ������
// 	IN lpgdc - ֮ǰ���� GetSafeDrawPtr �� GetHDCPtr ���ص�ָ��
//	IN dwFlags - DCF_WRITE, DCF_READ ...
// ����ֵ��
//	��
// ����������
//	������Ҫhdcʱ�� ��ĳЩ����...
// ����: 
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
// ������DWORD _Gdi_UnrealizeColor( DWORD dwRealizeColor, DWORD * lpPal, UINT uiPalNum, UINT uiSrcColorType )
// ������
// 	IN dwRealizeColor - �豸��ص���ɫֵ
//	IN uiSrcColorType - ��ɫ����
//	IN lpPal - ɫ��ָ��
//	IN uiPalNum - ɫ����Ŀ��
// ����ֵ��
//	����ɹ��������豸�޹ص���ɫֵ�����򣬷���0
// ����������
//	���豸��ص���ɫֵת��Ϊ�豸�޹ص���ɫֵ
// ����: 
//	
// ************************************************

DWORD _Gdi_UnrealizeColor( DWORD dwRealizeColor, LPCDWORD lpPal, UINT uiPalNum, UINT uiSrcColorType )
{
	DWORD dwRetv;

	if( uiSrcColorType == PAL_INDEX )
		dwRetv = lpPal[dwRealizeColor];	// ��ɫ����Ϊ����ɫ�������ֵ
	else if( uiSrcColorType == PAL_BITFIELD )
	{	//��ɫ����bitλ��ϣ� lpPal�ﱣ�������� r g b �� maskֵ
		int leftShift, rightShift;
		DWORD dwColorMask;

		dwRetv = 0;
		for( rightShift = 24; rightShift >= 0 && uiPalNum; rightShift -= 8, uiPalNum--, lpPal++ )
		{
			dwColorMask = *lpPal;
			//�õ� ��Ҫ��λ��ֵ
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
// ������static DWORD _Gdi_RealizeColor( COLORREF clRgbColor, DWORD * lpPal, UINT uiPalNum, UINT uiDestColorType )
// ������
// 	IN clRgbColor - �豸�޹ص���ɫֵ
//	IN uiDestColorType - ��ɫ����
//	IN lpPal - ɫ��ָ��
//	IN uiPalNum - ɫ����Ŀ��
// ����ֵ��
//	����ɹ��������豸��ص���ɫֵ�����򣬷���0
// ����������
//	���豸�޹ص���ɫֵת��Ϊ�豸��ص���ɫֵ
// ����: 
//	
// ************************************************

// return dest format

DWORD _Gdi_RealizeColor( COLORREF clRgbColor, LPCDWORD lpPal, UINT uiPalNum, UINT uiDestColorType )
{
	DWORD dwRetv;

	if( uiDestColorType == PAL_INDEX )
	{	// ��ɫ����Ϊ����ɫ�������ֵ
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
			{  //�õ�����
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
			{	//�õ���̾���
			    minDiff = curDiff;
			    dwRetv = uiPalCount;
			}
		}
		dwRetv = uiPalNum - dwRetv;
	}
	else if( uiDestColorType == PAL_BITFIELD )
	{	//��ɫ����bitλ��ϣ� lpPal�ﱣ�������� r g b �� maskֵ
		int leftShift, rightShift;
		DWORD dwColorMask;

		dwRetv = 0;
		for( leftShift = 24; leftShift >= 0 && uiPalNum; leftShift -= 8, uiPalNum--, lpPal++ )
		{
			dwColorMask = *lpPal;
			//�õ� ��Ҫ��λ��ֵ
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
// ������BOOL PixelTransfer( 
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

// ������
//	IN lpdcDest - Ŀ��DC����  //lpDestDispDrv - Ŀ����������ӿ�
//	IN lpPixDest - Ŀ���ṹָ��
//	IN lprcDest - Ŀ�����
//	IN lpdcSrc - ԴDC����  //lpSrcDispDrv - Դ��������ӿ�
//	IN lpPixSrc - Դ��ṹָ��
//	IN lprcSrc - Դ����
//	IN lpPixMask - ���ε㣨����������λ���͹��ܣ�   // if bltmask , it is valid
//	IN uiMaskRopSrc - ��Դ�����β���������������λ���͹��ܣ�       // if bltmask , it is valid
//	IN uiMaskRopDest - ��Ŀ�����β��� ������������λ���͹��ܣ�      // if bltmask , it is valid
//	IN clrTransparent - ͸����ɫ������͸�����䣨���Ϊ-1����͸����	// if == -1 , not transparent option
//						���� (clTransparent & 0x80000000) != 0, 
//						��˵�� clTransparent �Ǳ�ʾһ����Դλͼ���豸��ص���ɫֵ��������һ��RGBֵ
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	��������ͬ��λͼ��ʽ֮�䴫��λͼ
// ����: 
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
	UINT clrRealTransparent = -1;	//�豸��ص���ɫֵ
	
	if( bmType )
	{	//ԴλͼΪDIB��ʽ
		lpPal = (DWORD*)&lpSrcDIB->palEntry[0];
		uiPalNum = lpSrcDIB->biClrUsed;
		yDir = lpSrcDIB->biDir;
		//��ɫֵΪָ���ɫ�������ֵ
		if( lpSrcDIB->biCompression == BI_RGB )
		    bmType = PAL_INDEX;
		else if( lpSrcDIB->biCompression == BI_BITFIELDS )
			bmType = PAL_BITFIELD;  //��ɫֵΪλͼ��
		else
			return FALSE;
	}
	//Ŀ���Ⱥ͸߶�
	width = lprcDest->right - lprcDest->left;
	height = lprcDest->bottom - lprcDest->top;

	//RETAILMSG( 1, ( "PixelTransfer.\r\n" ) );
	//Դ��ʼλ����λͼ�е�ƫ��
	xSrcOffset = lprcSrc->left;
	ySrcOffset = lprcSrc->top;
	//Ŀ����ʼλ����λͼ�е�ƫ��
	xDestOffset = lprcDest->left;
	yDestOffset = lprcDest->top;
	if( clrTransparent != -1 )
	{	//��Ҫ͸������
		if( clrTransparent & TB_DEV_COLOR )
		{	//clrTransparent ���豸��ص�ֵ
			clrRealTransparent = clrTransparent & (~TB_DEV_COLOR);
		}
		else
		{	// clrTransparent��RGBֵ������ת���豸��ص�ֵ
			if( bmType && uiPalNum )
			{
				clrRealTransparent = _Gdi_RealizeColor( clrTransparent, lpPal, uiPalNum, bmType );
			}
			else  //get default 
				clrRealTransparent = lpSrcDispDrv->lpRealizeColor( clrTransparent, NULL, 0, 0 );
		}
	}
		
	for( y = 0; y < height; y++ )
	{	// ÿһ��
		for( x = 0; x < width; x++ )
		{	// ÿһ��
			//2004.07-01 - remove, ���ж�
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
			//�õ�Դ���豸�����ɫֵ
			clrCur = lpSrcDispDrv->lpGetPixel( lpPixSrc );
			if( clrCur != clrRealTransparent )
			{	//�����͸��ɫ����֮
				if( clrCur != clrPrev )
				{	//��֮ǰ����ɫֵ��ͬ���������ͬ������ת��Ϊ
					//�豸�޹ص�ֵ
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
				{	//�õ�Ŀ����ص�ֵ
					lpPixDest->color = lpDestDispDrv->lpRealizeColor( clrRgb, lpdcDest->lpdwPal, lpdcDest->wPalNumber, lpdcDest->wPalFormat );
				}
				//�����Ҫ���β������õ���Ӧx,y���������λ
				if( lpPixMask )
				{
					lpPixMask->x = lpPixSrc->x;
					lpPixMask->y = lpPixSrc->y;
					if( _drvDisplay1BPP.lpGetPixel( lpPixMask ) )
						lpPixMask->rop = uiMaskRopSrc;
					else
						lpPixMask->rop = uiMaskRopDest;
				}
				//д�㵽Ŀ��λͼ
				lpDestDispDrv->lpPutPixel( lpPixDest );
				//���浱ǰ����ɫֵ
				clrPrev = clrCur;
			}
		}
	}
	return TRUE;
}


// **************************************************
// ������BOOL WINAPI WinGdi_BitBlt(
//							HDC hdcDest - Ŀ��DC
//							int xDest - Ŀ��x����
//							int yDest - Ŀ��x����
//							int width, 
//							int height,
//							HDC hdcSrc, 
//							int xSrc, 
//							int ySrc, 
//							DWORD dwRop )
// ������
//	IN hdcDest - Ŀ��DC
//	IN xDest - Ŀ��������Ͻ�X����
//	IN yDest - Ŀ��������Ͻ�Y����
//	IN width - Ŀ����ο��
//	IN height - Ŀ����θ߶�
//	IN hdcSrc - ԴDC
//	IN xSrc - Դ�������Ͻ�X����
//	IN ySrc - Դ�������Ͻ�Y����
//	IN dwRop - ��դ������,������
//			SRCCOPY-����Դ���ε�Ŀ�����
//			SRCAND--Դ������Ŀ��������������(AND)
//			SRCINVERT-Դ������Ŀ���������������(XOR)
//			SRCPAINT-Դ������Ŀ��������������(OR)
//			PATCOPY-����ģ�嵽Ŀ�����
//			BLACKNESS-�ú�ɫ���Ŀ�����
//			WHITENESS-�ð�ɫ���Ŀ�����
//			PATINVERT-ģ����ɫ��Ŀ����ɫ���������(XOR)
//			DSTINVERT-��תĿ����ɫ
//			NOTSRCCOPY-������ת��Դ��ɫ��Ŀ����� 
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	��ԴDC��Ŀ��DC����λͼ����
// ����: 
//	ϵͳAPI
// ************************************************
						   
BOOL WINAPI WinGdi_BitBlt(HDC hdcDest, int xDest, int yDest, int width, int height,
				HDC hdcSrc, int xSrc, int ySrc, DWORD dwRop )
{
    _LPGDCDATA lpdcDest = _GetSafeDrawPtr( hdcDest );//�õ�Ŀ��DC����ָ��
	_LPGDCDATA lpdcSrc = NULL;// = _GetSafeDrawPtr( hdcSrc );//�õ�ԴDC����ָ��
    
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
			goto _return;//return FALSE;   //�������
		}
	}
	else
	{
		lpdcSrc = _GetSafeDrawPtr( hdcSrc );//�õ�ԴDC����ָ��
		if( lpdcSrc == NULL )
			goto _return;//return FALSE;
	}

    //if( lpdcDest && lpdcSrc && lpdcDest->lpDispDrv )
	if( lpdcDest && lpdcDest->lpDispDrv )
    {		
		RECT rcMask = { xDest, yDest, xDest + width, yDest + height };
        _LPCDISPLAYDRV lpDispDrv = lpdcDest->lpDispDrv;  //Ŀ��DC��������
		lprgn = _GetHRGNPtr( lpdcDest->hrgn );
		//Դ/Ŀ��λͼ���ݽṹ
        blt.lpDestImage = _GetHBITMAPPtr( lpdcDest->hBitmap );
		
		if( lpdcSrc )
			blt.lpSrcImage = _GetHBITMAPPtr( lpdcSrc->hBitmap );
		else
			blt.lpSrcImage = NULL;

		//��ǰˢ��
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
		//��ǰԴDC����ʾ�豸�ϵ�ƫ��
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

		//ת��Դ���굽�豸����
        rcTemp.left = xSrc;
        rcTemp.top = ySrc;
        rcTemp.right = rcTemp.left + width;
        rcTemp.bottom = rcTemp.top + height;
        OffsetRect( &rcTemp, xSrcOff, ySrcOff );
		//��ǰĿ��DC����ʾ�豸�ϵ�ƫ��
        xoff = XOFFSET( lpdcDest );
        yoff = YOFFSET( lpdcDest );
		//ת��Ŀ�����굽�豸����
        rcClip.left = xDest;
        rcClip.top = yDest;
        rcClip.right = rcClip.left + width;
        rcClip.bottom = rcClip.top + height;
        OffsetRect( &rcClip, xoff, yoff );
		//�õ�Դ��Ŀ��Ĺ�ͬ����
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
		//Դ������Ŀ���ϵ�Ͷ������ƫ��
		xoff = xSrc + xSrcOff - xDest - xoff;
		yoff = ySrc + ySrcOff - yDest - yoff;
		//Ŀ��ü���
        lprnNode = lprgn->lpNodeFirst;

        if( blt.lpSrcImage == NULL || 
			blt.lpDestImage->bmBits == blt.lpSrcImage->bmBits )
        {	//��ͬ����ʾ��
			//����ÿһ���ü����������ʾ�����
            while( lprnNode )
            {	//�õ��ڽ�����
                if( IntersectRect( &rcDest, &rcClip, &lprnNode->rect ) )
                {
                    rcSrc = rcDest;
                    OffsetRect( &rcSrc, xoff, yoff );
					//���û��Ʒ���Ϊ�����ң����ϵ���
                    blt.yPositive = 1;
                    blt.xPositive = 1;
					//
                    if( IntersectRect( &rcTemp, &rcSrc, &rcDest ) )
                    {	//��Ϊ����ͬ����ʾ�棬���Ա���ȷ����ȷ�Ŀ���˳��
						//�����ʾĿ�������Դ�������ص����ұ��뷴�򿽱�
                        if( rcSrc.top  < blt.lprcDest->top )
                            blt.yPositive = 0;	//���ҵ���
                        if( rcSrc.left  < blt.lprcDest->left )
                            blt.xPositive = 0;	//���µ���
                    }
                    lpDispDrv->lpBlkBitBlt( &blt );	//���Ƶ���ʾ��
                }
                lprnNode = lprnNode->lpNext;//��һ���ü���
            }
        }
        else
        {	//��ͬһ����ʾ��
			if( ( blt.lpDestImage->bmBitsPixel == blt.lpSrcImage->bmBitsPixel ||
				  blt.lpSrcImage->bmBitsPixel == 1 ) )
			{	//���ݸ�ʽ��ͬ����Դ��ʽ�Ǻڰ�λͼ��ʽ
				
				//���û��Ʒ���Ϊ�����ң����ϵ���
				blt.yPositive = 1;
				blt.xPositive = 1;				
				//����ÿһ���ü����������ʾ�����
				while( lprnNode )
				{	//�õ��ڽ�����
					if( IntersectRect( &rcDest, &rcClip, &lprnNode->rect ) )
					{
						rcSrc = rcDest;
						OffsetRect( &rcSrc, xoff, yoff );
						lpDispDrv->lpBlkBitBlt( &blt );	//���Ƶ���ʾ��
					}
					lprnNode = lprnNode->lpNext;	//��һ���ü���
				}
			}
			else if( lpdcSrc->lpDispDrv )
			{   // ��ͬ��λͼ��ʽ�� ��Ҫ��ÿһ������д��� ����Ե㿽�� pixel -> pixel
				// �����������Ĵ���ʱ�䣡
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
				//����ÿһ���ü����������ʾ�����
				while( lprnNode )
				{	//�õ��ڽ�����
					if( IntersectRect( &rcDest, &rcClip, &lprnNode->rect ) )
					{
						rcSrc = rcDest;
						OffsetRect( &rcSrc, xoff, yoff );
						//��ÿ�������ת��
						//PixelTransfer( lpDispDrv, &pxDest, &rcDest, lpSrcDispDrv, &pxSrc, &rcSrc, NULL, 0, 0, -1 );
						PixelTransfer( lpdcDest, &pxDest, &rcDest, lpdcSrc, &pxSrc, &rcSrc, NULL, 0, 0, -1 );
					}
					lprnNode = lprnNode->lpNext;//��һ���ü���
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
// ������BOOL WINAPI WinGdi_TransparentBlt(
//							HDC hdcDest - Ŀ��DC
//							int xDest - Ŀ��x����
//							int yDest - Ŀ��x����
//							int width, 
//							int height,
//							HDC hdcSrc, 
//							int xSrc, 
//							int ySrc, 
//							int widthSrc,
//							int heightSrc,
//							DWORD clTransparent )
// ������
//	IN hdcDest - Ŀ��DC
//	IN xDest - Ŀ��������Ͻ�X����
//	IN yDest - Ŀ��������Ͻ�Y����
//	IN width - Ŀ����ο��
//	IN height - Ŀ����θ߶�
//	IN hdcSrc - ԴDC
//	IN xSrc - Դ�������Ͻ�X����
//	IN ySrc - Դ�������Ͻ�Y����
//	IN width - Դ���ο��
//	IN height - Դ���θ߶�
//	IN clTransparent - ͸��ɫ,���� (clTransparent & 0x80000000) != 0, 
//				��˵�� clTransparent �Ǳ�ʾһ����Դλͼ���豸��ص���ɫֵ��������һ��RGBֵ
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	��ԴDC��Ŀ��DC����͸��λͼ����
// ����: 
//	ϵͳAPI
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
    _LPGDCDATA lpdcDest = _GetSafeDrawPtr( hdcDest );//�õ�Ŀ��DC����ָ��
    _LPGDCDATA lpdcSrc = _GetSafeDrawPtr( hdcSrc );//�õ�ԴDC����ָ��
    _LPRGNDATA lprgn;
    _LPRECTNODE lprnNode;
    _BLKBITBLT blt;
    RECT rcSrc, rcTemp, rcClip, rcDest;
    int xoff, yoff, xSrcOff, ySrcOff;
	BOOL retv = FALSE;

    if( lpdcDest && lpdcSrc && lpdcDest->lpDispDrv )
    {	
        _LPCDISPLAYDRV lpDispDrv = lpdcDest->lpDispDrv;  //Ŀ��DC��������
		lprgn = _GetHRGNPtr( lpdcDest->hrgn );
		//Դ/Ŀ��λͼ���ݽṹ
        blt.lpDestImage = _GetHBITMAPPtr( lpdcDest->hBitmap );
        blt.lpSrcImage = _GetHBITMAPPtr( lpdcSrc->hBitmap );
		//��ǰˢ��
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
		//��ǰԴDC����ʾ�豸�ϵ�ƫ��
        xSrcOff = XOFFSET( lpdcSrc );
        ySrcOff = YOFFSET( lpdcSrc );
		//ת��Դ���굽�豸����
        rcTemp.left = xSrc;
        rcTemp.top = ySrc;
        rcTemp.right = rcTemp.left + width;
        rcTemp.bottom = rcTemp.top + height;
        OffsetRect( &rcTemp, xSrcOff, ySrcOff );
		//��ǰĿ��DC����ʾ�豸�ϵ�ƫ��
        xoff = XOFFSET( lpdcDest );
        yoff = YOFFSET( lpdcDest );
		//ת��Ŀ�����굽�豸����
        rcClip.left = xDest;
        rcClip.top = yDest;
        rcClip.right = rcClip.left + width;
        rcClip.bottom = rcClip.top + height;
        OffsetRect( &rcClip, xoff, yoff );
		//�õ�Դ��Ŀ��Ĺ�ͬ����
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
		//Դ������Ŀ���ϵ�Ͷ������ƫ��
        xoff = xSrc + XOFFSET( lpdcSrc ) - xDest - xoff;
        yoff = ySrc + YOFFSET( lpdcSrc ) - yDest - yoff;
		//Ŀ��ü���
        lprnNode = lprgn->lpNodeFirst;
		/* supprt later
        if( blt.lpDestImage->bmBits == blt.lpSrcImage->bmBits )
        {	//��ͬ����ʾ��
			//����ÿһ���ü����������ʾ�����
            while( lprnNode )
            {	//�õ��ڽ�����
                if( IntersectRect( &rcDest, &rcClip, &lprnNode->rect ) )
                {
                    rcSrc = rcDest;
                    OffsetRect( &rcSrc, xoff, yoff );
					//���û��Ʒ���Ϊ�����ң����ϵ���
                    blt.yPositive = 1;
                    blt.xPositive = 1;
					//
                    if( IntersectRect( &rcTemp, &rcSrc, &rcDest ) )
                    {	//��Ϊ����ͬ����ʾ�棬���Ա���ȷ����ȷ�Ŀ���˳��
						//�����ʾĿ�������Դ�������ص����ұ��뷴�򿽱�
                        if( rcSrc.top  < blt.lprcDest->top )
                            blt.yPositive = 0;	//���ҵ���
                        if( rcSrc.left  < blt.lprcDest->left )
                            blt.xPositive = 0;	//���µ���
                    }
                    lpDispDrv->lpBlkBitTransparentBlt( &blt );	//���Ƶ���ʾ��
                }
                lprnNode = lprnNode->lpNext;//��һ���ü���
            }
        }
        else
		*/
        {	//��ͬһ����ʾ��
			/*	//support later
			if( ( blt.lpDestImage->bmBitsPixel == blt.lpSrcImage->bmBitsPixel ||
				  blt.lpSrcImage->bmBitsPixel == 1 ) )
			{	//���ݸ�ʽ��ͬ����Դ��ʽ�Ǻڰ�λͼ��ʽ
				
				//���û��Ʒ���Ϊ�����ң����ϵ���
				blt.yPositive = 1;
				blt.xPositive = 1;				
				//����ÿһ���ü����������ʾ�����
				while( lprnNode )
				{	//�õ��ڽ�����
					if( IntersectRect( &rcDest, &rcClip, &lprnNode->rect ) )
					{
						rcSrc = rcDest;
						OffsetRect( &rcSrc, xoff, yoff );
						lpDispDrv->lpBlkBitTransparentBlt( &blt );	//���Ƶ���ʾ��
					}
					lprnNode = lprnNode->lpNext;	//��һ���ü���
				}
			}
			else if( lpdcSrc->lpDispDrv )
			*/
			{   // ��ͬ��λͼ��ʽ�� ��Ҫ��ÿһ������д��� ����Ե㿽�� pixel -> pixel
				// �����������Ĵ���ʱ�䣡
				_LPCDISPLAYDRV lpSrcDispDrv = lpdcSrc->lpDispDrv;
				_PIXELDATA pxSrc, pxDest;
				
	            pxSrc.lpDestImage = blt.lpSrcImage;
	            pxSrc.pattern = 0xff;
	            pxSrc.rop = R2_NOP;// read only

				pxDest.lpDestImage = blt.lpDestImage;
	            pxDest.pattern = 0xff;
				pxDest.rop = R2_COPYPEN;

				//����ÿһ���ü����������ʾ�����
				while( lprnNode )
				{	//�õ��ڽ�����
					if( IntersectRect( &rcDest, &rcClip, &lprnNode->rect ) )
					{
						rcSrc = rcDest;
						OffsetRect( &rcSrc, xoff, yoff );
						//��ÿ�������ת��
						PixelTransfer( lpdcDest, &pxDest, &rcDest, lpdcSrc, &pxSrc, &rcSrc, NULL, 0, 0, clTransparent );
					}
					lprnNode = lprnNode->lpNext;//��һ���ü���
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
// ������
// ������
// 	IN 
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	
// ����: 
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
// ������int WINAPI WinGdi_SetDIBitsToDevice(
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
// ������
//	IN hdc �C Ŀ��DC
//	IN xDest �C �����Ŀ����ε�x����
//	IN yDest �C �����Ŀ����ε�y����
//	IN dwWidth �C �����Ŀ����εĿ��
//	IN dwHeight �C �����Ŀ����εĸ߶�
//	IN xSrc �C ԴDIB�����ݵĿ�ʼxλ��
//	IN ySrc �C ԴDIB�����ݵĿ�ʼyλ��
//	IN uStartScan �C DIB���ݵĿ�ʼɨ����
//	IN cScanLines �C DIB���ݵ�ɨ��������
//	IN lpvBits �C ����DIB���ݵ�ָ��
//	IN lpbmi - BITMAPINFO�ṹָ�룬��������lpvBits��������Ϣ
//	IN fuColorUse - ˵��BITMAPINFO�ṹ��ԱbmiColors�����ͣ�
//					DIB_PAL_COLORS �C bmiColorsΪ16�������飬ÿһ������ֵָ��ǰhdc�ĵ�ɫ��Ķ�Ӧ�� 
//					DIB_RGB_COLORS �C bmiColorsΪ�졢�̡�����ɫ����
// ����ֵ��
//	����ɹ�������ʵ�ʿ�����ɨ�����������򣬷���0
// ����������
//	����DIB���ݵ�Ŀ���豸��Ӧ�ľ�����
// ����: 
//	ϵͳAPI
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

{	// �õ�DC����ָ��
    _LPGDCDATA lpdc = _GetSafeDrawPtr( hdc );
	int retv = 0;

	if( lpdc && lpbmi )
	{
        _LPBITMAPDATA lpImage = NULL;//
		_LPCDISPLAYDRV lpDrv;
		UINT uiHeight = ABS(lpbmi->bmiHeader.biHeight);
		//�õ�DC��ʾ��
		lpImage = _GetHBITMAPPtr( lpdc->hBitmap );		

	    lpDrv = lpdc->lpDispDrv;
	    if( lpDrv == NULL )
		    goto _return;//return 0;	//�Ƿ�

		if( lpImage )
		{
			int iDir;
			// Դɨ�����ֽ���
			int iScanLineBytes = (lpbmi->bmiHeader.biBitCount * lpbmi->bmiHeader.biWidth + 31) / 32 * 4;  // align to dword
			int iMinWidth, iMinHeight;
			RECT rcSrc, rcDst, rcTemp;
			int xoff = XOFFSET( lpdc );
			int yoff = YOFFSET( lpdc );
			_LPRGNDATA lprgn;
			_LPRECTNODE lprnNode;
            //��ɫ�����Ǵӵ׵������е��� �� 			
			if( lpbmi->bmiHeader.biHeight > 0 )
				iDir = -1;	//�Ǵӵ׵������е�
			else
				iDir = 1;
			//�õ�Ŀ���Դ���ཻ����		
			rcDst.left = xDest;
			rcDst.top = yDest;
			rcDst.right = xDest + dwWidth;
			rcDst.bottom = yDest + dwHeight;
			OffsetRect( &rcDst, xoff, yoff );
			rcTemp.left = 0;
			rcTemp.top = 0;
			rcTemp.right = lpImage->bmWidth;
			rcTemp.bottom = lpImage->bmHeight;
			//Ŀ��������Ŀ����ʾ���ཻ
			if( IntersectRect( &rcDst, &rcDst, &rcTemp ) )
			{	//���ཻ����
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
				//Դ������Դ��ʾ������ཻ
				if( IntersectRect( &rcSrc, &rcSrc, &rcTemp ) )
				{	//���ཻ����
					iMinWidth = rcSrc.right - rcSrc.left;
					iMinHeight = rcSrc.bottom - rcSrc.top;
					
					rcDst.right = rcDst.left + iMinWidth;
					rcDst.bottom = rcDst.top + iMinHeight;
				}
				else
					goto _return;	//û���ཻ�����˳�
			}
			else
				goto _return;	//û���ཻ�����˳�
			xoff = xSrc - xDest - xoff;
			yoff = ySrc - yDest - yoff;
			//�õ�Ŀ��Ĳü�����
			lprgn = _GetHRGNPtr( lpdc->hrgn );
			ASSERT( lprgn );
			//����Ŀ������вü���������пɻ������������
			lprnNode = lprgn->lpNodeFirst;
			while( lprnNode )
			{
				RECT rcDest;
				if( IntersectRect( &rcDest, &rcDst, &lprnNode->rect ) )
				{	//�ཻ
					rcSrc = rcDest;
					OffsetRect( &rcSrc, xoff, yoff );
					//ת��λͼ���ݵ�Ŀ����ʾ��
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
				lprnNode = lprnNode->lpNext;	//��һ���ü���
			}
		}
	}
_return:
	_LeaveDCPtr( lpdc, DCF_WRITE );
	return retv;
}

// **************************************************
// ������BOOL WINAPI WinGdi_Line( HDC hdc, int x0, int y0, int x1, int y1 )
// ������
//	IN hdc - DC���
//	IN x0 - �������X
//	IN y0 - �������Y
//	IN x1 - ��������X
//	IN y1 - ��������Y
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	���߶�
// ����: 
//	ϵͳAPI
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
// ������COLORREF WINAPI WinGdi_GetPixel( HDC hdc, int x, int y )
// ������
//	IN hdc - DC���
//	IN x - ������X
//	IN y - ������Y
// ����ֵ��
//	�ɹ�������RGBֵ
//	���򣺷���CLR_INVALID
// ����������
//	�õ�����ɫֵ
// ����: 
//	ϵͳAPI
// ************************************************

COLORREF WINAPI WinGdi_GetPixel( HDC hdc, int x, int y )
{
    _LPGDCDATA lpdc = _GetSafeDrawPtr( hdc );
	COLORREF retv = CLR_INVALID;

    if( lpdc && lpdc->lpDispDrv )
    {
        _PIXELDATA pixelData;
        pixelData.lpDestImage = _GetHBITMAPPtr( lpdc->hBitmap );
		//ת����Ŀ��DC�豸����
		if( pixelData.lpDestImage )
		{
			pixelData.x = x + XOFFSET( lpdc );
			pixelData.y = y + YOFFSET( lpdc );
			//��Ҫ�ĵ��Ƿ�����ʾ���� ��
			if( WinRgn_PtInRegion( lpdc->hrgn, pixelData.x, pixelData.y ) )
				retv = lpdc->lpDispDrv->lpUnrealizeColor( lpdc->lpDispDrv->lpGetPixel( &pixelData ), lpdc->lpdwPal, lpdc->wPalNumber, lpdc->wPalFormat );
		}
    }

	_LeaveDCPtr( lpdc, DCF_READ );
    return retv;//CLR_INVALID;
}

// **************************************************
// ������COLORREF WINAPI WinGdi_SetPixel( HDC hdc, int x, int y, COLORREF color )
// ������
//	IN hdc - Ŀ��DC���
//	IN x - x����
//	IN y - y����
//	IN color - RGB��ɫֵ
// ����ֵ��
//	����ʵ��д��RGBֵ, ʧ�ܣ����� -1
// ����������
//	�ø�������ɫдһ����
// ����: 
//	ϵͳAPI
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
// ������static int Sqrt32( int x )
// ������
// 	IN x
// ����ֵ��
//	ƽ����
// ����������
//	�� ���� x ����ƽ����
// ����: 
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
// ÿһ��ɨ���е������С�ṹ
typedef struct _MINMAX
{
	int maxl;
	int minr;
}MINMAX;
//�����ݽṹ
typedef struct _ARC_DATA
{
    int nXStartArc; // ��һ�����ߵ�x������ x-coord of first radial ending point
    int nYStartArc; // ��һ�����ߵ�y������ y-coord of first radial ending point
	int nXEndArc;   // �ڶ������ߵ�x������ x-coord of second radial ending point
	int nYEndArc;    // �ڶ������ߵ�y������ y-coord of second radial ending point

    int cxStart;    // = (nXStartArc - x0)
    int cyStart;    // = (nYStartArc - y0)
    int cxEnd;      // = (nXEndArc - x0)
    int cyEnd;      // = (nYEndArc - y0)

    int iStartQuadrant; //��ʼ���ޣ�����0��
    int iEndQuadrant;//�������ޣ�����0��
    int iArcDirect;//  // ˳ʱ�뷽�� = -1, ���� = 1
	BOOL bInvert;   //  �� ��ʼ���������ķ����� iArcDirect�����෴����iInvert = TRUE
}ARC_DATA, * PARC_DATA;

#define DRAW_1_Q   0x01  //     1 ����  //quadrant
#define DRAW_2_Q   0x02  //     2 ����  //quadrant
#define DRAW_3_Q   0x04  //     3 ����  //quadrant
#define DRAW_4_Q   0x08  //     4 ����  //quadrant
#define DRAW_ALL_Q ( DRAW_1_Q | DRAW_2_Q | DRAW_3_Q | DRAW_4_Q )

// **************************************************
// ������static DWORD JudgeArcMiddlePoint( PARC_DATA pArc )
// ������
// 	IN pArc - ARC_DATA �ṹָ��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�ж� ���������˵�֮������ޣ�ʼ����Ч��
// ����: 
//	
// ************************************************

//�ж� ���������˵�֮������ޣ�ʼ����Ч��
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
		if(  (dir > 0 && iStart > iEnd) ||        // ˳ʱ�뷽��
			 (dir < 0 && iStart < iEnd)  )        // ��ʱ�뷽��
		{   //
			dwDrawMode = DRAW_ALL_Q & ( ~(0x1 << pArc->iStartQuadrant) );
		}
	}
	return dwDrawMode;
}

// **************************************************
// ������static DWORD JudgeArcEndPoint( 
//					PARC_DATA pArc, 
//				    int x0, //     ���ĵ�
//					int y0,
//					POINT points[4]
//					)
// ������
// 	IN pArc - ARC_DATA�ṹָ��
//	IN x0 - �������ĵ� x
//	IN y0 - �������ĵ� y
//	IN points - �����ڵľ��ε��ĸ���
// ����ֵ��
//	���ػ��������˵���������
// ����������
//  �ж�ͬ���������˵��������� ��ͬ�ĵ��Ƿ���Ч
//	
// ����: 
//	
// ************************************************


static DWORD JudgeArcEndPoint( PARC_DATA pArc, 
				    int x0, //     ���ĵ�
					int y0,
					POINT points[4]
					)
{
	int cxS = points[pArc->iStartQuadrant].x - x0;  //��ʼ���޵ĵ�
	int cyS = points[pArc->iStartQuadrant].y - y0;
	int cxE = points[pArc->iEndQuadrant].x - x0;  //�������޵ĵ�
	int cyE = points[pArc->iEndQuadrant].y - y0;
	DWORD dwDrawMode  = 0;
	int dir = pArc->iArcDirect;
	int bValidStart, bValidEnd;

	if( dir > 0 )
	{   // ˳ʱ�뷽��
		//cyS / cxS >= pArc->cyStart / pArc->cxStart 
		bValidStart = ( cyS * pArc->cxStart >= pArc->cyStart * cxS );

		//cyE / cxE < pArc->cyEnd / pArc->cxEnd 
		bValidEnd = ( cyE * pArc->cxEnd < pArc->cyEnd * cxE );
	}
	else
	{   //��ʱ�뷽��
		//cyS / cxS <= pArc->cyStart / pArc->cxStart 
		bValidStart = (cyS * pArc->cxStart <= pArc->cyStart * cxS);

		//cyE / cxE > pArc->cyEnd / pArc->cxEnd 
		bValidEnd = ( cyE * pArc->cxEnd > pArc->cyEnd * cxE );
	}
	
	if( pArc->iEndQuadrant == pArc->iStartQuadrant )
	{	//��ʼ���� = ��������
		if( !pArc->bInvert )
		{   //��ͬ�������� ���� �� ArcDirect ��˳����
			if( bValidStart && bValidEnd ) // ����ͬʱ����
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
// ������static BOOL DrawEllipseRect( 
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
// ������
//	IN lpDispDrv - ��ʾ��������
//	IN lpLineData - �������ݽṹ
//	IN lprNodes - �ü���
//  IN lpBlkData - �����ṹ
//  IN lpFillRgn - �������ṹ
//	IN x0 - ��Բ��xԶ��    // ellips's origin x point 
//	IN y0 - ��Բ��yԶ��    // ellips's origin y point 
//	IN r1 - ��Բ�� a �뾶����    // ellips's origin a len 
//	IN r2 - ��Բ�� b �뾶����   // ellips's origin b len
//	IN lw - �ʵ��󲿷ֿ��,   // pen's left width
//	IN rw - �ʵ��Ҳ��ֿ��,   // pen's right width
//	IN uiFlag - ����־��
//				EF_FILL - �����Բ
//				EF_ROUNDRECT - �����ε�Բ������
//				EF_OUTLINE - ����Բ������
//	IN nRectWidth - ���ο��
//	IN nRectHeight - ���θ߶�
//	IN lpMinMax - �����С�ṹ��������һ����ȵ�������ʱ���ýṹ������������ɨ�����ϵ����/��С���ֵ��
//	IN pArc - ARC_DATA�ṹָ�룬������Arc��ʱ��Ч
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	����Բ
// ����: 
//	�ú������ս��  Ellipse�� RoundRect�� Arc �Ļ�������
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
	{	//��Ҫ�����Բ���ڲ�
		if( lpBlkData->backMode == TRANSPARENT && lpBlkData->lpBrush->style != BS_SOLID )
			lpFillFun = _FillTransparentRgn;	//͸�����
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
	{	//�жϻ����м䲿�����ڵ�����
		dwMiddleMask = JudgeArcMiddlePoint( pArc );
	}
	
	// ��һ�λ�
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

		// �������� draw outline
		if( uiFlag & EF_OUTLINE )
		{
			if( x == 0 && (uiFlag & EF_ROUNDRECT) )
			{	//��Բ�Ǿ��ε��/������²���
				_DrawLine( lpDispDrv, lprNodes, lpLineData, l, b + wdiff, r + 1, b + wdiff );//һ����
				_DrawLine( lpDispDrv, lprNodes, lpLineData, l, t, r + 1, t );//������
			}
			else
			{
				if( pArc && (dwMiddleMask != DRAW_ALL_Q) )
				{   //����
					POINT pts[4];

					pts[0].x = r;
					pts[0].y = b;
					pts[1].x = l;
					pts[1].y = b;
					pts[2].x = l;
					pts[2].y = t;					
					pts[3].x = r;
					pts[3].y = t;
					//�õ����������˵����ڵ�����
					dwDrawMask = dwMiddleMask | JudgeArcEndPoint( pArc, x0, y0, pts );
				}
				if( dwDrawMask & DRAW_1_Q )
				    _DrawLine( lpDispDrv, lprNodes, lpLineData, r, b, r, b - 1 );//һ����
				if( dwDrawMask & DRAW_2_Q )
				    _DrawLine( lpDispDrv, lprNodes, lpLineData, l, b, l, b - 1 );//������
				if( dwDrawMask & DRAW_4_Q )
				    _DrawLine( lpDispDrv, lprNodes, lpLineData, r, t, r, t + 1 );//������
				if( dwDrawMask & DRAW_3_Q )
				    _DrawLine( lpDispDrv, lprNodes, lpLineData, l, t, l, t + 1 );//������
			}
		}

		if( lpFillFun && 
			xyCur != y &&
			y <= yFillStart
          )
		{   //��Ҫ���
			//׼���������
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

			//���һ����
            lpFillFun( lpDispDrv, lpBlkData, lpFillRgn );

			//���ԳƵ���һ����
			lpFillRgn->rect.top = t;
			lpFillRgn->rect.bottom = t + 1;

            lpFillFun( lpDispDrv, lpBlkData, lpFillRgn );
			xyCur = y;
		}

		if( yPrev != y && lpCurSave && lpFillFun )
		{   // ���ʵĺ�ȶԴ���1ʱ����Щ�������á�the data use when pen width > 1 and fill enable
			if( lpCurSave == lpEnd )
				lpCurSave = lpMinMax;
			lpCurSave->maxl = l;
			lpCurSave->minr = r;
			lpCurSave++;
			yPrev = y;
		}

		x++;
	}

	// �ڶ��λ�
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
		{	// ��������
            if( x == 0 && (uiFlag & EF_ROUNDRECT) )
			{	//��Բ�Ǿ��ε��/������²���
				_DrawLine( lpDispDrv, lprNodes, lpLineData, r, t, r, b + 1 );//һ����
				_DrawLine( lpDispDrv, lprNodes, lpLineData, l, t, l, b + 1 );//������
			}
			else
			{
                if( pArc && (dwMiddleMask != DRAW_ALL_Q) )
				{   //����
					POINT pts[4];

					pts[0].x = r;
					pts[0].y = b;
					pts[1].x = l;
					pts[1].y = b;
					pts[2].x = l;
					pts[2].y = t;
					pts[3].x = r;
					pts[3].y = t;
					//�õ����������˵����ڵ�����					
					dwDrawMask = dwMiddleMask | JudgeArcEndPoint( pArc, x0, y0, pts );
				}
				if( dwDrawMask & DRAW_1_Q )
				    _DrawLine( lpDispDrv, lprNodes, lpLineData, r, b, r, b - 1 );//һ����
                if( dwDrawMask & DRAW_4_Q )
				    _DrawLine( lpDispDrv, lprNodes, lpLineData, r, t, r, t + 1 );//������
                if( dwDrawMask & DRAW_2_Q )
					_DrawLine( lpDispDrv, lprNodes, lpLineData, l, b, l, b - 1 );//������
                if( dwDrawMask & DRAW_3_Q )
					_DrawLine( lpDispDrv, lprNodes, lpLineData, l, t, l, t + 1 );//������
			}
		}
		//���
		if( lpFillFun )
		{	//��Ҫ����ڲ�
			if( lpCurSave == NULL || x <= yPrev )
			{
				lpFillRgn->rect.left = l + lw;
				lpFillRgn->rect.top = b;
				lpFillRgn->rect.right = r + 1 - rw;
				lpFillRgn->rect.bottom = b + 1;
                //RETAILMSG( 1, ( "2,l=%d,t=%d,r=%d,b=%d.\r\n",lpFillRgn->rect.left,lpFillRgn->rect.top,lpFillRgn->rect.right,lpFillRgn->rect.bottom ) );
				//���һ����
				lpFillFun( lpDispDrv, lpBlkData, lpFillRgn );				
				
				lpFillRgn->rect.top = t;				
				lpFillRgn->rect.bottom = t + 1;
				//RETAILMSG( 1, ( "3,l=%d,t=%d,r=%d,b=%d.\r\n",lpFillRgn->rect.left,lpFillRgn->rect.top,lpFillRgn->rect.right,lpFillRgn->rect.bottom ) );
				//���ԳƵ���һ����
				lpFillFun( lpDispDrv, lpBlkData, lpFillRgn );
			}
			else
			{	//��Ϊ�������Ѿ�������ɨ���ߵ�һ���֣����ֻ��Ҫ���ɨ���ߵľֲ�
				lpFillRgn->rect.left = lpCurSave->maxl + lw;
				lpFillRgn->rect.top = b;
				lpFillRgn->rect.right = lpCurSave->minr + 1 - rw;
				lpFillRgn->rect.bottom = b + 1;
				//���һ����
				lpFillFun( lpDispDrv, lpBlkData, lpFillRgn );
    			if( lpCurSave == lpMinMax )
   				    lpCurSave = lpEnd - 1;
				else
				    lpCurSave--;
				
				lpFillRgn->rect.top = t;
				lpFillRgn->rect.bottom = t + 1;
				//RETAILMSG( 1, ( "4,l=%d,t=%d,r=%d,b=%d.\r\n",lpFillRgn->rect.left,lpFillRgn->rect.top,lpFillRgn->rect.right,lpFillRgn->rect.bottom ) );
				//���ԳƵ���һ����
				lpFillFun( lpDispDrv, lpBlkData, lpFillRgn );
			}
		}

		x++;	
	}
	if( lpFillFun && 
		(uiFlag & EF_ROUNDRECT) )
	{	//����ǻ��Ƶ�Բ�Ǿ��Σ���������м�ľ��β���
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
// ������static BOOL EllipseRect(
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
// ������
//	IN hdc - ��ͼDC
//	IN left - ���ε���, // Ellipse's left else RoundRect' left
//	IN top - ���εĶ� // Ellipse's top .....
//	IN right - ���ε��� // Ellipse's right .....
//	IN bottom - ���εĵ� // Ellipse's bottom .....
//	IN nWidth - ���εĿ�� // if bRoundRect == TRUE, rect width .....
//	IN nHeight - ���εĸ߶� //if bRoundRect == TRUE, rect height .....
//	IN bRoundRect - �Ƿ���Բ�Ǿ���
//	IN pArc - ������ arc ʱ��Ч
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	���� Ellipse, RoundRect Arc
// ����: 
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
	//�õ���ȫ�� DCָ��
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

		//�ü���
        lprNodes = _GetHRGNPtr( lpdc->hrgn )->lpNodeFirst;
        if( lprNodes == 0 )
		{
			retv = TRUE;
            goto _return;//return TRUE;
		}
		//��ʾ��
        lpBitmap = _GetHBITMAPPtr( lpdc->hBitmap );
        if( lpBitmap == 0 )
		{
            goto _return;//return FALSE;
		}
		// ���߼�����ת��Ϊ�豸���� make origin point
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
		{  //���� nWidth & nHeight
			nWidth += (right - left) - (ra * 2 + 1);
			nHeight += (bottom - top) - (rb * 2 + 1);
		}

		// get pen data
		lpPenAttrib = &lpdc->penAttrib;

        if( lpPenAttrib->pattern != PS_NULL )
        {	//׼���ʽṹ
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
        {   // ׼�����ṹ do fill
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
		{	//��Ҫ����...
			if( pArc )
			{   // arc ����
				//׼�� ARC �ṹ
				pArc->nXStartArc += xoffset;
				pArc->nYStartArc += yoffset;
				pArc->nXEndArc += xoffset;
				pArc->nYEndArc += yoffset;

				pArc->cxStart = pArc->nXStartArc - x0;
				pArc->cyStart = pArc->nYStartArc - y0;
				pArc->cxEnd = pArc->nXEndArc - x0;
				pArc->cyEnd = pArc->nYEndArc - y0;

				if( pArc->cxStart >= 0 && pArc->cyStart >= 0 )
				    pArc->iStartQuadrant = 0;  // 1 ����
				else if( pArc->cxStart <= 0 && pArc->cyStart >= 0 )
				    pArc->iStartQuadrant = 1;  // 2 ����
				else if( pArc->cxStart <= 0 && pArc->cyStart <= 0 )
				    pArc->iStartQuadrant = 2;  // 3 ����
				else if( pArc->cxStart >= 0 && pArc->cyStart <= 0 )
				    pArc->iStartQuadrant = 3;  // 4 ����
				else
				{
					ASSERT( 0 );
				}
				if( pArc->cxEnd >= 0 && pArc->cyEnd >= 0 )
				    pArc->iEndQuadrant = 0;  // 1 ����
				else if( pArc->cxEnd <= 0 && pArc->cyEnd >= 0 )
				    pArc->iEndQuadrant = 1;  // 2 ����
				else if( pArc->cxEnd <= 0 && pArc->cyEnd <= 0 )
				    pArc->iEndQuadrant = 2;  // 3 ����
				else if( pArc->cxEnd >= 0 && pArc->cyEnd <= 0 )
				    pArc->iEndQuadrant = 3;  // 4 ����
				else
				{
					ASSERT( 0 );
				}
				//���Ʒ���˳ʱ���ʱ�룩
				pArc->iArcDirect = (lpdc->arcMode == AD_COUNTERCLOCKWISE) ? -1 : 1;
				
				//  �� ��ʼ���������ķ���Ҳ iArcDirect�����෴����iInvert = TRUE
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
					if(  (pArc->iArcDirect > 0 && iStart > iEnd) ||        // ˳ʱ�뷽��
						(pArc->iArcDirect < 0 && iStart < iEnd)  )        // ��ʱ�뷽��
					{
						pArc->bInvert = TRUE;
					}
				}
				
				DrawEllipseRect( lpDispDrv, &lineData, lprNodes, &blkData, &aFillRgn, x0, y0, ra, rb, lw, rw, uiFlag, nWidth, nHeight, NULL, pArc );
			}
			else
			{   // ��Բ �� Բ����Բ epllise or round rect
				if( bRoundRect )
					uiFlag |= EF_ROUNDRECT;
				//�����к��ʱ������׼��һ����ʱ����ȥ�������������꣬���磺
				// [***]        ........................... scan1   
				// [****]       ........................... scan2
				// [****]       ........................... scan3
				//  [*****]     ........................... scan4
				//   [******]   ........................... scan5

				if( lw <= 16 )
				{	//���٣��� sp 
					MINMAX minmax[16];
					DrawEllipseRect( lpDispDrv, &lineData, lprNodes, &blkData, &aFillRgn, x0, y0, ra, rb, lw, rw, uiFlag, nWidth, nHeight, &minmax[0], NULL );
				}
				else
				{	//����, ��̬
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
// ������BOOL WINAPI WinGdi_Ellipse(
//						   HDC hdc, 
//						   int left, 
//						   int top, 
//						   int right, 
//						   int bottom
//						   )

// ������
// 	IN hdc - ��ͼDC
//	IN left - ��Χ��Բ�ľ��� ��
//	IN top - ��Χ��Բ�ľ��� ��
//	IN right - ��Χ��Բ�ľ��� ��
//	IN bottom - ��Χ��Բ�ľ��� ��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	����Բ����Բ�������Ǿ��ε����ģ���Բ�������õ�ǰ�������Ʋ��õ�ǰˢ�������
// ����: 
//	ϵͳAPI
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

	//������
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
// ������BOOL WINAPI WinGdi_RoundRect(
//						   HDC hdc, 
//						   int left, 
//						   int top, 
//						   int right, 
//						   int bottom,
//						   int nWidth,   // ellipse width
//						   int nHeight   // ellipse height
//						   )
// ������
//	IN hdc - DC���
//	IN left - ���ε����Ͻ�x����
//	IN top - ���ε����Ͻ�y����
//	IN right - ���ε����½�x����
//	IN bottom -���ε����½�y����
//	IN nWidth �C Բ�ǵĿ��
//	IN nHeight - Բ�ǵĸ߶�
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	��Բ�нǾ���
// ����: 
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
	//������
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
// ������BOOL WINAPI WinGdi_Arc(
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

// ������
//	IN hdc - DC���
//	IN left �C ���ε�����x����
//	IN top �C ���ε�����y����
//	IN right �C ���ε�����x����
//	IN bottom �C ���ε�����y����
//	IN nXStartArc �C ��ʼ���ߵĽ�����x����
//	IN nYStartArc �C ��ʼ���ߵĽ�����y����
//	IN nXEndArc �C ��ֹ���ߵĽ�����x����
//	IN nYEndArc �C ��ֹ���ߵĽ�����y����
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	����Բ��
// ����: 
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
	//������	
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
// ������static BOOL _NewClipLine( 
//						_LINEDATA * lpLine, 
//						int iInitError, 
//						int x0, 
//						int y0, 
//						int dx, 
//						int dy, 
//						LPRECT lprcClip )
// ������
// 	IN/OUT lpLine - �߽ṹ����ָ��,���ڽ��ܲü�����߶�
//	IN iInitError - ���
//	IN x0 - �߶ε����x0
//	IN y0 - �߶ε����y0
//	IN dx - �߶ε�x���򳤶�
//	IN dy - �߶ε�y���򳤶�
//	IN lprcClip - �ü�����
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�ھ����ڲü��߶�
// ����: 
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
// ������static BOOL _DrawLine( 
//					_LPCDISPLAYDRV lpDispDrv, 
//					_LPRECTNODE lprn, 
//					_LINEDATA * lpLine, 
//					int x0, 
//					int y0, 
//					int x1, 
//					int y1 )
// ������
//	IN lpDispDrv - ��ʾ��������
//	IN lprn - �ü��ڵ�
//	IN lpLine - �߽ṹ����
//	IN x0 - �߶ε����x
//	IN y0 - �߶ε����y
//	IN x1 - �߶ε��յ�x
//	IN y1 - �߶ε��յ�y
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�ڲ�ͬ�ü������ϻ���
// ����: 
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

	// ��ÿһ���ü������ϻ��� now draw line in clip region

	while( lprn )
	{
		lpLine->lprcClip = &lprn->rect;
		if( dx == 0 )
		{   // ��ֱ�� vert line
			// �Կ��ߴ�����, 2005-09-29
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
		{   // ˮƽ�� hori line
			// �Կ��ߴ�����, 2005-09-29
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
		{   // б�� dia line			
			if( _NewClipLine( lpLine, iInitErrorCount, x0, y0, dx, dy, &lprn->rect ) )
			{				
				lpDispDrv->lpLine( lpLine );
			}
			
		}
		
		lprn = lprn->lpNext;	//��һ���ü�����
	}
	return TRUE;

}

// **************************************************
// ������BOOL WINAPI WinGdi_LineTo( HDC hdc, int x1, int y1 )
// ������
//	IN hdc-Ŀ��DC
//	IN x1-�߽�����X����
//	IN y1-�߽�����Y����
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	���ߵ�������,���������õ�
//	������óɹ�, current position(CP)ֵ�Ƶ�������
// ����: 
//	ϵͳAPI
// ************************************************

BOOL WINAPI WinGdi_LineTo( HDC hdc, int x1, int y1 )
{	//�õ���ȫ DC
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

		//�߼����굽�豸����
        // make origin point
        x0 += xoffset;
        y0 += yoffset;
        x1 += xoffset;
        y1 += yoffset;

        lpPenAttrib = &lpdc->penAttrib;//_GetHPENPtr( lpdc->hPen );
		//����������		
        lineData.backMode = lpdc->backMode;
        lineData.clrBack = lpdc->backColor;
        lineData.color = lpPenAttrib->color;
        lineData.lpDestImage = lpBitmap;
        lineData.pattern = lpPenAttrib->pattern;
        lineData.rop = lpdc->rop;
        lineData.width = lpPenAttrib->width;
		lineData.lpfnCallback = NULL;
		//�����߶�		      
        _DrawLine( lpdc->lpDispDrv, lprn->lpNodeFirst, &lineData, x0, y0, x1, y1 );
        retv = TRUE;//return TRUE;
    }
	_LeaveDCPtr( lpdc, DCF_WRITE );
    return retv;////return FALSE;
}

// **************************************************
// ������BOOL WINAPI WinGdi_MoveTo( HDC hdc, int x, int y, LPPOINT lppt )
// ������
//	IN hdc-Ŀ����
//	IN x-�µ�ǰλ��X����
//	IN y-�µ�ǰλ��Y����
//	OUT lppt-����֮ǰ���õ�ԭ��,POINT�ṹָ��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	���õ�ǰλ��
// ����: 
//	ϵͳ API
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
// ������BOOL WINAPI WinGdi_Polyline( HDC hdc, const POINT* lppts, int nCount )
// ������
//	IN hdc-Ŀ��DC
//	IN lpcPoints-����x,y����Եĵ�����,POINT�ṹָ��
//	IN nCount-���������
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	��������˳�������߶�
// ����: 
//	ϵͳAPI 
// ************************************************

BOOL WINAPI WinGdi_Polyline( HDC hdc, const POINT* lppts, int nCount )
{
    int  i;
    POINT pt;
    if( nCount >= 2 )
    {
        WinGdi_MoveTo( hdc, lppts->x, lppts->y, &pt );
		lppts++;
		//��ÿһ����
        for( i = 1; i < nCount; i++, lppts++ )
	        WinGdi_LineTo( hdc, lppts->x, lppts->y );
        WinGdi_MoveTo( hdc, pt.x, pt.y, 0 );
	    return TRUE;
    }
    return FALSE;
}

// **************************************************
// ������static void _FillRgn( _LPCDISPLAYDRV lpDispDrv, _LPBLKBITBLT lpFillData, _FILLRGN * lpFillRgn )
// ������
// 	IN lpDispDrv - ��ʾ��������
//	IN lpFillData - ������ṹ
//	IN lpFillRgn - �ü���ṹ
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�ڲ�ͬ�Ĳü������ͼ��
// ����: 
//	
// ************************************************

static void _FillRgn( _LPCDISPLAYDRV lpDispDrv, _LPBLKBITBLT lpFillData, _FILLRGN * lpFillRgn )
{
    register const _RECTNODE FAR* lprNodes = lpFillRgn->lprNodes;
    RECT rectClip;
	//
    lpFillData->lprcDest = &rectClip;
	//����ÿһ���ü���
    while( lprNodes )
    {
	    if( IntersectRect( &rectClip, &lprNodes->rect, &lpFillRgn->rect ) )   // not empty
            lpDispDrv->lpBlkBitBlt( lpFillData );
	    lprNodes = lprNodes->lpNext;
    }
}

// **************************************************
// ������static void _FillTransparentRgn( _LPCDISPLAYDRV lpDispDrv, _LPBLKBITBLT lpFillData, _FILLRGN * lpFillRgn )
// ������
// 	IN lpDispDrv - ��ʾ��������
//	IN lpFillData - ������ṹ
//	IN lpFillRgn - �ü���ṹ
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�ڲ�ͬ�Ĳü�����͸����ʽ���ͼ��
// ����: 
//	
// ************************************************

static void _FillTransparentRgn( _LPCDISPLAYDRV lpDispDrv, _LPBLKBITBLT lpFillData, _FILLRGN * lpFillRgn )
{
    register const _RECTNODE FAR* lprNodes = lpFillRgn->lprNodes;
    RECT rectClip;

    lpFillData->lprcDest = &rectClip;
	//����ÿһ���ü���
    while( lprNodes )
    {
	    if( IntersectRect( &rectClip, &lprNodes->rect, &lpFillRgn->rect ) )   // not empty
            lpDispDrv->lpBlkBitTransparentBlt( lpFillData );
	    lprNodes = lprNodes->lpNext;
    }
}

// **************************************************
// ������BOOL WINAPI WinGdi_DrawFocusRect( HDC hdc, LPCRECT lprc )
// ������
//	IN hdc-Ŀ��DC
//	IN lprc-�߿�,RECT�ṹָ��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	���۽��߿�
// ����: 
//	ϵͳAPI
// ************************************************

BOOL WINAPI WinGdi_DrawFocusRect( HDC hdc, LPCRECT lprc )
{	//�õ���ȫ��DC
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
		//�õ���ʾ��
        lpBitmap = _GetHBITMAPPtr( lpdc->hBitmap );
		//�õ��ü���
        lprNodes = _GetHRGNPtr( lpdc->hrgn )->lpNodeFirst;

        xoffset = XOFFSET( lpdc );
        yoffset = YOFFSET( lpdc );
        left = lprc->left + xoffset;
        top = lprc->top + yoffset;
        right = lprc->right + xoffset;
        bottom = lprc->bottom + yoffset;
		//׼����������
        lineData.backMode = TRANSPARENT;
        lineData.clrBack = lpdc->backColor;
        lineData.color = lpDispDrv->lpRealizeColor( CL_LIGHTGRAY, lpdc->lpdwPal, lpdc->wPalNumber, lpdc->wPalFormat );
        lineData.lpDestImage = lpBitmap;
        lineData.pattern = 0x55;
        lineData.rop = R2_XORPEN;
        lineData.width = 1;
		lineData.lpfnCallback = NULL;
		//���ı�
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
// ������BOOL WINAPI WinGdi_Rectangle( HDC hdc, int left, int top, int right, int bottom )
// ������
//	IN hdc-Ŀ��DC
//	IN left-Ŀ���������X
//	IN top-Ŀ���������Y
//	IN right-Ŀ���������X
//	IN bottom-Ŀ���������Y
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	���ƾ���---�õ�ǰ�ʻ��߿�,�õ�ǰˢ�����߿��ڲ�
// ����: 
//	ϵͳ API
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

		//��������Ч��
        if( left == right || top == bottom )
            goto _return;//return FALSE;
		//�Ƿ�Ե����� ��
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
		//�ü���
        lprNodes = _GetHRGNPtr( lpdc->hrgn )->lpNodeFirst;
        if( lprNodes == 0 )
		{
			retv = TRUE;
			goto _return;//            return TRUE;
		}
		//��ʾ��
        lpBitmap = _GetHBITMAPPtr( lpdc->hBitmap );
        if( lpBitmap == 0 )
		{
            goto _return;//return FALSE;
		}
		rcBrushMask.left = left;
		rcBrushMask.top = top;
		rcBrushMask.right = right;
		rcBrushMask.bottom = bottom;
		// �߼��㵽�豸�� make origin point
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
            // ���߿�
            _DrawLine( lpdc->lpDispDrv, lprNodes, &lineData, left, top, right, top );    
            _DrawLine( lpdc->lpDispDrv, lprNodes, &lineData, right-1, top, right-1, bottom );
            _DrawLine( lpdc->lpDispDrv, lprNodes, &lineData, left, top, left, bottom );
            _DrawLine( lpdc->lpDispDrv, lprNodes, &lineData, left, bottom - 1, right, bottom - 1 );
        }
		lpBrushAttrib = &lpdc->brushAttrib;

        if( lpBrushAttrib->style != BS_NULL )
        {   // ����ڲ� do fill
            if( lpPenAttrib->pattern != PS_NULL && lpPenAttrib->width >= 1 )
            {	//���ݱ߿���У���ڲ����εĴ�С
                hw = lpPenAttrib->width;

                left += WIDTH_RIGHT( hw )+1;
                top += WIDTH_BOTTOM( hw )+1;
                right -= WIDTH_LEFT( hw )+1;
                bottom -= WIDTH_TOP( hw )+1;
            }
			//׼���������
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
            //���
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
// ������BOOL WINAPI WinGdi_FillRect( HDC hdc, LPCRECT lpRect, HBRUSH hBrush )
// ������
//	IN hdc-Ŀ��DC
//	IN lpRect-Ŀ�����,RECT�ṹָ��
//	IN hBrush-ˢ��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	��ˢ��������
// ����: 
//	ϵͳ API
// ************************************************

BOOL WINAPI WinGdi_FillRect( HDC hdc, LPCRECT lpRect, HBRUSH hBrush )
{	//�õ���ȫ DC
    _LPGDCDATA lpdc = _GetSafeDrawPtr( hdc );
	BOOL retv = FALSE;

	if( hBrush && lpdc && lpdc->lpDispDrv )
    {
        _FILLRGN aFillRgn;
        _LPBITMAPDATA lpBitmap;
        _BLKBITBLT blkData;
		_LPBRUSHDATA lpBrush;
		_BRUSHATTRIB brushAttrib;
		//�����ϵͳ��ɫˢ���õ���ʵ�ľ��
		if( (UINT)hBrush <= (UINT)SYS_COLOR_NUM )
			hBrush = WinSys_GetColorBrush( (int)hBrush - 1 );
	    lpBrush = _GetHBRUSHPtr( hBrush );		
        lpBitmap = _GetHBITMAPPtr( lpdc->hBitmap );
        if( lpBitmap && lpBrush )
        {	//����ˢ�Ӷ���
			brushAttrib = lpBrush->brushAttrib;
			//�õ��豸�����ɫ
			brushAttrib.color = lpdc->lpDispDrv->lpRealizeColor( brushAttrib.clrRef, lpdc->lpdwPal, lpdc->wPalNumber, lpdc->wPalFormat );

			aFillRgn.rect = *lpRect;
			// �߼����굽�豸���� make origin point
            OffsetRect( &aFillRgn.rect, XOFFSET( lpdc ), YOFFSET( lpdc ) );
			// do fill
            aFillRgn.lprNodes = _GetHRGNPtr( lpdc->hrgn )->lpNodeFirst;
			// ׼��ˢ������
            blkData.lpDestImage = lpBitmap;
            blkData.lpBrush = &brushAttrib;
			blkData.lpptBrushOrg = &lpdc->ptBrushOrg;
			////2005-09-15, modify
			//blkData.lprcMask = &aFillRgn.rect;  
			blkData.lprcMask = lpRect;
			//
            blkData.solidBkColor = lpdc->backColor;
            blkData.dwRop = PATCOPY;
			// ���
            _FillRgn( lpdc->lpDispDrv, &blkData, &aFillRgn );
            retv = TRUE;//return TRUE;
        }
    }
	_LeaveDCPtr( lpdc, DCF_WRITE );
    return retv;//FALSE;
}

// **************************************************
// ������BOOL WINAPI WinGdi_FillRgn( HDC hdc, HRGN hrgn, HBRUSH hBrush )
// ������
//	IN hdc-Ŀ��DC
//	IN hrgn-Ŀ������
//	IN hBrush-ˢ��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�ø�����ˢ��������������
// ����: 
//	ϵͳ API
// ************************************************

BOOL WINAPI WinGdi_FillRgn( HDC hdc, HRGN hrgn, HBRUSH hBrush )
{	//�õ���ȫ DC
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
		//�õ���ʾ��
        lpBitmap = _GetHBITMAPPtr( lpdc->hBitmap );
        if( lpBitmap )
        {	//����һ���µ���ʾ����
			WinRgn_GetBox( hrgn, &rcBrushMask ); //2005-09-15, add
            hrgnClip = WinRgn_CreateRect( 0, 0, 0, 0 );
            WinRgn_Combine( hrgnClip, hrgn, 0, RGN_COPY );
			//ת�����豸����
            WinRgn_Offset( hrgnClip, XOFFSET( lpdc ), YOFFSET( lpdc ) );
			//��DC�Ĳü����ཻ
            WinRgn_Combine( hrgnClip, hrgnClip, lpdc->hrgn, RGN_AND );
			//�õ����Ĳü���ľ���
            WinRgn_GetBox( hrgnClip, &aFillRgn.rect );
            aFillRgn.lprNodes = _GetHRGNPtr( hrgnClip )->lpNodeFirst;			
			//׼���鴫������
            blkData.lpDestImage = lpBitmap;			
            blkData.lpBrush = ( lpBrush = _GetHBRUSHPtr( hBrush ) ) ? &lpBrush->brushAttrib : NULL;
			blkData.lpptBrushOrg = &lpdc->ptBrushOrg;
			// 2005-09-15, modify by ln
			//blkData.lprcMask = &aFillRgn.rect;
			blkData.lprcMask = &rcBrushMask;
			//
            blkData.solidBkColor = lpdc->backColor;
            blkData.dwRop = PATCOPY;
			//���
            _FillRgn( lpdc->lpDispDrv, &blkData, &aFillRgn );
            WinGdi_DeleteObject( hrgnClip );
            retv = TRUE;//return TRUE;
        }
    }
	_LeaveDCPtr( lpdc, DCF_WRITE );
    return retv;//FALSE;
}

// **************************************************
// ������BOOL WINAPI WinGdi_InvertRect( HDC hdc, LPCRECT lpcRect )
// ������
//	IN hdc-Ŀ��DC���
//	IN hrgn-Ŀ�����
//	IN lpcRect-RECT�ṹָ��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	��תĿ�����
// ����: 
//	ϵͳAPI
// ************************************************

BOOL WINAPI WinGdi_InvertRect( HDC hdc, LPCRECT lpcRect )
{
    return WinGdi_PatBlt( hdc, lpcRect->left, lpcRect->top, lpcRect->right-lpcRect->left, lpcRect->bottom-lpcRect->top, DSTINVERT );
}

// **************************************************
// ������BOOL WINAPI WinGdi_InvertRgn( HDC hdc, HRGN hrgn )
// ������
// 	IN hdc-Ŀ��DC
//	IN hrgn-Ŀ������
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	��תĿ������
// ����: 
//	ϵͳAPI
// ************************************************

BOOL WINAPI WinGdi_InvertRgn( HDC hdc, HRGN hrgn )
{	//�õ���ȫ DC
    _LPGDCDATA lpdc = _GetSafeDrawPtr( hdc );//
	BOOL retv = FALSE;

    if( hrgn && lpdc && lpdc->lpDispDrv )
    {
        _FILLRGN aFillRgn;
        HRGN hrgnClip;
        _LPBITMAPDATA lpBitmap;
        _BLKBITBLT blkData;
		//�õ���ʾ��
        lpBitmap = _GetHBITMAPPtr( lpdc->hBitmap );
        if( lpBitmap )
        {	//��������
            hrgnClip = WinRgn_CreateRect( 0, 0, 0, 0 );
            WinRgn_Combine( hrgnClip, hrgn, 0, RGN_COPY );
			//�߼����굽�豸����
            WinRgn_Offset( hrgnClip, XOFFSET( lpdc ), YOFFSET( lpdc ) );
			//��DC�Ĳü������ڽ�
            WinRgn_Combine( hrgnClip, hrgnClip, lpdc->hrgn, RGN_AND );

            WinRgn_GetBox( hrgnClip, &aFillRgn.rect );
			//�ü�����
            aFillRgn.lprNodes = _GetHRGNPtr( hrgnClip )->lpNodeFirst;

            blkData.lpDestImage = lpBitmap;
            blkData.dwRop = DSTINVERT;
			//���
            _FillRgn( lpdc->lpDispDrv, &blkData, &aFillRgn );

            WinGdi_DeleteObject( hrgnClip );
            retv = TRUE;//return TRUE;
        }
    }
	_LeaveDCPtr( lpdc, DCF_WRITE );
    return retv;//FALSE;
}

// **************************************************
// ������BOOL WINAPI WinGdi_PatBlt(HDC hdc, int nX, int nY,  int nWidth, int nHeight, DWORD dwRop)
// ������
//	IN hdc-Ŀ��DC
//	IN nX-Ŀ��������Ͻ�X����
//	IN nY-Ŀ��������Ͻ�Y����
//	IN nWidth-Ŀ����ο��
//	IN nHeight-Ŀ����θ߶�
//	IN dwRop-��դ������PATCOPY-����ģ�嵽Ŀ�����
//		PATINVERT-ģ����ɫ��Ŀ����ɫ���������(XOR)
//		BLACKNESS-�ú�ɫ���Ŀ�����
//		WHITENESS-�ð�ɫ���Ŀ�����
//		DSTINVERT-��תĿ����ɫ
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�õ�ǰˢ��(brush)����Ŀ����
// ����: 
//	ϵͳ API
// ************************************************

BOOL WINAPI WinGdi_PatBlt(HDC hdc, int nX, int nY,  int nWidth, int nHeight, DWORD dwRop)
{	//�õ���ȫ DC
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
			// ׼�����ṹ do fill
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
// ������BOOL WINAPI WinGdi_MaskBlt( HDC hdc,
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

// ������
//	IN hdcDest-Ŀ��DC
//	IN nXDest-Ŀ��������Ͻ�X����
//	IN nYDest-Ŀ��������Ͻ�Y����
//	IN nWidth-Ŀ����ο��
//	IN nHeight-Ŀ����θ߶�
//	IN hdcSrc-ԴDC
//	IN nXSrc-Դ�������Ͻ�X����
//	IN nYSrc-Դ�������Ͻ�Y����
//	IN hbmMask-������ģ��λͼ
//	IN nXMask-��ģ���Ͻ�X����
//	IN nYMask-��ģ���Ͻ�Y����
//	IN dwRop-��դ������( ��֧��SRCCOPY)
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	������λλͼ����
// ����: 
//	ϵͳAPI
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
{	//�õ���ȫDC
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
		//׼��������ṹ
		blt.lpDestImage = _GetHBITMAPPtr( lpdcDest->hBitmap );
        blt.lpSrcImage = _GetHBITMAPPtr( lpdcSrc->hBitmap );
        blt.lpMaskImage = _GetHBITMAPPtr( hbmMask );
        blt.lprcSrc = &rcSrc;
        blt.lprcDest = &rcDest;
        blt.lprcMask = &rcMask;
        blt.solidColor = lpdcDest->textColor;
        blt.solidBkColor = lpdcDest->backColor;
        blt.dwRop = dwRop;
		//���Ŀ���Դ���εĵ��ص�����
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
        {	//Դ��Ŀ������ͬ����ʾ��
            while( lprnNode )
            {	//���ÿһ���ü����򣬿��Ƿ��ڽ�
                if( IntersectRect( &rcDest, &rcClip, &lprnNode->rect ) )
                {	//���û��Ʒ���Ϊ�����ң����ϵ���
                    rcMask = rcSrc = rcDest;
                    OffsetRect( &rcSrc, xOffSrc, yOffSrc );
                    OffsetRect( &rcMask, xOffMask, yOffMask );

                    blt.yPositive = 1;	//���ϵ���
                    blt.xPositive = 1;	//������

                    if( IntersectRect( &rcTemp, &rcSrc, &rcDest ) )
                    {	//��Ϊ����ͬ����ʾ�棬���Ա���ȷ����ȷ�Ŀ���˳��
						//�����ʾĿ�������Դ�������ص����ұ��뷴�򿽱�
                        if( rcSrc.top  < blt.lprcDest->top )
                            blt.yPositive = 0;	//���µ���
                        if( rcSrc.left  < blt.lprcDest->left )
                            blt.xPositive = 0;	//���ҵ���
                    }
                    lpDispDrv->lpBlkBitMaskBlt( &blt );
                }
                lprnNode = lprnNode->lpNext;
            }
        }
        else
        {	//��ͬ��ʾ��
			if( ( blt.lpDestImage->bmBitsPixel == blt.lpSrcImage->bmBitsPixel ||
				blt.lpSrcImage->bmBitsPixel == 1 ) )
			{	//���ݸ�ʽ��ͬ����Դ��ʽ�Ǻڰ�λͼ��ʽ				
				//���û��Ʒ���Ϊ�����ң����ϵ���
				
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
			{   // ��ͬ�ĸ�ʽ�� ��Ե㿽�� pixel -> pixel
				_LPCDISPLAYDRV lpSrcDispDrv = lpdcSrc->lpDispDrv;
				_PIXELDATA pxSrc, pxDest, pxMask;
				UINT uiRopSrc, uiRopDest;
				
	            pxSrc.lpDestImage = blt.lpSrcImage;
	            pxSrc.pattern = 0xff;
	            pxSrc.rop = R2_NOP;  // ������ read only

				pxMask.lpDestImage = blt.lpMaskImage;
	            pxMask.pattern = 0xff;
	            pxMask.rop = R2_NOP; // ������  read only

				pxDest.lpDestImage = blt.lpDestImage;
	            pxDest.pattern = 0xff;
				//׼����������
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
				//��ÿһ���ü�������в���
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
// ������BOOL WINAPI WinGdi_DrawEdge( HDC hdc, LPCRECT lpRect, UINT edgeType, UINT flags )
// ������
//	IN hdc-Ŀ��DC
//	IN lpcrc-�߿�RECT�ṹָ��
//	IN uType-����߿����״��������
//	    BDR_RAISEDOUTER-��߿���͹��
//		BDR_SUNKENOUTER-��߿��ǰ���
//		BDR_RAISEDINNER-�ڱ߿���͹��
//		BDR_SUNKENINNER-�ڱ߿��ǰ���
//		��Ҳ����������ϣ�
//		EDGE_RAISED-(BDR_RAISEDOUTER | BDR_RAISEDINNER)
//      EDGE_SUNKEN-(BDR_SUNKENOUTER | BDR_SUNKENINNER)
//      EDGE_ETCHED-(BDR_SUNKENOUTER | BDR_RAISEDINNER)
//      EDGE_BUMP-(BDR_RAISEDOUTER | BDR_SUNKENINNER) 
//	IN uFlags-�߿�����
//		BF_DIAGONAL-�Խ���
//		BF_MIDDLE-���߿��ڲ�
//		BF_LEFT-��߿�
//		BF_TOP-���߿�
//		BF_RIGHT-�ұ߿�
//		BF_BOTTOM-�ױ߿�
//		��Ҳ����������ϣ�
//		BF_TOPLEFT-(BF_TOP | BF_LEFT)
//      BF_BOTTOMRIGHT-(BF_BOTTOM | BF_RIGHT)
//      BF_RECT-(BF_TOP | BF_LEFT | BF_BOTTOM | BF_RIGHT)
//      BF_DIAGONAL_ENDTOPRIGHT-(BF_DIAGONAL | BF_TOP | BF_RIGHT)
//      BF_DIAGONAL_ENDTOPLEFT-(BF_DIAGONAL | BF_TOP | BF_LEFT)
//      BF_DIAGONAL_ENDBOTTOMLEFT-(BF_DIAGONAL | BF_BOTTOM | BF_LEFT)
//      BF_DIAGONAL_ENDBOTTOMRIGHT-(BF_DIAGONAL | BF_BOTTOM | BF_RIGHT)


// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�ò�ͬ��ɫ���߿��Բ�����͹��
// ����: 
//	ϵͳAPI
// ************************************************

BOOL WINAPI WinGdi_DrawEdge( HDC hdc, LPCRECT lpRect, UINT edgeType, UINT flags )
{	//�õ���ȫDC
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
		//�õ�ÿ���ߵ���ɫ
		clBorder[0][0] = lpdc->lpDispDrv->lpRealizeColor( WinSys_GetColor(COLOR_BTNHILIGHT), lpdc->lpdwPal, lpdc->wPalNumber, lpdc->wPalFormat );//LN:2003-05-06,���� GetSysColor Ϊ WinSys_GetColor
        clBorder[0][1] = lpdc->lpDispDrv->lpRealizeColor( WinSys_GetColor(COLOR_3DDKSHADOW), lpdc->lpdwPal, lpdc->wPalNumber, lpdc->wPalFormat );//LN:2003-05-06,���� GetSysColor Ϊ WinSys_GetColor
		clBorder[1][0] = lpdc->lpDispDrv->lpRealizeColor( WinSys_GetColor(COLOR_3DLIGHT), lpdc->lpdwPal, lpdc->wPalNumber, lpdc->wPalFormat );//LN:2003-05-06,���� GetSysColor Ϊ WinSys_GetColor
        clBorder[1][1] = lpdc->lpDispDrv->lpRealizeColor( WinSys_GetColor(COLOR_BTNSHADOW), lpdc->lpdwPal, lpdc->wPalNumber, lpdc->wPalFormat );//LN:2003-05-06,���� GetSysColor Ϊ WinSys_GetColor
        
		//�õ���ʾ��
        if( (lpBitmap = _GetHBITMAPPtr( lpdc->hBitmap )) != 0 )
        {
            lprNodes = _GetHRGNPtr( lpdc->hrgn )->lpNodeFirst;
			//�߼����굽�豸����
            OffsetRect( &rect, XOFFSET( lpdc ), YOFFSET( lpdc ) );

            if( flags & BF_MIDDLE )
            {   // ������� clear background
                WinGdi_FillRect( hdc, lpRect, WinSys_GetColorBrush( COLOR_BTNFACE ) );
            }
			//׼�����߽ṹ
            lineData.backMode = OPAQUE;
            lineData.clrBack = 0;            
            lineData.lpDestImage = lpBitmap;
            lineData.pattern = 0xff;
            lineData.rop = R2_COPYPEN;
            lineData.width = 1;
			lineData.lpfnCallback = NULL;

            if( flags & BF_DIAGONAL )
            {	//б��
                if( (flags & BF_TOPLEFT) == BF_TOPLEFT ||
                    (flags & BF_BOTTOMRIGHT) == BF_BOTTOMRIGHT )
                {
                    if( edgeType & BDR_OUTER )
                    {	//��������
                        if( (edgeType & BDR_RAISEDOUTER) )
                            lineData.color = clBorder[0][0];
                        else
                            lineData.color = clBorder[0][1];
                        _DrawLine( lpdc->lpDispDrv, lprNodes, &lineData, rect.left, rect.top, rect.right, rect.bottom );
                    }
                    if( edgeType & BDR_INNER )
                    {	//��������
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
                        {	//͹
#ifdef COLOR_1BPP
                            lineData.color = clBorder[i][1];
                            lineData.pattern = 0xaa;
#else
                            lineData.color = clBorder[i][0];
#endif
                        }
                        else
                        {	//��
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
                        {	//͹
#ifdef COLOR_1BPP
                            lineData.pattern = 0xff;
#endif
                            lineData.color = clBorder[i][1];
                        }
                        else
                        {	//��
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
// ������BOOL WINAPI WinGdi_DrawIconEx( HDC hdc, 
//										int x, 
//										int y, 
//										HICON hIcon,
//										int cxWidth, 
//										int cyWidth, 
//										UINT indexAnimal, 
//										HBRUSH hBrush, 
//										UINT uiFlags )
// ������
//	IN hdc - Ŀ��DC
//	IN x - ��Ҫ��������Ͻ�x����
//	IN y - ��Ҫ��������Ͻ�y����
//	IN hIcon - ��Ҫ�����ICONͼ��
//	IN cxWidth - ���ͼ����
//	IN cyWidth - ���ͼ��߶�
//	IN uiIndexAnimal - ����Ƕ���ICON����ʾ������ֵ��Ŀǰ��֧�֣�����ΪNULL��
//	IN hBrush - ˢ�ӣ�Ŀǰ��֧�֣�����ΪNULL��
//	IN uiFlags - ������ܣ�����ΪDI_NORMAL��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	��ͼ��ICON
// ����: 
//	ϵͳAPI
// ************************************************

BOOL WINAPI WinGdi_DrawIconEx( HDC hdc, int x, int y, HICON hIcon, int cxWidth, int cyWidth, UINT indexAnimal, HBRUSH hBrush, UINT uiFlags )
{	//�ɾ���õ�DCָ��
    //_LPGDCDATA lpdc = _GetHDCPtr( hdc );
	BOOL ret = FALSE;

    //if( lpdc && hIcon )
	if( hIcon )
    {
        ICONINFO icfo;
		//�õ�ICON��Ϣ
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
			//�õ�ICON��λͼ����
            WinGdi_GetObjectInfo( icfo.hbmColor, sizeof(bmpInfo), &bmpInfo );
			//����һ���ڴ�DC
			hMemDC = WinGdi_CreateCompatibleDC( hdc );
			if( hMemDC )
			{	//�����ɹ�
				HBITMAP hOld;
				hOld = WinGdi_SelectObject( hMemDC, icfo.hbmColor );
				if( hOld )
				{	//�õ���Ⱥ͸߶�
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
					//����
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
				//ɾ���ڴ�DC
				WinGdi_DeleteDC( hMemDC );
			}
		}
    }
    return ret;
}

// **************************************************
// ������BOOL WINAPI WinGdi_DrawIcon( HDC hdc, int x, int y, HICON hIcon )
// ������
// 	IN hdc-Ŀ��DC���
// 	IN nX-�����X����
// 	IN nY-�����Y����
// 	IN hIcon-ICON���
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	��ͼ��
// ����: 
//	ϵͳAPI
// ************************************************

BOOL WINAPI WinGdi_DrawIcon( HDC hdc, int x, int y, HICON hIcon )
{
	return WinGdi_DrawIconEx( hdc, x, y, hIcon, 0, 0, 0, NULL, DI_NORMAL );
}



// **************************************************
// ������static int _ScanText( _LPFONT lpFont, LPCSTR lpstr, UINT uFormat, int* lpxWidth, int* lpwCount, int xClip, int nCount, int * lpEllipsisPos )
// ������
// 	IN lpFont - �������
// 	IN lpstr - �����ַ�����ָ��
// 	IN uFormat - ɨ�蹦�ܣ�
//				DT_SINGLELINE - �����ı�
//				DT_NOPREFIX - û��ǰ׺���� &Hello, '&'��ǰ׺�� )  
//				DT_TABSTOP - �趨TAB�ַ���,Ĭ��Ϊ8���ַ���
//				DT_END_ELLIPSIS - ��ʡ�Ժű�ʾ�޷��ڲü�����ʾ���ַ�
// 	OUT lpxWidth - ���ڽ����ַ�����ˮƽ���
// 	IN lpwCount - ���ڽ����ַ�������
// 	IN xClip - �ü����(��ʾ���ַ������ܳ����ÿ�ȣ�
// 	IN nCount - �ַ�����ָ��ָ����ַ�����
// 	IN lpEllipsisPos - ʡ�Ժ�λ��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	ɨ���ַ������õ��ڲü���ȷ�Χ�ڵ��ַ�����
// ����: 
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
	int precw; //ǰһ���ַ���bytes��
	_LPFONTDRV lpFontDrv = lpFont->lpDriver;
	HANDLE hFont = lpFont->handle;

	DEBUGMSG( DEBUG_SCANTEXT, ( "xClip=%d,nCount=%d.\r\n",xClip, nCount ) );
	//���趨Ĭ��ֵ
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
        {	//�ǵ���
            (*lpwCount)+=2;
            break;
        }
	    if( *lpstr == TEXT_PREFIX_WORD && (!(uFormat & DT_NOPREFIX)) && firstPreFix == 0 )
	    {	//��ǰ׺
	        firstPreFix = 1 - firstPreFix;
	    }
	    else if( *lpstr  == TAB_CHAR )
	    {	//Ϊtab�ַ�
	        if( uFormat & DT_TABSTOP )
		        xWidth = HIBYTE(uFormat)*DFLT_TAB_WIDTH;
	        else
		        xWidth = DFLT_TAB*DFLT_TAB_WIDTH;
	        firstPreFix = 0;
	    }
	    else
        {	//ͨ�����ַ�
            cw = lpFontDrv->lpWordBitmap( hFont, (LPCBYTE)lpstr, nCount - *lpwCount, &textBitmap );
            xWidth = textBitmap.xAdvance;
	        firstPreFix = 0;
	    }
		if( (*lpxWidth + xWidth) > xClip )
		{	//�����˲ü���
			if( (uFormat & DT_SINGLELINE) && 
				(uFormat & DT_END_ELLIPSIS) == 0 )
			{	//������ʡ�Ժ�
				(*lpxWidth) += xWidth;
				(*lpwCount) += cw;
			}
			else
			{	//
				if( *lpwCount == 0 && xClip > 0 )
				{	//��
					(*lpxWidth) += xWidth;
					(*lpwCount) += cw;
				}
				if( (uFormat & DT_END_ELLIPSIS) && *lpwCount > cw )
				{
					//*lpEllipsisPos = *lpwCount - cw;//ʡ�Ժ�λ��
					if( *lpxWidth + cwEllipsis > xClip )
					{	//�����������ʾ "..."
						*lpEllipsisPos = *lpwCount - precw;//ʡ�Ժ�λ��						
					    *lpxWidth = *lpxWidth - prexWidth + cwEllipsis;
						*lpxWidth = MIN( *lpxWidth, xClip );
					}
					else
					{	//������ʾ "..."
						*lpEllipsisPos = *lpwCount;//ʡ�Ժ�λ��
						(*lpwCount) ++;	//����ʡ�Ժŵ���ʾ�ַ���
						(*lpxWidth) += cwEllipsis;//����ʡ�Ժŵ���ʾ���
						
					}
				}
			}
			break;
		}
		//��ʵ����ʾ���
	    (*lpxWidth) += xWidth;
		//��ʵ����ʾ��
	    (*lpwCount) += cw;
		//��һ���ַ�
	    lpstr += cw;
    }
    return *lpwCount;
}

// **************************************************
// ������int WINAPI WinGdi_DrawText( HDC hdc, LPCTSTR lpstr, int nCount, LPRECT lpRect, UINT uFormat )
// ������
// 	IN hdc - ��ͼDC���
// 	IN lpstr - ��Ҫ��ʾ���ַ���
// 	IN nCount-����ַ�����, �������-1, lpcstr������'\0'����
// 	IN lpcrc-RECT�ṹָ��,����ַ����ڸþ����ڽ��и�ʽ��
// 	IN uFormat-��ʽ����־,������
// 	        DT_TOP-�϶���(�������ı�DT_SINGLELINE)
// 	        DT_LEFT-�����(Ĭ��)
// 	        DT_SINGLELINE-�����ı�
// 	        DT_BOTTOM-�׶˶���(�������ı�DT_SINGLELINE)
// 	        DT_VCENTER-��ֱ����(�������ı�DT_SINGLELINE)
// 	        DT_RIGHT-�Ҷ���
// 	        DT_CENTER-ˮƽ����
// 	        DT_TABSTOP-�趨TAB�ַ���,Ĭ��Ϊ8���ַ���
// 	        DT_NOPREFIX-�ر�ǰ׺��
// 	        DT_NOCLIP-���ü�
// 	        DT_WORDBREAK-�Զ�����
//			DT_END_ELLIPSIS - ��ʡ�Ժű�ʾ������ʾ���ַ�
// ����ֵ��
//	�ɹ������ػ��Ƶ��ı��߶ȣ�ʧ�ܣ�����FALSE
// ����������
//	��ʽ���ı����
// ����: 
//	ϵͳAPI
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
		//�õ�λͼָ��
        lpBitmap = _GetHBITMAPPtr( lpdc->hBitmap );
        if( lpBitmap == 0 )
            goto _return;//return FALSE;
		//�߼����굽�豸����
        OffsetRect( &rectClip, xoff, yoff );
		//������ܣ��õ��ַ�������
        if( nCount <= 0 )
	        nCount = strlen( lpstr );
        y = rectClip.top;
        if( uFormat & DT_SINGLELINE )
        {	//����
	        if( uFormat & DT_BOTTOM )
	            y = rectClip.bottom - fh;
	        else if( uFormat & DT_VCENTER )
	            y = rectClip.top + ( (rectClip.bottom-rectClip.top) - fh ) / 2;
        }
		//2004-06-15,���Ӷ� DT_END_ELLIPSIS �Ĵ���
		//
		if( (uFormat & DT_CALCRECT) && 
			(uFormat & DT_SINGLELINE) &&
			(uFormat & DT_END_ELLIPSIS) == 0 )
			cw = 0x7fff;		//ͳ����ʾ���ζ�����ʾ
		else
			cw = lpRect->right - lpRect->left;
		//
		//��ʼ������ƽṹ
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
		//��ʼ���߻��ƽṹ
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
		//��ÿһ���ַ�
        while( nCount )
        {	// ɨ���ַ������õ�һ��ʾ�� get new line			
	        if( _ScanText( lpFont, lpstr, uFormat, &xWidth, &wCount, cw, nCount, &iEllipsis ) == 0 )   // is eof?
	            break;		//����ַ����������˳�

			if( uFormat & DT_CALCRECT ) 
			{   // ����ͳ����ʾ���Σ�only calc the rect bound, no thing to do
				if( iTextWidth < xWidth )
					iTextWidth = xWidth;
				iTextHeight += fh;
			    nCount -= wCount;
				lpstr += wCount;

				if( uFormat & DT_SINGLELINE )
					break;	//���У���ͳ���꣬�˳�
				continue;
			}
				
	        if( uFormat & DT_RIGHT )
	            x = rectClip.right - xWidth;	//�Ҷ���
	        else if( uFormat & DT_CENTER )
                x = rectClip.left + (cw - xWidth) / 2;	//����
	        else
                x = rectClip.left;		//Ĭ�������

			// ���´��봦��һ�е��ַ������� draw a line
            num = 0;
	        while( num < wCount )
            {    // �Ƿ�Ϊtab�ַ� is tab char
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
				// �Ƿ�Ϊ���б�־ is break char
	            if( IS_LINE_MARK( *lpstr ) )
	            {   // ׼����һ�� clear line
		            lpstr++;
		            num++;
                    continue;
                }
				// �Ƿ�Ϊǰ׺��־ is prefix char
	            if( *lpstr == TEXT_PREFIX_WORD )
                {
		            if( !(uFormat & DT_NOPREFIX) )
                    {	//����ǰ׺��־
                        lpstr++;
                        num++;
		                fUnderLine = TRUE;
		            }
	            }
				// ��ʾһ���ַ� draw one word's mask
				if( uFormat & DT_END_ELLIPSIS )
				{	//��ʡ�Ժŷ��
					TCHAR bEllipsis[4];
					bEllipsis[0] = 0;
					if( num && num == iEllipsis )
					{	// ��ǰΪһ�еĽ��� line end
						if( *(lpstr+1) != 0 )	//�պý����� ��
						{	//��
							if( (uFormat & DT_SINGLELINE) ||
								(y + fh >= rectClip.bottom) )
							{	// �������㣬��һ��ʡ�Ժ�
								// yes, drawELLIPSIS
								//bEllipsis[0] = (char)'��';
								bEllipsis[0]= (BYTE)'��'; //δ�˵õ��ո�Ŀ��
								bEllipsis[1]= 0;
								//bEllipsis[2]= 0;
								num = wCount;
							}
						}
					}
					if( bEllipsis[0] )
					{	//��Ҫ��ʡ�Ժ�,�õ��ַ��� ��ģ
						bw = lpFontDrv->lpWordBitmap( hFont, (LPCBYTE)bEllipsis, wCount - num, &textBitmap );
					}
					else 
						bw = lpFontDrv->lpWordBitmap( hFont, (LPCBYTE)lpstr, wCount - num, &textBitmap );//�õ��ַ��� ��ģ
					// handle the DT_END_ELLIPSIS end
				}
				else
                    bw = lpFontDrv->lpWordBitmap( hFont, (LPCBYTE)lpstr, wCount - num, &textBitmap );//�õ��ַ��� ��ģ


				rCharBox.left = x;
				rCharBox.top = y;
				rCharBox.right = rCharBox.left + textBitmap.xAdvance;
				rCharBox.bottom = rCharBox.top + fh;//yTextHeight;//


				//�õ��ַ���ʾ��λ��
                xMaskOff = r.left = x + textBitmap.left;
				yMaskOff = r.top = y + textBitmap.top; 

                r.right = r.left + textBitmap.bitmap.bmWidth; 
                r.bottom = r.top + textBitmap.bitmap.bmHeight;
                yUnderLine = r.bottom;
				//��ü�����
                //if( IntersectRect( &rectSave, &r, &lprgn->rect ) )
				if( IntersectRect( &rectSave, &rCharBox, &lprgn->rect ) )
				
                {   // in rgn box, now decice rect which intersect with word
	                lprNodes = lprgn->lpNodeFirst;
	                if( !(uFormat & DT_NOCLIP) )
		                IntersectRect( &rectSave, &rectSave, &rectClip );
					//��ÿһ���ü���������ʾ�ַ������󽻣����������ʾ�������
                    while( lprNodes )
	                {
		                //if( IntersectRect( &r, &rectSave, &lprNodes->rect ) )
						if( IntersectRect( &rCharBox, &rectSave, &lprNodes->rect ) )						
		                {   // ����ʾ���� not empty
                            //j = r.top - y;
							BOOL bShowBitmap;
							
							bShowBitmap = IntersectRect( &rCharBitmap, &r, &rCharBox );
                            rMask = rCharBitmap;
                            OffsetRect( &rMask, -xMaskOff, -yMaskOff );

                            if( bm )
                                //lpDispDrv->lpBlkBitBlt( &textBitblt );
								lpDispDrv->lpBlkBitBlt( &bkBitblt );
                            //else	//͸��
							if( bShowBitmap )
                                lpDispDrv->lpBlkBitTransparentBlt( &textBitblt );

                            // ������Ҫ���»��ߣ���֮ draw under line if possible
                            if( fUnderLine && r.bottom == yUnderLine )
                            {
                                lineData.xStart = (short)r.left;
                                lineData.yStart = (short)yUnderLine;//r.top;2004-09-15,����������
								lineData.cPels = r.right - r.left;
                                lineData.lprcClip = &r;
                                lpDispDrv->lpLine( &lineData );                                
                            }
		                }
						//��һ���ü�����
		                lprNodes = lprNodes->lpNext;
	                }
                }
                fUnderLine = FALSE;
				// ׼����һ����ʾ�ַ� set next word
                x += textBitmap.xAdvance;//bitmap.bmWidth;
                lpstr += bw;
                num += bw;
            }
			// set next line y position
			//׼����һ����ʾ�ַ�
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
// ������static BOOL _ExtTextOut( 
//							_LPGDCDATA lpdc,
//							int x,
//							int y,
//							UINT option,
//							LPCRECT lpRect,
//							LPCSTR lpstr,
//							UINT count,
//							int * lpNextPos )

// ������
// 	IN lpdc - ��ͼDCָ��
//	IN x - �ı���ʾ��ʼ��x����
//	IN y - �ı���ʾ��ʼ��y����
//	IN option-��lpcrc�ķ���,����:
//			ETO_CLIPPED-�ı���lpcrc�ü�,����������ı�������ʾ
//			ETO_OPAQUE-����ʾ�ı�ǰ���õ�ǰ����ɫ���
//	IN lpRect-RECT�ṹָ��
//	IN lpstr-����ַ���
//	IN nCount-����ַ�����
//	IN lpNextPos-��һ���ַ����λ��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�ı����
// ����: 
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
		{   //����ԭʼ�� x, y
		    x = lpdc->position.x;
			y = lpdc->position.y;
		}

        x += xoff;
        y += yoff;
        if( option & (ETO_CLIPPED|ETO_OPAQUE) )
        {	//��Ҫ���ü�����Ҫ�������
            rectClip = *lpRect;
			//�߼����굽�豸����
            OffsetRect( &rectClip, xoff, yoff );
        }

		// clear background if possible
    
        if( option & ETO_OPAQUE )
        {   // ������� clear background
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

        // ����û�ָ������ʾλ�ã�������� align border
		cw = 0;
		if( lpNextPos )
		{
			for( j = 0; (UINT)j < count; j++ )
				cw += *lpNextPos++;
		}
        if( (lpdc->textAlign & TA_RIGHT) ||
			(lpdc->textAlign & TA_CENTER)  )
		{	//�����Ҷ���;��ж���
			int w = lpFontDrv->lpTextWidth( hFont, (LPCBYTE)lpstr, count ) + cw;
			if( (lpdc->textAlign & TA_RIGHT) )
	            x -= w;
			else  // ta_center
				x -= w / 2;
		}
		//�׶���
		yTextHeight = lpFontDrv->lpMaxHeight( hFont );
        if( lpdc->textAlign & TA_BOTTOM )
	        y -= yTextHeight;
		//��ʼ�����߲����ṹ
        lineData.rop = R2_COPYPEN;
        lineData.lpDestImage = lpBitmap;
		lineData.lpfnCallback = NULL;

        textBitmap.bitmap.bmPlanes = 1;
        textBitmap.bitmap.bmBitsPixel = 1;
		//��ʼ����������ṹ
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
        {	//�õ���ģ
			int xMaskOff, yMaskOff;

            cw = lpFontDrv->lpWordBitmap( hFont, (LPCBYTE)lpstr, count, &textBitmap );
			//�õ��ַ�����ʾ����
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

			//�õ���ʾ�棬�Ƿ�����ʾ��
            //if( IntersectRect( &rectSave, &r, &lprgn->rect ) )
			if( IntersectRect( &rectSave, &rCharBox, &lprgn->rect ) )
            {   // in rgn box, now decice rect which intersect with word
	            lprNodes = lprgn->lpNodeFirst;
	            if( option & ETO_CLIPPED )
				{
		            IntersectRect( &rectSave, &rectSave, &rectClip );
				}
				//��ÿһ���ü��������ַ��󽻣����������ʾ������ʾ֮
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
						//��ʾ
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
			//׼����һ���ַ�
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
// ������BOOL WINAPI WinGdi_ExtTextOut( HDC hdc, 
//							  int x, 
//							  int y,
//							  UINT option,
//							  LPCRECT lpRect,
//							  LPCTSTR lpstr,
//							  UINT count,
//							  int * lpNextPos )
// ������
//	IN hdc-Ŀ��DC
//	IN x-�����X����
//	IN y-�����Y����
//	IN option-��lpcrc�ķ���
//			ETO_CLIPPED-�ı���lpcrc�ü�,����������ı�������ʾ
//			ETO_OPAQUE-����ʾ�ı�ǰ���õ�ǰ����ɫ���
//	IN lpRect-RECT�ṹָ��
//	IN lpstr-����ַ���
//	IN count-����ַ�����
//	IN lpNextPos-��һ���ַ����λ��

// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�ı����
// ����: 
//	ϵͳAPI
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
	//�õ���ȫ��DCָ��
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
// ������BOOL WINAPI WinGdi_TextOut( HDC hdc, int x, int y, LPCTSTR lpString, int count )
// ������
//	IN hdc-Ŀ��DC
//	IN x-�����X����
//	IN y-�����Y����
//	IN lpString-����ַ���
//	IN count-����ַ�����
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�ı����
// ����: 
//	ϵͳAPI
// ************************************************

BOOL WINAPI WinGdi_TextOut( HDC hdc, int x, int y, LPCTSTR lpString, int count )
{
    _LPGDCDATA lpdc = NULL;
	BOOL retv = FALSE;

    if( lpString && 
		count > 0 &&
		(lpdc = _GetSafeDrawPtr( hdc ) ) != NULL )
    {	//�Ƿ�͸����� ��
        if( lpdc->backMode == OPAQUE )
	        retv = _ExtTextOut( lpdc, x, y, ETO_OPAQUE, 0, lpString, count, 0 );
        else	//��
	        retv = _ExtTextOut( lpdc, x, y, 0, 0, lpString, count, 0 );
    }
	_LeaveDCPtr( lpdc, DCF_WRITE );
	return retv;
    //return FALSE;
}

// **************************************************
// ������BOOL WINAPI WinGdi_TextOutW(
//							HDC hdc,
//							int x,
//							int y,
//							LPCTSTR lpString,
//							int nCount
//							)
// ������
//	IN hdc-Ŀ��DC
//	IN x-�����x����
//	IN y-�����y����
//	IN lpcstr-����ַ�����unicode ��ʽ��
//	IN nCount-����ַ�����
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�ı����, uincode �汾
// ����: 
//	ϵͳAPI
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
	//������
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
			int k = MIN( nCount, 64 );	//ÿ�δ������64���ֽ� cache 
			int nByte;
			BYTE bString[256];	//ÿ�δ���64���ֽ� cache 
			//��ת��Ϊ���ֽ�
			if( 0 != ( nByte = WideCharToMultiByte( CP_GB2312, 0, lpcwsStr, k, (LPSTR)bString, sizeof(bString), NULL, NULL ) ) )
			{	//���
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
// ������COLORREF WINAPI WinGdi_GetTextColor( HDC hdc )
// ������
//	IN hdc-Ŀ��DC���
// ����ֵ��
//	����ɹ��������ı���ɫ�����򣬷���CLR_INVALID
// ����������
//	�õ��ı���ɫ
// ����: 
//	ϵͳAPI
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
// ������UINT WINAPI WinGdi_GetTextAlign( HDC hdc )
// ������
//	IN hdc-Ŀ��DC���
// ����ֵ��
//	����ɹ�����������ֵ����ϣ�
//		TA_BASELINE �C �ο������ı��Ļ���
//		TA_BOTTOM �C �ο����Ǿ��εĵױ�
//		TA_TOP - �ο����Ǿ��εĶ���
//		TA_CENTER - �ο����Ǿ������ĵ�ˮƽ��
//		TA_LEFT - �ο����Ǿ��ε����
//		TA_RIGHT - �ο����Ǿ��ε��ұ�
//		TA_NOUPDATECP �C ��ǰ���������ÿһ���ı�����󲻻����
//		TA_UPDATECP �C ��ǰ���������ÿһ���ı����������
//	���粻�ɹ�������GDI_ERROR
// ����������
//	�õ���ǰ�ı����뷽ʽ
// ����: 
//	ϵͳAPI
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
// ������static BOOL _GetTextExtentPoint32( 
//								HFONT hFont, 
//								_LPFONTDRV lpFontDrv, 
//                              LPCTSTR lpcstr, 
//								int nCount,
//								LPSIZE lpSize )
// ������
//	IN hFont - ���������
//	IN lpFontDrv - ������������
//	IN lpcstr - �ı�ָ��(����'\0'Ϊ��������,nLength�����ı�����)
//	IN nCount - �ı�����
//	OUT lpSize - SIZE�ṹָ��,�����ı��ĳ��ȺͿ��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�����ı��ĳ��ȺͿ��
// ����: 
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
	//�ȳ�ʼ��Ϊ0
	lpSize->cx = lpSize->cy = 0;
	//��ÿһ���ַ��ĳߴ磬�������ۼ�
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
// ������BOOL WINAPI WinGdi_GetTextExtentPoint32( HDC hdc, LPCTSTR lpcstr, int nCount, LPSIZE lpSize )
// ������
//	IN hdc-������
//	IN lpcstr-�ı�ָ��(����'\0'Ϊ��������,nLength�����ı�����)
//	IN nCount-�ı�����
//	OUT lpSize-SIZE�ṹָ��,�����ı��ĳ��ȺͿ��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�����ı��ĳ��ȺͿ��
// ����: 
//	ϵͳAPI
// ************************************************

BOOL WINAPI WinGdi_GetTextExtentPoint32( HDC hdc, LPCTSTR lpcstr, int nCount, LPSIZE lpSize )
{
    _LPGDCDATA lpdc;
	//�õ���Ч��DC ָ��
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
// ������BOOL WINAPI WinGdi_GetTextExtentPoint32W( HDC hdc, LPCTSTR lpcstr, int nCount, LPSIZE lpSize )
// ������
//	IN hdc-������
//	IN lpcstr-�ı�ָ��(����'\0'Ϊ��������,nLength�����ı����ȣ� unicode��ʽ)
//	IN nCount-�ı�����
//	OUT lpSize-SIZE�ṹָ��,�����ı��ĳ��ȺͿ��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�����ı��ĳ��ȺͿ�ȣ� unicode �汾 ��
// ����: 
//	ϵͳAPI
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
			int k = MIN( nCount, 64 );	//���64���ֽ�
			int nByte;
			//�Ƚ�unicode��ʽת��Ϊ���ֽڸ�ʽ
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
// ������BOOL WINAPI WinGdi_GetTextExtentExPoint( 
//									HDC hdc,
//									LPCTSTR lpszStr,
//									int nString,
//									int nMaxExtent,
//									LPINT lpFit,
//									LPINT lpDx,
//									LPSIZE lpSize )
// ������
//	IN hdc-������
//	IN lpszStr-�ı�ָ��(����'\0'Ϊ��������,nLength�����ı�����)
//	IN nString-�ı�����
//	IN nMaxExtent-�ı��������ɼ�����
//	OUT lpFit-���ڽ������Ƴ����ڵ��ı��ַ�����
//	OUT lpDx-���ڽ��ܵ�һ���ַ�����N�ַ����ı��ĳ���
//	OUT lpSize-SIZE�ṹָ��,���ڽ����ı��ĳ��ȺͿ��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�����ı��ĳ��ȺͿ��
// ����: 
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
			//�õ��ַ����ԣ���͸ߣ�
			ws = lpFontDrv->lpWordBitmap( hFont, (LPCBYTE)lpszStr, nString, &textBitmap );
			//�ۼ���ص�ֵ
            lpSize->cx += textBitmap.xAdvance;//bitmap.bmWidth;
            v = textBitmap.bitmap.bmHeight;
            lpSize->cy = MAX( v, lpSize->cy );
			
            if( lpDx )
                *lpDx++ = lpSize->cx;
            if( lpFit )
                (*lpFit)++;
			//��һ���ַ�
			nString -= ws;
			lpszStr += ws;
        }
        return TRUE;
    }
    return FALSE;
}

// **************************************************
// ������COLORREF WINAPI WinGdi_SetTextColor( HDC hdc, COLORREF color )
// ������
// 	IN hdc - DC���
//	IN color - RGB��ɫֵ
// ����ֵ��
//	�ɹ��������ϵ�RGBֵ
//	���򣺷���CLR_INVALID
// ����������
//	�����ı���ɫ
// ����: 
//	
// ************************************************

COLORREF WINAPI WinGdi_SetTextColor( HDC hdc, COLORREF color )
{
    _LPGDCDATA lpdc=_GetHDCPtr( hdc );

    if( lpdc && lpdc->lpDispDrv )
    {
        int c = lpdc->textColor;
		//�õ��豸��������ɫֵ
        lpdc->textColor = lpdc->lpDispDrv->lpRealizeColor(color, lpdc->lpdwPal, lpdc->wPalNumber, lpdc->wPalFormat);
		//�õ��豸����������ɫֵ��RGB��
        return lpdc->lpDispDrv->lpUnrealizeColor(c, lpdc->lpdwPal, lpdc->wPalNumber, lpdc->wPalFormat);
    }
    return CLR_INVALID;
}

// **************************************************
// ������UINT WINAPI WinGdi_SetTextAlign( HDC hdc, UINT uiAlignMode )
// ������
//	IN hdc - DC���
//	IN uiAlignMode �C �µĶ���ģʽ������Ϊ����ֵ��
//			TA_BASELINE �C �ο������ı��Ļ���
//			TA_BOTTOM �C �ο����Ǿ��εĵױ�
//			TA_TOP - �ο����Ǿ��εĶ���
//			TA_CENTER - �ο����Ǿ������ĵ�ˮƽ��
//			TA_LEFT - �ο����Ǿ��ε����
//			TA_RIGHT - �ο����Ǿ��ε��ұ�
//			TA_NOUPDATECP �C ��ǰ���������ÿһ���ı�����󲻻����
//			TA_UPDATECP �C ��ǰ���������ÿһ���ı����������
// ����ֵ��
//	����ɹ�������֮ǰ��ģʽֵ�����粻�ɹ�������GDI_ERROR
// ����������
//	�����ı�����ģʽ
// ����: 
//	ϵͳAPI
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
// ������COLORREF WINAPI WinGdi_GetBkColor( HDC hdc )
// ������
//	IN hdc-Ŀ��DC
// ����ֵ��
//	�ɹ���������ɫ
//	���򣺷���CLR_INVALID
// ����������
//	�õ�����ɫ
// ����: 
//	ϵͳAPI
// ************************************************

COLORREF WINAPI WinGdi_GetBkColor( HDC hdc )
{
    _LPGDCDATA lpdc = _GetHDCPtr( hdc );

    if( lpdc && lpdc->lpDispDrv )
        return lpdc->lpDispDrv->lpUnrealizeColor(lpdc->backColor, lpdc->lpdwPal, lpdc->wPalNumber, lpdc->wPalFormat);//�õ��豸����������ɫֵ
     return CLR_INVALID;
}

// **************************************************
// ������COLORREF WINAPI WinGdi_SetBkColor( HDC hdc, COLORREF color )
// ������
//	IN hdc-Ŀ��DC
//	IN color-RGB��ɫ
// ����ֵ��
//	����ɹ�,�����ϵı���ɫ;����,����CLR_INVALID
// ����������
//	���ñ���ɫ
// ����: 
//	ϵͳAPI
// ************************************************

COLORREF WINAPI WinGdi_SetBkColor( HDC hdc, COLORREF color )
{
    _LPGDCDATA lpdc=_GetHDCPtr( hdc );

    if( lpdc && lpdc->lpDispDrv )
    {
        COLORREF c = lpdc->backColor;
		//�õ��豸��������ɫֵ
        lpdc->backColor = lpdc->lpDispDrv->lpRealizeColor(color, lpdc->lpdwPal, lpdc->wPalNumber, lpdc->wPalFormat);
        return lpdc->lpDispDrv->lpUnrealizeColor(c, lpdc->lpdwPal, lpdc->wPalNumber, lpdc->wPalFormat);//�����豸����������ɫֵ
    }
    return CLR_INVALID;
}

// **************************************************
// ������int WINAPI WinGdi_GetBkMode( HDC hdc )
// ������
// 	IN hdc-Ŀ��DC
// ����ֵ��
//	�ɹ������ر���ģʽ
//	���򣺷���0
// ����������
//	�õ�����ģʽ
// ����: 
//	ϵͳAPI
// ************************************************

int WINAPI WinGdi_GetBkMode( HDC hdc )
{
    _LPGDCDATA lpdc;

    if( (lpdc=_GetHDCPtr( hdc )) != 0 )
        return lpdc->backMode;
    return 0;
}

// **************************************************
// ������int WINAPI WinGdi_SetBkMode( HDC hdc, int iMode )
// ������
//	IN hdc-Ŀ��DC
//	IN mode-����ģʽ,������
//			OPAQUE-������ı�,ˢ��,��ʱ,�������õ�ǰ����ɫ���
//			TRANSPARENT-����ɫ���ֲ���
// ����ֵ��
//	����ɹ�,�����ϵı���ģʽ;����,����0
// ����������
//	���ñ���ģʽ
// ����: 
//	ϵͳAPI
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
// ������HGDIOBJ WINAPI WinGdi_SelectObject(HDC hdc, HGDIOBJ hgdi)
// ������
//	IN hdc-Ŀ��DC
//	IN handle-������ ,������
//			HBITMAP-λͼ����
//			HBRUSH-ˢ�Ӷ���
//			HFONT-�������
//			HPEN-�ʶ���
//			HRGN-�������
// ����ֵ��
//	�ɹ�:��ѡ���Ķ������������,������ͬ���͵��϶���
//	����,��������ֵ��
//	    SIMPLEREGION-������һ������
//	    COMPLEXREGION-�����ж������
//	    NULLREGION-����Ϊ��ʧ��:��ѡ���Ķ������������, ����GDI_ERROR;
//					����,����NULL
// ����������
//	������ѡ���豸
// ����: 
//	ϵͳAPI
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
        {	//�ʶ���
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
        {	//ˢ�Ӷ���
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
        {	//λͼ����
            if( lpdc->obj.objType == OBJ_MEMDC )
            {   // �ڴ�DC mem dc
				_LPCDISPLAYDRV lpdd = NULL;
                
				hold = lpdc->hBitmap;
				ASSERT( hold );

                lpBitmap = _GetHBITMAPPtr( hgdi );
				lpdd = GetDisplayDeviceDriver( lpBitmap );
				if( lpdd )
				{
					lpdc->hBitmap = hgdi;
					lpdc->lpDispDrv = lpdd;
					
					//���õ�ɫ��
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
								lpdc->wPalFormat = PAL_BITFIELD;  //��ɫֵΪλͼ��
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
					// ��Ҫ����Ϊ�豸������ɫֵ
                    lpdc->backColor = lpdd->lpRealizeColor( CL_WHITE, lpdc->lpdwPal, lpdc->wPalNumber, lpdc->wPalFormat );
                    lpdc->textColor = lpdd->lpRealizeColor( CL_BLACK, lpdc->lpdwPal, lpdc->wPalNumber, lpdc->wPalFormat );
					// ��Ҫ����Ϊ�豸������ɫֵ
					lpdc->brushAttrib.color = lpdd->lpRealizeColor( lpdc->brushAttrib.clrRef, lpdc->lpdwPal, lpdc->wPalNumber, lpdc->wPalFormat );
					lpdc->penAttrib.color = lpdd->lpRealizeColor( lpdc->penAttrib.clrRef, lpdc->lpdwPal, lpdc->wPalNumber, lpdc->wPalFormat );
					// ��Ҫ��ʾ�ü�����Ϊ������ʾ��
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
        {   //�ü�����
			hold = (HGDIOBJ)DoExtSelectClipRgn( hdc, hgdi, RGN_COPY );			
        }
        else if( type == OBJ_FONT )
        {	//�������
            hold = lpdc->hFont;
            lpdc->hFont = hgdi;
        }
        else
		{	//�Ƿ�����
            ASSERT( 0 );
			WARNMSG( DEBUG_SELECTOBJECT, ( "error in SelectObject:hgdi=0x%x.\r\n", hgdi ) );
		}
    }
    return hold;
}

// **************************************************
// ������static int DoExtSelectClipRgn( HDC hdc, HRGN hrgn, int mode )
// ������
//	IN hdc - DC���
//	IN hrgn �C ������
//	IN iMode �C �뵱ǰDC��Ĳü��������ģʽ������Ϊ����ֵ��
//			RGN_AND �C �µĲü�����Ϊ��ǰDC��Ĳü�������hrgn���С��롱��AND������
//			RGN_COPY - �µĲü�����Ϊhrgn�Ŀ���������hrgnΪNULL���µĲü�����Ϊ��
//			RGN_DIFF - �µĲü�����Ϊ��ǰDC��Ĳü������ȥhrgn��ʣ�µĲ���
//			RGN_OR -�µĲü�����Ϊ��ǰDC��Ĳü�������hrgn���С��򡱣�OR������
//			RGN_XOR -�µĲü�����Ϊ��ǰDC��Ĳü�������hrgn���С���򡱣�XOR������
// ����ֵ��
//	NULLREGION - �ü������ǿյ�
//	SIMPLEREGION - �ü�����ֻ��һ������
//	COMPLEXREGION - �ü����򺬶������
//	ERROR �C ����
// ����������
//	ѡ��ü�����DC
// ����: 
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
		{	//��DC���봰�ڹ����ģ������봰�ڵ���ʾ���������ز���
			if( (iretv = GetExposeRgn( lpdc->hwnd, lpdc->uiFlags, &hrgnExpose )) == ERROR )
			{
				WARNMSG( DEBUG_EXT_SELECT_CLIP_RGN, ( "error at DoExtSelectClipRgn.\r\n" ) );
				goto _return;
			}
			if( iretv == NULLREGION )
			{	//��ǰ�����޿���ʾ����ֱ�ӷ���
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
			{	//��hrgnת�����豸����
				WinRgn_Offset( hrgn, lpdc->deviceOrg.x, lpdc->deviceOrg.y );
				//��hrgnExpose�ཻ
				WinRgn_Combine( hrgnExpose, hrgnExpose, hrgn, RGN_AND );
				//�ָ�
				WinRgn_Offset( hrgn, -lpdc->deviceOrg.x, -lpdc->deviceOrg.y );
			}
			//��mode���в���
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
// ������int WINAPI WinGdi_ExtSelectClipRgn( HDC hdc, HRGN hrgn, int mode )
// ������
//	IN hdc - DC���
//	IN hrgn �C ������
//	IN mode �C �뵱ǰDC��Ĳü��������ģʽ������Ϊ����ֵ��
//			RGN_AND �C �µĲü�����Ϊ��ǰDC��Ĳü�������hrgn���С��롱��AND������
//			RGN_COPY - �µĲü�����Ϊhrgn�Ŀ���������hrgnΪNULL���µĲü�����Ϊ��
//			RGN_DIFF - �µĲü�����Ϊ��ǰDC��Ĳü������ȥhrgn��ʣ�µĲ���
//			RGN_OR -�µĲü�����Ϊ��ǰDC��Ĳü�������hrgn���С��򡱣�OR������
//			RGN_XOR -�µĲü�����Ϊ��ǰDC��Ĳü�������hrgn���С���򡱣�XOR������
// ����ֵ��
//	NULLREGION - �ü������ǿյ�
//	SIMPLEREGION - �ü�����ֻ��һ������
//	COMPLEXREGION - �ü����򺬶������
//	ERROR �C ����
// ����������
//	���ü�������DC��ǰ�Ĳü���������ز����� ѡ��ü�����DC
// ����: 
//	ϵͳAPI
// ************************************************

// the GetSafeDrawPtr will return NULL when rgn == NULL_REGION
int WINAPI WinGdi_ExtSelectClipRgn( HDC hdc, HRGN hrgn, int mode )
{
	return DoExtSelectClipRgn( hdc, hrgn, mode );
}

// **************************************************
// ������int WINAPI WinGdi_SelectClipRgn( HDC hdc, HRGN hrgn )
// ������
//	IN hdc - DC���
//	IN hrgn �C ������
// ����ֵ��
//	NULLREGION - �ü������ǿյ�
//	SIMPLEREGION - �ü�����ֻ��һ������
//	COMPLEXREGION - �ü����򺬶������
//	ERROR �C ����
// ����������
//	ѡ��ü�����DC
// ����: 
//	ϵͳAPI
// ************************************************

// the GetSafeDrawPtr will return NULL when rgn == NULL_REGION
int WINAPI WinGdi_SelectClipRgn( HDC hdc, HRGN hrgn )
{
	return DoExtSelectClipRgn( hdc, hrgn, RGN_COPY );
}

// **************************************************
// ������int WINAPI WinGdi_GetClipBox( HDC hdc, LPRECT lpRect )
// ������
//	IN hdc-Ŀ��DC
//	OUT lpRect-���ڵõ�����, RECT�ṹָ��
// ����ֵ��
//		NULLREGION-������Ϊ��
//		SIMPLEREGION-��������һ������
//		COMPLEXREGION0-�������ж������
//		ERROR-����
// ����������
//	�õ��豸�ü�����ı߽�
// ����: 
//	ϵͳAPI
// ************************************************

// the GetSafeDrawPtr will return NULL when rgn == NULL_REGION
int WINAPI WinGdi_GetClipBox( HDC hdc, LPRECT lpRect )
{
    _LPGDCDATA lpdc = _GetHDCPtr( hdc );
	int retv = ERROR;

    if( lpdc )
	{
        retv = WinRgn_GetBox( lpdc->hrgn, lpRect );
		//ת�����߼�����
		OffsetRect( lpRect, -lpdc->deviceOrg.x, -lpdc->deviceOrg.y );
	}
	return retv;
}

// **************************************************
// ������int WINAPI WinGdi_GetClipRgn( HDC hdc, HRGN hrgn )
// ������
//	IN hdc-Ŀ��DC
//  IN/OUT hrgn-���ڵõ�����ľ��
// ����ֵ��
//		0-û������(������)
//		1-������
//		ERROR-����
// ����������
//	�õ��豸����
// ����: 
//	ϵͳAPI
// ************************************************

// the GetSafeDrawPtr will return NULL when rgn == NULL_REGION
int WINAPI WinGdi_GetClipRgn( HDC hdc, HRGN hrgn )
{
    _LPGDCDATA lpdc = _GetHDCPtr( hdc );
    if( lpdc )
    {	//������hrgn
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
// ������HGDIOBJ WINAPI WinGdi_GetCurrentObject( HDC hdc, UINT uObjectType )
// ������
//	IN hdc-Ŀ��DC
//	IN uObjectType-�������ͣ�������
//	    OBJ_PEN-�ʶ���
//	    OBJ_BRUSH-ˢ�Ӷ���
//	    OBJ_FONT-�������
//	    OBJ_BITMAP-λͼ����
// ����ֵ��
//	�ɹ���������Ӧ����
//	���򣺷���NULL
// ����������
//	�õ���ǰ��ѡ���豸�Ķ���
// ����: 
//	ϵͳAPI
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
// ������BOOL WINAPI WinGdi_GetIconInfo( HICON hIcon, LPICONINFO lpInfo )
// ������
//	IN hIcon - ͼ����
//	IN lpInfo �CICONINFO���ݽṹָ�룬���ڽ���ͼ����Ϣ��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�õ�ͼ����Ϣ
// ����: 
//	ϵͳAPI
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
// ������DWORD WINAPI WinGdi_GetObjectType( HGDIOBJ h )
// ������
// 	IN h - ������
// ����ֵ��
//	�ɹ������ض�������
//	�������Ͱ�����
//		OBJ_BITMAP-λͼ
//		OBJ_BRUSH-ˢ��
//		OBJ_FONT-����
//		OBJ_PEN-��
//		OBJ_REGION-����
//		OBJ_DC-dc
//		OBJ_MEMDC-�ڴ�dc
//	���򣺷���0
// ����������
//	�õ���������
// ����: 
//	ϵͳ API
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
// ������int WINAPI WinGdi_GetObjectInfo( HGDIOBJ h, int bufferSize, void * lpBuf )
// ������
//	IN h-������
//	IN bufferSize-���ܻ���Ĵ�С
//	IN lpBuf-���ڽ��ܶ�����Ϣ�Ļ���
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	����ɹ�����lpvBuf����Ч��ַ,�������ݴ�С,����lpvBuf��NULL, �������ݴ�С;����ʧ��,����0
// ����: 
//	ϵͳAPI
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
        case OBJ_BITMAP:			//λͼ����
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
        case OBJ_PEN:			//�ʶ���
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
        case OBJ_BRUSH:			//ˢ�Ӷ���
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
        case OBJ_FONT:			//�������
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
// ������int WINAPI WinGdi_ExcludeClipRect( HDC hdc, int left, int top, int right, int bottom )
// ������
//	IN hdc-Ŀ��DC
//	IN left-�������
//	IN top-���ζ���
//	IN right-�����ұ�
//	IN bottom-���εױ�
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	���豸������ȥ�������ľ���
// ����: 
//	ϵͳAPI
// ************************************************

int WINAPI WinGdi_ExcludeClipRect( HDC hdc, int left, int top, int right, int bottom )
{
    _LPGDCDATA lpdc = _GetHDCPtr( hdc );
    int retv = ERROR;

    if( lpdc )
    {	//����һ���������
        HRGN hrgn = WinRgn_CreateRect( left, top, right, bottom );
        if( hrgn )
        {	//�߼����굽�豸����
            WinRgn_Offset( hrgn, XOFFSET( lpdc ), YOFFSET( lpdc ) );
			//ȥ�������ľ���
            retv = WinRgn_Combine( lpdc->hrgn, lpdc->hrgn, hrgn, RGN_DIFF );
            WinGdi_DeleteObject( hrgn );
        }
    }
    return retv;
}

// **************************************************
// ������int WINAPI WinGdi_IntersectClipRect( HDC hdc, int left, int top, int right, int bottom )
// ������
//	IN hdc-Ŀ��DC
//	IN left-�������
//	IN top-���ζ���
//	IN right-�����ұ�
//	IN bottom-���εױ�
// ����ֵ��
//	NULLREGION-������Ϊ��
//	SIMPLEREGION-��������һ������
//	COMPLEXREGION-�������ж������
//	ERROR-����
// ����������
//	���豸����������ľ������ڽ�(��)����
// ����: 
//	ϵͳAPI
// ************************************************

int WINAPI WinGdi_IntersectClipRect( HDC hdc, int left, int top, int right, int bottom )
{
    _LPGDCDATA lpdc = _GetHDCPtr( hdc );
    int retv = ERROR;

    if( lpdc )
    {
        HRGN hrgn = WinRgn_CreateRect( left, top, right, bottom );
        if( hrgn )
        {	//�߼����굽�豸����
            WinRgn_Offset( hrgn, XOFFSET( lpdc ), YOFFSET( lpdc ) );
            retv = WinRgn_Combine( lpdc->hrgn, lpdc->hrgn, hrgn, RGN_AND );
            WinGdi_DeleteObject( hrgn );
        }
    }
    return retv;
}

// **************************************************
// ������BOOL WINAPI WinGdi_PtVisible( HDC hdc, int x, int y )
// ������
//	IN hdc-Ŀ����
//	IN x-��X����
//	IN y-��Y����
// ����ֵ��
//	������ڲü�������,����TRUE
//	����,����FALSE
// ����������
//	�ж����Ƿ��ڲü�������
// ����: 
//	ϵͳ API
// ************************************************

BOOL WINAPI WinGdi_PtVisible( HDC hdc, int x, int y )
{
    _LPGDCDATA lpdc = _GetHDCPtr( hdc );

    if( lpdc )
    {	//�߼����굽�豸����
        x += XOFFSET( lpdc );
        y += YOFFSET( lpdc );
        return WinRgn_PtInRegion( lpdc->hrgn, x, y );
    }
    return FALSE;
}

// **************************************************
// ������BOOL WINAPI WinGdi_RectVisible( HDC hdc, LPCRECT lpcRect )
// ������
//	IN hdc-Ŀ����
//	IN lpcRect-����, RECTָ��
// ����ֵ��
//	��������ڲü�������,����TRUE
//	���򣬷���FALSE
// ����������
//	�ж������Ƿ��ڲü�������
// ����: 
//	ϵͳAPI
// ************************************************

BOOL WINAPI WinGdi_RectVisible( HDC hdc, LPCRECT lpcRect )
{
    _LPGDCDATA lpdc = _GetHDCPtr( hdc );

    if( lpdc )
    {
        RECT rect = *lpcRect;
		//�߼����굽�豸����
        OffsetRect( &rect, XOFFSET( lpdc ), YOFFSET( lpdc ) );
        return WinRgn_RectInRegion( lpdc->hrgn, &rect );
    }
    return FALSE;
}

// **************************************************
// ������int WINAPI WinGdi_GetROP2( HDC hdc )
// ������
//	IN hdc-Ŀ���� 
// ����ֵ��
//	�ɹ������ز�����ֵ
//	���򣺷���0
// ����������
//	�õ��ʹ�դ������
// ����: 
//	ϵͳAPI
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
// ������int WINAPI WinGdi_SetROP2( HDC hdc, int rop )
// ������
//	IN hdc-Ŀ����
//	IN rop-��դ������
// ����ֵ��
//	�ɹ�������������ֵ
//	���򣺷���0
// ����������
//	���ñʹ�դ������
// ����: 
//	ϵͳAPI
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
// ������static void _Offset( LPBYTE lp, int xOffset, int yOffset )
// ������
// 	IN lp - ��Ҫ����λ�ƶ�������
//	IN xOffset - �� x ������ƶ�
//	IN yOffset - �� y ������ƶ�
// ����ֵ��
//	��
// ����������
//	��һ�� 8 bits * 8 ��λ�������λ�ƶ�
//   ----> x ����	 
//  0000 0000		^
//  0000 0000		|
//  0000 0000		|	y����
//  0000 0000		|
//  0000 0000		|
//  0000 0000
//  0000 0000
//  0000 0000
//
// ����: 
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
    {	//����
		for( i = 0; i < 8; i++ )
		{
			Temp = ( ((WORD)*p)<<8 ) | *p;
			*p = Temp >> xOffset;
			p++;
		}
    }
    else if( xOffset < 0 )   // left move
    {	//����
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
    {	//����
		for( i = 0; i < 8; i++ )
			buf[(yOffset+i) & 0x07] = *p++;
		memcpy( lp, buf, 8 );
    }
    else if( yOffset < 0 )
    {	//����
		for( i = 0; i < 8; i++ )
			*(buf+i) = lp[(i+yOffset) & 0x07];
		memcpy( lp, buf, 8 );
    }
}

// **************************************************
// ������BOOL WINAPI WinGdi_SetBrushOrg( HDC hdc, int xOrg, int yOrg, LPPOINT lppt )
// ������
//	IN hdc-Ŀ��DC
//	IN xOrg-��ԭ��X����
//	IN yOrg-��ԭ��Y����
//	IN lppt-���ڽ�����ԭ������,POINT�ṹ��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	����ˢ��ԭ��
// ����: 
//	ϵͳAPI
// ************************************************

BOOL WINAPI WinGdi_SetBrushOrg( HDC hdc, int xOrg, int yOrg, LPPOINT lppt )
{
	_LPBRUSHATTRIB lpBrushAttrib;
    _LPGDCDATA lpdc;
	//�������Ƿ�Ϸ�
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


			//�任ˢ����ģ
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

			//�任ˢ����ģ
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
// ������BOOL WINAPI WinGdi_GetTextMetrics( HDC hdc, LPTEXTMETRIC lptm )
// ������
//	IN hdc-Ŀ����
//	IN lptm-TEXTMETRIC�ṹָ��,���ڽ����������
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�õ��ı�����
// ����: 
//	ϵͳAPI
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
// ������BOOL WINAPI WinGdi_GetCharWidth( HDC hdc, WORD iFirst, WORD iLast, LPINT lpiBuf )
// ������
//	IN hdc-Ŀ����
//	IN iFirst-��ʼ�ַ�
//	IN iLast-�����ַ�
//	IN lpiBuf-�����ȵĻ���
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�õ��ַ����
// ����: 
//	ϵͳAPI
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
// ������BOOL WINAPI WinGdi_LPtoDP( HDC hdc, LPPOINT lpPoints, int nCount )
// ������
//	IN hdc-Ŀ��DC
//	IN/OUT lpPoints-������,POINT�ṹָ��,�����ܾ�ת���������
//	IN nCount-�����
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	���߼�����ת��Ϊ�豸����
// ����: 
//	ϵͳAPI
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
// ������BOOL WINAPI WinGdi_DPtoLP( HDC hdc, LPPOINT lpPoints, int nCount )
// ������
//	IN hdc-Ŀ��DC
//	IN/OUT lpPoints-������,POINT�ṹָ��,�����ܾ�ת���������
//	IN nCount-�����
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	���豸����ת��Ϊ�߼�����
// ����: 
//	ϵͳAPI
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
// ������BOOL WINAPI WinGdi_GetCurrentPosition( HDC hdc, LPPOINT lpPoint )
// ������
//	IN hdc-Ŀ��DC
//	IN lpPoint-���ڵõ�������,POINT�ṹָ��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�õ���ǰ��λ��
// ����: 
//	ϵͳAPI
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
// ������BOOL WINAPI WinGdi_GetViewportOrg( HDC hdc, LPPOINT lpPoint )
// ������
//	IN hdc-Ŀ����
//	IN lpPoint-����ԭ��,POINT�ṹָ��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�õ�����ԭ��
// ����: 
//	ϵͳAPI
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
// ������BOOL WINAPI WinGdi_SetViewportOrg( HDC hdc, int xOrg, int yOrg, LPPOINT lppt )
// ������
//	IN hdc-Ŀ����
//	IN xOrg-��ԭ��X����
//	IN yOrg-��ԭ��Y����
//	OUT lppt-����ԭ��,POINT�ṹָ��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	��������ԭ��
// ����: 
//	ϵͳAPI
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
// ������BOOL WINAPI WinGdi_GetWindowOrg( HDC hdc, LPPOINT lpPoint )
// ������
//	IN hdc-Ŀ����
//	OUT lpPoint-����ԭ��,POINT�ṹָ��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�õ�����ԭ��
// ����: 
//	ϵͳAPI
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
// ������BOOL WINAPI WinGdi_SetWindowOrg( HDC hdc, int xOrg, int yOrg, LPPOINT lppt )
// ������
//	IN hdc-Ŀ����
//	IN xOrg - �µ�ԭ��x
//	IN yOrg - �µ�ԭ��y
//	OUT lpPoint-����֮ǰ��ԭ��,POINT�ṹָ��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	���ô���ԭ��
// ����: 
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
// ������int WINAPI WinGdi_OffsetClipRgn( HDC hdc, int xOff, int yOff )
// ������
//	IN hdc �C DC���
//	IN xOff �C xƫ����
//	IN yOff �C yƫ����
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//		NULLREGION - �ü������ǿյ�
//		SIMPLEREGION - �ü�����ֻ��һ������
//		COMPLEXREGION - �ü����򺬶������
//		ERROR �C ����
// ����: 
//	ϵͳAPI
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
// ������BOOL WINAPI WinGdi_OffsetViewportOrg( HDC hdc, int xOff, int yOff, LPPOINT lppt )
// ������
//	IN hdc-Ŀ����
//	IN xOff-Xƫ��
//	IN yOff-Yƫ��
//	IN lppt-����ԭ��,POINT�ṹָ��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	ƫ������ԭ��
// ����: 
//	ϵͳAPI
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
// ������BOOL WINAPI WinGdi_OffsetWindowOrg( HDC hdc, int xOff, int yOff, LPPOINT lppt )
// ������
//	IN hdc-Ŀ����
//	IN xOff-Xƫ��
//	IN yOff-Yƫ��
//	IN lppt-����ԭ��,POINT �ṹָ��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	ƫ�ƴ���ԭ��
// ����: 
//	ϵͳAPI
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
// ������HGDIOBJ WINAPI WinGdi_GetStockObject( int fObject )
// ������
//	IN fObject-������,������
//			WHITE_BRUSH-��ˢ��
//			LTGRAY_BRUSH-���ˢ��
//			GRAY_BRUSH-��ˢ��
//			DKGRAY_BRUSH-�ػ�ˢ��
//			BLACK_BRUSH-��ˢ��
//			NULL_BRUSH-��ˢ��
//			WHITE_PEN-�ױ�
//			BLACK_PEN-�ڱ�
//			NULL_PEN-�ձ�
//			ANSI_FIXED_FONT-�̶���С����(����English)
//			ANSI_VAR_FONT-�ɱ��С����(����English)
//			SYSTEM_FONT-ϵͳ����(����English)
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�õ�ϵͳԤ�������
// ����: 
//	ϵͳAPI
// ************************************************

HGDIOBJ WINAPI WinGdi_GetStockObject( int fObject )
{
	//if( fObject < STOCK_GDIOBJ_NUM )	//�� dll �жϸò���
	if( fObject < SYS_STOCK_GDIOBJ_NUM )
	    return OEM_GetStockObject( fObject );
	return NULL;
}

// **************************************************
// ������int WINAPI WinGdi_GetMapMode( HDC hdc )
// ������
// 	IN hdc - DC���
// ����ֵ��
//	����ɹ������ص�ǰģʽ�����򣬷���0
// ����������
//	�õ���ǰģʽ
// ����: 
//	ϵͳAPI
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
// ������int WINAPI WinGdi_SetMapMode( HDC hdc, int iNewMapMode )
// ������
// 	IN hdc - DC���
//	IN iNewMapMode - �µ�ӳ��ģʽ
// ����ֵ��
//	����ɹ������ص�ǰģʽ�����򣬷���0
// ����������
//	�����µ�ӳ��ģʽ
// ����: 
//	ϵͳAPI
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
// ������BOOL DoStretchBlt(
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

// ������
//	IN hdcDest - Ŀ��DC
//	IN xDest - Ŀ��������Ͻ�X����
//	IN yDest - Ŀ��������Ͻ�Y����
//	IN nWidthDest - Ŀ����ο��
//	IN nHeightDest - Ŀ����θ߶�
//	IN hdcSrc - ԴDC
//	IN xSrc - Դ�������Ͻ�X����
//	IN ySrc - Դ�������Ͻ�Y����
//	IN nWidthSrc - Դ���ξ��ο��
//	IN nHeightSrc - Դ���ξ��θ߶�
//	IN dwRop - ��դ������,������
//			SRCCOPY-����Դ���ε�Ŀ�����
//			SRCAND--Դ������Ŀ��������������(AND)
//			SRCINVERT-Դ������Ŀ���������������(XOR)
//			SRCPAINT-Դ������Ŀ��������������(OR)
//			PATCOPY-����ģ�嵽Ŀ�����
//			BLACKNESS-�ú�ɫ���Ŀ�����
//			WHITENESS-�ð�ɫ���Ŀ�����
//			PATINVERT-ģ����ɫ��Ŀ����ɫ���������(XOR)
//			DSTINVERT-��תĿ����ɫ
//			NOTSRCCOPY-������ת��Դ��ɫ��Ŀ����� 
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	��ԴDC��Ŀ��DC����λͼ���ͣ��������ܣ�
//	����һ�������ܣ����ڽ������������
// ����: 
//	ϵͳAPI
// ************************************************

#define MAX_CACHE_INDEXS   16   //������2�Ĵη� 2 ^ n

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
								{  // ����
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
						{  // ����
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
// ������BOOL WINAPI WinGdi_StretchBlt(
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

// ������
//	IN hdcDest - Ŀ��DC
//	IN xDest - Ŀ��������Ͻ�X����
//	IN yDest - Ŀ��������Ͻ�Y����
//	IN nWidthDest - Ŀ����ο��
//	IN nHeightDest - Ŀ����θ߶�
//	IN hdcSrc - ԴDC
//	IN xSrc - Դ�������Ͻ�X����
//	IN ySrc - Դ�������Ͻ�Y����
//	IN nWidthSrc - Դ���ξ��ο��
//	IN nHeightSrc - Դ���ξ��θ߶�
//	IN dwRop - ��դ������,������
//			SRCCOPY-����Դ���ε�Ŀ�����
//			SRCAND--Դ������Ŀ��������������(AND)
//			SRCINVERT-Դ������Ŀ���������������(XOR)
//			SRCPAINT-Դ������Ŀ��������������(OR)
//			PATCOPY-����ģ�嵽Ŀ�����
//			BLACKNESS-�ú�ɫ���Ŀ�����
//			WHITENESS-�ð�ɫ���Ŀ�����
//			PATINVERT-ģ����ɫ��Ŀ����ɫ���������(XOR)
//			DSTINVERT-��תĿ����ɫ
//			NOTSRCCOPY-������ת��Դ��ɫ��Ŀ����� 
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	��ԴDC��Ŀ��DC����λͼ���ͣ��������ܣ�
//	����һ�������ܣ����ڽ������������
// ����: 
//	ϵͳAPI
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
// ������HWND WINAPI WinGdi_WindowFromDC( HDC hdc )
// ������
// 	IN hdc - DC���
// ����ֵ��
//	����ɹ������� hdc ��Ӧ�Ĵ��ھ�������򣬷��� NULL
// ����������
//	�õ�DC��Ӧ�Ĵ��ھ��
// ����: 
//	ϵͳAPI
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
// ������UINT WINAPI WinGdi_SetDIBColorTable( HDC hdc,
//											  UINT uStartIndex,
//											  UINT cEntries,
//											  CONST RGBQUAD *pColors )
// ������
// 	IN hdc - DC���
//  IN uStartIndex - ��Ҫ�ı�ĵ�ɫ�忪ʼ����
//  IN cEntries - ��Ҫ�ı�ĵ�ɫ������
//  IN pColors - �ṩ�ĵ�ɫ����
// ����ֵ��
//	����ɹ������� �Ѿ����õı����������򣬷��� 0
// ����������
//	���� ��ǰ�µĵ�ɫ�壬��� 1��4��8 BPP
// ����: 
//	ϵͳAPI
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
// ������ int WINAPI WinGdi_GetDeviceCaps( HDC hdc, int nIndex )
// ������
// 	IN hdc - DC���
//  IN nIndex - ����
// ����ֵ��
//	����ɹ������� ��Ӧ��ֵ�����򣬷��� 0
// ����������
//	�õ��豸��Ϣ
// ����: 
//	ϵͳAPI
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
// ������BOOL PixelAlphaTransfer( 
//							_LPGDCDATA lpdcDest,//_LPCDISPLAYDRV lpDestDispDrv,
//							_LPPIXELDATA lpPixDest,
//							LPRECT lprcDest,
//							_LPGDCDATA lpdcSrc,//_LPCDISPLAYDRV lpSrcDispDrv,
//							_LPPIXELDATA lpPixSrc,
//							LPRECT lprcSrc,
//							LPBYTE lpAlphaCache    //�Ѿ���õ�Alphaֵ
//						 )

// ������
//	IN lpdcDest - Ŀ��DC����  //lpDestDispDrv - Ŀ����������ӿ�
//	IN lpPixDest - Ŀ���ṹָ��
//	IN lprcDest - Ŀ�����
//	IN lpdcSrc - ԴDC����  //lpSrcDispDrv - Դ��������ӿ�
//	IN lpPixSrc - Դ��ṹָ��
//	IN lprcSrc - Դ����
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	��������λͼ֮�����Alpha����λͼ
// ����: 
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

	//Դ
	_LPBITMAP_DIB lpSrcDIB = (_LPBITMAP_DIB)lpPixSrc->lpDestImage;
	WORD bmType = lpSrcDIB->bitmap.bmFlags & BF_DIB;
	DWORD * lpPal = NULL;
	UINT uiPalNum = 0;

	//Ŀ��
	_LPBITMAP_DIB lpDestDIB = (_LPBITMAP_DIB)lpPixDest->lpDestImage;
	WORD bmDestType = lpDestDIB->bitmap.bmFlags & BF_DIB;
	DWORD * lpDestPal = NULL;
	UINT uiDestPalNum = 0;
	int yDestDir = 1;

	BYTE srcRed, srcGreen, srcBlue;
	BYTE destRed, destGreen, destBlue;
	BYTE red, green, blue;

	int yDir = 1;
	UINT clrRealTransparent = -1;	//�豸��ص���ɫֵ
	
	if( bmType )
	{	//ԴλͼΪDIB��ʽ
		lpPal = (DWORD*)&lpSrcDIB->palEntry[0];
		uiPalNum = lpSrcDIB->biClrUsed;
		yDir = lpSrcDIB->biDir;
		//��ɫֵΪָ���ɫ�������ֵ
		if( lpSrcDIB->biCompression == BI_RGB )
		    bmType = PAL_INDEX;
		else if( lpSrcDIB->biCompression == BI_BITFIELDS )
			bmType = PAL_BITFIELD;  //��ɫֵΪλͼ��
		else
			return FALSE;
	}

	if( bmDestType )
	{	//ԴλͼΪDIB��ʽ
		lpDestPal = (DWORD*)&lpDestDIB->palEntry[0];
		uiDestPalNum = lpDestDIB->biClrUsed;
		yDestDir = lpDestDIB->biDir;
		//��ɫֵΪָ���ɫ�������ֵ
		if( lpDestDIB->biCompression == BI_RGB )
		    bmDestType = PAL_INDEX;
		else if( lpDestDIB->biCompression == BI_BITFIELDS )
			bmDestType = PAL_BITFIELD;  //��ɫֵΪλͼ��
		else
			return FALSE;
	}

	//Ŀ���Ⱥ͸߶�
	width = lprcDest->right - lprcDest->left;
	height = lprcDest->bottom - lprcDest->top;

	//RETAILMSG( 1, ( "PixelTransfer.\r\n" ) );
	//Դ��ʼλ����λͼ�е�ƫ��
	xSrcOffset = lprcSrc->left;
	ySrcOffset = lprcSrc->top;
	//Ŀ����ʼλ����λͼ�е�ƫ��
	xDestOffset = lprcDest->left;
	yDestOffset = lprcDest->top;

	if( rgbTransparent != -1 )
	{
		rgbTransparent = lpSrcDispDrv->lpRealizeColor( rgbTransparent, lpPal, uiPalNum, bmType );
	}
		
	for( y = 0; y < height; y++ )
	{	// ÿһ��
		for( x = 0; x < width; x++ )
		{	// ÿһ��
			if( yDir < 0 )	//���λͼ�ķ���
			{
			    lpPixSrc->y = lpSrcDIB->bitmap.bmHeight - (y + ySrcOffset) - 1;
			}
			else
			{
			    lpPixSrc->y = y + ySrcOffset;
			}
		    lpPixSrc->x = x + xSrcOffset;
			//�õ�Դ���豸�����ɫֵ
			clrCur = lpSrcDispDrv->lpGetPixel( lpPixSrc );

			if( clrCur != rgbTransparent )
			{	//�����͸��ɫ����֮
				if( clrCur != clrPrev )
				{	//��֮ǰ����ɫֵ��ͬ���������ͬ������ת��Ϊ
					//�豸�޹ص�ֵ
					//					if( bmType && uiPalNum )
					//					{   // _Gdi_UnrealizeColor ���Զ� call 
					//						clrRgb = _Gdi_UnrealizeColor( clrCur, lpPal, uiPalNum, bmType );  
					//					}
					//					else
					clrRgb = lpSrcDispDrv->lpUnrealizeColor( clrCur, lpPal, uiPalNum, bmType ); //  ���Զ� call _Gdi_UnrealizeColor
					
					
					srcRed = GetRValue( clrRgb );
					srcGreen = GetGValue( clrRgb );
					srcBlue = GetBValue( clrRgb );
					
					clrPrev = clrCur;
				}
				
				// �õ�Ŀ��RGB
				
				lpPixDest->x = x + xDestOffset;
				lpPixDest->y = y + yDestOffset;
				clrDestCur = lpDestDispDrv->lpGetPixel( lpPixDest );
				if( clrDestCur != clrDestPrev )
				{	//��֮ǰ����ɫֵ��ͬ���������ͬ������ת��Ϊ
					//�豸�޹ص�ֵ
					//if( bmDestType && uiDestPalNum )
					//{
					//	clrDestRgb = _Gdi_UnrealizeColor( clrDestCur, lpDestPal, uiDestPalNum, bmDestType );  
					//}
					//else
					clrDestRgb = lpDestDispDrv->lpUnrealizeColor( clrDestCur, lpDestPal, uiDestPalNum, bmDestType ); //���Զ� call _Gdi_UnrealizeColor
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
				//{	//�õ�Ŀ����ص�ֵ
				
				//}
				//д�㵽Ŀ��λͼ
				lpDestDispDrv->lpPutPixel( lpPixDest );
			}
		}
	}
	return TRUE;
}


// alphe ���ǣ���ǰ֧�֡�nWidthDest = nWidthSrc && hHeightDest == nHeightSrc
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
	//������
	if( (nWidthDest != nWidthSrc) || (nHeightDest != nHeightSrc) )
	{
		goto _return;
	}
	if( !lpBlendFunction )
		goto _return;
	if( lpBlendFunction->bf.BlendOp != AC_SRC_OVER )
		goto _return;

    lpdcDest = _GetSafeDrawPtr( hdcDest );//�õ�Ŀ��DC����ָ��
    lpdcSrc = _GetSafeDrawPtr( hdcSrc );//�õ�ԴDC����ָ��

    if( lpdcDest && lpdcSrc && lpdcDest->lpDispDrv )
    {	
        _LPCDISPLAYDRV lpDispDrv = lpdcDest->lpDispDrv;  //Ŀ��DC��������
		_LPBITMAPDATA lpDestImage = _GetHBITMAPPtr( lpdcDest->hBitmap );
		_LPBITMAPDATA lpSrcImage = _GetHBITMAPPtr( lpdcSrc->hBitmap );

		lprgn = _GetHRGNPtr( lpdcDest->hrgn );
		//Դ/Ŀ��λͼ���ݽṹ
        //blt.lpDestImage = _GetHBITMAPPtr( lpdcDest->hBitmap );
        //blt.lpSrcImage = _GetHBITMAPPtr( lpdcSrc->hBitmap );
		//��ǰˢ��
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
		//��ǰԴDC����ʾ�豸�ϵ�ƫ��
        xSrcOff = XOFFSET( lpdcSrc );
        ySrcOff = YOFFSET( lpdcSrc );
		//ת��Դ���굽�豸����
        rcTemp.left = nXOriginSrc;
        rcTemp.top = nYOriginSrc;
        rcTemp.right = rcTemp.left + nWidthDest;
        rcTemp.bottom = rcTemp.top + nHeightDest;
        OffsetRect( &rcTemp, xSrcOff, ySrcOff );
		//��ǰĿ��DC����ʾ�豸�ϵ�ƫ��
        xoff = XOFFSET( lpdcDest );
        yoff = YOFFSET( lpdcDest );
		//ת��Ŀ�����굽�豸����
        rcClip.left = nXOriginDest;
        rcClip.top = nYOriginDest;
        rcClip.right = rcClip.left + nWidthDest;
        rcClip.bottom = rcClip.top + nHeightDest;
        OffsetRect( &rcClip, xoff, yoff );
		//�õ�Դ��Ŀ��Ĺ�ͬ����
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
		//Դ������Ŀ���ϵ�Ͷ������ƫ��
        xoff = nXOriginSrc + XOFFSET( lpdcSrc ) - nXOriginDest - xoff;
        yoff = nYOriginSrc + YOFFSET( lpdcSrc ) - nYOriginDest - yoff;
		//Ŀ��ü���
        lprnNode = lprgn->lpNodeFirst;

		if( lpdcSrc->lpDispDrv )
		{   //��Ҫ��ÿһ������д��� ����Ե㿽�� pixel -> pixel
			// �����������Ĵ���ʱ�䣡
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

			//����ÿһ���ü����������ʾ�����
			while( lprnNode )
			{	//�õ��ڽ�����
				if( IntersectRect( &rcDest, &rcClip, &lprnNode->rect ) )
				{
					rcSrc = rcDest;
					OffsetRect( &rcSrc, xoff, yoff );
					//��ÿ�������ת��

					//PixelAlphaTransfer( lpdcDest, &pxDest, &rcDest, lpdcSrc, &pxSrc, &rcSrc, alphaSrcCache, alphaDestCache, rgbTransparent );
					PixelAlphaTransfer( lpdcDest, &pxDest, &rcDest, lpdcSrc, &pxSrc, &rcSrc, alphaCache, rgbTransparent );
					
				}
				lprnNode = lprnNode->lpNext;//��һ���ü���
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
// ������BOOL WINAPI WinGdi_SetDeviceWindow( HDC hdc, int xOrg, int yOrg, LPPOINT lppt )
// ������
//	IN hdc-Ŀ����
//	IN xOrg - �µ�ԭ��x
//	IN yOrg - �µ�ԭ��y
//	OUT lpPoint-����֮ǰ��ԭ��,POINT�ṹָ��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	���ô���ԭ��
// ����: 
//	
// ************************************************

BOOL WINAPI WinGdi_SetDeviceWindow( HDC hdc, LPCRECT lprc )
{
	_LPGDCDATA lpdc;
	
	if( (lpdc = _GetHDCPtr( hdc )) != 0 )
    {
		lpdc->deviceOrg.x = lprc->left;
		lpdc->deviceOrg.y = lprc->top;
        lpdc->deviceExt.cx = lprc->right - lprc->left;//���ڿ��
        lpdc->deviceExt.cy = lprc->bottom - lprc->top;//���ڸ߶�		
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
        //lpdc->deviceExt.cx = lprc->right - lprc->left;//���ڿ��
        //lpdc->deviceExt.cy = lprc->bottom - lprc->top;//���ڸ߶�		
		return hrgnOld;
    }
	return NULL;
}
