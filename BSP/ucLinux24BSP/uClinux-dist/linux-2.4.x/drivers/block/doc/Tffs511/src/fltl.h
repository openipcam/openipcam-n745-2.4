/*
 * $Log: fltl.h,v $
 * Revision 1.1.1.1  2006-07-11 09:28:45  andy
 * W90N745 BSP
 *
 * Revision 1.1.1.1  2006/04/26 02:35:51  andy
 * W90N745 uCLinux kernel
 *
 * 
 *    Rev 1.8   03 May 2002 20:20:32   andreyk
 * private definitions moved to _FLTL.H
 * 
 *    Rev 1.7   Apr 15 2002 07:39:04   oris
 * Added support for VERIFY_ERASED_SECTOR compilation flag.
 * 
 *    Rev 1.6   Feb 19 2002 21:00:10   oris
 * Replaced blockev.h include directive with fltl.h and flreq.h
 * Added FL_LEAVE_BINARY_AREA definition.
 * 
 *    Rev 1.5   Jan 17 2002 23:02:54   oris
 * Added flash record as a parameter to flMount / flFormat / flPremount  prototypes
 * Added checkVolume routine pointer in the TL record.
 * Placed readBBT under NO_READ_BBT_CODE compilation flag.
 * Removed SINGLE_BUFFER compilation flag.
 * Added flash record as a parameter to flMount / flFormat / flPremount prototype.
 * 
 *    Rev 1.4   May 16 2001 21:19:56   oris
 * Made noOfDriver public.
 * 
 *    Rev 1.3   Apr 24 2001 17:09:02   oris
 * change readBBT routine interface.
 * 
 *    Rev 1.2   Apr 01 2001 07:57:48   oris
 * copywrite notice.
 * Changed readSectors prototype.
 * Aliggned left all # directives.
 * 
 *    Rev 1.1   Feb 14 2001 01:56:46   oris
 * Changed readBBT prototype.
 *
 *    Rev 1.0   Feb 04 2001 12:13:32   oris
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

#ifndef FLTL_H
#define FLTL_H

#include "flflash.h"
#include "flfuncno.h"
#include "flreq.h"

typedef struct tTL TL;

#include "_fltl.h"

#endif /* FLTL_H */


