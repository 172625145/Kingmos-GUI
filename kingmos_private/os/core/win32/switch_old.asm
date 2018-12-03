        ;.xlist
;        include cruntime.inc
        ;.list

;*******************************************************************************
.386
.MODEL flat, C
.CODE
        ;CODESEG
		;see cpu.h
	REG_EBX equ 0
	REG_ECX equ 4
	REG_EDX equ 8
	REG_ESI equ 12
	REG_EDI equ 16
	REG_EBP equ 20
	REG_EAX equ 24
	REG_DS equ 28
	REG_ES equ 30
	REG_FS equ 32
	REG_GS equ 34
	REG_CS equ 36
	REG_SS equ 38
	REG_EIP equ 40
	REG_EFLAGS equ 44
	REG_ESP equ 48


        public  Switch
Switch  proc

        .FPO    ( 0, 2, 0, 0, 0, 0 )

		push    edx
		push    ecx

		;mov DWORD ptr[esp + 4][REG_EDX], edx
		mov     edx,[esp + 12]   ; edx = lpPrev

		;mov DWORD ptr[esp + 8][REG_EDX], ecx
        mov     ecx,[esp + 16]   ; ecx = lpNext

		; save register
		mov DWORD ptr[edx+REG_EAX], eax
		mov DWORD ptr[edx+REG_EBX], ebx

		mov eax, edx
		mov ebx, ecx

		pop  ecx
		pop  edx

		mov DWORD ptr[eax+REG_ESI], esi
		mov DWORD ptr[eax+REG_EDI], edi
		mov DWORD ptr[eax+REG_ECX], ecx
		mov DWORD ptr[eax+REG_EDX], edx
        mov DWORD ptr[eax+REG_ESP], esp
		mov DWORD ptr[eax+REG_EBP], ebp
        mov WORD ptr[eax+REG_SS],  ss
		;mov WORD ptr[eax+REG_CS],  cs		
		mov ecx, l_ret 
		mov DWORD ptr[eax+REG_EIP], ecx

		;restore register

        mov ss, WORD ptr[ebx+REG_SS]
        mov ebp, DWORD ptr[ebx+REG_EBP]
        mov esp, DWORD ptr[ebx+REG_ESP]
		mov eax, DWORD ptr[ebx+REG_EAX]
		mov ecx, DWORD ptr[ebx+REG_ECX]
		mov esi, DWORD ptr[ebx+REG_ESI]
		mov edi, DWORD ptr[ebx+REG_EDI]

        ;setup switch address
        mov edx, DWORD ptr[ebx+REG_EIP]
		;mov ip, edx
        push edx

	    mov edx, DWORD ptr[ebx+REG_EDX]
		mov ebx, DWORD ptr[ebx+REG_EBX]

		ret
		;pop eip
l_ret:
        ret
Switch  endp

        end
