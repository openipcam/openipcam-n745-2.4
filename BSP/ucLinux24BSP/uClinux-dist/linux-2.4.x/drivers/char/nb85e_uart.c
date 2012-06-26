/*
 * drivers/char/nb85e_uart.c -- Serial I/O using V850E/NB85E on-chip UART
 *
 *  Copyright (C) 2001,2002  NEC Corporation
 *  Copyright (C) 2001,2002  Miles Bader <miles@gnu.org>
 *
 * This file is subject to the terms and conditions of the GNU General
 * Public License.  See the file COPYING in the main directory of this
 * archive for more details.
 *
 * Written by Miles Bader <miles@gnu.org>
 */

#include <linux/kernel.h>
#include <linux/console.h>
#include <linux/tty.h>
#include <linux/tty_flip.h>
#include <linux/tty_driver.h>
#include <linux/serial.h>
#include <linux/generic_serial.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/tqueue.h>
#include <linux/interrupt.h>

#include <asm/system.h>
#include <asm/nb85e_uart.h>
#include <asm/nb85e_utils.h>


/* Initial UART state.  This may be overridden by machine-dependent headers. */
#ifndef NB85E_UART_INIT_BAUD
#define NB85E_UART_INIT_BAUD	115200
#endif
#ifndef NB85E_UART_INIT_CFLAGS
#define NB85E_UART_INIT_CFLAGS	(B115200 | CS8 | CREAD)
#endif

/* A string used for prefixing printed descriptions; since the same UART
   macro is actually used on other chips than the V850E/NB85E.  This must
   be a constant string.  */
#ifndef NB85E_UART_CHIP_NAME
#define NB85E_UART_CHIP_NAME "V850E/NB85E"
#endif


/* Magic number used in generic_serial header.  */
#define NB85E_UART_MAGIC	0xFABCAB22


#define RS_EVENT_WRITE_WAKEUP	1 /* from generic_serial.h */

/* For use by modules eventually...  */
#define MOD_INC_USE_COUNT
#define MOD_DEC_USE_COUNT


/* Low-level UART functions.  */

/* These masks define which control bits affect TX/RX modes, respectively.  */
#define RX_BITS \
  (NB85E_UART_ASIM_PS_MASK | NB85E_UART_ASIM_CL_8 | NB85E_UART_ASIM_ISRM)
#define TX_BITS \
  (NB85E_UART_ASIM_PS_MASK | NB85E_UART_ASIM_CL_8 | NB85E_UART_ASIM_SL_2)

/* The UART require various delays after writing control registers.  */
static inline void nb85e_uart_delay (unsigned cycles)
{
	/* The loop takes 2 insns, so loop CYCLES / 2 times.  */
	register unsigned count = cycles >> 1;
	while (--count != 0)
		/* nothing */;
}

/* Configure and turn on uart channel CHAN, using the termios `control
   modes' bits in CFLAGS, and a baud-rate of BAUD.  */
void nb85e_uart_configure (unsigned chan, unsigned cflags, unsigned baud)
{
	int cksr_min, flags;
	unsigned new_config = 0; /* What we'll write to the control reg. */
	unsigned new_clk_divlog2; /* New baud-rate generate clock divider. */
	unsigned new_brgen_count; /* New counter max for baud-rate generator.*/
	/* These are the current values corresponding to the above.  */
	unsigned old_config, old_clk_divlog2, old_brgen_count;

	/* Calculate new baud-rate generator config values.  */
	cksr_min = 0;
	while ((NB85E_UART_BASE_FREQ >> cksr_min) > NB85E_UART_CKSR_MAX_FREQ)
		cksr_min++;
	/* Calculate the log2 clock divider and baud-rate counter values
	   (note that the UART divides the resulting clock by 2, so
	   multiply BAUD by 2 here to compensate).  */
	calc_counter_params (NB85E_UART_BASE_FREQ, baud * 2,
			     cksr_min, NB85E_UART_CKSR_MAX, 8/*bits*/,
			     &new_clk_divlog2, &new_brgen_count);

	/* Figure out new configuration of control register.  */
	if (cflags & CSTOPB)
		/* Number of stop bits, 1 or 2.  */
		new_config |= NB85E_UART_ASIM_SL_2;
	if ((cflags & CSIZE) == CS8)
		/* Number of data bits, 7 or 8.  */
		new_config |= NB85E_UART_ASIM_CL_8;
	if (! (cflags & PARENB))
		/* No parity check/generation.  */
		new_config |= NB85E_UART_ASIM_PS_NONE;
	else if (cflags & PARODD)
		/* Odd parity check/generation.  */
		new_config |= NB85E_UART_ASIM_PS_ODD;
	else
		/* Even parity check/generation.  */
		new_config |= NB85E_UART_ASIM_PS_EVEN;
	if (cflags & CREAD)
		/* Reading enabled.  */
		new_config |= NB85E_UART_ASIM_RXE;

	new_config |= NB85E_UART_ASIM_TXE; /* Writing is always enabled.  */
	new_config |= NB85E_UART_ASIM_CAE;
	new_config |= NB85E_UART_ASIM_ISRM; /* Errors generate a read-irq.  */

	/* Disable interrupts while we're twiddling the hardware.  */
	save_flags_cli (flags);

#ifdef NB85E_UART_PRE_CONFIGURE
	NB85E_UART_PRE_CONFIGURE (chan, cflags, baud);
#endif

	old_config = NB85E_UART_ASIM (chan);
	old_clk_divlog2 = NB85E_UART_CKSR (chan);
	old_brgen_count = NB85E_UART_BRGC (chan);

	if (new_clk_divlog2 != old_clk_divlog2
	    || new_brgen_count != old_brgen_count)
	{
		/* The baud rate has changed.  First, disable the UART.  */
		NB85E_UART_ASIM (chan) = 0;
		old_config = 0;
		/* Reprogram the baud-rate generator.  */
		NB85E_UART_CKSR (chan) = new_clk_divlog2;
		NB85E_UART_BRGC (chan) = new_brgen_count;
	}

	if (! (old_config & NB85E_UART_ASIM_CAE)) {
		/* If we are enabling the uart for the first time, start
		   by turning on the enable bit, which must be done
		   before turning on any other bits.  */
		NB85E_UART_ASIM (chan) = NB85E_UART_ASIM_CAE;
		/* Enabling the uart also resets it.  */
		old_config = NB85E_UART_ASIM_CAE;
	}

	if (new_config != old_config) {
		/* Which of the TXE/RXE bits we'll temporarily turn off
		   before changing other control bits.  */
		unsigned temp_disable = 0;
		/* Which of the TXE/RXE bits will be enabled.  */
		unsigned enable = 0;
		unsigned changed_bits = new_config ^ old_config;

		/* Which of RX/TX will be enabled in the new configuration.  */
		if (new_config & RX_BITS)
			enable |= (new_config & NB85E_UART_ASIM_RXE);
		if (new_config & TX_BITS)
			enable |= (new_config & NB85E_UART_ASIM_TXE);

		/* Figure out which of RX/TX needs to be disabled; note
		   that this will only happen if they're not already
		   disabled.  */
		if (changed_bits & RX_BITS)
			temp_disable |= (old_config & NB85E_UART_ASIM_RXE);
		if (changed_bits & TX_BITS)
			temp_disable |= (old_config & NB85E_UART_ASIM_TXE);

		/* We have to turn off RX and/or TX mode before changing
		   any associated control bits.  */
		if (temp_disable)
			NB85E_UART_ASIM (chan) = old_config & ~temp_disable;

		/* Write the new control bits, while RX/TX are disabled. */ 
		if (changed_bits & ~enable)
			NB85E_UART_ASIM (chan) = new_config & ~enable;

		/* The UART may not be reset properly unless we
		   wait at least 2 `basic-clocks' until turning
		   on the TXE/RXE bits again.  A `basic clock'
		   is the clock used by the baud-rate generator, i.e.,
		   the cpu clock divided by the 2^new_clk_divlog2.  */
		nb85e_uart_delay (1 << (new_clk_divlog2 + 1));

		/* Write the final version, with enable bits turned on.  */
		NB85E_UART_ASIM (chan) = new_config;
	}

	restore_flags (flags);
}


/*  Low-level console. */

static void nb85e_uart_cons_write (struct console *co,
				   const char *s, unsigned count)
{
	if (count > 0) {
		unsigned chan = co->index;
		unsigned irq = IRQ_INTST (chan);
		int irq_was_enabled, irq_was_pending, flags;

		/* We don't want to get `transmission completed' (INTST)
		   interrupts, since we're busy-waiting, so we disable
		   them while sending (we don't disable interrupts
		   entirely because sending over a serial line is really
		   slow).  We save the status of INTST and restore it
		   when we're done so that using printk doesn't
		   interfere with normal serial transmission (other than
		   interleaving the output, of course!).  This should
		   work correctly even if this function is interrupted
		   and the interrupt printks something.  */

		/* Disable interrupts while fiddling with INTST.  */
		save_flags_cli (flags);
		/* Get current INTST status.  */
		irq_was_enabled = nb85e_intc_irq_enabled (irq);
		irq_was_pending = nb85e_intc_irq_pending (irq);
		/* Disable INTST if necessary.  */
		if (irq_was_enabled)
			nb85e_intc_disable_irq (irq);
		/* Turn interrupts back on.  */
		restore_flags (flags);

		/* Send characters.  */
		while (count > 0) {
			int ch = *s++;

			if (ch == '\n') {
				/* We don't have the benefit of a tty
				   driver, so translate NL into CR LF.  */
				nb85e_uart_wait_for_xmit_ok (chan);
				nb85e_uart_putc (chan, '\r');
			}

			nb85e_uart_wait_for_xmit_ok (chan);
			nb85e_uart_putc (chan, ch);

			count--;
		}

		/* Restore saved INTST status.  */
		if (irq_was_enabled) {
			/* Wait for the last character we sent to be
			   completely transmitted (as we'll get an INTST
			   interrupt at that point).  */
			nb85e_uart_wait_for_xmit_done (chan);
			/* Clear pending interrupts received due
			   to our transmission, unless there was already
			   one pending, in which case we want the
			   handler to be called.  */
			if (! irq_was_pending)
				nb85e_intc_clear_pending_irq (irq);
			/* ... and then turn back on handling.  */
			nb85e_intc_enable_irq (irq);
		}
	}
}

static kdev_t nb85e_uart_cons_device (struct console *c)
{
        return MKDEV (TTY_MAJOR, NB85E_UART_MINOR_BASE + c->index);
}

static struct console nb85e_uart_cons =
{
    name:	"ttyS",
    write:	nb85e_uart_cons_write,
    device:	nb85e_uart_cons_device,
    flags:	CON_PRINTBUFFER,
    index:	-1,
};

void nb85e_uart_cons_init (unsigned chan)
{
	nb85e_uart_configure (chan, NB85E_UART_INIT_CFLAGS,
			      NB85E_UART_INIT_BAUD);
	nb85e_uart_cons.index = chan;
	register_console (&nb85e_uart_cons);
	printk ("Console: %s on-chip UART channel %d\n",
		NB85E_UART_CHIP_NAME, chan);
}


/* Interface for generic serial driver layer.  */

struct nb85e_uart_tty_port {
	struct gs_port gs;
	unsigned chan;
	struct tq_struct tqueue;
};

/* Transmit a character, if any are pending.  */
void nb85e_uart_tty_tx (struct nb85e_uart_tty_port *port)
{
	unsigned chan = port->chan;
	int flags;

	/* If there are characters to transmit, try to transmit one of them. */
	if (port->gs.xmit_cnt > 0 && nb85e_uart_xmit_ok (port->chan)) {
		nb85e_uart_putc (chan, port->gs.xmit_buf[port->gs.xmit_tail]);
		port->gs.xmit_tail
			= (port->gs.xmit_tail + 1) & (SERIAL_XMIT_SIZE - 1);
		port->gs.xmit_cnt--;

		if (port->gs.xmit_cnt <= port->gs.wakeup_chars) {
			port->gs.event |= 1 << RS_EVENT_WRITE_WAKEUP;
			queue_task (&port->tqueue, &tq_immediate);
			mark_bh (IMMEDIATE_BH);
		}
	}

	save_flags_cli (flags);
	if (port->gs.xmit_cnt == 0)
		port->gs.flags &= ~GS_TX_INTEN;
	restore_flags (flags);
}

static void nb85e_uart_tty_disable_tx_interrupts (void *driver_data)
{
	struct nb85e_uart_tty_port *port = driver_data;
	nb85e_intc_disable_irq (IRQ_INTST (port->chan));
}

static void nb85e_uart_tty_enable_tx_interrupts (void *driver_data)
{
	struct nb85e_uart_tty_port *port = driver_data;
	nb85e_intc_disable_irq (IRQ_INTST (port->chan));
	nb85e_uart_tty_tx (port);
	nb85e_intc_enable_irq (IRQ_INTST (port->chan));
}

static void nb85e_uart_tty_disable_rx_interrupts (void *driver_data)
{
	struct nb85e_uart_tty_port *port = driver_data;
	nb85e_intc_disable_irq (IRQ_INTSR (port->chan));
}

static void nb85e_uart_tty_enable_rx_interrupts (void *driver_data)
{
	struct nb85e_uart_tty_port *port = driver_data;
	nb85e_intc_enable_irq (IRQ_INTSR (port->chan));
}

static int nb85e_uart_tty_get_CD (void *driver_data)
{
	return 1;		/* Can't really detect it, sorry... */
}

static void nb85e_uart_tty_shutdown_port (void *driver_data)
{
	struct nb85e_uart_tty_port *port = driver_data;

	port->gs.flags &= ~ GS_ACTIVE;

	/* Disable port interrupts.  */
	free_irq (IRQ_INTST (port->chan), port);
	free_irq (IRQ_INTSR (port->chan), port);

	/* Turn off xmit/recv enable bits.  */
	NB85E_UART_ASIM (port->chan) &= ~(NB85E_UART_ASIM_TXE | NB85E_UART_ASIM_RXE);
	/* Then reset the channel.  */
	NB85E_UART_ASIM (port->chan) = 0;
}

static int nb85e_uart_tty_set_real_termios (void *driver_data)
{
	struct nb85e_uart_tty_port *port = driver_data;
	unsigned cflag = port->gs.tty->termios->c_cflag;
	nb85e_uart_configure (port->chan, cflag, port->gs.baud);
}

static int nb85e_uart_tty_chars_in_buffer (void *driver_data)
{
	/* There's only a one-character `buffer', and the only time we
	   actually know there's a character in it is when we receive a
	   character-received interrupt -- in which case we immediately
	   remove it anyway.  */
	return 0;
}

static void nb85e_uart_tty_close (void *driver_data)
{
	MOD_DEC_USE_COUNT;
}

static void nb85e_uart_tty_hungup (void *driver_data)
{
	MOD_DEC_USE_COUNT;
}

static void nb85e_uart_tty_getserial (void *driver_data, struct serial_struct *s)
{
	struct nb85e_uart_tty_port *port = driver_data;
	s->line = port->chan;
	s->xmit_fifo_size = 1;
	s->irq = IRQ_INTSR (port->chan); /* actually have TX irq too, but... */
}

static struct real_driver nb85e_uart_tty_gs_driver = {
	disable_tx_interrupts: 	nb85e_uart_tty_disable_tx_interrupts,
	enable_tx_interrupts:	nb85e_uart_tty_enable_tx_interrupts,
	disable_rx_interrupts:	nb85e_uart_tty_disable_rx_interrupts,
	enable_rx_interrupts:	nb85e_uart_tty_enable_rx_interrupts,
	get_CD:			nb85e_uart_tty_get_CD,
	shutdown_port:		nb85e_uart_tty_shutdown_port,
	set_real_termios:	nb85e_uart_tty_set_real_termios,
	chars_in_buffer:	nb85e_uart_tty_chars_in_buffer,
	close:			nb85e_uart_tty_close,
	hungup:			nb85e_uart_tty_hungup,
	getserial:		nb85e_uart_tty_getserial,
};

static struct nb85e_uart_tty_port nb85e_uart_tty_ports[NB85E_UART_NUM_CHANNELS];

static void init_nb85e_uart_tty_ports (void)
{
	int chan;
	for (chan = 0; chan < NB85E_UART_NUM_CHANNELS; chan++) {
		struct nb85e_uart_tty_port *port = &nb85e_uart_tty_ports[chan];

		port->chan = chan;

		port->gs.magic = NB85E_UART_MAGIC;
		port->gs.rd = &nb85e_uart_tty_gs_driver;

		init_waitqueue_head (&port->gs.open_wait);
		init_waitqueue_head (&port->gs.close_wait);

		port->gs.normal_termios	= tty_std_termios;
		port->gs.normal_termios.c_cflag = NB85E_UART_INIT_CFLAGS;

		port->gs.close_delay = HZ / 2;   /* .5s */
		port->gs.closing_wait = 30 * HZ; /* 30s */
	}
}


/* TTY interrupt handlers.  */

void nb85e_uart_tty_tx_irq (int irq, void *data, struct pt_regs *regs)
{
	struct nb85e_uart_tty_port *port = data;
	if (port->gs.flags & GS_ACTIVE)
		nb85e_uart_tty_tx (port);
	else
		nb85e_uart_tty_disable_tx_interrupts (data);
}

void nb85e_uart_tty_rx_irq (int irq, void *data, struct pt_regs *regs)
{
	struct nb85e_uart_tty_port *port = data;

	if (port->gs.flags & GS_ACTIVE) {
		unsigned ch_stat;
		unsigned err = NB85E_UART_ASIS (port->chan);
		unsigned ch = NB85E_UART_RXB (port->chan);

		if (err & NB85E_UART_ASIS_OVE)
			ch_stat = TTY_OVERRUN;
		else if (err & NB85E_UART_ASIS_FE)
			ch_stat = TTY_FRAME;
		else if (err & NB85E_UART_ASIS_PE)
			ch_stat = TTY_PARITY;
		else
			ch_stat = TTY_NORMAL;

		tty_insert_flip_char (port->gs.tty, ch, ch_stat);
		tty_schedule_flip (port->gs.tty);
	} else
		nb85e_uart_tty_disable_rx_interrupts (port);
}


/* Higher level TTY interface.  */

static struct tty_struct *nb85e_uart_ttys[NB85E_UART_NUM_CHANNELS] = { 0 };
static struct termios *nb85e_uart_tty_termios[NB85E_UART_NUM_CHANNELS] = { 0 };
static struct termios *nb85e_uart_tty_termios_locked[NB85E_UART_NUM_CHANNELS] = { 0 };
static struct tty_driver nb85e_uart_tty_driver = { 0 };
static int nb85e_uart_tty_refcount = 0;

int nb85e_uart_tty_open (struct tty_struct *tty, struct file *filp)
{
	int err;
	struct nb85e_uart_tty_port *port;
	unsigned chan = MINOR (tty->device) - NB85E_UART_MINOR_BASE;

	if (chan >= NB85E_UART_NUM_CHANNELS)
		return -ENODEV;

	port = &nb85e_uart_tty_ports[chan];

	tty->driver_data = port;
	port->gs.tty = tty;
	port->gs.count++;

	port->tqueue.routine = gs_do_softint;
	port->tqueue.data = &port->gs;

	/*
	 * Start up serial port
	 */
	err = gs_init_port (&port->gs);
	if (err)
		goto failed_1;

	port->gs.flags |= GS_ACTIVE;

	if (port->gs.count == 1) {
		MOD_INC_USE_COUNT;

		/* Alloc RX irq.  */
		err = request_irq (IRQ_INTSR (chan), nb85e_uart_tty_rx_irq,
				   SA_INTERRUPT, "nb85e_uart", port);
		if (err)
			goto failed_2;

		/* Alloc TX irq.  */
		err = request_irq (IRQ_INTST (chan), nb85e_uart_tty_tx_irq,
				   SA_INTERRUPT, "nb85e_uart", port);
		if (err) {
			free_irq (IRQ_INTSR (chan), port);
			goto failed_2;
		}
	}

	err = gs_block_til_ready (port, filp);
	if (err)
		goto failed_3;

	*tty->termios = port->gs.normal_termios;

	nb85e_uart_tty_enable_rx_interrupts (port);

	port->gs.session = current->session;
	port->gs.pgrp = current->pgrp;

	return 0;

failed_3:
	free_irq (IRQ_INTST (chan), port);
	free_irq (IRQ_INTSR (chan), port);
failed_2:
	MOD_DEC_USE_COUNT;
failed_1:
	port->gs.count--;

	return err;
}

int __init nb85e_uart_tty_init (void)
{
	struct tty_driver *d = &nb85e_uart_tty_driver;

	d->driver_name = "nb85e_uart";
#ifdef CONFIG_DEVFS_FS
	d->name = "tts/%d";
#else
	d->name = "ttyS";
#endif

	d->major = TTY_MAJOR;
	d->minor_start = NB85E_UART_MINOR_BASE;
	d->num = NB85E_UART_NUM_CHANNELS;
	d->type = TTY_DRIVER_TYPE_SERIAL;
	d->subtype = SERIAL_TYPE_NORMAL;

	d->refcount = &nb85e_uart_tty_refcount;

	d->table = nb85e_uart_ttys;
	d->termios = nb85e_uart_tty_termios;
	d->termios_locked = nb85e_uart_tty_termios_locked;

	d->init_termios = tty_std_termios;
	d->init_termios.c_cflag = NB85E_UART_INIT_CFLAGS;

	d->open = nb85e_uart_tty_open;
	d->put_char = gs_put_char;
	d->write = gs_write;
	d->write_room = gs_write_room;
	d->start = gs_start;
	d->stop = gs_stop;
	d->close = gs_close;
	d->write = gs_write;
	d->put_char = gs_put_char;
	d->flush_chars = gs_flush_chars;
	d->write_room = gs_write_room;
	d->chars_in_buffer = gs_chars_in_buffer;
	d->set_termios = gs_set_termios;
	d->throttle = 0;  /* NB85E_UART uarts have no hardware flow control */
	d->unthrottle = 0; /* " */
	d->stop = gs_stop;
	d->start = gs_start;
	d->hangup = gs_hangup;
	d->flush_buffer = gs_flush_buffer;

	init_nb85e_uart_tty_ports ();

	tty_register_driver (&nb85e_uart_tty_driver);
}
__initcall (nb85e_uart_tty_init);
