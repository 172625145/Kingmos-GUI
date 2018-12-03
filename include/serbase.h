/**************************************************************************
* Copyright (c)  微逻辑(WEILUOJI). All rights reserved.                   *
**************************************************************************/

#ifndef __SER_BASE_H
#define __SER_BASE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <eDevice.h>

//---------------------------------------------
//DCB 成员功能宏
// DTR  --- DCB.fDtrControl
#define DTR_CONTROL_DISABLE    0x00
#define DTR_CONTROL_ENABLE     0x01
#define DTR_CONTROL_HANDSHAKE  0x02
// RTS  --- DCB.fRtsControl
#define RTS_CONTROL_DISABLE    0x00
#define RTS_CONTROL_ENABLE     0x01
#define RTS_CONTROL_HANDSHAKE  0x02
#define RTS_CONTROL_TOGGLE     0x03
//DCB.Parity, and DCB.StopBits
#define NOPARITY            0
#define ODDPARITY           1
#define EVENPARITY          2
#define MARKPARITY          3
#define SPACEPARITY         4

#define ONESTOPBIT          0
#define ONE5STOPBITS        1
#define TWOSTOPBITS         2
#define IGNORE              0       // Ignore signal
// DCB.BaudRate
#define CBR_110             110
#define CBR_300             300
#define CBR_600             600
#define CBR_1200            1200
#define CBR_2400            2400
#define CBR_4800            4800
#define CBR_9600            9600
#define CBR_14400           14400
#define CBR_19200           19200
#define CBR_38400           38400
#define CBR_56000           56000
#define CBR_57600           57600
#define CBR_115200          115200
#define CBR_128000          128000
#define CBR_256000          256000

//DCB 结构定义
typedef struct _DCB 
{
	DWORD	DCBlength;
	DWORD	BaudRate; 
	DWORD	fBinary:1;
	DWORD	fParity:1;
	DWORD	fOutxCtsFlow:1;
	DWORD	fOutxDsrFlow:1;
	DWORD	fDtrControl:2;
	DWORD	fDsrSensitivity:1;
	DWORD	fTXContinueOnXoff:1;
	DWORD	fOutX: 1;
	DWORD	fInX: 1;
	DWORD	fErrorChar:1;
	DWORD	fNull:1; 
	DWORD	fRtsControl:2; 
	DWORD	fAbortOnError:1;
	DWORD	fDummy2:17;
	WORD wReserved;
	WORD XonLim; 
	WORD XoffLim; 
	BYTE ByteSize; 
	BYTE Parity; 
	BYTE StopBits;
	char XonChar; 
	char XoffChar; 
	char ErrorChar;
	char EofChar; 
	char EvtChar; 
	WORD wReserved1; 

} *PDCB, *LPDCB, DCB; 

//---------------------------------------------
//COMSTAT 结构定义
typedef struct _COMSTAT
{
	DWORD fCtsHold : 1;
	DWORD fDsrHold : 1;
	DWORD fRlsdHold : 1;
	DWORD fXoffHold : 1;
	DWORD fXoffSent : 1;
	DWORD fEof : 1;
	DWORD fTxim : 1;
	DWORD fReserved : 25;
	DWORD cbInQue;
	DWORD cbOutQue;

} COMSTAT, *LPCOMSTAT;

//---------------------------------------------
//COMMPROP 成员功能宏
//COMMPROP.dwSettableBaud
#define BAUD_075          ((DWORD)0x00000001)
#define BAUD_110          ((DWORD)0x00000002)
#define BAUD_134_5        ((DWORD)0x00000004)
#define BAUD_150          ((DWORD)0x00000008)
#define BAUD_300          ((DWORD)0x00000010)
#define BAUD_600          ((DWORD)0x00000020)
#define BAUD_1200         ((DWORD)0x00000040)
#define BAUD_1800         ((DWORD)0x00000080)
#define BAUD_2400         ((DWORD)0x00000100)
#define BAUD_4800         ((DWORD)0x00000200)
#define BAUD_7200         ((DWORD)0x00000400)
#define BAUD_9600         ((DWORD)0x00000800)
#define BAUD_14400        ((DWORD)0x00001000)
#define BAUD_19200        ((DWORD)0x00002000)
#define BAUD_38400        ((DWORD)0x00004000)
#define BAUD_56K          ((DWORD)0x00008000)
#define BAUD_128K         ((DWORD)0x00010000)
#define BAUD_115200       ((DWORD)0x00020000)
#define BAUD_57600        ((DWORD)0x00040000)
#define BAUD_USER         ((DWORD)0x10000000)
//COMMPROP.dwServiceMask
#define SP_SERIALCOMM     ((DWORD)0x00000001)
//COMMPROP.dwProvSubType
#define PST_UNSPECIFIED      ((DWORD)0x00000000)
#define PST_RS232            ((DWORD)0x00000001)
#define PST_PARALLELPORT     ((DWORD)0x00000002)
#define PST_RS422            ((DWORD)0x00000003)
#define PST_RS423            ((DWORD)0x00000004)
#define PST_RS449            ((DWORD)0x00000005)
#define PST_MODEM            ((DWORD)0x00000006)
#define PST_FAX              ((DWORD)0x00000021)
#define PST_SCANNER          ((DWORD)0x00000022)
#define PST_NETWORK_BRIDGE   ((DWORD)0x00000100)
#define PST_LAT              ((DWORD)0x00000101)
#define PST_TCPIP_TELNET     ((DWORD)0x00000102)
#define PST_X25              ((DWORD)0x00000103)
//COMMPROP.dwProvCapabilities
#define PCF_DTRDSR        ((DWORD)0x0001)
#define PCF_RTSCTS        ((DWORD)0x0002)
#define PCF_RLSD          ((DWORD)0x0004)
#define PCF_PARITY_CHECK  ((DWORD)0x0008)
#define PCF_XONXOFF       ((DWORD)0x0010)
#define PCF_SETXCHAR      ((DWORD)0x0020)
#define PCF_TOTALTIMEOUTS ((DWORD)0x0040)
#define PCF_INTTIMEOUTS   ((DWORD)0x0080)
#define PCF_SPECIALCHARS  ((DWORD)0x0100)
#define PCF_16BITMODE     ((DWORD)0x0200)
//COMMPROP.dwSettableParams
#define SP_PARITY         ((DWORD)0x0001)
#define SP_BAUD           ((DWORD)0x0002)
#define SP_DATABITS       ((DWORD)0x0004)
#define SP_STOPBITS       ((DWORD)0x0008)
#define SP_HANDSHAKING    ((DWORD)0x0010)
#define SP_PARITY_CHECK   ((DWORD)0x0020)
#define SP_RLSD           ((DWORD)0x0040)
//COMMPROP.wSettableData
#define DATABITS_5        ((WORD)0x0001)
#define DATABITS_6        ((WORD)0x0002)
#define DATABITS_7        ((WORD)0x0004)
#define DATABITS_8        ((WORD)0x0008)
#define DATABITS_16       ((WORD)0x0010)
#define DATABITS_16X      ((WORD)0x0020)
//COMMPROP.wSettableStopParity
#define STOPBITS_10       ((WORD)0x0001)
#define STOPBITS_15       ((WORD)0x0002)
#define STOPBITS_20       ((WORD)0x0004)
#define PARITY_NONE       ((WORD)0x0100)
#define PARITY_ODD        ((WORD)0x0200)
#define PARITY_EVEN       ((WORD)0x0400)
#define PARITY_MARK       ((WORD)0x0800)
#define PARITY_SPACE      ((WORD)0x1000)

//COMMPROP 结构定义
typedef struct _COMMPROP
{
	WORD wPacketLength;
	WORD wPacketVersion;
	DWORD dwServiceMask;
	DWORD dwReserved1;
	DWORD dwMaxTxQueue;
	DWORD dwMaxRxQueue;
	DWORD dwMaxBaud;
	DWORD dwProvSubType;
	DWORD dwProvCapabilities;
	DWORD dwSettableParams;
	DWORD dwSettableBaud;
	WORD wSettableData;
	WORD wSettableStopParity;
	WORD dwCurrentTxQueue;
	DWORD dwCurrentRxQueue;
	DWORD dwProvSpec1;
	DWORD dwProvSpec2;
	CHAR wcProvChar[1];//xxx

} *LPCOMMPROP, COMMPROP;

//---------------------------------------------
//COMMTIMEOUTS 结构定义
typedef struct _COMMTIMEOUTS
{ 
	DWORD ReadIntervalTimeout; 
	DWORD ReadTotalTimeoutMultiplier; 
	DWORD ReadTotalTimeoutConstant; 
	DWORD WriteTotalTimeoutMultiplier; 
	DWORD WriteTotalTimeoutConstant; 

} COMMTIMEOUTS,*LPCOMMTIMEOUTS; 

//---------------------------------------------
// ClearCommError's error value
#define CE_RXOVER           0x0001
#define CE_OVERRUN          0x0002
#define CE_RXPARITY         0x0004
#define CE_FRAME            0x0008
#define CE_BREAK            0x0010
#define CE_TXFULL           0x0100
#define CE_PTO              0x0200
#define CE_IOE              0x0400
#define CE_DNS              0x0800
#define CE_OOP              0x1000
#define CE_MODE             0x8000

#define IE_BADID            (-1)    
#define IE_OPEN             (-2)    
#define IE_NOPEN            (-3)    
#define IE_MEMORY           (-4)    
#define IE_DEFAULT          (-5)    
#define IE_HARDWARE         (-10)   
#define IE_BYTESIZE         (-11)   
#define IE_BAUDRATE         (-12)   

//SetCommMask's evtmask or WaitCommEvent's Events
#define EV_RXCHAR           0x0001
#define EV_RXFLAG           0x0002
#define EV_TXEMPTY          0x0004
#define EV_CTS              0x0008
#define EV_DSR              0x0010
#define EV_RLSD             0x0020
#define EV_BREAK            0x0040
#define EV_ERR              0x0080
#define EV_RING             0x0100
#define EV_PERR             0x0200
#define EV_RX80FULL         0x0400
#define EV_EVENT1           0x0800
#define EV_EVENT2           0x1000
#define EV_POWER			0x2000

// EscapeCommFunction's function
#define SETXOFF             1       
#define SETXON              2       
#define SETRTS              3       
#define CLRRTS              4       
#define SETDTR              5       
#define CLRDTR              6       
#define SETBREAK            8       
#define CLRBREAK            9       
#define SETIR               10      
#define CLRIR               11      

// PurgeComm
#define PURGE_TXABORT       0x0001  
#define PURGE_RXABORT       0x0002  
#define PURGE_TXCLEAR       0x0004  
#define PURGE_RXCLEAR       0x0008  

#define LPTx                0x80    

// GetModemStatus's status
#define MS_CTS_ON           ((DWORD)0x0010)
#define MS_DSR_ON           ((DWORD)0x0020)
#define MS_RING_ON          ((DWORD)0x0040)
#define MS_RLSD_ON          ((DWORD)0x0080)


//---------------------------------------------
//
//
#define IOCTL_SERIAL_SET_BREAK_ON       CTL_CODE(FILE_DEVICE_SERIAL_PORT,  1, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SERIAL_SET_BREAK_OFF      CTL_CODE(FILE_DEVICE_SERIAL_PORT,  2, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SERIAL_SET_DTR            CTL_CODE(FILE_DEVICE_SERIAL_PORT,  3, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SERIAL_CLR_DTR            CTL_CODE(FILE_DEVICE_SERIAL_PORT,  4, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SERIAL_SET_RTS            CTL_CODE(FILE_DEVICE_SERIAL_PORT,  5, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SERIAL_CLR_RTS            CTL_CODE(FILE_DEVICE_SERIAL_PORT,  6, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SERIAL_SET_XOFF           CTL_CODE(FILE_DEVICE_SERIAL_PORT,  7, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SERIAL_SET_XON            CTL_CODE(FILE_DEVICE_SERIAL_PORT,  8, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SERIAL_GET_WAIT_MASK      CTL_CODE(FILE_DEVICE_SERIAL_PORT,  9, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SERIAL_SET_WAIT_MASK      CTL_CODE(FILE_DEVICE_SERIAL_PORT, 10, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SERIAL_WAIT_ON_MASK       CTL_CODE(FILE_DEVICE_SERIAL_PORT, 11, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SERIAL_GET_COMMSTATUS     CTL_CODE(FILE_DEVICE_SERIAL_PORT, 12, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SERIAL_GET_MODEMSTATUS    CTL_CODE(FILE_DEVICE_SERIAL_PORT, 13, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SERIAL_GET_PROPERTIES     CTL_CODE(FILE_DEVICE_SERIAL_PORT, 14, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SERIAL_SET_TIMEOUTS       CTL_CODE(FILE_DEVICE_SERIAL_PORT, 15, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SERIAL_GET_TIMEOUTS       CTL_CODE(FILE_DEVICE_SERIAL_PORT, 16, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SERIAL_PURGE              CTL_CODE(FILE_DEVICE_SERIAL_PORT, 17, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SERIAL_SET_QUEUE_SIZE     CTL_CODE(FILE_DEVICE_SERIAL_PORT, 18, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SERIAL_IMMEDIATE_CHAR     CTL_CODE(FILE_DEVICE_SERIAL_PORT, 19, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SERIAL_GET_DCB			CTL_CODE(FILE_DEVICE_SERIAL_PORT, 20, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SERIAL_SET_DCB			CTL_CODE(FILE_DEVICE_SERIAL_PORT, 21, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SERIAL_ENABLE_IR			CTL_CODE(FILE_DEVICE_SERIAL_PORT, 22, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SERIAL_DISABLE_IR			CTL_CODE(FILE_DEVICE_SERIAL_PORT, 23, METHOD_BUFFERED, FILE_ANY_ACCESS)
//for tnf's modem
#define IOCTL_SERIAL_GETDEVICENAME		CTL_CODE(FILE_DEVICE_SERIAL_PORT, 24, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SERIAL_GETDEVICETYPE		CTL_CODE(FILE_DEVICE_SERIAL_PORT, 25, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define IOCTL_SERIAL_GETCONNECTSTATUS	CTL_CODE(FILE_DEVICE_SERIAL_PORT, 26, METHOD_BUFFERED, FILE_ANY_ACCESS)


#ifdef __cplusplus
}	
#endif

#endif   // __SER_BASE_H

