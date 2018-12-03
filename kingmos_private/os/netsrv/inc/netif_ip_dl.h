/**************************************************************************
* Copyright (c)  微逻辑(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef	_NETIF_IP_DL_H_
#define	_NETIF_IP_DL_H_

#ifndef	_TAPI_H_
#include "tapi.h"
#endif

#ifndef	_NETIF_RAS_H_
#include "netif_ras.h"
#endif

#ifndef	_NETIF_DL_H_
#include "netif_dl.h"
#endif

#ifndef	_NETIF_IP_H_
#include "netif_ip.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif



//Net层 和 DataLink层 的关联信息
typedef	struct	_NET_LINK_DL
{
	//拨号链路层：
	DL_LOCAL		LocalDl;

	//Net层：
	NET_LOCAL		LocalNet;

} NET_LINK_DL;
typedef	BOOL	(*NET_CREATEIF)(NET_LINK_DL* pInfoLink);

extern	BOOL	IP_CreateIf( NET_LINK_DL* pInfoLink );



#ifdef __cplusplus
}	
#endif

#endif	//_NETIF_IP_DL_H_
