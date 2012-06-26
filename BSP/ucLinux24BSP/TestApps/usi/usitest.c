#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#include <errno.h>

#include "w90n745_usi.h"

//-- function return value
#define	   Successful  0
#define	   Fail        1

#define UINT32	unsigned int
#define UINT16	unsigned short
#define UINT8		unsigned char

static __inline UINT16 Swap16(UINT16 val)
{
    return (val<<8) | (val>>8);
}

#define UsiSelectCS0()				ioctl(usi_fd, USI_IOC_SELECTSLAVE, 0)
#define UsiDeselectCS0()			ioctl(usi_fd, USI_IOC_SELECTSLAVE, -1)
#define UsiDoTransit()				ioctl(usi_fd, USI_IOC_TRANSIT, &gdata)
#define UsiSetBitLen(x)				gdata.bit_len = x
#define UsiSetData(x)					gdata.write_data = x
#define UsiGetData()					gdata.read_data

#define FLASH_BUFFER_SIZE	2048

UINT8 Flash_Buf[FLASH_BUFFER_SIZE];
UINT8 ReadBackBuffer[FLASH_BUFFER_SIZE];


int usi_fd;
struct usi_data gdata;

int usiCheckBusy()
{
	// check status
	UsiSelectCS0();	// CS0

	// status command
	UsiSetData(0x05);
	UsiSetBitLen(8);
	UsiDoTransit();

	// get status
	while(1)
	{
		UsiSetData(0xff);
		UsiSetBitLen(8);
		UsiDoTransit();
		if (((UsiGetData() & 0xff) & 0x01) != 0x01)
			break;
	}

	UsiDeselectCS0();	// CS0

	return Successful;
}

/*
	addr: memory address 
	len: byte count
	buf: buffer to put the read back data
*/
int usiRead(UINT32 addr, UINT32 len, UINT8 *buf)
{
	int volatile i;

	UsiSelectCS0();	// CS0

	// read command
	UsiSetData(03);
	UsiSetBitLen(8);
	UsiDoTransit();

	// address
	UsiSetData(addr);
	UsiSetBitLen(24);
	UsiDoTransit();

	// data
	for (i=0; i<len; i++)
	{
		UsiSetData(0xff);
		UsiSetBitLen(8);
		UsiDoTransit();
		*buf++ = UsiGetData() & 0xff;
	}

	UsiDeselectCS0();	// CS0

	return Successful;
}

int usiReadFast(UINT32 addr, UINT32 len, UINT8 *buf)
{
	int volatile i;

	UsiSelectCS0();	// CS0

	// read command
	UsiSetData(0x0b);
	UsiSetBitLen(8);
	UsiDoTransit();

	// address
	UsiSetData(addr);
	UsiSetBitLen(24);
	UsiDoTransit();

	// dummy byte
	UsiSetData(0xff);
	UsiSetBitLen(8);
	UsiDoTransit();

	// data
	for (i=0; i<len; i++)
	{
		UsiSetData(0xff);
		UsiSetBitLen(8);
		UsiDoTransit();
		*buf++ = UsiGetData() & 0xff;
	}

	UsiDeselectCS0();	// CS0

	return Successful;
}

int usiWriteEnable()
{
	UsiSelectCS0();// CS0

	UsiSetData(0x06);
	UsiSetBitLen(8);
	UsiDoTransit();

	UsiDeselectCS0();	// CS0

	return Successful;
}

int usiWriteDisable()
{
	UsiSelectCS0();	// CS0

	UsiSetData(0x04);
	UsiSetBitLen(8);
	UsiDoTransit();

	UsiDeselectCS0();	// CS0

	return Successful;
}

/*
	addr: memory address
	len: byte count
	buf: buffer with write data
*/
int usiWrite(UINT32 addr, UINT32 len, UINT16 *buf)
{
	int volatile count=0, page, i;

	count = len / 256;
	if ((len % 256) != 0)
		count++;

	for (i=0; i<count; i++)
	{
		// check data len
		if (len >= 256)
		{
			page = 128;
			len = len - 256;
		}
		else
			page = len/2;

		usiWriteEnable();

		UsiSelectCS0();	// CS0

		// write command
		UsiSetData(0x02);
		UsiSetBitLen(8);
		UsiDoTransit();

		// address
		UsiSetData(addr+i*256);
		UsiSetBitLen(24);
		UsiDoTransit();

		// write data
		while (page-- > 0)
		{
			UsiSetData(Swap16(*buf++));
			UsiSetBitLen(16);
			UsiDoTransit();
		}

		UsiDeselectCS0();	// CS0

		// check status
		usiCheckBusy();
	}

	return Successful;
}

int usiEraseSector(UINT32 addr, UINT32 secCount)
{
	int volatile i;

	for (i=0; i<secCount; i++)
	{
		usiWriteEnable();

		UsiSelectCS0();	// CS0

		// erase command
		UsiSetData(0xd8);
		UsiSetBitLen(8);
		UsiDoTransit();

		// address
		UsiSetData(addr+i*256);
		UsiSetBitLen(24);
		UsiDoTransit();

		UsiDeselectCS0();	// CS0

		// check status
		usiCheckBusy();
	}
	return Successful;
}

int usiEraseAll()
{
	usiWriteEnable();

	UsiSelectCS0();// CS0

	UsiSetData(0xc7);
	UsiSetBitLen(8);
	UsiDoTransit();

	UsiDeselectCS0();	// CS0

	// check status
	usiCheckBusy();

	return Successful;
}

UINT16 usiReadID()
{
	UINT16 volatile id;

	UsiSelectCS0();	// CS0

	// command 8 bit
	UsiSetData(0x90);
	UsiSetBitLen(8);
	UsiDoTransit();

	// address 24 bit
	UsiSetData(0x000000);
	UsiSetBitLen(24);
	UsiDoTransit();

	// data 16 bit
	UsiSetData(0xffff);
	UsiSetBitLen(16);
	UsiDoTransit();
	id = UsiGetData() & 0xffff;

	UsiDeselectCS0();	// CS0

	return id;
}

UINT8 usiStatusRead()
{
	UINT32 status;

	UsiSelectCS0();		// CS0

	// status command
	UsiSetData(0x05);
	UsiSetBitLen(8);
	UsiDoTransit();

	// get status
	UsiSetData(0xff);
	UsiSetBitLen(8);
	UsiDoTransit();
	status = UsiGetData() & 0xff;

	UsiDeselectCS0();	// CS0

	return status;
}

int usiStatusWrite(UINT8 data)
{
	usiWriteEnable();

	UsiSelectCS0();	// CS0

	// status command
	UsiSetData(0x01);
	UsiSetBitLen(8);
	UsiDoTransit();

	// write status
	UsiSetData(data);
	UsiSetBitLen(8);
	UsiDoTransit();

	UsiDeselectCS0();	// CS0

	// check status
	usiCheckBusy();

	return Successful;
}

int original_main()
{
	int volatile i;

	for (i=0; i<FLASH_BUFFER_SIZE; i++)
		Flash_Buf[i] = i;

	printf("flash ID [0x%04x]\n", usiReadID());
	printf("flash status [0x%02x]\n", usiStatusRead());

//	printf("erase all\n");
//	usiEraseAll();
	printf("erase sector\n");
	i = FLASH_BUFFER_SIZE / 256;
	if ((FLASH_BUFFER_SIZE % 256) != 0)
		i++;
	usiEraseSector(0, i);

	printf("write\n");
	usiWrite(0, FLASH_BUFFER_SIZE, (UINT16 *)Flash_Buf);

	printf("read\n");
	usiRead(0, FLASH_BUFFER_SIZE, ReadBackBuffer);
//	usiReadFast(0, FLASH_BUFFER_SIZE, ReadBackBuffer);

	printf("compare\n");
	for (i=0; i<FLASH_BUFFER_SIZE; i++)
	{
		if (ReadBackBuffer[i] != Flash_Buf[i])
		{
			printf("error! [%d]->wrong[%x] / correct[%x]\n", i, ReadBackBuffer[i], Flash_Buf[i]);
			break;
		}
	}

	printf("finish..\n");
	return 0;
}


int main()
{
	int i;
	struct usi_parameter para;
	
	usi_fd = open("/dev/usi", O_RDWR);
	if ( usi_fd < 0) {
		printf("Open usi error\n");
		return -1;
	}
	
	//ioctl(usi_fd, USI_IOC_GETPARAMETER, &para);  // Mark 01.14, Michael
	
	para.divider = 1;				// SCK = PCLK/16 (20MHz)
	para.active_level = 0;	// CS active low
	para.tx_neg = 1;				// Tx: falling edge, Rx: rising edge
	para.rx_neg = 0;
	para.lsb = 0;
	para.sleep = 0;
	
	ioctl(usi_fd, USI_IOC_SETPARAMETER, &para);
	ioctl(usi_fd, USI_IOC_GETPARAMETER, &para);
	
	
	printf("para.lsb : %d\n", para.lsb);
	printf("para.tx_neg : %d\n", para.tx_neg);
	printf("para.rx_neg : %d\n", para.rx_neg);
	printf("para.divider : %d\n", para.divider);
	printf("para.active_level : %d\n", para.active_level);
	printf("para.sleep : %d\n", para.sleep);	

/************************************************************************/

	original_main();
	
/************************************************************************/
	
	close(usi_fd);	
	
	return 0;
}	

