/*
 * $Log: _fltl.h,v $
 * Revision 1.1.1.1  2006-07-11 09:28:45  andy
 * W90N745 BSP
 *
 * Revision 1.1.1.1  2006/04/26 02:35:51  andy
 * W90N745 uCLinux kernel
 *
 * 
 *    Rev 1.0   03 May 2002 20:14:48   andreyk
 * Initial revision.
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

#ifndef _FLTL_H
#define _FLTL_H

typedef struct {
  SectorNo sectorsInVolume;
  unsigned long bootAreaSize;
  unsigned long eraseCycles;
  unsigned long tlUnitBits;
} TLInfo;

/* See interface documentation of functions in ftllite.c    */

typedef struct tTLrec TLrec;     /* Defined by translation layer */

struct tTL {
  TLrec        *rec;
  byte      partitionNo;
  byte      socketNo;

  const void FAR0 *(*mapSector)(TLrec *, SectorNo sectorNo, CardAddress *physAddr);
  FLStatus       (*writeSector)(TLrec *, SectorNo sectorNo, void FAR1 *fromAddress);

  FLStatus       (*writeMultiSector)(TLrec *, SectorNo sectorNo, void FAR1 *fromAddress,SectorNo sectorCount);
  FLStatus       (*readSectors)(TLrec *, SectorNo sectorNo, void FAR1 *dest,SectorNo sectorCount);

  FLStatus       (*deleteSector)(TLrec *, SectorNo sectorNo, SectorNo noOfSectors);
  FLStatus       (*tlSetBusy)(TLrec *, FLBoolean);
  void           (*dismount)(TLrec *);

#ifdef DEFRAGMENT_VOLUME
  FLStatus       (*defragment)(TLrec *, long FAR2 *bytesNeeded);
#endif
#if (defined(VERIFY_VOLUME) || defined(VERIFY_WRITE) || defined(VERIFY_ERASED_SECTOR)) 
  FLStatus       (*checkVolume)(TLrec *);
#endif /* VERIFY_VOLUME || VERIFY_WRITE || VERIFY_ERASED_SECTOR */
  SectorNo       (*sectorsInVolume)(TLrec *);
  FLStatus       (*getTLInfo)(TLrec *, TLInfo *tlInfo);
  void           (*recommendedClusterInfo)(TLrec *, int *sectorsPerCluster, SectorNo *clusterAlignment);
#ifndef NO_READ_BBT_CODE
  FLStatus       (*readBBT)(TLrec *, CardAddress FAR1 * buf, long FAR2 * mediaSize, unsigned FAR2 * noOfBB);
#endif
};


#include "dosformt.h"

/* Translation layer registration information */

extern int noOfTLs;    /* No. of translation layers actually registered */

typedef struct {
  FLStatus (*mountRoutine)   (unsigned volNo, TL *tl, FLFlash *flash, FLFlash **volForCallback);
  FLStatus (*formatRoutine)  (unsigned volNo, TLFormatParams *deviceFormatParams, FLFlash *flash);
  FLStatus (*preMountRoutine)(FLFunctionNo callType, IOreq FAR2* ioreq ,FLFlash* flash,FLStatus* status);
} TLentry;

extern TLentry tlTable[TLS];
extern FLStatus noFormat (unsigned volNo, TLFormatParams *formatParams, FLFlash *flash);
extern FLStatus flMount(unsigned volNo, unsigned socketNo,TL *, FLBoolean useFilters , FLFlash *flash);
extern FLStatus flPreMount(FLFunctionNo callType, IOreq FAR2* ioreq , FLFlash *flash);
extern unsigned noOfDrives;

#ifdef FORMAT_VOLUME
extern FLStatus flFormat(unsigned volNo, TLFormatParams *formatParams, FLFlash * flash);

#define FL_LEAVE_BINARY_AREA 8
#endif

#endif /* FLTL_H */


