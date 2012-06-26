/**************************************************************************************************
 *                                                                          
 * Copyright (c) 2004 - 2007 Winbond Electronics Corp. All rights reserved.      
 *                                                                         
 * FILENAME
 *     w90n745_Audio_REGS.H
 *
 * VERSION
 *     1.0
 *
 * DESCRIPTION
 *     This file contains the register map of W90N710 audio controller.
 *
 * HISTORY
 *     05/26/2004		 Ver 1.0 Created by PC30 YCHuang
 *	 11/24/2005		 Ver 1.1 Modified by PC34 QFu	
 *
 * REMARK
 *     None
 *     
 *************************************************************************************************/
#ifndef _W90N745_AUDIO_REGS_H_
#define _W90N745_AUDIO_REGS_H_

#define REG_CLKSEL			0xfff0000c

#define GPIO_BA				0xFFF83000
/**********************************************************************************************************
 *                                                               
 * 4. GPIO Control Registers  
 *
 **********************************************************************************************************/
/* GPIO Pins */ 
#define REG_GPIO_CFG			(GPIO_BA+0x00)   /* GPIO Pins Output Enable Control Register */
#define REG_GPIO_DIR		(GPIO_BA+0x04)   /* GPIO Pins Data Register */
#define REG_GPIO_DATAOUT		(GPIO_BA+0x08)   /* GPIO Pins Status Register */


/* GPIO-A Pins */
#define REG_GPIOA_OE		(GPIO_BA+0x20)   /* GPIO-A Pins Output Enable Control Register */
#define REG_GPIOA_DAT		(GPIO_BA+0x24)   /* GPIO-A Pins Data Register */
#define REG_GPIOA_STS		(GPIO_BA+0x28)   /* GPIO-A Pins Status Register */
#define REG_GPIOA_PE		(GPIO_BA+0x2c)   /* GPIO-A Pull-Up/Down Enable Control Register */

/* GPIO-B Pins */
#define REG_GPIOB_OE		(GPIO_BA+0x30)   /* GPIO-B Pins Output Enable Control Register */
#define REG_GPIOB_DAT		(GPIO_BA+0x34)   /* GPIO-B Pins Data Register */
#define REG_GPIOB_STS		(GPIO_BA+0x38)   /* GPIO-B Pins Status Register */
#define REG_GPIOB_PE		(GPIO_BA+0x3c)   /* GPIO-B Pull-Up/Down Enable Control Register */

/* GPIO-S Pins */
#define REG_GPIOS_OE		(GPIO_BA+0x40)   /* GPIO-S Pins Output Enable Control Register */
#define REG_GPIOS_DAT		(GPIO_BA+0x44)   /* GPIO-S Pins Data Register */
#define REG_GPIOS_STS		(GPIO_BA+0x48)   /* GPIO-S Pins Status Register */
#define REG_GPIOS_PE		(GPIO_BA+0x4c)   /* GPIO-S Pull-Up/Down Enable Control Register */


/* to be modified in future */
//#include "w90n745_reg.h"	/* in SYSLIB */
#define ADO_BA					0xfff09000
#define ADO_RANGE				0x50

/**********************************************************************************************************
 *                                                               
 * 8. Audio Interface Control Registers  
 *
 **********************************************************************************************************/
#define REG_ACTL_CON			(ADO_BA + 0x00)   /* Audio controller control register */
#define REG_ACTL_RESET			(ADO_BA + 0x04)   /* Sub block reset control register */
#define REG_ACTL_RDSTB			(ADO_BA + 0x08)   /* DMA destination base address register for record */
#define REG_ACTL_RDST_LENGTH	(ADO_BA + 0x0C)   /* DMA destination length register for record */
#define REG_ACTL_RSR			(ADO_BA + 0x14)   /* Record status register */
#define REG_ACTL_PDSTB			(ADO_BA + 0x18)   /* DMA destination base address register for play */
#define REG_ACTL_PDST_LENGTH	(ADO_BA + 0x1C)   /* DMA destination length register for play */
#define REG_ACTL_PDSTC			(ADO_BA + 0x20)
#define REG_ACTL_PSR			(ADO_BA + 0x24)   /* Play status register */
#define REG_ACTL_IISCON			(ADO_BA + 0x28)   /* IIS control register */
#define REG_ACTL_ACCON			(ADO_BA + 0x2C)   /* AC-link control register */
#define REG_ACTL_ACOS0			(ADO_BA + 0x30)   /* AC-link out slot 0 */
#define REG_ACTL_ACOS1			(ADO_BA + 0x34)   /* AC-link out slot 1 */
#define REG_ACTL_ACOS2			(ADO_BA + 0x38)   /* AC-link out slot 2 */
#define REG_ACTL_ACIS0			(ADO_BA + 0x3C)   /* AC-link in slot 0 */
#define REG_ACTL_ACIS1			(ADO_BA + 0x40)   /* AC-link in slot 1 */
#define REG_ACTL_ACIS2			(ADO_BA + 0x44)   /* AC-link in slot 2 */

  

#define AU_PLAY_INT_NUM	6
#define AU_REC_INT_NUM		6
#define GPIO_INT_NUM		4

#define USED_GPIO_NUM		5

#define MISCR				0xC
#define PADMFC				0x20
#define DCCS				0x5000
#define DEVICE_CTRL			0x5004


/* bit definition of REG_ACTL_CON register */
#define AUDCLK_EN			0x8000
#define PFIFO_EN			0x4000
#define RFIFO_EN			0x2000
#define R_DMA_IRQ			0x1000
#define T_DMA_IRQ			0x0800
#define IIS_AC_PIN_SEL		0x0100
#define FIFO_TH				0x0080
#define DMA_EN				0x0040
#define DAC_EN				0x0020
#define ADC_EN				0x0010
#define M80_EN				0x0008
#define ACLINK_EN			0x0004
#define IIS_EN				0x0002
#define AUDIO_EN			0x0001

/* bit definition of REG_ACTL_RESET register */
#define W5691_PLAY			0x20000
#define ACTL_RESET_BIT		0x10000			
#define RECORD_RIGHT_CHNNEL 0x8000
#define RECORD_LEFT_CHNNEL 	0x4000
#define PLAY_RIGHT_CHNNEL 	0x2000
#define PLAY_LEFT_CHNNEL 	0x1000
#define DAC_PLAY		 	0x0800
#define ADC_RECORD			0x0400
#define M80_PLAY			0x0200
#define AC_RECORD			0x0100
#define AC_PLAY				0x0080
#define IIS_RECORD			0x0040
#define IIS_PLAY			0x0020
#define DAC_RESET			0x0010
#define ADC_RESET			0x0008
#define M80_RESET			0x0004
#define AC_RESET			0x0002
#define IIS_RESET			0x0001

/* bit definition of REG_ACTL_ACCON register */
#define AC_BCLK_PU_EN		0x20
#define AC_R_FINISH			0x10
#define AC_W_FINISH			0x08
#define AC_W_RES			0x04
#define AC_C_RES			0x02

/* bit definition of REG_ACTL_RSR register */
#define R_FIFO_EMPTY		0x04
#define R_DMA_END_IRQ		0x02
#define R_DMA_MIDDLE_IRQ	0x01

/* bit definition of REG_ACTL_PSR register */
#define P_FIFO_EMPTY		0x04
#define P_DMA_END_IRQ		0x02
#define P_DMA_MIDDLE_IRQ	0x01

/* bit definition of REG_ACTL_M80CON register */
#define X86_PCM_TRANS		0x10000
#define BITS16				0x400
#define MA3_W5691			0x200
#define W_IF13_ACT			0x100
#define BUSY				0x80
#define R_IF11_ACT			0x40
#define R_IF10_ACT			0x20
#define W_IF12_ACT			0x10
#define W_IF11_ACT			0x08
#define W_IF10_ACT			0x04
#define SOFT_CON			0x02
#define W_GFIFO				0x20000

#define CLK_DIV				0xF800

#define SOFT_CON_A0			0x0
#define SOFT_CON_A1			0x100
#define SOFT_CON_R			0x200
#define SOFT_CON_W			0x400
#define SOFT_CON_CS			0x800
#define SOFT_CON_REQ		0x1000
#define SOFT_CON_REL		0x2000

/* bit definition of REG_ACTL_ADCON register */
#define ADC_ZCD_EN			0x8
#define ADC_MUTE			0x2

/* bit definition of REG_ACTL_DACON register */
#define DAC1_OEB			0x2000
#define DAC0_OEB			0x1000
#define DAC_ZCD_EN			0x8
#define DAC_MUTE			0x2

#define AD_DA_48000			(0x0 << 4)
#define AD_DA_44100			(0x1 << 4)
#define AD_DA_32000			(0x2 << 4)
#define AD_DA_24000			(0x3 << 4)
#define AD_DA_22050			(0x4 << 4)
#define AD_DA_16000			(0x5 << 4)
#define AD_DA_12000			(0x6 << 4)
#define AD_DA_11025			(0x7 << 4)
#define AD_DA_8000			(0x8 << 4)

#endif	/* _W90N745_AUDIO_REGS_H_ */


