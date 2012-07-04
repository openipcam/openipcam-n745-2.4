/******************************************************************************
 *
 * Copyright (c) 2003 Windond Electronics Corp.
 * All rights reserved.
 *
 * $Workfile: heap.c $
 *
 * $Author: Yachen $
 ******************************************************************************/
/*
 * $History: heap.c $
 * 
 * *****************  Version 1  *****************
 * User: Yachen       Date: 06/01/10   Time: 10:55a
 * Created in $/W90P710/Applications/710bootloader/shell
 * 710 Bootloader, without USB support
 * 
 * *****************  Version 1  *****************
 * User: Yachen       Date: 06/01/04   Time: 2:27p
 * Created in $/W90P710/Module Test Programs/FIRMWARE_710/shell
 * Module test bootloader, removed decompress function in order to save
 * memory space for LCD control
 * 
 * *****************  Version 3  *****************
 * User: Wschang0     Date: 03/09/17   Time: 5:16p
 * Updated in $/W90P710/FIRMWARE/shell
 * Implement: __Heap_Alloc/__Heap_Free/__Heap_ProvideMemory
 * 
 * *****************  Version 2  *****************
 * User: Wschang0     Date: 03/08/20   Time: 1:07p
 * Updated in $/W90P710/FIRMWARE/shell
 * Add VSS header
 */

#include <rt_misc.h>

struct heap_info_block
{
	int heap_base;
	int heap_limit;
	int stack_base;
	int stack_limit;
} heap_info;


unsigned int FileCount = 0x0;
unsigned int ExceptionStatus = 0;
unsigned int ExceptionLinkReg = 0;

__value_in_regs struct __initial_stackheap __user_initial_stackheap(
        unsigned R0, unsigned SP, unsigned R2, unsigned SL)
{
    struct __initial_stackheap config;

    extern unsigned int Image$$RAM$$ZI$$Limit;

    config.heap_base = (unsigned int)&Image$$RAM$$ZI$$Limit; // NOTE: The heap base must less than 0x7000
    config.heap_limit = (0x8000-0x1000); // 0x1000 Bytes reserved for SVC stack
    // The stack base, stack limit was defined in init.s

	// for semihosted application     
    heap_info.heap_base = 0x100000;
    heap_info.heap_limit = 0x400000;
    heap_info.stack_base = 0x400000;
    heap_info.stack_limit = 0x100000;
                                 
    return config;
}

#if 1 // Replace the _Heap_Alloc functions

#define MAX_HEAP_NUMBER 16
#ifndef NULL
#define NULL 0
#endif
//#define dbg(...) uprintf("HEAP DBG:"__VA_ARGS__);
#define dbg(...) while(0)

struct __Heap_Descriptor {
void *my_first_free_block;
void *my_heap_limit;
};

struct _heap_block
{
	struct _heap_block * next;
	struct _heap_block * prev;
	char * buf;
	unsigned int size;
};

static char * heap_base=NULL;
static char * heap_limit=NULL;

struct _heap_block my_heap_table[MAX_HEAP_NUMBER]={0};
struct _heap_block * my_heap_tail=NULL;

void __Heap_ProvideMemory(struct __Heap_Descriptor * my_heap,void * buf_base,unsigned int buf_size)
{
	int i;
	for(i=0;i<MAX_HEAP_NUMBER;i++)
	{
		my_heap_table[i].next=NULL;
		my_heap_table[i].prev=NULL;
		my_heap_table[i].buf=NULL;
		my_heap_table[i].size=0;
	}
	my_heap_tail=&my_heap_table[0];
	my_heap_tail->buf=(char *)buf_base;
			
	my_heap->my_first_free_block=buf_base;
	my_heap->my_heap_limit=(void *)((unsigned int)buf_base+(unsigned int)buf_size);
	heap_base=(char *)buf_base;
	heap_limit=(char *)buf_base+buf_size;
	dbg("Init heap base to 0x%08x\n",(int)my_heap->my_first_free_block);
}

//FIX ME: The heap limit is not checked
void * __Heap_Alloc(struct __Heap_Descriptor * my_heap,unsigned int size)
{
	int i;
	if( my_heap_tail->size == 0 )
	{
		my_heap_tail->size=size;
		dbg("Zero size block, tail is 0x%08x allocate 0x%08x %x\n",(int)my_heap_tail,my_heap_tail->buf,my_heap_tail->size);
		return my_heap_tail->buf;
		
	}
	else
	{
		for(i=0;i<MAX_HEAP_NUMBER;i++)
		{
			/* add a new block to tail */
			if( my_heap_table[i].buf == NULL )
			{
				my_heap_table[i].next=NULL;
				my_heap_table[i].prev=my_heap_tail;
				my_heap_table[i].buf=my_heap_tail->buf+my_heap_tail->size;
				my_heap_table[i].size=size;
				my_heap_tail->next=&my_heap_table[i];
				my_heap_tail=&my_heap_table[i];
				dbg("add new block to tail 0x%08x allocate 0x%08x %x\n",(int)my_heap_tail,my_heap_tail->buf,my_heap_tail->size);
				break;
			}
		}
		
		if( i == MAX_HEAP_NUMBER )
		{
			dbg("No enought free blocks\n");
			return NULL;
		}
		
		return my_heap_tail->buf;
	}
}

void __Heap_Free( struct __Heap_Descriptor * my_heap, char * buffer )
{
	struct _heap_block * block;
	
	for(block=my_heap_tail;block!=NULL;block=block->prev)
	{
		if( block->buf == buffer )
		{	/* free a block */
			if( block == my_heap_tail )
			{
				if( block->prev != NULL )
				{
					my_heap_tail=block->prev;
					my_heap_tail->next=NULL;
					block->next=NULL;
					block->prev=NULL;
					block->buf=NULL;
					block->size=0;
					dbg("Free a block, tail is 0x%08x allocate 0x%08x %x\n",(int)my_heap_tail,my_heap_tail->buf,my_heap_tail->size);
				}
				else
				{
					my_heap_tail=block;
					block->next=NULL;
					block->prev=NULL;
					/* preserve the buf pointer */
					block->size=0;
					dbg("tail only block free, tail is 0x%08x allocate 0x%08x %x\n",(int)my_heap_tail,my_heap_tail->buf,my_heap_tail->size);
				}
			}
			else
			{
				if( block->prev )block->prev->next=block->next;
				block->next->prev=block->prev;
				block->buf=NULL;
				block->size=0;
				block->prev=NULL;
				block->next=NULL;
			}
			
			break;
		}
	}
	
	if( block == NULL )
	{
		dbg("Free a unknown block\n");
	}
}


#endif