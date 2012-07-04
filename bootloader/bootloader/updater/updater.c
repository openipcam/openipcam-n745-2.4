/******************************************************************************
 *
 * Copyright (c) 2003 Windond Electronics Corp.
 * All rights reserved.
 *
 * $Workfile: updater.c $
 *
 * $Author: Yachen $
 ******************************************************************************/
/*
 * $History: updater.c $
 * 
 * *****************  Version 1  *****************
 * User: Yachen       Date: 06/01/10   Time: 10:55a
 * Created in $/W90P710/Applications/710bootloader/updater
 * 710 Bootloader, without USB support
 * 
 * *****************  Version 1  *****************
 * User: Yachen       Date: 06/01/04   Time: 2:27p
 * Created in $/W90P710/Module Test Programs/FIRMWARE_710/updater
 * Module test bootloader, removed decompress function in order to save
 * memory space for LCD control
 * 
 * *****************  Version 6  *****************
 * User: Wschang0     Date: 04/03/19   Time: 5:08p
 * Updated in $/W90P710/FIRMWARE/updater
 * Remove the bootROM update
 * 
 * *****************  Version 5  *****************
 * User: Wschang0     Date: 04/03/03   Time: 1:44p
 * Updated in $/W90P710/FIRMWARE/updater
 * remove bootrom image
 * add bootrom update check
 * 
 * *****************  Version 4  *****************
 * User: Wschang0     Date: 03/08/27   Time: 11:32a
 * Updated in $/W90P710/FIRMWARE/updater
 * Show the VSS Revision Info
 * 
 * *****************  Version 3  *****************
 * User: Wschang0     Date: 03/08/26   Time: 9:39a
 * Updated in $/W90P710/FIRMWARE/updater
 * ADD $Revision: 1 $ information
 * 
 * *****************  Version 2  *****************
 * User: Wschang0     Date: 03/08/20   Time: 1:33p
 * Updated in $/W90P710/FIRMWARE/updater
 * Add VSS header
 */
#include "platform.h"
#include "flash.h"
#include "serial.h"
#include "uprintf.h"

#ifdef SEMIHOSTED
# define uprintf printf
# define ugetchar getchar
#endif

//#define DEBUG

#define BANNER "W90P710 Updater Version " VERSION " " REVISION "\n"
#define VERSION	"1.0"
#define REVISION "$Revision: 1 $"
#define COPYRIGHT "Copyright (c) 2003 Windond Electronics Corp."

extern void * bootloader_entry;
extern void * bootloader_tail;
extern void * bootrom_entry;
extern void * bootrom_tail;
int flash_raw_write(void * src, void * dest, int size);
int cal_checksum(void *src, int size);

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
	int size;
	void * bin_entry, * bin_tail;
#ifdef DEBUG
	uprintf("Checksum: 0x%08x\n",cal_checksum((void *)0x8000,16));
#endif
	
	
	init_serial(0, ARM_BAUD_115200);
	PrgInfo();
	uprintf("The ROM code version in this update image : \n");
	uprintf("\tW90P710 Boot Loader - Version 1.1\n");
	if( ((int)&bootrom_entry != (int)&bootrom_entry) )
		uprintf("\tW90P710 Boot ROM    - Version 1.1\n");
	uprintf("\n");
	// check if boot rom environment
	if( ((ROMCON&0xFF000000) == 0xFC000000) && ((int)&bootrom_entry != (int)&bootrom_entry) )
	{
		// check if the flash in EXTIO3 ? (double check )
		if( (EXT3CON&0xFF000000) == 0xFE000000 )
		{
			uprintf("W90P710 platform is Found\n");
			uprintf("WARNING: Re-configure the External I/O 3 to avoid conflict.\n");
			EXT3CON=0x0;
			uprintf("WARNING: Re-configure the ROMCON to update ROM code.\n");
			ROMCON=ROMCON+0x2000000UL;
			uprintf("WARNING: W90P710 Boot ROM will be updated!\n");
			bin_entry=&bootrom_entry;
			bin_tail=&bootrom_tail;
		}
		else
		{
			uprintf("ERROR: Unknown board configuraion!\n");
			return -1;
		}
		
	}
	else // Singel flash environment
	{
		// check if really the single flash environment (double check)
		if( (ROMCON&0xFF000000) == 0xFE000000 )
		{
			uprintf("W90P710 board is Found\n");
			bin_entry=&bootloader_entry;
			bin_tail=&bootloader_tail;
			uprintf("WARNING: W90P710 Boot Loader will be updated!\n");
		}
		else
		{
			uprintf("ERROR: Unknown board configuration!\n");
			return -1;
		}
	}
	
	uprintf("\n -- Press any key to continue -- \n");
	if( ugetchar()=='b' ) // key char given, 
	{
		uprintf("Key character detected! Press key 'r' to update WBR, 'l' to update WBL\n");
		switch(ugetchar())
		{
			case 'r':
				uprintf("WARNING: Force to update W90P710 Boot ROM!!\n");
				bin_entry=&bootrom_entry;
				bin_tail=&bootrom_tail;
				break;
			case 'l':
				uprintf("WARNING: Force to update W90P710 Boot Loader!!\n");
				bin_entry=&bootloader_entry;
				bin_tail=&bootloader_tail;
				break;
			default:
				break;
		}
	}	
	
	size=(int)((unsigned int)bin_tail-(unsigned int)bin_entry);	
	if( flash_raw_write(bin_entry, (void *)0x7F000000, size) == 0 )
	{
		uprintf("ROM code update successed!\n");
	}
	else
	{
		uprintf("ROM code update failed!\n");
		return -1;
	}

	uprintf("\n -- Press any key to reboot -- \n");
	ugetchar();
	CLKSEL=CLKSEL|0x1;
	
	while(1); /* Avoid semihosted exit caused error */
	return 0;
}



int flash_raw_write(void * srcAddress, void * destAddress, int size)
{
	int fileSize;
	unsigned int src,dest;
	unsigned char pid0,pid1;
	int i,blockSize;
	int flash_type;
	
	uprintf("\n");
	uprintf("Flash Detecting ... \n");
	// Detect the flash type
	flash_type=-1;
	i=0;
	while( flash[i].PID0 | flash[i].PID1 )
	{
		pid0=pid1=0;
		flash[i].ReadPID(FLASH_BASE, &pid0, &pid1);
#ifdef DEBUG
		uprintf("PID Check [%d]: TYPE:%s ID:0x%02x 0x%02x\tFlash ID:0x%02x 0x%02x \n",i,flash[i].name,flash[i].PID0,flash[i].PID1,pid0, pid1);
#endif		
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

	fileSize=size;
	// Write program
	uprintf("Flash programming ");
	i=(fileSize&(~0x3))+0x4; //word aligment
	src=(unsigned int)srcAddress;
	dest=(unsigned int)destAddress;
	while(i)
	{
		uputchar('.');
		blockSize=flash[flash_type].BlockSize(dest);
		flash[flash_type].BlockErase(dest, blockSize);
		if( i < blockSize )blockSize=i; // Check if > a block size
		flash[flash_type].BlockWrite(dest, (UCHAR *)src, blockSize);
		src+=blockSize;
		dest+=blockSize;
		i-=blockSize;
	}
	uprintf(" OK!\n");
	uprintf("Verifing ");
	// Verify program
	i=(fileSize&(~0x3))+0x4; //word aligment
	src=(unsigned int)srcAddress;
	dest=(unsigned int)destAddress;
	blockSize=flash[flash_type].BlockSize((unsigned int)srcAddress);
	blockSize=blockSize-1;
	while(i)
	{
		if( (i&blockSize)==0x0 )uputchar('.');
		if( *((volatile unsigned int *)src) != *((volatile unsigned int *)dest) )
		{
			uprintf("\nERROR: A:0x%08x W:0x%08x R:0x%08x!!\n",dest,*((volatile unsigned int *)src),*((volatile unsigned int*)dest));
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

int cal_checksum(void *src, int size)
{
	int result;
	long long sum;
	unsigned int * p, *e;
	
	
	sum = 0UL;
	e=(unsigned int *)((int)p+size);
	for( p=(unsigned int *)src;p<e;p++)
	{
		sum+=*p;
	}
	
#ifdef DEBUG
	uprintf("MSB: 0x%08x\n",(int)(sum>>32));
	uprintf("LSB: 0x%08x\n",(int)sum);
#endif
	
	result = (int)((unsigned int)sum+(unsigned int)(sum>>32));
	return result;
}
