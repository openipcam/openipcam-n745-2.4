/*
 * asm/arch-W90N740/irqs.h:
 * Shirley yu
 */
#ifndef __ASM_ARCH_IRQS_H__
#define __ASM_ARCH_IRQS_H__
#define NR_IRQS		19
#define VALID_IRQ(i)	(i<=10 ||(i>=13 && i<NR_IRQS))

#define INT_WDTINT	1
#define INT_nIRQ0	2
#define INT_nIRQ1	3
#define INT_nIRQ2	4
#define INT_nIRQ3	5
#define INT_UARTINT	6
#define INT_TINT0	7
#define INT_TINT1	8
#define INT_USBINT0	9
#define INT_USBINT1	10
#define INT_Reserved0	11
#define INT_Reserved1	12
#define INT_EMCTXINT0	13
#define INT_EMCTXINT1	14
#define INT_EMCRXINT0	15
#define INT_EMCRXINT1	16
#define INT_GDMAINT0	17
#define INT_GDMAINT1	18
#define INT_IIC		20
#define INT_GLOBAL	21

#define IRQ_TIMER	INT_TINT0

#endif /* __ASM_ARCH_IRQS_H__ */
