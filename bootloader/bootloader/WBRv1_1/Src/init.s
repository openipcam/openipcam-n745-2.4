;/******************************************************************************
; *
; * Copyright (c) 2003 Windond Electronics Corp.
; * All rights reserved.
; *
; * $Workfile: init.s $
; *
; * $Author: Yachen $
; ******************************************************************************/
;/*
; * $History: init.s $
; 
; *****************  Version 1  *****************
; User: Yachen       Date: 06/01/10   Time: 10:55a
; Created in $/W90P710/Applications/710bootloader/WBRv1_1/Src
; 710 Bootloader, without USB support
; 
; *****************  Version 1  *****************
; User: Yachen       Date: 06/01/04   Time: 2:28p
; Created in $/W90P710/Module Test Programs/FIRMWARE_710/WBRv1_1/Src
; Module test bootloader, removed decompress function in order to save
; memory space for LCD control
; 
; *****************  Version 2  *****************
; User: Wschang0     Date: 03/08/20   Time: 1:30p
; Updated in $/W90P710/FIRMWARE/WBRv1_1/Src
; Add VSS header
; */

        AREA    Init, CODE, READONLY

; --- Standard definitions of mode bits and interrupt (I & F) flags in PSRs

Mode_USR        EQU     0x10
Mode_FIQ        EQU     0x11
Mode_IRQ        EQU     0x12
Mode_SVC        EQU     0x13
Mode_ABT        EQU     0x17
Mode_UNDEF      EQU     0x1B
Mode_SYS        EQU     0x1F ; available on ARM Arch 4 and later

I_Bit           EQU     0x80 ; when I bit is set, IRQ is disabled
F_Bit           EQU     0x40 ; when F bit is set, FIQ is disabled


; --- System memory locations
RAM_Limit       EQU     0x8000          	; For unexpanded W90P710 board

UND_Stack		EQU		RAM_Limit
Abort_Stack		EQU		RAM_Limit-256
IRQ_Stack       EQU     RAM_Limit-512       ; followed by IRQ stack
FIQ_Stack       EQU     RAM_Limit-768       ; followed by IRQ stack
SVC_Stack       EQU     RAM_Limit-1024      ; SVC stack at top of memory
; add FIQ_Stack, ABT_Stack, UNDEF_Stack here if you need them
USR_Stack       EQU     0x400000	       ; followed by USR(SYS) stack

ROM_Start       EQU     0x7E000000          ; Base address of ROM after remapping

Clk_Skew		EQU		0xFFF01F00			; W90P710 clock skew control register
EBI_Ctrl		EQU		0xFFF01000			; W90P710 EBI Controle register
SDRAM_config0	EQU		0xFFF01008			; W90P710 SDCONF0

CAHCON			EQU		0xFFF02000			; W90P710 Cache control register

IDREG			EQU		0xFFF00000			; W90P710 CHIP ID

	KEEP

	ENTRY
	EXPORT	Reset_Go
Reset_Go
	; Disable Interrupt, This is for safe ...
	MRS	r0, CPSR
	ORR	r0, r0, #0xC0
	MSR	CPSR_c, r0

	; Disable cache, This is for safe ...
	MOV		r0, #0x0
	LDR		r1, =CAHCON
	STR		r0, [r1], #4
	MOV		r0, #0x87
	STR		r0, [r1]
11
	LDR		r0, [r1]
	CMP		r0, #0
	BNE		%B11

 	; Check if the system had been initialized
	LDR   r0, =SDRAM_config0
	LDR   r0, [r0]
	LDR	  r1, =0x800
	CMP   r0, r1
	BNE   %FT0

    ; Check version number of W90P710 to set the clock skew
    ; The clock skew of W90P710 version A should be 0x7A
    ; The clock skew of W90P710 version B should be 0x39
    LDR		r0, =IDREG
    LDR		r0, [r0]
    LDR		r1, =0x0F000000
    ANDS	r0,r0,r1
    BEQ		version0
    LDR		r1, =0x01000000
    CMP		r0, r1
    BEQ		version1
    B		unknow_version
version0
	LDR		r0, =0x0FF007A
    B		update_clkskew
version1    
	LDR		r0, =0x0FF0039
    B		update_clkskew
update_clkskew
	LDR		r1, =0xFFF01F00
	STR		r0,[r1]		
unknow_version

 
; Set mode to SVC, interrupts disabled (just paranoid)
	MRS   r0, cpsr
	BIC   r0, r0, #0x1F
	ORR   r0, r0, #0xD3
	MSR   cpsr_fc, r0

;	Configure the System Manger to remap the flash

; The Memory Bank Control Registers must be set using store multpiles
; Set up a stack in internal sram to preserve the original register contents

	LDR	r2, =remap_temp
	MOV	r1, pc
	LDR r3, =remap_EndSysMapJump
remap_temp
	MOV lr, #0
	CMP	r2, r1
	LDRGT	lr, =ROM_Start

	SUB	r3, r3, r2
	ADD	r1, r1, r3
	ADD	lr, lr, r1
	
; Load in the target values into the control registers
   	ADRL    r0, remap_SystemInitData
 	LDMIA   r0, {r1-r10}
 	LDR	r0, =EBI_Ctrl
	
; Now run critical jump code
 	STMIA   r0, {r1-r10}
 	MOV	pc, lr
remap_EndSysMapJump

	B	%FT0

remap_SystemInitData
	DCD	0x000530C0					; EBICON : not need to change.
	DCD	0xFC040080					; ROMCON(Flash) -Unknown type:0xFE000FF0, A29LV800: 0xFE020080, W28J160: 0xFE030080
	DCD	0x000090E3					; SDCONF0(SDRAM) - 8M(2Mx32): 0x000090E3, 4M(1Mx16x2):0x00009062, 16M(4Mx16x2):0x000090E4 			
	DCD	0x010090E3					; SDCONF1 - The same as SDCONF0 with proper base address   
	DCD	0x0000014B					; SDTIME0 - not need to change
	DCD	0x0000014B					; SDTIME1 - not need to change
	DCD 0x00000000					; EXT0CON - Non
	DCD 0x00000000					; EXT1CON - Non
	DCD 0x00000000					; EXT2CON - Non
	DCD 0xFE04424A					; EXT3CON - For flash ; NOTE!!: Timing setting must > 1 MCLK
	DCB "@SYSTEM_INIT_CFG"			; A key word for future useage
	ALIGN
0

        
; SDRAM is now at address 0x0.
; The exception vectors (in vectors.s) must be copied from ROM to the RAM
; The copying is done later by the C library code inside __main

        EXPORT  Reset_Handler

Reset_Handler

; --- Initialise stack pointer registers
        MSR     CPSR_c, #Mode_UNDEF:OR:I_Bit:OR:F_Bit
        LDR     SP, =UND_Stack

        MSR     CPSR_c, #Mode_ABT:OR:I_Bit:OR:F_Bit 
        LDR     SP, =Abort_Stack

        MSR     CPSR_c, #Mode_IRQ:OR:I_Bit:OR:F_Bit
        LDR     SP, =IRQ_Stack

        MSR     CPSR_c, #Mode_FIQ:OR:I_Bit:OR:F_Bit
        LDR     SP, =FIQ_Stack

        MSR     CPSR_c, #Mode_SYS:OR:I_Bit:OR:F_Bit
        LDR     SP, =USR_Stack

        MSR     CPSR_c, #Mode_SVC:OR:I_Bit:OR:F_Bit
        LDR     SP, =SVC_Stack



; Set up other stack pointers if necessary
        ; ...

; --- Initialise memory system
        ; ...
	; check memory type (only check if 1Mx16x2 SDRAM type)
	LDR		r0,=0x0
	STR		r0,[r0]
	LDR		r0,=0x800
	LDR		r1,=0x12345678
	STR		r1,[r0]
	LDR		r0,=0x0
	LDR		r2,[r0]
	CMP		r1,r2
	LDREQ	r0,=0xFFF01008 ;sdconf0
	LDREQ	r1,=0x00009062
	STREQ	r1, [r0], #4
	LDREQ	r1,=0x00809062
	STREQ	r1, [r0] 


	; check bank 1 (check if the SDRAM existed in bank 1)
	LDR		r0, =0xFFF0100C
	LDR		r1, [r0]
	LDR		r2, =0xFFF80000
	AND		r2, r2, r1
	MOV		r2, r2, lsr #1
	MOV		r0, #0
	STR		r0, [r2]
	LDR		r0, [r2]
	LDR		r0, =0xAABBCCDD
	STR		r0, [r2]
	LDR		r1, [r2, #0xc]
	LDR		r1, [r2]
	CMP		r0, r1
	LDRNE	r0, =0xFFF0100C
	MOVNE	r1, #0
	STRNE	r1, [r0]
        
; --- Initialise critical IO devices
        ; ...

; --- Initialise interrupt system variables here
        ; ...

; --- Now change to User mode and set up User mode stack.
;        MSR     CPSR_c, #Mode_USR:OR:I_Bit:OR:F_Bit ; No interrupts
;        LDR     SP, =USR_Stack

        IMPORT  __main, weak

; --- Now enter the C code
		LDR		r0, =__main
		CMP		r0, #0
		BNE		__main	; note use B not BL, because an application will never return this way
; --- If no C code, just try to exit by semihosed swi
		MOV		r0, #0x18	
		SWI		0x123456
99		B		%B99	; forever loop

	LTORG

exit

	END
