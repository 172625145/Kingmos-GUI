/**************************************************************************
* Copyright (c)  ΢�߼�(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef	_NETIF_DL_RAS_H_
#define	_NETIF_DL_RAS_H_

#ifndef	_TAPI_H_
#include "tapi.h"
#endif

#ifndef	_NETIF_RAS_H_
#include "netif_ras.h"
#endif

#ifndef	_NETIF_DL_H_
#include "netif_dl.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif


//Datalink�� �� RAS/TSP�� �Ĺ�����Ϣ
typedef	struct	_PPP_LINK_RAS
{
	//RAS�㣺
	HANDLE			hRasDev;	//RAS�豸���
	DIALPARAM*		pDialParam;	//�ṩ���Ų���
	//TSP�㣺
	TSP_DLINFO*		pInfoFromTsp;//TSP�����ṩ����Ϣ

	//������·�㣺
	DL_LOCAL		LocalDl;

} PPP_LINK_RAS;
typedef	BOOL	(*DL_CREATEDEV)(PPP_LINK_RAS* pInfoLink, BOOL fOverHdlc);
//
extern	BOOL	PPP_CreateDev( PPP_LINK_RAS* pInfoLink, BOOL fOverHdlc );
extern	BOOL	PPPoEs_CreateDev( PPP_LINK_RAS* pInfoLink, BOOL fOverHdlc );


#ifdef __cplusplus
}	
#endif

#endif	//_NETIF_DL_RAS_H_
