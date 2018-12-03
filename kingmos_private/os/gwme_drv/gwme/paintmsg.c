/******************************************************
Copyright(c) ��Ȩ���У�1998-2005΢�߼�����������Ȩ����
******************************************************/


/*****************************************************
�ļ�˵��������WM_PAINT��Ϣ
�汾�ţ�3.0.0
����ʱ�ڣ�1999
���ߣ�����
�޸ļ�¼��
******************************************************/

#include <eframe.h>
#include <epalloc.h>
#include <eassert.h>
#include <eapisrv.h>
#include <winsrv.h>
#include <gdisrv.h>

static HRGN hRepaintRgn;
static CRITICAL_SECTION csPaintRgn;

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

BOOL _InitialPaintRgn( void )
{
	InitializeCriticalSection( &csPaintRgn );
	csPaintRgn.lpcsName = "CS-PRN";
	return TRUE;
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

void _DeInitialPaintRgn( void )
{
    DeleteCriticalSection( &csPaintRgn );
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

BOOL CALLBACK __LockRepaintRgn( HRGN * lphrgn )
{
    RECT rect;

    EnterCriticalSection( &csPaintRgn );
    if( lphrgn )
        *lphrgn = hRepaintRgn;
    if( WinRgn_GetBox( hRepaintRgn, &rect ) == NULLREGION )
        return FALSE;
    else
        return TRUE;
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

BOOL CALLBACK __UnlockRepaintRgn( void )
{
	LeaveCriticalSection( &csPaintRgn );
	return TRUE;
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

int CALLBACK __PutPaintRgn( HRGN hrgn )
{
    int retv;
	EnterCriticalSection( &csPaintRgn );

    if( !hRepaintRgn )
        hRepaintRgn = WinRgn_CreateRect( 0, 0, 0 ,0 );
    retv = WinRgn_Combine( hRepaintRgn,
                       hRepaintRgn,
                       hrgn, RGN_OR );
	LeaveCriticalSection( &csPaintRgn );
	return retv;
}
