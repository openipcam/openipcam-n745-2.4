/******************************************************************************
 *
 * Copyright (c) 2007 Windond Electronics Corp.
 * All rights reserved.
 *
 * $Workfile: cfi.h $
 *
 * 
 ******************************************************************************/
#ifndef __CFI_H__
#define __CFI_H__

#define SELECT_QUERY_MODE(_base_)      do{*(unsigned short volatile *)(((_base_) & 0xFFF00000) + (0x55 << 1)) = 0x98;}while(0)
#define DESELECT_QUERY_MODE(_base_)    do{*(unsigned short volatile *)((_base_) & 0xFFF00000) = 0xF0;\
                                          *(unsigned short volatile *)((_base_) & 0xFFF00000) = 0xFF;}while(0)

#define CFI_READ(_base_, _offset_, _var_)     do{_var_ = *(unsigned short volatile *)(((_base_) & 0xFFF00000) + ((_offset_) << 1));}while(0)
#define CFI_WRITE(_base_, _offset_, _var_)     do{*(unsigned short volatile *)(((_base_) /*& 0xFFF00000*/) + ((_offset_) << 1)) = (_var_);}while(0)

#define AMD_CMD_SET      0x0002
#define INTEL_CMD_SET    0x0003

struct cfi_erase_block_region_info {
	unsigned int size;
	unsigned int num;
};


struct cfi_command{
	int (*write) (unsigned int address, unsigned char *data, unsigned int size);
	int (*erase) (unsigned int address, unsigned int size);
};

extern unsigned int cfiGetBlockSize(unsigned int address);
extern unsigned int cfiGetFlashSize(void);
extern int cfiGetFlashInfo(void);
extern struct cfi_command cfiCmd;

#endif // #ifdef __CFI_H__
