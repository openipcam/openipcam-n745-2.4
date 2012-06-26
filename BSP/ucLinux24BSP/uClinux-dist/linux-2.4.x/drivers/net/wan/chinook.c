/*
 * Chinook synchronous serial driver for Linux
 *
 * Copyright (C) 2002 SnapGear Inc (www.snapgear.com)
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Note: integrated CSU/DSU/DDS are not supported by this driver
 *
 * Based on N2 driver by Krzysztof Halasa <khc@pm.waw.pl>
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/sched.h>
#include <linux/types.h>
#include <linux/fcntl.h>
#include <linux/in.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/ioport.h>
#include <linux/netdevice.h>
#include <linux/hdlc.h>
#include <asm/io.h>
#include "hd64570.h"

#define DEBUG_RINGS
/*#define DEBUG_PKT*/

static const char* version = "Chinook/HD64570 driver for Linux 2.4";
static const char* devname = "Chinook/HD64570";

#define CLOCK_BASE 16500000	/* 16.5 MHz */

#define CHINOOK_IOPORTS 0x100
#define CHINOOK_SRAM_SIZE	0x20000

#define PIO_DATA31_16	0xc32
#define PIO_LTEST		0x1000	/* PIO28 */
#define PIO_EXCLK_EN	0x0010	/* PIO20, aka TTEN */
#define PIO_SCTEN		0x0004	/* PIO18 */
#define PIO_DTR			0x0002	/* PIO17 */
#define PIO_DSR			0x0001	/* PIO16 */

static char *hw = NULL;	/* pointer to hw=xxx command line string */


typedef struct port_s {
	hdlc_device hdlc;	/* HDLC device struct - must be first */
	struct card_s *card;
	spinlock_t lock;	/* TX lock */
	int clkmode;		/* clock mode */
	int clkrate;		/* clock rate */
	int line;		/* loopback only */
	u8 rxs, txs, tmc;	/* SCA registers */
	u8 valid;		/* port enabled */
	u8 phy_node;		/* physical port # - 0 or 1 */
	u8 log_node;		/* logical port # */
	u8 rxin;		/* rx ring buffer 'in' pointer */
	u8 txin;		/* tx ring buffer 'in' and 'last' pointers */
	u8 txlast;
	u8 rxpart;		/* partial frame received, next frame invalid*/
}port_t;

typedef struct card_s {
	u8 *base;		/* ISA base address */
	u32 phy_base;	/* ISA physical base address */
	u32 ram_size;		/* number of bytes */
	u16 io;			/* IO Base address */
	u16 buff_offset;	/* offset of first buffer of first channel */
	u8 irq;			/* IRQ (3-15) */
	u8 ring_buffers;	/* number of buffers in a ring */
	u8 *mmcr_base;
	u16 sp504_io;
	u8 sp504_irq;

	port_t ports[2];
	struct card_s *next_card;
}card_t;


#define ALL_PAGES_ALWAYS_MAPPED

#define sca_reg(reg, card)		((card)->io | (reg))
#define sca_in(reg, card)		inb(sca_reg(reg, card))
#define sca_out(value, reg, card)	outb(value, sca_reg(reg, card))
#define sca_inw(reg, card)		inw(sca_reg(reg, card))
#define sca_outw(value, reg, card)	outw(value, sca_reg(reg, card))

#define port_to_card(port)		((port)->card)
#define log_node(port)			((port)->log_node)
#define phy_node(port)			((port)->phy_node)
#define winbase(card)      	     	((card)->base)
#define get_port(card, port)		((card)->ports[port].valid ? \
					 &(card)->ports[port] : NULL)


#include "hd6457x.c"


struct cable_mode {
	u8	sp504;
	int hdlc;
};

static struct cable_mode cable_mode[] = {
									/* PIO 31/30/29 */
	{ 0x0, 0 /* Invalid */ },		/* 000 */
	{ 0xee, LINE_V35 },				/* 001 */
	{ 0x0, 0 /* Invalid */ },		/* 010 */
	{ 0x22, LINE_RS232 },			/* 011 */
	{ 0xdd, 0, /* EIA530 */ },		/* 100 */
	{ 0xcc, 0 /* EIA449 */ },		/* 101 */
	{ 0x44, LINE_X21 },				/* 110 */
	{ 0x0, 0 /* No cable */ }		/* 111 */
};


static int chinook_get_line(port_t *port)
{
	card_t *card = port->card;
	u16 mode;

	mode = *(u16 *)(card->mmcr_base + PIO_DATA31_16);
	mode >>= 13;

	return cable_mode[mode].hdlc;
}



#define SC520_INTPINPOL		0xd10	/* Offset in the SC520 MMCR */
#define SC520_GPINT6_POL		0x0004

static void chinook_set_sp504(card_t *card)
{
	u16 mode;
	unsigned long flags;

	mode = *(u16 *)(card->mmcr_base + PIO_DATA31_16);
	mode >>= 13;

	save_flags(flags); cli();
	if (mode == 7)
		*(u16 *)(card->mmcr_base + SC520_INTPINPOL) |= SC520_GPINT6_POL;
	else
		*(u16 *)(card->mmcr_base + SC520_INTPINPOL) &= ~SC520_GPINT6_POL;
	restore_flags(flags);

	outb(cable_mode[mode].sp504, card->sp504_io);
}



static void chinook_sp504_intr(int irq, void* dev_id, struct pt_regs *regs)
{
	card_t *card = dev_id;

	chinook_set_sp504(card);
}



static int chinook_set_clock(port_t *port, int value)
{
	card_t *card = port->card;
	u8 msci = get_msci(port);
	u8 rxs = port->rxs & CLK_BRG_MASK;
	u8 txs = port->txs & CLK_BRG_MASK;
	u16 *pio = (u16 *)(card->mmcr_base + PIO_DATA31_16);

	switch(value) {
	case CLOCK_EXT:
		*pio |= PIO_EXCLK_EN | PIO_SCTEN;
		rxs |= CLK_LINE_RX; /* RXC input */
		txs |= CLK_LINE_TX; /* TXC input */
		break;

	case CLOCK_TXINT:
		*pio &= ~(PIO_EXCLK_EN | PIO_SCTEN);
		rxs |= CLK_LINE_RX; /* RXC input */
		txs |= CLK_BRG_TX; /* BRG output */
		break;

	case CLOCK_TXFROMRX:
		*pio |= PIO_EXCLK_EN | PIO_SCTEN;
		rxs |= CLK_LINE_RX; /* RXC input */
		txs |= CLK_RXCLK_TX; /* RX clock */
		break;

	default:
		return -EINVAL;
	}

	port->rxs = rxs;
	port->txs = txs;
	sca_out(rxs, msci + RXS, card);
	sca_out(txs, msci + TXS, card);
	port->clkmode = value;

	return 0;
}



static int chinook_open(hdlc_device *hdlc)
{
	port_t *port = hdlc_to_port(hdlc);

	MOD_INC_USE_COUNT;

	/* set DTR ON */
	*(u16 *)(port->card->mmcr_base + PIO_DATA31_16) &= ~PIO_DTR;

	sca_open(hdlc);
	chinook_set_clock(port, port->clkmode);
	chinook_set_sp504(port->card);
	return 0;
}



static void chinook_close(hdlc_device *hdlc)
{
	port_t *port = hdlc_to_port(hdlc);

	sca_close(hdlc);

	/* set DTR OFF */
	*(u16 *)(port->card->mmcr_base + PIO_DATA31_16) |= PIO_DTR;

	MOD_DEC_USE_COUNT;
}



static int chinook_ioctl(hdlc_device *hdlc, struct ifreq *ifr, int cmd)
{
	int value = ifr->ifr_ifru.ifru_ivalue;
	int result = 0;
	port_t *port = hdlc_to_port(hdlc);

	if(!capable(CAP_NET_ADMIN))
		return -EPERM;

	switch(cmd) {
	case HDLCSCLOCK:
		result = chinook_set_clock(port, value);
	case HDLCGCLOCK:
		value = port->clkmode;
		break;

	case HDLCSCLOCKRATE:
		port->clkrate = value;
		sca_set_clock(port);
	case HDLCGCLOCKRATE:
		value = port->clkrate;
		break;

	case HDLCSLINE:
		result = sca_set_loopback(port, value);
	case HDLCGLINE:
		value = port->line | chinook_get_line(port);
		break;

#ifdef DEBUG_RINGS
	case HDLCRUN:
		sca_dump_rings(hdlc);
		return 0;
#endif /* DEBUG_RINGS */

	default:
		return -EINVAL;
	}

	ifr->ifr_ifru.ifru_ivalue = value;
	return result;
}


#define CHINOOK_SRAM_TESTSIZE		0x1000

static u32 chinook_test_sram(card_t *card)
{
	u8 page;
	int i, bcount = CHINOOK_SRAM_TESTSIZE, wcount = CHINOOK_SRAM_TESTSIZE/2;
	u16 *dp0 = (u16*)card->base;

	for (page = 0; page < CHINOOK_SRAM_SIZE / CHINOOK_SRAM_TESTSIZE; page++) {
		u16 *dp = (u16*)(card->base + page * CHINOOK_SRAM_TESTSIZE);
		u8 *bp = (u8*)(card->base + page * CHINOOK_SRAM_TESTSIZE);

		writeb(page, dp);
		if (readb(dp) != page)
			return 0;	/* If can't read back, no good memory */

		if (readb(dp0))
			return 0;	/* If page 0 changed, then address line bad */

		/*  first do byte tests */
		for (i = 0; i < bcount; i++)
			writeb(i, bp + i);
		for (i = 0; i < bcount; i++)
			if (readb(bp + i) != (i & 0xff))
				return 0;

		for (i = 0; i < bcount; i++)
			writeb(~i, bp + i);
		for (i = 0; i < bcount; i++)
			if (readb(bp + i) != (~i & 0xff))
				return 0;

		/* next do 16-bit tests */
		for (i = 0; i < wcount; i++)
			writew(i, dp + i);
		for (i = 0; i < wcount; i++)
			if (readw(dp + i) != (i & 0xffff))
				return 0;

		for (i = 0; i < wcount; i++)
			writew(0x55AA, dp + i);
		for (i = 0; i < wcount; i++)
			if (readw(dp + i) != 0x55AA)
				return 0;

		for (i = 0; i < wcount; i++)
			writew(0xAA55, dp + i);
		for (i = 0; i < wcount; i++)
			if (readw(dp + i) != 0xAA55)
				return 0;

		for (i = 0; i < wcount; i++)
			writew(page, dp + i);
	}

	return CHINOOK_SRAM_SIZE;
}



static void chinook_destroy_card(card_t *card)
{
	if (card->ports[0].card)
		unregister_hdlc_device(&card->ports[0].hdlc);

	if (card->irq)
		free_irq(card->irq, card);

	if (card->base) {
		iounmap(card->base);
		release_mem_region(card->phy_base, CHINOOK_SRAM_SIZE);
	}

	if (card->io)
		release_region(card->io, CHINOOK_IOPORTS);
	kfree(card);
}



static int chinook_run(unsigned long io, unsigned long irq, unsigned long base)
{
	card_t *card;
	u32 cnt;
	u16 tmp;

	if (io < 0x200 || io > 0x4FF || (io % CHINOOK_IOPORTS) != 0) {
		printk(KERN_ERR "chinook: invalid I/O port value\n");
		return -ENODEV;
	}

	if (irq != 7) {
		printk(KERN_ERR "chinook: invalid IRQ value\n");
		return -ENODEV;
	}

	if (base < 0xA0000 || (base & 0xFFF) != 0) {
		printk(KERN_ERR "chinook: invalid RAM value\n");
		return -ENODEV;
	}

	card = kmalloc(sizeof(card_t), GFP_KERNEL);
	if (card == NULL) {
		printk(KERN_ERR "chinook: unable to allocate memory\n");
		return -ENOBUFS;
	}
	memset(card, 0, sizeof(card_t));

	card->mmcr_base = ioremap_nocache(0xfffef000, 4096);
	if (card->mmcr_base == NULL) {
		printk(KERN_ERR "chinook: unable to disable MMCR cache\n");
		return -EIO;
	}

	if (!request_region(io, CHINOOK_IOPORTS, devname)) {
		printk(KERN_ERR "chinook: I/O port region in use\n");
		chinook_destroy_card(card);
		return -EBUSY;
	}
	card->io = io;

	if (request_irq(irq, &sca_intr, 0, devname, card)) {
		printk(KERN_ERR "chinook: could not allocate IRQ\n");
		chinook_destroy_card(card);
		return(-EBUSY);
	}
	card->irq = irq;

	if (!request_mem_region(base, CHINOOK_SRAM_SIZE, devname)) {
		printk(KERN_ERR "chinook: could not request SRAM\n");
		chinook_destroy_card(card);
		return(-EBUSY);
	}
	card->phy_base = base;
	card->base = ioremap(base, CHINOOK_SRAM_SIZE);

	if (!request_region(0x300, 1, devname)) {
		printk(KERN_ERR "chinook: SP504 I/O port region in use\n");
		chinook_destroy_card(card);
		return -EBUSY;
	}
	card->sp504_io = 0x300;

	if (request_irq(6, &chinook_sp504_intr, 0, devname, card)) {
		printk(KERN_ERR "chinook: could not allocate SP504 IRQ\n");
		chinook_destroy_card(card);
		return(-EBUSY);
	}
	card->sp504_irq = 6;

	/* Test 8 and 16 bit IO to the SCA */
	sca_out(0, MSCI0_OFFSET + TMC, card);
	tmp = sca_in(MSCI0_OFFSET + TMC, card);
	if (tmp != 0) {
		printk(KERN_ERR "chinook: error reading SCA TMC=%x, expected 0\n",
				tmp);
		chinook_destroy_card(card);
		return(-EIO);
	}

	sca_out(0x5a, MSCI0_OFFSET + TMC, card);
	tmp = sca_in(MSCI0_OFFSET + TMC, card);
	if (tmp != 0x5a) {
		printk(KERN_ERR "chinook: error reading SCA TMC=%x, expected 5a\n",
				tmp);
		chinook_destroy_card(card);
		return(-EIO);
	}

	sca_outw(0x55aa, DMAC0RX_OFFSET + CDAL, card);
	tmp = sca_inw(DMAC0RX_OFFSET + CDAL, card);
	if (tmp != 0x55aa) {
		printk(KERN_ERR "chinook: error reading SCA CDA=%x, expected 55aa\n",
				tmp);
		chinook_destroy_card(card);
		return(-EIO);
	}

	cnt = chinook_test_sram(card);
	if (!cnt) {
		printk(KERN_ERR "chinook: memory test failed.\n");
		chinook_destroy_card(card);
		return -EIO;
	}

	card->ram_size = cnt;

	/* Turn LTEST off */
	*(u16 *)(card->mmcr_base + PIO_DATA31_16) |= PIO_LTEST;

	/* 2 rings for one port */
	card->ring_buffers = card->ram_size /
		(2 * (sizeof(pkt_desc) + HDLC_MAX_MRU));

	card->buff_offset = 2 * (sizeof(pkt_desc)) * card->ring_buffers;

	printk(KERN_DEBUG "chinook: HD64570 %u KB RAM, IRQ%u, "
	       "using %u packets rings\n", card->ram_size / 1024, card->irq,
	       card->ring_buffers);

	sca_init(card, 0);
	{
		port_t *port = &card->ports[0];

		port->phy_node = 0;
		port->valid = 1;
		port->log_node = 0;
		spin_lock_init(&port->lock);
		hdlc_to_dev(&port->hdlc)->irq = irq;
		hdlc_to_dev(&port->hdlc)->mem_start = base;
		hdlc_to_dev(&port->hdlc)->mem_end = base + CHINOOK_SRAM_SIZE-1;
		hdlc_to_dev(&port->hdlc)->tx_queue_len = 50;
		port->hdlc.ioctl = chinook_ioctl;
		port->hdlc.open = chinook_open;
		port->hdlc.close = chinook_close;
		port->hdlc.xmit = sca_xmit;

		if (register_hdlc_device(&port->hdlc)) {
			printk(KERN_WARNING "chinook: unable to register hdlc "
			       "device\n");
			chinook_destroy_card(card);
			return -ENOBUFS;
		}
		port->card = card;
		sca_init_sync_port(port); /* Set up SCA memory */

		printk(KERN_INFO "%s: Chinook node %d\n",
		       hdlc_to_name(&port->hdlc), port->phy_node);
	}

	*new_card = card;
	new_card = &card->next_card;

	return 0;
}



static int __init chinook_init(void)
{
	if (hw==NULL) {
		hw = "0x400,7,0x30000000";
	}

	printk(KERN_INFO "%s\n", version);

	do {
		unsigned long io, irq, ram;

		io = simple_strtoul(hw, &hw, 0);

		if (*hw++ != ',')
			break;
		irq = simple_strtoul(hw, &hw, 0);

		if (*hw++ != ',')
			break;
		ram = simple_strtoul(hw, &hw, 0);

		if (*hw == ':' || *hw == '\x0')
			chinook_run(io, irq, ram);

		if (*hw == '\x0')
			return 0;
	}while(*hw++ == ':');

	printk(KERN_ERR "chinook: invalid hardware parameters\n");
	return first_card ? 0 : -ENOSYS;
}


#ifndef MODULE
static int __init chinook_setup(char *str)
{
	hw = str;
	return 1;
}

__setup("chinook=", chinook_setup);
#endif


static void __exit chinook_cleanup(void)
{
	card_t *card = first_card;

	while (card) {
		card_t *ptr = card;
		card = card->next_card;
		chinook_destroy_card(ptr);
	}
}


module_init(chinook_init);
module_exit(chinook_cleanup);

MODULE_AUTHOR("Philip Craig <philipc@snapgear.com>");
MODULE_DESCRIPTION("Chinook HD64570 serial port driver");
MODULE_LICENSE("GPL");
MODULE_PARM(hw, "s");		/* hw=io,irq,ram:io,irq,... */
EXPORT_NO_SYMBOLS;
