/******************************************************************************
 *
 * Copyright (c) 2003 Windond Electronics Corp.
 * All rights reserved.
 *
 * $Workfile: cdefs.h $
 *
 * $Author: Yachen $
 ******************************************************************************/
/*
 * $History: cdefs.h $
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
#ifndef CDEFS_H
#define CDEFS_H
//------------------------------------------------------------------------------
typedef int INT;
typedef int SIGNED;
typedef unsigned int UNSIGNED;
typedef unsigned int UINT;
typedef unsigned int UINT32;
typedef unsigned short UINT16;
typedef unsigned char UCHAR;
typedef char CHAR;
typedef int INT;
typedef void VOID;
typedef unsigned long ULONG;
#define NULL	0

//------------------------------------------------------------------------------
#endif
