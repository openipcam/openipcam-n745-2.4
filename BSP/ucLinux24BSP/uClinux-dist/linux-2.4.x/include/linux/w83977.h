/****************************************************************************
 *                                                                                                                           *
 * Copyright (c) 2004 - 2006 Winbond Electronics Corp. All rights reserved.                          *
 *                                                                                                                           *
 ***************************************************************************/
 
/****************************************************************************
 * 
 * FILENAME
 *     linux-2.4.x/include/linux/w83977.h
 *
 * VERSION
 *     1.0
 *
 * DESCRIPTION
 *     This is the head file of W83977AF
 *
 * DATA STRUCTURES
 *     None
 *
 * FUNCTIONS
 *     None
 *
 * HISTORY
 *     13/2/2004		 Ver 1.0 Created by PC34 MCLi
 *
 * REMARK
 *     None
 * MODIFY
 *	  16/11/2004	by PC34 MCLi
 *	  	Delete the W83977EF driver codes
 **************************************************************************/

#ifndef __W83977_H_
#define __W83977_H_

#include <asm/arch/hardware.h>

#define VPint   			*(volatile unsigned int *)
#define VPshort 			*(volatile unsigned short *)
#define VPchar  			*(volatile unsigned char *)

#define outdw(r,v)		(VPint(r)=(v))
#define indw(r)			(VPint(r))

/* W83977AF UART Ports */
#ifndef UART_PORTA
#define UART_PORTA 2
#endif

#ifndef UART_PORTB
#define UART_PORTB 3
#endif

#ifndef UART_PORTC
#define UART_PORTC 6
#endif

#define PORT_W83977	        15
#define PORT_REF_CLOCK		1846100 /* 1.8461MHZ */

#define W83977_BASE (W83977AF_BASE_ADDR|0x80000000)
#define W83977AF_BASE W83977_BASE

#define PARA_BASE W83977_BASE+0x378

/* W83977 Default UART base address */
#define UART_BASEA	W83977_BASE+0x3F8
#define UART_BASEB	W83977_BASE+0x2F8
#define UART_BASEC	W83977_BASE+0x4F8

/* UART A */
#define PORTA_RBR	UART_BASEA + 0		/* R */
#define PORTA_TBR	UART_BASEA + 0		/* W */
#define PORTA_ICR	UART_BASEA + 1		/* R & W */
#define PORTA_ISR	UART_BASEA + 2		/* R */
#define PORTA_UFR	UART_BASEA + 2		/* W */
#define PORTA_UCR	UART_BASEA + 3		/* R & W */
#define PORTA_HCR	UART_BASEA + 4		/* R & W */
#define PORTA_USR	UART_BASEA + 5		/* R & W */
#define PORTA_HSR	UART_BASEA + 6		/* R & W */
#define PORTA_UDR	UART_BASEA + 7		/* R & W */
#define PORTA_BLL	UART_BASEA + 0		/* R & W */
#define PORTA_BLH	UART_BASEA + 1		/* R & W */

/* UART B */
#define PORTB_RBR	UART_BASEB + 0		/* R */
#define PORTB_TBR	UART_BASEB + 0		/* W */
#define PORTB_ICR	UART_BASEB + 1		/* R & W */
#define PORTB_ISR	UART_BASEB + 2		/* R */
#define PORTB_UFR	UART_BASEB + 2		/* W */
#define PORTB_UCR	UART_BASEB + 3		/* R & W */
#define PORTB_HCR	UART_BASEB + 4		/* R & W */
#define PORTB_USR	UART_BASEB + 5		/* R & W */
#define PORTB_HSR	UART_BASEB + 6		/* R & W */
#define PORTB_UDR	UART_BASEB + 7		/* R & W */
#define PORTB_BLL	UART_BASEB + 0		/* R & W */
#define PORTB_BLH	UART_BASEB + 1		/* R & W */

/* UART C */
#define PORTC_RBR	UART_BASEC + 0		/* R */
#define PORTC_TBR	UART_BASEC + 0		/* W */
#define PORTC_ICR	UART_BASEC + 1		/* R & W */
#define PORTC_ISR	UART_BASEC + 2		/* R */
#define PORTC_UFR	UART_BASEC + 2		/* W */
#define PORTC_ADCR1	UART_BASEC + 2		/* R & W */
#define PORTC_UCR	UART_BASEC + 3		/* R & W */
#define PORTC_HCR	UART_BASEC + 4		/* R & W */
#define PORTC_ADCR2	UART_BASEC + 4		/* R & W */
#define PORTC_USR	UART_BASEC + 5		/* R & W */
#define PORTC_HSR	UART_BASEC + 6		/* R & W */
#define PORTC_UDR	UART_BASEC + 7		/* R & W */
#define PORTC_BLL	UART_BASEC + 0		/* R & W */
#define PORTC_BLH	UART_BASEC + 1		/* R & W */

/* Configuration port and key */
#define AF_CONFIG_PORT		W83977_BASE+0x3f0
#define AF_INDEX_PORT		AF_CONFIG_PORT
#define AF_DATA_PORT		W83977_BASE+0x3f1

/* Compatible PnP registers */
#define EFER_REG	0x3F0         /* Extended Function Enable Register */
#define EFIR_REG	EFER_REG      /* Extended Function Index Register */
#define EFDR_REG	(EFER_REG+1)  /* Extended Function Data Register */

#define CR20		0x20
#define CR21		0x21

#define KBC_DEV				5	/* logical device 5 */

/* W83977 definitions */
#define W83977AF_DEV_ID		0x97     /* device ID */
#define W83977AF_DEV_REV	0x77     /* revision ID */
#define W83977EF_DEV_ID		0x52     /* device ID */
#define W83977EF_DEV_REV	0xF6	 /* device ID */

/* I/O registers */
#define KBC_STATUS_REG		*((volatile unsigned char *) (W83977_BASE + 0x64))  /* input */
#define KBC_COMMAND_REG		*((volatile unsigned char *) (W83977_BASE + 0x64))  /* output */
#define KBC_OUTBUF_REG		*((volatile unsigned char *) (W83977_BASE + 0x60))  /* output */
#define KBC_DATA_REG			*((volatile unsigned char *) (W83977_BASE + 0x60))  /* input */

#define KBC_INPUT_BUF_FULL	0x02
#define KBC_OUTPUT_BUF_FULL	0x01


#endif
