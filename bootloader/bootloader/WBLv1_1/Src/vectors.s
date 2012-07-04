;/******************************************************************************
; *
; * Copyright (c) 2003 Windond Electronics Corp.
; * All rights reserved.
; *
; * $Workfile: vectors.s $
; *
; * $Author: Yachen $
; ******************************************************************************/
;/*
; * $History: vectors.s $
; 
; *****************  Version 1  *****************
; User: Yachen       Date: 06/01/10   Time: 10:55a
; Created in $/W90P710/Applications/710bootloader/WBLv1_1/Src
; 710 Bootloader, without USB support
; 
; *****************  Version 1  *****************
; User: Yachen       Date: 06/01/04   Time: 2:28p
; Created in $/W90P710/Module Test Programs/FIRMWARE_710/WBLv1_1/Src
; Module test bootloader, removed decompress function in order to save
; memory space for LCD control
; 
; *****************  Version 2  *****************
; User: Wschang0     Date: 03/08/20   Time: 1:27p
; Updated in $/W90P710/FIRMWARE/WBLv1_1/Src
; Add VSS header
; */
	KEEP
    AREA Vect, CODE, READONLY
; These are example exception vectors and exception handlers

; Where there is ROM fixed at 0x0 , these are hard-coded at 0x0.
; Where ROM/RAM remapping occurs , these are copied from ROM to RAM.
; The copying is done automatically by the C library code inside __main.

; *****************
; Exception Vectors
; *****************
        ENTRY
        LDR     PC, Reset_Addr
        LDR     PC, Undefined_Addr
        LDR     PC, SWI_Addr
        LDR     PC, Prefetch_Addr
        LDR     PC, Abort_Addr
        NOP                             ; Reserved vector
        LDR     PC, IRQ_Addr
        LDR     PC, FIQ_Addr
        
        IMPORT  Reset_Handler           ; In init.s
        IMPORT  WB_TrapSWI			; for Semihosted system
        
Reset_Addr      DCD     Reset_Handler
Undefined_Addr  DCD     Undefined_Handler
SWI_Addr        DCD     WB_TrapSWI
Prefetch_Addr   DCD     Prefetch_Handler
Abort_Addr      DCD     Abort_Handler
                DCD     0               ; Reserved vector
IRQ_Addr        DCD     IRQ_Handler
FIQ_Addr        DCD     FIQ_Handler


; ************************
; Exception Handlers
; ************************

; Some dummy handlers do not do anything useful in this example.
; They are set up here for completeness.
	IMPORT ExceptionStatus
	IMPORT ExceptionLinkReg
	IMPORT sh, WEAK


Undefined_Handler
        B       Undefined_Handler
SWI_Handler
        B       SWI_Handler     
Prefetch_Handler
	STMFD	sp!, {r0, r1}
	LDR		r0,=ExceptionStatus
	MOV		r1,#0x08
	STR		r1, [r0]
	LDR		r0,=ExceptionLinkReg
	SUB		r1, lr, #4
	STR		r1, [r0]
	LDR		lr, =sh
	CMP		lr, #0
	BEQ		Reset_Handler
	LDMFD	sp!, {r0, r1}
	LDR		r0, =sh
	SUBS	pc, lr, #0	; Just ignore the data abort and go ahead.	

Abort_Handler
	STMFD	sp!, {r0, r1}
	LDR		r0,=ExceptionStatus
	MOV		r1,#0x10
	STR		r1, [r0]
	LDR		r0,=ExceptionLinkReg
	SUB		r1, lr, #8
	STR		r1, [r0]
	LDMFD	sp!, {r0, r1}
	SUBS	pc, lr, #4	; Just ignore the data abort and go ahead.	

        B       Abort_Handler
IRQ_Handler
        B       IRQ_Handler
FIQ_Handler
        B       FIQ_Handler
        
        END

