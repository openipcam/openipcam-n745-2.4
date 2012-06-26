/****************************************************************************
 * 
 * Copyright (c) 2004 - 2006 Winbond Electronics Corp. All rights reserved. 
 *
 ****************************************************************************/
 
/****************************************************************************
 * 
 * FILENAME
 *     w90n745_i2c.c
 *
 * VERSION
 *     1.0
 *
 * DESCRIPTION
 *	Winbond W90N710 I2C Driver
 *
 * DATA STRUCTURES
 *		None
 *
 * FUNCTIONS
 *		None
 *
 * HISTORY
 *	2005/05/20		Ver 1.0 Created by PC34 QFu
 *
 * REMARK
 *     None
 *************************************************************************/

#include <linux/module.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/ioport.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/semaphore.h>

#include "w90n745_i2c.h"


//#define I2C_DEBUG
//#define I2C_DEBUG_PRINT_LINE
//#define I2C_DEBUG_ENABLE_ENTER_LEAVE
//#define I2C_DEBUG_ENABLE_MSG
//#define I2C_DEBUG_ENABLE_MSG2

#ifdef I2C_DEBUG
#define PDEBUG(fmt, arg...)		printk(fmt, ##arg)
#else
#define PDEBUG(fmt, arg...)
#endif

#ifdef I2C_DEBUG_PRINT_LINE
#define PRN_LINE()				PDEBUG("[%-20s] : %d\n", __FUNCTION__, __LINE__)
#else
#define PRN_LINE()
#endif

#ifdef I2C_DEBUG_ENABLE_ENTER_LEAVE
#define ENTER()					PDEBUG("[%-20s] : Enter...\n", __FUNCTION__)
#define LEAVE()					PDEBUG("[%-20s] : Leave...\n", __FUNCTION__)
#else
#define ENTER()
#define LEAVE()
#endif

#ifdef I2C_DEBUG_ENABLE_MSG
#define MSG(msg)				PDEBUG("[%-20s] : %s", __FUNCTION__, msg)
#else
#define MSG(msg)
#endif

#ifdef I2C_DEBUG_ENABLE_MSG2
#define MSG2(fmt, arg...)			PDEBUG("[%-20s] : "fmt, __FUNCTION__, ##arg)
#define PRNBUF(buf, count)		{int i;MSG2("CID Data: ");for(i=0;i<count;i++)\
									PDEBUG("%02x ", buf[i]);PDEBUG("\n");}
#else
#define MSG2(fmt, arg...)
#define PRNBUF(buf, count)
#endif



#define i2c_outl(dev, dword, addr)		outl(dword, base[dev->no] + addr)
#define i2c_inl(dev, addr)				inl(base[dev->no] + addr)


static const int base[I2C_NUMBER] = {I2C0_IO_BASE, I2C1_IO_BASE};
static const int irq[I2C_NUMBER] = {I2C0_IRQ, I2C1_IRQ};
static int bNackValid;

static void i2c_Disable(i2c_dev *dev)	/* Disable i2c core and interrupt */
{

	i2c_outl(dev, 0x00, CSR);
}

static void i2c_Enable(i2c_dev *dev)	/* Enable i2c core and interrupt */
{
	i2c_outl(dev, 0x03, CSR);
}

static int i2c_SetSpeed(i2c_dev *dev, int sp)
{
	unsigned int d;

	if( sp != 100 && sp != 400)
		return -1;

	d = I2C_INPUT_CLOCK/(sp * 5) -1;

	i2c_outl(dev, d & 0xffff, DIVIDER);

	MSG2("Set Speed = %d\n", sp);

	return 0;
}

static int i2c_IsBusFree(i2c_dev *dev)
{
	if(	(i2c_inl(dev, SWR) & 0x18) == 0x18	&& 	//SDR and SCR keep high 
		(i2c_inl(dev, CSR) & 0x0400) == 0	){  	//I2C_BUSY is false
		//MSG("Bus Free\n");
		
		return 1;
	}
	else{
		//MSG("Bus Busy\n");

		return 0;
	}

}

static int i2c_Command(i2c_dev *dev, int cmd)
{

#if 0
	printk("CMD : ");

	if(cmd & I2C_CMD_START){
		printk("START ");
	}

	if (cmd & I2C_CMD_STOP){
		printk("STOP ");

	}

	if(cmd & I2C_CMD_NACK){
		printk("NACK ");
	}

	if(cmd & I2C_CMD_WRITE){
		printk("WRITE : [%02x]", i2c_inl(dev, TXR) & 0xff);
	}

	if(cmd & I2C_CMD_READ){
		printk("READ : [%02x]", i2c_inl(dev, RXR) & 0xff);
	}

	printk("\n");

#endif

	if(cmd & I2C_CMD_WRITE)
		bNackValid = 1;
	else
		bNackValid = 0;

	i2c_outl(dev, cmd, CMDR);

	return 0;

}


static int i2c_CalculateAddress(i2c_dev *dev, int mode)
{
	int i;
	unsigned int addr;
	unsigned int subaddr;

	subaddr = dev->subaddr;
	addr = dev->addr;

	addr = ((addr << 1) & 0xfe) | I2C_WRITE;

	dev->buffer[0] = addr & 0xff;

	for(i = dev->subaddr_len; i > 0; i--){
		dev->buffer[i] = subaddr & 0xff;
		subaddr >>= 8;
	}

	if(mode == I2C_STATE_READ){
		i = dev->subaddr_len + 1;
		dev->buffer[i] = (addr & 0xfe) | I2C_READ;
	}

#ifdef I2C_DEBUG
	if(mode == I2C_STATE_WRITE){
		MSG2("Begin Write to Device [%02x] Address [%02x]\n",
			dev->addr, dev->subaddr);
		MSG("Buffer : \n");
		for(i = 0; i < dev->subaddr_len +1; i++)
			PDEBUG("%02x ", dev->buffer[i]);
		PDEBUG("\n");
	}
	else{
		MSG2("Begin Read from Device [%02x] Address [%02x]\n",
			dev->addr, dev->subaddr);
		MSG("Buffer : \n");
		for(i = 0; i < dev->subaddr_len +2; i++)
			PDEBUG("%02x ", dev->buffer[i]);
		PDEBUG("\n");
	}
#endif	

	return 0;	
}

/* init i2c_dev after open */
static int i2c_ResetI2C(i2c_dev *dev)
{
	u32 reg;
	
	// configure i2c pin
	reg = inl(REG_GPIO_CFG5);
	reg &= 0x3ff00fff;
	reg |= 0x00055000;
	outl(reg, REG_GPIO_CFG5);

	dev->addr = -1;
	dev->last_error = 0;
	dev->subaddr = 0;
	dev->subaddr_len = 0;

	init_waitqueue_head(&dev->wq);

	return i2c_SetSpeed(dev, 100);
	
}

void i2c_interrupt(int irq, void *data, struct pt_regs *reg)
{
	i2c_dev *dev = (i2c_dev *)data;
	int csr, val;

	csr = i2c_inl(dev, CSR);

	//MSG2("INT : CSR : [%02x]     dev->pos = [%02x]\n", csr, dev->pos);

	csr |= 0x04;

	i2c_outl(dev, csr, CSR);

	if(dev->state == I2C_STATE_NOP)
		return;

	MSG2("bNackValid = %d\n", bNackValid);

	if((csr & 0x800) && bNackValid){	/* NACK only valid in WRITE */
		MSG("NACK Error\n");
		dev->last_error = I2C_ERR_NACK;
		i2c_Command(dev, I2C_CMD_STOP);
		goto wake_up_quit;
	}
	else if(csr & 0x200){	/* Arbitration lost */
		MSG("Arbitration lost\n");
		dev->last_error = I2C_ERR_LOSTARBITRATION;
		i2c_Command(dev, I2C_CMD_STOP);
		goto wake_up_quit;
	}
	else if(!(csr & 0x100)){		/* transmit complete */
		if(dev->pos < dev->subaddr_len + 1){	/* send address state */
			MSG("Send Address\n");
			val = dev->buffer[dev->pos++] & 0xff;
			i2c_outl(dev, val, TXR);
			i2c_Command(dev, I2C_CMD_WRITE);
		}
		else if(dev->state == I2C_STATE_READ){	

			/* sub address send over , begin restart a read command */

			if(dev->pos == dev->subaddr_len + 1){

				MSG("Restart Reading...\n");

				val = dev->buffer[dev->pos++];
				i2c_outl(dev, val, TXR);
				i2c_Command(dev, I2C_CMD_START | I2C_CMD_WRITE);
			}
			else{

				dev->buffer[dev->pos++] = i2c_inl(dev, RXR) & 0xff;

				MSG2("Read Pos : [%02x]   Data : [%02x]\n",
						dev->pos, dev->buffer[dev->pos-1]);

				if( dev->pos < dev->len){
					MSG("Read Next\n");
					if(dev->pos == dev->len -1 )	/* last character */
						i2c_Command(dev, I2C_CMD_READ |
										I2C_CMD_STOP |
										I2C_CMD_NACK);
					else
						i2c_Command(dev, I2C_CMD_READ);
				}
				else{
					MSG("Read Over \n");
					goto wake_up_quit;
				}
			}
		}
		else if(dev->state == I2C_STATE_WRITE){	/* write data */
					
			if( dev->pos < dev->len){
				MSG2("Write Pos : [%02x]   Data : [%02x]\n",
						dev->pos, dev->buffer[dev->pos]);

				val = dev->buffer[dev->pos];

				i2c_outl(dev, val, TXR);
				
				if(dev->pos == dev->len -1 )	/* last character */
					i2c_Command(dev, I2C_CMD_WRITE| I2C_CMD_STOP);
				else
					i2c_Command(dev, I2C_CMD_WRITE);

				dev->pos ++;
			}
			else{
				MSG("Write Over\n");
				goto wake_up_quit;
			}
		}
		//else if(dev->state == I2C_STATE_PROBE){
		//	MSG2("Probe Address : [%02x]\n", dev->addr);
		//	goto wake_up_quit;
		//}

	}
		
	return;

wake_up_quit:

	MSG("Wake up \n");

	dev->state = I2C_STATE_NOP;
	wake_up_interruptible(&dev->wq);

	return;
}

static int i2c_open(struct inode *inode, struct file *filp)
{
	i2c_dev *dev;

	unsigned int num = MINOR(inode->i_rdev);

	if( num >= I2C_NUMBER) {
		printk("I2C : no dev\n");
		return -ENODEV;
	}
	dev = (i2c_dev *)kmalloc(sizeof(i2c_dev), GFP_KERNEL);
	if(dev == NULL) {
		printk("no mem\n");
		return -ENOMEM;
	}
	dev->no = num;

	if(request_irq(irq[num], i2c_interrupt, SA_INTERRUPT, "i2c", dev) < 0){
		printk("I2C : Request Irq error\n");
		kfree(dev);
		return -EIO;
	}
	
	if(i2c_ResetI2C(dev)){
		free_irq(irq[num], dev);
		kfree(dev);
		printk("eio\n");
		return -EIO;
	}
	

	MOD_INC_USE_COUNT;

	filp->private_data = dev;
	
	return 0;
}

static int i2c_release(struct inode *inode, struct file *flip)
{
	i2c_dev *dev = flip->private_data;

	MSG2("Free IRQ %d\n", irq[dev->no]);

	free_irq(irq[dev->no], dev);
	kfree(dev);
	MOD_DEC_USE_COUNT;
	
	return 0;
}

static int i2c_read(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
	i2c_dev *dev = (i2c_dev *)filp->private_data;
	
	if(count == 0)
		return 0;

	if(!i2c_IsBusFree(dev))
		return -EBUSY;

	if(count > I2C_MAX_BUF_LEN - 10)
		count = I2C_MAX_BUF_LEN - 10;

	dev->state = I2C_STATE_READ;
	dev->pos = 1;
	dev->len = dev->subaddr_len + 1 + count + 2;/* plus 1 unused char */
	dev->last_error = 0;

	i2c_CalculateAddress(dev, I2C_STATE_READ);

	i2c_Enable(dev);
	
	i2c_outl(dev, dev->buffer[0] & 0xff, TXR);

	i2c_Command(dev, I2C_CMD_START | I2C_CMD_WRITE);

	wait_event_interruptible(dev->wq, dev->state == I2C_STATE_NOP);

	i2c_Disable(dev);

	if(dev->last_error)
		return -EIO;
	
	if(copy_to_user(buf, dev->buffer + dev->subaddr_len + 3, count))
		return -EFAULT;

	dev->subaddr += count;
	
	return count;
}

static int i2c_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos)
{
	i2c_dev *dev = (i2c_dev *)filp->private_data;

	
	if(count == 0)
		return 0;

	if(!i2c_IsBusFree(dev))
		return -EBUSY;


	if(count > I2C_MAX_BUF_LEN - 10)
		count = I2C_MAX_BUF_LEN - 10;

	if(copy_from_user(dev->buffer + dev->subaddr_len + 1 , buf, count))
		return -EFAULT;
	

	dev->state = I2C_STATE_WRITE;
	dev->pos = 1;
	dev->len = dev->subaddr_len + 1 + count;
	dev->last_error = 0;

	i2c_CalculateAddress(dev, I2C_STATE_WRITE);

	i2c_Enable(dev);
	
	i2c_outl(dev, dev->buffer[0] & 0xff, TXR);

	i2c_Command(dev, I2C_CMD_START | I2C_CMD_WRITE);

	wait_event_interruptible(dev->wq, dev->state == I2C_STATE_NOP);

	i2c_Disable(dev);

	if(dev->last_error)
		return -EIO;

	dev->subaddr += count;

	return count;
}

static int i2c_ioctl(struct inode *inode, struct file *flip, unsigned int cmd, unsigned long arg)
{
	int err = 0, tmp, retval = 0;
	struct sub_address sub_addr;
	i2c_dev *dev =(i2c_dev *)flip->private_data;
	
	if(_IOC_TYPE(cmd) != I2C_IOC_MAGIC) return -ENOTTY;
	if(_IOC_NR(cmd) > I2C_IOC_MAXNR) return -ENOTTY;

	if(_IOC_DIR(cmd) & _IOC_READ)
		err = !access_ok(VERIFY_WRITE, (void *)arg, _IOC_SIZE(cmd));
	else if(_IOC_DIR(cmd) & _IOC_WRITE)
		err = !access_ok(VERIFY_READ, (void *)arg, _IOC_SIZE(cmd));
	
	if(err) 
		return -EFAULT;

	switch(cmd){
		case I2C_IOC_SET_DEV_ADDRESS:
			dev->addr = arg;
			MSG2("Address : %02x\n", arg&0xff);
			break;
			
		case I2C_IOC_SET_SPEED:
			if(i2c_SetSpeed(dev, arg))
				retval = -EPERM;
			
			break;

		case I2C_IOC_GET_LAST_ERROR:
			if(put_user(dev->last_error, (int *)arg)){
				retval = -EFAULT;
				break;
			}
			
			break;

		case I2C_IOC_SET_SUB_ADDRESS:
			if(copy_from_user(&sub_addr, (void *)arg, sizeof(sub_addr))){
				retval = -EFAULT;
				break;
			}

			if(sub_addr.sub_addr_len > 4){
				retval = -EPERM;
				break;
			}

			MSG2("Sub Address = %02x, length = %d\n",
					sub_addr.sub_addr, sub_addr.sub_addr_len);

			dev->subaddr = sub_addr.sub_addr;
			dev->subaddr_len = sub_addr.sub_addr_len;
			
			break;

		default:
			retval = -ENOTTY;
			break;
	}

	return retval;
	
}


struct file_operations i2c_fops =                                                 
{
	owner: 		THIS_MODULE,
	open:		i2c_open,
	release:		i2c_release,
	ioctl:		i2c_ioctl,
	read:		i2c_read,
	write:		i2c_write,
};

static void i2c_module_exit(void)
{
	unregister_chrdev(I2C_MAJOR, "i2c");
}

static int __init i2c_module_init(void)
{
	int i, retval;

	retval = register_chrdev(I2C_MAJOR, "i2c", &i2c_fops);
	if(retval < 0)
		goto failed;

	printk("I2C Bus Driver has been installed successfully.\n");
	
	return 0;

failed:
	
	i2c_module_exit();

	printk("Init I2C Bus Driver failed!\n");

	return retval;
}

module_init(i2c_module_init);
module_exit(i2c_module_exit);
 
