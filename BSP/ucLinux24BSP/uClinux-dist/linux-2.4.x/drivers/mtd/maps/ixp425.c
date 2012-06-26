/*
 * $Id: ixp425.c,v 1.1.1.1 2006-07-11 09:29:06 andy Exp $
 *
 * drivers/mtd/maps/ixp425.c
 *
 * MTD Map file for IXP425 based systems. Please do not make per-board
 * map driver as the code will be 90% identical. For now just add
 * if(machine_is_XXX()) checks to the code. I'll clean this stuff to
 * use platform_data in the the future so we can get rid of that too.
 *
 * Original Author: Intel Corporation
 * Maintainer: Deepak Saxena <dsaxena@mvista.com>
 *
 * Copyright (C) 2002 Intel Corporation
 * Copyright (C) 2003 MontaVista Software, Inc.
 *
 */

#include <linux/module.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/map.h>
#include <linux/mtd/partitions.h>
#include <linux/ioport.h>
#include <asm/io.h>
#include <asm/mach-types.h>

#include <linux/reboot.h>

#define WINDOW_ADDR 	0x50000000
#define BUSWIDTH 	2

#ifndef __ARMEB__
#define	BYTE0(h)	((h) & 0xFF)
#define	BYTE1(h)	(((h) >> 8) & 0xFF)
#else
#define	BYTE0(h)	(((h) >> 8) & 0xFF)
#define	BYTE1(h)	((h) & 0xFF)
#endif

static __u16
ixp425_read16(struct map_info *map, unsigned long ofs)
{
	return *(__u16 *) (map->map_priv_1 + ofs);
}

/*
 * The IXP425 expansion bus only allows 16-bit wide acceses
 * when attached to a 16-bit wide device (such as the 28F128J3A),
 * so we can't just memcpy_fromio().
 */
static void
ixp425_copy_from(struct map_info *map, void *to,
		 unsigned long from, ssize_t len)
{
	int i;
	u8 *dest = (u8 *) to;
	u16 *src = (u16 *) (map->map_priv_1 + from);
	u16 data;

	for (i = 0; i < (len / 2); i++) {
		data = src[i];
		dest[i * 2] = BYTE0(data);
		dest[i * 2 + 1] = BYTE1(data);
	}

	if (len & 1)
		dest[len - 1] = BYTE0(src[i]);
}

static void
ixp425_write16(struct map_info *map, __u16 d, unsigned long adr)
{
	*(__u16 *) (map->map_priv_1 + adr) = d;
}

static struct map_info ixp425_map = {
	.name		= "IXP425 Flash",
	.buswidth	= BUSWIDTH,
	.read16		= ixp425_read16,
	.copy_from	= ixp425_copy_from,
	.write16	= ixp425_write16,
};

/*
 * Put flash back in read mode so RedBoot can boot properly.
 */
int ixp425_mtd_reboot(struct notifier_block *n, unsigned long code, void *p)
{
	if (code != SYS_RESTART)
		return NOTIFY_DONE;

	ixp425_write16(&ixp425_map, 0xff, 0x55 * 0x2);
	return NOTIFY_DONE;
}

static struct notifier_block ixp425_mtd_notifier = {
	notifier_call:ixp425_mtd_reboot,
	next:NULL,
	priority:0
};

static struct mtd_partition *parsed_parts;
static const char *probes[] = { "RedBoot", "cmdlinepart", NULL };

static struct mtd_partition ixp425_partitions[] = {
	{
		.name	= "image",
		.offset	= 0x00040000,
		.size	= 0x00400000,
	}, {
		.name	= "user",
		.offset	= 0x00440000,
		.size	= MTDPART_SIZ_FULL
	}
};

#define NB_OF(x)  (sizeof(x)/sizeof(x[0]))

static struct mtd_info *ixp425_mtd;
static struct resource *mtd_resource;

static void
ixp425_exit(void)
{
	if (ixp425_mtd) {
		del_mtd_partitions(ixp425_mtd);
		map_destroy(ixp425_mtd);
	}
	if (ixp425_map.map_priv_1)
		iounmap((void *) ixp425_map.map_priv_1);
	if (mtd_resource)
		release_mem_region(WINDOW_ADDR, ixp425_map.size);

	if (parsed_parts)
		kfree(parsed_parts);

	unregister_reboot_notifier(&ixp425_mtd_notifier);

	/* Disable flash write */
	*IXP425_EXP_CS0 &= ~IXP425_FLASH_WRITABLE;

	if(machine_is_adi_coyote())
		*IXP425_EXP_CS1 &= ~IXP425_FLASH_WRITABLE;
}

static int __init
ixp425_init(void)
{
	int res = -1, npart;

	/* Enable flash write */
	*IXP425_EXP_CS0 |= IXP425_FLASH_WRITABLE;

	/*
	 * Coyote requires CS1 write to be enabled and has 32MB flash.
	 * This will move to the platform init code in 2.6
	 */
	if(machine_is_adi_coyote()) {
		*IXP425_EXP_CS1 |= IXP425_FLASH_WRITABLE;
		ixp425_map.size = 0x02000000;
	} else
		ixp425_map.size = 0x01000000;

	ixp425_map.map_priv_1 = 0;
	mtd_resource =
	    request_mem_region(WINDOW_ADDR, ixp425_map.size, "IXP425 Flash");
	if (!mtd_resource) {
		printk(KERN_ERR
		       "ixp425 flash: Could not request mem region.\n");
		res = -ENOMEM;
		goto Error;
	}

	ixp425_map.map_priv_1 =
	    (unsigned long) ioremap(WINDOW_ADDR, ixp425_map.size);
	if (!ixp425_map.map_priv_1) {
		printk("ixp425 Flash: Failed to map IO region. (ioremap)\n");
		res = -EIO;
		goto Error;
	}

	ixp425_mtd = do_map_probe("cfi_probe", &ixp425_map);
	if (!ixp425_mtd) {
		res = -ENXIO;
		goto Error;
	}
	ixp425_mtd->owner = THIS_MODULE;

	/* Try to parse RedBoot partitions */
	npart = parse_mtd_partitions(ixp425_mtd, probes, &parsed_parts, 0);
	if (npart > 0)
		res = add_mtd_partitions(ixp425_mtd, parsed_parts, npart);
	else {
		printk("IXP425 Flash: Using static MTD partitions.\n");
		res = add_mtd_partitions(ixp425_mtd, ixp425_partitions,
					 NB_OF(ixp425_partitions));
	}

	if (res)
		goto Error;

	register_reboot_notifier(&ixp425_mtd_notifier);

	return res;

Error:
	ixp425_exit();
	return res;
}

module_init(ixp425_init);
module_exit(ixp425_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("MTD map driver for ixp425 evaluation board");
MODULE_AUTHOR("Deepak Saxena");
