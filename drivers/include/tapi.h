/**************************************************************************
* Copyright (c)  ΢�߼�(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef _TAPI_H_
#define _TAPI_H_

#ifndef	_RAS_H_
#include <ras.h>
#endif

#ifndef	_NETIF_COMM_H_
#include "netif_comm.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

// ----------------------------------------------------------
//  ����
// ----------------------------------------------------------

extern	DWORD	RasIf_UpData( HANDLE hRasDev, LPBYTE pBuf, DWORD dwLen );

//RAS�豸����
#define	MAX_CFG_BUF			128
typedef	struct	_RASDEVCFG
{
	WORD		wVersion;
	WORD		wSize;

	DCB			dcb;
	BYTE		pBufCfg[ MAX_CFG_BUF ];

} RASDEVCFG;


//TSP�豸�� �򿪷�ʽ
#define	MAX_RASOPENNAME		20
typedef	struct	_RASDEVOPEN
{
	BOOL		fOpenName;
	union
	{
		HANDLE	hDev;
		TCHAR	szOpenName[ MAX_RASOPENNAME+8 ];
	};

} RASDEVOPEN;


//֪ͨ RAS�ĺ���
typedef	void	(*FNRASNOTIFY)( HANDLE hRasDev, DWORD dwRasCnnStatus, DWORD dwErr );
//dwEvent
#define	PROEVT_DEV_INSERT	0x10000001	//�豸 ����---���� RAS�豸ע���� ����
#define	PROEVT_DEV_REMOVE	0x10000002	//�豸 ����---���� RAS�豸ע���� ����

#define	PROEVT_DEV_LINKED	0x10000010	//�豸�� �Ѿ����ӳɹ�(�粦�ųɹ����ȡAC�ɹ�)
#define	PROEVT_DEV_STOP		0x10000020	//ͣ�� �豸��
#define	PROEVT_DEV_CLOSE	0x10000030	//�ͷ� �豸��


//TSPI_OpenDev �Ĳ���
typedef	struct	_PARAM_OPENDEV
{
	HANDLE		hRasDev;
	LPTSTR		pszEntry;
	RASENTRY*	lpEntry;
	//

} PARAM_OPENDEV;


//TSP�豸�ṩ����·����Ϣ
typedef	struct	_TSP_DLINFO
{
	DWORD		dwMRU;

	//
	DWORD		dwDevType;
	//
	DWORD		dwNetType; //����GPRSʱ �� �豸������Ϣ
	//
	LPBYTE		pMAC;      //����ETHERʱ �� �豸������Ϣ
	DWORD		dwLenMAC;

} TSP_DLINFO;



//TSPI������
typedef struct _TSPITBL
{
	//�汾 �� ��С
	WORD	wSize;
	WORD	wVersion;

	//�豸����
	HANDLE	(*TSPI_CreateDev)( DWORD dwEnumID, LPTSTR pszDrvReg, RASDEVOPEN* lpRasOpenDev );
	BOOL	(*TSPI_OpenDev)( HANDLE hLineDev, PARAM_OPENDEV* pParamOpen );
	//void	(*TSPI_CloseDev)( HANDLE hLineDev );
	DWORD	(*TSPI_lineNotify)( HANDLE hLineDev, DWORD dwEvent );
	//����
	BOOL	(*TSPI_lineDial)( HANDLE hLineDev, LPCSTR pszLocalNumber );
	//���ųɹ��󣬻�ȡ�����Ϣ �� ���ý��պ���
	BOOL	(*TSPI_lineGetInfo)( HANDLE hLineDev, OUT TSP_DLINFO* pInfoDl );
	void	(*TSPI_lineSetRecv)( HANDLE hLineDev, HANDLE hIfDl, FNPROIN pFnIn );

	//TSP�豸���ͺ���
	DWORD	(*TSPI_lineSend)( HANDLE hLineDev, LPBYTE pBuf, DWORD dwLen );

} TSPITBL;


//TSP DLL ���������
typedef	 void	(*TSPI_GETTABLE)( TSPITBL** ppTspiTbl, HINSTANCE hInst, FNRASNOTIFY pNotifyRas );

#if 0
//modem������ͳһ����ֵ: lineDev_SendCmd / TSPI_SendData / 
#define	MR_OK			0x00
#define	MR_FAILURE		0x01
#define	MR_TIMEOUT		0x02
#define	MR_UNKNOWN		0x03
#define	MR_DOWN			0x04

#define	MR_CONNECT		0x13
#define	MR_NOANSWER		0x14
#define	MR_NODIALTONE	0x15
#define	MR_NOCARRIER	0x16
#define	MR_BUSY			0x17
#endif


#ifdef __cplusplus
}	
#endif

#endif	//_TAPI_H_
