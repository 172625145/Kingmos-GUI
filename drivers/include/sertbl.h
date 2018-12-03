/**************************************************************************
* Copyright (c)  ΢�߼�(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef __SER_TBL_H__
#define __SER_TBL_H__

#ifdef __cplusplus
extern "C" {
#endif

//define interrupt type
#define	INTR_NONE		0
#define	INTR_LINE		1
#define	INTR_RX			2
#define	INTR_TX			4
#define	INTR_MODEM		8

//function table
typedef struct __VTBL_ARCH_SER
{
	//ͨ�� ����
	PVOID	(*lpFnInit)(ULONG Identifier, PVOID pMDDContext, PVOID lpDCB);
	VOID	(*lpFnInitLast)(PVOID pHead);
	ULONG	(*lpFnDeinit)(PVOID pHead);
	BOOL	(*lpFnOpen)(PVOID pHead);
	ULONG	(*lpFnClose)(PVOID pHead);
	BOOL	(*lpFnPowerOff)(PVOID pHead);
	BOOL	(*lpFnPowerOn)(PVOID pHead);
	BOOL    (*lpFnIOControl)(PVOID pHead, DWORD dwCode,PBYTE pBufIn,DWORD dwLenIn,
						PBYTE pBufOut,DWORD dwLenOut,PDWORD pdwActualOut);
	//�շ� ����
	ULONG	(*lpFnRecv)(PVOID pHead, LPBYTE pBufDes, LONG* pnLenBuf);
	ULONG	(*lpFnSend)(PVOID pHead, LPBYTE pBufSrc, LONG* pnLenBuf);
	BOOL	(*lpFnXmitChar)(PVOID pHead, UCHAR ComChar);

	//�ж� ����
	ULONG	(*lpFnIntrTypeQuery)(PVOID pHead);
	VOID	(*lpFnIntrHandleModem)(PVOID pHead);
	VOID	(*lpFnIntrHandleLine)(PVOID pHead);
	VOID	(*lpFnIntrHandleTx)(PVOID pHead);

	//IR�����Ĺ��� ����
	BOOL	(*lpFnIREnable)(PVOID pHead, ULONG BaudRate);
	BOOL	(*lpFnIRDisable)(PVOID pHead);

	//Pin Singal�Ĺ��� ����
	VOID	(*lpFnClearSingalDTR)(PVOID pHead);
	VOID 	(*lpFnSetSingalDTR)(PVOID pHead);
	VOID	(*lpFnClearSingalRTS)(PVOID pHead);
	VOID	(*lpFnSetSingalRTS)(PVOID pHead);

	//Break�Ĺ��� ����
	VOID	(*lpFnClearBreak)(PVOID pHead);
	VOID	(*lpFnSetBreak)(PVOID pHead);

	//Purge�Ĺ��� ����
	VOID	(*lpFnPurgeComm)(PVOID pHead, DWORD fdwAction);

	//״̬���ԵĹ��� ����---for ClearCommError
	ULONG	(*lpFnGetComStat)(PVOID pHead, LPCOMSTAT lpStat);
	//״̬���ԵĹ��� ����---for GetCommModemStatus
	VOID	(*lpFnGetCommModemStatus)(PVOID pHead, LPDWORD pModemStatus);
	//״̬���ԵĹ��� ����---for GetCommProperties
	VOID	(*lpFnGetCommProperties)(PVOID pHead, LPCOMMPROP pCommProp);
	//״̬���ԵĹ��� ����---for SetCommState
	BOOL	(*lpFnSetDCB)(PVOID pHead, LPDCB pDCB, DWORD dwFlagSetDCB);
	//�õ���PDD��Ҫ�Ľ���buffer
	ULONG   (*lpGetRxBufferSize)( PVOID pHead ); 

} VTBL_ARCH_SER, *PVTBL_ARCH_SER;


//define serial ID, used to open
#define	ID_COM1				1
#define	ID_COM2				2
#define	ID_COM3				3
#define	ID_COM_MAX_RS232	ID_COM3

#define	ID_COM5				5

#define	ID_COM9				9
#define	ID_COM8				8
#define	ID_COM7				7
#define	ID_COM_MIN_CARD		ID_COM7


//hardware object
typedef struct __OBJECT_ARCH
{
	DWORD		dwIntID;	// Interrupt Identifier
	LPVOID		pFunTbl;

}OBJECT_ARCH, *POBJECT_ARCH;

//Baudrate setting
#define	SETDCB_NONE			0x0000
#define	SETDCB_BAUD			0x0001
#define	SETDCB_BYTESIZE		0x0002
#define	SETDCB_PARITY		0x0004
#define	SETDCB_STOPBITS		0x0008
#define	SETDCB_ALL			(SETDCB_BAUD |SETDCB_BYTESIZE |SETDCB_PARITY |SETDCB_PARITY |SETDCB_STOPBITS)

//Ĭ�ϵ� buffer ����
#define RX_BUFFER_SIZE            2048

#ifdef __cplusplus
}	
#endif

#endif	//__SER_TBL_H__
