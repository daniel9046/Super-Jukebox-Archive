	page	,132
	.listall

;Thunk Compiler Version 1.8  May 11 1995 13:16:19
;File Compiled Mon Nov 08 13:08:49 1999

;Command Line: C:\PROGRA~1\PLATFO~1\WIN16\BINW16\THUNK.EXE -t thk0 thk0.thk -o thk0.asm 

	TITLE	$thk0.asm

	.386
	OPTION READONLY


IFNDEF IS_16
IFNDEF IS_32
%out command line error: specify one of -DIS_16 -DIS_32
.err
ENDIF
ENDIF
IFDEF IS_16
IFDEF IS_32
%out command line error: you can't specify both -DIS_16 and -DIS_32
.err
ENDIF

	OPTION SEGMENT:USE16
	.model LARGE,PASCAL

externDef GetCurrentProcess:far16
externDef GetPriorityClass:far16
externDef SetPriorityClass:far16
externDef StopSPC:far16
externDef PauseSPC:far16
externDef PlaySPC:far16
externDef LoadSPC:far16
externDef RestoreSPC:far16
externDef DeinitSPC:far16
externDef InitSPC:far16
externDef StopZSPC:far16
externDef PauseZSPC:far16
externDef PlayZSPC:far16
externDef DeinitZSPC:far16
externDef InitZSPC:far16
externDef WriteRegistrySettings:far16
externDef ReadRegistrySettings:far16
externDef DisplayConfigDlg:far16

externDef C16ThkSL01:far16


	.data

public thk0_ThunkData16	;This symbol must be exported.
thk0_ThunkData16	dd	31304c53h	;Protocol 'SL01'
	dd	025e79h	;Checksum
	dd	0		;Flags.
	dd	0		;RESERVED. MUST BE ZERO.
	dd	0		;RESERVED. MUST BE ZERO.
	dd	0		;RESERVED. MUST BE ZERO.
	dd	0		;RESERVED. MUST BE ZERO.
	dd	3130424ch	;Late-binding signature 'LB01'
	dd	080000000h		;More flags.
	dd	0		;RESERVED. MUST BE ZERO.
	dw	offset thk0_ThunkData16ApiDatabase
	dw	   seg thk0_ThunkData16ApiDatabase


;; Api database. Each entry == 8 bytes:
;;  byte  0:     # of argument bytes.
;;  byte  1,2,3: Reserved: Must initialize to 0.
;;  dword 4:	 error return value.
public thk0_ThunkData16ApiDatabase
thk0_ThunkData16ApiDatabase	label	dword
	db	0
	db	0,0,0
	dd	0
	db	4
	db	0,0,0
	dd	0
	db	8
	db	0,0,0
	dd	0
	db	0
	db	0,0,0
	dd	0
	db	0
	db	0,0,0
	dd	0
	db	0
	db	0,0,0
	dd	0
	db	4
	db	0,0,0
	dd	0
	db	0
	db	0,0,0
	dd	0
	db	4
	db	0,0,0
	dd	0
	db	20
	db	0,0,0
	dd	0
	db	0
	db	0,0,0
	dd	0
	db	0
	db	0,0,0
	dd	0
	db	4
	db	0,0,0
	dd	0
	db	0
	db	0,0,0
	dd	0
	db	0
	db	0,0,0
	dd	0
	db	4
	db	0,0,0
	dd	0
	db	4
	db	0,0,0
	dd	0
	db	8
	db	0,0,0
	dd	0




	.code 


externDef ThunkConnect16:far16

public thk0_ThunkConnect16
thk0_ThunkConnect16:
	pop	ax
	pop	dx
	push	seg    thk0_ThunkData16
	push	offset thk0_ThunkData16
	push	seg    thk0_TD32Label
	push	offset thk0_TD32Label
	push	cs
	push	dx
	push	ax
	jmp	ThunkConnect16
thk0_TD32Label label byte
	db	"thk0_ThunkData32",0


GetCurrentProcess label far16
	mov	cx,0			; offset in jump table
	jmp	thk0EntryCommon

GetPriorityClass label far16
	mov	cx,4			; offset in jump table
	jmp	thk0EntryCommon

SetPriorityClass label far16
	mov	cx,8			; offset in jump table
	jmp	thk0EntryCommon

StopSPC label far16
	mov	cx,12			; offset in jump table
	jmp	thk0EntryCommon

PauseSPC label far16
	mov	cx,16			; offset in jump table
	jmp	thk0EntryCommon

PlaySPC label far16
	mov	cx,20			; offset in jump table
	jmp	thk0EntryCommon

LoadSPC label far16
	mov	cx,24			; offset in jump table
	jmp	thk0EntryCommon

RestoreSPC label far16
	mov	cx,28			; offset in jump table
	jmp	thk0EntryCommon

DeinitSPC label far16
	mov	cx,32			; offset in jump table
	jmp	thk0EntryCommon

InitSPC label far16
	mov	cx,36			; offset in jump table
	jmp	thk0EntryCommon

StopZSPC label far16
	mov	cx,40			; offset in jump table
	jmp	thk0EntryCommon

PauseZSPC label far16
	mov	cx,44			; offset in jump table
	jmp	thk0EntryCommon

PlayZSPC label far16
	mov	cx,48			; offset in jump table
	jmp	thk0EntryCommon

DeinitZSPC label far16
	mov	cx,52			; offset in jump table
	jmp	thk0EntryCommon

InitZSPC label far16
	mov	cx,56			; offset in jump table
	jmp	thk0EntryCommon

WriteRegistrySettings label far16
	mov	cx,60			; offset in jump table
	jmp	thk0EntryCommon

ReadRegistrySettings label far16
	mov	cx,64			; offset in jump table
	jmp	thk0EntryCommon

DisplayConfigDlg label far16
	mov	cx,68			; offset in jump table
	jmp	thk0EntryCommon

;===========================================================================
; This is the common setup code for 16=>32 thunks.
;
; Entry:  cx  = offset in flat jump table
;
; Don't optimize this code: C16ThkSL01 overwrites it
; after each discard.

align
thk0EntryCommon:
	db	0ebh, 030	;Jump short forward 30 bytes.
;;; Leave at least 30 bytes for C16ThkSL01's code patching.
	db	30 dup(0cch)	;Patch space.
	push	seg    thk0_ThunkData16
	push	offset thk0_ThunkData16
	pop	edx
	push	cs
	push	offset thk0EntryCommon
	pop	eax
	jmp	C16ThkSL01

ELSE	; IS_32
	.model FLAT,STDCALL

externDef STDCALL K32Thk1632Prolog@0:near32
externDef STDCALL K32Thk1632Epilog@0:near32
externDef STDCALL GetCurrentProcess@0:near32
externDef STDCALL GetPriorityClass@4:near32
externDef STDCALL SetPriorityClass@8:near32
externDef STDCALL StopSPC@0:near32
externDef STDCALL PauseSPC@0:near32
externDef STDCALL PlaySPC@0:near32
externDef STDCALL LoadSPC@4:near32
externDef STDCALL RestoreSPC@0:near32
externDef STDCALL DeinitSPC@4:near32
externDef STDCALL InitSPC@20:near32
externDef STDCALL StopZSPC@0:near32
externDef STDCALL PauseZSPC@0:near32
externDef STDCALL PlayZSPC@4:near32
externDef STDCALL DeinitZSPC@0:near32
externDef STDCALL InitZSPC@0:near32
externDef STDCALL WriteRegistrySettings@4:near32
externDef STDCALL ReadRegistrySettings@4:near32
externDef STDCALL DisplayConfigDlg@8:near32

MapSLFix		proto	STDCALL  :DWORD
MapSL		proto	STDCALL  :DWORD
UnMapSLFixArray		proto	STDCALL  :DWORD, :DWORD
externDef	MapHInstSL:near32
externDef	MapHInstSL_PN:near32
externDef	MapHInstLS:near32
externDef	MapHInstLS_PN:near32
externDef T_GETCURRENTPROCESS:near32
externDef T_GETPRIORITYCLASS:near32
externDef T_SETPRIORITYCLASS:near32
externDef T_STOPSPC:near32
externDef T_PAUSESPC:near32
externDef T_PLAYSPC:near32
externDef T_LOADSPC:near32
externDef T_RESTORESPC:near32
externDef T_DEINITSPC:near32
externDef T_INITSPC:near32
externDef T_STOPZSPC:near32
externDef T_PAUSEZSPC:near32
externDef T_PLAYZSPC:near32
externDef T_DEINITZSPC:near32
externDef T_INITZSPC:near32
externDef T_WRITEREGISTRYSETTINGS:near32
externDef T_READREGISTRYSETTINGS:near32
externDef T_DISPLAYCONFIGDLG:near32

;===========================================================================
	.code 


; This is a jump table to API-specific flat thunk code.

align
thk0_JumpTable label dword
	dd	offset FLAT:T_GETCURRENTPROCESS
	dd	offset FLAT:T_GETPRIORITYCLASS
	dd	offset FLAT:T_SETPRIORITYCLASS
	dd	offset FLAT:T_STOPSPC
	dd	offset FLAT:T_PAUSESPC
	dd	offset FLAT:T_PLAYSPC
	dd	offset FLAT:T_LOADSPC
	dd	offset FLAT:T_RESTORESPC
	dd	offset FLAT:T_DEINITSPC
	dd	offset FLAT:T_INITSPC
	dd	offset FLAT:T_STOPZSPC
	dd	offset FLAT:T_PAUSEZSPC
	dd	offset FLAT:T_PLAYZSPC
	dd	offset FLAT:T_DEINITZSPC
	dd	offset FLAT:T_INITZSPC
	dd	offset FLAT:T_WRITEREGISTRYSETTINGS
	dd	offset FLAT:T_READREGISTRYSETTINGS
	dd	offset FLAT:T_DISPLAYCONFIGDLG

thk0_ThunkDataName label byte
	db	"thk0_ThunkData16",0

	.data

public thk0_ThunkData32	;This symbol must be exported.
thk0_ThunkData32	dd	31304c53h	;Protocol 'SL01'
	dd	025e79h	;Checksum
	dd	0	;Reserved (MUST BE 0)
	dd	0	;Flat address of ThunkData16
	dd	3130424ch	;'LB01'
	dd	0	;Flags
	dd	0	;Reserved (MUST BE 0)
	dd	0	;Reserved (MUST BE 0)
	dd	offset thk0_JumpTable - offset thk0_ThunkDataName



	.code 


externDef ThunkConnect32@24:near32

public thk0_ThunkConnect32@16
thk0_ThunkConnect32@16:
	pop	edx
	push	offset thk0_ThunkDataName
	push	offset thk0_ThunkData32
	push	edx
	jmp	ThunkConnect32@24


;===========================================================================
; Common routines to restore the stack and registers
; and return to 16-bit code.  There is one for each
; size of 16-bit parameter list in this script.

align
ExitFlat_0:
	mov	cl,0		; parameter byte count
	mov	esp,ebp		; point to return address
	retn			; return to dispatcher

align
ExitFlat_4:
	mov	cl,4		; parameter byte count
	mov	esp,ebp		; point to return address
	retn			; return to dispatcher

align
ExitFlat_8:
	mov	cl,8		; parameter byte count
	mov	esp,ebp		; point to return address
	retn			; return to dispatcher

align
ExitFlat_20:
	mov	cl,20		; parameter byte count
	mov	esp,ebp		; point to return address
	retn			; return to dispatcher

;===========================================================================
T_GETCURRENTPROCESS label near32


;-------------------------------------
; create new call frame and make the call

	call	K32Thk1632Prolog@0
	call	GetCurrentProcess@0		; call 32-bit version
	call	K32Thk1632Epilog@0

; return code unsigned long --> unsigned long
	mov	edx,eax
	rol	edx,16

;-------------------------------------
	jmp	ExitFlat_0

;===========================================================================
T_GETPRIORITYCLASS label near32

; ebx+22   param1

;-------------------------------------
; create new call frame and make the call

; param1  from: unsigned long
	push	dword ptr [ebx+22]	; to unsigned long

	call	K32Thk1632Prolog@0
	call	GetPriorityClass@4		; call 32-bit version
	call	K32Thk1632Epilog@0

; return code unsigned long --> unsigned long
	mov	edx,eax
	rol	edx,16

;-------------------------------------
	jmp	ExitFlat_4

;===========================================================================
T_SETPRIORITYCLASS label near32

; ebx+26   param1
; ebx+22   param2

;-------------------------------------
; create new call frame and make the call

; param2  from: unsigned long
	push	dword ptr [ebx+22]	; to unsigned long

; param1  from: unsigned long
	push	dword ptr [ebx+26]	; to unsigned long

	call	K32Thk1632Prolog@0
	call	SetPriorityClass@8		; call 32-bit version
	call	K32Thk1632Epilog@0

; return code unsigned long --> unsigned long
	mov	edx,eax
	rol	edx,16

;-------------------------------------
	jmp	ExitFlat_8

;===========================================================================
T_STOPSPC label near32


;-------------------------------------
; create new call frame and make the call

	call	K32Thk1632Prolog@0
	call	StopSPC@0		; call 32-bit version
	call	K32Thk1632Epilog@0

; return code void --> void
; no conversion needed

;-------------------------------------
	jmp	ExitFlat_0

;===========================================================================
T_PAUSESPC label near32


;-------------------------------------
; create new call frame and make the call

	call	K32Thk1632Prolog@0
	call	PauseSPC@0		; call 32-bit version
	call	K32Thk1632Epilog@0

; return code void --> void
; no conversion needed

;-------------------------------------
	jmp	ExitFlat_0

;===========================================================================
T_PLAYSPC label near32


;-------------------------------------
; create new call frame and make the call

	call	K32Thk1632Prolog@0
	call	PlaySPC@0		; call 32-bit version
	call	K32Thk1632Epilog@0

; return code void --> void
; no conversion needed

;-------------------------------------
	jmp	ExitFlat_0

;===========================================================================
T_LOADSPC label near32

; ebx+22   param1

;-------------------------------------
; temp storage

	xor	eax,eax
	push	eax	;MapLS unfix temp
; param1 inline mapping
;-------------------------------------
; *** BEGIN parameter packing

; param1
; pointer char --> char
; inline mapping

; *** END   parameter packing
;-------------------------------------
; create new call frame and make the call

; param1  from: char
	mov	eax, [ebx+22]		; base address
	mov	[esp + 0],eax
	push	eax
	call	MapSLFix	
	push	eax

	call	K32Thk1632Prolog@0
	call	LoadSPC@4		; call 32-bit version
	call	K32Thk1632Epilog@0

; return code unsigned long --> unsigned long
	mov	edx,eax
	rol	edx,16

	lea	ecx, [esp+0]
	push	ecx
	push	dword ptr 1
	call	UnMapSLFixArray	;! Preserves eax & edx
;-------------------------------------
	jmp	ExitFlat_4

;===========================================================================
T_RESTORESPC label near32


;-------------------------------------
; create new call frame and make the call

	call	K32Thk1632Prolog@0
	call	RestoreSPC@0		; call 32-bit version
	call	K32Thk1632Epilog@0

; return code void --> void
; no conversion needed

;-------------------------------------
	jmp	ExitFlat_0

;===========================================================================
T_DEINITSPC label near32

; ebx+22   param1

;-------------------------------------
; create new call frame and make the call

; param1  from: unsigned long
	push	dword ptr [ebx+22]	; to unsigned long

	call	K32Thk1632Prolog@0
	call	DeinitSPC@4		; call 32-bit version
	call	K32Thk1632Epilog@0

; return code void --> void
; no conversion needed

;-------------------------------------
	jmp	ExitFlat_4

;===========================================================================
T_INITSPC label near32

; ebx+38   param1
; ebx+34   param2
; ebx+30   param3
; ebx+26   param4
; ebx+22   param5

;-------------------------------------
; create new call frame and make the call

; param5  from: unsigned long
	push	dword ptr [ebx+22]	; to unsigned long

; param4  from: unsigned long
	push	dword ptr [ebx+26]	; to unsigned long

; param3  from: unsigned long
	push	dword ptr [ebx+30]	; to unsigned long

; param2  from: unsigned long
	push	dword ptr [ebx+34]	; to unsigned long

; param1  from: unsigned long
	push	dword ptr [ebx+38]	; to unsigned long

	call	K32Thk1632Prolog@0
	call	InitSPC@20		; call 32-bit version
	call	K32Thk1632Epilog@0

; return code unsigned long --> unsigned long
	mov	edx,eax
	rol	edx,16

;-------------------------------------
	jmp	ExitFlat_20

;===========================================================================
T_STOPZSPC label near32


;-------------------------------------
; create new call frame and make the call

	call	K32Thk1632Prolog@0
	call	StopZSPC@0		; call 32-bit version
	call	K32Thk1632Epilog@0

; return code void --> void
; no conversion needed

;-------------------------------------
	jmp	ExitFlat_0

;===========================================================================
T_PAUSEZSPC label near32


;-------------------------------------
; create new call frame and make the call

	call	K32Thk1632Prolog@0
	call	PauseZSPC@0		; call 32-bit version
	call	K32Thk1632Epilog@0

; return code void --> void
; no conversion needed

;-------------------------------------
	jmp	ExitFlat_0

;===========================================================================
T_PLAYZSPC label near32

; ebx+22   param1

;-------------------------------------
; temp storage

	xor	eax,eax
	push	eax	;MapLS unfix temp
; param1 inline mapping
;-------------------------------------
; *** BEGIN parameter packing

; param1
; pointer char --> char
; inline mapping

; *** END   parameter packing
;-------------------------------------
; create new call frame and make the call

; param1  from: char
	mov	eax, [ebx+22]		; base address
	mov	[esp + 0],eax
	push	eax
	call	MapSLFix	
	push	eax

	call	K32Thk1632Prolog@0
	call	PlayZSPC@4		; call 32-bit version
	call	K32Thk1632Epilog@0

; return code unsigned long --> unsigned long
	mov	edx,eax
	rol	edx,16

	lea	ecx, [esp+0]
	push	ecx
	push	dword ptr 1
	call	UnMapSLFixArray	;! Preserves eax & edx
;-------------------------------------
	jmp	ExitFlat_4

;===========================================================================
T_DEINITZSPC label near32


;-------------------------------------
; create new call frame and make the call

	call	K32Thk1632Prolog@0
	call	DeinitZSPC@0		; call 32-bit version
	call	K32Thk1632Epilog@0

; return code void --> void
; no conversion needed

;-------------------------------------
	jmp	ExitFlat_0

;===========================================================================
T_INITZSPC label near32


;-------------------------------------
; create new call frame and make the call

	call	K32Thk1632Prolog@0
	call	InitZSPC@0		; call 32-bit version
	call	K32Thk1632Epilog@0

; return code unsigned long --> unsigned long
	mov	edx,eax
	rol	edx,16

;-------------------------------------
	jmp	ExitFlat_0

;===========================================================================
T_WRITEREGISTRYSETTINGS label near32

; ebx+22   lpVoid

;-------------------------------------
; temp storage

	xor	eax,eax
	push	eax	;MapLS unfix temp
; lpVoid inline mapping
;-------------------------------------
; *** BEGIN parameter packing

; lpVoid
; pointer struct --> struct
; inline mapping

; *** END   parameter packing
;-------------------------------------
; create new call frame and make the call

; lpVoid  from: struct
	mov	eax, [ebx+22]		; base address
	mov	[esp + 0],eax
	push	eax
	call	MapSLFix	
	push	eax

	call	K32Thk1632Prolog@0
	call	WriteRegistrySettings@4		; call 32-bit version
	call	K32Thk1632Epilog@0

; return code unsigned long --> unsigned long
	mov	edx,eax
	rol	edx,16

	lea	ecx, [esp+0]
	push	ecx
	push	dword ptr 1
	call	UnMapSLFixArray	;! Preserves eax & edx
;-------------------------------------
	jmp	ExitFlat_4

;===========================================================================
T_READREGISTRYSETTINGS label near32

; ebx+22   lpVoid

;-------------------------------------
; temp storage

	xor	eax,eax
	push	eax	;MapLS unfix temp
	push	eax	;MapLS unfix temp
; lpVoid inline mapping
;-------------------------------------
; *** BEGIN parameter packing

; lpVoid
; pointer struct --> struct
; inline mapping

; *** END   parameter packing
;-------------------------------------
; create new call frame and make the call

; lpVoid  from: struct
	mov	eax, [ebx+22]		; base address
	mov	[esp + 0],eax
	push	eax
	call	MapSLFix	
	push	eax

	call	K32Thk1632Prolog@0
	call	ReadRegistrySettings@4		; call 32-bit version
	call	K32Thk1632Epilog@0

; return code unsigned long --> unsigned long
	mov	edx,eax
	rol	edx,16

;-------------------------------------
; *** BEGIN parameter unpacking

	push	eax			; save return code
	push	edx			; save return code

; lpVoid
; pointer struct --> struct
	mov	eax,[ebx+22]		; base address
	or	eax,eax
	jz	L0			; skip if null


; structures are identical
; structures don't have pointers

L0:
	pop	edx			; restore return code
	pop	eax			; restore return code

; *** END   parameter packing
	lea	ecx, [esp+0]
	push	ecx
	push	dword ptr 1
	call	UnMapSLFixArray	;! Preserves eax & edx
;-------------------------------------
	jmp	ExitFlat_4

;===========================================================================
T_DISPLAYCONFIGDLG label near32

; ebx+26   param1
; ebx+22   lpVoid

;-------------------------------------
; temp storage

	xor	eax,eax
	push	eax	;MapLS unfix temp
	push	eax	;MapLS unfix temp
; lpVoid inline mapping
;-------------------------------------
; *** BEGIN parameter packing

; lpVoid
; pointer struct --> struct
; inline mapping

; *** END   parameter packing
;-------------------------------------
; create new call frame and make the call

; lpVoid  from: struct
	mov	eax, [ebx+22]		; base address
	mov	[esp + 0],eax
	push	eax
	call	MapSLFix	
	push	eax

; param1  from: unsigned long
	push	dword ptr [ebx+26]	; to unsigned long

	call	K32Thk1632Prolog@0
	call	DisplayConfigDlg@8		; call 32-bit version
	call	K32Thk1632Epilog@0

; return code unsigned long --> unsigned long
	mov	edx,eax
	rol	edx,16

;-------------------------------------
; *** BEGIN parameter unpacking

	push	eax			; save return code
	push	edx			; save return code

; lpVoid
; pointer struct --> struct
	mov	eax,[ebx+22]		; base address
	or	eax,eax
	jz	L1			; skip if null


; structures are identical
; structures don't have pointers

L1:
	pop	edx			; restore return code
	pop	eax			; restore return code

; *** END   parameter packing
	lea	ecx, [esp+0]
	push	ecx
	push	dword ptr 1
	call	UnMapSLFixArray	;! Preserves eax & edx
;-------------------------------------
	jmp	ExitFlat_8

ENDIF
END
