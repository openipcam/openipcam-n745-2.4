/*
 * linux/include/asm/arch-W90N740/serial.h
 * 2003 winbond
 */
#ifndef __ASM_ARCH_SERIAL_H
#define __ASM_ARCH_SERIAL_H

#include <asm/arch/hardware.h>
#include <asm/irq.h>

#define RS_TABLE_SIZE	1
#define BASE_BAUD	115200
#define STD_COM_FLAGS	(ASYNC_BOOT_AUTOCONF | ASYNC_SKIP_TEST)
#define STD_SERIAL_PORT_DEFNS		\
	/* UART CLK PORT IRQ FLAGS */	\
	{ 0, BASE_BAUD, UART_BASE0, INT_UARTINT, STD_COM_FLAGS },	/* ttyS0 */	
#define EXTRA_SERIAL_PORT_DEFNS

#define COM_TX			(Base_Addr+0x80000)
#define COM_RX			(Base_Addr+0x80000)
#define COM_DLL 		(Base_Addr+0x80000)
#define COM_DLM 		(Base_Addr+0x80004)
#define COM_IER 		(Base_Addr+0x80004)
#define COM_IIR 		(Base_Addr+0x80008)
#define COM_FCR 		(Base_Addr+0x80008)
#define COM_LCR 		(Base_Addr+0x8000c)
#define COM_MCR 		(Base_Addr+0x80010)
#define COM_LSR 		(Base_Addr+0x80014)
#define COM_MSR 		(Base_Addr+0x80018)
#define COM_TOR 		(Base_Addr+0x8001c)

#define UART_LSR_OE	0x02		// Overrun error
#define UART_LSR_PE	0x04		// Parity error
#define UART_LSR_FE	0x08		// Frame error
#define UART_LSR_BI	0x10		// Break detect
//#define UART_LSR_DTR	0x10		// Data terminal ready
#define UART_LSR_DR 0x01
#define UART_LSR_THRE 0x20
#define UART_IIR_DR	0x04		// Receive data ready
#define UART_IIR_THRE	0x02		// Transmit buffer register empty
#define UART_LSR_TEMT	0x40		// Transmit complete

#define UART_LCR_WLEN5	0x00
#define UART_LCR_WLEN6	0x01
#define UART_LCR_WLEN7	0x02
#define UART_LCR_WLEN8	0x03
#define UART_LCR_PARITY	0x08
#define UART_LCR_NPAR	0x00
#define UART_LCR_OPAR	0x00
#define UART_LCR_EPAR	0x10
#define UART_LCR_SPAR	0x20
#define UART_LCR_SBC	0x40
#define UART_LCR_NSB	0x04

#define UART_GCR_RX_INT	0x01
#define UART_GCR_TX_INT	0x08
#define UART_GCR_RX_STAT_INT	0x04

#define UART_IER_MSI
#define UART_IER_RLSI	0x04
#define UART_IER_THRI	0x02
#define UART_IER_RDI	0x01
	
#define UART_MSR		0
#define UART_MSR_DCD		0
#define UART_MSR_RI		0
#define UART_MSR_DSR		0
#define UART_MSR_CTS		0
#define UART_MSR_DDCD		0
#define UART_MSR_TERI		0
#define UART_MSR_DDSR		0
#define UART_MSR_DCTS		0
#define UART_MSR_ANY_DELTA	0

#define PORT_W90N740	14

struct serial_baudtable
{
	unsigned int baudrate;
	unsigned int div;
};

struct serial_baudtable uart_baudrate[] =
{
	{  1200, 0x30B},
	{  2400, 0x184},
	{  4800, 0xC1},
	{  9600, 0x5F},
	{ 19200, 0x2E},
	{ 38400, 0x16},
	{ 57600, 0x0E},
	{115200, 0x06},
	{230400, 0x02},
	{460860, 0x00}
};

unsigned int baudrate_div(unsigned int baudrate)
{
	int i;
	int len = sizeof(uart_baudrate)/sizeof(struct serial_baudtable);
	for(i = 0; i < len; i++)
		if(uart_baudrate[i].baudrate == baudrate)
			return uart_baudrate[i].div;
	return 0;
}

#define disable_uart_tx_interrupt(line)		\
{						\
	if(line) {				\
	}					\
	else {					\
		CSR_WRITE(COM_IER, CSR_READ(COM_IER)&0x1D);	\
	}					\
}

#define disable_uart_rx_interrupt(line)		\
{						\
	if(line) {				\
	}					\
	else {					\
		CSR_WRITE(COM_IER, CSR_READ(COM_IER)&0x1E);	\
	}					\
}

#define enable_uart_tx_interrupt(line)		\
{						\
	if(line) {				\
	}					\
	else {					\
		if(!(CSR_READ(COM_IER)&0x02))	\
			CSR_WRITE(COM_IER, CSR_READ(COM_IER)|0x02);	\
	}					\
}

#define enable_uart_rx_interrupt(line)		\
{						\
	if(line) {				\
	}					\
	else {					\
		CSR_WRITE(COM_IER, CSR_READ(COM_IER)|0x1);	\
	}					\
}

#endif /* __ASM_ARCH_SERIAL_H */
