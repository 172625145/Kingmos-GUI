/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵�������ع��̵���(LPC)����
�汾�ţ�2.0.0
����ʱ�ڣ�2000
���ߣ�����
�޸ļ�¼��
	2005-01-18, ���� KC_EnterAPI and KC_EnterAPIHandle
	2004-12-20, ���� ackAccessKey ����
    2003-10-07, LN, ֱ��Call _EnterAPI
    2003-07-12, LN, ��KL_EnterAPI & KL_EnterAPIHandle ��ΪCall _EnterAPI
    2003-07-01, LN ,KL_EnterAPIHandle & KL_EnterAPI 
	            ���ܷ��� *apiInfo[uiAPIId].lpfn
******************************************************/


#include <eframe.h>
#include <eapisrv.h>
#include <eucore.h>
#include <epcore.h>
#include <epalloc.h>
#include <eobjcall.h>
#include <coresrv.h>
#include <sysintr.h>

// ����API�����API
static const PFNVOID lpAPIAPI[] = {
	NULL,

	(PFNVOID)KL_RegisterAPI,
	(PFNVOID)KL_UnregisterAPI,
#ifdef CALL_TRAP
    NULL,
	NULL,
	NULL,
#else
	(PFNVOID)KL_EnterAPI,
	(PFNVOID)KL_EnterAPIHandle,
	(PFNVOID)KL_LeaveAPI,
#endif
	(PFNVOID)KL_RegisterAPIEx,
	(PFNVOID)KL_IsAPIReady,
	(PFNVOID)KL_SetAPIReady
};

static const DWORD dwWndArgs[] = {
	NULL,
    ARG3_MAKE( DWORD, PTR, DWORD  ),//KL_RegisterAPI
    ARG1_MAKE( DWORD ),//KL_UnregisterAPI
    ARG4_MAKE( DWORD, DWORD, PTR, PTR ),//KL_EnterAPI
	ARG5_MAKE( DWORD, DWORD, PTR, PTR, PTR ),//KL_EnterAPIHandle
	ARG0_MAKE(),//KL_LeaveAPI
	ARG4_MAKE( DWORD, PTR, PTR, DWORD  ),//KL_RegisterAPIEx
	ARG1_MAKE( DWORD ),//KL_IsAPIReady
	ARG1_MAKE( DWORD ),//KL_SetAPIReady
};

// ����ϵͳ����api��
APIINFO apiInfo[MAX_API] = { {lpAPIAPI,dwWndArgs,sizeof(lpAPIAPI)/sizeof(PFNVOID),&InitKernelProcess}, };
BYTE   bAPISetState[MAX_API] = { 1, };

// ********************************************************************
//������BOOL _InitAPIMgr( void )
//��������
//����ֵ������ɹ�������TURE�����򣬷���FALSE
//����������API����ģ���ʼ��
//����: ��InitialKingmosSystem����(system.c)
// ********************************************************************
BOOL _InitAPIMgr( void )
{
	memset( bAPISetState, 0, sizeof( bAPISetState ) );
	return TRUE;
}

// ********************************************************************
//������HANDLE DoRegisterAPI( UINT uiAPIId, PFNVOID lpfn, const DWORD * lpArgs, UINT uiOptions )
//������
// IN uiAPIId -����ID
// IN lpfn -ָ�����������ָ��
// IN lpArgs -ָ����������Ĳ�����Ϣָ��
// IN uiOptions -������
//����ֵ������ɹ���������Ч�ľ��ֵ�����𷵻�NULL
//����������ע�����
//����: KL_RegisterAPI & KL_RegisterAPIEx
// ********************************************************************
static HANDLE DoRegisterAPI( UINT uiAPIId, const PFNVOID * lpfn, const DWORD * lpArgs, UINT uiOptions )
{
	if( uiAPIId < MAX_API && lpfn )
	{
		lpfn = (const PFNVOID *)MapProcessPtr( lpfn, GetAPICallerProcessPtr() );
        lpArgs = (const DWORD *)MapProcessPtr( lpArgs, GetAPICallerProcessPtr() );
		if( KL_InterlockedCompareExchange( (LPLONG)&apiInfo[uiAPIId].lpfn, (LONG)lpfn, 0 ) == 0 )
		{
			apiInfo[uiAPIId].lpArgs = lpArgs;
			apiInfo[uiAPIId].lpProcess = GetAPICallerProcessPtr();//lpCurThread->lpCurProcess;
			apiInfo[uiAPIId].uiMaxOption = uiOptions;
			apiInfo[uiAPIId].hServer = (HANDLE)uiAPIId;
			apiInfo[uiAPIId].lpServerCtrlInfo = (LPSERVER_CTRL_INFO)(*lpfn);
			if( apiInfo[uiAPIId].lpServerCtrlInfo )
			    apiInfo[uiAPIId].dwServerInfo = apiInfo[uiAPIId].lpServerCtrlInfo->dwInfo;
			return (HANDLE)uiAPIId;//&apiInfo[uiAPIId];
		}
	}
	return NULL;
}

// ********************************************************************
//������HANDLE WINAPI KL_RegisterAPI( UINT uiAPIId, PFNVOID lpfn, UINT uiOptions )
//������
// IN uiAPIId -����ID
// IN lpfn -ָ�����������ָ��
// IN uiOptions -������
//����ֵ������ɹ���������Ч�ľ��ֵ�����𷵻�NULL
//����������ע�����
// ********************************************************************
HANDLE WINAPI KL_RegisterAPI( UINT uiAPIId, const PFNVOID * lpfn, UINT uiOptions )
{
	return DoRegisterAPI( uiAPIId, lpfn, 0, uiOptions );
}

// ********************************************************************
//������HANDLE WINAPI KL_RegisterAPIEx( UINT uiAPIId, PFNVOID lpfn, const DWORD * lpArgs, UINT uiOptions )
//������
// IN uiAPIId -����ID
// IN lpfn -ָ�����������ָ��
// IN lpArgs -ָ����������Ĳ�����Ϣָ��
// IN uiOptions -������
//����ֵ������ɹ���������Ч�ľ��ֵ�����𷵻�NULL
//����������ע�����
// ********************************************************************
HANDLE WINAPI KL_RegisterAPIEx( UINT uiAPIId, const PFNVOID * lpfn, const DWORD * lpArgs, UINT uiOptions )
{
	return DoRegisterAPI( uiAPIId, lpfn, lpArgs, uiOptions );
}

// ********************************************************************
//������BOOL WINAPI KL_IsAPIReady( UINT uiAPI )
//������
// IN uiAPI -����ID
//����ֵ��
// ����ɹ�������TRUE�����򣬷���FALSE
//�����������ж�ĳAPI�����Ƿ�׼����
//����: ϵͳ����
// ********************************************************************
BOOL WINAPI KL_IsAPIReady( UINT uiAPI )
{
	if( uiAPI < MAX_API )
	    return bAPISetState[uiAPI];
	return FALSE;
}

// ********************************************************************
//������BOOL WINAPI KL_SetAPIReady( UINT uiAPI )
//������
// IN uiAPI -����ID
//����ֵ��
// ����ɹ�������TRUE�����򣬷���FALSE
//�����������趨ĳAPI������׼���ã��û�����ʹ��
//����: ϵͳ����
// ********************************************************************
BOOL WINAPI KL_SetAPIReady( UINT uiAPI )
{
	if( uiAPI < MAX_API )
	{
        bAPISetState[uiAPI] = 1;
		return TRUE;
	}
	return FALSE;
}

// ********************************************************************
//������BOOL WINAPI KL_UnregisterAPI( HANDLE hAPI )
//������
// IN hAPI -Ϊ����KL_RegisterAPIEx���صľ��
//����ֵ��
// ����ɹ�������TRUE�����򣬷���FALSE
//����������ע��һ��API����
//����: ϵͳ����
// ********************************************************************

BOOL WINAPI KL_UnregisterAPI( HANDLE hAPI )
{
	if( (DWORD)hAPI && (DWORD)hAPI < MAX_API )
	{
		APIINFO * lpAPI = &apiInfo[(DWORD)hAPI];
		if( lpAPI->lpProcess == GetAPICallerProcessPtr() )
		{
			KL_InterlockedExchange( (LPLONG)&lpAPI->lpfn, 0 );
			return TRUE;
		}
		KL_SetLastError( ERROR_ACCESS_DENIED );
	}
	return FALSE;

}

// ********************************************************************
//������_EnterAPI( UINT uiAPIId, UINT uiOptionId, VOID * lpfn, CALLSTACK * lpcs )
//������
// IN uiAPIId -����ID
// IN uiOptionId -��������ID
// OUT lpfn -���ڽ��ܺ������õ�ַ
// OUT lpcs -CALLSTACK�ṹָ�룬���ڱ������������
//����ֵ��
//   ����ɹ�������TRUE�����򣬷���FALSE
//�����������ú���ִ�����²�����
// 1.�жϲ����Ƿ���Ч
// 2.�л����������������
// 3.����ĳAPI����Ӧ�ĺ���
//����: KL_EnterAPIHandle & KL_EnterAPI
// ********************************************************************
static BOOL _EnterAPI( UINT uiAPIId, UINT uiOptionId, VOID * lpfn, CALLSTACK * lpcs )
{
	if( uiAPIId < MAX_API )
	{  //�жϲ����Ƿ���Ч
		if( 
			uiOptionId && 
			uiOptionId < apiInfo[uiAPIId].uiMaxOption )
		{
			const PFNVOID * lppfn;
			
			lppfn = apiInfo[uiAPIId].lpfn;			

			if( lppfn ) //&& ( *( (PFNVOID*)lpfn ) = *( lppfn + uiOptionId ) ) )
			{  // �л����������������
				ACCESS_KEY aky = lpCurThread->akyAccessKey;
				LPPROCESS lpProcess = apiInfo[uiAPIId].lpProcess;

				//���� lpCurThread �� Server Process �ռ�Ĵ�ȡȨ��
				AccessKey_Add( &lpCurThread->akyAccessKey, lpProcess->akyAccessKey );
				*( (PFNVOID*)lpfn ) = *( lppfn + uiOptionId );
				AccessKey_Set( &lpCurThread->akyAccessKey, aky );  //�ָ�
				if( *( (PFNVOID*)lpfn ) )
				{
					if( SwitchToProcess( lpProcess, lpcs ) )
					{
						return TRUE;
					}
					else
						*( (PFNVOID*)lpfn ) = NULL;
				}				
			}
		}
	}
	KL_SetLastError( ERROR_INVALID_FUNCTION );			
	return 0;
}

static OutErrorAPIInfo( UINT uiAPIId, UINT uiOptionId )
{
	#ifdef INLINE_PROGRAM
		{
			extern void KL_EdbgOutputDebugString( const char * lpszFormat, ... );
			KL_EdbgOutputDebugString( "error: no the api[%d] function[%d].\r\n" , uiAPIId, uiOptionId );
		}
	#else
		RETAILMSG( 1, ( "error: no the api[%d] function[%d].\r\n" , uiAPIId, uiOptionId ) );
	#endif
}

// ********************************************************************
//������BOOL WINAPI KL_EnterAPIHandle( UINT uiAPIId, UINT uiOptionId, VOID * lpfn, CALLSTACK * lpcs, HANDLE * lpHandle )
//������
// IN uiAPIId -����ID
// IN uiOptionId -��������ID
// OUT lpfn -���ڽ��ܺ������õ�ַ
// OUT lpcs -CALLSTACK�ṹָ�룬���ڱ������������
// OUT lpHandle-���ָ��
//����ֵ��
//   ����ɹ�������TRUE�����򣬷���FALSE
//����������
// ����_EnterAPI�������ת��Ϊ��������Ҫ������
//����: ϵͳ����
// ********************************************************************
BOOL WINAPI KL_EnterAPIHandle( UINT uiAPIId, UINT uiOptionId, VOID * lpfn, CALLSTACK * lpcsRet, HANDLE * lpHandle )
{
	CALLSTACK * lpcall;
	
	// ����һ������stack,���ڱ���ϵͳ����������
	lpcall = KHeap_Alloc( sizeof( CALLSTACK ) );
	if( lpcall )
	{
		if( _EnterAPI( uiAPIId, uiOptionId, lpfn, lpcall ) )
		{			
			lpcall->dwCallInfo |= MAKE_OPTIONINFO( uiAPIId, uiOptionId );
			*lpcsRet = *lpcall;
			lpcall->dwCallInfo |= CALL_ALLOC;
			
			*lpHandle = HandleToPtr( *lpHandle, -1 );
			return TRUE;
		}
		else
		{
			KHeap_Free( lpcall, sizeof( CALLSTACK ) );
			OutErrorAPIInfo( uiAPIId, uiOptionId );
		}
	}
	return FALSE;
}

// ********************************************************************
//������BOOL WINAPI KC_EnterAPIHandle( UINT uiAPIId, UINT uiOptionId, VOID * lpfn, CALLSTACK * lpcs, HANDLE * lpHandle )
//������
// IN uiAPIId -����ID
// IN uiOptionId -��������ID
// OUT lpfn -���ڽ��ܺ������õ�ַ
// OUT lpcs -CALLSTACK�ṹָ�룬���ڱ������������
// OUT lpHandle-���ָ��
//����ֵ��
//   ����ɹ�������TRUE�����򣬷���FALSE
//����������
// ����_EnterAPI�������ת��Ϊ��������Ҫ������
//����: ϵͳ����
// ********************************************************************
BOOL WINAPI KC_EnterAPIHandle( UINT uiAPIId, UINT uiOptionId, VOID * lpfn, CALLSTACK * lpcsRet, HANDLE * lpHandle )
{
	CALLSTACK * lpcall;
	
	// ����һ������stack,���ڱ���ϵͳ����������
	lpcall = KHeap_Alloc( sizeof( CALLSTACK ) );
	if( lpcall )
	{		
		if( _EnterAPI( uiAPIId, uiOptionId, lpfn, lpcall ) )
		{
			lpcall->dwCallInfo |= MAKE_OPTIONINFO( uiAPIId, uiOptionId );
			*lpcsRet = *lpcall;
			lpcall->dwCallInfo |= CALL_ALLOC;
	
			*lpHandle = HandleToPtr( *lpHandle, -1 );
			return TRUE;
		}
		else
		{
			KHeap_Free( lpcall, sizeof( CALLSTACK ) );
		}
	}
	return FALSE;
}


// ********************************************************************
//������BOOL WINAPI KL_EnterAPI( UINT uiAPIId, UINT uiOptionId, VOID * lpfn, CALLSTACK * lpcs )
//������
// IN uiAPIId -����ID
// IN uiOptionId -��������ID
// OUT lpfn -���ڽ��ܺ������õ�ַ
// OUT lpcs -CALLSTACK�ṹָ�룬���ڱ������������
//����ֵ��
//   ����ɹ�������TRUE�����򣬷���FALSE
//����������
// ����_EnterAPI
//����:ϵͳ���� 
// ********************************************************************
BOOL WINAPI KL_EnterAPI( UINT uiAPIId, UINT uiOptionId, VOID * lpfn, CALLSTACK * lpcsRet )
{
	CALLSTACK * lpcall;
	
	// ����һ������stack,���ڱ���ϵͳ����������
	lpcall = KHeap_Alloc( sizeof( CALLSTACK ) );
	if( lpcall )
	{		
		if( _EnterAPI( uiAPIId, uiOptionId, lpfn, lpcall ) )
		{
			lpcall->dwCallInfo |= MAKE_OPTIONINFO( uiAPIId, uiOptionId );
			*lpcsRet = *lpcall;
			lpcall->dwCallInfo |= CALL_ALLOC;			
			return TRUE;
		}
		else
		{
			KHeap_Free( lpcall, sizeof( CALLSTACK ) );
			OutErrorAPIInfo( uiAPIId, uiOptionId );
		}
	}
	return FALSE;
}

// ********************************************************************
//������BOOL WINAPI KC_EnterAPI( UINT uiAPIId, UINT uiOptionId, VOID * lpfn, CALLSTACK * lpcs )
//������
// IN uiAPIId -����ID
// IN uiOptionId -��������ID
// OUT lpfn -���ڽ��ܺ������õ�ַ
// OUT lpcs -CALLSTACK�ṹָ�룬���ڱ������������
//����ֵ��
//   ����ɹ�������TRUE�����򣬷���FALSE
//����������
// ����_EnterAPI
//����:ϵͳ���� 
// ********************************************************************
BOOL WINAPI KC_EnterAPI( UINT uiAPIId, UINT uiOptionId, VOID * lpfn, CALLSTACK * lpcsRet )
{
	CALLSTACK * lpcall;
	
	// ����һ������stack,���ڱ���ϵͳ����������
	lpcall = KHeap_Alloc( sizeof( CALLSTACK ) );
	if( lpcall )
	{
		if( _EnterAPI( uiAPIId, uiOptionId, lpfn, lpcall ) )
		{
			lpcall->dwCallInfo |= MAKE_OPTIONINFO( uiAPIId, uiOptionId );
			*lpcsRet = *lpcall;
			lpcall->dwCallInfo |= CALL_ALLOC;
			return TRUE;
		}
		else
		{
			KHeap_Free( lpcall, sizeof( CALLSTACK ) );
		}
	}
	return FALSE;
}


// ********************************************************************
//������DWORD FASTCALL EnumServer( LPSERVER_ENUM_PROC lpEnumFunc, LPVOID lpParam )
//������
// IN lpEnumFunc-��Ҫ�ص���ö�ٹ���
// IN lpParam-���ݸ�lpEnumFunc���ܵĲ���
//����ֵ��
// �ɹ�����0
//����������ö��������ע��ķ��񣬼���lpEnumFunc����FALSE,����ֹ����ö��
//����: 
// ********************************************************************
BOOL FASTCALL EnumServer( LPSERVER_ENUM_PROC lpEnumFunc, LPVOID lpParam )
{
	int i;
	APIINFO * lpAPI = &apiInfo[MAX_API-1];

    for( i = MAX_API-1; i >= 8; i--, lpAPI-- )
	{
	    if( lpAPI->lpfn && lpAPI->lpServerCtrlInfo )
		{  // valid server
            if( lpEnumFunc( lpAPI, lpParam ) == FALSE )
				return FALSE;
		}
	}
	return TRUE;
}

// ********************************************************************
//������DWORD FASTCALL EnumServerHandler( DWORD dwIoCtl, DWORD dwParam, LPVOID lpParam )
//������
//	IN dwIoCtl - ��Ҫ�ص���ö�ٹ���
//	IN dwParam - ����ServerHandler��dwParam����
//	IN lpParam - ����ServerHandler��lpParam����
//����ֵ��
//	�ɹ�����0
//����������
//	ö��������ע��ķ����ServerHandler
//����: 
// ********************************************************************
DWORD FASTCALL EnumServerHandler( DWORD dwEventCode, DWORD dwParam, LPVOID lpParam )
{
	int i;
	APIINFO * lpAPI = &apiInfo[MAX_API-1];
	CALLSTACK * lpcs = KHeap_Alloc( sizeof( CALLSTACK ) );

    for( i = MAX_API-1; i >= 8; i--, lpAPI-- )
	{
		if( lpAPI->lpfn && lpAPI->lpServerCtrlInfo )
		{  // valid server
			//CALLSTACK cs;
			BOOL  bHandle = TRUE;
			DWORD dwServerInfo = lpAPI->dwServerInfo;

			if( dwServerInfo & (SCI_NOT_HANDLE_PROCESS_EXIT|SCI_NOT_HANDLE_THREAD_EXIT) )
			{	// �������߳��˳� �� �����˳��¼�
				if( ( dwEventCode == SCC_BROADCAST_PROCESS_EXIT && 
					(dwServerInfo & SCI_NOT_HANDLE_PROCESS_EXIT) ) ||
					( dwEventCode == SCC_BROADCAST_THREAD_EXIT && 
					(dwServerInfo & SCI_NOT_HANDLE_THREAD_EXIT) ) )
				{
					bHandle = FALSE;
				}
			}
			if( bHandle )
			{				
				if( SwitchToProcess( lpAPI->lpProcess, lpcs ) )
				{
					lpcs->dwCallInfo |= CALL_ALLOC;

					( (PSERVER_HANDLER)lpAPI->lpServerCtrlInfo->pServerHandler )( lpAPI->hServer, dwEventCode, (DWORD)dwParam, lpParam );
					SwitchBackProcess();
				}
			}
		}
	}
	KHeap_Free( lpcs, sizeof( CALLSTACK ) );
	return 0;
}


// ********************************************************************
//������PFNVOID MakeAPICall( DWORD dwCallInfo, LPDWORD lpdwCallSP, PFNVOID pfnRetAdress, UINT uiRetMode )
//������
// IN dwCallInfo-���������Ϣ
// IN lpdwCallSP-���ø÷����ܵĲ����б�lpdwCallSP[0] = ����0,lpdwCallSP[1]=����1,��������
// IN pfnRetAdress-���ø÷����ܵ��û��ķ��ص�ַ
// IN uiRetMode-���ø÷����ܵ��û��ķ���ģʽ(ָ����ģʽ��ϵͳģʽ���û�ģʽ,..)
//����ֵ��
// ����ɹ������ط����ܶ�Ӧ�Ĺ��ܵ�ַ(Ҳ����ΪNULL); ���򷵻�NULL
//����������
// �ú���ͨ������ϵͳ�ж�����á�
// ���û�����һ��������ʱ�������һ��ϵͳAPI�����жϣ������壩�������������Ϣ�����жϡ�
// ���жϽ�������Ϣ�����ݵĲ����б����ú��������ɸú���������ʲô������ã�
// �л���������̿ռ䣬ת����صĲ�������󣬷�����Ҫ���õķ����ܵ�ַ
//����: 
// ���жϻ��������
// ********************************************************************
/*
PFNVOID MakeAPICall( DWORD dwCallInfo, LPDWORD lpdwCallSP, PFNVOID pfnRetAdress, UINT uiRetMode )
{
	UINT uiAPIId = (dwCallInfo & APIID_MASK) >> APIID_SHIFT;
	UINT uiOptionId = (dwCallInfo & OPTION_MASK) >> OPTION_SHIFT;
	int  iArgs = (dwCallInfo & ARG_NUM_MASK) >> ARG_NUM_SHIFT;
	PFNVOID pfn;
	CALLSTACK * lpcs;
	BOOL bRetv;

   // RETAILMSG( 1, ( "MakeAPICall:dwCallInfo=0x%x,lpdwCallSP=0x%x,pfnRetAdress=0x%x,uiAPIId=%d,uiOptionId=%d.\r\n", dwCallInfo, lpdwCallSP, pfnRetAdress, uiAPIId, uiOptionId ) );

	pfn = 0; 
	// ����һ������stack,���ڱ���ϵͳ����������
	lpcs = KHeap_Alloc( sizeof( CALLSTACK ) );
	ASSERT( lpcs );

	if( lpcs && uiOptionId )
	{   //�ж��Ƿ��ǻ��ھ���ķ������
		if( IS_HANDLE_API( dwCallInfo ) )  // the first args in lpdwCallSP is a handle
		{   //�ǣ��õ����ú�����ַ������صĽ����л�
			if( _EnterAPI( uiAPIId, uiOptionId, &pfn, lpcs ) )
			{   // ת�����
				*lpdwCallSP = (DWORD)HandleToPtr( (HANDLE)*lpdwCallSP, -1 );
				bRetv = TRUE;
			}
			else
				bRetv = FALSE;						
		}
		else
		{   //�õ����ú�����ַ������صĽ����л�
			bRetv = _EnterAPI( uiAPIId, uiOptionId, &pfn, lpcs );
		}
		
		if( bRetv )
		{   // ת�������ߴ��ݵĲ���
			DWORD dwArg = apiInfo[uiAPIId].lpArgs[uiOptionId];
			//RETAILMSG( 1, ( "api:0x%x.\r\n", ((LPPROCESS)lpcs->lpvData)->dwVirtualAddressBase ) );
			
			if( uiAPIId > API_KERNEL )
			{
				for( ; dwArg; dwArg >>= 2 )
				{
					if( (dwArg & ARG_MASK) == ARG_PTR )
					{   //�����ָ�룬����ӳ����
						*lpdwCallSP = (DWORD)MapProcessPtr( *lpdwCallSP, (LPPROCESS)lpcs->lpvData );
					}
					lpdwCallSP++;
					iArgs--;
					DEBUGMSG( !(iArgs >= 0), ( "error (iArgs < 0 !) in MakeAPICall: uiAPIId(%d),uiOptionId(%d),dwCallInfo(0x%x),ArgNum(%d)", uiAPIId, uiOptionId, dwCallInfo, (dwCallInfo & ARG_NUM_MASK) >> ARG_NUM_SHIFT )  );
				}
			}
			//��������ߵķ��ص�ַ
			lpcs->pfnRetAdress = pfnRetAdress;
		}
		
		if( !bRetv )
		{   //���粻�ɹ�,���������
			KHeap_Free( lpcs, sizeof( CALLSTACK ) );
		}
		else
		{   // �ɹ������������Ϣ��CALL_ALLOC����lpcs�Ƕ�̬����ģ������Ӧ�õ���
			// KHeap_Freeȥ�����
			lpcs->dwCallInfo |= dwCallInfo | CALL_ALLOC;
			lpcs->dwStatus = uiRetMode;
		}
	}

	return pfn;
}
*/

// ********************************************************************
//������PFNVOID APICallReturn( DWORD * lpdwState )
//������
//	OUT lpdwState - ���״̬
//����ֵ��
// ���� MakeAPICall����ĵ����ߵķ��ص�ַ��pfnRetAdress����lpdwState����CPU״̬
//��������������������ܺ���˵������ߵ�������
//����: 
// ���жϻ��������
// ********************************************************************
PFNVOID APICallReturn( DWORD * lpdwState )
{
	CALLSTACK * lpcs = lpCurThread->lpCallStack;
    PFNVOID pfn = lpcs->pfnRetAdress;

//    ASSERT( lpcs );
	*lpdwState = lpcs->dwStatus;

//	RETAILMSG( 1, ( "APICallReturn(0x%x),lpcr(0x%x).\r\n", pfn, lpcr ) );
    //�л��������ߵĽ��̿ռ�
    SwitchBackProcess();

//	lpcr->lpRetSP = 0;
//	lpcr->uiRetMode = lpcs->uiRetMode;
	ASSERT( lpcs->dwCallInfo & CALL_ALLOC );
	//�ͷ�MakeAPICall�����CALLSTACK�ṹָ��
	KHeap_Free( lpcs, sizeof( CALLSTACK ) );
	//���ص������ߵĵ�ַ
	return pfn;
}

static VOID DoLeaveAPI( VOID )
{
	CALLSTACK * lpcs = lpCurThread->lpCallStack;

    //�л��������ߵĽ��̿ռ�
	SwitchBackProcess();
    ASSERT( (lpcs->dwCallInfo & CALL_ALLOC) );
	//�ͷ�MakeAPICall�����CALLSTACK�ṹָ��
	KHeap_Free( lpcs, sizeof( CALLSTACK ) );

	if( iISRActiveCount )
    {   
		INTR_OFF();
        ISR_Handler( ISR_ALL_INTRS );
		INTR_ON();
    }    
}

// ********************************************************************
//������void WINAPI KL_LeaveAPI( void )
//��������
//����ֵ����
//��������������������ܺ���˵������ߵ�������
//����: ϵͳ����
// ********************************************************************
void WINAPI KL_LeaveAPI( void )
{
	DoLeaveAPI();
}

// ********************************************************************
//������PFNVOID DoImplementCallBack4( LPICB * lpicb )
//������
//	IN/OUT lpicb - �������ģʽ
//  
//����ֵ��
//  ���� ���õ�ַ
//����������
//	�ı䵱ǰ�� CALLSTACK �ṹ����
//	�л������̣����ý��̹���
//����: 
// 
// ********************************************************************

PFNVOID DoImplementCallBack( LPSYSCALL lpCallInfo )
{
	HANDLE hProcess = ((LPCALLBACKDATA)lpCallInfo->uiArgs[0])->hProcess;
	LPPROCESS lpProcess  = HandleToPtr( hProcess, OBJ_PROCESS );
	PFNVOID pfn = (PFNVOID)((LPCALLBACKDATA)lpCallInfo->uiArgs[0])->lpfn;
	
	if( lpProcess && pfn )
	{	//�õ���ǰ��CS�ṹָ��
		UINT uiSave;
		BOOL bSwitchContext;
		CALLSTACK * lpcs = lpCurThread->lpCallStack;		
		
		LockIRQSave( &uiSave );

		// ����µĽ������̵߳ĵ�ǰ���̲�ͬ������Ҫ�л����̿ռ�
		if( lpProcess != lpCurThread->lpCurProcess )
		{
			bSwitchContext = TRUE;			
		}
		else
			bSwitchContext = FALSE;
		// ����CALLSTACK�ṹ
		lpcs->dwCallInfo &= ~CALL_KERNEL;
		lpcs->lpvData = lpCurThread->lpCurProcess;
		// ���������
		//lpcs->lpNext = lpCurThread->lpCallStack;		
		//lpCurThread->lpCallStack = lpcs;
		// ��lpProcess��Ϊ��ǰ�̵߳ĵ�ǰ����
	//	lpCurThread->lpCurProcess = lpProcess;  // �µĽ��̿ռ�
		
		if( bSwitchContext )
		{
//#ifdef VIRTUAL_MEM
			//װ�뵱ǰ���̵�MMU�й����ݣ���ЧCACHE������
			AccessKey_Add( &lpCurThread->akyAccessKey, lpProcess->akyAccessKey );
			GetMMUContext( lpCurThread, 1, lpProcess );
//#endif
        }

		UnlockIRQRestore( &uiSave );

		lpCallInfo->uiArgs[0] = ((LPCALLBACKDATA)lpCallInfo->uiArgs[0])->dwArg0;
		lpCallInfo->uiCallMode = GetCPUMode( lpProcess->dwFlags & 0xF );

		return pfn;
	}
	return NULL;
	
/*
	CALLSTACK * lpcs;

	lpcs = KHeap_Alloc( sizeof( CALLSTACK ) );
	if( lpcs )
	{
		HANDLE hProcess = ((LPCALLBACKDATA)lpCallInfo->uiArgs[0])->hProcess;
        if( SwitchToProcess( hProcess, lpcs ) )
		{
		    PFNVOID pfn = (PFNVOID)((LPCALLBACKDATA)lpCallInfo->uiArgs[0])->lpfn;
		    LPPROCESS lpProcess  = HandleToPtr( hProcess, OBJ_PROCESS );
			lpcs->pfnRetAdress = (PFNVOID)SYS_CALL_RETURN;
			lpcs->dwCallInfo |= MAKE_OPTIONINFO( API_KERNEL, SYS_IMPLEMENTCALLBACK4 ) | CALL_ALLOC;
			lpcs->dwStatus = GetCPUMode( M_SYSTEM );
			lpCallInfo->uiCallMode = GetCPUMode( lpProcess->dwFlags & 0xF );
			lpCallInfo->uiArgs[0] = ((LPCALLBACKDATA)lpCallInfo->uiArgs[0])->dwArg0;
			return pfn;
		}
		else
		{
			KHeap_Free( lpcs, sizeof( CALLSTACK ) );
		}
	}
	return NULL;
*/
}

// ********************************************************************
//������BOOL DoImplementCallBackSwitch( HANDLE hProcess )
//������
//	IN hProcess - ���̾��
//  
//����ֵ��
//	����ɹ�������TRUE�����򣬷���FALSE
//����������
//	����CALLSTACK �ṹ�� �л����µĽ��̿ռ�
//���ã�
// ********************************************************************


BOOL DoImplementCallBackSwitchTo( HANDLE hProcess )
{
	CALLSTACK * lpcs;
	BOOL bRetv = FALSE;

	lpcs = KHeap_Alloc( sizeof( CALLSTACK ) );
	if( lpcs )
	{
		bRetv = SwitchToProcessByHandle( hProcess, lpcs );
		if( bRetv )
		{
			lpcs->dwCallInfo |= CALL_ALLOC;
		}
		else
			KHeap_Free( lpcs, sizeof( CALLSTACK ) );
	}
	return bRetv;
}

// ********************************************************************
//������VOID DoImplementCallBackSwitchBack( VOID )
//������
//	��
//  
//����ֵ��
//	��
//����������
//	�л��ؽ��̿ռ�
//���ã�
// ********************************************************************


VOID DoImplementCallBackSwitchBack(VOID)
{
	DoLeaveAPI();
}

// ********************************************************************
//������PFNVOID MakeSysCall( LPSYSCALL lpCall )
//������
// IN lpCall-������ϵͳ������Ϣ�Ľṹָ��
//����ֵ��
// ����ɹ������ط����ܶ�Ӧ�Ĺ��ܵ�ַ(Ҳ����ΪNULL); ���򷵻�NULL
//����������
// �ú���ͨ������ϵͳ�ж�����á�
// ���û�����һ��������ʱ�������һ��ϵͳAPI�����жϣ������壩�������������Ϣ�����жϡ�
// ���жϽ�������Ϣ�����ݵĲ����б����ú��������ɸú���������ʲô������ã�
// �л���������̿ռ䣬ת����صĲ�������󣬷�����Ҫ���õķ����ܵ�ַ
//����: 
// ���жϻ��������
// ********************************************************************

#define DEBUG_MAKE_SYS_CALL 0
PFNVOID MakeSysCall( LPSYSCALL lpCallInfo )
{
	UINT dwCallInfo = lpCallInfo->dwCallInfo;
	LPDWORD lpdwCallSP = (LPDWORD)lpCallInfo->uiArgs;
	UINT uiAPIId = (dwCallInfo & APIID_MASK) >> APIID_SHIFT;
	UINT uiOptionId = (dwCallInfo & OPTION_MASK) >> OPTION_SHIFT;
	int  iArgs = (dwCallInfo & ARG_NUM_MASK) >> ARG_NUM_SHIFT;
	PFNVOID pfn;
	CALLSTACK * lpcs;
	
	BOOL bRetv;

    DEBUGMSG( DEBUG_MAKE_SYS_CALL, ( "MakeSysCall:dwCallInfo=0x%x,lpdwCallSP=0x%x,pfnRetAdress=0x%x,uiAPIId=%d,uiOptionId=%d.\r\n", dwCallInfo, lpCallInfo->uiArgs, lpCallInfo->pfnRetAdress, uiAPIId, uiOptionId ) );

	pfn = 0; 
	// ����һ������stack,���ڱ���ϵͳ����������
	lpcs = KHeap_Alloc( sizeof( CALLSTACK ) );
//	ASSERT( lpcs );

	if( lpcs && uiOptionId )
	{   //�ж��Ƿ��ǻ��ھ���ķ������
		if( IS_HANDLE_API( dwCallInfo ) )  // the first args in lpdwCallSP is a handle
		{   //�ǣ��õ����ú�����ַ������صĽ����л�
			if( ( *lpdwCallSP = (DWORD)HandleToPtr( (HANDLE)*lpdwCallSP, -1 ) ) )
			{
				bRetv = _EnterAPI( uiAPIId, uiOptionId, &pfn, lpcs );
//				{   // ת�����
					//if( ( *lpdwCallSP = (DWORD)HandleToPtr( (HANDLE)*lpdwCallSP, -1 ) ) == NULL )
					//					bRetv = FALSE;
					//				else
//					bRetv = TRUE;
//				}
//				else
//				{
//					bRetv = TRUE;
//				}
			}
			else
			{
				WARNMSG( DEBUG_MAKE_SYS_CALL, ( "error in MakeAPICall: invalid handle.\r\n" ) );
				bRetv = FALSE;
			}
		}
		else
		{   //�õ����ú�����ַ������صĽ����л�
			bRetv = _EnterAPI( uiAPIId, uiOptionId, &pfn, lpcs );
		}
		
		if( bRetv )
		{   // ת�������ߴ��ݵĲ���
			
			DWORD dwArg = apiInfo[uiAPIId].lpArgs[uiOptionId];
			//RETAILMSG( 1, ( "api:0x%x.\r\n", ((LPPROCESS)lpcs->lpvData)->dwVirtualAddressBase ) );
			
			if( uiAPIId > API_KERNEL )
			{
				for( ; dwArg; dwArg >>= 2 )
				{
					if( (dwArg & ARG_MASK) == ARG_PTR )
					{   //�����ָ�룬����ӳ����
						*lpdwCallSP = (DWORD)MapProcessPtr( (LPVOID)*lpdwCallSP, (LPPROCESS)lpcs->lpvData );
					}
					lpdwCallSP++;
					iArgs--;
					DEBUGMSG( !(iArgs >= 0), ( "error (iArgs < 0 !) in MakeAPICall: dwArg(0x%x),uiAPIId(%d),uiOptionId(%d),dwCallInfo(0x%x),ArgNum(%d).\r\n", apiInfo[uiAPIId].lpArgs[uiOptionId], uiAPIId, uiOptionId, dwCallInfo, (dwCallInfo & ARG_NUM_MASK) >> ARG_NUM_SHIFT )  );
				}
			}
			//��������ߵķ��ص�ַ
			lpcs->pfnRetAdress = lpCallInfo->pfnRetAdress;
		}
		else
		{
			OutErrorAPIInfo( uiAPIId, uiOptionId );
		}
		
		if( !bRetv )
		{   //���粻�ɹ�,���������
			KHeap_Free( lpcs, sizeof( CALLSTACK ) );
		}
		else
		{   // �ɹ������������Ϣ��CALL_ALLOC����lpcs�Ƕ�̬����ģ������Ӧ�õ���
			// KHeap_Freeȥ�����
			lpcs->dwCallInfo |= dwCallInfo | CALL_ALLOC;
			lpcs->dwStatus = lpCallInfo->uiCallMode;
			//����CPUģʽ
			lpCallInfo->uiCallMode = GetCPUMode( apiInfo[uiAPIId].lpProcess->dwFlags & 0xF );
		}
	}
	else
	{		
		if( lpcs )
		{
			KHeap_Free( lpcs, sizeof( CALLSTACK ) );
		}
		ERRORMSG( DEBUG_MAKE_SYS_CALL, ( "error in MakeSysCall :lpcs(0x%x), uiOptionId(0x%x).\r\n", lpcs, uiOptionId ) );
	}

	return pfn;
}

int __CheckAPIHeap( UINT uiAPIId, char * lpFile, int line )
{
	return 1;
}
