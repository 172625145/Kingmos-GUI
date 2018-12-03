/**************************************************************************
* Copyright (c)  微逻辑(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef _TCP_UDP_DEF_H_
#define _TCP_UDP_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif

//-------------------------------------------------------
//dwOption
#define	TCPIPO_NONBLOCK				0x00000001
#define	TCPIPO_DEBUG				0x00000002
#define	TCPIPO_NONKEEPALIVE			0x00000004
#define	TCPIPO_IPINTF				0x00000008
#define	TCPIPO_BROADCAST			0x00000010	//允许收发广播

//wFlag
#define	TCPIPF_EXIT					0x1000
#define	TCPIPF_DEBUG				0x2000

#ifdef __cplusplus
}	
#endif

#endif	//_TCP_UDP_DEF_H_
