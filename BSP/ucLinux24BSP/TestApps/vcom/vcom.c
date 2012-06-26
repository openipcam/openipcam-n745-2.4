/*-----------------------------------------------------------------------------------*/
/* Winbond Electronics Corporation confidential                                      */
/*                                                                                   */
/* Copyright (c) 2007-2008 by Winbond Electronics Corporation                        */
/* All rights reserved                                                               */
/*                                                                                   */
/*-----------------------------------------------------------------------------------*/


#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <syscall.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>  
#include <stddef.h>
#include <unistd.h>


/* USB ioctl commands */
#define WBUSB_IOC_MAGIC 'u'
#define WBUSB_GETVLEN	 _IOR(WBUSB_IOC_MAGIC, 1, unsigned long *)
#define WBUSB_REPLUG	 _IOR(WBUSB_IOC_MAGIC, 2, char *)

int usb_fd;

int init_system(void)
{

  /* open vcom */
	usb_fd = open("/dev/usbclient", O_RDWR);
	if (usb_fd < 0) 
	{
		printf("Can't open VCOM!\n");
		return -1;
	}

	return 0;
}



int main(int argc,char *argv[])
{
	int ret, vlen;
	char c[] = "abcdefg";
	
	printf("\n\n VCOM Demo with UART Start\n");
	
	/* initialize system */
	ret = init_system();
    if (ret < 0)
    	return -1;
 
	while(1)
	{
#if 0		
		/* VCOM RX */
		ioctl(usb_fd, WBUSB_GETVLEN, &vlen);
		if (vlen > 0)
		{
			vcom.nVCOMCnt = read(usb_fd, vcom.cVCOMBuff, vlen);	
			VDEBUG("vcom rx[%d]\n", vcom.nVCOMCnt);
		}
#endif		
		ret = write(usb_fd, &c[0], strlen(c));
		sleep(1);

	}
	
	return 0;	
}
