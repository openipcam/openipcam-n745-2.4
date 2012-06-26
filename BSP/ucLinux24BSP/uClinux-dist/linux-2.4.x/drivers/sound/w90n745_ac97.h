/**************************************************************************************************
 *                                                                          
 * Copyright (c) 2004 - 2007 Winbond Electronics Corp. All rights reserved.      
 *                                                                         
 * FILENAME
 *     w90n745_AC97.h
 *
 * VERSION
 *     1.0
 *
 * DESCRIPTION
 *     This file contains the register map of AC97 audio codec
 *
 * HISTORY
 *     02/09/2004		 Ver 1.0 Created by PC30 YCHuang
 *
 * REMARK
 *     None
 *     
 *************************************************************************************************/
#ifndef _W90N745_AC97_H_
#define _W90N745_AC97_H_

#define AC97_RESET				0x00
#define AC97_MASTER_VOLUME		0x02
#define AC97_AUX_OUT_VOLUME		0x04
#define AC97_MONO_VOLUME		0x06
#define AC97_MASTER_TONE		0x08
#define AC97_PC_BEEP_VOLUME		0x0A
#define AC97_PHONE_VOLUME		0x0C
#define AC97_MIC_VOLUME			0x0E
#define AC97_LINE_IN_VOLUME		0x10
#define AC97_CD_VOLUME			0x12
#define AC97_VIDEO_VOLUME		0x14
#define AC97_AUX_IN_VOLUME		0x16
#define AC97_PCM_OUT_VOLUME		0x18
#define AC97_RECORD_SELECT		0x1A
#define AC97_RECORD_GAIN		0x1C
#define AC97_RECORD_GAIN_MIC	0x1E
#define AC97_GENERAL_PURPOSE	0x20
#define AC97_3D_CONTROL			0x22
#define AC97_AUDIO_INT_PAGING	0x24
#define AC97_POWERDOWN_CTRL		0x26
#define AC97_EXT_AUDIO_ID		0x28
#define AC97_EXT_AUDIO_CTRL		0x2A
#define AC97_FRONT_DAC_RATE		0x2C
#define AC97_LR_ADC_RATE		0x32
#define AC97_MIC_ADC_RATE		0x34

/* bit definition of ATCL_CON register */
#define AUDCLK_EN			0x8000
#define PFIFO_EN			0x4000
#define RFIFO_EN			0x2000

#define AC97_ACTIVE			0x1
#define AC97_PLAY_ACTIVE	0x2
#define AC97_REC_ACTIVE	0x4


#endif	/* _W90N745_AC97_H_ */
