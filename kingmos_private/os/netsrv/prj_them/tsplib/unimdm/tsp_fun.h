/**************************************************************************
* Copyright (c)  ΢�߼�(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef _TSP_UNIMODEM_H_
#define _TSP_UNIMODEM_H_

#ifdef __cplusplus
extern "C" {
#endif


//---------------------------------------------------
//��̬�� ������
//---------------------------------------------------
//
#ifdef	_USRDLL

//for PC dll request
#ifdef UNIMODEM_EXPORTS
#define UNIMODEM_API __declspec(dllexport)
#else
#define UNIMODEM_API __declspec(dllimport)
#endif

#define GENERIC_DATA		0

#else	//_USRDLL

//for Kingmos
#define UNIMODEM_API

#endif	//_USRDLL


//---------------------------------------------------
// ����
//---------------------------------------------------

#ifdef	RDT_PPPOE

#ifndef	ETHER_MAC_LEN
#define	ETHER_MAC_LEN			6
#endif	//ETHER_MAC_LEN

#define	MAX_HOSTUNIQ			16

#endif

//---------------------------------------------------
//TSP �豸��Ϣ ����
//---------------------------------------------------

typedef	struct	_LINE_DEV
{
	HANDLE_THIS( _LINE_DEV );

	//TSP�豸����
	LIST_UNIT			hListDev;

	//Line�豸��Ϣ(����)
	DWORD				dwDevType;
	DWORD				dwEnumID;	//�� ras devices�� Ψһ����
	RASDEVOPEN			rasOpen;
	TCHAR				szDrvReg[ RAS_MaxDrvReg+8 ];
	//Line�豸������Ϣ
	HANDLE				hPort;
    CRITICAL_SECTION	csDev;
	DWORD				dwDevState;
	BOOL				fDialed;
	HANDLE				hEvtCmd;	//����֪ͨ
	DWORD				dwBaud;

	//������Ϣ
	TCHAR				szEntryName[ RAS_MaxEntryName + 8 ];

#ifdef	RDT_PPPOE
	WORD				wSID;
	WORD				wXXX;
	WORD				wLenName;
	WORD				wLenCookie;
	LPBYTE				pAcName;
	LPBYTE				pAcCookie;
	BYTE				bSrcMAC[ ETHER_MAC_LEN ];
	BYTE				bDesMAC[ ETHER_MAC_LEN ];
	BYTE				bHostUniq[ MAX_HOSTUNIQ ];

#else
	DWORD				dwResWrite;
#define	LEN_BUFRES			1024//512
	char				szBufRes[LEN_BUFRES+4];

#endif

	//Ras�豸��Ϣ
	HANDLE				hRasDev;

	//�����ϴ���Ϣ

#ifdef	RDT_PPPOE
	FNDLOUT				pFnDlOut;
#endif

//#ifdef	RDT_MDM
//	HANDLE				hIfDl;
//	FNPROIN				pFnDlIn;
//#endif

} LINE_DEV;

//dwDevState
#define	MDM_DOWN			0x10000000
#define	MDM_IDLE			0x20000000
#define	MDM_OPENED			0x80000000

#ifdef	RDT_PPPOE
#define	MDM_PADO			(0x00000002)
#endif

#define	MDM_COMMAND			(0x00000014)
#define	MDM_DATA			(0x00000015)


//---------------------------------------------------
//TSP ȫ����Ϣ ����
//---------------------------------------------------
typedef	struct	_TSP_GLOBAL
{
	HANDLE_THIS( _TSP_GLOBAL );

	HINSTANCE			hInst;
	//֪ͨRAS �ĺ���
	//FNRASNOTIFY		pFnNotifyRas;
	
	//���е� TSP�豸
	LIST_UNIT			hListDev;
	CRITICAL_SECTION	csListDev;

} TSP_GLOBAL;


//---------------------------------------------------
//ȫ������ ����
//---------------------------------------------------
//

//�����¼�
//#define	RASEVT_SEND(dwEvt, dwErr)	(*g_lpGlobalUni->pFnNotifyRas)( pLineDev->hRasDev, dwEvt, dwErr )
extern	void	RasIf_NotifyEvt( HANDLE hRasDev, DWORD dwRasCnnStatus, DWORD dwErr );
#define	RASEVT_SEND(dwEvt, dwErr)	RasIf_NotifyEvt( pLineDev->hRasDev, dwEvt, dwErr )

//״̬
extern	BOOL	lineDev_NewState( LINE_DEV* pLINE_DEV, DWORD dwDevState );
extern	BOOL	lineDev_IsDown( LINE_DEV* pLINE_DEV );


#ifdef __cplusplus
}	
#endif

#endif	//_TSP_UNIMODEM_H_

