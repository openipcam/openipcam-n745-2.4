/* $Id: bsd-getpeereid.h,v 1.1.1.1 2006-07-11 09:33:11 andy Exp $ */

#ifndef _BSD_GETPEEREID_H
#define _BSD_GETPEEREID_H

#include "config.h"

#include <sys/types.h> /* For uid_t, gid_t */

#ifndef HAVE_GETPEEREID
int	 getpeereid(int , uid_t *, gid_t *);
#endif /* HAVE_GETPEEREID */

#endif /* _BSD_GETPEEREID_H */
