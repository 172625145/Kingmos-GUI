/******************************************************
Copyright(c) ��Ȩ���У�1998-2005΢�߼�����������Ȩ����
******************************************************/


/*****************************************************
�ļ�˵��������ʱ��
�汾�ţ�2.0.0
����ʱ�ڣ�1999
���ߣ�����
�޸ļ�¼��
******************************************************/

#include <eframe.h>
#include <epwin.h>

extern UINT FASTCALL _SetTimer( HANDLE hOwnerProcess, HWND hWnd, UINT msg, UINT id, UINT uElapse, TIMERPROC lpTimerProc );
extern UINT FASTCALL _KillTimer( HWND hWnd, UINT msg, UINT id );
// **************************************************
// ������UINT WINAPI Win_SetTimer(	HWND hWnd, 
//									UINT id, 
//									UINT uElapse, 
//									TIMERPROC lpTimerProc )
// ������
// 	IN hWnd - ���ھ��
//	IN id - ʱ�ӵ�ID��
//	IN uElapse - ����ʱ�䣨����Ϊ��λ��
//  IN lpTimerProc - �ص�����
// ����ֵ��
//	�ɹ�������id
//	���򣺷���0
// ����������
//	��װ��ʱ��
// ����: 
//	ϵͳAPI
// ************************************************
 
UINT WINAPI Win_SetTimer( HWND hWnd, UINT id, UINT uElapse, TIMERPROC lpTimerProc )
{
	HANDLE hProcess = GetCallerProcess();

	if( hProcess == NULL )
		hProcess = hgwmeProcess;
    if( hWnd )
	    return _SetTimer( hProcess, hWnd, WM_TIMER, id, uElapse, lpTimerProc );
	else
		return _SetTimer( hProcess, NULL, 0, id, uElapse, lpTimerProc );
}

// **************************************************
// ������BOOL WINAPI Win_KillTimer( HWND hWnd, UINT id )
// ������
// 	IN hWnd - ���ھ��
//	IN id - ʱ�ӵ�ID��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	ע����һ��ʱ��ID
// ����: 
//	ϵͳAPI
// ************************************************

BOOL WINAPI Win_KillTimer( HWND hWnd, UINT id )
{
    if( hWnd )
	    return _KillTimer( hWnd, WM_TIMER, id );
	else
		return _KillTimer( hWnd, 0, id );

}

// **************************************************
// ������UINT WINAPI Win_SetSysTimer( HWND hWnd, UINT id, UINT uElapse, TIMERPROC lpTimerProc )
// ������
// 	IN hWnd - ���ھ��
//	IN id - ʱ�ӵ�ID��
//	IN uElapse - ����ʱ�䣨����Ϊ��λ��
//  IN lpTimerProc - �ص�����
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	��װϵͳ��ʱ��
// ����: 
//	ϵͳAPI
// ************************************************

UINT WINAPI Win_SetSysTimer( HWND hWnd, UINT id, UINT uElapse, TIMERPROC lpTimerProc )
{
	HANDLE hProcess = GetCallerProcess();

	if( hProcess == NULL )
		hProcess = hgwmeProcess;

    if( hWnd )
	    return _SetTimer( hProcess, hWnd, WM_SYSTIMER, id, uElapse, lpTimerProc );
	else
		return _SetTimer( hProcess, NULL, 0, id, uElapse, lpTimerProc );
}

// **************************************************
// ������BOOL WINAPI Win_KillSysTimer( HWND hWnd, UINT id )
// ������
// 	IN hWnd - ���ھ��
//	IN id - ʱ�ӵ�ID��
// ����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
// ����������
//	ע����һ��ʱ��ID
// ����: 
//	ϵͳAPI
// ************************************************

BOOL WINAPI Win_KillSysTimer( HWND hWnd, UINT id )
{
    if( hWnd )
	    return _KillTimer( hWnd, WM_SYSTIMER, id );
	else
		return _KillTimer( hWnd, 0, id );
}

