/****************************************************************************
 * 
 * Copyright (c) 2004 - 2006 Winbond Electronics Corp. All rights reserved. 
 *
 ****************************************************************************/
 
/****************************************************************************
 * 
 * FILENAME
 *     w90n745_usi.h
 *
 * VERSION
 *     1.0
 *
 * DESCRIPTION
 *     USI driver supported for W90n710.
 *
 * FUNCTIONS
 *	all functions, if they has return value, return 0 if they success, others failed.
 *
 * HISTORY
 *	2006/01/10		Created by QFu
 *
 * REMARK
 *     None
 *************************************************************************/
#ifndef _W90N745_USI_H_
#define _W90N745_USI_H_

#include <linux/types.h>
#include <linux/ioctl.h>

#define USI_BASE		0xfff86200
#define USI_SIZE			0x30

#define USI_CNTRL		(USI_BASE | 0x00)
#define USI_DIVIDER		(USI_BASE | 0x04)
#define USI_SSR			(USI_BASE | 0x08)

#define USI_RX0			(USI_BASE | 0x10)
#define USI_RX1			(USI_BASE | 0x14)
#define USI_RX2			(USI_BASE | 0x18)
#define USI_RX3			(USI_BASE | 0x1c)

#define USI_TX0			(USI_BASE | 0x10)
#define USI_TX1			(USI_BASE | 0x14)
#define USI_TX2			(USI_BASE | 0x18)
#define USI_TX3			(USI_BASE | 0x1c)

struct usi_parameter{
	unsigned int active_level:1;
	unsigned int lsb:1, tx_neg:1, rx_neg:1, divider:16;
	unsigned int sleep:4;
};

struct usi_data{
	unsigned int write_data;
	unsigned int read_data;
	unsigned int bit_len;
};

#define USI_MAJOR		231

#define USI_IOC_MAGIC			'u'
#define USI_IOC_MAXNR			3

#define USI_IOC_GETPARAMETER	_IOR(USI_IOC_MAGIC, 0, struct usi_parameter *)
#define USI_IOC_SETPARAMETER	_IOW(USI_IOC_MAGIC, 1, struct usi_parameter *)
#define USI_IOC_SELECTSLAVE	_IOW(USI_IOC_MAGIC, 2, int)
#define USI_IOC_TRANSIT			_IOW(USI_IOC_MAGIC, 3, struct usi_data *)

#endif /* _W90N745_USI_H_ */

