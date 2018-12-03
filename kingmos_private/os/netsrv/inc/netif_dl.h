/**************************************************************************
* Copyright (c)  ΢�߼�(WEILUOJI). All rights reserved.                   *
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
// Datalink ������ṩ�� ��������
// 
// ------------------------------------------------------

//------DataLink's Get MIB���� ����--------
typedef	BOOL	(*FNDLGETMIB)( HANDLE hIfDev, OUT PMIB_IFROW pIfRow );


//------DataLink's ���ͺ��� �� �����ṹ ����--------
typedef	struct	_PARAM_DLOUT
{
	DWORD		dwProtoID;
	DWORD		dwOption;
	LPBYTE		pDesAddr;
	DWORD		dwLenAddr;

	FRAGSETD*	pFragSetD;

} PARAM_DLOUT;
//dwOption
#define	DLOUT_OPT_ETH			0x10000000	//������ ָ����������ETHER֡
#define	DLOUT_OPT_ADDR_IP		0x20000000	//������ ָ����IP
#define	DLOUT_OPT_ADDR_MAC		0x40000000	//������ ָ����MAC
#define	DLOUT_OPT_ADDR_BC		0x80000000	//������ ��Ҫ�㲥

#define	DLOUT_OPT_ETH_NOSRC		(DLOUT_OPT_ETH | 0x00000002)	//������ ָ����ETHER�����ǲ�֪���Լ���MAC

typedef	DWORD	(*FNDLOUT)(HANDLE hIfDl, PARAM_DLOUT* pParamOut);


//Datalink�� �ṩ����Ϣ
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


//Datalink���Э��  �ϴ�����
typedef	struct	_DL_SUBMIT
{
	//����
	//PTR_NEXT(_DL_SUBMIT);

	//�ϴ���Э���
	union
	{
		DWORD		dwProtocol;
		WORD		wProtocol;
		BYTE		bProtocol;
	} ;
	DWORD			dwOption;

	//�����ĺ���
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
