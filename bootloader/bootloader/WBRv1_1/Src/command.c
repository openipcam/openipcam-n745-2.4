/******************************************************************************
 *
 * Copyright (c) 2003 Windond Electronics Corp.
 * All rights reserved.
 *
 * $Workfile: command.c $
 *
 * $Author: Yachen $
 ******************************************************************************/
/*
 * $History: command.c $
 * 
 * *****************  Version 1  *****************
 * User: Yachen       Date: 06/01/10   Time: 10:55a
 * Created in $/W90P710/Applications/710bootloader/WBRv1_1/Src
 * 710 Bootloader, without USB support
 * 
 * *****************  Version 1  *****************
 * User: Yachen       Date: 06/01/04   Time: 2:28p
 * Created in $/W90P710/Module Test Programs/FIRMWARE_710/WBRv1_1/Src
 * Module test bootloader, removed decompress function in order to save
 * memory space for LCD control
 * 
 * *****************  Version 3  *****************
 * User: Wschang0     Date: 03/12/25   Time: 4:04p
 * Updated in $/W90P710/FIRMWARE/WBRv1_1/Src
 * add debug_wait function
 * 
 * *****************  Version 2  *****************
 * User: Wschang0     Date: 03/08/20   Time: 1:30p
 * Updated in $/W90P710/FIRMWARE/WBRv1_1/Src
 * Add VSS header
 */
#include "cdefs.h"
#include "sh.h"
#include "command.h"
#include "net.h"
#include "flash.h"
#include "uprintf.h"
#include "tftp.h"
#include "irq.h"
#include "xmodem.h"


#define PROMPT	"\rbootrom > "
CHAR shell_prompt[]=PROMPT;
extern int _net_init;

static int I_action(int argc, char *argv[]);
static int BURNT_action(int argc, char *argv[]);
static int BURNX_action(int argc, char *argv[]);


// Command table. Add now commands here if needed.
 NU_Command_t NU_Commands[] =
{
  /*{comman name, desciption, function name}*/
    {"H","Display the available commands\n", H_action},
    {"B","Set Baud Rate\n", B_action},
    {"D","Display memory. D -? for help\n", D_action},
    {"E","Edit memory. E -? for help\n", E_action},
    {"G","Goto address\n", G_action},
    {"I","information\n", I_action},
    {"BT","Burn the raw data into flash by TFTP\n", BURNT_action},
    {"BX","Burn the raw data into flash by Xmodem\n", BURNX_action},
    {"MX","Xmodem download\n", MX_action},
    {"MT","TFTP download\n", MT_action},
    {"FT","Program the flash by TFTP. FT -? for help\n", FT_action},
    {"FX","Program the flash by Xmodem. FX -? for help\n", FX_action},
    {"CP","Memory copy\n", CP_action},
    {"LS","List the images in the flash\n", LS_action},
    {"SET","Setting boot loader configuration. SET -? for help\n", SET_action},
    {"CHK","Check the flash\n", CHK_action},
    {"RUN","Execute image\n", RUN_action},
    {"DEL","DEL the image or flash block\n", DEL_action},
    {"MSET","Fill memory\n", MSET_action},
    {"TERM","Change the terminal output port\n", TERM_action},
    {"BOOT","Reboot the system\n", BOOT_action},
    {"CACHE","Cache setting\n", CACHE_action},
    {"UNZIP","Unzip image\n", UNZIP_action},

};

NU_CommandTable_t NU_CommandTable =
{
    NU_Commands,
    NoOfElements(NU_Commands),
    ERROR_action,
};

static int I_action(int argc, char *argv[])
{
	extern void WBL_info(void);
	WBL_info();
	return 0;
}

/*
 * The debug_wait was used to wait for the "KEY" to enter debug mode.
 * 
 */
void debug_wait()
{
	int i;
	char ch;
	// Waiting 3 sec to enter debug mode.
	uprintf("Press ESC to enter debug mode ");
	for( i=0;i < 6;i++)
	{
		sleep(500); // 0.5 sec
		if( ukbhit() )
		{
			ch=ugetchar();
			/* Enter the debug mode if the key "ESC" or "B" was pressed */
			if( (ch == 27) || (ch == 'B') )sh(0,0);
		}
		uputchar('.');
	}
	uputchar('\n');
}


static int BURNX_action(int argc, char *argv[])
{
	UINT i;
	UINT32 src_addr=0x100000;
	UINT32 dest_addr=FLASH_BASE;
	UINT32 src,dest;
	UINT32 fileSize=0;
	UINT32 blockSize=0;
	INT flash_type;


	uprintf("Waiting for download ... \n");
	uprintf("Press Ctrl-x to cancel ... \n");
	if( xmodem(src_addr,&fileSize)==X_SSUCCESS )
	{
		uprintf("\nFlash programming ");
		if( _net_init )DisableIRQ();
		flash_type=FindFlash();
		if( _net_init )EnableIRQ();
			if( flash_type < 0 )
		{
			uprintf("ERROR: Un-supported flash type !!\n");
			return -1;
		}
		// Write program
		if( (fileSize&0x3) )fileSize=((fileSize&(~0x3))+4);//word-aligment
		i=fileSize; 
		src=src_addr;
		dest=dest_addr;
		while(i)
		{
			blockSize=flash[flash_type].BlockSize(dest);

			if( _net_init )DisableIRQ();
			flash[flash_type].BlockErase(dest, blockSize);
			if( i < blockSize )
			{
				flash[flash_type].BlockWrite(dest, (UCHAR *)src, i);
				blockSize=i;
			}
			else
			{
				flash[flash_type].BlockWrite(dest, (UCHAR *)src, blockSize);
			}
			if( _net_init )EnableIRQ();

			src+=blockSize;
			dest+=blockSize;
			i-=blockSize;
			uprintf(".");
		}
		uprintf(" OK!\n");

	}
	else
	{
		uprintf("\nDownload error!\n");
		return -1;
	}

	// verify data
	uprintf("Write data verifing ");
	for(i=0;i<fileSize;i+=4)
	{
		if( (i&0xFFFF)== 0x0 )uprintf(".");
		if( *((volatile unsigned int *)(src_addr+i))!=*((volatile unsigned int *)(dest_addr+i)) )
		{
			uprintf("ERROR: Data failed @ 0x%08x \n", dest_addr+i);
			return -1;
		}
	}
	uprintf(" OK!\n");

	return 0;


	return 0;
}


static int BURNT_action(int argc, char *argv[])
{
	UINT i;
	UINT32 src_addr=0x100000;
	UINT32 dest_addr=FLASH_BASE;
	UINT32 src,dest;
	UINT32 fileSize=0;
	UINT32 blockSize=0;
	INT flash_type;



	if( !_net_init )
	{
		if( Net_Init(_dhcp) < 0 )
		{
			uprintf("ERROR: Network initialization failed!\n");
			return -1;
		}
		_net_init=1;
	}

	uprintf("Waiting for download ...\n");
	if( TFTP_Download((UCHAR *)src_addr,(ULONG *)&fileSize,_dhcp)==0 )
	{
		uprintf("\nFlash programming ");
		if( _net_init )DisableIRQ();
		flash_type=FindFlash();
		if( _net_init )EnableIRQ();
		if( flash_type < 0 )
		{
			uprintf("ERROR: Un-supported flash type !!\n");
			return -1;
		}
		// Write program
		if( (fileSize&0x3) )fileSize=((fileSize&(~0x3))+4);//word-aligment
		i=fileSize; 
		src=src_addr;
		dest=dest_addr;
		while(i)
		{
			blockSize=flash[flash_type].BlockSize(dest);
			if( _net_init )DisableIRQ();
			flash[flash_type].BlockErase(dest, blockSize);
			if( i < blockSize )
			{
				flash[flash_type].BlockWrite(dest, (UCHAR *)src, i);
				blockSize=i;
			}
			else
			{
				flash[flash_type].BlockWrite(dest, (UCHAR *)src, blockSize);
			}
			if( _net_init )EnableIRQ();
			src+=blockSize;
			dest+=blockSize;
			i-=blockSize;
			uprintf(".");
		}
		uprintf(" OK!\n");

	}
	else
	{
		uprintf("\nDownload error!\n");
		return -1;
	}

	// verify data
	uprintf("Write data verifing ");
	for(i=0;i<fileSize;i+=4)
	{
		if( (i&0xFFFF)== 0x0 )uprintf(".");
		if( *((volatile unsigned int *)(src_addr+i))!=*((volatile unsigned int *)(dest_addr+i)) )
		{
			uprintf("ERROR: Data failed @ 0x%08x \n", dest_addr+i);
			return -1;
		}
	}
	uprintf(" OK!\n");


	return 0;
}
