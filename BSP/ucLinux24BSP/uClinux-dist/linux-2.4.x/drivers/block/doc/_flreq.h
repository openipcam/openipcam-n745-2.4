/*
 * $Log: _flreq.h,v $
 * Revision 1.1.1.1  2006-07-11 09:28:42  andy
 * W90N745 BSP
 *
 * Revision 1.1.1.1  2006/04/26 02:35:50  andy
 * W90N745 uCLinux kernel
 *
 * 
 *    Rev 1.0   May 14 2002 14:59:40   oris
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

#ifndef _FLREQ_H
#define _FLREQ_H

/*----------------------------------------------------------------------*/
/*                         f l I n i t                                  */
/*                                                                      */
/* Initializes the FLite system.                                        */
/*                                                                      */
/* Calling this function is optional. If it is not called,              */
/* initialization will be done automatically on the first FLite call.   */
/* This function is provided for those applications who want to	        */
/* explicitly initialize the system and get an initialization status.   */
/*                                                                      */
/* Calling flInit after initialization was done has no effect.          */
/*                                                                      */
/* Parameters:                                                          */
/*      None                                                            */
/*                                                                      */
/* Returns:                                                             */
/*      FLStatus      : 0 on success, otherwise failed                  */
/*----------------------------------------------------------------------*/

extern FLStatus flInit(void);

#endif /* _FLREQ_H */
