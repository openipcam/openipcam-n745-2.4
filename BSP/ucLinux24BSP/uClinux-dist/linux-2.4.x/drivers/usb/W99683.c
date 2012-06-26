/* This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * 
 */
/*------------------------ Now version 1.0 only support one camera-------------------------------------*/
#include <linux/config.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/vmalloc.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>
#include <linux/ctype.h>
#include <linux/pagemap.h>
#include <asm/io.h>
#include <asm/semaphore.h>
#include <asm/processor.h>
#include <linux/wrapper.h>

#if defined (__i386__)
	#include <asm/cpufeature.h>
#endif

#include "W99683.h"

/*
 * Version Information
 */
#define DRIVER_VERSION "v1.00 for Linux 2.4"
#define EMAIL "ypxin@winbond.com.tw"
#define DRIVER_AUTHOR "ypxin@winbond.com.tw"
#define DRIVER_DESC "W99683 USB Camera Driver"

static UINT8 W685ISPFW[] = {
	#include "W99685ISP.h"
};
#define ISPFW_SIZE (sizeof(W685ISPFW)/sizeof(W685ISPFW[0]))
static struct usb_driver W99683_driver;

/* table of devices that work with this driver */
static struct usb_device_id W99683_table [] = {
	{ USB_DEVICE(W99683_VENDOR, W99683i_PRODUCT) },
	{ USB_DEVICE(W99685ISP_VENDOR, W99685iISP_PRODUCT)},
	{ USB_DEVICE(W99685_VENDOR, W99685i_PRODUCT)}, 
	{},					/* Terminating entry */
};

static int video_nr = -1; 		/* next avail video device */

struct usb_W99683 *w683_debug = NULL;

static int
W99683IO(struct usb_device *dev, int Action,struct usb_args *arg, unsigned char *buf);

static int W99685_Reset2Ram(struct usb_device *dev)
{
	int result = 0;
	int action;
	struct usb_args init_args;
	
	INIT_USBARGS(&init_args, 0x16, 0x00, 0x02, 0x00);
	action = USB_WRITE;
	result = W99683IO(dev, action, &init_args, NULL);
	if(result < 0)
	{
		printk("Reset to DRAM failed\n");
		goto out;
	}
out:
	return result;
}

static int
W99685_ProgramPage(struct usb_device *dev)
{
	int result = 0;
	int action;
	struct usb_args init_args;
	
	INIT_USBARGS(&init_args, 0x11, 0x00, 0x00, 0x00);
	action = USB_WRITE;
	result = W99683IO(dev, action, &init_args, NULL);
	if(result < 0)
	{
		printk("W99685 Program Page failed\n");
		goto out;
	}
out:
	return result;
}

static int 
W99685_WriteReg(struct usb_device *dev, UINT16 addr, UINT8 *buf, UINT16 len)
{
	int result = 0;
	int action;
	struct usb_args init_args;
	
	INIT_USBARGS(&init_args, 0x01, 0x00, addr, len);
	action = USB_WRITE;
	result = W99683IO(dev, action, &init_args, buf);
	if(result < 0)
	{
		printk("Enable W99685 Write Register failed\n");
		goto out;
	}
out:
	return result;
}

static int
W99685_DOISP(struct usb_device *dev)
{
	int result = 0;
	int i, k;
	UINT8 retry = 4;
	UINT8 tmpbuf[W99685_RAM_PAGE_SIZE];
	UINT8 *firmware_datap = W685ISPFW;
	int fw_page_num = ISPFW_SIZE/W99685_RAM_PAGE_SIZE;
	printk("W99685 firmware size: %d\n", ISPFW_SIZE);
	if(ISPFW_SIZE > W99685_RAM_SIZE)
	{
		printk("W99685 firmware size beyond DRAM size\n");
		result = -EFBIG;
		goto out;
	}
	
	for(i = 0; i < fw_page_num; i++)
	{
		for(k = 0; k < W99685_RAM_PAGE_SIZE; k+=W99685_ISP_WRITE_SIZE, firmware_datap+=W99685_ISP_WRITE_SIZE)
		{
WriteReg_retry:			
			result = W99685_WriteReg(dev, W99685_ISP_BUFFER_ADDR+k, firmware_datap, W99685_ISP_WRITE_SIZE);
			if(result == -ETIMEDOUT&&retry > 0)
			{
				retry--;
				goto WriteReg_retry;
			}
			if(result < 0)
			{
				printk("Upload W99685 ISP firmware failed\n");
				goto out;
			}
		}
		result = W99685_ProgramPage(dev);
		if(result < 0)
		{
			printk("W99685 program firmware page failed\n");
			goto out;
		}
	}
	
	//handle not a multiple of 512 byte firmware data
	if((fw_page_num < W99685_RAM_PAGE_NUM) && (ISPFW_SIZE%W99685_RAM_PAGE_SIZE)) {
		memset(tmpbuf, 0xff, sizeof(tmpbuf));
		memcpy(tmpbuf, firmware_datap, ISPFW_SIZE%W99685_RAM_PAGE_SIZE);
		firmware_datap = tmpbuf;
		for(k = 0; k < W99685_RAM_PAGE_SIZE; k+=W99685_ISP_WRITE_SIZE, firmware_datap+=W99685_ISP_WRITE_SIZE)
		{
			result = W99685_WriteReg(dev, W99685_ISP_BUFFER_ADDR+k, firmware_datap, W99685_ISP_WRITE_SIZE);
			if(result < 0)
			{
				printk("Upload W99685 ISP firmware failed\n");
				goto out;
			}
		}
		result = W99685_ProgramPage(dev);
		if(result < 0)
		{
			printk("W99685 program firmware page failed\n");
			goto out;
		}
		i++;
	}
	
	memset(tmpbuf, 0xff, sizeof(tmpbuf));
	
	for(; i < W99685_RAM_PAGE_NUM; i++)
	{
		firmware_datap = tmpbuf;
		for(k = 0; k < W99685_RAM_PAGE_SIZE; k+=W99685_ISP_WRITE_SIZE, firmware_datap+=W99685_ISP_WRITE_SIZE)
		{
			result = W99685_WriteReg(dev, W99685_ISP_BUFFER_ADDR+k, firmware_datap, W99685_ISP_WRITE_SIZE);
			if(result < 0)
			{
				printk("Upload W99685 ISP firmware failed\n");
				goto out;
			}
		}
		result = W99685_ProgramPage(dev);
		if(result < 0)
		{
			printk("W99685 program firmware page failed\n");
			goto out;
		}
	}
out:
	return result;
}

static int
W99685ISP_INIT(struct usb_device *dev)
{
	int result = 0;
	int action;
	struct usb_args init_args;
	int i = 0;
	// Here, Enable W99685 ISP mode
	wait_ms(200);
	if (usb_clear_halt(dev, usb_sndbulkpipe(dev, 0))) {
		printk("Failed to reset control endpoint.\n");
	}
	INIT_USBARGS(&init_args, 0x16, 0x00, 0x01, 0x00);
	action = USB_WRITE;
	result = W99683IO(dev, action, &init_args, NULL);
	if(result < 0)
	{
		printk("Enable W99685 ISP mode failed\n");
		goto out;
	}
	
	wait_ms(200);
	result = W99685_DOISP(dev);
	if(result < 0)
	{
		printk("W99685 do ISP failed\n");
		goto out;
	}
	
	result = W99685_Reset2Ram(dev);
	if(result < 0)
	{
		printk("W99685 reset to DRAM failed\n");
		goto out;
	}
out:
	return result;
}

static int
W99683IO(struct usb_device *dev, int Action,struct usb_args *arg, unsigned char *buf) 
{
	int result = 0;
	//printk("request: %x, value: %x, index: %x, length: %x", arg->request, arg->value,\
								arg->index, arg->length);
	if(Action == USB_WRITE) {
		result = usb_control_msg(dev, usb_sndctrlpipe(dev, 0),
					 	arg->request, USB_TYPE_VENDOR|
					 	USB_RECIP_DEVICE|USB_DIR_OUT,
					 	arg->value, arg->index, buf,
					 	arg->length, HZ*5);
		//printk("  result: %d\n", result);
	}
	else
	{
		result = usb_control_msg(dev, usb_rcvctrlpipe(dev, 0),
						arg->request, USB_TYPE_VENDOR|
						USB_RECIP_DEVICE|USB_DIR_IN,
 				      		arg->value, arg->index, buf,
 				      		arg->length, HZ*5);
// 		printk("get: %d\n", result);
	}
	return result;
}

static int 
w99683_v4l1_ioctl_internal(struct video_device *vdev, unsigned int cmd,
			  void *arg)
{
	struct usb_W99683 *w683 = vdev->priv;
	int args;
	int pipe, nb, ret = 0;
	struct ctrlmsg_ioctl cmsg;
	
	PDEBUG(5, "IOCtl: 0x%X", cmd);
	//printk("IOCtl: 0x%X", cmd);
	
	if (!w683->dev)
		return -EIO;	
	memset(w683->iobuf, 0, 64*sizeof(char));
	switch (cmd) {
		case IOCTLGET:
			PDEBUG(4, "IOCTLGET");
			if(arg == NULL)
				return -EFAULT;
			if (copy_from_user(&cmsg, (void *)arg, sizeof(cmsg)))
 			return -EFAULT;

 			nb = le16_to_cpup(&cmsg.req.length);

 			if (nb > (64*sizeof(char)))
 				return -EINVAL;
 
 			ret = W99683IO(w683->dev, USB_READ, &(cmsg.req), w683->iobuf);
 			if (nb > 0 && copy_to_user(cmsg.data, w683->iobuf, nb))
				return -EFAULT;
//			printk("get2: %d\n", ret);
			return ret;
		case IOCTLSET:
			PDEBUG(4, "IOCTLSET");
			//printk("IOCTLSET");
			if (copy_from_user(&cmsg, (void *)arg, sizeof(cmsg)))
 			return -EFAULT;

 			nb = le16_to_cpup(&cmsg.req.length);

 			if (nb > (64*sizeof(char)))
 				return -EINVAL;
			if(nb == 0) {
 				ret = W99683IO(w683->dev, USB_WRITE, &(cmsg.req), NULL);
 			}
 			else if(nb > 0) {
 				if (copy_from_user(w683->iobuf, cmsg.data, nb))
 					return -EFAULT;
 				ret = W99683IO(w683->dev, USB_WRITE, &(cmsg.req), w683->iobuf);
 			}
 			else
 				return -EINVAL;
			return ret;
		case IOCTLGETCLASS:
			if(!arg)
			{
				return -EINVAL;
			}
			if(copy_to_user(arg, vdev->name, strlen(vdev->name)+1))
			{
				return -EFAULT;
			}
			return strlen(vdev->name)+1;
		default:
			PDEBUG(3, "Unsupported IOCtl: 0x%X", cmd);
			return -ENOIOCTLCMD;
	} /* end switch */

	return ret;
}


/****************************************************************************
 *
 * V4L 1 API
 *
 ***************************************************************************/
static int 
w99683_v4l1_open(struct video_device *vdev, int flags)
{
	struct usb_W99683 *w683 = vdev->priv;
	int err, i;

	PDEBUG(4, "opening");
	//printk(" debug: %x , orign: %x\n", w683_debug, w683);
	down(&w683->lock);

	err = -EBUSY;
	if (w683->user)  {
		PDEBUG(4, "erro user count: %d", w683->user);
		goto out;
	}
	
	err = -ENOMEM;
	
	/* In case app doesn't set them... */
	w683->user++;
	err = 0;
out:
	up(&w683->lock);

	return err;
}

static void 
w99683_v4l1_close(struct video_device *vdev)
{
	struct usb_W99683 *w683 = vdev->priv;

	PDEBUG(4, "W996983_close");
	
	down(&w683->lock);

	w683->user--;
	
	up(&w683->lock);

	/* Device unplugged while open. Only a minimum of unregistration is done
	 * here; the disconnect callback already did the rest. */
	if (!w683->dev) {
		video_unregister_device(&w683->vdev);
		if(w683->rawbuf) {
			kfree(w683->rawbuf);
			kfree(w683->iobuf);
			w683->rawbuf = NULL;
		}
		kfree(w683);
		w683 = NULL;
	}
}

static inline long 
w99683_v4l1_read(struct video_device *vdev, char *buf, unsigned long count,
		int noblock)
{
	struct usb_W99683 *w683 = vdev->priv;
	struct usb_device *dev = w683->dev;
	int i, rc = 0;
	int result;
	char *ibuf = w683->rawbuf;
	int partial;
	
	
	if (down_interruptible(&w683->lock))
		return -EINTR;

	PDEBUG(4, "%ld bytes, noblock=%d", count, noblock);

	if (!vdev || !buf ) {
		printk("vdev || !buf \n");
		rc = -EFAULT;
		goto error;
	}

	if (!w683->dev) {
		printk("!w683->dev\n");
		rc = -EIO;
		goto error;
	}

	if(count > W683BUF_SIZE) {
		printk("count > W683BUF_SIZE\n");
		rc = -EFAULT;
		goto error;
	}

	result = usb_bulk_msg(dev, usb_rcvbulkpipe(dev, 1), ibuf, count, &partial, HZ*10);
	
	if(result < 0) {
		printk("result < 0, %d\n", result);
		rc = result;
		goto error;
	}
	
	rc = partial;
	
	if (copy_to_user(buf, ibuf, partial)) {
			printk("copy_to_user\n");
			rc = -EFAULT;
			goto error;
	}
	up(&w683->lock);
	
	return count;

error:
	up(&w683->lock);
	return rc;
}


static long 
w99683_v4l1_write(struct video_device *vdev, const char *buf,
		 unsigned long count, int noblock)
{
	return -EINVAL;
}


static int 
w99683_v4l1_ioctl(struct video_device *vdev, unsigned int cmd, void *arg)
{
	int rc;
	struct usb_W99683 *w683 = vdev->priv;

	if (down_interruptible(&w683->lock))
		return -EINTR;
	//printk("ioctl: %x\n", cmd);
	rc = w99683_v4l1_ioctl_internal(vdev, cmd, arg);

	up(&w683->lock);
	return rc;
}

static struct video_device vdev_template = {
	owner:		THIS_MODULE,
	name:		W99685_DEVICECLASS,	//default for 685
	type:		VID_TYPE_CAPTURE,
	hardware:	VID_HARDWARE_OV511,
	open:		w99683_v4l1_open,
	close:		w99683_v4l1_close,
	read:		w99683_v4l1_read,
	write:		w99683_v4l1_write,
	ioctl:		w99683_v4l1_ioctl,
};


#if defined(CONFIG_PROC_FS) && defined(CONFIG_VIDEO_PROC_FS)

static struct proc_dir_entry *W99683_proc_entry = NULL;
extern struct proc_dir_entry *video_proc_entry;

static void 
proc_W99683_create(void)
{
	/* No current standard here. Alan prefers /proc/video/ as it keeps
	 * /proc "less cluttered than /proc/randomcardifoundintheshed/"
	 * -claudio
	 */
	if (video_proc_entry == NULL) {
		err("Error: /proc/video/ does not exist");
		return;
	}

	W99683_proc_entry = create_proc_entry("W99683", S_IFDIR,
					     video_proc_entry);

	if (W99683_proc_entry)
		W99683_proc_entry->owner = THIS_MODULE;
	else
		err("Unable to create /proc/video/W99683");
}

static void 
proc_W99683_destroy(void)
{
	PDEBUG(3, "removing /proc/video/W99683");

	if (W99683_proc_entry == NULL)
		return;

	remove_proc_entry("W99683", video_proc_entry);
}
#endif /* CONFIG_PROC_FS && CONFIG_VIDEO_PROC_FS */

/****************************************************************************
 *
 *  USB routines
 *
 ***************************************************************************/
static void *
W99683_probe(struct usb_device *dev, unsigned int ifnum,
	    const struct usb_device_id *id)
{
	struct usb_interface_descriptor *interface;
	struct usb_endpoint_descriptor *endpoint;
	struct usb_W99683 *w683;
	int i;
	int registered = 0;
	UINT8 mul = 1;
	
	PDEBUG(1, "probing for device...");
	printk("probing for device..., %d\n", ifnum);
	
	/* We don't handle multi-config cameras */
	if (dev->descriptor.bNumConfigurations != 1)
		return NULL;

	interface = &dev->actconfig->interface[ifnum].altsetting[0];
	endpoint = interface[ifnum].endpoint;
	
	/*printk("endponts: %d\n", interface->bNumEndpoints);
	for(i = 0; i < interface->bNumEndpoints; i++)
	{
		if(IS_EP_BULK_IN(endpoint[i]))
			printk("get bulk in: %d\n", i);
	}*/
	/* Checking vendor/product should be enough, but what the hell */
	//printk("bInterfaceClass: %x, bInterfaceSubClass: %x\n", interface->bInterfaceClass, interface->bInterfaceSubClass);
	if (interface->bInterfaceClass != 0x00)
		return NULL;
	if (interface->bInterfaceSubClass != 0x00)
		return NULL;

	/* Since code below may sleep, we use this as a lock */
	MOD_INC_USE_COUNT;

	if ((w683 = kmalloc(sizeof(*w683), GFP_KERNEL)) == NULL) {
		err("couldn't kmalloc ov struct");
		goto error_out;
	}

	w683_debug = w683;
	memset(w683, 0, sizeof(*w683));
	
	//printk("probe: user: %d\n", w683->user);
	w683->dev = dev;
	w683->iface = interface->bInterfaceNumber;
	w683->user = 0;
	//printk("probe: user: %d\n", w683->user);
	printk("probe: vendorID: %x, ProductID: %x\n", dev->descriptor.idVendor, dev->descriptor.idProduct);
#if 0
	if ((dev->descriptor.idVendor != W99683_VENDOR) ||
	    (dev->descriptor.idProduct != W99683i_PRODUCT)) {
		err("Unknown product ID 0x%x or Vendor ID 0x%x", dev->descriptor.idProduct, dev->descriptor.idVendor);
		goto error_dealloc;
	}
#endif
	if ((dev->descriptor.idVendor == W99683_VENDOR) &&
	    (dev->descriptor.idProduct == W99683i_PRODUCT)) {
	    	strcpy(vdev_template.name, W99683_DEVICECLASS);
	    	printk("Find W99683 USB Camera\n");
			goto init;
	}
	else if ((dev->descriptor.idVendor == W99685ISP_VENDOR) &&
	    (dev->descriptor.idProduct == W99685iISP_PRODUCT)) {
	    	printk("Find W99685 USB ISP\n");
	    	//usb_show_device(dev);
	    	if(!strcmp(vdev_template.name, W99685_DEVICECLASS)) {
	    		if(W99685ISP_INIT(dev) < 0)
	    			goto error_dealloc;
	    	}
	    	else {
			strcpy(vdev_template.name, W9968x_ISPMODE);
		}   		
	}
	else if ((dev->descriptor.idVendor == W99685_VENDOR) &&
	    (dev->descriptor.idProduct == W99685i_PRODUCT)) {
	    	mul = 2;
	    	strcpy(vdev_template.name, W99685_DEVICECLASS);
	    	goto init;
	}
	else {
		err("Unknown product ID 0x%x or Vendor ID 0x%x", dev->descriptor.idProduct, dev->descriptor.idVendor);
		goto error_dealloc;
	}
init:
	init_waitqueue_head(&w683->wq);

	init_MUTEX(&w683->param_lock);
	init_MUTEX(&w683->lock);
	printk("video name: %s\n", vdev_template.name);
	memcpy(&w683->vdev, &vdev_template, sizeof(vdev_template));
	w683->vdev.priv = w683;

	/* Use the next available one */
	if (video_register_device(&w683->vdev, VFL_TYPE_GRABBER, video_nr) < 0) {
		err("video_register_device failed");
		goto error;
	}

	info("Device registered on minor %d", w683->vdev.minor);
	
	w683->rawbuf = kmalloc(W683BUF_SIZE*mul, GFP_KERNEL);
	if (!w683->rawbuf)
		goto error;

	w683->rawbuf = (char *)((unsigned long)w683->rawbuf|0x80000000);

	w683->iobuf = kmalloc(64*sizeof(unsigned char), GFP_KERNEL);
	if (!w683->iobuf)
		goto error1;

	w683->iobuf = (unsigned char *)((unsigned long)w683->iobuf|0x80000000);	


	MOD_DEC_USE_COUNT;
     	return w683;
error1:
	kfree(w683->rawbuf);
error:
	err("Camera initialization failed");

#if defined(CONFIG_PROC_FS) && defined(CONFIG_VIDEO_PROC_FS)
	/* Safe to call even if entry doesn't exist */
#endif

	usb_driver_release_interface(&W99683_driver,
		&dev->actconfig->interface[w683->iface]);

error_dealloc:
	if (w683) {
		kfree(w683);
		w683 = NULL;
	}

error_out:
	MOD_DEC_USE_COUNT;
	return NULL;
}


static void
W99683_disconnect(struct usb_device *dev, void *ptr)
{
	struct usb_W99683 *w683 = (struct usb_W99683 *) ptr;
	int n;

	MOD_INC_USE_COUNT;

	printk("W99683 is disconnected\n");

	/* We don't want people trying to open up the device */
#if 1
	if (!w683->user)
		video_unregister_device(&w683->vdev);
	else {
		//PDEBUG(3, "Device open...deferring video_unregister_device");
		printk("Device open...deferring video_unregister_device\n");
		//for later application to close ??
	}
#else
	video_unregister_device(&w683->vdev);
#endif

	if (waitqueue_active(&w683->wq))
		wake_up_interruptible(&w683->wq);

#if defined(CONFIG_PROC_FS) && defined(CONFIG_VIDEO_PROC_FS)
 //       destroy_proc_ov511_cam(w683);
#endif

	usb_driver_release_interface(&W99683_driver,
		&w683->dev->actconfig->interface[w683->iface]);
	w683->dev = NULL;

	/* Free the memory */
	if(w683 && !w683->user) {
		if(w683->rawbuf) {
			kfree(w683->rawbuf);
			kfree(w683->iobuf);
			w683->rawbuf = NULL;
		}
		kfree(w683);
		w683 = NULL;
	}

	MOD_DEC_USE_COUNT;
}

static struct usb_driver W99683_driver = {
	name:		"W99683",
	id_table:       W99683_table,
	probe:		W99683_probe,
	disconnect:	W99683_disconnect
};

/******************************************************************************
 *
 *  Module Routines
 *
 ******************************************************************************/
static int __init 
usb_W99683_init(void)
{
#if defined(CONFIG_PROC_FS) && defined(CONFIG_VIDEO_PROC_FS)
        proc_W99683_create();
#endif
	//printk("init start user: %x\n", &W99683_driver);
	if(&W99683_driver == NULL)
		return -1;
//	printk("init start user: %d\n", w683_debug->user);
	if (usb_register(&W99683_driver) < 0) {
		printk("Can't register W99682 driver\n");
		return -1;
	}
	// FIXME: Don't know how to determine this yet
//	ov51x_mmx_available = 0;

#if defined (__i386__)
//	if (test_bit(X86_FEATURE_MMX, &boot_cpu_data.x86_capability))
//		ov51x_mmx_available = 1;
#endif
	info(DRIVER_VERSION " : " DRIVER_DESC);
	return 0;
}

static void __exit 
usb_W99683_exit(void)
{
	usb_deregister(&W99683_driver);
	info("driver deregistered");

#if defined(CONFIG_PROC_FS) && defined(CONFIG_VIDEO_PROC_FS)
        proc_W99683_destroy();
#endif
}

module_init(usb_W99683_init);
module_exit(usb_W99683_exit);
