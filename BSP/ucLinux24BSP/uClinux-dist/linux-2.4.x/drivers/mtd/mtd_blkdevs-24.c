/*
 * $Id: mtd_blkdevs-24.c,v 1.1.1.1 2006-07-11 09:29:04 andy Exp $
 *
 * (C) 2003 David Woodhouse <dwmw2@infradead.org>
 *
 * Interface to Linux 2.4 block layer for MTD 'translation layers'.
 *
 */

#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/list.h>
#include <linux/fs.h>
#include <linux/mtd/blktrans.h>
#include <linux/mtd/mtd.h>
#include <linux/blkdev.h>
#include <linux/blk.h>
#include <linux/blkpg.h>
#include <linux/spinlock.h>
#include <linux/hdreg.h>
#include <linux/init.h>
#include <asm/semaphore.h>
#include <asm/uaccess.h>

static LIST_HEAD(blktrans_majors);

extern struct semaphore mtd_table_mutex;
extern struct mtd_info *mtd_table[];

struct mtd_blkcore_priv {
	devfs_handle_t devfs_dir_handle;
	int blksizes[256];
	int sizes[256];
	struct hd_struct part_table[256];
	struct gendisk gd;
	spinlock_t devs_lock; /* See comment in _request function */
	struct completion thread_dead;
	int exiting;
	wait_queue_head_t thread_wq;
};

static inline struct mtd_blktrans_dev *tr_get_dev(struct mtd_blktrans_ops *tr,
					   int devnum)
{
	struct list_head *this;
	struct mtd_blktrans_dev *d;

	list_for_each(this, &tr->devs) {
		d = list_entry(this, struct mtd_blktrans_dev, list);

		if (d->devnum == devnum)
			return d;
	}
	return NULL;
}

static inline struct mtd_blktrans_ops *get_tr(int major)
{
	struct list_head *this;
	struct mtd_blktrans_ops *t;

	list_for_each(this, &blktrans_majors) {
		t = list_entry(this, struct mtd_blktrans_ops, list);

		if (t->major == major)
			return t;
	}
	return NULL;
}

static int do_blktrans_request(struct mtd_blktrans_ops *tr,
			       struct mtd_blktrans_dev *dev,
			       struct request *req)
{
	unsigned long block, nsect;
	char *buf;
	int minor;

	minor = MINOR(req->rq_dev);
	block = req->sector;
	nsect = req->current_nr_sectors;
	buf = req->buffer;

	if (block + nsect > tr->blkcore_priv->part_table[minor].nr_sects) {
		printk(KERN_WARNING "Access beyond end of device.\n");
		return 0;
	}
	block += tr->blkcore_priv->part_table[minor].start_sect;

	switch(req->cmd) {
	case READ:
		for (; nsect > 0; nsect--, block++, buf += 512)
			if (tr->readsect(dev, block, buf))
				return 0;
		return 1;

	case WRITE:
		if (!tr->writesect)
			return 0;

		for (; nsect > 0; nsect--, block++, buf += 512)
			if (tr->writesect(dev, block, buf))
				return 0;
		return 1;

	default:
		printk(KERN_NOTICE "Unknown request cmd %d\n", req->cmd);
		return 0;
	}
}

static int mtd_blktrans_thread(void *arg)
{
	struct mtd_blktrans_ops *tr = arg;
	struct request_queue *rq = BLK_DEFAULT_QUEUE(tr->major);

	/* we might get involved when memory gets low, so use PF_MEMALLOC */
	current->flags |= PF_MEMALLOC;

	snprintf(current->comm, sizeof(current->comm), "%sd", tr->name);

	/* daemonize() doesn't do this for us since some kernel threads
	   actually want to deal with signals. We can't just call 
	   exit_sighand() since that'll cause an oops when we finally
	   do exit. */
	spin_lock_irq(&current->sigmask_lock);
	sigfillset(&current->blocked);
	recalc_sigpending();
	spin_unlock_irq(&current->sigmask_lock);

	daemonize("%sd", tr->name);

	while (!tr->blkcore_priv->exiting) {
		struct request *req;
		struct mtd_blktrans_dev *dev;
		int devnum;
		int res = 0;
		DECLARE_WAITQUEUE(wait, current);

		spin_lock_irq(&io_request_lock);

		if (list_empty(&rq->queue_head)) {

			add_wait_queue(&tr->blkcore_priv->thread_wq, &wait);
			set_current_state(TASK_INTERRUPTIBLE);

			spin_unlock_irq(&io_request_lock);

			schedule();
			remove_wait_queue(&tr->blkcore_priv->thread_wq, &wait);

			continue;
		}

		req = blkdev_entry_next_request(&rq->queue_head);

		devnum = MINOR(req->rq_dev) >> tr->part_bits;

		/* The ll_rw_blk code knows not to touch the request
		   at the head of the queue */
		spin_unlock_irq(&io_request_lock);

		/* FIXME: Where can we store the dev, on which
		   we already have a refcount anyway? We need to
		   lock against concurrent addition/removal of devices,
		   but if we use the mtd_table_mutex we deadlock when
		   grok_partitions is called from the registration
		   callbacks. */
		spin_lock(&tr->blkcore_priv->devs_lock);
		dev = tr_get_dev(tr, devnum);
		spin_unlock(&tr->blkcore_priv->devs_lock);

		BUG_ON(!dev);

		/* Ensure serialisation of requests */
		down(&dev->sem);

		res = do_blktrans_request(tr, dev, req);
		up(&dev->sem);

		if (!end_that_request_first(req, res, tr->name)) {
			spin_lock_irq(&io_request_lock);
			blkdev_dequeue_request(req);
			end_that_request_last(req);
			spin_unlock_irq(&io_request_lock);
		}
	}
	complete_and_exit(&tr->blkcore_priv->thread_dead, 0);
}

static void mtd_blktrans_request(struct request_queue *rq)
{
	struct mtd_blktrans_ops *tr = rq->queuedata;
	wake_up(&tr->blkcore_priv->thread_wq);
}

int blktrans_open(struct inode *i, struct file *f)
{
	struct mtd_blktrans_ops *tr = NULL;
	struct mtd_blktrans_dev *dev = NULL;
	int major_nr = MAJOR(i->i_rdev);
	int minor_nr = MINOR(i->i_rdev);
	int devnum;
	int ret = -ENODEV;

	if (is_read_only(i->i_rdev) && (f->f_mode & FMODE_WRITE))
		return -EROFS;

	down(&mtd_table_mutex);

	tr = get_tr(major_nr);

	if (!tr)
		goto out;
 
	devnum = minor_nr >> tr->part_bits;

	dev = tr_get_dev(tr, devnum);

	if (!dev)
		goto out;

	if (!tr->blkcore_priv->part_table[minor_nr].nr_sects) {
		ret = -ENODEV;
		goto out;
	}

	if (!try_inc_mod_count(dev->mtd->owner))
		goto out;

	if (!try_inc_mod_count(tr->owner))
		goto out_tr;

	dev->mtd->usecount++;

	ret = 0;
	if (tr->open && (ret = tr->open(dev))) {
		dev->mtd->usecount--;
		if (dev->mtd->owner)
			__MOD_DEC_USE_COUNT(dev->mtd->owner);
	out_tr:
		if (tr->owner)
			__MOD_DEC_USE_COUNT(tr->owner);
	}
 out:
	up(&mtd_table_mutex);

	return ret;
}

int blktrans_release(struct inode *i, struct file *f)
{
	struct mtd_blktrans_dev *dev;
	struct mtd_blktrans_ops *tr;
	int ret = 0;
	int devnum;

	down(&mtd_table_mutex);

	tr = get_tr(MAJOR(i->i_rdev));
	if (!tr) {
		up(&mtd_table_mutex);
		return -ENODEV;
	}

	devnum = MINOR(i->i_rdev) >> tr->part_bits;
	dev = tr_get_dev(tr, devnum);

	if (!dev) {
		up(&mtd_table_mutex);
		return -ENODEV;
	}

	if (tr->release)
		ret = tr->release(dev);

	if (!ret) {
		dev->mtd->usecount--;
		if (dev->mtd->owner)
			__MOD_DEC_USE_COUNT(dev->mtd->owner);
		if (tr->owner)
			__MOD_DEC_USE_COUNT(tr->owner);
	}
	
	up(&mtd_table_mutex);

	return ret;
}

static int mtd_blktrans_rrpart(kdev_t rdev, struct mtd_blktrans_ops *tr,
			       struct mtd_blktrans_dev *dev)
{
	struct gendisk *gd = &(tr->blkcore_priv->gd);
	int i;
	int minor = MINOR(rdev);

	if (minor & ((1<<tr->part_bits)-1) || !tr->part_bits) {
		/* BLKRRPART on a partition. Go away. */
		return -ENOTTY;
	}

	if (!capable(CAP_SYS_ADMIN))
	    return -EACCES;

	/* We are required to prevent simultaneous open() ourselves.
	   The core doesn't do that for us. Did I ever mention how
	   much the Linux block layer sucks? Sledgehammer approach... */
	down(&mtd_table_mutex);

	for (i=0; i < (1<<tr->part_bits); i++) {
		invalidate_device(MKDEV(tr->major, minor+i), 1);
		gd->part[minor + i].start_sect = 0;
		gd->part[minor + i].nr_sects = 0;
	}

	grok_partitions(gd, minor, 1 << tr->part_bits, 
			tr->blkcore_priv->sizes[minor]);
	up(&mtd_table_mutex);

	return 0;
}

static int blktrans_ioctl(struct inode *inode, struct file *file, 
			      unsigned int cmd, unsigned long arg)
{
	struct mtd_blktrans_dev *dev;
	struct mtd_blktrans_ops *tr;
	int devnum;

	switch(cmd) {
	case BLKGETSIZE:
        case BLKGETSIZE64:
        case BLKBSZSET:
        case BLKBSZGET:
        case BLKROSET:
        case BLKROGET:
        case BLKRASET:
        case BLKRAGET:
        case BLKPG:
        case BLKELVGET:
        case BLKELVSET:
		return blk_ioctl(inode->i_rdev, cmd, arg);
	}

	down(&mtd_table_mutex);

	tr = get_tr(MAJOR(inode->i_rdev));
	if (!tr) {
		up(&mtd_table_mutex);
		return -ENODEV;
	}

	devnum = MINOR(inode->i_rdev) >> tr->part_bits;
	dev = tr_get_dev(tr, devnum);

	up(&mtd_table_mutex);

	if (!dev)
		return -ENODEV;

	switch(cmd) {
	case BLKRRPART:
		return mtd_blktrans_rrpart(inode->i_rdev, tr, dev);
		
        case BLKFLSBUF:
		blk_ioctl(inode->i_rdev, cmd, arg);
		if (tr->flush)
			return tr->flush(dev);
		/* The core code did the work, we had nothing to do. */
		return 0;

	case HDIO_GETGEO:
		if (tr->getgeo) {
			struct hd_geometry g;
			struct gendisk *gd = &(tr->blkcore_priv->gd);
			int ret;

			memset(&g, 0, sizeof(g));
			ret = tr->getgeo(dev, &g);
			if (ret)
				return ret;

			g.start = gd->part[MINOR(inode->i_rdev)].start_sect;
			if (copy_to_user((void *)arg, &g, sizeof(g)))
				return -EFAULT;
			return 0;
		} /* else */
	default:
		return -ENOTTY;
	}
}

struct block_device_operations mtd_blktrans_ops = {
	.owner		= THIS_MODULE,
	.open		= blktrans_open,
	.release	= blktrans_release,
	.ioctl		= blktrans_ioctl,
};

int add_mtd_blktrans_dev(struct mtd_blktrans_dev *new)
{
	struct mtd_blktrans_ops *tr = new->tr;
	struct list_head *this;
	int last_devnum = -1;
	int i;

	if (!down_trylock(&mtd_table_mutex)) {
		up(&mtd_table_mutex);
		BUG();
	}

	spin_lock(&tr->blkcore_priv->devs_lock);

	list_for_each(this, &tr->devs) {
		struct mtd_blktrans_dev *d = list_entry(this, struct mtd_blktrans_dev, list);
		if (new->devnum == -1) {
			/* Use first free number */
			if (d->devnum != last_devnum+1) {
				/* Found a free devnum. Plug it in here */
				new->devnum = last_devnum+1;
				list_add_tail(&new->list, &d->list);
				goto added;
			}
		} else if (d->devnum == new->devnum) {
			/* Required number taken */
			spin_unlock(&tr->blkcore_priv->devs_lock);
			return -EBUSY;
		} else if (d->devnum > new->devnum) {
			/* Required number was free */
			list_add_tail(&new->list, &d->list);
			goto added;
		} 
		last_devnum = d->devnum;
	}
	if (new->devnum == -1)
		new->devnum = last_devnum+1;

	if ((new->devnum << tr->part_bits) > 256) {
		spin_unlock(&tr->blkcore_priv->devs_lock);
		return -EBUSY;
	}

	init_MUTEX(&new->sem);
	list_add_tail(&new->list, &tr->devs);
 added:
	spin_unlock(&tr->blkcore_priv->devs_lock);

	if (!tr->writesect)
		new->readonly = 1;

	for (i = new->devnum << tr->part_bits;
	     i < (new->devnum+1) << tr->part_bits; 
	     i++) {
		set_device_ro(MKDEV(tr->major, i), new->readonly);
		tr->blkcore_priv->blksizes[i] = new->blksize;
		tr->blkcore_priv->sizes[i] = 0;
		tr->blkcore_priv->part_table[i].nr_sects = 0;
		tr->blkcore_priv->part_table[i].start_sect = 0;
	}

	/*
	  <viro_zzz> dwmw2: BLOCK_SIZE_BITS has nothing to do with block devices
	  <viro> dwmw2: any code which sets blk_size[][] should be 
			size >> 10 /+ 2.4 and its dumb units */

	tr->blkcore_priv->sizes[new->devnum << tr->part_bits] = 
		(new->size * new->blksize) >> 10; /* 2.4 and its dumb units */

	/* But this is still in device's sectors? $DEITY knows */
	tr->blkcore_priv->part_table[new->devnum << tr->part_bits].nr_sects = new->size;

	if (tr->part_bits) {
		grok_partitions(&tr->blkcore_priv->gd, new->devnum,
				1 << tr->part_bits, new->size);
	}
#ifdef CONFIG_DEVFS_FS
	if (!tr->part_bits) {
		char name[2];

		name[0] = '0' + new->devnum;
		name[1] = 0;

		new->blkcore_priv = 
			devfs_register(tr->blkcore_priv->devfs_dir_handle,
				       name, DEVFS_FL_DEFAULT, tr->major,
				       new->devnum, S_IFBLK|S_IRUGO|S_IWUGO,
				       &mtd_blktrans_ops, NULL);
	}
#endif
	return 0;
}

int del_mtd_blktrans_dev(struct mtd_blktrans_dev *old)
{
	struct mtd_blktrans_ops *tr = old->tr;
	int i;

	if (!down_trylock(&mtd_table_mutex)) {
		up(&mtd_table_mutex);
		BUG();
	}

#ifdef CONFIG_DEVFS_FS
	if (!tr->part_bits) {
		devfs_unregister(old->blkcore_priv);
		old->blkcore_priv = NULL;
	} else {
		devfs_register_partitions(&tr->blkcore_priv->gd,
					  old->devnum << tr->part_bits, 1);
	}
#endif
	spin_lock(&tr->blkcore_priv->devs_lock);
	list_del(&old->list);
	spin_unlock(&tr->blkcore_priv->devs_lock);

	for (i = (old->devnum << tr->part_bits); 
	     i < ((old->devnum+1) << tr->part_bits); i++) {
		tr->blkcore_priv->sizes[i] = 0;
		tr->blkcore_priv->part_table[i].nr_sects = 0;
		tr->blkcore_priv->part_table[i].start_sect = 0;
	}

	return 0;
}

void blktrans_notify_remove(struct mtd_info *mtd)
{
	struct list_head *this, *this2, *next;

	list_for_each(this, &blktrans_majors) {
		struct mtd_blktrans_ops *tr = list_entry(this, struct mtd_blktrans_ops, list);

		list_for_each_safe(this2, next, &tr->devs) {
			struct mtd_blktrans_dev *dev = list_entry(this2, struct mtd_blktrans_dev, list);

			if (dev->mtd == mtd)
				tr->remove_dev(dev);
		}
	}
}

void blktrans_notify_add(struct mtd_info *mtd)
{
	struct list_head *this;

	if (mtd->type == MTD_ABSENT)
		return;

	list_for_each(this, &blktrans_majors) {
		struct mtd_blktrans_ops *tr = list_entry(this, struct mtd_blktrans_ops, list);

		tr->add_mtd(tr, mtd);
	}

}

static struct mtd_notifier blktrans_notifier = {
	.add = blktrans_notify_add,
	.remove = blktrans_notify_remove,
};
      
int register_mtd_blktrans(struct mtd_blktrans_ops *tr)
{
	int ret, i;

	/* Register the notifier if/when the first device type is 
	   registered, to prevent the link/init ordering from fucking
	   us over. */
	if (!blktrans_notifier.list.next)
		register_mtd_user(&blktrans_notifier);

	tr->blkcore_priv = kmalloc(sizeof(*tr->blkcore_priv), GFP_KERNEL);
	if (!tr->blkcore_priv)
		return -ENOMEM;

	memset(tr->blkcore_priv, 0, sizeof(*tr->blkcore_priv));

	down(&mtd_table_mutex);

	ret = devfs_register_blkdev(tr->major, tr->name, &mtd_blktrans_ops);
	if (ret) {
		printk(KERN_WARNING "Unable to register %s block device on major %d: %d\n",
		       tr->name, tr->major, ret);
		kfree(tr->blkcore_priv);
		up(&mtd_table_mutex);
		return ret;
	}

	blk_init_queue(BLK_DEFAULT_QUEUE(tr->major), &mtd_blktrans_request);
	(BLK_DEFAULT_QUEUE(tr->major))->queuedata = tr;
	
	init_completion(&tr->blkcore_priv->thread_dead);
	init_waitqueue_head(&tr->blkcore_priv->thread_wq);

	ret = kernel_thread(mtd_blktrans_thread, tr, 
			  CLONE_FS|CLONE_FILES|CLONE_SIGHAND);
	if (ret < 0) {
		blk_cleanup_queue(BLK_DEFAULT_QUEUE(tr->major));
		devfs_unregister_blkdev(tr->major, tr->name);
		kfree(tr->blkcore_priv);
		up(&mtd_table_mutex);
		return ret;
	} 

	tr->blkcore_priv->devfs_dir_handle = 
			devfs_mk_dir(NULL, tr->name, NULL);

	blksize_size[tr->major] = tr->blkcore_priv->blksizes;
	blk_size[tr->major] = tr->blkcore_priv->sizes;

	tr->blkcore_priv->gd.major = tr->major;
	tr->blkcore_priv->gd.major_name = tr->name;
	tr->blkcore_priv->gd.minor_shift = tr->part_bits;
	tr->blkcore_priv->gd.max_p = (1<<tr->part_bits) - 1;
	tr->blkcore_priv->gd.part = tr->blkcore_priv->part_table;
	tr->blkcore_priv->gd.sizes = tr->blkcore_priv->sizes;
	tr->blkcore_priv->gd.nr_real = 256 >> tr->part_bits;

	spin_lock_init(&tr->blkcore_priv->devs_lock);

	add_gendisk(&tr->blkcore_priv->gd);

	INIT_LIST_HEAD(&tr->devs);
	list_add(&tr->list, &blktrans_majors);

	for (i=0; i<MAX_MTD_DEVICES; i++) {
		if (mtd_table[i] && mtd_table[i]->type != MTD_ABSENT)
			tr->add_mtd(tr, mtd_table[i]);
	}
	up(&mtd_table_mutex);

	return 0;
}

int deregister_mtd_blktrans(struct mtd_blktrans_ops *tr)
{
	struct list_head *this, *next;

	down(&mtd_table_mutex);

	/* Clean up the kernel thread */
	tr->blkcore_priv->exiting = 1;
	wake_up(&tr->blkcore_priv->thread_wq);
	wait_for_completion(&tr->blkcore_priv->thread_dead);
	
	/* Remove it from the list of active majors */
	list_del(&tr->list);

	/* Remove each of its devices */
	list_for_each_safe(this, next, &tr->devs) {
		struct mtd_blktrans_dev *dev = list_entry(this, struct mtd_blktrans_dev, list);
		tr->remove_dev(dev);
	}

	blksize_size[tr->major] = NULL;
	blk_size[tr->major] = NULL;

	del_gendisk(&tr->blkcore_priv->gd);

	blk_cleanup_queue(BLK_DEFAULT_QUEUE(tr->major));
	devfs_unregister_blkdev(tr->major, tr->name);

	devfs_unregister(tr->blkcore_priv->devfs_dir_handle);

	up(&mtd_table_mutex);

	kfree(tr->blkcore_priv);

	if (!list_empty(&tr->devs))
		BUG();
	return 0;
}

static void __exit mtd_blktrans_exit(void)
{
	/* No race here -- if someone's currently in register_mtd_blktrans
	   we're screwed anyway. */
	if (blktrans_notifier.list.next)
		unregister_mtd_user(&blktrans_notifier);
}

module_exit(mtd_blktrans_exit);

EXPORT_SYMBOL_GPL(register_mtd_blktrans);
EXPORT_SYMBOL_GPL(deregister_mtd_blktrans);
EXPORT_SYMBOL_GPL(add_mtd_blktrans_dev);
EXPORT_SYMBOL_GPL(del_mtd_blktrans_dev);

MODULE_AUTHOR("David Woodhouse <dwmw2@infradead.org>");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Common interface to block layer for MTD 'translation layers'");
