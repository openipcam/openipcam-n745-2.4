/****************************************************************************
 *                                                                          *
 * Copyright (c) 2005 - 2007 Winbond Electronics Corp. All rights reserved. *
 *                                                                          *
 ****************************************************************************/
 
/****************************************************************************
 * 
 * FILENAME
 *     w90n745_vcom .h
 *
 * VERSION
 *     1.0
 *
 * DESCRIPTION
 * 
 *
 * DATA STRUCTURES
 *     None
 *
 * FUNCTIONS
 *     None
 *
 * HISTORY
 *     2005/08/01		 Ver 1.0 Created by PC34 Lsshi
 *
 *
 * REMARK
 *     None
 *************************************************************************/
 
#ifndef __W90N745_VCOM_H
#define __W90N745_VCOM_H
//types

#define CONST             const

#define FALSE             0
#define TRUE              1

typedef void              VOID;
typedef void *            PVOID;

typedef char              BOOL;
typedef char *            PBOOL;

typedef char              INT8;
typedef char              CHAR;
typedef char *            PINT8;
typedef char *            PCHAR;
typedef unsigned char     UINT8;
typedef unsigned char     UCHAR;
typedef unsigned char *   PUINT8;
typedef unsigned char *   PUCHAR;
typedef char *            PSTR;
typedef const char *      PCSTR;

typedef short             SHORT;
typedef short *           PSHORT;
typedef unsigned short    USHORT;
typedef unsigned short *  PUSHORT;

typedef short             INT16;
typedef short *           PINT16;
typedef unsigned short    UINT16;
typedef unsigned short *  PUINT16;

typedef int               INT;
typedef int *             PINT;
typedef unsigned int      UINT;
typedef unsigned int *    PUINT;

typedef int               INT32;
typedef int *             PINT32;
typedef unsigned int      UINT32;
typedef unsigned int *    PUINT32;


typedef float             FLOAT;
typedef float *           PFLOAT;

typedef double            DOUBLE;
typedef double *          PDOUBLE;

typedef int               SIZE_T;

typedef unsigned char     REG8;
typedef unsigned short    REG16;
typedef unsigned int      REG32;


#define outpb(port,value)     (*((UINT8 volatile *) (port))=value)
#define inpb(port)            (*((UINT8 volatile *) (port)))
#define outphw(port,value)    (*((UINT16 volatile *) (port))=value)
#define inphw(port)           (*((UINT16 volatile *) (port)))
#define outpw(port,value)     (*((UINT32 volatile *) (port))=value)
#define inpw(port)            (*((UINT32 volatile *) (port)))


#define USB_WRITE(addr,data)	(*((unsigned int volatile *)(addr))=data)
#define USB_READ(addr)	(*((unsigned int volatile *)(addr)))
///
#define ENDPOINTS	2 //bulkin/out
#define USB_OFFSET	0xB4

#define RSTI		1
#define SUSI		2
#define RUMI		3
#define GDEVI		5
#define GCFGI		6
#define GSTRI		7
#define CLAI		8
#define VENI		9
#define CDOI		10
#define CDII		11
#define RST_ENDI	15
#define RST_ENDI	15

/**********************************************************************************************************
 *                                                               
 * USB Device Control Registers  
 *
 **********************************************************************************************************/
#define W90N745_USB_DEVICE       0xfff06000
#define REG_USB_CTL	  		(W90N745_USB_DEVICE+0x00)    /* USB control register */
#define REG_USB_CVCMD		(W90N745_USB_DEVICE+0x04)    /* USB class or vendor command register */
#define REG_USB_IE	  		(W90N745_USB_DEVICE+0x08)    /* USB interrupt enable register */
#define REG_USB_IS	  		(W90N745_USB_DEVICE+0x0c)    /* USB interrupt status register */
#define REG_USB_IC	  		(W90N745_USB_DEVICE+0x10)    /* USB interrupt status clear register */
#define REG_USB_IFSTR		(W90N745_USB_DEVICE+0x14)    /* USB interface and string register */
#define REG_USB_ODATA0		(W90N745_USB_DEVICE+0x18)    /* USB control transfer-out port 0 register */
#define REG_USB_ODATA1		(W90N745_USB_DEVICE+0x1C)    /* USB control transfer-out port 1 register */
#define REG_USB_ODATA2		(W90N745_USB_DEVICE+0x20)    /* USB control transfer-out port 2 register */
#define REG_USB_ODATA3		(W90N745_USB_DEVICE+0x24)    /* USB control transfer-out port 3 register */
#define REG_USB_IDATA0		(W90N745_USB_DEVICE+0x28)    /* USB control transfer-in data port 0 register */
#define REG_USB_IDATA1		(W90N745_USB_DEVICE+0x2C)    /* USB control transfer-in data port 1 register */
#define REG_USB_IDATA2		(W90N745_USB_DEVICE+0x30)    /* USB control transfer-in data port 2 register */
#define REG_USB_IDATA3		(W90N745_USB_DEVICE+0x34)    /* USB control transfer-in data port 2 register */
#define REG_USB_SIE			(W90N745_USB_DEVICE+0x38)    /* USB SIE status Register */
#define REG_USB_ENG			(W90N745_USB_DEVICE+0x3c)    /* USB Engine Register */
#define REG_USB_CTLS		(W90N745_USB_DEVICE+0x40)    /* USB control transfer status register */
#define REG_USB_CONFD		(W90N745_USB_DEVICE+0x44)    /* USB Configured Value register */
#define REG_USB_EPA_INFO	(W90N745_USB_DEVICE+0x48)    /* USB endpoint A information register */
#define REG_USB_EPA_CTL		(W90N745_USB_DEVICE+0x4c)    /* USB endpoint A control register */
#define REG_USB_EPA_IE		(W90N745_USB_DEVICE+0x50)    /* USB endpoint A Interrupt Enable register */
#define REG_USB_EPA_IC		(W90N745_USB_DEVICE+0x54)    /* USB endpoint A interrupt clear register */
#define REG_USB_EPA_IS		(W90N745_USB_DEVICE+0x58)    /* USB endpoint A interrupt status register */
#define REG_USB_EPA_ADDR	(W90N745_USB_DEVICE+0x5c)    /* USB endpoint A address register */
#define REG_USB_EPA_LENTH	(W90N745_USB_DEVICE+0x60)    /* USB endpoint A transfer length register */
#define REG_USB_EPB_INFO	(W90N745_USB_DEVICE+0x64)    /* USB endpoint B information register */
#define REG_USB_EPB_CTL		(W90N745_USB_DEVICE+0x68)    /* USB endpoint B control register */
#define REG_USB_EPB_IE		(W90N745_USB_DEVICE+0x6c)    /* USB endpoint B Interrupt Enable register */
#define REG_USB_EPB_IC		(W90N745_USB_DEVICE+0x70)    /* USB endpoint B interrupt clear register */
#define REG_USB_EPB_IS		(W90N745_USB_DEVICE+0x74)    /* USB endpoint B interrupt status register */
#define REG_USB_EPB_ADDR	(W90N745_USB_DEVICE+0x78)    /* USB endpoint B address register */
#define REG_USB_EPB_LENTH	(W90N745_USB_DEVICE+0x7c)    /* USB endpoint B transfer length register */
#define REG_USB_EPC_INFO	(W90N745_USB_DEVICE+0x80)    /* USB endpoint C information register */
#define REG_USB_EPC_CTL		(W90N745_USB_DEVICE+0x84)    /* USB endpoint C control register */
#define REG_USB_EPC_IE		(W90N745_USB_DEVICE+0x88)    /* USB endpoint C Interrupt Enable register */
#define REG_USB_EPC_IC		(W90N745_USB_DEVICE+0x8c)    /* USB endpoint C interrupt clear register */
#define REG_USB_EPC_IS		(W90N745_USB_DEVICE+0x90)    /* USB endpoint C interrupt status register */
#define REG_USB_EPC_ADDR	(W90N745_USB_DEVICE+0x94)    /* USB endpoint C address register */
#define REG_USB_EPC_LENTH	(W90N745_USB_DEVICE+0x98)    /* USB endpoint C transfer length register */
#define REG_USB_EPA_XFER	(W90N745_USB_DEVICE+0x9c)    /* USB endpoint A remain transfer length register */
#define REG_USB_EPA_PKT		(W90N745_USB_DEVICE+0xa0)    /* USB endpoint A remain packet length register */
#define REG_USB_EPB_XFER	(W90N745_USB_DEVICE+0xa4)    /* USB endpoint B remain transfer length register */
#define REG_USB_EPB_PKT		(W90N745_USB_DEVICE+0xa8)    /* USB endpoint B remain packet length register */
#define REG_USB_EPC_XFER	(W90N745_USB_DEVICE+0xac)    /* USB endpoint C remain transfer length register */
#define REG_USB_EPC_PKT		(W90N745_USB_DEVICE+0xb0)    /* USB endpoint C remain packet length register */

/////////////////////////////////////////////////////////////////////////////////////////////////////////

#define SizePerSector 512

#define RAMDISK


//extern UINT8 volatile USB_Power_Flag; // 0: bus power de-attached; 1: attached

//bulk set
//extern UINT32 volatile Bulk_set_length;
//extern UINT8 volatile bulksetflag;
/* Define Endpoint feature */
#define Ep_In        0x01
#define Ep_Out       0x00
#define Ep_Bulk      0x01
#define Ep_Int       0x02
#define Ep_Iso       0x03

#define EP_A         0x00
#define EP_B         0x01



typedef struct 
{
    UINT32 dwAddr;
    UINT32 dwValue;
    
}USB_INIT_T;

typedef struct 
{
 UINT8 Req_Type;
 UINT8 Req;
 UINT16 Value;
 UINT16 Index;
 UINT16 Length;
}USB_Vender_Cmd_Format_T __attribute__ ((aligned (4)));   // each field of vendor command

typedef struct {
    UINT8 EP_Num;
    UINT8 EP_Dir;
    UINT8 EP_Type;
    UINT8 p;
} USB_EP_Inf_T __attribute__ ((aligned (4)));

/////

//WBUSB Structs
 typedef struct _wbusb_dev {

  	UINT8				epnum;
  	UINT8				usb_online;
  	UINT8				bulkonlycmd;
  	UINT32				bulk_len;
  	
  	wait_queue_head_t	 wusbd_wait_a,wusbd_wait_b,wusbd_wait_c;
  	
 	USB_EP_Inf_T			ep[2];
 	USB_Vender_Cmd_Format_T	vcmd ;
  
 	enum{
 		GET_DEV=1,
 		GET_CFG,
 		GET_STR,
 		CLA_CMDIN,
 		VEN_CMDIN,
 		VEN_CMDOUT
 	}usb_enumstatus;
 
	int (*wait_cbw)(struct _wbusb_dev *dev,void* cbw);
 	void (*rw_data)(struct _wbusb_dev *dev,UINT8* buf,UINT32 length);
 	void (*rd_data)(struct _wbusb_dev *dev,UINT8* buf,UINT32 length);
 	
}wbusb_dev __attribute__ ((aligned (4)));
/////

#define WBUSB_MAJOR	250

#define WBUSB_IOC_MAXNR	3

#define WBUSB_IOC_MAGIC 'u'

#define WBUSB_IOC_GETCBW _IOR(WBUSB_IOC_MAGIC, 0, char *)
#define WBUSB_GETVLEN	 _IOR(WBUSB_IOC_MAGIC, 1, unsigned long *)
#define WBUSB_REPLUG	 _IOR(WBUSB_IOC_MAGIC, 2, char *)

#define INT_USBD   22

#endif /* __WBUSB_H */
