/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

/*****************************************************
文件说明：PPP Debug
版本号：  1.0.0
开发时期：2003-12-01
作者：    肖远钢
修改记录：
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
/***************  全局区 定义， 声明 *****************/

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

	//句柄安全检查
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
