/*
 * $Log: fllnx.h,v $
 * Revision 1.1.1.1  2006-07-11 09:28:45  andy
 * W90N745 BSP
 *
 * Revision 1.1.1.1  2006/04/26 02:35:50  andy
 * W90N745 uCLinux kernel
 *
 * 
 */

/***********************************************************************************/
/*                        M-Systems Confidential                                   */
/*           Copyright (C) M-Systems Flash Disk Pioneers Ltd. 1995-2001            */
/*                         All Rights Reserved                                     */
/***********************************************************************************/
/*                            NOTICE OF M-SYSTEMS OEM                              */
/*                           SOFTWARE LICENSE AGREEMENT                            */
/*                                                                                 */
/*      THE USE OF THIS SOFTWARE IS GOVERNED BY A SEPARATE LICENSE                 */
/*      AGREEMENT BETWEEN THE OEM AND M-SYSTEMS. REFER TO THAT AGREEMENT           */
/*      FOR THE SPECIFIC TERMS AND CONDITIONS OF USE,                              */
/*      OR CONTACT M-SYSTEMS FOR LICENSE ASSISTANCE:                               */
/*      E-MAIL = info@m-sys.com                                                    */
/***********************************************************************************/

/*
 * M-Systems supports access to their DiskOnChip (DOC) 2000 devices with
 * their O/S Adaptation Kit (OSAK).
 *
 * OSAK is proprietary and available only to to qualified developers under 
 * license from M-Systems.
 *
 * M-Systems also makes available a compiled versions of the OSAK suitable
 * for linking into various operating systems.
 *
 * The Linux Kit consists of five files:
 *
 *      README.DOC
 *      Makefile
 *      fldrvlnx.c
 *      fllnx.h
 *      libosak.a
 *
 *
 * This file, fllnx.h defines a simple interface to a version of the code
 * distributed as libosak.a. 
 * 
 * The code in libosak.a can be linked into a kernel driver module 
 * for use under linux. 
 * 
 * See the fldrvlnx.c file for the linux driver implementation.
 *
 *
 * Initialize the M-Systems OSAK library.
 *
 *      int fl_doc_init()
 *
 * Mount (make available for use) a DOC device.
 *
 *      int fl_doc_mount(int vol);
 *
 * Dismount a DOC device.
 *
 *      int fl_doc_dismount(int vol);
 *
 * Count the number of available DOC devices.
 *
 *      int fl_doc_count(int max);
 *
 * Return the number of sectors in a DOC device.
 *
 *      int fl_doc_sectors(int vol);
 *
 * Read a sector from a DOC device.
 *
 *      int fl_doc_read(int vol, void *data, int block, int count);
 *
 * Write a sector to a DOC device.
 *
 *      int fl_doc_write(int vol, void *data, int block, int count);
 *
 */

#ifndef ___FLLNX__H__
#define ___FLLNX__H__

#include <linux/types.h>	/* for size_t */

#define FLLNX_VERSION "5_0-000"

int fl_doc_init(void);
int fl_doc_mount(int VOL);
int fl_doc_dismount(int VOL);
int fl_doc_count(int max);
unsigned char fl_partition_count(int max);
int fl_doc_sectors(int VOL);
int fl_protType(int VOL);
int fl_doc_read(int VOL, void *data, int block, int count);
int fl_doc_write(int VOL, void *data, int block, int count);

/*
 * M-Systems OSAK requires certain OS based functions. These are defined here
 * and implemented in fldrvlnx.c so that they are compiled with the correct linux
 * kernel definitions.
 */

void flDelayMsecs(unsigned );
void flsleep(unsigned long );

extern void *   flmemcpy(void * dest,const void  *src,size_t count);
extern void *   flmemset(void * dest,int cval,size_t count);
extern int    flmemcmp(const void * dest,const void  *src,size_t count);

void *flmemcpy_fromio(void *, const void *, unsigned int);
void *flmemcpy_toio(void *, const void *, unsigned int);
void * flmemset_io(void *, int , unsigned int );
void * flkmalloc(unsigned long);
void flkfree(void *);
void * flvmalloc(unsigned long);
void flvfree(void *);
int flprintk(unsigned char fDebug,const char *fmt, ...);
unsigned char flreadb(volatile void *);
void flwriteb(unsigned value, volatile void *);
unsigned short flreadw(volatile void *);
void flwritew(unsigned value, volatile void *);

int fl_doc_ioctl(int bdtlVolume, int cmd,unsigned long arg);

void flGetEnvVarFromParam(unsigned char*flUseNFTLCachePar,unsigned int*flPolicyPar,
	unsigned char*flUseisRAMPar,unsigned int*flUseMultiDocPar,unsigned char*flUse8BitPar);
unsigned long flGetWinL(void);
unsigned long flGetWinH(void);
unsigned char flRemoveProtectionFromParm(unsigned char bPart,unsigned char*pKey);

#endif	/* ___FLLNX__H__ */
