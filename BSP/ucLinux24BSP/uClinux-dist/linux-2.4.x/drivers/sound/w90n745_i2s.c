/****************************************************************************
 * 
 * Copyright (c) 2004 - 2007 Winbond Electronics Corp. All rights reserved.     
 *
 * 
 * FILENAME
 *     w90n745_I2S.c
 *
 * VERSION
 *     1.0 
 *
 * DESCRIPTION
 *	IIS for PCM3003E codec
 *
 * DATA STRUCTURES
 *     None
 *
 * FUNCTIONS
 *     
 * HISTORY
 *	2004.07.16		Created by Shih-Jen Lu
 *	2005.11.24		Modified by PC34 QFu
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

#include "w90n745_i2s.h"

//#define I2S_DEBUG
//#define I2S_DEBUG_ENTER_LEAVE
//#define I2S_DEBUG_MSG
//#define I2S_DEBUG_MSG2


#ifdef I2S_DEBUG
#define DBG(fmt, arg...)			printk(fmt, ##arg)
#else
#define DBG(fmt, arg...)
#endif

#ifdef I2S_DEBUG_ENTER_LEAVE
#define ENTER()					DBG("[%-10s] : Enter\n", __FUNCTION__)
#define LEAVE()					DBG("[%-10s] : Leave\n", __FUNCTION__)
#else
#define ENTER()
#define LEAVE()
#endif

#ifdef I2S_DEBUG_MSG
#define MSG(fmt)					DBG("[%-10s] : "fmt, __FUNCTION__)
#else
#define MSG(fmt)
#endif

#ifdef I2S_DEBUG_MSG2
#define MSG2(fmt, arg...)			DBG("[%-10s] : "fmt, __FUNCTION__, ##arg)
#else
#define MSG2(fmt, arg...)
#endif

static AUDIO_T	_tIIS;

#define AUDIO_WRITE(addr, val)		writel(val, addr)
#define AUDIO_READ(addr)			readl(addr)

#define Delay(time)					udelay(time * 10)

#define MSB_FORMAT	1
#define IIS_FORMAT  2

static int	 _bIISActive = 0;
static UINT32 _uIISCR = 0;

#define WMDEVID 0

#define UINT8		unsigned char

/*----- set data format -----*/
static void IIS_Set_Data_Format(int choose_format)
{

	ENTER();

	switch(choose_format){
		case IIS_FORMAT: _uIISCR = _uIISCR | IIS;
				break;
		case MSB_FORMAT: _uIISCR = _uIISCR | MSB_Justified;
				break;
		default:break;
	}
	AUDIO_WRITE(REG_ACTL_IISCON,_uIISCR);

	LEAVE();
}

/*----- set sample Frequency -----*/
static void IIS_Set_Sample_Frequency(int choose_sf)
{

	ENTER();

	switch (choose_sf)
	{
		case AU_SAMPLE_RATE_8000:							//8KHz
			_uIISCR = _uIISCR | FS_256 | SCALE_4 | BCLK_48;

			break;
		case AU_SAMPLE_RATE_11025:					//11.025KHz
			_uIISCR = _uIISCR | FS_256 | SCALE_4 | BCLK_48;

			break;
		case AU_SAMPLE_RATE_16000:						//16KHz
			_uIISCR = _uIISCR | FS_256 | SCALE_2 | BCLK_48;

			break;
		case AU_SAMPLE_RATE_22050:					//22.05KHz
			_uIISCR = _uIISCR | FS_256 | SCALE_3 | BCLK_32;

			break;
		case AU_SAMPLE_RATE_24000:						//24KHz
			_uIISCR = _uIISCR | FS_256 | SCALE_2 | BCLK_32;

			break;
		case AU_SAMPLE_RATE_32000:						//32KHz
			_uIISCR = _uIISCR | SCALE_1 | FS_256  | BCLK_48;

			break;
		case AU_SAMPLE_RATE_44100:					//44.1KHz
			_uIISCR = _uIISCR | SCALE_1 | FS_256  | BCLK_48;

			break;
		case AU_SAMPLE_RATE_48000:						//48KHz
			_uIISCR = _uIISCR | FS_256 | SCALE_1 | BCLK_32;

			break;
		default:break;
	}
	AUDIO_WRITE(REG_ACTL_IISCON,_uIISCR);

	if(choose_sf == AU_SAMPLE_RATE_44100 || choose_sf ==AU_SAMPLE_RATE_22050 || choose_sf ==AU_SAMPLE_RATE_11025 ){
		//outpw(REG_APLLCON, 0x642D);//16.934
		//outpw(REG_CLKDIV0, (inpw(REG_CLKDIV0) & 0xFF0FFFFF) | 0x300000);
		AUDIO_WRITE(0xfff00010, 0x2601);//PLLCON 390Mhz
		AUDIO_WRITE(0xfff00014, 0x116);// divide by 23
	}else
	{
		AUDIO_WRITE(0xfff00010, 0x2f01);//PLLCON 480Mhz
		AUDIO_WRITE(0xfff00014, 0x126);// divide by 39
	}

	LEAVE();
}

static INT  iis_reset(void)
{
	ENTER();

	AUDIO_WRITE(REG_CLKSEL, AUDIO_READ(REG_CLKSEL) | 0x10000);

	AUDIO_WRITE(0xFFF83000,0x155);//GPIO_CFG0  PT0CFG0~4
	AUDIO_WRITE(0xFFF83004,0x1d);//GPIO1:In	  GPIO0,2,3,4:Out
	
	/* reset audio interface */
	AUDIO_WRITE(REG_ACTL_RESET,AUDIO_READ(REG_ACTL_RESET) | ACTL_RESET_BIT);
	Delay(100);
	AUDIO_WRITE(REG_ACTL_RESET,AUDIO_READ(REG_ACTL_RESET) & ~ACTL_RESET_BIT);
	Delay(100);
	
	/* reset IIS interface */
	AUDIO_WRITE(REG_ACTL_RESET,AUDIO_READ(REG_ACTL_RESET) | IIS_RESET);
	Delay(100);
	AUDIO_WRITE(REG_ACTL_RESET,AUDIO_READ(REG_ACTL_RESET) & ~IIS_RESET);
	Delay(100);
	
	/* enable audio controller and IIS interface */
	AUDIO_WRITE(REG_ACTL_CON, AUDIO_READ (REG_ACTL_CON) | AUDCLK_EN | PFIFO_EN | DMA_EN | IIS_EN | AUDIO_EN | RFIFO_EN | T_DMA_IRQ | R_DMA_IRQ);
	
#if 0
	/* use GPIO 17 18 19 as L3 interface control pin */
	//writew(REG_GPIO_OE,readw(REG_GPIO_OE) & ~(1 << L3MODE_GPIO_NUM | 1<< L3CLOCK_GPIO_NUM | 1<< L3DATA_GPIO_NUM));
	AUDIO_WRITE(REG_GPIO_CFG,AUDIO_READ(REG_GPIO_CFG) & ~(0x3F<<14));
	AUDIO_WRITE(REG_GPIO_DIR,AUDIO_READ(REG_GPIO_DIR) | L3MODE_GPIO_NUM | L3CLOCK_GPIO_NUM | L3DATA_GPIO_NUM);
	
	/* set volume, dsp power up and no de-emph.no mute of external codec, from L3 */
#endif
	
	_uIISCR = 0;

	LEAVE();
	
	return 0;
}


VOID  i2sSetPlaySampleRate(INT nSamplingRate)
{
	_tIIS.nPlaySamplingRate = nSamplingRate;
}

VOID  i2sSetRecordSampleRate(INT nSamplingRate)
{
	_tIIS.nRecSamplingRate = nSamplingRate;
}


VOID i2sSetPlayCallBackFunction(AU_CB_FUN_T fnCallBack)
{
	_tIIS.fnPlayCallBack = fnCallBack;
}

VOID i2sSetRecordCallBackFunction(AU_CB_FUN_T fnCallBack)
{
	_tIIS.fnRecCallBack = fnCallBack;
}

/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*      i2sStartPlay                                                   */
/*                                                                       */
/* DESCRIPTION                                                           */
/*      Start IIS playback.                                             */
/*                                                                       */
/* INPUTS                                                                */
/*      fnCallBack     client program provided callback function. The audio */
/*                  driver will call back to get next block of PCM data  */
/*      nSamplingRate  the playback sampling rate. Supported sampling    */
/*                  rate are 48000, 44100, 32000, 24000, 22050, 16000,   */
/*                  11025, and 8000 Hz                                   */
/*                                                                       */
/* OUTPUTS                                                               */
/*      None                                                             */
/*                                                                       */
/* RETURN                                                                */
/*      0           Success                                              */
/*		Otherwise	error												 */
/*                                                                       */
/*************************************************************************/
static int i2sStartPlay(AU_CB_FUN_T fnCallBack, INT nSamplingRate, 
								INT nChannels)
{
	INT		nStatus /* ,L3status */;

	ENTER();
	
	if (_bIISActive & IIS_PLAY_ACTIVE){
		MSG("IIS already playing\n");
		return ERR_IIS_PLAY_ACTIVE;		/* IIS was playing */
	}

	//printk("SamplingRate : %d  Channels : %d\n", nSamplingRate, nChannels);

	if (_bIISActive == 0)
	{
		AUDIO_WRITE(REG_ACTL_RESET, AUDIO_READ(REG_ACTL_RESET) & ~ PLAY_LEFT_CHNNEL & ~ PLAY_RIGHT_CHNNEL);
		AUDIO_WRITE(REG_ACTL_RESET, AUDIO_READ(REG_ACTL_RESET) | PLAY_RIGHT_CHNNEL);
		if (nChannels == AU_CH_STEREO)
			AUDIO_WRITE(REG_ACTL_RESET, AUDIO_READ(REG_ACTL_RESET) | PLAY_LEFT_CHNNEL);
		nStatus = iis_reset();
		if (nStatus < 0)
			return nStatus;	
	}
	
//	enable_irq(AU_PLAY_INT_NUM);
	Enable_Int(AU_PLAY_INT_NUM);

	i2sSetPlayCallBackFunction(fnCallBack);
	i2sSetPlaySampleRate(nSamplingRate);

#if 0
	/* set play sampling rate and data format */
	//L3status = L3_Set_Sample_Frequency(nSamplingRate);
	//L3status = L3status | L3_Set_Data_Format(data_format);
	
	//L3_Set_Data(EX_DAC_On);
	//L3_Set_Status(L3status);
#endif


	IIS_Set_Sample_Frequency(nSamplingRate);
	IIS_Set_Data_Format(MSB_FORMAT);
	
	
	/* set DMA play destination base address */
	AUDIO_WRITE(REG_ACTL_PDSTB, _tIIS.uPlayBufferAddr);
	
	/* set DMA play buffer length */
	AUDIO_WRITE(REG_ACTL_PDST_LENGTH, _tIIS.uPlayBufferLength);

	MSG2("DMA Buffer : %x,  Length : %x\n", _tIIS.uPlayBufferAddr,_tIIS.uPlayBufferLength);

	/* call back to fill DMA play buffer */
	//_tIIS.fnPlayCallBack((UINT8 *)_tIIS.uDMABufferAddr, _tIIS.uDMABufferLength/2);
	//_tIIS.fnPlayCallBack((UINT8 *)(_tIIS.uDMABufferAddr + _tIIS.uDMABufferLength/2),
	//							_tIIS.uDMABufferLength/2);

	/* start playing */
	MSG("IIS start playing...\n");
	AUDIO_WRITE(REG_ACTL_PSR, 0x3);
	AUDIO_WRITE(REG_ACTL_RESET, AUDIO_READ(REG_ACTL_RESET) | IIS_PLAY);
	_bIISActive |= IIS_PLAY_ACTIVE;

	LEAVE();
	
	return 0;
}


/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*      i2sStopPlay                                                    */
/*                                                                       */
/* DESCRIPTION                                                           */
/*      Stop IIS playback immdediately.                                 */
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
static void i2sStopPlay(void)
{
	ENTER();
	
	AUDIO_WRITE(REG_ACTL_RESET, AUDIO_READ(REG_ACTL_RESET) & ~(PLAY_RIGHT_CHNNEL | PLAY_LEFT_CHNNEL));

	if (!(_bIISActive & IIS_PLAY_ACTIVE))
		return;
	
	MSG("IIS stop playing\n");

	/* stop playing */
	while( AUDIO_READ(REG_ACTL_RESET) & IIS_PLAY )
		AUDIO_WRITE(REG_ACTL_RESET, AUDIO_READ(REG_ACTL_RESET) & ~IIS_PLAY);
	
	_bIISActive &= ~IIS_PLAY_ACTIVE; 
	
	/* disable audio play interrupt */
	if (!_bIISActive)
		Disable_Int(AU_PLAY_INT_NUM);


	LEAVE();
	
	return;
}


/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*      i2sStartRecord                                                 */
/*                                                                       */
/* DESCRIPTION                                                           */
/*      Start IIS record.                                               */
/*                                                                       */
/* INPUTS                                                                */
/*      fnCallBack     client program provided callback function. The audio */
/*                  driver will call back to deliver the newly recorded  */
/*                  block of PCM data                                    */
/*      nSamplingRate  the record sampling rate. Supported sampling      */
/*                  rate are 48000, 44100, 32000, 24000, 22050, 16000,   */
/*                  11025, and 8000 Hz                                   */
/*                                                                       */
/* OUTPUTS                                                               */
/*      None                                                             */
/*                                                                       */
/* RETURN                                                                */
/*      0           Success                                              */
/*		Otherwise	error												 */
/*                                                                       */
/*************************************************************************/
static int i2sStartRecord(AU_CB_FUN_T fnCallBack, INT nSamplingRate, 
							INT nChannels)
{
	INT		nStatus /* ,L3status */;

	ENTER();

	if (_bIISActive & IIS_REC_ACTIVE)
		return ERR_IIS_REC_ACTIVE;		/* IIS was recording */

	AUDIO_WRITE(REG_ACTL_RESET, AUDIO_READ(REG_ACTL_RESET) & ~(RECORD_LEFT_CHNNEL & RECORD_RIGHT_CHNNEL));
	AUDIO_WRITE(REG_ACTL_RESET, AUDIO_READ(REG_ACTL_RESET) | RECORD_LEFT_CHNNEL);
	if (nChannels != 1)
		AUDIO_WRITE(REG_ACTL_RESET, AUDIO_READ(REG_ACTL_RESET) | RECORD_RIGHT_CHNNEL);			

	if (_bIISActive == 0)
	{
		nStatus = iis_reset();
		if (nStatus < 0)
			return nStatus;	
	}

	Enable_Int(AU_REC_INT_NUM);
	
	i2sSetRecordCallBackFunction(fnCallBack);
	i2sSetRecordSampleRate(nSamplingRate);	

#if 0
	/* set record sampling rate and data format */
	//L3status = L3_Set_Sample_Frequency(nSamplingRate);
	//L3status = L3status | L3_Set_Data_Format(MSB_FORMAT);
	
	//L3_Set_Data(EX_ADC_On);
	//L3_Set_Status(L3status);
#endif

	IIS_Set_Sample_Frequency(nSamplingRate);
	IIS_Set_Data_Format(MSB_FORMAT);
	
	

	/* set DMA record destination base address */
	AUDIO_WRITE(REG_ACTL_RDSTB, _tIIS.uRecordBufferAddr);
	
	/* set DMA record buffer length */
	AUDIO_WRITE(REG_ACTL_RDST_LENGTH, _tIIS.uRecordBufferLength);
	
	/* start recording */
	MSG("IIS start recording...\n");
	AUDIO_WRITE(REG_ACTL_RSR, 0x3);
	AUDIO_WRITE(REG_ACTL_RESET, AUDIO_READ(REG_ACTL_RESET) | IIS_RECORD);
	_bIISActive |= IIS_REC_ACTIVE;

	LEAVE();
	
	return 0;
}


/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*      i2sStopRecord                                                  */
/*                                                                       */
/* DESCRIPTION                                                           */
/*      Stop IIS record immediately.                                    */
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
static void i2sStopRecord(void)
{
	ENTER();

	AUDIO_WRITE(REG_ACTL_RESET, AUDIO_READ(REG_ACTL_RESET) & ~(RECORD_RIGHT_CHNNEL | RECORD_LEFT_CHNNEL));

	if (!(_bIISActive & IIS_REC_ACTIVE))
		return;
	
	MSG("IIS stop recording\n");

	/* stop recording */
	AUDIO_WRITE(REG_ACTL_RESET, AUDIO_READ(REG_ACTL_RESET) & ~IIS_RECORD);
	
	_bIISActive &= ~IIS_REC_ACTIVE;
	/* disable audio record interrupt */
	if (!_bIISActive)
		Disable_Int(AU_PLAY_INT_NUM);


	LEAVE();
	
	return;
}


/*************************************************************************/
/*                                                                       */
/* FUNCTION                                                              */
/*      i2sSetPlayVolume                                                 */
/*                                                                       */
/* DESCRIPTION                                                           */
/*      Set i2S left and right channel play volume.                      */
/*                                                                       */
/* INPUTS                                                                */
/*      ucLeftVol    play volume of left channel                         */
/*      ucRightVol   play volume of left channel                         */
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
static int i2sSetPlayVolume(UINT32 ucLeftVol, UINT32 ucRightVol)  //0~31
{
	ENTER();

	if (ucLeftVol>31)
		ucLeftVol=31;
	if (ucRightVol>31)
		ucRightVol=31;

	//printk("Set IIS Play volume to : %d-%d\n", ucLeftVol, ucRightVol);
	
	_tIIS.sPlayVolume = 0x3F - ucLeftVol*2;

#if 0	
	/*----- Address Mode -----*/
	L3_Address_Mode(EX_1345ADDR | EX_DATA);//set the DATA(volumn,de-emphasis, mute, and power control)
	/*----- Data Transfer Mode -----*/
	L3_Data_Transfer_Mode((UINT8)_tIIS.sPlayVolume);
#endif
	
	LEAVE();
	
	return 0;
}

static int i2sSetRecordVolume(UINT32 ucLeftVol, UINT32 ucRightVol)
{
	ENTER();

	LEAVE();

	return 0;

}

static void i2sSetPlayBuffer(UINT32 uDMABufferAddr, UINT32 uDMABufferLength)
{
	ENTER();

	_tIIS.uPlayBufferAddr = uDMABufferAddr;
	_tIIS.uPlayBufferLength = uDMABufferLength;

	LEAVE();
}

static void i2sSetRecordBuffer(UINT32 uDMABufferAddr, UINT32 uDMABufferLength)
{
	ENTER();

	_tIIS.uRecordBufferAddr = uDMABufferAddr;
	_tIIS.uRecordBufferLength = uDMABufferLength;

	LEAVE();
}

static void iis_play_isr(void)
{
	int bPlayLastBlock = 0;
	
	ENTER();

	MSG2("[DMA:S:%x,L:%x,C:%x]\n", 
		AUDIO_READ(REG_ACTL_PDSTB),
		AUDIO_READ(REG_ACTL_PDST_LENGTH),
		AUDIO_READ(REG_ACTL_PDSTC));
		

	AUDIO_WRITE(REG_ACTL_CON, AUDIO_READ(REG_ACTL_CON) | T_DMA_IRQ);
	
	if (AUDIO_READ(REG_ACTL_PSR) & P_DMA_MIDDLE_IRQ)
	{
		AUDIO_WRITE(REG_ACTL_PSR, P_DMA_MIDDLE_IRQ);
		bPlayLastBlock = _tIIS.fnPlayCallBack(_tIIS.uPlayBufferAddr, 
												_tIIS.uPlayBufferLength/2);
	}
	else if (AUDIO_READ(REG_ACTL_PSR) & P_DMA_END_IRQ)
	{
		AUDIO_WRITE(REG_ACTL_PSR, P_DMA_END_IRQ);
		bPlayLastBlock = _tIIS.fnPlayCallBack(_tIIS.uPlayBufferAddr + _tIIS.uPlayBufferLength/2, 
									_tIIS.uPlayBufferLength/2);
	}


	/* check whether the next block is ready. If not, stop play */

	if (bPlayLastBlock)
	{
		AUDIO_WRITE(REG_ACTL_PSR, P_DMA_MIDDLE_IRQ | P_DMA_END_IRQ);
//		i2sStopPlay();
	}


	LEAVE();

}		


static void  iis_rec_isr(void)
{
	int bPlayLastBlock = 0;
	
	ENTER();
	
	AUDIO_WRITE(REG_ACTL_CON, AUDIO_READ(REG_ACTL_CON) | R_DMA_IRQ);

	if (AUDIO_READ(REG_ACTL_RSR) & R_DMA_MIDDLE_IRQ)
	{
		AUDIO_WRITE(REG_ACTL_RSR, R_DMA_MIDDLE_IRQ);
		bPlayLastBlock = _tIIS.fnRecCallBack(_tIIS.uRecordBufferAddr, _tIIS.uRecordBufferLength/2);
	}
	else if (AUDIO_READ(REG_ACTL_RSR) & R_DMA_END_IRQ)
	{
		AUDIO_WRITE(REG_ACTL_RSR, R_DMA_END_IRQ);
		bPlayLastBlock = _tIIS.fnRecCallBack(_tIIS.uRecordBufferAddr + _tIIS.uRecordBufferLength/2, 
									_tIIS.uRecordBufferLength/2);
	}

	/* check whether the next block is token away. If not, stop record */

	if (bPlayLastBlock)
	{
		AUDIO_WRITE(REG_ACTL_RSR, R_DMA_MIDDLE_IRQ | R_DMA_END_IRQ);
	}

	LEAVE();
}


INT i2sInit(VOID)
{
	int nStatus = 0;
	
	AUDIO_WRITE(REG_ACTL_RESET, AUDIO_READ(REG_ACTL_RESET) &~PLAY_LEFT_CHNNEL &~PLAY_RIGHT_CHNNEL);
	AUDIO_WRITE(REG_ACTL_RESET, AUDIO_READ(REG_ACTL_RESET) &~RECORD_LEFT_CHNNEL &~RECORD_RIGHT_CHNNEL);
	
	nStatus = iis_reset();
	if (nStatus < 0)
		return nStatus;
	
	return 0;	
}

static INT i2sGetCapacity(VOID)
{
	return DSP_CAP_DUPLEX;		/* support full duplex */
}

WB_AUDIO_CODEC_T wb_i2s_codec = {
	dev:				AU_DEV_IIS,	
	get_capacity:			i2sGetCapacity,
	set_play_buffer:		i2sSetPlayBuffer,
	set_record_buffer:		i2sSetRecordBuffer,
	reset:				i2sInit,
	start_play:			i2sStartPlay,
	stop_play:			i2sStopPlay,
	start_record:			i2sStartRecord,
	stop_record:			i2sStopRecord,
	set_play_volume:		i2sSetPlayVolume,
	set_record_volume:	i2sSetRecordVolume,					/* not supprted */
	play_interrupt:		iis_play_isr,
	record_interrupt:		iis_rec_isr,
};
