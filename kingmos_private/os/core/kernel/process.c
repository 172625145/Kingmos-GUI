/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵�������̹���
�汾�ţ�2.0.0
����ʱ�ڣ�2000
���ߣ�����
�޸ļ�¼��
	2004-12-30 SwitchBackProcess ȥ���������Ĵ��� ����
    2004-10-22 �޸�	LPPROCESS GetHandleOwner( void )

	2004-09-20: ȥ������ SwitchToProcess �е�һ�δ��룺
	            //dwProcessSlots[0] = (DWORD)lpProcess->lpSegIndex->lpSeg;
	            //SetCPUId( lpProcess->dwVirtualAddressBase );

    2003-09-09: DoCreateProcess ���ӶԲ����ļ��
    2003-09-02: ��Application Name and lpCmdLine �ŵ� mainthread stack
    2003-05-23: LN, GetCallerProcess, ���Ӷԡ�lpCurProcess���ж�
    2003-05-22-���Ӷ� CURRENT_PROCESS_HANDLE �Ĵ���
    2003-05-21: �� Process�ṹ�� lpSegment ��Ϊ lpSegIndex
    2003-05-19: ��CreateModule �ŵ�����
    2003-05-7:LN�޸�MODULE �ṹ
    2003-04-30:LN,Ϊ��ִ�У�exe�����ӽӿ�
	           ɾ�����õĴ���
******************************************************/

#include <eframe.h>
#include <eprogram.h>
#include <efile.h>

#include <epheap.h>
#include <epalloc.h>
#include <epcore.h>
#include <coresrv.h>

extern LPTHREAD FASTCALL _CreateThread(
									   HANDLE hProcess,
									   LPSECURITY_ATTRIBUTES lpThreadAttributes,   // 
									   DWORD dwStackSize,                      // 
									   DWORD dwPreservStackSize,
									   LPTHREAD_START_ROUTINE lpStartAddress, // 
									   LPVOID lpParameter,                // 
									   DWORD dwCreationFlags,         // 
									   LPDWORD lpThreadId         // 
									   );




// ********************************************************************
//������static DWORD WINAPI MainProcessProc( LPVOID lParam ) 
//������
//	IN lParam - ���̽ṹָ�룬ָ��ǰ����
//����ֵ��
//	������
//����������
//	�ú����Ǵ������½��̵���ʼ��ڣ�Ҳ�ǽ��̵��ں�̬��ͬʱΪ�ý��̳�ʼ��һ��Ĭ�ϵĶ�
//  �ý��̽��Ὣ����ģ������������𽫽���ģ��װ���ڴ沢���У�
//���ã�
// ********************************************************************

#define DEBUG_MAINPROCESSPROC 0
static DWORD WINAPI MainProcessProc( LPVOID lParam )
{
	extern HANDLE DoHeapCreate( DWORD flOptions, ULONG dwInitialSize, ULONG dwMaximumSize );
	DWORD dwExitCode = -1;
    LPPROCESS lpProcess = (LPPROCESS)lParam;

	RETAILMSG( DEBUG_MAINPROCESSPROC, ( "Load MainProcessProc=%s,id=%d,VMB=0x%x.\r\n", lpProcess->lpszApplicationName, lpProcess->dwProcessId, lpProcess->dwVirtualAddressBase ) );

	// ��ʼ��һ��Ĭ�ϵĽ��̶�
#ifdef VIRTUAL_MEM
	lpProcess->lpHeap = DoHeapCreate( HEAP_VIRTUAL_ALLOC, 0, 0 );// &lpProcess->heap );
#else
	lpProcess->lpHeap = DoHeapCreate( 0, 0, 0 );// &lpProcess->heap );
#endif


	RETAILMSG( DEBUG_MAINPROCESSPROC, ( "MainProcessProc:call module exe.\r\n" ) );
	
	// ����ģ�������
	dwExitCode = lpProcess->pModule->lpmd->lpLoadModule( lpProcess->pModule->hModuleData, lpProcess->pModule->hModule, lpProcess->lpszCommandLine, LF_DEFLOAD );//LN:2003-05-07, Add
	//

	while( 1 )
		KL_ExitProcess( dwExitCode );
	// ��������
}

// ********************************************************************
//������static DWORD AllocProcessId( void ) 
//������
//	��
//����ֵ��
//	����ɹ������ؽ���ID�����򣬷���0
//����������
//	����һ������ID
//���ã�
// ********************************************************************

static DWORD AllocProcessId( void )
{
	DWORD i;
	// 0 slots is reserver
	for( i = 1; i < MAX_PROCESSES; i++ )
	{
		if( KL_InterlockedCompareExchange( (LPLONG)&lppProcessSegmentSlots[i], 1, 0 ) == 0 )
		{
			return i;
		}
	}
	return 0;
}

// ********************************************************************
//������static BOOL FreeProcessId( DWORD dwProcessId ) 
//������
//	IN dwProcessId - ����ID
//����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
//����������
//	�ͷ�һ������ID
//���ã�
// ********************************************************************

static BOOL FreeProcessId( DWORD dwProcessId )
{
	if( dwProcessId < MAX_PROCESSES && dwProcessId > 0 )
	{	
		UINT uiSave;
		LockIRQSave( &uiSave );

		lppProcessSegmentSlots[dwProcessId] = 0;
		lppProcessPtr[dwProcessId] = 0;

	    //if( KL_InterlockedCompareExchange( (LPLONG)&dwProcessSlots[dwProcessId], 0, dwProcessSlots[dwProcessId] ) )
		//{
			//dwProcessPtr[dwProcessId] = 0;
		//}
		UnlockIRQRestore( &uiSave );
	}
	return FALSE;
}

//LN:2003-05-12, Add	
#ifdef VIRTUAL_MEM
// ********************************************************************
//������static LPPROCESS_SEGMENTS AllocSegmentIndexAndSegment( DWORD dwSegBaseAdr ) 
//������
//	IN dwSegBaseAdr - �öζ�Ӧ�Ļ���ַ
//����ֵ��
//	����ɹ�������һ���������ṹָ�룻���򣬷���NULL
//����������
//	����һ���������Ͷνṹ���������������������Σ��νṹ��������/��¼32M�Ľ��̿ռ�ʹ�������
//���ã�
// ********************************************************************

#define CACHE_PAGES 2
#define CACHE_PAGES_SIZE (1024*CACHE_PAGES)
#define DEBUG_AllocSegmentIndexAndSegment 0
LPPROCESS_SEGMENTS AllocSegmentIndexAndSegment( DWORD dwSegBaseAdr )
{	// ����һ��������
	LPPROCESS_SEGMENTS lpSegIndex = KHeap_Alloc( sizeof(PROCESS_SEGMENTS) );

	if( lpSegIndex )
	{   // ����һ���νṹ
		memset( lpSegIndex, 0, sizeof(PROCESS_SEGMENTS) );

		if( (lpSegIndex->lpSeg = Seg_Alloc()) )
		{
			lpSegIndex->lpdwSecondPageTable = KHeap_Alloc( CACHE_PAGES_SIZE );
			//return lpSegIndex;
			if( lpSegIndex->lpdwSecondPageTable )
			{
				lpSegIndex->lpdwPhySecondPageTable = (LPDWORD)( (DWORD)_GetPhysicalAdr( lpSegIndex->lpdwSecondPageTable ) | 1 );
				lpSegIndex->lpdwSecondPageTable = (LPDWORD)CACHE_TO_UNCACHE(lpSegIndex->lpdwSecondPageTable);				
				lpSegIndex->dwSegBaseAddress = dwSegBaseAdr;
				lpSegIndex->uiPageTableCount = CACHE_PAGES;

				memset( lpSegIndex->lpdwSecondPageTable, 0, CACHE_PAGES_SIZE );
				DEBUGMSG( DEBUG_AllocSegmentIndexAndSegment, ( "dwSegBaseAdr=0x%x,lpdwPhySecondPageTable=0x%x.\r\n", dwSegBaseAdr, lpSegIndex->lpdwPhySecondPageTable ) );
				 
				return lpSegIndex;
			}
		}
		if( lpSegIndex->lpSeg )
		    Seg_Free( lpSegIndex->lpSeg );

		KHeap_Free( lpSegIndex, sizeof(PROCESS_SEGMENTS) );
		lpSegIndex = NULL;
	}
	return lpSegIndex;
}
// ********************************************************************
//������ static void FreeSegmentIndexAndSegment( LPPROCESS_SEGMENTS lpSegIndex )
//������
//	IN lpSegIndex - �������ṹָ��
//����ֵ��
//	��
//����������
//	��AllocSegmentIndexAndSegment���Ӧ���ͷ�һ���������ṹָ�뼰��οռ�
//���ã�
// ********************************************************************

static void FreeSegmentIndexAndSegment( LPPROCESS_SEGMENTS lpSegIndex )
{
	ASSERT( lpSegIndex->lpSeg->lpBlks[0] == NULL );//ÿ���ε�ǰ64k�Ǳ������õģ�������NULL

	if( lpSegIndex->lpdwSecondPageTable )
	{
		LPDWORD lpT = (LPDWORD)UNCACHE_TO_CACHE( lpSegIndex->lpdwSecondPageTable );
		lpSegIndex->lpdwSecondPageTable = NULL;
	    KHeap_Free( lpT, CACHE_PAGES_SIZE );
	}
	Seg_Free( lpSegIndex->lpSeg );
	KHeap_Free( lpSegIndex, sizeof(PROCESS_SEGMENTS) );
}

// ********************************************************************
//������ static void FreeProcessSegmentIndexAndSegment( LPPROCESS_SEGMENTS lpSegIndex  )
//������
//	IN lpSegIndex - �������ṹָ��
//����ֵ��
//	��
//����������
//	�ͷŽ��̵����жΣ�һ�����̶���������/��¼32M�Ľ��̿ռ�ʹ�������
//���ã�
// ********************************************************************

static void FreeProcessSegmentIndexAndSegment( LPPROCESS_SEGMENTS lpSegIndex  )
{
	LPPROCESS_SEGMENTS lpSegNext;
	while( lpSegIndex )
	{
		lpSegNext = lpSegIndex->lpNext;
		FreeSegmentIndexAndSegment( lpSegIndex );
		lpSegIndex = lpSegNext;
	}
}
#endif  //VIRTUAL_MEM

// ********************************************************************
//������ void FreeProcessObject( LPPROCESS lpProcess )
//������
//	IN lpProcess - PROCESS�ṹָ��
//����ֵ��
//	��
//����������
//	�ͷŽ��̽ṹ�������øú���ʱ�����ڸý��̿ռ䣨�ý����Ѿ����ˣ�
//���ã�
//  ����������ʧ�ܻ�����Ѿ�����ʱ����øú���
// ********************************************************************

void FreeProcessObject( LPPROCESS lpProcess )
{
	DWORD dwProcessId = lpProcess->dwProcessId;
	if( lpProcess->lpMainThread )
		;// ????
	// �ͷ�ģ�������
	if( lpProcess->pModule )
		Module_DeInit( lpProcess->pModule );

#ifdef VIRTUAL_MEM
	// �ͷŽ��̵Ķ�
	if( lpProcess->lpProcessSegments )
	{
		VOID * lpT = lpProcess->lpProcessSegments;
		lpProcess->lpProcessSegments = NULL;

		FreeProcessSegmentIndexAndSegment( lpT );
	}
#endif

    // �ͷŽ��̾��
	if( lpProcess->hProcess )
        Handle_Free( lpProcess->hProcess, TRUE );
	if( lpProcess->lpCpuPTS )
		FreeCPUPTS( lpProcess->lpCpuPTS );

	lpProcess->objType = OBJ_NULL;
    // �ͷŽ��̽ṹ
    KHeap_Free( lpProcess, sizeof( PROCESS ) );
    // �ͷŽ���ID
    if( dwProcessId )
	    FreeProcessId( dwProcessId );

}

// ********************************************************************
//������BOOL FASTCALL DoCreateProcess(
//						   LPCTSTR lpszApplicationName,
//						   LPCTSTR lpszCommandLine, 
//						   LPSECURITY_ATTRIBUTES lpProcessAttributes,
//						   LPSECURITY_ATTRIBUTES lpThreadAttributes,
//						   BOOL bInheritHandles,
//						   DWORD dwCreationFlags,
//						   LPVOID lpEnvironment,
//						   LPCTSTR lpCurrentDirectory,
//						   LPSTARTUPINFO lpStartupInfo,
//						   LPPROCESS_INFORMATION lpProcessInformation,
//						   PCMODULE_DRV pmd
//						   )
//������
//	   IN lpszApplicationName - ��ִ��ģ����
//	   IN lpszCommandLine - ����WinMain�Ĳ���
//	   IN lpProcessAttributes - ���̰�ȫ����
//	   IN lpThreadAttributes - �̰߳�ȫ����
//	   IN bInheritHandles - �Ƿ�̳и����̵ľ��������ΪFALSE
//	   IN dwCreationFlags - ������־��
//							CREATE_SUSPENDED - ���������
//	   IN lpEnvironment - �����飨����ΪNULL��
//	   IN lpCurrentDirectory - �������еĵ�ǰĿ¼������ΪNULL��
//	   IN lpStartupInfo - ������Ϣ������ΪNULL��
//	   OUT lpProcessInformation - PROCESS_INFORMATION�ṹָ�루����ΪNULL�������ڽ��ܽ��̵�ĳЩ��Ϣ
//	   IN pmd - �ý���ģ������Ӧ�ļ�����
//����ֵ��
//	����ɹ�������TRUE�����lpProcessInformation��ΪNULL���ط��ؽ�����Ϣ��
//  ���򣬷���FALSE��
//����������
//	�����µĽ���
//���ã�
//	KL_CreateProcess & KL_LoadApplication
// ********************************************************************

#define DEBUG_DOCREATEPROCESS 0
BOOL FASTCALL DoCreateProcess(
						   LPCTSTR lpszApplicationName,
						   LPCTSTR lpszCommandLine,
						   LPSECURITY_ATTRIBUTES lpProcessAttributes,
						   LPSECURITY_ATTRIBUTES lpThreadAttributes,
						   BOOL bInheritHandles,
						   DWORD dwCreationFlags,
						   LPVOID lpEnvironment,
						   LPCTSTR lpCurrentDirectory,
						   LPSTARTUPINFO lpStartupInfo,
						   LPPROCESS_INFORMATION lpProcessInformation,
						   PCMODULE_DRV pmd
						   )
{

    DWORD id = 0;
	LPPROCESS lpProcess = NULL;
	int iCmdLineLen;
	ACCESS_KEY akySave;

	if( lpszApplicationName == NULL ||
		*lpszApplicationName == 0 )
	{
		KL_SetLastError( ERROR_INVALID_PARAMETER );
		return FALSE;
	}

	//
    DEBUGMSG( DEBUG_DOCREATEPROCESS, ( "DoCreateProcess:%s.\r\n", lpszApplicationName ) );
	if( lpszCommandLine == 0 )
	{
		lpszCommandLine = "";
	}

	iCmdLineLen = strlen( lpszCommandLine ) + strlen( lpszApplicationName ) + 2; // one null end, one space
	// ������̽ṹ
    DEBUGMSG( DEBUG_DOCREATEPROCESS, ( "DoCreateProcess:alloc process struct.\r\n" ) );
	lpProcess = KHeap_Alloc( sizeof( PROCESS ) );
	if( lpProcess )
	{   
		memset( lpProcess, 0, sizeof( PROCESS ) );
		// �������ID
		if( ( lpProcess->dwProcessId = AllocProcessId() ) == 0 )
		    goto _alloc_error;
		// ������̾��
		DEBUGMSG( DEBUG_DOCREATEPROCESS, ( "DoCreateProcess:alloc process handle.\r\n" ) );
		if( ( lpProcess->hProcess = Handle_Alloc( lpProcess, lpProcess, OBJ_PROCESS ) ) == NULL )
			goto _alloc_error;

#ifdef VIRTUAL_MEM
		// ����һ����
		if( ( lpProcess->lpProcessSegments = AllocSegmentIndexAndSegment(lpProcess->dwProcessId << SEGMENT_SHIFT) ) == NULL )
			goto _alloc_error;
		lppProcessSegmentSlots[lpProcess->dwProcessId] = lpProcess->lpProcessSegments;//->lpSeg;
		//lpProcess->lpProcessSegments->dwSegBaseAddress = lpProcess->dwProcessId << SEGMENT_SHIFT;
#else  
		//dwProcessSlots[lpProcess->dwProcessId] = NULL; //2004-09-21��lilin û��Ҫ		
#endif  //VIRTUAL_MEM

        

		
		if( (lpProcess->lpCpuPTS = AllocCPUPTS()) == NULL )
			goto _alloc_error;


		DEBUGMSG( DEBUG_DOCREATEPROCESS, ( "DoCreateProcess:call Module_Init.\r\n" ) );

		
		// ����ģ��������ṹ
		if( (lpProcess->pModule = Module_Init( lpProcess, lpszApplicationName, pmd, CREATE_EXE )) == NULL )	
			goto _alloc_error;
		//
		
		// ��ʼ��������������
		lpProcess->objType = OBJ_PROCESS;
		lpProcess->dwtlsMask = TLS_MASK;
#ifdef VIRTUAL_MEM
		lpProcess->dwVirtualAddressBase = lpProcess->lpProcessSegments->dwSegBaseAddress;
#else
		lpProcess->dwVirtualAddressBase = 0;
#endif

		//
		lpProcess->akyAccessKey = (0x1 << lpProcess->dwProcessId);
		//test only
//		lpProcess->akyAccessKey = -1;
		//
		
		
		lpProcess->lpszApplicationName = pmd->lpGetModuleFileName( lpProcess->pModule->hModuleData );//pModule->lpszApName;//LN:2003-05-07, Add
		
		//

		//lpProcess->dwCreationFlags = dwCreationFlags;
		lpProcess->dwFlags = (dwCreationFlags & CREATE_SYSTEM_MODE) ? M_SYSTEM : M_USER;
		RETAILMSG( DEBUG_DOCREATEPROCESS, ( "DoCreateProcess: lpProcess->dwFlags=0x%x.\r\n", lpProcess->dwFlags ) );
		akySave = lpCurThread->akyAccessKey;
		AccessKey_Add( &lpCurThread->akyAccessKey, lpProcess->akyAccessKey );
		// �������̲߳�������
		lpProcess->lpMainThread = _CreateThread( lpProcess, 
			                                     lpThreadAttributes, 
												 0, //������0��Ĭ�ϣ�����Ϊ���̵߳�stack��64k-128k
												 iCmdLineLen,
												 MainProcessProc, 
												 lpProcess, 
												 dwCreationFlags | CREATE_SUSPENDED | CREATE_MAINTHREAD, 
												 NULL );
		// 
		DEBUGMSG( DEBUG_DOCREATEPROCESS, (TEXT("DoCreateProcess:lpMainThread=%x.\r\n"), lpProcess->lpMainThread ) );

		if( lpProcess->lpMainThread  )
		{
			// �����в����ŵ��û�ջ��
			lpProcess->lpszCommandLine = (LPTSTR)lpProcess->lpMainThread->lpdwThreadStack + lpProcess->lpMainThread->dwThreadStackSize - iCmdLineLen;
			strcpy( (char*)lpProcess->lpszCommandLine, (char*)lpszApplicationName );
			if( *lpszCommandLine )
			{
			    strcat( (char*)lpProcess->lpszCommandLine, " " );
			    strcat( (char*)lpProcess->lpszCommandLine, (char*)lpszCommandLine );
			}
			lppProcessPtr[lpProcess->dwProcessId] = lpProcess;
			DEBUGMSG( DEBUG_DOCREATEPROCESS, ( "DoCreateProcess:check suspend.\r\n" ) );
			// �������߳�
			if( (dwCreationFlags & CREATE_SUSPENDED) == 0 )
			    //_WakeupThread( lpProcess->lpMainThread, FALSE );
				DoResumeThread(lpProcess->lpMainThread);

			DEBUGMSG( DEBUG_DOCREATEPROCESS, ( "DoCreateProcess:set lpProcessInformation.\r\n" ) );
			// ������ܣ����ý�����Ϣ�ṹ
			if( lpProcessInformation )
			{
				lpProcessInformation->hProcess = lpProcess->hProcess;
				lpProcessInformation->hThread = lpProcess->lpMainThread->hThread;
				lpProcessInformation->dwProcessId = lpProcess->dwProcessId;
				lpProcessInformation->dwThreadId = lpProcess->lpMainThread->dwThreadId;
			}
//			AccessKey_Remove( &lpCurThread->akyAccessKey, lpProcess->akyAccessKey );
			AccessKey_Set( &lpCurThread->akyAccessKey, akySave );
			
	        goto _ret;
		}
		else
		{
			RETAILMSG( 1, ( "error: KL_CreateProcess: Create Main Thread failure!!!!.\r\n" ) );
		}
//		AccessKey_Remove( &lpCurThread->akyAccessKey, lpProcess->akyAccessKey );
		AccessKey_Set( &lpCurThread->akyAccessKey, akySave );
	}
_alloc_error:
    RETAILMSG( 1, ( "error : KL_CreateProcess failure:%s.\r\n", lpszApplicationName ) );
	// �������
    if( lpProcess )
    {
		FreeProcessObject( lpProcess );
	}
	return FALSE;
_ret:
    return lpProcess ? TRUE : FALSE;
}

// ********************************************************************
//������BOOL WINAPI KL_CreateProcess(
//						   LPCTSTR lpszApplicationName,                 // name of executable module
//						   LPCTSTR lpszCommandLine,                      // command line string
//						   LPSECURITY_ATTRIBUTES lpProcessAttributes, // SD
//						   LPSECURITY_ATTRIBUTES lpThreadAttributes,  // SD
//						   BOOL bInheritHandles,                      // handle inheritance option
//						   DWORD dwCreationFlags,                     // creation flags
//						   LPVOID lpEnvironment,                      // new environment block
//						   LPCTSTR lpCurrentDirectory,                // current directory name
//						   LPSTARTUPINFO lpStartupInfo,               // startup information
//						   LPPROCESS_INFORMATION lpProcessInformation // process information
//						   )
 
//������
//	   IN lpszApplicationName - ��ִ��ģ����
//	   IN lpszCommandLine - ����WinMain�Ĳ���
//	   IN lpProcessAttributes - ���̰�ȫ����
//	   IN lpThreadAttributes - �̰߳�ȫ����
//	   IN bInheritHandles - �Ƿ�̳и����̵ľ��������ΪFALSE
//	   IN dwCreationFlags - ������־��
//							CREATE_SUSPENDED - ���������
//	   IN lpEnvironment - �����飨����ΪNULL��
//	   IN lpCurrentDirectory - �������еĵ�ǰĿ¼������ΪNULL��
//	   IN lpStartupInfo - ������Ϣ������ΪNULL��
//	   OUT lpProcessInformation - PROCESS_INFORMATION�ṹָ�루����ΪNULL�������ڽ��ܽ��̵�ĳЩ��Ϣ
//����ֵ��
//	����ɹ�������TRUE�����lpProcessInformation��ΪNULL���ط��ؽ�����Ϣ��
//  ���򣬷���FALSE��
//����������
//	�����µĽ���
//���ã�
//	ϵͳ����
// ********************************************************************

BOOL WINAPI KL_CreateProcess(
						   LPCTSTR lpszApplicationName, 
						   LPCTSTR lpszCommandLine,
						   LPSECURITY_ATTRIBUTES lpProcessAttributes,
						   LPSECURITY_ATTRIBUTES lpThreadAttributes,
						   BOOL bInheritHandles,
						   DWORD dwCreationFlags,
						   LPVOID lpEnvironment,
						   LPCTSTR lpCurrentDirectory,
						   LPSTARTUPINFO lpStartupInfo,
						   LPPROCESS_INFORMATION lpProcessInformation
						   )
{
#ifndef INLINE_PROGRAM
	// future , i will find module format.
	//
	extern MODULE_DRV ElfModule;
	return DoCreateProcess( lpszApplicationName, 
		                    lpszCommandLine, 
							lpProcessAttributes, 
							lpThreadAttributes, 
							bInheritHandles,
							dwCreationFlags,
							lpEnvironment,
							lpCurrentDirectory,
							lpStartupInfo,
							lpProcessInformation,
							&ElfModule );
#endif
	return FALSE;

}


// ********************************************************************
//������ BOOL FASTCALL Process_Close( HANDLE hProcess )
//������
//	IN hProcess - ���̾��
//����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
//����������
//	�ͷŽ��̾��
//���ã�
//	CloseHandle
// ********************************************************************

BOOL FASTCALL Process_Close( HANDLE hProcess )
{
	LPPROCESS lpProcess = HandleToPtr( hProcess, OBJ_PROCESS );
	lpProcess->hProcess = NULL;
	return TRUE;
}

// ********************************************************************
//������ VOID WINAPI KL_ExitProcess( UINT uExitCode )
//������
//	IN uExitCode - �˳�����
//����ֵ��
//	��
//����������
//	�˳�����
//���ã�
//	ϵͳAPI
// ********************************************************************
extern void KillAllChildThreads( LPPROCESS );
#define DEBUG_EXITPROCESS 0
VOID WINAPI KL_ExitProcess(
						 UINT uExitCode   // exit code for all threads
						 )
{
    extern void KillAllOtherThreads( LPPROCESS lpProcess );
	LPPROCESS lpProcess;
    
	ASSERT( lpCurThread->lpCurProcess == lpCurThread->lpOwnerProcess );//
	// �õ���ǰ���̣�current process
	lpProcess = lpCurThread->lpCurProcess;//GetAPICallerProcessPtr();
	DEBUGMSG( DEBUG_EXITPROCESS, ( "KL_ExitProcess:Exit Process(=%s) entry.\r\n", lpProcess->lpszApplicationName ) );

#if 0	// test
	{
		LPCALLSTACK lpcs = lpCurThread->lpCallStack;
		while( lpcs )
		{
			CALLSTACK * lpNext = lpcs->lpNext;
			
			RETAILMSG( 1, ( "lpcs:(0x%x).\r\n", lpcs ) );
			lpcs = lpNext;
		}
	}
#endif
	//
	// ���˳���Ϣ���浽���̾����
	SetHandleUserInfo( lpProcess->hProcess, uExitCode );
	lpCurThread->dwExitCode = uExitCode; 
	// 
	DEBUGMSG( DEBUG_EXITPROCESS, ( "KL_ExitProcess:KillAllChildThreads.\r\n" ) );
	while( 1 )
	{	//�ж��Ƿ��и�������߳�
		// ���ý���ӵ�е������߳�ɱ��
		KillAllOtherThreads( lpProcess );
		if( lpProcess->lpFirstThread->lpNextThreadInProcess == NULL )
			HandleThreadExit();
		else
			KL_Sleep( 3000 );
		WARNMSG( DEBUG_EXITPROCESS, ( "KL_ExitProcess:wait for child thread exit,ProcName=%s.\r\n", lpProcess->lpszApplicationName ) );
		//ASSERT( 0 );
	}
    //  not return here
}

// ********************************************************************
//������ VOID WINAPI KL_ExitProcess( UINT uExitCode )
//������
//	IN uExitCode - �˳�����
//����ֵ��
//	��
//����������
//	�˳�����
//���ã�
//	ϵͳAPI
// ********************************************************************
//extern void KillAllChildThreads( LPPROCESS );
//#define DEBUG_EXITPROCESS 1
VOID WINAPI KL_TerminateProcess(
						 HANDLE hProcess,
						 UINT uExitCode   // exit code for all threads
						 )
{
    extern void KillAllOtherThreads( LPPROCESS lpProcess );
	LPPROCESS lpProcess;
    
	if( hProcess == CURRENT_PROCESS_HANDLE )
		hProcess = lpCurThread->lpCurProcess->hProcess;

	lpProcess = HandleToPtr( hProcess, OBJ_PROCESS );
	if( lpProcess )
	{
		if( lpCurThread->lpCurProcess == lpProcess  )
		{
			KL_ExitProcess(uExitCode);
		}
		else
		{
			KL_TerminateThread( lpProcess->lpMainThread->hThread, uExitCode );
		}
	}
    //  not return here
}


// ********************************************************************
//������ HANDLE WINAPI KL_GetProcessHeap( void )
//������
//	��
//����ֵ��
//	����ɹ������ؽ��̵�Ĭ�϶Ѿ�������򣬷���NULL
//����������
//	���ؽ��̵�Ĭ�϶�
//���ã�
//	ϵͳAPI
// ********************************************************************

HANDLE WINAPI KL_GetProcessHeap( void )
{
	LPPROCESS lpProcess;
	lpProcess = GetAPICallerProcessPtr();
	if( lpProcess )
		return lpProcess->lpHeap;
	else
		return NULL;
}

// ********************************************************************
//������DWORD WINAPI KL_GetCurrentProcessId( void ) 
//������
//	��
//����ֵ��
//	���ؽ���id
//����������
//	�õ���ǰ�Ľ���id
//���ã�
//	ϵͳAPI
// ********************************************************************

DWORD WINAPI KL_GetCurrentProcessId( void )
{
	return GetAPICallerProcessPtr()->dwProcessId;
	
}

// ********************************************************************
//������BOOL WINAPI KL_GetExitCodeProcess( HANDLE hProcess, LPDWORD lpdwExitCode ) 
//������
//	IN hProcess - ���̾��
//	OUT lpdwExitCode - ���ڽ��ܽ����˳�����
//����ֵ��
//	����ɹ�������TRUE��lpdwExitCode�����˳����룻���򣬷���FALSE
//����������
//	�õ���ǰ���̵��˳�����
//���ã�
//	ϵͳAPI
// ********************************************************************

BOOL WINAPI KL_GetExitCodeProcess( HANDLE hProcess, LPDWORD lpdwExitCode )
{
	LPPROCESS lpProcess;
	if( hProcess == CURRENT_PROCESS_HANDLE )//2003-05-22, ADD
	{
		*lpdwExitCode = STILL_ACTIVE;
		return TRUE;
	}
	lpProcess = HandleToPtr( hProcess, OBJ_PROCESS );
	if( lpProcess )
	{
		*lpdwExitCode = STILL_ACTIVE;
		return TRUE;
	}
	else
	{	// ��������
	    return GetHandleUserInfo( hProcess, lpdwExitCode );
	}
}

// ********************************************************************
//������ LPVOID WINAPI KL_MapPtrToProcess( LPVOID lpv, HANDLE hProcess )
//������
//	IN lpv - ��Ҫӳ���ָ��
//	IN hProcess - ָ��ָ��ĵ�ַ�����Ľ��̵ľ��
//����ֵ��
//	����ɹ�������ӳ����ָ�룻���򣬷���NULL
//����������
//	ӳ����̵�ָ�룬ʹ�ø�ָ���ܱ��������̷���
//���ã�
// ********************************************************************

LPVOID WINAPI KL_MapPtrToProcess( LPVOID lpv, HANDLE hProcess )
{
#ifdef VIRTUAL_MEM
	LPPROCESS lpProcess;

	if( hProcess == CURRENT_PROCESS_HANDLE )
		lpProcess = lpCurThread->lpCurProcess;
	else
		lpProcess = (LPPROCESS)HandleToPtr( hProcess, OBJ_PROCESS );
	// ӳ��ָ��
    if( lpProcess )
		lpv = MapProcessPtr( lpv, lpProcess );
    else 
		lpv = 0;
    return lpv;

#else
	return lpv;
#endif
}

// ********************************************************************
//������LPVOID WINAPI KL_UnMapPtr( LPVOID lpv ) 
//������
//	IN lpv - ֮ǰ��ӳ���ָ��
//����ֵ��
//	����ɹ���������ӳ���ָ�룻���򣬷���NULL
//����������
//	��KL_MapPtrToProcess��Ӧ�� ��֮ǰ��ӳ��ָ�뻹ԭ
//���ã�
//	ϵͳAPI
// ********************************************************************

LPVOID WINAPI KL_UnMapPtr( LPVOID lpv )
{
	return UnMapProcessPtr( lpv );
}

// ********************************************************************
//������ HANDLE WINAPI KL_GetCurrentProcess( void )
//������
//	��
//����ֵ��
//	���ص�ǰ���̵ı���
//����������
//	�õ���ǰ���̾��
//���ã�
//	ϵͳAPI
// ********************************************************************

HANDLE WINAPI KL_GetCurrentProcess( void )
{
	//return GetAPICallerProcessPtr()->hProcess;//2003-05-22, DEL
	return CURRENT_PROCESS_HANDLE;//2003-05-22, ADD
}

// ********************************************************************
//������HANDLE WINAPI KL_GetOwnerProcess( void ) 
//������
//	��
//����ֵ��
//	����ɹ������ص�ǰ�̵߳�ӵ���߽��̣��������̵߳Ľ��̣�
//����������
//	�õ���ǰ�̵߳�ӵ���߽��̣��������̵߳Ľ��̣�
//���ã�
//  ϵͳAPI
// ********************************************************************

HANDLE WINAPI KL_GetOwnerProcess( void )
{
    return lpCurThread->lpOwnerProcess->hProcess;//
}

// ********************************************************************
//������ LPPROCESS GetHandleOwner( void )
//������
//	��
//����ֵ��
//	ӵ���߽��̽ṹָ��
//����������
//	�õ������ӵ����
//���ã�
//  ���������(hmgr.c)
// ********************************************************************
/* 2004-10-22
LPPROCESS GetHandleOwner( void )
{
    CALLSTACK * lpcs = lpCurThread->lpCallStack;	

	if( lpcs )
	    return (LPPROCESS)lpcs->lpvData;
	else
		return lpCurThread->lpCurProcess;  // the must kernel
}
*/
LPPROCESS GetHandleOwner( void )
{
    CALLSTACK * lpcs = lpCurThread->lpCallStack;	
	while( lpcs && (lpcs->dwCallInfo & CALL_KERNEL) )
		lpcs = lpcs->lpNext;
	return lpcs ? (LPPROCESS)lpcs->lpvData : NULL;
    //return lpcs ? ( (LPPROCESS)lpcs->lpvData )->hProcess : NULL;
/* origin
    CALLSTACK * lpcs = lpCurThread->lpCallStack;	

	if( lpcs )
	    return (LPPROCESS)lpcs->lpvData;
	else
		return lpCurThread->lpCurProcess;  // the must kernel
		*/
}


// ********************************************************************
//������BOOL SwitchToProcess( LPPROCESS lpProcess, LPCALLSTACK lpcs )
//������
//	IN  lpProcess - ���̽ṹָ��
//	OUT lpcs - ���ڱ�����������ĵ�CALLSTACK�ṹ, ����ΪNULL
//����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
//����������
//	�л����µĽ��̿ռ�
//���ã�
//	����API����ʱ���õ��ú��������û�����APIʱ�����Ȼ��л���API������̿ռ䣬
//	��APIִ����󣬽���ָ����û��ռ�
// ********************************************************************

BOOL SwitchToProcess( LPPROCESS lpProcess, LPCALLSTACK lpcs )
{
	//LPPROCESS lpProcess;
	UINT uiSave;
	BOOL bRetv = FALSE;
	
	//ASSERT( lpProcess );

	if( lpProcess == &InitKernelProcess )//KERNEL_PROCESS_HANDLE )
	{   // �л����ں�
		if( lpcs )
		{
			LockIRQSave( &uiSave );
			
			lpcs->lpvData = lpCurThread->lpCurProcess;
			lpcs->lpNext = lpCurThread->lpCallStack;
			lpcs->dwCallInfo = CALL_KERNEL;
			AccessKey_Set( &lpcs->akyAccessKey, lpCurThread->akyAccessKey );
			lpCurThread->lpCallStack = lpcs;			
			
			UnlockIRQRestore( &uiSave );
		}
		
		bRetv = TRUE;
	}
	else
	{	// �л�������
		//lpProcess = HandleToPtr( hProcess, OBJ_PROCESS );
		//if( lpProcess )
		{   
			BOOL bSwitchContext;
//			ASSERT( lpCurThread->lpCurProcess  );
	
			// ����µĽ������̵߳ĵ�ǰ���̲�ͬ������Ҫ�л����̿ռ�
			if( lpProcess != lpCurThread->lpCurProcess )
				bSwitchContext = TRUE;
			else
				bSwitchContext = FALSE;

			LockIRQSave( &uiSave );

			if( lpcs )
			{				
				// ����CALLSTACK�ṹ
				lpcs->dwCallInfo = 0;
				lpcs->lpvData = lpCurThread->lpCurProcess;
				AccessKey_Set( &lpcs->akyAccessKey, lpCurThread->akyAccessKey );

				// only test
				//if( lpcs )
				//{
				//	DWORD myAky = lpCurThread->lpCurProcess->akyAccessKey | lpCurThread->lpOwnerProcess->akyAccessKey;
				//	if( (lpcs->akyAccessKey & myAky) != myAky )
				//	{
				//		RETAILMSG( 1, ( "error: lpcs->akyAccessKey=0x%x,myAky=0x%x,CurProc=0x%x,OwnerProc=0x%x.\r\n",lpcs->akyAccessKey,myAky,lpCurThread->lpCurProcess->akyAccessKey,lpCurThread->lpOwnerProcess->akyAccessKey ) );
				//	}
				//}
				//


				// ���������
				lpcs->lpNext = lpCurThread->lpCallStack;		
				lpCurThread->lpCallStack = lpcs;
			}
			// ��lpProcess��Ϊ��ǰ�̵߳ĵ�ǰ����
		//	lpCurThread->lpCurProcess = lpProcess;




			if( bSwitchContext )
			{
//#ifdef VIRTUAL_MEM
				//װ�뵱ǰ���̵�MMU�й����ݣ���ЧCACHE������
				//2004-09-20��ln.?????-�� GetMMUContext ��Ҳ����δ��룬���ڿ���������û�б�Ҫ��
				//�п����ǵ���Ϊ�˽��һ��������ӵġ�
	           // dwProcessSlots[0] = (DWORD)lpProcess->lpSegIndex->lpSeg;
	           // SetCPUId( lpProcess->dwVirtualAddressBase );
				//?????
				AccessKey_Add( &lpCurThread->akyAccessKey, lpProcess->akyAccessKey );
			    GetMMUContext( lpCurThread, 1, lpProcess );
//#endif
            }
	        
			UnlockIRQRestore( &uiSave );

			bRetv = TRUE;
		}
	}
	return bRetv;
}

// ********************************************************************
//������BOOL SwitchToProcessByHandle( LPPROCESS lpProcess, LPCALLSTACK lpcs )
//������
//	IN  lpProcess - ���̽ṹָ��
//	OUT lpcs - ���ڱ�����������ĵ�CALLSTACK�ṹ, ����ΪNULL
//����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
//����������
//	�л����µĽ��̿ռ�
//���ã�
//	����API����ʱ���õ��ú��������û�����APIʱ�����Ȼ��л���API������̿ռ䣬
//	��APIִ����󣬽���ָ����û��ռ�
// ********************************************************************

BOOL SwitchToProcessByHandle( HANDLE hProcess, LPCALLSTACK lpcs )
{
	LPPROCESS lpProcess = HandleToPtr( hProcess, OBJ_PROCESS );
	if( lpProcess )
	{
		return SwitchToProcess( lpProcess, lpcs );
	}
	return FALSE;
}



// ********************************************************************
//������ void SwitchBackProcess( void )
//������
//	��
//����ֵ��
//	��
//����������
//	��SwitchToProcess���Ӧ���л��ص����߽���
//���ã�
//	����API����ʱ���õ��ú��������û�����APʱ�����Ȼ��л���API������̿ռ䣬
//	��APIִ����󣬽���ָ����û��ռ�
// ********************************************************************

void SwitchBackProcess( void )
{
	LPPROCESS lpProcess;
	UINT uiSave;
	BOOL bSwitchContext;
	ACCESS_KEY aky;


//	ASSERT(lpCurThread->lpCallStack);

	LockIRQSave( &uiSave );

	lpProcess = (LPPROCESS)lpCurThread->lpCallStack->lpvData;
	aky = lpCurThread->lpCallStack->akyAccessKey;
	// ����µĽ������̵߳ĵ�ǰ���̲�ͬ������Ҫ�л����̿ռ�
	//ASSERT( lpProcess != &InitKernelProcess );
	if( 
		//lpProcess != &InitKernelProcess && 
		( lpProcess != lpCurThread->lpCurProcess || 
		  aky != lpCurThread->akyAccessKey ) )
		bSwitchContext = TRUE;
	else
		bSwitchContext = FALSE;
	// ���������
    lpCurThread->lpCallStack = lpCurThread->lpCallStack->lpNext;


    //lpCurThread->lpCurProcess = lpProcess;

	// only test
	//{
	//	DWORD myAky = lpCurThread->lpCurProcess->akyAccessKey | lpCurThread->lpOwnerProcess->akyAccessKey;
	//	if( (aky & myAky) != myAky )
	//	{
	//		RETAILMSG( 1, ( "error: aky=0x%x,myAky=0x%x,CurProc=0x%x,OwnerProc=0x%x.\r\n",aky,myAky,lpCurThread->lpCurProcess->akyAccessKey,lpCurThread->lpOwnerProcess->akyAccessKey ) );
	//	}
	//}

	
    
	
	
	
	//

	if( bSwitchContext )
	{
//#ifdef VIRTUAL_MEM
		
		//װ�뵱ǰ���̵�MMU�й����ݣ���ЧCACHE������
		//2004-12-30��ln.?????-�� GetMMUContext ��Ҳ����δ��룬���ڿ���������û�б�Ҫ��
		//�п����ǵ���Ϊ�˽��һ��������ӵġ�
        //dwProcessSlots[0] = (DWORD)lpProcess->lpSegIndex->lpSeg;
        //SetCPUId( lpProcess->dwVirtualAddressBase );
		//
		AccessKey_Set( &lpCurThread->akyAccessKey, aky );
		GetMMUContext( lpCurThread, 2, lpProcess );
//#endif
	}

	UnlockIRQRestore( &uiSave );

}

// ********************************************************************
//������HANDLE WINAPI KL_GetCallerProcess( void ) 
//������
//	�� 
//����ֵ��
//	����API�����ߵĽ��̾�������û�У�����NULL
//����������
//	�õ�API�����ߵĽ��̾��
//���ã�
//	ϵͳAPI
// ********************************************************************

// ap->server->kernel->getcaller
// ap->kernel->other->getcaller

HANDLE WINAPI KL_GetCallerProcess( void )
{
	LPPROCESS lpProcess = GetHandleOwner();
	if( lpProcess )
		return lpProcess->hProcess;
	return NULL;
	/*
    CALLSTACK * lpcs = lpCurThread->lpCallStack;	
	while( lpcs && (lpcs->dwCallInfo & CALL_KERNEL) )
		lpcs = lpcs->lpNext;
    return lpcs ? ( (LPPROCESS)lpcs->lpvData )->hProcess : NULL;
	*/
}

// ********************************************************************
//������LPTSTR WINAPI KL_GetCommandLine(VOID) 
//������
//	��
//����ֵ��
//	����ɹ���������Ч��ָ�룻���򣬷���NULL
//����������
//	�õ������в���
//���ã�
//	ϵͳAPI
// ********************************************************************
#define DEBUG_GET_COMMAND_LINE 0
LPTSTR WINAPI KL_GetCommandLine(VOID)
{
	//LOCK_SCHE();
	DEBUGMSG( DEBUG_GET_COMMAND_LINE,( "KL_GetCommandLine:0x%x,%s.\r\n", lpCurThread->lpCurProcess->lpszCommandLine, lpCurThread->lpCurProcess->lpszCommandLine ) );
	//UNLOCK_SCHE();
	return (LPTSTR)lpCurThread->lpCurProcess->lpszCommandLine;
}

// ********************************************************************
//������LPVOID WINAPI KL_GetProcessUserData( VOID ) 
//������
//	��
//����ֵ��
//	����֮ǰ��KL_SetProcessUserData���õ�ֵ
//����������
//	���ص�ǰ����֮ǰ��KL_SetProcessUserData���õ�ֵ
//���ã�
//	ϵͳAPI
// ********************************************************************

LPVOID WINAPI KL_GetProcessUserData( VOID )
{
	return lpCurThread->lpCurProcess->lpUserData;
}

// ********************************************************************
//������LPVOID WINAPI KL_SetProcessUserData( LPVOID lpvData ) 
//������
//	IN lpvData - ��Ҫ���õ��û�ֵ
//����ֵ��
//	����֮ǰ���õ�ֵ
//����������
//	���ý����û�����
//���ã�
//	ϵͳAPI
// ********************************************************************

LPVOID WINAPI KL_SetProcessUserData( LPVOID lpvData )
{
	LPVOID lpvRetv = lpCurThread->lpCurProcess->lpUserData;
	lpCurThread->lpCurProcess->lpUserData = lpvData;
	return lpvRetv;
}


// ********************************************************************
//������HANDLE WINAPI KL_OpenProcess( DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwProcessId )
//������
//	IN dwDesiredAccess - ��ȡȨ�ޣ���֧�֣�����Ϊ0��
//	IN bInheritHandle - �̳б�־����֧�֣�����ΪFALSE��
//	IN dwProcessId - ����ID
//����ֵ��
//	����ɹ������ؽ��̵ľ����ʧ�ܣ�����NULL
//����������
//	ͨ������ID�õ����̾��
//���ã�
//	ϵͳAPI
// ********************************************************************

HANDLE WINAPI KL_OpenProcess( DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwProcessId )
{
	if( dwProcessId < MAX_PROCESSES && dwProcessId > 0 )
	{
		//LPPROCESS lpProcess = lppProcessPtr[dwProcessId] ? lppProcessPtr[dwProcessId]->hProcess : NULL;
		LPPROCESS lpProcess;
		UINT uiSave;

		LockIRQSave( &uiSave );
		lpProcess = lppProcessPtr[dwProcessId] ? lppProcessPtr[dwProcessId] : NULL;
		if( lpProcess )
		{
			Handle_AddRef( lpProcess->hProcess );
		}
		UnlockIRQRestore( &uiSave );
		if( lpProcess )
			return lpProcess->hProcess;
	}
	return NULL;
}


// ********************************************************************
//������DWORD WINAPI KL_GetProcessId( HANDLE hProcess )
//������
//	IN hProcess - ���̾��
//����ֵ��
//	����ɹ������ؽ��̵�ID��ʧ�ܣ����� 0xffffffff
//����������
//	ͨ�����̾���ĵõ�����ID
//���ã�
//	ϵͳAPI
// ********************************************************************

#define DEBUG_KL_GetProcessId 0
DWORD WINAPI KL_GetProcessId( HANDLE hProcess )
{
    LPPROCESS lpProcess = HandleToPtr( hProcess, OBJ_PROCESS );
    DEBUGMSG( DEBUG_KL_GetProcessId, ( "KL_GetProcessId: hProcess=0x%x entry.\r\n",hProcess ) );
    if( lpProcess ) 
	{
		return lpProcess->dwProcessId;
    }
	else
	{
		return 0xffffffff;
	}
}

