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

;EXTRN	SwitchToProcessByHandle:NEAR
;EXTRN	SwitchBackProcess:NEAR
EXTRN   APICallReturn:NEAR
EXTRN   DoImplementCallBackSwitchTo:NEAR
EXTRN   DoImplementCallBackSwitchBack:NEAR

EXTRN	mem_cpy:NEAR
EXTRN   _chkesp:NEAR

        public  LockSeg_Start
LockSeg_Start  proc
		mov eax, _ret0
		ret
_ret0:
LockSeg_Start  endp


        public  Switch
Switch  proc

        ;.FPO    ( 0, 2, 0, 0, 0, 0 )
        push ebp
		mov  ebp, esp

		push    edx
		push    ecx

        mov     edx,[esp + 16]   ; eax = intr_enable adr
		mov     ecx, 1
		mov     DWORD ptr[edx], ecx       ; enable intr

		mov     edx,[ebp + 8]   ; edx = lpPrev

        mov     ecx,[ebp + 12]   ; ecx = lpNext

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
        push edx

	    mov edx, DWORD ptr[ebx+REG_EDX]
		mov ebx, DWORD ptr[ebx+REG_EBX]
		ret               ;will return to l_ret or ip of setting
l_ret:
		mov esp, ebp
		pop ebp

        ret
Switch  endp

;----------------------------------------------------
        public  JmpBack
JmpBack  proc
        mov     ebx,[esp + 8]   ; eax = intr_enable adr
		mov     eax, 1
		mov     DWORD ptr[ebx], eax       ; enable intr

		mov     ebx,[esp + 4]   ; eax = tts adr

		;restore register

        mov ss, WORD ptr[ebx+REG_SS]
		;mov cs, WORD ptr[ebx+REG_CS]
		mov gs, WORD ptr[ebx+REG_GS]
		mov fs, WORD ptr[ebx+REG_FS]
		mov es, WORD ptr[ebx+REG_ES]
		mov ds, WORD ptr[ebx+REG_DS]

        mov ebp, DWORD ptr[ebx+REG_EBP]
        mov esp, DWORD ptr[ebx+REG_ESP]
		mov eax, DWORD ptr[ebx+REG_EAX]
		mov ecx, DWORD ptr[ebx+REG_ECX]
		mov esi, DWORD ptr[ebx+REG_ESI]
		mov edi, DWORD ptr[ebx+REG_EDI]

		mov edx, DWORD ptr[ebx+REG_EFLAGS]
		push edx
		popfd

        ;setup switch address
        mov edx, DWORD ptr[ebx+REG_EIP]
        push edx
		;

	    mov edx, DWORD ptr[ebx+REG_EDX]
		mov ebx, DWORD ptr[ebx+REG_EBX]
	    ret
JmpBack  endp

;------------------------------------------------
        public  LockSeg_End
LockSeg_End  proc
_ret1:
		mov eax, _ret1
		ret
LockSeg_End  endp

;------------------------------------------------------------------
        public  KL_ImplementCallBack
        ;LRESULT KL_ImplementCallBack( LPCALLBACKDATA lpcbd, ... );
KL_ImplementCallBack  proc
        push ebp
		mov  ebp, esp

		push esi
		push edi

;		sub  esp, 24              ;  callstack struct
;-------------------------------------------
		mov  ecx, dword ptr[ebp+8]    ; pointer to lpcbd
		mov  edx, dword ptr[ecx]      ; edx = hProcess

		;mov  ecx, esp        ; CALLSTACK struct		
		;push ecx             ; arg2

		push edx             ; arg1
		call DoImplementCallBackSwitchTo
		add  esp, 4

		cmp  eax, 0
		je   _ret

		; copy param
		sub  esp, 64
		mov  edi, esp       ; destion 
		mov  esi, ebp
		add  esi, 8        ; source
		mov  ecx, 16        ; 16 dword (64bytes)
        rep movs dword ptr [edi],dword ptr [esi]             ; now copy


		mov  eax, [ebp+8]   ;pointer to lpcbd
		mov  edx, [eax+8]   ;edx = arg0
		mov  [esp], edx     ;
		mov  edx, [eax+4]   ;edx = lpfn

		mov  esi, esp       ; save esp
		call edx
		mov  esp, esi       ; restore esp 

		add  esp, 64

		push eax            ; save retv

		call DoImplementCallBackSwitchBack

		pop eax

_ret:
        ;add esp, 24              ; pop CALLSTACK struct

		pop edi
		pop esi

		mov esp, ebp
		pop ebp
		ret

KL_ImplementCallBack  endp

;------------------------------------------------------------------
        ; max 4 param 
        public  KL_ImplementCallBack4
        ;LRESULT KL_ImplementCallBack4( LPCALLBACKDATA lpcbd, ... );
KL_ImplementCallBack4  proc
        push ebp
		mov  ebp, esp

		push esi
		push edi

		;sub  esp, 24              ;  CALLSTACK
;-------------------------------------------
		mov  ecx, dword ptr[ebp+8]    ; pointer to lpcbd
		mov  edx, dword ptr[ecx]      ; edx = hProcess

		;mov  ecx, esp        ; CALLSTACK struct
		
		;push ecx             ; arg2
		push edx             ; arg1
		call DoImplementCallBackSwitchTo
		add  esp, 4

		cmp  eax, 0
		je   _ret

		; copy param
		sub  esp, 16
		mov  edi, esp       ; destion 
		mov  esi, ebp
		add  esi, 12        ; source

		mov  ecx, dword ptr[ebp+8]    ; pointer to lpcbd
		mov  edx, dword ptr[ecx+8]    ; edx = arg0

		mov  [edi], edx             ;[edi] = arg0
		add  edi, 4

		mov  ecx, 3         ; 3 dword (12bytes)
        rep movs dword ptr [edi],dword ptr [esi]             ; now copy


		mov  eax, [ebp+8]   ;pointer to lpcbd
		mov  edx, [eax+4]   ;edx = lpfn

		mov  esi, esp       ; save esp
		call edx
		mov  esp, esi       ; restore esp 

		add  esp, 16

		push eax            ; save retv

		call DoImplementCallBackSwitchBack

		pop eax

_ret:
;        add esp, 24              ; pop CALLSTACK struct

		pop edi
		pop esi

		mov esp, ebp
		pop ebp
		ret

KL_ImplementCallBack4  endp

        public  DoAPICall
        ;void DoAPICall( DWORD ip, DWORD esp, DWORD dwArgNum );
DoAPICall  proc
        push ebp
		mov  ebp, esp

        
		mov esi, [ebp+12]   ;esp
		mov ecx, [ebp+16]   ;dwArgNum
		sub esp, ecx        ;esp + 4 * ecx
		sub esp, ecx
		sub esp, ecx
		sub esp, ecx
		mov edi, esp

        rep movs dword ptr [edi],dword ptr [esi]             ; now copy args

		mov eax, [ebp+8]    ;ip
		call eax            ;call api

		mov esp, ebp
		pop ebp
		ret
DoAPICall  endp

        public  DoAPICall

SetProcessId proc
        ret
SetProcessId endp

;==============================================
;void SwitchToKernelStackSpace( LPVOID );
;==============================================
SwitchToStackSpace proc
		mov eax, [esp]				;return address
        mov ebx, [esp+4]			;new sp
		mov esp, ebx				;
		push eax
		push eax                    ; push safe bounder
		push ebx
		push eax
		ret
SwitchToStackSpace endp

;=======================================================
;VOID HandlerException( jmp_buf jmp_data, int retv, UINT mode )
;
;=======================================================

HandlerException proc
		ret
HandlerException endp





        end