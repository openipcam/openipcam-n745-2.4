/******************************************************************************
 *
 * Copyright (c) 2003 Windond Electronics Corp.
 * All rights reserved.
 *
 * $Workfile: heap.c $
 *
 * Created by : 
 ******************************************************************************/
/*
 * $History: heap.c $
 * 
 * *****************  Version 1  *****************
 * User: Yachen       Date: 06/01/10   Time: 10:55a
 * Created in $/W90P710/Applications/710bootloader/TFTPserv/W90N740/Src
 * 710 Bootloader, without USB support
 * 
 * *****************  Version 1  *****************
 * User: Yachen       Date: 06/01/04   Time: 2:27p
 * Created in $/W90P710/Module Test Programs/FIRMWARE_710/TFTPserv/W90N740/Src
 * Module test bootloader, removed decompress function in order to save
 * memory space for LCD control
 * 
 * *****************  Version 2  *****************
 * User: Wschang0     Date: 03/09/24   Time: 7:53p
 * Updated in $/W90N740/FIRMWARE/TFTPserv/W90N740/Src
 * Add header
 */

#include <rt_misc.h>


__value_in_regs struct __initial_stackheap __user_initial_stackheap(
        unsigned R0, unsigned SP, unsigned R2, unsigned SL)
{
    struct __initial_stackheap config;

    extern unsigned int Image$$ZI$$Limit;

    config.heap_base = (unsigned int)&Image$$ZI$$Limit;
    config.heap_limit = config.heap_base + 0x10000;
    // The stack base, stack limit was defined in init.s
    config.stack_base = 0x400000;
    config.stack_limit = 0x300000;
	                                 
    return config;
}
