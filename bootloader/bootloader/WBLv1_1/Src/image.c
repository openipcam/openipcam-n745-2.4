/******************************************************************************
 *
 * Copyright (c) 2003 Windond Electronics Corp.
 * All rights reserved.
 *
 * $Workfile: image.c $
 *
 * $Author: Yachen $
 ******************************************************************************/
/*
 * $History: image.c $
 * 
 * *****************  Version 1  *****************
 * User: Yachen       Date: 06/01/10   Time: 10:55a
 * Created in $/W90P710/Applications/710bootloader/WBLv1_1/Src
 * 710 Bootloader, without USB support
 * 
 * *****************  Version 1  *****************
 * User: Yachen       Date: 06/01/04   Time: 2:28p
 * Created in $/W90P710/Module Test Programs/FIRMWARE_710/WBLv1_1/Src
 * Module test bootloader, removed decompress function in order to save
 * memory space for LCD control
 * 
 * *****************  Version 8  *****************
 * User: Wschang0     Date: 04/03/19   Time: 4:54p
 * Updated in $/W90P710/FIRMWARE/WBLv1_1/Src
 * Add W19L320ST, W19L320SB
 * Arrange the flash array to faster detection
 * 
 * *****************  Version 7  *****************
 * User: Wschang0     Date: 03/12/25   Time: 4:06p
 * Updated in $/W90P710/FIRMWARE/WBLv1_1/Src
 * Add W29LV800BT, W29LV160DT, W29LV320DT
 * Remove the FLASH_NUM
 * 
 * *****************  Version 6  *****************
 * User: Wschang0     Date: 03/10/02   Time: 5:29p
 * Updated in $/W90P710/FIRMWARE/WBLv1_1/Src
 * Fix the bug that the top boot flash may fail to erase small (8K) blocks
 * and cause the write fail if there are unknown data in the small blocks.
 * 
 * *****************  Version 5  *****************
 * User: Wschang0     Date: 03/08/27   Time: 1:41p
 * Updated in $/W90P710/FIRMWARE/WBLv1_1/Src
 * Add ImageCheck function to check the image checksum
 * 
 * *****************  Version 4  *****************
 * User: Wschang0     Date: 03/08/27   Time: 11:28a
 * Updated in $/W90P710/FIRMWARE/WBLv1_1/Src
 * Add ChangeImageType function
 * 
 * *****************  Version 3  *****************
 * User: Wschang0     Date: 03/08/20   Time: 1:39p
 * Updated in $/W90P710/FIRMWARE/WBLv1_1/Src
 * 
 * *****************  Version 2  *****************
 * User: Wschang0     Date: 03/08/20   Time: 11:53a
 * Updated in $/W90P710/FIRMWARE/WBLv1_1/Src
 * Add VSS header
 */

#include "flash.h"

//#define DEBUG

CHAR * _flash_buffer=(CHAR *)0x300000; // must equal to BL_BUFFER_BASE

INT FindFlash(void)
{
	static UINT32 flash_type=-1;
	INT i;
	UCHAR pid0=0,pid1=0;
	
	if( flash_type == -1 )
	{
		i=0;
		while(flash[i].PID0 | flash[i].PID0)
		{
			pid0=pid1=0;
			flash[i].ReadPID(FLASH_BASE, &pid0, &pid1);

#ifdef DEBUG
		uprintf("PID Check [%d]: TYPE:%s ID:0x%02x 0x%02x\tFlash ID:0x%02x 0x%02x \n",i,flash[i].name,flash[i].PID0,flash[i].PID1,pid0, pid1);
#endif		

			if( (flash[i].PID0 == pid0) && (flash[i].PID1 == pid1) )
			{
				flash_type=i;
				break;		
			}
			i++;
		}
	}
	return flash_type;
}

INT FindFooter(tfooter *** image_footer)
{
	static tfooter * image_footers[MAX_FOOTER_NUM];
	UINT32 footer_num=0;
	INT flash_type;
	UINT32 blocksize,addr,i;
	tfooter * footer;
	UINT32 * p;
	unsigned long long sum;
	
	*image_footer=image_footers;
	
	if( (flash_type=FindFlash())<0 )return -1;
	
	addr=FLASH_BASE+BOOTER_BLOCK_LENGTH;	// the 64kb of flash was preservied to boot loader
	blocksize=flash[flash_type].BlockSize(addr);
	while( blocksize!= 0 )
	{
		addr+=blocksize;
		footer=(tfooter *)(addr-sizeof(tfooter));
		if(footer->signature==SIGNATURE_WORD)
		{
			p=(UINT32 *)footer;
			sum=0;
			for(i=0;i<sizeof(tfooter)/4-1;i++)// Not include the checksum
			{
				sum+=*(p+i);
			}
			sum = ~((sum&(-1LU))+(sum>>32));
			if( (UINT32)sum == footer->checksum )					
			{
				if( footer_num < MAX_FOOTER_NUM )
					image_footers[footer_num++]=footer;
			}
		}
		
		blocksize=flash[flash_type].BlockSize(addr);
	}
	
	return footer_num;
}

INT FindImage(UINT32 image_num, tfooter ** image_footer)
{
	INT footer_num;
	tfooter ** footer;
	INT i;
	
	footer_num=FindFooter(&footer);
	
	for(i=0;i<footer_num;i++)
	{
		if( footer[i]->num == image_num )
		{
			*image_footer=footer[i];
			return 1;
		}
	}
	return 0;
}


INT DelBlock(UINT32 block)
{
	UINT32 addr;
	UINT32 flash_type;
	UINT32 blockSize;
	UINT32 size;
	if(block==0)return -1; // block 0 is the location of boot loader.
	
	addr=FLASH_BASE+FLASH_BLOCK_SIZE*block;
	flash_type=FindFlash();
	blockSize=flash[flash_type].BlockSize(addr);
	if( blockSize==FLASH_BLOCK_SIZE )
		flash[flash_type].BlockErase(addr, FLASH_BLOCK_SIZE);
	else
	{
		// for top boot flash
		for(size=0;size<FLASH_BLOCK_SIZE;size+=blockSize)
		{
			flash[flash_type].BlockErase(addr+size, blockSize);
			blockSize=flash[flash_type].BlockSize(addr+size);
			if(blockSize==0)break;
		}
	
	}

	return 0;
}



INT DelImage(UINT32 image_num)
{
	UINT32 flash_type;
	tfooter * footer;
	UINT32 addr;
	UINT32 blocksize;
	UINT32 end;
	__weak extern uprintf(char *inf, ...);
	
	if( FindImage( image_num, &footer) )
	{
	
		flash_type=FindFlash();
		addr=footer->base;
		blocksize=flash[flash_type].BlockSize(addr);
		end=footer->base+footer->length;
		 // if there are appended block for footer usage
		if( (footer->length & (blocksize-1))==0 )end+=blocksize;
		while(addr < end)
		{
			blocksize=flash[flash_type].BlockSize(addr);
			flash[flash_type].BlockErase(addr,blocksize);
			addr+=blocksize;
			if( (INT)uprintf!=NULL )uprintf(".");
		}
		return 0;
	}
	return -1;
}

INT CorruptCheck(tfooter * image_footer)
{
	UINT32 footer_num;
	tfooter ** footer;
	INT i;
	UINT32 a0,a1,b0,b1;

	
	if( image_footer->base < (FLASH_BASE+BOOTER_BLOCK_LENGTH) )return 1;
	if( image_footer->base+image_footer->length > (FLASH_BASE+FlashSize()) )return 1;
	
	footer_num=FindFooter(&footer);
	for(i=0;i<footer_num;i++)
	{
		if( image_footer->num == footer[i]->num )
			return 1;
	}
	
	
	b0=image_footer->base;
	if( FLASH_BLOCK_SIZE-(image_footer->length%FLASH_BLOCK_SIZE) < sizeof(tfooter) ) 
		b1=image_footer->base+image_footer->length+FLASH_BLOCK_SIZE;
	else
		b1=image_footer->base+image_footer->length;
	for(i=0;i<footer_num;i++)
	{
		a0=footer[i]->base;
		if( (FLASH_BLOCK_SIZE-(footer[i]->length%FLASH_BLOCK_SIZE) < sizeof(tfooter)) || !(footer[i]->length&(FLASH_BLOCK_SIZE-1)) ) 
			a1=footer[i]->base+footer[i]->length+FLASH_BLOCK_SIZE;
		else
			a1=footer[i]->base+footer[i]->length;
		
		if( (b0 >= a0) && (b0 < a1) )return 1;
		if( (b1 >= a0) && (b1 < a1) )return 1;
		if( (a0 > b0) && (a1 < b1) )return 1;
	
	}	

	return 0;
}




INT WriteImage(tfooter * image_footer, UINT32 image_source)
{
	UINT32 flash_type;
	UINT32 blockSize,i,src,dest;
	unsigned long long sum;
	UINT32 k;
	UINT32 footer_addr;
	__weak extern uprintf(char *inf, ...);
	
	if( (image_footer->base & (FLASH_BLOCK_SIZE-1)) )
		return -1;	// not block alignment
	
	// check if word aligment
	if( (image_footer->length & 0x3) )
		image_footer->length=(image_footer->length&(~0x3))+4;
	
	// check write corrupt
	if( !CorruptCheck( image_footer )	)
	{
		flash_type=FindFlash();
			
		// Write program
		i=image_footer->length;
		src=(UINT32)image_source;
		dest=image_footer->base;
		sum=0;
		while(i)
		{
			blockSize=flash[flash_type].BlockSize(dest);
			if( i <= blockSize )
			{
				flash[flash_type].BlockErase(dest, blockSize);
				flash[flash_type].BlockWrite(dest, (UCHAR *)src, i);
				for(k=0;k<i/4;k++)sum+=*((UINT32 *)src+k);
				sum = ~((sum&(-1LU))+(sum>>32));
				image_footer->image_checksum=(UINT32)sum; // checksum of image
				sum=0;
				for(k=0;k<sizeof(tfooter)/4-1;k++)sum+=*((UINT32 *)image_footer+k);
				sum = ~((sum&(-1LU))+(sum>>32));
				image_footer->checksum=(UINT32)sum;		// checksum of footer
				if( ((blockSize-i)<sizeof(tfooter)) ) // Create a new block for footer
				{
					flash[flash_type].BlockErase(dest+blockSize, blockSize);
					flash[flash_type].BlockWrite(dest+2*blockSize-sizeof(tfooter), (UCHAR *)image_footer, sizeof(tfooter));
					footer_addr=dest+2*blockSize-sizeof(tfooter);
				}
				else	// Create the footer at current block
				{
					flash[flash_type].BlockWrite(dest+blockSize-sizeof(tfooter), (UCHAR *)image_footer, sizeof(tfooter));
					footer_addr=dest+blockSize-sizeof(tfooter);
				}
		
				blockSize=i; // Check if > a block size
			}
			else
			{
				flash[flash_type].BlockErase(dest, blockSize);
				flash[flash_type].BlockWrite(dest, (UCHAR *)src, blockSize);
				for(k=0;k<blockSize/4;k++)sum+=*((UINT32 *)src+k);
			}
			src+=blockSize;
			dest+=blockSize;
			i-=blockSize;
			if( (INT)uprintf!=NULL )uprintf(".");
		}
	}
	else
		return -1;

	// image write finished, check image checksum
	
	sum=0;
	for( i=image_footer->base;i< image_footer->base + image_footer->length ;i+=4 )sum+=*((UINT32 *)i);
	sum = ~((sum&(-1LU))+(sum>>32));
	
	if( (UINT32)sum != image_footer->image_checksum )
		return -1; // image check sum error
		
	// check footer check sum
	
	sum=0;
	for( i=footer_addr;i<footer_addr+sizeof(tfooter)-4;i+=4)sum+=*((UINT32 *)i);
	sum = ~((sum&(-1LU))+(sum>>32));
	
	if( (UINT32)sum != image_footer->checksum )
		return -1; // footer check sum error

	return 0;

}

INT WriteRawImage(tfooter * image_footer, UINT32 image_source)
{
	UINT32 flash_type;
	UINT32 blockSize,i,src,dest;
	unsigned long long sum;
	UINT32 k;
	UINT32 footer_addr;
	__weak extern uprintf(char *inf, ...);
	
	if( (image_footer->base & (FLASH_BLOCK_SIZE-1)) )
		return -1;	// not block alignment
	
	// check if word aligment
	if( (image_footer->length & 0x3) )
		image_footer->length=(image_footer->length&(~0x3))+4;
	
	// check write corrupt
	if( !CorruptCheck( image_footer )	)
	{
		flash_type=FindFlash();
			
		// Write program
		i=image_footer->length;
		src=(UINT32)image_source;
		dest=image_footer->base;
		sum=0;
		while(i)
		{
			blockSize=flash[flash_type].BlockSize(dest);
			if( i <= blockSize )
			{
				flash[flash_type].BlockErase(dest, blockSize);
				flash[flash_type].BlockWrite(dest, (UCHAR *)src, i);
				for(k=0;k<i/4;k++)sum+=*((UINT32 *)src+k);
				sum = ~((sum&(-1LU))+(sum>>32));
				image_footer->image_checksum=(UINT32)sum; // checksum of image
				sum=0;
				for(k=0;k<sizeof(tfooter)/4-1;k++)sum+=*((UINT32 *)image_footer+k);
				sum = ~((sum&(-1LU))+(sum>>32));
				image_footer->checksum=(UINT32)sum;		// checksum of footer
/*
// no footer
				if( ((blockSize-i)<sizeof(tfooter)) ) // Create a new block for footer
				{
					flash[flash_type].BlockErase(dest+blockSize, blockSize);
					flash[flash_type].BlockWrite(dest+2*blockSize-sizeof(tfooter), (UCHAR *)image_footer, sizeof(tfooter));
					footer_addr=dest+2*blockSize-sizeof(tfooter);
				}
				else	// Create the footer at current block
				{
					flash[flash_type].BlockWrite(dest+blockSize-sizeof(tfooter), (UCHAR *)image_footer, sizeof(tfooter));
					footer_addr=dest+blockSize-sizeof(tfooter);
				}
*/		
				blockSize=i; // Check if > a block size
			}
			else
			{
				flash[flash_type].BlockErase(dest, blockSize);
				flash[flash_type].BlockWrite(dest, (UCHAR *)src, blockSize);
				for(k=0;k<blockSize/4;k++)sum+=*((UINT32 *)src+k);
			}
			src+=blockSize;
			dest+=blockSize;
			i-=blockSize;
			if( (INT)uprintf!=NULL )uprintf(".");
		}
	}
	else
		return -1;

	// image write finished, check image checksum
	
	sum=0;
	for( i=image_footer->base;i< image_footer->base + image_footer->length ;i+=4 )sum+=*((UINT32 *)i);
	sum = ~((sum&(-1LU))+(sum>>32));
	
	if( (UINT32)sum != image_footer->image_checksum )
		return -1; // image check sum error
		
	// check footer check sum
/*
// no footer	
	sum=0;
	for( i=footer_addr;i<footer_addr+sizeof(tfooter)-4;i+=4)sum+=*((UINT32 *)i);
	sum = ~((sum&(-1LU))+(sum>>32));
	
	if( (UINT32)sum != image_footer->checksum )
		return -1; // footer check sum error
*/
	return 0;

}


INT WriteFile2Image(tfooter * image_footer, UINT32 buffer, INT file, INT (*read_func)(INT, VOID *, UINT32) )
{

	UINT32 flash_type;
	UINT32 blockSize,i,src,dest;
	unsigned long long sum;
	UINT32 k;
	UINT32 bytes;
	UINT32 footer_addr;
	
	if( (image_footer->base & (FLASH_BLOCK_SIZE-1)) )
		return -1;	// not block alignment

	// check if word aligment
	if( (image_footer->length & 0x3) )
		image_footer->length=(image_footer->length&(~0x3))+4;

	
	// check write corrupt
	if( !CorruptCheck( image_footer )	)
	{
		flash_type=FindFlash();
			
		// Write program
		i=image_footer->length;
		src=(UINT32)buffer;
		dest=image_footer->base;
		sum=0;
		while(i)
		{
			blockSize=flash[flash_type].BlockSize(dest);
			if( (bytes=read_func(file, (VOID *)buffer, blockSize))==-1 )
			{
				return -1; // read file error
			}
			if( i <= blockSize )
			{
				flash[flash_type].BlockErase(dest, FLASH_BLOCK_SIZE);
				flash[flash_type].BlockWrite(dest, (UCHAR *)src, i);
				for(k=0;k<i/4;k++)sum+=*((UINT32 *)src+k);
				sum = ~((sum&(-1LU))+(sum>>32));
				image_footer->image_checksum=(UINT32)sum; // checksum of image
				sum=0;
				for(k=0;k<sizeof(tfooter)/4-1;k++)sum+=*((UINT32 *)image_footer+k);
				sum = ~((sum&(-1LU))+(sum>>32));
				image_footer->checksum=(UINT32)sum;		// checksum of footer
				if( ((blockSize-i)<sizeof(tfooter)) ) // Create a new block for footer
				{
					flash[flash_type].BlockErase(dest+blockSize, FLASH_BLOCK_SIZE);
					flash[flash_type].BlockWrite(dest+2*blockSize-sizeof(tfooter), (UCHAR *)image_footer, sizeof(tfooter));
					footer_addr=dest+2*blockSize-sizeof(tfooter);
				}
				else	// Create the footer at current block
				{
					flash[flash_type].BlockWrite(dest+blockSize-sizeof(tfooter), (UCHAR *)image_footer, sizeof(tfooter));
					footer_addr=dest+blockSize-sizeof(tfooter);
				}
		
				blockSize=i; // Check if > a block size
			}
			else
			{
				flash[flash_type].BlockErase(dest, FLASH_BLOCK_SIZE);
				flash[flash_type].BlockWrite(dest, (UCHAR *)src, blockSize);
				for(k=0;k<blockSize/4;k++)sum+=*((UINT32 *)src+k);
			}
			src+=blockSize;
			dest+=blockSize;
			i-=blockSize;
		}
	}
	else
		return -1;

	// image write finished, check image checksum
	
	sum=0;
	for( i=image_footer->base;i< image_footer->base  + image_footer->length ;i+=4 )sum+=*((UINT32 *)i);
	sum = ~((sum&(-1LU))+(sum>>32));
	
	if( (UINT32)sum != image_footer->image_checksum )
		return -1; // image check sum error
		
	// check footer check sum
	
	sum=0;
	for( i=footer_addr;i<footer_addr+sizeof(tfooter)-4;i+=4)sum+=*((UINT32 *)i);
	sum = ~((sum&(-1LU))+(sum>>32));
	
	if( (UINT32)sum != image_footer->checksum )
		return -1; // footer check sum error

	return 0;
}


INT ChangeImageType(int image, int type)
{
	unsigned int block_base;
	tfooter * footer;
	int flash_type;
	int i;
	long long sum;
	extern INT memcpy(CHAR *dest, CHAR *src, INT size);
	
	if( FindImage( image, &footer ) )
	{
	
		block_base = (footer->base + footer->length) & (~(FLASH_BLOCK_SIZE-1));
	
		/* A buffer is required to backup the un-change date */
		memcpy( _flash_buffer, (char *)block_base, FLASH_BLOCK_SIZE);
		/* Modify the image attributes */
		footer = (tfooter *) (_flash_buffer + FLASH_BLOCK_SIZE - sizeof(tfooter));
		footer->type = type;
		/* Re-calculate footer check sum */
		sum=0;
		for(i=0;i<(sizeof(tfooter)-4)/4;i++)
		{
			sum+=*((unsigned int *)footer+i);
		}
		footer->checksum=~( (UINT)(sum>>32) + (UINT)sum );
		/* Write-back the new footer */
		flash_type=FindFlash();
		flash[flash_type].BlockErase(block_base, FLASH_BLOCK_SIZE);
		flash[flash_type].BlockWrite(block_base, (UCHAR *)_flash_buffer, FLASH_BLOCK_SIZE);
		/* Check the written date */
		for(i=0;i<FLASH_BLOCK_SIZE/4;i++)
		{
			if( *((UINT *)block_base + i)!= *((UINT *)_flash_buffer + i) )
			{
				return -1;
			}
		}
	}
	else
	{
		/* Can't find the image */
		return -1;
	}
	
	return 0;
}

/* ImageCheck function is used to check the image checksum */
INT ImageCheck(INT image)
{

	tfooter * footer;
	long long sum;
	UINT checksum;
	INT i;
	
	
	if( FindImage(image, &footer) )
	{
		sum=0;
		for(i=0;i<footer->length/4;i++)
			sum+=*((UINT *)footer->base+i);
		checksum=~((UINT)(sum>>32)+(UINT)sum);

		if( checksum != footer->image_checksum )
			return -1;
	}

	return 0;
}