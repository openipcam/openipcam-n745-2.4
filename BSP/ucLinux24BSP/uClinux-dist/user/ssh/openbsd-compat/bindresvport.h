/* $Id: bindresvport.h,v 1.1.1.1 2006-07-11 09:33:11 andy Exp $ */

#ifndef _BSD_BINDRESVPORT_H
#define _BSD_BINDRESVPORT_H

#include "config.h"

#ifndef HAVE_BINDRESVPORT_SA
int bindresvport_sa(int sd, struct sockaddr *sa);
#endif /* !HAVE_BINDRESVPORT_SA */

#endif /* _BSD_BINDRESVPORT_H */
