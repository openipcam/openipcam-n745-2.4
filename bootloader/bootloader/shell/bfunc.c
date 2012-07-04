/******************************************************************************
 *
 * Copyright (c) 2003 Windond Electronics Corp.
 * All rights reserved.
 *
 * $Workfile: bfunc.c $
 *
 * $Author: Yachen $
 ******************************************************************************/
/*
 * $History: bfunc.c $
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
 * User: Wschang0     Date: 03/09/24   Time: 8:03p
 * Updated in $/W90P710/FIRMWARE/shell
 * Add VSS header
 */
#include "cdefs.h"
#include "flash.h"
#include "bfunc.h"

INT LoadBIB(void ** BIB)
{
	volatile unsigned int * p;
	UINT32 i;

	for(i=0;i<MAX_BIB_NUMBER;i++)
		if( BIB[i] != NULL )return -1; // check if BIB temp space is empty
	p=(volatile unsigned int *)BOOTINFO_BLOCK_BASE;
	i=0;
	while( (*p!=-1) && (i < MAX_BIB_NUMBER) )
	{
		BIB[i]=(void *)malloc(*p);
		memcpy(BIB[i],(void *)p,*p);
		p=p+*p/4;
		i++;
	}
	
	return i;
}

INT StoreBIB(void ** BIB)
{
	volatile unsigned int * p;
	UINT32 i;
	UINT32 flash_type;
	UINT32 addr;
	void * pBIB;
	
	// Every thing should be word alignment!!
	
	if( BIB == NULL )return -1;
	if( BIB[0] == NULL )return -1;
	
	flash_type=FindFlash();
	flash[flash_type].BlockErase(BOOTINFO_BLOCK_BASE,BOOTINFO_BLOCK_SIZE);
	addr=BOOTINFO_BLOCK_BASE;
	i=0;
	while( BIB[i]!=NULL )
	{
		pBIB=(void *)BIB[i];
		flash[flash_type].BlockWrite(addr,(UCHAR *) pBIB, *(UINT32 *)pBIB);
		addr+=*(UINT32 *)pBIB;	
		i++;
	}

	return i;
}


INT AddBIB(void * BIB, UINT32 src)
{
	void * BIBlist[MAX_BIB_NUMBER]={0};
	UINT32 bib_num;
	UINT32 i;
	UINT32 base;
	UINT32 flash_type;
	
	bib_num=LoadBIB(BIBlist);
	if( src != NULL )
	{
		// find free space
		base=BOOTFUNC_BLOCK_BASE;
		for(i=0;i<MAX_BIB_NUMBER;i++)
		{
			if( BIBlist[i]==NULL )break;
			base+=((tBIB *)BIBlist[i])->func_length;
		}
		((tBIB *)BIB)->base=base;
	}
	// add new element
	BIBlist[bib_num]=(void *)malloc(*(UINT32 *)BIB);	
	memcpy((char *)BIBlist[bib_num],(char *)BIB, *(UINT32 *)BIB);
	// wirte BIB 
	StoreBIB(BIBlist);
	// write function
	if( src != NULL )
	{
		flash_type=FindFlash();
		// !! the free space must real free!! 
		flash[flash_type].BlockWrite(((tBIB *)BIBlist[bib_num])->base,(UCHAR *) src, ((tBIB *)BIBlist[bib_num])->func_length);
	}
	
	// free the allocated memory
	for(i=0;i<MAX_BIB_NUMBER;i++)
		if( BIBlist[i]!=NULL )free(BIBlist[i]);
	
	return 0;
}

INT CleanFunc()
{
	UINT32 flash_type;
	UINT32 blocksize;
	UINT32 i,addr;
	flash_type=FindFlash();
	blocksize=flash[flash_type].BlockSize(BOOTFUNC_BLOCK_BASE);
	flash[flash_type].BlockErase(BOOTINFO_BLOCK_BASE, BOOTINFO_BLOCK_SIZE);
	
	i=BOOTFUNC_BLOCK_SIZE;
	addr=BOOTFUNC_BLOCK_BASE;
	while(i)
	{
		flash[flash_type].BlockErase(addr, blocksize);
		addr+=blocksize;
		i-=blocksize;
	}
	return 0;
}

INT FindFunc(tBIB ** BIB)
{
	volatile unsigned int * p;
	UINT32 i;

	for(i=0;i<MAX_BIB_NUMBER;i++)
		if( BIB[i] != NULL )return -1; // check if BIB temp space is empty
	p=(volatile unsigned int *)BOOTINFO_BLOCK_BASE;
	i=0;
	while( (*p!=-1) && (i < MAX_BIB_NUMBER) && (*(p+1)==FUN_INFO_BLOCK) )
	{
		BIB[i]=(void *)malloc(*p);
		memcpy(BIB[i],(void *)p,*p);
		p=p+*p/4;
		i++;
	}
	return i;
}