/****************************************************************************
 * 
 * Copyright (c) 2004 - 2007 Winbond Electronics Corp. All rights reserved.     
 *
 * 
 * FILENAME
 *     w90n745_AC97.c
 *
 * VERSION
 *     1.2 
 *
 * DESCRIPTION
 *	AC97 interface for W83972D codec
 *
 * DATA STRUCTURES
 *    None
 *
 * FUNCTIONS
 *	...
 *     
 * HISTORY
 *	2004.06.01		Created by Yung-Chang Huang
 *	2004.09.02       	Ver 1.0 Modify for w90n745 coding standard
 *	2005.11.24		Ver 1.1 Modified for uCLinux Sound Driver by PC34 QFu
 *	2006.01.17		Ver 1.2 Modified for w90n745 Version B
 *
 * REMARK
 *     None
 *
 **************************************************************************/
#include <linux/soundcard.h>
#include <linux/delay.h>
#include <asm/io.h>
#include <asm/irq.h>

#include "w90n745_audio_regs.h"
#include "w90n745_audio.h"
#include "w90n745_ac97.h"

//#define AC97_DEBUG
//#define AC97_DEBUG_ENTER_LEAVE
//#define AC97_DEBUG_MSG
//#define AC97_DEBUG_MSG2

#ifdef AC97_DEBUG
#define DBG(fmt, arg...)			printk(fmt, ##arg)
#else
#define DBG(fmt, arg...)
#endif

#ifdef AC97_DEBUG_ENTER_LEAVE
#define ENTER()					DBG("[%-10s] : Enter\n", __FUNCTION__)
#define LEAVE()					DBG("[%-10s] : Leave\n", __FUNCTION__)
#else
#define ENTER()
#define LEAVE()
#endif

#ifdef AC97_DEBUG_MSG
#define MSG(fmt)					DBG("[%-10s] : "fmt, __FUNCTION__)
#else
#define MSG(fmt)
#endif

#ifdef AC97_DEBUG_MSG2
#define MSG2(fmt, arg...)			DBG("[%-10s] : "fmt, __FUNCTION__, ##arg)
#else
#define MSG2(fmt, arg...)
#endif

#define AUDIO_WRITE(addr, val)		writel(val, addr)
#define AUDIO_READ(addr)			readl(addr)

static AUDIO_T	_tAC97;

static int	_bAC97Active = 0;
static volatile int _bPlayDmaToggle, _bRecDmaToggle;

static void ac97StopPlay(void);

struct semaphore ac97_sem;

static unsigned int ac97_read_register(INT nIdx)
{
	volatile INT	nWait;
	unsigned int i;

	down(&ac97_sem);

	/* set the R_WB bit and write register index */
	AUDIO_WRITE(REG_ACTL_ACOS1, 0x80 | nIdx);
	
	/* set the valid frame bit and valid slots */
	AUDIO_WRITE(REG_ACTL_ACOS0, AUDIO_READ(REG_ACTL_ACOS0) |0x11);

	udelay(100);
	
	/* polling the AC_R_FINISH */
	for (nWait = 0; nWait < 0x10000; nWait++)
	{
		if (AUDIO_READ(REG_ACTL_ACCON) & AC_R_FINISH)
			break;
	}
	if (nWait == 0x10000)
		MSG("ac97_read_register time out!\n");

	AUDIO_WRITE(REG_ACTL_ACOS0, AUDIO_READ(REG_ACTL_ACOS0) & ~1);
		
	if (AUDIO_READ(REG_ACTL_ACIS1) >> 2 != nIdx)
	{	MSG2("ac97_read_register - R_INDEX of REG_ACTL_ACIS1 not match!, 0x%x\n", AUDIO_READ(REG_ACTL_ACIS1));	}

	udelay(100);		
	i = AUDIO_READ(REG_ACTL_ACIS2) & 0xFFFF;

	up(&ac97_sem);
	return (i);
}


static int ac97_write_register(INT nIdx, UINT16 sValue)
{
	volatile INT	nWait;

	down(&ac97_sem);

	/* clear the R_WB bit and write register index */
	AUDIO_WRITE(REG_ACTL_ACOS1, nIdx);
	
	/* write register value */
	AUDIO_WRITE(REG_ACTL_ACOS2, (UINT32)sValue);
	
	/* set the valid frame bit and valid slots */
	AUDIO_WRITE(REG_ACTL_ACOS0, AUDIO_READ(REG_ACTL_ACOS0) | 0x13);
	
	udelay(100);

	/* polling the AC_W_FINISH */
	for (nWait = 0; nWait < 0x10000; nWait++)
	{
		if (!(AUDIO_READ(REG_ACTL_ACCON) & AC_W_FINISH))
			break;
	}
	
	AUDIO_WRITE(REG_ACTL_ACOS0, AUDIO_READ(REG_ACTL_ACOS0) & ~3);

	up(&ac97_sem);
	
	
	// paraniod?
	//if (ac97_read_register(nIdx) != sValue)
	//	MSG2("ac97_write_register, nIdx=0x%x, mismatch, 0x%x must be 0x%x\n", nIdx, ac97_read_register(nIdx), sValue);

	return 0;
}

#if 0
static void ac97_read_all_registers()
{
	MSG("AC97_RESET            = 0x%04x\n", ac97_read_register(AC97_RESET));
	MSG("AC97_MASTER_VOLUME    = 0x%04x\n", ac97_read_register(AC97_MASTER_VOLUME));
	MSG("AC97_AUX_OUT_VOLUME   = 0x%04x\n", ac97_read_register(AC97_AUX_OUT_VOLUME));
	MSG("AC97_MONO_VOLUME      = 0x%04x\n", ac97_read_register(AC97_MONO_VOLUME));
	MSG("AC97_MASTER_TONE      = 0x%04x\n", ac97_read_register(AC97_MASTER_TONE));
	MSG("AC97_PC_BEEP_VOLUME   = 0x%04x\n", ac97_read_register(AC97_PC_BEEP_VOLUME));
	MSG("AC97_PHONE_VOLUME     = 0x%04x\n", ac97_read_register(AC97_PHONE_VOLUME));
	MSG("AC97_MIC_VOLUME       = 0x%04x\n", ac97_read_register(AC97_MIC_VOLUME));
	MSG("AC97_LINE_IN_VOLUME   = 0x%04x\n", ac97_read_register(AC97_LINE_IN_VOLUME));
	MSG("AC97_CD_VOLUME        = 0x%04x\n", ac97_read_register(AC97_CD_VOLUME));
	MSG("AC97_VIDEO_VOLUME     = 0x%04x\n", ac97_read_register(AC97_VIDEO_VOLUME));
	MSG("AC97_AUX_IN_VOLUME    = 0x%04x\n", ac97_read_register(AC97_AUX_IN_VOLUME));
	MSG("AC97_PCM_OUT_VOLUME   = 0x%04x\n", ac97_read_register(AC97_PCM_OUT_VOLUME));
	MSG("AC97_RECORD_SELECT    = 0x%04x\n", ac97_read_register(AC97_RECORD_SELECT));
	MSG("AC97_RECORD_GAIN      = 0x%04x\n", ac97_read_register(AC97_RECORD_GAIN));
	MSG("AC97_RECORD_GAIN_MIC  = 0x%04x\n", ac97_read_register(AC97_RECORD_GAIN_MIC));
	MSG("AC97_GENERAL_PURPOSE  = 0x%04x\n", ac97_read_register(AC97_GENERAL_PURPOSE));
	MSG("AC97_3D_CONTROL       = 0x%04x\n", ac97_read_register(AC97_3D_CONTROL));
	MSG("AC97_AUDIO_INT_PAGING = 0x%04x\n", ac97_read_register(AC97_AUDIO_INT_PAGING));
	MSG("AC97_POWERDOWN_CTRL   = 0x%04x\n", ac97_read_register(AC97_POWERDOWN_CTRL));
	MSG("AC97_FRONT_DAC_RATE   = 0x%04x\n", ac97_read_register(AC97_FRONT_DAC_RATE));
}
#endif

static void  ac97_play_isr(void)
{
	int bPlayLastBlock;

	ENTER();

	if(!(AUDIO_READ(REG_ACTL_CON) &  T_DMA_IRQ))
		return;

	AUDIO_WRITE(REG_ACTL_CON, AUDIO_READ(REG_ACTL_CON) | T_DMA_IRQ);
	
	if (_bPlayDmaToggle == 0)
	{
		if (AUDIO_READ(REG_ACTL_PSR) & P_DMA_MIDDLE_IRQ)
			AUDIO_WRITE(REG_ACTL_PSR, P_DMA_MIDDLE_IRQ);
		else
			MSG("ac97_play_isr - miss middle!\n");

		_bPlayDmaToggle = 1;

		bPlayLastBlock = _tAC97.fnPlayCallBack(_tAC97.uPlayBufferAddr, 
										_tAC97.uPlayBufferLength/2);
	}
	else
	{
		if (AUDIO_READ(REG_ACTL_PSR) & P_DMA_END_IRQ)
			AUDIO_WRITE(REG_ACTL_PSR, P_DMA_END_IRQ);
		else
			MSG("ac97_play_isr - miss end!\n");

		_bPlayDmaToggle = 0;

		bPlayLastBlock = _tAC97.fnPlayCallBack(_tAC97.uPlayBufferAddr+ _tAC97.uPlayBufferLength/ 2, 
									_tAC97.uPlayBufferLength/2);
	}


	/* here, we will check whether the next buffer is ready. If not, stop play. */
	if (bPlayLastBlock)
		AUDIO_WRITE(REG_ACTL_PSR, P_DMA_MIDDLE_IRQ | P_DMA_END_IRQ);


	LEAVE();

}		


static void ac97_rec_isr(void)
{
	int bPlayLastBlock;

	ENTER();

	if(!(AUDIO_READ(REG_ACTL_CON) &  R_DMA_IRQ))
		return;

	AUDIO_WRITE(REG_ACTL_CON, AUDIO_READ(REG_ACTL_CON) | R_DMA_IRQ);

	if (_bRecDmaToggle == 0)
	{
		if (AUDIO_READ(REG_ACTL_RSR) & R_DMA_MIDDLE_IRQ)
			AUDIO_WRITE(REG_ACTL_RSR, R_DMA_MIDDLE_IRQ);
		else
			MSG("ac97_rec_isr - miss middle!\n");
		
		_bRecDmaToggle = 1;
		bPlayLastBlock = _tAC97.fnRecCallBack(_tAC97.uRecordBufferAddr,
							_tAC97.uRecordBufferLength/2);
	}
	else
	{
		if (AUDIO_READ(REG_ACTL_RSR) & R_DMA_END_IRQ)
			AUDIO_WRITE(REG_ACTL_RSR, R_DMA_END_IRQ);
		else
			MSG("ac97_rec_isr - miss end!\n");
		
		_bRecDmaToggle = 0;
		bPlayLastBlock = _tAC97.fnRecCallBack(_tAC97.uRecordBufferAddr + _tAC97.uRecordBufferLength / 2, 
									_tAC97.uRecordBufferLength /2);
	}

	if (bPlayLastBlock)
		AUDIO_WRITE(REG_ACTL_RSR, R_DMA_MIDDLE_IRQ | R_DMA_END_IRQ);

	LEAVE();
}

static int ac97_reset(void)
{
	ENTER();

	AUDIO_WRITE(REG_CLKSEL, AUDIO_READ(REG_CLKSEL) | 0x10000);

	AUDIO_WRITE(0xFFF83000,0x155);//GPIO_CFG0  PT0CFG0~4
	AUDIO_WRITE(0xFFF83004,0xd);//GPIO4,1:In	  GPIO0,2,3:Out

	//AUDIO_WRITE(REG_GPIOA_OE, (AUDIO_READ(REG_GPIOA_OE) & 0xFFFFC0FF) | 0x2700);

	/* disable pull-high/low */
	//AUDIO_WRITE(REG_GPIOA_PE, 0x3F00);

	udelay(1000);

	_tAC97.sPlayVolume = 0x0000;
	_tAC97.sRecVolume = 0x0000;

	/* enable audio controller and AC-link interface */
	AUDIO_WRITE(REG_ACTL_CON, IIS_AC_PIN_SEL | AUDIO_EN | ACLINK_EN | PFIFO_EN | RFIFO_EN | T_DMA_IRQ | R_DMA_IRQ | DMA_EN);
	udelay(1000);

	/* reset Audio Controller */
	AUDIO_WRITE(REG_ACTL_RESET, AUDIO_READ(REG_ACTL_RESET) | ACTL_RESET_BIT);
	udelay(1000);
	AUDIO_WRITE(REG_ACTL_RESET, AUDIO_READ(REG_ACTL_RESET) & ~ACTL_RESET_BIT);
	udelay(1000);
	
	/* reset AC-link interface */
	AUDIO_WRITE(REG_ACTL_RESET, AUDIO_READ(REG_ACTL_RESET) | AC_RESET);
	udelay(1000);
	AUDIO_WRITE(REG_ACTL_RESET, AUDIO_READ(REG_ACTL_RESET) & ~AC_RESET);
	udelay(1000);


	/* cold reset AC 97 */
	AUDIO_WRITE(REG_ACTL_ACCON, AUDIO_READ(REG_ACTL_ACCON) | AC_C_RES);
	udelay(1000);
	AUDIO_WRITE(REG_ACTL_ACCON, AUDIO_READ(REG_ACTL_ACCON) & ~AC_C_RES);
	udelay(1000);

	
	if (!(AUDIO_READ(REG_ACTL_ACIS0) & 0x10))
	{
		MSG("Error - AC97 codec ready was not set, cold reset failed!\n");
		return ERR_AC97_CODEC_RESET;
	}

	udelay(100);
	//ac97_read_all_registers();
	
	/* set volumes */

	ac97_write_register(AC97_MASTER_VOLUME, _tAC97.sPlayVolume);
		
	ac97_write_register(AC97_MONO_VOLUME, 0x000f);
	//ac97_write_register(AC97_MASTER_TONE, 0x0303);
	ac97_write_register(AC97_MIC_VOLUME, 0x8000);
	//ac97_write_register(AC97_LINE_IN_VOLUME, 0x0707);
	//ac97_write_register(AC97_AUX_IN_VOLUME, 0x0707);
	ac97_write_register(AC97_PCM_OUT_VOLUME, _tAC97.sPlayVolume);
	
	ac97_write_register(AC97_RECORD_SELECT, 0);	/* record select MIC in */
	ac97_write_register(AC97_RECORD_GAIN, 0x0404);
	ac97_write_register(AC97_RECORD_GAIN_MIC, 0x0004);
	ac97_write_register(AC97_GENERAL_PURPOSE, 0);

	LEAVE();

	return 0;	
}

static VOID  ac97SetPlaySampleRate(INT nSamplingRate)
{
	ENTER();

	/* set play sampling rate */
	if (nSamplingRate != 48000)
	{
		/* enable VRA and set sampling frequency */
		ac97_write_register(AC97_EXT_AUDIO_CTRL, ac97_read_register(AC97_EXT_AUDIO_CTRL)|0x1);
		ac97_write_register(AC97_FRONT_DAC_RATE, nSamplingRate);
	}

	_tAC97.nPlaySamplingRate = nSamplingRate;

	LEAVE();
}

static VOID  ac97SetRecordSampleRate(INT nSamplingRate)
{
	ENTER();

	/* set record sampling rate */
	if (nSamplingRate != 48000)
	{
		/* enable VRA and set sampling frequency */
		ac97_write_register(AC97_EXT_AUDIO_CTRL, ac97_read_register(AC97_EXT_AUDIO_CTRL)|0x1);
		ac97_write_register(AC97_LR_ADC_RATE, nSamplingRate);
	}

	_tAC97.nRecSamplingRate = nSamplingRate;

	LEAVE();
}

static VOID ac97SetPlayCallBackFunction(AU_CB_FUN_T fnCallBack)
{
	ENTER();

	_tAC97.fnPlayCallBack = fnCallBack;

	LEAVE();
}

static VOID ac97SetRecordCallBackFunction(AU_CB_FUN_T fnCallBack)
{
	ENTER();
	
	_tAC97.fnRecCallBack = fnCallBack;

	LEAVE();
}



/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*      ac97SetPlayVolume                                               */
/*                                                                       */
/* DESCRIPTION                                                           */
/*      Set AC97 left and right channel play volume.                     */
/*                                                                       */
/* INPUTS                                                                */
/*      ucLeftVol    play volume of left channel                          */
/*      ucRightVol   play volume of left channel                          */
/*                  0:  mute                                             */
/*                  1:  minimal volume                                   */
/*                  31: maxmum volume                                    */
/*                                                                       */
/* OUTPUTS                                                               */
/*      None                                                             */
/*                                                                       */
/* RETURN                                                                */
/*      0           Success                                              */
/*		Otherwise	error												 */
/*                                                                       */
/*************************************************************************/
static int ac97SetPlayVolume(UINT32 ucLeftVol, UINT32 ucRightVol)
{
	int nLData, nRData;

	ENTER();

	//MSG2("Set AC97 volume to : %d-%d\n", ucLeftVol, ucRightVol);

	if (ucLeftVol == 0)
		nLData = 0x80;
	else
		nLData = 31 - (ucLeftVol & 0x1f);

	if (ucRightVol == 0)
		nRData = 0x80;
	else
		nRData = 31 - (ucRightVol & 0x1f);

	_tAC97.sPlayVolume = (nLData << 8) | nRData;
	ac97_write_register(AC97_PCM_OUT_VOLUME, (nLData << 8) | nRData);
	//ac97_write_register(AC97_PCM_OUT_VOLUME, 0x101);

	if (ucLeftVol == 0)
		nLData = 0x80;
	else
		nLData = 62 - ucLeftVol*2;

	if (ucRightVol == 0)
		nRData = 0x80;
	else
		nRData = 62 - ucRightVol*2;

	ac97_write_register(AC97_AUX_OUT_VOLUME, (nLData << 8) | nRData);
	ac97_write_register(AC97_MASTER_VOLUME, (nLData << 8) | nRData);

	MSG2("AC97_MASTER_VOLUME    = 0x%04x\r\n", ac97_read_register(AC97_MASTER_VOLUME));
	MSG2("AC97_AUX_OUT_VOLUME    = 0x%04x\r\n", ac97_read_register(AC97_AUX_OUT_VOLUME));
	MSG2("AC97_PCM_OUT_VOLUME    = 0x%04x\r\n", ac97_read_register(AC97_PCM_OUT_VOLUME));
#if 0	
	down(&ac97_sem);
	if (_bAC97Active & AC97_PLAY_ACTIVE)
		AUDIO_WRITE(REG_ACTL_ACOS0, 0x1f);
	else
		AUDIO_WRITE(REG_ACTL_ACOS0, 0);
	up(&ac97_sem);
#endif	
	LEAVE();
	
	return 0;
}


/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*      ac97SetRecordVolume                                             */
/*                                                                       */
/* DESCRIPTION                                                           */
/*      Set AC97 left and right channel record volume.                   */
/*                                                                       */
/* INPUTS                                                                */
/*      ucLeftVol    record volume of left channel                        */
/*      ucRightVol   record volume of left channel                        */
/*                  0:  mute                                             */
/*                  1:  minimal volume                                   */
/*                  31: maxmum volume                                    */
/*                                                                       */
/* OUTPUTS                                                               */
/*      None                                                             */
/*                                                                       */
/* RETURN                                                                */
/*      0           Success                                              */
/*		Otherwise	error												 */
/*                                                                       */
/*************************************************************************/
static int ac97SetRecordVolume(UINT32 ucLeftVol, UINT32 ucRightVol)
{
	//INT		nStatus;

	ENTER();

	if (ucLeftVol == 0)
		ucLeftVol = 0x80;
	else
		ucLeftVol = 32 - (ucLeftVol & 0x1f);
		
	if (ucRightVol == 0)
		ucRightVol = 0x80;
	else
		ucRightVol = 32 - (ucRightVol & 0x1f);

	_tAC97.sRecVolume = (ucLeftVol << 8) | ucRightVol;

	ac97_write_register(AC97_MIC_VOLUME, ucRightVol );

	//ac97_read_all_registers();
	LEAVE();
	
	return 0;
}


/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*      ac97StartPlay                                                   */
/*                                                                       */
/* DESCRIPTION                                                           */
/*      Start AC97 playback.                                             */
/*                                                                       */
/* INPUTS                                                                */
/*      fnCallBack     client program provided callback function. The audio */
/*                  driver will call back to get next block of PCM data  */
/*      nSamplingRate  the playback sampling rate. Supported sampling    */
/*                  rate are 48000, 44100, 32000, 24000, 22050, 16000,   */
/*                  11025, and 8000 Hz                                   */
/*      nChannels	number of playback nChannels                          */
/*					1: single channel, otherwise: double nChannels        */
/*                                                                       */
/* OUTPUTS                                                               */
/*      None                                                             */
/*                                                                       */
/* RETURN                                                                */
/*      0           Success                                              */
/*		Otherwise	error												 */
/*                                                                       */
/*************************************************************************/
static int ac97StartPlay(AU_CB_FUN_T fnCallBack, INT nSamplingRate, INT nChannels)
{
	INT	 nStatus;

	ENTER();

	if (_bAC97Active & AC97_PLAY_ACTIVE)
		return ERR_AC97_PLAY_ACTIVE;		/* AC97 was playing */

	
	if (_bAC97Active == 0)
	{
		AUDIO_WRITE(REG_ACTL_RESET, AUDIO_READ(REG_ACTL_RESET) &~PLAY_LEFT_CHNNEL &~PLAY_RIGHT_CHNNEL);
		AUDIO_WRITE(REG_ACTL_RESET, AUDIO_READ(REG_ACTL_RESET) | PLAY_RIGHT_CHNNEL);
		if (nChannels != 1)
			AUDIO_WRITE(REG_ACTL_RESET, AUDIO_READ(REG_ACTL_RESET) | PLAY_LEFT_CHNNEL);

		nStatus = ac97_reset();
		if (nStatus < 0)
			return nStatus;	
	}

	/* disable by Qfu , for the installation of irq has been done in wb_audio.c
		the only thing to do is to enable irq by this routine */
	Enable_Int(AU_PLAY_INT_NUM);

	/* set play sampling rate */
	ac97SetPlaySampleRate(nSamplingRate);
	ac97SetPlayCallBackFunction(fnCallBack);

	/* set DMA play destination base address */
	AUDIO_WRITE(REG_ACTL_PDSTB, _tAC97.uPlayBufferAddr);
	
	/* set DMA play buffer length */
	AUDIO_WRITE(REG_ACTL_PDST_LENGTH, _tAC97.uPlayBufferLength);
		
	/* start playing */
	MSG("AC97 start playing...\n");
	_bPlayDmaToggle = 0;
	down(&ac97_sem);
	AUDIO_WRITE(REG_ACTL_ACOS0, AUDIO_READ(REG_ACTL_ACOS0) | 0x1C);
	up(&ac97_sem);
	AUDIO_WRITE(REG_ACTL_PSR, 0x3);
	AUDIO_WRITE(REG_ACTL_RESET, AUDIO_READ(REG_ACTL_RESET) | AC_PLAY);
	_bAC97Active |= AC97_PLAY_ACTIVE;

	LEAVE();
	
	return 0;
}


/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*      ac97StopPlay                                                     */
/*                                                                       */
/* DESCRIPTION                                                           */
/*      Stop AC97 playback immdediately.                                 */
/*                                                                       */
/* INPUTS                                                                */
/*      None    	                                                     */
/*                                                                       */
/* OUTPUTS                                                               */
/*      None                                                             */
/*                                                                       */
/* RETURN                                                                */
/*      0           Success                                              */
/*		Otherwise	error												 */
/*                                                                       */
/*************************************************************************/
static void ac97StopPlay(void)
{
	ENTER();
	
	AUDIO_WRITE(REG_ACTL_RESET, AUDIO_READ(REG_ACTL_RESET) & ~(PLAY_RIGHT_CHNNEL | PLAY_LEFT_CHNNEL));

	if (!(_bAC97Active & AC97_PLAY_ACTIVE))
		return;

	
	//ac97_read_all_registers	();

	/* stop playing */
	AUDIO_WRITE(REG_ACTL_RESET, AUDIO_READ(REG_ACTL_RESET) & ~AC_PLAY);

	down(&ac97_sem);
	AUDIO_WRITE(REG_ACTL_ACOS0, AUDIO_READ(REG_ACTL_ACOS0) & ~0xC);
	up(&ac97_sem);


	_bAC97Active &= ~AC97_PLAY_ACTIVE; 
	/* disable audio play interrupt */
	if (!_bAC97Active)
		Disable_Int(AU_PLAY_INT_NUM);


	LEAVE();
	
	return;
}


/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*      ac97StartRecord                                                  */
/*                                                                       */
/* DESCRIPTION                                                           */
/*      Start AC97 record.                                               */
/*                                                                       */
/* INPUTS                                                                */
/*      fnCallBack  client program provided callback function. The audio */
/*                  driver will call back to deliver the newly recorded  */
/*                  block of PCM data                                    */
/*      nSamplingRate  the record sampling rate. Supported sampling      */
/*                  rate are 48000, 44100, 32000, 24000, 22050, 16000,   */
/*                  11025, and 8000 Hz                                   */
/*      nChannels	number of record nChannels                            */
/*					1: single channel, otherwise: double nChannels        */
/*                                                                       */
/* OUTPUTS                                                               */
/*      None                                                             */
/*                                                                       */
/* RETURN                                                                */
/*      0           Success                                              */
/*		Otherwise	error												 */
/*                                                                       */
/*************************************************************************/
static int ac97StartRecord(AU_CB_FUN_T fnCallBack, INT nSamplingRate, INT nChannels)
{
	INT		nStatus;
	unsigned int i;

	ENTER();

	if (_bAC97Active & AC97_REC_ACTIVE)
		return ERR_AC97_REC_ACTIVE;		/* AC97 was recording */
	
	if (_bAC97Active == 0)
	{	

		nStatus = ac97_reset();
		if (nStatus < 0)
			return nStatus;	
	}
	

	AUDIO_WRITE(REG_ACTL_RESET, AUDIO_READ(REG_ACTL_RESET) &~RECORD_RIGHT_CHNNEL &~RECORD_LEFT_CHNNEL);

	AUDIO_WRITE(REG_ACTL_RESET, AUDIO_READ(REG_ACTL_RESET) | RECORD_RIGHT_CHNNEL);
	if (nChannels != 1) {

		AUDIO_WRITE(REG_ACTL_RESET, AUDIO_READ(REG_ACTL_RESET) | RECORD_LEFT_CHNNEL);
	}			

	
	

	/* enable AC97 record interrupt */
	Enable_Int(AU_REC_INT_NUM);


	
	/* set record sampling rate */
	ac97SetRecordSampleRate(nSamplingRate);
	ac97SetRecordCallBackFunction(fnCallBack);


	/* set DMA record destination base address */
	AUDIO_WRITE(REG_ACTL_RDSTB, _tAC97.uRecordBufferAddr);
	
	/* set DMA record buffer length */
	AUDIO_WRITE(REG_ACTL_RDST_LENGTH, _tAC97.uRecordBufferLength);

	/* start recording */
	MSG("AC97 start recording...\n");
	_bRecDmaToggle = 0;
	AUDIO_WRITE(REG_ACTL_RSR, 0x3);
	AUDIO_WRITE(REG_ACTL_RESET, AUDIO_READ(REG_ACTL_RESET) | AC_RECORD);
	_bAC97Active |= AC97_REC_ACTIVE;
	
	LEAVE();

	return 0;
}


/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*      ac97StopRecord                                                  */
/*                                                                       */
/* DESCRIPTION                                                           */
/*      Stop AC97 record immediately.                                    */
/*                                                                       */
/* INPUTS                                                                */
/*      None    	                                                     */
/*                                                                       */
/* OUTPUTS                                                               */
/*      None                                                             */
/*                                                                       */
/* RETURN                                                                */
/*      0           Success                                              */
/*		Otherwise	error												 */
/*                                                                       */
/*************************************************************************/
static void ac97StopRecord(void)
{
	ENTER();
	
	AUDIO_WRITE(REG_ACTL_RESET, AUDIO_READ(REG_ACTL_RESET) & ~(RECORD_RIGHT_CHNNEL | RECORD_LEFT_CHNNEL));

	if (!(_bAC97Active & AC97_REC_ACTIVE))
		return;

	//ac97_read_all_registers	();
	/* stop recording */
	AUDIO_WRITE(REG_ACTL_RESET, AUDIO_READ(REG_ACTL_RESET) & ~AC_RECORD);

	_bAC97Active &= ~AC97_REC_ACTIVE; 
	/* disable audio record interrupt */
	if (!_bAC97Active)
		Disable_Int(AU_PLAY_INT_NUM);


	LEAVE();
	
	return;
}

static void ac97SetPlayBuffer(UINT32 uDMABufferAddr, UINT32 uDMABufferLength)
{
	ENTER();

	_tAC97.uPlayBufferAddr = uDMABufferAddr;
	_tAC97.uPlayBufferLength = uDMABufferLength;

	LEAVE();
}

static void ac97SetRecordBuffer(UINT32 uDMABufferAddr, UINT32 uDMABufferLength)
{
	ENTER();

	_tAC97.uRecordBufferAddr = uDMABufferAddr;
	_tAC97.uRecordBufferLength = uDMABufferLength;

	LEAVE();
}


static INT ac97Init(VOID)
{
	int nStatus = 0;

	ENTER();
	
	AUDIO_WRITE(REG_ACTL_RESET, AUDIO_READ(REG_ACTL_RESET) &~PLAY_LEFT_CHNNEL &~PLAY_RIGHT_CHNNEL);

	AUDIO_WRITE(REG_ACTL_RESET, AUDIO_READ(REG_ACTL_RESET) &~RECORD_LEFT_CHNNEL &~RECORD_RIGHT_CHNNEL);
	
	nStatus = ac97_reset();
	if (nStatus < 0)
		return nStatus;

	LEAVE();

	return 0;	
}

static INT ac97GetCapacity(VOID)
{
	return DSP_CAP_DUPLEX;		/* support full duplex */
}

WB_AUDIO_CODEC_T wb_ac97_codec = {
	dev:				AU_DEV_AC97,
	get_capacity:			ac97GetCapacity,
	set_play_buffer:		ac97SetPlayBuffer,
	set_record_buffer:		ac97SetRecordBuffer,
	reset:				ac97Init,
	start_play:			ac97StartPlay,
	stop_play:			ac97StopPlay,
	start_record:			ac97StartRecord,
	stop_record:			ac97StopRecord,
	set_play_volume:		ac97SetPlayVolume,
	set_record_volume:	ac97SetRecordVolume,
	play_interrupt:		ac97_play_isr,
	record_interrupt:		ac97_rec_isr,
};
