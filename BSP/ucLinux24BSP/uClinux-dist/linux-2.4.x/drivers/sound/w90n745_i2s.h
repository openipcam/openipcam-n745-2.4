/**************************************************************************************************
 *                                                                          
 * Copyright (c) 2004 - 2007 Winbond Electronics Corp. All rights reserved.      
 *                                                                         
 * FILENAME
 *     w90n745_i2s.h
 *
 * VERSION
 *     1.0
 *
 * DESCRIPTION
 *     This file contains the register map of IIS audio interface
 *
 * HISTORY
 *     02/09/2004		 Ver 1.0 Created by PC31 SJLu
 *
 * REMARK
 *     None
 *     
 *************************************************************************************************/
#ifndef _W90N745_I2S_H_
#define _W90N745_I2S_H_

/*----- bit definition of REG_ACTL_IISCON register -----*/
#define IIS					0x0
#define MSB_Justified		0x0008
#define SCALE_1				0x0
#define SCALE_2				0x10000
#define SCALE_3				0x20000
#define SCALE_4				0x30000
#define SCALE_5				0x40000
#define SCALE_6				0x50000
#define SCALE_7				0x60000
#define SCALE_8				0x70000
#define SCALE_10			0x90000
#define SCALE_12			0xB0000
#define SCALE_14			0xD0000
#define SCALE_16			0xF0000
#define FS_384				0x20
#define FS_256				0x0
#define BCLK_32				0x00
#define BCLK_48				0x40

/* bit definition of L3DATA register */
#define EX_256FS 		0x20		/*-- system clock --*/
#define EX_384FS 		0x10		
#define EX_IIS			0x00		/*-- data input format  --*/
#define EX_MSB			0x08
#define EX_1345ADDR 	0x14		//The address of the UDA1345TS
#define EX_STATUS		0x02		//data transfer type (STATUS)
#define EX_DATA			0x00		//data transfer type (DATA)
#define EX_ADC_On		0xC2		//turn on the ADC
#define EX_DAC_On		0xC1		//turn on the DAC

/*----- GPIO NUM -----*/
#define L3MODE_GPIO_NUM		(1<<17)
#define L3CLOCK_GPIO_NUM 	(1<<18)
#define L3DATA_GPIO_NUM 	(1<<19)

#define MSB_FORMAT	1
#define IIS_FORMAT  2

#define	IIS_ACTIVE				0x1
#define	IIS_PLAY_ACTIVE		0x2
#define 	IIS_REC_ACTIVE			0x4

#endif	/* _W90N745_I2S_H_ */


