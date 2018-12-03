#ifndef	__NET_DEVICE_MDD_H__
#define	__NET_DEVICE_MDD_H__

/*	This function defines the structures and functions used in the PDD driver of a Network device.
	Following functions must be implemented by every PDD driver.

    DisableInterrupt;
    EnableInterrupt;
    Halt;
    HandleInterrupt;
    Initialize;
    QueryInformation;
    Reset;
    Send;
    SetInformation;
    TransferData;
*/

#include "ewindows.h"

#define		IN
#define		OUT

//definitions for Interrupt mask value
#define		INT_MASK_RECV		0X01
#define		INT_MASK_XMIT		0X02
#define		INT_MASK_RECV_ERROR	0X04
#define		INT_MASK_XMIT_ERROR	0X08
//#define		INT_MASK_OVERRUN	0X10
#define		INT_MASK_ERROR		0X20
#define		INT_MASK_DMA		0X40
#define		INT_MASK_ALL		0XFF

//definitions for Interrupt reason.




// definitions for Error Message transfered between MDD and PDD.
#define		ERROR_NOT_SUPPORTED				99
#define		ERROR_BUF_NOT_ENOUGH			100

#define		Infor_Address			0

//typedef	 UCHAR *	PLAN_MEDIUM		;
/*
typedef	 struct  __LAN_PACKET{

	PUCHAR	pData;
	DWORD	dwPackLong;
}LAN_PACKET, *PLAN_PACKET;
*/
typedef	void	(* DISABLE_INTERRUPT_HANDLER)(  IN  HANDLE  MiniportAdapterContext,IN DWORD	dwIntMask );
typedef void    (* ENABLE_INTERRUPT_HANDLER) (  IN  HANDLE  MiniportAdapterContext,IN DWORD	dwIntMask );
typedef void    (* START_HANDLER)(    IN  HANDLE        MiniportAdapterContext    );
typedef void    (* HALT_HANDLER)(    IN  HANDLE        MiniportAdapterContext    );
typedef DWORD	(* INTERRUPT_HANDLER)(    IN  HANDLE   MiniportAdapterContext    );
typedef void  (* DEINITIALIZE_HANDLER)(
	IN	HANDLE					hAdapter
    );
typedef HANDLE  (* INITIALIZE_HANDLER)(
	IN	HANDLE					hMddContext,
    OUT PDWORD		            OpenErrorStatus,
	IN	DWORD					identifer
/*    OUT PDWORD                  SelectedMediumIndex,
    IN  PLAN_MEDIUM             MediumArray,
    IN  UINT                    MediumArraySize*/
    );

typedef	DWORD	(*QUERY_INFORMATION_HANDLER)(
    IN  HANDLE					MiniportAdapterContext,
    IN  int                Oid,
    IN  PVOID                   InformationBuffer,
    IN  ULONG                   InformationBufferLength,
    OUT PDWORD                  BytesWritten,
    OUT PDWORD                  BytesNeeded
    );

typedef  void	(*RESET_HANDLER)(
    IN  HANDLE             MiniportAdapterContext
    );

typedef	DWORD (*SEND_HANDLER)(
    IN  HANDLE					MiniportAdapterContext,
	IN	PUCHAR					pBuf,
	IN	DWORD					dwBufLong,
//    IN  PLAN_PACKET             Packet,
    IN  UINT                    Flags
    );

typedef DWORD (*SET_INFORMATION_HANDLER)(
    IN  HANDLE		           MiniportAdapterContext,
    IN  int		                Oid,
    IN  PVOID                   InformationBuffer,
    IN  ULONG                   InformationBufferLength,
    OUT PDWORD                  BytesRead,
    OUT PDWORD                  BytesNeeded
    );

typedef	DWORD (*TRANSFER_DATA_HANDLER)(
	IN	HANDLE						hAdapter,	
	IN  PUCHAR						pBuf,
//    IN  OUT PLAN_PACKET             Packet,
    IN OUT DWORD					dwBytes,
	IN OUT UINT						*pFlags
    );

//typedef		int (*INTERRUPT_REQUEST ) ( IN HANDLE hAdapter ) ;



typedef  struct __LAN_DEV_ADAPTOR_FUNC{

	DISABLE_INTERRUPT_HANDLER	DisableInterrupt;
	ENABLE_INTERRUPT_HANDLER	EnableInterrupt;
	INTERRUPT_HANDLER			InterruptHandler;

	START_HANDLER				StartDevice;
	HALT_HANDLER				HaltDevice;
	INITIALIZE_HANDLER			InitializeDev;
	DEINITIALIZE_HANDLER		DeInitializeDev;
	RESET_HANDLER				ResetDevice;

	QUERY_INFORMATION_HANDLER	QueryInformation;
	SET_INFORMATION_HANDLER		SetInforamtion;
	SEND_HANDLER				SendPacket;
	TRANSFER_DATA_HANDLER		ReadPacket;
	
	//INTERRUPT_REQUEST			Int_Request;
//Note: the send & read function always transfer a total packet.

}LAN_DEV_ADAPTOR_FUNC, *PLAN_DEV_ADAPTOR_FUNC;





struct __Ether_Addr{

	union {
	
		UCHAR   cAddr[6];
		struct {
			UCHAR  cAddr0;
			UCHAR  cAddr1;
			UCHAR  cAddr2;
			UCHAR  cAddr3;
			UCHAR  cAddr4;
			UCHAR  cAddr5;
		};
	};
} __attribute__ ((packed));

typedef	struct __Ether_Addr	Ether_Addr;

#define		ETHER_ADDR_SIZE		(sizeof(Ether_Addr))

typedef	enum _Lan_State{

	Device_State_Dead,
	Device_State_Init,
	Device_State_Opened,
	Device_State_Closed

}Device_State;





typedef struct __Lan_Device_MDD{

	struct __Lan_Device * pNext;
	HANDLE				hLanDevicePdd;
    CRITICAL_SECTION    csDeviceCrit;
    CRITICAL_SECTION    csCritRead;
    CRITICAL_SECTION    csCritWrite;

	Device_State		eu_DeviceState;
	DWORD				dwEventMask;
	DWORD			    dwOpenCount;
	LAN_DEV_ADAPTOR_FUNC	Dev_Func;	//The table of the functions supplied by the PDD 
	Ether_Addr			ea_Address;
	WORD				wReserved0;

	HANDLE				hInterruptEvent;	//Event used with dwEventMask for WaitLanEvent.
	HANDLE				hInterrupteHandler;
	BOOL				bIntHandlerContinue; //Set it to FASLE, to stop the interrupt handler.
	HANDLE				hNDIS;	//Event used with dwEventMask for WaitLanEvent.

}LAN_DEVICE_MDD, *PLAN_DEVICE_MDD;

#endif
