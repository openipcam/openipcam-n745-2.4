#ifndef _PLATFORM_H
#define _PLATFORM_H
//------------------------------------------------------------------------------------------

#define PLATFORM_ID	0x90710
#define W90P710	1

/****************************************************************************************************
 *                                                               
 * I/O routines  
 *
 ****************************************************************************************************/
#define VPint(x)   			(*(volatile unsigned int *)(x))
#define VPshort(x) 			(*(volatile unsigned short *)(x))
#define VPchar(x)  			(*(volatile unsigned char *)(x))

#define inpw(port)			VPint(port)
#define outpw(port,x)		VPint(port)=(x)
#define inph(port)			VPshort(port)
#define outph(port,x)		VPshort(port)=(x)
#define inpb(port)			VPchar(port)
#define outpb(port,x)		VPchar(port)=(x)



#define AHBBASE	0xFFF00000
#define APBBASE	0xFFF80000

// ebi
#define ROMCON	VPint(AHBBASE+0x1004)
#define EXT3CON	VPint(AHBBASE+0x1024)

// cache
#define CAHCNF	VPint(AHBBASE+0x2000)
#define CAHCON	VPint(AHBBASE+0x2004)


// flash
#define FLASH_BASE 0x7F000000
#define FLASH_BLOCK_SIZE 0x10000

// system config
#define CLKSEL	VPint(AHBBASE+0x000C)

// timer
#define T_TCR0	VPint(APBBASE+0x1000)
#define T_TCR1	VPint(APBBASE+0x1004)
#define T_TICR0	VPint(APBBASE+0x1008)
#define T_TICR1	VPint(APBBASE+0x100c)
#define T_TDR0	VPint(APBBASE+0x1010)
#define T_TDR1	VPint(APBBASE+0x1014)
#define T_TISR	VPint(APBBASE+0x1018)
#define TIF0	0x1
#define TIF1	0x2


/*    Interrupt Controller Group   */
#define AIC_BASE	VPint(APBBASE+0x2000)
#define AIC_SCR0	AIC_BASE
#define AIC_SCR1	VPint(APBBASE+0x2004)
#define AIC_SCR2	VPint(APBBASE+0x2008)
#define AIC_SCR3	VPint(APBBASE+0x200C)
#define AIC_SCR4	VPint(APBBASE+0x2010)
#define AIC_SCR5	VPint(APBBASE+0x2014)
#define AIC_SCR6	VPint(APBBASE+0x2018)
#define AIC_SCR7	VPint(APBBASE+0x201C)
#define AIC_SCR8	VPint(APBBASE+0x2020)
#define AIC_SCR9	VPint(APBBASE+0x2024)
#define AIC_SCR10	VPint(APBBASE+0x2028)
#define AIC_SCR11	VPint(APBBASE+0x202C)
#define AIC_SCR12	VPint(APBBASE+0x2030)
#define AIC_SCR13	VPint(APBBASE+0x2034)
#define AIC_SCR14	VPint(APBBASE+0x2038)
#define AIC_SCR15	VPint(APBBASE+0x203C)
#define AIC_SCR16	VPint(APBBASE+0x2040)
#define AIC_SCR17	VPint(APBBASE+0x2044)
#define AIC_SCR18	VPint(APBBASE+0x2048)

#define AIC_IRSR	VPint(APBBASE+0x2100)
#define AIC_IASR	VPint(APBBASE+0x2104)
#define AIC_ISR		VPint(APBBASE+0x2108)
#define AIC_IPER	VPint(APBBASE+0x210C)
#define AIC_ISNR	VPint(APBBASE+0x2110)
#define AIC_IMR		VPint(APBBASE+0x2114)
#define AIC_OISR	VPint(APBBASE+0x2118)
#define AIC_MECR	VPint(APBBASE+0x2120)
#define AIC_MDCR	VPint(APBBASE+0x2124)
#define AIC_SSCR	VPint(APBBASE+0x2128)
#define AIC_SCCR	VPint(APBBASE+0x212C)
#define AIC_EOSCR	VPint(APBBASE+0x2130)


// serial port
#define UART_RBR	VPchar(APBBASE+0x0000)
#define UART_THR	VPchar(APBBASE+0x0000)
#define UART_IER	VPchar(APBBASE+0x0004)
#define MSIE	(0x1 <<  3	)/*   Modem status interrupt enable (Irpt_MOS)   */
#define RLSIE	(0x1 <<  2	)/*   Receive line status interrupt enable (Irpt_RLS)   */
#define THREIE	(0x1 <<  1	)/*   Transmit holding register empty interrupt enable (Irpt_THRE)   */
#define RDAIE	0x1	/*   Receive data available interrupt enable (Irpt_RDA)   */
#define rUART_IER	(MSIE+RLSIE+THREIE+RDAIE)
#define UART_DLL	VPchar(APBBASE+0x0000)
#define UART_DLM	VPchar(APBBASE+0x0004)
#define UART_IIR	VPchar(APBBASE+0x0008)


/*   UART FCR  */
#define UART_FCR	VPchar(APBBASE+0x0008)
#define RFITL	(0x2 <<  6	)/*  RX FIFO Interrupt trigger level  */
/*  00= 1bytes, 01= 4bytes, 10= 8bytes, 11= 14bytes  */
#define TFR	(0x1 <<  2	)/*  TX FIFO Reset.  */
/*  0= no effect, 1= reset  */
#define RFR	(0x1 <<  1	)/*  RX FIFO Reset.  */
/*  0= no effect, 1= reset  */
#define FME	0x1			/*  FIFO mode enable  */
/*  0= can't write command, 1= write command to FCR  */
#define rUART_FCR	(RFITL+TFR+RFR+FME)

/*   UART_LCR   */
#define UART_LCR	VPchar(APBBASE+0x000C)
#define DLAB	(0x1 <<  7)
#define BCB	(0x1 <<  6)
#define SPE	(0x1 <<  5)
#define EPE	(0x1 <<  4)
#define PBE	(0x1 <<  3)
#define NSB	(0x1 <<  2)
#define UART_5bit	0x0
#define UART_6bit	0x1
#define UART_7bit	0x2
#define UART_8bit	0x3

#define UART_MSR	VPchar(APBBASE+0x0018)
#define UART_TOR	VPchar(APBBASE+0x001C)
#define TOIE	0x80
#define RX_FIFO_LEVEL_1	(0x00)
#define RX_FIFO_LEVEL_4	(0x40)
#define RX_FIFO_LEVEL_8	(0x80)
#define RX_FIFO_LEVEL_14	(0xC0)


#define UART_MCR	VPchar(APBBASE+0x0010)
#define UART_LSR	VPchar(APBBASE+0x0014)
#define ERR_RX	(1 <<  7)
#define TRANS_EMPTY	(1 <<  6)
#define TRANS_HOLD_REG_EMPTY	(1 <<  5)
#define BREAK_INT	(1 <<  4)
#define FRAME_ERR	(1 <<  3)
#define PARITY_ERR	(1 <<  2)
#define OVER_RUN	(1 <<  1)
#define RX_FIFO_DATA_READY	0x1



/*   UART primitives   */
#define GET_STATUS(p)	UART_LSR
#define RX_DATA(s)     	((s) & RX_FIFO_DATA_READY)
#define GET_CHAR(p)		(UART_RBR & 0xFF)
#define TX_READY(s)    	((s) & TRANS_EMPTY)
#define PUT_CHAR(p,c)  	(UART_THR = (c&0xFF))
#define UART_IERSET(v)	(UART_IER = (v) )
#define UART_FIFOSET(v) (UART_FCR = (v) )
#define UART_TORSET(v)  (UART_TOR = (v) )



/*for 15 MHz clock*/
#define ARM_BAUD_1200	(779)
#define ARM_BAUD_2400	(389)
#define ARM_BAUD_4800	(193)
#define ARM_BAUD_9600	(96)
#define ARM_BAUD_14400	(65)
#define ARM_BAUD_19200	(47)
#define ARM_BAUD_28800	(32)
#define ARM_BAUD_38400	(22)
#define ARM_BAUD_57600	(14)
#define ARM_BAUD_115200	(6)
#define ARM_BAUD_230400	(2)
#define ARM_BAUD_460800	(0)

/*for 14.318 MHz clock*/
/*ARM_BAUD_1200	EQU	(744)*/
/*ARM_BAUD_2400	EQU	(371)*/
/*ARM_BAUD_4800	EQU	(184)*/
/*ARM_BAUD_9600	EQU	(91)*/
/*ARM_BAUD_19200	EQU	(45)*/
/*ARM_BAUD_38400	EQU	(21)*/
/*ARM_BAUD_57600	EQU	(14)*/
/*ARM_BAUD_115200	EQU	(6)*/
/*ARM_BAUD_230400	EQU	(2)*/
/*ARM_BAUD_460800	EQU	(0)*/


// GPIO

#define GPIO_CFG	VPint(APBBASE+0x3000)
#define GPIO_DIR	VPint(APBBASE+0x3004)
#define GPIO_DATAOUT	VPint(APBBASE+0x3008)
#define GPIO_DATAIN	VPint(APBBASE+0x300C)
#define DEBNCE_CTRL	VPint(APBBASE+0x3010)








// for SWI
#define EXEC_SWI_NUM	0x200
#ifdef __thumb
# define SemiSWI 0xAB
#else
# define SemiSWI 0x123456
#endif
__swi(SemiSWI) void swi_run(int op, int addr);

#define BL_BOOTLOADER_BLOCK_SIZE	0x10000
#define BL_PHY			NET_PHY
#define BL_IC_PLUS		NET_IC_PLUS
#define BL_MARVELL6052	NET_MARVELL6052
#define BL_DEFAULT_MAC0_ADDR	{0x00,0x00,0x00,0x00,0x00,0x01}
#define BL_DEFAULT_IP0_ADDR	{0,0,0,0,0,0}
#define BL_DEFAULT_MAC1_ADDR	{0x00,0x00,0x00,0x00,0x00,0x02}
#define BL_DEFAULT_IP1_ADDR	{0,0,0,0,0,0}
#define BL_DEFAULT_BAUD_RATE	ARM_BAUD_115200
#define BL_DHCP		0x1		// BL_DHCP=1 => dhcp default to enable
#define BL_NET_MAC	0x0		// default mac port used by TFTP
#define BL_DEFAULT_PHY_CHIP	BL_PHY
#define BL_IMAGE0_BASE	(FLASH_BASE+BL_BOOTLOADER_BLOCK_SIZE)
#define BL_BUFFER_BASE	0x300000
#define BL_BUFFER_SIZE	0x100000
#define BL_CACHE_DEFAULT 0x0
#define SEMI_HEAP_SIZE	0x20000
#define SEMI_STACK_SIZE	0x20000

//------------------------------------------------------------------------------------------
#endif
