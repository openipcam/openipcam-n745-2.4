/*
 * 2.5 compatibility 
 * $Id: workqueue.h,v 1.1 2002/11/11 16:39:10 dwmw2 Exp $
 */

#ifndef __MTD_COMPAT_WORKQUEUE_H__
#define __MTD_COMPAT_WORKQUEUE_H__

#include <linux/version.h>

#if LINUX_VERSION_CODE > KERNEL_VERSION(2,5,40)
#include_next <linux/workqueue.h>
#else
#include <linux/tqueue.h>
#define work_struct tq_struct
#define schedule_work(x) schedule_task(x)
#define flush_scheduled_work flush_scheduled_tasks
#define INIT_WORK(x,y,z) INIT_TQUEUE(x,y,z)
#endif

#endif /* __MTD_COMPAT_WORKQUEUE_H__ */
