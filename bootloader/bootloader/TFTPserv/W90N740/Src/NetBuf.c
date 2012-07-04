/******************************************************************************
 *
 * Copyright (c) 2003 Windond Electronics Corp.
 * All rights reserved.
 * Winbond W90N740 Boot Loader
 *
 * Module Name:	NETBUF.C
 *
 * Created by : 
 ******************************************************************************/
#include "w90p710.h"
#include "netbuf.h"


NETBUF *_bufpool;

int  _NetBufferAvailableCount=0;

NETBUF  *_nbuf_first, *_nbuf_last;
NETBUF  *_iqueue_first, *_iqueue_last;   /* incoming queue */

//__align(16)	static char _NetBufferPool[NUMBER_OF_NETBUF * sizeof(NETBUF)];

//static char * _NetBufferPool=(char *)0;

int  NetBuf_Init()
{
    int  idx;
    NETBUF  *nbuf;
    unsigned int tmp;
	struct __Heap_Descriptor { 
	void *my_first_free_block; 
	void *my_heap_limit; 
	}; 
	extern struct __Heap_Descriptor my_heap;

    
    tmp = ( ((unsigned int)__Heap_Alloc( &my_heap,NUMBER_OF_NETBUF * sizeof(NETBUF) + 128 )));
    if( tmp != NULL )tmp=(tmp&0xFFFFFFF0)+64;
    _bufpool=(NETBUF *)(tmp|0x80000000) ;

    
    
    if (_bufpool == NULL)
    {
       return -1;
    }

    _iqueue_first = _iqueue_last = NULL;
    nbuf = _nbuf_first = &_bufpool[0];
    for (idx = 0; idx < NUMBER_OF_NETBUF; idx++)
    {
       nbuf->status = NBUF_STATUS_FREE;
       nbuf->next = &_bufpool[idx+1];
       nbuf->txNext = NULL;
       nbuf = nbuf->next;
    }
    _nbuf_last = &_bufpool[NUMBER_OF_NETBUF-1];
    _nbuf_last->next = NULL;
    _NetBufferAvailableCount = NUMBER_OF_NETBUF;
    return 0;
}


NETBUF *NetBuf_Allocate()
{
    NETBUF  *nbuf;

    Mac_DisableInt();
    nbuf = _nbuf_first;
    if (_nbuf_first)                   /* buffer available */
    {
       _nbuf_first = _nbuf_first->next;
       nbuf->status = NBUF_STATUS_ALLOCATED;
       nbuf->next = NULL;
       _NetBufferAvailableCount--;
    }
    if (_nbuf_first == NULL)           /* have reached the last buffer */
        _nbuf_last = NULL;
    Mac_EnableInt();
    
    return nbuf;
}


NETBUF *NetBuf_AllocateIR()
{
    NETBUF  *nbuf;

    nbuf = _nbuf_first;
    if (_nbuf_first)                   /* buffer available */
    {
       _nbuf_first = _nbuf_first->next;
       nbuf->status = NBUF_STATUS_ALLOCATED;
       nbuf->next = NULL;
       _NetBufferAvailableCount--;
    }
    if (_nbuf_first == NULL)           /* have reached the last buffer */
        _nbuf_last = NULL;

    return nbuf;
}


void NetBuf_Free(NETBUF *nbuf)
{
    Mac_DisableInt();
    nbuf->status = NBUF_STATUS_FREE;
    if (_nbuf_last == NULL)
    {
       _nbuf_last = nbuf;
       _nbuf_last->next = NULL;
       _nbuf_first = nbuf;
    }
    else
    {
       _nbuf_last->next = nbuf;
       _nbuf_last = nbuf;
       _nbuf_last->next = NULL;
    }
    _NetBufferAvailableCount++;
    Mac_EnableInt();
}


void NetBuf_FreeIR(NETBUF *nbuf)
{
    nbuf->status = NBUF_STATUS_FREE;
    if (_nbuf_last == NULL)
    {
       _nbuf_last = nbuf;
       _nbuf_last->next = NULL;
       _nbuf_first = nbuf;
    }
    else
    {
       _nbuf_last->next = nbuf;
       _nbuf_last = nbuf;
       _nbuf_last->next = NULL;
    }
    _NetBufferAvailableCount++;
}


