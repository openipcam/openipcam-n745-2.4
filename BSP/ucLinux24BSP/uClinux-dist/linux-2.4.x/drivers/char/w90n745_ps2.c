/****************************************************************************
 *                                                                                                                           *
 * Copyright (c) 2004 - 2006 Winbond Electronics Corp. All rights reserved.                          *
 *                                                                                                                           *
 ***************************************************************************/
 
/****************************************************************************
 * 
 * FILENAME
 *     linux-2.4.x/drivers/char/w90n745_ps2.c
 *
 * VERSION
 *     1.0
 *
 * DESCRIPTION
 *     This file is the winbond ps2 keyboard driver
 *
 * HISTORY
 *     1/9/2005		 Ver 1.0 Created by PC34 MCLi
 *
 * REMARK
 *     None
 **************************************************************************/
 
#include <linux/config.h>


#include <linux/spinlock.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
#include <linux/tty.h>
#include <linux/mm.h>
#include <linux/signal.h>
#include <linux/init.h>
#include <linux/kbd_ll.h>
#include <linux/delay.h>
#include <linux/random.h>
#include <linux/poll.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>
#include <linux/kbd_kern.h>
#include <linux/vt_kern.h>
#include <linux/smp_lock.h>
#include <linux/kd.h>
#include <linux/pm.h>

#undef KBD_DATA_REG
#undef KBD_STATUS_REG	
#undef KBD_CNTL_REG 	

#include "keyboard.h"
#include "w90n745_ps2.h"

#include <asm/bitops.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <asm/system.h>

#include <asm/io.h>

/* Some configuration switches are present in the include file... */



/* Simple translation table for the SysRq keys */

static void kbd_write_command_w(int data);
static void kbd_write_output_w(int data);
#ifndef kbd_controller_present
#define kbd_controller_present()	1
#endif
static spinlock_t kbd_controller_lock = SPIN_LOCK_UNLOCKED;
static unsigned char handle_kbd_event(void);

/* used only by send_data - set by keyboard_interrupt */
static volatile unsigned char reply_expected;
static volatile unsigned char acknowledge;
static volatile unsigned char resend;

/*
 * Wait for keyboard controller input buffer to drain.
 *
 * Don't use 'jiffies' so that we don't depend on
 * interrupts..
 *
 * Quote from PS/2 System Reference Manual:
 *
 * "Address hex 0060 and address hex 0064 should be written only when
 * the input-buffer-full bit and output-buffer-full bit in the
 * Controller Status register are set 0."
 */

unsigned char IsExt = 0;
unsigned char IsLED = 0;
unsigned char CapsLock = 0;
unsigned char gvLEDCode = 0;       /* bit 2 ~ 0 : Capslock, 1:NumLock, 0:ScrollLock */

int IsLEDKeyPress(unsigned int code)
{
	int value = 0;

	if (code == 0x77)
	{
		value = 1;
		if (gvLEDCode & 0x02) 
			gvLEDCode &= 0xFD;
		else				  
			gvLEDCode |= 0x02;
	}	
	else if (code == 0x7E)
	{
		value = 1;
		if (gvLEDCode & 0x01) gvLEDCode &= 0xFE;
		else				  gvLEDCode |= 0x01;
	}
	else if (code == 0x58)
	{
		value = 1;
		if (gvLEDCode & 0x04) gvLEDCode &= 0xFB;
		else				  gvLEDCode |= 0x04;
	}
	
	return value;
}

int SendKBCommand(unsigned char cmd)
{	
	 unsigned volatile int  mctl;
   unsigned volatile char scancode;
   unsigned long timeout = KBD_TIMEOUT*5;
   
	DWORD_WRITE(KBD_CNTL_REG, (cmd | 0x100));
	mctl=DWORD_READ(KBD_CNTL_REG) & 0x100;
	while (mctl) 
	{
		if (!--timeout) {
				printk("keyboard: Timeout - AT keyboard not present?(%02x)\n", cmd);

				return 0;
			}
			
			mctl=DWORD_READ(KBD_CNTL_REG) & 0x100;
	}
		
	while (1)
	{
		if (!--timeout) {
				printk("keyboard: Timeout - AT keyboard not present?(%02x)\n", cmd);

				return 0;
			}
		scancode=DWORD_READ(PS2_SCANCODE_REG)&0xFF;	
	//	printk("scancode:%x\n",scancode);
		if (scancode == KBD_REPLY_ACK)
			break;
	}	
	
	return 1;
}

static void kb_wait(void)
{
	unsigned long timeout = KBC_TIMEOUT;

	do {
		/*
		 * "handle_kbd_event()" will handle any incoming events
		 * while we wait - keypresses or mouse movement.
		 */
		unsigned char status = handle_kbd_event();
		
		if(CapsLock)
			status=0x14;

		if (! (status & KBD_STAT_IBF))
			return;

		mdelay(1);
		timeout--;
	} while (timeout);
#ifdef KBD_REPORT_TIMEOUTS
	printk(KERN_WARNING "Keyboard timed out[1]\n");
#endif
}

/*
 * Translation of escaped scancodes to keycodes.
 * This is now user-settable.
 *
 */
#define E0_KPENTER 90
#define E0_RCTRL   20
#define E0_KPSLASH 74
#define E0_PRSCR   99
#define E0_RALT    100
#define E0_BREAK   101  /* (control-pause) */
#define E0_HOME    72
#define E0_UP      103
#define E0_PGUP    104
#define E0_LEFT    128
#define E0_RIGHT   106
#define E0_END     129
#define E0_DOWN    130
#define E0_PGDN    109
#define E0_INS     110
#define E0_DEL     111

#define E1_PAUSE   138

/*
 * They could be thrown away (and all occurrences below replaced by 0),
 * but that would force many users to use the `setkeycodes' utility, where
 * they needed not before. It does not matter that there are duplicates, as
 * long as no duplication occurs for any single keyboard.
 */
#define SC_LIM 89

#define FOCUS_PF1 85           /* actual code! */
#define FOCUS_PF2 89
#define FOCUS_PF3 90
#define FOCUS_PF4 91
#define FOCUS_PF5 92
#define FOCUS_PF6 93
#define FOCUS_PF7 94
#define FOCUS_PF8 95
#define FOCUS_PF9 120
#define FOCUS_PF10 121
#define FOCUS_PF11 122
#define FOCUS_PF12 123

#define JAP_86     124
/* mcli2@winbond.com.tw:
 * The four keys are located over the numeric keypad, and are
 * labelled A1-A4. It's an rc930 keyboard, from
 * Regnecentralen/RC International, Now ICL.
 * Scancodes: 59, 5a, 5b, 5c.
 */
#define RGN1 124
#define RGN2 125
#define RGN3 126
#define RGN4 127

static unsigned char high_keys[128 - SC_LIM] = {
  RGN1, RGN2, RGN3, RGN4, 0, 0, 0,                   /* 0x69-0x6f */
  0, 0, 0, 0, 0, 0, 0, 0,                            /* 0x70-0x77 */
  0, 0, 0, 0, 0, FOCUS_PF11, 0, FOCUS_PF12,          /* 0x78-0x7f */
  0, 0, 0, FOCUS_PF2, FOCUS_PF9, 0, 0, FOCUS_PF3,    /* 0x80-0x87 */
  FOCUS_PF4, FOCUS_PF5, FOCUS_PF6, FOCUS_PF7,        /* 0x88-0x8b */
  FOCUS_PF8, JAP_86, FOCUS_PF10, 0                   /* 0x8c-0x8f */
};

/* BTC */
#define E0_MACRO   132
/* LK450 */
#define E0_F13     133
#define E0_F14     134
#define E0_HELP    135
#define E0_DO      136
#define E0_F17     137
#define E0_KPMINPLUS 138

/*
 * My OmniKey generates e0 4c for  the "OMNI" key and the
 * right alt key does nada. [mcli2@winbond.com.tw]
 */
#define E0_OK	144
/*
 * New microsoft keyboard is rumoured to have
 * 1F (left window button), 27 (right window button),
 * 2F (menu button). [or: LBANNER, RBANNER, RMENU]
 * [or: Windows_L, Windows_R, TaskMan]
 */
#define E0_MSLW	125
#define E0_MSRW	126
#define E0_MSTM	127

static unsigned char e0_keys[144] = {
  0, 0, 0, 0, 0, 0, 0, 0,			      											/* 0x00-0x07 */
  0, 0, 0, 0, 0, 0, 0, 0,			      											/* 0x08-0x0f */
  0, 0, 0, 0, E0_RCTRL, 0, 0, 0,			      							/* 0x10-0x17 */
  0, 0, 0, 0, 0, 0, 0, 0,	      													/* 0x18-0x1f */
  0, 0, 0, 0, 0, 0, 0, 0,			      											/* 0x20-0x27 */
  0, 0, 0, 0, 0, 0, 0, 0,			      											/* 0x28-0x2f */
  0, 0, 0, 0, 0, 0, 0, E0_PRSCR,	      									/* 0x30-0x37 */
  E0_RALT, 0, 0, 0, 0, 0, 0, 0,	      										/* 0x38-0x3f */
  0, 0, 0, 0, 0, 0, E0_BREAK, E0_MSTM,	      						/* 0x40-0x47 */
  E0_HOME, 0, E0_KPSLASH, 0, 0, 0, 0, 0,									/* 0x48-0x4f */
  0, 0, 0, 0, 0, 0, 0, 0,	      													/* 0x50-0x57 */
  0, 0, E0_KPENTER, 0, E0_MSRW, 0, 0, 0,	      					/* 0x58-0x5f */
  0, 0, E0_MSLW, 0, 0, 0, 0, 0,			      								/* 0x60-0x67 */
  0, E0_END, 0, E0_LEFT, 0, 0, 0, E0_MACRO,		      			/* 0x68-0x6f */
  E0_INS, E0_DEL, E0_DOWN, E0_OK, E0_RIGHT, 0, E0_UP, 0,	/* 0x70-0x77 */
  0, E0_KPMINPLUS, E0_PGDN, 0, E0_PGUP, 0, 0, 0,			    /* 0x78-0x7f */
  0, 0, 0, 0, 0, E0_F13, E0_F14, E0_HELP,			      			/* 0x80-0x87 */
  E0_DO, E0_F17, 0, 0, 0, 0, 0, 0			      							/* 0x88-0x8F */
};

char temp=0;

int pckbd_setkeycode(unsigned int scancode, unsigned int keycode)
{
	if (scancode < SC_LIM || scancode > 255 || keycode > 143)
	  return -EINVAL;
	if (scancode < 144)
	  high_keys[scancode - SC_LIM] = keycode;
	else
	  e0_keys[scancode - 144] = keycode;
	  
	return 0;
}


int pckbd_getkeycode(unsigned int scancode)
{
	return
	  (scancode < SC_LIM || scancode > 255) ? -EINVAL :
	  (scancode < 144) ? high_keys[scancode - SC_LIM] :
	    e0_keys[scancode - 144];
}

static int do_acknowledge(unsigned char scancode)
{
	if (reply_expected) {
	  /* Unfortunately, we must recognise these codes only if we know they
	   * are known to be valid (i.e., after sending a command), because there
	   * are some brain-damaged keyboards (yes, FOCUS 9000 again) which have
	   * keys with such codes :(
	   */
		if (scancode == KBD_REPLY_ACK) {
			acknowledge = 1;
			reply_expected = 0;
			return 0;
		} else if (scancode == KBD_REPLY_RESEND) {
			resend = 1;
			reply_expected = 0;
			return 0;
		}
	}
	
	return 1;
}

int pckbd_translate(unsigned char scancode, unsigned char *keycode, char raw_mode)
{
	static int prev_scancode;

	/* special prefix scancodes.. */
	if (scancode == 0xe0 || scancode == 0xe1) {
		prev_scancode = scancode;
		return 0;
	}
	
	/* 0xFF is sent by a few keyboards, ignore it. 0x00 is error */
	if (scancode == 0x00 || scancode == 0xff) {
		prev_scancode = 0;
		return 0;
	}

	scancode &= 0x7f;
	
	if (prev_scancode) {
	  /*
	   * usually it will be 0xe0, but a Pause key generates
	   * e1 14 77 e1 14 77 when pressed, and nothing when released
	   */
	  if (prev_scancode != 0xe0) {
	      if (prev_scancode == 0xe1 && scancode == 0x14) { 
		  prev_scancode = 0x100;
		  return 0;
	      } else if (prev_scancode == 0x100 && scancode == 0x77) {
		  *keycode = E1_PAUSE;
       prev_scancode = 0;
	      } else {
#ifdef KBD_REPORT_UNKN
		  if (!raw_mode)
		    printk(KERN_INFO "keyboard: unknown e1 escape sequence\n");
#endif
		  prev_scancode = 0;
		  return 0;
	      }
	  } else {
	      prev_scancode = 0;
	      /*
	       *  The keyboard maintains its own internal caps lock and
	       *  num lock statuses. In caps lock mode E0 AA precedes make
	       *  code and E0 2A follows break code. In num lock mode,
	       *  E0 2A precedes make code and E0 AA follows break code.
	       *  We do our own book-keeping, so we will just ignore these.
	       */
	      /*
	       *  For my keyboard there is no caps lock mode, but there are
	       *  both Shift-L and Shift-R modes. The former mode generates
	       *  E0 2A / E0 AA pairs, the latter E0 B6 / E0 36 pairs.
	       *  So, we should also ignore the latter. - aeb@cwi.nl
	       */
	      if (scancode == 0x12 || scancode == 0x59)
					return 0;

	      if (e0_keys[scancode])
					*keycode = e0_keys[scancode];
	      else {
#ifdef KBD_REPORT_UNKN
		  if (!raw_mode)
		    printk(KERN_INFO "keyboard: unknown scancode e0 %02x\n",
			   scancode);
#endif
		  return 0;
	      }
	  }
	}
 	else
	  *keycode = scancode;
	  
 	return 1;
}

char pckbd_unexpected_up(unsigned char keycode)
{
	/* unexpected, but this can happen: maybe this was a key release for a
	   FOCUS 9000 PF key; if we want to see it, we have to clear up_flag */
	if (keycode >= 144 || keycode == 96)
	    return 0;
	else
	    return 0200;
}

int pckbd_pm_resume(struct pm_dev *dev, pm_request_t rqst, void *data) 
{
       return 0;
}


static inline void handle_mouse_event(unsigned char scancode)
{
}

static unsigned char kbd_exists = 1;

static inline void handle_keyboard_event(unsigned int scancode)
{	
#ifdef CONFIG_VT
	kbd_exists = 1;
	if (do_acknowledge(scancode&0xFF))
		handle_scancode(scancode&0xFF, !(scancode & 0x200));
#endif				
	tasklet_schedule(&keyboard_tasklet);
}	

/*
 * This reads the keyboard status port, and does the
 * appropriate action.
 *
 * It requires that we hold the keyboard controller
 * spinlock.
 */

static unsigned char handle_kbd_event(void)
{
		unsigned int  scancode;
		unsigned char aciicode;
		static unsigned char led_count=0;
		unsigned int status = kbd_read_status();
		
		IsExt=CapsLock=0;		
		
		if (status & 0x01)
	  {	  	 			
	  		scancode = DWORD_READ(KBD_DATA_REG);		
	 		  aciicode = DWORD_READ(PS2_ASCII_REG) & 0xFF;
	 		  
	 		  if(scancode & 0x100)
	 		  	IsExt=1;
	 		  
	 		  if(scancode==0x58)
	 		  	CapsLock=1;
	 		  		 		  
//	 	  printk("scancode:%x\n",scancode);	 		  
	
	 		  IsLEDKeyPress(scancode); 	 		  

			  handle_keyboard_event(scancode);			  
			  DWORD_WRITE(KBD_STATUS_REG, 0x01); //clear the status				 
	  }
	  else if (status & 0x10)
	  	  DWORD_WRITE(KBD_STATUS_REG, 0x10); //clear the status
	  else if (status & 0x20)
	  	  printk("WARNING ! TX_ERR Bit is set.\n");

		return status;
}

static void keyboard_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
#ifdef CONFIG_VT
	kbd_pt_regs = regs;
#endif
	spin_lock_irq(&kbd_controller_lock);
	handle_kbd_event();
	spin_unlock_irq(&kbd_controller_lock);
}

/*
 * send_data sends a character to the keyboard and waits
 * for an acknowledge, possibly retrying if asked to. Returns
 * the success status.
 *
 * Don't use 'jiffies', so that we don't depend on interrupts
 */
static int send_data(unsigned char data)
{
	int retries = 3;
	unsigned int scancode;

	do {
		unsigned long timeout = KBD_TIMEOUT;

		acknowledge = 0; /* Set by interrupt routine on receipt of ACK. */
		resend = 0;
		reply_expected = 1;
		kbd_write_output_w(data);
		
		
		if(!(DWORD_READ(KBD_CNTL_REG) & 0x100))
			acknowledge=1;
		
		scancode=DWORD_READ(PS2_SCANCODE_REG)&0xFF;
		if(scancode==KBD_REPLY_ACK)
			return 1;
			
		for (;;) {
			if (acknowledge)
				return 1;
			if (resend)
				break;
			mdelay(1);
			if (!--timeout) {
#ifdef KBD_REPORT_TIMEOUTS
				printk(KERN_WARNING "keyboard: Timeout - AT keyboard not present?(%02x)\n", data);
#endif
				return 0;
			}
		}
	} while (retries-- > 0);
#ifdef KBD_REPORT_TIMEOUTS
	printk(KERN_WARNING "keyboard: Too many NACKs -- noisy kbd cable?\n");
#endif
	return 0;
}

void pckbd_leds(unsigned char leds)
{	
#if 0	
	if (kbd_exists && (!send_data(KBD_CMD_SET_LEDS) || !send_data(leds))) {
		send_data(KBD_CMD_ENABLE);	/* re-enable kbd if any errors */
		kbd_exists = 0;
	}	
#else
	SendKBCommand(KBD_CMD_SET_LEDS);
	SendKBCommand(leds);
#endif	
}

#define DEFAULT_KEYB_REP_DELAY	250
#define DEFAULT_KEYB_REP_RATE		30	/* cps */

static struct kbd_repeat kbdrate={
	DEFAULT_KEYB_REP_DELAY,
	DEFAULT_KEYB_REP_RATE
};

static unsigned char parse_kbd_rate(struct kbd_repeat *r)
{
	static struct r2v{
		int rate;
		unsigned char val;
	} kbd_rates[]={	{5,0x14},
			{7,0x10},
			{10,0x0c},
			{15,0x08},
			{20,0x04},
			{25,0x02},
			{30,0x00}
	};
	static struct d2v{
		int delay;
		unsigned char val;
	} kbd_delays[]={{250,0},
			{500,1},
			{750,2},
			{1000,3}
	};
	int rate=0,delay=0;
	if (r != NULL){
		int i,new_rate=30,new_delay=250;
		if (r->rate <= 0)
			r->rate=kbdrate.rate;
		if (r->delay <= 0)
			r->delay=kbdrate.delay;
		for (i=0; i < sizeof(kbd_rates)/sizeof(struct r2v); i++)
			if (kbd_rates[i].rate == r->rate){
				new_rate=kbd_rates[i].rate;
				rate=kbd_rates[i].val;
				break;
			}
		for (i=0; i < sizeof(kbd_delays)/sizeof(struct d2v); i++)
			if (kbd_delays[i].delay == r->delay){
				new_delay=kbd_delays[i].delay;
				delay=kbd_delays[i].val;
				break;
			}
		r->rate=new_rate;
		r->delay=new_delay;
	}
	return (delay << 5) | rate;
}

static int write_kbd_rate(unsigned char r)
{
	if (!send_data(KBD_CMD_SET_RATE) || !send_data(r)){
		send_data(KBD_CMD_ENABLE); 	/* re-enable kbd if any errors */
		return 0; 
	}else
		return 1;
}

static int pckbd_rate(struct kbd_repeat *rep)
{
	if (rep == NULL)
		return -EINVAL;
	else{
		unsigned char r=parse_kbd_rate(rep);
		struct kbd_repeat old_rep;
		memcpy(&old_rep,&kbdrate,sizeof(struct kbd_repeat));
		if (write_kbd_rate(r)){
			memcpy(&kbdrate,rep,sizeof(struct kbd_repeat));
			memcpy(rep,&old_rep,sizeof(struct kbd_repeat));
			return 0;
		}
	}
	return -EIO;
}

#define KBD_NO_DATA	(-1)	/* No data */
#define KBD_BAD_DATA	(-2)	/* Parity or other error */

static int __init kbd_read_data(void)
{  
	int retval = KBD_NO_DATA;
	unsigned int status;

	status = kbd_read_status();
	
	if (status & KBD_STAT_OBF) {
		unsigned char data = kbd_read_input();

		retval = data;
		if (status & (KBD_STAT_GTO | KBD_STAT_PERR))
			retval = KBD_BAD_DATA;
	}
	
	return retval;
}  

static void __init kbd_clear_input(void)
{
	int maxread = 100;	/* Random number */

	do {
		if (kbd_read_data() == KBD_NO_DATA)
			break;
	} while (--maxread);
}

static int __init kbd_wait_for_input(void)
{
	long timeout = KBD_INIT_TIMEOUT;

	do {
		int retval = kbd_read_data();
		if (retval >= 0)
		{
		
			return retval;
		}	
		mdelay(1);
	} while (--timeout);
	return -1;
}

static void kbd_write_command_w(int data)
{
	unsigned long flags;

	spin_lock_irqsave(&kbd_controller_lock, flags);
	kb_wait();
	kbd_write_command(data);
	spin_unlock_irqrestore(&kbd_controller_lock, flags);
}

static void kbd_write_output_w(int data)
{
	unsigned long flags;

	spin_lock_irqsave(&kbd_controller_lock, flags);
	kb_wait();
	kbd_write_output(data);
	spin_unlock_irqrestore(&kbd_controller_lock, flags);
}
 
#if defined(__alpha__)
/*
 * Some Alphas cannot mask some/all interrupts, so we have to
 * make sure not to allow interrupts AT ALL when polling for
 * specific return values from the keyboard.
 *
 * I think this should work on any architecture, but for now, only Alpha.
 */
static int kbd_write_command_w_and_wait(int data)
{
	unsigned long flags;
	int input;

	spin_lock_irqsave(&kbd_controller_lock, flags);
	kb_wait();
	kbd_write_command(data);
	input = kbd_wait_for_input();
	spin_unlock_irqrestore(&kbd_controller_lock, flags);
	return input;
}

static int kbd_write_output_w_and_wait(int data)
{
	unsigned long flags;
	int input;

	spin_lock_irqsave(&kbd_controller_lock, flags);
	kb_wait();
	kbd_write_output(data);
	input = kbd_wait_for_input();
	spin_unlock_irqrestore(&kbd_controller_lock, flags);
	return input;
}
#else
static int kbd_write_command_w_and_wait(int data)
{
	kbd_write_command_w(data);
	return kbd_wait_for_input();
}

static int kbd_write_output_w_and_wait(int data)
{
	kbd_write_output_w(data);
	return kbd_wait_for_input();
}
#endif /* __alpha__ */

void __init pckbd_init_hw(void)
{ 
	/* change GPIO5 [4:5] to PS2 signals */
	DWORD_WRITE(0xFFF83050, DWORD_READ(0xFFF83050)|0xF00);
	
	/* Flush any pending input. */
	kbd_clear_input();

	kbd_rate = pckbd_rate;

	if(request_irq(INT_PS2, keyboard_interrupt, SA_SHIRQ, "keyboard", &temp))
	    printk("keyboard request_irq error !\n");
	else
	    printk("keyboard request_irq OK !\n");
}

static int blink_frequency = HZ/2;

/* Tell the user who may be running in X and not see the console that we have 
   panic'ed. This is to distingush panics from "real" lockups. 
   Could in theory send the panic message as morse, but that is left as an
   exercise for the reader.  */ 
void panic_blink(void)
{ 
	static unsigned long last_jiffie;
	static char led;
	/* Roughly 1/2s frequency. KDB uses about 1s. Make sure it is 
	   different. */
	if (!blink_frequency) 
		return;
	if (jiffies - last_jiffie > blink_frequency) {
		led ^= 0x01 | 0x04;
		while (kbd_read_status() & KBD_STAT_IBF) mdelay(1); 
		kbd_write_output(KBD_CMD_SET_LEDS);
		mdelay(1); 
		while (kbd_read_status() & KBD_STAT_IBF) mdelay(1); 
		mdelay(1); 
		kbd_write_output(led);
		last_jiffie = jiffies;
	}
}  

static int __init panicblink_setup(char *str)
{
    int par;
    if (get_option(&str,&par)) 
	    blink_frequency = par*(1000/HZ);
    return 1;
}

/* panicblink=0 disables the blinking as it caused problems with some console
   switches. otherwise argument is ms of a blink period. */
__setup("panicblink=", panicblink_setup);
