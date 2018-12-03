/**************************************************************************
* Copyright (c)  微逻辑(WEILUOJI). All rights reserved.                   *
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


//Datalink层 和 RAS/TSP层 的关联信息
typedef	struct	_PPP_LINK_RAS
{
	//RAS层：
	HANDLE			hRasDev;	//RAS设备句柄
	DIALPARAM*		pDialParam;	//提供拨号参数
	//TSP层：
	TSP_DLINFO*		pInfoFromTsp;//TSP额外提供的信息

	//拨号链路层：
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
