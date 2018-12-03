#ifndef __OEMFUNC_H
#define __OEMFUNC_H

#ifdef __cplusplus
extern "C" {
#endif      /* __cplusplus */

// OEM supplied debug support functions
void OEM_InitDebugSerial(void);
void OEM_WriteDebugString( LPCTSTR str );
void OEM_WriteDebugLED(WORD wIndex, DWORD dwPattern);
int OEM_ReadDebugByte(void);
void OEM_WriteDebugByte(BYTE ch);
void OEM_ClearDebugCommError(void);
int OEM_ParallelPortGetByte(void);
void OEM_ParallelPortSendByte(BYTE ch);

// OEM supplied functions 
void OEM_Init(void);

void OEM_InterruptDisable(DWORD idInt);
BOOL OEM_InterruptEnable(DWORD idInt, LPVOID pvData, DWORD dwData);
void OEM_InterruptDone(DWORD idInt);
BOOL OEM_GetRealTime( SYSTEMTIME FAR *lpst);
BOOL OEM_SetRealTime( const SYSTEMTIME FAR * lpst);
BOOL OEM_SetAlarmTime( const SYSTEMTIME FAR * lpst);
DWORD OEM_GetTickCount( void );
DWORD OEM_TimeToJiffies( DWORD dwMilliseconds, DWORD dwNanoseconds );
typedef struct _MEM_SEGMENT
{
	LPBYTE lpbStart;
	DWORD  dwSize;
}MEM_SEGMENT, FAR * LPMEM_SEGMENT;

int OEM_EnumExtensionDRAM( LPMEM_SEGMENT lpSegs, UINT uiMaxSegNum );

int OEM_InterruptHandler(unsigned int ra);

int OEM_EnterIdleMode( DWORD dwFlag );
int OEM_PowerOff( DWORD dwReserve );

//将注册表保存到或恢复从OEM设备
//假如你想支持该功能，按以下步骤：
//1.实现一个功能函数，如  BOOL OEM_RegistryIO( LPBYTE lpDataBuf, DWORD nNumberOfBytes, DWORD dwFlag )
//2.对全局变量付值        extern LPOEM_REGISTRYIO lpOEM_RegistryIO=OEM_RegistryIO;
typedef BOOL ( * LPOEM_REGISTRYIO )( LPBYTE lpDataBuf, DWORD nNumberOfBytes, DWORD dwFlag );

// OEM supplied data
extern unsigned long ulOEMClockFreq;  // Timer clock frequency in Hertz

#ifdef __cplusplus
}
#endif  /* __cplusplus */


#endif  //__OEMFUNC_H




