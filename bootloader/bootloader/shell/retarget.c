/******************************************************************************
 *
 * Copyright (c) 2003 Windond Electronics Corp.
 * All rights reserved.
 *
 * $Workfile: retarget.c $
 *
 * $Author: Yachen $
 ******************************************************************************/
/*
 * $History: retarget.c $
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
 * *****************  Version 2  *****************
 * User: Wschang0     Date: 03/08/20   Time: 1:07p
 * Updated in $/W90P710/FIRMWARE/shell
 * Add VSS header
 */
/*
** This implements a 'retarget' layer for low-level IO.  Typically, this
** would contain your own target-dependent implementations of fputc(),
** ferror(), etc.
** 
** This example provides implementations of fputc(), ferror(),
** _sys_exit(), _ttywrch() and __user_initial_stackheap().
**
** Here, semihosting SWIs are used to display text onto the console 
** of the host debugger.  This mechanism is portable across ARMulator,
** Angel, Multi-ICE and EmbeddedICE.
**
** Alternatively, to output characters from the serial port of an 
** ARM Integrator Board (see serial.c), use:
**
**     #define USE_SERIAL_PORT
**
** or compile with 
**
**     -DUSE_SERIAL_PORT
*/

#include <stdio.h>
#include <rt_misc.h>


#ifdef __thumb
/* Thumb Semihosting SWI */
#define SemiSWI 0xAB
#else
/* ARM Semihosting SWI */
#define SemiSWI 0x123456
#endif


/* Write a character */ 
__swi(SemiSWI) void _WriteC(unsigned op, char *c);
#define WriteC(c) _WriteC (0x3,c)

/* Exit */
__swi(SemiSWI) void _Exit(unsigned op, unsigned except);
#define Exit() _Exit (0x18,0x20026)


struct __FILE { int handle;   /* Add whatever you need here */};
FILE __stdout;

//unsigned int aaaa;

extern void sendchar( char *ch );    // in serial.c

int fputc(int ch, FILE *f)
{
    /* Place your implementation of fputc here     */
    /* e.g. write a character to a UART, or to the */
    /* debugger console with SWI WriteC            */

    char tempch = ch;
#ifdef USE_SERIAL_PORT
    extern void uputchar(char ch);
	uputchar(tempch); // output to UART
#else
    WriteC( &tempch );
#endif
    return ch;
}


int ferror(FILE *f)
{   /* Your implementation of ferror */
    return EOF;
}


void _sys_exit(int return_code)
{

    Exit();         /* for debugging */

label:  goto label; /* endless loop */
}



void _ttywrch(int ch)
{
    char tempch = ch;
#ifdef USE_SERIAL_PORT
    sendchar( &tempch );
#else
    WriteC( &tempch );
#endif
}


// Returns the number of characters transmitted.
int print_sys_buf(unsigned int count, unsigned char *stream_ptr)
{
    do
    {
		sendchar((char *)stream_ptr);
		count--;
		stream_ptr++;
    }while (count != 0);
    return count;
}


// Routine to fill the stream buffer from the com port.
// Returns the first character read.
int fil_sys_buf(unsigned int max_count, unsigned char *stream_ptr)
{
    unsigned int ch, count = 0;
	extern int ugetchar(void);
    do
    {
		ch = ugetchar();
		
		switch (ch)
		{
	    	case 0x8:
				// Delete code.
				if (count > 0)
				{
			    	stream_ptr--;
				    count--;
				}
				sendchar((char *)&ch);
				break;
	    case 0xd:
				// Carriage return code, change to 0xa.
				ch = 0xa;
				count--;
	    default:
				sendchar((char *)&ch);
				*stream_ptr++ = ch;
				count++;
				break;
		}
    }while (ch != 0xa);
    stream_ptr -= count;
    return max_count - (count + 1);
}
