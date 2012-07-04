/******************************************************************************
 *
 * Copyright (c) 2003 Windond Electronics Corp.
 * All rights reserved.
 *
 * $Workfile: bib.h $
 *
 * $Author: Yachen $
 ******************************************************************************/
/*
 * $History: bib.h $
 * 
 * *****************  Version 2  *****************
 * User: Yachen       Date: 06/01/19   Time: 2:18p
 * Updated in $/W90P710/Applications/710bootloader/WBLv1_1/Inc
 * Add USB support. User can chose between USB and MAC in shell.map
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
 * *****************  Version 4  *****************
 * User: Wschang0     Date: 04/03/19   Time: 5:07p
 * Updated in $/W90P710/FIRMWARE/WBLv1_1/Inc
 * 
 * *****************  Version 3  *****************
 * User: Wschang0     Date: 03/09/24   Time: 10:16a
 * Updated in $/W90P710/FIRMWARE/WBLv1_1/Inc
 * Add the baudrate member to tbl_info structure
 * 
 * *****************  Version 2  *****************
 * User: Wschang0     Date: 03/08/28   Time: 5:38p
 * Updated in $/W90P710/FIRMWARE/WBLv1_1/Inc
 * Add VSS header
 */
#ifndef _BIB_H
#define _BIB_H
#include "cdefs.h"
//---------------------------------------------------------------------------
typedef struct _t_bootloaer_info
{
	UINT32 length;
	UINT32 type;
	char mac0[6];
	char ip0[6];
	char mac1[6];
	char ip1[6];
	UINT32 cache;
	UINT32 dhcp;
	UINT32 net_mac;
	UINT32 phy;
	UINT32 buf_base;
	UINT32 buf_size;
	INT baudrate;
	INT rmii;
	UINT32 serial_no;
	UINT32	usb;
} tbl_info;

#define BOOTLOADER_INFO	0x1

extern int _dhcp;

//#define USE_NO_IMAGE0

//---------------------------------------------------------------------------
#endif