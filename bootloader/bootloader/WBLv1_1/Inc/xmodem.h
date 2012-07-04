/******************************************************************************
 *
 * Copyright (c) 2003 Windond Electronics Corp.
 * All rights reserved.
 *
 * $Workfile: xmodem.h $
 *
 * $Author: Yachen $
 ******************************************************************************/
/*
 * $History: xmodem.h $
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
 */
 
#ifndef XMODEM_H
#define XMODEM_H
//------------------------------------------------------------------------------------------------------
//#define XMODEM1K
#define SOH		0x1
#define STX		0x2
#define EOT		0x4
#define ACK		0x6
#define NAK		0x15
#define CANCEL	0x18

#define XMODEM_TIMEOUT_VALUE	0xFFFFFFFF
#define X_SSUCCESS			0
#define X_SUSERCANCEL		-1
#define X_SNAK				-2
#define X_STIMEOUT			-3
#define X_SPACKET_NUM_ERROR	-4

extern int xmodem(unsigned int buffer,unsigned int *fileSize);

//------------------------------------------------------------------------------------------------------
#endif