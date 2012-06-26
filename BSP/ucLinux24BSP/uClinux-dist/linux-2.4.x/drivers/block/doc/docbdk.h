/*
 * $Log: docbdk.h,v $
 * Revision 1.1.1.1  2006-07-11 09:28:42  andy
 * W90N745 BSP
 *
 * Revision 1.1.1.1  2006/04/26 02:35:50  andy
 * W90N745 uCLinux kernel
 *
 * 
 *    Rev 1.14   May 14 2002 15:04:46   oris
 * Separated private definitions to _docbdk.h
 * 
 *    Rev 1.13   Apr 15 2002 07:35:48   oris
 * Moved bdkCall to blockdev.h
 * Added include for flreq.h and flfuncno.h when BDK_ACCESS is defined.
 * 
 *    Rev 1.12   Feb 19 2002 20:58:56   oris
 * Removed flflash.h include directive.
 * Moved bdkCall prototype to blockdev.
 * 
 *    Rev 1.11   Nov 08 2001 10:45:10   oris
 * Moved BDK module ifdef statement in order to allow the use of basic definitions.
 * 
 *    Rev 1.10   Oct 18 2001 22:17:12   oris
 * Added number of blocks per floor in the bdkVol structure.
 * 
 *    Rev 1.9   Oct 10 2001 19:48:28   oris
 * More afficient way to store the Binary module internal structure (bdkVol).
 * 
 *    Rev 1.8   May 20 2001 14:35:00   oris
 * Removed mtdsa.h include file.
 * 
 *    Rev 1.7   May 17 2001 16:51:08   oris
 * Removed warnings.
 * 
 *    Rev 1.6   May 16 2001 21:17:12   oris
 * Added OTP routines declaration.
 * Removed redefinitions of SOCKETS and BINARY_PARTITIONS.
 * Changed variable types to TrueFFS standard types.
 * Added EXTRA_SIZE definition and removed SYNDROM_BYTES.
 * 
 *    Rev 1.5   May 09 2001 00:32:14   oris
 * Removed the DOC2000_FAMILY and DOCPLUS_FAMILY defintion and replaced it with NO_DOC2000_FAMILY_SUPPORT, NO_DOCPLUS_FAMILY_SUPPORT, NO_NFTL_SUPPORT and NO_INFTL_SUPPORT.
 * Added BINARY_PARTITIONS and SOCKETS defintions.
 * 
 *    Rev 1.4   Apr 30 2001 17:59:38   oris
 * Changed bdkSetBootPartitonNo, bdkGetProtectionType, bdkSetProtection prototypes
 * 
 *    Rev 1.3   Apr 16 2001 13:32:02   oris
 * Removed warrnings.
 * 
 *    Rev 1.2   Apr 09 2001 15:06:18   oris
 * End with an empty line.
 * 
 *    Rev 1.1   Apr 01 2001 07:50:38   oris
 * Updated copywrite notice.
 * Removed nested comments.
 * Changed #include "base2400.h" to "mdocplus.h"
 * Fix for Big endien compilation problems - changed LEmin to LEulong
 * Changed MULTIPLIER_OFFSET define.
 * Changed protectionType to word instead of unsigned.
 * Added extern prototype of bdkVol pointer.
 *
 *    Rev 1.0   Feb 02 2001 13:24:56   oris
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

/*****************************************************************************
* File Header                                                                *
* -----------                                                                *
* Name : docbdk.h                                                            *
*                                                                            *
* Description : This file contains the binary partition defintions , data    *
*               structures and function prototypes.                          *
*                                                                            *
* Note : The file exports 2 interfaces each under its own compilation flag:  *
*                                                                            *
*        BDK package - Standalone package that exports routines for binary   *
*                      partitions handling(MTD_STANDALONE compilation flag). *
*        OSAK module - Separated module of the OSAK package that exports a   *
*                      common entry point to the same routines. (BDK_ACCESS  *
*                      compilation flag).                                    *
*                                                                            *
* Warning : Do not use this file with the BDK_ACCESS compilation flag unless *
*           you own the full OSAK package.                                   *
*****************************************************************************/

#ifndef DOCBDK_H
#define DOCBDK_H

/*---------------------------------------------------------------------*/
/* Include the proper header files.                                    */
/*---------------------------------------------------------------------*/

#include "nanddefs.h"  /* The MTD for the doc2000 and millennium DiskOnChips */

#ifdef BDK_ACCESS
#include "flfuncno.h"
#include "flreq.h"
#endif /* BDK_ACCESS */



/* general constants */

#define MAX_BINARY_PARTITIONS_PER_DRIVE 3
#define SIGNATURE_LEN                   8
#define BDK_SIGNATURE_NAME              4
#define SIGNATURE_NUM                   4
#define BDK_SIGN_OFFSET                 8
#define ANAND_LEN                       5
#define BDK_COMPLETE_IMAGE_UPDATE       16
#define BDK_PARTIAL_IMAGE_UPDATE        0

#define MULTIPLIER_OFFSET               5
#define BDK_INVALID_VOLUME_HANDLE       0xff
#define BDK_HEADERS_SPACING             (SECTOR_SIZE * 4)
#define BDK_UNIT_BAD                    0
#define BDK_NO_OF_MEDIA_HEADERS         2
#define BDK_FIELDS_BEFORE_HEADER        9 /* number of LEmin fieldsr to skip
                                             to reach  the volume records */
#define BDK_HEADER_FIELDS              35 /* number of LEmin fields used for
                                             volumes infromation record */
#define BDK_BINARY_FLAG       0x20000000L /* flag representing a binary volume
                                             in the volume information record */
/* BDK specific flag area */
#define ERASE_BEFORE_WRITE 8
#define EXTRA_SIZE         16

#ifdef BDK_ACCESS

/*------------------------------------------------------------------------*/
/* OSAK Routines argument packet                                          */
/*------------------------------------------------------------------------*/

typedef struct {
    byte       oldSign[BDK_SIGNATURE_NAME];
    byte       newSign[BDK_SIGNATURE_NAME];
    byte       signOffset;
    dword      startingBlock;
    dword      length;
    byte       flags;
    byte FAR1 *bdkBuffer;
} BDKStruct;

#endif /* BDK_ACCESS */


#include "_docbdk.h"

#endif /* DOCBDK_H */




