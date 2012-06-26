#include <asm/semaphore.h>
#include <linux/errno.h>
#include <linux/module.h>
#include <linux/config.h>
#include <linux/major.h>
#include <linux/slab.h>
#include <asm/fcntl.h>
#include <asm/unistd.h>
#include <asm/uaccess.h>
#include <asm/arch/flash.h>
#include <asm/arch/bib.h>

#include <linux/init.h>
#include "cfi.h"

static int Init_WinbondFlash(void);

#define BL_PHY 0
#define BL_IC_PLUS 1
#define BL_MARVELL6052 2

tbl_info info;
DECLARE_MUTEX(spare_lock);

#define _DEBUG
#undef _DEBUG

#define MALLOC(x) 	kmalloc((x), GFP_KERNEL)
#define FREE		kfree
#define MEMCPY  	memcpy //or use "copy_to_user"

//yyang1 030605 ???
int __init Init_WinbondFlash(void)
{

	if (cfiGetFlashInfo() == -1) {
		printk("No supported flash detected!\n");
		return -EINVAL;
	}

	return 0;
}

asmlinkage int sys_ReadWinbondFlash(unsigned long pos, unsigned long length, char * buffer)
{
  //	if(!pCurFlash)
  //		return -EINVAL;
  if(cfiGetFlashSize() <= 0)
    return(-EINVAL);
	
#ifdef _DEBUG
	printk("\nWinbondFlash Read: pos = %x, length = %d, buffer = 0x%x.\n", pos, length, buffer);
#endif	
	if(length%2)
		length++;
	down(&spare_lock);
  	if(copy_to_user(buffer, (void*)(FLASH_BASE | pos), length))
  	{
  		up(&spare_lock);
  		return -EFAULT;
  	}
  	up(&spare_lock);
  	return length;
}

static int _FlashWrite(char *pcBuf, int iBufLen, int iOffset);
asmlinkage int sys_WriteWinbondFlash(unsigned long pos, unsigned long length, char * buffer)
{
  //	if(!pCurFlash)
  //	return -EINVAL;;

  if(cfiGetFlashSize() <= 0)
    return(-EINVAL);

#ifdef _DEBUG
	printk("\nWinbondFlash Write: pos = %x, length = %d, buffer = 0x%x.\n", pos, length, buffer);
#endif	

	return _FlashWrite(buffer, length, pos);
}

asmlinkage int sys_WinbondFlashBlockSize(unsigned long pos)
{
	if(cfiGetFlashSize() == 0)
		return -EINVAL;;

#ifdef _DEBUG
	printk("\nWinbondFlash BlockSize: pos = %d.\n", pos);
#endif	
	
	pos |= FLASH_BASE;
	return cfiGetBlockSize(pos);
}
asmlinkage int sys_WinbondFlashTotalSize(void)
{
	printk("sys_WinbondFlashTotalSize\n");
	return cfiGetFlashSize();
}
asmlinkage int sys_WinbondFlashBase(void)
{
		return FLASH_BASE;
}

static int _FlashWrite(char *pcBuf, int iBufLen, int iOffset)
{
  //	unsigned long flags;
	UINT32 blockSize,src,dest;
	INT i;

	if (cfiGetFlashSize() == 0) return -EINVAL;	

#ifdef _DEBUG			
	printk("\nWinbondFlash _FlashWrite: pos = %x, length = %d, buffer = 0x%x.\n", iOffset, iBufLen, pcBuf);
#endif	

	//i = iBufLen;
	i=(iBufLen%2)?(iBufLen+1):iBufLen;

	src = (UINT32)pcBuf;
	dest = (iOffset%2)?(iOffset+1):iOffset | FLASH_BASE;

	//CSR_WRITE(WTCR, (CSR_READ(WTCR)&0xF7)|0x01);//clyu reset watch dog
	
	while (i>0)
	{
		volatile UINT32 buf;
		volatile UINT32 addr;
		UINT32 count;
		int len;
		blockSize = cfiGetBlockSize(dest);
		if(blockSize<=0)
			return 0;
				
#ifdef _DEBUG			
//#if 1
	printk("\n_FlashWrite:dest:%x,cur blockSize:%x\n",dest,blockSize);
#endif			
		addr=dest&~(blockSize-1);
		for(len=0;len<i&&(len+dest-addr)<blockSize;len+=2)
			if(inph((dest+len))!=0xFFFF)
			{
#ifdef _DEBUG
				printk("01 inph(%x):%x\n",dest+len,inph(dest+len));
#endif
				if((dest&(blockSize-1))||(dest+i-addr)<=blockSize)
				{
					buf=(UINT32)MALLOC(blockSize);
					if(buf != 0)
					{
#ifdef _DEBUG			
	printk("\n_FlashWrite:dest:%x,addr:%x,buf:%x\n",dest,addr,buf);
#endif
						//memset((UCHAR *)buf,0xFF,blockSize);
						MEMCPY((UCHAR *)buf,(UCHAR *)addr,blockSize);
						
						//MEMCPY((UCHAR *)buf,(UCHAR *)addr,dest-addr);
						if(i<blockSize-(dest-addr))
							count=i;
						else 
							count=blockSize-(dest-addr);

#ifdef _DEBUG			
	printk("\n_FlashWrite:i:%d,count:%d,dest:%x,addr:%x,len:%d\n",i,count,dest,addr,len);
#endif		
			
						MEMCPY((void *)(buf+(dest-addr)),(void *)src,count);
						
						down(&spare_lock);
						//save_flags(flags); cli();//prevent closing watch dog interrupt
#ifdef _DEBUG
						printk("%s:%d BlockErase,%x\n",__FILE__,__LINE__,inph(dest+len));
#endif
						cfiCmd.erase(addr, blockSize);
#ifdef _DEBUG
						printk("02 inph(%x):%x\n",dest+len,inph(dest+len));
						printk("%s:%d BlockWrite\n",__FILE__,__LINE__);
#endif
						cfiCmd.write(addr, (UCHAR *)buf, blockSize);
#ifdef _DEBUG
						printk("03 inph(%x):%x\n",dest+len,inph(dest+len));
#endif
						//restore_flags(flags);
						up(&spare_lock);
				
						FREE((void *)buf);
						blockSize = count;
					}
					else
					{				
						return (iBufLen - i);
					}
				}
				else
				{
					down(&spare_lock);
					//save_flags(flags); cli();
#ifdef _DEBUG			
	printk("\nb_FlashWrite:i:%d,count:%d,dest:%x,addr:%x,%x\n",i,count,dest,addr,inph(dest+len));
#endif	
					cfiCmd.erase(dest, blockSize);									
					cfiCmd.write(dest, (UCHAR *)src, blockSize);
			
					//restore_flags(flags);
					up(&spare_lock);
				}
				break;
			}
		if((len>=i)||((len+dest-addr)>=cfiGetBlockSize(dest)))
		{
			if((dest&(blockSize-1))||(dest+i-addr)<=blockSize)
			{
#ifdef _DEBUG
printk("Write direct\n");
#endif
				if(i<blockSize-(dest-addr))
					count=i;
				else
					count=blockSize-(dest-addr);
				down(&spare_lock);
				//save_flags(flags); cli();
				cfiCmd.write(dest, (UCHAR *)src, count);
				//restore_flags(flags);
				up(&spare_lock);
				blockSize=count;
			}
			else
			{
#ifdef _DEBUG
printk("Write direct one block\n");
#endif
				down(&spare_lock);
				//save_flags(flags); cli();
				cfiCmd.write(dest, (UCHAR *)src, blockSize);
				//restore_flags(flags);
				up(&spare_lock);
			}
		}
		src+=blockSize;
		dest+=blockSize;
		i-=blockSize;
	}
		
	return (iBufLen);
}

module_init(Init_WinbondFlash);
