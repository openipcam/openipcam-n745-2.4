/*
 * $Log: fldrvlnx.c,v $
 * Revision 1.1.1.1  2006-07-11 09:28:42  andy
 * W90N745 BSP
 *
 * Revision 1.1.1.1  2006/04/26 02:35:50  andy
 * W90N745 uCLinux kernel
 *
 * 
 */

/***********************************************************************************/
/*                        M-Systems Confidential                                   */
/*           Copyright (C) M-Systems Flash Disk Pioneers Ltd. 1995-2001            */
/*                         All Rights Reserved                                     */
/***********************************************************************************/
/*                            NOTICE OF M-SYSTEMS OEM                              */
/*                           SOFTWARE LICENSE AGREEMENT                            */
/*                                                                                 */
/*      THE USE OF THIS SOFTWARE IS GOVERNED BY A SEPARATE LICENSE                 */
/*      AGREEMENT BETWEEN THE OEM AND M-SYSTEMS. REFER TO THAT AGREEMENT           */
/*      FOR THE SPECIFIC TERMS AND CONDITIONS OF USE,                              */
/*      OR CONTACT M-SYSTEMS FOR LICENSE ASSISTANCE:                               */
/*      E-MAIL = info@m-sys.com                                                    */
/***********************************************************************************/

/* Loadable M-Systems Linux driver for DOC devices. */

#ifndef __KERNEL__
#define __KERNEL__
#endif

#include <linux/autoconf.h>
#ifdef CONFIG_SMP
	#define __SMP__		/* must be defined before #include <linux/module.h> */
#endif

/*  get the kernel version */
#include <linux/version.h>

/* older kernels didn't define this */
#ifndef KERNEL_VERSION
#define KERNEL_VERSION(a,b,c) (((a) << 16) + ((b) << 8) + (c))
#endif

/* conditionally include depending on kernel version */

/* 2.4.X */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,0)

#warning "2.4.X - Work in Progress"

#ifdef MODULE
#include <linux/module.h>
#ifndef MODVERSIONS
#define MODVERSIONS
#include <linux/modversions.h>
#endif
#endif

#include <linux/init.h>

#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/malloc.h>
#include <linux/vmalloc.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/timer.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/hdreg.h>
#include <linux/delay.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/ioctl.h>
#include <linux/genhd.h>
#include <linux/hdreg.h>
#include <linux/sockios.h>
#include <linux/devfs_fs_kernel.h>
#include <linux/smp_lock.h>

#include <asm/io.h>
#include <asm/system.h>
#include <asm/segment.h>
#include <asm/uaccess.h>
#include <asm/delay.h>


#include "compat24.h"
static devfs_handle_t devfs_handle = NULL;

/* 2.3.X - not supported */
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2,3,0)
#error "Linux Kernel Version 2.3.0 not supported"

/* 2.2.X */
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2,2,0)

#warning "2.2.X - Stable"

#ifdef MODULE
#include <linux/module.h>
#ifndef MODVERSIONS
#define MODVERSIONS
#include <linux/modversions.h>
#endif
#endif

#include <linux/init.h>

#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/malloc.h>
#include <linux/vmalloc.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/timer.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/hdreg.h>
#include <linux/delay.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/ioctl.h>
#include <linux/genhd.h>
#include <linux/hdreg.h>
#include <linux/sockios.h>
#include <linux/smp_lock.h>

#include <asm/io.h>
#include <asm/system.h>
#include <asm/segment.h>
#include <asm/uaccess.h>
#include <asm/delay.h>
#include "compat22.h"

/* 2.1.X - not supported */
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2,1,0)
#error "Linux Kernel Version 2.1.0 not supported"

/* 2.0.X */
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2,0,0)

#warning "2.0.0 - Work in progress"

#ifdef MODULE
#include <linux/module.h>
#ifndef MODVERSIONS
#define MODVERSIONS
/* #    include <linux/modversions.h> */
#endif
#endif

#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/ioctl.h>
#include <linux/genhd.h>
#include <linux/malloc.h>       /* kmalloc() */
#include <linux/hdreg.h>        /* struct hd_geometry */
#include <linux/proc_fs.h>
#include <asm/system.h>
#include <asm/segment.h>
extern unsigned long loops_per_sec; /* needed by macro udelay() */

#include <asm/delay.h> /* macro udelay() */
#include <asm/io.h>		/* memcpy_*io() */
#ifndef __KERNEL_SYSCALLS__
/* need this to get declaration of kernel_thread() */
#define __KERNEL_SYSCALLS__
#endif
#include <linux/unistd.h> /* kernel_thread() */
#include <linux/sched.h> /* jiffies */

#include "compat20.h"

/* older than 2.0.X - not supported */
#else   /* we have no idea what kernel you have !!! */
#error "Linux Kernel Version less that 2.0.0 not supported"
#endif

/* local */
#include "fldrvlnx.h"
#include "flioctl.h"

/* definitions for blk.h 
 default major device number, define before including blk.h if it is not already in linux/major.h */

#include <linux/major.h>

#define MAJOR_NR FLASH_MAJOR

#define DEVICE_NAME "fl"

#define DEVICE_OFF(d)
#define DEVICE_REQUEST fl_request_fn1

#define DEVICE_NR(device) (MINOR(device)>>FL_SHIFT)
#define DEVICE_NO_RANDOM

// DDD debugging; let priority be a module parameter
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,0)
static int prio = -10; // max -20
#else
static int prio =  30; // max 40
#endif

#if LINUX_VERSION >= KERNEL_VERSION(2,2,0)
#ifdef MODULE
MODULE_PARM(prio, "i");
#endif
#endif

#include <linux/blk.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,3,0)
#include <linux/blkpg.h>
#endif

/* end of blk.h definitions */

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,0)
#include "compat24.c"
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2,2,0)
#include "compat22.c"
#else
#include "compat20.c"
#endif

/* private includes for M-Systems OSAK */
#include "tsi_doc.h"
#include "fllnx.h"

/* MAX_FL should be the same as the settings in MODULE_PARMS below.
 If more than four then FL_SHIFT must be adjusted to a lower value. */
#define MAX_FL		4

/* module parameters */

#ifdef MODULE

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,1,0)
MODULE_AUTHOR("yurid@msys.com");

MODULE_PARM(major, "i");        // set to zero to get dynamic major
MODULE_PARM_DESC(major, "Major device number to use.");

MODULE_PARM(rahead, "i");
MODULE_PARM_DESC(rahead, "Set readahead flag for driver.");

MODULE_PARM(hardsect, "1-4i");  // NB this should be the same as MAX_FL
MODULE_PARM_DESC(hardsect, "Set hardsect parameter for each DOC device.");

MODULE_PARM(blksize, "1-4i");   // NB this should be the same as MAX_FL
MODULE_PARM_DESC(blksize, "Set blksize parameter for each DOC device.");

MODULE_PARM(fl_debug,"h");
MODULE_PARM_DESC(fl_debug, "printk some debug messages.");

MODULE_PARM(fl_winl,"l");
MODULE_PARM_DESC(fl_winl, "Set the upper memory range in which driver will search.");

MODULE_PARM(fl_winh,"l");
MODULE_PARM_DESC(fl_winh, "Set the top memory range in which driver will search.");

MODULE_PARM(fl_part_prot,"h");
MODULE_PARM_DESC(fl_part_prot, "Set number of BDTL partition used with fl_key_prot.");

MODULE_PARM(fl_key_prot,"s");
MODULE_PARM_DESC(fl_key_prot, "Insert protection key to BDTL partition number fl_part_prot.");

MODULE_PARM(fl_nftl_cache,"h");
MODULE_PARM_DESC(fl_nftl_cache, "Sets corresponded environment variable.");

MODULE_PARM(fl_policy,"h");
MODULE_PARM_DESC(fl_policy, "Sets corresponded environment variable.");

MODULE_PARM(fl_is_ram_check,"h");
MODULE_PARM_DESC(fl_is_ram_check, "Sets corresponded environment variable.");

MODULE_PARM(fl_multidoc,"h");
MODULE_PARM_DESC(fl_multidoc, "Sets corresponded environment variable.");

MODULE_PARM(fl_8bit_access,"h");
MODULE_PARM_DESC(fl_8bit_access, "Sets corresponded environment variable.");

#else
#warning No parameters yet.
#endif

#endif

/* these are used only during module initialization */
#if !defined(__initdata)
#define __initdata
#endif

static int __initdata major             = MAJOR_NR;	/* MAJOR_NR == 100 */
static int __initdata rahead            = 0;
static int __initdata hardsect[MAX_FL];
static int __initdata blksize[MAX_FL];
static short __initdata	fl_debug        = 0;
static unsigned long __initdata	fl_winl = DOC_PHYS_ADDR;
static unsigned long __initdata	fl_winh = DOC_PHYS_ADDR;
static short __initdata	 fl_part_prot   = MAX_FL;
static char * __initdata fl_key_prot    = NULL;
static short  __initdata fl_nftl_cache  = 1;
static short  __initdata fl_policy      = 0;
static short  __initdata fl_is_ram_check = 0;
static short  __initdata fl_multidoc    = 0;
static short  __initdata fl_8bit_access = 0;

// some constants
#define FL_SHIFT        6
#define MINOR_NR(r) (r<<FL_SHIFT)

#define FL_DEBUG 1

// debugging macros
#if FL_DEBUG
#undef  DEBUG_FL
#define DEBUG_FL 0
#define DBGLVL KERN_DEBUG
//#define DBGLVL KERN_WARNING
#define PDEBUG(fmt, args...)            do { printk( DBGLVL fmt, ## args);} while(0)
#define TDEBUG(test, fmt, args...)      do { if(test) PDEBUG(fmt, ## args);} while(0)
#define PAUSE(usec)                     do { PDEBUG("+++\n"); schedule_timeout(usec);} while(0)
#else
#define TDEBUG(test, fmt, args...)
#define PDEBUG(fmt, args...)
#define PAUSE(usec)
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,0)
static DECLARE_MUTEX_LOCKED(start_stop_sem);
#else
static struct semaphore start_stop_sem = MUTEX_LOCKED;
#endif

#define SHUTDOWN_SIGS   (sigmask(SIGKILL)|sigmask(SIGINT)|sigmask(SIGTERM)|sigmask(SIGQUIT))

static void fl_setsid(void)
{
	/* Simple setsid, used at start of kernel thread,
	   no error checks needed, or at least none made :). */
	current->leader = 1;
	current->session = current->pgrp = current->pid;
	current->tty = NULL;
	current->tty_old_pgrp = 0;
}

static spinlock_t o_c_lock;

#ifdef EXTERNAL_MUTEX
struct semaphore oneMutex;
#endif

// private structure for driver
typedef struct Fl_Dev
{
	int             volume;     // DOC volume number
	int             size;       // DOC volume size in XXX kb
	int             usage;
	int             heads;
	int             sectors;
	int             cylinders;
	int             read_only;
	unsigned int    requests;
	unsigned int    requests_Read;
	unsigned int    requests_Write;

	// Every once in a while the DOC wants to "think" for a while.  We
	// can't sleep while this is happening if we are in the ordinary
	// request fn context.  What we do instead is use the reqest fn to
	// move the requests to a private, per device list, and schedule
	// a kernel task (capable of sleeping when required) to complete
	// the requests.
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,0)
	struct list_head queue_head;  // private list for I/O requests 
#else
	struct request *queue;  // Older kernels used singly linked list
	struct request *revq;   // reversed list, used to simplify transfer
#endif
	struct semaphore *sem;
	int terminating;
}FL_DEV;

// Start of structures dynamically allocated with kmalloc() during init and
// (if a module) de-allocated with kfree() if unloaded.

// per DEVICE structures
static FL_DEV  *Fl_devices;         // this is an array FL_Flash_Devices in length

// per DEVICE*MINOR structures
static int     *Fl_blksize_size;    // used in global blksize_size array
static int     *Fl_hardsect_size;   // used in global hardsect_size array
static int     *Fl_blk_size;        // used in gendisk structure and global blk_size array

static struct hd_struct *Fl_partitions;

// End of dynamically allocated structures.

static unsigned char flHandle[MAX_FL];
static unsigned char flProt[MAX_FL];

// function declarations

int             fl_open(struct inode *inode, struct file *filp);
int             fl_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg);
int             fl_check_change(kdev_t i_rdev);
int             fl_revalidate(kdev_t i_rdev);
int             fl_init(void);
static void     fl_geninit(struct gendisk *);

// copies flioctl_t data into kernel
/* static char *	fl_usr2kern(unsigned long); */

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,1,0)
int             fl_release(struct inode *inode, struct file *filp);
#else // > 2.1.0
void            fl_release(struct inode *inode, struct file *filp);
#endif // > 2.1.0

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,0)
void            fl_request_fn1(request_queue_t *q);
#else
void            fl_request_fn1(void);
#endif // < 2.4.0

// driver globals
int             Fl_major;           // our major device number
int             Fl_read_ahead;      // used in global read_ahead array 

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,0)

static struct block_device_operations fl_fops =
{
	open: fl_open,                   // open
	release: fl_release,             // release 
	ioctl: fl_ioctl,                 // ioctl 
};

static struct gendisk fl_gendisk =
{
	0,                         // major        Major number, assigned at runtime 
	DEVICE_NAME,               // major_name   Major name 
	FL_SHIFT,                  // minor_shift  Bits to shift to get real from partition 
	MINOR_NR(1),               // max_p        Number of partitions per real 
	NULL,                      // hd struct
	NULL,                      // block sizes
	0,                         // nr_real      maximum number of real, assigned at runtime 
	NULL,                      // internal
	NULL,                      // next         linked list (not used for modules) 
	&fl_fops,
	NULL,
	NULL
};

#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2,1,0)

static struct file_operations fl_fops =
{
	NULL,                      // lseek - default
	block_read,                // read - block dev read 
	block_write,               // write - block dev write 
	NULL,                      // readdir - not here! 
	NULL,                      // select 
	fl_ioctl,                  // ioctl 
	NULL,                      // mmap 
	fl_open,                   // open
	NULL,                      // flush 
	fl_release,                // release 
	block_fsync,               // fsync 
	NULL,                      // media change 
	fl_revalidate,             // revalidate 
	NULL
};

static struct gendisk fl_gendisk =
{
	0,                         // major        Major number, assigned at runtime 
	DEVICE_NAME,               // major_name   Major name 
	FL_SHIFT,                  // minor_shift  Bits to shift to get real from partition 
	MINOR_NR(1),               // max_p        Number of partitions per real 
	0,                         // max_nr       maximum number of real, assigned at runtime 
	fl_geninit,                // init         init function 
	NULL,                      // part         hd struct (partition table), assigned at runtime 
	NULL,                      // sizes        block sizes, assigned at runtime 
	0,                         // nr_real      number, assigned at runtime 
	NULL,                      // real_devices internal use
	NULL                       // next         linked list (not used for modules) 
};

#elif LINUX_VERSION_CODE >= KERNEL_VERSION(2,0,0)

static struct file_operations fl_fops =
{
	NULL,                      // lseek - default
	block_read,                // read - block dev read 
	block_write,               // write - block dev write 
	NULL,                      // readdir - not here! 
	NULL,                      // select 
	fl_ioctl,                  // ioctl 
	NULL,                      // mmap 
	fl_open,                   // open
	fl_release,                // release 
	block_fsync,               // fsync 
	NULL,                      // media change 
	fl_revalidate              // revalidate 
};

static struct gendisk fl_gendisk =
{
	0,                         // major        Major number, assigned at runtime 
	DEVICE_NAME,               // major_name   Major name 
	FL_SHIFT,                  // minor_shift  Bits to shift to get real from partition 
	MINOR_NR(1),               // max_p        Number of partitions per real 
	0,                         // max_nr       maximum number of real, assigned at runtime 
	fl_geninit,                // init         init function 
	NULL,                      // part         hd struct (partition table), assigned at runtime 
	NULL,                      // sizes        block sizes, assigned at runtime 
	0,                         // nr_real      number, assigned at runtime 
	NULL,                      // real_devices internal use
	NULL                       // next         linked list (not used for modules) 
};

#else
#error "OS Not supported"
#endif 

/* __init ckmalloc()
 *
 * kmalloc and clear, only used during init
 */
#if defined(__init)
void * __init ckmalloc(int size, int mode)
#elif defined(__initfunc)
__initfunc( void * ckmalloc(int size, int mode))
#else
void * ckmalloc(int size, int mode)
#endif
{
	void *ptr;
	if ((ptr = kmalloc(size, mode)))
		memset(ptr, 0, size);
	return(ptr);
}

/* IFF !NULL free and set to NULL */
void lkfree(void **ptr)
{
	if(*ptr)
	{
		kfree(*ptr);
		*ptr = NULL;
	}
}

int fl_open(struct inode *inode, struct file *filp)
{
	FL_DEV         *dev;        // device information 
	int             dev_nr;

	// get the real device number
	dev_nr = DEVICE_NR(inode->i_rdev);

	if (dev_nr >= fl_gendisk.nr_real)
	{
		PDEBUG(DEVICE_NAME"_open @1 dev_nr >= nr=%d -> ENODEV\n",fl_gendisk.nr_real);
		return -ENODEV;
	}

	if(flProt[dev_nr])
		if (filp->f_mode & 0x02)
			return -EROFS;

	// get pointer to device structure and increment use count
	dev = Fl_devices + dev_nr;

	spin_lock(&o_c_lock);
	dev->usage++;
#ifdef MODULE
	MOD_INC_USE_COUNT;
#endif
	spin_unlock(&o_c_lock);
	return 0;                   // success 
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,1,0)
int
#else
void
#endif
fl_release(struct inode *inode, struct file *filp)
{
	FL_DEV         *dev;        // device information 
	int             dev_nr;

	// get the real device number
	dev_nr = DEVICE_NR(inode->i_rdev);

	if (dev_nr >= fl_gendisk.nr_real)
	{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,1,0)
		return -ENODEV;
#else
		return;
#endif
	}

	dev = Fl_devices + dev_nr;

	// lock and dec usage count
	spin_lock(&o_c_lock);
	dev->usage--;
	// if there are no more open we want to ensure that we flush all blocks.
	// sync_dev can cause sleep, so this should be _before_ MOD_DEC_USE_COUNT.
	if (!dev->usage)
	{
		// flush it right now
		sync_dev(inode->i_rdev);
		invalidate_buffers(inode->i_rdev);
	}
#ifdef MODULE
	MOD_DEC_USE_COUNT;
#endif
	spin_unlock(&o_c_lock);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,1,0)
	return (0);
#endif
}

/* Derived from OSAK dosformt.c getDriveGeometry() */
static void get_drive_geo(long capacity, struct hd_geometry *geo)
{
	unsigned long temp;

	geo->heads = 16;
	geo->cylinders = 1024;
	temp = geo->cylinders * geo->heads;
	geo->sectors = capacity / temp;

	if (capacity % temp)
	{
		geo->sectors++;
		temp = geo->cylinders * geo->sectors;
		geo->heads = capacity / temp;
		if (capacity % temp)
		{
			geo->heads++;
			temp = geo->heads * geo->sectors;
			geo->cylinders = capacity / temp;
		}
	}
}

/* The ioctl() implementation */
int fl_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
	int             size;
	unsigned int    dev_nr;
	FL_DEV         *device;
	int             res;

	dev_nr = DEVICE_NR(inode->i_rdev);
	device = Fl_devices + dev_nr;

    // verify we have sysadmin capabilities for certain ioctl calls
    switch (cmd)
	{
	case FL_IOCTL_GET_INFO:
	case FL_IOCTL_DEFRAGMENT:
	case FL_IOCTL_WRITE_PROTECT:
	case FL_IOCTL_MOUNT_VOLUME:
	case FL_IOCTL_FORMAT_VOLUME:
	case FL_IOCTL_BDK_OPERATION:
	case FL_IOCTL_DELETE_SECTORS:
	case FL_IOCTL_READ_SECTORS:
	case FL_IOCTL_WRITE_SECTORS:
	case FL_IOCTL_FORMAT_PHYSICAL_DRIVE:
	case FL_IOCTL_FORMAT_LOGICAL_DRIVE:
	case FL_IOCTL_BDTL_HW_PROTECTION:
	case FL_IOCTL_BINARY_HW_PROTECTION:
	case FL_IOCTL_OTP:
	case FL_IOCTL_CUSTOMER_ID:
	case FL_IOCTL_UNIQUE_ID:
	case FL_IOCTL_NUMBER_OF_PARTITIONS:
	case FL_IOCTL_INQUIRE_CAPABILITIES:
	case FL_IOCTL_SET_ENVIRONMENT_VARIABLES:
	case FL_IOCTL_PLACE_EXB_BY_BUFFER:
	case FL_IOCTL_WRITE_IPL:
	case FL_IOCTL_DEEP_POWER_DOWN_MODE:
	case FL_IOCTL_LNX:
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,1,0)
        if (!capable(CAP_SYS_ADMIN))
	{
            PDEBUG(DEVICE_NAME"_ioctl -> EACCES\n");
            return -EACCES;
	}
#endif
	}
	// ioctl's that don't require libosak access
	switch (cmd)
	{
	case BLKGETSIZE:
		// Return the device size, expressed in sectors 
		if (!arg)
			return -EINVAL;     // NULL pointer: not valid 

		if ((res = verify_area(VERIFY_WRITE, (long *) arg, sizeof(long))))
			return res;

		size = fl_gendisk.part[MINOR(inode->i_rdev)].nr_sects;
		put_user(size, (long *) arg);

		TDEBUG(DEBUG_FL, DEVICE_NAME"_ioctl BLKGETSIZE: %x\n", size);
		return 0;

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,4,0)
	case BLKRAGET:             // return the readahead value 

		if (!arg)
			return -EINVAL;

		if ((res = verify_area(VERIFY_WRITE, (long *) arg, sizeof(long)))) {
			PDEBUG(DEVICE_NAME"_ioctl BLKRAGET: verify_area->%d\n",res);
			return res;
	}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,1,0)
	/* QQQ no need for verify area? ask SL */
		put_user_ret(read_ahead[MAJOR(inode->i_rdev)], (long *) arg, -EFAULT);
#else
		put_user(read_ahead[MAJOR(inode->i_rdev)], (long *) arg);
#endif
		TDEBUG(DEBUG_FL, DEVICE_NAME"_ioctl BLKRAGET: %x\n", read_ahead[MAJOR(inode->i_rdev)]);
		return 0;
#endif  < 2.4.0

	case HDIO_GETGEO:
		{
			struct hd_geometry *geo = (struct hd_geometry *) arg;
			struct hd_geometry geometry;

			// check arg
			if (geo == NULL)
			{
				PDEBUG(DEVICE_NAME"_ioctl HDIO_GETGEO: geo==NULL\n");
				return -EINVAL;
			}
			if ((res = verify_area(VERIFY_WRITE, geo, sizeof(*geo))))
			{
				PDEBUG(DEVICE_NAME"_ioctl HDIO_GETGEO: verify_area->%d\n",res);
				return res;
			}
			size = device->size;

			memset(&geometry,0,sizeof(geometry));
			get_drive_geo(size, &geometry);
			geometry.start = fl_gendisk.part[MINOR(inode->i_rdev)].start_sect;

			copy_to_user(geo, &geometry, sizeof(geometry));
			PDEBUG(DEVICE_NAME"_ioctl HDIO_GETGEO: b:%d h:%d s:%d c:%d\n",size, geometry.heads, geometry.sectors, geometry.cylinders);
			return 0;
		}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,0)
	// This is a list of "standard" ioctls.
	// The ones that modify things are protected by a semaphore below.
	case BLKROSET:
	case BLKROGET:
	// case BLKRASET:
	case BLKRAGET:
	// case BLKPG:
#ifdef EXTERNAL_MUTEX
		down(&oneMutex);
#endif
		res = blk_ioctl(inode->i_rdev, cmd, arg);
		PDEBUG(DEVICE_NAME"_ioctl (std): blk_ioctl->%d\n",res);
#ifdef EXTERNAL_MUTEX
		up(&oneMutex);
#endif
		return(res);
	case BLKPG:      // play with partitions
	case BLKFLSBUF:  // flush
		// This will call the low-level block I/O functions which lock
		// libosak access themselves, so we'd better not do it here.
	res = blk_ioctl(inode->i_rdev, cmd, arg);
		PDEBUG(DEVICE_NAME"_ioctl (std): blk_ioctl->%d\n",res);
		return(res);
#elif LINUX_VERSION_CODE < KERNEL_VERSION(2,4,0)

	case BLKFLSBUF:            // flush 
		fsync_dev(inode->i_rdev);
		invalidate_buffers(inode->i_rdev);
		break;

		// The default RO operations
		RO_IOCTLS(inode->i_rdev, arg);
#endif // >= 2.4.0

	case BLKRRPART:            // re-read partition table
		// This will call the low-level block I/O functions which lock
		// libosak access themselves, so we'd better not do it here.
		PDEBUG(DEVICE_NAME"_ioctl: fl_revalidate(%d)\n",inode->i_rdev);
		res = fl_revalidate(inode->i_rdev);
		return(res);
	}

	// ioctl's that require libosak access (we are modifying something)
	res = 0;
#ifdef EXTERNAL_MUTEX
		down(&oneMutex);
#endif
	switch(cmd)
	{
	// private stuff
	case FL_IOCTL_GET_INFO:
	case FL_IOCTL_DEFRAGMENT:
	case FL_IOCTL_WRITE_PROTECT:
	case FL_IOCTL_MOUNT_VOLUME:
	case FL_IOCTL_FORMAT_VOLUME:
	case FL_IOCTL_BDK_OPERATION:
	case FL_IOCTL_DELETE_SECTORS:
	case FL_IOCTL_READ_SECTORS:
	case FL_IOCTL_WRITE_SECTORS:
	case FL_IOCTL_FORMAT_PHYSICAL_DRIVE:
	case FL_IOCTL_FORMAT_LOGICAL_DRIVE:
	case FL_IOCTL_BDTL_HW_PROTECTION:
	case FL_IOCTL_BINARY_HW_PROTECTION:
	case FL_IOCTL_OTP:
	case FL_IOCTL_CUSTOMER_ID:
	case FL_IOCTL_UNIQUE_ID:
	case FL_IOCTL_NUMBER_OF_PARTITIONS:
	case FL_IOCTL_INQUIRE_CAPABILITIES:
	case FL_IOCTL_SET_ENVIRONMENT_VARIABLES:
	case FL_IOCTL_PLACE_EXB_BY_BUFFER:
	case FL_IOCTL_WRITE_IPL:
	case FL_IOCTL_DEEP_POWER_DOWN_MODE:
	case FL_IOCTL_LNX:
		fl_doc_ioctl(flHandle[DEVICE_NR(inode->i_rdev)], cmd, arg);
		break;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,0)

	case BLKRASET:             // set the readahead value 
		PDEBUG(DEVICE_NAME"_ioctl blk_ioctl(%d,%d,%lx)\n",inode->i_rdev, cmd, arg);
		res = blk_ioctl(inode->i_rdev, cmd, arg);
		PDEBUG(DEVICE_NAME"_ioctl (std): blk_ioctl->%d\n",res);
	break;
#else //>= 2.4.0

	case BLKRASET:             // set the readahead value 
		if (arg > 0xff)
		{
			res = -EINVAL;      // limit it 
			break;
		}
		read_ahead[MAJOR(inode->i_rdev)] = arg;
		break;
#endif >= 2.4.0
	default: // unknown command
		res = -EINVAL; /* ? ENOSYS ? */
	}
#ifdef EXTERNAL_MUTEX
		up(&oneMutex);
#endif
	return res;
}

/* Support for removable devices */
int fl_check_change(kdev_t i_rdev)
{
	// TODO
	return 0;
}

int fl_revalidate(kdev_t i_rdev)
{
	// first partition, # of partitions 
	int             part1 = MINOR_NR(DEVICE_NR(i_rdev)) + 1;
	int             npart = MINOR_NR(1) - 1;
	int             dev_nr;

	// first clear old partition information 
	memset(fl_gendisk.sizes + part1, 0, npart * sizeof(int));
	memset(fl_gendisk.part + part1, 0, npart * sizeof(struct hd_struct));

	// then fill new info 
	dev_nr = DEVICE_NR(i_rdev);
	// The partition code eventually wants to read
	// a block and wait for it.  We'd better not lock out the
	// low level routine fl_do1_request() or we're hosed.
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,0)
	grok_partitions(&fl_gendisk, dev_nr, fl_gendisk.max_p,Fl_devices[dev_nr].size);
				   /* device->size); */
#else
	resetup_one_dev(&fl_gendisk, dev_nr);
#endif
	return 0;
}

/* Block-driver specific functions */
static int fl_do1_request(FL_DEV *device, struct request *req)
{
	/* Do the I/O for _one_ request, but don't mess with the request queues. */
	/* All validity checking done before calling this fn. */

	unsigned int    blocks;
	unsigned int    block;
	void           *buffer;
	int             minor;
	int             dev_nr;
	int fl_last_error=0;

	/* Make sure we don't have ioctl's or anything playing with this physical device at the same time we are. */
#ifdef EXTERNAL_MUTEX
		down(&oneMutex);
#endif

	// get device information
	dev_nr = DEVICE_NR(req->rq_dev);
	minor = MINOR(req->rq_dev);

	// get io information
	blocks = req->current_nr_sectors;
	buffer = req->buffer;
	block = req->sector;

	// translate through partition table
	block += Fl_partitions[minor].start_sect;

	// iterate doing io until we are done
	for (; blocks--; block++, buffer += 512)
	{
		// determine if we are READing or WRITEing, then select io type
		switch (req->cmd)
		{
		case READ:
			device->requests_Read++;
			fl_last_error=fl_doc_read(flHandle[dev_nr], buffer, block, 1);
			if(fl_last_error)
				flprintk(0,"ERROR READING: fl_doc_read returns %d\n",(int)fl_last_error);
			break;

		case WRITE:
			device->requests_Write++;
			fl_last_error=fl_doc_write(flHandle[dev_nr], buffer, block, 1);
			if(fl_last_error)
				flprintk(0,"ERROR WRITING: fl_doc_write returns %d\n",(int)fl_last_error);
			break;
		}
	}
#ifdef EXTERNAL_MUTEX
		up(&oneMutex);
#endif
	if(fl_last_error)
		return 0;
	else
		return 1;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,0)
void            fl_request_fn1(request_queue_t *q)
#else
void            fl_request_fn1(void)
#endif // < 2.4.0
{
	// Move all the requests into the private device queue.
	struct request *req;
	FL_DEV         *device;
	int             minor;
	int             dev_nr;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,0)
	int             code;
#endif

	// Standard block device request loop
	//
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,0)
	while (code = 0, !QUEUE_EMPTY)
#else
	while (1)
#endif
	{
		// Standard block device boiler plate
		INIT_REQUEST;
		req = CURRENT;

		// get device information
		dev_nr = CURRENT_DEV;
		minor = MINOR(req->rq_dev);

		// Check if the minor number is in range 
		if ((dev_nr > fl_gendisk.nr_real) || (minor > MINOR_NR(fl_gendisk.nr_real)))
		{
			static int      count = 0;
			if (count++ < 5)    // print the message at most five times 
				printk(KERN_WARNING DEVICE_NAME"_request_fn: request for non-existent device\n");
			end_request(0);
			continue;
		}
		// get device structure
		device = Fl_devices + dev_nr;
		device->requests++;
		// check we are in range
		if ((req->sector + req->current_nr_sectors) > fl_gendisk.part[minor].nr_sects)
		{
			printk(KERN_WARNING DEVICE_NAME"_request_fn: request past end of device: %4lx >= %4lx\n",req->sector + req->current_nr_sectors, fl_gendisk.part[minor].nr_sects);
			if (req->sector != 0)
			{
				end_request(0);
				continue;
			}
		}
		// check we have a valid command
		if (req->cmd != READ && req->cmd != WRITE)
		{
			end_request(0);
			continue;
		}

		// Check that there is somebody to handle it :).
		if (NULL == device->sem)
		{
			// Oops, there isn't.
			end_request(0);
			continue;
		}

		if(req->cmd == WRITE && flProt[DEVICE_NR(req->rq_dev)]==1)
		{
			end_request(0);
			continue;
		}

		// Move to private list (io_request_lock aquired before calling this request fn).
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,0)
		list_del(&req->queue);
		INIT_LIST_HEAD(&req->queue);
		list_add_tail(&req->queue,&device->queue_head);
#else
		// no easy way to add to tail for singly linked list,
		// so build list in reverse order and flip it when processing starts
		CURRENT = req->next;
		req->next = device->revq;
		device->revq = req;
#endif
		// Make sure the next stage is set up to run.  We can
		// do this more than once because it's a semaphore,
		// and we _must_ do it now because some kernel versions
		// have a function exit inside the INIT_REQUEST macro
		// used above.
		up(device->sem);
	}
}

static struct request*gimme_req(FL_DEV *device)
{
	struct request *req;
	unsigned long flags;

	// Use the same lock that's protecting fl_request_fn1()
	grab_io_request_lock(flags);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,0)
	req = list_empty(&device->queue_head) ? NULL :list_entry(device->queue_head.next,struct request,queue);
#else
	if (NULL == (req = device->queue) && NULL != (req = device->revq))
	{
		// There are none on the private queue, but the reversed
		// queue has at least one, flip the reversed queue into
		// the private queue.
		do{
			device->revq = req->next;
			req->next = device->queue;
			device->queue = req;
		}while (NULL != (req = device->revq));
		// Pick up the first request
		req = device->queue;
	}
#endif
	/* A tiny bit of paranoia from INIT_REQ */
	if (NULL != req && NULL != req->bh)
	{
		if (!buffer_locked(req->bh))
			panic(DEVICE_NAME ": block not locked");
	}
	/* End paranoia */
	rlse_io_request_lock(flags);
	return(req);
}

static void request_completed(FL_DEV *device, struct request *req,int result)
{
	// Do the equivalent of end_request(1), but don't use "CURRENT"
	unsigned long flags;
	/* We really don't need to lock just yet, because the request is in
	   our private queue and we won't be returning it to the public free
	   list until _after_ end_that_request_first() returns 0. */
	if (end_that_request_first(req,result,DEVICE_NAME))
	{
		// This request is multi-sector and is not finished yet,
		// the various buffer pointers and counters have been adjusted,
		// but we need another crack at the remaining sectors.
		return;
	}
#if !defined(DEVICE_NO_RANDOM)
	add_blkdev_randomness(MAJOR(req->rq_dev));
#endif
	grab_io_request_lock(flags);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,0)
	blkdev_dequeue_request(req);
	INIT_LIST_HEAD(&req->queue);
#else
	device->queue = req->next;
	req->next = NULL;
#endif
	end_that_request_last(req);
	rlse_io_request_lock(flags);
}

/* Note: this runs as a kernel thread without aquired locks, so any list operations must aquire the appropriate lock. (See gimme_req() and request_completed(), above.) */
int fl_request_fn2(void *data)
{
	FL_DEV         *device = (FL_DEV *) data;
	struct request *req;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,0)
	DECLARE_MUTEX_LOCKED(reqs2do);
#else
	struct semaphore reqs2do = MUTEX_LOCKED;
#endif
	struct semaphore *initterm = device->sem;

	/* Detach as best we can from the process that started us. */
	lock_kernel();
	exit_mm(current);
	exit_files(current);
	exit_fs(current);
	fl_setsid();
	current->exit_signal = SIGCHLD;
	/* QQQ should terminate when we detect 'em.....if we detect 'em */
	siginitsetinv(&current->blocked, SHUTDOWN_SIGS);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,0)
	spin_lock(&current->sigmask_lock);
	flush_signals(current);
	spin_unlock(&current->sigmask_lock);
#else
	sigemptyset(&current->signal);
#endif
	/* Name this thread */
	sprintf(current->comm, "msys/fl%c", 'a'+(device-Fl_devices));
	/* Run at a high priority, ahead of sync and friends */
	/* QQQ priority > bdflush??? */
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,0)
	current->nice = prio;
#else
	current->priority = prio;
#endif
	current->policy = SCHED_OTHER;
	/* Replace the initialization/termination semaphore with our
	   "requests to do" semaphore. */
	device->sem = &reqs2do;
	unlock_kernel();

	// Let the process that started us know we are ready to roll.
	up(initterm);
	// Process all requests for this device.
	while (!device->terminating)
	{
		/* Wait for some requests. */
		down(device->sem);
		/* Do 'em. */
		while (NULL != (req = gimme_req(device)))
		{
			request_completed(device,req,fl_do1_request(device,req));
		}
	}
	// Let the process terminating us know we are done. */
	up(initterm);
	return(0);
}

static void kickoff_thread(FL_DEV *device, int (*th_fn)(void *data))
{
	device->terminating = 0;
	device->sem = &start_stop_sem;
	kernel_thread(th_fn,(void *)device,0);
	/* Wait for the function to finish initialization. */
	down(&start_stop_sem);
}

static void killoff_thread(FL_DEV *device)
{
	if (NULL == device->sem || device->terminating)
	{
		/* It's already dead or dying. */
		return;
	}
	/* Tell the thread to quit. */
	device->terminating = 1;
	/* Wake up the function for the last time (if it isn't already awake). */
	up(device->sem);
	/* Wait for the function to finish termination. */
	down(&start_stop_sem);
	// Make sure no one tries to wake up the function again.
	device->sem = NULL;
}

#ifdef MODULE
#define fl_init init_module
#endif

/* *************************************************************************************************** 
 * Finally, the module stuff
 */

static void fl_geninit(struct gendisk *gendisk)
{
	int             mdev;
	FL_DEV         *device;
	int	bDevice;

	// iterate across the per minor device arrays and prepare them
	for (mdev = 0; mdev < MINOR_NR(gendisk->nr_real); mdev++)
	{
		// prepare the Fl_blksize_size array and zero it 
		Fl_blksize_size[mdev] = blksize[mdev>>FL_SHIFT];
		// prepare the Fl_hardsect_size array and zero it 
		Fl_hardsect_size[mdev] = hardsect[mdev>>FL_SHIFT];
		Fl_blk_size[mdev] = 0;
	}

	// iterate across the flash devices filling in the device information structure
	for(bDevice=0;bDevice<gendisk->nr_real;bDevice++)
	{
		// we still increment dev
		// so that we can access the underlying device via ioctls for formatting etc
		device = Fl_devices + bDevice;
#ifdef EXTERNAL_MUTEX
	#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,0)
		init_MUTEX(&oneMutex);
	#else
		oneMutex = MUTEX;
	#endif
#endif
		// We have a private queue per physical device, but only one "public" queue for the major number
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,0)
		INIT_LIST_HEAD(&device->queue_head);
#else
		device->queue = device->revq = NULL;
#endif
		kickoff_thread(device,fl_request_fn2);

		//remove read protection
		if(flHandle[bDevice]==fl_part_prot)
		{
			if(!flRemoveProtectionFromParm(fl_part_prot,fl_key_prot))
				flProt[bDevice]=0;
			else
				flprintk(0,"device %d: Error removing protection.\n",(int)bDevice);
		}
		// does this DOC volume exist?
		if( !fl_doc_mount(flHandle[bDevice]) )
		{
			// start to fill in the required info
			device->volume = bDevice;
			device->size = fl_doc_sectors(flHandle[bDevice]);
			
			Fl_blk_size[MINOR_NR(bDevice)] = device->size;
			Fl_blksize_size[MINOR_NR(bDevice)] = 1024;
			Fl_partitions[MINOR_NR(bDevice)].start_sect = 0;
			Fl_partitions[MINOR_NR(bDevice)].nr_sects = 0;  
		}
		else
		{
			// start to fill in the required info
			device->volume = -1;
			device->size = 0;

			Fl_blk_size[MINOR_NR(bDevice)] = 0;
			Fl_blksize_size[MINOR_NR(bDevice)] = 0;
			Fl_partitions[MINOR_NR(bDevice)].start_sect = 0;
			Fl_partitions[MINOR_NR(bDevice)].nr_sects = 0;  
		}
	}

#ifndef MODULE
	printk(KERN_INFO DEVICE_NAME"_geninit: registered device at major: %d\n", Fl_major);
#endif

	// fill in partition info for whole disks so we can do resetup_one_dev()
	for (bDevice=0;bDevice<gendisk->nr_real;bDevice++)
	{
		device = Fl_devices+bDevice;
		Fl_partitions[MINOR_NR(bDevice)].nr_sects = device->size; 
	}

	// fill in the gendisk structure 
	gendisk->sizes = Fl_blk_size;

	// set global variables
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,0)
	blk_init_queue(BLK_DEFAULT_QUEUE(Fl_major),fl_request_fn1);
#elif LINUX_VERSION_CODE < KERNEL_VERSION(2,4,0)
	blk_dev[Fl_major].request_fn = fl_request_fn1;
#endif // >= 2.4.0
	read_ahead[Fl_major] = Fl_read_ahead;
	blk_size[Fl_major] = Fl_blk_size;
	blksize_size[Fl_major] = Fl_blksize_size;
	hardsect_size[Fl_major] = Fl_hardsect_size;

#if defined(MODULE) || LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,0)
	// call resetup/register_disk for each physical device
	// 2.4.0 - this now needs to be done for both module and kernel versions of the driver.
	for(bDevice=0;bDevice<gendisk->nr_real;bDevice++)
	{
		device = Fl_devices+bDevice;
		if (device->size)
		{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,0)
			register_disk(gendisk,MKDEV(Fl_major,(bDevice<<gendisk->minor_shift)),gendisk->max_p,gendisk->fops,device->size);
#else
			resetup_one_dev(gendisk,bDevice);
#endif
		}
	}
#endif

	// grab blk_size information and spit out some info
	for (mdev = 0; mdev < MINOR_NR(gendisk->nr_real); mdev++)
	{
		Fl_blk_size[mdev] = Fl_partitions[mdev].nr_sects / 2;

		if ((mdev%64 == 0) || (mdev%64 == 1)  || (mdev%64 == 2) || (mdev%64 == 3))
		{
			printk(KERN_INFO "partition: %d: start_sect: %lx, nr_sects: %lx Fl_blk_size[]: %xkb\n",mdev, Fl_partitions[mdev].start_sect, Fl_partitions[mdev].nr_sects, Fl_blk_size[mdev]);
		}
	}

	return;
}

/* DDD Here for debugging
 * print out the size of the request queue when /proc/fla_queue is read. */
static int fl_read_proc(char *buffer, char **start, off_t offset,int length, int *eof, void *data)
{
	int len;
	off_t begin=0;

	len = sprintf(buffer, "ReadQ WriteQ\n%5d %6d\n",Fl_devices->requests_Read,Fl_devices->requests_Write);

	if (len<offset)
	{
		begin = len;
		len=0;
		*eof = 1;
	}
	*start = buffer+(offset-begin);
	len-=(offset-begin);
	if (len < 0)
		len = 0;
	if (len > length)
		len = length;
	return len;
}

/* Initialize the driver, register major and link into gendisk_list. */
#if defined(__init)
int __init fl_init(void)
#elif defined(__initfunc)
__initfunc( int fl_init(void))
#else
int fl_init(void)
#endif
{
	int             result = 0;
	int             i;
	unsigned char	bDevice,bSocket;

	if( tsi_request_doc_iomem() )
	{
		fl_winl = (long)tsi_get_doc_vaddr();
		fl_winh = (long)tsi_get_doc_vaddr();
	}

	// Copy the (static) cfg variables to public prefixed ones to allow snoozing with a debugger.
	// major device number
	Fl_major = major;		/* 100 by default */
	Fl_read_ahead = rahead;	/* 0 by default */

	// set default blksize and hardsect if they where not passed as module parameters
	for (i = 0; i < MAX_FL; i++)
	{
		if (!blksize[i])
			blksize[i] = 1024;
		if (!hardsect[i])
			hardsect[i] = 512;
	}

	spin_lock_init(&o_c_lock);

	// can we initialize OSAK?
	if (fl_doc_init())
	{
		printk(KERN_ERR"Cannot initialize M-Systems DOC 2000 OSAK\n");
		tsi_release_doc_iomem();
		return(1);
	}

	flprintk(1,"fl_init: fl_doc_init OK\n");

	// find out how many devices we have
	if(!(fl_gendisk.nr_real = fl_doc_count(MAX_FL)))
	{
		printk(KERN_ERR"Cannot find any M-Systems DOC 2000 devices\n");
		tsi_release_doc_iomem();
		return(1);
	}
	flprintk(1,"fl_init: Found %d devices\n",fl_gendisk.nr_real);

	if(fl_multidoc==1)
	{
		fl_gendisk.nr_real=1;
		flprintk(1,"fl_init: multidoc: Number of devices redused to 1.\n");
	}
	
	for(bSocket=0,bDevice=0;bDevice<fl_gendisk.nr_real;bSocket++)
	{
		unsigned char bNDevicesOnSocket=fl_partition_count(bSocket),bDeviceOnSocket;

		if(bNDevicesOnSocket==0xff)
		{
			flprintk(0,"Socket %d : Can't count number of devices.\n",bSocket);
			return 1;
		}
		flprintk(1,"Socket %d : %d devices\n",bSocket,bNDevicesOnSocket);
		for(bDeviceOnSocket=0;bDeviceOnSocket<bNDevicesOnSocket;bDeviceOnSocket++,bDevice++)
		{
			flHandle[bDevice]=(bSocket|(bDeviceOnSocket<<4));
			flProt[bDevice]=fl_protType(flHandle[bDevice]);
			flprintk(1,"Device %d on socket %d : 0x%x\n",bDevice,bSocket,flHandle[bDevice]);
		}
	}

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,4,0)
	fl_gendisk.max_nr = fl_gendisk.nr_real;  // for the sake of parnoia...
#endif
	// partition array must be allocated here, setup_dev() accesses it before calling fl_genit()
	if (!(Fl_partitions = ckmalloc(MINOR_NR(fl_gendisk.nr_real) * sizeof(struct hd_struct), GFP_KERNEL)))
	{
		tsi_release_doc_iomem();
		return 1;
	}

	// allocate per device dynamic structures, these are done here so we can release ckmalloc()
	if (!(Fl_devices = ckmalloc(fl_gendisk.nr_real * sizeof(FL_DEV), GFP_KERNEL)))
		goto fail_malloc;

	// allocate per device * minor blk dynamic arrays
	if (!(Fl_blksize_size = ckmalloc(MINOR_NR(fl_gendisk.nr_real) * sizeof(unsigned int), GFP_KERNEL)))
		goto fail_malloc;

	if (!(Fl_hardsect_size = ckmalloc(MINOR_NR(fl_gendisk.nr_real) * sizeof(unsigned int), GFP_KERNEL)))
		goto fail_malloc;

	if (!(Fl_blk_size = ckmalloc(MINOR_NR(fl_gendisk.nr_real) * sizeof(unsigned int), GFP_KERNEL)))
		goto fail_malloc;

	// setup gendisk structure with partition information
	fl_gendisk.part = Fl_partitions;

	// ok, we have at least one DOC, proceed
	printk(KERN_INFO"Flash disk driver for DiskOnChip\n");
	printk(KERN_INFO"Copyright (C) 1998,2002 M-Systems Flash Disk Pioneers Ltd.\n");
	printk(KERN_INFO"DOC device(s) found: %d\n", fl_gendisk.nr_real);
#ifdef FL_FAT_FILTER	
	printk(KERN_INFO"Fat Filter Enabled\n");
#endif	

	// Register your major, and/or accept a dynamic number
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,0)
	if ((result = devfs_register_blkdev(Fl_major,DEVICE_NAME,&fl_fops)) < 0)
#else
	if ((result = register_blkdev(Fl_major, DEVICE_NAME, &fl_fops)) < 0)
#endif
	{
		printk(KERN_WARNING DEVICE_NAME": can't get major %d\n",Fl_major);
		tsi_release_doc_iomem();
		return result;
	}

	// save major if we are using a dynamic major
	if (Fl_major == 0)
		Fl_major = result;      // dynamic

	fl_gendisk.major = Fl_major;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,0)
	devfs_handle = devfs_mk_dir (NULL, fl_gendisk.major_name, NULL);
	// DDD this is in here for debugging
	create_proc_read_entry("fl_queue", 0, 0, fl_read_proc, NULL);
#endif // >= 2.4.0

#ifdef MODULE
	printk(KERN_INFO DEVICE_NAME"_init: registered device at major: %d\n",Fl_major);
#endif                          /* MODULE */

	// link into global gendisk list
	fl_gendisk.next = gendisk_head;
	gendisk_head = &fl_gendisk;

#if defined(MODULE) || LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,0)
	fl_geninit(&fl_gendisk);
#endif

	return 0;

fail_malloc:

	// Reset global block array entries
	// QQQ - the first part of this cleanup does not make real sense,
	//       we have not done the setup until fl_geninit(), and we
	//       will not be getting to here once we have called it.
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,0)
	blk_cleanup_queue(BLK_DEFAULT_QUEUE(Fl_major));
	devfs_unregister_blkdev(Fl_major,DEVICE_NAME);
#else
	blk_dev[Fl_major].request_fn = NULL;
#endif // >= 2.4.0
	read_ahead[Fl_major] = 0;
	blk_size[Fl_major] = NULL;
	blksize_size[Fl_major] = NULL;
	hardsect_size[Fl_major] = NULL;

	// Failed, free any memory that was allocated and set pointers to NULL
	lkfree((void **)&Fl_devices);
	lkfree((void **)&Fl_blksize_size);
	lkfree((void **)&Fl_hardsect_size);
	lkfree((void **)&Fl_blk_size);
	lkfree((void **)&Fl_partitions);

	tsi_release_doc_iomem();

	return 0;
}

static unsigned long counterWrite=0;
static unsigned long counterRead=0;
static unsigned long counterErase=0;
void AddToReadCounter(unsigned long add)
{
	counterRead+=add;
}
void AddToWriteCounter(unsigned long add)
{
	counterWrite+=add;
}
void AddToEraseCounter(unsigned long add)
{
	counterErase+=add;
}


#ifdef MODULE
void cleanup_module(void)
{
	int             i;
	struct gendisk ** gd;

	// first of all, flush it all and reset all the data structures 
	for (i = 0; i < MINOR_NR(fl_gendisk.nr_real); i++)
	{
		fsync_dev(MKDEV(Fl_major, i));  // flush the devices 
	}

	// Terminate the request threads.
	for (i = 0; i < fl_gendisk.nr_real; i++)
	{
		 killoff_thread(Fl_devices+i);
	}

	// remove our gendisk structure from the global linked list 
	for (gd = &gendisk_head; *gd; gd = &((*gd)->next))
	{
		if (*gd == &fl_gendisk)
		{
			*gd = (*gd)->next;
			break;
		}
	}

	flprintk(0,"cleanup_module: counterWrite 0x%lx, counterRead 0x%lx, counterErase 0x%lx\n",counterWrite,counterRead,counterErase);

	// complain if we didn't find ourselves in gendisk list
	if (!*gd)
		printk( KERN_ERR DEVICE_NAME"_cleanup_module: entry in gendisk chain missing!\n" );


	// reset global blk array entries
	read_ahead[Fl_major] = 0;
	blk_size[Fl_major] = NULL;
	blksize_size[Fl_major] = NULL;
	hardsect_size[Fl_major] = NULL;

	// unregister our device
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,0)
	devfs_unregister_blkdev(Fl_major, DEVICE_NAME);
	blk_cleanup_queue(BLK_DEFAULT_QUEUE(Fl_major));
	devfs_unregister(devfs_handle);
	devfs_handle = NULL;
	// DDD again, this is for debugging
	remove_proc_entry("fl_queue", 0);

#else
	blk_dev[Fl_major].request_fn = NULL;
	unregister_blkdev(Fl_major, DEVICE_NAME);
#endif // >= 2.4.0

	// free our data structures
	lkfree((void **)&Fl_blk_size);
	lkfree((void **)&Fl_blksize_size);
	lkfree((void **)&Fl_hardsect_size);
	lkfree((void **)&Fl_devices);
	lkfree((void **)&Fl_partitions);

	tsi_release_doc_iomem();

}
#endif                          /* MODULE */

/* The following are the Linux versions of various things required by the M-Systems OSAK that must be compiled here in the Linux kernel environment.
 These are prototyped in flsysfun.h which is NOT included here as it drags in too much other proprietary data. */
void flDelayMsecs(unsigned milliseconds)
{
	register int    i;
	for (i = 0; i < milliseconds; i++)
		udelay(1000L);
}

void flsleep(unsigned long milliseconds)
{
	unsigned long deltaj;
	/* Timeout has to be expressed in jiffies, there are HZ jiffies/sec. 
	   We add one to it anyway, because the "first" jiffy is really some
	   random fraction of a jiffy. */
	deltaj = 1 + ((HZ * milliseconds) / 1000);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,1,0)
	/* QQQ: why TASK_UNINTERRUPTIBLE? */
	current->state = TASK_UNINTERRUPTIBLE;
	while ((deltaj = schedule_timeout(deltaj)) > 0) /* skip */ ;
#else // kv < 2.1.0
	current->timeout = jiffies + deltaj;
	current->state = TASK_INTERRUPTIBLE; /* uninterruptible => no timeout ! */
	schedule();
	current->timeout = 0;
#endif // < 2.1.0
}

void*flmemcpy(void *dest, const void *src, size_t length)
{
	//printk("df: dest: 0x%08X, src: 0x%08X\n", (unsigned)dest, (unsigned)src);
	return memcpy(dest, src, length);
}

void*flmemset(void *dest, int value, unsigned int length)
{
	return memset(dest, value, length);
}

/*
void *flmemcpy_fromio(void *dest, const void * src, unsigned int length)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,0)
	return(isa_memcpy_fromio(dest, (unsigned long)src, length));
#else
	return(memcpy_fromio(dest, src, length));
#endif
}
*/
void *flmemcpy_fromio(void *dest, const void * src, unsigned int length)
{
	printk("df: flmemcpy_fromio, \n");
// dfoley (changed) was >=
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,4,0)
	return(isa_memcpy_fromio(dest, (unsigned long)src, length));
#else
	//return(memcpy_fromio(dest, src, length));
	//dfoley (changed)
	memcpy_fromio(dest, src, length);
	return dest;
#endif
}

/*
void *flmemcpy_toio(void *dest, const void * src, unsigned int length)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,0)
	return(isa_memcpy_toio((unsigned long)dest, src, length));
#else
	return(memcpy_toio(dest, src, length));
#endif
}
*/
void *flmemcpy_toio(void *dest, const void * src, unsigned int length)
{
	printk("df: flmemcpy_toio, \n");
// dfoley (changed) was >=
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,4,0)
	return(isa_memcpy_toio((unsigned long)dest, src, length));
#else
	//dfoley (changed)
	memcpy_toio(dest, src, length);
	return dest;
#endif
}
/*
void*flmemset_io(void *dest, int value, unsigned int length)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,0)
	isa_memset_io((unsigned long)dest, value, length);
#else
	memset_io(dest, value, length);
#endif
	return(0);
}
*/
void*flmemset_io(void *dest, int value, unsigned int length)
{
	printk("df: flmemset_io, \n");
// dfoley (changed) was >=
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,4,0)
	isa_memset_io((unsigned long)dest, value, length);
#else
	memset_io(dest, value, length);
#endif
	return(0);
}


int flmemcmp(const void * dest,const void  *src,size_t count)
{
	int res;
	res = memcmp(dest, src, count);
	return(res);
}

void*flvmalloc(unsigned long size)
{
	void*p=vmalloc(size);
	flprintk(1,"flvmalloc: %x length %x\n",p,size);
	return p;
}

void flvfree(void * addr)
{
	flprintk(1,"flvfree: %x\n",addr);
	vfree(addr);
}


int fix_flprintk(char *buf, int i)
{
	buf[i--] = '\0';

	// libosak uses \n\r, this make kerneld unhappy
	if (buf[i-1] == '\n' && buf[i] == '\r')
		buf[i--] = '\0';
	return i;
}

int do_printk(const char *fmt, ...)
{
	va_list args;
	char buf[1024];     /* hopefully enough, kernel/printk.c thinks so */
	int i;

	va_start(args, fmt);
	i = vsprintf(buf, fmt, args); 
	va_end(args);
	
	i = fix_flprintk(buf,i);

	printk(buf);

	return i;
}

int flprintk(unsigned char fDebug,const char *fmt, ...)
{
	if( !fDebug || fl_debug)
	{
		va_list args;
		char buf[1024];     /* hopefully enough, kernel/printk.c thinks so */
		int i;

		va_start(args, fmt);
		vsprintf(buf, fmt, args); 
		va_end(args);

		return do_printk(buf);
	}
	return 0;
}

unsigned char flreadb(volatile void *addr)
{
	unsigned char c;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,0)
	c = isa_readb((unsigned long)addr);
#else
	c = readb(addr);
#endif
	return(c);
}

unsigned short flreadw(volatile void *addr)
{
	unsigned short c;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,0)
	c = isa_readw((unsigned long)addr);
#else
	c = readw(addr);
#endif
	return(c);
}

void flwriteb(unsigned value, volatile void *addr)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,0)
	isa_writeb(value, (unsigned long)addr);
#else
	writeb(value, addr);
#endif
}

void flwritew(unsigned value, volatile void *addr)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,4,0)
	isa_writew(value, (unsigned long)addr);
#else
	writew(value, addr);
#endif
}
/*
#include <asm/msr.h>
unsigned flRandByte(void)
{
	long time;
	rdtscl(time);
	return (time >> 1) & 0xff;
}
*/
#include <linux/time.h>
unsigned flRandByte(void)
{
	struct timeval time;
	printk("df: flRandByte\n");	
	do_gettimeofday(&time);
	return (time.tv_usec >> 1) & 0xff;
}

unsigned long flGetWinL(void)
{
	return fl_winl;
}

unsigned long flGetWinH(void)
{
	return fl_winh;
}

void flGetEnvVarFromParam(unsigned char*flUseNFTLCachePar,unsigned int*flPolicyPar,
	unsigned char*flUseisRAMPar,unsigned int*flUseMultiDocPar,unsigned char*flUse8BitPar)
{
	*flUseNFTLCachePar=fl_nftl_cache;
	*flPolicyPar=fl_policy;
	*flUseisRAMPar=fl_is_ram_check;
	*flUseMultiDocPar=fl_multidoc;
	*flUse8BitPar=fl_8bit_access;
	flprintk(1,"nftl_cache=%d policy=%d is_ram_check=%d multidoc=%d 8bit_access=%d\n",fl_nftl_cache,fl_policy,fl_is_ram_check,fl_multidoc,fl_8bit_access);
}

