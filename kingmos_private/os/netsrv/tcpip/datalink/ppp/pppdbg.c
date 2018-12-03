/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

/*****************************************************
�ļ�˵����PPP Debug
�汾�ţ�  1.0.0
����ʱ�ڣ�2003-12-01
���ߣ�    ФԶ��
�޸ļ�¼��
******************************************************/
//root include
#include <ewindows.h>
#include <listunit.h>
//"\inc_app"
#include <socket.h>
#include <pcfile.h>
//"\inc_local"
#include "netif_dl_ras.h"
//"local"
#include "ppp_def.h"


#if DEBUG_PPP_LOG
/***************  ȫ���� ���壬 ���� *****************/

#ifdef	VC386
#define	PAHT_PPP_DBGITEM	"C:\\netdbg\\pppitem.xyg"
#define	PAHT_PPP_DBGBUF		"C:\\netdbg\\pppbuf.xyg"
#else
#define	PAHT_PPP_DBGITEM	"\\pppitem.xyg"
#define	PAHT_PPP_DBGBUF		"\\pppbuf.xyg"
#endif

/******************************************************/


void	PppDbg_Open()
{
	RETAILMSG(1,(TEXT("\r\n\r\n PppDbg_Open: called!\r\n")));
	if( !g_lpGlobalPPP->hDbgItem || g_lpGlobalPPP->hDbgItem==INVALID_HANDLE_VALUE )
	{
		g_lpGlobalPPP->hDbgItem = ver_CreateFile( PAHT_PPP_DBGITEM, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL );
		RETAILMSG(1,(TEXT("\r\n PppDbg_Open: [%s]\r\n"), PAHT_PPP_DBGITEM, g_lpGlobalPPP->hDbgItem));
		ver_SetEndOfFile( g_lpGlobalPPP->hDbgItem );
	}
	if( !g_lpGlobalPPP->hDbgBuf || g_lpGlobalPPP->hDbgBuf==INVALID_HANDLE_VALUE )
	{
		g_lpGlobalPPP->hDbgBuf = ver_CreateFile( PAHT_PPP_DBGBUF, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, NULL );
		RETAILMSG(1,(TEXT("\r\n PppDbg_Open: [%s]\r\n"), PAHT_PPP_DBGBUF, g_lpGlobalPPP->hDbgBuf));
		ver_SetEndOfFile( g_lpGlobalPPP->hDbgBuf );
	}
}

void	PppDbg_Close()
{
	RETAILMSG(1,(TEXT("\r\n\r\n PppDbg_Close: called!\r\n")));
	ver_CloseHandle( g_lpGlobalPPP->hDbgItem );
	g_lpGlobalPPP->hDbgItem = INVALID_HANDLE_VALUE;

	ver_CloseHandle( g_lpGlobalPPP->hDbgBuf );
	g_lpGlobalPPP->hDbgBuf = INVALID_HANDLE_VALUE;
}

void	PppDbg_Write( PPPDBG_ITEM* pItemDbg, LPBYTE pBuf )
{
	DWORD	dwTmp;

	//�����ȫ���
	if( !HANDLE_CHECK(g_lpGlobalPPP) )
	{
		return ;
	}
	if( (g_lpGlobalPPP->hDbgItem==INVALID_HANDLE_VALUE) || (g_lpGlobalPPP->hDbgBuf==INVALID_HANDLE_VALUE) )
	{
		return ;
	}
	pItemDbg->dwPosSav = g_lpGlobalPPP->dwPosBuf;
	ver_WriteFile( g_lpGlobalPPP->hDbgItem, pItemDbg, sizeof(PPPDBG_ITEM), &dwTmp, NULL );

	//
	ver_WriteFile( g_lpGlobalPPP->hDbgBuf, pBuf, pItemDbg->wLen, &dwTmp, NULL );
	g_lpGlobalPPP->dwPosBuf +=dwTmp;
}

#endif
