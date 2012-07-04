#include "platform.h"
#include "flash.h"
#include "uprintf.h"
#include "serial.h"
#include "tftp.h"

#define BANNER "W90P710 TFTP Server Version " VERSION " " REVISION "\n"
#define VERSION	"1.0"
#define REVISION "$Revision: 1 $"
#define COPYRIGHT "Copyright (c) 2003 Windond Electronics Corp."

#pragma import(__use_no_semihosting_swi)	

struct __Heap_Descriptor {
void *my_first_free_block;
void *my_heap_limit;
} my_heap;


void PrgInfo()
{
	uprintf("\n");
   	uprintf("                                           " __DATE__ "\n");
	uprintf("******************************************************\n\n");
	uprintf(BANNER);
	uprintf("\n");
	uprintf(COPYRIGHT"\n");
	uprintf("******************************************************\n");
	uprintf("\n");
}

int main()
{
	void (*func)();
	unsigned int src,dest,blockSize;
	unsigned int destAddress=FLASH_BASE;
	unsigned int srcAddress=0x100000;
	char *str;
	int i;
	int flash_type;
	unsigned long  fileSize;
	char mac[6]={0x00,0x01,0x02,0x03,0x04,0x00};
	__Heap_ProvideMemory(&my_heap,(void*)0x300000, 0x400000);
	// Initial serial
	uprintf("\n\n");
	uprintf("Initialize terminal baud rate to 115200 bps ...\n");
	init_serial(0,ARM_BAUD_115200);
	PrgInfo();
	uprintf("Flash Detecting ... \n");

	uprintf("\n");
	uprintf("PHY Chip         : DAVICOM DM9161E\n");
	uprintf("W90P710 MAC Port : MAC 1\n");
	uprintf("MAC Address      : %02x:%02x:%02x:%02x:%02x:%02x\n",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
	uprintf("\n");

	SetPhyChip(0);
	SetMacNumber(1);
	SetMacAddress(mac);
	Net_Init(1);	/* with DHCP */
	
	// download the image
	srcAddress=0x8000;
	uprintf("\nWaiting for LINUX.BIN download ... \n");	
	TFTP_Download((unsigned char *)srcAddress, &fileSize, 0);
	srcAddress=0x700000;
	uprintf("\nWaiting for ROMFS.IMG download ... \n");	
	TFTP_Download((unsigned char *)srcAddress, &fileSize, 0);
	
	func=(void (*)())0x8000;
	
	uprintf("\n\ng 0x8000 ...\n");
	func();	
	return 0;
}