/*
 * arch/arm/mach-ixp425/ixp425-time.c
 *
 * Timer tick for IXP425 based sytems. We use OS timer1 on the CPU.
 *
 * Author:  Peter Barry
 * Copyright:   (C) 2001 Intel Corporation.
 *
 * Maintainer: Deepak Saxena <dsaxena@mvista.com>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */


#include <linux/config.h>
#include <linux/module.h>
#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/time.h>
#include <linux/init.h>
#include <linux/smp.h>

#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/irq.h>

#include <linux/timex.h>
#include <asm/hardware.h>


extern int setup_arm_irq(int, struct irqaction *);

/* IRQs are disabled before entering here from do_gettimeofday() */
static unsigned long ixp425_gettimeoffset(void)
{
	u32 elapsed, usec, curr, reload;

	/* 
	 * We need elapsed timer ticks since last interrupt
	 * 
	 * Read the CCNT value.  The returned value is 
	 * between -LATCH and 0, 0 corresponding to a full jiffy 
	 */

	reload = *IXP425_OSRT1 & ~IXP425_OST_RELOAD_MASK;
	curr = *IXP425_OST1;

	/* Corner case when rolling over as int disabled ?? */
	elapsed = reload - curr;

	/* Now convert them to usec */
	usec = (unsigned long)(elapsed * tick) / LATCH;

	return usec;
}

static void ixp425_timer_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
	/* Clear Pending Interrupt by writing '1' to it */
	*IXP425_OSST = IXP425_OSST_TIMER_1_PEND;

	do_timer(regs);
}

extern unsigned long (*gettimeoffset)(void);

static struct irqaction timer_irq = {
	name: "IXP425 Timer 1",
};

void __init setup_timer(void)
{
	gettimeoffset = ixp425_gettimeoffset;
	timer_irq.handler = ixp425_timer_interrupt;

	/* Clear Pending Interrupt by writing '1' to it */
	*IXP425_OSST = IXP425_OSST_TIMER_1_PEND;

	/* Setup the Timer counter value */
	*IXP425_OSRT1 = (LATCH & ~IXP425_OST_RELOAD_MASK) | IXP425_OST_ENABLE;

	/* Connect the interrupt handler and enable the interrupt */
	setup_arm_irq(IRQ_IXP425_TIMER1, &timer_irq);
}
