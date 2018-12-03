#ifndef __CPU_H
#define __CPU_H
/*  // 8086
typedef struct _TSS{
	unsigned short	ip;
	unsigned short	flags;
	unsigned short	ax,cx,dx,bx;
	unsigned short	sp;
	unsigned short	bp;
	unsigned short	si;
	unsigned short	di;
	unsigned short	es;
	unsigned short	cs;
	unsigned short	ss;
	unsigned short	ds;
}TSS;
*/
typedef struct _TSS{
#define REG_EBX 0
	long ebx;//0
#define REG_ECX 4
	long ecx;//4
#define REG_EDX 8
	long edx;//8
#define REG_ESI 12
	long esi;//12
#define REG_EDI 16
	long edi;//16
#define REG_EBP 20
	long ebp;//20
#define REG_EAX 24
	long eax;//24
#define REG_DS 28
	unsigned short ds;//28
#define REG_ES 30
	unsigned short es;//30
#define REG_FS 32
	unsigned short fs;//32
#define REG_GS 34
	unsigned short gs;//34
#define REG_CS 36
	unsigned short cs;//36
#define REG_SS 38
	unsigned short ss;//38
#define REG_EIP 40
	long eip;//40
#define REG_EFLAGS 44
	long eflags;//44
#define REG_ESP 48
	long esp;//48
}TSS;

#define INIT_TSS  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }

#define EBX 0
#define ECX 1
#define EDX 2
#define ESI 3
#define EDI 4
#define EBP 5
#define EAX 6
#define DS 7
#define ES 8
#define FS 9
#define GS 10
#define ORIG_EAX 11
#define EIP 12
#define CS  13
#define EFL 14
#define UESP 15
#define SS   16

typedef struct _PTREGS {
	int  bx;
	int  cx;
	int  dx;
	int  si;
	int  di;
	int  bp;
	int  ax;
	int  ds;
	int  es;
    int  orig_ax; // irq index
	int  ip;
	int  cs;
	int  eflags;
	long sp;
	int  ss;
}PTREGS, * LPPTREGS;

//extern volatile int IntrEnable;

//#define STI()
//#define CLI()

void LockIRQSave( UINT * lpSave );
void UnlockIRQRestore( UINT * lpRestore );

//#define UnlockIRQRestore( lp ) ( KL_InterlockedExchange( &IntrEnable, *(lp) ) )
//#define LockIRQSave( lp ) ( *(lp) = KL_InterlockedExchange( &IntrEnable, FALSE ) )

#define IS_USER_MODE( lp ) (1)

#ifndef HZ
#define HZ 100
#endif

//#define PAGE_SIZE 1024
//#define PAGE_MASK (0x3ff)
//#define PAGE_TABLE_SIZE 1024
//#define PAGE_SHIFT 10

#define PAGE_SIZE (4*1024)
#define PAGE_MASK (4*1024-1)
#define PAGE_SHIFT 12
#define PAGE_ALIGN_DOWN( dwAdr ) ( (dwAdr) & (~PAGE_MASK) )
#define PAGE_ALIGN_UP( dwAdr ) ( ( (dwAdr) + PAGE_MASK ) & (~PAGE_MASK) )
#define PAGE_TABLE_SIZE 1024
#define NEXT_PHYSICAL_PAGE( lpAdr ) ( ((DWORD)lpAdr) + PAGE_SIZE )

#define UNCACHE_TO_CACHE( lpAdr ) ( lpAdr )
#define CACHE_TO_UNCACHE( lpAdr ) ( lpAdr )

#define FlushCacheAndClearTLB()
//#define GetMMUContext( lp )
#define GetVirtualPageAdr( lp ) (lp)

typedef int ATOMIC;
//中断状态
typedef struct __INTRBITS{
	unsigned mouse : 1;
	unsigned keyboard : 1;
	unsigned timer0 : 1;
	unsigned timer1 : 1;
	//volatile unsigned intr[32];
}_INTRBITS;
//中断屏蔽
typedef struct __INTRMASK{
	//volatile unsigned mask[32];
	unsigned mouse : 1;
	unsigned keyboard : 1;
	unsigned timer0 : 1;
	unsigned timer1 : 1;
}_INTRMASK;
//PIN 方向
typedef struct __INTRDIR{
	//volatile unsigned mask[32];
	unsigned mouse : 1;
	unsigned keyboard : 1;
	unsigned timer0 : 1;
	unsigned timer1 : 1;
}_INTRDIR;


//extern volatile _INTRBITS intrBits;
//extern volatile _INTRMASK intrMask;
//extern volatile int IntrEnable;

//extern MSG IntrMsg[32];


typedef struct _KERNEL_DATA
{
	_INTRBITS intrBits;
	_INTRMASK intrMask;
	int IntrEnable;
	MSG IntrMsg[32];
	MSG msgCurrent;
	HANDLE hKingmosThread;
	HANDLE hIntrThread;
	HANDLE hHardwareEvent;	
	HANDLE hJumpEvent;
	UINT uiLockSegStart;
	UINT uiLockSegEnd;
	CRITICAL_SECTION csMsg;
	LONG nLockCount;
	HANDLE hRTCControllerThread;
	long lRTCCount;
	BYTE   bCPUConext[2048];
}KERNEL_DATA, FAR * LPKERNEL_DATA;

extern volatile LPKERNEL_DATA lpKernelData;


//void AtomicAdd( ATOMIC i, ATOMIC *lpv );// { *(lpv) += (i); }
//void AtomicSub( ATOMIC i, ATOMIC *lpv );// { *(lpv) -= (i); }
//void AtomicInc( ATOMIC *lpv );// {  *(lpv)++; }
//void AtomicDec( ATOMIC *lpv );// {  *(lpv)--; }

//#define RestoreFlags 
//#define SaveFlags 
//#define GetMMUContext

#define GetPhysicalPageAdr( p ) (p)

#define XCHG( ptr, v, retv ) { retv = *ptr; *ptr = v; } 

#define RESCHED_PERIOD 5 // Reschedule period in ms

#ifdef __DEBUG

extern void _INTR_ON( char * lpfn, int line );
extern void _INTR_OFF( char * lpfn, int line );

#define INTR_ON() _INTR_ON( __FILE__, __LINE__ )
#define INTR_OFF() _INTR_OFF( __FILE__, __LINE__ )

#else

extern void INTR_ON( void );
extern void INTR_OFF( void );

#endif


#endif //__CPU_H
