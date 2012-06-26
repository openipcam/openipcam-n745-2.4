/****************************************************************************
 * 
 * Copyright (c) 2004 - 2006 Winbond Electronics Corp. All rights reserved. 
 *
 ****************************************************************************/
 
/****************************************************************************
 * 
 * FILENAME
 *     w90n745_i2c.h
 *
 * VERSION
 *     1.0
 *
 * DESCRIPTION
 *	Winbond W90N745 I2C Driver header
 *
 * DATA STRUCTURES
 *		None
 *
 * FUNCTIONS
 *		None
 *
 * HISTORY
 *	2005/05/20		Ver 1.0 Created by PC34 QFu
 *
 * REMARK
 *     None
 *************************************************************************/
#ifndef _W90N745_I2C_H_
#define _W90N745_I2C_H_

#include <linux/types.h>
#include <linux/ioctl.h>

#include <asm/arch/irqs.h>

#ifdef __KERNEL__

#define I2C_NUMBER				2

#define I2C_FIFO_LEN			4
#define I2C_MAX_BUF_LEN		450

#define I2C0_IRQ					INT_I2C0
#define I2C1_IRQ					INT_I2C1

#define I2C_MAJOR				89
#define I2C0_IO_BASE			0xfff86000
#define I2C1_IO_BASE			0xfff86100
#define I2C_IOMEM_SIZE			0x18

#define REG_GPIO_CFG5			0xFFF83050
#define I2C_INPUT_CLOCK			80000		/* 80 000 KHz */

/* register map */
#define CSR						0x00
#define DIVIDER					0x04
#define CMDR						0x08
#define SWR						0x0c	/* not available in W90n710 */
#define RXR						0x10
#define TXR						0x14

/* bit map in CMDR */
#define I2C_CMD_START			0x10
#define I2C_CMD_STOP			0x08
#define I2C_CMD_READ			0x04
#define I2C_CMD_WRITE			0x02
#define I2C_CMD_NACK			0x01

/* for transfer use */
#define I2C_WRITE				0x00
#define I2C_READ				0x01


#define I2C_STATE_NOP			0x00
#define I2C_STATE_READ			0x01
#define I2C_STATE_WRITE		0x02
#define I2C_STATE_PROBE		0x03

typedef struct _i2c_dev{
	int no;		/* i2c bus number */
	volatile int state;
	int last_error;
	int addr;

	unsigned subaddr;
	int subaddr_len;

	unsigned char buffer[I2C_MAX_BUF_LEN];
	volatile unsigned int pos, len;

	wait_queue_head_t wq;

}i2c_dev;

#endif

struct sub_address{
	char sub_addr_len;
	unsigned int sub_addr;
};

/* error code */
#define I2C_ERR_ID							(0x00)

#define I2C_ERR_NOERROR					(0x00)
#define I2C_ERR_LOSTARBITRATION			(0x01 | I2C_ERR_ID)
#define I2C_ERR_BUSBUSY					(0x02 | I2C_ERR_ID)
#define I2C_ERR_NACK						(0x03 | I2C_ERR_ID)	/* data transfer error */
#define I2C_ERR_SLAVENACK					(0x04 | I2C_ERR_ID)	/* slave not respond after address */

/* define ioctl command */
#define I2C_IOC_MAGIC				'i'

#define I2C_IOC_MAXNR				3

#define I2C_IOC_SET_DEV_ADDRESS	_IOW(I2C_IOC_MAGIC, 0, int)
#define I2C_IOC_SET_SUB_ADDRESS	_IOW(I2C_IOC_MAGIC, 1, int)
#define I2C_IOC_SET_SPEED			_IOW(I2C_IOC_MAGIC, 2, int)
#define I2C_IOC_GET_LAST_ERROR		_IOR(I2C_IOC_MAGIC, 3, int)
#endif

