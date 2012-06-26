/*
 * linux/drivers/char/vdd.c
 * 
 * Winbond uclinux virtual debug device.
 * yyang1|pc32  030528
 *
 */	
 
#include <linux/config.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <asm/uaccess.h>
#include <asm/page.h>
#include <linux/errno.h>

#ifndef _DEBUG
#define _DEBUG
#endif
#undef _DEBUG

#ifndef EOF
#define EOF (-1)
#endif

typedef unsigned char	BYTE;
typedef unsigned int	BOOL;
#define TRUE	1
#define FALSE	0

#define MALLOC(x) 	kmalloc((x), GFP_KERNEL)
#define FREE		kfree
#define MEMCPY  	memcpy //or use "copy_to_user"

#define	DEBUGBUF_SIZE	(1024 * 2)


#define		VDD_CLEAR	0x5901 //clear current queue
#define 	MAX_VDD_DEV  	2
#define 	VDD_MAJOR	99

typedef struct Queue
{
	void *	queue;
	size_t	MaxSize;
	size_t	begin;
	size_t  end;
	size_t  usage;
} Queue_t;

#ifdef CONFIG_DEVFS_FS
#include <linux/devfs_fs_kernel.h>
static devfs_handle_t devfs_vdd_handle[MAX_VDD_DEV]; 
static DECLARE_MUTEX(vdd_table_mutex);
#endif

static Queue_t debug_queue = {NULL, DEBUGBUF_SIZE + 1, 0, 0, 0};

static int 	vdd_open(struct inode *inode, struct file *filep);
static int 	vdd_release(struct inode *inode, struct file *filep);
static ssize_t 	vdd_read(struct file *filp, char * buf, size_t count, loff_t *f_pos);
static ssize_t 	vdd_write(struct file *filp, const char * buf, size_t count, loff_t *f_pos);
static loff_t  	vdd_llseek (struct file *, loff_t, int);
static int 	vdd_ioctl (struct inode * inode, struct file * filp, unsigned int cmd, unsigned long arg);


static struct file_operations vdd_ops = 
{
	open: 		vdd_open,
	read:		vdd_read,
	write:		vdd_write,
	llseek:		vdd_llseek,
	release: 	vdd_release,
	ioctl:		vdd_ioctl,
};

//----------------------------queue funcs----------------------------------//
#define _Capacity()	(debug_queue.MaxSize - 1)

#define _IsEmpty() 	(debug_queue.begin == debug_queue.end)
#define _IsFull() 	(((debug_queue.end + 1) % debug_queue.MaxSize == debug_queue.begin) ? TRUE : FALSE)
#define _Free()  	(_Capacity() - _Size())

#define _Begin() 	(debug_queue.begin)
#define _End() 	 	(debug_queue.end)
#define _nIterator(x)	((_Begin() + (x)) % debug_queue.MaxSize)

#define _First() 	(((BYTE *)debug_queue.queue)[(debug_queue.begin + 1) % debug_queue.MaxSize])
#define _Last()  	(((BYTE *)debug_queue.queue)[debug_queue.end])

#define _Usage() 	(debug_queue.usage)

static size_t _Size()
{
	long diff_ = (long)debug_queue.end - (long)debug_queue.begin;
	return (diff_ >= 0) ? diff_ : (debug_queue.MaxSize + diff_);
}

static size_t _Distance(size_t begin, size_t end)
{
	long diff_ = (long)end - (long)begin;
	return (diff_ >= 0) ? diff_ : (debug_queue.MaxSize + diff_);	
}

static BYTE _nElement(size_t index) // index = 0 --> *(begin + 1)
{
//	ASSERT((index >= 0) && (index < _Size()));
	return ((BYTE *)debug_queue.queue)[_nIterator(index + 1)];
}

static void _Delete()
{
	if (_IsEmpty()) 
		return;

	debug_queue.begin = (debug_queue.begin + 1) % debug_queue.MaxSize;
}

static void _Add(BYTE x)
{
	if (_IsFull()) 
		_Delete();

	debug_queue.end = (debug_queue.end + 1) % debug_queue.MaxSize;
	((BYTE *)debug_queue.queue)[debug_queue.end] = x;
}

//random and memcpy to optimize..
static size_t N_copy(void *buf, size_t begin, size_t end)
{
	void * src_, *dst_ = buf;
	size_t copy_num = 0;
	
//	ASSERT(buf && (begin < debug_queue.MaxSize) && (end < debug_queue.MaxSize));

	if(end == begin)
		return 0;

	if(end < begin)
	{
		size_t split_diff = _Capacity() - begin;
		if(split_diff)
		{
			(BYTE *)src_ = (BYTE *)debug_queue.queue + (begin + 1) % _Capacity();
			MEMCPY(dst_, src_, split_diff);
			copy_num += split_diff;
			(BYTE *)dst_ += copy_num;
		}
		
		src_ = debug_queue.queue;
		MEMCPY(dst_, src_, end + 1);
		copy_num += end + 1;
	}
	else // greater than
	{
		copy_num = end - begin;
		(BYTE *)src_ = (BYTE *)debug_queue.queue + begin + 1;
		MEMCPY(dst_, src_, copy_num);
	}

	return copy_num;
}

static void N_Delete(size_t n)
{
	if (_Size() < n)
	{
		debug_queue.begin = debug_queue.end;
		return;
	}

	debug_queue.begin = (debug_queue.begin + n) % debug_queue.MaxSize;
}

static void N_Add(const void *buf, size_t count)
{
	size_t freed_;
	size_t valid_ = (count > _Capacity()) ? _Capacity() : count;
	void * src_, *dst_;	
	src_ = (BYTE *)buf + count - valid_;
	dst_ = (BYTE *)debug_queue.queue + (debug_queue.end + 1) % debug_queue.MaxSize;

#ifdef _DEBUG
	printk("***N_Add(buf = 0x%x, count = %d).\n", buf, count);
#endif
	
	freed_ = _Free();
	if (freed_ < valid_) 
	{
#ifdef _DEBUG		
		printk("Free: %d, valid %d.\n", _Free(), valid_);
#endif
		
		N_Delete(valid_ - freed_);
	}

	if(debug_queue.end >= debug_queue.begin)
	{
		size_t split_diff = _Capacity() - debug_queue.end;
		if(valid_ > split_diff)
		{
			MEMCPY(dst_, src_, split_diff);

			debug_queue.end = _Capacity();

			(BYTE *)src_ += split_diff;
			valid_ -= split_diff;
			dst_ = debug_queue.queue;
		}
	}

	MEMCPY(dst_, src_, valid_);
	debug_queue.end = ((BYTE *)dst_ - (BYTE *)debug_queue.queue) +  valid_ - 1;
}

//----------------------------driver funcs----------------------------------//
static int vdd_ioctl (struct inode * inode, struct file * filp, unsigned int cmd, unsigned long arg)
{
	
	
#ifdef _DEBUG		
	printk("***vdd_ioctl: cmd = 0x%x, arg = 0x%x\n", cmd, arg);
#endif	
	
	switch(cmd)
	{
		case VDD_CLEAR:
		{
			debug_queue.begin = debug_queue.end = 0;

#ifdef _DEBUG		
			printk("***vdd_ioctl[VDD_CLEAR], now queue size = %d.\n", _Size());
#endif				

			break;	
		}
		default:
			return -EINVAL;	
	}
	
	return 0;
}

static loff_t  vdd_llseek (struct file *filp, loff_t off, int whence)
{
	long newpos;

#ifdef _DEBUG		
	printk("*** 1 vdd_llseek off = %llu, whence = %d.\n", off, whence);
#endif	
	
	switch(whence)
	{
	case 0: //begin
		newpos = off;
		break;	
		
	case 1: //current
		newpos = filp->f_pos + off;
		break;
	
	case 2: //end
		newpos = _Size() + off;
		break;		
	
	default:
		return -EINVAL;				
	}
	
	if(newpos < 0)
		return -EINVAL;				
	
	filp->f_pos = newpos;	

#ifdef _DEBUG		
	printk("*** 2 vdd_llseek newpos = %d, f_pos = %llu, whence = %d.\n", newpos, filp->f_pos, whence);
#endif	

	return newpos;	
}

static ssize_t vdd_read(struct file *filep, char * buf, size_t count, loff_t *f_pos)
{
	size_t  read_num = 0; 
	size_t 	begin_, end_;

	if(!debug_queue.queue || !count)
		return 0;
	
#ifdef _DEBUG		
	printk("***before debug_read count = %d, buf = 0x%x, f_pos = 0x%x.\n", count, buf, filep->f_pos);	
#endif	
				
#if 0
	
	begin_ = (count >= _Size()) ? _Begin() : (_nIterator(_Size() - count));
	read_num = N_copy(buf, begin_, _End());

#else

	if(filep->f_pos >= _Size())
	{
		//filep->f_pos = 0; //if end, then lseek to begin...
		read_num = 0;
	}
	else
	{
		begin_ = _nIterator(filep->f_pos);
		end_ = (count >= _Distance(begin_, _End())) ? _End() : (_nIterator(_Distance(_Begin(), begin_) + count));
		
		_Usage()++;
		read_num = N_copy(buf, begin_, end_);
		_Usage()--;
		
#ifdef _DEBUG		
		printk("***debug_reading begin_ = %d, end_ = %d, read_num = %d.\n", begin_, end_, read_num);	
#endif		
		filep->f_pos += read_num;
	}
				
#endif

#ifdef _DEBUG		
	printk("***after debug_read count = %d, buf = 0x%x, f_pos = 0x%x.\n", count, buf, filep->f_pos);	
#endif	

	return read_num;
}

static ssize_t vdd_write(struct file *filep, const char * buf, size_t count, loff_t *f_pos)
{
	if(!debug_queue.queue)
		return 0;
	
	_Usage()++;
	N_Add(buf, count);
	_Usage()--;
	
#ifdef _DEBUG		
	printk("***debug_write count = %d, buf = 0x%x, f_pos = 0x%x.\n", count, buf, filep->f_pos);	
#endif	
	
	return count;
}




static int vdd_open(struct inode *inode, struct file *filep)
{
	unsigned int dev = MINOR(inode->i_rdev);
	
	if(dev >= MAX_VDD_DEV)	
		return -EFAULT;
		
	//030529?	
	if(_Usage())
		return -EFAULT;
		
	filep->f_op = &vdd_ops;
	filep->f_pos = 0;
	
	MOD_INC_USE_COUNT;	

#ifdef _DEBUG
	printk("open VDD[vdd%d] succeed!\n", dev);
#endif

	return 0;	
}

static int vdd_release(struct inode *inode, struct file *filep)
{
	 MOD_DEC_USE_COUNT;
	 
#ifdef _DEBUG
	printk("close VDD succeed!\n");
#endif	 
	 return 0;
}
#ifdef CONFIG_DEVFS_FS
static void register_vdd_usr(void)
{
	int i;
		
	down(&vdd_table_mutex);
	
	MOD_INC_USE_COUNT;
	
	for (i=0; i< MAX_VDD_DEV; i++)
	{
		char name[8];
		sprintf(name, "vdd%d", i);

		devfs_vdd_handle[i] = devfs_register(NULL, name,
			DEVFS_FL_DEFAULT, VDD_MAJOR, i,
			S_IFCHR | S_IRUGO | S_IWUGO,
			&vdd_ops, NULL);

	}
	
	up(&vdd_table_mutex);
}	

static void unregister_vdd_usr(void)
{
	int i;
		
	down(&vdd_table_mutex);
	
	MOD_DEC_USE_COUNT;
	
	for (i=0; i< MAX_VDD_DEV; i++)
		devfs_unregister(devfs_vdd_handle[i]);
	
	up(&vdd_table_mutex);
}	
#endif
//----------------------------module funcs----------------------------------//
int vdd_init(void)
{
	int result;
	
#ifdef _DEBUG	
	char *cptr = "Winbond Virtual Debug Device[c, 2k, round queue]!\n";
	printk("****vdd initing...");	
#endif
#ifdef CONFIG_DEVFS_FS
	result = devfs_register_chrdev(VDD_MAJOR, "vdd", &vdd_ops);
	if(result < 0)
	{
		printk("VDD: can't register vdd.\n");
		return result;
	}
	
	register_vdd_usr();		
#else	
	result = register_chrdev(VDD_MAJOR, "vdd", &vdd_ops);
	if(result < 0)
	{
		printk("VDD: can't register vdd.\n");
		return result;
	}
#endif	
	if(!debug_queue.queue)
		debug_queue.queue = MALLOC(debug_queue.MaxSize);
		
#ifdef _DEBUG
	N_Add(cptr, strlen(cptr)); 
#endif	
	return 0;
}

void vdd_cleanup(void)
{
	if(debug_queue.queue)
	{
		FREE(debug_queue.queue);		
		debug_queue.queue = NULL;
	}
#ifdef CONFIG_DEVFS_FS
	unregister_vdd_usr();
	devfs_unregister_chrdev(VDD_MAJOR, "vdd");
#else	
	unregister_chrdev(VDD_MAJOR, "vdd");
#endif	
}

module_init(vdd_init);
module_exit(vdd_cleanup);
