#include <asm/arch/flash.h>
#include <asm/semaphore.h>
#include <linux/errno.h>
#include <linux/module.h>
#include <linux/config.h>
#include <linux/major.h>
#include <linux/slab.h>
#include <asm/fcntl.h>
#include <asm/unistd.h>
#include <asm/uaccess.h>
#include "cfi.h"

extern struct semaphore spare_lock;



int FindFooter(tfooter *** image_footer)
{
	static tfooter * image_footers[MAX_FOOTER_NUM];
	unsigned int footer_num=0;
	//int flash_type;
	unsigned int blocksize,addr,i;
	tfooter * footer;
	unsigned int * p;
	unsigned long long sum;
	
	*image_footer=image_footers;

	if(cfiGetFlashSize() == 0 )
		return -1;

	addr=FLASH_BASE+BOOTER_BLOCK_LENGTH;	// the 64kb of flash was preservied to boot loader
	blocksize=cfiGetBlockSize(addr);
	while( blocksize!= 0 )
	{
		addr+=blocksize;
		footer=(tfooter *)(addr-sizeof(tfooter));
		if(footer->signature==SIGNATURE_WORD)
		{
			p=(unsigned int *)footer;
			sum=0;
			for(i=0;i<sizeof(tfooter)/4-1;i++)// Not include the checksum
			{
				sum+=*(p+i);
			}
			sum = ~((sum&(-1LU))+(sum>>32));
			if( (unsigned int)sum == footer->checksum )					
				image_footers[footer_num++]=footer;
		}
		
		blocksize=cfiGetBlockSize(addr);
	}
	
	return footer_num;
}

/*1 success 0 failure*/
asmlinkage int sys_FindImage(unsigned int image_num, tfooter ** image_footer)
{
	int footer_num;
	tfooter ** footer;
	int i;

	
	footer_num=FindFooter(&footer);
	if(footer_num<=0)
	{
		return 0;
	}

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



/*0 success*/
asmlinkage int sys_DelImage(unsigned int image_num)
{
	unsigned int flash_type;
	tfooter * footer;
	unsigned int addr;
	unsigned int blocksize;
	unsigned int end;
	unsigned long flags;
	
	if( sys_FindImage( image_num, &footer) )
	{
		if(cfiGetFlashSize() == 0) {
			printk("delete failed\n");
			return -EINVAL;
		}	
		addr=footer->base;
		blocksize = cfiGetBlockSize(addr);
		end=footer->base+footer->length;

		end+=sizeof(tfooter);
		
		//printk("end at:%x", end);

		if((end & (blocksize-1)) == 0 ) 
			end -= blocksize;
		else 
			end = end&(~(blocksize-1));
		
		//printk("end at:%x", end);

		
		{
			down(&spare_lock);
			save_flags(flags); cli();

			cfiGetBlockSize(end);
			cfiCmd.erase(end,blocksize);
			restore_flags(flags);
			up(&spare_lock);
		}

		return 0;
	}
	printk("delete image failed\n");
	return -EINVAL;
}

/*0 success 1 Corrupt*/
asmlinkage int sys_CorruptCheck(tfooter * image_footer)
{
	int footer_num;
	tfooter ** footer;
	int i;
	unsigned int a0,a1,b0,b1;

	if( image_footer->base < (FLASH_BASE+BOOTER_BLOCK_LENGTH) )return 1;
	if( image_footer->base+image_footer->length > (FLASH_BASE+cfiGetFlashSize()) )return 1;
	footer_num=FindFooter(&footer);

	if(footer_num<=0)
		return 0;
	for(i=0;i<footer_num;i++)
	{
		if( image_footer->num == footer[i]->num ) {
			printk("iamge exists");
			return 1;
		}	
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



