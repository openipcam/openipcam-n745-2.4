
/*
 * $Log: flsocket.h,v $
 * Revision 1.1.1.1  2006-07-11 09:28:42  andy
 * W90N745 BSP
 *
 * Revision 1.1.1.1  2006/04/26 02:35:48  andy
 * W90N745 uCLinux kernel
 *
 * 
 *    Rev 1.6   May 14 2002 15:07:52   oris
 * Separated private definitions to _flsoc.h
 * 
 *    Rev 1.5   Apr 15 2002 07:37:06   oris
 * Added support for VERIFY_ERASED_SECTOR compilation flag.
 * 
 *    Rev 1.4   Feb 19 2002 20:59:58   oris
 * Removed include directive to flbase.h
 * 
 *    Rev 1.3   Jan 17 2002 23:02:22   oris
 * Added flReadBackBufferOf  prototype
 * Added curPartition and verifyWrite variables in the socket record.
 * 
 *    Rev 1.2   Jul 13 2001 01:05:44   oris
 * Add forward definition for get read back buffer pointer.
 * 
 *    Rev 1.1   Apr 01 2001 07:46:04   oris
 * Updated copywrite notice
 * 
 *    Rev 1.0   Feb 04 2001 11:53:24   oris
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


#ifndef FLSOCKET_H
#define FLSOCKET_H

#include "flbuffer.h"

#define ATTRIBUTE_SPACE_MAPPED   0x8000
#define UNDEFINED_MAPPING        0x7fff


typedef enum {PowerOff, PowerGoingOff, PowerOn} PowerState;

typedef unsigned long CardAddress;      /* Physical offset on card */

extern byte noOfSockets;    /* No. of drives actually registered */

/* The milliseconds counter is active when socket polling is enabled. When
   the socket interval routine is called, the counter is incremented by
   the interval in milliseconds.
   The counter can be used to avoid getting in a loop that is not guaranteed
   to terminate (such as waiting for a flash status register). Save the counter
   at entry to the loop, and check in the loop the amount of time that
   was spent in the loop. */

extern dword flMsecCounter;

typedef struct tSocket FLSocket;

#include "_flsoc.h"

#endif /* FLSOCKET_H */

