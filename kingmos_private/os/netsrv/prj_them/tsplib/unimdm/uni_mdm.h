/**************************************************************************
* Copyright (c)  ΢�߼�(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef _MODEM_H_
#define _MODEM_H_

#ifdef __cplusplus
extern "C" {
#endif


//------------------------------------------
//MACRO 
//------------------------------------------

//dwExeFlag of Mdm_SendCmd---��ʹ���߸���AT������ص㣬��ѡ�����±�־!
#define	MEXEF_NOTPURGE			0x00000001
#define	MEXEF_ONLYWRITE			0x00000004
#define	MEXEF_READWAIT			0x00000008
#define	MEXEF_TRYONE			0x00000010
#define	MEXEF_TRYNUM			0x00000020
#define	MEXEF_NOT_CHK_DIAL		0x00000040
//#define	MEXEF_CHK_DIAL			0x00000040
#define	MEXEF_ADD_DIAL			0x00000080

#define	MEXEF_FINDERR			0x00000100
#define	MEXEF_FINDMEE			0x00000200
#define	MEXEF_FINDMSE			0x00000400
#define	MEXEF_MUSTOK			0x00000800
#define	MEXEF_NOTFINDOK			0x00001000
#define	MEXEF_FINDPREFIX		0x00002000
#define	MEXEF_FINDNOCAR			0x00004000
#define	MEXEF_FINDNOANS			0x00008000
#define	MEXEF_AFTEROK			0x00010000
#define	MEXEF_MUSTOK2			0x00020000

#define	MEXEF_MAYUNFOUND		0x00040000
#define	MEXEF_MUSTCRLF			0x00080000

#define	MEXEF_FINDATD			(MEXEF_FINDNOCAR |MEXEF_FINDNOANS)

#define	MEXEF_NOT_GETCTL		0x10000000

#define	MEXEF_DEFAULT			0x00000000

//result value of Mdm_SendCmd
#define	MR_DOWN					1
#define	MR_FAILURE				2
#define	MR_NOCTL				3

#define	MR_UNFOUND				4
#define	MR_OK					5
#define	MR_GSMERR				6
#define	MR_GSMMEE				7
#define	MR_GSMMSE				8
#define	MR_PREFIX				9
#define	MR_ONLYWRITE			12
#define	MR_CANCEL				13

#define	MR_CONNECT				19
#define	MR_NOCAR				20
#define	MR_NOANS				21
#define	MR_NOTONE				22
#define	MR_BUSY					23


//------------------------------------------
//AT����͵Ľṹ
//------------------------------------------

typedef	struct	_MDM_CMD_EXE
{
	//ִ�е�����
	LPSTR		pszCmd;			// IN 
	DWORD		dwLenCmd;		// IN 

	//ִ�е�����
	int			nTryNum;		// IN 
	DWORD		dwReadWait;		// IN 

	//���� ָ��ǰ׺---��Ϣ
	LPSTR		pszPrefix;		// IN 
	DWORD		dwLenPrefix;	// IN 

	//�õ��Ŀ���Ȩ
	DWORD		dwCtlID;		// OUT 

	//���� �����ָ�룬�����Ҫ
	LPSTR		pszBufRes;		// OUT 

	//���� <<"+CME ERROR: " / "+CMS ERROR: ">> �� ������ֵ
	DWORD		dwErrRes;		// OUT 

	//���� <<"ERROR" / "OK"�ȣ�ָ��ǰ׺>> �� ��ʼλ��
	LPSTR		pszFound;		// OUT 
	DWORD		dwLenFound;		// OUT 

	//�������
	DWORD		dwGsmErr;		// OUT 

} MDM_CMD_EXE;



//------------------------------------------
//AT����ͺͲ���/���õĺ���
//------------------------------------------

//
extern	BOOL	UniMdm_OpenAndCfg( LINE_DEV* pLineDev );
extern	BOOL	UniMdm_PrepareDial( LINE_DEV* pLineDev );
extern	BOOL	UniMdm_MakeCall( LINE_DEV* pLineDev, LPSTR pszBufCmd, DWORD dwCntRedial );
extern	void	UniMdm_HangupCall( LINE_DEV* pLineDev );
extern	void	UniMdm_SwitchToCmd( LINE_DEV* pLineDev );

//
extern	DWORD	UniMdm_SendCmd( LINE_DEV* pLineDev, IN OUT MDM_CMD_EXE* pInfoCmd, DWORD dwExeFlag, DWORD dwWait );

#ifdef __cplusplus
}	
#endif

#endif	//_MODEM_H_

