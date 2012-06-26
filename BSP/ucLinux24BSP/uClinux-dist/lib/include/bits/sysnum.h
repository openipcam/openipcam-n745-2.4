/* WARNING!!! AUTO-GENERATED FILE!!! DO NOT EDIT!!! */

#ifndef _BITS_SYSNUM_H
#define _BITS_SYSNUM_H

#ifndef _SYSCALL_H
# error "Never use <bits/sysnum.h> directly; include <sys/syscall.h> instead."
#endif

#undef __NR_SYSCALL_BASE
#define __NR_SYSCALL_BASE 0x900000
#define SYS_SYSCALL_BASE __NR_SYSCALL_BASE
#undef __NR_exit
#define __NR_exit (0x900000 + 1)
#define SYS_exit __NR_exit
#undef __NR_fork
#define __NR_fork (0x900000 + 2)
#define SYS_fork __NR_fork
#undef __NR_read
#define __NR_read (0x900000 + 3)
#define SYS_read __NR_read
#undef __NR_write
#define __NR_write (0x900000 + 4)
#define SYS_write __NR_write
#undef __NR_open
#define __NR_open (0x900000 + 5)
#define SYS_open __NR_open
#undef __NR_close
#define __NR_close (0x900000 + 6)
#define SYS_close __NR_close
#undef __NR_waitpid
#define __NR_waitpid (0x900000 + 7)
#define SYS_waitpid __NR_waitpid
#undef __NR_creat
#define __NR_creat (0x900000 + 8)
#define SYS_creat __NR_creat
#undef __NR_link
#define __NR_link (0x900000 + 9)
#define SYS_link __NR_link
#undef __NR_unlink
#define __NR_unlink (0x900000 + 10)
#define SYS_unlink __NR_unlink
#undef __NR_execve
#define __NR_execve (0x900000 + 11)
#define SYS_execve __NR_execve
#undef __NR_chdir
#define __NR_chdir (0x900000 + 12)
#define SYS_chdir __NR_chdir
#undef __NR_time
#define __NR_time (0x900000 + 13)
#define SYS_time __NR_time
#undef __NR_mknod
#define __NR_mknod (0x900000 + 14)
#define SYS_mknod __NR_mknod
#undef __NR_chmod
#define __NR_chmod (0x900000 + 15)
#define SYS_chmod __NR_chmod
#undef __NR_lchown
#define __NR_lchown (0x900000 + 16)
#define SYS_lchown __NR_lchown
#undef __NR_break
#define __NR_break (0x900000 + 17)
#define SYS_break __NR_break
#undef __NR_lseek
#define __NR_lseek (0x900000 + 19)
#define SYS_lseek __NR_lseek
#undef __NR_getpid
#define __NR_getpid (0x900000 + 20)
#define SYS_getpid __NR_getpid
#undef __NR_mount
#define __NR_mount (0x900000 + 21)
#define SYS_mount __NR_mount
#undef __NR_umount
#define __NR_umount (0x900000 + 22)
#define SYS_umount __NR_umount
#undef __NR_setuid
#define __NR_setuid (0x900000 + 23)
#define SYS_setuid __NR_setuid
#undef __NR_getuid
#define __NR_getuid (0x900000 + 24)
#define SYS_getuid __NR_getuid
#undef __NR_stime
#define __NR_stime (0x900000 + 25)
#define SYS_stime __NR_stime
#undef __NR_ptrace
#define __NR_ptrace (0x900000 + 26)
#define SYS_ptrace __NR_ptrace
#undef __NR_alarm
#define __NR_alarm (0x900000 + 27)
#define SYS_alarm __NR_alarm
#undef __NR_pause
#define __NR_pause (0x900000 + 29)
#define SYS_pause __NR_pause
#undef __NR_utime
#define __NR_utime (0x900000 + 30)
#define SYS_utime __NR_utime
#undef __NR_stty
#define __NR_stty (0x900000 + 31)
#define SYS_stty __NR_stty
#undef __NR_gtty
#define __NR_gtty (0x900000 + 32)
#define SYS_gtty __NR_gtty
#undef __NR_access
#define __NR_access (0x900000 + 33)
#define SYS_access __NR_access
#undef __NR_nice
#define __NR_nice (0x900000 + 34)
#define SYS_nice __NR_nice
#undef __NR_ftime
#define __NR_ftime (0x900000 + 35)
#define SYS_ftime __NR_ftime
#undef __NR_sync
#define __NR_sync (0x900000 + 36)
#define SYS_sync __NR_sync
#undef __NR_kill
#define __NR_kill (0x900000 + 37)
#define SYS_kill __NR_kill
#undef __NR_rename
#define __NR_rename (0x900000 + 38)
#define SYS_rename __NR_rename
#undef __NR_mkdir
#define __NR_mkdir (0x900000 + 39)
#define SYS_mkdir __NR_mkdir
#undef __NR_rmdir
#define __NR_rmdir (0x900000 + 40)
#define SYS_rmdir __NR_rmdir
#undef __NR_dup
#define __NR_dup (0x900000 + 41)
#define SYS_dup __NR_dup
#undef __NR_pipe
#define __NR_pipe (0x900000 + 42)
#define SYS_pipe __NR_pipe
#undef __NR_times
#define __NR_times (0x900000 + 43)
#define SYS_times __NR_times
#undef __NR_prof
#define __NR_prof (0x900000 + 44)
#define SYS_prof __NR_prof
#undef __NR_brk
#define __NR_brk (0x900000 + 45)
#define SYS_brk __NR_brk
#undef __NR_setgid
#define __NR_setgid (0x900000 + 46)
#define SYS_setgid __NR_setgid
#undef __NR_getgid
#define __NR_getgid (0x900000 + 47)
#define SYS_getgid __NR_getgid
#undef __NR_signal
#define __NR_signal (0x900000 + 48)
#define SYS_signal __NR_signal
#undef __NR_geteuid
#define __NR_geteuid (0x900000 + 49)
#define SYS_geteuid __NR_geteuid
#undef __NR_getegid
#define __NR_getegid (0x900000 + 50)
#define SYS_getegid __NR_getegid
#undef __NR_acct
#define __NR_acct (0x900000 + 51)
#define SYS_acct __NR_acct
#undef __NR_umount2
#define __NR_umount2 (0x900000 + 52)
#define SYS_umount2 __NR_umount2
#undef __NR_lock
#define __NR_lock (0x900000 + 53)
#define SYS_lock __NR_lock
#undef __NR_ioctl
#define __NR_ioctl (0x900000 + 54)
#define SYS_ioctl __NR_ioctl
#undef __NR_fcntl
#define __NR_fcntl (0x900000 + 55)
#define SYS_fcntl __NR_fcntl
#undef __NR_mpx
#define __NR_mpx (0x900000 + 56)
#define SYS_mpx __NR_mpx
#undef __NR_setpgid
#define __NR_setpgid (0x900000 + 57)
#define SYS_setpgid __NR_setpgid
#undef __NR_ulimit
#define __NR_ulimit (0x900000 + 58)
#define SYS_ulimit __NR_ulimit
#undef __NR_umask
#define __NR_umask (0x900000 + 60)
#define SYS_umask __NR_umask
#undef __NR_chroot
#define __NR_chroot (0x900000 + 61)
#define SYS_chroot __NR_chroot
#undef __NR_ustat
#define __NR_ustat (0x900000 + 62)
#define SYS_ustat __NR_ustat
#undef __NR_dup2
#define __NR_dup2 (0x900000 + 63)
#define SYS_dup2 __NR_dup2
#undef __NR_getppid
#define __NR_getppid (0x900000 + 64)
#define SYS_getppid __NR_getppid
#undef __NR_getpgrp
#define __NR_getpgrp (0x900000 + 65)
#define SYS_getpgrp __NR_getpgrp
#undef __NR_setsid
#define __NR_setsid (0x900000 + 66)
#define SYS_setsid __NR_setsid
#undef __NR_sigaction
#define __NR_sigaction (0x900000 + 67)
#define SYS_sigaction __NR_sigaction
#undef __NR_sgetmask
#define __NR_sgetmask (0x900000 + 68)
#define SYS_sgetmask __NR_sgetmask
#undef __NR_ssetmask
#define __NR_ssetmask (0x900000 + 69)
#define SYS_ssetmask __NR_ssetmask
#undef __NR_setreuid
#define __NR_setreuid (0x900000 + 70)
#define SYS_setreuid __NR_setreuid
#undef __NR_setregid
#define __NR_setregid (0x900000 + 71)
#define SYS_setregid __NR_setregid
#undef __NR_sigsuspend
#define __NR_sigsuspend (0x900000 + 72)
#define SYS_sigsuspend __NR_sigsuspend
#undef __NR_sigpending
#define __NR_sigpending (0x900000 + 73)
#define SYS_sigpending __NR_sigpending
#undef __NR_sethostname
#define __NR_sethostname (0x900000 + 74)
#define SYS_sethostname __NR_sethostname
#undef __NR_setrlimit
#define __NR_setrlimit (0x900000 + 75)
#define SYS_setrlimit __NR_setrlimit
#undef __NR_getrlimit
#define __NR_getrlimit (0x900000 + 76)
#define SYS_getrlimit __NR_getrlimit
#undef __NR_getrusage
#define __NR_getrusage (0x900000 + 77)
#define SYS_getrusage __NR_getrusage
#undef __NR_gettimeofday
#define __NR_gettimeofday (0x900000 + 78)
#define SYS_gettimeofday __NR_gettimeofday
#undef __NR_settimeofday
#define __NR_settimeofday (0x900000 + 79)
#define SYS_settimeofday __NR_settimeofday
#undef __NR_getgroups
#define __NR_getgroups (0x900000 + 80)
#define SYS_getgroups __NR_getgroups
#undef __NR_setgroups
#define __NR_setgroups (0x900000 + 81)
#define SYS_setgroups __NR_setgroups
#undef __NR_select
#define __NR_select (0x900000 + 82)
#define SYS_select __NR_select
#undef __NR_symlink
#define __NR_symlink (0x900000 + 83)
#define SYS_symlink __NR_symlink
#undef __NR_readlink
#define __NR_readlink (0x900000 + 85)
#define SYS_readlink __NR_readlink
#undef __NR_uselib
#define __NR_uselib (0x900000 + 86)
#define SYS_uselib __NR_uselib
#undef __NR_swapon
#define __NR_swapon (0x900000 + 87)
#define SYS_swapon __NR_swapon
#undef __NR_reboot
#define __NR_reboot (0x900000 + 88)
#define SYS_reboot __NR_reboot
#undef __NR_readdir
#define __NR_readdir (0x900000 + 89)
#define SYS_readdir __NR_readdir
#undef __NR_mmap
#define __NR_mmap (0x900000 + 90)
#define SYS_mmap __NR_mmap
#undef __NR_munmap
#define __NR_munmap (0x900000 + 91)
#define SYS_munmap __NR_munmap
#undef __NR_truncate
#define __NR_truncate (0x900000 + 92)
#define SYS_truncate __NR_truncate
#undef __NR_ftruncate
#define __NR_ftruncate (0x900000 + 93)
#define SYS_ftruncate __NR_ftruncate
#undef __NR_fchmod
#define __NR_fchmod (0x900000 + 94)
#define SYS_fchmod __NR_fchmod
#undef __NR_fchown
#define __NR_fchown (0x900000 + 95)
#define SYS_fchown __NR_fchown
#undef __NR_getpriority
#define __NR_getpriority (0x900000 + 96)
#define SYS_getpriority __NR_getpriority
#undef __NR_setpriority
#define __NR_setpriority (0x900000 + 97)
#define SYS_setpriority __NR_setpriority
#undef __NR_profil
#define __NR_profil (0x900000 + 98)
#define SYS_profil __NR_profil
#undef __NR_statfs
#define __NR_statfs (0x900000 + 99)
#define SYS_statfs __NR_statfs
#undef __NR_fstatfs
#define __NR_fstatfs (0x900000 +100)
#define SYS_fstatfs __NR_fstatfs
#undef __NR_ioperm
#define __NR_ioperm (0x900000 +101)
#define SYS_ioperm __NR_ioperm
#undef __NR_socketcall
#define __NR_socketcall (0x900000 +102)
#define SYS_socketcall __NR_socketcall
#undef __NR_syslog
#define __NR_syslog (0x900000 +103)
#define SYS_syslog __NR_syslog
#undef __NR_setitimer
#define __NR_setitimer (0x900000 +104)
#define SYS_setitimer __NR_setitimer
#undef __NR_getitimer
#define __NR_getitimer (0x900000 +105)
#define SYS_getitimer __NR_getitimer
#undef __NR_stat
#define __NR_stat (0x900000 +106)
#define SYS_stat __NR_stat
#undef __NR_lstat
#define __NR_lstat (0x900000 +107)
#define SYS_lstat __NR_lstat
#undef __NR_fstat
#define __NR_fstat (0x900000 +108)
#define SYS_fstat __NR_fstat
#undef __NR_vhangup
#define __NR_vhangup (0x900000 +111)
#define SYS_vhangup __NR_vhangup
#undef __NR_idle
#define __NR_idle (0x900000 +112)
#define SYS_idle __NR_idle
#undef __NR_syscall
#define __NR_syscall (0x900000 +113)
#define SYS_syscall __NR_syscall
#undef __NR_wait4
#define __NR_wait4 (0x900000 +114)
#define SYS_wait4 __NR_wait4
#undef __NR_swapoff
#define __NR_swapoff (0x900000 +115)
#define SYS_swapoff __NR_swapoff
#undef __NR_sysinfo
#define __NR_sysinfo (0x900000 +116)
#define SYS_sysinfo __NR_sysinfo
#undef __NR_ipc
#define __NR_ipc (0x900000 +117)
#define SYS_ipc __NR_ipc
#undef __NR_fsync
#define __NR_fsync (0x900000 +118)
#define SYS_fsync __NR_fsync
#undef __NR_sigreturn
#define __NR_sigreturn (0x900000 +119)
#define SYS_sigreturn __NR_sigreturn
#undef __NR_clone
#define __NR_clone (0x900000 +120)
#define SYS_clone __NR_clone
#undef __NR_setdomainname
#define __NR_setdomainname (0x900000 +121)
#define SYS_setdomainname __NR_setdomainname
#undef __NR_uname
#define __NR_uname (0x900000 +122)
#define SYS_uname __NR_uname
#undef __NR_modify_ldt
#define __NR_modify_ldt (0x900000 +123)
#define SYS_modify_ldt __NR_modify_ldt
#undef __NR_adjtimex
#define __NR_adjtimex (0x900000 +124)
#define SYS_adjtimex __NR_adjtimex
#undef __NR_mprotect
#define __NR_mprotect (0x900000 +125)
#define SYS_mprotect __NR_mprotect
#undef __NR_sigprocmask
#define __NR_sigprocmask (0x900000 +126)
#define SYS_sigprocmask __NR_sigprocmask
#undef __NR_create_module
#define __NR_create_module (0x900000 +127)
#define SYS_create_module __NR_create_module
#undef __NR_init_module
#define __NR_init_module (0x900000 +128)
#define SYS_init_module __NR_init_module
#undef __NR_delete_module
#define __NR_delete_module (0x900000 +129)
#define SYS_delete_module __NR_delete_module
#undef __NR_get_kernel_syms
#define __NR_get_kernel_syms (0x900000 +130)
#define SYS_get_kernel_syms __NR_get_kernel_syms
#undef __NR_quotactl
#define __NR_quotactl (0x900000 +131)
#define SYS_quotactl __NR_quotactl
#undef __NR_getpgid
#define __NR_getpgid (0x900000 +132)
#define SYS_getpgid __NR_getpgid
#undef __NR_fchdir
#define __NR_fchdir (0x900000 +133)
#define SYS_fchdir __NR_fchdir
#undef __NR_bdflush
#define __NR_bdflush (0x900000 +134)
#define SYS_bdflush __NR_bdflush
#undef __NR_sysfs
#define __NR_sysfs (0x900000 +135)
#define SYS_sysfs __NR_sysfs
#undef __NR_personality
#define __NR_personality (0x900000 +136)
#define SYS_personality __NR_personality
#undef __NR_afs_syscall
#define __NR_afs_syscall (0x900000 +137)
#define SYS_afs_syscall __NR_afs_syscall
#undef __NR_setfsuid
#define __NR_setfsuid (0x900000 +138)
#define SYS_setfsuid __NR_setfsuid
#undef __NR_setfsgid
#define __NR_setfsgid (0x900000 +139)
#define SYS_setfsgid __NR_setfsgid
#undef __NR__llseek
#define __NR__llseek (0x900000 +140)
#define SYS__llseek __NR__llseek
#undef __NR_getdents
#define __NR_getdents (0x900000 +141)
#define SYS_getdents __NR_getdents
#undef __NR__newselect
#define __NR__newselect (0x900000 +142)
#define SYS__newselect __NR__newselect
#undef __NR_flock
#define __NR_flock (0x900000 +143)
#define SYS_flock __NR_flock
#undef __NR_msync
#define __NR_msync (0x900000 +144)
#define SYS_msync __NR_msync
#undef __NR_readv
#define __NR_readv (0x900000 +145)
#define SYS_readv __NR_readv
#undef __NR_writev
#define __NR_writev (0x900000 +146)
#define SYS_writev __NR_writev
#undef __NR_getsid
#define __NR_getsid (0x900000 +147)
#define SYS_getsid __NR_getsid
#undef __NR_fdatasync
#define __NR_fdatasync (0x900000 +148)
#define SYS_fdatasync __NR_fdatasync
#undef __NR__sysctl
#define __NR__sysctl (0x900000 +149)
#define SYS__sysctl __NR__sysctl
#undef __NR_mlock
#define __NR_mlock (0x900000 +150)
#define SYS_mlock __NR_mlock
#undef __NR_munlock
#define __NR_munlock (0x900000 +151)
#define SYS_munlock __NR_munlock
#undef __NR_mlockall
#define __NR_mlockall (0x900000 +152)
#define SYS_mlockall __NR_mlockall
#undef __NR_munlockall
#define __NR_munlockall (0x900000 +153)
#define SYS_munlockall __NR_munlockall
#undef __NR_sched_setparam
#define __NR_sched_setparam (0x900000 +154)
#define SYS_sched_setparam __NR_sched_setparam
#undef __NR_sched_getparam
#define __NR_sched_getparam (0x900000 +155)
#define SYS_sched_getparam __NR_sched_getparam
#undef __NR_sched_setscheduler
#define __NR_sched_setscheduler (0x900000 +156)
#define SYS_sched_setscheduler __NR_sched_setscheduler
#undef __NR_sched_getscheduler
#define __NR_sched_getscheduler (0x900000 +157)
#define SYS_sched_getscheduler __NR_sched_getscheduler
#undef __NR_sched_yield
#define __NR_sched_yield (0x900000 +158)
#define SYS_sched_yield __NR_sched_yield
#undef __NR_sched_get_priority_max
#define __NR_sched_get_priority_max (0x900000 +159)
#define SYS_sched_get_priority_max __NR_sched_get_priority_max
#undef __NR_sched_get_priority_min
#define __NR_sched_get_priority_min (0x900000 +160)
#define SYS_sched_get_priority_min __NR_sched_get_priority_min
#undef __NR_sched_rr_get_interval
#define __NR_sched_rr_get_interval (0x900000 +161)
#define SYS_sched_rr_get_interval __NR_sched_rr_get_interval
#undef __NR_nanosleep
#define __NR_nanosleep (0x900000 +162)
#define SYS_nanosleep __NR_nanosleep
#undef __NR_mremap
#define __NR_mremap (0x900000 +163)
#define SYS_mremap __NR_mremap
#undef __NR_setresuid
#define __NR_setresuid (0x900000 +164)
#define SYS_setresuid __NR_setresuid
#undef __NR_getresuid
#define __NR_getresuid (0x900000 +165)
#define SYS_getresuid __NR_getresuid
#undef __NR_vm86
#define __NR_vm86 (0x900000 +166)
#define SYS_vm86 __NR_vm86
#undef __NR_query_module
#define __NR_query_module (0x900000 +167)
#define SYS_query_module __NR_query_module
#undef __NR_poll
#define __NR_poll (0x900000 +168)
#define SYS_poll __NR_poll
#undef __NR_nfsservctl
#define __NR_nfsservctl (0x900000 +169)
#define SYS_nfsservctl __NR_nfsservctl
#undef __NR_setresgid
#define __NR_setresgid (0x900000 +170)
#define SYS_setresgid __NR_setresgid
#undef __NR_getresgid
#define __NR_getresgid (0x900000 +171)
#define SYS_getresgid __NR_getresgid
#undef __NR_prctl
#define __NR_prctl (0x900000 +172)
#define SYS_prctl __NR_prctl
#undef __NR_rt_sigreturn
#define __NR_rt_sigreturn (0x900000 +173)
#define SYS_rt_sigreturn __NR_rt_sigreturn
#undef __NR_rt_sigaction
#define __NR_rt_sigaction (0x900000 +174)
#define SYS_rt_sigaction __NR_rt_sigaction
#undef __NR_rt_sigprocmask
#define __NR_rt_sigprocmask (0x900000 +175)
#define SYS_rt_sigprocmask __NR_rt_sigprocmask
#undef __NR_rt_sigpending
#define __NR_rt_sigpending (0x900000 +176)
#define SYS_rt_sigpending __NR_rt_sigpending
#undef __NR_rt_sigtimedwait
#define __NR_rt_sigtimedwait (0x900000 +177)
#define SYS_rt_sigtimedwait __NR_rt_sigtimedwait
#undef __NR_rt_sigqueueinfo
#define __NR_rt_sigqueueinfo (0x900000 +178)
#define SYS_rt_sigqueueinfo __NR_rt_sigqueueinfo
#undef __NR_rt_sigsuspend
#define __NR_rt_sigsuspend (0x900000 +179)
#define SYS_rt_sigsuspend __NR_rt_sigsuspend
#undef __NR_pread
#define __NR_pread (0x900000 +180)
#define SYS_pread __NR_pread
#undef __NR_pwrite
#define __NR_pwrite (0x900000 +181)
#define SYS_pwrite __NR_pwrite
#undef __NR_chown
#define __NR_chown (0x900000 +182)
#define SYS_chown __NR_chown
#undef __NR_getcwd
#define __NR_getcwd (0x900000 +183)
#define SYS_getcwd __NR_getcwd
#undef __NR_capget
#define __NR_capget (0x900000 +184)
#define SYS_capget __NR_capget
#undef __NR_capset
#define __NR_capset (0x900000 +185)
#define SYS_capset __NR_capset
#undef __NR_sigaltstack
#define __NR_sigaltstack (0x900000 +186)
#define SYS_sigaltstack __NR_sigaltstack
#undef __NR_sendfile
#define __NR_sendfile (0x900000 +187)
#define SYS_sendfile __NR_sendfile
#undef __NR_vfork
#define __NR_vfork (0x900000 +190)
#define SYS_vfork __NR_vfork
#undef __NR_ugetrlimit
#define __NR_ugetrlimit (0x900000 +191)
#define SYS_ugetrlimit __NR_ugetrlimit
#undef __NR_mmap2
#define __NR_mmap2 (0x900000 +192)
#define SYS_mmap2 __NR_mmap2
#undef __NR_truncate64
#define __NR_truncate64 (0x900000 +193)
#define SYS_truncate64 __NR_truncate64
#undef __NR_ftruncate64
#define __NR_ftruncate64 (0x900000 +194)
#define SYS_ftruncate64 __NR_ftruncate64
#undef __NR_stat64
#define __NR_stat64 (0x900000 +195)
#define SYS_stat64 __NR_stat64
#undef __NR_lstat64
#define __NR_lstat64 (0x900000 +196)
#define SYS_lstat64 __NR_lstat64
#undef __NR_fstat64
#define __NR_fstat64 (0x900000 +197)
#define SYS_fstat64 __NR_fstat64
#undef __NR_lchown32
#define __NR_lchown32 (0x900000 +198)
#define SYS_lchown32 __NR_lchown32
#undef __NR_getuid32
#define __NR_getuid32 (0x900000 +199)
#define SYS_getuid32 __NR_getuid32
#undef __NR_getgid32
#define __NR_getgid32 (0x900000 +200)
#define SYS_getgid32 __NR_getgid32
#undef __NR_geteuid32
#define __NR_geteuid32 (0x900000 +201)
#define SYS_geteuid32 __NR_geteuid32
#undef __NR_getegid32
#define __NR_getegid32 (0x900000 +202)
#define SYS_getegid32 __NR_getegid32
#undef __NR_setreuid32
#define __NR_setreuid32 (0x900000 +203)
#define SYS_setreuid32 __NR_setreuid32
#undef __NR_setregid32
#define __NR_setregid32 (0x900000 +204)
#define SYS_setregid32 __NR_setregid32
#undef __NR_getgroups32
#define __NR_getgroups32 (0x900000 +205)
#define SYS_getgroups32 __NR_getgroups32
#undef __NR_setgroups32
#define __NR_setgroups32 (0x900000 +206)
#define SYS_setgroups32 __NR_setgroups32
#undef __NR_fchown32
#define __NR_fchown32 (0x900000 +207)
#define SYS_fchown32 __NR_fchown32
#undef __NR_setresuid32
#define __NR_setresuid32 (0x900000 +208)
#define SYS_setresuid32 __NR_setresuid32
#undef __NR_getresuid32
#define __NR_getresuid32 (0x900000 +209)
#define SYS_getresuid32 __NR_getresuid32
#undef __NR_setresgid32
#define __NR_setresgid32 (0x900000 +210)
#define SYS_setresgid32 __NR_setresgid32
#undef __NR_getresgid32
#define __NR_getresgid32 (0x900000 +211)
#define SYS_getresgid32 __NR_getresgid32
#undef __NR_chown32
#define __NR_chown32 (0x900000 +212)
#define SYS_chown32 __NR_chown32
#undef __NR_setuid32
#define __NR_setuid32 (0x900000 +213)
#define SYS_setuid32 __NR_setuid32
#undef __NR_setgid32
#define __NR_setgid32 (0x900000 +214)
#define SYS_setgid32 __NR_setgid32
#undef __NR_setfsuid32
#define __NR_setfsuid32 (0x900000 +215)
#define SYS_setfsuid32 __NR_setfsuid32
#undef __NR_setfsgid32
#define __NR_setfsgid32 (0x900000 +216)
#define SYS_setfsgid32 __NR_setfsgid32
#undef __NR_getdents64
#define __NR_getdents64 (0x900000 +217)
#define SYS_getdents64 __NR_getdents64
#undef __NR_pivot_root
#define __NR_pivot_root (0x900000 +218)
#define SYS_pivot_root __NR_pivot_root
#undef __NR_mincore
#define __NR_mincore (0x900000 +219)
#define SYS_mincore __NR_mincore
#undef __NR_madvise
#define __NR_madvise (0x900000 +220)
#define SYS_madvise __NR_madvise
#undef __NR_fcntl64
#define __NR_fcntl64 (0x900000 +221)
#define SYS_fcntl64 __NR_fcntl64

#endif
