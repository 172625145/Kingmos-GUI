/**************************************************************************
* Copyright (c)  ΢�߼�(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef _SOCKET_DEF_H_
#define _SOCKET_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif

//---------------------------------------------------
//SOCK ȫ����Ϣ
//---------------------------------------------------
typedef	struct	_SOCK_PROC
{
	HANDLE_THIS( _SOCK_PROC );

	LIST_UNIT			hListSkProc;

	//
	SKERR				nErrSock;
	SOCKET				nNewID;
	BOOL				fToEnd;

	// ----ʵ��
	CRITICAL_SECTION	csListSkInst;
	LIST_UNIT			hListSkInst;

	//App����Ϣ
	HANDLE				hProc;

} SOCK_PROC;


//---------------------------------------------------
//SOCK ȫ����Ϣ
//---------------------------------------------------
typedef	struct	_SOCK_GLOBAL
{
	HANDLE_THIS( _SOCK_GLOBAL );

	//�������Ϣ
	CRITICAL_SECTION	csListTl;
	LIST_UNIT			hListTl;

	//���н��̵�socketʵ����Ϣ
	CRITICAL_SECTION	csListSkProc;
	LIST_UNIT			hListSkProc;

	//
	//DWORD				dwOptDns;

	//for debug
	DWORD				dwOptionDbg;
	HANDLE				hFileDbg;
	DWORD				dwCntFileDbg;

} SOCK_GLOBAL;

#define	SKTID_MIN			5
#define	SKTID_MAX			(0xFFFFFFFF-5)

extern	SOCK_GLOBAL*		g_lpGlobalSock;


#ifdef __cplusplus
}	
#endif

#endif	//_SOCKET_DEF_H_

