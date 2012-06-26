#ifndef FLASH_H
#define FLASH_H
//---------------------------------------------------------------------------
#include <asm/arch/cdefs.h>
#include <asm/arch/bib.h>


// if no platform.h
#ifndef FLASH_BASE
#define FLASH_BASE 			(0x7F000000)
#define FLASH_BLOCK_SIZE	(0x10000)
#endif

#ifndef ROMCON
#define ROMCON	(0xFFF01004)
#endif

#define EXT3CON (0xFFF01024)

#define FLASH_NAME_SIZE	16

#define VP_int(x)	(*((volatile unsigned int*)(x)))
#define VP_short(x)	(*((volatile unsigned short*)(x)))
#define VP_char(x)	(*((volatile unsigned char*)(x)))
#define CAHCNF_R		VP_int(0xFFF02000)
#define CAHCON_R		VP_int(0xFFF02004)

#if 0
#define inph(x)		VP_short(x)
#define outph(x,y)	VP_short(x)=(y)
#define inpb(x)		VP_char(x)
#define outpb(x,y)	VP_char(x)=(y)
#endif


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


#define BOOTER_BLOCK_LENGTH	FLASH_BLOCK_SIZE
extern flash_t flash[];// The supported flash types
extern int flash_type;
extern UINT32 _flash_size;
extern char * _flash_buffer;

extern int FindFlash(void); // function to identify the flash type
extern int FindFooter(tfooter *** image_footer); //function to find the image footers
int sys_DelImage(UINT32 image_num);
int sys_FindImage(UINT32 image_num, tfooter ** image_footer);
extern int sys_CorruptCheck(tfooter * image_footer);//function to check if data corrupt
extern tbl_info info;
extern int GetImgAddr(unsigned int default_addr);
extern int GetLoadImage();
extern int get_flash_type_num(void);
//---------------------------------------------------------------------------
#endif
