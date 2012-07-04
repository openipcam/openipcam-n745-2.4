/******************************************************************************
 *
 * Copyright (c) 2003 Windond Electronics Corp.
 * All rights reserved.
 *
 * $Workfile: main.c $
 *
 * $Author: Yachen $
 ******************************************************************************/
/*
 * $History: main.c $
 * 
 * *****************  Version 3  *****************
 * User: Yachen       Date: 07/01/12   Time: 2:13p
 * Updated in $/W90P710/Applications/710bootloader/shell
 * 
 * *****************  Version 2  *****************
 * User: Yachen       Date: 06/01/19   Time: 2:18p
 * Updated in $/W90P710/Applications/710bootloader/shell
 * Add USB support. User can chose between USB and MAC in shell.map
 * 
 * *****************  Version 1  *****************
 * User: Yachen       Date: 06/01/10   Time: 10:55a
 * Created in $/W90P710/Applications/710bootloader/shell
 * 710 Bootloader, without USB support
 * 
 * *****************  Version 1  *****************
 * User: Yachen       Date: 06/01/04   Time: 2:27p
 * Created in $/W90P710/Module Test Programs/FIRMWARE_710/shell
 * Module test bootloader, removed decompress function in order to save
 * memory space for LCD control
 * 
 * *****************  Version 7  *****************
 * User: Wschang0     Date: 04/11/22   Time: 3:18p
 * Updated in $/W90P710/FIRMWARE/shell
 * Add serial number item
 * 
 * *****************  Version 6  *****************
 * User: Wschang0     Date: 04/03/19   Time: 5:09p
 * Updated in $/W90P710/FIRMWARE/shell
 * fix ROMCON bug
 * 
 * *****************  Version 5  *****************
 * User: Wschang0     Date: 03/11/05   Time: 11:04a
 * Updated in $/W90P710/FIRMWARE/shell
 * Add RMII Option
 * 
 * *****************  Version 4  *****************
 * User: Wschang0     Date: 03/10/20   Time: 6:14p
 * Updated in $/W90P710/FIRMWARE/shell
 * Replease the Wait "ESC" or "B" key to debug_wait() function. This
 * function should be implemented by user.
 * 
 * *****************  Version 3  *****************
 * User: Wschang0     Date: 03/09/26   Time: 2:28p
 * Updated in $/W90P710/FIRMWARE/shell
 * Add semi_stack variable to store the user program stack pointer
 * 
 * *****************  Version 2  *****************
 * User: Wschang0     Date: 03/08/20   Time: 1:07p
 * Updated in $/W90P710/FIRMWARE/shell
 * Add VSS header
 */
#include <string.h>
#include <rt_heap.h>
#include "platform.h"
#include "flash.h"
#include "bib.h"
#include "net.h"
#include "serial.h"
#include "uprintf.h"
#include "timer.h"
#include "tftp.h"
#include "sh.h"

extern void debug_wait(void);
extern VOID WBL_info(VOID);
extern INT BootProcess(VOID);

#ifdef __USB__  // Let bootloader knows which one of USB and MAC is enabled.
const int USE_USB = 1;
extern int Enable_USB(void);
#else
const int USE_USB = 0;
#endif


#pragma import(__use_no_semihosting_swi)	
#define DELAY_LOOP	0x50000

extern struct heap_info_block{
	int heap_base;
	int heap_limit;
	int stack_base;
	int stack_limit;
} heap_info;


struct __Heap_Descriptor {
void *my_first_free_block;
void *my_heap_limit;
} my_heap;

/* The WBL will detect the maximum memory size and store it to here */
UINT32	memory_size;
/* The WBL will use this to be the default stack for user program. It must be the limit of BL buffer for safe */
UINT32 semi_stack;
// from mem.c
extern UINT32 MemSize(void);
// from time.c

int main(void)
{
	UINT32 /*i,*/tmp;
	tfooter * footer=NULL;
	tbl_info info;
	INT flash_type;
	INT divider;
	// check memory size;
	memory_size=MemSize();
	
	// configure GPIO port 2 functino to PHY
	//GPIO_CFG2 = 0x00055555; 
	*(unsigned int *)(0xfff83040) = 0x55555;

	// system init
	init_serial(0 ,ARM_BAUD_115200);
	uputchar('\n');
	//init_timer();
	//Net_Init(1);

	// Update the flash ROM size
	flash_type=FindFlash(); // check flash type
	// un-supported flash type
	if( flash_type < 0 )
	{
		uprintf("ERROR: Un-supported flash type !! The system may not work.\n");
		flash_type = 0;
		uprintf("       Use default flash type: %s\n",flash[flash_type].name);
		ROMCON=ROMCON&(~(0x70000));
		ROMCON=ROMCON|0x40000;
	}
	else
	{
		if( (ROMCON&0xFF000000)!=0xFC000000 )
		{
			tmp=FlashSize();
			ROMCON=ROMCON&(~(0x70000));
			switch(tmp)
			{
				default:
				case 0x40000:
					ROMCON=ROMCON|0x00000;
					break;
				case 0x80000:
					ROMCON=ROMCON|0x10000;
					break;
				case 0x100000:
					ROMCON=ROMCON|0x20000;
					break;
				case 0x200000:
					ROMCON=ROMCON|0x30000;
					break;
				case 0x400000:
					ROMCON=ROMCON|0x40000;
					break;
				case 0x800000:
					ROMCON=ROMCON|0x50000;
					break;
				case 0x1000000:
					ROMCON=ROMCON|0x60000;
					break;
				case 0x2000000:
					ROMCON=ROMCON|0x70000;
					break;
			}
		}
	}


	// find image 0
	if( !FindImage(0, &footer) )
	{
		// image 0 not exist. Create it.
		tfooter image_footer;
		CHAR mac0[6]=BL_DEFAULT_MAC0_ADDR;
		CHAR ip0[6]=BL_DEFAULT_IP0_ADDR;
		//CHAR mac1[6]=BL_DEFAULT_MAC1_ADDR;
		//CHAR ip1[6]=BL_DEFAULT_IP1_ADDR;

		// check if the flash type supported
		if( FindFlash() < 0 )
		{
			// un-supported flash type, just create the default setting of boot information block
			info.length=sizeof(info);
			info.type=BOOTLOADER_INFO;
		
			memcpy(info.mac0, mac0, 6);
			memcpy(info.ip0, ip0, 4);
		//	memcpy(info.mac1, mac1, 6);
		//	memcpy(info.ip1, ip1, 4);
			ip0[4]=0;
			ip0[5]=0;
		//	ip1[4]=0;
		//	ip1[5]=0;
			info.cache=BL_CACHE_DEFAULT;
			info.dhcp=BL_DHCP;
			info.net_mac=BL_NET_MAC;
			info.phy=BL_DEFAULT_PHY_CHIP;
			info.buf_base=BL_BUFFER_BASE;
			info.buf_size=BL_BUFFER_SIZE;
			info.baudrate=BL_DEFAULT_BAUD_RATE;
			info.rmii=1;
			info.serial_no = 0;
			info.usb = 1;
		}
		else
		{				
			// make sure the block 1 is empty.
			DelBlock(1);

			info.length=sizeof(info);
			info.type=BOOTLOADER_INFO;
		
			memcpy(info.mac0, mac0, 6);
			memcpy(info.ip0, ip0, 4);
		//	memcpy(info.mac1, mac1, 6);
		//	memcpy(info.ip1, ip1, 4);
			ip0[4]=ip0[5]=0;//ip1[4]=ip1[5]=0;
			info.cache=BL_CACHE_DEFAULT;
			info.dhcp=BL_DHCP;
			info.net_mac=BL_NET_MAC;
			info.phy=BL_DEFAULT_PHY_CHIP;
			info.buf_base=BL_BUFFER_BASE;
			info.buf_size=BL_BUFFER_SIZE;
			info.baudrate=BL_DEFAULT_BAUD_RATE;
			info.rmii=1;
			info.serial_no = 0;
			info.usb = 1;
			uprintf("WARNING: Image 0 was not found. Try to create one ... \n");
			image_footer.num=0;
			image_footer.base=BL_IMAGE0_BASE;
			image_footer.length=sizeof(tbl_info);
			image_footer.load_address=image_footer.exec_address=image_footer.base;
			image_footer.signature=SIGNATURE_WORD;
			image_footer.type=IMAGE_FILE;
			strcpy(image_footer.name,"BOOT INFO");
		
			if( WriteImage(&image_footer, (UINT32)&info) )
			{
				uprintf("ERROR: Fail to create image 0. \n");
				footer=(tfooter *)0;
			}
			else
			{
				uprintf("Image 0 created\n");
				FindImage(0, &footer);
			}
		}
	}

	// Read boot loader configuration
	if( footer )
		memcpy((CHAR *)&info, (CHAR *)footer->base, sizeof(info));

	//uprintf("flash size=0x%08x\n", W90P710_FLASH_SIZE);

	// Change the baud rate according to the Boot Information
	if(UART_Speed(info.baudrate,&divider))
	{
		if( info.baudrate != 115200 )
		{
			//uprintf("WARNING: Baud rate changed to %dbps ...\n\n",info.baudrate);
			//sleep(500);
			init_serial(0, divider);
			uprintf("\n\n");
		}
	}
	else
	{
		info.baudrate=BL_DEFAULT_BAUD_RATE; 
	}
	// show the current boot loader information
	WBL_info();
#ifndef __USB__	
	// Load configuration from information block
	if( info.net_mac )
	{
		// MAC address
		SetMacAddress(info.mac1);  //========>
		// IP address
		SetIpAddress(info.ip1);   //========>
	}
	else
	{
		// MAC address
		SetMacAddress(info.mac0);   //=======>
		// IP address
		SetIpAddress(info.ip0);   //========>
	}
	
	// RMII interface
	_rmii=((info.rmii==1) ? 1:0);  //=======>
	
	// dhcp
	_dhcp=info.dhcp;  //========>
	// net_mac
	SetMacNumber(info.net_mac);  //=======>
	// phy
	_phy=info.phy;  //=========>
#endif	
	// check if the buffer size enought
	if( info.buf_size < SEMI_HEAP_SIZE+SEMI_STACK_SIZE )
		uprintf("WARNNING: The buffer size may not be enough\n");
	
	// heap space for ZIP & TFTP
	//__Heap_Initialize(&my_heap); // may be wrong
	__Heap_ProvideMemory(&my_heap,(void*)info.buf_base,info.buf_size);
	my_heap.my_first_free_block=0;
	// heap space for Semihosted applications, heap 128KB, stack 128KB
#if 0	
	heap_info.heap_base=(UINT32) __Heap_Alloc(&my_heap,SEMI_HEAP_SIZE);
	heap_info.heap_limit=heap_info.heap_base+SEMI_HEAP_SIZE;
	heap_info.stack_limit=(UINT32) __Heap_Alloc(&my_heap,SEMI_STACK_SIZE);
	heap_info.stack_base=heap_info.stack_limit+SEMI_STACK_SIZE;

	// initialize the default stack for user program
	semi_stack=info.buf_base+info.buf_size;
#else
	heap_info.heap_base= 0;
	heap_info.heap_limit=memory_size - 0x1000;
	heap_info.stack_limit=memory_size/2;
	heap_info.stack_base=memory_size - 0x1000;
	
	// initialize the default stack for user program
	semi_stack = memory_size;

#endif

	debug_wait();
	
	// Load images & exection
	BootProcess();
	// Maybe but not sure... The AP may return ... May no any boot images ...
	sh(0, 0);
	
	return 0;
}


