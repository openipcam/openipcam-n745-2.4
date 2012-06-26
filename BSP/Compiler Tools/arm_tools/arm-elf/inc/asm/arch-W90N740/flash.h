#ifndef FLASH_H
#define FLASH_H
//---------------------------------------------------------------------------
#include <asm/arch/cdefs.h>
#include <asm/arch/bib.h>
#include <asm/arch/firmware.h>


// if no platform.h
#ifndef FLASH_BASE
#define FLASH_BASE 			(0x7F000000)
#define FLASH_BLOCK_SIZE	(0x10000)
#endif

#ifndef ROMCON
#define ROMCON	(0xFFF01004)
#endif


#define FLASH_NAME_SIZE	16
#define FLASH_TYPE_NUM	5

#define inpw(port)	*((volatile unsigned int *)port)
#define outpw(port,x) *((volatile unsigned int *)port)=x
#define inph(port)	*((volatile unsigned short *)port)
#define outph(port,x) *((volatile unsigned short *)port)=x
#define inpb(port)	*((volatile unsigned char *)port)
#define outpb(port,x) *((volatile unsigned char *)port)=x


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
extern flash_t flash[FLASH_TYPE_NUM];// The supported flash types
extern int flash_type;

extern int FindFlash(void); // function to identify the flash type
extern int FindFooter(tfooter *** image_footer); //function to find the image footers
int sys_DelImage(UINT32 image_num);
int sys_FindImage(UINT32 image_num, tfooter ** image_footer);
int WriteImage(tfooter * image_footer, UINT32 image_source);
int sys_WriteFirmWare(void *filename,int *len);
extern int sys_CorruptCheck(tfooter * image_footer);//function to check if data corrupt
extern int DelBlock(UINT32 block);// function to delete a block size of flash memory
extern tbl_info info;
extern int GetImgAddr(void);
extern int GetLoadImage();

//---------------------------------------------------------------------------
#endif
