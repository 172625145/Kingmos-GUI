/**************************************************************************
* Copyright (c)  微逻辑(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef __EAPISRV_H
#define __EAPISRV_H

#ifdef __cplusplus
extern "C" {
#endif      // __cplusplus

#define API_TYPE   

enum{
    API_APISRV,//     0
    API_KERNEL,//     1    

    API_SHELL = 8,  //	 8 
	API_DEV,		//   9
    API_DEV_FILE,	//   10
    API_FILESYS,	//   11
    API_FSDMGR,		//   12
    API_REGISTRY,	//   13
    API_TABLE,		//   14
    API_USUAL,		//   15	
    API_GDI,		//   16
    API_WND,		//   17
    API_MSG,		//   18
    API_GWE,		//   19

    API_AUDIO,		//	  20    
    API_GSM,		//	  21

    API_TCPIP,		//	  22
    API_NDIS,		//	  23
    API_RAS,		//	  24
	API_INTERNET,	//	  25
    API_WAP,		//	  26  
    API_MMS,		//	  27  

	API_KEYBD,		//    28
};

// F=flag; H=handle API; A=ARG_NUM_MASK; M=APIID; S=OPTION
//FFFF 0000 000H AAAA MMMM MMSS SSSS SS00

// 改变系统调用协议，必须同时改变callsrv.s  和
// eobjcall.h内的Sys_ImplementCallBack Sys_ImplementCallBack4
typedef struct _CALLTRAP
{
	DWORD apiInfo;
	DWORD arg0;
}CALLTRAP, FAR * LPCALLTRAP;

DWORD WINAPI CALL_SERVER( LPCALLTRAP, ... );

#define API_FLAG      0xF0000000
#define HANDLE_MASK   0x00100000
#define IS_HANDLE_API( id ) ( (id) & HANDLE_MASK )

#define APIID_MASK    0x0000FC00
#define APIID_SHIFT   10
#define OPTION_MASK   0x000003FC
#define OPTION_SHIFT  2
#define ARG_NUM_MASK  0x000F0000
#define ARG_NUM_SHIFT 16

#define MAX_ARGS      12

#define MAKE_OPTIONINFO( id, option ) ( ( (id) << APIID_SHIFT ) | ( (option) << OPTION_SHIFT ) )
#define GET_APIID( callinfo ) ( ( (callinfo)&APIID_MASK ) >> APIID_SHIFT )
#define GET_OPTIONID( callinfo ) ( ( (callinfo)&OPTION_MASK ) >> OPTION_SHIFT )
#define CALL_API( id, option, arg_num )  ( MAKE_OPTIONINFO( id, option ) | ( (arg_num) << ARG_NUM_SHIFT ) | API_FLAG )
#define CALL_API_HANDLE( id, option, arg_num ) ( ( CALL_API( (id), (option), (arg_num) ) | HANDLE_MASK ) )
#define CALL_HANDLE_API CALL_API_HANDLE

#define ARG_DWORD       0
#define ARG_PTR         1
#define ARG_INT64       2

#define ARG_MASK   0x03
#define ARG_BITS   2

#define ARG_SHIFT(arg, index)   ( ARG_ ## arg << ARG_BITS * index )

#define ARG0_MAKE()             0
#define ARG1_MAKE(a0)           ( ARG_SHIFT(a0,0) )
#define ARG2_MAKE(a0,a1)        ( ARG_SHIFT(a0,0)|ARG_SHIFT(a1,1) )
#define ARG3_MAKE(a0,a1,a2)     ( ARG_SHIFT(a0,0)|ARG_SHIFT(a1,1)|ARG_SHIFT(a2,2) )
#define ARG4_MAKE(a0,a1,a2,a3)  ( ARG_SHIFT(a0,0)|ARG_SHIFT(a1,1)|ARG_SHIFT(a2,2)|ARG_SHIFT(a3,3) )
#define ARG5_MAKE(a0,a1,a2,a3,a4) \
	( ARG_SHIFT(a0,0)|ARG_SHIFT(a1,1)|ARG_SHIFT(a2,2)|ARG_SHIFT(a3,3)|ARG_SHIFT(a4,4) )
#define ARG6_MAKE(a0,a1,a2,a3,a4,a5) \
	( ARG_SHIFT(a0,0)|ARG_SHIFT(a1,1)|ARG_SHIFT(a2,2)|ARG_SHIFT(a3,3)|ARG_SHIFT(a4,4)|ARG_SHIFT(a5,5) )
#define ARG7_MAKE(a0,a1,a2,a3,a4,a5,a6) \
	( ARG_SHIFT(a0,0)|ARG_SHIFT(a1,1)|ARG_SHIFT(a2,2)|ARG_SHIFT(a3,3)|ARG_SHIFT(a4,4)|ARG_SHIFT(a5,5)|ARG_SHIFT(a6,6) )
#define ARG8_MAKE(a0,a1,a2,a3,a4,a5,a6,a7) \
	( ARG_SHIFT(a0,0)|ARG_SHIFT(a1,1)|ARG_SHIFT(a2,2)|ARG_SHIFT(a3,3)|ARG_SHIFT(a4,4)|ARG_SHIFT(a5,5)|ARG_SHIFT(a6,6)|ARG_SHIFT(a7,7) )
#define ARG9_MAKE(a0,a1,a2,a3,a4,a5,a6,a7,a8) \
	( ARG_SHIFT(a0,0)|ARG_SHIFT(a1,1)|ARG_SHIFT(a2,2)|ARG_SHIFT(a3,3)|ARG_SHIFT(a4,4)|ARG_SHIFT(a5,5)|ARG_SHIFT(a6,6)|ARG_SHIFT(a7,7)|ARG_SHIFT(a8,8) )
#define ARG10_MAKE(a0,a1,a2,a3,a4,a5,a6,a7,a8,a9) \
	( ARG_SHIFT(a0,0)|ARG_SHIFT(a1,1)|ARG_SHIFT(a2,2)|ARG_SHIFT(a3,3)|ARG_SHIFT(a4,4)|ARG_SHIFT(a5,5)|ARG_SHIFT(a6,6)|ARG_SHIFT(a7,7)|ARG_SHIFT(a8,8)|ARG_SHIFT(a9,9) )
#define ARG11_MAKE(a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10) \
	( ARG_SHIFT(a0,0)|ARG_SHIFT(a1,1)|ARG_SHIFT(a2,2)|ARG_SHIFT(a3,3)|ARG_SHIFT(a4,4)|ARG_SHIFT(a5,5)|ARG_SHIFT(a6,6)|ARG_SHIFT(a7,7)|ARG_SHIFT(a8,8)|ARG_SHIFT(a9,9)|ARG_SHIFT(a10,10) )
#define ARG12_MAKE(a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11) \
	( ARG_SHIFT(a0,0)|ARG_SHIFT(a1,1)|ARG_SHIFT(a2,2)|ARG_SHIFT(a3,3)|ARG_SHIFT(a4,4)|ARG_SHIFT(a5,5)|ARG_SHIFT(a6,6)|ARG_SHIFT(a7,7)|ARG_SHIFT(a8,8)|ARG_SHIFT(a9,9)|ARG_SHIFT(a10,10)|ARG_SHIFT(a11,11) )

#define ARG12_MAKE(a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11) \
	( ARG_SHIFT(a0,0)|ARG_SHIFT(a1,1)|ARG_SHIFT(a2,2)|ARG_SHIFT(a3,3)|ARG_SHIFT(a4,4)|ARG_SHIFT(a5,5)|ARG_SHIFT(a6,6)|ARG_SHIFT(a7,7)|ARG_SHIFT(a8,8)|ARG_SHIFT(a9,9)|ARG_SHIFT(a10,10)|ARG_SHIFT(a11,11) )

typedef HANDLE ( WINAPI * PAPI_REGISTER )( UINT uiServerId, const PFNVOID * lpfn, UINT uiOptions );
typedef HANDLE ( WINAPI * PAPI_REGISTEREX )( UINT uiServerId, const PFNVOID * lpfn, const DWORD * lpdwArgs, UINT uiOptions );
typedef BOOL ( WINAPI * PAPI_UNREGISTER )( HANDLE );
typedef BOOL ( WINAPI * PAPI_ENTER )( UINT uiAPIId, UINT uiOptionId, LPVOID lpfn, LPCALLSTACK lpdwPrevProcess );
typedef BOOL ( WINAPI * PAPI_ENTERHANDLE )( UINT uiAPIId, UINT uiOptionId, VOID * lpfn, CALLSTACK * lpcs, HANDLE * lpHandle );
typedef void ( WINAPI * PAPI_LEAVE )( void );
typedef BOOL ( WINAPI * PAPI_ISREADY )( UINT uiAPI );
typedef BOOL ( WINAPI * PAPI_SETREADY )( UINT uiAPI );
typedef DWORD ( WINAPI * PSERVER_HANDLER )( HANDLE handle, DWORD dwEventCode , DWORD dwParam, LPVOID lpParam );

// 定义 SERVER　功能编码
// 注意 function 0-2047 由Kingmos系统使用
//      function 2048-4095 由客户使用
// bit格式： DDDD DDDD DDDD DDDD AAFF FFFF FFFF FFRR
#define SRV_CTL_CODE( serverType, function, access ) \
	( ((serverType) << 16) | ((access) << 14) | ((function) << 2) )


#define SCI_NOT_HANDLE_THREAD_EXIT     0x00000001
#define SCI_NOT_HANDLE_PROCESS_EXIT    0x00000002
typedef struct _SERVER_CTRL_INFO
{
	DWORD dwSize;   // = sizeof(SERVER_CTRL_INFO)
	DWORD dwInfo;
    PSERVER_HANDLER pServerHandler;
}SERVER_CTRL_INFO, * PSERVER_CTRL_INFO, FAR * LPSERVER_CTRL_INFO;

// 定义 access 值

#define SRV_ANY_ACCESS                 0
#define SRV_READ_ACCESS               0x1
#define SRV_WRITE_ACCESS              0x2

// 定义服务类型
// 注意 0-32767 由Kingmos系统使用
// 32768-65535  由客户使用
//
// kernel 
#define SRV_NULL                          0x00000000
#define SRV_UNKNOWN                       0x00000001
#define SRV_BROADCAST                     0x00000002
#define SRV_SYSTEM                        0x00000003

#define SRV_DEVICE                        API_DEV          // 8
#define SRV_GWME                          API_GWE          // 13
#define SRV_FILE_SYSTEM                   API_FILESYS      // 14
#define SRV_TABLE                         API_TABLE        // 16
#define SRV_REGISTRY                      API_REGISTRY     // 23 
#define SRV_SHELL                         API_SHELL        // 24

// dwParam = ThreadID, lParam = NULL
#define SCC_BROADCAST_THREAD_EXIT    SRV_CTL_CODE( SRV_BROADCAST, 1, SRV_ANY_ACCESS )
// dwParam = Process Handle, lParam = NULL
#define SCC_BROADCAST_PROCESS_EXIT    SRV_CTL_CODE( SRV_BROADCAST, 2, SRV_ANY_ACCESS )

//广播电源请求
#define SCC_BROADCAST_POWER  SRV_CTL_CODE( SRV_BROADCAST, 3, SRV_ANY_ACCESS )
//SCC_BROADCAST_POWER 的wParam 参数,   lParam  = 0
#define SBP_APMBATTERYLOW          1		//低电源
#define SBP_APMQUERYSUSPEND		   10		//请求挂起许可
#define SBP_APMQUERYSUSPENDFAILED  11       //请求挂起许可失败
#define SBP_APMSUSPEND             12       //挂起操作...
#define SBP_APMRESUMESUSPEND       13       //挂起后的恢复...

//#define SCC_BROADCAST_POWERON   SRV_CTL_CODE( SRV_BROADCAST, 4, SRV_ANY_ACCESS )
#define SCC_BROADCAST_SHUTDOWN    SRV_CTL_CODE( SRV_BROADCAST, 5, SRV_ANY_ACCESS )
#define SCC_BROADCAST_REBOOT      SRV_CTL_CODE( SRV_BROADCAST, 6, SRV_ANY_ACCESS )

//#define IOCTL_SYS_THREAD_EXIT   SRV_CTL_CODE( SRV_SYSTEM, 1, SRV_ANY_ACCESS )
//#define IOCTL_SYS_PROCESS_EXIT  SRV_CTL_CODE( SRV_SYSTEM, 2, SRV_ANY_ACCESS )
//#define IOCTL_SYS_POWEROFF    SRV_CTL_CODE( SRV_SYSTEM, 3, SRV_ANY_ACCESS )
//#define IOCTL_SYS_POWERON     SRV_CTL_CODE( SRV_SYSTEM, 4, SRV_ANY_ACCESS )
//#define IOCTL_SYS_SHUTDOWN    SRV_CTL_CODE( SRV_SYSTEM, 5, SRV_ANY_ACCESS )
//#define IOCTL_SYS_REBOOT      SRV_CTL_CODE( SRV_SYSTEM, 6, SRV_ANY_ACCESS )

//#ifdef INLINE_PROGRAM
#ifdef CALL_TRAP

// 用陷阱/异常的方法
enum{
	API_REGISTER = 1,
	API_UNREGISTER,
	API_ENTER,
	API_ENTERHANDLE,
	API_LEAVE,
	API_REGISTEREX,
	API_ISREADY,
	API_SETREADY
};

HANDLE WINAPI API_Register( UINT uiServerId, const PFNVOID * lpfn, UINT uiOptions );
HANDLE WINAPI API_RegisterEx( UINT uiServerId, const PFNVOID * lpfn, const DWORD * lpdwArgs, UINT uiOptions );
BOOL WINAPI API_Unregister( HANDLE );
//BOOL WINAPI API_Enter( UINT uiAPIId, UINT uiOptionId, LPVOID lpfn, LPCALLSTACK lpdwPrevProcess );
//BOOL WINAPI API_EnterHandle( UINT uiAPIId, UINT uiOptionId, VOID * lpfn, CALLSTACK * lpcs, HANDLE * lpHandle );
//void WINAPI API_Leave( void );
BOOL WINAPI API_IsReady( UINT uiAPI );
BOOL WINAPI API_SetReady( UINT uiAPI );

#else

// 用静态连接的方法
extern const PAPI_REGISTER API_Register;
extern const PAPI_REGISTEREX  API_RegisterEx;
extern const PAPI_UNREGISTER API_Unregister;
extern const PAPI_ENTER API_Enter;
extern const PAPI_ENTERHANDLE API_EnterHandle;
extern const PAPI_LEAVE API_Leave;
extern const PAPI_ISREADY API_IsReady;
extern const PAPI_SETREADY API_SetReady;

#endif

//LRESULT WINAPI API_CallOption( UINT uiServerId, UINT uiOptionId, UINT 

#ifdef __cplusplus
}
#endif  // __cplusplus


#endif  //  __EAPISRV_H
