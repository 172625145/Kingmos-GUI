//
// Copyright (c) Esoft Corporation.  All rights reserved.
//
//
//
/*-------
Module Name:    cardserv.h
 
Description:	  


Notes:

Update date:	2002/10/14

--*/

#ifndef		_ESOFT_CARDSERVER_H
#define		_ESOFT_CARDSERVER_H		1

#ifdef	__cplusplus
extern "C" {
#endif

//**********************************************************************
// Card Services Event Codes.
//**********************************************************************

//
// @type CARD_EVENT | Event code passed to the client driver's callback function
//                    to indicate the type of PCMCIA event.
//
typedef UINT32 CARD_EVENT;
typedef UINT32* PUINT32; 
#define ESF_BATTERY_DEAD          0x01
#define ESF_BATTERY_LOW           0x02
#define ESF_CARD_LOCK             0x03
#define ESF_CARD_READY            0x04
#define ESF_CARD_REMOVAL          0x05
#define ESF_CARD_UNLOCK           0x06
#define ESF_EJECTION_COMPLETE     0x07
#define ESF_EJECTION_REQUEST      0x08
#define ESF_INSERTION_COMPLETE    0x09
#define ESF_INSERTION_REQUEST     0x0A
#define ESF_PM_RESUME             0x0B    // Power management
#define ESF_PM_SUSPEND            0x0C    // Power management
#define ESF_EXCLUSIVE_COMPLETE    0x0D
#define ESF_EXCLUSIVE_REQUEST     0x0E
#define ESF_RESET_PHYSICAL        0x0F
#define ESF_RESET_REQUEST         0x10
#define ESF_CARD_RESET            0x11
#define ESF_MTD_REQUEST           0x12
#define ESF_CLIENT_INFO           0x14
#define ESF_TIMER_EXPIRED         0x15
#define ESF_SS_UPDATED            0x16
#define ESF_WRITE_PROTECT         0x17
#define ESF_CARD_INSERTION        0x40
#define ESF_RESET_COMPLETE        0x80
#define ESF_ERASE_COMPLETE        0x81
#define ESF_REGISTRATION_COMPLETE 0x82
#define ESF_STATUS_CHANGE_INTERRUPT 0xFE   // The card generated a status change interrupt
#define ESF_LOAD_CLIENT			0x83
#define ESF_UNLOAD_CLIENT		0x84
//**********************************************************************
// Card Services Return Codes.
//**********************************************************************

//
// @type STATUS | Set of return codes used by Card Services
//
typedef UINT32 STATUS;

#define CARDERR_SUCCESS              0x00
#define CARDERR_BAD_ADAPTER          0x01
#define CARDERR_BAD_ATTRIBUTE        0x02
#define CARDERR_BAD_BASE             0x03
#define CARDERR_BAD_EDC              0x04
#define CARDERR_BAD_IRQ              0x06
#define CARDERR_BAD_OFFSET           0x07
#define CARDERR_BAD_PAGE             0x08
#define CARDERR_READ_FAILURE         0x09
#define CARDERR_BAD_SIZE             0x0A
#define CARDERR_BAD_SOCKET           0x0B
#define CARDERR_BAD_TYPE             0x0D
#define CARDERR_BAD_VCC              0x0E
#define CARDERR_BAD_VPP              0x0F
#define CARDERR_BAD_WINDOW           0x11
#define CARDERR_WRITE_FAILURE        0x12
#define CARDERR_NO_CARD              0x14
#define CARDERR_UNSUPPORTED_SERVICE  0x15
#define CARDERR_UNSUPPORTED_MODE     0x16
#define CARDERR_BAD_SPEED            0x17
#define CARDERR_BUSY                 0x18
#define CARDERR_GENERAL_FAILURE      0x19
#define CARDERR_WRITE_PROTECTED      0x1A
#define CARDERR_BAD_ARG_LENGTH       0x1B
#define CARDERR_BAD_ARGS             0x1C
#define CARDERR_CONFIGURATION_LOCKED 0x1D
#define CARDERR_IN_USE               0x1E
#define CARDERR_NO_MORE_ITEMS        0x1F
#define CARDERR_OUT_OF_RESOURCE      0x20
#define CARDERR_BAD_HANDLE           0x21
#define CARDERR_BAD_VERSION          0x22
#define CARDERR_EVENT_NONE			 0x23
#define CARDERR_EVENT_EXIST			 0x24
//*********************************************************************
// Card Services Interface Structures.
// (Must be packed to one byte to guarantee conformance to the
//  card services specification).
//*********************************************************************/
#pragma pack(1)

typedef DWORD CARD_CLIENT_HANDLE;
typedef PVOID CARD_WINDOW_HANDLE;

//*****************************************************************
// Socket handle (formed by concatenating the 0 based socket number
// and the 0 based function number)
//*****************************************************************/
//
// @struct CARD_SOCKET_HANDLE | Structure used to specify a logical socket
//                              and is composed of a socket number and a function number
//
typedef struct _SOCKET_HANDLE{
	short nSocket;		//@indicate fix socket;
	short nFunction;	//@indicate function socket;
}CARD_SOCKET_HANDLE, *PCARD_SOCKET_HANDLE;


//*****************************************************************
// Event Mask Structures.
//*****************************************************************/
#define EVENT_ATTR_SOCKET_ONLY        0x0001    // 0 for global event mask
//
// Event masks used in calls to CardRequestSocketMask, CardGetEventMask,
// CardSetEventMask and CardRegisterClient
//
// CARD_REGISTER_PARMS.fEventMask
// CARD_EVENT_MASK_PARMS.fEventMask
// CardRequestSocketMask(fEventMask)
// CARD_STATUS.fCardState
// CARD_STATUS.fSocketState
// (PCARD_SOCKET_INFO.fInterruptEvents)
// (PCARD_SOCKET_INFO.fNotifyEvents)
//
#define EVENT_MASK_WRITE_PROTECT    0x0001 // write protect change
#define EVENT_MASK_CARD_LOCK        0x0002 // card lock change
#define EVENT_MASK_EJECT_REQ        0x0004 // ejection request
#define EVENT_MASK_INSERT_REQ       0x0008 // insertion request
#define EVENT_MASK_BATTERY_DEAD     0x0010 // battery dead
#define EVENT_MASK_BATTERY_LOW      0x0020 // battery low
#define EVENT_MASK_CARD_READY       0x0040 // ready change
#define EVENT_MASK_CARD_DETECT      0x0080 // card detect change
#define EVENT_MASK_POWER_MGMT       0x0100 // power management change
#define EVENT_MASK_RESET            0x0200 // card resets
#define EVENT_MASK_STATUS_CHANGE    0x0400 // card generated status change interrupts
//
// @struct CARD_EVENT_MASK_PARMS | Structure passed to <f CardGetEventMask> and <f CardSetEventMask>
// 
typedef struct _CARD_EVENT_MASK_PARMS{
    CARD_SOCKET_HANDLE    hSocket;      // @field Logical socket (socket/function)
    UINT16  fAttributes;                // @field First bit clear indicates the global event mask
                                        //        First bit set indicates the socket specific event mask
    UINT16  fEventMask;                 // @field Input for CardSetEventMask, Output for CardGetEventMask
}CARD_EVENT_MASK_PARMS, *PCARD_EVENT_MASK_PARMS;


//******************************************************************
// Callback function parameter block
//******************************************************************
//
// @struct CARD_EVENT_PARMS | Structure passed to the client driver's callback function
//
typedef struct _CARD_EVENT_PARMS {
    UINT32 uClientData;             // @field Context data specified in CardRegisterClient
    UINT32 Parm1;                   // @field Event code specific parameter one
    UINT32 Parm2;                   // @field Event code specific parameter two
} CARD_EVENT_PARMS, *PCARD_EVENT_PARMS;
typedef STATUS (*CLIENT_CALLBACK)(CARD_EVENT, CARD_SOCKET_HANDLE, PCARD_EVENT_PARMS);
typedef CLIENT_CALLBACK *PCLIENT_CALLBACK;
//
// Client driver interrupt service routine prototype
//
typedef VOID (*CARD_ISR)(UINT32);
typedef CARD_ISR *PCARD_ISR;


//******************************************************************
// Register Client Flags
//******************************************************************
//
// CARD_REGISTER_PARMS.fAttributes
//
#define CLIENT_ATTR_MEM_DRIVER       0x0001   // memory client driver
#define CLIENT_ATTR_MTD_DRIVER       0x0002   // memory technology driver
#define CLIENT_ATTR_IO_DRIVER        0x0004   // i/o client driver
#define CLIENT_ATTR_NOTIFY_SHARED    0x0008   // sharable PC cards
#define CLIENT_ATTR_NOTIFY_EXCLUSIVE 0x0010   // exclusively used PC cards
//
// @struct CARD_REGISTER_PARMS | Structure used to indicate client driver characteristics to
//                               the <f CardRegisterClient> API
//
typedef struct _CARD_REGISTER_PARMS {
    UINT16 fAttributes;         // @field Type of driver
    UINT16 fEventMask;          // @field Global event mask
    UINT32 uClientData;         // @field Context data for the callback function
}CARD_REGISTER_PARMS,*PCARD_REGISTER_PARMS;

//*****************************************************************
// Window packet structures.
//****************************************************************

//
// Window attribute masks used in calls to
// CardRequestWindow and CardModifyWindow
//
// Usage:
// CARD_WINDOW_PARMS.fAttributes
// CardModifyWindow(fAttributes)
//
#define WIN_ATTR_IO_SPACE      0x0001        // 1 = I/O window
                                             // 0 = memory window
#define WIN_ATTR_ATTRIBUTE     0x0002        // 1 = attribute, 0 = common
#define WIN_ATTR_ENABLED       0x0004        // 1 = window is enabled
                                             // 0 = window is disabled
#define WIN_ATTR_16BIT         0x0008        // 1 = 16 bit, 0 = 8 bit
#define WIN_ATTR_PAGED         0x0010        // 1 = size is 16K multiple
#define WIN_ATTR_SHARED        0x0020        // 1 = shared memory request
#define WIN_ATTR_FIRST_SHARED  0x0040        // 1 = first shared request
#define WIN_ATTR_OFFSETS_SIZED 0x0100        // card offsets window sized
#define WIN_ATTR_ACCESS_SPEED_VALID 0x0200   // Used to cause CardModifyWindow 
                                             // to change the window's access speed

//
// Memory window speed encodings
//
// Usage:
// CARD_WINDOW_PARMS.fAccessSpeed
// CardModifyWindow(fAccessSpeed)
// (PDCARD_WINDOW_INFO.fSlowest)
// (PDCARD_WINDOW_INFO.fFastest)
// (PDCARD_WINDOW_STATE.fSpeed)
//
#define WIN_SPEED_EXP_MASK  0x07    // Exponent mask
#define WIN_SPEED_EXP_1NS   0x00    // 1 nanosecond
#define WIN_SPEED_EXP_10NS  0x01
#define WIN_SPEED_EXP_100NS 0x02
#define WIN_SPEED_EXP_1US   0x03    // 1 microsecond
#define WIN_SPEED_EXP_10US  0x04
#define WIN_SPEED_EXP_100US 0x05
#define WIN_SPEED_EXP_1MS   0x06    // 1 millisecond
#define WIN_SPEED_EXP_10MS  0x07

#define WIN_SPEED_MANT_MASK 0x78    // Mantissa mask
#define WIN_SPEED_MANT_10   0x08    // 1.0
#define WIN_SPEED_MANT_12   0x10    // 1.2      
#define WIN_SPEED_MANT_13   0x18    // 1.3
#define WIN_SPEED_MANT_15   0x20    // 1.5
#define WIN_SPEED_MANT_20   0x28    // 2.0
#define WIN_SPEED_MANT_25   0x30    // 2.5
#define WIN_SPEED_MANT_30   0x38    // 3.0
#define WIN_SPEED_MANT_35   0x40    // 3.5
#define WIN_SPEED_MANT_40   0x48    // 4.0
#define WIN_SPEED_MANT_45   0x50    // 4.5
#define WIN_SPEED_MANT_50   0x58    // 5.0
#define WIN_SPEED_MANT_55   0x60    // 5.5
#define WIN_SPEED_MANT_60   0x68    // 6.0
#define WIN_SPEED_MANT_70   0x70    // 7.0
#define WIN_SPEED_MANT_80   0x78    // 8.0

#define WIN_SPEED_USE_WAIT  0x80    // Use wait if available

//
// @struct CARD_WINDOW_PARMS | Structure to specify the desired memory or I/O window characteristics
//                             to <f CardRequestWindow>
//
typedef struct _CARD_WINDOW_PARMS {
    CARD_SOCKET_HANDLE hSocket;     // @field Socket to be associated with the window
	UINT8  fAccessSpeed;            // @field Bit encoded memory access speed
	UINT8  uReserve;
    UINT16 fAttributes;             // @field Bit encoded window attributes
    UINT32 uWindowSize;             // @field Desired window size in bytes
    
}CARD_WINDOW_PARMS, *PCARD_WINDOW_PARMS;


//*****************************************************************
// Configuration packet structures.
//****************************************************************

//
// CARD_CONFIG_INFO.fAttributes
//
#define CFG_ATTR_EXCLUSIVE    0x0001
#define CFG_ATTR_IRQ_STEERING 0x0002    // Enable interrupts from PC card to host
#define CFG_ATTR_IRQ_WAKEUP   0x0004    // Allow PCMCIA ints to wake system from
                                        // suspend mode. 
#define CFG_ATTR_KEEP_POWERED 0x0008    // Keep the PCMCIA bus powered while the
                                        // system is in the suspended state.
#define CFG_ATTR_ENABLE_DMA   0x0040
#define CFG_ATTR_VALID_CLIENT 0x0100    
#define CFG_ATTR_VS_OVERRIDE  0x0200
#define CFG_ATTR_NO_IO_IS_8   0x0400

//
// CARD_CONFIG_INFO.fInterfaceType
// (PDCARD_SOCKET_STATE.fInterfaceType)
//
#define CFG_IFACE_MEMORY    0x0001  // Put socket controller in memory only interface mode
#define CFG_IFACE_MEMORY_IO 0x0002  // Put socket controller in memory and I/O interface mode

//
// CARD_CONFIG_INFO.fRegisters
//
// Each of these bits indicates the presence one of the standard function and
// configuration registers in the attribute space of the PC card.  Using the API
// CardRequestConfiguration, a driver can set these registers to desired initial
// values.  The fRegisters field indicates which registers to write.
//
#define CFG_REGISTER_CONFIG   0x01  // Configuration Option Register (uConfigReg)
#define CFG_REGISTER_STATUS   0x02  // Function Configuration and Status Register (uStatusReg)
#define CFG_REGISTER_PIN      0x04  // Pin Replacement Register (uPinReg)
#define CFG_REGISTER_COPY     0x08  // Socket and Copy Register (uCopyReg)
#define CFG_REGISTER_EXSTATUS 0x10  // Extended Status register (uExtendedStatus)
#define CFG_REGISTER_EXREG    0x20  // Extended registers present

// Extended register presence bits

#define CFG_EXREGISTER_IOBASE0      0x0001
#define CFG_EXREGISTER_IOBASE1      0x0002
#define CFG_EXREGISTER_IOBASE2      0x0004
#define CFG_EXREGISTER_IOBASE3      0x0008
#define CFG_EXREGISTER_IOLIMIT      0x0010

//
// @struct CARD_CONFIG_INFO | Structure used to indicate desired PC card configuration parameters
//                            to the <f CardRequestConfiguration> API
//  
typedef struct _CARD_CONFIG_INFO {
    CARD_SOCKET_HANDLE  hSocket;// @field Logical socket (socket/function)
    UINT16 fAttributes;         // @field Bit encoded socket attributes
    UINT8  fInterfaceType;      // @field Bit encoded PC card interface type (memory or I/O)
    UINT8  uVcc;                // @field Vcc voltage to use
    UINT8  uVpp1;               // @field Vpp1 voltage to use
    UINT8  uVpp2;               // @field Vpp2 voltage to use
    UINT8  fRegMask;          // @field Register presence mask
    UINT8  uConfigReg;          // @field Initial value to write to the configuration option register
    UINT8  uStatusReg;          // @field Initial value to write to the function status register
    UINT8  uPinReg;             // @field Initial value to write to the pin replacement register
    UINT8  uCopyReg;            // @field Initial value to write to the socket and copy register
    UINT8  uExtendedStatus;     // @field Initial value to write to the extended status register
    UINT16 fExtRegMask;       // @field Extended register presence mask
    UINT8  IOBase[4];           // @field Initial value to write to the I/O Base registers
    UINT8  IOLimit;             // @field Initial value to write to the I/O Limit register
}CARD_CONFIG_INFO, *PCARD_CONFIG_INFO;

//
// Function Configuration Register offsets
//
#define FCR_OFFSET_COR   0   // Configuration Option Register
#define FCR_OFFSET_FCSR  1   // Function and Configuration Status Register
#define FCR_OFFSET_PRR   2   // Pin Replacement Register
#define FCR_OFFSET_SCR   3   // Socket and Copy Register
#define FCR_OFFSET_ESR   4   // Extended Status Register
#define FCR_OFFSET_IOB0  5   // I/O Base Register 0
#define FCR_OFFSET_IOB1  6   // I/O Base Register 1
#define FCR_OFFSET_IOB2  7   // I/O Base Register 2
#define FCR_OFFSET_IOB3  8   // I/O Base Register 3
#define FCR_OFFSET_IOSZ  9   // I/O Size Register

//
// Configuration Option Register Bit Masks
// CARD_CONFIG_INFO.uConfigReg
//
// Note: The lower 6 bits of the configuration option register are used to
//       specify which configuration number to use for single function cards.
//
#define FCR_COR_LEVEL_IREQ  0x40    // 1 = level mode interrupts, 0 = pulse mode
#define FCR_COR_SRESET      0x80    // 1 = soft reset

//
// Function and Configuration Status Register Bit Masks
// CARD_CONFIG_INFO.uStatusReg
//
#define FCR_FCSR_INTR_ACK   0x01    // interrupt acknowledge
#define FCR_FCSR_INTR       0x02    // interrupt pending
#define FCR_FCSR_PWR_DOWN   0x04    // place PC card in power down mode
#define FCR_FCSR_AUDIO      0x08    // enable audio signal on BVD2 (pin 62)
#define FCR_FCSR_IO_IS_8    0x20    // used by host to indicate 8 bit only I/O
#define FCR_FCSR_STSCHG     0x40    // Enable status change (STSCHG, pin 63) from PC card
#define FCR_FCSR_CHANGED    0x80    // Set if one of the status changed bits is set in
                                    // the pin replacement register.

#define FCR_FCSR_REQUIRED_BITS (FCR_FCSR_INTR_ACK|FCR_FCSR_IO_IS_8)

//
// CARD_CONFIG_INFO.uPinReg
//
#define FCR_PRR_RWP         0x01    // Current state of write protect
#define FCR_PRR_RREADY      0x02    // Current state of Ready
#define FCR_PRR_RBVD2       0x04    // Current state of BV2
#define FCR_PRR_RBVD1       0x08    // Current state of BV1
#define FCR_PRR_CWP         0x10    // Write protect changed
#define FCR_PRR_CRDY        0x20    // Ready changed
#define FCR_PRR_CBVD2       0x40    // BVD2 changed
#define FCR_PRR_CBVD1       0x80    // BVD1 changed


//
//  Used for CardAccessConfigurationRegister function
//
#define CARD_FCR_READ  0
#define CARD_FCR_WRITE 1

//*****************************************************************
// CardGetStatus structure
//*****************************************************************

//
// @struct CARD_STATUS | Structure used by <f CardGetStatus>
//
typedef struct _CARD_STATUS {
    CARD_SOCKET_HANDLE  hSocket;    // @field Logical socket (socket/function)
    UINT16  fCardState;             // @field current card state
    UINT16  fSocketState;           // @field changed socket state
} CARD_STATUS, *PCARD_STATUS;


//*****************************************************************
// Tuple request structures.
// NOTE:
//         - The tuple flags structure is only used internally by card
//           services and is defined as required by an implementation.
//         - A variable length tuple data area immediately follows
//           the tuple data packet structure.
//****************************************************************

#define		PARSED_TYPE_CONFIG		0x01
#define		PARSED_TYPE_CFTABLE		0x02

//
// @struct CARD_TUPLE_PARMS | Structure passed to <f CardGetFirstTuple> and used in
//                            subsequent calls to <f CardGetNextTuple>.
//                            
typedef struct _CARD_TUPLE_PARMS {
    UINT8  uDesiredTuple;       // @field Tuple code of desired tuple or 0xFF for any tuple
    UINT8  uTupleCode;          // @field Output: Tuple code found
    UINT8  uTupleLink;          // @field Output: Link field (length) of tuple found
	UINT8  uReserve;
}CARD_TUPLE_PARMS, *PCARD_TUPLE_PARMS;

//
// @struct CARD_DATA_PARMS | Structure passed to <f CardGetTupleData>.  With care,
//               one region of memory can be used as both a <t CARD_TUPLE_PARMS> and
//               a CARD_DATA_PARMS structure.
//
typedef struct _CARD_DATA_PARMS {
    UINT8  uTupleOffset;        // @field Offset in the current tuple to retrieve data
	UINT8  uReserve;
    UINT16 uBufLen;             // @field Input: number of bytes following this CARD_DATA_PARMS structure
    UINT16 uDataLen;            // @field Output: number of bytes of data returned
	PVOID  pBuf;				// Input: store tuple data 
} CARD_TUPLE_DATA, *PCARD_TUPLE_DATA;

//the struct don't require client to changed
typedef struct _CARD_TUPLE_OFFSET {
    UINT16 fFlags;              // @field Used by Card Services, value must be preserved
//	PUSER_WINDOW  pWin;				// pointer to CIS begin addrress 
	UINT16 uReserve;
	struct _USER_WINDOW *pWin;
    UINT32 uLinkOffset;         // @field Used by Card Services, value must be preserved
    UINT32 uCISOffset;          // @field Used by Card Services, value must be preserved
}CARD_TUPLE_OFFSET, *PCARD_TUPLE_OFFSET;


#pragma pack()   // return packing to normal



//
// Card Services Function Prototypes
//
//
// Card Services Function Prototypes
//
CARD_CLIENT_HANDLE
CardRegisterClient(
	CARD_SOCKET_HANDLE hSocket,
	CLIENT_CALLBACK		CallBackFn, 
	PCARD_REGISTER_PARMS pParms,
	DWORD *pRet
	);

STATUS CardDeregisterClient(
		CARD_CLIENT_HANDLE hClient
);

STATUS CardGetFirstTuple(
	    CARD_SOCKET_HANDLE	hSocket,	//Socket's handle	
		UINT8				fLink,		//see if return Link Tuple
		PCARD_TUPLE_PARMS   pTuple,		// @parm Pointer to a <t CARD_TUPLE_PARMS> structure.
		PCARD_TUPLE_OFFSET	pOffset
);	//Tuple's offset at CIS

STATUS CardGetNextTuple(
		CARD_SOCKET_HANDLE  hSocket,	//Socket's handle	
		UINT8				fLink,		//see if return Link Tuple
		PCARD_TUPLE_PARMS	pTuple,		//Pointer to a CARD_TUPLE_PARMS structure.
		PCARD_TUPLE_OFFSET	pOffset);	//Tuple's offset at CIS

STATUS CardGetTupleData(
		CARD_SOCKET_HANDLE	hSocket,	//Socket's handle	
		PCARD_TUPLE_DATA    pData,      //Pointer to a CARD_TUPLE_PARMS structure.
		PCARD_TUPLE_OFFSET	pOffset);	//Tuple's offset at CIS 
    

STATUS CardGetParsedTuple(
		CARD_SOCKET_HANDLE	hSocket,	//Socket/function identifier
		UINT8				TupleType,	//request tuple to parse (only CISTPL_CONFIG and CISTPL_CFTABLE_ENTRY for now)
		PVOID				pBuf,       //Pointer to buffer to receive parsed data
		PUINT32				pNumItems);  //Pointer to number of items in pBuf.

STATUS CardRequestExclusive(
		CARD_CLIENT_HANDLE hClient,
		CARD_SOCKET_HANDLE	hSocket
);

STATUS CardReleaseExclusive(
	CARD_CLIENT_HANDLE hClient,
	CARD_SOCKET_HANDLE	hSocket
);

STATUS CardRequestSocketMask(
		CARD_CLIENT_HANDLE hCardClient,
        CARD_SOCKET_HANDLE	hSock, 
		UINT16				fEventMask
);

STATUS CardReleaseSocketMask(
		CARD_CLIENT_HANDLE hCardClient,
		CARD_SOCKET_HANDLE	hSocket
);

STATUS CardGetEventMask(
		CARD_CLIENT_HANDLE hCardClient, 
		PCARD_EVENT_MASK_PARMS pMaskParms
);

STATUS CardSetEventMask(
		CARD_CLIENT_HANDLE hCardClient, 
		PCARD_EVENT_MASK_PARMS pMaskParms
);

STATUS CardResetFunction(
		CARD_CLIENT_HANDLE hCardClient,
		CARD_SOCKET_HANDLE	hSock
);

CARD_WINDOW_HANDLE CardRequestWindow(
		CARD_CLIENT_HANDLE hCardClient, 
		PCARD_WINDOW_PARMS	pCardWinParms,
		DWORD *pdwRetStatus
);

STATUS CardReleaseWindow(
		CARD_WINDOW_HANDLE	hCardWin
);   

STATUS CardModifyWindow(
		CARD_WINDOW_HANDLE	hCardWindow, 
		UINT16				fAttributes, 
		UINT8				fAccessSpeed);

PVOID CardMapWindow(
		CARD_WINDOW_HANDLE	hCardWindow, 
		UINT32				uCardAddress, 
		UINT32				uSize, 
		PUINT32				pGranularity,
		DWORD *pdwRetStatus
);

STATUS CardGetStatus(
		PCARD_STATUS		pStatus
);

STATUS CardRequestIRQ(
		CARD_CLIENT_HANDLE	hCardClient, 
		CARD_SOCKET_HANDLE	hSocket,
        CARD_ISR			ISRFunction, 
		UINT32				uISRContextData
);

STATUS CardReleaseIRQ(
		CARD_CLIENT_HANDLE	hCardClient,
		CARD_SOCKET_HANDLE	hSocket
);

STATUS CardRequestConfiguration(
		CARD_CLIENT_HANDLE	hCardClient,
		PCARD_CONFIG_INFO	pParms
);

STATUS CardReleaseConfiguration(
		CARD_CLIENT_HANDLE	hCardClient,
		CARD_SOCKET_HANDLE	hSock
);

STATUS CardAccessConfigurationRegister(
		CARD_CLIENT_HANDLE	hCardClient,
        CARD_SOCKET_HANDLE	hSock,
		UINT8				rw_flag,                                       
		UINT8				offset,
		UINT8				*pValue
);

STATUS CardPowerOn(
		CARD_CLIENT_HANDLE	hCardClient,
		CARD_SOCKET_HANDLE	hSock
);

STATUS CardPowerOff(
		CARD_SOCKET_HANDLE	hSock);

#ifdef __cplusplus
}
#endif


#endif	//#ifndef
