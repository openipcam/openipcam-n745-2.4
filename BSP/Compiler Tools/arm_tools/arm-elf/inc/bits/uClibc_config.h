/*
 * Automatically generated C config: don't edit
 */
#if !defined __FEATURES_H && !defined __need_uClibc_config_h
#error Never include <bits/uClibc_config.h> directly; use <features.h> instead.
#endif
#define AUTOCONF_INCLUDED

/*
 * Version Number
 */
#define __UCLIBC_MAJOR__ 0
#define __UCLIBC_MINOR__ 9
#define __UCLIBC_SUBLEVEL__ 19

/*
 * Target Architecture Features and Options
 */
#define __HAVE_ELF__ 1
#define __CONFIG_GENERIC_ARM__ 1
#undef __CONFIG_ARM7TDMI__
#undef __CONFIG_STRONGARM__
#undef __CONFIG_XSCALE__
#undef __UCLIBC_HAS_MMU__
#undef __UCLIBC_HAS_FLOATS__
#define __WARNINGS__ "-Wall"
#define __KERNEL_SOURCE__ "$(ROOTDIR)/$(LINUXDIR)/."
#define __UCLIBC_UCLINUX_BROKEN_MUNMAP__ 1
#define __EXCLUDE_BRK__ 1
#define __C_SYMBOL_PREFIX__ ""
#define __HAVE_DOT_CONFIG__ 1

/*
 * General Library Settings
 */
#undef __DOPIC__
#define __UCLIBC_CTOR_DTOR__ 1
#define __UCLIBC_HAS_THREADS__ 1
#define __UCLIBC_HAS_LFS__ 1
#define __MALLOC__ 1
#undef __MALLOC_930716__
#define __UCLIBC_DYNAMIC_ATEXIT__ 1
#undef __HAS_SHADOW__
#define __UCLIBC_HAS_REGEX__ 1
#undef __UNIX98PTY_ONLY__
#define __ASSUME_DEVPTS__ 1

/*
 * Networking Support
 */
#undef __UCLIBC_HAS_IPV6__
#define __UCLIBC_HAS_RPC__ 1
#undef __UCLIBC_HAS_FULL_RPC__

/*
 * String and Stdio Support
 */
#undef __UCLIBC_HAS_WCHAR__
#undef __USE_OLD_VFPRINTF__

/*
 * Library Installation Options
 */
#define __DEVEL_PREFIX__ "/usr/$(TARGET_ARCH)-linux-uclibc"
#define __SYSTEM_DEVEL_PREFIX__ "$(DEVEL_PREFIX)"
#define __DEVEL_TOOL_PREFIX__ "$(DEVEL_PREFIX)/usr"

/*
 * uClibc hacking options
 */
#undef __DODEBUG__
#undef __DOASSERTS__
#undef __UCLIBC_MALLOC_DEBUGGING__
