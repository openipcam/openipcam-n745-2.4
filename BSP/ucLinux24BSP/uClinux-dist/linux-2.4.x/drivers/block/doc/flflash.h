/*
 * $Log: flflash.h,v $
 * Revision 1.1.1.1  2006-07-11 09:28:42  andy
 * W90N745 BSP
 *
 * Revision 1.1.1.1  2006/04/26 02:35:50  andy
 * W90N745 uCLinux kernel
 *
 * 
 *    Rev 1.22   May 14 2002 15:06:42   oris
 * Moved all Boot SDK defintions (under MTD_STANDALONE compilation flag) to the privte _flflash.h file.
 * 
 *    Rev 1.21   03 May 2002 20:23:48   andreyk
 * typedef FLFlash definition moved from _FLFLASH.H to FLFLASH.H
 * 
 *    Rev 1.20   May 02 2002 19:56:02   oris
 * Seperated into public and private files in order to make public header files
 * for binary drivers.
 * 
 *    Rev 1.19   May 02 2002 09:41:34   oris
 * Changed memIntereleaveChanged to memSetGetMode and added the socket number as a parameter
 * 
 *    Rev 1.18   May 01 2002 19:03:24   oris
 * Added memInterleaveChanged field to flflash and added its prototype as a new typedef FLMemInterleaveChanged.
 * Placed memWindowSize under the FL_NO_USE_FUNC compilation flag.
 *
 *    Rev 1.17   Apr 15 2002 07:36:44   oris
 * Removed the use of NDOC2window  in access routine interface.
 * FL_NO_USE_FUNC now removes all of the access routines pointers.
 * 
 *    Rev 1.16   Feb 19 2002 20:59:44   oris
 * Bug fix changed definition of FL_IPL_MODE_XSCALE from 3 to 4.
 * 
 *    Rev 1.15   Jan 29 2002 20:08:26   oris
 * Changed FLAccessStruct definition to prevent compilation errors.
 * Added FL_IPL_MODE_XSCALE definition and change FL_IPL_XXX values.
 * 
 *    Rev 1.14   Jan 28 2002 21:24:48   oris
 * Added FL_IPL_DOWNLOAD flag to writeIPL routine in order to control whether the IPL will be reloaded after the update.
 * Added FLAccessStruct definition - used to get and set DiskOnChip memory access routines.
 * Removed win_io field from FLFlash record.
 * 
 *    Rev 1.13   Jan 23 2002 23:31:34   oris
 * Missing declaration of globalReadBack buffer, when MTD_RECONSTRUCT is defined.
 * 
 *    Rev 1.12   Jan 21 2002 20:44:32   oris
 * Bug fix - PARTIAL_EDC flag was support to incorporate EDC flag.
 * 
 *    Rev 1.11   Jan 20 2002 09:44:00   oris
 * Bug fix - changed include directive of flBuffer.h  to flbuffer.h
 * 
 *    Rev 1.10   Jan 17 2002 23:01:28   oris
 * Added flFlashOf() prototype.
 * New memory access routines mechanism :
 *  - Added memory access routines pointers in FLFlash.
 *  - Added win_io and win fields to FLFlash record pointing to DiskOnChip IO registers and window base.
 *  - Added busAccessType.
 * Moved CardAddress typedef and NDOC2window typedefs from flbase.h
 * Added DiskOnChip Millennium Plus 16MB type MDOCP_16_TYPE.
 * Added the following definitions FL_IPL_MODE_NORMAL / FL_IPL_MODE_SA /  MAX_PROTECTED_PARTITIONS /MAX_SECTORS_PER_BLOCK
 * Added Another flag to writeIPL for Strong Arm mode.
 * 
 *    Rev 1.9   Sep 15 2001 23:46:08   oris
 * Changed erase routine to support up to 64K erase blocks.
 * Added reconstruct flag to readBBT routine - stating whether to reconstruct BBT if it is not available.
 * 
 *    Rev 1.8   Jul 13 2001 01:04:48   oris
 * Added include directive to flBuffer and readBack buffer forward definition under the MTD_STANDALONE compilation flag.
 * Added volNo field to the socket record under the MTD_STANDALONE compilation flag.
 * Added definition for PARTIAL_EDC flash read mode.
 * Added protection default key.
 * Added bad block marking in the BBT (BBT_BAD_UNIT).
 * Moved syndrome length definition to reedsol files.
 * Added new field in FLFlash record - Max Erase Cycles of the flash.
 * Changed interleave field in FLFlash record to signed.
 *
 *    Rev 1.7   May 16 2001 21:18:30   oris
 * Moved SYNDROM_BYTES definition from diskonc.h and mdocplus.h.
 * Added forward definition for saveSyndromForDumping global EDC\ECC syndrome buffer.
 * Changed DATA definition to FL_DATA.
 *
 *    Rev 1.6   May 02 2001 06:40:58   oris
 * Removed the lastUsableBlock variable.
 * Added the BBT_UNAVAIL_UNIT defintion.
 *
 *    Rev 1.5   Apr 24 2001 17:08:12   oris
 * Added lastUsableBlock field and changed firstUsableBlock type to dword.
 *
 *    Rev 1.4   Apr 16 2001 13:40:48   oris
 * Added firstUsableBlock.
 * Removed warrnings by changing some of the fields types.
 *
 *    Rev 1.3   Apr 12 2001 06:51:12   oris
 * Changed protectionBounries and protectionSet routine to be floor specific.
 * Changed powerdown prototype.
 * Added download prototype.
 *
 *    Rev 1.2   Apr 01 2001 07:54:24   oris
 * copywrite notice.
 * Moved protection attributes definition from mdocplus.h
 * Changed prototype of routine pointers in flflash struct :read,write routines to dword length.
 * Other routine pointer prototypes have been changed as well.
 * Removed interface b routine pointers from flflash struct (experimental MTD interface for mdocp).
 * Changed prototype of :read,write routine to enabled dword length.
 * Changed unsigned char to byte.
 * Changed unsigned long to dword.
 * Changed long int to Sdword.
 * Spelling mistake "changable".
 *
 *    Rev 1.1   Feb 13 2001 01:37:38   oris
 * Changed ENTER_DEEP_POWER_DOWN_MODE to DEEP_POWER_DOWN
 * Changed LOCKED to LOCKED_OTP
 *
 *    Rev 1.0   Feb 04 2001 11:30:44   oris
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


#ifndef FLFLASH_H
#define FLFLASH_H

#include "flbase.h"

#ifndef MTD_STANDALONE
#include "flsocket.h"
#else
#include "flbuffer.h"
#endif /* MTD_STANDALONE */

/* Some useful types for mapped Flash locations */

typedef volatile byte FAR0 * FlashPTR;
typedef volatile unsigned short int FAR0 * FlashWPTR;
typedef volatile dword FAR0 * FlashDPTR;
typedef unsigned short FlashType;        /* JEDEC id */
typedef volatile unsigned char FAR0* NDOC2window;

/* DiskOnChip memory access routines type defintions */

/* Doc memory read routine         */
typedef  void (FLMemRead)(volatile byte FAR1* win,word regOffset,byte FAR1* dest,word count);
/* Doc memory write routine        */
typedef  void (FLMemWrite)(volatile byte FAR1* win,word regOffset,byte FAR1* src,word count);
/* Doc memory set routine          */
typedef  void (FLMemSet)(volatile byte FAR1* win,word regOffset,word count, byte val);
/* Doc memory 8 bit read routine   */
typedef  byte (FLMemRead8bit)(volatile byte FAR1* win,word offset);
/* Doc memory 8 bit write routine  */
typedef  void (FLMemWrite8bit)(volatile byte FAR1* win,word offset,byte Data);
/* Doc memory 16 bit read routine  */
typedef  word (FLMemRead16bit)(volatile byte FAR1* win,word offset);
/* Doc memory 16 bit write routine */
typedef  void (FLMemWrite16bit)(volatile byte FAR1* win,word offset,word Data);
/* Doc memory window size */
typedef  dword (FLMemWindowSize)(void);
/* Interleave change event Call back routine */
typedef  FLStatus (FLMemSetGetMode)(byte interleave, byte socketNo , dword flag);

typedef struct {        /* DiskOnChip memory access routines */
  dword                 access; /* Output only */
  FLMemRead       FAR1* memRead;
  FLMemWrite      FAR1* memWrite;
  FLMemSet        FAR1* memSet;
  FLMemRead8bit   FAR1* memRead8bit;
  FLMemWrite8bit  FAR1* memWrite8bit;
  FLMemRead16bit  FAR1* memRead16bit;
  FLMemWrite16bit FAR1* memWrite16bit;
  FLMemWindowSize FAR1* memWindowSize;
  FLMemSetGetMode FAR1* memSetGetMode;
}FLAccessStruct;

#define NOT_FLASH          0

/* Media types */
#define NOT_DOC_TYPE       0
#define DOC_TYPE           1
#define MDOC_TYPE          2
#define DOC2000TSOP_TYPE   3
#define MDOCP_TYPE         4
#define MDOCP_16_TYPE      5

/* page characteristics flags */
#define  BIG_PAGE    0x0100             /* page size > 100H*/
#define  FULL_PAGE   0x0200                  /* no partial page programming*/
#define  BIG_ADDR    0x0400             /* 4 byte address cycle */

/* MTD write routine mode flags */
#define FL_DATA       0      /* Read/Write data area                */
#define OVERWRITE     1      /* Overwriting non-erased area         */
#define EDC           2      /* Activate ECC/EDC                    */
#define EXTRA         4      /* Read/write spare area               */
#define PARTIAL_EDC   10     /* Read with EDC even for partial page */
#define NO_SECOND_TRY 0x8000 /* do not read again on EDC error      */

/* Protection attributes */
#define PROTECTABLE           1  /* partition can recieve protection */
#define READ_PROTECTED        2  /* partition is read protected      */
#define WRITE_PROTECTED       4  /* partition is write protected     */
#define LOCK_ENABLED          8  /* HW lock signal is enabled        */
#define LOCK_ASSERTED         16 /* HW lock signal is asserted       */
#define KEY_INSERTED          32 /* key is inserted (not currently   */
#define CHANGEABLE_PROTECTION 64 /* changeable protection area type   */

/* protection specific defintions */
#define DO_NOT_COMMIT_PROTECTION 0 /* The new values will take affect only after reset */
#define COMMIT_PROTECTION        1 /* The new values will take affect imidiatly        */
#define PROTECTION_KEY_LENGTH    8 /* Size of protection key in bytes    */  
#define MAX_PROTECTED_PARTITIONS 2 /* Max Number of protected partitiosn */
#define DEFAULT_KEY              "00000000"

/* IPL modes */
#define FL_IPL_MODE_NORMAL 0 /* IPL - Written as usual                     */
#define FL_IPL_DOWNLOAD    1 /* IPL - Force download of new IPL            */
#define FL_IPL_MODE_SA     2 /* IPL - Written with Strong Arm mode enabled */
#define FL_IPL_MODE_XSCALE 4 /* IPL - Written with X-Scale mode enabled    */

/* OTP specific defintions */
#define CUSTOMER_ID_LEN          4
#define UNIQUE_ID_LEN            16

/* BBT block types */
#define BBT_GOOD_UNIT            0xff
#define BBT_UNAVAIL_UNIT         0x1
#define BBT_BAD_UNIT             0x0

/* General purpose */
#define MAX_SECTORS_PER_BLOCK    64

/* Flag bit values */
#define SUSPEND_FOR_WRITE        1        /* MTD provides suspend for write */
#define NFTL_ENABLED             2        /* Flash can run NFTL             */
#define INFTL_ENABLED            4        /* Flash can run INFTL            */
#define EXTERNAL_EPROM           8        /* Can support external eprom     */

#define LOCKED_OTP               1

#define DEEP_POWER_DOWN          1 /* must be the same as in blockdev.h */

/* MTD registration information */
extern int noOfMTDs;        /* No. of MTDs actually registered */

/* Flash array identification structure */
typedef struct tFlash FLFlash;                /* Forward definition */

#include "_flflash.h"

#endif /* FLFLASH_H */

