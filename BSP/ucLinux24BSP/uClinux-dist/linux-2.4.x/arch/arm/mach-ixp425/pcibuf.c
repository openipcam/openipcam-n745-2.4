/*
 * arch/arm/mach-ixp425/ixp425-pcibuf.c 
 *
 * IXP425 PCI bounce buffer routines.  The IXP425 only has a 64MB inbound
 * PCI window, but allows for up 256MB of SDRAM.  This means that if 
 * running with > 64MB of memory, we need to bounce buffers between the
 * safe and unsafe areas.
 *
 * Maintainer: Deepak Saxena <dsaxena@mvista.com>
 *
 * Copyright (C) 2002 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include <linux/module.h>
#include <linux/pci.h>
#include <linux/slab.h>
#include <asm/hardware.h>
#include <asm/sizes.h>

#undef DEBUG

#ifdef DEBUG
#  define DBG(x...) printk(KERN_INFO __FILE__": "x)
#else
#  define DBG(x...)
#endif

#define MAX_SAFE 128

static dma_addr_t safe_buffers[MAX_SAFE];

static void *alloc_safe_buffer(void *unsafe, int size, dma_addr_t *dma_addr)
{
	int i;
	void *safe;

	DBG("alloc_safe_buffer(size=%d)\n", size);
	/* int+void* for buffer index and unsafe = original address */
	safe = kmalloc(size + sizeof(int) + sizeof(void *), GFP_ATOMIC | GFP_DMA);
	if (!safe)
		return 0;

	for (i = 0; i < MAX_SAFE; i++ )
	{
		if (safe_buffers[i] == 0)
			break;
	}

	if (i == MAX_SAFE)
		panic(__FILE__": exceeded MAX_SAFE buffers\n");

	*((int *)safe)++ = i;
	*((void **)safe)++ = unsafe;
	*dma_addr = virt_to_bus(safe);
	safe_buffers[i] = *dma_addr; 
	return safe;
}

static void free_safe_buffer(void *buf)
{
	int index;

	(char*)buf -= sizeof(void *) + sizeof(int);
	index = *(int*)buf;
	DBG("free_safe_buffer(%p) index %d\n", buf, index);

	if (index < 0 || index >= MAX_SAFE)
	{
		printk(KERN_ERR __FILE__": free_safe_buffer() corrupt buffer\n");
		return;
	}
	safe_buffers[index] = 0;
	kfree(buf);
}

static void *find_safe_buffer(dma_addr_t dma_addr, void **unsafe)
{
	int i;
	void *safe;

	for (i = 0; i < MAX_SAFE; i++ )
	{
		if (safe_buffers[i] == dma_addr)
		{
			DBG("find_safe_buffer(%p) found @ %d\n", (void *)dma_addr, i);
			safe = bus_to_virt(dma_addr);
			*unsafe = *(((void**)safe)-1);
			return safe;
		}
	}

	return 0;	    
}

dma_addr_t ixp425_map_single(struct pci_dev *hwdev, void *virt, size_t size,
	int direction)
{
	dma_addr_t dma_addr;

	DBG("ixp425_map_single(hwdev=%p,virt=%p,size=%d,dir=%x)\n", 
		hwdev, virt, size, direction);

	dma_addr = virt_to_bus(virt);

	if((dma_addr+size) > SZ_64M) 
	{
		void *safe = alloc_safe_buffer(virt, size, &dma_addr);
		if (!safe) 
		{
			printk(KERN_ERR __FILE__": unable to map unsafe buffer %p!\n", 
				virt);
			return 0;
		}
		DBG("unsafe buffer %p (phy=%p) mapped to %p (phy=%p)\n", virt, 
			(void *)virt_to_phys(virt), safe, (void *)dma_addr);
		/*
		 * Only need to copy if DMAing to device
		 */
		if(direction == PCI_DMA_TODEVICE)
			memcpy(safe, virt, size);
		consistent_sync(safe, size, direction);
	}
	else
		consistent_sync(virt, size, direction);

	return dma_addr;
}

void ixp425_unmap_single(struct pci_dev *hwdev, dma_addr_t dma_addr, 
	size_t size, int direction)
{
	void *safe, *unsafe;

	DBG("ixp425_unmap_single(hwdev=%p, ptr=%p, size=%d, dir=%x)\n", hwdev, 
		(void *)dma_addr, size, direction);

	if ((safe = find_safe_buffer(dma_addr, &unsafe)))
	{
		DBG("copyback unsafe %p, safe %p, size %d\n", unsafe, safe, size);
		consistent_sync(safe, size, direction);
		memcpy(unsafe, safe, size);
		free_safe_buffer(safe);
	} 
	else 
	{
		/* 
		 * Assume this is normal memory.  We have a possible
		 * OOPs here if someone sends us a bad dma_addr_t.
		 *
		 * A fix around this is to keep a record of what pci_devs
		 * have what mappings, but that seem overly complicated
		 */
		unsafe = bus_to_virt(dma_addr);
		consistent_sync(unsafe, size, direction);
	}
}

void ixp425_sync_single(struct pci_dev *hwdev, dma_addr_t dma_addr, 
	size_t size, int direction)
{
	void *safe, *unsafe;

	DBG("ixp425_sync_single(hwdev=%p, dma_addr=%p, size=%d, dir=%x)\n", hwdev, 
		(void *)dma_addr, size, direction);

	if((safe = find_safe_buffer(dma_addr, &unsafe))) {
		DBG("copyback unsafe %p, safe %p, size %d\n", unsafe, safe, size);
		switch(direction) {
			case PCI_DMA_TODEVICE:
				memcpy(safe, unsafe, size);
				consistent_sync(safe, size, direction);
				break;
			case PCI_DMA_FROMDEVICE:
				consistent_sync(safe, size, direction);
				memcpy(unsafe, safe, size);
				break;
		}
	}	else {
		/* assume this is normal memory */
		unsafe = bus_to_virt(dma_addr);
		consistent_sync(unsafe, size, direction);
	}
}

EXPORT_SYMBOL(ixp425_map_single);
EXPORT_SYMBOL(ixp425_unmap_single);
EXPORT_SYMBOL(ixp425_sync_single);
