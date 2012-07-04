/******************************************************************************
 *
 * Copyright (c) 2003 Windond Electronics Corp.
 * All rights reserved.
 *
 * $Workfile: main.c $
 *
 * $Author: Yachen $
 ******************************************************************************/
/*
 * $History: main.c $
 * 
 * *****************  Version 1  *****************
 * User: Yachen       Date: 06/01/10   Time: 10:55a
 * Created in $/W90P710/Applications/710bootloader/Xmodem
 * 710 Bootloader, without USB support
 * 
 * *****************  Version 1  *****************
 * User: Yachen       Date: 06/01/04   Time: 2:28p
 * Created in $/W90P710/Module Test Programs/FIRMWARE_710/Xmodem
 * Module test bootloader, removed decompress function in order to save
 * memory space for LCD control
 * 
 * *****************  Version 6  *****************
 * User: Wschang0     Date: 04/03/19   Time: 5:10p
 * Updated in $/W90P710/FIRMWARE/Xmodem
 * 
 * *****************  Version 5  *****************
 * User: Wschang0     Date: 03/08/20   Time: 10:55a
 * Updated in $/W90P710/FIRMWARE/Xmodem
 * Update the program information,
 * ADD PrgInfo() funciton to show the program information
 * 
 * *****************  Version 4  *****************
 * User: Wschang0     Date: 03/08/20   Time: 10:49a
 * Updated in $/W90P710/FIRMWARE/Xmodem
 * Revision expansion test
 * 
 * *****************  Version 3  *****************
 * User: Wschang0     Date: 03/08/20   Time: 10:47a
 * Updated in $/W90P710/FIRMWARE/Xmodem
 * Revision expansion test
 * 
 * *****************  Version 2  *****************
 * User: Wschang0     Date: 03/08/20   Time: 10:42a
 * Updated in $/W90P710/FIRMWARE/Xmodem
 * Add VSS header
 */

#include "platform.h"
#include "xmodem.h"
#include "flash.h"
#include "serial.h"
#include "uprintf.h"
#include <string.h>

#define BANNER "W90P710 Xmodem Version " VERSION " " REVISION "\n"
#define VERSION	"1.0"
#define REVISION "$Revision: 1 $"
#define COPYRIGHT "Copyright (c) 2003 Windond Electronics Corp."


#define MAX_NUM 8


char * GetAddress(char *string);
unsigned int s2hex(char *s);
int validhex(char *s);

#define _BUFFER 0x100000
CHAR * _flash_buffer = (CHAR *)0x3D0000;

#pragma import(__use_no_semihosting_swi)	


void PrgInfo()
{
	uprintf("\n");
   	uprintf("                                           " __DATE__ "\n");
	uprintf("******************************************************\n\n");
	uprintf(BANNER);
	uprintf("\n");
	uprintf(COPYRIGHT"\n");
	uprintf("******************************************************\n");
	uprintf("\n");
}


int main()
{
	INT status;
	UINT32 srcAddress;
	UINT32 destAddress=FLASH_BASE;
	UINT32 fileSize;
	UINT32 blockSize,src,dest;
	UINT32 flash_type;
	UCHAR pid0,pid1;
	INT i;
	CHAR *p;
	
	uprintf("\n\n");
	
	/*Check UART LCR to see if the UART initialized */
	if(*((volatile unsigned int *)0xFFF8000C) == 0)
	{
		uprintf("Initialize Xmodem baud rate to 115200 bps ...\n");
		init_serial(0,ARM_BAUD_115200);
	}
	
	PrgInfo();
	uprintf("Flash Detecting ... \n");
	// Detect the flash type
	flash_type=-1;
	i=0;
	while( flash[i].PID0 | flash[i].PID1 )
	{
		pid0=pid1=0;
		flash[i].ReadPID(FLASH_BASE, &pid0, &pid1);
		//uprintf("PID Check [%d]: TYPE:%s ID:0x%02x 0x%02x\tFlash ID:0x%02x 0x%02x \n",i,flash[i].name,flash[i].PID0,flash[i].PID1,pid0, pid1);
		if( (flash[i].PID0 == pid0) && (flash[i].PID1 == pid1) )
		{
			flash_type=i;
			uprintf("Flash type is: %s\n",flash[i].name);
			break;		
		}
		i++;
	}
	if( flash_type==-1 )
	{
		uprintf("Unsupported flash type!!\n");
		return -1;
	}


	srcAddress=_BUFFER;

	uprintf("Waiting for download ... \n");
	uprintf("Press Ctrl-x to cancel. \n");
	fileSize=0;
	if( (status=xmodem(srcAddress,&fileSize))!=X_SSUCCESS )
	{

		if( status == X_SUSERCANCEL )
		{
			uprintf("\nCancel by user !\n");		
		}
		else
		{
			uprintf("\nDownload error !\n");
		}
		return -1;
	}
	else
	{
		uprintf("\nDownload successed !\n");		
	}

	/* Get the flash address written */
	uprintf("\n");
	p=GetAddress("Enter image load address (0x7F000000):");
	if( validhex( p ) )
		destAddress = s2hex(p);
	else
		uprintf("Image load address default to 0x%08x\n",destAddress);

	

	uprintf("Flash programming ");
	// Write program
	i=(fileSize&(~0x3))+0x4; //word aligment
	src=srcAddress;
	dest=destAddress;
//	flash[flash_type].BlockLock(dest, BLOCK_UNLOCK);// Block unlock!
	while(i)
	{
		uputchar('.');
		blockSize=flash[flash_type].BlockSize(dest);
		flash[flash_type].BlockErase(dest, blockSize);
		if( i < blockSize )blockSize=i; // Check if > a block size
		flash[flash_type].BlockWrite(dest, (UCHAR *)src, blockSize);
//		flash[flash_type].BlockLock(dest, BLOCK_LOCK);// Block lock!
		src+=blockSize;
		dest+=blockSize;
		i-=blockSize;
	}
	uprintf(" OK!\n");
	uprintf("Verifing ");
	// Verify program
	i=(fileSize&(~0x3))+0x4; //word aligment
	src=srcAddress;
	dest=destAddress;
	blockSize=flash[flash_type].BlockSize(srcAddress);
	blockSize=blockSize-1;
	while(i)
	{
		if( (i&blockSize)==0x0 )uputchar('.');
		if( inpw(src) != inpw(dest) )
		{
			uprintf("\nERROR: A:0x%08x W:0x%08x R:0x%08x!!\n",dest,inpw(src),inpw(dest));
			return -1;
		}
		src+=4;
		dest+=4;
		i-=4;
	}
	uprintf(" OK!\n");	
	uprintf("Programming finished!!\n");	


	return 0;	
}


char * GetAddress(char *string)
{
	static char str[16];
	int i;
	char ch;
	
	for(i=0;i<16;i++)str[i]=0;
	uprintf("%s ",string);
	i=0;
	while(1)
	{
		ch=ugetchar();
		if( ch == '\n' || ch == '\r' )
		{
			uputchar('\n');
			return str;
		}
		else
		{
			if( ch == '\b' )
			{
			  if(  i >= 1 )
			  {
				str[--i]='\0';
				uprintf("\r                                                             \r%s %s",string,str);
			  }	
			}
			else
			{
			
				if( i<15 )
				{
					str[i++]=ch;
					uputchar(ch);
				}
			}		
		}
	}
}


int chartohex(char c)
{
    if ((c >= '0') && (c <= '9'))
        return c - '0';
    if ((c >= 'a') && (c <= 'f'))
        return c - 'a' + 10;
    if ((c >= 'A') && (c <= 'F'))
        return c - 'A' + 10;
    return -1;
}

unsigned int s2hex(char *s)
{
    char nStack[MAX_NUM];
    int n;
    unsigned int result;
    int shift;

    // hop over the 0x start
    s += 2;

    // stack the number in reverse order
    n = 0;
    while ((*s != '\0') && (n < MAX_NUM))
    {
        nStack[n++] = *s++;
    }

    // Pull characters off the stack, converting to hex on the way
    result = 0;
    shift = 0;
    while (n > 0)
    {
        result += (chartohex(nStack[--n]) << shift);
        shift += 4;
    }

    return result;
}


int validhex(char *s)
{
    // Is this string a valid hex number?
    // For example, of the form 0xNNNNNNNN

    // valid length?
    if (strlen(s) > 10)
        return 0;
    if (strlen(s) <= 2)
        return 0;

    // valid start? (0x or 0X)
    if ((s[0] != '0') || ((s[1] != 'X') && (s[1] != 'x')))
        return 0;

    s += 2;

    // valid characters?
    while (*s != '\0')
    {
        if (chartohex(*s) < 0)
            // this character not valid
            return 0;

        // go onto the next character
        s++;
    }

    // success
    return 1;
}
