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
 * *****************  Version 4  *****************
 * User: Yachen       Date: 06/08/16   Time: 5:49p
 * Updated in $/W90P710/Applications/710bootloader/WBLv1_1/Src
 * 1. Add command INTF
 * 2. USB -on is no more required for MT & FT even USB is disabled.
 * bootloader wil disable USB after transmit complete.
 * 
 * *****************  Version 3  *****************
 * User: Yachen       Date: 06/02/15   Time: 11:08a
 * Updated in $/W90P710/Applications/710bootloader/WBLv1_1/Src
 * Add run-time USB enable/disable function
 * 
 * *****************  Version 2  *****************
 * User: Yachen       Date: 06/01/19   Time: 2:18p
 * Updated in $/W90P710/Applications/710bootloader/WBLv1_1/Src
 * Add USB support. User can chose between USB and MAC in shell.map
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
 * *****************  Version 5  *****************
 * User: Wschang0     Date: 04/03/19   Time: 4:48p
 * Updated in $/W90P710/FIRMWARE/WBLv1_1/Src
 * Add message to show flash types
 * 
 * *****************  Version 4  *****************
 * User: Wschang0     Date: 03/10/20   Time: 6:16p
 * Updated in $/W90P710/FIRMWARE/WBLv1_1/Src
 * Add debug_wait() function. This function will be called by main
 * function to wait the user key to enter debug mode.
 * 
 * *****************  Version 3  *****************
 * User: Wschang0     Date: 03/08/27   Time: 11:30a
 * Updated in $/W90P710/FIRMWARE/WBLv1_1/Src
 * Add ATTRIB_action command
 * 
 * *****************  Version 2  *****************
 * User: Wschang0     Date: 03/08/20   Time: 1:27p
 * Updated in $/W90P710/FIRMWARE/WBLv1_1/Src
 * Add VSS header
 */
#include "cdefs.h"
#include "sh.h"
#include "command.h"
#include "uprintf.h"
#include "flash.h"


#define PROMPT	"\rbootloader > "
CHAR shell_prompt[]={PROMPT};
extern void sleep(int ms);
static int I_action(int argc, char *argv[]);


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
    {"MX","Xmodem download\n", MX_action},
    {"MT","TFTP/USB download\n", MT_action},
    {"FT","Program the flash by TFTP/USB. FT -? for help\n", FT_action},
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
    {"USB", "USB interface setting\n", USB_action},
    {"UNZIP","Unzip image\n", UNZIP_action},
    {"ATTRIB","Change the image attribution\n", ATTRIB_action},
    {"INTF", "Print bootloader supported interface, ether USB or MAC", INTF_action}
    
};

NU_CommandTable_t NU_CommandTable =
{
    NU_Commands,
    NoOfElements(NU_Commands),
    ERROR_action,
};

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

static int I_action(int argc, char *argv[])
{
	INT i;
	extern void WBL_info(void);
	
	WBL_info();

	/* Show the flash types supported */
	uprintf("Supports flash types:");
	for( i=0;flash[i].PID0;i++)
	{
		if( (i&3)==0 ) uprintf("\n");
		uprintf("%-16s ", flash[i].name);
	}

	return 0;
}

