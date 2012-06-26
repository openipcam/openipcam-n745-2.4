#ifndef CDEFS_H
#define CDEFS_H

#define AHBBASE	0xFFF00000
#define FLASH_BASE	(0x7F000000)
#define ROMCON	(AHBBASE + 0x1004)
#define W90N740_FLASH_SIZE	(0x40000<<(( *((UINT *)ROMCON)>>16 )&0x7))
#define FLASH_BLOCK_SIZE	(0x10000)
#define FLASH_SIZE	(W90N740_FLASH_SIZE)
//------------------------------------------------------------------------------
typedef int INT;
typedef int SIGNED;
typedef unsigned int UNSIGNED;
typedef unsigned int UINT;
typedef unsigned int UINT32;
typedef unsigned char UCHAR;
typedef char CHAR;
typedef void VOID;
typedef unsigned long ULONG;
#define NULL	0

//------------------------------------------------------------------------------
#endif
