;/******************************************************************************
; *
; * Copyright (c) 2003 Windond Electronics Corp.
; * All rights reserved.
; *
; * $Workfile: except_h.s $
; *
; * $Author: Yachen $
; ******************************************************************************/
;/*
; * $History: except_h.s $
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
 IF :LNOT: :DEF: __except_h
__except_h	EQU	1


NoIRQ		EQU	0x80		; Bit 7 of cspr
NoFIQ		EQU	0x40		; Bit 6 of cspr
NoINTS		EQU	(NoIRQ | NoFIQ) ; Both
MaskINTS	EQU	NoINTS

AllIRQs		EQU	0xFF		; Mask for interrupt controller

ResetV		EQU	0x00
UndefV		EQU	0x04
SwiV		EQU	0x08
PrefetchV	EQU	0x0c
DataV		EQU	0x10	
IrqV		EQU	0x18
FiqV		EQU	0x1C

ModeMask	EQU	0x1F		; /* Processor mode in CPSR */

SVC32Mode	EQU	0x13
IRQ32Mode	EQU	0x12
FIQ32Mode	EQU	0x11
User32Mode	EQU	0x10
Sys32Mode	EQU	0x1F
;; /* Error modes */
Abort32Mode	EQU	0x17
Undef32Mode	EQU	0x1B

PSR_T_bit	EQU	0x20

 IF {FALSE}
 ;/* If the memory size is too small in W90P710 */
 ;/* , we have to decrease the stack size */
UserStackSize	EQU	0x200
SVCStackSize	EQU	0x40
IRQStackSize	EQU	0x20
UndefStackSize	EQU	0x2
;/* Not currently used, but defined anyway */
FIQStackSize	EQU	0x4
AbortStackSize	EQU	0x4
 ELSE 
UserStackSize	EQU	0x20000
SVCStackSize	EQU	0x4000
IRQStackSize	EQU	0x2000
UndefStackSize	EQU	0x200
;/* Not currently used, but defined anyway */
FIQStackSize	EQU	0x400
AbortStackSize	EQU	0x400
 ENDIF

SWI_WB			EQU	0x123456
SWI_WB_Thumb			EQU	0xAB


SYS_FILE_OPEN			EQU	0x01
SYS_FILE_CLOSE			EQU	0x02
SYS_AGENTINFO			EQU 0x35
SYS_VECTORCHAIN		EQU 0x36
WB_SWI_SYS_WRITEC		EQU	0x03
WB_SWI_SYS_WRITE0		EQU	0x04
SYS_WRITE_SWI			EQU	0x05
SYS_READ_SWI			EQU	0x06
WB_SWI_SYS_READC		EQU	0x07
SYS_CLOCK			EQU	0x10
WB_SWI_SYS_HEAPINFO		EQU	0x16
WB_SWIreason_EnterSVC	EQU	0x17

WB_SWIreason_ReportException	EQU	0x18
ADP_Stopped_ApplicationExit	EQU	0x20026

 ENDIF

	END				; End of file

