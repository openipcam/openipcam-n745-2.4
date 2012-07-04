/******************************************************************************
 *
 * Copyright (c) 2003 Windond Electronics Corp.
 * All rights reserved.
 *
 * $Workfile: timer.c $
 *
 * $Author: Yachen $
 ******************************************************************************/
/*
 * $History: timer.c $
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
#include "cdefs.h"
#include "platform.h"

extern void EnableIRQ(void);
extern void DisableIRQ(void);
extern void timer_handler(void);
unsigned int clkTck=0;
const unsigned int CLK_TCK=100;

/*
void EnableIRQ(void)
{
	int tmp;
    __asm
    {
        MRS    tmp, CPSR
        BIC    tmp, tmp,0x80
        MSR    CPSR_c, tmp
    }
}

void DisableIRQ(void)
{
	int tmp;
    __asm
    {
        MRS    tmp, CPSR
        ORR    tmp, tmp,0x80
        MSR    CPSR_c, tmp
    }
}
*/
/*
__irq void timer_handler()
{
	clkTck++;
	*((volatile unsigned int *)T_TISR)=0xFFFFFFFE;
}
*/

void init_timer()
{
	T_TCR0=0x68000000;
	T_TICR0=150000;
	AIC_MECR=(1<<7);
	VPint(0x18)=(unsigned int)0xE59FF018;	// LDR pc, [0x38]
	VPint(0x38)=(unsigned int)timer_handler;	
	EnableIRQ();
}

unsigned int clock()
{
	return clkTck;
}

void sleep(int n)
{
	int i;
	for( i=0;i < n;i++)
	{
		//T_TISR&=(~TIF0);
		T_TISR =TIF0; //cmn, write 1 to clear
		T_TICR0=15000;
		T_TCR0=0x60000000;
		while(!(T_TISR&TIF0));
	}
}
