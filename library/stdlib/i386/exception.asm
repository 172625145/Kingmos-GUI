;	#include "i386mach.h"
;	#include "cpu.h"	

;*******************************************************************************
.386
.MODEL flat
.CODE


	public  _Sys_CaptureException@0

_Sys_CaptureException@0	proc near
    mov		eax, 0
	ret
_Sys_CaptureException@0	endp


	end



