/******************************************************
Copyright(c) ��Ȩ���У�1998-2003΢�߼�����������Ȩ����
******************************************************/

#ifndef __SYSINTR_H
#define __SYSINTR_H

#ifdef __cplusplus
extern "C" {
#endif      /* __cplusplus */


#define SYSINTR_NOP         0   // no option
#define SYSINTR_RESCHED     1   // reschedule interrupt
#define SYSINTR_BREAK       2   // break into debugger

// SYSINTR_DEVICES �ǻ������ж�����
#define SYSINTR_DEVICES     8

#define SYSINTR_PROFILE     (SYSINTR_DEVICES+1)  // system profiling
#define SYSINTR_TIMING      (SYSINTR_DEVICES+2)  // latency analysis
#define SYSINTR_RTC_ALARM	(SYSINTR_DEVICES+5) // real-time clock alarm

// SYSINTR_FIRMWARE �豸�����Զ�����ж�����
#define SYSINTR_FIRMWARE    (SYSINTR_DEVICES+8)

#define SYSINTR_MAX_DEVICES 32
#define SYSINTR_MAXIMUM     (SYSINTR_DEVICES+SYSINTR_MAX_DEVICES)

// �ڲ�����
extern int iISRActiveCount;
typedef void (CALLBACK * LPISR)( DWORD dwISRHandle );
BOOL ISR_Init( void );
void ISR_Deinit( void );
// if idISR = ISR_ALL_INTRS , enum all isr bit
#define ISR_ALL_INTRS   (-1)
void ISR_Handler( UINT idISR );
void ISR_Enable( UINT idISR );
void ISR_Disable( UINT idISR );
void ISR_Active( UINT idISR );
BOOL ISR_RegisterServer( UINT nIndex, LPISR lpISRFun, DWORD dwISRHandle );
void ISR_Unregister( UINT idISR );
//

// �ж�API, �豸����������
BOOL WINAPI INTR_Init( UINT idInt, 
				       HANDLE hIntEvent,  // // intr event
				       LPVOID lpvData,   // transfer to OEM_InterruptEnable
				       DWORD dwSize      // transfer to OEM_InterruptEnable
				     );
void WINAPI INTR_Done( UINT idInt );
void WINAPI INTR_Disable( UINT idInt );
void WINAPI INTR_Enable( UINT idInt, LPVOID lpvData, DWORD dwSize );
//

#ifdef __cplusplus
}
#endif  /* __cplusplus */

#endif // __SYSINTR_H


