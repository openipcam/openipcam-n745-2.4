/******************************************************************************
 *
 * Copyright (c) 2007 Windond Electronics Corp.
 * All rights reserved.
 *
 * $Workfile: cfi.c $
 *
 * 
 ******************************************************************************/

//#include <stdio.h>
//#include <string.h>
//#include <stdlib.h>
//#include "wblib.h"
#include <asm/arch/flash.h>
#include <linux/types.h>
#include <linux/slab.h>

#include "cfi.h"
//#include "platform.h"

//#define FLASH_BASE 0xFF000000
#define MAX_REGION_NUM	10

static unsigned short cfiCmdSet = 0;
static unsigned short cfiExtTab = 0;
static unsigned int cfiDevSize = 0;
static unsigned short cfiNumEraseBlkRegion = 0;

static struct cfi_erase_block_region_info cfiEraseBlkRegionInfo[MAX_REGION_NUM];
static int dummyErase(unsigned int address, unsigned int size);
static int dummyWrite(unsigned int address, unsigned char *data, unsigned int size);

struct cfi_command cfiCmd = {
	write: dummyWrite,
	erase: dummyErase,	
};


/* Retrun 0: OK, otherwise: CFI not found */
static int cfiCheckIdent(void)
{
	unsigned short i[3];
	
	CFI_READ(FLASH_BASE, 0x10, i[0]);
	CFI_READ(FLASH_BASE, 0x11, i[1]);
	CFI_READ(FLASH_BASE, 0x12, i[2]);
	
	if(i[0] == 'Q' && i[1] == 'R' && i[2] == 'Y')
		return(0);
	else
		return(-1);	

}

unsigned int cfiGetFlashSize(void)
{
	return(cfiDevSize);
}	

unsigned int cfiGetBlockSize(unsigned int address)
{
	int i, offset = 0;
	
	address = (address | 0x80000000) - (FLASH_BASE | 0x80000000);
	//printk("addr %x %x\n", address, FLASH_BASE);
	
	for(i = 0; i < cfiNumEraseBlkRegion; i++) {
		if(address < (offset += (cfiEraseBlkRegionInfo[i].num * cfiEraseBlkRegionInfo[i].size)))
			return(cfiEraseBlkRegionInfo[i].size);	
	}
	//uprintf("get size error\n");
	
	return(0);

}

static void flushDCache(void)
{

	if( *(unsigned int volatile *)(0xFFF02000) & 0x6 ) /* If write buffer or data cache is enabled */
	{
		 *(unsigned int volatile *)(0xFFF02004) = 0x86;
		while( *(unsigned int volatile *)(0xFFF02004));
	}
	
}


static int polling16(unsigned int addr, unsigned short data)
{
	unsigned short rdata;
	int timeout = 0x600000;
	
	rdata = *(unsigned short volatile *)(addr);
	while( rdata != data )
	{
		rdata = *(unsigned short volatile *)(addr);
		if( !(timeout--) )
		{	
			rdata = *(unsigned short volatile *)(addr);
			if( rdata != data )
			{
				//printf("timeout\n");
				return -1;	
			}
		}
	}

	return 0;
}

static int intelWrite(unsigned int address, unsigned char *data, unsigned int size)
{
	unsigned int i;
	int status = 0;
	
	address |= 0x80000000;
	for(i = address; i < address + size; i += 2)
	{
		CFI_WRITE(i, 0, 0x40);
		CFI_WRITE(i, 0, *(unsigned short *)data);
		CFI_WRITE(address, 0, 0x70);
		while(!( *(unsigned short *)(address) & 0x80));	
		if(*(unsigned short *)(address) & 0x19) {
			status = -1;
			goto exit;
		}	
		data+=2;
	}

exit:
	CFI_WRITE(address,0,0xFF);	
	flushDCache();	
	return(status);
}

static int dummyWrite(unsigned int address, unsigned char *data, unsigned int size)
{
	printk("Dummy Write\n");
	return(-1);	
}	
static int amdWrite(unsigned int address, unsigned char *data, unsigned int size)
{
	int status;
	unsigned int i;
	
	address |= 0x80000000;
	for(i = address; i < address+size; i += 2)
	{
		CFI_WRITE(FLASH_BASE, 0x555, 0xAA);
		CFI_WRITE(FLASH_BASE, 0x2AA, 0x55);
		CFI_WRITE(FLASH_BASE, 0x555, 0xA0);
		CFI_WRITE(i, 0, *(unsigned short *)data);
		status = polling16( i, *(unsigned short *)data);
		if(status < 0) {
			printk("write failed, time out!\n");
			return(status); // time-out	
		}	
		data+=2;
	}	
		
	flushDCache();
	return(0);

}

static int intelErase(unsigned int address, unsigned int size)
{
	int status = 0;

	address|=0x80000000;
	
	CFI_WRITE(address, 0, 0x50); // Clear sttaus register
	CFI_WRITE(address, 0, 0x20);
	CFI_WRITE(address, 0, 0xD0);
	
	CFI_WRITE(address, 0, 0x70);
	while(!( *(unsigned short *)(address) & 0x80));
	if(*(unsigned short *)(address) & 0x39)
		status = -1;

	CFI_WRITE(address, 0, 0xFF);
	flushDCache();	
	return(status);
}

static int dummyErase(unsigned int address, unsigned int size)
{
	printk("Dummy Erase\n");
	return(-1);	
}	
static int amdErase(unsigned int address, unsigned int size)
{
	int status;
	
	//printk("erase addr: %x\n", address);
	
	if((address & (size - 1)) != 0x0)
		return -1;// not in the start of a block
	address |= 0x80000000;
	//printf("erase addr: %08x\n", address);
	CFI_WRITE(FLASH_BASE, 0x555, 0xAA);
	CFI_WRITE(FLASH_BASE, 0x2AA, 0x55);
	CFI_WRITE(FLASH_BASE, 0x555, 0x80);
	CFI_WRITE(FLASH_BASE, 0x555, 0xAA);
	CFI_WRITE(FLASH_BASE, 0x2AA, 0x55);
	CFI_WRITE(address, 0, 0x30);
	status = polling16(address, 0xFFFF);
	CFI_WRITE(FLASH_BASE, 0, 0xFF);
	flushDCache();
	if(status < 0)
		printk("erase failed, time out!\n");
	return(status);

}

int cfiGetFlashInfo(void)
{

	unsigned int i;

	// goes into query mode
	DESELECT_QUERY_MODE(FLASH_BASE);
	SELECT_QUERY_MODE(FLASH_BASE);

	if(cfiCheckIdent() != 0) {
		printk("No CFI information found\n");
		goto exit;	
	}
		
	CFI_READ(FLASH_BASE, 0x13, cfiCmdSet);
	CFI_READ(FLASH_BASE, 0x15, cfiExtTab);
	CFI_READ(FLASH_BASE, 0x27, cfiDevSize);
	cfiDevSize = 1 << cfiDevSize;
		
	CFI_READ(FLASH_BASE, 0x2C, cfiNumEraseBlkRegion);	
	//cfiEraseBlkRegionInfo = (struct cfi_erase_block_region_info *)malloc(sizeof(struct cfi_erase_block_region_info) * cfiNumEraseBlkRegion);
	
	if(cfiNumEraseBlkRegion > MAX_REGION_NUM) {
		//sysprintf("Out of memory\n");
		goto exit;	
	}
	
	for(i = 0; i < cfiNumEraseBlkRegion * 4; i += 4) {
		unsigned short s1, s2;
		CFI_READ(FLASH_BASE, 0x2D + i, s1);
		CFI_READ(FLASH_BASE, 0x2E + i, s2);
		
		cfiEraseBlkRegionInfo[i/4].num = s1 + (s2 << 8) + 1;
		//printk("num %d ", (cfiEraseBlkRegionInfo + i/4)->num);
		
		CFI_READ(FLASH_BASE, 0x2F + i, s1);
		CFI_READ(FLASH_BASE, 0x30 + i, s2);	
		
		cfiEraseBlkRegionInfo[i/4].size = (s1 + (s2 << 8)) * 256;
		//printk("size %x \n", (cfiEraseBlkRegionInfo + i/4)->size);
	
	}
	
	DESELECT_QUERY_MODE(FLASH_BASE);
	if(cfiCmdSet == AMD_CMD_SET) {
		cfiCmd.write = amdWrite;
		cfiCmd.erase = amdErase;
	
	}else  if(cfiCmdSet == INTEL_CMD_SET) {
		cfiCmd.write = intelWrite;
		cfiCmd.erase = intelErase;	
	
	} else {
		printk("CFI command set %04x not support!\n", cfiCmdSet);
		return(-1);
	}
	printk("CFI command set %04x will be used\n", cfiCmdSet);

exit:
	return(0);

}
