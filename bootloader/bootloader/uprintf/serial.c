/******************************************************************************
 *
 * Copyright (c) 2003 Windond Electronics Corp.
 * All rights reserved.
 *
 * $Workfile: serial.c $
 *
 * $Author: Yachen $
 ******************************************************************************/
/*
 * $History: serial.c $
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

#define GET_MSB(x)	((x&0xFFFF)>>8)
#define GET_LSB(x)	((x&0x00FF))
void init_serial(unsigned int port, unsigned int baudRate) 
{
	GPIO_CFG5 = 0x15555555;
	/* CWS: Disable interrupts */
	UART_LCR = 0; // prepare to Init UART
	UART_IER = 0x0; // Disable all UART interrupt
    /* CWS: Set baud rate to baudRate bps */
	UART_LCR|= DLAB; // prepare to access Divisor
	UART_DLL = GET_LSB(baudRate);
	UART_DLM = GET_MSB(baudRate);
	UART_LCR&= ~DLAB; // prepare to access RBR, THR, IER
    /* CWS: Set port for 8 bit, 1 stop, no parity  */
	UART_LCR = UART_8bit;
	/* CWS: Set the RX FIFO trigger level, reset RX, TX FIFO*/
	UART_FCR = rUART_FCR;
	UART_TOR = 0x0 ;
}

void sendchar( char *ch )
{
	char Ch;
	unsigned int Status;
	Ch=*ch;
    do
    {
		Status = GET_STATUS(OS_COMPORT);
    }
    while (!TX_READY(Status));	// wait until ready

    PUT_CHAR(OS_COMPORT, Ch);
    if (Ch == '\n')
    {
		do
		{
		    Status = GET_STATUS(OS_COMPORT);
		}
		while (!TX_READY(Status));	// wait until ready

		PUT_CHAR(OS_COMPORT, '\r');
	}
}


int serial_kbhit(void)
{
    unsigned int Status;

#if defined(SEMIHOSTED)

    // If the ports are the same, have to use the debugger and we
    // can't tell if there is a character available
    if (HOST_COMPORT == OS_COMPORT)
    {
	return (1);
    }

#endif
    //lib_flush_buffer();
    Status = GET_STATUS(0xFFF80000);
    if (!RX_DATA(Status))
	return (0);
    else
	return (1);
}


int serial_getchar(void)
{
    unsigned int Status;
    unsigned int Ch;

	Ch=0;
    //Ch = lib_support_getchar();
    if (Ch == 0)
    {
#if defined(SEMIHOSTED)
	// Use the debugger if the ports are the same
	if (HOST_COMPORT == OS_COMPORT)
	    Ch = (int)SWI_READC();
	else
#endif
	{	    
		do
		{
			Status = GET_STATUS(OS_COMPORT);
	    }
	    while (!RX_DATA(Status));	// wait until ready

	    Ch = GET_CHAR(OS_COMPORT);
	}
    }
    return ((int)Ch);
}				

int UART_Speed(int speed, int *divider)
{
    switch (speed)
    {
        case 460800:
            *divider = ARM_BAUD_460800;
            break;
        case 230400:
            *divider = ARM_BAUD_230400;
            break;
        case 115200:
            *divider = ARM_BAUD_115200;
            break;
        case 57600:
            *divider = ARM_BAUD_57600;
            break;
        case 38400:
            *divider = ARM_BAUD_38400;
            break;
        case 28800:
            *divider = ARM_BAUD_28800;
            break;
        case 19200:
            *divider = ARM_BAUD_19200;
            break;
        case 14400:
            *divider = ARM_BAUD_14400;
            break;
        case 9600:
            *divider = ARM_BAUD_9600;
            break;
        case 4800:
            *divider = ARM_BAUD_2400;
            break;
        case 2400:
            *divider = ARM_BAUD_2400;
            break;
        case 1200:
            *divider = ARM_BAUD_1200;
            break;
        default:
            return 0;
    }
    return speed;
}

