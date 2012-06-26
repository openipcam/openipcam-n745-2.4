/* $Id: suspend.h,v 1.1.1.1 2006-07-11 09:30:14 andy Exp $ */

#ifndef __MTD_COMPAT_VERSION_H__
#include <linux/version.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
#include_next <linux/suspend.h>
#endif

#endif /* __MTD_COMPAT_VERSION_H__ */
