/******************************************************************************
 *
 * Copyright (c) 2003 Windond Electronics Corp.
 * All rights reserved.
 * Winbond W90N740 Boot Loader
 *
 * Module Name:	PACKETDRIVER.C
 *
 * Created by : 
 ******************************************************************************/
#include "netbuf.h"
#include "w90p710.h"
#include "net.h"

//#define USE_RMII
//#define PHY
//#define MARVELL_6052
//#define IC_PLUS 
INT _phy=NET_PHY;
INT _rmii=1;

volatile INT MAC_NUM = 0;

__align(16) static sFrameDescriptor RxFDBaseAddr0[MaxRxFrameDescriptors];
//__align(16) static sFrameDescriptor RxFDBaseAddr1[MaxRxFrameDescriptors];
__align(16) static sFrameDescriptor TxFDBaseAddr0[MaxTxFrameDescriptors];
//__align(16) static sFrameDescriptor TxFDBaseAddr1[MaxTxFrameDescriptors];


// Global variables  used for MAC driver
#ifdef USE_RMII
volatile UINT32 gMCMDR = MCMDR_FDUP | MCMDR_SPCRC | MCMDR_RXON | 
                           MCMDR_EnMDC | MCMDR_AEP | MCMDR_ACP | MCMDR_EnRMII;
#else
volatile UINT32 gMCMDR = MCMDR_FDUP | MCMDR_SPCRC | MCMDR_RXON | 
                           MCMDR_EnMDC | MCMDR_AEP | MCMDR_ACP;
#endif
                      
volatile UINT32 gMIEN = EnTXINTR | EnTXCP | EnRXINTR | EnTDU | EnRXGD | 
                          EnCRCE | EnRP | EnPTLE | EnRXOV |
                          EnLC | EnTXABT | EnNCS | EnEXDEF | EnTXEMP |
                          EnTxBErr | EnRxBErr | EnRDU | EnCFR;
                     
volatile UINT32 gCAMCMR = CAM_ECMP;

volatile UINT32 gCTxFDPtr[2], gWTxFDPtr[2], gCRxFDPtr[2];
volatile UINT32 gCam0M_0 = 0 , gCam0L_0 = 0;
volatile UINT32 gCam0M_1 = 0 , gCam0L_1 = 0;
volatile INT TDU_Flag0, TDU_Flag1;

volatile char TxReady0=1, TxReady1=1;

NETBUF	*_TxQueue0 = NULL, *_TxQueue1 = NULL;

// Interrupt Handler Table
typedef void (*fptr)();  // function pointer
fptr IRQ_HandlerTable[19];

extern void MAC0_Tx_isr(void) ;
extern void MAC0_Rx_isr(void) ;
//extern void MAC1_Tx_isr(void) ;
//extern void MAC1_Rx_isr(void) ;

extern UCHAR  _HostMAC[];
extern UCHAR  _HostIP[];

static int resetPhyOk = 0;

void  GetMacAddress(char *mac)
{
	memcpy((char *)mac, (char *)_HostMAC, 6);
}

void  SetMacAddress(char *mac)
{
	memcpy((char *)_HostMAC, (char *)mac, 6);
}

void SetIpAddress(char *ip)
{
	memcpy((char *)_HostIP, (char *)ip, 4);
}

void GetIpAddress(char *ip)
{
	memcpy((char *)ip, (char *)_HostIP, 4);
}

void SetMacNumber(INT num)
{
 	//if ((num==0)||(num==1))	
   	//	MAC_NUM = num;	
   	MAC_NUM = 0;
}	

void SetPhyChip(INT num)
{
	_phy=NET_PHY;
}

void SetRMII(INT num)
{
	//if( num == 1)
		_rmii = 1;
	//else
	//	_rmii = 0;
		
}

// Setup Interrupt Handler Vector Table
void SysSetInterrupt(volatile UINT32 vector, void (*handler)())
{
 	IRQ_HandlerTable[vector] = handler;
}



// Interrupt Service Routine                                                                         */
__irq void IRQ_IntHandler(void)
{
// 	UINT32 IPER, ISNR;
	UINT32 irq;

// 	IPER = AIC_IPER >> 2;
// 	ISNR = AIC_ISNR;
	irq=AIC_ISR;
	//if( irq & (0x1<<13) )
	//	(*IRQ_HandlerTable[13])();
	//if( irq & (0x1<<14) )
	//	(*IRQ_HandlerTable[14])();
	if( irq & (0x1<<17) )
		(*IRQ_HandlerTable[17])();
	if( irq & (0x1<<18) )
		(*IRQ_HandlerTable[18])();

// 	if (ISNR == IPER)
// 		if((IPER >=13) && (IPER <=16) )
//   			(*IRQ_HandlerTable[ISNR])();

// 	AIC_EOSCR = 0;
}



void Mac_EnableInt()
{

    	Enable_Int(EMCTXINT0);
    	Enable_Int(EMCRXINT0);

}



void Mac_DisableInt()
{

   		Disable_Int(EMCTXINT0);
    	Disable_Int(EMCRXINT0);

}



void Mac_EnableBroadcast()
{
 	if (MAC_NUM == 0)
   		CAMCMR_0 |= CAM_ABP ;
 	else 
 	if (MAC_NUM == 1)
   		CAMCMR_1 |= CAM_ABP ;
}



void Mac_DisableBroadcast()
{
 	if (MAC_NUM == 0)
   		CAMCMR_0 &= ~CAM_ABP ;
 	else 
 	if (MAC_NUM == 1)
   		CAMCMR_1 &= ~CAM_ABP ;
}



// MII Interface Station Management Register Write
void MiiStationWrite(INT num, UINT32 PhyInAddr, UINT32 PhyAddr, UINT32 PhyWrData)
{
 	//volatile INT i = 1000;

/*	
#ifdef MARVELL_6052
	num = 0;
#endif

#ifdef IC_PLUS
	num = 0;
#endif		
*/
//if( _phy != NET_PHY )num=0; //CWS 5/20

	
 	if (num == 0)
   	{
    	MIID_0 = PhyWrData ;
    	MIIDA_0 = PhyInAddr | PhyAddr | PHYBUSY | PHYWR | MDCCR;
    	
    	//while (i--) ;
    	while ( (MIIDA_0 & PHYBUSY) )  ;
    	MIID_0 = 0 ;
   	}
 	else 
 	if (num == 1)
   	{
    	MIID_1 = PhyWrData ;
    	MIIDA_1 = PhyInAddr | PhyAddr | PHYBUSY | PHYWR | MDCCR;
    	
    	//while (i--) ;
    	while ( (MIIDA_1 & PHYBUSY) )  ;
    	MIID_1 = 0 ;
    }
   
}



// MII Interface Station Management Register Read
UINT32 MiiStationRead(INT num, UINT32 PhyInAddr, UINT32 PhyAddr)
{
 	UINT32 PhyRdData ;
/* 	
#ifdef  MARVELL_6052
	num = 0;
#endif

#ifdef  IC_PLUS
	num = 0;
#endif			
*/
//	if( _phy!=NET_PHY )num=0; //CWS 5/20

 	if (num == 0)
   	{
    	MIIDA_0 = PhyInAddr | PhyAddr | PHYBUSY | MDCCR;
    	while( (MIIDA_0 & PHYBUSY) )  ;
    	PhyRdData = MIID_0 ;  
   	}
 	else if (num == 1)
   	{
    	MIIDA_1 = PhyInAddr | PhyAddr | PHYBUSY | MDCCR;
    	while( (MIIDA_1 & PHYBUSY) )  ;
    	PhyRdData = MIID_1 ;
    }

 	return PhyRdData ;
}



// Reset PHY, Auto-Negotiation Enable
void ResetPhyChip(int num)
{
 unsigned int RdValue;
 INT t0;
 int i;

#if 0  /* adde by cmn 2005/03/18 */		
	for (i=0; i<32; i++)
	{
		uprintf("PHY REG %d = %x\n", i, MiiStationRead(num, i, PHYAD));	
	}	
#endif

 /* [2005/07/28] by cmn */
 RdValue = MiiStationRead(num, 16, PHYAD) ;
 if (!(RdValue & 0x0100))
 {
 	uprintf("WARNING, RMII is not enabled, set it by software !\n");
 	RdValue |= 0x0100;
 	MiiStationWrite(num, 16, PHYAD, RdValue); 	 
 }

 //UART_printf("Reset PHY...\n");
 MiiStationWrite(num, PHY_CNTL_REG, PHYAD, RESET_PHY); 
 while (1)
 {
 	RdValue = MiiStationRead(num, PHY_CNTL_REG, PHYAD) ;
    if ((RdValue&RESET_PHY)==0)
      break;
 }
 //uprintf("PHY 1, CTRL REG   = %x\n",MiiStationRead(num, PHY_CNTL_REG, PHYAD));
 //uprintf("PHY 1, STATUS REG = %x\n",MiiStationRead(num, PHY_STATUS_REG, PHYAD));

 MiiStationWrite(num,PHY_ANA_REG,PHYAD,DR100_TX_FULL|DR100_TX_HALF|DR10_TX_FULL|DR10_TX_HALF|IEEE_802_3_CSMA_CD);
                      
 RdValue = MiiStationRead(num, PHY_CNTL_REG, PHYAD) ;
 RdValue |= RESTART_AN;
 MiiStationWrite(num, PHY_CNTL_REG, PHYAD, RdValue); 
 //MiiStationWrite(num, PHY_CNTL_REG, PHYAD, ENABLE_AN | RESTART_AN); 

 uprintf("Wait for auto-negotiation complete...\n"); 
 t0 = 1000000;
 while (1) 	/* wait for auto-negotiation complete */
 {
 	RdValue = MiiStationRead(num, PHY_STATUS_REG, PHYAD) ;
      
    if ((RdValue & AN_COMPLETE) != 0)
    {
    	*(unsigned int volatile *)(0xfff83020) = 0x55555;
    	uprintf("OK\n");
		break;
     }

     if (!(t0--)) 
     {
		uprintf("FAILED!!\n");
				
		/* By default, we set the MAC to work under 100M/Full-duplex */
		if (num == 0)
		{
	 		MCMDR_0 |= MCMDR_OPMOD;
	 		MCMDR_0 |= MCMDR_FDUP;				
		}
		else
		{
	 		MCMDR_1 |= MCMDR_OPMOD;
	 		MCMDR_1 |= MCMDR_FDUP;				
		}
		return;
     }
   } /* end while */

	resetPhyOk = 1;
 
 //UART_printf("W90P710 MAC%d: ",num);
 RdValue = MiiStationRead(num, PHY_ANLPA_REG, PHYAD) ;
 //UART_printf("RdValue = %x", RdValue);
 if ((RdValue&0x100)!=0) // 100MB
   {
    uprintf("100MB - Full\n");
      MCMDR_0 |= MCMDR_OPMOD;
      MCMDR_0 |= MCMDR_FDUP;
   }
  else if ((RdValue&0x80)!=0)
   {
    uprintf("100MB - Half\n");
      MCMDR_0 |= MCMDR_OPMOD;
      MCMDR_0 &= ~MCMDR_FDUP;
   }
 else if ((RdValue&0x40)!=0) // Full Duplex
   {
    uprintf("10MB - Full\n");
      MCMDR_0 &= ~MCMDR_OPMOD;
      MCMDR_0 |= MCMDR_FDUP;
   }
  else 
   { 
    uprintf("10MB - Half\n");
      MCMDR_0 &= ~MCMDR_OPMOD;
      MCMDR_0 &= ~MCMDR_FDUP;
   } 
     
   return;
}





void EnableCamEntry(INT num, INT entry)
{
 	if (num == 0)
   		CAMEN_0 |= 0x00000001 << entry ;
 	else 
 	if (num == 1)
   		CAMEN_1 |= 0x00000001 << entry ;
}



void DisableCamEntry(INT num, INT entry)
{
 	if (num == 0)
   		CAMEN_0 &= ~(0x00000001 << entry) ;
 	else 
 	if (num == 1)
   		CAMEN_1 &= ~(0x00000001 << entry) ;
}



void FillCamEntry(INT num, INT entry, UINT32 msw, UINT32 lsw)
{
 	if (num==0)
   	{
    	CAMxM_Reg_0(entry) = msw;
    	CAMxL_Reg_0(entry) = lsw;
   	}
 	else if (num==1)
   	{
    	CAMxM_Reg_1(entry) = msw;
    	CAMxL_Reg_1(entry) = lsw;
   	}
 	EnableCamEntry(num,entry);
}



// Set MAC Address to CAM
void SetMacAddr(INT num)
{
 	INT i;
 	UCHAR  mac[6];

 	GetMacAddress((CHAR *)mac);
 	if (num == 0)
   	{
    	/* Copy MAC Address to global variable */
    	for (i = 0; i < (INT)MAC_ADDR_SIZE-2; i++)
       		gCam0M_0 = (gCam0M_0 << 8) | mac[i] ;

    	for (i = (INT)(MAC_ADDR_SIZE-2); i < (INT)MAC_ADDR_SIZE; i++)
       		gCam0L_0 = (gCam0L_0 << 8) | mac[i] ;
    
    	gCam0L_0 = (gCam0L_0 << 16) ;

    	FillCamEntry(0, 0, gCam0M_0, gCam0L_0);
   	}
 	else 
 	if (num == 1)
   	{
    	/* Copy MAC Address to global variable */
    	for (i = 0; i < (INT)MAC_ADDR_SIZE-2; i++)
       		gCam0M_1 = (gCam0M_1 << 8) | mac[i] ;

    	for (i = (INT)(MAC_ADDR_SIZE-2); i < (INT)MAC_ADDR_SIZE; i++)
       		gCam0L_1 = (gCam0L_1 << 8) | mac[i] ;
    	
    	gCam0L_1 = (gCam0L_1 << 16) ;

    	FillCamEntry(1, 0, gCam0M_1, gCam0L_1);
   	}
}



// Initialize Tx frame descriptor area-buffers.
void TxFDInitialize(INT num)
{
 	sFrameDescriptor 	*pFrameDescriptor;
 	sFrameDescriptor 	*pStartFrameDescriptor;
 	sFrameDescriptor 	*pLastFrameDescriptor = NULL;
 	UINT32 			i;

 	if (num == 0)
   	{
    	// Get Frame descriptor's base address.
    	TXDLSA_0 = (UINT32)TxFDBaseAddr0 | 0x80000000;
    	gWTxFDPtr[0] = gCTxFDPtr[0] = TXDLSA_0;

    	// Generate linked list.
    	pFrameDescriptor = (sFrameDescriptor *) gCTxFDPtr[0];
    	pStartFrameDescriptor = pFrameDescriptor;

    	for(i = 0; i < MaxTxFrameDescriptors; i++)
    	{
     		if (pLastFrameDescriptor == NULL)
       			pLastFrameDescriptor = pFrameDescriptor;
     		else
       			pLastFrameDescriptor->NextFrameDescriptor = (UINT32)pFrameDescriptor;

     		pFrameDescriptor->Status1 = (PaddingMode | CRCMode | MACTxIntEn);
     		pFrameDescriptor->FrameDataPtr = (UINT32)0x0;
     		pFrameDescriptor->Status2 = (UINT32)0x0;
     		pFrameDescriptor->NextFrameDescriptor = NULL;

     		pLastFrameDescriptor = pFrameDescriptor;
     		pFrameDescriptor++;
    	}

    	// Make Frame descriptor to ring buffer type.
    	pFrameDescriptor--;
    	pFrameDescriptor->NextFrameDescriptor = (UINT32)pStartFrameDescriptor;
   	}
#if 0   	
 	else 
 	if (num == 1)
   	{
    	// Get Frame descriptor's base address.
    	TXDLSA_1 = (UINT32)TxFDBaseAddr1 | 0x80000000;
    	gWTxFDPtr[1] = gCTxFDPtr[1] = TXDLSA_1;

    	// Generate linked list.
    	pFrameDescriptor = (sFrameDescriptor *) gCTxFDPtr[1];
    	pStartFrameDescriptor = pFrameDescriptor;

    	for (i = 0; i < MaxTxFrameDescriptors; i++)
    	{
     		if (pLastFrameDescriptor == NULL)
       			pLastFrameDescriptor = pFrameDescriptor;
     		else
       			pLastFrameDescriptor->NextFrameDescriptor = (UINT32)pFrameDescriptor;

     		pFrameDescriptor->Status1 = (PaddingMode | CRCMode | MACTxIntEn);
     		pFrameDescriptor->FrameDataPtr = (UINT32)0x0;
     		pFrameDescriptor->Status2 = (UINT32)0x0;
     		pFrameDescriptor->NextFrameDescriptor = NULL;

     		pLastFrameDescriptor = pFrameDescriptor;
     		pFrameDescriptor++;
    	}

    	// Make Frame descriptor to ring buffer type.
    	pFrameDescriptor--;
    	pFrameDescriptor->NextFrameDescriptor = (UINT32)pStartFrameDescriptor;
   	}
#endif   	
}



// Initialize Rx frame descriptor area-buffers.
void RxFDInitialize(INT num)
{
 	sFrameDescriptor 	*pFrameDescriptor;
 	sFrameDescriptor 	*pStartFrameDescriptor;
 	sFrameDescriptor 	*pLastFrameDescriptor = NULL;
 	UINT32 			i;

 	if (num == 0)
   	{
    	// Get Frame descriptor's base address.
    	RXDLSA_0 = (UINT32)RxFDBaseAddr0 | 0x80000000;
    	gCRxFDPtr[0] = RXDLSA_0;

    	// Generate linked list.
    	pFrameDescriptor = (sFrameDescriptor *) gCRxFDPtr[0];
    	pStartFrameDescriptor = pFrameDescriptor;

    	for (i = 0; i < MaxRxFrameDescriptors; i++)
    	{
     		if (pLastFrameDescriptor == NULL)
       			pLastFrameDescriptor = pFrameDescriptor;
     		else
       			pLastFrameDescriptor->NextFrameDescriptor = (UINT32)pFrameDescriptor;

     		pFrameDescriptor->Status1 = RXfOwnership_DMA;
     		pFrameDescriptor->FrameDataPtr = (UINT32)(NetBuf_Allocate());
     		pFrameDescriptor->Status2 = (UINT32)0x0;
     		pFrameDescriptor->NextFrameDescriptor = NULL;

     		pLastFrameDescriptor = pFrameDescriptor;
     		pFrameDescriptor++;
    	}

    	// Make Frame descriptor to ring buffer type.
    	pFrameDescriptor--;
    	pFrameDescriptor->NextFrameDescriptor = (UINT32)pStartFrameDescriptor;
   	}
#if 0   	
 	else 
 	if (num == 1)
   	{
    	// Get Frame descriptor's base address.
    	RXDLSA_1 = (UINT32)RxFDBaseAddr1 | 0x80000000;
    	gCRxFDPtr[1] = RXDLSA_1;

    	// Generate linked list.
    	pFrameDescriptor = (sFrameDescriptor *) gCRxFDPtr[1];
    	pStartFrameDescriptor = pFrameDescriptor;

    	for (i = 0; i < MaxRxFrameDescriptors; i++)
    	{
     		if (pLastFrameDescriptor == NULL)
       			pLastFrameDescriptor = pFrameDescriptor;
     		else
       			pLastFrameDescriptor->NextFrameDescriptor = (UINT32)pFrameDescriptor;

     		pFrameDescriptor->Status1 = RXfOwnership_DMA;
     		pFrameDescriptor->FrameDataPtr = (UINT32)(NetBuf_Allocate());
     		pFrameDescriptor->Status2 = (UINT32)0x0;
     		pFrameDescriptor->NextFrameDescriptor = NULL;

     		pLastFrameDescriptor = pFrameDescriptor;
     		pFrameDescriptor++;
    	}

    	// Make Frame descriptor to ring buffer type.
    	pFrameDescriptor--;
    	pFrameDescriptor->NextFrameDescriptor = (UINT32)pStartFrameDescriptor;
   	}
#endif   	
}



// set Registers related with MAC.
void ReadyMac(INT num)
{
 	if (num == 0)
   	{
    	MIEN_0 = gMIEN ;
    	//if( _rmii==1 )
    	//{
	    //	MCMDR_0 = gMCMDR | MCMDR_EnRMII;
	    //}
	    //else
	    //{
	    	MCMDR_0 = gMCMDR ;
	    //}
	    	
   	}
 	else 
 	if (num == 1)
   	{
    	MIEN_1 = gMIEN ;
    	//if( _rmii==1 )
    	//{
	    //	MCMDR_1 = gMCMDR | MCMDR_EnRMII;
	    //}
	    //else
	    //{
	    	MCMDR_1 = gMCMDR ;
	    //}
   }
}



// MAC Transfer Start for interactive mode
void MacTxGo(INT num)
{
 	// Enable MAC Transfer
 	if (num == 0)
   	{
    	if (!(MCMDR_0&MCMDR_TXON))
      		MCMDR_0 |= MCMDR_TXON ;

    	if (TDU_Flag0==1)
      	{         
       		TDU_Flag0=0;
       		TSDR_0 = 0;  
      	}         
   	} 
 	else 
 	if (num == 1)
   	{
    	if (!(MCMDR_1&MCMDR_TXON))
      		MCMDR_1 |= MCMDR_TXON ;

    	if (TDU_Flag1==1)
      	{	         
       		TDU_Flag1=0;
       		TSDR_1 = 0;
      	}         
   	} 
}



// Initialize MAC Controller
void Mac_Initialize()
{
 	INT num;
 
 	num = MAC_NUM;	
 
 	if (num == 0)
   	{
		// Reset MAC   
		MCMDR_0 |= MCMDR_SWR;
        while (MCMDR_0 & MCMDR_SWR) ;
		
    	// MAC interrupt vector setup.
    	SysSetInterrupt(EMCTXINT0, MAC0_Tx_isr) ;
    	SysSetInterrupt(EMCRXINT0, MAC0_Rx_isr) ;

    	// Set the Tx and Rx Frame Descriptor
    	TxFDInitialize(num) ;
    	RxFDInitialize(num) ;

    	// Set the CAM Control register and the MAC address value
    	FillCamEntry(0, 0, gCam0M_0, gCam0L_0);
    	CAMCMR_0 = gCAMCMR ;

    	// Enable MAC Tx and Rx interrupt.
    	Enable_Int(EMCTXINT0);
    	Enable_Int(EMCRXINT0);

    	TDU_Flag0=0;

    	// Configure the MAC control registers.
    	ReadyMac(num) ;
   	}
#if 0   	
 	else 
 	if (num == 1)
   	{
		// Reset MAC   
		MCMDR_1 |= MCMDR_SWR;
        while (MCMDR_1 & MCMDR_SWR) ;

    	// MAC interrupt vector setup.
    	SysSetInterrupt(EMCTXINT1, MAC1_Tx_isr) ;
    	SysSetInterrupt(EMCRXINT1, MAC1_Rx_isr) ;

    	// Set the Tx and Rx Frame Descriptor
    	TxFDInitialize(num) ;
    	RxFDInitialize(num) ;

    	// Set the CAM Control register and the MAC address value
    	FillCamEntry(1, 0, gCam0M_1, gCam0L_1);
    	CAMCMR_1 = gCAMCMR ;

    	// Enable MAC Tx and Rx interrupt.
    	Enable_Int(EMCTXINT1);
    	Enable_Int(EMCRXINT1);
    
    	TDU_Flag1=0;
    
    	// Configure the MAC control registers.
    	ReadyMac(num) ;
   	}
#endif
	*(unsigned int volatile *)(0xfff83020) = 0x50000;
 	// Set PHY operation mode
 	ResetPhyChip(num) ;

	if(resetPhyOk == 1)
		*(unsigned int volatile *)(0xfff83020) = 0x55555;
 	// Set MAC address to CAM
 	SetMacAddr(num) ;


	{
		INT tmp;
		*((volatile UINT32 *)0x38)=(UINT32)IRQ_IntHandler;
		__asm
		{
			MRS	tmp, CPSR
			BIC	tmp, tmp, 0x80
			MSR	CPSR_c, tmp
		}
	}

}



void  Mac_ShutDown()
{
#if 1 //CMN
 	if (MAC_NUM == 0)
   		MCMDR_0 &= ~(MCMDR_RXON|MCMDR_TXON) ;
 	else 
 	if (MAC_NUM == 1)
   		MCMDR_1 &= ~(MCMDR_RXON|MCMDR_TXON) ;
#endif   
}       



// Send ethernet frame function
INT Mac_SendPacket(NETBUF *netbuf)
{
 	sFrameDescriptor *psTxFD;
 	UINT32         *pTXFDStatus1;

	netbuf->txNext = NULL;

	Mac_DisableInt();

 	if (MAC_NUM == 0)
   	{
    	// Get Tx frame descriptor & data pointer
    	psTxFD = (sFrameDescriptor *)gWTxFDPtr[0] ;

    	pTXFDStatus1 = (UINT32 *)&psTxFD->Status1;

		if (!TxReady0) // || (*pTXFDStatus1 & TXfOwnership_DMA))
		{
			//uprintf("\nTXFSM0=0x%08x\n", TXFSM_0);
			if (_TxQueue0 == NULL)
				_TxQueue0 = netbuf;
			else
			{			
				NETBUF	*ptr = _TxQueue0;
				
				while (ptr->txNext != NULL)
					ptr = ptr->txNext;
				ptr->txNext = netbuf;
			}
			Mac_EnableInt();
			return 1;
		}

    
    	psTxFD->FrameDataPtr=(UINT32)netbuf->packet;

    	// Set TX Frame flag & Length Field
    	//netbuf->len += 60;
    	psTxFD->Status2 = (UINT32)(netbuf->len & 0xffff);

    	// Cheange ownership to DMA
    	psTxFD->Status1 |= TXfOwnership_DMA;

    	TxReady0 = 0;

    	// Enable MAC Tx control register
    	MacTxGo(0);

    	// Change the Tx frame descriptor for next use
    	gWTxFDPtr[0] = (UINT32)(psTxFD->NextFrameDescriptor);
   	}
   #if 0	
 	else 
 	if (MAC_NUM == 1)
   	{
    	// Get Tx frame descriptor & data pointer
    	psTxFD = (sFrameDescriptor *)gWTxFDPtr[1] ;

    	pTXFDStatus1 = (UINT32 *)&psTxFD->Status1;

		if (!TxReady1)  // || (*pTXFDStatus1 & TXfOwnership_DMA))
		{
//			uprintf("\nTXFSM1=0x%08x\n", TXFSM_1);
			if (_TxQueue1 == NULL)
				_TxQueue1 = netbuf;
			else
			{			
				NETBUF	*ptr = _TxQueue1;
				
				while (ptr->txNext != NULL)
					ptr = ptr->txNext;
				ptr->txNext = netbuf;
			}
			Mac_EnableInt();
			return 1;
		}

    	psTxFD->FrameDataPtr=(UINT32)netbuf->packet;

    	// Set TX Frame flag & Length Field
    	psTxFD->Status2 = (UINT32)(netbuf->len & 0xffff);

    	// Cheange ownership to DMA
    	psTxFD->Status1 |= TXfOwnership_DMA;

    	TxReady1 = 0;

    	// Enable MAC Tx control register
    	MacTxGo(1);

    	// Change the Tx frame descriptor for next use
    	gWTxFDPtr[1] = (UINT32)(psTxFD->NextFrameDescriptor);
   	}
#endif   	
   	Mac_EnableInt();
 	return 1 ;
}



// Interrupt Service Routine for MAC0 Tx
void MAC0_Tx_isr(void)
{   
 	sFrameDescriptor 	*pTxFDptr;
 	UINT32 			Status, RdValue;

 	//if (TxReady0 == 1) //CWS
   	//	return;

 	RdValue = MISTA_0;
 	MISTA_0 = RdValue&0xffff0000;

 	if (RdValue & MISTA_TDU)
   		TDU_Flag0 = 1;
 
 	if (RdValue & MISTA_TxBErr)
   	{
    	MCMDR_0 |= MCMDR_SWR;
        while (MCMDR_0 & MCMDR_SWR) ;
    	Mac_Initialize();
   	}
 	else
   	{
    	pTxFDptr = (sFrameDescriptor *) gCTxFDPtr[0];

    	Status = (pTxFDptr->Status2 >> 16) & 0xffff;
    	if (Status & TXFD_TXCP)
      	{
       		TxReady0 = 1;

       		if (Status & ( TXFD_TXABT | TXFD_DEF | TXFD_PAU | TXFD_EXDEF |
                      TXFD_NCS | TXFD_SQE | TXFD_LC | TXFD_TXHA) )
         		; 

       		// Clear Framedata pointer already used.
       		pTxFDptr->Status2 = (UINT32)0x0;

       		NetBuf_FreeIR((NETBUF *)pTxFDptr->FrameDataPtr);
       		gCTxFDPtr[0] = (UINT32)pTxFDptr->NextFrameDescriptor ;
       		
       		if (_TxQueue0 != NULL)
       		{
       			NETBUF	*netbuf = _TxQueue0;
       			
       			_TxQueue0 = _TxQueue0->txNext;
       			netbuf->txNext = NULL;
       			
       			Mac_SendPacket(netbuf);
       		}
      	}
   	}
}

#if 0

// Interrupt Service Routine for MAC1 Tx
void MAC1_Tx_isr(void)
{   
 	sFrameDescriptor 	*pTxFDptr;
 	UINT32 			Status, RdValue;

 	//if (TxReady1 == 1) //CWS
   	//	return;

 	RdValue = MISTA_1;
 	MISTA_1 = RdValue&0xffff0000;

 	if (RdValue & MISTA_TDU)
   		TDU_Flag1 = 1;
 
 	if (RdValue & MISTA_TxBErr)
   	{
    	MCMDR_1 |= MCMDR_SWR;
        while (MCMDR_1 & MCMDR_SWR) ;
    	Mac_Initialize();
    	TxReady1 = 0;
   	}
 	else
   	{
    	pTxFDptr = (sFrameDescriptor *) gCTxFDPtr[1];

    	Status = (pTxFDptr->Status2 >> 16) & 0xffff;
    	if (Status & TXFD_TXCP)
      	{
       		TxReady1 = 1;

       		if (Status & ( TXFD_TXABT | TXFD_DEF | TXFD_PAU | TXFD_EXDEF |
                      TXFD_NCS | TXFD_SQE | TXFD_LC | TXFD_TXHA) )
         		; 

       		// Clear Framedata pointer already used.
       		pTxFDptr->Status2 = (UINT32)0x0;

       		NetBuf_FreeIR((NETBUF *)pTxFDptr->FrameDataPtr);
       		gCTxFDPtr[1] = (UINT32)pTxFDptr->NextFrameDescriptor ;

       		if (_TxQueue1 != NULL)
       		{
       			NETBUF	*netbuf = _TxQueue1;
       			
       			_TxQueue1 = _TxQueue1->txNext;
       			netbuf->txNext = NULL;
       			
       			Mac_SendPacket(netbuf);
       		}
      	}
   	}
}

#endif

//#define MAC0_DEBUG
// Interrupt Service Routine for MAC0 Rx
void MAC0_Rx_isr(void)
{
 	sFrameDescriptor 	*pRxFDptr ;
 	UINT32 			RxStatus ;
 	UINT32 			CRxPtr;
 	UINT32 			RdValue;
 	NETBUF   			*netbuf;
#ifdef MAC0_DEBUG
 static UINT32 	i=0;
#endif 

 	RdValue = MISTA_0;
 	MISTA_0 = RdValue&0x0000ffff;
 
 	if (RdValue & MISTA_RxBErr)
   	{
    	MCMDR_0 |= MCMDR_SWR;
        while (MCMDR_0 & MCMDR_SWR) ;
    	Mac_Initialize();
   	} 
 	else
   	{
    	if (RdValue & (MISTA_CFR | MISTA_CRCE | MISTA_PTLE | MISTA_ALIE | MISTA_RP))
      		uprintf("Rx error, status:%x\n",RdValue) ;
    	else
    	{
     		// Get current frame descriptor
     		CRxPtr = CRXDSA_0 ;

     		do
     		{
      			// Get Rx Frame Descriptor
      			pRxFDptr = (sFrameDescriptor *)gCRxFDPtr[0];

      			if ((pRxFDptr->Status1|RXfOwnership_CPU)==RXfOwnership_CPU)
        		{
#ifdef MAC0_DEBUG
        			if( (i & 0xF)== 0x0 )
             			uprintf("\nMAC0 - %03d", pRxFDptr->Status1 & 0xffff);
          			else
          				uprintf(" %03d", pRxFDptr->Status1 & 0xffff);
          			i++;
#endif  
         			RxStatus = (pRxFDptr->Status1 >> 16) & 0xffff;

         			// If Rx frame is good, then process received frame
         			if (RxStatus & RXFD_RXGD)
         			{
         				if (PacketProcessor((UCHAR *)pRxFDptr->FrameDataPtr, pRxFDptr->Status1 & 0xffff) == 0)
         					;
          				else
          				if ((netbuf = NetBuf_AllocateIR()) == NULL)
            				; //uprintf("No free buffer\n");
          				else
          				{
           					if (_iqueue_last == NULL)
             				{
              					_iqueue_last = (NETBUF *)pRxFDptr->FrameDataPtr;
              					_iqueue_first = _iqueue_last;
             				}
           					else
             				{
              					_iqueue_last->next = (NETBUF *)pRxFDptr->FrameDataPtr;
              					_iqueue_last = _iqueue_last->next;
             				}
           					_iqueue_last->len = pRxFDptr->Status1 & 0xffff;
           					_iqueue_last->next = NULL;
          				
             
           					pRxFDptr->FrameDataPtr = (unsigned long)netbuf;
          				}
         			}
        		}
      			else
        			break;

      			// Change ownership to DMA for next use
      			pRxFDptr->Status1 = RXfOwnership_DMA;

      			// Get Next Frame Descriptor pointer to process
      			gCRxFDPtr[0] = (UINT32)(pRxFDptr->NextFrameDescriptor) ;
     		
     		} 	while (CRxPtr != gCRxFDPtr[0]);
    	}

    	if (RdValue & MISTA_RDU)
      		RSDR_0 = 0;
   	}
}

#if 0

// Interrupt Service Routine for MAC1 Rx
void MAC1_Rx_isr(void)
{
 	sFrameDescriptor 	*pRxFDptr ;
 	UINT32 			RxStatus;
 	UINT32 			CRxPtr;
 	UINT32 			RdValue;
 	NETBUF   			*netbuf;

 	RdValue = MISTA_1;
 	MISTA_1 = RdValue & 0x0000ffff;
 
 	if (RdValue & MISTA_RxBErr)
   	{
    	MCMDR_1 |= MCMDR_SWR;
        while (MCMDR_1 & MCMDR_SWR) ;
    	Mac_Initialize();
   	} 
 	else
   	{
    	if (RdValue & (MISTA_CFR | MISTA_CRCE | MISTA_PTLE | MISTA_ALIE | MISTA_RP))
      		uprintf("Rx error, status:%x\n",RdValue) ;
    	else
    	{
     		// Get current frame descriptor
     		CRxPtr = CRXDSA_1 ;

     		do
     		{
      			// Get Rx Frame Descriptor
      			pRxFDptr = (sFrameDescriptor *)gCRxFDPtr[1];

      			if ((pRxFDptr->Status1|RXfOwnership_CPU)==RXfOwnership_CPU)
        		{
         			
         			RxStatus = (pRxFDptr->Status1 >> 16) & 0xffff;

         			// If Rx frame is good, then process received frame
         			if (RxStatus & RXFD_RXGD)
         			{
         				if (PacketProcessor((UCHAR *)pRxFDptr->FrameDataPtr, pRxFDptr->Status1 & 0xffff) == 0)
         					;
         				else
          				if ((netbuf = NetBuf_AllocateIR()) == NULL)
            				; //uprintf("No free buffer\n");
          				else
          				{
           					if (_iqueue_last == NULL)
             				{
              					_iqueue_last = (NETBUF *)pRxFDptr->FrameDataPtr;
              					_iqueue_first = _iqueue_last;
             				}
           					else
             				{
              					_iqueue_last->next = (NETBUF *)pRxFDptr->FrameDataPtr;
              					_iqueue_last = _iqueue_last->next;
             				}
           						_iqueue_last->len = pRxFDptr->Status1 & 0xffff;
           						_iqueue_last->next = NULL;
             
           					pRxFDptr->FrameDataPtr = (unsigned long)netbuf;
          				}
         			}
        		}
      			else
        			break;

      			// Change ownership to DMA for next use
      			pRxFDptr->Status1 = RXfOwnership_DMA;

      			// Get Next Frame Descriptor pointer to process
      			gCRxFDPtr[1] = (UINT32)(pRxFDptr->NextFrameDescriptor) ;
     		
     		} 	while (CRxPtr != gCRxFDPtr[1]);
    	}

    	if (RdValue & MISTA_RDU)
      		RSDR_1 = 0;
   	}
}

#endif
