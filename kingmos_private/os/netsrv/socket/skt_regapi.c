/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：Socket API 注册
版本号：  1.0.0
开发时期：2004-05-09
作者：    肖远钢
修改记录：
******************************************************/
//root include
#include <ewindows.h>
#include <listunit.h>
#include <eapisrv.h>
#include <apiexe.h>
//"inc_app"
#include <socket.h>
#include <iphlpapi.h>
//"inc_local"
#include "skt_call.h"
#include "handle_check.h"
#include "proto_ip_tcp.h"
#include "iphlpapi_call.h"

/***************  全局区 定义， 声明 *****************/

extern	DWORD	WINAPI	SKAPI_CallBack( HANDLE handle, DWORD dwEventCode, DWORD dwParam, LPVOID lpParam );
static	const	SERVER_CTRL_INFO	sci = { sizeof( SERVER_CTRL_INFO ), SCI_NOT_HANDLE_THREAD_EXIT, SKAPI_CallBack };
static	const	PFNVOID	g_lpTcpipAPI[] =
{
	  (PFNVOID)&sci

	, (PFNVOID)SKAPI_WSAStartup
	, (PFNVOID)SKAPI_WSACleanup
	, (PFNVOID)SKAPI_WSASetLastError
	, (PFNVOID)SKAPI_WSAGetLastError

	, (PFNVOID)SKAPI_Socket				//----5
	, (PFNVOID)SKAPI_Bind
	, (PFNVOID)SKAPI_Connect
	, (PFNVOID)SKAPI_Listen 
	, (PFNVOID)SKAPI_Accept
	, (PFNVOID)SKAPI_IoctlSocket		//----10
	, (PFNVOID)SKAPI_Send
	, (PFNVOID)SKAPI_SendTo
	, (PFNVOID)SKAPI_Recv
	, (PFNVOID)SKAPI_RecvFrom
	, (PFNVOID)SKAPI_Select				//----15

	, (PFNVOID)SKAPI_GetSockOpt	
	, (PFNVOID)SKAPI_SetSockOpt
	, (PFNVOID)SKAPI_GetPeerName	
	, (PFNVOID)SKAPI_GetSockName

	, (PFNVOID)SKAPI_ShutDown			//----20
	, (PFNVOID)SKAPI_CloseSocket 

	, (PFNVOID)SKAPI_GetHostName
	, (PFNVOID)SKAPI_GetHostByName
	, (PFNVOID)SKAPI_GetHostByAddr
	, (PFNVOID)SKAPI_GetHostByNameEx
	, (PFNVOID)SKAPI_GetHostByAddrEx	//----25


	// IP层提供的 函数
	, (PFNVOID)IPHAPI_GetNetworkParams
	, (PFNVOID)IPHAPI_GetDnsSrvList

	, (PFNVOID)IPHAPI_GetNumberOfInterfaces
	, (PFNVOID)IPHAPI_GetAdaptersInfo
	, (PFNVOID)IPHAPI_GetInterfaceInfo
	, (PFNVOID)IPHAPI_GetIfEntry
	, (PFNVOID)IPHAPI_GetIfTable
	, (PFNVOID)IPHAPI_SetIfEntry
	, (PFNVOID)IPHAPI_GetIpAddrTable
	, (PFNVOID)IPHAPI_GetBestInterface

	, (PFNVOID)IPHAPI_SetIpTTL
	, (PFNVOID)IPHAPI_GetIpStatistics

	, (PFNVOID)IPHAPI_AddIPAddress
	, (PFNVOID)IPHAPI_IpReleaseAddress
	, (PFNVOID)IPHAPI_IpRenewAddress
	, (PFNVOID)IPHAPI_NotifyAddrChange
	, (PFNVOID)IPHAPI_NotifyRouteChange

	, (PFNVOID)IPHAPI_CreateIpForwardEntry
	, (PFNVOID)IPHAPI_SetIpForwardEntry
	, (PFNVOID)IPHAPI_DeleteIpForwardEntry

	// ICMP层提供的 函数
	, (PFNVOID)IPHAPI_GetIcmpStatistics
	, (PFNVOID)IPHAPI_IcmpCreateFile
	, (PFNVOID)IPHAPI_IcmpCloseHandle
	, (PFNVOID)IPHAPI_IcmpSendEcho

	// TCP层提供的 函数

};

static const DWORD g_lpTcpipArgs[] = 
{
	0

	, ARG2_MAKE( DWORD, PTR )						//WSASartup
	, ARG0_MAKE()									//WSACleanup
	, ARG1_MAKE( DWORD )							//WSASetLastError
	, ARG0_MAKE()									//WSAGetLastError

	, ARG3_MAKE( DWORD, DWORD, DWORD )				//socket			//----5
	, ARG3_MAKE( DWORD, PTR, DWORD )				//bind
	, ARG3_MAKE( DWORD, PTR, DWORD )				//connect,
	, ARG2_MAKE( DWORD, DWORD)						//listen
	, ARG3_MAKE( DWORD, PTR, PTR )					//accept
	, ARG3_MAKE( DWORD, DWORD ,PTR)					//ioctlsocket,		//----10
	, ARG4_MAKE( DWORD, PTR, DWORD,DWORD )			//send,
	, ARG6_MAKE( DWORD, PTR, DWORD,DWORD ,PTR,DWORD)//sendto,
	, ARG4_MAKE( DWORD, PTR, DWORD,DWORD )			//recv
	, ARG6_MAKE( DWORD, PTR, DWORD,DWORD ,PTR,PTR)	//recvfrom
	, ARG5_MAKE( DWORD, PTR, PTR,PTR,PTR)			//select			//----15

	, ARG5_MAKE( DWORD, DWORD ,DWORD, PTR,PTR)		//getsockopt,
    , ARG5_MAKE( DWORD, DWORD,DWORD ,PTR,DWORD)		//setsockopt,
    , ARG3_MAKE(DWORD, PTR,PTR)						//getpeername,
    , ARG3_MAKE(DWORD, PTR,PTR)						//getsockname,

	, ARG2_MAKE( DWORD, DWORD )						//shutdown,			//----20
    , ARG1_MAKE( DWORD )							//closesocket,

	, ARG2_MAKE( PTR ,DWORD)						//gethostname		
    , ARG1_MAKE( PTR )								//gethostbyname,
    , ARG3_MAKE( PTR,DWORD,DWORD)					//gethostbyaddr,
    , ARG3_MAKE( PTR,PTR,PTR )						//gethostbynameex,	//----25
    , ARG5_MAKE( PTR,DWORD,DWORD, PTR,PTR)			//gethostbyaddrex,


	// IP层提供的 函数
    , ARG2_MAKE( PTR, PTR )				//IPHAPI_GetNetworkParams
    , ARG3_MAKE( PTR, DWORD, PTR )		//IPHAPI_GetDnsSrvList

    , ARG1_MAKE( PTR )					//IPHAPI_GetNumberOfInterfaces
    , ARG2_MAKE( PTR, PTR )				//IPHAPI_GetAdaptersInfo
    , ARG2_MAKE( PTR, PTR )				//IPHAPI_GetInterfaceInfo
    , ARG1_MAKE( PTR )					//IPHAPI_GetIfEntry
    , ARG3_MAKE( PTR, PTR, DWORD )		//IPHAPI_GetIfTable
    , ARG1_MAKE( PTR )					//IPHAPI_SetIfEntry
    , ARG3_MAKE( PTR, PTR, DWORD )		//IPHAPI_GetIpAddrTable
    , ARG2_MAKE( DWORD, PTR )			//IPHAPI_GetBestInterface


    , ARG1_MAKE( DWORD )				//IPHAPI_SetIpTTL
    , ARG1_MAKE( PTR )					//IPHAPI_GetIpStatistics

    , ARG5_MAKE( DWORD, DWORD, DWORD, PTR, PTR )//IPHAPI_AddIPAddress
    , ARG1_MAKE( PTR )					//IPHAPI_IpReleaseAddress
    , ARG1_MAKE( PTR )					//IPHAPI_IpRenewAddress
    , ARG2_MAKE( PTR, PTR )				//IPHAPI_NotifyAddrChange
    , ARG2_MAKE( PTR, PTR )				//IPHAPI_NotifyRouteChange

    , ARG1_MAKE( PTR )					//IPHAPI_CreateIpForwardEntry
    , ARG1_MAKE( PTR )					//IPHAPI_SetIpForwardEntry
    , ARG1_MAKE( PTR )					//IPHAPI_DeleteIpForwardEntry

	// ICMP层提供的 函数
    , ARG1_MAKE( PTR )					//IPHAPI_GetIcmpStatistics
    , ARG0_MAKE( )						//IPHAPI_IcmpCreateFile
    , ARG2_MAKE( PTR, PTR )				//IPHAPI_IcmpCloseHandle
    , ARG8_MAKE( PTR, DWORD, PTR, DWORD, PTR, PTR, PTR, DWORD )//IPHAPI_IcmpSendEcho

};

/******************************************************/


// ********************************************************************
// 声明：
// 参数：
// 返回值：
//	
// 功能描述：注册socket函数API
// 引用: 
// ********************************************************************
void	Sock_RegAPI( )
{
	API_RegisterEx( API_TCPIP, (PFNVOID*)g_lpTcpipAPI, g_lpTcpipArgs, ARR_COUNT(g_lpTcpipAPI) );
}

