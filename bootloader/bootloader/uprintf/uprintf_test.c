/******************************************************************************
 *
 * Copyright (c) 2003 Windond Electronics Corp.
 * All rights reserved.
 *
 * $Workfile: uprintf_test.c $
 *
 * $Author: Yachen $
 ******************************************************************************/
/*
 * $History: uprintf_test.c $
 * 
 * *****************  Version 1  *****************
 * User: Yachen       Date: 06/01/10   Time: 10:55a
 * Created in $/W90P710/Applications/710bootloader/uprintf
 * 710 Bootloader, without USB support
 * 
 * *****************  Version 1  *****************
 * User: Yachen       Date: 06/01/04   Time: 2:27p
 * Created in $/W90P710/Module Test Programs/FIRMWARE_710/uprintf
 * Module test bootloader, removed decompress function in order to save
 * memory space for LCD control
 * 
 * *****************  Version 2  *****************
 * User: Wschang0     Date: 03/08/20   Time: 1:10p
 * Updated in $/W90P710/FIRMWARE/uprintf
 * Add VSS header
 */

#include "platform.h"
#include "uprintf.h"
#include "serial.h"


int main(void)
{
	int i;
	
	init_serial(0, ARM_BAUD_115200);

	while(1)
	for(i=0;i<10;i++)
	*((volatile unsigned int *)0xFFF80000)=0x30+i;
	
	
	
	
	return 0;
	
	for(i=0;i<10;i++)
		uprintf("\nThis is uprintf test %d",i);
	for(i=0;i<10;i++)
		uputchar('0');
		
	return 0;
}