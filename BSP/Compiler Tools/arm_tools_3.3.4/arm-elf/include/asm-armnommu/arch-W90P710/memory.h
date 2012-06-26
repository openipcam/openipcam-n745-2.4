/*
 * linux/include/asm-armnommu/arch-W90P710/memory.h
 *
 * Copyright (c) 1999 Nicolas Pitre <nico@cam.org>
 * 2001 Mindspeed
 */

#ifndef __ASM_ARCH_MEMORY_H
#define __ASM_ARCH_MEMORY_H

#define TASK_SIZE	(0x01a00000UL)
#define TASK_SIZE_26	TASK_SIZE

#if 0
extern unsigned long _end_kernel;
#define PHYS_OFFSET    ((unsigned long) &_end_kernel)
#else
#define PHYS_OFFSET    (DRAM_BASE)
#endif
#define PAGE_OFFSET PHYS_OFFSET
#define END_MEM     (DRAM_BASE + DRAM_SIZE)
#endif
