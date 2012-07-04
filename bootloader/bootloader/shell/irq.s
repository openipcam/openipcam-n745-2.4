;/******************************************************************************
; *
; * Copyright (c) 2003 Windond Electronics Corp.
; * All rights reserved.
; *
; * $Workfile: irq.s $
; *
; * $Author: Yachen $
; ******************************************************************************/
;/*
; * $History: irq.s $
; 
; *****************  Version 1  *****************
; User: Yachen       Date: 06/01/10   Time: 10:55a
; Created in $/W90P710/Applications/710bootloader/shell
; 710 Bootloader, without USB support
; 
; *****************  Version 1  *****************
; User: Yachen       Date: 06/01/04   Time: 2:27p
; Created in $/W90P710/Module Test Programs/FIRMWARE_710/shell
; Module test bootloader, removed decompress function in order to save
; memory space for LCD control
; 
; *****************  Version 2  *****************
; User: Wschang0     Date: 03/08/20   Time: 1:07p
; Updated in $/W90P710/FIRMWARE/shell
; Add VSS header
; */
TIMER_TISR	EQU 0xFFF81018
	
	EXPORT	timer_handler	
	EXPORT	EnableIRQ
	EXPORT	DisableIRQ

	CODE32
	AREA irq_function, CODE, READONLY
	
EnableIRQ
		STMFD	sp!,{r0}
        MRS		r0, CPSR
        BIC		r0, r0,#0x80
        MSR    CPSR_c, r0
		LDMFD	sp!,{r0}
		BX		lr
		
DisableIRQ	
		STMFD	sp!,{r0}
        MRS		r0, CPSR
        ORR		r0, r0,#0x80
        MSR    CPSR_c, r0
		LDMFD	sp!,{r0}
		BX		lr

timer_handler
	IMPORT	clkTck
	STMFD	sp!,{r3,r4}
	LDR		r3,=clkTck
	LDR		r4, [r3]
	ADD		r4, r4, #1
	STR		r4, [r3]
	LDR		r3, =TIMER_TISR
	;LDR		r4, =0xFFFFFFFE;
	LDR		r4, =0x1;cmn, write 1 to clear
	STR		r4, [r3]
	LDMFD	sp!,{r3,r4}
	SUBS	pc, lr, #4
	
	END