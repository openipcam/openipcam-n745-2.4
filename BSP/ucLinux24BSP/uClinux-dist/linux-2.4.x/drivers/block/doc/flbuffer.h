
/*
 * $Log: flbuffer.h,v $
 * Revision 1.1.1.1  2006-07-11 09:28:42  andy
 * W90N745 BSP
 *
 * Revision 1.1.1.1  2006/04/26 02:35:50  andy
 * W90N745 uCLinux kernel
 *
 * 
 *    Rev 1.3   Jul 13 2001 01:03:58   oris
 * Added read Back buffer size definition.
 * 
 *    Rev 1.2   May 16 2001 21:29:24   oris
 * Change "data" named variables to flData to avoid name clashes.
 * 
 *    Rev 1.2   May 16 2001 21:17:44   oris
 * Added backwards compatibility check for FL_MALLOC the new definition replacing MALLOC.
 * 
 *    Rev 1.1   Apr 01 2001 07:45:44   oris
 * Updated copywrite notice
 * 
 *    Rev 1.0   Feb 04 2001 11:17:06   oris
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

#ifndef FLBUFFER_H
#define FLBUFFER_H

#include "flbase.h"

#define READ_BACK_BUFFER_SIZE    1024 /* Size of read back buffer
                                         Must be multiplication of 512 */
typedef struct {
  unsigned char flData[SECTOR_SIZE];	/* sector buffer */
  SectorNo	sectorNo;		/* current sector in buffer */
  void		*owner;			/* owner of buffer */
  FLBoolean	dirty;			/* sector in buffer was changed */
  FLBoolean	checkPoint;		/* sector in buffer must be flushed */
} FLBuffer;

#endif
