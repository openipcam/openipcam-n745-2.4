/******************************************************************************
 *
 * Copyright (c) 2003 Windond Electronics Corp.
 * All rights reserved.
 *
 * $Workfile: wbl_info.c $
 *
 * $Author: Yachen $
 ******************************************************************************/
/*
 * $History: wbl_info.c $
 * 
 * *****************  Version 1  *****************
 * User: Yachen       Date: 06/01/10   Time: 10:55a
 * Created in $/W90P710/Applications/710bootloader/WBLv1_1/Src
 * 710 Bootloader, without USB support
 * 
 * *****************  Version 1  *****************
 * User: Yachen       Date: 06/01/04   Time: 2:28p
 * Created in $/W90P710/Module Test Programs/FIRMWARE_710/WBLv1_1/Src
 * Module test bootloader, removed decompress function in order to save
 * memory space for LCD control
 * 
 * *****************  Version 9  *****************
 * User: Wschang0     Date: 04/06/11   Time: 9:38a
 * Updated in $/W90P710/FIRMWARE/WBLv1_1/Src
 * Add 4Mx32 (16MB) SDRAM support,
 * Add Intel 28F128,28F640 flash types
 * 
 * *****************  Version 8  *****************
 * User: Wschang0     Date: 04/03/19   Time: 4:51p
 * Updated in $/W90P710/FIRMWARE/WBLv1_1/Src
 * Add message to show flash types
 * Fix ROMCON bug
 * Add new flash types supports
 * Fix memory size detect function
 * Tune xmodem waiting time to fit the linux
 * Arrange the flash array to faster detection
 * 
 * *****************  Version 7  *****************
 * User: Wschang0     Date: 03/11/06   Time: 5:14p
 * Updated in $/W90P710/FIRMWARE/WBLv1_1/Src
 * revision 7 release
 * 
 * *****************  Version 6  *****************
 * User: Wschang0     Date: 03/09/26   Time: 2:43p
 * Updated in $/W90P710/FIRMWARE/WBLv1_1/Src
 * update revision
 * 
 * *****************  Version 5  *****************
 * User: Wschang0     Date: 03/09/04   Time: 5:39p
 * Updated in $/W90P710/FIRMWARE/WBLv1_1/Src
 * Add BIB_ShowInfo to wbl_info()
 * 
 * *****************  Version 4  *****************
 * User: Wschang0     Date: 03/08/26   Time: 9:31a
 * Updated in $/W90P710/FIRMWARE/WBLv1_1/Src
 * ADD $Revision: 1 $ to boot information
 * 
 * *****************  Version 3  *****************
 * User: Wschang0     Date: 03/08/20   Time: 1:28p
 * Updated in $/W90P710/FIRMWARE/WBLv1_1/Src
 * Add VSS header
 * 
 * *****************  Version 2  *****************
 * User: Wschang0     Date: 03/08/20   Time: 1:27p
 * Updated in $/W90P710/FIRMWARE/WBLv1_1/Src
 * Add VSS header
; */
#include "uprintf.h"
#include "flash.h"
#include "bib.h"

// BIB info
extern 	void BIB_ShowInfo(tbl_info *);
 
extern unsigned int memory_size;
#define WBL_VERSION_STRING	"1.1"
#define REVISION " $Revision: 1 $" 
#define WBL_BANNER "W90P710 Boot Loader [ Version " WBL_VERSION_STRING REVISION " ] Rebuilt on " __DATE__ "\n"

void WBL_info()
{
	tfooter * footer=NULL;
	tbl_info * info;

		uprintf("\n\n");
    uprintf(WBL_BANNER);
   // uprintf("Running on a %s Module Test Board\n", "W90P710");
   // uprintf("Board Revision %s, %s MCU\n", "1.0", "W90P710");
    uprintf("Memory Size is 0x%x Bytes, Flash Size is 0x%x Bytes\n",memory_size, FlashSize());
    uprintf("Board designed by %s\n", "Winbond");
    uprintf("Hardware support provided at %s\n", "Winbond");
    uprintf("Copyright (c) Winbond Limited 2001 - 2006. All rights reserved.\n");

	/* Show the boot information */
	if( FindImage(0, &footer) )
	{
		info=(tbl_info *)(footer->base);
		BIB_ShowInfo(info);
	}


    
    uprintf("\n\nFor help on the available commands type 'h'\n\n");
	
}


