/**************************************************************************
* Copyright (c)  微逻辑(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef _MODEM_H_
#define _MODEM_H_

#ifdef __cplusplus
extern "C" {
#endif


//------------------------------------------
//MACRO 
//------------------------------------------

//dwExeFlag of Mdm_SendCmd---请使用者根据AT命令的特点，来选用如下标志!
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
//AT命令发送的结构
//------------------------------------------

typedef	struct	_MDM_CMD_EXE
{
	//执行的命令
	LPSTR		pszCmd;			// IN 
	DWORD		dwLenCmd;		// IN 

	//执行的特征
	int			nTryNum;		// IN 
	DWORD		dwReadWait;		// IN 

	//搜索 指定前缀---信息
	LPSTR		pszPrefix;		// IN 
	DWORD		dwLenPrefix;	// IN 

	//得到的控制权
	DWORD		dwCtlID;		// OUT 

	//保存 结果的指针，如果需要
	LPSTR		pszBufRes;		// OUT 

	//保存 <<"+CME ERROR: " / "+CMS ERROR: ">> 的 错误数值
	DWORD		dwErrRes;		// OUT 

	//保存 <<"ERROR" / "OK"等，指定前缀>> 的 起始位置
	LPSTR		pszFound;		// OUT 
	DWORD		dwLenFound;		// OUT 

	//如果出错
	DWORD		dwGsmErr;		// OUT 

} MDM_CMD_EXE;



//------------------------------------------
//AT命令发送和测试/设置的函数
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

