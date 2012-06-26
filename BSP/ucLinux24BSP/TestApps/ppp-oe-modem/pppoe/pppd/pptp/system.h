#ifndef SYSTEM_H
#define SYSTEM_H

#include <asm/unistd.h>

#ifdef NEW_KERNEL //lsshi add

#define __NR_nat_spec_reset             (__NR_SYSCALL_BASE+222)//lsshi add
#define __NR_set_out_ip         (__NR_SYSCALL_BASE+223)//lsshi add
#define __NR_nat_url_filter_reset               (__NR_SYSCALL_BASE+224)//lsshi add
#define __NR_nat_ip_filter_reset        (__NR_SYSCALL_BASE+225)//lsshi add
#define __NR_set_local_ip         (__NR_SYSCALL_BASE+226)//lsshi add

#define __NR_set_pptp   (__NR_SYSCALL_BASE+235)
#else

#define __NR_nat_spec_reset             (__NR_SYSCALL_BASE+218)//lsshi add
#define __NR_set_out_ip         (__NR_SYSCALL_BASE+219)//lsshi add
#define __NR_nat_url_filter_reset               (__NR_SYSCALL_BASE+220)//lsshi add
#define __NR_nat_ip_filter_reset        (__NR_SYSCALL_BASE+221)//lsshi add
#define __NR_set_local_ip         (__NR_SYSCALL_BASE+222)//lsshi add
#endif

#endif

