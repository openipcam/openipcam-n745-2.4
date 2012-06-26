
/*
 * $Log: _doc2exb.h,v $
 * Revision 1.1.1.1  2006-07-11 09:28:45  andy
 * W90N745 BSP
 *
 * Revision 1.1.1.1  2006/04/26 02:35:51  andy
 * W90N745 uCLinux kernel
 *
 * 
 *    Rev 1.0   May 02 2002 19:58:24   oris
 * Initial revision.
 */


/*********************************************************************************** 
 *                                                                                 * 
 *                        M-Systems Confidential                                   * 
 *           Copyright (C) M-Systems Flash Disk Pioneers Ltd. 1995-2001            * 
 *                         All Rights Reserved                                     * 
 *                                                                                 * 
 *********************************************************************************** 
 *                                                                                 * 
 *                            NOTICE OF M-SYSTEMS OEM                              * 
 *                           SOFTWARE LICENSE AGREEMENT                            * 
 *                                                                                 * 
 *      THE USE OF THIS SOFTWARE IS GOVERNED BY A SEPARATE LICENSE                 * 
 *      AGREEMENT BETWEEN THE OEM AND M-SYSTEMS. REFER TO THAT AGREEMENT           * 
 *      FOR THE SPECIFIC TERMS AND CONDITIONS OF USE,                              * 
 *      OR CONTACT M-SYSTEMS FOR LICENSE ASSISTANCE:                               * 
 *      E-MAIL = info@m-sys.com                                                    * 
 *                                                                                 * 
 ***********************************************************************************
 *                                                                                 * 
 * Project     : TrueFFS source code                                               *
 *                                                                                 *
 * Name        : doc2exb.h                                                         *
 *                                                                                 *
 * Description : M-Systems EXB firmware files and media definitions and dats       *
 *               data structures                                                   *
 *                                                                                 * 
 ***********************************************************************************/



#ifndef _DOC2EXB_H
#define _DOC2EXB_H

#include "docbdk.h"

#ifdef BDK_ACCESS
extern BDKVol*  bdkVol;         /* pointer to current binary partition */
#endif

/* Firmware stack constant */
#ifdef VERIFY_WRITE
#define INFTL_NEAR_HEAP_SIZE sizeof(FLBuffer)+(SECTOR_SIZE<<1)+READ_BACK_BUFFER_SIZE
#else
#define INFTL_NEAR_HEAP_SIZE sizeof(FLBuffer)+(SECTOR_SIZE<<1)
#endif /* VERIFY_WRITE */
#define DEFAULT_DOC_STACK 2*1024


/* File specific record specifing data for all firmwares in the file */
typedef struct {
  byte  mSysSign[SIGN_MSYS_SIZE];       /* identification signature */
  byte  osakVer[SIGN_MSYS_SIZE];        /* identification signature */
  LEmin fileSize;                       /* Total file size */
  LEmin noOfFirmwares;                  /* Number of firmware supported by this file */
} ExbGlobalHeader;


/* File specific record specifing data for a specific firmwares in the file */
typedef struct {
  LEmin type;                           /* Firmware type (must fit the H/W)  */
  LEmin startOffset;                    /* Firmware start offset in the file */
  LEmin endOffset;                      /* Firmware end offset in the file   */
  LEmin splStartOffset;                 /* SPL start offset in the file      */
  LEmin splEndOffset;                   /* SPL end offset in the file        */
} FirmwareHeader;


/* data structure representing BIOS extention header */
typedef struct{
  unsigned char  signature[2]; /* BIOS extention signature (0xAA55) */
  unsigned char  lenMod512; /* length in unsigned chars modulo 512 */
} BIOSHeader;


/* data structure representing IPL header */
typedef struct{
  BIOSHeader     biosHdr;
  byte           jmpOpcode[3];     /* jmp start_of_code                      */
  byte           dummy;            /* dummy byte                             */
  byte           msysStr[17];      /* ORG  7h ManStr DB '(C)M-Systems1998',0 */
  word           pciHeader;        /* ORG 18h   ; PCI header                 */
  word           pnpHeader;        /* ORG 1Ah   ; P&P header                 */
  byte           dummy0[4];        /* Actual address must be shifted by 4 '0'*/
  LEushort       windowBase;       /* ORG 20h   ; explicit DOC window base   */
  Unaligned      spl_offset;       /* DFORMAT !!!                            */
  Unaligned      spl_size;         /* spl actual size                        */
  byte           spl_chksum;       /* 55                                     */
} IplHeader;


/* data structure representing SPL header */
typedef struct{
  unsigned char  jmpOpcode[2];
  BIOSHeader     biosHdr;
      /* Note: At run-time biosHdr.lenMod512 contains size of entire DOC 2000
      boot area modulo 512 as set by DFORMAT  */
  Unaligned      runtimeID;        /* filled in by DFORMAT  */
  Unaligned      tffsHeapSize;     /* filled in by DFORMAT  */
  unsigned char  chksumFix;        /* changed by DFORMAT */
  unsigned char  version;
  unsigned char  subversion;
  char           copyright[29];    /* "SPL_DiskOnChip (c) M-Systems", 0 */
  Unaligned      windowBase;       /* filled in by DFORMAT */
  Unaligned4     exbOffset;        /* filled in by DFORMAT */
} SplHeader;


/* data structure representing TFFS header */
typedef struct{
  BIOSHeader     biosHdr;
  unsigned char  jmpOpcode[3];
  char           tffsId[4];         /* "TFFS" */
  unsigned char  exbFlags;          /* filled in by writeExbDriverImage() */
  Unaligned      heapLen;           /* not used for now */
  Unaligned      windowBase;        /* passed by SPL and saved here */
  unsigned char  chksumFix;         /* changed by writeExbDriverImage() */
  Unaligned      runtimeID;         /* passed by SPL and saved here */
  unsigned char  firstDiskNumber;   /* filled in............  */
  unsigned char  lastDiskNumber;    /* ..........at run-time */
  Unaligned      versionNo;         /* filled in at run-time */
} TffsHeader;


/* data structure representing Socket Services  header */
typedef struct{
  BIOSHeader     biosHdr;
  unsigned char  jmpOpcode[3];
  char           tffsId[4];         /* "TFFS" */
  unsigned char  exbFlags;          /* filled in by writeExbDriverImage() */
  unsigned char  heapLen;           /* not used for now */
  Unaligned      windowBase;        /* filled in at run-time */
  unsigned char  chksumFix;         /* changed by writeExbDriverImage() */
} SSHeader;


/* Work space for writting the exb file */
typedef struct{
  word  exbFlags;            /* For the complete list see doc2hdrs.h     */
  word  iplMod512;           /* Size of the IPL module divided by 512    */
  word  splMod512;           /* Size of the SPL module divided by 512    */
  dword splMediaAddr;        /* Start of the SPL module media address    */
  dword ssMediaAddr;         /* Start of the SS module media address     */
  dword exbRealSize;         /* Actual binary area + bad blocks          */
  word  moduleLength;        /* Length of the modules in divided by 512  */
  dword tffsHeapSize;        /* TFFS needed heap size                    */
  word  tffsFarHeapSize;     /* TFFS needed far heap size                */
  word  bufferOffset;        /* Curret Offset inside the internal buffer */
  dword exbFileEnd;          /* Offset of the last Byte of the files     */
  dword exbFileOffset;       /* Current Offset inside the EXB file       */
  dword splStart;            /* First SPL byte offset                    */
  dword splEnd;              /* Last SPL byte offset                     */
  dword firmwareEnd;         /* End offset of the specific firmware      */
  dword firmwareStart;       /* Start offset of the specific firmware    */
  FLBuffer *buffer;          /* Internal 512 byte buffer                 */
} exbStruct;

#endif /* _DOC2EXB_H */

