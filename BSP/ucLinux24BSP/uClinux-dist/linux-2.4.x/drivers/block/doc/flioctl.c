/*
 * $Log: flioctl.c,v $
 * Revision 1.1.1.1  2006-07-11 09:28:42  andy
 * W90N745 BSP
 *
 * Revision 1.1.1.1  2006/04/26 02:35:50  andy
 * W90N745 uCLinux kernel
 *
 * 
 *    Rev 1.11   Jan 29 2002 20:08:44   oris
 * Changed LOW_LEVEL compilation flag with FL_LOW_LEVEL to prevent definition clashes.
 * 
 *    Rev 1.10   Jan 28 2002 21:25:08   oris
 * Removed error massage from FL_IOCTL_SET_ENVIRONMENT_VARIABLE now simply returns flBadParameter.
 * Replaced FL_IOCTL_WRITE_IPL (now return flBadParameter) with FL_IOCTL_EXTENDED_WRITE_IPL in order to support Strong arm mode and download flags.
 * Changed FL_IOCTL_SET_ACCESS_ROUTINE interface to support update of all DiskOnChip memory access in a single call.
 * Added FL_IOCTL_GET_ACCESS_ROUTINE to get current DiskOnChip memory access routine pointers.
 * 
 *    Rev 1.9   Jan 23 2002 23:31:44   oris
 * Added support for 2 additional parameters to FL_IOCTL_VERIFY_VOLUME call.
 * 
 *    Rev 1.8   Jan 17 2002 23:01:48   oris
 * Removed warnings.
 * Forced FL_IOCTL_SET_ENVIRONMENT_VARIABLES to return  flFeatureNotSupported in order to force the use of the new routine.
 * Added 3 new ioctls
 *  - FL_IOCTL_EXTENDED_ENVIRONMENT_VARIABLES - Set environment variables
 *  - FL_IOCTL_SET_ACCESS_ROUTINE -set access routines
 *  - FL_IOCTL_VERIFY_VOLUME - check volume for errors caused by power  failures.
 * 
 *    Rev 1.7   May 09 2001 00:45:48   oris
 * Changed protection ioctl interface to prevent the use of input buffer as an output buffer.
 * 
 *    Rev 1.6   Apr 16 2001 13:43:28   oris
 * Removed warrnings.
 * 
 *    Rev 1.5   Apr 09 2001 15:09:20   oris
 * End with an empty line.
 * 
 *    Rev 1.4   Apr 01 2001 15:16:26   oris
 * Updated inquire capability ioctl - diffrent input and output records.
 *
 *    Rev 1.3   Apr 01 2001 07:58:30   oris
 * copywrite notice.
 * FL_IOCTL_FORMAT_PHYSICAL_DRIVE ioctl is not under the LOW_LEVEL compilation flag.
 * Bug fix - BDK_GET_INFO no longer calls bdkCreate().
 *
 *    Rev 1.2   Feb 14 2001 02:15:00   oris
 * Updated inquire capabilities ioctl.
 *
 *    Rev 1.1   Feb 13 2001 01:52:20   oris
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
 * Those IOCTL were not qualed and the TrueFFS 5.0 should be release with revision 1.0 of this this file.
 *
 *    Rev 1.0   Feb 04 2001 11:37:36   oris
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

#include "flioctl.h"
#include "blockdev.h"

#ifdef IOCTL_INTERFACE

FLStatus flIOctl(IOreq FAR2 *ioreq1)
{
  IOreq ioreq2;
  void FAR1 *inputRecord;
  void FAR1 *outputRecord;

  inputRecord = ((flIOctlRecord FAR1 *)(ioreq1->irData))->inputRecord;
  outputRecord = ((flIOctlRecord FAR1 *)(ioreq1->irData))->outputRecord;
  ioreq2.irHandle = ioreq1->irHandle;

  switch (ioreq1->irFlags) {
    case FL_IOCTL_GET_INFO:
    {
      flDiskInfoOutput FAR1 *outputRec = (flDiskInfoOutput FAR1 *)outputRecord;

      ioreq2.irData = &(outputRec->info);
      outputRec->status = flVolumeInfo(&ioreq2);
      return outputRec->status;
    }

#ifdef DEFRAGMENT_VOLUME
    case FL_IOCTL_DEFRAGMENT:
    {
      flDefragInput FAR1 *inputRec = (flDefragInput FAR1 *)inputRecord;
      flDefragOutput FAR1 *outputRec = (flDefragOutput FAR1 *)outputRecord;

      ioreq2.irLength = inputRec->requiredNoOfSectors;
      outputRec->status = flDefragmentVolume(&ioreq2);
      outputRec->actualNoOfSectors = ioreq2.irLength;
      return outputRec->status;
    }
#endif /* DEFRAGMENT_VOLUME */
#ifndef FL_READ_ONLY
#ifdef WRITE_PROTECTION
    case FL_IOCTL_WRITE_PROTECT:
    {
      flWriteProtectInput FAR1 *inputRec = (flWriteProtectInput FAR1 *)inputRecord;
      flOutputStatusRecord FAR1 *outputRec = (flOutputStatusRecord FAR1 *)outputRecord;

      ioreq2.irData = inputRec->password;
      ioreq2.irFlags = inputRec->type;
      outputRec->status = flWriteProtection(&ioreq2);
      return outputRec->status;
    }
#endif /* WRITE_PROTECTION */
#endif /* FL_READ_ONLY */
    case FL_IOCTL_MOUNT_VOLUME:
    {
      flMountInput FAR1 *inputRec = (flMountInput FAR1 *)inputRecord;
      flOutputStatusRecord FAR1 *outputRec = (flOutputStatusRecord FAR1 *)outputRecord;

      if (inputRec->type == FL_DISMOUNT)
        outputRec->status = flDismountVolume(&ioreq2);
      else
        outputRec->status = flAbsMountVolume(&ioreq2);
      return outputRec->status;
    }

#ifdef FORMAT_VOLUME
    case FL_IOCTL_FORMAT_VOLUME:
    {
      flFormatInput FAR1 *inputRec = (flFormatInput FAR1 *)inputRecord;
      flOutputStatusRecord FAR1 *outputRec = (flOutputStatusRecord FAR1 *)outputRecord;

      ioreq2.irFlags = inputRec->formatType;
      ioreq2.irData = &(inputRec->fp);
      outputRec->status = flFormatVolume(&ioreq2);
      return outputRec->status;
    }

    case FL_IOCTL_FORMAT_LOGICAL_DRIVE:
    {
      flFormatLogicalInput FAR1 *inputRec = (flFormatLogicalInput FAR1 *)inputRecord;
      flOutputStatusRecord FAR1 *outputRec = (flOutputStatusRecord FAR1 *)outputRecord;

      ioreq2.irData = &(inputRec->fp);
      outputRec->status = flFormatLogicalDrive(&ioreq2);
      return outputRec->status;
    }

    case FL_IOCTL_FORMAT_PHYSICAL_DRIVE:
    {
      flFormatPhysicalInput FAR1 *inputRec = (flFormatPhysicalInput FAR1 *)inputRecord;
      flOutputStatusRecord FAR1 *outputRec = (flOutputStatusRecord FAR1 *)outputRecord;

      ioreq2.irFlags = inputRec->formatType;
      ioreq2.irData = &(inputRec->fp);
      outputRec->status = flFormatPhysicalDrive(&ioreq2);
      return outputRec->status;
    }
#endif /* FORMAT_VOLUME */

#ifdef BDK_ACCESS
    case FL_IOCTL_BDK_OPERATION:
    {
      flBDKOperationInput  FAR1 *inputRec  = (flBDKOperationInput  FAR1 *)inputRecord;
      flOutputStatusRecord FAR1 *outputRec = (flOutputStatusRecord FAR1 *)outputRecord;

      ioreq2.irData = &(inputRec->bdkStruct);
      switch(inputRec->type) {
        case BDK_INIT_READ:
          outputRec->status = bdkReadInit(&ioreq2);
          break;
        case BDK_READ:
          outputRec->status = bdkReadBlock(&ioreq2);
          break;
        case BDK_GET_INFO:
          outputRec->status = bdkPartitionInfo(&ioreq2);
          break;
#ifndef FL_READ_ONLY
        case BDK_INIT_WRITE:
          outputRec->status = bdkWriteInit(&ioreq2);
          break;
        case BDK_WRITE:
          outputRec->status = bdkWriteBlock(&ioreq2);
          break;
        case BDK_ERASE:
          outputRec->status = bdkErase(&ioreq2);
          break;
        case BDK_CREATE:
          outputRec->status = bdkCreate(&ioreq2);
          break;
#endif   /* FL_READ_ONLY */
    default:
      outputRec->status = flBadParameter;
          break;
      }
      return outputRec->status;
    }
#endif /* BDK_ACCESS */
#ifdef HW_PROTECTION
#ifdef BDK_ACCESS
    case FL_IOCTL_BINARY_HW_PROTECTION:
    {
      flProtectionInput    FAR1 *inputRec = (flProtectionInput FAR1 *)inputRecord;
      flProtectionOutput FAR1 *outputRec = (flProtectionOutput FAR1 *)outputRecord;

      switch(inputRec->type)
      {
    case PROTECTION_INSERT_KEY:
      ioreq2.irData = inputRec->key;
      outputRec->status = bdkInsertProtectionKey(&ioreq2);
      break;
    case PROTECTION_REMOVE_KEY:
      outputRec->status = bdkRemoveProtectionKey(&ioreq2);
      break;
    case PROTECTION_GET_TYPE:
      outputRec->status = bdkIdentifyProtection(&ioreq2);
      outputRec->protectionType = (byte)ioreq2.irFlags;
      break;
    case PROTECTION_DISABLE_LOCK:
      ioreq2.irFlags = 0;
      outputRec->status = bdkHardwareProtectionLock(&ioreq2);
      break;
    case PROTECTION_ENABLE_LOCK:
      ioreq2.irFlags = LOCK_ENABLED;
      outputRec->status = bdkHardwareProtectionLock(&ioreq2);
      break;
    case PROTECTION_CHANGE_KEY:
      ioreq2.irData = inputRec->key;
      outputRec->status = bdkChangeProtectionKey(&ioreq2);
      break;
    case PROTECTION_CHANGE_TYPE:
      ioreq2.irFlags = inputRec->protectionType;
      outputRec->status = bdkChangeProtectionType(&ioreq2);
      break;
        default:
          outputRec->status = flBadParameter;
          break;
      }
      return outputRec->status;
    }
#endif /* BDK_ACCESS */
    case FL_IOCTL_BDTL_HW_PROTECTION:
    {
      flProtectionInput  FAR1 *inputRec = (flProtectionInput FAR1 *)inputRecord;
      flProtectionOutput FAR1 *outputRec = (flProtectionOutput FAR1 *)outputRecord;

      switch(inputRec->type)
      {
    case PROTECTION_INSERT_KEY:
      ioreq2.irData = inputRec->key;
      outputRec->status = flInsertProtectionKey(&ioreq2);
      break;
    case PROTECTION_REMOVE_KEY:
      outputRec->status = flRemoveProtectionKey(&ioreq2);
      break;
    case PROTECTION_GET_TYPE:
      outputRec->status = flIdentifyProtection(&ioreq2);
      outputRec->protectionType = (byte)ioreq2.irFlags;
      break;
    case PROTECTION_DISABLE_LOCK:
      ioreq2.irFlags = 0;
      outputRec->status = flHardwareProtectionLock(&ioreq2);
      break;
    case PROTECTION_ENABLE_LOCK:
      ioreq2.irFlags = LOCK_ENABLED;
      outputRec->status = flHardwareProtectionLock(&ioreq2);
      break;
    case PROTECTION_CHANGE_KEY:
      ioreq2.irData = inputRec->key;
      outputRec->status = flChangeProtectionKey(&ioreq2);
      break;
    case PROTECTION_CHANGE_TYPE:
      ioreq2.irFlags = inputRec->protectionType;
      outputRec->status = flChangeProtectionType(&ioreq2);
      break;
    default:
      outputRec->status = flBadParameter;
      break;
      }
      return outputRec->status;
    }
#endif /* HW_PROTECTION */
#ifdef HW_OTP
    case FL_IOCTL_OTP:
    {
      flOtpInput           FAR1 *inputRec  = (flOtpInput FAR1 *)inputRecord;
      flOutputStatusRecord FAR1 *outputRec = (flOutputStatusRecord FAR1 *)outputRecord;

      switch(inputRec->type)
      {
         case OTP_SIZE:
          outputRec->status = flOTPSize(&ioreq2);
       inputRec->lockedFlag = (byte)ioreq2.irFlags;
           inputRec->length     = ioreq2.irCount ;
       inputRec->usedSize   = ioreq2.irLength ;
           break;
         case OTP_READ:
           ioreq2.irData   = inputRec->buffer;    /* user buffer  */
       ioreq2.irCount  = inputRec->usedSize;  /* offset       */
           ioreq2.irLength = inputRec->length;    /* size to read */
           outputRec->status = flOTPRead(&ioreq2);
       break;
         case OTP_WRITE_LOCK:
           ioreq2.irData   = inputRec->buffer;    /* user buffer  */
           ioreq2.irLength = inputRec->length;    /* size to read */
           outputRec->status = flOTPWriteAndLock(&ioreq2);
       break;
         default:
       outputRec->status = flBadParameter;
           break;
      }
      return outputRec->status;
    }

    case FL_IOCTL_CUSTOMER_ID:
    {
      flCustomerIdOutput FAR1 *outputRec = (flCustomerIdOutput FAR1 *)outputRecord;

      ioreq2.irData = outputRec->id;
      outputRec->status = flGetCustomerID(&ioreq2);
      return outputRec->status;
    }

    case FL_IOCTL_UNIQUE_ID:
    {
      flUniqueIdOutput FAR1 *outputRec = (flUniqueIdOutput FAR1 *)outputRecord;

      ioreq2.irData = outputRec->id;
      outputRec->status = flGetUniqueID(&ioreq2);
      return outputRec->status;
    }
#endif /* HW_OTP */

    case FL_IOCTL_NUMBER_OF_PARTITIONS:
    {
      flCountPartitionsOutput FAR1 *outputRec = (flCountPartitionsOutput FAR1 *)outputRecord;

      outputRec->status = flCountVolumes(&ioreq2);
      outputRec->noOfPartitions = (byte) ioreq2.irFlags;
      return outputRec->status;
    }

#ifdef FL_LOW_LEVEL

    case FL_IOCTL_INQUIRE_CAPABILITIES:
    {
      flCapabilityInput FAR1 *inputRec = (flCapabilityInput FAR1 *)inputRecord;
      flCapabilityOutput FAR1 *outputRec = (flCapabilityOutput FAR1 *)outputRecord;

      ioreq2.irLength       = inputRec->capability;
      outputRec->status     = flInquireCapabilities(&ioreq2);
      outputRec->capability = (FLCapability)ioreq2.irLength;
      return outputRec->status;
    }

#endif /* FL_LOW_LEVEL */
#ifdef FL_LOW_LEVEL
#ifdef WRITE_EXB_IMAGE

    case FL_IOCTL_PLACE_EXB_BY_BUFFER:
    {
      flPlaceExbInput      FAR1 *inputRec  = (flPlaceExbInput      FAR1 *)inputRecord;
      flOutputStatusRecord FAR1 *outputRec = (flOutputStatusRecord FAR1 *)outputRecord;
      ioreq2.irData       = inputRec->buf;
      ioreq2.irLength     = inputRec->bufLen;
      ioreq2.irWindowBase = inputRec->exbWindow;
      ioreq2.irFlags      = inputRec->exbFlags;
      outputRec->status   = flPlaceExbByBuffer(&ioreq2);
      return outputRec->status;
    }

#endif /* WRITE_EXB_IMAGE */

    case FL_IOCTL_DEEP_POWER_DOWN_MODE:
    {
      flPowerDownInput      FAR1 *inputRec  = (flPowerDownInput      FAR1 *)inputRecord;
      flOutputStatusRecord  FAR1 *outputRec = (flOutputStatusRecord  FAR1 *)outputRecord;
      ioreq2.irFlags    = inputRec->state;
      outputRec->status = flDeepPowerDownMode(&ioreq2);
      return outputRec->status;
    }

#endif /* FL_LOW_LEVEL */
#ifdef ABS_READ_WRITE
#ifndef FL_READ_ONLY
    case FL_IOCTL_DELETE_SECTORS:
    {
      flDeleteSectorsInput FAR1 *inputRec = (flDeleteSectorsInput FAR1 *)inputRecord;
      flOutputStatusRecord FAR1 *outputRec = (flOutputStatusRecord FAR1 *)outputRecord;

      ioreq2.irSectorNo = inputRec->firstSector;
      ioreq2.irSectorCount = inputRec->numberOfSectors;
      outputRec->status = flAbsDelete(&ioreq2);
      return outputRec->status;
    }
#endif  /* FL_READ_ONLY */

    case FL_IOCTL_READ_SECTORS:
    {
      flReadWriteInput FAR1 *inputRec = (flReadWriteInput FAR1 *)inputRecord;
      flReadWriteOutput FAR1 *outputRec = (flReadWriteOutput FAR1 *)outputRecord;

      ioreq2.irSectorNo = inputRec->firstSector;
      ioreq2.irSectorCount = inputRec->numberOfSectors;
      ioreq2.irData = inputRec->buf;
      outputRec->status = flAbsRead(&ioreq2);
      outputRec->numberOfSectors = ioreq2.irSectorCount;
      return outputRec->status;
    }
#ifndef FL_READ_ONLY

    case FL_IOCTL_WRITE_SECTORS:
    {
      flReadWriteInput FAR1 *inputRec = (flReadWriteInput FAR1 *)inputRecord;
      flReadWriteOutput FAR1 *outputRec = (flReadWriteOutput FAR1 *)outputRecord;

      ioreq2.irSectorNo = inputRec->firstSector;
      ioreq2.irSectorCount = inputRec->numberOfSectors;
      ioreq2.irData = inputRec->buf;
      outputRec->status = flAbsWrite(&ioreq2);
      outputRec->numberOfSectors = ioreq2.irSectorCount;
      return outputRec->status;
    }
#endif   /* FL_READ_ONLY */
#endif  /* ABS_READ_WRITE */

#ifdef ENVIRONMENT_VARS
    case FL_IOCTL_EXTENDED_ENVIRONMENT_VARIABLES:
    {
       flExtendedEnvVarsInput  FAR1 *inputRec  = (flExtendedEnvVarsInput   FAR1 *)inputRecord;
       flExtendedEnvVarsOutput FAR1 *outputRec = (flExtendedEnvVarsOutput  FAR1 *)outputRecord;

       switch(inputRec->flags)
       {
          case FL_APPLY_TO_ALL:
             outputRec->status = flSetEnvAll(inputRec->varName,
                                 inputRec->varValue,&(outputRec->prevValue));
             break;
          case FL_APPLY_TO_SOCKET:
             outputRec->status = flSetEnvSocket(inputRec->varName,
                                 FL_GET_SOCKET_FROM_HANDLE((&ioreq2)),
                                 inputRec->varValue,&(outputRec->prevValue));
             break;
          case FL_APPLY_TO_VOLUME:
             outputRec->status = flSetEnvVolume(inputRec->varName,
                                 FL_GET_SOCKET_FROM_HANDLE((&ioreq2)),
                                 FL_GET_PARTITION_FROM_HANDLE((&ioreq2)),
                                 inputRec->varValue,&(outputRec->prevValue));
             break;
          default:
             DEBUG_PRINT(("Debug: No such environment variable function flag\r\n"));
             outputRec->status = flFeatureNotSupported;
       }
       return outputRec->status;
    }
#endif /* ENVIRONMENT_VARS */

#ifndef FL_NO_USE_FUNC
    case FL_IOCTL_SET_ACCESS_ROUTINE:
    {
       flSetMemoryAccessInput FAR1 *inputRec = (flSetMemoryAccessInput FAR1 *)inputRecord;
       flOutputStatusRecord FAR1 *outputRec = (flOutputStatusRecord FAR1 *)outputRecord;

       outputRec->status = flSetDocBusRoutine(FL_GET_SOCKET_FROM_HANDLE((&ioreq2)),
                                              inputRec->structPtr);
       return outputRec->status;
    }
    case FL_IOCTL_GET_ACCESS_ROUTINE:
    {
       flGetMemoryAccessOutput FAR1 *outputRec = (flGetMemoryAccessOutput FAR1 *)outputRecord;

       outputRec->status = flGetDocBusRoutine(FL_GET_SOCKET_FROM_HANDLE((&ioreq2)),
                                              outputRec->structPtr);
       return outputRec->status;
    }
#endif /* FL_NO_USE_FUNC */

#ifdef VERIFY_VOLUME
    case FL_IOCTL_VERIFY_VOLUME:
    {
       flVerifyVolumeInput FAR1 *inputRec = (flVerifyVolumeInput FAR1 *)inputRecord;
       flVerifyVolumeOutput FAR1 *outputRec = (flVerifyVolumeOutput FAR1 *)outputRecord;     

       ioreq2.irLength   = (long)inputRec->flags;
       ioreq2.irData     = outputRec->callBack;
       outputRec->status = flVerifyVolume(&ioreq2);
       return outputRec->status;
    }
#endif /* VERIFY_VOLUME */

#ifndef NO_IPL_CODE
    case FL_IOCTL_EXTENDED_WRITE_IPL:
    {
      flIplInput           FAR1 *inputRec  = (flIplInput           FAR1 *)inputRecord;
      flOutputStatusRecord FAR1 *outputRec = (flOutputStatusRecord FAR1 *)outputRecord;
      ioreq2.irData       = inputRec->buf;
      ioreq2.irLength     = inputRec->bufLen;
      ioreq2.irFlags      = inputRec->flags;
      outputRec->status   = flWriteIPL(&ioreq2);
      return outputRec->status;
    }
#endif /* NO_IPL_CODE */

    default:
      return flBadParameter;
  }
}

#endif /* IOCTL_INTERFACE */
