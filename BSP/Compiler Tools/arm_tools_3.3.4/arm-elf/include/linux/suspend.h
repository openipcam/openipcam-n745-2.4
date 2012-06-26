/* $Id: suspend.h,v 1.1 2003/10/13 20:56:47 dwmw2 Exp $ */

#ifndef __MTD_COMPAT_VERSION_H__
#include <linux/version.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)
#include_next <linux/suspend.h>
#endif

#endif /* __MTD_COMPAT_VERSION_H__ */
