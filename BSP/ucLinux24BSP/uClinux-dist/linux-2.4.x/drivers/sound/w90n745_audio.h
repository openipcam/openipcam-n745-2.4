/**************************************************************************************************
 *                                                                          
 * Copyright (c) 2004 - 2007 Winbond Electronics Corp. All rights reserved.      
 *                                                                         
 * FILENAME
 *     W90N745_AUDIO.H
 *
 * VERSION
 *     1.1
 *
 * DESCRIPTION
 *     This file contains the main structure of w90n745 audio module
 *
 * HISTORY
 *     02/09/2004		 Ver 1.0 Created by PC30 YCHuang
 *	 11/25/2005		 Ver 1.1 Modified by PC34 QFu
 *
 * REMARK
 *     None
 *     
 *************************************************************************************************/

#ifndef _W90N745_AUDIO_H_
#define _W90N745_AUDIO_H_

#include <asm/arch/cdefs.h>
#include <asm/semaphore.h>

typedef int (*AU_CB_FUN_T)(UINT32, UINT32);

typedef enum au_dev_e
{
	AU_DEV_AC97,
	AU_DEV_IIS
} AU_DEV_E;

typedef struct wb_audio_codec_t{
	AU_DEV_E dev;			/* codec type */
	INT		(*get_capacity)(VOID);
	VOID	(*set_play_buffer)(UINT32, UINT32);
	VOID	(*set_record_buffer)(UINT32, UINT32);

	INT		(*set_play_volume)(UINT32 nLeft, UINT32 nRight);
	INT		(*set_record_volume)(UINT32 nLeft, UINT32 nRight);

	INT		(*reset)(VOID);
	
	INT		(*start_play)(AU_CB_FUN_T fnCallBack, INT nSamplingRate, INT nChannels);
	VOID	(*stop_play)(VOID);

	INT		(*start_record)(AU_CB_FUN_T fnCallBack, INT nSamplingRate, INT nChannels);
	VOID	(*stop_record)(VOID);

	VOID	(*play_interrupt)(VOID);		/* nonzero play stopped */
	VOID	(*record_interrupt)(VOID);
}WB_AUDIO_CODEC_T;
	
typedef struct audio_t
{
	AU_CB_FUN_T 	fnPlayCallBack;
	AU_CB_FUN_T 	fnRecCallBack;
	INT				nPlaySamplingRate;
	INT				nRecSamplingRate;
	short			sPlayVolume;
	short			sRecVolume;
	UINT32			uPlayBufferAddr;
	UINT32			uPlayBufferLength;
	UINT32			uRecordBufferAddr;
	UINT32			uRecordBufferLength;
}AUDIO_T;

typedef struct wb_audio_t{
	int state;
	int open_flags;
	int dsp_dev, dsp_openflag;
	int mixer_dev, mixer_openflag;
	unsigned int play_buf_addr, record_buf_addr;
	unsigned int play_buf_length, record_buf_length;
	int nSamplingRate;
	int nChannels;
	int nPlayVolumeLeft, nPlayVolumeRight, nRecordVolumeLeft, nRecordVolumeRight;

	struct{
		int ptr;
	}play_half_buf[2], record_half_buf[2];
	
	WB_AUDIO_CODEC_T *codec;
	struct semaphore dsp_read_sem, dsp_write_sem, mixer_sem;
	struct fasync_struct *fasync_ptr;
	wait_queue_head_t write_wait_queue, read_wait_queue;
}WB_AUDIO_T;



#define AU_SAMPLE_RATE_48000	48000
#define AU_SAMPLE_RATE_44100	44100
#define AU_SAMPLE_RATE_32000	32000
#define AU_SAMPLE_RATE_24000	24000
#define AU_SAMPLE_RATE_22050	22050
#define AU_SAMPLE_RATE_16000	16000
#define AU_SAMPLE_RATE_11025	11025
#define AU_SAMPLE_RATE_8000	8000

#define AU_CH_MONO		1
#define AU_CH_STEREO	2

/* state code */
#define AU_STATE_NOP			0
#define AU_STATE_PLAYING		1
#define AU_STATE_RECORDING	2

/* capacity */
#define AU_CAP_DUPLEX			1

/* Error Code */
#define ERR_AU_GENERAL_ERROR	-1
#define ERR_AU_NO_MEMORY		-5		/* memory allocate failure */
#define ERR_AU_ILL_BUFF_SIZE	-10		/* illegal callback buffer size */
#define ERR_AC97_CODEC_RESET	-20		/* AC97 codec reset failed */
#define ERR_AC97_PLAY_ACTIVE	-22		/* AC97 playback has been activated */
#define ERR_AC97_REC_ACTIVE	-23		/* AC97 record has been activated */
#define ERR_AC97_NO_DEVICE		-24		/* have no AC97 codec on board */
#define ERR_MA3_PLAY_ACTIVE	-50		/* MA3 playback has been activated */
#define ERR_MA3_NO_DEVICE		-51		/* have no MA3 chip on board */
#define ERR_MA5_PLAY_ACTIVE	-80		/* MA5 playback has been activated */
#define ERR_MA5_NO_DEVICE		-81		/* have no MA5 chip on board */
#define ERR_MA5I_NO_DEVICE		-90		/* have no MA5i chip on board */
#define ERR_DAC_PLAY_ACTIVE	-110	/* DAC playback has been activated */
#define ERR_DAC_NO_DEVICE		-111	/* DAC is not available */
#define ERR_ADC_REC_ACTIVE		-120	/* ADC record has been activated */
#define ERR_ADC_NO_DEVICE		-121	/* ADC is not available */
#define ERR_IIS_PLAY_ACTIVE		-140	/* IIS playback has been activated */
#define ERR_IIS_REC_ACTIVE		-141	/* IIS record has been activated */
#define ERR_IIS_NO_DEVICE		-142	/* has no IIS codec on board */
#define ERR_WM8753_NO_DEVICE	-150	/* has no wm8753 codec on board */
#define ERR_W5691_PLAY_ACTIVE	-160	/* W5691 playback has been activated */
#define ERR_W5691_NO_DEVICE	-161	/* Have no W5691 chip on board */

#define ERR_NO_DEVICE			-201	/* audio device not available */

extern WB_AUDIO_CODEC_T wb_ac97_codec;
extern WB_AUDIO_CODEC_T wb_i2s_codec;

#endif	/* _W90N745_AUDIO_H_ */

