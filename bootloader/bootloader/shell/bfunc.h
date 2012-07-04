/******************************************************************************
 *
 * Copyright (c) 2003 Windond Electronics Corp.
 * All rights reserved.
 *
 * $Workfile: bfunc.h $
 *
 * $Author: Yachen $
 ******************************************************************************/
/*
 * $History: bfunc.h $
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

#ifndef BOOT_FUNC_H
#define BOOT_FUNC_H
//-------------------------------------------------------------------------------

#define MAX_BIB_NUMBER	8

#define STATIC_BIB_BUFFER	0x200000
#define STATIC_BFB_BUFFER	(STATIC_BIB_BUFFER+0x2000)

#define SYS_INFO_BLOCK	0x0
#define FUN_INFO_BLOCK	0x1
#define USER_INFO_BLOCK 0x2


#define BOOTINFO_BLOCK_BASE 0x7F004000
#define BOOTINFO_BLOCK_SIZE 0x2000

#define BOOTFUNC_BLOCK_BASE 0x7F008000
#define BOOTFUNC_BLOCK_SIZE 0x8000

#define FUN_ZIP			0x1
#define FUN_COMPRESSED 	0x2

// the structure must be word alignment
typedef struct t_fun_info_block
{
	UINT32 length;
	UINT32 type;
	CHAR name[8];
	UINT32 base;
	UINT32 func_length;
	UINT32 exec_address;
	UINT32 func_type;
	UINT32 checksum;

}tBIB;

//-------------------------------------------------------------------------------
#endif