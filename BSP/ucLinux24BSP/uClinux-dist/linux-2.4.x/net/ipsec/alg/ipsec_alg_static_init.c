#include <linux/kernel.h>
#include "../ipsec_alg.h"
extern int ipsec_aes_init(void);
void ipsec_alg_static_init(void){ 
	int __attribute__ ((unused)) err=0;
	if ((err=ipsec_aes_init()) < 0)
		printk(KERN_WARNING "ipsec_aes_init() returned %d", err);
}
