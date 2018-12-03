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
	

;    .global SYM (setjmp)
;    .global SYM (longjmp)
;    SOTYPE_FUNCTION(setjmp)
;    SOTYPE_FUNCTION(longjmp)
	
;/*	
;SYM (setjmp):
;
;	pushl	ebp
;	movl	esp,ebp
;
;	pushl	edi
;	movl	8 (ebp),edi
;
;	movl	eax,0 (edi)
;	movl	ebx,4 (edi)
;	movl	ecx,8 (edi)
;;	movl	edx,12 (edi)
;;	movl	esi,16 (edi)
;;
;;	movl	-4 (ebp),eax
;;	movl	eax,20 (edi)
;;
;;	movl	0 (ebp),eax
;;	movl	eax,24 (edi)
;;
;;	movl	esp,eax
;;	addl	$12,eax
;	movl	eax,28 (edi)
;	
;	movl	4 (ebp),eax
;	movl	eax,32 (edi)
;
;	popl	edi
;	movl	$0,eax
;	leave
;	ret
;*/

	
	public  sys_setjmp 
sys_setjmp	proc
	
	push	ebp
	mov		ebp,esp

	push	edi
	mov		edi,[ebp + 8]

	mov	DWORD ptr[edi+REG_EAX], eax
	mov	DWORD ptr[edi+REG_EBX], ebx
	mov	DWORD ptr[edi+REG_ECX], ecx
	mov	DWORD ptr[edi+REG_EDX], edx
	mov	DWORD ptr[edi+REG_ESI], esi

	mov		eax,[ebp - 4]
	mov	DWORD ptr[edi+REG_EDI], eax

	mov		eax, [ebp + 0]	;!!!get the value of ebp pointer
	mov	DWORD ptr[edi+REG_EBP], eax

	mov		eax, esp
	add		eax, 12
	mov	DWORD ptr[edi+REG_ESP], eax

	mov		eax, [ebp + 4]
	mov DWORD ptr[edi+REG_EIP], eax

	pop		edi
	mov		eax, 0

	leave
	ret
sys_setjmp	endp

;/*
;SYM (longjmp):
;	pushl	ebp
;	movl	esp,ebp
;
;	movl	8(ebp),edi	/* get jmp_buf */
;	movl	12(ebp),eax	/* store retval in j->eax */
;	movl	eax,0(edi)
;
;	movl	24(edi),ebp
;
;;       __CLI
;	movl	28(edi),esp
;	
;	pushl	32(edi)	
;
;	movl	0(edi),eax
;	movl	4(edi),ebx
;	movl	8(edi),ecx
;	movl	12(edi),edx
;	movl	16(edi),esi
;	movl	20(edi),edi
;;       __STI

;	ret
;*/

	end



