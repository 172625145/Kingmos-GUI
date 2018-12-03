#include <ewindows.h>
#include <eapisrv.h>
#include <eobjcall.h>


typedef struct _APIINFO{
	const PFNVOID * lpfn;
	const DWORD * lpArgs;
	UINT    uiMaxOption;
	//LPPROCESS lpProcess;
	HANDLE hServer;
	LPSERVER_CTRL_INFO lpServerCtrlInfo;
	DWORD dwServerInfo;
}APIINFO, FAR * LPAPIINFO;

//HANDLE WINAPI KL_RegisterAPI( UINT uiAPIId, const PFNVOID * lpfn, UINT uiOptions )
//{
//}
#define MAX_API   32
static APIINFO apiInfo[MAX_API];// = { {lpAPIAPI,dwWndArgs,sizeof(lpAPIAPI)/sizeof(PFNVOID),&InitKernelProcess}, };
static BYTE   bAPISetState[MAX_API] = { 1, };

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
		//lpfn = (const PFNVOID *)MapProcessPtr( lpfn, GetAPICallerProcessPtr() );
        //lpArgs = (const DWORD *)MapProcessPtr( lpArgs, GetAPICallerProcessPtr() );
		if( InterlockedCompareExchange( (LPLONG)&apiInfo[uiAPIId].lpfn, (LONG)lpfn, 0 ) == 0 )
		{
			apiInfo[uiAPIId].lpArgs = lpArgs;
			//apiInfo[uiAPIId].lpProcess = GetAPICallerTaskPtr();//lpCurThread->lpCurProcess;
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

HANDLE WINAPI KL_RegisterAPIEx( UINT uiAPIId, const PFNVOID * lpfn, const DWORD * lpArgs, UINT uiOptions )
{
	return DoRegisterAPI( uiAPIId, lpfn, lpArgs, uiOptions );
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

BOOL WINAPI KL_UnregisterAPI( HANDLE hAPI )
{
	return TRUE;
}

BOOL WINAPI KL_IsAPIReady( UINT uiAPI )
{
	if( uiAPI < MAX_API )
	    return bAPISetState[uiAPI];
	return FALSE;
}

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
				lpcs->dwCallInfo = 0;
				*( (PFNVOID*)lpfn ) = *( lppfn + uiOptionId );
				return TRUE;
			}
		}
	}
	SetLastError( ERROR_INVALID_FUNCTION );			
	return 0;
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
//	CALLSTACK * lpcall;
	
	// ����һ������stack,���ڱ���ϵͳ����������
	if( lpcsRet )
	{
		if( _EnterAPI( uiAPIId, uiOptionId, lpfn, lpcsRet ) )
		{			
			lpcsRet->dwCallInfo |= MAKE_OPTIONINFO( uiAPIId, uiOptionId );
			//*lpcsRet = *lpcall;
			//lpcsRet->dwCallInfo |= CALL_ALLOC;
			
			//*lpHandle = HandleToPtr( *lpHandle, -1 );
			return TRUE;
		}
		else
		{
			//KHeap_Free( lpcall, sizeof( CALLSTACK ) );
			//OutErrorAPIInfo( uiAPIId, uiOptionId );
		}
	}
	return FALSE;
}

BOOL WINAPI KL_EnterAPI( UINT uiAPIId, UINT uiOptionId, VOID * lpfn, CALLSTACK * lpcs )
{
	//CALLSTACK * lpcall;
	
	// ����һ������stack,���ڱ���ϵͳ����������
	//lpcall = KHeap_Alloc( sizeof( CALLSTACK ) );
	if( lpcs )
	{		
		if( _EnterAPI( uiAPIId, uiOptionId, lpfn, lpcs ) )
		{
			lpcs->dwCallInfo |= MAKE_OPTIONINFO( uiAPIId, uiOptionId );
			//*lpcsRet = *lpcall;
			//lpcall->dwCallInfo |= CALL_ALLOC;			
			return TRUE;
		}
		else
		{
			//KHeap_Free( lpcall, sizeof( CALLSTACK ) );
			//OutErrorAPIInfo( uiAPIId, uiOptionId );
		}
	}
	return FALSE;

}

void WINAPI KL_LeaveAPI( void )
{
	//CALLSTACK * lpcs = lpCurThread->lpCallStack;

    //�л��������ߵĽ��̿ռ�
	//SwitchBackProcess();
    //ASSERT( (lpcs->dwCallInfo & CALL_ALLOC) );
	//�ͷ�MakeAPICall�����CALLSTACK�ṹָ��
	//KHeap_Free( lpcs, sizeof( CALLSTACK ) );

	//if( iISRActiveCount )
    //{   
	//	INTR_OFF();
      //  ISR_Handler( ISR_ALL_INTRS );
		//INTR_ON();
    //}    
}
/*
LRESULT KL_ImplementCallBack4( LPCALLBACKDATA lpcd, ... )
{
	va_list         vl;
	UINT arg1, arg2, arg3;
	
	va_start(vl, lpcd);

	arg1 = va_arg(vl, int);
	arg2 = va_arg(vl, int);
	arg3 = va_arg(vl, int);

	return lpcd->lpfn( lpcd->dwArg0, arg1, arg2, arg3 );	
}
*/