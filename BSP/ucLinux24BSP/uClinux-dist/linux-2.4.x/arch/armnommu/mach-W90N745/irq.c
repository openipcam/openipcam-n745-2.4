/*
*  linux/arch/armnommu/mach-W90N745/irq.c
*  2003 clyu <clyu2@winbond.com.tw>
*/
#include <linux/init.h>

#include <asm/mach/irq.h>
#include <asm/hardware.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/system.h>

void W90N745_mask_irq(unsigned int irq)
{
	INT_DISABLE(irq);
}

void W90N745_unmask_irq(unsigned int irq)
{
	INT_ENABLE(irq);
}

void W90N745_mask_ack_irq(unsigned int irq)
{
	INT_DISABLE(irq);
}

void W90N745_int_init()
{
	//int i=0;
	//IntPend = 0x1FFFFF;
	CSR_WRITE(AIC_MDCR,0xFFFFFFFF);
	CSR_WRITE(AIC_SCR9,0x41);
	CSR_WRITE(AIC_SCR13,0x41);
	//for(i=6;i<=18;i++)
	//	IntScr(i,0x41);
	//IntMode = INT_MODE_IRQ;
	//INT_ENABLE(INT_GLOBAL);
}
