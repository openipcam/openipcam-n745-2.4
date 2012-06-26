/*
 * $Log: _docbdk.h,v $
 * Revision 1.1.1.1  2006-07-11 09:28:42  andy
 * W90N745 BSP
 *
 * Revision 1.1.1.1  2006/04/26 02:35:48  andy
 * W90N745 uCLinux kernel
 *
 * 
 *    Rev 1.0   May 14 2002 14:59:14   oris
 * Initial revision.
 */

/***************************************************************************/
/*                  M-Systems Confidential                                 */
/*       Copyright (C) M-Systems Flash Disk Pioneers Ltd. 1995-2001        */
/*                     All Rights Reserved                                 */
/***************************************************************************/
/*                         NOTICE OF M-SYSTEMS OEM                         */
/*                        SOFTWARE LICENSE AGREEMENT                       */
/*                                                                         */
/*   THE USE OF THIS SOFTWARE IS GOVERNED BY A SEPARATE LICENSE            */
/*   AGREEMENT BETWEEN THE OEM AND M-SYSTEMS. REFER TO THAT AGREEMENT      */
/*   FOR THE SPECIFIC TERMS AND CONDITIONS OF USE,                         */
/*   OR CONTACT M-SYSTEMS FOR LICENSE ASSISTANCE:                          */
/*   E-MAIL = info@m-sys.com                                               */
/***************************************************************************/
/************************************************************************/
/* Caution: The BDK_ACCESS compilation flag is for M-SYSTEMS internal   */
/*          use ONLY. This flag is used by M-SYSTEMS drivers and        */
/*          therfore it is SHOULD NOT be used by this package           */
/************************************************************************/

#ifndef _DOCBDK_H
#define _DOCBDK_H

/*---------------------------------------------------------------------
 *
 *       Binary Development Kit Stand Alone Customization Area
 *
 *---------------------------------------------------------------------*/

/*---------------------------------------------------------------------*/
/* Boundries of the memory location to look for the DiskOnChip         */
/*---------------------------------------------------------------------*/

#define DOC_LOW_ADDRESS                 0xC8000L
#define DOC_HIGH_ADDRESS                0xE0000L

/*----------------------- Mtd selection -------------------------------
 *
 * Uncomment the following uneeded MTD or TL to reduce code size.
 *
 *---------------------------------------------------------------------*/

/* DiskOnChip2000, DiskOnChip Millennium and DiskOnChip 2000 Tsop devices */
/* #define NO_DOC2000_FAMILY_SUPPORT */

/* DiskOnChip Millennium plus device */
/* #define NO_DOCPLUS_FAMILY_SUPPORT */

/* NFTL format - DiskOnChip2000 and DiskOnChip Millennium */
/* #define NO_NFTL_SUPPORT */

/* INFTL format - DiskOnChip2000 Tsop and DiskOnChip Millennium Plus */
/* #define NO_INFTL_SUPPORT */

/*---------------------------------------------------------------------
 *
 *     End of Binary Development Kit Stand Alone Customization Area
 *
 *---------------------------------------------------------------------*/

/*---------------------------------------------------------------------*/
/* general constant                                                    */
/*---------------------------------------------------------------------*/

#define MBYTE                           0x100000L
#define KBYTE                           0x400
#define BLOCK                           0x200

#define BDK_MIN(a,b)   ((a) < (b) ? (a) : (b))

/*---------------------------------------------------------------------*/
/* The maximum number of binary partitions                             */
/*---------------------------------------------------------------------*/

#ifndef BINARY_PARTITIONS
#define BINARY_PARTITIONS  SOCKETS /* for backwards compatibility */
#endif

/*-------------------------- BDK Global Status Values --------------------*/
#define BDK_S_INIT          0    /* uninitialized binary partition record */
#define BDK_S_DOC_FOUND     0x01 /* DiskOnChip device was found           */
#define BDK_S_HEADER_FOUND  0X04 /* Partition information was found       */
#define BDK_S_INFO_FOUND    0x08 /* Sub partition information was found   */
/*------------------------------------------------------------------------*/

#if defined(BDK_ACCESS) || defined(MTD_STANDALONE)

/*------------------------------------------------------------------------*/
/* Global binary partition data structures                                */
/*------------------------------------------------------------------------*/

typedef struct { 

   byte  bdkGlobalStatus;            /* BDK global status variable         */
   byte  bdkEDC;                     /* ECC mode flag                      */
   byte  bdkSignOffset;              /* BDK signature offset ( 0 or 8)     */
   byte  bdkSavedSignOffset;         /* signature offset of last access    */
   word  bdkSavedStartUnit;          /* starting unit of last access       */
   word  startPartitionBlock, endPartitionBlock;   /* partition boundries  */
   word  startImageBlock, endImageBlock;       /* sub partition boundries  */
   word  curReadImageBlock;          /* current block number to read from  */
   word  blockPerFloor;              /* Blocks per floor                   */
   byte  signBuffer[SIGNATURE_LEN];  /* signature of binary sub partition  */
   dword bootImageSize;            /* available sub binary partition size  */
   dword realBootImageSize;  /* size used by an image on the sub partition */
   dword actualReadLen;              /* length needed to be read           */
   dword bdkDocWindow;               /* DiskOnChip window start address    */
   CardAddress curReadImageAddress;  /* current address to read from       */
#ifdef UPDATE_BDK_IMAGE
   CardAddress curUpdateImageAddress; /* current address to write to       */
   dword actualUpdateLen;      /* length needed to be write                */
   word  curUpdateImageBlock;  /* current block number to write to         */
   byte  updateImageFlag;/* BDK_COMPLETE_IMAGE_UPDATE \ ERASE_BEFORE_WRITE */
#endif /* UPDATE_BDK_IMAGE */
#ifdef PROTECT_BDK_IMAGE
   byte  protectionArea;       /* protection area no protecting the volume */
   word  protectionType;       /* PROTECTABLE , CHANGEABLE_PROTECTION      */
#endif /* PROTECT_BDK_IMAGE */

   byte  erasableBlockBits;    /* number of bits used to represen a block  */
   FLFlash * flash;            /* flash record representing the media      */

} BDKVol;

/*------------------------------------------------------------------------*/
/* Extern variables for low level operations.                             */
/*------------------------------------------------------------------------*/

extern BDKVol*  bdkVol;

/*------------------------------------------------------------------------*/
/* Diffrent records used by the media header                              */
/*------------------------------------------------------------------------*/

typedef struct {
  LEulong    virtualSize;  /* Virtual size exported by the trasnaltion layer */
  LEulong    firstUnit;    /* First erasable block of the partition */
  LEulong    lastUnit;     /* Last erasable block of the partition */
  LEulong    flags;        /* PROTECTABLE , BDK_BINARY_FLAG */
  LEulong    not_used1;
  LEulong    not_used2;
  LEulong    protectionArea; /* protection area no' */
} VolumeRecord;

/************************ Function Prototype Begin ************************/

#ifdef MTD_STANDALONE

/*************************/
/* BDK specific routines */
/*************************/

void     bdkExit                 (void);
void     bdkSetDocWindow         (CardAddress docWindow);
FLStatus bdkSetBootPartitionNo   (byte partitionNo);
FLStatus bdkFindDiskOnChip       (CardAddress FAR2 *docAddress,
                 dword FAR2 *docSize );
FLStatus bdkCheckSignOffset      (byte FAR2 *signature );
FLStatus bdkCopyBootArea         (byte FAR1 *startAddress,
                 word startUnit,
                 dword areaLen,
                 byte FAR2 *checkSum,
                 byte FAR2 *signature);

/**************************************************/
/* common functions which are exported by the BDK */
/**************************************************/

FLStatus bdkGetBootPartitionInfo (word startUnit,
                 dword FAR2 *partitionSize,
                 dword FAR2 *realPartitionSize,
                 dword FAR2 *unitSize,
                 byte FAR2 *signature);

FLStatus bdkCopyBootAreaInit     (word startUnit,
                 dword areaLen,
                 byte FAR2 *signature);
FLStatus bdkCopyBootAreaBlock    (byte FAR1 *buf ,
                 word bufferLen,
                 byte FAR2 *checkSum);

#ifdef BDK_IMAGE_TO_FILE

FLStatus bdkCopyBootAreaFile     ( char FAR2 *fname,
                   word startUnit,
                   dword areaLen,
                   byte FAR2 *checkSum,
                   byte FAR2 *signature );
#endif /* BDK_IMAGE_TO_FILE */

#ifdef UPDATE_BDK_IMAGE

FLStatus bdkUpdateBootAreaInit   (word  startUnit,
                 dword  areaLen,
                 byte updateFlag,
                 byte FAR2 *signature );
FLStatus bdkUpdateBootAreaBlock  (byte FAR1 *buf ,
                 word bufferLen );

#ifdef ERASE_BDK_IMAGE
FLStatus bdkEraseBootArea        (word startUnit,
                 word noOfBlocks,
                 byte FAR2 * signature);
#endif /* ERASE_BDK_IMAGE */
#ifdef CREATE_BDK_IMAGE
FLStatus bdkCreateBootArea       (word noOfBlocks,
                 byte FAR2 * oldSign,
                 byte FAR2 * newSign);
#endif /* CREATE_BDK_IMAGE */

#ifdef HW_OTP
FLStatus bdkGetUniqueID(byte FAR1* buf);
FLStatus bdkReadOtp(word offset,byte FAR1 * buffer,word length);
FLStatus bdkWriteAndLockOtp(const byte FAR1 * buffer,word length);
FLStatus bdkGetOtpSize(dword FAR2* sectionSize, dword FAR2* usedSize,
               word FAR2* locked);
#endif /* HW_OTP */

#ifdef BDK_IMAGE_TO_FILE

FLStatus bdkUpdateBootAreaFile(char FAR2 *fname, word startUnit,
                   dword areaLen, byte FAR2 *signature);
#endif /* BDK_IMAGE_TO_FILE */

#endif /* UPDATE_BDK_IMAGE */

#ifdef PROTECT_BDK_IMAGE

FLStatus bdkGetProtectionType    (word * protectionType);

FLStatus bdkSetProtectionType    (word newType);

FLStatus bdkInsertKey            (byte FAR1* key);

FLStatus bdkRemoveKey            (void);

FLStatus bdkLockEnable           (byte enable);

FLStatus bdkChangeKey            (byte FAR1* key);

#endif /* PROTECT_BDK_IMAGE */

#else /* MTD_STANDALONE */

extern FLStatus bdkCall(FLFunctionNo functionNo,
                        IOreq FAR2 *ioreq, FLFlash* flash);

#endif /* MTD_STANDALONE */

/********************/
/* common functions */
/********************/

void     bdkInit( void );

/************************ Function Prototype End **************************/

#endif /* BDK_ACCESS || MTD_STANDALONE */
#endif /* _DOCBDK_H */




