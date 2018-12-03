;/* This is file is a merger of SETJMP.S and LONGJMP.S */
;/*
; *  This file was modified to use the __USER_LABEL_PREFIX__ and
; *  __REGISTER_PREFIX__ macros defined by later versions of GNU cpp by
; *  Joel Sherrill (joel@OARcorp.com)
; *  Slight change: now includes i386mach.h for this (Werner Almesberger)
; *
; * Copyright (C) 1991 DJ Delorie
; * All rights reserved.
; *
; * Redistribution and use in source and binary forms is permitted
; * provided that the above copyright notice and following paragraph are
; * duplicated in all such forms.
; *
; * This file is distributed WITHOUT ANY WARRANTY; without even the implied
; * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
; */
;
; /*
; **	jmp_buf:
; **	 eax ebx ecx edx esi edi ebp esp eip
; **	 0   4   8   12  16  20  24  28  32
; */


;	#include "i386mach.h"
;	#include "cpu.h"	

;*******************************************************************************
.386
.MODEL flat, C
.CODE

	REG_EAX equ 0
	REG_EBX equ 4
	REG_ECX equ 8
	REG_EDX equ 12
	REG_ESI equ 16
	REG_EDI equ 20
	REG_EBP equ 24
	REG_ESP equ 28
	REG_EIP equ 32	
	


	public  sys_longjmp 
sys_longjmp	proc

	push	ebp
	mov		ebp,esp

	mov		edi, [ebp + 8]
	mov		eax, [ebp + 12]
	mov		DWORD ptr[edi + REG_EAX], eax

	mov		ebp, DWORD ptr[edi+REG_EBP]

;	__CLI
	mov		esp, DWORD ptr[edi+REG_ESP]

	push	DWORD ptr[edi+REG_EIP]

	mov	eax, DWORD ptr[edi+REG_EAX]
	mov	ebx, DWORD ptr[edi+REG_EBX]
	mov	ecx, DWORD ptr[edi+REG_ECX]
	mov	edx, DWORD ptr[edi+REG_EDX]
	mov	esi, DWORD ptr[edi+REG_ESI]
	mov	edi, DWORD ptr[edi+REG_EDI]
;	__STI

	ret
sys_longjmp	endp 

	end



