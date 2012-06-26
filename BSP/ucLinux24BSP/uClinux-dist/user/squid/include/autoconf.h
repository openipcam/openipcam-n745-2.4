/* include/autoconf.h.  Generated automatically by configure.  */
/* include/autoconf.h.in.  Generated automatically from configure.in by autoheader.  */

/* Define if using alloca.c.  */
#define C_ALLOCA 1

/* Define to empty if the keyword does not work.  */
/* #undef const */

/* Define to one of _getb67, GETB67, getb67 for Cray-2 and Cray-YMP systems.
   This function is required for alloca.c support on those systems.  */
/* #undef CRAY_STACKSEG_END */

/* Define if you have alloca, as a function or macro.  */
/* #undef HAVE_ALLOCA */

/* Define if you have <alloca.h> and it should be used (not on Ultrix).  */
/* #undef HAVE_ALLOCA_H */

/* Define to `int' if <sys/types.h> doesn't define.  */
/* #undef mode_t */

/* Define to `long' if <sys/types.h> doesn't define.  */
/* #undef off_t */

/* Define to `int' if <sys/types.h> doesn't define.  */
/* #undef pid_t */

/* Define to `unsigned' if <sys/types.h> doesn't define.  */
/* #undef size_t */

/* If using the C implementation of alloca, define if you know the
   direction of stack growth for your system; otherwise it will be
   automatically deduced at run-time.
 STACK_DIRECTION > 0 => grows toward higher addresses
 STACK_DIRECTION < 0 => grows toward lower addresses
 STACK_DIRECTION = 0 => direction of growth unknown
 */
#define STACK_DIRECTION -1

/* Define if you have the ANSI C header files.  */
#define STDC_HEADERS 1

/* Define if your processor stores words with the most significant
   byte first (like Motorola and SPARC, unlike Intel and VAX).  */
#define WORDS_BIGENDIAN 1

/*********************************
 * START OF CONFIGURABLE OPTIONS *
 *********************************/
/*
 * If you are upset that the cachemgr.cgi form comes up with the hostname
 * field blank, then define this to getfullhostname()
 */
/* #undef CACHEMGR_HOSTNAME */

/* Define to do simple malloc debugging */
/* #undef XMALLOC_DEBUG */

/* Define for log file trace of mem alloc/free */
/* #undef MEM_GEN_TRACE */

/* Define to have malloc statistics */
/* #undef XMALLOC_STATISTICS */

/* Define to have a detailed trace of memory allocations */
/* #undef XMALLOC_TRACE */

/* #undef FORW_VIA_DB */

/* Define to use async disk I/O operations */
/* #undef USE_ASYNC_IO */

/* Defines how many threads to use for async I/O */
/* #undef NUMTHREADS */

/*
 * If you want to use Squid's ICMP features (highly recommended!) then
 * define this.  When USE_ICMP is defined, Squid will send ICMP pings
 * to origin server sites.  This information is used in numerous ways:
 *         - Sent in ICP replies so neighbor caches know how close
 *           you are to the source.
 *         - For finding the closest instance of a URN.
 *         - With the 'test_reachability' option.  Squid will return
 *           ICP_OP_MISS_NOFETCH for sites which it cannot ping.
 */
/* #undef USE_ICMP */

/*
 * Traffic management via "delay pools".
 */
/* #undef DELAY_POOLS */

/*
 * If you want to log User-Agent request header values, define this.
 * By default, they are written to useragent.log in the Squid log
 * directory.
 */
/* #undef USE_USERAGENT_LOG */

/*
 * A dangerous feature which causes Squid to kill its parent process
 * (presumably the RunCache script) upon receipt of SIGTERM or SIGINT.
 * Use with caution.
 */
/* #undef KILL_PARENT_OPT */

/* Define to enable SNMP monitoring of Squid */
/* #undef SQUID_SNMP */

/*
 * Define to enable WCCP
 */
#define USE_WCCP 1

/*
 * Squid frequently calls gettimeofday() for accurate timestamping.
 * If you are concerned that gettimeofday() is called too often, and
 * could be causing performance degradation, then you can define
 * ALARM_UPDATES_TIME and cause Squid's clock to be updated at regular
 * intervals (one second) with ALARM signals.
 */
/* #undef ALARM_UPDATES_TIME */

/*
 * Define this to include code which lets you specify access control
 * elements based on ethernet hardware addresses.  This code uses
 * functions found in 4.4 BSD derviations (e.g. FreeBSD, ?).
 */
/* #undef USE_ARP_ACL */

/*
 * Define this to include code for the Hypertext Cache Protocol (HTCP)
 */
/* #undef USE_HTCP */

/*
 * Use Cache Digests for locating objects in neighbor caches.  This
 * code is still semi-experimental. 
 */
/* #undef USE_CACHE_DIGESTS */

/*
 * Cache Array Routing Protocol
 */
/* #undef USE_CARP */

/* Define if struct tm has tm_gmtoff member */
/* #undef HAVE_TM_GMTOFF */

/* Define if struct mallinfo has mxfast member */
/* #undef HAVE_EXT_MALLINFO */

/* Default FD_SETSIZE value */
#define DEFAULT_FD_SETSIZE 256

/* Maximum number of open filedescriptors */
#define SQUID_MAXFD 256

/* UDP send buffer size */
#define SQUID_UDP_SO_SNDBUF 16384

/* UDP receive buffer size */
#define SQUID_UDP_SO_RCVBUF 16384

/* TCP send buffer size */
#define SQUID_TCP_SO_SNDBUF 16384

/* TCP receive buffer size */
#define SQUID_TCP_SO_RCVBUF 16384

/* Host type from configure */
#define CONFIG_HOST_TYPE "i686-pc-linux-gnu"

/* If we need to declare sys_errlist[] as external */
/* #undef NEED_SYS_ERRLIST */

/* If gettimeofday is known to take only one argument */
/* #undef GETTIMEOFDAY_NO_TZP */

/* If libresolv.a has been hacked to export _dns_ttl_ */
/* #undef LIBRESOLV_DNS_TTL_HACK */

/* Define if struct ip has ip_hl member */
#define HAVE_IP_HL 1

/* Define if your compiler supports prototyping */
#define HAVE_ANSI_PROTOTYPES 1

/* Define if we should use GNU regex */
#define USE_GNUREGEX 1

/* signed size_t, grr */
/* #undef ssize_t */

/*
 * Yay! Another Linux brokenness.  Its not good enough to know that
 * setresuid() exists, because RedHat 5.0 declare setresuid() but
 * doesn't implement it.
 */
/* #undef HAVE_SETRESUID */

/* Define if you have struct rusage */
#define HAVE_STRUCT_RUSAGE 1

/*
 * This makes warnings go away.  If you have socklen_t defined in your
 * /usr/include files, then this should remain undef'd.  Otherwise it
 * should be defined to int.
 */
/* #undef socklen_t */

/*
 * By default (for now anyway) Squid includes options which allows
 * the cache administrator to violate the HTTP protocol specification
 * in terms of cache behaviour.  Setting this to '0' will disable
 * such code.
 */
#define HTTP_VIOLATIONS 1

/*
 * Enable support for Transparent Proxy on systems using IP-Filter
 * address redirection. This provides "masquerading" support for non
 *  Linux system.
 */
/* #undef IPF_TRANSPARENT */

/*
 * Enable code for assiting in finding memory leaks.  Hacker stuff only.
 */
/* #undef USE_LEAKFINDER */

/*
 * type of fd_set array
 */
/* #undef fd_mask */

/*
 * If _res structure has nsaddr_list member
 */
#define HAVE_RES_NSADDR_LIST 1

/*
 * If _res structure has ns_list member
 */
/* #undef HAVE_RES_NS_LIST */

/*
 * Compile in support for Ident (RFC 931) lookups?  Enabled by default.
 */
#define USE_IDENT 1

/*
 * If your system has statvfs(), and if it actually works!
 */
/* #undef HAVE_STATVFS */

/*
 * If --disable-internal-dns was given to configure, then we'll use
 * the dnsserver processes instead.
 */
/* #undef USE_DNSSERVERS */

/*
 * we check for the existance of struct mallinfo
 */
/* #undef HAVE_STRUCT_MALLINFO */

/*
 * Do we want to use truncate(2) or unlink(2)?
 */
/* #undef USE_TRUNCATE */

/*
 * Allow underscores in host names
 */
/* #undef ALLOW_HOSTNAME_UNDERSCORES */

/*
 * Use the heap-based replacement techniques
 */
/* #undef HEAP_REPLACEMENT */

/* The number of bytes in a int.  */
#define SIZEOF_INT 4

/* The number of bytes in a long.  */
#define SIZEOF_LONG 4

/* The number of bytes in a void *.  */
#define SIZEOF_VOID_P 4

/* Define if you have the bcopy function.  */
#define HAVE_BCOPY 1

/* Define if you have the crypt function.  */
#define HAVE_CRYPT 1

/* Define if you have the drand48 function.  */
/* #undef HAVE_DRAND48 */

/* Define if you have the fchmod function.  */
#define HAVE_FCHMOD 1

/* Define if you have the getdtablesize function.  */
#define HAVE_GETDTABLESIZE 1

/* Define if you have the getpagesize function.  */
#define HAVE_GETPAGESIZE 1

/* Define if you have the getrlimit function.  */
#define HAVE_GETRLIMIT 1

/* Define if you have the getrusage function.  */
#define HAVE_GETRUSAGE 1

/* Define if you have the getspnam function.  */
#define HAVE_GETSPNAM 1

/* Define if you have the lrand48 function.  */
/* #undef HAVE_LRAND48 */

/* Define if you have the mallinfo function.  */
/* #undef HAVE_MALLINFO */

/* Define if you have the mallocblksize function.  */
/* #undef HAVE_MALLOCBLKSIZE */

/* Define if you have the mallopt function.  */
/* #undef HAVE_MALLOPT */

/* Define if you have the memcpy function.  */
#define HAVE_MEMCPY 1

/* Define if you have the memmove function.  */
#define HAVE_MEMMOVE 1

/* Define if you have the memset function.  */
#define HAVE_MEMSET 1

/* Define if you have the mktime function.  */
#define HAVE_MKTIME 1

/* Define if you have the mstats function.  */
/* #undef HAVE_MSTATS */

/* Define if you have the poll function.  */
#define HAVE_POLL 1

/* Define if you have the pthread_attr_setschedparam function.  */
/* #undef HAVE_PTHREAD_ATTR_SETSCHEDPARAM */

/* Define if you have the pthread_attr_setscope function.  */
/* #undef HAVE_PTHREAD_ATTR_SETSCOPE */

/* Define if you have the pthread_setschedparam function.  */
/* #undef HAVE_PTHREAD_SETSCHEDPARAM */

/* Define if you have the pthread_sigmask function.  */
/* #undef HAVE_PTHREAD_SIGMASK */

/* Define if you have the putenv function.  */
#define HAVE_PUTENV 1

/* Define if you have the random function.  */
#define HAVE_RANDOM 1

/* Define if you have the regcomp function.  */
/* #undef HAVE_REGCOMP */

/* Define if you have the regexec function.  */
/* #undef HAVE_REGEXEC */

/* Define if you have the regfree function.  */
/* #undef HAVE_REGFREE */

/* Define if you have the res_init function.  */
/* #undef HAVE_RES_INIT */

/* Define if you have the rint function.  */
/* #undef HAVE_RINT */

/* Define if you have the seteuid function.  */
#define HAVE_SETEUID 1

/* Define if you have the setgroups function.  */
#define HAVE_SETGROUPS 1

/* Define if you have the setpgrp function.  */
#define HAVE_SETPGRP 1

/* Define if you have the setrlimit function.  */
#define HAVE_SETRLIMIT 1

/* Define if you have the setsid function.  */
#define HAVE_SETSID 1

/* Define if you have the sigaction function.  */
#define HAVE_SIGACTION 1

/* Define if you have the snprintf function.  */
#define HAVE_SNPRINTF 1

/* Define if you have the srand48 function.  */
/* #undef HAVE_SRAND48 */

/* Define if you have the srandom function.  */
#define HAVE_SRANDOM 1

/* Define if you have the strerror function.  */
#define HAVE_STRERROR 1

/* Define if you have the sysconf function.  */
#define HAVE_SYSCONF 1

/* Define if you have the syslog function.  */
#define HAVE_SYSLOG 1

/* Define if you have the tempnam function.  */
/* #undef HAVE_TEMPNAM */

/* Define if you have the timegm function.  */
/* #undef HAVE_TIMEGM */

/* Define if you have the vsnprintf function.  */
#define HAVE_VSNPRINTF 1

/* Define if you have the <arpa/inet.h> header file.  */
#define HAVE_ARPA_INET_H 1

/* Define if you have the <arpa/nameser.h> header file.  */
/* #undef HAVE_ARPA_NAMESER_H */

/* Define if you have the <assert.h> header file.  */
#define HAVE_ASSERT_H 1

/* Define if you have the <bstring.h> header file.  */
/* #undef HAVE_BSTRING_H */

/* Define if you have the <config.h> header file.  */
/* #undef HAVE_CONFIG_H */

/* Define if you have the <crypt.h> header file.  */
#define HAVE_CRYPT_H 1

/* Define if you have the <ctype.h> header file.  */
#define HAVE_CTYPE_H 1

/* Define if you have the <dirent.h> header file.  */
#define HAVE_DIRENT_H 1

/* Define if you have the <errno.h> header file.  */
#define HAVE_ERRNO_H 1

/* Define if you have the <fcntl.h> header file.  */
#define HAVE_FCNTL_H 1

/* Define if you have the <getopt.h> header file.  */
#define HAVE_GETOPT_H 1

/* Define if you have the <gnumalloc.h> header file.  */
/* #undef HAVE_GNUMALLOC_H */

/* Define if you have the <grp.h> header file.  */
#define HAVE_GRP_H 1

/* Define if you have the <ip_compat.h> header file.  */
/* #undef HAVE_IP_COMPAT_H */

/* Define if you have the <ip_fil.h> header file.  */
/* #undef HAVE_IP_FIL_H */

/* Define if you have the <ip_fil_compat.h> header file.  */
/* #undef HAVE_IP_FIL_COMPAT_H */

/* Define if you have the <ip_nat.h> header file.  */
/* #undef HAVE_IP_NAT_H */

/* Define if you have the <libc.h> header file.  */
/* #undef HAVE_LIBC_H */

/* Define if you have the <limits.h> header file.  */
#define HAVE_LIMITS_H 1

/* Define if you have the <malloc.h> header file.  */
#define HAVE_MALLOC_H 1

/* Define if you have the <math.h> header file.  */
#define HAVE_MATH_H 1

/* Define if you have the <memory.h> header file.  */
#define HAVE_MEMORY_H 1

/* Define if you have the <mount.h> header file.  */
/* #undef HAVE_MOUNT_H */

/* Define if you have the <ndir.h> header file.  */
/* #undef HAVE_NDIR_H */

/* Define if you have the <net/if.h> header file.  */
#define HAVE_NET_IF_H 1

/* Define if you have the <netdb.h> header file.  */
#define HAVE_NETDB_H 1

/* Define if you have the <netinet/if_ether.h> header file.  */
#define HAVE_NETINET_IF_ETHER_H 1

/* Define if you have the <netinet/in.h> header file.  */
#define HAVE_NETINET_IN_H 1

/* Define if you have the <netinet/ip_compat.h> header file.  */
/* #undef HAVE_NETINET_IP_COMPAT_H */

/* Define if you have the <netinet/ip_fil.h> header file.  */
/* #undef HAVE_NETINET_IP_FIL_H */

/* Define if you have the <netinet/ip_fil_compat.h> header file.  */
/* #undef HAVE_NETINET_IP_FIL_COMPAT_H */

/* Define if you have the <netinet/ip_nat.h> header file.  */
/* #undef HAVE_NETINET_IP_NAT_H */

/* Define if you have the <netinet/tcp.h> header file.  */
#define HAVE_NETINET_TCP_H 1

/* Define if you have the <poll.h> header file.  */
/* #undef HAVE_POLL_H */

/* Define if you have the <pwd.h> header file.  */
#define HAVE_PWD_H 1

/* Define if you have the <regex.h> header file.  */
/* #undef HAVE_REGEX_H */

/* Define if you have the <resolv.h> header file.  */
#define HAVE_RESOLV_H 1

/* Define if you have the <sched.h> header file.  */
/* #undef HAVE_SCHED_H */

/* Define if you have the <signal.h> header file.  */
#define HAVE_SIGNAL_H 1

/* Define if you have the <stdarg.h> header file.  */
#define HAVE_STDARG_H 1

/* Define if you have the <stddef.h> header file.  */
#define HAVE_STDDEF_H 1

/* Define if you have the <stdio.h> header file.  */
#define HAVE_STDIO_H 1

/* Define if you have the <stdlib.h> header file.  */
#define HAVE_STDLIB_H 1

/* Define if you have the <string.h> header file.  */
#define HAVE_STRING_H 1

/* Define if you have the <strings.h> header file.  */
#define HAVE_STRINGS_H 1

/* Define if you have the <sys/dir.h> header file.  */
/* #undef HAVE_SYS_DIR_H */

/* Define if you have the <sys/file.h> header file.  */
#define HAVE_SYS_FILE_H 1

/* Define if you have the <sys/ioctl.h> header file.  */
#define HAVE_SYS_IOCTL_H 1

/* Define if you have the <sys/ndir.h> header file.  */
/* #undef HAVE_SYS_NDIR_H */

/* Define if you have the <sys/param.h> header file.  */
#define HAVE_SYS_PARAM_H 1

/* Define if you have the <sys/resource.h> header file.  */
#define HAVE_SYS_RESOURCE_H 1

/* Define if you have the <sys/select.h> header file.  */
#ifndef __UC_LIBC__
#define HAVE_SYS_SELECT_H 1
#endif

/* Define if you have the <sys/socket.h> header file.  */
#define HAVE_SYS_SOCKET_H 1

/* Define if you have the <sys/stat.h> header file.  */
#define HAVE_SYS_STAT_H 1

/* Define if you have the <sys/statvfs.h> header file.  */
/* #undef HAVE_SYS_STATVFS_H */

/* Define if you have the <sys/syscall.h> header file.  */
#define HAVE_SYS_SYSCALL_H 1

/* Define if you have the <sys/time.h> header file.  */
#define HAVE_SYS_TIME_H 1

/* Define if you have the <sys/types.h> header file.  */
#define HAVE_SYS_TYPES_H 1

/* Define if you have the <sys/un.h> header file.  */
#define HAVE_SYS_UN_H 1

/* Define if you have the <sys/vfs.h> header file.  */
#define HAVE_SYS_VFS_H 1

/* Define if you have the <sys/wait.h> header file.  */
#define HAVE_SYS_WAIT_H 1

/* Define if you have the <syslog.h> header file.  */
#define HAVE_SYSLOG_H 1

/* Define if you have the <time.h> header file.  */
#define HAVE_TIME_H 1

/* Define if you have the <unistd.h> header file.  */
#define HAVE_UNISTD_H 1

/* Define if you have the <varargs.h> header file.  */
#define HAVE_VARARGS_H 1

/* Define if you have the 44bsd library (-l44bsd).  */
/* #undef HAVE_LIB44BSD */

/* Define if you have the bind library (-lbind).  */
/* #undef HAVE_LIBBIND */

/* Define if you have the bsd library (-lbsd).  */
/* #undef HAVE_LIBBSD */

/* Define if you have the crypt library (-lcrypt).  */
#define HAVE_LIBCRYPT 1

/* Define if you have the gnumalloc library (-lgnumalloc).  */
/* #undef HAVE_LIBGNUMALLOC */

/* Define if you have the intl library (-lintl).  */
/* #undef HAVE_LIBINTL */

/* Define if you have the m library (-lm).  */
/* #undef HAVE_LIBM */

/* Define if you have the malloc library (-lmalloc).  */
/* #undef HAVE_LIBMALLOC */

/* Define if you have the nsl library (-lnsl).  */
/* #undef HAVE_LIBNSL */

/* Define if you have the pthread library (-lpthread).  */
/* #undef HAVE_LIBPTHREAD */

/* Define if you have the resolv library (-lresolv).  */
/* #undef HAVE_LIBRESOLV */

/* Define if you have the socket library (-lsocket).  */
/* #undef HAVE_LIBSOCKET */
