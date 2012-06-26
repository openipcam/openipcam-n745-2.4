/*
 * $Id: compatmac.h,v 1.1.1.1 2006-07-11 09:30:17 andy Exp $
 *
 * Extensions and omissions from the normal 'linux/compatmac.h'
 * files. hopefully this will end up empty as the 'real' one 
 * becomes fully-featured.
 */

#ifndef __LINUX_MTD_COMPATMAC_H__
#define __LINUX_MTD_COMPATMAC_H__

#include <linux/version.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,4,10)
#error "This kernel is too old: not supported by this file"
#endif

	/* O(1) scheduler stuff. */

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,5,5) && !defined(__rh_config_h__)
#include <linux/sched.h>
static inline void __recalc_sigpending(void)
{
	recalc_sigpending(current);
}
#undef recalc_sigpending
#define recalc_sigpending() __recalc_sigpending ()

#define set_user_nice(tsk, n) do { (tsk)->nice = n; } while(0)
#endif



#if LINUX_VERSION_CODE < KERNEL_VERSION(2,4,20)

#ifndef yield
#define yield() do { set_current_state(TASK_RUNNING); schedule(); } while(0)
#endif

#ifndef minor
#define major(d) (MAJOR(to_kdev_t(d)))
#define minor(d) (MINOR(to_kdev_t(d)))
#endif

#ifndef mk_kdev
#define mk_kdev(ma,mi) MKDEV(ma,mi)
#define kdev_t_to_nr(x)	(x)
#endif

#define need_resched() (current->need_resched)
#define cond_resched() do { if need_resched() { yield(); } } while(0)

#endif /* < 2.4.20 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,5,73)
#define iminor(i) minor((i)->i_rdev)
#define imajor(i) major((i)->i_rdev)
#define old_encode_dev(d) ( (major(d)<<8) | minor(d) )
#define old_decode_dev(rdev)  (kdev_t_to_nr(mk_kdev((rdev)>>8, (rdev)&0xff)))
#define old_valid_dev(d) (1)
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,5,61)

#include <linux/sched.h>

#ifdef __rh_config_h__
#define sigmask_lock sighand->siglock
#define sig sighand
#endif

static inline void __daemonize_modvers(void)
{
	daemonize();

	spin_lock_irq(&current->sigmask_lock);
	sigfillset(&current->blocked);
	recalc_sigpending();
	spin_unlock_irq(&current->sigmask_lock);
}
#undef daemonize
#define daemonize(fmt, ...) do {						\
	snprintf(current->comm, sizeof(current->comm), fmt ,##__VA_ARGS__);	\
	__daemonize_modvers();							\
	} while(0)

static inline int dequeue_signal_lock(struct task_struct *tsk, sigset_t *mask, siginfo_t *info)
{
	unsigned long flags;
	unsigned long ret;

	spin_lock_irqsave(&current->sigmask_lock, flags);
	ret = dequeue_signal(mask, info);
	spin_unlock_irqrestore(&current->sigmask_lock, flags);

	return ret;
}

static inline int allow_signal(int sig)
{
	if (sig < 1 || sig > _NSIG)
		return -EINVAL;

        spin_lock_irq(&current->sigmask_lock);
	sigdelset(&current->blocked, sig);
	recalc_sigpending();
	/* Make sure the kernel neither eats it now converts to SIGKILL */
	current->sig->action[sig-1].sa.sa_handler = (void *)2;
	spin_unlock_irq(&current->sigmask_lock);
	return 0;
}
static inline int disallow_signal(int sig)
{
	if (sig < 1 || sig > _NSIG)
		return -EINVAL;

	spin_lock_irq(&current->sigmask_lock);
	sigaddset(&current->blocked, sig);
	recalc_sigpending();

	current->sig->action[sig-1].sa.sa_handler = SIG_DFL;
	spin_unlock_irq(&current->sigmask_lock);
	return 0;
}

#define PF_FREEZE 0
#define refrigerator(x) do { ; } while(0)
#endif

	/* Module bits */


#if LINUX_VERSION_CODE < KERNEL_VERSION(2,5,60)
#define try_module_get(m) try_inc_mod_count(m)
#define __module_get(m) do { if (!try_inc_mod_count(m)) BUG(); } while(0)
#define module_put(m) do { if (m) __MOD_DEC_USE_COUNT((struct module *)(m)); } while(0)
#define set_module_owner(x) do { x->owner = THIS_MODULE; } while(0)
#endif


	/* Random filesystem stuff, only for JFFS2 really */

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,5,5)
#define parent_ino(d) ((d)->d_parent->d_inode->i_ino)
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,5,12)
#define PageUptodate(x) Page_Uptodate(x)
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,5,48)
#define get_seconds() CURRENT_TIME
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,5,53)
#define generic_file_readonly_mmap generic_file_mmap
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,5,70)

#include <linux/kmod.h>
#include <linux/string.h>

static inline char *strlcpy(char *dest, const char *src, int len)
{
	dest[len-1] = 0;
	return strncpy(dest, src, len-1);
}

static inline int do_old_request_module(const char *mod)
{
	return request_module(mod);
}
#undef request_module
#define request_module(fmt, ...) \
 ({ char modname[32]; snprintf(modname, 31, fmt ,##__VA_ARGS__); do_old_request_module(modname); })

#endif /* 2.5.70 */

#ifndef container_of
#define container_of(ptr, type, member) ({		     \
	const typeof( ((type *)0)->member ) *__mptr = (ptr); \
	(type *)( (char *)__mptr - offsetof(type,member) );})
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,5,0)
#define kvec iovec
#define __user 
#endif

#ifndef __iomem
#define __iomem
#endif

#ifndef list_for_each_entry_safe
/**
 * list_for_each_entry_safe - iterate over list of given type safe against removal of list entry
 * @pos:	the type * to use as a loop counter.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your list.
 * @member:	the name of the list_struct within the struct.
 */
#define list_for_each_entry_safe(pos, n, head, member)			\
	for (pos = list_entry((head)->next, typeof(*pos), member),	\
		n = list_entry(pos->member.next, typeof(*pos), member);	\
	     &pos->member != (head); 					\
	     pos = n, n = list_entry(n->member.next, typeof(*n), member))

#endif

#endif /* __LINUX_MTD_COMPATMAC_H__ */
