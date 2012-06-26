#ifndef __LINUX_W99683_H
#define __LINUX_W99683_H

#include <linux/videodev.h>
#include <linux/smp_lock.h>
#include <linux/usb.h>


//#define W99683_DEBUG	/* Turn on debug messages */

#ifdef W99683_DEBUG
	#define PDEBUG(level, fmt, args...) \
		info("[" __PRETTY_FUNCTION__ ":%d] " fmt,\
		__LINE__ , ## args)
#else
	#define PDEBUG(level, fmt, args...) do {} while(0)
#endif

/* some type defines */
#define UINT16 	unsigned short
#define INT16 	short
#define UINT8 	unsigned char
#define UINT32 	unsigned int

/*vendor ID and product ID */
#define W99683_VENDOR 0x416
#define W99683i_PRODUCT 0x6830
#define W99685ISP_VENDOR 0x416
#define W99685iISP_PRODUCT 0x9680
#define W99685_VENDOR	0x416
#define W99685i_PRODUCT	0x6850

#define W99683_DEVICECLASS "W99683usb"
#define W99685_DEVICECLASS "W99685usb"
#define W9968x_ISPMODE "W9968xisp"

#define W99685_RAM_SIZE		16*1024
#define W99685_RAM_PAGE_SIZE	512
#define W99685_RAM_PAGE_NUM	32
#define W99685_ISP_BUFFER_ADDR 0xa000
#define W99685_ISP_WRITE_SIZE	64

//USB direction
#define USB_DIR_OUT	0
#define USB_DIR_IN	0x80

//define Read and Write
#define USB_READ 0
#define USB_WRITE 1

#define IS_EP_BULK(ep)  ((ep).bmAttributes == USB_ENDPOINT_XFER_BULK ? 1 : 0)
#define IS_EP_BULK_IN(ep) (IS_EP_BULK(ep) && ((ep).bEndpointAddress & USB_ENDPOINT_DIR_MASK) == USB_DIR_IN)

//define resolution
#define R1280X1024 	0x80		//No support currently
#define R1280X960 	0x40		//No support currently
#define R640X480 		0x20
#define R352X288 		0x10
#define R320X240 		0x08
#define R176X144 		0x04
#define R160X120 		0x02		//No support currently
#define R128X96 		0x01		//No support currently
		

/*--------------------------------------- For the New W99683 ----------------------------------------*/
typedef struct _VENDOR_COMMAND {
	unsigned char rBits;
	unsigned char bRequest;
	unsigned short iValue;
	unsigned short iIndex;
	unsigned short DataLen;
}VENDOR_COMMAND, *PVENDOR_COMMAND;

#define REG_READ     0x00
#define REG_WRITE    0x01
#define I2C_READ     0x02
#define I2C_WRITE    0x03
#define MEM_READ     0x04
#define MEM_WRITE    0x05
#define GET_VERSION  0x10
#define ENABLE_ISP   0x11
#define BULK_SETTING 0x13

#define	FAIL_FUNCTION_CALL		1

#define ENGCLKCR        0x03
#define ENGOPCR			0x04
#define SystemCR		0x16
#define DisplayOutputCR	0x9f
#define TVEncoderCR		0x2d0
// For DSP engine
#define DSPFunctionCR   0x101
#define DSPCropCR1      0x11c
#define DSPCropCR2      0x11d
#define DSPCropCR3      0x11e
#define DSPCropCR4		0x11f
#define DSPCropCR5      0x120  
#define DSPCropCR6      0x121 
#define DSPCropCR7      0x122  
#define DSPCropCR8 		0x123
#define DSPVideoQuaCR1  0x135
#define DSPVideoQuaCR2  0x136
#define DSPVideoQuaCR3  0x137
#define DSPVideoQuaCR4  0x138
#define DSPVideoQuaCR5  0x139
#define DSPVideoQuaCR6  0x13a
#define DSPVideoQuaCR7	0x13b	

// For VPRE engine
#define PEConfig1		0x200
#define PEConfig2		0x201
#define CapYUVXScalM    0x220
#define CapYUVXScalN    0x221
#define CapYUVYScalM    0x222
#define CapYUVYScalN	0x223

// For JPEG engine
#define JPEGModeCR      0x280
#define JPEGHeaderCR	0x281
#define JPEGPriScalUpCR 0x282
#define JPEGPriHeightL  0x28B
#define JPEGPriHeightH  0x28C
#define JPEGPriWidthL   0x28D
#define JPEGPriWidthH	0x28E
#define JPEGPriRestartL 0x293
#define JPEGPriRestartH 0x294
#define JPEGYStrideL    0x2ac
#define JPEGYStrideH    0x2ad
#define JPEGUStrideL    0x2ae
#define JPEGUStrideH    0x2af
#define JPEGVStrideL    0x2b0
#define JPEGVStrideH    0x2b1

#define JPEGLumQtblReg  0x300
#define JPEGChroQtblReg 0x340

#define W683BUF_SIZE 32*1024	//same to the application


//W99683 struct 
struct usb_W99683 {
	struct video_device vdev;

	/* Device structure */
	struct usb_device *dev;

	int customid;
	int desc;
	unsigned char iface;

	/* Determined by sensor type */
	int maxwidth;
	int maxheight;
	int minwidth;
	int minheight;

	int brightness;
	int colour;
	int contrast;
	int hue;
	int whiteness;
	int exposure;
	int auto_brt;		/* Auto brightness enabled flag */
	int auto_gain;		/* Auto gain control enabled flag */
	int auto_exp;		/* Auto exposure enabled flag */
	int backlight;		/* Backlight exposure algorithm flag */

	struct semaphore lock;	/* Serializes user-accessible operations */
	int user;		/* user count for exclusive use */
	char *rawbuf;
	char *iobuf;
	wait_queue_head_t wq;	/* Processes waiting */

	int snap_enabled;	/* Snapshot mode enabled */
	
	struct semaphore param_lock;	/* params lock for this camera */

	/* /proc entries, relative to /proc/video/ov511/ */
	struct proc_dir_entry *proc_devdir;   /* Per-device proc directory */
	struct proc_dir_entry *proc_info;     /* <minor#>/info entry */
	struct proc_dir_entry *proc_button;   /* <minor#>/button entry */
	struct proc_dir_entry *proc_control;  /* <minor#>/control entry */
};

struct usb_args {
	__u8 request;
	__u16 value;
	__u16 index;
	__u16 length;
};

struct ctrlmsg_ioctl {
	struct usb_args	req;
 	void*	 data;
};

#define INIT_USBARGS(argp, req, val, ind, len) \
do {\
	(argp)->request = req;\
	(argp)->value = val;\
	(argp)->index = ind;\
	(argp)->length = len;\
}while(0)
#define IOCTLGET		_IOR('v',1,struct ctrlmsg_ioctl)	/* Get capabilities */
#define IOCTLSET		_IOR('v',2,struct ctrlmsg_ioctl)	/* Set capabilities */
#define IOCTLGETCLASS		_IOR('v',3,UINT32)			/* Get Device Class */
#endif

/*
bus/device  idVendor/idProduct
001/001     0000/0000
  wTotalLength:         25
  bNumInterfaces:       1
  bConfigurationValue:  1
  iConfiguration:       0
  bmAttributes:         40h
  MaxPower:             0
    bInterfaceNumber:   0
    bAlternateSetting:  0
    bNumEndpoints:      1
    bInterfaceClass:    9
    bInterfaceSubClass: 0
    bInterfaceProtocol: 0
    iInterface:         0
      bEndpointAddress: 81h
      bmAttributes:     03h
      wMaxPacketSize:   8
      bInterval:        255
      bRefresh:         0
      bSynchAddress:    0
001/006     0416/6830
  wTotalLength:         103
  bNumInterfaces:       1
  bConfigurationValue:  1
  iConfiguration:       0
  bmAttributes:         80h
  MaxPower:             250
    bInterfaceNumber:   0
    bAlternateSetting:  0
    bNumEndpoints:      3
    bInterfaceClass:    0
    bInterfaceSubClass: 0
    bInterfaceProtocol: 0
    iInterface:         0
      bEndpointAddress: 81h
      bmAttributes:     02h
      wMaxPacketSize:   64
      bInterval:        1
      bRefresh:         0
      bSynchAddress:    0
      bEndpointAddress: 02h
      bmAttributes:     02h
      wMaxPacketSize:   64
      bInterval:        1
      bRefresh:         0
      bSynchAddress:    0
      bEndpointAddress: 83h
      bmAttributes:     03h
      wMaxPacketSize:   8
      bInterval:        1
      bRefresh:         0
      bSynchAddress:    0
*/
