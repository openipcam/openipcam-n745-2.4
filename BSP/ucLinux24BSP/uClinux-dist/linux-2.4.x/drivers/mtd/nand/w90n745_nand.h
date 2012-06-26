/*
 *  linux/drivers/mtd/nand/w99702g_nand.h
 *
 *  Copyright (c) 2005  Winbond (http://www.winbond.com.tw)
 *
 * History:
 *	2006/08/24		Created by NS24 zswan
 */
 #define EBI_BANK2
 //#define EBI_BANK3
#ifdef EBI_BANK2
#define EXT2CON			0xFFF01020
#else
#define EXT3CON			0xFFF01024
#endif
#define EBI_BASE_ADDR	0xF0000000

#define outpb(port,value)     (*((unsigned char volatile *) (port))=value)
#define inpb(port)            (*((unsigned char volatile *) (port)))
#define outpw(port,value)     (*((unsigned int volatile *) (port))=value)
#define inpw(port)            (*((unsigned int volatile *) (port)))

#define REG_SMCMD		(EBI_BASE_ADDR + 0x04)
#define REG_SMADDR		(EBI_BASE_ADDR + 0x08)
#define REG_SMDATA		(EBI_BASE_ADDR + 0x0)
#ifdef EBI_BANK2
#define Setup_EXTIO2_Base()		(*((unsigned int volatile *)(EXT2CON))=0xE0004491);
#else
#define Setup_EXTIO3_Base()		(*((unsigned int volatile *)(EXT3CON))=0xE0004491);
#endif
 
