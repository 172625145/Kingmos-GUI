/**************************************************************************
* Copyright (c)  微逻辑(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef _SKT_CALL_VER_H_
#define _SKT_CALL_VER_H_

#ifndef XYG_PC_PRJ
#ifndef _SKT_CALL_H_
#include "skt_call.h"
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif


//
//定义 socket操作的替代功能
//
#ifdef XYG_PC_PRJ//EML_WIN32
//在KINGMOS PC版上

#define	ver_WSAStartup			WSAStartup		
#define	ver_WSACleanup			WSACleanup		
#define	ver_WSASetLastError		WSASetLastError	
#define	ver_WSAGetLastError		WSAGetLastError	

#define	ver_socket				socket			
#define	ver_bind				bind			
#define	ver_connect				connect			
#define	ver_listen				listen			
#define	ver_accept				accept			
#define	ver_ioctlsocket			ioctlsocket		
#define	ver_send				send			
#define	ver_sendto				sendto			
#define	ver_recv				recv			
#define	ver_recvfrom			recvfrom		
#define	ver_select				select			

#define	ver_getsockopt			getsockopt		
#define	ver_setsockopt			setsockopt		
#define	ver_getpeername			getpeername		
#define	ver_getsockname			getsockname		

#define	ver_shutdown			shutdown		
#define	ver_closesocket			closesocket		

#define	ver_gethostname			gethostname		
#define	ver_gethostbyname		gethostbyname	
#define	ver_gethostbyaddr		gethostbyaddr	
#define	ver_gethostbynameex		gethostbynameex	
#define	ver_gethostbyaddrex		gethostbyaddrex	

#define	ver_getsockdbg			getsockdbg		
#define	ver_enablesockdbg		enablesockdbg	

#else
//在KINGMOS 版上

#define	ver_WSAStartup			SKAPI_WSAStartup
#define	ver_WSACleanup			SKAPI_WSACleanup
#define	ver_WSASetLastError		SKAPI_WSASetLastError
#define	ver_WSAGetLastError		SKAPI_WSAGetLastError

#define	ver_socket				SKAPI_Socket			
#define	ver_bind				SKAPI_Bind
#define	ver_connect				SKAPI_Connect
#define	ver_listen				SKAPI_Listen 
#define	ver_accept				SKAPI_Accept
#define	ver_ioctlsocket			SKAPI_IoctlSocket	
#define	ver_send				SKAPI_Send
#define	ver_sendto				SKAPI_SendTo
#define	ver_recv				SKAPI_Recv
#define	ver_recvfrom			SKAPI_RecvFrom
#define	ver_select				SKAPI_Select			

#define	ver_getsockopt			SKAPI_GetSockOpt	
#define	ver_setsockopt			SKAPI_SetSockOpt
#define	ver_getpeername			SKAPI_GetPeerName	
#define	ver_getsockname			SKAPI_GetSockName

#define	ver_shutdown			SKAPI_ShutDown		
#define	ver_closesocket			SKAPI_CloseSocket 

#define	ver_gethostname			SKAPI_GetHostName
#define	ver_gethostbyname		SKAPI_GetHostByName
#define	ver_gethostbyaddr		SKAPI_GetHostByAddr
#define	ver_gethostbynameex		SKAPI_GetHostByNameEx
#define	ver_gethostbyaddrex		SKAPI_GetHostByAddrEx

#define	ver_getsockdbg			SKAPI_GetSockDbg
#define	ver_enablesockdbg		SKAPI_EnableSockDbg


#endif



#ifdef __cplusplus
}	
#endif

#endif	//_SKT_CALL_VER_H_
