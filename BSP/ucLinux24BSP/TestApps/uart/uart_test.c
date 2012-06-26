/****************************************************************************
 *                                                                          *
 * Copyright (c) 2004 - 2006 Winbond Electronics Corp. All rights reserved. *
 *                                                                          *
 ****************************************************************************/
 
/****************************************************************************
 * 
 * FILENAME
 *     uart_test.c
 *
 * VERSION
 *     1.0
 *
 * DESCRIPTION
 *     This is the test program used to test the UARTs on POS-TAX Board
 *
 * DATA STRUCTURES
 *     None
 *
 * FUNCTIONS
 *     None
 *
 * HISTORY
 *     09/05/2005		 Ver 1.0 Created by PC34 MCLi
 *
 * REMARK
 *     None
 ****************************************************************************/
#include     <stdio.h>
#include     <stdlib.h>
#include     <unistd.h> 
#include     <sys/types.h> 
#include     <sys/stat.h> 
#include     <fcntl.h> 
#include     <termios.h>  
#include     <errno.h>

#define FALSE 0
#define TRUE  1
struct termios old_options;

/***@brief  set serial com baudrate
*@param  fd     type int  the file handle of the opened serial port
*@param  speed  type int  the serial port speed
*@return  void*/

int speed_arr[] = { B38400, B19200, B9600, B4800, B2400, B1200, B300,
	    B38400, B19200, B9600, B4800, B2400, B1200, B300,B115200 };
int name_arr[] = {38400,  19200,  9600,  4800,  2400,  1200,  300,
	    38400,  19200,  9600, 4800, 2400, 1200,  300,115200 };
void set_speed(int fd, int speed)
{
  int   i;
  int   status;
  struct termios   Opt;
  tcgetattr(fd, &Opt);
  
  for ( i= 0;  i < sizeof(speed_arr) / sizeof(int);  i++)
   {
   	if  (speed == name_arr[i])
   	{
   	    tcflush(fd, TCIOFLUSH);
    	cfsetispeed(&Opt, speed_arr[i]);
    	cfsetospeed(&Opt, speed_arr[i]);
    	status = tcsetattr(fd, TCSANOW, &Opt);
    	if  (status != 0)
            perror("tcsetattr fd1");
     	return;
     	}
   tcflush(fd,TCIOFLUSH);
   }
}
/**
*@brief   Set data bit,stop bit and parity bit
*@param  fd     type  int  the file handle of the opened serial port
*@param  databits type  int data bits    value: 7 or 8
*@param  stopbits type  int stop bit     value: 1 or 2
*@param  parity  type  int  check bit value:N,E,O,,S
*/
int set_Parity(int fd,int databits,int stopbits,int parity)
{
	struct termios options;
 if  ( tcgetattr( fd,&options)  !=  0)
  {
  	perror("SetupSerial 1");
  	return(FALSE);
  }
  options.c_cflag &= ~CSIZE;
  switch (databits) /*Set the bits of the data*/
  {
  	case 7:
  		options.c_cflag |= CS7;
  		break;
  	case 8:
		options.c_cflag |= CS8;
		break;
	default:
		fprintf(stderr,"Unsupported data size\n");
		return (FALSE);
	}
  switch (parity)
  	{
  	case 'n':
	case 'N':
		options.c_cflag &= ~PARENB;   /* Clear parity enable */
		options.c_iflag &= ~INPCK;     /* Enable parity checking */
		break;
	case 'o':
	case 'O':
		options.c_cflag |= (PARODD | PARENB);  /* Set the parity bit*/ 
		options.c_iflag |= INPCK;             /* Disnable parity checking */
		break;
	case 'e':
	case 'E':
		options.c_cflag |= PARENB;     /* Enable parity */
		options.c_cflag &= ~PARODD;   /* change to EVEN parity*/  
		options.c_iflag |= INPCK;       /* Disnable parity checking */
		break;
	case 'S':
	case 's':  /*as no parity*/
		options.c_cflag &= ~PARENB;
		options.c_cflag &= ~CSTOPB;
		break;
	default:
		fprintf(stderr,"Unsupported parity\n");
		return (FALSE);
		}
  /* Set stop bit*/   
  switch (stopbits)
  	{
  	case 1:
  		options.c_cflag &= ~CSTOPB;
		break;
	case 2:
		options.c_cflag |= CSTOPB;
		break;
	default:
		fprintf(stderr,"Unsupported stop bits\n");
		return (FALSE);
	}
	
	options.c_cflag &= ~CRTSCTS;	
  /* Set input parity option */
  if (parity != 'n')
  		options.c_iflag |= INPCK;
    options.c_cc[VTIME] = 150; // 15 seconds
    options.c_cc[VMIN] = 0;

  options.c_lflag  &= ~(ICANON | ECHO) ;
  
  tcflush(fd,TCIFLUSH); /* Update the options and do it NOW */
  if (tcsetattr(fd,TCSANOW,&options) != 0)
  	{
  		perror("SetupSerial 3");
		return (FALSE);
	}
  return (TRUE);
 }
/**
*@breif Open the serial port
*/
int OpenDev(char *Dev)
{
int	fd = open( Dev, O_RDWR | O_NONBLOCK);         //| O_NOCTTY | O_NDELAY
	if (-1 == fd)
		{ /*Set the Data bits*/
			perror("Can't Open Serial Port");
			return -1;
		}
	else
	return fd;

}
/**
*@breif 	main()
*/
int main(int argc, char **argv)
{
	int fd;
	int nwrite,nread;
	char buff[512],temp[16];
	char buff3[3]  = {"aaa"};
	char *dev ="/dev/ttyS0",*dev1 ="/dev/ttyS1",*dev2 ="/dev/ttyS2",*dev3 ="/dev/ttyS3";
	
	
	memset(buff,'A',512);
	
	if(argc>=2)
	{
		if(*argv[1]=='1')
		{
			fd = OpenDev(dev1);
			tcgetattr( fd,&old_options);
			if (fd>0)
		    	set_speed(fd,115200);
			else
			{
				printf("Can't Open Serial Port 1!\n");
				exit(0);
			}
		  if (set_Parity(fd,8,1,'N')== FALSE)
		  {
		    printf("Set Parity Error\n");
		    exit(1);
		  }
		
		  nwrite = write(fd,buff,512);
		  
		  printf("\nSent %d Bytes\n\n",nwrite);
		  tcsetattr(fd,TCSANOW,&old_options);
		  sleep(1);  // make sure all bytes are sent out. or we'll have to wait for 30 seconds.
		  close(fd);
		}
		else if(*argv[1]=='2')
		{
			fd = OpenDev(dev2);
			tcgetattr( fd,&old_options);
			if (fd>0)
		    	set_speed(fd,115200);
			else
			{
				printf("Can't Open Serial Port 2!\n");
				exit(0);
			}
		  if (set_Parity(fd,8,1,'N')== FALSE)
		  {
		    printf("Set Parity Error\n");
		    exit(1);
		  }
		
		  nwrite = write(fd,buff,512);
		  
		  printf("\nSent %d Bytes\n\n",nwrite);
		  tcsetattr(fd,TCSANOW,&old_options);	
		  sleep(1);  // make sure all bytes are sent out. or we'll have to wait for 30 seconds.	  
		  close(fd);
		}
		else if(*argv[1]=='3')
		{
			fd = OpenDev(dev3);
			tcgetattr( fd,&old_options);
			if (fd>0)
		    	set_speed(fd,115200);
			else
			{
				printf("Can't Open Serial Port 3!\n");
				exit(0);
			}
		  if (set_Parity(fd,8,1,'N')== FALSE)
		  {
		    printf("Set Parity Error\n");
		    exit(1);
		  }
		
		  nwrite = write(fd,buff,512);
		  
		  printf("\nSent %d Bytes\n\n",nwrite);
		  tcsetattr(fd,TCSANOW,&old_options);
		  sleep(1);  // make sure all bytes are sent out. or we'll have to wait for 30 seconds.
		  close(fd);
		}
		else
		{
			fd = OpenDev(dev);
			tcgetattr( fd,&old_options);
			if (fd>0)
		    	set_speed(fd,115200);
			else
			{
				printf("Can't Open Serial Port 0!\n");
				exit(0);
			}
		  if (set_Parity(fd,8,1,'N')== FALSE)
		  {
		    printf("Set Parity Error\n");
		    exit(1);
		  }
		
		  nwrite = write(fd,buff,512);
		  
		  printf("\nSent %d Bytes\n\n",nwrite);
		  tcsetattr(fd,TCSANOW,&old_options);
		  sleep(1);  // make sure all bytes are sent out. or we'll have to wait for 30 seconds.
		  close(fd);
		}		
	}
	else
	{
		fd = OpenDev(dev);
		tcgetattr( fd,&old_options);
		if (fd>0)
	    	set_speed(fd,115200);
		else
		{
			printf("Can't Open Serial Port 0!\n");
			exit(0);
		}
	  if (set_Parity(fd,8,1,'N')== FALSE)
	  {
	    printf("Set Parity Error\n");
	    exit(1);
	  }
	
	  nwrite = write(fd,buff,512);
	  
	  printf("\nSent %d Bytes\n\n",nwrite);
	  tcsetattr(fd,TCSANOW,&old_options);
	  sleep(1);  // make sure all bytes are sent out. or we'll have to wait for 30 seconds.
	  close(fd);
	}
		   
  return 0;
}
