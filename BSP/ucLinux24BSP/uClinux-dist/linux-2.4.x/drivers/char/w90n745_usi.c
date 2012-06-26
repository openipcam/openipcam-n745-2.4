/****************************************************************************
 * 
 * Copyright (c) 2004 - 2006 Winbond Electronics Corp. All rights reserved. 
 *
 ****************************************************************************/
 
/****************************************************************************
 * 
 * FILENAME
 *     w90n745_usi.c
 *
 * VERSION
 *     1.0
 *
 * DESCRIPTION
 *     USI driver supported for W90n710.
 *
 * FUNCTIONS
 *	all functions, if they has return value, return 0 if they success, others failed.
 *
 * HISTORY
 *	2006/01/10		Created by QFu
 *
 * REMARK
 *     None
 *************************************************************************/
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/ioport.h>
#include <linux/delay.h>
#include <linux/time.h>
#include <linux/sched.h>
#include <linux/slab.h>

#include <asm/arch/hardware.h>
#include <asm/arch/irqs.h>

#include <asm/uaccess.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <asm/string.h>
#include <asm/atomic.h>

#include "w90n745_usi.h"

//#define USI_DEBUG
//#define USI_DEBUG_ENABLE_ENTER_LEAVE
//#define USI_DEBUG_ENABLE_MSG
//#define USI_DEBUG_ENABLE_MSG2

#ifdef USI_DEBUG
#define PDEBUG(fmt, arg...)		printk(fmt, ##arg)
#else
#define PDEBUG(fmt, arg...)
#endif

#ifdef USI_DEBUG_ENABLE_ENTER_LEAVE
#define ENTER()					PDEBUG("[%-20s] : Enter...\n", __FUNCTION__)
#define LEAVE()					PDEBUG("[%-20s] : Leave...\n", __FUNCTION__)
#else
#define ENTER()
#define LEAVE()
#endif

#ifdef USI_DEBUG_ENABLE_MSG
#define MSG(msg)				PDEBUG("[%-20s] : %s\n", __FUNCTION__, msg)
#else
#define MSG(msg)
#endif

#ifdef USI_DEBUG_ENABLE_MSG2
#define MSG2(fmt, arg...)			PDEBUG("[%-20s] : "fmt, __FUNCTION__, ##arg)
#define PRNBUF(buf, count)		{int i;MSG2("Data: ");for(i=0;i<count;i++)\
									PDEBUG("%02x ", buf[i]);PDEBUG("\n");}
#else
#define MSG2(fmt, arg...)
#define PRNBUF(buf, count)
#endif

#define usi_inl(addr)			(*((volatile u32  *)(addr)))
#define usi_outl(val, addr)	(*((volatile u32  *)(addr))=(val))


static atomic_t usi_available = ATOMIC_INIT(1);
static struct usi_parameter global_parameter;
static wait_queue_head_t wq;
static volatile int trans_finish, slave_select;

void usi_deselect_slave(void)
{
	usi_outl(usi_inl(USI_SSR)&0xc, USI_SSR);
	slave_select = 0;
}

void usi_select_slave(int x)
{
	usi_outl((global_parameter.active_level << 2)|(1 << x), USI_SSR);
	slave_select = 1;
}

static void usi_interrupt(int irq, void * dev_id, struct pt_regs *regs)
{
	u32 reg;

	ENTER();

	reg = usi_inl(USI_CNTRL);

	if (!(reg & 0x10000))	/* it not me ? */
		return;

	reg |= 0x10000;
	usi_outl(reg, USI_CNTRL);		/* clear interrupt flag */

	trans_finish = 1;

	wake_up_interruptible(&wq);

	LEAVE();
}

static int usi_transit(struct usi_data *data_ptr)
{
	u32 reg,mask;
	
	ENTER();

	if (slave_select == 0)
		return -ENODEV;

	mask = (1 << data_ptr->bit_len) - 1;

	MSG2("bit_len : %d, mask : %x\n", data_ptr->bit_len, mask);

	usi_outl(data_ptr->write_data & mask , USI_TX0);		/* write data to hardware buffer */

	MSG2("-> %x\n", data_ptr->write_data & mask);

	reg = (global_parameter.sleep << 12) |
		(global_parameter.lsb << 10) |
		(data_ptr->bit_len << 3) |
		(global_parameter.tx_neg << 2) |
		(global_parameter.rx_neg << 1) | 0x20001;

	trans_finish = 0;
	usi_outl(reg, USI_CNTRL);		/* start */
	wait_event_interruptible(wq, trans_finish != 0);

	data_ptr->read_data = usi_inl(USI_RX0) & mask;

	MSG2("<- %x\n", data_ptr->read_data & mask);

	LEAVE();

	return 0;

}

static int usi_ioctl(struct inode *inode, struct file *flip, unsigned int cmd, unsigned long arg)
{
	int err = 0;
	struct usi_parameter tmp_parameter;
	struct usi_data tmp_data;

	ENTER();
	
	if(_IOC_TYPE(cmd) != USI_IOC_MAGIC) return -ENOTTY;
	if(_IOC_NR(cmd) > USI_IOC_MAXNR) return -ENOTTY;

	if(_IOC_DIR(cmd) & _IOC_READ)
		err = !access_ok(VERIFY_WRITE, (void *)arg, _IOC_SIZE(cmd));
	else if(_IOC_DIR(cmd) & _IOC_WRITE)
		err = !access_ok(VERIFY_READ, (void *)arg, _IOC_SIZE(cmd));
	
	if(err) 
		return -EFAULT;

	switch(cmd) {
		case USI_IOC_GETPARAMETER:
			if (copy_to_user((void *)arg, &global_parameter, 
				sizeof(struct usi_parameter)))
				return -EFAULT;
			break;
		case USI_IOC_SETPARAMETER:
			if (copy_from_user(&tmp_parameter, (void *)arg, 
				sizeof(struct usi_parameter)))
				return -EFAULT;
			memcpy(&global_parameter, &tmp_parameter,
				sizeof(struct usi_parameter));

			usi_outl(global_parameter.divider, USI_DIVIDER);	/* update clock */
			
			break;

		case USI_IOC_SELECTSLAVE:
			if (arg < -1 && arg > 1)
				return -EPERM;
			if (arg == -1)
				usi_deselect_slave();
			else
				usi_select_slave(arg);
			break;

		case USI_IOC_TRANSIT:
			if (copy_from_user(&tmp_data, (void *)arg, sizeof(tmp_data)))
				return -EFAULT;
			err = usi_transit(&tmp_data);
			if (err)
				return err;
			if (copy_to_user((void *)arg, &tmp_data, sizeof(tmp_data)))
				return -EFAULT;
			break;

		default:
			return -ENOTTY;
			
	}

	LEAVE();
			
	return 0;
}


static int usi_open(struct inode *inode, struct file *filp)
{
	u32 reg;
	int retval = -EBUSY;

	ENTER();

	if (! atomic_dec_and_test (&usi_available)) 
		goto failed;

	global_parameter.active_level = 0;
	global_parameter.lsb = 0;
	global_parameter.rx_neg = 0;
	global_parameter.tx_neg = 1;
	global_parameter.divider = 0x1;
	global_parameter.sleep = 0;
	slave_select = 0;

	reg = usi_inl(0xFFF83050) & 0xfff00fff;
	reg |= 0xaa000;
	usi_outl(reg, 0xFFF83050);

	reg = usi_inl(USI_CNTRL);
	reg |= 0x20000;
	usi_outl(reg, USI_CNTRL);

	MSG2("GPIO 5 : %x\n", usi_inl(0xFFF83050));

	usi_outl(global_parameter.divider, USI_DIVIDER);	/* set clock */

	if ((retval = request_irq(INT_SPI, usi_interrupt, SA_INTERRUPT, "usi", NULL)) < 0) {
		printk("usi : request irq error\n");
		goto failed;
	}

	LEAVE();

	return 0; /* success */

failed:

	atomic_inc(&usi_available); /* release the device */

	return retval;
}

static int usi_release(struct inode *inode, struct file *flip)
{
	u32 reg;
	
	ENTER();

	reg = usi_inl(USI_CNTRL);
	reg &= 0xffff;
	usi_outl(reg, USI_CNTRL);

	free_irq(INT_SPI, NULL);
	usi_deselect_slave();
	atomic_inc(&usi_available); /* release the device */

	LEAVE();
	
	return 0;
}


struct file_operations usi_fops =                                                 
{
	owner: 		THIS_MODULE,
	open:		usi_open,
	release:		usi_release,
	ioctl:		usi_ioctl,
};

static int __init usi_init(void)
{
	u32 reg;
	int result;

	init_waitqueue_head(&wq);

	/* every things ok, now, we can register char device safely */

	result = register_chrdev(USI_MAJOR, "usi", &usi_fops);
	if( result < 0){
		unregister_chrdev(USI_MAJOR, "usi"); 
		printk("usi : can't get major %d\n", USI_MAJOR);
		goto failed;
	}

	printk("USI driver has been installed successfully!\n");

failed:

	return result;

}

static void __exit usi_exit(void)
{
	unregister_chrdev(USI_MAJOR, "usi");
}

module_init(usi_init);
module_exit(usi_exit);

