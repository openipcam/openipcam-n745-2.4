/*
 * $Log: flsystem.c,v $
 * Revision 1.1.1.1  2006-07-11 09:28:44  andy
 * W90N745 BSP
 *
 * Revision 1.1.1.1  2006/04/26 02:35:48  andy
 * W90N745 uCLinux kernel
 *
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

/* flsystem.c for linux kernel mode*/

#include <stdcomp.h>
#include <asm/semaphore.h>

#ifndef EXTERNAL_MUTEX
static struct semaphore osak_sem = MUTEX_LOCKED;
#endif

void flSysfunInit(void)
{
#ifndef EXTERNAL_MUTEX
	up(&osak_sem);
#endif
}

/*----------------------------------------------------------------------*/
/*                     f l C r e a t e M u t e x                        */
/*                                                                      */
/* Creates or initializes a mutex                                       */
/*                                                                      */
/* Parameters:                                                          */
/*      mutex           : Pointer to mutex                              */
/*                                                                      */
/* Returns:                                                             */
/*      FLStatus        : 0 on success, otherwise failure               */
/*----------------------------------------------------------------------*/

FLStatus flCreateMutex(FLMutex *mutex)
{
#ifndef EXTERNAL_MUTEX
	up(&osak_sem);
#endif
	return flOK;
}

/*----------------------------------------------------------------------*/
/*                     f l D e l e t e M u t e x                        */
/*                                                                      */
/* Deletes a mutex.                                                     */
/*                                                                      */
/* Parameters:                                                          */
/*      mutex           : Pointer to mutex                              */
/*                                                                      */
/*----------------------------------------------------------------------*/

void flDeleteMutex(FLMutex *mutex)
{
#ifndef EXTERNAL_MUTEX
	up(&osak_sem);
#endif
}

/*----------------------------------------------------------------------*/
/*                      f l T a k e M u t e x                           */
/*                                                                      */
/* Try to take mutex, if free.                                          */
/*                                                                      */
/* Parameters:                                                          */
/*      mutex           : Pointer to mutex                              */
/*                                                                      */
/* Returns:                                                             */
/*      int             : TRUE = Mutex taken, FALSE = Mutex not free    */
/*----------------------------------------------------------------------*/

FLBoolean flTakeMutex(FLMutex *mutex)
{
#ifndef EXTERNAL_MUTEX
	down(&osak_sem);
#endif
	return TRUE;
}

/*----------------------------------------------------------------------*/
/*                        f l F r e e M u t e x                         */
/*                                                                      */
/* Free mutex.                                                          */
/*                                                                      */
/* Parameters:                                                          */
/*      mutex           : Pointer to mutex                              */
/*                                                                      */
/*----------------------------------------------------------------------*/

void flFreeMutex(FLMutex *mutex)
{
#ifndef EXTERNAL_MUTEX
	up(&osak_sem);
#endif
}

void FAR0 *flAddLongToFarPointer(void FAR0 *ptr, unsigned long offset)
{
  return ( (void *) ( (unsigned char *)(ptr)+(offset) ) );
}

/*----------------------------------------------------------------------*/
/*                        f l s l e e p                                 */
/*                                                                      */
/* Wait number of milliseconds with yield CPU.                          */
/*                                                                      */
/* Parameters:                                                          */
/*      msec            : Minimum number of milliseconds to wait        */
/*                                                                      */
/*----------------------------------------------------------------------*/
/* Defined in Linux driver layer */
