/**************************************************************************************************
 *                                                                          
 * Copyright (c) 2004 - 2006 Winbond Electronics Corp. All rights reserved.      
 *                                                                         
 * FILENAME
 *     w90p710.h
 *
 * VERSION
 *     0.1
 *
 * DESCRIPTION
 *     This file contains the register map of W90P710 processor.
 *
 *     1. System Address Map
 *     2. System Manager Control Registers
 *     3. EBI  
 *     4. SDRAM 
 *     5. PCI 
 *     6. MAC 
 *     7. NATA
 *     8. GDMA
 *     9. USB Host Controller
 *    10. Network Security Engine
 *    11. UART
 *    12. Timer
 *    13. AIC
 *    14. GPIO
 *
 * HISTORY
 *     2004/11/04		 0.1 Draft version, Created by PC30 MNCheng
 *
 *
 * REMARK
 *     None
 *     
 *************************************************************************************************/
#ifndef _W90P710_H
#define _W90P710_H

/**********************************************************************************************************
 *                                                               
 * 1. System Address Map - Defines the register base address of each Macro 
 *                         function.
 *
 **********************************************************************************************************/
#define    GCR_BA    0xFFF00000 /* Global Control */
#define    EBI_BA    0xFFF01000 /* EBI Control */
#define    CACHE_BA  0xFFF02000 /* Cache Control */
#define    EMC_BA    0xFFF03000 /* Ethernet MAC */
#define    GDMA_BA   0xFFF04000 /* GDMA control */
#define    USBH_BA   0xFFF05000 /* USB Host Control */
#define    USBD_BA   0xFFF06000 /* USB Device Control */
#define    FMI_BA    0xFFF07000 /* Flash Memory Card Interface */
#define    LCD_BA    0xFFF08000 /* Display, LCM Interface & Bypass */
#define    ADO_BA    0xFFF09000 /* Audio Control */

#define    UART0_BA  0xFFF80000 /* UART0 Control (console) */
#define    UART1_BA  0xFFF80100 /* UART1 Control (Bluetooth) */
#define    UART2_BA  0xFFF80200 /* UART2 Control (IrDA) */
#define    UART3_BA  0xFFF80300 /* UART3 Control (micro-printer) */
#define    TMR_BA    0xFFF81000 /* Timer */
#define    AIC_BA    0xFFF82000 /* Interrupt Controller */
#define    GPIO_BA   0xFFF83000 /* GPIO Control */
#define    RTC_BA	 0xFFF84000 /* Real Time Clock Control */
#define    SCHI0_BA  0xFFF85000 /* Smart Card Host Interface 0 Control */
#define	   SCHI1_BA	 0xFFF85800 /* Smart Card Host Interface 1 Control */
#define    I2C0_BA   0xFFF86000 /* I2C 0 Control */
#define    I2C1_BA   0xFFF86100 /* I2C 1 Control */
#define    SSP_BA    0xFFF86200 /* Synchronous Serial Port */
#define    PWM_BA    0xFFF87000 /* Pulse Width Modulation Control */
#define    KPI_BA    0xFFF88000 /* Keypad Interface Control */
#define    PS2_BA    0xFFF89000 /* PS2 Interface Control */


/****************************************************************************************************
 *                                                               
 * 2. System Manager Control Registers  
 *
 ****************************************************************************************************/
#define REG_PDID		(GCR_BA+0x0000)	/* Product Identifier Register  */
#define REG_ARBCON		(GCR_BA+0x0004)	/* Arbitration Control Register */
#define REG_PLLCON		(GCR_BA+0x0008)	/* PLL Control Register */
#define REG_CLKSEL		(GCR_BA+0x000C)	/* Clock Select Register */
#define REG_PLLCON2		(GCR_BA+0x0010)	/* PLL Control Register 2 */
#define REG_I2SCKCON	(GCR_BA+0x0014)	/* Audio IIS Clock Control Register */
#define REG_IRQWAKECON	(GCR_BA+0x0020)	/* IRQ Wakeup Control Register */
#define REG_IRQWAKEFLAG	(GCR_BA+0x0024)	/* IRQ Wakeup Flag Register */
#define REG_PMCON		(GCR_BA+0x0028)	/* Power Manager control Register */
#define REG_USBTXRCON	(GCR_BA+0x0030)	/* USB transceiver control Register */


/****************************************************************************************************
 *
 * 3. Memory Control Registers
 *
 ****************************************************************************************************/
#define REG_EBICON		(EBI_BA+0x000)	/* EBI control register */
#define REG_ROMCON		(EBI_BA+0x004)	/* ROM/FLASH control register */
#define REG_SDCONF0		(EBI_BA+0x008)	/* SDRAM bank 0 configuration register */
#define REG_SDCONF1		(EBI_BA+0x00C)	/* SDRAM bank 1 configuration register */
#define REG_SDTIME0		(EBI_BA+0x010)	/* SDRAM bank 0 timing control register */
#define REG_SDTIME1		(EBI_BA+0x014)	/* SDRAM bank 1 timing control register */
#define REG_EXT0CON		(EBI_BA+0x018)	/* External I/O 0 control register */
#define REG_EXT1CON		(EBI_BA+0x01C)	/* External I/O 1 control register */
#define REG_EXT2CON		(EBI_BA+0x020)	/* External I/O 2 control register */
#define REG_EXT3CON		(EBI_BA+0x024)	/* External I/O 3 control register */


/****************************************************************************************************
 *
 * Cache Control Registers
 *
 ****************************************************************************************************/
#define REG_CAHCNF		(CACHE_BA+0x000)	/* Cache configuration register */
#define REG_CAHCON		(CACHE_BA+0x004)	/* Cache control register */
#define REG_CAHADR		(CACHE_BA+0x008)	/* Cache address register */


/****************************************************************************************************
 *
 * 6. MAC Registers
 *
 ****************************************************************************************************/
	/* Control register */
#define REG_CAMCMR			(EMC_BA+0x000)	/* CAM Command Register */
#define REG_CAMEN			(EMC_BA+0x004)	/* CAM Enable Register */
#define REG_CAM0M_Base		(EMC_BA+0x008)
#define REG_CAM0L_Base		(EMC_BA+0x00c)
#define REG_CAMxM_Reg(x)	(REG_CAM0M_Base+x*0x8)		/*  */
#define REG_CAMxL_Reg(x)	(REG_CAM0L_Base+x*0x8)		/*  */

#define REG_TXDLSA			(EMC_BA+0x088)	/* Transmit Descriptor Link List Start Address Register */
#define REG_RXDLSA			(EMC_BA+0x08C)	/* Receive Descriptor Link List Start Address Register */
#define REG_MCMDR			(EMC_BA+0x090)	/* MAC Command Register */
#define REG_MIID			(EMC_BA+0x094)	/* MII Management Data Register */
#define REG_MIIDA			(EMC_BA+0x098)	/* MII Management Control and Address Register */
#define REG_FFTCR			(EMC_BA+0x09C)	/* FIFO Threshold Control Register */
#define REG_TSDR			(EMC_BA+0x0a0)	/* Transmit Start Demand Register */
#define REG_RSDR			(EMC_BA+0x0a4)	/* Receive Start Demand Register */
#define REG_DMARFC			(EMC_BA+0x0a8)	/* Maximum Receive Frame Control Register */
#define REG_MIEN			(EMC_BA+0x0ac)	/* MAC Interrupt Enable Register */
	/* Status Registers */
#define REG_MISTA			(EMC_BA+0x0b0)	/* MAC Interrupt Status Register */
#define REG_MGSTA			(EMC_BA+0x0b4)	/* MAC General Status Register */
#define REG_MPCNT			(EMC_BA+0x0b8)	/* Missed Packet Count Register */
#define REG_MRPC			(EMC_BA+0x0bc)	/* MAC Receive Pause Count Register */
#define REG_MRPCC			(EMC_BA+0x0c0)	/* MAC Receive Pause Current Count Register */
#define REG_MREPC			(EMC_BA+0x0c4)	/* MAC Remote Pause Count Register */
#define REG_DMARFS			(EMC_BA+0x0c8)	/* DMA Receive Frame Status Register */
#define REG_CTXDSA			(EMC_BA+0x0cc)	/* Current Transmit Descriptor Start Address Register */
#define REG_CTXBSA			(EMC_BA+0x0d0)	/* Current Transmit Buffer Start Address Register */
#define REG_CRXDSA			(EMC_BA+0x0d4)	/* Current Receive Descriptor Start Address Register */
#define REG_CRXBSA			(EMC_BA+0x0d8)	/* Current Receive Buffer Start Address Register */
	/* Diagnostic Registers */
#define REG_RXFSM			(EMC_BA+0x200)	/* Receive Finite State Machine Register */
#define REG_TXFSM			(EMC_BA+0x204)	/* Transmit Finite State Machine Register */
#define REG_FSM0			(EMC_BA+0x208)	/* Finite State Machine Register 0 */
#define REG_FSM1			(EMC_BA+0x20c)	/* Finite State Machine Register 1 */
#define REG_DCR				(EMC_BA+0x210)	/* Debug Configuration Register */
#define REG_DMMIR			(EMC_BA+0x214)	/* Debug Mode MAC Information Register */
#define REG_BISTR			(EMC_BA+0x300)	/* BIST Mode Register */


/****************************************************************************************************
 *
 * 8. GDMA Registers
 *
 ****************************************************************************************************/
#define REG_GDMA_CTL0		(GDMA_BA+0x000)  /* Channel 0 Control Register */
#define REG_GDMA_SRCB0		(GDMA_BA+0x004)  /* Channel 0 Source Base Address Register */
#define REG_GDMA_DSTB0		(GDMA_BA+0x008)  /* Channel 0 Destination Base Address Register */
#define REG_GDMA_TCNT0		(GDMA_BA+0x00C)  /* Channel 0 Transfer Count Register */
#define REG_GDMA_CSRC0		(GDMA_BA+0x010)  /* Channel 0 Current Source Address Register */
#define REG_GDMA_CDST0		(GDMA_BA+0x014)  /* Channel 0 Current Destination Address Register */
#define REG_GDMA_CTCNT0		(GDMA_BA+0x018)  /* Channel 0 Current Transfer Count Register */
#define REG_GDMA_CTL1		(GDMA_BA+0x020)  /* Channel 1 Control Register */
#define REG_GDMA_SRCB1		(GDMA_BA+0x024)  /* Channel 1 Source Base Address Register */
#define REG_GDMA_DSTB1		(GDMA_BA+0x028)  /* Channel 1 Destination Base Address Register */
#define REG_GDMA_TCNT1		(GDMA_BA+0x02C)  /* Channel 1 Transfer Count Register */
#define REG_GDMA_CSRC1		(GDMA_BA+0x030)  /* Channel 1 Current Source Address Register */
#define REG_GDMA_CDST1		(GDMA_BA+0x034)  /* Channel 1 Current Destination Address Register */
#define REG_GDMA_CTCNT1		(GDMA_BA+0x038)  /* Channel 1 Current Transfer Count Register */


/****************************************************************************************************
 *
 * 9. USB Host Controller Registers
 *
 ****************************************************************************************************/
/* OpenHCI Registers */
#define REG_HcRevision			(USBH_BA+0x000)	/*	Host Controller Revision Register */
#define REG_HcControl			(USBH_BA+0x004)	/*	Host Controller Control Register */
#define REG_HcCommandStatus		(USBH_BA+0x008)	/*	Host Controller Command Status Register */
#define REG_HcInterruptStatus 	(USBH_BA+0x00C)	/*	Host Controller Interrupt Status Register */
#define REG_HcInterruptEnable	(USBH_BA+0x010)	/*	Host Controller Interrupt Enable Register */
#define REG_HcInterruptDisable	(USBH_BA+0x014)	/*	Host Controller Interrupt Disable Register */
#define REG_HcHCCA				(USBH_BA+0x018)	/*	Host Controller Communication Area Register */
#define REG_HcPeriodCurrentED	(USBH_BA+0x01C)	/*	Host Controller Period Current ED Register */
#define REG_HcControlHeadED		(USBH_BA+0x020)	/*	Host Controller Control Head ED Register */
#define REG_HcControlCurrentED	(USBH_BA+0x024)	/*	Host Controller Control Current ED Register */
#define REG_HcBulkHeadED		(USBH_BA+0x028)	/*	Host Controller Bulk Head ED Register */
#define REG_HcBulkCurrentED		(USBH_BA+0x02C)	/*	Host Controller Bulk Current ED Register */
#define REG_HcDoneHead			(USBH_BA+0x030)	/*	Host Controller Done Head Register */
#define REG_HcFmInterval		(USBH_BA+0x034)	/*	Host Controller Frame Interval Register */
#define REG_HcFrameRemaining	(USBH_BA+0x038)	/*	Host Controller Frame Remaining Register */
#define REG_HcFmNumber			(USBH_BA+0x03C)	/*	Host Controller Frame Number Register */
#define REG_HcPeriodicStart		(USBH_BA+0x040)	/*	Host Controller Periodic Start Register */
#define REG_HcLSThreshold		(USBH_BA+0x044)	/*	Host Controller Low Speed Threshold Register */
#define REG_HcRhDescriptorA		(USBH_BA+0x048)	/*	Host Controller Root Hub Descriptor A Register */
#define REG_HcRhDescriptorB		(USBH_BA+0x04C)	/*	Host Controller Root Hub Descriptor B Register */
#define REG_HcRhStatus			(USBH_BA+0x050)	/*	Host Controller Root Hub Status Register */
#define REG_HcRhPortStatus1		(USBH_BA+0x054)	/*	Host Controller Root Hub Port Status [1] */
#define REG_HcRhPortStatus2		(USBH_BA+0x058)	/*	Host Controller Root Hub Port Status [2] */

/* USB Configuration Registers */
#define TestModeEnable			(USBH_BA+0x200)	/*	USB Test Mode Enable Register */
#define OperationalModeEnable	(USBH_BA+0x204)	/*	USB Operational Mode Enable Register */


/**********************************************************************************************************
 *                                                               
 * 9. USB Device Control Registers  
 *
 **********************************************************************************************************/
#define REG_USB_CTL	  		(USBD_BA+0x000)    /* USB control register */
#define REG_USB_CVCMD		(USBD_BA+0x004)    /* USB class or vendor command register */
#define REG_USB_IE	  		(USBD_BA+0x008)    /* USB interrupt enable register */
#define REG_USB_IS	  		(USBD_BA+0x00c)    /* USB interrupt status register */
#define REG_USB_IC	  		(USBD_BA+0x010)    /* USB interrupt status clear register */
#define REG_USB_IFSTR		(USBD_BA+0x014)    /* USB interface and string register */
#define REG_USB_ODATA0		(USBD_BA+0x018)    /* USB control transfer-out port 0 register */
#define REG_USB_ODATA1		(USBD_BA+0x01C)    /* USB control transfer-out port 1 register */
#define REG_USB_ODATA2		(USBD_BA+0x020)    /* USB control transfer-out port 2 register */
#define REG_USB_ODATA3		(USBD_BA+0x024)    /* USB control transfer-out port 3 register */
#define REG_USB_IDATA0		(USBD_BA+0x028)    /* USB control transfer-in data port 0 register */
#define REG_USB_IDATA1		(USBD_BA+0x02C)    /* USB control transfer-in data port 1 register */
#define REG_USB_IDATA2		(USBD_BA+0x030)    /* USB control transfer-in data port 2 register */
#define REG_USB_IDATA3		(USBD_BA+0x034)    /* USB control transfer-in data port 2 register */
#define REG_USB_SIE			(USBD_BA+0x038)    /* USB SIE status Register */
#define REG_USB_ENG			(USBD_BA+0x03c)    /* USB Engine Register */
#define REG_USB_CTLS		(USBD_BA+0x040)    /* USB control transfer status register */
#define REG_USB_CONFD		(USBD_BA+0x044)    /* USB Configured Value register */
#define REG_USB_EPA_INFO	(USBD_BA+0x048)    /* USB endpoint A information register */
#define REG_USB_EPA_CTL		(USBD_BA+0x04c)    /* USB endpoint A control register */
#define REG_USB_EPA_IE		(USBD_BA+0x050)    /* USB endpoint A Interrupt Enable register */
#define REG_USB_EPA_IC		(USBD_BA+0x054)    /* USB endpoint A interrupt clear register */
#define REG_USB_EPA_IS		(USBD_BA+0x058)    /* USB endpoint A interrupt status register */
#define REG_USB_EPA_ADDR	(USBD_BA+0x05c)    /* USB endpoint A address register */
#define REG_USB_EPA_LENTH	(USBD_BA+0x060)    /* USB endpoint A transfer length register */
#define REG_USB_EPB_INFO	(USBD_BA+0x064)    /* USB endpoint B information register */
#define REG_USB_EPB_CTL		(USBD_BA+0x068)    /* USB endpoint B control register */
#define REG_USB_EPB_IE		(USBD_BA+0x06c)    /* USB endpoint B Interrupt Enable register */
#define REG_USB_EPB_IC		(USBD_BA+0x070)    /* USB endpoint B interrupt clear register */
#define REG_USB_EPB_IS		(USBD_BA+0x074)    /* USB endpoint B interrupt status register */
#define REG_USB_EPB_ADDR	(USBD_BA+0x078)    /* USB endpoint B address register */
#define REG_USB_EPB_LENTH	(USBD_BA+0x07c)    /* USB endpoint B transfer length register */
#define REG_USB_EPC_INFO	(USBD_BA+0x080)    /* USB endpoint C information register */
#define REG_USB_EPC_CTL		(USBD_BA+0x084)    /* USB endpoint C control register */
#define REG_USB_EPC_IE		(USBD_BA+0x088)    /* USB endpoint C Interrupt Enable register */
#define REG_USB_EPC_IC		(USBD_BA+0x08c)    /* USB endpoint C interrupt clear register */
#define REG_USB_EPC_IS		(USBD_BA+0x090)    /* USB endpoint C interrupt status register */
#define REG_USB_EPC_ADDR	(USBD_BA+0x094)    /* USB endpoint C address register */
#define REG_USB_EPC_LENTH	(USBD_BA+0x098)    /* USB endpoint C transfer length register */
#define REG_USB_EPA_XFER	(USBD_BA+0x09c)    /* USB endpoint A remain transfer length register */
#define REG_USB_EPA_PKT		(USBD_BA+0x0a0)    /* USB endpoint A remain packet length register */
#define REG_USB_EPB_XFER	(USBD_BA+0x0a4)    /* USB endpoint B remain transfer length register */
#define REG_USB_EPB_PKT		(USBD_BA+0x0a8)    /* USB endpoint B remain packet length register */
#define REG_USB_EPC_XFER	(USBD_BA+0x0ac)    /* USB endpoint C remain transfer length register */
#define REG_USB_EPC_PKT		(USBD_BA+0x0b0)    /* USB endpoint C remain packet length register */
 
/**********************************************************************************************************
 *                                                               
 * 7. Flash memory Card Control Registers  
 *
 **********************************************************************************************************/
/* Flash Memory Interface Registers definition */
#define REG_FMICR    (FMI_BA+0x00)    /* FMI control register */
#define REG_FMIDSA   (FMI_BA+0x04)    /* FMI DMA transfer starting address register */
#define REG_FMIBCR   (FMI_BA+0x08)    /* FMI DMA byte count register */
#define REG_FMIIER   (FMI_BA+0x0C)    /* FMI interrupt enable register */
#define REG_FMIISR   (FMI_BA+0x10)    /* FMI interrupt status register */
#define REG_FMIBIST  (FMI_BA+0x14)    /* FMI bist register */
#define REG_FB0_0    (FMI_BA+0x400)   /* Flash buffer 0 */
#define REG_FB1_0    (FMI_BA+0x800)   /* Flash buffer 1 */

/* Secure Digit Registers definition */
#define REG_SDCR     (FMI_BA+0x300)   /* SD control register */
#define REG_SDHINI   (FMI_BA+0x304)   /* SD host initial register */
#define REG_SDIER    (FMI_BA+0x308)   /* SD interrupt enable register */
#define REG_SDISR    (FMI_BA+0x30C)   /* SD interrupt status register */
#define REG_SDARG    (FMI_BA+0x310)   /* SD command argument register */
#define REG_SDRSP0   (FMI_BA+0x314)   /* SD receive response token register 0 */
#define REG_SDRSP1   (FMI_BA+0x318)   /* SD receive response token register 1 */
#define REG_SDBLEN   (FMI_BA+0x31C)   /* SD block length register */
 

/**********************************************************************************************************
 *                                                               
 * 8. Audio Interface Control Registers  
 *
 **********************************************************************************************************/
#define REG_ACTL_CON			(ADO_BA + 0x00)   /* Audio controller control register */
#define REG_ACTL_RESET			(ADO_BA + 0x04)   /* Sub block reset control register */
#define REG_ACTL_RDSTB			(ADO_BA + 0x08)   /* DMA destination base address register for record */
#define REG_ACTL_RDST_LENGTH	(ADO_BA + 0x0C)   /* DMA destination length register for record */
#define REG_ACTL_PDSTB			(ADO_BA + 0x18)   /* DMA destination base address register for play */
#define REG_ACTL_PDST_LENGTH	(ADO_BA + 0x1C)   /* DMA destination length register for play */
#define REG_ACTL_RSR			(ADO_BA + 0x14)   /* Record status register */
#define REG_ACTL_PSR			(ADO_BA + 0x24)   /* Play status register */
#define REG_ACTL_IISCON			(ADO_BA + 0x28)   /* IIS control register */
#define REG_ACTL_ACCON			(ADO_BA + 0x2C)   /* AC-link control register */
#define REG_ACTL_ACOS0			(ADO_BA + 0x30)   /* AC-link out slot 0 */
#define REG_ACTL_ACOS1			(ADO_BA + 0x34)   /* AC-link out slot 1 */
#define REG_ACTL_ACOS2			(ADO_BA + 0x38)   /* AC-link out slot 2 */
#define REG_ACTL_ACIS0			(ADO_BA + 0x3C)   /* AC-link in slot 0 */
#define REG_ACTL_ACIS1			(ADO_BA + 0x40)   /* AC-link in slot 1 */
#define REG_ACTL_ACIS2			(ADO_BA + 0x44)   /* AC-link in slot 2 */
#define REG_ACTL_ADCON			(ADO_BA + 0x48)   /* ADC0 control register */
#define REG_ACTL_M80CON			(ADO_BA + 0x4C)   /* M80 interface control register */
#define REG_ACTL_M80DATA0		(ADO_BA + 0x50)   /* M80 data0 register */
#define REG_ACTL_M80DATA1		(ADO_BA + 0x54)   /* M80 data1 register */
#define REG_ACTL_M80DATA2		(ADO_BA + 0x58)   /* M80 data2 register */
#define REG_ACTL_M80DATA3		(ADO_BA + 0x5C)   /* M80 data3 register */
#define REG_ACTL_M80ADDR		(ADO_BA + 0x60)   /* M80 interface start address register */
#define REG_ACTL_M80SRADDR		(ADO_BA + 0x64)   /* M80 interface start address register of right channel */
#define REG_ACTL_M80SIZE		(ADO_BA + 0x70)   /* M80 interface data size register */
#define REG_ACTL_DACON			(ADO_BA + 0x74)   /* DAC control register */
 

/****************************************************************************************************
 *
 * 11. UART Control Registers
 *
 ****************************************************************************************************/
/* UART 0 */
#define REG_UART0_TX     (UART0_BA+0x0)    /* (W) TX buffer */
#define REG_UART0_RX     (UART0_BA+0x0)    /* (R) RX buffer */
#define REG_UART0_LSB    (UART0_BA+0x0)    /* Divisor latch LSB */
#define REG_UART0_MSB    (UART0_BA+0x04)   /* Divisor latch MSB */
#define REG_UART0_IER    (UART0_BA+0x04)   /* Interrupt enable register */
#define REG_UART0_IIR    (UART0_BA+0x08)   /* (R) Interrupt ident. register */
#define REG_UART0_FCR    (UART0_BA+0x08)   /* (W) FIFO control register */
#define REG_UART0_LCR    (UART0_BA+0x0C)   /* Line control register */
#define	REG_UART0_LSR    (UART0_BA+0x14)   /* (R) Line status register */
#define	REG_UART0_TOR    (UART0_BA+0x1C)   /* (R) Time out register */

/* UART 1 */
#define REG_UART1_TX     (UART1_BA+0x0)    /* (W) TX buffer */
#define REG_UART1_RX     (UART1_BA+0x0)    /* (R) RX buffer */
#define REG_UART1_LSB    (UART1_BA+0x0)    /* Divisor latch LSB */
#define REG_UART1_MSB    (UART1_BA+0x04)   /* Divisor latch MSB */
#define REG_UART1_IER    (UART1_BA+0x04)   /* Interrupt enable register */
#define REG_UART1_IIR    (UART1_BA+0x08)   /* (R) Interrupt ident. register */
#define REG_UART1_FCR    (UART1_BA+0x08)   /* (W) FIFO control register */
#define REG_UART1_LCR    (UART1_BA+0x0C)   /* Line control register */
#define REG_UART1_MCR    (UART1_BA+0x10)   /* Modem control register */
#define	REG_UART1_LSR    (UART1_BA+0x14)   /* (R) Line status register */
#define REG_UART1_MSR    (UART1_BA+0x18)   /* (R) Modem status register */
#define	REG_UART1_TOR    (UART1_BA+0x1C)   /* (R) Time out register */
#define	REG_UART1_UBCR   (UART1_BA+0x20)	/* Bluetooth */

/* UART 2 */
#define REG_UART2_TX     (UART2_BA+0x0)    /* (W) TX buffer */
#define REG_UART2_RX     (UART2_BA+0x0)    /* (R) RX buffer */
#define REG_UART2_LSB    (UART2_BA+0x0)    /* Divisor latch LSB */
#define REG_UART2_MSB    (UART2_BA+0x04)   /* Divisor latch MSB */
#define REG_UART2_IER    (UART2_BA+0x04)   /* Interrupt enable register */
#define REG_UART2_IIR    (UART2_BA+0x08)   /* (R) Interrupt ident. register */
#define REG_UART2_FCR    (UART2_BA+0x08)   /* (W) FIFO control register */
#define REG_UART2_LCR    (UART2_BA+0x0C)   /* Line control register */
#define	REG_UART2_LSR    (UART2_BA+0x14)   /* (R) Line status register */
#define	REG_UART2_TOR    (UART2_BA+0x1C)   /* (R) Time out register */
#define	REG_UART2_UBCR   (UART2_BA+0x20)	/* Bluetooth */
#define REG_UART2_IRCR   (UART2_BA+0x20)  /* (R/W) IrDA control register */

/* UART 3 */
#define REG_UART3_TX     (UART3_BA+0x0)    /* (W) TX buffer */
#define REG_UART3_RX     (UART3_BA+0x0)    /* (R) RX buffer */
#define REG_UART3_LSB    (UART3_BA+0x0)    /* Divisor latch LSB */
#define REG_UART3_MSB    (UART3_BA+0x04)   /* Divisor latch MSB */
#define REG_UART3_IER    (UART3_BA+0x04)   /* Interrupt enable register */
#define REG_UART3_IIR    (UART3_BA+0x08)   /* (R) Interrupt ident. register */
#define REG_UART3_FCR    (UART3_BA+0x08)   /* (W) FIFO control register */
#define REG_UART3_LCR    (UART3_BA+0x0C)   /* Line control register */
#define REG_UART3_MCR    (UART3_BA+0x10)   /* Modem control register */
#define	REG_UART3_LSR    (UART3_BA+0x14)   /* (R) Line status register */
#define REG_UART3_MSR    (UART3_BA+0x18)   /* (R) Modem status register */
#define	REG_UART3_TOR    (UART3_BA+0x1C)   /* (R) Time out register */


/****************************************************************************************************
 *
 * 12. Timer Control Registers
 *
 ****************************************************************************************************/
#define	REG_TCR0     (TMR_BA+0x0)    /* Control Register 0 */
#define	REG_TCR1     (TMR_BA+0x04)   /* Control Register 1 */
#define	REG_TICR0    (TMR_BA+0x08)   /* Initial Control Register 0 */
#define	REG_TICR1    (TMR_BA+0x0C)   /* Initial Control Register 1 */
#define	REG_TDR0     (TMR_BA+0x10)   /* Data Register 0 */
#define	REG_TDR1     (TMR_BA+0x14)   /* Data Register 1 */
#define	REG_TISR     (TMR_BA+0x18)   /* Interrupt Status Register */
#define REG_WTCR     (TMR_BA+0x1C)   /* Watchdog Timer Control Register */

/****************************************************************************************************
 *
 * 13. Advanced Interrupt Controller Registers
 *
 ****************************************************************************************************/
#define REG_AIC_SCR1    (AIC_BA+0x04)    /* Source control register 1 */
#define REG_AIC_SCR2    (AIC_BA+0x08)    /* Source control register 2 */
#define REG_AIC_SCR3    (AIC_BA+0x0C)    /* Source control register 3 */
#define REG_AIC_SCR4    (AIC_BA+0x10)    /* Source control register 4 */
#define REG_AIC_SCR5    (AIC_BA+0x14)    /* Source control register 5 */
#define REG_AIC_SCR6    (AIC_BA+0x18)    /* Source control register 6 */
#define REG_AIC_SCR7    (AIC_BA+0x1C)    /* Source control register 7 */
#define REG_AIC_SCR8    (AIC_BA+0x20)    /* Source control register 8 */
#define REG_AIC_SCR9    (AIC_BA+0x24)    /* Source control register 9 */
#define REG_AIC_SCR10   (AIC_BA+0x28)    /* Source control register 10 */
#define REG_AIC_SCR11   (AIC_BA+0x2C)    /* Source control register 11 */
#define REG_AIC_SCR12   (AIC_BA+0x30)    /* Source control register 12 */
#define REG_AIC_SCR13   (AIC_BA+0x34)    /* Source control register 13 */
#define REG_AIC_SCR14   (AIC_BA+0x38)    /* Source control register 14 */
#define REG_AIC_SCR15   (AIC_BA+0x3C)    /* Source control register 15 */
#define REG_AIC_SCR16   (AIC_BA+0x40)    /* Source control register 16 */
#define REG_AIC_SCR17   (AIC_BA+0x44)    /* Source control register 17 */
#define REG_AIC_SCR18   (AIC_BA+0x48)    /* Source control register 18 */
#define REG_AIC_SCR19   (AIC_BA+0x4C)    /* Source control register 19 */
#define REG_AIC_SCR20   (AIC_BA+0x50)    /* Source control register 20 */
#define REG_AIC_SCR21   (AIC_BA+0x54)    /* Source control register 21 */
#define REG_AIC_SCR22   (AIC_BA+0x58)    /* Source control register 22 */
#define REG_AIC_SCR23   (AIC_BA+0x5C)    /* Source control register 23 */
#define REG_AIC_SCR24   (AIC_BA+0x60)    /* Source control register 24 */
#define REG_AIC_SCR25   (AIC_BA+0x64)    /* Source control register 25 */
#define REG_AIC_SCR26   (AIC_BA+0x68)    /* Source control register 26 */
#define REG_AIC_SCR27   (AIC_BA+0x6c)    /* Source control register 27 */
#define REG_AIC_SCR28   (AIC_BA+0x70)    /* Source control register 28 */
#define REG_AIC_SCR29   (AIC_BA+0x74)    /* Source control register 29 */
#define REG_AIC_SCR30   (AIC_BA+0x78)    /* Source control register 30 */
#define REG_AIC_SCR31   (AIC_BA+0x7c)    /* Source control register 31 */

#define REG_AIC_IRSR    (AIC_BA+0x100)   /* Interrupt raw status register */
#define REG_AIC_IASR    (AIC_BA+0x104)   /* Interrupt active status register */
#define REG_AIC_ISR     (AIC_BA+0x108)   /* Interrupt status register */
#define REG_AIC_IPER    (AIC_BA+0x10C)   /* Interrupt priority encoding register */
#define REG_AIC_ISNR    (AIC_BA+0x110)   /* Interrupt source number register */
#define REG_AIC_IMR     (AIC_BA+0x114)   /* Interrupt mask register */
#define REG_AIC_OISR    (AIC_BA+0x118)   /* Output interrupt status register */
#define REG_AIC_MECR    (AIC_BA+0x120)   /* Mask enable command register */
#define REG_AIC_MDCR    (AIC_BA+0x124)   /* Mask disable command register */
#define REG_AIC_SSCR    (AIC_BA+0x128)   /* Source set command register */
#define REG_AIC_SCCR    (AIC_BA+0x12C)   /* Source clear command register */
#define REG_AIC_EOSCR   (AIC_BA+0x130)   /* End of service command register */
#define REG_AIC_TEST    (AIC_BA+0x200)   /* ICE/Debug mode register */


/**********************************************************************************************************
 *                                                               
 * 22. Universal Serial Interface Control Registers  
 *
 **********************************************************************************************************/
#define	REG_USI_CNTRL		(SSP_BA+0x0)     /* Control and Status Register */
#define	REG_USI_DIVIDER		(SSP_BA+0x04)    /* Clock Divider Register */
#define	REG_USI_SSR			(SSP_BA+0x08)    /* Slave Select Register */
#define	REG_USI_Rx0			(SSP_BA+0x10)    /* Data Receive Register 0 */
#define	REG_USI_Rx1			(SSP_BA+0x14)    /* Data Receive Register 1 */
#define	REG_USI_Rx2			(SSP_BA+0x18)    /* Data Receive Register 2 */
#define	REG_USI_Rx3			(SSP_BA+0x1C)    /* Data Receive Register 3 */
#define	REG_USI_Tx0			(SSP_BA+0x10)    /* Data Transmit Register 0 */
#define	REG_USI_Tx1			(SSP_BA+0x14)    /* Data Transmit Register 1 */
#define	REG_USI_Tx2			(SSP_BA+0x18)    /* Data Transmit Register 2 */
#define	REG_USI_Tx3			(SSP_BA+0x1C)    /* Data Transmit Register 3 */
 

/**********************************************************************************************************
 *                                                               
 * 23. RTC Control Registers  
 *
 **********************************************************************************************************/
#define REG_RTC_INIR		(RTC_BA+0x0000)	/* Product RTC RTC INITIALION Register  */
#define REG_RTC_AER			(RTC_BA+0x0004)	/* Product RTC RTC ACCESS ENABLE Register  */
#define REG_RTC_FCR			(RTC_BA+0x0008)	/* Product RTC RTC FREQUENCY COMPENSATION Register  */
#define REG_RTC_TLR			(RTC_BA+0x000C)	/* Product RTC TIME LOADING Register  */
#define REG_RTC_CLR			(RTC_BA+0x0010)	/* Product RTC CALENDAR LOADING Register  */
#define REG_RTC_TSSR		(RTC_BA+0x0014)	/* Product RTC TIME SCAL SELECTION Register  */
#define REG_RTC_DWR			(RTC_BA+0x0018)	/* Product RTC DAY OF THE WEEK Register  */
#define REG_RTC_TAR			(RTC_BA+0x001C)	/* Product RTC TIME ALARM Register  */
#define REG_RTC_CAR			(RTC_BA+0x0020)	/* Product RTC CALENDAR ALARM Register  */
#define REG_RTC_LIR			(RTC_BA+0x0024)	/* Product RTC LEAP YEAR INDICATOR Register  */
#define REG_RTC_RIER		(RTC_BA+0x0028)	/* Product RTC RTC INTERRUPT ENABLE Register  */
#define REG_RTC_RIIR		(RTC_BA+0x002C)	/* Product RTC RTC INTERRUPT INDICATOR Register  */
#define REG_RTC_TTR			(RTC_BA+0x0030)	/* Product RTC RTC TIME TICK Register  */


/**********************************************************************************************************
 *                                                               
 * PWM Control Registers  
 *
 **********************************************************************************************************/
#define REG_PWM_PPR			(PWM_BA+0x0000)	
#define REG_PWM_CSR			(PWM_BA+0x0004)	
#define REG_PWM_PCR			(PWM_BA+0x0008)	
#define REG_PWM_CNR0		(PWM_BA+0x000C)	
#define REG_PWM_CMR0		(PWM_BA+0x0010)	
#define REG_PWM_PDR0		(PWM_BA+0x0014)	
#define REG_PWM_CNR1		(PWM_BA+0x0018)	
#define REG_PWM_CMR1		(PWM_BA+0x001C)	
#define REG_PWM_PDR1		(PWM_BA+0x0020)
#define REG_PWM_CNR2		(PWM_BA+0x0024)	
#define REG_PWM_CMR2		(PWM_BA+0x0028)	
#define REG_PWM_PDR2		(PWM_BA+0x002C)
#define REG_PWM_CNR3		(PWM_BA+0x0030)	
#define REG_PWM_CMR3		(PWM_BA+0x0034)	
#define REG_PWM_PDR3		(PWM_BA+0x0038)
#define REG_PWM_PIER		(PWM_BA+0x003C)	
#define REG_PWM_PIIR		(PWM_BA+0x0040)	


/****************************************************************************************************
 *                                                               
 * Keypad Interface Registers  
 *
 ****************************************************************************************************/
#define REG_KPICONF			(KPI_BA+0x0000)	/* Product  KPI controller configuration Register  */
#define REG_KPI3KCONF		(KPI_BA+0x0004)	/* Product  KPI controller 3-keys configuration Register  */
#define REG_KPILPCONF		(KPI_BA+0x0008)	/* Product  KPI controller low power configuration Register  */
#define REG_KPISTATUS		(KPI_BA+0x000C)	/* Product  KPI controller status Register  */


/****************************************************************************************************
 *
 * General-Purpose Input/Output Controller Registers
 *
 ****************************************************************************************************/
#define GPIO_OFFSET             0x10
/* groups 0 */
#define REG_GPIO_CFG0			(GPIO_BA+0x0000)	/* GPIO port0 configuration Register  */
#define REG_GPIO_DIR0			(GPIO_BA+0x0004)	/* GPIO port0 direction control Register  */
#define REG_GPIO_DATAOUT0		(GPIO_BA+0x0008)	/* GPIO port0 data out Register  */
#define REG_GPIO_DATAIN0        (GPIO_BA+0x000c)	/* GPIO port0 data input Register */
/* groups 1 */ 
#define REG_GPIO_CFG1			(GPIO_BA+0x0010)	/* GPIO port1 configuration Register  */
#define REG_GPIO_DIR1			(GPIO_BA+0x0014)	/* GPIO port1 direction control Register  */
#define REG_GPIO_DATAOUT1		(GPIO_BA+0x0018)	/* GPIO port1 data out Register  */
#define REG_GPIO_DATAIN1        (GPIO_BA+0x001c)	/* GPIO port1 data input Register */
/* groups 2 */
#define REG_GPIO_CFG2			(GPIO_BA+0x0020)	/* GPIO port2 configuration Register  */
#define REG_GPIO_DIR2			(GPIO_BA+0x0024)	/* GPIO port2 direction control Register  */
#define REG_GPIO_DATAOUT2		(GPIO_BA+0x0028)	/* GPIO port2 data out Register  */
#define REG_GPIO_DATAIN2        (GPIO_BA+0x002c)	/* GPIO port2 data input Register */
/* groups 3 */  
#define REG_GPIO_CFG3			(GPIO_BA+0x0030)	/* GPIO port3 configuration Register  */
#define REG_GPIO_DIR3			(GPIO_BA+0x0034)	/* GPIO port3 direction control Register  */
#define REG_GPIO_DATAOUT3		(GPIO_BA+0x0038)	/* GPIO port3 data out Register  */
#define REG_GPIO_DATAIN3        (GPIO_BA+0x003c)	/* GPIO port3 data input Register */
/* groups 4 */
#define REG_GPIO_CFG4			(GPIO_BA+0x0040)	/* GPIO port4 configuration Register  */
#define REG_GPIO_DIR4			(GPIO_BA+0x0044)	/* GPIO port4 direction control Register  */
#define REG_GPIO_DATAOUT4		(GPIO_BA+0x0048)	/* GPIO port4 data out Register  */
#define REG_GPIO_DATAIN4        (GPIO_BA+0x004c)	/* GPIO port4 data input Register */
/* groups 5 */
#define REG_GPIO_CFG5			(GPIO_BA+0x0050)	/* GPIO port5 configuration Register  */
#define REG_GPIO_DIR5			(GPIO_BA+0x0054)	/* GPIO port5 direction control Register  */
#define REG_GPIO_DATAOUT5		(GPIO_BA+0x0058)	/* GPIO port5 data out Register  */
#define REG_GPIO_DATAIN5        (GPIO_BA+0x005c)	/* GPIO port5 data input Register */
/* groups 6 */ 
#define REG_GPIO_CFG6			(GPIO_BA+0x0060)	/* GPIO port6 configuration Register  */
#define REG_GPIO_DIR6			(GPIO_BA+0x0064)	/* GPIO port6 direction control Register  */
#define REG_GPIO_DATAOUT6		(GPIO_BA+0x0068)	/* GPIO port6 data out Register  */
#define REG_GPIO_DATAIN6        (GPIO_BA+0x006c)	/* GPIO port6 data input Register */
  
#define REG_GPIO_DBNCECON       (GPIO_BA+0x0070)	/* GPIO input debounce control Register */
#define REG_GPIO_XICFG			(GPIO_BA+0x0074) 	/* Extend Interrupt Configure Register */
#define REG_GPIO_XISTATUS		(GPIO_BA+0x0078) 	/* Extend Interrupt Status Register */

/****************************************************************************************************
 *
 * PS2 Controller Registers
 *
 ****************************************************************************************************/
#define REG_PS2CMD			(PS2_BA+0x0000)
#define REG_PS2STS			(PS2_BA+0x0004)
#define REG_PS2SCANCODE		(PS2_BA+0x0008)
#define REG_PS2ASCII		(PS2_BA+0x000c)


/****************************************************************************************************
 *
 * I2C Controller Registers
 *
 ****************************************************************************************************/
//#define I2C_BA	I2C0_BA
#define REG_I2C_CSR				(I2C_BA+0x00)
#define REG_I2C_DIVIDER 		(I2C_BA+0x04)
#define REG_I2C_CMDR			(I2C_BA+0x08)
#define REG_I2C_SWR				(I2C_BA+0x0C)
#define REG_I2C_RxR				(I2C_BA+0x10)
#define REG_I2C_TxR				(I2C_BA+0x14)

/****************************************************************************************************
 *
 * Smart Card Host Interface Registers
 *
 ****************************************************************************************************/
//#define SCHI_BA		SCHI0_BA
#define REG_SCHI_RBR			(SCHI_BA+0x00)
#define REG_SCHI_TBR			(SCHI_BA+0x00)
#define REG_SCHI_IER			(SCHI_BA+0x04)
#define REG_SCHI_ISR			(SCHI_BA+0x08)
#define REG_SCHI_SCFR			(SCHI_BA+0x08)
#define REG_SCHI_SCCR			(SCHI_BA+0x0c)
#define REG_SCHI_CBR			(SCHI_BA+0x10)
#define REG_SCHI_SCSR			(SCHI_BA+0x14)
#define REG_SCHI_GTR			(SCHI_BA+0x18)
#define REG_SCHI_ECR			(SCHI_BA+0x1c)
#define REG_SCHI_TMR			(SCHI_BA+0x20)
#define REG_SCHI_TOC			(SCHI_BA+0x28)
#define REG_SCHI_TOIR0			(SCHI_BA+0x2c)
#define REG_SCHI_TOIR1			(SCHI_BA+0x30)
#define REG_SCHI_TOIR2			(SCHI_BA+0x34)
#define REG_SCHI_TOD0			(SCHI_BA+0x38)
#define REG_SCHI_TOD1			(SCHI_BA+0x3c)
#define REG_SCHI_TOD2			(SCHI_BA+0x40)
#define REG_SCHI_BTOR			(SCHI_BA+0x44)
#define REG_SCHI_BLL			(SCHI_BA+0x00)
#define REG_SCHI_BLH			(SCHI_BA+0x04)
#define REG_SCHI_ID				(SCHI_BA+0x08)

/****************************************************************************************************
 *
 * 710 LCD Controller Register Sets
 *
 ****************************************************************************************************/
/* LCD Controller */
#define REG_LCD_LCDCON           (LCD_BA+0x0000)   /* LCD Controller control register */

/* LCD Interrupt Control */
#define REG_LCD_LCDINTENB        (LCD_BA+0x0004)   /* LCD interrupt enable register */
#define REG_LCD_LCDINTS          (LCD_BA+0x0008)   /* LCD interrupt status register */
#define REG_LCD_LCDINTC          (LCD_BA+0x000C)   /* LCD interrupt clear register */
                            
/* LCD Pre-processing */    
#define REG_LCD_OSDUPSCF         (LCD_BA+0x0010)   /* OSD data Horizontal/Vertical up-scaling factor */
#define REG_LCD_VDUPSCF          (LCD_BA+0x0014)   /* Video data Horizontal/Vertical up-scaling factor */
#define REG_LCD_OSDNSCF          (LCD_BA+0x0018)   /* OSD data Horizontal/Vertical down-scaling factor */
#define REG_LCD_VDDNSCF          (LCD_BA+0x001C)   /* Video data Horizontal/Vertical down-scaling factor */
                            
/* LCD FIFO Control */      
#define REG_LCD_FIFOCON          (LCD_BA+0x0020)   /* LCD FIFOs controller register */
#define REG_LCD_FIFOSTATUS       (LCD_BA+0x0024)   /* LCD FIFOs status register */
#define REG_LCD_FIFO1PRM         (LCD_BA+0x0028)   /* LCD FIFO1 transfer parameters */
#define REG_LCD_FIFO2PRM         (LCD_BA+0x002C)   /* LCD FIFO2 transfer parameters */
#define REG_LCD_F1SADDR          (LCD_BA+0x0030)   /* FIFO1 transfer data source start address */
#define REG_LCD_F2SADDR          (LCD_BA+0x0034)   /* FIFO2 transfer data source start address */
#define REG_LCD_F1DREQCNT        (LCD_BA+0x0038)   /* FIFO1 transfer data count register */
#define REG_LCD_F2DREQCNT        (LCD_BA+0x003C)   /* FIFO2 transfer data count register */
#define REG_LCD_F1CURADR         (LCD_BA+0x0040)   /* FIFO1 current access data address register */
#define REG_LCD_F2CURADR         (LCD_BA+0x0044)   /* FIFO2 current access data address register */
#define REG_LCD_FIFORELACOLCNT   (LCD_BA+0x0048)   /* FIFO 1 real column count register */
#define REG_LCD_FIFO2RELACOLCNT  (LCD_BA+0x004C)   /* FIFO 2 real column count register */

/* Color Generation */
#define REG_LCD_LUTENTY1         (LCD_BA+0x0060)   /* TFT: lookup table entry index register */
#define REG_LCD_LUTENTY2         (LCD_BA+0x0064)   /* TFT: lookup table entry index register */
#define REG_LCD_LUTENTY3         (LCD_BA+0x0068)   /* TFT: lookup table entry index register */
#define REG_LCD_LUTENTY4         (LCD_BA+0x006C)   /* TFT: lookup table entry index register */
#define REG_LCD_TMDDITHP1        (LCD_BA+0x0070)   /* Gray level dithered data duty pattern */
#define REG_LCD_TMDDITHP2        (LCD_BA+0x0074)   /* Gray level dithered data duty pattern */
#define REG_LCD_TMDDITHP3        (LCD_BA+0x0078)   /* Gray level dithered data duty pattern */
#define REG_LCD_TMDDITHP4        (LCD_BA+0x007C)   /* Gray level dithered data duty pattern */
#define REG_LCD_TMDDITHP5        (LCD_BA+0x0080)   /* Gray level dithered data duty pattern */
#define REG_LCD_TMDDITHP6        (LCD_BA+0x0084)   /* Gray level dithered data duty pattern */
#define REG_LCD_TMDDITHP7        (LCD_BA+0x0088)   /* Gray level dithered data duty pattern */
                             
/* LCD Post-processing */    
#define REG_LCD_DDISPCP          (LCD_BA+0x0090)   /* Dummy Display Color Pattern Register */
#define REG_LCD_DISPWINS         (LCD_BA+0x0094)   /* Valid Display Window Starting Coordinate */
#define REG_LCD_DISPWINE         (LCD_BA+0x0098)   /* Valid Display Window Ending Coordinate */
#define REG_LCD_OSDWINS          (LCD_BA+0x009C)   /* OSD Window Starting Coordinate */
#define REG_LCD_OSDWINE          (LCD_BA+0x00A0)   /* OSD Window Ending Coordinate */
#define REG_LCD_OSDOVCN          (LCD_BA+0x00A4)   /* OSD Overlay Control Register */
#define REG_LCD_OSDKYP           (LCD_BA+0x00A8)   /* OSD Overlay Color-Key Pattern */
#define REG_LCD_OSDKYM           (LCD_BA+0x00AC)   /* OSD Overlay Color-Key Mask */

/* LCD Timing Generation */
#define REG_LCD_LCDTCON1         (LCD_BA+0x00B0)   /* LCD Timing Control Register 1 */
#define REG_LCD_LCDTCON2         (LCD_BA+0x00B4)   /* LCD Timing Control Register 2 */
#define REG_LCD_LCDTCON3         (LCD_BA+0x00B8)   /* LCD Timing Control Register 3 */
#define REG_LCD_LCDTCON4         (LCD_BA+0x00BC)   /* LCD Timing Control Register 4 */
#define REG_LCD_LCDTCON5         (LCD_BA+0x00C0)   /* LCD Timing Control Register 5 */
#define REG_LCD_LCDTCON6         (LCD_BA+0x00C4)   /* LCD Timing Control Register 6 */
                             
/* Look Up Table SRAM */     
#define REG_LCD_LUTADDR          (LCD_BA+0x0100)   /* The start address of Look-Up Table. 
                                                      The memory range is 0x100 ~ 0x4FF. */


#endif /* _W90P710_H */ 