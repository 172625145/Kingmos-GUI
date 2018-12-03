/**************************************************************************
* Copyright (c)  微逻辑(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef	_NETIF_DL_H_
#define	_NETIF_DL_H_

#ifndef	_IPHLPAPI_H_
#include "iphlpapi.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif


// ------------------------------------------------------
// 
// Datalink 层可以提供的 函数功能
// 
// ------------------------------------------------------

//------DataLink's Get MIB函数 定义--------
typedef	BOOL	(*FNDLGETMIB)( HANDLE hIfDev, OUT PMIB_IFROW pIfRow );


//------DataLink's 发送函数 和 参数结构 定义--------
typedef	struct	_PARAM_DLOUT
{
	DWORD		dwProtoID;
	DWORD		dwOption;
	LPBYTE		pDesAddr;
	DWORD		dwLenAddr;

	FRAGSETD*	pFragSetD;

} PARAM_DLOUT;
//dwOption
#define	DLOUT_OPT_ETH			0x10000000	//发送者 指定了完整的ETHER帧
#define	DLOUT_OPT_ADDR_IP		0x20000000	//发送者 指定了IP
#define	DLOUT_OPT_ADDR_MAC		0x40000000	//发送者 指定了MAC
#define	DLOUT_OPT_ADDR_BC		0x80000000	//发送者 需要广播

#define	DLOUT_OPT_ETH_NOSRC		(DLOUT_OPT_ETH | 0x00000002)	//发送者 指定了ETHER，但是不知道自己的MAC

typedef	DWORD	(*FNDLOUT)(HANDLE hIfDl, PARAM_DLOUT* pParamOut);


//Datalink层 提供的信息
typedef	struct	_DL_LOCAL
{
	HANDLE			hIfDev;

	FNPRONOTIFY		pFnDlNotify;
	FNPROQUERY		pFnDlQuery;
	FNDLGETMIB		pFnDlGetMIB;
	FNPROSET		pFnDlSet;
	FNDLOUT			pFnDlOut;

	FNPROIN			pFnDlIn;

} DL_LOCAL;


//Datalink层的协议  上传处理
typedef	struct	_DL_SUBMIT
{
	//链表
	//PTR_NEXT(_DL_SUBMIT);

	//上传的协议号
	union
	{
		DWORD		dwProtocol;
		WORD		wProtocol;
		BYTE		bProtocol;
	} ;
	DWORD			dwOption;

	//处理层的函数
	HANDLE			hIf;
	FNPRONOTIFY		pFnNotify;
	FNPROIN			pFnIn;

} DL_SUBMIT;
typedef	BOOL	(*DL_CREATEUP)(DL_SUBMIT* pInfoUp, IN OUT DL_LOCAL* pLocalDl);

#define	ETHUP_OPT_GETHDR		0x00000001
#define	ETHUP_OPT_TAIL			0x00000002



#ifdef __cplusplus
}	
#endif

#endif	//_NETIF_DL_H_
