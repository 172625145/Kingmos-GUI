/******************************************************
Copyright(c) 版权所有，1998-2003微逻辑。保留所有权利。
******************************************************/

#ifndef __ESETJMP_H
#define __ESETJMP_H

#ifdef  __cplusplus
extern "C" {
#endif

#ifdef X86_CPU
	typedef struct {
		unsigned long eax;
		unsigned long ebx;
		unsigned long ecx;
		unsigned long edx;
		unsigned long esi;
		unsigned long edi;
		unsigned long ebp;
		unsigned long esp;
		unsigned long eip;
	} jmp_buf[1];

	typedef struct _EXCEPTION_CONTEXT{
		unsigned long eax;
		unsigned long ebx;
		unsigned long ecx;
		unsigned long edx;
		unsigned long esi;
		unsigned long edi;
		unsigned long ebp;
		unsigned long esp;
		unsigned long eip;
	} EXCEPTION_CONTEXT, FAR * LPEXCEPTION_CONTEXT;

#endif

#ifdef ARM_CPU

	typedef struct {
		//r4 - r12, sp, lr
        unsigned long r4;
		unsigned long r5;
		unsigned long r6;
		unsigned long r7;
		unsigned long r8;
		unsigned long r9;
		unsigned long r10;
		unsigned long r11;
		unsigned long r12;
		unsigned long sp;
		unsigned long lr;	
	} jmp_buf[1];

	//修改该结构， 必须同时修改 callsrv.s swi.s
	typedef struct _EXCEPTION_CONTEXT{
		//r4 - r12, sp, pc, lr
        unsigned long r4;
		unsigned long r5;
		unsigned long r6;
		unsigned long r7;
		unsigned long r8;
		unsigned long r9;
		unsigned long r10;
		unsigned long r11;
		unsigned long r12;
		unsigned long sp;
		unsigned long lr;	
		unsigned long ip;		// longjmp return address
	} EXCEPTION_CONTEXT, FAR * LPEXCEPTION_CONTEXT;

#endif

#define setjmp sys_setjmp
int sys_setjmp( jmp_buf );

#define longjmp sys_longjmp
void sys_longjmp( jmp_buf, int );


#ifdef  __cplusplus
}
#endif

#endif  // __ESETJMP
