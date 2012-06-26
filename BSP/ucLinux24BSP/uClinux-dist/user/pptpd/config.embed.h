/*
 * config.embed.h
 *
 * Dummy autoconf results for uClinux target.
 *
 * $Id: config.embed.h,v 1.1.1.1 2006-07-11 09:32:12 andy Exp $
 */

#include <features.h> /* for __UC_LIBC__ */

#define STDC_HEADERS 1
#define HAVE_SETSID 1
#define HAVE_MEMMOVE 1
#define HAVE_STRING_H 1
#define PPP_BINARY "/bin/pppd"
#define SBINDIR "/bin"
#if defined(__UCLIBC__) && !defined(__UC_LIBC__)
#define HAVE_STRLCPY 1
#endif
