

@#ifndef _KING_LINKAGE_H
@#define _KING_LINKAGE_H



@ The macro of ENTRY is define to export global symbol.
@
	.macro	ENTRY, name

	.globl \name; 
	.align 0; 
	\name:

	.endm

@#endif

