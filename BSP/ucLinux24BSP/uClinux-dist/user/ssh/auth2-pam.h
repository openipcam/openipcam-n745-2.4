/* $Id: auth2-pam.h,v 1.1.1.1 2006-07-11 09:33:08 andy Exp $ */

#include "includes.h"
#ifdef USE_PAM

int	auth2_pam(Authctxt *authctxt);

#endif /* USE_PAM */
