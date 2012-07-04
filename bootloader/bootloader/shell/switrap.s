	INCLUDE except_h.s

	EXPORT	WB_TrapSWI		; SWI exception handler

	IMPORT	uputchar
	IMPORT  print_sys_buf
	IMPORT 	fil_sys_buf
	IMPORT 	clock, WEAK

	IMPORT  FileCount	; Count of opened files.
	IMPORT	heap_info	; Variables holding above values
	IMPORT	semi_stack	; The default stack for user program
	IMPORT command_buffer
	

 IF :DEF: CHAIN_VECTORS 
 IF CHAIN_VECTORS = 1
   	IMPORT	Chainir_ChainWalk
	IMPORT  Chainiv_OwnerChain
	IMPORT	Chainir_InfoSWI
	IMPORT	Chainir_ChainEntry
 ENDIF
 ENDIF

THUMB_AWARE	EQU	1
AIC_IMR		EQU 0xFFF82114
AIC_MECR	EQU	0xFFF82120
AIC_MDCR	EQU	0xFFF82124


	AREA	WB_Traps, CODE, READONLY


; SWI handler - currently only handles switch to SVC mode.
WB_TrapSWI
	; SWI switches to SVC mode, with no IRQs (ARM Architecture Reference)
	STMFD	sp!, {r0-r1}

 IF :DEF: THUMB_AWARE   ; SWIs are different in THUMB mode
	MRS	r0, spsr		; Get spsr
	TST	r0, #PSR_T_bit		; Occurred in Thumb state?
	LDRNEH	r0, [lr,#-2]		; Yes: Load halfword and...
	BICNE	r0, r0, #0xFF00		; ...extract comment field
	LDREQ	r0, [lr,#-4]		; No: Load word and...
	BICEQ	r0, r0, #0xFF000000	; ...extract comment field
   
        ; r0 now contains SWI number
        
	LDR	r1, =SWI_WB		
	CMP	r0, r1
	BEQ	swi_correct

	LDR	r1, =SWI_WB_Thumb
	CMP	r0, r1
	BEQ	swi_correct

	; If we get here we can't service the SWI so return unknown
	
	LDMFD	sp!, {r0-r1}		; restore register
	B	WB_ExitSWI		; Unknown SWI

swi_correct
	LDMFD	sp!, {r0-r1}		; restore register


 ELSE                    ; some processors are not Thumb aware
	LDR	r0, [lr, #-4]		; Read SWI instruction
	BIC	r0, r0, #0xff000000	; extract the SWI number

	LDR	r1, =SWI_WB
	CMP	r0, r1
	LDMFD	sp!, {r0-r1}		; restore register
	BNE	WB_ExitSWI		; Unknown SWI
 ENDIF

	CMP	r0, #WB_SWIreason_EnterSVC
	BEQ	WB_SWIEnterOS	; return in SVC mode

  IF :DEF: CHAIN_VECTORS 
  IF CHAIN_VECTORS = 1
	CMP		r0, #SYS_AGENTINFO
	BEQ		Chainir_InfoSWI

	CMP		r0, #SYS_VECTORCHAIN
	BEQ		Chainir_ChainEntry

	STMFD	sp!, {r0, r1, lr}	; We must handle this since nobody else cares.
	MOV		r1, sp
	ADD		r1, r1, #8
	LDR		r0, =Chainiv_OwnerChain
	LDR		r0, [r0, #0x8]
	CMP		r0, #0
	BEQ		WB_SWI		
	BL		Chainir_ChainWalk
WB_SWI
	LDMFD	sp!, {r0, r1, lr}	; We must handle this since nobody else cares.

  ENDIF	
  ENDIF

	CMP r0, #SYS_FILE_OPEN
	BEQ WB_Sys_FileOpenSWI

	CMP r0, #SYS_READ_SWI
	BEQ WB_Sys_ReadSWI

	CMP r0, #SYS_WRITE_SWI
	BEQ WB_Sys_WriteSWI

	CMP r0, #WB_SWI_SYS_WRITEC
	BEQ WB_ttywrch_SWI

	CMP r0, #SYS_CLOCK
	BEQ WB_readtime_SWI

	CMP r0, #0x200
	BEQ sh_run_SWI

	CMP r0, #0x18
	BEQ sh_return_SWI
	
	CMP r0, #0x15
	BEQ sh_get_command_SWI
	
	CMP	r0, #WB_SWI_SYS_HEAPINFO
	BNE	WB_ExitSWI		; Unknown SWI


	; Load area pointed to by r1 with heap and stack info
	STMFD	sp!, {r0-r4}
	LDR	r0, [r1]		; Pointer to variables
	LDR	r4, =heap_info
	LDR	r3, [r4], #4	; heap_base
	LDR	r2, [r4], #4	; heap_limit
	STR	r3, [r0], #4	; heap_limit
	STR	r2, [r0], #4	; heap_base
	LDR	r2, [r4], #4	; stack_base
	LDR	r3, [r4], #4	; stack_limit
	STR	r2, [r0], #4	; stack_base  ; use the real stack pointer
	STR	r3, [r0]		; stack_limit
	LDMFD	sp!, {r0-r4}		; restore register

WB_ExitSWI
	MOV	r0, #0			; Pretend it was all okay
	MOVS	pc, lr			; just ignore SWI
      ; MOVS pc.. will return the Thumb state if called
      ;           from that state
      
WB_SWIEnterOS
	STMFD	sp!, {r0}
	MRS	r0, SPSR		; Get spsr
	TST	r0, #PSR_T_bit		; Occurred in Thumb state?
	ORRNE	r0, r0, #0x13		; Put into SVC mode
	MSRNE	SPSR_cxsf, r0		; Save SPSR
	LDMFD	sp!, {r0}		; restore register
	MOVNES	pc, lr
	MOV	pc, lr

WB_Sys_FileOpenSWI
	STMFD	sp!, {r4, lr}
	LDR	r0, =FileCount
	LDR	r4, [r0]		; Test the number of opened files
;	ADD 	r4, r4,#0x01; FileCount is no use yet, just clean it to avoid problem. CWS
	STR	r4, [r0]
	CMP	r4, #0x03		; If < 3 must be stdin, stdout, stderr
	MOVGT	r0, #0xffffffff 
	MOVLT	r0, #0x00000000
	LDMFD	sp!, {r4, lr}		; restore registers and return
	MOVS	pc, lr

WB_ttywrch_SWI
	STMFD	sp!, {r0, lr}
	LDR	r0, [r1]
	BL	uputchar		; Print single character from message.
	LDMFD	sp!, {r0, lr}		; restore registers and return
	MOVS	pc, lr

WB_Sys_WriteSWI
	STMFD	sp!, {r1-r3, lr}
	;MOV	r0, r2
	LDR		r0, [r1,#8]
	LDR 	r1, [r1,#4]
	BL 	print_sys_buf		; Print the stream buffer.
	LDMFD	sp!, {r1-r3, lr}	; restore registers and return
	MOVS	pc,lr

WB_Sys_ReadSWI
	STMFD	sp!, {lr}
	;MOV	r0, r2
	LDR		r0, [r1,#8]
	LDR 	r1, [r1,#4]
	BL 	fil_sys_buf		; Fill the stream buffer.
	LDMFD	sp!, {lr}		; restore registers and return
	MOVS	pc, lr

WB_readtime_SWI		; The time function is not valid now
	STMFD	sp!, {r1-r3, lr}
	MRS		r1, SPSR
	BIC		r1, r1, #0x80
	MSR		SPSR_cxsf, r1	; Enable interrupt for clock
	BL	clock				; What time is it, Mr. Wolf?
	LDMFD	sp!, {r1-r3, lr}	; restore registers and return
	MOVS	pc, lr

sh_run_SWI
	STMFD	sp!, {r2-r12,lr}
	; mask all interrupt from AIC controller
	LDR		r2, =AIC_IMR
	LDR		r2, [r2]
	MRS		r3, SPSR
	STMFD	sp!, {r2,r3}	; store the AIC_IMR,spsr for semihosted return
	LDR		r2, =AIC_MDCR
	LDR		r3, =0x7FFFE	
	STR		r3, [r2]	; mask all interrupt source
	
	MOV		r2, sp
	LDR		sp, =semi_stack
	LDR		sp, [sp]	; set to semihosted stack
	STMFD	sp!, {r2}		; store the stack of boot loader
	MOV		pc, r1			; run the program
	ldr r1,=0xfff83000
	mov r2,#0x31
	strb     r2,[r1,#0]				
	LDMFD	sp!, {r2-r12,pc}
	
sh_return_SWI

	LDR		sp, =semi_stack
	LDR		sp, [sp]
	LDR		sp, [sp,#-4]
	LDMFD	sp!, {r2,r3}	; restore the AIC_IMR, spsr
	MSR		SPSR_cxsf, r3	; restore the SPSR
	LDR		r3, =AIC_MDCR
	LDR		r4, =0x7FFE	
	STR		r4, [r3]
	LDR		r3, =AIC_MECR
	STR		r2, [r3]	; retore the AIC mask register
	LDMFD	sp!, {r2-r12,pc}^

sh_get_command_SWI
	LDR		r0, =command_buffer
	LDR		r1, [r1]
	MOV		r3, #0x100
0
	LDRB	r2, [r0],#1
	STRB	r2, [r1],#1
	SUBS	r3, r3, #1
	BNE		%B0
	MOV		r0, #0
	MOVS	pc, lr

	END





