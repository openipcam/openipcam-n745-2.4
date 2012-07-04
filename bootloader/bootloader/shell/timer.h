/******************************************************************************
 *
 * Copyright (c) 2003 Windond Electronics Corp.
 * All rights reserved.
 *
 * $Workfile: timer.h $
 *
 * $Author: Yachen $
 ******************************************************************************/
/*
 * $History: timer.h $
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
 * *****************  Version 2  *****************
 * User: Wschang0     Date: 03/09/24   Time: 7:46p
 * Updated in $/W90P710/FIRMWARE/shell
 * Add VSS header
 */

#ifndef _TIMER_H
#define _TIMER_H
//-----------------------------------------------------------------
extern void init_timer(void);
extern unsigned int clock(void);
extern void sleep(int n);



//-----------------------------------------------------------------
#endif