/*
 * $Log: flxscale.c,v $
 * Revision 1.1.1.1  2006-07-11 09:28:45  andy
 * W90N745 BSP
 *
 * Revision 1.1.1.1  2006/04/26 02:35:52  andy
 * W90N745 uCLinux kernel
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
/*      FOR THE SPECIFIC TERMS AND CONDITIONS OF USE, OR CONTACT M-SYSTEMS         */
/*      FOR LICENSE ASSISTANCE: E-MAIL = info@m-sys.com                            */
/***********************************************************************************/

/************************************************************************/
/* File Header                                                          */
/* -----------                                                          */
/* Name : flxscale.c                                                    */
/*                                                                      */
/* Description : This file contains the memory access layer code for    */
/*               DiskOnChip MTD customized for the X-Scale CPU.         */
/*                                                                      */
/* Note : The code only supports 16-bit DiskOnChip connected with no    */
/*        address shift on to a platforms that supports 16-bit access   */
/*        to the DiskOnChip memory range and is configured with if_cfg  */
/*        set to 16-bit interface.                                      */
/* Note : The implementation takes into account that read, write and set */
/*        routines are always called with the offset of 0x800 Which is  */
/*        the case for the DiskOnChip Millnnium Plus and Mobile         */ 
/*        DiskonChip (mdocplus.c) MTD.                                  */
/*                                                                      */
/************************************************************************/

/**************************************************************************/
/* Instructions for including X-Scale support                             */
/* ------------------------------------------                             */
/* 1. Include flxscale.c in your application/driver project.              */
/* 2. Before calling any TrueFFS/BDK routine (even initialization         */
/*    routines) add the following lines of code:                          */
/*   a. #include "docsys.h"                                               */
/*   b. extern FLStatus flSetPlatformIsXscale(byte socketNo,dword flags); */
/*   c. flSetPlatformIsXscale(x,0);                                       */
/*                                                                        */
/* Notes                                                                  */
/* -----                                                                  */
/* 1. This file should be used only when DiskOnChip is connected onto     */
/*    XScale platforms.                                                   */
/* 2. The first argument of 2.c should in fact be the socket number for   */
/*    which you want to add X-Scale support. If you have only 1           */
/*    DiskOnChip device in your target platform, then you should use 0.   */
/*    In any other case use a for loop to initialize all of your socket.  */
/*    When using the Boot SDK the first argument must be set to 0         */
/* 3. The support for X-Scale processors is disabled when calling:        */
/*    a. bdkInit() routine while using the Boot SDK.                      */
/*    b. flExit() routine while using the TrueFFS SDK.                    */
/* 4. If you are using an X-Scale platform, you may remove the            */
/*    extra code found in docsys.c simply by leaving it out of the build  */
/*    and uncomment the setBusTypeOfFlash routine found below.           */
/**************************************************************************/

#ifndef XSCALE_H
#define XSCALE_H

#include "docsys.h"

#ifndef MTD_STANDALONE
void flInitGlobalVars(void);
#endif /* MTD_STANDALONE */

#ifndef FL_NO_USE_FUNC

/* 
 * If you want to remove the docsys.c file out of the build uncomment 
 * the following lines:
 */

/* FLStatus  setBusTypeOfFlash(FLFlash * flash,dword access)
 * {
 *   return flOK;
 * }
 */

/*********************************************************/
/*     Report DiskOnChip Memory Size                     */
/*********************************************************/

/*---------------------------------------------------------------------- 
   f l X s c a l e D o c M e m W i n S i z e N o S h i f t

   This routine is called from MTD to quary the size of the DiskOnChip
   memory window for none shifted DiskOnChip.
------------------------------------------------------------------------*/

dword flXscaleDocMemWinSizeNoShift(void)
{
  return 0x2000;
}

/*********************************************************/
/*     Write 16 bits to DiskOnChip memory window         */
/*********************************************************/

/*---------------------------------------------------------------------- 
   f l X s c a l e W r i t e 1 6 b i t U s i n g 1 6 b i t s N o S h i f t
                                                                       
   Note : offset must be 16-bits aligned.

   Write 16-bit Using 16-bits operands with no address shifted.       
------------------------------------------------------------------------*/

void flXscaleWrite16bitUsing16bitsNoShift(volatile  byte FAR0 * win, word offset,Reg16bitType val)
{
  ((volatile word FAR0*)win)[offset>>1] = val;
}

/*********************************************************/
/*     Read 16 bits from DiskOnChip memory window        */
/*********************************************************/

/*----------------------------------------------------------------------
   f l X s c a l e R e a d 1 6 b i t U s i n g 1 6 b i t s N o S h i f t    

   Note : offset must be 16-bits aligned.

   Read 16-bit Using 16-bits operands with no address shifted.       
------------------------------------------------------------------------*/

Reg16bitType flXscaleRead16bitUsing16bitsNoShift(volatile  byte FAR0 * win,word offset)
{
  return ((volatile word FAR0*)win)[offset>>1];
}

/*********************************************************/
/*     Write 8 bits to DiskOnChip memory window          */
/*********************************************************/

/*----------------------------------------------------------------------
   f l X s c a l e W r i t e 8 b i t U s i n g 16 b i t s N o S h i f t

   Note : DiskOnChip is connected with 16-bit data bus.
   Note : Data is written only to lower memory addresses.
                                                                       
   Write 8-bits Using 16-bits operands with no address shifted.       
------------------------------------------------------------------------*/

void flXscaleWrite8bitUsing16bitsNoShift(volatile  byte FAR0 * win, word offset,Reg8bitType val)
{
#ifdef FL_BIG_ENDIAN
  ((volatile word FAR0 *)win)[offset>>1] = ((word)val)<<8;
#else
  ((volatile word FAR0 *)win)[offset>>1] = (word)val;
#endif /* FL_BIG_ENDIAN */
}

/*********************************************************/
/*     Read 8 bits to DiskOnChip memory window           */
/*********************************************************/

/*----------------------------------------------------------------------
   f l X s c a l e R e a d 8 b i t U s i n g 16 b i t s N o S h i f t
  
   Note : DiskOnChip is connected with 16-bit data bus.

   Read 8-bits Using 16-bits operands with no address shifted.       
------------------------------------------------------------------------*/

Reg8bitType flXscaleRead8bitUsing16bitsNoShift(volatile  byte FAR0 * win,word offset)
{
#ifdef FL_BIG_ENDIAN
   return (((offset & 0x1) == 0) ?
#else
   return (( offset & 0x1      ) ?
#endif /* FL_BIG_ENDIAN */
           (Reg8bitType)(((volatile word FAR0 *)win)[offset>>1]>>8) :
           (Reg8bitType) ((volatile word FAR0 *)win)[offset>>1]    );
}

/*********************************************************/
/***    Operation on several bytes (read/write/set)    ***/
/*********************************************************/

/*********************************************************/
/*                   Interleave - 2                      */
/*********************************************************/

/*----------------------------------------------------------------------
   f l X s c a l e 1 6 b i t D o c R e a d N o S h i f t
 
   Note : Offset is always set to the DiskOnChip IO registers (0x800)

   Read 'count' bytes from M+ DiskOnChip with none shifted address bus.
------------------------------------------------------------------------*/

void flXscale16bitDocReadNoShift (volatile  byte FAR0 * win, word offset, byte FAR1 * dest, word count )
{
   volatile word FAR0 * swin = (volatile word FAR0 *)( win + 0x802);
   register int         i;
   register word        tmp;

#ifndef FL_NO_INIT_MMU_PAGES 
  if (count == 0)
     return;

  *dest = (byte)0;
  *((byte FAR1*)addToFarPointer(dest, (count - 1)) ) = (byte)0;
#endif /* FL_NO_INIT_MMU_PAGES */

   if( pointerToPhysical(dest) & 0x1 )
   {
      /* rare case: unaligned target buffer */
      for (i = 0; i < (int)count; )
      {
         tmp = *swin;
#ifdef FL_BIG_ENDIAN
         dest[i++] = (byte)(tmp>>8);
         dest[i++] = (byte)tmp;
#else
         dest[i++] = (byte)tmp;
         dest[i++] = (byte)(tmp>>8);
#endif /* FL_BIG_ENDIAN */
      }
   }
   else
   {  /* mainstream case */
      /* read in short words */
      for (i = 0, count = count >> 1; i < (int)count; i++)
         ((word FAR1 *)dest)[i] = *swin;
   }
}

/*----------------------------------------------------------------------
   f l X s c a l e 1 6 b i t D o c W r i t e N o S h i f t
 
   Note : Offset is always set to the DiskOnChip IO registers (0x800)

   Write 'count' bytes to M+ DiskOnChip with none shifted address bus.
------------------------------------------------------------------------*/

void flXscale16bitDocWriteNoShift ( volatile  byte FAR0 * win , word offset ,
                             byte FAR1 * src, word count )
{
   volatile word FAR0 * swin = (volatile word FAR0 *)( win + 0x802);
   register int         i;
   register word        tmp;

   if( pointerToPhysical(src) & 0x1 ) /* rare case: unaligned source buffer */
   {       
       for (i = 0; i < (int)count; i+=2)
       {
          /* tmp variable is just a precation from compiler optimizations */
#ifdef FL_BIG_ENDIAN
          tmp = ((word)src[i]<<8) + (word)src[i+1];
#else
          tmp = (word)src[i] + ((word)src[i+1]<<8);
#endif /* FL_BIG_ENDIAN */
          *swin = tmp;
		 }
   }
   else /* mainstream case */
   {    /* write in short words */      
      for (i = 0, count = count >> 1; i < (int)count; i++)
        *swin = ((word FAR1 *)src)[i];
   }
}

/*----------------------------------------------------------------------
   f l X s c a l e 1 6 b i t D o c S e t N o S h i f t
 
   Note : Offset is always set to the DiskOnChip IO registers (0x800)

   Set 'count' bytes of M+ DiskOnChip with none shifted address bus
------------------------------------------------------------------------*/

void flXscale16bitDocSetNoShift ( volatile  byte FAR0 * win , word offset ,
                                  word count , byte val)
{
   volatile word FAR0 * swin = (volatile word FAR0 *)( win + 0x802);
   register int         i;
   word                 tmpVal = (word)val * 0x0101;

   /* write in short words */
   for (i = 0; i < (int)count; i+=2)
      *swin = tmpVal;
}

/*********************************************************/
/*                   Interleave - 1                      */
/*********************************************************/

/*************************************************************/
/*    16-Bit DiskOnChip - No Shift - Only 8 bits are valid   */
/*************************************************************/

/*----------------------------------------------------------------------
   f l X s c a l e 1 6 b i t D o c R e a d N o S h i f t I g n o r e H i g h e r 8 B i t s

   Note : offset must be 16-bits aligned.
  
   Read 'count' bytes from M+ DiskOnChip connected with all 16 data bits, but
   in interleave-1 mode , therefore only one of the 8 bits contains actual data.
   The DiskOnChip is connected without an address shift.
------------------------------------------------------------------------*/

void flXscale16bitDocReadNoShiftIgnoreHigher8bits(volatile  byte FAR0 * win, word offset, byte FAR1 * dest, word count )
{
   volatile word FAR0 * swin = (volatile word FAR0 *)( win + 0x802);
   register int         i;

#ifndef FL_NO_INIT_MMU_PAGES 
  if (count == 0)
     return;

  *dest = (byte)0;
  *((byte FAR1*)addToFarPointer(dest, (count - 1)) ) = (byte)0;
#endif /* FL_NO_INIT_MMU_PAGES */

   for (i = 0; i < (int)count; i++)
   {
#ifdef FL_BIG_ENDIAN
      dest[i] = (byte)((*swin)>>8);
#else
      dest[i] = (byte)*swin;
#endif /* FL_BIG_ENDIAN */
   }
}

/*----------------------------------------------------------------------
   f l X s c a l e 1 6 D o c W r i t e N o S h i f t I g n o r e H i g h e r 8 b i t s

   Note : offset must be 16-bits aligned.

   Write 'count' bytes to M+ DiskOnChip connected with all 16 data bits, but
   in interleave-1 mode , therefore only one of the 8bits contains actual data.
   The DiskOnChip is connected without an address shift.
------------------------------------------------------------------------*/

void flXscale16bitDocWriteNoShiftIgnoreHigher8bits ( volatile  byte FAR0 * win , word offset ,
                             byte FAR1 * src, word count )
{
   volatile word FAR0 * swin = (volatile word FAR0 *)( win + 0x802);
   register int         i;

   for (i = 0; i < (int)count; i++)
   {
#ifdef FL_BIG_ENDIAN
      *swin  = ((word)src[i])<<8;
#else
      *swin  = (word)src[i];
#endif /* FL_BIG_ENDIAN */
   }
}

/*----------------------------------------------------------------------
   f l X s c a l e 1 6 D o c S e t N o S h i f t I g n o r e H i g h e r 8 b i t s

   Note : offset must be 16-bits aligned.

   Set 'count' bytes to M+ DiskOnChip connected with all 16 data bits, but
   in interleave-1 mode , therefore only one of the 8bits contains actual data.
   The DiskOnChip is connected without an address shift.
------------------------------------------------------------------------*/

void flXscale16bitDocSetNoShiftIgnoreHigher8bits ( volatile  byte FAR0 * win , word offset ,
                                  word count , byte val)
{
   volatile word FAR0 * swin = (volatile word FAR0 *)( win + 0x802);
   register int         i;
   word                 tmpVal = val * 0x0101;

   for (i = 0; i < (int)count; i++)
      *swin = tmpVal;
}

/***********************************/
/***    Registeration Routine    ***/
/***********************************/

/*----------------------------------------------------------------------*/
/*               f l I n s t a l l X s c a l e S u p p o r t            */
/*                                                                      */
/* Call back routine called by the MTD to install the proper access     */
/* routines for Xscale platform using 16-bit DiskOnChip configured with */
/* no address shift and if_cfg set to 16-bit interface.                 */
/*                                                                      */
/* D O   N O T   C A L L   T H I S   R O U T I N E                      */
/*                                                                      */
/* Use "flSetPlatformIsXscale" routine to install the X-Scale support.  */
/* This routine is called by the MTD, using the call back mechanism of  */
/* when chainging the interleave factor.                                */
/*                                                                      */
/* Parameters:                                                          */
/*      interleave : The device interleave factor.                      */
/*      socketNo   : The socket number for which to install the support */
/*      flags      : Must be set to 0.                                  */
/*----------------------------------------------------------------------*/

FLStatus flInstallXscaleSupport(byte interleave , byte socketNo , dword flags)
{
  FLFlash* flash;

  flash = flFlashOf(socketNo);

  flBusConfig[socketNo]  = FL_ACCESS_USER_DEFINED;
  flash->memWindowSize = flXscaleDocMemWinSizeNoShift;
  flash->memRead8bit   = flXscaleRead8bitUsing16bitsNoShift;
  flash->memWrite8bit  = flXscaleWrite8bitUsing16bitsNoShift;
  flash->memRead16bit  = flXscaleRead16bitUsing16bitsNoShift;
  flash->memWrite16bit = flXscaleWrite16bitUsing16bitsNoShift;
  flash->memSetGetMode = flInstallXscaleSupport;

  switch(interleave)
  {
    case 1:
       flash->memRead  = flXscale16bitDocReadNoShiftIgnoreHigher8bits;
       flash->memWrite = flXscale16bitDocWriteNoShiftIgnoreHigher8bits;
       flash->memSet   = flXscale16bitDocSetNoShiftIgnoreHigher8bits;
       break;

    case 2:
       flash->memRead  = flXscale16bitDocReadNoShift;
       flash->memWrite = flXscale16bitDocWriteNoShift;
       flash->memSet   = flXscale16bitDocSetNoShift;
       break;

    default:
       DEBUG_PRINT(("Error in access routines\r\n"));
       return flGeneralFailure;
  }

  return flOK;
}

/*----------------------------------------------------------------------*/
/*               f l S e t P l a t f o r m I s X s c a l e              */
/*                                                                      */
/* Initialize TrueFFS to support XScale bus for 16-bit DiskOnChip       */
/* Configured with no address shift and if_cfg of 16bit                 */
/*                                                                      */
/* Parameters:                                                          */
/*      socketNo   : The socket number for which to install the support */
/*      flags      : Must be set to 0.                                  */
/*----------------------------------------------------------------------*/

FLStatus flSetPlatformIsXscale(byte socketNo , dword flags)
{
   /* Arg sanity check */
   if (socketNo >= SOCKETS)
   {
      DEBUG_PRINT(("Error : change SOCKETS definition to support that many sockets.\r\n"));
      return flFeatureNotSupported;
   }

   /* Make sure global variables are initialized to their default values */
#ifdef MTD_STANDALONE
   bdkInit();          /* Boot SDK initialization */
#else
   flInitGlobalVars(); /* TrueFFS SDK initialization */
#endif /* MTD_STANDALONE */

  /* Install access routine for X-Scale proccessor 
   *
   * Note that although we use call "flInstallXscaleSupport" with interleave-1. 
   * The MTD will autonatically call this routine again with the proper 
   * interleave factor once it was detected.
   */
   return flInstallXscaleSupport(1,socketNo,flags);
}

#endif /* FL_NO_USE_FUNC */
#endif /* XSCALE_H       */

