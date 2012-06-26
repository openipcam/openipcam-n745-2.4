/****************************************************************************
 *																			*
 * Copyright (c) 2005 -	2007 Winbond Electronics Corp. All rights reserved.	*
 *																			*
 ****************************************************************************/

/****************************************************************************
 *
 * FILENAME
 *	   w90n745_vcom.c
 *
 * VERSION
 *	   1.0
 *
 * DESCRIPTION
 *	   Winbond W90N745 USBD VCOM driver
 *
 * DATA	STRUCTURES
 *	   None
 *
 * FUNCTIONS
 *	   None
 *
 * HISTORY
 *	   2005/08/01		 Ver 1.0
 *	   2005/08/16		 Ver 1.1
 *
 * AUTHOR
 *	  PC34 Lsshi
 *
 * REMARK
 *	   None
 *************************************************************************/

#include <linux/config.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/module.h>
//#include <linux/moduleparam.h>
#include <linux/spinlock.h>

#include <linux/fs.h>
//#include <linux/cdev.h>

#include <asm/uaccess.h>
#include <asm/arch/hardware.h>
#include <asm/io.h>
#include <linux/usb.h>

#include "w90n745_vcom.h"
//#define printk(fmt, ...)
#define	DBGOUT(fmt,	arg...) //printk(fmt, ##arg)

//#define printk(fmt...)

/* Configure USBD as VCOM in the beginning */
#define USBDVCOM  /* 2007.11.29 add, PT23 HHWu */

//#define SD_DEBUG
//#define SD_DEBUG_PRINT_LINE
#define SD_DEBUG_ENABLE_ENTER_LEAVE
//#define SD_DEBUG_ENABLE_MSG
//#define SD_DEBUG_ENABLE_MSG2

#ifdef SD_DEBUG
#define PDEBUG(fmt, arg...)		printk(fmt, ##arg)
#else
#define PDEBUG(fmt, arg...)
#endif

#ifdef SD_DEBUG_PRINT_LINE
#define PRN_LINE()				PDEBUG("[%-20s] : %d\n", __FUNCTION__, __LINE__)
#else
#define PRN_LINE()
#endif

#ifdef SD_DEBUG_ENABLE_ENTER_LEAVE
#define ENTER()					PDEBUG("[%-20s] : Enter...\n", __FUNCTION__)
#define LEAVE()					PDEBUG("[%-20s] : Leave...\n", __FUNCTION__)
#else
#define ENTER()
#define LEAVE()
#endif

#ifdef SD_DEBUG_ENABLE_MSG
#define MSG(msg)				PDEBUG("[%-20s] : %s\n", __FUNCTION__, msg)
#else
#define MSG(msg)
#endif

#ifdef SD_DEBUG_ENABLE_MSG2
#define MSG2(fmt, arg...)			PDEBUG("[%-20s] : "fmt, __FUNCTION__, ##arg)
#define PRNBUF(buf, count)		{int i;MSG2("CID Data: ");for(i=0;i<count;i++)\
									PDEBUG("%02x ", buf[i]);PDEBUG("\n");}
#else
#define MSG2(fmt, arg...)
#define PRNBUF(buf, count)
#endif




static const char driver_name [] = "Winbond	W90N745	USB Device";


static int USB_Device_Mass[5] = {
0x01100112,	0x10000000,	0x96850416,	0x02010100,	0x00000100 };

static int USB_Device_VCOM[5] = {
0x01100112, 0x10000000, 0x70210416/*pid & vid*/, 0x02010100, 0x00000100 };

////////////////////////////////////////////////////////////////////////////

static int USB_Config_Mass[10] =	{
0x00270209,	0x40000101,	0x00040932,	0x05080300 , 0x05070050, 0x00400281,
0x02050701,	0x01004002,	0x03830507,	0x00010040 };

static int USB_Config_VCOM[8] = {
0x00200209, 0x40000101, 0x00040932, 0x00000200 , 0x05070000, 0x00400281,
0x02050701, 0x01004002 };

////////////////////////////////////////////////////////////////////////////

static int USB_Lang_Id_String[1] = {
0x04090304 };

////////////////////////////////////////////////////////////////////////////

static int USB_Vender_Id_String_Mass[4] = {
0x00570310,	0x004e0049,	0x004f0042,	0x0044004e };

static int USB_Vender_Id_String_VCOM[6] = {
0x00550316, 0x00420053, 0x00440020, 0x00760065, 0x00630069 ,0x00000065};

////////////////////////////////////////////////////////////////////////////

static int USB_Device_Id_String_Mass[5] = {
0x00570312,	0x00390039,	0x00380036,	0x00430033,	0x00000046 };

static int USB_Device_Id_String_VCOM[12] = {
0x0057032E, 0x00390039, 0x00300037, 0x00200032, 0x00530055, 0x00200042, 0x00690056,
0x00740072, 0x00610075, 0x0020006C, 0x004F0043, 0x0000004D};

////////////////////////////////////////////////////////////////////////////
static unsigned int* USB_Device;
static unsigned int* USB_Config;
static unsigned int* USB_Device_Id_String;
static unsigned int* USB_Vender_Id_String;

static USB_INIT_T USBInit[6] = {
	{0x00,0x000000e4}, //USB Control register
	{0x08,0x000047f7}, //USB Interrupt Enable register
	{0x14,0x00000030}, //USB Interface and string register
	{0x38,0x00000000}, //USB SIE Status	Register
	{0x44,0x00000001}, //USB Configured	Value register
	{0x48,0x00000000}  //USB Endpointer	A Information register
};

static int bCBlockFlag = 0;


static void	USB_Init(wbusb_dev*	dev);
static void	USB_Irq_Init(wbusb_dev*	dev);
static void	SDRAM2USB_Bulk(wbusb_dev* dev,UINT8* buf ,UINT32 Tran_Size);
static void	USB2SDRAM_Bulk(wbusb_dev* dev,UINT8* buf ,UINT32 Tran_Size);
static void	SDRAM_USB_Transfer(wbusb_dev *dev,UINT8	epname,UINT8* buf ,UINT32 Tran_Size);
static UINT8 USB_Setup_Endpoint(wbusb_dev *dev,UINT8 epname,UINT8 epnum,UINT8 epdir,UINT8 eptype,
						 UINT8 epcon,UINT8 epinf,UINT16	epmaxps	);


void write_data(wbusb_dev *dev,UINT8* buf,UINT32 length);
void read_data(wbusb_dev *dev,UINT8* buf,UINT32	length);
int	check_cbw(wbusb_dev	*dev,void* cbw);

static void	start_write(wbusb_dev *dev,UINT8* buf,UINT32 length);
static void	start_read(wbusb_dev *dev,UINT8* buf,UINT32	length);

static void	A_task_wake_up(wbusb_dev *dev);//write task

static void	B_task_block(wbusb_dev *dev);//idle	and	read task
static void	B_task_wake_up(wbusb_dev *dev);

static void	C_task_block(wbusb_dev *dev);//vcom vendor
static void	C_task_wake_up(wbusb_dev *dev);

#if 0
static void	dump_buffer(void *buf, int count)
{
	int	i;

	printk("USB	: ");

	for(i=0; i < count;	i ++)
		printk("%02x ",	*((unsigned	char *)buf + i));
	printk("\n");

}
#endif

void __usleep(int count)
{
	int i=0;
	
	for(i=0;i<count;i++);
	
}

static void	start_write(wbusb_dev *dev,UINT8* buf,UINT32 length)
{

	DECLARE_WAITQUEUE(wait,	current);
	add_wait_queue(&dev->wusbd_wait_a, &wait);
	set_current_state(TASK_INTERRUPTIBLE);

	if(USB_READ(REG_USB_EPA_CTL)&0x00000004)
	{
		printk("device -> host DMA busy	...\n");
		goto quit;
	}

	USB_WRITE(REG_USB_EPA_ADDR,(UINT32)buf);//Tell DMA the memory address
	USB_WRITE(REG_USB_EPA_LENTH,length);

	USB_WRITE(REG_USB_EPA_CTL,USB_READ(REG_USB_EPA_CTL)|0x00000004);//The memory is	ready for endpoint A to	access

	schedule();

quit:
	set_current_state(TASK_RUNNING);
	remove_wait_queue(&dev->wusbd_wait_a, &wait);

	return ;
}

static void	start_read(wbusb_dev *dev,UINT8* buf,UINT32	length)
{

	DECLARE_WAITQUEUE(wait,	current);
	add_wait_queue(&dev->wusbd_wait_b, &wait);
	set_current_state(TASK_INTERRUPTIBLE);

	if(USB_READ(REG_USB_EPB_CTL)&0x00000004)
	{
		printk("host ->	device DMA busy	...\n");
		goto quit;
	}
	USB_WRITE(REG_USB_EPB_ADDR,(UINT32)buf);//Tell DMA the memory address
	USB_WRITE(REG_USB_EPB_LENTH,length);

	USB_WRITE(REG_USB_EPB_CTL,USB_READ(REG_USB_EPB_CTL)|0x00000004);//The memory is	ready for endpoint A to	access

	schedule();

quit:
	set_current_state(TASK_RUNNING);
	remove_wait_queue(&dev->wusbd_wait_b, &wait);

	return ;
}

#if 0
static void	A_task_block(wbusb_dev *dev)
{


	DECLARE_WAITQUEUE(wait,	current);
	add_wait_queue(&dev->wusbd_wait_a, &wait);
	set_current_state(TASK_INTERRUPTIBLE);

	schedule();

	set_current_state(TASK_RUNNING);
	remove_wait_queue(&dev->wusbd_wait_a, &wait);

	return ;
}

#endif

static void	A_task_wake_up(wbusb_dev *dev)
{

	wake_up_interruptible(&dev->wusbd_wait_a);

	return ;
}


static void	B_task_block(wbusb_dev *dev)
{


	DECLARE_WAITQUEUE(wait,	current);
	add_wait_queue(&dev->wusbd_wait_b, &wait);
	set_current_state(TASK_INTERRUPTIBLE);

	schedule();

	set_current_state(TASK_RUNNING);
	remove_wait_queue(&dev->wusbd_wait_b, &wait);

	return ;
}

static void	B_task_wake_up(wbusb_dev *dev)
{

	wake_up_interruptible(&dev->wusbd_wait_b);

	return ;
}


static void	C_task_block(wbusb_dev *dev)
{

	ENTER();
#if 0
	DECLARE_WAITQUEUE(wait,	current);
	add_wait_queue(&dev->wusbd_wait_c, &wait);
	set_current_state(TASK_INTERRUPTIBLE);

	schedule();

	set_current_state(TASK_RUNNING);
	remove_wait_queue(&dev->wusbd_wait_c, &wait);

#else

	bCBlockFlag = 0;
  wait_event_interruptible(dev->wusbd_wait_c, bCBlockFlag != 0);
#endif

	LEAVE();

	return ;
}

static void	C_task_wake_up(wbusb_dev *dev)
{
	ENTER();

#if 1
	bCBlockFlag = 1;
	wake_up_interruptible(&dev->wusbd_wait_c);
	
	//printk("bCBlockFlag:%x\n",bCBlockFlag);

#endif
	LEAVE();

	return ;
}


static UINT8 USB_Setup_Endpoint(wbusb_dev *dev,UINT8 epname,UINT8 epnum,UINT8 epdir,UINT8 eptype,
						 UINT8 epcon,UINT8 epinf,UINT16	epmaxps	)
{
	UINT32 tmp;

	if (epname == EP_A)
	{
		if ((epdir==Ep_In)&&(eptype==Ep_Bulk)&&(epcon==1))
			USB_WRITE(REG_USB_EPA_INFO,0x30000010);
		else if	((epdir==Ep_In)&&(eptype==Ep_Int)&&(epcon==1))
			USB_WRITE(REG_USB_EPA_INFO,0x50000010);
		else if	((epdir==Ep_In)&&(eptype==Ep_Iso)&&(epcon==1))
			USB_WRITE(REG_USB_EPA_INFO,0x70000010);
		else if	((epdir==Ep_Out)&&(eptype==Ep_Bulk)&&(epcon==1))
			USB_WRITE(REG_USB_EPA_INFO,0x20000010);
		else
			 return	0;
		tmp	= epinf;
		tmp	= tmp << 8;
		USB_WRITE(REG_USB_EPA_INFO,(USB_READ(REG_USB_EPA_INFO)|tmp));
		tmp	= epmaxps;
		tmp	= tmp << 16;
		USB_WRITE(REG_USB_EPA_INFO,(USB_READ(REG_USB_EPA_INFO)|tmp));
		USB_WRITE(REG_USB_EPA_INFO,(USB_READ(REG_USB_EPA_INFO)|(epnum&0x0f)));

		dev->ep[EP_A].EP_Num = epnum;
		dev->ep[EP_A].EP_Dir = epdir;
		dev->ep[EP_A].EP_Type =	eptype;

		USB_WRITE(REG_USB_EPA_CTL,0x00000001);//enable endpoint	A
	}
	else if	(epname	== EP_B)
	{
		if ((epdir==Ep_In)&&(eptype==Ep_Bulk)&&(epcon==1))
			USB_WRITE(REG_USB_EPB_INFO,0x30000010);
		else if	((epdir==Ep_In)&&(eptype==Ep_Int)&&(epcon==1))
			USB_WRITE(REG_USB_EPB_INFO,0x50000010);
		else if	((epdir==Ep_In)&&(eptype==Ep_Iso)&&(epcon==1))
			USB_WRITE(REG_USB_EPB_INFO,0x70000010);
		else if	((epdir==Ep_Out)&&(eptype==Ep_Bulk)&&(epcon==1))
			USB_WRITE(REG_USB_EPB_INFO,0x20000010);
		else
			 return	0;

		tmp	= epinf;
		tmp	= tmp << 8;
		USB_WRITE(REG_USB_EPB_INFO,(USB_READ(REG_USB_EPB_INFO)|tmp));
		tmp	= epmaxps;
		tmp	= tmp << 16;
		USB_WRITE(REG_USB_EPB_INFO,(USB_READ(REG_USB_EPB_INFO)|tmp));
		USB_WRITE(REG_USB_EPB_INFO,(USB_READ(REG_USB_EPB_INFO)|(epnum&0x0f)));

		dev->ep[EP_B].EP_Num = epnum;
		dev->ep[EP_B].EP_Dir = epdir;
		dev->ep[EP_B].EP_Type =	eptype;
		USB_WRITE(REG_USB_EPB_CTL,0x00000001);//enable endpoint	B
	}


	return 0;
}

static void	USB_Cmd_Parsing(wbusb_dev *dev)
{
	 UINT32	cmd;

	 cmd=USB_READ(W90N745_USB_DEVICE+0x18);
	 dev->vcmd.Req_Type	= cmd&0xff;
	 dev->vcmd.Req	= (cmd>>8)&0xff;

	 dev->vcmd.Value = (cmd>>16)&0xffff;//2,3
	 cmd=USB_READ(W90N745_USB_DEVICE+0x1c);
	 dev->vcmd.Index = cmd&0xffff;//4,5

	 dev->vcmd.Length =	(cmd>>16)&0xffff;//6,7

	 if	(dev->usb_enumstatus==GET_STR)
		 dev->vcmd.Index = 0;
}

static void	USB_All_Flag_Clear(wbusb_dev *dev)
{

	DBGOUT("%s %d\n",__FUNCTION__,__LINE__);
	dev->usb_enumstatus=0;
}



static void	Get_Dev_Dpr_In(wbusb_dev *dev)
{
	 UINT8 i;
	 
	 DBGOUT("%s	%d\n",__FUNCTION__,__LINE__);

	 if	(dev->vcmd.Length >	0)
	 {
		 if	(dev->vcmd.Length <= 16)
		 {
			 USB_WRITE(REG_USB_CVCMD,dev->vcmd.Length);
		 }
		 else
		 {
			dev->vcmd.Length =	dev->vcmd.Length - 16;
			 USB_WRITE(REG_USB_CVCMD,0x10);//return	data packet	length is 16
		 }
		 for (i	= 0	; i	< USB_READ(REG_USB_CVCMD) ;	i=i+4)
		 {

			 switch(dev->usb_enumstatus)
			 {

				case GET_DEV:
					USB_WRITE(REG_USB_IDATA0+i,USB_Device[dev->vcmd.Index]);
				break;

				case GET_CFG:
					USB_WRITE(REG_USB_IDATA0+i,USB_Config[dev->vcmd.Index]);
				break;

				case GET_STR:

					if ((USB_READ(REG_USB_ODATA0)&0x00ff0000) == 0x00000000)
					{
						USB_WRITE(REG_USB_IDATA0+i,USB_Lang_Id_String[dev->vcmd.Index]);
					}
					if ((USB_READ(REG_USB_ODATA0)&0x00ff0000) == 0x00010000)
					{
						USB_WRITE(REG_USB_IDATA0+i,USB_Vender_Id_String[dev->vcmd.Index]);
					}
					if ((USB_READ(REG_USB_ODATA0)&0x00ff0000) == 0x00020000)
					{
						USB_WRITE(REG_USB_IDATA0+i,USB_Device_Id_String[dev->vcmd.Index]);
					}

				break;

				default:
					printk("%s %d\n",__FUNCTION__,__LINE__);
				break;

			}//switch end
			dev->vcmd.Index	= dev->vcmd.Index +	1;
		 }//for	end
	 }
	 else
		USB_WRITE(REG_USB_CVCMD,0x00);

}

int Disable_USB(wbusb_dev *dev)
{
	ENTER();

  USB_WRITE(REG_USB_CTL,0x0);//disable usb enginee
  Disable_Int(INT_USBD);
  free_irq(INT_USBD,dev);
  
  __usleep(1000);

  LEAVE();
  
  return 0;
	
}



int Enable_USB(wbusb_dev *dev)
{
	ENTER();

	USB_Init(dev);
  USB_Irq_Init(dev);
  __usleep(1000);

	LEAVE();
  
  	return 0;	
}

void Reset_USB(wbusb_dev *dev)
{
	ENTER();

	printk("Disable USB now ...\n");
	Disable_USB(dev);
	__usleep(1000000);
	printk("Enable USB now ...\n");
	Enable_USB(dev);
	
	LEAVE();
}

static void	USB_Init(wbusb_dev	*dev)
{
	int	j;

	ENTER();

	for	(j=0 ; j<6 ; j++)
	{
		USB_WRITE(W90N745_USB_DEVICE+USBInit[j].dwAddr,USBInit[j].dwValue);
	}

	dev->epnum=ENDPOINTS;

	for	(j=0 ; j<dev->epnum	; j++)
	{
		dev->ep[j].EP_Num =	0xff;
		dev->ep[j].EP_Dir =	0xff;
		dev->ep[j].EP_Type = 0xff;

	}
	
	dev->bulk_len=0;

	USB_Setup_Endpoint(dev,EP_A,1,Ep_In,Ep_Bulk,1,0,64);//Endpointer A bulkin //device -> host
	USB_Setup_Endpoint(dev,EP_B,2,Ep_Out,Ep_Bulk,1,0,64);//Endpointer B	bulkout	//host -> device


	USB_WRITE(REG_USB_EPA_IE,(USB_READ(REG_USB_EPA_IE)|0x00000008));//enable DMA interrupt
	USB_WRITE(REG_USB_EPB_IE,(USB_READ(REG_USB_EPB_IE)|0x00000008));//enable DMA interrupt

	USB_WRITE(REG_USB_CTL,(USB_READ(REG_USB_CTL)|0x00000001));//D+ high

	LEAVE();

}


static void	USB_ISR_Reset_Start(void)
{
	DBGOUT("%s %d\n",__FUNCTION__,__LINE__);
}

static void	USB_ISR_Reset_End(wbusb_dev	*dev)
{
	if ((USB_READ(REG_USB_EPA_CTL)&0x00000004)==0x00000004)
	{
		USB_WRITE(REG_USB_EPA_CTL,USB_READ(REG_USB_EPA_CTL)&0xfffffffb);//clear dma
		USB_WRITE(REG_USB_EPA_CTL,USB_READ(REG_USB_EPA_CTL)|0x00000004);
	}
	if ((USB_READ(REG_USB_EPB_CTL)&0x00000004)==0x00000004)
	{
		USB_WRITE(REG_USB_EPB_CTL,USB_READ(REG_USB_EPB_CTL)&0xfffffffb);//clear dma
		USB_WRITE(REG_USB_EPB_CTL,USB_READ(REG_USB_EPB_CTL)|0x00000004);
	}
}

static void	USB_ISR_Suspend(wbusb_dev *dev)
{

	if(dev->usb_online)
	{
		USB_All_Flag_Clear(dev);

		USB_WRITE(REG_USB_EPA_CTL,USB_READ(REG_USB_EPA_CTL)&0xfffffffb);//clear	dma	busy bit
		USB_WRITE(REG_USB_EPB_CTL,USB_READ(REG_USB_EPB_CTL)&0xfffffffb);


		dev->usb_online=0;

		A_task_wake_up(dev);
		B_task_wake_up(dev);

		printk("USB	plugout	!\n");
	}

}

static void	USB_ISR_Resume(void)
{
	DBGOUT("%s %d\n",__FUNCTION__,__LINE__);
}


void USB_ISR_Vendor_Cmd(wbusb_dev* dev)
{
	
	USB_All_Flag_Clear(dev);
	
	
	
     if ((USB_READ(REG_USB_CTLS)&0x0000001f) == 0x00000008)
     {
         USB_Cmd_Parsing(dev);
         
         if (dev->vcmd.Req_Type == 0x40)
         {
             dev->bulk_len=0;
             dev->usb_enumstatus=VEN_CMDOUT;
             
             
             
             if (dev->vcmd.Req == 0xA0)
             {
                 if (dev->vcmd.Value == 0x12)
                 {
					//VCOM PC driver Vendor CMD
					
                    dev->bulk_len=dev->vcmd.Index;
                    USB_WRITE(REG_USB_IE,USB_READ(REG_USB_IE)&0xfffffeff);//disable vender interrupt enable
                    C_task_wake_up(dev);
                 }
                 else if (dev->vcmd.Value == 0x13)
                 {
                     	 
                 	 USB_WRITE(REG_USB_IE,USB_READ(REG_USB_IE)|0x00000100);//enable vender interrupt enable
              
                    if ((USB_READ(REG_USB_EPB_CTL)&0x00000004)==0x00000004)
                    {
                        USB_WRITE(REG_USB_EPB_CTL,USB_READ(REG_USB_EPB_CTL)|0x00000002);//reset
                        USB_WRITE(REG_USB_EPB_CTL,USB_READ(REG_USB_EPB_CTL)&0xfffffffd);
                    }
                    if ((USB_READ(REG_USB_EPB_CTL)&0x00000004)==0x00000004)
                        USB_WRITE(REG_USB_EPB_CTL,USB_READ(REG_USB_EPB_CTL)&0xfffffffb);//clear ready
                 }
             }
         }
         else if (dev->vcmd.Req_Type == 0xc0)
             dev->usb_enumstatus = VEN_CMDIN;
         else
         	USB_WRITE(REG_USB_ENG,0x00000002);
 
     }    
     else {
        USB_WRITE(REG_USB_ENG,0x00000002);
     }
     USB_WRITE(REG_USB_ENG,0x00000008);
     USB_WRITE(REG_USB_IC,0x00000100);
}

static void	USB_ISR_Dev_Des(wbusb_dev* dev)
{
	 USB_All_Flag_Clear(dev);

	 dev->usb_enumstatus=GET_DEV;

	if(!dev->usb_online){

		dev->usb_online=1;

		A_task_wake_up(dev);
		B_task_wake_up(dev);

		printk("USB plugin !\n");
	}

	 if	((USB_READ(REG_USB_CTLS)&0x0000001f) ==	0x00000008)//check usb control transfer	status register
	 {
		 USB_Cmd_Parsing(dev);//read setup data
		 if	(dev->vcmd.Length <=(USB_Device[0]&0x000000ff))
			 USB_WRITE(REG_USB_ENG,0x00000008);//ACK
		 else
		 {
			 dev->vcmd.Length =	(USB_Device[0]&0x000000ff);
			 USB_WRITE(REG_USB_ENG,0x00000008);
		 }
	 }
	 else {
		USB_WRITE(REG_USB_ENG,0x00000002);//return stall
	 }

}

static void	USB_ISR_Conf_Des(wbusb_dev*	dev)
{

	 USB_All_Flag_Clear(dev);
	 dev->usb_enumstatus=GET_CFG;

	 if	((USB_READ(REG_USB_CTLS)&0x0000001f) ==	0x00000008)
	 {
		 USB_Cmd_Parsing(dev);
		 if	(dev->vcmd.Length <= ((USB_Config[0]&0x00ff0000)>>16))//configure total	length
			 USB_WRITE(REG_USB_ENG,0x00000008);
		 else
		 {
			 dev->vcmd.Length =	((USB_Config[0]&0x00ff0000)>>16);
			 USB_WRITE(REG_USB_ENG,0x00000008);
		 }
	 }
	 else {
		USB_WRITE(REG_USB_ENG,0x00000002);
	 }
}

static void	USB_ISR_Str_Des(wbusb_dev* dev)
{

	 USB_All_Flag_Clear(dev);
	 dev->usb_enumstatus=GET_STR;

	 if	((USB_READ(REG_USB_CTLS)&0x0000001f) ==	0x00000008)
	 {
		 USB_Cmd_Parsing(dev);
		 if	((USB_READ(REG_USB_ODATA0)&0x00ff0000) == 0x00000000)
		 {
			 if	(dev->vcmd.Length <= (USB_Lang_Id_String[0]&0x000000ff))
				 USB_WRITE(REG_USB_ENG,0x00000008);
			 else
			 {
				 dev->vcmd.Length =	(USB_Lang_Id_String[0]&0x000000ff);
				 USB_WRITE(REG_USB_ENG,0x00000008);
			 }
		 }
		 if	((USB_READ(REG_USB_ODATA0)&0x00ff0000) == 0x00010000)
		 {
			 if	(dev->vcmd.Length <= (USB_Vender_Id_String[0]&0x000000ff))
				 USB_WRITE(REG_USB_ENG,0x00000008);
			 else
			 {
				 dev->vcmd.Length =	(USB_Vender_Id_String[0]&0x000000ff);
				 USB_WRITE(REG_USB_ENG,0x00000008);
			 }
		 }
		 if	((USB_READ(REG_USB_ODATA0)&0x00ff0000) == 0x00020000)
		 {
			 if	(dev->vcmd.Length <= (USB_Device_Id_String[0]&0x000000ff))
				 USB_WRITE(REG_USB_ENG,0x00000008);
			 else
			 {
				 dev->vcmd.Length =	(USB_Device_Id_String[0]&0x000000ff);
				 USB_WRITE(REG_USB_ENG,0x00000008);
			 }
		 }
	 }
	 else {
		USB_WRITE(REG_USB_ENG,0x00000002);
	 }

}

void Vender_Data_Out(wbusb_dev* dev)
{
	
	 if ((USB_READ(REG_USB_CTLS)&0x0000001f) != 0)
     {
     	
     	 dev->vcmd.Index = dev->vcmd.Index + (USB_READ(REG_USB_CTLS)&0x0000001f);
         dev->vcmd.Length = dev->vcmd.Length - (USB_READ(REG_USB_CTLS)&0x0000001f);; 
     
    }
	
}



static void	USB_ISR_CtlOut(wbusb_dev* dev)
{
	 
	 if(dev->usb_enumstatus==VEN_CMDOUT)
	 {
	 	
	 	Vender_Data_Out(dev);
	}
	
	 USB_WRITE(REG_USB_ENG,0x00000002);
	 USB_WRITE(REG_USB_ENG,0x00000008);

}


void Outp_Byte(UINT32 addr,UINT8 value)
{
	UINT32 tmp1,tmp2;

	tmp1=USB_READ(addr&0xfffffffc);
	tmp2=(UINT32)(value);
	if (addr%4==0)
	{
		tmp1=tmp1&0xffffff00;
		tmp1=tmp1|tmp2;
	}
	else if	(addr%4==1)
	{
		tmp1=tmp1&0xffff00ff;
		tmp1=tmp1|(tmp2<<8);
	}
	else if	(addr%4==2)
	{
		tmp1=tmp1&0xff00ffff;
		tmp1=tmp1|(tmp2<<16);
	}
	else if	(addr%4==3)
	{
		tmp1=tmp1&0x00ffffff;
		tmp1=tmp1|(tmp2<<24);
	}
	USB_WRITE((addr&0xfffffffc),tmp1);
}

void Class_Data_In(void)
{

	 USB_WRITE(REG_USB_CVCMD,0x01);
	 Outp_Byte(REG_USB_IDATA0,0x00);
}


void Vender_Data_In(wbusb_dev *dev)
{
     //UINT8 i;
    // UINT32 tmp;
    
    

     if (dev->vcmd.Length > 0)
     {
     	 
     	 
     	 if (dev->vcmd.Length <= 16)
     	 {
     	     USB_WRITE(REG_USB_CVCMD,dev->vcmd.Length);     	     
     	 }
     	 else
     	 {
     	     dev->vcmd.Length = dev->vcmd.Length - 16;
     	     USB_WRITE(REG_USB_CVCMD,0x10);
     	 }
     	 
         if (dev->vcmd.Req == 0x00)     	 
         {
             printk("%s %d\n",__FUNCTION__,__LINE__);
     	     
         }else if (dev->vcmd.Req == 0x10)
         {
             printk("%s %d\n",__FUNCTION__,__LINE__);
         }else if (dev->vcmd.Req == 0x04)
         {
             printk("%s %d\n",__FUNCTION__,__LINE__);
         }
     }
     else
         USB_WRITE(REG_USB_CVCMD,0x00); 
}


void USB_ISR_Class_Cmd(wbusb_dev *dev)
{
	 USB_All_Flag_Clear(dev);

	 if(USB_READ(W90N745_USB_DEVICE+0x18)==0xfea1)
	 {
		 dev->usb_enumstatus=CLA_CMDIN;
	 }
	 USB_WRITE(REG_USB_ENG,0x00000008);

}

static void	USB_ISR_CtlIn(wbusb_dev	*dev)
{

	switch(dev->usb_enumstatus)
	{
		case CLA_CMDIN:
			Class_Data_In();
		break;
		
		case VEN_CMDIN:
			Vender_Data_In(dev);
		break;

		case GET_DEV:
		case GET_CFG:
		case GET_STR:
			Get_Dev_Dpr_In(dev);
		break;

		default:
			USB_WRITE(REG_USB_ENG,0x00000002);
		break;

	}

	 USB_WRITE(REG_USB_ENG,0x00000001);//ACK for DATA-IN is	Ready

}

static void	USB_ISR_EpA_DMA_Complete(wbusb_dev	*dev)
{

	A_task_wake_up(dev);

	USB_WRITE(REG_USB_EPA_IC,0x00000008);

}


static void	USB_ISR_EpB_DMA_Complete(wbusb_dev *dev)
{
	if ((USB_READ(REG_USB_EPB_LENTH)-USB_READ(REG_USB_EPB_XFER))==0x0000001f)
		dev->bulkonlycmd=1;
		
	if (dev->bulk_len!=0)
    {
         dev->bulk_len=0;
         USB_WRITE(REG_USB_IE,USB_READ(REG_USB_IE)|0x00000100);//enable vender interrupt enable
    }

	B_task_wake_up(dev);

	USB_WRITE(REG_USB_EPB_IC,0x00000008);


}



static void	SDRAM_USB_Transfer(wbusb_dev *dev,UINT8	epname,UINT8* buf ,UINT32 Tran_Size)
{

	if (epname == EP_A)
	{
		start_write(dev,buf,Tran_Size);
	}
	else if	(epname	== EP_B)
	{
		start_read(dev,buf,Tran_Size);
	}

	return;

}

static void	SDRAM2USB_Bulk(wbusb_dev *dev,UINT8* buf,UINT32	Tran_Size)
{

	if ((dev->ep[EP_A].EP_Dir==Ep_In)&&(dev->ep[EP_A].EP_Type==Ep_Bulk))
		SDRAM_USB_Transfer(dev,EP_A,buf,Tran_Size);
	else if	((dev->ep[EP_B].EP_Dir==Ep_In)&&(dev->ep[EP_B].EP_Type==Ep_Bulk))
		SDRAM_USB_Transfer(dev,EP_B,buf,Tran_Size);

	return;

}

static void	USB2SDRAM_Bulk(wbusb_dev *dev, UINT8* buf ,UINT32 Tran_Size)
{

	if ((dev->ep[EP_A].EP_Dir==Ep_Out)&&(dev->ep[EP_A].EP_Type==Ep_Bulk))
		SDRAM_USB_Transfer(dev,EP_A,buf,Tran_Size);
	else if	((dev->ep[EP_B].EP_Dir==Ep_Out)&&(dev->ep[EP_B].EP_Type==Ep_Bulk))
		SDRAM_USB_Transfer(dev,EP_B,buf,Tran_Size);

	return;

}

void write_data(wbusb_dev *dev,UINT8* buf,UINT32 length)
{

	if(!dev->usb_online)
		return;
	SDRAM2USB_Bulk(dev,	buf,length);
}

void read_data(wbusb_dev *dev,UINT8* buf,UINT32	length)
{

	if(!dev->usb_online)
		return;
	USB2SDRAM_Bulk(dev,buf,length);
}


void paser_irq(int irq,wbusb_dev *dev)
{

	switch(irq)
	{
		case RSTI:
			USB_ISR_Reset_Start();
		break;

		case SUSI:
			USB_ISR_Suspend(dev);
		break;

		case RUMI:
			USB_ISR_Resume();
		break;

		case GDEVI:
			USB_ISR_Dev_Des(dev);//Get device Descriptor();
		break;

		case GCFGI:
			USB_ISR_Conf_Des(dev);
		break;

		case GSTRI:
			USB_ISR_Str_Des(dev);
		break;

		case CLAI:
			USB_ISR_Class_Cmd(dev);
		break;
		
		case VENI:
        	USB_ISR_Vendor_Cmd(dev);

		case CDOI:
			USB_ISR_CtlOut(dev);
		break;

		case CDII:
			USB_ISR_CtlIn(dev);
		break;

		case RST_ENDI:
			USB_ISR_Reset_End(dev);
		break;
		
		

		default:
			//printk("irq: %d not handled !\n",irq);
		break;

	}

	USB_WRITE(REG_USB_IC,(1<<(irq-1)));//clear irq bit

	return ;

}


static void wbusbd_irq(int irq, void *usbdev, struct pt_regs *r)
{
	UINT32 id,dmaid,i;
	wbusb_dev *dev;

	dev=(wbusb_dev *)(usbdev);

	dmaid =	USB_READ(REG_USB_EPA_IS);

	if (dmaid&0x00000008)
	{
		USB_ISR_EpA_DMA_Complete(dev);
	}

	dmaid =	USB_READ(REG_USB_EPB_IS);

	if (dmaid&0x00000008)
	{
		USB_ISR_EpB_DMA_Complete(dev);
	}

	id = USB_READ(REG_USB_IS);

	for(i=0;i<16;i++)
	{
		if(id&(1<<i))
			paser_irq(i+1,dev);
	}

	return;


}

int	check_cbw(wbusb_dev	*dev,void* cbw)
{

	if(!dev->usb_online)
	{
		B_task_block(dev);
		return -1;
	}

	USB2SDRAM_Bulk(dev,cbw,0x1f);

	if (dev->bulkonlycmd)
	{
		dev->bulkonlycmd=0;//If	DMA	complete and got 31	bytes command, this	will be	"1"

		//dump_buffer(cbw, 0x1f);

		return 0;
	}

   return -1;

}

static int wbusb_installirq(wbusb_dev *dev)
{

	if (request_irq(INT_USBD, &wbusbd_irq, SA_INTERRUPT/*|SA_SAMPLE_RANDOM*/,
			driver_name, dev) != 0)	{
		printk("Request irq error\n");
		return -EBUSY;
	}

	Enable_Int(INT_USBD);

	return 0;

}


static void	USB_Irq_Init(wbusb_dev	*dev)
{

	ENTER();

	init_waitqueue_head(&dev->wusbd_wait_a);//write
	init_waitqueue_head(&dev->wusbd_wait_b);//read
	init_waitqueue_head(&dev->wusbd_wait_c);//vcom vendor cmd

	if(wbusb_installirq(dev)==0)
		printk("install	usb	device irq ok\n");

	USB_WRITE(REG_USB_EPA_IE,(USB_READ(REG_USB_EPA_IE)|0x00000008));//enable DMA interrupt
	USB_WRITE(REG_USB_EPB_IE,(USB_READ(REG_USB_EPB_IE)|0x00000008));//enable DMA interrupt


	LEAVE();
	
	return ;
}

 wbusb_dev *the_controller=NULL;

 void* wbusb_register_driver(void)
 {
	if(the_controller)
		return the_controller;
	else
		return NULL;
 }


void wbusbd_probe(void)
{
	wbusb_dev	*dev = 0;

	ENTER();
	/* alloc, and start	init */
	dev	= kmalloc (sizeof *dev,	GFP_KERNEL);
	if (dev	== NULL){
		DBGOUT("kmalloc	error !\n");
		goto done;
	}

	memset(dev,	0, sizeof *dev);

	dev->rw_data=write_data;
	dev->rd_data=read_data;
	dev->wait_cbw=check_cbw;

	USB_Init(dev);

	USB_Irq_Init(dev);

	the_controller=dev;

	LEAVE();

done:

	return ;

}

static char buffer[4096];

static size_t usb_client_read(char /*__user*/ *buf, size_t length)
{
	char *tmp = (char *)((UINT32)buffer | 0x80000000);

	ENTER();
	//printk("Buffer : %08x,   Count : %08x\n", buf, length);
	if(length == 0)
		return 0;

	if(length > 4096)
		length = 4096;

	read_data(the_controller, tmp, length);

	if(copy_to_user(buf, tmp, length))
		return -EFAULT;

	LEAVE();
	return length;
}

static size_t usb_client_write(const char/* __user*/ *buf, size_t length)
{
	char *tmp = (char *)((UINT32)buffer | 0x80000000);

	ENTER();
	//printk("Buffer : %08x,   Count : %08x\n", buf, length);

	if(length == 0)
		return 0;

	if(length > 4096)
		length = 4096;

	if(copy_from_user(tmp, buf, length))
		return -EFAULT;

	write_data(the_controller, tmp, length);

	LEAVE();

	return length;
}

static int usb_client_get_cbw(char *buf)
{
	int retval = 0;
	char *tmp = (char *)((UINT32)buffer | 0x80000000);

	ENTER();

	retval = check_cbw(the_controller, tmp);

	if(copy_to_user(buf, tmp, 0x1f))
                return -EFAULT;

	LEAVE();

	return 0;
}

static int wbusb_open(struct inode *ip, struct file *filp)
{
	ENTER();
	LEAVE();
	return 0;
}

static int wbusb_release(struct inode *ip, struct file *filp)
{
	ENTER();
	LEAVE();
	return 0;
}

static ssize_t wbusb_read(struct file *filp, char /*__user*/ * buf, size_t count, loff_t *lp)
{
	return usb_client_read(buf, count);
}

static ssize_t wbusb_write(struct file *filp, const char /*__user*/ * buf, size_t count, loff_t *lp)
{
	return usb_client_write(buf, count);
}

static int wbusb_ioctl(struct inode *ip, struct file *filp, unsigned int cmd, unsigned long arg)
{
	int err = 0;
	int retval = 0;

	ENTER();

	if (_IOC_TYPE(cmd) != WBUSB_IOC_MAGIC) return -ENOTTY;
	if (_IOC_NR(cmd) > WBUSB_IOC_MAXNR) return -ENOTTY;


	if (_IOC_DIR(cmd) & _IOC_READ)
	  err = !access_ok(VERIFY_WRITE, (void /*__user*/ *)arg, _IOC_SIZE(cmd));
	else if (_IOC_DIR(cmd) & _IOC_WRITE)
	  err = !access_ok(VERIFY_READ, (void /*__user*/ *)arg, _IOC_SIZE(cmd));

	if (err) return -EFAULT;
	
	
	switch(cmd) {
		case WBUSB_IOC_GETCBW:
			retval = usb_client_get_cbw((char *)arg);
			break;
			
		case WBUSB_GETVLEN:
			
			if(!the_controller->bulk_len)
			{
				//printk("waiting PC\n");
				//C_task_block(the_controller);  /* 11.28 mark, PT23 HHWu */
			}
			
			//printk("wakeup now !\n");
			
			*(unsigned long*)arg=the_controller->bulk_len;
			break;
			
		case WBUSB_REPLUG:
#if 0
			if(*(char*)arg)
			{			
#endif
				printk("VCOM Type !!!\n");
				USB_Device=USB_Device_VCOM;
				USB_Config=USB_Config_VCOM;
				USB_Device_Id_String=USB_Device_Id_String_VCOM;
				USB_Vender_Id_String=USB_Vender_Id_String_VCOM;		
#if 0
			}
			else
			{
				printk("MASS Type !!!\n");
				USB_Device=USB_Device_Mass;
				USB_Config=USB_Config_Mass;
				USB_Device_Id_String=USB_Device_Id_String_Mass;
				USB_Vender_Id_String=USB_Vender_Id_String_Mass;
			}
#endif				
			memset(the_controller,0, sizeof *the_controller);
			the_controller->rw_data=write_data;
			the_controller->rd_data=read_data;
			the_controller->wait_cbw=check_cbw;
			Reset_USB(the_controller);
			
					
			break;
			
		default:
			return -ENOTTY;
	}

	LEAVE();

	return retval;

}

//struct cdev wbusb_cdev;

struct file_operations wbusb_fops = {
	.owner	= THIS_MODULE,
	.open	= wbusb_open,
	.release	= wbusb_release,
	.read 	= wbusb_read,
	.write	= wbusb_write,
	.ioctl 	= wbusb_ioctl,
};

static int __init wbusb_init (void)
{
	int retval;

	ENTER();
	
#ifdef USBDVCOM
	printk("VCOM Type!\n");
	USB_Device=USB_Device_VCOM;
	USB_Config=USB_Config_VCOM;
	USB_Device_Id_String=USB_Device_Id_String_VCOM;
	USB_Vender_Id_String=USB_Vender_Id_String_VCOM;		
#else	

	USB_Device=USB_Device_Mass;
	USB_Config=USB_Config_Mass;
	USB_Device_Id_String=USB_Device_Id_String_Mass;
	USB_Vender_Id_String=USB_Vender_Id_String_Mass;
#endif	

	wbusbd_probe();
#if 0
	retval = register_chrdev_region(MKDEV(WBUSB_MAJOR, 0), 1, "usbclient");
	if ( retval < 0) {
		printk("wbusb : can not register chrdev region\n");
		return retval;
	}

	cdev_init(&wbusb_cdev, &wbusb_fops);
	wbusb_cdev.owner = THIS_MODULE;
	wbusb_cdev.ops = &wbusb_fops;
	retval = cdev_add(&wbusb_cdev, MKDEV(WBUSB_MAJOR, 0), 1);
	if ( retval < 0) {
		printk("wbusb : can not add wbusb_cdev\n");
		unregister_chrdev_region(MKDEV(WBUSB_MAJOR, 0), 1);
		return retval;
	}
#endif
	retval = register_chrdev(WBUSB_MAJOR, "usbclient", &wbusb_fops);
	if( retval < 0){
	  printk("can't get major %d\n", WBUSB_MAJOR);
	  return retval;
	}


	printk("W90N745 USB Device Driver 1.0 Initilization Success\n");


	LEAVE();
	
	return 0;

}

static void __exit wbusb_cleanup (void)
{
	ENTER();
#if 0
	cdev_del(&wbusb_cdev);
	unregister_chrdev_region(MKDEV(WBUSB_MAJOR, 0), 1);
#endif
	unregister_chrdev(WBUSB_MAJOR, "smartcard");

	Disable_USB(the_controller);
	
	if(the_controller)
		kfree(the_controller);

	LEAVE();
	
	return ;
}

module_init(wbusb_init);
module_exit(wbusb_cleanup);

MODULE_LICENSE("GPL");
