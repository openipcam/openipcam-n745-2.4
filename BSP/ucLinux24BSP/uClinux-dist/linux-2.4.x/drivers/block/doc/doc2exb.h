/*
 * $Log: doc2exb.h,v $
 * Revision 1.1.1.1  2006-07-11 09:28:42  andy
 * W90N745 BSP
 *
 * Revision 1.1.1.1  2006/04/26 02:35:48  andy
 * W90N745 uCLinux kernel
 *
 * 
 *    Rev 1.12   May 02 2002 19:55:42   oris
 * Seperated into public and private files in order to make public header files for binary drivers. 
 * 
 *    Rev 1.11   Apr 15 2002 07:35:22   oris
 * Make sure all relevant structures will allow easy little endian conversion.
 * 
 *    Rev 1.10   Feb 19 2002 20:58:38   oris
 * Moved include directive and routine  prototypes to C file.
 * 
 *    Rev 1.9   Jan 21 2002 20:44:12   oris
 * Added DiskOnChip Millennium Plus 16MB firmware family definition.
 * 
 *    Rev 1.8   Jan 17 2002 22:58:42   oris
 * Added INFTL_NEAR_HEAP_SIZE, FIRMWARE_NO_MASK, STRONG_ARM_IPL  definitions.
 * Removed different firmware STACK sizes.
 * Added parameter to getExbInfo() (firmware add to).
 * Added tffsFarHeapSize to exbStruct record.
 * 
 *    Rev 1.7   Jul 13 2001 01:01:06   oris
 * Added constant stack space for each of the different DiskOnChip.
 * 
 *    Rev 1.6   Jun 17 2001 08:17:24   oris
 * Changed placeExbByBuffer exbflags argument to word instead of byte to  support /empty flag.
 * Added LEAVE_EMPTY and EXB_IN_ROM flags.
 * 
 *    Rev 1.5   Apr 09 2001 15:05:14   oris
 * End with an empty line.
 * 
 *    Rev 1.4   Apr 03 2001 14:39:54   oris
 * Add iplMod512 and splMod512 fields to the exbStruct record.
 *
 *    Rev 1.3   Apr 02 2001 00:56:48   oris
 * Added EBDA_SUPPORT flag.
 * Bug fix of NO_PNP_HEADER flag.
 * Changed ifdef of h file from doc2hdrs_h to doc2exb_h.
 *
 *    Rev 1.2   Apr 01 2001 07:50:00   oris
 * Updated copywrite notice.
 * Changed LEmin to dword
 * Added DOC2300_FAMILY_FIRMWARE firmware types.
 *
 *    Rev 1.1   Feb 08 2001 10:32:06   oris
 * Seperated file signature into 2 fields signature and TrueFFS vesion to make it eligned
 *
 *    Rev 1.0   Feb 02 2001 13:10:58   oris
 * Initial revision.
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

/*****************************************************************************
* File Header                                                                *
* -----------                                                                *
* Project : TrueFFS source code                                              *
*                                                                            *
* Name : doc2exb.h                                                           *
*                                                                            *
* Description : M-Systems EXB firmware files and media definitions and       *
*               data structures                                              *
*                                                                            *
*****************************************************************************/

#ifndef DOC2EXB_H
#define DOC2EXB_H

/**********************************/
/* EXB file structure definitions */
/**********************************/

/* EXB Flag definitions */
#define INSTALL_FIRST     1
#define EXB_IN_ROM        2
#define QUIET             4
#define INT15_DISABLE     8
#define FLOPPY            0x10
#define SIS5598           0x20
#define EBDA_SUPPORT      0x40
#define NO_PNP_HEADER     0x80
#define LEAVE_EMPTY       0x100 
#define FIRMWARE_NO_MASK  0xd00 /* Up to 8 firmwares */
#define FIRMWARE_NO_SHIFT 9

/* Firmware types */
#define DOC2000_FAMILY_FIRMWARE      1
#define DOCPLUS_FAMILY_FIRMWARE      2
#define DOC2300_FAMILY_FIRMWARE      3
#define DOCPLUS_INT1_FAMILY_FIRMWARE 4

/*General definitions */
#define MAX_CODE_MODULES        6
#define ANAND_MARK_ADDRESS      0x406
#define ANAND_MARK_SIZE         2
#define EXB_SIGN_OFFSET         8
#define INVALID_MODULE_NO       0xff
#define SIGN_SPL                "Дима"    /* EXB binary signature */
#define SIGN_MSYS               "OSAK"    /* EXB file signature   */
#define SIGN_MSYS_SIZE          4

#include "docbdk.h"
#include "_doc2exb.h"

#endif /* DOC2EXB_H */
