/*
 * arch/arm/mach-ixp425/arch.c 
 *
 * Copyright (C) 2002 Intel Corporation.
 *
 * Maintainer: Deepak Saxena <dsaxena@mvista.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include <asm/types.h>
#include <asm/setup.h>
#include <asm/memory.h>
#include <asm/hardware.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>

extern void ixp425_map_io(void);

extern void ixp425_init_irq(void);

#ifdef CONFIG_ARCH_IXDP425
MACHINE_START(IXDP425, "Intel IXDP425 Development Platform")
	MAINTAINER("MontaVista Software, Inc.")
	BOOT_MEM(PHYS_OFFSET, IXP425_PERIPHERAL_BASE_PHYS,
		IXP425_PERIPHERAL_BASE_VIRT)
	MAPIO(ixp425_map_io)
	INITIRQ(ixp425_init_irq)
	BOOT_PARAMS(0x2000)
MACHINE_END
#endif

#ifdef CONFIG_ARCH_IXCDP1100
MACHINE_START(IXCDP1100, "Intel IXCDP1100 Development Platform")
	MAINTAINER("MontaVista Software, Inc.")
	BOOT_MEM(PHYS_OFFSET, IXP425_PERIPHERAL_BASE_PHYS,
		IXP425_PERIPHERAL_BASE_VIRT)
	MAPIO(ixp425_map_io)
	INITIRQ(ixp425_init_irq)
	BOOT_PARAMS(0x2000)
MACHINE_END
#endif

#ifdef CONFIG_ARCH_PRPMC1100
MACHINE_START(IXDP425, "Motorolla PrPMC 1100")
	MAINTAINER("MontaVista Software, Inc.")
	BOOT_MEM(PHYS_OFFSET, IXP425_PERIPHERAL_BASE_PHYS,
		IXP425_PERIPHERAL_BASE_VIRT)
	MAPIO(ixp425_map_io)
	INITIRQ(ixp425_init_irq)
	BOOT_PARAMS(0x2000)
MACHINE_END
#endif

