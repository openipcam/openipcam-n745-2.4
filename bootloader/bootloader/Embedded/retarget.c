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
 * Created in $/W90P710/Applications/710bootloader/Embedded
 * 710 Bootloader, without USB support
 * 
 * *****************  Version 1  *****************
 * User: Yachen       Date: 06/01/04   Time: 2:27p
 * Created in $/W90P710/Module Test Programs/FIRMWARE_710/Embedded
 * Module test bootloader, removed decompress function in order to save
 * memory space for LCD control
 * 
 * *****************  Version 2  *****************
 * User: Wschang0     Date: 03/08/20   Time: 1:22p
 * Updated in $/W90P710/FIRMWARE/Embedded
 * Add VSS header
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

void dummy_func(void)
{
    extern void Vector_Table(void);
    unsigned int temp;
    __asm
    {
    	mov temp, 0x38
    	sub	temp, temp, 0x38
    }
	if( temp )Vector_Table();
}

/*
To place heap_base directly above the ZI area, use e.g:
    extern unsigned int Image$$ZI$$Limit;
    config.heap_base = (unsigned int)&Image$$ZI$$Limit;
(or &Image$$region_name$$ZI$$Limit for scatterloaded images)

To specify the limits for the heap & stack, use e.g:
    config.heap_limit = SL;
    config.stack_limit = SL;
*/
__value_in_regs struct __initial_stackheap __user_initial_stackheap(
        unsigned R0, unsigned SP, unsigned R2, unsigned SL)
{
	extern unsigned int bottom_of_heap;     /* defined in heap.s */
	extern unsigned int top_of_stacks;		/* defined in stack.s */
    struct __initial_stackheap config;
    
    config.heap_base = (unsigned int)&bottom_of_heap; // defined in heap.s
                                                      // placed by scatterfile   
	config.heap_limit = config.heap_base + 0x100000;                                                      
//    config.stack_base = SP;   // inherit SP from the execution environment
    config.stack_base = (unsigned int)&top_of_stacks;   // inherit SP from the execution environment
	
	dummy_func(); // This dummy function was used to ensure the vector.s could be linked

    return config;
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
