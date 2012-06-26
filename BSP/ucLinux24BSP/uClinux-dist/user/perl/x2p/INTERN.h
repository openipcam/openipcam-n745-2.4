/* $RCSfile: INTERN.h,v $$Revision: 1.1.1.1 $$Date: 2006-07-11 09:32:05 $
 *
 *    Copyright (c) 1991-2001, Larry Wall
 *
 *    You may distribute under the terms of either the GNU General Public
 *    License or the Artistic License, as specified in the README file.
 *
 * $Log: INTERN.h,v $
 * Revision 1.1.1.1  2006-07-11 09:32:05  andy
 * W90N745 BSP
 *
 * Revision 1.1.1.1  2006/04/26 02:32:47  andy
 * W90N745 uCLinux kernel
 *
 */

#undef EXT
#define EXT

#undef INIT
#define INIT(x) = x

#define DOINIT
