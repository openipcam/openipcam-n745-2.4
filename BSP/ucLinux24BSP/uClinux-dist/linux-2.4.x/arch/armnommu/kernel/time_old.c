/*
 *  linux/arch/arm/kernel/time.c
 *
 *  Copyright (C) 1991, 1992, 1995  Linus Torvalds
 *  Modifications for ARM (C) 1994, 1995, 1996,1997 Russell King
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *  This file contains the ARM-specific time handling details:
 *  reading the RTC at bootup, etc...
 *
 *  1994-07-02  Alan Modra
 *              fixed set_rtc_mmss, fixed time.year for >= 2000, new mktime
 *  1998-12-20  Updated NTP code according to technical memorandum Jan '96
 *              "A Kernel Model for Precision Timekeeping" by Dave Mills
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
extern void setup_timer(void);
extern rwlock_t xtime_lock;
extern unsigned long wall_jiffies;

/* change this if you have some constant time drift */
#define USECS_PER_JIFFY	(1000000/HZ)

#ifndef BCD_TO_BIN
#define BCD_TO_BIN(val) ((val)=((val)&15) + ((val)>>4)*10)
#endif

#ifndef BIN_TO_BCD
#define BIN_TO_BCD(val) ((val)=(((val)/10)<<4) + (val)%10)
#endif

static int dummy_set_rtc(void)
{
	return 0;
}

/*
 * hook for setting the RTC's idea of the current time.
 */
int (*set_rtc)(void) = dummy_set_rtc;

static unsigned long dummy_gettimeoffset(void)
{
	return 0;
}

/*
 * hook for getting the time offset.  Note that it is
 * always called with interrupts disabled.
 */
unsigned long (*gettimeoffset)(void) = dummy_gettimeoffset;

/*
 * Handle kernel profile stuff...
 */
static inline void do_profile(struct pt_regs *regs)
{
	if (!user_mode(regs) &&
	    prof_buffer &&
	    current->pid) {
		unsigned long pc = instruction_pointer(regs);
		extern int _stext;

		pc -= (unsigned long)&_stext;

		pc >>= prof_shift;

		if (pc >= prof_len)
			pc = prof_len - 1;

		prof_buffer[pc] += 1;
	}
}

static long next_rtc_update;

/*
 * If we have an externally synchronized linux clock, then update
 * CMOS clock accordingly every ~11 minutes.  set_rtc() has to be
 * called as close as possible to 500 ms before the new second
 * starts.
 */
static inline void do_set_rtc(void)
{
	if (time_status & STA_UNSYNC || set_rtc == NULL)
		return;

	if (next_rtc_update &&
	    time_before(xtime.tv_sec, next_rtc_update))
		return;

	if (xtime.tv_usec < 50000 - (tick >> 1) &&
	    xtime.tv_usec >= 50000 + (tick >> 1))
		return;

	if (set_rtc())
		/*
		 * rtc update failed.  Try again in 60s
		 */
		next_rtc_update = xtime.tv_sec + 60;
	else
		next_rtc_update = xtime.tv_sec + 660;
}

#ifdef CONFIG_LEDS

#include <asm/leds.h>

static void dummy_leds_event(led_event_t evt)
{
}

void (*leds_event)(led_event_t) = dummy_leds_event;

#ifdef CONFIG_MODULES
EXPORT_SYMBOL(leds_event);
#endif
#endif

#ifdef CONFIG_LEDS_TIMER
static void do_leds(void)
{
	static unsigned int count = 50;

	if (--count == 0) {
		count = 50;
		leds_event(led_timer);
	}
}
#else
#define do_leds()
#endif

void do_gettimeofday(struct timeval *tv)
{
	unsigned long flags;
	unsigned long usec, sec;

	read_lock_irqsave(&xtime_lock, flags);
	usec = gettimeoffset();
	{
		unsigned long lost = jiffies - wall_jiffies;

		if (lost)
			usec += lost * USECS_PER_JIFFY;
	}
	sec = xtime.tv_sec;
	usec += xtime.tv_usec;
	read_unlock_irqrestore(&xtime_lock, flags);

	/* usec may have gone up a lot: be safe */
	while (usec >= 1000000) {
		usec -= 1000000;
		sec++;
	}

	tv->tv_sec = sec;
	tv->tv_usec = usec;
}

void do_settimeofday(struct timeval *tv)
{
	write_lock_irq(&xtime_lock);
	/* This is revolting. We need to set the xtime.tv_usec
	 * correctly. However, the value in this location is
	 * is value at the last tick.
	 * Discover what correction gettimeofday
	 * would have done, and then undo it!
	 */
	tv->tv_usec -= gettimeoffset();
	tv->tv_usec -= (jiffies - wall_jiffies) * USECS_PER_JIFFY;

	while (tv->tv_usec < 0) {
		tv->tv_usec += 1000000;
		tv->tv_sec--;
	}

	xtime = *tv;
	time_adjust = 0;		/* stop active adjtime() */
	time_status |= STA_UNSYNC;
	time_maxerror = NTP_PHASE_LIMIT;
	time_esterror = NTP_PHASE_LIMIT;
	write_unlock_irq(&xtime_lock);
}

static struct irqaction timer_irq = {
	name: "timer",
};

/*
 * Include architecture specific code
 */
#include <asm/arch/time.h>
spinlock_t rtc_lock;		/* serialize CMOS RAM access */
#include <linux/rtc710.h>//lsshi add 2005-4-18 15:52

static void rtc_check()
{
	int i;
	
	DWORD_WRITE(INIR,INIRRESET);
	DWORD_WRITE(AER,AERPOWERON);//at power on
	
	for (i = 0 ; i < 1000000 ; i++)	/* may take up to 1 second... */
	{
		if (CMOS_READ(INIR) & RTCSET)
		{
			break;
		}
	}//for end
	
	for (i = 0 ; i < 1000000 ; i++)	/* may take up to 1 second... */
	{
		if (CMOS_READ(AER) & AERRWENB)
		{
			//printk("AER R/W enabled %d !\n",i);
			break;
		}
		
	}//for end

	
	
}

#ifdef CONFIG_W90N745

#define YEAR2000		2000

unsigned long get_710_rtc_time(void) //lsshi add 2005-4-18 15:10
{
	unsigned int year, mon, day, hour, min, sec;

	spin_lock(&rtc_lock);

	rtc_check();
	
	//do { /* Isn't this overkill ? UIP above should guarantee consistency */
	sec = RTC_SECONDS;
	min = RTC_MINUTES;
	hour = RTC_HOURS;
	day = RTC_DAY_OF_MONTH;
	mon = RTC_MONTH;
	year = RTC_YEAR;
#if 0	
	BCD_TO_BIN(sec);
	BCD_TO_BIN(min);
	BCD_TO_BIN(hour);
	BCD_TO_BIN(day);
	BCD_TO_BIN(mon);
	BCD_TO_BIN(year);
#endif
	spin_unlock(&rtc_lock);

	year += YEAR2000;
	
	//printk("W90N745 RTC: year %d mon %d day %d hour %d min %d sec %d\n",(year),
	// (mon), (day), (hour), (min), (sec));
	
	return mktime(year, mon, day, hour, min, sec);
}
#endif




/*
 * This must cause the timer to start ticking.
 * It doesn't have to set the current time though
 * from an RTC - it can be done later once we have
 * some buses initialised.
 */
#ifdef CONFIG_W90N745

#define SECS *HZ
#define MINS * 60 SECS
#define HOURS * 60 MINS
#define DAYS * 24 HOURS

#define UPDATE_TIME	60 MINS

static void Syn_Timeout(unsigned long arg);

struct timer_list syntimer;

static void Syn_Timeout(unsigned long arg)
{
	write_lock_irq(&xtime_lock);

	xtime.tv_sec  = get_710_rtc_time(); // lsshi 2005-4-18 16:13
	
	del_timer(&syntimer);
	syntimer.expires =jiffies+UPDATE_TIME;
	add_timer(&syntimer);
	
	write_unlock_irq(&xtime_lock);
}

#endif

void __init time_init(void)
{
	xtime.tv_usec = 0;
#ifdef CONFIG_W90N745
	xtime.tv_sec  = get_710_rtc_time();//get_w83977_rtc_time(); 2005-4-18 16:06
#else
	xtime.tv_sec  = 0;
#endif

	setup_timer();

#ifdef CONFIG_W90N745
	init_timer(&syntimer);
	syntimer.expires =jiffies+UPDATE_TIME;
	syntimer.data = (unsigned long) NULL;
	syntimer.function = &Syn_Timeout;

	add_timer(&syntimer);
#endif

}
