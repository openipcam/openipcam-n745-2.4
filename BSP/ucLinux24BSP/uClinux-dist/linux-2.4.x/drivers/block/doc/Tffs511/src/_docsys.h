/*
 * $Log: _docsys.h,v $
 * Revision 1.1.1.1  2006-07-11 09:28:45  andy
 * W90N745 BSP
 *
 * Revision 1.1.1.1  2006/04/26 02:35:52  andy
 * W90N745 uCLinux kernel
 *
 * 
 *    Rev 1.1   May 14 2002 15:01:04   oris
 * Added CheckAccessMode defintion.
 * 
 *    Rev 1.0   May 02 2002 19:58:42   oris
 * Initial revision.
 */

/************************************************************************ 
 *                                                                      * 
 *        FAT-FTL Lite Software Development Kit                         * 
 *        Copyright (C) M-Systems Ltd. 1995-2001                        * 
 *                                                                      * 
 ************************************************************************ 
 *                                                                      * 
 *                          I M P O R T E N T                           *  
 *                                                                      * 
 * The file contains DiskOnChip memory access routines and macros       * 
 * defintions.                                                          * 
 *                                                                      * 
 * In order to use the complete set of TrueFFS memory access routine    * 
 * that allows runtime configuration of each socket access type make    * 
 * sure the FL_NO_USE_FUNC is not defined in either:                    * 
 * FLCUSTOME.H - when using TrueFFS SDK based application               * 
 * MTDSA.H     - when using Boot SDK based application                  * 
 *                                                                      * 
 * If you know the exact configuration of your application you can      * 
 * uncomment the FL_NO_USE_FUNC definition and set the proper access    * 
 * type using the macroe defintion bellow.                              * 
 *                                                                      * 
 ************************************************************************/

#ifndef _DOCSYS_H
#define _DOCSYS_H

#include "nanddefs.h"

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

#ifdef FL_NO_USE_FUNC 

#error "current version does not support the FL_NO_USE_FUNC compilation flag\r\n"

/* 
 * If you chose to customize the memory access routine using macroes, simply
 * add your implementation here.
 */
  
#define flWrite8bitReg(flash,offset,val)      
#define flRead8bitReg(flash,offset)           

#define docread     
#define docwrite    
#define docset      

/* DiskOnChip Plus memory access routines */

#define flWrite8bitRegPlus(flash,offset,val)  
#define flRead8bitRegPlus(flash,offset)         0x0
#define flWrite16bitRegPlus(flash,offset,val) 
#define flRead16bitRegPlus(flash,offset)        0x0

#define docPlusRead(win,offset,dest,count)    
#define docPlusWrite(win,offset,src,count)  
#define docPlusSet(win,offset,count,val)    

#define DOC_WIN                                 0x2000
#define setBusTypeOfFlash(flash,access)         flOK
#define CheckAccessMode(intNum,socketNo,flag)   /* Should be a routine */

#else

/* DiskOnChip memory access routines */

#define flWrite8bitReg(flash,offset,val)      flash->memWrite8bit(flash->win,offset,val)
#define flRead8bitReg(flash,offset)           flash->memRead8bit(flash->win,offset)

#define docread     flash->memRead
#define docwrite    flash->memWrite
#define docset      flash->memSet

/* DiskOnChip Plus memory access routines */

#define flWrite8bitRegPlus(flash,offset,val)  flash->memWrite8bit(flash->win,offset,val)
#define flRead8bitRegPlus(flash,offset)       flash->memRead8bit(flash->win,offset)
#define flWrite16bitRegPlus(flash,offset,val) flash->memWrite16bit(flash->win,offset,val)
#define flRead16bitRegPlus(flash,offset)      flash->memRead16bit(flash->win,offset)

#define docPlusRead(win,offset,dest,count)    flash->memRead(win,offset,dest,count)
#define docPlusWrite(win,offset,src,count)    flash->memWrite(win,offset,src,count)
#define docPlusSet(win,offset,count,val)      flash->memSet(win,offset,count,val)

#define DOC_WIN                               flash->memWindowSize()
#define CheckAccessMode                       flash->memSetGetMode

/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/

/* (private) types of DiskOnChip access configurations */

#define FL_8BIT_DOC_ACCESS         0x00000000L /* Has 8 data bits            */
#define FL_16BIT_DOC_ACCESS        0x00000100L /* Has 16 data bits           */
#define FL_XX_DATA_BITS_MASK       0x00000300L /* Mask of the above          */
#define FL_8BIT_FLASH_ACCESS       0x00000400L /* 8 bits of flash per cycle  */
#define FL_16BIT_FLASH_ACCESS      0x00000800L /* 16 bits of flash per cycle */
#define FL_XX_FLASH_ACCESS_MASK    0x00000C00L /* Mask of the above          */

#define FL_ACCESS_USER_DEFINED     0x00001000L /* User defined routines */

/* DiskOnChip routines prototypes */

extern FLStatus  setBusTypeOfFlash(FLFlash * flash,dword access);

#endif /* FL_NO_USE_FUNC */
#endif /* DOCSYS_H */
