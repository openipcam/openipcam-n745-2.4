/*
 * asm/arch-W90N740/irq.h:
 * 2003 shirley yu
 */

#ifndef __ASM_ARCH_IRQ_H__
#define __ASM_ARCH_IRQ_H__

#include <asm/hardware.h>
#include <asm/io.h>
#include <asm/mach/irq.h>
#include <asm/arch/irqs.h>

#define fixup_irq(x) (x)

extern void w90n740_mask_irq(unsigned int irq);
extern void w90n740_unmask_irq(unsigned int irq);
extern void w90n740_mask_ack_irq(unsigned int irq);
extern void w90n740_int_init(void);

static __inline__ void irq_init_irq(void)
{
	unsigned long flags;
	int irq;

	save_flags_cli(flags);
	w90n740_int_init();
	restore_flags(flags);

	for (irq = 0; irq < NR_IRQS; irq++) {
		irq_desc[irq].valid	= 1;
		irq_desc[irq].probe_ok	= 1;
		irq_desc[irq].mask_ack	= w90n740_mask_ack_irq;
		irq_desc[irq].mask	= w90n740_mask_irq;
		irq_desc[irq].unmask	= w90n740_unmask_irq;
	}
}
#endif /* __ASM_ARCH_IRQ_H__ */
