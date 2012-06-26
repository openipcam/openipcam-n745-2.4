/****************************************************************************
 * 
 * Copyright (c) 2004 - 2007 Winbond Electronics Corp. All rights reserved.     
 *
 *
 * FILENAME
 *     w90n745_Audio.c
 *
 * VERSION
 *     1.0 
 *
 * DESCRIPTION
 *	Winbond w90n745 Audio Driver for Linux 2.4.x with OSS frame
 *
 * DATA STRUCTURES
 *     None
 *
 * FUNCTIONS
 *
 *     
 * HISTORY
 *	 2005.11.24		Created by PC34 QFu
 *
 * REMARK
 *     None
 *
 **************************************************************************/
#include <linux/config.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/ioport.h>
#include <linux/delay.h>
#include <linux/poll.h>
#include <linux/soundcard.h>
#include <linux/sound.h>

#include <asm/io.h>
#include <asm/system.h>
#include <asm/uaccess.h>
#include <asm/arch/irqs.h>

#include "w90n745_audio_regs.h"
#include "w90n745_audio.h"

#define AUDIO_BUFFER_ORDER	4		/*  64K x 2 (play + record) */

#define FRAG_SIZE	(( PAGE_SIZE << AUDIO_BUFFER_ORDER ) / 2)

#define WB_AUDIO_DEFAULT_SAMPLERATE		AU_SAMPLE_RATE_44100
#define WB_AUDIO_DEFAULT_CHANNEL			2
#define WB_AUDIO_DEFAULT_VOLUME			30

//#define WB_AU_DEBUG
//#define WB_AU_DEBUG_ENTER_LEAVE
//#define WB_AU_DEBUG_MSG
//#define WB_AU_DEBUG_MSG2

#ifdef WB_AU_DEBUG
#define DBG(fmt, arg...)			printk(fmt, ##arg)
#else
#define DBG(fmt, arg...)
#endif

#ifdef WB_AU_DEBUG_ENTER_LEAVE
#define ENTER()					DBG("[%-10s] : Enter\n", __FUNCTION__)
#define LEAVE()					DBG("[%-10s] : Leave\n", __FUNCTION__)
#else
#define ENTER()
#define LEAVE()
#endif

#ifdef WB_AU_DEBUG_MSG
#define MSG(fmt)					DBG("[%-10s] : "fmt, __FUNCTION__)
#else
#define MSG(fmt)
#endif

#ifdef WB_AU_DEBUG_MSG2
#define MSG2(fmt, arg...)			DBG("[%-10s] : "fmt, __FUNCTION__, ##arg)
#else
#define MSG2(fmt, arg...)
#endif


static WB_AUDIO_T audio_dev;
static WB_AUDIO_CODEC_T *all_codecs[2] = {&wb_i2s_codec,  &wb_ac97_codec};
static int dev_dsp[2] = {-1, -1}, dev_mixer[2] = {-1, -1};
static int nSamplingRate[]={AU_SAMPLE_RATE_8000,
							AU_SAMPLE_RATE_11025,
							AU_SAMPLE_RATE_16000,
							AU_SAMPLE_RATE_22050,
							AU_SAMPLE_RATE_24000,
							AU_SAMPLE_RATE_32000,
							AU_SAMPLE_RATE_44100,
							AU_SAMPLE_RATE_48000};

static void wb_audio_stop_play(void)
{
	MSG("Stop Play! \n");

	if ( audio_dev.state & AU_STATE_PLAYING){
		audio_dev.codec->set_play_volume(0, 0);
		audio_dev.codec->stop_play();
		audio_dev.state &= ~AU_STATE_PLAYING;
		wake_up_interruptible(&audio_dev.write_wait_queue);
		MSG2("Buf[0] : %x, Buf[1] : %x\n",
			audio_dev.play_half_buf[0].ptr,
			audio_dev.play_half_buf[1].ptr);

	}
}

static void wb_audio_stop_record(void)
{
	MSG("Stop Record!\n");
	
	if ( audio_dev.state & AU_STATE_RECORDING){
		audio_dev.codec->set_record_volume(0, 0);
		audio_dev.codec->stop_record();
		audio_dev.state &= ~AU_STATE_RECORDING;
		wake_up_interruptible(&audio_dev.read_wait_queue);
	}
}

static int play_callback(UINT32 uAddr, UINT32 uLen)
{
	int i = 1;

	ENTER();

	if(uAddr == audio_dev.play_buf_addr){
		MSG("<");
		i = 0;
	}
	else
		MSG(">");

	audio_dev.play_half_buf[i].ptr = 0;		/* set buffer empty flag */

	wake_up_interruptible(&audio_dev.write_wait_queue);	/* wake up all block write system call */

	if(audio_dev.play_half_buf[i^1].ptr /*== FRAG_SIZE*/ > 0){	/* check whether next buffer is full ? */
		LEAVE();
		return 0;
	}
	else{
		wb_audio_stop_play();
		return 1;
	}
}

static int record_callback(UINT32 uAddr, UINT32 uLen)
{
	int i = 1;

	if(uAddr == audio_dev.record_buf_addr)
		i = 0;

	audio_dev.record_half_buf[i].ptr =0;		/* indicate read from buffer[0] */

	wake_up_interruptible(&audio_dev.read_wait_queue);	/* wake up all blocked read system call */

	if ( audio_dev.record_half_buf[i ^ 1].ptr == 0) {	/* last block wan't take off , user may stop record */
		wb_audio_stop_record();
		return 1;
	}
	else
		return 0;
}

static int wb_audio_start_play(void)
{
	int ret = 0;

	MSG("Start Playing ... \n");

	if ( audio_dev.state & AU_STATE_PLAYING)	/* playing? */
		return 0;

	if ( audio_dev.state & AU_STATE_RECORDING ) {		/* recording? */
		if (!(audio_dev.codec->get_capacity() & DSP_CAP_DUPLEX))		/* not support full duplex */
			wb_audio_stop_record();
	}

	audio_dev.state |= AU_STATE_PLAYING;

	MSG2("Buf[0] : %x, Buf[1] : %x\n",
		audio_dev.play_half_buf[0].ptr,
		audio_dev.play_half_buf[1].ptr);
	
	if ( audio_dev.codec->start_play(play_callback, 
								audio_dev.nSamplingRate,
								audio_dev.nChannels)) {
		audio_dev.state &= ~AU_STATE_PLAYING;
		MSG("Play error\n");

		ret = -EIO;
	}
	else{
		audio_dev.codec->set_play_volume(audio_dev.nPlayVolumeLeft, audio_dev.nPlayVolumeRight);
	}

	return ret;
}

static int wb_audio_start_record(void)
{
	int ret=0;

	MSG("Start Recording ... \n");

	if (audio_dev.state & AU_STATE_RECORDING)
		return 0;

	if (audio_dev.state & AU_STATE_PLAYING) {
		if (!(audio_dev.codec->get_capacity() & DSP_CAP_DUPLEX))
			wb_audio_stop_play();
	}

	audio_dev.record_half_buf[0].ptr =FRAG_SIZE;
	audio_dev.record_half_buf[1].ptr = FRAG_SIZE;
	audio_dev.state |= AU_STATE_RECORDING;

	if ( audio_dev.codec->start_record((AU_CB_FUN_T)record_callback,
								audio_dev.nSamplingRate,
								audio_dev.nChannels)) {						
		audio_dev.state &= ~AU_STATE_RECORDING;
		ret = -EIO;
	}
	else{
		audio_dev.codec->set_record_volume(audio_dev.nRecordVolumeLeft, audio_dev.nRecordVolumeRight);
	}	

	return ret;
}

static int wb_mixer_open(struct inode *inode, struct file *file)
{
	int retval;
	
	int minor = MINOR(inode->i_rdev);

	ENTER();

	if(minor != 0 && minor != 16)
		return -ENODEV;

	if(minor == 16)
		minor = 1;

	down(&audio_dev.mixer_sem);

	retval = -EBUSY;

	if(audio_dev.open_flags != 0){
		if(audio_dev.mixer_openflag != 0)
			goto quit;
		else{
			if(audio_dev.dsp_openflag != 0 && audio_dev.dsp_dev != minor)
				goto quit;
		}
	}
	
	audio_dev.open_flags = 1;
	audio_dev.mixer_openflag = 1;
	audio_dev.mixer_dev = minor;

	audio_dev.codec = all_codecs[minor];

	MSG2("Mixer[%d] opened\n", minor);

	retval = 0;

	MOD_INC_USE_COUNT;

quit:
	up(&audio_dev.mixer_sem);

	LEAVE();
	
	return retval;
}

static int wb_mixer_release(struct inode *inode, struct file *file)
{
	audio_dev.mixer_openflag = 0;
	if(audio_dev.dsp_openflag == 0)
		audio_dev.open_flags = 0;

	MOD_DEC_USE_COUNT;
	
	return 0;
}

static int wb_mixer_ioctl(struct inode *inode, struct file *file,
			       unsigned int cmd, unsigned long arg)
{
	int ret = 0, val=0, err = 0;
	int tmpVolumeLeft, tmpVolumeRight;

	if (cmd == SOUND_MIXER_INFO) {
		mixer_info info;
		memset(&info,0,sizeof(info));
              strncpy(info.id,"w90n745",sizeof(info.id)-1);
              strncpy(info.name,"Winbond w90n745 Audio",sizeof(info.name)-1);
              info.modify_counter = 0;
              if (copy_to_user((void *)arg, &info, sizeof(info)))
                      return -EFAULT;
		return 0;
	}
	if (cmd == SOUND_OLD_MIXER_INFO) {
		_old_mixer_info info;
		memset(&info,0,sizeof(info));
              strncpy(info.id,"w90n745",sizeof(info.id)-1);
              strncpy(info.name,"Winbond w90n745 Audio",sizeof(info.name)-1);
              if (copy_to_user((void *)arg, &info, sizeof(info)))
                      return -EFAULT;
		return 0;
	}
	if (cmd == OSS_GETVERSION)
		return put_user(SOUND_VERSION, (int *)arg);

	/* read */
	if (_SIOC_DIR(cmd) & _SIOC_WRITE)
		if (get_user(val, (int *)arg))
			return -EFAULT;

	switch (cmd) {
		case MIXER_READ(SOUND_MIXER_CAPS):
			ret = SOUND_CAP_EXCL_INPUT;		/* only one input can be selected */
			break;
		case MIXER_READ(SOUND_MIXER_STEREODEVS):
			ret = 1;									/* check whether support stereo */
			break;
		
		case MIXER_READ(SOUND_MIXER_RECMASK):		/* get input channels mask */
			ret = SOUND_MASK_MIC;
		
		case MIXER_READ(SOUND_MIXER_DEVMASK):		/* get all channels mask */
			ret |= SOUND_MASK_PCM + SOUND_MASK_VOLUME;
			break;

		case MIXER_WRITE(SOUND_MIXER_RECSRC):
			if ( val != SOUND_MASK_MIC )
				err = -EPERM;
			
		case MIXER_READ(SOUND_MIXER_RECSRC):
			ret = SOUND_MASK_MIC;
			break;

		case MIXER_WRITE(SOUND_MIXER_VOLUME):
		case MIXER_WRITE(SOUND_MIXER_PCM):
		case MIXER_WRITE(SOUND_MIXER_MIC):
			tmpVolumeLeft = (val & 0xff) * 31  / 100;
			tmpVolumeRight = ((val >> 8) & 0xff) * 31 / 100;
			if (cmd == MIXER_WRITE(SOUND_MIXER_MIC)){	/* set mic volume */
				audio_dev.codec->set_record_volume(tmpVolumeLeft, tmpVolumeRight);
				audio_dev.nRecordVolumeLeft = tmpVolumeLeft;
				audio_dev.nRecordVolumeRight = tmpVolumeRight;
			}
			else{
				audio_dev.codec->set_play_volume(tmpVolumeLeft, tmpVolumeRight);		/* set play volume */
				audio_dev.nPlayVolumeLeft = tmpVolumeLeft;
				audio_dev.nPlayVolumeRight = tmpVolumeRight;
			}
			
			ret = (val & 0xffff);

			break;

		case MIXER_READ(SOUND_MIXER_VOLUME):
		case MIXER_READ(SOUND_MIXER_PCM):
			ret = ((audio_dev.nPlayVolumeLeft * 100  / 31) | 
				((audio_dev.nPlayVolumeRight * 100  / 31 ) << 8));
			break;

		case MIXER_READ(SOUND_MIXER_MIC):
			ret =((audio_dev.nRecordVolumeLeft * 100  / 31) | 
				((audio_dev.nRecordVolumeRight * 100  / 31 ) << 8));
			break;
		
		default:
			return -EINVAL;
	}

	if (put_user(ret, (int *)arg))
		return -EFAULT;

	return err;
}


static struct file_operations wb_mixer_fops = {
	owner:	THIS_MODULE,
	ioctl:	wb_mixer_ioctl,
	open:	wb_mixer_open,
	release:	wb_mixer_release,
};

static void wb_dsp_irq(int irq, void *dev_id, struct pt_regs * regs)
{
	ENTER();
	
	if( audio_dev.state & AU_STATE_PLAYING)
		audio_dev.codec->play_interrupt();
	if(audio_dev.state & AU_STATE_RECORDING)
		audio_dev.codec->record_interrupt();

	LEAVE();
}

static int wb_dsp_open(struct inode *inode, struct file *file)
{
	int minor = MINOR(inode->i_rdev);
	int retval = -EBUSY;

	ENTER();

	if(minor != 3 && minor != 19){
		MSG2("Minor error : %d\n", minor);
		return -ENODEV;
	}

	if(minor == 3)
		minor = 0;
	else
		minor = 1;

	if(audio_dev.open_flags != 0){
		if(audio_dev.dsp_openflag != 0)
			goto quit;
		else{
			if(audio_dev.mixer_openflag != 0 && audio_dev.mixer_dev != minor)
				goto quit;
		}
	}

	if((retval = request_irq(INT_AC97, wb_dsp_irq, SA_INTERRUPT, "wb audio", NULL))){
		printk("wb_audio_init : Request IRQ error\n");
		goto quit;
	}

	//enable_irq(INT_AC97);

	audio_dev.open_flags = 1;
	audio_dev.dsp_openflag = 1;
	audio_dev.dsp_dev = minor;
	audio_dev.state = AU_STATE_NOP;

	audio_dev.play_buf_addr = __get_free_pages(GFP_KERNEL, AUDIO_BUFFER_ORDER);
	if(audio_dev.play_buf_addr == NULL){
		free_irq(INT_AC97, NULL);
		MSG("Not enough memory\n");
		return -ENOMEM;
	}

	audio_dev.record_buf_addr = __get_free_pages(GFP_KERNEL, AUDIO_BUFFER_ORDER);
	if(audio_dev.record_buf_addr == NULL){
		free_pages(audio_dev.play_buf_addr, AUDIO_BUFFER_ORDER);
		free_irq(INT_AC97, NULL);
		MSG("Not enough memory\n");
		return -ENOMEM;
	}

	audio_dev.play_buf_addr |= 0x80000000;	// none - cache
	audio_dev.play_buf_length = ( PAGE_SIZE << AUDIO_BUFFER_ORDER );
	audio_dev.record_buf_addr |= 0x80000000;	// none - cache
	audio_dev.record_buf_length = ( PAGE_SIZE << AUDIO_BUFFER_ORDER );

	MSG2("Audio_Dev.play_buf_addr : %x, Length: %x\n", 
		audio_dev.play_buf_addr, audio_dev.play_buf_length);

	init_waitqueue_head(&audio_dev.read_wait_queue);
	init_waitqueue_head(&audio_dev.write_wait_queue);

	memset(&audio_dev.play_half_buf, 0, sizeof(audio_dev.play_half_buf));
	memset(&audio_dev.record_half_buf, 0, sizeof(audio_dev.record_half_buf));

	audio_dev.nSamplingRate = WB_AUDIO_DEFAULT_SAMPLERATE;
	audio_dev.nChannels = WB_AUDIO_DEFAULT_CHANNEL;
	audio_dev.nPlayVolumeLeft = WB_AUDIO_DEFAULT_VOLUME;
	audio_dev.nPlayVolumeRight = WB_AUDIO_DEFAULT_VOLUME;
	audio_dev.nRecordVolumeLeft = WB_AUDIO_DEFAULT_VOLUME;
	audio_dev.nRecordVolumeRight = WB_AUDIO_DEFAULT_VOLUME;
	
	audio_dev.codec = all_codecs[minor];

	/* set dma buffer */
	audio_dev.codec->set_play_buffer(audio_dev.play_buf_addr,
					audio_dev.play_buf_length);
	audio_dev.codec->set_record_buffer(audio_dev.record_buf_addr,
					audio_dev.record_buf_length);

	audio_dev.codec->reset();

	MOD_INC_USE_COUNT;

	LEAVE();

	return 0;

quit:

	MSG2("Open failed : %d\n", retval);

	return retval;
}

static int wb_dsp_release(struct inode *inode, struct file *file)
{
	ENTER();

	if(audio_dev.state & AU_STATE_PLAYING){
		/* wait until stop playing */
		wait_event_interruptible(audio_dev.write_wait_queue,
								(audio_dev.state & AU_STATE_PLAYING)  == 0 ||
								(audio_dev.play_half_buf[0].ptr == 0 &&
								audio_dev.play_half_buf[1].ptr == 0));
	}
	wb_audio_stop_play();
	wb_audio_stop_record();

	free_irq(INT_AC97, NULL);

	free_pages(audio_dev.play_buf_addr & 0x7fffffff, AUDIO_BUFFER_ORDER);
	free_pages(audio_dev.record_buf_addr & 0x7fffffff, AUDIO_BUFFER_ORDER);

	audio_dev.dsp_openflag = 0;
	if(audio_dev.mixer_openflag == 0)
		audio_dev.open_flags = 0;

	MOD_DEC_USE_COUNT;

	LEAVE();
	
	return 0;
}

static ssize_t wb_dsp_read(struct file *file, char *buffer,
				size_t swcount, loff_t *ppos)
{
	int i, tmp, length, block_len, retval, bpos;
	char *dma_buf = (char *)audio_dev.record_buf_addr;

	ENTER();

	if(swcount == 0)
		return 0;

	if(down_interruptible(&audio_dev.dsp_read_sem))
		return -ERESTARTSYS;

again:
	
	if((audio_dev.state & AU_STATE_RECORDING) == 0) {	/* if record not start, then start it */
		retval = wb_audio_start_record();
		if ( retval )
			goto quit;
	}

	length = swcount;
	block_len = FRAG_SIZE;
	retval = -EFAULT;

	if(audio_dev.record_half_buf[0].ptr == FRAG_SIZE && 
	   audio_dev.record_half_buf[1].ptr == FRAG_SIZE){	/* buffer empty */
		if( file->f_flags & O_NONBLOCK){
			retval = -EAGAIN;
			goto quit;
		}
		else {
			wait_event_interruptible(audio_dev.read_wait_queue, 
					(audio_dev.state & AU_STATE_RECORDING) == 0 ||
					audio_dev.record_half_buf[0].ptr == 0 || 
					audio_dev.record_half_buf[1].ptr == 0 );
			if ( (audio_dev.state & AU_STATE_RECORDING)  == 0){
				retval = 0;
				goto quit;
			}
		}

	}

	retval = 0;
	bpos = 0;
	for(i = 0; i < 2; i++){
		tmp = block_len - audio_dev.record_half_buf[i].ptr;

		if(swcount < tmp)
			tmp = swcount;

		if(tmp){
			retval = -EFAULT;
			if(copy_to_user(buffer + bpos, 
				dma_buf + i * block_len + audio_dev.record_half_buf[i].ptr , tmp))
				goto quit;
		}
		else
			continue;

		swcount -= tmp;
		audio_dev.record_half_buf[i].ptr += tmp;
		bpos += tmp;

		if(swcount == 0)
			break;
	
	}


	retval = length - swcount;

	if(swcount != 0){
		if( file->f_flags & O_NONBLOCK 
			&& audio_dev.play_half_buf[0].ptr == FRAG_SIZE 
			&& audio_dev.play_half_buf[1].ptr == FRAG_SIZE)
			goto quit;
		
		buffer += retval;
		goto again;
	}

quit:
	up(&audio_dev.dsp_read_sem);

	LEAVE();

	return retval;
}

static ssize_t wb_dsp_write(struct file *file, const char *buffer,
				 size_t count, loff_t *ppos)
{
	int tmp, retval, length, i;
	char *dma_buf = (char *)audio_dev.play_buf_addr;

	ENTER();

	MSG2("DSP Write : Buffer : %08x Count : %x\n", buffer, count);

	if(count == 0)
		return 0;

	if(down_interruptible(&audio_dev.dsp_write_sem))
		return -ERESTARTSYS;

	
again:
	
	length = count;

	if(audio_dev.state & AU_STATE_PLAYING){
		if(audio_dev.play_half_buf[0].ptr == FRAG_SIZE &&
			audio_dev.play_half_buf[1].ptr == FRAG_SIZE){
			if( file->f_flags & O_NONBLOCK) {
				retval = -EAGAIN;
				goto quit;
			}
			else{
				MSG("Write block ...\n");
				wait_event_interruptible(audio_dev.write_wait_queue,
									(audio_dev.state & AU_STATE_PLAYING) == 0 ||
									audio_dev.play_half_buf[0].ptr == 0 ||
									audio_dev.play_half_buf[1].ptr == 0);
			}
		}
	}


	retval = -EFAULT;


	for(i = 0; i < 2; i ++){
		tmp = FRAG_SIZE - audio_dev.play_half_buf[i].ptr;

		if(tmp > count)
			tmp = count;

		if(tmp){
			if(copy_from_user(dma_buf + audio_dev.play_half_buf[i].ptr + i * FRAG_SIZE, 
					buffer + length - count, tmp))
					goto quit;
		}

		count -= tmp;
		audio_dev.play_half_buf[i].ptr += tmp;
		if( count == 0)
			break;
	}

	retval = length - count;

	if((audio_dev.state & AU_STATE_PLAYING ) == 0
		&& audio_dev.play_half_buf[0].ptr == FRAG_SIZE 
		&& audio_dev.play_half_buf[1].ptr == FRAG_SIZE){

		if (wb_audio_start_play()){
			retval =  -EIO;
			goto quit;
		}
	}

	if(count != 0){
		if( file->f_flags & O_NONBLOCK 
			&& audio_dev.play_half_buf[0].ptr == FRAG_SIZE 
			&& audio_dev.play_half_buf[1].ptr == FRAG_SIZE)
			goto quit;
		
		buffer += retval;
		goto again;
	}

quit:

	up(&audio_dev.dsp_write_sem);

	DBG("W");
	
	LEAVE();

	return retval;
}

static int wb_dsp_ioctl(struct inode *inode, struct file *file,
			     unsigned int cmd, unsigned long arg)
{
	int val = 0, i, err = 0;
	audio_buf_info info;

	ENTER();
	
	switch (cmd) {

	       case OSS_GETVERSION:
       	       val = SOUND_VERSION;

			break;
				
	       case SNDCTL_DSP_GETCAPS:

			if (audio_dev.codec->get_capacity() & DSP_CAP_DUPLEX)
				val |= DSP_CAP_DUPLEX;
			
		   	val |= DSP_CAP_TRIGGER;

			break;

	       case SNDCTL_DSP_SPEED:
			if (get_user(val, (int*)arg))
				return -EFAULT;

			for(i = 0; i < sizeof(nSamplingRate)/sizeof(unsigned int); i++)
				if(val == nSamplingRate[i])
					break;

			if(i >= sizeof(nSamplingRate)/sizeof(unsigned int)){	/* not supported */
				val = audio_dev.nSamplingRate;
				err = -EPERM;
			}
			else
				audio_dev.nSamplingRate = val;
		
			break;


	       case SOUND_PCM_READ_RATE:
			val = audio_dev.nSamplingRate;
			break;

	       case SNDCTL_DSP_STEREO:
			if (get_user(val, (int*)arg))
				return -EFAULT;

			audio_dev.nChannels = val ? 2:1;
		
			break;	

	       case SNDCTL_DSP_CHANNELS:
			if (get_user(val, (int*)arg))
				return -EFAULT;

			if(val != 1 && val != 2){
				val = audio_dev.nChannels;
				err = -EPERM;
			}

			audio_dev.nChannels = val;
			break;

	       case SOUND_PCM_READ_CHANNELS:
			val = audio_dev.nChannels;
		
			break;

	       case SNDCTL_DSP_SETFMT:
		   	if (get_user(val, (int*)arg))
				return -EFAULT;

			if ( (val & (AFMT_S16_LE | AFMT_S16_BE | AFMT_U16_LE |AFMT_U16_BE)) == 0)
				err = -EPERM;
			
		case SNDCTL_DSP_GETFMTS:

			val =  (AFMT_S16_LE | AFMT_S16_BE | AFMT_U16_LE |AFMT_U16_BE);
			break;

	       case SOUND_PCM_READ_BITS:
			val = 16;
			break;

	       case SNDCTL_DSP_NONBLOCK:
       	       file->f_flags |= O_NONBLOCK;
			break;

	       case SNDCTL_DSP_RESET:
			wb_audio_stop_play();
			wb_audio_stop_record();	
			
			return 0;
		
	       case SNDCTL_DSP_GETBLKSIZE:
			val =FRAG_SIZE;
			break;
		
		case SNDCTL_DSP_GETISPACE:

			info.fragsize = FRAG_SIZE;
			info.fragstotal = 2;
			info.bytes = audio_dev.record_buf_length - audio_dev.record_half_buf[0].ptr - audio_dev.record_half_buf[1].ptr;
			info.fragments = info.bytes / info.fragsize;
		
			MSG2("SNDCTL_DSP_GETISPACE returns %d/%d/%d/%d\n",
				       info.fragsize, info.fragstotal,
				       info.bytes, info.fragments);
		
			if (copy_to_user((void *)arg, &info, sizeof(info)))
				return -EFAULT;
		
			return 0;

		case SNDCTL_DSP_GETOSPACE:

			info.fragsize = FRAG_SIZE;
			info.fragstotal = 2;
			info.bytes = audio_dev.play_buf_length - audio_dev.play_half_buf[0].ptr - audio_dev.play_half_buf[1].ptr;
			info.fragments = info.bytes / info.fragsize;
		
			MSG2("SNDCTL_DSP_GETISPACE returns %d/%d/%d/%d\n",
				       info.fragsize, info.fragstotal,
				       info.bytes, info.fragments);
		
			if (copy_to_user((void *)arg, &info, sizeof(info)))
				return -EFAULT;
		
			return 0;

	       case SNDCTL_DSP_SYNC:
			/* no data */
			if ( audio_dev.play_half_buf[0].ptr == 0 &&
				audio_dev.play_half_buf[1].ptr == 0)
				return 0;
			
		   	wb_audio_start_play();

			/* wait until stop playing */
			wait_event_interruptible(audio_dev.write_wait_queue,
									(audio_dev.state & AU_STATE_PLAYING)  == 0 ||
									(audio_dev.play_half_buf[0].ptr == 0 &&
									audio_dev.play_half_buf[1].ptr == 0));
			wb_audio_stop_play();
			
		   	return 0;

		case SNDCTL_DSP_GETTRIGGER:
			val = 0;
			if ( audio_dev.state & AU_STATE_PLAYING )
				val |= PCM_ENABLE_OUTPUT;
			if ( audio_dev.state & AU_STATE_RECORDING)
				val |= PCM_ENABLE_INPUT;
			break;

		case SNDCTL_DSP_SETTRIGGER:
			if (get_user(val, (int*)arg))
				return -EFAULT;
			if ( val & PCM_ENABLE_OUTPUT){
				if ( wb_audio_start_play()){
					val &= ~PCM_ENABLE_OUTPUT;
					err = -EPERM ;
				}
			}
			else{
				wb_audio_stop_play();
			}

			if ( val & PCM_ENABLE_INPUT){
				if ( wb_audio_start_record()){
					val &= ~PCM_ENABLE_INPUT;
					err = -EPERM ;
				}
			}
			else{
				wb_audio_stop_record();
			}

			break;
				
		default:
			return -EINVAL;
	}

	LEAVE();

	return err?-EPERM:put_user(val, (int *)arg);

}

static unsigned int wb_dsp_poll(struct file *file, struct poll_table_struct *wait)
{
	int mask = 0;

	ENTER();

	if (file->f_mode & FMODE_WRITE){
		poll_wait(file, &audio_dev.write_wait_queue, wait);
		
		/* check if has data */
		if(audio_dev.play_half_buf[0].ptr != FRAG_SIZE ||
			audio_dev.play_half_buf[1].ptr != FRAG_SIZE )
			mask |= POLLOUT | POLLWRNORM;
	}
			
	if (file->f_mode & FMODE_READ){
		poll_wait(file, &audio_dev.read_wait_queue, wait);

		/* check if can read */
		if(audio_dev.record_half_buf[0].ptr !=  FRAG_SIZE  ||
			audio_dev.record_half_buf[1].ptr !=  FRAG_SIZE )
			mask |= POLLIN | POLLRDNORM;
	}

	LEAVE();

	return mask;
}

static struct file_operations wb_dsp_fops = {
	owner:	THIS_MODULE,
	llseek:	no_llseek,
	read:	wb_dsp_read,
	write:	wb_dsp_write,
	poll:		wb_dsp_poll,
	ioctl:	wb_dsp_ioctl,
	open:	wb_dsp_open,
	release:	wb_dsp_release,
};

static int __init wb_dsp_init(void)
{
	int i;

	for(i=0;i<2;i++){
		dev_dsp[i] = register_sound_dsp(&wb_dsp_fops, i);
		MSG2("Dsp Device No : %d\n", dev_dsp[i]);
		if(dev_dsp[i]< 0)
			goto quit;
	}

	return 0;

quit:
	printk("Register DSP device failed\n");
	return -1;
}


static int __init wb_mixer_init(void)
{
	int i;

	for(i=0;i<2;i++){
		dev_mixer[i] = register_sound_mixer(&wb_mixer_fops, i);
		MSG2("Mixer Device No : %d\n", dev_mixer[i]);
		if(dev_mixer[i]< 0)
			goto quit;
	}

	return 0;

quit:
	printk("Register Mixer device failed\n");
	return -1;
}

static void wb_dsp_unload(void)
{
	int i;
	for(i=0;i<2;i++)
		unregister_sound_dsp(dev_dsp[i]);
}

static void wb_mixer_unload(void)
{
	int i;
	for(i=0;i<2;i++)
		unregister_sound_mixer(dev_mixer[i]);
}


MODULE_AUTHOR("QFu");
MODULE_DESCRIPTION("Winbond w90n745 Audio Driver");
MODULE_LICENSE("GPL");

static void wb_audio_exit (void)
{
	wb_mixer_unload();
	wb_dsp_unload();
	free_irq(INT_AC97, NULL);
}
extern struct semaphore ac97_sem; // move here to ensure it's initialized before accesign it.
static int __init wb_audio_init(void)
{

	Disable_Int(INT_AC97);

	memset(&audio_dev, 0, sizeof(audio_dev));
	sema_init(& audio_dev.dsp_read_sem, 1);
	sema_init(&audio_dev.dsp_write_sem, 1);
	sema_init(&audio_dev.mixer_sem, 1);
	sema_init(&ac97_sem, 1);

	if(wb_dsp_init())
		goto quit;

	if(wb_mixer_init())
		goto quit;

	printk("Winbond Audio Driver v1.0 Initialization successfully.\n");

	return 0;

quit:
	printk("Winbond Audio Driver Initialization failed\n");
	wb_audio_exit();
	return -1;
}

module_init(wb_audio_init);
module_exit(wb_audio_exit);

