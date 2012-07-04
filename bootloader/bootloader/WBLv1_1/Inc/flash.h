/******************************************************************************
 *
 * Copyright (c) 2003 Windond Electronics Corp.
 * All rights reserved.
 *
 * $Workfile: flash.h $
 *
 * $Author: Yachen $
 ******************************************************************************/
/*
 * $History: flash.h $
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
 * *****************  Version 6  *****************
 * User: Wschang0     Date: 04/03/19   Time: 5:07p
 * Updated in $/W90P710/FIRMWARE/WBLv1_1/Inc
 * 
 * *****************  Version 5  *****************
 * User: Wschang0     Date: 03/08/28   Time: 5:34p
 * Updated in $/W90P710/FIRMWARE/WBLv1_1/Inc
 * Add VSS header
 */ 
#ifndef FLASH_H
#define FLASH_H
//---------------------------------------------------------------------------
#include "cdefs.h"
 

// if no platform.h
#ifndef _PLATFORM_H

#define FLASH_BASE			(0x7F000000)
#define FLASH_BLOCK_SIZE	(0x10000)

#define VPint(x)	(*((volatile unsigned int*)(x)))
#define VPshort(x)	(*((volatile unsigned short*)(x)))
#define VPchar(x)	(*((volatile unsigned char*)(x)))
#define ROMCON	VPint(0xFFF01004)
#define EXT3CON VPint(0xFFF01024)
#define CAHCNF	VPint(0xFFF02000)	
#define CAHCON	VPint(0xFFF02004)


#define inph(x)		VPshort(x)
#define outph(x,y)	VPshort(x)=(y);
#define inpb(x)		VPchar(x)
#define outpb(x,y)	VPchar(x)=(y);

#endif



#define FLASH_NAME_SIZE	16

#define BLOCK_LOCK 0
#define BLOCK_UNLOCK 1


typedef struct
{
	char PID0;
	char PID1;
	char name[FLASH_NAME_SIZE];
	int (*BlockSize)(UINT32 address);
	int (*BlockErase)(UINT32 address,UINT32 size);
	int (*BlockWrite)(UINT32 address, UCHAR * data, UINT32 size);
	int (*ReadPID)(UINT32 address, UCHAR *PID0, UCHAR *PID1 );
	int (*BlockLock)(UINT32 address, UINT32 op);
} flash_t;


#define IMAGE_ACTIVE	0x1		// Only the active image will be processed by bootloader
#define IMAGE_COPY2RAM	0x2		// copy this image to ram
#define IMAGE_EXEC		0x4		// execute this image
#define IMAGE_FILE		0x8		// file image
#define IMAGE_COMPRESSED 0x10	// compressed image, bootloader will unzip it

#define SIGNATURE_WORD	0xA0FFFF9F

typedef struct t_footer
{
	UINT32 num;
	UINT32 base;
	UINT32 length;
	UINT32 load_address;
	UINT32 exec_address;
	CHAR name[16];
	UINT32 image_checksum;
	UINT32 signature;
	UINT32 type;
	UINT32 checksum;
} tfooter;


#define MAX_FOOTER_NUM	8
#define MAX_IMAGE_NUM MAX_FOOTER_NUM

typedef struct t_free
{
	UINT32 address;
	UINT32 length;
} tfree;

#define MAX_FREE_NUM 4

#define BOOTER_BLOCK_LENGTH	FLASH_BLOCK_SIZE
extern flash_t flash[];// The supported flash types
extern char * _flash_buffer;

extern UINT32 FlashSize(void); // return the flash size
extern INT FindFlash(void); // function to identify the flash type
extern INT FindFooter(tfooter *** image_footer); //function to find the image footers
extern INT FindImage(UINT32 image_num, tfooter ** image_footer); // function to find image according to image number
extern INT WriteImage(tfooter * image_footer, UINT32 image_source);//function to write image
extern INT CorruptCheck(tfooter * image_footer);//function to check if data corrupt
extern INT DelBlock(UINT32 block);// function to delete a block size of flash memory
extern INT DelImage(UINT32 image_num);// function to delete an image
extern INT WriteFile2Image(tfooter * image_footer, UINT32 buffer, INT file, INT (*read_func)(INT, VOID *, UINT32) );// flash write from file
extern INT ChangeImageType(INT image, INT type);
extern INT ImageCheck(INT image);

//---------------------------------------------------------------------------
#endif