/*
 * asm/arch-W90P710/irqs.h:
 * PC34 Lsshi
 */
#ifndef __ASM_ARCH_IRQS_H__
#define __ASM_ARCH_IRQS_H__
#define NR_IRQS		32
//#define VALID_IRQ(i)	(i<=10 ||(i>=13 && i<NR_IRQS))
#define VALID_IRQ(i)	(i<NR_IRQS)

#define INT_WDTINT		1 	/* Watch Dog Timer Interrupt */

#define INT_nIRQ0		2 	/* External Interrupt 0 */
#define INT_nIRQ1		3 	/* External Interrupt 1 */
#define INT_nIRQ2		4 	/* External Interrupt 2 */
#define INT_nIRQ3		5 	/* External Interrupt 3 */

#define INT_AC97		6 	/* AC97 Interrupt */
#define INT_LCD			7 	/* LCD Controller Interrupt */
#define INT_RTC			8 	/* RTC Controller Interrupt */

#define INT_UARTINT		9 	/* UART 0 Interrupt */
#define INT_UART1		10 	/* UART 1 Interrupt */
#define INT_UART2		11	/* UART 2 Interrupt */
#define INT_UART3		12 	/* UART 3 Interrupt */

#define INT_TINT0		13 	/* Timer Interrupt 0 */
#define INT_TINT1		14	/* Timer Interrupt 1 */


#define INT_USBINT0		15 /* USB Host Interrupt 0 */
#define INT_USBINT1		16 /* USB Host Interrupt 1 */


#define INT_EMCTXINT0	17 /* EMC TX Interrupt 0 */
#define INT_EMCRXINT0	18 /* EMC RX Interrupt 0 */
#define INT_EMCTXINT1	17 /* EMC TX Interrupt 1 */ /* for debugging */
#define INT_EMCRXINT1	18 /* EMC RX Interrupt 1 */ /* for debugging */

#define INT_GDMAINT0	19 /* GDMA Channel Interrupt 0 */
#define INT_GDMAINT1	20 /* GDMA Channel Interrupt 1 */

#define INT_SDIO		21 	/* SDIO Interrupt */
#define INT_USBD		22 	/* USB Device Interrupt */
#define INT_SC0			23 	/* SmartCard Interrupt 0 */
#define INT_SC1			24 	/* SmartCard Interrupt 1 */
#define INT_I2C0		25 	/* I2C Interrupt 0 */
#define INT_I2C1		26 	/* I2C Interrupt 1 */
#define INT_SPI			27 	/* SPI Interrupt */
#define INT_PWM			28 	/* PWM Timer Interrupt */
#define INT_KEYPAD		29 	/* Keypad Interrupt */
#define INT_PS2			30 	/* PS2 Interrupt */

#define INT_Reserved0	31
//#define INT_Reserved1	12

//#define INT_IIC		20
//#define INT_GLOBAL	31

#define IRQ_TIMER	INT_TINT0


#endif /* __ASM_ARCH_IRQS_H__ */
