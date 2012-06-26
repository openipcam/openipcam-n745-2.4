/*
 * arch/arm/mach-ixp425/mm.c 
 *
 * Copyright (C) 2002 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include <linux/mm.h>
#include <linux/init.h>

#include <asm/io.h>
#include <asm/mach-types.h>
#include <asm/pgtable.h>
#include <asm/page.h>

#include <asm/mach/map.h>

#include <asm/arch-ixp425/ixp425.h>

/* See asm/arch/ixp425.h for a detailed memory map */

static struct map_desc ixp425_io_desc[] __initdata = {
	/* UART, Interrupt ctrl, GPIO, timers, NPEs, MACS, USB .... */
	{
		IXP425_PERIPHERAL_BASE_VIRT,
		IXP425_PERIPHERAL_BASE_PHYS,
		IXP425_PERIPHERAL_REGION_SIZE,
		DOMAIN_IO, 0, 1, 0, 0
			/* r  w  c  b */
	},
	/* PCI Config Registers */
	{
		IXP425_PCI_CFG_BASE_VIRT,
		IXP425_PCI_CFG_BASE_PHYS,
		IXP425_PCI_CFG_REGION_SIZE,
		DOMAIN_IO, 0, 1, 0, 0
			/* r  w  c  b */
	},
	/* Expansion Bus Config Registers */
	{
		IXP425_EXP_CFG_BASE_VIRT,
		IXP425_EXP_CFG_BASE_PHYS,
		IXP425_EXP_CFG_REGION_SIZE,
		DOMAIN_IO, 0, 1, 0, 0
			/* r  w  c  b */
	},
	LAST_DESC
};

void __init ixp425_map_io(void)
{
	iotable_init(ixp425_io_desc); 
}

