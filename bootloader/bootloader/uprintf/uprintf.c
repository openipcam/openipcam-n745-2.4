/******************************************************************************
 *
 * Copyright (c) 2003 Windond Electronics Corp.
 * All rights reserved.
 *
 * $Workfile: uprintf.c $
 *
 * $Author: Yachen $
 ******************************************************************************/
/*
 * $History: uprintf.c $
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

#include "serial.h"

#define SWI_WB                       0x123456
#define SWI_WB_Thumb                 0xAB

#define SYS_FILE_OPEN                   0x01
#define SYS_FILE_CLOSE                  0x02
#define SYS_AGENTINFO                   0x35
#define SYS_VECTORCHAIN                 0x36
#define WB_SWI_SYS_WRITEC            0x03
#define WB_SWI_SYS_WRITE0            0x04
#define SYS_WRITE_SWI                   0x05
#define SYS_READ_SWI                    0x06
#define WB_SWI_SYS_READC             0x07
#define SYS_CLOCK                       0x10
#define WB_SWI_SYS_HEAPINFO          0x16
#define WB_SWIreason_EnterSVC        0x17

#define WB_SWIreason_ReportException  0x18
#define ADP_Stopped_ApplicationExit     0x20026


#define FALSE 0
#define TRUE 1



#define BAUD_115200 0x7
#define BAUD_57600 0xF
#define BAUD_38400 0x17
#define BAUD_RATE BAUD_38400

//#define COMMA

void  uprintf(char *f, ...); /* variable arguments */

__weak extern void  NET_putchar(char ch);
__weak extern char  NET_getchar(void);
__weak extern int  NET_kbhit(void);

static int net_write_flag=0; // alternate output device

int IsNetWrite()
{
	return net_write_flag;
}

int SetNetWrite(int flag)
{
	net_write_flag=flag;

	return net_write_flag;
}


//void  SetBaudRate_115200(void);
//void  SetBaudRate_115200(void)
//{
//	static flag=0;
	
//	if(flag!=0)
//		return;

	/* CWS: Disable interrupts */
//	*(volatile unsigned *) (0xFFF8000C) = 0; // prepare to Init UART
//	*(volatile unsigned *) (0xFFF80004)=0x0; // Disable all UART interrupt
    /* CWS: Set baud rate to baudRate bps */
//	*(volatile unsigned *) (0xFFF8000C) |= (0x1 <<  7); // prepare to access Divisor
//	*(volatile unsigned *) (0xFFF80000)=6;
//	*(volatile unsigned *) (0xFFF80004)=0;
//	*(volatile unsigned *) (0xFFF8000C) &= ~(0x1 << 7); // prepare to access RBR, THR, IER
    /* CWS: Set port for 8 bit, 1 stop, no parity  */
//	*(volatile unsigned *) (0xFFF8000C) = 0x3;
	/* CWS: Set the RX FIFO trigger level, reset RX, TX FIFO*/
//	*(volatile unsigned *) (0xFFF80008)=(0x2<<6)+4+2+1;
//	*((volatile unsigned *)0xFFF8001C)= 0x0 ;
//}



#define vaStart(list, param) list = (char*)((int)&param + sizeof(param))
#define vaArg(list, type) ((type *)(list += sizeof(type)))[-1]


unsigned int bbbbp=0;                        
#define PUSH_BUF(x) (*((unsigned char *)0xc2000000+(bbbbp++))=x)

#if defined(SEMIHOSTED)

#ifdef __thumb
unsigned int __swi(SWI_WB_Thumb) SWI_0(unsigned int reason);
     unsigned int __swi(SWI_WB_Thumb) SWI_1(unsigned int reason, void *arg1);

#else
unsigned int __swi(SWI_WB) SWI_0(unsigned int reason);
     unsigned int __swi(SWI_WB) SWI_1(unsigned int reason, void *arg1);

#endif

#define SWI_READC() SWI_0(WB_SWI_SYS_READC)


     void SWI_WRITEC(char ch)
{
    SWI_1(WB_SWI_SYS_WRITEC, &ch);
}


void SWI_WRITE0(char *s)
{
    SWI_1(WB_SWI_SYS_WRITE0, s);
}

#endif


void uputchar(char Ch)
{
	if( IsNetWrite() )
	{
		if( (int)NET_putchar ) 
			NET_putchar(Ch);
		else
			sendchar(&Ch);
	}
	else
	{
		sendchar(&Ch);
	}
}


void UART_PutString(char *string)
{
    while (*string != '\0')
    {
        uputchar(*string);
        string++;
    }
}


static void  PutRepChar(char c, int count)
{
    while (count--)
        uputchar(c);
}


static void  PutStringReverse(char *s, int index)
{
    while ((index--) > 0)
        uputchar(s[index]);
}


static void  PutNumber(int value, int radix, int width, char fill)
{
    char    buffer[40];
    int     bi = 0;
    unsigned int  uvalue;
    unsigned short  digit;
    unsigned short  left = FALSE;
    unsigned short  negative = FALSE;

    if (fill == 0)
        fill = ' ';

    if (width < 0)
    {
        width = -width;
        left = TRUE;
    }

    if (width < 0 || width > 80)
        width = 0;

    if (radix < 0)
    {
        radix = -radix;
        if (value < 0)
        {
            negative = TRUE;
            value = -value;
        }
    }

    uvalue = value;

    do
    {
        if (radix != 16)
        {
            digit = uvalue % radix;
            uvalue = uvalue / radix;
        }
        else
        {
            digit = uvalue & 0xf;
            uvalue = uvalue >> 4;
        }
        buffer[bi] = digit + ((digit <= 9) ? '0' : ('A' - 10));
        bi++;
#ifdef COMMA		
        if (uvalue != 0)
        {
            if ((radix == 10)
                && ((bi == 3) || (bi == 7) || (bi == 11) | (bi == 15)))
            {
                buffer[bi++] = ',';
            }
        }
#endif        
    }
    while (uvalue != 0);

    if (negative)
    {
        buffer[bi] = '-';
        bi += 1;
    }

    if (width <= bi)
        PutStringReverse(buffer, bi);
    else
    {
        width -= bi;
        if (!left)
            PutRepChar(fill, width);
        PutStringReverse(buffer, bi);
        if (left)
            PutRepChar(fill, width);
    }
}


static char  *FormatItem(char *f, int a)
{
    char   c;
    int    fieldwidth = 0;
    int    leftjust = FALSE;
    int    radix = 0;
    char   fill = ' ';
   	int	   i;
    

    if (*f == '0')
        fill = '0';

    while ((c = *f++) != 0)
    {
        if (c >= '0' && c <= '9')
        {
            fieldwidth = (fieldwidth * 10) + (c - '0');
        }
        else
            switch (c)
            {
                case '\000':
                    return (--f);
                case '%':
                    uputchar('%');
                    return (f);
                case '-':
                    leftjust = TRUE;
                    break;
                case 'c':
                    {
                        if (leftjust)
                            uputchar(a & 0x7f);

                        if (fieldwidth > 0)
                            PutRepChar(fill, fieldwidth - 1);

                        if (!leftjust)
                            uputchar(a & 0x7f);
                        return (f);
                    }
                case 's':
                    {
                    	i=0;while( *((char *)(a+i)) !='\0' )i++;
                    
                        if (leftjust)
                            UART_PutString((char *)a);
                        //if (fieldwidth > strlen((char *)a))
                            //PutRepChar(fill,fieldwidth - strlen((char *)a));
                        if (fieldwidth > i )
                            PutRepChar(fill,fieldwidth - i);
                        if (!leftjust)
                            UART_PutString((char *)a);
                        return (f);
                    }
                case 'd':
                case 'i':
                    radix = -10;
                    break;
                case 'u':
                    radix = 10;
                    break;
                case 'x':
                    radix = 16;
                    break;
                case 'X':
                    radix = 16;
                    break;
                case 'o':
                    radix = 8;
                    break;
                default:
                    radix = 3;
                    break;      /* unknown switch! */
            }
        if (radix)
            break;
    }

    if (leftjust)
        fieldwidth = -fieldwidth;

    PutNumber(a, radix, fieldwidth, fill);

    return (f);
}


void uprintf(char *f, ...) /* variable arguments */
{
    char  *argP;

    vaStart(argP, f);       /* point at the end of the format string */
    while (*f)
    {                       /* this works because args are all ints */
        if (*f == '%')
            f = FormatItem(f + 1, vaArg(argP, int));

        else
            uputchar(*f++);
    }


}

// Get a byte from the serial port.
//
// Return:
//     The byte read. Does not return until the byte is available
int ugetchar(void)
{
	if( IsNetWrite() )
	{
		if( (int)NET_getchar ) 
			return NET_getchar();
		else
			return serial_getchar();
	}
	else
	{
	    return serial_getchar();
	}
}


int ukbhit(void)
{
	if( IsNetWrite() )
	{
		if( (int)NET_kbhit ) 
			return NET_kbhit();
		else
			return serial_kbhit();
	}
	else
	{
		return serial_kbhit();
	}

}



