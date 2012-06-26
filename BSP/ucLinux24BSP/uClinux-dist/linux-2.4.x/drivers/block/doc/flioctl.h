/*
 * $Log: flioctl.h,v $
 * Revision 1.1.1.1  2006-07-11 09:28:42  andy
 * W90N745 BSP
 *
 * Revision 1.1.1.1  2006/04/26 02:35:50  andy
 * W90N745 uCLinux kernel
 *
 * 
 *    Rev 1.12   May 14 2002 15:07:16   oris
 * Separated private definitions to _flioctl.h
 * 
 *    Rev 1.11   Jan 29 2002 20:08:54   oris
 * Changed LOW_LEVEL compilation flag with FL_LOW_LEVEL to prevent definition clashes.
 * 
 *    Rev 1.10   Jan 28 2002 21:25:20   oris
 * Added FL_IOCTL_GET_ACCESS_ROUTINE and FL_IOCTL_EXTENDED_WRITE_IPL .
 * Removed FL_IOCTL_ENVIRONMENT_VARIABLE record.
 * Compilation errors fixed.
 * Changed flSetMemoryAccessInput and added flGetMemoryAccessOutput records.
 * Changed flIplInput record to support the extended write IPL call (added flags field).
 * 
 *    Rev 1.9   Jan 23 2002 23:31:46   oris
 * Added support for 2 additional parameters to FL_IOCTL_VERIFY_VOLUME call.
 * 
 *    Rev 1.8   Jan 17 2002 23:02:02   oris
 * Added 3 new ioctls
 *  - FL_IOCTL_EXTENDED_ENVIRONMENT_VARIABLES - Set environment variables
 *    - flExtendedEnvVarsInput and flExtendedEnvVarsOutput records 
 *    - FL_APPLY_TO_ALL / FL_APPLY_TO_SOCKET / FL_APPLY_TO_VOLUME  definitions
 *  - FL_IOCTL_SET_ACCESS_ROUTINE -set access routines
 *    - flVerifyVolumeOutput record 
 *  - FL_IOCTL_VERIFY_VOLUME - check volume for errors caused by power  failures.
 *    - flSetMemoryAccessInput record
 * 
 *    Rev 1.7   May 09 2001 00:45:48   oris
 * Changed protection ioctl interface to prevent the use of input buffer as an output buffer.
 * 
 *    Rev 1.6   Apr 16 2001 13:45:10   oris
 * Removed warrnings by changing some of the fields types to standart flite types.
 * 
 *    Rev 1.5   Apr 09 2001 15:02:22   oris
 * Added comment to ifdef statment.
 * End with an empty line.
 * 
 *    Rev 1.4   Apr 01 2001 15:16:38   oris
 * Updated inquire capability ioctl - diffrent input and output records.
 *
 *    Rev 1.3   Apr 01 2001 07:58:44   oris
 * Moved the following defines to blockdev.h:
 *   FL_PROTECT   0
 *   FL_UNPROTECT 1
 *   FL_UNLOCK    2
 *
 *    Rev 1.2   Feb 14 2001 02:16:16   oris
 * Updated inquire capabilities ioctl.
 *
 *    Rev 1.1   Feb 13 2001 01:49:06   oris
 * Added the following new IO Controls:
 *   FL_IOCTL_FORMAT_VOLUME2,
 *   FL_IOCTL_FORMAT_PARTITION,
 *   FL_IOCTL_BDTL_HW_PROTECTION,
 *   FL_IOCTL_BINARY_HW_PROTECTION,
 *   FL_IOCTL_OTP,
 *   FL_IOCTL_CUSTOMER_ID,
 *   FL_IOCTL_UNIQUE_ID,
 *   FL_IOCTL_NUMBER_OF_PARTITIONS,
 *   FL_IOCTL_SUPPORTED_FEATURES,
 *   FL_IOCTL_SET_ENVIRONMENT_VARIABLES,
 *   FL_IOCTL_PLACE_EXB_BY_BUFFER,
 *   FL_IOCTL_WRITE_IPL,
 *   FL_IOCTL_DEEP_POWER_DOWN_MODE,
 * and BDK_GET_INFO type in FL_IOCTL_BDK_OPERATION
 *
 *    Rev 1.0   Feb 04 2001 11:38:18   oris
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

#include "flbase.h"
#include "dosformt.h"
#include "blockdev.h"
#ifdef BDK_ACCESS
#include "docbdk.h"
#endif

#ifndef FLIOCTL_H
#define FLIOCTL_H

#include "_flioctl.h"

#ifdef IOCTL_INTERFACE

/* In every call to flIOctl function, the irFlags field in the structure
   IOreq should hold one of the following: */
typedef enum{FL_IOCTL_GET_INFO = FL_IOCTL_START,
             FL_IOCTL_DEFRAGMENT,
             FL_IOCTL_WRITE_PROTECT,
             FL_IOCTL_MOUNT_VOLUME,
             FL_IOCTL_FORMAT_VOLUME,
             FL_IOCTL_BDK_OPERATION,
             FL_IOCTL_DELETE_SECTORS,
             FL_IOCTL_READ_SECTORS,
             FL_IOCTL_WRITE_SECTORS,
             FL_IOCTL_FORMAT_PHYSICAL_DRIVE,
             FL_IOCTL_FORMAT_LOGICAL_DRIVE,
             FL_IOCTL_BDTL_HW_PROTECTION,
             FL_IOCTL_BINARY_HW_PROTECTION,
             FL_IOCTL_OTP,
             FL_IOCTL_CUSTOMER_ID,
             FL_IOCTL_UNIQUE_ID,
             FL_IOCTL_NUMBER_OF_PARTITIONS,
             FL_IOCTL_INQUIRE_CAPABILITIES,
             FL_IOCTL_SET_ENVIRONMENT_VARIABLES, /* No longer supported */
             FL_IOCTL_PLACE_EXB_BY_BUFFER,
             FL_IOCTL_WRITE_IPL,                 /* No longer supported */
             FL_IOCTL_DEEP_POWER_DOWN_MODE,
             FL_IOCTL_EXTENDED_ENVIRONMENT_VARIABLES, 
             FL_IOCTL_VERIFY_VOLUME,
             FL_IOCTL_SET_ACCESS_ROUTINE,
             FL_IOCTL_GET_ACCESS_ROUTINE,
             FL_IOCTL_EXTENDED_WRITE_IPL
} flIOctlFunctionNo;



/* In every call to flIOctl function, the irData field in the structure
   IOreq should point to the structure defined below. The fields
   inputRecord and outputRecord should point to structures which are
   specific to each IOctl function as defined in this file. */
typedef struct {
  void FAR1 *inputRecord;
  void FAR1 *outputRecord;
} flIOctlRecord;


/* General output record that returns only status. */
typedef struct {
  FLStatus status;
} flOutputStatusRecord;



/* Input and output records for the different IOCTL functions: */
/* =========================================================== */

/* Get disk information (FL_IOCTL_GET_INFO) */
/* Input record: NULL */
/* Output record: */
typedef struct {
  VolumeInfoRecord info;  /* VolumeInfoRecord is defined in blockdev.h */
  FLStatus status;
} flDiskInfoOutput;
/* Output record: flOutputStatusRecord */
/*************************************************************************/
#ifdef DEFRAGMENT_VOLUME
/* Defragment volume (FL_IOCTL_DEFRAGMENT) */
/* Input record: */
typedef struct {
  long requiredNoOfSectors;   /* Minimum number of sectors to make available.
                                  if -1 then a quick garbage collection operation
                                 is invoked. */
} flDefragInput;
/* Outout record: */
typedef struct {
  long actualNoOfSectors;     /* Actual number of sectors available */
  FLStatus status;
} flDefragOutput;
#endif
/*************************************************************************/
#ifdef WRITE_PROTECTION
/* Write protection (FL_IOCTL_WRITE_PROTECT) */
/* Input record: */
typedef struct {
  byte type;        /*  type of operation: FL_PROTECT\FL_UNPROTECT */
  long password[2];          /*  password  */
} flWriteProtectInput;
/* Output record: flOutputStatusRecord */
#endif /* WRITE_PROTECTION */
/*************************************************************************/
/* Mount volume (FL_IOCTL_MOUNT_VOLUME) */
/* Input record: */
typedef struct {
  byte type;        /*  type of operation: FL_MOUNT\FL_DISMOUNT */
} flMountInput;
#define FL_MOUNT          0
#define FL_DISMOUNT        1
/* Output record: flOutputStatusRecord */
/*************************************************************************/

#ifdef FORMAT_VOLUME
/* Format volume (FL_IOCTL_FORMAT_VOLUME) */
/* Input record: */
typedef struct {
  byte formatType;   /* type of format as defined in blockdev.h */
  FormatParams fp;              /* Format parameters structure (defined in flformat.h) */
} flFormatInput;
/* Output record: flOutputStatusRecord */
/*************************************************************************/
/* Format volume (FL_IOCTL_FORMAT_LOGICAL_DRIVE) */
/* Input record: */
typedef struct {
  BDTLPartitionFormatParams fp;              /* Format parameters structure (defined in flformat.h) */
} flFormatLogicalInput;
/* Output record: flOutputStatusRecord */
/*************************************************************************/
#ifdef FL_LOW_LEVEL
/* Format volume (FL_IOCTL_FORMAT_PHYSICAL_DRIVE) */
/* Input record: */
typedef struct {
  byte formatType;   /* type of format as defined in blockdev.h */
  FormatParams2 fp;              /* Format parameters structure (defined in flformat.h) */
} flFormatPhysicalInput;
/* Output record: flOutputStatusRecord */
#endif /* FL_LOW_LEVEL */
#endif /* FORMAT_VOLUME */
/*************************************************************************/
#ifdef BDK_ACCESS
/* BDK operations read\write\erase\create (FL_IOCTL_BDK_OPERATION) */
/* Input record: */
typedef struct {
  byte type;  /* type of operation: BDK_INIT_READ\BDK_READ\BDK_INIT_WRITE\ */
                       /* BDK_WRITE\BDK_ERASE\BDK_CREATE\BDK_GET_INFO               */
  BDKStruct bdkStruct; /* parameters for BDK operations (defined in docbdk.h)       */
} flBDKOperationInput;
#define BDK_INIT_READ   0
#define BDK_READ        1
#define BDK_INIT_WRITE  2
#define BDK_WRITE       3
#define BDK_ERASE       4
#define BDK_CREATE      5
#define BDK_GET_INFO    6
/* Output record: flOutputStatusRecord */
#endif                                  /* BDK_ACCESS  */
/*************************************************************************/
#ifdef HW_PROTECTION
/* BDK and BDTL protection operations: (FL_IOCTL_BINARY_HW_PROTECTION) */
/*                                     (FL_IOCTL_BDTL_HW_PROTECTION)   */
/*   insert key \ remove key \ identify \ change key \                 */
/*   change protection type \ change lock status                       */
/* Input record: */
typedef struct {
   byte protectionType;    /* see flflash.h for the protection attributes */
   byte key[8];            /* The new key to the change Key call          */
   byte type;              /* Operation type see list bellow              */
} flProtectionInput;
#define PROTECTION_INSERT_KEY   0
#define PROTECTION_REMOVE_KEY   1
#define PROTECTION_GET_TYPE     2
#define PROTECTION_DISABLE_LOCK 3
#define PROTECTION_ENABLE_LOCK  4
#define PROTECTION_CHANGE_KEY   5
#define PROTECTION_CHANGE_TYPE  6

/* Output record: */
typedef struct {
   byte protectionType;    /* see flflash.h for the protection attributes */
   FLStatus status;
} flProtectionOutput;

#endif /* HW_PROTECTION */
/*************************************************************************/
#ifdef HW_OTP
/* One Time Programing operations: (FL_IOCTL_OTP */
/*   OTP size \ OTP read \ OTP write and lock    */
/* Input record: */
typedef struct {
dword       length;           /* Length to read\write\size                  */
dword       usedSize;         /* The written size of the area \ Area offset */
byte        lockedFlag;       /* The area condition LOCKED_OTP (flflash.h)  */
byte        FAR1* buffer;     /* pointer to user buffer                     */
word        type;             /* defined bellow                             */
} flOtpInput;
#define OTP_SIZE        1
#define OTP_READ        2
#define OTP_WRITE_LOCK        3
/* Output record: flOutputStatusRecord */
/*************************************************************************/
/* Read customer ID (FL_IOCTL_CUSTOMER_ID) */
/* Input record: NULL */
/* Output record: */
typedef struct {
  byte id[4];
  FLStatus status;
} flCustomerIdOutput;
/*************************************************************************/
/* Read unique ID (FL_IOCTL_UNIQUE_ID) */
/* Input record: NULL */
/* Output record: */
typedef struct {
  byte id[16];
  FLStatus status;
} flUniqueIdOutput;
#endif /* HW_OTP */
/*************************************************************************/
/* Read unique ID (FL_IOCTL_NUMBER_OF_PARTITIONS) */
/* Input record: NULL */
/* Output record: */
typedef struct {
  byte noOfPartitions;
  FLStatus status;
} flCountPartitionsOutput;
/*************************************************************************/
/* Quary the device h/w and s/w capabilities (FL_IOCTL_INQUIRE_CAPABILITIES) */
#ifdef FL_LOW_LEVEL
/* Input record:   */
typedef struct {
   FLCapability  capability;  /* defined in blockdev.h */
} flCapabilityInput;
/* Output record: */
typedef struct {
   FLCapability  capability;  /* defined in blockdev.h */
   FLStatus      status;
} flCapabilityOutput;
#endif /* FL_LOW_LEVEL */
/*************************************************************************/
/* Place EXB file by buffers (FL_IOCTL_PLACE_EXB_BY_BUFFER) */
#ifdef FL_LOW_LEVEL
#ifdef WRITE_EXB_IMAGE
/* Input record:  */
typedef struct {
  byte FAR1* buf;  /* buffer of EXB file */
  dword bufLen;     /* buffer length      */
  byte exbFlags;   /* a combination of EXB flags see flPlaceExbByBuffer routine  */
                            /* The list of flags is defined in doc2exb.h                  */
  word exbWindow; /* explicitly set device window. 0 will automatcly set window */
} flPlaceExbInput;
#endif /* WRITE_EXB_IMAGE */
/* Output record: flOutputStatusRecord */
/*************************************************************************/
/* Place the device into and out of the power down mode (FL_IOCTL_DEEP_POWER_DOWN_MODE) */
typedef struct {
byte state; /* DEEP_POWER_DOWN - low power consumption      */
                     /* otherwise       - regular power consumption  */
} flPowerDownInput;
/* DEEP_POWER_DOWN is defined in flflash.h */
#endif /* FL_LOW_LEVEL */
/* Output record: flOutputStatusRecord */
/*************************************************************************/
#ifdef ABS_READ_WRITE
/* Delete logical sectors (FL_IOCTL_DELETE_SECTORS) */
/* Input record: */
typedef struct {
  long firstSector;                /* First logical sector to delete */
  long numberOfSectors;                /* Number of sectors to delete */
} flDeleteSectorsInput;
/* Output record: flOutputStatusRecord */
/*************************************************************************/
/* read & write logical sectors (FL_IOCTL_READ_SECTORS & FL_IOCTL_WRITE_SECTORS) */
/* Input record: */
typedef struct {
  long firstSector;                  /* first logical sector */
  long numberOfSectors;         /* Number of sectors to read\write */
  byte FAR1 *buf;               /* Data to read\write */
} flReadWriteInput;
/* Output record: */
typedef struct {
  long numberOfSectors;         /* Actual Number of sectors read\written */
  FLStatus status;
} flReadWriteOutput;
#endif /* ABS_READ_WRITE */
/******************************************************************************/
/* Set environment variables values (FL_IOCTL_EXTENDED_ENVIRONMENT_VARIABLES) */
#ifdef ENVIRONMENT_VARS
/* Input record:  */
typedef struct {
  FLEnvVars      varName;  /* Enum describing the variable */
  dword          varValue; /* New variable value           */
  dword          flags;    /* FL_APPLY_TO_ALL    - All socket and partitions */
                           /* FL_APPLY_TO_SOCKET - All socket and partitions */
                           /* FL_APPLY_TO_VOLUME - All socket and partitions */
} flExtendedEnvVarsInput;
#define FL_APPLY_TO_ALL    1
#define FL_APPLY_TO_SOCKET 2
#define FL_APPLY_TO_VOLUME 3
/* Output record: */
typedef struct {
  dword    prevValue;   /* The previous value of the variable */
  FLStatus status;
} flExtendedEnvVarsOutput;
#endif /* ENVIRONMENT_VARS */
/******************************************************************************/
/* Check partition for power failures symptoms (FL_IOCTL_VERIFY_VOLUME) */
#ifdef VERIFY_VOLUME
/* Input record: NULL */
typedef struct {
  dword flags;   /* Must be set to 0 */    
} flVerifyVolumeInput;
/* Output record: */
typedef struct {
  void FAR1* callBack;   /* Must be set to null */  
  FLStatus status;
} flVerifyVolumeOutput;
#endif /* VERIFY_VOLUME */
/******************************************************************************/
#ifndef FL_NO_USE_FUNC
/* Plant user defined memory access routines (FL_IOCTL_SET_ACCESS_ROUTINE) */
/* Input record:  */
typedef struct {
  FLAccessStruct FAR1 * structPtr; /* Pointer memory access routine struct */
} flSetMemoryAccessInput;
/* Output record: flOutputStatusRecord */
/******************************************************************************/
/* Plant user defined memory access routines (FL_IOCTL_GET_ACCESS_ROUTINE) */
/* Input record: NULL */
typedef struct {
  FLAccessStruct FAR1 * structPtr; /* Pointer memory access routine struct */
  FLStatus              status;
} flGetMemoryAccessOutput;
/* Output record: flOutputStatusRecord */
#endif /* FL_NO_USE_FUNC */
/******************************************************************************/
#ifndef NO_IPL_CODE
/* Write IPL area for docPlus family (FL_IOCTL_WRITE_IPL) */
/* Input record:  */
typedef struct {
  byte FAR1* buf;      /* IPL data buffer           */
  word   flags;        /* IPL flags (see flflash.h) */
  word   bufLen;       /* IPL data buffer length    */
} flIplInput;
#endif /* NO_IPL_CODE */
#endif /* IOCTL_INTERFACE */
#endif /* FLIOCTL_H */




