/* $Id: getopt.h,v 1.1.1.1 2006-07-11 09:33:11 andy Exp $ */

#ifndef _BSDGETOPT_H
#define _BSDGETOPT_H

#include "config.h"

#if !defined(HAVE_GETOPT) || !defined(HAVE_GETOPT_OPTRESET)

int BSDgetopt(int argc, char * const *argv, const char *opts);

#endif

#endif /* _BSDGETOPT_H */
