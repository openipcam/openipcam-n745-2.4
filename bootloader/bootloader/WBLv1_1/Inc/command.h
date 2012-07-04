/******************************************************************************
 *
 * Copyright (c) 2003 Windond Electronics Corp.
 * All rights reserved.
 *
 * $Workfile: command.h $
 *
 * $Author: Yachen $
 ******************************************************************************/
/*
 * $History: command.h $
 * 
 * *****************  Version 3  *****************
 * User: Yachen       Date: 06/08/16   Time: 5:49p
 * Updated in $/W90P710/Applications/710bootloader/WBLv1_1/Inc
 * 1. Add command INTF
 * 2. USB -on is no more required for MT & FT even USB is disabled.
 * bootloader wil disable USB after transmit complete.
 * 
 * *****************  Version 2  *****************
 * User: Yachen       Date: 06/02/15   Time: 11:08a
 * Updated in $/W90P710/Applications/710bootloader/WBLv1_1/Inc
 * Add run-time USB enable/disable function
 * 
 * *****************  Version 1  *****************
 * User: Yachen       Date: 06/01/10   Time: 10:55a
 * Created in $/W90P710/Applications/710bootloader/WBLv1_1/Inc
 * 710 Bootloader, without USB support
 * 
 * *****************  Version 1  *****************
 * User: Yachen       Date: 06/01/04   Time: 2:28p
 * Created in $/W90P710/Module Test Programs/FIRMWARE_710/WBLv1_1/Inc
 * Module test bootloader, removed decompress function in order to save
 * memory space for LCD control
 * 
 * *****************  Version 2  *****************
 * User: Wschang0     Date: 03/08/28   Time: 5:38p
 * Updated in $/W90P710/FIRMWARE/WBLv1_1/Inc
 * Add VSS header
 */
#ifndef COMMAND_H
#define COMMAND_H
//----------------------------------------------------------------------------------------
extern int H_action(int argc, char *argv[]);
extern int D_action(int argc, char *argv[]);
extern int E_action(int argc, char *argv[]);
extern int MX_action(int argc, char *argv[]);
extern int MT_action(int argc, char *argv[]);
extern int G_action(int argc, char *argv[]);
extern int B_action(int argc, char *argv[]);
extern int FT_action(int argc, char *argv[]);
extern int FX_action(int argc, char *argv[]);
extern int CP_action(int argc, char *argv[]);
extern int LS_action(int argc, char *argv[]);
extern int CHK_action(int argc, char *argv[]);
extern int SET_action(int argc, char *argv[]);
extern int RUN_action(int argc, char *argv[]);
extern int DEL_action(int argc, char *argv[]);
extern int MSET_action(int argc, char *argv[]);
extern int TERM_action(int argc, char *argv[]);
extern int CACHE_action(int argc, char *argv[]);
extern int UNZIP_action(int argc, char *argv[]);
extern int BOOT_action(int argc, char *argv[]);
extern int ATTRIB_action(int argc, char *argv[]);
extern int USB_action(int argc, char *argv[]);
extern int INTF_action(int argc, char *argv[]);


extern void ERROR_action(void);

//----------------------------------------------------------------------------------------
#endif