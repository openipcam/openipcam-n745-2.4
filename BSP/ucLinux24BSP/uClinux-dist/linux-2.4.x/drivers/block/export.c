#include <asm/arch/flash.h>
#include <linux/slab.h>

#ifndef CONFIG_WBFLASH//CONFIG_W90N745FLASH

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
