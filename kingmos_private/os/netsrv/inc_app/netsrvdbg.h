/**************************************************************************
* Copyright (c)  ΢�߼�(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef _NETSRVDBG_H_
#define _NETSRVDBG_H_

#ifdef __cplusplus
extern "C" {
#endif


//----------
//SOCKET���� ��ʵ��
//----------

extern	DWORD	WINAPI	Skt_getsockdbg( );
extern	DWORD	WINAPI	Skt_enablesockdbg( BOOL fEnable, DWORD dwOptionDbg );


//----------
//�������� �ĺ����
//----------

#define	getsockdbg			Skt_getsockdbg		
#define	enablesockdbg		Skt_enablesockdbg		




#ifdef __cplusplus
}	
#endif

#endif	//_NETSRVDBG_H_
