/****************************************************************************
 * 
 * Copyright (c) 2004 - 2006 Winbond Electronics Corp. All rights reserved. 
 *
 ****************************************************************************/
 
/****************************************************************************
 * 
 * FILENAME
 *    wb_gdma.h
 *
 * VERSION
 *    1.0
 *
 * DESCRIPTION
 *	  Winbond FA5933 GDMA driver header.  
 *
 * DATA STRUCTURES
 *    None
 *
 * FUNCTIONS
 *    None
 *
 * HISTORY
 *    11/23/2006			Ver 1.0 Created by NS22 HHWu 
 *
 * REMARK
 *    None
 *
 *************************************************************************/
#ifndef _WB_FA5933_GDMA_H_
#define _WB_FA5933_GDMA_H_

#include <asm/irq.h>


/* 12.22 add, NS22 HHWu */
static wait_queue_head_t G0Wait, G1Wait;
static volatile int nG0TXFinish, nG1TXFinish;

/* if enable debug message, should take care the GDMA timing issue */
//#define WB_DEBUG_GDMA

#ifdef WB_DEBUG_GDMA
#define WB_PRINTK_GDMA(fmt, arg...)		printk(fmt, ##arg)
#else
#define WB_PRINTK_GDMA(fmt, arg...)
#endif 

/* define struct GDMA_PARAM_T ucStatus */
#define GDMA_INIT_OK		0x01
#define GDMA_INT_OCCUR		0x02
#define GDMA_INT_TERR		0x04  // GDMA transfer error

/* define GDMA channel */
#define GDMA0	0
#define GDMA1	1

/* define GDMA transfer mode */
#define GDMA_nXDREQ_MODE		0x01 
#define GDMA_DEMAND_MODE		0x02
#define GDMA_SINGLE_MODE		0x04
#define GDMA_BLOCK_MODE			0x08
#define GDMA_BURST_MODE			0x10
#define GDMA_SOFTWARE_MODE		0x20

/* define GDMA nXDREQ & nXDACK */
#define GDMA_REQ_ATV_HIGH		0x01
#define GDMA_REQ_ATV_LOW		0x02  // default
#define GDMA_ACK_ATV_HIGH		0x04
#define GDMA_ACK_ATV_LOW		0x08  // default

/* define GDMA SADIR & DADIR */
#define GDMA_SADIR_INCREASE		0  // default
#define GDMA_SADIR_DECREASE		1
#define GDMA_DADIR_INCREASE		2  // default
#define GDMA_DADIR_DECREASE		3

/* define GDMA transfer width */
#define GDMA_Bit8		0 
#define GDMA_Bit16		1
#define GDMA_Bit32		2

#define gdma_outpw(port,value)     ( *( (unsigned volatile int *) port ) = value )
#define gdma_inpw(port)            ( *( (unsigned volatile int *) port ) )

/* define FA5933 EBI register */
#define EBIBASEADDR				0xFFF01000
#define FA5933_EBI_EXT3CON		(EBIBASEADDR + 0x0024)  // R/W

/* define for FA5933_EBI_EXT3CON */
#define EBI_ACS_0			0x0000
#define EBI_ACS_1			0x0020
#define EBI_ACS_2			0x0040
#define EBI_ACS_3			0x0060
#define EBI_ACS_4			0x0080
#define EBI_ACS_5			0x00a0
#define EBI_ACS_6			0x00c0
#define EBI_ACS_7			0x00e0

#define EBI_COS_0			0x0000
#define EBI_COS_1			0x0004
#define EBI_COS_2			0x0008
#define EBI_COS_3			0x000c
#define EBI_COS_4			0x0010
#define EBI_COS_5			0x0014
#define EBI_COS_6			0x0018
#define EBI_COS_7			0x001c

#define EBI_DBWD_8_BIT		0x01
#define EBI_DBWD_16_BIT		0x02
#define EBI_DBWD_32_BIT		0x03

#define EBI_COH_0			0x0000
#define EBI_COH_1			0x0100
#define EBI_COH_2			0x0200
#define EBI_COH_3			0x0300
#define EBI_COH_4			0x0400
#define EBI_COH_5			0x0500
#define EBI_COH_6			0x0600
#define EBI_COH_7			0x0700

#define EBI_ACC_1			0x0800
#define EBI_ACC_2			0x1000
#define EBI_ACC_3			0x1800
#define EBI_ACC_4			0x2000
#define EBI_ACC_5			0x2800
#define EBI_ACC_6			0x3000
#define EBI_ACC_7			0x3800
#define EBI_ACC_9			0x4000
#define EBI_ACC_11			0x4800
#define EBI_ACC_13			0x5000
#define EBI_ACC_15			0x5800
#define EBI_ACC_17			0x6000
#define EBI_ACC_19			0x6800
#define EBI_ACC_21			0x7000
#define EBI_ACC_23			0x7800

#define EBI_SIZE_256K		0x00000
#define EBI_SIZE_512K		0x10000
#define EBI_SIZE_1M			0x20000
#define EBI_SIZE_2M			0x30000
#define EBI_SIZE_4M			0x40000
#define EBI_SIZE_8M			0x50000
#define EBI_SIZE_16M		0x60000

/* define FA5933 GPIO register */
#define GPIOBASEADDR				0xFFF83000
#define FA5933_GPIO_CFG1			(GPIOBASEADDR + 0x0010)  // R/W

/* define FA5933 AIC register */
#define AICBASEADDR				0xFFF82000
#define FA5933_AIC19			(AICBASEADDR + 0x004C)
#define FA5933_AIC_ISR			(AICBASEADDR + 0x0108)  // R
#define FA5933_AIC_IPER			(AICBASEADDR + 0x010C)  // R
#define FA5933_IMR				(AICBASEADDR + 0x0114)  // R
#define FA5933_AIC_EOSCR		(AICBASEADDR + 0x0130)  // W
#define FA5933_AIC_SCCR			(AICBASEADDR + 0x012C)  // W

/* define FA5933 GDMA register */
#define GDAMBASEADDR			0xFFF04000
#define FA5933_GDMA_CTL0		(GDAMBASEADDR + 0x00)  // R/W 
#define FA5933_GDMA_SRCB0		(GDAMBASEADDR + 0x04)  // R/W 
#define FA5933_GDMA_DSTB0		(GDAMBASEADDR + 0x08)  // R/W 
#define FA5933_GDMA_TCNT0		(GDAMBASEADDR + 0x0C)  // R/W 
#define FA5933_GDMA_CSRC0		(GDAMBASEADDR + 0x10)  // R
#define FA5933_GDMA_CDST0		(GDAMBASEADDR + 0x14)  // R
#define FA5933_GDMA_CTCNT0		(GDAMBASEADDR + 0x18)  // R

#define FA5933_GDMA_CTL1		(GDAMBASEADDR + 0x20)  // R/W 
#define FA5933_GDMA_SRCB1		(GDAMBASEADDR + 0x24)  // R/W 
#define FA5933_GDMA_DSTB1		(GDAMBASEADDR + 0x28)  // R/W 
#define FA5933_GDMA_TCNT1		(GDAMBASEADDR + 0x2C)  // R/W 
#define FA5933_GDMA_CSRC1		(GDAMBASEADDR + 0x30)  // R
#define FA5933_GDMA_CDST1		(GDAMBASEADDR + 0x34)  // R
#define FA5933_GDMA_CTCNT1		(GDAMBASEADDR + 0x38)  // R

// define for FA5933_GDMA_CTL0/1
#define GDMA_REQ_SEL_nXDREQ			(0x1 << 26)  // use nXDREQ
#define GDMA_REQ_ATV				(0x1 << 25)  // nXDREQ is HIGH active			
#define GDMA_ACK_ATV				(0x1 << 24)  // nXDACK is HIGH active		
#define GDMA_SABNDERR				(0x1 << 22)  // R
#define GDMA_DABNDERR				(0x1 << 21)  // R
#define GDMA_GDMATERR				(0x1 << 20)  
#define GDMA_AUTOIEN				(0x1 << 19) 
#define GDMA_TC						(0x1 << 18)	  
#define GDMA_BLOCK					(0x1 << 17)	
#define GDMA_SOFTREQ				(0x1 << 16)
#define GDMA_DM						(0x1 << 15)  // demand mode enable
#define GDMA_TWS_8_Bit				(0x0 << 12)  // transfer width 8 bits
#define GDMA_TWS_16_Bit				(0x1 << 12)  // transfer width 16 bits
#define GDMA_TWS_32_Bit				(0x2 << 12)  // transfer width 32 bits
#define GDMA_SBMS					(0x1 << 11)  // block mode
#define GDMA_BME					(0x1 << 9)	 // burst mode
#define GDMA_SIEN					(0x1 << 8)   
#define GDMA_SAFIX					(0x1 << 7)
#define GDMA_DAFIX					(0x1 << 6)
#define GDMA_SADIR					(0x1 << 5)   // source address is decremented 
#define GDMA_DADIR					(0x1 << 4)   // destination address is decremented 
#define GDMA_GDMAMS_nXDREQ			(0x1 << 2)   // external nXDREQ mode for externa device 
#define GDMA_GDMAEN					0x1

/* define GDMA & EBI parameter structure  */
// GDMA configure structure, defined for gdma_Init() 
typedef void (GDMA_FUNC_T)(void);

typedef struct FA5933_GDMA_PARAM{
	unsigned char ucChannel;	// GDMA channel
	unsigned char ucMode;
	unsigned char ucWidth;
	unsigned char ucReq;
	unsigned char ucAck;
	unsigned char ucSrcDir;
	unsigned char ucDestDir;
	
	unsigned volatile char ucStatus[2];  // Note: volatile should before char or int	
	GDMA_FUNC_T *fnGDMACallBack[2];  
}GDMA_PARAM_T;

// GDMA data transfer structure
typedef struct FA5933_GDMA_DATA_TRANSFER{
	unsigned char ucChannel;
	unsigned int uSrcAddr;
	unsigned int uDestAddr;
	unsigned int uDataSize;
}GDMA_DATA_T;

// EBI cofnigure structure 
typedef struct FA5933_EBI_PARAM{	
	unsigned int uIOBaseAddr;	// EBI IO address
	unsigned int uSize;
	unsigned int uACC;
	unsigned int uCOH;
	unsigned int uACS;
	unsigned int uCOS;
	unsigned int uDBWD;
}EBI_PARAM_T;

/* define GDMA macro & inline functions */
// Note: GDMA_SOFTREQ bit should be set with GDMA_GDMAEN at the same time
#define gdma_Config_Transfer_Mode(ch, mode)\
		{	unsigned volatile int val = 0;\
			if(mode & GDMA_nXDREQ_MODE) val |= (GDMA_REQ_SEL_nXDREQ | GDMA_GDMAMS_nXDREQ);\
		/*	if(mode & GDMA_SOFTWARE_MODE) val |= GDMA_SOFTREQ;*/ \
			if(mode & GDMA_DEMAND_MODE) val |= GDMA_DM;\
			if(mode & GDMA_BLOCK_MODE) val |= GDMA_SBMS;\
			if(mode & GDMA_BURST_MODE) val |= GDMA_BME;\
			WB_PRINTK_GDMA("GDMA: channel[%d] mode[0x%08x]\n", ch, val);\
			if(ch == 0)	gdma_outpw(FA5933_GDMA_CTL0, val);\
			if(ch == 1)	gdma_outpw(FA5933_GDMA_CTL1, val);\
		}
		
#define gdma_Config_Transfer_Width(ch, width)\
		{	unsigned volatile int val = 0;\
			if(width == GDMA_Bit16) val = GDMA_TWS_16_Bit;\
			if(width == GDMA_Bit32) val = GDMA_TWS_32_Bit;\
			WB_PRINTK_GDMA("GDMA: channel[%d] widht[0x%08x]\n", ch, val);\
			if(ch == 0)	gdma_outpw(FA5933_GDMA_CTL0, gdma_inpw(FA5933_GDMA_CTL0) | val);\
			if(ch == 1)	gdma_outpw(FA5933_GDMA_CTL1, gdma_inpw(FA5933_GDMA_CTL1) | val);\
		}		
		
#define gdma_Config_REQ_ACK_Active(ch, req, ack)\
		{	unsigned volatile int val = 0;\
			if(req == GDMA_REQ_ATV_HIGH) val |= GDMA_REQ_ATV;\
			if(ack == GDMA_ACK_ATV_HIGH) val |= GDMA_ACK_ATV;\
			WB_PRINTK_GDMA("GDMA: channel[%d] req_ack[0x%08x]\n", ch, val);\
			if(ch == 0)	gdma_outpw(FA5933_GDMA_CTL0, gdma_inpw(FA5933_GDMA_CTL0) | val);\
			if(ch == 1)	gdma_outpw(FA5933_GDMA_CTL1, gdma_inpw(FA5933_GDMA_CTL1) | val);\
		}		
		
#define gdma_Conifg_SIEN(ch)\
		if(ch == 0)	gdma_outpw(FA5933_GDMA_CTL0, gdma_inpw(FA5933_GDMA_CTL0) | GDMA_SIEN);\
		if(ch == 1)	gdma_outpw(FA5933_GDMA_CTL1, gdma_inpw(FA5933_GDMA_CTL1) | GDMA_SIEN);\
		
#define gdma_Count_TCNT(ch, size)\
		{	unsigned volatile int reg = 0, cnt = 0;\
			if(ch == 0){ reg = (gdma_inpw(FA5933_GDMA_CTL0) & 0x3000) >> 12;\
				if(reg == 0) cnt = size;\
				if(reg == 1) cnt = size/2;\
				if(reg == 2) cnt = size/4;\
				if(gdma_inpw(FA5933_GDMA_CTL0) & GDMA_BME) cnt = cnt/4;\
				gdma_outpw(FA5933_GDMA_TCNT0, cnt);\
			}\
			if(ch == 1){ reg = (gdma_inpw(FA5933_GDMA_CTL1) & 0x3000) >> 12;\
				if(reg == 0) cnt = size;\
				if(reg == 1) cnt = size/2;\
				if(reg == 2) cnt = size/4;\
				if(gdma_inpw(FA5933_GDMA_CTL1) & GDMA_BME) cnt = cnt/4;\
				gdma_outpw(FA5933_GDMA_TCNT1, cnt);\
			}\
			/*WB_PRINTK_GDMA("GDMA: channel[%d] cnt[%d]\n", ch, cnt); // mark timing issue */ \
		} 
						
#define gdma_Set_Source_Address(ch, src)\
		{	if(ch == 0)	gdma_outpw(FA5933_GDMA_SRCB0, src | 0x80000000);\
			if(ch == 1)	gdma_outpw(FA5933_GDMA_SRCB1, src  | 0x80000000);\
			/*WB_PRINTK_GDMA("GDMA: ch[%d] src[0x%08x]\n", ch, src); // mark timing issue */  \
		}
		
#define gdma_Set_Destination_Address(ch, dest)\
		{	if(ch == 0)	gdma_outpw(FA5933_GDMA_DSTB0, dest | 0x80000000);\
			if(ch == 1)	gdma_outpw(FA5933_GDMA_DSTB1, dest | 0x80000000);\
			/*WB_PRINTK_GDMA("GDMA: ch[%d] dest[0x%08x]\n", ch, dest); // mark timing issue */ \
		}
		
#define gdma_Config_ADDR_DIR(ch, sr, dr)\
		{	unsigned volatile int reg = 0;\
			if(sr == GDMA_SADIR_DECREASE) reg |= GDMA_SADIR;\
			if(dr == GDMA_DADIR_DECREASE) reg |= GDMA_DADIR;\
			if(ch == 0) gdma_outpw(FA5933_GDMA_CTL0, gdma_inpw(FA5933_GDMA_CTL0) | reg);\
			if(ch == 1) gdma_outpw(FA5933_GDMA_CTL1, gdma_inpw(FA5933_GDMA_CTL1) | reg);\
			WB_PRINTK_GDMA("GDMA: ch[%d] sadir[%d] dadir[%d]\n", ch, sr, dr);\
		}		
				

//EBI_PARAM_T ebi_para;  
GDMA_PARAM_T gdma_para;
void gdma_Interrupt_Handler0(int irq, void *dev_id, struct pt_regs *regs);
void gdma_Interrupt_Handler1(int irq, void *dev_id, struct pt_regs *regs);

inline int gdma_Init(EBI_PARAM_T *pe, GDMA_PARAM_T *pg, GDMA_FUNC_T *pgfun) 
{
	unsigned volatile int uExt3Con, uReg;
	
	/* Configure FA5933 EBI */    
	if(pe != 0)
	{
		uExt3Con = pe->uIOBaseAddr | pe->uSize | pe->uACC | pe->uCOH | pe->uACS | pe->uCOS | pe->uDBWD;
		gdma_outpw(FA5933_EBI_EXT3CON, uExt3Con);
		printk("EBI: EXT3CON[0x%08x]\n", gdma_inpw(FA5933_EBI_EXT3CON));
	}		
	
	/* Configure FA5933 GDMA */
	// step 1, check GDMA channel
	if( (pg->ucChannel != 0) && (pg->ucChannel != 1) )
	{
		WB_PRINTK_GDMA("GDMA: channel error\n");
		return -1;
	}
	
	// Configure GPIO as nXDREQ & nXDACK    12.25 add, NS22 HHWu
	uReg = gdma_inpw(FA5933_GPIO_CFG1);
	uReg &= ~(0x0F);	
	uReg |= 0x05;
	gdma_outpw(FA5933_GPIO_CFG1, uReg);
	
	// initial GDMA struct, important
	gdma_para.ucStatus[pg->ucChannel] = 0;  
	gdma_para.fnGDMACallBack[pg->ucChannel] = 0;
		
	// step 2, configure transfer mode	
	// Note: SINGLE_MODE & BLOCK_MODE are alternative. 	
	// Note: nXDREQ_MODE & SOFTWARE_MODE are alternative. 
	if( (pg->ucMode & GDMA_SINGLE_MODE) && (pg->ucMode & GDMA_BLOCK_MODE) )	
	{
		WB_PRINTK_GDMA("GDMA: single & block mode are alternative\n");
		return -2;
	}	
	if( (pg->ucMode & GDMA_nXDREQ_MODE) && (pg->ucMode & GDMA_SOFTWARE_MODE) )
	{	
		WB_PRINTK_GDMA("GDMA: XDREQ & software mode are alternative\n");	
		return -3;	
	}		
	gdma_Config_Transfer_Mode(pg->ucChannel, pg->ucMode);	
	
	// step 3, configure transfer width
	if( (pg->ucWidth != GDMA_Bit8) && (pg->ucWidth != GDMA_Bit16) && (pg->ucWidth != GDMA_Bit32) )
	{
		WB_PRINTK_GDMA("GDMA: transfer width error\n");
		return -4;
	}		
	gdma_Config_Transfer_Width(pg->ucChannel, pg->ucWidth);	
	
	// step 4, configure nXDREQ & nXDACK active
	if( (pg->ucReq != GDMA_REQ_ATV_HIGH) && (pg->ucReq != GDMA_REQ_ATV_LOW) )
	{
		WB_PRINTK_GDMA("GDMA: nXDREQ active error\n");
		return -5;
	}
	if( (pg->ucAck != GDMA_ACK_ATV_HIGH) && (pg->ucAck != GDMA_ACK_ATV_LOW) )
	{
		WB_PRINTK_GDMA("GDMA: nXDACK active error\n");
		return -6;
	}	
	gdma_Config_REQ_ACK_Active(pg->ucChannel, pg->ucReq, pg->ucAck);

	// step 5, configure SIEN
	gdma_Conifg_SIEN(pg->ucChannel);
	
	// step 6, configure SADIR DADIR 
	if( (pg->ucSrcDir != GDMA_SADIR_INCREASE) && (pg->ucSrcDir != GDMA_SADIR_DECREASE) )
	{
		WB_PRINTK_GDMA("GDMA: source address direction error\n");
		return -7;
	}
	if( (pg->ucDestDir != GDMA_DADIR_INCREASE) && (pg->ucDestDir != GDMA_DADIR_DECREASE) )
	{
		WB_PRINTK_GDMA("GDMA: destination address direction error\n");
		return -8;
	}
	gdma_Config_ADDR_DIR(pg->ucChannel, pg->ucSrcDir, pg->ucDestDir);
	
	// step 7, install GDMA ISR and call back function ..... call back function param issue not yet
	if(pg->ucChannel == 0)
	{
		if(pgfun != 0)
			gdma_para.fnGDMACallBack[0] = pgfun;	
		
		if(request_irq(19, gdma_Interrupt_Handler0,  SA_INTERRUPT, "wbgdma0", 0)) 
		{
			WB_PRINTK_GDMA("GDMA: install GDMA ISR 0 fail\n");
			return -9;
		}	
		else
		{	
			WB_PRINTK_GDMA("GDMA: install GDMA ISR 0 success\n");
			enable_irq(19);
		}

// HHWu adde    12.22 add.		
		init_waitqueue_head(&G0Wait);	
// HHWu adde			
	}
	else
	{
		if(pgfun != 0)
			gdma_para.fnGDMACallBack[1] = pgfun;	
		
		if(request_irq(20, gdma_Interrupt_Handler1,  SA_INTERRUPT, "wbgdma1", 0)) 
		{
			WB_PRINTK_GDMA("GDMA: install GDMA ISR 1 fail\n");
			return -10;
		}	
		else
		{	
			WB_PRINTK_GDMA("GDMA: install GDMA ISR 1 success\n");
			enable_irq(20);
		}
		
// HHWu adde    12.22 add.		
		init_waitqueue_head(&G1Wait);	
// HHWu adde		
	}
	
	if(pg->ucChannel == 0)
		WB_PRINTK_GDMA("GDMA: gdma_Init ch[%d] CTL[0x%08x]\n", pg->ucChannel, gdma_inpw(FA5933_GDMA_CTL0));
	else
		WB_PRINTK_GDMA("GDMA: gdma_Init ch[%d] CTL[0x%08x]\n", pg->ucChannel, gdma_inpw(FA5933_GDMA_CTL1));	

	gdma_para.ucStatus[pg->ucChannel] |= GDMA_INIT_OK;
	return 0;  // success	
}	

inline void gdma_Release(GDMA_PARAM_T *pg)
{
	if(pg->ucChannel == 0)
		free_irq(19, 0);  // 2nd parameter ..... not yet
	if(pg->ucChannel == 1)
		free_irq(20, 0);  // 2nd parameter ..... not yet
}	

inline int gdma_IO_To_Memory(GDMA_DATA_T *pd)  // set GDMA_SAFIX
{
	// should check initial specific GDMA channel before 
	if( !(gdma_para.ucStatus[pd->ucChannel] & GDMA_INIT_OK) )
	{
		WB_PRINTK_GDMA("GDMA: gdma_IO_To_Memory GDMA[%d] not initial\n", pd->ucChannel);
		return -1;	
	}
	
	gdma_Count_TCNT(pd->ucChannel, pd->uDataSize);
	gdma_Set_Source_Address(pd->ucChannel, pd->uSrcAddr);
	gdma_Set_Destination_Address(pd->ucChannel, pd->uDestAddr);
	
	switch(pd->ucChannel)
	{
		case 0:
			// should check SABNDERR & DABNDERR bit, TWS have to set in gdma_Init() before
			if(gdma_inpw(FA5933_GDMA_CTL0) & 0x600000)
			{
				WB_PRINTK_GDMA("GDMA: gdma_IO_To_Memory GDMA[%d] address boundary error[0x%08x]\n", 
					pd->ucChannel, gdma_inpw(FA5933_GDMA_CTL0));  
				return -2;	
			}	
			
// HHWu adde    12.22 modify.			
			nG0TXFinish = 0;
			gdma_outpw( FA5933_GDMA_CTL0, (gdma_inpw(FA5933_GDMA_CTL0) | GDMA_SAFIX | GDMA_GDMAEN) );
			wait_event_interruptible(G0Wait, nG0TXFinish != 0);
// HHWu adde			
			break;
			
		case 1:
			// should check SABNDERR & DABNDERR bit, TWS have to set in gdma_Init() before
			if(gdma_inpw(FA5933_GDMA_CTL1) & 0x600000)
			{
				WB_PRINTK_GDMA("GDMA: gdma_IO_To_Memory GDMA[%d] address boundary error[0x%08x]\n", 
					pd->ucChannel, gdma_inpw(FA5933_GDMA_CTL1));  
				return -3;	
			}
			
// HHWu adde    12.22 modify.			
			nG1TXFinish = 0;
			gdma_outpw( FA5933_GDMA_CTL1, (gdma_inpw(FA5933_GDMA_CTL1) | GDMA_SAFIX | GDMA_GDMAEN) );
			wait_event_interruptible(G1Wait, nG1TXFinish != 0);
// HHWu adde			
			break;
			
		default:
			WB_PRINTK_GDMA("GDMA: gdma_IO_To_Memory channel error\n");
			return -4;	
	}	
	return 0;  // success	
}

inline int gdma_Memory_To_IO(GDMA_DATA_T *pd)  // set GDMA_DAFIX
{
	// should check initial specific GDMA channel before 
	if( !(gdma_para.ucStatus[pd->ucChannel] & GDMA_INIT_OK) )
	{
		WB_PRINTK_GDMA("GDMA: gdma_Memory_To_IO GDMA[%d] not initial\n", pd->ucChannel);
		return -1;	
	}
	
	gdma_Count_TCNT(pd->ucChannel, pd->uDataSize);
	gdma_Set_Source_Address(pd->ucChannel, pd->uSrcAddr);
	gdma_Set_Destination_Address(pd->ucChannel, pd->uDestAddr);
	
	switch(pd->ucChannel)
	{
		case 0:
			// should check SABNDERR & DABNDERR bit, TWS have to set in gdma_Init() before
			if(gdma_inpw(FA5933_GDMA_CTL0) & 0x600000)
			{
				WB_PRINTK_GDMA("GDMA: gdma_Memory_To_IO GDMA[%d] address boundary error[0x%08x]\n", 
					pd->ucChannel, gdma_inpw(FA5933_GDMA_CTL0));  
				return -2;	
			}
			
// HHWu adde    12.22 modify.			
			nG0TXFinish = 0;
			gdma_outpw( FA5933_GDMA_CTL0, (gdma_inpw(FA5933_GDMA_CTL0) | GDMA_DAFIX | GDMA_GDMAEN) );
			wait_event_interruptible(G0Wait, nG0TXFinish != 0);
// HHWu adde			
			break;
			
		case 1:
			// should check SABNDERR & DABNDERR bit, TWS have to set in gdma_Init() before
			if(gdma_inpw(FA5933_GDMA_CTL1) & 0x600000)
			{
				WB_PRINTK_GDMA("GDMA: gdma_Memory_To_IO GDMA[%d] address boundary error[0x%08x]\n", 
					pd->ucChannel, gdma_inpw(FA5933_GDMA_CTL1));  
				return -3;	
			}
			
// HHWu adde    12.22 modify.			
			nG1TXFinish = 0;
			gdma_outpw( FA5933_GDMA_CTL1, (gdma_inpw(FA5933_GDMA_CTL1) | GDMA_DAFIX | GDMA_GDMAEN) );
			wait_event_interruptible(G1Wait, nG1TXFinish != 0);
// HHWu adde			
			break;
			
		default:
			WB_PRINTK_GDMA("GDMA: gdma_Memory_To_IO channel error\n");
			return -4;	
	}	
	return 0;  // success	
}

inline int gdma_Memory_To_Memory(GDMA_DATA_T *pd)  // don't set GDMA_DAFIX & GDMA_SAFIX, set GDMA_SOFTREQ
{
	// should check initial specific GDMA channel before 
	if( !(gdma_para.ucStatus[pd->ucChannel] & GDMA_INIT_OK) )
	{
		WB_PRINTK_GDMA("GDMA: gdma_Memory_To_Memory GDMA[%d] not initial\n", pd->ucChannel);
		return -1;	
	}
	
	gdma_Count_TCNT(pd->ucChannel, pd->uDataSize);
	gdma_Set_Source_Address(pd->ucChannel, pd->uSrcAddr);
	gdma_Set_Destination_Address(pd->ucChannel, pd->uDestAddr);
	
	switch(pd->ucChannel)
	{
		case 0:
			// should check SABNDERR & DABNDERR bit, TWS have to set in gdma_Init() before
			if(gdma_inpw(FA5933_GDMA_CTL0) & 0x600000)
			{
				WB_PRINTK_GDMA("GDMA: gdma_Memory_To_Memory GDMA[%d] address boundary error[0x%08x]\n", 
					pd->ucChannel, gdma_inpw(FA5933_GDMA_CTL0)); 
				return -2;	
			}
			
// HHWu adde    12.22 modify.			
			nG0TXFinish = 0;
			gdma_outpw(FA5933_GDMA_CTL0, gdma_inpw(FA5933_GDMA_CTL0) | GDMA_SOFTREQ | GDMA_GDMAEN);
			wait_event_interruptible(G0Wait, nG0TXFinish != 0);
// HHWu adde			
			break;
			
		case 1:
			// should check SABNDERR & DABNDERR bit, TWS have to set in gdma_Init() before
			if(gdma_inpw(FA5933_GDMA_CTL1) & 0x600000)
			{
				WB_PRINTK_GDMA("GDMA: gdma_Memory_To_Memory GDMA[%d] address boundary error[0x%08x]\n", 
					pd->ucChannel, gdma_inpw(FA5933_GDMA_CTL1)); 
				return -3;	
			}
			
// HHWu adde    12.22 modify.			
			nG1TXFinish = 0;
			gdma_outpw( FA5933_GDMA_CTL1, gdma_inpw(FA5933_GDMA_CTL1) | GDMA_SOFTREQ | GDMA_GDMAEN);
			wait_event_interruptible(G1Wait, nG1TXFinish != 0);
// HHWu adde				
			break;
			
		default:
			WB_PRINTK_GDMA("GDMA: gdma_Memory_To_Memory channel error\n");
			return -4;	
	}	
	return 0;  // success	
}

void gdma_Interrupt_Handler0(int irq, void *dev_id, struct pt_regs *regs)
{
	WB_PRINTK_GDMA("GDMA: INT0\n");  
	
	// interrupt occured continuously, TC be set & AIC IPER is 0 ..... strange	
	gdma_outpw(FA5933_AIC_SCCR, 1 << 19);
	gdma_outpw(FA5933_GDMA_CTL0, gdma_inpw(FA5933_GDMA_CTL0) & (~GDMA_TC));  // clear by write 0
	
	// maintain GDMA status ..... GDMATERR not yet 
	gdma_para.ucStatus[0] |= GDMA_INT_OCCUR;
		
	// execute back function 
	if(gdma_para.fnGDMACallBack[0] != 0)
		gdma_para.fnGDMACallBack[0](); 
		
// HHWu addb    12.22 add
	nG0TXFinish = 1;
	wake_up_interruptible(&G0Wait);
	return;
// HHWu adde		 
}
 
void gdma_Interrupt_Handler1(int irq, void *dev_id, struct pt_regs *regs)
{
	WB_PRINTK_GDMA("GDMA: INT1\n");  
	
	// interrupt occured continuously, TC be set & AIC IPER is 0 ..... strange	
	gdma_outpw(FA5933_AIC_SCCR, 1 << 20);
	gdma_outpw(FA5933_GDMA_CTL1, gdma_inpw(FA5933_GDMA_CTL1) & (~GDMA_TC));  // clear by write 0
	
	// maintain GDMA status ..... GDMATERR not yet 
	gdma_para.ucStatus[1] |= GDMA_INT_OCCUR;
	
	// execute back function 
	if(gdma_para.fnGDMACallBack[1] != 0)
		gdma_para.fnGDMACallBack[1]();
		
// HHWu addb    12.22 add
	nG1TXFinish = 1;
	wake_up_interruptible(&G1Wait);
// HHWu adde			  
} 
 
 
#endif 
 