/*
 * $Log: blockdev.h,v $
 * Revision 1.1.1.1  2006-07-11 09:28:42  andy
 * W90N745 BSP
 *
 * Revision 1.1.1.1  2006/04/26 02:35:50  andy
 * W90N745 uCLinux kernel
 *
 * 
 *    Rev 1.21   May 14 2002 15:03:22   oris
 * Seperated the private definitions to _blkdev.h
 * 
 *    Rev 1.20   Apr 15 2002 07:34:06   oris
 * Bug fix - FL_IPL_MODE_XSCALE define was set to 3 instead of 4 and therefore caused FL_IPL_DOWNLOAD and FL_IPL_MODE_SA to be set as well.
 * 
 *    Rev 1.19   Feb 19 2002 20:58:20   oris
 * Removed warnings.
 * Moved FLFunctionNo enumerator to dedicated file flfuncno.h
 * Added include directive for cleaner customer usage.
 * 
 *    Rev 1.18   Jan 29 2002 20:07:16   oris
 * Moved flParsePath declaration to the end of the file.
 * Changed LOW_LEVEL compilation flag with FL_LOW_LEVEL to prevent definition clashes.
 * Added documentation of irFlags in flMountVolume (returns no of hidden sectors of the media).
 * flSetEnvVolume, flSetEnvSocket , flSetEnvAll , flSetDocBusRoutine , flGetDocBusRoutine, flBuildGeometry , bdCall and flExit
 * Added FL_IPL_MODE_XSCALE definition and change FL_IPL_XXX values.
 * 
 *    Rev 1.17   Jan 28 2002 21:23:46   oris
 * Changed FL_NFTL_CACHE_ENABLED to FL_TL_CACHE_ENABLED.
 * Changed flSetDocBusRoutine interface and added flGetDocBusRoutine. 
 * 
 *    Rev 1.16   Jan 23 2002 23:30:54   oris
 * Added documentation of irData and irLength to flCheckVolume.
 * 
 *    Rev 1.15   Jan 20 2002 20:27:40   oris
 * Added TL_NORMAL_FORMAT flag was added to bdFormatPhisycalDrive instead of 0 (in the comments).
 * Removed TL_QUICK_MOUNT_FORMAT flag definition.
 * 
 *    Rev 1.14   Jan 17 2002 22:57:18   oris
 * Added flClearQuickMountInfo() routine - FL_CLEAR_QUICK_MOUNT_INFO
 * Added flVerifyVolume() routine - FL_VERIFY_VOLUME
 * Added DiskOnChip Millennium Plus 16MB type
 * Changed the order of FLEnvVars enumerator.
 * Added FLEnvVars values for :
 *       FL_SECTORS_VERIFIED_PER_FOLDING 
 *       FL_SUSPEND_MODE
 *       FL_VERIFY_WRITE_OTHER
 *       FL_MTD_BUS_ACCESS_TYPE
 *       FL_VERIFY_WRITE_BDTL
 *       FL_VERIFY_WRITE_BINARY
 * flSetEnv() routine was changed into 3 different routines: flSetEnvVolume / flSetEnvSocket / flSetEnvAll
 * Removed TL_SINGLE_FLOOR_FORMATTING flag definition from format routine.
 * Added flSetDocBusRoutines prototype and required definitions.
 * 
 *    Rev 1.13   Nov 21 2001 11:39:36   oris
 * Changed FL_VERIFY_WRITE_MODE to FL_MTD_VERIFY_WRITE.
 * 
 *    Rev 1.12   Nov 08 2001 10:44:18   oris
 * Added FL_VERIFY_WRITE_MODE enumerator type for the flSetEnv routine .
 * Moved environment variable states definitions to flbase.h.
 * 
 *    Rev 1.11   Sep 15 2001 23:44:30   oris
 * Placed flDeepPowerDownMone under LOW_LEVEL compilation flag.
 * 
 *    Rev 1.10   May 17 2001 16:50:32   oris
 * Removed warnings.
 * 
 *    Rev 1.9   May 16 2001 21:16:22   oris
 * Added the Binary state (0,1) of the environment variables to meaningful definitions.
 * Removed LAST function enumerator.
 * Improved documentation.
 * 
 *    Rev 1.8   May 06 2001 22:41:14   oris
 * Added SUPPORT_WRITE_IPL_ROUTIN capability.
 * 
 *    Rev 1.7   Apr 30 2001 17:57:50   oris
 * Added required defintions to support the flMarkDeleteOnFlash environment variable. 
 * 
 *    Rev 1.6   Apr 24 2001 17:05:52   oris
 * Changed bdcall function numbers in order to allow future grouth.
 * 
 *    Rev 1.5   Apr 01 2001 07:49:04   oris
 * Added FL_READ_IPL .
 * flChangeEnvironmentVariable prototype removed.
 * Moved s/w protection definitions from iovtl.h to blockdev.h
 * Changed s\w and h\w to s/w and h/w.
 * Added flBuildGeometry prototype 
 * Moved bdcall prototype to the end of the file with the rest of the prototypes.
 * 
 *    Rev 1.4   Feb 18 2001 14:15:38   oris
 * Changed function enums order.
 *
 *    Rev 1.3   Feb 14 2001 01:44:16   oris
 * Changed capabilities from defined flags to an enumerator
 * Improoved documentation of readBBT, writeBBT InquireCapabilities, countVolumes
 * Added environment variables defintions
 *
 *    Rev 1.2   Feb 13 2001 02:08:42   oris
 * Moved LOCKED_OTP and DEEP_POWER_DOWN to flflash.h
 * Moved TL_FORMAT_FAT and TL_FORMAT_COMPRESSION to flformat.h
 * Added extern declaration for flSetEnv routine.
 *
 *    Rev 1.1   Feb 12 2001 11:54:46   oris
 * Added baseAddress in flGetPhysicalInfo as irLength.
 * Added boot sectors in flMountVolumes as irFlags.
 * Change order of routines definition.
 *
 *    Rev 1.0   Feb 04 2001 18:05:04   oris
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

#ifndef BLOCKDEV_H
#define BLOCKDEV_H

#include "flreq.h"
#include "flfuncno.h"
#include "docsys.h"

#ifdef FORMAT_VOLUME
#include "dosformt.h"
#endif /* FORMAT_VOLUME */
#ifdef WRITE_EXB_IMAGE
#include "doc2exb.h"
#else
#ifdef BDK_ACCESS
#include "docbdk.h"
#endif /* BDK_ACCESS */
#endif /* WRITE_EXB_IMAGE */

#include "_blkdev.h"


/*----------------------------------------------------------------------*/
/*                     V o l u m e I n f o R e c o r d                  */
/*                                                                      */
/* A structure that holds general information about the media. The      */
/* information includes Physical Info (see flGetPhysicalInfo), Logical  */
/* partition (number of sectors and CHS), boot area size, S/W versions  */
/* Media life-time etc.                                                 */
/* A pointer to this structure is passed to the function flVolumeInfo   */
/* where it receives the relevant data.                                 */
/*----------------------------------------------------------------------*/

typedef struct {
  unsigned long  logicalSectors;    /*  number of logical sectors                  */
  unsigned long  bootAreaSize;      /*  boot area size                             */
  unsigned long  baseAddress;       /*  physical base address                      */
#ifdef FL_LOW_LEVEL
  unsigned short flashType;         /*  JEDEC id of the flash                      */
  unsigned long  physicalSize;      /*  physical size of the media                 */
  unsigned short physicalUnitSize;  /*  Erasable block size                        */
  char DOCType;                     /*  DiskOnChip type (MDoc/Doc2000)             */
  char lifeTime;                    /*  Life time indicator for the media (1-10)   */
                                    /*  1 - the media is fresh,                    */
                                    /*  10 - the media is close to its end of life */
#endif
  char driverVer[10];               /*  driver version (NULL terminated string)    */
  char OSAKVer[10];                 /*  OSAK version that driver is based on
                                        (NULL terminated string)                   */
#ifdef ABS_READ_WRITE
  unsigned long cylinders;          /*  Media.....                                 */
  unsigned long heads;              /*            geometry......                   */
  unsigned long sectors;            /*                            parameters.      */
#endif
} VolumeInfoRecord;


#ifndef FL_READ_ONLY
#ifdef FORMAT_VOLUME

/** Values of irFlags for flLowLevelFormat: */
#define FAT_ONLY_FORMAT          0
#define TL_FORMAT                1
#define TL_FORMAT_IF_NEEDED      2
#define TL_FORMAT_ONLY           8

#define TL_NORMAL_FORMAT         0
#define TL_LEAVE_BINARY_AREA     8

#endif /* FORMAT_VOLUME */
#endif /*FL_READ_ONLY */


#ifdef ABS_READ_WRITE
#ifndef FL_READ_ONLY
#ifdef WRITE_PROTECTION
#define FL_PROTECT   0
#define FL_UNPROTECT 1
#define FL_UNLOCK    2
#endif /* WRITE_PROTETION */
#endif /* FL_READ_ONLY */
#endif /* ABS_READ_WRITE */

#ifdef FL_LOW_LEVEL

/*----------------------------------------------------------------------*/
/*                          P h y s i c a l I n f o                     */
/*                                                                      */
/* A structure that holds physical information about the media. The     */
/* information includes JEDEC ID, unit size and media size. Pointer     */
/* to this structure is passed to the function flGetPhysicalInfo where  */
/* it receives the relevant data.                                       */
/*                                                                      */
/*----------------------------------------------------------------------*/

typedef struct {
  unsigned short type;         /* Flash device type (JEDEC id)         */
  char           mediaType;    /* type of media see below              */
  long int       unitSize;     /* Smallest physically erasable size
                                  (with interleaving taken in account) */
  long int       mediaSize;    /* media size in bytes                  */
  long int       chipSize;     /* individual chip size in bytes        */
  int            interleaving; /* device interleaving                  */
} PhysicalInfo;

/* media types */
#define FL_NOT_DOC     0
#define FL_DOC         1
#define FL_MDOC        2
#define FL_DOC2000TSOP 3
#define FL_MDOCP_16    4
#define FL_MDOCP       5

/* capabilities flags */
typedef enum{
   CAPABILITY_NOT_SUPPORTED           = 0,
   CAPABILITY_SUPPORTED               = 1,
   SUPPORT_UNERASABLE_BBT             = 2,
   SUPPORT_MULTIPLE_BDTL_PARTITIONS   = 3,
   SUPPORT_MULTIPLE_BINARY_PARTITIONS = 4,
   SUPPORT_HW_PROTECTION              = 5,
   SUPPORT_HW_LOCK_KEY                = 6,
   SUPPORT_CUSTOMER_ID                = 7,
   SUPPORT_UNIQUE_ID                  = 8,
   SUPPORT_DEEP_POWER_DOWN_MODE       = 9,
   SUPPORT_OTP_AREA                   = 10,
   SUPPORT_WRITE_IPL_ROUTINE          = 11
}FLCapability;

#endif /* FL_LOW_LEVEL */


#ifdef ENVIRONMENT_VARS

typedef enum {        /* Variable type code for flSetEnv routin */
      FL_ENV_VARS_PER_SYSTEM          = 0,
      FL_IS_RAM_CHECK_ENABLED         = 1,
      FL_TL_CACHE_ENABLED             = 2,
      FL_DOC_8BIT_ACCESS              = 3,
      FL_MULTI_DOC_ENABLED            = 4,      
      FL_SET_MAX_CHAIN                = 5,
      FL_MARK_DELETE_ON_FLASH         = 6,
      FL_MTL_POLICY                   = 7,
      FL_SECTORS_VERIFIED_PER_FOLDING = 8,
      FL_SUSPEND_MODE                 = 9,
      FL_ENV_VARS_PER_SOCKET          = 100,
      FL_VERIFY_WRITE_OTHER           = 101,
      FL_MTD_BUS_ACCESS_TYPE          = 102,
      FL_ENV_VARS_PER_VOLUME          = 200,
      FL_SET_POLICY                   = 201,
      FL_VERIFY_WRITE_BDTL            = 202,
      FL_VERIFY_WRITE_BINARY          = 203
} FLEnvVars;

/*----------------------------------------------------------------------*/
/*                   f l S e t E n v V o l u m e                        */
/*                                                                      */
/* Change one of TrueFFS environment variables for a specific partition */
/*                                                                      */
/* Note : This routine is used by all other flSetEnv routines.          */
/*        In order to effect variables that are common to several       */
/*        sockets or volumes use INVALID_VOLUME_NUMBER                  */
/*                                                                      */
/* Parameters:                                                          */
/*      variableType    : variable type to cahnge                       */
/*      socket          : Associated socket                             */
/*      volume          : Associated volume (partition)                 */
/*      value           : varaible value                                */
/*                                                                      */
/* Note: Variables common to al sockets must be addressed using socket  */
/*       0 and volume 0.                                                */
/*                                                                      */
/* Returns:                                                             */
/*      FLStatus        : 0 on success, otherwise failed                */
/*      prevValue       : The previous value of the variable            */
/*----------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
FLStatus NAMING_CONVENTION flSetEnvVolume(FLEnvVars variableType ,
                  byte socket,byte volume ,
                  dword value, dword FAR2 *prevValue);
#ifdef __cplusplus
}
#endif /* __cplusplus */

/*----------------------------------------------------------------------*/
/*                       f l S e t E n v S o c k e t                    */
/*                                                                      */
/* Change one of TrueFFS environment variables for a specific sockets.  */
/*                                                                      */
/* Parameters:                                                          */
/*      variableType    : variable type to cahnge                       */
/*      socket          : socket number                                 */
/*      value           : varaible value                                */
/*                                                                      */
/* Returns:                                                             */
/*      FLStatus        : 0 on success, otherwise failed                */
/*      prevValue       : The previous value of the variable            */
/*                        if there are more then 1 partition in that    */
/*                        socket , the first partition value is returned*/
/*----------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
FLStatus NAMING_CONVENTION flSetEnvSocket(FLEnvVars variableType , byte socket ,
                        dword value, dword FAR2 *prevValue);
#ifdef __cplusplus
}
#endif /* __cplusplus */

/*----------------------------------------------------------------------*/
/*                       f l S e t E n v All                            */
/*                                                                      */
/* Change one of TrueFFS environment variables for all systems, sockets */
/* and partitions.                                                      */
/*                                                                      */
/* Parameters:                                                          */
/*      variableType    : variable type to cahnge                       */
/*      value           : varaible value                                */
/*                                                                      */
/* Returns:                                                             */
/*      FLStatus        : 0 on success, otherwise failed                */
/*      prevValue       : The previous value of the variable            */
/*----------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
FLStatus NAMING_CONVENTION flSetEnvAll(FLEnvVars variableType , dword value, dword FAR2 *prevValue);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ENVIRONMENT_VARS */


#ifndef FL_NO_USE_FUNC

/*----------------------------------------------------------------------*/
/*                  f l S e t D o c B u s R o u t i n e                 */
/*                                                                      */
/* Set user defined memory acces routines for DiskOnChip.               */
/*                                                                      */
/* Parameters:                                                          */
/*      socket      : Socket number to install routine for.             */
/*      structPtr   : Pointer to function structure.                    */
/*                                                                      */
/* Returns:                                                             */
/*      FLStatus        : 0 on success, otherwise failed                */
/*----------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
FLStatus NAMING_CONVENTION flSetDocBusRoutine(byte socket, FLAccessStruct FAR1 * structPtr);
#ifdef __cplusplus
}
#endif /* __cplusplus */

/*----------------------------------------------------------------------*/
/*                  f l G e t D o c B u s R o u t i n e                 */
/*                                                                      */
/* Get currently installed memory access routines for DiskOnChip.       */
/*                                                                      */
/* Parameters:                                                          */
/*      socket      : Socket number to install routine for.             */
/*      structPtr   : Pointer to function structure.                    */
/*                                                                      */
/* Returns:                                                             */
/*      FLStatus        : 0 on success, otherwise failed                */
/*----------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */
FLStatus NAMING_CONVENTION flGetDocBusRoutine(byte socket, FLAccessStruct FAR1 * structPtr);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* FL_NO_USE_FUNC */

#endif /* BLOCKDEV_H */
