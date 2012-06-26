#include <asm/arch/flash.h>
#include <linux/slab.h>

#ifdef CONFIG_W90N745FLASH

/* W28J800TT */
INT BlockSize_W28J800TT(UINT32 address);
INT BlockSize_W28J800BT(UINT32 address);
INT BlockErase_W28J800TT(UINT32 address,UINT32 size);
INT BlockWrite_W28J800TT(UINT32 address, UCHAR * data, UINT32 size);
INT ReadPID_W28J800TT(UINT32 address, UCHAR *PID0, UCHAR *PID1);
INT BlockLock_W28J800TT(UINT32 address, UINT32 op);
/* W28J160TT */
INT BlockSize_W28J160TT(UINT32 address);
INT ReadPID_W28J160TT(UINT32 address, UCHAR *PID0, UCHAR *PID1);
/* W28J160BT */
INT BlockSize_W28J160BT(UINT32 address);
/* W28J320TT */
INT BlockSize_W28J320TT(UINT32 address);
INT ReadPID_W28J320TT(UINT32 address, UCHAR *PID0, UCHAR *PID1);
/* W28J320BT */
INT BlockSize_W28J320BT(UINT32 address);


/* W39L010 */
INT BlockSize_W39L010(UINT32 address);
INT BlockErase_W39L010(UINT32 address,UINT32 size);
INT BlockWrite_W39L010(UINT32 address, UCHAR * data, UINT32 size);
INT ReadPID_W39L010(UINT32 address, UCHAR *PID0, UCHAR *PID1);
INT BlockLock_W39L010(UINT32 address, UINT32 op);
/* W29EE011 */
INT BlockSize_W29EE011(UINT32 address);
INT BlockErase_W29EE011(UINT32 address,UINT32 size);
INT BlockWrite_W29EE011(UINT32 address, UCHAR * data, UINT32 size);
INT ReadPID_W29EE011(UINT32 address, UCHAR *PID0, UCHAR *PID1);
INT BlockLock_W29EE011(UINT32 address, UINT32 op);
/* AM29LV800BB */
INT BlockSize_AM29LV800BB(UINT32 address);
INT BlockErase_AM29LV800BB(UINT32 address,UINT32 size);
INT BlockWrite_AM29LV800BB(UINT32 address, UCHAR * data, UINT32 size);
INT ReadPID_AM29LV800BB(UINT32 address, UCHAR *PID0, UCHAR *PID1);
INT BlockLock_AM29LV800BB(UINT32 address, UINT32 op);
/* AM29LV800BB */
INT ReadPID_AM29LV160DB(UINT32 address, UCHAR *PID0, UCHAR *PID1);
INT BlockSize_AM29LV160DB(UINT32 address);
/* AM29LV320DB */
INT ReadPID_AM29LV320DB(UINT32 address, UCHAR *PID0, UCHAR *PID1);
INT BlockSize_AM29LV320DB(UINT32 address);
/* Intel E28F640 */
INT BlockSize_E28F640(UINT32 address);
INT BlockErase_E28F640(UINT32 address,UINT32 size);
INT BlockWrite_E28F640(UINT32 address, UCHAR * data, UINT32 size);
INT ReadPID_E28F640(UINT32 address, UCHAR *PID0, UCHAR *PID1);
INT BlockLock_E28F640(UINT32 address, UINT32 op);
/* SST 39VF160 */
INT BlockSize_SST39VF160(UINT32 address);
INT ReadPID_SST39VF160(UINT32 address, UCHAR *PID0, UCHAR *PID1);
INT BlockErase_SST39VF160(UINT32 address,UINT32 size);

flash_t flash[]={
{0xDA,0x7E,"W19B320ABT",BlockSize_AM29LV320DB,BlockErase_AM29LV800BB,BlockWrite_AM29LV800BB,ReadPID_AM29LV320DB,BlockLock_AM29LV800BB},
{0xB0,0xEC,"W28J800TT",BlockSize_W28J800TT,BlockErase_W28J800TT,BlockWrite_W28J800TT,ReadPID_W28J800TT,BlockLock_W28J800TT},
{0xB0,0xED,"W28J800BT",BlockSize_W28J800BT,BlockErase_W28J800TT,BlockWrite_W28J800TT,ReadPID_W28J800TT,BlockLock_W28J800TT},
{0xB0,0xE8,"W28J160TT",BlockSize_W28J160TT,BlockErase_W28J800TT,BlockWrite_W28J800TT,ReadPID_W28J160TT,BlockLock_W28J800TT},
{0xB0,0xE9,"W28J160BT",BlockSize_W28J160BT,BlockErase_W28J800TT,BlockWrite_W28J800TT,ReadPID_W28J160TT,BlockLock_W28J800TT},
{0xB0,0xE2,"W28J320TT",BlockSize_W28J320TT,BlockErase_W28J800TT,BlockWrite_W28J800TT,ReadPID_W28J320TT,BlockLock_W28J800TT},
{0xB0,0xE3,"W28J320BT",BlockSize_W28J320BT,BlockErase_W28J800TT,BlockWrite_W28J800TT,ReadPID_W28J320TT,BlockLock_W28J800TT},
{0xDA,0x31,"W39L010", BlockSize_W39L010, BlockErase_W39L010, BlockWrite_W39L010, ReadPID_W39L010 ,BlockLock_W39L010 },
{0xDA,0xC1,"W29EE011",BlockSize_W29EE011,BlockErase_W29EE011,BlockWrite_W29EE011,ReadPID_W29EE011,BlockLock_W29EE011},
{0x01,0x5B,"AM29LV800BB",BlockSize_AM29LV800BB,BlockErase_AM29LV800BB,BlockWrite_AM29LV800BB,ReadPID_AM29LV800BB,BlockLock_AM29LV800BB},
{0x01,0x49,"AM29LV160DB",BlockSize_AM29LV160DB,BlockErase_AM29LV800BB,BlockWrite_AM29LV800BB,ReadPID_AM29LV160DB,BlockLock_AM29LV800BB},
{0x01,0xF9,"AM29LV320DB",BlockSize_AM29LV320DB,BlockErase_AM29LV800BB,BlockWrite_AM29LV800BB,ReadPID_AM29LV320DB,BlockLock_AM29LV800BB},
{0xDA,0x2A,"W19B320SB",BlockSize_AM29LV320DB,BlockErase_AM29LV800BB,BlockWrite_AM29LV800BB,ReadPID_AM29LV320DB,BlockLock_AM29LV800BB},//mcli 2004-11-29 15:25
{0xBF,0x82,"SST39LV160",BlockSize_SST39VF160,BlockErase_SST39VF160,BlockWrite_AM29LV800BB,ReadPID_SST39VF160,BlockLock_AM29LV800BB},
{0x89,0x17,"INTEL E28F640",BlockSize_E28F640,BlockErase_E28F640,BlockWrite_E28F640,ReadPID_E28F640,BlockLock_E28F640},
{0xAD,0x49,"HY29LV160",BlockSize_AM29LV160DB,BlockErase_AM29LV800BB,BlockWrite_AM29LV800BB,ReadPID_AM29LV160DB,BlockLock_AM29LV800BB},
/* add your new flash' operation functions here */
};


int get_flash_type_num(void)
{
	return sizeof(flash)/sizeof(flash_t);	
}	

/* please assign your flash's size to _flash_size in your flash_t->ReadPID,
   for example
	INT ReadPID_XXX(...)
	{
		......
		_flash_size=0x100000;
	
		.......
		return 0;
	}
*/
#else
asmlinkage int sys_FindImage(UINT32 image_num, tfooter ** image_footer)
{
	return -ENOSYS;
}
asmlinkage int sys_DelImage(UINT32 image_num)
{
	return -ENOSYS;
}
asmlinkage int sys_CorruptCheck(tfooter * image_footer)
{
	return -ENOSYS;
}
asmlinkage int sys_ReadWinbondFlash(unsigned long pos, unsigned long length, char * buffer)
{
	return -ENOSYS;
}
asmlinkage int sys_WriteWinbondFlash(unsigned long pos, unsigned long length, char * buffer)
{
	return -ENOSYS;
}
asmlinkage int sys_WinbondFlashBlockSize(unsigned long pos)
{
	return -ENOSYS;
}
asmlinkage int sys_WinbondFlashTotalSize()
{
	return -ENOSYS;
}
asmlinkage int sys_WinbondFlashBase()
{
	return -ENOSYS;
}
#endif
