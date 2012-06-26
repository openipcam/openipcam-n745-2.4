/****************************************************************************
 * 
 * FILENAME 	: Audio_test.c
 *
 * VERSION 	: 1.0 
 *
 * DESCRIPTION :
 *	This program is test for w90n745 audio driver
 *
 * DATA STRUCTURES
 *     None
 *
 * HISTORY
 *	 2005.11.23		Created by NM23 QFu
 *
 **************************************************************************/
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/soundcard.h>
#include <sys/poll.h>

#define BUFFER_SIZE	(1024 * 128)

char buffer[BUFFER_SIZE];
int block_size = 4096;

int choice;
char *dsp_device[] = {"/dev/dsp0", "/dev/dsp1"};
char *mixer_device[] = {"/dev/mixer0", "/dev/mixer1"};
char *interface_type[] ={"IIS", "AC97"};


int ioctl_test(int dsp, int mixer)
{
	int data;
	mixer_info info;
	char tmpBuf[40];

	printf("***IOCTL Test ***\n");

	ioctl(dsp, OSS_GETVERSION, &data);
	if(data != SOUND_VERSION) {
		printf("Get OSS_GETVERSION error, %d\n", data);
		return -1;
	}

	ioctl(mixer, OSS_GETVERSION, &data);
	if(data != SOUND_VERSION) {
		printf("Get OSS_GETVERSION error, %d\n", data);
		return -1;
	}

	ioctl(dsp, SNDCTL_DSP_GETCAPS, &data);

	/* set volume to 100 */
	data = 0x6464;
	ioctl(mixer, MIXER_WRITE(SOUND_MIXER_VOLUME), &data);

	ioctl(mixer, SOUND_MIXER_INFO, &info);
	memset(tmpBuf, 0, sizeof(tmpBuf));
	strncpy(tmpBuf, info.name, 32);
	printf("Device Information : %s\n", tmpBuf);

	/* get internel data block size */
	ioctl(dsp, SNDCTL_DSP_GETBLKSIZE, &data);
	printf("Internal Fragment Size : %d\n", data);
	block_size = data;


	return 0;
}

int play_test(int dsp)
{
	const char *pcmfiles[] = {
		"8k.pcm",
		"11.025k.pcm",
		"16k.pcm",
		"22.05k.pcm",
		"24k.pcm",
		"32k.pcm",
		"44.1k.pcm",
		"48k.pcm"};
	const int samplerate[] = {
		8000,
		11025,
		16000,
		22050,
		24000,
		32000,
		44100,
		48000};
	int i, fd, status, ret = -1, channel = 2;
		

	printf("***Play and SampleRate Test***\n");

	for(i=0; i<8; i++){

		printf("SampleRate %d Stereo Test ...  \n", samplerate[i]);
		fd = open(pcmfiles[i], O_RDONLY);
		if ( fd < 0){
			printf("SKIP ( Open file %s error )\n", pcmfiles[i]);
			continue;
		}

		ret = 0;

		ioctl(dsp, SNDCTL_DSP_SPEED, &samplerate[i]);
		ioctl(dsp, SNDCTL_DSP_CHANNELS, &channel);

		while(read(fd, buffer, block_size))
			write(dsp, buffer, block_size);

		close(fd);

		/* wait the last block play off */

		ioctl(dsp, SNDCTL_DSP_SYNC);
		
		ioctl(dsp, SNDCTL_DSP_RESET);

	}

	return ret;

}

int record_test(int dsp, int mixer)
{
	int i, status = 0;
	int samplerate = 8000, channel = 1, volume = 0x6464;

	printf("***Record Test***\n");
	printf("Press ENTER to Begin Record for 6s ...\n");
	getchar();
	printf("\nStart recording ...\n");
	ioctl(mixer, MIXER_WRITE(SOUND_MIXER_MIC), &volume);	/* set MIC max volume */
	ioctl(dsp, SNDCTL_DSP_SPEED, &samplerate);
	ioctl(dsp, SNDCTL_DSP_CHANNELS, &channel);

	for ( i = 0 ; i < ( BUFFER_SIZE / block_size); i++){
		read(dsp, buffer + i * block_size, block_size);	/* recording */
	}

	ioctl(dsp, SNDCTL_DSP_GETTRIGGER, &status);	/* get record and play status */
	status &= (~PCM_ENABLE_INPUT);
	ioctl(dsp, SNDCTL_DSP_SETTRIGGER, &status);
	

	printf("Record Over, Now press ENTER to play ...\n");
	getchar();
	printf("\nStart Playing...\n");

	for( i = 0; i < (BUFFER_SIZE / block_size); i++) {
		write(dsp, buffer + i * block_size, block_size);
	}

	/* wait the last block play off */
	ioctl(dsp, SNDCTL_DSP_SYNC);

	return 0;
	
		
}

int poll_test(int dsp)
{
	int fd, success = 0, failed = 0, status, i = 0;
	int samplerate = 8000, channel = 2;
	struct pollfd pfd;

	printf("***None block mode Test***\n");
	printf("Playing 8k.pcm in none block mode ...\n");

	ioctl(dsp, SNDCTL_DSP_SPEED, &samplerate);
	ioctl(dsp, SNDCTL_DSP_CHANNELS, &channel);
	ioctl(dsp, SNDCTL_DSP_NONBLOCK);

	fd = open("8k.pcm", O_RDONLY);
	if ( fd < 0) {
		printf("Open 44.1k.pcm error\n");
		return -1;
	}

	do{
		if ( read(fd, buffer, block_size) <= 0)
			break;

		while( write(dsp, buffer, block_size) < 0 && failed >= 0)
			failed ++;
		success++;
	}while(1);

	/* wait the last block play off */
	ioctl(dsp, SNDCTL_DSP_SYNC);

	printf("Request Success : %d , Waiting Request : %d\n",
		success, failed);

	if ( failed ==success )
		return -1;

	printf("***Poll mode test***\n");
	printf("Playing 8k.pcm in poll mode ...\n");
	lseek(fd, 0, SEEK_SET);
	
	pfd.fd = dsp;
	pfd.events = POLLOUT;
	success = 0; failed = 0;
	
	while(read(fd, buffer, block_size)){
		while ( poll(&pfd, 1, 0) <= 0) failed++; 
		if ( pfd.revents & POLLOUT){
			write(dsp, buffer, block_size);
			success ++;
		}
		else
			failed ++;
	}

	close(fd);

	/* wait the last block play off */
	ioctl(dsp, SNDCTL_DSP_SYNC);


	printf("Request Success : %d , Waiting Request : %d\n",
		success, failed);

	if ( failed ==success )
		return -1;

	return 0;

}

int mixer_test(int dsp, int mixer)
{
	int fd, samplerate = 8000, channel = 2;
	int status, volume[] = {0x2121, 0x4242, 0x6464}, i;

	printf("***Mixer Volume test***\n");

	fd = open("8k.pcm", O_RDONLY);
	if ( fd < 0 ) {
		close(mixer);
		printf("Open 44.1k.pcm error\n");
		return -1;
	}

	ioctl(dsp, SNDCTL_DSP_SPEED, &samplerate);
	ioctl(dsp, SNDCTL_DSP_CHANNELS, &channel);

	for(i = 0; i < 3; i++){
		printf("Set Volume to %d%% and press ENTER to play ...\n",
				volume[i]&0xff);
		ioctl(mixer, MIXER_WRITE(SOUND_MIXER_VOLUME), &volume[i]);
		getchar();

		while(read(fd, buffer, block_size))
			write(dsp, buffer, block_size);

		/* wait the last block play off */
		ioctl(dsp, SNDCTL_DSP_SYNC);

		
		lseek(fd, 0, SEEK_SET);
	}

	close(fd);
	close(mixer);

	return 0;	
	
}

int main(int argc, char *argv[])
{
	int dsp, mixer, retval = 0, i;

	printf("Winbond W90N745 Audio Module Test Code, written by QFu\n");

	printf("\nSelect Device : \n");
	for(i = 0; i< 2; i++){
		printf("%d. %s and %s (%s)\n", i,
			dsp_device[i], mixer_device[i],
			interface_type[i]);
	}
	printf("\nEnter your choice:");
	scanf("%d", &choice);
	getchar();

	if(choice <0 || choice >1)
		return 0;

	dsp = open(dsp_device[choice], O_RDWR);
	if( dsp < 0 ){
		printf("Open device %s error\n", dsp_device[choice]);
		return -1;
	}

	mixer = open(mixer_device[choice], O_RDWR);
	if ( mixer < 0){
		printf("Open %s error\n", mixer_device[choice]);
		return -1;
	}

	retval = ioctl_test(dsp, mixer);
	printf("***IOCTRL Test ... %s\n", retval?"Failed":"Pass");
	if (retval) goto quit;

	retval = 	play_test(dsp);
	printf("***Play And SampleRate Test ... %s\n", retval?"Failed":"Pass");
	if (retval) goto quit;

	retval = mixer_test(dsp, mixer);
	printf("***Mixer Volume  Test ... %s\n", retval?"Failed":"Pass");
	if (retval) goto quit;

	retval = record_test(dsp, mixer);
	printf("***Record Test .. %s\n", retval?"Failed":"Pass");
	if ( retval)goto quit;

	retval = poll_test(dsp);
	printf("***None Block Mode & Poll Mode Test ... %s\n", retval?"Failed":"Pass");
	if (retval) goto quit;

quit:

	close(dsp);

	return retval;

}

