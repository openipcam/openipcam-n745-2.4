/* $Id: setproctitle.h,v 1.1.1.1 2006-07-11 09:33:11 andy Exp $ */

#ifndef _BSD_SETPROCTITLE_H
#define _BSD_SETPROCTITLE_H

#include "config.h"

#ifndef HAVE_SETPROCTITLE
void setproctitle(const char *fmt, ...);
#endif

#endif /* _BSD_SETPROCTITLE_H */
