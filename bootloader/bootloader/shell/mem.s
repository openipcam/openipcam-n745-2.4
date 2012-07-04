;/******************************************************************************
; *
; * Copyright (c) 2003 Windond Electronics Corp.
; * All rights reserved.
; *
; * $Workfile: mem.s $
; *
; * $Author: Yachen $
; ******************************************************************************/
;/*
; * $History: mem.s $
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
; *****************  Version 4  *****************
; User: Wschang0     Date: 04/03/19   Time: 5:09p
; Updated in $/W90P710/FIRMWARE/shell
; Fix memory detection bug
; 
; *****************  Version 3  *****************
; User: Wschang0     Date: 03/08/20   Time: 1:39p
; Updated in $/W90P710/FIRMWARE/shell
; *
; ******************  Version 2  *****************
; * User: Wschang0     Date: 03/08/20   Time: 1:07p
; * Updated in $/W90P710/FIRMWARE/shell
; * Add VSS header
; */


		
STEP_SIZE	EQU		0x100000
SIGNATURE	EQU		0x12345a15

	EXPORT	MemSize
	
	AREA memory_asm, CODE, READONLY

; UINT32 MemSize(VOID), return the memory size
MemSize	
	STMFD	sp!,{r1-r6,lr}
	;backup data reset & abort vector
	MOV		r0, #0x0
	LDR		r5, [r0,#0x10]
	LDR		r6, [r0]
	
	;install data abort vector
	LDR		r1, =0xE25EF004
	STR		r1, [r0,#0x10]

	;check memory size
	LDR		r1,=STEP_SIZE
	LDR		r2,=SIGNATURE
	MOV		r3, r0
0
	ADD		r3, r3, r1
	STR		r2, [r3]
	STR		r0, [r0]
	MOV		r4, r0
	LDR		r4, [r3]
	CMP		r2, r4
	BEQ		%B0
	;retore data reset & abort vector
	STR		r5, [r0,#0x10]
	STR		r6, [r0]
	; return memory size
	MOV		r0, r3
	
	LDMFD	sp!,{r1-r6,pc}

	END