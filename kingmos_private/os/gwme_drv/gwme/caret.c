/******************************************************
Copyright(c) ��Ȩ���У�1998-2005΢�߼�����������Ȩ����
******************************************************/


/*****************************************************
�ļ�˵����Caret����
�汾�ţ�2.0.0
����ʱ�ڣ�1999
���ߣ�����
�޸ļ�¼��
    2003-10-07: ����caret.hrgnClip
******************************************************/

#include <eframe.h>
#include <ecaret.h>
#include <eassert.h>
#include <eapisrv.h>
#include <winsrv.h>
#include <gdisrv.h>

#define CCS_HIDE 0
#define CCS_SHOW 1

//��������ṹ
typedef struct __CARETDATA
{
    HWND hwnd;	//����������Ĵ���
    short int x;		//��ǰ����ڴ����пͻ����Ŀ�ʼ����
    short int y;
    short int width;		//��ǰ����ڴ����пͻ����ĳ��Ϳ��
    short int height;
    HBITMAP hBitmap;		//��������һ��λͼ����ֵ��Ч
    short int lockCount;		//����������
    WORD iBlinkTime;			//�����˸Ƶ�ʣ��Ժ���Ϊ��λ��
    //DWORD ticks;			//����
    WORD state;			//״̬
	WORD dump;
	HRGN hrgnClip;		//��ǰ��ʾ�Ĺ��ͼ���ڴ����еĲü���
}_CARETDATA, FAR * _LPCARETDATA;

//Ĭ������
static _CARETDATA caretData = { 0, 0, 0, 0, 0, 0, 0, 500, 0, 0, NULL };

static void _CaretBlink( HDC hdc, BOOL bFromPaintDC );

VOID CALLBACK AutoCaretBlink( 
  HWND hwnd, 
  UINT uMsg,
  UINT idEvent,
  DWORD dwTime  );

// **************************************************
// ������BOOL _InitCaret( void )
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	��ʼ��ϵͳ�ü���
// ����: 
//	GWME ϵͳ���س�ʼ��ʱ���øú���
// ************************************************

BOOL _InitCaret( void )
{
	caretData.hrgnClip = WinRgn_CreateRect( 0, 0, 0, 0 );
	return (BOOL)caretData.hrgnClip;
}

// **************************************************
// ������void _DeInitialCaret( void )
// ������
// 	��
// ����ֵ��
//	��
// ����������
//	�� _InitCaret �෴���ͷ�֮ǰ�������Դ
// ����: 
//	GWME ϵͳ�˳�ʱ���øú���
// ************************************************

void _DeInitialCaret( void )
{
	WinGdi_DeleteObject( caretData.hrgnClip );
}

// **************************************************
// ������static void _CaretBlink( HDC hdc, BOOL bFromPaintDC )
// ������
// 	IN hdc - ��ͼDC ���
//	IN bFromPaintDC - �� hdc�Ƿ������� WM_PAINT �ľ��
// ����ֵ��
//	��
// ����������
//	����ʹ�����˸
// ����: 
//	
// ************************************************

static void _CaretBlink( HDC hdc, BOOL bFromPaintDC )
{
	HRGN hRgn;

	if( caretData.state == CCS_SHOW && bFromPaintDC == FALSE )
	{	//��ǰ�Ǵ�����ʾ״̬����hdc���������� BeginPaint
		//��֮ǰ����ʾ����ѡ��DC
		WinGdi_SelectClipRgn( hdc, caretData.hrgnClip );
	}
	//����
    WinGdi_PatBlt(hdc,
               caretData.x,
               caretData.y,
               caretData.width,
               caretData.height,
               DSTINVERT);
	//��ת״̬
    caretData.state = 1 - caretData.state;

	if( caretData.state == CCS_SHOW )
	{	//���浱ǰ��ʾ����
		hRgn = WinRgn_CreateRect( caretData.x, caretData.y, caretData.x + caretData.width, caretData.y + caretData.height );
		WinGdi_GetClipRgn( hdc, caretData.hrgnClip );
		WinRgn_Combine( caretData.hrgnClip, caretData.hrgnClip, hRgn, RGN_AND );
		WinGdi_DeleteObject( hRgn );
	}
}


// **************************************************
// ������BOOL WINAPI WinCaret_Create( HWND hWnd, HBITMAP hBitmap, int nWidth, int nHeight )
// ������
// 	IN hWnd	- ���ھ������������ӵ����
//	IN hBitmap - ������ʾ��ۣ����ΪNULL, ����ʾΪ���Σ���֧�֣�����ΪNULL��
//	IN nWidth - ���Ŀ��
//	IN nHeight - ���ĸ߶�
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	Ϊ���ڴ���һ�����
// ����: 
//	ϵͳAPI
// ************************************************
 
BOOL WINAPI WinCaret_Create( HWND hWnd, HBITMAP hBitmap, int nWidth, int nHeight )
{
    if( caretData.hwnd )
	{	//��ǰ�����ӵ����,�ͷ���
        WinCaret_Destroy();
	}
	//Ϊ��ӵ���߳�ʼ������
    caretData.hwnd = hWnd;
    caretData.x = 0;
    caretData.y = 0;
    caretData.width = nWidth;
    caretData.height = nHeight;
    caretData.hBitmap = hBitmap;
    caretData.state = CCS_HIDE;
    caretData.lockCount = 1;
	
	WinRgn_SetRect( caretData.hrgnClip, 0, 0, nWidth, nHeight );
	//����һ��ϵͳ timer
	SetSysTimer( hWnd, IDCARET, 400, NULL );//AutoCaretBlink );
    return TRUE;
}

// **************************************************
// ������BOOL WINAPI WinCaret_Destroy( void )
// ������
// 	��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	�ƻ����
// ����: 
//	ϵͳAPI
// ************************************************

BOOL WINAPI WinCaret_Destroy( void )
{
    HDC hdc;
	//�ƻ��ù��� timer
	KillSysTimer( caretData.hwnd, IDCARET );
	//����Ƿ���ס ��
    if( caretData.lockCount <= 0 )
    {   // û��caret is active
		//������ܣ��������
        if( caretData.state == CCS_SHOW && caretData.hwnd )
        {
            hdc = WinGdi_GetClientDC( caretData.hwnd );
            _CaretBlink( hdc, FALSE );
            WinGdi_ReleaseDC( caretData.hwnd, hdc );
        }
    }
	//�ͷ�ӵ����
    caretData.hwnd = NULL;
    caretData.lockCount = 1; // lock it
	// 2003-10-07
	WinRgn_SetRect( caretData.hrgnClip, 0, 0, 0, 0 );
	//
    return TRUE;
}

// **************************************************
// ������UINT WINAPI WinCaret_GetBlinkTime( void )
// ������
// 	��
// ����ֵ��
//	���ص�ǰ������˸ʱ�䣨�Ժ���Ϊ��λ��
// ����������
//	�õ���ǰ������˸ʱ�䣨�Ժ���Ϊ��λ��
// ����: 
//	ϵͳAPI
// ************************************************

UINT WINAPI WinCaret_GetBlinkTime( void )
{
    return caretData.iBlinkTime;
}

// **************************************************
// ������BOOL WINAPI WinCaret_GetPos( LPPOINT lpPoint )
// ������
// 	OUT lpPoint - POINT �ṹָ��
// ����ֵ��
//	����ɹ�������TRUE��lpPoint �����굱ǰ��λ����Ϣ�����򣬷���FALSE
// ����������
//	�õ���굱ǰ��λ����Ϣ
// ����: 
//	ϵͳAPI
// ************************************************

BOOL WINAPI WinCaret_GetPos( LPPOINT lpPoint )
{
    if( caretData.hwnd && lpPoint )
    {
        lpPoint->x = caretData.x;
        lpPoint->y = caretData.y;
        return TRUE;
    }
    else
        return FALSE;
}

// **************************************************
// ������BOOL _HideCaret( HWND hWnd, HDC hdc )
// ������
// 	IN hWnd - ���ھ��
//	IN hdc - ��ͼDC���
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	���ع��
// ����: 
//	�� BeginPaint GetDC �ȵ���
// ************************************************

BOOL _HideCaret( HWND hWnd, HDC hdc )
{
    if( caretData.hwnd == hWnd )
    {	//��Ч��ӵ����

        if( caretData.lockCount <= 0 )
        {   // ��굱ǰ�ǻ�� caret is active
            // ������ܣ����ع�� noactive caret
            caretData.lockCount = 1;
            if( caretData.state == CCS_SHOW )
            {   // hide the caret				
				hdc = WinGdi_GetClientDC( hWnd );
				_CaretBlink( hdc, FALSE );
				WinGdi_ReleaseDC( hWnd, hdc );
            }
        }
        else   // caret is no active
            caretData.lockCount++;	//���֮ǰ�ͱ���ס��������Ҫ�ۼӼ���
        return TRUE;
    }
    return FALSE;
}

// **************************************************
// ������BOOL WINAPI WinCaret_Hide( HWND hWnd )
// ������
// 	IN hWnd - ���ھ��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	���ع��
// ����: 
//	ϵͳAPI
// ************************************************

BOOL WINAPI WinCaret_Hide( HWND hWnd )
{
    return _HideCaret( hWnd, NULL );
}

// **************************************************
// ������BOOL WINAPI WinCaret_SetBlinkTime( UINT uMSeconds )
// ������
// 	IN uMSeconds - �Ժ���Ϊ��λ��ʾ��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	������˸ʱ��
// ����: 
//	ϵͳAPI
// ************************************************

BOOL WINAPI WinCaret_SetBlinkTime( UINT uMSeconds )
{
    caretData.iBlinkTime = uMSeconds;
    return TRUE;
}

// **************************************************
// ������BOOL WINAPI WinCaret_SetPos( int x, int y )
// ������
// 	IN x - ����µ��ڴ������λ��
// 	IN y - ����µ��ڴ������λ��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	���ô����µ�λ��
// ����: 
//	ϵͳAPI
// ************************************************

BOOL WINAPI WinCaret_SetPos( int x, int y )
{
    // ������ܣ������� hide caret if possible
    if( caretData.hwnd )
        WinCaret_Hide( caretData.hwnd );
    // set new position
    caretData.x = x;
    caretData.y = y;
    // show caret if possible
    if( caretData.hwnd )
        WinCaret_Show( caretData.hwnd );
    return TRUE;
}

// **************************************************
// ������BOOL _ShowCaret( HWND hWnd, HDC hdc )
// ������
// 	IN hWnd - ���ھ��
//	IN hdc - ��ͼ���
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	��ʾ���
// ����: 
//	
// ************************************************

BOOL _ShowCaret( HWND hWnd, HDC hdc )
{
    if( caretData.hwnd == hWnd )
    {
        if( caretData.lockCount >= 1 )
        {    // caret not been actived
            caretData.lockCount--;
        }
        else
            return TRUE;
    }
    return FALSE;
}

// **************************************************
// ������BOOL WINAPI WinCaret_Show( HWND hWnd )
// ������
// 	IN hWnd - ���ھ��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	��ʾ���
// ����: 
//	ϵͳAPI
// ************************************************

BOOL WINAPI WinCaret_Show( HWND hWnd )
{
	return _ShowCaret( hWnd, NULL );
}

// **************************************************
// ������void CALLBACK AutoCaretBlink( 
//					HWND hwnd, 
//					UINT uMsg,     // WM_SYSTIMER message
//					UINT idEvent,  // timer identifier
//					DWORD dwTime  )

// ������
// 	IN hWnd - ���ھ��
//	IN uMsg - ��Ϣ
//	IN idEvent - ID
//	IN dwTime - ����
// ����ֵ��
//	��
// ����������
//  ����������˸
// ����: 
//	����defproc.c ����
// ************************************************

#define ABS( v ) ( (v) > 0 ? (v) : -(v) )
//void AutoCaretBlink( void )
void CALLBACK AutoCaretBlink( 
  HWND hwnd, 
  UINT uMsg,     // WM_SYSTIMER message
  UINT idEvent,  // timer identifier
  DWORD dwTime  )
{
    HDC hdc;

	ASSERT( idEvent == IDCARET );
	if( idEvent == IDCARET )
	{
		if( caretData.hwnd && caretData.lockCount <= 0 )
		{
			hdc = WinGdi_GetClientDC( caretData.hwnd );
			//ASSERT( hdc );
			_CaretBlink( hdc, FALSE );
			WinGdi_ReleaseDC( caretData.hwnd, hdc );
		}
	}
}
