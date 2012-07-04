/******************************************************************************
 *
 * Copyright (c) 2003 Windond Electronics Corp.
 * All rights reserved.
 *
 * $Workfile: NetBuf.h $
 *
 * Created by : 
 ******************************************************************************/
/*
 * $History: NetBuf.h $
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
 
#ifndef  _netbuf_h_
#define  _netbuf_h_
#include "cdefs.h"

//#define NET_BUF_SIZE   0x600
#define NET_BUF_SIZE   0x600   /* Note! NET_BUF_SIZE must be larger than
                                 the maximum packet size plus 8 and
                                 must be adjusted to multiple of 8 */

/* !! Note !!  sizeof(NETBUF) must be multiple of 4 */
typedef struct NETBUF
{
    UCHAR   packet[NET_BUF_SIZE-16];
    UINT16  status;
    UINT16  len;
    struct NETBUF	*txNext;	/* used by TX queue */
    struct NETBUF   *next;
    ULONG	reserved;	/* for 16 bytes alignment */
} 	NETBUF;


enum
{
    NBUF_STATUS_FREE=0,
    NBUF_STATUS_ALLOCATED
};

//#define NUMBER_OF_NETBUF  128
#define NUMBER_OF_NETBUF  64


#define Disable_GlobalInterrupt()  *(ULONG *)0x03FF4008 |= 0x00200000
#define Enable_GlobalInterrupt()   *(ULONG *)0x03FF4008 &= 0xFFDFFFFF

extern INT  _NetBufferAvailableCount;
extern NETBUF  *_iqueue_first, *_iqueue_last;   /* incoming queue */

extern INT  NetBuf_Init(VOID);
extern NETBUF *NetBuf_Allocate(VOID);
extern NETBUF *NetBuf_AllocateIR(VOID);
VOID NetBuf_Free(NETBUF *nbuf);
VOID NetBuf_FreeIR(NETBUF *nbuf);


#endif  /* _netbuf_h_ */
