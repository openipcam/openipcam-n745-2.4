/********************************************************/
/*							*/
/* Winbond W90N740					*/
/* Shirley yu <clyu2@winbond.com.tw>			*/
/*							*/
/********************************************************/
#ifndef __ASM_ARCH_HARDWARE_H
#define __ASM_ARCH_HARDWARE_H

/*
 * define W90N740 CPU master clock
 */
#define MHz		1000000
#define fMCLK_MHz	(15 * MHz)
#define fMCLK		(fMCLK_MHz / MHz)
#define MCLK2		(fMCLK_MHz / 2)

#define pcibios_assign_all_busses()        1

/*
 * ASIC Address Definition
 */

#define Base_Addr	0xFFF00000

#define VPint	*(volatile unsigned int *)
#define VPshort	*(volatile unsigned short *)
#define VPchar	*(volatile unsigned char *)

#ifndef CSR_WRITE
#   define CSR_WRITE(addr,data)	(VPint(addr) = (data))
#endif

#ifndef CSR_READ
#   define CSR_READ(addr)	(VPint(addr))
#endif

#ifndef CAM_Reg
#   define CAM_Reg(x)		(VPint(CAMBASE+(x*0x4)))
#endif

/* ************************ */
/* System Manager Registers */
/* ************************ */
#define PDID		(Base_Addr+0x00000)
#define ARBCON		(Base_Addr+0x00004)
#define PLLCON		(Base_Addr+0x00008)
#define CLKSEL		(Base_Addr+0x0000c)

/*****************************/
/* Cache Control Register Map*/
/*****************************/
#define CAHCNF		(Base_Addr+0x02000)
#define CAHCON		(Base_Addr+0x02004)
#define CAHADR		(Base_Addr+0x02008)

#define NON_CANCHABLE 0x80000000
//#define NONCACHE
/*****************************/
/* EBI Control Registers Map */
/*****************************/
#define EBICON		(Base_Addr+0x01000)
#define ROMCON0		(Base_Addr+0x01004)
#define DRAMCON0	(Base_Addr+0x01008)
#define DRAMCON1	(Base_Addr+0x0100c)
#define SDTIME0		(Base_Addr+0x01010)
#define SDTIME1		(Base_Addr+0x01014)

/********************************/
/* GPIO Controller Registers Map*/
/********************************/
#define GPIO_CFG		(Base_Addr+0x83000)
#define GPIO_DIR		(Base_Addr+0x83004)
#define GPIO_DATAOUT	(Base_Addr+0x83008)
#define GPIO_DATAIN		(Base_Addr+0x8300c)
#define GPIO_DEBNCE		(Base_Addr+0x83010)


/* *********************** */
/* Ethernet BDMA Registers */
/* *********************** */
#define BDMATXCON	(Base_Addr+0x9000)
#define BDMARXCON	(Base_Addr+0x9004)
#define BDMATXPTR	(Base_Addr+0x9008)
#define BDMARXPTR	(Base_Addr+0x900C)
#define BDMARXLSZ	(Base_Addr+0x9010)
#define BDMASTAT	(Base_Addr+0x9014)
#define CAMBASE		(Base_Addr+0x9100)
/*
 * CAM		0x9100 ~ 0x917C
 * BDMATXBUF	0x9200 ~ 0x92FC
 * BDMARXBUF	0x9800 ~ 0x99FC
 */

/* ********************** */
/* Ethernet MAC Registers */
/* ********************** */
#define MACON		(Base_Addr+0xA000)
#define CAMCON		(Base_Addr+0xA004)
#define MACTXCON	(Base_Addr+0xA008)
#define MACTXSTAT	(Base_Addr+0xA00C)
#define MACRXCON	(Base_Addr+0xA010)
#define MACRXSTAT	(Base_Addr+0xA014)
#define STADATA		(Base_Addr+0xA018)
#define STACON		(Base_Addr+0xA01C)
#define CAMEN		(Base_Addr+0xA028)
#define EMISSCNT	(Base_Addr+0xA03C)
#define EPZCNT		(Base_Addr+0xA040)
#define ERMPZCNT	(Base_Addr+0xA044)
#define EXTSTAT		(Base_Addr+0x9040)

/* ************************ */
/* HDLC Channel A Registers */
/* ************************ */

/* ************************ */
/* HDLC Channel B Registers */
/* ************************ */

/* ******************* */
/* I/O Ports Registers */
/* ******************* */
#define IOPMOD		(Base_Addr+0x5000)
#define IOPCON		(Base_Addr+0x5004)
#define IOPDATA		(Base_Addr+0x5008)


/* ****************************** */
/* AIC Registers Map			  */
/* ****************************** */
#define AIC_SCR1	(Base_Addr+0x82004)
#define AIC_SCR2	(Base_Addr+0x82008)
#define AIC_SCR3	(Base_Addr+0x8200c)
#define AIC_SCR4	(Base_Addr+0x82010)
#define AIC_SCR5	(Base_Addr+0x82014)
#define AIC_SCR6	(Base_Addr+0x82018)
#define AIC_SCR7	(Base_Addr+0x8201c)
#define AIC_SCR8	(Base_Addr+0x82010)
#define AIC_SCR9	(Base_Addr+0x82024)
#define AIC_SCR10	(Base_Addr+0x82028)
#define AIC_SCR11	(Base_Addr+0x8202c)
#define AIC_SCR12	(Base_Addr+0x82030)
#define AIC_SCR13	(Base_Addr+0x82034)
#define AIC_SCR14	(Base_Addr+0x82038)
#define AIC_SCR15	(Base_Addr+0x8203c)
#define AIC_SCR16	(Base_Addr+0x82040)
#define AIC_SCR17	(Base_Addr+0x82044)
#define AIC_SCR18	(Base_Addr+0x82048)

#define AIC_IRSR	(Base_Addr+0x82100)
#define AIC_IASR	(Base_Addr+0x82104)
#define AIC_ISR		(Base_Addr+0x82108)
#define AIC_IPER	(Base_Addr+0x8210c)
#define AIC_ISNR	(Base_Addr+0x82110)
#define AIC_IMR		(Base_Addr+0x82114)
#define AIC_OISR	(Base_Addr+0x82118)
#define AIC_MECR	(Base_Addr+0x82120)
#define AIC_MDCR	(Base_Addr+0x82124)
#define AIC_SSCR	(Base_Addr+0x82128)
#define AIC_SCCR	(Base_Addr+0x8212c)
#define AIC_EOSCR	(Base_Addr+0x82130)
#define AIC_TEST	(Base_Addr+0x82200)


#define IntScr(index,value)		(VPint(Base_Addr+0x82000+4*index)=value)
#define IntPend		(VPint(AIC_EOSCR))
#define IntMask		(VPint(AIC_MDCR))
#define IntUnMask	(VPint(AIC_MECR))

#define INT_ENABLE(n)		IntUnMask = (1<<(n))
#define INT_DISABLE(n)		IntMask = (1<<(n))
//#define CLEAR_PEND_INT(n)	IntPend = (0)
//#define SET_PEND_INT(n)		IntPndTst |= (1<<(n))

/* ***************** */
/* I2C Bus Registers */
/* ***************** */

/* ************** */
/* GDMA Registers */
/* ************** */

/* ************** */
/* UART Registers */
/* ************** */

#define DEBUG_CONSOLE	(0)

#define COM_TX			(Base_Addr+0x80000)
#define COM_RX			(Base_Addr+0x80000)
#define COM_DLL 		(Base_Addr+0x80000)
#define COM_DLM 		(Base_Addr+0x80004)
#define COM_IER 		(Base_Addr+0x80004)
#define COM_IIR 		(Base_Addr+0x80008)
#define COM_FCR 		(Base_Addr+0x80008)
#define COM_LCR 		(Base_Addr+0x8000c)
#define COM_MCR 		(Base_Addr+0x80010)
#define COM_LSR 		(Base_Addr+0x80014)
#define COM_MSR 		(Base_Addr+0x80018)
#define COM_TOR 		(Base_Addr+0x8001c)

#define UART_BASE0	COM_TX

#if DEBUG_CONSOLE == 0
	#define DEBUG_TX_BUFF_BASE	COM_TX
	#define DEBUG_RX_BUFF_BASE	COM_RX
	#define DEBUG_UARTLCON_BASE	COM_LCR
	#define DEBUG_UARTCONT_BASE	COM_LCR                  
	#define DEBUG_UARTBRD_BASE	COM_DLL
	#define DEBUG_CHK_STAT_BASE	COM_IIR
#endif

#define DEBUG_ULCR_REG_VAL	(0x3)
#define DEBUG_ULCR_REG_VAL	(0x3)
#define DEBUG_UDLL_REG_VAL	(0x6)
#define DEBUG_RX_CHECK_BIT	(0X20)
#define DEBUG_TX_CAN_CHECK_BIT	(0X40)
#define DEBUG_TX_DONE_CHECK_BIT	(0X80)


/* **************** */
/* Timers Registers */
/* **************** */
#define TCR0		(Base_Addr+0x81000)
#define TCR1		(Base_Addr+0x81004)
#define TICR0		(Base_Addr+0x81008)
#define TICR1		(Base_Addr+0x8100c)
#define TDR0		(Base_Addr+0x81010)
#define TDR1		(Base_Addr+0x81014)
#define TISR		(Base_Addr+0x81018)
#define WTCR		(Base_Addr+0x8101c)

/*******************/
/* SYSCFG Register */
/*******************/

#define SYS_INIT_BASE	EXTDBWTH
#define rSYSCFG		(0x87FFFF90)	/* disable Cache/Write buffer */

/**********************************/
/* System Memory Control Register */
/**********************************/
#define DSR0		(2<<0)	/* ROM Bank0 */
#define DSR1		(0<<2)	/* 0: Disable, 1: Byte, 2: Half-Word, 3: Word */
#define DSR2		(0<<4)
#define DSR3		(0<<6)
#define DSR4		(0<<8)
#define DSR5		(0<<10)
#define DSD0		(2<<12) /* RAM Bank0 */
#define DSD1		(0<<14)
#define DSD2		(0<<16)
#define DSD3		(0<<18)
#define DSX0		(0<<20)	/* EXTIO0 */
#define DSX1		(0<<22)
#define DSX2		(0<<24)
#define DSX3		(0<<26)

#define rEXTDBWTH	(DSR0|DSR1|DSR2|DSR3|DSR4|DSR5 | DSD0|DSD1|DSD2|DSD3 | DSX0|DSX1|DSX2|DSX3)

/****************************************/
/* ROMCON0: ROM Bank 0 Control Register */
/****************************************/
#define PMC0		(0x0<<0)	/*00: Normal ROM   01: 4 word page*/
					/*10: 8 word page  11:16 word page*/
#define tPA0		(0x0<<2)	/*00: 5 cycles     01: 2 cycles*/
					/*10: 3 cycles     11: 4 cycles*/
#define tACC0		(0x6<<4)	/*000: Disable bank 001: 2 cycles*/
					/*010: 3 cycles     011: 4 cycles*/
					/*110: 7 cycles     111: Reserved*/
#define ROM_BASE0_R	((0x00000000>>16)<<10)
#define ROM_NEXT0_R	((0x00200000>>16)<<20)
#define ROM_BASE0_B	((0x01000000>>16)<<10)
#define ROM_NEXT0_B	((0x01200000>>16)<<20)
#define rROMCON0_R	(ROM_NEXT0_R|ROM_BASE0_R|tACC0|tPA0|PMC0)
#define rROMCON0_B	(ROM_NEXT0_B|ROM_BASE0_B|tACC0|tPA0|PMC0)

#define rROMCON1	0x0
#define rROMCON2	0x0
#define rROMCON3	0x0
#define rROMCON4	0x0
#define rROMCON5	0x0


/********************************************/
/* SDRAMCON0: SDRAM Bank 0 Control Register */
/********************************************/
#define StRC0		(0x1<<7)
#define StRP0		(0x3<<8)
#define SDRAM_BASE0_R	((0x01000000>>16)<<10)
#define SDRAM_NEXT0_R	((0x01800000>>16)<<20)
#define SDRAM_BASE0_B	((0x00000000>>16)<<10)
#define SDRAM_NEXT0_B	((0x00800000>>16)<<20)
#define SCAN0		(0x0<<30)
#define rSDRAMCON0_R	(SCAN0|SDRAM_NEXT0_R|SDRAM_BASE0_R|StRP0|StRC0)
#define rSDRAMCON0_B	(SCAN0|SDRAM_NEXT0_B|SDRAM_BASE0_B|StRP0|StRC0)

#define rSDRAMCON1	0x0
#define rSDRAMCON2	0x0
#define rSDRAMCON3	0x0

/************************************************/
/* DRAM Refresh & External I/O Control Register */
/************************************************/
#define ExtIOBase	(0x360<<0)
#define VSF		(0x1<<15)
#define REN		(0x1<<16)
#define tCHR		(0x0<<17)
#define tCSR		(0x0<<20)
#define RefCountValue	((2048+1-(16*fMCLK))<<21)
#define rREFEXTCON	(RefCountValue|tCSR|tCHR|REN|VSF|ExtIOBase)

/********/
/* Misc */
/********/

#define TMOD_TIMER0_VAL	0x3	/* Timer0  TOGGLE, and Run */
#define TAG_BASE	0x11000000

#define HARD_RESET_NOW()

/*PCI*/
#define PCIBIOS_MIN_IO		0x6000
#define PCIBIOS_MIN_MEM 	0x01000000


#endif /* __ASM_ARCH_HARDWARE_H */
