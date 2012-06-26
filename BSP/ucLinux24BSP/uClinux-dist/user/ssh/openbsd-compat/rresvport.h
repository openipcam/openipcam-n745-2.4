/* $Id: rresvport.h,v 1.1.1.1 2006-07-11 09:33:11 andy Exp $ */

#ifndef _BSD_RRESVPORT_H
#define _BSD_RRESVPORT_H

#include "config.h"

#ifndef HAVE_RRESVPORT_AF
int rresvport_af(int *alport, sa_family_t af);
#endif /* !HAVE_RRESVPORT_AF */

#endif /* _BSD_RRESVPORT_H */
