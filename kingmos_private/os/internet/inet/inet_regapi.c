/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵����internet API ע��
�汾�ţ�  1.0.0
����ʱ�ڣ�2004-05-09
���ߣ�    ФԶ��
�޸ļ�¼��
******************************************************/
//root include
#include <ewindows.h>
#include <listunit.h>
#include <eapisrv.h>
//"\inc_app"
#include <internet.h>
//"\inc_drv"
#include "handle_check.h"
//"\inc_local"
#include "inet_call.h"


/***************  ȫ���� ���壬 ���� *****************/

static	const	PFNVOID	g_lpINetAPI[] =
{
	  0

// inet ����
	, (PFNVOID)INETAPI_InternetOpen
	, (PFNVOID)INETAPI_InternetConnect
	, (PFNVOID)INETAPI_InternetReadFile
	, (PFNVOID)INETAPI_InternetCloseHandle
	, (PFNVOID)INETAPI_InternetSetStatusCallback

// http ����
	, (PFNVOID)INETAPI_HttpOpenRequest
	, (PFNVOID)INETAPI_InternetOpenUrl
	, (PFNVOID)INETAPI_HttpAddRequestHeaders
	, (PFNVOID)INETAPI_HttpSendRequest

	, (PFNVOID)INETAPI_HttpQueryQuick
	, (PFNVOID)INETAPI_HttpQueryInfo

// ftp ����

};


static const DWORD g_lpINetArgs[] = 
{
	0

// inet ����
	, ARG5_MAKE( PTR, DWORD, PTR, PTR, DWORD )						//INETAPI_InternetOpen
	, ARG8_MAKE( DWORD, PTR, DWORD, PTR, PTR, DWORD, DWORD, DWORD )	//INETAPI_InternetConnect
	, ARG4_MAKE( DWORD, PTR, DWORD, PTR )							//INETAPI_InternetReadFile
	, ARG1_MAKE( DWORD )											//INETAPI_InternetCloseHandle
	, ARG3_MAKE( DWORD, PTR, DWORD )								//INETAPI_InternetSetStatusCallback

// inet ����
	, ARG8_MAKE( DWORD, PTR, PTR, PTR, PTR, PTR, DWORD, DWORD )		//INETAPI_HttpOpenRequest
	, ARG6_MAKE( DWORD, PTR, PTR, DWORD, DWORD, DWORD )				//INETAPI_InternetOpenUrl
	, ARG4_MAKE( DWORD, PTR, DWORD, DWORD )							//INETAPI_HttpAddRequestHeaders
	, ARG5_MAKE( DWORD, PTR, DWORD, PTR, DWORD )					//INETAPI_HttpSendRequest

	, ARG6_MAKE( DWORD, PTR, PTR, PTR, DWORD, PTR )					//INETAPI_HttpQueryQuick
	, ARG5_MAKE( DWORD, DWORD, PTR, PTR, PTR )						//INETAPI_HttpQueryInfo

// ftp ����

};

/******************************************************/


void	Internet_RegAPI()
{
	API_RegisterEx( API_INTERNET, (PFNVOID*)g_lpINetAPI, g_lpINetArgs, ARR_COUNT(g_lpINetAPI) );
	API_SetReady( API_INTERNET );
}

