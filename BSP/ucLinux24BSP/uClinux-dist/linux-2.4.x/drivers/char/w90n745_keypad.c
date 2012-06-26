/****************************************************************************
 *                                                                          *
 * Copyright (c) 2004 - 2006 Winbond Electronics Corp. All rights reserved. *
 *                                                                          *
 ****************************************************************************/
 
/****************************************************************************
 * 
 * FILENAME
 *     w90n745_keypad.c
 *
 * VERSION
 *     1.0
 *
 * DESCRIPTION
 *     The Winbond 64-keyboard driver
 *
 * DATA STRUCTURES
 *     None
 *
 * FUNCTIONS
 *     None
 *
 * HISTORY
 *     2005/09/09		 Ver 1.0 Created by PC34 YHan
 *
 * REMARK
 *     None
 *************************************************************************/

#include <linux/init.h>
#include <linux/slab.h>
#include <asm/errno.h>
#include <asm/delay.h>
//#include <asm/fcntl.h>
#include <asm/arch/irqs.h>
#include <linux/mm.h>
#include <linux/poll.h>
#include <linux/module.h>
#include <asm/hardware.h>
#include <asm/io.h>
#include "w90n745_keypad.h"

#define MAJOR_NUM 192

static int volatile kpd_get=0;
static int volatile kpd_block=1;
unsigned char DEV_NAME[10] = "Keypad";

typedef struct _keymap 
{
	short row;
	short col;	
}keymap;

keymap key __attribute__ ((aligned (4)));

static DECLARE_WAIT_QUEUE_HEAD(read_wait_a);

static void read_task_block()
{	
	DECLARE_WAITQUEUE(wait, current);
	add_wait_queue(&read_wait_a, &wait);
	set_current_state(TASK_INTERRUPTIBLE);

	schedule();
		
	set_current_state(TASK_RUNNING);
	remove_wait_queue(&read_wait_a, &wait);
		
	return ;
}

static void read_task_wake_up(void)
{
	wake_up_interruptible(&read_wait_a);
	
	return ;
}

void keypad745_irq(int irq, void *dev_id, struct pt_regs *regs)
{
	
	volatile unsigned int status;	
	kpd_get=1;
	
	key.row = 0;
	key.col = 0;
	
	status = readl(KPISTATUS);
	#ifdef KPI_DEBUG
	printk("KPI ISR KPISTATUS=0x%08x\n",status);
	#endif
    if(status&0x00210000)
	{		
	    key.row = (status&0x00000078)>>3;
	    key.col = (status&0x00000007);		    
	}
	
	if(kpd_block)
		read_task_wake_up();
	
	return;
	
}

int keypad745_open(struct inode* i,struct file* f)
{
	int result;
	int irq;
	int old_cfg;
	kpd_block=1;

	if(f->f_flags & 0x800)	//0x800:04000
		kpd_block=0;
	
	
	irq = INT_KEYPAD;
	
	MOD_INC_USE_COUNT;
		
	result = request_irq(irq,keypad745_irq,0,DEV_NAME,NULL);
	if(result!=0)
		printk("register the keypad_irq failed!\n");
	
	old_cfg=readl(GPIO_CFG);
	old_cfg=old_cfg&GPIO_CFG_MASK;
	old_cfg=old_cfg|GPIO_CFG_VALUE;
	writel(old_cfg,GPIO_CFG);
    
    #ifdef KPI_DEBUG
    printk("KPI OPEN:\nKPICONF=0x%08x\nGPIO_CFG=0x%08x\n",readl(KPICONF),readl(GPIO_CFG));
    #endif

	return 0;
}

int keypad745_close(struct inode* i,struct file* f)
{
	MOD_DEC_USE_COUNT;
	free_irq(INT_KEYPAD,NULL);
	
	return 0;
}


ssize_t keypad745_read(struct file *filp, char *buff, size_t read_mode, loff_t *offp)
{
	kpd_block = read_mode ;
	
	if(kpd_block)
	{
		read_task_block();
	}
	
	if(kpd_get)
	{
		kpd_get=0;
		copy_to_user(buff,(char*)&key,sizeof(keymap));
		key.row = 0;
		key.col = 0;
		return 1;
	}
	else
		return -1;	
}

static int keypad745_ioctl(struct inode *inode, struct file *flip, 
													 		unsigned int cmd, unsigned long arg)
{
	int err = 0;
	
	if(_IOC_TYPE(cmd) != KEYPAD_MAGIC) return -ENOTTY;
	if(_IOC_NR(cmd) > KEYPAD_MAXNR) return -ENOTTY;

	if(_IOC_DIR(cmd) & _IOC_READ)
		err = !access_ok(VERIFY_WRITE, (void *)arg, _IOC_SIZE(cmd));
	else if(_IOC_DIR(cmd) & _IOC_WRITE)
		err = !access_ok(VERIFY_READ, (void *)arg, _IOC_SIZE(cmd));
		
	if(err) 
		return -EFAULT;
	
	switch (cmd)
	{
		case KPD_BLOCK:	
			kpd_block=1;
			flip->f_flags &= ~0x800;
			break;
		case KPD_NONBLOCK:	
			kpd_block=0;
			flip->f_flags |= ~0x800;
			
			break;
		default:
			break;
	}	
	
	return 0;		
}

struct file_operations keypad745_fops = 
{
	owner: THIS_MODULE,
	open: keypad745_open,
	read: keypad745_read,
	ioctl:keypad745_ioctl,
	release: keypad745_close,
};

static int __init keypad_745_reg(void)
{		
	int result;
	int old_cfg;
	result = register_chrdev(MAJOR_NUM,DEV_NAME,&keypad745_fops);
	if(result<0)
	{
		printk("initial the device error!\n");
		return (result);	
	}	
	
	old_cfg=readl(GPIO_CFG);
	old_cfg=old_cfg&GPIO_CFG_MASK;
	old_cfg=old_cfg|GPIO_CFG_VALUE;
	writel(old_cfg,GPIO_CFG);
	
	writel(0,KPICONF);
	writel(0,KPI3KCONF);
	writel(0,KPISTATUS);	
#ifdef __WB_EVB__
	old_cfg=readl(GPIO_DIR);
	old_cfg |= 0x3FF0000;
	writel(old_cfg,GPIO_DIR);
#endif	
	writel(KPICONF_VALUE,KPICONF);	
	
	init_waitqueue_head(&read_wait_a);
	
	printk("W90N745 Keypad initialized successful\n");
	
	return (result);
	
}

static void keypad_745_exit(void)
{
	unregister_chrdev(MAJOR_NUM,DEV_NAME);
	
	return;
}

module_init(keypad_745_reg);
module_exit(keypad_745_exit);
