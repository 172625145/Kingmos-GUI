/**************************************************************************
* Copyright (c)  ΢�߼�(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef _RAS_DEF_H_
#define _RAS_DEF_H_

#ifdef __cplusplus
extern "C" {
#endif


//------------------------------------------------------
// �궨��
//------------------------------------------------------
#define	TIMER_STATBPS			1200

#define	RAS_BPS_1

//------------------------------------------------------
// ���� �ṹ
//------------------------------------------------------

// TSPI DLL����Ϣ
typedef	struct	_TSPINFO
{
	HANDLE_THIS( _TSPINFO );

	LIST_UNIT			hListTsp;
	//������Ϣ
	LONG				nRefCnt;
	//��̬����Ϣ
#ifdef RAS_LOAD_TSPDLL
	HANDLE				hDll;
#endif
	TCHAR				pszDllName[MAX_PATH];
	TSPITBL*			pTspTbl;

} TSPINFO;


// RAS�����豸 ����Ϣ
typedef	struct	_RAS_DEV
{
	HANDLE_THIS( _RAS_DEV );

	LIST_UNIT			hListRasDev;	//�豸����

	//�豸��Ϣ:
	DWORD				dwEnumID;	//�� ras_tapi.c�� Ψһ����
	DWORD				dwDevType;
	TCHAR				szDrvReg[ RAS_MaxDrvReg+8 ];
	RASDEVOPEN			rasOpen;

	//������Ϣ��
	WORD				wFlag;
	WORD				wDevOp;
	CRITICAL_SECTION	csRasDev;
	DWORD				dwDevStatus;
	HANDLE				hEvtWaitCnn;

	//������Ŀ��Ϣ
	TCHAR				szEntryName[ RAS_MaxEntryName + 4 ];

	//RAS APP��Ϣ
	DWORD				dwNotifierType;
	LPVOID				notifier;
	//TSP DLL �� TSP �豸  ����Ϣ
	TSPINFO*			lpTspInfo;
	HANDLE				hLineDev;
	//PPP ������Ϣ
	HANDLE				hIfDl;
	FNPRONOTIFY			pFnDlNotify;
	FNPROIN				pFnDlIn;

	//
	//ͳ����Ϣ
	//
	//ͳ��---����ʱ��
	DWORD				dwTickCnn;
	//ͳ��---���Ӻ� �յ�/���� ���ֽ���
	DWORD				dwBytesXmited;
	DWORD				dwBytesRcved;
	//DWORD				dwCompressionRatioIn;
	//DWORD				dwCompressionRatioOut;
	//DWORD				dwConnectDuration;

	//ͳ��---�ٶ�
#ifdef RAS_BPS_1
	HANDLE				hThrdBps;
	HANDLE				hEvtBps;

	DWORD				dwBytesRx_Bps;  //ÿ��ͳ�������� ���յ� Bytes
	DWORD				dwTickRx_Bps;   //ÿ��ͳ�����ڵ� ��ʼ
	DWORD				dwRxBps;        //ͳ�ƽ����ٶ�

	DWORD				dwBytesTx_Bps;  //ÿ��ͳ�������� ���͵� Bytes
	DWORD				dwTickTx_Bps;   //ÿ��ͳ�����ڵ� ��ʼ
	DWORD				dwTxBps;        //ͳ�Ʒ����ٶ�

#else
	DWORD				dwBytesRx_Bps;  //ÿ��ͳ�������� ���յ� Bytes
	DWORD				dwTickRx_Bps;   //ÿ��ͳ�����ڵ� ��ʼ

	DWORD				dwBytesTx_Bps;  //ÿ��ͳ�������� ���͵� Bytes
	DWORD				dwTickTx_Bps;   //ÿ��ͳ�����ڵ� ��ʼ

#endif

} RAS_DEV;

//wFlag
#define	RASDF_EXIT				0x8000

//wDevOp
#define	RASOP_HANGUP			0x0010
#define	RASOP_FREE				0x0020
#define	RASOP_WAITMS			0x0040

#define	RASOP_NO_BPS			0x0100


// RAS ��ȫ�ֽṹ
typedef	struct	_RAS_GLOBAL
{
	HANDLE_THIS( _RAS_GLOBAL );

	//����ע��� RAS�豸
	CRITICAL_SECTION	csListRasDev;	//�����豸�Ĳ���
	LIST_UNIT			hListRasDev;
	//���м��ص� Tsp Dll��Ϣ
	CRITICAL_SECTION	csListTsp;
	LIST_UNIT			hListTsp;

	//������Ϣ
	DWORD				dwEnumNewID;
	HWND				hWndNetMgr;

} RAS_GLOBAL;

extern	RAS_GLOBAL*		g_lpGlobalRas;


#ifdef __cplusplus
}	
#endif

#endif	//_RAS_DEF_H_
