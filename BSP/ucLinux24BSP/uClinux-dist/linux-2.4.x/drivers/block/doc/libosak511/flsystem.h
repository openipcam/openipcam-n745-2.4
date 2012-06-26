/*
 * $Log: flsystem.h,v $
 * Revision 1.1.1.1  2006-07-11 09:28:45  andy
 * W90N745 BSP
 *
 * Revision 1.1.1.1  2006/04/26 02:35:50  andy
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

/* flsystem.h for linux kernel mode*/

#ifndef FLSYSTEM_H
#define FLSYSTEM_H

extern void * flkmalloc(unsigned long size);
extern void flkfree(void * bl);
int flprintk(unsigned char fDebug,const char *fmt, ...);

/*
 * 			signed/unsigned char
 *
 * It is assumed that 'char' is signed. If this is not your compiler
 * default, use compiler switches, or insert a #pragma here to define this.
 *
 */

	/* char is signed in GNU C by default */


/* 			CPU target
 *
 * Use compiler switches or insert a #pragma here to select the CPU type
 * you are targeting.
 *
 * If the target is an Intel 80386 or above, also uncomment the CPU_i386
 * definition.
 */

	/* defined in a Makefile */

/* 			NULL constant
 *
 * Some compilers require a different definition for the NULL pointer
 */

#define NULL ((void *) 0)


/* 			Little-endian/big-endian
 *
 * FAT and translation layers structures use the little-endian (Intel)
 * format for integers.
 * If your machine uses the big-endian (Motorola) format, uncomment the
 * following line.
 * Note that even on big-endian machines you may omit the BIG_ENDIAN
 * definition for smaller code size and better performance, but your media
 * will not be compatible with standard FAT and FTL.
 */

/* #define BIG_ENDIAN */


/* 			Far pointers
 *
 * Specify here which pointers may be far, if any.
 * Far pointers are usually relevant only to 80x86 architectures.
 *
 * Specify FAR_LEVEL:
 *   0 -	if using a flat memory model or having no far pointers.
 *   1 -        if only the socket window may be far
 *   2 -	if only the socket window and caller's read/write buffers
 *		may be far.
 *   3 -	if socket window, caller's read/write buffers and the
 *		caller's I/O request packet may be far
 */

#define FAR_LEVEL	0


/* 			Memory routines
 *
 * You need to supply library routines to copy, set and compare blocks of
 * memory, internally and to/from callers. The code uses the names 'tffscpy',
 * 'tffsset' and 'tffscmp' with parameters as in the standard 'memcpy',
 * 'memset' and 'memcmp' C library routines.
 */

/* 			Pointer arithmetic
 *
 * The following macros define machine- and compiler-dependent macros for
 * handling pointers to physical window addresses. The definitions below are
 * for PC real-mode Borland-C.
 *
 * 'physicalToPointer' translates a physical flat address to a (far) pointer.
 * Note that if when your processor uses virtual memory, the code should
 * map the physical address to virtual memory, and return a pointer to that
 * memory (the size parameter tells how much memory should be mapped).
 *
 * 'addToFarPointer' adds an increment to a pointer and returns a new
 * pointer. The increment may be as large as your window size. The code
 * below assumes that the increment may be larger than 64 KB and so performs
 * huge pointer arithmetic.
 */

#define physicalToPointer(physical,size,drive) ((void *) (physical))

#define pointerToPhysical(ptr)  ((unsigned long)(ptr))

#define addToFarPointer(base,increment) ((void *) ((unsigned char *) (base) + (increment)))

/* 			Default calling convention
 *
 * C compilers usually use the C calling convention to routines (cdecl), but
 * often can also use the pascal calling convention, which is somewhat more
 * economical in code size. Some compilers also have specialized calling
 * conventions which may be suitable. Use compiler switches or insert a
 * #pragma here to select your favorite calling convention.
 */

/* use GNU C default calling convention */

/* Naming convention for functions that uses non-default convention. 
 *
 * In case the calling application uses a different convention then the one 
 * used to compile TrueFFS you can use the NAMING_CONVENTION definition. 
 * The NAMING_CONVENTION definition is added as a qualifier to all TrueFFS 
 * exported API. A good example is a c++ application that uses TrueFFS, 
 * which was compiled using standard C.If this is not the case simply leave 
 * the NAMING_CONVENTION defined as empty macro.
 */

#define NAMING_CONVENTION


/* 			Mutex type
 *
 * If you intend to access the FLite API in a multi-tasking environment,
 * you may need to implement some resource management and mutual-exclusion
 * of FLite with mutex & semaphore services that are available to you. In
 * this case, define here the Mutex type you will use, and provide your own
 * implementation of the Mutex functions incustom.c
 *
 * By default, a Mutex is defined as a simple counter, and the Mutex
 * functions in custom.c implement locking and unlocking by incrementing
 * and decrementing the counter. This will work well on all single-tasking
 * environment, as well as on many multi-tasking environments.
 */

typedef int FLMutex;

#define flStartCriticalSection(FLMutex)
#define flEndCriticalSection(FLMutex)

/* 			Memory allocation
 *
 * The translation layers (e.g. FTL) need to allocate memory to handle
 * Flash media. The size needed depends on the media being handled.
 *
 * You may choose to use the standard 'malloc' and 'free' to handle such
 * memory allocations, provide your own equivalent routines, or you may
 * choose not to define any memory allocation routine. In this case, the
 * memory will be allocated statically at compile-time on the assumption of
 * the largest media configuration you need to support. This is the simplest
 * choice, but may cause your RAM requirements to be larger than you
 * actually need.
 *
 * If you define routines other than malloc & free, they should have the
 * same parameters and return types as malloc & free. You should either code
 * these routines in flcustom.c or include them when you link your application.
 */


#define MALLOC(a) flvmalloc(a)
#define FREE flvfree


/*			Debug mode
 *
 * Uncomment the following lines if you want debug messages to be printed
 * out. Messages will be printed at initialization key points, and when
 * low-level errors occure.
 * You may choose to use 'printf' or provide your own routine.
 */

#define DEBUG_PRINT(args...) flprintk(1, ## args)
#define DEBUG_PRINT_MALLOC(s,p) flprintk(1,"%s:malloc %x\n",s,p)
#define DEBUG_PRINT_FREE(s,p) flprintk(1,"%s:free %x\n",s,p)

#include "fllnx.h"

/* Memory routines
 *
 * The library routines memcpy, memset, and memcmp are standard for ANSI-C
 * compilers. However, there may be variations in which a #include defines
 * them, and variations such as the Borland's _fmemcpy, _fmemset, and _fmemcmp
 * routines also exist.To overcome such nuances, the TrueFFS code uses memory
 * handling macros called tffscpy, tffsset, and tffscmp, with prototypes 
 * identical to the standard memcpy, memset, memcmp.Provide the correct 
 * #include directive, and incorporate C macros that define the tffsxxx 
 * routines to the names of your library routines (or user-supplied routines
 * to manually code these routines).
 */

#ifndef ENVIRONMENT_VARS
#if FAR_LEVEL > 0
#define tffscpy _fmemcpy
#define tffscmp _fmemcmp
#define tffsset _fmemset
#else /* FAR_LEVEL > 0 */
#define tffscpy memcpy
#define tffscmp memcmp
#define tffsset memset
#endif /* FAR_LEVEL > 0 */
#else /* ENVIRONMENT_VARS */ 
#if FAR_LEVEL > 0
#define flcpy _fmemcpy
#define flcmp _fmemcmp
#define flset _fmemset
#else /* FAR_LEVEL > 0 */
#define flcpy flmemcpy
#define flcmp flmemcmp
#define flset flmemset
#endif /* FAR_LEVEL > 0 */
#endif /* ENVIRONMENT_VARS */

#include <linux/sockios.h>
#define FL_IOCTL_START  SIOCDEVPRIVATE

#define freePointer(ptr,size) 1

/*
 * Yielding the CPU
 *
 * TrueFFS utilizes the routine flSleep to yield the CPU while
 * waiting for time consuming operations like a flash erase. 
 * If the routine is not implemented, then uncomment the define below.
 */

/* #define DO_NOT_YIELD_CPU */

#endif
