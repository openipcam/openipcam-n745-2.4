/******************************************************************************
 *
 * Copyright (c) 2003 Windond Electronics Corp.
 * All rights reserved.
 *
 * $Workfile: flash.c $
 *
 * $Author: andy $
 ******************************************************************************/
/*
 * $History: flash.c $
 * 
 * *****************  Version 10  *****************
 * User: Wschang0     Date: 04/01/07   Time: 11:34a
 * Updated in $/W90N740/FIRMWARE/WBLv1_1/Src
 * Increase the timeout count in polling16 function to avoid early timeout
 * in W29LV800BT
 * 
 * *****************  Version 9  *****************
 * User: Wschang0     Date: 03/12/25   Time: 4:06p
 * Updated in $/W90N740/FIRMWARE/WBLv1_1/Src
 * Add W29LV800BT, W29LV160DT, W29LV320DT
 * Remove the FLASH_NUM
 * 
 * *****************  Version 8  *****************
 * User: Wschang0     Date: 03/12/03   Time: 5:16p
 * Updated in $/W90N740/FIRMWARE/WBLv1_1/Src
 * Add MX29LV160BT/TT
 * Fixed MX28F160C3BT/TT.
 * The MX28F160C3 sectors are default to be locked, thus it needs unlock
 * it before write/erase it.
 * 
 * *****************  Version 7  *****************
 * User: Wschang0     Date: 03/11/05   Time: 11:03a
 * Updated in $/W90N740/FIRMWARE/WBLv1_1/Src
 * Add MX28F160C3T & MX28F160C3B
 * 
 * *****************  Version 6  *****************
 * User: Wschang0     Date: 03/09/26   Time: 2:30p
 * Updated in $/W90N740/FIRMWARE/WBLv1_1/Src
 * Correct the flash name of SST39VF160 in the flash[] table
 * 
 * *****************  Version 5  *****************
 * User: Wschang0     Date: 03/08/27   Time: 1:41p
 * Updated in $/W90N740/FIRMWARE/WBLv1_1/Src
 * Add FlushDCache to Write/Erase functions to avoid data cache
 * incohereance after Write/Erase flash.
 * 
 * *****************  Version 4  *****************
 * User: Wschang0     Date: 03/08/27   Time: 11:28a
 * Updated in $/W90N740/FIRMWARE/WBLv1_1/Src
 * Add SST 39VF160 flash type
 * 
 * *****************  Version 3  *****************
 * User: Wschang0     Date: 03/08/20   Time: 1:39p
 * Updated in $/W90N740/FIRMWARE/WBLv1_1/Src
 * 
 * *****************  Version 2  *****************
 * User: Wschang0     Date: 03/08/20   Time: 11:53a
 * Updated in $/W90N740/FIRMWARE/WBLv1_1/Src
 * Add VSS header
 */
#include <asm/arch/flash.h>
#include <linux/types.h>
#include <linux/slab.h>
// Update Note:
// 8/19, 2003: Add Flash Tyep W28J800BT, W28J320TT, Fix the PID of W28J320BT
// 8/07, 2003: W39L010 was protected when the erase address not in offset 0
// 7/24, 2003: Remove the redundant code. Time-out check, for reliability

UINT32 _flash_size = 0;

#define DELAY_1US 20

#define TIMEOUT		8000000UL
static int normal_polling(unsigned int addr, unsigned short mask)
{
	unsigned short	rdata;
	unsigned int cnt;
	
	rdata=inph(addr)&mask;
	cnt=0;
	while( rdata != mask )
	{
		rdata=inph(addr)&mask;
		if( cnt++ > TIMEOUT ) return -1; // time-out
	}

	return 0;
}

static int polling16(unsigned int addr, unsigned short data)
{
	unsigned short	rdata;
	int timeout=0x600000;
	
	rdata=inph(addr);
	while( rdata != data )
	{
		rdata=inph(addr);
		if( (rdata&0x20) || !(timeout--) )
		{	
			rdata=inph(addr);
			if( rdata != data )
			{
				//printf("polling time-out: %x\n",rdata);
				return -1;	//polling time-out
			}
		}
	}

	return 0;
}


static INT CheckDataWidth(INT w)
{
	INT extio_flag=0;
	// check if the platform (the Flash ROM is not in 0x7F000000)
	if( (ROMCON&0xFF000000)==0xFC000000 )extio_flag=1;

	switch(w)
	{
		case 8:
			if(extio_flag)
			{
				if( (EXT3CON&0x3)== 0x1 )return 1;
				else return 0;
			} 
			else
			{
				if( (ROMCON&0xC)== 0x0 )return 1;
				else return 0;
			}
		case 16:
			if(extio_flag)
			{
				if( (EXT3CON&0x3)== 0x2 )return 1;
				else return 0;
			} 
			else
			{
				if( (ROMCON&0xC)== 0x4 )return 1;
				else return 0;
			}
		case 32:
			if(extio_flag)
			{
				if( (EXT3CON&0x3)== 0x3 )return 1;
				else return 0;
			} 
			else
			{
				if( (ROMCON&0xC)== 0x8 )return 1;
				else return 0;
			}
		default:
			return 0;
	}

}


void FlashDelay(UINT32 delay)
{
	volatile UINT32 i;
	for(i=0;i<delay*DELAY_1US;i++);	
}

static __inline void FlushDCache()
{
	/* Flush Entire DCache */
	if( CAHCNF_R & 0x6 ) /* If write buffer or data cache is enabled */
	{
		CAHCON_R=0x86;
		while( CAHCON_R );
	}
	
}



INT BlockLock_W28J800TT(UINT32 address, UINT32 op)
{
	address|=0x80000000;

	if( op==BLOCK_LOCK )
	{	
		outph(address,0x70);
		while( !(inph(address)&0x80) );
		outph(address,0x60);
		outph(address,0x01);
		while( !(inph(address)&0x80) );
		outph(address,0xFFFF);
	}
	else if( op==BLOCK_UNLOCK )
	{
		outph(address,0x70);
		while( !(inph(address)&0x80) );
		outph(address,0x60);
		outph(address,0xD0);
		while( !(inph(address)&0x80) );
		outph(address,0xFFFF);
	}
	return 0;
}

INT BlockSize_W28J800TT(UINT32 address)
{
	address-=FLASH_BASE; 
	address&=0x7FFFFFFF;
	if( address >= 0xF0000 )
	{
		if( address < _flash_size )
			return 0x2000;
		else
			return 0;
	}
	else
		return 0x10000;
}

INT BlockSize_W28J800BT(UINT32 address)
{
	address-=FLASH_BASE; 
	address&=0x7FFFFFFF;
	if( address < 0x10000 )
		return 0x2000;
	else
	{
		if( address <  _flash_size )
			return 0x10000;
		else
			return 0;
	}
}


INT BlockErase_W28J800TT(UINT32 address,UINT32 size)
{
	int status;
	if( (address&(size-1))!=0x0 )return -1;// not in the start of a block
	
	BlockLock_W28J800TT(address,BLOCK_UNLOCK); // The intel flash sector is default to be locked

	address|=0x80000000;
	outph(address,0x70);
	status=normal_polling( address, 0x80 );
	if( status < 0 )return -1; // polling time-out
	outph(address,0x20);
	outph(address,0xD0);
	status=normal_polling( address, 0x80 );
	if( status < 0 )return -1; // polling time-out
	outph(address,0xFFFF);
	FlushDCache();
	
	return 0;
}

INT BlockWrite_W28J800TT(UINT32 address, UCHAR * data, UINT32 size)
{
	UINT32 i;
	int status;	
	address|=0x80000000;
	BlockLock_W28J800TT(address,BLOCK_UNLOCK); // The MX28F160C3 sector is default to be locked
	for(i=address;i<address+size;i+=2)
	{
		outph(i,0x40);
		outph(i,*((UINT16 *)data));
		//while( !(inph(i)&0x80) );
		status=normal_polling( i, 0x80 );
		if( status < 0 )return -1; // polling time-out
		data+=2;
	}
	outph(address,0xFFFF);
	FlushDCache();

	return 0;
}

INT ReadPID_Winbond(UINT32 address, UCHAR *pid0, UCHAR *pid1)
{
	if( !CheckDataWidth(16) )return -1;

	address|=0x80000000;
	outph(address,0x70);
	FlashDelay(50000); // delay 50ms
	outph(address,0x90);
	*pid0=inph(address);
	*pid1=inph(address+1);
	outph(address,0xFFFF);

	return 0;
}

INT ReadPID_W28J800TT(UINT32 address, UCHAR *pid0, UCHAR *pid1)
{
	int status;
	status=ReadPID_Winbond(address,pid0,pid1);
	if( status < 0 )return -1; // not x16 flash
	_flash_size=0x100000;
	return 0;
}

INT ReadPID_W28J800BT(UINT32 address, UCHAR *pid0, UCHAR *pid1)
{
	int status;
	status=ReadPID_Winbond(address,pid0,pid1);
	if( status < 0 )return -1; // not x16 flash
	_flash_size=0x100000;
	return 0;
}

INT ReadPID_W28J160TT(UINT32 address, UCHAR *pid0, UCHAR *pid1)
{
	int status;
	status=ReadPID_Winbond(address,pid0,pid1);
	if( status < 0 )return -1; // not x16 flash
	_flash_size=0x200000;
	return 0;
}

INT ReadPID_W28J320TT(UINT32 address, UCHAR *pid0, UCHAR *pid1)
{
	int status;
	status=ReadPID_Winbond(address,pid0,pid1);
	if( status < 0 )return -1; // not x16 flash
	_flash_size=0x400000;
	return 0;
}




INT BlockSize_W28J320TT(UINT32 address)
{
	address-=FLASH_BASE; 
	address&=0x7FFFFFFF;
	if( address >= 0x3F0000 )
	{
		if( address < _flash_size )
			return 0x2000;
		else
			return 0;
	}
	else
		return 0x10000;
}


INT BlockSize_W28J320BT(UINT32 address)
{
	address-=FLASH_BASE; 
	address&=0x7FFFFFFF;
	if( address < 0x10000 )
		return 0x2000;
	else
	{
		if( address <  _flash_size )
			return 0x10000;
		else
			return 0;
	}
}



INT BlockSize_W28J160BT(UINT32 address)
{
	address-=FLASH_BASE; 
	address&=0x7FFFFFFF;
	if( address < 0x10000 )
		return 0x2000;
	else
	{
		if( address <  _flash_size )
			return 0x10000;
		else
			return 0;
	}
}

INT BlockSize_W28J160TT(UINT32 address)
{
	address-=FLASH_BASE; 
	address&=0x7FFFFFFF;
	if( address >= 0x1F0000 )
	{
		if( address < 0x200000 )
			return 0x2000;
		else
			return 0;
	}
	else
		return 0x10000;
}


INT BlockLock_W39L010(UINT32 address, UINT32 op)
{
	return -1;
}

INT BlockSize_W39L010(UINT32 address)
{
	address-=FLASH_BASE;
	address&=0x7FFFFFFF;
	if( address	< _flash_size )
		return 0x20000;
	else
		return 0;
}

INT BlockErase_W39L010(UINT32 address,UINT32 size)
{
	UINT32 addr1,addr2;

	if( (address&(0x20000-1))!=0x0 )return -1;// not in the start of flash

	address|=0x80000000;
	addr1=(address&0xFFF00000)+(0x5555);
	addr2=(address&0xFFF00000)+(0x2AAA);
	
	outpb(addr1,0xAA);
	outpb(addr2,0x55);
	outpb(addr1,0x80);
	outpb(addr1,0xAA);
	outpb(addr2,0x55);
	outpb(addr1,0x10);
	while( (inpb(address)&0x80)!=0x80 );

	FlushDCache();
	
	return 0;
}

INT BlockWrite_W39L010(UINT32 address, UCHAR * data, UINT32 size)
{
	UINT32 i;
	UINT32 addr1, addr2;

	address|=0x80000000;
	addr1=(address&0xFFF00000)+(0x5555);
	addr2=(address&0xFFF00000)+(0x2AAA);
	for(i=address;i<address+size;i++)
	{
		outpb(addr1,0xAA);
		outpb(addr2,0x55);
		outpb(addr1,0xA0);
		outpb(i,*data);
		while( !((inpb(i)&0x80)==(*data&0x80)) );		
		data++;
	}
	FlushDCache();

	return 0;
}

INT ReadPID_W39L010(UINT32 address, UCHAR *pid0, UCHAR *pid1)
{
	UINT32 addr1, addr2;

	if( !CheckDataWidth(8) )return -1;



	address|=0x80000000;
	addr1=(address&0xFFF00000)+(0x5555);
	addr2=(address&0xFFF00000)+(0x2AAA);

	outpb(addr1,0xAA);
	outpb(addr2,0x55);
	outpb(addr1,0x80);
	outpb(addr1,0xAA);
	outpb(addr2,0x55);
	outpb(addr1,0x60);
	*pid0=inpb(address);
	*pid1=inpb(address+1);
	outpb(addr1,0xAA);
	outpb(addr2,0x55);
	outpb(addr1,0xF0);
	outpb(address,0xFF);

	_flash_size=0x20000;

	return 0;
}



INT BlockLock_W29EE011(UINT32 address, UINT32 op)
{
	return -1;
}


INT BlockSize_W29EE011(UINT32 address)
{
	address-=FLASH_BASE;
	address&=0x7FFFFFFF;
	if( address < _flash_size )
		return 0x80;
	else
		return 0;
}

INT BlockErase_W29EE011(UINT32 address,UINT32 size)
{
	UINT32 addr1,addr2;
	static INT flag=0;

	if( flag )return 0;

	flag=1;		

	address|=0x80000000;
	addr1=(address&0xFFF00000)+(0x5555);
	addr2=(address&0xFFF00000)+(0x2AAA);
	
	outpb(addr1,0xAA);
	outpb(addr2,0x55);
	outpb(addr1,0x80);
	outpb(addr1,0xAA);
	outpb(addr2,0x55);
	outpb(addr1,0x10);
	FlashDelay(50000); // delay 50ms

	FlushDCache();

	return 0;
}

INT BlockWrite_W29EE011(UINT32 address, UCHAR * data, UINT32 size)
{
	UINT32 i;
	UINT32 addr1, addr2;

	address|=0x80000000;
	addr1=(address&0xFFF00000)+(0x5555);
	addr2=(address&0xFFF00000)+(0x2AAA);
	outpb(addr1,0xAA);
	outpb(addr2,0x55);
	outpb(addr1,0xA0);
	for(i=0;i<size;i++)
		outpb(address+i,*(data+i));
	while( inpb(address+i-1)!=*(data+i-1) );
	FlushDCache();

	return 0;
}

INT ReadPID_W29EE011(UINT32 address, UCHAR *pid0, UCHAR *pid1)
{
	UINT32 addr1, addr2;
	
	if( !CheckDataWidth(8) )return -1;


	address|=0x80000000;
	addr1=(address&0xFFF00000)+(0x5555);
	addr2=(address&0xFFF00000)+(0x2AAA);

	outpb(addr1,0xAA);
	outpb(addr2,0x55);
	outpb(addr1,0x80);
	outpb(addr1,0xAA);
	outpb(addr2,0x55);
	outpb(addr1,0x60);
	FlashDelay(10);//delay 10 us
	*pid0=inpb(address);
	*pid1=inpb(address+1);
	outpb(addr1,0xAA);
	outpb(addr2,0x55);
	outpb(addr1,0xF0);
	FlashDelay(10);//delay 10 us
	outpb(address,0xFF);

	_flash_size=0x20000;

	return 0;
}



INT BlockLock_AM29LV800BB(UINT32 address, UINT32 op)
{
	return -1;
}


INT BlockSize_AM29LV800BB(UINT32 address)
{
	address-=FLASH_BASE; 
	address&=0x7FFFFFFF;
	if( address < 0x4000 )
		return 0x4000;
	else if( address < 0x6000 )
		return 0x2000;
	else if( address < 0x8000 )
		return 0x2000;
	else if( address < 0x10000 )
		return 0x8000;
	else
	{
		if( address <  _flash_size )
			return 0x10000;
		else
			return 0;
	}
}

INT BlockSize_AM29LV800BT(UINT32 address)
{
	address-=FLASH_BASE; 
	address&=0x7FFFFFFF;
	if( address < 0x0F0000 )
		return 0x10000;
	else if( address < 0xF8000 )
		return 0x8000;
	else if( address < 0xFA000 )
		return 0x2000;
	else if( address < 0xFC000 )
		return 0x2000;
	else
	{
		if( address <  _flash_size )
			return 0x4000;
		else
			return 0;
	}
}
INT BlockErase_AM29LV800BB(UINT32 address,UINT32 size)
{
	UINT32 addr1,addr2;
	INT status;

	if( (address&(size-1))!=0x0 )return -1;// not in the start of a block

	address|=0x80000000;
	addr1=(address&0xFFF00000)+(0x5555<<1);
	addr2=(address&0xFFF00000)+(0x2AAA<<1);
	
	outph(addr1,0xAA);
	outph(addr2,0x55);
	outph(addr1,0x80);
	outph(addr1,0xAA);
	outph(addr2,0x55);
	outph(address,0x30);
	//while( (inph(address))!=0xFFFF );
	status=polling16(address, 0xFFFF);

	FlushDCache();

	return status;
}

INT BlockWrite_AM29LV800BB(UINT32 address, UCHAR * data, UINT32 size)
{
	UINT32 i;
	UINT32 addr1, addr2;
	INT status;

	address|=0x80000000;
	addr1=(address&0xFFF00000)+(0x5555<<1);
	addr2=(address&0xFFF00000)+(0x2AAA<<1);
	
	for(i=address;i<address+size;i+=2)
	{
		outph(addr1,0xAA);
		outph(addr2,0x55);
		outph(addr1,0xA0);
		outph(i,*((UINT16*)data));
		status=polling16( i, (*(UINT16 *)data));
		if( status < 0 )return status; // time-out
		data+=2;
	}

	outph(address,0xFFFF);
	FlushDCache();

	return status;
}


INT ReadPID_AM29LV(UINT32 address, UCHAR *pid0, UCHAR *pid1)
{
	UINT32 addr1, addr2;

	if( !CheckDataWidth(16) )return -1;


	address|=0x80000000;
	addr1=(address&0xFFF00000)+(0x5555<<1);
	addr2=(address&0xFFF00000)+(0x2AAA<<1);

	outph(addr1,0xAA);
	outph(addr2,0x55);
	outph(addr1,0x90);
	*pid0=(char)inph(address);
	*pid1=(char)inph((address+2));
	outph(address,0xFFFF);

	return 0;
}


INT ReadPID_AM29LV800BB(UINT32 address, UCHAR *pid0, UCHAR *pid1)
{
	
	ReadPID_AM29LV(address, pid0, pid1);
	_flash_size=0x100000;

	return 0;
}

INT ReadPID_AM29LV160DB(UINT32 address, UCHAR *pid0, UCHAR *pid1)
{

	ReadPID_AM29LV(address, pid0, pid1);
	_flash_size=0x200000;

	return 0;
}

INT ReadPID_AM29LV320DB(UINT32 address, UCHAR *pid0, UCHAR *pid1)
{

	ReadPID_AM29LV(address, pid0, pid1);
	_flash_size=0x400000;

	return 0;
}



INT BlockSize_AM29LV160DB(UINT32 address)
{
	address-=FLASH_BASE; 
	address&=0x7FFFFFFF;
	if( address < 0x4000 )
		return 0x4000;
	else if( address < 0x6000 )
		return 0x2000;
	else if( address < 0x8000 )
		return 0x2000;
	else if( address < 0x10000 )
		return 0x8000;
	else
	{
		if( address <  _flash_size )
			return 0x10000;
		else
			return 0;
	}
}

INT BlockSize_AM29LV160DT(UINT32 address)
{
	address-=FLASH_BASE; 
	address&=0x7FFFFFFF;
	if( address < 0x1F0000 )
		return 0x10000;
	else if( address < 0x1F8000 )
		return 0x8000;
	else if( address < 0x1FA000 )
		return 0x2000;
	else if( address < 0x1FC000 )
		return 0x2000;
	else
	{
		if( address <  _flash_size )
			return 0x4000;
		else
			return 0;
	}
}
INT BlockSize_AM29LV320DB(UINT32 address)
{
	address-=FLASH_BASE; 
	address&=0x7FFFFFFF;
	if( address < 0x10000 )
		return 0x2000;
	else
	{
		if( address < _flash_size )
			return 0x10000;
		else
			return 0;
	}
}

INT BlockSize_AM29LV320DT(UINT32 address)
{
	address-=FLASH_BASE; 
	address&=0x7FFFFFFF;
	if( address < 0x3F0000 )
		return 0x10000;
	else
	{
		if( address < _flash_size )
			return 0x2000;
		else
			return 0;
	}
}

INT BlockLock_E28F640(UINT32 address, UINT32 op)
{

	address|=0x80000000;

	if( op==BLOCK_LOCK )
	{	
		outph(address,0x60);
		outph(address,0x01);
		while( !(inph(address)&0x80) );
		outph(address,0xFFFF);
	}
	else if( op==BLOCK_UNLOCK )
	{
		outph(address,0x60);
		outph(address,0xD0);
		while( !(inph(address)&0x80) );
		outph(address,0xFFFF);
	}

	return 0;
}

INT BlockSize_E28F640(UINT32 address)
{
	address-=FLASH_BASE; 
	address&=0x7FFFFFFF;
	if( address < _flash_size )
		return 0x10000;
	else
		return 0;
}

INT BlockErase_E28F640(UINT32 address,UINT32 size)
{
	UINT32 i,j,tmp;
	CHAR * buffer;
	
	address|=0x80000000;
	buffer = kmalloc(128*1024,GFP_KERNEL);
	if(!buffer)
		return 1;
	// backup the data
	j=0xFFFF;
	if( (address & FLASH_BLOCK_SIZE) )
	{
		
		for(i=0;i<FLASH_BLOCK_SIZE/2;i++)
		{
			tmp=*((volatile unsigned short *)(address-FLASH_BLOCK_SIZE)+i);
			*((volatile unsigned short *)buffer+i)=tmp;
			j&=tmp;
		}
	}
	else
	{
		for(i=0;i<FLASH_BLOCK_SIZE/2;i++)
		{
			tmp=*((volatile unsigned short *)(address+FLASH_BLOCK_SIZE)+i);
			*((volatile unsigned short *)buffer+i)=tmp;
			j&=tmp;
		}
	}
	

	outph(address,0x20);
	outph(address,0xD0);
	while( !(inph(address)&0x80) );
	outph(address,0xFFFF);

	
	// write back the data
	if( (j&0xFFFF)!=0xFFFF )
	{
		if( (address & FLASH_BLOCK_SIZE) )
			BlockWrite_E28F640(address-FLASH_BLOCK_SIZE, (UCHAR *)buffer, FLASH_BLOCK_SIZE);
		else
			BlockWrite_E28F640(address+FLASH_BLOCK_SIZE, (UCHAR *)buffer, FLASH_BLOCK_SIZE);
	}
	FlushDCache();
	kfree(buffer);
	return 0;
}

INT BlockWrite_E28F640(UINT32 address, UCHAR * data, UINT32 size)
{
	UINT32 i;
#if 0
	UINT32 pdata,j;

	address|=0x80000000;
	
	do{
		outph(address, 0xE8);
	}while( !(inph(address)&0x80)	);

	i=size;
	while(i)
	{
		if( i < 32 )
		{
			outph(address, i);
			for(j=0;j<i/2;j++)
				outph(address+j, *((unsigned short *)pdata+j));
			pdata+=i;
			i=0;
		}
		else
		{
			outph(address, 32);
			for(j=0;j<16;j++)
				outph(address+j, *((unsigned short *)pdata+j));
			pdata+=32;
			i-=32;
		}
		outph(address, 0xD0);
		while( !(inph(address)&0x80) );	
	}

#else		
	address|=0x80000000;

	for(i=address;i<address+size;i+=2)
	{
		outph(i,0x40);
		outph(i,*((UINT16 *)data));
		while( !(inph(i)&0x80) );
		data+=2;
	}

	outph(address,0xFFFF);
#endif

	FlushDCache();
	return 0;
}

INT ReadPID_E28F640(UINT32 address, UCHAR *pid0, UCHAR *pid1)
{
	if( !CheckDataWidth(16) )return -1;

	outph(address,0x90);
	*pid0=inph(address);
	*pid1=inph(address+1);
	outph(address,0xFFFF);

	_flash_size=0x800000;

	return 0;
}


INT BlockSize_SST39VF160(UINT32 address)
{
	address-=FLASH_BASE; 
	address&=0x7FFFFFFF;
	if( address <  _flash_size )
		return 0x10000;
	else
		return 0;

}


INT ReadPID_SST39VF160(UINT32 address, UCHAR *pid0, UCHAR *pid1)
{
	
	ReadPID_AM29LV(address, pid0, pid1);
	outph(address, 0x00F0 ); /* Exit Software ID mode */
	_flash_size=0x200000;

	return 0;
}

INT BlockErase_SST39VF160(UINT32 address,UINT32 size)
{
	UINT32 addr1,addr2;
	INT status;

	if( (address&(size-1))!=0x0 )return -1;// not in the start of a block

	address|=0x80000000;
	addr1=(address&0xFFF00000)+(0x5555<<1);
	addr2=(address&0xFFF00000)+(0x2AAA<<1);
	
	outph(addr1,0xAA);
	outph(addr2,0x55);
	outph(addr1,0x80);
	outph(addr1,0xAA);
	outph(addr2,0x55);
	outph(address,0x50);
	status=polling16(address, 0xFFFF);

	FlushDCache();

	return status;
}

INT BlockErase_MX28F160C3B(UINT32 address,UINT32 size)
{
	int status;
	if( (address&(size-1))!=0x0 )return -1;// not in the start of a block
	
	BlockLock_W28J800TT(address,BLOCK_UNLOCK); // The MX28F160C3 sector is default to be locked

	address|=0x80000000;
	outph(address,0x70);
	status=normal_polling( address, 0x80 );
	if( status < 0 )return -1; // polling time-out
	outph(address,0x20);
	outph(address,0xD0);
	status=normal_polling( address, 0x80 );
	if( status < 0 )return -1; // polling time-out
	outph(address,0xFFFF);
	FlushDCache();
	
	return 0;
}

INT BlockWrite_MX28F160C3B(UINT32 address, UCHAR * data, UINT32 size)
{
	UINT32 i;
	int status;	

	BlockLock_W28J800TT(address,BLOCK_UNLOCK); // The MX28F160C3 sector is default to be locked

	address|=0x80000000;
	for(i=address;i<address+size;i+=2)
	{
		outph(i,0x40);
		outph(i,*((UINT16 *)data));
		//while( !(inph(i)&0x80) );
		status=normal_polling( i, 0x80 );
		if( status < 0 )return -1; // polling time-out
		data+=2;
	}
	outph(address,0xFFFF);
	FlushDCache();

	return 0;
}

UINT32 FlashSize()
{
	return _flash_size;
}




