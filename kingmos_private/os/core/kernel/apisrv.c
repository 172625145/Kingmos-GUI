/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：本地过程调用(LPC)管理
版本号：2.0.0
开发时期：2000
作者：李林
修改记录：
	2005-01-18, 增加 KC_EnterAPI and KC_EnterAPIHandle
	2004-12-20, 增加 ackAccessKey 功能
    2003-10-07, LN, 直接Call _EnterAPI
    2003-07-12, LN, 将KL_EnterAPI & KL_EnterAPIHandle 改为Call _EnterAPI
    2003-07-01, LN ,KL_EnterAPIHandle & KL_EnterAPI 
	            不能访问 *apiInfo[uiAPIId].lpfn
******************************************************/


#include <eframe.h>
#include <eapisrv.h>
#include <eucore.h>
#include <epcore.h>
#include <epalloc.h>
#include <eobjcall.h>
#include <coresrv.h>
#include <sysintr.h>

// 定义API服务的API
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

// 定义系统最大的api集
APIINFO apiInfo[MAX_API] = { {lpAPIAPI,dwWndArgs,sizeof(lpAPIAPI)/sizeof(PFNVOID),&InitKernelProcess}, };
BYTE   bAPISetState[MAX_API] = { 1, };

// ********************************************************************
//声明：BOOL _InitAPIMgr( void )
//参数：无
//返回值：假如成功，返回TURE；否则，返回FALSE
//功能描述：API管理模块初始化
//引用: 由InitialKingmosSystem调用(system.c)
// ********************************************************************
BOOL _InitAPIMgr( void )
{
	memset( bAPISetState, 0, sizeof( bAPISetState ) );
	return TRUE;
}

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

// ********************************************************************
//声明：HANDLE WINAPI KL_RegisterAPIEx( UINT uiAPIId, PFNVOID lpfn, const DWORD * lpArgs, UINT uiOptions )
//参数：
// IN uiAPIId -服务ID
// IN lpfn -指向服务函数集的指针
// IN lpArgs -指向服务函数集的参数信息指针
// IN uiOptions -函数数
//返回值：假如成功，返回有效的句柄值，负责返回NULL
//功能描述：注册服务集
// ********************************************************************
HANDLE WINAPI KL_RegisterAPIEx( UINT uiAPIId, const PFNVOID * lpfn, const DWORD * lpArgs, UINT uiOptions )
{
	return DoRegisterAPI( uiAPIId, lpfn, lpArgs, uiOptions );
}

// ********************************************************************
//声明：BOOL WINAPI KL_IsAPIReady( UINT uiAPI )
//参数：
// IN uiAPI -服务ID
//返回值：
// 假如成功，返回TRUE；否则，返回FALSE
//功能描述：判断某API服务集是否准备好
//引用: 系统调用
// ********************************************************************
BOOL WINAPI KL_IsAPIReady( UINT uiAPI )
{
	if( uiAPI < MAX_API )
	    return bAPISetState[uiAPI];
	return FALSE;
}

// ********************************************************************
//声明：BOOL WINAPI KL_SetAPIReady( UINT uiAPI )
//参数：
// IN uiAPI -服务ID
//返回值：
// 假如成功，返回TRUE；否则，返回FALSE
//功能描述：设定某API服务集已准备好，用户可以使用
//引用: 系统调用
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
//声明：BOOL WINAPI KL_UnregisterAPI( HANDLE hAPI )
//参数：
// IN hAPI -为调用KL_RegisterAPIEx返回的句柄
//返回值：
// 假如成功，返回TRUE；否则，返回FALSE
//功能描述：注销一个API服务集
//引用: 系统调用
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
				ACCESS_KEY aky = lpCurThread->akyAccessKey;
				LPPROCESS lpProcess = apiInfo[uiAPIId].lpProcess;

				//增加 lpCurThread 对 Server Process 空间的存取权限
				AccessKey_Add( &lpCurThread->akyAccessKey, lpProcess->akyAccessKey );
				*( (PFNVOID*)lpfn ) = *( lppfn + uiOptionId );
				AccessKey_Set( &lpCurThread->akyAccessKey, aky );  //恢复
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
	CALLSTACK * lpcall;
	
	// 分配一个调用stack,用于保存系统调用上下文
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
//声明：BOOL WINAPI KC_EnterAPIHandle( UINT uiAPIId, UINT uiOptionId, VOID * lpfn, CALLSTACK * lpcs, HANDLE * lpHandle )
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
BOOL WINAPI KC_EnterAPIHandle( UINT uiAPIId, UINT uiOptionId, VOID * lpfn, CALLSTACK * lpcsRet, HANDLE * lpHandle )
{
	CALLSTACK * lpcall;
	
	// 分配一个调用stack,用于保存系统调用上下文
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
//声明：BOOL WINAPI KL_EnterAPI( UINT uiAPIId, UINT uiOptionId, VOID * lpfn, CALLSTACK * lpcs )
//参数：
// IN uiAPIId -服务ID
// IN uiOptionId -函数功能ID
// OUT lpfn -用于接受函数调用地址
// OUT lpcs -CALLSTACK结构指针，用于保存调用上下文
//返回值：
//   假如成功，返回TRUE；否则，返回FALSE
//功能描述：
// 调用_EnterAPI
//引用:系统调用 
// ********************************************************************
BOOL WINAPI KL_EnterAPI( UINT uiAPIId, UINT uiOptionId, VOID * lpfn, CALLSTACK * lpcsRet )
{
	CALLSTACK * lpcall;
	
	// 分配一个调用stack,用于保存系统调用上下文
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
//声明：BOOL WINAPI KC_EnterAPI( UINT uiAPIId, UINT uiOptionId, VOID * lpfn, CALLSTACK * lpcs )
//参数：
// IN uiAPIId -服务ID
// IN uiOptionId -函数功能ID
// OUT lpfn -用于接受函数调用地址
// OUT lpcs -CALLSTACK结构指针，用于保存调用上下文
//返回值：
//   假如成功，返回TRUE；否则，返回FALSE
//功能描述：
// 调用_EnterAPI
//引用:系统调用 
// ********************************************************************
BOOL WINAPI KC_EnterAPI( UINT uiAPIId, UINT uiOptionId, VOID * lpfn, CALLSTACK * lpcsRet )
{
	CALLSTACK * lpcall;
	
	// 分配一个调用stack,用于保存系统调用上下文
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
//声明：DWORD FASTCALL EnumServer( LPSERVER_ENUM_PROC lpEnumFunc, LPVOID lpParam )
//参数：
// IN lpEnumFunc-需要回调的枚举功能
// IN lpParam-传递给lpEnumFunc功能的参数
//返回值：
// 成功返回0
//功能描述：枚举所有已注册的服务，假如lpEnumFunc返回FALSE,则终止继续枚举
//引用: 
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
//声明：DWORD FASTCALL EnumServerHandler( DWORD dwIoCtl, DWORD dwParam, LPVOID lpParam )
//参数：
//	IN dwIoCtl - 需要回调的枚举功能
//	IN dwParam - 传递ServerHandler的dwParam参数
//	IN lpParam - 传递ServerHandler的lpParam参数
//返回值：
//	成功返回0
//功能描述：
//	枚举所有已注册的服务的ServerHandler
//引用: 
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
			{	// 不处理线程退出 或 进程退出事件
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
//声明：PFNVOID MakeAPICall( DWORD dwCallInfo, LPDWORD lpdwCallSP, PFNVOID pfnRetAdress, UINT uiRetMode )
//参数：
// IN dwCallInfo-服务调用信息
// IN lpdwCallSP-调用该服务功能的参数列表，lpdwCallSP[0] = 参数0,lpdwCallSP[1]=参数1,依次类推
// IN pfnRetAdress-调用该服务功能的用户的返回地址
// IN uiRetMode-调用该服务功能的用户的返回模式(指超级模式，系统模式，用户模式,..)
//返回值：
// 假如成功，返回服务功能对应的功能地址(也可以为NULL); 否则返回NULL
//功能描述：
// 该函数通常是在系统中断里调用。
// 当用户调用一个服务功能时，会产生一个系统API调用中断（或陷阱）并将服务调用信息传给中断。
// 该中断将调用信息及传递的参数列表传给该函数，并由该函数决定是什么服务调用，
// 切换到服务进程空间，转化相关的参数。最后，返回需要调用的服务功能地址
//引用: 
// 被中断或陷阱调用
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
	// 分配一个调用stack,用于保存系统调用上下文
	lpcs = KHeap_Alloc( sizeof( CALLSTACK ) );
	ASSERT( lpcs );

	if( lpcs && uiOptionId )
	{   //判断是否是基于句柄的服务调用
		if( IS_HANDLE_API( dwCallInfo ) )  // the first args in lpdwCallSP is a handle
		{   //是，得到调用函数地址并做相关的进程切换
			if( _EnterAPI( uiAPIId, uiOptionId, &pfn, lpcs ) )
			{   // 转换句柄
				*lpdwCallSP = (DWORD)HandleToPtr( (HANDLE)*lpdwCallSP, -1 );
				bRetv = TRUE;
			}
			else
				bRetv = FALSE;						
		}
		else
		{   //得到调用函数地址并做相关的进程切换
			bRetv = _EnterAPI( uiAPIId, uiOptionId, &pfn, lpcs );
		}
		
		if( bRetv )
		{   // 转换调用者传递的参数
			DWORD dwArg = apiInfo[uiAPIId].lpArgs[uiOptionId];
			//RETAILMSG( 1, ( "api:0x%x.\r\n", ((LPPROCESS)lpcs->lpvData)->dwVirtualAddressBase ) );
			
			if( uiAPIId > API_KERNEL )
			{
				for( ; dwArg; dwArg >>= 2 )
				{
					if( (dwArg & ARG_MASK) == ARG_PTR )
					{   //如果是指针，重新映射它
						*lpdwCallSP = (DWORD)MapProcessPtr( *lpdwCallSP, (LPPROCESS)lpcs->lpvData );
					}
					lpdwCallSP++;
					iArgs--;
					DEBUGMSG( !(iArgs >= 0), ( "error (iArgs < 0 !) in MakeAPICall: uiAPIId(%d),uiOptionId(%d),dwCallInfo(0x%x),ArgNum(%d)", uiAPIId, uiOptionId, dwCallInfo, (dwCallInfo & ARG_NUM_MASK) >> ARG_NUM_SHIFT )  );
				}
			}
			//保存调用者的返回地址
			lpcs->pfnRetAdress = pfnRetAdress;
		}
		
		if( !bRetv )
		{   //假如不成功,做清除工作
			KHeap_Free( lpcs, sizeof( CALLSTACK ) );
		}
		else
		{   // 成功，保存相关信息，CALL_ALLOC代表lpcs是动态分配的，在随后应该调用
			// KHeap_Free去清除它
			lpcs->dwCallInfo |= dwCallInfo | CALL_ALLOC;
			lpcs->dwStatus = uiRetMode;
		}
	}

	return pfn;
}
*/

// ********************************************************************
//声明：PFNVOID APICallReturn( DWORD * lpdwState )
//参数：
//	OUT lpdwState - 输出状态
//返回值：
// 返回 MakeAPICall保存的调用者的返回地址（pfnRetAdress），lpdwState返回CPU状态
//功能描述：调用完服务功能后回退到调用者的上下文
//引用: 
// 被中断或陷阱调用
// ********************************************************************
PFNVOID APICallReturn( DWORD * lpdwState )
{
	CALLSTACK * lpcs = lpCurThread->lpCallStack;
    PFNVOID pfn = lpcs->pfnRetAdress;

//    ASSERT( lpcs );
	*lpdwState = lpcs->dwStatus;

//	RETAILMSG( 1, ( "APICallReturn(0x%x),lpcr(0x%x).\r\n", pfn, lpcr ) );
    //切换到调用者的进程空间
    SwitchBackProcess();

//	lpcr->lpRetSP = 0;
//	lpcr->uiRetMode = lpcs->uiRetMode;
	ASSERT( lpcs->dwCallInfo & CALL_ALLOC );
	//释放MakeAPICall分配的CALLSTACK结构指针
	KHeap_Free( lpcs, sizeof( CALLSTACK ) );
	//返回到调用者的地址
	return pfn;
}

static VOID DoLeaveAPI( VOID )
{
	CALLSTACK * lpcs = lpCurThread->lpCallStack;

    //切换到调用者的进程空间
	SwitchBackProcess();
    ASSERT( (lpcs->dwCallInfo & CALL_ALLOC) );
	//释放MakeAPICall分配的CALLSTACK结构指针
	KHeap_Free( lpcs, sizeof( CALLSTACK ) );

	if( iISRActiveCount )
    {   
		INTR_OFF();
        ISR_Handler( ISR_ALL_INTRS );
		INTR_ON();
    }    
}

// ********************************************************************
//声明：void WINAPI KL_LeaveAPI( void )
//参数：无
//返回值：无
//功能描述：调用完服务功能后回退到调用者的上下文
//引用: 系统调用
// ********************************************************************
void WINAPI KL_LeaveAPI( void )
{
	DoLeaveAPI();
}

// ********************************************************************
//声明：PFNVOID DoImplementCallBack4( LPICB * lpicb )
//参数：
//	IN/OUT lpicb - 输出调用模式
//  
//返回值：
//  返回 调用地址
//功能描述：
//	改变当前的 CALLSTACK 结构数据
//	切换到进程，调用进程功能
//引用: 
// 
// ********************************************************************

PFNVOID DoImplementCallBack( LPSYSCALL lpCallInfo )
{
	HANDLE hProcess = ((LPCALLBACKDATA)lpCallInfo->uiArgs[0])->hProcess;
	LPPROCESS lpProcess  = HandleToPtr( hProcess, OBJ_PROCESS );
	PFNVOID pfn = (PFNVOID)((LPCALLBACKDATA)lpCallInfo->uiArgs[0])->lpfn;
	
	if( lpProcess && pfn )
	{	//得到当前的CS结构指针
		UINT uiSave;
		BOOL bSwitchContext;
		CALLSTACK * lpcs = lpCurThread->lpCallStack;		
		
		LockIRQSave( &uiSave );

		// 如果新的进程与线程的当前进程不同，则需要切换进程空间
		if( lpProcess != lpCurThread->lpCurProcess )
		{
			bSwitchContext = TRUE;			
		}
		else
			bSwitchContext = FALSE;
		// 设置CALLSTACK结构
		lpcs->dwCallInfo &= ~CALL_KERNEL;
		lpcs->lpvData = lpCurThread->lpCurProcess;
		// 保存调用链
		//lpcs->lpNext = lpCurThread->lpCallStack;		
		//lpCurThread->lpCallStack = lpcs;
		// 将lpProcess设为当前线程的当前进程
	//	lpCurThread->lpCurProcess = lpProcess;  // 新的进程空间
		
		if( bSwitchContext )
		{
//#ifdef VIRTUAL_MEM
			//装入当前进程的MMU有关数据，无效CACHE。。。
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
//声明：BOOL DoImplementCallBackSwitch( HANDLE hProcess )
//参数：
//	IN hProcess - 进程句柄
//  
//返回值：
//	假如成功，返回TRUE；否则，返回FALSE
//功能描述：
//	分配CALLSTACK 结构并 切换到新的进程空间
//引用：
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
//声明：VOID DoImplementCallBackSwitchBack( VOID )
//参数：
//	无
//  
//返回值：
//	无
//功能描述：
//	切换回进程空间
//引用：
// ********************************************************************


VOID DoImplementCallBackSwitchBack(VOID)
{
	DoLeaveAPI();
}

// ********************************************************************
//声明：PFNVOID MakeSysCall( LPSYSCALL lpCall )
//参数：
// IN lpCall-包含有系统调用信息的结构指针
//返回值：
// 假如成功，返回服务功能对应的功能地址(也可以为NULL); 否则返回NULL
//功能描述：
// 该函数通常是在系统中断里调用。
// 当用户调用一个服务功能时，会产生一个系统API调用中断（或陷阱）并将服务调用信息传给中断。
// 该中断将调用信息及传递的参数列表传给该函数，并由该函数决定是什么服务调用，
// 切换到服务进程空间，转化相关的参数。最后，返回需要调用的服务功能地址
//引用: 
// 被中断或陷阱调用
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
	// 分配一个调用stack,用于保存系统调用上下文
	lpcs = KHeap_Alloc( sizeof( CALLSTACK ) );
//	ASSERT( lpcs );

	if( lpcs && uiOptionId )
	{   //判断是否是基于句柄的服务调用
		if( IS_HANDLE_API( dwCallInfo ) )  // the first args in lpdwCallSP is a handle
		{   //是，得到调用函数地址并做相关的进程切换
			if( ( *lpdwCallSP = (DWORD)HandleToPtr( (HANDLE)*lpdwCallSP, -1 ) ) )
			{
				bRetv = _EnterAPI( uiAPIId, uiOptionId, &pfn, lpcs );
//				{   // 转换句柄
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
		{   //得到调用函数地址并做相关的进程切换
			bRetv = _EnterAPI( uiAPIId, uiOptionId, &pfn, lpcs );
		}
		
		if( bRetv )
		{   // 转换调用者传递的参数
			
			DWORD dwArg = apiInfo[uiAPIId].lpArgs[uiOptionId];
			//RETAILMSG( 1, ( "api:0x%x.\r\n", ((LPPROCESS)lpcs->lpvData)->dwVirtualAddressBase ) );
			
			if( uiAPIId > API_KERNEL )
			{
				for( ; dwArg; dwArg >>= 2 )
				{
					if( (dwArg & ARG_MASK) == ARG_PTR )
					{   //如果是指针，重新映射它
						*lpdwCallSP = (DWORD)MapProcessPtr( (LPVOID)*lpdwCallSP, (LPPROCESS)lpcs->lpvData );
					}
					lpdwCallSP++;
					iArgs--;
					DEBUGMSG( !(iArgs >= 0), ( "error (iArgs < 0 !) in MakeAPICall: dwArg(0x%x),uiAPIId(%d),uiOptionId(%d),dwCallInfo(0x%x),ArgNum(%d).\r\n", apiInfo[uiAPIId].lpArgs[uiOptionId], uiAPIId, uiOptionId, dwCallInfo, (dwCallInfo & ARG_NUM_MASK) >> ARG_NUM_SHIFT )  );
				}
			}
			//保存调用者的返回地址
			lpcs->pfnRetAdress = lpCallInfo->pfnRetAdress;
		}
		else
		{
			OutErrorAPIInfo( uiAPIId, uiOptionId );
		}
		
		if( !bRetv )
		{   //假如不成功,做清除工作
			KHeap_Free( lpcs, sizeof( CALLSTACK ) );
		}
		else
		{   // 成功，保存相关信息，CALL_ALLOC代表lpcs是动态分配的，在随后应该调用
			// KHeap_Free去清除它
			lpcs->dwCallInfo |= dwCallInfo | CALL_ALLOC;
			lpcs->dwStatus = lpCallInfo->uiCallMode;
			//返回CPU模式
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
