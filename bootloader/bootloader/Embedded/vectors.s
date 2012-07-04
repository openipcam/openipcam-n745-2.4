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
; Created in $/W90P710/Applications/710bootloader/Embedded
; 710 Bootloader, without USB support
; 
; *****************  Version 1  *****************
; User: Yachen       Date: 06/01/04   Time: 2:27p
; Created in $/W90P710/Module Test Programs/FIRMWARE_710/Embedded
; Module test bootloader, removed decompress function in order to save
; memory space for LCD control
; 
; *****************  Version 2  *****************
; User: Wschang0     Date: 03/08/20   Time: 1:22p
; Updated in $/W90P710/FIRMWARE/Embedded
; Add VSS header
; */

	AREA Vect, CODE, READONLY


; *****************
; Exception Vectors
; *****************

; Note: LDR PC instructions are used here, though branch (B) instructions
; could also be used, unless the ROM is at an address >32MB.

;        ENTRY
        EXPORT	Vector_Table
Vector_Table		
        LDR     PC, Reset_Addr
        LDR     PC, Undefined_Addr
        LDR     PC, SWI_Addr
        LDR     PC, Prefetch_Addr
        LDR     PC, Abort_Addr
        NOP                             ; Reserved vector
        LDR     PC, IRQ_Addr
        LDR     PC, FIQ_Addr
        
        IMPORT  Reset_Handler           ; In init.s
        
Reset_Addr      DCD     Reset_Handler
Undefined_Addr  DCD     Undefined_Handler
SWI_Addr        DCD     SWI_Handler
Prefetch_Addr   DCD     Prefetch_Handler
Abort_Addr      DCD     Abort_Handler
				DCD		0
IRQ_Addr        DCD     IRQ_Handler
FIQ_Addr        DCD     FIQ_Handler


; ************************
; Exception Handlers
; ************************

; The following dummy handlers do not do anything useful in this example.
; They are set up here for completeness.

Undefined_Handler
        B       Undefined_Handler
SWI_Handler
        B       SWI_Handler     
Prefetch_Handler
        B       Prefetch_Handler
Abort_Handler
        B       Abort_Handler
		NOP
IRQ_Handler
        B       IRQ_Handler
FIQ_Handler
        B       FIQ_Handler
        
        END

