/******************************************************************************
 *
 * Copyright (c) 2003 Windond Electronics Corp.
 * All rights reserved.
 *
 * $Workfile: tftp_test.c $
 *
 * $Author: Yachen $
 ******************************************************************************/
/*
 * $History: tftp_test.c $
 * 
 * *****************  Version 1  *****************
 * User: Yachen       Date: 06/01/10   Time: 10:55a
 * Created in $/W90P710/Applications/710bootloader/TFTPserv/W90N740/Src
 * 710 Bootloader, without USB support
 * 
 * *****************  Version 1  *****************
 * User: Yachen       Date: 06/01/04   Time: 2:27p
 * Created in $/W90P710/Module Test Programs/FIRMWARE_710/TFTPserv/W90N740/Src
 * Module test bootloader, removed decompress function in order to save
 * memory space for LCD control
 * 
 * *****************  Version 5  *****************
 * User: Wschang0     Date: 04/03/19   Time: 5:26p
 * Updated in $/W90N740/FIRMWARE/TFTPserv/W90N740/Src
 * 
 * *****************  Version 4  *****************
 * User: Wschang0     Date: 03/08/20   Time: 1:17p
 * Updated in $/W90N740/FIRMWARE/TFTPserv/W90N740/Src
 * Add VSS header
 * Add PrgInfo function to show the version information
 * 
 * *****************  Version 3  *****************
 * User: Wschang0     Date: 03/08/20   Time: 1:16p
 * Updated in $/W90N740/FIRMWARE/TFTPserv/W90N740/Src
 * Add VSS header
 * Add PrgInfo function to show the version information
 * 
 * *****************  Version 2  *****************
 * User: Wschang0     Date: 03/08/20   Time: 1:14p
 * Updated in $/W90N740/FIRMWARE/TFTPserv/W90N740/Src
 * Add VSS header
 * Add PrgInfo function to show the version information
 */
#include "platform.h"
#include "flash.h"
#include "uprintf.h"
#include "serial.h"
#include "tftp.h"

#define BANNER "W90P710 TFTP Server Version " VERSION " " REVISION "\n"
#define VERSION	"1.0"
#define REVISION "$Revision: 1 $"
#define COPYRIGHT "Copyright (c) 2003 Windond Electronics Corp."


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

//#pragma import(__use_no_semihosting_swi)	

extern char  NET_getchar(void);


struct __Heap_Descriptor {
void *my_first_free_block;
void *my_heap_limit;
} my_heap;




#if 1
#define MAX_NUM 8
char * GetAddress(char *string);
unsigned int s2hex(char *s);
int validhex(char *s);


int main()
{
	unsigned int src,dest,blockSize;
	unsigned int destAddress=FLASH_BASE;
	unsigned int srcAddress=0x100000;
	char *str;
	int i;
	int flash_type;
	unsigned long  fileSize;
	char mac[6]={0x00,0x00,0x00,0x00,0x00,0x01};
	__Heap_ProvideMemory(&my_heap,(void*)0x40000, 0xA0000);
	
	// configure GPIO port 2 functino to PHY
	GPIO_CFG2 = 0x00055555; 
	
	// Initial serial
	uprintf("\n\n");
	uprintf("Initialize terminal baud rate to 115200 bps ...\n");
	init_serial(0,ARM_BAUD_115200);
	PrgInfo();
	uprintf("Flash Detecting ... \n");
	// check flash type first
	flash_type=FindFlash();
	if( flash_type < 0 )
	{
		uprintf("Un-supported flash type !! \n");
		return -1;
	}

	uprintf("\n");
	uprintf("Flash type is    : %s\n",flash[flash_type].name);
	uprintf("PHY Chip         : DAVICOM DM9161E\n");
	uprintf("W90N740 MAC Port : MAC 1\n");
	uprintf("MAC Address      : %02x:%02x:%02x:%02x:%02x:%02x\n",mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
	uprintf("\n");
	
	SetRMII(1);
	SetPhyChip(0);
	SetMacNumber(1);
	SetMacAddress(mac);
	Net_Init(1);	/* with DHCP */
	
	// download the image
	uprintf("\nWaiting for download ... \n");	
	TFTP_Download((unsigned char *)srcAddress, &fileSize, 0);
	// disable irq
	{
		INT tmp;
		__asm
		{
			MRS	tmp, CPSR
			ORR	tmp, tmp, 0x80
			MSR	CPSR_c, tmp
		}
	}
	
	// write the image
	
	str=GetAddress("Please enter the write address(0x7F000000):");	
	if( validhex( str ) )
	{
		destAddress = s2hex(str);
		uprintf("\nImage write to 0x%08x\n\n",destAddress);
	}
	else
		uprintf("Image load address default to 0x%08x\n",destAddress);

	
	
	// Write program
	i=(fileSize&(~0x3))+0x4; //word aligment
	src=srcAddress;
	dest=destAddress;
	while(i)
	{
		uputchar('.');
		blockSize=flash[flash_type].BlockSize(dest);
		if( (dest&(blockSize-1)) )
			blockSize=blockSize-(dest&(blockSize-1));
		else
			flash[flash_type].BlockErase(dest, blockSize);
		if( i < blockSize )blockSize=i; // Check if > a block size
		flash[flash_type].BlockWrite(dest, (UCHAR *)src, blockSize);
		src+=blockSize;
		dest+=blockSize;
		i-=blockSize;
	}
	uprintf("\nVerifing ... ");
	// Verify program
	i=(fileSize&(~0x3))+0x4; //word aligment
	src=srcAddress;
	dest=destAddress;
	blockSize=flash[flash_type].BlockSize(destAddress);
	blockSize=blockSize-1;
	while(i)
	{
		if( (i&blockSize)==0x0 )putchar('.');
		if( inpw(src) != inpw(dest) )
		{
			printf("\nERROR: A:0x%08x W:0x%08x R:0x%08x!!\n",dest,inpw(src),inpw(dest));
			return -1;
		}
		src+=4;
		dest+=4;
		i-=4;
	}
	printf(" OK!\n");	
	printf("Programming finished!!\n");	
	
	return 0;

}


#else

int  main(int argc, void *argv[])
{
	unsigned long  fileSize;
	char mac[6]={0x00,0x01,0x02,0x03,0x04,0x00};

	__Heap_ProvideMemory(&my_heap,(void*)0x200000, 0x100000);
//	__Heap_Initialize(&my_heap);
//	SetMacNumber(0);	
	uprintf("TFTP server demo \n");
	//Net_Init(1);	/* with DHCP */
	//SetNetWrite(1);
/*
{
	char ch;
	int i;
	unsigned int cnt;
	int flag=0;

	while(0)
	{
		if( NET_kbhit() )
		{
			ch=NET_getchar();
			if( ch==SOH )
			{
				flag=1;
				ch=NET_getchar();//packet no
				ch=NET_getchar();// ~packet no
				for(i=0;i<128;i++)
				{
					ch=NET_getchar();
					//uprintf("%d\n",ch);
				}
				NET_putchar(ACK);
			}
		}
		else
		{
			if( (cnt++ & 0xFFFF)==0 )
			{
				if( flag== 0 )
					NET_putchar('C');
				else
					NET_putchar(ACK);
			}
			
		}
	}

	return 0;
}
*/

	SetPhyChip(0);
	SetMacNumber(1);
	Net_Init(1);	/* with DHCP */

/*
while(1)
{
	char ch;
	//uprintf("TFTP server demo \n");
	//ch=NET_getchar();
	ch=ugetchar();
	if( ch=='\r' )ch='\n';
	if( ch==27 )
		TFTP_Download((unsigned char *)0x200000, &fileSize, 0);
	else
		uputchar(ch);
}	
*/	
	while(1)TFTP_Download((char *)0x300000, &fileSize, 0);

	return 0;
}
#endif



char * GetAddress(char *string)
{
	static char str[16];
	int i;
	char ch;
	
	for(i=0;i<16;i++)str[i]=0;
	uprintf("%s ",string);
	i=0;
	while(1)
	{
		ch=ugetchar();
		if( ch == '\n' || ch == '\r' )
		{
			uputchar('\n');
			return str;
		}
		else
		{
			if( ch == '\b' )
			{
			  if(  i >= 1 )
			  {
				str[--i]='\0';
				uprintf("\r\r%20s %s",string,str);
			  }	
			}
			else
			{
			
				if( i<15 )
				{
					str[i++]=ch;
					uputchar(ch);
				}
			}		
		}
	}
}


int chartohex(char c)
{
    if ((c >= '0') && (c <= '9'))
        return c - '0';
    if ((c >= 'a') && (c <= 'f'))
        return c - 'a' + 10;
    if ((c >= 'A') && (c <= 'F'))
        return c - 'A' + 10;
    return -1;
}

unsigned int s2hex(char *s)
{
    char nStack[MAX_NUM];
    int n;
    unsigned int result;
    int shift;

    // hop over the 0x start
    s += 2;

    // stack the number in reverse order
    n = 0;
    while ((*s != '\0') && (n < MAX_NUM))
    {
        nStack[n++] = *s++;
    }

    // Pull characters off the stack, converting to hex on the way
    result = 0;
    shift = 0;
    while (n > 0)
    {
        result += (chartohex(nStack[--n]) << shift);
        shift += 4;
    }

    return result;
}


int validhex(char *s)
{
    // Is this string a valid hex number?
    // For example, of the form 0xNNNNNNNN

    // valid length?
    if (strlen(s) > 10)
        return 0;
    if (strlen(s) <= 2)
        return 0;

    // valid start? (0x or 0X)
    if ((s[0] != '0') || ((s[1] != 'X') && (s[1] != 'x')))
        return 0;

    s += 2;

    // valid characters?
    while (*s != '\0')
    {
        if (chartohex(*s) < 0)
            // this character not valid
            return 0;

        // go onto the next character
        s++;
    }

    // success
    return 1;
}
