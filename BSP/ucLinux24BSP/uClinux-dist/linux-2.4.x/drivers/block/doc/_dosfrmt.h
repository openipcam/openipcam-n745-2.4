
/*
 * $Log: _dosfrmt.h,v $
 * Revision 1.1.1.1  2006-07-11 09:28:42  andy
 * W90N745 BSP
 *
 * Revision 1.1.1.1  2006/04/26 02:35:48  andy
 * W90N745 uCLinux kernel
 *
 * 
 *    Rev 1.0   May 14 2002 14:59:24   oris
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

#ifndef _DOSFRMT_H
#define _DOSFRMT_H

#ifdef FORMAT_VOLUME

extern FLStatus flDosFormat(TL *, BDTLPartitionFormatParams FAR1 *formatParams);

#endif /* FORMAT_VOLUME */

#endif /* _DOSFRMT_H */


