#ifndef _BIB_H
#define _BIB_H
#include <asm/arch/cdefs.h>
//---------------------------------------------------------------------------
typedef struct _t_bootloaer_info
{
	UINT32 length;
	UINT32 type;
	char mac0[6];
	char ip0[6];
	char mac1[6];
	char ip1[6];
	UINT32 cache;
	UINT32 dhcp;
	UINT32 net_mac;
	UINT32 phy;
	UINT32 buf_base;
	UINT32 buf_size;
} tbl_info;

#define BOOTLOADER_INFO	0x1

extern int _dhcp;


//---------------------------------------------------------------------------
#endif
