/****************************************************************************
 *                                                                                                                           *
 * Copyright (c) 2004 - 2006 Winbond Electronics Corp. All rights reserved.                          *
 *                                                                                                                           *
 ***************************************************************************/
 
/****************************************************************************
 * 
 * FILENAME
 *     kpdtest.c
 *
 * VERSION
 *     1.0
 *
 * DESCRIPTION
 *     This file is a sample test program used to test keypad
 *
 * DATA STRUCTURES
 *     None
 *
 * FUNCTIONS
 *     None
 *
 * HISTORY
 *     2005/08/30		 Ver 1.0 Created by PC34 YHan
 *
 * REMARK
 *     None
 **************************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>
#include <signal.h>

typedef struct _keymap 
{
	short row;
	short col;
	
}keymap;

int fd;

void mySignalHandler (int sig)
{
      close(fd);
      printf("Keypad test over!\n");
      
      exit(0);
}




int main(int argc, char* argv[])
{
	
	int count;
	keymap buf;
	
	signal (SIGTERM, mySignalHandler); /* for the TERM signal.. */
  signal (SIGINT, mySignalHandler); /* for the CTRL+C signal.. */
  
	fd = open("/dev/keypad", O_RDWR);
	if(fd == -1)
		printf("can not open the dev\n");
	
	while(1){
		count=read(fd,&buf,sizeof(keymap));//count equal to how many key has been pressed at on time
		if(count>0)
			printf("row:%d colum:%d\n",buf.row,buf.col);		
	}
	
	return 0;
}
