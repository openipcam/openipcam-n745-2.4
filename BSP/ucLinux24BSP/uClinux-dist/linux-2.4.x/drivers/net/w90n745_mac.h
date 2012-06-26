/****************************************************************************
 *																																					*
 * Copyright (c) 2005 -	2007 Winbond Electronics Corp. All rights reserved.	*
 *																																					*
 ****************************************************************************/

/****************************************************************************
 *
 * FILENAME
 *	   w90n745_mac.h
 *
 * VERSION
 *	   1.0
 *
 * DESCRIPTION
 *	   The head file for MAC driver on POS-TAX Board
 *
 * DATA	STRUCTURES
 *	   None
 *
 * FUNCTIONS
 *	   None
 *
 * HISTORY
 *	   2005/08/01		 Ver 1.0
 *
 * AUTHOR
 *	  PC34 Lsshi
 *
 * REMARK
 *	   None
 *************************************************************************/
#ifndef _W90N745_MAC_H_
#define _W90N745_MAC_H_

#define NON_CACHE_FLAG		0x80000000

#define  MAC_OFFSET  	0x0
#define  MAC_0_OFFSET  	MAC_OFFSET

// Advanced Interrupt Controller Registers
#define AIC_SCR_EMCTX0 (VPint(AIC_SCR17))
#define AIC_SCR_EMCRX0 (VPint(AIC_SCR18))

//CAM Registers
#define  CAMCMR			(MAC_OFFSET)    //CAM Command Regiser
#define  CAMEN			(MAC_OFFSET+0x4)//CAM ennable regiser
#define  CAM0M			(MAC_OFFSET+0x8)//CAM1 Most significant Word register
#define  CAM0L			(MAC_OFFSET+0xc)//CAM1 Least Significant Word Register
#define  CAM_ENTRY_SIZE	0x8     		//CAM  entry size
#define  CAM_ENTRIES	0x16    		//CAM  entries

//MAC Regiseters
#define MIEN			(MAC_OFFSET+0xac) //MAC Interrupt Enable Register
#define MCMDR			(MAC_OFFSET+0x90) //MAC Command Regiser
#define MIID			(MAC_OFFSET+0x94) //MII Management Data Register
#define MIIDA			(MAC_OFFSET+0x98) //MII Management Data Control and Address Register
#define MPCNT			(MAC_OFFSET+0xb8) //Missed Packet Counter Register

//DMA Registers
#define TXDLSA			(MAC_OFFSET+0x88) //Transmit Descriptor Link List Start Address Regiser 
#define RXDLSA			(MAC_OFFSET+0x8c) //Receive Descriptor LInk List Start Addresss Register
#define DMARFC			(MAC_OFFSET+0xa8) //DMA Receive Frame Control Register
#define TSDR			(MAC_OFFSET+0xa0) //Transmit Start Demand Register
#define RSDR			(MAC_OFFSET+0xa4) //Recevie Start Demand Register
#define FIFOTHD			(MAC_OFFSET+0x9c) //FIFO Threshold Adjustment Register

//EMC Status Register
#define MISTA			(MAC_OFFSET+0xb0) //MAC Interrupter Status Register
#define MGSTA			(MAC_OFFSET+0xb4) //MAC General Status Register
#define MRPC			(MAC_OFFSET+0xbc)  //MAC Receive Pauese counter register
#define MRPCC			(MAC_OFFSET+0xc0) //MAC Receive Pauese Current Count Regiser
#define MREPC			(MAC_OFFSET+0xc4)  //MAC Remote pause count retister

//DMA Registers
#define DMARFS			(MAC_OFFSET+0xc8) //DMA Receive Frame Status Register
#define CTXDSA			(MAC_OFFSET+0xcc) //Current Transmit Descriptor Start Addresss Register
#define CTXBSA			(MAC_OFFSET+0xd0) //Current Transmit Buffer Start Address Regiser
#define CRXDSA			(MAC_OFFSET+0xd4) //Current Receive Descriptor start Address regiser
#define CRXBSA			(MAC_OFFSET+0xd8) //Current Receive Buffer Start Address Regiser

//Debug Mode Receive Finite State Machine Registers
#define RXFSM			(MAC_OFFSET+0x200)
#define TXFSM			(MAC_OFFSET+0x204)
#define FSM0			(MAC_OFFSET+0x208)
#define FSM1			(MAC_OFFSET+0x20c)

//Descriptor
typedef struct 
{
	volatile unsigned long	SL;
	volatile unsigned long	buffer;
	volatile unsigned long	reserved;
	volatile unsigned long	next;
}RXBD;


typedef struct 
{
	volatile unsigned long mode;
	volatile unsigned long buffer;
	volatile unsigned long SL;
	volatile unsigned long next;
}TXBD;


// CAM Command Register(CAMCMR)
#define CAM_AUP  0x0001  // Accept Packets with Unicast Address
#define CAM_AMP  0x0002  // Accept Packets with Multicast Address
#define CAM_ABP  0x0004  // Accept Packets with Broadcast Address
#define CAM_CCAM 0x0008  // 0: Accept Packets CAM Recognizes and Reject Others
                         // 1: Reject Packets CAM Recognizes and Accept Others
#define CAM_ECMP 0x0010  // Enable CAM Compare
//ownership bit
#define	RX_OWNERSHIP_CPU	(0x0<<30)
#define	RX_OWNERSHIP_DMA	(2<<30)

#define TX_OWNERSHIP_CPU	(0x0<<30)
#define	TX_OWNERSHIP_DMA	(2<<30)

// RX Frame Descriptor's Owner bit
#define RXfOwnership_DMA 0x80000000  // 10 = DMA
#define RXfOwnership_CPU 0x3fffffff  // 00 = CPU

// TX Frame Descriptor's Owner bit
#define TXfOwnership_DMA 0x80000000  // 1 = DMA
#define TXfOwnership_CPU 0x7fffffff  // 0 = CPU

// Tx Frame Descriptor's Control bits
#define MACTxIntEn    0x04
#define CRCMode       0x02
#define NoCRCMode     0x00
#define PaddingMode   0x01
#define NoPaddingMode 0x00

//received descriptor status
#define RXDS_RXINTR     (1<<16) //set if reception of packet caused an interrupt condition
#define RXDS_CRCE		(1<<17) // set if crc error 
#define RXDS_PTLE		(1<<19) //set if received frame longer than 1518 bytes
#define RXDS_RXGD		(1<<20) // receiving good packet 
#define RXDS_ALIE		(1<<21) //Alignment Error
#define RXDS_RP			(1<<22) //runt packet	
#define RXDS_Inverse	(1<<26) //current hit entry is setting on inverse mode
#define RXDS_PortHit	(1<<27) //port hit
#define RXDS_IPHit		(1<<28)	//ip hit
#define RXDS_Hit		(1<<29) //hit

//Tx ownership bit
#define TX_OWNERSHIP_CPU  (0x0<<31) 
#define TX_OWNERSHIP_DMA  (0x1<<31)
//tx mode 
#define  TX_MODE_PAD	 0x1	  //pad
#define  TX_MODE_CRC	(0x1<<1)  //crc mode 
#define  TX_MODE_IE		(0x1<<2)  //interrupt enable

//Tx status
#define TXDS_TXINTR 	(1<<16)		//Interruput on Transmit
#define TXDS_DEF		(1<<17)		//Transmit defered
#define TXDS_TXCP		(1<<19)		//Transmit Completion
#define TXDS_EXDEF		(1<<20)		//exceed deferal
#define TXDS_NCS		(1<<21)		//No Carrier Sense Error
#define TXDS_TXABT		(1<<22)		//transimtting aborted
#define TXDS_LC			(1<<23)		//late collision
#define TXDS_TXHA		(1<<24)		//transmitting halted
#define TXDS_PAU		(1<<25)		//Paused
#define TXDS_SQE		(1<<26)		//SQE error
#define TXDS_CCNT		(0xf<<27)	//transmit collision count
//cam command regiser
#define CAMCMR_AUP		0x1 //Accept unicast packet
#define CAMCMR_AMP		(0x1<<1) //Accpet multicast packet
#define CAMCMR_ABP		(0x1<<2) //Accept broadcast packet 
#define CAMCMR_CCAM		(0x1<<3) //complement CAM
#define CAMCMR_ECMP		(0x1<<4) //Enable CAM compare


// MAC MII Management Data Control and Address Register(MIIDA)
#define MDCCR    0x00300000  // MDC clock rating
#define PHYAD    0x00000100  // PHY Address
#define PHYWR    0x00010000  // Write Operation
#define PHYBUSY  0x00020000  // Busy Bit
#define PHYPreSP 0x00040000  // Preamble Suppress

// PHY(DM9161) Register Description
#define PHY_CNTL_REG    0x00
#define PHY_STATUS_REG  0x01
#define PHY_ID1_REG     0x02
#define PHY_ID2_REG     0x03
#define PHY_ANA_REG     0x04
#define PHY_ANLPA_REG   0x05
#define PHY_ANE_REG     0x06

#define PHY_DSC_REG     0x10
#define PHY_DSCS_REG    0x11
#define PHY_10BTCS_REG  0x12
#define PHY_SINT_REG    0x15
#define PHY_SREC_REG    0x16
#define PHY_DISC_REG    0x17

//PHY Control Register
#define RESET_PHY       (1 << 15)
#define ENABLE_LOOPBACK (1 << 14)
#define DR_100MB        (1 << 13)
#define ENABLE_AN       (1 << 12)
#define PHY_MAC_ISOLATE (1 << 10)
#define RESTART_AN      (1 << 9)
#define PHY_FULLDUPLEX  (1 << 8)
#define PHY_COL_TEST    (1 << 7)


// MAC Interrupt Enable Register(MIEN)
#define EnRXINTR 0x00000001  // Enable Interrupt on Receive Interrupt
#define EnCRCE   0x00000002  // Enable CRC Error Interrupt
#define EnRXOV   0x00000004  // Enable Receive FIFO Overflow Interrupt
#define EnPTLE   0x00000008  // Enable Packet Too Long Interrupt
#define EnRXGD   0x00000010  // Enable Receive Good Interrupt
#define EnALIE   0x00000020  // Enable Alignment Error Interrupt
#define EnRP     0x00000040  // Enable Runt Packet on Receive Interrupt
#define EnMMP    0x00000080  // Enable More Missed Packets Interrupt
#define EnDFO    0x00000100  // Enable DMA receive frame over maximum size Interrupt
#define EnDEN    0x00000200  // Enable DMA early notification Interrupt
#define EnRDU    0x00000400  // Enable Receive Descriptor Unavailable Interrupt
#define EnRxBErr 0x00000800  // Enable Receive Bus ERROR interrupt
#define EnCFR    0x00004000  // Enable Control Frame Receive Interrupt
#define EnTXINTR 0x00010000  // Enable Interrupt on Transmit Interrupt
#define EnTXEMP  0x00020000  // Enable Transmit FIFO Empty Interrupt
#define EnTXCP   0x00040000  // Enable Transmit Completion Interrupt
#define EnEXDEF  0x00080000  // Enable Defer Interrupt
#define EnNCS    0x00100000  // Enable No Carrier Sense Interrupt
#define EnTXABT  0x00200000  // Enable Transmit Abort Interrupt
#define EnLC     0x00400000  // Enable Late Collision Interrupt
#define EnTDU    0x00800000  // Enable Transmit Descriptor Unavailable Interrupt
#define EnTxBErr 0x01000000  // Enable Transmit Bus ERROR Interrupt

// PHY Status Register
#define AN_COMPLETE     (1 << 5)

// PHY Auto-negotiation Advertisement Register

#define MODE_DR100_FULL   3
#define MODE_DR100_HALF   2
#define MODE_DR10_FULL    1
#define MODE_DR10_HALF    0


#define DR100_TX_FULL   (1 << 8)
#define DR100_TX_HALF   (1 << 7)
#define DR10_TX_FULL    (1 << 6)
#define DR10_TX_HALF    (1 << 5)
#define IEEE_802_3_CSMA_CD   1



//MAC Interrupt Enable Register
#define MIEN_EnRXINTR	1
#define MIEN_EnCRCE		(1<<1)
#define MIEN_EnRXOV		(1<<2)
#define MIEN_EnPTLE		(1<<3)
#define MIEN_EnRXGD		(1<<4)
#define MIEN_EnALIE		(1<<5)
#define MIEN_EnRP		(1<<6)
#define MIEN_EnMMP		(1<<7)
#define MIEN_EnDFO		(1<<8)
#define MIEN_EnDEN		(1<<9)
#define MIEN_EnRDU		(1<<10)
#define MIEN_EnRXBErr	(1<<11)
#define MIEN_EnCFR		(1<<14)
#define MIEN_EnTXINTR	(1<<16)
#define MIEN_EnTXEMP	(1<<17)
#define MIEN_EnTXCP		(1<<18)
#define MIEN_EnEXDEF	(1<<19)
#define MIEN_EnNCS		(1<<20)
#define MIEN_EnTXABT	(1<<21)
#define MIEN_EnLC		(1<<22)
#define MIEN_EnTDU		(1<<23)
#define MIEN_EnTxBErr	(1<<24)
//MAC Command Regiser
#define MCMDR_RXON		1
#define MCMDR_ALP		(1<<1)
#define MCMDR_ARP		(1<<2)
#define MCMDR_ACP		(1<<3)
#define MCMDR_AEP		(1<<4)
#define MCMDR_SPCRC		(1<<5)
#define MCMDR_TXON		(1<<8)
#define MCMDR_NDEF		(1<<9)
#define MCMDR_SDPZ		(1<<16)
#define MCMDR_EnSQE		(1<<17)
#define MCMDR_FDUP		(1<<18)
#define MCMDR_EnMDC		(1<<19)
#define MCMDR_OPMOD		(1<<20)
#define MCMDR_LBK		(1<<21)
//#define MCMDR_EnMII		(1<<22)
//#define MCMDR_LAN		(1<<23)
#define SWR				(1<<24)//lsshi add 2005-4-22 12:07


//MAC MII Management Data Control and Address Register
#define  MIIDA_PHYRAD	1
#define  MIIDA_PHYAD	(1<<8)
#define  MIIDA_WR		(1<<16)
#define  MIIDA_BUSY		(1<<17)
#define  MIIDA_PreSP	(1<<18)
#define  MIIDA_MDCON	(1<<19)
#define  MIIDA_MDCCR	(1<<20)

//FIFO Threshold Adjustment Register 
#define  FIFOTHD_RxTHD	 1
#define  FIFOTHD_TxTHD	(1<<8)
#define  FIFOTHD_SWR	(1<<16)
#define  FIFOTHD_Blength (1<<20)
//MAC Interrupt Status Register
#define MISTA_RXINTR	1
#define MISTA_CRCE		(1<<1)
#define MISTA_RXOV		(1<<2)
#define MISTA_PTLE		(1<<3)
#define MISTA_RXGD		(1<<4)
#define MISTA_ALIE		(1<<5)
#define MISTA_RP		(1<<6)
#define MISTA_MMP		(1<<7)
#define MISTA_DFOI		(1<<8)
#define MISTA_DENI		(1<<9)
#define MISTA_RDU		(1<<10)
#define MISTA_RxBErr	(1<<11)
#define MISTA_CFR		(1<<14)
#define MISTA_TXINTR	(1<<16)
#define MISTA_TXEMP		(1<<17)
#define MISTA_TXCP		(1<<18)
#define MISTA_EXDEF		(1<<19)
#define MISTA_NCS		(1<<20)
#define MISTA_TXABT		(1<<21)
#define MISTA_LC		(1<<22)
#define MISTA_TDU		(1<<23)
#define MISTA_TxBErr	(1<<24)

//MAC General Status Register
#define MGSTA_CFR		1
#define MGSTA_RXHA		(1<<1)
#define MGSTA_RFFull	(1<<2) ////RxFIFO is full  lsshi 2005-4-22 12:09
#define MGSTA_DEF		(1<<4)
#define MGSTA_PAU		(1<<5)
#define MGSTA_SQE		(1<<6)
#define MGSTA_TXHA		(1<<7)


#define n745_WriteReg(reg,val,which)      (*((volatile unsigned int *)(MAC_BASE+(which)*0x800+reg))=(val))
#define n745_ReadReg(reg,which)           (*((volatile unsigned int *)(MAC_BASE+reg+(which)*0x800)))

#define n745_WriteCam0(which,x,lsw,msw) \
		n745_WriteReg(CAM0L+(x)*CAM_ENTRY_SIZE,lsw,which);\
		n745_WriteReg(CAM0M+(x)*CAM_ENTRY_SIZE,msw,which);\
		

#define MDCCR1   0x00a00000  // MDC clock rating

#endif 
