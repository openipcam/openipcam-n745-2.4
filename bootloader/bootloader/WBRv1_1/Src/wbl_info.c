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
 * User: Wschang0     Date: 03/09/05   Time: 1:08p
 * Updated in $/W90P710/FIRMWARE/WBRv1_1/Src
 * Add boot information to wbl_info
 * 
 * *****************  Version 2  *****************
 * User: Wschang0     Date: 03/08/20   Time: 1:30p
 * Updated in $/W90P710/FIRMWARE/WBRv1_1/Src
 * Add VSS header
 */
#include "uprintf.h"
#include "flash.h"

extern unsigned int memory_size;
#define WBR_VERSION_STRING	"1.1"
#define REVISION " $Revision: 1 $"
#define WBR_BANNER "W90P710 boot ROM [ Version " WBR_VERSION_STRING REVISION " ] Rebuilt on " __DATE__ "\n"

void WBL_info()
{

	uprintf("\n\n");
    uprintf(WBR_BANNER);
    uprintf("Running on a %s Evaluation Board\n", "W90P710");
    uprintf("Board Revision %s, %s Processor\n", "1.0", "ARM7TDMI");
    uprintf("Memory Size is 0x%x Bytes, Flash Size is 0x%x Bytes\n",memory_size, FlashSize());
    uprintf("Board designed by %s\n", "Winbond");
    uprintf("Hardware support provided at %s\n", "Winbond");
    uprintf("Copyright (c) Winbond Limited 2001 - 2003. All rights reserved.\n");
    uprintf("\n\nFor help on the available commands type 'h'\n\n");



}


