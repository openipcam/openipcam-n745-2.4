/******************************************************************************
 *
 * Copyright (c) 2003 Windond Electronics Corp.
 * All rights reserved.
 *
 * $Workfile: bib.c $
 *
 * $Author: Yachen $
 ******************************************************************************/
/*
 * $History: bib.c $
 * 
 * *****************  Version 3  *****************
 * User: Yachen       Date: 06/01/20   Time: 4:18p
 * Updated in $/W90P710/Applications/710bootloader/shell
 * Fix USB status display bug
 * 
 * *****************  Version 2  *****************
 * User: Yachen       Date: 06/01/19   Time: 2:18p
 * Updated in $/W90P710/Applications/710bootloader/shell
 * Add USB support. User can chose between USB and MAC in shell.map
 * 
 * *****************  Version 1  *****************
 * User: Yachen       Date: 06/01/10   Time: 10:55a
 * Created in $/W90P710/Applications/710bootloader/shell
 * 710 Bootloader, without USB support
 * 
 * *****************  Version 1  *****************
 * User: Yachen       Date: 06/01/04   Time: 2:27p
 * Created in $/W90P710/Module Test Programs/FIRMWARE_710/shell
 * Module test bootloader, removed decompress function in order to save
 * memory space for LCD control
 * 
 * *****************  Version 7  *****************
 * User: Wschang0     Date: 04/12/03   Time: 9:27a
 * Updated in $/W90P710/FIRMWARE/shell
 * Change the serial number display into Hex format
 * 
 * *****************  Version 6  *****************
 * User: Wschang0     Date: 04/11/22   Time: 3:18p
 * Updated in $/W90P710/FIRMWARE/shell
 * Add serial number item
 * 
 * *****************  Version 5  *****************
 * User: Wschang0     Date: 04/07/02   Time: 2:34p
 * Updated in $/W90P710/FIRMWARE/shell
 * 
 * *****************  Version 4  *****************
 * User: Wschang0     Date: 04/07/02   Time: 2:31p
 * Updated in $/W90P710/FIRMWARE/shell
 * Show the current baud rate setting
 * 
 * *****************  Version 3  *****************
 * User: Wschang0     Date: 03/11/05   Time: 11:04a
 * Updated in $/W90P710/FIRMWARE/shell
 * Add RMII Option
 * 
 * *****************  Version 2  *****************
 * User: Wschang0     Date: 03/08/20   Time: 1:07p
 * Updated in $/W90P710/FIRMWARE/shell
 * Add VSS header
 */
#include "platform.h"
#include "bib.h"
#include "flash.h"
#include "net.h"
#include "uprintf.h"

#ifdef USE_NO_IMAGE0
INT WriteBIB(tbl_info * info, CHAR * buf)
{
	UINT32 blockSize;
	INT i,flash_type;

	flash_type=FindFlash();	

	// check the block size at the end of first 64KB
	blockSize=flash[flash_type].BlockSize(FLASH_BASE+BOOTER_BLOCK_LENGTH-sizeof(tbl_info));
	// backup the block. Using given buffer to do this
	memcpy(buf, FLASH_BASE+BOOTER_BLOCK_LENGTH-blockSize, blockSize-sizeof(tbl_info));
	// update the boot information into buffer
	memcpy(buf+blockSize-sizeof(tbl_info),(CHAR *)info, sizeof(tbl_info));
	// write the boot information, it must not be interrupted!!!!
	// Erase the block first
	flash[flash_type].BlockErase(FLASH_BASE+FLASH_BLOCK_SIZE-blockSize, blockSize);
	// write back the buffer
	flash[flash_type].BlockWrite(FLASH_BASE+FLASH_BLOCK_SIZE-blockSize,(UCHAR *) buf, blockSize);

	return 0;
}
#endif

void BIB_ShowInfo(tbl_info * info)
{
	if( info->type == BOOTLOADER_INFO )
	{
		uprintf("Boot Loader Configuration:\n\n");
//		uprintf("\t%-20s: MAC %d\n","TFTP server port",info->net_mac);
#if 0
		uprintf("\t%-20s: ","Network phy chip");
		switch( info->phy )
		{
			case BL_PHY:
				uprintf("DAVICOM DM9161E");
				break;
			case BL_IC_PLUS:
				uprintf("IC PLUS IP175A");
				break;
			case BL_MARVELL6052:
				uprintf("MARVELL 88E6052");
				break;
			default:
				break;			
		}
#endif		
//		uprintf("\n");
		uprintf("\t%-20s: %02x:%02x:%02x:%02x:%02x:%02x\n","MAC Address",info->mac0[0],info->mac0[1],info->mac0[2],info->mac0[3],info->mac0[4],info->mac0[5]);
		uprintf("\t%-20s: %d.%d.%d.%d\n","IP Address",info->ip0[0],info->ip0[1],info->ip0[2],info->ip0[3]);
		//uprintf("\t%-20s: %02x:%02x:%02x:%02x:%02x:%02x\n","MAC 1 Address",info->mac1[0],info->mac1[1],info->mac1[2],info->mac1[3],info->mac1[4],info->mac1[5]);
		//uprintf("\t%-20s: %d.%d.%d.%d\n","IP 1 Address",info->ip1[0],info->ip1[1],info->ip1[2],info->ip1[3]);
		if( info->dhcp )
			uprintf("\t%-20s: Enabled\n","DHCP Client");	
		else
			uprintf("\t%-20s: Disabled\n","DHCP Client");	
		if( info->cache )
			uprintf("\t%-20s: Enabled\n","CACHE");	
		else
			uprintf("\t%-20s: Disabled\n","CACHE");	
		uprintf("\t%-20s: 0x%08x\n","BL buffer base",info->buf_base);
		uprintf("\t%-20s: 0x%08x\n","BL buffer size",info->buf_size);
		uprintf("\t%-20s: %d\n","Baud Rate",info->baudrate);
		uprintf("\t%-20s: %s\n","USB Interface",((info->usb == 1)? "Enabled":"Disabled"));		
		//uprintf("\t%-20s: %s\n","RMII interface",((info->rmii==1)?"Enabled":"Disabled"));
		uprintf("\t%-20s: 0x%08x\n","Serial Number",info->serial_no);
		/*
		uprintf("\tEBICON =0x%08x \t ROMCON =0x%08x\n",info->ebicon,info->romcon);
		uprintf("\tSDCONF0=0x%08x \t SDCONF1=0x%08x\n",info->sdconf0,info->sdconf1);
		uprintf("\tSDTIME0=0x%08x \t SDTIME1=0x%08x\n",info->sdtime0,info->sdtime1);
		*/
	}
	else
	{
		uprintf("ERROR: Unknown data\n");
		return;		
	}
}

