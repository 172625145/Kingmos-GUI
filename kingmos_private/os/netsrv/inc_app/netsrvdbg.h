/**************************************************************************
* Copyright (c)  微逻辑(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef _NETSRVDBG_H_
#define _NETSRVDBG_H_

#ifdef __cplusplus
extern "C" {
#endif


//----------
//SOCKET函数 的实现
//----------

extern	DWORD	WINAPI	Skt_getsockdbg( );
extern	DWORD	WINAPI	Skt_enablesockdbg( BOOL fEnable, DWORD dwOptionDbg );


//----------
//辅助函数 的宏替代
//----------

#define	getsockdbg			Skt_getsockdbg		
#define	enablesockdbg		Skt_enablesockdbg		




#ifdef __cplusplus
}	
#endif

#endif	//_NETSRVDBG_H_
