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
//声明：HANDLE DoRegisterAPI( UINT uiAPIId, PFNVOID lpfn, const DWORD * lpArgs, UINT uiOptions )
//参数：
// IN uiAPIId -服务ID
// IN lpfn -指向服务函数集的指针
// IN lpArgs -指向服务函数集的参数信息指针
// IN uiOptions -函数数
//返回值：假如成功，返回有效的句柄值，负责返回NULL
//功能描述：注册服务集
//引用: KL_RegisterAPI & KL_RegisterAPIEx
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
//声明：HANDLE WINAPI KL_RegisterAPI( UINT uiAPIId, PFNVOID lpfn, UINT uiOptions )
//参数：
// IN uiAPIId -服务ID
// IN lpfn -指向服务函数集的指针
// IN uiOptions -函数数
//返回值：假如成功，返回有效的句柄值，负责返回NULL
//功能描述：注册服务集
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
//声明：_EnterAPI( UINT uiAPIId, UINT uiOptionId, VOID * lpfn, CALLSTACK * lpcs )
//参数：
// IN uiAPIId -服务ID
// IN uiOptionId -函数功能ID
// OUT lpfn -用于接受函数调用地址
// OUT lpcs -CALLSTACK结构指针，用于保存调用上下文
//返回值：
//   假如成功，返回TRUE；否则，返回FALSE
//功能描述：该函数执行以下操作：
// 1.判断参数是否有效
// 2.切换到服务进程上下文
// 3.返回某API集对应的函数
//引用: KL_EnterAPIHandle & KL_EnterAPI
// ********************************************************************
static BOOL _EnterAPI( UINT uiAPIId, UINT uiOptionId, VOID * lpfn, CALLSTACK * lpcs )
{
	if( uiAPIId < MAX_API )
	{  //判断参数是否有效
		if( 
			uiOptionId && 
			uiOptionId < apiInfo[uiAPIId].uiMaxOption )
		{
			const PFNVOID * lppfn;
			
			lppfn = apiInfo[uiAPIId].lpfn;			

			if( lppfn ) //&& ( *( (PFNVOID*)lpfn ) = *( lppfn + uiOptionId ) ) )
			{  // 切换到服务进程上下文
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
//声明：BOOL WINAPI KL_EnterAPIHandle( UINT uiAPIId, UINT uiOptionId, VOID * lpfn, CALLSTACK * lpcs, HANDLE * lpHandle )
//参数：
// IN uiAPIId -服务ID
// IN uiOptionId -函数功能ID
// OUT lpfn -用于接受函数调用地址
// OUT lpcs -CALLSTACK结构指针，用于保存调用上下文
// OUT lpHandle-句柄指针
//返回值：
//   假如成功，返回TRUE；否则，返回FALSE
//功能描述：
// 调用_EnterAPI并将句柄转化为服务功能需要的数据
//引用: 系统调用
// ********************************************************************

BOOL WINAPI KL_EnterAPIHandle( UINT uiAPIId, UINT uiOptionId, VOID * lpfn, CALLSTACK * lpcsRet, HANDLE * lpHandle )
{
//	CALLSTACK * lpcall;
	
	// 分配一个调用stack,用于保存系统调用上下文
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
	
	// 分配一个调用stack,用于保存系统调用上下文
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

    //切换到调用者的进程空间
	//SwitchBackProcess();
    //ASSERT( (lpcs->dwCallInfo & CALL_ALLOC) );
	//释放MakeAPICall分配的CALLSTACK结构指针
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