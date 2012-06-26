#include <linux/config.h>
#include <linux/kernel.h>
#include <linux/blk.h>
#include <linux/init.h>
#include <linux/spinlock.h>
#include <linux/errno.h>
#include <linux/ioport.h>
#include <linux/tqueue.h>
#include <asm/hardware.h>
#include <asm/bitops.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/system.h>
#include <asm/uaccess.h>
#include <linux/ioport.h>

#ifdef MODULE
#include <linux/module.h>
#endif

#include "tsi_doc.h"

#ifdef MODULE
MODULE_PARM(doc_phys_addr,"l");
MODULE_PARM_DESC(doc_phys_addr, "Physical Address for DiskOnChip");
#endif

static unsigned long __initdata	doc_phys_addr = DOC_PHYS_ADDR; /* DOCPHYSADDR; */

static void *vaddr_doc = NULL;

#ifdef COTULLA
#define MSC2_CNTR_REG_ADDR 0x48000000
#define MSC2_OFFSET        0x10
#define MCS5_INIT_VAL      0xFFF80000

void cotulla_CS5_init( void )
{
  volatile void *myMSC2ptr;
  unsigned long msc2Val;
  
  myMSC2ptr = (volatile void *)ioremap_nocache(MSC2_CNTR_REG_ADDR, 0x1000);
  /* MSC2 Static Memory Control Register - 16-bit mode for CS5 */
  msc2Val = readl( myMSC2ptr + MSC2_OFFSET );
  writel( ((msc2Val & 0x0000FFFF) | MCS5_INIT_VAL), myMSC2ptr + MSC2_OFFSET );
/*  *(myMCS5 + 4) = MCS5_INIT_VAL; */
  iounmap( myMSC2ptr ); 
}
#endif

int tsi_request_doc_iomem()
{
#ifdef COTULLA
  cotulla_CS5_init(); 
#endif
#ifdef TECHSOL
  if( request_region( doc_phys_addr, DOC_PHYS_SIZE, "DiskOnChip" ) ) {
#endif  
    vaddr_doc = (void *)ioremap_nocache(doc_phys_addr, DOC_PHYS_SIZE);
    return 1;
#ifdef TECHSOL    
  }
  else
    return 0;
#endif
}

int tsi_release_doc_iomem()
{
  if( vaddr_doc ) {
    iounmap( vaddr_doc );
#ifndef COTULLA    
    release_region( doc_phys_addr, DOC_PHYS_SIZE );
#endif    
  }
  return 1;
}

void *tsi_get_doc_vaddr()
{
  return vaddr_doc;
}

